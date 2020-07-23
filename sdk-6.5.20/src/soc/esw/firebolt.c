/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt.c
 * Purpose:
 * Requires:    
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/firebolt.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/lpm.h>
#include <soc/error.h>
#include <soc/phyctrl.h>
#include <soc/debug.h>
#ifdef  BCM_RAVEN_SUPPORT
#include <soc/post.h>
#endif  /* BCM_RAVEN_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

/*
 * firebolt chip driver functions.  
 */
soc_functions_t soc_firebolt_drv_funs = {
    soc_firebolt_misc_init,
    soc_firebolt_mmu_init,
    soc_firebolt_age_timer_get,
    soc_firebolt_age_timer_max_get,
    soc_firebolt_age_timer_set,
};

static soc_ser_functions_t _fb_ser_functions;

STATIC int
soc_firebolt_cam_sam_config(int unit, int sam)
{
    uint32 val;

    SOC_IF_ERROR_RETURN(READ_L2_USER_ENTRY_CAM_CONTROLr(unit, &val));
    soc_reg_field_set(unit, L2_USER_ENTRY_CAM_CONTROLr, &val, SAMf, sam);
    SOC_IF_ERROR_RETURN(WRITE_L2_USER_ENTRY_CAM_CONTROLr(unit, val));

    SOC_IF_ERROR_RETURN(READ_L3_TUNNEL_CAM_CONTROLr(unit, &val));
    soc_reg_field_set(unit, L3_TUNNEL_CAM_CONTROLr, &val, SAMf, sam);
    SOC_IF_ERROR_RETURN(WRITE_L3_TUNNEL_CAM_CONTROLr(unit, val));

    SOC_IF_ERROR_RETURN(READ_VLAN_SUBNET_CAM_CONTROLr(unit, &val));
    soc_reg_field_set(unit, VLAN_SUBNET_CAM_CONTROLr, &val, SAMf, sam);
    SOC_IF_ERROR_RETURN(WRITE_VLAN_SUBNET_CAM_CONTROLr(unit, val));

    SOC_IF_ERROR_RETURN(READ_L3_DEFIP_CAM_CONTROL0r(unit, &val));
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM0_SAMf, sam);
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM1_SAMf, sam);
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM2_SAMf, sam);
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM3_SAMf, sam);
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM4_SAMf, sam);
    soc_reg_field_set(unit, L3_DEFIP_CAM_CONTROL0r, &val, CAM5_SAMf, sam);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_CONTROL0r(unit, val));

    SOC_IF_ERROR_RETURN(READ_FP_CAM_CONTROL_LOWERr(unit, &val));
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_0f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_1f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_2f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_3f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_4f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_5f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_6f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_LOWERr, &val,
             FP_CAM_CONTROL_SLICE_7f, sam);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_LOWERr(unit, val));

    SOC_IF_ERROR_RETURN(READ_FP_CAM_CONTROL_UPPERr(unit, &val));
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_8f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_9f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_10f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_11f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_12f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_13f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_14f, sam);
    soc_reg_field_set(unit, FP_CAM_CONTROL_UPPERr, &val,
                         FP_CAM_CONTROL_SLICE_15f, sam);
    SOC_IF_ERROR_RETURN(WRITE_FP_CAM_CONTROL_UPPERr(unit, val));

    return SOC_E_NONE;
}
STATIC int
soc_firebolt_pipe_mem_clear(int unit)
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
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x4000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x2000);
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
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    return SOC_E_NONE;
}

#if defined (BCM_FIREBOLT2_SUPPORT)
/* SER processing for TCAMs */
STATIC _soc_ser_parity_info_t _soc_fb2_ser_parity_info[] = {
    { EFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1,
        -1, 370, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { FP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1,
        -1, 370, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { VFP_TCAMm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1,
        -1, 370, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { L3_TUNNELm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1,
        -1, 183, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { VLAN_SUBNETm, _SOC_SER_PARITY_MODE_2BITS,
        -1, -1, -1, -1,
        -1, 144, INVALIDr, 0, 0, 0, _SOC_SER_FLAG_SW_COMPARE},
    { INVALIDm, _SOC_SER_PARITY_MODE_NUM},
};

#endif /* BCM_FIREBOLT2_SUPPORT */

#ifdef SOC_MEM_L3_DEFIP_WAR
/*
 * Support to map out unused blocks in L3_DEFIP Table
 */
#define L3_DEFIP_CAM_NUM_INSTANCE       6
/*
 * L3_DEFIP CAM BIST
 */
STATIC int
soc_fb_l3_defip_bist(int unit, int instance)
{
    static int bist_enf[L3_DEFIP_CAM_NUM_INSTANCE] = {
                            BIST_EN0f,
                            BIST_EN1f,
                            BIST_EN2f,
                            BIST_EN3f,
                            BIST_EN4f,
                            BIST_EN5f
                            };
/*
    static int cam_samf[L3_DEFIP_CAM_NUM_INSTANCE] =
                        {   CAM0_SAMf,
                            CAM1_SAMf,
                            CAM2_SAMf,
                            CAM3_SAMf,
                            CAM4_SAMf,
                            CAM5_SAMf
                            };
*/
    static int cam_bist_statr[] = {
            L3_DEFIP_CAM_BIST_S2_STATUSr,
            L3_DEFIP_CAM_BIST_S3_STATUSr,
            L3_DEFIP_CAM_BIST_S5_STATUSr,
            L3_DEFIP_CAM_BIST_S6_STATUSr,
            L3_DEFIP_CAM_BIST_S8_STATUSr,
            L3_DEFIP_CAM_BIST_S10_STATUSr,
            INVALIDr
            };
    uint32      cam_stat = 0;
    uint32      cam_ctrl = 0;
    int         i;
    int         rv = SOC_E_NONE;
    soc_timeout_t       to;

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_CONTROL1r(unit, cam_ctrl));

    /* Enable BIST */
    soc_reg_field_set(unit,
                      L3_DEFIP_CAM_CONTROL1r,
                      &cam_ctrl,
                      bist_enf[instance],
                      1);
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_CONTROL1r(unit, cam_ctrl));

    /* Poll for BIST done */
    soc_timeout_init(&to, 50000, 0);   /* 50 msec */
    do {
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_CAM_BIST_STATUSr(unit, &cam_stat));
        if (soc_reg_field_get(unit, L3_DEFIP_CAM_BIST_STATUSr,
                              cam_stat, BIST_DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : DEFIP_CAM_BIST timeout\n"), unit));
            rv = SOC_E_INTERNAL;
            goto done;
        }
    } while (TRUE);

    for(i = 0; cam_bist_statr[i] != INVALIDr; i++) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, cam_bist_statr[i],
                                          REG_PORT_ANY, 0,
                                          &cam_stat));
        if (cam_stat != 0) {
            rv = SOC_E_INTERNAL;
            goto done;
        }
    }

done:
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_CONTROL1r(unit, 0));
    return rv;
}

#define spn_L3_DEFIP_MAP  "l3_defip_map"

/*
 * Resize the L3_DEFIP tables to account for unused Blocks
 */
STATIC int
soc_fb_l3_defip_resize(int unit)
{
    soc_persist_t       *sop = SOC_PERSIST(unit);
    int                 i;
    int                 unused_count = 0;
    uint32              cam_en = 0;
    uint32              l3_defip_map = 0;
    static int cam_enf[L3_DEFIP_CAM_NUM_INSTANCE] =
                        {   CAM_0_ENABLEf,
                            CAM_1_ENABLEf,
                            CAM_2_ENABLEf,
                            CAM_3_ENABLEf,
                            CAM_4_ENABLEf,
                            CAM_5_ENABLEf
                            };

    /* Use all instances by deafult */
    sop->l3_defip_map = soc_property_get(unit, spn_L3_DEFIP_MAP, 0x00);

    /*
     *  If config property is not specified the use SAM = 3 and do not
     *  resize the DEFIP Table
     */
    if ((soc_property_get(unit, spn_L3_DEFIP_MAP, 0xFF) == 0xFF) &&
        (soc_property_get(unit, spn_L3_DEFIP_MAP, 0xF0) == 0xF0)) {
        return SOC_E_NONE;
    }

    /* Always use SAM = 0 */
    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_CONTROL0r(unit, 0));

    for(i = 0; i < L3_DEFIP_CAM_NUM_INSTANCE; i++) {
        if (soc_fb_l3_defip_bist(unit, i) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit = %d, Recommend Mapping out L3_DEFIP block %d\n"),
                       unit, i));
            l3_defip_map |= (1 << i);
        }
        if (sop->l3_defip_map & (1 << i)) { /* Map out the instance */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit = %d, Mapped out L3_DEFIP block %d\n"),
                       unit, i));
            unused_count++;
            continue;
        }
        soc_reg_field_set(unit,
                          L3_DEFIP_CAM_ENABLEr,
                          &cam_en,
                          cam_enf[i],
                          1);
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Recommend config add  %s.%d=0x%02x); config save\n"),
               spn_L3_DEFIP_MAP, unit, l3_defip_map));

    SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_CAM_ENABLEr(unit, cam_en));
    sop->memState[L3_DEFIPm].index_max =
                        SOC_MEM_INFO(unit, L3_DEFIPm).index_max -
                        (unused_count << 10);
    sop->memState[L3_DEFIP_ONLYm].index_max =
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                    sop->memState[L3_DEFIPm].index_max; 

    return SOC_E_NONE;
}

/*
 * Map an index in L3_DEFIP to the real h/w index to make by jumping over
 * unused 1K blocks to make the table appear contiguous
 */

