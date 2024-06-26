#include <iostream>
#include <boost/asio.hpp>
#include <vector>

using boost::asio::ip::tcp;

void socks5_handshake_and_request(tcp::socket& socket, const std::string& dest_host, unsigned short dest_port) {
    boost::system::error_code ec;

    // SOCKS5 greeting
    std::vector<uint8_t> request = {0x05, 0x01, 0x00}; // VER, NMETHODS, METHODS
    boost::asio::write(socket, boost::asio::buffer(request), ec);
    if (ec) {
        throw boost::system::system_error(ec);
    } else {
        std::cout << "[socks5_client] send SOCKS5 handshake greeting request.\n";
    }

    // Server response to greeting
    std::vector<uint8_t> response(2);
    boost::asio::read(socket, boost::asio::buffer(response), ec);
    if (ec || response[1] != 0x00) {
        throw boost::system::system_error(ec);
    } else {
        std::cout << "[socks5_client] SOCKS5 handshake successfull.\n";
    }

    // SOCKS5 connection request
    std::vector<uint8_t> connect_request = {
        0x05, // VER
        0x01, // CMD (CONNECT)
        0x00, // RSV
        0x03, // ATYP (Domain name)
        static_cast<uint8_t>(dest_host.size())
    };
    connect_request.insert(connect_request.end(), dest_host.begin(), dest_host.end());
    connect_request.push_back((dest_port >> 8) & 0xFF); // Port high byte
    connect_request.push_back(dest_port & 0xFF);        // Port low byte

    boost::asio::write(socket, boost::asio::buffer(connect_request), ec);
    if (ec) {
        throw boost::system::system_error(ec);
    } else {
        std::cout << "[socks5_client] send SOCKS5 http request.\n";
    }

    // Server response to connection request
    std::vector<uint8_t> connect_response(10);
    boost::asio::read(socket, boost::asio::buffer(connect_response), ec);
    if (ec || connect_response[1] != 0x00) {
        throw boost::system::system_error(ec);
    } else {
        std::cout << "[socks5_client] socks5 server is ready to request the target server...\n";
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Set up the SOCKS5 proxy connection
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "2080");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        std::cout << "[socks5_client] connect socks5 server successfully.\n";

        // Set the destination address and port
        std::string dest_host = "salonchow.me";
        // std::string dest_host = "www.solidot.org";
        unsigned short dest_port = 8443;

        // Perform the SOCKS5 connection
        socks5_handshake_and_request(socket, dest_host, dest_port);

        // Read and print the response
        boost::asio::streambuf response;
        boost::system::error_code error;
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
            std::cout << &response;
            response.consume(response.size());
        }
        std::cout << std::endl;

        if (error != boost::asio::error::eof) {
            std::cerr << "Error reading response: " << error.message() << std::endl;
        }

        // Close the socket
        boost::system::error_code ec;
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket.close();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
