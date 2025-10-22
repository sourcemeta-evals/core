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

auto gunzip(std::istream &stream) -> std::optional<std::string> {
  z_stream zs;
  std::memset(&zs, 0, sizeof(zs));
  int code = inflateInit2(&zs, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  std::array<unsigned char, 4096> input_buffer;
  std::array<unsigned char, 4096> output_buffer;
  std::string result;
  bool stream_ended = false;

  while (stream.read(reinterpret_cast<char *>(input_buffer.data()),
                     input_buffer.size()) ||
         stream.gcount() > 0) {
    zs.next_in = input_buffer.data();
    zs.avail_in = static_cast<uInt>(stream.gcount());

    while (zs.avail_in > 0) {
      zs.next_out = output_buffer.data();
      zs.avail_out = static_cast<uInt>(output_buffer.size());

      code = inflate(&zs, Z_NO_FLUSH);

      if (code != Z_OK && code != Z_STREAM_END) {
        inflateEnd(&zs);
        return std::nullopt;
      }

      const auto produced =
          static_cast<std::size_t>(output_buffer.size() - zs.avail_out);
      result.append(reinterpret_cast<const char *>(output_buffer.data()),
                    produced);

      if (code == Z_STREAM_END) {
        stream_ended = true;
        code = inflateReset2(&zs, 16 + MAX_WBITS);
        if (code != Z_OK) {
          inflateEnd(&zs);
          return std::nullopt;
        }
      }
    }
  }

  inflateEnd(&zs);

  if (!stream_ended) {
    return std::nullopt;
  }

  return result;
}

} // namespace sourcemeta::core