int
soc_fb_l3_defip_index_map(int unit, int index)
{
    uint32              map = SOC_PERSIST(unit)->l3_defip_map;
    int                 offset;
    int                 good_block;

    good_block = 0;
    offset = -1;
    do {
        offset++;
        if (map & (1 << offset))  {
            continue;
        }
        good_block++;
    } while(good_block <= (index >> 10));
        
    return(index + ((offset + 1 - good_block) << 10));
}
#endif

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
int
soc_raptor_hw_linkscan_map_init(int unit)
{
    soc_port_t  port;
    uint32      oregv = 0;
    uint32      regv = 0;
    soc_reg_t   reg = CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r;
    soc_reg_t   map_r[16] = {   CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r,
                                CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r
                                };
    soc_field_t   map_f[4] = {  PHY_ID_0f, PHY_ID_1f, PHY_ID_2f, PHY_ID_3f};

    /* Re-initialize the phy port map for the unit */
    PBMP_PORT_ITER(unit, port) {
        /* Use MDIO address re-mapping for hardware linkscan */ 
        assert((uint32)(port / 4) < (sizeof(map_r)/sizeof(map_r[0])));
        reg = map_r[port / 4];
        regv = soc_pci_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0));
        oregv = regv;
        soc_reg_field_set(unit,
                      CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r,
                      &regv, map_f[port % 4],
                      PHY_ADDR(unit, port) & 0x1f);
        if (oregv != regv) {
            soc_pci_write(unit,
                          soc_reg_addr(unit, reg, REG_PORT_ANY, 0),
                          regv);
        }

    }
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CONFIGr(unit, &regv));
    oregv = regv;
    soc_reg_field_set(unit, CMIC_CONFIGr, &regv, MIIM_ADDR_MAP_ENABLEf, 1);
    if (oregv != regv) {
        WRITE_CMIC_CONFIGr(unit, regv);
    }

    return SOC_E_NONE;
}
#endif


int
soc_firebolt_misc_init(int unit)
{
    uint32              rval, orval, prev_reg_addr;
    int                 port;
    uint32              imask;
    uint32              misc_cfg;
    int                 divisor, dividend;

#if defined(BCM_HAWKEYE_SUPPORT)
    if(SOC_IS_HAWKEYE(unit)) {
        /* Disable HW based idle mode power down feature by default */
        if (soc_reg_field_valid(unit, CMIC_QGPHY_QSGMII_CONTROLr, DISABLE_HW_IDLE_PWRDWNf)) {
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN(READ_CMIC_QGPHY_QSGMII_CONTROLr(unit, &rval));
            soc_reg_field_set(unit,
                                  CMIC_QGPHY_QSGMII_CONTROLr,
                                  &rval,
                                  DISABLE_HW_IDLE_PWRDWNf,
                                  1);
            SOC_IF_ERROR_RETURN(WRITE_CMIC_QGPHY_QSGMII_CONTROLr(unit, rval));
        }
 
        /*
         * To set bit ECO_EEE_LINKUP_CONFIG_EN to 0.
         * This is for SW one second delay on HKEEE,
         * and this needs to happen before PHY is enabled to negotiate the link.
         * Otherwise, the EEE_DELAY_ENTRY_TIMER could be wrong sometimes.
         */
        PBMP_PORT_ITER(unit, port) {
            if(SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
                SOC_IF_ERROR_RETURN(
                       READ_UMAC_EEE_CTRLr(unit, port, &rval));
                soc_reg_field_set(unit, UMAC_EEE_CTRLr, &rval,
                                  ECO_EEE_LINKUP_CONFIG_ENf, 0);
                SOC_IF_ERROR_RETURN(
                       WRITE_UMAC_EEE_CTRLr(unit, port, rval));
            }
        }

        /* Enable fp slice */
        SOC_IF_ERROR_RETURN(READ_FP_SLICE_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_SLICE_ENABLE_SLICE_0f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_SLICE_ENABLE_SLICE_1f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_SLICE_ENABLE_SLICE_2f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_SLICE_ENABLE_SLICE_3f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_LOOKUP_ENABLE_SLICE_0f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_LOOKUP_ENABLE_SLICE_1f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_LOOKUP_ENABLE_SLICE_2f, 1);
        soc_reg_field_set(unit, FP_SLICE_ENABLEr, &rval, FP_LOOKUP_ENABLE_SLICE_3f, 1);
        SOC_IF_ERROR_RETURN(WRITE_FP_SLICE_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_ING_MISC_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ING_MISC_CONFIGr, &rval, USE_EPC_LINK_BMPf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_MISC_CONFIGr(unit, rval));
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &rval));
            soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
        }
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLE_SELECTr(unit, &rval));
        soc_reg_field_set(unit, EGR_ENABLE_SELECTr, &rval, ENABLE_SELECTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLE_SELECTr(unit, rval));

        /* Set test mode bits to 0x10 for all FP TCAM */
        rval = 0x10;
        SOC_IF_ERROR_RETURN(WRITE_FP_NON_ROTATED_CAM_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_FP_ROTATED_CAM_CONTROLr(unit, rval));
        /* QSGMII Pause Work aroud */
        rval = 0x0;
        SOC_IF_ERROR_RETURN(WRITE_CMIC_TX_PAUSE_CAPABILITYr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RX_PAUSE_CAPABILITYr(unit, rval));
        rval = 0xffffffff; /* all pbmp */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_TX_PAUSE_OVERRIDE_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RX_PAUSE_OVERRIDE_CONTROLr(unit, rval));
    }
#endif

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        soc_persist_t       *sop;
        uint32              entry_mask;

        sop = SOC_PERSIST(unit);

        /* This is really a bucket mask */
        entry_mask = sop->memState[L2_HITDA_ONLYm].index_max;
        SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_ADDR_MASKr(unit, entry_mask));

        if (SOC_REG_IS_VALID(unit, L3_ENTRY_ADDR_MASKr)) {
            /* This is really a bucket mask */
            entry_mask = sop->memState[L3_ENTRY_VALID_ONLYm].index_max;
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_ADDR_MASKr(unit, entry_mask));
        }

        if (SOC_REG_IS_VALID(unit, VLAN_STG_ADDR_MASKr)) {
            entry_mask = sop->memState[STG_TABm].index_max;
            SOC_IF_ERROR_RETURN(WRITE_VLAN_STG_ADDR_MASKr(unit, entry_mask));
            SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_STG_ADDR_MASKr(unit, entry_mask));
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined (BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    }
#endif

    if (soc_feature(unit, soc_feature_l2x_parity)) {
        /* Enable L2_ENTRY/L3_ENTRY parity error interrupt */
        SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_CONTROLr(unit, &rval));
        orval = rval;
        soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 0);
        if (orval != rval) {
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, rval));
        }
        soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_ENf, 1);
        soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, rval));
    }

#if defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l3x_parity)) {
        SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, rval));
        soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_ENf, 1);
        soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, rval));
    }
#endif /* INCLUDE_L3 */

    if (soc_feature(unit, soc_feature_l3defip_parity)) {
        /* Enable L3_DEFIP parity error interrupt */
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, rval));
        soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &rval,
                          PARITY_ENf, 1);
        soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &rval,
                          PARITY_IRQ_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, rval));
    }

#ifdef BCM_RAVEN_SUPPORT
    if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
        int i;
        soc_reg_t reg[10] = {DSCP_TABLE_PARITY_CONTROLr, EGR_L3_INTF_PARITY_CONTROLr,
                             EGR_MASK_PARITY_CONTROLr, EGR_NEXT_HOP_PARITY_CONTROLr,
                             EGR_VLAN_TABLE_PARITY_CONTROLr, FP_POLICY_PARITY_CONTROLr,
                             ING_L3_NEXT_HOP_PARITY_CONTROLr, L2MC_PARITY_CONTROLr,
                             L3_IPMC_PARITY_CONTROLr, VLAN_PARITY_CONTROLr};
        for (i = 0; i < 10 ; i++) {
            /* Skip non-existed registers for HAWKEYE */ 
            if (!SOC_REG_IS_VALID(unit, reg[i])) {
                continue;
            }
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg[i], REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, reg[i], &rval, PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, rval));
            if (reg[i] == FP_POLICY_PARITY_CONTROLr) {
#if defined(BCM_HAWKEYE_SUPPORT)
                if(SOC_IS_HAWKEYE(unit)) {
                    soc_reg_field_set(unit, reg[i], &rval, PARITY_ENf, 0xf);
                    soc_reg_field_set(unit, reg[i], &rval, PARITY_IRQ_ENf, 0xf);
                } else
#endif /* BCM_HAWKEYE_SUPPORT */
                {
                    soc_reg_field_set(unit, reg[i], &rval, PARITY_ENf, 0xffff);
                    soc_reg_field_set(unit, reg[i], &rval, PARITY_IRQ_ENf, 0xffff);
                }
            } else { 
                soc_reg_field_set(unit, reg[i], &rval, PARITY_ENf, 1);
                soc_reg_field_set(unit, reg[i], &rval, PARITY_IRQ_ENf, 1);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, rval));
        }
        SOC_IF_ERROR_RETURN(READ_ING_MISC_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ING_MISC_CONFIGr, &rval,
                          DROP_PACKET_ON_PARITY_ERRORf, 1);        
        SOC_IF_ERROR_RETURN(WRITE_ING_MISC_CONFIGr(unit, rval));
    }
    if (SOC_REG_IS_VALID(unit, ILPM_TM_REG_1r)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ILPM_TM_REG_1r, REG_PORT_ANY, 
                                                   TM_L3_DEFIPf, 0x10));
    }
    if (SOC_REG_IS_VALID(unit, ILPM_TM_REG_2r)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ILPM_TM_REG_2r, REG_PORT_ANY, 
                                                   TM_L3_DEFIPf, 0x10));
    }
    if (SOC_REG_IS_VALID(unit, FP_NON_ROTATED_CAM_CONTROLr)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, FP_NON_ROTATED_CAM_CONTROLr, REG_PORT_ANY, 
                                                   FP_NON_ROTATED_CAM_CONTROL_f, 0x10));
    }
    if (SOC_REG_IS_VALID(unit, FP_ROTATED_CAM_CONTROLr)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, FP_ROTATED_CAM_CONTROLr, REG_PORT_ANY, 
                                                   FP_ROTATED_CAM_CONTROL_f, 0x10));
    }
#endif /* BCM_RAVEN_SUPPORT */

    /*
     * Only on FB.
     */
    if (SOC_IS_FIREBOLT(unit)) {
        SOC_IF_ERROR_RETURN(soc_firebolt_cam_sam_config(unit, 0));
    }
    
#ifdef SOC_MEM_L3_DEFIP_WAR
    /* Test L3_DEFIP and adjust table size */
    if (soc_feature(unit, soc_feature_l3_defip_map)) {
        SOC_IF_ERROR_RETURN(soc_fb_l3_defip_resize(unit));
    }
#endif

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_firebolt_pipe_mem_clear(unit));
    }

#if defined(BCM_RAPTOR_SUPPORT) 
    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, PORT_OR_TRUNK_MAC_LIMITm, COPYNO_ALL, TRUE));
        SOC_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, 0x3fff));
    }
