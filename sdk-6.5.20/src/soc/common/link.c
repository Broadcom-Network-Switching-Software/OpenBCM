/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hardware Linkscan module
 *
 * Hardware linkscan is available, but its use is not recommended
 * because a software linkscan task is very low overhead and much more
 * flexible.
 *
 * If hardware linkscan is used, each MII operation must temporarily
 * disable it and wait for the current scan to complete, increasing the
 * latency.  PHY status register 1 may contain clear-on-read bits that
 * will be cleared by hardware linkscan and not seen later.  Special
 * support is provided for the Serdes MAC.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif
#include <soc/defs.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#endif /* PORTMOD_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
#include <soc/saber2.h>
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#ifdef BCM_HURRICANE3_SUPPORT
#include <soc/wolfhound2.h>
#endif /* BCM_HURRICANE3_SUPPORT */

#if 64 > SOC_MAX_NUM_PORTS
#define SOC_CMIC_PHY_PORT_MAX      (64)
#else
#define SOC_CMIC_PHY_PORT_MAX      (SOC_MAX_NUM_PORTS)
#endif

#if defined(BCM_CMICM_SUPPORT)

#ifndef SOC_CMICM_PHY_PORT_MAX
#define SOC_CMICM_PHY_PORT_MAX     (128)
#endif
#if     SOC_CMICM_PHY_PORT_MAX  >  SOC_CMIC_PHY_PORT_MAX
#undef  SOC_CMIC_PHY_PORT_MAX
#define SOC_CMIC_PHY_PORT_MAX      SOC_CMICM_PHY_PORT_MAX
#endif

#if defined(BCM_CMICDV2_SUPPORT)
#ifndef SOC_CMICDV2_PHY_PORT_MAX
#define SOC_CMICDV2_PHY_PORT_MAX   (SOC_CMICM_PHY_PORT_MAX + 64)
#endif
#if     SOC_CMICDV2_PHY_PORT_MAX > SOC_CMIC_PHY_PORT_MAX
#undef  SOC_CMIC_PHY_PORT_MAX
#define SOC_CMIC_PHY_PORT_MAX      SOC_CMICDV2_PHY_PORT_MAX
#endif

#if defined(BCM_CMICDV4_SUPPORT)
#ifndef SOC_CMICDV4_PHY_PORT_MAX
#define SOC_CMICDV4_PHY_PORT_MAX   (SOC_CMICDV2_PHY_PORT_MAX + 128)
#endif
#if     SOC_CMICDV4_PHY_PORT_MAX > SOC_CMIC_PHY_PORT_MAX
#undef  SOC_CMIC_PHY_PORT_MAX
#define SOC_CMIC_PHY_PORT_MAX      SOC_CMICDV4_PHY_PORT_MAX
#endif
#endif /* BCM_CMICDV4_SUPPORT */
#endif /* BCM_CMICDV2_SUPPORT */

#endif /* BCM_CMICM_SUPPORT */

#define SOC_BITWORD_SET(_a, _word, _val)   _a[_word] = _val
#define SOC_BITWORD_GET(_a, _word)         _a[_word]

#ifdef BCM_APACHE_SUPPORT
/* In Apache, phy ports 1-36 are mapped to
 * CMIC_MIIM_SCAN_PORTS_0 and CMIC_MIIM_SCAN_PORTS_1
 * CMIC_MIIM_INT_SEL_MAP_0 and CMIC_MIIM_INT_SEL_MAP_1
 * CMIC_MIIM_LINK_STATUS_0 and CMIC_MIIM_LINK_STATUS_1
 * phy ports 37-72 are mapped to
 * CMIC_MIIM_SCAN_PORTS_2 and CMIC_MIIM_SCAN_PORTS_3
 * CMIC_MIIM_INT_SEL_MAP_2 and CMIC_MIIM_INT_SEL_MAP_3
 * CMIC_MIIM_LINK_STATUS_2 and CMIC_MIIM_LINK_STATUS_3
 */
#define MAX_PORTS_PER_SCAN_CHAIN 64
#define APACHE_PORTS_PER_SCAN_CHAIN 36
#define APACHE_UNUSED_PORTS_PER_SCAN_CHAIN (MAX_PORTS_PER_SCAN_CHAIN - APACHE_PORTS_PER_SCAN_CHAIN)
#define PHY_PORT_TO_LINKSCAN_PORT_XLATE(unit, phy_port)                           \
    do {                                                                          \
        if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {                                                \
            phy_port = (phy_port > APACHE_PORTS_PER_SCAN_CHAIN) ?                 \
                       (phy_port + APACHE_UNUSED_PORTS_PER_SCAN_CHAIN) : phy_port;\
        }                                                                         \
    } while (0);
#define LINKSCAN_PORT_TO_PHY_PORT_XLATE(unit, phy_port)                           \
    do {                                                                          \
        if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {                                                \
            phy_port = ((phy_port  > APACHE_PORTS_PER_SCAN_CHAIN) &&              \
                        (phy_port <= MAX_PORTS_PER_SCAN_CHAIN))   ? -1 :          \
                       (phy_port > MAX_PORTS_PER_SCAN_CHAIN)      ?               \
                       (phy_port - APACHE_UNUSED_PORTS_PER_SCAN_CHAIN) : phy_port;\
        }                                                                         \
    } while (0);
#else
#define PHY_PORT_TO_LINKSCAN_PORT_XLATE(unit, phy_port)
#define LINKSCAN_PORT_TO_PHY_PORT_XLATE(unit, phy_port)
#endif
#define GREYHOUND2_PHY_PORT_OFFSET 7
/*
 * Function:    
 *      _soc_link_update
 * Purpose:
 *      Update the forwarding state in the chip (EPC_LINK).
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 * NOTE:
 * soc_link_fwd_set and soc_link_mask2_set call
 * this function to update EPC_LINK_BMAP. soc_link_fwd_set is called 
 * with LINK_LOCK and soc_link_mask2_set is called with PORT_LOCK.
 * No synchronization mechanism is implemented in this function. Therefore,
 * the user must make sure that the call to this function is synchronized 
 * between linkscan thread and calling thread. 
 */

STATIC int
_soc_link_update(int unit)
{
    pbmp_t      	pbm;
#ifdef BCM_TOMAHAWK3_SUPPORT
    pbmp_t      	temp_pbm;
#endif
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_persist_t	*sop = SOC_PERSIST(unit);
    char        pfmtl[SOC_PBMP_FMT_LEN],
        pfmtm2[SOC_PBMP_FMT_LEN],
        pfmtp[SOC_PBMP_FMT_LEN];

    COMPILER_REFERENCE(pfmtl);
    COMPILER_REFERENCE(pfmtm2);
    COMPILER_REFERENCE(pfmtp);

    SOC_PBMP_ASSIGN(pbm, sop->link_fwd);
    SOC_PBMP_AND(pbm, soc->link_mask2);

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        /* In Saber2, if OAMP is enabled, keep the OAMP port(5) up */
        uint32  rval;
        SOC_IF_ERROR_RETURN(READ_OAMP_ENABLEr(unit, &rval));
        if(soc_reg_field_get(unit, OAMP_ENABLEr, rval, ENABLEf)) {
           SOC_PBMP_PORT_ADD(pbm, SOC_SB2_SAT_OAMP_PHY_PORT_NUMBER);
        }
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_LINK,
                (BSL_META_U(unit,
                            "_soc_link_update: link=%s m2=%s pbm=%s\n"),
                 SOC_PBMP_FMT(sop->link_fwd, pfmtl),
                 SOC_PBMP_FMT(soc->link_mask2, pfmtm2),
                 SOC_PBMP_FMT(pbm, pfmtp)));

#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES(unit)) {
        int port;
        uint32  nlink, olink;

        nlink = SOC_PBMP_WORD_GET(pbm, 0);
        olink = -1;
        PBMP_PORT_ITER(unit, port) {
            SOC_IF_ERROR_RETURN
                (READ_ING_EPC_LNKBMAPr(unit, port, &olink));
            break;
        }

        if (nlink != olink) {
            PBMP_PORT_ITER(unit, port) {
                SOC_IF_ERROR_RETURN
                    (WRITE_ING_EPC_LNKBMAPr(unit, port, nlink));
            }
        }
        return SOC_E_NONE;
    }
#endif /* BCM_HERCULES_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit) ||
        SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANAX(unit) ||
        SOC_IS_SABER2(unit)) {
        epc_link_bmap_entry_t entry;

        /* Don't allow updates to EPC_LINK_BMAP or ING_DEST_PORT_ENABLE
         * if in fast reboot */
        if (SOC_FAST_REBOOT(unit)) {
            return SOC_E_NONE;
        }

        SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                            &entry));

#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            /* For TH3-5341 WAR, we need to allow the spare port to be
               set in the epc link bmap. This will allow to send packet to
               the spare port and not override the bitmap with valid ports.
             */
            soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, &entry, PORT_BITMAPf,
                                   &temp_pbm);
            if (SOC_PBMP_MEMBER(temp_pbm, _TH3_MMU_SPARE_DST_PORT)) {
                SOC_PBMP_CLEAR(temp_pbm);
                SOC_PBMP_PORT_SET(temp_pbm, _TH3_MMU_SPARE_DST_PORT);
                SOC_PBMP_OR(pbm, temp_pbm);
            }
            if (SOC_PBMP_MEMBER(temp_pbm, _TH3_MMU_SPARE_SRC_PORT)) {
                SOC_PBMP_CLEAR(temp_pbm);
                SOC_PBMP_PORT_SET(temp_pbm, _TH3_MMU_SPARE_SRC_PORT);
                SOC_PBMP_OR(pbm, temp_pbm);
            }
        }
#endif
        soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, &entry, PORT_BITMAPf,
                               &pbm);
        if (SOC_MEM_FIELD_VALID(unit, EPC_LINK_BMAPm, ENABLE_SOBMH_BLOCKINGf)) {
            soc_mem_field32_set(unit, EPC_LINK_BMAPm, &entry,
                                ENABLE_SOBMH_BLOCKINGf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,
                            &entry));

        if (soc_feature(unit, soc_feature_ingress_dest_port_enable)) {
            ing_dest_port_enable_entry_t dest_en;
            SOC_IF_ERROR_RETURN
                (READ_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ANY, 0, &dest_en));

            soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm,
                                   &dest_en, PORT_BITMAPf, &pbm);

            SOC_IF_ERROR_RETURN
                (WRITE_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ANY, 0, &dest_en));
        }

        return SOC_E_NONE;
    } else if (SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANE(unit)
                && !SOC_IS_GREYHOUND2(unit)) {
        uint64      nlink64, olink64;

        COMPILER_64_SET(nlink64, SOC_PBMP_WORD_GET(pbm, 1),
                        SOC_PBMP_WORD_GET(pbm, 0));

        SOC_IF_ERROR_RETURN
            (soc_reg64_read_any_block(unit, EPC_LINK_BMAP_64r, &olink64));
        if (COMPILER_64_NE(nlink64, olink64)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, EPC_LINK_BMAP_64r, nlink64));
        }
        return SOC_E_NONE;
    } else if (SOC_IS_GREYHOUND2(unit)) {
        uint64      nlink64, olink64;

        COMPILER_64_SET(nlink64, SOC_PBMP_WORD_GET(pbm, 1),
                        SOC_PBMP_WORD_GET(pbm, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg64_read_any_block(unit, EPC_LINK_BMAP_LO_64r, &olink64));
        if (COMPILER_64_NE(nlink64, olink64)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, EPC_LINK_BMAP_LO_64r, nlink64));
        }
        COMPILER_64_SET(nlink64, SOC_PBMP_WORD_GET(pbm, 3),
                        SOC_PBMP_WORD_GET(pbm, 2));
        SOC_IF_ERROR_RETURN
            (soc_reg64_read_any_block(unit, EPC_LINK_BMAP_HI_64r, &olink64));
        if (COMPILER_64_NE(nlink64, olink64)) {
            SOC_IF_ERROR_RETURN
                (soc_reg64_write_all_blocks(unit, EPC_LINK_BMAP_HI_64r, nlink64));
        }
        return SOC_E_NONE;
    } else if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)){
        uint32          olink, nlink;
        nlink = SOC_PBMP_WORD_GET(pbm, 0);

        SOC_IF_ERROR_RETURN
            (soc_reg_read_any_block(unit, EPC_LINK_BMAP_64r, &olink));
        if (nlink != olink) {
            SOC_IF_ERROR_RETURN
                (soc_reg_write_all_blocks(unit, EPC_LINK_BMAP_64r, nlink));
        }
        return SOC_E_NONE;    
    }
