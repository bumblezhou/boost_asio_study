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
4. build
    * VSCode -> CTRL + SHIFT + P -> CMake: Configure
    * VSCode -> CTRL + SHIFT + P -> CMake: Build Target -> all

## How to run
1. timer_1_synchronously
```bash
./build/src/timer_1_synchronously
```

2. timer_2_asynchronously
```bash
./build/src/timer_2_asynchronously
```

3. timer_3_binding_arguments_to_a_handler
```bash
./build/src/timer_3_binding_arguments_to_a_handler
```

4. timer_4_using_member_fun_as_handler
```bash
./build/src/timer_4_using_member_fun_as_handler
```

5. tcp daytime client and tcp servers of sync and async
    * sync tcp server && tcp client
    ```bash
    [terminal 1]
    sudo ./build/src/sync_tcp_server

    [terminal 2]
    ./build/src/tcp_client localhost
    ```

    * async tcp server && tcp client
    ```bash
    [terminal 1]
    sudo ./build/src/async_tcp_server

    [terminal 2]
    ./build/src/tcp_client localhost
    ```
6. udp daytime client and udp servers of sync and async
    * sync udp server && udp client
    ```bash
    [terminal 1]
    sudo ./build/src/sync_udp_server

    [terminal 2]
    ./build/src/udp_client localhost
    ```

    * async udp server && udp client
    ```bash
    [terminal 1]
    sudo ./build/src/async_udp_server

    [terminal 2]
    ./build/src/udp_client localhost
    ```