#endif /* BCM_RAPTOR_SUPPORT */

    /* Enable MMU parity error interrupt */
    SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTSTATUSr(unit, 0));
    SOC_IF_ERROR_RETURN(READ_MEMFAILINTMASKr(unit, &imask));
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CFAPPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CCPPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CBPPKTHDRPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CBPCELLHDRPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      XQPARITYERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CRCERRORINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CFAPFAILINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      SOFTRESETINTMASKf, 1);
    soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                      CELLNOTIPINTMASKf, 1);
    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        soc_reg_field_set(unit, MEMFAILINTMASKr, &imask,
                          IPMCREPOVERLIMITERRORINTMASKf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTMASKr(unit, imask));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &misc_cfg));
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                      PARITY_CHECK_ENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                      CELLCRCCHECKENf, 1);
    soc_reg_field_set(unit, MISCCONFIGr, &misc_cfg,
                      METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, misc_cfg));

    SOC_IF_ERROR_RETURN(READ_MEMFAILINTSTATUSr(unit, &imask));
    soc_intr_enable(unit, IRQ_MEM_FAIL);

#if defined (BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
        if (soc_feature(unit, soc_feature_ser_parity)) {
#ifdef INCLUDE_MEM_SCAN        
            soc_mem_scan_ser_list_register(unit, FALSE,
                    _soc_fb2_ser_parity_info);
#endif /* INCLUDE_MEM_SCAN */                    
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */


    memset(&_fb_ser_functions, 0, sizeof(soc_ser_functions_t));
    _fb_ser_functions._soc_ser_parity_error_intr_f = &soc_fb_mmu_parity_error;
    soc_ser_function_register(unit, &_fb_ser_functions);

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        soc_field_t fields[3];
        uint32 values[3];

        /* Enable dual hash on L2 and L3 tables */
        fields[0] = ENABLEf;
        values[0] = 1;
        fields[1] = HASH_SELECTf;
        values[1] = FB_HASH_CRC32_LOWER;
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY,
                                     2, fields, values));

        if (!SOC_IS_HAWKEYE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr,
                                         REG_PORT_ANY, 2, fields, values));
        }
        if (!SOC_IS_FIREBOLT2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, VLAN_MAC_AUX_HASH_CONTROLr,
                                         REG_PORT_ANY, 2, fields, values));
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

    /*
     * Egress Enable
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_ENABLEr, &rval, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, rval));
    }

    rval = 0;
    soc_reg_field_set(unit, EPC_LINK_BMAPr, &rval, PORT_BITMAPf,
                          SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPr(unit, rval));
#if defined(BCM_RAPTOR_SUPPORT) 
    if (soc_feature(unit, soc_feature_register_hi)) {
        soc_reg_field_set(unit, EPC_LINK_BMAP_HIr, &rval, PORT_BITMAPf,
                              SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 1));
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_HIr(unit, rval));
    }
#endif /* BCM_RAPTOR_SUPPORT */

    /* GMAC init should be moved to mac */
    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    prev_reg_addr = 0xffffffff;
    PBMP_GE_ITER(unit, port) {
        uint32  reg_addr;
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    /* Felix */
    prev_reg_addr = 0xffffffff;
    PBMP_FE_ITER(unit, port) {
        uint32  reg_addr;
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_GE_ITER(unit, port) {
        uint32  reg_addr;
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    /* Felix */
    PBMP_FE_ITER(unit, port) {
        uint32  reg_addr;
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        if (SOC_PORT_VALID(unit, 1)) {
            soc_reg_field_set(unit,
                              GPORT_CONFIGr,
                              &rval,
                              HGIG2_EN_S0f,
                              IS_ST_PORT(unit, 1) ? 1 : 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            if (SOC_IS_RAPTOR(unit)) {
                /* Select LCPLL output as the reference clock for TxPll. */
                soc_reg_field_set(unit, GPORT_CONFIGr, &rval,
                                  SEL_LCPLL_S0f, 1);
            } 
#endif
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, 1, rval));
        }
        if (SOC_PORT_VALID(unit, 2)) {
            soc_reg_field_set(unit,
                              GPORT_CONFIGr,
                              &rval,
                              HGIG2_EN_S1f,
                              IS_ST_PORT(unit, 2) ? 1 : 0);
#if defined(BCM_RAPTOR1_SUPPORT)
            if (SOC_IS_RAPTOR(unit)) {
                /* Select LCPLL output as the reference clock for TxPll. */
                soc_reg_field_set(unit, GPORT_CONFIGr, &rval,
                                  SEL_LCPLL_S1f, 1);
            }
#endif
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, 2, rval));
        }
#if defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
            if (SOC_PORT_VALID(unit, 4)) {
                soc_reg_field_set(unit,
                                  GPORT_CONFIGr,
                                  &rval,
                                  HGIG2_EN_S3f,
                                  IS_ST_PORT(unit, 4) ? 1 : 0);
                SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, 3, rval));
            }
            /* coverity[result_independent_of_operands] */              
            if (SOC_PORT_VALID(unit, 5)) {
                soc_reg_field_set(unit,
                                  GPORT_CONFIGr,
                                  &rval,
                                  HGIG2_EN_S4f,
                                  IS_ST_PORT(unit, 5) ? 1 : 0);
                SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, 4, rval));
            }
            if (soc_property_get(unit, spn_SERDES_LCPLL, 0)) {
                /* Select between external or LCPLL clock */
                SOC_IF_ERROR_RETURN
                    (READ_CMIC_GFPORT_CLOCK_CONFIGr(unit, &rval));
                soc_reg_field_set(unit,
                                  CMIC_GFPORT_CLOCK_CONFIGr,
                                  &rval,
                                  SEL_DOZEN_SERDES_0_REF_CLK_SRCf,
                                  3);
                soc_reg_field_set(unit,
                                  CMIC_GFPORT_CLOCK_CONFIGr,
                                  &rval,
                                  SEL_DOZEN_SERDES_1_REF_CLK_SRCf,
                                  3);
                if (soc_reg_field_valid(unit, CMIC_GFPORT_CLOCK_CONFIGr,
                                        PROG_DOZEN_SERDES_PLLDIV_CTRL_DEFf)) {
                    soc_reg_field_set(unit,
                                      CMIC_GFPORT_CLOCK_CONFIGr,
                                      &rval,
                                      PROG_DOZEN_SERDES_PLLDIV_CTRL_DEFf,
                                      3);                    
                }
                SOC_IF_ERROR_RETURN
                    (WRITE_CMIC_GFPORT_CLOCK_CONFIGr(unit, rval));
            }
        }
#endif /* BCM_RAVEN_SUPPORT */
    }
#endif /* BCM_RAPTOR_SUPPORT || BCM_RAVEN_SUPPORT*/

    /* XMAC init should be moved to mac */
    if (SOC_PBMP_NOT_NULL(PBMP_XE_ALL(unit)) ||
        SOC_PBMP_NOT_NULL(PBMP_HG_ALL(unit))) {
        rval = 0;
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, XPORT_ENf, 1);
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        PBMP_HG_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
        PBMP_XE_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
        }
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
         * causes the outer tag to be removed from packets that don't have
         * a hit in the egress vlan tranlation table. Set to 0 to disable this.
         */
        if (SOC_REG_IS_VALID(unit, EGR_VLAN_CONTROL_1r)) {
            rval = 0;
            PBMP_ALL_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
            }
        }
    }
#endif

    /*
     * ING_CONFIG
     */
    SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, ING_CONFIGr, &rval, L3SRC_HIT_ENABLEf, 1);
    soc_reg_field_set(unit, ING_CONFIGr, &rval, L2DST_HIT_ENABLEf, 1);
    /* Fix the reset value and make it compatible with other devices */
    if (soc_reg_field_valid(unit, ING_CONFIGr, APPLY_EGR_MASK_ON_L2f)) {
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          APPLY_EGR_MASK_ON_L2f, 1);
    }
    if (soc_reg_field_valid(unit, ING_CONFIGr, APPLY_EGR_MASK_ON_L3f)) {
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          APPLY_EGR_MASK_ON_L3f, 1);
    }
    if (soc_feature(unit, soc_feature_remote_learn_trust)) {
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          DO_NOT_LEARN_ENABLEf, 1);
    }
#ifdef BCM_FIREBOLT2_SUPPORT
    /* Turn on ARP MACSA check for VFP */
    if (soc_reg_field_valid(unit, ING_CONFIGr, ARP_VALIDATION_ENf)) {
        soc_reg_field_set(unit, ING_CONFIGr, &rval,
                          ARP_VALIDATION_ENf, 1);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, rval));

    /* Backwards compatible mirroring by default */
    if (soc_feature(unit, soc_feature_xgs1_mirror)) {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                          DRACO_1_5_MIRRORING_MODE_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ING_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ING_CONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, EGR_CONFIGr, &rval, DRACO1_5_MIRRORf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIGr(unit, rval));
    }
    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /*
     * Set up PVID. Done in BCM Port init. Till then PVID = 0!
     */

    if (soc_feature(unit, soc_feature_lpm_tcam)) {
        int rv;
        rv = soc_mem_clear(unit, L3_DEFIPm, COPYNO_ALL, TRUE);
        if (rv < 0) {
            LOG_CLI((BSL_META_U(unit,
                                "soc_mem_clear: LPM flush failed.\n")));
            return(rv);
        }
    }
