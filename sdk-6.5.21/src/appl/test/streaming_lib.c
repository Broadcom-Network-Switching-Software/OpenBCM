/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Streaming library for diagnostic (TR) tests.
 * This library contains functions use by streaming diagnostic tests.
 */


#include <appl/diag/system.h>
#include <appl/diag/test.h>
#include <appl/diag/parse.h>

#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/cmicm.h>
#include <soc/mcm/cmicm.h>
#include <soc/cmic.h>
#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/iproc.h>
#include <soc/phyctrl.h>

#include <sal/types.h>
#include <bcm/vlan.h>
#include <bcm/port.h>

#include "gen_pkt.h"
#include "streaming_lib.h"

#if defined(BCM_ESW_SUPPORT) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))

#define DMA_CHAN_PER_CMC(unit) (SOC_VCHAN_NUM(unit) / SOC_CMCS_NUM(unit))

/*
 * Function:
 *      stream_print_port_config
 * Purpose:
 *      Print port config
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
void
stream_print_port_config(int unit, pbmp_t pbmp)
{
    int cnt = 0,port, port_speed;

    LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\n=========== PORT CONFIG INFO ============\n")));
    LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\n%4s %4s %4s %6s %6s\n"),
                        "idx", "port", "spd", "LR/OS", "EH/HG"));
    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, port, &port_speed);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%4d "), cnt));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%4d "), port));
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%3dG "), port_speed/1000));
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%6s "), "OVSB"));
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%6s "), "LR"));
            }
            if (IS_HG_PORT(unit, port)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%6s "), "HG"));
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "%6s "), "EHTN"));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
            cnt++;
        }
    }
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "\n\n")));
}

/*
 * Function:
 *      stream_get_port_pipe
 * Purpose:
 *      Get pipe number for a given device port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port #
 *
 * Returns:
 *     Pipe number.
 */
uint32
stream_get_port_pipe(int unit, int port)
{
    uint32 pipe;

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (SOC_PBMP_MEMBER(PBMP_PIPE(unit, pipe), port)) {
            break;
        }
    }
    return pipe;
}

/*
 * Function:
 *      stream_get_pkt_cell_cnt
 * Purpose:
 *      Get the cell count for a given packet size.
 * Parameters:
 *      unit: StrataSwitch Unit #
 *      pkt_size: packet size in bytes
 *      port: Device port #
 *
 * Returns:
 *     Cell count
 */
uint32
stream_get_pkt_cell_cnt(int unit, uint32 pkt_size, int port)
{
    uint32 num_cells = 1;
    uint32 i;

    if (IS_HG_PORT(unit, port)) {
        i = FIRST_CELL_HG2;
    }
    else {
        i = FIRST_CELL_ENET;
    }
    while (i < pkt_size) {
        num_cells++;
        i += CELL_SIZE;
    }
    return num_cells;
}

/*
 * Function:
 *      stream_get_safe_pkt_size
 * Purpose:
 *      Calculate safe packet size based on oversub ratio. The assumption is that
 *      a front panel port will behave essentially as a line rate port and show no
 *      bandwidth degradation for packet sizes above the safe packet size.
 *      This is based on input from device microarchitects. Broadcom internal
 *      users of this test can contact microarchitecture for more info.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      ovs_ratio_x1000: Oversub ratio x1000
 *
 * Returns:
 *     Safe packet size.
 */
uint32
stream_get_safe_pkt_size(int unit, int ovs_ratio_x1000)
{
    uint32 safe_size = 64;

    if (ovs_ratio_x1000 <= 1507) {
        safe_size =
            145 +
            ((((229000 - 144000) / (1507 - 1000)) * (ovs_ratio_x1000 -
                                                     1000)) / 1000);
    } else if ((ovs_ratio_x1000 > 1507) && (ovs_ratio_x1000 <= 1760)) {
        safe_size =
            353 +
            ((((416000 - 353000) / (1760 - 1508)) * (ovs_ratio_x1000 -
                                                     1508)) / 1000);
    } else if ((ovs_ratio_x1000 > 1760) && (ovs_ratio_x1000 <= 1912)) {
        safe_size =
            562 +
            ((((611000 - 562000) / (1912 - 1760)) * (ovs_ratio_x1000 -
                                                     1760)) / 1000);
    } else {
        safe_size = 770;
    }

    return safe_size;
}

/*
 * Function:
 *      stream_get_ll_flood_cnt
 * Purpose:
 *      Calculates number of packets that need to be sent to a port for a
 *      lossless swirl
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_size : Packet size in bytes
 *      port: Device port #
 * Returns:
 *     Number of packets needed for lossless flooding
 */
uint32
stream_get_ll_flood_cnt(int unit, int port, uint32 pkt_size,
                        uint32 *rand_pkt_size)
{
    uint32 flood_pkt_cnt = 0;
    uint32 total_cells = 0;

    if (pkt_size == 1 && rand_pkt_size != NULL) {
        while (total_cells < TARGET_CELL_COUNT &&
               flood_pkt_cnt < TARGET_CELL_COUNT) {
            total_cells += stream_get_pkt_cell_cnt(unit,
                                     rand_pkt_size[flood_pkt_cnt],
                                     port);
            flood_pkt_cnt++;
        }
    } else {
        total_cells = stream_get_pkt_cell_cnt(unit, pkt_size, port);
        if (total_cells > 0) {
            flood_pkt_cnt = TARGET_CELL_COUNT / total_cells;
        }
    }

    if (flood_pkt_cnt < 3) {
        flood_pkt_cnt = 3;
    }
    if ((flood_pkt_cnt % 2) == 1) {
        flood_pkt_cnt++;
    }

    return(flood_pkt_cnt);
}

/*
 * Function:
 *      stream_get_pipe_bandwidth
 * Purpose:
 *      Return bandwidth of the given pipe
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      flag: reserved for special pipe config, i.e. extra 2nd MGMT port.
 *
 * Returns:
 *    Nothing
 */
uint32
stream_get_pipe_bandwidth(int unit, uint32 flag)
{
    int pipe_bandwidth;
    int freq;
    uint16 dev_id;
    uint8 rev_id;
    int cal_universal;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    freq = SOC_INFO(unit).frequency;
    cal_universal = SOC_INFO(unit).fabric_port_enable ? 1 : 0;

    /* TH */
    if (dev_id == BCM56960_DEVICE_ID) {
        switch (freq) {
            case TH_FREQ_850: pipe_bandwidth = TH_BW_850; break;
            case TH_FREQ_765: pipe_bandwidth = TH_BW_765; break;
            case TH_FREQ_672: pipe_bandwidth = TH_BW_672; break;
            case TH_FREQ_645: pipe_bandwidth = TH_BW_645; break;
            case TH_FREQ_545: pipe_bandwidth = TH_BW_545; break;
            default: pipe_bandwidth = TH_BW_850; break;
        }
    }
    /* TD2P */
    else if (dev_id == BCM56860_DEVICE_ID || dev_id == BCM56850_DEVICE_ID) {
        switch (freq) {
            case TD2P_FREQ_793: pipe_bandwidth = TD2P_BW_793; break;
            case TD2P_FREQ_635: pipe_bandwidth = TD2P_BW_635; break;
            case TD2P_FREQ_529: pipe_bandwidth = TD2P_BW_529; break;
            case TD2P_FREQ_421: pipe_bandwidth = TD2P_BW_421; break;
            default: pipe_bandwidth = TD2P_BW_793; break;
        }
    }
    /* TH 2 */
    else if (dev_id == BCM56970_DEVICE_ID) {
        switch (freq) {
            case TH2_FREQ_1700: pipe_bandwidth = cal_universal ? 1050 : 1095; break;
            case TH2_FREQ_1625: pipe_bandwidth = cal_universal ? 1002 : 1047; break;
            case TH2_FREQ_1525: pipe_bandwidth = cal_universal ? 940 : 980; break;
            case TH2_FREQ_1425: pipe_bandwidth = cal_universal ? 877 : 915; break;
            case TH2_FREQ_1325: pipe_bandwidth = cal_universal ? 812 : 847; break;
            case TH2_FREQ_1275: pipe_bandwidth = cal_universal ? 782 : 815; break;
            case TH2_FREQ_1225: pipe_bandwidth = cal_universal ? 750 : 782; break;
            case TH2_FREQ_1125: pipe_bandwidth = cal_universal ? 687 : 717; break;
            case TH2_FREQ_1050: pipe_bandwidth = cal_universal ? 640 : 667; break;
            case TH2_FREQ_950:  pipe_bandwidth = cal_universal ? 575 : 600; break;
            case TH2_FREQ_850:  pipe_bandwidth = cal_universal ? 512 : 535; break;
            default: pipe_bandwidth = cal_universal ? 1050 : 1095; break;
        }
        pipe_bandwidth = pipe_bandwidth * 1000;
    }
    /* TD3 or TD3 2T or MV2*/
    else if (dev_id == BCM56870_DEVICE_ID || dev_id == BCM56873_DEVICE_ID ||
             dev_id == BCM56770_DEVICE_ID || dev_id == BCM56771_DEVICE_ID) {
        switch (freq) {
            case TD3_FREQ_1525: pipe_bandwidth = TD3_BW_1525; break;
            case TD3_FREQ_1425: pipe_bandwidth = TD3_BW_1425; break;
            case TD3_FREQ_1325: pipe_bandwidth = TD3_BW_1325; break;
            case TD3_FREQ_1275: pipe_bandwidth = TD3_BW_1275; break;
            case TD3_FREQ_1012: pipe_bandwidth = TD3_BW_1012; break;
            case TD3_FREQ_850:  pipe_bandwidth = TD3_BW_850;  break;
            default: pipe_bandwidth = TD3_FREQ_850; break;
        }
    }
    /* default */
    else {
        cli_out("\n*WARNING: Unrecognized DEVICE_ID %d\n", dev_id);
        pipe_bandwidth = TH_BW_850;
    }

    return pipe_bandwidth;
}

/*
 * Function:
 *      stream_get_ancl_bandwidth
 * Purpose:
 *      Return ancillary bandwidth of the given pipe
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      flag: reserved for special pipe config, i.e. extra 2nd MGMT port.
 *
 * Returns:
 *    Nothing
 */
uint32
stream_get_ancl_bandwidth(int unit, uint32 flag)
{
    int ancl_bandwidth;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    /* TH */
    if (dev_id == BCM56960_DEVICE_ID) {
        ancl_bandwidth = TH_MISC_BW;
    }
    /* TD2P */
    else if (dev_id == BCM56860_DEVICE_ID || dev_id == BCM56850_DEVICE_ID) {
        ancl_bandwidth = TD2P_MISC_BW;
    }
    /* TH 2 */
    else if (dev_id == BCM56970_DEVICE_ID) {
        ancl_bandwidth = TH2_MISC_BW;
    }
    /* TD3 or TD3 2T or MV2*/
    else if (dev_id == BCM56870_DEVICE_ID || dev_id == BCM56873_DEVICE_ID ||
             dev_id == BCM56770_DEVICE_ID || dev_id == BCM56771_DEVICE_ID) {
        ancl_bandwidth = TD3_MISC_BW;
    }
    /* default */
    else {
        cli_out("\n*WARNING: Unrecognized DEVICE_ID %d\n", dev_id);
        ancl_bandwidth = TH_MISC_BW;
    }

    return ancl_bandwidth;
}