#endif

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
        uint32          olink, nlink;

#if defined(BCM_RAPTOR_SUPPORT)
        if (soc_feature(unit, soc_feature_register_hi)) {
            SOC_IF_ERROR_RETURN
                (WRITE_EPC_LINK_BMAP_HIr(unit, SOC_PBMP_WORD_GET(pbm, 1)));
        }
#endif /* BCM_RAPTOR_SUPPORT */

        SOC_IF_ERROR_RETURN
            (soc_reg_read_any_block(unit, EPC_LINK_BMAPr, &olink));
        nlink = SOC_PBMP_WORD_GET(pbm, 0);
        if (nlink != olink) {
            if (SOC_IS_SHADOW(unit)) {
                /* Deal with aggregated Interlaken ports */
                if (SOC_PBMP_MEMBER(pbm, 9) &&
                   !(SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), 9))) {
                     SOC_PBMP_PORT_ADD(pbm, 10);
                     SOC_PBMP_PORT_ADD(pbm, 11);
                     SOC_PBMP_PORT_ADD(pbm, 12);
                }
                if (SOC_PBMP_MEMBER(pbm, 13) &&
                   !(SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), 13))) {
                     SOC_PBMP_PORT_ADD(pbm, 14);
                     SOC_PBMP_PORT_ADD(pbm, 15);
                     SOC_PBMP_PORT_ADD(pbm, 16);
                }
                nlink = SOC_PBMP_WORD_GET(pbm, 0);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_write_all_blocks(unit, EPC_LINK_BMAPr, nlink));
        }
        return SOC_E_NONE; 
    }
#endif /* BCM_ESW_SUPPORT */    

    return SOC_E_NONE;  /* SOC_E_UNAVAIL? */
}

/*
 * Function:
 *      soc_link_fwd_set
 * Purpose:
 *      Sets EPC_LINK independent of chip type.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbmp - Value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      EPC_LINK should be manipulated only through this routine and
 *      soc_link_maskX_set.
 */

int
soc_link_fwd_set(int unit, pbmp_t fwd)
{
    SOC_PERSIST(unit)->link_fwd = fwd;

    return _soc_link_update(unit);
}

/*
 * Function:
 *      soc_link_fwd_get
 * Purpose:
 *      Gets EPC_LINK independent of chip type.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbmp - (OUT) Value.
 */

void
soc_link_fwd_get(int unit, pbmp_t *fwd)
{
    *fwd = SOC_PERSIST(unit)->link_fwd;
}

/*
 * Function:
 *      soc_link_mask2_set
 * Purpose:
 *      Mask bits in EPC_LINK independent of soc_link_fwd value.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mask - Value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      This routine is used to clear bits in the EPC_LINK to support
 *      the mac_fe/ge_enable_set() calls.
 */

int
soc_link_mask2_set(int unit, pbmp_t mask)
{
#if defined(BCM_KATANA2_SUPPORT)
    int pp_port_id = 0;
#endif

    SOC_CONTROL(unit)->link_mask2 = mask;

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) &&
        SOC_INFO(unit).linkphy_enabled) {
        SOC_PBMP_REMOVE(SOC_CONTROL(unit)->link_mask2,
                    SOC_INFO(unit).linkphy_pbm);
        /* If the pp_port is set in the enabled linkphy bmap, set the same in the
         * link_mask2 bmap, do this only for pp_ports.
         */
        SOC_PBMP_ITER(SOC_INFO(unit).enabled_linkphy_pp_port_pbm, pp_port_id) {
            if (SOC_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm, pp_port_id)) {
                SOC_PBMP_PORT_ADD(SOC_CONTROL(unit)->link_mask2, pp_port_id);
            }
        } else {
            if (SOC_PBMP_MEMBER(SOC_INFO(unit).linkphy_pp_port_pbm, pp_port_id)) {
                SOC_PBMP_PORT_REMOVE(SOC_CONTROL(unit)->link_mask2, pp_port_id);
            }
        }
    }
    if (soc_feature(unit, soc_feature_subtag_coe) &&
        SOC_INFO(unit).subtag_enabled) {
        SOC_PBMP_REMOVE(SOC_CONTROL(unit)->link_mask2,
                        SOC_INFO(unit).subtag_pbm);
        /* If the pp_port is set in the enabled subtag bmap, set the same in the
         * link_mask2 bmap, do this only for pp_ports
         */
        SOC_PBMP_ITER(SOC_INFO(unit).enabled_subtag_pp_port_pbm, pp_port_id) {
            if (SOC_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm, pp_port_id)) {
                SOC_PBMP_PORT_ADD(SOC_CONTROL(unit)->link_mask2, pp_port_id);
            }
        } else {
            if (SOC_PBMP_MEMBER(SOC_INFO(unit).subtag_pp_port_pbm, pp_port_id)) {
                SOC_PBMP_PORT_REMOVE(SOC_CONTROL(unit)->link_mask2, pp_port_id);
            }
        }
    }
#endif

    return _soc_link_update(unit);
}

/*
 * Function:
 *      soc_link_mask2_get
 * Purpose:
 *      Counterpart to soc_link_mask2_set
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      mask - (OUT) Value.
 */

void
soc_link_mask2_get(int unit, pbmp_t *mask)
{
    *mask = SOC_CONTROL(unit)->link_mask2;
}

#if defined(BCM_ESW_SUPPORT)
/*
 * Function:
 *      _soc_link_scan_ports_write
 * Purpose:
 *      Writes the CMIC_SCAN_PORTS register(s) of the device with the
 *      provided HW linkscan port configuration.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      hw_mii_pbm - Scan ports.
 * Returns:
 *      Nothing
 * Notes:
 *      Assumes interrupt suspension handled in the calling function
 */

STATIC void
_soc_link_scan_ports_write(int unit, pbmp_t hw_mii_pbm)
{
    uint32      link_pbmp;
    SHR_BITDCL  pb[_SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX)] = {0};
    soc_port_t  phy_port, port;

    link_pbmp = SOC_PBMP_WORD_GET(hw_mii_pbm, 0); 
#if defined(BCM_GOLDWING_SUPPORT) 
    if (SOC_IS_GOLDWING(unit)) {
        /* (MSB) 15-14-19-18-17-16-13-12-11-10-9-8-7-6-5-4-3-2-1-0 (LSB) */
        link_pbmp =  (link_pbmp & 0x00003FFF) |
            ((link_pbmp & 0x000F0000) >> 2) |
            ((link_pbmp & 0x0000C000) << 4);
    }
#endif /* BCM_GOLDWING_SUPPORT */
#if defined (BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        /* CMIC port not included in link status */
        link_pbmp >>=  1;
    }
#endif /* BCM_SCORPION_SUPPORT */
    sal_memset(pb, 0, sizeof(SHR_BITDCL) * _SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX));
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        PBMP_ITER(hw_mii_pbm, port) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
#ifdef BCM_GREYHOUND2_SUPPORT
            if (SOC_IS_GREYHOUND2(unit)) {
                phy_port = (phy_port >= 58) ? (phy_port + GREYHOUND2_PHY_PORT_OFFSET)
                                            : phy_port;
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
            if (phy_port == 0) {
                continue;
            }
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
                SHR_BITSET(pb, phy_port);
            } else {
                PHY_PORT_TO_LINKSCAN_PORT_XLATE(unit, phy_port);
                SHR_BITSET(pb, phy_port - 1);
            }
        }
        link_pbmp = SOC_BITWORD_GET(pb, 0); 
    } else {
        PBMP_ITER(hw_mii_pbm, port) {
            phy_port = port;
            SHR_BITSET(pb, phy_port);
        }
    }
#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        WRITE_CMIC_MIIM_SCAN_PORTS_0r(unit, link_pbmp);
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_1r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_1r(unit,
                                          SOC_BITWORD_GET(pb, 1));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_2r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_2r(unit,
                                          SOC_BITWORD_GET(pb, 2));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_3r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_3r(unit,
                                          SOC_BITWORD_GET(pb, 3));
        }
#if defined (BCM_CMICDV2_SUPPORT)
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_4r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_4r(unit,
                                          SOC_BITWORD_GET(pb, 4));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_5r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_5r(unit,
                                          SOC_BITWORD_GET(pb, 5));
        }
#if defined (BCM_CMICDV4_SUPPORT)
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_6r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_6r(unit,
                                          SOC_BITWORD_GET(pb, 6));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_7r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_7r(unit,
                                          SOC_BITWORD_GET(pb, 7));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_8r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_8r(unit,
                                          SOC_BITWORD_GET(pb, 8));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_MIIM_SCAN_PORTS_9r)) {
            WRITE_CMIC_MIIM_SCAN_PORTS_9r(unit,
                                          SOC_BITWORD_GET(pb, 9));
        }
#endif /* BCM_CMICDV4_SUPPORT */
#endif /* BCM_CMICDV2_SUPPORT */
    } else
#endif /* BCM_CMICM_SUPPORT */
#if defined (BCM_CMICX_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_HW_LINKSCAN
        soc_cmicx_linkscan_config(unit, &hw_mii_pbm);
#endif
        return;
    } else
#endif /* BCM_CMICX_SUPPORT */
    {
        soc_pci_write(unit, CMIC_SCAN_PORTS, link_pbmp);

        if (((SOC_IS_TR_VL(unit))
             && !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANE(unit)) ||
            soc_feature(unit, soc_feature_register_hi)) {
            WRITE_CMIC_SCAN_PORTS_HIr
                (unit, SOC_BITWORD_GET(pb, 1));
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if (SOC_REG_IS_VALID(unit, CMIC_SCAN_PORTS_HI_2r)) {
            WRITE_CMIC_SCAN_PORTS_HI_2r
                (unit, SOC_BITWORD_GET(pb, 2));
        }
#endif
    }
}
#endif /* BCM_ESW_SUPPORT */

