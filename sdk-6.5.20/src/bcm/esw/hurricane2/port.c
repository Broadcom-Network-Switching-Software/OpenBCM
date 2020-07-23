/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_HURRICANE2_SUPPORT)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hurricane2.h>
#include <bcm/error.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>

#include <bcm_int/esw_dispatch.h>

/*
 * Each TSC can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *    single port  40G   x    x    x
 *      dual port  20G   x   20G   x
 *   tri_023 port  20G   x   10G  10G
 *   tri_012 port  10G  10G  20G   x
 *      quad port  10G  10G  10G  10G
 *
 *          lanes                mode         valid lane index
 *       ------------      ----------------   ----------------
 *       new  current        new    current
 *       ---  -------      -------  -------
 * #1     4      1         single    quad            0
 * #2     4      1         single   tri_012          0
 * #3     4      2         single   tri_023          0
 * #4     4      2         single    dual            0
 * #5     2      1         tri_023   quad            0
 * #6     2      1         tri_012   quad            2
 * #7     2      1          dual    tri_023          2
 * #8     2      1          dual    tri_012          0
 * #9     2      4          dual    single           0
 * #10    1      2         tri_023   dual            2
 * #11    1      2         tri_012   dual            0
 * #12    1      2          quad    tri_023          0
 * #13    1      2          quad    tri_012          2
 * #14    1      4          quad    single           0
 * Following mode change requires 2 transition
 *   - from single to tri_023: #9 + #10
 *   - from single to tri_012: #9 + #11
 * Following mode change are the result of lane change on multiple ports
 *   - from quad to dual: #12 + #7 or #13 + #8
 *   - from dual to quad: #10 + #12 or #11 + # 13
 *   - from tri_023 to tri_012: #7 + #11 or #12 + #6
 *   - from tri_012 to tri_023: #8 + #10 or #13 + #5
 *
 * Logical port number will stay the same after conversion, for example
 *     converting single port to dual port, the logical port number of lane 0
 *     will be changed.
 */
