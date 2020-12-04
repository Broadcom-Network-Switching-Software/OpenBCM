/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The part of PCID that simulates the cmic
 *
 * Requires:
 *
 * Provides:
 *     soc_internal_pcic_init
 *     soc_internal_read
 *     soc_internal_write
 *     soc_internal_reset
 *     soc_internal_vlan_tab_init
 *     soc_internal_memory_fetch
 *     soc_internal_memory_store
 *     soc_internal_manual_hash
 *     soc_internal_vlan_tab_init
 *     soc_internal_manual_hash
 *     soc_internal_send_int
 *     soc_internal_table_dma
 *     soc_internal_schan_ctrl_write
 */

#include <shared/bsl.h>

#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>

#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#ifdef BCM_ISM_SUPPORT
#include "ism.h"
#endif

/*
 * PCI Config Space Simulation
 */

void
soc_internal_pcic_init(pcid_info_t *pcid_info, uint16 pci_dev_id,
                       uint16 pci_ven_id, uint8 pci_rev_id, uint32 pci_mbar0)
{
    /* PCI ID's translated at higher level */

    /* Device ID (16b), Vendor ID (16b) */
    PCIC(pcid_info, 0x00) = (pci_dev_id << 16) | pci_ven_id;

    /* Status (16b), Command (16b) */
    PCIC(pcid_info, 0x04) = 0x04800046;

    /* Class Code (24b), Rev ID (8b) */
    PCIC(pcid_info, 0x08) = 0x02800000 | pci_rev_id;

    /* BIST, Hdr Type, Lat Timer, Cache Line Size */
    PCIC(pcid_info, 0x0c) = 0x00002008;

    /* BAR0 */
    PCIC(pcid_info, 0x10) = pci_mbar0;

    /* BAR1 */
    PCIC(pcid_info, 0x14) = 0x00000000;

    /* BAR2 */
    PCIC(pcid_info, 0x18) = 0x00000000;

    /* BAR3 */
    PCIC(pcid_info, 0x1c) = 0x00000000;

    /* BAR4 */
    PCIC(pcid_info, 0x20) = 0x00000000;

    /* BAR5 */
    PCIC(pcid_info, 0x24) = 0x00000000;

    /* Cardbus CIS Pointer */
    PCIC(pcid_info, 0x28) = 0x00000000;

    /* Subsystem ID (16b), Subsystem Vendor ID (16b) */
    PCIC(pcid_info, 0x2c) = 0x00000000;

    /* Expansion ROM base addr */
    PCIC(pcid_info, 0x30) = 0x00000000;

    /* Reserved (24b), Capabilities Ptr (8b) */
    PCIC(pcid_info, 0x34) = 0x00000000;

    /* Reserved */
    PCIC(pcid_info, 0x38) = 0x00000000;

    /* Max_Lat, Min_Gnt, Int Pin, Int Line */
    PCIC(pcid_info, 0x3c) = 0x0000010b;

    /* Retry_Timeout, TRDY_Timeout */
    PCIC(pcid_info, 0x40) = 0x00008080;

    /* Retry_Timeout, TRDY_Timeout */
    PCIC(pcid_info, 0x44) = 0x00000000;
}

int
soc_internal_write_reg(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    soc_regaddrinfo_t ainfo;
    int rv;

    soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);

    if (!ainfo.valid || ainfo.reg < 0) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("Attempt to write to unknown S-channel address 0x%x\n"),
                  address));
    } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
        SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags &
               SOC_REG_FLAG_RO) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("Attempt to write to read-only S-channel address 0x%x\n"),
                  address));
    }

#if defined(BCM_FIREBOLT_SUPPORT)
    /*
    ** Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
    */
    if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
        int blk;
        int unit = pcid_info->unit;
        blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            if ((ainfo.reg == ICOS_SELr) ||
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASKr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASKr) ||
                (ainfo.reg == IBCAST_BLOCK_MASKr) ||
                (ainfo.reg == IEMIRROR_CONTROLr) ||
                (ainfo.reg == IMIRROR_CONTROLr) ||
#if defined(BCM_RAPTOR_SUPPORT) 
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IBCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IEMIRROR_CONTROL_HIr) ||
#endif /* BCM_RAPTOR_SUPPORT */
                0) {
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                address = (address & ~(0x00f00000)) | blk;
            }
        }
        blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
            address = (address & ~(0x00f00000)) | blk;
        }
    }
#endif    /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(pcid_info->unit)) {
        if (ainfo.reg == COMMAND_CONFIGr) {
            uint32 old_data[2];
            if (soc_internal_read_reg(pcid_info, address, old_data) >= 0) {
                if (soc_reg_field_get(pcid_info->unit, COMMAND_CONFIGr,
                                 data[0], SW_RESETf) == 0) {
                    /*
                     * Warn if any field is modified without UNIMAC in reset.
                     */
                    uint32  cc_rst;
                    cc_rst = data[0] ^ old_data[0];
                    soc_reg_field_set(pcid_info->unit, COMMAND_CONFIGr,
                                         &cc_rst, SW_RESETf, 0);
                    if (cc_rst) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                                 (BSL_META("UNIMAC: Attempt to Modify COMMAND_CONFIG without MAC in reset old = 0x%08X new = 0x%08X\n"), old_data[0], data[0]));
                    }
                }
            }
        }
    }
#endif    /* BCM_RAVEN_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
    if (soc_feature(pcid_info->unit, soc_feature_logical_port_num)) {
        soc_info_t *si;
        int phy_port;
        int blk;

        si = &SOC_INFO(pcid_info->unit);
        switch (ainfo.reg) {
        case EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr:
            phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg, data[0],
                                         PHYSICAL_PORT_NUMBERf);
            if (phy_port != 0x7f) {
                si->port_p2l_mapping[phy_port] = ainfo.port;
                si->port_l2p_mapping[ainfo.port] = phy_port;
                blk = SOC_PORT_BLOCK(pcid_info->unit, phy_port);
                si->port_type[ainfo.port] =
                    SOC_BLOCK_INFO(pcid_info->unit, blk).type;
                if (ainfo.port > si->port.max) {
                    si->port.max = ainfo.port;
                }
            }
            break;
        case MMU_TO_PHY_PORT_MAPPINGr:
            phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg, data[0],
                                         PHY_PORTf);
            if (phy_port != 0x7f) {
                si->port_p2m_mapping[phy_port] = ainfo.idx & 0x7f;
                si->port_m2p_mapping[ainfo.idx & 0x7f] = phy_port;
            }
            break;
        default:
            break;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    rv = soc_datum_reg_write(pcid_info, ainfo, 0, address, data);

    return rv;
}

int
soc_internal_read_reg(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    soc_regaddrinfo_t ainfo;
    int rv;

    soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);

    if (!ainfo.valid || ainfo.reg < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("Attempt to read from unknown S-channel address 0x%x\n"),
                   address));
    } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
        SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags & SOC_REG_FLAG_WO) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("Attempt to read from write-only S-channel address 0x%x\n"),
                  address));
    }

#if defined(BCM_FIREBOLT_SUPPORT)
    /*
    ** Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
    ** Well not really. Some of the registers are Higig only.
    */
    if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
        int blk;
        int unit = pcid_info->unit;
        blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            if ((ainfo.reg == ICOS_SELr) ||
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASKr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASKr) ||
                (ainfo.reg == IBCAST_BLOCK_MASKr) ||
                (ainfo.reg == IEMIRROR_CONTROLr) ||
                (ainfo.reg == IMIRROR_CONTROLr) ||
#if defined(BCM_RAPTOR_SUPPORT)
                (ainfo.reg == IUNKNOWN_UCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IUNKNOWN_MCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IBCAST_BLOCK_MASK_HIr) ||
                (ainfo.reg == IEMIRROR_CONTROL_HIr) ||
#endif /* BCM_RAPTOR_SUPPORT */
                0) {
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                address = (address & ~(0x00f00000)) | blk;
                soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);
            }
        }
        blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
        if (((address & 0x00f80000) == blk)
            && ((address & 0x0001f000) != (CMIC_PORT(unit) << 12)))    {
            blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
            address = (address & ~(0x00f00000)) | blk;
            soc_regaddrinfo_get(pcid_info->unit, &ainfo, address);
        }
    }
#endif  /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
|| defined(BCM_RAPTOR_SUPPORT)
    /*
    ** Patch over array registers with different reset values
    */
    if (SOC_IS_FIREBOLT2(pcid_info->unit) || SOC_IS_TRX(pcid_info->unit) ||
        SOC_IS_RAVEN(pcid_info->unit) || SOC_IS_HAWKEYE(pcid_info->unit)) {
        if (ainfo.reg == EGR_OUTER_TPIDr) {
            switch (address & 0x3) {
            case 0:
                ainfo.reg = EGR_OUTER_TPID_0r;
                break;
            case 1:
                ainfo.reg = EGR_OUTER_TPID_1r;
                break;
            case 2:
                ainfo.reg = EGR_OUTER_TPID_2r;
                break;
            case 3:
                ainfo.reg = EGR_OUTER_TPID_3r;
                break;
            /* coverity[dead_error_begin] */
            default:
                break;
            }
            ainfo.idx = -1;
        }
        if (ainfo.reg == ING_OUTER_TPIDr) {
            switch (address & 0x3) {
            case 0:
                ainfo.reg = ING_OUTER_TPID_0r;
                break;
            case 1:
                ainfo.reg = ING_OUTER_TPID_1r;
                break;
            case 2:
                ainfo.reg = ING_OUTER_TPID_2r;
                break;
            case 3:
                ainfo.reg = ING_OUTER_TPID_3r;
                break;
            /* coverity[dead_error_begin] */
            default:
                break;
            }
            ainfo.idx = -1;
        }
    }
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(pcid_info->unit)) {
        if (ainfo.reg == ING_MPLS_TPIDr) {
            switch (address & 0x3) {
            case 0:
                ainfo.reg = ING_MPLS_TPID_0r;
                break;
            case 1:
                ainfo.reg = ING_MPLS_TPID_1r;
                break;
            case 2:
                ainfo.reg = ING_MPLS_TPID_2r;
                break;
            case 3:
                ainfo.reg = ING_MPLS_TPID_3r;
                break;
             /* coverity[dead_error_begin] */
            default:
                break;
            }
            ainfo.idx = -1;
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#endif  /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT
           || BCM_RAPTOR_SUPPORT */
    rv = soc_datum_reg_read(pcid_info, ainfo, 0, address, data);

    if (SOC_E_NONE == rv) {
        /* Chip specific register value patches */
#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_TRX(pcid_info->unit)) {
            /*
             * Reading ING_HW_RESET_CONTROL_2/EGR_HW_RESET_CONTROL_1
             * returns DONE=1
             */
            if (SOC_IS_TD_TT(pcid_info->unit)) {
                if (address == 0x180003) {  /* ING_HW_RESET_CONTROL_2 */
                    data[0] |= (1 << 18);
                }
                if (address == 0x280001) {  /* EGR_HW_RESET_CONTROL_1 */
                    data[0] |= (1 << 18);
                }
            } else if (SOC_IS_TRIUMPH2(pcid_info->unit) ||
                       SOC_IS_APOLLO(pcid_info->unit)) {
                if (address == 0x080003) {  /* ING_HW_RESET_CONTROL_2 */
                    data[0] |= (1 << 18);
                }
                if (address == 0x180001) {  /* EGR_HW_RESET_CONTROL_1 */
                    data[0] |= (1 << 18);
                }
            } else {
                if (address == 0xa80003) {  /* ING_HW_RESET_CONTROL_2 */
                    data[0] |= (1 << 18);
                }
                if (address == 0xb80001) {  /* EGR_HW_RESET_CONTROL_1 */
                    data[0] |= (1 << 18);
                }
            }
        }
        else if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
            /*
             * Reading ING_HW_RESET_CONTROL_2/EGR_HW_RESET_CONTROL_1
             * returns DONE=1
             */
            if (address == 0x780002) {      /* ING_HW_RESET_CONTROL_2 */
                    data[0] |= (1 << 18);
            }
            if (address == 0x980001) {      /* EGR_HW_RESET_CONTROL_1 */
                    data[0] |= (1 << 18);
            }
        }
        else if (SOC_IS_HBX(pcid_info->unit)) {
            /*
             * Reading ING_HW_RESET_CONTROL_2/EGR_HW_RESET_CONTROL_1
             * returns DONE=1
             */
            if (address == 0x180203) {      /* ING_HW_RESET_CONTROL_2_X */
                    data[0] |= (1 << 18);
            }
            if (address == 0x180403) {      /* ING_HW_RESET_CONTROL_2_Y */
                    data[0] |= (1 << 18);
            }
            if (address == 0x180603) {      /* ING_HW_RESET_CONTROL_2 */
                    data[0] |= (1 << 18);
            }
            if (address == 0x480601) {      /* EGR_HW_RESET_CONTROL_1 */
                    data[0] |= (1 << 18);
            }
        }
#endif  /* BCM_FIREBOLT_SUPPORT */
        return 0;
    } else if (SOC_E_NOT_FOUND == rv) {

    /*
         * The register is not in the hash table, so it has not been written
         * before.  Provide a default value.
         */
        if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
            (ainfo.valid && ainfo.reg >= 0)) {
            /*This macro handle both 64 and above_64 regs*/
            SOC_REG_ABOVE_64_RST_VAL_GET(pcid_info->unit, ainfo.reg, data);
        } else {
            data[0] = 0xffffffff;
            data[1] = 0xffffffff;
        }

        rv = SOC_E_NONE;

#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT)
        if (SOC_IS_FX_HX(pcid_info->unit)) {
            if (address == 0xb80210) {  /* BSAFE_GLB_DEV_STATUS */
                data[0] |= (1 << 7);    /* PROD_CFG_VLD */
            }
            if (address == 0xb8020c) {  /* BSAFE_GLB_PROD_CFG */
                data[0] |= (pcid_info->opt_gbp_wid << 8); /* LMD_ENABLE */
            }
        }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT */
#ifdef  BCM_HERCULES_SUPPORT
        if (SOC_IS_HERCULES15(pcid_info->unit) &&
            (address & 0xff0fffff) == 0x00000202) {
            /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
            data[0] |= 0x00000008;
        }
#endif  /* BCM_HERCULES_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_FB(pcid_info->unit) ||
            SOC_IS_HELIX(pcid_info->unit) ||
            SOC_IS_FELIX(pcid_info->unit)) {
            if (address == 0x00200002 ||        /* MAC_XGXS_STAT.XE0 */
                address == 0x00300002 ||        /* MAC_XGXS_STAT.XE1 */
                address == 0x00400002 ||        /* MAC_XGXS_STAT.XE2 */
                address == 0x00500002) {        /* MAX_XGXS_STAT_XE3 */
                /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
                data[0] |= 0x00000008;
            }
        }
#endif  /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(pcid_info->unit)) {
            if ((address & 0xfff00fff) == 0x000002) { /* MAX_XGXS_STAT */
                /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
                data[0] |= 0x00000020;
            }
        }
