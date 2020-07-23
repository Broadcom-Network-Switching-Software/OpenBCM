/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Loopback Tests
 *
 *      DMA     Sends packets from CPU to MMU, and back
 *      S/G     Sends packets from CPU to MMU, and back, using multiple
 *              descriptors per packet
 *      MAC     Sends packets from CPU to MAC in loopback mode, and back
 *      MII     Sends packets from CPU to PHY in loopback mode, and back
 *      EXT     Sends packets from CPU out one port, in another, and back
 *      SNAKE   Sends packets from CPU through a series of ports, and back
 *
 * In Continuous DMA mode we append a reload descriptor to the end of the
 * chain and enable a new controlled descriptor interrupt to manage the
 * DMA state. To operate in this mode, two descriptor chains are used rather
 * than one and DMA readies one while the other is in use and moves into it
 * once the demand is there. In this mode DMA never stops. It only halts
 * on the reload descriptor if the next chain is not ready (RX) or there is
 * no packet to send (TX).
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <appl/diag/system.h>
#include <appl/test/loopback2.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/hash.h>

#include <soc/dma.h>
#include <soc/higig.h>
#include <soc/pbsmh.h>
#include <soc/counter.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/firebolt.h>
#include <bcm/init.h>
#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/stack.h>
#include <bcm/cosq.h>
#include <bcm/stat.h>

#if defined(BCM_CMICM_SUPPORT)
#include <soc/cmicm.h>
#endif

#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/mbcm.h>
#endif /* BCM_ESW_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#if defined (BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#else
#define SOC_DPP_PP_ENABLE(unit) (0)
#endif

#include <appl/test/loopback.h>
#include <appl/test/lb_util.h>

#include "testlist.h"

#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT) 
#include <appl/diag/dcmn/counter.h>
#endif

#ifdef INCLUDE_KNET
#include <kcom.h>
#endif


#define LB_LAST_DCB(unit, dcb, cnt) SOC_DCB_IDX2PTR(unit, dcb, cnt-1)
#define LB_LAST_PKT_DCB(unit, dcb, cnt) SOC_DCB_IDX2PTR(unit, dcb, cnt-2)

#if defined (BCM_ESW_SUPPORT) ||  defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
#undef LB_PBM_TEST                      /* Test not yet validated */

/* sal_mac_addr_t lb_mac_default = {0x00, 0x00, 0x00, 0xde, 0xad, 0x00}; */
sal_mac_addr_t lb_mac_src = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
sal_mac_addr_t lb_mac_dst = {0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb};

const char *lb_parse_speed[] = {
    LB2_SPEED_INIT_STR,
    NULL
};
#define CHECK_PBM_SPEED(x, y) ((x) & (y))
/*
 * CRC mode handling: Defines MUST MATCH BELOW order
 */

#define CRC_MODE_CPU_NONE               0
#define CRC_MODE_CPU_APPEND             1
#define CRC_MODE_MAC_REGEN              2

STATIC uint32 lb_saved_srcmod[SOC_MAX_NUM_DEVICES];
STATIC uint32 lb_saved_srcport[SOC_MAX_NUM_DEVICES];
#ifdef BCM_XGS3_SWITCH_SUPPORT
static bcm_l2_cache_addr_t lb_saved_l2[SOC_MAX_NUM_DEVICES];
static int l2_index = -1;
#endif

#ifdef BCM_SHADOW_SUPPORT
static int lb_setup_static_path(loopback_test_t *lw, int unit, soc_port_t port,
                     soc_port_t src_port);
#endif

STATIC
char *crc_mode[] = { "CpuNone", "CpuAppend", "MacRegen", NULL };

STATIC INLINE int crc_mode_to_dmac(int m)
{
    if (m == CRC_MODE_MAC_REGEN) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Loopback work structure
 */

STATIC loopback_test_t lb_work[SOC_MAX_NUM_DEVICES];

loopback_test_t*
lb_get_test(int unit)
{
    return &lb_work[unit];
}

loopback_testdata_t*
lb_get_dma_testdata(int unit)
{
    return &lb_get_test(unit)->lw_lp_dma;
}

int
lb_is_xgs_fabric(int unit)
{
    /* True fabric device */
    if (SOC_IS_XGS_FABRIC(unit)&& !SOC_IS_TOMAHAWK2(unit)) {
        return 1;
    }
    /* Ethernet ports disabled by config or bond-option */
    if (SOC_PBMP_IS_NULL(PBMP_E_ALL(unit))) {
        return 1;
    }
    return 0;
}

STATIC  uint32
lb_timeout_usec(int unit)
{
    uint32              defl;

    if (SAL_BOOT_QUICKTURN) {
        defl = LB_PACKET_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
        defl = LB_PACKET_TIMEOUT_PLI;
    } else {
        defl = LB_PACKET_TIMEOUT;
    }

    return (soc_property_get(unit, spn_DIAG_LB_PACKET_TIMEOUT, defl) *
            SECOND_USEC);
}

STATIC int
lb_link_wait(int unit, pbmp_t pbm)
/*
 * Function:    lb_link_wait
 * Purpose:     Wait for link up status on the spedified ports.
 * Parameters:  unit - StrataSwitch unit #.
 *              pbm  - port bit map of ports to wait for.
 * Returns:     0 - Success
 *              -1 - time-out waiting for ports.
 */
{
    int         rv;
    pbmp_t      link_pbm, tpbm;

    BCM_PBMP_ASSIGN(link_pbm, pbm);
    if ((rv = bcm_link_wait(unit, &link_pbm, LB_LINK_TIMEOUT))) {
        char    b[FORMAT_PBMP_MAX];

        BCM_PBMP_ASSIGN(tpbm, pbm);
        BCM_PBMP_REMOVE(tpbm, link_pbm);
        (void)format_pbmp(unit, b, sizeof(b), tpbm);
        test_error(unit, "Failed to get Link up indication: %s\n", b);
        return(-1);
    }

    return(0);
}

/*
 * Function:    lb_ppce_initval
 * Purpose:     Calculates values that are used to keep the number of packets per 
                chain within the limits that the MMU COS settings can handle.
 * Parameters:  unit - unit to be used at a test
 *              len -  actual len of a packet
 *              ppce - output for packets per chain end
 * Returns:     SOC_E_XXX
*/
static int 
lb_ppce_initval(int unit, int *lene, int *ppce)
{
    int max_cells, num_cells, num_ports, num_cos, mmu_cell_size;
    int len;
    int ppce_max = SOC_DV_PKTS_MAX; 
    

    *ppce = ppce_max;
    len = *lene;

    if (SOC_IS_SHADOW(unit)) {
        return SOC_E_NONE;
    }

    if (SOC_IS_XGS_SWITCH(unit) || SOC_IS_XGS12_FABRIC(unit)) {

        /* Check that we don't overload the MMU HOL cell limits */
        /*  (This should be from #defines somewhere else)  */
        mmu_cell_size = 128;
        BCM_IF_ERROR_RETURN(bcm_cosq_config_get(unit, &num_cos));
        max_cells = (len + mmu_cell_size - 1) / mmu_cell_size;
#ifndef _KATANA_DEBUG
		if (SOC_IS_KATANAX(unit)) {
			num_cells = 8192;
		} else 
#endif
#if defined(BCM_ESW_SUPPORT)
        if (SOC_IS_FBX(unit)) {
            /* MMU_CBPDATA0 does not exist in TH3, so calc total cells */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                num_cells = _TH3_MMU_TOTAL_CELLS_PER_ITM;
            } else 
#endif
            {
                num_cells = soc_mem_index_count(unit, MMU_CBPDATA0m);
            }
        } else 
#endif
        {
            num_cells = 8192;
        }
        num_ports = NUM_ALL_PORT(unit);
        *ppce = (num_cells / (num_cos * num_ports * max_cells)) - 1;

#if defined(BCM_HERCULES_SUPPORT)
        /* For hercules, look at what is configured for any port/cos 0,0 */
        if (SOC_IS_HERCULES15(unit)) {
            int cos = 0;
            int port = 0;
            uint32 num_cells_per_port_cos = 0;

            SOC_IF_ERROR_RETURN
                (READ_MMU_CELLLMTCOS_LOWERr(unit, port, cos,
                                            &num_cells_per_port_cos));
            /* overwrite ppce */
            *ppce = (num_cells_per_port_cos / max_cells) - 1; 
        }
#endif /* BCM_HERCULES_SUPPORT */

        if (*ppce < 1) {
            *ppce = 1;
        }
        if (*ppce > ppce_max) {
            *ppce = ppce_max;
        }
        /*
         * Number of cells allocated to this port
         * Assume DYNCELL_PERCENT 50
         */
        len = num_cells / num_ports / 2;

        /*
         * Number of cells usable by a cos queue
         */
        len += (len / num_cos);
        /*
         * Max packet size supported
         */
        len *= mmu_cell_size;
        if (*lene > len) {
            *lene = len;
        }
    }
    return(SOC_E_NONE);
}

STATIC  void
lb_setup(int unit, loopback_test_t *lw)
/*
 * Function:    lb_setup
 * Purpose:     Fill in defaults for the parameters.
 * Parameters:  unit - unit #.
 *              lw - pointer to work struct.
 * Returns:     Nothing
 */
{
    loopback_testdata_t *lp;
    int                 p, setport;
    int                 num_cos;
    int                 lene_ignore = 0;

    if (bcm_cosq_config_get(unit, &num_cos) < 0 || num_cos == 0) {
        num_cos = 1;
    }

    /* Keep statistics from printing if user ^C's before test starts */
    lw->lw_stats_init = FALSE;

    if (lw->lw_set_up) {
        return;
    }

    lw->lw_set_up = soc_property_get(unit, "lw_set_up_reinit_skip", TRUE);
    lw->lw_unit = unit;

    lw->lw_mac_src_inc = 0;
    lw->lw_mac_dst_inc = 0;

    /* Check config for DMA Mode */
    lw->lw_dma_mode = LB_LEGACY_DMA_MODE;
#if defined(BCM_CMICDV2_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    if (soc_feature(unit, soc_feature_continuous_dma) &&
        soc_property_get(unit, spn_PDMA_CONTINUOUS_MODE_ENABLE, 0)) {
        lw->lw_dma_mode = LB_CONTINUOUS_DMA_MODE;
        cli_out("Using Continuous DMA Mode\n");
    }
#endif

    /* CPU */

    lp = &lw->lw_lp_dma;
    SOC_PBMP_CLEAR(lp->lp_pbm);
    SOC_PBMP_PORT_ADD(lp->lp_pbm, CMIC_PORT(unit));

    lp->lp_dst_inc      = 0;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 1;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_l_start      = 64;
    lp->lp_l_end        = 2048 + 64;
    lp->lp_l_inc        = 256;
    lp->lp_c_start      = 1;
    lp->lp_c_end        = 16;
    lp->lp_c_inc        = 5;
    lp->lp_dv_start     = 1;
    lp->lp_dv_end       = 1;
    lp->lp_dv_inc       = 1;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = num_cos - 1;
    lp->lp_count        = 10;
    lp->lp_crc_mode     = CRC_MODE_CPU_NONE;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;
 
    /* MAC */

    lp = &lw->lw_lp_mac;

    SOC_PBMP_ASSIGN(lp->lp_pbm, PBMP_PORT_ALL(unit));
    lp->lp_dst_inc      = 0;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 0;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_d_port       = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->lp_d_mod) < 0) {
            lp->lp_d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->lp_d_mod    = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->lp_d_mod    = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->lp_opcode       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->lp_opcode       = 0;
#endif
    lp->lp_l_start      = 68;
    lp->lp_l_end        = 1522;
    lp->lp_l_inc        = 128;
    lp->lp_c_start      = 1;
    lene_ignore = lp->lp_l_end;
    (void)lb_ppce_initval(unit, &lene_ignore, &lp->lp_c_end);
    lp->lp_c_inc        = 1;
    lp->lp_dv_start     = 1;
    lp->lp_dv_end       = LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode) ? 2 : 1;
    lp->lp_dv_inc       = 1;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = num_cos - 1;
    lp->lp_count        = 10;
    lp->lp_crc_mode     = CRC_MODE_MAC_REGEN;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;

    ENET_SET_MACADDR(lp->lp_mac_src, lb_mac_src);
    lp->lp_mac_src_inc = 0;
    ENET_SET_MACADDR(lp->lp_mac_dst, lb_mac_dst);
    lp->lp_mac_dst_inc = 0;

    /* PHY */

    lp = &lw->lw_lp_phy;

    SOC_PBMP_ASSIGN(lp->lp_pbm, PBMP_PORT_ALL(unit));
    lp->lp_dst_inc      = 0;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 0;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_d_port       = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->lp_d_mod) < 0) {
            lp->lp_d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->lp_d_mod    = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->lp_d_mod    = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->lp_opcode       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->lp_opcode       = 0;
#endif
    lp->lp_l_start      = 68;
    lp->lp_l_end        = 1522;
    lp->lp_l_inc        = 128;
    lp->lp_c_start      = 1;
    lene_ignore = lp->lp_l_end;
    (void)lb_ppce_initval(unit, &lene_ignore, &lp->lp_c_end);
    lp->lp_c_inc        = 1;
    lp->lp_dv_start     = 1;
    lp->lp_dv_end       = LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode) ? 2 : 1;
    lp->lp_dv_inc       = 1;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = num_cos - 1;
    lp->lp_count        = 10;
    lp->lp_crc_mode     = CRC_MODE_MAC_REGEN;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;

    ENET_SET_MACADDR(lp->lp_mac_src, lb_mac_src);
    lp->lp_mac_src_inc = 0;
    ENET_SET_MACADDR(lp->lp_mac_dst, lb_mac_dst);
    lp->lp_mac_dst_inc = 0;

    /* EXT */

    lp = &lw->lw_lp_ext;

    SOC_PBMP_CLEAR(lp->lp_pbm);
    /* set up pairs of transmitting and receiving ports */
    SOC_PBMP_CLEAR(lp->lp_pbm_tx);
    setport = 1;
    PBMP_PORT_ITER(unit, p) {
        if (setport) {
            SOC_PBMP_PORT_ADD(lp->lp_pbm_tx, p);
        }
        setport = !setport;
    }
    lp->lp_dst_inc      = 1;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 0;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_d_port       = CMIC_PORT(unit);
    if (SOC_IS_XGS_SWITCH(unit)) {
        if (bcm_stk_my_modid_get(unit, &lp->lp_d_mod) < 0) {
            lp->lp_d_mod = 0;
        }
    } else {
#ifdef BCM_XGS_SUPPORT
        lp->lp_d_mod    = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_DST_MOD_CPU : 0;
#else
        lp->lp_d_mod    = 0;
#endif
    }
#ifdef BCM_XGS_SUPPORT
    lp->lp_opcode       = SOC_IS_XGS12_FABRIC(unit) ? SOC_HIGIG_OP_UC : 0;
#else
    lp->lp_opcode       = 0;
#endif
    lp->lp_l_start      = 68;
    lp->lp_l_end        = 1522;
    lp->lp_l_inc        = 128;
    lp->lp_c_start      = 1; 
    lene_ignore = lp->lp_l_end;
    (void)lb_ppce_initval(unit, &lene_ignore, &lp->lp_c_end);
    lp->lp_c_inc        = 1;
    lp->lp_dv_start     = 1;
    lp->lp_dv_end       = 1;
    lp->lp_dv_inc       = 1;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = num_cos - 1;
    lp->lp_count        = 10;
    lp->lp_crc_mode     = CRC_MODE_MAC_REGEN;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;
    lp->lp_autoneg      = TRUE;

    ENET_SET_MACADDR(lp->lp_mac_src, lb_mac_src);
    lp->lp_mac_src_inc = 0;
    ENET_SET_MACADDR(lp->lp_mac_dst, lb_mac_dst);
    lp->lp_mac_dst_inc = 0;

    /* S/G DMA */
    lp = &lw->lw_lp_sg_dma;

    SOC_PBMP_ASSIGN(lp->lp_pbm, PBMP_PORT_ALL(unit));
    lp->lp_dst_inc      = 1;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 1;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_l_start      = 64;
    lp->lp_l_end        = 1024 + 64;
    lp->lp_l_inc        = 128;
    lp->lp_ppc_start    = 1;
    lene_ignore = lp->lp_l_end;
    (void)lb_ppce_initval(unit, &lene_ignore, &lp->lp_ppc_end);
    lp->lp_ppc_inc      = 2;
    lp->lp_dpp_start    = 1;
    lp->lp_dpp_end      = 13;
    lp->lp_dpp_inc      = 3;
    lp->lp_c_start      = 2;
    lp->lp_c_end        = 8;
    lp->lp_c_inc        = 2;
    lp->lp_dv_start     = 2;
    lp->lp_dv_end       = 8;
    lp->lp_dv_inc       = 3;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = 0;
    lp->lp_count        = 1;
    lp->lp_crc_mode     = CRC_MODE_CPU_NONE;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;
    lp->lp_seed         = (2 << 24) - 1;
    ENET_SET_MACADDR(lp->lp_mac_src, lb_mac_src);
    ENET_SET_MACADDR(lp->lp_mac_dst, lb_mac_dst);

    /* S/G RANDOM */
    lp = &lw->lw_lp_sg_random;

    SOC_PBMP_ASSIGN(lp->lp_pbm, PBMP_PORT_ALL(unit));
    lp->lp_dst_inc      = 1;
    SOC_PBMP_CLEAR(lp->lp_ubm);
    lp->lp_pattern      = 0xa5a4a3a2;
    lp->lp_pattern_inc  = 1;
    lp->lp_vlan         = LB_VLAN_ID;
    lp->lp_l_start      = 64;
    lp->lp_l_end        = 1024 + 64;
    lp->lp_l_inc        = 128;
    lp->lp_ppc_start    = 1;
    lene_ignore = lp->lp_l_end;
    (void)lb_ppce_initval(unit, &lene_ignore, &lp->lp_ppc_end);
    lp->lp_ppc_inc      = 2;
    lp->lp_dpp_start    = 1;
    lp->lp_dpp_end      = 13;
    lp->lp_dpp_inc      = 3;
    lp->lp_c_start      = 2;
    lp->lp_c_end        = 8;
    lp->lp_c_inc        = 2;
    lp->lp_dv_start     = 2;
    lp->lp_dv_end       = 8;
    lp->lp_dv_inc       = 3;
    lp->lp_cos_start    = 0;
    lp->lp_cos_end      = 0;
    lp->lp_count        = 5;
    lp->lp_crc_mode     = CRC_MODE_CPU_NONE;
    lp->lp_check_data   = TRUE;
    lp->lp_check_crc    = FALSE;
    lp->lp_seed         = (2 << 24) - 1;
    ENET_SET_MACADDR(lp->lp_mac_src, lb_mac_src);
    ENET_SET_MACADDR(lp->lp_mac_dst, lb_mac_dst);


}

STATIC int
lb_unused_10g_ports_enable_set(int unit, soc_port_t rx_port, int enable)
{
    int rv = 0;

    /* Disable all but rx_port */
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int port;
        PBMP_HG_ITER(unit, port) {
            if (port != rx_port) {
                if ((rv = bcm_port_enable_set(unit, port, enable)) < 0) {
                    test_error(unit,
                            "Port %s: Failed to set MAC %s: %s\n",
                            SOC_PORT_NAME(unit, port),
                            (enable ? "Enable" : "Disable"), bcm_errmsg(rv));
                    return(-1);
                }
            }
        }
        PBMP_XE_ITER(unit, port) {
            if (port != rx_port) {
                if ((rv = bcm_port_enable_set(unit, port, enable)) < 0) {
                    test_error(unit,
                            "Port %s: Failed to set MAC %s: %s\n",
                            SOC_PORT_NAME(unit, port),
                            (enable ? "Enable" : "Disable"), bcm_errmsg(rv));
                    return(-1);
                }
            }
        }
    }
    return(0);
}

STATIC int
lb_save_port(loopback_test_t *lw)
{
    int         rv = 0;
    soc_port_t  port;
    pbmp_t      save_ports;

    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        /* Save and restore all ports -- see lb_mac_init */
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(lw->lw_unit));
    } else {
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(lw->lw_unit));
        SOC_PBMP_AND(save_ports, lw->lw_lp->lp_pbm);
    }
    
    if (SOC_IS_SHADOW(lw->lw_unit)) {
        return SOC_E_NONE;
    }
    PBMP_ITER(save_ports, port) {
        if ((rv = bcm_port_info_save(lw->lw_unit, port,
                                     &lw->lw_save_port[port])) < 0) {
            test_error(lw->lw_unit,
                       "Port %s: Could not get port info: %s\n",
                       SOC_PORT_NAME(lw->lw_unit, port), bcm_errmsg(rv));
            return(-1);
        }
    }

    /* Module settings */
    if (SOC_IS_XGS_SWITCH(lw->lw_unit)) {
        int     not_my_mod;
        /* Save current source port/module and change to != 
           lw->lw_lp->lp_d_mod */

        lb_saved_srcmod[lw->lw_unit] = SOC_DEFAULT_DMA_SRCMOD_GET(lw->lw_unit);
        lb_saved_srcport[lw->lw_unit] =
            SOC_DEFAULT_DMA_SRCPORT_GET(lw->lw_unit);
        not_my_mod = (lw->lw_lp->lp_d_mod == 0);
        SOC_DEFAULT_DMA_SRCMOD_SET(lw->lw_unit, not_my_mod);
        SOC_DEFAULT_DMA_SRCPORT_SET(lw->lw_unit, CMIC_PORT(lw->lw_unit));
        /* Program these values into CMIC_HGTX_CTRL* register */
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(lw->lw_unit)) {
            /*
             * Should save (not yet done here) ICONTROL_OPCODE_BITMAP before
             * the test and restore it after test is done
             * Redirect all packets with CPU control opcode from HG ports to CPU.
             */
            int pbm_len;
            uint32 cpu_pbm;

            if (SOC_IS_TR_VL(lw->lw_unit) && !(SOC_IS_TRIUMPH2(lw->lw_unit) 
                || SOC_IS_APOLLO(lw->lw_unit) || SOC_IS_VALKYRIE2(lw->lw_unit))) {
                SOC_IF_ERROR_RETURN(soc_xgs3_port_to_higig_bitmap
                                        (lw->lw_unit, CMIC_PORT(lw->lw_unit),
                                         &cpu_pbm));
            } else if (CMIC_PORT(lw->lw_unit)) {
                pbm_len = soc_reg_field_length(lw->lw_unit,
                                               ICONTROL_OPCODE_BITMAPr,
                                               BITMAPf);
                if (pbm_len == 0) {
                    return (-1);
                }
                cpu_pbm = 1 << (pbm_len - 1);
            } else {
                cpu_pbm = 1;
            }
            PBMP_ST_ITER(lw->lw_unit, port) {
                if (SOC_MEM_IS_VALID(lw->lw_unit, ICONTROL_OPCODE_BITMAPm)) {
                    icontrol_opcode_bitmap_entry_t entry;
                    sal_memset(&entry, 0, sizeof(entry));
                    soc_mem_pbmp_field_set(lw->lw_unit,
                                           ICONTROL_OPCODE_BITMAPm,
                                           &entry, BITMAPf,
                                           &PBMP_CMIC(lw->lw_unit));
                    rv = soc_mem_write(lw->lw_unit, ICONTROL_OPCODE_BITMAPm,
                                       MEM_BLOCK_ANY, port, &entry);
                } else if (SOC_IS_TRIUMPH2(lw->lw_unit) ||
                           SOC_IS_APOLLO(lw->lw_unit) ||
                           SOC_IS_VALKYRIE2(lw->lw_unit)) {
                    uint64 cpu_pbm_64;
                    COMPILER_64_ZERO(cpu_pbm_64);
                    COMPILER_64_SET(cpu_pbm_64, 0, cpu_pbm);
                    rv = WRITE_ICONTROL_OPCODE_BITMAP_64r(lw->lw_unit,
                                                          port, cpu_pbm_64);
                } else if (SOC_REG_IS_VALID(lw->lw_unit, ICONTROL_OPCODE_BITMAP_LOr) &&
                           SOC_REG_IS_VALID(lw->lw_unit, ICONTROL_OPCODE_BITMAP_HIr)) {

                    uint64      fval64, rval64;
                    uint32      t_port;
                    pbmp_t      curr_pbmp;
                    soc_reg_t   reg;

                    BCM_PBMP_CLEAR(curr_pbmp);
                    COMPILER_64_ZERO(fval64);

                    /* Set HG ingress CPU Opcode map to the CPU */
                    if (CMIC_PORT(lw->lw_unit) < 64) {
                        reg = ICONTROL_OPCODE_BITMAP_LOr;
                        t_port = CMIC_PORT(lw->lw_unit);
                    } else {
                        reg = ICONTROL_OPCODE_BITMAP_HIr;
                        t_port = CMIC_PORT(lw->lw_unit) - 64;
                    }

                    SOC_PBMP_PORT_ADD(curr_pbmp, t_port);

                    rv = soc_reg_get(lw->lw_unit, reg, port, 0, &rval64);
                    if (BCM_SUCCESS(rv)) {
                        COMPILER_64_SET(fval64,
                                        SOC_PBMP_WORD_GET(curr_pbmp, 1),
                                        SOC_PBMP_WORD_GET(curr_pbmp, 0));

                        soc_reg64_field_set(lw->lw_unit, 
                                reg, &rval64, BITMAPf, fval64);
                    }
                    rv = soc_reg_set(lw->lw_unit, reg, port, 0, rval64);
                } else {
                    if (SOC_REG_IS_VALID(lw->lw_unit, ICONTROL_OPCODE_BITMAPr)) {
                        rv = WRITE_ICONTROL_OPCODE_BITMAPr(lw->lw_unit,
                                                           port, cpu_pbm);
                    }
                }
                if (rv < 0) {
                    test_error(lw->lw_unit,
                               "Failed to direct HG traffic to CPU %s\n",
                               bcm_errmsg(rv));
                    return(-1);
                }
            }
            if (l2_index >= 0) {
                if ((rv = bcm_l2_cache_get(lw->lw_unit, l2_index,
                                           &lb_saved_l2[lw->lw_unit])) < 0) {
                    test_error(lw->lw_unit,
                               "Failed to save L2 Cache entry %s\n",
                               bcm_errmsg(rv));
                    return(-1);
                }
            }
        } else
#endif
        {
            test_error(lw->lw_unit, "Loopback Error:  Invalid SOC type\n");
            return(-1);
        }
    } 
#ifdef  BCM_XGS12_FABRIC_SUPPORT
	else if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        uint32      ucdata;
        pbmp_t      uc;
        int         blk;
        
        SOC_PBMP_CLEAR(uc);
        SOC_PBMP_PORT_ADD(uc, CMIC_PORT(lw->lw_unit));
        ucdata = SOC_PBMP_WORD_GET(uc, 0);
        PBMP_ITER(save_ports, port) {
            blk = SOC_PORT_BLOCK(lw->lw_unit, port);
#if defined(BCM_ESW_SUPPORT)
			
            if ((rv = soc_mem_write(lw->lw_unit, MEM_UCm, blk,
                                    lw->lw_lp->lp_d_mod, &ucdata)) < 0) {
                test_error(lw->lw_unit,
                           "Could not write MEM_UC table %s\n",
                           bcm_errmsg(rv));
                return(-1);
            }
#endif			
        }
    }
#endif
    return(0);
}