#ifdef BCM_FIREBOLT2_SUPPORT
    if (SOC_IS_FIREBOLT2(unit)) {
        uint32              e[SOC_MAX_MEM_FIELD_WORDS];
        /*  
         *  Uninitialized L3_DEFIP buffers prep. 
         *  Read entries from both tcam banks to clear possible false parity 
         *  error indication *  DO NOT CHECK FOR READ ERRORS *
         */
        (void)READ_L3_DEFIPm(unit, MEM_BLOCK_ANY,
                             soc_mem_index_min(unit, L3_DEFIPm), e);
        (void)READ_L3_DEFIPm(unit, MEM_BLOCK_ANY,
                             soc_mem_index_max(unit, L3_DEFIPm), e);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    /* Improves the trunk distribution for Higig and unicast trunking for
     * non-power of 2 number of trunk ports.
     */
    if (soc_feature(unit, soc_feature_trunk_group_size)) {
        SOC_IF_ERROR_RETURN(READ_ING_MISC_CONFIGr(unit, &rval));
#if defined(BCM_FIREBOLT2_SUPPORT)
        if(SOC_REG_FIELD_VALID(unit, ING_MISC_CONFIGr,
                               UNIFORM_HG_TRUNK_DIST_ENABLEf)) {
            soc_reg_field_set(unit, ING_MISC_CONFIGr, &rval, 
                              UNIFORM_HG_TRUNK_DIST_ENABLEf, 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        if(SOC_REG_FIELD_VALID(unit, ING_MISC_CONFIGr,
                               UNIFORM_TRUNK_DIST_ENABLEf)) {
            soc_reg_field_set(unit, ING_MISC_CONFIGr, &rval,
                              UNIFORM_TRUNK_DIST_ENABLEf, 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

        SOC_IF_ERROR_RETURN(WRITE_ING_MISC_CONFIGr(unit, rval));
    }
    /*
     * Set reference clock (based on 133MHz core clock)
     * to be 133MHz * (1/27) ~= 4.93MHz
     * so MDC output frequency is 0.5 * 4.93MHz ~= 2.46MHz
     */
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 27);
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUSTr(unit, rval));

    /* Match the Internal MDC freq with above for External MDC */
    if (SOC_REG_IS_VALID(unit, CMIC_RATE_ADJUST_INT_MDIOr)) {
        divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 27);
        dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
        rval = 0;
        soc_reg_field_set
            (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, divisor);
        soc_reg_field_set
            (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));
    }

    if (SOC_REG_IS_VALID(unit, CMIC_RATE_ADJUST_STDMAr)) {
        divisor = soc_property_get(unit, spn_RATE_STDMA_DIVISOR, 6);
        dividend = soc_property_get(unit, spn_RATE_STDMA_DIVIDEND, 1);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                          DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_STDMAr, &rval,
                          DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_STDMAr(unit, rval));
    }

    if (SOC_REG_IS_VALID(unit, CMIC_RATE_ADJUST_I2Cr)) {
        /*
         * Set reference clock (based on 133MHz core clock)
         * to be 133MHz * (1/27) ~= 4.93MHz
         * so I2C output frequency is 4.93MHz ~= 5Mhz.
         */
        divisor = soc_property_get(unit, spn_RATE_I2C_DIVISOR, 27);
        dividend = soc_property_get(unit, spn_RATE_I2C_DIVIDEND, 1);
        rval = 0;
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVISORf, divisor);
        soc_reg_field_set(unit, CMIC_RATE_ADJUST_I2Cr, &rval, DIVIDENDf, dividend);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_RATE_ADJUST_I2Cr(unit, rval));
    }

    rval = 0x01; /* 125KHz I2C sampling rate based on 5Mhz reference clock */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_I2C_STATr(unit, rval));

    /*
     *  H/W linkscan default MDIO address map.
     */
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        SOC_IF_ERROR_RETURN(soc_raptor_hw_linkscan_map_init(unit));
    }
#endif

    /* Remap LED processor scan ports */ 
#if defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        SOC_IF_ERROR_RETURN(READ_CMIC_LED_PORT_ORDER_REMAP_20_24r(unit, &rval));
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_20_24r, &rval, 
                          REMAP_PORT_24f, 0x3);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_20_24r(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval, 
                          REMAP_PORT_25f, 0x1);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval, 
                          REMAP_PORT_26f, 0x2);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval, 
                          REMAP_PORT_27f, 0x4);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval, 
                          REMAP_PORT_28f, 0x5);
        soc_reg_field_set(unit, CMIC_LED_PORT_ORDER_REMAP_25_29r, &rval, 
                          REMAP_PORT_29f, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_25_29r(unit, rval));   
        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_30_34r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_35_39r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_40_44r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_45_49r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LED_PORT_ORDER_REMAP_50_54r(unit, rval));
    }
#endif

#if defined (BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
    }
#endif

    return SOC_E_NONE;
}
#define  FB_NUM_COS  8
int soc_fb_xq_mem(int unit, soc_port_t port, soc_mem_t *xq)
{
    soc_mem_t   xq_mems[] = {   MMU_XQ0m,
                                MMU_XQ1m,
                                MMU_XQ2m,
                                MMU_XQ3m,
                                MMU_XQ4m,
                                MMU_XQ5m,
                                MMU_XQ6m,
                                MMU_XQ7m,
                                MMU_XQ8m,
                                MMU_XQ9m,
                                MMU_XQ10m,
                                MMU_XQ11m,
                                MMU_XQ12m,
                                MMU_XQ13m,
                                MMU_XQ14m,
                                MMU_XQ15m,
                                MMU_XQ16m,
                                MMU_XQ17m,
                                MMU_XQ18m,
                                MMU_XQ19m,
                                MMU_XQ20m,
                                MMU_XQ21m,
                                MMU_XQ22m,
                                MMU_XQ23m,
                                MMU_XQ24m,
                                MMU_XQ25m,
                                MMU_XQ26m,
                                MMU_XQ27m,
#if defined(BCM_RAPTOR_SUPPORT)
                                MMU_XQ28m,
                                MMU_XQ29m,
                                MMU_XQ30m,
                                MMU_XQ31m,
#if defined(BCM_RAPTOR1_SUPPORT)
                                MMU_XQ32m,
                                MMU_XQ33m,
                                MMU_XQ34m,
                                MMU_XQ35m,
                                MMU_XQ36m,
                                MMU_XQ37m,
                                MMU_XQ38m,
                                MMU_XQ39m,
                                MMU_XQ40m,
                                MMU_XQ41m,
                                MMU_XQ42m,
                                MMU_XQ43m,
                                MMU_XQ44m,
                                MMU_XQ45m,
                                MMU_XQ46m,
                                MMU_XQ47m,
                                MMU_XQ48m,
                                MMU_XQ49m,
                                MMU_XQ50m,
                                MMU_XQ51m,
                                MMU_XQ52m,
                                MMU_XQ53m
#endif /* BCM_RAPTOR1_SUPPORT */
#else
                                MMU_XQ28m
#endif /* BCM_RAPTOR_SUPPORT */
                            };
    int max_port;
    max_port = sizeof(xq_mems) / sizeof(xq_mems[0]);
    if (port >= max_port) {
        return(SOC_E_PORT);
    }
    *xq = xq_mems[port];
    return(SOC_E_NONE);
}

int
soc_firebolt_internal_mmu_init(int unit)
{
    int         xq_per_port, cos, limit;
    soc_port_t  port;
    uint32      val, oval, cfap_max_idx;
    int         wgt[FB_NUM_COS], tot_wgt = 0;
    int         age[FB_NUM_COS], max_age, min_age;
    int         age_enable, disabled_age;
    int         inactive_cosqs = 0;
    int         skid_mark;
    soc_mem_t   xq_mem;

    cfap_max_idx = soc_mem_index_max(unit, MMU_CFAPm); 
    SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &val));
    oval = val;
    soc_reg_field_set(unit, CFAPCONFIGr, &val, CFAPPOOLSIZEf, cfap_max_idx);
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, val));
    }

    SOC_IF_ERROR_RETURN(READ_CFAPFULLTHRESHOLDr(unit, &val));
    oval = val;
    /* coverity [dead_error_line]      */
    /* coverity[dead_error_condition : FALSE] */
    if (cfap_max_idx == 0x17ff) {
        soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                          CFAPFULLRESETPOINTf, 0x1700);
        soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &val,
                          CFAPFULLSETPOINTf, 0x1770);
    }
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, val));
    }

    /* 
     * Spread the XQs across all COSQs as dictated by the weight
     * properties. This will allow use of all COSQs. However, if 
     * some COSQs are later disabled, the XQs allocated here to 
     * those disabled COSQs will be unavailable for use.
     */
    for (cos = 0; cos < NUM_COS(unit); cos++) {
        wgt[cos] =
            soc_property_suffix_num_get(unit, cos, spn_MMU_XQ_WEIGHT,
                                        "cos",  1);
        tot_wgt += wgt[cos];
        if (!wgt[cos]) {
            inactive_cosqs++;
        }
    }

    /* Every inactive COSQ consumes skid mark + 4 XQs; remove them from pool */
    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &val));
    skid_mark = soc_reg_field_get(unit, MISCCONFIGr, val, SKIDMARKERf) + 4;

    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_fb_xq_mem(unit, port, &xq_mem));
        xq_per_port = soc_mem_index_count(unit, xq_mem) -
                      (skid_mark * inactive_cosqs);
        for (cos = 0; cos < NUM_COS(unit); cos++) {
            /* 
             * The HOLCOSPKTSETLIMITr register controls BOTH the XQ 
             * size per cosq AND the HOL set limit for that cosq.
             */
            SOC_IF_ERROR_RETURN
                (READ_HOLCOSPKTSETLIMITr(unit, port, cos, &val));
            if (!wgt[cos]) {
                limit = skid_mark; /* Minimum value for this register */
            } else if (wgt[cos] == tot_wgt) {
              limit = xq_per_port; /* All available XQs */ 
            } else {
                limit = (xq_per_port * wgt[cos] / tot_wgt);
            }
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &val,
                              PKTSETLIMITf, 
                              limit);
            SOC_IF_ERROR_RETURN
                (WRITE_HOLCOSPKTSETLIMITr(unit, port, cos, val));

        }
    }

    /* 
     * Configure per-XQ packet aging for the various COSQs. 
     *
     * The shortest age allowed by H/W is 250 microseconds.
     * The longest age allowed is 7.162 seconds (7162 msec).
     * The maximum ratio between the longest age and the shortest 
     * (nonzero) age is 7:2.
     */
    age_enable = disabled_age = max_age = 0;
    min_age = 7162;
    for (cos = 0; cos < NUM_COS(unit); cos++) {
        if ((age[cos] = 
             soc_property_suffix_num_get(unit, cos, spn_MMU_XQ_AGING,
                                         "cos",  0)) > 0) {
            age_enable = 1;
            if (age[cos] > 7162) {
                age[cos] = 7162;
            }
            if (age[cos] < min_age) {
                min_age = age[cos];
            }
        } else {
            disabled_age = 1;
            age[cos] = 0;
        }
        if (age[cos] > max_age) {
            max_age = age[cos];
        }
    }
    if (!age_enable) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0)); /* Aesthetic */
    } else {
        uint32 regval = 0;
        uint32 timerval;

        /* Enforce the 7:2 ratio between min and max values */
        if ((((max_age * 2)+6) / 7) > min_age) {
            /* Keep requested max age; make min_age comply */ 
            min_age = ((max_age * 2) + 6) / 7; 
        }

        /* 
         * Give up granularity for range, if we need to
         * "disable" (max out) aging for any COSQ(s).
         */
        if (disabled_age) {
            /* Max range */
            max_age = min_age * 7 / 2;
        } 

        /* 
         * Compute shortest duration of one PKTAGINGTIMERr cycle. 
         * This duration is 1/7th of the longest packet age. 
         * This duration is in units of 125 usec (msec * 8).
         */

        timerval = ((8 * max_age) + 6) / 7; 
        if (timerval <= 0) {
            return SOC_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, timerval));

        for (cos = 0; cos < NUM_COS(unit); cos++) {
            if (!age[cos]) {
                /* 
                 * Requested to be disabled, but cannot disable individual
                 * COSQs once packet aging is enabled. Therefore, mark
                 * this COSQ's aging duration as maxed out.
                 */
                age[cos] = -1;
            } else if (age[cos] < min_age) {
                age[cos] = min_age;
            }

            /* Normalize each "age" into # of PKTAGINGTIMERr cycles. */
        if (age[cos] > 0) {
            age[cos] = ((8 * age[cos]) + timerval - 1) / timerval;
        }
        else {
            age[cos] = 7;
        }
            /* Format each "age" for its appropriate field */
            regval |= ((7 - age[cos]) << (cos * 3));
        }
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, regval));
    }

    /*
     * Port enable
     */
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                      SOC_PBMP_WORD_GET(PBMP_ALL(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));
#if defined(BCM_RAPTOR1_SUPPORT)
    if (soc_feature(unit, soc_feature_register_hi)) {
        val = 0;
        soc_reg_field_set(unit, MMUPORTENABLE_HIr, &val, MMUPORTENABLEf,
                          SOC_PBMP_WORD_GET(PBMP_ALL(unit), 1));
        SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLE_HIr(unit, val));
    }