int
soc_hurricane2_port_lanes_set(int unit, soc_port_t port_base, int lanes,
                            int *cur_lanes, int *phy_ports, int *phy_ports_len)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_field_t fields[2];
    uint32 values[2];
    int mode, cur_mode;
    int phy_port_base, port, i;
    int blk, bindex;
    uint32 rval;

    /* Find physical port number and lane index of the specified port */
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }

    bindex = -1;
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port_base, i);
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT) {
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port_base, i);
            break;
        }
    }

    /* Get the current mode */
    SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port_base, &rval));
    cur_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                 XPORT0_CORE_PORT_MODEf);

    /* Figure out the current number of lane from current mode */
    switch (cur_mode) {
    case SOC_HU2_PORT_MODE_QUAD:
        *cur_lanes = 1;
        break;
    case SOC_HU2_PORT_MODE_TRI_012:
        *cur_lanes = bindex == 0 ? 1 : 2;
        break;
    case SOC_HU2_PORT_MODE_TRI_023:
        *cur_lanes = bindex == 0 ? 2 : 1;
        break;
    case SOC_HU2_PORT_MODE_DUAL:
        *cur_lanes = 2;
        break;
    case SOC_HU2_PORT_MODE_SINGLE:
        *cur_lanes = 4;
        break;
    default:
        return SOC_E_FAIL;
    }

    if (lanes == 4 || *cur_lanes == 4) {
        if (bindex & 0x3) {
            return SOC_E_PARAM;
        }
    } else if (lanes == 2 || *cur_lanes == 2) {
        if (bindex & 0x1) {
            return SOC_E_PARAM;
        }
    }

    if (lanes == *cur_lanes) {
        return SOC_E_NONE;
    }

    /* Figure out new mode */
    if (lanes == 4) {
        mode = SOC_HU2_PORT_MODE_SINGLE;
    } else if (lanes == 2) {
        if (cur_mode == SOC_HU2_PORT_MODE_QUAD) {
            mode = bindex == 0 ?
                SOC_HU2_PORT_MODE_TRI_023 : SOC_HU2_PORT_MODE_TRI_012;
        } else {
            mode = SOC_HU2_PORT_MODE_DUAL;
        }
    } else{
        if (cur_mode == SOC_HU2_PORT_MODE_DUAL) {
            mode = bindex == 0 ?
                SOC_HU2_PORT_MODE_TRI_012 : SOC_HU2_PORT_MODE_TRI_023;
        } else {
            mode = SOC_HU2_PORT_MODE_QUAD;
        }
    }

    *phy_ports_len = 0;
    if (lanes > *cur_lanes) { /* Figure out which port(s) to be removed */
        if (lanes == 4) {
            if (cur_mode == SOC_HU2_PORT_MODE_TRI_012 ||
                cur_mode == SOC_HU2_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                    phy_ports[(*phy_ports_len)++] = phy_port_base + 1;
                }
            }
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                phy_ports[(*phy_ports_len)++] = phy_port_base + 2;
            }
            if (cur_mode == SOC_HU2_PORT_MODE_TRI_023 ||
                cur_mode == SOC_HU2_PORT_MODE_QUAD) {
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    phy_ports[(*phy_ports_len)++] = phy_port_base + 3;
                }
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                phy_ports[(*phy_ports_len)++] = phy_port_base + 1;
            }
        }
    } else { /* Figure out which port(s) to be added */
        if (lanes == 2) {
            if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                phy_ports[(*phy_ports_len)++] = phy_port_base + 2;
            }
        } else {
            if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
                phy_ports[(*phy_ports_len)++] = phy_port_base + 1;
            }
            if (cur_mode == SOC_HU2_PORT_MODE_SINGLE) {
                if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
                    phy_ports[(*phy_ports_len)++] = phy_port_base + 2;
                }
                if (si->port_p2l_mapping[phy_port_base + 3] != -1) {
                    phy_ports[(*phy_ports_len)++] = phy_port_base + 3;
                }
            }
        }
    }
    
    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        static char *mode_name[] = {
            "QUAD", "TRI_012", "TRI_023", "DUAL", "SINGLE"
        };
        LOG_CLI((BSL_META_U(unit,
                            "port %d physical port %d bindex %d\n"),
                 port_base, phy_port_base, bindex));
        LOG_CLI((BSL_META_U(unit,
                            "mode (new:%s cur:%s) lanes (new:%d cur:%d)\n"),
                 mode_name[mode], mode_name[cur_mode], lanes, *cur_lanes));
        for (i = 0; i < *phy_ports_len; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "%s physical port %d (port %d)\n"),
                     lanes > *cur_lanes ? "del" : "add",
                     phy_ports[i], si->port_p2l_mapping[phy_ports[i]]));
        }
    }

    /* Update soc_control information */
    SOC_CONTROL_LOCK(unit);
    if (lanes > *cur_lanes) { /* port(s) to be removed */
        for (i = 0; i < *phy_ports_len; i++) {
            port = si->port_p2l_mapping[phy_ports[i]];
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            if (IS_HG_PORT(unit, phy_ports[i])) {
                SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
            } else {
                SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
                if (IS_GE_PORT(unit, port)) {
                    SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
                }
            }
        }
    } else { /* port(s) to be added */
        for (i = 0; i < *phy_ports_len; i++) {
            port = si->port_p2l_mapping[phy_ports[i]];
            SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
            if (IS_HG_PORT(unit, port)) {
                SOC_PBMP_PORT_ADD(si->st.bitmap, port);
                SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
            } else {
                SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
                if (IS_GE_PORT(unit, port)) {
                    SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
                } else {
                    SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
                }
            }
        }
    }

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, port) { \
        si->ptype.port[si->ptype.num++] = port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = port; \
        } \
        if (port > si->ptype.max) { \
            si->ptype.max = port; \
        } \
    }

    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(xe);
    RECONFIGURE_PORT_TYPE_INFO(ge);