STATIC int
lb_restore_port(loopback_test_t *lw)
{
    int         rv;
    int         ms;
    soc_port_t  port;
    pbmp_t      save_ports;

    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        /* Save and restore all ports -- see lb_mac_init */
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(lw->lw_unit));
    } else {
        SOC_PBMP_ASSIGN(save_ports, PBMP_PORT_ALL(lw->lw_unit));
        SOC_PBMP_AND(save_ports, lw->lw_lp->lp_pbm);
    }

    if (SOC_IS_SHADOW(lw->lw_unit)) {
        return SOC_E_NONE;
    }
    PBMP_ITER(save_ports, port) {
        /* If the Master mode is not changed, remove phy_master attributes from action_mask*/
        ms = 0;
        if ((rv = bcm_port_master_get(lw->lw_unit, port, &ms)) != BCM_E_NONE) {
            test_error(lw->lw_unit,
                       "Port %s: Could not get port Master mode: %s\n",
                       SOC_PORT_NAME(lw->lw_unit, port), bcm_errmsg(rv));
            return (-1);
        }
        if (ms == lw->lw_save_port[port].phy_master) {
            lw->lw_save_port[port].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
        }

        if ((rv = bcm_port_info_restore(lw->lw_unit, port,
                                    &lw->lw_save_port[port])) < 0) {
            test_error(lw->lw_unit,
                       "Port %s: Could not restore port info: %s\n",
                       SOC_PORT_NAME(lw->lw_unit, port), bcm_errmsg(rv));
            return(-1);
        }
    }

    if (SOC_IS_XGS_SWITCH(lw->lw_unit)) {
        /* Restore source port/module for ipic */
        SOC_DEFAULT_DMA_SRCMOD_SET(lw->lw_unit, lb_saved_srcmod[lw->lw_unit]);
        SOC_DEFAULT_DMA_SRCPORT_SET(lw->lw_unit, lb_saved_srcport[lw->lw_unit]);
    }
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(lw->lw_unit)) {
        if (l2_index >= 0) {
            if ((rv = bcm_l2_cache_set(lw->lw_unit, l2_index,
                                       &lb_saved_l2[lw->lw_unit],
                                       &l2_index)) < 0) {
                test_error(lw->lw_unit,
                           "Failed to Restore L2 Cache entry %s\n",
                           bcm_errmsg(rv));
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return(0);
}


STATIC int
lb_setup_port(int unit, soc_port_t port, int req_speed, int autoneg)
{
    int         rv;
    pbmp_t      pbm;

    rv = lbu_setup_port(unit, port, req_speed, autoneg);

    if (rv >= 0) {
        SOC_PBMP_CLEAR(pbm);
        SOC_PBMP_PORT_ADD(pbm, port);
        bcm_link_wait(unit, &pbm, 200);
    }

    return(rv);
}

STATIC int
lb_save_vlan(loopback_test_t *lw)
{
    int         rv;
    
    if (SOC_IS_SHADOW(lw->lw_unit)) {
        return SOC_E_NONE;
    }
    /* Save the spanning tree group of the vlan */
    if ((rv = bcm_vlan_stg_get(lw->lw_unit, lw->lw_lp->lp_vlan, 
                               &lw->lw_save_vlan_stg)) < 0) {
        if (rv == BCM_E_NOT_FOUND) {
            /* vlan is not valid, nothing to save so just return */
            return (0);
        } else {
            test_error(lw->lw_unit,
                       "Could not get vlan stg: %s\n", bcm_errmsg(rv));
            return(-1);
        }
    }
    
    /* Save the vlan port bitmaps */
    if ((rv = bcm_vlan_port_get(lw->lw_unit, lw->lw_lp->lp_vlan, 
                                &lw->lw_save_vlan_pbmp,
                                &lw->lw_save_vlan_ubmp)) < 0) {
        if (rv == BCM_E_NOT_FOUND) {
            /* vlan is not valid, nothing to save so just return */
            return (0);
        } else {
            test_error(lw->lw_unit,
                       "Could not get vlan port: %s\n", bcm_errmsg(rv));
            return(-1);
        }
    }

    /* Remove ports before test, they will be re-added when restoring */
    if ((rv = bcm_vlan_port_remove(lw->lw_unit, lw->lw_lp->lp_vlan, 
                                   lw->lw_save_vlan_pbmp)) < 0) {
        test_error(lw->lw_unit,
                   "Could not remove vlan ports: %s\n", bcm_errmsg(rv));
        return(-1);
    } 

    /* A valid vlan was found, remember that */
    lw->lw_save_vlan_valid = 1;

    return(0);
}

STATIC int
lb_restore_vlan(loopback_test_t *lw)
{
    int         rv;
    pbmp_t      pbmp, ubmp;

    if (SOC_IS_SHADOW(lw->lw_unit)) {
        return SOC_E_NONE;
    }
    if (lw->lw_save_vlan_valid) {
        /* the saved vlan was valid, restore it */
        
        /* Set the spanning tree group */
        rv = bcm_vlan_stg_set(lw->lw_unit, lw->lw_lp->lp_vlan,
                              lw->lw_save_vlan_stg);
        if (rv < 0 && rv != BCM_E_UNAVAIL) {
            test_error(lw->lw_unit, "Could not set vlan %d stg %d: %s\n",
                       lw->lw_lp->lp_vlan,
                       lw->lw_save_vlan_stg,
                       bcm_errmsg(rv));
            return(-1);
        }

        /* Get/remove all the ports that were added during the test */
        if ((rv = bcm_vlan_port_get(lw->lw_unit, lw->lw_lp->lp_vlan, 
                                    &pbmp, &ubmp)) < 0) {
            test_error(lw->lw_unit,
                       "Could not get vlan port: %s\n", bcm_errmsg(rv));
            return(-1);
        } 
        if ((rv = bcm_vlan_port_remove(lw->lw_unit, lw->lw_lp->lp_vlan, 
                                       pbmp)) < 0) {
            test_error(lw->lw_unit,
                       "Could not remove vlan ports: %s\n", bcm_errmsg(rv));
            return(-1);
        }

        /* Re-add the saved ports to the vlan */
        if ((rv = bcm_vlan_port_add(lw->lw_unit, lw->lw_lp->lp_vlan, 
                                    lw->lw_save_vlan_pbmp,
                                    lw->lw_save_vlan_ubmp)) < 0) {
            test_error(lw->lw_unit,
                       "Could not set vlan port: %s\n", bcm_errmsg(rv));
            return(-1);
        } 

        SOC_PBMP_CLEAR(lw->lw_save_vlan_pbmp);
        SOC_PBMP_CLEAR(lw->lw_save_vlan_ubmp);
        lw->lw_save_vlan_stg = 0;
        lw->lw_save_vlan_valid = 0;
    } else {
        /* the vlan is not valid, destroy what we created */

        rv = bcm_vlan_destroy(lw->lw_unit, lw->lw_lp->lp_vlan);
        if (rv < 0) {
            test_error(lw->lw_unit, "Could not destroy vlan %d: %s\n",
                       lw->lw_lp->lp_vlan, bcm_errmsg(rv));
            return(-1);
        } 
    }

    return(0);
}

STATIC int
lb_stack_check(int unit)
{
    if (SOC_SL_MODE(unit)) {
        return -1;
    }

    return 0;
}

/*
 * Loopback Statistics Report
 */

STATIC void
lb_stats_init(loopback_test_t *lw)
{
    lw->lw_tx_count = 0;
    lw->lw_rx_count = 0;

    lw->lw_tx_bytes = 0.0;
    lw->lw_rx_bytes = 0.0;

    lw->lw_tx_stime = sal_time();
    lw->lw_tx_rtime = lw->lw_tx_stime + sh_set_report_time;

    lw->lw_stats_init = TRUE;
}

STATIC void
lb_stats_report(loopback_test_t *lw)
{
    sal_time_t          now;
    int                 secs;

    if (!sh_set_report_status) {
        return;
    }

    /*
     * Feel free to call this routine after every packet sent.
     * It will only report stats every "sh_set_report_time" seconds.
     */

    now = sal_time();

    if ((secs = (now - lw->lw_tx_stime)) == 0) {
        secs = 1;
    }

    if (now >= lw->lw_tx_rtime) {
        test_msg("LB: xmit %d pkt (%d%%, %d pkt/s, %d KB/s avg), "
                 "recv %d pkt (%d sec)\n",
                 lw->lw_tx_count/lw->lw_tx_dcb_factor,
                 100 * lw->lw_rx_count / lw->lw_tx_total,
                 (int) (lw->lw_tx_count / secs),
                 (int) (lw->lw_tx_bytes / 1024 / secs),
                 lw->lw_rx_count, secs);

        lw->lw_tx_rtime += sh_set_report_time;
    }
}

STATIC void
lb_stats_done(loopback_test_t *lw)
{
    if (lw->lw_stats_init && lw->lw_tx_count > 0) {
        lw->lw_tx_rtime = 0;            /* Force output now */
        lb_stats_report(lw);
    }
}


#ifdef BCM_XGS3_SWITCH_SUPPORT
STATIC  void
lb_xgs3_higig_setup(loopback_test_t *lw, int cos, uint32 dst_mod,
                    uint32 dst_port, uint32 vlan, uint32 *xghp)
{
    int         unit = lw->lw_unit;
    soc_higig_hdr_t *xgh = (soc_higig_hdr_t *)xghp; 
    /* loopback_testdata_t      *lp = lw->lw_lp;*/

#if defined(BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) 
        && !soc_feature(unit, soc_feature_higig2)) {
        soc_pbsmh_hdr_t *pbh = (soc_pbsmh_hdr_t *)xghp;
        if ((SOC_IS_TOMAHAWK(unit)) || (SOC_IS_TOMAHAWK3(unit))) {
            PBS_MH_V9_W0_START_SET(pbh);
            PBS_MH_V9_W1_DPORT_SET(pbh, dst_port);
            PBS_MH_V9_W2_SMOD_COS_SET(pbh, 0, 1, cos);
            return;
        } else if (SOC_IS_TRIDENT3X(unit)) {
            PBS_MH_V11_W0_START_SET(pbh);
            PBS_MH_V11_W1_DPORT_SET(pbh, dst_port);
            PBS_MH_V11_W2_SMOD_COS_SET(pbh, 0, 1, cos);
            return;
        }
    }
#endif

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) || SOC_IS_SHADOW(unit) ||
       ((SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) && (IS_E_PORT(unit, dst_port)))) {
        soc_pbsmh_hdr_t *pbsh = (soc_pbsmh_hdr_t *)xghp;
        sal_memset(xgh, 0, sizeof(soc_higig2_hdr_t));
        soc_pbsmh_field_set(unit, pbsh, PBSMH_start, 0xff);
        soc_pbsmh_field_set(unit, pbsh, PBSMH_src_mod, dst_mod);
        soc_pbsmh_field_set(unit, pbsh, PBSMH_dst_port, dst_port);
        soc_pbsmh_field_set(unit, pbsh, PBSMH_cos, cos);
        soc_pbsmh_field_set(unit, pbsh, PBSMH_pri, cos);
        return;
    }
#endif

    /*
     * Construct higig header to direct packet to a particular
     * egress port
     */
    if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_HIGIG2_SUPPORT
        sal_memset(xgh, 0, sizeof(soc_higig2_hdr_t));
        soc_higig_field_set(unit, xgh, HG_dst_port, dst_port);
        if (soc_feature(unit, soc_feature_higig2)) {
            soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
        } else {
            soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
        }
#else
        sal_memset(xgh, 0, sizeof(soc_higig_hdr_t));
        soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG_START);
#endif
        soc_higig_field_set(unit, xgh, HG_hgi, SOC_HIGIG_HGI);
        soc_higig_field_set(unit, xgh, HG_hdr_format,
                            SOC_HIGIG_HDR_DEFAULT);
        soc_higig_field_set(unit, xgh, HG_src_mod, dst_mod + 1);
        if (IS_ST_PORT(unit, dst_port)) {
            soc_higig_field_set(unit, xgh, HG_opcode, 0);
            soc_higig_field_set(unit, xgh, HG_src_port, dst_port);
        } else {
            soc_higig_field_set(unit, xgh, HG_opcode, 1);
        }
        soc_higig_field_set(unit, xgh, HG_vlan_id, vlan);
        soc_higig_field_set(unit, xgh, HG_dst_port, dst_port);
        soc_higig_field_set(unit, xgh, HG_dst_mod, dst_mod);
        soc_higig_field_set(unit, xgh, HG_cos, cos);
        soc_higig_field_set(unit, xgh, HG_ingress_tagged, 1);
    }
}
#else
STATIC  void
lb_xgs3_higig_setup(loopback_test_t *lw, int cos, uint32 dst_mod,
                    uint32 dst_port, uint32 vlan, void *xgh)
{
}
#endif

STATIC  void
lb_setup_dcbs(uint8 **packets, dv_t *dv, int l, int c, pbmp_t pbm,
              pbmp_t ubm, uint32 crc, uint32 cos, uint32 dport,
              uint32 dmod, uint32 *hgh, int dma_mode, dvt_t dvt, uint8 **dcbs)
/*
 * Function:    lb_setup_dcbs
 * Purpose:     Fill in DCBs with length, bitmaps etc.
 * Parameters:  packets - pointer to array of packets to use.
 *              dv - DMA I/O vector
 *              l - length for each dcb
 *              c - # of dcbs to chain
 *              pbm, ubm, ip, crc, cos - descriptor control word settings
 *              hgh - Higig header for XGS3 devices
 * Returns:     Nothing
 */
{
    uint32      flags;
    pbmp_t      empty;

    flags = SOC_DMA_COS(cos);
    if (crc) {
        flags |= SOC_DMA_CRC_REGEN;
    }
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_HAWKEYE(dv->dv_unit) || ((!PBMP_MEMBER(pbm, CMIC_PORT(dv->dv_unit))) && (hgh != NULL))) {

        SOC_DMA_HG_SET(flags, 1);
        SOC_DMA_STATS_SET(flags, 1);
    }

#endif
    SOC_DMA_DPORT_SET(flags, dport);
    SOC_DMA_DMOD_SET(flags, dmod);
    SOC_PBMP_CLEAR(empty);

    while (c--) {
        if(soc_feature(dv->dv_unit, soc_feature_cmicx)) {
            if (dvt == DV_TX) {
                if (SOC_DMA_HG_GET(flags)) {
                sal_memcpy(*dcbs, hgh, MH_BYTES);
                soc_dma_desc_add(dv, (sal_vaddr_t)(*dcbs++), MH_BYTES,
                             pbm, ubm, empty, flags, (uint32 *)hgh);
            }
        }
        }

        soc_dma_desc_add(dv, (sal_vaddr_t)(*packets++), l,
                         pbm, ubm, empty, flags, (uint32 *)hgh);
        soc_dma_desc_end_packet(dv);
    }

    if (LB_DMA_MODE_IS_CONTINUOUS(dma_mode)) {
        if ((soc_dma_rld_desc_add(dv, 0)) < 0) {
            cli_out("ERROR: Could not add rld desc at end of chain\n");
        }
    }
}

STATIC void
lb_done_chain(int unit, dv_t *dv_chain)
/*
 * Function:    lb_done_chain
 * Purpose:     Process End-of-chain interrupt.
 * Parameters:  unit - unit #
 *              dv_chain - pointer to dv chain that completed, from ISR
 * Returns:     Nothing.
 */
{
    loopback_test_t     *lw = &lb_work[unit];
    dv_t                *last_dv;
    dcb_t               *last_dcb;
    int                 level;

    /* NOTE: This routine is called in interrupt context. */
    assert(dv_chain);
    for (last_dv = dv_chain; last_dv->dv_chain; last_dv = last_dv->dv_chain)
        ;

    last_dcb = SOC_DCB_IDX2PTR(unit, last_dv->dv_dcb, last_dv->dv_vcnt - 1);

    if (!SOC_DCB_DONE_GET(unit, last_dcb)) {
        cli_out("ERROR: Chain done when not done at dcb %p\n", last_dcb);
        soc_dma_dump_dv(unit, "err vals: ", dv_chain);
    }

    if (dv_chain == lw->lw_tx_dv_chain_done) {
        lw->lw_eoc_tx = TRUE;
    } else if (dv_chain == lw->lw_rx_dv_chain_done) {
        lw->lw_eoc_rx = TRUE;
    } else {
        cli_out("Warning: dv chain done on DV_CHAIN[%p] "
                "Expecting Rx[%p] or Tx[%p]\n",
                (void *)dv_chain, 
                (void *)lw->lw_rx_dv_chain_done,
                (void *)lw->lw_tx_dv_chain_done);
    }

    level = sal_splhi();
    /* Wake up thread to check things out */
    if (!lw->lw_sema_woke) {
        lw->lw_sema_woke = TRUE;
        if (sal_sem_give(lw->lw_sema)) {
            cli_out("Warning: Chain done give failed\n");
        }
    }
    sal_spl(level);
}

STATIC void
lb_done_desc(int unit, dv_t *dv, dcb_t *dcb)
/*
 * Function:    lb_done_desc
 * Purpose:     Process End-of-descriptor interrupt.
 * Parameters:  unit - unit #
 *              dv - pointer to dv containing DCB
 *              dcb - pointer to completed DCB
 * Returns:     Nothing.
 */
{
    loopback_test_t     *lw = &lb_work[unit];
    dcb_t               *last_pkt_dcb;
    int                 level;

    COMPILER_REFERENCE(dv);
    COMPILER_REFERENCE(dcb);
    /*
     * NOTE: This routine is called in interrupt context.
     * Wake up thread to check things out.
     */

    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        assert(dv);

        /*
         * In this mode, the last packet DCB is the second to last DCB in
         * the chain. Find the DCB at index (valid_count - 2).
         */
        last_pkt_dcb = LB_LAST_PKT_DCB(unit, dv->dv_dcb, dv->dv_vcnt);

        /* When it is complete set pkt done (use chain done) */
        if (dcb == last_pkt_dcb) {
            if (dv == lw->lw_tx_dv_chain_done) {
                lw->lw_eoc_tx = TRUE;
            } else if (dv == lw->lw_rx_dv_chain_done) {
                lw->lw_eoc_rx = TRUE;
            } else {
                cli_out("Warning: dv desc done on DV[%p] "
                        "Expecting Rx[%p] or Tx[%p]\n",
                        (void *)dv,
                        (void *)lw->lw_rx_dv_chain_done,
                        (void *)lw->lw_tx_dv_chain_done);
            }
            level = sal_splhi();
            /* Wake up thread to check things out */
            if (!lw->lw_sema_woke) {
                lw->lw_sema_woke = TRUE;
                if (sal_sem_give(lw->lw_sema)) {
                    cli_out("Warning: Desc done give failed\n");
                }
            }
            sal_spl(level);
        }
    } else {
        level = sal_splhi();
        /* Wake up thread to check things out */
        if (!lw->lw_sema_woke) {
            lw->lw_sema_woke = TRUE;
            if (sal_sem_give(lw->lw_sema)) {
                cli_out("Warning: Desc done give failed\n");
            }
        }
        sal_spl(level);
    }
}

STATIC void
lb_done_reload(int unit, dv_t *dv)
/*
 * Function:    lb_done_reload
 * Purpose:     Process End-of-DV.
 * Parameters:  unit - unit #
 *              dv - pointer to dv containing DCB
 * Returns:     Nothing.
 */
{
    loopback_test_t     *lw = &lb_work[unit];

    /*
     * When the reload descriptor has been processed, the dv index is
     * updated to reflect the DV that is currently in use.
     */
    if (dv->dv_op == DV_TX) {
        lw->lw_tx_dv_idx = (lw->lw_tx_dv_idx == 0) ? 1 : 0;
    } else {
        lw->lw_rx_dv_idx = (lw->lw_rx_dv_idx == 0) ? 1 : 0;
    }
}


int
lb_check_tx(loopback_test_t *lw, int length, int c)
/*
 * Function:    lb_check_tx
 * Purpose:     Verify the DCB status for each of the packets.
 * Parameters:  lw - pointer to work struct.
 *              length - # bytes in transmitted packet
 *              c - count of # dcbs in chain.
 * Returns:     0 - OK
 *              1 - OK and complete.
 *              -1 - failed.
 */
{
    dv_t                *dv = lw->lw_tx_dv[lw->lw_tx_dv_idx];
    dcb_t               *dcb;
    dcb_t               *end_dcb;
    int                 unit = lw->lw_unit;

    COMPILER_REFERENCE(c);

    /* dv->dv_dsc is the next entry to look at */

    /* after last dcb */
    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        /*
         * In cont mode, the last DCB is the reload desc. We need to
         * signal done AFTER the last packet DCB (on reload).
         */
        end_dcb = LB_LAST_DCB(unit, dv->dv_dcb, dv->dv_vcnt);
    } else {
        end_dcb = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt);
    }
    dcb = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_dsc);

    while ((dcb < end_dcb) && SOC_DCB_DONE_GET(unit, dcb)) {
        /* Check dcb info */
        dcb = SOC_DCB_IDX2PTR(unit, dcb, 1);
        lw->lw_tx_count++;
        lw->lw_tx_bytes += length;
    }

    /* Update next to look at */
    dv->dv_dsc = SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb);

    return(dcb == end_dcb);
}

STATIC int
lb_check_packet(loopback_test_t *lw, uint8 *tx_data, int tx_len,
                uint8 *rx_data, int rx_len, int rx_crc_valid,
                sal_mac_addr_t mac_dst, sal_mac_addr_t mac_src, int skip_vlan)
/*
 * Function:    lb_check_packet
 * Purpose:     Check a received packet to be sure it matches a sent packet.
 * Parameters:  lw - current loopback work structure.
 *              tx_data - transmit data buffer
 *              tx_len - user-requested transmit length
 *              rx_data - receive data buffer
 *              rx_len - actual received packet length, should match tx_len.
 *              mac_src - Source MAC address under test
 *              mac_dst - Dest MAC address under test
 *              skip_vlan - Jump over VLAN tag for higig lb packets
 *              on XGS3 device
 * Returns:     -1 - failed compare
 *              0 - passed compare.
 */
{
    char        src_mac[SAL_MACADDR_STR_LEN],
                dst_mac[SAL_MACADDR_STR_LEN],
                exp_src_mac[SAL_MACADDR_STR_LEN],
                exp_dst_mac[SAL_MACADDR_STR_LEN];
    int         i;
    int         rv = 0;
    int         tx_compare_len, orig_rx_len;
    uint8       *orig_rx_data;
    uint32      header_size = 0;

    if(soc_feature(lw->lw_unit, soc_feature_cmicx)) {
        soc_dma_header_size_get(lw->lw_unit, &header_size);

        rx_data += header_size;
        rx_len -= header_size;
    }

#ifdef BCM_XGS12_FABRIC_SUPPORT
    /* Inspect HIGIG/XGS header */
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        soc_higig_hdr_t *tx_hdr = (soc_higig_hdr_t*)tx_data;
        soc_higig_hdr_t *rx_hdr = (soc_higig_hdr_t*)rx_data;

        if (soc_higig_field_get(lw->lw_unit, tx_hdr, HG_start) !=
            soc_higig_field_get(lw->lw_unit, rx_hdr, HG_start)) {
            cli_out("ERROR: HIGIG START not detected (tx=%x,rx=%x)\n",
                    soc_higig_field_get(lw->lw_unit, tx_hdr, HG_start),
                    soc_higig_field_get(lw->lw_unit, rx_hdr, HG_start));
        }
        
        rx_data += SOC_HIGIG_HDR_SIZE;
        tx_data += SOC_HIGIG_HDR_SIZE;
        /* Subtract the header from bytes received and transmitted */
        rx_len -= SOC_HIGIG_HDR_SIZE;
        tx_len -= SOC_HIGIG_HDR_SIZE;
    }
#endif /* BCM_XGS12_FABRIC_SUPPORT */

    /* Check source and destination MAC address */

    if (ENET_CMP_MACADDR(mac_dst, &rx_data[0]) ||
        ENET_CMP_MACADDR(mac_src, &rx_data[6])) {
        format_macaddr(exp_src_mac, mac_src);
        format_macaddr(exp_dst_mac, mac_dst);
        format_macaddr(src_mac, &rx_data[6]);
        format_macaddr(dst_mac, &rx_data[0]);
        cli_out("ERROR: MAC address miscompare:\n"
                "\tExpected src=%s dst=%s\n\tReceived src=%s dst=%s\n",
                exp_src_mac, exp_dst_mac, src_mac, dst_mac);

        rv = -1;
    }
    /* Save these for error printouts */
    orig_rx_data = rx_data;
    orig_rx_len = rx_len;
    if (skip_vlan) {
        rx_len -= 4; /* Account for 4 bytes of VLAN tag added by Egress */   
        rx_data += 4;
    }
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        if (lw->lw_lp->lp_crc_mode != CRC_MODE_CPU_NONE) {
            tx_len += sizeof(uint32); /* Add length for CRC */
        }
        tx_compare_len = tx_len;
    } else {
        if ((lw->lw_lp->lp_crc_mode == CRC_MODE_CPU_NONE ||
             lw->lw_lp->lp_crc_mode == CRC_MODE_CPU_APPEND) && rx_crc_valid) {
            tx_compare_len = tx_len;
        } else {
            tx_compare_len = tx_len - 4;
        }
    }

    /* Verify lengths, and if they match, verify payload */

    if (tx_len != rx_len) {
        soc_pci_analyzer_trigger(lw->lw_unit);
        cli_out("ERROR: Length miscompare: TX(%d) RX(%d)\n",
                tx_len, rx_len);
        rv = -1;
    } else if ((i = packet_compare(rx_data + 12,
                                   tx_data + 12,
                                   4)) >= 0) {
        cli_out("ERROR: VLAN tag miscompare: offset 0x%x\n", i + 12);
        rv = -1;
    } else if ((i = packet_compare(rx_data + 16,
                                   tx_data + 16,
                                   4)) >= 0) {
        cli_out("ERROR: Sequence # miscompare: offset 0x%x\n"
                "\tExpected 0x%08x, Received 0x%08x\n", 16,
                packet_load(tx_data + 16, 4),
                packet_load(rx_data + 16, 4));
        rv = -1;
    } else if ((i = packet_compare(rx_data + 20,
                                   tx_data + 20,
                                   tx_compare_len - 20)) >= 0) {
        cli_out("ERROR: Payload miscompare: offset 0x%x\n", i + 20);
        rv = -1;
    }

    if (lw->lw_lp->lp_check_crc) {
        uint32          calc_crc, rx_crc;

        calc_crc = ~_shr_crc32(~0, rx_data, rx_len - 4);
        rx_crc = packet_load(rx_data + rx_len - 4, 4);

        if (calc_crc != rx_crc) {
            cli_out("ERROR: CRC miscompare: calc=0x%08x rx=0x%08x\n",
                    calc_crc, rx_crc);
            if (! rx_crc_valid)
                cli_out("ERROR: Note that invalid CRC was expected "
                        "due to chosen parameters.\n");
            rv = -1;
        }
    }

    if (rv) {
        /* On error, dump tx/rx packets, and call test_error once */

        cli_out("TX packet: len=%d\n", tx_len);
        soc_dma_dump_pkt(lw->lw_unit, "  ", tx_data, tx_len, TRUE);

        cli_out("RX packet: len=%d%s\n", orig_rx_len,
                skip_vlan ? " - 4 (VLAN)" : "");
        soc_dma_dump_pkt(lw->lw_unit, "  ", orig_rx_data, orig_rx_len - header_size, TRUE);

        test_error(lw->lw_unit,
                   "ERROR Found when verifying received packet\n");
    }

    return(rv);
}

int
lb_check_rx(loopback_test_t *lw, int length, int c)
/*
 * Function:    lb_check_rx
 * Purpose:     Verify the DCB status for each of the packets.
 * Parameters:  lw - pointer to work struct.
 *              length - # bytes in transmitted packet
 *              c - count of # dcbs in chain.
 * Returns:     0 - OK
 *              1 - OK and complete.
 *              -1 - failed.
 */
{
    dv_t        *dv = lw->lw_rx_dv[lw->lw_rx_dv_idx];
    dcb_t       *rx_dcb, *tx_dcb, *end_dcb;
    int         rv = 0;
    loopback_testdata_t *lp = lw->lw_lp;
    int         unit = lw->lw_unit;
    int         skip_vlan = 0;
    uint8       *rx_data = NULL;
    soc_port_t  sp;

    COMPILER_REFERENCE(c);

    /* Check the next entry to look at */
    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        /*
         * In cont mode, the last DCB is the reload desc. We need to
         * signal done after the last packet DCB (just before reload)
         */
        end_dcb = LB_LAST_DCB(unit, dv->dv_dcb, dv->dv_vcnt);
    } else {
        end_dcb = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt);
    }
    rx_dcb = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_dsc);
    tx_dcb = SOC_DCB_IDX2PTR(unit,
                             lw->lw_tx_dv[lw->lw_tx_dv_idx]->dv_dcb,
                             dv->dv_dsc + lw->lw_tx_dcb_factor - 1);

    while (rx_dcb < end_dcb) {
        int             rx_crc_valid, rx_length;

        if (!SOC_DCB_DONE_GET(unit, rx_dcb)) {
            cli_out("RX DCB @%p NOT DONE\n", rx_dcb);
            break;
        }

        /*
         * If the CPU, as an egress, was asked to regenerate or append
         * the CRC for this packet, that means the packet was modified
         * at the ingress/MMU and the CRC is known to be invalid.  This
         * was probably an untagged packet that got tagged.
         */

        rx_crc_valid = !SOC_DCB_RX_CRC_GET(unit, rx_dcb);
        rx_length = SOC_DCB_XFERCOUNT_GET(unit, rx_dcb);

        lw->lw_rx_count++;
        lw->lw_rx_bytes += rx_length;

        if(soc_feature(unit, soc_feature_cmicx)) {
            rx_data = (uint8 *)SOC_DCB_ADDR_GET(unit, rx_dcb);
        }

        if (lp->lp_check_data) {
            if(soc_feature(unit, soc_feature_cmicx)) {
                sp = SOC_DCB_RX_INGPORT_GET(unit, rx_data);
            } else {
                sp = SOC_DCB_RX_INGPORT_GET(unit, rx_dcb);
            }
            if (sp != lp->lp_rx_port) {
                cli_out("RX packet with invalid port: "
                        "expected port %s, received port %s\n",
                        SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                        SOC_PORT_NAME(lw->lw_unit, sp));
                rv = -1;
            }
#ifdef BCM_XGS3_SWITCH_SUPPORT
            if ((SOC_IS_XGS3_SWITCH(lw->lw_unit)) &&
                (IS_ST_PORT(lw->lw_unit, lp->lp_rx_port))){
                skip_vlan = 1;
            }
#endif
            if (lb_check_packet(lw,
                 (uint8 *)SOC_DCB_ADDR_GET(unit, tx_dcb),
                 length,
                 (uint8 *)SOC_DCB_ADDR_GET(unit, rx_dcb),
                 rx_length,
                 rx_crc_valid,
                 lw->lw_cur_mac_dst,
                 lw->lw_cur_mac_src, skip_vlan) < 0) {
                rv = -1;
            }

            if (rv < 0) {
                int diff = SOC_DCB_PTR2IDX(unit, rx_dcb, dv->dv_dcb);
                cli_out("Failing DV @%p, DCB[%d]\n", (void *)dv, diff);
                soc_dma_dump_dv(lw->lw_unit, "bad pkt dv: ", dv);
                break;                  /* while */
            }
        }
        rx_dcb = SOC_DCB_IDX2PTR(unit, rx_dcb, 1);
        tx_dcb = SOC_DCB_IDX2PTR(unit, tx_dcb, 1 + lw->lw_tx_dcb_factor - 1);

        increment_macaddr(lw->lw_cur_mac_dst, lp->lp_mac_dst_inc);
        increment_macaddr(lw->lw_cur_mac_src, lp->lp_mac_src_inc);
    }

    dv->dv_dsc = SOC_DCB_PTR2IDX(unit, rx_dcb, dv->dv_dcb); /* Update next */

    if (!rv && (rx_dcb == end_dcb)) {
        rv = 1;                         /* Signal done */
    }

    return(rv);
}

STATIC  void
lb_deallocate(int unit, int dv_cnt, dv_t ***dv_array_ptr,
              int packet_cnt, uint8 ***packet_array_ptr,
              uint8 ***dcb_array_ptr, dvt_t dvt)
/*
 * Purpose:     Deallocate whatever is allocated by lb_allocate.
 * Parameters:  dv_cnt - number of DVs originally allocated.
 *              dv_array - array of dv pointers.
 *              packet_cnt - number of packets originally allocated.
 *              packet_array - array of packet ptrs
 * Returns:     0 - success
 *              -1 - failed, all allocated memory should be freed.
 */
{
    int                 i;
    dv_t                **dv_array      = *dv_array_ptr;
    uint8               **packet_array  = *packet_array_ptr;
    uint8               **dcb_array     = NULL;

    if(soc_feature(unit, soc_feature_cmicx) && (dvt == DV_TX)) {
        dcb_array     = *dcb_array_ptr;
    }

    if (dv_array) {
        *dv_array_ptr = 0;
        for (i = 0; i < dv_cnt; i++)
            if (dv_array[i])
                soc_dma_dv_free(unit, dv_array[i]);
        sal_free(dv_array);
    }

    if (packet_array) {
        *packet_array_ptr = 0;

        packet_cnt *= 4;    /* for 2 way and 2 phase */
        for (i = 0; i < packet_cnt; i++)
            if (packet_array[i])
                soc_cm_sfree(unit, packet_array[i]);
        sal_free(packet_array);
    }

    if (dcb_array) {
        *dcb_array_ptr = 0;

        for (i = 0; i < packet_cnt; i++)
            if (dcb_array[i])
                soc_cm_sfree(unit, dcb_array[i]);
        sal_free(dcb_array);
    }
}

