/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        post.c
 * Purpose:     Run MAC-Phy connectivity check after Reset init
 */

#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/pbsmh.h>
#include <soc/post.h>
#include <soc/debug.h>
#include <soc/phyctrl.h>
#include <soc/error.h>
#include <soc/feature.h>
#include <soc/mem.h>
#include <soc/firebolt.h>
#include <shared/bsl.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef  BCM_RAPTOR_SUPPORT

#define SOC_RAPTOR_POST_MAX_COUNT 25

#define SOC_RAPTOR_POST_BUF_SIZE 256
#define SOC_RAPTOR_POST_PKT_SIZE 64

/* microsec */
#ifdef BCM_ICS
/* Same time on Quickturn as well */
#define DMA_DONE_TIMEOUT 100000
#else
#define DMA_DONE_TIMEOUT \
    (SAL_BOOT_SIMULATION ? 100000000 : 100000)
#endif

#define SOC_DEFAULT_RCPU_TRX_PORT     (4) /* GMII port */

/*
 * Function:
 *      _soc_dma_tx_packet_port
 * Purpose:
 *      TX a packet
 * Parameters:
 *      unit         - Unit on which being transmitted
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_dma_tx_packet_port(int unit, dma_chan_t c, soc_port_t port, uint32 *tx_pkt)
{
    int         rv = SOC_E_NONE;

    dv_t        dvs;
    uint32      ctrl;
    dv_t        *dv = &dvs;
    dcb_t       *dcb;
    uint32      flags = 0;
    int         tx_unit = unit;
    soc_pbsmh_hdr_t phh_s;
    soc_pbsmh_hdr_t *pbh = &phh_s;

    if (tx_pkt == NULL) {
        return SOC_E_NONE;
    }

#ifdef INCLUDE_RCPU
    if (SOC_IS_RCPU_UNIT(unit)) {
        port = soc_property_get(unit, spn_RCPU_PORT, SOC_DEFAULT_RCPU_TRX_PORT);
        tx_unit = soc_property_get(unit, spn_RCPU_MASTER_UNIT, 0);;
    }
#endif /* INCLUDE_RCPU */

    sal_memset(dv, 0, sizeof(*dv));
    dv->dv_unit    = tx_unit;
    dcb = dv->dv_dcb = tx_pkt;

    dv->dv_op      = DV_TX;
    dv->dv_magic   = 0xba5eba11;
    dv->dv_channel = c;
    dv->dv_cnt     = 1;
    SOC_DMA_HG_SET(flags, 1);
    PBS_MH_W0_START_SET(pbh);
    PBS_MH_W1_RSVD_SET(pbh);
    PBS_MH_V2_W2_SMOD_DPORT_COS_SET(pbh, 0, port, 0, 0, 0);

    rv = SOC_DCB_ADDTX(tx_unit, dv,
        (sal_vaddr_t) SOC_DCB_IDX2PTR(dv->dv_unit, tx_pkt + 16, dv->dv_vcnt),
        SOC_RAPTOR_POST_PKT_SIZE,   /* Min packet size */
        PBMP_ALL(tx_unit), /* Not used */
        PBMP_ALL(tx_unit), /* Not used */
        PBMP_ALL(tx_unit), /* Not used */
        flags,          /* SOB MH */
        (uint32 *)pbh   /* SOB MH */
        );
        /* Mark the end of the packet */
        SOC_DCB_SG_SET(dv->dv_unit, dcb, 0);
    if (rv >= 0) {
        soc_cm_sflush(tx_unit, dv->dv_dcb, SOC_DCB_SIZE(tx_unit));
        if (SOC_IS_RCPU_UNIT(unit)) {
#ifdef INCLUDE_RCPU
            SOC_IF_ERROR_RETURN(soc_dma_wait(tx_unit, dv));
#endif /* INCLUDE_RCPU */
        } else {
            sal_usecs_t start_time;
            int         diff_time;
            uint32      dma_stat;
            uint32      dma_state; 

            ctrl = soc_pci_read(unit, CMIC_DMA_CTRL);
            soc_pci_write(unit, CMIC_DMA_CTRL, ctrl | DC_MEM_TO_SOC(c));
            soc_pci_write(unit, CMIC_DMA_DESC(c),
                          soc_cm_l2p(unit, dv->dv_dcb));
            /* Start DMA */
            soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_SET(c));

            start_time = sal_time_usecs();
            diff_time = 0;
            dma_state = (DS_DESC_DONE_TST(c) | DS_CHAIN_DONE_TST(c));
            do {
                sal_udelay(SAL_BOOT_SIMULATION ? 1000 : 100);
                dma_stat = soc_pci_read(unit, CMIC_DMA_STAT) &
                    (DS_DESC_DONE_TST(c) | DS_CHAIN_DONE_TST(c));
                diff_time = SAL_USECS_SUB(sal_time_usecs(), start_time);
                if (diff_time > DMA_DONE_TIMEOUT) { /* 10 Sec(QT)/10 msec */
                    rv = SOC_E_TIMEOUT;
                    break;
                } else if (diff_time < 0) {
                    /* Restart in case system time changed */
                    start_time = sal_time_usecs();
                }
            } while (dma_stat != dma_state);
            /* Clear CHAIN_DONE and DESC_DONE */
            soc_pci_write(unit, CMIC_DMA_STAT, DS_DMA_EN_CLR(c));
            soc_pci_write(unit, CMIC_DMA_STAT, DS_DESC_DONE_CLR(c));
            soc_pci_write(unit, CMIC_DMA_STAT, DS_CHAIN_DONE_CLR(c));
            soc_pci_write(unit, CMIC_DMA_CTRL, ctrl);
        }
    }
    return rv;
}
/*
 * Function:
 *      _soc_raptor_port_probe
 * Purpose:
 *      Probe the phy and set up the phy and mac of the indicated port
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - Port to probe
 *      okay - Output parameter indicates port can be enabled.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_INTERNAL - internal error.
 * Notes:
 *      If error is returned, the port should not be enabled.
 */

