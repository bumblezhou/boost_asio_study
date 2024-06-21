#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void handle_client(tcp::socket client_socket) {
  try {
    // Perform SOCKS5 handshake with the client

    // Read and verify the SOCKS5 version and authentication methods
    std::array<char, 3> handshake;
    boost::asio::read(client_socket, boost::asio::buffer(handshake));
    if (handshake[0] != 0x05) {
        throw std::runtime_error("Invalid SOCKS5 version");
    }

    std::cout << "[socks5_server] receive SOCKS5 handshake request.\n";
    std::cout << "[socks5_server] receive handshake[0]:" << std::string("") + handshake[0] << std::endl;
    std::cout << "[socks5_server] receive handshake[1]:" << std::string("") + handshake[1] << std::endl;
    std::cout << "[socks5_server] receive handshake[2]:" << std::string("") + handshake[2] << std::endl;

    // Respond with the supported version and chosen authentication method
    std::array<char, 2> handshake_response = {0x05, 0x00};
    boost::asio::write(client_socket, boost::asio::buffer(handshake_response));
    std::cout << "[socks5_server] SOCKS5 handshake successfull.\n";

    // Read the client's request for target server
    std::array<char, 4> request_header;
    std::cout << "[socks5_server] receive SOCKS5 http request.\n";
    boost::asio::read(client_socket, boost::asio::buffer(request_header));

    if (request_header[0] != 0x05 || request_header[1] != 0x01) {
        throw std::runtime_error("Invalid SOCKS5 request");
    }

    std::cout << "[socks5_server] receive http request_header[3]: " << std::string("") + request_header[3] << std::endl;

    std::string target_address;
    switch (request_header[3]) {
      case 0x01: // IPv4 address
      {
          std::array<char, 4> ipv4_address;
          boost::asio::read(client_socket, boost::asio::buffer(ipv4_address));
          
          target_address = boost::asio::ip::address_v4(*reinterpret_cast<boost::asio::ip::address_v4::bytes_type*>(ipv4_address.data())).to_string();
          std::cout << "[socks5_server] 0x01 target_address:" << target_address << std::endl;
          break;
      }
      case 0x03: // Domain name
      {
          std::array<char, 1> domain_length;
          boost::asio::read(client_socket, boost::asio::buffer(domain_length));
          
          std::vector<char> domain_name(domain_length[0]);
          std::cout << "[socks5_server] 0x03 domain_length[0]:" << std::string("") + domain_length[0] << std::endl;
          boost::asio::read(client_socket, boost::asio::buffer(domain_name));
          target_address = std::string(domain_name.begin(), domain_name.end());
          std::cout << "[socks5_server] 0x03 target_address:" << target_address << std::endl;
          break;
      }
      case 0x04: // IPv6 address
      {
          std::array<char, 16> ipv6_address;
          boost::asio::read(client_socket, boost::asio::buffer(ipv6_address));
          target_address = boost::asio::ip::address_v6(*reinterpret_cast<boost::asio::ip::address_v6::bytes_type*>(ipv6_address.data())).to_string();
          std::cout << "[socks5_server] 0x04 target_address:" << target_address << std::endl;
          break;
      }
      default: {
        throw std::runtime_error("Invalid address type");
      }
    }

    std::array<char, 2> port_data;
    boost::asio::read(client_socket, boost::asio::buffer(port_data));
    printf("[socks5_server] port_data[0]:0x%x, port_data[1]:0x%x\n", port_data[0], port_data[1]);
    unsigned short target_port;
    target_port = port_data[0];
    target_port = (target_port << 8) & 0xFF00;
    target_port = target_port | port_data[1];
    std::cout << "[socks5_server] target_port:" << target_port << std::endl;

    // std::array<char, 3> user_data;
    // boost::asio::read(client_socket, boost::asio::buffer(user_data));
    // std::string user_data_str{user_data.data()};
    // std::cout << "[socks5_server] user_data:" << user_data_str << std::endl;

    // Connect to the target server
    tcp::socket server_socket(client_socket.get_executor());
    std::cout << "[socks5_server] create server socket." << std::endl;

    boost::asio::ip::tcp::resolver resolver(client_socket.get_executor());
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(target_address, std::to_string(target_port).c_str());
    boost::asio::connect(server_socket, endpoints);
    std::cout << "[socks5_server] connect server socket." << std::endl;

    // // Send a successful response to the client indicating connection establishment
    // std::array<char, 10> response = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // boost::asio::write(client_socket, boost::asio::buffer(response));
    // std::cout << "[socks5_server] connection establishment!" << std::endl;

    std::string request = "GET / HTTP/1.1\r\n"
                        "Host: " + target_address + "\r\n"
                        "Connection: close\r\n\r\n";
    boost::asio::write(server_socket, boost::asio::buffer(request));
    std::cout << "[socks5_server] send HTTP request ." << std::endl;

    // Read and print the response
    boost::asio::streambuf response;
    boost::system::error_code error;

    while (boost::asio::read(server_socket, response, boost::asio::transfer_at_least(1), error)) {
      boost::asio::write(client_socket, boost::asio::buffer(response.data()));
      std::cout << "[socks5_server] response to socks client." << std::endl;
      response.consume(response.size());
    }

    if (error != boost::asio::error::eof) {
        std::cerr << "Error reading response: " << error.message() << std::endl;
    }

    // Close the server_socket
    boost::system::error_code ec;
    server_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    server_socket.close();
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }
}

int main() {
  try {
      boost::asio::io_context io_context;
      tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1080));

      while (true) {
          tcp::socket client_socket = acceptor.accept();
          std::thread(handle_client, std::move(client_socket)).detach();
      }
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
