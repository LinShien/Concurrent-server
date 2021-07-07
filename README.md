# Concurrent-server
Practice network programming and typical server models. [Tutorial here](https://eli.thegreenplace.net/2017/concurrent-servers-part-1-introduction/)

## Sequential Server

*A very simple server that can only serve one client in order at a time.*

> * Client side: using multithreading to connect to the sequential server at the same time
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/2021-07-04%2018-03-51%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png" width=700>

> * Server side: using a blocking function called serve_connection() to server a client socket at a time
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/2021-07-04%2018-04-35%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png" width=700>

---

## Multithreaded Server
*A server that use a new thread to handle every client socket connection

> * Client side: using multithreading to connect to the sequential server at the same time
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/2021-07-05%2001-21-44%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png" width=700>

> * Server side: using threads to handle client sockets connections concurrently
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/2021-07-05%2001-21-30%20%E7%9A%84%E8%9E%A2%E5%B9%95%E6%93%B7%E5%9C%96.png" width=700>

## Useful references to network programming and multi-thread programming
* [C/C++ socket lib guide and reference](https://beej-zhtw-gitbook.netdpi.net/)
* [C++ multithreading guide](https://medium.com/ching-i/%E5%A4%9A%E5%9F%B7%E8%A1%8C%E7%B7%92-c-thread-9f6e37c7cf32)
* [C socket programming guide](https://www.geeksforgeeks.org/socket-programming-cc/)
* [thread::join() vs thread::detach()](https://www.twblogs.net/a/5b84dea72b71775d1cd2336c)
* [C++11 ThreadPool implementation](https://wangpengcheng.github.io/2019/05/17/cplusplus_theadpool/#%E7%BA%BF%E7%A8%8B%E6%B1%A0%E5%B7%A5%E4%BD%9C%E7%9A%84%E5%9B%9B%E7%A7%8D%E6%83%85%E5%86%B5)
* [C++ std::bind explanation](https://www.jyt0532.com/2017/01/08/bind/)
* [C++ std::forward explanation](https://tjsw.medium.com/%E6%BD%AE-c-11-perfect-forwarding-%E5%AE%8C%E7%BE%8E%E8%BD%89%E7%99%BC%E4%BD%A0%E7%9A%84%E9%9C%80%E6%B1%82-%E6%B7%B1%E5%BA%A6%E8%A7%A3%E6%9E%90-f)
* [C++ std::packaged_task 教學](https://blog.csdn.net/c_base_jin/article/details/79541301)
* [How to solve Most Vexing Parse problems in C++](https://byvoid.com/zht/blog/cpp-most-vexing-parse/)
