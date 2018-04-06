#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <term.h>
#include <curses.h>
#include<unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/shm.h>

typedef struct{
	char* host;
	int port;
	char* filename;
	int line,col;
} str_config;
str_config config={
	"127.0.0.1",
	5555,
	"show.txt",
	0,
	0
};
char** msg;
int init();
int show(int);
void get_wh(int* line,int* col);
void loc_xy(int x,int y);
int line,col;
char* pname;
void help(){
	fprintf(stderr,"%s [-p port][-h host][-f file][end]\n\t"
	"port: the listen port\n\t"
	"host: ip addr\n\t"
	"file: file\n\t"
	"end: means the last client,notify server start\n",pname);
	exit(1);
}
int main(int argc,char** argv){
	pname = argv[0];
	int i,state=0,end=0;

	for(i=1;i<argc;i++){
		if(state==0){
			if(!strcmp(argv[i],"-p"))state=1;
			else if(!strcmp(argv[i],"-h"))state=2;
			else if(!strcmp(argv[i],"end"))end=1;
			else if(!strcmp(argv[i],"-f"))state=3;
			else help();
		}else if(state==1){
			config.port=atoi(argv[i]);
			state=0;
		}else if(state==2){
			config.host=argv[i];
			state=0;
		}else if(state==3){
			config.filename=argv[i];
			state=0;
		}
	}
	if(config.port<=0)help();
	
	get_wh(&line,&col);
    int sock_cli = socket(AF_INET,SOCK_STREAM, 0);
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(config.port);
    servaddr.sin_addr.s_addr = inet_addr(config.host);
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("No Server");
        exit(1);
    }

	char sendbuf[50];
	char recvbuf[50];
	int ret;
	memset(sendbuf, 0, sizeof(sendbuf));
	sprintf(sendbuf,"%d %d",2,col);
	send(sock_cli, sendbuf, sizeof(sendbuf),0);
	if(end==1){
		memset(sendbuf, 0, sizeof(sendbuf));
		sprintf(sendbuf,"%d %d",3,0);
		send(sock_cli, sendbuf, sizeof(sendbuf),0);
	}
	init();
	printf("\x1b[2J\x1b[?25l");
     while(1){
		 ret = recv(sock_cli, recvbuf, sizeof(recvbuf),0);
		 if(ret < 0)
		{
			printf("recv error!, The error is %s, errno is %d\n", strerror(errno), errno);
			goto end;
		}
		else if(ret == 0)
		{
			printf("recv() time over!\n");
			goto end;
		}
		int a,b;
		sscanf(recvbuf,"%d %d",&a,&b);
		if(a==4){
			show(b-col);
		}else if(a==5){
			break;
		}
	 }


end:
	//end
	close(sock_cli);
	printf("\x1b[2J");
	loc_xy(0,0);
	printf("\x1b[?25h");
	return 0;
}
int init(){
	char buffer[1024];
	char* tmp;
	int line,col,i;
	FILE* file=fopen(config.filename,"rb");
	if(file==NULL){
		fprintf(stderr,"file read error\n");
		exit(1);
	}
	if(feof(file)){
		fprintf(stderr,"file read error\n");
		exit(1);
	}
	memset(buffer,0,1024);
	fgets(buffer,1000,file);
	tmp=buffer;
	while(*tmp!=' '&&*tmp!=0){
		tmp++;
	}
	if(*tmp==' '){
		*tmp=0;
		tmp++;
	}
	line=atoi(buffer);
	col=atoi(tmp);
	if(line<=0||col<=0){
		fprintf(stderr,"file read error\n");
		fclose(file);
		exit(1);
	}
	config.line=line;
	config.col=col;
	msg=(char**)malloc(sizeof(char*)*line);
	i=0;
	while(!feof(file)&&i<line){
		memset(buffer,0,1024);
		fgets(buffer,1000,file);
		tmp=(char*)malloc(col+1);
		memset(tmp,0,col+1);
		strcpy(tmp,buffer);
		tmp[col]=0;
		msg[i++]=tmp;
	}
	fclose(file);
}
//return totall column
int show(int index){
	int i,len;
	char fmt[20];
	printf("\x1b[2J");
	fflush(stdout);
	if(index<0){
		len=col+index;
		if(len<=0||len>=col)return 0;
		sprintf(fmt,"%%-%d.%ds",len,len);
		for(i=0;i<line&&i<config.line;i++){
			loc_xy(-index,i);
			printf(fmt,msg[i]);
		}
	}else{
		if(index>=config.col)return 0;
		len=col;
		sprintf(fmt,"%%-%d.%ds",len,len);
		for(i=0;i<line&&i<config.line;i++){
			loc_xy(0,i);
			if(index>=strlen(msg[i]))printf(fmt," ");
			else
			printf(fmt,msg[i]+index);
		}
	}
	fflush(stdout);
	return 0;
}
void get_wh(int* line,int* col){
	setupterm(NULL, fileno(stdout), (int *)0);
    *line = tigetnum("lines");
    *col = tigetnum("cols");
}
void loc_xy(int x,int y){
	printf("\x1b[%d;%dH",y,x);
	fflush(stdout);
}