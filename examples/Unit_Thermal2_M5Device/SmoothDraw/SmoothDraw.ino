
#include <M5Unified.h>  // https://github.com/m5stack/M5Unified/

#include <M5_Thermal2.h>

M5_Thermal2 thermal2;

auto& display = M5.Display;

static constexpr uint8_t frame_width  = 32;
static constexpr uint8_t frame_height = 24;

static constexpr const char* graph_text_table[] = {"Med", "Avg", "High", "Low"};

static constexpr const uint32_t graph_color_table[] = {
    0x00808080u, 0x00008000u, 0x00FFCF00u, 0x0000CFFFu};

static constexpr const int step_table[] = {1,  2,   5,   10,     20,
                                           50, 100, 200, INT_MAX};

const int32_t raw_zero = M5_Thermal2::convertCelsiusToRaw(0.0f);

static constexpr const uint16_t color_map_table[][256] = {
    {
        // colormap_golden
        0x0004, 0x0004, 0x0004, 0x0004, 0x0005, 0x0005, 0x0825, 0x0825, 0x0825,
        0x0826, 0x0826, 0x0826, 0x1027, 0x1027, 0x1027, 0x1027, 0x1828, 0x1828,
        0x1848, 0x1849, 0x2049, 0x2049, 0x204A, 0x204A, 0x284A, 0x284B, 0x284B,
        0x284B, 0x306C, 0x306C, 0x306C, 0x386D, 0x386D, 0x386D, 0x408E, 0x408E,
        0x408E, 0x408F, 0x488F, 0x488F, 0x4890, 0x5090, 0x50B0, 0x50B0, 0x58B1,
        0x58B1, 0x58B1, 0x58B1, 0x60D2, 0x60D2, 0x60D2, 0x68D2, 0x68D2, 0x68D2,
        0x68F3, 0x70F3, 0x70F3, 0x70F3, 0x78F3, 0x7913, 0x7913, 0x7913, 0x8113,
        0x8133, 0x8133, 0x8133, 0x8933, 0x8932, 0x8952, 0x9152, 0x9152, 0x9152,
        0x9151, 0x9971, 0x9971, 0x9971, 0x9970, 0xA190, 0xA190, 0xA18F, 0xA98F,
        0xA9AF, 0xA9AE, 0xA9AE, 0xB1AD, 0xB1CD, 0xB1CD, 0xB9CC, 0xB9EC, 0xB9EB,
        0xB9EB, 0xC1EB, 0xC20A, 0xC20A, 0xCA09, 0xCA29, 0xCA29, 0xCA28, 0xCA28,
        0xD247, 0xD247, 0xD247, 0xDA66, 0xDA66, 0xDA65, 0xDA85, 0xDA85, 0xE284,
        0xE2A4, 0xE2A4, 0xE2A3, 0xEAC3, 0xEAC3, 0xEAE2, 0xEAE2, 0xEAE2, 0xF2E2,
        0xF301, 0xF301, 0xF321, 0xF321, 0xF321, 0xF340, 0xFB40, 0xFB40, 0xFB60,
        0xFB60, 0xFB80, 0xFB80, 0xFB80, 0xFBA0, 0xFBA0, 0xFBC0, 0xFBC0, 0xFBE0,
        0xFBE0, 0xFBE0, 0xFC00, 0xFC00, 0xFC20, 0xFC20, 0xFC40, 0xFC40, 0xFC60,
        0xFC60, 0xFC80, 0xFC80, 0xFC80, 0xFCA0, 0xFCA0, 0xFCC0, 0xFCE0, 0xFCE0,
        0xFD00, 0xFD00, 0xFD20, 0xFD20, 0xFD40, 0xFD40, 0xFD40, 0xFD60, 0xFD60,
        0xFD80, 0xFDA0, 0xFDA0, 0xFDC0, 0xFDC0, 0xFDC1, 0xFDE1, 0xFDE1, 0xFE01,
        0xFE01, 0xFE21, 0xFE21, 0xFE41, 0xFE42, 0xFE62, 0xFE62, 0xFE62, 0xFE82,
        0xFE82, 0xFE83, 0xFEA3, 0xFEA3, 0xFEC3, 0xFEC3, 0xFEC3, 0xFEE3, 0xFEE4,
        0xFEE4, 0xFF04, 0xFF04, 0xFF04, 0xFF25, 0xFF25, 0xFF25, 0xFF45, 0xFF46,
        0xFF46, 0xFF46, 0xFF67, 0xFF67, 0xFF67, 0xFF68, 0xFF68, 0xFF89, 0xFF89,
        0xFF89, 0xFF8A, 0xFF8A, 0xFFAB, 0xFFAB, 0xFFAC, 0xFFAC, 0xFFAD, 0xFFAD,
        0xFFAE, 0xFFCE, 0xFFCE, 0xFFCF, 0xFFD0, 0xFFD0, 0xFFD1, 0xFFD1, 0xFFD2,
        0xFFD2, 0xFFD3, 0xFFD3, 0xFFD4, 0xFFD4, 0xFFD5, 0xFFF5, 0xFFF6, 0xFFF6,
        0xFFF7, 0xFFF7, 0xFFF8, 0xFFF8, 0xFFF9, 0xFFF9, 0xFFF9, 0xFFFA, 0xFFFA,
        0xFFFB, 0xFFFB, 0xFFFC, 0xFFFC, 0xFFFC, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFE,
        0xFFFE, 0xFFFE, 0xFFFF, 0xFFFF,
    },
    {
        // colormap_rainbow
        0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x000A,
        0x002A, 0x002B, 0x004B, 0x006B, 0x008C, 0x00AC, 0x00CC, 0x00ED, 0x010D,
        0x010E, 0x012E, 0x014E, 0x014F, 0x0170, 0x0190, 0x0190, 0x0191, 0x01B1,
        0x01B1, 0x01B1, 0x01B2, 0x01D2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0214,
        0x0234, 0x0234, 0x0235, 0x0255, 0x0256, 0x0276, 0x0277, 0x0277, 0x0297,
        0x0297, 0x02B8, 0x02B8, 0x02D9, 0x02D9, 0x02F9, 0x02F9, 0x02FA, 0x02FA,
        0x031A, 0x031A, 0x031A, 0x033B, 0x033B, 0x035B, 0x035B, 0x035B, 0x037B,
        0x037B, 0x039B, 0x039B, 0x03BB, 0x03BB, 0x03DB, 0x03DB, 0x03FB, 0x03FA,
        0x041A, 0x0419, 0x0439, 0x0439, 0x0438, 0x0458, 0x0457, 0x0476, 0x0C75,
        0x0C94, 0x0C94, 0x0C93, 0x0C93, 0x0C92, 0x0CB1, 0x14B0, 0x14CF, 0x1CCE,
        0x1CED, 0x24EC, 0x2D0B, 0x2D0A, 0x3529, 0x3D28, 0x4547, 0x4D66, 0x4D66,
        0x5585, 0x5D84, 0x5DA4, 0x65A3, 0x6DC3, 0x6DC2, 0x75E2, 0x75E2, 0x7DE2,
        0x8601, 0x8601, 0x8E21, 0x8E21, 0x9620, 0x9640, 0x9E40, 0xA640, 0xA660,
        0xAE60, 0xAE60, 0xAE60, 0xB660, 0xBE80, 0xBE80, 0xC680, 0xC6A0, 0xC6A0,
        0xCEA0, 0xCEA0, 0xD6A0, 0xD6A0, 0xDEA0, 0xDEA0, 0xDEA0, 0xE6A0, 0xE6A0,
        0xE6A0, 0xE680, 0xEE80, 0xEE80, 0xEE80, 0xEE80, 0xEE80, 0xEE80, 0xEE81,
        0xEE61, 0xF661, 0xF641, 0xF641, 0xF641, 0xF641, 0xF621, 0xF621, 0xF621,
        0xFE01, 0xFDE1, 0xFDE1, 0xFDC1, 0xFDC2, 0xFDA2, 0xFD82, 0xFD62, 0xFD42,
        0xFD42, 0xFD22, 0xFD22, 0xFD02, 0xFD02, 0xFCE2, 0xFCE2, 0xFCC3, 0xFCA3,
        0xFC63, 0xFC43, 0xFC23, 0xFC03, 0xFBE4, 0xFBA4, 0xFB64, 0xFB44, 0xFB24,
        0xFB04, 0xFAE5, 0xFAC5, 0xFA85, 0xFA65, 0xFA25, 0xF9E6, 0xF9C6, 0xF9A6,
        0xF986, 0xF966, 0xF927, 0xF907, 0xF907, 0xF8E7, 0xF8E7, 0xF8E7, 0xF8E7,
        0xF8C7, 0xF8C8, 0xF8C8, 0xF8C8, 0xF8C8, 0xF8C9, 0xF0C9, 0xF0C9, 0xF0C9,
        0xF0CA, 0xF10A, 0xF10A, 0xF12A, 0xF14B, 0xF16B, 0xF18B, 0xF9AB, 0xF9CC,
        0xFA0C, 0xFA4C, 0xFA8D, 0xFAAD, 0xFAED, 0xFAED, 0xFB0D, 0xFB2D, 0xFB2E,
        0xFB2E, 0xFB6E, 0xFBAF, 0xFBCF, 0xFBEF, 0xFC10, 0xFC30, 0xFC50, 0xFC91,
        0xFCB1, 0xFCF2, 0xFD12, 0xFD52, 0xFD73, 0xFD93, 0xFD93, 0xFDD4, 0xFDF4,
        0xFE15, 0xFE35, 0xFE55, 0xFE76, 0xFE96, 0xFED7, 0xFED7, 0xFEF8, 0xFEF9,
        0xFF19, 0xFF19, 0xFF39, 0xFF5A,
    },
    {
        // colormap_grayscale
        0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0020, 0x0820, 0x0820, 0x0840,
        0x0840, 0x0841, 0x0841, 0x0861, 0x0861, 0x1061, 0x1061, 0x1081, 0x1081,
        0x1082, 0x1082, 0x10a2, 0x10a2, 0x18a2, 0x18a2, 0x18c2, 0x18c2, 0x18c3,
        0x18c3, 0x18e3, 0x18e3, 0x20e3, 0x20e3, 0x2103, 0x2103, 0x2104, 0x2104,
        0x2124, 0x2124, 0x2924, 0x2924, 0x2944, 0x2944, 0x2945, 0x2945, 0x2965,
        0x2965, 0x3165, 0x3165, 0x3185, 0x3185, 0x3186, 0x3186, 0x31a6, 0x31a6,
        0x39a6, 0x39a6, 0x39c6, 0x39c6, 0x39c7, 0x39c7, 0x39e7, 0x39e7, 0x41e7,
        0x41e7, 0x4207, 0x4207, 0x4208, 0x4208, 0x4228, 0x4228, 0x4a28, 0x4a28,
        0x4a48, 0x4a48, 0x4a49, 0x4a49, 0x4a69, 0x4a69, 0x5269, 0x5269, 0x5289,
        0x5289, 0x528a, 0x528a, 0x52aa, 0x52aa, 0x5aaa, 0x5aaa, 0x5aca, 0x5aca,
        0x5acb, 0x5acb, 0x5aeb, 0x5aeb, 0x62eb, 0x62eb, 0x630b, 0x630b, 0x630c,
        0x630c, 0x632c, 0x632c, 0x6b2c, 0x6b2c, 0x6b4c, 0x6b4c, 0x6b4d, 0x6b4d,
        0x6b6d, 0x6b6d, 0x736d, 0x736d, 0x738d, 0x738d, 0x738e, 0x738e, 0x73ae,
        0x73ae, 0x7bae, 0x7bae, 0x7bce, 0x7bce, 0x7bcf, 0x7bcf, 0x7bef, 0x7bef,
        0x83ef, 0x83ef, 0x840f, 0x840f, 0x8410, 0x8410, 0x8430, 0x8430, 0x8c30,
        0x8c30, 0x8c50, 0x8c50, 0x8c51, 0x8c51, 0x8c71, 0x8c71, 0x9471, 0x9471,
        0x9491, 0x9491, 0x9492, 0x9492, 0x94b2, 0x94b2, 0x9cb2, 0x9cb2, 0x9cd2,
        0x9cd2, 0x9cd3, 0x9cd3, 0x9cf3, 0x9cf3, 0xa4f3, 0xa4f3, 0xa513, 0xa513,
        0xa514, 0xa514, 0xa534, 0xa534, 0xad34, 0xad34, 0xad54, 0xad54, 0xad55,
        0xad55, 0xad75, 0xad75, 0xb575, 0xb575, 0xb595, 0xb595, 0xb596, 0xb596,
        0xb5b6, 0xb5b6, 0xbdb6, 0xbdb6, 0xbdd6, 0xbdd6, 0xbdd7, 0xbdd7, 0xbdf7,
        0xbdf7, 0xc5f7, 0xc5f7, 0xc617, 0xc617, 0xc618, 0xc618, 0xc638, 0xc638,
        0xce38, 0xce38, 0xce58, 0xce58, 0xce59, 0xce59, 0xce79, 0xce79, 0xd679,
        0xd679, 0xd699, 0xd699, 0xd69a, 0xd69a, 0xd6ba, 0xd6ba, 0xdeba, 0xdeba,
        0xdeda, 0xdeda, 0xdedb, 0xdedb, 0xdefb, 0xdefb, 0xe6fb, 0xe6fb, 0xe71b,
        0xe71b, 0xe71c, 0xe71c, 0xe73c, 0xe73c, 0xef3c, 0xef3c, 0xef5c, 0xef5c,
        0xef5d, 0xef5d, 0xef7d, 0xef7d, 0xf77d, 0xf77d, 0xf79d, 0xf79d, 0xf79e,
        0xf79e, 0xf7be, 0xf7be, 0xffbe, 0xffbe, 0xffde, 0xffde, 0xffdf, 0xffdf,
        0xffff, 0xffff, 0xffff, 0xffff,
    },
    {
        // colormap_ironblack
        0xFFFF, 0xFFFF, 0xFFDF, 0xFFDF, 0xF7BE, 0xF7BE, 0xF79E, 0xF79E, 0xEF7D,
        0xEF7D, 0xEF5D, 0xEF5D, 0xE73C, 0xE73C, 0xE71C, 0xE71C, 0xDEFB, 0xDEFB,
        0xDEDB, 0xDEDB, 0xD6BA, 0xD6BA, 0xD69A, 0xD69A, 0xCE79, 0xCE79, 0xCE59,
        0xCE59, 0xC638, 0xC638, 0xC618, 0xC618, 0xBDF7, 0xBDF7, 0xBDD7, 0xBDD7,
        0xB5B6, 0xB5B6, 0xB596, 0xB596, 0xAD75, 0xAD75, 0xAD55, 0xAD55, 0xA534,
        0xA534, 0xA514, 0xA514, 0x9CF3, 0x9CF3, 0x9CD3, 0x9CD3, 0x94B2, 0x94B2,
        0x9492, 0x9492, 0x8C71, 0x8C71, 0x8C51, 0x8C51, 0x8430, 0x8430, 0x8410,
        0x8410, 0x7BEF, 0x7BEF, 0x7BCF, 0x7BCF, 0x73AE, 0x73AE, 0x738E, 0x738E,
        0x6B6D, 0x6B6D, 0x6B4D, 0x6B4D, 0x632C, 0x632C, 0x630C, 0x630C, 0x5AEB,
        0x5AEB, 0x5ACB, 0x5ACB, 0x52AA, 0x52AA, 0x528A, 0x528A, 0x4A69, 0x4A69,
        0x4A49, 0x4A49, 0x4228, 0x4228, 0x4208, 0x4208, 0x39E7, 0x39E7, 0x39C7,
        0x39C7, 0x31A6, 0x31A6, 0x3186, 0x3186, 0x2965, 0x2965, 0x2945, 0x2945,
        0x2124, 0x2124, 0x2104, 0x2104, 0x18E3, 0x18E3, 0x18C3, 0x18C3, 0x10A2,
        0x10A2, 0x1082, 0x1082, 0x0861, 0x0861, 0x0841, 0x0841, 0x0020, 0x0020,
        0x0000, 0x0000, 0x0001, 0x0002, 0x0003, 0x0003, 0x0804, 0x0805, 0x0806,
        0x0807, 0x1008, 0x1009, 0x100A, 0x100B, 0x180C, 0x180C, 0x180D, 0x180E,
        0x200F, 0x280F, 0x280F, 0x300F, 0x380F, 0x380F, 0x400F, 0x400F, 0x4810,
        0x5010, 0x5010, 0x5810, 0x6010, 0x6010, 0x6810, 0x6810, 0x7011, 0x7811,
        0x7811, 0x8011, 0x8011, 0x8811, 0x9011, 0x9031, 0x9831, 0x9831, 0xA031,
        0xA031, 0xA831, 0xB031, 0xB031, 0xB831, 0xB851, 0xB870, 0xC08F, 0xC08F,
        0xC0AE, 0xC8CD, 0xC8ED, 0xC8EC, 0xC90B, 0xD12B, 0xD14A, 0xD14A, 0xD969,
        0xD988, 0xD9A8, 0xD9A7, 0xE1C6, 0xE1E5, 0xE205, 0xE205, 0xE224, 0xE244,
        0xE264, 0xE284, 0xE2A3, 0xEAC3, 0xEAE3, 0xEAE2, 0xEB02, 0xEB22, 0xEB41,
        0xEB61, 0xEB81, 0xF3A1, 0xF3A1, 0xF3C1, 0xF3E1, 0xF401, 0xF421, 0xF441,
        0xF461, 0xF481, 0xF4A1, 0xF4C1, 0xF4E1, 0xF501, 0xF501, 0xF521, 0xF541,
        0xFD61, 0xFD81, 0xFDA2, 0xFDC2, 0xFDE2, 0xFE02, 0xFE22, 0xFE22, 0xFE42,
        0xFE63, 0xFE83, 0xFEA3, 0xFEC3, 0xFEE3, 0xFF03, 0xFF04, 0xFF26, 0xFF28,
        0xFF4A, 0xFF4B, 0xFF6D, 0xFF6F, 0xFF91, 0xFF92, 0xFFB4, 0xFFB6, 0xFFD8,
        0xFFD9, 0xFFDB, 0xFFFD, 0xFFE3,
    },
    {
        // colormap_cam
        0x480F, 0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810,
        0x3010, 0x3010, 0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010,
        0x2010, 0x1810, 0x1810, 0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811,
        0x0811, 0x0811, 0x0011, 0x0011, 0x0011, 0x0011, 0x0011, 0x0031, 0x0031,
        0x0051, 0x0072, 0x0072, 0x0092, 0x00B2, 0x00B2, 0x00D2, 0x00F2, 0x00F2,
        0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192, 0x0192, 0x01B2, 0x01D2,
        0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273, 0x0293, 0x02B3,
        0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373, 0x0394,
        0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
        0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554,
        0x0574, 0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571,
        0x0591, 0x0591, 0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE,
        0x05AE, 0x05AD, 0x05AD, 0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB,
        0x05CA, 0x05CA, 0x05CA, 0x05C9, 0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7,
        0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5, 0x05E5, 0x0604, 0x0604, 0x0604,
        0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621, 0x0621, 0x0620, 0x0620,
        0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640, 0x1E40, 0x1E40,
        0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60, 0x3E60,
        0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
        0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0,
        0x8EA0, 0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0,
        0xB6E0, 0xB6E0, 0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0,
        0xD700, 0xDF00, 0xDEE0, 0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640,
        0xE620, 0xE600, 0xE5E0, 0xE5C0, 0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520,
        0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480, 0xE460, 0xEC40, 0xEC20, 0xEC00,
        0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40, 0xEB20, 0xEB00, 0xEAE0,
        0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200, 0xF1E0, 0xF1C0,
        0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0, 0xF080,
        0xF060, 0xF040, 0xF020, 0xF800,
    }};