#endif  /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TR_VL(pcid_info->unit)) {
            if (address == 0x00200002 || /* MAC_XGXS_STAT.XE0 */
                address == 0x00300002 || /* MAC_XGXS_STAT.XE1 */
                address == 0x00400002 || /* MAC_XGXS_STAT.XE2 */
                address == 0x00500002 || /* MAC_XGXS_STAT.XE3 */
                address == 0x00600002 || /* MAC_XGXS_STAT.XE4 */
                address == 0x00700002 || /* MAC_XGXS_STAT.XE5 */
                address == 0x00800002 || /* MAC_XGXS_STAT.XE6 */
                address == 0x00900002) { /* MAX_XGXS_STAT.XE7 */
                /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
                data[0] |= 0x00000020;
            }
        }
#endif  /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(pcid_info->unit) || SOC_IS_APOLLO(pcid_info->unit) ||
            SOC_IS_VALKYRIE2(pcid_info->unit)) {
            if (address == 0x800002 || /* MAC_XGXS_STAT.XP0 */
                address == 0x900002 || /* MAC_XGXS_STAT.XP1 */
                address == 0xa00002 || /* MAC_XGXS_STAT.XP2 */
                address == 0xb00002 || /* MAC_XGXS_STAT.XP3 */
                address == 0x40000002 || /* MAC_XGXS_STAT.XQ0 */
                address == 0x40100002 || /* MAC_XGXS_STAT.XQ1 */
                address == 0x40200002 || /* MAC_XGXS_STAT.XQ2 */
                address == 0x40300002 || /* MAC_XGXS_STAT.XQ3 */
                address == 0x40400002 || /* MAC_XGXS_STAT.XQ4 */
                address == 0x40500002) { /* MAX_XGXS_STAT.XQ5 */
                /* Reading MAC_XGXS_STAT returns PLL_LOCK=1 */
                data[0] |= 0x00000020;
            }
        }
#endif  /* BCM_TRIUMPH2_SUPPORT */
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("RPC error: soc_internalreg_read failed %d. \n"), rv));
    }

    return rv;
}

int
soc_internal_write_bytes(pcid_info_t *pcid_info, uint32 address, uint8 *data, 
                         int len)
{
    uint32  temp[SOC_MAX_MEM_WORDS];

    if ((len < 0) || (len > (SOC_MAX_MEM_WORDS * (sizeof(uint32))))) {
        return 1;
    }

    /* writing bytes is supported only for bcmsim */
    if ((pcid_info->regmem_cb) && 
        (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_WRITE, address, 
                              (uint32 *)data, (len * 8)) == 0)) {
        return 0;
    }

    /* if no registered call-back, default to write_mem */
    sal_memset(temp, 0, (SOC_MAX_MEM_WORDS * sizeof(uint32)));
    sal_memcpy(temp, data, len);
    return soc_internal_write_mem(pcid_info, address, temp);

}

int
soc_internal_write_mem(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    soc_mem_t       mem;
    int rv;

    if ((pcid_info->regmem_cb) 
        && (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_WRITE, address, data, 
                                 ((SOC_MAX_MEM_WORDS * 4) * 8)) == 0)) {
        return 0;
    }

    if ((mem = soc_addr_to_mem(pcid_info->unit, address, NULL)) != INVALIDm) {
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
                /*
                 * Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
                 */
                int blk;
                int unit = pcid_info->unit;
                int cpu_port = CMIC_PORT(unit);
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port )
                    && (mem != IFP_PORT_FIELD_SELm))    {
                    blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
                blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port))    {
                    blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
            }

            switch(mem) {
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    if(SOC_IS_HAWKEYE(pcid_info->unit)){
                        break;
                    }
                    return (soc_internal_l3x2_write(pcid_info, mem,
                                                    address, data));
                default: break;
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(pcid_info->unit)) {
            if (mem == VLAN_MACm) {
                /* In real HW, VLAN_MAC table is overlaid on VLAN_XLATE table */
                return (soc_internal_vlan_xlate_entry_write(pcid_info, address,
                                       (vlan_xlate_entry_t *) data));
            }
        }
#endif

        if  (soc_mem_is_aggr(pcid_info->unit, mem)) {
            return soc_internal_write_aggrmem(pcid_info, address, data);
        }
    }

    rv = soc_datum_mem_write(pcid_info, 0, address, data);
    return rv;
}

int
soc_internal_read_bytes(pcid_info_t *pcid_info, uint32 address, uint8 *data,
                        int len)
{
    uint32  temp[SOC_MAX_MEM_WORDS];
    int     rv;

    if ((len < 0) || (len > (SOC_MAX_MEM_WORDS * (sizeof(uint32))))) {
        return 1;
    }

    /* reading bytes is supported only for bcmsim */
    if ((pcid_info->regmem_cb) && 
        (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_READ, address, 
                              (uint32 *) data, (len * 8)) == 0)) {
        return 0;
    }

    /* if no registered call-back, default to read_mem */
    sal_memset(temp, 0, (SOC_MAX_MEM_WORDS * sizeof(uint32)));
    rv = soc_internal_read_mem(pcid_info, address, temp);
    if (!rv) {
        sal_memcpy(data, temp, len);
    }
    return rv;
}
int
soc_internal_read_mem(pcid_info_t *pcid_info, uint32 address, uint32 *data)
{
    int             i, rv;
    soc_mem_t       mem;

    if ((pcid_info->regmem_cb) 
        && (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_READ, address, data, 
                                 ((SOC_MAX_MEM_WORDS * 4) * 8)) == 0)) {
        return 0;
    }

    if ((mem = soc_addr_to_mem(pcid_info->unit, address, NULL)) != INVALIDm) {
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            if (SOC_IS_FB_FX_HX(pcid_info->unit)) {
                /*
                 * Map EPIPE_HI/IPIPE_HI port registers to EPIPE/IPIPE block
                 */
                int blk;
                int unit = pcid_info->unit;
                int cpu_port = CMIC_PORT(unit);
                blk = SOC_BLOCK2OFFSET(unit, IPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port )
                    && (mem != IFP_PORT_FIELD_SELm))    {
                    blk = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
                blk = SOC_BLOCK2OFFSET(unit, EPIPE_HI_BLOCK(unit)) << 20;
                if (((address & 0x00f00000) == blk)
                    && ((address & 0x0000001f) != cpu_port))    {
                    blk = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit)) << 20;
                    address = (address & ~(0x00f00000)) | blk;
                }
            }

            switch(mem) {
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    if(SOC_IS_HAWKEYE(pcid_info->unit)){
                        break;
                    }
                    return (soc_internal_l3x2_read(pcid_info, mem,
                                                   address, data));
                default: break;
            }
        }
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(pcid_info->unit)) {
            if (mem == VLAN_MACm) {
                /* In real HW, VLAN_MAC table is overlaid on VLAN_XLATE table */
                return (soc_internal_vlan_xlate_entry_read(pcid_info, address,
                                       (vlan_xlate_entry_t *) data));
            }
        }
#endif
        if  (soc_mem_is_aggr(pcid_info->unit, mem)) {
            return soc_internal_read_aggrmem(pcid_info, address, data);
        }
    }

    rv = soc_datum_mem_read(pcid_info, 0, address, data);

    if (SOC_E_NOT_FOUND == rv) {
        /* Return null entry value if recognize, otherwise 0xf's */
        if ((mem = soc_addr_to_mem(pcid_info->unit, address, NULL)) != INVALIDm) {
            if((NULL != soc_mem_entry_null(pcid_info->unit, mem))) {
                memcpy(data,               /* Memory */
                       soc_mem_entry_null(pcid_info->unit, mem),
                       4 * soc_mem_entry_words(pcid_info->unit, mem));
            } else {
                memset(data, 0, 4 * soc_mem_entry_words(pcid_info->unit, mem));
            }
        } else {                   /* Other */
            for (i = 0; i < SOC_MAX_MEM_WORDS; i++) {
                data[i] = 0xffffffff;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Re-sort fields of aggregate mems based on bit position
 */
void
_soc_internal_fields_sort(soc_field_info_t *src, uint32 count, soc_field_info_t *dst)
{
    uint32 i, j;
    soc_field_info_t tmp;
    
    sal_memcpy(dst, src, count * sizeof(soc_field_info_t));
    for (i = 0; i < count; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (dst[j].bp < dst[j+1].bp) {
                sal_memcpy(&tmp, &dst[j], 
                           sizeof(soc_field_info_t));
                sal_memcpy(&dst[j], &dst[j+1],
                           sizeof(soc_field_info_t));
                sal_memcpy(&dst[j+1], &tmp,
                           sizeof(soc_field_info_t));
            }
        }
    }    
}

/*
** This is a generic function to handle aggregate memories. Memories
** made up of unequal depth and field mismatch between unified view
** and individual component memories require special handling.
** For example L2 table has HIT bits which are packet into 1/8 size table
*/
int
soc_internal_read_aggrmem(pcid_info_t *pcid_info, uint32 addr, uint32 *data)
{
    int         unit = pcid_info->unit;
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    soc_field_info_t *asflds; /* array for aggregate mem sorted fields */
    soc_field_info_t *sflds; /* array for partial mem sorted fields */

    mem = soc_addr_to_mem(pcid_info->unit, addr, NULL);

    if (mem == INVALIDm)
    {
        return SOC_E_PARAM;
    }

    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;
    index = 0;
    memset(data, 0, 4 * soc_mem_entry_words(unit, mem));
    do {
        m = SOC_MEM_AGGR(unit, index);
        if (m[0] == mem) {
            /* Fetch the individual components */
            nfield = 1;
            for(i = 1; m[i] != INVALIDm; i++) {
                /*
                ** Multiple entries compacted into one entry like HIT
                ** requires special handling.
                */
                amemp = &SOC_MEM_INFO(unit, mem);
                if(!soc_mem_index_max(unit, m[i]) || !soc_mem_index_max(unit, mem)) {
                    continue;
                }
                pck_factor = (soc_mem_index_max(unit, mem) /
                              soc_mem_index_max(unit, m[i]));
                assert(pck_factor != 0);
                adj_offset = offset / pck_factor;
                soc_internal_read_mem(pcid_info,
                        soc_mem_addr(unit, m[i], 0,
                                SOC_MEM_BLOCK_ANY(unit, m[i]),
                                adj_offset), ent);

                /* Iterate thru' the fields in this View */
                memp = &SOC_MEM_INFO(unit, m[i]);
                if (pck_factor == 1) {
                    for (f = memp->nFields - 1; f >= 0; f--) {
                        fieldp = &memp->fields[f];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        if (soc_mem_field_valid(unit, mem, fieldp->field) &&
                            soc_mem_field_valid(unit, m[i], fieldp->field)) {
                            soc_mem_field_set(unit, mem, data, fieldp->field,
                                              fv);
                        }
                        nfield++;
                    }
                } else if (memp->nFields == pck_factor) {
                        asflds = sal_alloc(amemp->nFields * sizeof(soc_field_info_t), 
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields * sizeof(soc_field_info_t), 
                                           "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields, amemp->nFields,
                                                  asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &sflds[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds); sal_free(sflds);
                } else if (memp->nFields == (pck_factor * 2)) {
                        asflds = sal_alloc(amemp->nFields * sizeof(soc_field_info_t), 
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields * sizeof(soc_field_info_t), 
                                           "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields, amemp->nFields,
                                                  asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &sflds[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;

                        fieldp = &sflds[memp->nFields - pck_factor - 
                                    (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds); sal_free(sflds);
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_internal_read_aggrmem:mem=%d"
                                          "Unable to handle Aggregate Mem component %d\n"),
                               mem, m[i]));
                }
            }
            return(0);
        }
        index++;
    } while (m[0] != INVALIDm);

    /* Not found in the table of aggregate memories */
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_internal_read_aggrmem:mem=%d missing in Aggregate Mem List\n"),
               mem));
    if((NULL != soc_mem_entry_null(pcid_info->unit, mem))) {
        memcpy(data,               /* Memory */
               soc_mem_entry_null(pcid_info->unit, mem),
               4 * soc_mem_entry_words(pcid_info->unit, mem));
    } else {
        memset(data, 0, 4 * soc_mem_entry_words(pcid_info->unit, mem));
    }           
    return(0);
}

int
soc_internal_write_aggrmem(pcid_info_t *pcid_info, uint32 addr, uint32 *data)
{
    int         unit = pcid_info->unit;
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    soc_field_info_t *asflds; /* array for aggregate mem sorted fields */
    soc_field_info_t *sflds; /* array for partial mem sorted fields */

    mem = soc_addr_to_mem(pcid_info->unit, addr, NULL);

    if (mem == INVALIDm)
    {
        return SOC_E_PARAM;
    }

    memset(ent, 0, 4 * soc_mem_entry_words(unit, mem));
    offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0xfffff;
    index = 0;
    do {
        m = SOC_MEM_AGGR(unit, index);
        if (m[0] == mem) {
            /* Fetch the individual components */
            nfield = 1;
            for(i = 1; m[i] != INVALIDm; i++) {
                /*
                ** Multiple entries compacted into one entry like HIT
                ** requires special handling.
                */
                amemp = &SOC_MEM_INFO(unit, mem);
                if(!soc_mem_index_max(unit, m[i]) || !soc_mem_index_max(unit, mem)) {
                    continue;
                }
                pck_factor = (soc_mem_index_max(unit, mem) /
                              soc_mem_index_max(unit, m[i]));
                assert(pck_factor != 0);
                adj_offset = offset / pck_factor;
                memp = &SOC_MEM_INFO(unit, m[i]);
                
                /* Iterate thru' the fields in this View */
                if (pck_factor == 1) {
                    for (f = memp->nFields - 1; f >= 0; f--) {
                        fieldp = &memp->fields[f];
                        if (soc_mem_field_valid(unit, mem, fieldp->field) &&
                            soc_mem_field_valid(unit, m[i], fieldp->field)) {
                            soc_mem_field_get(unit, mem, data, fieldp->field,
                                              fv);
                            soc_mem_field_set(unit, m[i], ent, fieldp->field,
                                              fv);
                        }
                        nfield++;
                    }
                } else if (memp->nFields == pck_factor) {
                        asflds = sal_alloc(amemp->nFields * sizeof(soc_field_info_t), 
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields * sizeof(soc_field_info_t), 
                                           "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields, amemp->nFields,
                                                  asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        fieldp = &sflds[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_internal_read_mem(pcid_info,
                                soc_mem_addr(unit, m[i], 0,
                                        SOC_MEM_BLOCK_ANY(unit, m[i]),
                                        adj_offset), ent);
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds); sal_free(sflds);
                } else if (memp->nFields == (pck_factor * 2)) {
                        asflds = sal_alloc(amemp->nFields * sizeof(soc_field_info_t), 
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields * sizeof(soc_field_info_t), 
                                           "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields, amemp->nFields,
                                                  asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        fieldp = &sflds[memp->nFields -
                                    (offset % pck_factor) - 1];
                        soc_internal_read_mem(pcid_info,
                                soc_mem_addr(unit, m[i], 0,
                                        SOC_MEM_BLOCK_ANY(unit, m[i]),
                                        adj_offset), ent);
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;

                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        fieldp = &sflds[memp->nFields - pck_factor -
                                    (offset % pck_factor) - 1];
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds); sal_free(sflds);
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_internal_write_aggrmem:mem=%d"
                                          "Unable to handle Aggregate Mem component %d\n"),
                               mem, m[i]));
                }
                soc_internal_write_mem(pcid_info,
                                soc_mem_addr(unit, m[i], 0,
                                    SOC_MEM_BLOCK_ANY(unit, m[i]),
                                    adj_offset), ent);
            }
            return(0);
        }
        index++;
    } while (m[0] != INVALIDm);

    /* Not found in the table of aggregate memories */
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_internal_write_aggrmem:mem=%d missing in Aggregate Mem List\n"),
               mem));
    return(0);
}

