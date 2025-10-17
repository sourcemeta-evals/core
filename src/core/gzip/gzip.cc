#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <cstring> // std::memset
#include <sstream> // std::ostringstream, std::istringstream
#include <vector>  // std::vector

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
  z_stream zstream;
  std::memset(&zstream, 0, sizeof(zstream));
  int code = inflateInit2(&zstream, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  std::vector<char> input_buffer;
  std::array<char, 4096> output_buffer;
  std::ostringstream decompressed;

  stream.seekg(0, std::ios::end);
  const auto size = stream.tellg();
  stream.seekg(0, std::ios::beg);

  if (size > 0) {
    input_buffer.resize(static_cast<std::size_t>(size));
    stream.read(input_buffer.data(), size);
  }

  zstream.next_in = reinterpret_cast<Bytef *>(input_buffer.data());
  zstream.avail_in = static_cast<uInt>(input_buffer.size());

  do {
    zstream.next_out = reinterpret_cast<Bytef *>(output_buffer.data());
    zstream.avail_out = sizeof(output_buffer);
    code = inflate(&zstream, Z_NO_FLUSH);

    if (code != Z_OK && code != Z_STREAM_END) {
      inflateEnd(&zstream);
      return std::nullopt;
    }

    decompressed.write(output_buffer.data(),
                       static_cast<long>(sizeof(output_buffer)) -
                           zstream.avail_out);
  } while (code != Z_STREAM_END);

  code = inflateEnd(&zstream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return decompressed.str();
}

} // namespace sourcemeta::core