static constexpr const size_t color_map_table_len =
    (sizeof(color_map_table) / sizeof(color_map_table[0]));
volatile size_t color_map_table_idx = 0;

// Add horizontal flip functionality
bool horizontal_flip = true;  // Set to true to flip horizontally

struct framedata_t {
    enum {
        median,
        average,
        highest,
        lowest,
    };
    uint16_t temp[4];
    bool subpage;
    uint16_t pixel_raw[frame_width * frame_height];
    uint8_t low_x;
    uint8_t low_y;
    uint8_t high_x;
    uint8_t high_y;
};
static constexpr size_t framedata_len = 4;
framedata_t framedata[framedata_len];

volatile int idx_recv = -1;

struct rect_t {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    constexpr rect_t(void) : x{0}, y{0}, w{0}, h{0} {
    }
    constexpr rect_t(int x_, int y_, int w_, int h_)
        : x{(uint16_t)x_}, y{(uint16_t)y_}, w{(uint16_t)w_}, h{(uint16_t)h_} {
    }
    inline constexpr int top(void) const {
        return y;
    }
    inline constexpr int left(void) const {
        return x;
    }
    inline constexpr int right(void) const {
        return x + w;
    }
    inline constexpr int bottom(void) const {
        return y + h;
    }
    inline constexpr bool empty(void) const {
        return 0 == (w * h);
    }

