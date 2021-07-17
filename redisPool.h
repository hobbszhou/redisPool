#ifndef _OPC_REDIS_POOL_
#define _OPC_REDIS_POOL_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <hiredis/hiredis.h>
#include <pthread.h>
#define STRLEN 128

#define redisIp "1.1.1.1"
#define redisPassword "123456"
#define redisPort 6379
#define redisDataBase1 15
#define redisDataBase2 16
#define redisPoolSize 120
typedef struct _REDIS_NODE_T
{
    redisContext *conn;
    int cntIdx;
    int usedFlag;
    struct _REDIS_NODE_T *next;
} redisNode_t, *pRedisNode_t;
typedef struct _REDIS_POOL_T
{
    char ip[STRLEN];
    char passwd[STRLEN];
    int port;
    int dataBase;
    int used;
    int maxCnt;
    int block;
    int cntSum;
    int initPoolFlag;
    pthread_mutex_t _popMutex;
    pthread_cond_t _pCond;
    redisNode_t *freePosi;
    redisNode_t *redisNode;

} redisPool_t, *pRedisPool_t;
int opcRedisPoolInit(redisPool_t **poolTmp, const char *ipTmp, const char *passwdTmp, int portTmp, const int dbTmp, const int maxCntTmp);
int initOpcRedisCnt(redisPool_t **poolTmp, const int maxCntTmp);
redisNode_t *popClient(redisPool_t **poolTmp);
int pushClient(redisPool_t **poolTmp, redisNode_t *curClient);
int opcRedisPoolFree(redisPool_t **poolTmp);

#endif