#ifdef    BCM_FIREBOLT_SUPPORT
void
_soc_l2x2_nak_check(int unit, schan_msg_t    *msg)
{
    int             op_fail_pos;

    /*
     * ==================================================
     * | MFIFO_FULL | OP_FAIL | Index  | L2x entry data |
     * ==================================================
     */
    op_fail_pos = (_shr_popcount(soc_mem_index_max(unit, L2Xm)) +
                   soc_mem_entry_bits(unit, L2Xm)) % 32;

    if (msg->readresp.data[3] & (0x3 << op_fail_pos)) {
        msg->readresp.header.v2.nack = 1;
    }
}
#endif    /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_TRX_SUPPORT
static int ing_ipfix_fifo_count[SOC_MAX_NUM_DEVICES] = { 0 };
static int egr_ipfix_fifo_count[SOC_MAX_NUM_DEVICES] = { 0 };
static int ext_l2mod_fifo_count[SOC_MAX_NUM_DEVICES] = { 0 };
static int l2mod_fifo_count[SOC_MAX_NUM_DEVICES]     = { 0 };

/*
 * soc_internal_ing_ipfix_fifo_pop
 */
static int
soc_internal_fifo_pop(pcid_info_t * pcid_info, soc_mem_t mem, uint32 *result)
{
    int      *count, i;
    uint32   tmp[SOC_MAX_MEM_WORDS];
    int      unit = pcid_info->unit;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Ingress IPFIX fifo pop\n")));

    *result = 0;
    switch (mem) {
        case ING_IPFIX_EXPORT_FIFOm:
            count = &ing_ipfix_fifo_count[unit];
            break;
        case EGR_IPFIX_EXPORT_FIFOm:
            count = &egr_ipfix_fifo_count[unit];
            break;
        case EXT_L2_MOD_FIFOm:
            count = &ext_l2mod_fifo_count[unit];
            break;
        case L2_MOD_FIFOm:
            count = &l2mod_fifo_count[unit];
            break;
        default:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Pop not supported on this memory\n")));
/*             PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST; */
            pcid_reg_or_write(pcid_info, CMIC_SCHAN_CTRL, SC_MSG_NAK_TST);
            return 0;
    }

    if (*count > 0) {
        soc_internal_read_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                         SOC_MEM_BLOCK_ANY(unit, mem),
                                         0), result);

        /* Shift up the remaining entries */
        for (i = 0; i < *count; i++) {
            soc_internal_read_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                             SOC_MEM_BLOCK_ANY(unit, mem),
                                             i + 1), tmp);
            soc_internal_write_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                              SOC_MEM_BLOCK_ANY(unit, mem),
                                              i), tmp);
        }
        *count -= 1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Fifo entry popped.\n")));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Fifo Empty\n")));
        return 1;
    }
    return 0;
}

/*
 * soc_internal_ing_ipfix_fifo_push
 */
static int
soc_internal_fifo_push(pcid_info_t * pcid_info, soc_mem_t mem, uint32 *entry)
{
    int       *count, i;
    uint32    tmp[SOC_MAX_MEM_WORDS];
    int       unit = pcid_info->unit;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Ingress IPFIX fifo push\n")));

    switch (mem) {
        case ING_IPFIX_EXPORT_FIFOm:
            count = &ing_ipfix_fifo_count[unit];
            break;
        case EGR_IPFIX_EXPORT_FIFOm:
            count = &egr_ipfix_fifo_count[unit];
            break;
        case EXT_L2_MOD_FIFOm:
            count = &ext_l2mod_fifo_count[unit];
            break;
        case L2_MOD_FIFOm:
            count = &l2mod_fifo_count[unit];
            break;
        default:
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Pop not supported on this memory\n")));
/*            PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_NAK_TST; */
            pcid_reg_or_write(pcid_info, CMIC_SCHAN_CTRL, SC_MSG_NAK_TST);
            return 0;
    }

    if (*count == soc_mem_index_count(unit, mem)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Fifo Full\n")));
        return 1;
    } else {
        /* Shift down the existing entries */
        for (i = *count; i > 0; i--) {
            soc_internal_read_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                             SOC_MEM_BLOCK_ANY(unit, mem),
                                             i - 1), tmp);
            soc_internal_write_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                              SOC_MEM_BLOCK_ANY(unit, mem),
                                              i), tmp);
        }
        soc_internal_write_mem(pcid_info, soc_mem_addr(unit, mem, 0,
                                          SOC_MEM_BLOCK_ANY(unit, mem),
                                          0), entry);
        *count += 1;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Fifo entry pushed.\n")));
    }
    return 0;
}

#endif /* BCM_TRX_SUPPORT */

/*
 * schan_op
 *
 * Look at schan header, perform action(s), and make response.
 */

int
schan_op(pcid_info_t *pcid_info, int unit, schan_msg_t* data)
{
    schan_msg_t    *msg;
    int             opcode, dst_blk, src_blk, acc_type, data_byte_len, dma;
    uint32          bank_ignore_mask;
    int             rv = -1;
#ifdef    BCM_TRX_SUPPORT
    int32           banks = 0;
#endif
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    soc_mem_t mem;
#endif

#ifdef BCM_CMICM_SUPPORT
    if (pcid_info->cmicm >= CMC0) {
        msg = (schan_msg_t *)&PCIM(pcid_info, CMIC_CMCx_SCHAN_MESSAGEn(CMC0, 0));
    } else 
#endif
    {
        msg = (schan_msg_t *)&PCIM(pcid_info, CMIC_SCHAN_MESSAGE(unit, 0));
    }

    if(data) {
        *msg = *data; 
    }

    /* Clear nak bit in schan control */
#ifdef BCM_CMICM_SUPPORT
    if (pcid_info->cmicm >= CMC0) {
/*         PCIM(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET) &= ~SC_CMCx_MSG_NAK; */
       pcid_reg_and_write(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET, SC_CMCx_MSG_NAK);
       
    } else 
#endif
    {
/*        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~SC_MSG_NAK_TST; */
        pcid_reg_and_write(pcid_info, CMIC_SCHAN_CTRL, SC_MSG_NAK_TST);
    }

    soc_schan_header_cmd_get(pcid_info->unit, &msg->header, &opcode,
                             &dst_blk, &src_blk, &acc_type, &data_byte_len,
                             &dma, &bank_ignore_mask);
    switch (opcode) {
    case WRITE_REGISTER_CMD_MSG:
       if (pcid_info->opt_pli_verbose)
          LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "S-Channel operation: WRITE_REGISTER_CMD 0x%x\n"),
                       msg->writecmd.address));
        opcode = WRITE_REGISTER_ACK_MSG;
        rv = soc_internal_extended_write_reg(pcid_info, dst_blk, acc_type,
                                             msg->writecmd.address,
                                             msg->writecmd.data); 
        break;
    case READ_REGISTER_CMD_MSG:
       if (pcid_info->opt_pli_verbose)
          LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "S-Channel operation: READ_REGISTER_CMD 0x%x\n"),
                       msg->readcmd.address));
        opcode = READ_REGISTER_ACK_MSG;
        rv = soc_internal_extended_read_reg(pcid_info, dst_blk, acc_type,
                                            msg->readcmd.address,
                                            msg->readresp.data);
        break;
    case WRITE_MEMORY_CMD_MSG:
       if (pcid_info->opt_pli_verbose)
          LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "S-Channel operation: WRITE_MEMORY_CMD 0x%x\n"),
                       msg->writecmd.address));
        opcode = WRITE_MEMORY_ACK_MSG;
        rv = soc_internal_extended_write_mem(pcid_info, dst_blk, acc_type,
                                             msg->writecmd.address,
                                             msg->writecmd.data); 
        break;
    case READ_MEMORY_CMD_MSG:
       if (pcid_info->opt_pli_verbose)
          LOG_VERBOSE(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "S-Channel operation: READ_MEMORY_CMD 0x%x\n"),
                       msg->readcmd.address));
        opcode = READ_MEMORY_ACK_MSG;
        rv = soc_internal_extended_read_mem(pcid_info, dst_blk, acc_type,
                                            msg->readcmd.address,
                                            msg->readresp.data);
        break;
    case ARL_INSERT_CMD_MSG:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "S-Channel operation: ARL_INSERT_CMD_MSG\n")));
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = ARL_INSERT_DONE_MSG;
            rv = soc_internal_l2x2_entry_ins(pcid_info, bank_ignore_mask,
                                             (l2x_entry_t *)msg->l2x2.data,
                                             msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
#endif    /* BCM_FIREBOLT_SUPPORT */
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Bad call for Hercules: ARL_INSERT_CMD_MSG\n")));
            break;
        }
        break;
    case ARL_DELETE_CMD_MSG:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "S-Channel operation: ARL_DELETE_CMD_MSG\n")));
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = ARL_DELETE_DONE_MSG;
            rv = soc_internal_l2x2_entry_del(pcid_info, bank_ignore_mask,
                                             (l2x_entry_t *)msg->l2x2.data,
                                             msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
#endif    /* BCM_FIREBOLT_SUPPORT */
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Bad call for Hercules: ARL_DELETE_CMD_MSG\n")));
            break;
        }
        break;
    case L2_LOOKUP_CMD_MSG:
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = L2_LOOKUP_ACK_MSG;
            rv = soc_internal_l2x2_entry_lkup(pcid_info, bank_ignore_mask,
                                              (l2x_entry_t *)msg->l2x2.data,
                                              msg->readresp.data);
            _soc_l2x2_nak_check(pcid_info->unit, msg);
            break;
        }
        break;
#endif    /* BCM_FIREBOLT_SUPPORT */
    case ARL_LOOKUP_CMD_MSG:
        if (SOC_IS_XGS_FABRIC(pcid_info->unit)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "Bad call for Hercules: ARL_LOOKUP_CMD_MSG\n")));
            break;
        }
        break;
    case L3_INSERT_CMD_MSG:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "S-Channel operation: L3_INSERT_CMD_MSG\n")));
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = L3_INSERT_DONE_MSG;
            /* Coverity
             * This is being called under #ifdef FIREBOLT
             * The coverity is reporting error
             * from the called function in case of TRIDENT2
             * which will not be called
 */
            /* coverity[overrun-buffer-val] */
            rv = soc_internal_l3x2_entry_ins(pcid_info, bank_ignore_mask,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data,
                          msg->readresp.data);
            break;
        }
#endif    /* BCM_FIREBOLT_SUPPORT */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Bad call for Non-XGS3: L3_INSERT_CMD_MSG\n")));
        rv = -1;
        break;
    case L3_DELETE_CMD_MSG:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "S-Channel operation: L3_DELETE_CMD_MSG\n")));
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = L3_DELETE_DONE_MSG;
            rv = soc_internal_l3x2_entry_del(pcid_info, bank_ignore_mask,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data,
                          msg->readresp.data);
            break;
        }
#endif    /* BCM_FIREBOLT_SUPPORT */
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "Bad call for Non-XGS3: L3_DELETE_CMD_MSG\n")));
        rv = -1;
        break;
    case L3X2_LOOKUP_CMD_MSG:
#ifdef    BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(pcid_info->unit)) {
            opcode = L3X2_LOOKUP_ACK_MSG;
            rv = soc_internal_l3x2_entry_lkup(pcid_info, bank_ignore_mask,
                          (l3_entry_ipv6_multicast_entry_t *)msg->l3x2.data,
                          msg->readresp.data);
            if (msg->readresp.data[0] == -1) {
                msg->readresp.header.v2.nack = 1;
            }
            break;
        }
        break;
#endif    /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
    case TABLE_INSERT_CMD_MSG:
        if (soc_feature(unit, soc_feature_generic_table_ops)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "S-Channel operation: TABLE_INSERT_CMD_MSG\n")));

            opcode = TABLE_INSERT_DONE_MSG;
            banks = bank_ignore_mask;
            if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
                uint32 address;
                address = msg->gencmd.address;
                if (soc_feature(unit, soc_feature_ism_memory)) {
                    address &= ~SOC_HASH_BANK_MASK_ISM;
                }
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    address &= ~SOC_HASH_BANK_MASK_SHARED;
                }
                mem = soc_addr_to_mem_extended(pcid_info->unit, dst_blk,
                                               acc_type, address);
            } else {
                mem = soc_addr_to_mem(pcid_info->unit, msg->gencmd.address, NULL);
            } 
#ifdef BCM_ISM_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_ISM;
                if (!banks) {
                    banks = SOC_MEM_HASH_BANK_ALL;
                } else {
                    banks = ~banks & SOC_HASH_BANK_MASK_ISM;
                }
                rv = soc_internal_generic_hash_insert(pcid_info, mem, banks,
                                                      msg->gencmd.data,
                                                      (uint32 *)&msg->genresp.response);
            } else
