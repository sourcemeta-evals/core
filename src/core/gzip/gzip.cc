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
  z_stream state;
  std::memset(&state, 0, sizeof(state));

  int code = inflateInit2(&state, 16 + MAX_WBITS);
  if (code != Z_OK) {
    return std::nullopt;
  }

  std::array<char, 4096> in_buffer;
  std::array<char, 4096> out_buffer;
  std::ostringstream result;

  do {
    stream.read(in_buffer.data(), in_buffer.size());
    state.avail_in = static_cast<uInt>(stream.gcount());
    state.next_in = reinterpret_cast<Bytef *>(in_buffer.data());

    if (state.avail_in == 0) {
      break;
    }

    do {
      state.avail_out = out_buffer.size();
      state.next_out = reinterpret_cast<Bytef *>(out_buffer.data());

      code = inflate(&state, Z_NO_FLUSH);
      if (code != Z_OK && code != Z_STREAM_END && code != Z_BUF_ERROR) {
        inflateEnd(&state);
        return std::nullopt;
      }

      result.write(out_buffer.data(),
                   static_cast<long>(out_buffer.size()) - state.avail_out);
    } while (state.avail_out == 0);
  } while (code != Z_STREAM_END);

  inflateEnd(&state);

  if (code == Z_STREAM_END) {
    return result.str();
  }

  return std::nullopt;
}

} // namespace sourcemeta::core
