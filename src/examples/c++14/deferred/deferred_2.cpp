#include <boost/asio.hpp>
#include <iostream>

using boost::asio::deferred;

int main() {
  boost::asio::io_context ctx;

  boost::asio::steady_timer timer(ctx);
  timer.expires_after(std::chrono::milliseconds(100));

  auto deferred_op = timer.async_wait(
    deferred([&](boost::system::error_code ec) {
      std::cout << "first timer wait finished: " << ec.message() << "\n";
      timer.expires_after(std::chrono::milliseconds(100));
      return timer.async_wait(deferred);
    })
  );

  std::move(deferred_op)([](boost::system::error_code ec) {
    std::cout << "second timer wait finished: " << ec.message() << "\n";
  });

  ctx.run();

  return 0;
}