STATIC int
_soc_raptor_port_probe(int unit, soc_port_t p, mac_driver_t **macdp)
{
    int                 rv;
    mac_driver_t        *macd;

    *macdp = NULL;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Init port %d PHY...\n"), p));

    if ((rv = soc_phyctrl_probe(unit, p)) < 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Port %s: Failed to probe PHY: %s\n"),
                     SOC_PORT_NAME(unit, p), soc_errmsg(rv)));
        return rv;
    }

    if ((rv = soc_phyctrl_init(unit, p)) < 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Port %s: Failed to initialize PHY: %s\n"),
                     SOC_PORT_NAME(unit, p), soc_errmsg(rv)));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Init port %d MAC...\n"), p));

    if ((rv = soc_mac_probe(unit, p, &macd)) < 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Port %s: Failed to probe MAC: %s\n"),
                     SOC_PORT_NAME(unit, p), soc_errmsg(rv)));
        return rv;
    }

    *macdp = macd;
    if ((rv = MAC_INIT(macd, unit, p)) < 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Port %s: Failed to initialize MAC: %s\n"),
                     SOC_PORT_NAME(unit, p), soc_errmsg(rv)));
        return rv;
    }

    return(SOC_E_NONE);
}


STATIC int
_soc_raptor_port_tx_ok(int unit, soc_port_t port)
{
    uint32 grpkt;

    SOC_IF_ERROR_RETURN(READ_GRFCSr(unit, port, &grpkt));
    if (grpkt) {
        return SOC_E_FAIL;
    }

    SOC_IF_ERROR_RETURN(READ_GRPKTr(unit, port, &grpkt));
    if (grpkt) {
        return SOC_E_NONE;
    } else {
        return SOC_E_FAIL;
    }
}

#ifdef INCLUDE_RCPU

#define TABLE_HAS_UT_BITMAP(unit, table)                        \
     ((SOC_IS_FBX(unit) && (table) == EGR_VLANm) || \
     (SOC_IS_KATANA2 (unit) && (table) == VLAN_TABm))

#define TABLE_HAS_T_BITMAP(unit, table)         \
    (((table) == VLAN_TABm)   ||                 \
     (soc_feature(unit, soc_feature_egr_vlan_check) && (table) == EGR_VLANm))

