#include "common.h"



int mainloop (int port) {
    int clientfd = 0, serverfd = 0;
    struct sockaddr_in client_addr;

    int fromfd = 0;
    struct sockaddr_in from_addr;

    serverfd = buildserver (port); //build up server, listending

    size_t tep = sizeof (struct sockaddr);


    struct tcp_info info1,info2;
    int len = sizeof (struct tcp_info);
    int cnt_while=0;

    while (1) {
        cout<<"\ncnt_while:"<<cnt_while++<<endl;

        while(1){
            getsockopt (clientfd, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
            if (info1.tcpi_state != TCP_ESTABLISHED) {
                cout<<"waiting for first client.."<<endl;
                clientfd = accept (serverfd, (struct sockaddr*) &client_addr, (socklen_t*) &tep);
                if ( clientfd >0){//
                    cout<<"success:"<<port<<endl;
                }else if(clientfd ==-1 && errno==EWOULDBLOCK ){
                    continue;
                }else{
                    perror ("accept error..");
                    //exit (-1);
                    sleep(4);
                }
                
            }else{
                cout<<"from port:"<<port<<endl;
                cout << "get first client->ip:" << inet_ntoa (client_addr.sin_addr) << "  port :" << ntohs (client_addr.sin_port)<<endl;
                break;
            }
            
        }


        tep = sizeof (struct sockaddr);

        while (1){
            getsockopt (fromfd, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);
            if (info2.tcpi_state != TCP_ESTABLISHED) {
                cout<<"waiting for second client.."<<endl;

                fromfd = accept (serverfd, (struct sockaddr*) &from_addr, (socklen_t*) &tep);                
                if ( fromfd >0){//
                    cout<<"success:"<<port<<endl;
                }else if(fromfd ==-1 && errno==EWOULDBLOCK ){
                    
                }else{
                    perror ("accept error..");
                    //exit (-1);
                    sleep(4);
                }               
            }else{
                cout<<"from port:"<<port<<endl;
                cout << "get another client->ip:" << inet_ntoa (from_addr.sin_addr) << "port :" << ntohs (from_addr.sin_port);
                break;
            }
            
        }

        getsockopt (clientfd, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
        getsockopt (fromfd, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);

        if (info1.tcpi_state == TCP_ESTABLISHED && info2.tcpi_state == TCP_ESTABLISHED){
            build_conn (clientfd, fromfd);
            //cout<<"now close the two socket"<<endl;
            //close(clientfd);
            //close(fromfd);
        }else{
            cout<<"\nstill at least one socket not connected!!!!"<<endl;
        }
        sleep(3);
        //cout << "add client size:" << add_client (clientfd, &client_addr) << endl;///add client information
    }
    close(serverfd);
    close(clientfd);
    close(fromfd);

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
            signal (SIGCHLD, signal_handler);
            mainloop (i);
            exit (-1);
        }
        kep_pid[i - serverport] = tep;
        cout<<"buid child process pid:"<<tep<<endl;
    }
    cout << "port:" << serverport << " to " << serverport_end << "fork done!" << endl;

    while (1) {//after make all the forks, the main process do as a guard
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







