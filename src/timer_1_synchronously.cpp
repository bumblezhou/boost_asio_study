#include <iostream>
#include <boost/asio.hpp>

using namespace std;

int main() {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(2));

    cout << "[main] t.wait();" << endl;
    t.wait();

    std::cout << "[main] Hello, world!" << std::endl;
    
    return 0;
}