#include "board.h"
#include "move.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

/**********************************************************/
Position gamePosition;		// Position we are going to use

//Move moveReceived;		// temporary move to retrieve opponent's choice
Move myMove;				// move to save our choice and send it to the server

char myColor;				// to store our color
int mySocket;				// our socket

char * agentName = "La Geo Stormers";		//default name.. change it! keep in mind MAX_NAME_LENGTH

char * ip = "127.0.0.1";	// default ip (local machine)
/**********************************************************/

List *createList();
void addToList(Move *move, List *list);
ListNode *removeFromList(List *list);
int isEmpty(List *list);

int main( int argc, char ** argv ) {
	int c;

	while( ( c = getopt ( argc, argv, "i:p:h" ) ) != -1 )
		switch( c )
		{
			case 'h':
				printf( "[-i ip] [-p port]\n" );
				return 0;
			case 'i':
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?':
				if( optopt == 'i' || optopt == 'p' )
					printf( "Option -%c requires an argument.\n", ( char ) optopt );
				else if( isprint( optopt ) )
					printf( "Unknown option -%c\n", ( char ) optopt );
				else
					printf( "Unknown option character -%c\n", ( char ) optopt );
				return 1;
			default:
			return 1;
		}



	connectToTarget( port, ip, &mySocket );

	char msg;

/**********************************************************/
// used in random
	srand( time( NULL ) );
	int i, j, k;
	int jumpPossible;
	int playerDirection;
/**********************************************************/

	while( 1 ) {

		msg = recvMsg( mySocket );
		switch ( msg ) {
			case NM_REQUEST_NAME:		//server asks for our name
				sendName( agentName, mySocket );
				break;
			case NM_NEW_POSITION:		//server is trying to send us a new position
				getPosition( &gamePosition, mySocket );
				printPosition( &gamePosition );
				break;
			case NM_COLOR_W:			//server indorms us that we have WHITE color
				myColor = WHITE;
				printf("My color is %d\n",myColor);
				break;
			case NM_COLOR_B:			//server indorms us that we have BLACK color
				myColor = BLACK;
				printf("My color is %d\n",myColor);
				break;
			case NM_REQUEST_MOVE:		//server requests our move
				myMove.color = myColor;
				
				if(!canMove( &gamePosition, myColor )) // no moves available
				   myMove.tile[ 0 ][ 0 ] = -1;		//null move
				else // you can move
                   predictMove(&gamePosition, myColor);

				sendMove( &myMove, mySocket );			//send our move
				break;
			case NM_QUIT:			//server wants us to quit...we shall obey
				close( mySocket );
				return 0;
			default:
				printf("Wrong Signal!\n");
				return 0;
		}

	} 

	return 0;
}



void predictMove(){


    Position predictedPosition = gamePosition;
    minimax(&predictedPosition, 1);
}


void minimax(Position *position, int depth){
    if (depth == 0) return;

    if (myColor == WHITE) { // WHITE -> MAXIMIZING PLAYER
        int maxEvaluation = - INFINITY;

        // find every ant position of White Player (current gamePosition)
        int ants[2][MAX_ANTS];
        findAnts(&position, &ants[0][0], WHITE); 

        // 

        Move possibleMoves[2 * MAX_ANTS];
        findPossibleMoves(&position, &possibleMoves[0], ants, WHITE);

        
    } else {
        int maxEvaluation = - INFINITY;
    }
}

// Find every ant of a particular color
void findAnts(Position *position, int *ants, char color){
    int counter = 0;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLUMNS; j++) {
            if (position->board[i][j] == color) {
                *(ants + counter) = i;                  //ants[0][counter] = i;
                *(ants + counter + BOARD_ROWS) = j;     //ants[1][counter] = j;
                counter ++;
            }
        }
    }

    if (counter < MAX_ANTS) *(ants + counter + 1) = -1; //set next ant position to -1
    return ants;
}