/*
 * Function:
 *      soc_linkscan_pause
 * Purpose:
 *      Pauses link scanning, without disabling it.
 *      This call is used to pause scanning temporarily.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 * Notes:
 *      Nesting pauses is provided for.
 *      Software must ensure every pause is accompanied by a continue
 *      or linkscan will never resume.
 */
void
soc_linkscan_pause(int unit)
{
#if defined(BCM_ESW_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			s, stall_count = 0;
    uint32              schan_ctrl;

    if (NULL == soc) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_linkscan_pause: SOC_CONTROL not init!\n")));
        return;
    }

    if (SOC_IS_ESW(unit)) {
        SOC_LINKSCAN_LOCK(unit, s);    /* Manipulate flags & regs atomically */

        if (soc->soc_link_pause++ == 0 &&
            (soc->soc_flags & SOC_F_LSE)) {
            /* Stop link scan and wait for current pass to finish */
#ifdef BCM_CMICM_SUPPORT
            if(soc_feature(unit, soc_feature_cmicm)) {
                /* Turn off HW linkscan */
                READ_CMIC_MIIM_SCAN_CTRLr(unit,&schan_ctrl);
                soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &schan_ctrl,
                                  MIIM_LINK_SCAN_ENf, 0);
                WRITE_CMIC_MIIM_SCAN_CTRLr(unit,schan_ctrl);
                if (soc_feature(unit, soc_feature_linkscan_pause_timeout)) {
                     soc_timeout_t to;
                    /* Wait for Linkscan stopped signal */
                    soc_timeout_init(&to, 1000000 /*1 sec*/, 100);  
                    while (soc_pci_read(unit, CMIC_MIIM_SCAN_STATUS_OFFSET) &
                           CMIC_MIIM_SCAN_BUSY) {
                        if (soc_timeout_check(&to)) {
                           LOG_ERROR(BSL_LS_SOC_COMMON,
                                     (BSL_META_U(unit,
                                                 "soc_linkscan_pause: pausing hw linkscan failed\n")));
                           break;
                        }
                    }
                } else {
                    /* Wait for Linkscan stopped signal */
                    /* We're using this PCI operation to pass some time
                    * since we can't use sal_usleep safely with the
                    * interrupts suspended.  Wait > 1ms
                    */
                    /* Sometimes, value of field MIIM_SCAN_BUSYf of
                    * register CMIC_MIIM_SCAN_STATUSr always is 1, so there are unbounded loops
                    * without condition "stall_count++ < 4000".
                    */
                    while ((soc_pci_read(unit, CMIC_MIIM_SCAN_STATUS_OFFSET) & CMIC_MIIM_SCAN_BUSY) &&
                            stall_count++ < 4000) {
                        /* Nothing */
                    }
                }
            } else
#endif
#ifdef BCM_CMICX_SUPPORT
            if(soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_HW_LINKSCAN
               soc_cmicx_linkscan_pause(unit);
#endif
               SOC_LINKSCAN_UNLOCK(unit, s);
               return;
            } else
#endif
            {
                /* Turn off HW linkscan */
                soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_CLR);

                if (soc_feature(unit, soc_feature_linkscan_pause_timeout)) {
                    soc_timeout_t to;
                    /* Wait for Linkscan stopped signal */
                    soc_timeout_init(&to, 1000000 /*1 sec*/, 100);  
                    while (soc_pci_read(unit, CMIC_SCHAN_CTRL) &
                           SC_MIIM_SCAN_BUSY_TST) {
                        if (soc_timeout_check(&to)) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "soc_linkscan_pause: pausing hw linkscan failed\n")));
                            break;
                        }
                    }
                } else {
                    /* Wait for Linkscan stopped signal */
                    /* We're using this PCI operation to pass some time
                    * since we can't use sal_usleep safely with the
                    * interrupts suspended.  Wait > 1ms
                    */
                    /* Sometimes, value of field MIIM_SCAN_BUSYf of
                    * register CMIC_MIIM_SCAN_STATUSr always is 1, so there are unbounded loops
                    * without condition "stall_count++ < 4000".
                    */
                    while ((soc_pci_read(unit, CMIC_SCHAN_CTRL) & SC_MIIM_SCAN_BUSY_TST) &&
                        stall_count++ < 6000) {
                        /* Nothing */
                    }
                }
            }
            if (stall_count >= 6000) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_linkscan_pause: pausing hw linkscan failed\n")));
            }
            COMPILER_REFERENCE(schan_ctrl);
#ifdef BCM_RAVEN_SUPPORT
            if (SOC_IS_RAVEN(unit)) {
                /* Wait last HW linkscan operation to complete.
                         * Assume each CMIC read below takes 2us */
                stall_count = (soc->hw_linkscan_delay_ns) / 2000;
            } else
#endif
            {
                /* Wait > 1us for last HW linkscan operation to complete. */
                stall_count = 4;
            }
            for (; stall_count > 0; stall_count--) {
                /* We're using this PCI operation to pass some time
                 * since we can't use sal_usleep safely with the
                 * interrupts suspended.  We only record the read value
                 * to prevent any complaint about an uninspected return
                 * value.
                 */
#ifdef BCM_CMICM_SUPPORT
                if(soc_feature(unit, soc_feature_cmicm)) {
                    schan_ctrl = soc_pci_read(unit, CMIC_MIIM_SCAN_STATUS_OFFSET);
                } else
#endif
                {
                    schan_ctrl = soc_pci_read(unit, CMIC_SCHAN_CTRL);
                }
            }
        }

        SOC_LINKSCAN_UNLOCK(unit, s);
    }
#endif /* BCM_ESW_SUPPORT */
}

/*
 * Function:
 *      soc_linkscan_continue
 * Purpose:
 *      Continue link scanning after it has been paused.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 * Notes:
 *      This routine is designed so if soc_linkscan_config is called,
 *      it won't be confused whether or not a pause is in effect.
 */

void
soc_linkscan_continue(int unit)
{
#if defined(BCM_ESW_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			s;
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
    uint32 schan_ctrl;
#endif

    if (NULL == soc) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_linkscan_continue: SOC_CONTROL not init!\n")));
        return;
    }

    if (SOC_IS_ESW(unit)) {
        SOC_LINKSCAN_LOCK(unit, s);    /* Manipulate flags & regs atomically */

        if (soc->soc_link_pause <= 0) {
            SOC_LINKSCAN_UNLOCK(unit, s);
            assert(0);      /* Continue not preceded by a pause */
        }

        if (--soc->soc_link_pause == 0 &&
            (soc->soc_flags & SOC_F_LSE)) {

            /*
             * NOTE: whenever hardware linkscan is running, the PHY_REG_ADDR
             * field of the MIIM_PARAM register must be set to 1 (PHY Link
             * Status register address).
             */
#ifdef BCM_CMICM_SUPPORT
            if(soc_feature(unit, soc_feature_cmicm)) {
                if (soc_feature(unit, soc_feature_phy_cl45))  {
                    /*
                    ** Clause 22 Register 0x01 (MII_STAT) for FE/GE.
                    ** Clause 45 Register 0x01 (MII_STAT) Devad = 0x1 (PMA_PMD) 
                    ** for XE.
                    */
                    uint32 phy_miim_addr = 0;
                    soc_reg_field_set(unit, CMIC_CMC0_MIIM_ADDRESSr, &phy_miim_addr,
                                  CLAUSE_45_DTYPEf, 0x01);
                    soc_reg_field_set(unit, CMIC_CMC0_MIIM_ADDRESSr, &phy_miim_addr,
                                  CLAUSE_45_REGADRf, 0x01);
                    /* To set 0x1 to CLAUSE_22_REGADRf is not required.
                     *  - in all CMICM device, the CLAUSE_22_REGADRf in register 
                     * CMIC_CMCx_MIIM_ADDRESSr is the field overlay from bit0 to
                     *    bit4 of CLAUSE_45_REGADRf.
                     */
                    if (SOC_REG_FIELD_VALID(unit, CMIC_CMC0_MIIM_ADDRESSr, 
                            CLAUSE_22_REGADRf)) {
                        soc_reg_field_set(unit, CMIC_CMC0_MIIM_ADDRESSr, 
                                &phy_miim_addr, CLAUSE_22_REGADRf, 0x01);
                    }
                    soc_pci_write(unit, CMIC_CMCx_MIIM_ADDRESS_OFFSET(cmc), phy_miim_addr);
                } else {
                    soc_pci_write(unit, CMIC_CMCx_MIIM_PARAM_OFFSET(cmc), (uint32) 0x01 << 24);
                }
                _soc_link_scan_ports_write(unit, soc->hw_linkscan_pbmp);
                READ_CMIC_MIIM_SCAN_CTRLr(unit,&schan_ctrl);
                soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &schan_ctrl,
                                                         MIIM_LINK_SCAN_ENf, 1);
                WRITE_CMIC_MIIM_SCAN_CTRLr(unit,schan_ctrl);
            } else
#endif
#if defined (BCM_CMICX_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_HW_LINKSCAN
                soc_cmicx_linkscan_continue(unit);
#endif
                SOC_LINKSCAN_UNLOCK(unit, s);
                return;
            } else
#endif
            {
                if (soc_feature(unit, soc_feature_phy_cl45))  {
                    /*
                    ** Clause 22 Register 0x01 (MII_STAT) for FE/GE.
                    ** Clause 45 Register 0x01 (MII_STAT) Devad = 0x1 (PMA_PMD) 
                    ** for XE.
                    */
                    uint32 phy_miim_addr = 0;
                    soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                                  CLAUSE_45_DTYPEf, 0x01);
                    soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                                  CLAUSE_45_REGADRf, 0x01);
                    if (SOC_REG_FIELD_VALID(unit, CMIC_CMC0_MIIM_ADDRESSr,
                            CLAUSE_22_REGADRf)) {
                        soc_reg_field_set(unit, CMIC_MIIM_ADDRESSr, &phy_miim_addr,
                                          CLAUSE_22_REGADRf, 0x01);
                    }
                    WRITE_CMIC_MIIM_ADDRESSr(unit, phy_miim_addr);
                } else {
                    soc_pci_write(unit, CMIC_MIIM_PARAM, (uint32) 0x01 << 24);
                }
                _soc_link_scan_ports_write(unit, soc->hw_linkscan_pbmp);
                soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_SET);
            }
        }

        SOC_LINKSCAN_UNLOCK(unit, s);
    }
#endif /* BCM_ESW_SUPPORT */
}

/*
 * Function:
 *      soc_linkscan_register
 * Purpose:
 *      Provide a callout made when CMIC link scanning detects a link change.
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      f    - Function called when link status change is detected.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Handler called in interrupt context.
 */

