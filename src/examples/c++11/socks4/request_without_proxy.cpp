#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: request_without_proxy target_ip target_port relative_url\n";
    std::cout << "Examples:\n";
    std::cout << "  request_without_proxy localhost 80 /\n";
    std::cout << "  request_without_proxy 127.0.0.1 http /\n";
    std::cout << "  request_without_proxy www.boost.org https /doc/libs/1_85_0/doc/html/boost_asio/example/cpp11/socks4/sync_client.cpp\n";
    return 1;
  }
  boost::asio::io_context io_context;

  // Resolve the URL
  boost::asio::ip::tcp::resolver resolver(io_context);
  // boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("www.boost.org", "https");
  boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], argv[2]);

  // Establish a connection
  boost::asio::ip::tcp::socket socket(io_context);
  boost::asio::connect(socket, endpoints);

  // Send HTTP request
  // std::string request = "GET /doc/libs/1_85_0/doc/html/boost_asio/example/cpp11/socks4/sync_client.cpp HTTP/1.1\r\n"
  //                       "Host: www.boost.org\r\n"
  //                       "Connection: close\r\n\r\n";
  std::string request = "GET " + std::string(argv[3]) + " HTTP/1.1\r\n"
                        "Host: " + std::string(argv[1]) + "\r\n"
                        "Connection: close\r\n\r\n";
  boost::asio::write(socket, boost::asio::buffer(request));

  // Read and print the response
  boost::asio::streambuf response;
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