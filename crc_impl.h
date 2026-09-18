#ifndef CRC_IMPL_H
#define CRC_IMPL_H

#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include <array>
#include <utility>
#include <string_view>

#define CRC_STATIC_CHECK

namespace _crc_details {

template <typename T, std::size_t bitness>
constexpr T make_mask() noexcept {
    if constexpr (bitness >= sizeof(T) * 8) {
        return static_cast<T>(~static_cast<T>(0));
    } else {
        return (static_cast<T>(1) << bitness) - 1;
    }
}

template<class T, std::size_t bitness = sizeof(T) * 8>
constexpr T reverse(T n) noexcept {
    T reversed_n = 0u;
    for (std::size_t i = 0; i < bitness; ++i) {
        if ((n >> i) & 1) {
            reversed_n |= (static_cast<T>(1) << (bitness - i - 1));
        }
    }
    return reversed_n;
}

template <class T, T polynomial, bool reflect_in, std::size_t bitness>
constexpr T generate_forward_crc_value(std::size_t crc) noexcept {
    constexpr T mask = make_mask<T, bitness>();
    constexpr T local_poly = (reflect_in) ? reverse<T, bitness>(polynomial) : polynomial;

    if (not reflect_in) {
        if constexpr (bitness >= 8) {
            crc = crc << (bitness - 8);
        } else {
            crc = crc >> (8 - bitness);
        }
    }

    for (int j = 0; j < 8; j++) {
        if constexpr (reflect_in) {
            if ( crc & static_cast<T>(1) ) {
                crc = ((crc >> 1) ^ local_poly) & mask;
            }
            else {
                crc = (crc >> 1) & mask;
            }
        }
        else {
            if ( crc & (static_cast<T>(1) << (bitness - 1)) ) {
                crc = ((crc << 1) ^ local_poly) & mask;
            }
            else {
                crc = (crc << 1) & mask;
            }
        }
    }

    return static_cast<T>(crc & mask);
}

template <class T, T polynomial, bool reflect_in, std::size_t bitness>
constexpr T generate_backward_crc_value(std::size_t crc) noexcept {
    size_t shift = (bitness >= 8) ? (bitness - 8) : 0;

    for( unsigned int i = 0; i < 256; ++i ) {
        auto tmp = generate_forward_crc_value<T, polynomial, reflect_in, bitness>(i);

        if constexpr (reflect_in) {
            if((uint8_t)(tmp >> shift) == crc) {
                crc = (tmp << 8) ^ i;
                break;
            }
        }
        else {
            if ((uint8_t)tmp == crc) {
                crc = (tmp >> 8) ^ (static_cast<T>(i) << shift);
                break;
            }
        }
    }

    return crc;
}

template <class T, T polynomial, bool reflect_in, std::size_t bitness>
constexpr T generate_backward2_crc_value(std::size_t crc) noexcept {
    size_t shift = (bitness >= 8) ? (bitness - 8) : 0;

    for( unsigned int i = 256; i > 0; --i ) {
        auto tmp = generate_forward_crc_value<T, polynomial, reflect_in, bitness>(i - 1);

        if constexpr (reflect_in) {
            if((uint8_t)(tmp >> shift) == crc) {
                crc = (tmp << 8) ^ (i - 1);
                break;
            }
        }
        else {
            if ((uint8_t)tmp == crc) {
                crc = (tmp >> 8) ^ (static_cast<T>(i - 1) << shift);
                break;
            }
        }
    }

    return crc;
}

template <class T, T polynomial, bool reflect_in, std::size_t bitness, std::size_t... Is>
constexpr auto make_crc_array_helper(std::index_sequence<Is...>) noexcept {
    return std::array<T, sizeof...(Is)>{{generate_forward_crc_value<T, polynomial, reflect_in, bitness>(Is)...}};
}

template <class T, T polynomial, bool reflect_in, std::size_t bitness, std::size_t... Is>
constexpr auto make_inv_crc_array_helper(std::index_sequence<Is...>) noexcept {
    return std::array<T, sizeof...(Is)>{{generate_backward2_crc_value<T, polynomial, reflect_in, bitness>(Is)...}};
}

template <class T, T polynomial, std::size_t bitness, bool reflect_in = false, std::size_t N = 256>
constexpr auto make_crc_array() noexcept {
    return make_crc_array_helper<T, polynomial, reflect_in, bitness>(std::make_index_sequence<N>{});
}

template <class T, T polynomial, std::size_t bitness, bool reflect_in = false, std::size_t N = 256>
constexpr auto make_inv_crc_array() noexcept {
    return make_inv_crc_array_helper<T, polynomial, reflect_in, bitness>(std::make_index_sequence<N>{});
}

template <unsigned bitness, typename Enable = void>
struct bit_type_selector;

template <unsigned bitness>
struct bit_type_selector<bitness, typename std::enable_if<bitness <= 8>::type> {
    using type = uint8_t;
};

template <unsigned bitness>
struct bit_type_selector<bitness, typename std::enable_if< (bitness > 8) and (bitness <= 16) >::type> {
    using type = uint16_t;
};

template <unsigned bitness>
struct bit_type_selector<bitness, typename std::enable_if< (bitness > 16) and (bitness <= 32) >::type> {
    using type = uint32_t;
};

template <unsigned bitness>
struct bit_type_selector<bitness, typename std::enable_if< (bitness > 32) and (bitness <= 64) >::type> {
    using type = uint64_t;
};

constexpr bool is_power_of_two(size_t x) { return x && !(x & (x - 1)); }

} // namespace _crc_details

