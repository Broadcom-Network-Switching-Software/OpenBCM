/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        shadow.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/debug.h>

#ifdef BCM_SHADOW_SUPPORT
#include <soc/shadow.h>

/*
 * shadow chip driver functions.  
 */
soc_functions_t soc_shadow_drv_funs = {
    soc_shadow_misc_init,
    soc_shadow_mmu_init,
    soc_shadow_age_timer_get,
    soc_shadow_age_timer_max_get,
    soc_shadow_age_timer_set,
};

static uint32 iptdm5[30] = {1,9,5,13,31,1,9,5,13,31,
                            1,9,5,13,0,1,9,5,13,31,
                            1,9,5,13,31,1,9,5,13,31};

static uint32 mmutdm5[30] = {2,7680,32,122880,0,2,7680,32,122880,0,
                             2,7680,32,122880,1,2,7680,32,122880,0,
                             2,7680,32,122880,0,2,7680,32,122880,0};

static uint32 iptdm6[30] = {1,9,5,13,0,1,9,5,13,31,
                            1,9,5,13,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,0,0};

static uint32 mmutdm6[30] = {2,130560,32,0,0,2,130560,32,0,0,
                             2,130560,32,0,1,2,130560,32,0,0,
                             2,130560,32,0,0,2,130560,32,0,0};

static uint32 iptdm7[60] = {1,9,5,13,31,2,9,6,13,31,
                            3,9,7,13,0,4,9,8,13,31,
                            1,9,5,13,31,2,9,6,13,31,
                            3,9,7,13,31,4,9,8,13,31,
                            1,9,5,13,0,2,9,6,13,31,
                            3,9,7,13,31,4,9,8,13,31};

static uint32 mmutdm7[60] = {2,7680,32,122880,0,4,7680,64,122880,0,
                             8,7680,128,122880,1,16,7680,256,122880,0,
                             2,7680,32,122880,0,4,7680,64,122880,0,
                             8,7680,128,122880,0,16,7680,256,122880,0,
                             2,7680,32,122880,1,4,7680,64,122880,0,
                             8,7680,128,122880,0,16,7680,256,122880,0};

static uint32 iptdm8[60] = {1, 9, 5, 13, 0, 2, 9, 6, 13, 0x1f,
                           3, 9, 7, 13, 0, 4, 9, 8, 13, 0x1f,
                           1, 9, 5, 13, 0, 2, 9, 6, 13, 0x1f,
                           3, 9, 7, 13, 0, 4, 9, 8, 13, 0x1f,
                           1, 9, 5, 13, 0, 2, 9, 6, 13, 0x1f,
                           3, 9, 7, 13, 0, 4, 9, 8, 13, 0x1f};

static uint32 mmutdm8[60] = {2,130560,32,0,0,4,130560,64,0,0,
                             8,130560,128,0,1,16,130560,256,0,0,
                             2,130560,32,0,0,4,130560,64,0,0,
                             8,130560,128,0,0,16,130560,256,0,0,
                             2,130560,32,0,1,4,130560,64,0,0,
                             8,130560,128,0,0,16,130560,256,0,0};

static uint32 iptdm9[30] = {1,9,5,13,31,1,9,5,13,31,
                            1,9,5,13,0,1,9,5,13,31,
                            1,9,5,13,31,1,9,5,13,31};

static uint32 mmutdm9[30] = {2,512,32,8192,0,2,512,32,8192,0,
                             2,512,32,8192,1,2,512,32,8192,0,
                             2,512,32,8192,0,2,512,32,8192,0};

static uint32 iptdm0_xfi[43] = {1, 9, 5, 11, 13, 1, 15, 5, 10, 12, 1,
                             14, 5, 16, 9, 1, 11, 5, 13, 15, 31, 1,
                             10, 5, 12, 14, 1, 16, 5, 9, 11, 1, 13,
                             5, 15, 10, 1, 12, 5, 14, 16, 31, 0};

static uint32 mmutdm0_xfi[60] =  {2, 512, 32, 8192, 1024, 2, 16384, 32, 2048, 32768, 
                                  2, 4096, 32, 65536, 0, 2, 512, 32, 8192, 1024,
                                  2, 16384, 32, 2048, 32768, 2, 4096, 32, 65536, 0,
                                  2, 512, 32, 8192, 1024, 2, 16384, 32, 2048, 32768,
                                  2, 4096, 32, 65536, 0, 2, 512, 32, 8192, 1024,
                                  2, 16384, 32, 2048, 32768, 2, 4096, 32, 65536, 0};

static uint32 iptdm1_xfi[43] = {1,9,5,11,13,2,15,6,10,12,
                            3,14,7,16,9,4,11,8,13,15,
                            31,1,10,5,12,14,2,16,6,9,
                            11,3,13,7,15,10,4,12,8,14,
                            16,31,0};

static uint32 mmutdm1_xfi[60] = {2,512,32,8192,1024,4,16384,64,2048,32768,
                             8,4096,128,65536,1,16,512,256,8192,1024,
                             2,16384,32,2048,32768,4,4096,64,65536,0,
                             8,512,128,8192,1024,16,16384,256,2048,32768,
                             2,4096,32,65536,1,4,512,64,8192,1024,
                             8,16384,128,2048,32768,16,4096,256,65536,0};

static uint32 iptdm18[31] = {1,9,5,13,31,1,10,5,15,11,
                            1,14,5,12,16,1,9,5,13,31,
                            1,10,5,15,11,1,14,5,12,16,0};

static uint32 mmutdm18[30] = {2,512,32,8192,1024,2,16384,32,2048,32768,
                             2,4096,32,65536,1,2,512,32,8192,1024,
                             2,16384,32,2048,32768,2,4096,32,65536,0};

static uint32 iptdm19[43] = {1,9,5,11,13,2,15,6,10,12,
                            3,14,7,16,9,4,11,8,13,15,
                            31,1,10,5,12,14,2,16,6,9,
                            11,3,13,7,15,10,4,12,8,14,
                            16,31,0};

static uint32 mmutdm19[60] = {2,1536,32,0,24576,4,0,64,6144,0,
                             8,98304,128,0,1,16,1536,256,0,24576,
                             2,0,32,6144,0,4,98304,64,0,0,
                             8,1536,128,0,24576,16,0,256,6144,0,
                             2,98304,32,0,1,4,1536,64,0,24576,
                             8,0,128,6144,0,16,98304,256,0,0};

static const soc_reg_t thermal_reg[] = {
    CMIC_THERMAL_MON_RESULT_0r, CMIC_THERMAL_MON_RESULT_1r,
    CMIC_THERMAL_MON_RESULT_2r, CMIC_THERMAL_MON_RESULT_3r,
    CMIC_THERMAL_MON_RESULT_4r, CMIC_THERMAL_MON_RESULT_5r,
    CMIC_THERMAL_MON_RESULT_6r, CMIC_THERMAL_MON_RESULT_7r
};


static int
soc_shadow_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec;
    soc_timeout_t       to;

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf,
                      SOC_IS_SC_CQ(unit) ? 0x8000 : 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf,
                      SOC_IS_SC_CQ(unit) ? 0x4000 : 0x2000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

int
soc_shadow_misc_init(int unit)
{
    static const soc_field_t port_field[4] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    soc_info_t          *si;
    uint32              rval;
    uint64              reg64;
    uint32              addr;
    soc_pbmp_t          pbmp;
    int                 port, divisor, dividend;
    uint32              *iparr = NULL, *mmuarr = NULL;
    int                 i, idx, ip_tdm_size = 0, mmu_tdm_size = 0;
    uint16              dev_id;
    uint8               rev_id;
    arb_tdm_table_entry_t arb_tdm;
    es_arb_tdm_table_entry_t es_arb_tdm;
    int                 blk, blk_num, bindex;
    uint32              core_mode, phy_mode;
    uint8               port_exist[18], port_valid[18];
    uint8               active_il = 0;
    uint8               en40g = 0;
    int                 pipe_init_usec;
    soc_timeout_t       to;
    il_channel_remap0_entry_t remap0;
    egr_il_channel_map_entry_t egr_map;
    soc_port_t p;
    soc_field_t field_ids[1];
    uint32 field_values[1];
    int prio = 0;
    int ext_mdio_divisor, ext_mdio_dividend,
        int_mdio_divisor, int_mdio_dividend,
        stdma_divisor, stdma_dividend,
        i2c_divisor, i2c_dividend;

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EPIPE Memories */
        SOC_IF_ERROR_RETURN(soc_shadow_pipe_mem_clear(unit));
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval,
                      METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

#ifdef BCM_HB_GW_SUPPORT
    if (soc_reg_field_valid(unit, CMIC_XGXS0_PLL_CONTROL_2r,
                            XGPLL_EN125f)) {
        SOC_IF_ERROR_RETURN(READ_CMIC_XGXS0_PLL_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, CMIC_XGXS0_PLL_CONTROL_2r, &rval,
                          XGPLL_EN125f, 1);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_XGXS0_PLL_CONTROL_2r(unit, rval));
    }
