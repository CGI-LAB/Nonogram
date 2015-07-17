#ifndef _CONFIG_H_
#define _CONFIG_H_


#define P_SIZE 25
#define S_SIZE P_SIZE * P_SIZE
#define _CRT_SECURE_NO_WARNINGS

#define STACK_MAX_DEPTH 626


/*use hash tables?*/
#define _USE_QUEUE_

/* for search return value */
#define SOLVED 0
#define UNSOLVED 1
#define CONFLICT 2
#define TIMEOUT 3
#define MANY_SOLUTION 4



// CONFIG
#define NUM_OF_QUESTIONS 1000
#define MODE 1 // 0 schdule 
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "solution.txt"
#define LOG_FILE "log.txt"   
#define LIGHT_NODE_LIMITED 15000
#define HEAVT_NODE_LIMITED 60000


// 每次集中火力解幾題
#define SCHDULE_NUM_QUESTIONS 10

#endif 