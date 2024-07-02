#include <iostream>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;

std::vector<uint8_t> in_buf_;
std::vector<uint8_t> out_buf_;

void do_write(int direction, std::size_t Length, tcp::socket& in_socket_, tcp::socket& out_socket_);

void do_read(int direction, tcp::socket& in_socket_, tcp::socket& out_socket_) {
  // We must divide reads by direction to not permit second read call on the same socket.
  if (direction & 0x1) {
    std::cout << "[do_read]: 0x1 .." << std::endl;
    // in_socket_.async_receive(boost::asio::buffer(in_buf_), [&](boost::system::error_code ec, std::size_t length) {
    //   if (!ec) {
    //     std::ostringstream what; what << "--> " << std::to_string(length) << " bytes";
    //     std::cout << "[do_read]:" << what.str() << std::endl;
    //     do_write(1, length, in_socket_, out_socket_);
    //   }
    //   else {
    //     // Most probably client closed socket. Let's close both sockets and exit session.
    //     in_socket_.close(); out_socket_.close();
    //   }
    // });
    std::vector<std::int8_t> buffer;
    boost::system::error_code error;
    size_t size = in_socket_.receive(boost::asio::buffer(buffer));
    if (size > 0) {
      boost::asio::write(out_socket_, boost::asio::buffer(buffer, size));
      do_read(2, in_socket_, out_socket_);
    }
  }
    

  if (direction & 0x2) {
    std::cout << "[do_read]: 0x2 .." << std::endl;
    // out_socket_.async_receive(boost::asio::buffer(out_buf_), [&](boost::system::error_code ec, std::size_t length) {
    //   if (!ec) {
    //     std::ostringstream what; what << "<-- " << std::to_string(length) << " bytes";
    //     std::cout << "[do_read]:" << what.str() << std::endl;
    //     do_write(2, length, in_socket_, out_socket_);
    //   }
    //   else {
    //     // Most probably remote server closed socket. Let's close both sockets and exit session.
    //     in_socket_.close(); out_socket_.close();
    //   }
    // });
    std::vector<std::int8_t> buffer;
    boost::system::error_code error;
    size_t size = out_socket_.receive(boost::asio::buffer(buffer));
    if (size > 0) {
      boost::asio::write(in_socket_, boost::asio::buffer(buffer, size));
      do_read(2, in_socket_, out_socket_);
    }
  }
}

void do_write(int direction, std::size_t Length, tcp::socket& in_socket_, tcp::socket& out_socket_) {
  switch (direction) {
  case 1:
    std::cout << "[do_write]: 0x1" << std::endl;
    boost::asio::async_write(out_socket_, boost::asio::buffer(in_buf_, Length), [&](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        do_read(direction, in_socket_, out_socket_);
      }
      else {
        // Most probably client closed socket. Let's close both sockets and exit session.
        in_socket_.close(); out_socket_.close();
      }
    });
    break;
  case 2:
    std::cout << "[do_write]: 0x2" << std::endl;
    boost::asio::async_write(in_socket_, boost::asio::buffer(out_buf_, Length), [&](boost::system::error_code ec, std::size_t length) {
      if (!ec) {
        do_read(direction, in_socket_, out_socket_);
      }
      else {
        // Most probably remote server closed socket. Let's close both sockets and exit session.
        in_socket_.close(); out_socket_.close();
      }
    });
    break;
  }
}