#endif

    /* Take temp and votlage monitors out of powerdown and reset */
    SOC_IF_ERROR_RETURN(READ_CMIC_THERMAL_MON_CTRLr(unit, &rval));
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, BG_ADJf, 2);
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, PWDNf, 0);
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, RESETBf, 0);
    WRITE_CMIC_THERMAL_MON_CTRLr(unit, rval);
    soc_reg_field_set(unit, CMIC_THERMAL_MON_CTRLr, &rval, RESETBf, 1);
    WRITE_CMIC_THERMAL_MON_CTRLr(unit, rval);


    /* To get 125/25MHz clocks to internal MAC */
    SOC_IF_ERROR_RETURN(READ_CMIC_XGXS0_PLL_CONTROL_1r(unit, &rval));
    rval |= 1;
    SOC_IF_ERROR_RETURN(WRITE_CMIC_XGXS0_PLL_CONTROL_1r(unit, rval));

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_XGXS1_PLL_CONTROL_1r(unit, &rval));
    rval |= 1;
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_XGXS1_PLL_CONTROL_1r(unit, rval));

    if (soc_property_get(unit, spn_BCM_STAT_FLAGS, 0)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IARB_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IPARS_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW1_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IFP_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW2_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_HW_CONTROLr, REG_PORT_ANY,
                                        XGS_COUNTER_COMPAT_MODEf, 1));
    }

    if (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0)) {
        ip_tdm_size = mmu_tdm_size = 30;
        iparr = iptdm5;
        mmuarr = mmutdm5;
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x7; /* SPEED = IL50G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        active_il = 2;
        en40g = 1;

        for (idx = 0; idx <= 7; idx ++) {
            sal_memset(&remap0, 0, sizeof(remap0));
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 9);
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        }
        for (idx = 0; idx <= 63; idx ++) {
            sal_memset(&egr_map, 0, sizeof(egr_map));
            soc_EGR_IL_CHANNEL_MAPm_field32_set(unit, &egr_map, CHANNELf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_IL_CHANNEL_MAPm(unit, MEM_BLOCK_ALL, idx, &egr_map));
        }
        /* uflow set for Xe0 -> port 10 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                        DEST_PORTf, 9)); 

        /* uflow set for Xe1 -> port 13 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                        DEST_PORTf, 13)); 

        /* Static balance for Switch ports : (1->[9,10,11,12] 5->[13,14,15,16]) */
        for (p = 9; p <= 16; p++) {
            idx = ((p-8)/5)?5:1;
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, idx)); 
        }

    } else if (soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) {
        ip_tdm_size =15;
        mmu_tdm_size = 30;
        iparr = iptdm6;
        mmuarr = mmutdm6;
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x7; /* SPEED = IL50G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        rval = 0; /* ENABLE = 0 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        active_il = 1;
        en40g = 1;

        for (idx = 0; idx <= 7; idx ++) {
            sal_memset(&remap0, 0, sizeof(remap0));
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 9);
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        }
        for (idx = 8; idx <= 15; idx ++) {
            sal_memset(&remap0, 0, sizeof(remap0));
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 13);
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, (idx - 8));
            SOC_IF_ERROR_RETURN
                (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        }
        for (idx = 0; idx <= 63; idx ++) {
            sal_memset(&egr_map, 0, sizeof(egr_map));
            soc_EGR_IL_CHANNEL_MAPm_field32_set(unit, &egr_map, CHANNELf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_IL_CHANNEL_MAPm(unit, MEM_BLOCK_ALL, idx, &egr_map));
        }
        /* Some XMAC related stuff: this needs to be moved to xmac.c */
        PBMP_XE_ITER(unit, p) {
            SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_CTRLr, &reg64, XLGMII_ALIGN_ENBf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, p, reg64));
            SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &reg64, PAD_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, p, reg64));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, XMAC_RX_MAX_SIZEr, p, 
                                        RX_MAX_SIZEf, 9600));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, GPORT_CNTMAXSIZEr, p, 
                                        CNTMAXSIZEf, 9600));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                    PORT_BITMAPf, 0x1ffff));

    } else if (soc_property_get(unit, spn_BCM88732_2X40_2X40E, 0)) {
        ip_tdm_size = mmu_tdm_size = 30;
        iparr = iptdm9;
        mmuarr = mmutdm9;
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        en40g = 1;
    } else if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0)) {
        port_tab_entry_t ptab;
        ip_tdm_size = 43;
        mmu_tdm_size = 60;
        iparr = iptdm0_xfi;
        mmuarr = mmutdm0_xfi;
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x3; /* SPEED = 20G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0B_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1B_PORT_MODEf, 1)); /* Dual */
        en40g = 1;
        /* Some XMAC related stuff: this needs to be moved to xmac.c */
        PBMP_XE_ITER(unit, p) {
            SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_CTRLr, &reg64, XLGMII_ALIGN_ENBf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, p, reg64));
            SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &reg64, PAD_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, p, reg64));
        }
        /* Setup Static load balance */   
        field_ids[0] = DEST_PORT_OFFSETf;
        for (i = 0; i < 65535; i+=4) {
            field_values[0] = 8;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i, 1,
                                     field_ids, field_values));
            field_values[0] = 9;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+1, 1,
                                     field_ids, field_values));
            field_values[0] = 10;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+2, 1,
                                     field_ids, field_values));
            field_values[0] = 11;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+3, 1,
                                     field_ids, field_values));
        }
        /* Configure Port Table to use UFLOW_A */
        SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, 5, &ptab));
        soc_PORT_TABm_field32_set(unit, &ptab, AGGREGATION_GROUP_SELECTf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, 5, &ptab));

        /* UFLOW_PORT reg will use UFLOW_A for load balance */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    DEST_PORTf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 1));

        /* Setup Static load balance for Port 5*/   
        field_ids[0] = DEST_PORT_OFFSETf;
        for (i = 0; i < 65535; i+=4) {
            field_values[0] = 12;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i, 1,
                                     field_ids, field_values));
            field_values[0] = 13;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+1, 1,
                                     field_ids, field_values));
            field_values[0] = 14;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+2, 1,
                                     field_ids, field_values));
            field_values[0] = 15;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+3, 1,
                                     field_ids, field_values));
        }
        /* port 5 (xe1) configure uflow_b for static load balancing */
        SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, 5, &ptab));
        soc_PORT_TABm_field32_set(unit, &ptab, AGGREGATION_GROUP_SELECTf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, 5, &ptab));

        /* UFLOW_PORT reg will use UFLOW_A for load balance */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    DEST_PORTf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    HASH_MODEf, 1));

        /* For switch link ports (9-12) use port 1 as the destination port */
        for (p = 9; p <= 12; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 1)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
        }
        /* For switch link ports (13-16) use port 5 as the destination port */
        for (p = 13; p <= 16; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 5)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                    PORT_BITMAPf, 0x1ffff));
    } else if (soc_property_get(unit, spn_BCM88732_8X10_8X10, 0)) {
        ip_tdm_size = 43;
        mmu_tdm_size = 60;
        iparr = iptdm1_xfi;
        mmuarr = mmutdm1_xfi;
        rval = 0x1; /* SPEED = 10G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x3; /* SPEED = 20G, ENABLE = 1 (dual ports) */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0B_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1B_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 2)); /* Quad */
        en40g = 0;
        /* Some XMAC related stuff: this needs to be moved to xmac.c */
        PBMP_XE_ITER(unit, p) {
            SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &reg64, PAD_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, p, reg64));
        }
        /* Setup Static load balance for Port (one to one mapping) */   
        for (p = 1; p <= 8; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, p+8)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p+8,
                                        DEST_PORTf, p)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p+8, 
                                        HASH_MODEf, 0));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                    PORT_BITMAPf, 0x1ffff));
    } else if (soc_property_get(unit, spn_BCM88732_8X10_1X40, 0)) {
        /* 8x10G port-link 1x40G IL */
        ip_tdm_size = 60;
        mmu_tdm_size = 60;
        iparr = iptdm8;
        mmuarr = mmutdm8;
        rval = 0x1; /* SPEED = 10G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x7; /* SPEED = IL50G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        rval = 0; /* ENABLE = 0 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        active_il = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                    PORT_BITMAPf, 0x1ffff));

        for (idx = 0; idx <= 63; idx ++) {
            sal_memset(&remap0, 0, sizeof(remap0));
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 
                                               9 + idx % 8);
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, idx % 8);
            SOC_IF_ERROR_RETURN
                (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        }
        for (idx = 0; idx <= 63; idx ++) {
            sal_memset(&egr_map, 0, sizeof(egr_map));
            soc_EGR_IL_CHANNEL_MAPm_field32_set(unit, &egr_map, CHANNELf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_IL_CHANNEL_MAPm(unit, MEM_BLOCK_ALL, idx, &egr_map));
        }
        /* Some XMAC related stuff: this needs to be moved to xmac.c */
        PBMP_XE_ITER(unit, p) {
            SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, p, &reg64));
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &reg64, PAD_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, p, reg64));
        }
    } else if (soc_property_get(unit, spn_BCM88732_1X40_4X10, 0)) {
        ip_tdm_size = 31;
        mmu_tdm_size = 30;
        iparr = iptdm18;
        mmuarr = mmutdm18;
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        rval = 0x3; /* SPEED = 20G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0B_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1B_PORT_MODEf, 1)); /* Dual */
        en40g = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                    PORT_BITMAPf, 0x1ffff));

        /* Setup Static load balance */   
        field_ids[0] = DEST_PORT_OFFSETf;
        for (i = 0; i < 65535; i+=4) {
            field_values[0] = 8;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i, 1,
                                     field_ids, field_values));
            field_values[0] = 10;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+1, 1,
                                     field_ids, field_values));
            field_values[0] = 12;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+2, 1,
                                     field_ids, field_values));
            field_values[0] = 14;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+3, 1,
                                     field_ids, field_values));
        }
        /* Configure Port Table to use UFLOW_A */
        field_ids[0] = AGGREGATION_GROUP_SELECTf;
        field_values[0] = 0;
        SOC_IF_ERROR_RETURN
            (soc_mem_fields32_modify(unit, PORT_TABm, 1, 1,
                                 field_ids, field_values));

        /* UFLOW_PORT reg will use UFLOW_A for load balance */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    DEST_PORTf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 1));

        /* For switch link ports use port 1 as the destination port */
        for (p = 9; p <= 16; p+=2) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 1)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
        }
    } else if (soc_property_get(unit, spn_BCM88732_4X10_4X10, 0)) {
        ip_tdm_size = 43;
        mmu_tdm_size = 60;
        iparr = iptdm19;
        mmuarr = mmutdm19;
        rval = 0x1; /* SPEED = 10G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        rval = 0x3; /* SPEED = 20G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX0B_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1A_PORT_MODEf, 1)); /* Dual */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IX1B_PORT_MODEf, 1)); /* Dual */
        /* Configure load balance */
        for (p = 1; p <= 4; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    DEST_PORTf, (8 + (p-1)*2)));
            SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 0));
        }

        /* For switch link ports use port 1-4 as the destination port */
        rval = 0;
        for (p = 9; p < 16; p+=2) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, rval)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
            rval++;
        }
    } else {
        /* 8x10G port-link 2x40G IL - default */
        ip_tdm_size = 60;
        mmu_tdm_size = 60;
        iparr = iptdm7;
        mmuarr = mmutdm7;
        rval = 0x1; /* SPEED = 10G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x7; /* SPEED = IL50G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG2r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG3r(unit, rval));
        active_il = 2;
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 2)); /* Quad */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 2)); /* Quad */

        for (idx = 0; idx <= 31; idx ++) {
            sal_memset(&remap0, 0, sizeof(remap0));
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 
                                               9 + idx % 8);
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, idx % 8);
            SOC_IF_ERROR_RETURN
                (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        }
        for (idx = 0; idx <= 63; idx ++) {
            sal_memset(&egr_map, 0, sizeof(egr_map));
            soc_EGR_IL_CHANNEL_MAPm_field32_set(unit, &egr_map, CHANNELf, idx);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_IL_CHANNEL_MAPm(unit, MEM_BLOCK_ALL, idx, &egr_map));
        }
    }

    /* Program the TDM tables */
    for (i = 0; (i < ip_tdm_size) && iparr; i++) {
        sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));    
        soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, iparr[i]);
        if (i == (ip_tdm_size - 1)) {
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, WRAP_ENf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                 &arb_tdm));
    }
    SOC_IF_ERROR_RETURN(WRITE_TDM_ENr(unit, 1));

    for (i = 0; (i < mmu_tdm_size) && mmuarr; i++) {
        sal_memset(&es_arb_tdm, 0, sizeof(es_arb_tdm_table_entry_t));
        soc_ES_ARB_TDM_TABLEm_field32_set(unit, &es_arb_tdm, PORT_VECTORf, 
                                          mmuarr[i]);
        SOC_IF_ERROR_RETURN(WRITE_ES_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL, i, 
                                                    &es_arb_tdm));
    }
    rval = 0;
    soc_reg_field_set(unit, ES_TDM_CAL_CFGr, &rval, END_Bf, mmu_tdm_size);
    soc_reg_field_set(unit, ES_TDM_CAL_CFGr, &rval, END_Af, mmu_tdm_size);
    SOC_IF_ERROR_RETURN(WRITE_ES_TDM_CAL_CFGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_ES_TDM_ENr(unit, 1));

    /*
     * Egress Enable
     */
    rval = 0;
    PBMP_ALL_ITER(unit, port) {
        soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
        if (IS_IL_PORT(unit, port) && (port != 9) && (port != 13)) {
            /* Second IL port is enabled only in few configurations */
                continue;
        }
        if ((port == 13) &&
             (soc_property_get(unit, spn_BCM88732_8X10_1X40, 0) ||
             soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) ) {
            continue;
        }
        if (SOC_PBMP_MEMBER(si->port.disabled_bitmap, port)) {
            soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    }

    /* QG ports only */
    SOC_PBMP_ASSIGN(pbmp, PBMP_GE_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_GX_ALL(unit));
    /* Clear 8 beat accumulation on QG ports only */
    rval = 0;
    PBMP_ITER(pbmp, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ACCU_8BEATSr(unit, port, rval));
    }

    if (soc_feature(unit, soc_feature_ctr_xaui_activity)) {
        PBMP_PORT_ITER(unit, port) {
            break;
        }
        addr = soc_reg_addr(unit, XPORT_CONFIGr, port, 0);
        addr &= ~(0x3F << SOC_REGIDX_BP);

        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        /* 
         * For correct LED operation, XPORT must be enabled also on
         * all unused  ports, however the soc_reg_addr function only
         * works on valid ports, so the registers address must be 
         * calculated 'manually'.
         */
        for (port = 0;  port < 20; port++) {
            if (!SOC_PORT_VALID(unit, port)) {
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit, 
                                    addr | (port << SOC_REGIDX_BP), rval));
            }
        }
    }

    /*
     * ING_CONFIG
     */
    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); 
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    rval = 0x01;	/* 125KHz I2C sampling rate */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_I2C_STATr(unit, rval));


    /*
     * Premises of the calculation:
     *
     * Core clock speed
     * 56820 (220 MHz)
     * 56821/56720/56721 (225 MHz)
     * 56822/56823/56725 (245 MHz)
     * 56825 (290 MHz)
     *
     *  EXT_MDIO result = INT_MDIO result = 5 MHz
     *  I2C result / (2 * 10) = 250-400 kHz
     *      I2C result = 5-8 MHz
     *  STDMA result = 25 MHz
     *
     * result = core * dividend / divisor
     *      ==>
     * dividend : divisor = result : core
     */

    switch (dev_id) {
    case BCM88732_DEVICE_ID:
        ext_mdio_dividend = 1;
        ext_mdio_divisor = 44;
        int_mdio_dividend = 1;
        int_mdio_divisor = 44;
        stdma_dividend = 1;
        stdma_divisor = 9;
        i2c_dividend = 1;
        i2c_divisor = 40;
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR,
                               ext_mdio_divisor);
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND,
                                ext_mdio_dividend);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval,
                      DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval,
                      DIVIDENDf, dividend);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR,
                               int_mdio_divisor);
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND,
                                int_mdio_dividend);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval,
                      DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval,
                      DIVIDENDf, dividend);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    divisor = soc_property_get(unit, spn_RATE_I2C_DIVISOR,
                               i2c_divisor);
    dividend = soc_property_get(unit, spn_RATE_I2C_DIVIDEND,
                                i2c_dividend);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval,
                      DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval,
                      DIVIDENDf, dividend);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));

    divisor = soc_property_get(unit, spn_RATE_STDMA_DIVISOR,
                               stdma_divisor);
    dividend = soc_property_get(unit, spn_RATE_STDMA_DIVIDEND,
                                stdma_dividend);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                      DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                      DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));

    
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_STAT_CLEARf, 0);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));


    /* Initialize the Interlaken blocks */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 40000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    switch (active_il) {
    case 1:
        SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT10_MAPPINGf, 0x9);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT11_MAPPINGf, 0x9);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT12_MAPPINGf, 0x9);
        SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, SOFT_RESETf, 0);
        soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_IL_ENf, 1);
        soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, 
                          WC_CONFIG_IL_MODEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IL_GLOBAL_CONTROLr(unit, 9, rval));
        /* Need to reset only after Warpcores are initialized */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONTROLr, 9, SOFT_RESETf, 1));

        rval = 0;
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM0f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM1f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM2f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM3f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM4f, 1);
        SOC_IF_ERROR_RETURN(WRITE_IL_MEMORY_INITr(unit, 9, rval));
        do {
            SOC_IF_ERROR_RETURN(READ_IL_MEMORY_INIT_DONEr(unit, 9, &rval));
            if (soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM0f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM1f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM2f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM3f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM4f)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : IL_MEMORY_INIT_DONE timeout\n"), unit));
                break;
            }
        } while (TRUE);

        /* Need to take out of reset only after Warpcores are initialized */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONTROLr, 9, SOFT_RESETf, 1));

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IL0_ENf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                    CFG_TX_WC_BITFLIPf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                    CFG_RX_WC_BITFLIPf, 1));

        /* Configure IL Counters in accumulate mode;default is read on clear */
        if (soc_property_get(unit, spn_BCM_STAT_FLAGS, 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        }

        /* Set Inband flow control, reset val(0x1) is OOB for shadow */
        if (soc_property_get(unit, spn_BCM88732_USE_OOB, 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        CFG_FLOWCONTROL_TYPEf, 1));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        CFG_FLOWCONTROL_TYPEf, 0));
        }

        /* Set lane for 16lane support */
        if (soc_property_get(unit, "IL3125", 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_RX_CORE_CONFIG1r, 9, 
                                        CTL_RX_LAST_LANEf, 15));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_TX_CORE_CONFIG2r, 9, 
                                        CTL_TX_LAST_LANEf, 15));
            /* 
             * Need to set WC_CONFIG_IL for IL1 in 16 lane mode as SERDES MUX
             * is contorolled by IL1
             */
            soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, SOFT_RESETf, 0);
            soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_IL_ENf, 1);
            soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_CHANNEL_SELf, 0);
            soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_BROADCAST_ENf, 0);
            soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_IL_MODEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_IL_GLOBAL_CONTROLr(unit, 13, rval));

        }
        break;
    case 2:
        SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT10_MAPPINGf, 0x9);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT11_MAPPINGf, 0x9);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, 
                          PORT12_MAPPINGf, 0x9);
        SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_2r(unit, &rval));
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, 
                          PORT14_MAPPINGf, 0xd);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, 
                          PORT15_MAPPINGf, 0xd);
        soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, 
                          PORT16_MAPPINGf, 0xd);
        SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_2r(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, SOFT_RESETf, 1);
        soc_reg_field_set(unit, IL_GLOBAL_CONTROLr, &rval, WC_CONFIG_IL_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IL_GLOBAL_CONTROLr(unit, 9, rval));
        SOC_IF_ERROR_RETURN(WRITE_IL_GLOBAL_CONTROLr(unit, 13, rval));
        rval = 0;
        /* Need to take out of reset only after Warpcores are initialized */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONTROLr, 9, SOFT_RESETf,
            0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONTROLr, 13, SOFT_RESETf,
            0));

        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM0f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM1f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM2f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM3f, 1);
        soc_reg_field_set(unit, IL_MEMORY_INITr, &rval, INIT_MEM4f, 1);
        SOC_IF_ERROR_RETURN(WRITE_IL_MEMORY_INITr(unit, 9, rval));
        SOC_IF_ERROR_RETURN(WRITE_IL_MEMORY_INITr(unit, 13, rval));
        do {
            SOC_IF_ERROR_RETURN(READ_IL_MEMORY_INIT_DONEr(unit, 9, &rval));
            if (soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM0f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM1f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM2f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM3f) && 
                soc_reg_field_get(unit, IL_MEMORY_INIT_DONEr, rval, INIT_DONE_MEM4f)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : IL_MEMORY_INIT_DONE timeout\n"), unit));
                break;
            }
        } while (TRUE);

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IL0_ENf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    IL1_ENf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                    CFG_TX_WC_BITFLIPf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                    CFG_RX_WC_BITFLIPf, 1));

        /* Configure IL Counters in accumulate mode;default is read on clear */
        if (soc_property_get(unit, spn_BCM_STAT_FLAGS, 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        }

        /* Set Inband flow control, reset val(0x1) is OOB for shadow */
        if (soc_property_get(unit, spn_BCM88732_USE_OOB, 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        CFG_FLOWCONTROL_TYPEf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        CFG_FLOWCONTROL_TYPEf, 1));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 9, 
                                        CFG_FLOWCONTROL_TYPEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        CFG_FLOWCONTROL_TYPEf, 0));
        }

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                    CFG_TX_WC_BITFLIPf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                    CFG_RX_WC_BITFLIPf, 1));

        /* Configure IL Counters in accumulate mode;default is read on clear */
        if (soc_property_get(unit, spn_BCM_STAT_FLAGS, 0)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_GLOBAL_CONFIGr, 13, 
                                        XGS_COUNTER_COMPAT_MODEf, 1));
        }
        break;
    default:
        break; /* No action */
    }


    /* Bypass ISEC and ESEC */
    PBMP_PORT_ITER(unit, port) {
        if (port > 8) {
            break;
        }
        rval = 0;
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, ACTIVATE_PD_CREDITSf, 1);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, EN_40Gf, en40g);
        SOC_IF_ERROR_RETURN(WRITE_ESEC_MASTER_CTRLr(unit, port, rval));
        rval = 0;
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, EN_40Gf, en40g);
        SOC_IF_ERROR_RETURN(WRITE_ISEC_MASTER_CTRLr(unit, port, rval));
        if ((port == 1) || (port ==5)) {
            rval = 0;
            soc_reg_field_set(unit, ISEC_GLOBAL_CTRLr, &rval, INITf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg32_write(unit, soc_reg_addr(unit, ISEC_GLOBAL_CTRLr, 
                                                    port, 0), rval));
            rval = 0;
            soc_reg_field_set(unit, ESEC_GLOBAL_CTRLr, &rval, INITf, 1);
            SOC_IF_ERROR_RETURN
                (soc_reg32_write(unit, soc_reg_addr(unit, ESEC_GLOBAL_CTRLr, 
                                                    port, 0), rval));
        }
    }

    /* Deal with XLPORT block */
    sal_memset(port_exist, 0, sizeof(port_exist));
    sal_memset(port_valid, 0, sizeof(port_valid));
    for (port = 1; port <= 16; port++) {
        blk = SOC_PORT_BLOCK(unit, port);
        bindex = SOC_PORT_BINDEX(unit, port);
        blk_num = SOC_BLOCK_INFO(unit, blk).number;
        port_exist[blk_num] |= 1 << bindex;
        if (!SOC_PORT_VALID(unit, port) || 
            SOC_PBMP_MEMBER(si->port.disabled_bitmap, port)) {
            continue;
        }
        port_valid[blk_num] |= 1 << bindex;
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        blk_num = SOC_BLOCK_INFO(unit, blk).number;
        port = SOC_BLOCK_PORT(unit, blk);
        if ((port < 0) || IS_IL_PORT(unit, port)) {
            continue;
        }
        /* Program XLPORT mode */
        if (((port_valid[blk_num] & 0xf) == 0xf) ||
            ((port_valid[blk_num] & 0x7) == 0x7)) { /* for TDM 3 */
            core_mode = 2; /* quad */
        } else if (((port_valid[blk_num] & 0x5) == 0x5) ||
                   ((port > 8) && (port_valid[blk_num] & 0x1) == 0x1)) {
            core_mode = 1; /* dual */
        } else {
            core_mode = 0; /* single */
        }

        if (si->port_speed_max[port] > 20000) {
            phy_mode = 0; /* 16 lanes */
        } else if (si->port_speed_max[port] > 10000) {
            phy_mode = 1; /* 8 lanes */
        } else {
            phy_mode = 2; /* 4 lanes */
        }
        /* TDM18/TDM19 XAUI mode has 4 lanes */
        if (((soc_property_get(unit, spn_BCM88732_4X10_4X10, 0) ||
            soc_property_get(unit, spn_BCM88732_1X40_4X10, 0))) && (port > 8)) {
            core_mode = 0; /* single port mode */
            phy_mode  = 2; /* 4 lanes          */
        }
        if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0) && (port > 8)) {
            core_mode = 1; /* Xlport-XMAC dual lane mode */
            phy_mode  = 2; /* 4 lanes          */
        }
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, CORE_PORT_MODEf,
                          core_mode);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, PHY_PORT_MODEf,
                          phy_mode);
        if (blk_num == 0) { /* XLPORT0 block may have GE port */
            if (si->port_speed_max[port] < 10000) {
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                                  PORT0_MAC_MODEf, 1);
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                                  PORT1_MAC_MODEf, 1);
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                                  PORT2_MAC_MODEf, 1);
                soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                                  PORT3_MAC_MODEf, 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* Bring XMAC out of reset */
        if (si->port_speed_max[port] >= 10000) {
            if (!SOC_PBMP_MEMBER(si->port.disabled_bitmap, port)) {
                SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, 0));
            }
        }

        /* Reset XLPORT MIB counter */
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf,
                          port_valid[blk_num]);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        for (bindex = 0; bindex < 4; bindex++) {
            if (port_valid[blk_num] & (1 << bindex)) {
                soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));
    }

    /* Initialize additional device mode configurations */
    if (soc_property_get(unit, spn_BCM88732_DEVICE_MODE, 0)) {
        SOC_IF_ERROR_RETURN(soc_shadow_petrab_misc_init(unit));
    }

    for (port = 1; port <= 16; port++) {
        if (!SOC_PORT_VALID(unit, port) || 
            SOC_PBMP_MEMBER(si->port.disabled_bitmap, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &rval));
        phy_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval, PHY_PORT_MODEf);
        if (phy_mode == 0 ) { /* Quad Lanes Port */
            si->port_num_lanes[port] = 4;
        } else if (phy_mode == 1 ) { /* DUAL Lane Port */
            si->port_num_lanes[port] = 2;
        } else {
            si->port_num_lanes[port] = 1; /* Independent lane port */
        }
    }


    /* Setup internal Priority to COSQ Mapping */
    for(port = 0; port <= 16; port++) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, COS_MAP_SELr, port, 
                                                   SELECTf, 0));
    }
    for(i = 0; i < 4; i++) {
        for (prio = 0; prio < 8; prio++) {
           SOC_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, PORT_COS_MAPm, (i*16)+prio, COSf, prio));
        }
    }

    /* Setup EXP_TABLE with default priorities */
    for(port = 0; port <= 16; port++) {
        for (prio = 0; prio < 8; prio++) {
           SOC_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, EXP_TABLEm, (port*8)+prio, PRIf, prio));
        }
    }

    soc_intr_enable(unit, IRQ_MEM_FAIL);
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY, 
                                PORT_BITMAPf, 0x1ffff));
    return SOC_E_NONE;
}

