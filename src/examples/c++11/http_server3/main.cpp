#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"

int main(int argc, char* argv[]) {
  try {
    // Check command line arguments.
    if (argc != 5) {
      std::cerr << "Usage: http_server3 <address> <port> <threads> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 1 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 1 .\n";
      return 1;
    }

    // Initialise the server.
    std::size_t num_threads = std::stoi(argv[3]);
    http::server3::server s(argv[1], argv[2], argv[4], num_threads);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
  }

  return 0;
}