#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include<netinet/tcp.h>

#include <vector>
#include <map>
#include <algorithm>

#define bufflen 2048


#define serverport 7001
#define serverport_end 7050


#define server_configport 7000

using namespace std;

typedef struct client_info {
    int sockfd;
    struct sockaddr_in addr;
} client_info;

typedef vector<client_info>::iterator vec_client_iter;
typedef map<int, vector<int> >::iterator map_iv_iter;
typedef map<int, struct sockaddr_in>::iterator map_iaddr_iter;

enum ops {connect_clients, disconnect_clients, showips_clients};



void signal_handler (int signo) { //處理殭屍進程
    if (signo == SIGCHLD) {
        pid_t pid;
        while ( (pid = waitpid (-1, NULL, WNOHANG)) > 0) {//WNOHANG 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若结束，则返回该子进程的ID。
            cout << ("SIGCHLD pid ") << pid << endl;
        }
    }
}


int buildserver (int port) {
    int sockfd;

    struct sockaddr_in my_addr;

    if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("couldn't build socket..");
        exit (-1);
    }

    cout << "\nget socket:" << sockfd << endl;

    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_port = htons (port);   /* short, network byte order */
    my_addr.sin_addr.s_addr = htons (INADDR_ANY);

    bzero (& (my_addr.sin_zero), sizeof (my_addr.sin_zero));       /* zero the rest of the struct */

    int optval = 1;
    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval)) < 0) {
        perror ("setsockopt1 error");
        //exit(-1);
    }

    if (bind (sockfd, (struct sockaddr*) &my_addr, sizeof (struct sockaddr)) == -1) {
        perror ("bind error:");
        exit (-1);
    }

    listen (sockfd, 5);
    cout << "listening in port:" << port << endl;
    return sockfd;
}


int transfer (int src_sock, int to_sock) {
    /*
    one node tranfer to those who connect to this node
    */
    char buff[bufflen];
    int len = bufflen;
    int i;
    int getlen = 0, sendlen = 0;

    if ( (getlen = recv (src_sock, buff, len, 0)) <= 0) {
        perror ("transfer:recv error");

        return -1;
    }

    if ( (sendlen = send (to_sock, buff, getlen, 0)) <= 0) {
        perror ("transfer:send error ");
        return -1;
    }

    //cout << "transfer:get:" << getlen << "  send :" << sendlen << endl;

    return sendlen;
}


int build_conn (int sock1, int sock2) {
    /*
    build up the connects in sockfd_reflects
    */

    int tep = fork();
    if (tep < 0) {
        perror ("fork error");
        //exit(-1);
        return -1;
    } else if (!tep) { //child

        while (transfer (sock1, sock2) != -1);
        cout<<"build_conn child1 out"<<endl;
        exit (-1);
    } 
    cout<<"build_conn:build child1 process pid:"<<tep<<endl;

    int tep2=fork();
    if (tep2 < 0) {
        perror ("fork error");
        //exit(-1);
        return -1;
    } else if (!tep2) { //child
        while (transfer (sock2, sock1) != -1);
        cout<<"build_conn child2 out"<<endl;
        exit (-1);
    } 
    cout<<"build_conn:build child2 process pid:"<<tep2<<endl;

    while(waitpid (tep, NULL, WNOHANG)==0 && waitpid (tep2, NULL, WNOHANG)==0);
    //if (!waitpid (tep, NULL, WNOHANG)) 
        kill (tep, SIGKILL);
    //if (!waitpid (tep2, NULL, WNOHANG)) 
        kill (tep2, SIGKILL);

    return tep;
}