int
soc_linkscan_register(int unit, void (*f)(int))
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (f != NULL && soc->soc_link_callout != NULL) {
        return SOC_E_EXISTS;
    }

    soc->soc_link_callout = f;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_linkscan_config
 * Purpose:
 *      Set ports to scan in CMIC.
 * Parameters:
 *      unit - StrataSwich Unit #
 *      mii_pbm - Port bit map of ports to scan with MIIM registers
 *      direct_pbm - Port bit map of ports to scan using NON MII.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_linkscan_config(int unit, pbmp_t hw_mii_pbm, pbmp_t hw_direct_pbm)
{
#if defined(BCM_ESW_SUPPORT)
    soc_control_t	*soc = SOC_CONTROL(unit);
#ifdef BCM_HERCULES_SUPPORT
    uint32      cmic_config;
#endif
    int			s, has_mge, has_dge;
    pbmp_t		pbm;

    if (SOC_IS_ESW(unit)) {

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    assert(SOC_PBMP_IS_NULL(pbm));      /* !(hw_mii_pbm & hw_direct_pbm) */

    /*
     * Hardware (direct) scanning is NOT supported on 10/100 ports.
     */
    SOC_PBMP_ASSIGN(pbm, hw_direct_pbm);
    SOC_PBMP_AND(pbm, PBMP_FE_ALL(unit));
    if (SOC_PBMP_NOT_NULL(pbm)) {
        return SOC_E_UNAVAIL;
    }

    /*
     * The LINK_SCAN_GIG control affects ALL ports. Thus, all ports
     * being scanned by H/W must be either MIIM scanned or scanned
     * using the direct connection.
     */
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_mii_pbm);
    has_mge = SOC_PBMP_NOT_NULL(pbm);
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    has_dge = SOC_PBMP_NOT_NULL(pbm);
    if (has_mge && has_dge) {
        return SOC_E_UNAVAIL;
    }

    /*
     * soc_linkscan_pause/continue combination will result in the
     * registers being setup and started properly if we are enabling for
     * the first time.
     */

    SOC_LINKSCAN_LOCK(unit, s);

    soc_linkscan_pause(unit);

    /* Check if disabling port scanning */

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_OR(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {
        /*
         * NOTE: we are no longer using CC_LINK_STAT_EN since it is
         * unavailable on 5695 and 5665.  EPC_LINK will be updated by
         * software anyway, it will just take a few extra milliseconds.
         */
        soc->soc_flags |= SOC_F_LSE;
    } else {
        soc->soc_flags &= ~SOC_F_LSE;
    }


#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_HW_LINKSCAN
        soc_cmicx_linkscan_config(unit, &hw_mii_pbm);
#endif
        soc_linkscan_continue(unit);
        SOC_LINKSCAN_UNLOCK(unit, s);
        return SOC_E_NONE;
    }
#endif /* BCM_CMICX_SUPPORT */

#ifdef BCM_HERCULES_SUPPORT
    if (soc_reg_field_valid(unit, CMIC_CONFIGr, LINK_STAT_ENf)) {
       
        cmic_config = soc_pci_read(unit, CMIC_CONFIG);
        soc_reg_field_set(unit, CMIC_CONFIGr, &cmic_config, LINK_STAT_ENf,
                          SOC_PBMP_NOT_NULL(pbm) ? 1 : 0);
        soc_pci_write(unit, CMIC_CONFIG, cmic_config);
    }
#endif
    /* The write of the HW linkscan ports is moved to the linkscan
     * continue below.  Note that though the continue function
     * will not write to the CMIC scan ports register if linkscan
     * was disabled above, that is only the case when the port bitmap
     * is empty.  Since linkscan pause clears the bitmap, this is the
     * desired result.
     */
    SOC_PBMP_ASSIGN(soc->hw_linkscan_pbmp, hw_mii_pbm);

    soc_linkscan_continue(unit);

    SOC_LINKSCAN_UNLOCK(unit, s);
    }
#endif /* BCM_ESW_SUPPORT */
    return SOC_E_NONE;
}

#if defined(BCM_XGS3_SWITCH_SUPPORT)

STATIC int
_soc_linkscan_port_to_phyaddr(int unit, int port)
{
#ifdef PORTMOD_SUPPORT
    int nof_cores = 0;
    phymod_core_access_t core_acc;
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        portmod_port_main_core_access_get(unit, port, 1, &core_acc, &nof_cores);
        if (nof_cores == 0) {
            if ((SOC_PORTCTRL_FUNCTIONS(unit)) &&
                (SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_port_phyaddr_get)) {
                return SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_port_phyaddr_get
                        (unit, port);
            } else {
                return -1;
            }
        } else {
            return portmod_port_to_phyaddr(unit, port);
        }
    }
#endif

    return PHY_ADDR(unit, port);
}

extern int
_soc_linkscan_phy_flags_test(int unit, int port, int flags)
{
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        /* For disabled ports - return -1, fix it after flexport arch is in place */
        if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all),port)) return (0);
        return portmod_port_flags_test(unit, port, flags) == 1 ? 1 : 0;
    }
#endif

    return PHY_FLAGS_TST(unit, port, flags);
}


STATIC int
_soc_linkscan_fault_status_set(int unit, soc_port_t port) 
{
    soc_port_t phy_port;
    int blk;
    int blktype;
    uint32 rval;
    int i;

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        if ((blktype == SOC_BLK_XLPORT) && 
            SOC_REG_IS_VALID(unit, XLPORT_FAULT_LINK_STATUSr)) {

            rval = 0;

            soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &rval, 
                    REMOTE_FAULTf, 1);
            soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &rval, 
                    LOCAL_FAULTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, 
                        port, rval));
        }

        if ((blktype == SOC_BLK_CLPORT) &&
            SOC_REG_IS_VALID(unit, CLPORT_FAULT_LINK_STATUSr)) {

            rval = 0;

            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                    REMOTE_FAULTf, 1);
            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                    LOCAL_FAULTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_FAULT_LINK_STATUSr(unit,
                        port, rval));
        }

        /* Triumph3 and variants have reg name as PORT_FAULT_LINK_STATUS */
        if ( ((blktype == SOC_BLK_CLPORT) || (blktype == SOC_BLK_XLPORT) ||
              (blktype == SOC_BLK_XTPORT)) &&
             (SOC_REG_PORT_VALID(unit, PORT_FAULT_LINK_STATUSr, port)) ) {

            rval = 0;

            soc_reg_field_set(unit, PORT_FAULT_LINK_STATUSr, &rval,
                    REMOTE_FAULTf, 1);
            soc_reg_field_set(unit, PORT_FAULT_LINK_STATUSr, &rval,
                    LOCAL_FAULTf, 1);
            SOC_IF_ERROR_RETURN(WRITE_PORT_FAULT_LINK_STATUSr(unit,
                    port, rval));
        }
    }

    return SOC_E_NONE;
}

int
_soc_linkscan_hw_port_init(int unit, soc_port_t port)
{
    STATIC const soc_reg_t protocol_map_reg[] = {
        CMIC_MIIM_PROTOCOL_MAPr,
        CMIC_MIIM_PROTOCOL_MAP_HIr,
        CMIC_MIIM_PROTOCOL_MAP_HI_2r
    };
#if defined (BCM_CMICM_SUPPORT)
    STATIC const soc_reg_t protocol_map_reg_cmicm[] = {
                                    CMIC_MIIM_PROTOCOL_MAP_0r,
                                    CMIC_MIIM_PROTOCOL_MAP_1r,
                                    CMIC_MIIM_PROTOCOL_MAP_2r,
                                    CMIC_MIIM_PROTOCOL_MAP_3r,
#if defined (BCM_CMICDV2_SUPPORT)
                                    CMIC_MIIM_PROTOCOL_MAP_4r,
                                    CMIC_MIIM_PROTOCOL_MAP_5r,
#endif
#if defined (BCM_CMICDV3_SUPPORT)
                                    CMIC_MIIM_PROTOCOL_MAP_6r,
                                    CMIC_MIIM_PROTOCOL_MAP_7r,
                                    CMIC_MIIM_PROTOCOL_MAP_8r,
                                    CMIC_MIIM_PROTOCOL_MAP_9r,
#endif
                                  };
#endif /* End of BCM_CMICM_SUPPORT */

    STATIC const soc_reg_t *protocol_map = protocol_map_reg;
    STATIC const soc_reg_t int_sel_map_reg[] = {
        CMIC_MIIM_INT_SEL_MAPr,
        CMIC_MIIM_INT_SEL_MAP_HIr,
        CMIC_MIIM_INT_SEL_MAP_HI_2r
    };
#if defined (BCM_CMICM_SUPPORT)
    STATIC const soc_reg_t int_sel_map_reg_cmicm[] = {
        CMIC_MIIM_INT_SEL_MAP_0r,
        CMIC_MIIM_INT_SEL_MAP_1r,
        CMIC_MIIM_INT_SEL_MAP_2r,
        CMIC_MIIM_INT_SEL_MAP_3r,
#if defined (BCM_CMICDV2_SUPPORT)
        CMIC_MIIM_INT_SEL_MAP_4r,
        CMIC_MIIM_INT_SEL_MAP_5r,
#endif
#if defined (BCM_CMICDV3_SUPPORT)
        CMIC_MIIM_INT_SEL_MAP_6r,
        CMIC_MIIM_INT_SEL_MAP_7r,
        CMIC_MIIM_INT_SEL_MAP_8r,
        CMIC_MIIM_INT_SEL_MAP_9r,
#endif
    };
#endif /* Enf of BCM_CMICM_SUPPORT */

    STATIC const soc_reg_t *int_sel_map = int_sel_map_reg;
    STATIC const soc_reg_t port_type_map_reg[] = {
        CMIC_MIIM_PORT_TYPE_MAPr,
        CMIC_MIIM_PORT_TYPE_MAP_HIr
    };
    STATIC const soc_reg_t port_type_map_bus2_reg[] = {
        CMIC_MIIM_PORT_TYPE_MAP_BUS2r,
        CMIC_MIIM_PORT_TYPE_MAP_BUS2_HIr
    };
    STATIC const soc_reg_t bus_map_reg[] = {
        CMIC_MIIM_BUS_MAP_9_0r, CMIC_MIIM_BUS_MAP_19_10r,
        CMIC_MIIM_BUS_MAP_29_20r, CMIC_MIIM_BUS_MAP_39_30r,
        CMIC_MIIM_BUS_MAP_49_40r, CMIC_MIIM_BUS_MAP_59_50r,
        CMIC_MIIM_BUS_MAP_69_60r, CMIC_MIIM_BUS_MAP_79_70r
    };

#if defined (BCM_CMICDV2_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicdv2[] = {
        CMIC_MIIM_BUS_SEL_MAP_9_0r, CMIC_MIIM_BUS_SEL_MAP_19_10r,
        CMIC_MIIM_BUS_SEL_MAP_29_20r, CMIC_MIIM_BUS_SEL_MAP_39_30r,
        CMIC_MIIM_BUS_SEL_MAP_49_40r, CMIC_MIIM_BUS_SEL_MAP_59_50r,
        CMIC_MIIM_BUS_SEL_MAP_69_60r, CMIC_MIIM_BUS_SEL_MAP_79_70r,
        CMIC_MIIM_BUS_SEL_MAP_89_80r, CMIC_MIIM_BUS_SEL_MAP_99_90r,
        CMIC_MIIM_BUS_SEL_MAP_109_100r, CMIC_MIIM_BUS_SEL_MAP_119_110r,
        CMIC_MIIM_BUS_SEL_MAP_129_120r, CMIC_MIIM_BUS_SEL_MAP_139_130r,
        CMIC_MIIM_BUS_SEL_MAP_149_140r, CMIC_MIIM_BUS_SEL_MAP_159_150r,
        CMIC_MIIM_BUS_SEL_MAP_169_160r, CMIC_MIIM_BUS_SEL_MAP_179_170r,
        CMIC_MIIM_BUS_SEL_MAP_189_180r, CMIC_MIIM_BUS_SEL_MAP_191_190r
    };
