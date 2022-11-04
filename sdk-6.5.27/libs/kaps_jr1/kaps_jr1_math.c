

#include <kaps_jr1_math.h>

static const uint16_t crc32_table[256] = {
    0x0000, 0x00af, 0x015e, 0x01f1, 0x02bc, 0x0213, 0x03e2, 0x034d,
    0x0578, 0x05d7, 0x0426, 0x0489, 0x07c4, 0x076b, 0x069a, 0x0635,
    0x0af0, 0x0a5f, 0x0bae, 0x0b01, 0x084c, 0x08e3, 0x0912, 0x09bd,
    0x0f88, 0x0f27, 0x0ed6, 0x0e79, 0x0d34, 0x0d9b, 0x0c6a, 0x0cc5,
    0x15e0, 0x154f, 0x14be, 0x1411, 0x175c, 0x17f3, 0x1602, 0x16ad,
    0x1098, 0x1037, 0x11c6, 0x1169, 0x1224, 0x128b, 0x137a, 0x13d5,
    0x1f10, 0x1fbf, 0x1e4e, 0x1ee1, 0x1dac, 0x1d03, 0x1cf2, 0x1c5d,
    0x1a68, 0x1ac7, 0x1b36, 0x1b99, 0x18d4, 0x187b, 0x198a, 0x1925,
    0x2bc0, 0x2b6f, 0x2a9e, 0x2a31, 0x297c, 0x29d3, 0x2822, 0x288d,
    0x2eb8, 0x2e17, 0x2fe6, 0x2f49, 0x2c04, 0x2cab, 0x2d5a, 0x2df5,
    0x2130, 0x219f, 0x206e, 0x20c1, 0x238c, 0x2323, 0x22d2, 0x227d,
    0x2448, 0x24e7, 0x2516, 0x25b9, 0x26f4, 0x265b, 0x27aa, 0x2705,
    0x3e20, 0x3e8f, 0x3f7e, 0x3fd1, 0x3c9c, 0x3c33, 0x3dc2, 0x3d6d,
    0x3b58, 0x3bf7, 0x3a06, 0x3aa9, 0x39e4, 0x394b, 0x38ba, 0x3815,
    0x34d0, 0x347f, 0x358e, 0x3521, 0x366c, 0x36c3, 0x3732, 0x379d,
    0x31a8, 0x3107, 0x30f6, 0x3059, 0x3314, 0x33bb, 0x324a, 0x32e5,
    0x5780, 0x572f, 0x56de, 0x5671, 0x553c, 0x5593, 0x5462, 0x54cd,
    0x52f8, 0x5257, 0x53a6, 0x5309, 0x5044, 0x50eb, 0x511a, 0x51b5,
    0x5d70, 0x5ddf, 0x5c2e, 0x5c81, 0x5fcc, 0x5f63, 0x5e92, 0x5e3d,
    0x5808, 0x58a7, 0x5956, 0x59f9, 0x5ab4, 0x5a1b, 0x5bea, 0x5b45,
    0x4260, 0x42cf, 0x433e, 0x4391, 0x40dc, 0x4073, 0x4182, 0x412d,
    0x4718, 0x47b7, 0x4646, 0x46e9, 0x45a4, 0x450b, 0x44fa, 0x4455,
    0x4890, 0x483f, 0x49ce, 0x4961, 0x4a2c, 0x4a83, 0x4b72, 0x4bdd,
    0x4de8, 0x4d47, 0x4cb6, 0x4c19, 0x4f54, 0x4ffb, 0x4e0a, 0x4ea5,
    0x7c40, 0x7cef, 0x7d1e, 0x7db1, 0x7efc, 0x7e53, 0x7fa2, 0x7f0d,
    0x7938, 0x7997, 0x7866, 0x78c9, 0x7b84, 0x7b2b, 0x7ada, 0x7a75,
    0x76b0, 0x761f, 0x77ee, 0x7741, 0x740c, 0x74a3, 0x7552, 0x75fd,
    0x73c8, 0x7367, 0x7296, 0x7239, 0x7174, 0x71db, 0x702a, 0x7085,
    0x69a0, 0x690f, 0x68fe, 0x6851, 0x6b1c, 0x6bb3, 0x6a42, 0x6aed,
    0x6cd8, 0x6c77, 0x6d86, 0x6d29, 0x6e64, 0x6ecb, 0x6f3a, 0x6f95,
    0x6350, 0x63ff, 0x620e, 0x62a1, 0x61ec, 0x6143, 0x60b2, 0x601d,
    0x6628, 0x6687, 0x6776, 0x67d9, 0x6494, 0x643b, 0x65ca, 0x6565
};

