
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