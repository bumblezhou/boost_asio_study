#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include "connection.hpp"
#include "connection_manager.hpp"
#include "request_handler.hpp"

namespace http {
namespace server {

/// @brief  The top-level class of the HTTP server
class server {
public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    /// @brief  Construct the server to listen on the specified TCP address and port, and serve up files from the given directory.
    explicit server(const std::string& address, const std::string& port, const std::string& doc_root);

    /// @brief  Run the server's io_context loop.
    void run();

private:
    /// @brief  Perform an asynchronous accept operation
    void do_accept();

    /// @brief  Wait for a request to stop the server
    void do_await_stop();

    /// @brief  The io_context used to perform asynchronous operations
    boost::asio::io_context io_context_;

    /// @brief  The signal_set is used to register for process termination notifications
    boost::asio::signal_set signals_;

    /// @brief  Acceptor used to listen
    boost::asio::ip::tcp::acceptor acceptor_;

    /// @brief  The connection manager which owns all live connections.
    connection_manager connection_manager_;

    /// @brief  The handler for all incoming requests.
    request_handler request_handler_;
};

} // namespace server
} // namespace http

#endif // HTTP_SERVER_HPP