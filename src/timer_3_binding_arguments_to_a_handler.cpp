#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using namespace std;

void print(const boost::system::error_code& /*e*/, boost::asio::steady_timer* t, int* count) {
    if(*count < 5) {
        cout << "[print] *count:" << *count << endl;
        ++(*count);
        cout << "[print] ++(*count);" << endl;
        cout << "[print] expiry() + 1" << endl;
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
        cout << "[print] t->async_wait()" << endl;
        boost::system::error_code ignored_error;
        t->async_wait(boost::bind(print, ignored_error, t, count));
    }
}

int main() {
    boost::asio::io_context io;
    int count = 0;
    cout << "[main] count:" << count << std::endl;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

    cout << "[main] t.async_wait(boost::bind(...));" << endl;
    boost::system::error_code ignored_error;
    t.async_wait(boost::bind(print, ignored_error, &t, &count));
    
    cout << "[main] io.run()" << endl;
    io.run();
    cout << "[main] count:" << count << std::endl;

    return 0;
}