int
soc_shadow_petrab_mmu_init(int unit)
{   
    int         idx;
    uint32      rval;
    soc_port_t  p;
    uint32 cosw[8] = {0, 127, 64, 32, 24, 16, 8, 1};

    /* Disable watchdog in the Shadow MMU */
    PBMP_PORT_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, DEQ_AGINGMASKr, p, AGINGMASKf, 0xff));
    }

    /* Set port max pkt size */
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p,
                                    PORT_MAX_PKT_SIZEf, 128)); 
    }
	
    /* Reconcile CoS conventions used between Petra and shadow
     * Petra CoS=0 is highest priority traffic.
     */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }  

    /* Enable PFC */ 
    PBMP_XE_ITER(unit, p) {
        rval = 0;
        soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval, 
                          PORT_PRI_XON_ENABLEf, 0xff);
        soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval, 
                          PORT_XON_ENABLEf, 1);
        soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval, 
                          PORT_PG_PAUSE_DISABLEf, 8);
        SOC_IF_ERROR_RETURN
            (WRITE_PORT_PRI_XON_ENABLEr(unit, p, rval));
    }
    
    return SOC_E_NONE;
}

int
soc_shadow_petrab_misc_init(int unit)
{
    int         idx;
    int         port;
    uint32      rval;
    soc_port_t  p;
    fp_tcam_entry_t           tcam_entry;
    il_channel_remap0_entry_t remap0;
    il_channel_remap1_entry_t remap1;
    fp_policy_table_entry_t   policy_entry;
    uint32 field_values[4];

    /* Petra uses channel numbers 0 through 7 to specify port. 
     * CoS is determined by SCH header via the IFP 
     */
    for (idx = 0; idx <= 7; idx ++) {
        sal_memset(&remap0, 0, sizeof(remap0));
        sal_memset(&remap1, 0, sizeof(remap1));

        soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, PORT_NUMf, 
                                           9 + idx);
        soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap1, PORT_NUMf, 
                                           13);

        if (idx == 3) {
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, 3);
        } else {
            soc_IL_CHANNEL_REMAP0m_field32_set(unit, &remap0, COSf, 0);
        }

        SOC_IF_ERROR_RETURN
            (WRITE_IL_CHANNEL_REMAP0m(unit, MEM_BLOCK_ALL, idx, &remap0));
        SOC_IF_ERROR_RETURN
            (WRITE_IL_CHANNEL_REMAP1m(unit, MEM_BLOCK_ALL, idx, &remap1));
    }


    /* setup user defined field in IPARS */
    for (idx = 0; idx < 512; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, UDF_OFFSETm, idx, 
                                    BASE_OFFSET0f, 11));

        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, UDF_OFFSETm, idx, 
                                    OFFSET_VALUE0f, 0));
    }

    /* setup FP port fields */
    rval = 0;
    soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval,
                      FP_LOOKUP_ENABLE_SLICE_0f, 1);
    soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, 
                      FP_SLICE_ENABLE_SLICE_0f, 1);
    SOC_IF_ERROR_RETURN(WRITE_FP_SLICE_ENABLEr(unit, rval));

    for (idx = 9; idx <= 16; idx ++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, FP_PORT_FIELD_SELm, idx, 
                                    SLICE0_F1f, 0));

        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, FP_PORT_FIELD_SELm, idx, 
                                    SLICE0_F2f, 8));
    }

    /* setup FP_TCAM fields */
    sal_memset(&tcam_entry, 0, sizeof(tcam_entry));
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, IPBMf, 0x1fe00);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, IPBM_MASKf, 0x001ff);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, FIXED_MASKf, 0);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, F4_MASKf, 0);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, DOUBLE_WIDE_MODE_MASKf, 0);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, DOUBLE_WIDE_MODEf, 0);
    soc_FP_TCAMm_field32_set(unit, &tcam_entry, VALIDf, 3);

    /* Fields greater than 32bits */
    sal_memset(field_values, 0, sizeof(field_values));
    field_values[0] = 1;
    soc_FP_TCAMm_field_set(unit, &tcam_entry, F1f, field_values);
    field_values[0] = 1;
    soc_FP_TCAMm_field_set(unit, &tcam_entry, F1_MASKf, field_values);
    field_values[0] = 0;
    soc_FP_TCAMm_field_set(unit, &tcam_entry, F3_MASKf, field_values);
    
    sal_memset(field_values, 0, sizeof(field_values));
    field_values[3] = 0xe0; 
    soc_FP_TCAMm_field_set(unit, &tcam_entry, F2_MASKf, field_values);
   
    for (idx = 0; idx <= 7; idx++) {
        field_values[3] = 0x20 * idx; 
        soc_FP_TCAMm_field_set(unit, &tcam_entry, F2f, field_values);
        SOC_IF_ERROR_RETURN
            (WRITE_FP_TCAMm(unit, MEM_BLOCK_ALL, idx, &tcam_entry));
    }

    for (idx = 0; idx <= 7; idx++) {
        /* setup FP policy */
        sal_memset(&policy_entry, 0, sizeof(policy_entry));
        soc_FP_POLICY_TABLEm_field32_set(unit, &policy_entry, 
                                         G_CHANGE_COS_OR_INT_PRIf, 5);
        soc_FP_POLICY_TABLEm_field32_set(unit, &policy_entry, 
                                         G_COS_INT_PRIf, idx);
        SOC_IF_ERROR_RETURN
            (WRITE_FP_POLICY_TABLEm(unit, MEM_BLOCK_ALL, idx, &policy_entry));
    }

    /* Set Max pkt size */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L2_MTUr, port, 
                                    VALUEf, 16256)); 
    }

    /* vlan otag set to 0 */ 
    for (idx = 0; idx <  soc_mem_index_count(unit, ING_VLAN_TAG_ACTION_PROFILEm); idx ++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ING_VLAN_TAG_ACTION_PROFILEm, idx, 
                                    SOT_OTAG_ACTIONf, 0));
    }

    /* xlport config */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, PFC_ENABLEf, 1);
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, XPAUSE_RX_ENf, 0);
    soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, XPAUSE_TX_ENf, 0);
    PBMP_XE_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
    }

    /* Configure Static balance */
    if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0)) {
        /* uflow set for Xe0 -> port 10 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                        DEST_PORTf, 9)); 

        /* uflow set for Xe1 -> port 13 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                        DEST_PORTf, 13)); 

        /* Static balance for Switch ports : (1->[9,10,11,12] 5->[13,14,15,16]) */
        for (p = 9; p <= 16; p++) {
            idx = ((p-8)/5)?5:1;
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, idx)); 
        }
    } else if (soc_property_get(unit, spn_BCM88732_8X10_8X10, 0)) {
        for (p = 1; p <= 8; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 8+p)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 8+p, 
                                        DEST_PORTf, p)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 8+p, 
                                        HASH_MODEf, 0));
        }
    } else if ((soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) ||
               (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0))) {

        /* uflow set for Xe0 -> port 10 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                        DEST_PORTf, 9)); 

        /* uflow set for Xe1 -> port 13 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                        DEST_PORTf, 13)); 

        /* Static balance for Switch ports : (1->[9,10,11,12] 5->[13,14,15,16]) */
        for (p = 9; p <= 16; p++) {
            idx = ((p-8)/5)?5:1;
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, idx)); 
        }

    } else if ((soc_property_get(unit, spn_BCM88732_8X10_1X40, 0)) ||
              (!(soc_property_get(unit, spn_BCM88732_1X40_4X10, 0) ||
                 soc_property_get(unit, spn_BCM88732_4X10_4X10, 0) ||
                 soc_property_get(unit, spn_BCM88732_8X10_8X10, 0) ||
                 soc_property_get(unit, spn_BCM88732_2X40_8X10, 0)))) {
        /* Static balance for interlaken ports : (1->9, 2->10 ... 8->16) */
        for (p = 1; p <= 8; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 8+p)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 8+p, 
                                        DEST_PORTf, p)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 8+p, 
                                        HASH_MODEf, 0));
        }
    }

    /* Enable all ILKN channels: */
    PBMP_IL_ITER(unit, p) {
        if ((p == 9) || (p == 13)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_RX_CHAN_ENABLE0r, p, 
                                        IL_RX_CHAN_ENABLE0f, 0xffffffff));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_TX_CHAN_ENABLE0r, p, 
                                        IL_TX_CHAN_ENABLE0f, 0xffffffff));
            
            /* Set Pkt Max Size */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_RX_CONFIGr, p,
                                        IL_RX_MAX_PACKET_SIZEf, 16256)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_TX_CONFIGr, p,
                                        IL_TX_MAX_PACKET_SIZEf, 16256)); 

            /* burst shot to 64B */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_TX_CORE_CONFIG0r, p, 
                                        CTL_TX_BURSTSHORTf, 1));
            
            /* IL MU Link Level Flow-Control */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_MU_LLFC_CONTROLr, p, 
                                        RX_ENABLEf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_MU_LLFC_CONTROLr, p, 
                                        RX_POLARITYf, 0));

            /* Turn off CRC32 check */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_TX_IEEE_CRC32_GEN_OFFSET16f, 0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_RX_IEEE_CRC32_CHECK_OFFSET16f, 0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_TX_IEEE_CRC32_GEN_STRIP_ENf, 0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_TX_IEEE_CRC32_GEN_APPEND_ENf, 0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_RX_IEEE_CRC32_STRIP_ENf, 0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IL_IEEE_CRC32_CONFIGr, p, 
                                        CFG_RX_IEEE_CRC32_CHECK_ENf, 0));
        }
    }
    return SOC_E_NONE;
}