/*
 * Function:
 *      stream_get_reg_tpkt_tbyt
 * Purpose:
 *      Get register of TPKTr and TBYTr
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      reg_tpkt: ptr to register TPKTr
 *      reg_tbyt: ptr to register TBYTr
 *
 * Returns:
 *    BCM_E_XXX
 */
bcm_error_t
stream_get_reg_tpkt_tbyt(int unit, int port, soc_reg_t* reg_tpkt, soc_reg_t* reg_tbyt)
{
    int phy_port;
    bcm_error_t rv = BCM_E_NONE;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if(phy_port == -1) {
        cli_out("\nERROR : stream_get_reg_tpkt_tbyt called with invalid "
                      "logical port %0d", port);
        return BCM_E_INTERNAL;
    }

    /* TH+ and later (TH+, TH2, TD3) */
    if (soc_feature(unit, soc_feature_cxl_mib)) {
        if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                             SOC_BLK_CLPORT)) {
            (*reg_tpkt) = CLMIB_TPKTr;
            (*reg_tbyt) = CLMIB_TBYTr;
        } else if (IS_QSGMII_PORT(unit, port) ||
                   IS_EGPHY_PORT(unit, port)) {
            (*reg_tpkt) = GTPKTr;
            (*reg_tbyt) = GTBYTr;
        } else {
            (*reg_tpkt) = XLMIB_TPKTr;
            (*reg_tbyt) = XLMIB_TBYTr;
        }
    /* TH, TD2P */
    } else if (SOC_IS_TOMAHAWK(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        (*reg_tpkt) = TPKTr;
        (*reg_tbyt) = TBYTr;
    }
    /* unrecognized device */
    else {
        rv = BCM_E_FAIL;
    }

    return rv;
}

/*
 * Function:
 *      stream_get_wc_pkt_size
 * Purpose:
 *      Get worst-case packet size for HiGig packet
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      hg_en: HiGig enable, 0->false, otherwise->true
 *
 * Returns:
 *      worst-case packet size
 */
uint32
stream_get_wc_pkt_size(int unit, int hg_en)
{
    if (SOC_IS_TRIDENT3X(unit)) {
        return HG2_WC_PKT_SIZE;
    }

    if (hg_en == 0) {
        return ENET_WC_PKT_SIZE;
    } else {
        return HG2_WC_PKT_SIZE;
    }
}

/*
 * Function:
 *      stream_set_vlan
 * Purpose:
 *      Validates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *      enable - 0->disable, otherwise->enable.
 *
 * Returns:
 *     Nothing
 */
void
stream_set_vlan(int unit, bcm_vlan_t vlan, int enable)
{
    uint32 value;
    vlan_tab_entry_t vlan_tab_entry;
    vlan_2_tab_entry_t vlan_2_tab_entry;

    if (SOC_MEM_IS_VALID(unit, VLAN_TABm)) {
        value = (enable == 0) ? (0x0) : (0x1);
        (void) soc_mem_read(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
        soc_mem_field32_set(unit, VLAN_TABm, vlan_tab_entry.entry_data,
                            VALIDf, value);
        (void) soc_mem_write(unit, VLAN_TABm, COPYNO_ALL, vlan, vlan_tab_entry.entry_data);
        if (SOC_MEM_IS_VALID(unit, VLAN_2_TABm)) {
            (void) soc_mem_read(unit, VLAN_2_TABm, COPYNO_ALL, vlan,
                                vlan_2_tab_entry.entry_data);
            soc_mem_field32_set(unit, VLAN_2_TABm, vlan_2_tab_entry.entry_data,
                                VALIDf, value);
            (void) soc_mem_write(unit, VLAN_2_TABm, COPYNO_ALL, vlan,
                                 vlan_2_tab_entry.entry_data);
        }
    } else {
        cli_out("\n*ERROR, invalid mem VLAN_TABm\n");
    }
}

/*
 * Function:
 *      stream_set_vlan_valid
 * Purpose:
 *      Validates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *
 * Returns:
 *     Nothing
 */
void
stream_set_vlan_valid(int unit, bcm_vlan_t vlan)
{
    stream_set_vlan(unit, vlan, 1);
}

/*
 * Function:
 *      stream_set_vlan_invalid
 * Purpose:
 *      InValidates VLAN in VLAN table
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *
 * Returns:
 *     Nothing
 */
void
stream_set_vlan_invalid(int unit, bcm_vlan_t vlan)
{
    stream_set_vlan(unit, vlan, 0);
}

/*
 * Function:
 *      stream_set_l3mtu
 * Purpose:
 *      set L3_MTU_VALUES
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *      mtu_value - MTU value
 *
 * Returns:
 *     Nothing
 */
static bcm_error_t
stream_set_l3mtu(int unit, bcm_vlan_t vlan, uint32 mtu_value)
{
    bcm_error_t rv = BCM_E_NONE;
    mtu_values_entry_t l3_mtu_values_entry;

    /*cli_out("Setting MTU to %0d for vlan/oif 0x%0x\n", mtu_value, vlan);*/
    SOC_IF_ERROR_RETURN(READ_L3_MTU_VALUESm(
                        unit, MEM_BLOCK_ANY, vlan, &l3_mtu_values_entry));
    soc_L3_MTU_VALUESm_field32_set(
                        unit, &l3_mtu_values_entry, MTU_SIZEf, mtu_value);
    SOC_IF_ERROR_RETURN(WRITE_L3_MTU_VALUESm(
                        unit, MEM_BLOCK_ALL, vlan, &l3_mtu_values_entry));

    return rv;
}

/*
 * Function:
 *      stream_set_l3mtu_valid
 * Purpose:
 *      set L3_MTU_VALUES to a valid value (max value)
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *
 * Returns:
 *     Nothing
 */
void
stream_set_l3mtu_valid(int unit, bcm_vlan_t vlan)
{
    (void) stream_set_l3mtu(unit, vlan, MTU);
}

/*
 * Function:
 *      stream_set_l3mtu_invalid
 * Purpose:
 *      set L3_MTU_VALUES to an invalid value (min_pkt_size-1)
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vlan - VLAN ID
 *
 * Returns:
 *     Nothing
 */
static void
stream_set_l3mtu_invalid(int unit, bcm_vlan_t vlan)
{
    (void) stream_set_l3mtu(unit, vlan, 63);
}

/*
 * Function:
 *      stream_set_lpbk
 * Purpose:
 *      Enable and disable MAC/PHY loopback on all ports specified by pbmp.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp: port bitmap
 *      loopback: loopback mode
 *
 * Returns:
 *     SOC_E_XXXX
 */
STATIC int _last_lpbk = BCM_PORT_LOOPBACK_NONE;
bcm_error_t
stream_set_lpbk(int unit, pbmp_t pbmp, int loopback)
{
    int port, lp_invalid = 0;

    _last_lpbk = loopback;

    if (loopback == BCM_PORT_LOOPBACK_MAC) {
        cli_out("\nEnabling MAC loopback");
    } else if (loopback == BCM_PORT_LOOPBACK_PHY) {
        cli_out("\nEnabling PCS local loopback");
    } else if (loopback == BCM_PORT_LOOPBACK_PHY_REMOTE) {
        cli_out("\nEnabling PMD loopback");
    } else if (loopback == BCM_PORT_LOOPBACK_NONE) {
        cli_out("\nDisabling loopback");
    } else {
        lp_invalid = 1;
        loopback = BCM_PORT_LOOPBACK_NONE;
        cli_out("\nInvalid loopback mode");
    }

    if (lp_invalid == 0) {
        PBMP_ITER(pbmp, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                if (loopback == BCM_PORT_LOOPBACK_NONE) {
                     if (_last_lpbk == BCM_PORT_LOOPBACK_PHY_REMOTE) {
                        BCM_IF_ERROR_RETURN(stream_set_pmd_lpbk(unit, port,
                                    BCM_PORT_PHY_CONTROL_LOOPBACK_PMD, 0));
                     } else {
                        BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                    loopback));
                    }
                } else if (loopback == BCM_PORT_LOOPBACK_PHY_REMOTE) {
                    BCM_IF_ERROR_RETURN(stream_set_pmd_lpbk(unit, port,
                                    BCM_PORT_PHY_CONTROL_LOOPBACK_PMD, 1));
                } else {
                    BCM_IF_ERROR_RETURN(bcm_port_loopback_set(unit, port,
                                    loopback));
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      stream_set_pmd_lpbk
 * Purpose:
 *      Enable PMD local loopback on a port.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: Device port number.
 *      type: PHY control type.
 *      value: PHY control value.
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_set_pmd_lpbk( int unit, bcm_port_t port, bcm_port_phy_control_t type, uint32 value)
{
    BCM_IF_ERROR_RETURN(bcm_port_phy_control_set(unit, port, type, value));
    BCM_IF_ERROR_RETURN(bcm_port_enable_set(unit, port, 1));
    return BCM_E_NONE;
}

/*
 * Function:
 *      stream_set_mac_lpbk
 * Purpose:
 *      Enable MAC loopback on all ports specified by pbmp.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp: port bitmap
 *
 * Returns:
 *     Nothing
 */
void
stream_set_mac_lpbk(int unit, pbmp_t pbmp)
{
    stream_set_lpbk(unit, pbmp, BCM_PORT_LOOPBACK_MAC);
}

/*
 * Function:
 *      stream_set_phy_lpbk
 * Purpose:
 *      Enable PHY loopback on all ports specified by pbmp.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp: port bitmap
 *
 * Returns:
 *     Nothing
 */
void
stream_set_phy_lpbk(int unit, pbmp_t pbmp)
{
    stream_set_lpbk(unit, pbmp, BCM_PORT_LOOPBACK_PHY);
}

/*
 * Function:
 *      stream_set_no_lpbk
 * Purpose:
 *      Clear loopback on all ports specified by pbmp.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp: port bitmap
 *
 * Returns:
 *     Nothing
 */
void
stream_set_no_lpbk(int unit, pbmp_t pbmp)
{
    stream_set_lpbk(unit, pbmp, BCM_PORT_LOOPBACK_NONE);
}

/*
 * Function:
 *      stream_turn_off_cmic_mmu_bkp
 * Purpose:
 *      Turn off CMIC to MMU backpressure on all channels
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
void
stream_turn_off_cmic_mmu_bkp(int unit)
{
    int cmc, ch;
    int vchan;
    uint32 threshold = CMICM_MMU_BKP_OFF_THRESHOLD;

    if (soc_feature(unit, soc_feature_cmicm) ||
        soc_feature(unit, soc_feature_cmicd_v2) ||
        soc_feature(unit, soc_feature_cmicd_v3)) {
        threshold = CMICM_MMU_BKP_OFF_THRESHOLD;
    } else if (soc_feature(unit, soc_feature_cmicx)) {
        threshold = CMICX_MMU_BKP_OFF_THRESHOLD;
    }

    for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < DMA_CHAN_PER_CMC(unit); ch++) {
            vchan = cmc * DMA_CHAN_PER_CMC(unit) + ch;
            soc_dma_chan_rxbuf_threshold_cfg(unit, vchan, threshold);
        }
    }

    if (soc_feature(unit, soc_feature_cmicx)) {
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
            soc_dma_cmc_rxbuf_threshold_cfg(unit, cmc, threshold);
        }
    }
}

/*
 * Function:
 *      stream_turn_off_idb_fc
 * Purpose:
 *      Turn off IDB flow control.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      idb_fcc_regs - ptr to idb flow control registers.
 *      a_size - number of registers in the input ptr.
 *
 * Returns:
 *     Nothing
 */
static void
stream_turn_off_idb_fc(int unit, soc_reg_t *idb_fcc_regs, int obm_per_pipe)
{
    int obm_subp;
    int pipe, obm, idx;
    uint64 idb_fc;
    soc_field_t idb_flow_control_config_fields[] = {
        PORT_FC_ENf,
        LOSSLESS1_FC_ENf,
        LOSSLESS0_FC_ENf,
        LOSSLESS1_PRIORITY_PROFILEf,
        LOSSLESS0_PRIORITY_PROFILEf
    };
    uint32 idb_flow_control_config_values[] = {0x0, 0x0, 0x0, 0xff, 0xff};

    cli_out("\nTurning off flow control at IDB/MMU");

    COMPILER_64_SET(idb_fc, 0x0, 0x0);

    if (SOC_REG_IS_VALID(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r)) {
        soc_reg_fields32_modify(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
                                REG_PORT_ANY, 5, idb_flow_control_config_fields,
                                idb_flow_control_config_values);
        (void) soc_reg_get(unit, IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r, 0, 0,
                           &idb_fc);
    }

    for(pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for(obm = 0; obm < obm_per_pipe; obm++) {
            idx = (pipe * obm_per_pipe) + obm;
            if(SOC_REG_IS_VALID(unit, idb_fcc_regs[idx])) {
                for (obm_subp = 0; obm_subp < NUM_SUBP_OBM; obm_subp++) {
                    (void) soc_reg_set(unit, idb_fcc_regs[idx], 0, obm_subp, idb_fc);
                }
            }
        }
    }
}

/*
 * Function:
 *      stream_turn_off_fc
 * Purpose:
 *      Turn off flow control at the MAC, IDB and MMU.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pbmp - Device port bitmap
 *
 * Returns:
 *     Nothing
 */
void
stream_turn_off_fc(int unit, pbmp_t pbmp)
{
    int port, idx, obm_per_pipe;

    soc_reg_t idb_fcc_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE1r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE2r,
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE3r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE3r
    };

    soc_reg_t pgw_fcc_regs[] = {
        PGW_OBM_PORT0_FC_CONFIGr,
        PGW_OBM_PORT1_FC_CONFIGr,
        PGW_OBM_PORT2_FC_CONFIGr,
        PGW_OBM_PORT3_FC_CONFIGr,
        PGW_OBM_PORT4_FC_CONFIGr,
        PGW_OBM_PORT5_FC_CONFIGr,
        PGW_OBM_PORT6_FC_CONFIGr,
        PGW_OBM_PORT7_FC_CONFIGr,
        PGW_OBM_PORT8_FC_CONFIGr,
        PGW_OBM_PORT9_FC_CONFIGr,
        PGW_OBM_PORT10_FC_CONFIGr,
        PGW_OBM_PORT11_FC_CONFIGr,
        PGW_OBM_PORT12_FC_CONFIGr,
        PGW_OBM_PORT13_FC_CONFIGr,
        PGW_OBM_PORT14_FC_CONFIGr,
        PGW_OBM_PORT15_FC_CONFIGr
    };

    soc_reg_t idb_mv2_fcc_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM4_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM5_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM6_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM7_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM8_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM9_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM10_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM11_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM12_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM13_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM14_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM15_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM16_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM17_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM18_FLOW_CONTROL_CONFIG_PIPE0r,
        IDB_OBM19_FLOW_CONTROL_CONFIG_PIPE0r
    };

    if (SOC_REG_IS_VALID(unit, THDI_INPUT_PORT_XON_ENABLESr)) {
        PBMP_ITER(pbmp, port) {
            if (port < SOC_MAX_NUM_PORTS) {
                bcm_port_pause_set(unit, port, FALSE, FALSE);
                soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr,
                                       port, PORT_PAUSE_ENABLEf, 0x0);
            }
        }
        soc_reg_field32_modify(unit, THDI_INPUT_PORT_XON_ENABLESr,
                               0, PORT_PAUSE_ENABLEf, 0x0);
    }

    if(SOC_IS_TOMAHAWK(unit)) { /*TH, TH+*/
        obm_per_pipe = 8;
    } else if (SOC_IS_MAVERICK2(unit)) {
        obm_per_pipe = 20;
    }
    else { /* All other devices are using 16 obm per pipe */
        obm_per_pipe = 16;
    }
    if (SOC_IS_MAVERICK2(unit)) stream_turn_off_idb_fc(unit, idb_mv2_fcc_regs, obm_per_pipe);
    else stream_turn_off_idb_fc(unit, idb_fcc_regs, obm_per_pipe);
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        for (idx = 0; idx < (sizeof(pgw_fcc_regs) / sizeof(soc_reg_t)); idx++) {
            if (SOC_REG_IS_VALID(unit, pgw_fcc_regs[idx])) {
                soc_reg_field32_modify(unit, pgw_fcc_regs[idx], REG_PORT_ANY,
                                       PORT_FC_ENABLEf, 0x0);
            }
        }
    }
}

