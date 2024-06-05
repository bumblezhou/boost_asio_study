#include <boost/asio.hpp>
#include <iostream>

using boost::asio::deferred;

template <typename CompletionToken>
auto async_wait_twice(boost::asio::steady_timer& timer, CompletionToken&& token) {
  return timer.async_wait(
    deferred([&](boost::system::error_code ec) {
      std::cout << "first timer wait finished: " << ec.message() << "\n";
      timer.expires_after(std::chrono::milliseconds(100));
      return timer.async_wait(deferred);
    })
  )(
    deferred([&](boost::system::error_code ec) {
      std::cout << "second timer wait finished: " << ec.message() << "\n";
      return deferred.values(42);
    })
  )(
    std::forward<CompletionToken>(token)
  );
}

int main() {
  boost::asio::io_context ctx;

  boost::asio::steady_timer timer(ctx);
  timer.expires_after(std::chrono::milliseconds(100));

  async_wait_twice(timer, [](int result) {
    std::cout << "result is: " << result << "\n";
  });

  ctx.run();

  return 0;
}