/*
 * MMU Config for TDM0_XFI
 * 2x40G to 8X10G_XFI
 */
STATIC
int _shadow_tdm0_xfi_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[30] = 
                        {0x2,    0x200,   0x20,   0x2000, 0x400,   0x2,
                         0x4000, 0x20,    0x800,  0x8000, 0x2,     0x1000,
                         0x20,   0x10000, 0x1,    0x2,    0x200,   0x20,
                         0x2000, 0x400,   0x2,    0x4000, 0x20,    0x800,
                         0x8000, 0x2,     0x1000, 0x20,   0x10000, 0x0};
    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    uint32 cosw[8] = {1, 16, 32, 64, 127, 0, 0, 0};

    uint32 rval;
    uint32 val = 0;

    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               CFAPPOOLSIZEf, 98304));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        for (idx = 0; idx <= 9; idx++) {
            SOC_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, 
                              Q_SHARED_LIMITf, 98304);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 54728));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        if (p <= 8) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 62304));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 3200));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_RESET_OFFSETr, p, 
                                    PG_RESET_OFFSETf, 10));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_MINr, p, PG_MINf, 8));

        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            if (p <= 8) {
                val = 1133;
            } else {
                val = 275;
            }
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, val);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 8));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 1000));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG_MINr, 0, PG_MINf, 0));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, 0, 
                                PORT_MAX_PKT_SIZEf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, 0, 
                                    PORT_XON_ENABLEf, 1));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 
                                REG_PORT_ANY, TOTAL_SHARED_AVAIL_THRESHf, 54472));

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                LB_TIME_DOMAINf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN1f, 1));
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_WEIGHTf, 4));
        if (p <= 8) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
        }
    }

    /* ES */
    PBMP_PORT_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, MTU_QUANTA_SELECTf, 2));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 30; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 59));
    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    THRESHOLDf, 544));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_1r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_1_8f, 10));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_2r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_1_8f, 256));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 10));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));
    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));

    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));

    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));

    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));

    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));

    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));

    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));

    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));

    return SOC_E_NONE;
}

/*
 * MMU Config for TDM1-XFI
 * 8x10G to 8X10G
 */
STATIC
int _shadow_tdm1_xfi_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[60] = 
                        {0x2,   512,   32,   8192,  1024,  0x4,
                         16384, 64,    2048, 32768, 8,     4096,
                         128,   65536, 1,    0x10,  512,   256,
                         8192,  1024,  2,    16384, 0x20,  2048,
                         32768, 4,     4096, 64,    65536, 0x0,
                         0x8,   512,   128,  8192,  1024,  0x10,
                         16384, 0x100, 2048, 32768, 0x2,   4096,
                         0x20,  65536, 0x1,  0x4,   512,   0x40,
                         8192,  1024,  0x8,  16384, 0x80,  2048,
                         32768, 16,    4096, 256,   65536, 0x0};

    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 0, 1};
    uint32 cosw[8] = {1, 16, 32, 64, 127, 0, 0, 0};

    uint32 rval;
    uint32 val = 0;

    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               CFAPPOOLSIZEf, 98304));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        for (idx = 0; idx <= 9; idx++) {
            SOC_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, 
                              Q_SHARED_LIMITf, 98304);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 54728));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));

        val = (p <= 8) ? 15576 : 3200;
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_RESET_OFFSETr, p, 
                                    PG_RESET_OFFSETf, 10));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_MINr, p, PG_MINf, 8));

        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            val = (p <= 8) ? 283 : 275;
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, val);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 8));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 1000));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 1));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 
                                REG_PORT_ANY, TOTAL_SHARED_AVAIL_THRESHf,
                                54472));

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                LB_TIME_DOMAINf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN1f, 1));
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_WEIGHTf, 4));
        if (p <= 8) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
        }
    }

    /* ES */
    PBMP_PORT_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 60; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                CURRENT_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                SWITCH_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_1r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_1_8f, 10));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_2r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_1_8f, 256));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 10));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));

    return SOC_E_NONE;
}



/*
 * MMU Configurations for TDM5
 * 2x40G to 2x40G(Interlaken)
 * Refered to SOC script : radian_init_88732_2X40G+2X40G_IL.soc
 */

