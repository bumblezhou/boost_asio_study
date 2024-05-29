#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::udp;

int main(int argc, char* argv[]) {
    try
    {
        if ( argc != 2 ) {
            cerr << "Usage: udp_client <host>" << endl;
            return 1;
        }
        boost::asio::io_context io;
        cout << "[main] init udp resolver..." << endl;
        udp::resolver resolver(io);
        cout << "[main] resolve receiver_endpoint..." << endl;
        udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), argv[1], "daytime").begin();
        cout << "[main] init udp socket..." << endl;
        udp::socket socket(io);
        cout << "[main] socket.open()..." << endl;
        socket.open(udp::v4());

        boost::array<char, 1> send_buf = {{0}};
        cout << "[main] socket.send(buf, receiver_endpoint)..." << endl;
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        boost::array<char, 128> recv_buf;
        cout << "[main] init sender_endpoint..." << endl;
        udp::endpoint sender_endpoint;
        cout << "[main] socket.receive_from(buf, sender_endpoint)..." << endl;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
        cout.write(recv_buf.data(), len);
    }
    catch(const std::exception& e)
    {
        cerr << e.what() << endl;
    }
    
    return 0;
}