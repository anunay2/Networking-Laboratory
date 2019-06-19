#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
char t_buff[10];
int CUM_DELAY[100],m=0;
//populating the time buffer

int time_con(int n){
	int i=0;
	while(n){
		t_buff[i++]=n%256; n/=256;
	}
	return i;
}
//compare function
int cmpfunc (const void * a, const void * b){
   return ( *(int*)a - *(int*)b );
}

int main(int argc,char ** argv) {
	int P = atoi(argv[3]);
	int ttl = atoi(argv[4]);
	int num_of_packets = atoi(argv[5]);
	int sockfd,PORT;
	PORT = atoi(argv[2]);

	FILE *Filepointer;
	Filepointer=fopen("data.csv","w");
	char *hello = "Hello from client";
	struct sockaddr_in	 servaddr;
	struct hostent *host;
	host = gethostbyname(argv[1]);
	// Creating socket file descriptor
	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	//servaddr.sin_addr = *((struct in_addr *)host->h_addr);

	int itr = num_of_packets;

	float tot=0;
	struct timeval tv;
	int ttl1[]={2,8,16};
	int packet_size[]={100,200,300,400,500,600,700,800,900,1000};
	for(int x=0;x<10;x++){
		P=packet_size[x];
		int MAXLINE = P + 7;
		char packet[MAXLINE];
		char rec_buf[MAXLINE];
	for(int h=0;h<3;h++){
		m=0;
		ttl=ttl1[h];
		for(int seq=1; seq<=itr ; seq++){
			int save_ttl = ttl;
			int total_delay = 0;
			while(ttl){
				int n, len, ind=5;
				//sequence number in position 0 1
				packet[0]=seq/256;
				packet[1]=seq%256;

				//time stamp from 2 to 5
				gettimeofday(&tv,NULL);//time stamp
				int t_stamp = tv.tv_usec;
				int ll = time_con(t_stamp);
				for(int i=0;i<ll;i++){
					packet[ind--]=t_buff[i];
				}
				while(ind>=2) packet[ind--]=(char)0;

				//ttl at 6
				packet[6]=ttl/1;
				//from 7 store messege
				ind=7;
				for(int i=0;i<P;i++) packet[ind++]='P';
				packet[ind++]='!';

				sendto(sockfd, packet, MAXLINE , MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
				printf("\tMessage sent. (%d)\n",t_stamp);

			//receive part
				n = recvfrom(sockfd, (char *)rec_buf, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				gettimeofday(&tv,NULL);//use this
				ttl = rec_buf[6]; ttl--;
				int seq_rec = 256*rec_buf[0] + rec_buf[1];
				/*int po=3;
				int t_stamp_rec=0;
				for(ind = 2; ind<6; ind++){
					t_stamp_rec += ((int)pow(256,po))*rec_buf[ind]; po--;
				}*/
				printf("%d : RTT = %ld  ",seq_rec,  tv.tv_usec - t_stamp );
				total_delay += tv.tv_usec - t_stamp;
				ind = 7;
				while(ind<MAXLINE && rec_buf[ind]!='!') printf("%c",rec_buf[ind++]);printf("\n");
			}//while loop for TTL ends here
			ttl = save_ttl;
			CUM_DELAY[m++]=total_delay;
			float cur = 2*((float)(total_delay))/ttl;
			printf("Average RTT delay for seq = %d is :: %f\n\n", seq, cur);
			tot += cur;
		}//
		qsort(CUM_DELAY, m, sizeof(int), cmpfunc);
		fprintf(Filepointer,"%d\t",CUM_DELAY[m/2]);
		/*for(int i=0;i<m;i++){
			printf("%d\n",CUM_DELAY[i]);
		}*/
	}//inner loop for changing the packet size
	fprintf(Filepointer,"\n");
	}//outer loop for the ttl
	itr--;
	tot/=itr;
	printf("final = %f\n\n\n",tot);
	close(sockfd);
	fclose(Filepointer);
	return 0;
}