STATIC
int _shadow_tdm5_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t wredcfg_cell[7] = {MAXDROPRATEf, YELLOW_MAXDROPRATEf, 
                                   RED_MAXDROPRATEf, PRI0_MAXDROPRATEf,
                                   WEIGHTf, CAP_AVERAGEf, ENABLEf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[30] = 
                        {0x2,     0x1e00,  0x20,    0x1e000, 0x0,     0x2,
                         0x0,     0x20,    0x1e000, 0x0,     0x2,     0x1e00,
                         0x20,    0x1e000, 0x1,     0x2,     0x1e00,  0x0,
                         0x1e000, 0x0,     0x2,     0x1e00,  0x20,    0x1e000,
                         0x0,     0x2,     0x1e00,  0x20,    0x1e000, 0x0};

    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 wredcfg_cell_val[7] = {10, 10, 10, 10, 4, 1, 1};
    uint32 cosw[8] = {1, 8, 16, 24, 32, 64, 127, 0};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;
    uint64 rval64;


    /* Some XMAC related stuff: this needs to be moved to xmac.c */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, p, &rval64));
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, XLGMII_ALIGN_ENBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, p, rval64));
    }
    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        for (idx = 0; idx < 10; idx++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, Q_SHARED_LIMITf, 98304);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 61278));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {4, 4, 4, 4, 4, 4, 4, 4};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 24504));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 1024);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 1024);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    PBMP_IL_ITER(unit, p) {
        uint32 il_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 6072));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, il_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 8);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 256);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
    }
    for (idx = 0; idx < 8; idx++) {
        SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, 0, idx, &rval));
        soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
        SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, 0, idx, rval));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, 0, 
                                PORT_XON_ENABLEf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 0, 
                                TOTAL_SHARED_AVAIL_THRESHf, 61022));

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                LB_TIME_DOMAINf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN1f, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN0f, 1));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_WEIGHTf, 4));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
    }
    PBMP_XE_ITER(unit, p) {
        for (idx = 0; idx < 8; idx++) {
            int j;
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_START_CELLr, &rval,
                              DROPSTARTPOINTf, 2860);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_END_CELLr, &rval,
                              DROPENDPOINTf, 7150);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDCONFIG_CELLr(unit, p, idx, &rval));
            for (j = 0; j < 7; j++) {
                soc_reg_field_set(unit, WREDCONFIG_CELLr, &rval,
                                  wredcfg_cell[j], wredcfg_cell_val[j]);
            }
            SOC_IF_ERROR_RETURN(WRITE_WREDCONFIG_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_START_CELLr, &rval,
                              YELLOW_DROPSTARTPOINTf, 2043);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_END_CELLr, &rval,
                              YELLOW_DROPENDPOINTf, 5720);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_START_CELLr, &rval,
                              RED_DROPSTARTPOINTf, 1226);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_END_CELLr, &rval,
                              RED_DROPENDPOINTf, 4290);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_START_CELLr, &rval,
                              PRI0_DROPSTARTPOINTf, 817);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_END_CELLr, &rval,
                              PRI0_DROPENDPOINTf, 3575);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_END_CELLr(unit, p, idx, rval));
        }
    }

    /* ES */
    PBMP_PORT_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 30; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                CURRENT_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 29));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 29));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                SWITCH_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ES_PORTGRP_WDRR_WEIGHTSr, p, 
                                    PORTWEIGHTSf, 1));
    }

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    METER_GRANf, 5));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT10_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT11_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT12_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_2r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT14_MAPPINGf, 0xd);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT15_MAPPINGf, 0xd);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT16_MAPPINGf, 0xd);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_2r(unit, rval));

    return SOC_E_NONE;
}

/*
 * MMU Configurations for TDM6
 * 2x40G to 40G(Interlaken)
 * Refered to SOC script : radian_init_88732_2X40G+1X40G_IL.soc
 */

STATIC
int _shadow_tdm6_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t wredcfg_cell[7] = {MAXDROPRATEf, YELLOW_MAXDROPRATEf, 
                                   RED_MAXDROPRATEf, PRI0_MAXDROPRATEf,
                                   WEIGHTf, CAP_AVERAGEf, ENABLEf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[30] = 
                        {0x2,     0x1fe00, 0x20,    0x0,     0x0,     0x2,
                         0x1fe00, 0x20,    0x0,     0x0,     0x2,     0x1fe00,
                         0x20,    0x0,     0x1,     0x2,     0x1fe00, 0x20,
                         0x0,     0x0,     0x2,     0x1fe00, 0x20,    0x0,
                         0x0,     0x2,     0x1fe00, 0x20,    0x0,     0x0};

    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 wredcfg_cell_val[7] = {10, 10, 10, 10, 4, 1, 1};
    uint32 cosw[8] = {1, 8, 16, 24, 32, 64, 127, 0};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;

    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        for (idx = 0; idx < 10; idx++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, Q_SHARED_LIMITf, 98304);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 61278));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {4, 4, 4, 4, 4, 4, 4, 4};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 24504));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 1024);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf,
                              1024);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    PBMP_IL_ITER(unit, p) {
        uint32 il_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 6072));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, il_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 8);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 256);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 0, 
                                TOTAL_SHARED_AVAIL_THRESHf, 61022));

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                LB_TIME_DOMAINf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN1f, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN0f, 1));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_WEIGHTf, 4));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
    }
    PBMP_XE_ITER(unit, p) {
        for (idx = 0; idx < 8; idx++) {
            int j;
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_START_CELLr, &rval,
                              DROPSTARTPOINTf, 2860);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_END_CELLr, &rval,
                              DROPENDPOINTf, 7150);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDCONFIG_CELLr(unit, p, idx, &rval));
            for (j = 0; j < 7; j++) {
                soc_reg_field_set(unit, WREDCONFIG_CELLr, &rval,
                                  wredcfg_cell[j], wredcfg_cell_val[j]);
            }
            SOC_IF_ERROR_RETURN(WRITE_WREDCONFIG_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_START_CELLr, &rval,
                              YELLOW_DROPSTARTPOINTf, 2043);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_END_CELLr, &rval,
                              YELLOW_DROPENDPOINTf, 5720);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_START_CELLr, &rval,
                              RED_DROPSTARTPOINTf, 1226);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_END_CELLr, &rval,
                              RED_DROPENDPOINTf, 4290);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_START_CELLr, &rval,
                              PRI0_DROPSTARTPOINTf, 817);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_END_CELLr, &rval,
                              PRI0_DROPENDPOINTf, 3575);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_END_CELLr(unit, p, idx, rval));
        }
    }

    /* ES */
    PBMP_PORT_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 30; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                CURRENT_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 0x1d));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 0x1d));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                SWITCH_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ES_PORTGRP_WDRR_WEIGHTSr, p, 
                                    PORTWEIGHTSf, 1));
    }

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    METER_GRANf, 5));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT10_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT11_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT12_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_2r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT13_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT14_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT15_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT16_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_2r(unit, rval));

    return SOC_E_NONE;
}

/*
 * Shadow MMU Configuration
 * MMU configurations for each TDM mode
 */

STATIC
int _shadow_tdm7_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t wredcfg_cell[7] = {MAXDROPRATEf, YELLOW_MAXDROPRATEf, 
                                   RED_MAXDROPRATEf, PRI0_MAXDROPRATEf,
                                   WEIGHTf, CAP_AVERAGEf, ENABLEf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 wredcfg_cell_val[7] = {10, 10, 10, 10, 4, 1, 1};
    uint32 cosw[8] = {1, 8, 16, 24, 32, 64, 127, 0};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;

    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_QUEUE_CONFIGUr, p, 
                                    Q_SHARED_LIMITf, 98304));
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 66222));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {4, 4, 4, 4, 4, 4, 4, 4};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 7150));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_RESET_OFFSETr, p, 
                                    PG_RESET_OFFSETf, 128));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 256);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    PBMP_IL_ITER(unit, p) {
        uint32 il_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 1112));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, il_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 256));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_RESET_OFFSETr, p, 
                                    PG_RESET_OFFSETf, 36));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 8);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 108);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN0f, 1));
    PBMP_XE_ITER(unit, p) {
        for (idx = 0; idx < 8; idx++) {
            int j;
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_START_CELLr, &rval,
                              DROPSTARTPOINTf, 715);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_END_CELLr, &rval,
                              DROPENDPOINTf, 1915);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDCONFIG_CELLr(unit, p, idx, &rval));
            for (j = 0; j < 7; j++) {
                soc_reg_field_set(unit, WREDCONFIG_CELLr, &rval,
                                  wredcfg_cell[j], wredcfg_cell_val[j]);
            }
            SOC_IF_ERROR_RETURN(WRITE_WREDCONFIG_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_START_CELLr, &rval,
                              YELLOW_DROPSTARTPOINTf, 510);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_END_CELLr, &rval,
                              YELLOW_DROPENDPOINTf, 1532);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_START_CELLr, &rval,
                              RED_DROPSTARTPOINTf, 306);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_END_CELLr, &rval,
                              RED_DROPENDPOINTf, 1149);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_START_CELLr, &rval,
                              PRI0_DROPSTARTPOINTf, 204);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_END_CELLr, &rval,
                              PRI0_DROPENDPOINTf, 957);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_END_CELLr(unit, p, idx, rval));
        }
    }

    /* ES */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));

        }
    }   
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ES_PORTGRP_WDRR_WEIGHTSr, p, 
                                    PORTWEIGHTSf, 3));
    }

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    METER_GRANf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT10_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT11_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT12_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_2r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT14_MAPPINGf, 0xd);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT15_MAPPINGf, 0xd);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT16_MAPPINGf, 0xd);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_2r(unit, rval));

    return SOC_E_NONE;
}


/*
 * MMU Configurations for TDM8
 * 8x10G to 40G(Interlaken)
 * Refered to SOC script : radian_init_88732_8X10G+1X40G_IL.soc
 */

STATIC
int _shadow_tdm8_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t wredcfg_cell[7] = {MAXDROPRATEf, YELLOW_MAXDROPRATEf, 
                                   RED_MAXDROPRATEf, PRI0_MAXDROPRATEf,
                                   WEIGHTf, CAP_AVERAGEf, ENABLEf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[60] = 
                        {0x2,     0x1fe00, 0x20,    0x0,     0x0,     0x4,
                         0x1fe00, 0x40,    0x0,     0x0,     0x8,     0x1fe00,
                         0x80,    0x0,     0x1,     0x10,    0x1fe00, 0x100,
                         0x0,     0x0,     0x2,     0x1fe00, 0x20,    0x0,
                         0x0,     0x4,     0x1fe00, 0x40,    0x0,     0x0,
                         0x8,     0x1fe00, 0x80,    0x0,     0x0,     0x10,
                         0x1fe00, 0x100,   0x0,     0x0,     0x2,     0x1fe00,
                         0x20,    0x0,     0x1,     0x4,     0x1fe00, 0x40,
                         0x0,     0x0,     0x8,     0x1fe00, 0x80,    0x0,
                         0x0,     0x10,    0x1fe00, 0x100,   0x0,     0x0};


    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 wredcfg_cell_val[7] = {10, 10, 10, 10, 4, 1, 1};
    uint32 cosw[8] = {1, 8, 16, 24, 32, 64, 127, 0};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;

    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 98304));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 98304));
        for (idx = 0; idx < 10; idx++) {
            SOC_IF_ERROR_RETURN(READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, Q_SHARED_LIMITf, 98304);
            SOC_IF_ERROR_RETURN(WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 66222));
    PBMP_XE_ITER(unit, p) {
        uint32 xe_val[8] = {4, 4, 4, 4, 4, 4, 4, 4};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 7150));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 512));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 128);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 256);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    PBMP_IL_ITER(unit, p) {
        uint32 il_val[8] = {2, 2, 2, 2, 2, 2, 2, 2};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 72));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 1112));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, il_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 256));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_RESET_OFFSETr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_RESET_OFFSETr, &rval, PG_RESET_OFFSETf,
                              36);
            SOC_IF_ERROR_RETURN(WRITE_PG_RESET_OFFSETr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_MINr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_MINr, &rval, PG_MINf, 8);
            SOC_IF_ERROR_RETURN(WRITE_PG_MINr(unit, p, idx, rval));
        }
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 108);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));

        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PRI_XON_ENABLEf, 0x3fff));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 0, 
                                TOTAL_SHARED_AVAIL_THRESHf, 65966));

    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN0f, 1));
    PBMP_XE_ITER(unit, p) {
        for (idx = 0; idx < 8; idx++) {
            int j;
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_START_CELLr, &rval,
                              DROPSTARTPOINTf, 715);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_END_CELLr, &rval,
                              DROPENDPOINTf, 1915);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_END_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDCONFIG_CELLr(unit, p, idx, &rval));
            for (j = 0; j < 7; j++) {
                soc_reg_field_set(unit, WREDCONFIG_CELLr, &rval,
                                  wredcfg_cell[j], wredcfg_cell_val[j]);
            }
            SOC_IF_ERROR_RETURN(WRITE_WREDCONFIG_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_START_CELLr, &rval,
                              YELLOW_DROPSTARTPOINTf, 510);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_START_CELLr(unit, p, idx, rval));

            SOC_IF_ERROR_RETURN(READ_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_YELLOW_END_CELLr, &rval,
                              YELLOW_DROPENDPOINTf, 1532);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_YELLOW_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_START_CELLr, &rval,
                              RED_DROPSTARTPOINTf, 306);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_RED_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_RED_END_CELLr, &rval,
                              RED_DROPENDPOINTf, 1149);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_RED_END_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_START_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_START_CELLr, &rval,
                              PRI0_DROPSTARTPOINTf, 204);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_START_CELLr(unit, p, idx, rval));
            SOC_IF_ERROR_RETURN(READ_WREDPARAM_PRI0_END_CELLr(unit, p, idx, &rval));
            soc_reg_field_set(unit, WREDPARAM_PRI0_END_CELLr, &rval,
                              PRI0_DROPENDPOINTf, 957);
            SOC_IF_ERROR_RETURN(WRITE_WREDPARAM_PRI0_END_CELLr(unit, p, idx, rval));
        }
    }

    /* ES */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p,  MTU_QUANTA_SELECTf, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, SCHEDULING_SELECTf, 3));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, p, idx, &rval));
            soc_reg_field_set(unit, COSWEIGHTSr, &rval, COSWEIGHTSf, cosw[idx]);
            SOC_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, p, idx, rval));
        }
    }   
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 30; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                CURRENT_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                SWITCH_CALENDARf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));
    PBMP_IL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ES_PORTGRP_WDRR_WEIGHTSr, p, 
                                    PORTWEIGHTSf, 1));
    }

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    METER_GRANf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_1r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT10_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT11_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_1r, &rval, PORT12_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_1r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_OP_EP_PORT_MAPPING_TABLE_2r(unit, &rval));
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT13_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT14_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT15_MAPPINGf, 9);
    soc_reg_field_set(unit, OP_EP_PORT_MAPPING_TABLE_2r, &rval, PORT16_MAPPINGf, 9);
    SOC_IF_ERROR_RETURN(WRITE_OP_EP_PORT_MAPPING_TABLE_2r(unit, rval));

    return SOC_E_NONE;
}