STATIC  int
lb_allocate(int unit, dvt_t dvt, int dv_cnt, int dv_size, dv_t ***dv_array_ptr,
            int packet_cnt, int packet_size, uint8 ***packet_array_ptr, uint8 ***dcb_array_ptr)
/*
 * Purpose:     Allocate array of dv and array of packets
 * Parameters:  dvt - Type of operation being allocated for (DV_TX/DV_RX)
 *              dv_cnt - number of DVs to allocate.
 *              dv_size - number of DCBs in DV.
 *              dv_array_ptr - address of pointer to array of dv pointers.
 *              packet_cnt - number of packets to allocate.
 *              packet_size - size of packets to allocate.
 *              packet_array_ptr - address of pointer array of packet ptrs.
 * Returns:     0 - success
 *              -1 - out of memory (no memory is left allocated)
 */
{
    dv_t        **dv_array = NULL; 
    uint8       **packet_array = NULL;
    uint8       **dcb_array = NULL;
    int         i = 0;
    uint32      header_size = 0;

    *dv_array_ptr = 0;
    *packet_array_ptr = 0;

    /* Allocate Array for dv pointers */

    if (!(dv_array = sal_alloc(dv_cnt * sizeof(dv_t *), "dv_array"))) {
        goto fail;
    }
    sal_memset(dv_array, 0, dv_cnt * sizeof(dv_t *));
    *dv_array_ptr = dv_array;

    /* Allocate actual DVs */

    for (i = 0; i < dv_cnt; i++) {
        /*
         * Add 1 to DV size to get an extra DCB so soc_dma_dv_join can
         * always be used.
         * *4 because of 2 way and 2 circulation test.
         */
        if (!(dv_array[i] = soc_dma_dv_alloc(unit, dvt, (dv_size*4) + 1))) {
            goto fail;
        }
        dv_array[i]->dv_flags       &= ~DV_F_COMBINE_DCB;
        dv_array[i]->dv_done_chain  = lb_done_chain;
        dv_array[i]->dv_done_desc   = lb_done_desc;
        dv_array[i]->dv_done_reload = lb_done_reload;
        dv_array[i]->dv_dsc         = 0;
        dv_array[i]->dv_pckt        = 0;
    }

    /* Allocate packet pointer array, 2 way and 2 phase */

    if (!(packet_array = sal_alloc(packet_cnt* 4 * sizeof(uint8 *),
                                   "packet_array"))) {
        goto fail;
    }
    /* remember there are 2 way and 2 phase 2*2=4*/
    sal_memset(packet_array, 0, packet_cnt *4 * sizeof(uint8 *));
    *packet_array_ptr = packet_array;

    /* Allocate actual packets */

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        packet_size += SOC_HIGIG_HDR_SIZE + sizeof(uint32); /* CRC */
    }
#endif

    if(soc_feature(unit, soc_feature_cmicx)) {
        soc_dma_header_size_get(unit, &header_size);

        if (dvt == DV_RX) {
            packet_size += header_size;
        }

        if (dvt == DV_TX) {
            *dcb_array_ptr = 0;

            if (!(dcb_array = sal_alloc(packet_cnt * 4 * sizeof(uint8 *),
                                   "dcb_array"))) {
                goto fail;
            }

            sal_memset(dcb_array, 0, packet_cnt * 4 * sizeof(uint8 *));
            *dcb_array_ptr = dcb_array;

            for (i = 0; i < packet_cnt*4; i++) {
                if (!(dcb_array[i] =
                    soc_cm_salloc(unit,
                                  SOC_DMA_ROUND_LEN(MH_BYTES), "DCB"))) {
                    goto fail;
                }
            }
        }
    }

    for (i = 0; i < packet_cnt*4; i++) {
        if (!(packet_array[i] =
              soc_cm_salloc(unit,
                            SOC_DMA_ROUND_LEN(packet_size + 4), "LB"))) {
            goto fail;
        }
    }

    return(0);                          /* All done */

 fail:
    lb_deallocate(unit, dv_cnt, &dv_array, packet_cnt, &packet_array, &dcb_array, dvt);
    return(-1);
}

int
lb_wait(loopback_test_t *lw, int length, int chain)
/*
 * Function:    lb_wait
 * Purpose:     Wait for packets to Tx/Rx and check data/dcbs
 *              as they complete.
 * Parameters:  lw - pointer to work structure.
 *              length - current Tx/Rx length
 *              chain - Current number DCBs valid in chain
 * Returns:     0 - success
 *              !0 - failed
 * Notes:       10-seconds without any activity results in failure.
 */
{
    int rv = 0;
    int level = 0;

    ENET_SET_MACADDR(lw->lw_cur_mac_dst, lw->lw_mac_dst);
    ENET_SET_MACADDR(lw->lw_cur_mac_src, lw->lw_mac_src);

    while (!lw->lw_eoc_tx || !lw->lw_eoc_rx) {
        if (sal_sem_take(lw->lw_sema, lw->lw_timeout_usec)) {
            cli_out("Time-out waiting for completion "
                    "Tx(%s)=%s Rx(%s)=%s\n",
                    SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_tx_port),
                    lw->lw_eoc_tx ? "Done" : "Pending",
                    SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_rx_port),
                    lw->lw_eoc_rx ? "Done" : "Pending");
            rv = -1;
            return rv;
        } else {
           level = sal_splhi();
           lw->lw_sema_woke = FALSE;
           sal_spl(level);
        }
    }

    if ((lb_check_tx(lw, length, chain) < 0) ||
        (lb_check_rx(lw, length, chain) < 0)) {
        rv = -1;
    }
    if (rv == 0 &&
        bsl_check(bslLayerAppl, bslSourceDma, bslSeverityNormal, lw->lw_unit)) {
        soc_dma_dump_dv(lw->lw_unit, "dma (after): ", lw->lw_rx_dv[0]);
    }

    return rv;
}

STATIC  void
lb_fill_packet_rx(loopback_test_t *lw, uint8 **packets, int count, int len)
/*
 * Function:    lb_fill_packet_rx
 * Purpose:     Fill RX buffer before receiving into it
 * Parameters:  lw - pointer to work structure
 *              packets - array of packet pointers
 *              count - number of packets
 *              len - length of packets
 * Returns:     Nothing.
 * Notes:       Optionally fills the RX buffer with 0xdeadbeef before
 *              receiving into it.  This may help reveal holes in the
 *              RX packet the hardware was supposed to DMA but did not.
 *              This feature is turned on by setting the property
 *              diag_lb_fill_rx=1.
 */
{
    if (lw->lw_rx_fill) {
        int             i;

        for (i = 0; i < count; i++) {
            packet_store(packets[i], len, 0xdeadbeef, 0);
        }
    }
}


#define REG_READ(unit, port, RPKTr, GRPKTr, iPhase, rvp)                \
    (NUM_FE_PORT(unit) > 0                                              \
     ? soc_reg32_read((unit),                                           \
                     soc_reg_addr((unit),                               \
                                  (iPhase) != 1 ? (GRPKTr) : (RPKTr),   \
                                  (port), 0),                           \
                     (rvp))                                             \
     : soc_reg32_read((unit),                                           \
                     soc_reg_addr((unit),                               \
                                  (GRPKTr),                             \
                                  (port), 0),                           \
                     (rvp)))

#define REG_WRITE(unit, port, RPKTr, GRPKTr, iPhase, rv)                \
    (NUM_FE_PORT(unit) > 0                                              \
     ? soc_reg32_write((unit),                                          \
                      soc_reg_addr((unit),                              \
                                   (iPhase) != 1 ? (GRPKTr): (RPKTr),   \
                                   (port), 0),                          \
                      (rv))                                             \
     : soc_reg32_write((unit),                                          \
                      soc_reg_addr((unit),                              \
                                   (GRPKTr),                            \
                                   (port), 0),                          \
                      (rv)))

STATIC  int
lb_do_rx(loopback_test_t *lw, int length, int chain, int cos)
/*
 * Function:    lb_do_rx
 * Purpose:     Start an RX for the specified length/chain.
 * Parameters:  lw - pointer to lb work.
 *              length - length of packet
 *              chain  - # dcb's in chain
 *              cos    - Class of service
 * Returns:     0 - success, -1 failed.
 */
{
    dv_t        *dv = lw->lw_rx_dv[0];
    int         rv;
    pbmp_t      empty_pbm;
    int level = 0;
    uint32 header_size = 0;

    if(soc_feature(lw->lw_unit, soc_feature_cmicx)) {
        soc_dma_header_size_get(lw->lw_unit, &header_size);
    }

    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        /*
         * In continuous mode, 2 DVs are allocated at init to handle the
         * fact that DMA halts on the reload descriptor of the previous DV.
         * If we have already started receiving, then the next DV is
         * appended to the current reload descriptor of the other.
         */
        level = sal_splhi();
        if (!lw->lw_rx_started) {
            lw->lw_rx_started = 1;
            lw->lw_rx_dv_idx = 0;
            dv = lw->lw_rx_dv[lw->lw_rx_dv_idx];
        } else {
            dv = (lw->lw_rx_dv_idx == 0) ? lw->lw_rx_dv[1] : lw->lw_rx_dv[0];
        }
        sal_spl(level);
    }

    /*
     * For RX, set flags to call out for each DCB completed.
     */
    soc_dma_dv_reset(DV_RX, dv);

    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        dv->dv_flags |= DV_F_NOTIFY_DSC;
    } else {
        dv->dv_flags |= DV_F_NOTIFY_DSC | DV_F_NOTIFY_CHN;
    }

    dv->dv_dsc  = 0;

    /*
     * Receive buffer lengths are 4 bytes larger than necessary in order
     * to catch receiving packets larger than expected.
     */

    SOC_PBMP_CLEAR(empty_pbm);

    lb_setup_dcbs(lw->lw_rx_packets, dv, length + 4 + header_size,
                  chain, empty_pbm, empty_pbm, 0, cos, 0, 0,
                  NULL, lw->lw_dma_mode, DV_RX, NULL);
    lb_fill_packet_rx(lw, lw->lw_rx_packets, chain, length + 4 + header_size);
    lw->lw_rx_dv_chain_done = dv;       /* Mark who waiting for */
    lw->lw_eoc_rx = 0;

    if ((rv = soc_dma_start(lw->lw_unit, -1, dv)) < 0) {
        test_error(lw->lw_unit, "Failed to start RX DMA: %s\n",
                   soc_errmsg(rv));
        return(-1);
    }
    return(0);
}

STATIC  void
lb_fill_packet_tx(loopback_test_t *lw, loopback_testdata_t *lp,
                  uint8 *packet,
                  int *tx_len, sal_mac_addr_t mac_dst, sal_mac_addr_t mac_src)
/*
 * Function:    lb_fill_packet_tx
 * Purpose:     Fill in a packet for TXing.
 * Parameters:  lw - pointer to work structure
 *              lp - pointer to parameters
 *              packet - pointer to packet to fill in
 *              tx_len - # bytes in packet.
 *              mac_src - Source MAC address under test
 *              mac_dst - Dest MAC address under test
 * Returns:     Nothing.
 */
{
    enet_hdr_t          *eh = NULL;

#ifdef BCM_XGS12_FABRIC_SUPPORT
    /* All hercules chips need module header prepended to frame */
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        soc_higig_hdr_t     *xghp = (soc_higig_hdr_t *)packet;
        soc_higig_hdr_t     xghd;
        soc_higig_hdr_t     *xgh = &xghd;
        sal_memset(xgh, 0, SOC_HIGIG_HDR_SIZE);
        eh = (enet_hdr_t *)(packet + SOC_HIGIG_HDR_SIZE);
        soc_higig_field_set(lw->lw_unit, xgh, HG_start, SOC_HIGIG_START);
        soc_higig_field_set(lw->lw_unit, xgh, HG_hgi, SOC_HIGIG_HGI);
        soc_higig_field_set(lw->lw_unit, xgh, HG_vlan_id, lp->lp_vlan);
        soc_higig_field_set(lw->lw_unit, xgh, HG_opcode, lp->lp_opcode);
        soc_higig_field_set(lw->lw_unit, xgh, HG_hdr_format,
                            SOC_HIGIG_HDR_DEFAULT);
        soc_higig_field_set(lw->lw_unit, xgh, HG_dst_port, lp->lp_d_port);
        soc_higig_field_set(lw->lw_unit, xgh, HG_dst_mod, lp->lp_d_mod);
        sal_memcpy(xghp, xgh, sizeof(*xghp));
    } else {
        eh = (enet_hdr_t *)packet;
    }
#else
    eh = (enet_hdr_t *)packet;
#endif

    /* Write source/destination MAC address */

    ENET_SET_MACADDR(&eh->en_dhost, mac_dst);
    ENET_SET_MACADDR(&eh->en_shost, mac_src);

    /* Write VLAN tag (16 + 16 bits) and sequence number (32 bits) */

    packet_store((uint8 *)&eh->en_tag_tpid, 2,
                 0x8100 << 16, 0);
    packet_store((uint8 *)&eh->en_tag_ctrl, 2,
                 VLAN_CTRL(0, 0, lp->lp_vlan) << 16, 0);
    packet_store((uint8*)eh + 16, 4,
                 lw->lw_tx_seq++, 0);
    packet_store((uint8*)&eh->en_tag_len, 2, (*tx_len - 22) << 16, 0);

    /*
     * Fill data from after header & seq # to end of packet.  Byte data
     * starts with the lw_pattern MSByte through LSByte and recycles.
     * Optionally overwrite last 4 bytes of packet with correct CRC.
     */

    lp->lp_pattern = packet_store((uint8*)eh + 20, *tx_len - 20,
                                  lp->lp_pattern, lp->lp_pattern_inc);
    if (lp->lp_crc_mode == CRC_MODE_CPU_APPEND) {
        packet_store((uint8*)eh + *tx_len - 4, 4,
                     ~_shr_crc32(~0, (uint8*)eh, *tx_len - 4), 0);
    }

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        if (lp->lp_crc_mode == CRC_MODE_MAC_REGEN) {
            /* hercules always requires CRC field to be zero */
            packet_store(((uint8*)eh + (*tx_len) - 4), 4, 0, 0);
        }

        /* Update length */
        *tx_len += SOC_HIGIG_HDR_SIZE;
    }
#endif
}

STATIC int
lb_do_tx(loopback_test_t *lw, int tx_len, int chain, int cos,
         uint32 dport, uint32 dmod)
/*
 * Function:    lb_do_tx
 * Purpose:     Start a transmit for the current length/chain.
 * Parameters:  lw - pointer to lb work.
 *              tx_len - user-requested packet length
 *              chain  - # dcb's in chain
 *              cos    - Class of service
 * Returns:     0 - success, -1 failed.
 */
{
    dv_t        *dv = lw->lw_tx_dv[0];
    loopback_testdata_t *lp = lw->lw_lp;
    int         c;
    int         rv, org_len;
    sal_mac_addr_t  cur_dst_mac;
    sal_mac_addr_t  cur_src_mac;
    pbmp_t      pbm;
#ifdef BCM_HIGIG2_SUPPORT
    soc_higig2_hdr_t    xgh;
    uint32              *xghp = (uint32 *)&xgh;
#elif defined(BCM_XGS_SUPPORT)
    soc_higig_hdr_t     xgh;
    uint32              *xghp = (uint32 *)&xgh;
#else
    uint32              *xghp = NULL;
#endif
    int level;

    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        /*
         * In continuous mode, 2 DVs are allocated at init to handle the
         * fact that DMA halts on the reload descriptor of the previous DV.
         * If we have already started transmitting, then the next DV is
         * appended to the current reload descriptor of the other.
         */
        level = sal_splhi();
        if (!lw->lw_tx_started) {
            lw->lw_tx_started = 1;
            lw->lw_tx_dv_idx = 0;
            dv = lw->lw_tx_dv[lw->lw_tx_dv_idx];
        } else {
            dv = (lw->lw_tx_dv_idx == 0) ? lw->lw_tx_dv[1] : lw->lw_tx_dv[0];
        }
        sal_spl(level);
    }

    soc_dma_dv_reset(DV_TX, dv);

    if (LB_DMA_MODE_IS_CONTINUOUS(lw->lw_dma_mode)) {
        dv->dv_flags = DV_F_NOTIFY_DSC;
    } else {
        SET_NOTIFY_CHN_ONLY(dv->dv_flags);
    }

    dv->dv_dsc = 0;

    if (lp->lp_pattern_inc || (lw->lw_tx_init_chn != chain) ||
        (lw->lw_tx_init_len != tx_len)) {

        lw->lw_tx_init_chn = chain;
        lw->lw_tx_init_len = tx_len;

        /* Fill in start of TX packet --- len/chain/cos value */

        ENET_SET_MACADDR(cur_dst_mac, lw->lw_mac_dst);
        ENET_SET_MACADDR(cur_src_mac, lw->lw_mac_src);
        org_len = tx_len;
        for (c = 0; c < chain; c++) {
            int new_len = org_len;
            lb_fill_packet_tx(lw, lp, lw->lw_tx_packets[c], &new_len,
                              cur_dst_mac, cur_src_mac);
            increment_macaddr(cur_dst_mac, lp->lp_mac_dst_inc);
            increment_macaddr(cur_src_mac, lp->lp_mac_src_inc);
            tx_len = lw->lw_tx_init_len = new_len ;
        }
    }

    /* Setup # of descriptors desired with current length */

    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, lp->lp_tx_port);
    lb_xgs3_higig_setup(lw, cos, dmod,
                        lp->lp_tx_port, lp->lp_vlan, xghp);
    lb_setup_dcbs(lw->lw_tx_packets, dv, tx_len, chain,
                  pbm, lp->lp_ubm, crc_mode_to_dmac(lp->lp_crc_mode),
                  cos, dport, dmod, xghp, lw->lw_dma_mode, DV_TX, lw->lw_tx_dcbs);

    level = sal_splhi();
    lw->lw_tx_dv_chain_done = dv;       /* Mark who waiting for */
    lw->lw_eoc_tx = 0;
    sal_spl(level);

    if ((rv = soc_dma_start(lw->lw_unit, -1, dv)) < 0) {
        test_error(lw->lw_unit, "Failed to start TX DMA: %s\n",
                   soc_errmsg(rv));
        return(-1);
    }
    return (0);
}

int
lb_do_txrx(loopback_test_t *lw)
/*
 * Function:    lb_do_txrx
 * Purpose:     Loop over requested lengths and chaining sizes, and
 *              transmit/receive packets.
 * Parameters:  lw - pointer to work structure
 * Returns:     0 - success
 *              -1- failed.
 */
{
    loopback_testdata_t *lp = lw->lw_lp;
    int         l_cur, c_cur, cos_cur, c_count;
    int         rv, test_rv = 0;
    uint32      tx_count, rx_count;
    uint32      rx_count0, rx_count1, rx_count2, rx_count3;
    int         i = 0;

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Testing %s->%s count %d, chain %d-%d += %d, "
                       "len %d-%d += %d, cos %d-%d\n"),
              SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
              SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
              lp->lp_count,
              lp->lp_c_start, lp->lp_c_end, lp->lp_c_inc,
              lp->lp_l_start, lp->lp_l_end, lp->lp_l_inc,
              lp->lp_cos_start, lp->lp_cos_end));

    /* Clear port rx/tx counters */
    if ((rv = bcm_stat_clear(lw->lw_unit, lp->lp_tx_port)) < 0) {
        test_error(lw->lw_unit, "Could not clear port %s counters: %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
                   soc_errmsg(rv));
        return -1;
    }

    if ((rv = bcm_stat_clear(lw->lw_unit, lp->lp_rx_port)) < 0) {
        test_error(lw->lw_unit, "Could not clear port %s counters: %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   soc_errmsg(rv));
        return -1;
    }

    for (c_count = 0; c_count < lp->lp_count; c_count++) {
        for (l_cur = lp->lp_l_start; l_cur <= lp->lp_l_end;
             l_cur += lp->lp_l_inc) {
            for (c_cur = lp->lp_c_start; c_cur <= lp->lp_c_end;
                 c_cur += lp->lp_c_inc) {
                for (cos_cur = lp->lp_cos_start;
                     cos_cur <= lp->lp_cos_end;
                     cos_cur++) {
                     if((0 > lb_do_tx(lw, l_cur, c_cur, cos_cur,
                                      lp->lp_d_port,
                                      lp->lp_d_mod)) ||
                        (0 > lb_do_rx(lw, l_cur, c_cur, cos_cur))) {
                         return(-1);
                     }
                    if (lb_wait(lw, l_cur, c_cur) < 0) {
                        test_error(lw->lw_unit,
                                   "\tPort %s->%s\n"
                                   "\tPacket Length=%d bytes\n"
                                   "\tChain length=%d\n"
                                   "\tCurrent COS=%d\n"
                                   "\tCurrent Count (%d/%d)\n",
                                   SOC_PORT_NAME(lw->lw_unit,
                                                 lp->lp_tx_port),
                                   SOC_PORT_NAME(lw->lw_unit,
                                                 lp->lp_rx_port),
                                   l_cur, c_cur, cos_cur, c_count + 1,
                                   lp->lp_count);

                        for (i = 0; i < c_cur; i++) {
                            soc_dma_dump_pkt(lw->lw_unit, "LB TX ", lw->lw_tx_packets[i], l_cur, TRUE);
                        }

                        test_rv = -1;
                        goto done;
                    }
                    lb_stats_report(lw);
                }
            }
        }
    }

done:
    /* Verify port rx/tx stats */
    bcm_stat_sync(lw->lw_unit);
    if (!(IS_ST_PORT(lw->lw_unit, lp->lp_rx_port) || 
          IS_XE_PORT(lw->lw_unit, lp->lp_rx_port) ||
          IS_XL_PORT(lw->lw_unit, lp->lp_rx_port))) {
        if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_tx_port, 
                                 snmpIfOutUcastPkts, &tx_count)) < 0) {
            test_error(lw->lw_unit,
                       "Could not get port %s tx %s stats : %s\n",
                       SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
                       "snmpIfOutUcastPkts",
                       soc_errmsg(rv));
            return -1;
        }
        if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_rx_port, 
                                 snmpIfInUcastPkts, &rx_count)) < 0) {
            test_error(lw->lw_unit,
                       "Could not get port %s rx %s stats : %s\n",
                       SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                       "snmpIfInUcastPkts",
                       soc_errmsg(rv));
            return -1;
        }
        if (tx_count != rx_count) {
            test_error(lw->lw_unit,
                       "TX/RX packet stats mismatch: (%s) TX=%d, (%s) RX=%d\n",
                       SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
                       (int) tx_count,
                       SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                       (int) rx_count);
            test_rv = -1;
        }
    }

    /* Check for port TX error statistics */
    if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_tx_port, 
                             snmpIfOutErrors, &tx_count)) < 0) {
        test_error(lw->lw_unit,
                   "Could not get port %s tx error %s stats : %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
                   "snmpIfOutErrors",
                   soc_errmsg(rv));
        return -1;
    }
    if (tx_count) {
        test_error(lw->lw_unit,
                   "TX error stats found: TX (%s) error count=%d\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_tx_port),
                   (int) tx_count);
        test_rv = -1;
    }

    /* Check for port RX error statistics 
     * Use individual stat to count rx errors instead of snmpIfInErrors 
     * which includes good oversize packets.
     */
    if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_rx_port, 
                             snmpEtherStatsUndersizePkts, &rx_count0)) < 0) {
        test_error(lw->lw_unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   "snmpEtherStatsUndersizePkts",
                   soc_errmsg(rv));
        return -1;
    }
    if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_rx_port, 
                             snmpEtherStatsFragments, &rx_count1)) < 0) {
        test_error(lw->lw_unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   "snmpEtherStatsFragments",
                   soc_errmsg(rv));
        return -1;
    }
    if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_rx_port, 
                             snmpEtherStatsCRCAlignErrors, &rx_count2)) < 0) {
        test_error(lw->lw_unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   "snmpEtherStatsCRCAlignErrors",
                   soc_errmsg(rv));
        return -1;
    }
    if ((rv = bcm_stat_get32(lw->lw_unit, lp->lp_rx_port, 
                             snmpEtherStatsJabbers, &rx_count3)) < 0) {
        test_error(lw->lw_unit,
                   "Could not get port %s rx %s stats : %s\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   "snmpEtherStatsJabbers",
                   soc_errmsg(rv));
        return -1;
    }
    rx_count = rx_count0 + rx_count1 + rx_count2 + rx_count3;
    if (rx_count) {
        test_error(lw->lw_unit,
                   "RX error stats found: RX (%s) error count=%d\n",
                   SOC_PORT_NAME(lw->lw_unit, lp->lp_rx_port),
                   (int) rx_count);
        test_rv = -1;
    }

    return(test_rv);
}

int
lb_dma_run_common(int unit, loopback_test_t *lw, loopback_testdata_t *lp)
/*
 * Function:    dma_test
 * Purpose:     Common code for simple interrupt driven DMA test.
 * Parameters:  <see usage above>
 * Returns:     0 - success
 *              -1 - failed
 */
{
    int rx_chan, tx_chan, chan;
#if defined(BCM_FIRELIGHT_SUPPORT)
    int big_pio, big_packet, big_other;
#endif /* BCM_FIRELIGHT_SUPPORT */

    /*
     * For each of the DMA channel configurations, run the entire test.
     */

    lw->lw_lp = lp;                     /* Assign test # */

    lb_stats_init(lw);

    lp->lp_tx_port = CMIC_PORT(unit);
    lp->lp_rx_port = CMIC_PORT(unit);

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        soc_endian_get(unit, &big_pio, &big_packet, &big_other);
        /* Set Rx EP_TO_CPU_HDR endian same as DCB endian for chan 0 */
        soc_cmicx_pktdma_hdr_endian_set(unit, 0, 0, big_other);
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    for (rx_chan = 0; rx_chan < chan; rx_chan++) {
#ifdef INCLUDE_KNET
        if (SOC_KNET_MODE(unit)) {
            /* Currently KNET uses chan 1 for rx */
            if (rx_chan != KCOM_DMA_RX_CHAN) {
                continue;
            }
        }
#endif /* INCLUDE_KNET */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Configuring RX-channel: %d\n"), rx_chan));
        if (soc_dma_chan_config(unit, rx_chan, DV_RX, 
                                SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
            test_error(unit, "Unable to configure RX channel: %d\n", rx_chan);
            break;
        }
#ifdef BCM_CMICM_SUPPORT
        /* Assign all COS to this channel
         * Though this is added for CMICm, 
         * it should be OK if we do this for CMICe as well */
        if(soc_feature(unit, soc_feature_cmicm)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Assign all COS to channel: %d\n"), rx_chan));
            BCM_IF_ERROR_RETURN(
                soc_rx_queue_channel_set(unit, -1, rx_chan));

#if defined(BCM_KATANA2_SUPPORT)
            if(SOC_IS_KATANA2(unit)) {
                uint32 reg_val, reg_addr;

                /* Including CPU L0 bitmap to support CMIC backpressure */
                reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, rx_chan);
                reg_val = soc_pci_read(unit, reg_addr);
                reg_val |= (1 << 16);
                soc_pci_write(unit, reg_addr, reg_val);
            }
#endif
        }
#endif

#if defined(BCM_RAVEN_SUPPORT)
       if(SOC_IS_RAVEN(unit)) {
            uint32 val;
            /* Increase the dynamic cell limit for cpu port */
            SOC_IF_ERROR_RETURN(READ_DYNCELLLIMITr(unit, 0, &val));
            if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLSETLIMITf)) {
               soc_reg_field_set(unit, DYNCELLLIMITr, &val,DYNCELLSETLIMITf, 0x1644);
            }
			
            if (soc_reg_field_valid(unit, DYNCELLLIMITr, DYNCELLRESETLIMITf)) {
               soc_reg_field_set(unit, DYNCELLLIMITr, &val,  DYNCELLRESETLIMITf, 0x162c);
            }
            SOC_IF_ERROR_RETURN(WRITE_DYNCELLLIMITr(unit, 0, val));
        }
#endif
#ifdef BCM_CMICX_SUPPORT
        /* Assign all COS to this channel */
        if(soc_feature(unit, soc_feature_cmicx)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Assign all COS to channel: %d\n"), rx_chan));
            BCM_IF_ERROR_RETURN(
                soc_rx_queue_channel_set(unit, -1, rx_chan));
        }
#endif

        for (tx_chan = 0; tx_chan < chan; tx_chan++) {
#ifdef INCLUDE_KNET
            if (SOC_KNET_MODE(unit)) {
                /* Currently KNET uses chan 0 for tx */
                if (tx_chan != KCOM_DMA_TX_CHAN) {
                    continue;
                }
            }
#endif /* INCLUDE_KNET */
            if (rx_chan == tx_chan) {   /* Does not make sense */
                continue;
            }
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Configuring TX-channel: %d\n"), tx_chan));
            if (soc_dma_chan_config(unit, tx_chan,
                                    DV_TX, SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
                test_error(unit,
                           "Unable to configure TX channel: %d\n",
                           tx_chan);
                break;
            }
            if (lb_do_txrx(lw) < 0) {
                return -1;
            }

            if (soc_dma_chan_config(unit, tx_chan, DV_NONE, SOC_DMA_F_INTR)) {
                test_error(unit,
                           "Unable to de-configure TX channel: %d\n",
                           tx_chan);
                break;
            }
        }

        if (soc_dma_chan_config(unit, rx_chan,
                                DV_NONE, SOC_DMA_F_INTR)) {
            test_error(unit,
                       "Unable to de-configure RX channel: %d\n", rx_chan);
            break;
        }
    }

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        /*
         * Restore Tx header endian for chan 0 after testing :
         * set Tx header endian same as packet DMA endian for chan 0.
         * Let it be the same config programmed in soc_endian_config.
         */
        soc_cmicx_pktdma_hdr_endian_set(unit, 0, 0, big_packet);
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    lb_stats_done(lw);

    return 0;
}


