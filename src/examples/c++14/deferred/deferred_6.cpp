#include <boost/asio.hpp>
#include <iostream>

using boost::asio::append;
using boost::asio::deferred;

template <typename CompletionToken>
auto async_wait_twice(boost::asio::steady_timer& timer, CompletionToken&& token) {
  return deferred.values(&timer)(
      deferred([](boost::asio::steady_timer* timer) {
        timer->expires_after(std::chrono::milliseconds(100));
        std::cout << "append timer, waiting for 100 milisecs " << "\n";
        return timer->async_wait(append(deferred, timer));
      })
    )(
      deferred([](boost::system::error_code ec, boost::asio::steady_timer* timer) {
        std::cout << "first timer wait finished: " << ec.message() << "\n";
        timer->expires_after(std::chrono::milliseconds(100));
        return deferred.when(!ec)
          .then(timer->async_wait(deferred))
          .otherwise(deferred.values(ec));
      })
    )(
      deferred([](boost::system::error_code ec){
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

  async_wait_twice(timer, [](int result){
    std::cout << "result is " << result << "\n";
  });

  // Uncomment the following line to trigger an error in async_wait_twice.
  //timer.cancel();

  ctx.run();

  return 0;
}