#undef RECONFIGURE_PORT_TYPE_INFO

    /* Update num of lanes info which is used by SerDes driver */
    SOC_PORT_NUM_LANES_SET(unit, port_base, lanes);
    for (i = 0; i < *phy_ports_len; i++) {
        port = si->port_p2l_mapping[phy_ports[i]];
        SOC_PORT_NUM_LANES_SET(unit, port, lanes > *cur_lanes ? 0 : lanes);
    }

    soc_dport_map_update(unit);
    SOC_CONTROL_UNLOCK(unit);

    fields[0] = XPORT0_CORE_PORT_MODEf;
    values[0] = mode;
    fields[1] = XPORT0_PHY_PORT_MODEf;
    values[1] = mode;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, XLPORT_MODE_REGr,
                                                port_base, 2, fields, values));

    if (*phy_ports_len == 0) {
        return SOC_E_NONE;
    }

    /* Update TDM */
#ifdef _HURRICANE2_DEBUG
    SOC_IF_ERROR_RETURN
        (_soc_hurricane2_port_lanes_update_tdm(unit, port_base, lanes,
                                             *cur_lanes, phy_ports,
                                             *phy_ports_len));
#endif
    return SOC_E_NONE;
}


int
soc_hurricane2_port_lanes_get(int unit, soc_port_t port_base, int *cur_lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    int cur_mode;
    int phy_port_base, i;
    int blk, bindex;
    uint32 rval;

    /* Find physical port number and lane index of the specified port */
    phy_port_base = si->port_l2p_mapping[port_base];
    if (phy_port_base == -1) {
        return SOC_E_PORT;
    }

    bindex = -1;
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port_base, i);
        if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT) {
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port_base, i);
            break;
        }
    }

    /* Get the current mode */
    SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port_base, &rval));
    cur_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                 XPORT0_CORE_PORT_MODEf);

    /* Figure out the current number of lane from current mode */
    switch (cur_mode) {
    case SOC_HU2_PORT_MODE_QUAD:
        *cur_lanes = 1;
        break;
    case SOC_HU2_PORT_MODE_TRI_012:
        *cur_lanes = bindex == 0 ? 1 : 2;
        break;
    case SOC_HU2_PORT_MODE_TRI_023:
        *cur_lanes = bindex == 0 ? 2 : 1;
        break;
    case SOC_HU2_PORT_MODE_DUAL:
        *cur_lanes = 2;
        break;
    case SOC_HU2_PORT_MODE_SINGLE:
        *cur_lanes = 4;
        break;
    default:
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

int
_bcm_hr2_dual_port_mode_pilot_tx(int unit, int port, bcm_pkt_t *pkt) {
    int rv;
    uint64 old_tpok;
    uint64 new_tpok;
    uint64 tx_ctl;

    /* Save original counter */
    SOC_IF_ERROR_RETURN(READ_TPOKr(unit, port, &old_tpok));

    /* Setup */
    if (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d LoopBack Set Failed\n"), port));
    }
    /* 
     * stop the packet from actually going out on the wire, 
     * while still incrementing the MAC counters 
     */
    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &tx_ctl));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctl, DISCARDf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, tx_ctl));

    /* Send Packets */
    BCM_PBMP_CLEAR(pkt->tx_pbmp);
    BCM_PBMP_PORT_ADD(pkt->tx_pbmp, port);
    rv = bcm_esw_tx(unit, pkt, NULL);
    if (rv < 0) {
    	LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Tx Error %d\n"), rv));
    	return BCM_E_INTERNAL; 
    }
    sal_udelay(10000);

    /* Verify Counters */
    SOC_IF_ERROR_RETURN(READ_TPOKr(unit, port, &new_tpok));
    LOG_VERBOSE(BSL_LS_BCM_PORT, \
                (BSL_META_U(unit, \
                            "old TPOK(port=%d)=0x%x, 0x%x\n"), \
                            port, COMPILER_64_HI(old_tpok), COMPILER_64_LO(old_tpok)));
    LOG_VERBOSE(BSL_LS_BCM_PORT, \
                (BSL_META_U(unit, \
                            "new TPOK(port=%d)=0x%x, 0x%x\n"), \
                            port, COMPILER_64_HI(new_tpok), COMPILER_64_LO(new_tpok)));
    if ((COMPILER_64_HI(new_tpok) == COMPILER_64_HI(old_tpok)) && \
        (COMPILER_64_LO(new_tpok) == COMPILER_64_LO(old_tpok))) {
        /* packet not transmit out */
        return BCM_E_FAIL;
    }
    LOG_VERBOSE(BSL_LS_BCM_PORT, \
                (BSL_META_U(unit, \
                            "Dual port mode check pass on port %d\n"),port));

    /* Cleanup */
    if (bcm_esw_l2_addr_delete_by_port(unit, -1, port, 0) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d L2 entry delete Failed\n"), port));
    }
    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &tx_ctl));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctl, DISCARDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, tx_ctl));
    if (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d LoopBack Set Failed\n"), port));
    }
    if (bcm_esw_stat_clear(unit, port) < 0) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Port %d Stat Clear Failed\n"), port));
    }
    return rv;
}