/*
 * Function:
 *      stream_gen_random_l2_pkt
 * Purpose:
 *      Generate random L2 packet with seq ID
 * Parameters:
 *      pkt_ptr - ptr to packet.
 *      pkt_size - Packet size in bytes
 *      mac_da - Destination MAC Address
 *      mac_sa - Source MAC Address
 *      tpid - TPID
 *      vlan_id - VLAN
 *      seq_id - Sequence ID
 *
 * Returns:
 *     Nothing
 */
void
stream_gen_random_l2_pkt(uint8 *pkt_ptr, uint32 pkt_size, uint32 seq_id,
                         uint8 mac_da[NUM_BYTES_MAC_ADDR],
                         uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                         uint16 tpid, uint16 vlan_id)
{
    int i, offset;
    uint32 crc;

    tgp_gen_random_l2_pkt(pkt_ptr, pkt_size, mac_da, mac_sa, tpid, vlan_id);

    /* Replace First 4 Payload Bytes with Sequence ID */
    offset = NUM_BYTES_L2_HDR;
    pkt_ptr[offset + 0] = (seq_id >> (3 * 8)) & 0xff;
    pkt_ptr[offset + 1] = (seq_id >> (2 * 8)) & 0xff;
    pkt_ptr[offset + 2] = (seq_id >> (1 * 8)) & 0xff;
    pkt_ptr[offset + 3] = (seq_id >> (0 * 8)) & 0xff;

    /* Replace Next 2 Payload Bytes with Packet Size */
    pkt_ptr[offset + 4] = (pkt_size >> 8) & 0xff;
    pkt_ptr[offset + 5] = (pkt_size) & 0xff;

    /* Recalculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    offset = pkt_size - NUM_BYTES_CRC;
    for (i = NUM_BYTES_CRC; i > 0 ; i--) {
        pkt_ptr[offset++] = (crc >> ((i - 1) * 8)) & 0xff;
    }
}

/*
 * Function:
 *      stream_gen_random_l3_pkt
 * Purpose:
 *      Generate random L3 packet with seq ID
 * Parameters:
 *
 * Returns:
 *     Nothing
 */
void
stream_gen_random_l3_pkt(uint8 *pkt_ptr, uint8 ipv6_en,
                         uint32 pkt_size, uint32 seq_id,
                         uint8 mac_da[NUM_BYTES_MAC_ADDR],
                         uint8 mac_sa[NUM_BYTES_MAC_ADDR],
                         uint16 vlan_id, uint32 ip_da, uint32 ip_sa,
                         uint8 ttl, uint8 tos, uint8 sv_tag_en,
                         uint32 sv_tag)
{
    int i, offset;
    uint32 crc;
    offset = 0;
    tgp_gen_random_ip_pkt(ipv6_en, pkt_ptr, pkt_size, mac_da, mac_sa,
                          vlan_id, ip_da, ip_sa, ttl, tos, sv_tag_en,
                          sv_tag);

    /* Replace First 4 Payload Bytes with Sequence ID */
    if (ipv6_en == 1) {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV6_HDR;
    } else {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;
    }
    if (sv_tag_en == 1) offset+= NUM_BYTES_SV_TAG;

    pkt_ptr[offset + 0] = (seq_id >> (3 * 8)) & 0xff;
    pkt_ptr[offset + 1] = (seq_id >> (2 * 8)) & 0xff;
    pkt_ptr[offset + 2] = (seq_id >> (1 * 8)) & 0xff;
    pkt_ptr[offset + 3] = (seq_id >> (0 * 8)) & 0xff;

    /* Replace Next 2 Payload Bytes with Packet Size */
    if (sv_tag_en == 1) {
        pkt_ptr[offset + 4] = ((pkt_size - 4) >> 8) & 0xff;
        pkt_ptr[offset + 5] = (pkt_size - 4) & 0xff;
    } else {
        pkt_ptr[offset + 4] = (pkt_size >> 8) & 0xff;
        pkt_ptr[offset + 5] = (pkt_size) & 0xff;
    }
    /* Recalculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);
    if (sv_tag_en == 1) crc = 0;
    offset = pkt_size - NUM_BYTES_CRC;
    for (i = NUM_BYTES_CRC; i > 0 ; i--) {
        pkt_ptr[offset++] = (crc >> ((i - 1) * 8)) & 0xff;
    }
}

/*
 * Function:
 *      stream_gen_random_mpls_l3_pkt
 * Purpose:
 *      Generate random L3 packet with seq ID
 * Parameters:
 *
 * Returns:
 *     Nothing
 */
void
stream_gen_random_mpls_l3_pkt(
    uint8 *pkt_ptr,
    stream_pkt_t *tx_pkt,
    uint32 pkt_size,
    uint32 seq_id,
    uint8 tos)
{
    int i, offset;
    uint32 crc;
    int l2_hdr_size, mpls_hdr_size, ip_hdr_size;

    tgp_gen_random_mpls_ip_pkt(
        pkt_ptr,
        pkt_size,
        tx_pkt->mac_da,
        tx_pkt->mac_sa,
        tx_pkt->tx_vlan,
        tx_pkt->l3_en,
        tx_pkt->l3_mpls_en,
        tx_pkt->mpls_labels,
        tx_pkt->ipv6_en,
        tx_pkt->ipv6_da,
        tx_pkt->ipv6_sa,
        tx_pkt->ttl,
        tos);

    l2_hdr_size = NUM_BYTES_L2_HDR;
    mpls_hdr_size = (tx_pkt->l3_mpls_en == 1) ? (3 * NUM_BYTES_MPLS_HDR) : 0;
    ip_hdr_size = (tx_pkt->l3_en == 1) ?
                      ((tx_pkt->ipv6_en == 1) ? NUM_BYTES_IPV6_HDR :
                                                NUM_BYTES_IPV4_HDR) : 0;
    offset = l2_hdr_size + mpls_hdr_size + ip_hdr_size;

    pkt_ptr[offset + 0] = (seq_id >> (3 * 8)) & 0xff;
    pkt_ptr[offset + 1] = (seq_id >> (2 * 8)) & 0xff;
    pkt_ptr[offset + 2] = (seq_id >> (1 * 8)) & 0xff;
    pkt_ptr[offset + 3] = (seq_id >> (0 * 8)) & 0xff;

    /* Replace Next 2 Payload Bytes with Packet Size */
    pkt_ptr[offset + 4] = (pkt_size >> 8) & 0xff;
    pkt_ptr[offset + 5] = (pkt_size) & 0xff;

    pkt_ptr[offset + 6] = (tx_pkt->port) & 0xff;

    /* Recalculate CRC */
    tgp_populate_crc_table();
    crc = tgp_generate_calculate_crc(pkt_ptr, pkt_size);

    offset = pkt_size - NUM_BYTES_CRC;
    for (i = NUM_BYTES_CRC; i > 0 ; i--) {
        pkt_ptr[offset++] = (crc >> ((i - 1) * 8)) & 0xff;
    }
}

/*
 * Function:
 *      stream_gen_ref_l2_pkt
 * Purpose:
 *      Generate reference L2 packet by retrieving pkt_size and seq_id
 *      from the given rx packet.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - device port number
 *      pkt_intg - ptr to packet integrity struct, containing all necessary
 *                 parameters needed for integrity check.
 *      rx_pkt - ptr to rx packet.
 *      ref_pkt - ptr to reference packet.
 *      ref_pkt_size - packet size of reference packet
 *
 * Returns:
 *     Nothing
 */
static void
stream_gen_ref_l2_pkt(int unit, int port, stream_integrity_t *pkt_intg,
                      uint8 *rx_pkt, uint8 *ref_pkt, uint32 *ref_pkt_size)
{
    uint32 offset, seq_id, pkt_size;
    uint32 header_size = 0;

    soc_dma_header_size_get(unit, &header_size);
    seq_id   = 0x00000000;
    pkt_size = 0x00000000;
    offset   = NUM_BYTES_L2_HDR + header_size;

    seq_id   |= (rx_pkt[offset] << 24);
    seq_id   |= (rx_pkt[offset + 1] << 16);
    seq_id   |= (rx_pkt[offset + 2] << 8);
    seq_id   |= (rx_pkt[offset + 3]);
    pkt_size |= (rx_pkt[offset + 4] << 8);
    pkt_size |= (rx_pkt[offset + 5]);

    sal_srand(pkt_intg->port_pkt_seed[port] + seq_id);
    stream_gen_random_l2_pkt(ref_pkt, pkt_size, seq_id,
                             pkt_intg->mac_da[port],
                             pkt_intg->mac_sa[port],
                             TPID, pkt_intg->tx_vlan[port]);

    (*ref_pkt_size) = pkt_size;
}

/*
 * Function:
 *      stream_gen_ref_l3_pkt
 * Purpose:
 *      Generate reference L3 packet by retrieving pkt_size and seq_id
 *      from the given rx packet.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      port - device port number
 *      pkt_intg - ptr to packet integrity struct, containing all necessary
 *                 parameters needed for integrity check.
 *      rx_pkt - ptr to rx packet.
 *      ref_pkt - ptr to reference packet.
 *      ref_pkt_size - packet size of reference packet
 *
 * Returns:
 *     Nothing
 */
static void
stream_gen_ref_l3_pkt(int unit, int port, stream_integrity_t *pkt_intg,
                      uint8 *rx_pkt, uint8 *ref_pkt, uint32 *ref_pkt_size)
{
    uint32 offset, seq_id, pkt_size;
    uint8 ttl, tos = 0;
    uint32 header_size = 0;

    soc_dma_header_size_get(unit, &header_size);
    seq_id   = 0x00000000;
    pkt_size = 0x00000000;
    if (pkt_intg->ipv6_en == 1) {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV6_HDR;
    } else {
        offset = NUM_BYTES_L2_HDR + NUM_BYTES_IPV4_HDR;
    }
    offset += header_size;
    if (pkt_intg->sv_tag_en[port] == 1) offset+= NUM_BYTES_SV_TAG;

    seq_id   |= (rx_pkt[offset] << 24);
    seq_id   |= (rx_pkt[offset + 1] << 16);
    seq_id   |= (rx_pkt[offset + 2] << 8);
    seq_id   |= (rx_pkt[offset + 3]);
    pkt_size |= (rx_pkt[offset + 4] << 8);
    pkt_size |= (rx_pkt[offset + 5]);

    if (pkt_intg->ipv6_en == 1) {
        offset = NUM_BYTES_L2_HDR + 7; /* Byte 25 */
    } else {
        offset = NUM_BYTES_L2_HDR + 1; /* Byte 19 */
        if (pkt_intg->sv_tag_en[port]) {
            tos = rx_pkt[offset + header_size + NUM_BYTES_SV_TAG];
        } else {
            tos = rx_pkt[offset + header_size];
        }
        offset = NUM_BYTES_L2_HDR + 8; /* Byte 26 */
    }
    offset += header_size;
    if (pkt_intg->sv_tag_en[port] == 1) {
        offset+= NUM_BYTES_SV_TAG;
        pkt_size+= NUM_BYTES_SV_TAG;
    }
    ttl = rx_pkt[offset];

    sal_srand(pkt_intg->port_pkt_seed[port] + seq_id);
    stream_gen_random_l3_pkt(ref_pkt, pkt_intg->ipv6_en,
                             pkt_size, seq_id,
                             pkt_intg->mac_da[port],
                             pkt_intg->mac_sa[port],
                             pkt_intg->tx_vlan[port],
                             pkt_intg->ip_da[port],
                             pkt_intg->ip_sa[port],
                             ttl, tos, pkt_intg->sv_tag_en[port],
                             pkt_intg->sv_tag[port]);

    (*ref_pkt_size) = pkt_size;
}

/*
 * Function:
 *      stream_gen_ref_pkt
 * Purpose:
 *      Generate reference packet by retrieving pkt_size and seq_id
 *      from the given rx packet.
 * Parameters:
 *      pkt_intg - ptr to packet integrity struct, containing all necessary
 *                 parameters needed for integrity check.
 *      rx_pkt - ptr to rx packet.
 *      ref_pkt - ptr to reference packet.
 *      pkt_size - packet size of reference packet
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_gen_ref_pkt(int unit, int port, stream_integrity_t *pkt_intg,
                   uint8 *rx_pkt, uint8 *ref_pkt, uint32 *pkt_size)
{
    bcm_error_t rv = BCM_E_NONE;

    if (pkt_intg->type == PKT_TYPE_L2 ||
        pkt_intg->type == PKT_TYPE_L2_TS) {
        stream_gen_ref_l2_pkt(unit, port, pkt_intg, rx_pkt, ref_pkt, pkt_size);
    } else if (pkt_intg->type == PKT_TYPE_IPMC ||
               pkt_intg->type == PKT_TYPE_L3UC) {
        stream_gen_ref_l3_pkt(unit, port, pkt_intg, rx_pkt, ref_pkt, pkt_size);
    } else {
        rv = BCM_E_FAIL;
    }

    return rv;
}

/*
 * Function:
 *      stream_pktdma_tx
 * Purpose:
 *      Transmit one packet from CPU
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - ptr to tx dma descriptor vector
 *      pkt - ptr to store tx packet
 *      cnt - number of bytes for tx packet
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_pktdma_tx(int unit, dv_t *dv, uint8 *pkt, uint16 cnt)
{
    bcm_error_t rv = BCM_E_NONE;
    int channel_done = 0;
    uint32 timeout = TXDMA_TIMEOUT;
    uint32 flags = 0;
    pbmp_t lp_pbmp;
    pbmp_t empty_pbmp0;
    pbmp_t empty_pbmp1;

    SOC_PBMP_CLEAR(lp_pbmp);
    SOC_PBMP_PORT_ADD(lp_pbmp, 1);
    SOC_PBMP_CLEAR(empty_pbmp0);
    SOC_PBMP_CLEAR(empty_pbmp1);

    soc_dma_abort_dv(unit, dv);
    soc_dma_dv_reset(DV_TX, dv);
    soc_dma_desc_add(dv, (sal_vaddr_t) (pkt), cnt,
                     lp_pbmp, empty_pbmp0, empty_pbmp1, flags, NULL);
    soc_dma_desc_end_packet(dv);
    SOC_IF_ERROR_RETURN(
        soc_dma_chan_config(unit, TX_CHAN, DV_TX, SOC_DMA_F_POLL));
    soc_dma_start(unit, TX_CHAN, dv);

    while (channel_done == 0 && (timeout--) > 0) {
        soc_dma_chan_poll_done(unit, TX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
                               &channel_done);
    }
    if (channel_done == 0) {
        rv = BCM_E_FAIL;
    }
    soc_dma_done_desc(unit, TX_CHAN);

    return rv;
}

/*
 * Function:
 *      stream_pktdma_rx
 * Purpose:
 *      Receive packet by CPU from given vlan
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      rst_vlan - reset vlan
 *                     0 -> no reset
 *                     1 -> invalid vlan for L2 packet
 *                     2 -> invalid vlan for L3 packet
 *      vlan - vlan ID
 *      dv - ptr to rx dma descriptor
 *      pkt - ptr to store rx packet
 *      cnt - number of bytes of rx packet
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_pktdma_rx(int unit, int rst_vlan, bcm_vlan_t vlan,
                 dv_t *dv, uint8 *pkt, uint16 cnt)
{
    bcm_error_t rv = BCM_E_NONE;
    int k;
    int channel_done = 0;
    uint32 timeout = RXDMA_TIMEOUT;
    uint32 flags = 0;
    pbmp_t lp_pbmp;
    pbmp_t empty_pbmp0;
    pbmp_t empty_pbmp1;

    SOC_PBMP_CLEAR(lp_pbmp);
    SOC_PBMP_PORT_ADD(lp_pbmp, 1);
    SOC_PBMP_CLEAR(empty_pbmp0);
    SOC_PBMP_CLEAR(empty_pbmp1);

    cnt = (cnt <= MTU) ? (cnt) : (MTU);
    for (k = 0; k < cnt; k++) {
        pkt[k] = 0x00;
    }

    /* set up dma to receive packet */
    channel_done = 0;
    soc_dma_abort_dv(unit, dv);
    soc_dma_dv_reset(DV_RX, dv);
    soc_dma_desc_add(dv, (sal_vaddr_t) (pkt), cnt,
                     lp_pbmp, empty_pbmp0, empty_pbmp1, flags, NULL);
    soc_dma_desc_end_packet(dv);
    SOC_IF_ERROR_RETURN(
        soc_dma_chan_config(unit, RX_CHAN, DV_RX, SOC_DMA_F_POLL));
    soc_dma_start(unit, RX_CHAN, dv);

    /* forward packet to CPU by making packet size invalid */
    if (rst_vlan != 0) {
        if (rst_vlan == 1) {
            stream_set_vlan_invalid(unit, vlan);
        } else if (rst_vlan == 2) {
            stream_set_l3mtu_invalid(unit, vlan);
        }
    }

    /* wait until packet received or timeout */
    while (channel_done == 0 && (timeout--) > 0) {
        soc_dma_chan_poll_done(unit, RX_CHAN, SOC_DMA_POLL_CHAIN_DONE,
                               &channel_done);
    }
    if (channel_done == 0) {
        rv = BCM_E_FAIL;
    }

    /* valid vlan to disable forwarding packet to CPU */
    /* stream_set_vlan_valid(unit, vlan); */

    return rv;
}

/*
 * Function:
 *      stream_tx_pkt
 * Purpose:
 *      Transmit packet from CPU through pkt_dma
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      dv - ptr to tx dma descriptor vector
 *      pkt - ptr to store tx packet
 *      cnt - number of bytes for tx packet
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_tx_pkt(int unit, stream_pkt_t *tx_pkt)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 j;
    uint32 pkt_size;
    uint8 tos;
    uint8 *pkt_ptr;
    dv_t *dv_tx;

    dv_tx = soc_dma_dv_alloc(unit, DV_TX, 3);
    soc_dma_init(unit);

    tx_pkt->cnt_pkt = 0;
    pkt_size = tx_pkt->pkt_size;
    for (j = 0; j < tx_pkt->num_pkt; j++) {
        if (tx_pkt->rand_pkt_size_en == 1 && tx_pkt->rand_pkt_size != NULL) {
            pkt_size = tx_pkt->rand_pkt_size[j];
        }
        tos = (j%2) ? 0x55: 0xaa;
        pkt_ptr = sal_dma_alloc(pkt_size * sizeof(uint8), "packet");
        sal_srand(tx_pkt->pkt_seed + j);
        if (tx_pkt->new_flow_en == 1) {
            tos = 0;
            stream_gen_random_mpls_l3_pkt(pkt_ptr,
                                     tx_pkt,
                                     pkt_size,
                                     j,
                                     tos);
        } else if (tx_pkt->l3_en == 1) {
            stream_gen_random_l3_pkt(pkt_ptr,
                                     tx_pkt->ipv6_en,
                                     pkt_size,
                                     j,
                                     tx_pkt->mac_da,
                                     tx_pkt->mac_sa,
                                     (uint16) tx_pkt->tx_vlan,
                                     tx_pkt->ip_da,
                                     tx_pkt->ip_sa,
                                     tx_pkt->ttl, tos, tx_pkt->sv_tag_en,
                                     tx_pkt->sv_tag);
        } else {
            stream_gen_random_l2_pkt(pkt_ptr,
                                     pkt_size,
                                     j,
                                     tx_pkt->mac_da,
                                     tx_pkt->mac_sa,
                                     TPID,
                                     (uint16) tx_pkt->tx_vlan);
        }

        if (stream_pktdma_tx(unit, dv_tx, pkt_ptr, pkt_size) != BCM_E_NONE) {
            rv = BCM_E_FAIL;
            cli_out("*ERROR: Waiting for tx_pkt done, "
                    "port %0d, pkt_num %0d\n",
                    tx_pkt->port, j);
        } else {
            tx_pkt->cnt_pkt++;
        }
        sal_dma_free(pkt_ptr);
    }

    soc_dma_abort_dv(unit, dv_tx);
    soc_dma_dv_reset(DV_TX, dv_tx);
    soc_dma_dv_free(unit, dv_tx);

    return rv;
}

/*
 * Function:
 *      stream_calc_pipe_ovs_ratio
 * Purpose:
 *      Calculate oversub ratio for each pipe based on port config
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      pkt_size: packet size in bytes
 *      num_pipe: number of pipes per device
 *      pipe_ovs_ratio_x1000: ptr to pipe oversub ratio array
 *
 * Returns:
 *    SOC_E_XXXX
 */
static bcm_error_t
stream_calc_pipe_ovs_ratio(int unit, uint32 pkt_size, int num_pipe,
                           uint32 *pipe_ovs_ratio_x1000)
{
    int port, port_speed;
    int pipe, pipe_bandwidth = 0;
    int lr_bandwidth = 0, ov_bandwidth = 0;
    bcm_error_t rv = BCM_E_NONE;

    cli_out("\nCalculate pipe oversub ratio\n");

    /* initialize */
    pipe_bandwidth = stream_get_pipe_bandwidth(unit, 0) -
                     stream_get_ancl_bandwidth(unit, 0);

    for (pipe = 0; pipe < num_pipe; pipe++) {
        pipe_ovs_ratio_x1000[pipe] = 0;
    }

    cli_out("Freq = %0d, BW_per_pipe = %0d\n",
            SOC_INFO(unit).frequency, pipe_bandwidth);

    /* calculate oversub ratio for each pipe */
    for (pipe = 0; pipe < num_pipe; pipe++) {
        lr_bandwidth = 0;
        ov_bandwidth = 0;
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (port < SOC_MAX_NUM_PORTS) {
                if (SOC_PBMP_MEMBER(PBMP_PIPE(unit, pipe), port)) {
                    bcm_port_speed_get(unit, port, &port_speed);
                    if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port)) {
                        ov_bandwidth += port_speed;
                    } else {
                        lr_bandwidth += port_speed;
                    }
                }
            }
        }

        if ((pipe_bandwidth - lr_bandwidth) > 0) {
            pipe_ovs_ratio_x1000[pipe] =
                (ov_bandwidth * 1000) / (pipe_bandwidth - lr_bandwidth);
            if (pkt_size >
                stream_get_safe_pkt_size(unit, pipe_ovs_ratio_x1000[pipe])) {
                pipe_ovs_ratio_x1000[pipe] = 1000;
            }
            if (pipe_ovs_ratio_x1000[pipe] < 1000) {
                pipe_ovs_ratio_x1000[pipe] = 1000;
            }
        } else {
            pipe_ovs_ratio_x1000[pipe] = 0;
        }

        cli_out("Pipe %0d, LR_BW = %0d, OV_BW = %0d, ovs_ratio_x1000 = %0d\n",
                pipe, lr_bandwidth, ov_bandwidth,
                pipe_ovs_ratio_x1000[pipe]);
    }

    return rv;
}