#endif /* BCM_RAPTOR_SUPPORT */

    return SOC_E_NONE;
}

int
soc_firebolt_mmu_init(int unit)
{
    int rv = SOC_E_NONE;

    /* Don't run mmu setting for Hawkeye */
    if(!SOC_IS_HAWKEYE(unit)) {
        rv = soc_firebolt_internal_mmu_init(unit);

#ifdef BCM_RAVEN_SUPPORT
        if (soc_feature(unit, soc_feature_post)) {
            sal_usecs_t stime = sal_time_usecs();
            if ((rv = soc_post_init(unit)) < 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_post_init failed on unit %d\n"), unit));
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "soc_post_init: took %d usec\n"),
                                    SAL_USECS_SUB(sal_time_usecs(), stime)));
        }
#endif
    }
    return (rv);
}

int
soc_firebolt_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_firebolt_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_firebolt_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

/*
 * _soc_fb_mmu_parity_error
 *      FB MMU parity error decoder
 */
int _soc_fb_mmu_parity_error(int unit)
{
    uint32              istat, val;
    soc_control_t       *soc = SOC_CONTROL(unit);

    SOC_IF_ERROR_RETURN(READ_MEMFAILINTSTATUSr(unit, &istat));
    if (istat != 0) {
        soc->stat.intr_mmu++;
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_fb_mmu_parity_error:unit = %d,"
                          "INTSTATUS  = 0x%08x Fail Count = %d\n"),
               unit,
               istat,
               soc_reg_field_get(unit, MEMFAILINTSTATUSr,
               istat, MEMFAILINTCOUNTf)));


    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, CFAPFAILERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CFAPFAILERROR\n"), unit));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr,
                          istat, CFAPPARITYERRORf)) {
        soc->stat.err_cfap++;
        SOC_IF_ERROR_RETURN(READ_CFAPPARITYERRORPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CFAPPARITYERRORPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, SOFTRESETERRORf)) {
        soc->stat.err_sr++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, MMU Soft Reset Error\n"), unit));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, CELLNOTIPERRORf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, MMU Cell not in progress\n"), unit));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, CELLCRCERRORf)) {
        soc->stat.err_cellcrc++;
        SOC_IF_ERROR_RETURN(READ_CBPCELLCRCERRPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CBPCELLCRCERRPTR 0x%08x\n"), unit, val));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr,
                          istat, CBPPKTHDRPARITYERRORf)) {
        soc->stat.err_cbphp++;
        SOC_IF_ERROR_RETURN(READ_CBPPKTHDRPARITYERRPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CBPPKTHDRPARITYERRPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr,
                          istat, CBPCELLHDRPARITYERRORf)) {
        soc->stat.err_cpbcrc++;
        SOC_IF_ERROR_RETURN(READ_CBPCELLHDRPARITYERRPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CBPCELLHDRPARITYERRPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, XQPARITYERRORf)) {
        soc->stat.err_mp++;
        SOC_IF_ERROR_RETURN(READ_XQPARITYERRORPBMr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, XQPARITYERRORPBM 0x%08x\n"),
                              unit, val));
    }

    if (soc_reg_field_get(unit, MEMFAILINTSTATUSr, istat, CCPPARITYERRORf)) {
        soc->stat.err_cpcrc++;
        SOC_IF_ERROR_RETURN(READ_CCPPARITYERRORPTRr(unit, &val));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit = %d, CCPPARITYERRORPTR 0x%08x\n"),
                              unit, val));
    }

    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        if (soc_reg_field_get(unit, MEMFAILINTSTATUSr,
                              istat, IPMCREPOVERLIMITERRORf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit=%d, IPMC Over Pre-set Replication Limit\n"),
                       unit));
        }
    }

    SOC_IF_ERROR_RETURN(WRITE_MEMFAILINTSTATUSr(unit, 0));

    return SOC_E_NONE;
}

/*
 * _soc_hx_ipipe_parity_error
 *      HX/FX L2/L3 parity error decoder
 */
int _soc_hx_ipipe_parity_error(int unit)
{
    uint32              istat = 0;
    uint32              val = 0;
    _soc_ser_correct_info_t spci;

    if (soc_feature(unit, soc_feature_l2x_parity)) {
        SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     bucket, entry_bmap, index, b_index;
            bucket = soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                                       istat, BUCKET_IDXf),
            entry_bmap = soc_reg_field_get(unit, L2_ENTRY_PARITY_STATUSr,
                                           istat, ENTRY_BMf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "_soc_hx_ipipe_parity_error:unit = %d,"
                                  "L2_ENTRY_PARITY_STATUS  = 0x%08x, "
                                  "Bucket = %d Entry Bitmap 0x%02x\n"),
                       unit,
                       istat,
                       bucket,
                       entry_bmap));

            if (SOC_IS_FIREBOLT2(unit)) {
                for (b_index = 0; ((entry_bmap & 1) == 0) && (entry_bmap != 0); entry_bmap >>= 1, b_index++) { }
                index = bucket*8 + b_index;

                memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = L2_ENTRY_ONLYm;
                spci.blk_type = SOC_BLK_IPIPE;
                spci.index = index;
                (void)soc_ser_correction(unit, &spci);
            }

            /* Clear all parity errors */ 
            SOC_IF_ERROR_RETURN(READ_L2_ENTRY_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L2_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L2_ENTRY_PARITY_CONTROLr(unit, val));
        }
    }

    if (soc_feature(unit, soc_feature_l3x_parity)) {
        SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     bucket, entry_bmap, index, b_index;

            bucket = soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                                       istat, BUCKET_IDXf),
            entry_bmap = soc_reg_field_get(unit, L3_ENTRY_PARITY_STATUSr,
                                           istat, ENTRY_BMf);
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "_soc_hx_ipipe_parity_error:unit = %d,"
                                  "L3_ENTRY_PARITY_STATUS  = 0x%08x, "
                                  "Bucket = %d Entry Bitmap 0x%02x\n"),
                       unit,
                       istat,
                       bucket,
                       entry_bmap));

            if (SOC_IS_FIREBOLT2(unit)) {
                for (b_index = 0; ((entry_bmap & 1) == 0) && (entry_bmap != 0) ; entry_bmap >>= 1, b_index++) { }
                index = bucket*8 + b_index;

                memset(&spci, 0, sizeof(_soc_ser_correct_info_t));

                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                spci.reg = INVALIDr;
                spci.mem = L3_ENTRY_ONLYm;
                spci.blk_type = SOC_BLK_IPIPE;
                spci.index = index;
                (void)soc_ser_correction(unit, &spci);
            }

            SOC_IF_ERROR_RETURN(READ_L3_ENTRY_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L3_ENTRY_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L3_ENTRY_PARITY_CONTROLr(unit, val));
        }
    }

    if (soc_feature(unit, soc_feature_l3defip_parity)) {
        SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_STATUSr(unit, &istat));
        if (soc_reg_field_get(unit, L3_DEFIP_PARITY_STATUSr,
                              istat, PARITY_ERRf)) {
            uint32     mem_idx;

            mem_idx = soc_reg_field_get(unit, L3_DEFIP_PARITY_STATUSr,
                                        istat, MEMORY_IDXf);
            if (SOC_IS_FIREBOLT2(unit)) {
                uint32 mem_idx_min, mem_idx_max, index;
                if (mem_idx > (soc_mem_index_max(unit, L3_DEFIPm) / 2)) {
                    /* HW idiosyncrasy for second URPF bank */
                    mem_idx -= 1024;
                }
                
                mem_idx_min = mem_idx & ~0x3;
                mem_idx_max = mem_idx_min + 3;
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "_soc_fb_ipipe_parity_error:unit = %d,"
                                      "L3_DEFIP_PARITY_STATUS  = 0x%08x, "
                                      "Index range = %d - %d"),
                           unit, istat, mem_idx_min, mem_idx_max));

                for (index = mem_idx_min; index <= mem_idx_max; index++) {
                    memset(&spci, 0, sizeof(_soc_ser_correct_info_t));
                    spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = L3_DEFIP_DATA_ONLYm;
                    spci.blk_type = SOC_BLK_IPIPE;
                    spci.index = index;
                    (void)soc_ser_correction(unit, &spci);
                }
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "_soc_fb_ipipe_parity_error:unit = %d,"
                                      "L3_DEFIP_PARITY_STATUS  = 0x%08x, Index = %d "),
                           unit,
                           istat,
                           mem_idx));
            }
            SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PARITY_CONTROLr(unit, &val));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, val));
            soc_reg_field_set(unit, L3_DEFIP_PARITY_CONTROLr, &val,
                              PARITY_IRQ_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PARITY_CONTROLr(unit, val));
        }
    }
