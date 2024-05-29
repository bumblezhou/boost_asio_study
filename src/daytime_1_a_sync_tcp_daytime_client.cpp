#include <array>
#include <iostream>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try{
        if (argc != 2) {
            cerr << "Usage: client <host>" << endl;
            return 1;
        }
        boost::asio::io_context io;
        cout << "[main] init tcp resolver..." << endl;
        tcp::resolver resolver(io);
        cout << "[main] resolve remote endpoint..." << endl;
        auto endpoints = resolver.resolve(argv[1], "daytime");
        cout << "[main] init socket..." << endl;
        tcp::socket socket(io);
        cout << "[main] connect socket of remote endpoint..." << endl;
        boost::asio::connect(socket, endpoints);

        for(;;) {
            std::array<char, 128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);
            cout << "[main] socket.read_some(buf)..." << endl;
            if (error == boost::asio::error::eof) {
                cout << "[main] nothing to read..." << endl;
                break;
            } else if (error) {
                throw boost::system::system_error(error);
            }
            cout.write(buf.data(), len);
        }
    } catch (std::exception& e) {
        cerr << e.what() << endl;
    }
    return 0;
}