// Find every possible move of a player
void findPossibleMoves(Position *gamePosition, Move *possibleMoves, int ants[2][MAX_ANTS], char color){
    int playerDirection = (color == WHITE) ? 1 : -1;
	int jumpPossible = FALSE;

    for (int counter = 0; counter < MAX_ANTS; counter++){
        if (ants[0][counter] != -1) {
            int i = ants[0][counter];
            int j = ants[1][counter];
            
            // can jump, can move, etc...
			int jumpInfo = canJump( i, j, color, &gamePosition);
			if(jumpInfo != 0){
				jumpPossible = TRUE;
				int k = 1;
                while( jumpInfo != 0 )
                {
					myMove.tile[ 0 ][ k ] = i + 2 * playerDirection;
					if( jumpInfo == 3){ // both jumps available

						Move myMove2;
						memcpy(myMove2, myMove, sizeof(Move));
						myMove.tile[ 1 ][ k ] = j - 2;
						myMove2.tile[ 1 ][ k ] = j + 2;
					}
					else if( jumpInfo == 1){ // left jump available
						myMove.tile[ 1 ][ k ] = j - 2;
					}
					else if ( jumpInfo == 2){ // right jump available
						myMove.tile[ 1 ][ k ] = j + 2;
					}

					
                    if( k + 1 == MAXIMUM_MOVE_SIZE )	//maximum tiles reached
                        break;
                    
                    myMove.tile[ 0 ][ k + 1 ] = -1;		//maximum tiles not reached

                    i = myMove.tile[ 0 ][ k ];		//we will try to jump from this point in the next loop
                    j = myMove.tile[ 1 ][ k ];


                    k++;
					jumpInfo = canJump( i, j, myColor, &gamePosition );
                }
			}
                
                
        }
    }

    
    if (!jumpPossible){
        int moves = 0;
        for (int counter = 0; counter < MAX_ANTS; counter++){
            if (ants[0][counter] != -1) {
                int i = ants[0][counter];
                int j = ants[1][counter];

                // Check if left move is available
                Move leftMove;
                leftMove.color = color;
                leftMove.tile[0][0] = i;
                leftMove.tile[1][0] = j;

                leftMove.tile[0][1] = i + 1 * playerDirection;
                leftMove.tile[1][1] = j - 1;

                leftMove.tile[0][2] = -1;

                if (isLegal(&gamePosition, &leftMove)){
                    possibleMoves[moves] = leftMove;
                    moves++;
                }

                // Check if right move is available
                Move rightMove;
                rightMove.color = color;
                rightMove.tile[0][0] = i;
                rightMove.tile[1][0] = j;

                rightMove.tile[0][1] = i + 1 * playerDirection;
                rightMove.tile[1][1] = j + 1;

                rightMove.tile[0][2] = -1;

                if (isLegal(&gamePosition, &rightMove)){
                    possibleMoves[moves] = rightMove;
                    moves++;
                }
            }
        }
    }
	
}

*/
// List related functions

List *createList(){
    List *list = (List *)malloc(sizeof(List));
    list->front = list->rear = NULL;
    list->length = 0;
    return list;
}

void addToList(Move *move, List *list){
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp->data = move;
    temp->next = NULL;

    if(list->rear == NULL){
        list->front = temp;
        list->rear = temp;
        list->length++;
        return;
    }

    //Add the new node at the end of list and change rear
    list->rear->next = temp;
    list->rear = temp;
    list->length++;
}

ListNode *removeFromList(List *list){
    if(isEmpty(list))
        return;
    //Store previous front and move front one node ahead
    ListNode *temp = list->front;
    list->front = list->front->next;
    list->length--;
    return(temp);
}

int isEmpty(List *list){
    return (list->rear == NULL) ? TRUE : FALSE;
}

void printNode(ListNode *listNode){
    for (int i = 0; i < MAXIMUM_MOVE_SIZE; i++){
        printf("(%d,%d)", listNode->data->tile[0][i], listNode->data->tile[1][i]);
        printf(i == MAXIMUM_MOVE_SIZE - 1 ? "\n" : ", ");
    }
}