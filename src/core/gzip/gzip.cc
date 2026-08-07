#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <cstring> // std::memset
#include <memory>  // std::unique_ptr
#include <sstream> // std::ostringstream

namespace sourcemeta::core {

constexpr auto ZLIB_BUFFER_SIZE{4096};

auto gzip(std::string_view input) -> std::optional<std::string> {
  z_stream stream;
  std::memset(&stream, 0, sizeof(stream));
  auto code = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                           16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
  if (code != Z_OK) {
    return std::nullopt;
  }

  stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(input.data()));
  stream.avail_in = static_cast<uInt>(input.size());

  std::array<char, ZLIB_BUFFER_SIZE> buffer;
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
  z_stream zstream{};
  if (inflateInit2(&zstream, 16 + MAX_WBITS) != Z_OK) {
    return std::nullopt;
  }

  const auto zstream_cleanup = [](z_stream *owned) { inflateEnd(owned); };
  const std::unique_ptr<z_stream, decltype(zstream_cleanup)> zstream_guard{
      &zstream, zstream_cleanup};

  std::array<char, ZLIB_BUFFER_SIZE> buffer_input;
  std::array<char, ZLIB_BUFFER_SIZE> buffer_output;
  std::ostringstream decompressed;

  auto code{Z_OK};
  while (code != Z_STREAM_END) {
    if (zstream.avail_in == 0 && stream) {
      stream.read(buffer_input.data(), buffer_input.size());
      const auto count{stream.gcount()};
      if (count > 0) {
        zstream.next_in = reinterpret_cast<Bytef *>(buffer_input.data());
        zstream.avail_in = static_cast<uInt>(count);
      } else {
        break;
      }
    }

    zstream.next_out = reinterpret_cast<Bytef *>(buffer_output.data());
    zstream.avail_out = static_cast<uInt>(buffer_output.size());

    code = inflate(&zstream, Z_NO_FLUSH);
    if (code != Z_OK && code != Z_STREAM_END) {
      return std::nullopt;
    } else {
      decompressed.write(buffer_output.data(),
                         static_cast<long>(buffer_output.size()) -
                             zstream.avail_out);
    }
  }

  if (code != Z_STREAM_END) {
    return std::nullopt;
  }

  return decompressed.str();
}

} // namespace sourcemeta::core
