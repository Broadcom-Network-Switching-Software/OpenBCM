
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <shared/util.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/dnxc_fabric_cell.h>

#define SOC_DNXC_CRC_DATA_LENGHT_BASE_BITS  (BYTES2BITS(512))
#define SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS (BYTES2WORDS(512))
#define _DNXC_CRC_4K_POLYNOM                0x1021
#define _DNXC_CRC_4K_INIT_CRC               0x1634

typedef uint32 _dnxc_crc4k_bitmap_data_t[SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS];

static shr_error_e
soc_dnxc_crc4k_data_init(
    int unit,
    uint32 polynom,
    uint32 is_flip,
    _dnxc_crc4k_bitmap_data_t * crc4k_data,
    uint16 *crc4k_crc)
{
    uint32 bit, ind, new_bit;
    uint32 bits_num = SOC_DNXC_CRC_DATA_LENGHT_BASE_BITS;
    uint32 crc_length = DNXC_FTMH_CRC_SIZE_BITS;

    _dnxc_crc4k_bitmap_data_t new_data[DNXC_FTMH_CRC_SIZE_BITS];
    uint16 new_crc[DNXC_FTMH_CRC_SIZE_BITS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(crc4k_data, _SHR_E_PARAM, "crc4k_data");
    SHR_NULL_CHECK(crc4k_crc, _SHR_E_PARAM, "crc4k_crc");

    for (ind = 0; ind < DNXC_FTMH_CRC_SIZE_BITS; ind++)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_clear(new_data[ind], SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS));
        new_crc[ind] = (1 << ind);

        SHR_IF_ERR_EXIT(utilex_bitstream_clear(crc4k_data[ind], SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS));
        crc4k_crc[ind] = (1 << ind);
    }

    for (bit = 0; bit < bits_num; bit++)
    {
        for (ind = 0; ind < DNXC_FTMH_CRC_SIZE_BITS; ind++)
        {
            if (ind == 0)
            {
                if (is_flip)
                {
                    new_bit = bits_num - bit - 1;
                }
                else
                {
                    new_bit = 0;
                }

                sal_memcpy(new_data[ind], crc4k_data[crc_length - 1], sizeof(_dnxc_crc4k_bitmap_data_t));
                SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip(new_data[ind], new_bit));

                new_crc[ind] = crc4k_crc[crc_length - 1];
            }
            else
            {
                sal_memcpy(new_data[ind], crc4k_data[ind - 1], sizeof(_dnxc_crc4k_bitmap_data_t));
                new_crc[ind] = crc4k_crc[ind - 1];
                if (polynom & (1 << ind))
                {
                    SHR_IF_ERR_EXIT(utilex_bitstream_xor(new_data[ind], crc4k_data[crc_length - 1],
                                                         SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS));
                    new_crc[ind] = new_crc[ind] ^ crc4k_crc[crc_length - 1];
                }
            }
        }

        for (ind = 0; ind < DNXC_FTMH_CRC_SIZE_BITS; ind++)
        {
            sal_memcpy(crc4k_data[ind], new_data[ind], sizeof(_dnxc_crc4k_bitmap_data_t));
            crc4k_crc[ind] = new_crc[ind];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_crc_poly_based_calc_4K(
    int unit,
    uint8 *payload,
    int payload_size,
    _dnxc_crc4k_bitmap_data_t * crc4k_data,
    uint16 *crc4k_crc,
    uint16 init_crc,
    uint16 *crc_result)
{
    uint32 ii, nof_on_bits;
    uint32 byte_idx, bit_offset;
    _dnxc_crc4k_bitmap_data_t crc_in_data;
    _dnxc_crc4k_bitmap_data_t crc_data;
    uint16 crc_on_data, crc_on_crc, crc_mask;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&crc_in_data, 0, sizeof(crc_in_data));
    for (ii = 0; ii < BYTES2BITS(payload_size); ii++)
    {
        byte_idx = ii / 8;
        bit_offset = 7 - (ii % 8);

        if ((payload[byte_idx] >> bit_offset) & 1)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(crc_in_data, SOC_DNXC_CRC_DATA_LENGHT_BASE_BITS - ii - 1));
        }
    }

    crc_on_data = 0;
    crc_on_crc = 0;
    for (ii = 0; ii < DNXC_FTMH_CRC_SIZE_BITS; ii++)
    {
        sal_memcpy(crc_data, crc_in_data, sizeof(crc_data));
        SHR_IF_ERR_EXIT(utilex_bitstream_and(crc_data, crc4k_data[ii], SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS));
        nof_on_bits = utilex_bitstream_get_nof_on_bits(crc_data, SOC_DNXC_CRC_DATA_LENGHT_BASE_WORDS);
        if (nof_on_bits & 0x1)
        {
            crc_on_data |= 1 << ii;
        }

        crc_mask = init_crc & crc4k_crc[ii];
        nof_on_bits = utilex_nof_on_bits_in_short(crc_mask);
        if (nof_on_bits & 0x1)
        {
            crc_on_crc |= 1 << ii;
        }
    }

    *crc_result = crc_on_data ^ crc_on_crc;

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_back_param_calc(
    uint16 input_crc)
{
    int i, crc[DNXC_FTMH_CRC_SIZE_BITS], back_param[DNXC_FTMH_CRC_SIZE_BITS];
    uint16 crc_out = 0;

    for (i = 0; i < DNXC_FTMH_CRC_SIZE_BITS; i++)
    {
        crc[i] = (input_crc >> i) & 1;
    }

    back_param[0] = crc[0] ^ crc[12] ^ crc[13] ^ crc[15] ^ crc[2] ^ crc[5] ^ crc[6] ^ crc[8];
    back_param[1] = crc[0] ^ crc[1] ^ crc[13] ^ crc[14] ^ crc[3] ^ crc[6] ^ crc[7] ^ crc[9];
    back_param[2] = crc[0] ^ crc[1] ^ crc[10] ^ crc[14] ^ crc[15] ^ crc[2] ^ crc[4] ^ crc[7] ^ crc[8];
    back_param[3] = crc[1] ^ crc[11] ^ crc[15] ^ crc[2] ^ crc[3] ^ crc[5] ^ crc[8] ^ crc[9];
    back_param[4] = crc[0] ^ crc[10] ^ crc[13] ^ crc[15] ^ crc[3] ^ crc[4] ^ crc[5] ^ crc[8] ^ crc[9];
    back_param[5] = crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[14] ^ crc[4] ^ crc[5] ^ crc[6] ^ crc[9];
    back_param[6] = crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[12] ^ crc[15] ^ crc[2] ^ crc[5] ^ crc[6] ^ crc[7];
    back_param[7] = crc[1] ^ crc[11] ^ crc[12] ^ crc[13] ^ crc[2] ^ crc[3] ^ crc[6] ^ crc[7] ^ crc[8];
    back_param[8] = crc[0] ^ crc[12] ^ crc[13] ^ crc[14] ^ crc[2] ^ crc[3] ^ crc[4] ^ crc[7] ^ crc[8] ^ crc[9];
    back_param[9] =
        crc[0] ^ crc[1] ^ crc[10] ^ crc[13] ^ crc[14] ^ crc[15] ^ crc[3] ^ crc[4] ^ crc[5] ^ crc[8] ^ crc[9];
    back_param[10] =
        crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[14] ^ crc[15] ^ crc[2] ^ crc[4] ^ crc[5] ^ crc[6] ^ crc[9];
    back_param[11] = crc[0] ^ crc[1] ^ crc[10] ^ crc[11] ^ crc[13] ^ crc[3] ^ crc[7] ^ crc[8];
    back_param[12] = crc[1] ^ crc[11] ^ crc[12] ^ crc[14] ^ crc[2] ^ crc[4] ^ crc[8] ^ crc[9];
    back_param[13] = crc[10] ^ crc[12] ^ crc[13] ^ crc[15] ^ crc[2] ^ crc[3] ^ crc[5] ^ crc[9];
    back_param[14] = crc[0] ^ crc[10] ^ crc[11] ^ crc[13] ^ crc[14] ^ crc[3] ^ crc[4] ^ crc[6];
    back_param[15] = crc[1] ^ crc[11] ^ crc[12] ^ crc[14] ^ crc[15] ^ crc[4] ^ crc[5] ^ crc[7];

    for (i = 0; i < DNXC_FTMH_CRC_SIZE_BITS; i++)
    {
        crc_out |= back_param[i] << i;
    }

    return crc_out;
}

