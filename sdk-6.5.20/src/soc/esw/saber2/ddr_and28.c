/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ddr_and28.c
 * Purpose:
 * Requires:
 */

#include <sal/core/boot.h>

#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/trident.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/shmoo_and28.h>
#include <soc/phy/ddr34.h>
#include <soc/mspi.h>
#include <soc/phyreg.h>
#include <shared/bsl.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/iproc.h>
#include <sal/appl/config.h>
#include <shared/util.h>
#ifdef BCM_SABER2_SUPPORT

static uint32 dram0_clamshell_enable = 0;
static uint32 dram1_clamshell_enable = 0;

int soc_sb2_and28_post_shmoo_dram_init(int unit, int ci);

const sb2_req_grade_mem_set_set_t SABER2fgmss[SB2_FREQ_CNT][SB2_GRADE_CNT] = 
{
    { /* FREQ_667 */
        /* GRADE_DEFAULT */
        {10,  5, 17,    32, 48,      5,  8, 15, 11,     17, 24,  0,  0,     2600,   14, 18, {100, 54,    37},  272,     9,  7, 10,     16,  667,   0x1b50, 0x0210},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {10,  5, 17,    32, 48,      5,  8, 15, 11,     17, 24,  0,  0,     2600,   14, 18, {100, 54,    37},  272,     9,  7, 10,     16,  667,   0x1b50, 0x0210},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    },

    { /* FREQ_800 */
        /* GRADE_DEFAULT */
        {13,  12, 20,    32, 48,      8, 12, 16, 15,     20, 25,  0,  0,     3056,   18, 22, {120, 64,    44},  272,    11,  8, 12,     21,  800,   0x1d70, 0x0218},
        /* GRADE_080808 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_090909 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_101010 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_111111 */
        {13,  12, 20,    32, 48,      8, 12, 16, 15,     20, 25,  0,  0,     3056,   18, 22, {120, 64,    44},  272,    11,  8, 12,     21,  800,   0x1d70, 0x0218},
        /* GRADE_121212 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_131313 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1},
        /* GRADE_141414 */
        {-1, -1, -1,    -1, -1,     -1, -1, -1, -1,     -1, -1, -1, -1,       -1,   -1, -1, { -1, -1, -  1},   -1,    -1, -1, -1,     -1,   -1,       -1,     -1}
    }
};

#define SEGMENTS_CNT 6

const static uint32
sb2_ite_segment_gap[SB2_FREQ_CNT][SB2_CORE_PLL_CLOCK_CNT][SEGMENTS_CNT] =
{
    /* FREQ_667 */
    {
        /* CLK_130 */ {2, 2, 1, 1, 0, 0},
        /* CLK_118 */ {2, 2, 1, 1, 0, 0},
        /* CLK_75 */  {2, 2, 1, 1, 0, 0},
        /* CLK_37 */  {0, 0, 0, 0, 0, 0},
        /* CLK_24 */  {0, 0, 0, 0, 0, 0}
    },
    /* FREQ_800 */
    {
        /* CLK_130 */ {2, 1, 1, 1, 0, 0},
        /* CLK_118 */ {1, 1, 1, 0, 0, 0},
        /* CLK_75 */  {2, 1, 1, 1, 0, 0},
        /* CLK_37 */  {0, 0, 0, 0, 0, 0},
        /* CLK_24 */  {0, 0, 0, 0, 0, 0}
    }
};

int
_soc_ddr_sb2_mem_gradetoloc(uint32 grade)
{
    switch(grade) {
        case SB2_MEM_GRADE_080808:
            return SB2_GRADE_080808;
        case SB2_MEM_GRADE_090909:
            return SB2_GRADE_090909;
        case SB2_MEM_GRADE_101010:
            return SB2_GRADE_101010;
        case SB2_MEM_GRADE_111111:
            return SB2_GRADE_111111;
        case SB2_MEM_GRADE_121212:
            return SB2_GRADE_121212;
        case SB2_MEM_GRADE_131313:
            return SB2_GRADE_131313;
        case SB2_MEM_GRADE_141414:
            return SB2_GRADE_141414;
        default:
            return SB2_GRADE_DEFAULT;
    }
}

int
_soc_ddr_sb2_phy_freqtoloc(uint32 freq)
{
    switch(freq) {
        case SB2_DDR_FREQ_667:
            return SB2_FREQ_667;
        case SB2_DDR_FREQ_800:
            return SB2_FREQ_800;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Unsupported Frequency %d MHz \n"), freq));
            return SOC_E_PARAM;
    }
}

int
_soc_sb2_mem_rowstoloc(uint32 rows)
{
    switch(rows) {
        case SB2_MEM_ROWS_32K:
            return SB2_MEM_4G;
        case SB2_MEM_ROWS_16K:
            return SB2_MEM_2G;
        default:
            return SB2_MEM_1G;
    }
}

int
_soc_sb2_clktoloc(uint32 core_clock) {
    switch(core_clock) {
        case CORE_PLL_CLOCK_130M:
            return SB2_CORE_PLL_CLOCK_130M;
        case CORE_PLL_CLOCK_118M:
            return SB2_CORE_PLL_CLOCK_118M;
        case CORE_PLL_CLOCK_75M:
            return SB2_CORE_PLL_CLOCK_75M;
        case CORE_PLL_CLOCK_37M:
            return SB2_CORE_PLL_CLOCK_37M;
        case CORE_PLL_CLOCK_24M:
            return SB2_CORE_PLL_CLOCK_24M;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Unsupported Clock %d MHz \n"), core_clock));
            return SOC_E_PARAM;
    }
}

void
_soc_ddr_sb2_phy_init_mem_set_wrapper(  uint32 unit,
                                        sb2_req_grade_mem_set_set_t *fgmssPtr,
                                        int freq_loc, int grade_loc ) {


    fgmssPtr->twl = SABER2fgmss[freq_loc][grade_loc].twl;
    fgmssPtr->twr = SABER2fgmss[freq_loc][grade_loc].twr;
    fgmssPtr->trc = SABER2fgmss[freq_loc][grade_loc].trc;
    fgmssPtr->rfifo_afull = SABER2fgmss[freq_loc][grade_loc].rfifo_afull;
    fgmssPtr->rfifo_full = SABER2fgmss[freq_loc][grade_loc].rfifo_full;
    fgmssPtr->trtw = SABER2fgmss[freq_loc][grade_loc].trtw;
    fgmssPtr->twtr = SABER2fgmss[freq_loc][grade_loc].twtr;
    fgmssPtr->tfaw = SABER2fgmss[freq_loc][grade_loc].tfaw;
    fgmssPtr->tread_enb = soc_property_get(unit,spn_DDR3_TREAD_ENB,
            SABER2fgmss[freq_loc][grade_loc].tread_enb);
    fgmssPtr->bank_unavail_rd = soc_property_get(unit,
            spn_DDR3_BANK_UNAVAIL_RD, SABER2fgmss[freq_loc][grade_loc].bank_unavail_rd);
    fgmssPtr->bank_unavail_wr = soc_property_get(unit,
            spn_DDR3_BANK_UNAVAIL_WR, SABER2fgmss[freq_loc][grade_loc].bank_unavail_wr);
    fgmssPtr->rr_read = soc_property_get(unit,spn_DDR3_ROUND_ROBIN_READ, 
            SABER2fgmss[freq_loc][grade_loc].rr_read);
    fgmssPtr->rr_write = soc_property_get(unit,spn_DDR3_ROUND_ROBIN_WRITE,
            SABER2fgmss[freq_loc][grade_loc].rr_write);
    fgmssPtr->refrate = soc_property_get(unit,spn_DDR3_REFRESH_INTVL_OVERRIDE, 
            SABER2fgmss[freq_loc][grade_loc].refrate);
    fgmssPtr->trp_read = soc_property_get(unit,spn_DDR3_TRP_READ,
            SABER2fgmss[freq_loc][grade_loc].trp_read);
    fgmssPtr->trp_write = soc_property_get(unit,spn_DDR3_TRP_WRITE,
            SABER2fgmss[freq_loc][grade_loc].trp_write);
    fgmssPtr->trfc[SB2_MEM_4G] = SABER2fgmss[freq_loc][grade_loc].trfc[SB2_MEM_4G];
    fgmssPtr->trfc[SB2_MEM_2G] = SABER2fgmss[freq_loc][grade_loc].trfc[SB2_MEM_2G];
    fgmssPtr->trfc[SB2_MEM_1G] = SABER2fgmss[freq_loc][grade_loc].trfc[SB2_MEM_1G];
    fgmssPtr->tzq = SABER2fgmss[freq_loc][grade_loc].tzq;
    fgmssPtr->cl = SABER2fgmss[freq_loc][grade_loc].cl;
    fgmssPtr->cwl = SABER2fgmss[freq_loc][grade_loc].cwl;
    fgmssPtr->wr = SABER2fgmss[freq_loc][grade_loc].wr;
    fgmssPtr->jedec = SABER2fgmss[freq_loc][grade_loc].jedec;
    fgmssPtr->mhz = SABER2fgmss[freq_loc][grade_loc].mhz;
    fgmssPtr->mr0 = SABER2fgmss[freq_loc][grade_loc].mr0;
    fgmssPtr->mr2 = SABER2fgmss[freq_loc][grade_loc].mr2;

}