#endif
            if (mem == L2Xm) {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                /* coverity[overrun-buffer-val] */
                rv = soc_internal_l2_tr_entry_ins(pcid_info, banks,
                                          (l2x_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if ((mem == VLAN_MACm) || (mem == VLAN_XLATEm)) {
                rv = soc_internal_vlan_xlate_entry_ins(pcid_info, banks,
                                          (vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VLAN_XLATEm) {
                rv = soc_internal_egr_vlan_xlate_entry_ins(pcid_info, banks,
                                          (egr_vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#ifdef BCM_TRIUMPH_SUPPORT
            } else if ((SOC_IS_TOMAHAWK3(unit) && mem == MPLS_ENTRY_SINGLEm) || mem == MPLS_ENTRYm) {
                rv = soc_internal_mpls_entry_ins(pcid_info, banks,
                                          (mpls_entry_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            } else if (mem == ING_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_ing_vp_vlan_member_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_egr_vp_vlan_member_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ING_DNAT_ADDRESS_TYPEm) {
                rv = soc_internal_ing_dnat_address_type_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == L2_ENDPOINT_IDm) {
                rv = soc_internal_l2_endpoint_id_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ENDPOINT_QUEUE_MAPm) {
                rv = soc_internal_endpoint_queue_map_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            } else if (mem == EXACT_MATCH_2m || mem == EXACT_MATCH_4m) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                rv = soc_internal_exact_match_ins
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
            } else if (mem == L3_TUNNEL_SINGLEm ||
                mem == L3_TUNNEL_DOUBLEm || mem == L3_TUNNEL_QUADm) {
                rv = soc_internal_tunnel_entry_ins(pcid_info,
                                 banks,
                                 (void *)msg->gencmd.data,
                                 (uint32 *)&msg->genresp.response);
#endif /* BCM_TOMAHAWK3_SUPPORT */
            } else {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                /* coverity[overrun-buffer-val] */
                rv = soc_internal_l3x2_entry_ins(pcid_info, banks,
                              (l3_entry_ipv6_multicast_entry_t *)msg->gencmd.data,
                              (uint32 *)&msg->genresp.response);
            }
            if (soc_feature(unit, soc_feature_new_sbus_format) &&
		        !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                if ((msg->genresp_v2.response.type != SCHAN_GEN_RESP_TYPE_INSERTED) &&
                    (msg->genresp_v2.response.type != SCHAN_GEN_RESP_TYPE_REPLACED)) {
                    msg->genresp_v2.header.v2.nack = 1;
                }
            } else {
                if ((msg->genresp.response.type != SCHAN_GEN_RESP_TYPE_INSERTED) &&
                    (msg->genresp.response.type != SCHAN_GEN_RESP_TYPE_REPLACED)) {
                    msg->genresp.header.v2.nack = 1;
                }
            }
            break;
        }
    case TABLE_DELETE_CMD_MSG:
        if (soc_feature(unit, soc_feature_generic_table_ops)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "S-Channel operation: TABLE_DELETE_CMD_MSG\n")));

            opcode = TABLE_DELETE_DONE_MSG;
            banks = bank_ignore_mask;
            if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
                uint32 address;
                address = msg->gencmd.address;
                if (soc_feature(unit, soc_feature_ism_memory)) {
                    address &= ~SOC_HASH_BANK_MASK_ISM;
                }
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    address &= ~SOC_HASH_BANK_MASK_SHARED;
                }
                mem = soc_addr_to_mem_extended(pcid_info->unit, dst_blk,
                                               acc_type, address);
            } else {
                mem = soc_addr_to_mem(pcid_info->unit, msg->gencmd.address, NULL);
            } 
#ifdef BCM_ISM_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_ISM;
                if (!banks) {
                    banks = SOC_MEM_HASH_BANK_ALL;
                } else {
                    banks = ~banks & SOC_HASH_BANK_MASK_ISM;
                }
                rv = soc_internal_generic_hash_delete(pcid_info, mem, banks,
                                                      msg->gencmd.data,
                                                      (uint32 *)&msg->genresp.response);
            } else
#endif
            if (mem == L2Xm) {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                rv = soc_internal_l2_tr_entry_del(pcid_info, banks,
                                          (l2x_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if ((mem == VLAN_MACm) || (mem == VLAN_XLATEm)) {
                rv = soc_internal_vlan_xlate_entry_del(pcid_info, banks,
                                          (vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VLAN_XLATEm) {
                rv = soc_internal_egr_vlan_xlate_entry_del(pcid_info, banks,
                                          (egr_vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#ifdef BCM_TRIUMPH_SUPPORT
            } else if ((SOC_IS_TOMAHAWK3(unit) && mem == MPLS_ENTRY_SINGLEm) ||
                       mem == MPLS_ENTRYm) {
                /* coverity[callee_ptr_arith] */
                rv = soc_internal_mpls_entry_del(pcid_info, banks,
                                          (mpls_entry_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            } else if (mem == ING_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_ing_vp_vlan_member_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_egr_vp_vlan_member_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ING_DNAT_ADDRESS_TYPEm) {
                rv = soc_internal_ing_dnat_address_type_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == L2_ENDPOINT_IDm) {
                rv = soc_internal_l2_endpoint_id_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ENDPOINT_QUEUE_MAPm) {
                rv = soc_internal_endpoint_queue_map_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            } else if (mem == EXACT_MATCH_2m || mem == EXACT_MATCH_4m) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                rv = soc_internal_exact_match_del
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif /* BCM_TOMAHAWK_SUPPORT */
            } else {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                rv = soc_internal_l3x2_entry_del(pcid_info, banks,
                              (l3_entry_ipv6_multicast_entry_t *)msg->gencmd.data,
                              (uint32 *)&msg->genresp.response);
            }
            if (soc_feature(unit, soc_feature_new_sbus_format) &&
                !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                if (msg->genresp_v2.response.type != SCHAN_GEN_RESP_TYPE_DELETED) {
                    msg->genresp_v2.header.v2.nack = 1;
                }
            } else {
                if (msg->genresp.response.type != SCHAN_GEN_RESP_TYPE_DELETED) {
                    msg->genresp.header.v2.nack = 1;
                }
            }
            break;
        }
    case TABLE_LOOKUP_CMD_MSG:
        if (soc_feature(unit, soc_feature_generic_table_ops)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "S-Channel operation: TABLE_LOOKUP_CMD_MSG\n")));

            opcode = TABLE_LOOKUP_DONE_MSG;
            banks = bank_ignore_mask;
            if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
                uint32 address;
                address = msg->gencmd.address;
                if (soc_feature(unit, soc_feature_ism_memory)) {
                    address &= ~SOC_HASH_BANK_MASK_ISM;
                }
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    address &= ~SOC_HASH_BANK_MASK_SHARED;
                }
                mem = soc_addr_to_mem_extended(pcid_info->unit, dst_blk,
                                               acc_type, address);
            } else {
                mem = soc_addr_to_mem(pcid_info->unit, msg->gencmd.address, NULL);
            } 
            assert(mem != INVALIDm);

#ifdef BCM_ISM_SUPPORT
            if (soc_feature(unit, soc_feature_ism_memory)) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_ISM;
                if (!banks) {
                    banks = SOC_MEM_HASH_BANK_ALL;
                } else {
                    banks = ~banks & SOC_HASH_BANK_MASK_ISM;
                }
                rv = soc_internal_generic_hash_lookup(pcid_info, mem, banks,
                                                      msg->gencmd.data,
                                                      (uint32 *)&msg->genresp.response);
            } else
#endif
            if (mem == L2Xm) {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                rv = soc_internal_l2_tr_entry_lkup(pcid_info, banks,
                                          (l2x_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if ((mem == VLAN_MACm) || (mem == VLAN_XLATEm)) {
                rv = soc_internal_vlan_xlate_entry_lkup(pcid_info, banks,
                                          (vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VLAN_XLATEm) {
                rv = soc_internal_egr_vlan_xlate_entry_lkup(pcid_info, banks,
                                          (egr_vlan_xlate_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#ifdef BCM_TRIUMPH_SUPPORT
            } else if ((SOC_IS_TOMAHAWK3(unit) && mem == MPLS_ENTRY_SINGLEm) ||
                       mem == MPLS_ENTRYm) {
                rv = soc_internal_mpls_entry_lkup(pcid_info, banks,
                                          (mpls_entry_entry_t *)msg->gencmd.data,
                                          (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
            } else if (mem == ING_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_ing_vp_vlan_member_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == EGR_VP_VLAN_MEMBERSHIPm) {
                rv = soc_internal_egr_vp_vlan_member_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ING_DNAT_ADDRESS_TYPEm) {
                rv = soc_internal_ing_dnat_address_type_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == L2_ENDPOINT_IDm) {
                rv = soc_internal_l2_endpoint_id_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
            } else if (mem == ENDPOINT_QUEUE_MAPm) {
                rv = soc_internal_endpoint_queue_map_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
            } else if (mem == EXACT_MATCH_2m || mem == EXACT_MATCH_4m) {
                banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                rv = soc_internal_exact_match_lkup
                    (pcid_info, banks, msg->gencmd.data,
                     (uint32 *)&msg->genresp.response);
#endif /* BCM_TOMAHAWK_SUPPORT */
            } else {
                if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                    banks = msg->gencmd.address & SOC_HASH_BANK_MASK_SHARED;
                }
                rv = soc_internal_l3x2_entry_lkup(pcid_info, banks,
                              (l3_entry_ipv6_multicast_entry_t *)msg->gencmd.data,
                              (uint32 *)&msg->genresp.response);
            }
            if (soc_feature(unit, soc_feature_new_sbus_format) &&
                !soc_feature(unit, soc_feature_new_sbus_old_resp) ) {
                if (msg->genresp_v2.response.type != SCHAN_GEN_RESP_TYPE_FOUND) {
                    msg->genresp_v2.header.v2.nack = 1;
                }
            } else {
                if (msg->genresp.response.type != SCHAN_GEN_RESP_TYPE_FOUND) {
                    msg->genresp.header.v2.nack = 1;
                }
            }
            break;
        }
    case FIFO_POP_CMD_MSG:
        if (soc_feature(unit, soc_feature_mem_push_pop)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "S-Channel operation: FIFO_POP_CMD_MSG\n")));

            mem = soc_addr_to_mem(pcid_info->unit, msg->popcmd.address, NULL);
            /* coverity[overrun-buffer-val] */
            rv = soc_internal_fifo_pop(pcid_info, mem, msg->popresp.data);
            if (rv == 1) {
                /* Fifo empty */
                msg->popresp.header.v2.nack = 1;
                rv = 0;
            }
            opcode = FIFO_POP_DONE_MSG;
        }
        break;
    case FIFO_PUSH_CMD_MSG:
        if (soc_feature(unit, soc_feature_mem_push_pop)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "S-Channel operation: FIFO_PUSH_CMD_MSG\n")));
            
            mem = soc_addr_to_mem(pcid_info->unit, msg->pushcmd.address, NULL);
            /* coverity[overrun-buffer-val] */
            rv = soc_internal_fifo_push(pcid_info, mem, msg->pushcmd.data);
            if (rv == 1) {
                /* Fifo full */
                msg->popresp.header.v2.nack = 1;
                rv = 0;
            }
            opcode = FIFO_PUSH_DONE_MSG;
        }
        break;
#endif /* BCM_TRX_SUPPORT */
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "S-Channel operation: message not handled: %s (%d)\n"),
                     soc_schan_op_name(opcode), opcode));
        break;
    }

    msg->header.v2.opcode = opcode;
    if (rv == 0) {
#ifdef BCM_CMICM_SUPPORT
        if (pcid_info->cmicm >= CMC0) {
/*            PCIM(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET) |= SC_CMCx_MSG_DONE;  */
            pcid_reg_or_write(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET, SC_CMCx_MSG_DONE);
        } else 
#endif
        {
/*            PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_MSG_DONE_TST; */
            pcid_reg_or_write(pcid_info, CMIC_SCHAN_CTRL, SC_MSG_DONE_TST);
        }
    }

    if (soc_feature(unit, soc_feature_schmsg_alias)) {
        /* Synchronize message buffer (at 0) with aliased memory */
        memcpy(&(PCIM(pcid_info, 0)),
               &(PCIM(pcid_info, PCIM_ALIAS_OFFSET)), PCIM_ALIASED_BYTES);
    }

    if(data) {
        *data = *msg; 
    }

    return rv;
}

/*
 * soc_internal_table_dma
 *
 * Do a table DMA transfer
 */

void
soc_internal_table_dma(pcid_info_t *pcid_info)
{
    uint32        entry[SOC_MAX_MEM_WORDS];
    uint32        table_addr;
    uint32        dma_addr;
    uint32        dma_count;
    uint32        dma_beats;

    table_addr = PCIM(pcid_info, CMIC_TABLE_DMA_START);
    dma_addr = PCIM(pcid_info, CMIC_ARL_DMA_ADDR);
    dma_count = PCIM(pcid_info, CMIC_ARL_DMA_CNT) & 0x1fffffff;
    dma_beats = PCIM(pcid_info, CMIC_ARL_DMA_CNT) >> 29;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Table DMA: schan_addr=0x%x count=0x%x beats=%d dest=0x%x\n"),
                 table_addr, dma_count, dma_beats, dma_addr));

    while (dma_count-- > 0) {
        soc_internal_read_mem(pcid_info, table_addr, entry);
        table_addr++;

        soc_internal_memory_store(pcid_info, dma_addr,
                                  (uint8 *)entry, dma_beats * 4,
                                  MF_ES_DMA_OTHER);
        dma_addr += dma_beats * 4;
    }

    

    PCIM(pcid_info, CMIC_SCHAN_CTRL) |= SC_ARL_DMA_DONE_TST;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_CNT0;
    soc_internal_send_int(pcid_info);
}

/*
 * soc_internal_xgs3_table_dma
 *
 * Do a table DMA transfer
 */

#define DMA_BURST_SIZE  256 /* Max number of WORDS per DMA transfer */

#if DMA_BURST_SIZE < SOC_MAX_MEM_WORDS
#error Burst size is too small
#endif

#define DMA_BURST_ALLOC  (DMA_BURST_SIZE + SOC_MAX_MEM_WORDS)
/* Max number of WORDS per DMA transfer + datum mem read overhead*/

void
soc_internal_xgs3_table_dma(pcid_info_t *pcid_info)
{
    uint32 entry[DMA_BURST_ALLOC];
    uint32 table_addr;
    uint32 dma_addr;
    uint32 dma_count;
    uint32 dma_incr;
    uint32 dma_beats;
    int    i, burst_count;
    schan_msg_t    *msg;
    int             opcode, dst_blk, src_blk, acc_type, data_byte_len, dma;
    uint32          bank_ignore_mask;

    msg = (schan_msg_t *)&PCIM(pcid_info,
                               CMIC_SCHAN_MESSAGE(pcid_info->unit, 0));

    if (!(PCIM(pcid_info, CMIC_TABLE_DMA_CFG) & CMIC_TDMA_CFG_EN)) {
        return;
    }
    soc_schan_header_cmd_get(pcid_info->unit, &msg->header, &opcode,
                             &dst_blk, &src_blk, &acc_type, &data_byte_len,
                             &dma, &bank_ignore_mask);

    table_addr = PCIM(pcid_info, CMIC_TABLE_DMA_SBUS_START_ADDR);
    dma_addr = PCIM(pcid_info, CMIC_TABLE_DMA_PCIMEM_START_ADDR);
    dma_count = PCIM(pcid_info, CMIC_TABLE_DMA_ENTRY_COUNT) & 0xffffff;
    dma_incr = (PCIM(pcid_info, CMIC_TABLE_DMA_ENTRY_COUNT) >> 24) & 0x1f;
    dma_beats = (PCIM(pcid_info, CMIC_TABLE_DMA_CFG) >> 16) & (0x1f);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Table DMA: schan_addr=0x%x count=0x%x inc=%d beats=%d dest=0x%x\n"),
                 table_addr, dma_count, dma_incr, dma_beats, dma_addr));

    burst_count = DMA_BURST_SIZE / dma_beats;

    while (dma_count > 0) {
        /* Adjust the burst size at the end of the table */
        if (dma_count < burst_count) burst_count = dma_count;
       
        /* Read as many entries as can fit in the burst buffer */
        for (i = 0; i < burst_count; i++) {
            soc_internal_extended_read_bytes
                (pcid_info, dst_blk, 0, table_addr,
                 (uint8 *) (entry + i * dma_beats),
                 dma_beats * (sizeof(uint32)));
            table_addr += 1 << dma_incr;
        }

        /* convert to CPU endian-ness */
        for (i=0; i < (burst_count * dma_beats); i++) {
            *(entry + i) = soc_internal_endian_swap(pcid_info, *(entry + i), 
                                                    MF_ES_DMA_OTHER);
        }
        
        /* Send all these entries in one shot */
        soc_internal_bytes_store(pcid_info, dma_addr, (uint8 *)entry,
                                 (dma_beats * sizeof(uint32) * burst_count));
        dma_addr += dma_beats * sizeof(uint32) * burst_count;
        dma_count -= burst_count;
    }

    PCIM(pcid_info, CMIC_TABLE_DMA_CFG) |= CMIC_TDMA_CFG_DONE;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_TDMA_DONE;
    soc_internal_send_int(pcid_info);
}

