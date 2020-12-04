
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement soc interrupt handler.
 */


#include <shared/bsl.h>
#include <soc/intr.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/error.h>
#include <soc/drv.h>
#include <soc/intr_cmicx.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr.h>
#include <soc/dnxf/cmn/dnxf_intr.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR




int
soc_dnxf_ser_init(int unit)
{
    const dnxf_data_intr_ser_mem_mask_t *mem_mask;
    const dnxc_data_table_info_t *table_info;
    int mask_index;
    SHR_FUNC_INIT_VARS(unit);

    table_info = dnxf_data_intr.ser.mem_mask_info_get(unit);
    for (mask_index = 0; mask_index < table_info->key_size[0]; mask_index++)
    {
        mem_mask = dnxf_data_intr.ser.mem_mask_get(unit, mask_index);
        if (mem_mask->reg != INVALIDr)
        {
            switch(mem_mask->mode)
            {
                case dnxc_mem_mask_mode_none:
                {
                    
                    break;
                }
                case dnxc_mem_mask_mode_zero:
                {
                    SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 0));
                    break;
                }
                case dnxc_mem_mask_mode_one:
                {
                    SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 1));
                    break;
                }
                default:
                {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"Wrong memory mask mode %d, table index %d\n"),
                    mem_mask->mode, mask_index));
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