/*
 * Function:
 *      stream_adjust_port_exp_rate_Mbps
 * Purpose:
 *      Guarantee port expected rate is no greater than port max speed.
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port: physical port number.
 *      exp_rate_Mbps: ptr to port rate in Mbps.
 *
 * Returns:
 *     SOC_E_XXXX
 */
static void
stream_adjust_port_exp_rate_Mbps(int unit, int port, uint64 *exp_rate_Mbps)
{
    int port_speed, encap;
    uint64 max_rate_64;

    if (exp_rate_Mbps != NULL) {
        if (port < SOC_MAX_NUM_PORTS) {
            /* get 64bit speed in Mbps */
            bcm_port_speed_get(unit, port, &port_speed);
            encap = (IS_HG_PORT(unit, port) | IS_HG2_ENABLED_PORT(unit, port)) ?
                    BCM_PORT_ENCAP_HIGIG2 : BCM_PORT_ENCAP_IEEE;
            if (!SOC_IS_TOMAHAWKX(unit)) {
                port_speed = stream_get_exact_speed(port_speed, encap);
            }
            COMPILER_64_ZERO(max_rate_64);
            COMPILER_64_SET(max_rate_64, 0, port_speed);

            /* check exp_rate with max_rate*/
            if (COMPILER_64_GT((*exp_rate_Mbps), max_rate_64)) {
                COMPILER_64_SET((*exp_rate_Mbps),
                                COMPILER_64_HI(max_rate_64),
                                COMPILER_64_LO(max_rate_64));
            }
        }
    }
}