#ifdef BCM_CMICM_SUPPORT
void
soc_internal_xgs3_cmicm_table_dma(pcid_info_t *pcid_info)
{
    uint32      entry[DMA_BURST_ALLOC];
    uint32      table_addr;
    uint32      dma_addr;
    uint32      dma_count;
    uint32      dma_incr;
    uint32      dma_beats;
    int         i, burst_count;
    soc_block_t block;
    int         acc_type;

    if (!(PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_CFG_OFFSET) & CMIC_TDMA_CFG_EN)) {
        return;
    }

    table_addr = PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_SBUS_START_ADDR_OFFSET);
    dma_addr = PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_PCIMEM_START_ADDR_OFFSET);
    dma_count = PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_ENTRY_COUNT_OFFSET) & 0xffffff;
    dma_incr = (PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_ENTRY_COUNT_OFFSET) >> 24) & 0x1f;
    dma_beats = (PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_CFG_OFFSET) >> 11) & (0x1f);
    
    block = (PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_SBUS_CMD_CONFIG_OFFSET) >> 3) & 0x3f;
    acc_type = PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_SBUS_CMD_CONFIG_OFFSET) & 7;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Table DMA: schan_addr=0x%x block: %d acc_type=%d "
                          "count=0x%x inc=%d beats=%d dest=0x%x\n"),
                 table_addr, block, acc_type, dma_count, dma_incr, dma_beats,
                 dma_addr));

    burst_count = DMA_BURST_SIZE / dma_beats;

    while (dma_count > 0) {
        /* Adjust the burst size at the end of the table */
        if (dma_count < burst_count) burst_count = dma_count;
       
        /* Read as many entries as can fit in the burst buffer */
        for (i = 0; i < burst_count; i++) {
            soc_internal_extended_read_bytes(pcid_info, block, acc_type,
                                             table_addr,
                                             (uint8 *) (entry + i * dma_beats),
                                             dma_beats * (sizeof(uint32)));
            table_addr += 1 << dma_incr;
        }

        /* convert to CPU endian-ness */
        for (i=0; i < (burst_count * dma_beats); i++) {
            *(entry + i) = soc_internal_endian_swap(pcid_info, *(entry + i), 
                                                    MF_ES_DMA_OTHER);
        }
        
        /* Send all these entries in one shot */
        soc_internal_bytes_store(pcid_info, dma_addr, (uint8 *)entry,
                                 (dma_beats * sizeof(uint32) * burst_count));
        dma_addr += dma_beats * sizeof(uint32) * burst_count;
        dma_count -= burst_count;
    }

    PCIM(pcid_info, CMIC_CMC0_TABLE_DMA_STAT_OFFSET) |= TDMA_DONE;
    PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_TDMA_DONE;
    soc_internal_cmicm_send_int(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET);
}
#endif

/*
 * soc_internal_xgs3_tslam_dma
 *
 * Do a table SLAM DMA transfer
 */

void
soc_internal_xgs3_tslam_dma(pcid_info_t *pcid_info)
{
    uint32        entry[DMA_BURST_SIZE];
    uint32        table_addr;
    uint32        dma_addr;
    uint32        dma_count;
    uint32      dma_incr;
    uint32        dma_beats;
    int         direction;
    int         i, burst_count, entry_idx;
    schan_msg_t    *msg;
    int             opcode, dst_blk, src_blk, acc_type, data_byte_len, dma;
    uint32          bank_ignore_mask;

    msg = (schan_msg_t *)&PCIM(pcid_info,
                               CMIC_SCHAN_MESSAGE(pcid_info->unit, 0));

    if (!(PCIM(pcid_info, CMIC_SLAM_DMA_CFG) & CMIC_SLAM_CFG_EN)) {
        return;
    }
    
    soc_schan_header_cmd_get(pcid_info->unit, &msg->header, &opcode,
                             &dst_blk, &src_blk, &acc_type, &data_byte_len,
                             &dma, &bank_ignore_mask);

    table_addr = PCIM(pcid_info, CMIC_SLAM_DMA_SBUS_START_ADDR);
    dma_addr = PCIM(pcid_info, CMIC_SLAM_DMA_PCIMEM_START_ADDR);
    dma_count = PCIM(pcid_info, CMIC_SLAM_DMA_ENTRY_COUNT) & 0xffffff;
    dma_incr = (PCIM(pcid_info, CMIC_SLAM_DMA_ENTRY_COUNT) >> 24) & 0x1f;
    dma_beats = (PCIM(pcid_info, CMIC_SLAM_DMA_CFG) >> 16) & (0x1f);
    direction = (PCIM(pcid_info, CMIC_SLAM_DMA_CFG) & CMIC_SLAM_CFG_DIR);

    if (direction) {
        /* Last entry */
        dma_addr = dma_addr + dma_count * 4 * dma_beats;
        table_addr = table_addr + (dma_count - 1) * (1 << dma_incr);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Table DMA: schan_addr=0x%x count=0x%x inc=%d beats=%d dest=0x%x\n"),
                 table_addr, dma_count, dma_incr, dma_beats, dma_addr));

    burst_count = DMA_BURST_SIZE / dma_beats;

    while (dma_count > 0) {
        /* Adjust the burst size at the end of the table */
        if (dma_count < burst_count) burst_count = dma_count;

        /* Get as many entries as can fit in the burst buffer */
        soc_internal_bytes_fetch(pcid_info, 
                                 (direction ? 
                                   (dma_addr - burst_count * dma_beats * 4) : 
                                   dma_addr),
                                 (uint8 *)entry,
                                 dma_beats * 4 * burst_count);
        
        /* convert to chip (same as host for sim) endian-ness */
        for (i=0; i < (burst_count * dma_beats); i++) {
            *(entry + i) = soc_internal_endian_swap(pcid_info, *(entry + i),
                                                    MF_ES_DMA_OTHER);
        }

        /* Write all the entries we've got */
        for (i = 0; i < burst_count; i++) {
            entry_idx = direction ? burst_count - i - 1 : i;
            soc_internal_extended_write_bytes
                (pcid_info, dst_blk, 0, table_addr,
                 (uint8 *)(entry + entry_idx * dma_beats), dma_beats * 4);
            table_addr += direction ? -(1 << dma_incr) : 1 << dma_incr;
        }

        dma_addr += (direction ? -(dma_beats * 4 * burst_count) : 
                                 (dma_beats * 4 * burst_count));
        dma_count -= burst_count;
    }

    PCIM(pcid_info, CMIC_SLAM_DMA_CFG) |= CMIC_SLAM_CFG_DONE;
    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_TSLAM_DONE;
    soc_internal_send_int(pcid_info);
}

#ifdef BCM_CMICM_SUPPORT
void
soc_internal_xgs3_cmicm_tslam_dma(pcid_info_t *pcid_info)
{
    uint32      entry[DMA_BURST_SIZE];
    uint32      table_addr;
    uint32      dma_addr;
    uint32      dma_count;
    uint32      dma_incr;
    uint32      dma_beats;
    int         direction;
    int         i, burst_count, entry_idx;
    soc_block_t block;
    int         acc_type;

    if (!(PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_CFG_OFFSET) & CMIC_SLAM_CFG_EN)) {
        return;
    }
    
    table_addr = PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_SBUS_START_ADDR_OFFSET);
    dma_addr = PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_PCIMEM_START_ADDR_OFFSET);
    dma_count = PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_ENTRY_COUNT_OFFSET) & 0xffffff;
    dma_incr = (PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_ENTRY_COUNT_OFFSET) >> 24) & 0x1f;
    dma_beats = (PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_CFG_OFFSET) >> 11) & (0x1f);
    direction = (PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_CFG_OFFSET) & SLDMA_CFG_DIR);

    block = (PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_SBUS_CMD_CONFIG_OFFSET) >> 3) & 0x3f;
    acc_type = PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_SBUS_CMD_CONFIG_OFFSET) & 7;

    if (direction) {
        /* Last entry */
        dma_addr = dma_addr + dma_count * 4 * dma_beats;
        table_addr = table_addr + (dma_count - 1) * (1 << dma_incr);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Table DMA: schan_addr=0x%x block=%d acc_type=%d "
                          "count=0x%x inc=%d beats=%d dest=0x%x\n"),
                 table_addr, block, acc_type, dma_count, dma_incr, dma_beats,
                 dma_addr));

    burst_count = DMA_BURST_SIZE / dma_beats;

    while (dma_count > 0) {
        /* Adjust the burst size at the end of the table */
        if (dma_count < burst_count) burst_count = dma_count;

        /* Get as many entries as can fit in the burst buffer */
        soc_internal_bytes_fetch(pcid_info, 
                                 (direction ? 
                                   (dma_addr - burst_count * dma_beats * 4) : 
                                   dma_addr),
                                 (uint8 *)entry,
                                 dma_beats * 4 * burst_count);
        
        /* convert to chip (same as host for sim) endian-ness */
        for (i=0; i < (burst_count * dma_beats); i++) {
            *(entry + i) = soc_internal_endian_swap(pcid_info, *(entry + i),
                                                    MF_ES_DMA_OTHER);
        }

        /* Write all the entries we've got */
        for (i = 0; i < burst_count; i++) {
            entry_idx = direction ? burst_count - i - 1 : i;
            soc_internal_extended_write_bytes(pcid_info, block, acc_type,
                                              table_addr, 
                                     (uint8 *)(entry + entry_idx * dma_beats),
                                     dma_beats * 4);
            table_addr += direction ? -(1 << dma_incr) : 1 << dma_incr;
        }

        dma_addr += (direction ? -(dma_beats * 4 * burst_count) : 
                                 (dma_beats * 4 * burst_count));
        dma_count -= burst_count;
    }

    PCIM(pcid_info, CMIC_CMC0_SLAM_DMA_STAT_OFFSET) |= SLDMA_DONE;
    PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_TSLAM_DONE;
    soc_internal_cmicm_send_int(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET);
}

void
soc_internal_cmicm_sbusdma(pcid_info_t *pcid_info, uint32 reg)
{
    schan_msg_t msg;
    int         opcode, dst_blk, src_blk, acc_type, data_byte_len, dma;
    uint32      bank_ignore_mask;
    uint32      buffer[DMA_BURST_ALLOC];
    uint32      rval;
    uint32      host_addr, sbus_addr, index, offset;
    int         host_incr, sbus_incr, index_incr, offset_incr;
    uint32      dma_count, beats;
    int         i, burst_count, burst_size;
    int8        ch;
    int         cmc;
    soc_mem_t   mem;
    soc_field_t field;

    cmc = ((reg >> 12) & 0x3) - 1;
    ch = ((reg & 0xfff) - 0x600) / 0x50;

    rval = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, ch));
    if (!soc_reg_field_get(pcid_info->unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                           rval, STARTf)) {
        return;
    }

    msg.header_dword = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_OPCODE(cmc, ch));
    soc_schan_header_cmd_get(pcid_info->unit, &msg.header, &opcode,
                             &dst_blk, &src_blk, &acc_type, &data_byte_len,
                             &dma, &bank_ignore_mask);

    switch (opcode) {
    case WRITE_MEMORY_CMD_MSG:
        field = REQ_WORDSf;
        break;
    case READ_MEMORY_CMD_MSG:
        field = REP_WORDSf;
        break;
    default:
        return;
    }
    host_addr = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_HOSTADDR(cmc, ch));
    sbus_addr = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_ADDRESS(cmc, ch));
    dma_count = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_COUNT(cmc, ch));

    mem = soc_addr_to_mem_extended(pcid_info->unit, dst_blk, acc_type, sbus_addr);
    assert(mem != INVALIDm);
    index = sbus_addr ^ SOC_MEM_INFO(pcid_info->unit, mem).base;

    rval = PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, ch));
    beats = soc_reg_field_get(pcid_info->unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                              rval, field);
    offset_incr = soc_reg_field_get(pcid_info->unit,
                                    CMIC_CMC0_SBUSDMA_CH0_REQUESTr, rval,
                                    REQ_SINGLEf) ? 0 : beats;
    host_incr = offset_incr * sizeof(uint32);
    if (soc_reg_field_get(pcid_info->unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                          rval, INCR_NOADDf)) {
        sbus_incr = 0;
        index_incr = 0;
    } else {
        sbus_incr =
            1 << soc_reg_field_get(pcid_info->unit,
                                   CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                                   rval, INCR_SHIFTf);
        index_incr = 1;
    }

    if (host_incr == 0) {
        burst_count = dma_count;
        burst_size = beats * sizeof(uint32);
    } else {
        burst_count = DMA_BURST_SIZE / beats;
        burst_size = burst_count * beats * sizeof(uint32);
    }

    if (soc_reg_field_get(pcid_info->unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr,
                           rval, DECRf)) {
        sbus_addr += (dma_count - 1) * sbus_incr;
        sbus_incr = -sbus_incr;
        index_incr = -index_incr;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("SBUS DMA: mem=%d index=0x%x sbus_addr=0x%x dst_blk=%d acc_type=%d"
                          "dma_cout=0x%x beats=%d sbus_incr=%d host_addr=0x%x host_incr=%d\n"),
                 mem, index, sbus_addr, dst_blk, acc_type, dma_count, beats, sbus_incr,
                 host_addr, host_incr));

    if (opcode == WRITE_MEMORY_CMD_MSG) {
        while (dma_count > 0) {
            /* Adjust the burst size at the end of the table */
            if (dma_count < burst_count) {
                burst_count = dma_count;
                burst_size = burst_count * beats * sizeof(uint32);
            }

            /* Fill the burst buffer from S-chan socket */
            soc_internal_bytes_fetch(pcid_info, host_addr, (uint8 *)buffer,
                                     burst_size);

            /* Convert to device (same as host for sim) endian-ness */
            for (i = 0; i < burst_size / sizeof(uint32); i++) {
                buffer[i] = soc_internal_endian_swap(pcid_info, buffer[i],
                                                     MF_ES_DMA_OTHER);
            }

            /* Write entries from burst buffer to sim storage */
            offset = 0;
            for (i = 0; i < burst_count; i++) {
                soc_internal_extended_write_mem_index(pcid_info, mem, index,
                                                      &buffer[offset]);
                index += index_incr;
                offset += offset_incr;
            }

            sbus_addr += sbus_incr * burst_count;
            host_addr += host_incr * burst_count;
            dma_count -= burst_count;
        }
    } else {
        while (dma_count > 0) {
            /* Adjust the burst size at the end of the table */
            if (dma_count < burst_count) {
                burst_count = dma_count;
                burst_size = burst_count * beats * sizeof(uint32);
            }

            /* Read entries sim storage into burst buffer */
            offset = 0;
            for (i = 0; i < burst_count; i++) {
                soc_internal_extended_read_mem_index(pcid_info, mem, index,
                                                     &buffer[offset]);
                index += index_incr;
                offset += offset_incr;
            }

            /* Convert to host (same as device for sim) endian-ness */
            for (i = 0; i < burst_size / sizeof(uint32); i++) {
                buffer[i] = soc_internal_endian_swap(pcid_info, buffer[i],
                                                     MF_ES_DMA_OTHER);
            }

            /* Flush the burst buffer into S-chan socket */
            soc_internal_bytes_store(pcid_info, host_addr, (uint8 *)buffer,
                                     burst_size);

            sbus_addr += sbus_incr * burst_count;
            host_addr += host_incr * burst_count;
            dma_count -= burst_count;
        }
    }

    PCIM(pcid_info, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,ch)) |=
        ST_CMCx_SBUSDMA_DONE;
    PCIM(pcid_info, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc)) |=
        IRQ_CMCx_SBUSDMA_CH_DONE(ch);
    soc_internal_cmicm_send_int(pcid_info,
                                CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc));
}
#endif