int soc_sb2_and28_shmoo_phy_reg_read(int unit, int ci, uint32 reg_addr, uint32 *reg_data)
{
    uint32 rval;
    soc_timeout_t to;

    rval = 0;
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ERR_ACKf, 1); 
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ADDRf, reg_addr);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_RD_WR_Nf, 1);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_CTRLr(unit, rval));

    if (SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, 10000000, 0);     /* 10 Sec */
    } else {
        soc_timeout_init(&to, 50000, 0);        /* 50 mS */
    }
    do {
        SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_CTRLr(unit, &rval));
        if (soc_reg_field_get(unit, TOP_CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ACKf)) {
            break;
        }
        if (soc_reg_field_get
            (unit, TOP_CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ERR_ACKf)) {
            return SOC_E_FAIL;
        }
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    } while (TRUE);
    SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_DATAr(unit, reg_data));
    
    return SOC_E_NONE;
}

int soc_sb2_and28_shmoo_phy_reg_write(int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
    uint32 rval;
    soc_timeout_t to;

    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_DATAr(unit, reg_data));

    rval = 0;
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ACKf, 1);      /* write 1 to    clear */
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ERR_ACKf, 1);  /* write 1 to    clear */
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_ADDRf,
                      reg_addr);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_RD_WR_Nf, 0);
    soc_reg_field_set(unit, TOP_CI_DDR_PHY_REG_CTRLr, &rval, PHY_REG_REQf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CI_DDR_PHY_REG_CTRLr(unit, rval));

    if (SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, 10000000, 0);     /* 10 Sec */
    } else {
        soc_timeout_init(&to, 50000, 0);        /* 50 mS */
    }

    do {
        SOC_IF_ERROR_RETURN(READ_TOP_CI_DDR_PHY_REG_CTRLr(unit, &rval));
        if (soc_reg_field_get(unit, TOP_CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ACKf)) {
            break;
        }
        if (soc_reg_field_get
            (unit, TOP_CI_DDR_PHY_REG_CTRLr, rval, PHY_REG_ERR_ACKf)) {
            return SOC_E_FAIL;
        }
        if (soc_timeout_check(&to)) {
            return SOC_E_TIMEOUT;
        }
    } while (TRUE);
    
    return SOC_E_NONE;
}

int soc_sb2_and28_shmoo_phy_reg_modify(int unit, int ci, uint32 reg_addr, uint32 data,
        uint32 mask)
{
    int rv = SOC_E_NONE;
    uint32 tmp, otmp;

    data = data & mask;

    SOC_IF_ERROR_RETURN(soc_sb2_and28_shmoo_phy_reg_read(unit, ci, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= data;

    if (otmp != tmp) {
        SOC_IF_ERROR_RETURN
            (soc_sb2_and28_shmoo_phy_reg_write(unit, ci, reg_addr, tmp));
    }

    return rv;
}

int sb2_and28_configure_bist(
    int unit,
    int ci,
    uint32 write_weight,
    uint32 read_weight,
    uint32 pattern_bit_mode,
    uint32 two_addr_mode,
    uint32 prbs_mode,
    uint32 address_shift_mode,
    uint32 data_shift_mode,
    uint32 data_addr_mode,
    uint32 bist_num_actions,
    uint32 bist_start_address,
    uint32 bist_end_address,
    uint32 bg_interleave,
    uint32 single_bank_test,
    uint32 address_prbs_mode,
    uint32 bist_pattern[SB2_BIST_NUM_PATTERNS])
{

    uint32 rval;
    int i;
    const static soc_reg_t bist_pattern_word_reg_list[] =
                        {BISTPATTERNWORD0r, BISTPATTERNWORD1r, BISTPATTERNWORD2r,
                         BISTPATTERNWORD3r, BISTPATTERNWORD4r, BISTPATTERNWORD5r,
                         BISTPATTERNWORD6r, BISTPATTERNWORD7r};
    const static soc_field_t bist_patten_field_list[] =
                        {BISTPATTERN0f, BISTPATTERN1f, BISTPATTERN2f,
                         BISTPATTERN3f, BISTPATTERN4f, BISTPATTERN5f,
                         BISTPATTERN6f, BISTPATTERN7f};
    
    SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, ci, &rval));
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, WRITEWEIGHTf, write_weight);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, READWEIGHTf, read_weight);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PATTERNBITMODEf, pattern_bit_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, TWOADDRMODEf, two_addr_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, PRBSMODEf, prbs_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, CONSADDR8BANKSf, 1);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, ADDRESSSHIFTMODEf, address_shift_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATASHIFTMODEf, data_shift_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, DATAADDRMODEf, data_addr_mode);
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, INDWRRDADDRMODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, ci, rval));
    

    SOC_IF_ERROR_RETURN(READ_BISTNUMBEROFACTIONSr(unit, ci, &rval));
    soc_reg_field_set(unit, BISTNUMBEROFACTIONSr, &rval, BISTNUMACTIONSf, bist_num_actions);
    SOC_IF_ERROR_RETURN(WRITE_BISTNUMBEROFACTIONSr(unit, ci, rval));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSTARTADDRESSr, ci, 0, &rval));
    soc_reg_field_set(unit, BISTSTARTADDRESSr, &rval, BISTSTARTADDRESSf, bist_start_address);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTSTARTADDRESSr, ci, 0, rval));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTENDADDRESSr, ci, 0, &rval));
    soc_reg_field_set(unit, BISTENDADDRESSr, &rval, BISTENDADDRESSf, bist_end_address);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, BISTENDADDRESSr, ci, 0, rval));
   
    if(bist_pattern != NULL) {
        for(i = 0; i < SB2_BIST_NUM_PATTERNS; i++){
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                        bist_pattern_word_reg_list[i], REG_PORT_ANY, 0,
                        bist_patten_field_list[i])); 
        }
    }
    return SOC_E_NONE;
}

int sb2_bist_atomic_action_polling(int unit, int ci) {
    uint32 rval;
    soc_timeout_t to;

    if (SAL_BOOT_SIMULATION) {
        soc_timeout_init(&to, 10000000, 0);     /* 10 Sec */
    } else {
        soc_timeout_init(&to, 50000, 0);        /* 50 mS */
    }

    do {
        SOC_IF_ERROR_RETURN(READ_BISTSTATUSESr(unit, ci, &rval));
        if (soc_reg_field_get(unit, BISTSTATUSESr, rval, BISTFINISHEDf)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            LOG_INFO(BSL_LS_SOC_DDR,
                (BSL_META_U(unit,
                        "BIST TIMEOUT!!!\n")));
            return SOC_E_TIMEOUT;
        }
    }while(TRUE);

    return SOC_E_NONE;
}

