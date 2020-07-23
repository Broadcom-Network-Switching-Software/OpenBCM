/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/drv.h>

#include <sal/core/time.h>
#include <shared/bsl.h>
#include <bcm/port.h>
#include <bcm/tx.h>
#include <bcm/error.h>

#include <soc/katana.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana.h>

#if defined(BCM_KATANA_SUPPORT)


#define BCM_KT_MAX_BURST_VALUE     16711

/*
 * Function:
 *      bcm_kt_port_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (IN)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                            uint32 kbits_sec, uint32 kbits_burst)
{
    int rv = BCM_E_NONE; 
    uint32 rate_exp, rate_mantissa;
    uint32 burst_exp, burst_mantissa;
    uint32 cycle_sel = 0;
    uint32 temp_burst_exp, temp_burst_mantissa;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_port_shaper_config_c_entry_t port_null_entry;
    lls_port_shaper_bucket_c_entry_t port_bucket_entry;

    soc_field_t rate_exp_f =  C_MAX_REF_RATE_EXPf;
    soc_field_t rate_mant_f = C_MAX_REF_RATE_MANTf;
    soc_field_t burst_exp_f = C_MAX_THLD_EXPf;
    soc_field_t burst_mant_f = C_MAX_THLD_MANTf; 
    soc_field_t cycle_sel_f = C_MAX_CYCLE_SELf;
   
    if (kbits_burst > BCM_KT_MAX_BURST_VALUE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                   "Invalid Burst value %d\n"), kbits_burst));
               return BCM_E_PARAM;
    }

    /* compute exp and mantissa and program the registers */
    rv = soc_katana_get_shaper_rate_info(unit, kbits_sec,
                                         &rate_mantissa, &rate_exp);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_get_shaper_burst_info(unit, kbits_burst,
                                          &burst_mantissa, &burst_exp, 0);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    /* compute recommended minimum burst and cycle_sel */
    if (rate_exp < 10) {
        temp_burst_exp = 5;
        temp_burst_mantissa = 0;
    } else {
        temp_burst_exp = rate_exp - 4;
        temp_burst_mantissa = (rate_mantissa >> 3);
    }

    if (rate_exp > 4) {
        cycle_sel = 0;
    } else if (rate_exp > 1) {    
        cycle_sel = 5 - rate_exp;
    } else {
        cycle_sel = 4;
    }

    if (burst_exp < temp_burst_exp) {
        burst_exp = temp_burst_exp;
        burst_mantissa = temp_burst_mantissa;
    } else {
        if ((burst_exp == temp_burst_exp) &&
            (burst_mantissa == temp_burst_mantissa)) {
            burst_exp = temp_burst_exp;
            burst_mantissa = temp_burst_mantissa;
        } else if ((burst_exp >= 14) && 
                   (burst_mantissa >= 127)) {
            /* maximum supported is 2064384 byte */
            burst_exp = 14;
            burst_mantissa = 127;
            cycle_sel = 9;
        } else { 
            rv =  soc_katana_compute_refresh_rate(unit, rate_exp, 
                                rate_mantissa, burst_exp, burst_mantissa, 
                                &cycle_sel); 
            if (BCM_FAILURE(rv)) {
                return(rv);
            }
        }
    }

    rv = soc_katana_compute_burst_rate_check(unit, rate_exp,
                    rate_mantissa, burst_exp, burst_mantissa,
                    cycle_sel);
    if (BCM_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Burst value %d is not supported for given rate %d\n"),
                                  kbits_burst, kbits_sec));
           return BCM_E_PARAM;
    }

    SOC_EGRESS_METERING_LOCK(unit);
    rv = soc_mem_read(unit, LLS_PORT_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                     port, &port_entry);
    if (BCM_FAILURE(rv)) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        rate_exp_f, rate_exp);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        rate_mant_f, rate_mantissa);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        burst_exp_f, burst_exp);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        burst_mant_f, burst_mantissa);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        cycle_sel_f, cycle_sel);    
    if (soc_feature(unit, soc_feature_dynamic_shaper_update)) {
        sal_memset(&port_null_entry, 0, sizeof(lls_port_shaper_config_c_entry_t));
        sal_memset(&port_bucket_entry, 0, sizeof(lls_port_shaper_bucket_c_entry_t));
        rv = soc_mem_write(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                MEM_BLOCK_ALL, port, &port_null_entry);
        if (BCM_FAILURE(rv)) {
            SOC_EGRESS_METERING_UNLOCK(unit);
            return rv;
        }
        soc_mem_field32_set(unit, LLS_PORT_SHAPER_BUCKET_Cm, &port_bucket_entry,
                            NOT_ACTIVE_IN_LLSf, 1);   
        rv = soc_mem_write(unit, LLS_PORT_SHAPER_BUCKET_Cm,
                           MEM_BLOCK_ALL, port, &port_bucket_entry);   
        if (BCM_FAILURE(rv)) {
            SOC_EGRESS_METERING_UNLOCK(unit);
            return rv;
        }
    }    
    rv = soc_mem_write(unit, LLS_PORT_SHAPER_CONFIG_Cm,
            MEM_BLOCK_ALL, port, &port_entry);
    if (BCM_FAILURE(rv)) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }

    SOC_EGRESS_METERING_UNLOCK(unit);
    return BCM_E_NONE; 
}

