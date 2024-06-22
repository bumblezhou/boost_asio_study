#include <iostream>
#include <array>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main() {
  try {
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    // Connect to the SOCKS5 proxy server
    boost::asio::ip::tcp::resolver resolver(io_context);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("localhost", "2080");
    boost::asio::connect(socket, endpoints);
    std::cout << "[socks5_client] connect socks5 server successfully.\n";

    // Perform SOCKS5 handshake with the proxy server
    std::array<std::uint8_t, 3> handshake = {0x05, 0x01, 0x00};
    boost::asio::write(socket, boost::asio::buffer(handshake));
    std::cout << "[socks5_client] send SOCKS5 handshake request.\n";

    std::array<std::uint8_t, 2> handshake_response;
    boost::asio::read(socket, boost::asio::buffer(handshake_response));

    if (handshake_response[0] != 0x05 || handshake_response[1] != 0x00) {
        throw std::runtime_error("SOCKS5 handshake failed");
        std::cout << "[socks5_client] SOCKS5 handshake failed.\n";
    }
    std::cout << "[socks5_client] SOCKS5 handshake successfull.\n";

    // Send the request for the target server
    // std::string target_url = "localhost/index.html";
    // unsigned short target_port = 80;

    std::string target_url = "www.solidot.org/story?sid=78496";
    unsigned short target_port = 443;

    std::vector<std::uint8_t> request_header{
        0x05, 0x01, 0x00, 0x03, static_cast<std::uint8_t>(target_url.length())
    };
    request_header.insert(request_header.end(), target_url.begin(), target_url.end());
    request_header.push_back(static_cast<std::uint8_t>((target_port >> 8) & 0xFF));
    request_header.push_back(static_cast<std::uint8_t>(target_port & 0xFF));
    printf("[socks5_client] request_header[size-2]:0x%x, request_header[size-1]:0x%x\n", request_header[request_header.size()-2], request_header[request_header.size()-1]);
    std::cout << "[socks5_client] request_header size:" << request_header.size() << "\n";

    std::cout << "[socks5_client] send SOCKS5 http request.\n";
    boost::asio::write(socket, boost::asio::buffer(request_header));

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
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
