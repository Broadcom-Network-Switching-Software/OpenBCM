/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MAC driver support utilities. 
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/error.h>
#include <soc/portmode.h>
#include <soc/macutil.h>
#include <soc/phyctrl.h>
#include <soc/mem.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/td2_td2p.h>
#include <soc/esw/portctrl.h>
#endif
#if defined(BCM_KATANA_SUPPORT)
#include <soc/katana.h>
#endif
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif

#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined (BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#endif
#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
#include <soc/hurricane3.h>
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/greyhound2.h>
#endif
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif /* BCM_FIRELIGHT_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#include <soc/debug.h>

#if defined(BCM_TRIUMPH3_SUPPORT)
int ibod_recovery_in_progress[SOC_MAX_NUM_DEVICES];
int egress_drain_timeout_flag[SOC_MAX_NUM_DEVICES];
#endif

#if defined(BCM_KATANA2_SUPPORT)
#define _SOC_KT2_COSQ_MAX_INDEX_S0   128
#define _SOC_KT2_COSQ_MAX_INDEX_S1   256
#endif

#if defined (BCM_SABER2_SUPPORT)
#define _SOC_SB2_COSQ_MAX_INDEX_S0   16
#endif

#if defined(BCM_ESW_SUPPORT)

/*
 * Function:
 *      soc_egress_cell_count
 * Purpose:
 *      Return the approximate number of cells of packets pending
 *      in the MMU destined for a specified egress.
 */
int
soc_egress_cell_count(int unit, soc_port_t port, uint32 *count)
{
    uint32              val;
    int                 cos;
#if defined(BCM_TRIDENT_SUPPORT)
    int                 i;
#if defined(BCM_TRIDENT2_SUPPORT)
    soc_mem_t           mem;
    int                 index;
    uint32              entry[SOC_MAX_MEM_WORDS];
#if defined(BCM_TOMAHAWK_SUPPORT)
    soc_info_t          *si = &SOC_INFO(unit);
    soc_mem_t           base_mem;
    int                 pipe, xpe, base;
#endif /* BCM_TOMAHAWK_SUPPORT */
#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int itm, num_ucq, num_mcq, uc_base, mc_base;
#endif
    *count = 0;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56504:
    case SOC_CHIP_BCM56102:
    case SOC_CHIP_BCM56304:
    case SOC_CHIP_BCM56218:
    case SOC_CHIP_BCM56112:
    case SOC_CHIP_BCM56314:
    case SOC_CHIP_BCM56514:
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
    case SOC_CHIP_BCM56142:
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53540:
        for (cos = 0; cos < NUM_COS(unit); cos++) {
            SOC_IF_ERROR_RETURN(soc_reg_egress_cell_count_get(unit, port, cos, &val));
            *count += val;
        }
#if defined(BCM_HURRICANE1_SUPPORT)
       if (SOC_IS_HURRICANE(unit) &&
          (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port))) {
           if (SOC_REG_PORT_VALID(unit, XP_XBODE_CELL_CNTr, port)) {
               SOC_IF_ERROR_RETURN(
                       READ_XP_XBODE_CELL_CNTr(unit, port, &val));
               *count += soc_reg_field_get(
                       unit, XP_XBODE_CELL_CNTr, val, CELL_CNTf);
           }
           if (SOC_REG_PORT_VALID(unit, GE_GBODE_CELL_CNTr, port)) {
               SOC_IF_ERROR_RETURN(READ_GE_GBODE_CELL_CNTr(unit, port, &val));
               *count += soc_reg_field_get(
                       unit, GE_GBODE_CELL_CNTr, val, CELL_CNTf);
           }
       }
#endif
        break;
#if defined(BCM_GREYHOUND2_SUPPORT)
    case SOC_CHIP_BCM53570:
#if defined (BCM_FIRELIGHT_SUPPORT)
    case SOC_CHIP_BCM56070:
#endif /* BCM_FIRELIGHT_SUPPORT */
        for (cos = 0; cos < SOC_REG_NUMELS(unit, COSLCCOUNTr); cos++) {
            SOC_IF_ERROR_RETURN(READ_COSLCCOUNTr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, COSLCCOUNTr, val,
                                    LCCOUNTf);
        }
        for (cos = 0; cos < SOC_REG_NUMELS(unit, COSLCCOUNT_QGROUPr); cos++) {
            SOC_IF_ERROR_RETURN(READ_COSLCCOUNT_QGROUPr(unit, port, cos, &val));
            *count += soc_reg_field_get(unit, COSLCCOUNT_QGROUPr, val,
                                    LCCOUNTf);
        }
        break;
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_HERCULES_SUPPORT)
    case SOC_CHIP_BCM5675:
        
        break;
#endif /* BCM_HERCULES_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    case SOC_CHIP_BCM56800:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
    case SOC_CHIP_BCM88732:
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNTr(unit, port, &val));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNTr, val,
                                    OP_PORT_TOTAL_COUNTf);
        if (IS_GX_PORT(unit, port) && !SOC_IS_SHADOW(unit)) {
            SOC_IF_ERROR_RETURN(READ_XP_XBODE_CELL_CNTr(unit, port, &val));
            *count += soc_reg_field_get(unit, XP_XBODE_CELL_CNTr, val,
                                        CELL_CNTf);
            SOC_IF_ERROR_RETURN(READ_GE_GBODE_CELL_CNTr(unit, port, &val));
            *count += soc_reg_field_get(unit, GE_GBODE_CELL_CNTr, val,
                                        CELL_CNTf);
        }
        break;
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56334:
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &val));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNT_CELLr, val,
                                   OP_PORT_TOTAL_COUNT_CELLf);
        break;
    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56340:
        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &val));
        *count += soc_reg_field_get(unit, OP_PORT_TOTAL_COUNT_CELLr, val,
                                   TOTAL_COUNTf);
        break;
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    case SOC_CHIP_BCM56840:
        for (i = 0; i < SOC_INFO(unit).port_num_uc_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (READ_OP_UC_QUEUE_TOTAL_COUNT_CELLr(unit, port, i, &val));
            *count += soc_reg_field_get(unit, OP_UC_QUEUE_TOTAL_COUNT_CELLr,
                                        val, Q_TOTAL_COUNT_CELLf);
        }
        for (i = 0; i < SOC_INFO(unit).port_num_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (READ_OP_QUEUE_TOTAL_COUNT_CELLr(unit, port, i, &val));
            *count += soc_reg_field_get(unit, OP_QUEUE_TOTAL_COUNT_CELLr,
                                        val, Q_TOTAL_COUNT_CELLf);
        }
        if (SOC_INFO(unit).port_num_ext_cosq[port] > 0) {
            for (i = 0; i < SOC_INFO(unit).port_num_uc_cosq[port] +
                     SOC_INFO(unit).port_num_ext_cosq[port]; i++) {
                SOC_IF_ERROR_RETURN
                    (READ_OP_EX_QUEUE_TOTAL_COUNT_CELLr(unit, port, i, &val));
                *count += soc_reg_field_get(unit,
                                            OP_EX_QUEUE_TOTAL_COUNT_CELLr, val,
                                            Q_TOTAL_COUNT_CELLf);
            }
        }
        break;
#endif /* BCM_TRIDENT_SUPPORT */
    case SOC_CHIP_BCM56440:
    case SOC_CHIP_BCM56450:
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56270:
        
        *count = 0;
        break;
#if defined (BCM_TRIDENT2_SUPPORT)
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
        /* Unicast queues */
        index = SOC_INFO(unit).port_uc_cosq_base[port];
        if (index < 1480) {
            mem = MMU_THDU_XPIPE_COUNTER_QUEUEm;
        } else {
            index -= 1480;
            mem = MMU_THDU_YPIPE_COUNTER_QUEUEm;
        }
        for (i = 0; i < SOC_INFO(unit).port_num_uc_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index + i, entry));
            *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
            *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
        }

        /* Multicast queues */
        index = SOC_INFO(unit).port_cosq_base[port];
        if (index < 568) {
            mem = MMU_THDM_DB_QUEUE_COUNT_0m;
        } else {
            index -= 568;
            mem = MMU_THDM_DB_QUEUE_COUNT_1m;
        }
        for (i = 0; i < SOC_INFO(unit).port_num_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index + i, entry));
            *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
            *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
        }
        break;
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#if defined(BCM_TRIDENT3_SUPPORT)
	case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56770:
    case SOC_CHIP_BCM56370:
    case SOC_CHIP_BCM56470:
    case SOC_CHIP_BCM56275:
#endif
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56965:
    case SOC_CHIP_BCM56970:
        pipe = si->port_pipe[port];
        base = si->port_uc_cosq_base[port];

        /* Since TH has xpe architecture, go over all XPEs and pick the instance
         * which is valid for the given Mem/XPE/Pipe combination.
         */
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            /* UC queue Count */
            base_mem = MMU_THDU_COUNTER_QUEUEm;

            /* Get XPE/PIPE view from base mem */
            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);

            if (mem == INVALIDm) {
                /* If given XPE/PIPE combination is not valid, continue */
                continue;
            }
            for (i = 0; i < SOC_INFO(unit).port_num_uc_cosq[port]; i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + i, &entry));
                *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
                *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
            }
        }

        base = si->port_cosq_base[port];
        for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
            /* MC queue Count */
            base_mem = MMU_THDM_DB_QUEUE_COUNTm;

            /* xpe/pipe/mem check not performed again as it is similar to UC
             * queue layout.
             */

            /* Get XPE/PIPE view from base mem */
            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, xpe, pipe);

            if (mem == INVALIDm) {
                /* If given XPE/PIPE combination is not valid, continue */
                continue;
            }
            for (i = 0; i < SOC_INFO(unit).port_num_cosq[port]; i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + i, &entry));
                *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
                *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
            }
        }

        break;
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined (BCM_TOMAHAWK3_SUPPORT)
        case SOC_CHIP_BCM56980:
            num_ucq = _soc_th3_get_num_ucq(unit);
            num_mcq = _soc_th3_get_num_mcq(unit);
            SOC_IF_ERROR_RETURN(
                soc_th3_chip_queue_num_get(unit, port, 0, 0, &uc_base));
            SOC_IF_ERROR_RETURN(
                soc_th3_chip_queue_num_get(unit, port, 0, 1, &mc_base));
            for (itm = 0; itm < NUM_ITM(unit); itm++) {
                if (!soc_tomahawk3_itm_valid(unit, itm)) {
                    continue;
                }
                mem = SOC_MEM_UNIQUE_ACC_ITM(unit, MMU_THDO_COUNTER_QUEUEm, itm);
                for (i = 0; i < num_ucq; i++) {
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ALL, uc_base + i, &entry));
                    *count += soc_mem_field32_get(unit, mem, entry, TOTALCOUNTf);
                }
                for (i = 0; i< num_mcq; i++) {
                    SOC_IF_ERROR_RETURN(
                        soc_mem_read(unit, mem, MEM_BLOCK_ALL, mc_base + i, &entry));
                    *count += soc_mem_field32_get(unit, mem, entry, TOTALCOUNTf);
                }
            }
        break;
#endif
#if defined (BCM_APACHE_SUPPORT)
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56670:
        /* Unicast queues */
        index = SOC_INFO(unit).port_uc_cosq_base[port];
        mem = MMU_THDU_XPIPE_COUNTER_QUEUEm;
        for (i = 0; i < SOC_INFO(unit).port_num_uc_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index + i, entry));
            *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
            *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
        }

        /* Multicast queues */
        index = SOC_INFO(unit).port_cosq_base[port];
        mem = MMU_THDM_DB_QUEUE_COUNT_0m;
        for (i = 0; i < SOC_INFO(unit).port_num_cosq[port]; i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ANY, index + i, entry));
            *count += soc_mem_field32_get(unit, mem, entry, MIN_COUNTf);
            *count += soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);
        }
        break;
#endif /* BCM_APACHE_SUPPORT */

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

#if defined(BCM_TRIDENT2_SUPPORT)
int
_soc_egress_cell_check(int unit, soc_port_t port, int *empty)
{
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    case SOC_CHIP_BCM56860:
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    {
        int                 bit_pos, phy_port, mmu_port;
        uint64              rval64;
        if (SAL_BOOT_SIMULATION && !SAL_BOOT_QUICKTURN) {
            *empty = TRUE;
            break;
        }
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

        if (mmu_port <= 63) {
            SOC_IF_ERROR_RETURN(READ_MMU_ENQ_PORT_EMPTY_BMP0r(unit, &rval64));
        } else {
            SOC_IF_ERROR_RETURN(READ_MMU_ENQ_PORT_EMPTY_BMP1r(unit, &rval64));
            mmu_port = mmu_port - 64;
        }
        bit_pos = mmu_port & 0x3f;
        *empty = COMPILER_64_BITTEST(rval64, bit_pos);
        break;
    }
#if defined (BCM_APACHE_SUPPORT)
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56670:
    {
        int                 bit_pos, phy_port, mmu_port;
        uint64              rval64;
        if (SAL_BOOT_SIMULATION && !SAL_BOOT_QUICKTURN) {
            *empty = TRUE;
            break;
        }
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

        if (mmu_port <= 63) {
            SOC_IF_ERROR_RETURN(READ_MMU_ENQ_PORT_EMPTY_BMP0r(unit, &rval64));
        } else {
            SOC_IF_ERROR_RETURN(READ_MMU_ENQ_PORT_EMPTY_BMP1r(unit, &rval64));
            mmu_port = mmu_port - 64;
        }
        bit_pos = mmu_port & 0x3f;
        *empty = COMPILER_64_BITTEST(rval64, bit_pos);
        break;
    }
#endif /* BCM_APACHE_SUPPORT */

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
STATIC int
_soc_port_txfifo_cell_count(int unit, soc_port_t port, int *count)
{
    uint32 rval;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56340:
        SOC_IF_ERROR_RETURN(READ_PORT_TXFIFO_CELL_CNTr(unit, port, &rval));
        *count = soc_reg_field_get(unit, PORT_TXFIFO_CELL_CNTr, rval,
                                   CELL_CNTf);
        break;
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
        /* Should only be called by CMAC driver */
        SOC_IF_ERROR_RETURN(READ_CPORT_TXFIFO_CELL_CNTr(unit, port, &rval));
        *count = soc_reg_field_get(unit, CPORT_TXFIFO_CELL_CNTr, rval,
                                   CELL_CNTf);
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT */

STATIC int
soc_mmu_backpressure_clear(int unit, soc_port_t port) {
    uint64 rv;

    COMPILER_64_SET(rv, 0, 0);

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56840:
    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
    case SOC_CHIP_BCM56340:
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56770:
    case SOC_CHIP_BCM56965:
    case SOC_CHIP_BCM56970:
        SOC_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));
        break;
    case SOC_CHIP_BCM56370:
        SOC_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKP_64r(unit, port, rv));
        break;

    case SOC_CHIP_BCM56440:
        if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));
        }
        break;
#if defined (BCM_APACHE_SUPPORT)
    case SOC_CHIP_BCM56560:
        {
            soc_mem_t mem;
            soc_field_t field;
            static const soc_mem_t flowcontrol_mems[4] = {
                MMU_INTFI_S1_ST_TBm,
                MMU_INTFI_FC_ST_TBL0m, 
                MMU_INTFI_FC_ST_TBL1m, 
                MMU_INTFI_FC_ST_TBL2m 
            };

            static const soc_field_t flowcontrol_fields[4] = {
                COEFCf,
                FC_STf,
                FC_STf,
                FC_STf
            };
            uint32 *bmap;
            uint32 *bmap_list[SOC_APACHE_NODE_LVL_MAX - SOC_APACHE_NODE_LVL_S1];
            uint32 fval;
            mmu_intfi_s1_st_tb_entry_t rval; /* Can be used with all mems  */
            int lvl, index, word, bit, word_count, index_count, cindex;
            int bit_val, rv;
            bmap_list[0] = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[3] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

            for (lvl = 0; 
                    lvl < (SOC_APACHE_NODE_LVL_MAX - SOC_APACHE_NODE_LVL_S1);
                    lvl++) {
                bmap = bmap_list[lvl];
                mem = flowcontrol_mems[lvl];
                field = flowcontrol_fields[lvl];
                index_count = soc_mem_index_count(unit, mem) * 4;
                word_count = _SHR_BITDCLSIZE(index_count);

                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = (word * SHR_BITWID + bit) / 4;
                        cindex = (word * SHR_BITWID + bit) % 4;
                        bit_val = 1 << cindex;
                        /* Save the entry */
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                                &rval);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                        fval = soc_mem_field32_get(unit, mem, &rval, field);
                        /* Disable the flowcontrol */
                        fval = fval & (~bit_val);

                        soc_mem_field32_set(unit, mem, &rval, field, fval);
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                                &rval);
                        if (SOC_FAILURE(rv)) {
                            break;
                        }
                    }
                }

            }
            soc_mem_field32_set(unit, mem, &rval, field, 0);
            rv = soc_mem_write(unit, MMU_INTFI_PORT_COEFC_TBm, MEM_BLOCK_ANY, port,
                    &rval);
            if (SOC_FAILURE(rv)) {
                break;
            }

            SOC_IF_ERROR_RETURN(WRITE_XPORT_TO_MMU_BKPr(unit, port, 0));
            break;
        }