    inline bool operator==(const rect_t& src) const {
        return x == src.x && y == src.y && w == src.w && h == src.h;
    }
    inline bool operator!=(const rect_t& src) const {
        return !operator==(src);
    }
};

rect_t image_rect;
rect_t hist_rect;
rect_t text_rect;
rect_t graph_rect;

struct draw_param_t {
    LovyanGFX* gfx;
    const framedata_t* frame;
    const uint16_t* color_map = color_map_table[0];
    int32_t temp_lowest;
    int32_t temp_highest;
    int32_t temp_diff;
    uint8_t update_count = 0;
    uint8_t modify_count = 0;

    enum marker_mode_t {
        marker_mode_highest,
        marker_mode_lowest,
        marker_mode_center,
        marker_mode_max,
    };
    marker_mode_t marker_mode = marker_mode_t::marker_mode_highest;

    void setup(LovyanGFX* gfx_, framedata_t* frame_array_, int frameindex) {
        gfx = gfx_;
        for (int i = 0; i < 2; ++i) {
            _canvas[i].setFont(gfx_->getFont());
            _canvas[i].setPsram(false);
        }
        _frame_array = frame_array_;
        frame        = &frame_array_[frameindex];
        _lowest_value.set(frame->temp[frame->lowest]);
        _highest_value.set(frame->temp[frame->highest]);
        update(frameindex);
        _prev_frameindex = -1;
    }

