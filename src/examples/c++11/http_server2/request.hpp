#ifndef HTTP_SERVER2_REQUEST_HPP
#define HTTP_SERVER2_REQUEST_HPP

#include <string>
#include <vector>
#include "header.hpp"

namespace http {
namespace server2 {

/// A request received from a client.
struct request {
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
};

} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_REQUEST_HPP