/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag routines to identify registers that are only implemented
 * on a subset of ports/cos.
 */

#include <soc/drv.h>
#include <appl/diag/system.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#include <soc/mcm/memregs.h>
#endif
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#include <shared/bsl.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
#include <soc/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif /*BCM_DFE_SUPPORT*/
#define _MAP_REG_MASK_SUBSETS(unit, i, count, arr) \
    for (i = 0; i < count; i++) {\
        *arr[i] = SOC_REG_MASK_SUBSET(unit)[i];\
    }

#define _INIT_REG_MASK_SUBSETS(unit, i, count, arr) \
    SOC_REG_MASK_SUBSET(unit) = sal_alloc(count * sizeof(pbmp_t *),\
                                          "per unit reg mask subsets");\
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {\
        return SOC_E_MEMORY;\
    }\
    sal_memset(SOC_REG_MASK_SUBSET(unit), 0, count * sizeof(pbmp_t *));\
    for (i = 0; i < count; i++) {\
        SOC_REG_MASK_SUBSET(unit)[i] = sal_alloc(sizeof(pbmp_t),\
                                                 "reg mask subsets");\
        if (NULL == SOC_REG_MASK_SUBSET(unit)[i]) {\
            return SOC_E_MEMORY;\
        }\
        *mask_map_arr[i] = SOC_REG_MASK_SUBSET(unit)[i];\
    }

#define CHECK_FEATURE_BLK(feat, blk) \
    (SOC_REG_BLOCK_IS(unit, ainfo->reg, blk) && \
    (!soc_feature(unit, feat)))

/*
 * Description:
 *  There are many per-port registers in Triumph which only exist on
 *  a subset of ports as described by PORTLIST and PERPORT_MASKBITS
 *  attributes in the regsfile. There are also several indexed registers
 *  with different number of elements depending on the port as described
 *  by the NUMELS_PERPORT attribute. These routines identify whether
 *  a register should be skipped for a given port/cos index. These routines 
 *  also adjust the mask for these special registers so unimplemented
 *  bits get skipped during register tests. 
 */



#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
reg_mask_subset_tr(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *tr_8pg_ports;
    pbmp_t *tr_higig_ports;
    pbmp_t *tr_24q_ports;
    pbmp_t *tr_non_cpu_ports;
    pbmp_t *tr_all_ports;
#define _REG_MASK_SUBSET_MAX_TR 5
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_TR];
    int i, count = _REG_MASK_SUBSET_MAX_TR;
    uint64 temp_mask;
    pbmp_t *pbmp;

    mask_map_arr[0] = &tr_8pg_ports;
    mask_map_arr[1] = &tr_higig_ports;
    mask_map_arr[2] = &tr_24q_ports;
    mask_map_arr[3] = &tr_non_cpu_ports;
    mask_map_arr[4] = &tr_all_ports;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
        
        /* 8PG_PORTS = [2,14,26..31] */
        SOC_PBMP_CLEAR(*tr_8pg_ports);
        if (IS_XE_PORT(unit, 2) || IS_HG_PORT(unit, 2)) {
            SOC_PBMP_PORT_ADD(*tr_8pg_ports, 2);
        }
        if (IS_XE_PORT(unit, 14) || IS_HG_PORT(unit, 14)) {
            SOC_PBMP_PORT_ADD(*tr_8pg_ports, 14);
        }
        if (IS_XE_PORT(unit, 26) || IS_HG_PORT(unit, 26)) {
            SOC_PBMP_PORT_ADD(*tr_8pg_ports, 26);
        }
        if (IS_XE_PORT(unit, 27) || IS_HG_PORT(unit, 27)) {
            SOC_PBMP_PORT_ADD(*tr_8pg_ports, 27);
        }
        SOC_PBMP_PORT_ADD(*tr_8pg_ports, 28);
        SOC_PBMP_PORT_ADD(*tr_8pg_ports, 29);
        SOC_PBMP_PORT_ADD(*tr_8pg_ports, 30); 
        SOC_PBMP_PORT_ADD(*tr_8pg_ports, 31);

        /* Higig ports [0,2,14,26,27,28,29,30,31] */
        SOC_PBMP_CLEAR(*tr_higig_ports);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 0);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 2);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 14);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 26);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 27);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 28);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 29);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 30);
        SOC_PBMP_PORT_ADD(*tr_higig_ports, 31);

        /*  24Q_PORTS = [2,3,14,15,26..32,43] */
        SOC_PBMP_CLEAR(*tr_24q_ports);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 2);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 3);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 14);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 15);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 26);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 27);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 28);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 29);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 30);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 31);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 32);
        SOC_PBMP_PORT_ADD(*tr_24q_ports, 43);

        /* all port except CMIC */
        SOC_PBMP_CLEAR(*tr_non_cpu_ports);
        SOC_PBMP_ASSIGN(*tr_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*tr_non_cpu_ports, 0);
 
        /* All ports */
        SOC_PBMP_CLEAR(*tr_all_ports); 
        SOC_PBMP_ASSIGN(*tr_all_ports, PBMP_ALL(unit)); 
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
 
    switch(ainfo->reg) {
        case MAC_CTRLr:
        case MAC_XGXS_CTRLr:
        case MAC_XGXS_STATr:
        case MAC_TXMUXCTRLr:
        case MAC_CNTMAXSZr:
        case MAC_CORESPARE0r:
        case MAC_TXCTRLr:
        case MAC_TXMACSAr:
        case MAC_TXMAXSZr:
        case MAC_TXPSETHRr:
        case MAC_TXSPARE0r:
        case ITPOKr:
        case ITXPFr: 
        case ITFCSr:
        case ITUCr:
        case ITMCAr:
        case ITBCAr:
        case ITOVRr:
        case ITFRGr:
        case ITPKTr:
        case IT64r:
        case IT127r:
        case IT255r:
        case IT511r:
        case IT1023r:
        case IT1518r:
        case IT2047r:
        case IT4095r:
        case IT9216r:
        case IT16383r:
        case ITMAXr:
        case ITUFLr:
        case ITERRr:
        case ITBYTr:
        case MAC_RXCTRLr:
        case MAC_RXMACSAr:
        case MAC_RXMAXSZr:
        case MAC_RXLSSCTRLr:
        case MAC_RXLSSSTATr:
        case MAC_RXSPARE0r:
        case IR64r:
        case IR127r:
        case IR255r:
        case IR511r:
        case IR1023r:
        case IR1518r:
        case IR2047r:
        case IR4095r:
        case IR9216r:
        case IR16383r:
        case IRMAXr:
        case IRPKTr:
        case IRFCSr:
        case IRUCr:
        case IRMCAr:
        case IRBCAr:
        case IRXCFr:
        case IRXPFr:
        case IRXUOr:
        case IRJBRr:
        case IROVRr:
        case IRFLRr:
        case IRPOKr:
        case IRMEGr:
        case IRMEBr:
        case IRBYTr:
        case IRUNDr:
        case IRFRGr:
        case IRERBYTr:
        case IRERPKTr:
        case IRJUNKr:
        case MAC_TXLLFCCTRLr:
        case MAC_TXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGCNTr:
            if (IS_GE_PORT(unit, ainfo->port)) {
                goto skip;
            }
            break;
        default:
            break;
    }

    switch(ainfo->reg) {
        case IE2E_CONTROLr:
        case ING_MODMAP_CTRLr:
        case IHG_LOOKUPr:
        case ICONTROL_OPCODE_BITMAPr:
        case UNKNOWN_HGI_BITMAPr:
        case IUNHGIr:
        case ICTRLr:
        case IBCASTr:
        case ILTOMCr:
        case IIPMCr:
        case IUNKOPCr:
            pbmp = tr_higig_ports;
            break;
        case PG_THRESH_SELr:
        case PORT_PRI_GRP0r:
        case PORT_PRI_GRP1r:
        case PORT_PRI_XON_ENABLEr:
            pbmp = tr_8pg_ports;
            break;
        case ECN_CONFIGr:
        case HOL_STAT_PORTr:
        case PORT_WREDPARAM_CELLr:
        case PORT_WREDPARAM_YELLOW_CELLr:
        case PORT_WREDPARAM_RED_CELLr:
        case PORT_WREDPARAM_NONTCP_CELLr:
        case PORT_WREDCONFIG_CELLr:
        case PORT_WREDAVGQSIZE_CELLr:
        case PORT_WREDPARAM_PACKETr:
        case PORT_WREDPARAM_YELLOW_PACKETr:
        case PORT_WREDPARAM_RED_PACKETr:
        case PORT_WREDPARAM_NONTCP_PACKETr:
        case PORT_WREDCONFIG_PACKETr:
        case PORT_WREDAVGQSIZE_PACKETr:
        case ESCONFIGr:
        case COSMASKr:
        case MINSPCONFIGr:
            pbmp = tr_non_cpu_ports;
            break;
        /* ING_PORTS_24Q */
        case ING_COS_MODEr:
        /* MMU_24Q_PORTS */
        case COS_MODEr:
        case S1V_CONFIGr:
        case S1V_COSWEIGHTSr:
        case S1V_COSMASKr:
        case S1V_MINSPCONFIGr:
        case S1V_WDRRCOUNTr:
            pbmp = tr_24q_ports;
            break;
        /* MMU_PERPORTPERCOS_REGS */
        case OP_QUEUE_CONFIG_CELLr:
        case OP_QUEUE_CONFIG1_CELLr:
        case OP_QUEUE_CONFIG_PACKETr:
        case OP_QUEUE_CONFIG1_PACKETr:
        case OP_QUEUE_LIMIT_YELLOW_CELLr:
        case OP_QUEUE_LIMIT_YELLOW_PACKETr:
        case OP_QUEUE_LIMIT_RED_CELLr:
        case OP_QUEUE_LIMIT_RED_PACKETr:
        case OP_QUEUE_RESET_OFFSET_CELLr:
        case OP_QUEUE_RESET_OFFSET_PACKETr:
        case OP_QUEUE_MIN_COUNT_CELLr:
        case OP_QUEUE_MIN_COUNT_PACKETr:
        case OP_QUEUE_SHARED_COUNT_CELLr:
        case OP_QUEUE_SHARED_COUNT_PACKETr:
        case OP_QUEUE_TOTAL_COUNT_CELLr:
        case OP_QUEUE_TOTAL_COUNT_PACKETr:
        case OP_QUEUE_RESET_VALUE_CELLr:
        case OP_QUEUE_RESET_VALUE_PACKETr:
        case DROP_PKT_CNTr:
        case DROP_BYTE_CNTr:
            if (ainfo->port == 0) {
                return 0;
            } else if (ainfo->idx < 8) {
                pbmp = tr_all_ports;
            } else if (ainfo->idx < 24) {
                pbmp = tr_24q_ports;
            } else {
                goto skip;
            }
            break;
        /* MMU_PERPORTPERCOS_NOCPU_REGS */
        case WREDPARAM_CELLr:
        case WREDPARAM_YELLOW_CELLr:
        case WREDPARAM_RED_CELLr:
        case WREDPARAM_NONTCP_CELLr:
        case WREDCONFIG_CELLr:
        case WREDAVGQSIZE_CELLr:
        case WREDPARAM_PACKETr:
        case WREDPARAM_YELLOW_PACKETr:
        case WREDPARAM_RED_PACKETr:
        case WREDPARAM_NONTCP_PACKETr:
        case WREDCONFIG_PACKETr:
        case WREDAVGQSIZE_PACKETr:
        /* MMU_MTRO_REGS */
        case MINBUCKETCONFIG_64r:
        case MINBUCKETr:
        case MAXBUCKETCONFIG_64r:
        case MAXBUCKETr:
        /* MMU_ES_REGS */
        case COSWEIGHTSr:
        case WDRRCOUNTr:
            if (ainfo->port == 0) {
                goto skip;
            } else if (ainfo->idx < 8) {
                pbmp = tr_all_ports;
            } else {
                pbmp = tr_24q_ports;
            }
            break;
        /* MMU_PERPORTPERPRI_REGS */
        case PG_RESET_OFFSET_CELLr:
        case PG_RESET_OFFSET_PACKETr:
        case PG_RESET_FLOOR_CELLr:
        case PG_MIN_CELLr:
        case PG_MIN_PACKETr:
        case PG_HDRM_LIMIT_CELLr:
        case PG_HDRM_LIMIT_PACKETr:
        case PG_COUNT_CELLr:
        case PG_COUNT_PACKETr:
        case PG_MIN_COUNT_CELLr:
        case PG_MIN_COUNT_PACKETr:
        case PG_PORT_MIN_COUNT_CELLr:
        case PG_PORT_MIN_COUNT_PACKETr:
        case PG_SHARED_COUNT_CELLr:
        case PG_SHARED_COUNT_PACKETr:
        case PG_HDRM_COUNT_CELLr:
        case PG_HDRM_COUNT_PACKETr:
        case PG_GBL_HDRM_COUNTr:
        case PG_RESET_VALUE_CELLr:
        case PG_RESET_VALUE_PACKETr:
            if (ainfo->idx == 0) {
                pbmp = tr_all_ports;
            } else {
                pbmp = tr_8pg_ports;
            }
            break;
        default:
            pbmp = tr_all_ports;
            break; 
    }

    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip; 
    }

    if (mask != NULL) {
        switch(ainfo->reg) {
            case HOL_STAT_PORTr:
            case ECN_CONFIGr:
                if (!SOC_PBMP_MEMBER(*tr_24q_ports, ainfo->port)) {
                    /* adjust mask for ports without 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
            case TOQ_QUEUESTAT_64r:
            case TOQ_ACTIVATEQ_64r:
            case TOQEMPTY_64r:
            case DEQ_AGINGMASK_64r:
                if (SOC_PBMP_MEMBER(*tr_24q_ports, ainfo->port)) {
                    /* adjust mask for ports with 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x00ffffff);
                    COMPILER_64_AND(*mask, temp_mask);
                } else if (ainfo->port != 0) {
                    /* remaining ports only have 8 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
        }
    }
    return 0; 
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
STATIC int
reg_mask_subset_sh(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *sh_non_cpu_ports;
    pbmp_t *sh_non_il_non_cpu_ports;
    pbmp_t *sh_port_link_ports;
    pbmp_t *sh_il_ports;
    pbmp_t *sh_all_ports;
#define _REG_MASK_SUBSET_MAX_SH 5
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_SH];
    int i, count = _REG_MASK_SUBSET_MAX_SH;
    pbmp_t *pbmp;

    mask_map_arr[0] = &sh_non_cpu_ports;
    mask_map_arr[1] = &sh_non_il_non_cpu_ports;
    mask_map_arr[2] = &sh_port_link_ports;
    mask_map_arr[3] = &sh_il_ports;
    mask_map_arr[4] = &sh_all_ports;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
        
        /* all port except CMIC */
        SOC_PBMP_CLEAR(*sh_non_cpu_ports);
        SOC_PBMP_ASSIGN(*sh_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*sh_non_cpu_ports, 0);

        /* All ports except Interlaken and CPU */
        SOC_PBMP_CLEAR(*sh_non_il_non_cpu_ports);
        SOC_PBMP_ASSIGN(*sh_non_il_non_cpu_ports, PBMP_PORT_ALL(unit));
        SOC_PBMP_REMOVE(*sh_non_il_non_cpu_ports, PBMP_IL_ALL(unit));

        /* All port link ports */
        SOC_PBMP_CLEAR(*sh_port_link_ports);
        SOC_PBMP_ASSIGN(*sh_port_link_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*sh_port_link_ports, 0);
        SOC_PBMP_WORD_SET(*sh_port_link_ports, 0, 
                          (SOC_PBMP_WORD_GET(*sh_port_link_ports, 0) & 0x1ff));

        /* All Interlaken ports */
        SOC_PBMP_CLEAR(*sh_il_ports);
        SOC_PBMP_ASSIGN(*sh_il_ports, PBMP_PORT_ALL(unit));
        SOC_PBMP_REMOVE(*sh_il_ports, *sh_port_link_ports);

        /* All ports */
        SOC_PBMP_CLEAR(*sh_all_ports);
        SOC_PBMP_ASSIGN(*sh_all_ports, PBMP_ALL(unit));
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

    switch(ainfo->reg) {
        case BUCKET_ECCr:
        case CONFIG_ECCr:
        case HOL_STAT_PORTr:
        case TOQ_QUEUESTATr:
        case TOQ_ACTIVATEQr:
        case TOQEMPTYr:
        case ECN_CONFIGr:
        case ES_PORTGRP_WDRR_WEIGHTSr:
        case MTRO_SHAPE_MINMASKr:
        case MMU_TO_XLP_BKP_STATUSr:
        case XLP_TO_MMU_BKP_STATUSr:
        case PORT_WREDPARAM_CELLr:
        case PORT_WREDPARAM_YELLOW_CELLr:
        case PORT_WREDPARAM_RED_CELLr:
        case PORT_WREDPARAM_NONTCP_CELLr:
        case PORT_WREDCONFIG_CELLr:
        case PORT_WREDAVGQSIZE_CELLr:
        case PORT_LB_WREDAVGQSIZE_CELLr:
        case PORT_WREDCONFIG_ECCPr:
        case PORT_WREDPARAM_END_CELLr:
        case PORT_WREDPARAM_PRI0_END_CELLr:
        case PORT_WREDPARAM_PRI0_START_CELLr:
        case PORT_WREDPARAM_RED_END_CELLr:
        case PORT_WREDPARAM_RED_START_CELLr:
        case PORT_WREDPARAM_START_CELLr:
        case PORT_WREDPARAM_YELLOW_END_CELLr:
        case PORT_WREDPARAM_YELLOW_START_CELLr:
        case PORT_WRED_THD_0_ECCPr:
        case PORT_WRED_THD_1_ECCPr:
        case WREDPARAM_CELLr:
        case WREDPARAM_YELLOW_CELLr:
        case WREDPARAM_RED_CELLr:
        case WREDPARAM_NONTCP_CELLr:
        case WREDCONFIG_CELLr:
        case WREDAVGQSIZE_CELLr:
        case WREDPARAM_END_CELLr:
        case WREDPARAM_PRI0_END_CELLr:
        case WREDPARAM_PRI0_START_CELLr:
        case WREDPARAM_RED_END_CELLr:
        case WREDPARAM_RED_START_CELLr:
        case WREDPARAM_START_CELLr:
        case WREDPARAM_YELLOW_END_CELLr:
        case WREDPARAM_YELLOW_START_CELLr:
        case WREDCONFIG_ECCPr:
        case WRED_DEBUG_ENQ_DROP_PORTr:
        case WRED_THD_0_ECCPr:
        case WRED_THD_1_ECCPr:
        case COSWEIGHTSr:
        case MINSPCONFIGr:
        case WDRRCOUNTr:
        case MINBUCKETCONFIGr:
        case MINBUCKETCONFIG1r:
        case MINBUCKETr:
        case MAXBUCKETCONFIGr:
        case MAXBUCKETCONFIG1r:
        case MAXBUCKETr:
        case MMU_TO_XPORT_BKPr:
        case DEQ_AGINGMASKr:
        case MMU_LLFC_RX_CONFIGr:
        case XPORT_TO_MMU_BKPr:
        case UNIMAC_PFC_CTRLr:
        case MAC_PFC_REFRESH_CTRLr:
        case MAC_PFC_TYPEr:
        case MAC_PFC_OPCODEr:
        case MAC_PFC_DA_0r:
        case MAC_PFC_DA_1r:
        case MACSEC_PROG_TX_CRCr:
        case MACSEC_CNTRLr:
        case TS_STATUS_CNTRLr:
        case TX_TS_DATAr:
        case BMAC_PFC_CTRLr:
        case BMAC_PFC_TYPEr:
        case BMAC_PFC_OPCODEr:
        case BMAC_PFC_DA_LOr:
        case BMAC_PFC_DA_HIr:
        case BMAC_PFC_COS0_XOFF_CNTr:
        case BMAC_PFC_COS1_XOFF_CNTr:
        case BMAC_PFC_COS2_XOFF_CNTr:
        case BMAC_PFC_COS3_XOFF_CNTr:
        case BMAC_PFC_COS4_XOFF_CNTr:
        case BMAC_PFC_COS5_XOFF_CNTr:
        case BMAC_PFC_COS6_XOFF_CNTr:
        case BMAC_PFC_COS7_XOFF_CNTr:
        case BMAC_PFC_COS8_XOFF_CNTr:
        case BMAC_PFC_COS9_XOFF_CNTr:
        case BMAC_PFC_COS10_XOFF_CNTr:
        case BMAC_PFC_COS11_XOFF_CNTr:
        case BMAC_PFC_COS12_XOFF_CNTr:
        case BMAC_PFC_COS13_XOFF_CNTr:
        case BMAC_PFC_COS14_XOFF_CNTr:
        case BMAC_PFC_COS15_XOFF_CNTr:
        case PFC_COS0_XOFF_CNTr:
        case PFC_COS1_XOFF_CNTr:
        case PFC_COS2_XOFF_CNTr:
        case PFC_COS3_XOFF_CNTr:
        case PFC_COS4_XOFF_CNTr:
        case PFC_COS5_XOFF_CNTr:
        case PFC_COS6_XOFF_CNTr:
        case PFC_COS7_XOFF_CNTr:
        case PFC_COS8_XOFF_CNTr:
        case PFC_COS9_XOFF_CNTr:
        case PFC_COS10_XOFF_CNTr:
        case PFC_COS11_XOFF_CNTr:
        case PFC_COS12_XOFF_CNTr:
        case PFC_COS13_XOFF_CNTr:
        case PFC_COS14_XOFF_CNTr:
        case PFC_COS15_XOFF_CNTr:
            pbmp = sh_non_cpu_ports;
            break;
        default:
            pbmp = sh_all_ports;
            break;
    }

    switch(ainfo->reg) {
        /* EP_PERPORTPERCOS_REGS */
        case EGR_PERQ_XMT_COUNTERSr:
        /* MMU_PERPORTPERCOS_REGS */
        case OP_QUEUE_CONFIGr:
        case OP_QUEUE_RESET_OFFSETr:
        case OP_QUEUE_MIN_COUNTr:
        case OP_QUEUE_SHARED_COUNTr:
        case OP_QUEUE_TOTAL_COUNTr:
        case OP_QUEUE_RESET_VALUEr:
        case OP_QUEUE_LIMIT_YELLOWr:
        case OP_QUEUE_LIMIT_REDr:
        case HOLDROP_PKT_CNTr:
            if (ainfo->port == 0) {
                return 0;
            } else if (ainfo->idx < 10) { 
                pbmp = sh_all_ports;
            } else {
                goto skip;
            }
            break;
        default:
            break;
    }

    if (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_XLPORT)) {
        pbmp = sh_non_il_non_cpu_ports;
    } else if ((SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_MS_ISEC)) ||
               (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_MS_ESEC))) { 
        pbmp = sh_port_link_ports;
    } else if (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_IL)) {
        pbmp = sh_il_ports;
    }

    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip;
    }
    return 0;

skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
STATIC int
reg_mask_subset_sc(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *sc_no_ports;
    pbmp_t *sc_xports;
    pbmp_t *sc_non_cpu_ports;
    pbmp_t *sc_all_ports;
#define _REG_MASK_SUBSET_MAX_SC 4
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_SC];
    int i, count = _REG_MASK_SUBSET_MAX_SC;
    pbmp_t *pbmp;

    mask_map_arr[0] = &sc_no_ports;
    mask_map_arr[1] = &sc_xports;
    mask_map_arr[2] = &sc_non_cpu_ports;
    mask_map_arr[3] = &sc_all_ports;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
        
        /* all port except CMIC */
        SOC_PBMP_CLEAR(*sc_non_cpu_ports);
        SOC_PBMP_ASSIGN(*sc_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*sc_non_cpu_ports, 0);

        SOC_PBMP_CLEAR(*sc_xports);
        SOC_PBMP_ASSIGN(*sc_xports, PBMP_GX_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*sc_xports, 0);

        /* All ports */
        SOC_PBMP_CLEAR(*sc_all_ports);
        SOC_PBMP_ASSIGN(*sc_all_ports, PBMP_ALL(unit));

        /* No ports */
        SOC_PBMP_CLEAR(*sc_no_ports);
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

    switch(ainfo->reg) {
        case HOL_STAT_PORTr:
        case TOQ_QUEUESTATr:
        case TOQ_ACTIVATEQr:
        case TOQEMPTYr:
        case ECN_CONFIGr:
        case PORT_WREDPARAM_CELLr:
        case PORT_WREDPARAM_YELLOW_CELLr:
        case PORT_WREDPARAM_RED_CELLr:
        case PORT_WREDPARAM_NONTCP_CELLr:
        case PORT_WREDCONFIG_CELLr:
        case PORT_WREDAVGQSIZE_CELLr:
        case WREDPARAM_CELLr:
        case WREDPARAM_YELLOW_CELLr:
        case WREDPARAM_RED_CELLr:
        case WREDPARAM_NONTCP_CELLr:
        case WREDCONFIG_CELLr:
        case WREDAVGQSIZE_CELLr:
        case COSWEIGHTSr:
        case MINSPCONFIGr:
        case WDRRCOUNTr:
        case MINBUCKETCONFIGr:
        case MINBUCKETCONFIG1r:
        case MINBUCKETr:
        case MAXBUCKETCONFIGr:
        case MAXBUCKETCONFIG1r:
        case MAXBUCKETr:
        case MMU_TO_XPORT_BKPr:
        case DEQ_AGINGMASKr:
            pbmp = sc_non_cpu_ports;
            break;
        case MMU_LLFC_RX_CONFIGr:
        case XPORT_TO_MMU_BKPr:
            pbmp = sc_xports;
            break;
        case UNIMAC_PFC_CTRLr:
        case MAC_PFC_REFRESH_CTRLr:
        case MAC_PFC_TYPEr:
        case MAC_PFC_OPCODEr:
        case MAC_PFC_DA_0r:
        case MAC_PFC_DA_1r:
        case MACSEC_PROG_TX_CRCr:
        case MACSEC_CNTRLr:
        case TS_STATUS_CNTRLr:
        case TX_TS_DATAr:
            if (soc_feature(unit, soc_feature_priority_flow_control)) {
                pbmp = sc_non_cpu_ports;
            } else {
                pbmp = sc_no_ports;
            }
            break;
        case BMAC_PFC_CTRLr:
        case BMAC_PFC_TYPEr:
        case BMAC_PFC_OPCODEr:
        case BMAC_PFC_DA_LOr:
        case BMAC_PFC_DA_HIr:
        case BMAC_PFC_COS0_XOFF_CNTr:
        case BMAC_PFC_COS1_XOFF_CNTr:
        case BMAC_PFC_COS2_XOFF_CNTr:
        case BMAC_PFC_COS3_XOFF_CNTr:
        case BMAC_PFC_COS4_XOFF_CNTr:
        case BMAC_PFC_COS5_XOFF_CNTr:
        case BMAC_PFC_COS6_XOFF_CNTr:
        case BMAC_PFC_COS7_XOFF_CNTr:
        case BMAC_PFC_COS8_XOFF_CNTr:
        case BMAC_PFC_COS9_XOFF_CNTr:
        case BMAC_PFC_COS10_XOFF_CNTr:
        case BMAC_PFC_COS11_XOFF_CNTr:
        case BMAC_PFC_COS12_XOFF_CNTr:
        case BMAC_PFC_COS13_XOFF_CNTr:
        case BMAC_PFC_COS14_XOFF_CNTr:
        case BMAC_PFC_COS15_XOFF_CNTr:
        case PFC_COS0_XOFF_CNTr:
        case PFC_COS1_XOFF_CNTr:
        case PFC_COS2_XOFF_CNTr:
        case PFC_COS3_XOFF_CNTr:
        case PFC_COS4_XOFF_CNTr:
        case PFC_COS5_XOFF_CNTr:
        case PFC_COS6_XOFF_CNTr:
        case PFC_COS7_XOFF_CNTr:
        case PFC_COS8_XOFF_CNTr:
        case PFC_COS9_XOFF_CNTr:
        case PFC_COS10_XOFF_CNTr:
        case PFC_COS11_XOFF_CNTr:
        case PFC_COS12_XOFF_CNTr:
        case PFC_COS13_XOFF_CNTr:
        case PFC_COS14_XOFF_CNTr:
        case PFC_COS15_XOFF_CNTr:
            if (soc_feature(unit, soc_feature_priority_flow_control)) {
                pbmp = sc_xports;
            } else {
                pbmp = sc_no_ports;
            }
            break;
        default:
            pbmp = sc_all_ports;
            break;
    }

    switch(ainfo->reg) {
        /* EP_PERPORTPERCOS_REGS */
        case EGR_PERQ_XMT_COUNTERSr:
        /* MMU_PERPORTPERCOS_REGS */
        case OP_QUEUE_CONFIGr:
        case OP_QUEUE_RESET_OFFSETr:
        case OP_QUEUE_MIN_COUNTr:
        case OP_QUEUE_SHARED_COUNTr:
        case OP_QUEUE_TOTAL_COUNTr:
        case OP_QUEUE_RESET_VALUEr:
        case OP_QUEUE_LIMIT_YELLOWr:
        case OP_QUEUE_LIMIT_REDr:
        case HOLDROP_PKT_CNTr:
            if (ainfo->port == 0) {
                return 0;
            } else if (ainfo->idx < 10) { 
                pbmp = sc_all_ports;
            } else {
                goto skip;
            }
            break;
        default:
            break;
    }
    
    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip;
    }
    return 0;

skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_SCORPION_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
STATIC int
reg_mask_subset_tr2(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *ch_2pg_ports;
    pbmp_t *ch_8pg_ports;
    pbmp_t *ch_24q_ports;
    pbmp_t *ch_24q_ports_with_cpu;
    pbmp_t *ch_ext_ports;
    pbmp_t *ch_non_cpu_ports;
    pbmp_t *ch_all_ports;
    pbmp_t *ch_lb_ports;
    pbmp_t *ch_all_ports_with_mmu;
    pbmp_t *ch_gxports;
    pbmp_t *ch_cmic;
#define _REG_MASK_SUBSET_MAX_TR2 11
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_TR2];
    int i, count = _REG_MASK_SUBSET_MAX_TR2;
    uint64 temp_mask;
    pbmp_t *pbmp;

    mask_map_arr[0] = &ch_2pg_ports;
    mask_map_arr[1] = &ch_8pg_ports;
    mask_map_arr[2] = &ch_24q_ports;
    mask_map_arr[3] = &ch_24q_ports_with_cpu;
    mask_map_arr[4] = &ch_ext_ports;
    mask_map_arr[5] = &ch_non_cpu_ports;
    mask_map_arr[6] = &ch_all_ports;
    mask_map_arr[7] = &ch_lb_ports;
    mask_map_arr[8] = &ch_all_ports_with_mmu;
    mask_map_arr[9] = &ch_gxports;
    mask_map_arr[10] = &ch_cmic;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

        /* 8PG_PORTS = [26..30,34,38,42,46,50,54] */
        SOC_PBMP_CLEAR(*ch_8pg_ports);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 26);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 27);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 28);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 29);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 30); 
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 34);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 38);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 42);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 46);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 50);
        SOC_PBMP_PORT_ADD(*ch_8pg_ports, 54);

        /*  24Q_PORTS = [26..31,34,38,39,42,43,46,50,51,54] */
        SOC_PBMP_CLEAR(*ch_24q_ports);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 26);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 27);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 28);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 29);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 30);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 31);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 34);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 38);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 39);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 42);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 43);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 46);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 50);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 51);
        SOC_PBMP_PORT_ADD(*ch_24q_ports, 54);
        SOC_PBMP_ASSIGN(*ch_24q_ports_with_cpu, *ch_24q_ports);
        SOC_PBMP_PORT_ADD(*ch_24q_ports_with_cpu, 0);

        /* 2PG_PORTS = [0,1..25,31..33,35..37,39..41,43..45,47..49,51..53,55,56] */
        SOC_PBMP_CLEAR(*ch_2pg_ports);
        SOC_PBMP_ASSIGN(*ch_2pg_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 26);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 27);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 28);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 29);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 30); 
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 34);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 38);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 42);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 46);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 50);
        SOC_PBMP_PORT_REMOVE(*ch_2pg_ports, 54);
        SOC_PBMP_PORT_ADD(*ch_2pg_ports, 55);
        SOC_PBMP_PORT_ADD(*ch_2pg_ports, 56);

        /* GXPORTS = [26, 27, 28, 29] */
        SOC_PBMP_CLEAR(*ch_gxports);
        SOC_PBMP_PORT_ADD(*ch_gxports, 26);
        SOC_PBMP_PORT_ADD(*ch_gxports, 27);
        SOC_PBMP_PORT_ADD(*ch_gxports, 28);
        SOC_PBMP_PORT_ADD(*ch_gxports, 29);
       
        /* Loopback pbmp */
        SOC_PBMP_CLEAR(*ch_lb_ports);
        SOC_PBMP_ASSIGN(*ch_lb_ports, PBMP_LB(unit));

        /* All ports except CMIC */
        SOC_PBMP_CLEAR(*ch_non_cpu_ports);
        SOC_PBMP_ASSIGN(*ch_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*ch_non_cpu_ports, 0);

        /* All ports except CMIC and loopback - external ports */
        SOC_PBMP_CLEAR(*ch_ext_ports);
        SOC_PBMP_ASSIGN(*ch_ext_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*ch_ext_ports, 0);
        SOC_PBMP_PORT_REMOVE(*ch_ext_ports, 54);
 
        /* All ports (excluding the internal MMU ports) */
        SOC_PBMP_CLEAR(*ch_all_ports); 
        SOC_PBMP_ASSIGN(*ch_all_ports, PBMP_ALL(unit)); 

        /* All ports (including the internal MMU ports) */
        SOC_PBMP_CLEAR(*ch_all_ports_with_mmu); 
        SOC_PBMP_ASSIGN(*ch_all_ports_with_mmu, PBMP_ALL(unit)); 
        SOC_PBMP_PORT_ADD(*ch_all_ports_with_mmu, 55);
        SOC_PBMP_PORT_ADD(*ch_all_ports_with_mmu, 56);

        /* CPU port */
        SOC_PBMP_CLEAR(*ch_cmic); 
        SOC_PBMP_ASSIGN(*ch_cmic, PBMP_CMIC(unit));
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
 
    switch(ainfo->reg) {
        case MAC_CTRLr:
        case MAC_XGXS_CTRLr:
        case MAC_XGXS_STATr:
        case MAC_TXMUXCTRLr:
        case MAC_CNTMAXSZr:
        case MAC_CORESPARE0r:
        case MAC_TXCTRLr:
        case MAC_TXMACSAr:
        case MAC_TXMAXSZr:
        case MAC_TXPSETHRr:
        case MAC_TXSPARE0r:
        case MAC_TXPPPCTRLr:
        case ITPOKr:
        case ITXPFr: 
        case ITFCSr:
        case ITXPPr:
        case ITUCr:
        case ITMCAr:
        case ITBCAr:
        case ITOVRr:
        case ITFRGr:
        case ITPKTr:
        case IT64r:
        case IT127r:
        case IT255r:
        case IT511r:
        case IT1023r:
        case IT1518r:
        case IT2047r:
        case IT4095r:
        case IT9216r:
        case IT16383r:
        case ITMAXr:
        case ITUFLr:
        case ITERRr:
        case ITBYTr:
        case MAC_RXCTRLr:
        case MAC_RXMACSAr:
        case MAC_RXMAXSZr:
        case MAC_RXLSSCTRLr:
        case MAC_RXLSSSTATr:
        case MAC_RXSPARE0r:
        case IR64r:
        case IR127r:
        case IR255r:
        case IR511r:
        case IR1023r:
        case IR1518r:
        case IR2047r:
        case IR4095r:
        case IR9216r:
        case IR16383r:
        case IRMAXr:
        case IRPKTr:
        case IRFCSr:
        case IRUCr:
        case IRMCAr:
        case IRBCAr:
        case IRXCFr:
        case IRXPFr:
        case IRXPPr:
        case IRXUOr:
        case IRJBRr:
        case IROVRr:
        case IRFLRr:
        case IRPOKr:
        case IRMEGr:
        case IRMEBr:
        case IRBYTr:
        case IRUNDr:
        case IRFRGr:
        case IRERBYTr:
        case IRERPKTr:
        case IRJUNKr:
        case MAC_TXLLFCCTRLr:
        case MAC_TXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGCNTr:
            if (IS_GE_PORT(unit, ainfo->port)) {
                goto skip;
            }
            break;
        case EGR_PORT_REQUESTSr:
            if (IS_LB_PORT(unit, ainfo->port)) {
                goto skip;
            }
            break;
        case MAC_PFC_COS0_XOFF_CNTr:
        case MAC_PFC_COS1_XOFF_CNTr:
        case MAC_PFC_COS2_XOFF_CNTr:
        case MAC_PFC_COS3_XOFF_CNTr:
        case MAC_PFC_COS4_XOFF_CNTr:
        case MAC_PFC_COS5_XOFF_CNTr:
        case MAC_PFC_COS6_XOFF_CNTr:
        case MAC_PFC_COS7_XOFF_CNTr:
        case MAC_PFC_COS8_XOFF_CNTr:
        case MAC_PFC_COS9_XOFF_CNTr:
        case MAC_PFC_COS10_XOFF_CNTr:
        case MAC_PFC_COS11_XOFF_CNTr:
        case MAC_PFC_COS12_XOFF_CNTr:
        case MAC_PFC_COS13_XOFF_CNTr:
        case MAC_PFC_COS14_XOFF_CNTr:
        case MAC_PFC_COS15_XOFF_CNTr:
            /* Not implemented in HW */
            goto skip;
            break;
        default:
            break;
    }

    switch(ainfo->reg) {
        case PG_THRESH_SELr:
        case PORT_PRI_GRP0r:
        case PORT_PRI_GRP1r:
        case PORT_PRI_XON_ENABLEr:
            pbmp = ch_8pg_ports;
            break;
        case PG_THRESH_SEL2r:
        case PORT_PRI_GRP2r:
            pbmp = ch_2pg_ports;
            break;
        case ECN_CONFIGr:
        case HOL_STAT_PORTr:
        case PORT_WREDPARAM_CELLr:
        case PORT_WREDPARAM_YELLOW_CELLr:
        case PORT_WREDPARAM_RED_CELLr:
        case PORT_WREDPARAM_NONTCP_CELLr:
        case PORT_WREDCONFIG_CELLr:
        case PORT_WREDAVGQSIZE_CELLr:
        case PORT_WREDPARAM_PACKETr:
        case PORT_WREDPARAM_YELLOW_PACKETr:
        case PORT_WREDPARAM_RED_PACKETr:
        case PORT_WREDPARAM_NONTCP_PACKETr:
        case PORT_WREDCONFIG_PACKETr:
        case PORT_WREDAVGQSIZE_PACKETr:
        case BKPMETERINGCONFIG_64r:
        case BKPMETERINGBUCKETr:
        case MTRI_IFGr:
            pbmp = ch_ext_ports;
            break;
        case COSMASKr:
        case MINSPCONFIGr:
            pbmp = ch_non_cpu_ports;
            break;
        case S1V_CONFIGr:
        case S1V_COSWEIGHTSr:
        case S1V_COSMASKr:
        case S1V_MINSPCONFIGr:
        case S1V_WDRRCOUNTr:
            pbmp = ch_24q_ports;
            break;
        /* MMU_WLP_PERCOS_REGS */
        case OP_QUEUE_FIRST_FRAGMENT_CONFIG_CELLr:
        case OP_QUEUE_FIRST_FRAGMENT_RESET_OFFSET_CELLr:
        case OP_QUEUE_FIRST_FRAGMENT_CONFIG_PACKETr:
        case OP_QUEUE_FIRST_FRAGMENT_RESET_OFFSET_PACKETr:
        case OP_QUEUE_FIRST_FRAGMENT_COUNT_CELLr:
        case OP_QUEUE_FIRST_FRAGMENT_COUNT_PACKETr:
        case OP_QUEUE_REDIRECT_CONFIG_CELLr:
        case OP_QUEUE_REDIRECT_RESET_OFFSET_CELLr:
        case OP_QUEUE_REDIRECT_CONFIG_PACKETr:
        case OP_QUEUE_REDIRECT_RESET_OFFSET_PACKETr:
        case OP_QUEUE_REDIRECT_COUNT_CELLr:
        case OP_QUEUE_REDIRECT_COUNT_PACKETr:
        case OP_QUEUE_REDIRECT_XQ_CONFIG_PACKETr:
        case OP_QUEUE_REDIRECT_XQ_RESET_OFFSET_PACKETr:
        case OP_QUEUE_REDIRECT_XQ_COUNT_PACKETr:
        case OP_PORT_FIRST_FRAGMENT_DISC_RESUME_THD_CELLr:
        case OP_PORT_FIRST_FRAGMENT_DISC_RESUME_THD_PACKETr:
        case OP_PORT_FIRST_FRAGMENT_DISC_SET_THD_CELLr:
        case OP_PORT_FIRST_FRAGMENT_DISC_SET_THD_PACKETr:
        case OP_PORT_FIRST_FRAGMENT_COUNT_CELLr:
        case OP_PORT_FIRST_FRAGMENT_COUNT_PACKETr:
        case FIRST_FRAGMENT_DROP_STATE_CELLr:
        case FIRST_FRAGMENT_DROP_STATE_PACKETr:
        case OP_PORT_REDIRECT_COUNT_CELLr:
        case OP_PORT_REDIRECT_COUNT_PACKETr:
        case REDIRECT_DROP_STATE_CELLr:
        case REDIRECT_DROP_STATE_PACKETr:
        case REDIRECT_XQ_DROP_STATE_PACKETr: 
        case OP_PORT_REDIRECT_DISC_RESUME_THD_CELLr:
        case OP_PORT_REDIRECT_DISC_RESUME_THD_PACKETr:
        case OP_PORT_REDIRECT_DISC_SET_THD_CELLr:
        case OP_PORT_REDIRECT_DISC_SET_THD_PACKETr:
        case OP_PORT_REDIRECT_XQ_DISC_RESUME_THD_PACKETr:
        case OP_PORT_REDIRECT_XQ_DISC_SET_THD_PACKETr:
        case OP_PORT_REDIRECT_XQ_COUNT_PACKETr:
            pbmp = ch_lb_ports;
            break;
        case PORT_MIN_CELLr:
        case PORT_MIN_PACKETr:
        case PORT_SHARED_LIMIT_CELLr:
        case PORT_SHARED_LIMIT_PACKETr:
        case PORT_COUNT_CELLr:
        case PORT_COUNT_PACKETr:
        case PORT_MIN_COUNT_CELLr:
        case PORT_MIN_COUNT_PACKETr:
        case PORT_SHARED_COUNT_CELLr:
        case PORT_SHARED_COUNT_PACKETr:
            pbmp = ch_all_ports_with_mmu;
            break;
        default:
            pbmp = ch_all_ports;
            break;
    }

    switch(ainfo->reg) {
        /* MMU_PERPORTPERCOS_REGS */
        case OP_QUEUE_CONFIG_CELLr:
        case OP_QUEUE_CONFIG1_CELLr:
        case OP_QUEUE_CONFIG_PACKETr:
        case OP_QUEUE_CONFIG1_PACKETr:
        case OP_QUEUE_LIMIT_YELLOW_CELLr:
        case OP_QUEUE_LIMIT_YELLOW_PACKETr:
        case OP_QUEUE_LIMIT_RED_CELLr:
        case OP_QUEUE_LIMIT_RED_PACKETr:
        case OP_QUEUE_RESET_OFFSET_CELLr:
        case OP_QUEUE_RESET_OFFSET_PACKETr:
        case OP_QUEUE_MIN_COUNT_CELLr:
        case OP_QUEUE_MIN_COUNT_PACKETr:
        case OP_QUEUE_SHARED_COUNT_CELLr:
        case OP_QUEUE_SHARED_COUNT_PACKETr:
        case OP_QUEUE_TOTAL_COUNT_CELLr:
        case OP_QUEUE_TOTAL_COUNT_PACKETr:
        case OP_QUEUE_RESET_VALUE_CELLr:
        case OP_QUEUE_RESET_VALUE_PACKETr:
        case OP_QUEUE_LIMIT_RESUME_COLOR_CELLr:
        case OP_QUEUE_LIMIT_RESUME_COLOR_PACKETr:
        /* MMU_PERPORTPERCOS_REGS_CTR */
        case DROP_PKT_CNTr:
        case DROP_BYTE_CNTr:
            if (ainfo->port == 0) {
                return 0;
            } else if (ainfo->idx < 8) {
                pbmp = ch_all_ports;
            } else if ((ainfo->idx < 24) && (ainfo->port == 54)) {
                goto skip;
            } else if ((ainfo->idx < 24)) {
                pbmp = ch_24q_ports;
            } else {
                pbmp = ch_cmic;
            }
            break;

        /* MMU_PERPORTPERCOS_NOCPU_REGS */
        case WREDPARAM_CELLr:
        case WREDPARAM_YELLOW_CELLr:
        case WREDPARAM_RED_CELLr:
        case WREDPARAM_NONTCP_CELLr:
        case WREDCONFIG_CELLr:
        case WREDAVGQSIZE_CELLr:
        case WREDPARAM_PACKETr:
        case WREDPARAM_YELLOW_PACKETr:
        case WREDPARAM_RED_PACKETr:
        case WREDPARAM_NONTCP_PACKETr:
        case WREDCONFIG_PACKETr:
        case WREDAVGQSIZE_PACKETr:
            if (ainfo->port == 0) {
                goto skip;
            } else if (ainfo->idx < 8) {
                pbmp = ch_non_cpu_ports;
            } else {
                pbmp = ch_24q_ports;
            }
            break;

        /* MMU_MTRO_REGS */
        case MINBUCKETCONFIG_64r:
        case MINBUCKETr:
        case MAXBUCKETCONFIG_64r:
        case MAXBUCKETr:
            if (ainfo->idx < 8) {
                pbmp = ch_all_ports;
            } else if (ainfo->idx < 26) {
                pbmp = ch_24q_ports_with_cpu;
            } else {
                pbmp = ch_cmic;
            }
            break;

        /* MMU_ES_REGS */
        case COSWEIGHTSr:
        case WDRRCOUNTr:
            if (ainfo->idx < 8) {
                pbmp = ch_all_ports;
            } else if (ainfo->idx < 10) {
                pbmp = ch_24q_ports_with_cpu;
            } else {
                pbmp = ch_cmic;
            }
            break;

        /* MMU_PERPORTPERPRI_REGS */
        case PG_RESET_OFFSET_CELLr:
        case PG_RESET_OFFSET_PACKETr:
        case PG_RESET_FLOOR_CELLr:
        
        case PG_MIN_CELLr:
        case PG_MIN_PACKETr:
        case PG_HDRM_LIMIT_CELLr:
        case PG_HDRM_LIMIT_PACKETr:
        case PG_COUNT_CELLr:
        case PG_COUNT_PACKETr:
        case PG_MIN_COUNT_CELLr:
        case PG_MIN_COUNT_PACKETr:
        case PG_PORT_MIN_COUNT_CELLr:
        case PG_PORT_MIN_COUNT_PACKETr:
        case PG_SHARED_COUNT_CELLr:
        case PG_SHARED_COUNT_PACKETr:
        case PG_HDRM_COUNT_CELLr:
        case PG_HDRM_COUNT_PACKETr:
        case PG_GBL_HDRM_COUNTr:
        case PG_RESET_VALUE_CELLr:
        case PG_RESET_VALUE_PACKETr:
            if (ainfo->idx < 2) {
                pbmp = ch_all_ports_with_mmu;
            } else {
                pbmp = ch_8pg_ports;
            }
            break;
 
        default:
            break; 
    }

    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip; 
    }

    if (mask != NULL) {
        switch(ainfo->reg) {
            case HOL_STAT_PORTr:
            case ECN_CONFIGr:
                if (!SOC_PBMP_MEMBER(*ch_24q_ports, ainfo->port)) {
                    /* adjust mask for ports without 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
            case TOQ_QUEUESTAT_64r:
            case TOQ_ACTIVATEQ_64r:
            case TOQEMPTY_64r:
            case DEQ_AGINGMASK_64r:
            case SHAPING_MODEr:
            case EAV_MAXBUCKET_64r:
            case EAV_MINBUCKET_64r:
                if (SOC_PBMP_MEMBER(*ch_24q_ports, ainfo->port)) {
                    /* adjust mask for ports with 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x00ffffff);
                    COMPILER_64_AND(*mask, temp_mask);
                } else if (ainfo->port != 0) {
                    /* remaining ports only have 8 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
        }
    }
    return 0; 
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_ENDURO_SUPPORT
STATIC int
reg_mask_subset_en(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *en_8pg_ports;
    pbmp_t *en_higig_ports;
    pbmp_t *en_24q_ports;
    pbmp_t *en_24q_ports_with_cpu;
    pbmp_t *en_non_cpu_ports;
    pbmp_t *en_all_ports;
    pbmp_t *en_cmic;
#define _REG_MASK_SUBSET_MAX_EN 7
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_EN];
    int i, count = _REG_MASK_SUBSET_MAX_EN;
    uint64 temp_mask;
    pbmp_t *pbmp;

    mask_map_arr[0] = &en_8pg_ports;
    mask_map_arr[1] = &en_higig_ports;
    mask_map_arr[2] = &en_24q_ports;
    mask_map_arr[3] = &en_24q_ports_with_cpu;
    mask_map_arr[4] = &en_non_cpu_ports;
    mask_map_arr[5] = &en_all_ports;
    mask_map_arr[6] = &en_cmic;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

        /* Higig ports [0,26,27,28,29] */
        SOC_PBMP_CLEAR(*en_higig_ports);
        SOC_PBMP_PORT_ADD(*en_higig_ports, 0);
        SOC_PBMP_PORT_ADD(*en_higig_ports, 26);
        SOC_PBMP_PORT_ADD(*en_higig_ports, 27);
        SOC_PBMP_PORT_ADD(*en_higig_ports, 28);
        SOC_PBMP_PORT_ADD(*en_higig_ports, 29);

        /*  24Q_PORTS = [26,27,28,29] */
        SOC_PBMP_CLEAR(*en_24q_ports);
        SOC_PBMP_PORT_ADD(*en_24q_ports, 26);
        SOC_PBMP_PORT_ADD(*en_24q_ports, 27);
        SOC_PBMP_PORT_ADD(*en_24q_ports, 28);
        SOC_PBMP_PORT_ADD(*en_24q_ports, 29);
        SOC_PBMP_ASSIGN(*en_24q_ports_with_cpu, *en_24q_ports);
        SOC_PBMP_PORT_ADD(*en_24q_ports_with_cpu, 0);

        SOC_PBMP_CLEAR(*en_8pg_ports);
        SOC_PBMP_PORT_ADD(*en_8pg_ports, 26);
        SOC_PBMP_PORT_ADD(*en_8pg_ports, 27);
        SOC_PBMP_PORT_ADD(*en_8pg_ports, 28);
        SOC_PBMP_PORT_ADD(*en_8pg_ports, 29);

        /* all port except CMIC */
        SOC_PBMP_CLEAR(*en_non_cpu_ports);
        SOC_PBMP_ASSIGN(*en_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*en_non_cpu_ports, 0);
 
        /* All ports */
        SOC_PBMP_CLEAR(*en_all_ports); 
        SOC_PBMP_ASSIGN(*en_all_ports, PBMP_ALL(unit)); 
 
        /* CPU port */
        SOC_PBMP_CLEAR(*en_cmic); 
        SOC_PBMP_ASSIGN(*en_cmic, PBMP_CMIC(unit));
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
 
    switch(ainfo->reg) {
        case MAC_CTRLr:
        case MAC_XGXS_CTRLr:
        case MAC_XGXS_STATr:
        case MAC_TXMUXCTRLr:
        case MAC_CNTMAXSZr:
        case MAC_CORESPARE0r:
        case MAC_TXCTRLr:
        case MAC_TXMACSAr:
        case MAC_TXMAXSZr:
        case MAC_TXPSETHRr:
        case MAC_TXSPARE0r:
        case MAC_TXPPPCTRLr:
        case ITPOKr:
        case ITXPFr: 
        case ITFCSr:
        case ITUCr:
        case ITUCAr:
        case ITMCAr:
        case ITBCAr:
        case ITOVRr:
        case ITFRGr:
        case ITPKTr:
        case IT64r:
        case IT127r:
        case IT255r:
        case IT511r:
        case IT1023r:
        case IT1518r:
        case IT2047r:
        case IT4095r:
        case IT9216r:
        case IT16383r:
        case ITMAXr:
        case ITUFLr:
        case ITERRr:
        case ITBYTr:
        case MAC_RXCTRLr:
        case MAC_RXMACSAr:
        case MAC_RXMAXSZr:
        case MAC_RXLSSCTRLr:
        case MAC_RXLSSSTATr:
        case MAC_RXSPARE0r:
        case IR64r:
        case IR127r:
        case IR255r:
        case IR511r:
        case IR1023r:
        case IR1518r:
        case IR2047r:
        case IR4095r:
        case IR9216r:
        case IR16383r:
        case IRMAXr:
        case IRPKTr:
        case IRFCSr:
        case IRUCAr:
        case IRMCAr:
        case IRBCAr:
        case IRXCFr:
        case IRXPFr:
        case IRXUOr:
        case IRJBRr:
        case IROVRr:
        case IRFLRr:
        case IRPOKr:
        case IRMEGr:
        case IRMEBr:
        case IRBYTr:
        case IRUNDr:
        case IRFRGr:
        case IRERBYTr:
        case IRERPKTr:
        case IRJUNKr:
        case MAC_TXLLFCCTRLr:
        case MAC_TXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGCNTr:
            if (IS_GE_PORT(unit, ainfo->port)) {
                goto skip;
            }
            break;
        default:
            break;
    }

    switch(ainfo->reg) {
        case IE2E_CONTROLr:
        case ING_MODMAP_CTRLr:
        case IHG_LOOKUPr:
        case ICONTROL_OPCODE_BITMAPr:
        case UNKNOWN_HGI_BITMAPr:
        case IUNHGIr:
        case ICTRLr:
        case IBCASTr:
        case ILTOMCr:
        case IIPMCr:
        case IUNKOPCr:
            pbmp = en_higig_ports;
            break;
        case PG_THRESH_SELr:
        case PORT_PRI_GRP0r:
        case PORT_PRI_GRP1r:
        case PORT_PRI_XON_ENABLEr:
            pbmp = en_8pg_ports;
            break;
        case ECN_CONFIGr:
        case HOL_STAT_PORTr:
        case PORT_WREDPARAM_CELLr:
        case PORT_WREDPARAM_YELLOW_CELLr:
        case PORT_WREDPARAM_RED_CELLr:
        case PORT_WREDPARAM_NONTCP_CELLr:
        case PORT_WREDCONFIG_CELLr:
        case PORT_WREDAVGQSIZE_CELLr:
        case PORT_WREDPARAM_PACKETr:
        case PORT_WREDPARAM_YELLOW_PACKETr:
        case PORT_WREDPARAM_RED_PACKETr:
        case PORT_WREDPARAM_NONTCP_PACKETr:
        case PORT_WREDCONFIG_PACKETr:
        case PORT_WREDAVGQSIZE_PACKETr:
        case COSMASKr:
        case MINSPCONFIGr:
        case EGRMETERINGCONFIG_64r:
        case EGRMETERINGBUCKETr:
            pbmp = en_non_cpu_ports;
            break;
        case ING_COS_MODEr:
        case COS_MODEr:
        case S1V_CONFIGr:
        case S1V_COSWEIGHTSr:
        case S1V_COSMASKr:
        case S1V_MINSPCONFIGr:
        case S1V_WDRRCOUNTr:
            pbmp = en_24q_ports;
            break;
        default:
            pbmp = en_all_ports;
            break;
    }

    switch(ainfo->reg) {
        /* MMU_PERPORTPERCOS_REGS */
        case OP_QUEUE_CONFIG_CELLr:
        case OP_QUEUE_CONFIG1_CELLr:
        case OP_QUEUE_CONFIG_PACKETr:
        case OP_QUEUE_CONFIG1_PACKETr:
        case OP_QUEUE_LIMIT_YELLOW_CELLr:
        case OP_QUEUE_LIMIT_YELLOW_PACKETr:
        case OP_QUEUE_LIMIT_RED_CELLr:
        case OP_QUEUE_LIMIT_RED_PACKETr:
        case OP_QUEUE_RESET_OFFSET_CELLr:
        case OP_QUEUE_RESET_OFFSET_PACKETr:
        case OP_QUEUE_MIN_COUNT_CELLr:
        case OP_QUEUE_MIN_COUNT_PACKETr:
        case OP_QUEUE_SHARED_COUNT_CELLr:
        case OP_QUEUE_SHARED_COUNT_PACKETr:
        case OP_QUEUE_TOTAL_COUNT_CELLr:
        case OP_QUEUE_TOTAL_COUNT_PACKETr:
        case OP_QUEUE_RESET_VALUE_CELLr:
        case OP_QUEUE_RESET_VALUE_PACKETr:
        case OP_QUEUE_LIMIT_RESUME_COLOR_CELLr:
        case OP_QUEUE_LIMIT_RESUME_COLOR_PACKETr:
        /* MMU_PERPORTPERCOS_REGS_CTR */
        case DROP_PKT_CNTr:
        case DROP_BYTE_CNTr:
            if (ainfo->port == 0) {
                return 0;
            } else if (ainfo->idx < 8) {
                pbmp = en_all_ports;
            } else if (ainfo->idx < 24) {
                pbmp = en_24q_ports;
            } else {
                pbmp = en_cmic;
            }
            break;
        /* MMU_PERPORTPERCOS_NOCPU_REGS */
        case WREDPARAM_CELLr:
        case WREDPARAM_YELLOW_CELLr:
        case WREDPARAM_RED_CELLr:
        case WREDPARAM_NONTCP_CELLr:
        case WREDCONFIG_CELLr:
        case WREDAVGQSIZE_CELLr:
        case WREDPARAM_PACKETr:
        case WREDPARAM_YELLOW_PACKETr:
        case WREDPARAM_RED_PACKETr:
        case WREDPARAM_NONTCP_PACKETr:
        case WREDCONFIG_PACKETr:
        case WREDAVGQSIZE_PACKETr:
            if (ainfo->port == 0) {
                goto skip;
            } else if (ainfo->idx < 8) {
                pbmp = en_non_cpu_ports;
            } else {
                pbmp = en_24q_ports;
            }
            break;


        /* MMU_MTRO_REGS */
        case MINBUCKETCONFIG_64r:
        case MINBUCKETr:
        case MAXBUCKETCONFIG_64r:
        case MAXBUCKETr:
            if (ainfo->port == 0) {
                goto skip;
            } else if (ainfo->idx < 8) {
                pbmp = en_all_ports;
            } else if (ainfo->idx < 26) {
                pbmp = en_24q_ports_with_cpu;
            } else {
                pbmp = en_cmic;
            }
            break;

        /* MMU_ES_REGS */
        case COSWEIGHTSr:
        case WDRRCOUNTr:
            if (ainfo->idx < 8) {
                pbmp = en_all_ports;
            } else if (ainfo->idx < 10) {
                pbmp = en_24q_ports_with_cpu;
            } else {
                pbmp = en_cmic;
            }
            break;
        /* MMU_PERPORTPERPRI_REGS */
        case PG_RESET_OFFSET_CELLr:
        case PG_RESET_OFFSET_PACKETr:
        case PG_RESET_FLOOR_CELLr:
        
        case PG_MIN_CELLr:
        case PG_MIN_PACKETr:
        case PG_HDRM_LIMIT_CELLr:
        case PG_HDRM_LIMIT_PACKETr:
        case PG_COUNT_CELLr:
        case PG_COUNT_PACKETr:
        case PG_MIN_COUNT_CELLr:
        case PG_MIN_COUNT_PACKETr:
        case PG_PORT_MIN_COUNT_CELLr:
        case PG_PORT_MIN_COUNT_PACKETr:
        case PG_SHARED_COUNT_CELLr:
        case PG_SHARED_COUNT_PACKETr:
        case PG_HDRM_COUNT_CELLr:
        case PG_HDRM_COUNT_PACKETr:
        case PG_GBL_HDRM_COUNTr:
        case PG_RESET_VALUE_CELLr:
        case PG_RESET_VALUE_PACKETr:
            if (ainfo->idx != 0) {
                pbmp = en_8pg_ports;
            }
            break;
        default:
            break; 
    }

    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip; 
    }

    if (mask != NULL) {
        switch(ainfo->reg) {
            case HOL_STAT_PORTr:
            case ECN_CONFIGr:
                if (!SOC_PBMP_MEMBER(*en_24q_ports, ainfo->port)) {
                    /* adjust mask for ports without 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
            case TOQ_QUEUESTAT_64r:
            case TOQ_ACTIVATEQ_64r:
            case TOQEMPTY_64r:
            case DEQ_AGINGMASK_64r:
                if (SOC_PBMP_MEMBER(*en_24q_ports, ainfo->port)) {
                    /* adjust mask for ports with 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x00ffffff);
                    COMPILER_64_AND(*mask, temp_mask);
                } else if (ainfo->port != 0) {
                    /* remaining ports only have 8 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
        }
    }
    return 0; 
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_HURRICANE1_SUPPORT
STATIC int
reg_mask_subset_hu(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *hu_8pg_ports;
    pbmp_t *hu_24q_ports;
    pbmp_t *hu_24q_ports_with_cpu;
    pbmp_t *hu_higig_ports;
    pbmp_t *hu_non_cpu_ports;
    pbmp_t *hu_all_ports;
#define _REG_MASK_SUBSET_MAX_HU 6
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_HU];
    int i, count = _REG_MASK_SUBSET_MAX_HU;
    uint64 temp_mask;
    pbmp_t *pbmp;

    mask_map_arr[0] = &hu_8pg_ports;
    mask_map_arr[1] = &hu_24q_ports;
    mask_map_arr[2] = &hu_24q_ports_with_cpu;
    mask_map_arr[3] = &hu_higig_ports;
    mask_map_arr[4] = &hu_non_cpu_ports;
    mask_map_arr[5] = &hu_all_ports;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

        /* Higig ports [0,26,27,28,29] */
        SOC_PBMP_CLEAR(*hu_higig_ports);
        SOC_PBMP_PORT_ADD(*hu_higig_ports, 0);
        SOC_PBMP_PORT_ADD(*hu_higig_ports, 26);
        SOC_PBMP_PORT_ADD(*hu_higig_ports, 27);
        SOC_PBMP_PORT_ADD(*hu_higig_ports, 28);
        SOC_PBMP_PORT_ADD(*hu_higig_ports, 29);

        /*  24Q_PORTS = NONE */
        SOC_PBMP_CLEAR(*hu_24q_ports);
        SOC_PBMP_ASSIGN(*hu_24q_ports_with_cpu, *hu_24q_ports);
        SOC_PBMP_PORT_ADD(*hu_24q_ports_with_cpu, 0);

        SOC_PBMP_CLEAR(*hu_8pg_ports);
        SOC_PBMP_PORT_ADD(*hu_8pg_ports, 26);
        SOC_PBMP_PORT_ADD(*hu_8pg_ports, 27);
        SOC_PBMP_PORT_ADD(*hu_8pg_ports, 28);
        SOC_PBMP_PORT_ADD(*hu_8pg_ports, 29);

        /* all port except CMIC */
        SOC_PBMP_CLEAR(*hu_non_cpu_ports);
        SOC_PBMP_ASSIGN(*hu_non_cpu_ports, PBMP_ALL(unit));
        SOC_PBMP_PORT_REMOVE(*hu_non_cpu_ports, 0);
 
        /* All ports */
        SOC_PBMP_CLEAR(*hu_all_ports); 
        SOC_PBMP_ASSIGN(*hu_all_ports, PBMP_ALL(unit)); 
    }
    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    if (!SOC_REG_IS_VALID(unit, ainfo->reg)) {
        goto skip;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);
 
    switch(ainfo->reg) {
        case MAC_CTRLr:
        case MAC_XGXS_CTRLr:
        case MAC_XGXS_STATr:
        case MAC_TXMUXCTRLr:
        case MAC_CNTMAXSZr:
        case MAC_CORESPARE0r:
        case MAC_TXCTRLr:
        case MAC_TXMACSAr:
        case MAC_TXMAXSZr:
        case MAC_TXPSETHRr:
        case MAC_TXSPARE0r:
        case MAC_TXPPPCTRLr:
        case ITPOKr:
        case ITXPFr: 
        case ITFCSr:
        case ITUCr:
        case ITUCAr:
        case ITMCAr:
        case ITBCAr:
        case ITOVRr:
        case ITFRGr:
        case ITPKTr:
        case IT64r:
        case IT127r:
        case IT255r:
        case IT511r:
        case IT1023r:
        case IT1518r:
        case IT2047r:
        case IT4095r:
        case IT9216r:
        case IT16383r:
        case ITMAXr:
        case ITUFLr:
        case ITERRr:
        case ITBYTr:
        case MAC_RXCTRLr:
        case MAC_RXMACSAr:
        case MAC_RXMAXSZr:
        case MAC_RXLSSCTRLr:
        case MAC_RXLSSSTATr:
        case MAC_RXSPARE0r:
        case IR64r:
        case IR127r:
        case IR255r:
        case IR511r:
        case IR1023r:
        case IR1518r:
        case IR2047r:
        case IR4095r:
        case IR9216r:
        case IR16383r:
        case IRMAXr:
        case IRPKTr:
        case IRFCSr:
        case IRUCAr:
        case IRMCAr:
        case IRBCAr:
        case IRXCFr:
        case IRXPFr:
        case IRXUOr:
        case IRJBRr:
        case IROVRr:
        case IRFLRr:
        case IRPOKr:
        case IRMEGr:
        case IRMEBr:
        case IRBYTr:
        case IRUNDr:
        case IRFRGr:
        case IRERBYTr:
        case IRERPKTr:
        case IRJUNKr:
        case ITXPPr:
        case IRXPPr:
        case MAC_TXLLFCCTRLr:
        case MAC_TXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGFLDSr:
        case MAC_RXLLFCMSGCNTr:
            if (IS_GE_PORT(unit, ainfo->port)) {
                goto skip;
            }
            break;
        default:
            break;
    }

    switch(ainfo->reg) {
        case IE2E_CONTROLr:
        case ING_MODMAP_CTRLr:
        case IHG_LOOKUPr:
        case ICONTROL_OPCODE_BITMAPr:
        case UNKNOWN_HGI_BITMAPr:
        case IUNHGIr:
        case ICTRLr:
        case IBCASTr:
        case ILTOMCr:
        case IIPMCr:
        case IUNKOPCr:
            pbmp = hu_higig_ports;
            break;
        case HOL_STAT_PORTr:
            pbmp = hu_non_cpu_ports;
            break;
        case ING_COS_MODEr:
            pbmp = hu_24q_ports;
            break;
        default:
            pbmp = hu_all_ports;
            break;
    }

    if (!SOC_PBMP_MEMBER(*pbmp, ainfo->port)) {
        goto skip; 
    }


    if (mask != NULL) {
        switch(ainfo->reg) {
            case HOL_STAT_PORTr:
                if (!SOC_PBMP_MEMBER(*hu_24q_ports, ainfo->port)) {
                    /* adjust mask for ports without 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
            case TOQ_QUEUESTAT_64r:
            case TOQ_ACTIVATEQ_64r:
            case TOQEMPTY_64r:
            case DEQ_AGINGMASK_64r:
                if (SOC_PBMP_MEMBER(*hu_24q_ports, ainfo->port)) {
                    /* adjust mask for ports with 24 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x00ffffff);
                    COMPILER_64_AND(*mask, temp_mask);
                } else if (ainfo->port != 0) {
                    /* remaining ports only have 8 queues */
                    COMPILER_64_SET(temp_mask, 0, 0x000000ff);
                    COMPILER_64_AND(*mask, temp_mask);
                }
                break;
            case MAC_MODEr:
                /* skip LINK_STATUS bit */
                COMPILER_64_SET(temp_mask, 0, 0x0000000f);
                COMPILER_64_AND(*mask, temp_mask);
                break;
        }
    }
    return 0; 
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_HURRICANE_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
reg_mask_subset_trident(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    soc_info_t *si;
    uint64 temp_mask;

    switch (ainfo->reg) {
    case PQEPARITYERRORADRr:
    case PQ_C0_PORT_A_CMDr:
    case PQ_C0_PORT_A_DATA_0r:
    case PQ_C0_PORT_A_DATA_1r:
    case PQ_C0_PORT_A_DATA_2r:
    case PQ_C0_PORT_A_DATA_3r:
    case PQ_C0_PORT_A_RADDRr:
    case PQ_C0_PORT_A_WADDRr:
    case PQ_C0_PORT_B_CMDr:
    case PQ_C0_PORT_B_DATA_0r:
    case PQ_C0_PORT_B_DATA_1r:
    case PQ_C0_PORT_B_DATA_2r:
    case PQ_C0_PORT_B_DATA_3r:
    case PQ_C0_PORT_B_RADDRr:
    case PQ_C0_PORT_B_WADDRr:
    case PQ_C1_PORT_A_CMDr:
    case PQ_C1_PORT_A_DATA_0r:
    case PQ_C1_PORT_A_DATA_1r:
    case PQ_C1_PORT_A_DATA_2r:
    case PQ_C1_PORT_A_DATA_3r:
    case PQ_C1_PORT_A_RADDRr:
    case PQ_C1_PORT_A_WADDRr:
    case PQ_C1_PORT_B_CMDr:
    case PQ_C1_PORT_B_DATA_0r:
    case PQ_C1_PORT_B_DATA_1r:
    case PQ_C1_PORT_B_DATA_2r:
    case PQ_C1_PORT_B_DATA_3r:
    case PQ_C1_PORT_B_RADDRr:
    case PQ_C1_PORT_B_WADDRr:
    case PQ_C2_PORT_A_CMDr:
    case PQ_C2_PORT_A_DATA_0r:
    case PQ_C2_PORT_A_DATA_1r:
    case PQ_C2_PORT_A_DATA_2r:
    case PQ_C2_PORT_A_DATA_3r:
    case PQ_C2_PORT_A_RADDRr:
    case PQ_C2_PORT_A_WADDRr:
    case PQ_C2_PORT_B_CMDr:
    case PQ_C2_PORT_B_DATA_0r:
    case PQ_C2_PORT_B_DATA_1r:
    case PQ_C2_PORT_B_DATA_2r:
    case PQ_C2_PORT_B_DATA_3r:
    case PQ_C2_PORT_B_RADDRr:
    case PQ_C2_PORT_B_WADDRr:
    case PQ_C3_PORT_A_CMDr:
    case PQ_C3_PORT_A_DATA_0r:
    case PQ_C3_PORT_A_DATA_1r:
    case PQ_C3_PORT_A_DATA_2r:
    case PQ_C3_PORT_A_DATA_3r:
    case PQ_C3_PORT_A_RADDRr:
    case PQ_C3_PORT_A_WADDRr:
    case PQ_C3_PORT_B_CMDr:
    case PQ_C3_PORT_B_DATA_0r:
    case PQ_C3_PORT_B_DATA_1r:
    case PQ_C3_PORT_B_DATA_2r:
    case PQ_C3_PORT_B_DATA_3r:
    case PQ_C3_PORT_B_RADDRr:
    case PQ_C3_PORT_B_WADDRr:
    case PQ_MODULE_CONTROLr:
    case PQ_PROGRAM_GOr:
        goto skip;
        break;
    default:
        break;
    }

    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }

    si = &SOC_INFO(unit);

    switch (ainfo->reg) {
    case XMAC_CTRLr:
    case XMAC_MODEr:
    case XMAC_SPARE0r:
    case XMAC_SPARE1r:
    case XMAC_TX_CTRLr:
    case XMAC_TX_MAC_SAr:
    case XMAC_RX_CTRLr:
    case XMAC_RX_MAC_SAr:
    case XMAC_RX_MAX_SIZEr:
    case XMAC_RX_VLAN_TAGr:
    case XMAC_RX_LSS_CTRLr:
    case XMAC_RX_LSS_STATUSr:
    case XMAC_CLEAR_RX_LSS_STATUSr:
    case XMAC_PAUSE_CTRLr:
    case XMAC_PFC_CTRLr:
    case XMAC_PFC_TYPEr:
    case XMAC_PFC_OPCODEr:
    case XMAC_PFC_DAr:
    case XMAC_LLFC_CTRLr:
    case XMAC_TX_LLFC_MSG_FIELDSr:
    case XMAC_RX_LLFC_MSG_FIELDSr:
    case XMAC_HCFC_CTRLr:
    case XMAC_TX_TIMESTAMP_FIFO_DATAr:
    case XMAC_TX_TIMESTAMP_FIFO_STATUSr:
    case XMAC_FIFO_STATUSr:
    case XMAC_CLEAR_FIFO_STATUSr:
    case XMAC_TX_FIFO_CREDITSr:
    case XMAC_EEE_CTRLr:
    case XMAC_EEE_TIMERSr:
        /* Skip Xmac registers when the XLPORT is used as GE port */
        if (IS_GE_PORT(unit, ainfo->port)) {
            goto skip;
        }
        break;


    case DROP_PKT_CNT_OVQr:
        /* @OVQ_PORT_DROP_REGS (all ports except cpu port) */
    case DEQ_AGINGMASKr:
    case MCQ_FIFO_BASE_REGr:
    case MCQ_FIFO_EMPTY_REGr:
        /* PORTLIST => [1..65] */
    case COSMASKr:
    case MINSPCONFIGr:
    case S3_CONFIGr:
    case S3_CONFIG_MCr:
    case S3_COSMASKr:
    case S3_COSMASK_MCr:
    case S3_MINSPCONFIGr:
    case S3_MINSPCONFIG_MCr:
    case S2_CONFIGr:
    case S2_COSWEIGHTSr:
    case S2_COSMASKr:
    case S2_MINSPCONFIGr:
    case S2_WERRCOUNTr:
    case S2_S3_ROUTINGr:
        /* @MMU_NO_CPU_PORTS (all ports except cpu port) */
    case PORT_LLFC_CFGr:
        /* @PFC_PORT_CFG_REGS (all ports except cpu port) */
    case S3_MINBUCKETCONFIG_64r:
    case S3_MINBUCKETr:
    case S3_MAXBUCKETCONFIG_64r:
    case S3_MAXBUCKETr:
        /* @MMU_MTRO_S3_REG (all ports except cpu port) */
    case S2_MINBUCKETCONFIG_64r:
    case S2_MINBUCKETr:
    case S2_MAXBUCKETCONFIG_64r:
    case S2_MAXBUCKETr:
        /* @MMU_MTRO_S2_REG (all ports except cpu port) */
    case OVQ_MCQ_CREDITSr:
    case OVQ_MCQ_STATEr:
        /* @OVQ_PORT_COS_REGS (all ports except cpu port) */
        if (IS_CPU_PORT(unit, ainfo->port)) {
            goto skip;
        }
        break;

    case XPORT_TO_MMU_BKPr:
        /* @PFC_PORT_STS_REGS (all ports except cpu and lb port) */
    case BKPMETERINGCONFIG_64r:
    case BKPMETERINGBUCKETr:
    case MTRI_IFGr:
        /* @MMU_MTRI_PORTS (all ports except cpu and lb port) */
    case OP_UC_PORT_CONFIG_CELLr:
    case OP_UC_PORT_CONFIG1_CELLr:
    case OP_UC_PORT_LIMIT_COLOR_CELLr:
    case OP_UC_PORT_SHARED_COUNT_CELLr:
    case OP_UC_PORT_LIMIT_RESUME_COLOR_CELLr:
    case UCQ_COS_EMPTY_REGr:
    case PORT_SP_WRED_CONFIGr:
    case PORT_SP_WRED_AVG_QSIZEr:
        /* PORTLIST => [1..32,34..65] */
    case OP_UC_QUEUE_MIN_COUNT_CELLr:
    case OP_UC_QUEUE_SHARED_COUNT_CELLr:
    case OP_UC_QUEUE_TOTAL_COUNT_CELLr:
    case OP_UC_QUEUE_RESET_VALUE_CELLr:
        /* @THDO_UC_PORT_COS_REGS (all ports except cpu and lb port) */
    case WRED_CONFIGr:
    case WRED_AVG_QSIZEr:
        /* @MMU_PERPORTPERCOS_NOCPU_REGS (all ports except cpu and lb port) */
        if (IS_CPU_PORT(unit, ainfo->port) || IS_LB_PORT(unit, ainfo->port)) {
            goto skip;
        }
        break;

    case OP_EX_QUEUE_MIN_COUNT_CELLr:
    case OP_EX_QUEUE_SHARED_COUNT_CELLr:
    case OP_EX_QUEUE_TOTAL_COUNT_CELLr:
    case OP_EX_QUEUE_RESET_VALUE_CELLr:
        /* @THDO_EX_PORT_COS_REGS (extended queue ports) */
    case OP_EX_PORT_CONFIG_SPID_0r:
    case OP_EX_PORT_CONFIG_SPID_1r:
    case OP_EX_PORT_CONFIG_SPID_2r:
    case OP_EX_PORT_CONFIG_SPID_3r:
    case OP_EX_PORT_CONFIG_SPID_4r:
    case OP_EX_PORT_CONFIG_COS_MIN_0r:
    case OP_EX_PORT_CONFIG_COS_MIN_1r:
    case OP_EX_PORT_CONFIG_COS_MIN_2r:
    case UCQ_EXTCOS1_EMPTY_REGr:
        /* PORTLIST => [1..4,34..37] */
    case DMVOQ_WRED_CONFIGr:
    case VOQ_WRED_AVG_QSIZEr:
        /* @MMU_PERPORTPERVOQ_NOCPU_REGS (extended queue ports) */
        if (!si->port_num_ext_cosq[ainfo->port]) {
            goto skip;
        }
        break;

    case COSWEIGHTSr:
    case WERRCOUNTr:
        /* @MMU_ES_REGS */
        if (!IS_CPU_PORT(unit, ainfo->port)) {
            if (ainfo->idx >= 7) {
                goto skip;
            }
        }
        break;

    case S3_COSWEIGHTSr:
    case S3_WERRCOUNTr:
        /* @MMU_ES_S3_REGS */
        if (!si->port_num_ext_cosq[ainfo->port]) {
            if (IS_CPU_PORT(unit, ainfo->port) || ainfo->idx >= 8) {
                goto skip;
            }
        }
        break;

    case MINBUCKETCONFIG_64r:
    case MINBUCKETr:
    case MAXBUCKETCONFIG_64r:
    case MAXBUCKETr:
        /* @MMU_MTRO_REGS */
        if (IS_CPU_PORT(unit, ainfo->port)) {
            if (ainfo->idx >= 48) {
                goto skip;
            }
        } else if (IS_LB_PORT(unit, ainfo->port)) {
            if (ainfo->idx >= 5) {
                goto skip;
            }
        } else if (!si->port_num_ext_cosq[ainfo->port]) {
            if (ainfo->idx >= 15) {
                goto skip;
            }
        }
        break;

    case OP_QUEUE_CONFIG_CELLr:
    case OP_QUEUE_CONFIG1_CELLr:
    case OP_QUEUE_LIMIT_COLOR_CELLr:
    case OP_QUEUE_RESET_OFFSET_CELLr:
    case OP_QUEUE_MIN_COUNT_CELLr:
    case OP_QUEUE_SHARED_COUNT_CELLr:
    case OP_QUEUE_TOTAL_COUNT_CELLr:
    case OP_QUEUE_RESET_VALUE_CELLr:
    case OP_QUEUE_LIMIT_RESUME_COLOR_CELLr:
        /* @THDO_PORT_COS_REGS */
        if (!IS_CPU_PORT(unit, ainfo->port)) {
            if (ainfo->idx >= 5) {
                goto skip;
            }
        }
        break;
    default:
        break;
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case SHAPING_CONTROLr:
        case RESET_ON_EMPTY_MAX_64r:
            if (!si->port_num_ext_cosq[ainfo->port]) {
                COMPILER_64_SET(temp_mask, 0, 0x007fffff);
                COMPILER_64_AND(*mask, temp_mask);
            }
            break;
        default:
            break;
        }
    }

    return 0;
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
STATIC int
reg_mask_subset_trident2(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    soc_info_t *si;
    int port, phy_port = 0, mmu_port = 0;
    int pgw, xlp, blk, index;
    int instance_mask = 0;

    port = ainfo->port;
    if (REG_PORT_ANY != port) {
        instance_mask = port & SOC_REG_ADDR_INSTANCE_MASK;
        port &= (~SOC_REG_ADDR_INSTANCE_MASK);
    }

    if (!instance_mask) {
        if (!SOC_PORT_VALID(unit, port)) {
            return 0;
        }
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[ainfo->port];
        mmu_port = si->port_p2m_mapping[phy_port];
    }

    if ((ainfo->block >= 0) && !SOC_INFO(unit).block_valid[ainfo->block]) {
        goto skip;
    }

    switch (ainfo->reg) {
    case PORT_INITIAL_COPY_COUNT_WIDTHr:
    case MMU_PORT_TO_LOGIC_PORT_MAPPINGr:
    case MMU_PORT_TO_PHY_PORT_MAPPINGr:
    case MMU_TO_XPORT_BKPr:
    case CHFC2PFC_STATEr:
    case QCN_PFC_STATEr:
        /* @MMU_PORT_LIST */
    case XPORT_TO_MMU_BKPr:
        /* @PFC_PORT_STS_REGS */
    case MTRI_IFGr:
        /* @MMU_MTRI_PORTS */
    case THDI_INPUT_PORT_XON_ENABLESr:
    case THDI_PORT_PRI_GRP0r:
    case THDI_PORT_PRI_GRP1r:
    case THDI_PORT_PG_SPIDr:
    case THDI_PORT_LIMIT_STATESr:
    case THDI_FLOW_CONTROL_XOFF_STATEr:
        /* MMU_THDI_PORTS */
    case THDU_PORT_E2ECC_COS_SPIDr:
        /* THDU_UC_PORT_LIST */
    case ASF_PORT_CFGr:
    case ENQ_ASF_ERRORr:
        /* @ENQ_PORT_LIST */
        break;
    case HSP_SCHED_PORT_CONFIGr:
    case HSP_SCHED_L0_NODE_CONFIGr:
    case HSP_SCHED_L0_NODE_WEIGHTr:
    case HSP_SCHED_L0_NODE_CONNECTION_CONFIGr:
    case HSP_SCHED_L1_NODE_CONFIGr:
    case HSP_SCHED_L1_NODE_WEIGHTr:
    case HSP_SCHED_L2_UC_QUEUE_CONFIGr:
    case HSP_SCHED_L2_MC_QUEUE_CONFIGr:
    case HSP_SCHED_L2_UC_QUEUE_WEIGHTr:
    case HSP_SCHED_L2_MC_QUEUE_WEIGHTr:
        /* @HSP_PORT_LIST => [0..15,64..79] */
        if (!(mmu_port >= 0 && mmu_port <= 15) &&
            !(mmu_port >= 64 && mmu_port <= 79)) {
            goto skip;
        }
        break;
    case MMU_THDM_DB_PORTSP_SHARED_COUNTr:
    case MMU_THDM_DB_PORTSP_TOTAL_COUNTr:
    case MMU_THDM_DB_PORTSP_THRESHOLD_PROFILE_SELr:
    case MMU_THDM_MCQE_PORTSP_SHARED_COUNTr:
    case MMU_THDM_MCQE_PORTSP_TOTAL_COUNTr:
    case MMU_THDM_MCQE_PORTSP_THRESHOLD_PROFILE_SELr:
        /* @THDM_PORT_LIST */
        break;
    case PGW_OBM0_CONTROLr:
    case PGW_OBM0_SHARED_CONFIGr:
    case PGW_OBM0_NIV_ETHERTYPEr:
    case PGW_OBM0_PE_ETHERTYPEr:
    case PGW_OBM0_OUTER_TPIDr:
    case PGW_OBM0_INNER_TPIDr:
    case PGW_OBM0_PRIORITY_MAPr:
    case PGW_OBM0_THRESHOLDr:
    case PGW_OBM0_THRESHOLD2r:
    case PGW_OBM0_LOW_PRI_DROP_PROBr:
    case PGW_OBM0_TM_CONTROLr:
    case PGW_OBM0_ECC_ENABLEr:
    case PGW_OBM0_ECC_STATUSr:
    case PGW_OBM0_USE_COUNTERr:
    case PGW_OBM0_MAX_USAGEr:
    case PGW_OBM0_LOW_PRI_PKT_DROPr:
    case PGW_OBM0_LOW_PRI_BYTE_DROPr:
    case PGW_OBM0_HIGH_PRI_PKT_DROPr:
    case PGW_OBM0_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM1_CONTROLr:
    case PGW_OBM1_SHARED_CONFIGr:
    case PGW_OBM1_NIV_ETHERTYPEr:
    case PGW_OBM1_PE_ETHERTYPEr:
    case PGW_OBM1_OUTER_TPIDr:
    case PGW_OBM1_INNER_TPIDr:
    case PGW_OBM1_PRIORITY_MAPr:
    case PGW_OBM1_THRESHOLDr:
    case PGW_OBM1_THRESHOLD2r:
    case PGW_OBM1_LOW_PRI_DROP_PROBr:
    case PGW_OBM1_TM_CONTROLr:
    case PGW_OBM1_ECC_ENABLEr:
    case PGW_OBM1_ECC_STATUSr:
    case PGW_OBM1_USE_COUNTERr:
    case PGW_OBM1_MAX_USAGEr:
    case PGW_OBM1_LOW_PRI_PKT_DROPr:
    case PGW_OBM1_LOW_PRI_BYTE_DROPr:
    case PGW_OBM1_HIGH_PRI_PKT_DROPr:
    case PGW_OBM1_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM2_CONTROLr:
    case PGW_OBM2_SHARED_CONFIGr:
    case PGW_OBM2_NIV_ETHERTYPEr:
    case PGW_OBM2_PE_ETHERTYPEr:
    case PGW_OBM2_OUTER_TPIDr:
    case PGW_OBM2_INNER_TPIDr:
    case PGW_OBM2_PRIORITY_MAPr:
    case PGW_OBM2_THRESHOLDr:
    case PGW_OBM2_THRESHOLD2r:
    case PGW_OBM2_LOW_PRI_DROP_PROBr:
    case PGW_OBM2_TM_CONTROLr:
    case PGW_OBM2_ECC_ENABLEr:
    case PGW_OBM2_ECC_STATUSr:
    case PGW_OBM2_USE_COUNTERr:
    case PGW_OBM2_MAX_USAGEr:
    case PGW_OBM2_LOW_PRI_PKT_DROPr:
    case PGW_OBM2_LOW_PRI_BYTE_DROPr:
    case PGW_OBM2_HIGH_PRI_PKT_DROPr:
    case PGW_OBM2_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM3_CONTROLr:
    case PGW_OBM3_SHARED_CONFIGr:
    case PGW_OBM3_NIV_ETHERTYPEr:
    case PGW_OBM3_PE_ETHERTYPEr:
    case PGW_OBM3_OUTER_TPIDr:
    case PGW_OBM3_INNER_TPIDr:
    case PGW_OBM3_PRIORITY_MAPr:
    case PGW_OBM3_THRESHOLDr:
    case PGW_OBM3_THRESHOLD2r:
    case PGW_OBM3_LOW_PRI_DROP_PROBr:
    case PGW_OBM3_TM_CONTROLr:
    case PGW_OBM3_ECC_ENABLEr:
    case PGW_OBM3_ECC_STATUSr:
    case PGW_OBM3_USE_COUNTERr:
    case PGW_OBM3_MAX_USAGEr:
    case PGW_OBM3_LOW_PRI_PKT_DROPr:
    case PGW_OBM3_LOW_PRI_BYTE_DROPr:
    case PGW_OBM3_HIGH_PRI_PKT_DROPr:
    case PGW_OBM3_HIGH_PRI_BYTE_DROPr:
        switch (ainfo->reg) {
        case PGW_OBM0_CONTROLr:
        case PGW_OBM0_SHARED_CONFIGr:
        case PGW_OBM0_NIV_ETHERTYPEr:
        case PGW_OBM0_PE_ETHERTYPEr:
        case PGW_OBM0_OUTER_TPIDr:
        case PGW_OBM0_INNER_TPIDr:
        case PGW_OBM0_PRIORITY_MAPr:
        case PGW_OBM0_THRESHOLDr:
        case PGW_OBM0_THRESHOLD2r:
        case PGW_OBM0_LOW_PRI_DROP_PROBr:
        case PGW_OBM0_TM_CONTROLr:
        case PGW_OBM0_ECC_ENABLEr:
        case PGW_OBM0_ECC_STATUSr:
        case PGW_OBM0_USE_COUNTERr:
        case PGW_OBM0_MAX_USAGEr:
        case PGW_OBM0_LOW_PRI_PKT_DROPr:
        case PGW_OBM0_LOW_PRI_BYTE_DROPr:
        case PGW_OBM0_HIGH_PRI_PKT_DROPr:
        case PGW_OBM0_HIGH_PRI_BYTE_DROPr:
            xlp = 0;
            break;
        case PGW_OBM1_CONTROLr:
        case PGW_OBM1_SHARED_CONFIGr:
        case PGW_OBM1_NIV_ETHERTYPEr:
        case PGW_OBM1_PE_ETHERTYPEr:
        case PGW_OBM1_OUTER_TPIDr:
        case PGW_OBM1_INNER_TPIDr:
        case PGW_OBM1_PRIORITY_MAPr:
        case PGW_OBM1_THRESHOLDr:
        case PGW_OBM1_THRESHOLD2r:
        case PGW_OBM1_LOW_PRI_DROP_PROBr:
        case PGW_OBM1_TM_CONTROLr:
        case PGW_OBM1_ECC_ENABLEr:
        case PGW_OBM1_ECC_STATUSr:
        case PGW_OBM1_USE_COUNTERr:
        case PGW_OBM1_MAX_USAGEr:
        case PGW_OBM1_LOW_PRI_PKT_DROPr:
        case PGW_OBM1_LOW_PRI_BYTE_DROPr:
        case PGW_OBM1_HIGH_PRI_PKT_DROPr:
        case PGW_OBM1_HIGH_PRI_BYTE_DROPr:
            xlp = 1;
            break;
        case PGW_OBM2_CONTROLr:
        case PGW_OBM2_SHARED_CONFIGr:
        case PGW_OBM2_NIV_ETHERTYPEr:
        case PGW_OBM2_PE_ETHERTYPEr:
        case PGW_OBM2_OUTER_TPIDr:
        case PGW_OBM2_INNER_TPIDr:
        case PGW_OBM2_PRIORITY_MAPr:
        case PGW_OBM2_THRESHOLDr:
        case PGW_OBM2_THRESHOLD2r:
        case PGW_OBM2_LOW_PRI_DROP_PROBr:
        case PGW_OBM2_TM_CONTROLr:
        case PGW_OBM2_ECC_ENABLEr:
        case PGW_OBM2_ECC_STATUSr:
        case PGW_OBM2_USE_COUNTERr:
        case PGW_OBM2_MAX_USAGEr:
        case PGW_OBM2_LOW_PRI_PKT_DROPr:
        case PGW_OBM2_LOW_PRI_BYTE_DROPr:
        case PGW_OBM2_HIGH_PRI_PKT_DROPr:
        case PGW_OBM2_HIGH_PRI_BYTE_DROPr:
            xlp = 2;
            break;
        case PGW_OBM3_CONTROLr:
        case PGW_OBM3_SHARED_CONFIGr:
        case PGW_OBM3_NIV_ETHERTYPEr:
        case PGW_OBM3_PE_ETHERTYPEr:
        case PGW_OBM3_OUTER_TPIDr:
        case PGW_OBM3_INNER_TPIDr:
        case PGW_OBM3_PRIORITY_MAPr:
        case PGW_OBM3_THRESHOLDr:
        case PGW_OBM3_THRESHOLD2r:
        case PGW_OBM3_LOW_PRI_DROP_PROBr:
        case PGW_OBM3_TM_CONTROLr:
        case PGW_OBM3_ECC_ENABLEr:
        case PGW_OBM3_ECC_STATUSr:
        case PGW_OBM3_USE_COUNTERr:
        case PGW_OBM3_MAX_USAGEr:
        case PGW_OBM3_LOW_PRI_PKT_DROPr:
        case PGW_OBM3_LOW_PRI_BYTE_DROPr:
        case PGW_OBM3_HIGH_PRI_PKT_DROPr:
        case PGW_OBM3_HIGH_PRI_BYTE_DROPr:
            xlp = 3;
            break;
        default:
            return SOC_E_INTERNAL;
        }
        pgw = port;
        index = pgw * 4 + xlp;
        for (blk = 0; SOC_BLOCK_TYPE(unit, blk) >= 0; blk++) {
            if (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_XLPORT) {
                continue;
            }
            if (SOC_BLOCK_NUMBER(unit, blk) == index) {
                if (!SOC_INFO(unit).block_valid[blk]) {
                    goto skip;
                }
            }
        }
        break;
    default:
        break;
    }

    return 0;
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC int
reg_mask_subset_tomahawk3(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    /* Enable all register accesses */
    soc_info_t *si;
    int phy_port, port_value;
    soc_reg_t reg=ainfo->reg;
    soc_reg_t reg_excep_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                                MMU_RQE_REPL_PORT_AGG_MAPr};
    int reg_index, num_regs, add_exception = 0;


    if (!SOC_PORT_VALID(unit, ainfo->port)) {
           return 0;
    }
    si = &SOC_INFO(unit);
    if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_GLB) ||
        SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_ITM) ||
        SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_EB)) {
        num_regs = sizeof(reg_excep_list) / sizeof(soc_reg_t);
        for(reg_index = 0;reg_index < num_regs;reg_index++) {
            if (reg_excep_list[reg_index] == reg) {
                add_exception = 1;
                break;
            }
        }
        if (add_exception) {
            port_value = ainfo->port;
        } else {
            phy_port = si->port_l2p_mapping[ainfo->port];
            port_value = si->port_p2m_mapping[phy_port];
        }
    } else {
        port_value = ainfo->port;
    }
    if (!SOC_REG_PORT_IDX_VALID(unit, reg, port_value, ainfo->idx)) {
        goto skip;
    }

    return 0;

skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;

}
#endif

#ifdef BCM_TOMAHAWK_SUPPORT

STATIC int
reg_mask_subset_tomahawk(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    soc_info_t *si;
    soc_reg_t reg;
    int phy_port, port_value, inst;
    int instance_mask = 0;
    int xpe, acc_type, base_type;
    soc_reg_info_t *reginfo;

    reg = ainfo->reg;
    reginfo = &SOC_REG_INFO(unit, reg);

    if (REG_PORT_ANY != ainfo->port) {
        instance_mask = ainfo->port & SOC_REG_ADDR_INSTANCE_MASK;
        if (instance_mask) {
            inst = ainfo->port & ~SOC_REG_ADDR_INSTANCE_MASK;
            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_XPE)) {
                if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
                    switch (reginfo->regtype) {
                    case soc_layerreg:
                        break;
                    default:
                        reg = SOC_REG_UNIQUE_ACC(unit, reg)[0];
                        break;
                    }
                }
                acc_type = SOC_REG_ACC_TYPE(unit, reg);
                xpe = acc_type < SOC_MAX_NUM_PIPES ? acc_type : -1;
                base_type = SOC_REG_BASE_TYPE(unit, reg);
                if (soc_tomahawk_xpe_base_index_check(unit, base_type, xpe,
                                                      inst, NULL) < 0) {
                    goto skip;
                }
            } else if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_SC)) {
                if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
                    reg = SOC_REG_UNIQUE_ACC(unit, reg)[0];
                }
                acc_type = SOC_REG_ACC_TYPE(unit, reg);
                xpe = acc_type < SOC_MAX_NUM_PIPES ? acc_type : -1;
                base_type = SOC_REG_BASE_TYPE(unit, reg);
                if (soc_tomahawk_sc_base_index_check(unit, base_type, xpe,
                                                     inst, NULL) < 0) {
                    goto skip;
                }