/*
 * Function:
 *      stream_calc_exp_rate_by_rx
 * Purpose:
 *      Calculate expected rate array for all ports. This is based on port
 *      speed and pipe oversub ratio (for oversubscribed ports).
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port_rate_exp: ptr to expected rate array
 *      rate_calc: interface containing all necessary parameters for rate
 *                 calculation
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_calc_exp_rate_by_rx(int unit, uint64 *port_rate_exp,
                                stream_rate_t *rate_calc)
{
    int port, port_src, port_speed, encap;
    uint32 pkt_size, num_pipe;
    uint64 meg;
    uint64 ovs_ratio_x1000_64,traffic_load_64;
    uint32 *pipe_ovs_ratio;
    uint64 *port_rate;
    uint32 *param_src_port;

    bcm_error_t rv = BCM_E_NONE;

    cli_out("\nCalculate expected port rate\n");

    /* initialize */
    pkt_size = rate_calc->pkt_size;
    param_src_port = rate_calc->src_port;
    num_pipe = NUM_PIPE(unit);
    port_rate = port_rate_exp;
    pipe_ovs_ratio = (uint32 *) sal_alloc(num_pipe * sizeof(uint32),
                                          "pipe_ovs_ratio");
    if (pipe_ovs_ratio == NULL) {
        test_error(unit, "Failed to allocate memory for exp rate check\n");
        return BCM_E_FAIL;
    }
    sal_memset(pipe_ovs_ratio, 0, num_pipe * sizeof(uint32));

    COMPILER_64_SET(meg, 0, 1000000);
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        COMPILER_64_ZERO(port_rate[port]);
    }

    /* calculate oversub ratio for each pipe */
    stream_calc_pipe_ovs_ratio(unit, pkt_size, num_pipe, pipe_ovs_ratio);


    COMPILER_64_ZERO(traffic_load_64);
    /* calculate expected port rate */
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, port, &port_speed);
            encap = (IS_HG_PORT(unit, port) | IS_HG2_ENABLED_PORT(unit, port)) ?
                         BCM_PORT_ENCAP_HIGIG2 : BCM_PORT_ENCAP_IEEE;
            if (!SOC_IS_TOMAHAWKX(unit)) {
                port_speed = stream_get_exact_speed(port_speed, encap);
            }

            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port)) {
                COMPILER_64_SET(port_rate[port], 0, port_speed);
                COMPILER_64_UMUL_32(port_rate[port], 1000000);
                COMPILER_64_SET(ovs_ratio_x1000_64, 0,
                            pipe_ovs_ratio[stream_get_port_pipe(unit, port)]);
                COMPILER_64_UMUL_32(port_rate[port], 1000);
                COMPILER_64_SET(traffic_load_64, 0, 1000 * 100);
                if ( rate_calc->traffic_load != 0) {
                    COMPILER_64_UDIV_32(traffic_load_64,  rate_calc->traffic_load);
                }
                if (rate_calc->traffic_load != 0 &&
                    (!COMPILER_64_IS_ZERO(traffic_load_64)) &&
                    COMPILER_64_GT(traffic_load_64, ovs_ratio_x1000_64)) {
                    COMPILER_64_UDIV_64(port_rate[port], traffic_load_64);
                }
                else {
                    if (!COMPILER_64_IS_ZERO(ovs_ratio_x1000_64)) {
                        COMPILER_64_UDIV_64(port_rate[port], ovs_ratio_x1000_64);
                    }
                }
            } else {
                COMPILER_64_SET(port_rate[port], 0, port_speed);
                if (rate_calc->traffic_load != 0 &&
                    !COMPILER_64_IS_ZERO(traffic_load_64)) {
                    COMPILER_64_UDIV_64(port_rate[port], traffic_load_64);
                }
                COMPILER_64_UMUL_32(port_rate[port], 1000000);
            }
            COMPILER_64_UDIV_64(port_rate[port], meg); /* translate to Mbps */
        }
    }

    /* adjust expected port rate with src port rate */
    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            char rate_str[32];
            port_src = param_src_port[port];
            format_uint64_decimal(rate_str, port_rate[port], ',');
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "adjust expected port rate with src port rate dst_port = %8d src_port = %8d src_port rate = %7sM \n"),
                                 port, port_src, rate_str));
            if (port_src < SOC_MAX_NUM_PORTS &&
                port_src != port && port_src > 0) {
                COMPILER_64_SET(port_rate[port],
                                COMPILER_64_HI(port_rate[port_src]),
                                COMPILER_64_LO(port_rate[port_src]));
            }
        }
    }

    sal_free(pipe_ovs_ratio);
    return rv;
}

