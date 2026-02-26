#include <sourcemeta/core/gzip.h>

extern "C" {
#include <zlib.h>
}

#include <array>   // std::array
#include <sstream> // std::istringstream, std::ostringstream

namespace sourcemeta::core {

constexpr auto ZLIB_BUFFER_SIZE{4096};

auto gzip(const std::string &input) -> std::optional<std::string> {
  z_stream zstream{};
  if (deflateInit2(&zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS,
                   8, Z_DEFAULT_STRATEGY) != Z_OK) {
    return std::nullopt;
  }

  std::istringstream input_stream{input};
  std::ostringstream output;

  std::array<char, ZLIB_BUFFER_SIZE> buffer_input{};
  std::array<char, ZLIB_BUFFER_SIZE> buffer_output{};
  bool reached_end_of_input{false};
  auto code{Z_OK};

  while (code != Z_STREAM_END) {
    if (zstream.avail_in == 0 && !reached_end_of_input) {
      input_stream.read(buffer_input.data(), buffer_input.size());
      const auto bytes_read = input_stream.gcount();
      if (bytes_read > 0) {
        zstream.next_in = reinterpret_cast<Bytef *>(buffer_input.data());
        zstream.avail_in = static_cast<uInt>(bytes_read);
      } else {
        reached_end_of_input = true;
      }
    }

    zstream.next_out = reinterpret_cast<Bytef *>(buffer_output.data());
    zstream.avail_out = static_cast<uInt>(buffer_output.size());

    const int flush_mode = reached_end_of_input ? Z_FINISH : Z_NO_FLUSH;
    code = deflate(&zstream, flush_mode);
    if (code == Z_STREAM_ERROR) {
      deflateEnd(&zstream);
      return std::nullopt;
    }

    const auto bytes_written = buffer_output.size() - zstream.avail_out;
    if (bytes_written > 0) {
      output.write(buffer_output.data(),
                   static_cast<std::streamsize>(bytes_written));
      if (!output) {
        deflateEnd(&zstream);
        return std::nullopt;
      }
    }
  }

  if (deflateEnd(&zstream) != Z_OK) {
    return std::nullopt;
  }

  return output.str();
}

auto gunzip(std::istream &stream) -> std::optional<std::string> {
  z_stream zstream{};
  if (inflateInit2(&zstream, 16 + MAX_WBITS) != Z_OK) {
    return std::nullopt;
  }

  std::ostringstream output;
  std::array<char, ZLIB_BUFFER_SIZE> buffer_input{};
  std::array<char, ZLIB_BUFFER_SIZE> buffer_output{};

  auto code{Z_OK};
  while (code != Z_STREAM_END) {
    if (zstream.avail_in == 0 && stream) {
      stream.read(buffer_input.data(), buffer_input.size());
      const auto bytes_read = stream.gcount();
      if (bytes_read > 0) {
        zstream.next_in = reinterpret_cast<Bytef *>(buffer_input.data());
        zstream.avail_in = static_cast<uInt>(bytes_read);
      } else {
        break;
      }
    }

    zstream.next_out = reinterpret_cast<Bytef *>(buffer_output.data());
    zstream.avail_out = static_cast<uInt>(buffer_output.size());

    code = inflate(&zstream, Z_NO_FLUSH);
    if (code != Z_OK && code != Z_STREAM_END) {
      inflateEnd(&zstream);
      return std::nullopt;
    }

    const auto bytes_written = buffer_output.size() - zstream.avail_out;
    output.write(buffer_output.data(),
                 static_cast<std::streamsize>(bytes_written));
    if (!output) {
      inflateEnd(&zstream);
      return std::nullopt;
    }
  }

  code = inflateEnd(&zstream);
  if (code != Z_OK) {
    return std::nullopt;
  }

  return output.str();
}

} // namespace sourcemeta::core
