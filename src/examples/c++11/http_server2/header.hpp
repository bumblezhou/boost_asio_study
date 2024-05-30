#ifndef HTTP_SERVER2_HEADER_HPP
#define HTTP_SERVER2_HEADER_HPP

#include <string>

namespace http {
namespace server2 {

struct header {
  std::string name;
  std::string value;
};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_HEADER_HPP