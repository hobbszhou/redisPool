# redisPool
# hiredis连接池
```
    受内存池启发，纯手撸的一套hiredis连接池的代码。采用条件变量和互斥锁，该套代码是并发安全，已经做过开启
10000个线程并发测试，不会存在增删改查redis存在失败的情况，已用于商业项目。欢迎点赞，投币+关注，一键三连
```
#### 一、前提条件
    1. 得安装redis及hiredis库
    2. 得安装cmake
#### 二、编译
1. 首先得确定你电脑有没有装cmake，没装的话得装一下
2. 采用CMake外部构建的方法，所以你得输入下面命令:
``` bash
    mkdir build
    cd build
    cmake ..
    make
```
    至此编译成功
#### 三、 运行
``` bash
    进入build下的bin目录:
    cd bin
    ./opcredisPool
```