/*
 * Function:
 *      stream_calc_exp_rate_by_tx
 * Purpose:
 *      Calculate expected rate array for all ports. This is based on port
 *      speed and pipe oversub ratio (for oversubscribed ports).
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      port_rate_exp: ptr to expected rate array
 *      port_rate_act: ptr to actual rate array
 *      rate_calc: interface containing all necessary parameters for rate
 *                 calculation
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
stream_calc_exp_rate_by_tx(int unit, pbmp_t pbmp, uint64 *rate_exp,
                           uint64 *rate_act, stream_rate_t *rate_calc)
{
    uint32 port, p, n;
    uint32 *src_port_array;
    uint32 *num_rep_array;
    bcm_error_t rv = BCM_E_NONE;

    cli_out("\nCalculate expected port rate\n");

    /* initialize */
    src_port_array = rate_calc->src_port;
    num_rep_array  = rate_calc->num_rep;

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        COMPILER_64_ZERO(rate_exp[port]);
    }

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        if (port < SOC_MAX_NUM_PORTS) {
            p = src_port_array[port];
            n = num_rep_array[port];
            COMPILER_64_SET(rate_exp[port], COMPILER_64_HI(rate_act[p]),
                                            COMPILER_64_LO(rate_act[p]));
            if (n > 0) {
                COMPILER_64_UMUL_32(rate_exp[port], n);
            }
            /* make sure expected rate is no greater than config rate */
            stream_adjust_port_exp_rate_Mbps(unit, port, &(rate_exp[port]));
        }
    }

    return rv;
}


bcm_error_t
record_rate_information(int unit, uint32 emulation_param, pbmp_t pbmp, uint64 *time, uint64 *pkt_cnt, uint64 *byt_cnt)
{
    int port;
    uint64 rdata, emulation_factor;
    soc_reg_t reg_tpkt, reg_tbyt;

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            if (SOC_IS_GREYHOUND2(unit)) {
                time[port] = get_cur_time(unit, 0);
                if (emulation_param) {
                    COMPILER_64_SET(emulation_factor, 0, 2024);
                    COMPILER_64_UDIV_64(time[port], emulation_factor);
                }
                bcm_stat_sync_get(unit, port, snmpIfInNUcastPkts, &(pkt_cnt[port]));
                bcm_stat_sync_get(unit, port, snmpIfInOctets, &(byt_cnt[port]));
            } else {
                time[port] = get_cur_time(unit, emulation_param);
                if (SOC_IS_TOMAHAWK3(unit)){
                    bcm_stat_sync_get(unit, port, snmpIfInUcastPkts, &(pkt_cnt[port]));
                    bcm_stat_sync_get(unit, port, snmpIfInOctets, &(byt_cnt[port]));
                } else {
                    BCM_IF_ERROR_RETURN(stream_get_reg_tpkt_tbyt(unit, port,
                                     &reg_tpkt, &reg_tbyt));
                    (void) soc_reg_get(unit, reg_tpkt, port, 0, &rdata);
                    COMPILER_64_SET(pkt_cnt[port], COMPILER_64_HI(rdata),
                                           COMPILER_64_LO(rdata));

                    (void) soc_reg_get(unit, reg_tbyt, port, 0, &rdata);
                    COMPILER_64_SET(byt_cnt[port], COMPILER_64_HI(rdata),
                                           COMPILER_64_LO(rdata));
                }
           }
        }
    }
    return BCM_E_NONE;
}

void
calc_act_port_rate(int unit, pbmp_t pbmp, uint64 *time_start, uint64 *time_end,
                uint64 *pkt_start, uint64 *pkt_end, uint64 *byt_start,
                uint64 *byt_end, uint64 *port_rate)
{
    int port;
    uint32 ipg, preamble;
    uint64 time_delta, pkt_delta, byt_delta;

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            if (IS_HG_PORT(unit, port)) {
                ipg = HG2_IPG;
                preamble = 0;
            } else {
                ipg = ENET_IPG;
                preamble = ENET_PREAMBLE;
            }
            COMPILER_64_DELTA(time_delta, time_start[port], time_end[port]);

            COMPILER_64_DELTA(pkt_delta, pkt_start[port], pkt_end[port]);
            COMPILER_64_DELTA(byt_delta, byt_start[port], byt_end[port]);
            COMPILER_64_UMUL_32(pkt_delta, (ipg + preamble));
            COMPILER_64_ADD_64(byt_delta, pkt_delta);
            COMPILER_64_UMUL_32(byt_delta, 8);
            COMPILER_64_SET(port_rate[port], COMPILER_64_HI(byt_delta),
                                             COMPILER_64_LO(byt_delta));

            COMPILER_64_UDIV_64(port_rate[port], time_delta);
        }
    }
}


/*
 * Function:
 *      stream_calc_act_port_rate
 * Purpose:
 *      Calculate actual rate array for all ports. This is based on port
 *      speed and pipe oversub ratio (for oversubscribed ports).
 * Parameters:
 *      unit: StrataSwitch Unit #.
 *      scaling_factor: emulator speedup scaling factor, default 1;
 *      rate_interval: interval length for rate calculation
 *      port_rate_act : ptr to port actual rate array (x1000)
 *
 * Returns:
 *     SOC_E_XXXX
 */
static bcm_error_t
stream_calc_act_port_rate(int unit, uint32 emulation_param,
                          uint32 rate_interval, uint64 *port_rate_act)
{
    int port;
    bcm_error_t rv = BCM_E_NONE;
    uint32 interval_len;
    uint64 *port_rate;
    uint64 *time_start, *time_end;
    uint64 *tpkt_start, *tpkt_end,
           *tbyt_start, *tbyt_end;

    cli_out("\nCalculate actual port rate\n");

    /* initialize */
    interval_len = rate_interval;
    port_rate = port_rate_act;

    time_start = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                           "time_start");
    time_end   = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                           "time_end");
    tpkt_start = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                      "tpkt_start");
    tpkt_end   = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                      "tpkt_end");
    tbyt_start = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                      "tbyt_start");
    tbyt_end   = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                      "tbyt_end");
    if (time_start == NULL || time_end == NULL ||
        tpkt_start == NULL || tpkt_end == NULL ||
        tbyt_start == NULL || tbyt_end == NULL) {
        sal_free(time_start);
        sal_free(time_end);
        sal_free(tpkt_start);
        sal_free(tpkt_end);
        sal_free(tbyt_start);
        sal_free(tbyt_end);
        cli_out("*ERROR, Failed to allocate memory in "
                "stream_calc_act_port_rate()\n");
        return BCM_E_FAIL;
    }

    sal_memset(time_start, 0, SOC_MAX_NUM_PORTS * sizeof(uint64));
    sal_memset(time_end,   0, SOC_MAX_NUM_PORTS * sizeof(uint64));
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        COMPILER_64_ZERO(port_rate[port]);
        COMPILER_64_ZERO(tpkt_start[port]);
        COMPILER_64_ZERO(tpkt_end[port]);
        COMPILER_64_ZERO(tbyt_start[port]);
        COMPILER_64_ZERO(tbyt_end[port]);
    }

    /* wait 2s for traffic to stabilize */
    cli_out("-- Wait 2s for traffic to stabilize\n");
    sal_usleep(2000000);
    /* read counters at the beginning of interval */
    record_rate_information(unit, emulation_param, PBMP_PORT_ALL(unit), time_start,
                                               tpkt_start, tbyt_start);

    /* wait xs according to interval_len value */
    cli_out("-- Measure Rate over a period of %0ds\n", interval_len);
    sal_usleep(interval_len * 1000000);
    /* read counters at the end of interval */
    record_rate_information(unit, emulation_param, PBMP_PORT_ALL(unit), time_end,
                                               tpkt_end, tbyt_end);

    calc_act_port_rate(unit, PBMP_PORT_ALL(unit), time_start, time_end,
                       tpkt_start, tpkt_end, tbyt_start, tbyt_end,
                       port_rate);

    sal_free(time_start);
    sal_free(time_end);
    sal_free(tpkt_start);
    sal_free(tpkt_end);
    sal_free(tbyt_start);
    sal_free(tbyt_end);
    return rv;
}

