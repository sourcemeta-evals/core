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

auto gunzip(std::istream &stream) -> std::optional<std::string> {
  z_stream inflater;
  std::memset(&inflater, 0, sizeof(inflater));

  int code = inflateInit2(&inflater, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  std::array<char, 4096> input_buffer;
  std::array<char, 4096> output_buffer;
  std::string decompressed;

  while (code != Z_STREAM_END) {
    if (inflater.avail_in == 0) {
      stream.read(input_buffer.data(),
                  static_cast<std::streamsize>(input_buffer.size()));
      const auto read_size{stream.gcount()};
      if (read_size <= 0) {
        break;
      }

      inflater.next_in = reinterpret_cast<Bytef *>(input_buffer.data());
      inflater.avail_in = static_cast<uInt>(read_size);
    }

    inflater.next_out = reinterpret_cast<Bytef *>(output_buffer.data());
    inflater.avail_out = static_cast<uInt>(output_buffer.size());

    code = inflate(&inflater, Z_NO_FLUSH);
    if (code != Z_OK && code != Z_STREAM_END) {
      inflateEnd(&inflater);
      return std::nullopt;
    }

    decompressed.append(output_buffer.data(),
                        output_buffer.size() -
                            static_cast<std::size_t>(inflater.avail_out));
  }

  const auto end_code{inflateEnd(&inflater)};
  if (code != Z_STREAM_END || end_code != Z_OK) {
    return std::nullopt;
  }

  return decompressed;
}

} // namespace sourcemeta::core