/*
 * MMU Config for TDM18
 * 1X40G to 4X10G
 */
STATIC
int _shadow_tdm18_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[30] = 
                        {0x2,    0x200,   0x20,   0x2000, 0x400,   0x2,
                         0x4000, 0x20,    0x800,  0x8000, 0x2,     0x1000,
                         0x20,   0x10000, 0x1,    0x2,    0x200,   0x20,
                         0x2000, 0x400,   0x2,    0x4000, 0x20,    0x800,
                         0x8000, 0x2,     0x1000, 0x20,   0x10000, 0x0};
    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;


    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               CFAPPOOLSIZEf, 98304));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, OP_BUFFER_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               OP_BUFFER_SHARED_LIMITf, 32768));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, OP_PORT_CONFIGUr, p, 
                                    OP_SHARED_LIMITf, 32768));
        for (idx = 0; idx <= 9; idx++) {
            SOC_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIGUr(unit, p, idx, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIGUr, &rval, Q_SHARED_LIMITf,
                              32768);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIGUr(unit, p, idx, rval));
        }
    }

    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 61278));

    PBMP_ALL_ITER(unit, p) {
        uint32 xe_val[8] = {15, 15, 15, 15, 15, 15, 15, 15};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 75));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_HDRM_LIMITr, p, 
                                    PG_HDRM_LIMITf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 1, PORT_MINf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));

    for (p = 9; p < 16; p+=2) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PG_HDRM_LIMITr, p, 
                                    PG_HDRM_LIMITf, 216));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                PORT_SHARED_LIMITf, 8192));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, 0, 
                                PORT_MAX_PKT_SIZEf, 75));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, 0, 
                                    PORT_XON_ENABLEf, 1));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 1, 
                                PORT_RESET_OFFSETf, 384));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 5, 
                                PORT_RESET_OFFSETf, 0));
    for (idx = 0; idx < 8; idx++) {
        SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, 1, idx, &rval));
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 1024);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, 1, idx, rval));
    }

    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 
                                REG_PORT_ANY, TOTAL_SHARED_AVAIL_THRESHf,
                                61022));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG0_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG0_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG1_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG1_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG2_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG2_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG3_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG3_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG4_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG4_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG5_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG5_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG6_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG6_HDRM_LIMIT_OFFSETf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG7_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG7_HDRM_LIMIT_OFFSETf, 128));

    SOC_IF_ERROR_RETURN(WRITE_PORT_HDRM_ENABLEr(unit, 0));


    /* WRED */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                LB_TIME_DOMAINf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TIME_DOMAIN_CONFIGr, REG_PORT_ANY, 
                                TIME_DOMAIN1f, 1));
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_WEIGHTf, 4));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
    }

    /* ES */
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, MTU_QUANTA_SELECTf, 2));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 43522));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 43522));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 30; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 29));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 29));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));

    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    METER_GRANf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, p, 
                                    THRESHOLDf, 544));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, p, 
                                    REFRESHf, 156250));
    }

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));

    return SOC_E_NONE;
}

/*
 * MMU Config for TDM19
 * 4x10G to 4X10G
 */
STATIC
int _shadow_tdm19_mmu_init(int unit)
{
    soc_port_t p;
    int idx;
    soc_field_t pg_thresh[8] = {PG0_THRESH_SELf, PG1_THRESH_SELf, 
                                PG2_THRESH_SELf, PG3_THRESH_SELf,
                                PG4_THRESH_SELf, PG5_THRESH_SELf,
                                PG6_THRESH_SELf, PG7_THRESH_SELf};
    soc_field_t pri_grp0[7] = {PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                               PRI4_GRPf, PRI5_GRPf, PRI6_GRPf};
    soc_field_t pri_grp1[7] = {PRI7_GRPf, PRI8_GRPf, PRI9_GRPf, PRI10_GRPf,
                               PRI11_GRPf, PRI12_GRPf, PRI13_GRPf};
    soc_field_t aging0_fld[8] = {AGINGLIMITPRI7f, AGINGLIMITPRI6f,
                                 AGINGLIMITPRI5f, AGINGLIMITPRI4f,
                                 AGINGLIMITPRI3f, AGINGLIMITPRI2f,
                                 AGINGLIMITPRI1f, AGINGLIMITPRI0f};
    soc_field_t aging1_fld[8] = {AGINGLIMITPRI15f, AGINGLIMITPRI14f,
                                 AGINGLIMITPRI13f, AGINGLIMITPRI12f,
                                 AGINGLIMITPRI11f, AGINGLIMITPRI10f,
                                 AGINGLIMITPRI9f, AGINGLIMITPRI8f};
    uint32 es_arb_tdm_table[60] = 
                        {0x2,  0x600,  0x20,    0,     0x6000, 0x4,
                         0,    0x40,   0x1800,  0,     0x8,    0x18000,
                         0x80, 0,      1,       0x10,  0x600,  0x100,
                         0,    0x6000, 2,       0,     0x20,   0x1800,
                         0,    0x4,    0x18000, 0x40,  0,      0x0,
                         0x8,  0x600,  0x80,    0,     0x6000, 0x10,
                         0,    0x100,  0x1800,  0,     0x2,    0x18000,
                         0x20, 0,      0x1,     0x4,   0x600,  0x40,
                         0,    0x6000, 0x8,     0,     0x80,   0x1800,
                         0,    0x10,   0x18000, 0x100, 0,      0x0};

    uint32 pri_grp0_val[7] = {0, 1, 2, 3, 4, 5, 6};
    uint32 pri_grp1_val[7] = {7, 7, 7, 7, 7, 7, 7};
    uint32 aging0_val[8] = {1, 1, 5, 5, 5, 5, 5, 5};
    uint32 aging1_val[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    uint32 rval;


    /* CFAP */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               START_CFAP_INITf, 1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPCONFIGr, REG_PORT_ANY,
                                               CFAPPOOLSIZEf, 98304));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD0r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLSETPOINTf, 98160));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CFAPFULLTHRESHOLD1r, 
                                               REG_PORT_ANY, 
                                               CFAPFULLRESETPOINTf, 98016));

    /* THDO */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDO_BYPASSr, REG_PORT_ANY,
                                               OUTPUT_THRESHOLD_BYPASSf, 1));
    /* THDI */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOTAL_SHARED_LIMITr, 
                                               REG_PORT_ANY, 
                                               TOTAL_SHARED_LIMITf, 61278));
    for (p = 1; p <= 4; p++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 8192));
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf,
                              1024);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));
        }
    }
    for (p = 1; p <= 8; p++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 128));
    }
    for (p = 9; p <= 16; p++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, p, 
                                    PORT_SHARED_LIMITf, 2048));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, p, 
                                    PORT_RESET_OFFSETf, 72));
    }
    for (p = 9; p < 16; p+=2) {
        for (idx = 0; idx < 8; idx++) {
            SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, p, idx, &rval));
            soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf,
                              384);
            SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, p, idx, rval));
        }
    }
    PBMP_ALL_ITER(unit, p) {
        uint32 xe_val[8] = {15, 15, 15, 15, 15, 15, 15, 15};
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, p, PORT_MINf, 32));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MAX_PKT_SIZEr, p, 
                                    PORT_MAX_PKT_SIZEf, 75));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PG_THRESH_SELr, p, 8,
                                    pg_thresh, xe_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_XON_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_PRI_XON_ENABLEr, p, 
                                    PORT_PG_PAUSE_DISABLEf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, 0, PORT_MINf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, 0, 
                                PORT_SHARED_LIMITf, 126));    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, 0, 
                                PORT_RESET_OFFSETf, 16));
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP0r, p, 7, 
                                     pri_grp0, pri_grp0_val));
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, PORT_PRI_GRP1r, p, 7, 
                                     pri_grp1, pri_grp1_val));
    }    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOTAL_SHARED_AVAIL_THRESHr, 
                                REG_PORT_ANY, TOTAL_SHARED_AVAIL_THRESHf,
                                61022));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG0_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG0_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG1_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG1_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG2_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG2_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG3_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG3_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG4_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG4_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG5_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG5_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG6_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG6_HDRM_LIMIT_OFFSETf, 72));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PG7_HDRM_LIMIT_OFFSETr, 
                                REG_PORT_ANY, PG7_HDRM_LIMIT_OFFSETf, 72));


    /* MTRO */
    PBMP_XE_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, 
                                    p, METER_GRANf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, 
                                    p, THRESHOLDf, 1056));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, 
                                    p, REFRESHf, 156250));
    }

    /* WRED */
    for (p = 1; p <= 16; p++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, p, 
                                    LB_ENABLEf, 1));
    }

    /* ES */
    PBMP_ALL_ITER(unit, p) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ESCONFIGr, p, MTU_QUANTA_SELECTf, 2));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 43551));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 43551));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    for (idx = 0; idx < 60; idx++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                PORT_VECTORf, es_arb_tdm_table[idx]));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_ENf, 0));
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, ES_ARB_TDM_TABLEm, idx, 
                                SHAPER_IDf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORTGRP_WDRR_CONFIGr, REG_PORT_ANY, 
                                PG_WDRR_MTU_QUANTA_SELECTf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Af, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_CAL_CFGr, REG_PORT_ANY, 
                                END_Bf, 59));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ES_TDM_ENr, REG_PORT_ANY, 
                                ENf, 1));

    /* INTF */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_3r, REG_PORT_ANY, 
                                BW_TIMER_VALUE_9_16f, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MMU_LLFC_TX_CONFIG_4r, REG_PORT_ANY, 
                                XOFF_REFRESH_TIME_9_16f, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI15r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI14r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI13r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI12r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI11r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI10r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI9r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI8r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI7r, REG_PORT_ANY, 
                                COS0_7_BMPf, 128));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI6r, REG_PORT_ANY, 
                                COS0_7_BMPf, 64));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI5r, REG_PORT_ANY, 
                                COS0_7_BMPf, 32));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI4r, REG_PORT_ANY, 
                                COS0_7_BMPf, 16));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI3r, REG_PORT_ANY, 
                                COS0_7_BMPf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI2r, REG_PORT_ANY, 
                                COS0_7_BMPf, 4));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI1r, REG_PORT_ANY, 
                                COS0_7_BMPf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PRIO2COS_0_PRI0r, REG_PORT_ANY, 
                                COS0_7_BMPf, 1));

    /* AGING */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PKTAGINGTIMERr, REG_PORT_ANY, 
                                DURATIONSELECTf, 4096));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT0r, REG_PORT_ANY, 8, 
                                 aging0_fld, aging0_val));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, PKTAGINGLIMIT1r, REG_PORT_ANY, 8, 
                                 aging1_fld, aging1_val));

    /* CTR */
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 7, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ALL_DROP_ACCEPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 6, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ENQ_DEQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 5, rval));
    rval = 0;
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, COSf, 15);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, DST_PORTf, 31);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, SRC_PORTf, 31);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 4, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 3, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 2, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 3);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 1, rval));
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, CNGf, 7);
    soc_reg_field_set(unit, TX_CNT_CONFIGr, &rval, ECN_MARKEDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TX_CNT_CONFIGr_REG32(unit, 0, rval));

    return SOC_E_NONE;
}






