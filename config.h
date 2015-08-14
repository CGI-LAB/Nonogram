#ifndef _CONFIG_H_
#define _CONFIG_H_

#define P_SIZE 25
#define S_SIZE P_SIZE * P_SIZE
// for sse
#if P_SIZE>32

// use SSE of not, defined by kk
//#define USE_SSE
#define SSE_BOARD
#endif

#define _CRT_SECURE_NO_WARNINGS

#define STACK_MAX_DEPTH S_SIZE + 1

#define SENDQUEUE_IN_INVERSE_LINKS

#ifndef SSE_BOARD
#define COPY_PROBING_NODES
#endif
// #define CHECK_DT
// #define DEBUG_TREENODES

// #define ENABLE_BACKJUMPING
// #define ENABLE_BACKJUMPING_DEBUG

// #define TWO_SAT
// #define DT

#define MOVE_LOG_NUMBER 1.85
#define THREADHOLD_FULLYPROBING P_SIZE*P_SIZE

// CONFIG
#define NUM_OF_QUESTIONS 1000
#define MODE  2 //schdule 1: unique solution 2: multiple 3: multiple and dump all answers.
#define INPUT_FILE "input.txt"
#define INPUT_STATE "input_state.txt"
#define OUTPUT_FILE "solution.txt"
#define LOF_FILE "log.txt"   
#define LIGHT_NODE_LIMITED 15000
#define HEAVT_NODE_LIMITED 60000

// algorithm
#define DOING_INVERSE_LINKS // no sat, inverse link only.
// #define DOING_INVERSE_LINKS_BY_COPY_ONE // fully probing

// INFORMATION, maybe not work anymore.
//#define BEFORE_PRE_SEARCH
//#define ONE_LAYER
//#define PRE_SEARCH_DETAIL

// 每次集中火力解幾題
#define SCHDULE_NUM_QUESTIONS 10

// move
#define WEIGHTED // move ordering
  // #define ADD_MOVE
  // #define MUL_MOVE
  //  #define MIN_MOVE
  // #define MAX_MOVE
   #define MIN_LOG_MOVE
  // #define LOG_MOVE
  // #define SQRT_MOVE
  // #define RANDOM_MOVE
  //#define LINE_WEIGHTED_MOVE



/* for search return value */
#define SOLVED 0
#define UNSOLVED 1
#define CONFLICT 2
#define TIMEOUT 3
#define MANY_SOLUTION 4
#define DO_PRE_SEARCH 5
#define UNCHANGE 6
#define CHANGE 7


#endif 
