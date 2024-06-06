#include <boost/asio.hpp>
#include <iostream>

using boost::asio::deferred;

template <typename CompletionToken>
auto async_wait_twice(boost::asio::steady_timer& timer, CompletionToken&& token) {
  return timer.async_wait(
    deferred([&](boost::system::error_code ec) {
      std::cout << "first timer wait finished: " << ec.message() << "\n";
      timer.expires_after(std::chrono::milliseconds(100));
      return deferred.when(!ec)
        .then(timer.async_wait(deferred))
        .otherwise(deferred.values(ec));
    })
  )(
    deferred([&](boost::system::error_code ec) {
      std::cout << "second timer wait finished: " << ec.message() << "\n";
      return deferred.when(!ec)
        .then(deferred.values(42))
        .otherwise(deferred.values(0));
    })
  )(
    std::forward<CompletionToken>(token)
  );
}

int main() {
  boost::asio::io_context ctx;

  boost::asio::steady_timer timer(ctx);
  timer.expires_after(std::chrono::milliseconds(100));

  std::cout << "Execute 1st:\n";
  async_wait_twice(timer, [](int result) {
    std::cout << "result is: " << result << "\n";
  });

  // Uncomment the following line to trigger an error in async_wait_twice.
  // timer.cancel();

  ctx.run();

  return 0;
}