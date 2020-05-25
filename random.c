
/**********************************************************/
// random player - not the most efficient implementation
/*

if( myColor == WHITE )		// find movement's direction
    playerDirection = 1;
else
    playerDirection = -1;

jumpPossible = FALSE;		// determine if we have a jump available
for( i = 0; i < BOARD_ROWS; i++ )
{
    for( j = 0; j < BOARD_COLUMNS; j++ )
    {
        if( gamePosition.board[ i ][ j ] == myColor )
        {
            if( canJump( i, j, myColor, &gamePosition ) )
                jumpPossible = TRUE;
        }
    }
}

while( 1 )
{
    i = rand() % BOARD_ROWS;
    j = rand() % BOARD_COLUMNS;

    if( gamePosition.board[ i ][ j ] == myColor )		//find a piece of ours
    {

        myMove.tile[ 0 ][ 0 ] = i;		//piece we are going to move
        myMove.tile[ 1 ][ 0 ] = j;

        if( jumpPossible == FALSE )
        {
            myMove.tile[ 0 ][ 1 ] = i + 1 * playerDirection;
            myMove.tile[ 0 ][ 2 ] = -1;
            if( rand() % 2 == 0 )	//with 50% chance try left and then right
            {
                myMove.tile[ 1 ][ 1 ] = j - 1;
                if( isLegal( &gamePosition, &myMove ))
                    break;

                myMove.tile[ 1 ][ 1 ] = j + 1;
                if( isLegal( &gamePosition, &myMove ))
                    break;
            }
            else	//the other 50%...try right first and then left
            {
                myMove.tile[ 1 ][ 1 ] = j + 1;
                if( isLegal( &gamePosition, &myMove ))
                    break;

                myMove.tile[ 1 ][ 1 ] = j - 1;
                if( isLegal( &gamePosition, &myMove ))
                    break;
            }
        }
        else	//at least one jump possible
        {
            if( canJump( i, j, myColor, &gamePosition ) ) // this i,j has available jump
            {
                k = 1;
                while( canJump( i, j, myColor, &gamePosition ) != 0 )
                {
                    myMove.tile[ 0 ][ k ] = i + 2 * playerDirection;
                    if( rand() % 2 == 0 )	//50% chance
                    {

                        if( canJump( i, j, myColor, &gamePosition ) % 2 == 1 )		//left jump possible
                            myMove.tile[ 1 ][ k ] = j - 2;
                        else
                            myMove.tile[ 1 ][ k ] = j + 2;

                    }
                    else	//50%
                    {

                        if( canJump( i, j, myColor, &gamePosition ) > 1 )		//right jump possible
                            myMove.tile[ 1 ][ k ] = j + 2;
                        else
                            myMove.tile[ 1 ][ k ] = j - 2;

                    }

                    if( k + 1 == MAXIMUM_MOVE_SIZE )	//maximum tiles reached
                        break;
                    
                    myMove.tile[ 0 ][ k + 1 ] = -1;		//maximum tiles not reached

                    i = myMove.tile[ 0 ][ k ];		//we will try to jump from this point in the next loop
                    j = myMove.tile[ 1 ][ k ];


                    k++;
                }
                break;
            }
        }
    }

}

*/         					
// end of random
/**********************************************************/

/*
void test(){

    int jumpStatus = canJump(i, j, color, &gamePosition);
    int jumps = 0;
    while (jumpStatus != 0){
        jumpPossible = TRUE;
        32
    }

}
*/

