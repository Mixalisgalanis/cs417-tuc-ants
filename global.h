#ifndef _GLOBAL_H
#define _GLOBAL_H


//#define _DEBUG_		//comment this line when you are done

#ifdef _DEBUG_
    #include <assert.h>
#else
    #define assert(p) {}
#endif


#ifndef getOtherSide
	#define getOtherSide( a ) ( 1-(a) )
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//#define max(x,y) ((x) >= (y)) ? (x) : (y)
//#define min(x,y) ((x) <= (y)) ? (x) : (y)

/**********************************************************/
#define MAXIMUM_MOVE_SIZE 6
#define MAX_ANTS 12
#define MAX_JUMPS 5

/* The size of our board */
#define BOARD_COLUMNS 8
#define BOARD_ROWS 12
#define BOARD_SIZE 8

/* Values for each possible tile state */
#define WHITE 0
#define BLACK 1
#define EMPTY 2
#define RTILE 3
#define ILLEGAL 4

// max size of our name
#define MAX_NAME_LENGTH 16

//default port for client and server
#define DEFAULT_PORT "6001"

//define infinity numbers
#define INFINITY 2147483647

//define minimax static evaluation scores
#define WHITE_KILLED_ANT 1
#define WHITE_LOST_ANT -1
#define WHITE_INCREASED_SCORE 3

//define cell types
#define WHITE_CELL 0
#define BLACK_CELL 1
#define EMPTY_CELL 2
#define FOOD_CELL 3

//define verbose levels
#define VERBOSE_OFF 0
#define VERBOSE_BASIC 1
#define VERBOSE_EXTENDED 2
#endif