#ifdef BCM_TOMAHAWK2_SUPPORT
            } else if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_SED)) {
                if (SOC_REG_UNIQUE_ACC(unit, reg) != NULL) {
                    reg = SOC_REG_UNIQUE_ACC(unit, reg)[0];
                }
                acc_type = SOC_REG_ACC_TYPE(unit, reg);
                xpe = acc_type < SOC_MAX_NUM_PIPES ? acc_type : -1;
                base_type = SOC_REG_BASE_TYPE(unit, reg);
                if (soc_tomahawk2_sed_base_index_check(unit, base_type, xpe,
                                                       inst, NULL) < 0) {
                    goto skip;
                }
#endif /* BCM_TOMAHAWK2_SUPPORT */
            }
        } else {
            if (!SOC_PORT_VALID(unit, ainfo->port)) {
                return 0;
            }
            si = &SOC_INFO(unit);
            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_GLB) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_XPE) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_SED) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MMU_SC)) {
                phy_port = si->port_l2p_mapping[ainfo->port];
                port_value = si->port_p2m_mapping[phy_port];
            } else {
                port_value = ainfo->port;
            }
            if (!SOC_REG_PORT_IDX_VALID(unit, reg, port_value, ainfo->idx)) {
                goto skip;
            }
        }
    } else {
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) {
            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_OTPC)) {
                inst = SOC_BLOCK_NUMBER(unit, ainfo->block);
                /* Can not read OTPC1 registers */
                if (inst == 1) {
                    goto skip;
                }
            }
        }
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_TRIDENT3_SUPPORT*/
    }

    if (SOC_IS_TRIDENT3X(unit)) {
        switch (ainfo->reg) {
            case XLPORT_MIB_RSC_RAM_CONTROLr:
            /* RH_DLB_SELECTION field in ENHANCED_HASHING_CONTROL needs to be set to 1
             *  in oder to use RH. This can't be set by default to 1 in init time.
             *  Hence skip RH related following registers during test.
             */
            case RH_ECMP1_DROPSr:
            case RH_ECMP1_DROPS_PIPE0r:
            case RH_ECMP1_DROPS_PIPE1r:
            case RH_ECMP2_DROPSr:
            case RH_ECMP2_DROPS_PIPE0r:
            case RH_ECMP2_DROPS_PIPE1r:
            case RH_HGT_DROPSr:
            case RH_HGT_DROPS_PIPE0r:
            case RH_HGT_DROPS_PIPE1r:
            case RH_LAG_DROPSr:
            case RH_LAG_DROPS_PIPE0r:
            case RH_LAG_DROPS_PIPE1r:
            /* CCH psuedo registers need to be skipped */
            case FLEXIBLE_IPV6_EXT_HDRr:
            case ING_FCOE_ETHERTYPEr:
            case ING_L2_TUNNEL_PARSE_CONTROLr:
            case MIM_ETHERTYPEr:
            case ING_PACKET_PROCESSING_CONTROL_0r:
            case ING_PACKET_PROCESSING_CONTROL_1r:
#ifdef BCM_HELIX5_SUPPORT
           case TOP_XG_PLL0_CTRL_0r:
           case TOP_XG_PLL0_CTRL_1r:
           case TOP_XG_PLL0_CTRL_2r:
           case TOP_XG_PLL0_CTRL_3r:
           case TOP_XG_PLL0_CTRL_4r:
           case TOP_XG_PLL0_CTRL_5r:
           case TOP_XG_PLL0_CTRL_6r:
           case TOP_XG_PLL0_CTRL_7r:
           case TOP_XG_PLL0_CTRL_8r:
           case TOP_XG_PLL0_CTRL_9r:
           case TOP_XG_PLL0_CTRL_10r:
           case TOP_XG_PLL0_CTRL_11r:
           case TOP_XG_PLL0_CTRL_12r:
           case TOP_XG_PLL0_CTRL_13r:
           case TOP_XG_PLL0_STATUSr:
           case TOP_XG_PLL1_CTRL_0r:
           case TOP_XG_PLL1_CTRL_1r:
           case TOP_XG_PLL1_CTRL_2r:
           case TOP_XG_PLL1_CTRL_3r:
           case TOP_XG_PLL1_CTRL_4r:
           case TOP_XG_PLL1_CTRL_5r:
           case TOP_XG_PLL1_CTRL_6r:
           case TOP_XG_PLL1_CTRL_7r:
           case TOP_XG_PLL1_CTRL_8r:
           case TOP_XG_PLL1_CTRL_9r:
           case TOP_XG_PLL1_CTRL_10r:
           case TOP_XG_PLL1_CTRL_11r:
           case TOP_XG_PLL1_CTRL_12r:
           case TOP_XG_PLL1_CTRL_13r:
           case TOP_XG_PLL1_STATUSr:
           case TOP_HW_TAP_CONTROLr:
#endif
                goto skip;
            default:
                if (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_AVS)) {
                    /* Skip AVS block registers */
                   goto skip;
                }
                break;
        }
    }
 
    return 0;

skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
STATIC int
reg_mask_subset_apache(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    int port, instance_mask = 0;

    port = ainfo->port;

    if (REG_PORT_ANY != port) {

        instance_mask = port & SOC_REG_ADDR_INSTANCE_MASK;
        if (instance_mask) {
            port &= (~SOC_REG_ADDR_INSTANCE_MASK);
        }

        /* CLPORT registers - Only CL and CLG2 ports */
        if (((ainfo->reg >= CLPORT_CNTMAXSIZEr) &&
             (ainfo->reg <= CLPORT_XGXS_COUNTER_MODEr)) &&
            (!(IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)))) {

            goto skip;
        }

        /* CLMAC registers - Only CL ports */
        if (((ainfo->reg >= CLMAC_CLEAR_ECC_STATUSr) &&
             (ainfo->reg <= CLMAC_TX_MAC_SAr)) &&
            (!IS_CL_PORT(unit, port))) {

            goto skip;
        }

        /* CLG2MAC registers - Only CLG2 ports */
        if (((ainfo->reg >= CLG2MAC_CLEAR_ECC_STATUSr) &&
             (ainfo->reg <= CLG2MAC_TX_MAC_SAr)) &&
            (!IS_CLG2_PORT(unit, port))) {

            goto skip;
        }


        /* XLPORT registers - Only XL, XLB0 and CXX ports */
        if (((ainfo->reg >= XLPORT_CNTMAXSIZEr) &&
             (ainfo->reg <= XLPORT_XGXS_COUNTER_MODEr)) &&
            (!(IS_XL_PORT(unit, port) || IS_XLB0_PORT(unit, port) || IS_CXX_PORT(unit, port)))) {

            goto skip;
        }

        /* XLMAC registers - Only XL ports */
        if (((ainfo->reg >= XLMAC_CLEAR_ECC_STATUSr) &&
             (ainfo->reg <= XLMAC_TX_MAC_SAr)) &&
            (!IS_XL_PORT(unit, port))) {

            goto skip;
        }

        /* XLMAC_B0 registers - Only XLB0 ports */
        if (((ainfo->reg >= XLMAC_B0_CLEAR_ECC_STATUSr) &&
             (ainfo->reg <= XLMAC_B0_TX_MAC_SAr)) &&
            (!IS_XLB0_PORT(unit, port))) {

            goto skip;
        }

        /* SOC_REG_PORT_IDX_VALID check should be done with appropriate
         * port type; mmu port for mmu block registers. phy port for port registers
 */
        if (SOC_REG_BLOCK_IN_LIST(unit, ainfo->reg, SOC_BLK_MMU)) {
            /* mmu port */
            port = SOC_INFO(unit).port_p2m_mapping[SOC_INFO(unit).port_l2p_mapping[port]];
        } else if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, ainfo->reg).block, SOC_BLK_PORT)) {
            /* phy port */
            port = SOC_INFO(unit).port_l2p_mapping[port];
        }

        /* Per-Port register is not implemented in the regsiter array */
        if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, port, ainfo->idx)) {
            goto skip;
        }

    } else {
        if (!SOC_INFO(unit).block_valid[ainfo->block]) {
            goto skip;
        }
    }

    switch (ainfo->reg) {

#ifdef TR_TESTS_EXCLUDE
    case CFAPCONFIGr:
    case CFAPFULLSETPOINTr:
    case CFAPFULLRESETPOINTr:
        /* Only a few values (See regsfile) can be set. Skip. */
        goto skip;
#endif
    case AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROLr:
    case AVS_REG_PMB_SLAVE_AVS_PWD_CONTROLr:
    case AVS_REG_PMB_SLAVE_AVS_ROSC_CONTROLr:
    case AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr:
    case AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr:
    case AVS_REG_PMB_SLAVE_BPCM_CONTROLr:
        /* ARS wrappers stubbed out in QT. Register access will fail */
         if (SAL_BOOT_QUICKTURN) {
             goto skip;
         }
         break;  
#if defined(BCM_MONTEREY_SUPPORT) 
     case SP_GLB_MIB_CTRLr :
     case XLPORT_TXPI_CSR_CTRL1r :
              if (SOC_IS_MONTEREY(unit)) { 
                  goto skip;
               } 
#endif 
    default:
        break;
    }

    return 0;

skip:

    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
STATIC int
reg_mask_subset_tr3(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    pbmp_t *tr3_8mcq_ports_32_35;
    pbmp_t *tr3_8mcq_ports_36_39;
    pbmp_t *tr3_10mcq_ports_40_47;
    pbmp_t *tr3_10mcq_ports_48_55;
    pbmp_t *tr3_wlan_encap_ports;
    pbmp_t *tr3_pass_thru_ports;
    pbmp_t *tr3_cpu_ports;
    pbmp_t *tr3_high_speed_ports;
    pbmp_t *tr3_all_ports;
    pbmp_t *tr3_cl_ports;
    pbmp_t *tr3_repl_head_ports;
    pbmp_t *tr3_ports_none;
#define _REG_MASK_SUBSET_MAX_TR3 12
    pbmp_t **mask_map_arr[_REG_MASK_SUBSET_MAX_TR3];
    int i, count = _REG_MASK_SUBSET_MAX_TR3;
    uint64 temp_mask;
    pbmp_t *pbmp;
    soc_port_t port;
    uint16 dev_id;
    uint8 rev_id;

    mask_map_arr[0] = &tr3_8mcq_ports_32_35;
    mask_map_arr[1] = &tr3_8mcq_ports_36_39;
    mask_map_arr[2] = &tr3_10mcq_ports_40_47;
    mask_map_arr[3] = &tr3_10mcq_ports_48_55;
    mask_map_arr[4] = &tr3_wlan_encap_ports;
    mask_map_arr[5] = &tr3_pass_thru_ports;
    mask_map_arr[6] = &tr3_cpu_ports;
    mask_map_arr[7] = &tr3_high_speed_ports;
    mask_map_arr[8] = &tr3_all_ports;
    mask_map_arr[9] = &tr3_cl_ports;
    mask_map_arr[10] = &tr3_repl_head_ports;
    mask_map_arr[11] = &tr3_ports_none;
    if (NULL == SOC_REG_MASK_SUBSET(unit)) {
        soc_info_t *si = &SOC_INFO(unit);
        soc_port_t mmu_port, phy_port;
        
        _INIT_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

        /* 8MCQ = MMU[32-35] */
        SOC_PBMP_CLEAR(*tr3_8mcq_ports_32_35);
        for (mmu_port = 32; mmu_port < 36; mmu_port++) {
            phy_port = si->port_m2p_mapping[mmu_port];
            port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
            if (-1 != port) {
                SOC_PBMP_PORT_ADD(*tr3_8mcq_ports_32_35, port);
            }
        }

        /* 8MCQ = MMU[36-39] */
        SOC_PBMP_CLEAR(*tr3_8mcq_ports_36_39);
        for (mmu_port = 36; mmu_port < 40; mmu_port++) {
            phy_port = si->port_m2p_mapping[mmu_port];
            port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
            if (-1 != port) {
                SOC_PBMP_PORT_ADD(*tr3_8mcq_ports_36_39, port);
            }
        }

        /* 10MCQ = MMU[40-55] */
        SOC_PBMP_CLEAR(*tr3_10mcq_ports_40_47);
        for (mmu_port = 40; mmu_port < 48; mmu_port++) {
            phy_port = si->port_m2p_mapping[mmu_port];
            port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
            if (-1 != port) {
                SOC_PBMP_PORT_ADD(*tr3_10mcq_ports_40_47, port);
            }
        }

        SOC_PBMP_CLEAR(*tr3_10mcq_ports_48_55);
        for (mmu_port = 48; mmu_port < 56; mmu_port++) {
            phy_port = si->port_m2p_mapping[mmu_port];
            port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
            if (-1 != port) {
                SOC_PBMP_PORT_ADD(*tr3_10mcq_ports_48_55, port);
            }
        }

        /* CPU port = MMU[59] */
        SOC_PBMP_CLEAR(*tr3_cpu_ports);
        SOC_PBMP_PORT_ADD(*tr3_cpu_ports, CMIC_PORT(unit));

        /* WLAN Encap = MMU[56] */
        SOC_PBMP_CLEAR(*tr3_wlan_encap_ports);
        SOC_PBMP_PORT_ADD(*tr3_wlan_encap_ports,
                          AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP));

        /* Pass thru = MMU[58] */
        SOC_PBMP_CLEAR(*tr3_pass_thru_ports);
        SOC_PBMP_PORT_ADD(*tr3_pass_thru_ports,
                          AXP_PORT(unit, SOC_AXP_NLF_PASSTHRU));

        /* High speed ports = MMU[48,52] */
        SOC_PBMP_CLEAR(*tr3_high_speed_ports);
        phy_port = si->port_m2p_mapping[48];
        port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
        if (-1 != port) {
            SOC_PBMP_PORT_ADD(*tr3_high_speed_ports, port);
        }
        phy_port = si->port_m2p_mapping[52];
        port = (phy_port == -1) ? -1 : si->port_p2l_mapping[phy_port];
        if (-1 != port) {
            SOC_PBMP_PORT_ADD(*tr3_high_speed_ports, port);
        }

        /* CE ports */
        SOC_PBMP_CLEAR(*tr3_cl_ports);
        PBMP_CE_ITER(unit, port) {
            SOC_PBMP_PORT_ADD(*tr3_cl_ports, port);
        }
        PBMP_HG_ITER(unit, port) {
            if (si->port_speed_max[port] >= 100000) {
                SOC_PBMP_PORT_ADD(*tr3_cl_ports, port);
            }
        }

        /* All ports */
        SOC_PBMP_CLEAR(*tr3_all_ports); 
        SOC_PBMP_ASSIGN(*tr3_all_ports, PBMP_ALL(unit)); 

        /* Ports for REPL_HEAD_PTR_REPLACE */
        SOC_PBMP_ASSIGN(*tr3_repl_head_ports, *tr3_all_ports);
        SOC_PBMP_PORT_REMOVE(*tr3_repl_head_ports, CMIC_PORT(unit));

        SOC_PBMP_CLEAR(*tr3_ports_none);
    }

    /* Non-port register */
    if (mask != NULL) {
        switch(ainfo->reg) {
        case ETU_CONFIG0r:
            if (!soc_feature(unit, soc_feature_esm_support)) {
                 goto skip;
	    }
            if (!soc_feature(unit, soc_feature_etu_support)) {
                /* HBIT_MEM_PSM_VDD bits are stuck without ESM */
                COMPILER_64_SET(temp_mask, 0, 0x00027fff);
                COMPILER_64_AND(*mask, temp_mask);
	     }
            break;
        case TOP_XG_PLL0_CTRL_0r:
        case TOP_XG_PLL1_CTRL_0r:
        case TOP_XG_PLL2_CTRL_0r:
        case TOP_XG_PLL3_CTRL_0r:
        case AUX_ARB_CONTROL_2r:
        case TOP_TAP_CONTROLr:
            
            goto skip;
       case ESMIF_CNP_SEARCH_REQ_COUNTr:
       case ESMIF_CUR_PENDING_COST0r:
       case ESMIF_CUR_PENDING_COST1r:
       case ESMIF_CUR_PENDING_COST2r:
       case ESMIF_DROP_COUNTr:
       case ESMIF_INIT_CONFIGr:
       case ESMIF_MAX_PENDING_COST0r:
       case ESMIF_MAX_PENDING_COST1r:
       case ESMIF_MAX_PENDING_COST2r:
       case ESMIF_MULTI_ECC_PARITY_ERROR_LOGr:
       case ESM_ADM_CTRL_FIFO_STATUSr:
       case ESM_ADM_THR_DROP_ALL_COST0r:
       case ESM_ADM_THR_DROP_ALL_COST1r:
       case ESM_ADM_THR_DROP_ALL_COST2r:
       case ESM_ADM_THR_DROP_ALL_RESET_COST0r:
       case ESM_ADM_THR_DROP_ALL_RESET_COST1r:
       case ESM_ADM_THR_DROP_ALL_RESET_COST2r:
       case ESM_ADM_THR_DROP_OPT_COST0r:
       case ESM_ADM_THR_DROP_OPT_COST1r:
       case ESM_ADM_THR_DROP_OPT_COST2r:
       case ESM_ADM_THR_DROP_OPT_RESET_COST0r:
       case ESM_ADM_THR_DROP_OPT_RESET_COST1r:
       case ESM_ADM_THR_DROP_OPT_RESET_COST2r:
       case ESM_AGE_CNTr:
       case ESM_AXP_THR_XOFF_COST0r:
       case ESM_AXP_THR_XOFF_COST1r:
       case ESM_AXP_THR_XOFF_COST2r:
       case ESM_AXP_THR_XOFF_RESET_COST0r:
       case ESM_AXP_THR_XOFF_RESET_COST1r:
       case ESM_AXP_THR_XOFF_RESET_COST2r:
       case ESM_CONTAINER_SELECTr:
       case ESM_CTLr:
       case ESM_ERR_CTLr:
       case ESM_ET_HWTL_CONTROLr:
       case ESM_ET_HWTL_EVENT_ERR_INFOr:
       case ESM_ET_HWTL_MM_INFOr:
       case ESM_ET_HWTL_STATUS_0r:
       case ESM_ET_HWTL_STATUS_1r:
       case ESM_ET_RSP_CBA_MM_INFOr:
       case ESM_ET_RSP_FIFO_STATUSr:
       case ESM_EVENT_ERR_STATUS_INTRr:
       case ESM_EVENT_ERR_STATUS_INTR_ENABLEr:
       case ESM_EXT_HITBIT_CONTROLr:
       case ESM_FIFO_MAX_COUNTSr:
       case ESM_KEYGEN_CTLr:
       case ESM_L2_AGE_CTLr:
       case ESM_L2_AGE_STATUSr:
       case ESM_MAX_LATENCY_RECORDEDr:
       case ESM_MEMORY_DBGCTRL_0r:
       case ESM_MEMORY_DBGCTRL_1r:
       case ESM_MEMORY_DBGCTRL_2r:
       case ESM_MEMORY_DBGCTRL_3r:
       case ESM_MEMORY_DBGCTRL_4r:
       case ESM_MISC_CONTROLr:
       case ESM_MISC_STATUSr:
       case ESM_MODE_PER_PORTr:
       case ESM_OPT_REQ_INTERVALr:
       case ESM_PER_PORT_AGE_CONTROLr:
       case ESM_PER_PORT_REPL_CONTROLr:
       case ESM_PKT_TYPE_ID_CAM_BIST_CONFIGr:
       case ESM_PKT_TYPE_ID_CAM_BIST_DBGCTRLr:
       case ESM_PKT_TYPE_ID_CAM_BIST_DBG_DATAr:
       case ESM_PKT_TYPE_ID_CAM_BIST_STATUSr:
       case ESM_PKT_TYPE_ID_CAM_DBGCTRLr:
       case ESM_PPA_STATUSr:
       case ESM_REQUEST_LANE_MAPr:
       case ESM_REUSE_COUNTER_0r:
       case ESM_REUSE_COUNTER_1r:
       case ESM_REUSE_COUNTER_2r:
       case ESM_REUSE_COUNTER_3r:
       case ESM_SEARCH_PROFILE_BASEr:
       case ETU_CONFIG1r:
       case ETU_CONFIG2r:
       case ETU_CONFIG4r:
            if (!soc_feature(unit, soc_feature_esm_support)) {
            goto skip;
            }
       /*coverity[unterminated_case:FALSE]*/	    
        case ICFG_EN_COR_ERR_RPTr:
        case IPARS_EN_COR_ERR_RPTr:
        case IESMIF_EN_COR_ERR_RPTr:
        case EGR_EL3_EN_COR_ERR_RPTr:
        case EGR_EHCPM_EN_COR_ERR_RPTr:
        case EGR_1588_TIMER_VALUEr:
        case EGR_1588_EGRESS_CTRLr:
        case EGR_1588_TIMER_DEBUGr:
        case EGR_1588_PARSING_CONTROLr:
        case LLS_CONFIG0r:
        case LLS_SP_WERR_DYN_CHANGE_0r:
        case LLS_SP_WERR_DYN_CHANGE_1r:
        case LLS_SP_WERR_DYN_CHANGE_2r:
        case LLS_SP_WERR_DYN_CHANGE_3r:
        case LLS_SP_WERR_DYN_CHANGE_0Ar:
        case LLS_SP_WERR_DYN_CHANGE_1Ar:
        case LLS_SP_WERR_DYN_CHANGE_2Ar:
        case LLS_SP_WERR_DYN_CHANGE_3Ar:
        case LLS_SP_WERR_DYN_CHANGE_0Br:
        case LLS_SP_WERR_DYN_CHANGE_1Br:
        case LLS_SP_WERR_DYN_CHANGE_2Br:
        case LLS_SP_WERR_DYN_CHANGE_3Br:
        case LLS_TDM_CAL_CFGr:
        case LLS_FC_CONFIGr:   
        case ISM_INTR_MASKr:
        case PORT_MODE_REGr:
        case PORT_TS_TIMER_31_0_REGr:
        case PORT_TS_TIMER_47_32_REGr:
        case XMAC_OSTS_TIMESTAMP_ADJUSTr:
            soc_cm_get_id(unit, &dev_id, &rev_id);
            if (rev_id < BCM56640_B0_REV_ID) {
                goto skip;
            }
        default:
            break;
        }
    }

    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }
    _MAP_REG_MASK_SUBSETS(unit, i, count, mask_map_arr);

    switch (ainfo->reg) {
    case MCQ_FIFO_BASE_REG_32_35r:
        /* PORTLIST => MMU[32..35] 8 MCQ (High-speed) */
        pbmp = tr3_8mcq_ports_32_35;
        break;
    case MCQ_FIFO_BASE_REG_36_39r:
        /* PORTLIST => MMU[36..39] 8 MCQ (High-speed) */
        pbmp = tr3_8mcq_ports_36_39;
        break;
    case MCQ_FIFO_BASE_REGr:
        /* PORTLIST => MMU[40..55] 10 MCQ (High-speed) */
        pbmp = tr3_10mcq_ports_40_47;
        break;
    case MCQ_FIFO_BASE_REG_48_55r:
        /* PORTLIST => MMU[40..55] 10 MCQ (High-speed) */
        pbmp = tr3_10mcq_ports_48_55;
        break;
    case MCQ_FIFO_BASE_REG_56r:
        /* PORTLIST => [56] */
        pbmp = tr3_wlan_encap_ports;
        break;
    case MCQ_FIFO_BASE_REG_PASSTHRUr:
        /* PORTLIST => [58] */
        pbmp = tr3_pass_thru_ports;
        break;
    case MCQ_FIFO_BASE_REG_CPUr:
        /* PORTLIST => [59] */
        pbmp = tr3_cpu_ports;
        break;

    case HES_PORT_CONFIGr:
    case HES_L0_CONFIGr:
    case HES_Q_MINSPr:
    case HES_Q_COSMASKr:
    case HES_Q_COSWEIGHTSr:
    case HES_Q_WERRCOUNTr:
        /* @MMU_HSP_PORT_NUMBERS (High-speed ports [48,52])*/
        pbmp = tr3_high_speed_ports;
        break;

    case CMAC_CTRLr:
    case CMAC_MODEr:
    case CMAC_SPARE0r:
    case CMAC_SPARE1r:
    case CMAC_TX_CTRLr:
    case CMAC_TX_MAC_SAr:
    case CMAC_RX_CTRLr:
    case CMAC_RX_MAC_SAr:
    case CMAC_RX_MAX_SIZEr:
    case CMAC_RX_VLAN_TAGr:
    case CMAC_RX_LSS_CTRLr:
    case CMAC_RX_LSS_STATUSr:
    case CMAC_CLEAR_RX_LSS_STATUSr:
    case CMAC_PAUSE_CTRLr:
    case CMAC_PFC_CTRLr:
    case CMAC_PFC_TYPEr:
    case CMAC_PFC_OPCODEr:
    case CMAC_PFC_DAr:
    case CMAC_LLFC_CTRLr:
    case CMAC_TX_LLFC_MSG_FIELDSr:
    case CMAC_RX_LLFC_MSG_FIELDSr:
    case CMAC_HCFC_CTRLr:
    case CMAC_TX_TIMESTAMP_FIFO_DATAr:
    case CMAC_TX_TIMESTAMP_FIFO_STATUSr:
    case CMAC_FIFO_STATUSr:
    case CMAC_CLEAR_FIFO_STATUSr:
    case CMAC_TX_FIFO_CREDITSr:
    case CMAC_EEE_CTRLr:
    case CMAC_EEE_TIMERSr:
    case CMAC_EEE_1_SEC_LINK_STATUS_TIMERr:
    case CMAC_TIMESTAMP_ADJUSTr:
    case CMAC_MACSEC_CTRLr:
    case CMAC_ECC_CTRLr:
    case CMAC_ECC_STATUSr:
    case CMAC_CLEAR_ECC_STATUSr:
    case CMAC_VERSION_IDr:
        /* CMAC registers (Only CL configured ports)*/
        pbmp = tr3_cl_ports;
        break;

    case REPL_HEAD_PTR_REPLACEr:
        if (!soc_feature(unit, soc_feature_repl_head_ptr_replace)) {
            pbmp = tr3_ports_none;
            break;
        }
        /* [0..56,58,60,61] */
        pbmp = tr3_repl_head_ports;
        break;
    
    case THDO_PORT_E2ECC_COS_SPIDr:
    case PORT_SW_FLOW_CONTROLr:
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if ((rev_id < BCM56640_B0_REV_ID) && !SOC_IS_HELIX4(unit)) {
            pbmp = tr3_ports_none;
            break;
        }
        /* Fall through */

    default:
        pbmp = tr3_all_ports;
        break;
    }

    if ((soc_portreg == SOC_REG_INFO(unit, ainfo->reg).regtype) &&
        (!SOC_PBMP_MEMBER(*pbmp, ainfo->port))) {
        goto skip; 
    }

    switch (ainfo->reg) {

    case DEQ_AGINGMASKr:
        /* PORTLIST => MMU[0..58,60..62] Non-CPU */
    case XPORT_TO_MMU_BKPr:
        /* @PFC_PORT_STS_REGS Non-CPU */
    case BKPMETERINGCONFIG_64r:
    case BKPMETERINGBUCKETr:
    case MTRI_IFGr:
        /* @MMU_MTRI_PORTS = MMU[0..58,60..62] */
        if (IS_CPU_PORT(unit, ainfo->port)) {
            goto skip;
        }
        break;
    case OP_PORT_CONFIG_CELLr:
    case OP_PORT_CONFIG1_CELLr:
    case OP_PORT_LIMIT_COLOR_CELLr:
    case OP_PORT_SHARED_COUNT_CELLr:
    case OP_PORT_TOTAL_COUNT_CELLr:
    case OP_PORT_LIMIT_RESUME_COLOR_CELLr:
        /* @THDO_MC_PORT_LIST = MMU[0..56,58..62] */
        if (ainfo->port > AXP_PORT(unit, SOC_AXP_NLF_SM)) {
            goto skip;
        }
        break;

    case MCQ_FIFO_BASE_REG_CPUr:
        /* PORTLIST => [59] */
    case OP_CPU_QUEUE_BST_STATr:
    case OP_CPU_QUEUE_BST_THRESHOLDr:
        /* @THDO_CPU_BST_REGS */
        if (!IS_CPU_PORT(unit, ainfo->port)) {
            goto skip;
        }
        break;

    case L0_COSWEIGHTSr:
    case L0_WERRCOUNTr:
        /* @MMU_ES_REGS[0-9]=[48,52] */
        if (!SOC_PBMP_MEMBER(*tr3_high_speed_ports, ainfo->port)) {
            goto skip;
        }
        break;

    case OVQ_MCQ_CREDITSr:
    case OVQ_MCQ_STATEr:
        /* @OVQ_PORT_COS_REGS (all ports except cpu and lb port) */
        if (ainfo->port == AXP_PORT(unit, SOC_AXP_NLF_WLAN_DECAP)) {
            goto skip;
        }
        /* Fall thru */
    case OP_QUEUE_CONFIG_CELLr:
    case OP_QUEUE_CONFIG1_CELLr:
    case OP_QUEUE_LIMIT_COLOR_CELLr:
    case OP_QUEUE_RESET_OFFSET_CELLr:
    case OP_QUEUE_MIN_COUNT_CELLr:
    case OP_QUEUE_SHARED_COUNT_CELLr:
    case OP_QUEUE_TOTAL_COUNT_CELLr:
    case OP_QUEUE_RESET_VALUE_CELLr:
    case OP_QUEUE_LIMIT_RESUME_COLOR_CELLr:
        /* @THDO_PORT_COS_REGS */
        if ((ainfo->port == AXP_PORT(unit, SOC_AXP_NLF_SM)) || 
            (ainfo->port == AXP_PORT(unit, SOC_AXP_NLF_WLAN_DECAP))) {
             if (ainfo->idx >= 1) {
                goto skip;
            }
        } else if (SOC_PBMP_MEMBER(*tr3_10mcq_ports_40_47, ainfo->port) ||
                   SOC_PBMP_MEMBER(*tr3_10mcq_ports_48_55, ainfo->port)) {
             if (ainfo->idx >= 10) {
                goto skip;
            }
        } else if (!IS_CPU_PORT(unit, ainfo->port)) {
             if (ainfo->idx >= 8) {
                goto skip;
            }
        }
        /* Else CPU port, all queues allowed */
        break;
    case TOP_HW_TAP_MEM_ECC_CTRLr:
        if (SOC_IS_HELIX4(unit)) {
            goto skip;
        }
    default:
        break;
    }

    return 0;
skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#if 0 /* #ifdef BCM_KATANA_SUPPORT */
/* Remove this completely once the new function is fully verified */
STATIC int
reg_mask_subset_katana(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    soc_info_t *si;
    int i;
    static int kt_port_init = 0;
    static pbmp_t kt_ipipe_higig_ports;
    static pbmp_t kt_epipe_ports_subports;
    static pbmp_t kt_ports_0_35;
    static pbmp_t kt_ports_25_28;
    static pbmp_t kt_mmu_mtri_ports;
    static pbmp_t kt_all_ports;
    uint64 temp_mask;
    pbmp_t *pbmp;

    if (!kt_port_init) {

        /* IPIPE_HIGIG_PORTS = [0,25,26,27,28,29,30,31] */
        SOC_PBMP_CLEAR(kt_ipipe_higig_ports);
        SOC_PBMP_PORT_ADD(kt_ipipe_higig_ports, 0);
        for (i = 25; i <= 31; i++) {
            SOC_PBMP_PORT_ADD(kt_ipipe_higig_ports, i);
        }

        /* EPIPE_PORTS_SUBPORTS = [0..38] */
        SOC_PBMP_CLEAR(kt_epipe_ports_subports);
        for (i = 0; i <= 38; i++) {
            SOC_PBMP_PORT_ADD(kt_epipe_ports_subports, i);
        }

        /* [0..35] */
        /*  @MMU_THDO_PORTS = [0..35] */
        /*  @MMU_RQE_PORTS = [0..35]; */
        SOC_PBMP_CLEAR(kt_ports_0_35);
        for (i = 0; i <= 35; i++) {
            SOC_PBMP_PORT_ADD(kt_ports_0_35, i);
        }

        /* PFC_PORT_STS_REGS = [25..28] */
        SOC_PBMP_CLEAR(kt_ports_25_28);
        for (i = 25; i <= 28; i++) {
            SOC_PBMP_PORT_ADD(kt_ports_25_28, i);
        }
        /* @MMU_MTRI_PORTS = [1..35] */
        SOC_PBMP_CLEAR(kt_mmu_mtri_ports);
        for (i = 1; i <= 35; i++) {
            SOC_PBMP_PORT_ADD(kt_mmu_mtri_ports, i);
        }
                
        /* All ports */
        SOC_PBMP_CLEAR(kt_all_ports); 
        SOC_PBMP_ASSIGN(kt_all_ports, PBMP_ALL(unit)); 
 
        kt_port_init = 1; 
    }


    if (!SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;
    }

    si = &SOC_INFO(unit);

    switch (ainfo->reg) {
        /* IPIPE_HIGIG_PORTS = [0,25,26,27,28,29,30,31] */
        case IUNHGIr:
        case ICTRLr:
        case IBCASTr:
        case ILTOMCr:
        case IIPMCr:
        case IUNKOPCr:
            pbmp = &kt_ipipe_higig_ports;
            break;

        /* EPIPE_PORTS_SUBPORTS = [0..38] */
        case EGR_VLAN_CONTROL_1r:
        case EGR_IPMC_CFG2r:
        case EGR_PORT_TO_NHI_MAPPINGr:
        case EGR_VLAN_CONTROL_2r:
        case EGR_VLAN_CONTROL_3r:
        case EGR_PVLAN_EPORT_CONTROLr:
        case EGR_INGRESS_PORT_TPID_SELECTr:
        case EGR_VLAN_LOGICAL_TO_PHYSICAL_MAPPINGr:
        case EGR_DBGr:
        case EGR_MODMAP_CTRLr:
        case EGR_SF_SRC_MODID_CHECKr:
        case EGR_SHAPING_CONTROLr:
        case EGR_COUNTER_CONTROLr:
        case EGR_MTUr:
        case EGR_PORT_1r:
        case EGR_1588_INGRESS_CTRLr:
        case EGR_1588_EGRESS_CTRLr:
        case EGR_1588_LINK_DELAYr:
        case EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr:
        case TDBGC0r:
        case TDBGC1r:
        case TDBGC2r:
        case TDBGC3r:
        case TDBGC4r:
        case TDBGC5r:
        case TDBGC6r:
        case TDBGC7r:
        case TDBGC8r:
        case TDBGC9r:
        case TDBGC10r:
        case TDBGC11r:
            pbmp = &kt_epipe_ports_subports;
            break;

        /* [0..35] */
        case DEQ_EFIFO_CFGr:
        case DEQ_EFIFO_STATUS_DEBUGr:
        case DEQ_EFIFO_EMPTY_FULL_STATUS_DEBUGr:
        case DEQ_EFIFO_WATERMARK_DEBUGr:
        case MMU_TO_XPORT_BKPr:
        case PORT_MAX_SHARED_CELLr:
        case THDIEMA_PORT_MAX_SHARED_CELLr:
        case THDIEXT_PORT_MAX_SHARED_CELLr:
        case THDIQEN_PORT_MAX_SHARED_CELLr:
        case THDIRQE_PORT_MAX_SHARED_CELLr:
        case PORT_MAX_PKT_SIZEr:
        case THDIEMA_PORT_MAX_PKT_SIZEr:
        case THDIEXT_PORT_MAX_PKT_SIZEr:
        case THDIQEN_PORT_MAX_PKT_SIZEr:
        case THDIRQE_PORT_MAX_PKT_SIZEr:
        case PORT_RESUME_LIMIT_CELLr:
        case THDIEMA_PORT_RESUME_LIMIT_CELLr:
        case THDIEXT_PORT_RESUME_LIMIT_CELLr:
        case THDIQEN_PORT_RESUME_LIMIT_CELLr:
        case THDIRQE_PORT_RESUME_LIMIT_CELLr:
        case PORT_PG_SPIDr:
        case THDIEMA_PORT_PG_SPIDr:
        case THDIEXT_PORT_PG_SPIDr:
        case THDIQEN_PORT_PG_SPIDr:
        case THDIRQE_PORT_PG_SPIDr:
        case PG_PORT_MIN_COUNT_CELLr:
        case THDIEMA_PG_PORT_MIN_COUNT_CELLr:
        case THDIEXT_PG_PORT_MIN_COUNT_CELLr:
        case THDIQEN_PG_PORT_MIN_COUNT_CELLr:
        case THDIRQE_PG_PORT_MIN_COUNT_CELLr:
        case PORT_FC_STATUSr:
        case THDIEMA_PORT_FC_STATUSr:
        case THDIEXT_PORT_FC_STATUSr:
        case THDIQEN_PORT_FC_STATUSr:
        case THDIRQE_PORT_FC_STATUSr:
        case FLOW_CONTROL_XOFF_STATEr:
        case THDIEMA_FLOW_CONTROL_XOFF_STATEr:
        case THDIEXT_FLOW_CONTROL_XOFF_STATEr:
        case THDIQEN_FLOW_CONTROL_XOFF_STATEr:
        case THDIRQE_FLOW_CONTROL_XOFF_STATEr:
        case TOQ_EG_CREDITr:
        case TOQ_PORT_BW_CTRLr:
        case E2EFC_PORT_MAPPINGr:
            pbmp = &kt_ports_0_35;
            break;

        case PG_SHARED_LIMIT_CELLr:
        case THDIEMA_PG_SHARED_LIMIT_CELLr:
        case THDIEXT_PG_SHARED_LIMIT_CELLr:
        case THDIQEN_PG_SHARED_LIMIT_CELLr:
        case THDIRQE_PG_SHARED_LIMIT_CELLr:
        case PG_RESET_OFFSET_CELLr:
        case THDIEMA_PG_RESET_OFFSET_CELLr:
        case THDIEXT_PG_RESET_OFFSET_CELLr:
        case THDIQEN_PG_RESET_OFFSET_CELLr:
        case THDIRQE_PG_RESET_OFFSET_CELLr:
        case PG_RESET_FLOOR_CELLr:
        case THDIEMA_PG_RESET_FLOOR_CELLr:
        case THDIEXT_PG_RESET_FLOOR_CELLr:
        case THDIQEN_PG_RESET_FLOOR_CELLr:
        case THDIRQE_PG_RESET_FLOOR_CELLr:
        case PG_MIN_CELLr:
        case THDIEMA_PG_MIN_CELLr:
        case THDIEXT_PG_MIN_CELLr:
        case THDIQEN_PG_MIN_CELLr:
        case THDIRQE_PG_MIN_CELLr:
        case PG_HDRM_LIMIT_CELLr:
        case THDIEMA_PG_HDRM_LIMIT_CELLr:
        case THDIEXT_PG_HDRM_LIMIT_CELLr:
        case THDIQEN_PG_HDRM_LIMIT_CELLr:
        case THDIRQE_PG_HDRM_LIMIT_CELLr:
        case PG_MIN_COUNT_CELLr:
        case THDIEMA_PG_MIN_COUNT_CELLr:
        case THDIEXT_PG_MIN_COUNT_CELLr:
        case THDIQEN_PG_MIN_COUNT_CELLr:
        case THDIRQE_PG_MIN_COUNT_CELLr:
        case PG_SHARED_COUNT_CELLr:
        case THDIEMA_PG_SHARED_COUNT_CELLr:
        case THDIEXT_PG_SHARED_COUNT_CELLr:
        case THDIQEN_PG_SHARED_COUNT_CELLr:
        case THDIRQE_PG_SHARED_COUNT_CELLr:
        case PG_HDRM_COUNT_CELLr:
        case THDIEMA_PG_HDRM_COUNT_CELLr:
        case THDIEXT_PG_HDRM_COUNT_CELLr:
        case THDIQEN_PG_HDRM_COUNT_CELLr:
        case THDIRQE_PG_HDRM_COUNT_CELLr:
        case PG_GBL_HDRM_COUNTr:
        case THDIEMA_PG_GBL_HDRM_COUNTr:
        case THDIEXT_PG_GBL_HDRM_COUNTr:
        case THDIQEN_PG_GBL_HDRM_COUNTr:
        case THDIRQE_PG_GBL_HDRM_COUNTr:
        case PG_RESET_VALUE_CELLr:
        case THDIEMA_PG_RESET_VALUE_CELLr:
        case THDIEXT_PG_RESET_VALUE_CELLr:
        case THDIQEN_PG_RESET_VALUE_CELLr:
        case THDIRQE_PG_RESET_VALUE_CELLr:
            if (ainfo->idx == 0) {
                pbmp = &kt_ports_0_35;
            } else {
                pbmp = &kt_ports_25_28;
            }
            break;

        /* PFC_PORT_STS/CFG_REGS = [25..28] */
        case XPORT_TO_MMU_BKPr:
        case PORT_LLFC_CFGr:
        case INTFI_PORT_CFGr:
        case PORT_MIN_CELLr:
        case THDIEMA_PORT_MIN_CELLr:
        case THDIEXT_PORT_MIN_CELLr:
        case THDIQEN_PORT_MIN_CELLr:
        case THDIRQE_PORT_MIN_CELLr:
        case PORT_PRI_GRP0r:
        case THDIEMA_PORT_PRI_GRP0r:
        case THDIEXT_PORT_PRI_GRP0r:
        case THDIQEN_PORT_PRI_GRP0r:
        case THDIRQE_PORT_PRI_GRP0r:
        case PORT_PRI_GRP1r:
        case THDIEMA_PORT_PRI_GRP1r:
        case THDIEXT_PORT_PRI_GRP1r:
        case THDIQEN_PORT_PRI_GRP1r:
        case THDIRQE_PORT_PRI_GRP1r:
        case PORT_PRI_XON_ENABLEr:
        case THDIEMA_PORT_PRI_XON_ENABLEr:
        case THDIEXT_PORT_PRI_XON_ENABLEr:
        case THDIQEN_PORT_PRI_XON_ENABLEr:
        case THDIRQE_PORT_PRI_XON_ENABLEr:
        case PORT_COUNT_CELLr:
        case THDIEMA_PORT_COUNT_CELLr:
        case THDIEXT_PORT_COUNT_CELLr:
        case THDIQEN_PORT_COUNT_CELLr:
        case THDIRQE_PORT_COUNT_CELLr:
        case PORT_MIN_COUNT_CELLr:
        case THDIEMA_PORT_MIN_COUNT_CELLr:
        case THDIEXT_PORT_MIN_COUNT_CELLr:
        case THDIQEN_PORT_MIN_COUNT_CELLr:
        case THDIRQE_PORT_MIN_COUNT_CELLr:
        case PORT_SHARED_COUNT_CELLr:
        case THDIEMA_PORT_SHARED_COUNT_CELLr:
        case THDIEXT_PORT_SHARED_COUNT_CELLr:
        case THDIQEN_PORT_SHARED_COUNT_CELLr:
        case THDIRQE_PORT_SHARED_COUNT_CELLr:
        case PORT_SHARED_MAX_PG_ENABLEr:
        case THDIEMA_PORT_SHARED_MAX_PG_ENABLEr:
        case THDIEXT_PORT_SHARED_MAX_PG_ENABLEr:
        case THDIQEN_PORT_SHARED_MAX_PG_ENABLEr:
        case THDIRQE_PORT_SHARED_MAX_PG_ENABLEr:
        case PORT_MIN_PG_ENABLEr:
        case THDIEMA_PORT_MIN_PG_ENABLEr:
        case THDIEXT_PORT_MIN_PG_ENABLEr:
        case THDIQEN_PORT_MIN_PG_ENABLEr:
        case THDIRQE_PORT_MIN_PG_ENABLEr:
            pbmp = &kt_ports_25_28;
            break;

        /* @MMU_MTRI_PORTS = [1..35] */
        case BKPMETERINGCONFIG_64r:
        case BKPMETERINGBUCKETr:
        case MTRI_IFGr:
            pbmp = &kt_mmu_mtri_ports;
            break;

        /*  @MMU_THDO_PORTS = [0..35] */
        case OP_E2ECC_PORT_CONFIGr:
            pbmp = &kt_ports_0_35;
            break;

        /*  @MMU_RQE_PORTS = [0..35]; */
        case RQE_PORT_CONFIGr:
        case RQE_MIRROR_CONFIGr:
            pbmp = &kt_ports_0_35;
            break;

        case MAC_PFC_CTRLr:
            /* These are not implemented in Katana */
            goto skip;

        default:
            pbmp = &kt_all_ports;
            break;
    }

    if ((soc_portreg == SOC_REG_INFO(unit, ainfo->reg).regtype) &&
        (!SOC_PBMP_MEMBER(*pbmp, ainfo->port))) {
        goto skip; 
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case SHAPING_CONTROLr:
        case RESET_ON_EMPTY_MAX_64r:
            if (!si->port_num_ext_cosq[ainfo->port]) {
                COMPILER_64_SET(temp_mask, 0, 0x007fffff);
                COMPILER_64_AND(*mask, temp_mask);
            }
            break;
        default:
            break;
        }
    }

    return 0;

skip:
    /* set mask to all 0's so test will skip it */
    if (mask != NULL) {
        COMPILER_64_SET(*mask, 0, 0);
    }
    return 1;
}
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
STATIC int
reg_mask_subset_katanax(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    soc_info_t *si;
    uint64 temp_mask;
    uint16 dev_id;
    uint8  rev_id;
    int rv = 0;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, ainfo->port, ainfo->idx)) {
        /* set mask to all 0's so test will skip it */
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    /* Validate block accesses against variant features */
    if (CHECK_FEATURE_BLK(soc_feature_ces, SOC_BLK_CES) ||
        CHECK_FEATURE_BLK(soc_feature_ddr3, SOC_BLK_CI)) {
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case SHAPING_CONTROLr:
        case RESET_ON_EMPTY_MAX_64r:
            if (!si->port_num_ext_cosq[ainfo->port]) {
                COMPILER_64_SET(temp_mask, 0, 0x007fffff);
                COMPILER_64_AND(*mask, temp_mask);
            }
            break;
        case CFAPIFULLRESETPOINTr:
        case CFAPEFULLRESETPOINTr:
        case CFAPIFULLSETPOINTr:
        case CFAPEFULLSETPOINTr:
        case MAC_PFC_CTRLr:
        case CFAPICONFIGr:
        case CHFC_TC2PRI_TBL_ECC_CONFIGr:
        case MMU_INTFO_CONGST_STr:
        case RXLP_DFC_CPU_UPDATE_REFRESHr:
        case RXLP_DFC_STATUS_HIr:
        case RXLP_DFC_STATUS_LOr:
        case THDI_TO_OOBFC_SP_STr:
        case THDO_TO_OOBFC_SP_STr:
        case CFAPIOTPCONFIGr:
            COMPILER_64_SET(*mask, 0, 0);
            rv = 1;
            break;
        case CFAPECONFIGr:
        case CFAPEOTPCONFIGr:
        case EMC_CFGr:
        case EMC_CSDB_MEM_DEBUGr:
        case EMC_ECC_DEBUG_0r:
        case EMC_ECC_DEBUG_1r:
        case EMC_ERRB_0_MEM_DEBUGr:
        case EMC_ERRB_1_MEM_DEBUGr:
        case EMC_ERRB_FIFO_NFULL_THRESHOLDr:
        case EMC_ERROR_MASK_0r:
        case EMC_ERROR_MASK_1r:
        case EMC_ERROR_MASK_2r:
        case EMC_ERROR_MASK_3r:
        case EMC_EWRB_0_MEM_DEBUGr:
        case EMC_EWRB_1_MEM_DEBUGr:
        case EMC_EWRB_FIFO_NFULL_THRESHOLDr:
        case EMC_FREE_POOL_SIZESr:
        case EMC_IRRB_THRESHOLDSr:
        case EMC_IWRB_MEM_DEBUGr:
        case EMC_IWRB_SIZEr:
        case EMC_RFCQ_MEM_DEBUGr:
        case EMC_RSFP_MEM_DEBUGr:
        case EMC_SWAT_MEM_DEBUGr:
        case EMC_WCMT_MEM_DEBUGr:
        case EMC_WLCT_MEM_DEBUGr:
        case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_MEM_DEBUGr:
        case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_NFULL_THRESHOLDr:
        case EMC_WTFP_MEM_DEBUGr:
        case EMC_WTOQ_MEM_DEBUGr:
        case EMC_WTOQ_BUFFER_ECC_STATUS_DEBUGr:
        case GLOBAL_WRED_AVG_QSIZE_BUFFERE_POOL0r:
        case GLOBAL_WRED_AVG_QSIZE_BUFFERE_POOL1r:
        case GLOBAL_WRED_AVG_QSIZE_BUFFERIr:
        case GLOBAL_WRED_AVG_QSIZE_FRACTION_BUFFERE_POOL0r:
        case GLOBAL_WRED_AVG_QSIZE_FRACTION_BUFFERE_POOL1r:
        case GLOBAL_WRED_AVG_QSIZE_FRACTION_BUFFERIr:
        case GLOBAL_WRED_AVG_QSIZE_FRACTION_QENTRYr:
        case GLOBAL_WRED_AVG_QSIZE_QENTRYr:
        case GLOBAL_WRED_CONFIG_BUFFERE_POOL0r:
        case GLOBAL_WRED_CONFIG_BUFFERE_POOL1r:
        case GLOBAL_WRED_CONFIG_BUFFERIr:
        case GLOBAL_WRED_CONFIG_QENTRYr:
        case GLOBAL_WRED_DROP_THD_NONTCP_BUFFERE_POOL0r:
        case GLOBAL_WRED_DROP_THD_NONTCP_BUFFERE_POOL1r:
        case GLOBAL_WRED_DROP_THD_NONTCP_BUFFERIr:
        case GLOBAL_WRED_DROP_THD_NONTCP_QENTRYr:
        case GLOBAL_WRED_DROP_THD_TCP_BUFFERE_POOL0r:
        case GLOBAL_WRED_DROP_THD_TCP_BUFFERE_POOL1r:
        case GLOBAL_WRED_DROP_THD_TCP_BUFFERIr:
        case GLOBAL_WRED_DROP_THD_TCP_QENTRYr:
        case HCFC_ERRr:
        case INTFI_CFGr:
        case INTFI_DEBUGr:
        case INTFI_ECC_CONTROLr:
        case INTFI_ECC_INTR_MASKr:
        case INTFI_MAP_L0_TBL_ADDRr:
        case INTFI_MAP_L1_TBL_ADDRr:
        case INTFI_MAP_L2_TBL_ADDRr:
        case INTFI_OOBFC_MSG_RX_FAILED_CNTr:
        case INTFO_FCN_ENr:
        case INTFO_HW_UPDATE_DISr:
        case INTFO_OOBFC_MSG_RX_FAILED_CNTr:
        case ITE_CONFIGr:
        case ITE_SCHED_WRR_WEIGHT_COS0r:
        case ITE_SCHED_WRR_WEIGHT_COS1r:
        case ITE_SCHED_WRR_WEIGHT_COS2r:
        case ITE_SCHED_WRR_WEIGHT_COS3r:
        case ITE_SCHED_WRR_WEIGHT_COS4r:
        case ITE_SCHED_WRR_WEIGHT_COS5r:
        case ITE_SCHED_WRR_WEIGHT_COS6r:
        case ITE_SCHED_WRR_WEIGHT_COS7r:
        case MMU1_PLL_DEBUGr:
        case MMU2_PLL_DEBUGr:
        case MMU_ADM_ECC_COUNTERSr:
        case MMU_ADM_ECC_DEBUG_0r:
        case MMU_ADM_ECC_ERROR_0_MASKr:
        case MMU_ADM_QUEUE_DB_DEBUGr:
        case MMU_E2EFC_CNT_DEBUG_STATUS_0r:
        case MMU_E2EFC_CNT_DEBUG_STATUS_1r:
        case MMU_E2EFC_ERROR_0_MASKr:
        case MMU_ENQ_CBP_STORE_DEBUG_0r:
        case MMU_ENQ_CFAPI_INTERNAL_RECYCLE_DEBUG_0r:
        case MMU_ENQ_CFG_ECC_DEBUG_0r:
        case MMU_ENQ_CFG_ECC_ERROR_0_MASKr:
        case MMU_ENQ_CONFIG_0r:
        case MMU_ENQ_DEST_PPP_CFG_0r:
        case MMU_ENQ_DEST_PPP_CFG_1r:
        case MMU_ENQ_DEST_PPP_CFG_2r:
        case MMU_ENQ_DEST_PPP_CFG_3r:
        case MMU_ENQ_DEST_PPP_CFG_4r:
        case MMU_ENQ_DEST_PPP_CFG_5r:
        case MMU_ENQ_ECC_COUNTERSr:
        case MMU_ENQ_EMA_COS_TO_FIFO_LOOKUP_0r:
        case MMU_ENQ_EMA_COS_TO_FIFO_LOOKUP_2r:
        case MMU_ENQ_EMA_COS_TO_FIFO_LOOKUP_3r:
        case MMU_ENQ_EMA_COS_TO_FIFO_LOOKUP_1r:
        case MMU_ENQ_EMA_COS_TO_FIFO_PROFILE_1r:
        case MMU_ENQ_EMA_COS_TO_FIFO_PROFILE_2r:
        case MMU_ENQ_EMA_QUEUE_SELECT_0r:
        case MMU_ENQ_EMA_QUEUE_SELECT_1r:
        case MMU_ENQ_ERROR_0_MASKr:
        case MMU_ENQ_FAPCONFIG_0r:
        case MMU_ENQ_FAPINIT_0r:
        case MMU_ENQ_FAP_ECC_DEBUG_0r:
        case MMU_ENQ_FAP_ECC_ERROR_0_MASKr:
        case MMU_ENQ_ILLEGAL_CELL_TYPE_0r:
        case MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r:
        case MMU_ENQ_ITE_REORDER_DEBUG_0r:
        case MMU_ENQ_MISSING_START_ERR_STAT_0r:
        case MMU_ENQ_MISSING_START_ERR_STAT_1r:
        case MMU_ENQ_PACKING_REAS_FIFO_PROFILE_THRESHr:
        case MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_0r:
        case MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_1r:
        case MMU_ENQ_PACKING_REAS_FIFO_THRESH_PROFILE_2r:
        case MMU_ENQ_PACK_PKT_LEN_FIFO_DEBUG_0r:
        case MMU_ENQ_PACK_SRC_CTXT_FIFO_DEBUG_0r:
        case MMU_ENQ_PROFILE_0_PRI_GRP0r:
        case MMU_ENQ_PROFILE_0_PRI_GRP1r:
        case MMU_ENQ_PROFILE_1_PRI_GRP0r:
        case MMU_ENQ_PROFILE_1_PRI_GRP1r:
        case MMU_ENQ_PROFILE_2_PRI_GRP0r:
        case MMU_ENQ_PROFILE_2_PRI_GRP1r:
        case MMU_ENQ_PROFILE_3_PRI_GRP0r:
        case MMU_ENQ_PROFILE_3_PRI_GRP1r:
        case MMU_ENQ_RQE_QUEUE_SELECT_0r:
        case MMU_ENQ_RQE_QUEUE_SELECT_1r:
        case MMU_ENQ_RQE_WR_COMPLETE_DEBUG_0r:
        case MMU_ENQ_SOP_STORE_DEBUG_0r:
        case MMU_ENQ_SRC_PORT_STATE_DEBUG_0r:
        case MMU_ENQ_START_BY_START_ERR_STAT_0r:
        case MMU_ENQ_START_BY_START_ERR_STAT_1r:
        case MMU_ENQ_TRACE_IF_CONTROLr:
        case MMU_IPCTR_CONFIG_0r:
        case MMU_IPCTR_COUNTER1_SNAPr:
        case MMU_IPCTR_ECC_COUNTERSr:
        case MMU_IPCTR_ECC_DEBUG_0r:
        case MMU_IPCTR_ECC_ERROR_0_MASKr:
        case MMU_ITE_CFG_ECC_DEBUG_0r:
        case TOP_MISC_CONTROL_0r:
             if (!soc_feature(unit, soc_feature_ddr3) ||
                              (SOC_DDR3_NUM_MEMORIES(unit) == 0)) {
                 COMPILER_64_SET(*mask, 0, 0);
                 rv = 1;
              }
              break;
        default:
            break;
        }
    }

    return rv;
}
#endif /* BCM_KATANAX_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
STATIC int
reg_mask_subset_saber2(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    int rv = 0;
#if 0
    soc_info_t *si;
    si = &SOC_INFO(unit);
#endif

   if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, ainfo->port, ainfo->idx)) {
      /* set mask to all 0's so test will skip it */
       if (mask != NULL) {
           COMPILER_64_SET(*mask, 0, 0);
       }
       return 1;
   }


    /* Validate block accesses against variant features */
    if (CHECK_FEATURE_BLK(soc_feature_ddr3, SOC_BLK_CI)) {
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    if (!soc_feature(unit,soc_feature_ddr3)) {
        switch(ainfo->reg) {
        case CI0_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
        case CI0_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
        case CI1_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
        case CI1_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
        case EMC_CFGr:
        case EMC_BUFFER_EMPTY_FULL_STATUS_DEBUGr:
        case EMC_CI_FULL_STATUS_DEBUGr:
        case EMC_CSDB_0_BUFFER_PAR_STATUS_DEBUGr:
        case EMC_CSDB_1_BUFFER_PAR_STATUS_DEBUGr:
        case EMC_CSDB_MEM_DEBUGr:
        case EMC_ECC_DEBUG_0r:
        case EMC_ECC_DEBUG_1r:
        case EMC_ERRORr:
        case EMC_ERROR_0r:
        case EMC_ERROR_MASK_0r:
        case EMC_ERROR_1r:
        case EMC_ERROR_MASK_1r:
        case EMC_ERROR_2r:
        case EMC_ERROR_MASK_2r:
        case EMC_ERROR_3r:
        case EMC_ERROR_MASK_3r:
        case EMC_FIXED_PATTERN_DEBUGr:
        case EMC_FREE_POOL_SIZESr:
        case EMC_GLOBAL_1B_ECC_ERROR_COUNT_DEBUGr:
        case EMC_GLOBAL_2B_ECC_ERROR_COUNT_DEBUGr:
        case EMC_IRRB_BUFFER_ECC_STATUS_DEBUGr:
        case EMC_IRRB_BUFFER_FILL_LEVEL_DEBUGr:
        case EMC_IRRB_BUFFER_WATERMARK_DEBUGr:
        case EMC_IRRB_MEM_DEBUGr:
        case EMC_IRRB_THRESHOLDSr:
        case EMC_RFCQ_BUFFER_ECC_STATUS_DEBUGr:
	case EMC_RFCQ_BUFFER_FILL_LEVEL_DEBUGr:
	case EMC_RFCQ_BUFFER_WATERMARK_DEBUGr: 
        case EMC_RFCQ_MEM_DEBUGr: 
  	case EMC_RSFP_BUFFER_ECC_STATUS_DEBUGr: 
	case EMC_RSFP_BUFFER_FILL_LEVEL_DEBUGr: 
	case EMC_RSFP_MEM_DEBUGr: 
        case EMC_SWAT_BUFFER_ECC_STATUS_DEBUGr: 
  	case EMC_SWAT_MEM_DEBUGr: 
	case EMC_TO_CI0_RD_REQ_COUNT_DEBUGr: 
  	case EMC_TO_CI0_WR_REQ_COUNT_DEBUGr: 
	case EMC_TO_CI1_RD_REQ_COUNT_DEBUGr: 
  	case EMC_TO_CI1_WR_REQ_COUNT_DEBUGr: 
  	case EMC_TO_ITE_WR_REQ_PKT_COMPLETE_COUNT_DEBUGr: 
  	case EMC_WLCT_BUFFER_ECC_STATUS_DEBUGr: 
	case EMC_WLCT_MEM_DEBUGr: 
  	case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr: 
  	case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_FILL_LEVEL_DEBUGr: 
  	case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_MEM_DEBUGr: 
  	case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_NFULL_THRESHOLDr: 
  	case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_WATERMARK_DEBUGr: 
  	case EMC_WTFP_BUFFER_ECC_STATUS_DEBUGr: 
	case EMC_WTFP_BUFFER_FILL_LEVEL_DEBUGr: 
  	case EMC_WTFP_MEM_DEBUGr: 
	case EMC_WTOQ_BUFFER_ECC_STATUS_DEBUGr: 
  	case EMC_WTOQ_BUFFER_FILL_LEVEL_DEBUGr: 
	case EMC_WTOQ_MEM_DEBUGr: 
  	case ITE_TO_EMC_WR_REQ_EOP_COUNT_DEBUGr: 
	case ITE_TO_EMC_WR_REQ_SOP_COUNT_DEBUGr: 
  	case MMU_ITE_EMC_BACKPRESSURE_DEBUG_CONFIGr: 
  	case MMU_ITE_EMC_BACKPRESSURE_DEBUG_STATUSr:
             if (mask != NULL) {
                 COMPILER_64_SET(*mask, 0, 0);
             }
             return 1;
        }
    }
    
    if (mask != NULL) {
        switch (ainfo->reg) {
#if 0 
        case AVS_REG_PVT_MNTR_CONFIG_PVT_MNTR_SW_RESETBr:
#endif
/* Shouldn't touch Controlling registers :-) */
	case CMIC_XGXS0_PLL_CONTROL_1r:
	case CMIC_XGXS0_PLL_CONTROL_2r:
	case CMIC_XGXS0_PLL_CONTROL_3r:
	case CMIC_XGXS0_PLL_CONTROL_4r:
	case CMIC_XGXS1_PLL_CONTROL_1r:
	case CMIC_XGXS1_PLL_CONTROL_2r:
	case CMIC_XGXS1_PLL_CONTROL_3r:
	case CMIC_XGXS1_PLL_CONTROL_4r:
	case CMIC_XGXS2_PLL_CONTROL_1r:
	case CMIC_XGXS2_PLL_CONTROL_2r:
	case CMIC_XGXS2_PLL_CONTROL_3r:
	case CMIC_XGXS2_PLL_CONTROL_4r:
	case CMIC_XGXS3_PLL_CONTROL_1r:
	case CMIC_XGXS3_PLL_CONTROL_2r:
	case CMIC_XGXS3_PLL_CONTROL_3r:
	case CMIC_XGXS3_PLL_CONTROL_4r:
	case CMIC_MISC_CONTROLr:
	case CMIC_SBUS_RING_MAP_0_7r:
	case CMIC_SBUS_RING_MAP_8_15r:
	case CMIC_SBUS_RING_MAP_16_23r:
	case CMIC_SBUS_RING_MAP_24_31r:
	case TOP_SOFT_RESET_REGr:
	case TOP_SOFT_RESET_REG_2r:
	case TOP_SOFT_RESET_REG_3r:
	case TOP_XGXS0_PLL_STATUSr:
	case TOP_XGXS1_PLL_STATUSr:
	case TOP_MISC_CONTROL_0r:
	case TOP_MISC_CONTROL_1r:
	case TOP_MISC_CONTROL_2r:
	case TOP_MISC_CONTROL_3r:
	case TOP_TIME_SYNC_PLL_STATUSr:
	case TOP_BROAD_SYNC0_PLL_STATUSr:
	case TOP_BROAD_SYNC1_PLL_STATUSr:
	case TOP_CORE_PLL0_CTRL_REGISTER_0r:
	case TOP_CORE_PLL0_CTRL_REGISTER_1r:
	case TOP_CORE_PLL0_CTRL_REGISTER_2r:
	case TOP_CORE_PLL0_CTRL_REGISTER_3r:
	case TOP_CORE_PLL0_CTRL_REGISTER_4r:
	case TOP_CORE_PLL0_CTRL_REGISTER_5r:
	case TOP_TIME_SYNC_PLL_CTRL_REGISTER_1r:
	case TOP_TIME_SYNC_PLL_CTRL_REGISTER_2r:
	case TOP_TIME_SYNC_PLL_CTRL_REGISTER_3r:
	case TOP_TIME_SYNC_PLL_CTRL_REGISTER_4r:
	case TOP_TIME_SYNC_PLL_CTRL_REGISTER_5r:
	case TOP_XGXS0_PLL_CONTROL_1r:
	case TOP_XGXS0_PLL_CONTROL_2r:
	case TOP_XGXS0_PLL_CONTROL_3r:
	case TOP_XGXS0_PLL_CONTROL_4r:
	case TOP_XGXS0_PLL_CONTROL_5r:
	case TOP_XGXS0_PLL_CONTROL_6r:
	case TOP_XGXS0_PLL_CONTROL_7r:
	case TOP_XGXS0_PLL_CONTROL_8r:
	case TOP_XGXS1_PLL_CONTROL_1r:
	case TOP_XGXS1_PLL_CONTROL_2r:
	case TOP_XGXS1_PLL_CONTROL_3r:
	case TOP_XGXS1_PLL_CONTROL_4r:
	case TOP_XGXS1_PLL_CONTROL_5r:
	case TOP_XGXS1_PLL_CONTROL_6r:
	case TOP_XGXS1_PLL_CONTROL_7r:
	case TOP_XGXS1_PLL_CONTROL_8r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_0r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_1r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_2r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_3r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_4r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_5r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_6r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_7r:
	case IPROC_WRAP_IPROC_DDR_PLL_CTRL_8r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_0r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_2r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_4r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_5r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_6r:
	case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_7r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_0r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_2r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_4r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_5r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_6r:
	case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_7r:
	case CMIC_SBUS_TIMEOUTr:
	case TOP_MMU_DDR_PHY_CTRLr:
	case TOP_CI_PHY_CONTROLr:
	case XPORT_XGXS_CTRLr:
	case CI_RESETr:
	case EGR_HW_RESET_CONTROL_1r:
	case ING_HW_RESET_CONTROL_2r:
	case IP0_INTR_STATUSr:

	/* Special Registers */
	case CFAPEFULLRESETPOINTr:
	case CFAPEFULLSETPOINTr:
	case CFAPICONFIGr:
	case CFAPIFULLRESETPOINTr:
	case CFAPIFULLSETPOINTr:
	case CHFC_TC2PRI_TBL_ECC_CONFIGr:
	case MMU_INTFO_CONGST_STr:
	case OAMP_ERROR_INITIATION_DATAr:
	case OAMP_INDIRECT_COMMANDr:
	case OAMP_INDIRECT_COMMAND_WR_DATAr:
	case OAMP_INTERRUPT_REGISTER_TESTr:
	case OAMP_SAT_RX_STATr:
	case PARITY_ERROR_STATUS_0r:
	case THDIEMA_PARITY_ERROR_STATUS_0r:
	case THDIEXT_PARITY_ERROR_STATUS_0r:
	case THDIQEN_PARITY_ERROR_STATUS_0r:
	case THDIRQE_PARITY_ERROR_STATUS_0r:
	case THDI_TO_OOBFC_SP_STr:
	case THDO_TO_OOBFC_SP_STr:
	case RXLP_DFC_CPU_UPDATE_REFRESHr:
	case RXLP_DFC_STATUS_HIr:
	case RXLP_DFC_STATUS_LOr:

 
	case BKPMETERINGBUCKETr:
	case BKPMETERINGCONFIG_64r:
	case MTRI_IFGr:
	case OP_E2ECC_PORT_CONFIGr:
                 COMPILER_64_SET(*mask, 0, 0);
                 rv = 1;
                 break;
        default:
             /* Cannot play with AVS block related registers :-). 
                Better to ignore as system might retaliate */
             if (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_AVS)) {
                 COMPILER_64_SET(*mask, 0, 0);
                 rv = 1;
                 break;
             }
             break;
        }
    }
    return rv;
}
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_HURRICANE2_SUPPORT
STATIC int
reg_mask_subset_hurricane2(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, ainfo->port, ainfo->idx)) {
        /* set mask to all 0's so test will skip it */
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r:
        case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r:
        case TOP_BS_PLL0_CTRL_0r:
        case TOP_BS_PLL0_CTRL_1r:
        case TOP_BS_PLL1_CTRL_0r:
        case TOP_BS_PLL1_CTRL_1r:
        case TOP_TS_PLL_CTRL_4r:
            COMPILER_64_SET(*mask, 0, 0);
            return 1;
        default:
            break;
        }
    }

    return 0;
}
#endif /* BCM_HURRICANE2_SUPPORT */

