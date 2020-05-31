#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "board.h"
#include "comm.h"
#include "move.h"

// =========================== Global Variables ===========================

Position gamePosition;    // Position we are going to use
Move myMove;              // move to save our choice and send it to the server

char myColor;    // to store our color
int mySocket;    // our socket

char *agentName = "La Geo Stormers";    // agent name
char *ip = "127.0.0.1";                 // default ip (localhost)

// ======================== Functions Definitions =========================

float minimax(Position *curr_position, Position *prev_position, Move *move, int depth, int maximizingPlayer, int verbose);
float calculateCost(Position *position, Move *move, char color, int verbose);
int findAnts(Position *position, int ants[2][MAX_ANTS], char color, int verbose);
void printAnts(int ants[2][MAX_ANTS]);
void printMove(Move *move);
void findPossibleMoves(Position *position, List *possibleMoves, int ants[2][MAX_ANTS], char color, int verbose);
void buildMove(Position *position, List *possibleMoves, char color, int vDir, int hDir, int i, int j, int verbose);

List *createList();
void addItemToList(List *list, Move *move);
void printList(List *list);
void addItemsToList(List *list, List *items);
ListNode *removeFromList(List *list);
int isEmpty(List *list);
void clearList(List *list);

const char *getColor(char color);
int getPlayerColor(int maximizingPlayer);
const char *getColorPlayerType(int maximizingPlayer);

// ================================= ΜΑΙΝ =================================