#define TABLE_HAS_PFM(unit, table)                              \
    (((table) == VLAN_TABm) ||                                  \
     (soc_feature(unit, soc_feature_egr_vlan_pfm) && (table) == EGR_VLANm))

#define STG_BITS_PER_PORT       2
#define STG_PORT_MASK           ((1 << STG_BITS_PER_PORT)-1)
#define STG_PORTS_PER_WORD      (32 / STG_BITS_PER_PORT)
#define STG_WORD(port)          ((port) / STG_PORTS_PER_WORD)
#define STG_BITS_SHIFT(port)    \
        (STG_BITS_PER_PORT * ((port) % STG_PORTS_PER_WORD))
#define STG_BITS_MASK(port)     (STG_PORT_MASK << (STG_BITS_SHIFT(port)))

STATIC int
_soc_raptor_post_create_vlan(int unit, int vid, 
                             pbmp_t pbmp, pbmp_t upbmp, int table)
{
    vlan_tab_entry_t         vt;
    vlan_profile_tab_entry_t vp;
    uint32                   bmval, ptr, fldval;

    sal_memcpy(&vt, soc_mem_entry_null(unit, table),
               soc_mem_entry_words(unit, table) * 4);

    soc_mem_field32_set(unit, table, &vt, STGf, 1);
    soc_mem_field32_set(unit, table, &vt, VALIDf, 1);
    if (TABLE_HAS_PFM(unit, table)) {
        soc_mem_field32_set(unit, table, &vt, PFMf, 1);
    }

    if (TABLE_HAS_T_BITMAP(unit, table)) {
        bmval = SOC_PBMP_WORD_GET(pbmp, 0);
        soc_mem_field_set(unit, table, (uint32 *) &vt,
                          PORT_BITMAP_LOf, &bmval);
        bmval = SOC_PBMP_WORD_GET(pbmp, 1);
        soc_mem_field_set(unit, table, (uint32 *) &vt,
                          PORT_BITMAP_HIf, &bmval);
    }

    if (TABLE_HAS_UT_BITMAP(unit, table)) {
        bmval = SOC_PBMP_WORD_GET(upbmp, 0);
        soc_mem_field_set(unit, table, (uint32 *) &vt,
                          UT_BITMAP_LOf, &bmval);
        bmval = SOC_PBMP_WORD_GET(upbmp, 1);
        soc_mem_field_set(unit, table, (uint32 *) &vt,
                          UT_BITMAP_HIf, &bmval);
    }

    if (SOC_MEM_IS_VALID(unit, VLAN_PROFILE_TABm) && table == VLAN_TABm) {
        ptr = 7;
        soc_mem_field_set(unit, table, (uint32 *) &vt,
                          VLAN_PROFILE_PTRf, &ptr);
        sal_memcpy(&vp, soc_mem_entry_null(unit, VLAN_PROFILE_TABm),
                   soc_mem_entry_words(unit, VLAN_PROFILE_TABm) * 4);
        fldval = 1;
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          L2_PFMf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPMCV6_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPMCV4_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPMCV6_L2_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPMCV4_L2_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPV6L3_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          IPV4L3_ENABLEf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          L3_IPV6_PFMf, &fldval);
        soc_mem_field_set(unit, VLAN_PROFILE_TABm, (uint32 *) &vp,
                          L3_IPV4_PFMf, &fldval);
        SOC_IF_ERROR_RETURN(soc_mem_write
                           (unit, VLAN_PROFILE_TABm, MEM_BLOCK_ALL, ptr, &vp));
    }

    return soc_mem_write(unit, table, MEM_BLOCK_ALL, (int) vid, &vt);
}

