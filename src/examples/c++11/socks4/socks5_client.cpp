#include <iostream>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    // Connect to the SOCKS5 proxy server
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("0.0.0.0"), 8080));

    // Perform SOCKS5 handshake with the proxy server
    std::array<char, 3> handshake = {0x05, 0x01, 0x00};
    boost::asio::write(socket, boost::asio::buffer(handshake));
    std::cout << "[socks5_client] send SOCKS5 handshake request.\n";

    std::array<char, 2> handshake_response;
    boost::asio::read(socket, boost::asio::buffer(handshake_response));

    if (handshake_response[0] != 0x05 || handshake_response[1] != 0x00) {
        throw std::runtime_error("SOCKS5 handshake failed");
        std::cout << "[socks5_client] SOCKS5 handshake failed.\n";
    }
    std::cout << "[socks5_client] SOCKS5 handshake successfull.\n";

    // Send the request for the target server
    std::string target_address = "www.google.com";
    unsigned short target_port = 80;

    constexpr size_t header_length = 7 + 14;

    std::array<char, 21> request_header = {
        0x05, 0x01, 0x00, 0x03, static_cast<char>(target_address.length())
    };

    std::copy(target_address.begin(), target_address.end(), request_header.begin() + 5);
    request_header[5 + target_address.length()] = (target_port >> 8) & 0xFF;
    request_header[6 + target_address.length()] = target_port & 0xFF;

    std::cout << "[socks5_client] send SOCKS5 http request.\n";
    boost::asio::write(socket, boost::asio::buffer(request_header));

    // Receive the response from the proxy server
    std::array<char, 10> response;
    boost::asio::read(socket, boost::asio::buffer(response));
    if (response[1] != 0x00) {
        throw std::runtime_error("Connection to target server failed");
    }
    std::cout << "[socks5_client] receive SOCKS5 http response.\n";

    // Start relaying data between the client and the target server
    std::array<char, 8192> buffer;
    std::size_t bytes_transferred;

    while ((bytes_transferred = socket.read_some(boost::asio::buffer(buffer))) > 0) {
        // Process the received data from the target server

        // Optionally, perform any additional processing or modifications

        // Send the processed data back to the target server
        boost::asio::write(socket, boost::asio::buffer(buffer, bytes_transferred));
    }
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}