#ifdef BCM_RAVEN_SUPPORT
    if (soc_feature(unit, soc_feature_ip_ep_mem_parity)) {
        int i;
        uint32 err_addr, mem_idx, slice_idx;
        soc_reg_t ctrl_reg[10] = {DSCP_TABLE_PARITY_CONTROLr, EGR_L3_INTF_PARITY_CONTROLr,
                             EGR_MASK_PARITY_CONTROLr, EGR_NEXT_HOP_PARITY_CONTROLr,
                             EGR_VLAN_TABLE_PARITY_CONTROLr, FP_POLICY_PARITY_CONTROLr,
                             ING_L3_NEXT_HOP_PARITY_CONTROLr, L2MC_PARITY_CONTROLr,
                             L3_IPMC_PARITY_CONTROLr, VLAN_PARITY_CONTROLr};
        soc_reg_t stat_reg[10] = {DSCP_TABLE_PARITY_STATUSr, EGR_L3_INTF_PARITY_STATUSr,
                             EGR_MASK_PARITY_STATUSr, EGR_NEXT_HOP_PARITY_STATUSr,
                             EGR_VLAN_TABLE_PARITY_STATUSr, FP_POLICY_PARITY_STATUSr,
                             ING_L3_NEXT_HOP_PARITY_STATUSr, L2MC_PARITY_STATUSr,
                             L3_IPMC_PARITY_STATUSr, VLAN_PARITY_STATUSr};

        for (i = 0; i < 10 ; i++) {
            /* Skip non-existed registers for HAWKEYE */ 
            if (!SOC_REG_IS_VALID(unit, ctrl_reg[i])) {
                continue;
            }
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, stat_reg[i], REG_PORT_ANY, 0, 
                                              &istat));
            if (soc_reg_field_get(unit, stat_reg[i], istat, PARITY_ERRf)) {
                switch (stat_reg[i]) {
                case DSCP_TABLE_PARITY_STATUSr: 
                case EGR_MASK_PARITY_STATUSr:
                case ING_L3_NEXT_HOP_PARITY_STATUSr:
                case L2MC_PARITY_STATUSr:
                case VLAN_PARITY_STATUSr:
                    err_addr = soc_reg_field_get(unit, stat_reg[i], istat, ERR_ADDRf);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "_soc_fb_ipipe_parity_error:unit = %d,"
                                          "PARITY_STATUS register (%x) = 0x%08x, Index = %d "),
                               unit,
                               stat_reg[i],
                               istat,
                               err_addr));
                    break;
                case EGR_L3_INTF_PARITY_STATUSr:
                case EGR_NEXT_HOP_PARITY_STATUSr:
                case EGR_VLAN_TABLE_PARITY_STATUSr:
                case L3_IPMC_PARITY_STATUSr:
                    mem_idx = soc_reg_field_get(unit, stat_reg[i], istat, MEMORY_IDXf);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "_soc_fb_ipipe_parity_error:unit = %d,"
                                          "PARITY_STATUS register (%x) = 0x%08x, Index = %d "),
                               unit,
                               stat_reg[i],
                               istat,
                               mem_idx));
                    break;
                case FP_POLICY_PARITY_STATUSr:
                    slice_idx = soc_reg_field_get(unit, stat_reg[i], istat, SLICE_IDXf);
                    mem_idx = soc_reg_field_get(unit, stat_reg[i], istat, MEMORY_IDXf);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "_soc_fb_ipipe_parity_error:unit = %d,"
                                          "PARITY_STATUS register (%x) = 0x%08x, Slice = %d,"
                                          "Index = %d "),
                               unit,
                               stat_reg[i],
                               istat,
                               slice_idx,
                               mem_idx));
                    break;
                }
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ctrl_reg[i], REG_PORT_ANY, 
                                                  0, &val));
                soc_reg_field_set(unit, ctrl_reg[i], &val, PARITY_IRQ_ENf, 0);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ctrl_reg[i], REG_PORT_ANY, 
                                                  0, val));
                if (ctrl_reg[i] == FP_POLICY_PARITY_CONTROLr) {
                    soc_reg_field_set(unit, ctrl_reg[i], &val, PARITY_IRQ_ENf, 0xffff);
                } else {
                    soc_reg_field_set(unit, ctrl_reg[i], &val, PARITY_IRQ_ENf, 1);
                }
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ctrl_reg[i], REG_PORT_ANY, 
                                                  0, val));
            }
        }
    }
#endif /* BCM_RAVEN_SUPPORT */
    return SOC_E_NONE;
}

void soc_fb_mmu_parity_error(void *unit, void *d1, void *d2,
                             void *d3, void *d4 )
{
    int u = PTR_TO_INT(unit);

    COMPILER_REFERENCE(d1);
    COMPILER_REFERENCE(d2);
    COMPILER_REFERENCE(d3);
    COMPILER_REFERENCE(d4);
    _soc_hx_ipipe_parity_error(u);
    _soc_fb_mmu_parity_error(u);
    soc_intr_enable(u, IRQ_MEM_FAIL);
}

void
soc_helix_mem_config(int unit)
{
    soc_persist_t       *sop;
    sop = SOC_PERSIST(unit);

/*
    sop->memState[L2Xm].index_max = 8191;
    sop->memState[L2_ENTRY_ONLYm].index_max = 8191;
    sop->memState[L2_HITDA_ONLYm].index_max = 1023;
    sop->memState[L2_HITSA_ONLYm].index_max = 1023;

    sop->memState[L3_ENTRY_ONLYm].index_max = 2047;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = 2047;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = 2047;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = 1023;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = 511;
    sop->memState[L3_ENTRY_VALID_ONLYm].index_max = 255;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = 255;
*/

    sop->memState[L3_DEFIPm].index_max = 255;
    sop->memState[L3_DEFIP_ONLYm].index_max = 255;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 255;

    sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 255;

    sop->memState[FP_TCAMm].index_max = 1023;
    sop->memState[FP_TCAM_PLUS_POLICYm].index_max = 1023;
    sop->memState[FP_POLICY_TABLEm].index_max = 1023;
    sop->memState[FP_METER_TABLEm].index_max = 1023;
    sop->memState[FP_COUNTER_TABLEm].index_max = 1023;

    sop->memState[L3_ECMPm].index_max = 127;
    sop->memState[ING_L3_NEXT_HOPm].index_max = 2047;
    sop->memState[EGR_L3_NEXT_HOPm].index_max = 2047;

    sop->memState[MMU_XQ0m].index_max = 0x2ff;
    sop->memState[MMU_XQ1m].index_max = 0x2ff;
    sop->memState[MMU_XQ2m].index_max = 0x2ff;
    sop->memState[MMU_XQ3m].index_max = 0x2ff;
    sop->memState[MMU_XQ4m].index_max = 0x2ff;
    sop->memState[MMU_XQ5m].index_max = 0x2ff;
    sop->memState[MMU_XQ6m].index_max = 0x2ff;
    sop->memState[MMU_XQ7m].index_max = 0x2ff;
    sop->memState[MMU_XQ8m].index_max = 0x2ff;
    sop->memState[MMU_XQ9m].index_max = 0x2ff;
    sop->memState[MMU_XQ10m].index_max = 0x2ff;
    sop->memState[MMU_XQ11m].index_max = 0x2ff;
    sop->memState[MMU_XQ12m].index_max = 0x2ff;
    sop->memState[MMU_XQ13m].index_max = 0x2ff;
    sop->memState[MMU_XQ14m].index_max = 0x2ff;
    sop->memState[MMU_XQ15m].index_max = 0x2ff;
    sop->memState[MMU_XQ16m].index_max = 0x2ff;
    sop->memState[MMU_XQ17m].index_max = 0x2ff;
    sop->memState[MMU_XQ18m].index_max = 0x2ff;
    sop->memState[MMU_XQ19m].index_max = 0x2ff;
    sop->memState[MMU_XQ20m].index_max = 0x2ff;
    sop->memState[MMU_XQ21m].index_max = 0x2ff;
    sop->memState[MMU_XQ22m].index_max = 0x2ff;
    sop->memState[MMU_XQ23m].index_max = 0x2ff;

    sop->memState[MMU_XQ24m].index_max = 0x3ff;
    sop->memState[MMU_XQ25m].index_max = 0x3ff;
    sop->memState[MMU_XQ26m].index_max = 0x3ff;
    sop->memState[MMU_XQ27m].index_max = 0x3ff;

    sop->memState[MMU_XQ28m].index_max = 0x2ff;

    sop->memState[MMU_AGING_CTRm].index_max = 0x2ff;
    sop->memState[MMU_AGING_EXPm].index_max = 0x2ff;

    sop->memState[MMU_CBPCELLHEADERm].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA0m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA1m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA2m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA3m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA4m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA5m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA6m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA7m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA8m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA9m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA10m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA11m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA12m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA13m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA14m].index_max = 0x17ff; 
    sop->memState[MMU_CBPDATA15m].index_max = 0x17ff; 
    sop->memState[MMU_CBPPKTHEADER0m].index_max = 0x17ff; 
    sop->memState[MMU_CBPPKTHEADER1m].index_max = 0x17ff; 
    sop->memState[MMU_CCPm].index_max = 0x17ff; 
    sop->memState[MMU_CFAPm].index_max = 0x17ff; 
}

