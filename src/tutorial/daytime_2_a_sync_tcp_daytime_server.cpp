#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace std;

string make_daytime_string() {
    time_t now = time(0);
    return ctime(&now);
}

int main() {
    try
    {
        boost::asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 13));
        cout << "[main] daytime server is running." << endl;
        for(;;) {
            tcp::socket socket(io);
            acceptor.accept(socket);
            cout << "[main] accept request" << endl;
            string msg = make_daytime_string();
            boost::system::error_code ignored_error;
            cout << "[main] response msg:" << msg << endl;
            boost::asio::write(socket, boost::asio::buffer(msg), ignored_error);
        }
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    return 0;   
}