#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <cstring> // std::memset
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
  code = Z_OK;

  while (code != Z_STREAM_END) {
    if (stream.avail_in == 0U) {
      input.read(input_buffer.data(),
                 static_cast<std::streamsize>(input_buffer.size()));
      const auto read_count = input.gcount();
      if (read_count <= 0) {
        break;
      }

      stream.next_in = reinterpret_cast<Bytef *>(input_buffer.data());
      stream.avail_in = static_cast<uInt>(read_count);
    }

    do {
      stream.next_out = reinterpret_cast<Bytef *>(output_buffer.data());
      stream.avail_out = static_cast<uInt>(output_buffer.size());
      code = inflate(&stream, Z_NO_FLUSH);
      if (code != Z_OK && code != Z_STREAM_END) {
        inflateEnd(&stream);
        return std::nullopt;
      }

      decompressed.write(output_buffer.data(),
                         static_cast<long>(output_buffer.size()) -
                             stream.avail_out);
    } while (stream.avail_out == 0U);
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
