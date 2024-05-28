#include <iostream>
#include <boost/asio.hpp>

using namespace std;

void print(const boost::system::error_code& /*d*/) {
    cout << "[print] Hello, world!" << endl;
}

int main() {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(2));

    cout << "[main] t.async_wait(&print);" << endl;
    t.async_wait(&print);

    cout << "[main] io.run();" << endl;
    io.run();

    return 0;
}