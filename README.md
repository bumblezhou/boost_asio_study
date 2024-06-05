## A simple project to study boost.asio, based boost.asio version 1.85.0

## How to build
1. install cmake and build-essential
```bash
sudo apt update
sudo apt install cmake

sudo apt update
sudo apt install build-essential

sudo apt update
sudo apt install libboost-all-dev
```
2. install VSCode
3. install "C/C++ Extension Pack", "CMake Tools" extensions for VSCode
4. how to build
    * VSCode -> CTRL + SHIFT + P -> CMake: Configure
    * VSCode -> CTRL + SHIFT + P -> CMake: Build Target -> all

## How to run
1. timer_1_synchronously
```bash
./build/src/tutorial/timer_1_synchronously
```

2. timer_2_asynchronously
```bash
./build/src/tutorial/timer_2_asynchronously
```

3. timer_3_binding_arguments_to_a_handler
```bash
./build/src/tutorial/timer_3_binding_arguments_to_a_handler
```

4. timer_4_using_member_fun_as_handler
```bash
./build/src/tutorial/timer_4_using_member_fun_as_handler
```

5. tcp daytime client and tcp servers of sync and async
    * sync tcp server && tcp client
    ```bash
    [terminal 1]
    sudo ./build/src/tutorial/sync_tcp_server

    [terminal 2]
    ./build/src/tutorial/tcp_client localhost
    ```

    * async tcp server && tcp client
    ```bash
    [terminal 1]
    sudo ./build/src/tutorial/async_tcp_server

    [terminal 2]
    ./build/src/tutorial/tcp_client localhost
    ```
6. udp daytime client and udp servers of sync and async
    * sync udp server && udp client
    ```bash
    [terminal 1]
    sudo ./build/src/tutorial/sync_udp_server

    [terminal 2]
    ./build/src/tutorial/udp_client localhost
    ```

    * async udp server && udp client
    ```bash
    [terminal 1]
    sudo ./build/src/tutorial/async_udp_server

    [terminal 2]
    ./build/src/tutorial/udp_client localhost
    ```

7. combined async tcp and udp server
    * async tcp udp combined server && tcp/udp client
    ```bash
    [terminal 1]
    sudo ./build/src/tutorial/async_tcp_udp_server

    [terminal 2]
    ./build/src/tutorial/tcp_client localhost

    [terminal 3]
    ./build/src/tutorial/udp_client localhost
    ```

8. http server v1
    * open http server v1: A simple single-threaded server implementation of HTTP 1.0
    * Usage: http_server1 <address> <port> <doc_root>
    ```bash
    [terminal 1]
    sudo ./build/src/examples/c++11/http_server1/http_server1 0.0.0.0 80 ./static/
    ```
    * open service it served:
    ```bash
    [browser]
    http://localhost/
    ```

9. http server v2
    * open http server v2: An HTTP server using an io_context-per-CPU design.
    * Usage: http_server2 <address> <port> <threads> <doc_root>
    ```bash
    [terminal 1]
    sudo ./build/src/examples/c++11/http_server2/http_server2 0.0.0.0 80 3 ./static/
    ```
    * open service it served:
    ```bash
    [browser]
    http://localhost/
    ```

10. http server v3
    * open http server v3: An HTTP server using a single io_context and a thread pool calling io_context::run().
    * Usage: http_server3 <address> <port> <threads> <doc_root>
    ```bash
    [terminal 1]
    sudo ./build/src/examples/c++11/http_server3/http_server3 0.0.0.0 80 4 ./static/
    ```
    * open service it served:
    ```bash
    [browser]
    http://localhost/
    ```

11. http server v4
    * open http server v4: A single-threaded HTTP server implemented using stackless coroutines.
    * Usage: http_server4 <address> <port> <doc_root>
    ```bash
    [terminal 1]
    sudo ./build/src/examples/c++11/http_server4/http_server4 0.0.0.0 80 ./static/
    ```
    * open service it served:
    ```bash
    [browser]
    http://localhost/
    ```

12. chat
    * This example implements a chat server and client. The programs use a custom protocol with a fixed length message header and variable length message body.
    * Usage: chat_server <host> <port>
    * Usage: chat_client <host> <port>
    ```bash
    [terminal 1]
    sudo ./build/src/examples/c++11/chat/chat_server local 8080
    [terminal 2]
    sudo ./build/src/examples/c++11/chat/chat_client local 8080
    ```

13. Operations
    * Examples showing how to implement composed asynchronous operations as reusable library functions.
    * Usage: composed_1
    ```bash
    sudo ./build/src/examples/c++11/operations/composed_1
    ```


## How to do performance benchmarking for http servers:
    * Install Apache Bench
    ```bash
    sudo apt update && sudo apt install apache2-utils -y
    ```
    * Do performance benchmarking:
    ```bash
    ap -n <total_requests> -c <concurrenct_count> http://target_url
    ab -n 1000 -c 10 http://localhost/
    ```