#endif /* BCM_APACHE_SUPPORT */
    default:
        break;
    }

    return SOC_E_NONE;
}

int
_soc_egress_metering_freeze(int unit, soc_port_t port, void **setting)
{
    int      rv;

    rv = SOC_E_NONE;

    SOC_EGRESS_METERING_LOCK(unit);

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
    case SOC_CHIP_BCM56142:
        {
            uint32 *rval32;

            rval32 = sal_alloc(sizeof(uint32), "shaper buffer");
            if (rval32 == NULL) {
                rv = SOC_E_MEMORY;
                break;
            }

            rv = READ_EGRMETERINGCONFIGr(unit, port, rval32);
            if (SOC_SUCCESS(rv)) {
                rv = WRITE_EGRMETERINGCONFIGr(unit, port, 0);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(rval32);
                break;
            }
            *setting = rval32;
            break;
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT || BCM_HURRICANE_SUPPORT */
#if defined (BCM_HURRICANE2_SUPPORT)
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53540:
        {
            int i;
            uint32 regval;
            uint32 *buffer;
            int count;

            /* buffer required = (X+1), for store original register setting.
             * MAXBUCKETCONFIGr : X elements for a port
             * EGRMETERINGCONFIG : 1 register value for a port
             */
            count = SOC_REG_NUMELS(unit, MAXBUCKETCONFIGr) + 1;
            buffer = sal_alloc(count * sizeof(uint32), "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _hu_freeze_abort;
            }
            sal_memset(buffer, 0, count);

            /* save the original configuration : MAXBUCKETCONFIGr */
            count = 0;
            for (i = 0; i < SOC_REG_NUMELS(unit, MAXBUCKETCONFIGr); i++) {
                rv = READ_MAXBUCKETCONFIGr(unit, port, i, &regval);
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    goto _hu_freeze_abort;
                }
                buffer[count] = regval;
                count ++;
                /* Disable egress metering for this port */
                soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_REFRESHf, 0);
                soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_THD_SELf, 0);
                rv = WRITE_MAXBUCKETCONFIGr(unit, port, i, regval);
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    goto _hu_freeze_abort;
                }
            }

            /* save the original configuration : EGRMETERINGCONFIGr */
            rv = READ_EGRMETERINGCONFIGr(unit, port, &regval);
            if (SOC_SUCCESS(rv)) {
                buffer[count] = regval;
                count ++;
                rv = WRITE_EGRMETERINGCONFIGr(unit, port, 0);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _hu_freeze_abort;
            }
            *setting = buffer;
_hu_freeze_abort:
            break;
        }
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    case SOC_CHIP_BCM53570:
#if defined (BCM_FIRELIGHT_SUPPORT)
    case SOC_CHIP_BCM56070:
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            int i;
            uint32 value;
            uint32 *buffer;
            int count;
            int qlayer_entry_count = 0;
            int qgroup_entry_count = 0;
            soc_mem_t config_mem = INVALIDm;
            uint32 entry[SOC_MAX_MEM_WORDS];
            int memidx;
            int is_64q_port;

#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_64q_port_check(unit, port, &is_64q_port);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_64q_port_check(unit, port, &is_64q_port);
            }
            if (SOC_FAILURE(rv)) {
                break;
            }

            if (is_64q_port) {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    qlayer_entry_count = SOC_FL_QLAYER_COSQ_PER_PORT_NUM;
                    qgroup_entry_count = SOC_FL_QGROUP_PER_PORT_NUM;
                }else
#endif
                {
                    qlayer_entry_count = SOC_GH2_QLAYER_COSQ_PER_PORT_NUM;
                    qgroup_entry_count = SOC_GH2_QGROUP_PER_PORT_NUM;
                }
            } else {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    qlayer_entry_count = SOC_FL_LEGACY_QUEUE_NUM;
                    qgroup_entry_count = 0;
                }else
#endif
                {
                    qlayer_entry_count = SOC_GH2_LEGACY_QUEUE_NUM;
                    qgroup_entry_count = 0;
                }
            }
            
            /* buffer required (X*2+Y*2+1) for store original setting.
             *   MMU port 0~57: X=8, Y=0
             *   MMU port 58-65 : X=64, Y=8
             * MMU_MAX_BUCKET_QLAYERm : 
             *   X entries for a port, 2 fields (REFRESHf, THD_SELf)stored
             * MMU_MAX_BUCKET_QGROUPm : Y entries for a port
             *   Y entries for a port, 2 fields (REFRESHf, THD_SELf)stored
             * EGRMETERINGCONFIG : 1 register value for a port
             */
            count = (qlayer_entry_count * 2) +  (qgroup_entry_count * 2) + 1;
            buffer = sal_alloc(count * sizeof(uint32), "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                break;
            }
            sal_memset(buffer, 0, count);

            /* save the original configuration : MMU_MAX_BUCKET_QLAYERm */
            count = 0;
            config_mem = MMU_MAX_BUCKET_QLAYERm;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_mmu_bucket_qlayer_index(unit, port, 0, &memidx);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_mmu_bucket_qlayer_index(unit, port, 0, &memidx);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                break;
            }
            for (i = memidx; i < qlayer_entry_count; i++) {
                rv = soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, 
                                  i, &entry);
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    break;
                }
                value = soc_mem_field32_get(unit, config_mem,
                                            &entry, REFRESHf);
                buffer[count] = value;
                count ++;
                value = soc_mem_field32_get(unit, config_mem,
                                            &entry, THD_SELf);
                buffer[count] = value;
                count ++;

                /* Disable egress metering for this port */
                soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, 0);
                soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, 0);
                rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, 
                                   i, &entry);
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    break;
                }
            }

            /* save the original configuration : MMU_MAX_BUCKET_QGROUPm */
            if (qgroup_entry_count) {
                config_mem = MMU_MAX_BUCKET_QGROUPm;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    rv = soc_fl_mmu_bucket_qgroup_index(unit, port, 0, &memidx);
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    rv = soc_gh2_mmu_bucket_qgroup_index(unit, port, 0, &memidx);
                }
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    break;
                }
                for (i = memidx; i < qgroup_entry_count; i++) {
                    rv = soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, 
                                  i, &entry);
                    if (SOC_FAILURE(rv)) {
                        sal_free(buffer);
                        break;
                    }
                    value = soc_mem_field32_get(unit, config_mem,
                                                &entry, REFRESHf);
                    buffer[count] = value;
                    count ++;
                    value = soc_mem_field32_get(unit, config_mem,
                                                &entry, THD_SELf);
                    buffer[count] = value;
                    count ++;

                    /* Disable egress metering for this port */
                    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, 0);
                    soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, 0);
                    rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, 
                                       i, &entry);
                    if (SOC_FAILURE(rv)) {
                        sal_free(buffer);
                        break;
                    }
                }
            }

            /* save the original configuration : EGRMETERINGCONFIGr */
            rv = READ_EGRMETERINGCONFIGr(unit, port, &value);
            if (SOC_SUCCESS(rv)) {
                buffer[count] = value;
                count ++;
                rv = WRITE_EGRMETERINGCONFIGr(unit, port, 0);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                break;
            }
            *setting = buffer;
            break;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56334:
        {
            uint64 *rval64, temp_rval64;

            rval64 = sal_alloc(sizeof(uint64), "shaper buffer");
            if (rval64 == NULL) {
                rv = SOC_E_MEMORY;
                break;
            }

            rv = READ_EGRMETERINGCONFIG_64r(unit, port, rval64);
            if (SOC_SUCCESS(rv)) {
                COMPILER_64_ZERO(temp_rval64);
                rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, temp_rval64);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(rval64);
                break;
            }
            *setting = rval64;
            break;
        }
    case SOC_CHIP_BCM56840:
        {
            uint64 zero_rval64;
            int count, index, queue_cnt;
            soc_info_t *si;
            uint64 *buffer;

            si = &SOC_INFO(unit);
            COMPILER_64_ZERO(zero_rval64);
            queue_cnt = si->port_num_uc_cosq[port] + si->port_num_cosq[port] +
                        si->port_num_ext_cosq[port];
            /* Port level + S2 + S3 + UCQ + MCQ + EXT_UCQ. */
            count = 1 + 4 + 4 + queue_cnt;

            buffer = sal_alloc(count * sizeof(mem_entry_t),
                               "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _trident_freeze_abort;
            }

            count = 0;
            /* Port level meter. */
            rv = READ_EGRMETERINGCONFIG_64r(unit, port, &buffer[count]);
            if (SOC_SUCCESS(rv)) {
                rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, zero_rval64);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _trident_freeze_abort;
            }
            count++;

            if (!IS_CPU_PORT(unit, port)) {
                /* S2 meter (including MC group). */
                for (index = 0; index < 4; index++) {
                    rv = READ_S2_MAXBUCKETCONFIG_64r(unit, port, index,
                                                     &buffer[count]);
                    if (SOC_SUCCESS(rv)) {
                        rv = WRITE_S2_MAXBUCKETCONFIG_64r(unit, port, index,
                                                          zero_rval64);
                    }
                    if (SOC_FAILURE(rv)) {
                        sal_free(buffer);
                        goto _trident_freeze_abort;
                    }
                    count++;
                }
                /* S3 meter. */
                for (index = 0; index < 4; index++) {
                    rv = READ_S3_MAXBUCKETCONFIG_64r(unit, port, index,
                                                     &buffer[count]);
                    if (SOC_SUCCESS(rv)) {
                        rv = WRITE_S3_MAXBUCKETCONFIG_64r(unit, port, index,
                                                          zero_rval64);
                    }
                    if (SOC_FAILURE(rv)) {
                        sal_free(buffer);
                        goto _trident_freeze_abort;
                    }
                    count++;
                }
            }

            /* Queue meter. */
            for (index = 0; index < queue_cnt; index++) {
                rv = READ_MAXBUCKETCONFIG_64r(unit, port, index,
                                              &buffer[count]);
                if (SOC_SUCCESS(rv)) {
                    rv = WRITE_MAXBUCKETCONFIG_64r(unit, port, index,
                                                   zero_rval64);
                }
                if (SOC_FAILURE(rv)) {
                    sal_free(buffer);
                    goto _trident_freeze_abort;
                }
                count++;
            }
            *setting = buffer;
_trident_freeze_abort:
            break;
        }
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56340:
        {
            uint64 rval64, temp_rval64;
            soc_mem_t mem;
            uint32 *bmap, *bmap_list[3];
            int lvl, index, word, bit, word_count, index_count, count;
            mem_entry_t *buffer;
            static const soc_mem_t shaper_mems[3] = {
                MMU_MTRO_L0_MEMm, MMU_MTRO_L1_MEMm, MMU_MTRO_L2_MEMm
            };

            bmap_list[0] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

            SOC_LLS_SCHEDULER_LOCK(unit);
            count = 1; /* One entry for port level */
            for (lvl = 0; lvl < 3; lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        count += _shr_popcount(bmap[word]);
                    }
                }
            }

            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                               "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _tr_freeze_abort;
            }

            /* Port level meter */
            rv = READ_EGRMETERINGCONFIG_64r(unit, port, &rval64);
            if (SOC_SUCCESS(rv)) {
                COMPILER_64_ZERO(temp_rval64);
                rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, temp_rval64);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _tr_freeze_abort;
            }
            sal_memcpy(&buffer[0].entry[0], &rval64, sizeof(uint64));

            /* L0, L1, L2 meter */
            count = 1;
            for (lvl = 0; lvl < 3; lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);

                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        /* Save the entry */
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                                          &buffer[count].entry);
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _tr_freeze_abort;
                        }
                        buffer[count].mem = mem;
                        buffer[count].index = index;
                        count++;
                        /* Disable the entry */
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                                           soc_mem_entry_null(unit, mem));
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _tr_freeze_abort;
                        }
                    }
                }
            }

            buffer[count].mem = INVALIDm;
            *setting = buffer;
_tr_freeze_abort:
            SOC_LLS_SCHEDULER_UNLOCK(unit);
            break;
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    case SOC_CHIP_BCM56560:
        {
            soc_mem_t mem;
            uint32 *bmap, 
                   *bmap_list[SOC_APACHE_NODE_LVL_MAX - SOC_APACHE_NODE_LVL_S1];
            int pipe, phy_port, mmu_port;
            int lvl, index, word, bit, word_count, index_count, count;
            mem_entry_t *buffer;
            static const int max_l1_table = 2;
            static const int max_l2_table = 8;
            static const soc_mem_t port_shaper_mems[1] = {
                MMU_MTRO_EGRMETERINGCONFIG_MEM_0m
            };
            static const soc_mem_t shaper_mems[][1] = {
                { MMU_MTRO_S1_MEM_0m},
                { MMU_MTRO_L0_MEM_0m},
                { MMU_MTRO_L1_MEM_0m},
                { MMU_MTRO_L2_MEM_0m}
            };
            static const soc_mem_t l1_shaper_mems[] = {
                MMU_MTRO_L1_MEM_0m,
                MMU_MTRO_L1_MEM_1m
            };
            static const soc_mem_t l2_shaper_mems[] = {
                MMU_MTRO_L2_MEM_0m,
                MMU_MTRO_L2_MEM_1m,
                MMU_MTRO_L2_MEM_2m,
                MMU_MTRO_L2_MEM_3m,
                MMU_MTRO_L2_MEM_4m,
                MMU_MTRO_L2_MEM_5m,
                MMU_MTRO_L2_MEM_6m,
                MMU_MTRO_L2_MEM_7m
            };

            pipe = 0;
            bmap_list[0] = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[3] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

            SOC_LLS_SCHEDULER_LOCK(unit);
            count = 1; /* One entry for port level */
            for (lvl = 0; 
                    lvl < (SOC_APACHE_NODE_LVL_MAX - SOC_APACHE_NODE_LVL_S1);
                    lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl][pipe];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        count += _shr_popcount(bmap[word]);
                    }
                }
            }

            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                               "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _ap_freeze_abort;
            }

            count = 0;

            /* Port level meter */
            mem = port_shaper_mems[pipe];
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            index = mmu_port & 0x7f;
            /* Save the entry */
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                              &buffer[count].entry);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ap_freeze_abort;
            }
            buffer[count].mem = mem;
            buffer[count].index = index;
            count++;
            /* Disable the entry */
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                               soc_mem_entry_null(unit, mem));
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ap_freeze_abort;
            }

            /*S1, L0, L1, L2 meter */
            mem = shaper_mems[0][pipe];
            for (lvl = 0; lvl < (SOC_APACHE_NODE_LVL_MAX - SOC_APACHE_NODE_LVL_S1);
                    lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl][pipe];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);

                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        if ((SOC_APACHE_NODE_LVL_L1 - SOC_APACHE_NODE_LVL_S1) == lvl) {
                            mem = l1_shaper_mems[index % max_l1_table];
                            index /= max_l1_table;
                        } else if ((SOC_APACHE_NODE_LVL_L2 - SOC_APACHE_NODE_LVL_S1) == lvl) {
                            mem = l2_shaper_mems[index % max_l2_table];
                            index /= max_l2_table;
                        }
                        /* Save the entry */
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                                          &buffer[count].entry);
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _ap_freeze_abort;
                        }
                        buffer[count].mem = mem;
                        buffer[count].index = index;
                        count++;
                        /* Disable the entry */
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                                           soc_mem_entry_null(unit, mem));
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _ap_freeze_abort;
                        }
                    }
                }
            }
            buffer[count].mem = INVALIDm;
            *setting = buffer;
_ap_freeze_abort:
            SOC_LLS_SCHEDULER_UNLOCK(unit);
            break;
        }
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
        {
            soc_info_t *si;
            soc_mem_t mem;
            uint32 *bmap, *bmap_list[3];
            int pipe, phy_port, mmu_port;
            int lvl, index, word, bit, word_count, index_count, count;
            mem_entry_t *buffer;
            static const soc_mem_t port_shaper_mems[2] = {
                MMU_MTRO_EGRMETERINGCONFIG_MEM_0m,
                MMU_MTRO_EGRMETERINGCONFIG_MEM_1m
            };
            static const soc_mem_t shaper_mems[][2] = {
                { MMU_MTRO_L0_MEM_0m, MMU_MTRO_L0_MEM_1m },
                { MMU_MTRO_L1_MEM_0m, MMU_MTRO_L1_MEM_1m },
                { MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m }
            };

            si = &SOC_INFO(unit);
            pipe = SOC_PBMP_MEMBER(si->ypipe_pbm, port) ? 1 : 0;
            bmap_list[0] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
            if ((bmap_list[0] == NULL) ||
                (bmap_list[1] == NULL) ||
                (bmap_list[2] == NULL)) {
                rv =  SOC_E_RESOURCE;
                goto _td_freeze_abort;
            }
            SOC_LLS_SCHEDULER_LOCK(unit);
            count = 1; /* One entry for port level */
            for (lvl = 0; lvl < 3; lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl][pipe];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        count += _shr_popcount(bmap[word]);
                    }
                }
            }

            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                               "shaper buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _td_freeze_abort;
            }

            count = 0;

            /* Port level meter */
            mem = port_shaper_mems[pipe];
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            index = mmu_port & 0x3f;
            /* Save the entry */
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                              &buffer[count].entry);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _td_freeze_abort;
            }
            buffer[count].mem = mem;
            buffer[count].index = index;
            count++;
            /* Disable the entry */
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                               soc_mem_entry_null(unit, mem));
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _td_freeze_abort;
            }

            /* L0, L1, L2 meter */
            mem = shaper_mems[0][pipe];
            for (lvl = 0; lvl < 3; lvl++) {
                bmap = bmap_list[lvl];
                mem = shaper_mems[lvl][pipe];
                index_count = soc_mem_index_count(unit, mem);
                word_count = _SHR_BITDCLSIZE(index_count);

                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        /* Save the entry */
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                                          &buffer[count].entry);
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _td_freeze_abort;
                        }
                        buffer[count].mem = mem;
                        buffer[count].index = index;
                        count++;
                        /* Disable the entry */
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, index,
                                           soc_mem_entry_null(unit, mem));
                        if (SOC_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _td_freeze_abort;
                        }
                    }
                }
            }
            buffer[count].mem = INVALIDm;
            *setting = buffer;