int sb2_bist_atomic_action_start(int unit, int ci, uint8 start) {
    uint32 rval;

    /* Disable BIST and enable if start is set */
    SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, ci, &rval));
    soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, ci, rval));
    
    if (start) {
        SOC_IF_ERROR_RETURN(READ_BISTCONFIGURATIONSr(unit, ci, &rval));
        soc_reg_field_set(unit, BISTCONFIGURATIONSr, &rval, BISTENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_BISTCONFIGURATIONSr(unit, ci, rval));
    }

    return SOC_E_NONE;
}

int sb2_bist_atomic_action(int unit, int ci, uint8 is_infinite, uint8 stop) {

    /* Start BIST */
    if (stop == FALSE) {
        SOC_IF_ERROR_RETURN(sb2_bist_atomic_action_start(unit, ci, TRUE));
    }
    
    if (is_infinite == FALSE && stop == FALSE) {
        SOC_IF_ERROR_RETURN(sb2_bist_atomic_action_polling(unit, ci));
    }

    /*  Wait BIST finished */
    if (is_infinite == FALSE || stop == TRUE) {
        SOC_IF_ERROR_RETURN(sb2_bist_atomic_action_start(unit, ci, FALSE));
    }

    return SOC_E_NONE;
}

int sb2_bist_start_test( int unit, int ci, 
        sb2_bist_info *info) {

    int     i;
    uint32  pattern_lcl[SB2_BIST_NUM_PATTERNS];
    uint32  bist_repeat_pattern;
    
    if ((info->bist_flags & SB2_BIST_FLAGS_USE_RANDOM_DATA_SEED ) != 0x0) {
        for(i=0; i <  SB2_BIST_NUM_PATTERNS; ++i) {
            pattern_lcl[i] = sal_rand();
        }
    } else if (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  SB2_BIST_NUM_PATTERNS; ++i) {
            pattern_lcl[i] = 0x55555555;
        }
    } else if (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  SB2_BIST_NUM_PATTERNS; ++i) {
            pattern_lcl[i] = 0xffffffff;
        }
    } else if (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  SB2_BIST_NUM_PATTERNS; ++i) {
            pattern_lcl[i] = 0x00000000;
        }
    } else {
        for (i=0; i <  SB2_BIST_NUM_PATTERNS; ++i) {
            pattern_lcl[i] = info->pattern[i];
        }
    }
/*
    LOG_INFO(BSL_LS_SOC_DDR,
            (BSL_META_U(unit,
                        "Bit test parameters : ci : %d\n write_weight %d\n"
                        "read weight : %d\npattern_mode: %d\n"
                        "bist flag : 0x%x\n"), ci, info->write_weight, 
                                info->read_weight, info->pattern_mode, info->bist_flags));
    */
    bist_repeat_pattern = soc_property_suffix_num_get(unit, -1,
            spn_CUSTOM_FEATURE, "bist_repeat_pattern", 0xffffffff);

    if(((info->bist_flags & 
                    (SB2_BIST_FLAGS_STOP | SB2_BIST_FLAGS_GET_DATA)) == 0) &&
            bist_repeat_pattern == 0xffffffff) {
        /* Configure BIST */
        SOC_IF_ERROR_RETURN(sb2_and28_configure_bist(
                    unit, ci,
                    info->write_weight, info->read_weight,
                    (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_BIT_MODE) ? TRUE : FALSE,
                    (info->bist_flags & SB2_BIST_FLAGS_TWO_ADDRESS_MODE) ? TRUE : FALSE,
                    (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS) ? TRUE : FALSE,
                    (info->bist_flags & SB2_BIST_FLAGS_ADDRESS_SHIFT_MODE) ? TRUE : FALSE,
                    (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_SHIFT_MODE) ? TRUE : FALSE,
                    (info->pattern_mode == SB2_DRAM_BIST_DATA_PATTERN_ADDR_MODE) ? TRUE : FALSE,
                    (info->bist_flags & SB2_BIST_FLAGS_INFINITE) ? 0 : info->bist_num_actions,
                    info->bist_start_address, info->bist_end_address,
                    (info->bist_flags & SB2_BIST_FLAGS_BG_INTERLEAVE) ? TRUE : FALSE,
                    (info->bist_flags & SB2_BIST_FLAGS_SINGLE_BANK_TEST) ? TRUE : FALSE,
                    (info->bist_flags & SB2_BIST_FLAGS_ADDRESS_PRBS_MODE) ? TRUE : FALSE,
                    pattern_lcl));
    }

    if((info->bist_flags & SB2_BIST_FLAGS_GET_DATA) == 0) {
         SOC_IF_ERROR_RETURN(sb2_bist_atomic_action(
                     unit, ci,
                     (info->bist_flags & SB2_BIST_FLAGS_INFINITE) ? TRUE : FALSE,
                     (info->bist_flags & SB2_BIST_FLAGS_STOP) ? TRUE : FALSE));
    }

    return SOC_E_NONE;
}

int soc_sb2_and28_shmoo_drc_bist_ci_conf_set(int unit, int ci, and28_bist_info_t info)
{
    int                 i;
    sb2_bist_info       bist_info;
    uint32              bist_timer_us;

    sal_memset(&bist_info, 0, sizeof(bist_info));

    bist_info.write_weight       = info.write_weight;
    bist_info.read_weight        = info.read_weight;
    bist_info.bist_start_address = info.bist_start_address;
    bist_info.bist_end_address   = info.bist_end_address;

    for (i = 0; i < SB2_BIST_NUM_PATTERNS; i++) {
        bist_info.pattern[i] = info.data_pattern[i];
    }

    bist_timer_us = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
            "bist_repeat_pattern", 0xffffffff);

    if(bist_timer_us != 0xffffffff) {
        info.bist_timer_us = bist_timer_us;
    }

    if(info.bist_timer_us == 0x0) {
        /* Run according to Number of Actions */
        bist_info.bist_num_actions   = info.bist_num_actions;

        if (info.prbs_mode == 0x1) {
            bist_info.pattern_mode |= SB2_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS;
            bist_info.bist_flags = 
                SB2_BIST_FLAGS_USE_RANDOM_SEED | SB2_BIST_FLAGS_BG_INTERLEAVE;
        }

        SOC_IF_ERROR_RETURN(sb2_bist_start_test(unit, ci, &bist_info));
    } else {
        /* Run according to timeout */
        bist_info.bist_flags |= SB2_BIST_FLAGS_INFINITE;
        SOC_IF_ERROR_RETURN(sb2_bist_start_test(unit, ci, &bist_info));

        sal_usleep(info.bist_timer_us);

        bist_info.bist_flags = SB2_BIST_FLAGS_STOP;
        SOC_IF_ERROR_RETURN(sb2_bist_start_test(unit, ci, &bist_info));
    }
    return SOC_E_NONE;
}


