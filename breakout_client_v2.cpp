#include "common.h"



int main (int argc, char *argv[]) {
    /*
        client_v2 serverip serverport localport
    */
    if (argc != 4) {
        cout << "USAGE:\nbreakout_client_v2  serverip  serverport  localport  " << endl;
        cout << "-- serverip  (your server's ip)" << endl;
        cout << "-- serverport (which server port you want to connect to server,this should equal the other side's port   eg: ssh  root@serverip -p serverport)" << endl;
        cout << "-- which port you want to expose,for ssh it should be 22" << endl;

        return 0;
    }
    signal (SIGCHLD, signal_handler);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct sockaddr_in serveraddr;

    serveraddr.sin_addr.s_addr = inet_addr (argv[1]); //net sequence
    serveraddr.sin_port = htons (atoi (argv[2]));
    serveraddr.sin_family = AF_INET;
    bzero (& (serveraddr.sin_zero), sizeof (serveraddr.sin_zero));       /* zero the rest of the struct */


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct sockaddr_in localaddr;

    localaddr.sin_addr.s_addr = inet_addr ("127.0.0.1"); //net sequence
    localaddr.sin_port = htons (atoi (argv[3]));
    localaddr.sin_family = AF_INET;
    bzero (& (localaddr.sin_zero), sizeof (localaddr.sin_zero));       /* zero the rest of the struct */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int sock_server=0, sock_local=0;

    getnewsocket(sock_server);
    getnewsocket(sock_local);

    //if (setnoblock(sock_server)==-1) perror("set server socket noblock error");
    //if (setnoblock(sock_local)==-1) perror("set local socket noblock error");


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct tcp_info info1,info2;
    int len = sizeof (info1);
    int cnt_while=0;
    char tep_char;

    while (1) {
        cout<<"\ncnt_while:"<<cnt_while++<<endl;

        while (1){
            getsockopt (sock_server, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
            if (info1.tcpi_state != TCP_ESTABLISHED  ) { // || read(sock_server, &tep_char, 1)<=0
                cout<<"trying to building connect to server..."<<endl;

                //调用connect函数，如果返回0，则连接建立；如果返回-1，检查errno ，如果值为 EINPROGRESS，则连接正在建立
                int ttt=connect (sock_server, (struct sockaddr*) &serveraddr, sizeof (struct sockaddr));
                if (ttt ) {
                    if (errno == EISCONN){//in case the cosket is already connected or the status is not freshed, so , a new socket is the most simple way to handle this
                        getnewsocket(sock_server);
                    }else{
                        //cout<<errno<<endl;
                        perror ("connect to server error..");
                        //exit (-1);
                    }
                    sleep(4);
                }else{
                    cout<<"success"<<endl;
                }

            }else break;
            
        }

        while (1){
            getsockopt (sock_local, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);
            if (info2.tcpi_state != TCP_ESTABLISHED  ) { //||  read(sock_local, &tep_char, 1)<=0 
                cout<<"trying to building connect to local..."<<endl;

                int ttt=connect (sock_local, (struct sockaddr*) &localaddr, sizeof (struct sockaddr));
                if (ttt) {
                     if (errno == EISCONN){//in case the cosket is already connected or the status is not freshed, so , a new socket is the most simple way to handle this
                        getnewsocket(sock_local);
                    }else{
                        //cout<<errno<<endl;
                         perror ("connect to localport error..");
                        //exit (-1);
                    }
                    sleep(4);
                }else{
                    cout<<"success"<<endl;
                }
            }else break;
            
        }

        getsockopt (sock_server, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
        getsockopt (sock_local, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);

        if (info1.tcpi_state == TCP_ESTABLISHED && info2.tcpi_state == TCP_ESTABLISHED){
            //here means everything is ok..
            build_conn (sock_server, sock_local);
            //cout<<"now close the two socket"<<endl;
            //close(sock_local);
            //close(sock_server);
            //getnewsocket(sock_server);
            //getnewsocket(sock_local);
        }else{
            cout<<"although process come to here,but still one link is broken:";
            //if (info1.tcpi_state != TCP_ESTABLISHED) cout<<"server is down  ";
            //if (info2.tcpi_state != TCP_ESTABLISHED) cout<<"local is down  ";
            cout<<endl;
        }
        sleep (3);


        /*

        getsockopt (sock_server, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
        if (info1.tcpi_state != TCP_ESTABLISHED) {
            close (sock_server);
            if ( (sock_server = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                perror ("couldn't build socket..");
                exit (-1);
            }

            if (connect (sock_server, (struct sockaddr*) &serveraddr, sizeof (struct sockaddr))  == -1) {
                perror ("connect to server error..");
                //exit (-1);
            }
            cout << "connected to server" << endl;
        }


        getsockopt (sock_local, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);
        if (info2.tcpi_state != TCP_ESTABLISHED) {

            close (sock_local);
            if ( (sock_local = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                perror ("couldn't build socket..");
                exit (-1);
            }

            if (connect (sock_local, (struct sockaddr*) &localaddr, sizeof (struct sockaddr))  == -1) {
                perror ("connect to localport error..");
                //exit (-1);
            }
            cout << "connected to local port" << endl;
        }

        cout<<"socket server<-->local done"<<endl;


        getsockopt (sock_server, IPPROTO_TCP, TCP_INFO, &info1, (socklen_t *) &len);
        getsockopt (sock_local, IPPROTO_TCP, TCP_INFO, &info2, (socklen_t *) &len);

        if (info1.tcpi_state == TCP_ESTABLISHED && info2.tcpi_state == TCP_ESTABLISHED){
            build_conn (sock_server, sock_local);
        }else{
            cout<<"still at least one socket no connected"<<endl;
        }

        

        //close(sock_local);
        //close(sock_server);
    */

        
    }
    close(sock_local);
    close(sock_server);


    return 0;
}
