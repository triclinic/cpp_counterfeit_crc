#ifndef CRC_H
#define CRC_H

#include "crc_impl.h"

namespace CRC8 {

struct POLY_49 : CRC::Base<8> {
    static constexpr value_type polynomial = 0x49;
    static constexpr value_type check = 0x6B;
};

struct POLY_39 : CRC::Base<8> {
    static constexpr value_type polynomial = 0x39;
    static constexpr value_type check = 0x8F;
};

struct POLY_D5 : CRC::Base<8> {
    static constexpr value_type polynomial = 0xD5;
    static constexpr value_type check = 0xBC;
};
using DVB_S2 = POLY_D5;

struct POLY_A7 : CRC::Base<8> {
    static constexpr value_type polynomial = 0xA7;
    static constexpr value_type check = 0x76;
};

struct POLY_31 : CRC::Base<8> {
    static constexpr value_type polynomial = 0x31;
    static constexpr value_type check = 0xA2;
};

struct POLY_07 : CRC::Base<8> {
    static constexpr value_type polynomial = 0x07;
    static constexpr value_type check = 0xF4;
};
using SMBUS = POLY_07;

struct POLY_2F : CRC::Base<8> {
    static constexpr value_type polynomial = 0x2f;
    static constexpr value_type check = 0x3e;
};
using OPENSAFETY = POLY_2F;

struct POLY_9B : CRC::Base<8> {
    static constexpr value_type polynomial = 0x9B;
    static constexpr value_type check = 0xea;
};
using LTE = POLY_9B;

struct POLY_1D : CRC::Base<8> {
    static constexpr value_type polynomial = 0x1D;
    static constexpr value_type check = 0x37;
};

struct AUTOSAR : POLY_2F {
    static constexpr value_type init = 0xff;
    static constexpr value_type xor_out = 0xff;
    static constexpr value_type check = 0xdf;
};

struct WCDMA : POLY_9B {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x25;
};

struct CDMA2000 : POLY_9B {
    static constexpr value_type init = 0xff;
    static constexpr value_type check = 0xda;
};

struct TECH_3250 : POLY_1D {
    static constexpr value_type init = 0xFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x97;
};

struct MIFARE_MAD : POLY_1D {
    static constexpr value_type init = 0xC7;
    static constexpr value_type check = 0x99;
};

struct SAE_J1850 : POLY_1D {
    static constexpr value_type init = 0xFF;
    static constexpr value_type xor_out = 0xFF;
    static constexpr value_type check = 0x4B;
};

struct I_CODE : POLY_1D {
    static constexpr value_type init = 0xFD;
    static constexpr value_type check = 0x7E;
};

struct HITAG : POLY_1D {
    static constexpr value_type init = 0xFF;
    static constexpr value_type check = 0xB4;
};

struct ROHC : POLY_07 {
    static constexpr value_type init = 0xFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xD0;
};

struct I_432_1 : POLY_07 {
    static constexpr value_type xor_out = 0x55;
    static constexpr value_type check = 0xA1;
};

struct MAXIM_DOW : POLY_31 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xA1;
};

struct NRSC_5 : POLY_31 {
    static constexpr value_type init = 0xFF;
    static constexpr value_type check = 0xF7;
};

struct BLUETOOTH : POLY_A7 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x26;
};

struct DARC : POLY_39 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x15;
};

struct GSM_B : POLY_49 {
    static constexpr value_type xor_out = 0xFF;
    static constexpr value_type check = 0x94;
};

} // namespace CRC8

// namespace CRC12 {

// struct _3GPP : CRC::Base<12> {
//     static constexpr value_type polynomial = 0x80F;
//     //static constexpr bool reflect_out = true;
//     static constexpr value_type check = 0xF5B;//0xDAF;
// };

// struct CDMA2000 : CRC::Base<12> {
//     static constexpr value_type init = 0xFFF;
//     static constexpr value_type polynomial = 0xF13;
//     static constexpr value_type check = 0xD4D;
// };

// struct GSM : CRC::Base<12> {
//     static constexpr value_type init = 0xFFFF;
//     static constexpr value_type xor_out = 0xFFFF;
//     static constexpr value_type polynomial = 0xd31;
//     static constexpr value_type check = 0x724;
// };

