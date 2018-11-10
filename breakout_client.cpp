#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>

using namespace std;

int main() {
    int sockfd;

    struct sockaddr_in server_addr;

    if ( ( sockfd = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
        perror ( "couldn't build socket.." );
        exit ( -1 );
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons ( 22 ); /* short, network byte order */
    server_addr.sin_addr.s_addr = inet_addr ( "127.0.0.1 " );

    bzero ( & ( server_addr.sin_zero ), sizeof ( server_addr.sin_zero ) ); /* zero the rest of the struct */


    ////////////////////////////////test bind/////////////
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET; /* host byte order */

    bindaddr.sin_port = htons ( 2002 ); /* short, network byte order */
    bindaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//htons ( INADDR_ANY );


    bzero ( & ( bindaddr.sin_zero ), sizeof ( bindaddr.sin_zero ) ); /* zero the rest of the struct */

    int flag = 1;
    setsockopt ( sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&flag,  sizeof(flag));

    if ( bind ( sockfd, ( struct sockaddr* ) &bindaddr, sizeof ( struct sockaddr ) ) == -1 ) {
        perror ( "bind error:" );
        exit ( -1 );
    }


    //////////////////////////////////////////////////////
    if ( connect ( sockfd, ( struct sockaddr* ) &server_addr, sizeof ( struct sockaddr ) )  == -1 ) {
        perror ( "connect error.." );
        exit ( -1 );
    }

    char buff[1024] = {0};
    int len = sizeof ( buff );

    while ( 1 ) {
        cout << "recving bytes;" << recv ( sockfd, buff, len, 0 ) << "  " << buff << endl;
    }


    return 0;
}