int
soc_shadow_mmu_init(int unit)
{
    /* Configure MMU based on the TDM */
    if (soc_property_get(unit, spn_BCM88732_1X40_4X10, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm18_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_4X10_4X10, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm19_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm5_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_2X40_1X40, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm6_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_8X10_1X40, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm8_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_8X10_2X40, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm7_mmu_init(unit));
    } else if (soc_property_get(unit, spn_BCM88732_8X10_8X10, 0)) {
        SOC_IF_ERROR_RETURN(_shadow_tdm1_xfi_mmu_init(unit));
    } else { /* if (soc_property_get(unit, spn_BCM88732_2X40_8X10, 0)) */
        /* Default is 2x40 - 8x10 TDM */
        SOC_IF_ERROR_RETURN(_shadow_tdm0_xfi_mmu_init(unit));
    }

    /* Initialize additional device mode MMU configurations */
    if (soc_property_get(unit, spn_BCM88732_DEVICE_MODE, 0)) {
        SOC_IF_ERROR_RETURN(soc_shadow_petrab_mmu_init(unit));
    }

    return SOC_E_NONE;
}

void soc_shadow_mmu_parity_error(void *unit, void *d1, void *d2,
                             void *d3, void *d4 )
{
    int u = PTR_TO_INT(unit);

    COMPILER_REFERENCE(d1);
    COMPILER_REFERENCE(d2);
    COMPILER_REFERENCE(d3);
    COMPILER_REFERENCE(d4);
    
    soc_intr_enable(u, IRQ_MEM_FAIL);
}

int
soc_shadow_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_shadow_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_shadow_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

/* 
 * To support dynamic port configurations
 */
int 
_bcm_shadow_port_reconfig_40g(int unit, soc_port_t port)
{
    uint64 mac_ctrl, rval64;
    uint32 rval, rval1, rval2, rval3;
    int count=0, i, p;
    soc_info_t          *si;
    soc_field_t field_ids[1];
    uint32 field_values[4];
    port_tab_entry_t ptab;

    if (port != 1 && port != 5) {
        return SOC_E_PARAM;
    }

    si = &SOC_INFO(unit);
    /* Drain packets enqueued to port */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 1));
    do {
        sal_usleep(10000);
        SOC_IF_ERROR_RETURN(READ_PORT_COUNTr(unit, port+8, &rval));
        SOC_IF_ERROR_RETURN(READ_PORT_COUNTr(unit, port+9, &rval1));
        SOC_IF_ERROR_RETURN(READ_PORT_COUNTr(unit, port+10, &rval2));
        SOC_IF_ERROR_RETURN(READ_PORT_COUNTr(unit, port+11, &rval3));
        count++;
    } while ((count < 5) && (rval & rval1 & rval2 & rval3));

    if (count >= 5) {
        return SOC_E_FAIL;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 0));

    /* Reconfigure the port to 40G */
    /* disable transmit ports in use */
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_ENABLEr, i, PRT_ENABLEf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ECRCr, i, COUNTf, 0));
    }

    /* Reinitialize port in 40G mode */
    if (port == 1) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS0_RESETf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS0_RESETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 0)); /* Single */
    } else { /* port == 5 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS1_RESETf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS1_RESETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 0)); /* Single */
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_ENABLEr, port, PRT_ENABLEf, 1));

    /* Preenqueue reconfiguration */
    rval = 0x4; /* SPEED = 40G, ENABLE = 1 */
    if (port == 1) {
        int arb_tdm_index[] = {0, 5, 10, 15, 21, 26, 31, 36};
        int es_arb_tdm_index[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
        arb_tdm_table_entry_t arb_tdm;
        es_arb_tdm_table_entry_t es_arb_tdm;

        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));

        /* Modify receive TDM table */
        for (i = 0; i < 8; i++) {
            sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));    
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                     arb_tdm_index[i], &arb_tdm));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS0_SYS_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS0_SYS_RESET_Nf, 1)); 

        for (i = 0; i < 12; i++) {
            sal_memset(&es_arb_tdm, 0, sizeof(es_arb_tdm_table_entry_t));
            soc_ES_ARB_TDM_TABLEm_field32_set(unit, &es_arb_tdm, PORT_VECTORf, 
                                              2);
            SOC_IF_ERROR_RETURN(WRITE_ES_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                        es_arb_tdm_index[i], 
                                                        &es_arb_tdm));
        }

    } else {
        int arb_tdm_index[] = {2, 7, 12, 17, 23, 28, 33, 38};
        int es_arb_tdm_index[] = {2, 7, 12, 17, 22, 27, 32, 37, 42, 47, 52, 57};
        arb_tdm_table_entry_t arb_tdm;
        es_arb_tdm_table_entry_t es_arb_tdm;

        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x5; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));

        /* Modify receive TDM table */
        for (i = 0; i < 8; i++) {
            sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));    
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf, 5);
            SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                     arb_tdm_index[i], &arb_tdm));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS1_SYS_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS1_SYS_RESET_Nf, 1)); 

        for (i = 0; i < 12; i++) {
            sal_memset(&es_arb_tdm, 0, sizeof(es_arb_tdm_table_entry_t));
            soc_ES_ARB_TDM_TABLEm_field32_set(unit, &es_arb_tdm, PORT_VECTORf, 
                                              0x20);
            SOC_IF_ERROR_RETURN(WRITE_ES_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                        es_arb_tdm_index[i], 
                                                        &es_arb_tdm));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, ESEC_GLOBAL_CTRLr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_write(unit, soc_reg_addr(unit, ESEC_GLOBAL_CTRLr, 
                                            port, 0), rval));
    for (i = port; i < port+4; i++) {
        rval = 0;
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ESEC_MASTER_CTRLr(unit, i, rval));

        rval = 0;
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ISEC_MASTER_CTRLr(unit, i, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
    soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
    soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, ACTIVATE_PD_CREDITSf, 1);
    soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, EN_40Gf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ESEC_MASTER_CTRLr(unit, port, rval));

    rval = 0;
    soc_reg_field_set(unit, ISEC_GLOBAL_CTRLr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_write(unit, soc_reg_addr(unit, ISEC_GLOBAL_CTRLr, 
                                            port, 0), rval));
    rval = 0;
    soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
    soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
    soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, EN_40Gf, 1);
    SOC_IF_ERROR_RETURN(WRITE_ISEC_MASTER_CTRLr(unit, port, rval));

    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, TXD10G_FIFO_RSTBf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_PLLf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_MDIOREGSf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_HWf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, LCREF_ENf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PLLBYPf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, IDDQf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PWRDWNf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PWRDWN_PLLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS_CTRL_REGr(unit, port, rval));

    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));

    if (port == 1) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT0_LOGIC_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT0_LOGIC_RESET_Nf, 1)); 
    } else {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT1_LOGIC_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT1_LOGIC_RESET_Nf, 1)); 
    }

    SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));

    /* Deal with XLPORT block */
    /* put XMAC in reset*/
    for (i = port; i < port+4; i++) {
        /* toggle Reset */
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, i, &mac_ctrl));
        soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, i, mac_ctrl));
    }

    rval = 0;
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 0); 
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, PHY_PORT_MODEf, 0);

    /* Bring XMAC out of reset */
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, mac_ctrl));

    /* Reset XLPORT MIB counter */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));

    /* Enable XLPORT */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));

    /* Provide XMAC reset to ensure valid warpcore clock */
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, 1));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, 0));

    /* Reinitialize XLPORT block */
    /* Deal with XLPORT block */
    /* Bring XMAC out of reset */
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, mac_ctrl));

    rval = 0;
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 0); 
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, PHY_PORT_MODEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

    /* Reset XLPORT MIB counter */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));

    /* Enable XLPORT */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));


    si->port_num_lanes[port] = 4;

    /* Provision buffers for the newly created port */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_MINr, port, PORT_MINf, 8));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, port, 
                            PORT_SHARED_LIMITf, 62304));
    for (i = port+1; i < port+4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, i, PORT_MINf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, i, 
                                PORT_SHARED_LIMITf, 0));

    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, port, METER_GRANf, 3));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, port, THRESHOLDf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, port, REFRESHf, 156250));

    for (i = port+8; i < port+12; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, i, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, i, 
                                PORT_SHARED_LIMITf, 3200));

    }
    for (i = 0; i < 8; i++) {
        SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, port, i, &rval));
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 1133);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, i, rval));
    }
    for (i = port+8; i < port+12; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, i, 
                                    LB_WEIGHTf, 4));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, i, 
                                    LB_ENABLEf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));

    /* Clear any transients */
    SOC_IF_ERROR_RETURN(READ_RFRGr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RFCSr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RERPKTr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_TPKTr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RUCr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RDROPr(unit, port, &rval64));

    /* Setup Static load balance */   
    if (port == 1) {
        /* Setup Static load balance */   
        field_ids[0] = DEST_PORT_OFFSETf;
        for (i = 0; i < 65535; i+=4) {
            field_values[0] = 8;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i, 1,
                                     field_ids, field_values));
            field_values[0] = 9;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+1, 1,
                                     field_ids, field_values));
            field_values[0] = 10;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+2, 1,
                                     field_ids, field_values));
            field_values[0] = 11;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Am, i+3, 1,
                                     field_ids, field_values));
        }
        /* Configure Port Table to use UFLOW_A */
        SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, 1, &ptab));
        soc_PORT_TABm_field32_set(unit, &ptab, AGGREGATION_GROUP_SELECTf, 0);
        SOC_IF_ERROR_RETURN(
                WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, 1, &ptab));

        /* UFLOW_PORT reg will use UFLOW_A for load balance */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    DEST_PORTf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 1, 
                                    HASH_MODEf, 1));
        /* For switch link ports (9-12) use port 1 as the destination port */
        for (p = 9; p <= 12; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 1)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
        }
    } else {
        /* Setup Static load balance for Port 5*/   
        field_ids[0] = DEST_PORT_OFFSETf;
        for (i = 0; i < 65535; i+=4) {
            field_values[0] = 12;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i, 1,
                                     field_ids, field_values));
            field_values[0] = 13;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+1, 1,
                                     field_ids, field_values));
            field_values[0] = 14;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+2, 1,
                                     field_ids, field_values));
            field_values[0] = 15;
            SOC_IF_ERROR_RETURN
                (soc_mem_fields32_modify(unit, UFLOW_Bm, i+3, 1,
                                     field_ids, field_values));
        }
        /* port 5 configure uflow_b for static load balancing */
        SOC_IF_ERROR_RETURN(
                READ_PORT_TABm(unit, MEM_BLOCK_ANY, 5, &ptab));
        soc_PORT_TABm_field32_set(unit, &ptab, AGGREGATION_GROUP_SELECTf, 1);
        SOC_IF_ERROR_RETURN(
                WRITE_PORT_TABm(unit,MEM_BLOCK_ALL, 5, &ptab));

        /* UFLOW_PORT reg will use UFLOW_A for load balance */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    DEST_PORTf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, 5, 
                                    HASH_MODEf, 1));

        /* For switch link ports (13-16) use port 5 as the destination port */
        for (p = 13; p <= 16; p++) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        DEST_PORTf, 5)); 
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                        HASH_MODEf, 0));
        }
    }

    return SOC_E_NONE;
}

/* 
 * To support dynamic port configurations
 */
