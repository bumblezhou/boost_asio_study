#ifndef HTTP_SERVER2_MIME_TYPES_HPP
#define HTTP_SERVER2_MIME_TYPES_HPP

#include <string>

namespace http {
namespace server2 {
namespace mime_types {

/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string& extension);

} // namespace mime_types
} // namespace server2
} // namespace http

#endif // HTTP_SERVER2_MIME_TYPES_HPP