    void setColorTable(const uint16_t* tbl) {
        color_map = tbl;
    }

    bool update(int frameindex) {
        bool result = _prev_frameindex != frameindex;
        if (result) {
            _prev_frameindex = frameindex;
            frame            = &_frame_array[frameindex];
            ++update_count;
        }
        int32_t lowest  = frame->temp[frame->lowest];
        int32_t highest = frame->temp[frame->highest];
        int32_t margin  = ((highest - lowest) >> 4) + 1;
        lowest          = _lowest_value.exec(lowest - margin, margin);
        highest         = _highest_value.exec(highest + margin, margin);
        if (temp_lowest != lowest || temp_highest != highest) {
            temp_lowest  = lowest;
            temp_highest = highest;
            temp_diff    = (highest - lowest) + 1;
            ++modify_count;
        }
        return result;
    }
    M5Canvas* getCanvas(int width, int height) {
        _canvas_idx = !_canvas_idx;
        auto result = &_canvas[_canvas_idx];
        if (result->width() != width || result->height() != height) {
            result->createSprite(width, height);
        }
        return result;
    }

    void markerChange(void) {
        auto m = marker_mode + 1;
        marker_mode =
            (marker_mode_t)((m >= marker_mode_t::marker_mode_max) ? 0 : m);
    }

   protected:
    struct value_smooth_t {
        int32_t exec(int32_t src, int32_t margin) {
            int32_t new_target = src << 8;
            if (abs(new_target - _target) > (margin << 8)) {
                _target = new_target;
            }
            int diff = _target - _current;

            _add = ((_add + diff + 2) >> 5);
            if (diff) {
                _add += diff < 0 ? -1 : 1;
            }
            _current += _add;

            return _current >> 8;
        }
        void set(int32_t default_value) {
            _target  = default_value << 8;
            _current = default_value << 8;
        }

       private:
        int _add = 0;
        int32_t _target;
        int32_t _current;
    };

    uint8_t _prev_frameindex;
    const framedata_t* _frame_array;
    M5Canvas _canvas[2];
    bool _canvas_idx = false;
    value_smooth_t _lowest_value;
    value_smooth_t _highest_value;
};

draw_param_t draw_param;

struct ui_base_t {
    bool isModified(draw_param_t* param) const {
        return _prev_update_count != param->update_count ||
               _prev_modify_count != param->modify_count;
    }

    virtual bool draw(draw_param_t* param) {
        bool res = _prev_update_count != param->update_count ||
                   _prev_modify_count != param->modify_count;

        _prev_update_count = param->update_count;
        _prev_modify_count = param->modify_count;

        return res;
    }

    void smooth_move(const rect_t& src) {
        if (_client_rect == src) return;
        invalidate();
        int b = _smooth_move_inner(src.bottom(), _client_rect.bottom());
        int y = _smooth_move_inner(src.y, _client_rect.y);
        _client_rect.h = b - y;
        _client_rect.y = y;
        int r          = _smooth_move_inner(src.right(), _client_rect.right());
        int x          = _smooth_move_inner(src.x, _client_rect.x);
        _client_rect.w = r - x;
        _client_rect.x = x;
    }
    inline bool isInvalidated(void) const {
        return _invalidate;
    }
    inline void invalidate(void) {
        _invalidate = true;
    }

   protected:
    static int _smooth_move_inner(int dst, int src) {
        return (dst == src) ? dst : ((dst + src + (src < dst ? 1 : 0)) >> 1);
    }
    inline void clearInvalidate(void) {
        _invalidate = false;
    }
    bool _invalidate;
    rect_t _client_rect;
    uint8_t _prev_update_count;
    uint8_t _prev_modify_count;
};

class image_ui_t : public ui_base_t {
    struct marker_t {
        int16_t mark_x = 64;
        int16_t mark_y = 64;

        int16_t text_x = 64;
        int16_t text_y = 64;

        uint16_t raw = UINT16_MAX;
        char text[8];
        bool update(int x_, int y_, int raw_, const rect_t& rect) {
            int mx = x_ * rect.w / (frame_width - 1);
            int my = y_ * rect.h / (frame_height - 1);

            int tx =
                (rect.w >> 1) + ((mx - (rect.w >> 1)) * (rect.w - 40) / rect.w);
            int ty = my + ((my * 2 < rect.h) ? 12 : -10);

            tx = (text_x * 3 + tx + (text_x < tx ? 3 : 0)) >> 2;
            ty = (text_y * 3 + ty + (text_y < ty ? 3 : 0)) >> 2;

            bool result =
                mark_x != mx || mark_y != my || text_x != tx || text_y != ty;
            if (result) {
                mark_x = mx;
                mark_y = my;
                text_x = tx;
                text_y = ty;
            }

            if (raw != raw_) {
                raw    = raw_;
                result = true;
                snprintf(text, sizeof(text), "%5.1f ",
                         M5_Thermal2::convertRawToCelsius(raw_));
            }
            return result;
        }
    };
    marker_t _marker;