int _bcm_hr2_dual_port_mode_check(int unit, bcm_port_t port, uint32 *tx_pkt)
{
    bcm_pkt_t   pkt_info;
    uint32 rval = 0;
    int mode = 0;
    int timeout_cnt = 0;
    int i;
    int rv = 0;
    sal_memset(&pkt_info, 0, sizeof(bcm_pkt_t));
    pkt_info.unit = unit;
    pkt_info._pkt_data.data = (uint8 *) tx_pkt;
    pkt_info.pkt_data = &pkt_info._pkt_data;
    pkt_info.blk_count = 1;
    pkt_info._pkt_data.len = 68;
    pkt_info.call_back = NULL;
    pkt_info.flags = 0x20; 

    timeout_cnt = 0;
    for (i = 0; i < 10 ; i++) {
        /* coverity[overrun-buffer-val] */
        rv = _bcm_hr2_dual_port_mode_pilot_tx(unit, port, &pkt_info);
        if (rv == BCM_E_NONE) {
            break;
        } else {
            /* Re-write the MODEs */
            /* program the port to quad port mode */
            LOG_VERBOSE(BSL_LS_BCM_PORT, \
                        (BSL_META_U(unit, \
                                    "WAR step 1: program the port to quad port mode\n")));
            mode = 0; /* quad port mode */
            SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                              XPORT0_CORE_PORT_MODEf, mode);
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                              XPORT0_PHY_PORT_MODEf, mode);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));
    
            /* re-program the port to dual port mode */
            LOG_VERBOSE(BSL_LS_BCM_PORT, \
                        (BSL_META_U(unit, \
                                    "WAR step 2: re-program the port to dual port mode\n")));
            mode = 3; /* dual port mode */
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                              XPORT0_CORE_PORT_MODEf, mode);
            soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                              XPORT0_PHY_PORT_MODEf, mode);
    	    SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));
    	}
        timeout_cnt++;
    }
    LOG_VERBOSE(BSL_LS_BCM_PORT, \
                (BSL_META_U(unit, \
                            "WAR retried %d times on port %d\n"), timeout_cnt, port));
    if (timeout_cnt >= 10) {
        return BCM_E_TIMEOUT;
    }
    return BCM_E_NONE;
}