shr_error_e
soc_dnxc_crc_calc(
    int unit,
    uint8 *payload,
    int payload_size,
    uint16 *crc_result)
{
    static int _dnxc_crc_init_flag = 0;
    static _dnxc_crc4k_bitmap_data_t crc4k_data[DNXC_FTMH_CRC_SIZE_BITS];
    static uint16 crc4k_crc[DNXC_FTMH_CRC_SIZE_BITS];

    uint32 polynom = _DNXC_CRC_4K_POLYNOM, is_flip = 1;
    uint32 initial_crc = _DNXC_CRC_4K_INIT_CRC;
    uint16 crc_temp = 0, crc_temp1 = 0, crc_temp1_rev = 0, crc_out_rev = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (_dnxc_crc_init_flag == 0)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_crc4k_data_init(unit, polynom, is_flip, crc4k_data, crc4k_crc));
        _dnxc_crc_init_flag = 1;
    }

    SHR_IF_ERR_EXIT(dnxc_crc_poly_based_calc_4K(unit, payload, payload_size, crc4k_data, crc4k_crc, 0, &crc_temp));

    crc_temp1 = crc_temp ^ initial_crc;

    crc_temp1_rev = _shr_bit_rev16(crc_temp1);

    crc_out_rev = soc_dnxc_back_param_calc(crc_temp1_rev);

    *crc_result = _shr_bit_rev16(crc_out_rev);

exit:
    SHR_FUNC_EXIT;
}