STATIC int
_soc_raptor_post_init_switch(int unit, pbmp_t pbmp)
{
    mac_driver_t *macd;
    int          speed, tmo;
    int          an = TRUE, an_done = FALSE, duplex = SOC_PORT_DUPLEX_FULL;
    port_tab_entry_t ptab;                 /* Port table entry. */
    uint32      entry[SOC_MAX_MEM_WORDS];    /* STP group ports state map.   */
    uint32      rcpu_port, rval;

    rcpu_port = soc_property_get(unit, spn_RCPU_PORT, SOC_DEFAULT_RCPU_TRX_PORT);

    /* 
     * Init Port 
     */
    /* Write PORT_TABm memory */
    sal_memcpy(&ptab, soc_mem_entry_null(unit, PORT_TABm), sizeof (ptab));

    /* Set default  vlan id(pvid) for port. */
    soc_PORT_TABm_field32_set(unit, &ptab, PORT_VIDf, 1);

    if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, OUTER_TPIDf)) {
        soc_PORT_TABm_field32_set(unit, &ptab, OUTER_TPIDf, 0x8100);
    }

    /* Enable mac based vlan classification. */
    soc_PORT_TABm_field32_set(unit, &ptab, MAC_BASED_VID_ENABLEf, 1);

    /* Enable subned based vlan classification. */
    soc_PORT_TABm_field32_set(unit, &ptab, SUBNET_BASED_VID_ENABLEf, 1);

    /* Set port type. */
    soc_PORT_TABm_field32_set(unit, &ptab, HIGIG_PACKETf, 0);

    SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, rcpu_port, &ptab));

    /* 
     * Init VLAN 
     */
    SOC_PBMP_PORT_ADD(pbmp, rcpu_port);

    SOC_IF_ERROR_RETURN(
       _soc_raptor_post_create_vlan(unit, 1, pbmp, pbmp, EGR_VLANm));

    SOC_IF_ERROR_RETURN(
        _soc_raptor_post_create_vlan(unit, 1, pbmp, pbmp, VLAN_TABm));

    /* Init STG */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, STG_TABm, MEM_BLOCK_ANY, 1, entry));

    /* set all the ports in forwarding state. */
    entry[0] = 0xffffffff;
    entry[1] = 0xffffffff;
    entry[2] = 0xffffffff;
    entry[3] = 0xfff;

    /* Write spanning tree group port states to hw. */
    SOC_IF_ERROR_RETURN(
            soc_mem_write(unit, STG_TABm, MEM_BLOCK_ALL, 1, entry));

    SOC_IF_ERROR_RETURN(
         soc_mem_write(unit, EGR_VLAN_STGm, MEM_BLOCK_ALL, 1, entry));

    /* Init MAC/PHY */

    /* 
     * If EB is connected through HiGig port, set the port mode to
     * ethernet 
     */
    SOC_IF_ERROR_RETURN(_soc_raptor_port_probe(unit, rcpu_port, &macd));
    SOC_IF_ERROR_RETURN(MAC_INTERFACE_SET(macd, unit, rcpu_port, 
                                                 SOC_PORT_IF_SGMII));

    if ((speed = 
         soc_property_port_get(unit, rcpu_port, spn_PORT_INIT_SPEED, -1)) != -1) {
        an = FALSE;
    }

    SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_set(unit, rcpu_port, an));
    if (an == TRUE) {
        tmo = (SAL_BOOT_SIMULATION) ? 300000000 : 3000000;
        while (
            (!soc_phyctrl_auto_negotiate_get(unit, rcpu_port, &an, &an_done)) &&
               (an_done == FALSE) && (tmo > 0)) {
            sal_udelay(100000);
            tmo -= 100000;
        }

        if (an_done == FALSE) {
            return SOC_E_INIT;
        }
    } 

    SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, rcpu_port, TRUE));

    SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, rcpu_port, TRUE));

    if (an == TRUE) {
        SOC_IF_ERROR_RETURN(soc_phyctrl_speed_get(unit, rcpu_port, &speed));
    }

    SOC_IF_ERROR_RETURN(MAC_SPEED_SET(macd, unit, rcpu_port, speed));

    if ((an == FALSE) && ((duplex = 
      soc_property_port_get(unit, rcpu_port, spn_PORT_INIT_DUPLEX, -1)) != -1)) {
        SOC_IF_ERROR_RETURN(MAC_DUPLEX_SET(macd, unit, rcpu_port, duplex));
    } else {
        SOC_IF_ERROR_RETURN
            (MAC_DUPLEX_SET(macd, unit, rcpu_port, SOC_PORT_DUPLEX_FULL));
    }

    /* Init EPC_LINK_MAP */
    rval = 0;
    soc_reg_field_set(unit, EPC_LINK_BMAPr, &rval, PORT_BITMAPf,
                          SOC_PBMP_WORD_GET(pbmp, 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPr(unit, rval));
    if (soc_feature(unit, soc_feature_register_hi)) {
        soc_reg_field_set(unit, EPC_LINK_BMAP_HIr, &rval, PORT_BITMAPf,
                              SOC_PBMP_WORD_GET(pbmp, 1));
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_HIr(unit, rval));
    }

    return SOC_E_NONE;
}
#endif /* INCLUDE_RCPU */

