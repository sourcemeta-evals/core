#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>    // std::array
#include <cstring>  // std::memset
#include <iterator> // std::istreambuf_iterator
#include <sstream>  // std::ostringstream

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
  std::string input{std::istreambuf_iterator<char>(stream),
                    std::istreambuf_iterator<char>()};

  if (input.empty()) {
    return std::nullopt;
  }

  z_stream zstream;
  std::memset(&zstream, 0, sizeof(zstream));
  int code = inflateInit2(&zstream, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  zstream.next_in = reinterpret_cast<Bytef *>(input.data());
  zstream.avail_in = static_cast<uInt>(input.size());

  std::array<unsigned char, 32768> buffer;
  std::string decompressed;

  for (;;) {
    zstream.next_out = buffer.data();
    zstream.avail_out = static_cast<uInt>(buffer.size());
    code = inflate(&zstream, Z_NO_FLUSH);

    const auto produced = buffer.size() - zstream.avail_out;
    if (produced > 0) {
      decompressed.append(reinterpret_cast<char *>(buffer.data()), produced);
    }

    if (code == Z_STREAM_END) {
      if (zstream.avail_in > 0) {
        if (inflateReset(&zstream) != Z_OK) {
          inflateEnd(&zstream);
          return std::nullopt;
        }
        continue;
      }
      break;
    }

    if (code == Z_OK) {
      continue;
    }

    inflateEnd(&zstream);
    return std::nullopt;
  }

  code = inflateEnd(&zstream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return decompressed;
}

} // namespace sourcemeta::core