/*
 * Function:
 *      stream_chk_dma_chain_done
 * Purpose:
 *      Check chain done or desc done bit in DMA status register.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      vchan - Virtual channel number
 *      type -  check desc done or chain done
 *      detected - whether desc done or chain done was detected
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_chk_dma_chain_done(int unit, int vchan, soc_dma_poll_type_t type,
                          int *detected)
{
    int rv = SOC_E_NONE;
    int cmc = vchan / N_DMA_CHAN;
    int chan = vchan % N_DMA_CHAN;

    switch (type) {
    case SOC_DMA_POLL_DESC_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_DESC_DONE(chan));
        break;
    case SOC_DMA_POLL_CHAIN_DONE:
        *detected = (soc_pci_read(unit, CMIC_CMCx_DMA_STAT_OFFSET(cmc))
                     & DS_CMCx_DMA_CHAIN_DONE(chan));
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *      stream_chk_mib_counters
 * Purpose:
 *      Checks MIB counters in MAC, IP, and EP
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      flag - set to 1 to check extra counter registers, default 0.
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_chk_mib_counters(int unit, pbmp_t pbmp, int flag)
{
    uint32 port, j, column;
    uint32 good_reg_num, error_reg_num;
    uint64 rdata;
    int pblk, phy_port;
    bcm_error_t rv = BCM_E_NONE;
    soc_reg_t good_counters[][4] = {
                                    {RUCr,    RUC_64r,       RUC_64r,     RUC_64r},
                                    {RPKTr,   XLMIB_RPKTr,   CLMIB_RPKTr, GRPKTr},
                                    {RUCAr,   XLMIB_RUCAr,   CLMIB_RUCAr, GRUCr},
                                    {RVLNr,   XLMIB_RVLNr,   CLMIB_RVLNr, INVALIDr},
                                    {RBYTr,   XLMIB_RBYTr,   CLMIB_RBYTr, GRBYTr},
                                    {RIPC4r,  RIPC4_64r,     RIPC4_64r,   RIPC4_64r},
                                    {TPKTr,   XLMIB_TPKTr,   CLMIB_TPKTr, GTPKTr},
                                    {TUCAr,   XLMIB_TUCAr,   CLMIB_TUCAr, GTUCr},
                                    {TVLNr,   XLMIB_TVLNr,   CLMIB_TVLNr, INVALIDr},
                                    {TBYTr,   XLMIB_TBYTr,   CLMIB_TBYTr, GTBYTr}
                                };
    soc_reg_t error_counters[][4] = {
                                    {RFLRr,   XLMIB_RFLRr,   CLMIB_RFLRr,   GRFLRr},
                                    {RFCSr,   XLMIB_RFCSr,   CLMIB_RFCSr,   GRFCSr},
                                    {RJBRr,   XLMIB_RJBRr,   CLMIB_RJBRr,   GRJBRr},
                                    {RMTUEr,  XLMIB_RMTUEr,  CLMIB_RMTUEr,  GRMTUEr},
                                    {RTRFUr,  XLMIB_RTRFUr,  CLMIB_RTRFUr,  INVALIDr},
                                    {RERPKTr, XLMIB_RERPKTr, CLMIB_RERPKTr, GRCDEr},
                                    {RRPKTr,  XLMIB_RRPKTr,  CLMIB_RRPKTr,  GRRPKTr},
                                    {RUNDr,   XLMIB_RUNDr,   CLMIB_RUNDr,   GRUNDr},
                                    {RFRGr,   XLMIB_RFRGr,   CLMIB_RFRGr,   GRFRGr},
                                    {RRBYTr,  XLMIB_RRBYTr,  CLMIB_RRBYTr,  GRRBYTr},
                                    {TJBRr,   XLMIB_TJBRr,   CLMIB_TJBRr,   GTJBRr},
                                    {TFCSr,   XLMIB_TFCSr,   CLMIB_TFCSr,   GTFCSr},
                                    {TERRr,   XLMIB_TERRr,   CLMIB_TERRr,   GTXCLr},
                                    {TFRGr,   XLMIB_TFRGr,   CLMIB_TFRGr,   GTFRGr},
                                    {TRPKTr,  XLMIB_TRPKTr,  CLMIB_TRPKTr,  INVALIDr},
                                    {TUFLr,   XLMIB_TUFLr,   CLMIB_TUFLr,   INVALIDr},
                                    {TPCEr,   TPCE_64r,      TPCE_64r,      TPCE_64r},
                                    {RDISCr,  RDISC_64r,     RDISC_64r,     RDISC_64r},
                                    {RIPHE4r, RIPHE4_64r,    RIPHE4_64r,    RIPHE4_64r},
                                    {RIPHE6r, RIPHE6_64r,    RIPHE6_64r,    RIPHE6_64r},
                                    {RIPD4r,  RIPD4_64r,     RIPD4_64r,     RIPD4_64r},
                                    {RIPD6r,  RIPD6_64r,     RIPD6_64r,     RIPD6_64r},
                                    {RPORTDr, RPORTD_64r,    RPORTD_64r,    RPORTD_64r}
                                 };

    good_reg_num = sizeof(good_counters) / sizeof(good_counters[0]);
    error_reg_num = sizeof(error_counters) / sizeof(error_counters[0]);
    cli_out("\n==================================================\n");
    cli_out("Checking Counter ...\n");

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            column = 0;

            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            if(phy_port == -1) {
                cli_out("\nERROR : Invalid logical port %0d used in counter "
                              "check.", port);
                return BCM_E_INTERNAL;
            }

            if (soc_feature(unit, soc_feature_cxl_mib)) {
                pblk = SOC_PORT_BLOCK(unit, phy_port);
                if (SOC_BLOCK_IS_CMP(unit, pblk, SOC_BLK_CLPORT)) {
                    column = 2;
                } else if (IS_QSGMII_PORT(unit, port) ||
                           IS_EGPHY_PORT(unit, port)) {
                    column = 3;
                } else {
                    column = 1;
                }
            }
            if (flag != 0) {
                for (j = 0; j < good_reg_num; j++) {
                    if (SOC_REG_IS_VALID(unit, good_counters[j][column])) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg_get(unit, good_counters[j][column], port, 0, &rdata));
                        if (COMPILER_64_IS_ZERO(rdata)) {
                            cli_out("*ERROR: (a) Counter %s "
                                    "has a zero value for port %0d\n",
                                    SOC_REG_NAME(unit, good_counters[j][column]), port);
                            rv = BCM_E_FAIL;
                        }
                    }
                }
            }

            for (j = 0; j < error_reg_num; j++) {
                if (SOC_REG_IS_VALID(unit, error_counters[j][column])) {
                    SOC_IF_ERROR_RETURN(
                        soc_reg_get(unit, error_counters[j][column], port, 0, &rdata));
                    if (!COMPILER_64_IS_ZERO(rdata)) {
                        cli_out("*ERROR: (b) Counter %s "
                                "has a non zero value for port %0d\n",
                                SOC_REG_NAME(unit, error_counters[j][column]), port);
                        rv = BCM_E_FAIL;
                    }
                }
            }
        }
    }

    if (rv != BCM_E_NONE) {
        cli_out("\n********* COUNTER CHECK FAILED *********\n");
    } else {
        cli_out("\n********* COUNTER CHECK PASSED *********\n");
    }
    return rv;
}


/*
 * Function:
 *      compare_rates
 * Purpose:
 *      Helper funtion to compare actual and expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      tolerance_lr - Rate tolerance allowed for LR ports
 *      tolerance_os - Rate tolerance allowed for OVS ports
 *      port_rate_exp - expected port rate array
 *      port_rate_act - actual port rate array
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
compare_rates(int unit, pbmp_t pbmp, uint32 tolerance_lr, uint32 tolerance_os,
              uint64 *port_rate_exp, uint64 *port_rate_act)
{
    int cnt = 0, port, port_speed, encap;
    uint32 tolerance;
    uint64 min_rate, max_rate, hundred_64, margin_of_error;
    char exp_rate_str[32], act_rate_str[32],
         max_rate_str[32], min_rate_str[32];
    bcm_error_t rv = BCM_E_NONE;

    COMPILER_64_SET(hundred_64, 0, 100);

    /* check actual rate against expected rate under tolerant range */
    LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\n%4s %8s %8s %8s %8s %8s %8s \n"),
                        "idx", "Dev_Port", "Speed", "Exp_Rate",
                        "Min_Rate", "Max_Rate", "Act_Rate"));

    PBMP_ITER(pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            bcm_port_speed_get(unit, port, &port_speed);
            encap = (IS_HG_PORT(unit, port) | IS_HG2_ENABLED_PORT(unit, port)) ?
                         BCM_PORT_ENCAP_HIGIG2 : BCM_PORT_ENCAP_IEEE;
            if (!SOC_IS_TOMAHAWKX(unit)) {
                port_speed = stream_get_exact_speed(port_speed, encap);
            }

            /* margin_of_error */
            if (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port)) {
                tolerance = tolerance_os;
            } else {
                tolerance = tolerance_lr;
            }

            /* adjust speed tolerance */
            if (SOC_IS_TRIDENT3X(unit)) {
                /* plus 1% extra tolerance for 10G higig */
                if ((encap == BCM_PORT_ENCAP_HIGIG2) &&
                    (port_speed <= 11000 && port_speed >= 10000)) {
                    tolerance +=1;
                }
            }

            /* max_rate */
            COMPILER_64_SET(max_rate, 0, port_speed);
            COMPILER_64_SET(margin_of_error, 0, port_speed);
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);
            COMPILER_64_ADD_64(max_rate, margin_of_error);

            /* min_rate */
            COMPILER_64_SET(margin_of_error,
                            COMPILER_64_HI(port_rate_exp[port]),
                            COMPILER_64_LO(port_rate_exp[port]));
            COMPILER_64_UMUL_32(margin_of_error, tolerance);
            COMPILER_64_UDIV_64(margin_of_error, hundred_64);
            COMPILER_64_DELTA(min_rate, margin_of_error, port_rate_exp[port]);

            /* printout */
            format_uint64_decimal(min_rate_str, min_rate, ',');
            format_uint64_decimal(max_rate_str, max_rate, ',');
            format_uint64_decimal(exp_rate_str, port_rate_exp[port], ',');
            format_uint64_decimal(act_rate_str, port_rate_act[port], ',');
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%4d %8d %7dG %7sM %7sM %7sM %7sM "),
                                 cnt, port, port_speed/1000,
                                 exp_rate_str, min_rate_str,
                                 max_rate_str, act_rate_str));

            /* check */
            if (COMPILER_64_LT(port_rate_act[port], min_rate)) {
                rv = BCM_E_FAIL;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, " %s"), "*FAIL"));
            } else if (COMPILER_64_GT(port_rate_act[port], max_rate)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, " %s"), "*WARNING"));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));

            cnt++;
        }
    }
    return rv;
}

/*
 * Function:
 *      stream_chk_port_rate_new
 * Purpose:
 *      Check rates against expected rates.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pbmp - Device port bitmap to be checked
 *      rate_calc - ptr to rate struct, containing all necessary
 *                 parameters needed for port rate check.
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_chk_port_rate(int unit, pbmp_t pbmp, stream_rate_t *rate_calc)
{
    uint32 mode, interval_len, jitter_lr, jitter_os;
    uint64 *port_rate_exp, *port_rate_act;
    bcm_error_t rv = BCM_E_NONE;

    cli_out("\n==================================================\n");
    cli_out("Checking Port Rate ...\n\n");
    /* initialize */
    mode = rate_calc->mode;
    interval_len = rate_calc->interval_len;
    jitter_lr = rate_calc->tolerance_lr;
    jitter_os = rate_calc->tolerance_os;

    port_rate_exp = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                         "port_rate_exp");
    port_rate_act = (uint64 *) sal_alloc(SOC_MAX_NUM_PORTS * sizeof(uint64),
                                         "port_rate_act");
    if (port_rate_exp == NULL || port_rate_act == NULL) {
        sal_free(port_rate_exp);
        sal_free(port_rate_act);
        test_error(unit, "Failed to allocate memory for rate check\n");
        return BCM_E_FAIL;
    }

    /* calculate actual rate */
    stream_calc_act_port_rate(unit, rate_calc->emulation_param, interval_len,
                              port_rate_act);

    /* calculate expected rate */
    if (mode == 1) {
        /* mode -> 1, calculate expected rate by tx_rate */
        stream_calc_exp_rate_by_tx(unit, pbmp, port_rate_exp,
                                        port_rate_act, rate_calc);
    } else {
        /* mode -> 0, calculate expected rate by rx config rate */
        stream_calc_exp_rate_by_rx(unit, port_rate_exp, rate_calc);
    }


    rv = compare_rates(unit, rate_calc->pbmp, jitter_lr, jitter_os, port_rate_exp, port_rate_act);
    if (rv != BCM_E_NONE) {
        cli_out("\n********** RATE CHECK FAILED ***********\n");
    } else {
        cli_out("\n********** RATE CHECK PASSED ***********\n");
    }

    sal_free(port_rate_exp);
    sal_free(port_rate_act);
    return rv;
}