_td_freeze_abort:
            SOC_LLS_SCHEDULER_UNLOCK(unit);
            break;
        }
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
    case SOC_CHIP_BCM56440:
        {
            soc_mem_t mem;
            uint32 *bmap, *bmap_list[3];
            int word_count, wdc[3], blk_size=0;
            int lvl, j, index, word, bit, index_count, count;
            int idx, upper_flag = 0;
            int prev_idx, prev_upper_flag;
            mem_entry_t *buffer;
            uint32 mant, cycle, f_idx;
            uint32 entry[SOC_MAX_MEM_WORDS];
            uint16 dev_id;
            uint8 rev_id;

            static const soc_mem_t cfg_mems[][2] = {
                { LLS_L0_SHAPER_CONFIG_Cm, LLS_L0_MIN_CONFIG_Cm },
                { LLS_L1_SHAPER_CONFIG_Cm, LLS_L1_MIN_CONFIG_Cm },
                { LLS_L2_SHAPER_CONFIG_LOWERm, LLS_L2_MIN_CONFIG_LOWER_Cm },
                { LLS_L2_SHAPER_CONFIG_UPPERm, LLS_L2_MIN_CONFIG_UPPER_Cm },
            };


            static const soc_field_t rate_mant_fields[] = {
               C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
               C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
               C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
               C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
            };

            static const soc_field_t rate_exp_fields[] = {
               C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
               C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
               C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
               C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
            };
            static const soc_field_t burst_exp_fields[] = {
               C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
               C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
               C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
               C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
            };
            static const soc_field_t burst_mant_fields[] = {
               C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
               C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
               C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
               C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
            };

            static const soc_field_t cycle_sel_fields[] = {
                C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
                C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
                C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
                C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
            };

            bmap_list[0] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

            count = 0; /* One entry for port level */
            for (lvl = 0; lvl < 3; lvl++) {
                bmap = bmap_list[lvl];
                mem = cfg_mems[lvl][0];
                index_count = soc_mem_index_count(unit, mem);

                /*
                 * Allocation done using LLS_LX_PARENT
                 * reading also done using same memories
                 */
                if (lvl == 0) {
                    index_count = soc_mem_index_count(unit, LLS_L0_PARENTm);
                } else if (lvl == 1) {
                    index_count = soc_mem_index_count(unit, LLS_L1_PARENTm);
                } else if (lvl == 2) {
                    index_count = soc_mem_index_count(unit, LLS_L2_PARENTm);
                }

                wdc[lvl] = _SHR_BITDCLSIZE(index_count);
                word_count = wdc[lvl];
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        count += _shr_popcount(bmap[word]);
                    }
                }
            }

            count = (count * 8) + 1;

            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                               "shaper buffer");

            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _ka_freeze_abort;
            }

            count = 0;
            /* port */
            mem = LLS_PORT_SHAPER_CONFIG_Cm;
            index = port & 0x3f;

            /* Save the entry */
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                              &buffer[count].entry);

            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ka_freeze_abort;
            }
            buffer[count].mem = mem;
            buffer[count].index = index;
            count++;
            /* Set the threshold for the port to max shaper rate */
            rv = soc_kt_cosq_max_bucket_set(unit, port, index,
                                            _SOC_KT_COSQ_NODE_LEVEL_ROOT);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ka_freeze_abort;
            }

            /* L0, L1, L2 meter */
            for (lvl = 0; lvl < 3; lvl++) {
                idx = upper_flag = 0;
                prev_idx = prev_upper_flag = -1;
                blk_size = (lvl == 2) ? 8: ((lvl ==1)? 4:1);
                bmap = bmap_list[lvl];
                word_count = wdc[lvl];
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        idx = index / blk_size;

                        for (j = 0; j < 2; j++) {
                            mem = cfg_mems[lvl][j];
                            if (lvl == 2) {
                                if ((index % 8) >= 4) {
                                  mem = cfg_mems[lvl+1][j];
                                  upper_flag = 1;
                                } else {
                                  upper_flag = 0;
                                }
                            }

                            /* Store the Threshold MIN/MAX table entries only
                             * once per table index as for the
                             * next consecutive L1/L2 indices falling on
                             * to the same table index need not be stored
                             */
                            if ((prev_idx != idx) ||
                              ((lvl == 2) && (prev_upper_flag != upper_flag))) {
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                                                  &buffer[count].entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _ka_freeze_abort;
                                }
                                buffer[count].mem = mem;
                                buffer[count].index = idx;
                                count++;
                            }

                            if (lvl == 2){
                                /* Set the  MIN/MAX thresholds for L2 node
                                 * to 2 Kbps
                                 */
                                mant=1;
                                /* For 56440_A0 minimum and maximum shapers
                                 * should be programmed to use a common value
                                 * for C_MIN_CYCLE_SEL and C_MAX_CYCLE_SEL
                                 * fields and the recommended value is 0x0.
                                 */
                                soc_cm_get_id(unit, &dev_id, &rev_id);
                                cycle = (rev_id == BCM56440_A0_REV_ID) ? 0 : 4;
                                f_idx = (j * 4) + (index % 4);
                                sal_memset(entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                                      idx, &entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _ka_freeze_abort;
                                }

                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_mant_fields[f_idx], mant);
                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_exp_fields[f_idx], 0);
                                soc_mem_field32_set(unit, mem, &entry,
                                     burst_exp_fields[f_idx], 0);
                                soc_mem_field32_set(unit, mem, &entry,
                                     burst_mant_fields[f_idx], 0);
                                soc_mem_field32_set(unit, mem, &entry,
                                     cycle_sel_fields[f_idx], cycle);
                                rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                                   idx, &entry);
                            } else if ((lvl != 2) && (j == 1)) {
                                /* Set the MIN/MAX thresholds for L0/L1 node
                                 * to max shaper rate
                                 */
                                rv = soc_kt_cosq_max_bucket_set(unit, port,
                                                                index, (lvl + 1));
                            }
                            if (SOC_FAILURE(rv)) {
                                sal_free(buffer);
                                goto _ka_freeze_abort;
                            }
                        }/*END of for j */
                        prev_idx = idx;
                        prev_upper_flag = upper_flag;
                    } /* END of the valid bit */
                } /* END of the word */
            } /* END of lvl */

            buffer[count].mem = INVALIDm;
            *setting = buffer;
_ka_freeze_abort:
            break;
        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    case SOC_CHIP_BCM56450:
        {
            soc_mem_t mem;
            uint32 *bmap, *bmap_list[6];
            int word_count, wdc[6], blk_size=0;
            int lvl, j, index, word, bit, index_count, count;
            int idx, upper_flag = 0;
            int prev_idx, prev_upper_flag;
            mem_entry_t *buffer;
            uint32 mant = 0, cycle = 0 , f_idx;
            uint32 entry[SOC_MAX_MEM_WORDS];
            uint32 lvl_count = 0;
            uint32 min = 0;  
            uint32 exp = 0 ;
            uint32 sched_mode = 0; 
            int wt = 0 ;
            uint32 burst_mant = 0; 
            uint32 burst_exp = 0; 
            soc_info_t *si;
            static const soc_mem_t parent_mems[] = {
               0,
               LLS_L0_PARENTm,
               LLS_L0_PARENTm,
               LLS_L0_PARENTm,
               LLS_L1_PARENTm,
               LLS_L2_PARENTm
            };

            static const soc_mem_t cfg_mems[][2] = {
                { LLS_PORT_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_S0_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_S1_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_L0_SHAPER_CONFIG_Cm, LLS_L0_MIN_CONFIG_Cm },
                { LLS_L1_SHAPER_CONFIG_Cm, LLS_L1_MIN_CONFIG_Cm },
                { LLS_L2_SHAPER_CONFIG_LOWERm, LLS_L2_MIN_CONFIG_LOWER_Cm },
                { LLS_L2_SHAPER_CONFIG_UPPERm, LLS_L2_MIN_CONFIG_UPPER_Cm },
            };

            static const soc_field_t rate_mant_fields[] = {
               C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
               C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
               C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
               C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
            };

            static const soc_field_t rate_exp_fields[] = {
               C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
               C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
               C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
               C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
            };
            static const soc_field_t burst_exp_fields[] = {
               C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
               C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
               C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
               C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
            };
            static const soc_field_t burst_mant_fields[] = {
               C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
               C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
               C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
               C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
            };

            static const soc_field_t cycle_sel_fields[] = {
                C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
                C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
                C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
                C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
            };

            bmap_list[0] = NULL;
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_s0_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
            bmap_list[3] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[4] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[5] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

            si = &SOC_INFO(unit);
            /*
             * On KTX we can have both s0 and s1 node or only
             * s1 node but can not have only s0 node
             */
            bmap = bmap_list[2];
            index_count = _SOC_KT2_COSQ_MAX_INDEX_S1;
            word_count = _SHR_BITDCLSIZE(index_count);
            for (word = 0; word < word_count; word++) {
                 if (bmap[word] != 0) {
                     break;
                 }
            }
            if (word == word_count) {
               /* s1 list is zero */
               bmap =  bmap_list[1];
               index_count = _SOC_KT2_COSQ_MAX_INDEX_S0;
               word_count = _SHR_BITDCLSIZE(index_count);
               for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        break;
                    }
               }
               if (word < word_count) {
                   /* s0 list is non zero */
                   /* swap  s0 ans s1 list */
                   bmap = bmap_list[2];
                   bmap_list[2] = bmap_list[1];
                   bmap_list[1] = bmap;
               }
            }
            count = 0;
            /* Count the number of the valid Root,S0,S1
             * L0/L1/L2 entries per port.
             * To allocate the buffer to store the Min, Max shaper
             * entries for the valid indices
             */
            for (lvl = 0; lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
                lvl_count = 0;
                if (lvl == _SOC_KT2_COSQ_NODE_LVL_ROOT) {
                    /* Port entry */
                    lvl_count = 1;
                    wdc[lvl] = 0;
                } else {
                    bmap = bmap_list[lvl];
                    mem = parent_mems[lvl];
                    index_count = soc_mem_index_count(unit, mem);

                    if (lvl == _SOC_KT2_COSQ_NODE_LVL_S0) {
                        index_count = _SOC_KT2_COSQ_MAX_INDEX_S0;
                    } else
                    if (lvl == _SOC_KT2_COSQ_NODE_LVL_S1) {
                        index_count =_SOC_KT2_COSQ_MAX_INDEX_S1;
                    }
                    wdc[lvl] = _SHR_BITDCLSIZE(index_count);
                    word_count = wdc[lvl];

                    for (word = 0; word < word_count; word++) {
                        if (bmap[word] != 0) {
                            lvl_count += _shr_popcount(bmap[word]);
                        }
                    }
                    if (lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) {
                        /* for MIN/MAX  shaper config */
                        /* count allocation is based on worst case
                         * estimate that entries might fall into different
                         * table entries
                         */
                        lvl_count = (lvl_count * 2);
                    }
                }
                count += lvl_count;
            }
            count = count + 1;


            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                          "shaper buffer");

            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto _ka2_freeze_abort;
            }
            count = 0;
            /* port */
            mem = LLS_PORT_SHAPER_CONFIG_Cm;
            index = port & 0x3f;

            /* Save the entry */
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                              &buffer[count].entry);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ka2_freeze_abort;
            }
            buffer[count].mem = mem;
            buffer[count].index = index;
            count++;

            /* Set the threshold for the port to max shaper rate */
            rv = soc_kt2_cosq_max_bucket_set(unit, port, index,
                                            _SOC_KT2_COSQ_NODE_LVL_ROOT);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto _ka2_freeze_abort;
            }

            /* S0, S1, L0, L1, L2 meter */
            for (lvl = _SOC_KT2_COSQ_NODE_LVL_S0;
                 lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
                idx = upper_flag = 0;
                prev_idx = prev_upper_flag = -1;
                blk_size = ((lvl == _SOC_KT2_COSQ_NODE_LVL_L2) ? 8:
                            ((lvl == _SOC_KT2_COSQ_NODE_LVL_L1)? 4: 1));
                bmap = bmap_list[lvl];
                word_count = wdc[lvl];
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        idx = index / blk_size;

                        for (j = 0; j < 2; j++) {
                            mem = cfg_mems[lvl][j];

                            if (mem == INVALIDm) {
                                continue;
                            }

                            if (lvl == _SOC_KT2_COSQ_NODE_LVL_L2) {
                                if ((index % 8) >= 4) {
                                  mem = cfg_mems[lvl+1][j];
                                  upper_flag = 1;
                                } else {
                                  upper_flag = 0;
                                }
                            }

                            /* Store the Threshold MIN/MAX table entries only
                             * once per table index as for the
                             * next consecutive L1/L2 indices falling on
                             * to the same table index need not be stored
                             */
                            if ((prev_idx != idx) ||
                                ((lvl == _SOC_KT2_COSQ_NODE_LVL_L2) &&
                                (prev_upper_flag != upper_flag))) {
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                                                  &buffer[count].entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _ka2_freeze_abort;
                                }
                                buffer[count].mem = mem;
                                buffer[count].index = idx;
                                count++;
                            }

                            if (lvl == _SOC_KT2_COSQ_NODE_LVL_L2){
                                if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
                                    /* coverity[leaked_storage] */ 
                                    rv = soc_kt2_cosq_get_sched_mode(unit, port, lvl,
                                                    index, &sched_mode, &wt);
                                } else {
                                    /* coverity[leaked_storage] */ 
                                    /* coverity[resource_leak : FALSE] */
                                    rv = soc_kt2_cosq_get_sched_mode(unit, port, lvl - 2,
                                                    index, &sched_mode, &wt);
                                }
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _ka2_freeze_abort;
                                }

                                if ( sched_mode == BCM_COSQ_STRICT ) 
                                {
                                     rv = _soc_kt2_cosq_min_bucket_get(unit, port,
                                                                  index, lvl,
                                                                  &min, &mant,
                                                                  &exp, &cycle);
                                     if (SOC_FAILURE(rv)) {
                                        sal_free(buffer);
                                        goto _ka2_freeze_abort;
                                     }
                                     if (min) { 
                                          if (exp != 0) {
                                              if (exp < 10) {
                                                  burst_exp = 5;
                                                  burst_mant = 0;
                                              } else {
                                                  burst_exp = exp - 4;
                                                  burst_mant = (mant >> 3);
                                              }
                                           }
                                           else { 
                                           burst_mant = 0;
                                           burst_exp = 0;
                                           } 
                                     }
                                }
                                if (!min) 
                                {
                                  /* Set the  MIN/MAX thresholds for L2 node
                                   * to 2 Kbps
                                   */
                                  mant = 1;
                                  cycle = 4;
                                  exp = 0; 
                                }
                                f_idx = (j * 4) + (index % 4);
                                sal_memset(entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                                    idx, &entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _ka2_freeze_abort;
                                }

                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_mant_fields[f_idx], mant);
                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_exp_fields[f_idx], exp);
                                soc_mem_field32_set(unit, mem, &entry,
                                     burst_exp_fields[f_idx], burst_exp);
                                soc_mem_field32_set(unit, mem, &entry,
                                   burst_mant_fields[f_idx], burst_mant);
                                soc_mem_field32_set(unit, mem, &entry,
                                     cycle_sel_fields[f_idx], cycle);
                                rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                                 idx, &entry);
                                 min = 0;
                                 sched_mode = 0; 
                                 cycle = 0;
                                 exp = 0; 
                                 mant = 0;
                                 burst_exp = 0;
                                 burst_mant = 0;
                              } else if (((lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) && (j == 1)) ||
                                       (lvl < _SOC_KT2_COSQ_NODE_LVL_L0)) {
                                /* Set the MIN/MAX thresholds for L0/L1 node
                                 * to max shaper rate
                                 */
                                rv = soc_kt2_cosq_max_bucket_set(unit, port,
                                                                index, lvl);
                            }
                            if (SOC_FAILURE(rv)) {
                                sal_free(buffer);
                                goto _ka2_freeze_abort;
                            }
                        }/*END of for j */
                        prev_idx = idx;
                        prev_upper_flag = upper_flag;
                    } /* END of the valid bit */
                } /* END of the word */
            } /* END of lvl */

            buffer[count].mem = INVALIDm;
            *setting = buffer;
