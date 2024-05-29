#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::udp;

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    try
    {
        boost::asio::io_context io;
        cout << "[main] init socket with local udp endpoint..." << endl;
        udp::socket socket(io, udp::endpoint(udp::v4(), 13));
        for(;;) {
            boost::array<char, 1> recv_buf;
            cout << "[main] init remote udp endpoint..." << endl;
            udp::endpoint remote_endpoint;
            cout << "[main] socket.receive_from(buf, remote_endpoint)..." << endl;
            socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);

            string msg = make_daytime_string();
            boost::system::error_code ignored_error;
            cout << "[main] socket.send_to(buf, remote_endpoint)..." << endl;
            socket.send_to(boost::asio::buffer(msg), remote_endpoint, 0, ignored_error);
        }
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    
    return 0;
}