/*
 * Function:
 *      bcm_kt_port_rate_egress_get
 * Purpose:
 *      Get egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (OUT)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(OUT)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                            uint32 *kbits_sec, uint32 *kbits_burst)
{
    int rv = BCM_E_NONE;
    uint32 rate_exp, rate_mantissa;
    uint32 burst_exp, burst_mantissa;
    lls_port_shaper_config_c_entry_t port_entry;

    soc_field_t rate_exp_f =  C_MAX_REF_RATE_EXPf;
    soc_field_t rate_mant_f = C_MAX_REF_RATE_MANTf;
    soc_field_t burst_exp_f = C_MAX_THLD_EXPf;
    soc_field_t burst_mant_f = C_MAX_THLD_MANTf;

    /* Input parameters check. */
    if (!kbits_sec || !kbits_burst) {
        return (BCM_E_PARAM);
    }
    SOC_EGRESS_METERING_LOCK(unit);
    rv = soc_mem_read(unit, LLS_PORT_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
            port, &port_entry);
    SOC_EGRESS_METERING_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rate_exp = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm, 
                                   &port_entry,  rate_exp_f);
    rate_mantissa = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                        &port_entry, rate_mant_f);
    burst_exp = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                    &port_entry, burst_exp_f);
    burst_mantissa = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                         &port_entry, burst_mant_f);

    /* convert exp and mantissa to bps */
    rv = soc_katana_compute_shaper_rate(unit, rate_mantissa, rate_exp,
                                        kbits_sec);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_compute_shaper_burst(unit, burst_mantissa, burst_exp,
                                         kbits_burst);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }


    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_kt_port_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (IN)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                            uint32 kbits_sec, uint32 kbits_burst, uint32 mode)
{
  if (mode == _BCM_PORT_RATE_PPS_MODE) {
      return bcm_kt_port_pps_rate_egress_set(unit, port, kbits_sec, kbits_burst);
  } else {  
      return _bcm_kt_port_rate_egress_set(unit, port, kbits_sec, kbits_burst);
  }
}

/*
 * Function:
 *      bcm_kt_port_rate_egress_get
 * Purpose:
 *      Get egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (OUT)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(OUT)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                            uint32 *kbits_sec, uint32 *kbits_burst, uint32 *mode)
{
  return _bcm_kt_port_rate_egress_get(unit, port, kbits_sec, kbits_burst);
}

/*
 * Function:
 *      bcm_kt_port_pps_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit        - (IN)SOC unit number
 *      port       - (IN)Port number
 *      pps        - (IN)Rate in packets per second
 *                       Rate of 0 disables rate limiting.
 *      burst      -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_pps_rate_egress_set(int unit, bcm_port_t port,
                                uint32 pps, uint32 burst)
{
    lls_port_config_entry_t port_cfg;
    int packet_mode, new_mode;

    SOC_IF_ERROR_RETURN
        (READ_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    packet_mode = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg, 
                                      PACKET_MODE_WRR_ACCOUNTING_ENABLEf);
    if (pps == 0) {
        /* Packet mode shaper is being cleared */
        new_mode = 0;
    } else {
        /* Packet mode shaper is being enabled */
        new_mode = 1;
    }
    if (packet_mode != new_mode) {
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, 
                            PACKET_MODE_WRR_ACCOUNTING_ENABLEf, new_mode);
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, 
                            PACKET_MODE_SHAPER_ACCOUNTING_ENABLEf, new_mode);
        SOC_IF_ERROR_RETURN
            (WRITE_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    }    
        
    return _bcm_kt_port_rate_egress_set(unit, port, pps, burst);    
}

