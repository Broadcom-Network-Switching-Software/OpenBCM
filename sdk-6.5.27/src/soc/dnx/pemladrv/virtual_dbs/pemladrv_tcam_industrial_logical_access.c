
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../pemladrv_logical_access.h"
#include "pemladrv_tcam_logical_access.h"
#include "../pemladrv_meminfo_init.h"
#include "../pemladrv_meminfo_access.h"
#include "../pemladrv_cfg_api_defines.h"
#include "pemladrv_direct_logical_access.h"

#ifdef BCM_DNX_SUPPORT
#include <soc/register.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#endif

#ifdef BCM_DNX_SUPPORT
#define BSL_LOG_MODULE BSL_LS_SOCDNX_ADAPTERDNX
#endif

extern ApiInfo api_info[MAX_NOF_UNITS];

#ifdef BCM_DNX_SUPPORT
shr_error_e
pem_cs_tcam_read_entry(
    int unit,
    int sub_unit_idx,
    phy_mem_t * mem,
    void *entry_data)
{
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 field_data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 field_data1[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 *entry_data_as_uint;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_field.features.tcam_cs_is_tcam_direct_access_get(unit))
    {

        int tcam_key_line_address = mem->mem_address;
        int tcam_mask_line_address = tcam_key_line_address + 1;
        uint32 valid_bits = 0;

        if (mem->mem_address & 0x1)
        {
            tcam_mask_line_address = mem->mem_address;
            tcam_key_line_address = tcam_mask_line_address - 1;
        }

        sal_memset(data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        data[0] = 2;
        SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                        (unit, sub_unit_idx, FALSE, TRUE, (int) mem->block_identifier, tcam_mask_line_address,
                         mem->mem_width_in_bits, data));

        sal_memset(data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        sal_memset(field_data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                        (unit, sub_unit_idx, TRUE, TRUE, (int) mem->block_identifier, tcam_mask_line_address,
                         mem->mem_width_in_bits, data));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, 0, 2, &valid_bits));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, 2, 200, field_data));

        sal_memset(data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        data[0] = 2;
        SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                        (unit, sub_unit_idx, FALSE, TRUE, (int) mem->block_identifier, tcam_key_line_address,
                         mem->mem_width_in_bits, data));

        sal_memset(data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        sal_memset(field_data1, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));
        SHR_IF_ERR_EXIT(dnx_pem_direct_access_wrapper
                        (unit, sub_unit_idx, TRUE, TRUE, (int) mem->block_identifier, tcam_key_line_address,
                         mem->mem_width_in_bits, data));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(data, 2, 200, field_data1));

        sal_memset(data, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(uint32));

        entry_data_as_uint = entry_data;

        entry_data_as_uint[0] = 1;

        entry_data_as_uint[0] |= ((valid_bits << 3) & 0x18);

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_data1, 5, 200, entry_data_as_uint));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_data, 205, 200, entry_data_as_uint));
    }
exit:
    SHR_FUNC_EXIT;
}
#endif