int soc_sb2_and28_shmoo_drc_bist_conf_set(int unit, int ci, and28_bist_info_t info)
{
    SOC_IF_ERROR_RETURN(soc_sb2_and28_shmoo_drc_bist_ci_conf_set(unit, (2 * ci), info));
    SOC_IF_ERROR_RETURN(soc_sb2_and28_shmoo_drc_bist_ci_conf_set(unit, ((2 * ci) + 1), info));
    return SOC_E_NONE;

}
int soc_sb2_and28_shmoo_drc_bist_err_cnt(int unit, int ci, 
        and28_bist_err_cnt_t *info) {

    uint32 rval1, rval2, temp;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTERROROCCURREDr, (2 * ci), 0,  &rval1));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTERROROCCURREDr, ((2 * ci) + 1), 0,  &rval2));
    temp = ((rval1 & 0xFFFF) & ((rval1 >> 16) & 0xFFFF)) | 
           (((rval2 & 0xFFFF) & ((rval2 >> 16) & 0xFFFF)) <<  16);
    info->bist_err_occur = temp;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTFULLMASKERRORCOUNTERr, (2 * ci), 0,  &rval1));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTFULLMASKERRORCOUNTERr, ((2 * ci) + 1), 0,  &rval2));
    temp = rval1 + rval2;
    if ((temp < rval1) || (temp < rval2)) {
        /* To check whether temp has overflown */
        temp = 0xFFFFFFFF;
    }
    info->bist_full_err_cnt = temp;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSINGLEBITMASKERRORCOUNTERr, (2 * ci), 0,  &rval1));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTSINGLEBITMASKERRORCOUNTERr, ((2 * ci) + 1), 0,  &rval2));
    temp = rval1 + rval2;
    if ((temp < rval1) || (temp < rval2)) {
        /* To check whether temp has overflown */
        temp = 0xFFFFFFFF;
    }
    info->bist_single_err_cnt = temp;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTGLOBALERRORCOUNTERr, (2 * ci), 0,  &rval1));
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, BISTGLOBALERRORCOUNTERr, ((2 * ci) + 1), 0,  &rval2));
    temp = rval1 + rval2;
    if ((temp < rval1) || (temp < rval2)) {
        /* To check whether temp has overflown */
        temp = 0xFFFFFFFF;
    }
    info->bist_global_err_cnt = temp;

    return SOC_E_NONE;
}

int soc_sb2_and28_shmoo_dram_init(int unit, int ci, int phase) 
{
    uint32 rval;

    if(phase != 0) {
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "Phase must be 0 for DDR3\n")));
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &rval));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 0);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, RST_Nf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, rval));
    sal_usleep(2000);
    
    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &rval));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 0);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, RST_Nf, 3);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, rval));
    sal_usleep(2000);
    
    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &rval));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 3);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, RST_Nf, 3);
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, WARMSTARTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, rval));
    sal_usleep(2000);

    return SOC_E_NONE;
}

int soc_sb2_ci_config(int unit, int ci)
{
    uint32 rval;
    sb2_req_grade_mem_set_set_t T0fgmss;
    int freq_loc, grade_loc, row_loc;

    freq_loc = _soc_ddr_sb2_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));
    grade_loc = _soc_ddr_sb2_mem_gradetoloc(SOC_DDR3_MEM_GRADE(unit));

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }

    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "B01. Bring CI out of software reset\n")));

    /* CI Config register configurations. */
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "B02. Set Strap and parameter per speed and grade\n")));

    _soc_ddr_sb2_phy_init_mem_set_wrapper(unit, &T0fgmss, freq_loc, grade_loc);

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG0r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWLf, (ci & 0x1) ? \
            (T0fgmss.twl) : (T0fgmss.twl - 1));       
    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TWRf, T0fgmss.twr);
    soc_reg_field_set(unit, CI_CONFIG0r, &rval, TRCf, T0fgmss.trc);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG0r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG1r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG1r, &rval, RFIFO_ALMOST_FULL_THRESHOLDf, 
            T0fgmss.rfifo_afull);
    soc_reg_field_set(unit, CI_CONFIG1r, &rval, RFIFO_FULL_THRESHOLDf, 
            T0fgmss.rfifo_full);
    soc_reg_field_set(unit, CI_CONFIG1r, &rval, WFIFO_ALMOST_FULL_THRESHOLDf, 
            0x82);
    soc_reg_field_set(unit, CI_CONFIG1r, &rval, WFIFO_FULL_THRESHOLDf, 
            0xF6);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG1r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG2r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TRTWf, T0fgmss.trtw);
    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TWTRf, T0fgmss.twtr);
    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TFAWf, T0fgmss.tfaw);
    soc_reg_field_set(unit, CI_CONFIG2r, &rval, TREAD_ENBf,
            (ci & 0x1)?(T0fgmss.tread_enb-1):T0fgmss.tread_enb);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG2r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG3r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG3r, &rval, BANK_UNAVAILABLE_WRf,
            T0fgmss.bank_unavail_wr);
    soc_reg_field_set(unit, CI_CONFIG3r, &rval, BANK_UNAVAILABLE_RDf,
            T0fgmss.bank_unavail_rd);
    soc_reg_field_set(unit, CI_CONFIG3r, &rval, RR_READf, T0fgmss.rr_read);
    soc_reg_field_set(unit, CI_CONFIG3r, &rval, RR_WRITEf, T0fgmss.rr_write);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG3r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG6r(unit, ci, &rval));

    row_loc = _soc_sb2_mem_rowstoloc(SOC_DDR3_NUM_ROWS(unit));
    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRFCf, T0fgmss.trfc[row_loc]);
    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRP_READf, T0fgmss.trp_read);
    soc_reg_field_set(unit, CI_CONFIG6r, &rval, TRP_WRITEf, T0fgmss.trp_write);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG6r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG7r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG7r, &rval, TZQf, T0fgmss.tzq);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG7r(unit, ci, rval));

    LOG_INFO(BSL_LS_SOC_DDR,
            (BSL_META_U(unit,
                        "B03. Clear DDR Reset\n")));
    
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "B04. Set config MR0, MR1, and MR2 registers\n")));

    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR0r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_DDR_MR0r, &rval, Af, T0fgmss.mr0);
    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR0r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR1r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_DDR_MR1r, &rval, Af, 0x10);
    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR1r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_DDR_MR2r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_DDR_MR2r, &rval, Af, T0fgmss.mr2);
    SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR2r(unit, ci, rval));

    SOC_IF_ERROR_RETURN(READ_CI_CONFIG4r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG4r, &rval, REFRESH_INTERVALf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG4r(unit, ci, rval));
    
    SOC_IF_ERROR_RETURN(READ_CI_CONFIG4r(unit, ci, &rval));
    soc_reg_field_set(unit, CI_CONFIG4r, &rval, REFRESH_INTERVALf, T0fgmss.refrate);
    SOC_IF_ERROR_RETURN(WRITE_CI_CONFIG4r(unit, ci, rval));
    
    return SOC_E_NONE;

}