_ka2_freeze_abort:
            break;
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56270:
        {
            soc_mem_t mem;
            uint32 *bmap, *bmap_list[6];
            int word_count, wdc[6], blk_size=0;
            int lvl, j, index, word, bit, index_count, count;
            int idx;
            int prev_idx;
            mem_entry_t *buffer;
            uint32 mant=0, cycle=0, f_idx;
            uint32 entry[SOC_MAX_MEM_WORDS];
            uint32 lvl_count = 0;
            uint32 min = 0;
            uint32 exp = 0;
            uint32 sched_mode = 0;
            int wt = 0;
            uint32 burst_mant = 0;
            uint32 burst_exp = 0;
            soc_info_t *si;
            static const soc_mem_t parent_mems[] = {
               0,
               LLS_L0_PARENTm,
               LLS_L0_PARENTm,
               LLS_L0_PARENTm,
               LLS_L1_PARENTm,
               LLS_L2_PARENTm
            };

            static const soc_mem_t cfg_mems[][2] = {
                { LLS_PORT_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_S0_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_S1_SHAPER_CONFIG_Cm, INVALIDm },
                { LLS_L0_SHAPER_CONFIG_Cm, LLS_L0_MIN_CONFIG_Cm },
                { LLS_L1_SHAPER_CONFIG_Cm, LLS_L1_MIN_CONFIG_Cm },
                { LLS_L2_SHAPER_CONFIG_Cm, LLS_L2_MIN_CONFIG_Cm },
            };
            static const soc_field_t rate_mant_fields[] = {
                C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
                C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
                C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
                C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
            };

            static const soc_field_t rate_exp_fields[] = {
                C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
                C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
                C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
                C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
            };
            static const soc_field_t burst_exp_fields[] = {
                C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
                C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
                C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
                C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
            };
            static const soc_field_t burst_mant_fields[] = {
                C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
                C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
                C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
                C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
            };

            static const soc_field_t cycle_sel_fields[] = {
                C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
                C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
                C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
                C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
            };

            si = &SOC_INFO(unit);
            bmap_list[0] = NULL;
            bmap_list[1] = SOC_CONTROL(unit)->port_lls_s0_bmap[port];
            bmap_list[2] = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
            bmap_list[3] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            bmap_list[4] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            bmap_list[5] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
            /* 
             * On KTX we can have both s0 and s1 node or only 
             * s1 node but can not have only s0 node 
             */
            SOC_LLS_SCHEDULER_LOCK(unit);
            bmap = bmap_list[2];
            index_count = soc_mem_index_count(unit, parent_mems[2]);
            word_count = _SHR_BITDCLSIZE(index_count);
            for (word = 0; word < word_count; word++) {
                 if (bmap[word] != 0) {
                     break;
                 }
            }
            if (word == word_count) {
               /* s1 list is zero */
               bmap =  bmap_list[1];
               for (word = 0; word < word_count; word++) {
                    if (bmap[word] != 0) {
                        break;
                    }
               }
               if (word < word_count) {
                   /* s0 list is non zero */
                   /* swap  s0 ans s1 list */
                   bmap = bmap_list[2];
                   bmap_list[2] = bmap_list[1];
                   bmap_list[1] = bmap;
               }
            } 
            count = 0;
            /* Count the number of the valid Root,S0,S1
             * L0/L1/L2 entries per port.
             * To allocate the buffer to store the Min, Max shaper
             * entries for the valid indices
             */
            for (lvl = 0; lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
                lvl_count = 0;
                if (lvl == _SOC_KT2_COSQ_NODE_LVL_ROOT) {
                    /* Port entry */
                    lvl_count = 1;
                    wdc[lvl] = 0;
                } else {
                    bmap = bmap_list[lvl];
                    mem = parent_mems[lvl];
                    index_count = soc_mem_index_count(unit, mem);
                    if (lvl == _SOC_KT2_COSQ_NODE_LVL_S0) {
                        index_count = _SOC_SB2_COSQ_MAX_INDEX_S0;
                    }
                    wdc[lvl] = _SHR_BITDCLSIZE(index_count);
                    word_count = wdc[lvl];

                    for (word = 0; word < word_count; word++) {
                        if (bmap[word] != 0) {
                            lvl_count += _shr_popcount(bmap[word]);
                        }
                    }
                    if (lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) {
                        /* for MIN/MAX  shaper config */
                        /* count allocation is based on worst case
                         * estimate that entries might fall into different
                         * table entries
                         */
                        lvl_count = (lvl_count * 2);
                    }
                }
                count += lvl_count;
            }
            count = count + 1;


            buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
                          "shaper buffer");

            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                goto  _sb2_freeze_abort; 
            }
            count = 0;
            /* port */
            mem = LLS_PORT_SHAPER_CONFIG_Cm;
            index = port & 0x1f;

            /* Save the entry */
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
                              &buffer[count].entry);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto  _sb2_freeze_abort; 
            }
            buffer[count].mem = mem;
            buffer[count].index = index;
            count++;

            /* Set the threshold for the port to max shaper rate */
            rv = soc_sb2_cosq_max_bucket_set(unit, port, index,
                                            _SOC_KT2_COSQ_NODE_LVL_ROOT);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                goto  _sb2_freeze_abort; 
            }

            /* S0, S1, L0, L1, L2 meter */
            for (lvl = _SOC_KT2_COSQ_NODE_LVL_S0;
                 lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
                idx = 0;
                prev_idx = -1;
                blk_size = ((lvl == _SOC_KT2_COSQ_NODE_LVL_L2) ? 4: 1);
                bmap = bmap_list[lvl];
                word_count = wdc[lvl];
                for (word = 0; word < word_count; word++) {
                    if (bmap[word] == 0) {
                        continue;
                    }
                    for (bit = 0; bit < SHR_BITWID; bit++) {
                        if (!(bmap[word] & (1 << bit))) {
                            continue;
                        }
                        index = word * SHR_BITWID + bit;
                        idx = index / blk_size;
                        if ((lvl == _SOC_KT2_COSQ_NODE_LVL_S0) &&
                            (idx >=  _SOC_SB2_COSQ_MAX_INDEX_S0)) {
                            continue;
                        }

                        for (j = 0; j < 2; j++) {
                            mem = cfg_mems[lvl][j];

                            if (mem == INVALIDm || (!SOC_MEM_IS_VALID(unit, mem))) {
                                continue;
                            }

                            /* Store the Threshold MIN/MAX table entries only
                             * once per table index as for the
                             * next consecutive L1/L2 indices falling on
                             * to the same table index need not be stored
                             */
                            if (prev_idx != idx) { 
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                                                  &buffer[count].entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _sb2_freeze_abort;
                                }
                                buffer[count].mem = mem;
                                buffer[count].index = idx;
                                count++;
                            }

                            if (lvl == _SOC_KT2_COSQ_NODE_LVL_L2){
                                /* Set the  MIN/MAX thresholds for L2 node
                                 * to 2 Kbps
                                 */
                                if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
                                    rv = soc_kt2_cosq_get_sched_mode(unit, port,
                                               lvl, index, &sched_mode, &wt);
                                } else {
                                    rv = soc_kt2_cosq_get_sched_mode(unit, port,
                                               lvl - 2, index, &sched_mode, &wt);
                                }
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _sb2_freeze_abort;
                                }
                                if ( sched_mode == BCM_COSQ_STRICT )
                                {
                                     rv = soc_sb2_cosq_min_bucket_get(unit, port,
                                                                  index, lvl,
                                                                  &min, &mant,
                                                                  &exp, &cycle);
                                     if (SOC_FAILURE(rv)) {
                                        sal_free(buffer);
                                        goto _sb2_freeze_abort;
                                     }
                                     if (min) {
                                          if (exp != 0) {
                                              if (exp < 10) {
                                                  burst_exp = 5;
                                                  burst_mant = 0;
                                              } else {
                                                  burst_exp = exp - 4;
                                                  burst_mant = (mant >> 3);
                                              }
                                           }
                                           else { 
                                           burst_mant = 0;
                                           burst_exp = 0;
                                           } 
                                     }
                                }
                                if (!min)
                                {
                                  /* Set the  MIN/MAX thresholds for L2 node
                                   * to 2 Kbps
                                   */
                                  mant = 1;
                                  cycle = 4;
                                  exp = 0;
                                }
                                f_idx = (j * 4) + index % 4;
                                sal_memset(entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
                                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                                      idx, &entry);
                                if (SOC_FAILURE(rv)) {
                                    sal_free(buffer);
                                    goto _sb2_freeze_abort;
                                }
                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_mant_fields[f_idx], mant);
                                soc_mem_field32_set(unit, mem, &entry,
                                     rate_exp_fields[f_idx], exp);
                                soc_mem_field32_set(unit, mem, &entry,
                                     burst_exp_fields[f_idx], burst_exp);
                                soc_mem_field32_set(unit, mem, &entry,
                                     burst_mant_fields[f_idx], burst_mant);
                                soc_mem_field32_set(unit, mem, &entry,
                                     cycle_sel_fields[f_idx], cycle);
                                rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                                   idx, &entry);
                                 min = 0;
                                 sched_mode = 0;
                                 cycle = 0;
                                 exp = 0;
                                 mant = 0;
                                 burst_exp = 0;
                                 burst_mant = 0;
                            } else if (((lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) && (j == 1)) ||
                                       (lvl < _SOC_KT2_COSQ_NODE_LVL_L0)) {
                                /* Set the MIN/MAX thresholds for L0/L1 node
                                 * to max shaper rate
                                 */
                                rv = soc_sb2_cosq_max_bucket_set(unit, port,
                                                                index, lvl);
                            }
                            if (SOC_FAILURE(rv)) {
                                sal_free(buffer);
                                goto _sb2_freeze_abort;
                            }
                        }/*END of for j */
                        prev_idx = idx;
                    } /* END of the valid bit */
                } /* END of the word */
            } /* END of lvl */

            buffer[count].mem = INVALIDm;
            *setting = buffer;
_sb2_freeze_abort:
            SOC_LLS_SCHEDULER_UNLOCK(unit);
            break;
        }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56370:
    case SOC_CHIP_BCM56470:
    case SOC_CHIP_BCM56275:
    case SOC_CHIP_BCM56770:
        {
            uint64 rval64;
            uint64 *buffer;
            uint64 pbits64;
            int pipe;
            int split, pos;
            soc_reg_t mtro_reg;
            soc_reg_t reg_mtro[2] = {MTRO_PORT_ENTITY_DISABLE_SPLIT0r,
                                     MTRO_PORT_ENTITY_DISABLE_SPLIT1r};

            COMPILER_64_ZERO(rval64);
            COMPILER_64_ZERO(pbits64);

            SOC_IF_ERROR_RETURN(
                soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));
            mtro_reg = reg_mtro[split];

            if (pos < 32) {
                COMPILER_64_SET(pbits64, 0, (1 << pos));
            } else {
                COMPILER_64_SET(pbits64, (1 << (pos - 32)), 0);
            }

            buffer = sal_alloc(sizeof(uint64), "MTRO status buffer");
            if (buffer == NULL) {
                rv = SOC_E_MEMORY;
                break;
            }
            /* Disable Shaper Updates */
            rv = soc_reg_get(unit, mtro_reg, pipe, 0, &rval64);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                break;
            }
            COMPILER_64_COPY(*buffer, rval64);
            COMPILER_64_AND(*buffer, pbits64);

            COMPILER_64_OR(rval64, pbits64);

            /* METERING_DISABLEf is the only field in reg. */
            rv = soc_reg_set(unit, mtro_reg, pipe, 0, rval64);
            if (SOC_FAILURE(rv)) {
                sal_free(buffer);
                break;
            }

            *setting = buffer;
            break;
        }
#endif /* BCM_TRIDENT3_SUPPORT */
    case SOC_CHIP_BCM56960: /* Use hardware flush */
    case SOC_CHIP_BCM56965:
    default:
        break;
    }

    if (SOC_FAILURE(rv)) {
        /* UNLOCK if fail */
        SOC_EGRESS_METERING_UNLOCK(unit);
    }

    return rv;
}

int
_soc_egress_metering_thaw(int unit, soc_port_t port, void *setting)
{
    int       rv;

    if (setting == NULL) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return SOC_E_NONE;
    }

    rv = SOC_E_NONE;

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
    case SOC_CHIP_BCM53314:
    case SOC_CHIP_BCM56142:
        {
            uint32 *rval32;

            rval32 = setting;
            /* Restore egress metering configuration. */
            rv = WRITE_EGRMETERINGCONFIGr(unit, port, *rval32);
            sal_free(setting);
            break;
        }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT || BCM_HURRICANE_SUPPORT */
#if defined (BCM_HURRICANE2_SUPPORT)
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53540:
        {
            int i;
            uint32 *buffer = setting;
            int count;
            int err_flag = 0;
 
            /* restore the original configuration : MAXBUCKETCONFIGr */
            count = 0;
            for (i = 0; i < SOC_REG_NUMELS(unit, MAXBUCKETCONFIGr); i++) {
                rv = WRITE_MAXBUCKETCONFIGr(unit, port, i, buffer[count]);
                if (SOC_FAILURE(rv)) {
                    err_flag = 1;
                    break;
                }
                count ++;
            }
            if (!err_flag) {
                /* restore the original configuration : EGRMETERINGCONFIGr */
                rv = WRITE_EGRMETERINGCONFIGr(unit, port, buffer[count]);
            }

            sal_free(setting);
            break;
        }
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    case SOC_CHIP_BCM53570:
#if defined (BCM_FIRELIGHT_SUPPORT)
    case SOC_CHIP_BCM56070:
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            int i;
            uint32 *buffer = setting;
            int err_flag = 0;
            int count;
            int qlayer_entry_count = 0;
            int qgroup_entry_count = 0;
            soc_mem_t config_mem = INVALIDm;
            uint32 entry[SOC_MAX_MEM_WORDS];
            int memidx;
            int is_64q_port;

#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_64q_port_check(unit, port, &is_64q_port);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_64q_port_check(unit, port, &is_64q_port);
            }
            if (SOC_FAILURE(rv)) {
                sal_free(setting);
                break;
            }

            if (is_64q_port) {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    qlayer_entry_count = SOC_FL_QLAYER_COSQ_PER_PORT_NUM;
                    qgroup_entry_count = SOC_FL_QGROUP_PER_PORT_NUM;
                }else
#endif
                {
                    qlayer_entry_count = SOC_GH2_QLAYER_COSQ_PER_PORT_NUM;
                    qgroup_entry_count = SOC_GH2_QGROUP_PER_PORT_NUM;
                }
            } else {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    qlayer_entry_count = SOC_FL_LEGACY_QUEUE_NUM;
                    qgroup_entry_count = 0;
                }else
