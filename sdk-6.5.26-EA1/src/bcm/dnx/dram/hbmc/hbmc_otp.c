
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/util.h>

#include <sal/appl/sal.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/hbmc/hbmc_cb.h>
#include <soc/shmoo_hbm16.h>
#include "hbmc_dbal_access.h"
#include "hbmc_monitor.h"
#include "hbmc_otp.h"
#include <bcm_int/dnx/dram/dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/sand/sand_aux_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ingress_congestion_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/dnxc_otp.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#define DNX_HBMC_OTP_NOF_ROWS (64)
#define DNX_HBMC_OTP_ROW_DATA_BITS (64)
#define DNX_HBMC_OTP_ROW_ECC_BITS (0)
#define DNX_HBMC_OTP_ROW_BITS (DNX_HBMC_OTP_ROW_DATA_BITS + DNX_HBMC_OTP_ROW_ECC_BITS)
#define DNX_HBMC_OTP_USER_BITS_ROWS_OFFSET (22)
#define DNX_HBMC_OTP_DATA_ARR_MAX_SIZE (64)

static shr_error_e
dnx_hbmc_otp_version_to_bitmap_convert(
    int unit,
    int version,
    uint32 *bitmap)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (version)
    {
        case 1:
            *bitmap = HBMC_SHMOO_OTP_VER_1;
            break;
        case 2:
            *bitmap = HBMC_SHMOO_OTP_VER_2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid otp version read. version=%d\n", version);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_hbmc_otp_version_size_get(
    int unit,
    int version,
    int *size)
{
    int metadata_idx;
    uint32 version_bitmap;
    uint32 field_size;
    const hbmc_shmoo_channel_config_param_t *channel_metadata = soc_hbm16_shmoo_channel_config_param_metadata_get();
    const hbmc_shmoo_midstack_config_param_t *midstack_metadata = soc_hbm16_shmoo_midstack_config_param_metadata_get();
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_to_bitmap_convert
                    (unit, dnx_data_dram.general_info.otp_restore_version_get(unit), &version_bitmap));

    *size = HBMC_OTP_HEADER_SIZE;
    for (metadata_idx = 0; metadata_idx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_idx++)
    {
        if ((channel_metadata[metadata_idx].level & version_bitmap) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_channel_field_size_get(unit, channel_metadata[metadata_idx].reg,
                                                             channel_metadata[metadata_idx].field, &field_size));

            (*size) += field_size * HBMC_SHMOO_NOF_HBM_CHANNELS;
        }
    }

    for (metadata_idx = 0; metadata_idx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_idx++)
    {
        if ((midstack_metadata[metadata_idx].level & version_bitmap) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_midstack_field_size_get(unit, midstack_metadata[metadata_idx].reg,
                                                              midstack_metadata[metadata_idx].field, &field_size));
            (*size) += field_size;
        }
    }

    *size = UTILEX_ALIGN_UP(*size, 8);
    *size = *size + 16;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_hbmc_otp_header_info_get(
    int unit,
    int hbm_index,
    int is_quiet,
    int *start_bit_idx,
    int *nof_bits)
{
    uint32 version = 0xF;
    SHR_BIT_DCL_CLR_NAME(header, HBMC_OTP_HEADER_SIZE);
    int bit_idx = (DNX_HBMC_OTP_USER_BITS_ROWS_OFFSET * DNX_HBMC_OTP_ROW_BITS);
    int size_per_ver = 0;
    int invalid;
    const int otp_instance = SOC_DNXC_OTP_INSTANCE_HBM_0 + hbm_index;
    SHR_FUNC_INIT_VARS(unit);

    do
    {

        if (bit_idx > (DNX_HBMC_OTP_NOF_ROWS * DNX_HBMC_OTP_ROW_BITS - HBMC_OTP_HEADER_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY,
                         "HBM tune restore from otp - no valid data to read. hbm_index=%d (bit_idx=%d)  \n", hbm_index,
                         bit_idx);
        }

        SHR_IF_ERR_EXIT(soc_dnxc_otp_read(unit, otp_instance, bit_idx, HBMC_OTP_HEADER_SIZE, 0, header));

        version = HBMC_OTP_HEADER_VERSION_GET(header[0]);
        invalid = HBMC_OTP_HEADER_INVALID_GET(header[0]);

        if (version == 0)
        {
            if (is_quiet)
            {
                SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_EMPTY);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_EMPTY,
                             "HBM tune restore from otp - Not found any data to restore from otp. hbm_index=%d (bit_idx=%d) \n",
                             hbm_index, bit_idx);
            }
        }
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_size_get(unit, version, &size_per_ver));

        if ((version != dnx_data_dram.general_info.otp_restore_version_get(unit)) || ((invalid == 1)))
        {

            bit_idx += size_per_ver;
            if ((invalid == 0) && (version != 0))
            {
                LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                     "hbm tune restore from otp - read from otp version=%d, expected version=%d. hbm_index=%d \n"),
                                          version, dnx_data_dram.general_info.otp_restore_version_get(unit),
                                          hbm_index));
            }
        }

    }
    while ((version != dnx_data_dram.general_info.otp_restore_version_get(unit)) || (invalid == 1));

    (*start_bit_idx) = bit_idx;
    (*nof_bits) = size_per_ver;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