int
lb_dma_testapi_run(int unit, loopback_test_t *lw, loopback_testdata_t *lp)
{
/*
 * Function:    dma_test
 * Purpose:     Perform simple interrupt driven DMA test via API.
 * Parameters:  <see usage above>
 * Returns:     0 - success
 *              -1 - failed
 */
    return lb_dma_run_common(unit, lw, lp);
}

int
lb_dma_test(int unit, args_t *a, void *pa)
/*
 * Function:    dma_test
 * Purpose:     Perform simple interrupt driven DMA test via CLI.
 * Parameters:  <see usage above>
 * Returns:     0 - success
 *              -1 - failed
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = &lw->lw_lp_dma;
    COMPILER_REFERENCE(a);
    lb_stats_init(lw);
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit)) {
            if (lb_setup_static_path(lw, unit, CMIC_PORT(unit), CMIC_PORT(unit))) {
                return(-1);
            }
        }
        if (SOC_IS_SHADOW(unit)) {
            port_tab_entry_t ptab;
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY,
                                        PORT_BITMAPf, 0x1ffff));
            /* Disable VLAN checks */
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                            MEM_BLOCK_ANY, 0, &ptab));
            if (soc_mem_field_valid(unit, PORT_TABm, DISABLE_VLAN_CHECKSf)) {
                soc_PORT_TABm_field32_set(unit, &ptab, DISABLE_VLAN_CHECKSf, 1);
            }
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                                0, &ptab));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                            REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 1));
        }
#endif

    lw->lw_tx_dcb_factor = 1;

    return lb_dma_run_common(unit, lw, lp);
}

STATIC int
lb_set_mac_addr(int unit, soc_port_t p, uint32 vlan, bcm_l2_addr_t *arl,
                sal_mac_addr_t mac_addr)
/*
 * Function:    lb_set_arl_mac_addr
 * Purpose:     Set a mac address for a port.
 * Parameters:  unit - SOC unit #
 *              p - port to set address for.
 *              vlan - vlan tag to use in ARL entry.
 *              arl - pointer to arl entry to use.
 * Returns:     0 - success, -1 failed.
 */
{
    int rv = BCM_E_UNAVAIL;

    
    /*
       To avoid flipping external vs internal L2 memory settings 
       for XGS3 devices the same path can be configured using 
       l2_cache_XXX API
    */
#if defined(BCM_ESW_SUPPORT)
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
#if defined(BCM_TRIUMPH_SUPPORT)
        bcm_l2_cache_addr_t l2caddr;
        int index;

        rv = bcm_l2_cache_delete_all(unit);
        if (rv < 0) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            } /* else, non-existent entry */
        }

        sal_memset(&l2caddr, 0, sizeof(bcm_l2_cache_addr_t));
        l2caddr.flags = BCM_L2_STATIC;
        l2caddr.vlan = vlan;
        sal_memcpy(l2caddr.mac, mac_addr, sizeof(sal_mac_addr_t));
        l2caddr.src_port = p;

        rv = bcm_l2_cache_set(unit, -1, &l2caddr ,&index); 
#endif
    }	else 
#endif    
    {
        /*
         * Prior XGS3 don't have external L2 memory so it is 
         * safe to use bcm_l2_XXX API 
         */
        rv = bcm_l2_addr_delete(unit, mac_addr, (vlan_id_t)vlan);
        if (rv < 0) {
            if (rv != BCM_E_NOT_FOUND) {
                return rv;
            } /* else, non-existent entry */
        }

        sal_memcpy(arl->mac, mac_addr, sizeof(sal_mac_addr_t));
        arl->vid = vlan;
        arl->flags = BCM_L2_STATIC;
        arl->port = p;
        rv = bcm_l2_addr_add(unit, arl);
    }

    return rv;
}

STATIC void
lb_cleanup_arl(loopback_test_t *lw, int unit)
/*
 * Function:    lb_cleanup_arl
 * Purpose:     Clean up arl entries inserted after a test.
 * Parameters:  lw - pointer to work
 *              unit  - SOC unit #
 * Returns:     Nothing
 */
{
    loopback_testdata_t *lp = lw->lw_lp;
    sal_mac_addr_t  cur_mac;
    int         c_cur;
#ifdef BCM_TRIUMPH_SUPPORT
    int ext_index_max = -1;
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        ext_index_max = SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if (lw->lw_arl_src_delete) {
        ENET_SET_MACADDR(cur_mac, lw->lw_mac_src);

        for (c_cur = 0; c_cur < lp->lp_c_end; c_cur++) {
            (void)bcm_l2_addr_delete(unit, cur_mac,
                                     (vlan_id_t)lw->lw_lp->lp_vlan);
            increment_macaddr(cur_mac, lp->lp_mac_src_inc);
        }

        lw->lw_arl_src_delete = FALSE;
    }

    if (lw->lw_arl_dst_delete) {
        ENET_SET_MACADDR(cur_mac, lw->lw_mac_dst);

        for (c_cur = 0; c_cur < lp->lp_c_end; c_cur++) {
            (void)bcm_l2_addr_delete(unit, cur_mac,
                                     (vlan_id_t)lw->lw_lp->lp_vlan);
            increment_macaddr(cur_mac, lp->lp_mac_dst_inc);
        }

        lw->lw_arl_dst_delete = FALSE;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = ext_index_max;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
}

#ifdef BCM_SHADOW_SUPPORT
/*
 * Function:    lb_setup_static_path
 * Purpose:     Setup a static path for the specified port
 * Parameters:  unit - unit #
 *              port - port to which address should map
 *              src_port - port under test
 * Returns:     0 - success
 *              -1 - failed.
 * Notes:       Only the destination port CBIT is returned, as only
 *              it is required for the test.
 */
static int
lb_setup_static_path(loopback_test_t *lw, int unit, soc_port_t port,
                     soc_port_t src_port)
{
    loopback_testdata_t *lp = lw->lw_lp;
    int         rv = 0;
    uint32 rval = 0;

    ENET_SET_MACADDR(lp->lp_mac_src, lw->lw_mac_src);
    ENET_SET_MACADDR(lp->lp_mac_dst, lw->lw_mac_dst);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Setting up static path for port %s\n"),
              SOC_PORT_NAME(unit, src_port)));

    /* Get the module ID for this unit */
    if (SOC_IS_SHADOW(unit)) {
        lw->lw_arl_dst.modid = 0;
    } else if (SOC_IS_XGS_SWITCH(unit)) {
        if ((rv = bcm_stk_my_modid_get(unit, &lw->lw_arl_dst.modid)) < 0) {
            test_error(lw->lw_unit,
                       "Could not get modid: %s\n", bcm_errmsg(rv));
            return(-1);
        }
        if (SOC_PORT_MOD1(unit, port)) {
            lw->lw_arl_dst.modid += 1;
        }
    }

    rv = READ_UFLOW_PORT_CONTROLr(unit, src_port, &rval);
    if (rv < 0) {
        return(-1);
    }
    soc_reg_field_set(unit, UFLOW_PORT_CONTROLr, &rval, HASH_MODEf, 0);
    soc_reg_field_set(unit, UFLOW_PORT_CONTROLr, &rval, DEST_PORTf, port);
    rv = WRITE_UFLOW_PORT_CONTROLr(unit, src_port, rval);

    lw->lw_arl_dst_delete = TRUE;

    if (rv < 0) {
        test_error(lw->lw_unit,
                   "Failed to set static path: port %s\n",
                   SOC_PORT_NAME(lw->lw_unit, port));
        return(-1);
    } else if (lw->lw_arl_dst.port != CMIC_PORT(unit)) {
        test_error(lw->lw_unit,
                   "Port %s: invalid destination port: %s\n",
                   SOC_PORT_NAME(lw->lw_unit, port),
                   SOC_PORT_NAME(lw->lw_unit, lw->lw_arl_dst.port));
        return(-1);
    }

    return(0);
}
#endif /* BCM_SHADOW_SUPPORT */

/*
 * Function:    lb_setup_arl
 * Purpose:     Setup an ARL entry for the specified port
 * Parameters:  unit - unit #
 *              port - port to which address should map
 *              src_port - port under test
 * Returns:     0 - success
 *              -1 - failed.
 * Notes:       Only the destination port CBIT is returned, as only
 *              it is required for the test.
 */
int
lb_setup_arl(loopback_test_t *lw, int unit, soc_port_t port, 
             soc_port_t src_port)
{
    loopback_testdata_t *lp = lw->lw_lp;
    int         rv = 0;
    sal_mac_addr_t  cur_mac;
    int c_cur;

    ENET_SET_MACADDR(lw->lw_mac_src, lp->lp_mac_src);
    ENET_SET_MACADDR(lw->lw_mac_dst, lp->lp_mac_dst);

    if (IS_ST_PORT(unit, src_port)) {
        /* Don't install L2 entry if port is HG */
        return(0);
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Setting up ARL for port %s\n"),
              SOC_PORT_NAME(unit, src_port)));

    ENET_SET_MACADDR(cur_mac, lw->lw_mac_dst);

    /* Get the module ID for this unit */
    if (SOC_IS_XGS_SWITCH(unit)) {
        if ((rv = bcm_stk_my_modid_get(unit, &lw->lw_arl_dst.modid)) < 0) {
            test_error(lw->lw_unit,
                       "Could not get modid: %s\n", bcm_errmsg(rv));
            return(-1);
        } 
        if (SOC_PORT_MOD1(unit, port)) {
            lw->lw_arl_dst.modid += 1;
        }
    }

    for (c_cur = 0; c_cur < lp->lp_c_end; c_cur++) {
        rv |= lb_set_mac_addr(unit, port, lw->lw_lp->lp_vlan,
                              &lw->lw_arl_dst, cur_mac);
        increment_macaddr(cur_mac, lp->lp_mac_dst_inc);
    }

    lw->lw_arl_dst_delete = TRUE;

    if (rv) {
        test_error(lw->lw_unit,
                   "Failed to set MAC address: port %s\n",
                   SOC_PORT_NAME(lw->lw_unit, port));
        return(-1);
    } else if (lw->lw_arl_dst.port != CMIC_PORT(unit)) {
        test_error(lw->lw_unit,
                   "Port %s: invalid destination arl port: %s\n",
                   SOC_PORT_NAME(lw->lw_unit, port),
                   SOC_PORT_NAME(lw->lw_unit, lw->lw_arl_dst.port));
        return(-1);
    }

    return(0);
}

STATIC   int
lb_done(loopback_test_t *lw)
/*
 * Function:    lb_done
 * Purpose:     Generic loopback done function
 * Parameters:  lw - pointer to lw to clean up.
 * Returns:
 */
{
    int         rv = 0;
    dma_chan_t  c, chan;
    loopback_testdata_t *lp = lw->lw_lp;

    if(soc_feature(lw->lw_unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    (void)soc_dma_abort(lw->lw_unit);   /* Abort all dma */

    if (lw->lw_sema) {
        sal_sem_destroy(lw->lw_sema);
        lw->lw_sema = NULL;
    }

    if (lp) {
        lb_deallocate(lw->lw_unit, 
                      lp->lp_dv_end, &lw->lw_rx_dv,
                      lp->lp_ppc_end, &lw->lw_rx_packets, NULL, DV_RX);

        lb_deallocate(lw->lw_unit, 
                      lp->lp_dv_end, &lw->lw_tx_dv,
                      lp->lp_ppc_end, &lw->lw_tx_packets,  &lw->lw_tx_dcbs, DV_TX);
    }

    /* Clean up ARL if interrupted in middle */

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(lw->lw_unit)) {
    } else
#endif
    {
        lb_cleanup_arl(lw, lw->lw_unit);
    }

    /* ENABLE ALL PORTS */

    if (lp) {
        rv |= lb_restore_port(lw);
    }

	if (SOC_DPP_PP_ENABLE(lw->lw_unit) && SOC_IS_ARAD(lw->lw_unit)) {
	    /* RESTORE VLAN */
	    if (lp) {
	        rv |= lb_restore_vlan(lw);
	    }
	}
        if (SOC_IS_KATANAX(lw->lw_unit) && (SAL_BOOT_QUICKTURN)) {
	    if (lp) {
                cli_out("QUICKTURN:: ATTN : Restoring VLAN \n");
	        rv |= lb_restore_vlan(lw);
	    }
        }
    /*
     * Deconfigure all DMA channels.
     * Doing this aborts any DMAs that may be left over.
     */
    for (c = 0; c < chan; c++) {
        if (soc_dma_chan_config(lw->lw_unit, c, DV_NONE, 0) < 0) {
            rv = -1;
        }
    }

    /*
     * Leave channels configured in default state at end of test.
     */
    rv |= soc_dma_init(lw->lw_unit);

    /*
     * Clear L2 cache entries (if used)
     */
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (l2_index >= 0) {
        bcm_l2_cache_delete(lw->lw_unit, l2_index);
        l2_index = -1;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return(rv);
}

STATIC int
lb_init(int unit, loopback_test_t *lw, loopback_testdata_t *lp, int external)
/*
 * Function:    lb_init
 * Purpose:     Loopback generic initialization
 * Parameters:  unit - unit number to init.
 *              lw - pointer to lw to init.
 *              a - args from test cmd
 *              external - true if actual link up required
 * Returns:     0 on success, -1 on failure
 */
{
    soc_port_t          port;
    int                 rv;
    pbmp_t              pbm;

    lw->lw_lp = lp;

    lw->lw_rx_fill = soc_property_get(unit, spn_DIAG_LB_FILL_RX, 0);

    if (NULL == (lw->lw_sema = sal_sem_create("lb-sema", sal_sem_BINARY, 0))) {
        return (-1);
    }

    lw->lw_sema_woke = FALSE;
    lw->lw_timeout_usec = lb_timeout_usec(unit);

    /* Try to allocate required memory */

    lw->lw_tx_packet_cnt = lp->lp_ppc_end;
    lw->lw_rx_packet_cnt = lp->lp_ppc_end;

    if (lb_allocate(unit, DV_RX, lp->lp_dv_end, lp->lp_c_end, &lw->lw_rx_dv,
                    lp->lp_ppc_end, lp->lp_l_end, &lw->lw_rx_packets, NULL) ||
        lb_allocate(unit, DV_TX, lp->lp_dv_end, lp->lp_c_end, &lw->lw_tx_dv,
                    lp->lp_ppc_end, lp->lp_l_end, &lw->lw_tx_packets, &lw->lw_tx_dcbs)) {
        test_error(unit, "Unable to allocate DCB and/or packet memory\n");
        lb_done(lw);
    }

    lw->lw_tx_seq = 0;

    lw->lw_tx_init_chn = 0;            /* Start off clean */
    lw->lw_tx_init_len = 0;

    lw->lw_tx_started = 0;
    lw->lw_rx_started = 0;

    SOC_PBMP_CLEAR(lw->lw_save_vlan_pbmp);
    SOC_PBMP_CLEAR(lw->lw_save_vlan_ubmp);
    lw->lw_save_vlan_stg = 0;
    lw->lw_save_vlan_valid = 0;

#ifdef BCM_XGS12_FABRIC_SUPPORT
    /* Setup H/W specific offsets */
    if (SOC_IS_XGS12_FABRIC(unit)) {
        lw->lw_ether_start = SOC_HIGIG_HDR_SIZE;
    } else {
        lw->lw_ether_start = 0; /* No XGS header */
    }
#else
    lw->lw_ether_start = 0;
#endif

    /* Save all the current port states */

    if (lb_save_port(lw) < 0) {
        return (-1);
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
		
	    if (lb_save_vlan(lw) < 0) {
	        return (-1);
	    }
    }
    SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    SOC_PBMP_AND(pbm, lp->lp_pbm);
	if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {

	    if (!SOC_IS_SHADOW(unit)) {
	        PBMP_ITER(pbm, port) {
	            if (!external &&
	                (rv = bcm_port_stp_set(unit, port,
	                                       BCM_STG_STP_FORWARD)) < 0) {
	                test_error(unit,
	                           "Unable to set port %s in forwarding state: %s\n",
	                           SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
	                return -1;
	            }
	        }
	    }
	}
    return 0;
}

/*
 * Adjust FE_MAXFr, MAXFRr, and GTH_MAXFr as necessary, to
 * accommodate requested packet size.
 */
static int
_lb_max_frame_set(loopback_test_t *lw, loopback_testdata_t *lp)
{
    pbmp_t          pbm;
    int             max_pkt, max_set;
    soc_port_t      port;
    int             max_adj = 0;
    int             jumbo_adj = 0;
    int             unit = lw->lw_unit;

    max_pkt = lp->lp_l_start;

    if (max_pkt < lp->lp_l_end) {
        max_pkt = lp->lp_l_end;
    }

    if (max_pkt > LB_JUMBO_MAXSZ) {
        max_pkt = LB_JUMBO_MAXSZ;
    }

    if (SOC_IS_SHADOW(unit)) {
        return SOC_E_NONE;
    }
    BCM_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    BCM_PBMP_AND(pbm, lp->lp_pbm);
    BCM_PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_port_frame_max_get(unit, port, &max_set));
        if (max_set < max_pkt) {
            BCM_IF_ERROR_RETURN
                (bcm_port_frame_max_set(unit, port, max_pkt));
            max_adj = 1;
        }
    }

    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, lp->lp_pbm);
    PBMP_ITER(pbm, port) {
        if (max_pkt > 1536) {
            if (soc_feature(unit, soc_feature_trimac)) {
#if defined(BCM_ESW_SUPPORT)
				
                uint32          gmacc1;

                SOC_IF_ERROR_RETURN(READ_GMACC1r(unit, port, &gmacc1));

                if (! soc_reg_field_get(unit, GMACC1r, gmacc1, JUMBOf)) {
                    soc_reg_field_set(unit, GMACC1r, &gmacc1, JUMBOf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_GMACC1r(unit, port, gmacc1));
                    jumbo_adj = 1;
                }
#endif				
            } 
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAVEN_SUPPORT)            
            else if (soc_feature(unit, soc_feature_unimac)) {
                if (SOC_BLOCK_TYPE(unit, SOC_PORT_BLOCK(unit, 
                    SOC_INFO(unit).port_l2p_mapping[port])) != SOC_BLK_XLPORT) {
                        SOC_IF_ERROR_RETURN(WRITE_FRM_LENGTHr(unit, 
                                  port, LB_JUMBO_MAXSZ));
                }
            }
#endif /* BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */
        }
    }

    if (max_adj) {
        cli_out("NOTICE: Increased max frame size for %d-byte packets\n",
                max_pkt);
    }

    if (jumbo_adj) {
        cli_out("NOTICE: Enabled JUMBO frames on GE\n");
    }
    return(0);
}

int 
lb_check_parms(loopback_test_t *lw, loopback_testdata_t *lp, pbmp_t pbm_ok)
/*
 * Function:    lb_check_parms
 * Purpose:     Check parameters for reasonable values.
 * Parameters:  lw - pointer to current work structure.
 *              lp - pointer to parameters.
 *              pbm_ok - allowable ports for test
 * Returns:     0 - OK
 *              -1 - failed.
 */
{
    int         i, nLen, nChain, nCOS, nPort, nChanConf;
    int         unit = lw->lw_unit;
    pbmp_t      pbm;
    char        pfmt[SOC_PBMP_FMT_LEN];
    int         num_cos, num_ports;
    
    if(soc_feature(unit, soc_feature_cmicx)) {
        nChanConf = ((lp == &lw->lw_lp_dma) ? CMICX_CHAN_CONFIG_COUNT : 1);
    } else {
    nChanConf = ((lp == &lw->lw_lp_dma) ? CHAN_CONFIG_COUNT : 1);
    }

    if (lp->lp_l_start < 24) {
        test_error(unit,
                   "Packet start length %d too small\n",
                   lp->lp_l_start);
        return(-1);
    }

    if (bcm_cosq_config_get(unit, &num_cos) < 0 || num_cos == 0) {
        num_cos = 1;
    }

    if (lp->lp_cos_start >= num_cos || lp->lp_cos_end >= num_cos) {
        LOG_WARN(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit,
                             "***NOTICE: COS %d-%d requested, but only %d COS configured\n"),
                  lp->lp_cos_start, lp->lp_cos_end, num_cos));
    }

    if ((lp != &lw->lw_lp_dma) || (SOC_IS_XGS3_SWITCH(unit))) {
        _lb_max_frame_set(lw, lp);
    }

    if (lp->lp_l_inc <= 0 || lp->lp_c_inc <= 0) {
        test_error(unit, "Increments must be > 0\n");
        return(-1);
    }

    SOC_PBMP_ASSIGN(pbm, lp->lp_pbm);
    SOC_PBMP_REMOVE(pbm, pbm_ok);
    if (SOC_PBMP_NOT_NULL(pbm)) {
        test_error(unit,
                   "Invalid bits in port bitmap.\n"
                   "Allowable ports are %s\n",
                   SOC_PBMP_FMT(pbm_ok, pfmt));
        return(-1);
    }

    if (lp->lp_cos_start < 0 ||
        lp->lp_cos_end < lp->lp_cos_start ||
        lp->lp_cos_end >= NUM_COS(unit)) {
        test_error(unit,
                   "Invalid COS values: start=%d end=%d\n",
                   lp->lp_cos_start, lp->lp_cos_end);
        return(-1);
    }

    if (lp->lp_check_crc && lp->lp_crc_mode == CRC_MODE_CPU_NONE) {
        test_error(unit, "Can't check CRC if not generating it\n");
        return(-1);
    }
#if defined(BCM_XGS12_FABRIC_SUPPORT)

    if (SOC_IS_XGS12_FABRIC(unit)) {
        if (lp->lp_d_mod < soc_mem_index_min(unit, MEM_UCm) ||
             lp->lp_d_mod > soc_mem_index_max(unit, MEM_UCm)) {
            test_error(unit,
                       "Invalid Module ID selection: %d\n",
                       lp->lp_d_mod);
            return (-1);
        }
    }
#endif
    nLen = (lp->lp_l_end - lp->lp_l_start) / lp->lp_l_inc + 1;
    nCOS = (lp->lp_cos_end - lp->lp_cos_start) / 1 + 1;
    SOC_PBMP_COUNT(lp->lp_pbm, nPort);

    if (lp == &lw->lw_lp_ext ||
        (lp == &lw->lw_lp_sg_dma &&
         (lb_is_xgs_fabric(unit) || SOC_IS_XGS3_SWITCH(unit)))) {
        assert(nPort % 2 == 0);
        nPort /= 2;
    }

    if (SOC_IS_XGS_SWITCH(unit) &&
        ((lp->lp_speed == LB_SPEED_100FD) ||
         (SOC_IS_RAPTOR(unit)) ||
         (lp->lp_speed == LB_SPEED_10FD))) {
        int ppce = 0; 
        int lene = lp->lp_l_end;

        BCM_IF_ERROR_RETURN(bcm_cosq_config_get(unit, &num_cos));
        num_ports = NUM_ALL_PORT(unit);
        SOC_IF_ERROR_RETURN(lb_ppce_initval(unit, &lene, &ppce));
 
        if (lp->lp_l_end > lene) {
            test_error(unit,
                   "Packet length (%d) too Large. Try (%d)\n"
                   "\twith current configuration of %d COS, %d ports.\n",
                   lp->lp_l_end, lene, num_cos, num_ports);
            return (-1);
        }        
        if ((lp->lp_ppc_end > ppce) &&
            ((lp->lp_speed == LB_SPEED_100FD) ||
             (lp->lp_speed == LB_SPEED_10FD))) {
            test_error(unit,
                   "Too many packets per chain (%d) for maximum length: %d\n"
                   "\twith current configuration of %d COS, %d ports.\n",
                   lp->lp_ppc_end, lp->lp_l_end, num_cos, num_ports);
            return (-1);
        }        
    }

    nChain = 0;

    if (lp->lp_sg) {
        int ppc, dpp, dpp_rx;
        for (ppc = lp->lp_ppc_start; ppc <= lp->lp_ppc_end;
             ppc += lp->lp_ppc_inc) {
            for (dpp = lp->lp_dpp_start; dpp <= lp->lp_dpp_end;
                 dpp += lp->lp_dpp_inc) {
                for (dpp_rx = lp->lp_dpp_start; dpp_rx <= lp->lp_dpp_end;
                     dpp_rx += lp->lp_dpp_inc) {
                    nChain += ppc;      /* Tx and Rx loop over these */
                }
            }
        }
        nChain *= lp->lp_sg ? 4 : 1;    /* For alignment */

        if(soc_feature(unit, soc_feature_cmicx)) {
            nChanConf = CMICX_CHAN_CONFIG_COUNT;  /* For DMA channel combinations */
        } else {
        nChanConf = CHAN_CONFIG_COUNT;  /* For DMA channel combinations */
        }
    } else {
        for (i = lp->lp_c_start; i <= lp->lp_c_end; i += lp->lp_c_inc) {
            nChain += i;
        }
    }


    lw->lw_tx_total = nLen * nChain * nCOS * nPort * lp->lp_count * 
        nChanConf;
    test_msg("LB: total %d pkt (%d len "
             "* %d chain * %d COS * %d port * %d count * %d chan)\n",
             lw->lw_tx_total,
             nLen, nChain, nCOS, nPort, lp->lp_count, nChanConf);

    return(0);
}



int
lb_dma_init_parse(int unit, args_t* a)
/*
 * Function:    lb_dma_init_parse
 * Purpose:     Initialize DMA loopback test via parsing args.
 * Parameters:  unit - unit number.
 *              a - pointer to arguments.
 *              p - pointer passed to test function.
 * Returns:     0 for success, -1 for failed.
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_dma;
    parse_table_t       pt;

    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL, 0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "CHainStart",  PQ_INT|PQ_DFL,  0,
                    &lp->lp_c_start,NULL);
    parse_table_add(&pt, "CHainEnd",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_c_end,      NULL);
    parse_table_add(&pt, "CHainIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_c_inc,      NULL);
    parse_table_add(&pt, "COSStart",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_start,NULL);
    parse_table_add(&pt, "COSEnd",      PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_end,NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data,NULL);

    lb_setup(unit, lw);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

return 0;
}


int
lb_dma_common_init(int unit,
                   loopback_test_t  *lw,
                   loopback_testdata_t *lp)
{
    int rv = 0;
    dma_chan_t          c, chan;

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    SOC_PBMP_CLEAR(lp->lp_pbm);
    SOC_PBMP_PORT_ADD(lp->lp_pbm, CMIC_PORT(unit));
    lp->lp_rx_port = CMIC_PORT(unit);

    /* Packets Per Chain == DCB Per chain for non-S/G */

    lp->lp_ppc_start = lp->lp_c_start;
    lp->lp_ppc_end   = lp->lp_c_end;
    lp->lp_ppc_inc   = lp->lp_c_inc;

    lp->lp_sg = FALSE;

    if ((rv = lb_check_parms(lw, lp, lp->lp_pbm)) < 0) {
        return(rv);
    }

    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, FALSE)) {
        return(-1);
    }

    /* Set up MAC addresses (currently constant) */

    ENET_SET_MACADDR(lw->lw_mac_src, lb_mac_src);
    ENET_SET_MACADDR(lw->lw_mac_dst, lb_mac_dst);

    /*
     * Deconfigure all DMA channels.
     * Doing this aborts any DMAs that may be left over.
     */
    for (c = 0; c < chan; c++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "lb_dma_init: Deconfiguring channel %d\n"), c));
        if (soc_dma_chan_config(unit, c, DV_NONE, 0)) {
            rv = -1;
        }
    }

    if (rv) {
        lb_done(lw);
    }
    return rv;
}

int
lb_dma_init(int unit, args_t *a, void **p)
/*
 * Function:    lb_dma_init
 * Purpose:     Initialize DMA loopback test.
 * Parameters:  unit - unit number.
 *              a - pointer to arguments.
 *              p - pointer passed to test function.
 * Returns:     0 for success, -1 for failed.
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_dma;
    int                 rv = 0;
    
    /* Continuous DMA mode not supported, skip test */
    if (soc_feature(unit, soc_feature_continuous_dma) &&
        soc_property_get(unit, spn_PDMA_CONTINUOUS_MODE_ENABLE, 0)) {
        test_msg("Unit %d : Continuous DMA mode not supported, "
                  "skipping test\n", unit);
        /* return UNAVAIL, upper level test infra checks for
         * SOC_E_UNAVAIL to skip test */
        return -16;
    }

    if ((rv = lb_dma_init_parse(unit, a)) < 0) {
        return rv;
    }
    rv = lb_dma_common_init(unit, lw, lp);
    *p = lw;                            /* Pass back cookie */
    if (rv < 0) {
        return rv;
    }
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (!SOC_IS_SHADOW(unit) && soc_feature(unit, soc_feature_l2_user_table)) {
        bcm_l2_cache_addr_t l2_cache_addr;
        int dmod;
        int rv;

        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(unit, &dmod));

        bcm_l2_cache_addr_t_init(&l2_cache_addr);
        l2_cache_addr.flags = BCM_L2_CACHE_CPU;
        l2_cache_addr.dest_modid = dmod;
        l2_cache_addr.dest_port = CMIC_PORT(unit);
        /*
         * Valid index in the my station tcam  will be mostly 
         * 4 or 8 incase of emulator.So hardcoding the index to 3 as by
         * default we use two index.
         */
        if (SAL_BOOT_QUICKTURN) {
            l2_index = 3; 
        }
        if ((rv = bcm_l2_cache_set(unit, l2_index, &l2_cache_addr,
                                   &l2_index)) < 0) {
            test_error(lw->lw_unit,
                       "Failed to configure L2 User Entry Table %s\n",
                       bcm_errmsg(rv));
            return(rv);
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return rv;
}


int lb_dma_testapi_init(int unit,
                        uint32 pattern,
                        uint32 patternIncrement,
                        int startLength,
                        int endLength,
                        int lengthIncrement,
                        int chainStart,
                        int chainEnd,
                        int chainIncrement,
                        int cosStart,
                        int cosEnd,
                        int countPackets,
                        int checkData)