#endif
                {
                    qlayer_entry_count = SOC_GH2_LEGACY_QUEUE_NUM;
                    qgroup_entry_count = 0;
                }
            }
            
            /* buffer required (X*2+Y*2+1) for store original setting.
             *   MMU port 0~57: X=8, Y=0
             *   MMU port 58-65 : X=64, Y=8
             * MMU_MAX_BUCKET_QLAYERm : 
             *   X entries for a port, two fields (REFRESHf, THD_SELf)stored
             * MMU_MAX_BUCKET_QGROUPm : Y entries for a port
             *   Y entries for a port, two fields (REFRESHf, THD_SELf)stored
             * EGRMETERINGCONFIG : 1 register value for a port
             */

            /* restore the original configuration : MMU_MAX_BUCKET_QLAYERm */
            count = 0;
            config_mem = MMU_MAX_BUCKET_QLAYERm;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_mmu_bucket_qlayer_index(unit, port, 0, &memidx);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_mmu_bucket_qlayer_index(unit, port, 0, &memidx);
            }
            if (SOC_FAILURE(rv)) {
                err_flag = 1;
            } else {
                for (i = memidx; i < qlayer_entry_count; i++) {
                    rv = soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, 
                                      i, &entry);
                    if (SOC_FAILURE(rv)) {
                        err_flag = 1;
                        break;
                    }
                    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, 
                                        buffer[count]);
                    count ++;
                    soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, 
                                        buffer[count]);
                    count ++;
                    rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, 
                                       i, &entry);
                    if (SOC_FAILURE(rv)) {
                        err_flag = 1;
                        break;
                    }
                }
            }

            /* restore the original configuration : MMU_MAX_BUCKET_QGROUPm */
            if (!err_flag) {
                if (qgroup_entry_count) {
                    config_mem = MMU_MAX_BUCKET_QGROUPm;
#ifdef BCM_FIRELIGHT_SUPPORT
                    if (soc_feature(unit, soc_feature_fl)) {
                        rv = soc_fl_mmu_bucket_qgroup_index(unit, port,
                                                            0, &memidx);
                    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                    {
                        rv = soc_gh2_mmu_bucket_qgroup_index(unit, port,
                                                             0, &memidx);
                    }
                    if (SOC_FAILURE(rv)) {
                        err_flag = 1;
                    } else {
                        for (i = memidx; i < qgroup_entry_count; i++) {
                            rv = soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                          i, &entry);
                            if (SOC_FAILURE(rv)) {
                                err_flag = 1;
                                break;
                            }

                            soc_mem_field32_set(unit, config_mem, &entry, 
                                                REFRESHf, buffer[count]);
                            count ++;
                            soc_mem_field32_set(unit, config_mem, &entry, 
                                                THD_SELf, buffer[count]);
                            count ++;
                            rv = soc_mem_write(unit, config_mem, 
                                               MEM_BLOCK_ALL, i, &entry);
                            if (SOC_FAILURE(rv)) {
                                err_flag = 1;
                                break;
                            }
                        }
                    }
                }
            }

            if (!err_flag) {
                /* restore the original configuration : EGRMETERINGCONFIGr */
                rv = WRITE_EGRMETERINGCONFIGr(unit, port, buffer[count]);
            }
            sal_free(setting);
            break;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56334:
    case SOC_CHIP_BCM56685:
        {
            uint64 *rval64;

            rval64 = setting;
            /* Restore egress metering configuration. */
            rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, *rval64);
            sal_free(setting);
            break;
        }
    case SOC_CHIP_BCM56840:
        {
            int count, index, queue_cnt, err_flag = 0;
            soc_info_t *si;
            uint64 *buffer = setting;

            si = &SOC_INFO(unit);
            /* Port level + S2 + S3 + UCQ + MCQ + EXT_UCQ. */
            queue_cnt = si->port_num_uc_cosq[port] + si->port_num_cosq[port] +
                        si->port_num_ext_cosq[port];

            count = 0;
            /* Port level meter. */
            rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, buffer[count]);
            if (SOC_FAILURE(rv)) {
                err_flag = 1;
            }
            count++;

            if (!IS_CPU_PORT(unit, port)) {
                /* S2 meter (including MC group). */
                for (index = 0; (index < 4) && (err_flag == 0); index++) {
                    rv = WRITE_S2_MAXBUCKETCONFIG_64r(unit, port, index,
                                                      buffer[count]);
                    if (SOC_FAILURE(rv)) {
                        err_flag = 1;
                        break;
                    }
                    count++;
                }
                /* S3 meter. */
                for (index = 0; (index < 4) && (err_flag == 0); index++) {
                    rv = WRITE_S3_MAXBUCKETCONFIG_64r(unit, port, index,
                                                      buffer[count]);
                    if (SOC_FAILURE(rv)) {
                        err_flag = 1;
                        break;
                    }
                    count++;
                }
            }

            /* Queue meter. */
            for (index = 0; (index < queue_cnt) && (err_flag == 0); index++) {
                rv = WRITE_MAXBUCKETCONFIG_64r(unit, port, index,
                                               buffer[count]);
                if (SOC_FAILURE(rv)) {
                    break;
                }
                count++;
            }
            sal_free(setting);
            break;
        }
    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56340:
        {
            uint64 rval64;
            int index;
            mem_entry_t *buffer = setting;

            /* Restore port level metering configuration. */
            sal_memcpy(&rval64, &buffer[0].entry[0], sizeof(uint64));
            rv = WRITE_EGRMETERINGCONFIG_64r(unit, port, rval64);

            /* Restore L0,L1,L2 metering configuration. */
            for (index = 1; buffer[index].mem != INVALIDm ;index++) {
                rv = soc_mem_write(unit, buffer[index].mem, MEM_BLOCK_ANY,
                                   buffer[index].index, &buffer[index].entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }

            sal_free(setting);
            break;
        }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    case SOC_CHIP_BCM56440:
    case SOC_CHIP_BCM56450:
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56270:
        {
            int index;
            mem_entry_t *buffer;

            buffer = setting;
            for (index = 0; buffer[index].mem != INVALIDm ;index++) {
                rv = soc_mem_write(unit, buffer[index].mem, MEM_BLOCK_ANY,
                                   buffer[index].index, &buffer[index].entry);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
            sal_free(setting);
            break;
        }
#endif /* BCM_TRIDENT2_SUPPORT  || BCM_KATANA_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56370:
    case SOC_CHIP_BCM56470:
    case SOC_CHIP_BCM56275:
    case SOC_CHIP_BCM56770:
        {
            uint64 rval64;
            uint64 *buffer;
            uint64 pbits64;
            int pipe;
            int split, pos;
            soc_reg_t mtro_reg;
            soc_reg_t reg_mtro[2] = {MTRO_PORT_ENTITY_DISABLE_SPLIT0r,
                                     MTRO_PORT_ENTITY_DISABLE_SPLIT1r};

            COMPILER_64_ZERO(rval64);
            COMPILER_64_ZERO(pbits64);

            SOC_IF_ERROR_RETURN(
                soc_td3_mmu_bmp_reg_pos_get(unit, port, &pipe, &split, &pos));
            mtro_reg = reg_mtro[split];

            if (pos < 32) {
                COMPILER_64_SET(pbits64, 0, (1 << pos));
            } else {
                COMPILER_64_SET(pbits64, (1 << (pos - 32)), 0);
            }

            /* Restore Shaper Updates */
            rv = soc_reg_get(unit, mtro_reg, pipe, 0, &rval64);
            if (SOC_FAILURE(rv)) {
                sal_free(setting);
                break;
            }
            buffer = setting;
            COMPILER_64_NOT(pbits64);
            COMPILER_64_AND(rval64, pbits64);
            COMPILER_64_OR(rval64, *buffer);

            /* METERING_DISABLEf is the only field in reg. */
            rv = soc_reg_set(unit, mtro_reg, pipe, 0, rval64);
            if (SOC_FAILURE(rv)) {
                sal_free(setting);
                break;
            }

            sal_free(setting);
            break;
        }
#endif /* BCM_TRIDENT3_SUPPORT */
    case SOC_CHIP_BCM56960: /* Use hardware flush */
    case SOC_CHIP_BCM56965:
    default:
        break;
    }

    SOC_EGRESS_METERING_UNLOCK(unit);

    return rv;
}

int
soc_mmu_flush_enable(int unit, soc_port_t port, int enable)
{
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)  
    uint32 flush_ctrl;
#endif

#if defined(BCM_KATANA_SUPPORT)
    void *setting = NULL;
    int rv = SOC_E_NONE;
    int rv1 = SOC_E_NONE;
#endif

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
        SOC_IF_ERROR_RETURN
            (READ_MMUFLUSHCONTROLr(unit, &flush_ctrl));
        flush_ctrl &= ~(0x1 << port);
        flush_ctrl |= enable ? (0x1 << port) : 0;
        SOC_IF_ERROR_RETURN
            (WRITE_MMUFLUSHCONTROLr(unit, flush_ctrl));
        break;
#endif /* BCM_RAVEN_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53540:
        SOC_IF_ERROR_RETURN
            (READ_MMUFLUSHCONTROLr(unit, &flush_ctrl));
        flush_ctrl &= ~(0x1 << port);
        flush_ctrl |= enable ? (0x1 << port) : 0;
        SOC_IF_ERROR_RETURN
            (WRITE_MMUFLUSHCONTROLr(unit, flush_ctrl));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d MMUFLUSHCONTROl.FLUSH=0x%x\n"), port, flush_ctrl));
        break;
    case SOC_CHIP_BCM53570:
    case SOC_CHIP_BCM56070:
        {
            int port_off;
            if (port < 32) {
                port_off = port;
                SOC_IF_ERROR_RETURN
                    (READ_MMUFLUSHCONTROL_0r(unit, &flush_ctrl));
                flush_ctrl &= ~(0x1 << port_off);
                flush_ctrl |= enable ? (0x1 << port_off) : 0;
                SOC_IF_ERROR_RETURN
                    (WRITE_MMUFLUSHCONTROL_0r(unit, flush_ctrl));
            } else if ((port >= 32) && (port < 64)) {
                port_off = port - 32;
                SOC_IF_ERROR_RETURN
                    (READ_MMUFLUSHCONTROL_1r(unit, &flush_ctrl));
                flush_ctrl &= ~(0x1 << port_off);
                flush_ctrl |= enable ? (0x1 << port_off) : 0;
                SOC_IF_ERROR_RETURN
                    (WRITE_MMUFLUSHCONTROL_1r(unit, flush_ctrl));
            } else {
                port_off = port - 64;
                SOC_IF_ERROR_RETURN
                    (READ_MMUFLUSHCONTROL_1r(unit, &flush_ctrl));
                flush_ctrl &= ~(0x1 << port_off);
                flush_ctrl |= enable ? (0x1 << port_off) : 0;
                SOC_IF_ERROR_RETURN
                    (WRITE_MMUFLUSHCONTROL_1r(unit, flush_ctrl));
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                        "port %d MMUFLUSHCONTROl.FLUSH=0x%x\n"), port, flush_ctrl));
        }
        break;
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_HURRICANE_SUPPORT)
    case SOC_CHIP_BCM56142:
        SOC_IF_ERROR_RETURN
            (READ_MMUFLUSHCONTROLr(unit, &flush_ctrl));
        flush_ctrl &= ~(0x1 << port);
        flush_ctrl |= enable ? (0x1 << port) : 0;
        SOC_IF_ERROR_RETURN
            (WRITE_MMUFLUSHCONTROLr(unit, flush_ctrl));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d MMUFLUSHCONTROl.FLUSH=0x%x\n"), port, flush_ctrl));
    /* Fall through */
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
        SOC_IF_ERROR_RETURN
            (READ_FAST_TX_FLUSHr(unit, port, &flush_ctrl));
        soc_reg_field_set(unit, FAST_TX_FLUSHr, &flush_ctrl, IDf,
                          (enable) ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (WRITE_FAST_TX_FLUSHr(unit, port, flush_ctrl));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d FAST_TX_FLUSH.ID=0x%x\n"), port, flush_ctrl));
    /* Fall through */
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56334:
    /*case SOC_CHIP_BCM88732:*/
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_XP_EGR_PKT_DROP_CTLr(unit, port, &flush_ctrl));
            soc_reg_field_set(unit, XP_EGR_PKT_DROP_CTLr, &flush_ctrl, FLUSHf,
                              (enable) ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, flush_ctrl));
           LOG_VERBOSE(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                                   "port %d XP_EGR_PKT_DROP_CTL.FLUSH=0x%x\n"), port, flush_ctrl));
        }
        break;
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
        if (IS_GX_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_XP_EGR_PKT_DROP_CTLr(unit, port, &flush_ctrl));
            soc_reg_field_set(unit, XP_EGR_PKT_DROP_CTLr, &flush_ctrl, FLUSHf,
                              (enable) ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (WRITE_XP_EGR_PKT_DROP_CTLr(unit, port, flush_ctrl));
        }
        break;
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
    case SOC_CHIP_BCM56840:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLP_TXFIFO_PKT_DROP_CTLr, port,
                                    DROP_ENf, enable ? 1 : 0));
        break;
#endif /* BCM_TRIDENT_SUPPORT */

    case SOC_CHIP_BCM56640:
    case SOC_CHIP_BCM56340:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, PORT_TXFIFO_PKT_DROP_CTLr, port,
                                    DROP_ENf, enable ? 1 : 0));
        break;

#ifdef BCM_KATANA_SUPPORT
    case SOC_CHIP_BCM56440:
    case SOC_CHIP_BCM56450:
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56270:
        if (!(SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM
                   || SAL_BOOT_XGSSIM)) {
            soc_kt_port_flush_state_set(unit, port, TRUE); 
            if (enable == 0) {
                rv = soc_kt_port_flush(unit, port, enable);
            } else {
                rv = soc_mmu_backpressure_clear(unit, port);
                if (SOC_FAILURE(rv)) {
                    soc_kt_port_flush_state_set(unit, port, FALSE);
                    return rv;
                } 
                /*
                 *****************************************************
                 * NOTE: Must not exit soc_kt_port_flush
                 *       without calling soc_egress_metering_thaw,
                 *       soc_egress_metering_freeze holds the lock.
                 *       soc_egress_metering_freeze releases the lock
                 *       on failure.
 */
                rv = _soc_egress_metering_freeze(unit,
                                        port, &setting);
                if (SOC_FAILURE(rv)) {
                    soc_kt_port_flush_state_set(unit, port, FALSE);
                    return rv;
                }
                rv = soc_kt_port_flush(unit, port, enable);
                /* Restore egress metering configuration. */
                rv1 = _soc_egress_metering_thaw(unit, port, setting);
                if (SOC_SUCCESS(rv)) {
                    rv = rv1;
                }
            }
            soc_kt_port_flush_state_set(unit, port, FALSE);
            return rv;
        }
        break;
#endif /* BCM_KATANA_SUPPORT */

    default:
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

int
soc_mmu_port_flush_hw(int unit, soc_port_t port, uint32 drain_timeout)
{
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        return SOC_E_NONE;
    }
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWK(unit)) {
        return soc_tomahawk_mmu_port_flush_hw(unit, port, drain_timeout);
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        return SOC_E_NONE;
    }
#endif
    return SOC_E_UNAVAIL;
}

#if defined(BCM_TRIUMPH3_SUPPORT)
/*#define IBOD_DEBUG*/
void soc_ibod_recovery_in_progress_set(int unit, int enable)
{
    ibod_recovery_in_progress[unit] = enable;
}
int soc_ibod_recovery_in_progress_get(int unit)
{
    return ibod_recovery_in_progress[unit];
}
void _soc_egress_drain_timeout_flag_set(int unit, int flag)
{
    egress_drain_timeout_flag[unit] = flag;
}
void _soc_egress_drain_timeout_flag_get(int unit, int *flag)
{
    *flag = egress_drain_timeout_flag[unit];
}

#if defined(IBOD_DEBUG) || defined(TXERR_DEBUG)
int  _soc_egress_drain_debug(int unit, soc_port_t port, soc_reg_t reg)
{
    uint32 credit, mmu_req, ep_level, total_count;
    egr_mmu_requests_entry_t mmu_entry;
    uint32 max_used_entry;
    int rv = SOC_E_NONE;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &credit));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_MMU_REQUESTSm, MEM_BLOCK_ANY,
                            si->port_l2p_mapping[port], &mmu_entry));
    mmu_req = soc_mem_field32_get(unit, EGR_MMU_REQUESTSm, &mmu_entry, OUTSTANDING_MMU_REQUESTSf);

    SOC_IF_ERROR_RETURN(READ_EGR_EDB_MISC_CTRLr(unit, &rval));
    soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_EDB_MISC_CTRLr(unit, rval));
    SOC_IF_ERROR_RETURN(READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                         si->port_l2p_mapping[port], &max_used_entry));
    ep_level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm, &max_used_entry, LEVELf);

    SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &rval));
    total_count = soc_reg_field_get(unit, OP_PORT_TOTAL_COUNT_CELLr,rval, TOTAL_COUNTf);

    cli_out("Recovery: port %d mmu still has credit[total_count %d, level %d, credit %d mmu_req %d] \r\n",
       port, total_count, ep_level, credit, mmu_req);

    return rv;
}
#endif /* IBOD_DEBUG || TXERR_DEBUG */

int _soc_egress_drain_ep_max_level_get(int unit, soc_port_t port, uint32 *ep_max_level)
{
    int rv = SOC_E_NONE;
    uint32 rval;
    uint32 max_used_entry;
    soc_info_t *si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(READ_EGR_EDB_MISC_CTRLr(unit, &rval));
    soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval, SELECT_CURRENT_USED_ENTRIESf, 0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_EDB_MISC_CTRLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL, si->port_l2p_mapping[port], &max_used_entry));
    *ep_max_level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm, &max_used_entry, LEVELf);

    return rv;
}

int _soc_egress_drain_credit_calc(uint32 credit_current, uint32 credit_default,
                            uint32 mmu_req, uint32 ep_level, uint32 *credit_set)
{
    if (mmu_req > credit_default ) {
        if(ep_level < credit_default) {
            *credit_set = credit_default - ep_level;
        }
        else {
            *credit_set = credit_default;
        }
    }
    else if (mmu_req == 0) {
        if(ep_level < credit_default) {
            *credit_set = credit_default - ep_level;
        }
        else {
            *credit_set = credit_default;
        }
    }
    else if (mmu_req != credit_current) {
        *credit_set = mmu_req;
    }
    else {
        return 0;
    }

    return 1;
}
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
int
_soc_egress_port_request_check(int unit, soc_port_t port, uint32 drain_timeout)
{
    soc_timeout_t to;
    uint32 ep_credit;
    int    phy_port;
    uint32 read_entry[SOC_MAX_MEM_WORDS] = {0};
    int    rv = SOC_E_NONE;
    int empty = FALSE;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56670:
    case SOC_CHIP_BCM56860:
    {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_PORT_REQUESTSm,
                                         MEM_BLOCK_ANY, phy_port, read_entry));
        ep_credit = soc_mem_field32_get(unit, EGR_PORT_REQUESTSm, read_entry,
                                        OUTSTANDING_PORT_REQUESTSf);

        if (ep_credit == 0) {
            /* Assert SOFT_RESETf. */
            SOC_IF_ERROR_RETURN(soc_esw_portctrl_mac_reset_set(unit, port, 1));

            /* De-assert SOFT_RESETf. */
            SOC_IF_ERROR_RETURN(soc_esw_portctrl_mac_reset_set(unit, port, 0));

            soc_timeout_init(&to, drain_timeout, 0);
            for (;;) {
                rv = _soc_egress_cell_check(unit, port, &empty);
                if (empty == TRUE) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    rv = _soc_egress_cell_check(unit, port, &empty);
                    if (empty == TRUE) {
                        break;
                    }
                    /*
                     * Don't return error so that remaining port sequence is
                     * executed and port is recovered successfully.
                     */
                    LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "MacDrainTimeOut:port %d,%s, "
                                        "timeout draining packets \n"),
                             unit, SOC_PORT_NAME(unit, port)));
                    break;
                }
            }
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d,%s, "
                                "Number of cell requests is not zero.\n"),
                     unit, SOC_PORT_NAME(unit, port)));
        }

        break;
    }

    default:
        return SOC_E_UNAVAIL;
    }

    return rv;
}
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

