#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define bufflen 1024

using namespace std;

int transfer (int src_sock, int des_sock) {
    char buff[bufflen];
    int len = bufflen;
    int getlen = 0, sendlen = 0;

    if ( (getlen = recv (src_sock, buff, len, 0)) <= 0) {
        perror ("recv error");
        return -1;
    }

    if ( (sendlen = send (des_sock, buff, getlen, 0)) <= 0) {
    perror ("send error");
        return -1;
    }

    cout << "get:" << getlen << "  send:" << sendlen << endl;

         return sendlen;
}

int build_conn (int sock1, int sock2) {
    int tep = fork();
    if (tep < 0) perror ("fork error");
    else if (!tep) { //child
        while (1) transfer (sock1, sock2);
    } else {
        while (1) transfer (sock2, sock1);
    }


    return 0;
}

int main() {
    int sockfd, clientfd;

    struct sockaddr_in my_addr, client_addr;

    if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("couldn't build socket..");
        exit (-1);
    }

    cout << "get socket:" << sockfd << endl;

    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_port = htons (2002);   /* short, network byte order */
    my_addr.sin_addr.s_addr = htons (INADDR_ANY);

    bzero (& (my_addr.sin_zero), sizeof (my_addr.sin_zero));       /* zero the rest of the struct */


    if (bind (sockfd, (struct sockaddr*) &my_addr, sizeof (struct sockaddr)) == -1) {
        perror ("bind error:");
        exit (-1);
    }

    listen (sockfd, 20);

    size_t tep = sizeof (struct sockaddr);

    cout << "waiting to accept.."<<endl;

    if ( (clientfd = accept (sockfd, (struct sockaddr*) &client_addr, (socklen_t*) &tep)) == -1) {
        perror ("accept error..");
        exit (-1);
    }

    cout << "client ip:" << inet_ntoa (client_addr.sin_addr) << "port :" << ntohs (client_addr.sin_port);
    /////////////////////////////////////////////////////up is server/////////

    int client2fd;
    struct sockaddr_in server_addr;

    if ( (client2fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("couldn't build socket..");
        exit (-1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (22);   /* short, network byte order */
    server_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");

    bzero (& (server_addr.sin_zero), sizeof (server_addr.sin_zero));       /* zero the rest of the struct */

    cout<<"connecting to "<<inet_ntoa(server_addr.sin_addr)<<":"<<ntohs(server_addr.sin_port)<<endl;
    if (connect (client2fd, (struct sockaddr*) &server_addr, sizeof (struct sockaddr))  == -1) {
        perror ("server connect ssh server error..");
        exit (-1);
    }
    cout<<"connected"<<endl;


    /////////////////////////////////////////////////////up is client/////////
    build_conn (clientfd, client2fd);


    char p[] = "hello from server..";
    //cout<<sizeof(p)<<endl;
    int sd = 0;

    while (1) {

        if ( (sd = send (clientfd, (void*) p, sizeof (p), 0)) != sizeof (p)) {
            cout << "send error " << sd << "/" << sizeof (p) << endl;
        }

        cout << "send once" << sizeof (p) << endl;

        sleep (2);
    }




    return 0;
}
