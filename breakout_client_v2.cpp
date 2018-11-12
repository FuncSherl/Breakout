#include "common.h"



int main (int argc, char *argv[]) {
    /*
        client_v2 serverip serverport localport
    */
    if (argc != 4) {
        cout << "usage:client_v2  serverip  serverport  localport  " << endl;
        cout << "serverip  (your server's ip)" <, endl;
        cout << "serverport (which server port you want to connect to server,this should equal the other side's port   eg.ssh  root@serverip -p serverport)" << endl;
        cout << "which port you want to expose,for ssh it should be 22" << endl;

        return 0;
    }
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
    int sock_server, sock_local;

    if ( ( sock_server = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
        perror ( "couldn't build socket.." );
        exit ( -1 );
    }
    if ( ( sock_client = socket ( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) {
        perror ( "couldn't build socket.." );
        exit ( -1 );
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while (1) {
        if (connect (sock_server, (struct sockaddr*) &serveraddr, sizeof (struct sockaddr))  == -1) {
            perror ("connect to server error..");
            exit (-1);
        }

        if (connect (sock_local, (struct sockaddr*) &localaddr, sizeof (struct sockaddr))  == -1) {
            perror ("connect to localport error..");
            exit (-1);
        }




        close(sock_local);
        close(sock_server);

        sleep(3);
    }


    return 0;
}