STATIC int
_soc_raptor_post_run(int unit, uint32 *tx_pkt)
{
    mac_driver_t *macd = NULL;
    soc_port_t   p;
    int          rv = 0;
    uint32       int_mask = 0;
    int          speed;
    int          count;
    soc_pbmp_t   fe_pbmp;
    port_tab_entry_t ptab;                 /* Port table entry. */

    /*
     * Self test needs to be executed only on fe12 and fe36.
     */
    SOC_PBMP_CLEAR(fe_pbmp);
    if (SOC_IS_RAVEN(unit)) {
        SOC_PBMP_ASSIGN(fe_pbmp, PBMP_FE_ALL(unit));
    } else {
        p = (12 + 6);                       /* fe12 */
        if (SOC_PBMP_MEMBER(PBMP_FE_ALL(unit), p)) {
            SOC_PBMP_PORT_ADD(fe_pbmp, p);      /* fe12 */
        }
        p = (36 + 6);                       /* fe36 */
        if (SOC_PBMP_MEMBER(PBMP_FE_ALL(unit), p)) {
            SOC_PBMP_PORT_ADD(fe_pbmp, p);  /* fe36 */
        }
    }
    if (SOC_PBMP_IS_NULL(fe_pbmp)) {
        return SOC_E_NONE;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Running post on unit %d \n"), unit));

    for(count = 0; count < SOC_RAPTOR_POST_MAX_COUNT; count++) {
        /*
         * Setup
         */
        SOC_IF_ERROR_RETURN(soc_phy_common_init(unit));
        int_mask = soc_intr_disable(unit, IRQ_DESC_DONE(0) | IRQ_CHAIN_DONE(0));
        PBMP_ITER(fe_pbmp, p) {
            SOC_IF_ERROR_RETURN(_soc_raptor_port_probe(unit, p, &macd));
            SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_set(unit, p, FALSE));
            SOC_IF_ERROR_RETURN(soc_phyctrl_loopback_set(unit, p, TRUE, TRUE));
            SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, p, TRUE));
            SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, p, TRUE));
            SOC_IF_ERROR_RETURN
                (soc_phyctrl_speed_get(unit, p, &speed));
            SOC_IF_ERROR_RETURN
                (MAC_SPEED_SET(macd, unit, p, speed));
            SOC_IF_ERROR_RETURN
                (MAC_DUPLEX_SET(macd, unit, p, SOC_PORT_DUPLEX_FULL));
            SOC_IF_ERROR_RETURN
                (MAC_PAUSE_SET(macd, unit, p, 0, 0));

#ifdef INCLUDE_RCPU
            /*
             * Setup slave device.
             */
            if (SOC_IS_RCPU_UNIT(unit)) {
                if ((rv = _soc_raptor_post_init_switch(unit, fe_pbmp)) < 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "Failed to init switch unit %d \n"), unit));
                    return rv;
                }
            }
