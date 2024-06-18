#include <array>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include "socks4.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
  boost::asio::io_context io_context;

  // Resolve the proxy server
  boost::asio::ip::tcp::resolver resolver(io_context);
  boost::asio::ip::tcp::resolver::results_type proxy_endpoints = resolver.resolve("172.16.200.15", "80");

  // Establish a connection with the proxy server
  boost::asio::ip::tcp::socket socket(io_context);
  boost::asio::connect(socket, proxy_endpoints);

  // Send HTTP CONNECT request to establish a tunnel
  std::string request = "CONNECT www.boost.org:443 HTTP/1.1\r\n"
                        "Host: www.boost.org\r\n"
                        "Proxy-Connection: Keep-Alive\r\n\r\n";
  boost::asio::write(socket, boost::asio::buffer(request));

  // Read and print the response from the proxy server
  boost::asio::streambuf response;
  boost::asio::read_until(socket, response, "\r\n");
  std::cout << &response;

  // Read and discard the remaining response headers
  boost::asio::read_until(socket, response, "");

  // Clear the response buffer for the subsequent request
  response.consume(response.size());

  // Send HTTP GET request through the proxy tunnel
  request = "GET /doc/libs/1_85_0/doc/html/boost_asio/examples/cpp11_examples.html HTTP/1.1\r\n"
            "Host: www.boost.org\r\n"
            "Connection: close\r\n\r\n";
  boost::asio::write(socket, boost::asio::buffer(request));

  // Read and print the response
  boost::system::error_code error;

  while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
      std::cout << &response;
      response.consume(response.size());
  }

  if (error != boost::asio::error::eof) {
      std::cerr << "Error reading response: " << error.message() << std::endl;
  }

  // Close the socket
  boost::system::error_code ec;
  socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  socket.close();

  return 0;
}