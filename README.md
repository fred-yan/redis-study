# redis-study
## 1 ae事件
### 1.1 源代码说明 
代码地址：fred-yan/redis-study/ae

参考网址：https://my.oschina.net/u/917596/blog/161077

代码抽取redis/src/下面的 ae.c  ae.h  ae_epoll.c  ae_evport.c  ae_kqueue.c  ae_select.c文件，利用malloc，free和rellaoc函数替代了jmalloc的zmalloc，zfree和zrellaoc函数，编写了Makefile和示例函数test.c

### 1.2 编译说明
编译： make

执行用例： ./test

删除： make clean