int soc_sb2_and28_post_shmoo_dram_init(int unit, int ci)
{
    uint32 rval;
    int ciCS, ciCM;
    sal_usecs_t to_val;
    soc_timeout_t to;
    uint32 mr0, mr1, mr2, mr3;

    if (SAL_BOOT_QUICKTURN) {
            to_val = 10000000;  /* 10 Sec */
    } else {
            to_val = 50000;     /* 50 mS */
    }

    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &rval));
    soc_reg_field_set(unit, CI_PHY_CONTROLr, &rval, CKEf, 3);
    SOC_IF_ERROR_RETURN(WRITE_CI_PHY_CONTROLr(unit, ci, rval));
    sal_usleep(2000);

    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "C12. Wait for Phy Ready\n")));

    soc_timeout_init(&to, to_val, 0);
    do {
        SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit, ci, &rval));
        if (soc_reg_field_get(unit, CI_PHY_CONTROLr, rval, READYf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "CI%d: Timed out waiting for PHY Ready\n"), ci));
            return SOC_E_TIMEOUT;
        }
    } while (TRUE);

    /* MR0-MR2 configurations */
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "C13. CI DDR Init : MR0, MR1, MR2, and MR3 command\n")));
    dram0_clamshell_enable = soc_property_get(unit, spn_DRAM0_CLAMSHELL_ENABLE, 0);
    dram1_clamshell_enable = soc_property_get(unit, spn_DRAM1_CLAMSHELL_ENABLE, 0);

    if(dram0_clamshell_enable || dram1_clamshell_enable) {
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "     Manual Init\n")));

        if(dram0_clamshell_enable) {
            if(dram1_clamshell_enable) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Illegal clam shell configuration\n")));
                return SOC_E_FAIL;
            }

            ciCM = 0;    /* Only Even CIs Mirrored */
            ciCS = 1;    /* Only Odd CIs Straight */
        } else {
            ciCM = 1;    /* Only Odd CIs Mirrored */
            ciCS = 0;    /* Only Even CIs Straight */
        }

        if (ci == ciCM) {
            SOC_IF_ERROR_RETURN(READ_CI_DDR_MR0r(unit, ci, &rval));
            mr0 = soc_reg_field_get( unit, CI_DDR_MR0r, rval, Af );
            mr0 = (mr0 & 0x00007E07) | ((mr0 & 0x0A8) << 1) | ((mr0 & 0x150) >> 1);
            soc_reg_field_set( unit, CI_DDR_MR0r, &mr0, Af, mr0 );
            soc_reg_field_set( unit, CI_DDR_MR0r, &mr0, BAf, 0 );

            SOC_IF_ERROR_RETURN(READ_CI_DDR_MR1r(unit, ci, &rval));
            mr2 = soc_reg_field_get( unit, CI_DDR_MR1r, rval, Af );
            mr2 = (mr2 & 0x00007E07) | ((mr2 & 0x0A8) << 1) | ((mr2 & 0x150) >> 1);
            soc_reg_field_set( unit, CI_DDR_MR2r, &mr2, Af, mr2 );
            soc_reg_field_set( unit, CI_DDR_MR2r, &mr2, BAf, 2 );

            SOC_IF_ERROR_RETURN(READ_CI_DDR_MR2r(unit, ci, &rval));
            mr1 = soc_reg_field_get( unit, CI_DDR_MR2r, rval, Af );
            mr1 = (mr1 & 0x00007E07) | ((mr1 & 0x0A8) << 1) | ((mr1 & 0x150) >> 1);
            soc_reg_field_set( unit, CI_DDR_MR1r, &mr1, Af, mr1 );
            soc_reg_field_set( unit, CI_DDR_MR1r, &mr1, BAf, 1 );

            SOC_IF_ERROR_RETURN(READ_CI_DDR_MR3r(unit, ci, &rval));
            mr3 = soc_reg_field_get( unit, CI_DDR_MR3r, rval, Af );
            mr3 = (mr3 & 0x00007E07) | ((mr3 & 0x0A8) << 1) | ((mr3 & 0x150) >> 1);
            soc_reg_field_set( unit, CI_DDR_MR3r, &mr3, Af, mr3 );
            soc_reg_field_set( unit, CI_DDR_MR3r, &mr3, BAf, 3 );

            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR0r(unit, ci, mr0));
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR1r(unit, ci, mr1));
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR2r(unit, ci, mr2));
            SOC_IF_ERROR_RETURN(WRITE_CI_DDR_MR3r(unit, ci, mr3));
        }

        rval = 0;
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DONEf, 1 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, REQf, 1 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x0 );

        /* MR2 */
        if (ci == ciCS) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x22);
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        } else {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        }

        /* MR3 */
        if (ci == ciCS) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        } else {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        }
        
        /* MR1 */
        if (ci == ciCS) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x1 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        } else {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x2 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        }

        /* MR0 */
        if (ci == ciCS) {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x1 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0 );
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        } else {
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x2 );
            soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0);
            SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
        }
    
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 0x3 );
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_SELECTf, 0x3);
        soc_reg_field_set( unit, CI_MRS_CMDr, &rval, MRS_CMDf, 0x3);
        SOC_IF_ERROR_RETURN( WRITE_CI_MRS_CMDr( unit, ci, rval) );
    
    } else {
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "     Auto Init\n")));

        SOC_IF_ERROR_RETURN(READ_CI_MRS_CMDr(unit, ci, &rval));
        soc_reg_field_set(unit, CI_MRS_CMDr, &rval, DEVICE_SELECTf, 3);
        SOC_IF_ERROR_RETURN(WRITE_CI_MRS_CMDr(unit, ci, rval));

        SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit, ci, &rval));
        soc_reg_field_set(unit, CI_DDR_AUTOINITr, &rval, DONEf, 1);
        soc_reg_field_set(unit, CI_DDR_AUTOINITr, &rval, STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CI_DDR_AUTOINITr(unit, ci, rval));
        sal_usleep(1000);

        soc_timeout_init(&to, to_val, 0);
        do {
            SOC_IF_ERROR_RETURN(READ_CI_DDR_AUTOINITr(unit, ci, &rval));
            if (soc_reg_field_get(unit, CI_DDR_AUTOINITr, rval, DONEf)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "CI%d: Timed out DDR Init\n"), ci));
                return SOC_E_TIMEOUT;
            }
        } while (TRUE);
        
        /* Delay for TZQ init */
        sal_usleep(5000);
    }
    
    /* Clear CI_ERROR */
    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "C14. Clear Read/Write FIFO errors\n")));

    rval = 0;
    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_CORRECTED_ERRORf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_CTL_UNCORRECTED_ERRORf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_CORRECTED_ERRORf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_CTL_UNCORRECTED_ERRORf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, RFIFO_OVERFLOWf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, WFIFO_OVERFLOWf, 1);
    soc_reg_field_set(unit, CI_ERRORr, &rval, PHY_READY_EVENTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CI_ERRORr(unit, ci, rval));

    return SOC_E_NONE;
}

/* Fill shmoo CB structure */
static and28_shmoo_cbi_t sb2_shmoo_cbi = {
    soc_sb2_and28_shmoo_phy_reg_read,
    soc_sb2_and28_shmoo_phy_reg_write,
    soc_sb2_and28_shmoo_phy_reg_modify,
    soc_sb2_and28_shmoo_drc_bist_conf_set,
    soc_sb2_and28_shmoo_drc_bist_err_cnt,
    soc_sb2_and28_shmoo_dram_init
};

int soc_saber2_and28_info_config(int unit, soc_sb2_and28_info_t *drc_info, 
        and28_shmoo_dram_info_t *sdi) {
    int i, num_dram = 0;

    /* Calculate dram bitmap from number of DDR memories */
    num_dram = soc_property_get(unit, spn_EXT_RAM_PRESENT, 0);
    drc_info->dram_bitmap = 0;
    for( i = 0; i < num_dram; i++ ){
        drc_info->dram_bitmap |= (1 << i);
    }
    sdi->dram_bitmap = drc_info->dram_bitmap;
    sdi->interface_bitwidth = 16;
    sdi->split_bus = 1;

    drc_info->enable = (drc_info->dram_num == 0) ? FALSE : TRUE;
    drc_info->auto_tune = soc_property_get(unit, spn_DDR3_AUTO_TUNE, 0);

    drc_info->ref_clk_mhz = soc_property_get(unit, spn_SYNTH_DRAM_FREQ, 50);
    sdi->ref_clk_mhz = drc_info->ref_clk_mhz;
    drc_info->dram_freq = soc_property_get(unit, spn_DDR3_CLOCK_MHZ, 800);

    drc_info->data_rate_mbps = (2 * drc_info->dram_freq);

    if( drc_info->dram_freq == 667) {
        drc_info->data_rate_mbps = drc_info->data_rate_mbps - 1;
    }

    sdi->data_rate_mbps = drc_info->data_rate_mbps;

#if 0
    SOC_IF_ERROR_RETURN(_soc_dpp_drc_combo28_str_prop_ext_ram_type(unit, &drc_info->dram_type));
#endif
    drc_info->nof_columns = soc_property_get(unit, spn_EXT_RAM_COLUMNS, 1024);
    drc_info->nof_rows    = soc_property_get(unit, spn_EXT_RAM_ROWS, 32768);
    drc_info->nof_banks   = soc_property_get(unit, spn_EXT_RAM_BANKS, 0x8);
    
    sdi->num_columns = drc_info->nof_columns;
    sdi->num_rows = drc_info->nof_rows;
    sdi->num_banks = drc_info->nof_banks;
    sdi->num_bank_groups = 2;

    /* dramType and ctlType are constant for SB2 */
    sdi->dram_type = SHMOO_AND28_DRAM_TYPE_DDR3;
    sdi->ctl_type = SHMOO_AND28_CTL_TYPE_1;

    
    drc_info->gear_down_mode        = soc_property_get(unit, spn_EXT_RAM_GEAR_DOWN_MODE, 0x0);
    drc_info->abi                   = soc_property_get(unit, spn_EXT_RAM_ABI, 0x0);
    drc_info->write_dbi             = soc_property_get(unit, spn_EXT_RAM_WRITE_DBI, 0x0);
    drc_info->read_dbi              = soc_property_get(unit, spn_EXT_RAM_READ_DBI, 0x0);
    drc_info->cmd_par_latency       = soc_property_get(unit, spn_EXT_RAM_CMD_PAR_LATENCY, 0x0);
    drc_info->write_crc             = soc_property_get(unit, spn_EXT_RAM_WRITE_CRC, 0x0);
    drc_info->read_crc              = soc_property_get(unit, spn_EXT_RAM_READ_CRC, 0x0);
    
    drc_info->bist_enable = soc_property_suffix_num_get(unit, -1, spn_BIST_ENABLE, "dram", 0x1);
    
    return SOC_E_NONE;
}

