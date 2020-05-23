#include "board.h"
#include "move.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "c-generic-library/list.h"


/**********************************************************/
Position gamePosition;		// Position we are going to use

//Move moveReceived;		// temporary move to retrieve opponent's choice
Move myMove;				// move to save our choice and send it to the server

char myColor;				// to store our color
int mySocket;				// our socket

char * agentName = "La Geo Stormers";		//default name.. change it! keep in mind MAX_NAME_LENGTH

char * ip = "127.0.0.1";	// default ip (local machine)
/**********************************************************/


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
				
				//if(!canMove( &gamePosition, myColor )) // no moves available
				//    myMove.tile[ 0 ][ 0 ] = -1;		//null move
				//else // you can move
                //    predictMove(&gamePosition, myColor);

                /* ******************* TESTING IMPLEMENTATION OF LIST ********************/
                List container;
                memset(&container, 0, sizeof(container));
                //We want to store Move structs
                construct_List(&container, sizeof(Move), FREEOBJ);

                // Inserting first move
                Move firstMove;
                firstMove.tile[0][0] = 1;
                firstMove.tile[1][0] = 0;
                firstMove.tile[0][1] = 2;
                firstMove.tile[1][1] = 1;
                firstMove.tile[0][2] = -1;
                firstMove.color = WHITE;
                push_back_List(&container, &firstMove, sizeof(firstMove), DYNAMIC);

                // Inserting second move
                Move secondMove;
                firstMove.tile[0][0] = 2;
                firstMove.tile[1][0] = 1;
                firstMove.tile[0][1] = 3;
                firstMove.tile[1][1] = 2;
                firstMove.tile[0][2] = -1;
                firstMove.color = WHITE;
                 push_back_List(&container, &secondMove, sizeof(firstMove), DYNAMIC);

                // Prinf info about first two moves
                ListIter* iterator = create_ListIter(&container);
                if (!empty_List(iterator)){
                    head_ListIter(iterator);
                    do {
                        Move *iterMove = (Move*)retrieve_ListIter(iterator);
                        printf("%d %d %d %d %d\n", iterMove->tile[0][0], iterMove->tile[1][0], iterMove->tile[0][1], iterMove->tile[1][1], iterMove->tile[2][0]);
                    } while(!next_ListIter(iterator));
                }

                /* ******************* END OF TESTING ********************/
                
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
                    myList = newList;
                    if( k == MAXIMUM_MOVE_SIZE )	//maximum tiles reached
                        break;
                }	
                // Ara h myList periexei ola ta possible moves (ektws apo ta filla pou vrethikan nwritera kai ta exoume idi prosthesei sto possbileMoves)
                possibleMoves.add(myList);
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