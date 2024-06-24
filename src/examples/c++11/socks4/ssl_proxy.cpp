#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

void forwardRequest(boost::asio::ssl::stream<tcp::socket>& ssl_socket) {
    // Read the client's request
    boost::asio::streambuf request_buffer;
    boost::system::error_code error;
    size_t bytes_transferred = boost::asio::read_until(ssl_socket, request_buffer, "\r\n", error);
    if (error) {
        std::cerr << "Error reading request: " << error.message() << std::endl;
        return;
    }

    // Forward the request to the destination server
    std::string request = boost::asio::buffer_cast<const char*>(request_buffer.data());
    std::cout << "Received request: " << request << std::endl;

    // Create a connection to the destination server
    boost::asio::io_context destination_io_context;
    tcp::socket destination_socket(destination_io_context);
    tcp::resolver resolver(destination_io_context);
    tcp::resolver::results_type endpoints = resolver.resolve("destination.server.com", "80");
    boost::asio::connect(destination_socket, endpoints);

    // Send the request to the destination server
    boost::asio::write(destination_socket, boost::asio::buffer(request));

    // Forward the response from the destination server back to the client
    boost::asio::streambuf response_buffer;
    while (true) {
        size_t bytes_transferred = boost::asio::read(destination_socket, response_buffer,
                                                     boost::asio::transfer_at_least(1), error);
        if (error == boost::asio::error::eof) {
            break; // Reached the end of the response
        } else if (error) {
            std::cerr << "Error reading response: " << error.message() << std::endl;
            break;
        }

        // Send the response back to the client
        boost::asio::write(ssl_socket, response_buffer.data());
        response_buffer.consume(bytes_transferred);
    }
}

void handleClientRequest(boost::asio::ssl::context& ssl_context, tcp::socket&& socket) {
    try {
        boost::asio::ssl::stream<tcp::socket> ssl_socket(std::move(socket), ssl_context);

        // Perform SSL handshake
        ssl_socket.handshake(boost::asio::ssl::stream_base::server);

        // Forward the request to the destination server
        forwardRequest(ssl_socket);

        // Close the SSL connection
        ssl_socket.shutdown();
    } catch (const std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << std::endl;
    }
}

int main() {
    boost::asio::io_context io_context;
    boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);

    // Load SSL key and certificate files
    ssl_context.use_private_key_file("/full_path/proxy.key", boost::asio::ssl::context::pem);
    ssl_context.use_certificate_chain_file("/full_path/proxy.crt");

    // Set up the TCP server
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 443));

    const int THREAD_POOL_SIZE = 4;  // Number of threads in the pool
    boost::thread_group thread_pool;

    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        thread_pool.create_thread([&io_context]() {
            io_context.run();
        });
    }

    while (true) {
        // Accept incoming connection
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // Handle the request in a separate thread
        boost::asio::post(io_context, [&]() {
            handleClientRequest(ssl_context, std::move(socket));
        });
    }

    thread_pool.join_all();

    return 0;
}