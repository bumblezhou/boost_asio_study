#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using namespace std;

class printer {
public:
    printer(boost::asio::io_context& io) : timer_(io, boost::asio::chrono::seconds(1)), count_(0) {
        timer_.async_wait(boost::bind(&printer::print, this));
    }
    ~printer() {
        cout << "[printer->~print] count:" << count_ << endl;
    }
    void print() {
        if(count_ < 5) {
            cout << "[printer->print] count:" << count_ << endl;
            ++count_;
            cout << "[printer->print] expires_at(expiry() + 1)" << endl;
            timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
            cout << "[printer->print] async_wait(bind(...))" << endl;
            timer_.async_wait(boost::bind(&printer::print, this));
        }
    }
private:
    boost::asio::steady_timer timer_;
    int count_;
};

int main() {
    boost::asio::io_context io;
    cout << "[main] printer p(io);" << endl;
    printer p(io);
    cout << "[main] io.run();" << endl;
    io.run();

    return 0;
}