/*



    //*********************************************************
    // used in random
    srand(time(NULL));
    int i, j, k;
    int jumpPossible;
    int playerDirection;
    //*********************************************************



                // ******************* TESTING IMPLEMENTATION OF LIST ******************
                //We want to store Move structs
                List *list = createList();
                //construct_List(&container, sizeof(Move), FREEOBJ);

                // Inserting first move
                Move firstMove;
                firstMove.tile[0][0] = 1;
                firstMove.tile[1][0] = 0;
                firstMove.tile[0][1] = 2;
                firstMove.tile[1][1] = 1;
                firstMove.tile[0][2] = -1;
                firstMove.color = WHITE;
                addItemToList(list, &firstMove);

                // Inserting second move
                Move secondMove;
                secondMove.tile[0][0] = 2;
                secondMove.tile[1][0] = 1;
                secondMove.tile[0][1] = 3;
                secondMove.tile[1][1] = 2;
                secondMove.tile[0][2] = -1;
                secondMove.color = WHITE;
                addItemToList(list, &secondMove);

                Move thirdMove;
                thirdMove.tile[0][0] = 7;
                thirdMove.tile[1][0] = 6;
                thirdMove.tile[0][1] = 5;
                thirdMove.tile[1][1] = 4;
                thirdMove.tile[0][2] = -1;
                thirdMove.color = WHITE;
                addItemToList(list, &thirdMove);
                
                // Inserting first move
                Move fourthMove;
                fourthMove.tile[0][0] = 1;
                fourthMove.tile[1][0] = 0;
                fourthMove.tile[0][1] = 6;
                fourthMove.tile[1][1] = 6;
                fourthMove.tile[0][2] = -1;
                fourthMove.color = WHITE;
                addItemToList(list, &fourthMove);

                // Print info about first two moves
                if (!isEmpty(list)){
                    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
                    temp = list->front;while(temp != NULL){
                        printNode(temp);
                        temp = temp->next;
                    }
                }

                // ******************* END OF TESTING *******************
*/




/* OLD JUMP IMPLEMENTATION

            // can jump
            int jumpInfo = canJump(i, j, color, position);
            if (jumpInfo != 0) {
                jumpPossible = TRUE;
                int k = 0;
                List *oldList = createList();
                List *newList = createList();
                if (verbose == VERBOSE_EXTENDED) printf("JUMP IS POSSIBLE (%d): ", jumpInfo);
                Move *tempMove = (Move *)malloc(sizeof(Move));
                tempMove->tile[0][0] = i;
                tempMove->tile[1][0] = j;
                addItemToList(oldList, tempMove);
                while (isEmpty(newList)) {
                    while (!isEmpty(oldList)) {
                        tempMove = removeFromList(oldList)->data;
                        jumpInfo = canJump(tempMove->tile[0][k], tempMove->tile[1][k], color, position);
                        if (jumpInfo == 3) {    // both jumps available
                            Move *myMove2 = (Move *)malloc(sizeof(Move));
                            myMove2 = tempMove;
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j - 2;
                            myMove2->tile[1][k] = j + 2;
                            if (isLegal(position, tempMove))
                                addItemToList(newList, tempMove);
                            if (isLegal(position, myMove2))
                                addItemToList(newList, myMove2);
                            if (verbose == VERBOSE_EXTENDED) printf("(LEFT, RIGHT)");
                        } else if (jumpInfo == 1) {    // left jump available
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j - 2;
                            if (isLegal(position, tempMove))
                                addItemToList(newList, tempMove);
                            if (verbose == VERBOSE_EXTENDED) printf("(LEFT) ");
                        } else if (jumpInfo == 2) {    // right jump available
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j + 2;
                            if (isLegal(position, tempMove)) {
                                addItemToList(newList, tempMove);
                            }
                            if (verbose == VERBOSE_EXTENDED) printf("(RIGHT) ");
                        } else if (jumpInfo == 0) {
                            if (k < MAXIMUM_MOVE_SIZE)
                                tempMove->tile[0][k] = -1;
                            if (isLegal(position, tempMove))
                                addItemToList(possibleMoves, tempMove);
                        }
                    }
                    k++;
                    oldList = newList;
                    if (k == MAXIMUM_MOVE_SIZE)
                        break;    //maximum tiles reached
                }
                addItemsToList(possibleMoves, oldList);
            }

*/



