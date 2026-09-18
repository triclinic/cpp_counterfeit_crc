#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include "crc_impl.h"
#include "crc_file_utils.h"

// 1. Объявляем тестовые политики для CRC
struct CRC32_Policy {
    using value_type = uint32_t;
    static constexpr uint32_t init = 0xFFFFFFFF;
    static constexpr uint32_t polynomial = 0x04C11DB7;
    static constexpr bool reflect_in = true;
    static constexpr bool reflect_out = true;
    static constexpr size_t bitness = 32;
    static constexpr uint32_t xor_out = 0xFFFFFFFF;
    static constexpr uint32_t check = 0xCBF43926; // Стандартный чек для "123456789"
};

struct CRC8_Policy {
    using value_type = uint8_t;
    static constexpr uint8_t init = 0x00;
    static constexpr uint8_t polynomial = 0x07; // CRC-8/SMBUS
    static constexpr bool reflect_in = false;
    static constexpr bool reflect_out = false;
    static constexpr size_t bitness = 8;
    static constexpr uint8_t xor_out = 0x00;
    static constexpr uint8_t check = 0xF4; // Чек для "123456789"
};

// ==========================================
// TЕСТЫ ВРЕМЕНИ КОМПИЛЯЦИИ (Compile-time)
// ==========================================
// Эти проверки гарантируют, что constexpr вычисления работают корректно.
// Если что-то сломается, код просто не скомпилируется.

static_assert(_crc_details::make_mask<uint32_t, 32>() == 0xFFFFFFFF, "Mask 32-bit failed");
static_assert(_crc_details::make_mask<uint32_t, 16>() == 0xFFFF, "Mask 16-bit failed");
static_assert(_crc_details::make_mask<uint8_t, 4>() == 0x0F, "Mask 4-bit failed");

static_assert(_crc_details::reverse<uint8_t, 8>(0b10100000) == 0b00000101, "Reverse 8-bit failed");
static_assert(_crc_details::reverse<uint32_t, 32>(0x80000000) == 0x00000001, "Reverse 32-bit failed");

static_assert(_crc_details::is_power_of_two(1), "1 is power of 2");
static_assert(_crc_details::is_power_of_two(1024), "1024 is power of 2");
static_assert(!_crc_details::is_power_of_two(0), "0 is not power of 2");
static_assert(!_crc_details::is_power_of_two(7), "7 is not power of 2");

// Проверка селектора типов данных по битам
static_assert(std::is_same_v<typename _crc_details::bit_type_selector<8>::type, uint8_t>);
static_assert(std::is_same_v<typename _crc_details::bit_type_selector<16>::type, uint16_t>);
static_assert(std::is_same_v<typename _crc_details::bit_type_selector<32>::type, uint32_t>);
static_assert(std::is_same_v<typename _crc_details::bit_type_selector<64>::type, uint64_t>);


// ==========================================
// ТЕСТЫ ВРЕМЕНИ ВЫПОЛНЕНИЯ (Runtime / Catch2)
// ==========================================