namespace CRC {

template <class T>
struct crc_handle {
    using value_type = T;

    value_type (*init_round)(value_type crc) = nullptr;
    value_type (*finalize_round)(value_type crc) = nullptr;
    value_type (*make_round)(value_type crc, uint8_t byte) = nullptr;
};

template <class T>
struct patch_handle {
    using value_type = T;

    crc_handle<value_type> forward{}, backward{};
    void (*patch)(value_type from, value_type to, uint8_t * buff) = nullptr;
};

template<class crc_policy>
struct resolver {
    using value_type = typename crc_policy::value_type;
    static constexpr value_type init = crc_policy::init;
    static constexpr value_type polynomial = crc_policy::polynomial;
    static constexpr bool reflect_in = crc_policy::reflect_in;
    static constexpr bool reflect_out = crc_policy::reflect_out;
    static constexpr size_t bitness = crc_policy::bitness;
    static constexpr value_type xor_out = crc_policy::xor_out;
    static constexpr value_type mask = _crc_details::make_mask<value_type, bitness>();

    static constexpr auto table = _crc_details::make_crc_array< value_type, polynomial, bitness, reflect_in>();

    static inline constexpr value_type make_round(value_type crc, uint8_t byte) noexcept {
        if constexpr (reflect_in) {
            crc = (crc >> 8) ^ table[(crc ^ byte) & 0xff];
        }
        else {
            if constexpr (bitness >= 8) {
                crc = ((crc << 8) ^ table[((crc >> (bitness - 8)) ^ byte) & 0xff]) & mask;
            } else {
                crc = table[((crc << (8 - bitness)) ^ byte) & 0xff] & mask;
            }
        }
        return crc;
    }

    static inline constexpr value_type init_round(value_type crc) noexcept {
        return crc & mask;
    }

    static inline constexpr value_type finalize_round(value_type crc) noexcept {
        if constexpr (reflect_out != reflect_in) {
            crc = _crc_details::reverse<value_type, bitness>(crc);
        }
        return (crc ^ xor_out) & mask;
    }

    static value_type calculate(const void * c_ptr, size_t len, value_type crc) noexcept {
        const uint8_t* c = static_cast<const uint8_t *>(c_ptr);
        crc = init_round(crc);

        while (len--) {
            crc = make_round(crc, *c++);
        }

        return finalize_round(crc);
    }

