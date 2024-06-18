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

    std::size_t num_auth_methods = handshake[1];
    std::vector<char> auth_methods(num_auth_methods);
    std::cout << "[socks5_server] receive handshake[1]: 0x" << std::hex << handshake[1] << std::endl;
    std::cout << "[socks5_server] receive SOCKS5 handshake request.\n";
    boost::asio::read(client_socket, boost::asio::buffer(auth_methods));

    std::cout << "[socks5_server] receive handshake[0]: 0x" << std::hex << handshake[0] << std::endl;
    std::cout << "[socks5_server] receive auth_methods[0]: 0x" << std::hex << auth_methods[0] << std::endl;

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

    std::cout << "[socks5_server] receive http request header[3]: 0x" << std::hex << request_header[5] << std::endl;

    std::string target_address;
    unsigned short target_port;

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
    target_port = (port_data[0] << 8) | port_data[1];

    std::cout << "[socks5_server] target_port:" << target_port << std::endl;

    // Connect to the target server
    tcp::socket server_socket(client_socket.get_executor());
    server_socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(target_address), target_port));

    // Send a successful response to the client indicating connection establishment
    std::array<char, 10> response = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    boost::asio::write(client_socket, boost::asio::buffer(response));

    std::cout << "[socks5_server] connection establishment!" << std::endl;

    // Start relaying data between the client and server
    std::array<char, 8192> buffer;
    std::size_t bytes_transferred;

    while ((bytes_transferred = client_socket.read_some(boost::asio::buffer(buffer))) > 0) {
      boost::asio::write(server_socket, boost::asio::buffer(buffer, bytes_transferred));

      bytes_transferred = server_socket.read_some(boost::asio::buffer(buffer));
      boost::asio::write(client_socket, boost::asio::buffer(buffer, bytes_transferred));
    }
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }
}

int main() {
  try {
      boost::asio::io_context io_context;
      tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

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