int soc_sb2_ite_segment_gap_update(int unit, and28_shmoo_dram_info_t *sdi)
{
    uint32              rval;
    int                 freq_loc, core_clk_loc;
    uint32              ref_clk_freq = 50; /* in MHz */
    uint32              mdiv, ndiv;
    uint32              core_pll_clock; /* in MHz */

    SOC_IF_ERROR_RETURN(READ_TOP_SWITCH_FEATURE_ENABLE_2r(unit, &rval));
    mdiv = soc_reg_field_get(unit, TOP_SWITCH_FEATURE_ENABLE_2r, rval,
            BOND_CORE_PLL_CH0_MDIVf);
    ndiv = soc_reg_field_get(unit, TOP_SWITCH_FEATURE_ENABLE_2r, rval,
            BOND_CORE_PLL_NDIV_INTf);
    core_pll_clock = _shr_div32r((ref_clk_freq * ndiv), mdiv);
    core_clk_loc = _soc_sb2_clktoloc(core_pll_clock);
    freq_loc = _soc_ddr_sb2_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));
    if((freq_loc < 0) || (core_clk_loc < 0)) {
        /* Invalid frequency */
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_ITE_SEGMENTS_GAPr(unit, &rval));
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS6_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][0]);
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS5_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][1]);
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS4_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][2]);
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS3_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][3]);
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS2_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][4]);
    soc_reg_field_set(unit, ITE_SEGMENTS_GAPr, &rval,
            SEGMENTS1_GAPf, sb2_ite_segment_gap[freq_loc][core_clk_loc][5]);
    SOC_IF_ERROR_RETURN(WRITE_ITE_SEGMENTS_GAPr(unit, rval));

    return SOC_E_NONE;
}


int soc_sb2_and28_dram_init_reset(int unit) 
{
    uint32 rval;
    int ci = 0; 
    and28_shmoo_dram_info_t sdi;
    soc_sb2_and28_info_t *drc_info;

    sal_memset(&sdi, 0, sizeof(sdi));

    drc_info = sal_alloc(sizeof(soc_sb2_and28_info_t),"DRC info");
    sal_memset(drc_info, 0, sizeof(soc_sb2_and28_info_t));
    
    /* Get drc_info parameters */
    SOC_IF_ERROR_CLEAN_RETURN(
            soc_saber2_and28_info_config(unit, drc_info, &sdi),
            sal_free(drc_info));

    /* DRAM info set */
    SOC_IF_ERROR_CLEAN_RETURN(soc_and28_shmoo_dram_info_set(unit, &sdi),
            sal_free(drc_info));
    
    /* Shmoo callback function registration */
    SOC_IF_ERROR_CLEAN_RETURN(
            soc_and28_shmoo_interface_cb_register(unit, sb2_shmoo_cbi),
            sal_free(drc_info));
    
    for (ci=0; ci<2; ci++) {
        /* initiate master DRCs */
        SOC_IF_ERROR_CLEAN_RETURN(soc_and28_shmoo_phy_init(unit, ci),
                sal_free(drc_info));
    }

    LOG_INFO(BSL_LS_SOC_DDR,
             (BSL_META_U(unit,
                         "B01. Bring CI out of software reset\n")));

    for (ci=0; ci<2; ci++) {
        SOC_IF_ERROR_CLEAN_RETURN(READ_CI_RESETr(unit,ci,&rval),
                sal_free(drc_info));
        soc_reg_field_set(unit, CI_RESETr, &rval, TREX2_DEBUG_ENABLEf, 0);
        soc_reg_field_set(unit, CI_RESETr, &rval, SW_RESETf, 0);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_CI_RESETr(unit,ci,rval),
                sal_free(drc_info));
        sal_usleep(2000);
    }
    
    sal_free(drc_info);
    return SOC_E_NONE;
}
int soc_sb2_and28_dram_init_config(int unit) 
{
    uint32 rval;
    int ci = 0;
    and28_shmoo_dram_info_t sdi;
    and28_shmoo_config_param_t config_param;
    soc_sb2_and28_info_t *drc_info;

    sal_memset(&sdi, 0, sizeof(sdi));
    sal_memset(&config_param, 0, sizeof(config_param));

    drc_info = sal_alloc(sizeof(soc_sb2_and28_info_t),"DRC info");
    sal_memset(drc_info, 0, sizeof(soc_sb2_and28_info_t));
    
    /* Get drc_info parameters */
    SOC_IF_ERROR_CLEAN_RETURN(
            soc_saber2_and28_info_config(unit, drc_info, &sdi),
            sal_free(drc_info));

    /* DRAM info set */
    SOC_IF_ERROR_CLEAN_RETURN(
            soc_and28_shmoo_dram_info_set(unit, &sdi),
            sal_free(drc_info));
    
    for (ci=0; ci<2; ci++) {
        SOC_IF_ERROR_CLEAN_RETURN(
                soc_sb2_and28_shmoo_dram_init(unit, ci, 0),
                sal_free(drc_info));
        SOC_IF_ERROR_CLEAN_RETURN(
                soc_sb2_ci_config(unit, ci),
                sal_free(drc_info));
        SOC_IF_ERROR_CLEAN_RETURN(
                soc_sb2_and28_post_shmoo_dram_init(unit, ci),
                sal_free(drc_info));

        /* Config and release BIST */
        LOG_INFO(BSL_LS_SOC_DDR,
                 (BSL_META_U(unit,
                             "C15. Config and Release BIST from reset\n")));

        rval = 0;
        soc_reg_field_set(unit, BISTGENERALCONFIGURATIONSr,
                &rval, NUMCOLSf, 2);
        SOC_IF_ERROR_CLEAN_RETURN(
                WRITE_BISTGENERALCONFIGURATIONSr(unit, ci, rval),
                sal_free(drc_info));

        SOC_IF_ERROR_CLEAN_RETURN(
                READ_BISTCONFIGr(unit, ci, &rval), sal_free(drc_info));
        soc_reg_field_set(unit, BISTCONFIGr, &rval, BIST_RESETBf, 1);
        SOC_IF_ERROR_CLEAN_RETURN(
                WRITE_BISTCONFIGr(unit, ci, rval), sal_free(drc_info));

    }

    SOC_IF_ERROR_CLEAN_RETURN(
            soc_sb2_ite_segment_gap_update(unit, &sdi), sal_free(drc_info));


    ci=0;
    if (drc_info->auto_tune == 0x1) {
        SOC_IF_ERROR_CLEAN_RETURN(soc_and28_shmoo_ctl(unit, ci, -1, 0,
                                        SHMOO_AND28_ACTION_RUN,
                                        &(drc_info->shmoo_config_param[ci])),
                sal_free(drc_info));
    }
    else {
        rval = SOC_E_NONE;
        /* restore from the config variables */
        rval = soc_sb2_and28_dram_restorecfg(unit,
                    &(drc_info->shmoo_config_param[ci]));
        if (rval == SOC_E_NONE){
            rval = soc_and28_shmoo_ctl(unit, ci, -1, 0,
                        SHMOO_AND28_ACTION_RESTORE,
                        &(drc_info->shmoo_config_param[ci]));

            if (rval == SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                "DDR Tune Values Restored\n")));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                "DDR Tune Values Restore Failed\n")));
            }
        }
        else{
          LOG_CLI((BSL_META_U(unit,
                  "DDR Tune Values Restore Failed\n")));
        }
    }

    sal_free(drc_info);
    return SOC_E_NONE;
}
/* This function is used to store the and28_shmoo_config_param_t structure 
 * into config variables. Should be used only form diag shell*/