#endif /* INCLUDE_RCPU */

            /* Filter packets received on port after loopback */
            sal_memcpy(&ptab, soc_mem_entry_null(unit, PORT_TABm), sizeof (ptab));
            soc_PORT_TABm_field32_set(unit, &ptab, PORT_VIDf, 1);
            soc_PORT_TABm_field32_set(unit, &ptab, PORT_DIS_TAGf, 1);
            soc_PORT_TABm_field32_set(unit, &ptab, PORT_DIS_UNTAGf, 1);
            if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, TRUST_INCOMING_VIDf)) {
                soc_PORT_TABm_field32_set(unit, &ptab, TRUST_INCOMING_VIDf, 1);
            }
            if (SOC_MEM_FIELD_VALID(unit, PORT_TABm, OUTER_TPIDf)) {
                soc_PORT_TABm_field32_set(unit, &ptab, OUTER_TPIDf, 0x8100);
            }
            if ((rv = soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, 
                                    p, &ptab)) < 0) {
                return rv;
            }
        }

        /*
         * Test Run
         */
        PBMP_ITER(fe_pbmp, p) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Port %s: Packet Send \n"),
                         SOC_PORT_NAME(unit, p)));
            SOC_IF_ERROR_RETURN(_soc_dma_tx_packet_port(unit, 0, p, tx_pkt));
        }

        /*
         * Result Check
         */
        sal_udelay(SAL_BOOT_SIMULATION ? 100000 : 100);
        rv = SOC_E_NONE;
        PBMP_ITER(fe_pbmp, p) {
            if ((rv = _soc_raptor_port_tx_ok(unit, p)) < 0) {
                break;
            }
        }

        /*
         *      Need to re-init and retest ?
         */
        if (rv < 0) {
            SOC_IF_ERROR_RETURN(soc_reset_init(unit));
            SOC_IF_ERROR_RETURN(soc_misc_init(unit));
            SOC_IF_ERROR_RETURN(soc_firebolt_internal_mmu_init(unit));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Post completed in %d Iterations\n"), count + 1));
            break; /* All clear */
        }
    }


    /*
     * Cleanup
     */
    PBMP_ITER(fe_pbmp, p) {
	/* macd is for the last FE port. But it should be fine */
        if (NULL != macd) {
        SOC_IF_ERROR_RETURN(MAC_ENABLE_SET(macd, unit, p, FALSE));
        }
        SOC_IF_ERROR_RETURN(soc_phyctrl_enable_set(unit, p, FALSE));
        SOC_IF_ERROR_RETURN(soc_phyctrl_loopback_set(unit, p, FALSE, TRUE));
    }
    soc_intr_enable(unit, int_mask);

    return(SOC_E_NONE);
}

static int _soc_raptor_post_init(int unit)
{
    uint32 *tx_pkt;
    int rv = SOC_E_NONE;

    /*
     *  Applicable only for FE ports.
     */
    if ((!soc_feature(unit, soc_feature_post)) ||
        (SOC_PBMP_IS_NULL(PBMP_FE_ALL(unit)))) {
        return SOC_E_NONE;
    }

    tx_pkt = soc_cm_salloc(unit, SOC_RAPTOR_POST_BUF_SIZE, "tx_pkt");
    if (tx_pkt == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(tx_pkt, 0xFF, SOC_RAPTOR_POST_BUF_SIZE);
    soc_cm_sflush(unit, tx_pkt, SOC_RAPTOR_POST_BUF_SIZE);

    rv = _soc_raptor_post_run(unit, tx_pkt);

    soc_cm_sfree(unit, tx_pkt);

    return rv;
}

#endif  /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) ||\
    defined(BCM_KATANA_SUPPORT)

STATIC int _soc_tr3_cpu_queue_xoff_disable(int unit) 
{
    int rv = SOC_E_NONE;
    dv_t *dv = NULL;
    char *pkt_data = NULL;
    pbmp_t empty_pbm;
    int channel = 1;
    uint32 rval;

    if (SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }

    soc_dma_init(unit);

    sal_memset( &empty_pbm, 0, sizeof(empty_pbm));

    if ((pkt_data = soc_cm_salloc( unit, 1512, "flush_pkt" )) == NULL ) {
        return SOC_E_MEMORY;
    }

    if ((dv = soc_dma_dv_alloc(unit, DV_RX, 1)) == NULL) {
        rv = SOC_E_MEMORY;
        goto cleanup;
    }

    sal_memset(dv->dv_dcb, 0, SOC_DCB_SIZE(unit));

    if ( soc_dma_desc_add(dv, (sal_vaddr_t) pkt_data, 1512, empty_pbm, 
                                    empty_pbm, empty_pbm, 0, NULL) < 0 ) {
        goto cleanup;
    }

    soc_dma_desc_end_packet(dv);
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        int cmc = SOC_PCI_CMC(unit);
        uint32 reg_addr;
        uint32 numq, reg_val;

        numq = soc_property_uc_get(unit, 0, spn_NUM_QUEUES, NUM_CPU_COSQ(unit));
        reg_val = 0xffffffff;

        if (numq < 32) {
            reg_val = (1 << numq) - 1;
        }
        reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(cmc, channel);
        soc_pci_write(unit, reg_addr, reg_val);
        reg_val = 0;
        if (numq > 32 && numq < 64) {
            reg_val = (1 << (numq - 32)) - 1;
        }
        reg_addr = CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(cmc, channel);
        soc_pci_write(unit, reg_addr, reg_val);

        /** chain end drop */
        reg_val = soc_pci_read(unit,CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, channel));
        reg_val |= 0x40;
        soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc, channel), reg_val);

        (void)soc_cmicm_intr0_disable(unit,
                      IRQ_CMCx_DESC_DONE(channel) | IRQ_CMCx_CHAIN_DONE(channel));
    }
