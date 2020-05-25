#include "board.h"
#include "move.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// =========================== Global Variables ===========================

Position gamePosition;                  // Position we are going to use
Move myMove;                            // move to save our choice and send it to the server

char myColor;                           // to store our color
int mySocket;                           // our socket

char *agentName = "La Geo Stormers";    // agent name
char *ip = "127.0.0.1";                 // default ip (localhost)

// ======================== Functions Definitions =========================

void minimax(Position *position, int depth);
void findAnts(Position *position, int ants[2][MAX_ANTS], char color);
void findPossibleMoves(Position *position, List *possibleMoves, int ants[2][MAX_ANTS], char color);
void buildMove(Position *position, List *possibleMoves, char color, int vDir, int hDir, int i, int j);

List *createList();
void addItemToList(List *list, Move *move);
void printList(List *list);
void addItemsToList(List *list, List *items);
ListNode *removeFromList(List *list);
int isEmpty(List *list);
void printNode(ListNode *listNode);

// ================================= ΜΑΙΝ =================================

int main(int argc, char **argv)
{
    int c;
    while ((c = getopt(argc, argv, "i:p:h")) != -1)
        switch (c)
        {
        case 'h':
            printf("[-i ip] [-p port]\n");
            return 0;
        case 'i':
            ip = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        case '?':
            if (optopt == 'i' || optopt == 'p')
                printf("Option -%c requires an argument.\n", (char)optopt);
            else if (isprint(optopt))
                printf("Unknown option -%c\n", (char)optopt);
            else
                printf("Unknown option character -%c\n", (char)optopt);
            return 1;
        default:
            return 1;
        }

    connectToTarget(port, ip, &mySocket);

    char msg;

    /**********************************************************/
    // used in random
    srand(time(NULL));
    int i, j, k;
    int jumpPossible;
    int playerDirection;
    /**********************************************************/

    while (1)
    {

        msg = recvMsg(mySocket);
        switch (msg)
        {
        case NM_REQUEST_NAME: //server asks for our name
            sendName(agentName, mySocket);
            break;
        case NM_NEW_POSITION: //server is trying to send us a new position
            getPosition(&gamePosition, mySocket);
            printPosition(&gamePosition);
            break;
        case NM_COLOR_W: //server indorms us that we have WHITE color
            myColor = WHITE;
            printf("My color is %d\n", myColor);
            break;
        case NM_COLOR_B: //server indorms us that we have BLACK color
            myColor = BLACK;
            printf("My color is %d\n", myColor);
            break;
        case NM_REQUEST_MOVE: //server requests our move
            myMove.color = myColor;

            if (!canMove(&gamePosition, myColor)) // no moves available
               myMove.tile[0][0] = -1;           //null move
            else
            { // you can move

                Position predictedPosition;
                memcpy(&predictedPosition, &gamePosition, sizeof(gamePosition));
                minimax(&predictedPosition, 1);
            }

            sendMove(&myMove, mySocket); //send our move
            break;
        case NM_QUIT: //server wants us to quit...we shall obey
            close(mySocket);
            return 0;
        default:
            printf("Wrong Signal!\n");
            return 0;
        }
    }

    return 0;
}

// ==================== Algorithm - Related Functions  ====================

void minimax(Position *position, int depth)
{
    printf("Checkpoint 1\n");
    if (depth == 0) return;
    printf("Checkpoint 2\n");
    //printf((myColor == WHITE) ? "I AM WHITE!\n" : "I AM BLACK\n");
    if (myColor == WHITE)
    { // WHITE -> MAXIMIZING PLAYER
        printf("Checkpoint 3\n");
        int maxEvaluation = -INFINITY;

        int ants[2][MAX_ANTS];
        printf("Checkpoint 4\n");
        findAnts(position, ants, WHITE);
        //printAnts(ants);
        printf("WHITE IS PLAYING");
        List *possible_moves = createList();
        findPossibleMoves(position, possible_moves, ants, WHITE);
        //printList(possible_moves);
        printf("possible moves:");
        printList(possible_moves);
        myMove = *possible_moves->front->data;

    }
    else {  // BLACK -> MINIMIZING PLAYER
        int maxEvaluation = INFINITY;

        int ants[2][MAX_ANTS];
        findAnts(position, ants, BLACK);
        //printAnts(ants);
        printf("BLACK IS PLAYING");
        List *possible_moves = createList();
        findPossibleMoves(position, possible_moves, ants, BLACK);
        //printList(possible_moves);

        myMove = *possible_moves->rear->data;
    }
}

void findAnts(Position *position, int ants[2][MAX_ANTS], char color)
{
    int counter = 0;
    for (int i = 0; i < BOARD_ROWS; i++)
    {
        for (int j = 0; j < BOARD_COLUMNS; j++)
        {
            if (position->board[i][j] == color)
            {
                ants[0][counter] = i;
                ants[1][counter] = j;
                //*(ants + counter) = i;              //ants[0][counter] = i;
                //*(ants + counter + BOARD_ROWS) = j; //ants[1][counter] = j;
                counter++;
            }
        }
    }
    printf("%d\n",counter);
    if (counter < MAX_ANTS){
        ants[0][counter] = -1;
        printf("Checkpoint 6\n");}
    //*(ants + counter + 1) = -1; //set next ant position to -1
    printf("Checkpoint 7\n");
}

void printAnts(int ants[2][MAX_ANTS]){
    for (int i = 0; i < MAX_ANTS; i++)
        printf("Ant %d: (%d, %d)\n", i, ants[0][i], ants[1][i]);
}