void
soc_internal_schan_ctrl_write(pcid_info_t *pcid_info, uint32 value)
{
    if (value & 0x80) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Set SCHAN_CTRL bit %d\n"), value & 0x1f));
        switch (value & 0xff) {
        case SC_MSG_START_SET:
            if ((pcid_info->schan_cb) && 
                (pcid_info->schan_cb(pcid_info, pcid_info->unit, NULL) == 0)) {
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_SCH_MSG_DONE;
                soc_internal_send_int(pcid_info);
                break;
            } else {
                if (schan_op(pcid_info, pcid_info->unit, NULL) == 0) {
                    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_SCH_MSG_DONE;
                    soc_internal_send_int(pcid_info);
                }
            }
            break;
        case SC_MIIM_RD_START_SET:
            soc_internal_miim_op(pcid_info, 1);
            PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_MIIM_OP_DONE;
            soc_internal_send_int(pcid_info);
            break;
        case SC_MIIM_WR_START_SET:
            soc_internal_miim_op(pcid_info, 0);
            PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_MIIM_OP_DONE;
            soc_internal_send_int(pcid_info);
            break;
        case SC_ARL_DMA_EN_SET:
            if (SOC_IS_XGS_SWITCH(pcid_info->unit)) {
                soc_internal_table_dma(pcid_info);
                PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_ARL_DMA_CNT0;
                soc_internal_send_int(pcid_info);
            } else {
                PCIM(pcid_info, CMIC_SCHAN_CTRL) |= 1 << (value & 0x1f);
            }
            break;
        default:
            PCIM(pcid_info, CMIC_SCHAN_CTRL) |= 1 << (value & 0x1f);
            break;
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Clear SCHAN_CTRL bit %d\n"), value & 0x1f));
        PCIM(pcid_info, CMIC_SCHAN_CTRL) &= ~(1 << (value & 0x1f));
        switch (value & 0xff) {
        case SC_MSG_DONE_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_SCH_MSG_DONE;
            break;
        case SC_MIIM_OP_DONE_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_MIIM_OP_DONE;
            break;
        case SC_ARL_DMA_EN_CLR:
            PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_ARL_DMA_CNT0;
            break;
        default:
            break;
        }
    }
}

#ifdef BCM_CMICM_SUPPORT
void
soc_internal_cmicm_schan_ctrl_write(pcid_info_t *pcid_info, uint32 reg, uint32 value)
{
    if (value & SC_CMCx_MSG_START) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("SCHAN_CTRL msg start.\n")));
        if ((pcid_info->schan_cb) && 
            (pcid_info->schan_cb(pcid_info, pcid_info->unit, NULL) == 0)) {
            PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_SCH_OP_DONE;
            soc_internal_cmicm_send_int(pcid_info, reg);
        } else {
            if (schan_op(pcid_info, pcid_info->unit, NULL) == 0) {
                PCIM(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET) &= 
                    ~(SC_CMCx_MSG_SER_CHECK_FAIL | SC_CMCx_MSG_NAK | SC_CMCx_MSG_TIMEOUT_TST);
                PCIM(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET) |= IRQ_CMCx_SCH_OP_DONE;
                soc_internal_cmicm_send_int(pcid_info, reg);
            }
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("SCHAN_CTRL msg done.\n")));
        PCIM(pcid_info, CMIC_CMC0_SCHAN_CTRL_OFFSET) &= 
                ~(SC_CMCx_MSG_DONE | SC_CMCx_MSG_SER_CHECK_FAIL | 
                SC_CMCx_MSG_NAK | SC_CMCx_MSG_TIMEOUT_TST);
    }
}
#endif

void
soc_internal_reset(pcid_info_t *pcid_info)
{
    int index;
    soc_datum_t *d;

    memset(&PCIM(pcid_info, 0), 0, PCIM_SIZE(0));

#ifdef BCM_CMICM_SUPPORT
    if (pcid_info->single_mode) {
        pcid_info->cmicm = CMC0; 
    } else {
        if (soc_feature(pcid_info->unit, soc_feature_cmicm)) {
            pcid_info->cmicm = CMC0;
            
        } else
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        if (soc_feature(pcid_info->unit, soc_feature_cmicx) &&
          (SOC_IS_DNX(pcid_info->unit) || SOC_IS_DNXF(pcid_info->unit))) {
            /* Use CMICM simulation instead of CMICX simulation */
            pcid_info->cmicm = CMC0;
        } else
#endif /* BCM_DNX_SUPPORT || BCM_DNXF_SUPPORT */
#endif
        {
            pcid_info->cmicm = -1;
            PCIM(pcid_info, CMIC_SCHAN_CTRL) = 0x48002;
        }
#ifdef BCM_CMICM_SUPPORT
    }
#endif
    /*
     * Delete all register values from hash table, which effectively
     * returns them to their reset default values.
     */
    for (index = 0; index < SOC_DATUM_HASH_SIZE; index++) {
        while ((d = pcid_info->reg_ht[index]) != NULL) {
            pcid_info->reg_ht[index] = d->next;
            sal_free(d);
        }
    }

    /*
     * Delete all memory entries from hash table, which effectively
     * returns them to their null entry values.
     */
    for (index = 0; index < SOC_DATUM_HASH_SIZE; index++) {
        while ((d = pcid_info->mem_ht[index]) != NULL) {
            pcid_info->mem_ht[index] = d->next;
            sal_free(d);
        }
    }

    /*
     * Invoke reset callback if installed.
     */
    if (pcid_info->reset_cb) {
        pcid_info->reset_cb(pcid_info, pcid_info->unit);
    }
}

uint32
soc_internal_endian_swap(pcid_info_t *pcid_info, uint32 data, int mode)
{
    if (((mode == MF_ES_DMA_PACKET) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_DMA_PACKET))
        ||
        ((mode == MF_ES_DMA_OTHER) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_DMA_OTHER))
        ||
        ((mode == MF_ES_PIO) &&
         (PCIM(pcid_info, CMIC_ENDIAN_SELECT) & ES_BIG_ENDIAN_PIO))) {
        data = (data << 16) | (data >> 16);
        return (data & 0xff00ffff) >> 8 | (data & 0xffff00ff) << 8;
    } else {
        return(data);
    }
}

void
soc_internal_memory_fetch(pcid_info_t *pcid_info, uint32 addr, uint8 *b,
                          int length, int mode)
{
    uint32        offset;

    if (pcid_info->opt_rpc_error) {
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_internal_memory_fetch: addr=0x%08x length=%d\n"),
                 addr, length));

    /*
     * For memory fetch, any alignment is allowed, but 4-byte accesses
     * are always made. This is slow but ....
     */
    offset = addr & 3;

    addr &= ~3;            /* Only read 4-bytes */

    while (length) {
        uint32    data;
        int    tl;
        int rv;

        rv = dma_readmem(pcid_info->client->dmasock, addr, &data);
        if (rv != RPC_OK) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("RPC error: soc_internal_memory_fetch failed. \n")));
            pcid_info->opt_rpc_error = 1;
            break;
        }

        data =
            soc_internal_endian_swap(pcid_info,
                                     data,
                                     mode);
        tl = (sizeof(uint32) - offset);
        if (length < tl) {
            tl = length;
        }

        sal_memcpy(b, ((uint8 *)&data) + offset, tl);
        length -= tl;
        addr += 4;
        b += 4 - offset;
        offset = 0;
    }
}

/* Get a string of bytes from memory addr given length; store in buf. */
void
soc_internal_bytes_fetch(pcid_info_t *pcid_info, uint32 addr, uint8 *buf,
                          int length)
{
    int read_len;

    if (pcid_info->opt_rpc_error) {
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_internal_bytes_fetch: addr=0x%08x length=%d\n"),
                 addr, length));

    read_len = dma_read_bytes(pcid_info->client->dmasock, addr, buf, length);

    if (read_len != length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("soc_internal_bytes_fetch: length mismatch.  want %d. got %d\n"),
                   length, read_len));
    }

    if (read_len < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("RPC error: soc_internal_bytes_fetch failed. \n")));
        pcid_info->opt_rpc_error = 1;
    }
}

/* Get a string of bytes from memory addr given length; store in buf. */
void
soc_internal_bytes_store(pcid_info_t *pcid_info, uint32 addr, uint8 *buf,
                         int length)
{
    int rv;

    if (pcid_info->opt_rpc_error) {
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_internal_bytes_store: addr=0x%08x length=%d\n"),
                 addr, length));

    rv = dma_write_bytes(pcid_info->client->dmasock, addr, buf, length);
    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("RPC error: soc_internal_bytes_store failed. \n")));
        pcid_info->opt_rpc_error = 1;
    }
}

void
soc_internal_memory_store(pcid_info_t *pcid_info, uint32 addr, uint8 *b,
                          int length, int mode)
{
    uint32          data;
    int             rv;

    if (pcid_info->opt_rpc_error) {
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("soc_internal_memory_store: addr=0x%08x length=%d\n"),
                 addr, length));

    /*
     * Strata (and friends) only support 4-byte aligned writes
     */

   addr &= ~3;

    while (length > 0) {
        sal_memcpy(&data, b, length < sizeof(data) ? length : sizeof(data));
        rv = dma_writemem(pcid_info->client->dmasock,
                     addr, soc_internal_endian_swap(pcid_info, data, mode));
        if (rv != RPC_OK) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("RPC error: soc_internal_memory_store failed. \n")));
            pcid_info->opt_rpc_error = 1;
            break;
        }
        b += sizeof(uint32);
        addr += sizeof(uint32);
        length -= sizeof(uint32);
    }
}

/*
 * soc_internal_send_int
 *
 *    Checks if any interrupt is pending, and if so, sends an interrupt
 *    over the PLI socket.
 */

void
soc_internal_send_int(pcid_info_t *pcid_info)
{
    int rv;

    if (pcid_info->opt_rpc_error) {
        return;
    }
    if (PCIM(pcid_info, CMIC_IRQ_MASK) & PCIM(pcid_info, CMIC_IRQ_STAT)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Send interrupt: msk 0x%x stat 0x%x\n"),
                     PCIM(pcid_info, CMIC_IRQ_MASK), PCIM(pcid_info, CMIC_IRQ_STAT)));
        rv = send_interrupt(pcid_info->client->intsock, 0);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("RPC error: soc_internal_send_int failed. \n")));
            pcid_info->opt_rpc_error = 1;
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Interrupt masked: msk 0x%x stat 0x%x\n"),
                     PCIM(pcid_info, CMIC_IRQ_MASK), PCIM(pcid_info, CMIC_IRQ_STAT)));
    }
}

#ifdef BCM_CMICM_SUPPORT
void
soc_internal_cmicm_send_int(pcid_info_t *pcid_info, uint32 reg)
{
    int rv;

    if (pcid_info->opt_rpc_error) {
        return;
    }
    switch (reg) {
    case CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET:
        if (pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET) & 
            pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Send interrupt: msk0 0x%x stat0 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET)));
            rv = send_interrupt(pcid_info->client->intsock, 0);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("RPC error: soc_internal_send_int failed. \n")));
                pcid_info->opt_rpc_error = 1;
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Interrupt masked: msk0 0x%x stat0 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT0_OFFSET)));
        }
        break;
    case CMIC_CMC0_PCIE_IRQ_MASK1_OFFSET:
        if (pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK1_OFFSET) & 
            pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT1_OFFSET)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Send interrupt: msk1 0x%x stat1 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK1_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT1_OFFSET)));
            rv = send_interrupt(pcid_info->client->intsock, 0);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("RPC error: soc_internal_send_int failed. \n")));
                pcid_info->opt_rpc_error = 1;
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Interrupt masked: msk1 0x%x stat1 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK1_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT1_OFFSET)));
        }
        break;
    case CMIC_CMC0_PCIE_IRQ_MASK2_OFFSET:
        if (pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK2_OFFSET) & 
            pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT2_OFFSET)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Send interrupt: msk2 0x%x stat2 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK2_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT2_OFFSET)));
            rv = send_interrupt(pcid_info->client->intsock, 0);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("RPC error: soc_internal_send_int failed. \n")));
                pcid_info->opt_rpc_error = 1;
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("Interrupt masked: msk2 0x%x stat2 0x%x\n"),
                         pcid_reg_read(pcid_info, CMIC_CMC0_PCIE_IRQ_MASK2_OFFSET), 
                         pcid_reg_read(pcid_info, CMIC_CMC0_IRQ_STAT2_OFFSET)));
        }
    }
}
#endif

static const soc_reg_t ing_mpls_tpid_regs[] = {
    ING_MPLS_TPID_0r, ING_MPLS_TPID_1r,
    ING_MPLS_TPID_2r, ING_MPLS_TPID_3r
};
static const soc_reg_t ing_outer_tpid_regs[] = {
    ING_OUTER_TPID_0r, ING_OUTER_TPID_1r,
    ING_OUTER_TPID_2r, ING_OUTER_TPID_3r
};
static const soc_reg_t egr_outer_tpid_regs[] = {
    EGR_OUTER_TPID_0r, EGR_OUTER_TPID_1r,
    EGR_OUTER_TPID_2r, EGR_OUTER_TPID_3r
};