#endif
#if defined (BCM_CMICDV3_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicdv3[] = {
        CMIC_MIIM_BUS_SEL_MAP_7_0r,     CMIC_MIIM_BUS_SEL_MAP_15_8r,
        CMIC_MIIM_BUS_SEL_MAP_23_16r,   CMIC_MIIM_BUS_SEL_MAP_31_24r,
        CMIC_MIIM_BUS_SEL_MAP_39_32r,   CMIC_MIIM_BUS_SEL_MAP_47_40r,
        CMIC_MIIM_BUS_SEL_MAP_55_48r,   CMIC_MIIM_BUS_SEL_MAP_63_56r,
        CMIC_MIIM_BUS_SEL_MAP_71_64r,   CMIC_MIIM_BUS_SEL_MAP_79_72r,
        CMIC_MIIM_BUS_SEL_MAP_87_80r,   CMIC_MIIM_BUS_SEL_MAP_95_88r,
        CMIC_MIIM_BUS_SEL_MAP_103_96r,  CMIC_MIIM_BUS_SEL_MAP_111_104r,
        CMIC_MIIM_BUS_SEL_MAP_119_112r, CMIC_V3_MIIM_BUS_SEL_MAP_127_120r,
        CMIC_MIIM_BUS_SEL_MAP_135_128r, CMIC_MIIM_BUS_SEL_MAP_143_136r,
        CMIC_MIIM_BUS_SEL_MAP_151_144r, CMIC_MIIM_BUS_SEL_MAP_159_152r,
        CMIC_MIIM_BUS_SEL_MAP_167_160r, CMIC_MIIM_BUS_SEL_MAP_175_168r,
        CMIC_MIIM_BUS_SEL_MAP_183_176r, CMIC_MIIM_BUS_SEL_MAP_191_184r,
        CMIC_MIIM_BUS_SEL_MAP_199_192r, CMIC_MIIM_BUS_SEL_MAP_207_200r,
        CMIC_MIIM_BUS_SEL_MAP_215_208r, CMIC_MIIM_BUS_SEL_MAP_223_216r,
        CMIC_MIIM_BUS_SEL_MAP_231_224r, CMIC_MIIM_BUS_SEL_MAP_239_232r,
        CMIC_MIIM_BUS_SEL_MAP_247_240r, CMIC_MIIM_BUS_SEL_MAP_255_248r,
        CMIC_MIIM_BUS_SEL_MAP_263_256r, CMIC_MIIM_BUS_SEL_MAP_271_264r,
        CMIC_MIIM_BUS_SEL_MAP_279_272r, CMIC_MIIM_BUS_SEL_MAP_287_280r,
        CMIC_MIIM_BUS_SEL_MAP_295_288r, CMIC_MIIM_BUS_SEL_MAP_303_296r,
        CMIC_MIIM_BUS_SEL_MAP_311_304r, CMIC_MIIM_BUS_SEL_MAP_319_312r
    };
#endif

#if defined (BCM_CMICM_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicm[] = {
        CMIC_MIIM_BUS_SEL_MAP_9_0r, CMIC_MIIM_BUS_SEL_MAP_19_10r,
        CMIC_MIIM_BUS_SEL_MAP_29_20r, CMIC_MIIM_BUS_SEL_MAP_39_30r,
        CMIC_MIIM_BUS_SEL_MAP_49_40r, CMIC_MIIM_BUS_SEL_MAP_59_50r,
        CMIC_MIIM_BUS_SEL_MAP_69_60r, CMIC_MIIM_BUS_SEL_MAP_79_70r,
        CMIC_MIIM_BUS_SEL_MAP_89_80r, CMIC_MIIM_BUS_SEL_MAP_99_90r,
        CMIC_MIIM_BUS_SEL_MAP_109_100r, CMIC_MIIM_BUS_SEL_MAP_119_110r,
        CMIC_MIIM_BUS_SEL_MAP_127_120r
    };
#endif /* End of BCM_CMICM_SUPPORT */

    STATIC const soc_reg_t *bus_map = bus_map_reg;
    STATIC const soc_field_t bus_map_9_0_field[] = {
        PORT_0_BUS_NUMf, PORT_1_BUS_NUMf, PORT_2_BUS_NUMf, PORT_3_BUS_NUMf,
        PORT_4_BUS_NUMf, PORT_5_BUS_NUMf, PORT_6_BUS_NUMf, PORT_7_BUS_NUMf,
        PORT_8_BUS_NUMf, PORT_9_BUS_NUMf
    };
    STATIC const soc_reg_t phy_map_reg[] = {
        CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r, CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r, CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r, CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r, CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r, CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r, CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r, CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r, CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_67_64r, CMIC_MIIM_EXT_PHY_ADDR_MAP_71_68r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_75_72r, CMIC_MIIM_EXT_PHY_ADDR_MAP_79_76r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_83_80r, CMIC_MIIM_EXT_PHY_ADDR_MAP_87_84r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_91_88r, CMIC_MIIM_EXT_PHY_ADDR_MAP_95_92r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_99_96r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_103_100r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_107_104r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_111_108r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_115_112r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_119_116r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_123_120r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_127_124r,
#if defined(BCM_CMICDV2_SUPPORT)
        CMIC_MIIM_EXT_PHY_ADDR_MAP_131_128r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_135_132r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_139_136r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_143_140r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_147_144r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_151_148r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_155_152r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_159_156r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_163_160r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_167_164r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_171_168r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_175_172r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_179_176r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_183_180r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_187_184r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_191_188r,
#endif
#if defined(BCM_CMICDV3_SUPPORT)
        CMIC_MIIM_EXT_PHY_ADDR_MAP_195_192r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_199_196r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_203_200r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_207_204r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_211_208r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_215_212r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_219_216r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_223_220r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_227_224r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_231_228r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_235_232r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_239_236r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_243_240r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_247_244r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_251_248r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_255_252r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_259_256r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_263_260r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_267_264r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_271_268r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_275_272r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_279_276r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_283_280r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_287_284r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_291_288r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_295_292r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_299_296r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_303_300r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_307_304r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_311_308r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_315_312r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_319_316r,
#endif
    };
    STATIC const soc_field_t phy_map_3_0_field[] = {
        PHY_ID_0f, PHY_ID_1f, PHY_ID_2f, PHY_ID_3f
    };
#ifdef BCM_RAVEN_SUPPORT
    soc_control_t   *soc = SOC_CONTROL(unit);
#endif
    soc_reg_t reg;
    soc_field_t field;
    int addr;
    uint32 rval;
    int bus_sel;
    soc_port_t phy_port, port_bit;
    int embedded_phy_port = 0;
    int phy_addr;
    int bus_map_array_size;

    /* TH3 FIXME!!!! THIS NEEDS TO CHANGE WHEN CMICX LINKSCAN_HW FUNCTION IS AVAILABLE!!!! */
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit) ||
        SOC_IS_FIRELIGHT(unit)) {
        return SOC_E_NONE; 
    }

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    } else {
        phy_port = port;
    }

    port_bit = phy_port;
#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        port_bit = (port_bit >= 58) ? (port_bit + GREYHOUND2_PHY_PORT_OFFSET)
                                    : port_bit;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
    PHY_PORT_TO_LINKSCAN_PORT_XLATE(unit, port_bit);
    
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit) || SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_KATANAX(unit)) {
        /*
         * SC/CQ devices omit the CMIC port from the PHY counting.  All
         * "port bitmaps" are shifted by 1.
         */
        if (port == CMIC_PORT(unit)) {
            /* This should not be triggered */
            return SOC_E_PORT;
        }
        if (!SOC_IS_KATANAX(unit)) {
            port_bit -= 1;
        }
    }
#endif

    bus_map_array_size = COUNTOF(bus_map_reg);
#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        protocol_map = protocol_map_reg_cmicm;
        int_sel_map = int_sel_map_reg_cmicm;
        bus_map = bus_map_reg_cmicm;
        bus_map_array_size = COUNTOF(bus_map_reg_cmicm);
#if defined (BCM_CMICDV2_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicd_v2)) {
            bus_map = bus_map_reg_cmicdv2;
            bus_map_array_size = COUNTOF(bus_map_reg_cmicdv2);
        }
#endif /* BCM_CMICDV2_SUPPORT */
#if defined (BCM_CMICDV3_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicd_v3)) {
            bus_map = bus_map_reg_cmicdv3;
            bus_map_array_size = COUNTOF(bus_map_reg_cmicdv3);
        }
#endif /* BCM_CMICDV3_SUPPORT */
    }
#endif

    /*
     * Check If Hardware Linkscan should use Clause 45 mode
     */
    if ((IS_XE_PORT(unit, port) || IS_CE_PORT(unit, port) || 
         IS_HG_PORT(unit, port) || IS_GE_PORT(unit, port)) && 
        _soc_linkscan_phy_flags_test(unit, port, PHY_FLAGS_C45)) {
        reg = protocol_map[port_bit / 32];
        if (SOC_REG_IS_VALID(unit, reg)) {
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            rval = soc_pci_read(unit, addr);
            rval |= 1 << (port_bit % 32);
            soc_pci_write(unit, addr, rval);
        }
    }

    /*
     *  Select the appropriate MDIO bus
     */
    phy_addr = _soc_linkscan_port_to_phyaddr(unit, port);

    if (SOC_IS_TD_TT(unit) || SOC_IS_METROLITE(unit)) {
        bus_sel = ((phy_addr & 0x300) >> 6) |
            ((phy_addr & 0x60) >> 5) ;
    } else if (SOC_IS_TRX(unit)) {
        bus_sel = (phy_addr & 0x60) >> 5; /* bus 0, 1, and 2 */
    } else {
        bus_sel = (phy_addr & 0x40) >> 6; /* bus 0 and 1 */
    }

#if defined (BCM_CMICDV3_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicd_v3)) {
        if ((port_bit / 8) < bus_map_array_size) {
            reg = bus_map[port_bit / 8];
        } else {
            return SOC_E_INTERNAL;
        }
        field = bus_map_9_0_field[port_bit % 8];

        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        rval = soc_pci_read(unit, addr);
        soc_reg_field_set(unit, bus_map[0], &rval, field, bus_sel);
        soc_pci_write(unit, addr, rval);

    } else