   public:
    bool draw(draw_param_t* param) override {
        int mark_x = frame_width >> 1;
        int mark_y = frame_height >> 1;
        switch (param->marker_mode) {
            case param->marker_mode_lowest:
                mark_x = horizontal_flip ? (frame_width - 1 - param->frame->low_x) : param->frame->low_x;
                mark_y = param->frame->low_y;
                break;

            case param->marker_mode_highest:
                mark_x = horizontal_flip ? (frame_width - 1 - param->frame->high_x) : param->frame->high_x;
                mark_y = param->frame->high_y;
                break;

            default:
                break;
        }
        // Use original coordinates for data access, flipped coordinates for display
        int data_mark_x = horizontal_flip ? (frame_width - 1 - mark_x) : mark_x;
        if (_marker.update(
                mark_x, mark_y,
                param->frame->pixel_raw[data_mark_x + (frame_width * mark_y)],
                _client_rect)) {
            invalidate();
        }

        if (!isInvalidated() && !ui_base_t::draw(param)) return false;

        clearInvalidate();

        if (_client_rect.empty()) return false;
        int y1 = 0;
        for (int fy = 1; fy < frame_height; ++fy) {
            int y0        = y1;
            y1            = (fy * _client_rect.h) / (frame_height - 1);
            int boxHeight = y1 - y0;
            if (boxHeight == 0) continue;

            auto img = param->getCanvas(
                _client_rect.w, (_client_rect.h - 1) / (frame_height - 1) + 1);

            int v0;
            int fx_data_init = horizontal_flip ? (frame_width - 1) : 0;
            int v1 = ((param->frame->pixel_raw[fx_data_init + (fy - 1) * frame_width] -
                       param->temp_lowest)
                      << 16) /
                     boxHeight;
            int v2;
            int v3 = ((param->frame->pixel_raw[fx_data_init + (fy)*frame_width] -
                       param->temp_lowest)
                      << 16) /
                     boxHeight;

            int x1 = 0;
            for (int fx = 1; fx < frame_width; ++fx) {
                int x0       = x1;
                x1           = (fx * _client_rect.w) / (frame_width - 1);
                int boxWidth = x1 - x0;
                v0           = v1;
                int fx_data = horizontal_flip ? (frame_width - 1 - fx) : fx;
                v1 = ((param->frame->pixel_raw[fx_data + (fy - 1) * frame_width] -
                       param->temp_lowest)
                      << 16) /
                     boxHeight;
                v2 = v3;
                v3 = ((param->frame->pixel_raw[fx_data + (fy)*frame_width] -
                       param->temp_lowest)
                      << 16) /
                     boxHeight;
                if (boxWidth == 0) continue;
                int divider = boxWidth * param->temp_diff;

                for (int by = 0; by < boxHeight; ++by) {
                    int v02 = (v0 * (boxHeight - by) + v2 * by) / divider;
                    int v13 = (v1 * (boxHeight - by) + v3 * by) / divider;
                    auto img_buf =
                        &((m5gfx::swap565_t*)
                              img->getBuffer())[x0 + by * img->width()];
                    for (int bx = 0; bx < boxWidth; ++bx) {
                        int v       = (v02 * (boxWidth - bx) + v13 * bx) >> 8;
                        img_buf[bx] = m5gfx::getSwap16(
                            (param->color_map[(v < 0)     ? 0
                                              : (v > 255) ? 255
                                                          : v]));
                    }
                }
            }

            if (abs((y0 + y1) - (_marker.mark_y * 2)) < 20) {
                img->setColor(abs(15 - (int)(31 & param->update_count)) *
                              0x0F0F0Fu);
                int x = _marker.mark_x;
                int y = _marker.mark_y - y0;
                img->drawCircle(x, y, 4);
                img->drawFastVLine(x, y - 6, 13);
                img->drawFastHLine(x - 6, y, 13);
            }

            if (abs((y0 + y1) - (_marker.text_y * 2)) < 20) {
                img->setTextSize(1);
                img->setTextDatum(textdatum_t::middle_center);
                img->setTextColor(TFT_BLACK);
                int x = _marker.text_x;
                int y = _marker.text_y - y0;
                img->drawString(_marker.text, x - 1, y);
                img->drawString(_marker.text, x + 1, y);
                img->drawString(_marker.text, x, y - 1);
                img->drawString(_marker.text, x, y + 1);
                img->setTextColor(TFT_WHITE);
                img->drawString(_marker.text, x, y);
            }
            param->gfx->setClipRect(_client_rect.x, _client_rect.y + y0,
                                    _client_rect.w, boxHeight);
            img->pushSprite(param->gfx, _client_rect.x, _client_rect.y + y0);
        }
        return true;
    };
};

class graph_ui_t : public ui_base_t {
    uint16_t _range_lowest;
    uint16_t _range_highest;
    uint16_t _current_index   = 0;
    uint16_t _temp_arrays_len = 0;
    uint16_t* _temp_arrays[4];
    int xs = 0;

   public:
    void setup(draw_param_t* param) {
        _temp_arrays_len = param->gfx->width() + 2;
        for (int i = 0; i < 4; ++i) {
            _temp_arrays[i] =
                (uint16_t*)malloc(_temp_arrays_len * sizeof(uint16_t));
            auto tmp = param->frame->temp[i];
            for (int j = 0; j < _temp_arrays_len; ++j) {
                _temp_arrays[i][j] = tmp;
            }
        }
        _range_lowest  = param->frame->temp[framedata_t::lowest];
        _range_highest = param->frame->temp[framedata_t::highest];
    }

    void update(draw_param_t* param) {
        xs = (xs + 1) & 15;
        if (++_current_index >= _temp_arrays_len) {
            _current_index = 0;
        }
        for (int i = 0; i < 4; ++i) {
            auto tmp                        = param->frame->temp[i];
            _temp_arrays[i][_current_index] = tmp;
        }
        invalidate();
    }

    bool draw(draw_param_t* param) override {
        if (_client_rect.empty()) return false;

        if (!isInvalidated() && !ui_base_t::draw(param)) return false;
        clearInvalidate();

        {  // Obtain the maximum and minimum values within the displayed range.
            uint_fast16_t t0 = UINT16_MAX;
            uint_fast16_t t1 = 0;
            int idx = (_current_index + _temp_arrays_len - _client_rect.w) %
                      _temp_arrays_len;
            for (int i = 0; i < _client_rect.w; ++i) {
                auto lowest = _temp_arrays[framedata_t::lowest][idx];
                if (t0 > lowest) {
                    t0 = lowest;
                }
                auto highest = _temp_arrays[framedata_t::highest][idx];
                if (t1 < highest) {
                    t1 = highest;
                }
                if (++idx == _temp_arrays_len) {
                    idx = 0;
                }
            }
            int diff       = (t1 - t0) >> 3;
            _range_lowest  = (_range_lowest * 3 + t0 - diff) >> 2;
            _range_highest = (_range_highest * 3 + t1 + diff + 3) >> 2;
        }
        int32_t graph_temp_diff = _range_highest - _range_lowest + 1;

        int step_index = 0;
        while ((graph_temp_diff >> 3) >
               _client_rect.h * step_table[step_index]) {
            ++step_index;
        }
        int step_raw = step_table[step_index] * 128;

        param->gfx->setClipRect(_client_rect.x, _client_rect.y, _client_rect.w,
                                _client_rect.h);
        int y_div       = ((_client_rect.w * _client_rect.h - 1) >> 13) + 1;
        int blockHeight = (_client_rect.h - 1) / y_div + 1;

        int yend = 0;
        for (int fy = 0; fy < y_div; ++fy) {
            int ystart = yend;
            yend += blockHeight;

            auto img = param->getCanvas(_client_rect.w, blockHeight);
            img->setTextDatum(textdatum_t::bottom_right);
            img->setTextSize(1);
            int fontHeight = img->fontHeight();

            int32_t raw =
                ((_client_rect.h - ystart) * graph_temp_diff / _client_rect.h) +
                _range_lowest;
            int32_t line_idx = (raw - raw_zero) / step_raw;
            for (int y = ystart; y < yend + fontHeight; ++y) {
                int32_t prev_raw = line_idx;
                int32_t i        = _client_rect.h - (y + 1);
                raw   = (i * graph_temp_diff / _client_rect.h) + _range_lowest;
                int v = ((raw - param->temp_lowest) << 8) / param->temp_diff;
                line_idx       = (raw - raw_zero) / step_raw;
                v              = (v < 0) ? 0 : (v > 255) ? 255 : v;
                uint16_t color = param->color_map[v];

                uint16_t bgcolor = (color >> 2) & 0x39E7;
                if (prev_raw == line_idx) {
                    if (i >= 0) {
                        img->drawFastHLine(0, y - ystart, img->width(),
                                           bgcolor);
                        img->setColor(bgcolor + 0x2104);
                        for (int x = img->width() - xs; x >= 0; x -= 16) {
                            img->drawPixel(x, y - ystart);
                        }
                    }
                } else {
                    int gauge_value = M5_Thermal2::convertRawToCelsius(
                        prev_raw * step_raw + raw_zero);
                    img->setTextColor(((color >> 1) & 0x7BEF) + 0x630C);
                    img->drawNumber(gauge_value, img->width(), y - ystart);
                    if (i >= 0) {
                        img->drawFastHLine(0, y - ystart, img->width(),
                                           bgcolor + 0x2104);
                    }
                }
            }
            for (int i = 0; i < 4; ++i) {
                int idx = (_current_index + _temp_arrays_len - _client_rect.w) %
                          _temp_arrays_len;
                img->setColor(graph_color_table[i]);
                int y = _client_rect.h -
                        (1 + (int32_t)((_temp_arrays[i][idx] - _range_lowest) *
                                       _client_rect.h) /
                                 graph_temp_diff);
                for (int gi = 0; gi < _client_rect.w; ++gi) {
                    int prev_idx = idx;
                    if (++idx >= _temp_arrays_len) idx -= _temp_arrays_len;
                    int prev_y = y;
                    y          = _client_rect.h -
                        (1 + (int32_t)((_temp_arrays[i][idx] - _range_lowest) *
                                       _client_rect.h) /
                                 graph_temp_diff);
                    int y0 = (y < prev_y) ? y : prev_y;
                    int y1 = (y > prev_y) ? y : prev_y;
                    img->fillRect(gi, y0 - ystart, 1, y1 - y0 + 1);
                }
            }
            img->pushSprite(param->gfx, _client_rect.x,
                            _client_rect.y + ystart);
        }
        return true;
    }
};