// }

namespace CRC16 {

struct POLY_1021 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x1021;
    static constexpr value_type check = 0x31C3;
};
using ACORN = POLY_1021;
using LTE = POLY_1021;
using V_41_MSB = POLY_1021;
using ZMODEM = POLY_1021;
using XMODEM = POLY_1021;

struct POLY_8005 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x8005;
    static constexpr value_type check = 0xFEE8;
};
using IBM = POLY_8005;
using BUYPASS = POLY_8005;
using VERIFONE = POLY_8005;
using UMTS = POLY_8005;

struct POLY_2F15 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x2F15;
    static constexpr value_type check = 0xA2D1;
};
using CHAKRAVARTY = POLY_2F15;

struct POLY_BAAD : CRC::Base<16> {
    static constexpr value_type polynomial = 0xBAAD;
    static constexpr value_type check = 0x715D;
};
using KOOPMAN = POLY_BAAD;

//struct POLY_AC9A : CRC::Base<16> {
//    static constexpr value_type polynomial = 0xAC9A;
//    static constexpr value_type check = 0x74a2;
//};
//struct CASTAGNOLI : POLY_AC9A {
//    static constexpr value_type init = 0xFFFFul;
//    static constexpr value_type xor_out = 0xFFFFul;
//    static constexpr bool reflect_in = true;
//    static constexpr value_type check = 0x9398;
//};

struct POLY_A02B : CRC::Base<16> {
    static constexpr value_type polynomial = 0xA02B;
    static constexpr value_type check = 0xEBA4;
};
using ARINC = POLY_A02B;

struct POLY_C867 : CRC::Base<16> {
    static constexpr value_type polynomial = 0xC867;
    static constexpr value_type check = 0xE355;
};

struct POLY_0589 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x0589;
    static constexpr value_type check = 0x007F;
};
using DECT_X = POLY_0589;

struct POLY_8BB7 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x8BB7;
    static constexpr value_type check = 0xD0DB;
};
using T10_DIF = POLY_8BB7;

struct POLY_3D65 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x3D65;
    static constexpr value_type check = 0x3D48;
};

struct POLY_5935 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x5935;
    static constexpr value_type check = 0x5D38;
};
using OPENSAFETY_A = POLY_5935;

struct POLY_755B : CRC::Base<16> {
    static constexpr value_type polynomial = 0x755B;
    static constexpr value_type check = 0x20FE;
};
using OPENSAFETY_B = POLY_755B;

struct POLY_1DCF : CRC::Base<16> {
    static constexpr value_type polynomial = 0x1DCF;
    static constexpr value_type check = 0xC7FC;
};

struct POLY_6F63 : CRC::Base<16> {
    static constexpr value_type polynomial = 0x6F63;
    static constexpr value_type check = 0xBDF4;
};
using LJ1200 = POLY_6F63;

struct POLY_080B : CRC::Base<16> {
    static constexpr value_type polynomial = 0x080B;
    static constexpr value_type check = 0x4FB0;
};

struct POLY_A097 : CRC::Base<16> {
    static constexpr value_type polynomial = 0xA097;
    static constexpr value_type check = 0x0FB3;
};
using TELEDISK = POLY_A097;

struct KERMIT : POLY_1021 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x2189;
};
using BLUETOOTH = KERMIT;
using CCITT = KERMIT;
using CCITT_TRUE = KERMIT;
using V_41_LSB = KERMIT;

struct MODBUS : POLY_8005 {
    static constexpr value_type init = 0xFFFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x4B37;
};

struct USB : POLY_8005 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xB4C8;
};

struct ARC : POLY_8005 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xBB3D;
};

struct CMS : POLY_8005 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type check = 0xAEE7;
};

struct DDS_110 : POLY_8005 {
    static constexpr value_type init = 0x800D;
    static constexpr value_type check = 0x9ECF;
};

struct CDMA2000 : POLY_C867 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type check = 0x4C06;
};

struct DECT_R : POLY_0589 {
    static constexpr value_type xor_out = 0x0001;
    static constexpr value_type check = 0x007E;
};

