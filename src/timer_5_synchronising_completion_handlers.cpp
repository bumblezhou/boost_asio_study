#include <functional>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

using namespace std;

class printer {
public:
    printer(boost::asio::io_context& io) 
        : strand_(boost::asio::make_strand(io)),
        timer1_(io, boost::asio::chrono::seconds(1)),
        timer2_(io, boost::asio::chrono::seconds(1)),
        count_(0) {
        cout << "[printer] count:" << count_ << endl;
        cout << "[printer] timer1_.async_wait()..." << endl;
        timer1_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print1, this)));
        cout << "[printer] timer2_.async_wait()..." << endl;
        timer2_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print2, this)));
    }
    ~printer() {
        cout << "[~printer] count:" << count_ << endl;
    }
    void print1() {
        if (count_ < 10) {
            cout << "[print1] count:" << count_ << endl;
            ++count_;
            cout << "[print1] timer1.expires_at(expiry() + 1)" << endl;
            timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
            cout << "[print1] timer1.async_wati(...)" << endl;
            timer1_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print1, this)));
        }
    }
    void print2() {
        if (count_ < 10) {
            cout << "[print2] count:" << count_ << endl;
            ++count_;
            cout << "[print2] timer2_.expires_at(expiry() + 1)" << endl;
            timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
            cout << "[print2] timer2_.async_wati(...)" << endl;
            timer2_.async_wait(boost::asio::bind_executor(strand_, std::bind(&printer::print2, this)));
        }
    }
private:
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer1_;
    boost::asio::steady_timer timer2_;
    int count_;
};

int main() {
    boost::asio::io_context io;
    cout << "[main] printer p(io);" << endl;
    printer p(io);
    cout << "[main] std::thread t([&]{ io.run(); });" << endl;
    std::thread t([&]{ io.run(); });
    if (t.joinable()) {
        t.join();
    }
    return 0;
}