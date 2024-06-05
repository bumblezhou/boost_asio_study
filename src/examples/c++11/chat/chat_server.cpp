#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <random>
#include <boost/asio.hpp>
#include "chat_message.hpp"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant {
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
  virtual std::string name() = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room {
public:
  void join(chat_participant_ptr participant) {
    std::cout << "[chat_room->join] participants_.insert(participant);" << std::endl;
    participants_.insert(participant);
    for (auto msg: recent_msgs_) {
      std::cout << "[chat_room->join] participant.deliver(msg:" << msg.data() << ");" << std::endl;
      participant->deliver(msg);
    }
  }

  void leave(chat_participant_ptr participant) {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg) {
    // std::cout << "[chat_room->deliver] msg:" << msg.data() << std::endl;
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs) {
      recent_msgs_.pop_front();
    }

    for (auto participant: participants_) {
      std::cout << "[chat_room->deliver] participant->deliver(msg:" << msg.data() << ");" << std::endl;
      participant->deliver(msg);
    }
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------
static const std::vector<std::string> names = { "John", "Alice", "Michael", "Emily", "David", "Olivia", "Jack", "Jason", "Anna", "Sophia", "Lima", "Neal" };

class chat_session : public chat_participant, public std::enable_shared_from_this<chat_session> {
public:
  chat_session(tcp::socket socket, chat_room& room) : socket_(std::move(socket)), room_(room) {
    std::cout << "[chat_session->chat_session] constructor" << std::endl;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, names.size() - 1);
    name_ = names[distribution(generator)];
  }

  void start() {
    std::cout << "[chat_session->start] room_.join(shared_from_this());" << std::endl;
    room_.join(shared_from_this());
    do_read_header();
  }

  void deliver(const chat_message& msg) {
    bool write_in_progress = !write_msgs_.empty();
    std::cout << "[chat_session->deliver] write_in_progress:" << (write_in_progress?"true":"false") << std::endl;
    write_msgs_.push_back(msg);
    if (!write_in_progress) {
      std::cout << "[chat_session->deliver] do_write();" << std::endl;
      do_write();
    }
  }

  std::string name() override {
    return name_;
  }

private:
  void do_read_header() {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
      boost::asio::buffer(read_msg_.data(), chat_message::header_length),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec && read_msg_.decode_header()) {
          do_read_body();
        }
        else {
          room_.leave(shared_from_this());
        }
      });
  }

  void do_read_body() {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
      boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          std::cout << "[chat_session->do_read_body] room_.deliver(msg:" << read_msg_.data() << ");" << std::endl;
          room_.deliver(read_msg_);
          do_read_header();
        }
        else {
          room_.leave(shared_from_this());
        }
      });
  }

  void do_write() {
    auto self(shared_from_this());
    std::cout << "[chat_session->do_write] async_write(socket_, ...);" << std::endl;
    boost::asio::async_write(socket_,
      boost::asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          write_msgs_.pop_front();
          if (!write_msgs_.empty()) {
            do_write();
          }
        }
        else {
          room_.leave(shared_from_this());
        }
      });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  std::string name_;
};

//----------------------------------------------------------------------

class chat_server {
public:
  chat_server(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
    std::cout << "[chat_server->chat_server] do_accept();" << std::endl;
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(
      [this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
          std::cout << "[chat_server->do_accept] acceptor_.async_accept() callback => std::make_shared<chat_session>(std::move(socket), room_)->start();" << std::endl;
          std::make_shared<chat_session>(std::move(socket), room_)->start();
        }

        do_accept();
      });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    boost::asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i) {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      std::cout << "[chat_server->main] servers.emplace_back(io_context, endpoint);" << std::endl;
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}