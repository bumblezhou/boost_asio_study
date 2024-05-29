#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
using namespace std;

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

class udp_server {
public:
    udp_server(boost::asio::io_context& io) : socket_(io, udp::endpoint(udp::v4(), 13)) {
        cout << "[udp_server] start_receive();" << endl;
        start_receive();
    }
private:
    void start_receive() {
        boost::system::error_code ignored_error;
        size_t bytes_transferred;
        cout << "[udp_server::start_receive] socket_.async_receive_from(buf, remote_endpoint, bind(handle_receive...)...);" << endl;
        socket_.async_receive_from(boost::asio::buffer(recv_buf_), remote_endpoint_, boost::bind(&udp_server::handle_receive, this, ignored_error, bytes_transferred));
    }
    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
        if(!error) {
            cout << "[udp_server::handle_receive] make_daytime_string()" << endl;
            boost::shared_ptr<string> msg(new string(make_daytime_string()));
            boost::system::error_code ignored_error;
            size_t bytes_transferred;
            cout << "[udp_server::handle_receive] socket_.async_send_to(msg, remote_endpoint, bind(handle_send, ...), ...);" << endl;
            socket_.async_send_to(boost::asio::buffer(*msg), remote_endpoint_, boost::bind(&udp_server::handle_send, this, msg, ignored_error, bytes_transferred));
        }
        cout << "[udp_server::handle_receive] start_receive();" << endl;
        start_receive();
    }
    void handle_send(boost::shared_ptr<string> msg, const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/) {
        cout << "[udp_server::handle_send] do nothing..." << endl;
    }
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1> recv_buf_;
};

int main() {
    try
    {
        boost::asio::io_context io;
        cout << "[main] udp_server server(io);" << endl;
        udp_server server(io);
        cout << "[main] io.run();" << endl;
        io.run();
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    
    return 0;
}