/*
 * Function:    lb_dma_testapi_init
 * Purpose:     Initialize DMA loopback test API call.
 * Parameters:  unit - unit number.
 *              pattern
 *              patternIncrement
 *              startLength
 *              endLength
 *              lengthIncrement
 *              chainStart
 *              chainEnd
 *              chainIncrement
 *              cosStart
 *              cosEnd
 *              countPackets
 *              checkData
 * Returns:     0 for success, -1 for failed.
 */
{

    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_dma;

    lb_setup(unit, lw);
    
    lp->lp_pattern = pattern;
    lp->lp_pattern_inc = patternIncrement;
    lp->lp_l_start = startLength;
    lp->lp_l_end = endLength;
    lp->lp_l_inc = lengthIncrement;
    lp->lp_c_start = chainStart;
    lp->lp_c_end = chainEnd;
    lp->lp_c_inc = chainIncrement;

    return lb_dma_common_init(unit, lw, lp);
}
                    


int
lb_dma_done(int unit, void *pa)
/*
 * Function:    lb_dma_done
 * Purpose:     Clean up after DMA loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    COMPILER_REFERENCE(pa);

    /* De-configure channels */
    lb_done(&lb_work[unit]);
#if defined(BCM_ESW_SUPPORT)
	
    if (SOC_IS_SHADOW(unit)) {
        port_tab_entry_t ptab;
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EPC_LINK_BMAPr, REG_PORT_ANY,
                                    PORT_BITMAPf, 0x1ffff));
        /* Restore VLAN checks to default */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm,
                        MEM_BLOCK_ANY, 0, &ptab));
        if (soc_mem_field_valid(unit, PORT_TABm, DISABLE_VLAN_CHECKSf)) {
            soc_PORT_TABm_field32_set(unit, &ptab, DISABLE_VLAN_CHECKSf, 0);
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                            0, &ptab));
        /* Thdi bypass is restored */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                        REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 0));
    }
#endif
    return(0);
}

int
lb_dma_testapi_done(int unit)
/*
 * Function:    lb_dma_done
 * Purpose:     Clean up after DMA loopback test API call.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    lb_done(&lb_work[unit]);
    return 0;
}

int
lb_mac_init(int unit, args_t *a, void **pa)
/*
 * Function:    lb_mac_init
 * Purpose:     Initialize MAC loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
  
    loopback_test_t     *lw = &lb_work[unit];
    loopback_testdata_t *lp = &lw->lw_lp_mac;
    soc_port_t          p;
    parse_table_t       pt;
    int                 rv;
    pbmp_t              temp1_pbmp, temp2_pbmp;
    uint16              dev_id;
    uint8               rev_id;
    uint8               flag = 0;
    bcm_port_abil_t     ability;

    soc_cm_get_id(unit, &dev_id, &rev_id);	
                                                                              
    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_PBMP|PQ_DFL, 0,
                    &lp->lp_pbm,        NULL);
    if (lb_is_xgs_fabric(unit)) {
        parse_table_add(&pt, "DestMOD",     PQ_DFL|PQ_INT,  0,
                        &lp->lp_d_mod,      NULL);
        parse_table_add(&pt, "DestPORT",    PQ_DFL|PQ_PORT, 0,
                        &lp->lp_d_port,     NULL);
        parse_table_add(&pt, "OpCode",    PQ_DFL|PQ_INT, 0,
                        &lp->lp_opcode,     NULL);
    } else {
        parse_table_add(&pt, "UntagBitMap",     PQ_PBMP|PQ_DFL, 0,
                        &lp->lp_ubm,    NULL);
    }
    parse_table_add(&pt, "Speed",       PQ_MULTI|PQ_DFL,0,
                    &lp->lp_speed,      lb_parse_speed);
    parse_table_add(&pt, "SourceMac",   PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_src,    NULL);
    parse_table_add(&pt, "SourceMacInc",PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_src_inc,NULL);
    parse_table_add(&pt, "DestMac",     PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_dst,    NULL);
    parse_table_add(&pt, "DestMacInc",  PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_dst_inc,NULL);

    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL, 0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "VLantag",     PQ_INT|PQ_DFL,  0,
                    &lp->lp_vlan,       NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "COSStart",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_start,NULL);
    parse_table_add(&pt, "COSEnd",      PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_end,NULL);
    parse_table_add(&pt, "CRC",         PQ_MULTI|PQ_DFL, 0,
                    &lp->lp_crc_mode, crc_mode);
    parse_table_add(&pt, "PacketsPerChain",PQ_INT|PQ_DFL, 0,
                    &lp->lp_c_start,    NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data,NULL);
    parse_table_add(&pt, "CheckCrc",    PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_crc,NULL);

    lb_setup(unit, lw);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

    /* Packets Per Chain == DCB Per chain for non-S/G */

    lp->lp_ppc_start = lp->lp_c_start;
    lp->lp_ppc_end   = lp->lp_c_end = lp->lp_c_start;
    lp->lp_ppc_inc   = lp->lp_c_inc;

    lp->lp_sg = FALSE;

    if (lb_check_parms(lw, lp, PBMP_PORT_ALL(unit))) {
        return(-1);
    }


    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, FALSE)) {
        return(-1);
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
        if (!SOC_IS_SHADOW(unit)) {
	    if ((rv = bcm_vlan_create(unit, lp->lp_vlan)) < 0 &&
	        rv != BCM_E_EXISTS) {
	        test_error(unit,
	                  "Could not create VLAN %d: %s\n",
	                   lp->lp_vlan, bcm_errmsg(rv));
	        return (-1);
            }
        }
    }    
    SOC_PBMP_CLEAR(temp1_pbmp);
    SOC_PBMP_CLEAR(temp2_pbmp);
    SOC_PBMP_OR(temp1_pbmp, PBMP_PORT_ALL(unit));
    SOC_PBMP_OR(temp1_pbmp, PBMP_CMIC(unit));
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_PBMP_ASSIGN(temp2_pbmp, PBMP_PORT_ALL(unit));
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
        if (!SOC_IS_SHADOW(unit)) {
	    if ((rv = bcm_vlan_port_add(unit, lp->lp_vlan,
	                                temp1_pbmp, temp2_pbmp)) < 0) {
	        test_error(unit,
	                   "Could not add all ports to VLAN %d: %s\n",
	                   lp->lp_vlan, bcm_errmsg(rv));
	        return (-1);
            }
        }
    }

    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
        /* Set the spanning tree group to 1 since port state is forwarding */
        if (!SOC_IS_SHADOW(unit)) {
            rv = bcm_vlan_stg_set(unit, lp->lp_vlan, 1);
            if (rv < 0 && rv != BCM_E_UNAVAIL) {
                test_error(unit, "Could not set vlan %d stg 1: %s\n",
                           lp->lp_vlan, bcm_errmsg(rv));
                return(-1);
            }
        }
    }
    /*
     * Be sure all DMA channels ignore port-bit-map modification.
     *|

    if (((rv = soc_dma_chan_config(unit, 0, DV_TX, 
                                   SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) < 0) ||
        ((rv = soc_dma_chan_config(unit, 1, DV_RX, 
                                   SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) < 0)) {
        test_error(unit,
                   "soc_dma_chan_config: Failed: %s\n",
                   soc_errmsg(rv));
        lb_done(lw);
        return(-1);
    } */
#ifdef BCM_CMICM_SUPPORT
    /* Assign all COS to this channel
     * Though this is added for CMICm, 
     * it should be OK if we do this for CMICe as well */
    if(soc_feature(unit, soc_feature_cmicm)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));

#if defined(BCM_KATANA2_SUPPORT)
            if(SOC_IS_KATANA2(unit)) {
                uint32 reg_val, reg_addr;

                /* Including CPU L0 bitmap to support CMIC backpressure */
                reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, 1);
                reg_val = soc_pci_read(unit, reg_addr);
                reg_val |= (1 << 16);
                soc_pci_write(unit, reg_addr, reg_val);
            }
#endif

    }
#endif
#ifdef BCM_CMICX_SUPPORT
    /* Assign all COS to this channel */
    if(soc_feature(unit, soc_feature_cmicx)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));
    }
#endif

    /* Before we set up the ports, rip out the stack port(s) */
    if (!SOC_IS_SHADOW(unit)) {
        if ((rv = lb_stack_check(unit)) < 0) {
            test_error(unit,
                       "Unable to shut down stacking mode: %s\n",
                       bcm_errmsg(rv));
            lb_done(lw);                    /* In case they continue */
            return(-1);
        }
    }

    PBMP_ITER(lp->lp_pbm, p) {
        if(dev_id == BCM56443_DEVICE_ID) {  
            ability = 0;
            flag = 0;
        }
        /*
         * Set loopback mode before speed, since it may affect
         * the allowable speeds.
         */
        if (IS_HG_PORT(unit, p)) {
            /*
             *      Disabling pause reuiqred on IPIC/XPORT
             *      HG ports. Should already be disabled on
             *      GE (stack) ports during init.
             */
            bcm_port_pause_set(unit, p, FALSE, FALSE);
        }
        if(dev_id == BCM56443_DEVICE_ID) {
            if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
                test_error(unit, "===========> ERROR : fail to get ability\n");
                return -1;
            }
            switch(lp->lp_speed) {
                case 1:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_10MB))) {
                        flag = 1;
                    }
                    break;
                case 2:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_100MB))) {
                        flag = 1;
                    }
                    break;
                case 3:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_1000MB))) {
                        flag = 1;
                    }
                    break;
                case 4:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_2500MB))) {
                        flag = 1;
                    }
                    break;
                default :
                    flag = 0;
                    break;

            }
            if (flag)
                continue;
        }

        if ((rv = lb_setup_port(unit, p, lp->lp_speed, FALSE)) < 0) {
            test_error(unit,
                       "Port %s: Port setup failed: %s\n",
                       SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
            lb_done(lw);                        /* In case they continue */
            return(-1);
        }

        if ((rv = bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_MAC)) < 0) {
            test_error(unit,
                       "Port %s: Failed to set MAC loopback: %s\n",
                       SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
            lb_done(lw);                        /* In case they continue */
            return(-1);
        }
    }

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        pbmp_t          pbm;

        /*
         * If there is an external cable looped back, 5670 will
         * receive packets on ports that may not be in the test
         * and these packets will go to the CPU and interfere.
         */

        SOC_PBMP_ASSIGN(pbm, lp->lp_pbm);
        SOC_PBMP_XOR(pbm, PBMP_PORT_ALL(unit));
        PBMP_ITER(pbm, p) {
            if ((rv = bcm_port_enable_set(unit, p, FALSE)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set MAC enable: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                lb_done(lw);
                return(-1);
            }
        }
    }
#endif
    
    *pa = lw;

    return(0);
}

int
lb_mac_done(int unit, void *pa)
/*
 * Function:    lb_mac_done
 * Purpose:     Clean up after MAC loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    soc_port_t  p;

    if (pa) {
        loopback_testdata_t *lp = ((loopback_test_t *)pa)->lw_lp;

        /* Clear MAC loopback - Only if pa is set */

        PBMP_ITER(lp->lp_pbm, p) {
            int         rv;

            rv = bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_NONE);
            if (rv != BCM_E_NONE) {
                test_error(unit,
                           "Port %s: Failed to reset MAC loopback: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
            }
            if (IS_HG_PORT(unit, p)) {
                bcm_port_pause_set(unit, p, TRUE, TRUE);
            }
        }
    }

    lb_done(&lb_work[unit]);

    return(0);
}


/*ARGSUSED*/
int
lb_mac_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_mac_test
 * Purpose:     Perform MAC loopback tests.
 * Parameters:  unit - unit #
 *              a - arguments (Not used, assumed parse by lb_mac_init).
 * Returns:
 * NOTE:  Reference: BCM569X Errata Sheet 
 *       On 5695A0, port 5 falsely trigger flow-control mechanisms
 *       when the CPU has transmitted packets.  
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = lw->lw_lp;
    int                  port, rv;
    pbmp_t               pbm, tpbm;
    uint16               dev_id;
    uint8                rev_id;
    uint8                flag = 0;
    bcm_port_abil_t      ability;

    soc_cm_get_id(unit, &dev_id, &rev_id);	
    
    COMPILER_REFERENCE(a);


    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "lb_mac_test: unit %d starting ....\n"), lw->lw_unit));

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                            REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 1));
    }
#endif /* BCM_SHADOW_SUPPORT */

    lb_stats_init(lw);

    SOC_PBMP_ASSIGN(pbm, lp->lp_pbm);
    SOC_PBMP_ASSIGN(tpbm, lp->lp_pbm);
    SOC_PBMP_AND(tpbm, PBMP_FE_ALL(unit));

    PBMP_ITER(pbm, port) {
        if(dev_id == BCM56443_DEVICE_ID) {  
            ability = 0;
            flag = 0;
        }
        /*
         * Get our current MAC address for this port and be sure the
         * SOC will route packets to the CPU. 
         */
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit)) {
            if (lb_setup_static_path(lw, unit, CMIC_PORT(unit), port)) {
                return(-1);
            }

        } else
#endif /* BCM_SHADOW_SUPPORT */
        {
            if (lb_setup_arl(lw, unit, CMIC_PORT(unit), port)) {
                return(-1);
            }
        }

        lw->lw_tx_init_chn = 0;         /* Start off clean */
        lw->lw_tx_init_len = 0;         /* Start off clean */

        if(soc_feature(unit, soc_feature_cmicx)) {
            /* We send 2 dcbs per packet as the header is no longer part of dcb */
            lw->lw_tx_dcb_factor = 2;
        } else {
            lw->lw_tx_dcb_factor = 1;
        }

        if(dev_id == BCM56443_DEVICE_ID) {
            if ( bcm_port_ability_get(unit, port, &ability) != BCM_E_NONE) {
                test_error(unit, "===========> ERROR : fail to get ability\n");
                return -1;
            }
            switch(lp->lp_speed) {
                case 1:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_10MB))) {
                        flag = 1;
                    }
                    break;
                case 2:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_100MB))) {
                        flag = 1;
                    }
                    break;
                case 3:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_1000MB))) {
                        flag = 1;
                    }
                    break;
                case 4:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_2500MB))) {
                        flag = 1;
                    }
                    break;
                default :
                    flag = 0;
                    break;

            }
            if (flag)
                continue;
        }
        lp->lp_tx_port = port;
        lp->lp_rx_port = port;


        rv = lb_do_txrx(lw);

        if (!IS_ST_PORT(unit, port)) {
            lb_cleanup_arl(lw, unit);
        }
        if (rv < 0) {
            pbmp_t  portpbm;
            BCM_PBMP_CLEAR(portpbm);
            BCM_PBMP_PORT_ADD(portpbm, port);
#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
            if (SOC_IS_DFE(unit) || SOC_IS_DPP(unit)) {
                do_show_dpp_counters(unit, INVALIDr, portpbm,
                               SHOW_CTR_CHANGED | SHOW_CTR_SAME | SHOW_CTR_NZ);
            } else 
#elif defined (BCM_ESW_SUPPORT) 
            {
                do_show_counters(unit, INVALIDr, portpbm,
                               SHOW_CTR_CHANGED | SHOW_CTR_SAME | SHOW_CTR_NZ);
            }
#endif
            return -1;
        }
    }

    lb_stats_done(lw);
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                            REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 0));
    }
#endif /* BCM_SHADOW_SUPPORT */

    return(0);
}

int
lb_mii_init(int unit, args_t *a, void **pa)
/*
 * Function:    lb_mii_init
 * Purpose:     Initialize DMA-mii loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_phy;
    soc_port_t          p;
    parse_table_t       pt;
    int                 rv;
    pbmp_t              temp1_pbmp, temp2_pbmp, phy_pbmp;
    int                 rcpu;
    uint16              dev_id;
    uint8               rev_id;
    uint8               flag = 0;
    bcm_port_abil_t     ability;

    soc_cm_get_id(unit, &dev_id, &rev_id);	

    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_PBMP|PQ_DFL, 0,
                    &lp->lp_pbm,        NULL);
    parse_table_add(&pt, "ExtselfloopPBM",  PQ_PBMP|PQ_DFL, 0,
                    &lp->lp_ext_self_loop_pbm,        NULL);
    if (lb_is_xgs_fabric(unit)) {
        parse_table_add(&pt, "DestMOD",     PQ_DFL|PQ_INT,  0,
                        &lp->lp_d_mod,      NULL);
        parse_table_add(&pt, "DestPORT",    PQ_DFL|PQ_PORT, 0,
                        &lp->lp_d_port,     NULL);
        parse_table_add(&pt, "OpCode",    PQ_DFL|PQ_INT, 0,
                        &lp->lp_opcode,     NULL);
    } else {
        parse_table_add(&pt, "UntagBitMap",     PQ_PBMP|PQ_DFL, 0,
                        &lp->lp_ubm,    NULL);
    }
    parse_table_add(&pt, "Speed",       PQ_MULTI|PQ_DFL,0,
                    &lp->lp_speed,      lb_parse_speed);
    parse_table_add(&pt, "SourceMac",   PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_src,    NULL);
    parse_table_add(&pt, "SourceMacInc",PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_src_inc,NULL);
    parse_table_add(&pt, "DestMac",     PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_dst,    NULL);
    parse_table_add(&pt, "DestMacInc",  PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_dst_inc,NULL);

    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,    NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL, 0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "VLantag",     PQ_INT|PQ_DFL,  0,
                    &lp->lp_vlan,       NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,    NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "COSStart",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_start,  NULL);
    parse_table_add(&pt, "COSEnd",      PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_end,    NULL);
    parse_table_add(&pt, "CRC",         PQ_MULTI|PQ_DFL, 0,
                    &lp->lp_crc_mode,   crc_mode);
    parse_table_add(&pt, "PacketsPerChain",PQ_INT|PQ_DFL, 0,
                    &lp->lp_c_start,    NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data, NULL);
    parse_table_add(&pt, "CheckCrc",    PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_crc,  NULL),

        lb_setup(unit, lw);

    lw->lw_lp = lp;

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

    /* Packets Per Chain == DCB Per chain for non-S/G */

    lp->lp_ppc_start = lp->lp_c_start;
    lp->lp_ppc_end   = lp->lp_c_end = lp->lp_c_start;
    lp->lp_ppc_inc   = lp->lp_c_inc;

    lp->lp_sg = FALSE;

    if (lb_check_parms(lw, lp, PBMP_PORT_ALL(unit))) {
        return(-1);
    }

    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, FALSE)) {
        return(-1);
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
	if ((rv = bcm_vlan_create(unit, lp->lp_vlan)) < 0 &&
	    rv != BCM_E_EXISTS) {
	    test_error(unit,
	               "Could not create VLAN %d: %s\n",
	               lp->lp_vlan, bcm_errmsg(rv));
	    return (-1);
        }
    }    
    SOC_PBMP_CLEAR(temp1_pbmp);
    SOC_PBMP_CLEAR(temp2_pbmp);
    SOC_PBMP_OR(temp1_pbmp, PBMP_PORT_ALL(unit));
    SOC_PBMP_OR(temp1_pbmp, PBMP_CMIC(unit));
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_PBMP_ASSIGN(temp2_pbmp, PBMP_PORT_ALL(unit));
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {

	if ((rv = bcm_vlan_port_add(unit, lp->lp_vlan,
	                            temp1_pbmp, temp2_pbmp)) < 0) {
	    test_error(unit,
	               "Could not add all ports to VLAN %d: %s\n",
	               lp->lp_vlan, bcm_errmsg(rv));
	    return (-1);
        }

        rv = bcm_vlan_stg_set(unit, lp->lp_vlan, 1);
        if (rv < 0 && rv != BCM_E_UNAVAIL) {
            test_error(unit, "Could not set vlan %d stg 1: %s\n",
                       lp->lp_vlan, bcm_errmsg(rv));
            return(-1);
        }
    }
    
    /*
     * Be sure all DMA channels ignore port-bit-map modification.
     */

    if (((rv = soc_dma_chan_config(unit, 0, DV_TX, 
                                   SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) < 0) ||
        ((rv = soc_dma_chan_config(unit, 1, DV_RX, 
                                   SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) < 0)) {
        test_error(unit,
                   "soc_dma_chan_config: Failed: %s\n",
                   soc_errmsg(rv));
        lb_done(lw);
        return(-1);
    }
#ifdef BCM_CMICM_SUPPORT
    /* Assign all COS to this channel
     * Though this is added for CMICm, 
     * it should be OK if we do this for CMICe as well */
    if(soc_feature(unit, soc_feature_cmicm)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));

#if defined(BCM_KATANA2_SUPPORT)
            if(SOC_IS_KATANA2(unit)) {
                uint32 reg_val, reg_addr;

                /* Including CPU L0 bitmap to support CMIC backpressure */
                reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, 1);
                reg_val = soc_pci_read(unit, reg_addr);
                reg_val |= (1 << 16);
                soc_pci_write(unit, reg_addr, reg_val);
            }
#endif

    }
#endif
#ifdef BCM_CMICX_SUPPORT
    /* Assign all COS to this channel */
    if(soc_feature(unit, soc_feature_cmicx)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));
    }
#endif

    /* Before we set up the ports, rip out the stack port(s) */
    if ((rv = lb_stack_check(unit)) < 0) {
        test_error(unit,
                   "Unable to shut down stacking mode: %s\n",
                   bcm_errmsg(rv));
        lb_done(lw);                    /* In case they continue */
        return(-1);
    }

    PBMP_ITER(lp->lp_pbm, p) {
        if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
            test_error(unit,"======> ERROR : fail to get ability Port %s\n", SOC_PORT_NAME(unit, p));
        }
        if (!(ability & BCM_PORT_ABIL_LB_PHY)) {
            SOC_PBMP_PORT_REMOVE(lp->lp_pbm, p);
            test_msg("Port %s: Doesn't support PHY loopback. Skip it\n", SOC_PORT_NAME(unit, p));
        }
    }

    /*
     * Setup the PHY loopback for the ports which are in the lp_pbm but
     * are not in the lb_ext_self_loop_lpm. lb_ext_self_loop_pbm contains
     * the map of the ports which have external self loopback cable connected
     * to then and hence no need to put then in the PHY loopback mode.
     */
    SOC_PBMP_ASSIGN(phy_pbmp, lp->lp_pbm);
    SOC_PBMP_NEGATE(temp1_pbmp, lp->lp_ext_self_loop_pbm);
    SOC_PBMP_AND(phy_pbmp, temp1_pbmp);

    rcpu = soc_property_get(unit, spn_RCPU_PORT, -1);
    PBMP_ITER(phy_pbmp, p) {
        if(dev_id == BCM56443_DEVICE_ID) {  
            ability = 0;
            flag = 0;
        }
        if (rcpu !=0 && rcpu != -1 && SOC_IS_RAVEN(unit)) { 
            if ((rv = bcm_port_loopback_set(unit, p,
                                            BCM_PORT_LOOPBACK_PHY)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set PHY loopback: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                lb_done(lw);
                return(-1);                 /* In case they continue */
            }
            if ((rv = lb_setup_port(unit, p, lp->lp_speed, FALSE)) < 0) {
                test_error(unit,
                           "Port %s: Port setup failed: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                lb_done(lw);
                return(-1);
            }
        } else {
              if(dev_id == BCM56443_DEVICE_ID) {
                  if ( bcm_port_ability_get(unit, p, &ability) != BCM_E_NONE) {
                      test_error(unit,"======> ERROR : fail to get ability\n");
                      return -1;
                  }
                  switch(lp->lp_speed) {
                     case 1:
                         if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_10MB))) {
                             flag = 1;
                         }
                         break;
                     case 2:
                         if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_100MB))) {
                             flag = 1;
                         }
                         break;
                     case 3:
                         if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_1000MB))) {
                             flag = 1;
                         }
                         break;
                     case 4:
                         if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_2500MB))) {
                             flag = 1;
                         }
                         break;
                     default :
                         flag = 0;
                         break;

                  }
                  if (flag)
                      continue;
             }

            if ((rv = lb_setup_port(unit, p, lp->lp_speed, FALSE)) < 0) {
                test_error(unit,
                           "Port %s: Port setup failed: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                lb_done(lw);
                return(-1);
            }
            if ((rv = bcm_port_loopback_set(unit, p,
                                            BCM_PORT_LOOPBACK_PHY)) < 0) {
                test_error(unit,
                           "Port %s: Failed to set PHY loopback: %s\n",
                           SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                lb_done(lw);
                return(-1);                 /* In case they continue */
            }
        }
        sal_sleep(1);
    }

    *pa = lw;

    return(0);
}

int
lb_mii_done(int unit, void *pa)
/*
 * Function:    lb_mii_done
 * Purpose:     Clean up after DMA loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{

    lb_done(&lb_work[unit]);
    return(0);
}

int
lb_mii_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_mii_test
 * Purpose:     Perform phy loopback tests.
 * Parameters:  unit - unit #
 *              a - arguments
 * Returns:     0 - success, -1 failed. 
 * NOTE:  Reference: BCM569X Errata Sheet 
 *       On 5695A0, port 5 falsely trigger flow-control mechanisms
 *       when the CPU has transmitted packets.  
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = lw->lw_lp;
    int                  port, rv;
    uint16               dev_id;
    uint8                rev_id;
    uint8                flag = 0;
    bcm_port_abil_t      ability;

    soc_cm_get_id(unit, &dev_id, &rev_id);	
    
    COMPILER_REFERENCE(a);


#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                            REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 1));
    }
#endif /* BCM_SHADOW_SUPPORT */
    lb_stats_init(lw);

    /* #################################################################### */
    /* With below wait, tr 19 passes even if tried several times */
    /* Believe it is applicable for other chips but don't want to take risk */
    /* #################################################################### */
    if (SOC_IS_KATANA2(unit) || SOC_IS_FIRELIGHT(unit) || SOC_IS_FIREBOLT6(unit)) {
        if (lb_link_wait(unit, lp->lp_pbm) < 0) {
            return(-1);
        }
    }


    PBMP_ITER(lp->lp_pbm, port) {
        if(dev_id == BCM56443_DEVICE_ID) {  
            ability = 0;
            flag = 0;
        }
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit)) {
            if (lb_setup_static_path(lw, unit, CMIC_PORT(unit), port)) {
                return(-1);
            }

        } else 
#endif /* BCM_SHADOW_SUPPORT */
        {
            if (!lb_is_xgs_fabric(unit)) {
                /*
                 * Get our current MAC address for this port and be sure the
                 * SOC will route packets to the CPU.
                 */
                if (lb_setup_arl(lw, unit, CMIC_PORT(unit), port)) {
                    return(-1);
                }
            }
        }
        if(dev_id == BCM56443_DEVICE_ID) {
            if ( bcm_port_ability_get(unit, port, &ability) != BCM_E_NONE) {
                test_error(unit, "===========> ERROR : fail to get ability\n");
                return -1;
            }
            switch(lp->lp_speed) {
                case 1:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_10MB))) {
                        flag = 1;
                    }
                    break;
                case 2:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_100MB))) {
                        flag = 1;
                    }
                    break;
                case 3:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_1000MB))) {
                        flag = 1;
                    }
                    break;
                case 4:
                    if (!CHECK_PBM_SPEED(ability, (BCM_PORT_ABIL_2500MB))) {
                        flag = 1;
                    }
                    break;
                default :
                    flag = 0;
                    break;

            }
            if (flag)
                continue;
        }
        lp->lp_tx_port = port;
        lp->lp_rx_port = port;

        lw->lw_tx_init_chn = 0;
        lw->lw_tx_init_len = 0;

        if(soc_feature(unit, soc_feature_cmicx)) {
            /* We send 2 dcbs per packet as the header is no longer part of dcb */
            lw->lw_tx_dcb_factor = 2;
        } else {
            lw->lw_tx_dcb_factor = 1;
        }

        rv = lb_do_txrx(lw);

        if (!lb_is_xgs_fabric(unit) && !IS_ST_PORT(unit, port)) {
            lb_cleanup_arl(lw, unit);
        }

        if (rv < 0) {
            pbmp_t  portpbm;
            BCM_PBMP_CLEAR(portpbm);
            BCM_PBMP_PORT_ADD(portpbm, port);
#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)
            if (SOC_IS_DFE(unit) || SOC_IS_DPP(unit)) {
                do_show_dpp_counters(unit, INVALIDr, portpbm,
                               SHOW_CTR_CHANGED | SHOW_CTR_SAME | SHOW_CTR_NZ);
            } else 
#elif defined (BCM_ESW_SUPPORT) 
            {
                do_show_counters(unit, INVALIDr, portpbm,
                               SHOW_CTR_CHANGED | SHOW_CTR_SAME | SHOW_CTR_NZ);
            }
#endif
            return -1;
        }
    }

    lb_stats_done(lw);
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, THDI_BYPASSr,
                            REG_PORT_ANY, INPUT_THRESHOLD_BYPASSf, 0));
    }
#endif /* BCM_SHADOW_SUPPORT */

    return(0);
}

/*
 * Rotate port number by dst_inc.
 * Handles positive or negative dst_inc.
 */
STATIC soc_port_t
lb_ext_partner(int unit, soc_port_t port, int dst_inc)
{
    pbmp_t      bm;
    int         nports, pindex, nindex;
    soc_port_t  p;

    if (IS_FE_PORT(unit, port)) {
        nports = NUM_FE_PORT(unit);
        SOC_PBMP_ASSIGN(bm, PBMP_FE_ALL(unit));
    } else if (IS_GE_PORT(unit, port)) {
        nports = NUM_GE_PORT(unit);
        SOC_PBMP_ASSIGN(bm, PBMP_GE_ALL(unit));
    } else if (IS_XE_PORT(unit, port)) {
        nports = NUM_XE_PORT(unit);
        SOC_PBMP_ASSIGN(bm, PBMP_XE_ALL(unit));
    } else if (IS_ST_PORT(unit, port)) {
        nports = NUM_ST_PORT(unit);
        SOC_PBMP_ASSIGN(bm, PBMP_ST_ALL(unit));
    } else {
        return port;
    }

    /* short circuit trivial cases */
    if (dst_inc == 0 || nports < 2) {
        return port;
    }

    /* find index of old port */
    pindex = -1;
    PBMP_ITER(bm, p) {
        pindex += 1;
        if (p == port) {
            break;
        }
    }

    /* increment/decrement index with wrap around */
    nindex = (pindex + dst_inc) % nports;
    if (nindex < 0) {           /* handle C doing modulo or remainder for % */
        nindex = nports + nindex;
    }

    /* short circuit another trivial case */
    if (nindex == pindex) {
        return port;
    }

    /* find nindex'th port */
    PBMP_ITER(bm, p) {
        if (--nindex < 0) {
            return p;
        }
    }

    /* can't get here */
    return port;
}