void
soc_bcm53300_mem_config(int unit)
{
    soc_persist_t       *sop;
    sop = SOC_PERSIST(unit);

    sop->memState[MMU_CBPCELLHEADERm].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA0m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA1m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA2m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA3m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA4m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA5m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA6m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA7m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA8m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA9m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA10m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA11m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA12m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA13m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA14m].index_max = 0xfff; 
    sop->memState[MMU_CBPDATA15m].index_max = 0xfff; 
    sop->memState[MMU_CBPPKTHEADER0m].index_max = 0xfff; 
    sop->memState[MMU_CBPPKTHEADER1m].index_max = 0xfff; 
    sop->memState[MMU_CCPm].index_max = 0xfff; 
    sop->memState[MMU_CFAPm].index_max = 0xfff; 

    sop->memState[EGR_VLAN_STGm].index_max = 31; 
    sop->memState[STG_TABm].index_max = 31; 
    sop->memState[L2MCm].index_max = 255; 

    sop->memState[TRUNK_BITMAPm].index_max = 15; 
    sop->memState[TRUNK_EGR_MASKm].index_max = 15; 
    sop->memState[TRUNK_GROUPm].index_max = 15; 

    sop->memState[FP_TCAMm].index_max = 511;
    sop->memState[FP_TCAM_PLUS_POLICYm].index_max = 511;
    sop->memState[FP_POLICY_TABLEm].index_max = 511;
    sop->memState[FP_METER_TABLEm].index_max = 511;
    sop->memState[FP_COUNTER_TABLEm].index_max = 511;
    /* sop->memState[FP_RANGE_CHECKm].index_max = 1;*/

    sop->memState[L3_DEFIPm].index_max = -1;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_ONLYm].index_max = -1;
    sop->memState[L3_ECMPm].index_max = -1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
    sop->memState[L3_IPMCm].index_max = -1;
    sop->memState[L3_TUNNELm].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL0m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL1m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL2m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL3m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL4m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL5m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL6m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL7m].index_max = -1;
    sop->memState[MMU_IPMC_VLAN_TBLm].index_max = -1;
}

#ifdef BCM_FIREBOLT2_SUPPORT
void
soc_firebolt2_mem_config(int unit)
{
    soc_persist_t       *sop;
    uint32              entry_mask, entry_request;
#ifndef EXCLUDE_BCM56324
    uint16              dev_id;
    uint8               rev_id;
#endif /* EXCLUDE_BCM56324 */

    sop = SOC_PERSIST(unit);

#ifndef EXCLUDE_BCM56324
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56322_DEVICE_ID) || (dev_id == BCM56324_DEVICE_ID)) {
        sop->memState[L2Xm].index_max = 0x3fff;
        sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = 0xfff;

        sop->memState[L3_DEFIPm].index_max = 0x7ff;
        sop->memState[L3_DEFIP_ONLYm].index_max = 0x7ff;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 0x7ff;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 0x7ff;
        sop->memState[FP_TCAMm].index_max = 0x7ff;
        sop->memState[FP_POLICY_TABLEm].index_max = 0x7ff;
        sop->memState[FP_METER_TABLEm].index_max = 0x7ff;
        sop->memState[FP_COUNTER_TABLEm].index_max = 0x7ff;
    }
#endif /* EXCLUDE_BCM56324 */

    /* L2 tables */
    entry_mask = sop->memState[L2Xm].index_max;
    
    entry_request = soc_property_get(unit, spn_L2_TABLE_SIZE, entry_mask);

    if (entry_request < entry_mask) {
        /* Round up to next power of 2 */
        for (entry_mask >>= 1; entry_mask != 0; entry_mask >>= 1) {
            if (entry_request & ~entry_mask) {
                entry_mask = (entry_mask << 1) | 0x1;
                break;
            }
        }
    }

    sop->memState[L2Xm].index_max = entry_mask;
    sop->memState[L2_ENTRY_ONLYm].index_max = entry_mask;
    sop->memState[L2_HITDA_ONLYm].index_max = entry_mask >> 3;
    sop->memState[L2_HITSA_ONLYm].index_max = entry_mask >> 3;

    /* L3 tables */
    entry_mask = sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max;
    
    entry_request = soc_property_get(unit, spn_L3_TABLE_SIZE, entry_mask);

    if (entry_request < entry_mask) {
        /* Round up to next power of 2 */
        for (entry_mask >>= 1; entry_mask != 0; entry_mask >>= 1) {
            if (entry_request & ~entry_mask) {
                entry_mask = (entry_mask << 1) | 0x1;
                break;
            }
        }
    }

    sop->memState[L3_ENTRY_ONLYm].index_max = entry_mask;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = entry_mask;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = entry_mask;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = entry_mask >> 1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = entry_mask >> 2;
    sop->memState[L3_ENTRY_VALID_ONLYm].index_max = entry_mask >> 3;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = entry_mask >> 3;

    /* STG tables */
    entry_mask = sop->memState[STG_TABm].index_max;
    
    entry_request = soc_property_get(unit, spn_STG_TABLE_SIZE, entry_mask);

    if (entry_request < entry_mask) {
        /* Round up to next power of 2 */
        for (entry_mask >>= 1; entry_mask != 0; entry_mask >>= 1) {
            if (entry_request & ~entry_mask) {
                entry_mask = (entry_mask << 1) | 0x1;
                break;
            }
        }
    }

    sop->memState[STG_TABm].index_max = entry_mask;
    sop->memState[EGR_VLAN_STGm].index_max = entry_mask;
}
#endif /* BCM_FIREBOLT2_SUPPORT */

#ifdef BCM_RAPTOR_SUPPORT
void
soc_bcm56225_mem_config(int unit)
{
    soc_persist_t       *sop;
    sop = SOC_PERSIST(unit);

    sop->memState[L3_DEFIPm].index_max = -1;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_ONLYm].index_max = -1;
    sop->memState[L3_ECMPm].index_max = -1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
    sop->memState[L3_IPMCm].index_max = -1;
    sop->memState[L3_TUNNELm].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL0m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL1m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL2m].index_max = -1;
    sop->memState[MMU_IPMC_VLAN_TBLm].index_max = -1;
}

void
soc_bcm53724_mem_config(int unit)
{
    soc_persist_t       *sop;
    sop = SOC_PERSIST(unit);

    sop->memState[FP_TCAMm].index_max = 511;
    sop->memState[FP_POLICY_TABLEm].index_max = 511;
    sop->memState[FP_METER_TABLEm].index_max = 511;
    sop->memState[FP_COUNTER_TABLEm].index_max = 511;

    sop->memState[L3_DEFIPm].index_max = -1;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
    sop->memState[L3_DEFIP_ONLYm].index_max = -1;
    sop->memState[L3_ECMPm].index_max = -1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = -1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = -1;
    sop->memState[L3_ENTRY_ONLYm].index_max = -1;
    sop->memState[L3_ENTRY_VALID_ONLYm].index_max = -1;
    sop->memState[L3_IPMCm].index_max = -1;
    sop->memState[L3_TUNNELm].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL0m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL1m].index_max = -1;
    sop->memState[MMU_IPMC_GROUP_TBL2m].index_max = -1;
    sop->memState[MMU_IPMC_VLAN_TBLm].index_max = -1;

    sop->memState[L2Xm].index_max = 8191;
    sop->memState[L2_ENTRY_ONLYm].index_max = 8191;
    sop->memState[L2_HITDA_ONLYm].index_max = 1023;
    sop->memState[L2_HITSA_ONLYm].index_max = 1023;
}
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
static int soc_tr_modport_map[9] = {2,14,26,27,28,29,30,31,0};
#endif

#if defined(BCM_ENDURO_SUPPORT)
static int soc_en_modport_map[5] = {26,27,28,29,0};
#endif

#if defined(BCM_RAVEN_SUPPORT)
static int soc_rv_modport_map[5] = {1, 2, 4, 5, 0};
#endif

/*
 * Function:
 *  soc_xgs3_port_to_higig_bitmap
 * Purpose:
 *  Converts Stacking port into HG port bitmap
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - port 
 *  hg_pbm - HG port bitmap value
 * Returns:
 *  SOC_E_XXX
 * Note:
 *  Raptor's port 0 is a CPU port, so -1 should apply in Raptor's 
 *  calculation of HG bitmap from port.
 */
int 
soc_xgs3_port_to_higig_bitmap(int unit, soc_port_t port, uint32 *hg_pbm)
{
    soc_pbmp_t hg;
    SOC_PBMP_CLEAR(hg);
    if (!IS_ST_PORT(unit, port)) {
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
      if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
          if (!IS_CPU_PORT(unit, port)) {
              return SOC_E_PORT;
          }
      } else
#endif  /* BCM_BRADLEY_SUPPORT */
        {
            return SOC_E_PORT;
        }
    }

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit)) {
        if (SOC_IS_SC_CQ(unit) && port > 24) {
            return SOC_E_PORT;
        }
        *hg_pbm = (1 << port);
    } else 
#endif /* BCM_BRADLEY_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        *hg_pbm = (1 << port);
        return SOC_E_NONE;
    } else 
#endif /* BCM_BRADLEY_SUPPORT */


#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        int i;
#if defined(BCM_ENDURO_SUPPORT)
        if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) || 
            SOC_IS_HURRICANE2(unit)) {
            for (i = 0; i < 5; i++) {
                if (soc_en_modport_map[i] == port) {
                    *hg_pbm = 1 << i;
                    return SOC_E_NONE;
                }
            }
        } else 
#endif
        {
            for (i = 0; i < 9; i++) {
                if (soc_tr_modport_map[i] == port) {
                    *hg_pbm = 1 << i;
                    return SOC_E_NONE;
                }
            }
        }
        return SOC_E_PORT;
    } else 
#endif  /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit)) {
        int i;
        for (i = 0; i < 5; i++) {
            if (soc_rv_modport_map[i] == port) {
                *hg_pbm = 1 << i;
                return SOC_E_NONE;
            }
        }
        return SOC_E_PORT;
    } else 
