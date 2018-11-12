#include "common.h"

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

    cout << "get socket:" << sockfd << endl;

    my_addr.sin_family = AF_INET; /* host byte order */
    my_addr.sin_port = htons (port);   /* short, network byte order */
    my_addr.sin_addr.s_addr = htons (INADDR_ANY);

    bzero (& (my_addr.sin_zero), sizeof (my_addr.sin_zero));       /* zero the rest of the struct */


    if (bind (sockfd, (struct sockaddr*) &my_addr, sizeof (struct sockaddr)) == -1) {
        perror ("bind error:");
        exit (-1);
    }

    listen (sockfd, 20);
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
        perror ("recv error");

        return -1;
    }

    if ( (sendlen = send (to_sock, buff, getlen, 0)) <= 0) {
        perror ("send error ");
        return -1;
    }

    cout << "get:" << getlen << "  send :" << sendlen << endl;

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
        exit (-1);
    } else {
        while (transfer (sock2, sock1) != -1);
        kill (tep, SIGKILL);
    }
    return tep;
}

int mainloop (int port) {
    int clientfd = 0, serverfd = 0;
    struct sockaddr_in client_addr;

    int fromfd = 0;
    struct sockaddr_in from_addr;

    serverfd = buildserver (port); //build up server, listending

    size_t tep = sizeof (struct sockaddr);

    cout << "port:" << port << "  waiting to accept.." << endl;


    while (1) {
        if ( (clientfd = accept (serverfd, (struct sockaddr*) &client_addr, (socklen_t*) &tep)) == -1) {
            perror ("accept error..");
            exit (-1);
        }

        tep = sizeof (struct sockaddr);
        if ( (fromfd = accept (serverfd, (struct sockaddr*) &from_addr, (socklen_t*) &tep)) == -1) {
            perror ("accept error..");
            exit (-1);
        }
        build_conn (clientfd, fromfd);

        //cout << "get client->ip:" << inet_ntoa (client_addr.sin_addr) << "port :" << ntohs (client_addr.sin_port);

        //cout << "add client size:" << add_client (clientfd, &client_addr) << endl;///add client information
    }

    return NULL;
}


int main() {
    signal (SIGCHLD, signal_handler);

    int i = serverport;
    int kep_pid[serverport_end - serverport + 1] = {0};
    for (i = serverport; i <= serverport_end; ++i) {
        int tep = fork();
        if (tep < 0) {
            perror ("main: fork error");
        } else if (!tep) {
            mainloop (i);
            exit (-1);
        }
        kep_pid[i - serverport] = tep;
    }
    cout << "port:" << serverport << " to " << serverport_end << "fork done!" << endl;

    while (1) {
        int tepid = wait (NULL);//don't care how it ends
        for (i = 0; i < serverport_end - serverport + 1; ++i) {
            if (kep_pid[i] == tepid) {
                int tepp = fork();
                if (tepp < 0) {
                    perror ("main: fork error");
                } else if (!tepp) {
                    mainloop (i+serverport);
                    exit (-1);
                }
                kep_pid[i] = tepp;
                break;
            }
        }
    }
}

return 0;
}