int 
_bcm_shadow_port_reconfig_10g(int unit, soc_port_t port)
{
    uint64 rval64;
    uint32 rval;
    int count=0, i, p;
    soc_info_t          *si;

    if (port != 1 && port != 5) {
        return SOC_E_PARAM;
    }

    si = &SOC_INFO(unit);
    /* Drain packets enqueued to port */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 1));
    do {
        sal_usleep(10000);
        SOC_IF_ERROR_RETURN(READ_PORT_COUNTr(unit, port+8, &rval));
        count++;
    } while ((count < 5) && (rval));

    if (count >= 5) {
        return SOC_E_FAIL;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 0));

    /* Reconfigure the port to 10G */
    /* disable transmit ports in use */
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_ENABLEr, i, PRT_ENABLEf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ECRCr, i, COUNTf, 0));
    }

    /* Reinitialize port in 10G mode */
    if (port == 1) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS0_RESETf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS0_RESETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS0_PORT_MODEf, 2)); /* Quad */
    } else { /* port == 5 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS1_RESETf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_EDB_DEBUG_SFT_RESETr, REG_PORT_ANY, 
                                    MS1_RESETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_PORT_MODEr, REG_PORT_ANY, 
                                    MS1_PORT_MODEf, 2)); /* Quad */
    }
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_ENABLEr, i, PRT_ENABLEf, 1));
    }

    /* Preenqueue reconfiguration */
    rval = 0x0; /* SPEED = 10G, ENABLE = 1 */
    if (port == 1) {
        int arb_tdm_index[] = {0, 5, 10, 15, 21, 26, 31, 36};
        int arb_tdm_ports[] = {1, 2, 3, 4, 1, 2, 3, 4};
        int es_arb_tdm_index[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
        int es_arb_tdm_vector[] = {2, 4, 8, 16, 2, 4, 8, 16, 2, 4, 8, 16};
        arb_tdm_table_entry_t arb_tdm;
        es_arb_tdm_table_entry_t es_arb_tdm;

        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));
        rval = 0x1; /* SPEED = 10G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG0r(unit, rval));

        /* Modify receive TDM table */
        for (i = 0; i < 8; i++) {
            sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));    
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf,
                                           arb_tdm_ports[i]);
            SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                     arb_tdm_index[i], &arb_tdm));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS0_SYS_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS0_SYS_RESET_Nf, 1)); 

        for (i = 0; i < 12; i++) {
            sal_memset(&es_arb_tdm, 0, sizeof(es_arb_tdm_table_entry_t));
            soc_ES_ARB_TDM_TABLEm_field32_set(unit, &es_arb_tdm, PORT_VECTORf, 
                                              es_arb_tdm_vector[i]);
            SOC_IF_ERROR_RETURN(WRITE_ES_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                        es_arb_tdm_index[i], 
                                                        &es_arb_tdm));
        }

    } else {
        int arb_tdm_index[] = {2, 7, 12, 17, 23, 28, 33, 38};
        int arb_tdm_ports[] = {5, 6, 7, 8, 5, 6, 7, 8};
        int es_arb_tdm_index[] = {2, 7, 12, 17, 22, 27, 32, 37, 42, 47, 52, 57};
        int es_arb_tdm_vector[] = {0x20, 0x40, 0x80, 0x100, 
                                   0x20, 0x40, 0x80, 0x100,
                                   0x20, 0x40, 0x80, 0x100};
               
        arb_tdm_table_entry_t arb_tdm;
        es_arb_tdm_table_entry_t es_arb_tdm;

        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));
        rval = 0x1; /* SPEED = 40G, ENABLE = 1 */
        SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIG1r(unit, rval));

        /* Modify receive TDM table */
        for (i = 0; i < 8; i++) {
            sal_memset(&arb_tdm, 0, sizeof(arb_tdm_table_entry_t));    
            soc_ARB_TDM_TABLEm_field32_set(unit, &arb_tdm, PORT_NUMf,
                                           arb_tdm_ports[i]);
            SOC_IF_ERROR_RETURN(WRITE_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                     arb_tdm_index[i], &arb_tdm));
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS1_SYS_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    MS1_SYS_RESET_Nf, 1)); 

        for (i = 0; i < 12; i++) {
            sal_memset(&es_arb_tdm, 0, sizeof(es_arb_tdm_table_entry_t));
            soc_ES_ARB_TDM_TABLEm_field32_set(unit, &es_arb_tdm, PORT_VECTORf, 
                                              es_arb_tdm_vector[i]);
            SOC_IF_ERROR_RETURN(WRITE_ES_ARB_TDM_TABLEm(unit, SOC_BLOCK_ALL,
                                                        es_arb_tdm_index[i], 
                                                        &es_arb_tdm));
        }
    }

    rval = 0;
    soc_reg_field_set(unit, ESEC_GLOBAL_CTRLr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_write(unit, soc_reg_addr(unit, ESEC_GLOBAL_CTRLr, 
                                            port, 0), rval));
    rval = 0;
    soc_reg_field_set(unit, ISEC_GLOBAL_CTRLr, &rval, INITf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_write(unit, soc_reg_addr(unit, ISEC_GLOBAL_CTRLr, 
                                            port, 0), rval));
    for (i = port; i < port+4; i++) {
        rval = 0;
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ESEC_MASTER_CTRLr(unit, i, rval));

        rval = 0;
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, ACTIVATE_PD_CREDITSf, 1);
        soc_reg_field_set(unit, ESEC_MASTER_CTRLr, &rval, EN_40Gf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ESEC_MASTER_CTRLr(unit, i, rval));
    
        rval = 0;
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ISEC_MASTER_CTRLr(unit, i, rval));

        rval = 0;
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, SOFT_RESETf, 0);
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, BYPASS_ENf, 1);
        soc_reg_field_set(unit, ISEC_MASTER_CTRLr, &rval, EN_40Gf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ISEC_MASTER_CTRLr(unit, i, rval));
    }

    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, TXD10G_FIFO_RSTBf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_PLLf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_MDIOREGSf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, RSTB_HWf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, LCREF_ENf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PLLBYPf, 0);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, IDDQf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PWRDWNf, 1);
    soc_reg_field_set(unit, XLPORT_XGXS_CTRL_REGr, &rval, PWRDWN_PLLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XGXS_CTRL_REGr(unit, port, rval));

    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 0);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));

    if (port == 1) {
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                XLPORT0_LOGIC_RESET_Nf, 0)); 
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                XLPORT0_LOGIC_RESET_Nf, 1)); 
    } else {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT1_LOGIC_RESET_Nf, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMIC_SOFT_RESET_REGr, REG_PORT_ANY, 
                                    XLPORT1_LOGIC_RESET_Nf, 1)); 
    }

    SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
    /* put XMAC in reset*/
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, i, &rval64));
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, i, rval64));
    }

    /* Deal with XLPORT block */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 2); 
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, PHY_PORT_MODEf, 2);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

    /* Bring XMAC out of reset */
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, i, &rval64));
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, SOFT_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, i, rval64));
    }

    /* Reset XLPORT MIB counter */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));

    /* Enable XLPORT */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));

    /* Provide XMAC reset to ensure valid warpcore clock */
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, 1));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, 0));


    /* Reinitialize XLPORT block */
    /* Deal with XLPORT block */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, CORE_PORT_MODEf, 2); 
    soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, PHY_PORT_MODEf, 2);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

    /* Bring XMAC out of reset */
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, i, &rval64));
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, SOFT_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, i, rval64));
    }

    /* Reset XLPORT MIB counter */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));

    /* Enable XLPORT */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT0f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT1f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT2f, 1);
    soc_reg_field_set(unit, XLPORT_PORT_ENABLEr, &rval, PORT3f, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLPORT_PORT_ENABLEr(unit, port, rval));


    for (i = port; i < port+4; i++) {
        si->port_num_lanes[i] = 1;
    }

    /* Provision buffers for the newly created port */
    for (i = port; i < port+4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, i, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, i, 
                                PORT_SHARED_LIMITf, 15576));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_RESET_OFFSETr, i, 
                                PORT_RESET_OFFSETf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, i, METER_GRANf, 3));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIGr, i, THRESHOLDf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGRMETERINGCONFIG1r, i, REFRESHf, 156250));
    }

    for (i = port+8; i < port+12; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_MINr, i, PORT_MINf, 8));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_SHARED_LIMITr, i, 
                                PORT_SHARED_LIMITf, 3200));

    }
    for (i = 0; i < 8; i++) {
        SOC_IF_ERROR_RETURN(READ_PG_HDRM_LIMITr(unit, port, i, &rval));
        soc_reg_field_set(unit, PG_HDRM_LIMITr, &rval, PG_HDRM_LIMITf, 283);
        SOC_IF_ERROR_RETURN(WRITE_PG_HDRM_LIMITr(unit, port, i, rval));
    }
    for (i = port+8; i < port+12; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, i, 
                                    LB_WEIGHTf, 4));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_WREDCONFIG_CELLr, i, 
                                    LB_ENABLEf, 0));
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, INPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                INPUT_PORT_RX_ENABLEf, 0x1ffff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, OUTPUT_PORT_RX_ENABLEr, REG_PORT_ANY, 
                                OUTPUT_PORT_RX_ENABLEf, 0x1ffff));

    /* Clear any transients */
    SOC_IF_ERROR_RETURN(READ_RFRGr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RFCSr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RERPKTr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_TPKTr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RUCr(unit, port, &rval64));
    SOC_IF_ERROR_RETURN(READ_RDROPr(unit, port, &rval64));

    /* Apply software workaround to clear some FIFO points */
    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                DROP_ENf, 0));

    /* Setup Static load balance for Port (one to one mapping) */   
    for (p = port; p < port+4; p++) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                    DEST_PORTf, p+8)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p, 
                                    HASH_MODEf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p+8,
                                    DEST_PORTf, p)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, UFLOW_PORT_CONTROLr, p+8, 
                                    HASH_MODEf, 0));
    }

    return SOC_E_NONE;
}


/* 
 * To support dynamic port configurations
 */
int 
_bcm_shadow_port_lanes_set(int unit, soc_port_t port, int value)
{
    soc_port_t          oport;
    soc_info_t          *si;
    int rv = SOC_E_NONE;

    si = &SOC_INFO(unit);

    if (!SOC_PORT_VALID(unit, port)) {
        return SOC_E_PORT;
    }

    /* Apply to only Front panel, 1 and 5 ports */
    if ((port != 1) && (port != 5)) {
        return SOC_E_PORT;
    }

    if ((value != 1) && (value != 4)) {
        return SOC_E_CONFIG;
    }
    
    /* Update disabled ports */
    if (value == 4) { /* modified from 1 lane to 4 lanes */
        if (port == 1) {
            for (oport = 2; oport <= 4; oport++) {
                SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->xe.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->port.bitmap, oport);
                si->port_num_lanes[oport] = 1;
            }
        }
        if (port == 5) {
            for (oport = 6; oport <= 8; oport++) {
                SOC_PBMP_PORT_ADD(si->ge.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->xe.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->ether.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, oport);
                SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->port.bitmap, oport);
                si->port_num_lanes[oport] = 1;
            }
        }
        si->port_speed_max[port] = 40000; /* new max speed = 40G */
        si->port_num_lanes[port] = 4;     /* Quad lane port */

        rv = _bcm_shadow_port_reconfig_40g(unit, port);
    } else if (value == 1) {
        if (port == 1) {
            for (oport = 2; oport <= 4; oport++) {
                si->port_speed_max[oport] = 10000;
                SOC_PBMP_PORT_ADD(si->port.bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, oport);
                si->port_num_lanes[oport] = 1;     /* Independent lane port */
            }
        }
        if (port == 5) {
            for (oport = 6; oport <= 8; oport++) {
                SOC_PBMP_PORT_ADD(si->port.bitmap, oport);
                si->port_speed_max[oport] = 10000;
                SOC_PBMP_PORT_REMOVE(si->ge.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->xe.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->ether.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->port.disabled_bitmap, oport);
                SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, oport);
                si->port_num_lanes[oport] = 1;     /* Independent lane port */
            }
        }
        si->port_speed_max[port] = 10000; /* new max speed = 40G */
        si->port_num_lanes[port] = 1;     /* Single lane port */
        rv = _bcm_shadow_port_reconfig_10g(unit, port);
    }

    return rv;
}

int
soc_shadow_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    uint32 rval, addr;
    soc_reg_t reg;
    int index;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    /* only first 5 monitors are used */
    if (5 > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = 5;
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = thermal_reg[index];
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        soc_pci_getreg(unit, addr, &rval);
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        cur = (4180000 - (5556 * fval)) / 1000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (4180000 - (5556 * fval)) / 1000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak    = peak;
    }
    READ_CMIC_SOFT_RESET_REGr(unit, &rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                  TEMP_MON_PEAK_RESET_Nf, 0);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);
    soc_reg_field_set(unit, CMIC_SOFT_RESET_REGr, &rval,
                  TEMP_MON_PEAK_RESET_Nf, 1);
    WRITE_CMIC_SOFT_RESET_REGr(unit, rval);
    *temperature_count=num_entries_out;

    return SOC_E_NONE;

}

#endif /* BCM_SHADOW_SUPPORT */