int main(int argc, char **argv) {
    int c;
    while ((c = getopt(argc, argv, "i:p:h")) != -1)
        switch (c) {
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

    while (1) {
        msg = recvMsg(mySocket);
        switch (msg) {
            case NM_REQUEST_NAME:    //server asks for our name
                sendName(agentName, mySocket);
                break;
            case NM_NEW_POSITION:    //server is trying to send us a new position
                getPosition(&gamePosition, mySocket);
                printPosition(&gamePosition);
                break;
            case NM_COLOR_W:    //server indorms us that we have WHITE color
                myColor = WHITE;
                printf("My color is %d\n", myColor);
                break;
            case NM_COLOR_B:    //server indorms us that we have BLACK color
                myColor = BLACK;
                printf("My color is %d\n", myColor);
                break;
            case NM_REQUEST_MOVE:    //server requests our move
                myMove.color = myColor;

                if (!canMove(&gamePosition, myColor))    // no moves available
                    myMove.tile[0][0] = -1;              //null move
                else {                                   // you can move
                    int maximizingPlayer = (myColor == WHITE) ? TRUE : FALSE;
                    int verboseLevel = VERBOSE_OFF;
                    printf("%.1f", minimax(&gamePosition, NULL, NULL, 4, maximizingPlayer, verboseLevel));
                }

                sendMove(&myMove, mySocket);    //send our move
                break;
            case NM_QUIT:    //server wants us to quit...we shall obey
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

float minimax(Position *curr_position, Position *prev_position, Move *move, int depth, int maximizingPlayer, int verbose) {
    if (verbose == VERBOSE_EXTENDED) printf("[minimax][D:%d] Ι ΑΜ: %s, EXAMINING: %s\n", depth, getColor(myColor), getColorPlayerType(maximizingPlayer));
    if (depth == 0) return calculateCost(prev_position, move, getPlayerColor(maximizingPlayer), verbose);
    if (maximizingPlayer) {    // WHITE -> MAXIMIZING PLAYER
        float maxEvaluation = -INFINITY;
        ListNode *maxEvaluationNode;

        int ants[2][MAX_ANTS];
        int foundAnts = findAnts(curr_position, ants, WHITE, VERBOSE_OFF);

        if (!foundAnts) {
            //if (verbose == VERBOSE_EXTENDED) printf("[D:%d] Did not find any ants! No moves available! Returning cost: %.1f\n", depth, -maxEvaluation);
            return -maxEvaluation;
        }

        List *possible_moves = createList();
        findPossibleMoves(curr_position, possible_moves, ants, WHITE, VERBOSE_OFF);
        //if (verbose == VERBOSE_BASIC || verbose == VERBOSE_EXTENDED) printList(possible_moves);

        if (isEmpty(possible_moves)) {
            //if (verbose == VERBOSE_EXTENDED) printf("[D:%d] Found ants but no moves available! Returning cost: %.1f\n", depth, -maxEvaluation);
            return -maxEvaluation;
        }

        ListNode *temp_move = possible_moves->front;
        while (temp_move != NULL) {
            // Duplicate board and apply temp_move to the board
            Position temp_prev = *curr_position;
            Position temp_curr = temp_prev;
            doMove(&temp_curr, temp_move->data);
            // Calculate evaluation cost
            float evaluation = minimax(&temp_curr, &temp_prev, temp_move->data, depth - 1, FALSE, verbose);
            if (evaluation >= maxEvaluation) {
                maxEvaluation = evaluation;
                maxEvaluationNode = temp_move;
            }
            temp_move = temp_move->next;
        }
        if (verbose == VERBOSE_BASIC || verbose == VERBOSE_EXTENDED) {
            printf("[D:%d] BEST (MAX) MOVE (COST %.1f): ", depth, maxEvaluation);
            printMove(maxEvaluationNode->data);
        }
        myMove = *maxEvaluationNode->data;
        return maxEvaluation;

    } else {    // BLACK -> MINIMIZING PLAYER
        float minEvaluation = INFINITY;
        ListNode *minEvaluationNode;

        int ants[2][MAX_ANTS];
        int foundAnts = findAnts(curr_position, ants, BLACK, VERBOSE_OFF);

        if (!foundAnts) {
            //if (verbose == VERBOSE_EXTENDED) printf("[D:%d] Did not find any ants! No moves available! Returning cost: %.1f\n", depth, -minEvaluation);
            return -minEvaluation;
        }

        List *possible_moves = createList();
        findPossibleMoves(curr_position, possible_moves, ants, BLACK, VERBOSE_OFF);
        //if (verbose == VERBOSE_BASIC || verbose == VERBOSE_EXTENDED) printList(possible_moves);

        if (isEmpty(possible_moves)) {
            //if (verbose == VERBOSE_EXTENDED) printf("[D:%d] Found ants but no moves available! Returning cost: %.1f\n", depth, -minEvaluation);
            return -minEvaluation;
        }

        ListNode *temp_move = possible_moves->front;
        while (temp_move != NULL) {
            // Duplicate board and apply temp_move to the board
            Position temp_prev = *curr_position;
            Position temp_curr = temp_prev;
            doMove(&temp_curr, temp_move->data);
            // Calculate evaluation cost
            float evaluation = minimax(&temp_curr, &temp_prev, temp_move->data, depth - 1, TRUE, verbose);
            if (evaluation <= minEvaluation) {
                minEvaluation = evaluation;
                minEvaluationNode = temp_move;
            }
            temp_move = temp_move->next;
        }

        if (verbose == VERBOSE_BASIC || verbose == VERBOSE_EXTENDED) {
            printf("[D:%d] BEST (MIN) MOVE (COST %.1f): ", depth, minEvaluation);
            printMove(minEvaluationNode->data);
        }
        myMove = *minEvaluationNode->data;
        return minEvaluation;
    }
}

float calculateCost(Position *position, Move *move, char color, int verbose) {
    //if (verbose == VERBOSE_EXTENDED) printf("FUNCTION [calculateCost] CALLED [D:0]!\n");
    int MULTIPLIER = (color == BLACK) ? 1 : -1;
    if (verbose == VERBOSE_EXTENDED) {
        printf("[calculateCost] [D:0] %s", getColor(color));
        printMove(move);
        printf("MULTIPLIER = %d, ", MULTIPLIER);
    }

    float cost = 0;
    for (int move_counter = 1; move_counter < MAXIMUM_MOVE_SIZE; move_counter++) {
        if (move->tile[0][move_counter] == -1) break;
        int i = move->tile[0][move_counter];
        int j = move->tile[1][move_counter];
        char cell = position->board[i][j];
        if (abs(move->tile[0][move_counter] - move->tile[0][move_counter - 1]) == 2)    // jump
            cost += WHITE_KILLED_ANT * MULTIPLIER;
        if (cell == FOOD_CELL)    // food
            cost += (WHITE_INCREASED_SCORE / 5.0) * MULTIPLIER;
        if ((color == BLACK && move->tile[0][move_counter] == BOARD_ROWS - 1) || (color == WHITE && move->tile[0][move_counter] == 0))    // reached end
            cost += (WHITE_INCREASED_SCORE + WHITE_LOST_ANT) * MULTIPLIER;
    }
    if (verbose == VERBOSE_BASIC || verbose == VERBOSE_EXTENDED) printf("Total cost = %.1f\n", cost);
    return cost;
}

int findAnts(Position *position, int ants[2][MAX_ANTS], char color, int verbose) {
    if (verbose == VERBOSE_EXTENDED) printf("FUNCTION [findAnts] CALLED!\nAnts: ");
    int counter = 0;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLUMNS; j++) {
            if (position->board[i][j] == color) {
                ants[0][counter] = i;
                ants[1][counter] = j;
                if (verbose == VERBOSE_EXTENDED) printf("(%d, %d) ", ants[0][counter], ants[1][counter]);
                counter++;
            }
        }
    }
    if (counter < MAX_ANTS) {
        ants[0][counter] = -1;
    }
    if (verbose == VERBOSE_EXTENDED) printf("\n");
    return (counter != 0) ? TRUE : FALSE;
}

void printAnts(int ants[2][MAX_ANTS]) {
    printf("Ants: ");
    for (int i = 0; i < MAX_ANTS; i++) {
        if (ants[0][i] == -1) break;
        printf("(%d, %d) ", ants[0][i], ants[1][i]);
    }
    printf("\n");
}

void printMove(Move *move) {
    printf("Move: ");
    for (int i = 0; i < MAXIMUM_MOVE_SIZE; i++) {
        printf("(%d,%d)", move->tile[0][i], move->tile[1][i]);
        printf(i == MAXIMUM_MOVE_SIZE - 1 ? "\n" : "->");
    }
}

void findPossibleMoves(Position *position, List *possibleMoves, int ants[2][MAX_ANTS], char color, int verbose) {
    if (verbose == VERBOSE_EXTENDED) printf("FUNCTION [findPossibleMoves] CALLED!\n");
    int playerDirection = (color == WHITE) ? 1 : -1;
    int jumpPossible = FALSE;
    for (int counter = 0; counter < MAX_ANTS; counter++) {
        if (ants[0][counter] == -1) break;
        int i = ants[0][counter];
        int j = ants[1][counter];

        int jumpInfo = canJump(i, j, color, position);
        if (jumpInfo == 0) continue;

        if (verbose == VERBOSE_EXTENDED) printf("JUMP IS POSSIBLE (%d)\n", jumpInfo);
        jumpPossible = TRUE;
        int k = 0;
        List *oldList = createList();
        List *newList = createList();
        Move *tempMove = (Move *)malloc(sizeof(Move));
        tempMove->color = color;
        tempMove->tile[0][0] = i;
        tempMove->tile[1][0] = j;
        addItemToList(oldList, tempMove);
        do {
            clearList(newList);
            while (!isEmpty(oldList)) {
                tempMove = removeFromList(oldList)->data;
                jumpInfo = canJump(tempMove->tile[0][k], tempMove->tile[1][k], color, position);
                if (jumpInfo == 3) {    // both jumps available
                    Move *myMove2 = (Move *)malloc(sizeof(Move));
                    tempMove->tile[0][k + 1] = tempMove->tile[0][k] + 2 * playerDirection;
                    memcpy(myMove2, tempMove, sizeof(tempMove));
                    tempMove->tile[1][k + 1] = tempMove->tile[1][k] - 2;
                    myMove2->tile[1][k + 1] = tempMove->tile[1][k] + 2;
                    addItemToList(newList, tempMove);
                    addItemToList(newList, myMove2);
                } else if (jumpInfo == 1) {    // left jump available
                    tempMove->tile[0][k + 1] = tempMove->tile[0][k] + 2 * playerDirection;
                    tempMove->tile[1][k + 1] = tempMove->tile[1][k] - 2;
                    addItemToList(newList, tempMove);
                } else if (jumpInfo == 2) {
                    tempMove->tile[0][k + 1] = tempMove->tile[0][k] + 2 * playerDirection;
                    tempMove->tile[1][k + 1] = tempMove->tile[1][k] + 2;
                    addItemToList(newList, tempMove);
                } else if (jumpInfo == 0) {
                    if (k + 1 < MAXIMUM_MOVE_SIZE)
                        tempMove->tile[0][k + 1] = -1;
                    if (isLegal(position, tempMove))
                        addItemToList(possibleMoves, tempMove);
                }
            }
            k++;
            memcpy(oldList, newList, sizeof(*newList));
            if (k == MAXIMUM_MOVE_SIZE)
                break;    //maximum tiles reached
        } while (!isEmpty(newList));
        ListNode *check_move = oldList->front;
        while (check_move != NULL) {
            if (isLegal(position, check_move->data))
                addItemToList(possibleMoves, check_move->data);
            check_move = check_move->next;
        }
    }

    if (verbose == VERBOSE_EXTENDED) printf(jumpPossible ? "AT LEAST ONE JUMP IS AVAILABLE\n" : "NO JUMPS ARE AVAILABLE\n");

    if (!jumpPossible) {
        for (int counter = 0; counter < MAX_ANTS; counter++) {
            if (ants[0][counter] != -1) {
                int i = ants[0][counter];
                int j = ants[1][counter];

                buildMove(position, possibleMoves, color, playerDirection, 0, i, j, verbose);
                buildMove(position, possibleMoves, color, playerDirection, 1, i, j, verbose);
            }
        }
    }
}

void buildMove(Position *position, List *possibleMoves, char color, int vDir, int hDir, int i, int j, int verbose) {    //hDir (left:0, right:1)
    //if (verbose == VERBOSE_EXTENDED) printf("FUNCTION [buildMove] CALLED!\n");
    Move *tempMove = (Move *)malloc(sizeof(Move));
    tempMove->color = color;
    tempMove->tile[0][0] = i;
    tempMove->tile[1][0] = j;
    tempMove->tile[0][1] = i + 1 * vDir;
    tempMove->tile[1][1] = (hDir == 0) ? j - 1 : j + 1;
    tempMove->tile[0][2] = -1;

    //if (verbose == VERBOSE_EXTENDED) printf("Checking %s Move Validness: (%d, %d) -> (%d, %d) ", (hDir == 0) ? "Left" : "Right", tempMove->tile[0][0],  tempMove->tile[1][0],  tempMove->tile[0][1],  tempMove->tile[1][1]);
    if (isLegal(position, tempMove)) {
        //if (verbose == VERBOSE_EXTENDED) printf("%s Move is Legal!\n", (hDir == 0) ? "Left" : "Right");
        addItemToList(possibleMoves, tempMove);
    }    //else if (verbose == VERBOSE_EXTENDED) printf("%s Move is NOT Legal!\n", (hDir == 0) ? "Left" : "Right");
}

// ======================= List - Related Functions =======================

List *createList() {
    List *list = (List *)malloc(sizeof(List));
    list->front = list->rear = NULL;
    list->length = 0;
    return list;
}

void addItemToList(List *list, Move *move) {
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

void printList(List *list) {
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp = list->front;
    while (temp != NULL) {
        printMove(temp->data);
        temp = temp->next;
    }
}

void addItemsToList(List *list, List *items) {
    while (!isEmpty(items))
        addItemToList(list, removeFromList(items)->data);
}

void clearList(List *list) {
    while (!isEmpty(list)) {
        removeFromList(list);
    }
}

ListNode *removeFromList(List *list) {
    if (isEmpty(list)) return NULL;
    //Store previous front and move front one node ahead
    ListNode *temp = (ListNode *)malloc(sizeof(ListNode));
    temp = list->front;
    list->front = list->front->next;
    if (list->front == NULL) list->rear = NULL;
    list->length--;
    return (temp);
}

int isEmpty(List *list) {
    return (list->rear == NULL) ? TRUE : FALSE;
}

// ===================== Utility - Related Functions =====================

const char *getColor(char color) {
    return (color == WHITE) ? "White" : "Black";
}

int getPlayerColor(int maximizingPlayer) {
    return (maximizingPlayer == 1) ? WHITE : BLACK;
}

const char *getColorPlayerType(int maximizingPlayer) {
    return (getPlayerColor(maximizingPlayer) == WHITE) ? "White" : "Black";
}