class text_ui_t : public ui_base_t {
    static constexpr int _text_count = 4;
    char _value_text[_text_count][8];
    int _value_x10[_text_count];
    int16_t _text_width = 0;
    bool _text_mod[_text_count];
    bool _two_line    = false;
    float _textsize_x = 1.0f;
    float _textsize_y = 1.0f;

   public:
    void update(draw_param_t* param) {
        // if (param->modified) {
        if (isModified(param)) {
            for (int i = 0; i < _text_count; ++i) {
                int tmp =
                    thermal2.convertRawToCelsius(param->frame->temp[i]) * 10;
                bool mod     = (_value_x10[i] != tmp);
                _text_mod[i] = mod;
                if (mod) {
                    _value_x10[i] = tmp;
                    snprintf(_value_text[i], sizeof(_value_text[0]), "%5.1f",
                             (float)tmp / 10);
                }
            }
        }
    }
    bool draw(draw_param_t* param) override {
        if (_client_rect.empty()) return false;
        if (!isInvalidated() && !ui_base_t::draw(param)) return false;

        param->gfx->setClipRect(_client_rect.x, _client_rect.y, _client_rect.w,
                                _client_rect.h);
        bool draw_title = false;
        if (isInvalidated()) {
            draw_title = true;
            param->gfx->setTextSize(1);
            size_t tw1 = param->gfx->textWidth("High");
            size_t tw2 = param->gfx->textWidth("888.8");
            float sw1  = (float)_client_rect.w / (tw1 + tw2);
            float sw2  = (float)_client_rect.w / tw2;
            float sh   = (float)_client_rect.h / (param->gfx->fontHeight() * 8);
            _two_line  = (sw1 < 1.0f) && (sh > 1.0f);
            _text_width = _client_rect.w;
            if (_two_line) {
                _textsize_x = sw2 < 1.0f ? 1.0f : sw2;
                _textsize_y = sh;
            } else {
                if (sw1 < 1.0f) {
                    draw_title = false;
                    sw1        = sw2;
                } else {
                    _text_width = tw2 * sw1;
                }
                _textsize_x = sw1 < 1.0f ? 1.0f : sw1;
                _textsize_y = sh * 2;
            }
        }

        for (int i = 0; i < _text_count; ++i) {
            int y0 = _client_rect.h * (i) / _text_count;
            int y1 = _client_rect.h * (i + 1) / _text_count;
            param->gfx->setClipRect(_client_rect.x, _client_rect.y + y0,
                                    _client_rect.w, y1 - y0);
            if (draw_title) {
                auto img = param->getCanvas(
                    _client_rect.w / (2 - _two_line),
                    (_client_rect.h - 1) / (_text_count * (1 + _two_line)) + 1);
                img->setTextSize(_textsize_x, _textsize_y);
                img->setTextColor(graph_color_table[i], 0);
                img->setTextPadding(img->width());
                img->setTextDatum(textdatum_t::top_left);
                auto tw = img->textWidth(graph_text_table[i]);
                img->drawString(graph_text_table[i], 1, 0);
                img->pushSprite(
                    param->gfx, _client_rect.x,
                    _client_rect.y + (_client_rect.h * i / _text_count));
            }

            if (!_text_mod[i] && !isInvalidated()) continue;
            _text_mod[i] = false;

            auto img = param->getCanvas(
                _text_width,
                (_client_rect.h - 1) / (_text_count * (1 + _two_line)) + 1);
            img->setTextSize(_textsize_x, _textsize_y);
            img->setTextColor(graph_color_table[i], 0);
            img->setTextPadding(img->width());
            img->setTextDatum(textdatum_t::top_right);
            auto tw = img->textWidth(_value_text[i]);
            img->drawString(_value_text[i], img->width(), 0);
            if (_two_line) y0 = (y1 + y0) / 2;
            img->pushSprite(param->gfx, _client_rect.right() - img->width(),
                            _client_rect.y + y0);
        }
        clearInvalidate();
        return true;
    }
};

class hist_ui_t : public ui_base_t {
    uint16_t* _histgram    = nullptr;
    uint16_t* _prev_hist_x = nullptr;
    bool* _prev_hist_line  = nullptr;

