#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <sys/types.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <netdb.h>

#define PORT 8000
#define SIZE 10

void receiver(int);
void sender(int);

int sfd;
char buf[256];

int main()
{
	struct sockaddr_in s;
	char ip[20];
	int port;
	sfd=socket(PF_INET,SOCK_STREAM,0);//0 signifies TCP

	s.sin_family=AF_INET;
	// s.sin_port=htons(PORT);
	// s.sin_addr.s_addr=INADDR_ANY;
	printf("Enter IP : ");
	scanf("%s",&ip);
	printf("Enter Port : ");
	scanf("%d",&port);
	
	s.sin_port=htons(port);
	s.sin_addr.s_addr = inet_addr(ip);

	int cs=connect(sfd,(struct sockaddr*)&s,sizeof(s));
	
	if(cs!=-1)
		printf("Connection established\n");

	//Create the thread
	pthread_t tid1,tid2;
	//pthread_t pid[SIZE+1];
	// pthread_attr_t attr;

	// pthread_attr_init(&attr);

	memset(buf,NULL,sizeof(buf));

	//Call the threads
	pthread_create(&tid1,NULL,&sender,0);
	pthread_create(&tid2,NULL,&receiver,0);

	//Join the threads
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	//Close the file descriptor
	close(sfd);
}

void receiver(int d)
{
	int num;
	while(1)
	{
		if(recv(sfd,&buf,sizeof(buf),0)==0)
		{
			close(sfd);
			exit(0);
		}

		num=buf[0]-'0';

		if(!((num>=46 && num<=71) || (num>=14 && num<=39)))
		{
			memcpy(buf,buf+1,sizeof(buf));
			buf[strlen(buf)]='\0';
			printf("Message received from client no. %d : %s\n",num-sfd,buf);
		}

		else
			printf("Message received from server : %s\n",buf);

		memset(buf,NULL,sizeof(buf));
	}
}

void sender(int d)
{
	char cl,dum;
	while(1)
	{
		memset(buf,NULL,sizeof(buf));
		//scanf("%s",buf);
		scanf(" %[^\t\n]s",&buf);
		if(strcmp(buf,"q")==0)
		{
			close(sfd);
			exit(0);
		}
		//memcpy(buf+1,buf,sizeof(buf));

		printf("Press 0 to send to server, or the client number : ");
		scanf("%c %c",&dum,&cl);

		for(int i=strlen(buf)-1;i>=0;i--)
			buf[i+1]=buf[i];

		buf[strlen(buf)+1]='\0';
		buf[0]=cl;
		// printf("Buf[0] : %c\n",buf[0]);
		
		// printf("Buffer : %s",buf);

		if(strlen(buf)!=0)
			send(sfd,&buf,sizeof(buf),0);
	}
}