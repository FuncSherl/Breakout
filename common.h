
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
#include <vector>
#include <map>

#define bufflen 2048
#define serverport 2002

using namespace std;

typedef struct client_info{
    int sockfd;
    struct sockaddr_in addr;
}client_info;

typedef vector<client_info>::iterator vec_client_iter

