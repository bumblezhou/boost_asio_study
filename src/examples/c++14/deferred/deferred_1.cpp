#include <boost/asio.hpp>
#include <iostream>

using boost::asio::deferred;
using namespace std;

int main() {
  boost::asio::io_context io;
  boost::asio::steady_timer timer(io);
  timer.expires_after(std::chrono::milliseconds(100));

  auto deferred_op = timer.async_wait(deferred);

  std::move(deferred_op) ([] (boost::system::error_code ec) {
    cout << "timer wait finished: " << ec.message() << endl;
  });

  io.run();
  
  return 0;
}