struct DNP : POLY_3D65 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0xEA82;
};

struct EN_13757 : POLY_3D65 {
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0xC2B7;
};

struct GENIBUS : POLY_1021 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0xD64E;
};
using DARC = GENIBUS;
using EPC = GENIBUS;
using EPC_C1G2 = GENIBUS;
using I_CODE = GENIBUS;

struct GSM : POLY_1021 {
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0xCE3C;
};

struct IBM_3740 : POLY_1021 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type check = 0x29B1;
};
using AUTOSAR = IBM_3740;
using CCITT_FALSE = IBM_3740;

struct IBM_SDLC : POLY_1021 {
    static constexpr value_type init = 0xFFFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0x906E;
};
using ISO_HDLC = IBM_SDLC;
using ISO_IEC_14443_3_B = IBM_SDLC;
using X_25 = IBM_SDLC;


struct M17 : POLY_5935 {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type check = 0x772B;
};

struct MAXIM_DOW : POLY_8005 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0x44C2;
};
using MAXIM = MAXIM_DOW;

struct MCRF4XX : POLY_1021 {
    static constexpr value_type init = 0xFFFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x6F91;
};

struct NRSC_5 : POLY_080B {
    static constexpr value_type init = 0xFFFF;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xA066;
};

struct PROFIBUS : POLY_1DCF {
    static constexpr value_type init = 0xFFFF;
    static constexpr value_type xor_out = 0xFFFF;
    static constexpr value_type check = 0xA819;
};
using IEC_61158_2 = PROFIBUS;

struct SPI_FUJITSU : POLY_1021 {
    static constexpr value_type init = 0x1D0F;
    static constexpr value_type check = 0xE5CC;
};
using AUG_CCITT = SPI_FUJITSU;

// struct TMS37157 : XMODEM {
//     static constexpr value_type init = 0x89EC;
//     static constexpr bool reflect_out = true;
//     static constexpr bool reflect_in = true;
//     static constexpr value_type check = 0x26B1;
// };

// struct ISO_IEC_14443_3_A : XMODEM {
//     static constexpr value_type init = 0xC6C6;
//     static constexpr bool reflect_out = true;
//     static constexpr bool reflect_in = true;
//     static constexpr value_type check = 0xBF05;
// };

// struct RIELLO : XMODEM {
//     static constexpr value_type init = 0xB2AA;
//     static constexpr bool reflect_in = true;
//     static constexpr value_type check = 0x63D0;
// };

} // namespace CRC16

namespace CRC32 {

struct POLY_F4ACFB13 : CRC::Base<32> {
    static constexpr value_type polynomial = 0xF4ACFB13ul;
    static constexpr value_type check = 0x6C9F84A8ul;
};

struct POLY_04C11DB7 : CRC::Base<32> {
    static constexpr value_type polynomial = 0x04C11DB7ul;
    static constexpr value_type check = 0x89A1897Ful;
};
using IEEE_802_3 = POLY_04C11DB7;

struct POLY_000000AF : CRC::Base<32> {
    static constexpr value_type polynomial = 0x000000AFul;
    static constexpr value_type check = 0xBD0BE338ul;
};
using XFER = POLY_000000AF;

struct POLY_814141AB : CRC::Base<32> {
    static constexpr value_type polynomial = 0x814141ABul;
    static constexpr value_type check = 0x3010BF7Ful;
};
using AIXM = POLY_814141AB;

struct POLY_1EDC6F41 : CRC::Base<32> {
    static constexpr value_type polynomial = 0x1EDC6F41ul;
    static constexpr value_type check = 0xC052A8C8ul;
};

struct POLY_A833982B : CRC::Base<32> {
    static constexpr value_type polynomial = 0xA833982Bul;
    static constexpr value_type check = 0x7DC48B71ul;
};

struct POLY_8001801B : CRC::Base<32> {
    static constexpr value_type polynomial = 0x8001801Bul;
    static constexpr value_type check = 0x8A7C3B7Eul;
};

struct POLY_741B8CD7 : CRC::Base<32> {
    static constexpr value_type polynomial = 0x741B8CD7ul;
    static constexpr value_type check = 0x85A3197ul;
};

///////////////////////////////////////////////////////////////////////
struct AUTOSAR : POLY_F4ACFB13 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x1697D06Aul;
};