int
soc_egress_drain_cells(int unit, soc_port_t port, uint32 drain_timeout)
{
    soc_timeout_t to;
    uint32 cur_cells, new_cells;
    int rv, rv1;
    void *setting = NULL;
#if defined(BCM_TRIUMPH3_SUPPORT)
    uint32 max_used_entry, ep_level,rval, overflow0, overflow1, total_cell;
    uint32 credit, mmu_req, credit_set, mmu_cells;
    soc_info_t *si;
    egr_mmu_requests_entry_t mmu_entry;
    static const soc_reg_t lls_port_credit_reg[] = {
        LLS_PORT_0_CREDITr, LLS_PORT_1_CREDITr, LLS_PORT_2_CREDITr,
        LLS_PORT_3_CREDITr, LLS_PORT_4_CREDITr, LLS_PORT_5_CREDITr,
        LLS_PORT_6_CREDITr, LLS_PORT_7_CREDITr, LLS_PORT_8_CREDITr,
        LLS_PORT_9_CREDITr, LLS_PORT_10_CREDITr, LLS_PORT_11_CREDITr,
        LLS_PORT_12_CREDITr, LLS_PORT_13_CREDITr, LLS_PORT_14_CREDITr,
        LLS_PORT_15_CREDITr, LLS_PORT_16_CREDITr, LLS_PORT_17_CREDITr,
        LLS_PORT_18_CREDITr, LLS_PORT_19_CREDITr, LLS_PORT_20_CREDITr,
        LLS_PORT_21_CREDITr, LLS_PORT_22_CREDITr, LLS_PORT_23_CREDITr,
        LLS_PORT_24_CREDITr, LLS_PORT_25_CREDITr, LLS_PORT_26_CREDITr,
        LLS_PORT_27_CREDITr, LLS_PORT_28_CREDITr, LLS_PORT_29_CREDITr,
        LLS_PORT_30_CREDITr, LLS_PORT_31_CREDITr, LLS_PORT_32_CREDITr,
        LLS_PORT_33_CREDITr, LLS_PORT_34_CREDITr, LLS_PORT_35_CREDITr,
        LLS_PORT_36_CREDITr, LLS_PORT_37_CREDITr, LLS_PORT_38_CREDITr,
        LLS_PORT_39_CREDITr, LLS_PORT_40_CREDITr, LLS_PORT_41_CREDITr,
        LLS_PORT_42_CREDITr, LLS_PORT_43_CREDITr, LLS_PORT_44_CREDITr,
        LLS_PORT_45_CREDITr, LLS_PORT_46_CREDITr, LLS_PORT_47_CREDITr,
        LLS_PORT_48_CREDITr, LLS_PORT_49_CREDITr, LLS_PORT_50_CREDITr,
        LLS_PORT_51_CREDITr, LLS_PORT_52_CREDITr, LLS_PORT_53_CREDITr,
        LLS_PORT_54_CREDITr, LLS_PORT_55_CREDITr, LLS_PORT_56_CREDITr,
        LLS_PORT_57_CREDITr, LLS_PORT_58_CREDITr, LLS_PORT_59_CREDITr,
        LLS_PORT_60_CREDITr, LLS_PORT_61_CREDITr, LLS_PORT_62_CREDITr
    }; 
    si = &SOC_INFO(unit);
#endif
    if (soc_feature(unit, soc_feature_mmu_hw_flush)) {
        SOC_IF_ERROR_RETURN(soc_mmu_port_flush_hw(unit, port, drain_timeout));
        rv = SOC_E_NONE;
    } else {
        if(!SOC_IS_KATANAX(unit)) {
        SOC_IF_ERROR_RETURN(soc_mmu_backpressure_clear(unit, port));

        /*
         **************************************************************
         * NOTE: Must not exit this function without calling
         *       soc_egress_metering_thaw(). soc_egress_metering_freeze
         *       holds a lock.
 */
        SOC_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit, port, &setting));
        }

#if defined (BCM_APACHE_SUPPORT) || defined (BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_APACHE(unit) ||
            SOC_IS_TRIDENT2PLUS(unit) ||
            SOC_IS_TRIDENT2(unit)) {
            int empty = 0;
            if (SOC_IS_TRIDENT2PLUS(unit)) {
                /* Wait long enough to ensure CT FIFO of flushed ports are empty */
                sal_usleep(4);
            }
            if (SOC_IS_MONTEREY(unit)) { 
            /*  extra delay added in monterey to drain the transit macsec packets for the port*/  
                sal_usleep(1000);
            }
 
            soc_timeout_init(&to, drain_timeout, 0);
            for (;;) {
                rv = _soc_egress_cell_check(unit, port, &empty);
                if (empty == TRUE) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    rv = _soc_egress_cell_check(unit, port, &empty);
                    if (empty == TRUE) {
                        break;
                    }

                    break;
                }
            }
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
            if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
                if (empty != TRUE) {
                   /*
                    * Check for credit wrap scenario in response to initial credit
                    * reset and if detected, re-init the credit.
                    */
                   rv = _soc_egress_port_request_check(unit, port, drain_timeout);
                }
            } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
            if (empty != TRUE) {
                /* Don't return error so that remaining port sequence is executed
                 * and port is recovered successfully
                 */
                LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MacDrainTimeOut:port %d,%s, "
                                    "timeout draining packets \n"),
                         unit, SOC_PORT_NAME(unit, port)));
            }
        } else
#endif /* BCM_APACHE_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT || BCM_TRIDENT2_SUPPORT */
        {

#if defined(BCM_TRIDENT2_SUPPORT)
        
#endif /* BCM_TRIDENT2_SUPPORT */

        cur_cells = 0xffffffff;

        /* Probably not required to continuously check COSLCCOUNT if the fast
         * MMU flush feature is available - done just as an insurance */
        rv = SOC_E_NONE;
        for (;;) {
#if defined(BCM_TRIUMPH3_SUPPORT)
            /* only for TR3 MMU drain cells */
            if(SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit) &&
               IS_CL_PORT(unit, port) && ibod_recovery_in_progress[unit]) {
#ifdef IBOD_DEBUG
                uint32 ep_max_level = 0;
                uint32 mmu_req_1 = 0;
                /* check epc_link_bmap for debug */
                pbmp_t  pbm;
                epc_link_bmap_entry_t entry;
                READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0,&entry);
                soc_mem_pbmp_field_get(unit, EPC_LINK_BMAPm, &entry, PORT_BITMAPf, &pbm);
                if (SOC_PBMP_MEMBER(pbm, port)) {
                    LOG_WARN(BSL_LS_BCM_PORT,
                                 (BSL_META_U(unit,
                                 "soc_egress_drain_cells:unit %d port %d still in epc link bitmap 0x%x\r\n"),
                                 unit, port, pbm.pbits[0]));
                }
#endif
                new_cells=0;

                /* check MMU cell */
                if ((rv = soc_egress_cell_count(unit, port, &mmu_cells)) < 0) {
                    break;
                }
                new_cells += mmu_cells;

                /* read MMU credit */
                rv = soc_reg32_get(unit,lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]], REG_PORT_ANY, 0, &credit);
                SOC_IF_ERROR_RETURN(rv);

                rv = soc_mem_read(unit, EGR_MMU_REQUESTSm, MEM_BLOCK_ANY, si->port_l2p_mapping[port], &mmu_entry);
                SOC_IF_ERROR_RETURN(rv);
                mmu_req = soc_mem_field32_get(unit, EGR_MMU_REQUESTSm, &mmu_entry, OUTSTANDING_MMU_REQUESTSf);

                /* read EP level */
                rv = READ_EGR_EDB_MISC_CTRLr(unit, &rval);
                SOC_IF_ERROR_RETURN(rv);
                soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
                rv = WRITE_EGR_EDB_MISC_CTRLr(unit, rval);
                SOC_IF_ERROR_RETURN(rv);
                rv = READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL, si->port_l2p_mapping[port], &max_used_entry);
                SOC_IF_ERROR_RETURN(rv);
                ep_level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm, &max_used_entry, LEVELf);

                /* Update cur_cells and give more time to drain packets.  */
                if (new_cells < cur_cells) {
                    soc_timeout_init(&to, drain_timeout, 0);
                    cur_cells = new_cells;
                }
                else {
                    rv = soc_reg32_set(unit,lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]], REG_PORT_ANY, 0, si->port_credit[port]);
                    SOC_IF_ERROR_RETURN(rv);
#ifdef IBOD_DEBUG
                    cli_out("soc_egress_drain_cells: port %d mmu cells not draining: %d cells remain,cur_cells %d, mmu_cells %d, level %d, credit %d mmu_req %d \r\n",
                          port, new_cells, cur_cells, mmu_cells, ep_level, credit, mmu_req);
#endif
                }
                if (soc_timeout_check(&to)) {
                    if (ep_level == 0){
                        _soc_egress_drain_timeout_flag_set(unit, 1);
#ifdef IBOD_DEBUG
                        cli_out("soc_egress_drain_cells: Timeout Drain packets and ep_level is 0 (port %d, %d cells remain,cur_cells %d, mmu_cells %d, level %d, credit %d mmu_req %d) \r\n",
                            port, new_cells, cur_cells, mmu_cells, ep_level, credit, mmu_req);
#endif
                        rv = SOC_E_NONE;
                        break;
                    }
                    else {
                        SOC_IF_ERROR_RETURN(READ_PQEFIFOOVERFLOW0r(unit, &overflow0));
                        SOC_IF_ERROR_RETURN(READ_PQEFIFOOVERFLOW1r(unit, &overflow1));
                        SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &total_cell));
                        LOG_ERROR(BSL_LS_BCM_PORT,
                                 (BSL_META_U(unit,
                                   "\n soc_egress_drain_cells: Timeout Drain packets unit %d PORT: %s"
                                   "(%d cells remain,cur_cells %d, mmu_cells %d, level %d, credit %d "
                                   "mmu_req %d overflow0 %d overflow1 %d total_cell %d) \r\n"),
                                  unit, SOC_PORT_NAME(unit,port),
                                  new_cells,cur_cells, mmu_cells, ep_level, credit,
                                  mmu_req, overflow0, overflow1, total_cell));
                        rv = SOC_E_INTERNAL;
                        break;
                    }
                }

#ifdef IBOD_DEBUG
                /* for debug */
                rv = soc_mem_read(unit, EGR_MMU_REQUESTSm, MEM_BLOCK_ANY, si->port_l2p_mapping[port], &mmu_entry);
                SOC_IF_ERROR_RETURN(rv);
                mmu_req_1 = soc_mem_field32_get(unit, EGR_MMU_REQUESTSm, &mmu_entry, OUTSTANDING_MMU_REQUESTSf);
                if(mmu_req_1 != mmu_req){
                    cli_out("soc_egress_drain_cells: port %d credit %d mmu_req %d is not equal mmu_req_1 %d  !!!!!\r\n",port,credit, mmu_req, mmu_req_1);
                }
#endif
                /* All packets were drained */
                if (new_cells == 0) {
                    /* default credit is not set */
                    if(si->port_credit[port] == 0) {
                        si->port_credit[port] = mmu_req;
                    }

                    if (_soc_egress_drain_credit_calc(credit, si->port_credit[port], mmu_req, ep_level, &credit_set)) {
                        rv = soc_reg32_set(unit,lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]], REG_PORT_ANY, 0, credit_set);
                        SOC_IF_ERROR_RETURN(rv);
                    }
#ifdef IBOD_DEBUG
                    rv = _soc_egress_drain_ep_max_level_get(unit, port, &ep_max_level);
                    if(rv == SOC_E_NONE)
                    {
                         cli_out("IBOD recovery: MacDrainOK max_level %d  !!!!!\r\n", ep_max_level);
                    }
#endif
                    break;
                }
                else if (mmu_cells){  /* some packets still stay in MMU */
                    if((credit == 0)) {
                        rv = soc_reg32_get(unit,lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]], REG_PORT_ANY, 0, &credit);
                        SOC_IF_ERROR_RETURN(rv);
                        if(credit == 0) {
                            rv = soc_mem_read(unit, EGR_MMU_REQUESTSm, MEM_BLOCK_ANY, si->port_l2p_mapping[port], &mmu_entry);
                            SOC_IF_ERROR_RETURN(rv);
                            mmu_req = soc_mem_field32_get(unit, EGR_MMU_REQUESTSm, &mmu_entry, OUTSTANDING_MMU_REQUESTSf);

                            /* default credit is not set */
                            if(si->port_credit[port] == 0) {
                                si->port_credit[port] = mmu_req;
                            }

                            if (_soc_egress_drain_credit_calc(credit, si->port_credit[port], mmu_req, ep_level, &credit_set)) {
                                rv = soc_reg32_set(unit,lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]], REG_PORT_ANY, 0, credit_set);
                                SOC_IF_ERROR_RETURN(rv);
                            }
                        }
                        else {
#ifdef IBOD_DEBUG
                            _soc_egress_drain_debug(unit, port, lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]]);
#endif
                        }
                    }
                    else {
#ifdef IBOD_DEBUG
                        _soc_egress_drain_debug(unit, port, lls_port_credit_reg[si->port_p2m_mapping[si->port_l2p_mapping[port]]]);
#endif
                    }
                }
            }
            else
#endif
        {
            if ((rv = soc_egress_cell_count(unit, port, &new_cells)) < 0) {
                break;
            }

            if (new_cells == 0) {
                rv = SOC_E_NONE;
                break;
            }

            if (new_cells < cur_cells) {                    /* Progress made */
                /* Restart timeout */
                soc_timeout_init(&to, drain_timeout, 0);
                cur_cells = new_cells;
            }

            if (soc_timeout_check(&to)) {
                if ((rv = soc_egress_cell_count(unit, port, &new_cells)) < 0) {
                    break;
                }
#if defined(BCM_TRIUMPH3_SUPPORT)
                /*Drain timeout is not a real issue for triumph3 and Helix4, so need not return error code*/
                if(SOC_IS_HELIX4(unit)){
                    SOC_IF_ERROR_RETURN(READ_EGR_EDB_MISC_CTRLr(unit, &rval));
                    soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
                    SOC_IF_ERROR_RETURN(WRITE_EGR_EDB_MISC_CTRLr(unit, rval));
                    SOC_IF_ERROR_RETURN(READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                        si->port_l2p_mapping[port], &max_used_entry));
                    ep_level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm, &max_used_entry, LEVELf);
                    SOC_IF_ERROR_RETURN(READ_PQEFIFOOVERFLOW0r(unit, &overflow0));
                    SOC_IF_ERROR_RETURN(READ_PQEFIFOOVERFLOW1r(unit, &overflow1));
                    SOC_IF_ERROR_RETURN(READ_OP_PORT_TOTAL_COUNT_CELLr(unit, port, &total_cell));
                    if(ep_level == 0) {
                        _soc_egress_drain_timeout_flag_set(unit, 1);
                    }
                    else {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                      "MacDrainTimeOut:port %d,%s, "
                                      "timeout draining packets (%d cells remain), ep_level:%d, overflow0:%d, overflow1:%d,total_cell:%d\n"),
                           unit, SOC_PORT_NAME(unit, port), new_cells, ep_level, overflow0, overflow1,total_cell));
                    }
                }
#else
                LOG_ERROR(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit,"MacDrainTimeOut:port %d,%s, "
                                    "timeout draining packets (%d cells remain)\n"),
                            unit, SOC_PORT_NAME(unit, port), new_cells));
                rv = SOC_E_INTERNAL;
#endif
                break;
             }
           }
        }
        }

        if (!SOC_IS_KATANAX(unit)) {
        /* Restore egress metering configuration. */
        rv1 = _soc_egress_metering_thaw(unit, port, setting);
        if (SOC_SUCCESS(rv)) {
            rv  = rv1;
        }
        }
    }

    return rv;
}

int
soc_txfifo_drain_cells(int unit, soc_port_t port, uint32 drain_timeout)
{
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2X(unit)) {
        soc_timeout_t to;
        int count = 0;

        soc_timeout_init(&to, drain_timeout, 0);
        for (;;) {
            SOC_IF_ERROR_RETURN
                (_soc_port_txfifo_cell_count(unit, port, &count));
            if (!count) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MacDrainTimeOut:port %d,%s, timeout draining TXFIFO "
                                      "(pending: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), count));
                return SOC_E_INTERNAL;
            }
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIDENT2_SUPPORT */

    return SOC_E_NONE;
}