int
_bcm_kt_port_downsizer_check_port(int unit, int port, bcm_pkt_t *pkt) {
    int i,rv;
    int enable;
    uint64 tpok, rfcs, rpok;
    uint32 entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(READ_ING_EGRMSKBMAPm(unit, MEM_BLOCK_ANY, port,entry));
    soc_mem_field32_set(unit, ING_EGRMSKBMAPm, entry, BITMAP_W0f, 0xffffffff);
    soc_mem_field32_set(unit, ING_EGRMSKBMAPm, entry, BITMAP_W1f, 0x7f);
    SOC_IF_ERROR_RETURN(WRITE_ING_EGRMSKBMAPm(unit, MEM_BLOCK_ANY, port, entry));

    /* Setup */
    rv = bcm_esw_port_enable_get(unit, port, &enable);
    if(!enable) {
#ifdef BCM_PORT_DEFAULT_DISABLE
       if (bcm_esw_port_enable_set(unit, port, TRUE) < 0) {
           LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Port %d could not be enabled\n"), port));
       }
#endif
    }
    if (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d LoopBack Set Failed\n"), port));
    }
    /* Send Packets */
    for (i = 0; i < 10; i++) {
        BCM_PBMP_CLEAR(pkt->tx_pbmp);
        BCM_PBMP_PORT_ADD(pkt->tx_pbmp, port);
        rv = bcm_esw_tx(unit, pkt, NULL);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Tx Error %d\n"), rv));
            return BCM_E_FAIL;
        }
        sal_udelay(SAL_BOOT_SIMULATION ? 100000 : 100);
    }
    sal_udelay(SAL_BOOT_SIMULATION ? 1000000 : 10000);
    /* Verify Counters */
    SOC_IF_ERROR_RETURN(READ_TPOKr(unit, port, &tpok));
    SOC_IF_ERROR_RETURN(READ_RFCSr(unit, port, &rfcs));
    SOC_IF_ERROR_RETURN(READ_RPOKr(unit, port, &rpok));
    if ((COMPILER_64_HI(tpok) != 0) || (COMPILER_64_LO(tpok) != 10) ||
        (COMPILER_64_HI(rpok) != 0) || (COMPILER_64_LO(rpok) != 10) ||
        (COMPILER_64_HI(rfcs) != 0) || (COMPILER_64_LO(rfcs) != 0) ) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Port %d Stat Mismatch\n"), port));
        rv = BCM_E_FAIL;
    } else {
        rv = BCM_E_NONE;
    }
    /* Cleanup */
    soc_mem_field32_set(unit, ING_EGRMSKBMAPm, entry, BITMAP_W0f, 0);
    soc_mem_field32_set(unit, ING_EGRMSKBMAPm, entry, BITMAP_W1f, 0);
    SOC_IF_ERROR_RETURN(WRITE_ING_EGRMSKBMAPm(unit, MEM_BLOCK_ANY, port, entry));
    if (bcm_esw_l2_addr_delete_by_port(unit, -1, port, 0) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d L2 entry delete Failed\n"), port));
    }
    if (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d LoopBack Set Failed\n"), port));
    }
    if(!enable) {
#ifdef BCM_PORT_DEFAULT_DISABLE
       if (bcm_esw_port_enable_set(unit, port, FALSE) < 0) {
           LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Port %d could not be disabled\n"), port));
       }
    
#endif
    }
    if (bcm_esw_stat_clear(unit, port) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d Stat Clear Failed\n"), port));
    }
    return rv;
}

