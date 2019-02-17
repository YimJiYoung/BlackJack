#define	MY_ID			38

#define	SERV_TCP_PORT	(8000 + MY_ID)
#define	SERV_HOST_ADDR	"127.0.0.1"

#define	MSG_REQUEST		1
#define	MSG_REPLY		2

#define CLUBS 0
#define SPADES 1
#define DIAMONDS 2
#define HEARTS 3

typedef struct {
	int number;
	int suit; 
}  Card;

typedef struct  {
	int 	key; // key like start, quit, hit , stand ... 
	Card 	deck[12]; // client deck
	int 	deckSize; 
	char	msg[1024]; // message to client
	int 	lose;
}
	MsgType;

