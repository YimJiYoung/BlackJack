#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "tcp.h"


int	Sockfd;

void
CloseServer()
{
	close(Sockfd); //close socket
	printf("\nBlackJack Server exit.....\n");

	exit(0);
}

void
StartGame()
{
	
}

void
SuffleCards(Card cards[])
{
	int i,j;
	for(i=0;i<4;i++){
		for(int j=0;j<52;j++){//swap random 2 cards for 52 times
			int rand1= rand()%52;
			int rand2= rand()%52;
			Swap(&cards[rand1],&cards[rand2]);
		}
	}	
}
void 
SetCards(Card cards[]) //inital card setting
{
	int i,j;
	int idx=0;
	for(i=0;i<4;i++){//suit
		for(j=1;j<14;j++){//number
			cards[idx].suit=i;
			cards[idx].number=j;
			idx++;
		}
	}
}
char*
CardInfo(Card card)
{
	char str[20];
	char suit[10];
	char number[2];
		//suit
		if(card.suit==CLUBS)
			strcpy(suit,"CLUB ");
		if(card.suit==SPADES)
			strcpy(suit,"SPADE ");
		if(card.suit==DIAMONDS)
			strcpy(suit,"DIAMOND ");
		if(card.suit==HEARTS)
			strcpy(suit,"HEART ");
		//number
		if(card.number<11)
			sprintf(number,"%d",card.number);
		else if(card.number==11)
			strcpy(number,"J");
		else if(card.number==12)
			strcpy(number,"Q");
		else if(card.number==13)
			strcpy(number,"K");
			
		strcat(str, number);

	return str;
}

int 
CalculateScore(Card *deck, int deckSize) //calculate the score of deck
{	
	int i;
	int sum=0;
	int A_number=0;

	for(i=0;i<deckSize;i++){
		if (deck[i].number==1){ //A
			A_number++;
		}
		else if(deck[i].number<=10&&deck[i].number>=2)
			sum+=deck[i].number;
		else // J,Q,K
			sum+=10;
	}
	for (i =0;i<A_number;i++){// A 1 or 11
		if(sum+11<=21) 
			sum+=11;
		else
			sum+=1;
	}
	if(sum>21)
		sum=0;
	return sum;
}

void 
Swap(Card *card1, Card *card2)//swap two card
{
	Card temp=*card1;
	*card1=*card2;
	*card2=temp;
}

main(int argc, char *argv[])
{
	int newSockfd, cliAddrLen, n, top;
	struct sockaddr_in cliAddr, servAddr;

	MsgType msg;
	msg.key=-1;
	Card cards[52];
	Card serverDeck[10];
	int turn=1;
	int clientScore, serverScore;

	// resister SIGINT handler
	signal(SIGINT, CloseServer);
	// create a socket ( ipv4 tcp )
	if((Sockfd=socket(PF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}
	
	bzero((char *)&servAddr, sizeof(servAddr));// fill byte 0
	servAddr.sin_family = PF_INET; //ipv4
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // any available ip address
	servAddr.sin_port = htons(SERV_TCP_PORT); // port number
	//bind socket to address
	if(bind(Sockfd,(struct sockaddr *)&servAddr, sizeof(servAddr))<0){
		perror("bind");
		exit(1);	
	}
	//set max the number of connections
	listen(Sockfd, 5);

	printf("BlackJack Server started.....\n");
	
	cliAddrLen = sizeof(cliAddr);
	while(1){
		//accept a connection on a socket
		newSockfd=accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen);
		if(newSockfd<0){
			perror("accept");
			exit(1);
		}
		sprintf(msg.msg,"#######Welcome to BlackJack Game#########\n1.START\n2.QUIT\n");

		//write(send) socket
		if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
			perror("write");
			exit(1);
		}

		//read(receive) socket
		if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
			perror("read");
			exit(1);
		}
		//game start
		if(msg.key==1)
		{
			srand(time(NULL));	

			SetCards(cards);
			SuffleCards(cards);
		
			top=0;
			//client inital card distribution
			msg.deck[0]=cards[0];
			msg.deck[1]=cards[1];
			msg.deckSize=2;
			//server inital card distribution
			serverDeck[0]=cards[2];
			serverDeck[1]=cards[3];
			top=4;
			
			sprintf(msg.msg,"\n#########%d turn###########\n1.Hit\n2.Stand\n",turn);
			turn+=1;
			//write(send) socket
			if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
				perror("write");
				exit(1);
			}
			//read(receive) socket
			if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
				perror("read");
				exit(1);
			}
			printf("%d\n",msg.key);

			//Hit
			while(msg.key==1){
				printf("Hit");
				//give client new card
				msg.deck[msg.deckSize]=cards[top];
				top++;
				msg.deckSize++;
				//calculate client socre
				clientScore=CalculateScore(msg.deck,msg.deckSize);				
				//Bust
				if(clientScore==0){
					sprintf(msg.msg,"\n#########BUST##########\nYOU LOSE:(\n");
					msg.lose=1;
					break;
				}
				else{	
					//write(send) socket
					sprintf(msg.msg,"\n#########%d turn###########\n1.Hit\n2.Stand\n",turn);
					if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
						perror("write");
						exit(1);
					}
					//read socket
					if ((n = read(newSockfd, (char *)&msg, sizeof(msg))) < 0)  {
						perror("read");
						exit(1);
					}
				}
			}
			//Stand
			if(msg.key=2){
				printf("Stand");
				clientScore=CalculateScore(msg.deck,msg.deckSize);				
				//Bust
				if(clientScore==0){
					sprintf(msg.msg,"\n#########BUST##########\nYOU LOSE:(\n");
				}
				else{
				 	//server card
					serverScore=CalculateScore(serverDeck,2);
					int size;				
					while(serverScore<16){ //get card until score>=16
						serverDeck[size]=cards[top];
						size++;top++;
						serverScore=CalculateScore(serverDeck,2);
					}
					if(clientScore>serverScore){//win
						sprintf(msg.msg,"\n#######################\nYOU WIN:)\nYour Score: %d  Computer Score: %d\n",clientScore, serverScore);
					}
					else if(clientScore==serverScore){//draw
						sprintf(msg.msg,"\n#######################\nDRAW!!\nYour Score: %d  Computer Score: %d\n",clientScore, serverScore);
					}
					else{//lose
						sprintf(msg.msg,"\n#######################\nYOU LOSE:(\nYour Score: %d  Computer Score: %d\n",clientScore, serverScore);
					}
				}
				if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
					perror("write");
					exit(1);
				}
			}
			else{ //Hit and Bust	
				if (write(newSockfd, (char *)&msg, sizeof(msg)) < 0)  {
					perror("write");
					exit(1);
				}
			}
			
		}
				
		usleep(10000);
		close(newSockfd); //close newsocket
	
	}
}
