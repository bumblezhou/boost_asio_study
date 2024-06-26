#include "mime_types.hpp"

namespace http {
namespace server4 {
namespace mime_types {

struct mapping {
  const char* extension;
  const char* mime_type;
} mappings[] = {
  { "gif", "image/gif" },
  { "htm", "text/html" },
  { "html", "text/html" },
  { "jpg", "image/jpeg" },
  { "png", "image/png" },
  { "ttf", "font/ttf" },
  { "woff", "font/woff" },
  { "woff2", "font/woff2" },
  { 0, 0 } // Marks end of list.
};

std::string extension_to_type(const std::string& extension) {
  for (mapping* m = mappings; m->extension; ++m) {
    if (m->extension == extension) {
      return m->mime_type;
    }
  }

  return "text/plain";
}

} // namespace mime_types
} // namespace server4
} // namespace http