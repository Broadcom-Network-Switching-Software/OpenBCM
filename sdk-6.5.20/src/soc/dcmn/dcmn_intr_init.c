
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
#include <soc/ipoll.h>
#include <soc/dpp/JER/jer_intr_cb_func.h>
#include <soc/dpp/JER/jer_intr.h>
#include <soc/dpp/JER/jer_defs.h>

#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/register.h>


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR




STATIC 
int
soc_dcmn_ser_init_cb(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    soc_reg_t reg = ainfo->reg;
    char *reg_name = SOC_REG_NAME(unit,reg);
    int rc;
    int inst=0;
    int blk;
    soc_reg_above_64_val_t above_64;
    SOCDNX_INIT_FUNC_DEFS;
    
    if(sal_strstr(reg_name, "MEM_MASK") == NULL ||  sal_strstr(reg_name, "MESH_TOPOLOGY_ECC_ERR") != NULL)
        SOC_EXIT;

    if (SOC_IS_JERICHO(unit) &&
        ((reg == BRDC_FSRD_ECC_ERR_1B_MONITOR_MEM_MASKr) || 
            (reg == BRDC_FSRD_ECC_ERR_2B_MONITOR_MEM_MASKr) || 
            (reg == BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr))) {
        SOC_EXIT;
    }

    
    SOC_REG_ABOVE_64_ALLONES(above_64);

    if (reg == IDR_ECC_ERR_2B_MONITOR_MEM_MASKr) {
        soc_reg_above_64_field32_set(unit, reg, above_64, PACKET_CRC_ECC_2B_ERR_MASKf, 0); 
    }
    if (reg == IDR_ECC_ERR_1B_MONITOR_MEM_MASKr) {
        soc_reg_above_64_field32_set(unit, reg, above_64, PACKET_CRC_ECC_1B_ERR_MASKf, 0); 
    }

    if (SOC_IS_JERICHO(unit)) {
        
        
        if (reg == IPT_PACKET_LATENCY_MEASURE_CFGr) {
            soc_reg_above_64_field32_set(unit, reg, above_64, EN_LATENCY_DROPf , 0); 
        }
		
        
        
        if (reg == ECI_ECC_ERR_1B_MONITOR_MEM_MASKr) {
            soc_field_t field = SOC_IS_QAX(unit) ? MBU_MEM_ECC_1B_ERR_MASKf : (SOC_IS_JERICHO_PLUS(unit) ? ECC_ERR_1B_MONITOR_MEM_MASKf : FIELD_0_0f);
            soc_reg_above_64_field32_set(unit, reg, above_64, field , 0); 
        }
        if (reg == ECI_ECC_ERR_2B_MONITOR_MEM_MASKr) {
            soc_field_t field = SOC_IS_QAX(unit) ? MBU_MEM_ECC_2B_ERR_MASKf : (SOC_IS_JERICHO_PLUS(unit) ? ECC_ERR_2B_MONITOR_MEM_MASKf : FIELD_0_0f);
            soc_reg_above_64_field32_set(unit, reg, above_64, field , 0); 
        }

        if (reg == MMU_ECC_ERR_1B_MONITOR_MEM_MASK_1r) {
            soc_reg_above_64_field32_set(unit, reg, above_64, LBM_ECC_1B_ERR_MASKf, 0); 
        }
        if (reg == MMU_ECC_ERR_2B_MONITOR_MEM_MASK_1r) {
            soc_reg_above_64_field32_set(unit, reg, above_64, LBM_ECC_2B_ERR_MASKf, 0); 
        }

        if ((reg == OAMP_ECC_ERR_1B_MONITOR_MEM_MASKr) && SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
            soc_reg_above_64_field32_set(unit, reg, above_64, FLEX_VER_MASK_TEMP_ECC_1B_ERR_MASKf, 0);
        }
        if ((reg == OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr) && SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
            soc_reg_above_64_field32_set(unit, reg, above_64, FLEX_VER_MASK_TEMP_ECC_2B_ERR_MASKf, 0);
        }

        
        if ((reg == FDT_ECC_ERR_1B_MONITOR_MEM_MASKr) && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_0_ECC_1B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_1_ECC_1B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_2_ECC_1B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_3_ECC_1B_ERR_MASKf, 0);
        }
        if ((reg == FDT_ECC_ERR_2B_MONITOR_MEM_MASKr) && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_0_ECC_2B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_1_ECC_2B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_2_ECC_2B_ERR_MASKf, 0);
            soc_reg_above_64_field32_set(unit, reg, above_64, IPT_PAYLOAD_FIFO_3_ECC_2B_ERR_MASKf, 0);
        }

        
        if ((reg == OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr) && SOC_IS_QUX(unit) &&
            !(soc_mem_cache_get(unit, OAMP_RMEP_DB_EXTm, MEM_BLOCK_ALL))) {
            soc_reg_above_64_field32_set(unit, reg, above_64, RMEP_DB_EXT_ECC_2B_ERR_MASKf, 0);
        }
    }

    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block)) {
        if (SOC_INFO(unit).block_valid[blk]) {
            rc = soc_reg_above_64_set(unit, reg, inst, 0, above_64);
            SOCDNX_IF_ERR_EXIT(rc);
        }
        inst++;
    }
exit:
    SOCDNX_FUNC_RETURN;

}

int
soc_dcmn_ser_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_reg_iterate(unit, soc_dcmn_ser_init_cb, NULL));


exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