hbm_otp_read_data(
    int unit,
    int hbm_index,
    int is_quiet,
    uint32 *concatenate_data)
{
    int start_bit_addr = 0;
    int nof_bits = 0;
    int nof_words = 0;
    int nof_bytes = 0;
    uint16 crc;
    int word_i;
    uint32 concatenate_data_le[DNX_HBMC_OTP_DATA_ARR_MAX_SIZE];
    const int otp_instance = SOC_DNXC_OTP_INSTANCE_HBM_0 + hbm_index;
    SHR_FUNC_INIT_VARS(unit);

    if (is_quiet)
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(dnx_hbmc_otp_header_info_get
                                           (unit, hbm_index, is_quiet, &start_bit_addr, &nof_bits), _SHR_E_EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_otp_header_info_get(unit, hbm_index, is_quiet, &start_bit_addr, &nof_bits));
    }

    nof_words = nof_bits / SHR_BITWID;
    nof_bytes = nof_words * sizeof(uint32);
    if (nof_words > DNX_HBMC_OTP_DATA_ARR_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_bits=%d, bigger than concatenate_data array size \n", nof_bits);
    }
    SHR_IF_ERR_EXIT(soc_dnxc_otp_read(unit, otp_instance, start_bit_addr, nof_bits, 0, concatenate_data));

    for (word_i = 0; word_i < nof_words; word_i++)
    {
#if defined(BE_HOST)

        concatenate_data_le[word_i] = UTILEX_BYTE_SWAP(concatenate_data[word_i]);
#else
        concatenate_data_le[word_i] = concatenate_data[word_i];
#endif
    }

    crc = _shr_crc16(0, (unsigned char *) concatenate_data_le, nof_bytes);
    if (crc)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "the CRC of the image is different than zero 0x%x, CRC size 0x%x", crc,
                     nof_bytes);
    }

exit:
    SHR_FUNC_EXIT;
}

soc_error_t
dnx_hbmc_otp_shmoo_hbm16_restore_from_otp(
    int unit,
    int hbm_ndx,
    int is_quiet,
    hbmc_shmoo_config_param_t * shmoo_config_param)
{
    int bit_idx = 0;
    uint32 concatenate_data[DNX_HBMC_OTP_DATA_ARR_MAX_SIZE];
    const hbmc_shmoo_channel_config_param_t *channel_metadata = soc_hbm16_shmoo_channel_config_param_metadata_get();
    const hbmc_shmoo_midstack_config_param_t *midstack_metadata = soc_hbm16_shmoo_midstack_config_param_metadata_get();
    uint32 otp_restore_version;
    uint32 field_size;
    uint32 max_vdl_dqs_p_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(concatenate_data, 0x0, DNX_HBMC_OTP_DATA_ARR_MAX_SIZE * sizeof(uint32));

    if (is_quiet)
    {
        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(hbm_otp_read_data(unit, hbm_ndx, is_quiet, concatenate_data), _SHR_E_EMPTY);
    }
    else
    {
        SHR_IF_ERR_EXIT(hbm_otp_read_data(unit, hbm_ndx, is_quiet, concatenate_data));
    }

    bit_idx += HBMC_OTP_HEADER_SIZE;

    SHR_IF_ERR_EXIT(dnx_hbmc_otp_version_to_bitmap_convert
                    (unit, dnx_data_dram.general_info.otp_restore_version_get(unit), &otp_restore_version));

    for (int channel_ndx = 0; channel_ndx < dnx_data_dram.general_info.nof_channels_get(unit); channel_ndx++)
    {
        for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_CHANNEL_METADATA_SIZE; metadata_ndx++)
        {

            if ((channel_metadata[metadata_ndx].level & otp_restore_version) != 0)
            {
                SHR_IF_ERR_EXIT(soc_hbm16_channel_field_size_get(unit, channel_metadata[metadata_ndx].reg,
                                                                 channel_metadata[metadata_ndx].field, &field_size));
                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(concatenate_data, bit_idx,
                                                               field_size,
                                                               &shmoo_config_param->channel_data[channel_ndx]
                                                               [metadata_ndx].value));
                shmoo_config_param->channel_data[channel_ndx][metadata_ndx].valid = TRUE;
                bit_idx += field_size;
                if ((channel_metadata[metadata_ndx].reg == HBM16_DWORD0_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD1_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD2_READ_MAX_VDL_DQS_Pr) ||
                    (channel_metadata[metadata_ndx].reg == HBM16_DWORD3_READ_MAX_VDL_DQS_Pr))
                {
                    max_vdl_dqs_p_value = shmoo_config_param->channel_data[channel_ndx][metadata_ndx].value;
                }
            }
            else
            {
                if (dnx_data_dram.general_info.otp_restore_version_get(unit) == 1)
                {
                    if (((channel_metadata[metadata_ndx].reg == HBM16_DWORD0_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD1_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD2_READ_MAX_VDL_DQS_Nr) ||
                         (channel_metadata[metadata_ndx].reg == HBM16_DWORD3_READ_MAX_VDL_DQS_Nr)) &&
                        (channel_metadata[metadata_ndx].field == HBM16_MAX_VDL_STEPf))
                    {
                        shmoo_config_param->channel_data[channel_ndx][metadata_ndx].valid = TRUE;
                        shmoo_config_param->channel_data[channel_ndx][metadata_ndx].value = max_vdl_dqs_p_value;
                    }
                }
            }
        }
    }

    for (int metadata_ndx = 0; metadata_ndx < HBMC_SHMOO_MIDSTACK_METADATA_SIZE; metadata_ndx++)
    {

        if ((midstack_metadata[metadata_ndx].level & otp_restore_version) != 0)
        {
            SHR_IF_ERR_EXIT(soc_hbm16_midstack_field_size_get(unit, midstack_metadata[metadata_ndx].reg,
                                                              midstack_metadata[metadata_ndx].field, &field_size));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(concatenate_data, bit_idx,
                                                           field_size,
                                                           &shmoo_config_param->midstack_data[metadata_ndx].value));
            shmoo_config_param->midstack_data[metadata_ndx].valid = TRUE;
            bit_idx += field_size;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
