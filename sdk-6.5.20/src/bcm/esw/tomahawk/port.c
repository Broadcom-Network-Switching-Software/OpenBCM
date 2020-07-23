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
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/drv.h>
#include <soc/tomahawk.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/portctrl.h>

/*
 * Function:
 *      bcm_th_port_rate_egress_set
 * Purpose:
 *      Set egress metering information
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
bcm_th_port_rate_egress_set(int unit, bcm_port_t port, uint32 bandwidth,
                            uint32 burst, uint32 mode)
{
    soc_info_t *si;
    int phy_port, mmu_port, index, pipe;
    soc_mem_t mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    uint32 rval, itu_mode_sel;
    mmu_mtro_egrmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;
    int refresh_bitsize, bucket_bitsize;

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    sal_memset(&entry, 0, sizeof(entry));

    index = mmu_port & 0x3f;
    pipe = si->port_pipe[port];
    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    /* If metering is disabled on this ingress port we are done. */
    if (!bandwidth || !burst) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
        return BCM_E_NONE;
    }
    if((bandwidth < _BCM_PORT_RATE_MIN_KILOBITS ||
        bandwidth > _BCM_PORT_RATE_MAX_KILOBITS) ||
       (burst < _BCM_PORT_BURST_MIN_KILOBITS ||
        burst > _BCM_PORT_BURST_MAX_KILOBITS)){
        return BCM_E_PARAM;
    }
    flags = mode == _BCM_PORT_RATE_PPS_MODE ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_mmu_sed)) {
        BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));
        if (soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval, ITU_MODE_SELf)) {
            flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_MMU_SCFG_MISCCONFIGr(unit, &rval));
        itu_mode_sel =
            soc_reg_field_get(unit, MMU_SCFG_MISCCONFIGr, rval, ITU_MODE_SELf);

        if (itu_mode_sel) {
            flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
        }
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
 *      bcm_th_port_rate_egress_get
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
bcm_th_port_rate_egress_get(int unit, bcm_port_t port, uint32 *bandwidth,
                            uint32 *burst, uint32 *mode)
{
    soc_info_t *si;
    int phy_port, mmu_port, index, pipe;
    soc_mem_t mem;
    uint32 rval, itu_mode_sel;
    mmu_mtri_bkpmeteringconfig_mem_0_entry_t entry;
    uint32 refresh_rate, bucketsize, granularity, flags;

    if (bandwidth == NULL || burst == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    index = mmu_port & 0x3f;
    pipe = si->port_pipe[port];

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_mem_is_valid(unit, MMU_MTRO_EGRMETERINGCONFIGm)) {
        mem = MMU_MTRO_EGRMETERINGCONFIGm;
    } else 
#endif
    {
        mem = MMU_MTRO_EGRMETERINGCONFIG_MEMm;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));

    refresh_rate = soc_mem_field32_get(unit, mem, &entry, REFRESHf);
    bucketsize = soc_mem_field32_get(unit, mem, &entry, THD_SELf);
    granularity = soc_mem_field32_get(unit, mem, &entry, METER_GRANf);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_mem_field_valid(unit, mem, PACKET_SHAPINGf)) {
        flags = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf) ?
            _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    } else 
#endif
    {
        flags = soc_mem_field32_get(unit, mem, &entry, MODEf) ?
            _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (soc_feature(unit, soc_feature_mmu_sed)) {
        BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));
        if (soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval, ITU_MODE_SELf)) {
            flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
        }
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(READ_MMU_SCFG_MISCCONFIGr(unit, &rval));
        itu_mode_sel =
            soc_reg_field_get(unit, MMU_SCFG_MISCCONFIGr, rval, ITU_MODE_SELf);

        if (itu_mode_sel) {
            flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, flags, bandwidth,
                                         burst));
    *mode = flags & _BCM_TD_METER_FLAG_PACKET_MODE ?
        _BCM_PORT_RATE_PPS_MODE : _BCM_PORT_RATE_BYTE_MODE;

    return BCM_E_NONE;
}

int bcm_th_port_drain_cells(int unit, int port)
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

/*
 * Function    : _bcm_th_port_speed_supported
 * Description : Helper function for bcm_esw_port_speed_set, used
 *               to determine whether a requested speed can be
 *               supported by the current port lane config
 * Parameters  :
 *               unit  (IN)
 *               port  (IN)
 *               speed (IN)
 *               supported (OUT)
 * Returns     : BCM_E_XXX
 * Note        : Caller needs to ensure soc_feature_flexport_based_speed_set
 *               is defined
 */
bcm_error_t
_bcm_th_port_speed_supported(int unit, bcm_port_t port, int speed)
{
    SOC_IF_ERROR_RETURN(soc_th_port_speed_supported(unit, port, speed));
    return BCM_E_NONE;
}

int bcm_th_phy_lb_check(int unit,  bcm_port_t port, int loopback) {
    uint16 dev_id = 0;
    uint8 rev_id = 0;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((BCM56963_DEVICE_ID == dev_id) || (BCM56967_DEVICE_ID == dev_id)) {
        if(SOC_PBMP_MEMBER(loopback_disable[unit], port)) {
            if(loopback != BCM_PORT_LOOPBACK_PHY) {
                return BCM_E_UNAVAIL;
            }
        }
    }
    return BCM_E_NONE;
}

int bcm_th_phy_lb_set(int unit) {
    uint16 dev_id = 0;
    uint8 rev_id = 0;
    int port;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((BCM56963_DEVICE_ID == dev_id) || (BCM56967_DEVICE_ID == dev_id)){
        PBMP_ITER(loopback_disable[unit], port) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY));
        }
    }
    return BCM_E_NONE;
}


#endif /* BCM_TOMAHAWK_SUPPORT */