#endif /* BCM_CMICDV3_SUPPORT */
    if (SOC_REG_IS_VALID(unit, bus_map[0])) {
        if ((port_bit / 10) < bus_map_array_size) {
            reg = bus_map[port_bit / 10];
        } else {
            return SOC_E_INTERNAL;
        }
        field = bus_map_9_0_field[port_bit % 10];
        if (SOC_REG_IS_VALID(unit, reg)) {
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            rval = soc_pci_read(unit, addr);
            soc_reg_field_set(unit, bus_map[0], &rval, field, bus_sel);
            soc_pci_write(unit, addr, rval);
        }
    } else {
        reg = INVALIDr;
        if (bus_sel == 1) {
            if ((port_bit / 32) < COUNTOF(port_type_map_reg)) {
                reg = port_type_map_reg[port_bit / 32];
            } else {
                return SOC_E_INTERNAL;
            }
        } else if ((bus_sel == 2) && !SOC_IS_SHADOW(unit)) {
            if ((port_bit / 32) < COUNTOF(port_type_map_bus2_reg)) {
                reg = port_type_map_bus2_reg[port_bit / 32];
            } else {
                return SOC_E_INTERNAL;
            }
        }
        if (SOC_REG_IS_VALID(unit, reg)) {
            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            rval = soc_pci_read(unit, addr);
            rval |= 1 << (port_bit % 32);
            soc_pci_write(unit, addr, rval);
        }
    }

    /*
     * Check If Hardware Linkscan should use internal phy
     */
    embedded_phy_port = 0;
    if (soc_feature(unit, soc_feature_gphy)) {
        if (SOC_IS_HURRICANE2(unit)) {
            if ((phy_port < 18)) {
                embedded_phy_port = 1;
            }
        }
#ifdef BCM_HURRICANE3_SUPPORT
        else if (SOC_IS_HURRICANE3(unit)) {
            if (soc_feature(unit, soc_feature_wh2)) {
                uint8 is_gphy = 0;

                (void)soc_wolfhound2_gphy_get(unit, port, &is_gphy);
                embedded_phy_port = (is_gphy) ? 1 : 0;
            } else {
                if (((phy_port >= 10) && (phy_port <= 17)) ||
                   ((phy_port >= 26) && (phy_port <= 33))) {
                    embedded_phy_port = 1;
                }
            }
        }
#endif /* BCM_HURRICANE3_SUPPORT */
    }
    reg = int_sel_map[port_bit / 32];
    if (SOC_REG_IS_VALID(unit, reg)) {
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        rval = soc_pci_read(unit, addr);
        if (!_soc_linkscan_phy_flags_test(unit, port, PHY_FLAGS_EXTERNAL_PHY) ||
            _soc_linkscan_phy_flags_test(unit, port, PHY_FLAGS_REPEATER) ||
            embedded_phy_port) {
            rval |= 1 << (port_bit % 32);
            soc_pci_write(unit, addr, rval);
        } else {
            rval &= ~(1<< (port_bit % 32));
            soc_pci_write(unit, addr, rval);

#ifdef BCM_RAVEN_SUPPORT
            if (SOC_IS_RAVEN(unit)) {
                /* Assume:
                 *   - CMIC related processing time is negligible
                 *   - MIIM interface is running at 2.5MHz
                 *   - Each register read is 65 bits (32 preamble, 32 payload, 1 idle)
                 */
                 (soc->hw_linkscan_delay_ns) += 400 * 65;
            }
#endif
        }
    }

    /* Re-initialize the phy port map for the unit */
    /* Use MDIO address re-mapping for hardware linkscan */
    assert(port_bit >= 0);
    assert(port_bit / 4 < sizeof(phy_map_reg) / sizeof(phy_map_reg[0]));

    /*
     * COVERITY
     *
     * assert validates the input
     */
    /* coverity[overrun-local : FALSE] */
    reg = phy_map_reg[port_bit / 4];
    field = phy_map_3_0_field[port_bit % 4];
    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
    rval = soc_pci_read(unit, addr);
    soc_reg_field_set(unit, phy_map_reg[0], &rval, field,
                      PHY_ADDR(unit, port) & 0x1f);
    soc_pci_write(unit, addr, rval);


#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        READ_CMIC_MIIM_SCAN_CTRLr(unit, &rval);
        soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &rval, 
                          MIIM_ADDR_MAP_ENABLEf, 1);
        WRITE_CMIC_MIIM_SCAN_CTRLr(unit, rval);
    } else
#endif
    {
        READ_CMIC_CONFIGr(unit, &rval);
        soc_reg_field_set(unit, CMIC_CONFIGr, &rval, MIIM_ADDR_MAP_ENABLEf, 1);
        WRITE_CMIC_CONFIGr(unit, rval);
    }

    /*
     * Configure remote/local faults to trigger link interrupt.
     * The configuration is included in Portmod internally.
     */
    if (!SOC_PORT_USE_PORTCTRL(unit, port)) {
        _soc_linkscan_fault_status_set(unit, port);
    }

    return SOC_E_NONE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */


#if defined (BCM_ESW_SUPPORT)
STATIC int
_soc_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    uint32              link_stat = 0;
#ifdef BCM_GREYHOUND2_SUPPORT
    uint32              scan_stat = 0;
#endif /* BCM_GREYHOUND2_SUPPORT */
    uint32              link_pbmp = 0;
    SHR_BITDCL          pb[_SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX)] = {0};
#ifdef BCM_GREYHOUND2_SUPPORT
    SHR_BITDCL          ps[_SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX)] = {0};
#endif /* BCM_GREYHOUND2_SUPPORT */
    soc_port_t          port_bit, phy_port, port;
    int                 num_phy_port = 0;

    if (NULL == hw_link) {
        return SOC_E_PARAM;
    }

#if defined (BCM_CMICX_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicx)) {
#if CMICX_HW_LINKSCAN
        SOC_PBMP_CLEAR(*hw_link);
        return soc_cmicx_linkscan_hw_link_get(unit, hw_link);
#endif
        return SOC_E_NONE;
    }
#endif

     sal_memset(pb, 0, sizeof(SHR_BITDCL) * _SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX));
#ifdef BCM_GREYHOUND2_SUPPORT
     sal_memset(ps, 0, sizeof(SHR_BITDCL) * _SHR_BITDCLSIZE(SOC_CMIC_PHY_PORT_MAX));

     if (SOC_IS_GREYHOUND2(unit)) {
         READ_CMIC_MIIM_SCAN_PORTS_0r(unit, &scan_stat);
         SOC_BITWORD_SET(ps, 0, scan_stat);
         READ_CMIC_MIIM_SCAN_PORTS_1r(unit, &scan_stat);
         SOC_BITWORD_SET(ps, 1, scan_stat);
         READ_CMIC_MIIM_SCAN_PORTS_2r(unit, &scan_stat);
         SOC_BITWORD_SET(ps, 2, scan_stat);
     }
#endif /* BCM_GREYHOUND2_SUPPORT */

    /*
     * Read CMIC link status to determine which ports that
     * actually need to be scanned.
     */
#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        READ_CMIC_MIIM_LINK_STATUS_0r(unit, &link_stat);
    } else
#endif
    {
        READ_CMIC_LINK_STATr(unit, &link_stat);
    }

#if defined(BCM_FIREBOLT_SUPPORT)
    if (soc_feature(unit, soc_feature_status_link_fail)) {
        uint32  intsel_reg;

        READ_CMIC_MIIM_INT_SEL_MAPr(unit, &intsel_reg);
        link_stat ^= intsel_reg;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm)) {
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_0r,
                                      link_stat, PORT_BITMAPf);
    } else
#endif
    {
        link_pbmp = soc_reg_field_get(unit, CMIC_LINK_STATr,
                                        link_stat, PORT_BITMAPf);
    }
#if defined (BCM_GOLDWING_SUPPORT)
    if (SOC_IS_GOLDWING(unit)) {
        /* (MSB) 15-14-19-18-17-16-13-12-11-10-9-8-7-6-5-4-3-2-1-0 (LSB) */
        link_pbmp =  (link_pbmp & 0x00003FFF) |
            ((link_pbmp & 0x000C0000) >> 4) |
            ((link_pbmp & 0x0003C000) << 2);
    }
#endif /* BCM_GOLDWING_SUPPORT */

#if defined (BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        /* CMIC port not included in link status */
        link_pbmp <<=  1;
    }
#endif /* BCM_SCORPION_SUPPORT */

    SOC_BITWORD_SET(pb, 0, link_pbmp);

    /* Check for more than 32 ports per unit */
#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_1r)) {
        READ_CMIC_MIIM_LINK_STATUS_1r(unit, &link_stat);
        SOC_BITWORD_SET(pb, 1,
                        soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_1r,
                                          link_stat, PORT_BITMAPf));
    } else
#endif /* BCM_CMICM_SUPPORT */
    {
#if defined (BCM_RAPTOR_SUPPORT) || defined (BCM_TRIUMPH_SUPPORT) || defined (BCM_KATANA_SUPPORT)
        if ((SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANE(unit)) ||
            soc_feature(unit, soc_feature_register_hi)) {
            READ_CMIC_LINK_STAT_HIr(unit, &link_stat);
            SOC_BITWORD_SET(pb, 1,
                            soc_reg_field_get(unit, CMIC_LINK_STAT_HIr,
                                              link_stat, PORT_BITMAPf));
        }
#endif
    }

#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_2r) ) {
        READ_CMIC_MIIM_LINK_STATUS_2r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_2r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 2, link_pbmp);
    } else
#endif /* BCM_CMICM_SUPPORT */
    {
#if defined (BCM_TRIDENT_SUPPORT)
        if (SOC_REG_IS_VALID(unit, CMIC_LINK_STAT_HI_2r)) {
            READ_CMIC_LINK_STAT_HI_2r(unit, &link_stat);
            link_pbmp = soc_reg_field_get(unit, CMIC_LINK_STAT_HI_2r,
                                          link_stat, PORT_BITMAPf);
            SOC_BITWORD_SET(pb, 2, link_pbmp);
        }
#endif /* BCM_TRIDENT_SUPPORT */
    }

#if defined (BCM_CMICM_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicm) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_3r)) {
        READ_CMIC_MIIM_LINK_STATUS_3r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_3r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 3, link_pbmp);
    }

#if defined (BCM_CMICDV2_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicd_v2) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_4r)) {
        READ_CMIC_MIIM_LINK_STATUS_4r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_4r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 4, link_pbmp);
    }

    if (soc_feature(unit, soc_feature_cmicd_v2) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_5r)) {
        READ_CMIC_MIIM_LINK_STATUS_5r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_5r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 5, link_pbmp);
    }

#if defined (BCM_CMICDV4_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicd_v4) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_6r)) {
        READ_CMIC_MIIM_LINK_STATUS_6r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_6r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 6, link_pbmp);
    }

    if (soc_feature(unit, soc_feature_cmicd_v4) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_7r)) {
        READ_CMIC_MIIM_LINK_STATUS_7r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_7r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 7, link_pbmp);
    }

    if (soc_feature(unit, soc_feature_cmicd_v4) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_8r)) {
        READ_CMIC_MIIM_LINK_STATUS_8r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_8r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 8, link_pbmp);
    }

    if (soc_feature(unit, soc_feature_cmicd_v4) &&
        SOC_REG_IS_VALID(unit, CMIC_MIIM_LINK_STATUS_9r)) {
        READ_CMIC_MIIM_LINK_STATUS_9r(unit, &link_stat);
        link_pbmp = soc_reg_field_get(unit, CMIC_MIIM_LINK_STATUS_9r,
                                      link_stat, PORT_BITMAPf);
        SOC_BITWORD_SET(pb, 9, link_pbmp);
    }
