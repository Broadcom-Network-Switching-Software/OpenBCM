/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    port.c
 * Purpose: Port Management
 */

#include <soc/defs.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/drv.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/portctrl.h>

int
bcm_td2_port_rate_ingress_set(int unit, bcm_port_t port, uint32 bandwidth,
                              uint32 burst)
{
    int phy_port, mmu_port, index;
    static soc_mem_t config_mem[] = {
        MMU_MTRI_BKPMETERINGCONFIG_MEM_0m,
        MMU_MTRI_BKPMETERINGCONFIG_MEM_1m
    };
    soc_mem_t mem;
    uint32 rval;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 pause, refresh_rate, bucketsize, granularity, flags;
    int refresh_bitsize, bucket_bitsize;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) {
        mem = config_mem[1];
        index = mmu_port & 0x3f;
    } else {
        mem = config_mem[0];
        index = mmu_port;
    }

    sal_memset(&entry, 0, sizeof(entry));

    /* If metering is disabled on this ingress port we are done. */
    if (!bandwidth || !burst) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        return BCM_E_NONE;
    }

    flags = 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    /* Discard threshold will be set to 12.5 % above pause threshold */
    pause = (burst * 8) / 9;
    refresh_bitsize = soc_mem_field_length(unit, mem, REFRESHCOUNTf);
    bucket_bitsize = soc_mem_field_length(unit, mem, PAUSE_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, bandwidth, pause, flags,
                                         refresh_bitsize, bucket_bitsize,
                                         &refresh_rate, &bucketsize,
                                         &granularity));

    soc_mem_field32_set(unit, mem, &entry, METER_GRANULARITYf, granularity);
    soc_mem_field32_set(unit, mem, &entry, REFRESHCOUNTf, refresh_rate);
    soc_mem_field32_set(unit, mem, &entry, PAUSE_THDf, bucketsize);
    soc_mem_field32_set(unit, mem, &entry, BKPDISCARD_ENf, 1);
    /* Set discard threshold to 12.5 % above pause threshold */
    soc_mem_field32_set(unit, mem, &entry, DISCARD_THDf, 3);
    /* Set resume threshold to 75% above pause threshold */
    /* RESUME_THD field in entry is 0 */

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    return BCM_E_NONE;
}

int
bcm_td2_port_rate_ingress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                              uint32 *burst)
{
    int phy_port, mmu_port, index;
    static soc_mem_t config_mem[] = {
        MMU_MTRI_BKPMETERINGCONFIG_MEM_0m,
        MMU_MTRI_BKPMETERINGCONFIG_MEM_1m
    };
    soc_mem_t mem;
    uint32 rval;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 pause, refresh_rate, bucketsize, granularity, flags;
    soc_info_t *si;

    if (bandwidth == NULL || burst == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) {
        mem = config_mem[1];
        index = mmu_port & 0x3f;
    } else {
        mem = config_mem[0];
        index = mmu_port;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    if (!soc_mem_field32_get(unit, mem, &entry, BKPDISCARD_ENf)) {
        *bandwidth = *burst = 0;
        return BCM_E_NONE;
    }

    refresh_rate = soc_mem_field32_get(unit, mem, &entry, REFRESHCOUNTf);
    bucketsize = soc_mem_field32_get(unit, mem, &entry, PAUSE_THDf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANULARITYf);

    flags = 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, flags, bandwidth,
                                         &pause));

    switch (soc_mem_field32_get(unit, mem, &entry, DISCARD_THDf)) {
    case 0:
        *burst = pause * 7 / 4; /* 75% above PAUSE_THD */
        break;
    case 1:
        *burst = pause * 3 / 2; /* 50% above PAUSE_THD */
        break;
    case 2:
        *burst = pause * 5 / 4; /* 25% above PAUSE_THD */
        break;
    case 3:
        *burst = pause * 9 / 8; /* 12.5% above PAUSE_THD */
        break;
    default:
        /* Should never happen */
        *burst = 0;
        break;
    }

    return BCM_E_NONE;
}