int soc_sb2_and28_dram_savecfg(int unit, and28_shmoo_config_param_t *config_param)
{
  char name_str[32], val_str[96];
  uint64 value64;
  uint32 value32_lo = 0;
  uint32 value32_hi = 0;
  uint16 value16 = 0;
  uint8  value8 = 0;

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;

  if (soc_mem_config_set == NULL) {
    return SOC_E_UNAVAIL;
  }

  if (config_param == NULL) {
    return SOC_E_PARAM;
  }

  /* uint16  control_regs_ad[SHMOO_AND28_PHY_NOF_AD] */
  value16 = (uint16) (*config_param).control_regs_ad[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_ad[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[3];
  value32_lo |= value16;
  value16 = 0;

  COMPILER_64_SET(value64, value32_hi, value32_lo);
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_AD00);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_ad[4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_ad[6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[7];
  value32_lo |= value16;
  value16 = 0;

  COMPILER_64_SET(value64, value32_hi, value32_lo);
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_AD04);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_ad[8];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[9];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_ad[10];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[11];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_AD08);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_ad[12];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[13];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_ad[14];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).control_regs_ad[15];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_AD12);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of  uint16  control_regs_ad[SHMOO_AND28_PHY_NOF_AD] */

  /* uint16  control_regs_ba[SHMOO_AND28_PHY_NOF_BA] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_ba[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_ba[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_ba[2];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_BA);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  control_regs_ba[SHMOO_AND28_PHY_NOF_BA] */

  /* uint16  control_regs_aux[SHMOO_AND28_PHY_NOF_AUX] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_aux[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_aux[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).control_regs_aux[2];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_AUX);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16 control_regs_aux[SHMOO_AND28_PHY_NOF_AUX] */

  /* uint16 control_regs_cs[SHMOO_AND28_PHY_NOF_CS] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_cs[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).control_regs_cs[1];
  value32_hi |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_CS);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16 control_regs_cs[SHMOO_AND28_PHY_NOF_CS] */

  /*
   * uint16  control_regs_par;
   * uint16  control_regs_ras_n;
   * uint16  control_regs_cas_n;
   * uint16  control_regs_cke;
   * uint16  control_regs_rst_n;
   * uint16  control_regs_odt;
   * uint16  control_regs_we_n;
   * uint32  control_regs_vref_dac_control;*/
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_par;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_PAR);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_ras_n;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_RAS_N);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_cas_n;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_CAS_N);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_cke;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_CKE);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_rst_n;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_RST_N);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_odt;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_ODT);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).control_regs_we_n;
  value32_hi |= value16;
  value16 = 0;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VDL_WE_N);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value32_hi = (uint32) (*config_param).control_regs_vref_dac_control;
  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_CTRL_VREF_DAC);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  /* END of uint16 control_regs_cs[SHMOO_AND28_PHY_NOF_CS] */

  /* uint16  wr_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dqsp[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsp[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dqsp[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsp[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQSP);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  wr_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dqsn[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQSN);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dqsn[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dqsn[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQSN);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_dq[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[0][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ0_BL0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[0][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[0][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ4_BL0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[1][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ0_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[1][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[1][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ4_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[2][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ0_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[2][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[2][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ4_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[3][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ0_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dq[3][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dq[3][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DQ4_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  wr_vdl_dq[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  wr_vdl_dm[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_dm[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dm[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_dm[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_dm[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_DM);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  .wr_vdl_dm[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_edc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).wr_vdl_edc[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_edc[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).wr_vdl_edc[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).wr_vdl_edc[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_VDL_EDC);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  wr_vdl_edc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint8 wr_chan_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value8 = (uint8) (*config_param).wr_chan_dly_cyc[0];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).wr_chan_dly_cyc[1];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).wr_chan_dly_cyc[2];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).wr_chan_dly_cyc[3];
  value32_hi |= value8;
  value8 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_WR_CHAN_DLY_CYC);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  wr_chan_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqsp[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqsp[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqsp[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqsp[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQSP);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqsn[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqsn[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqsn[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqsn[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQSN);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dqp[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[0][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][3];
  value32_lo |= value16;
  value16 = 0;

  COMPILER_64_SET(value64, value32_hi, value32_lo);

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP0_BL0);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[0][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[0][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP4_BL0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[1][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP0_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[1][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[1][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP4_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[2][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP0_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[2][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[2][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP4_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[3][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP0_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqp[3][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqp[3][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQP4_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  rd_vdl_dqp[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  rd_vdl_dqn[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[0][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN0_BL0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[0][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[0][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN4_BL0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[1][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN0_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[1][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[1][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN4_BL1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[2][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN0_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[2][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[2][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN4_BL2);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[3][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN0_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dqn[3][4];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][5];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][6];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dqn[3][7];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DQN4_BL3);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  rd_vdl_dqn[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  rd_vdl_dmp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dmp[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dmp[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dmp[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dmp[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DMP);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_vdl_dmp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dmn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_vdl_dmn[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dmn[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_vdl_dmn[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_vdl_dmn[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_VDL_DMN);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_vdl_dmn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_en_vdl_cs[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_PHY_NOF_CS] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_en_vdl_cs[0][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[0][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[1][0];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[1][1];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_EN_VDL_CS0);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_en_vdl_cs[2][0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[2][1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[3][0];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_en_vdl_cs[3][1];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_EN_VDL_CS1);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /* END of uint16  rd_en_vdl_cs[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_PHY_NOF_CS]; */

  /* uint16  rd_en_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value16 = (uint16) (*config_param).rd_en_dly_cyc[0];
  value32_hi |= value16;
  value16 = 0;
  value32_hi <<= 16;

  value16 = (uint16) (*config_param).rd_en_dly_cyc[1];
  value32_hi |= value16;
  value16 = 0;

  value16 = (uint16) (*config_param).rd_en_dly_cyc[2];
  value32_lo |= value16;
  value16 = 0;
  value32_lo <<= 16;

  value16 = (uint16) (*config_param).rd_en_dly_cyc[3];
  value32_lo |= value16;
  value16 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_EN_DLY_CYC);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_en_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_control[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  value8 = (uint8) (*config_param).rd_control[0];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).rd_control[1];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).rd_control[2];
  value32_hi |= value8;
  value8 = 0;
  value32_hi <<= 8;

  value8 = (uint8) (*config_param).rd_control[3];
  value32_hi |= value8;
  value8 = 0;

  sal_sprintf(name_str, "%s", spn_DDR3_TUNE_RD_CONTROL);
  COMPILER_64_SET(value64, value32_hi, value32_lo);
  soc_format_uint64(val_str, value64);
  if (soc_mem_config_set(name_str, val_str) < 0) {
    return SOC_E_MEMORY;
  }
  /*END of uint16  rd_control[SHMOO_AND28_BYTES_PER_INTERFACE] */

  return SOC_E_NONE;
}

/* This function is used to restore the and28_shmoo_config_param_t structure 
 * from config variables */
