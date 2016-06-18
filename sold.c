/*
 * Sleep on LAN
 *
 * execute a script on magic packets
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define PORT 9
#define BUFLEN 512

char *default_interface = "eth0";
const char *config = "/etc/default/sold";
const char *script = "/etc/sold.sh";
int up = 1;

int main( )
{
  struct sockaddr_in addr;
  int sd, i;
  unsigned char buf[BUFLEN];
  struct ifreq iface;
  size_t len;
  int magic;
  pid_t pid;
  int status;
  FILE *fh;
  char line[128];
  char interface[16];
  char *t;

  strncpy( interface, default_interface, sizeof( interface ));
  interface[ sizeof( interface ) - 1 ] = '\0';

  if(( fh = fopen( config, "r" )) != NULL )
  {
    while( fgets( line, sizeof(line), fh ) != NULL )
    {
      line[ sizeof( line ) - 1 ] = '\0';
      /*printf( "line: %s", line );*/
      if( sscanf( line, "INTERFACE=%ms", &t ) > 0 )
      {
        strncpy( interface, t, sizeof( interface ));
        interface[ sizeof( interface ) - 1 ] = '\0';
        free( t );
        break;
      }
    }
    fclose( fh );
  }
  printf( "interface: '%s'\n", interface );

  if(( sd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP )) == -1 )
  {
    perror( "error opening udp socket" );
    exit( 1 );
  }

  strcpy( iface.ifr_name, interface );
  if( ioctl( sd, SIOCGIFHWADDR, &iface ) != 0 )
  {
    perror( "error getting mac address" );
    close( sd );
    exit( 1 );
  }

  /*printf( "Mac address of %s: ", interface );*/
  /*for( i = 0; i < 6; i++ )*/
    /*printf( "%02x", (unsigned char) iface.ifr_addr.sa_data[i] );*/
  /*putchar( '\n' );*/

  bzero( &addr, sizeof( addr ));
  addr.sin_family = AF_INET;
  addr.sin_port = htons( PORT );
  addr.sin_addr.s_addr = htonl( INADDR_ANY );

  if( bind( sd, (struct sockaddr *) &addr, sizeof( addr )) == -1 )
  {
    perror( "error binding udp socket" );
    close( sd );
    exit( 1 );
  }

  while( up )
  {
    len = recvfrom( sd, buf, BUFLEN, 0, 0, 0 );
    if( len < 0 )
    {
      perror( "error reveiving data" );
      break;
    }
    if( len < 12 )
      continue;

    magic = 1;
    for( i = 0; i < 6; i++ )
      if( buf[i] != 0xFF )
      {
        magic = 0;
        break;
      }
    if( !magic )
      continue;
    for( i = 0; i < 6; i++ )
      if( buf[i+6] != (unsigned char) iface.ifr_addr.sa_data[i] )
      {
        magic = 0;
        break;
      }
    if( !magic )
      continue;

    pid = fork( );
    if( pid == 0 ) // child
    {
      execl( script, "", NULL );
    }
    else // parent
    {
      wait( &status );
    }
  }

  close( sd );
  return 0;
}