int
bcm_td2_port_rate_pause_get(int unit, bcm_port_t port, uint32 *pause,
                            uint32 *resume)
{
    int phy_port, mmu_port, index;
    static soc_mem_t config_mem[] = {
        MMU_MTRI_BKPMETERINGCONFIG_MEM_0m,
        MMU_MTRI_BKPMETERINGCONFIG_MEM_1m
    };
    soc_mem_t mem;
    uint32 rval;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 burst, bandwidth, bucketsize, granularity, flags;
    soc_info_t *si;

    if (pause == NULL || resume == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) {
        mem = config_mem[1];
        index = mmu_port & 0x3f;
    } else {
        mem = config_mem[0];
        index = mmu_port;
    }

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    if (!soc_mem_field32_get(unit, mem, &entry, BKPDISCARD_ENf)) {
        *pause = *resume = 0;
        return BCM_E_NONE;
    }

    bucketsize = soc_mem_field32_get(unit, mem, &entry, PAUSE_THDf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANULARITYf);

    flags = 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, 0, bucketsize, granularity,
                                         flags, &bandwidth, pause));

    switch (soc_mem_field32_get(unit, mem, &entry, RESUME_THDf)) {
    case 0:
        *resume = *pause * 3 / 4; /* 75% of PAUSE_THD */
        break;
    case 1:
        *resume = *pause * 1 / 2; /* 50% of PAUSE_THD */
        break;
    case 2:
        *resume = *pause * 1 / 4; /* 25% of PAUSE_THD */
        break;
    case 3:
        *resume = *pause * 1 / 8; /* 12.5% of PAUSE_THD */
        break;
    default:
        /* Should never happen */
        *resume = 0;
        break;
    }

    switch (soc_mem_field32_get(unit, mem, &entry, DISCARD_THDf)) {
    case 0:
        burst = *pause * 7 / 4; /* 75% above PAUSE_THD */
        break;
    case 1:
        burst = *pause * 3 / 2; /* 50% above PAUSE_THD */
        break;
    case 2:
        burst = *pause * 5 / 4; /* 25% above PAUSE_THD */
        break;
    case 3:
        burst = *pause * 9 / 8; /* 12.5% above PAUSE_THD */
        break;
    default:
        /* Should never happen */
        burst = 0;
        break;
    }

    *pause = burst - *pause;
    *resume = burst - *resume;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_rate_egress_get
 * Purpose:
 *      Get egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      bandwidth  - (IN) Kilobits per second or packets per second
 *                        zero if rate limiting is disabled
 *      burst      - (IN) Maximum burst size in kilobits or packets
 *      mode       - (IN) _BCM_PORT_RATE_BYTE_MODE or _BCM_PORT_RATE_PPS_MODE
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_rate_egress_set(int unit, bcm_port_t port, uint32 bandwidth,
                             uint32 burst, uint32 mode)
{
    int phy_port, mmu_port, index;
    static soc_mem_t config_mem[] = {
        MMU_MTRO_EGRMETERINGCONFIG_MEM_0m,
        MMU_MTRO_EGRMETERINGCONFIG_MEM_1m
    };
    soc_mem_t mem;
    uint32 rval;
    mmu_mtro_egrmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
    int refresh_bitsize, bucket_bitsize;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) {
        mem = config_mem[1];
        index = mmu_port & 0x3f;
    } else {
        mem = config_mem[0];
        index = mmu_port;
    }
    if (SOC_IS_MONTEREY(unit)) {
        mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    }  

    /*
     * The procedure followed is first disable the egress metering, then
     * re-enable the egress metering.
     *
     * NOTE: During the period of disabling and re-enabling the Egress metering
     * may be in-effective for couple of cycles
     */
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    /* If metering is disabled on this ingress port we are done. */
    if (!bandwidth || !burst) {
        return BCM_E_NONE;
    }

    flags = mode == _BCM_PORT_RATE_PPS_MODE ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    refresh_bitsize = soc_mem_field_length(unit, mem, REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, mem, THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, bandwidth, burst, flags,
                                         refresh_bitsize, bucket_bitsize,
                                         &refresh_rate, &bucketsize,
                                         &granularity));
    soc_mem_field32_set(unit, mem, &entry, MODEf,
                        mode == _BCM_PORT_RATE_PPS_MODE ? 1 : 0);
    soc_mem_field32_set(unit, mem, &entry, METER_GRANf, granularity);
    soc_mem_field32_set(unit, mem, &entry, REFRESHf, refresh_rate);
    soc_mem_field32_set(unit, mem, &entry, THD_SELf, bucketsize);

    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_rate_egress_get
 * Purpose:
 *      Get egress metering information
 * Parameters:
 *      unit       - (IN) Unit number
 *      port       - (IN) Port number
 *      bandwidth  - (OUT) Kilobits per second or packets per second
 *                         zero if rate limiting is disabled
 *      burst      - (OUT) Maximum burst size in kilobits or packets
 *      mode       - (OUT) _BCM_PORT_RATE_BYTE_MODE or _BCM_PORT_RATE_PPS_MODE
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_rate_egress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                             uint32 *burst, uint32 *mode)
{
    int phy_port, mmu_port, index;
    static soc_mem_t config_mem[] = {
        MMU_MTRO_EGRMETERINGCONFIG_MEM_0m,
        MMU_MTRO_EGRMETERINGCONFIG_MEM_1m
    };
    soc_mem_t mem;
    uint32 rval;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
    soc_info_t *si;


    if (bandwidth == NULL || burst == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) {
        mem = config_mem[1];
        index = mmu_port & 0x3f;
    } else {
        mem = config_mem[0];
        index = mmu_port;
    }

    if (SOC_IS_MONTEREY(unit)) {
        mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    }  
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    refresh_rate = soc_mem_field32_get(unit, mem, &entry, REFRESHf);
    bucketsize = soc_mem_field32_get(unit, mem, &entry, THD_SELf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANf);

    flags = soc_mem_field32_get(unit, mem, &entry, MODEf) ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, flags, bandwidth,
                                         burst));
    *mode = flags & _BCM_TD_METER_FLAG_PACKET_MODE ?
        _BCM_PORT_RATE_PPS_MODE : _BCM_PORT_RATE_BYTE_MODE;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_port_drain_cells
 * Purpose:
 *     To drain cells associated to the port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_td2_port_drain_cells(int unit, int port)
{
    mac_driver_t *macd;
    int rv = BCM_E_NONE;

    /* Call Port Control module */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return bcmi_esw_portctrl_egress_queue_drain(unit, port);
    }

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    
    PORT_LOCK(unit);
    rv = soc_mac_probe(unit, port, &macd);

    if (BCM_SUCCESS(rv)) {
        rv = MAC_CONTROL_SET(macd, unit, port, SOC_MAC_CONTROL_EGRESS_DRAIN, 1);

    }
    PORT_UNLOCK(unit);
    return rv;
}