int _bcm_kt_port_downsizer_blk_reinit(int unit, int blk_num, int blk_port, bcm_pbmp_t blk_pbmp, int hotswap)
{
    uint32 rval, en_bkp, xctrl_bkp, xcfg;
    uint64 ctrl_bkp;
    int                 rv, port_enable;
    bcm_port_t          port;
    pbmp_t              okay_ports;
    soc_field_t hs_rst_fld[4] = {TOP_MXQ0_HOTSWAP_RST_Lf, TOP_MXQ1_HOTSWAP_RST_Lf,
                                 TOP_MXQ2_HOTSWAP_RST_Lf, TOP_MXQ3_HOTSWAP_RST_Lf};
    soc_field_t rst_fld[4] = {TOP_MXQ0_RST_Lf, TOP_MXQ1_RST_Lf,
                              TOP_MXQ2_RST_Lf, TOP_MXQ3_RST_Lf};

    if (hotswap) {
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, hs_rst_fld[blk_num], 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        sal_udelay(1000);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, hs_rst_fld[blk_num], 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    } else {
        SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit, blk_port, &en_bkp));
        SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_CTRLr(unit, blk_port, &xctrl_bkp));
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, blk_port, &ctrl_bkp));
        SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, blk_port, &xcfg));

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, rst_fld[blk_num], 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        sal_udelay(1000);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, rst_fld[blk_num], 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

        SOC_IF_ERROR_RETURN(WRITE_XPORT_PORT_ENABLEr(unit, blk_port, en_bkp));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_CTRLr(unit, blk_port, xctrl_bkp));
        rval = 0;
        soc_reg_field_set(unit, XPORT_XGXS_NEWCTL_REGr, &rval, TXD1G_FIFO_RSTBf,
                          0xf);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_NEWCTL_REGr(unit, blk_port, rval));
        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, blk_port, ctrl_bkp));
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, blk_port, xcfg));

        /* Probe for ports */
        SOC_PBMP_CLEAR(okay_ports);
        if ((rv = bcm_esw_port_probe(unit, blk_pbmp, &okay_ports)) !=
            BCM_E_NONE) {
            return rv;
        }
        
        /* Probe and initialize MAC and PHY drivers for ports that were OK */
        PBMP_ITER(blk_pbmp, port) {
            if ((rv = _bcm_port_mode_setup(unit, port, TRUE)) < 0) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Warning: Port %s: "
                                     "Failed to set initial mode: %s\n"),
                          SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
            }
#ifdef BCM_RCPU_SUPPORT
            if (SOC_IS_RCPU_ONLY(unit) && IS_RCPU_PORT(unit, port)) {
                if ((rv = soc_phyctrl_enable_set(unit, port, TRUE)) < 0) {
                    LOG_WARN(BSL_LS_BCM_PORT,
                             (BSL_META_U(unit,
                                         "Warning: Port %s: "
                                         "Failed to set Phyctrl Enable: %s\n"),
                              SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                }
                continue;
            }
#endif
    
#ifdef BCM_PORT_DEFAULT_DISABLE
            port_enable = FALSE;
#else
            port_enable = TRUE;
#endif
            if ((rv = bcm_esw_port_enable_set(unit, port, port_enable)) < 0) {
                LOG_WARN(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Warning: Port %s: "
                                     "Failed to %s port: %s\n"),
                          SOC_PORT_NAME(unit, port),(port_enable) ? \
                          "enable" : "disable" ,bcm_errmsg(rv)));
            }
    
#ifdef BCM_RCPU_SUPPORT
            if ((uint32)port == soc_property_get(unit, spn_RCPU_PORT, -1)) {
                if ((rv = bcm_esw_port_frame_max_set(unit, port, BCM_PORT_JUMBO_MAXSZ)) < 0) {
                    LOG_WARN(BSL_LS_BCM_PORT,
                             (BSL_META_U(unit,
                                         "Warning: Port %s: "
                                         "Failed to set Max Frame: %s\n"),
                              SOC_PORT_NAME(unit, port), bcm_errmsg(rv)));
                }
            }
#endif
        }
    }
    return BCM_E_NONE;
}