#define CRC32_HIBYTE(x) ((x) >> 24)



#define CRC10_POLYNOMIAL 0x233
#define CRC10_MASK       0x3FF
#define CRC10_WIDTH      10
#define CRC24_POLYNOMIAL 0x328b63
#define CRC24_MASK       0xFFFFFF

static uint32_t g_crc10_table[256];

void
kaps_jr1_compute_crc10_table(
    void)
{
    uint32_t remainder = 0;
    uint32_t dividend = 0;
    uint32_t iter = 0;
    uint32_t width = CRC10_WIDTH;
    uint32_t topBit = 0;

    
    for (dividend = 0; dividend < 256; ++dividend)
    {
        
        remainder = dividend << (width - 8);

        for (iter = 8; iter > 0; --iter)
        {
            topBit = (1 << (width - 1));

            if (remainder & topBit)
            {
                remainder = (remainder << 1) ^ CRC10_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        g_crc10_table[dividend] = remainder & CRC10_MASK;
    }

}

uint32_t
kaps_jr1_fast_crc10(
    uint8_t * data_p,
    uint32_t numBytes)
{
    uint32_t remainder = 0;
    uint32_t i = 0;
    uint32_t width = CRC10_WIDTH;
    uint32_t rightShift = width - 8;
    uint8_t t = 0;

    for (i = 0; i < numBytes; ++i)
    {
        t = (uint8_t) ((remainder >> rightShift) & 0xFF);
        remainder = (remainder << 8) | *data_p++;
        remainder = remainder ^ g_crc10_table[t];
    }

    remainder = remainder & CRC10_MASK;

    return remainder;
}

static void
int_to_bin_digit(
    uint64_t in,
    uint8_t * out)
{
    uint64_t mask = 1ULL << 63;
    int32_t i;

    for (i = 0; i < 64; i++)
    {
        out[63 - i] = (in & mask) ? 1 : 0;
        in <<= 1;
    }
}

void
kaps_jr1_crc24(
    uint64_t data,
    int32_t reset_crc,
    uint32_t * crcreg)
{
    uint8_t msb = 0;
    int32_t iter;
    uint8_t data_tmp[64] = { 0, };

    if (reset_crc)
        *crcreg = CRC24_MASK;

    int_to_bin_digit(data, data_tmp);
    for (iter = 63; iter >= 0; iter--)
    {

        if ((*crcreg & (1 << 23)))
            msb = 1 ^ data_tmp[iter];
        else
            msb = 0 ^ data_tmp[iter];

        *crcreg = ((*crcreg << 1) & CRC24_MASK);
        if (msb == 1)
        {
            *crcreg = *crcreg ^ CRC24_POLYNOMIAL;
        }
    }
}

uint32_t
kaps_jr1_crc32(
    uint32_t crc,
    const uint8_t * bits,
    uint32_t num_bytes)
{
    while (num_bytes--)
        crc = crc32_table[*bits++ ^ CRC32_HIBYTE(crc)] ^ (crc << 8);
    return crc;
}

static uint32_t
floor_sqrt(
    uint32_t x)
{
    uint32_t lo;
    uint32_t hi;

    
    if (0 == x)
        return 0;

    lo = 1;
    hi = x;

    
    if (hi == 0xFFFFFFFFU)
        hi--;

    while (lo < hi)
    {
        hi = (lo + hi) / 2;
        lo = x / hi;
    }

    return hi;
}

uint32_t
kaps_jr1_find_next_prime(
    uint32_t lower_bound)
{
    uint32_t factor, candidate, limit;
    int32_t maybe_prime;

    
    if (lower_bound > 4294967291U)
        return 0;

    if (lower_bound <= 3)
        return 3;

    
    if (lower_bound % 2 == 0)
        lower_bound++;

    maybe_prime = 0;
    for (candidate = lower_bound; maybe_prime == 0; candidate += 2)
    {
        maybe_prime = 1;
        limit = floor_sqrt(candidate);
        for (factor = 3; factor <= limit; factor += 2)
        {
            if (candidate % factor == 0)
            {
                maybe_prime = 0;
                break;
            }
        }
    }

    return (candidate - 2);
}
