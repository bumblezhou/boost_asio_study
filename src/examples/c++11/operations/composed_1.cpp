//
// composed_1.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/deferred.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <boost/array.hpp>
#include <thread>

using boost::asio::ip::tcp;

//------------------------------------------------------------------------------

// This is the simplest example of a composed asynchronous operation, where we
// simply repackage an existing operation. The asynchronous operation
// requirements are met by delegating responsibility to the underlying
// operation.

template <typename CompletionToken>
auto async_write_message(tcp::socket& socket, const char* message, CompletionToken&& token)
  // The return type of the initiating function is deduced from the combination
  // of:
  //
  // - the CompletionToken type,
  // - the completion handler signature, and
  // - the asynchronous operation's initiation function object.
  //
  // When the completion token is a simple callback, the return type is void.
  // However, when the completion token is boost::asio::yield_context (used for
  // stackful coroutines) the return type would be std::size_t, and when the
  // completion token is boost::asio::use_future it would be std::future<std::size_t>.
  // When the completion token is boost::asio::deferred, the return type differs for
  // each asynchronous operation.
  //
  // In this example we are trivially delegating to an underlying asynchronous
  // operation, so we can deduce the return type from that.
  -> decltype(
      boost::asio::async_write(socket,
        boost::asio::buffer(message, std::strlen(message)),
        std::forward<CompletionToken>(token))) {
  // When delegating to the underlying operation we must take care to perfectly
  // forward the completion token. This ensures that our operation works
  // correctly with move-only function objects as callbacks, as well as other
  // completion token types.
  return boost::asio::async_write(socket,
    boost::asio::buffer(message, std::strlen(message)),
    std::forward<CompletionToken>(token));
}

//------------------------------------------------------------------------------

void test_callback() {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, {tcp::v4(), 55555});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using a lambda as a callback.
  async_write_message(socket, "Testing callback\r\n",
    [](const boost::system::error_code& error, std::size_t n) {
      if (!error) {
        std::cout << n << " bytes transferred\n";
      }
      else {
        std::cout << "Error: " << error.message() << "\n";
      }
    });

  io_context.run();
}

//------------------------------------------------------------------------------

void test_deferred() {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, {tcp::v4(), 55556});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using the deferred completion token. This
  // token causes the operation's initiating function to package up the
  // operation and its arguments to return a function object, which may then be
  // used to launch the asynchronous operation.
  auto op = async_write_message(socket, "Testing deferred\r\n", boost::asio::deferred);

  // Launch the operation using a lambda as a callback.
  std::move(op)(
    [](const boost::system::error_code& error, std::size_t n) {
      if (!error) {
        std::cout << n << " bytes transferred\n";
      }
      else {
        std::cout << "Error: " << error.message() << "\n";
      }
    });

  io_context.run();
}

//------------------------------------------------------------------------------

void test_future() {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor(io_context, {tcp::v4(), 55557});
  tcp::socket socket = acceptor.accept();

  // Test our asynchronous operation using the use_future completion token.
  // This token causes the operation's initiating function to return a future,
  // which may be used to synchronously wait for the result of the operation.
  std::future<std::size_t> f = async_write_message(socket, "Testing future\r\n", boost::asio::use_future);

  io_context.run();

  try {
    // Get the result of the operation.
    std::size_t n = f.get();
    std::cout << n << " bytes transferred\n";
  }
  catch (const std::exception& e) {
    std::cout << "Error: " << e.what() << "\n";
  }
}

//------------------------------------------------------------------------------
using namespace std;
void connect_socket(const std::string port) {
  boost::asio::io_context io;
  tcp::resolver resolver(io);
  auto endpoints = resolver.resolve("localhost", port);
  tcp::socket socket(io);

  cout << "[connect_socket] async_connect(socket, receiver_endpoint)..." << endl;
  boost::asio::async_connect(socket, endpoints,
    [&](boost::system::error_code ec, tcp::endpoint) {
      if (!ec) {
        cout << "[connect_socket] async_connect(socket, receiver_endpoint) successfully." << endl;
      } else {
        cout << "[connect_socket] async_connect(socket, receiver_endpoint) failed." << endl;
        socket.close();
      }
    });
}

int main() {
  std::cout << "--------------------------- test_callback(); ----------------------------\n";
  thread t0(test_callback);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "--------------------------- connect_socket of test_callback ----------------------------\n";
  connect_socket("55555");

  std::cout << "--------------------------- test_deferred(); ----------------------------\n";
  thread t1(test_deferred);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "--------------------------- connect_socket of test_deferred ----------------------------\n";
  connect_socket("55556");

  std::cout << "--------------------------- test_future(); ----------------------------\n";
  thread t2(test_future);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  std::cout << "--------------------------- connect_socket of test_future ----------------------------\n";
  connect_socket("55557");

  if(t0.joinable()) {
    t0.join();
  }

  if(t1.joinable()) {
    t1.join();
  }

  if(t2.joinable()) {
    t2.join();
  }
}