#endif /* BCM_CMICDV4_SUPPORT */
#endif /* BCM_CMICDV2_SUPPORT */
#endif /*BCM_CMICM_SUPPORT*/
   if (soc_feature(unit, soc_feature_logical_port_num)) {

#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWK(unit)) {
            num_phy_port = NUM_PIPE(unit) * (soc_mem_index_count (unit,
                            ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm));
        } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            num_phy_port = NUM_PIPE(unit) * (soc_mem_index_count (unit,
                            ING_IDB_TO_DEVICE_PORT_NUMBER_MAPPING_TABLEm));
        } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            num_phy_port = NUM_PIPE(unit) * (soc_mem_index_count (unit,
                            ING_PHY_TO_IDB_PORT_MAPm));
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        {
        num_phy_port = soc_mem_index_count
            (unit, ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm);
        }

        SOC_PBMP_CLEAR(*hw_link);
        SHR_BIT_ITER(pb, SOC_CMIC_PHY_PORT_MAX, port_bit) {
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
                SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
                phy_port = port_bit;
#ifdef BCM_GREYHOUND2_SUPPORT
              /*
               * Use CMIC_MIIM_SCAN_PORTS bitmap to check the port link
               * status is valid or not due to the difference between
               * phy port allocation and hardware implementation.
               */
                if (SOC_IS_GREYHOUND2(unit)) {
                    if (SHR_BITGET(ps, phy_port)) {
                        phy_port = (phy_port >= 65) ?
                                    (phy_port - GREYHOUND2_PHY_PORT_OFFSET) :
                                    phy_port;
                    } else {
                        continue;
                    }
                }
#endif /* BCM_GREYHOUND2_SUPPORT */
            } else {
                phy_port = port_bit + 1;
                LINKSCAN_PORT_TO_PHY_PORT_XLATE(unit, phy_port);
                if (phy_port == -1) {
                    continue;
                }
            }
#ifdef BCM_TOMAHAWK2_SUPPORT
            if (SOC_IS_TOMAHAWK2(unit)) {
                if (phy_port > num_phy_port) {
                    break;
                }
            } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
            if (phy_port >= num_phy_port) {
                break;
            }
            /*
             * COVERITY
             *
             * The check above will protect any real cases so the
             * index does not exceed the declared size.
             */
            /* coverity[overrun-local] */
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != -1) {
                SOC_PBMP_PORT_ADD(*hw_link, port);
            }
        }
    } else {
        SOC_PBMP_CLEAR(*hw_link);
        SHR_BIT_ITER(pb, SOC_CMIC_PHY_PORT_MAX, port_bit) {
            port = port_bit;
            /* coverity[overrun-local] */
            SOC_PBMP_PORT_ADD(*hw_link, port);
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_ESW_SUPPORT */

/*
 * This function updates port phy address in CMIC external
 * phy address registers for hardware linkscan
 */
int
soc_linkscan_hw_port_update(int unit, soc_port_t port)
{
#if defined(BCM_ESW_SUPPORT)
    STATIC const soc_reg_t port_type_map_reg[] = {
        CMIC_MIIM_PORT_TYPE_MAPr,
        CMIC_MIIM_PORT_TYPE_MAP_HIr
    };
    STATIC const soc_reg_t port_type_map_bus2_reg[] = {
        CMIC_MIIM_PORT_TYPE_MAP_BUS2r,
        CMIC_MIIM_PORT_TYPE_MAP_BUS2_HIr
    };
    STATIC const soc_reg_t bus_map_reg[] = {
        CMIC_MIIM_BUS_MAP_9_0r, CMIC_MIIM_BUS_MAP_19_10r,
        CMIC_MIIM_BUS_MAP_29_20r, CMIC_MIIM_BUS_MAP_39_30r,
        CMIC_MIIM_BUS_MAP_49_40r, CMIC_MIIM_BUS_MAP_59_50r,
        CMIC_MIIM_BUS_MAP_69_60r, CMIC_MIIM_BUS_MAP_79_70r
    };

#if defined (BCM_CMICDV2_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicdv2[] = {
        CMIC_MIIM_BUS_SEL_MAP_9_0r, CMIC_MIIM_BUS_SEL_MAP_19_10r,
        CMIC_MIIM_BUS_SEL_MAP_29_20r, CMIC_MIIM_BUS_SEL_MAP_39_30r,
        CMIC_MIIM_BUS_SEL_MAP_49_40r, CMIC_MIIM_BUS_SEL_MAP_59_50r,
        CMIC_MIIM_BUS_SEL_MAP_69_60r, CMIC_MIIM_BUS_SEL_MAP_79_70r,
        CMIC_MIIM_BUS_SEL_MAP_89_80r, CMIC_MIIM_BUS_SEL_MAP_99_90r,
        CMIC_MIIM_BUS_SEL_MAP_109_100r, CMIC_MIIM_BUS_SEL_MAP_119_110r,
        CMIC_MIIM_BUS_SEL_MAP_129_120r, CMIC_MIIM_BUS_SEL_MAP_139_130r,
        CMIC_MIIM_BUS_SEL_MAP_149_140r, CMIC_MIIM_BUS_SEL_MAP_159_150r,
        CMIC_MIIM_BUS_SEL_MAP_169_160r, CMIC_MIIM_BUS_SEL_MAP_179_170r,
        CMIC_MIIM_BUS_SEL_MAP_189_180r, CMIC_MIIM_BUS_SEL_MAP_191_190r
    };
#endif
#if defined (BCM_CMICDV3_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicdv3[] = {
        CMIC_MIIM_BUS_SEL_MAP_7_0r,     CMIC_MIIM_BUS_SEL_MAP_15_8r,
        CMIC_MIIM_BUS_SEL_MAP_23_16r,   CMIC_MIIM_BUS_SEL_MAP_31_24r,
        CMIC_MIIM_BUS_SEL_MAP_39_32r,   CMIC_MIIM_BUS_SEL_MAP_47_40r,
        CMIC_MIIM_BUS_SEL_MAP_55_48r,   CMIC_MIIM_BUS_SEL_MAP_63_56r,
        CMIC_MIIM_BUS_SEL_MAP_71_64r,   CMIC_MIIM_BUS_SEL_MAP_79_72r,
        CMIC_MIIM_BUS_SEL_MAP_87_80r,   CMIC_MIIM_BUS_SEL_MAP_95_88r,
        CMIC_MIIM_BUS_SEL_MAP_103_96r,  CMIC_MIIM_BUS_SEL_MAP_111_104r,
        CMIC_MIIM_BUS_SEL_MAP_119_112r, CMIC_V3_MIIM_BUS_SEL_MAP_127_120r,
        CMIC_MIIM_BUS_SEL_MAP_135_128r, CMIC_MIIM_BUS_SEL_MAP_143_136r,
        CMIC_MIIM_BUS_SEL_MAP_151_144r, CMIC_MIIM_BUS_SEL_MAP_159_152r,
        CMIC_MIIM_BUS_SEL_MAP_167_160r, CMIC_MIIM_BUS_SEL_MAP_175_168r,
        CMIC_MIIM_BUS_SEL_MAP_183_176r, CMIC_MIIM_BUS_SEL_MAP_191_184r,
        CMIC_MIIM_BUS_SEL_MAP_199_192r, CMIC_MIIM_BUS_SEL_MAP_207_200r,
        CMIC_MIIM_BUS_SEL_MAP_215_208r, CMIC_MIIM_BUS_SEL_MAP_223_216r,
        CMIC_MIIM_BUS_SEL_MAP_231_224r, CMIC_MIIM_BUS_SEL_MAP_239_232r,
        CMIC_MIIM_BUS_SEL_MAP_247_240r, CMIC_MIIM_BUS_SEL_MAP_255_248r,
        CMIC_MIIM_BUS_SEL_MAP_263_256r, CMIC_MIIM_BUS_SEL_MAP_271_264r,
        CMIC_MIIM_BUS_SEL_MAP_279_272r, CMIC_MIIM_BUS_SEL_MAP_287_280r,
        CMIC_MIIM_BUS_SEL_MAP_295_288r, CMIC_MIIM_BUS_SEL_MAP_303_296r,
        CMIC_MIIM_BUS_SEL_MAP_311_304r, CMIC_MIIM_BUS_SEL_MAP_319_312r
    };
#endif

#if defined (BCM_CMICM_SUPPORT)
    STATIC const soc_reg_t bus_map_reg_cmicm[] = {
        CMIC_MIIM_BUS_SEL_MAP_9_0r, CMIC_MIIM_BUS_SEL_MAP_19_10r,
        CMIC_MIIM_BUS_SEL_MAP_29_20r, CMIC_MIIM_BUS_SEL_MAP_39_30r,
        CMIC_MIIM_BUS_SEL_MAP_49_40r, CMIC_MIIM_BUS_SEL_MAP_59_50r,
        CMIC_MIIM_BUS_SEL_MAP_69_60r, CMIC_MIIM_BUS_SEL_MAP_79_70r,
        CMIC_MIIM_BUS_SEL_MAP_89_80r, CMIC_MIIM_BUS_SEL_MAP_99_90r,
        CMIC_MIIM_BUS_SEL_MAP_109_100r, CMIC_MIIM_BUS_SEL_MAP_119_110r,
        CMIC_MIIM_BUS_SEL_MAP_127_120r
    };
#endif /* End of BCM_CMICM_SUPPORT */

    STATIC const soc_reg_t *bus_map = bus_map_reg;
    STATIC const soc_field_t bus_map_9_0_field[] = {
        PORT_0_BUS_NUMf, PORT_1_BUS_NUMf, PORT_2_BUS_NUMf, PORT_3_BUS_NUMf,
        PORT_4_BUS_NUMf, PORT_5_BUS_NUMf, PORT_6_BUS_NUMf, PORT_7_BUS_NUMf,
        PORT_8_BUS_NUMf, PORT_9_BUS_NUMf
    };
    STATIC const soc_reg_t phy_map_reg[] = {
        CMIC_MIIM_EXT_PHY_ADDR_MAP_3_0r, CMIC_MIIM_EXT_PHY_ADDR_MAP_7_4r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_11_8r, CMIC_MIIM_EXT_PHY_ADDR_MAP_15_12r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_19_16r, CMIC_MIIM_EXT_PHY_ADDR_MAP_23_20r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_27_24r, CMIC_MIIM_EXT_PHY_ADDR_MAP_31_28r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_35_32r, CMIC_MIIM_EXT_PHY_ADDR_MAP_39_36r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_43_40r, CMIC_MIIM_EXT_PHY_ADDR_MAP_47_44r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_51_48r, CMIC_MIIM_EXT_PHY_ADDR_MAP_55_52r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_59_56r, CMIC_MIIM_EXT_PHY_ADDR_MAP_63_60r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_67_64r, CMIC_MIIM_EXT_PHY_ADDR_MAP_71_68r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_75_72r, CMIC_MIIM_EXT_PHY_ADDR_MAP_79_76r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_83_80r, CMIC_MIIM_EXT_PHY_ADDR_MAP_87_84r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_91_88r, CMIC_MIIM_EXT_PHY_ADDR_MAP_95_92r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_99_96r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_103_100r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_107_104r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_111_108r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_115_112r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_119_116r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_123_120r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_127_124r,
#if defined(BCM_CMICDV2_SUPPORT)
        CMIC_MIIM_EXT_PHY_ADDR_MAP_131_128r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_135_132r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_139_136r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_143_140r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_147_144r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_151_148r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_155_152r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_159_156r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_163_160r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_167_164r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_171_168r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_175_172r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_179_176r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_183_180r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_187_184r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_191_188r,
#endif
#if defined(BCM_CMICDV3_SUPPORT)
        CMIC_MIIM_EXT_PHY_ADDR_MAP_195_192r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_199_196r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_203_200r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_207_204r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_211_208r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_215_212r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_219_216r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_223_220r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_227_224r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_231_228r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_235_232r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_239_236r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_243_240r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_247_244r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_251_248r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_255_252r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_259_256r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_263_260r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_267_264r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_271_268r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_275_272r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_279_276r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_283_280r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_287_284r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_291_288r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_295_292r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_299_296r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_303_300r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_307_304r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_311_308r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_315_312r,
        CMIC_MIIM_EXT_PHY_ADDR_MAP_319_316r,
#endif
    };
    STATIC const soc_field_t phy_map_3_0_field[] = {
        PHY_ID_0f, PHY_ID_1f, PHY_ID_2f, PHY_ID_3f
    };
    soc_reg_t reg;
    soc_field_t field;
    int addr;
    uint32 rval;
    int bus_sel;
    soc_port_t phy_port, port_bit;
    int phy_addr;
    int bus_map_array_size;

    /* TH3 FIXME!!!! THIS NEEDS TO CHANGE WHEN CMICX LINKSCAN_HW FUNCTION IS AVAILABLE!!!! */
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit) ||
        SOC_IS_FIRELIGHT(unit)) {
        return SOC_E_NONE;
    }

    if (SOC_IS_ESW(unit)) {

        /* hold MIIM lock */
        MIIM_LOCK(unit);

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }

        port_bit = phy_port;
