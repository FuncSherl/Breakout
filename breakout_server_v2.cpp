#include "common.h"

void signal_handler (int signo) { //處理殭屍進程
    if (signo == SIGCHLD) {
        pid_t pid;
        while ( (pid = waitpid (-1, NULL, WNOHANG)) > 0) {//WNOHANG 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若结束，则返回该子进程的ID。
            cout << ("SIGCHLD pid ") << pid << endl;
        }
    }
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

    return 0;
}


int main() {
    //signal (SIGCHLD, signal_handler);

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

                cout << "one port down:" << i + serverport << " pid:" << tepid << " now fork a new process on this port.." << endl;

                int tepp = fork();
                if (tepp < 0) {
                    perror ("main: fork error");
                } else if (!tepp) {
                    mainloop (i + serverport);
                    exit (-1);
                }
                kep_pid[i] = tepp;
                break;
            }
        }
    }


    return 0;
}