TEST_CASE("CRC::resolver basic compliance with standard vectors", "[crc]") {
    using CRC32 = CRC::resolver<CRC32_Policy>;
    using CRC8  = CRC::resolver<CRC8_Policy>;

    SECTION("String view and string literal calculation") {
        // Базовый чек-вектор "123456789"
        REQUIRE(CRC32::calculate_str("123456789") == CRC32_Policy::check);
        REQUIRE(CRC8::calculate_str("123456789") == CRC8_Policy::check);

        // Пустая строка
        REQUIRE(CRC32::calculate_str("") == 0x00000000); // Реверс + XOR над init дают 0
        REQUIRE(CRC8::calculate_str("") == 0x00);
    }

    SECTION("Raw memory calculation (void* and length)") {
        const uint8_t data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

        REQUIRE(CRC32::calculate(data, sizeof(data)) == CRC32_Policy::check);
        REQUIRE(CRC8::calculate(data, sizeof(data)) == CRC8_Policy::check);
    }

    SECTION("Incremental calculation using handles") {
        // Имитируем потоковую передачу данных по одному байту
        auto handle = CRC32::handle;

        uint32_t crc = handle.init_round(CRC32_Policy::init);

        std::string_view stream = "123456789";
        for (char ch : stream) {
            crc = handle.make_round(crc, static_cast<uint8_t>(ch));
        }

        crc = handle.finalize_round(crc);

        REQUIRE(crc == CRC32_Policy::check);
    }

    SECTION("Different initial values") {
        // Переопределение начального значения CRC в процессе
        std::string_view part1 = "12345";
        std::string_view part2 = "6789";

        uint32_t intermediate = CRC32::calculate_str(part1);

        // Важно: чтобы продолжить расчет, нужно передавать сырой CRC (до финализации)
        // Но так как calculate возвращает уже финализированный, проверим через побайтовый ввод:
        auto handle = CRC32::handle;
        uint32_t crc = handle.init_round(CRC32_Policy::init);

        for (char ch : part1) crc = handle.make_round(crc, ch);
        // Сейчас в crc промежуточное состояние. Передаем его в calculate как стартовое
        // Но resolver::calculate внутри делает init_round, поэтому трюк сработает корректно только
        // если init_round(crc) не портит состояние (для CRC32 с маской 32-бит это безопасно).

        uint32_t final_res = CRC32::calculate_str(part2, crc);
        REQUIRE(final_res == CRC32_Policy::check);
    }
}

TEST_CASE("Internal lookup tables generation", "[crc_details]") {
    // Проверяем, что таблицы создаются правильного размера (256 элементов)
    constexpr auto crc32_table = _crc_details::make_crc_array<uint32_t, 0x04C11DB7, 32, true>();
    REQUIRE(crc32_table.size() == 256);

    // Проверяем инверсную таблицу
    constexpr auto inv_crc32_table = _crc_details::make_inv_crc_array<uint32_t, 0x04C11DB7, 32, true>();
    REQUIRE(inv_crc32_table.size() == 256);

    // Известные константы для таблицы CRC-32 (IEEE 802.3)
    REQUIRE(crc32_table[0] == 0x00000000);
    REQUIRE(crc32_table[1] == 0x77073096);
    REQUIRE(crc32_table[2] == 0xEE0E612C);
}

// Объявляем тестовые политики, совместимые с patch_resolver (bitness >= 8 и степень двойки)
struct CRC32_Standard_Policy {
    using value_type = uint32_t;
    static constexpr uint32_t init = 0xFFFFFFFF;
    static constexpr uint32_t polynomial = 0x04C11DB7;
    static constexpr bool reflect_in = true;
    static constexpr bool reflect_out = true;
    static constexpr size_t bitness = 32;
    static constexpr uint32_t xor_out = 0xFFFFFFFF;
    static constexpr uint32_t check = 0xCBF43926; // Чек-вектор для "123456789"
};

struct CRC16_XMODEM_Policy {
    using value_type = uint16_t;
    static constexpr uint16_t init = 0x0000;
    static constexpr uint16_t polynomial = 0x1021;
    static constexpr bool reflect_in = false; // Без рефлексии для проверки второй ветки patch()
    static constexpr bool reflect_out = false;
    static constexpr size_t bitness = 16;
    static constexpr uint16_t xor_out = 0x0000;
    static constexpr uint16_t check = 0x31C3; // Чек-вектор для "123456789"
};

// ==========================================
// TЕСТЫ ВРЕМЕНИ КОМПИЛЯЦИИ (Compile-time)
// ==========================================
// static_assert внутри reverse_resolver автоматически проверяет валидность
// на этапе сборки: calculate("123456789", check) == init
template struct CRC::reverse_resolver<CRC32_Standard_Policy>;
template struct CRC::reverse_resolver<CRC16_XMODEM_Policy>;


// ==========================================
// ТЕСТЫ ВРЕМЕНИ ВЫПОЛНЕНИЯ (Runtime / Catch2)
// ==========================================