#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            port_bit = (port_bit >= 58) ? (port_bit + GREYHOUND2_PHY_PORT_OFFSET)
                                        : port_bit;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
        PHY_PORT_TO_LINKSCAN_PORT_XLATE(unit, port_bit);

#ifdef BCM_SCORPION_SUPPORT
        if (SOC_IS_SC_CQ(unit) || SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit) ||
            SOC_IS_KATANAX(unit)) {
            /*
             * SC/CQ devices omit the CMIC port from the PHY counting.  All
             * "port bitmaps" are shifted by 1.
             */
            if (port == CMIC_PORT(unit)) {
                /* This should not be triggered */

                /* Release MIIM lock */
                MIIM_UNLOCK(unit);
                return SOC_E_PORT;
            }
            if (!SOC_IS_KATANAX(unit)) {
                port_bit -= 1;
            }
        }
#endif

        bus_map_array_size = COUNTOF(bus_map_reg);
#if defined (BCM_CMICM_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicm)) {
            bus_map = bus_map_reg_cmicm;
            bus_map_array_size = COUNTOF(bus_map_reg_cmicm);
#if defined (BCM_CMICDV2_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicd_v2)) {
                bus_map = bus_map_reg_cmicdv2;
                bus_map_array_size = COUNTOF(bus_map_reg_cmicdv2);
            }
#endif /* BCM_CMICDV2_SUPPORT */
#if defined (BCM_CMICDV3_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicd_v3)) {
                bus_map = bus_map_reg_cmicdv3;
                bus_map_array_size = COUNTOF(bus_map_reg_cmicdv3);
            }
#endif /* BCM_CMICDV3_SUPPORT */
        }
#endif

        /*
         *  Select the appropriate MDIO bus
         */
        phy_addr = _soc_linkscan_port_to_phyaddr(unit, port);

        if (SOC_IS_TD_TT(unit) || SOC_IS_METROLITE(unit)) {
            bus_sel = ((phy_addr & 0x300) >> 6) |
                ((phy_addr & 0x60) >> 5) ;
        } else if (SOC_IS_TRX(unit)) {
            bus_sel = (phy_addr & 0x60) >> 5; /* bus 0, 1, and 2 */
        } else {
            bus_sel = (phy_addr & 0x40) >> 6; /* bus 0 and 1 */
        }

#if defined (BCM_CMICDV3_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicd_v3)) {
            if ((port_bit / 8) < bus_map_array_size) {
                reg = bus_map[port_bit / 8];
            } else {
                return SOC_E_INTERNAL;
            }
            field = bus_map_9_0_field[port_bit % 8];

            addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
            rval = soc_pci_read(unit, addr);
            soc_reg_field_set(unit, bus_map[0], &rval, field, bus_sel);
            soc_pci_write(unit, addr, rval);

        } else
#endif /* BCM_CMICDV3_SUPPORT */
        if (SOC_REG_IS_VALID(unit, bus_map[0])) {
            if ((port_bit / 10) < bus_map_array_size) {
                reg = bus_map[port_bit / 10];
            } else {
                return SOC_E_INTERNAL;
            }
            field = bus_map_9_0_field[port_bit % 10];
            if (SOC_REG_IS_VALID(unit, reg)) {
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                rval = soc_pci_read(unit, addr);
                soc_reg_field_set(unit, bus_map[0], &rval, field, bus_sel);
                soc_pci_write(unit, addr, rval);
            }
        } else {
            reg = INVALIDr;
            if (bus_sel == 1) {
                if ((port_bit / 32) < COUNTOF(port_type_map_reg)) {
                    reg = port_type_map_reg[port_bit / 32];
                } else {
                    return SOC_E_INTERNAL;
                }
            } else if ((bus_sel == 2) && !SOC_IS_SHADOW(unit)) {
                if ((port_bit / 32) < COUNTOF(port_type_map_bus2_reg)) {
                    reg = port_type_map_bus2_reg[port_bit / 32];
                } else {
                    return SOC_E_INTERNAL;
                }
            }
            if (SOC_REG_IS_VALID(unit, reg)) {
                addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                rval = soc_pci_read(unit, addr);
                rval |= 1 << (port_bit % 32);
                soc_pci_write(unit, addr, rval);
            }
        }

        /* Re-initialize the phy port map for the unit */
        /* Use MDIO address re-mapping for hardware linkscan */
        assert(port_bit >= 0);
        assert(port_bit / 4 < sizeof(phy_map_reg) / sizeof(phy_map_reg[0]));

        /*
         * COVERITY
         *
         * assert validates the input
         */
        /* coverity[overrun-local : FALSE] */
        reg = phy_map_reg[port_bit / 4];
        field = phy_map_3_0_field[port_bit % 4];
        addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
        rval = soc_pci_read(unit, addr);
        soc_reg_field_set(unit, phy_map_reg[0], &rval, field,
                          PHY_ADDR(unit, port) & 0x1f);
        soc_pci_write(unit, addr, rval);


#if defined (BCM_CMICM_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicm)) {
            READ_CMIC_MIIM_SCAN_CTRLr(unit, &rval);
            soc_reg_field_set(unit, CMIC_MIIM_SCAN_CTRLr, &rval,
                              MIIM_ADDR_MAP_ENABLEf, 1);
            WRITE_CMIC_MIIM_SCAN_CTRLr(unit, rval);
        } else
#endif
#if defined (BCM_CMICX_SUPPORT)
        if (soc_feature(unit, soc_feature_cmicx)) {
            
        } else
#endif
        {
            READ_CMIC_CONFIGr(unit, &rval);
            soc_reg_field_set(unit, CMIC_CONFIGr, &rval, MIIM_ADDR_MAP_ENABLEf, 1);
            WRITE_CMIC_CONFIGr(unit, rval);
        }

        /* Release MIIM lock */
        MIIM_UNLOCK(unit);
    }
#endif /* defined(BCM_ESW_SUPPORT) */
    return SOC_E_NONE;
}

int
soc_linkscan_hw_init(int unit)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int automedium = 0;
    uint32 rval = 0;
#endif

    if (SOC_IS_XGS3_SWITCH(unit)) {
        soc_port_t port;
        pbmp_t phy_port_pbmp;
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            if (!(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
#if CMICX_HW_LINKSCAN
                SOC_IF_ERROR_RETURN(soc_cmicx_linkscan_hw_init(unit));
#endif
                return SOC_E_NONE;
            } else {
                /* CMICx supported and BCMSIM/PLISIM true */
                return SOC_E_NONE;
            }
        }
        else
#endif
        {
            SOC_PBMP_ASSIGN(phy_port_pbmp, PBMP_PORT_ALL(unit));
            PBMP_ITER(phy_port_pbmp, port) {
               /*
                * COVERITY
                *
                * The iterator above will protect any real cases so the
                * port index does not exceed the declared size.
               */
               /* coverity[overrun-call] */
               SOC_IF_ERROR_RETURN
                    (_soc_linkscan_hw_port_init(unit, port));
            }
        }
#ifdef BCM_XGS3_SWITCH_SUPPORT
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            automedium |= soc_property_port_get(unit, port,
                                    spn_PHY_AUTOMEDIUM, 0);
        }
        if (!automedium) {
            /*
             * Setting the Auto Scan Address for HW linkscan
             * Bit 2 of 0x01 register (MII_STATUS for Cu and Fiber)
             */
            if (soc_feature(unit, soc_feature_cmicm)) {
                READ_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, &rval);
                soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                       MIIM_LINK_STATUS_BIT_POSITIONf, 0x2);
                if (SOC_REG_FIELD_VALID(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr,
                                        CLAUSE_22_REGADRf)) {
                    soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                      CLAUSE_22_REGADRf, 0x1);
                }
                if (SOC_REG_FIELD_VALID(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr,
                                        CLAUSE_45_REGADRf)) {
                    soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                      CLAUSE_45_REGADRf, 0x1);
                }
                if (SOC_REG_FIELD_VALID(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr,
                                        CLAUSE_45_DTYPEf)) {
                    soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                      CLAUSE_45_DTYPEf, 0x1);
                }
                WRITE_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, rval);
             } else if (SOC_IS_HAWKEYE(unit) || (SOC_IS_TR_VL(unit) && 
                      !(SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
                        SOC_IS_GREYHOUND2(unit)))) {
                READ_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, &rval);
                soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                       MIIM_LINK_STATUS_BIT_POSITIONf, 0x2);
                soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, &rval,
                                                 MIIM_DEVICE_ADDRESSf, 0x1);
                WRITE_CMIC_MIIM_AUTO_SCAN_ADDRESSr(unit, rval);
            } else {
                if (SOC_REG_IS_VALID(unit, CMIC_MIIM_ADDRESSr)) {
                    if (soc_reg_field_valid(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr, 
                                            CLAUSE_22_REGADRf)) {
                        READ_CMIC_MIIM_ADDRESSr(unit, &rval);
                        soc_reg_field_set(unit, CMIC_MIIM_AUTO_SCAN_ADDRESSr,
                                          &rval, CLAUSE_22_REGADRf, 0x1);
                        WRITE_CMIC_MIIM_ADDRESSr(unit, rval);
                    }
                }
            }
        }
#endif
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return SOC_E_NONE;
}

int
soc_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    int rv;

    rv = SOC_E_UNAVAIL;

#if defined (BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
        rv = _soc_linkscan_hw_link_get(unit, hw_link);
    }
#endif /* BCM_ESW_SUPPORT */ 

    return rv;
}

int
soc_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link)
{
    int rv;
    rv = SOC_E_UNAVAIL;

#if defined (BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
#ifdef BCM_CMICX_SUPPORT
        if(soc_feature(unit, soc_feature_cmicx)) {
            rv = soc_cmicx_linkscan_hw_link_cache_get(unit, hw_link);
        }
#endif /* BCM_CMICX_SUPPORT */
    }
#endif /* BCM_ESW_SUPPORT */
    return rv;
}