   public:
    bool draw(draw_param_t* param) override {
        if (_client_rect.empty()) return false;
        if (!isInvalidated() && !ui_base_t::draw(param)) return false;
        if (isInvalidated()) {
            int hist_len = _client_rect.h;
            if (_histgram) {
                free(_histgram);
            }

            _histgram =
                (typeof(_histgram))malloc(hist_len * sizeof(_histgram[0]));
            memset(_histgram, 0, hist_len * sizeof(_histgram[0]));
            if (_prev_hist_x) {
                free(_prev_hist_x);
            }
            _prev_hist_x = (typeof(_prev_hist_x))malloc(
                hist_len * sizeof(_prev_hist_x[0]));
            memset(_prev_hist_x, 0, hist_len * sizeof(_prev_hist_x[0]));
            if (_prev_hist_line) {
                free(_prev_hist_line);
            }
            _prev_hist_line = (typeof(_prev_hist_line))malloc(
                hist_len * sizeof(_prev_hist_line[0]));
            memset(_prev_hist_line, 0, hist_len * sizeof(_prev_hist_line[0]));
        }

        // Histogram Aggregation.
        int hist_max = _client_rect.h - 1;
        for (int idx = 0; idx < frame_width * frame_height; ++idx) {
            int hist_idx = (param->frame->pixel_raw[idx] - param->temp_lowest) *
                           _client_rect.h / param->temp_diff;
            ++_histgram[(hist_idx < 0)
                            ? 0
                            : ((hist_idx > hist_max) ? hist_max : hist_idx)];
        }
        int step_index = 0;
        while ((param->temp_diff >> 3) >
               _client_rect.h * step_table[step_index]) {
            ++step_index;
        }
        int step_raw = step_table[step_index] * 128;

        int hist_text_w = param->gfx->fontWidth() * 3;
        auto img        = param->getCanvas(hist_text_w, _client_rect.h);
        img->setTextDatum(textdatum_t::bottom_right);
        img->setTextSize(1);

        int x_offset = (_client_rect.w >> 5) + 3;
        if (isInvalidated()) {
            param->gfx->setClipRect(_client_rect.x, _client_rect.y,
                                    _client_rect.w - img->width(),
                                    _client_rect.h);
            int x = _client_rect.x;
            for (int i = 0; i < _client_rect.h; ++i) {
                auto color       = param->color_map[(i * 255) / _client_rect.h];
                uint16_t bgcolor = (color >> 2) & 0x39E7;
                int y            = _client_rect.y + _client_rect.h - (i + 1);
                param->gfx->drawFastHLine(x, y, x_offset, bgcolor);
                param->gfx->drawFastHLine(x + 1, y, x_offset - 2, color);
            }
            memset(_prev_hist_x, 0, _client_rect.h);
            memset(_prev_hist_line, 0, _client_rect.h);
        }
        param->gfx->setClipRect(_client_rect.x + x_offset, _client_rect.y,
                                _client_rect.w - (x_offset + img->width()),
                                _client_rect.h);

        int raw = (param->temp_highest - raw_zero) / step_raw;
        for (int y = 0; y < _client_rect.h + 14; ++y) {
            int i        = _client_rect.h - (y + 1);
            int prev_raw = raw;
            raw          = ((i * param->temp_diff / _client_rect.h) +
                   param->temp_lowest - raw_zero) /
                  step_raw;
            bool drawline = (prev_raw != raw);
            uint16_t color =
                param->color_map[((i < 0 ? 0 : i) << 8) / (_client_rect.h + 1)];
            if (drawline) {
                int gauge_value = M5_Thermal2::convertRawToCelsius(
                    prev_raw * step_raw + raw_zero);
                img->setTextColor(((color >> 1) & 0x7BEF) + 0x630C);
                img->drawNumber(gauge_value, img->width(), y);
            }

            if (i >= 0) {
                int x =
                    ((_histgram[i] * _client_rect.w * _client_rect.h) >> 13);
                _histgram[i] = 0;
                x = (x < 0) ? 0 : (x > _client_rect.w) ? _client_rect.w : x;
                int px          = _prev_hist_x[i];
                _prev_hist_x[i] = x;

                uint16_t bgcolor = (color >> 2) & 0x39E7;
                if (drawline) {
                    bgcolor += 0x2104;
                }
                if (_prev_hist_line[i] != drawline || isInvalidated()) {
                    _prev_hist_line[i] = drawline;
                    param->gfx->fillRect(
                        _client_rect.x + x + x_offset, _client_rect.y + y,
                        _client_rect.w - img->width(), 1, bgcolor);
                    px = 0;
                }
                param->gfx->fillRect(_client_rect.x + x + x_offset,
                                     _client_rect.y + y, px - x, 1,
                                     x > px ? color : bgcolor);
                x = x + x_offset + hist_text_w - _client_rect.w;
                if (x < 0) {
                    x = 0;
                } else {
                    img->drawFastHLine(0, y, x, color);
                }
                img->drawFastHLine(x, y, img->width(), bgcolor);
            }
        }
        param->gfx->setClipRect(_client_rect.x + x_offset, _client_rect.y,
                                _client_rect.w - x_offset, _client_rect.h);
        img->pushSprite(param->gfx,
                        _client_rect.x + _client_rect.w - hist_text_w,
                        _client_rect.y);
        clearInvalidate();
        return true;
    }
};

void drawTask(void*) {
    text_ui_t text_ui;
    hist_ui_t hist_ui;
    image_ui_t image_ui;
    graph_ui_t graph_ui;

    ui_base_t* ui_list[4] = {&text_ui, &hist_ui, &image_ui, &graph_ui};

    if (display.width() >= 240) {
        display.setFont(&fonts::DejaVu12);
    }

    do {
        vTaskDelay(1);
    } while (idx_recv < 2);

    draw_param.setup(&display, framedata, 2);
    draw_param.setColorTable(color_map_table[0]);
    graph_ui.setup(&draw_param);

    uint8_t prev_color_table_idx = 0;
    uint8_t prev_sec             = UINT8_MAX;
    uint32_t prev_msec           = UINT32_MAX;
    uint32_t framecount          = 0;

    display.startWrite();
    for (;;) {
        ++framecount;
        uint32_t msec = millis();
        uint8_t sec   = msec / 1000;
        if (prev_sec != sec) {
            prev_sec = sec;
            Serial.printf("fps: %d : %d\n", framecount,
                          draw_param.update_count);
            framecount              = 0;
            draw_param.update_count = 0;
            delay(1);
        }
        int limit_delay = 17 - (msec - prev_msec);
        prev_msec       = msec;
        if (limit_delay > 0) {
            delay(limit_delay);
        }

        if (prev_color_table_idx != color_map_table_idx) {
            prev_color_table_idx = color_map_table_idx;
            draw_param.setColorTable(color_map_table[prev_color_table_idx]);
            image_ui.invalidate();
            graph_ui.invalidate();
            hist_ui.invalidate();
        }

        graph_ui.smooth_move(graph_rect);
        image_ui.smooth_move(image_rect);
        text_ui.smooth_move(text_rect);
        hist_ui.smooth_move(hist_rect);

        int drawcount = 0;
        for (int i = 0; i < 4; ++i) {
            if (draw_param.update(idx_recv)) {
                graph_ui.update(&draw_param);
                text_ui.update(&draw_param);
            }
            drawcount += ui_list[i]->draw(&draw_param);
        }
        if (drawcount) {
            display.display();
        }
    }
    display.endWrite();
}