int
bcm_hr2_dual_port_mode_reinit(int unit) {
    uint32 *tx_pkt;
    uint32 *buff;
    int rv,i;
    sal_usecs_t stime = sal_time_usecs();
    bcm_port_t test_ports[2];
    uint16      dev_id;
    uint8       rev_id;
    int hu2_config_id;
    int check_required = 0;
    int enable = 0;
    uint32 pkt[17] = {0x66778899, 0xaabb0011, 0x22334455, 0x81000001,
                      0x002e0000, 0x56761234, 0x56771234, 0x56781234,
                      0x56791234, 0x567a1234, 0x567b1234, 0x567c1234,
                      0x567d1234, 0x567e1234, 0x567f1234, 0x56801234,
                      0x00000000};

    /* Qualify situation that should apply the WAR */
    /* get the port number for the checking */
    soc_cm_get_id(unit, &dev_id, &rev_id);
    hu2_config_id = soc_property_get(unit, spn_BCM5615X_CONFIG, 0);

    /* Test both the dual mode port */
    check_required = 0;
    switch (dev_id) {
        case BCM56150_DEVICE_ID:
        case BCM56151_DEVICE_ID:
        case BCM53346_DEVICE_ID:
        case BCM53347_DEVICE_ID:
        case BCM56152_DEVICE_ID:
        case BCM53344_DEVICE_ID:
            if ((hu2_config_id == 1) || (hu2_config_id == 10)) {
                check_required = 1;
                test_ports[0] = 28;
                test_ports[1] = 29;
            } else if (hu2_config_id == 11) {
                check_required = 1;
                test_ports[0] = 26;
                test_ports[1] = 27;

            }
            break;
        default :
            break;
    }
    
    if (check_required) {
        tx_pkt = soc_cm_salloc(unit, 256, "tx_pilot_pkt");
        if (tx_pkt == NULL) {
            return BCM_E_MEMORY;
        }
        buff = tx_pkt;
        for(i = 0; i < 17; i++) {
            *buff = pkt[i];
            buff++;
        }
        soc_cm_sflush(unit, tx_pkt, 256);
        for (i = 0; i < 2; i++) {
            /* Apply to valid ports */
            if (BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), test_ports[i])) {
                /* 
                 * Check if port is enabled already 
                 * Ports could be disabled in this state because of 
                 * 'CFLAGS += -DBCM_PORT_DEFAULT_DISABLE'
                 * Check the state and enable port if necessary.
                 */
                rv = bcm_esw_port_enable_get(unit, \
                                     test_ports[i], &enable);
                if (rv < 0) {
                    LOG_ERROR(BSL_LS_BCM_PORT, \
                              (BSL_META_U(unit, \
                                          "Failed to get Port %d enable status\n"), test_ports[i]));
                }
                if (!enable) {
                    rv = bcm_esw_port_enable_set(unit, \
                                            test_ports[i], 1);
                    if (rv < 0) {
                        LOG_ERROR(BSL_LS_BCM_PORT, \
                                  (BSL_META_U(unit, \
                                              "Failed to set Port %d enable status\n"), test_ports[i]));
                    }
                }
                LOG_VERBOSE(BSL_LS_BCM_PORT, \
                            (BSL_META_U(unit, \
                                        "Apply Dual Port Mode WAR to port(%d)\n"), test_ports[i]));
                rv = _bcm_hr2_dual_port_mode_check(unit, test_ports[i], tx_pkt);
                if (rv < 0) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "Dual port mode WAR Failed on port %d\n"), test_ports[i]));
                }

                /* recover the port enable state */
                if (!enable) {
                    rv = bcm_esw_port_enable_set(unit, \
                                         test_ports[i], enable);
                    if (rv < 0) {
                        LOG_ERROR(BSL_LS_BCM_PORT, \
                                  (BSL_META_U(unit, \
                                              "Failed to recover Port %d enable status\n"), test_ports[i]));
                    }
                }
            }
        }
        soc_cm_sfree(unit, tx_pkt);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Dual port mode WAR: took %d usec\n"),
                            SAL_USECS_SUB(sal_time_usecs(), stime)));

    /* Always return OK so that further init can go on */
    return BCM_E_NONE;
}

#endif