/*
 * Function:
 *      stream_chk_pkt_integrity
 * Purpose:
 *      Redirect all packets back to CPU and check packet integrity
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      pkt_intg - ptr to packet integrity struct, containing all necessary
 *                 parameters needed for integrity check.
 *
 * Returns:
 *     SOC_E_XXXX
 */
bcm_error_t
stream_chk_pkt_integrity(int unit, stream_integrity_t *pkt_intg)
{
    bcm_error_t rv = BCM_E_NONE;
    uint8 *rx_pkt, *ref_pkt;
    int port, port_speed, j, k, chk_fail = 0, stream_cnt = 0;
    int rst_vlan;
    dv_t *dv_rx;
    uint32 vlan;
    uint32 pkt_size;
    uint32 port_exp_pkt_cnt, port_act_pkt_cnt, pkt_cnt_sum = 0;
    uint32 header_size = 0;
    uint32 ts1, ts2, latency, max = 0, min = -1, sum = 0, average;

    cli_out("\n==================================================\n");
    cli_out("Checking Packet Integrity ...\n\n");
    /* initialize */
    soc_dma_header_size_get(unit, &header_size);
    dv_rx   = soc_dma_dv_alloc(unit, DV_RX, 3);
    ref_pkt = sal_dma_alloc(MTU * sizeof(uint8), "ref_packet");
    rx_pkt  = sal_dma_alloc(MTU * sizeof(uint8) + header_size, "rx_pkt");
    if (dv_rx == NULL || ref_pkt == NULL || rx_pkt == NULL) {
        soc_dma_dv_free(unit, dv_rx);
        sal_dma_free(ref_pkt);
        sal_dma_free(rx_pkt);
        test_error(unit, "Failed to allocate memory for integrity check\n");
        return BCM_E_FAIL;
    }

    /* config all channels as RX */
    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 1, 0xffffffff);
    soc_dma_chan_cos_ctrl_set(unit, RX_CHAN, 2, 0xffffffff);

    /* config unmatched VLAN packet to be forwarded to CPU */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CPU_CONTROL_0r, 0,
                                               UVLAN_TOCPUf, 0x1));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CPU_CONTROL_1r, 0,
                                               L3_MTU_FAIL_TOCPUf, 0x1));

    /* check packet integrity for each port */
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "%4s %4s %4s %8s %8s\n"),
                         "idx", "port", "spd", "tx_pkt", "rx_pkt"));
    PBMP_ITER(pkt_intg->rx_pbmp, port) {
        if (port < SOC_MAX_NUM_PORTS) {
            /* check integrity of each packet */
            port_exp_pkt_cnt = pkt_intg->port_flood_cnt[port];
            port_act_pkt_cnt = 0;
            vlan = pkt_intg->rx_vlan[port];
            for (j = 0; j < port_exp_pkt_cnt; j++) {
                rst_vlan = 0;
                if (j == 0) {
                    if (pkt_intg->type == PKT_TYPE_L3UC) {
                        rst_vlan = 2; /* L3 packet*/
                    } else {
                        rst_vlan = 1; /* L2 packet*/
                    }
                }
                /* receive one packet from CPU though rx_pkt_dma */
                if (stream_pktdma_rx(unit, rst_vlan, vlan, dv_rx, rx_pkt,
                                     MTU + header_size) != BCM_E_NONE) {
                    /*
                    cli_out("*ERROR, failed to receive pkt from CPU"
                            "for integrity check, port %d\n",
                            port);
                    chk_fail = 1; */
                    break;
                }
                port_act_pkt_cnt ++;

                /* generate reference packet */
                if (stream_gen_ref_pkt(unit, port, pkt_intg, rx_pkt, ref_pkt,
                                       &pkt_size) != BCM_E_NONE) {
                    cli_out("*ERROR, failed to generate ref_pkt "
                            "for integrity check, port %d\n",
                            port);
                    chk_fail = 1;
                    break;
                }

                /* check packet content (payload) */
                for (k = 0; k < (pkt_size - NUM_BYTES_CRC); k++) {
                    if (rx_pkt[k + header_size] != ref_pkt[k]) {
                        cli_out("*ERROR: Packet corruption, "
                                "Stream %d, Port %d, Packet %d\n",
                                stream_cnt, port, j);
                        cli_out("-- Expected Packet:");
                        tgp_print_pkt(ref_pkt, pkt_size);
                        cli_out("-- Received Packet:");
                        tgp_print_pkt(rx_pkt, pkt_size+header_size);
                        chk_fail = 1;
                        break;
                    }
                }

                /* latency calculation */
                if (pkt_intg->type == PKT_TYPE_L2_TS) {
                    ts1 = (rx_pkt[pkt_size+header_size+2] << 24) + (rx_pkt[pkt_size+header_size+3] << 16) +
                          (rx_pkt[pkt_size+header_size+4] << 8) + rx_pkt[pkt_size+header_size+5];
                    ts2 = (rx_pkt[pkt_size+header_size+16] << 24) + (rx_pkt[pkt_size+header_size+17] << 16) +
                          (rx_pkt[pkt_size+header_size+18] << 8) + rx_pkt[pkt_size+header_size+19];
                    latency = ts2 - ts1;
                    if (latency < min) {
                        min = latency;
                    }
                    if (latency > max) {
                        max = latency;
                    }
                    sum += latency;
                    LOG_INFO(BSL_LS_APPL_TESTS,
                             (BSL_META_U(unit, "-- Packet %0d TS2 %0x TS1 %0x Latency %0d\n"),
                                         j, ts2, ts1, latency));
                }
            }

            /* disable forwarding pkt to cpu for current port */
            if (pkt_intg->type == PKT_TYPE_L3UC) {
                stream_set_l3mtu_valid(unit, vlan);
            } else {
                stream_set_vlan_valid(unit, vlan);
            }

            /* check packet count */
            bcm_port_speed_get(unit, port, &port_speed);
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit, "%4d %4d %3dG %8d %8d "),
                                 stream_cnt, port, port_speed/1000,
                                 port_exp_pkt_cnt, port_act_pkt_cnt));
            if (port_act_pkt_cnt == 0) {
                chk_fail = 1;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "*WARNING")));
            } else if (port_act_pkt_cnt < port_exp_pkt_cnt &&
                !(SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))) {
                chk_fail = 1;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit, "*WARNING")));
            }
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));

            /* print latency */
            if (pkt_intg->type == PKT_TYPE_L2_TS) {
                average = _shr_div_exp10(sum, port_act_pkt_cnt, 2);
                cli_out("Latency: max %0d ns, min %0d ns, average %0d.%02d ns\n",
                        max, min, average / 100, average % 100);
            }

            pkt_cnt_sum += port_act_pkt_cnt;
            stream_cnt++;
        }
    }

    cli_out("Total packets received: %d\n", pkt_cnt_sum);
    if (chk_fail == 0) {
        cli_out("\n**** PACKET INTEGRITY CHECK PASSED *****\n");
    } else {
        cli_out("\n**** PACKET INTEGRITY CHECK FAILED *****\n");
        rv = BCM_E_FAIL;
    }

    soc_dma_abort_dv(unit, dv_rx);
    soc_dma_dv_free(unit, dv_rx);
    sal_dma_free(rx_pkt);
    sal_dma_free(ref_pkt);

    return rv;
}


int
stream_get_exact_speed(int speed, int encap) {
    if(encap == BCM_PORT_ENCAP_IEEE) return speed;
    else {
        switch (speed) {
            case 120000: speed = 127000; break;
            case 100000: speed = 106000; break;
            case 50000:  speed = 53000;  break;
            case 40000:  speed = 42000;  break;
            case 25000:  speed = 27000;  break;
            case 20000:  speed = 21000;  break;
            case 10000:  speed = 11000;  break;
            default: break;
        }
        return speed;
    }
}

/*
 * Function:
 *      start_cmic_timesync
 * Purpose:
 *      Enable CMIC timesync module to run NS timestamp
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *
 * Returns:
 *     Nothing
 */
void start_cmic_timesync(int unit)
{
    uint32 rval;

    /* Enable Timestamp Generation */
    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &rval);
    /* 250 Mhz - 4.000 ns */
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &rval, FRACf,
                      0x40000000);
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, rval);

    READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &rval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &rval,
                      TIME_CAPTURE_ENABLEf, 1);
    WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, rval);

    READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &rval,
                      ENABLEf, 1);
    WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval);
}

uint64 get_ts_count(int unit)
{
    uint64 ts, div_64;
    uint32 rval, valid, ts_lower, ts_upper;

    if (soc_feature(unit, soc_feature_cmicx)) {
        /*soc_iproc_getreg(unit,
              soc_reg_addr(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr,
              REG_PORT_ANY, 0), &rval);
        cli_out("\n ts capture %0x", rval);*/
        rval = 0;
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &rval,
                          TIME_CAPTURE_MODEf, 1);
        /*soc_iproc_setreg(unit,
              soc_reg_addr(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr,
              REG_PORT_ANY, 0), rval);*/
        soc_pci_write(unit, 0x35024, rval);

        rval = soc_pci_read(unit, 0x3502c);
        /*soc_iproc_getreg(unit,
              soc_reg_addr(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr,
              REG_PORT_ANY, 0), &rval);
        cli_out("\n ts lower %0x", rval);*/
        ts_lower = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr,
                              rval, TIMESTAMPf);
        rval = soc_pci_read(unit, 0x35030);
        /*soc_iproc_getreg(unit,
              soc_reg_addr(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
              REG_PORT_ANY, 0), &rval);
        cli_out("\n ts upper %0x", rval);*/
        valid = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                  rval, VALIDf);
        ts_upper = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                  rval, TIMESTAMPf);
    } else {
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &rval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &rval,
                          TIME_CAPTURE_MODEf, 1);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, rval);
        READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &rval);
        ts_lower = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr,
                              rval, TIMESTAMPf);
        READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &rval);
        valid = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                  rval, VALIDf);
        ts_upper = soc_reg_field_get(unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                                  rval, TIMESTAMPf);
    }

    COMPILER_64_SET(div_64, 0, 1000);
    COMPILER_64_ZERO(ts);
    if(valid) {
            COMPILER_64_SET(ts, ts_upper, ts_lower);
            COMPILER_64_UDIV_64(ts, div_64);
    }
    return ts;
}

uint64 get_cur_time(int unit, uint32 emulation_param)
{
    uint64 time;

    if(emulation_param) time = get_ts_count(unit);
    else COMPILER_64_SET(time, 0, sal_time_usecs());

    return time;
}

#endif /* BCM_ESW_SUPPORT */