void layoutChange(void) {
    int disp_w = display.width();
    int disp_h = display.height();

    int image_w = disp_w >> 1;

    static uint8_t prev_layout_idx = 0;

    switch (++prev_layout_idx) {
        default:
            break;
        case 2:
            image_w = disp_w * 3 / 4;
            break;

        case 3:
            image_w = disp_w;
    }
    int image_h = (image_w * 3 + 3) >> 2;
    if (image_h > disp_h) {
        image_h = disp_h;
        image_w = disp_h * 4 / 3;
    }
    image_rect = {0, 0, image_w, image_h};

    int text_h = display.fontHeight() * 4;
    if (text_h < disp_h - image_h) {
        text_h = disp_h - image_h;
    }

    switch (prev_layout_idx) {
        case 1:
            hist_rect  = {image_rect.right(), 0, disp_w - image_rect.right(),
                         disp_h - text_h};
            text_rect  = {hist_rect.x, hist_rect.bottom(), hist_rect.w, text_h};
            graph_rect = {image_rect.left(), image_rect.bottom(), image_rect.w,
                          disp_h - image_rect.bottom()};
            break;

        case 2:
            hist_rect = {image_rect.w, 0, disp_w - image_rect.w,
                         disp_h - text_h};
            if (image_rect.bottom() >= disp_h) {
                int tw     = (disp_w - image_rect.w) >> 1;
                graph_rect = {image_rect.w, hist_rect.bottom(), tw, text_h};
            } else {
                graph_rect = {image_rect.x, image_rect.bottom(), image_rect.w,
                              disp_h - image_rect.bottom()};
            }
            text_rect = {graph_rect.right(), hist_rect.bottom(),
                         disp_w - graph_rect.right(), text_h};
            break;

        default:
            prev_layout_idx = 0;
            hist_rect = {image_rect.right(), 0, disp_w - image_rect.right(),
                         disp_h - text_h};
            text_rect = {disp_w, disp_h, 0, 0};
            if (image_rect.right() >= disp_w) {
                graph_rect = {image_rect.left(), image_rect.bottom(),
                              image_rect.w, disp_h - image_rect.bottom()};
            } else {
                graph_rect = {hist_rect.left(), hist_rect.bottom(), hist_rect.w,
                              text_h};
            }
            break;
    }
}

void setup(void) {
    M5.begin();
    if (display.width() < display.height()) {
        display.setRotation(display.getRotation() ^ 1);
    }
    display.print("M5_Thermal2 demo.\nPress the Thermal 2 reset button.");

    M5.Ex_I2C.begin();
    delay(500);
    if (!thermal2.begin()) {
        do {
            M5.Ex_I2C.release();
            delay(50);
            M5.Ex_I2C.begin();
            display.print(".");
        } while (!thermal2.begin());
    }

    // 0=1/2Hz  1=1Hz  2=2Hz  3=4Hz  4=8Hz  5=16Hz  6=32Hz  7=64Hz
    thermal2.setRefreshRate(6);

    // If set to 64Hz, increase I2C freq
    // thermal2.setRefreshRate(7);
    // thermal2.setI2CFreq(400000, 700000);

    // Noise filter strength can be specified from 0~15.
    thermal2.setNoiseFilterLevel(8);

    thermal2.ledOff();
    thermal2.buzzerOff();
    thermal2.alarmOff();
    thermal2.setMonitorArea(15, 11);

    layoutChange();

    xTaskCreatePinnedToCore(drawTask, "drawTask", 8192, nullptr, 1, nullptr,
                            PRO_CPU_NUM);
}

void loop(void) {
    if (thermal2.btnWasClicked()) {
        thermal2.setNoiseFilterLevel(thermal2.getNoiseFilterLevel() ? 0 : 8);
    }

    M5.update();
    bool color_change  = M5.BtnA.wasClicked();
    bool marker_change = M5.BtnB.wasClicked();
    bool layout_change = M5.BtnC.wasClicked() || M5.BtnA.wasHold();
    bool flip_change   = M5.BtnB.wasHold() || M5.BtnC.wasHold();

    if (M5.Touch.isEnabled()) {
        auto dt = M5.Touch.getDetail();
        if (dt.y < display.height() && dt.wasClicked()) {
            switch (dt.x * 3 / display.width()) {
                case 0:
                    color_change = true;
                    break;
                case 1:
                    marker_change = true;
                    break;
                default:
                    layout_change = true;
                    break;
            }
        }
    }
    if (color_change) {
        color_map_table_idx = (color_map_table_idx + 1) % color_map_table_len;
    }
    if (layout_change) {
        layoutChange();
    }
    if (marker_change) {
        draw_param.markerChange();
    }
    if (flip_change) {
        horizontal_flip = !horizontal_flip;
    }

    if (!thermal2.update()) {
        delay(1);
    } else {
        int idx_recv_next = (idx_recv + 1) & 3;
        auto frame        = &framedata[idx_recv_next];
        auto prev_frame   = &framedata[idx_recv & 3];
        memcpy(frame, prev_frame, sizeof(framedata_t));
        // Obtain temperature data structure.
        auto temp_data = thermal2.getTemperatureData();
        {
            static uint16_t prev_lowest = 0;
            auto lowest                 = temp_data.getLowestRaw();
            frame->temp[frame->lowest]  = (prev_lowest + lowest) >> 1;

            if (prev_lowest > lowest) {
                frame->low_x = temp_data.getLowestX();
                frame->low_y = temp_data.getLowestY();
            }
            prev_lowest = lowest;

            static uint16_t prev_highest = UINT16_MAX;
            auto highest                 = temp_data.getHighestRaw();
            frame->temp[frame->highest]  = (prev_highest + highest + 1) >> 1;
            if (prev_highest < highest) {
                frame->high_x = temp_data.getHighestX();
                frame->high_y = temp_data.getHighestY();
            }
            prev_highest = highest;

            static uint16_t prev_median = INT16_MAX;
            auto median                 = temp_data.getMedianRaw();
            frame->temp[frame->median]  = (prev_median + median) >> 1;
            prev_median                 = median;

            static uint16_t prev_average = INT16_MAX;
            auto average                 = temp_data.getAverageRaw();
            frame->temp[frame->average]  = (prev_average + average) >> 1;
            prev_average                 = average;
        }

        uint16_t diff[384];
        // Pixel data is held in an array. Array size is 384. (16x24)
        bool subpage   = temp_data.getSubPage();
        frame->subpage = subpage;
        for (int idx = 0; idx < 384; ++idx) {
            uint_fast8_t y       = idx >> 4;
            uint_fast8_t x       = ((idx & 15) << 1) + ((y & 1) != subpage);
            uint_fast16_t xy     = x + y * frame_width;
            int32_t raw          = temp_data.getPixelRaw(idx);
            diff[idx]            = abs(raw - (int32_t)frame->pixel_raw[xy]);
            frame->pixel_raw[xy] = raw;
        }

        // Interpolation is performed from surrounding pixels where the
        // temperature change is large. (Areas with little temperature change
        // inherit values from the previous frame.)
        for (int idx = 0; idx < 384; ++idx) {
            uint_fast8_t y   = idx >> 4;
            uint_fast8_t x   = ((idx & 15) << 1) + ((y & 1) == subpage);
            uint_fast16_t xy = x + y * frame_width;

            uint32_t sum = 0;
            size_t count = 0;
            if (x > 0) {
                ++count;
                sum += diff[(xy - 1) >> 1];
            }
            if (x < (frame_width - 1)) {
                ++count;
                sum += diff[(xy + 1) >> 1];
            }
            if (y > 0) {
                ++count;
                sum += diff[(xy - frame_width) >> 1];
            }
            if (y < (frame_height - 1)) {
                ++count;
                sum += diff[(xy + frame_width) >> 1];
            }
            if (sum >= (count << 7)) {
                sum = 0;
                if (x > 0) {
                    sum += frame->pixel_raw[xy - 1];
                }
                if (x < (frame_width - 1)) {
                    sum += frame->pixel_raw[xy + 1];
                }
                if (y > 0) {
                    sum += frame->pixel_raw[xy - frame_width];
                }
                if (y < (frame_height - 1)) {
                    sum += frame->pixel_raw[xy + frame_width];
                }
                frame->pixel_raw[xy] = (sum + (count >> 1)) / count;
            }
        }

        idx_recv = idx_recv_next;
    }
}
