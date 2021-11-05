#include <string.h>
#include "redisPool.h"

int main(int argc, char *argv[])
{
    // 示例:  建立两个连接不同 db 的连接池
    redisPool_t *gRpool1 = NULL;                                                                            // 连接 db=15
    redisPool_t *gRpool2 = NULL;                                                                            // 连接 db=16
    if (-1 == opcRedisPoolInit(&gRpool1, redisIp, redisPassword, redisPort, redisDataBase1, redisPoolSize)) // 初始化连接池
    {
        return -1;
    }

    if (-1 == opcRedisPoolInit(&gRpool2, redisIp, redisPassword, redisPort, redisDataBase2, redisPoolSize)) // 初始化连接池
    {
        return -1;
    }
    redisNode_t *conTmp1 = popClient(&gRpool1); // 从连接池1中取出一个连接
    redisReply *reply = redisCommand(conTmp1->conn, "HSET %s %s %s", "aaaaaaaa", "bbbbbbbbbb", "cccccccccc");
    pushClient(&gRpool1, conTmp1); // 用完记得放入连接池

    redisNode_t *conTmp2 = popClient(&gRpool2); // 从连接池1中取出一个连接
    redisReply *reply = redisCommand(conTmp1->conn, "HSET %s %s %s", "aaaaaaaa", "bbbbbbbbbb", "cccccccccc");
    pushClient(&gRpool2, conTmp2); // 用完记得放入连接池

    // 彻底释放连接
    opcRedisPoolFree(&gRpool1);
    opcRedisPoolFree(&gRpool2);

    return 0;
}