// JUMP UPDATE 1

    if (verbose == VERBOSE_EXTENDED) printf("FUNCTION [findPossibleMoves] CALLED!\n");
    int playerDirection = (color == WHITE) ? 1 : -1;
    int jumpPossible = FALSE;
    for (int counter = 0; counter < MAX_ANTS; counter++) {
        //printf("1. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        if (ants[0][counter] == -1) break;
        //printf("2. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        int i = ants[0][counter];
        int j = ants[1][counter];

        //printf("3. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        int jumpInfo = canJump(i, j, color, position);
        if (jumpInfo == 0) continue;

        if (verbose == VERBOSE_EXTENDED) printf("JUMP IS POSSIBLE (%d)\n", jumpInfo);
        //printf("\n4. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        jumpPossible = TRUE;
        int k = 0;
        List *oldList = createList();
        List *newList = createList();
        printf("5. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        Move *tempMove = (Move *)malloc(sizeof(Move));
        tempMove->color = color;
        tempMove->tile[0][0] = i;
        tempMove->tile[1][0] = j;
        printf("6. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        addItemToList(oldList, tempMove);
        printf("7. (%d, %d)\n", ants[0][counter], ants[1][counter]);
        while (isEmpty(newList)) {
            while (!isEmpty(oldList)) {
                printf("==08==\n");
                tempMove = removeFromList(oldList)->data;
                printf("==08.5==\n");
                jumpInfo = canJump(tempMove->tile[0][k], tempMove->tile[1][k], color, position);
                printf("==09==\n");
                if (jumpInfo == 3) {    // both jumps available
                    Move *myMove2 = (Move *)malloc(sizeof(Move));
                    tempMove->tile[0][k + 1] = i + 2 * playerDirection;
                    tempMove->tile[0][k + 2] = -1;
                    myMove2 = tempMove;
                    tempMove->tile[1][k + 1] = j - 2;
                    myMove2->tile[1][k + 1] = j + 2;
                    printf("==10==\n");
                    printf("k = %d\n", k);
                    printf("Checking LEFT JUMP (%d, %d) -> (%d, %d)\n", tempMove->tile[0][k], tempMove->tile[1][k], tempMove->tile[0][k+1], tempMove->tile[1][k+1]);
                    if (isLegal(position, tempMove)){
                        addItemToList(newList, tempMove);
                        printf("LEFT LEGAL, ADDED\n");
                    } else printf("LEFT NOT LEGAL, DISCARDED\n");
                    printf("Checking RIGHT JUMP (%d, %d) -> (%d, %d)\n", myMove2->tile[0][k], myMove2->tile[1][k], myMove2->tile[0][k+1], myMove2->tile[1][k+1]);
                    if (isLegal(position, myMove2)){
                        addItemToList(newList, myMove2);    
                        printf("RIGHT LEGAL, ADDED\n");
                    } else printf("RIGHT NOT LEGAL, DISCARDED\n");
                        
                    //if (verbose == VERBOSE_EXTENDED) printf("(LEFT, RIGHT)");
                } else if (jumpInfo == 1) {    // left jump available
                    tempMove->tile[0][k + 1] = i + 2 * playerDirection;
                    tempMove->tile[1][k + 1] = j - 2;
                    tempMove->tile[0][k + 2] = -1;
                    printf("==10==\n");
                    printf("k = %d\n", k);
                    printf("Checking LEFT JUMP (%d, %d) -> (%d, %d)\n", tempMove->tile[0][k], tempMove->tile[1][k], tempMove->tile[0][k+1], tempMove->tile[1][k+1]);
                    if (isLegal(position, tempMove)){
                        addItemToList(newList, tempMove);
                        printf("LEFT LEGAL, ADDED\n");
                    } else printf("LEFT NOT LEGAL, DISCARDED\n");
                    //if (verbose == VERBOSE_EXTENDED) printf("(LEFT) ");
                } else if (jumpInfo == 2) {    // right jump available
                    tempMove->tile[0][k + 1] = i + 2 * playerDirection;
                    tempMove->tile[1][k + 1] = j + 2;
                    tempMove->tile[0][k + 2] = -1;
                    printf("==10==\n");
                    printf("k = %d\n", k);
                    printf("Checking RIGHT JUMP (%d, %d) -> (%d, %d)\n", tempMove->tile[0][k], tempMove->tile[1][k], tempMove->tile[0][k+1], tempMove->tile[1][k+1]);
                    if (isLegal(position, tempMove)){
                        addItemToList(newList, tempMove);
                        printf("RIGHT LEGAL, ADDED\n");
                    } else printf("RIGHT NOT LEGAL, DISCARDED\n");
                    //if (verbose == VERBOSE_EXTENDED) printf("(RIGHT) ");
                } else if (jumpInfo == 0) {
                    if (k < MAXIMUM_MOVE_SIZE)
                        tempMove->tile[0][k] = -1;
                    printf("==10==\n");
                    if (isLegal(position, tempMove)){
                        printf("NO FURTHER JUMPS, ADDED LEGAL JUMP STREAK\n");
                        addItemToList(possibleMoves, tempMove);
                    } else printf("NO FURTHER JUMPS, JUMP STREAK NOT LEGAL, DISCARDED\n");
                }
            }
            k++;
            oldList = newList;
            if (k == MAXIMUM_MOVE_SIZE)
                break;    //maximum tiles reached
        }
        addItemsToList(possibleMoves, oldList);
        
    }