#ifdef BCM_GREYHOUND_SUPPORT
STATIC int
reg_mask_subset_greyhound(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    int rv = 0;

    if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, ainfo->port, ainfo->idx)) {
        /* set mask to all 0's so test will skip it */
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r:
        case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r:
        case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r:
        case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r:
        case TOP_XG0_LCPLL_FBDIV_CTRL_0r:
        case TOP_XG0_LCPLL_FBDIV_CTRL_1r:
        case TOP_XG1_LCPLL_FBDIV_CTRL_0r:
        case TOP_XG1_LCPLL_FBDIV_CTRL_1r:
            COMPILER_64_SET(*mask, 0, 0);
            rv = 1;
            break;
        default:
            break;
        }
    }

    return rv;
}
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_HURRICANE3_SUPPORT
STATIC int
reg_mask_subset_hurricane3(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
    if (!SOC_REG_PORT_IDX_VALID(unit,ainfo->reg, ainfo->port, ainfo->idx)) {
        /* set mask to all 0's so test will skip it */
        if (mask != NULL) {
            COMPILER_64_SET(*mask, 0, 0);
        }
        return 1;
    }

    if (mask != NULL) {
        switch (ainfo->reg) {
        case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r:
        case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r:
        case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r:
        case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r:
        case TOP_BS_PLL0_CTRL_0r:
        case TOP_BS_PLL0_CTRL_1r:
        case TOP_BS_PLL1_CTRL_0r:
        case TOP_BS_PLL1_CTRL_1r:
        case TOP_TS_PLL_CTRL_4r:
        case TOP_XG0_LCPLL_FBDIV_CTRL_0r:
        case TOP_XG0_LCPLL_FBDIV_CTRL_1r:
        case TOP_XG1_LCPLL_FBDIV_CTRL_0r:
        case TOP_XG1_LCPLL_FBDIV_CTRL_1r:
        case AVS_REG_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr:
        case AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROLr:
                /* 
                 * The bit 27 of register, AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROL,
                 * is a DONE bit. The "NOTEST" should be added for this register in the regprofile.
                 * Once the regfile is updated, this register should be removed from here.
                 */
            COMPILER_64_SET(*mask, 0, 0);
            return 1;
        default:
            break;
        }
    }

    return 0;
}
#endif /* BCM_HURRICANE3_SUPPORT */


/*
 * Function: reg_mask_subset
 * 
 * Description: 
 *   Returns 1 if the register is not implemented in HW for the 
 *   specified port/cos. Returns 0 if the register is implemented
 *   in HW. If "mask" argument is supplied, the mask is modified
 *   to only include bits that are implemented in HW for the specified
 *   port/cos index.
 */
int
reg_mask_subset(int unit, soc_regaddrinfo_t *ainfo, uint64 *mask)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        return (reg_mask_subset_tr2(unit, ainfo, mask));
    }
#endif
#ifdef BCM_HURRICANE1_SUPPORT
    if (SOC_IS_HURRICANE(unit)) {
        return (reg_mask_subset_hu(unit, ainfo, mask));
    }
#endif
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        return (reg_mask_subset_en(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        return (reg_mask_subset_tr3(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return (reg_mask_subset_tomahawk3(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return (reg_mask_subset_tomahawk(unit, ainfo, mask));
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        return (reg_mask_subset_apache(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        return (reg_mask_subset_trident2(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return (reg_mask_subset_trident(unit, ainfo, mask));
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        return (reg_mask_subset_saber2(unit, ainfo, mask));
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        return (reg_mask_subset_katanax(unit, ainfo, mask));
    }
#endif
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return (reg_mask_subset_katanax(unit, ainfo, mask));
    }
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        return (reg_mask_subset_hurricane2(unit, ainfo, mask));
    }
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)||SOC_IS_GREYHOUND2(unit)) {
        return (reg_mask_subset_hurricane3(unit, ainfo, mask));
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        return (reg_mask_subset_greyhound(unit, ainfo, mask));
    }
#endif
#ifdef BCM_TRIUMPH_SUPPORT
    /* This case is a sort of 'catch-All" for triumph Family. So
     * Make sure this comes after others like Hu, En, Kt, Kt2 etc */
    if (SOC_IS_TR_VL(unit)) {
        return (reg_mask_subset_tr(unit, ainfo, mask));
    }
#endif
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        return (reg_mask_subset_sh(unit, ainfo, mask));
    }
#endif
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        return (reg_mask_subset_sc(unit, ainfo, mask));
    }
#endif
    return 0;
}

/* Some registers need to be skipped only in reset value test,
 * -Not- in RW test. So, we can't use the NOTEST flag. List those regs here
 * For RW test, use the reg_mask_subset
 */
int
rval_test_skip_reg(int unit, soc_regaddrinfo_t *ainfo, reg_data_t *rd)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
#endif

#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        switch (ainfo->reg) {
            case MTRO_PORT_L0_MAPPINGr:
            case Q_SCHED_PORT_EMPTY_STATUS_SPLIT0r:
            case Q_SCHED_PORT_EMPTY_STATUS_SPLIT1r:
                return 1;   /* Skip these registers */
            default:
                break;
        }
        if (_soc_fb6_reg_skip(unit, ainfo->reg)) {
            return 1;
        }
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        switch (ainfo->reg) {
            case TOP_MISC_STATUSr:
            case TOP_SWITCH_FEATURE_ENABLE_1r:
            case TOP_SWITCH_FEATURE_ENABLE_2r:
            case TOP_SWITCH_FEATURE_ENABLE_4r:
            case TOP_THERMAL_PVTMON_RESULT_0r:
            case TOP_THERMAL_PVTMON_RESULT_1r:
            case TOP_THERMAL_PVTMON_RESULT_2r:
            case TOP_THERMAL_PVTMON_RESULT_3r:
            case TOP_XGXS_MDIO_CONFIG_0r:
            case TOP_XGXS_MDIO_CONFIG_1r:
            case TOP_XGXS_MDIO_CONFIG_2r:
            case TOP_XGXS_MDIO_CONFIG_3r:
            case TOP_MMU_PLL1_CTRL1r:
            case TOP_MMU_PLL1_CTRL3r:
            case TOP_MMU_PLL2_CTRL1r:
            case TOP_MMU_PLL2_CTRL3r:
            case TOP_MMU_PLL3_CTRL1r:
            case TOP_MMU_PLL3_CTRL3r:
            case TOP_MMU_PLL_STATUS0r:
            case TOP_MMU_PLL_STATUS1r:
            case QUAD0_SERDES_STATUS0r:
            case QUAD0_SERDES_STATUS1r:
            case QUAD1_SERDES_STATUS0r:
            case QUAD0_SERDES_CTRLr:
            case QUAD1_SERDES_CTRLr:
            case MAC_MODEr:
            case OAM_SEC_NS_COUNTER_64r:
            case MMU_ITE_DEBUG_STATUS_0r:
            case TOP_MISC_CONTROL_2r:
            case XPORT_XGXS_NEWCTL_REGr:
            case CI0_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI0_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI1_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI1_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI2_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI2_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case EMC_BUFFER_EMPTY_FULL_STATUS_DEBUGr:
            case EMC_CFGr:
            case EMC_CI_FULL_STATUS_DEBUGr:
            case EMC_CSDB_0_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_1_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_2_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_MEM_DEBUGr:
            case EMC_ECC_DEBUGr:
            case EMC_ERRB_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_ERRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_ERRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_ERRB_MEM_DEBUGr:
            case EMC_ERRORr:
            case EMC_ERROR_0r:
            case EMC_ERROR_1r:
            case EMC_ERROR_2r:
            case EMC_ERROR_MASK_0r:
            case EMC_ERROR_MASK_1r:
            case EMC_ERROR_MASK_2r:
            case EMC_EWRB_BUFFER_0_ECC_STATUS_DEBUGr:
            case EMC_EWRB_BUFFER_1_ECC_STATUS_DEBUGr:
            case EMC_EWRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_EWRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_EWRB_MEM_DEBUGr:
            case EMC_FREE_POOL_SIZESr:
            case EMC_GLOBAL_1B_ECC_ERROR_COUNT_DEBUGr:
            case EMC_GLOBAL_2B_ECC_ERROR_COUNT_DEBUGr:
            case EMC_IRRB_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_IRRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_IRRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_IRRB_THRESHOLDSr:
            case EMC_IWRB_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_IWRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_IWRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_IWRB_MEM_DEBUGr:
            case EMC_IWRB_SIZEr:
            case EMC_RFCQ_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_RFCQ_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_RFCQ_BUFFER_WATERMARK_DEBUGr:
            case EMC_RFCQ_MEM_DEBUGr:
            case EMC_RSFP_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_RSFP_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_RSFP_MEM_DEBUGr:
            case EMC_SWAT_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_SWAT_MEM_DEBUGr:
            case EMC_TO_CI0_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI0_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI1_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI1_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI2_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI2_WR_REQ_COUNT_DEBUGr:
            case EMC_WCMT_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WCMT_MEM_DEBUGr:
            case EMC_WLCT0_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT_MEM_DEBUGr:
            case EMC_WTFP_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WTFP_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_WTFP_MEM_DEBUGr:
            case EMC_WTOQ_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WTOQ_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_WTOQ_MEM_DEBUGr:
            case TOP_SWITCH_FEATURE_ENABLE_3r:
            case QSTRUCT_FAPOTPCONFIG_0r:
            case QSTRUCT_FAPOTPCONFIG_1r:
            case QSTRUCT_FAPOTPCONFIG_2r:
            case QSTRUCT_FAPOTPCONFIG_3r:
            case QSTRUCT_FAPCONFIG_0r:
            case QSTRUCT_FAPCONFIG_1r:
            case QSTRUCT_FAPCONFIG_2r:
            case QSTRUCT_FAPCONFIG_3r:
            case CFAPEOTPCONFIGr:
                return 1;   /* Skip these registers */
            default:
                return 0;
        }
    }
#endif 
#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit)) {
        switch (ainfo->reg) {
            case OAMP_GLOBAL_SYS_HEADER_CFGr:
            case OAMP_INDIRECT_WR_MASKr:
            case OAMP_REG_0086r:
            case OAMP_REG_0090r:
            case OAMP_REG_0091r:
            case OAMP_REG_00ADr:
#ifdef BCM_METROLITE_SUPPORT
            case OAMP_PE_CONSTr:
            case OAMP_PE_PTRr:
            case XLPORT_XGXS_COUNTER_MODEr:
#endif
                if (SAL_BOOT_QUICKTURN) {
#ifdef SABER2_DEBUG
                    LOG_WARN(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit, "Skipping register %s\n"), 
                             SOC_REG_NAME(unit, ainfo->reg)));
#endif
                    return 1;
                } else {
                    return 0;
                }
            case TOP_CI_PHY_CONTROLr:
            case AVS_REG_HW_MNTR_LAST_MEASURED_SENSORr:
            case AVS_REG_PMB_SLAVE_BPCM_IDr:
            case AVS_REG_RO_REGISTERS_0_CEN_ROSC_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_0P85V_0_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_0P85V_1_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_1P8V_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_1V_1_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_3P3V_MNTR_STATUSr:
            case AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr:
            case AVS_REG_TEMPERATURE_MONITOR_TEMPERATURE_MEASUREMENT_STATUSr:
            case AVS_REG_TOP_CTRL_VTRAP_STATUSr:
            case XLMAC_RX_LSS_STATUSr:
            case XLPORT_LED_CHAIN_CONFIGr:
            case CI_ERRORr:
            case CI_RESETr:
            case MMU_ITE_DEBUG_STATUS_0r:
            case TOP_SWITCH_FEATURE_ENABLE_0r:
            case TOP_SWITCH_FEATURE_ENABLE_1r:
            case TOP_SWITCH_FEATURE_ENABLE_2r:
            case TOP_SWITCH_FEATURE_ENABLE_3r:
            case TOP_SWITCH_FEATURE_ENABLE_4r:
            case TOP_SWITCH_FEATURE_ENABLE_5r:
            case TOP_SWITCH_FEATURE_ENABLE_6r:
            case TOP_SWITCH_FEATURE_ENABLE_7r:
            case TOP_XGXS_MDIO_CONFIG_0r:
            case TOP_PVTMON_RESULT_0r:
            case TOP_PVTMON_RESULT_1r:
            case OAMP_ECC_1B_ERR_CNTr:
            case OAMP_ECC_2B_ERR_CNTr:
            case OAMP_GTIMER_CONFIGURATIONr:
            case OAMP_INDIRECT_COMMANDr:
            case OAMP_REG_0087r:
            case OAMP_REG_00AEr:
            case TOP_CI_DDR_PHY_REG_CTRLr:
            case TOP_CI_DDR_PHY_REG_DATAr:
            case TOP_HW_TAP_MEM_ECC_CTRLr:
            case TOP_HW_TAP_MEM_ECC_STATUSr:
            case TOP_MISC_STATUS_0r:
            case TOP_MMU_DDR_PHY_CTRLr:
            case TOP_RESCAL_STATUS_0r:
            case TOP_UC_TAP_READ_DATAr:
            case TOP_XGXS0_PLL_CONTROL_3r:
            case TOP_XGXS0_PLL_CONTROL_4r:
            case TOP_XGXS1_PLL_CONTROL_3r:
            case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_3r:
            case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_3r:
            case OAMP_STAT_INTERRUPT_MESSAGEr: 
            case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r:
            case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r:
            case TOP_MASTER_LCPLL_FBDIV_CTRL_0r:
            case TOP_MASTER_LCPLL_FBDIV_CTRL_1r:
            case TOP_SERDES_LCPLL_FBDIV_CTRL_0r:
            case TOP_SERDES_LCPLL_FBDIV_CTRL_1r:
            case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r:
            case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r:
            case CFAPIOTPCONFIGr:
            case TOP_TAP_CONTROLr:

                return 1;  /* Skip these registers */
            default:
                if ((SAL_BOOT_QUICKTURN) && 
                        (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_AVS))) {
                    /* Skip AVS block registers */
#ifdef SABER2_DEBUG
                    LOG_WARN(BSL_LS_APPL_COMMON,
                            (BSL_META_U(unit, "Skipping register %s\n"), 
                             SOC_REG_NAME(unit, ainfo->reg)));
#endif
                    return 1;
                }
                return 0;
        }
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        uint16 dev_id;
        uint8  rev_id;
        soc_cm_get_id(unit, &dev_id, &rev_id);
        switch (ainfo->reg) {
            case TOP_HW_TAP_MEM_ECC_CTRLr:
            case TOP_MISC_CONTROL_1r:
            case TOP_MISC_STATUS_0r:
            case TOP_MMU_PLL_INITr:
            case TOP_MMU_PLL_STATUS0r:
            case TOP_MMU_PLL_STATUS1r:
            case TOP_PVTMON_RESULT_0r:
            case TOP_PVTMON_RESULT_1r:
            case TOP_PVTMON_RESULT_2r:
            case TOP_PVTMON_RESULT_3r:
            case TOP_SWITCH_FEATURE_ENABLE_0r:
            case TOP_SWITCH_FEATURE_ENABLE_1r:
            case TOP_SWITCH_FEATURE_ENABLE_2r:
            case TOP_SWITCH_FEATURE_ENABLE_3r:
            case TOP_SWITCH_FEATURE_ENABLE_4r:
            case TOP_SWITCH_FEATURE_ENABLE_5r:
            case TOP_SWITCH_FEATURE_ENABLE_6r:
            case TOP_SW_BOND_OVRD_CTRL0r:
            case TOP_SW_BOND_OVRD_CTRL1r:
            case TOP_UC_TAP_READ_DATAr:
            case MMU_ITE_DEBUG_STATUS_0r:
            case XMAC_RX_LSS_STATUSr:
            case TOP_MISC_CONTROL_2r:
            case TOP_XGXS_MDIO_CONFIG_0r:
            case TOP_XGXS_MDIO_CONFIG_1r:
            case TOP_XGXS_MDIO_CONFIG_2r:
            case TOP_XGXS_MDIO_CONFIG_3r:
            case TOP_MMU_PLL1_CTRL1r:
            case TOP_MMU_PLL1_CTRL3r:
            case TOP_MMU_PLL2_CTRL1r:
            case TOP_MMU_PLL2_CTRL3r:
            case TOP_MMU_PLL3_CTRL1r:
            case TOP_MMU_PLL3_CTRL3r:
            case XPORT_XGXS_NEWCTL_REGr:
            case TOP_TAP_CONTROLr:
            case TOP_MEM_PWR_DWN_BITMAP1_STATUSr:
            case TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r:
            case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_0r:
            case TOP_BROAD_SYNC0_PLL_CTRL_REGISTER_1r:
            case TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r:
            case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_0r:
            case TOP_BROAD_SYNC1_PLL_CTRL_REGISTER_1r:
                return 1;   /* Skip these registers */
            case CFAPECONFIGr:
            case CFAPEOTPCONFIGr:
            case CI0_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI0_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI1_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI1_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI2_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI2_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI3_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI3_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI4_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI4_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case CI5_TO_EMC_CELL_DATA_RETURN_COUNT_DEBUGr:
            case CI5_TO_EMC_WTAG_RETURN_COUNT_DEBUGr:
            case EMC_BUFFER_EMPTY_FULL_STATUS_DEBUGr:
            case EMC_CFGr:
            case EMC_CI_FULL_STATUS_DEBUGr:
            case EMC_CSDB_0_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_1_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_2_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_3_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_4_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_5_BUFFER_PAR_STATUS_DEBUGr:
            case EMC_CSDB_MEM_DEBUGr:
            case EMC_ECC_DEBUG_0r:
            case EMC_ECC_DEBUG_1r:
            case EMC_ERRB_0_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_ERRB_0_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_ERRB_0_BUFFER_WATERMARK_DEBUGr:
            case EMC_ERRB_0_MEM_DEBUGr:
            case EMC_ERRB_1_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_ERRB_1_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_ERRB_1_BUFFER_WATERMARK_DEBUGr:
            case EMC_ERRB_1_MEM_DEBUGr:
            case EMC_ERRB_FIFO_NFULL_THRESHOLDr:
            case EMC_ERRORr:
            case EMC_ERROR_0r:
            case EMC_ERROR_1r:
            case EMC_ERROR_2r:
            case EMC_ERROR_3r:
            case EMC_ERROR_MASK_0r:
            case EMC_ERROR_MASK_1r:
            case EMC_ERROR_MASK_2r:
            case EMC_ERROR_MASK_3r:
            case EMC_EWRB_0_BUFFER_0_ECC_STATUS_DEBUGr:
            case EMC_EWRB_0_BUFFER_1_ECC_STATUS_DEBUGr:
            case EMC_EWRB_0_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_EWRB_0_BUFFER_WATERMARK_DEBUGr:
            case EMC_EWRB_0_MEM_DEBUGr:
            case EMC_EWRB_1_BUFFER_0_ECC_STATUS_DEBUGr:
            case EMC_EWRB_1_BUFFER_1_ECC_STATUS_DEBUGr:
            case EMC_EWRB_1_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_EWRB_1_BUFFER_WATERMARK_DEBUGr:
            case EMC_EWRB_1_MEM_DEBUGr:
            case EMC_EWRB_FIFO_NFULL_THRESHOLDr:
            case EMC_FREE_POOL_SIZESr:
            case EMC_GLOBAL_1B_ECC_ERROR_COUNT_DEBUGr:
            case EMC_GLOBAL_2B_ECC_ERROR_COUNT_DEBUGr:
            case EMC_IRRB_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_IRRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_IRRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_IRRB_THRESHOLDSr:
            case EMC_IWRB_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_IWRB_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_IWRB_BUFFER_WATERMARK_DEBUGr:
            case EMC_IWRB_MEM_DEBUGr:
            case EMC_IWRB_SIZEr:
            case EMC_RFCQ_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_RFCQ_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_RFCQ_BUFFER_WATERMARK_DEBUGr:
            case EMC_RFCQ_MEM_DEBUGr:
            case EMC_RSFP_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_RSFP_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_RSFP_MEM_DEBUGr:
            case EMC_SWAT_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_SWAT_MEM_DEBUGr:
            case EMC_TO_CI0_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI0_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI1_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI1_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI2_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI2_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI3_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI3_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI4_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI4_WR_REQ_COUNT_DEBUGr:
            case EMC_TO_CI5_RD_REQ_COUNT_DEBUGr:
            case EMC_TO_CI5_WR_REQ_COUNT_DEBUGr:
            case EMC_WCMT_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WCMT_MEM_DEBUGr:
            case EMC_WLCT0_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT0_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT1_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_LOWER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_LOWER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_MERGED_RETURN_WTAG_FIFO_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_UPPER_A_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT2_UPPER_B_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WLCT_MEM_DEBUGr:
            case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_MEM_DEBUGr:
            case EMC_WLCT_MERGED_RETURN_WTAG_FIFO_NFULL_THRESHOLDr:
            case EMC_WTFP_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WTFP_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_WTFP_MEM_DEBUGr:
            case EMC_WTOQ_BUFFER_ECC_STATUS_DEBUGr:
            case EMC_WTOQ_BUFFER_FILL_LEVEL_DEBUGr:
            case EMC_WTOQ_MEM_DEBUGr:
            case EXT_BUFFER_POOL_CONG_STATEr:
            case FC_MAP_TBL2_ECC_ERR1r:
            case FC_MAP_TBL2_ECC_ERR2r:
            case GLOBAL_WRED_AVG_QSIZE_BUFFERE_POOL0r:
            case GLOBAL_WRED_AVG_QSIZE_BUFFERE_POOL1r:
            case GLOBAL_WRED_AVG_QSIZE_BUFFERIr:
            case GLOBAL_WRED_AVG_QSIZE_FRACTION_BUFFERE_POOL0r:
            case GLOBAL_WRED_AVG_QSIZE_FRACTION_BUFFERE_POOL1r:
            case TOP_MEM_PWR_DWN_BITMAP0_STATUSr:
            case QSTRUCT_FAPCONFIG_0r:
            case QSTRUCT_FAPCONFIG_1r:
            case QSTRUCT_FAPCONFIG_2r:
            case QSTRUCT_FAPCONFIG_3r:
            case QSTRUCT_FAPOTPCONFIG_0r:
            case QSTRUCT_FAPOTPCONFIG_1r:
            case QSTRUCT_FAPOTPCONFIG_2r:
            case QSTRUCT_FAPOTPCONFIG_3r:
            if ((dev_id == BCM56248_DEVICE_ID) ||
                (dev_id == BCM56456_DEVICE_ID) ||
                (dev_id == BCM55455_DEVICE_ID))
                return 1;
            default:
                return 0;
        }
    }
#endif
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        switch (ainfo->reg) {
            case TOP_MISC_CONTROL_3r:
            case CMICTXCOSMASKr:
            case MAC_MODEr:
            case TOP_UC_TAP_READ_DATAr:
            case XLMAC_RX_LSS_STATUSr:
                return 1;   /* Skip these registers */
            default:
                return 0;
        }
    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
    if(SOC_IS_GREYHOUND(unit)) {
        uint16 dev_id;
        uint8  rev_id;
        soc_cm_get_id(unit, &dev_id, &rev_id);
        switch (ainfo->reg) {
            case CMICTXCOSMASKr:
            case PGW_CTRL_0r:
            case XLPORT_LED_CHAIN_CONFIGr:
            case TOP_XG1_LCPLL_FBDIV_CTRL_1r:
            case TOP_XG_PLL0_CTRL_5r:
            case TOP_XG_PLL0_CTRL_6r:
            case TOP_XG_PLL0_CTRL_7r:
            case TOP_XG_PLL1_CTRL_1r:
            case TOP_XG_PLL1_CTRL_5r: 
            case TOP_XG_PLL1_CTRL_6r:
            case TOP_XG_PLL1_CTRL_7r:
            case TOP_BS_PLL0_CTRL_0r:
            case TOP_BS_PLL0_CTRL_1r:
            case TOP_BS_PLL0_CTRL_5r:
            case TOP_BS_PLL0_CTRL_6r:
            case TOP_BS_PLL0_CTRL_7r:
            case TOP_BS_PLL1_CTRL_0r:
            case TOP_BS_PLL1_CTRL_1r:
            case TOP_BS_PLL1_CTRL_5r:
            case TOP_BS_PLL1_CTRL_6r:
            case TOP_BS_PLL1_CTRL_7r:
            case SPEED_LIMIT_ENTRY0_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY1_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY2_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY3_PHYSICAL_PORT_NUMBERr:                
            case SPEED_LIMIT_ENTRY4_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY8_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY9_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY10_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY11_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY13_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY14_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY15_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY16_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY18_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY22_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY23_PHYSICAL_PORT_NUMBERr:
            case TOP_PVTMON_CTRL_0r:
            case AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr:
                return 1;
            case CFAPFULLTHRESHOLDr:
                if ((dev_id == BCM53422_DEVICE_ID) || (dev_id == BCM53424_DEVICE_ID) ||
                    (dev_id == BCM53426_DEVICE_ID)) {
                    return 1;
                }
                break;
            default:
                break;
        }
    }