int bcm_td2_port_drain_cells(int unit, int port)
{
    BCM_IF_ERROR_RETURN
        (_bcm_td2_port_drain_cells(unit, port));

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_port_queue_state_check(unit, port));

    return BCM_E_NONE;
}

/* Port table field programming - assumes PORT_LOCK is taken */
int
_bcm_td2_egr_port_set(int unit, bcm_port_t port,
                      soc_field_t field, int value)

{
    egr_port_entry_t pent;
    soc_mem_t mem = EGR_PORTm;
    int rv, cur_val;

    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }
    sal_memset(&pent, 0, sizeof(egr_port_entry_t));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, port, &pent);

    if (BCM_SUCCESS(rv)) {
        cur_val = soc_EGR_PORTm_field32_get(unit, &pent, field);
        if (value != cur_val) {
            soc_EGR_PORTm_field32_set(unit, &pent, field, value);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &pent);
        }
    }
    return rv;
}

int
_bcm_td2_egr_port_get(int unit, bcm_port_t port,
                      soc_field_t field, int *value)

{
    egr_port_entry_t pent;
    soc_mem_t mem = EGR_PORTm;

    if (!SOC_MEM_FIELD_VALID(unit, mem, field)) {
        return (BCM_E_UNAVAIL);
    }
    if (value == NULL) {
        return (BCM_E_PARAM);
    }

    sal_memset(&pent, 0, sizeof(egr_port_entry_t));
    BCM_IF_ERROR_RETURN
        (READ_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &pent));
    *value = soc_EGR_PORTm_field32_get(unit, &pent, field);

    return BCM_E_NONE;
}