int
lb_ext_init(int unit, args_t *a, void **pa)
/*
 * Function:    lb_ext_init
 * Purpose:     Initialize external loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_ext;
    parse_table_t       pt;
    soc_port_t          port;
    pbmp_t              pbmp_partners, tpbm;
    int                 rv;
    pbmp_t              temp_pbmp, temp_pbmp2;
    bcm_port_medium_t   medium;


    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "TxPortBitMap",PQ_PBMP|PQ_DFL, 0,
                    &lp->lp_pbm_tx,     NULL);
    parse_table_add(&pt, "DestinationIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_dst_inc,    NULL);
    if (lb_is_xgs_fabric(unit)) {
        parse_table_add(&pt, "DestMOD",     PQ_DFL|PQ_INT,  0,
                        &lp->lp_d_mod,      NULL);
        parse_table_add(&pt, "DestPORT",    PQ_DFL|PQ_PORT, 0,
                        &lp->lp_d_port,     NULL);
        parse_table_add(&pt, "OpCode",    PQ_DFL|PQ_INT, 0,
                        &lp->lp_opcode,     NULL);
    } else {
        parse_table_add(&pt, "UntagBitMap",     PQ_PBMP|PQ_DFL, 0,
                        &lp->lp_ubm,    NULL);
    }
    parse_table_add(&pt, "Speed",       PQ_MULTI|PQ_DFL, 0,
                    &lp->lp_speed,      lb_parse_speed);
    parse_table_add(&pt, "AutoNeg",     PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_autoneg,    NULL);
    parse_table_add(&pt, "SourceMac",   PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_src,    NULL);
    parse_table_add(&pt, "SourceMacInc",PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_src_inc,NULL);
    parse_table_add(&pt, "DestMac",     PQ_DFL|PQ_MAC,  0,
                    &lp->lp_mac_dst,    NULL);
    parse_table_add(&pt, "DestMacInc",  PQ_DFL|PQ_INT,  0,
                    &lp->lp_mac_dst_inc,NULL);

    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,    NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL, 0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "VLantag",     PQ_INT|PQ_DFL,  0,
                    &lp->lp_vlan,       NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,    NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL, 0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "COSStart",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_start,  NULL);
    parse_table_add(&pt, "COSEnd",      PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_end,    NULL);
    parse_table_add(&pt, "CRC",         PQ_MULTI|PQ_DFL, 0,
                    &lp->lp_crc_mode,   crc_mode);
    parse_table_add(&pt, "PacketsPerChain",PQ_INT|PQ_DFL, 0,
                    &lp->lp_c_start,    NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data, NULL);
    parse_table_add(&pt, "CheckCrc",    PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_crc,  NULL);

    lb_setup(unit, lw);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

    /* Packets Per Chain == DCB Per chain for non-S/G */

    lp->lp_ppc_start = lp->lp_c_start;
    lp->lp_ppc_end   = lp->lp_c_end = lp->lp_c_start;
    lp->lp_ppc_inc   = lp->lp_c_inc;

    /* Determine other ports that are involved in the test */

    SOC_PBMP_CLEAR(pbmp_partners);

    PBMP_ITER(lp->lp_pbm_tx, port) {
        SOC_PBMP_PORT_ADD(pbmp_partners,
                          lb_ext_partner(unit, port, lp->lp_dst_inc));
    }

    SOC_PBMP_ASSIGN(tpbm, lp->lp_pbm_tx);
    SOC_PBMP_AND(tpbm, pbmp_partners);
    if (SOC_PBMP_NOT_NULL(tpbm)) {
        test_error(unit,
               "Loopback partners of specified ports "
               "may not overlap other specified ports\n");
        return(-1);
    }

    SOC_PBMP_ASSIGN(lp->lp_pbm, lp->lp_pbm_tx);         /* All TX+RX ports */
    SOC_PBMP_OR(lp->lp_pbm, pbmp_partners);

    /* Packets Per Chain == DCB Per chain for non-S/G */

    lp->lp_ppc_start = lp->lp_c_start;
    lp->lp_ppc_end   = lp->lp_c_end;
    lp->lp_ppc_inc   = lp->lp_c_inc;

    lp->lp_sg = FALSE;

    if (lb_check_parms(lw, lp, PBMP_PORT_ALL(unit))) {
        return(-1);
    }

    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, TRUE)) {
        return(-1);
    }
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {

	if ((rv = bcm_vlan_create(unit, lp->lp_vlan)) < 0 &&
	    rv != BCM_E_EXISTS) {
	    test_error(unit,
	               "Could not create VLAN %d: %s\n",
	               lp->lp_vlan, bcm_errmsg(rv));
	    return (-1);
        }
    }    
    SOC_PBMP_CLEAR(temp_pbmp);
    if (SOC_IS_XGS3_SWITCH(unit)) {
        SOC_PBMP_ASSIGN(temp_pbmp, PBMP_PORT_ALL(unit));
    }
    SOC_PBMP_ASSIGN(temp_pbmp2, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(temp_pbmp2, PBMP_LB(unit));
    if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
	if ((rv = bcm_vlan_port_add(unit, lp->lp_vlan,
	                            temp_pbmp2, temp_pbmp)) < 0) {
	    test_error(unit,
	               "Could not add all ports to VLAN %d: %s\n",
	               lp->lp_vlan, bcm_errmsg(rv));
	        return (-1);
        }
    }
    /* Set the spanning tree group to 1 since port state is forwarding */
    rv = bcm_vlan_stg_set(unit, lp->lp_vlan, 1);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        test_error(unit, "Could not set vlan %d stg 1: %s\n",
               lp->lp_vlan, bcm_errmsg(rv));
        return(-1);
    }

#ifdef BCM_CMICM_SUPPORT
    /* Assign all COS to this channel
     * Though this is added for CMICm, 
     * it should be OK if we do this for CMICe as well */
    if(soc_feature(unit, soc_feature_cmicm)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));

#if defined(BCM_KATANA2_SUPPORT)
            if(SOC_IS_KATANA2(unit)) {
                uint32 reg_val, reg_addr;

                /* Including CPU L0 bitmap to support CMIC backpressure */
                reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, 1);
                reg_val = soc_pci_read(unit, reg_addr);
                reg_val |= (1 << 16);
                soc_pci_write(unit, reg_addr, reg_val);
            }
#endif

    }
#endif
#ifdef BCM_CMICX_SUPPORT
    /* Assign all COS to this channel */
    if(soc_feature(unit, soc_feature_cmicx)) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Assign all COS to channel: 1\n")));
        SOC_IF_ERROR_RETURN(
            bcm_rx_queue_channel_set(unit, -1, 1));
    }
#endif

    PBMP_ITER(lp->lp_pbm_tx, port) {
        if ((rv = lb_setup_port(unit, port,
                                lp->lp_speed, lp->lp_autoneg)) < 0) {
            test_error(unit,
                       "Port %s: Port setup failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            lb_done(lw);                        /* In case they continue */
            return(-1);
        }

        /*
         * Put xmit ports in master mode and recv ports in slave mode
         * (actually only a requirement of gigabit ports)
         */

        if ((rv = bcm_port_medium_get(unit, port, &medium)) < 0 ) {
            test_error(unit,
                       "Port %s: Port medium get failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
        if (medium == BCM_PORT_MEDIUM_COPPER) {
            if (!lp->lp_autoneg &&
                (rv = bcm_port_master_set(unit, port,
                                      SOC_PORT_MS_MASTER)) < 0) {
                test_error(unit,
                       "Port %s: Could not set port master: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                lb_done(lw);                        /* In case they continue */
                return(-1);
            }
        }
    }

    PBMP_ITER(pbmp_partners, port) {
        if ((rv = lb_setup_port(unit, port,
                                lp->lp_speed, lp->lp_autoneg)) < 0) {
            test_error(unit,
                       "Port %s: Port setup failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            lb_done(lw);                        /* In case they continue */
            return(-1);
        }

        /*
         * Put xmit ports in master mode and recv ports in slave mode
         * (actually only a requirement of gigabit ports)
         */
        if ((rv = bcm_port_medium_get(unit, port, &medium)) < 0 ) {
            test_error(unit,
                       "Port %s: Port medium get failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
        if (medium == BCM_PORT_MEDIUM_COPPER) {
            if (!lp->lp_autoneg &&
                (rv = bcm_port_master_set(unit, port,
                                      SOC_PORT_MS_SLAVE)) < 0) {
                test_error(unit,
                       "Port %s: Could not set port master: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                lb_done(lw);                        /* In case they continue */
                return(-1);
            }
        }
    }

    /* Wait for links */

    if (lb_link_wait(unit, lp->lp_pbm) < 0) {
        lb_done(lw);
        return(-1);
    }

    *pa = lw;

    return(0);
}

int
lb_ext_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_mii_test
 * Purpose:     Perform ext loopback tests.
 * Parameters:  unit - unit #
 *              a - arguments
 * Returns:     0 - success, -1 failed.
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = lw->lw_lp;
    soc_port_t  port, dst_port;
    int         rv;

    COMPILER_REFERENCE(a);

    lb_stats_init(lw);

    PBMP_ITER(lp->lp_pbm_tx, port) {
        /*
         * Figure out RX port and add ARL entry to direct packets to CPU.
         */

        dst_port = lb_ext_partner(unit, port, lp->lp_dst_inc);

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "lb_ext_test: Looping port %s (%d) ---> %s (%d)\n"),
                  SOC_PORT_NAME(unit, port), port,
                  SOC_PORT_NAME(unit, dst_port), dst_port));
        if (!lb_is_xgs_fabric(unit)) {
            if (lb_setup_arl(lw, unit, CMIC_PORT(unit), port)) {
                return(-1);
            }
        }

        lp->lp_tx_port = port;
        lp->lp_rx_port = dst_port;

        lw->lw_tx_init_chn = 0;
        lw->lw_tx_init_len = 0;

        if(soc_feature(unit, soc_feature_cmicx)) {
            /* We send 2 dcbs per packet as the header is no longer part of dcb */
            lw->lw_tx_dcb_factor = 2;
        } else {
            lw->lw_tx_dcb_factor = 1;
        }

        rv = lb_do_txrx(lw);
        if (!lb_is_xgs_fabric(unit)) {
            lb_cleanup_arl(lw, unit);
        }
        if (rv < 0) {
            return -1;
        }
    }

    lb_stats_done(lw);

    return(0);
}

int
lb_ext_done(int unit, void *pa)
/*
 * Function:    lb_ext_done
 * Purpose:     Clean up after external loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    COMPILER_REFERENCE(pa);

    lb_done(&lb_work[unit]);
    return(0);
}

#undef PAUSE_SUPT

STATIC void
lb_setup_sg_dcbs(dv_t *dv, int l, int sg_c, int end_pkt, void *p, uint32 align, 
                 pbmp_t pbm, pbmp_t ubm, uint32 crc, uint32 cos,
                 uint32 *hgh)
/*
 * Function:    lb_setup_sg_dcbs
 * Purpose:     Set up the DCBs for a TX/RX
 * Parameters:  dv - Pointer to DV containing chain.
 *              l - total length of packet.
 *              sg_c - number of DCBs to scatter/gather across.
 *              end_pkt - if TRUE, sg bit is cleared on last DCB
 *              p - pointer to packet data.
 *              align - Power of 2 integer alignment required for
 *                      Address/length
 *              pbm - port Bit map to send to.
 *              ubm - Untag port bit map.
 *              crc - CRC value used for DCB.
 *              cos - COS value used in DCBS.
 *              hgh - Higig header for XGS3 devices
 * Returns:     Pointer to NEXT empty DCB.
 *
 * Notes:       Pass in if need to align len, and for Tx, can do weird
 *              lengths ...
 */
{
    int         dcb_l;                  /* Length per DCB */
    sal_vaddr_t pp = (sal_vaddr_t)p;
    uint32      flags = SOC_DMA_COS(cos) | (crc ? SOC_DMA_CRC_REGEN : 0);
    pbmp_t      l3pbm;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (hgh != NULL) {
        SOC_DMA_HG_SET(flags, 1);
        SOC_DMA_STATS_SET(flags, 1);
    }
#endif

    SOC_PBMP_CLEAR(l3pbm);

    dcb_l = l / sg_c;
    dcb_l &= -align;

    while (sg_c-- > 0) {
        int     cnt;

        if (sg_c) {
            cnt = dcb_l;
        } else {
            cnt = l;
        }
        soc_dma_desc_add(dv, pp, cnt, pbm, ubm, l3pbm, flags, (uint32 *)hgh);
        l -= dcb_l;
        pp += dcb_l;
    }

    if (end_pkt) {
        soc_dma_desc_end_packet(dv);
    }
}

STATIC  int
lb_sg_check_tx(loopback_test_t *lw, int ppc, int dpp, int length)
/*
 * Function:    lb_sg_check_tx
 * Purpose:     Check the status and correctness of a TX chain.
 * Parameters:  lw - pointer to work structure.
 *              ppc - TX packets-per-chain.
 *              dpp - if >0 DCBs-per-packet, if <0, DCBs-per-packet is
 *                      random.
 *              length - current packet length.
 * Returns:     0 - OK, chain not complete.
 *              1 - OK, chain complete
 *              -1- ERROR found.
 */

{
    int         rv = 0;
    dv_t        *dv = lw->lw_tx_dv[0];
    dcb_t       *start_dcb, *end_dcb, *cur_dcb;
    int         cur_packet, cur_len;    /* Current packet #/pckt length  */
    int         unit = lw->lw_unit;

    /* Check next DCB to look at */

    start_dcb  = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_dsc);
    cur_packet = dv->dv_pckt;                   /* Total packets in chain */

    /* Check for all packets we re transmitting */

    while (!rv && (cur_packet < ppc)) {
        assert(start_dcb < SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt));
        /*
         * Find the end of the current packet, be sure it is complete,
         * if not, escape to check later.
         */
        cur_len = 0;
        cur_dcb = start_dcb;

        /*
         * Find last DCB for current packet, if dpp is >0, then it
         * indicates number of DCBs, if <0, then random, and last DCB
         * must be scanned for.
         */

        if (dpp > 0) {
            end_dcb = start_dcb;
            end_dcb = SOC_DCB_IDX2PTR(unit, end_dcb, dpp - 1);
        } else {
            end_dcb = NULL;             /* Keep gcc happy */
            assert(0);                  /* Not implemented. */
        }

        assert(end_dcb <= SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt));

        if (!SOC_DCB_DONE_GET(unit, end_dcb)) {
            break;                      /* WHILE (cur_packet < ppc) */
        }

        while (cur_dcb <= end_dcb) {

            /* Verify DONE */

            if (!SOC_DCB_DONE_GET(unit, cur_dcb)) {
                cli_out("Error: TX: Expected done bit in DCB Pckt(%d)\n",
                        cur_packet);
                rv = -1;
            }

            /* Verify byte count */

            if (SOC_DCB_REQCOUNT_GET(unit, cur_dcb) != 
                SOC_DCB_XFERCOUNT_GET(unit, cur_dcb)) {
                cli_out("Error: TX: Unexpected count Packet %d: "
                        "Expected %d Got %d dcb=%p\n",
                        cur_packet, 
                        SOC_DCB_REQCOUNT_GET(unit, cur_dcb), 
                        SOC_DCB_XFERCOUNT_GET(unit, cur_dcb), 
                        cur_dcb);
                rv = -1;
            }

            cur_len += SOC_DCB_XFERCOUNT_GET(unit, cur_dcb);
            cur_dcb = SOC_DCB_IDX2PTR(unit, cur_dcb, 1);  /* Next DCB please */
        } /* while (cur_dcb <= end_dcb */

        /*
         * One final check - verify total length is what we want.
         * This should be an assert, but useful for debug.
         */
        if (cur_len != length) {
            cli_out("Error: TX: Unexpected total length: "
                    "Expected %d Got %d\n", length, cur_len);
            rv = -1;
        }
        cur_packet++;                   /* Next packet please */
        lw->lw_tx_count++;              /* Count packet */
        lw->lw_tx_bytes += length;      /*  ... and total bytes */
        start_dcb = SOC_DCB_IDX2PTR(unit, end_dcb, 1); /* Next packet please */
    } /* while */

    dv->dv_pckt = cur_packet;
    dv->dv_dsc  = SOC_DCB_PTR2IDX(unit, start_dcb, dv->dv_dcb);

    if (rv < 0) {
        soc_dma_dump_dv(lw->lw_unit, "TX err", dv);
        test_error(lw->lw_unit, "TX Error detected\n");
        return(rv);
    } else {
        return((cur_packet == ppc) && lw->lw_eoc_rx);
    }
}

STATIC  int
lb_sg_check_rx(loopback_test_t *lw, int ppc, int dpp, int length,
               int tx_offset, int rx_offset)
/*
 * Function:    lb_sg_check_rx
 * Purpose:     Check an RX chain for S/G tests.
 * Parameters:  lw -    pointer to work structure.
 *              ppc -   packets-per-chain, total number of packets to
 *                      expect in the DCB chain.
 *              dpp -   dcbs-per-packet, number of DCBs per packet,
 *                      if -1, then #dcbs is random.
 *              length- # bytes expected in total for a packet.
 *              tx_offset - offset into TX buffer where packet starts.
 *              rx_offset - offset into RX buffer where packet starts.
 * Returns:     0 - OK, chain not complete
 *              1 - OK, chain complete
 *              -1- ERROR found.
 */
{
    int         rv = 0;
    dv_t        *dv = lw->lw_rx_dv[0];
    dcb_t       *end_dcb, *start_dcb, *cur_dcb;
    int         cur_packet;             /* Current packet number in chain */
    int         cur_len;                /* Current running packet length */
    int         rx_crc_valid;           /* Boolean - TRUE if CRC !HW update*/
    int         unit = lw->lw_unit;
    int         skip_vlan = 0;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if ((SOC_IS_XGS3_SWITCH(lw->lw_unit)) &&
        (IS_ST_PORT(lw->lw_unit, lw->lw_lp->lp_rx_port))){
        skip_vlan = 1;
    }
#endif

    /* Check next DCB to look at */

    start_dcb  = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_dsc);
    cur_packet = dv->dv_pckt;                   /* Total packets in chain */

    /* Check for all packets we are expecting */

    while (!rv && (cur_packet < ppc)) {
        /*
         * Find the end of the current packet, be sure it is complete,
         * if not, escape to check later.
         */
        cur_len = 0;
        cur_dcb = start_dcb;

        /*
         * Find last DCB for current packet, if dpp is >0, then it indicates
         * number of DCBs, if <0, then random, and last DCB must be scanned
         * for.
         */
        if (dpp > 0) {
            end_dcb = SOC_DCB_IDX2PTR(unit, start_dcb, dpp - 1);
        } else {
            end_dcb = NULL;             /* Keep gcc happy */
            assert(0);                  /* Not implemented. */
        }

        assert(end_dcb <= SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt));

        if (!SOC_DCB_DONE_GET(unit, end_dcb)) {
            break;
        }

        /* If debugging DMA - dump the chain */
        if (bsl_check(bslLayerAppl, bslSourceDma, bslSeverityNormal, unit)) {
            soc_dma_dump_dv(lw->lw_unit, "S/G Rx:", dv);
        }

        rx_crc_valid = !SOC_DCB_RX_CRC_GET(unit, end_dcb);
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(lw->lw_unit)) {
            rx_crc_valid = 0;
        }
#endif

        /* Last DCB must have END bit set */

        if (!SOC_DCB_RX_END_GET(unit, end_dcb)) {
            cli_out("Error: RX: DCB (%d) Last DCB for packet does not have "
                    "END bit set\n",
                    SOC_DCB_PTR2IDX(unit, end_dcb, dv->dv_dcb));
            rv = -1;
        }

        /* First DCB MUST have Start bit set. */

        if (!SOC_DCB_RX_START_GET(unit, cur_dcb) && 
            !soc_feature(unit, soc_feature_dcb_st0_bug)) {
            cli_out("Error: RX: DCB (%d) First for packet does not have "
                    "START set\n",
                    SOC_DCB_PTR2IDX(unit, end_dcb, dv->dv_dcb));
            rv = -1;
        }

        /*
         * Loop through looking for the end of the packet, checking
         * along the way that the END and START bits appear in the
         * correct place. When an entire packet is noticed to be complete,
         * check the data.
         */
        while (cur_dcb <= end_dcb) {

            /* Check for unexpected start bit */

            if ((cur_dcb != start_dcb) && 
                SOC_DCB_RX_START_GET(unit, cur_dcb)) {
                cli_out("Error: RX: DCB (%d) Unexpected START bit\n",
                        SOC_DCB_PTR2IDX(unit, cur_dcb, dv->dv_dcb));
                rv = -1;
            }

            /* Check for unexpected END bit */

            if ((cur_dcb != end_dcb) &&
                SOC_DCB_RX_END_GET(unit, cur_dcb)) {
                cli_out("Error: RX: DCB (%d) Unexpected END bit\n",
                        SOC_DCB_PTR2IDX(unit, cur_dcb, dv->dv_dcb));
                rv = -1;
            }

            /* Verify DONE is set is all DCBs */

            if (!SOC_DCB_DONE_GET(unit, cur_dcb)) {
                cli_out("Error: RX: DCB (%d) Expected DONE set\n",
                        SOC_DCB_PTR2IDX(unit, cur_dcb, dv->dv_dcb));
                rv = -1;
            }

            /* Verify byte count */

            if (SOC_DCB_REQCOUNT_GET(unit, cur_dcb) != 
                SOC_DCB_XFERCOUNT_GET(unit, cur_dcb)) {
                cli_out("Error: RX: Unexpected count: Expected %d Got %d\n",
                        SOC_DCB_REQCOUNT_GET(unit, cur_dcb), 
                        SOC_DCB_XFERCOUNT_GET(unit, cur_dcb));
                rv = -1;
            }
            cur_len += SOC_DCB_XFERCOUNT_GET(unit, cur_dcb);
            cur_dcb = SOC_DCB_IDX2PTR(unit, cur_dcb, 1);   /* Next DCB */
        }

        /* Check packet Data */

        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "lb_sg_check_rx : rxlen = %d txlen = %d\n"),cur_len,length));
        if (lb_check_packet(lw, lw->lw_tx_packets[cur_packet] + tx_offset,
                            length,
                            lw->lw_rx_packets[cur_packet] + rx_offset,
                            cur_len,
                            rx_crc_valid,
                            lw->lw_mac_dst, lw->lw_mac_src, skip_vlan) < 0) {
            rv = -1;
        }

        /* Bump pointer to start of next packet */

        cur_packet++;
        lw->lw_rx_count++;              /* Count RX packets */
        lw->lw_rx_bytes += length;      /*  ... and total bytes */
        start_dcb = SOC_DCB_IDX2PTR(unit, end_dcb, 1);
    }

    /* Update current packet */
    dv->dv_pckt = cur_packet;
    /* Update next to look at */
    dv->dv_dsc = SOC_DCB_PTR2IDX(unit, start_dcb, dv->dv_dcb);

    if (rv) {
        soc_dma_dump_dv(lw->lw_unit, "Rx ERR ", dv);
        soc_dma_dump_dv(lw->lw_unit, "Tx chn ", lw->lw_tx_dv[0]);
        test_error(lw->lw_unit, "RX: Check failed\n");
        return(-1);
    } else {
        return((cur_packet == ppc) && lw->lw_eoc_rx);
    }
}

STATIC  int
lb_sg_wait(loopback_test_t *lw, int length, int ppc, int tx_dpp, int tx_offset,
           int rx_dpp)
/*
 * Function:    lb_sg_wait
 * Purpose:     Wait for packets to Tx/Rx and check data/dcbs
 *              as they complete.
 * Parameters:  lw - pointer to work structure.
 *              length - current Tx/Rx packet length.
 *              ppc - packets-per-chain
 *              tx_dpp - TX DCBs-per-packet.
 *              tx_offset - offset into tx packet where header starts.
 *              rx_dpp - RX DCBs-per-packet.
 * Returns:     0 - success
 *              !0 - failed
 * Notes:       10-seconds without any activity results in failure.
 */
{
    int tx_done = 0;
    int rx_done = 0;
    int rv = 0;
    int level = 0;

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        /* Update length */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("lb_sg_wait: length = %d\n"), length));
        length += SOC_HIGIG_HDR_SIZE;
    }
#endif
    while (!lw->lw_eoc_tx || !lw->lw_eoc_rx) {
        if (sal_sem_take(lw->lw_sema, lw->lw_timeout_usec)) {
            cli_out("Time-out waiting for completion "
                    "Tx(%s)=%s Rx(%s)=%s\n",
                    SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_tx_port),
                    lw->lw_eoc_tx ? "Done" : "Pending",
                    SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_rx_port),
                    lw->lw_eoc_rx ? "Done" : "Pending");
            rv = -1;
            return rv;
        } else {
           level = sal_splhi();
            lw->lw_sema_woke = FALSE;
           sal_spl(level);
        }
    }

    rv = tx_done = lb_sg_check_tx(lw, ppc, tx_dpp, length);
    if (rv < 0) {
        return rv;
    }
    rv = rx_done = lb_sg_check_rx(lw, ppc, rx_dpp, length,
                                  tx_offset, 0);

    return rv;
}

STATIC  int
lb_do_sg_rx(loopback_test_t *lw, int p_len, int p_cnt, int dpp)
/*
 * Function:    lb_do_sg_rx
 * Purpose:     Start an RX for the specified length/chain.
 * Parameters:  lw - pointer to lb work.
 *              p_len - length of each packet.
 *              p_cnt - # of packets per chain.
 *              dpp  - # dcbs per packet.
 * Returns:     0 - success, -1 failed.
 */
{
    dv_t        *dv = lw->lw_rx_dv[0];
    loopback_testdata_t *lp = lw->lw_lp;
    int         i;
    pbmp_t      pbm;

    /*
     * For RX, set flags to call out for each DCB completed.
     */
    soc_dma_dv_reset(DV_RX, dv);
    dv->dv_flags |= DV_F_NOTIFY_DSC | DV_F_NOTIFY_CHN;
    dv->dv_done_chain = lb_done_chain;
    dv->dv_dsc  = 0;                    /* Start at descriptor 0 */
    dv->dv_pckt = 0;                    /* Start at packet 0  */

    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, lp->lp_tx_port);

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        /* Update length */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("lb_do_sg_rx: p_len = %d\n"), p_len));
        p_len += SOC_HIGIG_HDR_SIZE;
    }
#endif
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if ((SOC_IS_XGS3_SWITCH(lw->lw_unit)) &&
        (IS_ST_PORT(lw->lw_unit, lw->lw_lp->lp_rx_port))){
        p_len += 4;
    }
#endif
    for (i = 0; i < p_cnt; i++) {

        /* Zero recv packet to help with debugging */

        sal_memset(lw->lw_rx_packets[i], 0, p_len);

        lb_setup_sg_dcbs(dv, p_len, dpp, FALSE, lw->lw_rx_packets[i], 4,
                         pbm, lp->lp_ubm,
                         crc_mode_to_dmac(lp->lp_crc_mode), 0, NULL);
    }

    lw->lw_rx_dv_chain_done = dv;
    lw->lw_eoc_rx = 0;

    return((soc_dma_start(lw->lw_unit, -1, dv) < 0) ? -1 : 0);
}

STATIC int
lb_do_sg_tx(loopback_test_t *lw, int tx_offset, int tx_len,
            int tx_ppc, int tx_dpp, int cos)
/*
 * Function:    lb_do_sg_tx
 * Purpose:     Start a transmit for the current length/chain.
 * Parameters:  lw - pointer to lb work.
 *              tx_offset - offset into TX buffer where to build packet.
 *              tx_len - length of packet
 *              tx_ppc - TX packets-per-chain.
 *              tx_dpp - TX DCBs-per-packet.
 *              cos    - Class of service
 * Returns:     0 - success, -1 failed.
 */
{
    dv_t        *dv = lw->lw_tx_dv[0];
    loopback_testdata_t *lp = lw->lw_lp;
    int         i;
    int         o_tx_len;
    pbmp_t      pbm;
    int         dmod = 0;
#ifdef BCM_HIGIG2_SUPPORT
    soc_higig2_hdr_t    xgh;
    uint32              *xghp = (uint32 *)&xgh;
#elif defined(BCM_XGS_SUPPORT)
    soc_higig_hdr_t     xgh;
    uint32              *xghp = (uint32 *)&xgh;
#else
    uint32              *xghp = NULL;
#endif
    
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(lw->lw_unit)) {
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(lw->lw_unit, &dmod));
    }
#endif

    soc_dma_dv_reset(DV_TX, dv);
    dv->dv_done_chain = lb_done_chain;
    SET_NOTIFY_CHN_ONLY(dv->dv_flags);
    dv->dv_dsc  = 0;                    /* Start descriptor 0 */
    dv->dv_pckt = 0;                    /* Start at packet 0  */

    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, lp->lp_tx_port);

    for (i = 0; i < tx_ppc; i++) {
        uint8           *packet = lw->lw_tx_packets[i] + tx_offset;
        int             tx_align = 1;

        if (soc_feature(lw->lw_unit, soc_feature_pkt_tx_align)) {
            tx_align = 4;
        }

        /* Build packet */
        o_tx_len = tx_len;
        lb_fill_packet_tx(lw, lp, packet, &tx_len,
                          lw->lw_mac_dst, lw->lw_mac_src);

        lb_xgs3_higig_setup(lw, cos, dmod,
                            lp->lp_tx_port, lp->lp_vlan, xghp);
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(lw->lw_unit)) {
            lb_setup_sg_dcbs(dv, tx_len, tx_dpp, TRUE,
                             packet, tx_align, pbm, lp->lp_ubm, 
                             crc_mode_to_dmac(lp->lp_crc_mode), cos, NULL);
        } else 
#endif
        {
            lb_setup_sg_dcbs(dv, tx_len, tx_dpp, TRUE,
                         packet, tx_align, pbm, lp->lp_ubm, 
                         crc_mode_to_dmac(lp->lp_crc_mode), cos, xghp);
        }

        tx_len = o_tx_len;
    }

    lw->lw_tx_dv_chain_done = dv;
    lw->lw_eoc_tx = 0;
    return((soc_dma_start(lw->lw_unit, -1, dv) < 0) ? -1 : 0);
}

