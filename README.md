# Concurrent-server
Practice network programming and typical server models. [Tutorial here](https://eli.thegreenplace.net/2017/concurrent-servers-part-1-introduction/)

* [Sequential Server](#SequentialServer)
* [Multithreaded Server](#MultithreadedServer)
* [Thread Pools based Concurrent Server](#ThreadPoolServer)
* [Event-Driven Concurrent Server](#SelectServer)

## <a name="SequentialServer"> Sequential Server </a>

*A very simple server that can only serve one client in order at a time.*

> * Client side: using multithreading to connect to the sequential server at the same time.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/sequential-client.png" height=500>

> * Server side: using a blocking function called serve_connection() to server a client socket at a time.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/sequential-server.png" width=600 height=400>

---

## <a name="MultithreadedServer"> Multithreaded Server </a>

*A server that use a new thread to handle every client socket connection.*

> * **Client side**: 
> using multithreading to connect to the sequential server at the same time.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/multithreaded-client.png" height=500>

> * **Server side**: 
> using threads to handle client sockets connections concurrently.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/multithreaded-server.png" width=600 height=400>

---

## <a name="ThreadPoolServer"> Thread Pools based Concurrent Server </a>

*A server that use a threadpool with limited number of threads to handle every client socket connection.*

> * **Client side**: 
> using multithreading to connect to the sequential server at the same time.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/threadpool-client.png" height=500>

> * **Server side**: 
> using threadpool to handle client sockets connections concurrently.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/threadpool-server.png" width=600 height=400>

---

## <a name="SelectServer"> Event-Driven Concurrent Server </a>
*A server that use a just one thread and the feature of select() function to monitor the file descriptors and pending events to them, then handle every client socket connection with some callback functions. There is not multithreading involved*

*This kind of concurrent servers also called event-driven, callback-based(like javascript), asynchronous concurrent server.*

> * **Client side**: 
> using multithreading to connect to the sequential server at the same time.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/select-client.png" height=500>

> * **Server side**: 
> using a event for loop to keep polling and checking whether there are upcoming events to monitored file descriptors and use specific callbacks and states to handle the upcoming events.
<img src="https://github.com/LinShien/Concurrent-server/blob/main/demo/select-server.png" width=600 height=400>

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
