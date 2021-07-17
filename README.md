# redisPool
# hiredis连接池
```
    受内存池思想的启发，纯手写的一套hiredis连接池的代码。采用条件变量和互斥锁，该套代码是并发安全，已经做过开启
1000个线程并发测试，不会存在增删改查redis失败的情况，已用于商业项目。
```
### 1、前提条件

    1. 得安装redis及hiredis库
    2. 得安装cmake
### 2、编译
1. 首先得确定你电脑有没有装cmake，没装的话得装一下
2. 采用CMake外部构建的方法，所以你得输入下面命令:
``` bash
    mkdir build
    cd build
    cmake ..
    make
```
至此编译成功!

### 3、 运行
``` bash
    进入build下的bin目录:
    cd bin
    ./opcredisPool
```

### 4、快速开始

- **首先得初始化连接池**

```c
    // 示例:  建立一个连接不同 db 的连接池
    redisPool_t *gRpool1 = NULL;                                                                 
    if (-1 == opcRedisPoolInit(&gRpool1, redisIp, redisPassword, redisPort, redisDataBase1, redisPoolSize)) // 初始化连接池
    {
        return -1;
    }

    
```

- **从连接池中取出一个连接进行操作**

```c
    redisNode_t *conTmp1 = popClient(&gRpool1); // 从连接池中取出一个连接
    redisReply *reply = redisCommand(conTmp1->conn, "HSET %s %s %s", "aaaaaaaa", "bbbbbbbbbb", "cccccccccc");
    pushRpoolCnt(conTmp1, key); // 用完记得放入连接池


```

- **彻底释放连接**

```c
// 当你再也不需要使用redis的时候进行彻底释放
opcRedisPoolFree(&gRpool1);


```