int
lb_do_sg_txrx(loopback_test_t *lw)
/*
 * Function:    lb_do_sg_txrx
 * Purpose:     Loop over requested lengths and chaining sizes, and
 *              transmit/receive packets.
 * Parameters:  lw - pointer to work structure
 * Returns:     0 - success
 *              -1- failed.
 */
{
    loopback_testdata_t *lp = lw->lw_lp;
    int         l_cur, cos_cur, c_count, cur_ppc;
    int         tx_offset, tx_dpp, rx_dpp;
    int         tx_align = 1;

    if (soc_feature(lw->lw_unit, soc_feature_pkt_tx_align)) {
        tx_align = 4;
    }

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Testing Port %s --> %s cnt(%d) chn(%d,%d += %d) "
                       "l(%d,%d += %d) cos(%d,%d)\n"),
              SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_tx_port),
              SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_rx_port),
              lp->lp_count,
              lp->lp_c_start, lp->lp_c_end, lp->lp_c_inc,
              lp->lp_l_start, lp->lp_l_end, lp->lp_l_inc,
              lp->lp_cos_start, lp->lp_cos_end));

    for (l_cur = lp->lp_l_start;
         l_cur <= lp->lp_l_end;
         l_cur += lp->lp_l_inc) {
        for (cos_cur = lp->lp_cos_start;
             cos_cur <= lp->lp_cos_end;
             cos_cur++) {
            for (tx_offset = 0; tx_offset <= 3; tx_offset += tx_align) {
                for (cur_ppc = lp->lp_ppc_start;
                     cur_ppc <= lp->lp_ppc_end;
                     cur_ppc += lp->lp_ppc_inc) {
                    for (tx_dpp = lp->lp_dpp_start;
                         tx_dpp <= lp->lp_dpp_end;
                         tx_dpp += lp->lp_dpp_inc) {
                        for (rx_dpp = lp->lp_dpp_start;
                             rx_dpp <= lp->lp_dpp_end;
                             rx_dpp += lp->lp_dpp_inc) {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                     (BSL_META("Testing cnt=%d L=%d cos=%d "
                                               "TX-offset=%d TX-dpp=%d "
                                               "RX-dpp=%d\n"),
                                      lp->lp_count, l_cur, cos_cur,
                                      tx_offset, tx_dpp,
                                      rx_dpp));
                            for (c_count = 0;
                                 c_count < lp->lp_count;
                                 c_count++) {
                                lw->lw_sema_woke = FALSE;
                                lb_do_sg_rx(lw, l_cur, cur_ppc, rx_dpp);
                                lb_do_sg_tx(lw, tx_offset, l_cur, cur_ppc,
                                            tx_dpp, cos_cur);

                                if (lb_sg_wait(lw, l_cur,
                                               cur_ppc, tx_dpp, tx_offset,
                                               rx_dpp) < 0) {
                                    test_error(lw->lw_unit,
                                        "\tPort %s->%s\n"
                                        "\tPacket Length=%d bytes\n"
                                        "\tCurrent COS=%d\n"
                                        "\tTX offset=%d\n"
                                        "\tCurrent PPC=%d\n"
                                        "\tTX DPP=%d\n"
                                        "\tRX DPP=%d\n"
                                        "\tCurrent Count (%d/%d)\n",
                                        SOC_PORT_NAME(lw->lw_unit,
                                                      lp->lp_tx_port),
                                        SOC_PORT_NAME(lw->lw_unit,
                                                      lp->lp_rx_port),
                                        l_cur, cos_cur, tx_offset,
                                        cur_ppc, tx_dpp, rx_dpp, 
                                        c_count + 1, lp->lp_count);
                                         return(-1);
                                }
                                lb_stats_report(lw);
                            }
                        }
                    }
                }
            }
        }
    }
    return(0);
}

int
lb_sg_dma_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_sg_dma_test
 * Purpose:     Perform simple interrupt driven DMA test.
 * Parameters:  <see usage above>
 * Returns:     0 - success
 *              -1 - failed
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = &lw->lw_lp_sg_dma;
    int         rx_chan, tx_chan, chan;       /* RX/TX channel numbers */
    int rv;

    lb_stats_init(lw);

    COMPILER_REFERENCE(a);

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    /*
     * For each of the DMA channel configurations, run the entire test.
     */

    lw->lw_lp = lp;                     /* Assign test # */
    if ((lb_is_xgs_fabric(unit)) || (SOC_IS_XGS3_SWITCH(unit))) {
        int port;
        pbmp_t      tpbm;

        if (lb_is_xgs_fabric(unit)) {
            SOC_PBMP_ASSIGN(tpbm, PBMP_PORT_ALL(unit));
        } else {
            SOC_PBMP_ASSIGN(tpbm, PBMP_E_ALL(unit));
        }
        PBMP_ITER(tpbm, port) {
            break;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            test_error(unit, "No Ports available for loopback\n");
            return(-1);
        }
        SOC_PBMP_PORT_ADD(lp->lp_pbm, port);
        lp->lp_tx_port = port;
        lp->lp_rx_port = port;
        _lb_max_frame_set(lw, lp);
        if ((rv = bcm_port_loopback_set(unit, port,
                                        BCM_PORT_LOOPBACK_MAC)) < 0) {
            test_error(unit,
                   "Port %s: Failed to set MAC loopback: %s\n",
                   SOC_PORT_NAME(unit, 1), bcm_errmsg(rv));
            return(-1);
        }

        if (IS_HG_PORT(unit, port)) {
            bcm_port_pause_set(unit, port, FALSE, FALSE);
        }
        /*
         * Get our current MAC address for this port and be sure the
         * SOC will route packets to the CPU.
         */
        if (lb_setup_arl(lw, unit, CMIC_PORT(unit), port)) {
            return(-1);
        }
        if (SOC_IS_XGS3_SWITCH(unit) && !(SOC_IS_SHADOW(unit))) {
            pbmp_t              temp_pbmp;
            SOC_PBMP_CLEAR(temp_pbmp);
            /*
             * Setup VLAN
             */
            SOC_PBMP_PORT_ADD(temp_pbmp, CMIC_PORT(unit));
            SOC_PBMP_PORT_ADD(temp_pbmp, port);
			if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
	            if ((rv = bcm_vlan_port_add(unit, lp->lp_vlan,
	                                        temp_pbmp, temp_pbmp)) < 0) {
	                test_error(unit,
	                           "Could not add all ports to VLAN %d: %s\n",
	                           lp->lp_vlan, bcm_errmsg(rv));
	                return (-1);
	            }
			}
        }

        if ((rv = lb_setup_port(unit, port, LB_SPEED_MAX, FALSE)) < 0) { 
            test_error(unit,
                       "Port %s: Port setup failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
    } else {
        lp->lp_tx_port = CMIC_PORT(unit);
        lp->lp_rx_port = CMIC_PORT(unit);
    }

    lw->lw_tx_dcb_factor = 1;

    for (rx_chan = 0; rx_chan < chan; rx_chan++) {
#ifdef INCLUDE_KNET
        if (SOC_KNET_MODE(unit)) {
            /* Currently KNET uses chan 1 for rx */
            if (rx_chan != KCOM_DMA_RX_CHAN) {
                continue;
            }
        }
#endif /* INCLUDE_KNET */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Configuring RX-channel: %d\n"), rx_chan));

        if (soc_dma_chan_config(unit, rx_chan, DV_RX, 
                                SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
            test_error(unit, "Unable to configure RX channel: %d\n", rx_chan);
            break;
        }
#ifdef BCM_CMICM_SUPPORT
        /* Assign all COS to this channel
         * Though this is added for CMICm, 
         * it should be OK if we do this for CMICe as well */
        if(soc_feature(unit, soc_feature_cmicm)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Assign all COS to channel: 1\n")));
            SOC_IF_ERROR_RETURN(
                bcm_rx_queue_channel_set(unit, -1, rx_chan));

#if defined(BCM_KATANA2_SUPPORT)
                if(SOC_IS_KATANA2(unit)) {
                    uint32 reg_val, reg_addr;

                    /* Including CPU L0 bitmap to support CMIC backpressure */
                    reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, rx_chan);
                    reg_val = soc_pci_read(unit, reg_addr);
                    reg_val |= (1 << 16);
                    soc_pci_write(unit, reg_addr, reg_val);
                }
#endif

        }
#endif
#ifdef BCM_CMICX_SUPPORT
        /* Assign all COS to this channel */
        if(soc_feature(unit, soc_feature_cmicx)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Assign all COS to channel: 1\n")));
            SOC_IF_ERROR_RETURN(
                bcm_rx_queue_channel_set(unit, -1, rx_chan));
        }
#endif

        for (tx_chan = 0; tx_chan < chan; tx_chan++) {
#ifdef INCLUDE_KNET
            if (SOC_KNET_MODE(unit)) {
                /* Currently KNET uses chan 0 for tx */
                if (tx_chan != KCOM_DMA_TX_CHAN) {
                    continue;
                }
            }
#endif /* INCLUDE_KNET */
            if (rx_chan == tx_chan) {   /* Does not make sense */
                continue;
            }

            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Configuring TX-channel: %d\n"), tx_chan));

            if (soc_dma_chan_config(unit, tx_chan, DV_TX,
                                    SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
                test_error(unit,
                           "Unable to configure TX channel: %d\n",
                           tx_chan);
                break;
            }

            if (lb_do_sg_txrx(lw)) {
                return -1;
            }

            if (soc_dma_chan_config(unit, tx_chan, DV_NONE, 0)) {
                test_error(unit,
                           "Unable to de-configure TX channel: %d\n",
                           tx_chan);
                break;
            }
        }

        if (soc_dma_chan_config(unit, rx_chan, DV_NONE, 0)) {
            test_error(unit,
                       "Unable to de-configure RX channel: %d\n",
                       rx_chan);
            break;
        }
    }

    lb_stats_done(lw);

    return(0);
}

int
lb_sg_dma_init(int unit, args_t *a, void **p)
/*
 * Function:    lb_sg_dma_init
 * Purpose:     Initialize scatter/gather DMA loopback test.
 * Parameters:  unit - unit number.
 *              a - pointer to arguments.
 *              p - pointer passed to test function.
 * Returns:     0 for success, -1 for failed.
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_sg_dma;
    int                 i;
    int                 rv = 0;
    parse_table_t       pt;
    int chan;

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL,0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL,0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "PacketsPerChainStart",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_start,NULL);
    parse_table_add(&pt, "PacketsPerChainEnd",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_end,NULL);
    parse_table_add(&pt, "PacketsPerChainInc",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_inc,NULL);
    parse_table_add(&pt, "DcbsPerPacketStart",PQ_INT|PQ_DFL,0,
                    &lp->lp_dpp_start,NULL);
    parse_table_add(&pt, "DcbsPerPacketsEnd",PQ_INT|PQ_DFL,0,
                    &lp->lp_dpp_end,NULL);
    parse_table_add(&pt, "DcbsPerPacketInc",PQ_INT|PQ_DFL,0,
                    &lp->lp_dpp_inc,NULL);
    parse_table_add(&pt, "COSStart",    PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_start,NULL);
    parse_table_add(&pt, "COSEnd",      PQ_INT|PQ_DFL,  0,
                    &lp->lp_cos_end,NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data,NULL);
    parse_table_add(&pt, "CheckCrc",    PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_crc,NULL);

    lb_setup(unit, lw);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        cli_out("Invalid test case under KNET mode.\n");
        return(-1);
    }
#endif /* INCLUDE_KNET */

    SOC_PBMP_CLEAR(lp->lp_pbm);
    if ((lb_is_xgs_fabric(unit)) || (SOC_IS_XGS3_SWITCH(unit))) {
        int port;
        pbmp_t      tpbm;

        if (lb_is_xgs_fabric(unit)) {
            SOC_PBMP_ASSIGN(tpbm, PBMP_PORT_ALL(unit));
        } else {
            SOC_PBMP_ASSIGN(tpbm, PBMP_E_ALL(unit));
        }
        PBMP_ITER(tpbm, port) {
            break;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            test_error(unit, "No Ports available for loopback\n");
            return(-1);
        }
        SOC_PBMP_PORT_ADD(lp->lp_pbm, CMIC_PORT(unit));
        SOC_PBMP_PORT_ADD(lp->lp_pbm, port);
        lp->lp_rx_port = port;
    } else {
        SOC_PBMP_PORT_ADD(lp->lp_pbm, CMIC_PORT(unit));
        lp->lp_rx_port = CMIC_PORT(unit);
    }

    /* Update chain sizes */

    lp->lp_c_start = lp->lp_ppc_start * lp->lp_dpp_start;
    lp->lp_c_end   = lp->lp_ppc_end * lp->lp_dpp_end;
    lp->lp_c_inc   = 1;

    lp->lp_sg = TRUE;

    if ((rv = lb_check_parms(lw, lp, lp->lp_pbm)) < 0) {
        return(rv);
    }

    /* Verify parameters make sense */

    if (lp->lp_ppc_start > lp->lp_ppc_end) {
        test_error(lw->lw_unit,
                   "Packets-per-chain start/end values don't make sense\n");
        return(-1);
    }

    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, FALSE)) {
        lb_sg_dma_done(unit, NULL);
        return(-1);
    }

    /*
     * Allow us to run with external loopback cables plugged in as the
     * BigMAC loopback allows the packets to go out on the wire.
     */
    if (lb_unused_10g_ports_enable_set(unit, lp->lp_rx_port, 0)) {
        lb_sg_dma_done(unit, NULL);
        return(-1);
    }

    /* Set up MAC addresses (currently constant) */

    ENET_SET_MACADDR(lw->lw_mac_src, lb_mac_src);
    ENET_SET_MACADDR(lw->lw_mac_dst, lb_mac_dst);

    if (!SOC_PBMP_MEMBER(lp->lp_pbm, CMIC_PORT(unit))) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Warning: CPU port not set in bitmap.\n"
                             "Packet will not be received, receive DMA will hang.\n")));
    }

    *p = lw;                            /* Pass back cookie */

    for (i = 0; i < chan; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "lb_dma_init: Deconfiguring channel %d\n"), i));

        if (soc_dma_chan_config(unit, i, DV_NONE, 0)) {
            rv = -1;
        }
    }

    if (rv) {
        lb_sg_dma_done(unit, NULL);
    }

    return(rv);
}

int
lb_sg_dma_done(int unit, void *pa)
/*
 * Function:    lb_sg_dma_done
 * Purpose:     Clean up after DMA loopback test.
 * Parameters:  unit - unit number.
 * Returns:     0 for success, -1 for failed.
 */
{
    COMPILER_REFERENCE(pa);

    lb_unused_10g_ports_enable_set(unit, lb_work[unit].lw_lp->lp_rx_port, 1);

    /* Re-configure channels */
    lb_done(&lb_work[unit]);

    return(0);
}

STATIC  void
lb_dump_dv(int unit, char *pfx, dv_t **dv_array, int dv_cnt)
/*
 * Function:    lb_dump_dv
 * Purpose:     Dump a DV list including RLD/Chained descriptors
 * Parameters:  pfx - Prefix printed with messages
 *              dv_array - array of dv pointers
 *              dv_cnt - number of dvs to look at.
 * Returns:     Nothing.
 */
{
    dv_t        *dv;
    int         i = 0;

    while (i < dv_cnt) {
        dv = dv_array[i];

        soc_dma_dump_dv(unit, pfx, dv);

        while (dv) {
            dv = dv->dv_chain;
            i++;
        }
    }
}

int
lb_reload_init(int unit, args_t *a, void **p)
/*
 * Function:    lb_reload_init
 * Purpose:     Initialize loopback "reload" test.
 * Parameters:  unit - unit #
 *              a - pointer to args
 *              p - pointer passed to test function.
 * Returns:     0 - success
 *              -1 - failed
 */
{
    loopback_test_t             *lw = &lb_work[unit];
    loopback_testdata_t         *lp = &lw->lw_lp_sg_random;
    int                 i;
    int                 rv = 0;
    parse_table_t       pt;
    int chan;

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    bcm_l2_addr_t_init(&lw->lw_arl_src, lb_mac_src, 1);
    bcm_l2_addr_t_init(&lw->lw_arl_dst, lb_mac_dst, 1);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &lp->lp_pattern,NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL,0,
                    &lp->lp_pattern_inc,NULL);
    parse_table_add(&pt, "PacketsPerChainStart",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_start,NULL);
    parse_table_add(&pt, "PacketsPerChainEnd",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_end,NULL);
    parse_table_add(&pt, "PacketsPerChainInc",PQ_INT|PQ_DFL,0,
                    &lp->lp_ppc_inc,NULL);
    parse_table_add(&pt, "LengthStart", PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_start,NULL);
    parse_table_add(&pt, "LengthEnd",   PQ_INT|PQ_DFL,  0,
                    &lp->lp_l_end,      NULL);
    parse_table_add(&pt, "LengthIncrement",PQ_INT|PQ_DFL,0,
                    &lp->lp_l_inc,      NULL);
    parse_table_add(&pt, "DVStart",     PQ_INT|PQ_DFL,0,
                    &lp->lp_dv_start,NULL);
    parse_table_add(&pt, "DVEnd",       PQ_INT|PQ_DFL,0,
                    &lp->lp_dv_end,NULL);
    parse_table_add(&pt, "DVInc",       PQ_INT|PQ_DFL,0,
                    &lp->lp_dv_inc,NULL);
    parse_table_add(&pt, "DcbStart",    PQ_INT|PQ_DFL,0,
                    &lp->lp_c_start,NULL);
    parse_table_add(&pt, "DcbEnd",      PQ_INT|PQ_DFL,0,
                    &lp->lp_c_end,NULL);
    parse_table_add(&pt, "DcbIncrement",PQ_INT|PQ_DFL,0,
                    &lp->lp_c_inc,NULL);
    parse_table_add(&pt, "Count",       PQ_INT|PQ_DFL,  0,
                    &lp->lp_count,      NULL);
    parse_table_add(&pt, "SEED",        PQ_INT|PQ_DFL,  0,
                    &lp->lp_seed,       NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_data,NULL);
    parse_table_add(&pt, "CheckCrc",    PQ_BOOL|PQ_DFL, 0,
                    &lp->lp_check_crc,NULL);

    lb_setup(unit, lw);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return(-1);
    }
    parse_arg_eq_done(&pt);

#ifdef INCLUDE_KNET
    if (SOC_KNET_MODE(unit)) {
        cli_out("Invalid test case under KNET mode.\n");
        return(-1);
    }
#endif /* INCLUDE_KNET */

    SOC_PBMP_CLEAR(lp->lp_pbm);
    SOC_PBMP_PORT_ADD(lp->lp_pbm, CMIC_PORT(unit));
    if ((lb_is_xgs_fabric(unit)) || (SOC_IS_XGS3_SWITCH(unit))) {
        int port;
        PBMP_PORT_ITER(unit, port) {
            break;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            test_error(unit, "No Ports available for loopback\n");
            return(-1);
        }
        SOC_PBMP_PORT_ADD(lp->lp_pbm, port);
        lp->lp_tx_port = port;
        lp->lp_rx_port = port;
    } else {
        lp->lp_tx_port = CMIC_PORT(unit);
        lp->lp_rx_port = CMIC_PORT(unit);
    }

    lp->lp_sg = TRUE;

    /* Perform common loopback initialization */

    if (lb_init(unit, lw, lp, FALSE)) {
        return(-1);
    }

    /*
     * Allow us to run with external loopback cables plugged in as the
     * BigMAC loopback allows the packets to go out on the wire.
     */
    if (lb_unused_10g_ports_enable_set(unit, lp->lp_rx_port, 0)) {
        return(-1);
    }

    /* Check Parameters and update useful things. */

    *p = lw;                            /* Pass back cookie */

    for (i = 0; i < chan; i++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "lb_dma_init: Deconfiguring channel %d\n"), i));

        if (soc_dma_chan_config(unit, i, DV_NONE, 0)) {
            rv = -1;
        }
    }

    if (rv) {
        lb_done(lw);
    }

    return(rv);
}

int
lb_random_init(int unit, args_t *a, void **p)
/*
 * Function:    lb_random_init
 * Purpose:     Initialize loopback "random" test.
 * Parameters:  unit - unit #
 *              a - pointer to args
 *              p - pointer passed to test function.
 * Returns:     0 - success
 *              -1 - failed
 */
{
    /* Currently same as reload init */
    return lb_reload_init(unit, a, p);
}

int
lb_reload_done(int unit, void *pa)
/*
 * Function:    lb_reload_done
 * Purpose:     Clean up after reload test complete
 * Parameters:  unit - unit #
 *              pa - argument returned from init (may be NULL)
 * Returns:     0 - success.
 *              -1 - failed.
 */
{
    COMPILER_REFERENCE(pa);

    lb_unused_10g_ports_enable_set(unit, lb_work[unit].lw_lp->lp_rx_port, 1);

    lb_done(&lb_work[unit]);

    return(0);
}

int
lb_random_done(int unit, void *pa)
/*
 * Function:    lb_random_done
 * Purpose:     Clean up after random test complete
 * Parameters:  unit - unit #
 *              pa - argument returned from init (may be NULL)
 * Returns:     0 - success.
 *              -1 - failed.
 */
{
    /* Currently same as reload done */
    return lb_reload_done(unit, pa);
}

#define LB_START_MAGIC  (('S' << 24) | ('t' << 16) | ('r' << 8) | 't')
#define LB_END_MAGIC    (('E' << 24) | ('n' << 16) | ('d' << 8) | 's')

STATIC int
lb_random_restore_dv(int unit, uint8 *packet, int length,
                     dv_t **dv_array, int *dv_idx, int dv_cnt)
/*
 * Function:    lb_random_restore_dv
 * Purpose:     Inspect dv/dcbs and copy out the received data into
 *              a received buffer.
 * Parameters:  unit - current unit number being operated on.
 *              packet - pointer to packet to copy data into.
 *              dv_array - pointer to array of pointers to dvs to fill in.
 *              dv_idx - index into dv array of "current" dv, updated on
 *                      return.
 *              dv_cnt - total # DVs in DV list.
 * Returns:     0 - OK.
 *              -1 - failed.
 * Notes:       Verifies the prefix/suffix Magic numbers.
 */
{
    dv_t                *dv = dv_array[*dv_idx];
    dcb_t               *dcb;
    dcb_t               *dcb_end;
    int                 rv = 0;
    int                 length_rcv = 0;
    uint32              magic;                  /* 32-bit Magic Holder */

    if (NULL == dv) {
        return -1;
    }
    dcb = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_dsc);
    dcb_end = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt);

    while ((NULL !=dv) && (length_rcv < length)) {
        uint8   *p;                     /* Current data pointer */

        /* Verify DCB is complete */

        if (!SOC_DCB_DONE_GET(unit, dcb)) {
            cli_out("Expected DONE not set in dv[%d@%p] dcb[%d@%p]\n",
                    *dv_idx, (void *)dv,
                    SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb), (void *)dcb);
            rv = -1;
            break;
        }

        /*
         * Check for RELOAD, if this is a reload DCB, follow on to the
         * new chain. The dv_chain pointer points to the next dv in the
         * list.
         */

        if (SOC_DCB_RELOAD_GET(unit, dcb)) {
            (*dv_idx)++;
            assert(*dv_idx < dv_cnt);
            assert(dv_array[*dv_idx] == dv->dv_chain);
            dv = dv_array[*dv_idx];
            if (NULL == dv) {
                /* Internal error -- this list should be valid */
                return -1;
            }
            dcb = dv->dv_dcb;
            dcb_end = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt);
            continue;                   /* while */
        }

        /* if first Chunk - verify SW set, if not verify not set */

        if (length_rcv == 0) {
            if (!SOC_DCB_RX_START_GET(unit, dcb) && 
                !soc_feature(unit, soc_feature_dcb_st0_bug)) {
                cli_out("Expected Start dv[%d@%p] dcb[%d] not set\n",
                        *dv_idx, (void *)dv,
                        SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb));
                rv = -1;
                break;                  /* while */
            }
        } else if (SOC_DCB_RX_START_GET(unit, dcb)) {
            cli_out("Un-expected Start dv[%d@%p] dcb[%d] set\n",
                    *dv_idx, (void *)dv, 
                    SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb));
            rv = -1;
            break;                      /* while */
        }

        /*
         * For current DCB, verify prefix/suffix.  Since it is receive,
         * we know the length and alignment are multiples of 4.
         */

        p = (uint8 *)SOC_DCB_ADDR_GET(unit, dcb);

        magic = packet_load(p - 4, 4);

        if (magic != LB_START_MAGIC) {
            rv = -1;
            cli_out("Prefix of block clobbered: "
                    "Addr: %p Exp: 0x%x Rcv: 0x%x\n",
                    (void *)(p - 4), LB_START_MAGIC, magic);
            break;                      /* while */
        }

        magic = packet_load(p + SOC_DCB_REQCOUNT_GET(unit, dcb), 4);

        if (magic != LB_END_MAGIC) {
            rv = -1;
            cli_out("Suffix of block clobbered: "
                    "Addr: %p Exp: 0x%x Rcv: 0x%x\n",
                    (void *)(p + SOC_DCB_REQCOUNT_GET(unit, dcb)), 
                    LB_END_MAGIC, magic);
            break;                      /* while */
        }

        /* Don't run over expected length */

        if (SOC_DCB_XFERCOUNT_GET(unit, dcb) > (uint32)(length - length_rcv)) {
            rv = -1;
            cli_out("Too many bytes: Exp: <= 0x%x Rcv: 0x%x\n",
                    length - length_rcv, SOC_DCB_XFERCOUNT_GET(unit, dcb));
            break;                      /* while */
        }

        length_rcv += SOC_DCB_XFERCOUNT_GET(unit, dcb);

        /* Copy out data */

        sal_memcpy(packet, p, SOC_DCB_XFERCOUNT_GET(unit, dcb));

        packet += SOC_DCB_XFERCOUNT_GET(unit, dcb);

        /* Verify EW bit settings */

        if (length_rcv >= length) {
            if (!SOC_DCB_RX_END_GET(unit, dcb)) {
                cli_out("Expected End dv[%d] dcb[%d] not set\n",
                        *dv_idx, SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb));
                rv = -1;
                break;                  /* while */
            }
        } else if (SOC_DCB_RX_END_GET(unit, dcb)) {
            cli_out("Un-expected End dv[%d] dcb[%d] set\n",
                    *dv_idx, SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb));
            rv = -1;
            break;                      /* while */
        }

        /*
         * Move to next dcb, if in current DV, simple increment,
         * otherwise, must follow the dv_chain pointer to the next
         * dcb, verify the expected completion status for the dcb.
         * The very last DCB in the entire chain of chains is also
         * special since.
         */

        dcb = SOC_DCB_IDX2PTR(unit, dcb, 1);

        if (dcb == dcb_end) {           /* On to next DV */
            (*dv_idx)++;
            if (*dv_idx == dv_cnt) {    /* End of DVs */
                dv = NULL;
                break;
            } else {
                dv = dv_array[*dv_idx];
                if (NULL == dv) {
                    /* Internal error -- this list should be valid */
                    return -1;
                }
                dcb = dv->dv_dcb;
                dcb_end = SOC_DCB_IDX2PTR(unit, dv->dv_dcb, dv->dv_vcnt);
            }
        }

    } /* while */

    if (dv) {
        dv->dv_dsc = SOC_DCB_PTR2IDX(unit, dcb, dv->dv_dcb); /* Update current */
    }

    if (length_rcv < length) { /* Something went wrong */
        cli_out("End of chain and length-received=%d expected=%d\n",
                length_rcv, length);
        rv = -1;
    }

    return(rv);
}

STATIC int
lb_random_setup_dv(int unit, uint8 *packet, int length, int align, int *dcb_cnt,
                   int dv_idx, int dv_max_dcb, dv_t *dv_array[], pbmp_t pbm,
                   pbmp_t ubm, int cos, uint32 *hgh)
/*
 * Function:    lb_random_setup_dv
 * Purpose:     Setup dcb/DV chain in "random" manner, allocate memory
 *              for EACH segment, add sentinel at beginning/end.
 * Parameters:  packet - pointer to packet data, if NULL, buffers
 *                      allocated but not filled in.
 *              length - # bytes in packets.
 *              align - alignment requirements (1 for TX, 4 for RX)
 *              dcb_cnt - Maximum # of DCBs to use on entry, number used
 *                      on return.
 *              dv_idx - current index in DV list working on.
 *              dv_max_dcb - max number of DCBs to use in a dv.
 *              dv_array - array of DVs to fill in.
 *              pbm/ubm/cos - values used for DCB fields.
 *              hgh - Higig header for XGS3 devices
 * Returns:     index into dv_array where finished.
 */
{
    dv_t        *dv = dv_array[dv_idx];
    uint8       *p;
    int         dcb_idx, dcb_length, dcb_align;
    int         max_dcbs = *dcb_cnt;
    int         dcb_used = 0;
    pbmp_t      l3pbm;
    uint32      flags = SOC_DMA_COS(cos);
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (hgh != NULL) {
        SOC_DMA_HG_SET(flags, 1);
        SOC_DMA_STATS_SET(flags, 1);
    }
#endif

    dcb_idx = dv->dv_vcnt;

    assert(dcb_idx <= dv_max_dcb);

    SOC_PBMP_CLEAR(l3pbm);

    while (length > 0) {
        /* If require next DV, link onto last, and bump dv idx/pointer */

        assert(dcb_idx <= dv_max_dcb);

        if (dcb_idx == dv_max_dcb) {
            dv = dv_array[++dv_idx];
            dcb_idx = 0;
            if (dv_idx && (sal_rand() & (0x8080))) {
                soc_dma_dv_join(dv_array[dv_idx - 1], dv);
            }
        }

        if (max_dcbs == 1) {
            /*
             * This MUST be our last DCB - consume rest of packet.
             */
            dcb_length = length;
        } else {
            dcb_length = sal_rand() % length;
            dcb_length |= dcb_length ? 0 : 1; /* Be sure 1 byte */
        }

        dcb_align  = sal_rand() & -align & 3;
        dcb_length = (dcb_length + align - 1) & -align;
        length -= dcb_length;

        /*
         * Based on the length, allocate that # bytes, +4 for magic at
         * head + 4 for magic at tail +4 for arbitrary alignment.
         */

        p = soc_cm_salloc(unit,
                          SOC_DMA_ROUND_LEN(dcb_length + 4 + 4 + 4),
                          "LB-Rand");

        /* set data in header/tail */

        packet_store(p, dcb_align + 4, LB_START_MAGIC, 0);
        p += dcb_align + 4;             /* Move past prefix */
        packet_store(p + dcb_length, 4, LB_END_MAGIC, 0);

        if (packet) {
            /* Copy in packet data - if supplied */
            sal_memcpy(p, packet, dcb_length);
            packet += dcb_length;       /* Bump packet contents pointer */
        }

        /*
         * We now have the length for the next DCB, if 'length' is now
         * <= 0, S/G is set to 0, otherwise it is set to 1.
         */

        soc_dma_desc_add(dv, (sal_vaddr_t)p, dcb_length, pbm, ubm, l3pbm,
                         flags, (uint32 *)hgh);

        max_dcbs--;

        dcb_idx++;
        dcb_used++;
    }

    soc_dma_desc_end_packet(dv);
    *dcb_cnt = dcb_used;

    return(dv_idx);
}