#endif
#ifdef BCM_HURRICANE3_SUPPORT
    if(SOC_IS_HURRICANE3(unit)) {
        if (soc_feature(unit, soc_feature_wh2)) {
            switch (ainfo->reg) {
                case CMICTXCOSMASKr:
                case GPORT_SGMII0_CTRL_REGr:
                case GPORT_SGMII1_CTRL_REGr:
                case TOP_SOFT_RESET_REGr:
                case TOP_QGPHY_CTRL_0r:
                case TOP_SGMII_CTRL_REGr:
                case TOP_QGPHY_CTRL_2r:
                case TOP_XG_PLL0_CTRL_3r:
                case GPORT_XGXS0_CTRL_REGr:
                case TOP_BS_PLL0_CTRL_3r:
                case TOP_BS_PLL0_CTRL_5r:
                case TOP_BS_PLL0_CTRL_6r:
                case TOP_BS_PLL0_CTRL_7r:
                case TOP_XG_PLL0_CTRL_5r:
                case TOP_XG_PLL0_CTRL_6r:
                case TOP_XG_PLL0_CTRL_7r:
                    return 1;
                default:
                    break;
            }
        } else {
            uint16 dev_id;
            uint8  rev_id;
            soc_cm_get_id(unit, &dev_id, &rev_id);
            switch (ainfo->reg) {
                case CMICTXCOSMASKr:
                case PGW_CTRL_0r:
                case XLPORT_LED_CHAIN_CONFIGr:
                case TOP_BS_PLL0_CTRL_0r:
                case TOP_BS_PLL0_CTRL_1r:
                case TOP_BS_PLL0_CTRL_3r:
                case TOP_BS_PLL0_CTRL_5r:
                case TOP_BS_PLL0_CTRL_6r:
                case TOP_BS_PLL0_CTRL_7r:
                case TOP_BS_PLL1_CTRL_0r:
                case TOP_BS_PLL1_CTRL_1r:
                case TOP_BS_PLL1_CTRL_3r:
                case TOP_BS_PLL1_CTRL_5r:
                case TOP_BS_PLL1_CTRL_6r:
                case TOP_BS_PLL1_CTRL_7r:
                case TOP_XG1_LCPLL_FBDIV_CTRL_1r:
                case TOP_XG_PLL0_CTRL_1r:
                case TOP_XG_PLL0_CTRL_5r:
                case TOP_XG_PLL0_CTRL_6r:
                case TOP_XG_PLL0_CTRL_7r:
                case TOP_XG_PLL1_CTRL_1r:
                case TOP_XG_PLL1_CTRL_5r:
                case TOP_XG_PLL1_CTRL_6r:
                case TOP_XG_PLL1_CTRL_7r:
                case SPEED_LIMIT_ENTRY0_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY4_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY8_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY13_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY14_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY15_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY16_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY18_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY22_PHYSICAL_PORT_NUMBERr:
                case SPEED_LIMIT_ENTRY23_PHYSICAL_PORT_NUMBERr:
                case TOP_PVTMON_CTRL_0r:
                case AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr:
                case GPORT_XGXS0_CTRL_REGr:
                case TPCEr:
                    return 1;
                case CFAPFULLTHRESHOLDr:
                    if (dev_id == BCM53434_DEVICE_ID) {
                        return 1;
                    }
                    break;
                default:
                    break;
            }
        }
    }
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if(SOC_IS_GREYHOUND2(unit)) {
        uint16 dev_id;
        uint8  rev_id;
        soc_cm_get_id(unit, &dev_id, &rev_id);
        switch (ainfo->reg) {
            case CMICTXCOSMASKr:
            case PGW_CTRL_0r:
            case PGW_CL_LED_CHAIN_CONFIGr:
            case PGW_GE_LED_CHAIN_CONFIGr:
            case PGW_XL_LED_CHAIN_CONFIGr:
            case XLPORT_LED_CHAIN_CONFIGr:
            case CLPORT_LED_CHAIN_CONFIGr:
            case TOP_XG1_LCPLL_FBDIV_CTRL_1r:
            case TOP_XG_PLL0_CTRL_6r:
            case TOP_XG_PLL0_CTRL_3r:
            case TOP_XG_PLL0_CTRL_7r:
            case TOP_XG_PLL1_CTRL_1r:
            case TOP_XG_PLL1_CTRL_3r:
            case TOP_XG_PLL1_CTRL_6r:
            case SPEED_LIMIT_ENTRY0_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY4_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY8_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY13_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY14_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY15_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY16_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY18_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY22_PHYSICAL_PORT_NUMBERr:
            case SPEED_LIMIT_ENTRY23_PHYSICAL_PORT_NUMBERr:
            case TOP_PVTMON_CTRL_0r:
            case AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr:
            case GPORT_XGXS0_CTRL_REGr:
            case XLPORT_XGXS0_CTRL_REGr:
            case CLPORT_XGXS0_CTRL_REGr:
            case GPORT_SGMII0_CTRL_REGr:
            case GPORT_SGMII1_CTRL_REGr:
            case SR_RX_FLOW_ID_CONTROLr:
            case SR_TX_FLOW_ID_CONTROLr:
                return 1;
#ifdef BCM_FIRELIGHT_SUPPORT
            case CHIP_CONFIGr:
            case TOP_MACSEC_CTRLr:
            case MACSEC_CTRLr:
            case AVS_REG_TEMPERATURE_MONITOR_TEMPERATURE_INTERRUPT_THRESHOLDSr:
            case TOP_XG_PLL0_CTRL_8r:
                if (soc_feature(unit, soc_feature_fl)) {
                    return 1;
                }
                break;
#endif /* BCM_FIRELIGHT_SUPPORT */
            default:
                break;
        }
    }
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int rv, is_filtered = 0;

        rv =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_reg_default_val_filter, (unit, ainfo->reg, &is_filtered));
        if (rv == BCM_E_NONE && is_filtered)
        {
            return 1; /* Skip these registers */
        }
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
		if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "AVS_") != NULL) {
			return 1; /* Skip broadcast registers (sw only registers)*/
		}
        switch(ainfo->reg) {
	   case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
	   case MESH_TOPOLOGY_RESERVED_MTCPr:

	   case ECI_BLOCKS_POWER_DOWNr:
	   case CMIC_CPS_RESETr:
	   case ECI_BLOCKS_SOFT_RESETr:
	   case ECI_BLOCKS_SOFT_INITr:
	   case ECI_BLOCKS_SBUS_RESETr:
	   case ECI_AUTO_DOC_NAME_20r:
	   case ECI_ECC_ERR_1B_MONITOR_MEM_MASKr:
	   case ECI_ECC_ERR_2B_MONITOR_MEM_MASKr:
	   case ECI_ECC_INTERRUPT_REGISTER_MASKr:
	   case ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case ECI_FAP_GLOBAL_GENERAL_CFG_2r:
	   case ECI_FE_GLOBAL_GENERAL_CFG_1r:
	   case ECI_INTERRUPT_MASK_REGISTERr:
       case ECI_INDIRECT_COMMAND_ADDRESSr:
       case ECI_TAP_CPU_INTERFACE_COMMANDr:
	   case FMAC_ASYNC_FIFO_CONFIGURATIONr:
	   case FMAC_RETIMERr:
	   /*In these registers the read value
		  will always be different than write value by design*/
	   case ECI_INDIRECT_COMMANDr:
	   case FSRD_INDIRECT_COMMANDr:
	   case RTP_INDIRECT_COMMANDr:
	   case DCL_INDIRECT_COMMANDr:
	   /*SBUS last in chain*/
	   case QRH_SBUS_BROADCAST_IDr:
	   case LCM_SBUS_BROADCAST_IDr:
	   case FSRD_SBUS_BROADCAST_IDr:
	   case CCH_SBUS_BROADCAST_IDr:
	   case FMAC_SBUS_BROADCAST_IDr:
	   case DCML_SBUS_BROADCAST_IDr:
	   case DCH_SBUS_BROADCAST_IDr:
	   case FMAC_SBUS_LAST_IN_CHAINr:
	   case FSRD_SBUS_LAST_IN_CHAINr:
	   case DCH_SBUS_LAST_IN_CHAINr:
	   case DCML_SBUS_LAST_IN_CHAINr:
	   case LCM_SBUS_LAST_IN_CHAINr:
	   case QRH_SBUS_LAST_IN_CHAINr:
	   case CCS_SBUS_LAST_IN_CHAINr:
	   case CCH_SBUS_LAST_IN_CHAINr:
	   case RTP_SBUS_LAST_IN_CHAINr:
	   case MESH_TOPOLOGY_SBUS_LAST_IN_CHAINr:
	   case ECI_SBUS_LAST_IN_CHAINr:
	   case OCCG_REG_0087r:
	   case BRDC_CCH_SBUS_BROADCAST_IDr:
	   case BRDC_DCML_SBUS_BROADCAST_IDr:
	   case BRDC_LCM_SBUS_BROADCAST_IDr:
	   case BRDC_QRH_SBUS_BROADCAST_IDr:
	   /*the following registers are filtered temporarily- the length of these registers should be amended according to the block they are in 
	   (need to be the longest line of a register that is RW instead of always 640 bit)*/
	   case DCL_INDIRECT_COMMAND_WR_DATAr:
	   case ECI_INDIRECT_COMMAND_WR_DATAr:
	   case FSRD_INDIRECT_COMMAND_WR_DATAr:
	   case RTP_INDIRECT_COMMAND_WR_DATAr:
	   /*the following registers should not exist in data base and therefore filtered*/
					
	   /*the following registers are filtered because they relate to Pll*/ 
	   case ECI_REG_0172r:
	   case ECI_AUTO_DOC_NAME_19r:
	   case ECI_REG_016Cr:
	   case ECI_PRM_PLL_CONTROL_STATUSr:
	   case RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case QRH_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case MCT_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case DCML_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case BRDC_DCML_ENABLE_DYNAMIC_MEMORY_ACCESSr:
	   case BRDC_QRH_ENABLE_DYNAMIC_MEMORY_ACCESSr:
       case ECI_ECIC_BLOCKS_RESETr:
       case ECI_MISC_PLL_0_CONFIGr:
       case ECI_MISC_PLL_1_CONFIGr:
       case ECI_MISC_PLL_2_CONFIGr:
       case ECI_MISC_PLL_3_CONFIGr:
       case FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr:
       case FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr:
       case FMAC_ECC_INTERRUPT_REGISTER_MASKr:
       case FMAC_FMAL_GENERAL_CONFIGURATIONr:
       case FMAC_FMAL_RX_GENERAL_CONFIGURATIONr:
       case FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr:
       case FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr:
       case FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr:
       case FMAC_FMAL_TX_GENERAL_CONFIGURATIONr:
       case FMAC_FPS_CONFIGURATION_RX_SYNCr:
       case FMAC_INTERRUPT_MASK_REGISTERr:
       case FMAC_LEAKY_BUCKET_CONTROL_REGISTERr:
       case FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr:
       case FMAC_PAR_ERR_MEM_MASKr:
       case FMAC_RECEIVE_RESET_REGISTERr:
       case FMAC_FE_GLOBAL_GENERAL_CFG_1r:
       case FMAC_RSF_CONFIGURATIONr:
       case FMAC_TX_LANE_SWAP_0r:
       case FMAC_TX_LANE_SWAP_1r:
       case FMAC_TX_LANE_SWAP_2r:
       case FMAC_TX_LANE_SWAP_3r:
       case DCML_INDIRECT_COMMANDr:
               return 1; /* Skip these registers */
            default:
                break;
		}
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "AN_") != NULL) {
            return 1; /* Skip broadcast registers (sw only registers)*/
        }
        if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "BRDC_") != NULL) {
            return 1; /* Skip broadcast registers (sw only registers)*/
        }
        if ((sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "FASIC_") != NULL) ||
            (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "FLEXEWP_") != NULL) ||
            (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "FSAR_") != NULL) ||
            (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "FSCL_") != NULL)) {
            return 1; /* Skip FlexE because not initilized registers (sw only registers)*/
        }

        switch(ainfo->reg) {

            /*SBUS last in chain*/
            case CDMAC_ECC_STATUSr:
            case CDPORT_INTR_STATUSr:
            case CDPORT_LINKSTATUS_DOWNr:
            case CDPORT_TSC_PLL_LOCK_STATUSr:
            case CDPORT_GENERAL_SPARE0_REGr:
            case CDPORT_GENERAL_SPARE1_REGr:
            case CDPORT_GENERAL_SPARE2_REGr:
            case CDPORT_SBUS_CONTROLr:
            case CDPORT_GENERAL_SPARE3_REGr:
            case CDPORT_INTR_ENABLEr:
            case CDPORT_MAC_CONTROLr:
            case CDPORT_MODE_REGr:
            case CDPORT_TSC_MEM_CTRLr:
            case CDPORT_XGXS0_CTRL_REGr:
            case CLPORT_EEE_DURATION_TIMER_PULSEr:
            case CLPORT_MAC_CONTROLr:
            case CLPORT_MODE_REGr:
            case CLPORT_SBUS_CONTROLr:
            case CLPORT_TXPI_LANE_SELECTIONr:
            case CLPORT_XGXS0_CTRL_REGr:
            case CLPORT_XGXS_COUNTER_MODEr:
            case DCC_DRAM_READ_FIFO_CNTRLr:
            case BDM_SBUS_BROADCAST_IDr:
            case CDUM_SBUS_BROADCAST_IDr:
            case CDU_SBUS_BROADCAST_IDr:
            case CFC_SBUS_BROADCAST_IDr:
            case CGM_SBUS_BROADCAST_IDr:
            case CRPS_SBUS_BROADCAST_IDr:
            case DDHA_SBUS_BROADCAST_IDr:
            case DDHB_SBUS_BROADCAST_IDr:
            case DDP_SBUS_BROADCAST_IDr:
            case DCC_SBUS_BROADCAST_IDr:
            case DHC_SBUS_BROADCAST_IDr:
            case DQM_SBUS_BROADCAST_IDr:
            case ECGM_SBUS_BROADCAST_IDr:
            case ECI_SBUS_BROADCAST_IDr:
            case EDB_SBUS_BROADCAST_IDr:
            case EPNI_SBUS_BROADCAST_IDr:
            case EPRE_SBUS_BROADCAST_IDr:
            case EPS_SBUS_BROADCAST_IDr:
            case ERPP_SBUS_BROADCAST_IDr:
            case ETPPA_SBUS_BROADCAST_IDr:
            case ETPPB_SBUS_BROADCAST_IDr:
            case ETPPC_SBUS_BROADCAST_IDr:
            case EVNT_SBUS_BROADCAST_IDr:
            case FCR_SBUS_BROADCAST_IDr:
            case FCT_SBUS_BROADCAST_IDr:
            case FDA_SBUS_BROADCAST_IDr:
            case FDR_SBUS_BROADCAST_IDr:
            case FDTL_SBUS_BROADCAST_IDr:
            case FDT_SBUS_BROADCAST_IDr:
            case FMAC_SBUS_BROADCAST_IDr:
            case FQP_SBUS_BROADCAST_IDr:
            case FSRD_SBUS_BROADCAST_IDr:
            case HBC_SBUS_BROADCAST_IDr:
            case HBMC_SBUS_BROADCAST_IDr:
            case ILE_SBUS_BROADCAST_IDr:
            case IPPA_SBUS_BROADCAST_IDr:
            case IPPB_SBUS_BROADCAST_IDr:
            case IPPC_SBUS_BROADCAST_IDr:
            case IPPD_SBUS_BROADCAST_IDr:
            case IPPE_SBUS_BROADCAST_IDr:
            case IPPF_SBUS_BROADCAST_IDr:
            case IPS_SBUS_BROADCAST_IDr:
            case IPT_SBUS_BROADCAST_IDr:
            case IQM_SBUS_BROADCAST_IDr:
            case IRE_SBUS_BROADCAST_IDr:
            case ITPPD_SBUS_BROADCAST_IDr:
            case ITPP_SBUS_BROADCAST_IDr:
            case KAPS_SBUS_BROADCAST_IDr:
            case MACT_SBUS_BROADCAST_IDr:
            case MCP_SBUS_BROADCAST_IDr:
            case MDB_SBUS_BROADCAST_IDr:
            case MESH_TOPOLOGY_SBUS_BROADCAST_IDr:
            case MRPS_SBUS_BROADCAST_IDr:
            case MTM_SBUS_BROADCAST_IDr:
            case NMG_SBUS_BROADCAST_IDr:
            case OAMP_SBUS_BROADCAST_IDr:
            case OCB_SBUS_BROADCAST_IDr:
            case OLP_SBUS_BROADCAST_IDr:
            case PDM_SBUS_BROADCAST_IDr:
            case PEM_SBUS_BROADCAST_IDr:
            case PQP_SBUS_BROADCAST_IDr:
            case RQP_SBUS_BROADCAST_IDr:
            case RTP_SBUS_BROADCAST_IDr:
            case SCH_SBUS_BROADCAST_IDr:
            case SIF_SBUS_BROADCAST_IDr:
            case SPB_SBUS_BROADCAST_IDr:
            case SQM_SBUS_BROADCAST_IDr:
            case TCAM_SBUS_BROADCAST_IDr:
            case TDU_SBUS_BROADCAST_IDr:
            /*suspects */
            case ECI_ECIC_MDIO_CONFIGr:
            case ECI_ECIC_MISC_RESETr:
            case ECI_BLOCKS_POWER_DOWNr:
            case ECI_BLOCKS_SBUS_RESETr:
            case ECI_BLOCKS_SOFT_INITr:
            case ECI_BLOCKS_SOFT_RESETr:
            case ECI_TAP_CPU_INTERFACE_COMMANDr:
            case ECI_TS_PLL_CONFIGURATIONSr:
            /* Skipping indirrect command registers */
            case CDU_INDIRECT_COMMANDr:
            case CDUM_INDIRECT_COMMANDr:
            case CFC_INDIRECT_COMMANDr:
            case CGM_INDIRECT_COMMANDr:
            case CRPS_INDIRECT_COMMANDr:
            case DDHA_INDIRECT_COMMANDr:
            case DDHB_INDIRECT_COMMANDr:
            case DHC_INDIRECT_COMMANDr:
            case DQM_INDIRECT_COMMANDr:
            case ECGM_INDIRECT_COMMANDr:
            case ECI_INDIRECT_COMMANDr:
            case EDB_INDIRECT_COMMANDr:
            case EPNI_INDIRECT_COMMANDr:
            case EPS_INDIRECT_COMMANDr:
            case ERPP_INDIRECT_COMMANDr:
            case ETPPA_INDIRECT_COMMANDr:
            case ETPPB_INDIRECT_COMMANDr:
            case ETPPC_INDIRECT_COMMANDr:
            case EVNT_INDIRECT_COMMANDr:
            case FCR_INDIRECT_COMMANDr:
            case FCT_INDIRECT_COMMANDr:
            case FDR_INDIRECT_COMMANDr:
            case FDT_INDIRECT_COMMANDr:
            case FQP_INDIRECT_COMMANDr:
            case FSRD_INDIRECT_COMMANDr:
            case HBC_INDIRECT_COMMANDr:
            case HBMC_INDIRECT_COMMANDr:
            case ILE_INDIRECT_COMMANDr:
            case IPPA_INDIRECT_COMMANDr:
            case IPPB_INDIRECT_COMMANDr:
            case IPPC_INDIRECT_COMMANDr:
            case IPPD_INDIRECT_COMMANDr:
            case IPPE_INDIRECT_COMMANDr:
            case IPPF_INDIRECT_COMMANDr:
            case IPS_INDIRECT_COMMANDr:
            case IPT_INDIRECT_COMMANDr:
            case IQM_INDIRECT_COMMANDr:
            case IRE_INDIRECT_COMMANDr:
            case ITPP_INDIRECT_COMMANDr:
            case ITPPD_INDIRECT_COMMANDr:
            case KAPS_INDIRECT_COMMANDr:
            case MACT_INDIRECT_COMMANDr:
            case MCP_INDIRECT_COMMANDr:
            case MDB_INDIRECT_COMMANDr:
            case MRPS_INDIRECT_COMMANDr:
            case MTM_INDIRECT_COMMANDr:
            case OAMP_INDIRECT_COMMANDr:
            case OCB_INDIRECT_COMMANDr:
            case OLP_INDIRECT_COMMANDr:
            case PEM_INDIRECT_COMMANDr:
            case PQP_INDIRECT_COMMANDr:
            case RQP_INDIRECT_COMMANDr:
            case RTP_INDIRECT_COMMANDr:
            case SCH_INDIRECT_COMMANDr:
            case SPB_INDIRECT_COMMANDr:
            case SQM_INDIRECT_COMMANDr:
            case TCAM_INDIRECT_COMMANDr:
            /* Read only register and driven by another registers */
            case MDB_ARM_KAPS_GLOBAL_EVENTr:
            case MDB_ARM_KAPS_IBC_COMMAND_WORDr:
            case MDB_ARM_KAPS_IBC_FIFO_CORRECTABLE_STATUSr:
            case MDB_ARM_KAPS_IBC_FIFO_UNCORRECTABLE_STATUSr:
            case MDB_ARM_KAPS_IBC_QUEUE_STATUSr:
            case MDB_ARM_KAPS_IBC_RESPONSE_DATAr:
            case MDB_ARM_KAPS_MEMORY_A_ERRORr:
            case MDB_ARM_KAPS_MEMORY_B_ERRORr:
            case MDB_ARM_KAPS_REVISIONr:
            case MDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_0r:
            case MDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_1r:
            case MDB_ARM_KAPS_R_5_CORE_MEM_CONTROLr:
            case MDB_ARM_KAPS_R_5_DAP_APB_CTRLr:
            case MDB_ARM_KAPS_R_5_DAP_APB_RDATAr:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUSr:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_0r:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_1r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_Ar:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_0r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_1r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_Ar:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_0r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_1r:
            case MDB_ARM_KAPS_R_5_FIFO_MON_EVENTr:
            case MDB_ARM_KAPS_SEARCH_0_A_ERRORr:
            case MDB_ARM_KAPS_SEARCH_0_B_ERRORr:
            case MDB_ARM_KAPS_SEARCH_1_A_ERRORr:
            case MDB_ARM_KAPS_SEARCH_1_B_ERRORr:
            case MDB_ARM_KAPS_STATUS_CFIFOr:
            case MDB_ARM_KAPS_STATUS_RFIFOr:
            case CDMAC_ECC_CTRLr:
            case CDMAC_LINK_INTR_CTRLr:
            case CDMAC_MEM_CTRLr:
            case CDMAC_MIB_COUNTER_MODEr:
            case CDMAC_MIB_COUNTER_PROG_RANGE_CNTR0r:
            case CDMAC_MIB_COUNTER_PROG_RANGE_CNTR1r:
            case CDMAC_MIB_COUNTER_PROG_RANGE_CNTR2r:
            case CDMAC_MIB_COUNTER_PROG_RANGE_CNTR3r:
            case ILKN_X4_CONTROL0_ILKN_CONTROL0r:
            case MAIN0_ECC_1B_ERR_INTERRUPT_ENr:
            case MAIN0_ECC_2B_ERR_INTERRUPT_ENr:
            case MAIN0_ECC_CORRUPT_CONTROL_0r:
            case MAIN0_ECC_CORRUPT_CONTROL_1r:
            case MAIN0_ECC_DISABLE_CONTROLr:
            case MAIN0_SETUPr:
            case MAIN0_TICK_CONTROL_0r:
            case MAIN0_TICK_CONTROL_1r:
            case MAIN0_TM_CONTROLr:
            case PKTGEN0_PCS_SEEDA0r:
            case PKTGEN0_PCS_SEEDA1r:
            case PKTGEN0_PCS_SEEDA2r:
            case PKTGEN0_PCS_SEEDA3r:
            case PKTGEN0_PCS_SEEDB0r:
            case PKTGEN0_PCS_SEEDB1r:
            case PKTGEN0_PCS_SEEDB2r:
            case PKTGEN0_PCS_SEEDB3r:
            case PKTGEN0_PKTGENCTRL1r:
            case PKTGEN0_PRTPCONTROLr:
            case PKTGEN1_ERRORMASK0r:
            case PKTGEN1_ERRORMASK1r:
            case PKTGEN1_ERRORMASK2r:
            case PKTGEN1_ERRORMASK3r:
            case PKTGEN1_ERRORMASK4r:
            case PKTGEN1_GLASTEST_CONTROLr:
            case PMD_X1_CONTROLr:
            case PMD_X1_FCLK_PERIODr:
            case PMD_X1_PM_TIMER_OFFSETr:
            case PMD_X4_CONTROLr:
            case PMD_X4_MODEr:
            case PMD_X4_OVERRIDEr:
            case PMD_X4_RX_FIXED_LATENCYr:
            case PMD_X4_TX_FIXED_LATENCYr:
            case PMD_X4_UI_VALUE_HIr:
            case PMD_X4_UI_VALUE_LOr:
            case RX_X1_CONTROL0_RS_FEC_CONFIGr:
            case RX_X1_CONTROL0_RX_LANE_SWAPr:
            case RX_X4_CONTROL0_DECODE_CONTROL_0r:
            case RX_X4_CONTROL0_PMA_CONTROL_0r:
            case RX_X4_CONTROL0_RS_FEC_RX_CONTROL_0r:
            case RX_X4_CONTROL0_RS_FEC_TIMERr:
            case RX_X4_CONTROL0_RX_TS_CONTROLr:
            case RX_X4_FEC_CONTROL_FEC_0r:
            case RX_X4_FEC_CONTROL_FEC_1r:
            case RX_X4_FEC_CONTROL_FEC_2r:
            case RX_X4_STATUS1_RL_MODE_SW_CONTROLr:
            case SC_X1_CONTROL_PIPELINE_RESET_COUNTr:
            case SC_X1_CONTROL_TX_RESET_COUNTr:
            case SC_X4_CONTROL_CONTROLr:
            case SC_X4_CONTROL_SPARE0r:
            case SC_X4_CONTROL_SW_SPARE1r:
            case SYNCE_X4_FRACTIONAL_DIVr:
            case SYNCE_X4_INTEGER_DIVr:
            case TX_X1_CONTROL0_GLAS_TPMA_CONTROLr:
            case TX_X1_CONTROL0_TX_LANE_SWAPr:
            case TX_X4_CONTROL0_MISCr:
            case TX_X4_CONTROL0_RS_SYMBOLr:
            case TX_X4_CONTROL0_TX_TS_CONTROLr:
            case TX_X4_CONTROL0_ERROR_CONTROLr:
            /* Skipped after consultin with ASIC */
            case IPT_SHAPER_STATUSr:
            case MACT_RESERVED_SPARE_0r:
            case MESH_TOPOLOGY_SPECIAL_CLOCK_CONTROLSr:
            case FMAC_GTIMER_CONFIGURATIONr:
            case FSRD_GTIMER_CONFIGURATIONr:
            case DCC_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case DPC_ENABLE_DYNAMIC_MEMORY_ACCESSr:

            /* Registers filled during init */

            case CDUM_INDIRECT_FORCE_BUBBLEr:
            case CDU_INDIRECT_FORCE_BUBBLEr:
            case CFC_INDIRECT_FORCE_BUBBLEr:
            case CGM_INDIRECT_FORCE_BUBBLEr:
            case CRPS_INDIRECT_FORCE_BUBBLEr:
            case DDHA_INDIRECT_FORCE_BUBBLEr:
            case DDHA_INDIRECT_WR_MASKr:
            case DDHB_INDIRECT_FORCE_BUBBLEr:
            case DDHB_INDIRECT_WR_MASKr:
            case DHC_INDIRECT_FORCE_BUBBLEr:
            case DHC_INDIRECT_WR_MASKr:
            case DQM_INDIRECT_FORCE_BUBBLEr:
            case ECGM_INDIRECT_FORCE_BUBBLEr:
            case ECI_FAB_0_PLL_CONFIGURATIONSr:
            case ECI_FAB_1_PLL_CONFIGURATIONSr:
            case ECI_INDIRECT_FORCE_BUBBLEr:
            case ECI_NIF_0_PLL_CONFIGURATIONSr:
            case ECI_NIF_1_PLL_CONFIGURATIONSr:
            case ECI_PIR_CONFIGURATIONSr:
            case ECI_RESERVED_20r:
            case ECI_RESERVED_22r:
            case ECI_SBUS_LAST_IN_CHAINr:
            case EDB_INDIRECT_FORCE_BUBBLEr:
            case EPNI_INDIRECT_FORCE_BUBBLEr:
            case EPS_INDIRECT_FORCE_BUBBLEr:
            case ERPP_INDIRECT_FORCE_BUBBLEr:
            case ERPP_INDIRECT_WR_MASKr:
            case ETPPA_INDIRECT_FORCE_BUBBLEr:
            case ETPPB_INDIRECT_FORCE_BUBBLEr:
            case ETPPC_INDIRECT_FORCE_BUBBLEr:
            case EVNT_INDIRECT_FORCE_BUBBLEr:
            case FCR_INDIRECT_FORCE_BUBBLEr:
            case FCT_INDIRECT_FORCE_BUBBLEr:
            case FDR_INDIRECT_FORCE_BUBBLEr:
            case FDT_INDIRECT_FORCE_BUBBLEr:
            case FMAC_RETIMERr:
            case FMAC_SBUS_LAST_IN_CHAINr:
            case FQP_INDIRECT_FORCE_BUBBLEr:
            case FSRD_INDIRECT_FORCE_BUBBLEr:
            case FSRD_SBUS_LAST_IN_CHAINr:
            case HBC_DATA_SOURCE_CONFIGr:
            case HBC_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case HBC_HBM_COLUMN_ACCESS_TIMINGSr:
            case HBC_HBM_CONFIGr:
            case HBC_HBM_CPU_CONTROLr:
            case HBC_HBM_MODE_REGISTERSr:
            case HBC_HBM_MODE_REGISTER_MANAGER_CONFIGr:
            case HBC_HBM_REFRESH_INTERVALSr:
            case HBC_HBM_REFRESH_TIMINGSr:
            case HBC_HBM_ROW_ACCESS_TIMINGSr:
            case HBC_INDIRECT_COMMAND_ADDRESSr:
            case HBC_INDIRECT_COMMAND_WR_DATAr:
            case HBC_PIPELINES_CONFIGr:
            case HBC_SBUS_LAST_IN_CHAINr:
            case HBC_TSM_CONFIGr:
            case HBMC_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case HBMC_HBM_PHY_CHANNEL_CONTROLr:
            case HBMC_HBM_PHY_CONTROLr:
            case HBMC_HBM_PLL_CONFIGr:
            case HBMC_HBM_RESET_CONTROLr:
            case HBMC_INDIRECT_COMMAND_ADDRESSr:
            case HBMC_INDIRECT_COMMAND_WR_DATAr:
            case HBMC_INDIRECT_FORCE_BUBBLEr:
            case ILE_INDIRECT_FORCE_BUBBLEr:
            case IPPA_INDIRECT_FORCE_BUBBLEr:
            case IPPA_INDIRECT_WR_MASKr:
            case IPPB_FLPACS_TCAM_PROTECTION_MACHINE_CONFIGURATIONr:
            case IPPB_FLPBCS_TCAM_PROTECTION_MACHINE_CONFIGURATIONr:
            case IPPB_INDIRECT_FORCE_BUBBLEr:
            case IPPB_INDIRECT_WR_MASKr:
            case IPPC_INDIRECT_FORCE_BUBBLEr:
            case IPPC_INDIRECT_WR_MASKr:
            case IPPC_SPECIAL_CLOCK_CONTROLSr:
            case IPPD_INDIRECT_FORCE_BUBBLEr:
            case IPPD_SPECIAL_CLOCK_CONTROLSr:
            case IPPE_INDIRECT_FORCE_BUBBLEr:
            case IPPF_INDIRECT_FORCE_BUBBLEr:
            case IPS_INDIRECT_FORCE_BUBBLEr:
            case IPT_FLOW_CONTROL_DEBUGr:
            case IPT_INDIRECT_FORCE_BUBBLEr:
            case IQM_INDIRECT_FORCE_BUBBLEr:
            case IRE_INDIRECT_FORCE_BUBBLEr:
            case ITPPD_INDIRECT_FORCE_BUBBLEr:
            case ITPP_INDIRECT_FORCE_BUBBLEr:
            case KAPS_INDIRECT_FORCE_BUBBLEr:
            case MACT_INDIRECT_FORCE_BUBBLEr:
            case MCP_INDIRECT_FORCE_BUBBLEr:
            case MDB_INDIRECT_FORCE_BUBBLEr:
            case MRPS_INDIRECT_FORCE_BUBBLEr:
            case MTM_INDIRECT_FORCE_BUBBLEr:
            case OAMP_INDIRECT_FORCE_BUBBLEr:
            case OCB_INDIRECT_FORCE_BUBBLEr:
            case OLP_INDIRECT_FORCE_BUBBLEr:
            case PEM_INDIRECT_FORCE_BUBBLEr:
            case PQP_INDIRECT_FORCE_BUBBLEr:
            case RQP_INDIRECT_FORCE_BUBBLEr:
            case RTP_INDIRECT_FORCE_BUBBLEr:
            case SCH_INDIRECT_FORCE_BUBBLEr:
            case SPB_INDIRECT_FORCE_BUBBLEr:
            case SQM_INDIRECT_FORCE_BUBBLEr:
            case TCAM_INDIRECT_FORCE_BUBBLEr:
            case TDU_ADDRESS_TRANSLATION_CONFIGr:
            case TDU_ADDRESS_TRANSLATION_MATRIXr:
            case TDU_DRAM_BLOCKED_CONFIGr:
            case TDU_READ_DATA_PATH_CONFIGr:
            case TDU_READ_REQUEST_PATH_CONFIGr:
            case TDU_SBUS_LAST_IN_CHAINr:
            case TDU_WRITE_REQUEST_PATH_CONFIGr:
            case SCH_INDIRECT_COMMAND_ADDRESSr:
            case SCH_DVS_NIF_CONFIGr:
            case SCH_DVS_CONFIGr:

            /* Control registers that are making configurations */
            case CGM_INVALID_CUDr:
            case ECI_FAB_PLL_CONFIGURATIONSr:
            case ECI_TOP_LEVEL_SAMPLING_CFGr:
            case DPC_DRAM_PHY_CONTROLr:
            case DPC_DRAM_PLL_CONFIGr:
            case ECI_FLEXE_PLL_CONFIGURATIONSr:
            case ECI_NIF_PLL_CONFIGURATIONSr:
               return 1; /* Skip these registers */
            default:
                break;
		}
    }