TEST_CASE("CRC::reverse_resolver tests", "[crc_reverse]") {
    using RevCRC32 = CRC::reverse_resolver<CRC32_Standard_Policy>;
    using RevCRC16 = CRC::reverse_resolver<CRC16_XMODEM_Policy>;

    SECTION("Reverse calculation from final CRC to init") {
        // Передаем правильный CRC и ожидаем на выходе значение init
        REQUIRE(RevCRC32::calculate_str("123456789", CRC32_Standard_Policy::check) == CRC32_Standard_Policy::init);
        REQUIRE(RevCRC16::calculate_str("123456789", CRC16_XMODEM_Policy::check) == CRC16_XMODEM_Policy::init);
    }

    SECTION("Reverse calculation with raw pointer and length") {
        std::string data = "Hello World!";
        // 1. Считаем прямым резолвером
        uint32_t forward_crc = CRC::resolver<CRC32_Standard_Policy>::calculate_str(data.c_str());

        // 2. Рассчитываем в обратную сторону
        uint32_t expected_init = RevCRC32::calculate(data.data(), data.size(), forward_crc);

        REQUIRE(expected_init == CRC32_Standard_Policy::init);
    }
}

TEST_CASE("CRC::patch_resolver tests", "[crc_patch]") {
    using Patcher32 = CRC::patch_resolver<CRC32_Standard_Policy>;
    using Patcher16 = CRC::patch_resolver<CRC16_XMODEM_Policy>;

    SECTION("Counterfeit CRC-32 (Reflected)") {
        // Исходная строка: "Hello, world! [____] Test" -> 25 байт
        // Мы хотим заставить строку иметь конкретный целевой CRC, изменив байты в "[____]" (индекс 15)
        std::string buffer = "Hello, world! [____] Test";
        size_t patch_offset = 15; // Позиция начала '[____]'
        uint32_t target_crc = 0xDEADBEEF;

        // Выполняем подделку контрольной суммы
        bool success = Patcher32::counterfeit(buffer.data(), buffer.size(), patch_offset, target_crc);

        REQUIRE(success == true);

        // Проверяем, действительно ли прямой расчет по измененному буферу даст target_crc
        uint32_t real_crc = Patcher32::calculate(buffer.data(), buffer.size());
        REQUIRE(real_crc == target_crc);
    }

    SECTION("Counterfeit CRC-16 (Non-Reflected)") {
        // Проверяем аналогичную логику для нерефлексивного полинома (XMODEM)
        std::string buffer = "Data payload: ____. End.";
        size_t patch_offset = 14; // Позиция '____'
        uint16_t target_crc = 0x1337;

        bool success = Patcher16::counterfeit(buffer.data(), buffer.size(), patch_offset, target_crc);

        REQUIRE(success == true);

        uint16_t real_crc = Patcher16::calculate(buffer.data(), buffer.size());
        REQUIRE(real_crc == target_crc);
    }

    SECTION("Counterfeit out of bounds check") {
        std::string buffer = "Short";
        uint32_t target_crc = 0x11223344;

        // Попытка записать 4 байта патча на позицию 3 при длине буфера 5 (3 + 4 > 5)
        bool success = Patcher32::counterfeit(buffer.data(), buffer.size(), 3, target_crc);

        REQUIRE(success == false);
    }

    SECTION("Verify handles block assignment") {
        // Проверяем корректность инициализации структуры patch_handle
        auto handle = Patcher32::handle;
        REQUIRE(handle.patch == &Patcher32::patch);
        REQUIRE(handle.forward.make_round != nullptr);
        REQUIRE(handle.backward.make_round != nullptr);
    }
}

struct CRC32_File_Policy {
    using value_type = uint32_t;
    static constexpr uint32_t init = 0xFFFFFFFF;
    static constexpr uint32_t polynomial = 0x04C11DB7;
    static constexpr bool reflect_in = true;
    static constexpr bool reflect_out = true;
    static constexpr size_t bitness = 32;
    static constexpr uint32_t xor_out = 0xFFFFFFFF;
    static constexpr uint32_t check = 0xCBF43926;
};