void
lb_reset_dv(int unit, dv_t **dv_array, int dv_cnt)
/*
 * Function:    lb_reset_dv
 * Purpose:     Reset the current dv list to a clean state.
 * Parameters:  dv_array - array of pointers to dv structures to reset
 *              dv_cnt - # of dv pointers in dv_array.
 * Returns:     Nothing
 */
{
    sal_vaddr_t         addr;
    dcb_t               *dcb;
    int                 i; 

    while (dv_cnt--) {
        dv_array[dv_cnt]->dv_pckt = 0;
        dv_array[dv_cnt]->dv_dsc  = 0;

        /* Free memory */

        for (i = 0; i < dv_array[dv_cnt]->dv_vcnt; i++) {
            dcb = SOC_DCB_IDX2PTR(unit, dv_array[dv_cnt]->dv_dcb, i);

            /* Don't free reload */
            if (!SOC_DCB_RELOAD_GET(unit, dcb) &&
                SOC_DCB_ADDR_GET(unit, dcb)) {
                /* Before prefix */
                addr = (SOC_DCB_ADDR_GET(unit, dcb) & -4) - 4;
                soc_cm_sfree(unit, (void *)addr);
                SOC_DCB_ADDR_SET(unit, dcb, 0);
            }
        }

        soc_dma_dv_reset(dv_array[dv_cnt]->dv_op, dv_array[dv_cnt]);
    }
}


STATIC  int
lb_random_pkt(loopback_test_t *lw, loopback_testdata_t *lp, dma_chan_t tx_chan,
              dma_chan_t rx_chan, int l, int p_cnt, int c_cnt, int dv_cnt)
/*
 * Function:    lb_random_pkt
 * Purpose:     Setup and transmit/receive random packets.
 *              scatter/gather DMA and unaligned address DMA.
 * Parameters:  lw - pointer to work structure
 *              lp - pointer to parameters
 *              tx_chan - transmit channel #
 *              rx_chan - receive channel #.
 *              l - length of packet to transmit.
 *              p_cnt - total number of packets to place in current
 *                      dv/dcb list.
 *              c_cnt - current # dcbs to use for packets in each dv.
 *              dv_cnt - total # of DV's to use for p_cnt packets.
 * Returns:     0 - success
 *              -1 - failed
 */
{
    int         tx_dv_idx, rx_dv_idx, p_idx;
    int         rx_dcb_remain, tx_dcb_remain;
    int         dcb_cnt;
    int         tx_rv, rx_rv;
    int         rv = 0, i;
    pbmp_t      port_pbm, empty_pbm;
    int         o_l;
    int         skip_vlan = 0;
    int         dmod = 0;
    int        level = 0;
#ifdef BCM_HIGIG2_SUPPORT
    soc_higig2_hdr_t    xgh;
    uint32              *xghp = (uint32 *)&xgh;
#elif defined(BCM_XGS_SUPPORT)
    soc_higig_hdr_t     xgh;
    uint32              *xghp = (uint32 *)&xgh;
#else
    uint32              *xghp = NULL;
#endif
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(lw->lw_unit)) {
        BCM_IF_ERROR_RETURN(bcm_stk_my_modid_get(lw->lw_unit, &dmod));
        if (IS_ST_PORT(lw->lw_unit, lw->lw_lp->lp_rx_port)) {
            skip_vlan = 1;
        }
    }
#endif

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Random: Tx-chan(%d) Rx-chan(%d) l=%d pkt-cnt=%d "
                       "chn-cnt=%d dv-cnt=%d\n"),
              tx_chan, rx_chan, l, p_cnt, c_cnt, dv_cnt));

    SOC_PBMP_CLEAR(empty_pbm);
    SOC_PBMP_CLEAR(port_pbm);
    SOC_PBMP_PORT_ADD(port_pbm, lp->lp_tx_port);

    /*
     * For number of packets desired, fill in DV/DCB, each
     * packet is allowed a maximum of #remaining
     * DCBs/#remaining packets.
     */

    dcb_cnt = c_cnt * dv_cnt;           /* Total DCB count */

    rx_dcb_remain = dcb_cnt;
    tx_dcb_remain = dcb_cnt;

    tx_dv_idx = 0;
    rx_dv_idx = 0;

    for (p_idx = 0; p_idx < p_cnt; p_idx++) {
        int     tmp_dcb_cnt;
        int     tx_align = 1;

        if (soc_feature(lw->lw_unit, soc_feature_pkt_tx_align)) {
            tx_align = 4;
        }

        /* Build packet */

        lb_xgs3_higig_setup(lw, 0, dmod,
                            lp->lp_tx_port, lp->lp_vlan, xghp);

        o_l = l;
        lb_fill_packet_tx(lw, lp, lw->lw_tx_packets[p_idx], &l,
                          lw->lw_mac_dst, lw->lw_mac_src);

        tmp_dcb_cnt = tx_dcb_remain / (p_cnt - p_idx);
        tx_dv_idx =
            lb_random_setup_dv(lw->lw_unit, lw->lw_tx_packets[p_idx],
                               l, tx_align, &tmp_dcb_cnt,
                               tx_dv_idx, c_cnt, lw->lw_tx_dv,
                               port_pbm, lp->lp_ubm, 0, xghp);
        tx_dcb_remain -= tmp_dcb_cnt;
        tmp_dcb_cnt = rx_dcb_remain / (p_cnt - p_idx);
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if ((SOC_IS_XGS3_SWITCH(lw->lw_unit)) &&
            (IS_ST_PORT(lw->lw_unit, lw->lw_lp->lp_rx_port))) {
            l += 4;
        }
#endif
        rx_dv_idx =
            lb_random_setup_dv(lw->lw_unit, NULL, l, 4,
                               &tmp_dcb_cnt,
                               rx_dv_idx, c_cnt, lw->lw_rx_dv,
                               empty_pbm, empty_pbm, 0, NULL);
        l = o_l;
        rx_dcb_remain -= tmp_dcb_cnt;

        lw->lw_tx_bytes += l;
        lw->lw_tx_count++;
    }

    /* Now fire off dma for Tx and Rx */

    lw->lw_eoc_tx = FALSE;
    lw->lw_eoc_rx = FALSE;

    rx_rv = SOC_E_NONE;
    tx_rv = SOC_E_NONE;

    for (i = 0; (rx_rv == SOC_E_NONE) && (i <= rx_dv_idx); i++) {
        dv_t    *dv, *dv_start = lw->lw_rx_dv[i];

        for (dv = dv_start; dv->dv_chain; i++, dv = dv->dv_chain)
            ;

        if (i == rx_dv_idx) {           /* Last chain */
            /*
             * dv_start points to beginning of LAST dv-joined chain, set
             * NOTIFY_CHN in that DV only.
             */
            dv_start->dv_flags |= DV_F_NOTIFY_CHN;
            lw->lw_rx_dv_chain_done = dv_start;
        }

        if ((rx_rv = soc_dma_start(lw->lw_unit, rx_chan, dv_start)) < 0) {
            break;
        }
    }

    for (i = 0;
         (rx_rv == SOC_E_NONE) && (tx_rv == SOC_E_NONE) && i <= tx_dv_idx;
         i++) {
        dv_t    *dv, *dv_start = lw->lw_tx_dv[i];

        for (dv = dv_start; dv->dv_chain; i++, dv = dv->dv_chain)
            ;

        if (i == tx_dv_idx) {
            /*
             * dv_start points to beginning of LAST dv-joined chain, set
             * NOTIFY_CHN in that DV only.
             */
            dv_start->dv_flags |= DV_F_NOTIFY_CHN;
            lw->lw_tx_dv_chain_done = dv_start;
        }

        if ((tx_rv = soc_dma_start(lw->lw_unit, tx_chan, dv_start)) < 0) {
            break;
        }
    }

    if (tx_rv < 0 || rx_rv < 0) {
        rv = -1;
        test_error(lw->lw_unit,
                   "Unexpected Tx/Rx Status: tx=%s rx=%s\n",
                   soc_errmsg(tx_rv), soc_errmsg(rx_rv));
    } else {
        int     dv_idx = 0;

        /* Wait for completion */

        while (!lw->lw_eoc_tx || !lw->lw_eoc_rx) {
            if (sal_sem_take(lw->lw_sema, lw->lw_timeout_usec)) {
                cli_out("Time-out waiting for completion "
                        "Tx(%s)=%s Rx(%s)=%s\n",
                        SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_tx_port),
                        lw->lw_eoc_tx ? "Done" : "Pending",
                        SOC_PORT_NAME(lw->lw_unit, lw->lw_lp->lp_rx_port),
                        lw->lw_eoc_rx ? "Done" : "Pending");
                rv = -1;
                break;
            } else {
               level = sal_splhi();
                lw->lw_sema_woke = FALSE;
               sal_spl(level);
            }
        }

        /*
         * Copy data out into normal rcv buffers - checking
         * the start/end sentinels.
         */

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(lw->lw_unit)) {
        /* Update length */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("lb_random_restore_dv: l = %d\n"), l));
        l += SOC_HIGIG_HDR_SIZE;
    }
#endif
        for (p_idx = 0; !rv && (p_idx < p_cnt); p_idx++) {
            rv |= lb_random_restore_dv(lw->lw_unit,
                                       lw->lw_rx_packets[p_idx],
                                       l + (skip_vlan ? 4 : 0),
                                       lw->lw_rx_dv,
                                       &dv_idx, dv_cnt);
            if (rv) {
                break;
            }
        }

        /* Verify Data */

        if (!rv && lp->lp_check_data) {
            for (p_idx = 0; p_idx < p_cnt; p_idx++) {
                rv |= lb_check_packet(lw,
                                      lw->lw_tx_packets[p_idx], l,
                                      lw->lw_rx_packets[p_idx], 
                                      l + (skip_vlan ? 4 : 0),
                                      lp->lp_check_crc,
                                      lw->lw_mac_dst, lw->lw_mac_src, skip_vlan);
                lw->lw_rx_bytes += l;
                lw->lw_rx_count++;
            }
        }
    }

    /* If an error occurred, dump the DVs now */

    if (rv) {
        lb_dump_dv(lw->lw_unit, "Rand-Tx:", lw->lw_tx_dv, tx_dv_idx + 1);
        lb_dump_dv(lw->lw_unit, "Rand-Rx:", lw->lw_rx_dv, rx_dv_idx + 1);
        test_error(lw->lw_unit, "Something went wrong folks\n");
        soc_dma_abort(lw->lw_unit);  /* Abort active DMAs before reset_dv */
    }

    /* Free up allocated memory */

    lb_reset_dv(lw->lw_unit, lw->lw_rx_dv, dv_cnt);
    lb_reset_dv(lw->lw_unit, lw->lw_tx_dv, dv_cnt);

    lw->lw_tx_dv_chain_done = NULL;
    lw->lw_rx_dv_chain_done = NULL;

    return(rv);
}

int
lb_random_exec(int unit, loopback_test_t *lw, int dry_run)
{
    loopback_testdata_t *lp = lw->lw_lp;
    dma_chan_t  tx_chan, rx_chan, chan;
    int         c_cur, l_cur, p_cur, dv_cur;
    int         rv = 0;

    if(soc_feature(lw->lw_unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    for (tx_chan = 0; tx_chan < chan; tx_chan++) {
#ifdef INCLUDE_KNET
        if (SOC_KNET_MODE(unit)) {
            /* Currently KNET uses chan 0 for tx */
            if (tx_chan != KCOM_DMA_TX_CHAN) {
                continue;
            }
        }
#endif /* INCLUDE_KNET */
        if (soc_dma_chan_config(lw->lw_unit, tx_chan,
                                DV_TX, SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
            test_error(lw->lw_unit,
                       "Unable to configure TX channel: %d\n",
                       tx_chan);
            break;
        }

        for (rx_chan = 0; rx_chan < chan; rx_chan++) {
#ifdef INCLUDE_KNET
            if (SOC_KNET_MODE(unit)) {
                /* Currently KNET uses chan 1 for rx */
                if (rx_chan != KCOM_DMA_RX_CHAN) {
                    continue;
                }
            }
#endif /* INCLUDE_KNET */
            if (rx_chan == tx_chan) {
                continue;
            }
            if (soc_dma_chan_config(lw->lw_unit, rx_chan,
                                    DV_RX, SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
                test_error(lw->lw_unit,
                           "Unable to configure RX channel: %d\n",
                           rx_chan);
                break;
            }
#ifdef BCM_CMICM_SUPPORT
            /* Assign all COS to this channel
             * Though this is added for CMICm, 
             * it should be OK if we do this for CMICe as well */
            if(soc_feature(lw->lw_unit, soc_feature_cmicm)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META("Assign all COS to channel: 1\n")));
                SOC_IF_ERROR_RETURN(
                    bcm_rx_queue_channel_set(lw->lw_unit, -1, rx_chan));

#if defined(BCM_KATANA2_SUPPORT)
                if(SOC_IS_KATANA2(unit)) {
                    uint32 reg_val, reg_addr;

                    /* Including CPU L0 bitmap to support CMIC backpressure */
                    reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, rx_chan);
                    reg_val = soc_pci_read(unit, reg_addr);
                    reg_val |= (1 << 16);
                    soc_pci_write(unit, reg_addr, reg_val);
                }
#endif

            }
#endif
#ifdef BCM_CMICX_SUPPORT
            /* Assign all COS to this channel */
            if(soc_feature(lw->lw_unit, soc_feature_cmicx)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META("Assign all COS to channel: 1\n")));
                SOC_IF_ERROR_RETURN(
                    bcm_rx_queue_channel_set(lw->lw_unit, -1, rx_chan));
            }
#endif
            for (p_cur = lp->lp_ppc_start;
                 !rv && p_cur <= lp->lp_ppc_end;
                 p_cur += lp->lp_ppc_inc) {
                for (c_cur = lp->lp_c_start;
                     !rv && c_cur<=lp->lp_c_end;
                     c_cur += lp->lp_c_inc) {
                    for (l_cur = lp->lp_l_start;
                         !rv && l_cur <= lp->lp_l_end;
                         l_cur += lp->lp_l_inc) {
                        for (dv_cur = lp->lp_dv_start;
                             !rv && dv_cur <= lp->lp_dv_end;
                             dv_cur += lp->lp_dv_inc) {
                            if (p_cur >= c_cur * dv_cur) {
                                continue;
                            }

                            if (dry_run) {
                                lw->lw_tx_total += p_cur;
                            } else if (lb_random_pkt(lw, lp,
                                                     tx_chan, rx_chan,
                                                     l_cur, p_cur, c_cur,
                                                     dv_cur) < 0) {
                                rv = -1;
                                break;
                            } else {
                                lb_stats_report(lw);
                            }
                        }
                    }
                }
            }

            if (soc_dma_chan_config(lw->lw_unit, rx_chan,
                                    DV_NONE, SOC_DMA_F_INTR)) {
                test_error(lw->lw_unit,
                           "Unable to de-configure RX channel: %d\n",
                           rx_chan);
                break;
            }
        }

        if (soc_dma_chan_config(lw->lw_unit, tx_chan,
                                DV_NONE, SOC_DMA_F_INTR)) {
            test_error(lw->lw_unit,
                       "Unable to de-configure TX channel: %d\n",
                       tx_chan);
            break;
        }
    }

    return(rv);
}

int
lb_random_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_random_test
 * Purpose:     Perform random packet loopback.
 * Parameters:  lw - pointer to work structure.
 *              lp - pointer to work parameters.
 *              pa - parameter returned from lb_random_init.
 * Returns:     0 - success
 *              -1 - failed
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = lw->lw_lp;
    int         rv, count;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(a);

    if ((lb_is_xgs_fabric(unit)) || (SOC_IS_XGS3_SWITCH(unit))) {
        int port;
        PBMP_PORT_ITER(unit, port) {
            break;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            test_error(unit, "No Ports available for loopback\n");
            return(-1);
        }
        lp->lp_tx_port = port;
        lp->lp_rx_port = port;

        _lb_max_frame_set(lw, lp);
        if ((rv = bcm_port_loopback_set(unit, port,
                                        BCM_PORT_LOOPBACK_MAC)) < 0) {
            test_error(unit,
                   "Port %s: Failed to set MAC loopback: %s\n",
                   SOC_PORT_NAME(unit, 1), bcm_errmsg(rv));
            return(-1);
        }

        if (IS_HG_PORT(unit, port)) {
            bcm_port_pause_set(unit, port, FALSE, FALSE);
        }
        /*
         * Get our current MAC address for this port and be sure the
         * SOC will route packets to the CPU.
         */
        if (lb_setup_arl(lw, unit, CMIC_PORT(unit), port)) {
            return(-1);
        }
        if (SOC_IS_XGS3_SWITCH(unit)) {
            pbmp_t              temp_pbmp;
            SOC_PBMP_CLEAR(temp_pbmp);
            /*
             * Setup VLAN
             */
            SOC_PBMP_PORT_ADD(temp_pbmp, CMIC_PORT(unit));
            SOC_PBMP_PORT_ADD(temp_pbmp, port);
			if ((SOC_DPP_PP_ENABLE(unit) && SOC_IS_ARAD(unit)) || (!SOC_IS_ARAD(unit))) {
	            if ((rv = bcm_vlan_port_add(unit, lp->lp_vlan,
	                                        temp_pbmp, temp_pbmp)) < 0) {
	                test_error(unit,
	                           "Could not add all ports to VLAN %d: %s\n",
	                           lp->lp_vlan, bcm_errmsg(rv));
	                return (-1);
	            }
			}
        }

        if ((rv = lb_setup_port(unit, port, LB_SPEED_MAX, FALSE)) < 0) {
            test_error(unit,
                       "Port %s: Port setup failed: %s\n",
                       SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            return(-1);
        }
    }

    lb_stats_init(lw);

    if (lp->lp_seed) {
        sal_srand(lp->lp_seed);
    }

    lw->lw_tx_dcb_factor = 1;

    /*
     * First do a dry run without actually sending packets, as a lazy
     * way to calculate the total # packets for statistics reporting.
     */

    lw->lw_tx_total = 0;

    for (count = 0; count < lp->lp_count; count++) {
        if ((rv = lb_random_exec(unit, lw, 1)) < 0) {
            return rv;
        }
    }

    test_msg("LB: total %d pkt\n", lw->lw_tx_total);

    for (count = 0; count < lp->lp_count; count++)
        if ((rv = lb_random_exec(unit, lw, 0)) < 0)
            return rv;

    lb_stats_done(lw);

    return 0;
}

#ifdef LB_PBM_TEST
int
lb_pbm_test(loopback_test_t *lw, loopback_testdata_t *lp)
/*
 * Function:    lb_pbm_test
 * Purpose:     Test that a PBM of 0 results in 0-bytes transferred,
 *              and does not hang DMA engine.
 * Parameters:  lw - pointer to work structure.
 *              lp - pointer to parameters.
 * Returns:     0 - success, -1 - failed.
 */
{
    int         rv = 0;
    soc_dma_t   srv;
    dv_t        *dv = lw->lw_tx_dv[0];
    dma_chan_t  channel, chan;
    dcb_type_t  dt = dv->dv_dcb_type;

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    for (channel = 0; channel < chan; channel++) {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META("PBMP(0) testing Tx channel %d\n"), channel));

        if (soc_dma_chan_config(lw->lw_unit, channel, DV_TX)) {
            test_error(lw->lw_unit,
                       "Unable to configure TX channel: %d\n",
                       channel);
            rv = -1;
            continue;
        }

        soc_dma_dv_reset(DV_TX, dv);

        dv->dv_flags |= DV_F_NOTIFY_CHN | DV_F_WAIT;

        DMAC_INIT(dmac, 64);
        soc_dma_desc_add(dv, 0xdeadbeef, dmac, 0, 0, NULL);
        soc_dma_desc_end_packet(dv);

        if (soc_dmaChainDone !=
            (srv = soc_dma_start(lw->lw_unit, channel, dv))) {
            test_error(lw->lw_unit, "Unexpected DMA status: %d\n", srv);
            rv = -1;
        } else {
            dcb_get_dmas0(dt, dv->dv_dcb, &dmas0);
            dcb_get_dmas1(dt, dv->dv_dcb, &dmas1);
            if ((SOC_DCB_XFERCOUNT_GET(unit, dv->dv_dcb != 0) 
                 || !SOC_DCB_DONE_GET(unit, dv->dv_dcb) {
                soc_dma_dump_dv(lw->lw_unit, "PBM: ", dv);
                test_error(lw->lw_unit,
                           "PBM of 0 resulted in unexpected status\n");
                rv = -1;
            }
        }

        if (soc_dma_chan_config(lw->lw_unit, channel, DV_NONE)) {
            test_error(lw->lw_unit,
                       "Unable to de-configure TX channel: %d\n",
                       channel);
            rv = -1;
            continue;
        }
    }

    lb_stats_done(lw);

    return(rv);
}
#endif /* LB_PBM_TEST */

int
lb_reload_test(int unit, args_t *a, void *pa)
/*
 * Function:    lb_reload_test
 * Purpose:     Test RLD option is dma descriptor.
 * Parameters:  unit - unit
 *              a - pointer to args (ignored)
 *              pa - parameter returned from lb_reload_init.
 * Returns:     0 - success
 *              -1 - failed
 */
{
    loopback_test_t     *lw = (loopback_test_t *)pa;
    loopback_testdata_t *lp = lw->lw_lp;
    int                 channel, rv, tx, d, chan;
    int                 soc_dma_rv;
    static sal_vaddr_t  reload_addr = (sal_vaddr_t) NULL;
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    int cmc = SOC_PCI_CMC(unit);
#endif

    COMPILER_REFERENCE(a);
    lb_stats_init(lw);
    rv = 0;

    if(soc_feature(unit, soc_feature_cmicx)) {
        chan = CMICX_N_DMA_CHAN;
    } else {
        chan = N_DMA_CHAN;
    }

    if (lp->lp_seed) {
        sal_srand(lp->lp_seed);
    }

    if (reload_addr == (sal_vaddr_t) NULL) {
        reload_addr = (sal_vaddr_t) soc_cm_salloc(unit, 1, "rld");
    }

    lw->lw_tx_dcb_factor = 1;

    /*
     * Create a simple dcb chain with only a RLD, verify descriptor
     * address register is updated.  This tests RLD=1, CHN=0.
     */

    for (tx = 0; tx <= 1; tx++) {
        for (channel = 0; channel < chan; channel++) {
            sal_vaddr_t dma_desc;
            dv_t        **dv_array = tx ? lw->lw_tx_dv : lw->lw_rx_dv;

            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Basic DCB Reload %s Channel %d\n"),
                      tx ? "Tx" : "Rx", channel));

            if (soc_dma_chan_config(unit, channel,
                                    tx ? DV_TX : DV_RX, 
                                    SOC_DMA_F_INTR | SOC_DMA_F_DEFAULT)) {
                test_error(lw->lw_unit,
                           "Unable to configure %s channel: %d\n",
                           tx ? "Tx" : "Rx", channel);
                break;
            }

#ifdef BCM_CMICM_SUPPORT
            /* Assign all COS to this channel
             * Though this is added for CMICm, 
             * it should be OK if we do this for CMICe as well */
            if(soc_feature(lw->lw_unit, soc_feature_cmicm) && (!tx)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit,
                                     "Assign all COS to channel: 1\n")));
                SOC_IF_ERROR_RETURN(
                    bcm_rx_queue_channel_set(unit, -1, channel));

#if defined(BCM_KATANA2_SUPPORT)
                if(SOC_IS_KATANA2(unit)) {
                    uint32 reg_val, reg_addr;

                    /* Including CPU L0 bitmap to support CMIC backpressure */
                    reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(0, channel);
                    reg_val = soc_pci_read(unit, reg_addr);
                    reg_val |= (1 << 16);
                    soc_pci_write(unit, reg_addr, reg_val);
                }
#endif

            }
#endif
#ifdef BCM_CMICX_SUPPORT
            /* Assign all COS to this channel */
            if(soc_feature(lw->lw_unit, soc_feature_cmicx) && (!tx)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit,
                                     "Assign all COS to channel: 1\n")));
                SOC_IF_ERROR_RETURN(
                    bcm_rx_queue_channel_set(unit, -1, channel));
            }
#endif

            for (d = lp->lp_dv_start; d <= lp->lp_dv_end; d += lp->lp_dv_inc) {
                int     i;

                rv = 0;
                LOG_INFO(BSL_LS_APPL_TESTS,
                         (BSL_META_U(unit,
                                     "Testing %cx channel %d: %d independent chains\n"),
                          tx ? 'T' : 'R', channel, d));

                for (i = 0; i < d; i++) {
                    dv_t        *dv = dv_array[i];

                    soc_dma_dv_reset(tx ? DV_TX : DV_RX, dv);
                    if (i == 0) {
                      SET_NOTIFY_CHN_ONLY(dv->dv_flags);
                    } else {
                        dv->dv_flags &= ~(DV_F_NOTIFY_CHN|DV_F_NOTIFY_DSC);
                        soc_dma_dv_join(dv_array[i - 1], dv);
                    }
                }

                /* Add Reload DCB and End of Packet to last DV */
                /* coverity[check_return] */
                soc_dma_rld_desc_add(dv_array[d - 1], reload_addr);
                soc_dma_desc_end_packet(dv_array[d - 1]);

                if (tx) {
                    lw->lw_tx_dv_chain_done = dv_array[0];
                } else {
                    lw->lw_rx_dv_chain_done = dv_array[0];
                }

                if ((soc_dma_rv = soc_dma_start(unit, channel,
                                                dv_array[0])) < 0) {
                    cli_out("%s Channel %d: Unexpected DMA start: %s\n",
                            tx ? "Tx" : "Rx",
                            channel, soc_errmsg(soc_dma_rv));
                    rv = -1;
                }

                while (!rv) {
                    if (sal_sem_take(lw->lw_sema, lw->lw_timeout_usec)) {
                        lb_dump_dv(lw->lw_unit, "Rld-TO:",
                                   tx ? lw->lw_tx_dv : lw->lw_rx_dv, d);
                        test_error(lw->lw_unit,
                                   "Time-out waiting for completion: "
                                   "%cx on channel %d\n",
                                   tx ? 'T' : 'R', channel);
                        rv = -1;
                    } else {
                        lw->lw_sema_woke = FALSE;
                    }

                    if ((tx && lw->lw_eoc_tx) || (!tx && lw->lw_eoc_rx)) {
                        break;          /* while */
                    }

                    /* Verify DMA address register reloaded */

#ifdef BCM_CMICM_SUPPORT
                    if (soc_feature(unit, soc_feature_cmicm)) {
                        dma_desc = (sal_vaddr_t) soc_cm_p2l(unit,
                             soc_pci_read(unit,
                                 CMIC_CMCx_DMA_DESCy_OFFSET(cmc, channel)));
                    } else 
#endif /* CMICM Support */
#ifdef BCM_CMICX_SUPPORT
                    if (soc_feature(unit, soc_feature_cmicx)) {
                        dma_desc = soc_cmicx_pktdma_desc_addr_get(unit, cmc, channel);
                    } else
#endif /* CMICX Support */
                    {
                        dma_desc = (sal_vaddr_t) soc_cm_p2l(unit,
                            soc_pci_read(unit, CMIC_DMA_DESC(channel)));
                    }

                    if (dma_desc != reload_addr) {
                        cli_out("%s channel %d: Expected ADDR=%p "
                                "Received ADDR=%p\n",
                                tx ? "Tx" : "Rx", channel,
                                (void *)reload_addr, (void *)dma_desc);
                        rv = -1;
                    }
                }

                /*
                 * Check all the DCBs are marked correct.
                 */

                for (i = 0; !rv && (i < d); i++) {
                    dcb_t       *dcb = dv_array[i]->dv_dcb;

                    if (!SOC_DCB_DONE_GET(unit, dcb)) {
                        rv = -1;
                        cli_out("DCB[%d] Done expected but not set\n", i);
                    }

                    if (SOC_DCB_RX_START_GET(unit, dcb) ||
                        SOC_DCB_RX_END_GET(unit, dcb)) {
                        cli_out("DCB[%d] SW/EW set but not expected\n", i);
                        rv = -1;
                    }
                }

                if (rv) {
                    /* Fail test, but allow to continue if desired */

                    if (tx) {
                        lb_dump_dv(unit, "Tx:", lw->lw_tx_dv, d);
                    } else {
                        lb_dump_dv(unit, "Rx:", lw->lw_rx_dv, d);
                    }

                    test_error(unit, "Test Failed\n");
                    /*
                     * Abort active DMAs before reset_dv
                     */
                    soc_dma_abort(unit);
                }

                /*
                 * Reset DVs.
                 */

                lb_reset_dv(unit, dv_array, d);
            }

            if ((rv |= soc_dma_chan_config(unit, channel,
                                           DV_NONE, SOC_DMA_F_INTR))) {
                cli_out("%s Channel %d: unable to deconfigure\n",
                        tx ? "Tx" : "Rx", channel);
            }
        }
    }

#ifdef LB_PBM_TEST
    rv |= lb_pbm_test(lw, lp);
#endif

    return(rv);
}
#endif /* BCM_ESW_SUPPORT */