int
soc_port_credit_reset(int unit, soc_port_t port)
{
    int phy_port;
    egr_port_credit_reset_entry_t entry;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56770:
    case SOC_CHIP_BCM56370:
    case SOC_CHIP_BCM56470:
    case SOC_CHIP_BCM56275:
    case SOC_CHIP_BCM56970:
    case SOC_CHIP_BCM56670:
        /* Should be called by CLMAC and XLMAC driver */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        sal_memset(&entry, 0, sizeof(entry));
        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm, &entry, VALUEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                         phy_port, &entry));
        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm, &entry, VALUEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                         phy_port, &entry));
        break;
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53570:
    case SOC_CHIP_BCM56070:
    case SOC_CHIP_BCM53540:
    {
        uint32 rval;
        int bindex;
        static const soc_field_t port_field[] = {
            PORT0f, PORT1f, PORT2f, PORT3f
        };
        soc_reg_t txfifo_reg;

        /* Should only be called by XLMAC/CLMAC driver */
        if (!IS_XL_PORT(unit, port) && !IS_CL_PORT(unit, port)) {
            break;
        }

        txfifo_reg = XLPORT_TXFIFO_CTRLr;
        
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        bindex = SOC_PORT_BINDEX(unit, phy_port);

#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit)){
            int blk, blktype;
            blk = SOC_PORT_BLOCK(unit, phy_port);
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if (blktype == SOC_BLK_GXPORT){
                txfifo_reg = PGW_GX_TXFIFO_CTRLr;
            } else if (blktype == SOC_BLK_XLPORT) {
                txfifo_reg = PGW_XL_TXFIFO_CTRLr;
            } else if (blktype == SOC_BLK_CLPORT) {
                txfifo_reg = PGW_CL_TXFIFO_CTRLr;
            }
        }
#endif

        if (IS_CL_PORT(unit, port)) {
            if (SOC_REG_PORT_VALID(unit, CLPORT_ENABLE_REGr, port)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, CLPORT_ENABLE_REGr, port,
                                            port_field[bindex], 0));
            }
        } else {
            if (SOC_REG_PORT_VALID(unit, XLPORT_ENABLE_REGr, port)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, XLPORT_ENABLE_REGr, port,
                                            port_field[bindex], 0));
            }
        }

        sal_memset(&entry, 0, sizeof(entry));
        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm, &entry, VALUEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL, phy_port, &entry));

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, txfifo_reg, port, 0, &rval));        
        soc_reg_field_set(unit, txfifo_reg, &rval, MAC_CLR_COUNTf,1);
        soc_reg_field_set(unit, txfifo_reg, &rval, CORE_CLR_COUNTf,1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, txfifo_reg, port, 0, rval));

        sal_usleep(1000);

        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm, &entry, VALUEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL, phy_port, &entry));

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, txfifo_reg, port, 0, &rval));        
        soc_reg_field_set(unit, txfifo_reg, &rval, MAC_CLR_COUNTf, 0);
        soc_reg_field_set(unit, txfifo_reg, &rval, CORE_CLR_COUNTf, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, txfifo_reg, port, 0, rval));

        if (IS_CL_PORT(unit, port)) {
            if (SOC_REG_PORT_VALID(unit, CLPORT_ENABLE_REGr, port)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, CLPORT_ENABLE_REGr, port,
                                            port_field[bindex], 1));
            }
        } else {
            if (SOC_REG_PORT_VALID(unit, XLPORT_ENABLE_REGr, port)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, XLPORT_ENABLE_REGr, port,
                                            port_field[bindex], 1));
            }
        }
    }
        break;
#if defined(BCM_SABER2_SUPPORT) 
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56270:
        /* Bring the port down and back up to clear credits */
        SOC_IF_ERROR_RETURN(soc_saber2_port_enable_set(unit, port, 0));
        SOC_IF_ERROR_RETURN(soc_saber2_port_enable_set(unit, port, 1));
        break;
#endif
    default:
        break;
    }

    return SOC_E_NONE;
}

int
soc_port_ingress_buffer_reset(int unit, soc_port_t port, int reset)
{
    switch (SOC_CHIP_GROUP(unit)) {
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    case SOC_CHIP_BCM56860:
        /* Poll until PGW buffers are empty */
        SOC_IF_ERROR_RETURN(soc_td2p_idb_buf_reset(unit, port, reset));
        break;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56965:
        SOC_IF_ERROR_RETURN(soc_tomahawk_idb_buf_reset(unit, port, reset));
        break;
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
    case SOC_CHIP_BCM56970:
        SOC_IF_ERROR_RETURN(soc_tomahawk2_idb_buf_reset(unit, port, reset));
        break;
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
    case SOC_CHIP_BCM56275:
        SOC_IF_ERROR_RETURN(soc_hurricane4_idb_buf_reset(unit, port, reset));
        break;
#endif
    case SOC_CHIP_BCM56870:
#if defined(BCM_TRIDENT3_SUPPORT)
    {
        SOC_IF_ERROR_RETURN(soc_trident3_idb_buf_reset(unit, port, reset));
    }
#endif
        break;
    case SOC_CHIP_BCM56560:
#if defined(BCM_APACHE_SUPPORT)
    {
        SOC_IF_ERROR_RETURN(soc_apache_idb_buf_reset(unit, port, reset));
    }
#endif /*  BCM_APACHE_SUPPORT */
       break; 
    case SOC_CHIP_BCM56670:
#if defined(BCM_MONTEREY_SUPPORT)
    {
        SOC_IF_ERROR_RETURN(soc_monterey_idb_buf_reset(unit, port, reset));
    }
#endif /*  BCM_APACHE_SUPPORT */
    default:
        break;
    }
    return SOC_E_NONE;
}

#if defined (BCM_TRIDENT2_SUPPORT)
STATIC int
soc_port_credit_init(int unit, soc_port_t port)
{
#ifdef BCM_XLMAC_SUPPORT
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    uint32 rval;
    int rv = SOC_E_NONE, mac_lb, speed;
    int phy_port, mmu_port, pipe;
    static soc_reg_t mmu_port_credit_regs[] = {
        ES_PIPE0_MMU_PORT_CREDITr, ES_PIPE1_MMU_PORT_CREDITr
    };

    rv = soc_mac_xl.md_lb_get(unit, port, &mac_lb);
    if (SOC_SUCCESS(rv) && mac_lb) {
        rv = soc_mac_xl.md_speed_get(unit, port, &speed);
    } else {
        rv = soc_phyctrl_speed_get(unit, port, &speed);
        if (SOC_E_UNAVAIL == rv) {
            /* PHY driver doesn't support speed_get.
             * Get the speed from MAC.
             */
            rv = soc_mac_xl.md_speed_get(unit, port, &speed);
        }
        if ((IS_HG_PORT(unit, port) && speed < 5000) &&
            !(IS_HL_PORT(unit,port))) {
            speed = 0;
        }
    }
    /* Set MMU port credit */
    pipe = SOC_PBMP_MEMBER(si->ypipe_pbm, port) ? 1 : 0;
    reg = mmu_port_credit_regs[pipe];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (speed > 21000){
        rval = 48;
    } else if (speed > 11000) {
        rval = 24;
    } else if (speed > 1000) {
        rval = 12;
    } else {
        rval = 3;
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                      mmu_port & 0x3f, rval));
#endif
    return SOC_E_NONE;
}
#endif

int
soc_port_egress_buffer_sft_reset(int unit, soc_port_t port, int reset)
{
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
#if defined (BCM_TRIDENT2_SUPPORT)
    {
        soc_info_t *si = &SOC_INFO(unit);
        soc_reg_t reg;
        soc_field_t fields;
        int phy_port, block, bindex, lanemode, lanenum, i;
        uint32 xmrval, errval, value, entry;
        static soc_reg_t egr_reset_regs[] = {
            EGR_XLPORT_BUFFER_SFT_RESET_0_Xr, EGR_XLPORT_BUFFER_SFT_RESET_1_Xr,
            EGR_XLPORT_BUFFER_SFT_RESET_0_Yr, EGR_XLPORT_BUFFER_SFT_RESET_1_Yr
        };
        static soc_field_t egr_reset_fields[] = {
            XLP0_RESETf, XLP1_RESETf, XLP2_RESETf, XLP3_RESETf,
            XLP4_RESETf, XLP5_RESETf, XLP6_RESETf, XLP7_RESETf,
            XLP8_RESETf, XLP9_RESETf, XLP10_RESETf, XLP11_RESETf,
            XLP12_RESETf, XLP13_RESETf, XLP14_RESETf, XLP15_RESETf
        };

        /* Find physical for the specified port */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];

        /* Find lane index for the specified port (base port) */
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORT) {
                break;
            }
        }

        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);

        /* Get the current mode */
        SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &xmrval));
        lanemode = soc_reg_field_get(unit, XLPORT_MODE_REGr, xmrval,
                                     XPORT0_CORE_PORT_MODEf);

        /* Figure out the current number of lanes */
        switch (lanemode) {
        case SOC_TD2_PORT_MODE_QUAD:
            lanenum = 1;
            break;
        case SOC_TD2_PORT_MODE_TRI_012:
            lanenum = bindex == 0 ? 1 : 2;
            break;
        case SOC_TD2_PORT_MODE_TRI_023:
            lanenum = bindex == 0 ? 2 : 1;
            break;
        case SOC_TD2_PORT_MODE_DUAL:
            lanenum = 2;
            break;
        case SOC_TD2_PORT_MODE_SINGLE:
            lanenum = 4;
            break;
        default:
            return SOC_E_FAIL;
        }

        if (lanenum == 1) {
            /* Don't reset egress buffer if port only take 1 lane */
            return SOC_E_NONE;
        }

        if (lanenum == 4) { /* reset all 4 lanes */
            value = 4;
        } else if (bindex == 0) { /* reset lanes 0 and 1 */
            value = 1;
        } else { /* reset lanes 2 and 3 */
            value = 2;
        }

        /* Reset egress hardware resource */
        reg = egr_reset_regs[si->port_serdes[port] / 8];
        fields = egr_reset_fields[si->port_serdes[port] % 16];

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &errval));

        if (reset) {
            /*
             * Hold EDB port buffer in reset state and disable cell
             * request generation in EP.
             */
            SOC_IF_ERROR_RETURN
                (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));
            soc_mem_field32_set(unit, EGR_ENABLEm, &entry, PRT_ENABLEf, 0);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));

            soc_reg_field_set(unit, reg, &errval, fields, value);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0,
                                                errval));
        } else {
            /*
             * Release EDB port buffer reset and enable cell request
             * generation in EP.
             */
            soc_reg_field_set(unit, reg, &errval, fields, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0,
                                                errval));

            SOC_IF_ERROR_RETURN
                (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));
            soc_mem_field32_set(unit, EGR_ENABLEm, &entry, PRT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));
            /*
             * Config credit after EGR_ENABLE is set.
             */
            SOC_IF_ERROR_RETURN(soc_port_credit_init(unit, port));
        }
    }
#endif
        break;

#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    case SOC_CHIP_BCM56860:
        SOC_IF_ERROR_RETURN(soc_td2p_edb_buf_reset(unit, port, reset));
    break;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    case SOC_CHIP_BCM56870:
    case SOC_CHIP_BCM56770:
    case SOC_CHIP_BCM56370:
    case SOC_CHIP_BCM56470:
    case SOC_CHIP_BCM56275:
#if defined(BCM_TRIDENT3_SUPPORT)
    {
        SOC_IF_ERROR_RETURN(soc_trident3_edb_buf_reset(unit, port, reset));
    }
#endif
    break;

#if defined(BCM_TOMAHAWK_SUPPORT)
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56965:
        SOC_IF_ERROR_RETURN(soc_tomahawk_edb_buf_reset(unit, port, reset));
        break;
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
    case SOC_CHIP_BCM56970:
        SOC_IF_ERROR_RETURN(soc_tomahawk2_edb_buf_reset(unit, port, reset));
        break;
#endif
    case SOC_CHIP_BCM56560:
#if defined(BCM_APACHE_SUPPORT)
    {
        SOC_IF_ERROR_RETURN(soc_apache_edb_buf_reset(unit, port, reset));
    }
#endif /*  BCM_APACHE_SUPPORT */
        break;

    case SOC_CHIP_BCM56670:

#if defined(BCM_MONTEREY_SUPPORT)
if(SOC_IS_MONTEREY(unit)) 
    {
        SOC_IF_ERROR_RETURN(soc_monterey_edb_buf_reset(unit, port, reset));
    }
#endif /*  BCM_MONTEREY_SUPPORT */
        break;

    case SOC_CHIP_BCM56070:
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit) &&
       soc_feature(unit, soc_feature_portmod))
    {
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    }
#endif /* BCM_FIRELIGHT_SUPPORT */
        break;

    default:
        break;
    }

    return SOC_E_NONE;
}

int
soc_port_fifo_reset(int unit, soc_port_t port)
{
    int phy_port, block, bindex, i;
    uint32 rval, orig_rval;
    static const soc_field_t fields[] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
#if defined(BCM_TRIDENT2_SUPPORT)
        /* OBM counters will be cleared by XLPORT_SOFT_RESET.
         * Clear in counter thread accordingly.
         */
        SOC_IF_ERROR_RETURN(soc_td2_obm_counters_reset(unit, port));
#endif /* BCM_TRIDENT2_SUPPORT */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        SOC_IF_ERROR_RETURN(READ_XLPORT_SOFT_RESETr(unit, port, &rval));
        orig_rval = rval;
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_XLPORT) {
                bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval,
                                  fields[bindex], 1);
                break;
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, orig_rval));
        break;
    case SOC_CHIP_BCM56560:
    case SOC_CHIP_BCM56670:
        {
            soc_block_t port_blk;
            soc_reg_t port_reset_reg;

            if (IS_CXX_PORT(unit, port) &&
                (SOC_INFO(unit).port_speed_max[port] >= 100000)) {
                port_blk = SOC_BLK_CLPORT;
                port_reset_reg = CLPORT_SOFT_RESETr;
            } else if (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)) {
                port_blk = SOC_BLK_CLPORT;
                port_reset_reg = CLPORT_SOFT_RESETr;
            } else if (IS_CLG2_PORT(unit, port)) {
                port_blk = SOC_BLK_CLG2PORT;
                port_reset_reg = CLPORT_SOFT_RESETr;
            } else {
                port_blk = SOC_BLK_XLPORT;
                port_reset_reg = XLPORT_SOFT_RESETr;
            }

            phy_port = SOC_INFO(unit).port_l2p_mapping[port];    
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, port_reset_reg, port, 0, &rval));
            orig_rval = rval;
            for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
                block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                if (SOC_BLOCK_INFO(unit, block).type == port_blk) {
                    bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    soc_reg_field_set(unit, port_reset_reg, &rval,
                                      fields[bindex], 1);
                    break;
                }
            }
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, port_reset_reg, port, 0, rval));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, port_reset_reg, port, 0, orig_rval));
        }
        break;
    case SOC_CHIP_BCM56260:
    case SOC_CHIP_BCM56270:
        if (SOC_REG_PORT_VALID(unit, XLPORT_SOFT_RESETr, port)) {
            bindex = ( port - 1 ) % 4;
            SOC_IF_ERROR_RETURN(READ_XLPORT_SOFT_RESETr(unit, port, &rval));
            orig_rval = rval;
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval,
                    fields[bindex], 1);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, orig_rval));
        }
        break;
    default:
        break;
    }

    return SOC_E_NONE;
}

int
soc_port_blk_init(int unit, soc_port_t port)
{    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
        if (IS_S_PORT(unit, port)) {
            soc_pbmp_t pbmp_s0, pbmp_s1, pbmp_s3, pbmp_s4;
            uint32     val32;
            int        higig_mode;

            SOC_IF_ERROR_RETURN(READ_GPORT_CONFIGr(unit, port, &val32));
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 1);
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, GPORT_ENf, 1);
            SOC_PBMP_WORD_SET(pbmp_s0, 0, 0x00000002);
            SOC_PBMP_WORD_SET(pbmp_s1, 0, 0x00000004);
            SOC_PBMP_WORD_SET(pbmp_s3, 0, 0x00000010);
            SOC_PBMP_WORD_SET(pbmp_s4, 0, 0x00000020);
 
            higig_mode = IS_ST_PORT(unit, port) ? 1 : 0;
            if (SOC_PBMP_MEMBER(pbmp_s0, port)) {
                /* The "SOP check enables" are not gated with "HiGig2 enables"
                 * and "bond_disable_stacking".
                 * Thus software need to enable/disable the SOP drop check
                 * in HiGig2 mode as desired.
                 */
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S0f, 0);
                /* Enable HiGig 2 */
                /* Assuming that always use stacking port in HiGig mode */
                /* Actually, stacking port can also be used in ethernet mode */
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S0f, higig_mode); 
            } else if (SOC_PBMP_MEMBER(pbmp_s1, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S1f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S1f, higig_mode);
            } else if (SOC_PBMP_MEMBER(pbmp_s3, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S3f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S3f, higig_mode); 
            } else if (SOC_PBMP_MEMBER(pbmp_s4, port)) {
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  DROP_ON_WRONG_SOP_EN_S4f, 0);
                soc_reg_field_set(unit, GPORT_CONFIGr, &val32,
                                  HGIG2_EN_S4f, higig_mode); 
            }

            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_CONFIGr(unit, port, val32));

            /* Reset the clear-count bit after 64 clocks */
            soc_reg_field_set(unit, GPORT_CONFIGr, &val32, CLR_CNTf, 0);
            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_CONFIGr(unit, port, val32));
        }
        break;