void handle_client(tcp::socket client_socket) {
  try {
    // Perform SOCKS5 handshake with the client

    // Read and verify the SOCKS5 version and authentication methods
    std::array<std::uint8_t, 3> handshake;
    boost::asio::read(client_socket, boost::asio::buffer(handshake));
    if (handshake[0] != 0x05) {
        throw std::runtime_error("[socks5_server] Invalid SOCKS5 version");
    }
    std::cout << "[socks5_server] receive SOCKS5 handshake request.\n";

    // Respond with the supported version and chosen authentication method
    std::array<char, 2> handshake_response = {0x05, 0x00};
    boost::asio::write(client_socket, boost::asio::buffer(handshake_response));
    std::cout << "[socks5_server] SOCKS5 handshake successfull.\n";

    // Read the client's request for target server
    std::array<std::uint8_t, 4> request_header;
    boost::asio::read(client_socket, boost::asio::buffer(request_header));

    if (request_header[0] != 0x05 || request_header[1] != 0x01) {
      printf("request_header[1]:0x%x\n", request_header[1]);
      throw std::runtime_error("[socks5_server] Invalid SOCKS5 request");
    }

    std::string target_address;
    std::string domain_name;
    std::string relative_url;
    switch (request_header[3]) {
      case 0x01: // IPv4 address
      {
          std::array<std::uint8_t, 4> ipv4_address;
          boost::asio::read(client_socket, boost::asio::buffer(ipv4_address));
          
          target_address = boost::asio::ip::address_v4(*reinterpret_cast<boost::asio::ip::address_v4::bytes_type*>(ipv4_address.data())).to_string();
          std::cout << "[socks5_server] 0x01 target_address:" << target_address << std::endl;
          break;
      }
      case 0x03: // Domain name
      {
          std::array<std::uint8_t, 1> url_length;
          boost::asio::read(client_socket, boost::asio::buffer(url_length));
          
          std::vector<std::uint8_t> url(url_length[0]);
          boost::asio::read(client_socket, boost::asio::buffer(url));
          target_address = std::string(url.begin(), url.end());

          std::size_t pos = target_address.find('/');
          if (pos != std::string::npos) {
            domain_name = target_address.substr(0, pos);
            relative_url = target_address.substr(pos);
          } else {
            domain_name = target_address;
            relative_url = "/";
          }
          std::cout << "[socks5_server] 0x03 target_address:" << target_address << std::endl;
          break;
      }
      case 0x04: // IPv6 address
      {
          std::array<std::uint8_t, 16> ipv6_address;
          boost::asio::read(client_socket, boost::asio::buffer(ipv6_address));
          target_address = boost::asio::ip::address_v6(*reinterpret_cast<boost::asio::ip::address_v6::bytes_type*>(ipv6_address.data())).to_string();
          std::cout << "[socks5_server] 0x04 target_address:" << target_address << std::endl;
          break;
      }
      default: {
        throw std::runtime_error("Invalid address type");
      }
    }

    std::array<std::uint8_t, 2> port_data;
    boost::asio::read(client_socket, boost::asio::buffer(port_data));
    unsigned short target_port;
    target_port = port_data[0];
    target_port = (target_port << 8) & 0xFF00;
    target_port = target_port | port_data[1];
    std::cout << "[socks5_server] target_port:" << target_port << std::endl;

    // // Create a reply indicating success
    // std::vector<uint8_t> reply = {0x05, 0x00, 0x00, 0x01, 0, 0, 0, 0, 0, 0};
    // boost::asio::write(client_socket, boost::asio::buffer(reply));
    // std::cout << "[socks5_server] reply client, ready to request target server.." << std::endl;

    // // Connect to the target server
    // // tcp::socket proxy_socket(client_socket.get_executor());
    // ssl::context ssl_context(ssl::context::tlsv12_client);
    // ssl_context.set_default_verify_paths();
    // ssl::stream<tcp::socket> proxy_socket(client_socket.get_executor(), ssl_context);
    // std::cout << "[socks5_server] create a proxy socket." << std::endl;

    // if(domain_name == "") {
    //   boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(target_address), target_port);
    //   // boost::asio::connect(proxy_socket.lowest_layer(), endpoint);
    //   proxy_socket.lowest_layer().connect(endpoint);
    // } else {
    //   boost::asio::ip::tcp::resolver resolver(client_socket.get_executor());
    //   boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(domain_name, std::to_string(target_port).c_str());
    //   boost::asio::connect(proxy_socket.lowest_layer(), endpoints);
    // }
    // std::cout << "[socks5_server] connect to target server." << std::endl;

    // proxy_socket.handshake(ssl::stream_base::client);
    // std::cout << "[socks5_server] handshake to target server." << std::endl;

    // std::string request = "GET " + relative_url + " HTTP/1.1\r\n"
    //                     "Host: " + domain_name + "\r\n"
    //                     "Connection: close\r\n\r\n";
    // boost::asio::write(proxy_socket, boost::asio::buffer(request));
    // std::cout << "[socks5_server] send HTTP request:" << std::endl;
    // std::cout << request;

    // Read and print the response
    // boost::asio::streambuf response;
    // boost::system::error_code error;

    // while (boost::asio::read(proxy_socket, response, boost::asio::transfer_at_least(1), error)) {
    //   if (response.size() == 5) {
    //     continue;
    //   }
    //   boost::asio::write(client_socket, boost::asio::buffer(response.data()));
    //   std::cout << "[socks5_server] response to socks client with size:" << response.size() << std::endl;
    //   response.consume(response.size());
    // }

    // if (error != boost::asio::error::eof) {
    //     std::cerr << "Error reading response: " << error.message() << std::endl;
    // }

    boost::system::error_code ec;
    tcp::socket proxy_socket(client_socket.get_executor());
    if(domain_name == "") {
      boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(target_address), target_port);
      proxy_socket.lowest_layer().connect(endpoint);
    } else {
      boost::asio::ip::tcp::resolver resolver(client_socket.get_executor());
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(domain_name, std::to_string(target_port).c_str()).begin();
      proxy_socket.connect(endpoint);
    }
    std::cout << "[socks5_server] connect to target server." << std::endl;

    // The SOCKS request information
    std::vector<std::uint8_t> reply_buf_{0,0,0,0,0,0,0,0,0,0};
    reply_buf_[0] = 0x05; reply_buf_[1] = 0x00; reply_buf_[2] = 0x00; reply_buf_[3] = 0x01;
    uint32_t realRemoteIP = proxy_socket.remote_endpoint().address().to_v4().to_ulong();
		uint16_t realRemoteport = htons(proxy_socket.remote_endpoint().port());
    printf("[socks5_server] realRemoteIP: 0x%x, realRemoteport: 0x%x\n", realRemoteIP, realRemoteport);
		std::memcpy(&reply_buf_[4], &realRemoteIP, 4);
		std::memcpy(&reply_buf_[8], &realRemoteport, 2);
    printf("[socks5_server] reply_buf_:");
    for(auto& c : reply_buf_) {
      printf("0x%x ", c);
    }
    printf("\n");
    boost::asio::write(client_socket, boost::asio::buffer(reply_buf_, 10));
    std::cout << "[socks5_server] reply client, ready to request target server..." << std::endl;

    do_read(3, client_socket, proxy_socket); // Read both sockets
    
    proxy_socket.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
    proxy_socket.close();
  }
  catch (std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
  }
}

int main() {
  try {
      boost::asio::io_context io_context;
      tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 2080));

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