#endif

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit) && !SOC_IS_JERICHO(unit)) {
        switch(ainfo->reg) {
            case ECI_VERSION_REGISTERr:
               return 1; /* Skip these registers */
            default:
                break;
        }
    }
    if(SOC_IS_ARDON(unit)) {
        switch(ainfo->reg) {
            case ECI_AVSr:
            case FMAC_ASYNC_FIFO_CONFIGURATIONr:
            case EDB_ESEM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr:
            case EPNI_ESEM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr:
               return 1; /* Skip these registers */
            default:
                break;
        }
    }
    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        switch(ainfo->reg) {
       
        /* clk time changed - Witing for MCM release*/
        case CFC_GTIMER_CONFIGURATIONr:
        case DRCA_GTIMER_CONFIGURATIONr:
        case DRCB_GTIMER_CONFIGURATIONr:
        case DRCC_GTIMER_CONFIGURATIONr:
        case DRCD_GTIMER_CONFIGURATIONr:
        case DRCE_GTIMER_CONFIGURATIONr:
        case DRCF_GTIMER_CONFIGURATIONr:
        case DRCG_GTIMER_CONFIGURATIONr:
        case DRCH_GTIMER_CONFIGURATIONr:
        case OAMP_GTIMER_CONFIGURATIONr:

        /* we configure these on init */
            /* Broadcast */
        case CGM_SBUS_BROADCAST_IDr:
        case EGQ_SBUS_BROADCAST_IDr:
        case IHB_SBUS_BROADCAST_IDr:
        case IHP_SBUS_BROADCAST_IDr:
        case IPS_SBUS_BROADCAST_IDr:
        case IQM_SBUS_BROADCAST_IDr:
        case EPNI_SBUS_BROADCAST_IDr:
        case FSRD_SBUS_BROADCAST_IDr:
        case SCH_SBUS_BROADCAST_IDr:
        case KAPS_BBS_SBUS_BROADCAST_IDr:
        case FMAC_SBUS_BROADCAST_IDr:
            /* Last in chain */
        case EPNI_SBUS_LAST_IN_CHAINr:
        case ECI_SBUS_LAST_IN_CHAINr: 
        case EGQ_SBUS_LAST_IN_CHAINr:
        case KAPS_BBS_SBUS_LAST_IN_CHAINr:
        case CGM_SBUS_LAST_IN_CHAINr:
        case FMAC_SBUS_LAST_IN_CHAINr:
        case IHB_SBUS_LAST_IN_CHAINr:
        case IHP_SBUS_LAST_IN_CHAINr:
        case IPS_SBUS_LAST_IN_CHAINr:
        case IQM_SBUS_LAST_IN_CHAINr:
        case SCH_SBUS_LAST_IN_CHAINr:
        case DRCH_REG_0087r:

        /* INDIRECT_COMMAND register is a special register and there is no meaning for the "default value". */
        case CFC_INDIRECT_COMMANDr:
        case OAMP_INDIRECT_COMMANDr:

        case EPNI_INDIRECT_WR_MASKr:
        case EPNI_ECC_ERR_2B_MONITOR_MEM_MASKr:
        case FCR_ECC_ERR_1B_MONITOR_MEM_MASKr:
        case FCR_ECC_ERR_2B_MONITOR_MEM_MASKr:
        case FDA_ECC_ERR_1B_MONITOR_MEM_MASKr:
        case FDA_ECC_ERR_2B_MONITOR_MEM_MASKr:
        case KAPS_BBS_INDIRECT_WR_MASKr:
        case KAPS_BBS_KAPS_BB_ERROR_REGISTER_MASKr:
        case MESH_TOPOLOGY_GLOBAL_GENERAL_CFG_1r:
        case ECI_BLOCKS_SBUS_RESETr:
        case ECI_BLOCKS_SOFT_RESETr:
        case EDB_INDIRECT_WR_MASKr:
        case IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr:
       
        /* read error */
        case EGQ_INDIRECT_WR_MASKr:
        /* default value should be 0 */
        case FDT_ECC_ERR_1B_MONITOR_MEM_MASKr:
        case FDT_ECC_ERR_2B_MONITOR_MEM_MASKr:
            return 1; /* Skip these registers */

            default:
                break;
        }
    }
    if(SOC_IS_JERICHO(unit)) {
        if (ainfo->reg<rd->start_from || ainfo->reg > rd->start_from + rd->count ) {
            return 1;
        }
        if (!(rd->flags & REGTEST_FLAG_INC_PORT_MACROS)) {
            soc_block_types_t regblktype = SOC_REG_INFO(unit, ainfo->reg).block; 
            if (blocks_can_be_disabled(regblktype)) {
                return 1;
            }
        }

        switch(ainfo->reg) {

            case EPNI_CFG_LINK_FIFOS_FIXED_LATENCY_SETTINGr:
            case ECI_CM_PLL_CONFIGr:
            case ECI_UC_PLL_CONFIGr:
            case ECI_NIF_PMH_PLL_CONFIGr:
            case ECI_NIF_PML_0_PLL_CONFIGr:
            case ECI_NIF_PML_1_PLL_CONFIGr:
            case FSRD_SBUS_LAST_IN_CHAINr:
            case ECI_ECIC_BLOCKS_RESETr:
            case ECI_FAB_1_PLL_CONFIGr:
            case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
            case MESH_TOPOLOGY_RESERVED_MTCPr:
            case ECI_FAB_0_PLL_CONFIGr:
		    case MRPS_SBUS_BROADCAST_IDr:
			case MRPS_SBUS_LAST_IN_CHAINr:
			case MTRPS_EM_SBUS_BROADCAST_IDr:
			case MTRPS_EM_SBUS_LAST_IN_CHAINr:
			case SCH_SBUS_BROADCAST_IDr:
			case SCH_SBUS_LAST_IN_CHAINr:
			case CFC_GTIMER_CONFIGURATIONr:
			case CFC_INDIRECT_COMMANDr:
			case OAMP_GTIMER_CONFIGURATIONr:
			case OAMP_INDIRECT_COMMANDr:
			case CGM_SBUS_LAST_IN_CHAINr:
			case DRCA_GTIMER_CONFIGURATIONr:
			case DRCB_GTIMER_CONFIGURATIONr:
			case DRCC_GTIMER_CONFIGURATIONr:
			case DRCD_GTIMER_CONFIGURATIONr:
			case DRCE_GTIMER_CONFIGURATIONr:
			case DRCF_GTIMER_CONFIGURATIONr:
			case DRCG_GTIMER_CONFIGURATIONr:
			case DRCH_GTIMER_CONFIGURATIONr:
			case DRCH_REG_0087r:
			case ECI_SBUS_LAST_IN_CHAINr:
			case EGQ_SBUS_LAST_IN_CHAINr:
			case EPNI_SBUS_LAST_IN_CHAINr:
			case FMAC_SBUS_BROADCAST_IDr:
			case FMAC_SBUS_LAST_IN_CHAINr:
			case FSRD_SBUS_BROADCAST_IDr:
			case IHB_SBUS_LAST_IN_CHAINr:
			case IHP_PROTOCOL_TRAPS_PROGRAM_SELECTr:
			case IHP_SBUS_LAST_IN_CHAINr:
			case IPS_SBUS_LAST_IN_CHAINr:
			case IQM_SBUS_LAST_IN_CHAINr:
			case MESH_TOPOLOGY_GLOBAL_GENERAL_CFG_1r:

#if defined(PLISIM)
                /* list that we dont get the reset values back from the registers on jer pcid*/

            case ILKN_SLE_RX_AFIFO_WMr:
            case ILKN_SLE_RX_BURSTr:
            case ILKN_SLE_RX_CAL_INBANDr:
            case ILKN_SLE_RX_CAL_INBAND_DYNr:
            case ILKN_SLE_RX_CAL_OUTBANDr:
            case ILKN_SLE_RX_CAL_OUTBAND_DYNr:
            case ILKN_SLE_RX_DEBUG_HOLD_0r:
            case ILKN_SLE_RX_DEBUG_HOLD_1r:
            case ILKN_SLE_RX_DEBUG_HOLD_2r:
            case ILKN_SLE_RX_DEBUG_HOLD_3r:
            case ILKN_SLE_RX_EXT_INT_2_ND_MASKr:
            case ILKN_SLE_RX_EXT_INT_FORCEr:
            case ILKN_SLE_RX_EXT_INT_MASKr:
            case ILKN_SLE_RX_INT_2_ND_MASKr:
            case ILKN_SLE_RX_INT_FORCEr:
            case ILKN_SLE_RX_INT_MASKr:
            case ILKN_SLE_RX_LANE_2r:
            case ILKN_SLE_RX_METAFRAMEr:
            case ILKN_SLE_RX_REMAP_LANE_14_10r:
            case ILKN_SLE_RX_REMAP_LANE_19_15r:
            case ILKN_SLE_RX_REMAP_LANE_24_20r:
            case ILKN_SLE_RX_REMAP_LANE_29_25r:
            case ILKN_SLE_RX_REMAP_LANE_34_30r:
            case ILKN_SLE_RX_REMAP_LANE_39_35r:
            case ILKN_SLE_RX_REMAP_LANE_44_40r:
            case ILKN_SLE_RX_REMAP_LANE_47_45r:
            case ILKN_SLE_RX_REMAP_LANE_4_0r:
            case ILKN_SLE_RX_REMAP_LANE_9_5r:
            case ILKN_SLE_RX_RETRANSMIT_CONFIGr:
            case ILKN_SLE_RX_RETRANSMIT_TIME_CONFIGr:
            case ILKN_SLE_RX_RETRANSMIT_TIME_CONFIG_2r:
            case ILKN_SLE_RX_SERDES_TEST_CNTLr:
            case ILKN_SLE_RX_SERDES_TEST_PATTERNAr:
            case ILKN_SLE_RX_SERDES_TEST_PATTERNBr:
            case ILKN_SLE_RX_SERDES_TEST_PATTERNCr:
            case ILKN_SLE_TX_AFIFO_WMr:
            case ILKN_SLE_TX_BURSTr:
            case ILKN_SLE_TX_CAL_INBANDr:
            case ILKN_SLE_TX_CAL_INBAND_DYNr:
            case ILKN_SLE_TX_CAL_OUTBANDr:
            case ILKN_SLE_TX_CAL_OUTBAND_DYNr:
            case ILKN_SLE_TX_DEBUG_HOLD_0r:
            case ILKN_SLE_TX_DEBUG_HOLD_1r:
            case ILKN_SLE_TX_DEBUG_HOLD_2r:
            case ILKN_SLE_TX_DEBUG_HOLD_3r:
            case ILKN_SLE_TX_FIFO_CFGr:
            case ILKN_SLE_TX_INT_2_ND_MASKr:
            case ILKN_SLE_TX_INT_FORCEr:
            case ILKN_SLE_TX_INT_MASKr:
            case ILKN_SLE_TX_LANE_2r:
            case ILKN_SLE_TX_METAFRAMEr:
            case ILKN_SLE_TX_REMAP_LANE_14_10r:
            case ILKN_SLE_TX_REMAP_LANE_19_15r:
            case ILKN_SLE_TX_REMAP_LANE_24_20r:
            case ILKN_SLE_TX_REMAP_LANE_29_25r:
            case ILKN_SLE_TX_REMAP_LANE_34_30r:
            case ILKN_SLE_TX_REMAP_LANE_39_35r:
            case ILKN_SLE_TX_REMAP_LANE_44_40r:
            case ILKN_SLE_TX_REMAP_LANE_47_45r:
            case ILKN_SLE_TX_REMAP_LANE_4_0r:
            case ILKN_SLE_TX_REMAP_LANE_9_5r:
            case ILKN_SLE_TX_RETRANSMIT_CONFIGr:
            case ILKN_SLE_TX_SERDES_TEST_CNTLr:
            case ILKN_SLE_TX_SERDES_TEST_PATTERNAr:
            case ILKN_SLE_TX_SERDES_TEST_PATTERNBr:
            case ILKN_SLE_TX_SERDES_TEST_PATTERNCr:
#endif   /*(PLISIM) */

                return 1; /* Skip these registers */
            default:
                break;
        }
        if (SOC_IS_QUX(unit)) {
            /* XLMAC registers cannot access when reset, need confirm(Nir) */
            if ((ainfo->addr & 0xffff0000) == 0x60000) {
                /* chip reset cannot reset it */
               return 1;
            }
            switch (ainfo->reg) {
                /* reset chip cannot reach */
                case CGM_REG_0501r:
                case EDB_SBUS_LAST_IN_CHAINr:
                case EPNI_CHICKEN_BIT_VECTORr:
				    return 1;
                default:
                    break;
            }
        }
        if(SOC_IS_QAX(unit)) {

            if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "BROADCAST") != NULL) {
                return 1; /* Skip broadcast registers (sw only registers)*/
            }
            switch (ainfo->reg) {
             /**
              * DQM - need to review further 
              * regarding other registers 
              * pem,mmu,ipsec_wrapper_b,eci is indeed last in his sbus 
              * chainYaar/Nir - can you comment if that's ok ? 
              * (Tomer) 
              */
                case  IPSEC_SPU_WRAPPER_TOP_SBUS_LAST_IN_CHAINr:
                case  MMU_SBUS_LAST_IN_CHAINr:
                case  PEM_SBUS_LAST_IN_CHAINr:
                case  ECI_SBUS_LAST_IN_CHAINr:
                case  DQM_SBUS_LAST_IN_CHAINr:

              /**
               * default of this registers is reset(0) but when reset chip 
               * cant be access 
               */
                case  ECI_BLOCKS_SBUS_RESETr:
                case  ECI_BLOCKS_SOFT_RESETr:

             /**
              * the reset of this values does not apear at excel 
              * and its not 0's 
              */
                case  ECI_OGER_1027r:
                case  ECI_OGER_1028r:
                /* need confirm (Yael) */
                case ILB_LBRG_CONFIGURATIONr:
                /**
                 * following registers are not set to default values after init 
                 * (mirror registers) tomer & ariel
                 * CGM_RRP_MIRROR,DQM_DEQ_DRX_DCC_MIRROR,SQM_DEQ_DRX_DCC_MIRROR 
                 */
                case  CGM_REG_0504r:
                case  DQM_REG_0119r:
                case  SQM_REG_0122r:


                /**
                 * The PRGE_DEBUG_DATA can be updated by the block - the 
                 * differentce here is OK. (Ofer landau)
                 */
                case  IPSEC_REG_014Ar:

                    return 1; /* Skip these registers */
                default:
                    break;
            }
        }
    }
#endif    
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        switch (ainfo->reg) {
            case TOP_TSC_AFE_PLL_STATUSr: 
            case TOP_MISC_CONTROLr:
            case TOP_MISC_CONTROL_2r:
                /* registers have different values then their reset values */
                return 1;
            case TOP_TSC_ENABLEr:
            case TOP_CORE_CLK_FREQ_SELr: 
                /* Overwritten by init soc */
            case XLPORT_TSC_PLL_LOCK_STATUSr:
            case CLPORT_TSC_PLL_LOCK_STATUSr:
            case XLPORT_LED_CHAIN_CONFIGr:
            case CLPORT_LED_CHAIN_CONFIGr:
            case CXXPORT_LED_CHAIN_CONFIGr:
                /* Changes depending on chip top level integration. */
                return 1;
            case CFAPCONFIGr:
            case CFAPFULLSETPOINTr:
            case CFAPFULLRESETPOINTr:
                /* registers have different values then their reset values */
                return 1;
            case AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROLr:
            case AVS_REG_PMB_SLAVE_AVS_PWD_CONTROLr:
            case AVS_REG_PMB_SLAVE_AVS_ROSC_CONTROLr:
            case AVS_REG_PMB_SLAVE_AVS_ROSC_H_THRESHOLDr:
            case AVS_REG_PMB_SLAVE_AVS_ROSC_S_THRESHOLDr:
            case AVS_REG_PMB_SLAVE_BPCM_CONTROLr:
                /* ARS wrappers stubbed out in QT. Register access will fail */
                if (SAL_BOOT_QUICKTURN) {
                    return 1;
                } else {
                    break;
                }
            case L2_ISS_BANK_CONFIGr:
            case L3_ISS_BANK_CONFIGr:
            case L2_ISS_LOG_TO_PHY_BANK_MAPr:
            case L3_ISS_LOG_TO_PHY_BANK_MAPr:
                /* Changes dependent on config properties set during initialization */
                return 1;
            case TOP_HW_TAP_MEM_IF_ECC_CTRLr:
            case TOP_HW_TAP_MEM_OTP_ECC_CTRLr:
            case TOP_TAP_CONTROLr:
                /*Following registers are capturing the ECC for chip level memories,
                so reset value of 0x0 will not match*/
                return 1;
            case TOP_UC_TAP_CONTROLr:
            case TOP_UC_TAP_WRITE_DATAr:
                /* OVSTB toggle sequence run during soc init  */
                return (soc_feature(unit, soc_feature_apache_ovst_toggle));

            case TOP_PVTMON_0_INTR_THRESHOLDr:
            case TOP_PVTMON_1_INTR_THRESHOLDr:
            case TOP_PVTMON_2_INTR_THRESHOLDr:
            case TOP_PVTMON_3_INTR_THRESHOLDr:
            case TOP_PVTMON_4_INTR_THRESHOLDr:
            case TOP_PVTMON_5_INTR_THRESHOLDr:
            case TOP_PVTMON_6_INTR_THRESHOLDr:
            case TOP_PVTMON_7_INTR_THRESHOLDr:
            case TOP_PVTMON_MASKr:
                /*  Registers programmed to non-default value during SOC init */
                return 1;
            default:
                break;
        }
    }
#endif /* BCM_APACHE_SUPPORT */

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        switch (ainfo->reg) {
            case XLPORT_PMD_PLL_CTRL_CONFIGr:
                /*  Registers programmed to non-default value during SOC init */
                return 1;
            default:
                break;
        }
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2(unit)) {
        uint16 dev_id;
        uint8  rev_id;
        int    skew_id;
        uint32 rval;

        READ_TOP_DEV_REV_IDr(unit, &rval);
        skew_id = soc_reg_field_get(unit, TOP_DEV_REV_IDr, rval, DEVICE_SKEWf);
        soc_cm_get_id(unit, &dev_id, &rev_id);
        switch (ainfo->reg) {
            case TOP_CORE_CLK_FREQ_SELr: 
                if (rev_id == BCM56850_A0_REV_ID || 
                    (BCM56851_DEVICE_ID == dev_id && skew_id == 2)) { /*56851P*/
                    return 1;
                }
                break;
            default:
                break;
        }
    }
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TD2P_TT2P(unit)) {
        uint16 dev_id;
        uint8  rev_id;

        soc_cm_get_id(unit, &dev_id, &rev_id);

        switch (ainfo->reg) {
            case TOP_CORE_CLK_FREQ_SELr:
                if (BCM56833_DEVICE_ID == dev_id 
                        || BCM56861_DEVICE_ID == dev_id
                        || BCM56864_DEVICE_ID == dev_id
                        || BCM56832_DEVICE_ID == dev_id) {
                    /*
                     * Register has different value than it's reset default
                     * value.
                     */
                    return 1;
                }
                break;
            case TOP_MISC_CONTROLr: 
            case TOP_TSC_DISABLEr: 
                /* registers have different values than their reset values */
                return 1;
            default:
                break;
        }
    }
#endif /* !BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_TOMAHAWKPLUS_SUPPORT
    if (SOC_IS_TOMAHAWKPLUS(unit)) {
        switch (ainfo->reg) {
            /* registers have different read-only values then their default values */
            case TOP_HW_TAP_MEM_ECC_CTRL_0r:
            case TOP_HW_TAP_MEM_ECC_CTRL_1r:
            case TOP_TAP_CONTROLr:
                return 1;
            default:
                break;
        }
    }
#endif /* !BCM_TOMAHAWKPLUS_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        switch (ainfo->reg) {
            case IS_OPP_SCHED_CFGr:
            case IS_OPP_SCHED_CFG_PIPE0r:
            case IS_OPP_SCHED_CFG_PIPE1r:
            case IS_OPP_SCHED_CFG_PIPE2r:
            case IS_OPP_SCHED_CFG_PIPE3r:
            case IS_TDM_CONFIGr:
            case IS_TDM_CONFIG_PIPE0r:
            case IS_TDM_CONFIG_PIPE1r:
            case IS_TDM_CONFIG_PIPE2r:
            case IS_TDM_CONFIG_PIPE3r:
            case TOP_XG_PLL0_CTRL_5r:
            /* registers have different read-only values then their default values */
            case TOP_HW_TAP_MEM_ECC_CTRL_0r:
            case TOP_HW_TAP_MEM_ECC_CTRL_1r:
            case TOP_TAP_CONTROLr:
            /* This register would be modified during init soc */
            case IDB_SER_CONTROLr:
                /* Skip these registers */
                return 1;
            default:
                break;
        }
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (dev_id == BCM56982_DEVICE_ID) {
            switch (ainfo->reg) {
                /* These regs belong to PMs in pipes which are not active
                    so need to skip them
                 */
                case IDB_CA0_HW_STATUS_2r:
                case IDB_CA0_HW_STATUS_2_PIPE0r:
                case IDB_CA0_HW_STATUS_2_PIPE2r:
                case IDB_CA0_HW_STATUS_2_PIPE4r:
                case IDB_CA0_HW_STATUS_2_PIPE6r:
                case IDB_CA1_HW_STATUS_2r:
                case IDB_CA1_HW_STATUS_2_PIPE0r:
                case IDB_CA1_HW_STATUS_2_PIPE4r:
                case IDB_CA2_HW_STATUS_2_PIPE3r:
                case IDB_CA2_HW_STATUS_2_PIPE7r:
                case IDB_CA3_HW_STATUS_2_PIPE1r:
                case IDB_CA3_HW_STATUS_2_PIPE3r:
                case IDB_CA3_HW_STATUS_2_PIPE5r:
                case IDB_CA3_HW_STATUS_2_PIPE7r:
                    return 1;
                default:
                    break;
            }


        }
        switch (ainfo->reg) {
            /* this reg is used for broadcast PM programming
               so the value will be diff from the expected
               reset value
             */
            case CDPORT_SBUS_CONTROLr:
            /* this reg has read-only fields */
            case CDMAC_FIFO_STATUSr:
            case CDMAC_RX_LSS_STATUSr:
            /* This reg has different value for diff dpr clock freq */
            case EPOST_SLOT_PIPELINE_CONFIGr:
            case SLOT_PIPELINE_CONFIGr:
            /* This reg changes its state after deassertion of reset
               so skipping the register */
            case MMU_RQE_INFOTBL_FP_INITr:
            /* This reg has been set with diff value for SW-WAR
               SDK-215193 so skipping this reg */
            case AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr:
                return 1;
            default:
                break;
        }
    }
#endif

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        switch (ainfo->reg) {
        case XLMAC_CLEAR_ECC_STATUSr:
        case XLMAC_CLEAR_FIFO_STATUSr:
        case XLMAC_CLEAR_RX_LSS_STATUSr:
        case XLMAC_CTRLr:
        case XLMAC_E2ECC_DATA_HDR_0r:
        case XLMAC_E2ECC_DATA_HDR_1r:
        case XLMAC_E2ECC_MODULE_HDR_0r:
        case XLMAC_E2ECC_MODULE_HDR_1r:
        case XLMAC_E2EFC_DATA_HDR_0r:
        case XLMAC_E2EFC_DATA_HDR_1r:
        case XLMAC_E2EFC_MODULE_HDR_0r:
        case XLMAC_E2EFC_MODULE_HDR_1r:
        case XLMAC_E2E_CTRLr:
        case XLMAC_ECC_CTRLr:
        case XLMAC_ECC_FORCE_DOUBLE_BIT_ERRr:
        case XLMAC_ECC_FORCE_SINGLE_BIT_ERRr:
        case XLMAC_EEE_1_SEC_LINK_STATUS_TIMERr:
        case XLMAC_EEE_CTRLr:
        case XLMAC_EEE_TIMERSr:
        case XLMAC_GMII_EEE_CTRLr:
        case XLMAC_HIGIG_HDR_0r:
        case XLMAC_HIGIG_HDR_1r:
        case XLMAC_INTR_ENABLEr:
        case XLMAC_LLFC_CTRLr:
        case XLMAC_MEM_CTRLr:
        case XLMAC_MODEr:
        case XLMAC_PAUSE_CTRLr:
        case XLMAC_PFC_CTRLr:
        case XLMAC_PFC_DAr:
        case XLMAC_PFC_OPCODEr:
        case XLMAC_PFC_TYPEr:
        case XLMAC_RX_CTRLr:
        case XLMAC_RX_LLFC_MSG_FIELDSr:
        case XLMAC_RX_LSS_CTRLr:
        case XLMAC_RX_MAC_SAr:
        case XLMAC_RX_MAX_SIZEr:
        case XLMAC_RX_VLAN_TAGr:
        case XLMAC_SPARE0r:
        case XLMAC_SPARE1r:
        case XLMAC_TIMESTAMP_ADJUSTr:
        case XLMAC_TIMESTAMP_BYTE_ADJUSTr:
        case XLMAC_TX_CRC_CORRUPT_CTRLr:
        case XLMAC_TX_CTRLr:
        case XLMAC_TX_LLFC_MSG_FIELDSr:
        case XLMAC_TX_MAC_SAr:
            /* Skip these registers for QSGMII port */
            if (SOC_PORT_VALID(unit, ainfo->port) &&
                IS_QSGMII_PORT(unit, ainfo->port)) {
                return 1;
            }
            break;
        case TOP_QGPHY_CTRL_0r:
        case TOP_QGPHY_CTRL_2r:
        case TOP_QGPHY_CTRL_3r:
            return 1;
        default:
            break;
        }
        if (_soc_hr4_reg_skip(unit, ainfo->reg)) {
            return 1;
        }
    }
#endif

#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                switch (ainfo->reg) {
                    /* Ingress Scheduler registers need to be programmed during init to get access to IP/EP block
                     * skip following IS_* registers from TR1 test
                     */
                    case IS_OPP_SCHED_CFGr:
                    case IS_OPP_SCHED_CFG_PIPE0r:
                    case IS_OPP_SCHED_CFG_PIPE1r:
                    case IS_TDM_CONFIGr:
                    case IS_TDM_CONFIG_PIPE0r:
                    case IS_TDM_CONFIG_PIPE1r:
                    case TOP_XG_PLL0_CTRL_5r:
                    /* registers have different read-only values then their default values */
                    case TOP_HW_TAP_MEM_ECC_CTRL_0r:
                    case TOP_HW_TAP_MEM_ECC_CTRL_1r:
                    case TOP_TAP_CONTROLr:
                    /* RH_DLB_SELECTION field in ENHANCED_HASHING_CONTROL needs to be set to 1
                     *  in oder to use RH. This can't be set by default to 1 in init time.
                     *  Hence skip RH related following registers during test.
                     */
                    case RH_ECMP1_DROPSr:
                    case RH_ECMP1_DROPS_PIPE0r:
                    case RH_ECMP1_DROPS_PIPE1r:
                    case RH_ECMP2_DROPSr:
                    case RH_ECMP2_DROPS_PIPE0r:
                    case RH_ECMP2_DROPS_PIPE1r:
                    /* This register would be modified during init soc */
                    case IDB_SER_CONTROLr:
                    case IDB_SER_CONTROL_64r:
                    case TOP_MISC_GENERIC_CONTROLr:
                    case CHIP_CONFIGr:
                        /* Skip these registers */
                        return 1;
                    default:
                        if (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_AVS)) {
                            /* Skip AVS block registers */
                            return 1;
                        }
                        break;
                }
            }
#endif

    if (SOC_REG_INFO(unit, ainfo->reg).flags &
        (SOC_REG_FLAG_IGNORE_DEFAULT | SOC_REG_FLAG_RO | SOC_REG_FLAG_WO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL)) {
        return 1;   /* no testable bits */
    }

    return 0;
}