struct BASE91_D : POLY_A833982B {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x87315576ul;
};

struct BZIP2 : POLY_04C11DB7 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr value_type check = 0xFC891918ul;
};
using AAL5 = BZIP2;
using DECT_B = BZIP2;

struct CD_ROM_EDC : POLY_8001801B {
    static constexpr bool reflect_in = true;
    static constexpr bool reflect_out = true;
    static constexpr value_type check = 0x6EC2EDC4ul;
};

struct POSIX : POLY_04C11DB7 {
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr value_type check = 0x765E7680ul;
};
using CKSUM = POSIX;

struct ISCSI : POLY_1EDC6F41 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xE3069283ul;
};
using BASE91_C = ISCSI;
using CASTAGNOLI = ISCSI;
using INTERLAKEN = ISCSI;

struct PKZIP : POLY_04C11DB7 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type xor_out = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xCBF43926ul;
};
using ISO_HDLC = PKZIP;
using ADCCP = PKZIP;
using V_42 = PKZIP;
using XZ = PKZIP;

struct JAMCRC : POLY_04C11DB7 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x340BC6D9ul;
};

struct MEF : POLY_741B8CD7 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xD2C22F51ul;
};

struct MPEG_2 : POLY_04C11DB7 {
    static constexpr value_type init = 0xFFFFFFFFul;
    static constexpr value_type check = 0x0376E6E7ul;
};

} // namespace CRC32

namespace CRC64 {

struct POLY_000000000000001B : CRC::Base<64> {
    static constexpr value_type polynomial = 0x000000000000001Bull;
    static constexpr value_type check = 0xe4ffbea588933790ull;
};
using ISO = POLY_000000000000001B;

struct POLY_42F0E1EBA9EA3693 : CRC::Base<64> {
    static constexpr value_type polynomial = 0x42F0E1EBA9EA3693ull;
    static constexpr value_type check = 0x6C40DF5F0B497347ull;
};
using ECMA = POLY_42F0E1EBA9EA3693;

struct POLY_259C84CBA6426349 : CRC::Base<64> {
    static constexpr value_type polynomial = 0x259C84CBA6426349ull;
    static constexpr value_type check = 0x3e921c142489a472ull;
};

struct POLY_AD93D23594C935A9 : CRC::Base<64> {
    static constexpr value_type polynomial = 0xAD93D23594C935A9ull;
    static constexpr value_type check = 0xf1d27d0a6c25edc8ull;
};

struct NVME : POLY_AD93D23594C935A9 {
    static constexpr value_type init = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type xor_out = 0xFFFFFFFFFFFFFFFFull;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x3558e8e979f60d7eull; //0xAE9CE8123001843Aull;
};

struct MS : POLY_259C84CBA6426349 {
    static constexpr value_type init = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type xor_out = 0xFFFFFFFFFFFFFFFFull;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x8a2b48b0fdb13115ull; //0x75D4B727EFD2075Aull;
};

struct GO_ISO : POLY_000000000000001B {
    static constexpr value_type init = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type xor_out = 0xFFFFFFFFFFFFFFFFull;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0xB90956C775A41001ull;
};

struct GO_ECMA : POLY_42F0E1EBA9EA3693 {
    static constexpr value_type init = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type xor_out = 0xFFFFFFFFFFFFFFFFull;
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x995DC9BBDF1939FAull;
};
using XZ = GO_ECMA;

struct JONES : POLY_42F0E1EBA9EA3693 {
    static constexpr bool reflect_out = true;
    static constexpr bool reflect_in = true;
    static constexpr value_type check = 0x2b9c7ee4e2780c8aull;
};

struct WE : POLY_42F0E1EBA9EA3693 {
    static constexpr value_type init = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type xor_out = 0xFFFFFFFFFFFFFFFFull;
    static constexpr value_type check = 0x62ec59e3f1a4f00aull; //0x62ECAEAC00BCF0F6ull;
};

} // namespace CRC64

#endif // CRC_H
