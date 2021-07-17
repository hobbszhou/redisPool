#include "redisPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <hiredis/hiredis.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <time.h>
#include <string.h>

int opcRedisPoolInit(redisPool_t **poolTmp, const char *ipTmp, const char *passwdTmp, const int portTmp, const int dbTmp, const int maxCntTmp)
{
    redisPool_t *newPool = NULL;
    newPool = (redisPool_t *)malloc(sizeof(redisPool_t));
    snprintf(newPool->ip, sizeof(newPool->ip), "%s", ipTmp);
    snprintf(newPool->passwd, sizeof(newPool->passwd), "%s", passwdTmp);
    newPool->port = portTmp;
    newPool->dataBase = dbTmp;
    newPool->used = 0;
    newPool->maxCnt = maxCntTmp;
    newPool->freePosi = NULL;
    newPool->redisNode = NULL;
    newPool->block = 0;
    newPool->cntSum = 0;
    newPool->initPoolFlag = 0;
    pthread_mutex_init(&(newPool->_popMutex), NULL);
    pthread_cond_init(&(newPool->_pCond), NULL);
    if (-1 == initOpcRedisCnt(&newPool, maxCntTmp))
    {
        printf("error: opcRedisPoolInit failed\n");
        return -1;
    }
    (*poolTmp) = newPool;
    printf("opcRedisPoolInit success\n");
    return 0;
}
int initOpcRedisCnt(redisPool_t **poolTmp, const int maxCntTmp)
{
    ++(*poolTmp)->block;
    redisNode_t *head = NULL;
    redisNode_t *pb = NULL;
    int i = 0;
    for (i = 0; i < maxCntTmp; ++i)
    {
        redisContext *conn = NULL;
        conn = redisConnect((*poolTmp)->ip, (*poolTmp)->port);
        redisReply *reply = redisCommand(conn, "AUTH %s", (*poolTmp)->passwd);
        if (conn->err && NULL != conn)
        {
            printf("error: initClient failed\n");
            freeReplyObject(reply);
            return -1;
        }
        reply = redisCommand(conn, "select %d", (*poolTmp)->dataBase);
        if (NULL == reply)
        {
            printf("error: null\n");
            return -1;
        }
        if (REDIS_REPLY_ERROR == reply->type)
        {
            printf("error: set database failed, db=%d\n", (*poolTmp)->dataBase);
            freeReplyObject(reply);
            return -1;
        }
        freeReplyObject(reply);
        redisNode_t *newTmp = NULL;
        newTmp = (redisNode_t *)malloc(sizeof(redisNode_t));
        newTmp->conn = conn;
        ++(*poolTmp)->cntSum;
        newTmp->cntIdx = (*poolTmp)->cntSum;
        newTmp->usedFlag = 0;

        if (NULL == head)
        {
            head = newTmp;
            newTmp->next = NULL;
            pb = head;
        }
        else
        {
            pb->next = newTmp;
            newTmp->next = NULL;
            pb = newTmp;
        }
    }
    pb->next = NULL;
    (*poolTmp)->redisNode = head;
    (*poolTmp)->freePosi = head;
    (*poolTmp)->initPoolFlag = 1;
    return 0;
}
redisNode_t *popClient(redisPool_t **poolTmp)
{
    pthread_mutex_lock(&((*poolTmp)->_popMutex));
    usleep(20);
again:
    if (NULL != (*poolTmp)->freePosi)
    {
        (*poolTmp)->freePosi->usedFlag = 1;
        redisNode_t *curClient = (*poolTmp)->freePosi;
        (*poolTmp)->freePosi = ((*poolTmp)->freePosi)->next;
        ++((*poolTmp)->used);

        pthread_mutex_unlock(&((*poolTmp)->_popMutex));
        return curClient;
    }
    else
    {
        pthread_cond_wait(&((*poolTmp)->_pCond), &(*poolTmp)->_popMutex);
        goto again;
    }
    printf("error: i > 200000, i > 200000, i > 200000\n");
    pthread_mutex_unlock(&((*poolTmp)->_popMutex));
    return NULL;
}
int pushClient(redisPool_t **poolTmp, redisNode_t *curClient)
{
    pthread_mutex_lock(&((*poolTmp)->_popMutex));
    if (NULL != curClient)
    {
        if (1 == curClient->usedFlag)
        {
            curClient->usedFlag = 0;
            curClient->next = (*poolTmp)->freePosi;
            (*poolTmp)->freePosi = curClient;
            --((*poolTmp)->used);
        }
        else
        {
            printf("error: already release\n");
        }
    }
    else
    {
        printf("error: curClient is null\n");
    }
    pthread_cond_signal(&((*poolTmp)->_pCond));
    pthread_mutex_unlock(&((*poolTmp)->_popMutex));
    return 0;
}
int opcRedisPoolFree(redisPool_t **poolTmp)
{
    redisNode_t *tmp = popClient(poolTmp);
    while (NULL != tmp)
    {
        tmp->cntIdx = -1;
        tmp->usedFlag = 0;
        if (NULL != tmp->conn)
        {
            redisFree(tmp->conn);
        }
        tmp = popClient(poolTmp);
    }
    (*poolTmp)->port = -1;
    (*poolTmp)->dataBase = -1;
    (*poolTmp)->used = 0;
    (*poolTmp)->maxCnt = 0;
    (*poolTmp)->block = 0;
    (*poolTmp)->cntSum = 0;
    (*poolTmp)->initPoolFlag = 0;
    (*poolTmp)->freePosi = NULL;
    (*poolTmp)->redisNode = NULL;
    memset((*poolTmp)->ip, 0, sizeof((*poolTmp)->ip));
    memset((*poolTmp)->passwd, 0, sizeof((*poolTmp)->passwd));
    pthread_mutex_destroy(&((*poolTmp)->_popMutex));
    pthread_cond_destroy(&((*poolTmp)->_pCond));
    return 0;
}