#endif  /* BCM_RAVEN_SUPPORT */
    if (SOC_IS_FBX(unit)) {
        int hg_port;
        /* Other firebolt family devices */
        hg_port = port - SOC_HG_OFFSET(unit);
#ifdef BCM_RAPTOR1_SUPPORT
        if (SOC_IS_RAPTOR(unit)) {
            hg_port -= 1;  /* On Raptor port 0 is CPU port */
        }
#endif
        SOC_PBMP_PORT_SET(hg, hg_port);
        *hg_pbm = SOC_PBMP_WORD_GET(hg, 0);
    } else {
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_xgs3_higig_bitmap_to_port_all
 * Purpose:
 *  Converts HG port bitmap into Stacking port value
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  hg_pbm - HG port bitmap value
 *  port_max - size of input array
 *  port_array - port array
 *  port_count - size of returned array;
 * Returns:
 *  SOC_E_XXX
 * Note:
 *  Raptor's port 0 is a CPU port, so +1 should apply in Raptor's 
 *  calculation of stacking port from bitmap
 */
int
soc_xgs3_higig_bitmap_to_port_all(int unit, uint32 hg_pbm,
                                  int port_max, soc_port_t *port_array,
                                  int *port_count)
{
    soc_pbmp_t hg;

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_HBX(unit)){
        int p;
        int count = 0;
        /* Return all ports found in port map */
        for (p = 0; hg_pbm != 0; p++, hg_pbm >>= 1) {
            if (hg_pbm & 1) {
                if (port_max != 0) {
                    if (count < port_max) {
                        port_array[count] = p;
                    } else {
                        break;
                    }
                }
                count++;
            }
        }
        if (port_count) {
            *port_count = count;
        }
        return (count > 0) ? SOC_E_NONE : SOC_E_NOT_FOUND;
    } else
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        int p;
        int count = 0;
        /* Return all ports found in port map */
#if defined(BCM_ENDURO_SUPPORT) 
        if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
            SOC_IS_HURRICANE2(unit)) {
            for (p = 0; hg_pbm != 0 && p < 5; p++) {
                if (hg_pbm & (1 << p)) {
                    if (port_max != 0) {
                        if (count < port_max) {
                            port_array[count] = soc_en_modport_map[p];
                        } else {
                            break;
                        }
                    }
                    count++;
                }
            }
        } else 
#endif /* BCM_ENDURO_SUPPORT */
        {
            for (p = 0; hg_pbm != 0 && p < 9; p++) {
                if (hg_pbm & (1 << p)) {
                    if (port_max != 0) {
                        if (count < port_max) {
                            port_array[count] = soc_tr_modport_map[p];
                        } else {
                            break;
                        }
                    }
                    count++;
                }
            }
        }
        if (port_count) {
            *port_count = count;
        }
        return (count > 0) ? SOC_E_NONE : SOC_E_NOT_FOUND;
    } else
#endif
#if defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit)) {
        int p;
        int count = 0;
        /* Return all ports found in port map */
        for (p = 0; hg_pbm != 0; p++) {
            if (hg_pbm & (1 << p)) {
                if (port_max != 0) {
                    if (count < port_max) {
                        port_array[count] = soc_rv_modport_map[p];
                    } else {
                        break;
                    }
                }
                count++;
            }
        }
        if (port_count) {
            *port_count = count;
        }
        return (count > 0) ? SOC_E_NONE : SOC_E_NOT_FOUND;
    } else
#endif /* BCM_RAVEN_SUPPORT */
    if (SOC_IS_FBX(unit)) {
        int p;
        int count = 0;
        int offset;

        /* On Raptor port 0 is CPU port */
        offset = SOC_IS_RAPTOR(unit) ? 1 : 0;
        SOC_PBMP_CLEAR(hg);
        /* Other firebolt family devices */
        SOC_PBMP_WORD_SET(hg, 0, hg_pbm);
        /* Convert HG bitmap to port array*/
        for (p=0; p<4; p++) {
            if (SOC_PBMP_MEMBER(hg, p)) {
                if (count < port_max) {
                    port_array[count] = SOC_HG_OFFSET(unit) + p + offset;
                }
                if ((port_max != 0) &&
                    (port_max == count)) {
                    break;
                }
                count++;
            }
        }
        if (port_count) {
            *port_count = count;
        }
        return (count > 0) ? SOC_E_NONE : SOC_E_NOT_FOUND;
    }
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *  soc_xgs3_higig_bitmap_to_port
 * Purpose:
 *  Converts first bit set in HG port bitmap into Stacking port value
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  hg_pbm - HG port bitmap value
 *  port - port 
 * Returns:
 *  SOC_E_XXX
 */
int
soc_xgs3_higig_bitmap_to_port(int unit, uint32 hg_pbm,soc_port_t *port)
{
    return soc_xgs3_higig_bitmap_to_port_all(unit, hg_pbm, 1, port, NULL);
}

/*
 * Function:
 *  soc_xgs3_higig_bitmap_to_bitmap
 * Purpose:
 *  Converts HG port bitmap into absolute port bitmap
 * Parameters:
 *  unit - StrataSwitch unit #.
 *      hg_pbm - HG port bitmap value
 *      pbmp (OUT) - absolute port bitmap
 * Returns:
 *  SOC_E_XXX
 */

int
soc_xgs3_higig_bitmap_to_bitmap(int unit, pbmp_t hg_pbmp, pbmp_t *pbmp)
{
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_RAVEN(unit) || 
        (SOC_IS_TR_VL(unit) && 
        !(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || 
          SOC_IS_GREYHOUND2(unit)))) {
        soc_port_t hgp;
        int *hg_list, len;

        hg_list = NULL;
        len = 0;
#if defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_RAVEN(unit)) {
            hg_list = soc_rv_modport_map;
            len = sizeof(soc_rv_modport_map) / sizeof(soc_rv_modport_map[0]);
        }
#endif /* BCM_RAVEN_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_ENDURO_SUPPORT)         
            if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
                SOC_IS_HURRICANE2(unit)) {
                hg_list = soc_en_modport_map;
                len = sizeof(soc_en_modport_map) / sizeof(soc_en_modport_map[0]);
            } else 
#endif
            {
                hg_list = soc_tr_modport_map;
                len = sizeof(soc_tr_modport_map) / sizeof(soc_tr_modport_map[0]);
            }
        }
#endif /* BCM_TRIUMPH_SUPPORT */

        SOC_PBMP_CLEAR(*pbmp);
        PBMP_ITER(hg_pbmp, hgp) {
            if (hgp >= len) {
                return SOC_E_PORT;
            }
            SOC_PBMP_PORT_ADD(*pbmp, hg_list[hgp]);
        }
        return SOC_E_NONE;
    } else
#endif /* BCM_RAVEN_SUPPORT || BCM_TRIUMPH_SUPPORT */
    if (SOC_IS_FBX(unit)) {
        SOC_PBMP_WORD_SET(*pbmp, 0, SOC_PBMP_WORD_GET(hg_pbmp, 0) << SOC_HG_OFFSET(unit));
        return SOC_E_NONE;
    }
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *  soc_xgs3_bitmap_to_higig_bitmap
 * Purpose:
 *  Converts absolute port bitmap to HG port bitmap
 * Parameters:
 *  unit - StrataSwitch unit #.
 *      hg_pbm (OUT) - HG port bitmap value
 *      pbmp - absolute port bitmap
 * Returns:
 *  SOC_E_XXX
 */

int
soc_xgs3_bitmap_to_higig_bitmap(int unit, pbmp_t pbmp, pbmp_t *hg_pbm)
{
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_RAVEN(unit) || 
        (SOC_IS_TR_VL(unit) && 
        !(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || 
          SOC_IS_GREYHOUND2(unit)))) {
        soc_port_t p;
        int *hg_list, len, i;

        hg_list = NULL;
        len = 0;
#if defined(BCM_RAVEN_SUPPORT)
        if (SOC_IS_RAVEN(unit)) {
            hg_list = soc_rv_modport_map;
            len = sizeof(soc_rv_modport_map) / sizeof(soc_rv_modport_map[0]);
        }
#endif /* BCM_RAVEN_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(unit)) {
#if defined(BCM_ENDURO_SUPPORT)         
            if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
                SOC_IS_HURRICANE2(unit)) {
                hg_list = soc_en_modport_map;
                len = sizeof(soc_en_modport_map) / sizeof(soc_en_modport_map[0]);
            } else 
#endif
            {
                hg_list = soc_tr_modport_map;
                len = sizeof(soc_tr_modport_map) / sizeof(soc_tr_modport_map[0]);
            }
        }
#endif /* BCM_TRIUMPH_SUPPORT */

        SOC_PBMP_CLEAR(*hg_pbm);
        PBMP_ITER(pbmp, p) {
            for (i = 0; i < len; i++) {
                if (hg_list[i] == p) {
                    SOC_PBMP_PORT_ADD(*hg_pbm, i);
                    break;
                }
            }
            if (i == len) {
                return SOC_E_PORT;
            }
        }
        return SOC_E_NONE;
    } else
#endif /* BCM_RAVEN_SUPPORT */
    if (SOC_IS_FBX(unit)) {
         SOC_PBMP_WORD_SET(*hg_pbm, 0, SOC_PBMP_WORD_GET(pbmp, 0) >> SOC_HG_OFFSET(unit));
        return SOC_E_NONE;
    }
    return SOC_E_UNAVAIL;
} 

/*
 * Function:
 *  soc_xgs3_higig_bitmap_to_higig_port_num
 * Purpose:
 *  Converts HG port bitmap into higig port number
 * Parameters:
 *      hg_pbm - HG port bitmap value
 *      hg_port_num - HG port num
 * Returns:
 *  Higig port number - e.g. hg1 would be 1
 */

int
soc_xgs3_higig_bitmap_to_higig_port_num(uint32 hg_pbm, uint32 *hg_port_num)
{
    int result = 0;
    /* If empty pbm passed - configuration error */
    if (!hg_pbm) {
        return (SOC_E_CONFIG);
    }
    while (hg_pbm > 0) {
        hg_pbm = hg_pbm >> 1;
        result++;
    }

    *hg_port_num = result - 1;

    return (SOC_E_NONE);  
}

int
soc_xgs3_port_num_to_higig_port_num(int unit, int port, int *hg_port)
{

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        int *hg_list, len, i;

        hg_list = NULL;
        len = 0;
#if defined(BCM_ENDURO_SUPPORT)        
        if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
            SOC_IS_HURRICANE2(unit)) {
            hg_list = soc_en_modport_map;
            len = sizeof(soc_en_modport_map) / sizeof(soc_en_modport_map[0]);
        } else 
#endif
        {
            hg_list = soc_tr_modport_map;
            len = sizeof(soc_tr_modport_map) / sizeof(soc_tr_modport_map[0]);
        }
        for (i = 0; i < len; i++) {
            if (hg_list[i] == port) {
                *hg_port = i;
                return SOC_E_NONE;
            }
        }
    }
#endif  /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        if (port > 0 && port < 25) {
            *hg_port = port;
            return SOC_E_NONE;
        }
    }
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        if (port > 0 && port < 21) {
            *hg_port = port;
            return SOC_E_NONE;
        }
    }
#endif  /* BCM_BRADLEY_SUPPORT */

    return SOC_E_PORT;
}

#endif /* BCM_FIREBOLT_SUPPORT */