int soc_sb2_and28_dram_restorecfg(int unit, and28_shmoo_config_param_t *config_param)
{
  uint64 value64;
  uint32 value32_lo = 0;
  uint32 value32_hi = 0;
  char* svalue = NULL;

  and28_shmoo_config_param_t null_config_param;
  sal_memset(&null_config_param, 0, sizeof(null_config_param));

  if (config_param == NULL) {
    return SOC_E_PARAM;
  }

  /* uint16  control_regs_ad[SHMOO_AND28_PHY_NOF_AD] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_AD00);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ad[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_ad[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_AD04);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ad[4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[5] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_ad[6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[7] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_AD08);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ad[8] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[9] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_ad[10] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[11] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_AD12);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ad[12] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[13] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_ad[14] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ad[15] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of  uint16  control_regs_ad[SHMOO_AND28_PHY_NOF_AD] */

  /* uint16  control_regs_ba[SHMOO_AND28_PHY_NOF_BA] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_BA);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ba[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_ba[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_ba[2] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16  control_regs_ba[SHMOO_AND28_PHY_NOF_BA] */

  /* uint16  control_regs_aux[SHMOO_AND28_PHY_NOF_AUX] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_AUX);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_aux[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_aux[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).control_regs_aux[2] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16 control_regs_aux[SHMOO_AND28_PHY_NOF_AUX] */

  /* uint16 control_regs_cs[SHMOO_AND28_PHY_NOF_CS] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_CS);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_cs[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).control_regs_cs[1] = (uint16) (value32_hi & 0xFFFF); 
  }
  /* END of uint16 control_regs_cs[SHMOO_AND28_PHY_NOF_CS] */

  /*
   * uint16  control_regs_par;
   * uint16  control_regs_ras_n;
   * uint16  control_regs_cas_n;
   * uint16  control_regs_cke;
   * uint16  control_regs_rst_n;
   * uint16  control_regs_odt;
   * uint16  control_regs_we_n;
   * uint32  control_regs_vref_dac_control;*/
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_PAR);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_par = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_RAS_N);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_ras_n = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_CAS_N);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_cas_n = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_CKE);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_cke = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_RST_N);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_rst_n = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_ODT);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_odt = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VDL_WE_N);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_we_n = (uint16) (value32_hi & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VREF_DAC);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).control_regs_vref_dac_control = (uint32) (value32_hi); 
  }
  /* END */ 

  /* uint16  wr_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQSP);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dqsp[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dqsp[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dqsp[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dqsp[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16  wr_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQSN);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dqsn[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dqsn[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dqsn[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dqsn[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16  wr_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_dq[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ0_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[0][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[0][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[0][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[0][3] = (uint16) (value32_lo & 0xFFFF); 
  }

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ4_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[0][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[0][5] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[0][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[0][7] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ0_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[1][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[1][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[1][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[1][3] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ4_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[1][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[1][5] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[1][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[1][7] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ0_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[2][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[2][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[2][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[2][3] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ4_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[2][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[2][5] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[2][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[2][7] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ0_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[3][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[3][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[3][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[3][3] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DQ4_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dq[3][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dq[3][5] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dq[3][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dq[3][7] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16  wr_vdl_dq[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  wr_vdl_dm[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_DM);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_dm[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_dm[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_dm[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_dm[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  .wr_vdl_dm[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  wr_vdl_edc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_VDL_EDC);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_vdl_edc[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_vdl_edc[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_vdl_edc[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_vdl_edc[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  wr_vdl_edc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint8 wr_chan_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_WR_CHAN_DLY_CYC);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).wr_chan_dly_cyc[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).wr_chan_dly_cyc[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).wr_chan_dly_cyc[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).wr_chan_dly_cyc[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  wr_chan_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_CTRL_VREF_DAC);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);
    (*config_param).control_regs_vref_dac_control = (uint32) value32_hi;
  }
  /* uint16  rd_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQSP);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqsp[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).rd_vdl_dqsp[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_vdl_dqsp[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqsp[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  rd_vdl_dqsp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQSN);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqsn[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).rd_vdl_dqsn[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_vdl_dqsn[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqsn[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  rd_vdl_dqsn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dqp[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP0_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[0][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[0][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[0][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[0][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP4_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[0][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[0][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[0][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[0][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP0_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[1][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[1][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[1][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[1][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP4_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[1][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[1][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[1][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[1][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP0_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[2][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[2][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[2][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[2][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP4_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[2][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[2][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[2][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[2][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP0_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[3][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[3][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[3][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[3][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQP4_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqp[3][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[3][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqp[3][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqp[3][7] = (uint16) (value32_lo & 0xFFFF);
  }
  /* END of uint16  rd_vdl_dqp[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  rd_vdl_dqn[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN0_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[0][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[0][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[0][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[0][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN4_BL0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[0][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).rd_vdl_dqn[0][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[0][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[0][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN0_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[1][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[1][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[1][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[1][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN4_BL1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[1][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[1][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[1][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[1][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN0_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[2][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[2][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[2][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[2][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN4_BL2);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[2][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[2][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[2][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[2][7] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN0_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[3][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[3][1] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[3][2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[3][3] = (uint16) (value32_lo & 0xFFFF);
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DQN4_BL3);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dqn[3][4] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[3][5] = (uint16) (value32_hi & 0xFFFF);
    (*config_param).rd_vdl_dqn[3][6] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dqn[3][7] = (uint16) (value32_lo & 0xFFFF);
  }
  /* END of uint16  rd_vdl_dqn[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_BYTE]; */

  /* uint16  rd_vdl_dmp[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DMP);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dmp[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dmp[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_vdl_dmp[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dmp[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  rd_vdl_dmp[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_vdl_dmn[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_VDL_DMN);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_vdl_dmn[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dmn[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_vdl_dmn[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_vdl_dmn[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  rd_vdl_dmn[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_en_vdl_cs[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_PHY_NOF_CS] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_EN_VDL_CS0);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_en_vdl_cs[0][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_en_vdl_cs[0][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_en_vdl_cs[1][0] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_en_vdl_cs[1][1] = (uint16) (value32_lo & 0xFFFF); 
  }
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_EN_VDL_CS1);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_en_vdl_cs[2][0] = (uint16) ((value32_hi & 0xFFFF0000)>>16); 
    (*config_param).rd_en_vdl_cs[2][1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_en_vdl_cs[3][0] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_en_vdl_cs[3][1] = (uint16) (value32_lo & 0xFFFF); 
  }
  /* END of uint16  rd_en_vdl_cs[SHMOO_AND28_BYTES_PER_INTERFACE][SHMOO_AND28_PHY_NOF_CS]; */

  /* uint16  rd_en_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_EN_DLY_CYC);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_en_dly_cyc[0] = (uint16) ((value32_hi & 0xFFFF0000)>>16);
    (*config_param).rd_en_dly_cyc[1] = (uint16) (value32_hi & 0xFFFF); 
    (*config_param).rd_en_dly_cyc[2] = (uint16) ((value32_lo & 0xFFFF0000)>>16); 
    (*config_param).rd_en_dly_cyc[3] = (uint16) (value32_lo & 0xFFFF); 
  }
  /*END of uint16  rd_en_dly_cyc[SHMOO_AND28_BYTES_PER_INTERFACE] */

  /* uint16  rd_control[SHMOO_AND28_BYTES_PER_INTERFACE] */
  COMPILER_64_ZERO(value64);value32_lo = 0;value32_hi = 0;
  svalue = soc_property_get_str(unit, spn_DDR3_TUNE_RD_CONTROL);
  if (svalue != NULL){
    value64 = soc_parse_uint64(svalue);
    COMPILER_64_TO_32_LO(value32_lo,value64);
    COMPILER_64_TO_32_HI(value32_hi,value64);

    (*config_param).rd_control[0] = (uint8) ((value32_hi & 0xFF000000)>>24);
    (*config_param).rd_control[1] = (uint8) ((value32_hi & 0xFF0000)>>16); 
    (*config_param).rd_control[2] = (uint8) ((value32_hi & 0xFF00)>>8); 
    (*config_param).rd_control[3] = (uint8) (value32_hi & 0xFF); 
  }
  /*END of uint16  rd_control[SHMOO_AND28_BYTES_PER_INTERFACE] */
  if(sal_memcmp(&null_config_param, config_param, sizeof(null_config_param)) == 0) {
      return SOC_E_FAIL;
  }
  return SOC_E_NONE;
}

#endif
