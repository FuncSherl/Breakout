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
#include <netinet/tcp.h>
#include <sys/time.h>

#include <vector>
#include <map>
#include <algorithm>

#define bufflen 2048*6

//here define the server reflect ports
#define serverport 7001
#define serverport_end 7050

/////////////////////////////////


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

void show_time(){
    char now[64];
    time_t tt=time(NULL);
    struct tm *ttime;
    
    ttime = localtime(&tt);
    strftime(now,64,"%Y-%m-%d %H:%M:%S",ttime);
    cout << now << ":"<<endl;
}



void signal_handler (int signo) { //處理殭屍進程
    if (signo == SIGCHLD) {
        pid_t pid;
        while ( (pid = waitpid (-1, NULL, WNOHANG)) > 0) {//WNOHANG 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若结束，则返回该子进程的ID。
            cout << ("SIGCHLD pid ") << pid << endl;
        }
    }
}

void getnewsocket(int &fd){
    if (fd>0) close(fd); 
    if ( (fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("couldn't build socket..");
        exit (-1);
    }
}

int setnoblock(int &m_sock){
    int flags = fcntl(m_sock, F_GETFL, 0);
    return fcntl(m_sock, F_SETFL, flags|O_NONBLOCK);
}

int getsockstatus(int sd){ //0->everything is ok   
    int optval;
    socklen_t optlen = sizeof(int);

    getsockopt(sd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen);
    return optval;
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
    both node should be noblock
    */
    char buff[bufflen];
    int len = bufflen;
    int i;
    int getlen = 0, sendlen = 0;

    getlen = recv (src_sock, buff, len, 0);
    /*
    boblock:
    recv返回>0，   正常
    返回-1，而且errno被置为EWOULDBLOCK(11)  正常
    其它情况    关闭
    */

    if (getlen>0) {//get data and ready to send

        int retry=0;
        while (1){//constantly send data unless link is broken
            sendlen = send (to_sock, buff, getlen, 0);

            if ( sendlen>0 ){//send success
                return sendlen;
            }else if(sendlen==-1 && errno==EWOULDBLOCK && retry<=10){//should be blocked,max retry 10 times
                ++retry;
                continue;  
            }else{
                perror ("transfer:send error ");
                return -1;
            }
        }


    }else if (getlen==-1 && errno==EWOULDBLOCK){//noblock cause this
        return 0;
    }
    else{
        perror ("transfer:recv error");
        return -1;
    }
    //cout << "transfer:get:" << getlen << "  send :" << sendlen << endl;
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

    while(waitpid (tep, NULL, WNOHANG)==0 && waitpid (tep2, NULL, WNOHANG)==0) sleep(3);
    //if (!waitpid (tep, NULL, WNOHANG)) 
        kill (tep, SIGKILL);
    //if (!waitpid (tep2, NULL, WNOHANG)) 
        kill (tep2, SIGKILL);

    return tep;
}