int _bcm_kt_port_downsizer_chk_reinit(int unit, uint32 *tx_pkt)
{
    bcm_pkt_t   *pkt_info;
    bcm_pbmp_t  blk_pbmp;
    bcm_port_t  port;
    int         blk, i;
    int         blk_num, blk_port, fail=0;
    uint32      mask;
    int ports_in_block[4][4] = {
            {25,-1,-1,-1},
            {26,-1,-1,-1},
            {27,32,33,34},
            {28,29,30,31}
        };
    soc_pbmp_t all_pbmp;
    int        local_port = 0;
    int        mxq2cnt = 4;
    int        mxq3cnt = 4;

    pkt_info = sal_alloc(sizeof(bcm_pkt_t), "pkt_info");
    if (NULL == pkt_info) {
        return BCM_E_MEMORY;
    }
    sal_memset(pkt_info, 0, sizeof(bcm_pkt_t));
    pkt_info->unit = unit;
    pkt_info->_pkt_data.data = (uint8 *) tx_pkt;
    pkt_info->pkt_data = &pkt_info->_pkt_data;
    pkt_info->blk_count = 1;
    pkt_info->_pkt_data.len = 68;
    pkt_info->call_back = NULL;
    pkt_info->flags = 0x00230020;


    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
    PBMP_ITER(all_pbmp, local_port) {
        if ((local_port == 27) || (local_port == 32) ||
            (local_port == 33) || (local_port == 34)) {
             mxq2cnt--;
        }
        if ((local_port == 28) || (local_port == 29) ||
            (local_port == 30) || (local_port == 31)) {
             mxq3cnt--;
        }
    }
    if (((mxq2cnt != 4) && (!SOC_PORT_VALID(unit, 27))) ||
        ((mxq3cnt != 4) && (!SOC_PORT_VALID(unit, 28)))) {
         LOG_VERBOSE(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping Downsizer logic \n")));
         if (pkt_info != NULL) {
            sal_free(pkt_info);
         }
         return BCM_E_NONE;
    } 


    SOC_BLOCK_ITER(unit, blk, SOC_BLK_MXQPORT) {
        blk_num =  SOC_BLOCK_INFO(unit, blk).number;
        blk_port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(READ_XPORT_PORT_ENABLEr(unit,blk_port,&mask));
        BCM_PBMP_CLEAR(blk_pbmp);
        /* enumerate all valid ports in this block */
        for (i=0; i<4; i++) {
            if (mask & (1<<i)) {
                BCM_PBMP_PORT_ADD(blk_pbmp, ports_in_block[blk_num][i]);
            }
        }

        /* Validate all ports in this block */
        fail = 0;
        PBMP_ITER(blk_pbmp, port) {
            if (_bcm_kt_port_downsizer_check_port(unit, port, pkt_info) == BCM_E_FAIL) {
                fail = 1;
                break;
            }
        }

        if (fail) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Block MXQPORT%d failed.. Trying HotSwap Reset\n"),
                         blk_num));
            (void)_bcm_kt_port_downsizer_blk_reinit(unit, blk_num, blk_port, blk_pbmp, TRUE);
            /* Re-Validate all ports in this block */
            fail = 0;
            PBMP_ITER(blk_pbmp, port) {
                if (_bcm_kt_port_downsizer_check_port(unit, port, pkt_info) == BCM_E_FAIL) {
                    fail = 1;
                    break;
                }
            }
            if (fail) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Block MXQPORT%d failed.. "
                                        "Trying Full Block Reset\n"), blk_num));
                BCM_IF_ERROR_RETURN(
                    _bcm_kt_port_downsizer_blk_reinit(unit, blk_num, blk_port, blk_pbmp, FALSE));
                    /* Re-Re-Validate all ports in this block */
                    fail = 0;
                    PBMP_ITER(blk_pbmp, port) {
                        if (_bcm_kt_port_downsizer_check_port(unit, port, pkt_info) == BCM_E_FAIL) {
                            fail = 1;
                            break;
                        }
                    }
                    if (fail) {
                        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                    (BSL_META_U(unit,
                                                "Block MXQPORT%d failed after WAR\n"),
                                     blk_num));
                        if (pkt_info != NULL) {
                            sal_free(pkt_info);
                        }
                        return BCM_E_FAIL;
                    }
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Block MXQPORT%d OK\n"), blk_num));
        }
    }
    if (pkt_info != NULL) {
        sal_free(pkt_info);
    }
    return BCM_E_NONE;
}

#define KT_WAR_BUF_SIZE	   256

int
bcm_kt_port_downsizer_chk_reinit(int unit) {
    uint32 *tx_pkt;
    uint32 *buff;
    int rv,i;
    sal_usecs_t stime = sal_time_usecs();
    
    uint32 pkt[17] = {0x66778899, 0xaabb0011, 0x22334455, 0x81000001,
                      0x002e0000, 0x56761234, 0x56771234, 0x56781234,
                      0x56791234, 0x567a1234, 0x567b1234, 0x567c1234,
                      0x567d1234, 0x567e1234, 0x567f1234, 0x56801234,
                      0x00000000};

    /* Applicable only for MXQ ports. */
    if (SOC_PBMP_IS_NULL(PBMP_MXQ_ALL(unit))) {         
        return BCM_E_NONE;
    }

    tx_pkt = soc_cm_salloc(unit, KT_WAR_BUF_SIZE, "tx_pkt");
    if (tx_pkt == NULL) {
        return BCM_E_MEMORY;
    }
    buff = tx_pkt;
    for(i = 0; i < 17; i++) {
        *buff = pkt[i];
        buff++;
    }
    soc_cm_sflush(unit, tx_pkt, KT_WAR_BUF_SIZE);
    rv = _bcm_kt_port_downsizer_chk_reinit(unit, tx_pkt);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port Downsizer WAR Failed\n")));
    }

    soc_cm_sfree(unit, tx_pkt);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Port Downsizer WAR: took %d usec\n"),
                            SAL_USECS_SUB(sal_time_usecs(), stime)));
    /* Always return OK so that further init can go on */
    return BCM_E_NONE;
}

#endif /* BCM_KATANA_SUPPORT */