// Хелпер для создания файла с определенным контентом
static void create_test_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    file.write(content.data(), content.size());
}

// Хелпер для создания файла большого размера
static void create_large_test_file(const std::string& path, size_t size) {
    std::ofstream file(path, std::ios::binary);
    std::vector<char> buffer(4096, 'A');
    size_t written = 0;
    while (written < size) {
        size_t to_write = std::min(buffer.size(), size - written);
        file.write(buffer.data(), to_write);
        written += to_write;
    }
}

TEST_CASE("CRC file calculation functions", "[crc_file]") {
    using Resolver = CRC::resolver<CRC32_File_Policy>;
    using RevResolver = CRC::reverse_resolver<CRC32_File_Policy>;

    // Используем простое имя файла в текущей рабочей директории приложения
    std::string test_file_path = "temp_test_data_buffer.bin";

    SECTION("Non-existent file handling") {
        uint32_t crc_val = CRC32_File_Policy::init;

        REQUIRE_FALSE(CRC::calculate_full_file(Resolver::handle, "non_existent_file_xyz.dat", crc_val));
        REQUIRE_FALSE(CRC::reverse_calculate_full_file(RevResolver::handle, "non_existent_file_xyz.dat", crc_val));
    }

    SECTION("Empty file processing") {
        create_test_file(test_file_path, "");

        uint32_t forward_crc = CRC32_File_Policy::init;
        bool f_res = CRC::calculate_full_file(Resolver::handle, test_file_path, forward_crc);
        REQUIRE(f_res == true);
        REQUIRE(forward_crc == Resolver::calculate_str(""));

        uint32_t backward_crc = forward_crc;
        bool b_res = CRC::reverse_calculate_full_file(RevResolver::handle, test_file_path, backward_crc);
        REQUIRE(b_res == true);
        REQUIRE(backward_crc == CRC32_File_Policy::init);
    }

    SECTION("Standard small file (under 4096 bytes)") {
        std::string content = "123456789";
        create_test_file(test_file_path, content);

        uint32_t forward_crc = CRC32_File_Policy::init;
        bool f_res = CRC::calculate_full_file(Resolver::handle, test_file_path, forward_crc);

        REQUIRE(f_res == true);
        REQUIRE(forward_crc == CRC32_File_Policy::check);

        uint32_t backward_crc = CRC32_File_Policy::check;
        bool b_res = CRC::reverse_calculate_full_file(RevResolver::handle, test_file_path, backward_crc);

        REQUIRE(b_res == true);
        REQUIRE(backward_crc == CRC32_File_Policy::init);
    }

    SECTION("Large file processing (boundary conditions for 4096 bytes buffer)") {
        size_t test_size = GENERATE(4095, 4096, 8197);

        INFO("Testing file size: " << test_size);
        create_large_test_file(test_file_path, test_size);

        uint32_t file_crc = CRC32_File_Policy::init;
        REQUIRE(CRC::calculate_full_file(Resolver::handle, test_file_path, file_crc) == true);

        std::vector<char> mem_buffer(test_size, 'A');
        uint32_t mem_crc = Resolver::calculate(mem_buffer.data(), mem_buffer.size());

        REQUIRE(file_crc == mem_crc);

        uint32_t backward_crc = file_crc;
        REQUIRE(CRC::reverse_calculate_full_file(RevResolver::handle, test_file_path, backward_crc) == true);

        REQUIRE(backward_crc == CRC32_File_Policy::init);
    }

    SECTION("Missing optional handler functions (Nullptr safety)") {
        create_test_file(test_file_path, "ABC");

        CRC::crc_handle<uint32_t> null_handle = { nullptr, nullptr, nullptr };
        uint32_t crc_val = 0x12345678;

        REQUIRE(CRC::calculate_full_file(null_handle, test_file_path, crc_val) == true);
        REQUIRE(crc_val == 0x12345678);

        REQUIRE(CRC::reverse_calculate_full_file(null_handle, test_file_path, crc_val) == true);
        REQUIRE(crc_val == 0x12345678);
    }

    std::remove(test_file_path.c_str());
}
