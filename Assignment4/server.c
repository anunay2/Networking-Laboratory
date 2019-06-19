#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
//stroing the maximum size of the buffer
#define MAX 1500
char t_buff[10];
int time_con(int n){
	//for time
	int i=0;
	while(n){
		t_buff[i++]=n%256;
		n/=256;
	}
	return i;
}
int main(int argc,char ** argv) {
	int sockfd,PORT;
	PORT = atoi(argv[1]);
	char buffer[MAX];
	char *tata = "Hi!Client.I just forwarded a Packet";
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	// AF_INET is an address family that is used to designate
	//the type of addresses that your socket can communicate with
	// (in this case, Internet Protocol v4 addresses). When you create a
	// socket, you have to specify its address family, and then
	//you can only use addresses of that type with the socket.
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)) == -1){
            perror("Set socket option");
            exit(1);
    }
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	//htons to the network byte order
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	while(1){
		struct timeval tv;
		int len, n;
		//  This flag requests that the operation block  until  the
    //  full request is satisfied.  However, the call may still
    //  return less data than requested if a signal is  caught,
    //  an  error  or disconnect occurs, or the next data to be
    //    received is of a different type than that returned.

		n = recvfrom(sockfd, (char *)buffer, MAX, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
		gettimeofday(&tv,NULL);
		int seq_rec = 256*buffer[0] + buffer[1];
		buffer[6]--;//decrese ttl
		//printf("Client (%d) : ",tv.tv_usec);
		printf("%d:",seq_rec);
		//printing the exact same contents in the buffer
 			for(int ind=7;ind<n && buffer[ind]!='!' ; ind++)
		 		printf("%c",buffer[ind]);
				printf("\n");
		 int ind=7;
		 for(int i=0;i<strlen(tata);i++)
			buffer[i+7]=tata[i];
		 buffer[7+strlen(tata)]='\0';
		// Tell the link layer that forward progress happened: you
		// got a successful reply from the  other  side.   If  the
		// link  layer  doesn't get this it will regularly reprobe
		// the neighbor (e.g., via a unicast ARP).
		sendto(sockfd, buffer, n, MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
		gettimeofday(&tv,NULL);
		printf("\nHello message sent at %ld.\n",tv.tv_usec);
	}
	return 0;
}
