#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <cstring> // std::memset
#include <sstream> // std::ostringstream, std::istringstream

namespace sourcemeta::core {

auto gzip(std::string_view input) -> std::optional<std::string> {
  z_stream stream;
  std::memset(&stream, 0, sizeof(stream));
  int code = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                          16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
  if (code != Z_OK) {
    return std::nullopt;
  }

  stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(input.data()));
  stream.avail_in = static_cast<uInt>(input.size());

  std::array<char, 4096> buffer;
  std::ostringstream compressed;

  do {
    stream.next_out = reinterpret_cast<Bytef *>(buffer.data());
    stream.avail_out = sizeof(buffer);
    code = deflate(&stream, Z_FINISH);
    compressed.write(buffer.data(),
                     static_cast<long>(sizeof(buffer)) - stream.avail_out);
  } while (code == Z_OK);

  if (code != Z_STREAM_END) {
    return std::nullopt;
  }

  code = deflateEnd(&stream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return compressed.str();
}

auto gunzip(std::istream &stream) -> std::optional<std::string> {
  const std::string input{std::istreambuf_iterator<char>(stream),
                          std::istreambuf_iterator<char>()};

  z_stream zstream;
  std::memset(&zstream, 0, sizeof(zstream));
  int code = inflateInit2(&zstream, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  zstream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(input.data()));
  zstream.avail_in = static_cast<uInt>(input.size());

  std::array<char, 4096> buffer;
  std::ostringstream decompressed;

  do {
    zstream.next_out = reinterpret_cast<Bytef *>(buffer.data());
    zstream.avail_out = sizeof(buffer);
    code = inflate(&zstream, Z_NO_FLUSH);
    if (code != Z_OK && code != Z_STREAM_END) {
      inflateEnd(&zstream);
      return std::nullopt;
    }

    decompressed.write(buffer.data(),
                       static_cast<long>(sizeof(buffer)) - zstream.avail_out);
  } while (code != Z_STREAM_END);

  code = inflateEnd(&zstream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return decompressed.str();
}

} // namespace sourcemeta::core
