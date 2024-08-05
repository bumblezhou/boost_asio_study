//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
using boost::asio::ssl::stream;

class session : public std::enable_shared_from_this<session>
{
public:
  session(stream<tcp::socket> socket) : socket_(std::move(socket)) {}

  void start ()
  {
    std::cout << "session->start()\n";
    do_handshake();
  }

private:
  void do_handshake()
  {
    std::cout << "session->do_handshake()\n";
    auto self(shared_from_this());
    socket_.async_handshake(boost::asio::ssl::stream_base::server, [this, self](const boost::system::error_code& error) {
      if (!error)
      {
        do_read();
      }
    });
  }
  
  void do_read()
  {
    std::cout << "session->do_read()\n";
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_), [this, self](const boost::system::error_code& error, std::size_t length) {
      if (!error)
      {
        do_write(length);
      }
    });
  }

  void do_write(const std::size_t length)
  {
    std::cout << "session->do_write()\n";
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length), [this, self] (const boost::system::error_code& error, std::size_t /*length*/) {
      if (!error)
      {
        do_read();
      }
    });
  }
  stream<tcp::socket> socket_;
  char data_[1024];
};

class server
{
public:
  server(boost::asio::io_context& io_context, uint16_t port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), context_(boost::asio::ssl::context::sslv23)
  {
    context_.set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use
    );
    context_.set_password_callback(std::bind(&server::get_password, this));
    context_.use_certificate_chain_file("server.pem");
    context_.use_private_key_file("server.key", boost::asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh4096.pem");

    do_accept();
  }

private:
  std::string get_password() const
  {
    std::cout << "server->get_password()\n";
    return "test";
  }
  void do_accept()
  {
    std::cout << "server->do_accept()\n";
    acceptor_.async_accept([this](const boost::system::error_code& error, tcp::socket socket) {
      if (!error)
      {
        std::make_shared<session>(boost::asio::ssl::stream<tcp::socket>(std::move(socket), context_))->start();
      }

      do_accept();
    });
  }
  tcp::acceptor acceptor_;
  boost::asio::ssl::context context_;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: server <port> \n";
      return 1;
    }

    boost::asio::io_context io_context;
    server s(io_context, std::atoi(argv[1]));

    io_context.run();
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }
  
  return 0;
}