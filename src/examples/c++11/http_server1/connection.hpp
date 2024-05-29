#ifndef HTTP_CONNECTION_HPP
#define HTTP_CONNECTION_HPP

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include "reply.hpp"
#include "request.hpp"
#include "request_handler.hpp"
#include "request_parser.hpp"

namespace http{
namespace server{

class connection_manager;

/// @brief  Represents a single connection from a client.
class connection : public std::enable_shared_from_this<connection> {
public:
    connection(const connection&) = delete;
    connection& operator=(const connection&) = delete;

    /// @brief  Construct a connection with the given socket.
    explicit connection(boost::asio::ip::tcp::socket, connection_manager& manager, request_handler& handler);

    /// @brief  Start the first asynchronous operation for the connection.
    void start();

    /// @brief  Stop all asynchronous operations associated with the connection.
    void stop();

private:
    /// @brief  Perform an asynchronous read operation.
    void do_read();

    /// @brief  Perform an asynchronous write operation.
    void do_write();

    /// @brief  Socket for the connection.
    boost::asio::ip::tcp::socket socket_;

    /// @brief  The manager for this connection.
    connection_manager& connection_manager_;

    /// @brief  The handler used to process the incoming request.
    request_handler& request_handler_;

    /// @brief  Buffer for incoming data.
    std::array<char, 8192> buffer_;

    /// @brief  The incoming request.
    request request_;

    /// @brief  The parser the the incoming request.
    request_parser request_parser_;

    /// @brief  The reply to be sent back to the client.
    reply reply_;
};

typedef std::shared_ptr<connection> connection_ptr;

} // server
} // http

#endif // HTTP_CONNECTION_HPP