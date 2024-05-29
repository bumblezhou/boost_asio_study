#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
    typedef boost::shared_ptr<tcp_connection> ptr_tcp_conn;
    static ptr_tcp_conn create(boost::asio::io_context& io) {
        cout << "[tcp_connection] create();" << endl;
        return ptr_tcp_conn(new tcp_connection(io));
    }

    tcp::socket& socket() {
        return socket_;
    }

    void start() {
        cout << "[tcp_connection::start] msg_ = make_daytime_string();" << endl;
        msg_ = make_daytime_string();
        cout << "[tcp_connection::start] boost::asio::async_write(socket_, msg, ...);" << endl;
        boost::system::error_code ignored_error;
        size_t bytes_transferred;
        boost::asio::async_write(socket_, boost::asio::buffer(msg_), boost::bind(&tcp_connection::handle_write, shared_from_this(), ignored_error, bytes_transferred));
    }
private:
    tcp_connection(boost::asio::io_context& io) : socket_(io) {}
    void handle_write(const boost::system::error_code& /*e*/, size_t /*bytes_transferred*/) {}
    tcp::socket socket_;
    string msg_;
};

class tcp_server {
public:
    tcp_server(boost::asio::io_context& io) : io_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 13)) {
        cout << "[tcp_server] start_accept();" << endl;
        start_accept();
    }
private:
    void start_accept() {
        cout << "[tcp_server::start_accept] new_conn = tcp_connection::create(io_);" << endl;
        tcp_connection::ptr_tcp_conn new_conn = tcp_connection::create(io_);
        cout << "[tcp_server::start_accept] acceptor_.async_accept(...);" << endl;
        boost::system::error_code ignored_error;
        acceptor_.async_accept(new_conn->socket(), boost::bind(&tcp_server::handle_accept, this, new_conn, ignored_error));
    }
    void handle_accept(tcp_connection::ptr_tcp_conn new_conn, const boost::system::error_code& error) {
        if (!error) {
            cout << "[tcp_server::handle_accept] new_conn->start();" << endl;
            new_conn->start();
        }
        cout << "[tcp_server::handle_accept] start_accept();" << endl;
        start_accept();
    }
    boost::asio::io_context& io_;
    tcp::acceptor acceptor_;
};

int main() {
    try
    {
        boost::asio::io_context io;
        cout << "[main] tcp_server server(io);" << endl;
        tcp_server server(io);
        cout << "[main] io.run();" << endl;
        io.run();
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    
}