    static value_type calculate(const void * c_ptr, size_t len) noexcept {
        return calculate(c_ptr, len, init);
    }

    static constexpr value_type calculate_str(std::string_view str, value_type crc = init) noexcept {
        crc = init_round(crc);

        for (char ch : str) {
            uint8_t byte = static_cast<uint8_t>(ch);
            crc = make_round(crc, byte);
        }

        return finalize_round(crc);
    }

    static constexpr value_type calculate_str(const char * str, value_type crc = init) noexcept {
        return calculate_str(std::string_view(str), crc);
    }

    static constexpr crc_handle<value_type> handle = {
        .init_round = &init_round,
        .finalize_round = &finalize_round,
        .make_round = &make_round
    };
#ifdef CRC_STATIC_CHECK
    static_assert( calculate_str("123456789", crc_policy::init) == crc_policy::check, "Bad CRC vector!" );
#endif
};

template<class crc_policy>
struct reverse_resolver {
    using value_type = typename crc_policy::value_type;
    static constexpr value_type init = crc_policy::init;
    static constexpr value_type polynomial = crc_policy::polynomial;
    static constexpr bool reflect_in = crc_policy::reflect_in;
    static constexpr bool reflect_out = crc_policy::reflect_out;
    static constexpr size_t bitness = crc_policy::bitness;
    static constexpr value_type xor_out = crc_policy::xor_out;
    static constexpr value_type mask = _crc_details::make_mask<value_type, bitness>();

    static constexpr auto table = _crc_details::make_inv_crc_array< value_type, polynomial, bitness, reflect_in>();

    static inline constexpr value_type make_round(value_type crc, uint8_t byte) noexcept {
        if constexpr (reflect_in) {
            crc = (((crc << 8) ^ table[(crc >> (bitness - 8)) & 0xff] ^ byte) & mask);
        }
        else {
            size_t shift = (bitness >= 8) ? (bitness - 8) : 0;
            crc = (((crc >> 8) ^ table[crc & 0xff] ^ (static_cast<value_type>(byte) << shift)) & mask);
        }
        return crc;
    }

    static inline constexpr value_type init_round(value_type crc) noexcept {
        crc = (crc ^ xor_out) & mask;
        if constexpr (reflect_out != reflect_in) {
            crc = _crc_details::reverse<value_type, bitness>(crc);
        }
        return crc;
    }

    static inline constexpr value_type finalize_round(value_type crc) noexcept {
        return crc & mask;
    }

    static value_type calculate(const void * c_ptr, size_t len, value_type crc) noexcept {
        const uint8_t * c = static_cast<const uint8_t *>(c_ptr);

        crc = init_round(crc);
        c += len - 1;

        while (len--) {
            crc = make_round(crc, *c--);
        }
        return finalize_round(crc);
    }

    static constexpr value_type calculate_str(std::string_view str, value_type crc) noexcept {
        crc = init_round(crc);

        if (not str.empty()) {
            size_t i = str.size();
            do {
                --i;
                uint8_t byte = static_cast<uint8_t>(str[i]);
                crc = make_round(crc, byte);
            } while (i > 0);
        }
        return finalize_round(crc);
    }

    static constexpr value_type calculate_str(const char * str, value_type crc) noexcept {
        return calculate_str(std::string_view(str), crc);
    }