#endif /* BCM_RAVEN_SUPPORT */
    default:
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

int
soc_packet_purge_control_init(int unit, soc_port_t port)
{
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_TRX_SUPPORT)
    uint32 mask;
    mask = soc_property_port_get(unit, port, spn_GPORT_RSV_MASK, 0x070);
#endif
    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT)
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
        /* GPORT_RSV_MASK fields
         * bit
         * 0     frm_align_err_latch
         * 1     rx_frm_stack_vlan_latch
         * 2     rx_carrier_event_latch
         * 3     rx_frm_gmii_err_latch
         * 4     (frm_crc_err_latch | frm_crc_err)
         * 5     frm_length_err_latch & ~frm_truncate_latch
         * 6     frm_truncate_latch
         * 7     ~rx_frm_err_latch & ~frm_truncate_latch
         * 8     rx_frm_mltcast_latch
         * 9     rx_frm_broadcast_latch
         * 10    drbl_nbl_latch
         * 11    cmd_rcv_latch
         * 12    pause_rcv_latch
         * 13    rx_cmd_op_err_latch
         * 14    rx_frm_vlan_latch
         * 15    rx_frm_unicast_latch
         * 16    frm_truncate_latch
         */
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_RSV_MASKr(unit, port, mask));
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, mask));
        break;
#endif /* BCM_RAVEN_SUPPORT */
#ifdef BCM_TRX_SUPPORT
    case SOC_CHIP_BCM56624:
    case SOC_CHIP_BCM56680:
    case SOC_CHIP_BCM56634:
    case SOC_CHIP_BCM56524:
    case SOC_CHIP_BCM56685:
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56725:
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_RSV_MASKr(unit, port, mask));
        SOC_IF_ERROR_RETURN
            (WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, mask));
        break;
    case SOC_CHIP_BCM56334:
    case SOC_CHIP_BCM56142:
    case SOC_CHIP_BCM56150:
    case SOC_CHIP_BCM53400:
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53570:
    case SOC_CHIP_BCM56070:
    case SOC_CHIP_BCM53540:
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port) ||
            IS_XQ_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (WRITE_QPORT_RSV_MASKr(unit, port, mask));
            SOC_IF_ERROR_RETURN
                (WRITE_QPORT_STAT_UPDATE_MASKr(unit, port, mask)); 
        } else {
            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_RSV_MASKr(unit, port, mask));
            SOC_IF_ERROR_RETURN
                (WRITE_GPORT_STAT_UPDATE_MASKr(unit, port, mask));
        }
        break;
#endif
    default:
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}

int
soc_egress_enable(int unit, soc_port_t port, int enable)
{
#ifdef BCM_SHADOW_SUPPORT
    soc_info_t          *si;
#endif

#ifdef BCM_SHADOW_SUPPORT
    si = &SOC_INFO(unit);
#endif

    switch (SOC_CHIP_GROUP(unit)) {
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    case SOC_CHIP_BCM56820:
    case SOC_CHIP_BCM56224:
    case SOC_CHIP_BCM53314:
    case SOC_CHIP_BCM88732:
    {
        uint32 val32;
        SOC_IF_ERROR_RETURN(READ_EGR_ENABLEr(unit, port, &val32));
#ifdef BCM_SHADOW_SUPPORT
        if (!SOC_PBMP_MEMBER(si->port.disabled_bitmap, port)) {
         soc_reg_field_set(unit, EGR_ENABLEr, &val32, PRT_ENABLEf, 1);
        }
#else
        soc_reg_field_set(unit, EGR_ENABLEr, &val32, PRT_ENABLEf, 1);
#endif
        SOC_IF_ERROR_RETURN(WRITE_EGR_ENABLEr(unit, port, val32));
        break;
    }
#endif /* BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    case SOC_CHIP_BCM56860:
    {
        int phy_port;
        uint32 entry;

        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, &entry, PRT_ENABLEf, enable);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, &entry));
        break;
    }
#endif
    default:
        return SOC_E_NONE;
    }
    return SOC_E_NONE;
}

int
soc_port_speed_update(int unit, soc_port_t port, int speed) 
{
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
#ifdef BCM_TRIDENT2_SUPPORT
        SOC_IF_ERROR_RETURN(soc_trident2_port_speed_update(unit, port, speed));
#endif /* BCM_TRIDENT2_SUPPORT */
        break;
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56965:
#ifdef BCM_TOMAHAWK_SUPPORT
        SOC_IF_ERROR_RETURN(soc_tomahawk_port_speed_update(unit, port, speed));
#endif /* BCM_TOMAHAWK_SUPPORT */
        break;
    case SOC_CHIP_BCM56560:
#ifdef BCM_APACHE_SUPPORT
        SOC_IF_ERROR_RETURN(soc_apache_port_speed_update(unit, port, speed));
#endif /* BCM_APACHE_SUPPORT */
        break;
    case SOC_CHIP_BCM56670:
#ifdef BCM_MONTEREY_SUPPORT
        SOC_IF_ERROR_RETURN(soc_monterey_port_speed_update(unit, port, speed));
#endif /* BCM_MONTEREY_SUPPORT */
        break;
    case SOC_CHIP_BCM56870:
#ifdef BCM_TRIDENT3_SUPPORT
        SOC_IF_ERROR_RETURN(soc_trident3_port_speed_update(unit, port, speed));
#endif /* BCM_TRIDENT3_SUPPORT */
        break;
    case SOC_CHIP_BCM53570:
#ifdef BCM_GREYHOUND2_SUPPORT
        SOC_IF_ERROR_RETURN(
                soc_greyhound2_port_speed_update(unit, port, speed));
#endif /* BCM_GREYHOUND2_SUPPORT */
        break;
    case SOC_CHIP_BCM56070:
#ifdef BCM_FIRELIGHT_SUPPORT
        SOC_IF_ERROR_RETURN(
                soc_firelight_port_speed_update(unit, port, speed));
#endif /* BCM_FIRELIGHT_SUPPORT */
            break;
    default:
        break;
    }

    return SOC_E_NONE;

}

int
soc_port_hg_speed_get(int unit, soc_port_t port, int *speed)
{
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56960:
    case SOC_CHIP_BCM56965:
#ifdef BCM_TOMAHAWK_SUPPORT
        SOC_IF_ERROR_RETURN(soc_th_port_hg_speed_get(unit, port, speed));
#endif /* BCM_TOMAHAWK_SUPPORT */
        break;
#ifdef BCM_GREYHOUND2_SUPPORT
    case SOC_CHIP_BCM53570:
        SOC_IF_ERROR_RETURN(soc_greyhound2_port_hg_speed_get(unit, port, speed));
        break;
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined (BCM_FIRELIGHT_SUPPORT)
    case SOC_CHIP_BCM56070:
        SOC_IF_ERROR_RETURN(soc_firelight_port_hg_speed_get(unit, port, speed));
        break;
#endif /* BCM_FIRELIGHT_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400:
        SOC_IF_ERROR_RETURN(soc_greyhound_port_hg_speed_get(unit, port, speed));
        break;
#endif /* BCM_GREYHOUND_SUPPORT */
    default:
        break;
    }

    return SOC_E_NONE;
}

int
soc_port_thdo_rx_enable_set(int unit, soc_port_t port, int enable) 
{
    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM56850:
    case SOC_CHIP_BCM56860:
#ifdef BCM_TRIDENT2_SUPPORT
        SOC_IF_ERROR_RETURN
            (soc_trident2_port_thdo_rx_enable_set(unit, port, enable));
#endif
        break;
    case SOC_CHIP_BCM56560:
#ifdef BCM_APACHE_SUPPORT
        SOC_IF_ERROR_RETURN(soc_apache_port_thdo_rx_enable_set(unit,
                                                               port, enable));
#endif
    break;
    case SOC_CHIP_BCM56670:
#ifdef BCM_MONTEREY_SUPPORT
        SOC_IF_ERROR_RETURN(soc_monterey_port_thdo_rx_enable_set(unit,
                                                               port, enable));
#endif 
     break; 
    default:
        break;
    }

    return SOC_E_NONE;

}

#endif /* defined(BCM_ESW_SUPPORT) */

int
soc_llfc_xon_set(int unit, soc_port_t port, int enable)
{
    if (enable) {
        /* no action required */
    } else {
#if defined(BCM_ESW_SUPPORT)
        if (soc_feature(unit, soc_feature_llfc_force_xon)) {
            SOC_IF_ERROR_RETURN(soc_mmu_backpressure_clear(unit, port));
        }
#endif /* defined(BCM_ESW_SUPPORT) */
    }

    return SOC_E_NONE;
}

/* function to setup EEE Wake/LPI timers per speed */
int
soc_port_eee_timers_setup(int unit, soc_port_t port, int speed)
{
    switch (SOC_CHIP_GROUP(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400:
        SOC_IF_ERROR_RETURN(soc_gh_port_eee_timers_setup(unit, port, speed));
        break;
#endif  /* BCM_GREYHOUND_SUPPORT */
    default :
        break;
    }
    return SOC_E_NONE;
}
 
/* function to provide the ways to assigning EEE timers during MAC init.
 *
 * Note :
 *  1. MAC has it own EEE timer default value but these setting for some
 *      specific chip might not proper for EEE operation.
 *  2. Once EEE timers at reset value=0 on a MAC, Packet lost will happen.
 */
int
soc_port_eee_timers_init(int unit, soc_port_t port, int speed)
{
    if (!soc_feature(unit, soc_feature_eee)) {
        return SOC_E_NONE;
    }
 
    switch (SOC_CHIP_GROUP(unit)) {
#ifdef BCM_GREYHOUND_SUPPORT
    case SOC_CHIP_BCM53400:
        SOC_IF_ERROR_RETURN(soc_gh_port_eee_ref_count_init(unit, port));
        SOC_IF_ERROR_RETURN(soc_gh_port_eee_timers_setup(unit, port, speed));
        break;
#endif  /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
    case SOC_CHIP_BCM56275:
        if (IS_EGPHY_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, UMAC_EEE_REF_COUNTr, port,
                                        EEE_REF_COUNTf, 250));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, GMII_EEE_WAKE_TIMERr, port,
                                        GMII_EEE_WAKE_TIMERf, 17));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, GMII_EEE_DELAY_ENTRY_TIMERr, port,
                                        GMII_EEE_LPI_TIMERf, 4));
        }

        break;
#endif  /* BCM_HURRICANE4_SUPPORT */

    default :
        break;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_granular_speed_get
 * Purpose:
 *      Map MMU_CELL_CREDIT to port speed
 * Parameters:
 *      unit   - (IN) unit number
 *      port   - (IN) port number
 *      speed  - (OUT) speed in Mbps
 * Returns:
 *      N/A
 */
soc_error_t
soc_granular_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_error_t rc;

    switch (SOC_CHIP_GROUP(unit)) {
#if defined (BCM_TRIDENT3_SUPPORT)
        case SOC_CHIP_BCM56870:
            rc = soc_td3_granular_speed_get(unit, port, speed);
            break;
#endif
#if defined (BCM_TOMAHAWK_SUPPORT)
        case SOC_CHIP_BCM56960:
        case SOC_CHIP_BCM56965:
        case SOC_CHIP_BCM56970:
            rc = soc_th_granular_speed_get(unit, port, speed);
            break;
#endif
#if defined (BCM_GREYHOUND2_SUPPORT)
        case SOC_CHIP_BCM53570:
            rc = soc_greyhound2_granular_speed_get(unit, port, speed);
            break;
#endif  /* BCM_GREYHOUND2_SUPPORT */
#if defined (BCM_FIRELIGHT_SUPPORT)
        case SOC_CHIP_BCM56070:
            rc = soc_firelight_granular_speed_get(unit, port, speed);
            break;
#endif /* BCM_FIRELIGHT_SUPPORT */
        default:
            rc = SOC_E_PARAM;
            break;
    }
    return rc;
}

/*
 * Function:
 *      soc_mac_x_sync_fifo_reset
 * Purpose:
 *      Reset a XMAC loopback syc fifo
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mac_x_sync_fifo_reset(int unit, soc_port_t port)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) && SOC_INFO(unit).port_num_lanes[port] == 4) {
        static const soc_field_t line_lb_field[] = {
            XMAC0_LINE_LPBK_RESETf,
            XMAC1_LINE_LPBK_RESETf,
            XMAC2_LINE_LPBK_RESETf
        };
        int phy_port, bindex = -1;
        uint32 rval;
        
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        if (phy_port != -1) {
            bindex = SOC_PORT_BINDEX(unit, phy_port) / 4;
            if (bindex != -1) {
                SOC_IF_ERROR_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval));
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval, 
                                  line_lb_field[bindex], 1);
                SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval));
                sal_udelay(10);
                soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval, 
                                  line_lb_field[bindex], 0);    
                SOC_IF_ERROR_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval)); 
            }
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_pgw_ge_rx_fifo_reset
 * Purpose:
 *      Reset PGW Rx FIFO to prevent packets entering switch
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      reset - TRUE/FALSE
 * Returns:
 *      SOC_E_XXX
 */
int soc_pgw_rx_fifo_reset(int unit, int port, int reset)
{
    if (!soc_feature(unit, soc_feature_unimac_reset_wo_clock)) {
        return SOC_E_UNAVAIL;
    }

    switch (SOC_CHIP_GROUP(unit)) {
#ifdef BCM_HURRICANE3_SUPPORT
    case SOC_CHIP_BCM56160:
    case SOC_CHIP_BCM53540:
        SOC_IF_ERROR_RETURN
            (soc_hurricane3_pgw_rx_fifo_reset(unit, port, reset));
        break;
#endif  /* BCM_HURRICANE3_SUPPORT */
    default :
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_port_mmu_buffer_enable
 * Purpose:
 *      MMU memory buffer enable configuration to specify which
 *      egress ports are allowed to buffer packets.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      enable - TRUE/FALSE
 * Returns:
 *      SOC_E_XXX
 */
int
soc_port_mmu_buffer_enable(int unit, int port, int enable)
{
    switch (SOC_CHIP_GROUP(unit)) {
#if defined (BCM_HURRICANE2_SUPPORT)
        case SOC_CHIP_BCM56150:
        case SOC_CHIP_BCM53400:
        case SOC_CHIP_BCM56160:
        {
            uint32      val, pbmp;
            pbmp_t      mmu_pbmp;

            SOC_PBMP_CLEAR(mmu_pbmp);
            SOC_IF_ERROR_RETURN(READ_MMUPORTENABLEr(unit, &val));
            pbmp = soc_reg_field_get(unit, MMUPORTENABLEr, val, MMUPORTENABLEf);
            SOC_PBMP_WORD_SET(mmu_pbmp, 0, pbmp);
            if (enable) {
                SOC_PBMP_PORT_ADD(mmu_pbmp, port);
            } else {
                SOC_PBMP_PORT_REMOVE(mmu_pbmp, port);
            }
            soc_reg_field_set(unit, MMUPORTENABLEr, &val, MMUPORTENABLEf,
                              SOC_PBMP_WORD_GET(mmu_pbmp, 0));
            SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLEr(unit, val));
            break;
        }
#endif /* BCM_HURRICANE2_SUPPORT */
#if defined (BCM_GREYHOUND2_SUPPORT)
        case SOC_CHIP_BCM53570:
#if defined (BCM_FIRELIGHT_SUPPORT)
        case SOC_CHIP_BCM56070:
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            soc_info_t  *si = &SOC_INFO(unit);
            pbmp_t      mmu_pbmp;
            uint32      val, pbmp;
            int         phy_port, mmu_port, word;
            static const soc_reg_t mmu_reg[] = {MMUPORTENABLE_0r,
                                                MMUPORTENABLE_1r,
                                                MMUPORTENABLE_2r};

            SOC_PBMP_CLEAR(mmu_pbmp);
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            word = mmu_port / 32;
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, mmu_reg[word], REG_PORT_ANY, 0, &val));
            pbmp = soc_reg_field_get(unit, mmu_reg[word], val, MMUPORTENABLEf);
            SOC_PBMP_WORD_SET(mmu_pbmp, word, pbmp);
            if (enable) {
                SOC_PBMP_PORT_ADD(mmu_pbmp, mmu_port);
            } else {
                SOC_PBMP_PORT_REMOVE(mmu_pbmp, mmu_port);
            }
            soc_reg_field_set(unit, mmu_reg[word], &val, MMUPORTENABLEf,
                              SOC_PBMP_WORD_GET(mmu_pbmp, word));
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, mmu_reg[word], REG_PORT_ANY, 0, val));
            break;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
        default:
            break;
    }
    return SOC_E_NONE;
}
