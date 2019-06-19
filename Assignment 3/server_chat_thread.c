#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#define PORT 8000
#define MAX 10

void receiver(int);
void sender(int*);

char buf[256];
int dfd[MAX];
//int count;
int sfd;
int main()
{
	//int timeout = 100;  // user timeout in milliseconds [ms]
	int count,yes=1;

	struct sockaddr_in s;
	// The protocol specifies a particular protocol to be used with the socket.  Normally only a single protocol exists to support a partic‐
	// ular socket type within a given protocol family, in which case protocol can be specified as 0.
	sfd=socket(PF_INET,SOCK_STREAM,0);//0 signifies TCP

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes));

	s.sin_family=AF_INET;
	s.sin_port=htons(PORT);
	s.sin_addr.s_addr=INADDR_ANY;

	//s.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(sfd,(struct addr*)&s,sizeof(s))==0)
		printf("Bind successful\n");

	listen(sfd,5);

	count=-1;

	pthread_t tid1;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid1,&attr,&sender,&count);
	while(1)
	{
		dfd[++count]=accept(sfd,NULL,NULL);
		printf("Count : %d\n",(count+1));

		pthread_t tid2;

		memset(buf,NULL,sizeof(buf));

		//Call the thread
		pthread_create(&tid2,&attr,&receiver,dfd[count]);

		// //Join the threads
		// pthread_join(tid1,NULL);
		// pthread_join(tid2,NULL);

		// close(dfd[count]);
	}

	//Close the file descriptor

}

void receiver(int dfd)
{
	int len,cl;
	while(1)
	{
		if(recv(dfd,&buf,sizeof(buf),0)==0)
		{
			close(dfd);
			pthread_exit(NULL);
			//exit(0);
		}

		else
		{
			len=strlen(buf);
			cl=buf[0]-'0';
			//printf("CL : %d\n",cl+sfd);
			//memcpy(buf,buf+1,sizeof(buf));
			//buf[len-1]='\0';

			if(cl==0)
			{
				memcpy(buf,buf+1,sizeof(buf));
				buf[len-1]='\0';
				printf("Message received from client %d : %s\n",(dfd-sfd),buf);
				//send(dfd,&buf,sizeof(buf),0);
			}

			else
			{
				buf[0]=dfd+'0';
				send(cl+sfd,&buf,sizeof(buf),0);
			}
		}

		memset(buf,NULL,sizeof(buf));
	}
}

void sender(int* count)
{
	int cl;
	while(1)
	{
		int i;
		memset(buf,NULL,sizeof(buf));
		//scanf("%s",&buf);
		scanf(" %[^\t\n]s",&buf);

		if(strcmp(buf,"q")==0)
		{
			//close(d);
			exit(0);
		}

		if(strlen(buf)!=0)
		{
			printf("Enter client number (0 to brodcast) : ");
			scanf("%d",&cl);

			if(cl==0)
			{
				for(i=0;i<*count;i++)
					send(dfd[i],&buf,sizeof(buf),0);
			}

			else
				send(dfd[cl-1],&buf,sizeof(buf),0);
		}
	}
}
