#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <cstring> // std::memset
#include <istream> // std::istream
#include <sstream> // std::ostringstream

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

auto gunzip(std::istream &input) -> std::optional<std::string> {
  z_stream stream;
  std::memset(&stream, 0, sizeof(stream));
  int code = inflateInit2(&stream, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  std::array<char, 4096> input_buffer;
  std::array<char, 4096> output_buffer;
  std::ostringstream decompressed;

  while (true) {
    input.read(input_buffer.data(),
               static_cast<std::streamsize>(input_buffer.size()));
    if (input.bad()) {
      inflateEnd(&stream);
      return std::nullopt;
    }

    const auto read{input.gcount()};
    if (read == 0) {
      break;
    }

    stream.next_in = reinterpret_cast<Bytef *>(input_buffer.data());
    stream.avail_in = static_cast<uInt>(read);

    do {
      stream.next_out = reinterpret_cast<Bytef *>(output_buffer.data());
      stream.avail_out = sizeof(output_buffer);
      code = inflate(&stream, Z_NO_FLUSH);
      if (code != Z_OK && code != Z_STREAM_END) {
        inflateEnd(&stream);
        return std::nullopt;
      }

      decompressed.write(output_buffer.data(),
                         static_cast<long>(sizeof(output_buffer)) -
                             stream.avail_out);
    } while (stream.avail_out == 0);

    if (code == Z_STREAM_END) {
      break;
    }
  }

  if (code != Z_STREAM_END) {
    inflateEnd(&stream);
    return std::nullopt;
  }

  code = inflateEnd(&stream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return decompressed.str();
}

} // namespace sourcemeta::core