void findPossibleMoves(Position *position, List *possibleMoves, int ants[2][MAX_ANTS], char color)
{
    int playerDirection = (color == WHITE) ? 1 : -1;
    int jumpPossible = FALSE;
    printf("HEY");
    for (int counter = 0; counter < MAX_ANTS; counter++)
    {
        if (ants[0][counter] != -1)
        {
            int i = ants[0][counter];
            int j = ants[1][counter];

            // can jump, can move, etc...
            int jumpInfo = canJump(i, j, color, position);
            if (jumpInfo != 0)
            {
                jumpPossible = TRUE;
                int k = 0;
                List *oldList = createList();
                List *newList = createList();
                printf("jumpInfo: %d", jumpInfo);
                Move* tempMove = (Move*)malloc(sizeof(Move));
                tempMove->tile[0][0] = i;
                tempMove->tile[1][0] = j;
                addItemToList(oldList, &tempMove);
                while (isEmpty(newList))
                {
                    while (!isEmpty(oldList))
                    {
                        
                        tempMove = removeFromList(oldList)->data;
                        jumpInfo = canJump(tempMove->tile[0][k], tempMove->tile[1][k], color, position);
                        if (jumpInfo == 3)
                        { // both jumps available

                            Move* myMove2 = (Move*)malloc(sizeof(Move));
                            myMove2 = tempMove;
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j - 2;
                            myMove2->tile[1][k] = j + 2;
                            if (isLegal(position, tempMove))
                                addItemToList(newList, tempMove);
                            if (isLegal(position, myMove2))
                                addItemToList(newList, myMove2);
                        }
                        else if (jumpInfo == 1)
                        { // left jump available
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j - 2;
                            if (isLegal(position, tempMove))
                                addItemToList(newList, tempMove);
                        }
                        else if (jumpInfo == 2)
                        { // right jump available
                            tempMove->tile[0][k] = i + 2 * playerDirection;
                            tempMove->tile[1][k] = j + 2;
                            if (isLegal(position, tempMove)){
                                printf("mpika");
                                addItemToList(newList, tempMove);
                            }
                        }
                        else if (jumpInfo == 0)
                        {
                            if (k < MAXIMUM_MOVE_SIZE)
                                tempMove->tile[0][k] = -1;
                            if (isLegal(position, tempMove))
                                addItemToList(possibleMoves, tempMove);
                        }
                    }
                    k++;
                    oldList = newList;
                    if (k == MAXIMUM_MOVE_SIZE)
                        break; //maximum tiles reached
                }
                addItemsToList(possibleMoves, oldList);
            }
        }
    }
    printf("POSSIBLE MOVES: \n");
    printList(possibleMoves);
    printf("My color is %d \n" ,color);

    printf(jumpPossible ? "AT LEAST ONE JUMP IS AVAILABLE\n" : "NO JUMPS ARE AVAILABLE\n");

    if (!jumpPossible) {
        for (int counter = 0; counter < MAX_ANTS; counter++) {
            if (ants[0][counter] != -1) {
                int i = ants[0][counter];
                int j = ants[1][counter];
                // Check if left move is available
                buildMove(position, possibleMoves, color, playerDirection, 0, i, j);
                buildMove(position, possibleMoves, color, playerDirection, 1, i, j);
            }
        }
    }
    printf("END \n");
}

void buildMove(Position *position, List *possibleMoves, char color, int vDir, int hDir, int i, int j){ //hDir (left:0, right:1)
    Move *tempMove = (Move *)malloc(sizeof(Move));
    tempMove->color = color;
    tempMove->tile[0][0] = i;
    tempMove->tile[1][0] = j;
    tempMove->tile[0][1] = i + 1 * vDir;
    tempMove->tile[1][1] = (hDir == 0) ? j - 1 : j + 1;
    tempMove->tile[0][2] = -1;
    
    //printf("Checking RM Validness: (%d, %d) -> (%d, %d) ", move.tile[0][0],  move.tile[1][0],  move.tile[0][1],  move.tile[1][1]);
    if (isLegal(position, tempMove)){
        //printf("%s Move is Legal!\n", (hDir == 0) ? "Left" : "Right");
        addItemToList(possibleMoves, tempMove);
    } //else printf("%s Move is NOT Legal!\n", (hDir == 0) ? "Left" : "Right");
}

// ======================= List - Related Functions =======================

List *createList()
{
    List *list = (List *)malloc(sizeof(List));
    list->front = list->rear = NULL;
    list->length = 0;
    return list;
}

void addItemToList(List *list, Move *move)
{
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp->data = move;
    temp->next = NULL;

    if (isEmpty(list))
        list->front = temp;
    else
        list->rear->next = temp;
    
    list->rear = temp;
    list->length++;
}

void printList(List *list){
    ListNode *temp = (ListNode*)malloc(sizeof(ListNode));
    temp = list->front;
    while(temp!=NULL){
        printNode(temp);
        temp = temp->next;
    }
}

void addItemsToList(List *list, List *items)
{
    while (!isEmpty(items))
        addItemToList(list, removeFromList(items)->data);
}

ListNode *removeFromList(List *list)
{
    if (isEmpty(list)) return NULL;
    //Store previous front and move front one node ahead
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp = list->front;
    list->front = list->front->next;
    list->length--;
    return (temp);
}

int isEmpty(List *list)
{
    return (list->rear == NULL) ? TRUE : FALSE;
}

void printNode(ListNode *listNode)
{
    for (int i = 0; i < MAXIMUM_MOVE_SIZE; i++)
    {
        printf("(%d,%d)", listNode->data->tile[0][i], listNode->data->tile[1][i]);
        printf(i == MAXIMUM_MOVE_SIZE - 1 ? "\n" : ", ");
    }
}