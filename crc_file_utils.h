#ifndef CRC_FILE_UTILS_H
#define CRC_FILE_UTILS_H

#include <fstream>
#include <vector>

namespace CRC {

template<class Accessor>
static bool calculate_full_file(
        const Accessor& acc,
        const std::string& filepath,
        typename Accessor::value_type &value
        ) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    auto crc = (acc.init_round) ? acc.init_round(value) : value;

    if (acc.make_round) {
        constexpr size_t buffer_size = 4096;
        std::vector<char> buffer(buffer_size);

        while (file.read(buffer.data(), buffer_size) || file.gcount() > 0) {
            size_t bytes_read = file.gcount();
            for (size_t i = 0; i < bytes_read; ++i) {
                crc = acc.make_round(crc, buffer[i]);
            }
        }
    }

    file.close();

    value = (acc.finalize_round) ? acc.finalize_round(crc) : crc;
    return true;
}

template<class Accessor>
static bool reverse_calculate_full_file(
        const Accessor& acc,
        const std::string& filepath,
        typename Accessor::value_type &value
        ) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }

    auto crc = (acc.init_round) ? acc.init_round(value) : value;

    std::streamsize file_size = file.tellg();

    if (acc.make_round) {
        constexpr size_t buffer_size = 4096;
        std::vector<char> buffer(buffer_size);

        std::streamsize bytes_left = file_size;

        while (bytes_left > 0) {
            std::streamsize chunk_size = (bytes_left > buffer_size) ? buffer_size : bytes_left;

            bytes_left -= chunk_size;
            file.seekg(bytes_left, std::ios::beg);

            if (file.read(buffer.data(), chunk_size)) {
                for (size_t i = chunk_size; i > 0; --i) {
                    crc = acc.make_round(crc, buffer[i - 1]);
                }
            }
        }
    }

    file.close();

    value = (acc.finalize_round) ? acc.finalize_round(crc) : crc;
    return true;
}

} // namespace CRC

#endif // CRC_FILE_UTILS_H