int
soc_internal_extended_write_reg(pcid_info_t *pcid_info,
                                soc_block_t block, int acc_type,
                                uint32 address, uint32 *data)
{
    soc_regaddrinfo_t ainfo;
    int rv;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */

        soc_regaddrinfo_extended_get(pcid_info->unit, &ainfo, block, acc_type,
                                     address);
    
        if (!ainfo.valid || ainfo.reg < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Attempt to write to unknown S-channel "
                                "block %d acc_type %d address 0x%x\n"),
                       block, acc_type, address));
            return SOC_E_NOT_FOUND;
        } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
            SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags &
                   SOC_REG_FLAG_RO) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Attempt to write to read-only S-channel address 0x%x\n"),
                       address));
        }

        switch (ainfo.reg) {
        case EGR_OUTER_TPIDr:
            ainfo.reg = egr_outer_tpid_regs[(address >> 8) & 0x3];
            ainfo.idx = -1;
            break;
        case ING_OUTER_TPIDr:
            ainfo.reg = ing_outer_tpid_regs[(address >> 8) & 0x3];
            ainfo.idx = -1;
            break;
        case ING_MPLS_TPIDr:
            ainfo.reg = ing_mpls_tpid_regs[(address >> 8) & 0x3];
            ainfo.idx = -1;
            break;
        }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
        if (soc_feature(pcid_info->unit, soc_feature_logical_port_num)) {
            soc_info_t *si;
            int phy_port, num_phy_ports = SOC_MAX_NUM_PORTS;
            int blk;
            int field_len;

            si = &SOC_INFO(pcid_info->unit);
            switch (ainfo.reg) {
            case EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr:
                phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg,
                                             data[0], PHYSICAL_PORT_NUMBERf);
                field_len = soc_reg_field_length(pcid_info->unit, ainfo.reg,
                                                 PHYSICAL_PORT_NUMBERf);

                if (SOC_MEM_IS_VALID(pcid_info->unit, ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm))  {
                    num_phy_ports = soc_mem_index_count(pcid_info->unit, ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm);
                }

                if ((phy_port != (1 << field_len) - 1) &&
                    (phy_port < num_phy_ports)){
                    si->port_p2l_mapping[phy_port] = ainfo.port;
                    si->port_l2p_mapping[ainfo.port] = phy_port;
                    blk = SOC_PORT_BLOCK(pcid_info->unit, phy_port);
                    si->port_type[ainfo.port] =
                        SOC_BLOCK_INFO(pcid_info->unit, blk).type;
                    if (ainfo.port > si->port.max) {
                        si->port.max = ainfo.port;
                    }
                }
                break;
            case MMU_TO_PHY_PORT_MAPPINGr:
                phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg,
                                             data[0], PHY_PORTf);
                field_len = soc_reg_field_length(pcid_info->unit, ainfo.reg,
                                                 PHY_PORTf);
                if (phy_port != (1 << field_len) - 1) {
                    si->port_p2m_mapping[phy_port] = ainfo.idx;
                    /*    coverity[negative_returns : FALSE]    */
                    si->port_m2p_mapping[ainfo.idx] = phy_port;
                }
                break;
            case EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr:
                phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg,
                                             data[0], PHYSICAL_PORT_NUMBERf);
                field_len = soc_reg_field_length(pcid_info->unit, ainfo.reg,
                                                 PHYSICAL_PORT_NUMBERf);
                if (phy_port != (1 << field_len) - 1) {
                    si->port_p2l_mapping[phy_port] = ainfo.port;
                    si->port_l2p_mapping[ainfo.port] = phy_port;
                    blk = SOC_PORT_BLOCK(pcid_info->unit, phy_port);
                    si->port_type[ainfo.port] =
                        SOC_BLOCK_INFO(pcid_info->unit, blk).type;
                    if (ainfo.port > si->port.max) {
                        si->port.max = ainfo.port;
                    }
                }
                break;
            case MMU_PORT_TO_PHY_PORT_MAPPINGr:
                phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg,
                                             data[0], PHY_PORTf);
                field_len = soc_reg_field_length(pcid_info->unit, ainfo.reg,
                                                 PHY_PORTf);
                if (phy_port != (1 << field_len) - 1) {
                    si->port_p2m_mapping[phy_port] = address & 0xff;
                    /*    coverity[negative_returns : FALSE]    */
                    si->port_m2p_mapping[address & 0xff] = phy_port;
                }
                break;
            case MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr:
                phy_port = soc_reg_field_get(pcid_info->unit, ainfo.reg,
                                             data[0], PHY_PORT_NUMf);
                field_len = soc_reg_field_length(pcid_info->unit, ainfo.reg,
                                                 PHY_PORT_NUMf);
                if (phy_port != (1 << field_len) - 1) {
                    si->port_p2m_mapping[phy_port] = address & 0xff;
                    /*    coverity[negative_returns : FALSE]    */
                    si->port_m2p_mapping[address & 0xff] = phy_port;
                }
                break;
            default:
                break;
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
        if (ainfo.acc_type < 0xff) {
            block |= acc_type << 24;
        }
        if (ainfo.reg == Q_SCHED_RQE_SNAPSHOTr) {
            *data &= ~0x1;  /* always assume flush is finished. */
        }
        rv = soc_datum_reg_write(pcid_info, ainfo, block, address, data);

        return rv;
    } else {
        return soc_internal_write_reg(pcid_info, address, data);
    }
}

int
soc_internal_extended_read_reg(pcid_info_t *pcid_info,
                               soc_block_t block, int acc_type,
                               uint32 address, uint32 *data)
{
    soc_regaddrinfo_t ainfo;
    int rv;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */

        soc_regaddrinfo_extended_get(pcid_info->unit, &ainfo, block, acc_type,
                                     address);

        if (!ainfo.valid || ainfo.reg < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Attempt to read from unknown register S-channel "
                                "block %d acc_type %d address 0x%x\n"),
                       block, acc_type, address));
            return SOC_E_NOT_FOUND;
        } else if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
            SOC_REG_INFO(pcid_info->unit, ainfo.reg).flags & SOC_REG_FLAG_WO) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("Attempt to read from write-only S-channel address 0x%x\n"),
                       address));
        }

        switch (ainfo.reg) {
        case EGR_OUTER_TPIDr:
            if (SOC_IS_GREYHOUND(pcid_info->unit) || 
                SOC_IS_HURRICANE3(pcid_info->unit) ||
                SOC_IS_GREYHOUND2(pcid_info->unit)){
                ainfo.reg = ing_outer_tpid_regs[(address >> 8) - 0x120013];
            } else {
                ainfo.reg = egr_outer_tpid_regs[(address >> 8) & 0x3];
            }
            ainfo.idx = -1;
            break;
        case ING_OUTER_TPIDr:
            if (SOC_IS_TRIUMPH3(pcid_info->unit)) {
                ainfo.reg = ing_outer_tpid_regs[(address >> 8) - 0xa0016];
            } else if (SOC_IS_KATANA2(pcid_info->unit)) {
                ainfo.reg = ing_outer_tpid_regs[(address >> 8) - 0xa0017];
            } else if (SOC_IS_HURRICANE2(pcid_info->unit) || 
                SOC_IS_GREYHOUND(pcid_info->unit) ||
                SOC_IS_HURRICANE3(pcid_info->unit) ||
                SOC_IS_GREYHOUND2(pcid_info->unit)) {
                ainfo.reg = ing_outer_tpid_regs[(address >> 8) - 0xa000d];
            } else {
                ainfo.reg = ing_outer_tpid_regs[(address >> 8) & 0x3];
            }
            ainfo.idx = -1;
            break;
        case ING_MPLS_TPIDr:
            ainfo.reg = ing_mpls_tpid_regs[(address >> 8) & 0x3];
            ainfo.idx = -1;
            break;
        }
        if (ainfo.acc_type < 0xff) {
            block |= acc_type << 24;
        }
        rv = soc_datum_reg_read(pcid_info, ainfo, block, address, data);
        if (SOC_E_NOT_FOUND == rv) {
            /*
             * The register is not in the hash table, so it has not been
             * written before.  Provide a default value.
             */
            if (SOC_REG_IS_VALID(pcid_info->unit, ainfo.reg) &&
                (ainfo.valid && ainfo.reg >= 0)) {
                /*This macro handle both 64 and above_64 regs*/
                SOC_REG_ABOVE_64_RST_VAL_GET(pcid_info->unit, ainfo.reg, data);
            } else {
                data[0] = 0xffffffff;
                data[1] = 0xffffffff;
            }
            rv = SOC_E_NONE;
        }

        if (SOC_E_NONE == rv) {
            if (SOC_IS_TRIDENT3X(pcid_info->unit)) {
                static soc_field_t f_iter, field_done[] = {
                                                  DONEf,
                                                  INVALIDf
                                                  };
                for(f_iter = 0; field_done[f_iter] != INVALIDf; f_iter++) {
                    if (soc_reg_field_valid(pcid_info->unit, ainfo.reg,
                                            field_done[f_iter])) {
                        soc_reg_field_set(pcid_info->unit, ainfo.reg,
                                          data, field_done[f_iter], 1);
                    }
                }
            } else if (SOC_IS_TOMAHAWK2(pcid_info->unit)) {
                soc_field_t f_iter;
                soc_field_t field_done[] = {DONEf, INVALIDf};
                for(f_iter = 0; field_done[f_iter] != INVALIDf; f_iter++) {
                    if (soc_reg_field_valid(pcid_info->unit, ainfo.reg,
                                            field_done[f_iter])) {
                        soc_reg_field_set(pcid_info->unit, ainfo.reg,
                                          data, field_done[f_iter], 1);
                    }
                }
            } else if (SOC_IS_TRIUMPH3(pcid_info->unit)) {
                /* ING/EGR/ISM_HW_RESET_CONTROL */
                if (((address == 0x2010200) && (block == 1)) ||
                    ((address == 0x2010100) && (block == 2))) {
                    data[0] |= (1 << 20);
                }
                if ((address == 0xfe000f00) && (block == 16)) {
                    data[0] |= (1 << 31);
                }
                if ((address == 0x2000100) && (block == 3)) {
                    data[0] |= 0x2;
                } 
                if ((address == 0xfe000200) && (block == 16)) {
                    data[0] |= 0x10; /* L2 ageing complete */
                }
                if ((address == 0x5a000100) && (block == 3)) {
                    data[0] |= 2; /* LLS init done */
                }
            } else if (SOC_IS_TD2_TT2(pcid_info->unit)) {
                switch (ainfo.reg) {
                case ING_HW_RESET_CONTROL_2r:
                case ING_HW_RESET_CONTROL_2_Xr:
                case ING_HW_RESET_CONTROL_2_Yr:
                    if(SOC_IS_TD2P_TT2P(pcid_info->unit)
                       || SOC_IS_APACHE(pcid_info->unit)) {
                        data[0] |= (1 << 22);
                    }
                    else {
                        data[0] |= (1 << 21);
                    }
                    break;
                case EGR_HW_RESET_CONTROL_1r:
                case EGR_HW_RESET_CONTROL_1_Xr:
                case EGR_HW_RESET_CONTROL_1_Yr:
                    data[0] |= (1 << 18);
                    break;
                
                case TOP_TSC_AFE_PLL_STATUSr:
                    data[0] = 0xffffffff;
                    break;
                default:
                    break;
                }
            }
        }
        return rv;
    } else {
        return soc_internal_read_reg(pcid_info, address, data);
    }
}

int
_soc_internal_extended_write_mem(pcid_info_t *pcid_info, soc_mem_t mem,
                                 soc_block_t block, int acc_type,
                                 uint32 address, uint32 *data)
{
    soc_mem_t       alt_mem;
    uint32          offset, phy_port, field_len;
    uint8           alt_acc_type;
    int             blk;
    soc_info_t      *si;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        if (mem != INVALIDm) {
            alt_mem = INVALIDm;
            switch(mem) {
#ifdef BCM_XGS_SWITCH_SUPPORT
            case L3_ENTRY_IPV4_UNICASTm:
            case L3_ENTRY_IPV4_MULTICASTm:
            case L3_ENTRY_IPV6_UNICASTm:
            case L3_ENTRY_IPV6_MULTICASTm:
                return soc_internal_l3x2_write(pcid_info, mem, address, data);
#ifdef BCM_TOMAHAWK_SUPPORT
            case EXACT_MATCH_2m:
            case EXACT_MATCH_4m:
                return soc_internal_exact_match_write(pcid_info, mem, address,
                                                      data);
#endif /* BCM_TOMAHAWK_SUPPORT */
            case VLAN_MACm:
                alt_mem = VLAN_XLATEm;
                break;
#endif
            default:
                break;
            }
            if (alt_mem != INVALIDm) {
                offset = address ^ SOC_MEM_INFO(pcid_info->unit, mem).base;
                mem = alt_mem;
                address = soc_mem_addr_get(pcid_info->unit, mem, 0, block, offset,
                                           &alt_acc_type);
                acc_type = alt_acc_type;
            }
            if (mem == EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm) {
                /* Update port mapping. */
                si = &SOC_INFO(pcid_info->unit);
                offset = address ^ SOC_MEM_INFO(pcid_info->unit, mem).base;
                soc_mem_field_get(pcid_info->unit, mem, data, DATAf, &phy_port);
                field_len = soc_mem_field_length(pcid_info->unit, mem, DATAf);
                if (phy_port != (1 << field_len) - 1) {
                    si->port_p2l_mapping[phy_port] = offset;
                    si->port_l2p_mapping[offset] = phy_port;
                    blk = SOC_PORT_BLOCK(pcid_info->unit, phy_port);
                    si->port_type[offset] =
                        SOC_BLOCK_INFO(pcid_info->unit, blk).type;
                    if (offset > si->port.max) {
                        si->port.max = offset;
                    }
                }
            }
            if (soc_mem_is_aggr(pcid_info->unit, mem)) {
                return soc_internal_extended_write_aggrmem(pcid_info, block,
                                                           acc_type, address,
                                                           data);
            }
        }
        if (acc_type >= 0) {
            block |= acc_type << 24;
        }
        return soc_datum_mem_write(pcid_info, block, address, data);
    } else {
        return soc_internal_write_mem(pcid_info, address, data);
    }
}

int
_soc_internal_extended_read_mem(pcid_info_t *pcid_info, soc_mem_t mem,
                                soc_block_t block, int acc_type,
                                uint32 address, uint32 *data)
{
    int             i, rv;
    soc_mem_t       alt_mem;
    uint32          offset;
    uint8           alt_acc_type;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        if (mem != INVALIDm) {
            alt_mem = INVALIDm;
            switch(mem) {
#ifdef BCM_XGS_SWITCH_SUPPORT
            case L3_ENTRY_IPV4_UNICASTm:
            case L3_ENTRY_IPV4_MULTICASTm:
            case L3_ENTRY_IPV6_UNICASTm:
            case L3_ENTRY_IPV6_MULTICASTm:
                return soc_internal_l3x2_read(pcid_info, mem, address, data);
#ifdef BCM_TOMAHAWK_SUPPORT
            case EXACT_MATCH_2m:
            case EXACT_MATCH_4m:
                return soc_internal_exact_match_read(pcid_info, mem, address,
                                                     data);
#endif /* BCM_TOMAHAWK_SUPPORT */
            case VLAN_MACm:
                alt_mem = VLAN_XLATEm;
                break;
#endif
            default:
                break;
            }
            if (alt_mem != INVALIDm) {
                offset = address ^ SOC_MEM_INFO(pcid_info->unit, mem).base;
                mem = alt_mem;
                address = soc_mem_addr_get(pcid_info->unit, mem, 0, block, offset,
                                           &alt_acc_type);
                acc_type = alt_acc_type;
            }
            if  (soc_mem_is_aggr(pcid_info->unit, mem)) {
                return soc_internal_extended_read_aggrmem(pcid_info, block, 
                                                          acc_type, address,
                                                          data);
            }
        }
        if (acc_type >= 0) {
            block |= acc_type << 24;
        }
        rv = soc_datum_mem_read(pcid_info, block, address, data);
        if (SOC_E_NOT_FOUND == rv) {
            /* Return null entry value if recognized, otherwise 0xf's */
            if (mem != INVALIDm) {
                if((NULL != soc_mem_entry_null(pcid_info->unit, mem))) {
                    memcpy(data,               /* Memory */
                           soc_mem_entry_null(pcid_info->unit, mem),
                           4 * soc_mem_entry_words(pcid_info->unit, mem));
                } else {
                    memset(data, 0, 4 * soc_mem_entry_words(pcid_info->unit, mem));
                }
            } else {                   /* Other */
                for (i = 0; i < SOC_MAX_MEM_WORDS; i++) {
                    data[i] = 0xffffffff;
                }
            }
            rv = SOC_E_NONE;
        }

        return rv;
    } else {
        return soc_internal_read_mem(pcid_info, address, data);
    }
}

