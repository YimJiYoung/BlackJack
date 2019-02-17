#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "tcp.h"


void
PrintCardInfo(Card card)
{
	char str[20];
	char suit[10];
	char number[5];
		//suit
		if(card.suit==CLUBS)
			printf("CLUB ");
		if(card.suit==SPADES)
			printf("SPADES ");
		if(card.suit==DIAMONDS)
			printf("DIAMOND ");
		if(card.suit==HEARTS)
			printf("HEART ");
		//number
		if(card.number==1)
			printf("A\n");
		else if(card.number<11)
			printf("%d\n",card.number);
		else if(card.number==11)
			printf("J\n");
		else if(card.number==12)
			printf("Q\n");
		else if(card.number==13)
			printf("K\n");
			
		strcat(str, number);

}
main(int argc, char *argv[])
{
	int					sockfd, n;
	struct sockaddr_in	servAddr;
	MsgType				msg;
	msg.key=-1;
	//create a socket
	if ((sockfd=socket(PF_INET, SOCK_STREAM,0))<0)  {
		perror("socket");
		exit(1);
	}
	
	bzero((char *)&servAddr, sizeof(servAddr));//fill byte 0
	servAddr.sin_family = PF_INET;//ipv4
	servAddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);//server ip address
	servAddr.sin_port = htons(SERV_TCP_PORT);//server port
	//establish a connection on a socket
	if (connect(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) <0)  {
		perror("connect");
		exit(1);
	}

//	while(!msg.key){
		//read(receive) socket
		if ((n=read(sockfd,(char *)&msg, sizeof(msg)))<0)  {
			perror("read");
			exit(1);
		}
		//print message
		printf("%s\n", msg.msg);
		//input key
		printf("KEY>");
		scanf("%d", &msg.key);

		//write(send) socekt
		if (write(sockfd, (char *)&msg, sizeof(msg))<0)  {
			perror("write");
			exit(1);
		}
		//read socket(first card distrivution)
		if ((n=read(sockfd,(char *)&msg, sizeof(msg)))<0)  {
			perror("read");
			exit(1);
		}
		while(msg.key==1){//Hit
			if(msg.lose==1)
				break;
			printf("%s\n", msg.msg);
			printf("------my deck-------\n");
			//print my deck information
			for(int i=0;i<msg.deckSize;i++){
		
				PrintCardInfo(msg.deck[i]);
			}
			printf("--------------------\n\n");
			//input key
			printf("KEY>");
			scanf("%d", &msg.key);
		
			//write(send) socekt
			if (write(sockfd, (char *)&msg, sizeof(msg))<0)  {
				perror("write");
				exit(1);
			}
			//read socket(first card distrivution)
			if ((n=read(sockfd,(char *)&msg, sizeof(msg)))<0)  {
				perror("read");
				exit(1);
			}
			
		}
		
		printf("%s\n", msg.msg);
		
		
	
	close(sockfd);//close socket
}
