#include "wrapper.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri,char *filename,char *cgiargs);
void serve_static(int fd,char *filename,int filesize);
void get_filetype(char *filename,char *filetype);
void serve_dynamic(int fd,char *filename,char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,  char *shortmsg, char *longmsg);

int main(int argc,char **argv)
{
	int listenfd,connfd;
	char hostname[MAXLINE],port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;

	/*check command line args*/
	if(argc!=2)
	{
		fprintf(stderr,"usage:%s <port> \n",argv[0];
		exit(1);
	}
	/*调用open_listen_sock函数打开监听套接字*/
	listenfd=Open_listenfd(argv[1]);
	while(1)
	{
		clientlen=sizeof(clientaddr);
		/*accept */
		connfd=Accept(listenfd,(SA *)&clientaddr,&clientlen);
		Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accpeted connetion from (%s,%s)\n",hostname,port);
		doit(connfd);
		close(connfd);
	}
}
/*HTTP事务处理函数*/
void doit(int fd)
{
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE];
	char filename[MAXLINE],cgiargs[MAXLINE];
	rio_t rio;
	/*初始化rio文件及缓冲区*/
	Rio_readinitb(&rio,fd);
	if(!Rio_readlineb(&rio,buf,MAXLINE))
		return;
	printf("%s",buf);
	sscanf(buf, "%s %s %s", method, uri, version);
	if(strcasecmp(method,"GET")){
		clienterror(fd,method,"501","NOT IMPLEMENTED","WEB DOES NOT IMPLEMENT THIS METHOD");
		return;
	}

	read_requesthdrs(&rio);
	is_static=parse_uri(uri,filename,cgiargs);
	if(stat(filename,&sbuf)<0){
		clienterror(fd, filename, "404", "NOT fOUND","WEB  COULD NOT FOUND THIS FILE");
	return;
	}
	if(is_static)
	{
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
		{
			clienterror(fd,fliename,"403","FORBIDDEN","WEB CLOUND NOT READ THE FILE");
		return;
		}
	serve_static(fd,filename,sbuf.st_size);
	}
	else
	{
		if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
		{
			clienterror(fd,fliename,"403","FORBIDDEN","WEB CLOUND NOT RUN CGI PROGRAM");
		return;
		}
	serve_dynamic(fd,filename,cgiargs);
	}
}