int 
bcm_td2p_port_coe_e2ecc(int unit, bcm_port_t port,
        bcm_port_congestion_config_t *config)
{
    uint32 regval = 0;
    int e2ecc_enable = 0;
    uint32 field_val;
    int index;
    int copy_to_cpu = 0; 
    mmu_intfi_st_trans_tbl_entry_t st_trans_entry;

    if (config->flags & BCM_PORT_CONGESTION_CONFIG_E2ECC_COE_FLEX_MODE) {
        return BCM_E_UNAVAIL;
    }
     /*
      * e2ecc_enable here denotes the coe flow control mode we are operating 
      * 0 = flow control messages treated as normal data frames.
      * 1 = subtending ports send E2ECC pause frames only (only strict mode is supported)
      * 2 = subtending ports send VLAN pause frames .
      * 3 = reserved
      */
    if ((config->flags &
         BCM_PORT_CONGESTION_CONFIG_E2ECC_COE_STRICT_MODE) && 
        (config->flags & BCM_PORT_CONGESTION_CONFIG_RX)) {
        e2ecc_enable = 1;
    } else if ((config->flags &
         BCM_PORT_CONGESTION_CONFIG_VLAN_PAUSE_COE) && 
        (config->flags & BCM_PORT_CONGESTION_CONFIG_RX)) {
        e2ecc_enable = 2;
    }  

    if (config->flags &
         BCM_PORT_CONGESTION_CONFIG_COPY_TO_CPU) {
         copy_to_cpu = 1 ;
    }
  
    SOC_IF_ERROR_RETURN(
               READ_COE_FLOW_CONTROL_CONFIGr (unit, port, &regval));
    /* Enable the Flow control and E2ECC mode
     * 1. When there is enable request
     */
    soc_reg_field_set(unit, COE_FLOW_CONTROL_CONFIGr, &regval,
                    FLOW_CONTROL_FORMATf, e2ecc_enable);
    soc_reg_field_set(unit, COE_FLOW_CONTROL_CONFIGr, &regval,
                    COPY_TO_CPUf, copy_to_cpu);
    SOC_IF_ERROR_RETURN(
      WRITE_COE_FLOW_CONTROL_CONFIGr (unit, port, regval));

    if (e2ecc_enable == 1) {
        /* Configure E2ECC-FC to receive MAC-DA, Length/Type, and Opcode.
         * HW will match these settings so that we can get the subport tag
         * from 12 bits of MAC SA 
         */ 
        field_val = (config->dest_mac[0] << 8) |    /* MAC-DA[47:40] */
                    (config->dest_mac[1]);          /* MAC-DA[39:32] */
        SOC_IF_ERROR_RETURN(READ_ING_VOQFC_MACDA_MSr(unit, &regval));
        soc_reg_field_set(unit, ING_VOQFC_MACDA_MSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_ING_VOQFC_MACDA_MSr(unit, regval));

        field_val = (config->dest_mac[2] << 24) |   /* MAC-DA[31:24] */
                    (config->dest_mac[3] << 16) |   /* MAC-DA[23:16] */
                    (config->dest_mac[4] << 8) |    /* MAC-DA[15:8] */ 
                    (config->dest_mac[5]);          /* MAC-DA[7:0] */  

        SOC_IF_ERROR_RETURN(READ_ING_VOQFC_MACDA_LSr(unit, &regval));
        soc_reg_field_set(unit, ING_VOQFC_MACDA_LSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_ING_VOQFC_MACDA_LSr(unit, regval));


        SOC_IF_ERROR_RETURN(READ_ING_VOQFC_IDr(unit, &regval));
        soc_reg_field_set(unit, ING_VOQFC_IDr, &regval,
                          LENGTH_TYPEf, config->ethertype);
        soc_reg_field_set(unit, ING_VOQFC_IDr, &regval,
                          OPCODEf, config->opcode);
        SOC_IF_ERROR_RETURN(WRITE_ING_VOQFC_IDr(unit, regval));


        /*
         * Hardcoding the MMU_INTFI_ST_TRANS_TBL so the
         * congestion data is written as it is
         * in FC_ST_TABLE, This we are doing so that
         * congestion data is updated as it is
         * it recevied
         */

        for (index=0 ; index < soc_mem_index_count(unit,
                    MMU_INTFI_ST_TRANS_TBLm); index++) {
            SOC_IF_ERROR_RETURN(READ_MMU_INTFI_ST_TRANS_TBLm(
                        unit, MEM_BLOCK_ALL,index,
                        &st_trans_entry));
            soc_mem_field32_set(unit, MMU_INTFI_ST_TRANS_TBLm,
                    &st_trans_entry, FC_ST_XLATEf,index%16);
            SOC_IF_ERROR_RETURN(WRITE_MMU_INTFI_ST_TRANS_TBLm(
                        unit, MEM_BLOCK_ALL, index,
                        &st_trans_entry));
        }
        /*
         *  FC_WIDTH is set to 2 so that 4 bits of i
         * congestion data is read on each cycle
         */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, INTFI_CFGr, REG_PORT_ANY,
                                    FC_WIDTHf, 2));
        /*
         * MERGE_EN is set to 0, for COE messages 
         * to bypass the DMVOQ merge block
         */

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, INTFI_CFGr, REG_PORT_ANY,
                                    MERGE_ENf, 0));
        /* MODE_SEL is set to 1 to specify the message is of type
         *  DMVOQ/E2ECC
         */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, INTFI_CFGr, REG_PORT_ANY,
                                    MODE_SELf, 1));

        /* Setting VOQFC_Enable to false so that it will identify CoE
         * E2ECC messsages rather than VOQFC
         */
        SOC_IF_ERROR_RETURN(READ_IE2E_CONTROLr(unit, port, &regval));
        if (soc_reg_field_get(unit, IE2E_CONTROLr, regval,
                            VOQFC_ENABLEf)) {
            soc_reg_field_set(unit, IE2E_CONTROLr, &regval,
                          VOQFC_ENABLEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_IE2E_CONTROLr(unit, port, regval));
        }

        /* set CONGESTION_STATE_BYTES to 1 because we're only expecting 1
         * byte per interface */
        for (index = 0 ; index < SOC_REG_NUMELS(unit, CONGESTION_STATE_BYTESr)
                ; index++) {
            soc_reg_field_set(unit, CONGESTION_STATE_BYTESr, &regval,
                    DATAf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CONGESTION_STATE_BYTESr(unit,
                        index, regval));
        }
    } else if(e2ecc_enable == 2){ 

        field_val = (config->dest_mac[0] << 8) |    /* MAC-DA[47:40] */
                    (config->dest_mac[1]);          /* MAC-DA[39:32] */

        SOC_IF_ERROR_RETURN(READ_COE_VLAN_PAUSE_MACDA_MSr(unit, &regval));
        soc_reg_field_set(unit, COE_VLAN_PAUSE_MACDA_MSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_COE_VLAN_PAUSE_MACDA_MSr(unit, regval));

        field_val = (config->dest_mac[2] << 24) |   /* MAC-DA[31:24] */
                    (config->dest_mac[3] << 16) |   /* MAC-DA[23:16] */
                    (config->dest_mac[4] << 8) |    /* MAC-DA[15:8] */ 
                    (config->dest_mac[5]);          /* MAC-DA[7:0] */  

        SOC_IF_ERROR_RETURN(READ_COE_VLAN_PAUSE_MACDA_LSr(unit, &regval));
        soc_reg_field_set(unit, COE_VLAN_PAUSE_MACDA_LSr, &regval, DAf, field_val);
        SOC_IF_ERROR_RETURN(WRITE_COE_VLAN_PAUSE_MACDA_LSr(unit, regval));
        SOC_IF_ERROR_RETURN(READ_COE_VLAN_PAUSE_IDr(unit, &regval));
        soc_reg_field_set(unit, COE_VLAN_PAUSE_IDr, &regval,
                          LENGTH_TYPEf, config->ethertype);
        soc_reg_field_set(unit, COE_VLAN_PAUSE_IDr, &regval,
                          OPCODEf, config->opcode);
        SOC_IF_ERROR_RETURN(WRITE_COE_VLAN_PAUSE_IDr(unit, regval));

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, INTFI_CFGr, REG_PORT_ANY,
                                    FC_WIDTHf, 2));
    }

    return BCM_E_NONE;
}

#else /* BCM_TRIDENT2_SUPPORT */
typedef int _td2_port_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIDENT2_SUPPORT */