    static constexpr crc_handle<value_type> handle = {
        .init_round = &init_round,
        .finalize_round = &finalize_round,
        .make_round = &make_round
    };
#ifdef CRC_STATIC_CHECK
    static_assert( calculate_str(std::string_view("123456789"), crc_policy::check) == init, "Bad inv CRC vector!" );
#endif
};

template<class crc_policy, class = typename std::enable_if<_crc_details::is_power_of_two(crc_policy::bitness) && (crc_policy::bitness >= 8)>::type>
struct patch_resolver : resolver<crc_policy>, reverse_resolver<crc_policy> {
    using value_type = typename crc_policy::value_type;
    static constexpr value_type init = crc_policy::init;
    static constexpr value_type polynomial = crc_policy::polynomial;
    static constexpr bool reflect_in = crc_policy::reflect_in;
    static constexpr size_t bitness = crc_policy::bitness;
    static constexpr value_type xor_out = crc_policy::xor_out;
    static constexpr value_type mask = _crc_details::make_mask<value_type, bitness>();

    static inline value_type calculate(const void * c_ptr, size_t len, value_type crc = crc_policy::init) {
        return resolver<crc_policy>::calculate(c_ptr, len, crc);
    }
    static inline value_type inv_calculate(const void * c_ptr, size_t len, value_type crc) {
        return reverse_resolver<crc_policy>::calculate(c_ptr, len, crc);
    }

    static void patch(value_type from, value_type to, uint8_t * buff) noexcept {
        constexpr size_t bytes_count = sizeof(value_type);
        unsigned temp[bytes_count] = {0};

        from = reverse_resolver<crc_policy>::init_round(from);

        if constexpr (reflect_in) {
            for (size_t i = bytes_count; i > 0; --i) {
                int j = 0;
                for (; j < 256; j++) {
                    if (((resolver<crc_policy>::table[j] ^ to) & (static_cast<value_type>(0xFF) << (bitness - 8))) == 0)
                        break;
                }
                temp[i - 1] = j;
                value_type t = resolver<crc_policy>::table[temp[i - 1]];
                to = ((to ^ t) << 8) | temp[i - 1];
            }

            for (size_t i = 0; i < bytes_count; i++) {
                buff[i] = static_cast<uint8_t>(from ^ temp[i]);
                from = resolver<crc_policy>::make_round(from, buff[i]);
            }
        }
        else {
            for (size_t i = 0; i < bytes_count; ++i) {
                uint8_t target_low = static_cast<uint8_t>(to & 0xFF);
                int j = 0;
                for (; j < 256; j++) {
                    if (static_cast<uint8_t>(resolver<crc_policy>::table[j] & 0xFF) == target_low) {
                        break;
                    }
                }
                temp[bytes_count - 1 - i] = j;
                to = (to ^ resolver<crc_policy>::table[j]) >> 8;
            }

            for (size_t i = 0; i < bytes_count; ++i) {
                buff[i] = static_cast<uint8_t>((from >> (bitness - 8)) & 0xFF) ^ static_cast<uint8_t>(temp[i]);
                from = resolver<crc_policy>::make_round(from, buff[i]);
            }
        }
    }

    static bool counterfeit(void * buff, size_t size, size_t at, value_type target, value_type init = crc_policy::init) noexcept {
        if ( at + sizeof(value_type) > size)
            return false;

        uint8_t * buffer = (uint8_t *)buff;
        value_type from = calculate(buffer, at, init);
        value_type to = inv_calculate(buffer + at + sizeof(value_type), size - (at + sizeof(value_type)), target);

        patch(from, to, buffer + at);
        if(calculate(buffer, size, init) == target)
            return true;

        return false;
    }

    static constexpr patch_handle<value_type> handle = {
        .forward = resolver<crc_policy>::handle,
        .backward = reverse_resolver<crc_policy>::handle,
        .patch = &patch
    };
};

template< size_t N >
struct Base {
    using value_type = typename _crc_details::bit_type_selector<N>::type;
    static constexpr size_t bitness = N;
    static constexpr value_type polynomial = 0;
    static constexpr value_type init = 0;
    static constexpr value_type xor_out = 0;
    static constexpr value_type check = ~static_cast<value_type>(0);
    static constexpr bool reflect_out = false;
    static constexpr bool reflect_in = false;
};

} // namespace CRC

#endif // CRC_IMPL_H