int
soc_internal_extended_write_mem(pcid_info_t *pcid_info,
                                soc_block_t block, int acc_type,
                                uint32 address, uint32 *data)
{
    soc_mem_t       mem;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        if ((pcid_info->regmem_cb) 
            && (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_WRITE, address, data, 
                                     ((SOC_MAX_MEM_WORDS * 4) * 8)) == 0)) {
            return 0;
        }
    
        mem = soc_addr_to_mem_extended(pcid_info->unit, block, acc_type, address);
        return _soc_internal_extended_write_mem(pcid_info, mem, block,
                                                acc_type, address, data);
    } else {
        return soc_internal_write_mem(pcid_info, address, data);
    }
}

int
soc_internal_extended_read_mem(pcid_info_t *pcid_info,
                               soc_block_t block, int acc_type,
                               uint32 address, uint32 *data)
{
    soc_mem_t       mem;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */
        if ((pcid_info->regmem_cb) 
            && (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_READ, address, data, 
                                     ((SOC_MAX_MEM_WORDS * 4) * 8)) == 0)) {
            return 0;
        }

        mem = soc_addr_to_mem_extended(pcid_info->unit, block, acc_type, address);
        return _soc_internal_extended_read_mem(pcid_info, mem, block,
                                               acc_type, address, data);
    } else {
        return soc_internal_read_mem(pcid_info, address, data);
    }
}

int
soc_internal_extended_read_aggrmem(pcid_info_t *pcid_info,
                                   soc_block_t block, int acc_type,
                                   uint32 addr, uint32 *data)
{
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    soc_field_info_t *asflds; /* array for aggregate mem sorted fields */
    soc_field_info_t *sflds; /* array for partial mem sorted fields */
    uint8       acc_type_member;
    uint32      member_addr;
    int         unit = pcid_info->unit;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */
        mem = soc_addr_to_mem_extended(pcid_info->unit, block, acc_type, addr);
    
        if (mem == INVALIDm) {
            return SOC_E_PARAM;
        }
    
        offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0x3ffffff;
        index = 0;
        memset(data, 0, 4 * soc_mem_entry_words(unit, mem));
        do {
            m = SOC_MEM_AGGR(unit, index);
            if (m[0] == mem) {
                /* Fetch the individual components */
                nfield = 1;
                for(i = 1; m[i] != INVALIDm; i++) {
                    /*
                    ** Multiple entries compacted into one entry like HIT
                    ** requires special handling.
                    */
                    amemp = &SOC_MEM_INFO(unit, mem);
                    if(!soc_mem_index_max(unit, m[i]) || !soc_mem_index_max(unit, mem)) {
                        continue;
                    }
                    pck_factor = (soc_mem_index_max(unit, mem) /
                                  soc_mem_index_max(unit, m[i]));
                    assert(pck_factor != 0);
                    adj_offset = offset / pck_factor;
                    member_addr =
                        soc_mem_addr_get(unit, m[i], 0,
                                         SOC_MEM_BLOCK_ANY(unit, m[i]),
                                         adj_offset, &acc_type_member);
                    soc_internal_extended_read_mem(pcid_info, block,
                                                   acc_type_member,
                                                   member_addr, ent);
    
                    /* Iterate thru' the fields in this View */
                    memp = &SOC_MEM_INFO(unit, m[i]);
                    if (pck_factor == 1) {
                        for (f = memp->nFields - 1; f >= 0; f--) {
                            fieldp = &memp->fields[f];
                            soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                            if (soc_mem_field_valid(unit, mem, fieldp->field) &&
                                soc_mem_field_valid(unit, m[i], fieldp->field)) {
                                soc_mem_field_set(unit, mem, data, fieldp->field,
                                                  fv);
                            }
                            nfield++;
                        }
                    } else if (memp->nFields == pck_factor) {
                        asflds = sal_alloc(amemp->nFields *
                                           sizeof(soc_field_info_t),
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields *
                                          sizeof(soc_field_info_t),
                                          "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields,
                                                  amemp->nFields, asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &sflds[memp->nFields -
                                        (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds);
                        sal_free(sflds);
                    } else if (memp->nFields == (pck_factor * 2)) {
                        asflds = sal_alloc(amemp->nFields *
                                           sizeof(soc_field_info_t),
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields *
                                          sizeof(soc_field_info_t),
                                          "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields,
                                                  amemp->nFields, asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &sflds[memp->nFields -
                                        (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;

                        fieldp = &sflds[memp->nFields - pck_factor - 
                                    (offset % pck_factor) - 1];
                        soc_mem_field_get(unit, m[i], ent, fieldp->field, fv);
                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_set(unit, mem, data, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds);
                        sal_free(sflds);
                    } else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_internal_extended_read_aggrmem:mem=%d"
                                              "Unable to handle Aggregate Mem component %d\n"),
                                   mem, m[i]));
                    }
                }
                return(0);
            }
            index++;
        } while (m[0] != INVALIDm);
    
        /* Not found in the table of aggregate memories */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_internal_extended_read_aggrmem:mem=%d missing in Aggregate Mem List\n"),
                   mem));
        if((NULL != soc_mem_entry_null(pcid_info->unit, mem))) {
            memcpy(data,               /* Memory */
                   soc_mem_entry_null(pcid_info->unit, mem),
                   4 * soc_mem_entry_words(pcid_info->unit, mem));
        } else {
            memset(data, 0, 4 * soc_mem_entry_words(pcid_info->unit, mem));
        }

        return 0;
    } else {
        return soc_internal_read_aggrmem(pcid_info, addr, data);
    }
}

int
soc_internal_extended_write_aggrmem(pcid_info_t *pcid_info,
                                    soc_block_t block, int acc_type,
                                    uint32 addr, uint32 *data)
{
    uint32      ent[SOC_MAX_MEM_WORDS];
    uint32      fv[SOC_MAX_MEM_WORDS];
    soc_mem_t   *m = NULL;
    int         index, i;
    int         offset;
    int         adj_offset = 0;
    int         f;
    int         nfield;
    int         pck_factor;
    soc_mem_t   mem;
    soc_mem_info_t *amemp;
    soc_mem_info_t *memp;
    soc_field_info_t *fieldp;
    soc_field_info_t *asflds; /* array for aggregate mem sorted fields */
    soc_field_info_t *sflds; /* array for partial mem sorted fields */
    uint8       acc_type_member;
    uint32      member_addr;
    int         unit = pcid_info->unit;
        
    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */

        mem = soc_addr_to_mem_extended(pcid_info->unit, block, acc_type, addr);
    
        if (mem == INVALIDm)
        {
            return SOC_E_PARAM;
        }
    
        memset(ent, 0, 4 * soc_mem_entry_words(unit, mem));
        offset = (addr ^ SOC_MEM_INFO(unit, mem).base) & 0x3ffffff;
        index = 0;
        do {
            m = SOC_MEM_AGGR(unit, index);
            if (m[0] == mem) {
                /* Fetch the individual components */
                nfield = 1;
                for(i = 1; m[i] != INVALIDm; i++) {
                    /*
                    ** Multiple entries compacted into one entry like HIT
                    ** requires special handling.
                    */
                    amemp = &SOC_MEM_INFO(unit, mem);
                    if(!soc_mem_index_max(unit, m[i]) || !soc_mem_index_max(unit, mem)) {
                        continue;
                    }
                    pck_factor = (soc_mem_index_max(unit, mem) /
                                  soc_mem_index_max(unit, m[i]));
                    assert(pck_factor != 0);
                    adj_offset = offset / pck_factor;
                    memp = &SOC_MEM_INFO(unit, m[i]);
                    member_addr =
                        soc_mem_addr_get(unit, m[i], 0,
                                         SOC_MEM_BLOCK_ANY(unit, m[i]),
                                         adj_offset, &acc_type_member);

                    /* Iterate thru' the fields in this View */
                    if (pck_factor == 1) {
                        for (f = memp->nFields - 1; f >= 0; f--) {
                            fieldp = &memp->fields[f];
                            if (soc_mem_field_valid(unit, mem, fieldp->field) &&
                                soc_mem_field_valid(unit, m[i], fieldp->field)) {
                                soc_mem_field_get(unit, mem, data, fieldp->field,
                                                  fv);
                                soc_mem_field_set(unit, m[i], ent, fieldp->field,
                                                  fv);
                            }
                            nfield++;
                        }
                    } else if (memp->nFields == pck_factor) {
                        asflds = sal_alloc(amemp->nFields *
                                           sizeof(soc_field_info_t),
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields *
                                          sizeof(soc_field_info_t),
                                          "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields,
                                                  amemp->nFields, asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &asflds[amemp->nFields - nfield];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        soc_internal_extended_read_mem(pcid_info, block,
                                                       acc_type_member,
                                                       member_addr, ent);
                        fieldp = &sflds[memp->nFields -
                                        (offset % pck_factor) - 1];
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds);
                        sal_free(sflds);
                    } else if (memp->nFields == (pck_factor * 2)) {
                        asflds = sal_alloc(amemp->nFields *
                                           sizeof(soc_field_info_t),
                                           "aggregate fields");
                        if (!asflds) {
                            return SOC_E_INTERNAL;
                        }
                        sflds = sal_alloc(memp->nFields *
                                          sizeof(soc_field_info_t),
                                          "mem fields");
                        if (!sflds) {
                            sal_free(asflds);
                            return SOC_E_INTERNAL;
                        }
                        _soc_internal_fields_sort(amemp->fields,
                                                  amemp->nFields, asflds);
                        _soc_internal_fields_sort(memp->fields, memp->nFields,
                                                  sflds);
                        fieldp = &asflds[amemp->nFields - nfield];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        soc_internal_extended_read_mem(pcid_info, block,
                                                       acc_type_member,
                                                       member_addr, ent);
                        fieldp = &sflds[memp->nFields -
                                        (offset % pck_factor) - 1];
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;

                        fieldp = &asflds[(amemp->nFields - nfield)];
                        soc_mem_field_get(unit, mem, data, fieldp->field, fv);
                        fieldp = &sflds[memp->nFields - pck_factor -
                                        (offset % pck_factor) - 1];
                        soc_mem_field_set(unit, m[i], ent, fieldp->field, fv);
                        nfield++;
                        sal_free(asflds);
                        sal_free(sflds);
                    } else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_internal_extended_write_mem:mem=%d"
                                              "Unable to handle Aggregate Mem component %d\n"),
                                   mem, m[i]));
                    }
                    soc_internal_extended_write_mem
                        (pcid_info, block, acc_type_member, member_addr, ent);
                }
                return(0);
            }
            index++;
        } while (m[0] != INVALIDm);
    
        /* Not found in the table of aggregate memories */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_internal_write_aggrmem:mem=%d missing in Aggregate Mem List\n"),
                   mem));        
        return 0;
    } else {
        return soc_internal_write_aggrmem(pcid_info, addr, data);
    }
}

int
soc_internal_extended_read_bytes(pcid_info_t *pcid_info,
                                 soc_block_t block, int acc_type,
                                 uint32 address, uint8 *data, int len)
{
    uint32  temp[SOC_MAX_MEM_WORDS];
    int     rv;

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */
    
        if ((len < 0) || (len > (SOC_MAX_MEM_WORDS * (sizeof(uint32))))) {
            return 1;
        }
    
        /* reading bytes is supported only for bcmsim */
        if ((pcid_info->regmem_cb) && 
            (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_READ, address, 
                                  (uint32 *) data, (len * 8)) == 0)) {
            return 0;
        }
    
        /* if no registered call-back, default to read_mem */
        rv = soc_internal_extended_read_mem(pcid_info, block, acc_type, address, 
                                            temp);
        if (!rv) {
            sal_memcpy(data, temp, len);
        }
        return rv;
    } else {
        return soc_internal_read_bytes(pcid_info, address, data, len);
    }
}

int
soc_internal_extended_write_bytes(pcid_info_t *pcid_info,
                                  soc_block_t block, int acc_type,
                                  uint32 address, uint8 *data, int len)
{
    uint32  temp[SOC_MAX_MEM_WORDS];
    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        /* To fill in */

        if ((len < 0) || (len > (SOC_MAX_MEM_WORDS * (sizeof(uint32))))) {
            return 1;
        }
        
        /* writing bytes is supported only for bcmsim */
        if ((pcid_info->regmem_cb) && 
            (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_WRITE, address, 
                                  (uint32 *)data, (len * 8)) == 0)) {
            return 0;
        }
        
        /* if no registered call-back, default to write_mem */
        sal_memset(temp, 0, (SOC_MAX_MEM_WORDS * sizeof(uint32)));
        sal_memcpy(temp, data, len);
        return soc_internal_extended_write_mem(pcid_info, block, acc_type, 
                                               address, temp);
    } else {
        return soc_internal_write_bytes(pcid_info, address, data, len);
    }
}

int
soc_internal_extended_write_mem_index(pcid_info_t *pcid_info, soc_mem_t mem,
                                      int index, uint32 *data)
{
    int             block_info_index;
    int             block;
    uint8           acc_type;
    uint32          addr;

    block_info_index = SOC_MEM_BLOCK_ANY(pcid_info->unit, mem);
    block = SOC_BLOCK2SCH(pcid_info->unit, block_info_index);
    addr = soc_mem_addr_get(pcid_info->unit, mem, 0, block_info_index, index,
                            &acc_type);

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {    
        if (pcid_info->regmem_cb) {
            if (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_WRITE, addr,
                                     data, SOC_MAX_MEM_WORDS * 4 * 8) == 0) {
                return 0;
            }
        }
        return _soc_internal_extended_write_mem(pcid_info, mem, block,
                                                acc_type, addr, data);
    } else {
        return soc_internal_write_mem(pcid_info, addr, data);
    }
}

int
soc_internal_extended_read_mem_index(pcid_info_t *pcid_info, soc_mem_t mem,
                                     int index, uint32 *data)
{
    int             block_info_index;
    int             block;
    uint8           acc_type;
    uint32          addr;

    block_info_index = SOC_MEM_BLOCK_ANY(pcid_info->unit, mem);
    block = SOC_BLOCK2SCH(pcid_info->unit, block_info_index);
    addr = soc_mem_addr_get(pcid_info->unit, mem, 0, block_info_index, index,
                            &acc_type);

    if (SBUS_ADDR_VERSION_2 == pcid_info->sbus_addr_version) {
        if (pcid_info->regmem_cb != NULL) {
            if (pcid_info->regmem_cb(pcid_info, BCMSIM_SCHAN_MEM_READ, addr,
                                     data, SOC_MAX_MEM_WORDS * 4 * 8) == 0) {
                return 0;
            }
        }
        return _soc_internal_extended_read_mem(pcid_info, mem, block,
                                               acc_type, addr, data);
    } else {
        return soc_internal_read_mem(pcid_info, addr, data);
    }
}