#endif

    if (soc_dma_start(unit, channel, dv) < 0) {
        rv = SOC_E_INTERNAL;
        goto cleanup;
    }

    sal_usleep(10);

    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANAX(unit) || SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(READ_LLS_FC_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, 
                                FC_CFG_DISABLE_L2_COSMASK_XOFFf, 1);
        SOC_IF_ERROR_RETURN(WRITE_LLS_FC_CONFIGr(unit, rval));
    }

    if (SOC_IS_TD_TT(unit) && !SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(READ_ES_PIPE0_LLS_FC_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ES_PIPE0_LLS_FC_CONFIGr, &rval, 
                                FC_CFG_DISABLE_L2_COSMASK_XOFFf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ES_PIPE0_LLS_FC_CONFIGr(unit, rval));
    }

cleanup:
    if (dv) {
        soc_dma_abort_dv(unit, dv);
        soc_dma_dv_free(unit, dv);
    }
    if (pkt_data) {
        soc_cm_sfree(unit, pkt_data);
    }

    soc_dma_init(unit);

    return rv;

}

STATIC int _soc_tr3_cpu_queue_post( int unit ) 
{
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANAX(unit)) {
        pbmp_t empty_pbm;
        epc_link_bmap_entry_t entry, oentry;

        sal_memset( &empty_pbm, 0, sizeof(empty_pbm));
        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, &oentry));
        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, 
                                                &entry));
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, &entry, PORT_BITMAPf,
                               &empty_pbm);

        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                                                 &entry));   

        SOC_IF_ERROR_RETURN(_soc_tr3_cpu_queue_xoff_disable(unit));

        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                                                 &oentry));   
    }
    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT BCM_TRIUMPH3_SUPPORT BCM_KATANA_SUPPORT */

int
soc_post_init(int unit)
{
    uint32 runpost = 0;
    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }
    runpost = soc_property_get(unit, spn_POST_INIT_ENABLE, 1);
    if (runpost) {
#ifdef  BCM_RAPTOR_SUPPORT
        return _soc_raptor_post_init(unit);
#else
        return SOC_E_NONE;
#endif
    } else {
        return SOC_E_NONE;
    }
}

int
soc_mmu_post_init(int unit)
{
    uint32 runpost = 0;

    runpost = soc_property_get(unit, spn_POST_INIT_ENABLE, 0);
    if ((runpost) && ((SOC_IS_TD_TT(unit) || 
                       SOC_IS_TRIUMPH3(unit) || 
                       SOC_IS_KATANAX(unit)))) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) ||\
    defined(BCM_KATANA_SUPPORT)
            return _soc_tr3_cpu_queue_post(unit);
#else
            return SOC_E_NONE;
#endif
    } else {
        return SOC_E_NONE;
    }
}
