
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include<unistd.h>


int port = 5555;
void help();
static char* pname;

typedef struct {
	int fd;
	int begin;
	int end;
} str_client;
//max client is 100
#define MAX_CLIENT 100
str_client clients[MAX_CLIENT]={{0,0,0},};
int len_client=0;
int time=30;
int main(int argc,char** argv){
	int i,state=0;
	pname = argv[0];
	for(i=1;i<argc;i++){
		if(state==0){
			if(!strcmp(argv[i],"-p"))state=1;
			else if(!strcmp(argv[i],"-t"))state=2;
			else help();
		}else if(state==1){
			port=atoi(argv[i]);
			state=0;
		}else if(state==2){
			time=atoi(argv[i]);
			state=0;
		}
	}
	if(port<=0) help();
	if(time<1||time>1000)help();

	int ret;
    int server_socket;
    int conn_socket;
    char RecvBuffer[50];
    char SendBuffer[50];
    struct sockaddr_in server_address;
    struct sockaddr_in conn_address;
	
    fd_set readfds, readcpyfds;

    memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
    memset(SendBuffer, 0x00, sizeof(SendBuffer));
    memset(&server_address, 0x00, sizeof(server_address));
    memset(&conn_address, 0x00, sizeof(conn_address));

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    ret=bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if(ret==-1){
		printf("Bind Error\n");
		exit(1);
	}
	ret=listen(server_socket, 10);
	if(ret==-1){
		printf("Listen Error\n");
		exit(1);
	}
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);
    while(1)
    {
        readcpyfds = readfds;
        ret = select(FD_SETSIZE, &readcpyfds, (fd_set*)0, (fd_set*)0, (struct timeval*)0);
        if(ret < 1)
        {
            fprintf(stderr, "select error!\n");
            exit(EXIT_FAILURE);
        }
        int fd;
        for(fd = 0; fd < FD_SETSIZE; fd++)
        {
            if(FD_ISSET(fd, &readcpyfds))
            {
                if(fd == server_socket)
                {
                    int size = sizeof(conn_address);
                    conn_socket = accept(server_socket, (struct sockaddr*)&conn_address, &size);
                    FD_SET(conn_socket, &readfds);
					memset(SendBuffer, 0x00, sizeof(SendBuffer));
					sprintf(SendBuffer,"%d %d",1,len_client);
					send(conn_socket,SendBuffer,sizeof(SendBuffer), 0);
					clients[len_client++].fd=conn_socket;
				}
                else
                {
					memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
                    ret = recv(fd, RecvBuffer, sizeof(RecvBuffer), 0);
                    if(ret < 0)
                    {
                        fprintf(stderr, "recv error!, The error is %s, errno is %d\n", strerror(errno), errno);
                        exit(EXIT_FAILURE);
                    }
                    else if(ret == 0)
                    {
                        printf("recv() time over!\n");
                        exit(EXIT_FAILURE);
                    }
					int a,b;
                    sscanf(RecvBuffer,"%d %d",&a,&b);
					if(a==2){
						for(i=0;i<len_client;i++){
							if(clients[i].fd==fd){
								clients[i].begin=b;
								break;
							}
						}
					//begin game
					}else if(a==3){
						goto begin_game;
					}
                }
            }
        }
    }
	int last_len=0;
	int begin_index=0;
begin_game:
	close(server_socket);
	last_len=0;
	begin_index=0;
	for(i=0;i<len_client;i++){
		clients[i].end=last_len+clients[i].begin-1;
		clients[i].begin=last_len;
		last_len=clients[i].end+1;
	}
	//printf("last_len=%d\n",last_len);
	while(begin_index<last_len+100){
		//printf("begin_index=%d\n",begin_index);
		for(i=0;i<len_client;i++){
			memset(SendBuffer, 0x00, sizeof(SendBuffer));
			sprintf(SendBuffer,"%d %d",4,begin_index-clients[i].begin);
			send(clients[i].fd,SendBuffer,sizeof(RecvBuffer), 0);
		}
		usleep(1000*time);
		begin_index++;
	}
	for(i=0;i<len_client;i++){
		memset(SendBuffer, 0x00, sizeof(SendBuffer));
		sprintf(SendBuffer,"%d %d",5,0);
		send(clients[i].fd,SendBuffer,sizeof(RecvBuffer), 0);
		//close(clients[i].fd);
	}
	sleep(1);
	return 0;
}
void help(){
	fprintf(stderr,"%s [-p port][-t time]\n\t"
	"port: the listen port\n\t"
	"time: dely time 1-1000\n",pname);
	exit(1);
}

