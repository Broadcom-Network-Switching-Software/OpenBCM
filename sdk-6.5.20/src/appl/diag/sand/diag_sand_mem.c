/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       mem.c
 * Purpose:    Diag shell memory commands for SAND
 */

#include <shared/bsl.h>
#include <ibde.h>

#include <sal/appl/pci.h>

#include <soc/mem.h>
#include <soc/cm.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/dcmn_mem.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#include <soc/sand/sand_mem.h>

#if defined (BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/counters.h>
#endif
#ifdef BCM_SAND_SUPPORT
#include "src/appl/test/testlist.h"
#endif
#ifdef BCM_DNX_SUPPORT
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#endif
#include <soc/intr.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/dcmn_intr_corr_act_func.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#include <soc/sand/sand_intr_corr_act_func.h>

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>

#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/dcmn/interrupts/dcmn_intr.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_access.h>

#define DUMP_TABLE_RAW          0x01
#define DUMP_TABLE_HEX          0x02
#define DUMP_TABLE_ALL          0x04
#define DUMP_TABLE_CHANGED      0x08
#define DUMP_DISABLE_CACHE      0x10
#define DUMP_ADDR_ONLY          0x20
#define DUMP_ALL_FROM_BLOCK     0x40
#define ARAD_INDIRECT_SIXE      512

extern int mem_test_rw_init(
    int,
    soc_mem_t mem,
    void **);

static void diag_sand_mem_entry_dump_field(
    int unit,
    soc_mem_t mem,
    void *buf,
    char *field_name);

extern int dpp_all_reg_get(
    int unit,
    int is_debug);
extern int mem_test_default_init(
    int,
    soc_mem_t mem,
    void **);
extern void tr8_reset_bits_counter(
    void);
extern void tr8_increment_bits_counter(
    uint32 bits_num);
extern uint32 tr8_bits_counter_get(
    void);
extern uint32 tr8_get_bits_num(
    uint32 number);
extern void tr8_write_dump(
    const char *_Format);
#if defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
void memtest_mask_get_tr8(
    int unit,
    soc_mem_t mem,
    uint32 *mask);
#endif

/*
 * TYPE DEFS
 */

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */
#define MAX_MEM_SIZE   100000

typedef struct test_run_memory_stat_s
{

    int mem_valid_num;
    int mem_test_num;
    int mem_pass_num;
    int mem_fail_num;

} test_run_memory_stat_t;

test_run_memory_stat_t test_run_memory_stat;
#if defined (BCM_PETRA_SUPPORT)
/**
 *  check if memory too large to be tested
 *  due to endless test time
 * 
 * @author elem (17/12/2014)
 * 
 * @param unit 
 * @param mem 
 * @param max_size
 * 
 * @return STATIC int 
 */
STATIC int
mem_too_large_for_test(
    int unit,
    soc_mem_t mem,
    int max_size)
{
    int mem_nof_elem = SOC_MEM_IS_ARRAY(unit, mem) ? SOC_MEM_NUMELS(unit, mem) : 1;
    int mem_size = mem_nof_elem * SOC_MEM_TABLE_BYTES(unit, mem);
    return (mem_size > max_size && max_size >= 0) ? 1 : 0;
}

STATIC int
mem_in_tr7_schan_skip_list_for_jericho_emulation(
    int unit,
    soc_mem_t mem)
{

    /*
     * memories failed due to scahn timeout in tr 7
     */
    switch (mem)
    {

        case EDB_TRILL_FORMAT_HALF_ENTRYm:
        case SER_ACC_TYPE_MAPm:
        case MRPS_MCDB_PRFCFG_1m:
        case SER_MEMORYm:
        case PPDB_A_OEMA_ACDT_SLAVE_Hm:
        case PPDB_A_OEMA_ACDT_MASTER_Hm:
        case IHP_FIFO_OID_2m:
        case EDB_PROTECTION_ENTRYm:
        case IQM_PAKCET_DESCRIPTOR_MEMORY_A_DYNAMICm:
        case EDB_LINK_LAYER_OR_ARP_NEW_FORMATm:
        case EDB_TRILL_FORMAT_FULL_ENTRYm:
        case IQM_PAKCET_DESCRIPTOR_MEMORY_B_DYNAMICm:
            return 1;

    }

    return 0;

}

STATIC int
mem_in_tr7_skip_list_for_jericho_emulation(
    int unit,
    soc_mem_t mem)
{

    if (mem_in_tr7_schan_skip_list_for_jericho_emulation(unit, mem))
    {
        return 1;
    }

    return 0;
}

STATIC int
mem_in_skip_list_for_jericho_emulation(
    int unit,
    soc_mem_t mem)
{
    if (!SOC_IS_JERICHO(unit))
    {
        return 0;
    }
    return mem_in_tr7_skip_list_for_jericho_emulation(unit, mem);

}
#endif
/* The function rval_test_skip_mem contains skip list for TR 7 */
STATIC int
rval_test_skip_mem(
    int unit,
    soc_mem_t mem,
    tr7_dbase_t * tr7_ptr)
{
#ifdef BCM_DFE_SUPPORT
    int rv;
    int is_filtered = 0;

    if (SOC_IS_DFE(unit))
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_mem_filter, (unit, mem, &is_filtered));
        if (rv != SOC_E_NONE)
        {
            return rv;
        }
        if (is_filtered)
        {
            return 1;
        }
    }
#endif /* BCM_DFE_SUPPORT */

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        if (!SOC_MEM_IS_VALID(unit, mem)
            || (soc_mem_flags(unit, mem) & (SOC_MEM_FLAG_READONLY | SOC_MEM_FLAG_WRITEONLY | SOC_MEM_FLAG_SIGNAL)) != 0)
        {
            return 1;
        }
        if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))
        {
            switch (mem)
            {
                case RTP_MULTI_CAST_TABLE_UPDATEm:
                    return 1;   /* Skip these tables */
                default:
                    break;
            }
        }
        switch (mem)
        {
            case BRDC_QRH_FFLBm:
            case BRDC_QRH_MCLBTm:
            case BRDC_QRH_MCSFFm:
            case BRDC_QRH_MNOLm:
            case FSRD_FSRD_PROM_MEMm:
            case QRH_FFLBm:
            case QRH_MCLBTm:
            case QRH_MCSFFm:
            case QRH_MNOLm:
                return 1;       /* Skip these tables */
            default:
                break;
        }
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        soc_block_type_t block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
        if (!SOC_MEM_IS_VALID(unit, mem)
            || (soc_mem_flags(unit, mem) & (SOC_MEM_FLAG_READONLY | SOC_MEM_FLAG_WRITEONLY | SOC_MEM_FLAG_SIGNAL)) != 0)
        {
            return 1;
        }
        if (soc_mem_is_signal(unit, mem))
        {
            return 1;
        }
        if ((dnx_data_dev_init_general_access_only_get(unit) == 1)
            && (block == SOC_BLK_CDPORT || block == SOC_BLK_CDMAC))
        {
            return 1;
        }
        if (block == SOC_BLK_FSCL)
        {
            return 1;
        }
        switch (mem)
        {
                /*
                 * this memory has to be skipped
                 */
                /*
                 * The HBM memories are not real memories 
                 */
            case BRDC_HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case BRDC_HBC_HBM_DRAM_CPU_ACCESSm:
            case BRDC_FSRD_FSRD_PROM_MEMm:
            case FSRD_FSRD_PROM_MEMm:
            case HBC_HBM_DRAM_CPU_ACCESSm:
            case HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case HBMC_HBM_PHY_CHM_REGISTER_ACCESSm:

                /*
                 * The CDU instrumentation and rx/tx memories do not have to be initialized and can not be written by
                 * DMA 
                 */
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm:
            case CDU_RX_MEMA_CTRLm:
            case CDU_RX_MEMB_CTRLm:
            case CDU_RX_MEMC_CTRLm:
            case CDU_RX_MEMD_CTRLm:
            case CDU_RX_MEME_CTRLm:
            case CDU_RX_MEMF_CTRLm:
            case CDU_RX_MEMG_CTRLm:
            case CDU_RX_MEMH_CTRLm:
            case CDU_RX_MEMI_CTRLm:
            case CDU_RX_MEMJ_CTRLm:
            case CDU_RX_MEMK_CTRLm:
            case CDU_RX_MEML_CTRLm:
            case CDU_RX_MEMM_CTRLm:
            case CDU_RX_MEMN_CTRLm:
            case CDU_RX_MEMO_CTRLm:
            case CDU_RX_MEMP_CTRLm:
            case CDU_TX_MEMA_CTRLm:
            case CDU_TX_MEMB_CTRLm:
            case CDU_TX_MEMC_CTRLm:
            case CDU_TX_MEMD_CTRLm:
            case CDU_TX_MEME_CTRLm:
            case CDU_TX_MEMF_CTRLm:
            case CDU_TX_MEMG_CTRLm:
            case CDU_TX_MEMH_CTRLm:
            case CDU_TX_MEMI_CTRLm:
            case CDU_TX_MEMJ_CTRLm:

                /*
                 * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization 
                 */
            case DDHB_MACRO_0_ABK_BANK_Am:
            case DDHB_MACRO_0_ABK_BANK_Bm:
            case DDHB_MACRO_0_ENTRY_BANKm:
            case DDHB_MACRO_1_ABK_BANK_Am:
            case DDHB_MACRO_1_ABK_BANK_Bm:
            case DDHB_MACRO_1_ENTRY_BANKm:
            case DDHA_MACRO_0_ABK_BANK_Am:
            case DDHA_MACRO_0_ABK_BANK_Bm:
            case DDHA_MACRO_0_ENTRY_BANKm:
            case DDHA_MACRO_1_ABK_BANK_Am:
            case DDHA_MACRO_1_ABK_BANK_Bm:
            case DDHA_MACRO_1_ENTRY_BANKm:
            case DHC_MACRO_ABK_BANK_Am:
            case DHC_MACRO_ABK_BANK_Bm:
            case DHC_MACRO_ENTRY_BANKm:

                /*
                 * These are address spaces saved for ilkn memory mapped registers. 
                 */
            case ILE_PORT_0_CPU_ACCESSm:
            case ILE_PORT_1_CPU_ACCESSm:

                /*
                 * All below are either not real memories or dont have protection. - Mosi Ravia 
                 */
            case KAPS_RPB_TCAM_CPU_COMMANDm:
            case KAPS_TCAM_ECC_MEMORYm:
            case MDB_ARM_KAPS_TCMm:
            case MDB_EEDB_ABK_BANKm:
            case MDB_MEM_2300000m:
            case MACT_CPU_REQUESTm:
            case ECI_SAM_CTRLm:

                /*
                 * SCH_SCHEDULER_INIT is not a real memory 
                 */
            case SCH_SCHEDULER_INITm:
            case SQM_DEQ_QSTATE_PENDING_FIFOm:
            case SQM_ENQ_COMMAND_RXIm:
            case SQM_PDB_LINK_LISTm:
            case SQM_PDMm:
            case SQM_QUEUE_DATA_MEMORYm:
            case SQM_TX_BUNDLE_MEMORYm:
            case SQM_TX_PACKET_DESCRIPTOR_FIFOS_MEMORYm:
            case SCH_FORCE_STATUS_MESSAGEm:
            case SCH_RESERVED_37m:
            case SCH_RESERVED_54m:
            case SCH_RESERVED_55m:
            case SCH_SLOW_FACTOR_MEMORY_SFMm:
            case SPB_COUNTERS_SHADOWm:
            case EPS_CAL_CAL_INDX_MUXm:
            case EPS_HEADR_CMPNS_CAL_HP_CBMm:
            case EPS_HEADR_CMPNS_CAL_LP_CBMm:
            case EPS_OTM_CALENDAR_CRDT_TABLEm:
            case EPS_OTM_HP_CRDT_TABLEm:
            case EPS_OTM_LP_CRDT_TABLEm:
            case EPS_QP_CREDIT_TABLEm:
            case EPS_TCG_CREDIT_TABLEm:
            case AM_TABLEm:
            case CDMIB_MEMm:
            case CDPORT_TSC_UCMEM_DATAm:
                /*
                 * The *_DEBUG_UNIT and *_HIT_INDICATION memories are not real 
                 */
            case IPPD_MEM_F000000m:
            case ERPP_EPMFCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTECS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPACS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPBCS_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSA_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSB_TCAM_HIT_INDICATIONm:
            case IPPD_PMFBCS_TCAM_HIT_INDICATIONm:
            case IPPE_PRTCAM_TCAM_HIT_INDICATIONm:
            case IPPE_LLRCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTACS_TCAM_HIT_INDICATIONm:
            case IPPF_VTCCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTBCS_TCAM_HIT_INDICATIONm:
            case KAPS_RPB_TCAM_HIT_INDICATIONm:
            case KAPS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_BANKm:
            case IPPA_VTECS_TCAM_BANKm:
            case IPPB_FLPACS_TCAM_BANKm:
            case IPPB_FLPBCS_TCAM_BANKm:
            case IPPE_PRTCAM_TCAM_BANKm:
            case MDB_EEDB_ENTRY_BANKm:
            case MDB_MEM_02000000m:

                /*
                 * Q2A memory skip list not real memories
                 */
            case CLPORT_WC_UCMEM_DATAm:
            case DPC_DRAM_PHY_REGISTER_ACCESSm:
            case FLEXEWP_TINY_MAC_MEMORIESm:
            case FLEXEWP_TINY_MAC_REGISTERSm:
            case FSCL_FLEXE_CORE_REGISTERSm:
            case OAMP_CLS_FLEX_CRC_TCAMm:
            case OAMP_CLS_TRAP_CODE_TCAMm:
            case OAMP_PE_0_PROG_TCAMm:
            case OAMP_PE_1_PROG_TCAMm:
            case OAMP_RSP_TCAMm:
            case OAMP_RX_OAM_ID_TCAMm:
            case OAMP_RXP_R_MEP_INDEX_FIFOm:
                /*
                 * Write access is forbidden for this memory 
                 */
            case IPS_MEM_2E8000m:
                /*
                 * NOT real memories. 
                 */
            case MDB_EOEM_0m:
            case MDB_EOEM_1m:
            case MDB_ESEMm:
            case MDB_EXEM_1m:
            case MDB_EXEM_2m:
            case MDB_EXEM_3m:
            case MDB_EXEM_4m:
            case MDB_GLEM_0m:
            case MDB_GLEM_1m:
            case MDB_IOEM_0m:
            case MDB_IOEM_1m:
            case MDB_ISEM_1m:
            case MDB_ISEM_2m:
            case MDB_ISEM_3m:
            case MDB_LEMm:
            case MDB_MC_IDm:
            case MDB_RMEPm:
            case MDB_ARM_MEM_0m:
                /*
                 * Dynamic and should be skipped. - ASIC 
                 */
            case EVNT_ASSIGNED_CMIC_ENGINE_TABLEm:
            case ERPP_EPMFCS_TCAM_BANKm:
            case IPPC_PMFACSA_TCAM_BANKm:
            case IPPC_PMFACSB_TCAM_BANKm:
            case IPPD_PMFBCS_TCAM_BANKm:
            case IPPE_LLRCS_TCAM_BANKm:
            case IPPF_VTACS_TCAM_BANKm:
            case IPPF_VTBCS_TCAM_BANKm:
            case IPPF_VTCCS_TCAM_BANKm:
            case IPPB_MPLS_LABEL_TYPES_CAM_0m:
            case IPPB_MPLS_LABEL_TYPES_CAM_1m:
            case TCAM_TCAM_ACTION_HIT_INDICATIONm:
            case TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm:
            case TCAM_TCAM_BANKm:
            case TX_LKUP_1588_MEM_400Gm:

                /*
                 * Part of the field was dynamic and should be skipped. 
                 */
            case OAMP_MEP_DBm:
            case OAMP_RMEP_DBm:
                return 1;
            default:
                break;
        }
    }
#endif

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        switch (mem)
        {
            case IDR_MEM_1B0000m:
            case IPT_PRIORITY_BITS_MAPPING_2_FDTm:
            case EPNI_MEM_760000m:
            case FSRD_FSRD_WL_EXT_MEMm:
            case IDR_GLOBAL_METER_STATUSm:
            case IHB_MEM_10E0000m:
            case IHB_MEM_16E0000m:
            case IHB_TCAM_BANKm:
            case IHB_TCAM_BANK_COMMANDm:
            case IHB_TCAM_BANK_REPLYm:
            case IHP_MEM_740000m:
            case IHP_MEM_9E0000m:
            case IHP_MEM_C50000m:
            case IPS_MEM_200000m:
            case IQM_PDMm:
            case IQM_VSQ_A_MX_OCm:
            case IQM_VSQ_B_MX_OCm:
            case IQM_VSQ_C_MX_OCm:
            case IQM_VSQ_D_MX_OCm:
            case IQM_VSQ_E_MX_OCm:
            case IQM_VSQ_F_MX_OCm:
            case MMU_RAFA_RADDR_STATUSm:
            case MMU_RAFB_RADDR_STATUSm:
            case MMU_RAFC_RADDR_STATUSm:
            case MMU_RAFD_RADDR_STATUSm:
            case MMU_RAFE_RADDR_STATUSm:
            case MMU_RAFF_RADDR_STATUSm:
            case MMU_RAFG_RADDR_STATUSm:
            case MMU_RAFH_RADDR_STATUSm:
            case MMU_WAFA_HALFA_RADDR_STATUSm:
            case MMU_WAFA_HALFB_RADDR_STATUSm:
            case MMU_WAFB_HALFA_RADDR_STATUSm:
            case MMU_WAFB_HALFB_RADDR_STATUSm:
            case MMU_WAFC_HALFA_RADDR_STATUSm:
            case MMU_WAFC_HALFB_RADDR_STATUSm:
            case MMU_WAFD_HALFA_RADDR_STATUSm:
            case MMU_WAFD_HALFB_RADDR_STATUSm:
            case MMU_WAFE_HALFA_RADDR_STATUSm:
            case MMU_WAFE_HALFB_RADDR_STATUSm:
            case MMU_WAFF_HALFA_RADDR_STATUSm:
            case MMU_WAFF_HALFB_RADDR_STATUSm:
            case MMU_WAFG_HALFA_RADDR_STATUSm:
            case MMU_WAFG_HALFB_RADDR_STATUSm:
            case MMU_WAFH_HALFA_RADDR_STATUSm:
            case MMU_WAFH_HALFB_RADDR_STATUSm:
            case OAMP_MEM_340000m:
            case PPDB_A_TCAM_BANKm:
            case PPDB_A_TCAM_BANK_COMMANDm:
            case PPDB_A_TCAM_BANK_REPLYm:
            case PPDB_B_LARGE_EM_FLUSH_DBm:
            case SCH_PORT_QUEUE_SIZE__PQSm:
            case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm:
            case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm:

                /*
                 * Schan TO on Write 
                 */
            case ECI_MBU_MEMm:
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case EPNI_ESEM_MANAGEMENT_REQUESTm:
            case IHB_OEMA_MANAGEMENT_REQUESTm:
            case IHB_OEMB_MANAGEMENT_REQUESTm:
            case IHP_ISA_MANAGEMENT_REQUESTm:
            case IHP_ISB_MANAGEMENT_REQUESTm:
            case OAMP_RMAPEM_MANAGEMENT_REQUESTm:
            case PORT_WC_UCMEM_DATAm:
            case PPDB_A_OEMA_MANAGEMENT_REQUESTm:
            case PPDB_A_OEMB_MANAGEMENT_REQUESTm:

                /*
                 * Schan TO on Read 
                 */
            case MMU_DRAM_ADDRESS_SPACEm:
                return 1;       /* Skip these tables */
            default:
                break;
        }
    }

    if (SOC_IS_JERICHO_PLUS_A0(unit))
    {
        switch (mem)
        {
            case IHB_FEC_PATH_SELECTm:
            case IHP_VTT_PATH_SELECTm:

                return 1;       /* Skip these tables */
            default:
                break;
        }
    }

    if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        switch (mem)
        {
            case IPS_MAXQSZm:
            case RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm:
            case RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm:

                return 1;

            default:
                break;
        }
    }
    if (SOC_IS_JERICHO(unit))
    {
        soc_field_t crps_disallowed_fields[] = { PACKETS_COUNTERf, NUM_SOC_FIELD };

        soc_block_type_t block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));

        if (mem < tr7_ptr->start_from || mem > tr7_ptr->start_from + tr7_ptr->count)
        {
            return 1;
        }
        if (!tr7_ptr->include_port_macros)
        {
            if (block_can_be_disabled(block))
            {
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Jericho excluded block %s\n"), SOC_BLOCK_NAME(unit, mem)));
                return 1;
            }
        }

        if (sand_mem_contain_one_of_the_fields(unit, mem, crps_disallowed_fields))
        {
            return 1;
        }
        if (mem_in_skip_list_for_jericho_emulation(unit, mem))
        {
            return 1;
        }
        /*
         * EDB have r/w comparission problems(read!=write) even that enable dynamic done for the block
         */
        if (block == SOC_BLK_EDB)
        {
            return 1;
        }

        switch (mem)
        {
            case OAMP_RMAPEM_MANAGEMENT_REQUESTm:
            case PPDB_A_OEMA_MANAGEMENT_REQUESTm:
            case PPDB_A_OEMB_MANAGEMENT_REQUESTm:
            case PPDB_B_LARGE_EM_FORMAT_0_TYPE_0m:
            case SCH_MEM_30000000m:
            case SCH_MEM_01F00000m:
            case SCH_SCHEDULER_INITm:
            case SER_RESULT_0m:
            case SER_RESULT_1m:
            case SER_RESULT_DATA_0m:
            case SER_RESULT_DATA_1m:
            case SER_RESULT_EXPECTED_0m:
            case SER_RESULT_EXPECTED_1m:
            case SCH_FORCE_STATUS_MESSAGEm:
            case EGQ_QP_CBMm:
            case EGQ_TCG_CBMm:
            case IHB_ISEM_MAA_CAMm:
            case IPS_CR_BAL_TABLEm:
            case IPS_QDESCm:
            case IPS_MEM_220000m:
            case IQMT_PDM_0m:
            case IQMT_PDM_1m:
            case IQMT_PDM_2m:
            case IQMT_PDM_3m:
            case IQMT_PDM_4m:
            case IQMT_PDM_5m:
            case IQM_IQM_READ_PENDING_FIFOm:
            case IQM_PAKCET_DESCRIPTOR_MEMORY_ECC_DYNAMICm:
            case IQM_VSQA_MX_OCm:
            case IQM_VSQB_MX_OCm:
            case IQM_VSQC_MX_OCm:
            case IQM_VSQD_MX_OCm:
            case IQM_VSQE_MX_OCm:
            case IQM_VSQF_MX_OCm:
            case MRPS_MCDA_DYNAMICm:
            case MRPS_MCDB_DYNAMICm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_MAA_CAMm:
            case OAMP_DM_TRIGERm:
            case PPDB_A_OEMA_MAA_CAMm:
            case PPDB_A_OEMB_MAA_CAMm:
            case PPDB_A_TCAM_BANKm:
            case PPDB_B_LARGE_EM_MAA_CAMm:
            case SCH_CIR_SHAPERS_DYNAMIC_TABEL_CSDTm:
            case SCH_PIR_SHAPERS_DYNAMIC_TABEL_PSDTm:
            case IHB_FIFO_DSP_2m:
            case IHP_FIFO_8_TO_41m:
            case RTP_UNICAST_DISTRIBUTION_MEMORYm:
            case RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CTRL_CELLSm:
            case RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLSm:

                /*
                 *   list of dynamic memories from crps,mtrps_em that currently  change by the chip
                 *    and we wait to solution from the asic block owner(inbal b.
                 */
            case MTRPS_EM_MCDA_DYNAMICm:
            case MTRPS_EM_MCDB_DYNAMICm:

                /*
                 * Not a real memory 
                 */
            case FSRD_FSRD_WL_EXT_MEMm:

            /** Below memory represent TCAM entries parity field, may changed once TCAM parity error be reported */
            case PPDB_A_TCAM_ENTRY_PARITYm:
            case PPDB_A_TCAM_ENTRY_PARITY_SMALL_12m:
            case PPDB_A_TCAM_ENTRY_PARITY_SMALL_13m:
            case PPDB_A_TCAM_ENTRY_PARITY_SMALL_14m:
            case PPDB_A_TCAM_ENTRY_PARITY_SMALL_15m:
            /** skip IPS SIGNAL type memory */
            case IPS_MAXQSZm:

#if defined(PLISIM)
                /*
                 * The below memories does not failed on the device. It only failed on emulation or pcid.
                 */
                /*
                 * list of mems failed on pcid bcm88675
                 */
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case EDB_GLEM_MANAGEMENT_REQUESTm:
            case IHB_ISEM_MANAGEMENT_REQUESTm:

                /*
                 * list of problems that found in jericho emulations
                 */
            case ECI_MBU_MEMm:
            case EDB_ESEM_MAA_CAMm:
            case EDB_GLEM_MAA_CAMm:
            case EDB_LINK_LAYER_OR_ARP_NEW_FORMATm:
            case EDB_PROTECTION_ENTRYm:
            case EDB_TRILL_FORMAT_FULL_ENTRYm:
            case EDB_TRILL_FORMAT_HALF_ENTRYm:
            case FDA_FDA_MCm:
            case FDT_IN_BAND_MEMm:
            case FDT_IPT_CTRL_FIFOm:
            case IHB_FEC_ENTRY_ACCESSEDm:
            case IHB_ISEM_ACDT_MASTER_Hm:
            case IHB_ISEM_ACDT_SLAVE_Hm:
            case IHP_FIFO_10_TO_41m:
            case IHP_FIFO_LEM_1m:
            case IHP_FIFO_LEM_2m:
            case IHP_FIFO_LPM_1m:
            case IHP_FIFO_LPM_2m:
            case IHP_FIFO_LPM_PUBLIC_1m:
            case IHP_FIFO_LPM_PUBLIC_2m:
            case IHP_FIFO_OAMm:
            case IHP_FIFO_OID_1m:
            case IHP_FIFO_OID_2m:
            case IHP_FIFO_PROGRAM_ATTRIBUTESm:
            case IHP_FIFO_TCAMm:
            case IHP_LIF_ACCESSEDm:
            case ILKN_PMH_CORE_TX_FIFO_MEMm:
            case ILKN_PMH_PORT_0_CPU_ACCESSm:
            case ILKN_PMH_PORT_1_CPU_ACCESSm:
            case ILKN_PMH_RX_STATS_MEMm:
            case ILKN_PMH_TX_STATS_MEMm:
            case ILKN_PML_CORE_TX_FIFO_MEMm:
            case ILKN_PML_PORT_0_CPU_ACCESSm:
            case ILKN_PML_PORT_1_CPU_ACCESSm:
            case ILKN_PML_RX_STATS_MEMm:
            case ILKN_PML_TX_STATS_MEMm:
            case IPST_IPS_0_CREDIT_ARBITER_FIFOm:
            case IPST_IPS_1_CREDIT_ARBITER_FIFOm:
            case IPS_MEM_200000m:
            case IPS_MEM_240000m:
            case IQMT_BDBLLm:
            case IQMT_FLUSCNTm:
            case IQMT_ING_RPT_PCMm:
            case IQMT_MEM_300000m:
            case IQMT_MNUSCNTm:
            case IQM_PAKCET_DESCRIPTOR_MEMORY_A_DYNAMICm:
            case IQM_PAKCET_DESCRIPTOR_MEMORY_B_DYNAMICm:
            case MMU_FDFm:
            case MRPS_CONTEXT_COLORm:
            case MRPS_CONTEXT_SIZE_HIGHm:
            case MRPS_CONTEXT_SIZE_LOWm:
            case MRPS_MCDA_PCUCm:
            case MRPS_MCDA_PRFCFG_0m:
            case MRPS_MCDA_PRFCFG_1m:
            case MRPS_MCDA_PRFCFG_SHARING_DISm:
            case MRPS_MCDA_PRFCFG_SHARING_ENm:
            case MRPS_MCDA_PRFSELm:
            case MRPS_MCDB_PCUCm:
            case MRPS_MCDB_PRFCFG_0m:
            case MRPS_MCDB_PRFCFG_1m:
            case MRPS_MCDB_PRFSELm:
            case OAMP_MEM_40000m:
            case OAMP_MEM_50000m:
            case OAMP_MEM_60000m:
            case OAMP_MEM_70000m:
            case OAMP_MEM_90000m:
            case OAMP_MEM_1C0000m:
            case OAMP_MEM_C0000m:
            case OAMP_MEM_D0000m:
            case OAMP_REMOTE_MEP_EXACT_MATCH_ACDT_AUXm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_ACDT_Hm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_KEYT_AUXm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_KEYT_PLDT_Hm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_MAA_CAM_PAYLOADm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_MANAGEMENT_MEMORY_Hm:
            case OAMP_REMOTE_MEP_EXACT_MATCH_PLDT_AUXm:
            case OAMP_SAT_RX_FLOW_PARAMSm:
            case OAMP_SAT_RX_FLOW_STATSm:
            case OAMP_SAT_TXm:
            case PPDB_A_OEMA_ACDT_AUXm:
            case PPDB_A_OEMA_ACDT_MASTER_Hm:
            case PPDB_A_OEMA_ACDT_SLAVE_Hm:
            case PPDB_A_OEMA_KEYT_AUXm:
            case PPDB_A_OEMA_KEYT_PLDT_Hm:
            case PPDB_A_OEMA_MAA_CAM_PAYLOADm:
            case PPDB_A_OEMA_MANAGEMENT_MEMORY_Hm:
            case PPDB_A_OEMA_PLDT_AUXm:
            case PPDB_A_OEMB_ACDT_AUXm:
            case PPDB_A_OEMB_ACDT_MASTER_Hm:
            case PPDB_A_OEMB_ACDT_SLAVE_Hm:
            case PPDB_A_OEMB_KEYT_AUXm:
            case PPDB_A_OEMB_KEYT_PLDT_Hm:
            case PPDB_A_OEMB_MAA_CAM_PAYLOADm:
            case PPDB_A_OEMB_MANAGEMENT_MEMORY_Hm:
            case PPDB_A_OEMB_PLDT_AUXm:
            case PPDB_A_TCAM_ACTION_HIT_INDICATIONm:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m:
            case PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m:
            case PPDB_B_LARGE_EM_ACDT_AUXm:
            case PPDB_B_LARGE_EM_ACDT_MASTER_Hm:
            case PPDB_B_LARGE_EM_AGET_AUXm:
            case PPDB_B_LARGE_EM_AGET_Hm:
            case PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm:
            case PPDB_B_LARGE_EM_FID_COUNTER_DBm:
            case PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm:
            case PPDB_B_LARGE_EM_KEYT_AUXm:
            case PPDB_B_LARGE_EM_KEYT_PLDT_Hm:
            case PPDB_B_LARGE_EM_MAA_CAM_PAYLOADm:
            case PPDB_B_LARGE_EM_MANAGEMENT_MEMORY_Hm:
            case PPDB_B_LARGE_EM_PLDT_AUXm:
            case PPDB_B_LARGE_EM_REQUEST_FIFOm:
            case PPDB_B_MACT_ACDT_SLAVE_Hm:
            case SCH_MAX_PORT_QUEUE_SIZE_MPQSm:
            case SCH_MEM_01C00000m:
            case SCH_MEM_04D00000m:
            case SCH_PORT_QUEUE_SIZE_PQSm:
            case SCH_SLOW_FACTOR_MEMORY_SFMm:
            case SER_ACC_TYPE_MAPm:
            case SER_MEMORYm:

                /*
                 * list of dynamic memories from crps,mtrps_em that currently change by the chip * and we wait to
                 * solution from the asic block owner(inbal b. 
                 */
            case CRPS_CRPS_0_CNTS_MEMm:
            case CRPS_CRPS_11_CNTS_MEMm:
            case CRPS_CRPS_12_CNTS_MEMm:
            case CRPS_CRPS_13_CNTS_MEMm:
            case CRPS_CRPS_15_CNTS_MEMm:
            case CRPS_CRPS_17_CNTS_MEMm:
            case CRPS_CRPS_1_CNTS_MEMm:
            case CRPS_CRPS_9_CNTS_MEMm:
            case MTRPS_EM_CONTEXT_COLORm:
            case MTRPS_EM_CONTEXT_SIZE_HIGHm:
            case MTRPS_EM_CONTEXT_SIZE_LOWm:
            case MTRPS_EM_MCDA_PCUCm:
            case MTRPS_EM_MCDB_PCUCm:
#endif

                return 1;
        }

        if (SOC_IS_QAX(unit))
        {

            /*
             * missed blocks at emulation 2 boards
             */
            if (block == SOC_BLK_NBIL || block == SOC_BLK_NBIH)
            {
                return 1;
            }
            switch (mem)
            {

                    /*
                     * This is Write-Mask dynamic memory so write/read access to it is different them all regular
                     * memories (need to set the write mask field in the indirect registers).. 
                     */
                case CGM_CNI_STATUSm:

                    /*
                     * skip SIGNAL type memory 
                     */
                case CGM_SYS_RED_SIZEm:

                    /*
                     * They should have bus's to be accessible by sbus - but they are memories used by our FIFOs and so 
                     * the cpu shouldn't really access them.. 
                     */
                case IPSEC_SPU_WRAPPER_TOP_SPU_INPUT_FIFO_MEM_Hm:
                case IPSEC_SPU_WRAPPER_TOP_SPU_OUTPUT_FIFO_MEM_Hm:

                    /*
                     * These are internal memories and I don't expect users to access. 
                     */
                case IPS_MEM_2A0000m:
                case IPS_MEM_2E0000m:
                case OAMP_REMOTE_MEP_EXACT_MATCH_ACDT_Hm:

                    /*
                     * skip IPS SIGNAL type memory 
                     */
                case IPS_CRBALm:

                case OAMP_FLEX_VER_MASK_TEMPm:

                    /*
                     * IEP/IMP - access error after meter initialization 
                     */
                case IEP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm:
                case IEP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm:
                case IMP_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm:
                case IMP_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm:

                    /*
                     * IEP/IMP under asic investigation 9/9/15 
                     */
                    /*
                     * It is dynamic memory and I can't think of any reason to access it.(Inbal) 
                     */
                case IMP_CONTEXT_SIZE_HIGHm:
                case IMP_CONTEXT_SIZE_LOWm:
                case IMP_MCDA_PCUCm:
                case IMP_MCDB_PCUCm:

                case IEP_CONTEXT_SIZE_HIGHm:
                case IEP_CONTEXT_SIZE_LOWm:
                case IEP_MCDA_PCUCm:
                case IEP_MCDB_PCUCm:

                case IEP_MCDA_DYNAMICm:
                case IEP_MCDB_DYNAMICm:
                case IMP_MCDA_DYNAMICm:
                case IMP_MCDB_DYNAMICm:
                case MMU_RAFA_RADDR_STATUSm:
                case MMU_RAFB_RADDR_STATUSm:
                case MMU_RAFC_RADDR_STATUSm:

                    /*
                     * for MMU_DRAM_ADDRESS_SPACE you will need a special support/ramp up from the mmu tram + sw mmu
                     * team - you can sync with them 
                     */
                case MMU_DRAM_ADDRESS_SPACEm:

                    /*
                     * not real memories (need special config) 
                     */
                case MMU_RAF_WADDRm:
                case MMU_RDFA_WADDR_STATUSm:
                case MMU_RDFB_WADDR_STATUSm:
                case MMU_RDFC_WADDR_STATUSm:
                case MMU_RDF_RADDRm:
                case MMU_WAFA_HALFA_RADDR_STATUSm:
                case MMU_WAFA_HALFB_RADDR_STATUSm:
                case MMU_WAFB_HALFA_RADDR_STATUSm:
                case MMU_WAFB_HALFB_RADDR_STATUSm:
                case MMU_WAFC_HALFA_RADDR_STATUSm:
                case MMU_WAFC_HALFB_RADDR_STATUSm:
                case MMU_WAF_HALFA_WADDRm:
                case MMU_WAF_HALFB_WADDRm:

                    /*
                     * for now PPDB tables you will need to wait for the chip to return to the lab since we don't have
                     * TCAM in emulation 
                     */
                case PPDB_A_ISEM_ACDT_Hm:
                case PPDB_A_ISEM_MAA_CAMm:
                case PPDB_A_ISEM_MANAGEMENT_REQUESTm:
                case PPDB_B_ISEM_ACDT_Hm:
                case PPDB_B_ISEM_MAA_CAMm:
                case PPDB_B_ISEM_MANAGEMENT_REQUESTm:
                case PPDB_B_LARGE_EM_ACDT_Hm:
                    /*
                     * Description: Counters per reassembly context, read from here also clear the counters.
                     * __REGGEN_NL__ Write is not working, ignored. 
                     */
                case SPB_COUNTERS_SHADOWm:
                    return 1;   /* Skip these tables */
                case XLPORT_WC_UCMEM_DATAm:
                case EPNI_PROTECTION_PTR_TABLEm:
                case IPS_CNT_DRAM_CAMm:
                case IPS_CNT_FABRIC_CAMm:
                    if (SOC_IS_QUX(unit))
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                default:
                    break;
            }
        }
    }

    if (SOC_IS_ARDON(unit))
    {
        switch (mem)
        {
            case MMU_RDFA_WADDR_STATUSm:
            case MMU_RDFB_WADDR_STATUSm:
            case MMU_RDFC_WADDR_STATUSm:
            case MMU_RDFD_WADDR_STATUSm:
            case MMU_WAF_HALFA_WADDRm:
            case MMU_WAF_HALFB_WADDRm:

                /*
                 * Schan TO on Write 
                 */
            case ECI_MBU_MEMm:
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case EDB_ESEM_STEP_TABLEm:
            case EPNI_ESEM_MANAGEMENT_REQUESTm:
            case EPNI_ESEM_STEP_TABLEm:
            case EPNI_MEM_520000m:
            case EPNI_MEM_630000m:
            case EPNI_MEM_640000m:
            case EPNI_MEM_760000m:
            case EPNI_MEM_770000m:
            case EPNI_MEM_5A0000m:
            case EPNI_MEM_5B0000m:
            case EPNI_MEM_6C0000m:
            case EPNI_MEM_6D0000m:

                /*
                 * NonExisting Tabels (DOC) 
                 */
            case IHB_DESTINATION_STATUSm:
            case IHB_FEC_ECMPm:
            case IHB_FEC_ENTRYm:
            case IHB_FEC_ENTRY_ACCESSEDm:
            case IHB_FEC_ENTRY_FORMAT_Am:
            case IHB_FEC_ENTRY_FORMAT_Bm:
            case IHB_FEC_ENTRY_FORMAT_Cm:
            case IHB_FEC_ENTRY_FORMAT_NULLm:
            case IHB_FEC_ENTRY_GENERALm:
            case IHB_FEC_SUPER_ENTRYm:
            case IHB_PATH_SELECTm:
                return 1;       /* Skip these tables */
            default:
                break;
        }
    }
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_JERICHO2_A0(unit))
    {
        /**
         * to exclude the MDB accesses from the tr 7 test on Jr2. All the tables that starts with DDHB_MACRO. 
         * the memories started with  DDHB_MACRO cant by accessed 
         * directly(Ely) 
         */
        if (sal_strstr(SOC_MEM_NAME(unit, mem), "DDHB_MACRO") != NULL)
        {
            return 1;
        }
        switch (mem)
        {
            case IPPB_MEM_1E10000m:
            case IPPB_MEM_1E00000m:
            case IPPB_MEM_1E20000m:
            case IPPE_MEM_570000m:
            case MACT_LARGE_EM_MANAGEMENT_REQUEST_FIFOm:
            case OAMP_ITR_PKT_FIFOm:
            case OAMP_PE_0_FDBK_FIFOm:
            case OAMP_PE_1_FDBK_FIFOm:
            case OAMP_RSP_FIRST_DATA_FIFOm:
            case OAMP_RSP_OUT_DATA_FIFOm:
            case OAMP_RXB_INPUT_DATA_FIFOm:
            case OAMP_RXB_OUTPUT_DATA_FIFOm:
            case DDHA_MACRO_0_ENTRY_BANKm:
            case DDHA_MACRO_1_ENTRY_BANKm:
            case DHC_MACRO_ENTRY_BANKm:
            case ERPP_INITIAL_ACTIONS_FIFOm:
            case IPPA_VTDCS_TCAM_BANKm:
            case IPPA_VTECS_TCAM_BANKm:
            case IPPB_FLPACS_TCAM_BANKm:
            case IPPB_FLPBCS_TCAM_BANKm:
            case IPPC_EVENTOR_FIFOm:
            case IPPE_PRTCAM_TCAM_BANKm:
                return 1;       /* Skip these tables */
            default:
                break;
        }
    }
#endif

#endif

    return 0;
}

#if defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
int
rval_test_skip_mem_flipflop(
    int unit,
    soc_mem_t mem,
    tr8_dbase_t * tr8_ptr)
{
#if defined (BCM_PETRA_SUPPORT)
    if (SOC_IS_ARAD(unit))
    {

        if (mem_too_large_for_test(unit, mem, tr8_ptr->max_size))
        {
            return 1;
        }

        if ((mem < tr8_ptr->start_from || mem > tr8_ptr->start_from + tr8_ptr->count))
        {
            return 1;
        }

        switch (mem)
        {
                /*
                 * This is since the TCAM isn't a memory, it is an IP instanced in the block and its data need to be
                 * formatted in a specific way in order to implement either read or write actions. If you want you can
                 * format the data according to the description found in the "TcamBankCommand" in the excel at:
                 * /projects/JERICHO_FE/A0/Jericho_v10/design/ipp/ppdb/data/PPDB_TCAM_register_definition.xlsm
                 * 
                 * Thanks Ofer L. 
                 */
            case PPDB_A_TCAM_BANKm:

            case BRDC_FSRD_FSRD_WL_EXT_MEMm:
            case PORT_WC_UCMEM_DATAm:
            case EGQ_CBMm:
            case IHB_MEM_230000m:
            case RTP_UNICAST_DISTRIBUTION_MEMORYm:
            case IHB_DBG_LAST_FEMm:
            case IHB_DBG_LAST_FESm:
            case IPS_MEM_260000m:
            case IPS_MEM_280000m:
            case IPT_EGQ_TXQ_RD_ADDRm:
            case IPT_EGQ_TXQ_WR_ADDRm:
            case IPT_FDT_TXQ_RD_ADDRm:
            case IPT_FDT_TXQ_WR_ADDRm:
            case IQM_CNG_QUE_SETm:
            case IQM_MEM_1400000m:
            case IQM_MEM_1600000m:
            case NBI_RBINS_MEMm:
            case NBI_RLENG_MEMm:
            case NBI_RPKTS_MEMm:
            case NBI_RTYPE_MEMm:
            case NBI_TBINS_MEMm:
            case NBI_TLENG_MEMm:
            case NBI_TPKTS_MEMm:
            case NBI_TTYPE_MEMm:
            case SCH_BUCKET_DEFICIT__BDFm:
            case SCH_FLOW_INSTALLED_MEMORY__FIMm:
            case SCH_FLOW_STATUS_MEMORY__FSMm:
            case SCH_MEM_01300000m:
            case SCH_MEM_01400000m:
            case SCH_MEM_01500000m:
            case SCH_MEM_01600000m:
            case SCH_MEM_01700000m:
            case SCH_MEM_01800000m:
            case SCH_MEM_01900000m:
            case SCH_MEM_01A00000m:
            case SCH_MEM_01B00000m:
            case SCH_MEM_01C00000m:
            case SCH_MEM_03000000m:
            case SCH_MEM_03100000m:
            case SCH_MEM_03200000m:
            case SCH_MEM_03300000m:
            case SCH_MEM_03400000m:
            case SCH_MEM_03500000m:
            case SCH_SCHEDULER_ENABLE_MEMORY_SEM_Bm:
            case CRPS_CRPS_0_CNTS_MEMm:
            case CRPS_CRPS_1_CNTS_MEMm:
            case CRPS_CRPS_2_CNTS_MEMm:
            case CRPS_CRPS_3_CNTS_MEMm:
            case ECI_MBU_MEMm:
            case EGQ_RDMMCm:
            case EGQ_RDMUCm:
            case FCR_FCR_CRM_Am:
            case FCR_FCR_CRM_Bm:
            case IDR_COMPLETE_PCm:
            case IDR_MEM_100000m:
            case IDR_MEM_110000m:
            case IDR_MEM_120000m:
            case IDR_MEM_140000m:
            case IDR_MEM_1F0000m:
            case IHB_OEMA_MANAGEMENT_REQUESTm:
            case IHB_OEMB_MANAGEMENT_REQUESTm:
            case IHB_TCAM_BANKm:
            case IHP_MACT_FLUSH_DBm:
            case IHP_MEM_430000m:
            case IPS_MAXQSZm:
            case IQM_VSQ_A_MX_OCm:
            case IQM_VSQ_B_MX_OCm:
            case IQM_VSQ_C_MX_OCm:
            case IQM_VSQ_D_MX_OCm:
            case IQM_VSQ_E_MX_OCm:
            case IQM_VSQ_F_MX_OCm:
            case NBI_MLF_RX_MEM_A_CTRLm:
            case NBI_MLF_RX_MEM_B_CTRLm:
            case OAMP_RMAPEM_MANAGEMENT_REQUESTm:
            case FSRD_FSRD_WL_EXT_MEMm:
            case IDR_CONTEXT_COLORm:
            case IDR_CONTEXT_SIZEm:
            case IDR_MEM_1B0000m:
            case IHB_TCAM_ACTIONm:
            case IHB_TCAM_ACTION_24m:
            case IHB_TCAM_ACTION_25m:
            case IHB_TCAM_ACTION_26m:
            case IHB_TCAM_ACTION_27m:
            case IHB_TCAM_ACTION_HIT_INDICATIONm:
            case IHB_TCAM_ENTRY_PARITYm:
            case IHB_TCAM_ENTRY_PARITY_12m:
            case IHB_TCAM_ENTRY_PARITY_13m:
            case IHP_MEM_420000m:
            case IPS_MEM_200000m:
            case IQM_MEM_8000000m:
            case MMU_DRAM_ADDRESS_SPACEm:
            case MMU_RAFA_RADDR_STATUSm:
            case MMU_RAFB_RADDR_STATUSm:
            case MMU_RAFC_RADDR_STATUSm:
            case MMU_RAFD_RADDR_STATUSm:
            case MMU_RAFE_RADDR_STATUSm:
            case MMU_RAFF_RADDR_STATUSm:
            case MMU_RAFG_RADDR_STATUSm:
            case MMU_RAFH_RADDR_STATUSm:
            case MMU_WAFA_HALFA_RADDR_STATUSm:
            case MMU_WAFA_HALFB_RADDR_STATUSm:
            case MMU_WAFB_HALFA_RADDR_STATUSm:
            case MMU_WAFB_HALFB_RADDR_STATUSm:
            case MMU_WAFC_HALFA_RADDR_STATUSm:
            case MMU_WAFC_HALFB_RADDR_STATUSm:
            case MMU_WAFD_HALFA_RADDR_STATUSm:
            case MMU_WAFD_HALFB_RADDR_STATUSm:
            case MMU_WAFE_HALFA_RADDR_STATUSm:
            case MMU_WAFE_HALFB_RADDR_STATUSm:
            case MMU_WAFF_HALFA_RADDR_STATUSm:
            case MMU_WAFF_HALFB_RADDR_STATUSm:
            case MMU_WAFG_HALFA_RADDR_STATUSm:
            case MMU_WAFG_HALFB_RADDR_STATUSm:
            case MMU_WAFH_HALFA_RADDR_STATUSm:
            case MMU_WAFH_HALFB_RADDR_STATUSm:
            case MMU_RAF_WADDRm:
            case MMU_RDF_RADDRm:
            case MMU_WAF_HALFA_WADDRm:
            case MMU_WAF_HALFB_WADDRm:
            case SCH_PORT_QUEUE_SIZE__PQSm:
            case SCH_MEM_04A00000m:
            case SCH_MEM_04700000m:
            case SCH_PORT_ENABLE__PORTENm:
            case SCH_PORT_GROUP_PFGMm:
            case SCH_CH_NIF_CALENDAR_CONFIGURATION__CNCCm:
            case SCH_CH_NIF_RATES_CONFIGURATION__CNRCm:
            case SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm:
            case SCH_CIR_SHAPER_CALENDAR__CSCm:
            case SCH_PIR_SHAPER_CALENDAR__PSCm:
            case SCH_PORT_SCHEDULER_MAP__PSMm:
            case SCH_SCHEDULER_INITm:
            case SCH_FORCE_STATUS_MESSAGEm:
            case EPNI_AC_FORMATm:
            case EPNI_DATA_FORMATm:
            case EPNI_IPV4_TUNNEL_FORMATm:
            case EPNI_LINK_LAYER_OR_ARP_FORMATm:
            case EPNI_MPLS_POP_FORMATm:
            case EPNI_MPLS_PUSH_FORMATm:
            case EPNI_MPLS_SWAP_FORMATm:
            case EPNI_OUT_RIF_FORMATm:
            case EPNI_TRILL_FORMATm:
            case IDR_MCDA_PRFCFG_0m:
            case IDR_MCDA_PRFCFG_FORMAT_1m:
            case IHB_FEC_ENTRY_FORMAT_Am:
            case IHB_FEC_ENTRY_FORMAT_Bm:
            case IHB_FEC_ENTRY_FORMAT_Cm:
            case IHB_FEC_ENTRY_FORMAT_NULLm:
            case IHB_FEC_ENTRY_GENERALm:
            case IHP_MACT_FORMAT_0_TYPE_0m:
            case IHP_MACT_FORMAT_0_TYPE_1m:
            case IHP_MACT_FORMAT_0_TYPE_2m:
            case IHP_MACT_FORMAT_1m:
            case IHP_MACT_FORMAT_2m:
            case IHP_LIF_TABLE_AC_MPm:
            case IHP_LIF_TABLE_AC_P2P_TO_ACm:
            case IHP_LIF_TABLE_AC_P2P_TO_PBBm:
            case IHP_LIF_TABLE_AC_P2P_TO_PWEm:
            case IHP_LIF_TABLE_IP_TTm:
            case IHP_LIF_TABLE_ISID_MPm:
            case IHP_LIF_TABLE_ISID_P2Pm:
            case IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm:
            case IHP_LIF_TABLE_LABEL_PWE_MPm:
            case IHP_LIF_TABLE_LABEL_PWE_P2Pm:
            case IHP_LIF_TABLE_TRILLm:
            case IPS_QPM_1_NO_SYS_REDm:
            case IPS_QPM_2_SYS_REDm:
            case IPS_CRBALm:
            case IPS_QDESCm:
            case IPT_MEM_80000m:
            case IRR_MCDB_EGRESS_FORMAT_0m:
            case IRR_MCDB_EGRESS_FORMAT_1m:
            case IRR_MCDB_EGRESS_FORMAT_2m:
            case IRR_MCDB_EGRESS_FORMAT_4m:
            case IRR_MCDB_EGRESS_FORMAT_5m:
            case IRR_MCDB_EGRESS_FORMAT_6m:
            case IRR_MCDB_EGRESS_FORMAT_7m:
            case IRR_MCDB_EGRESS_SPECIAL_FORMATm:
            case IRR_MCDB_EGRESS_TDM_FORMATm:
            case OAMP_MEP_DBm:
            case OAMP_MEP_DB_BFD_CC_ON_MPLSTPm:
            case OAMP_MEP_DB_BFD_CV_ON_MPLSTPm:
            case OAMP_MEP_DB_BFD_ON_IPV4_ONE_HOPm:
            case OAMP_MEP_DB_BFD_ON_MPLSm:
            case OAMP_MEP_DB_BFD_ON_PWEm:
            case OAMP_MEP_DB_Y_1731_ON_MPLSTPm:
            case OAMP_MEP_DB_Y_1731_ON_PWEm:
            case EPNI_ESEM_MANAGEMENT_REQUESTm:
                return 1;       /* Skip these mems */
            default:
                break;
        }
    }

    if (SOC_IS_JERICHO(unit))
    {
        soc_block_type_t block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
        if (!tr8_ptr->include_port_macros)
        {
            if (block_can_be_disabled(block))
            {
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Jericho excluded block %s\n"), SOC_BLOCK_NAME(unit, mem)));
                return 1;
            }
        }

        if (mem_in_skip_list_for_jericho_emulation(unit, mem))
        {
            return 1;
        }

        /*
         * need to check why iproc mems doesnt have an access on emu while iproc regs have
         */
        if (block == SOC_BLK_IPROC)
        {
            return 1;
        }

        if (soc_mem_is_signal(unit, mem) || soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem))
        {
            return 1;
        }

    }

    if (SOC_IS_ARDON(unit))
    {
        switch (mem)
        {
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case EDB_ESEM_STEP_TABLEm:
            case EPNI_MEM_520000m:
            case EPNI_MEM_630000m:
            case EPNI_MEM_640000m:
            case EPNI_MEM_760000m:
            case EPNI_MEM_770000m:
            case EPNI_MEM_5A0000m:
            case EPNI_MEM_5B0000m:
            case EPNI_MEM_6C0000m:
            case EPNI_MEM_6D0000m:
            case IHB_DESTINATION_STATUSm:
            case IHB_FEC_ECMPm:
            case IHB_FEC_ENTRYm:
            case IHB_FEC_ENTRY_ACCESSEDm:
            case IHB_FEC_SUPER_ENTRYm:
            case IHB_PATH_SELECTm:
            case PPDB_A_TCAM_BANKm:
                return 1;       /* Skip these mems */

        }
    }

    if (SOC_IS_QAX(unit))
    {
        /**
         * all this memories aliased to OAMP_SAT_RX_FLOW_STATS 
         * This table is continuously accessed by the design and 
         * probably this is the reason CPU cannot write it. Anyway, this 
         * table should not be written by the CPU 
         * Boris Kapchits 
         */
        switch (mem)
        {
            case OAMP_FLOW_STAT_ACCUM_ENTRY_1m:
            case OAMP_FLOW_STAT_ACCUM_ENTRY_2m:
            case OAMP_FLOW_STAT_ACCUM_ENTRY_3m:
            case OAMP_FLOW_STAT_ACCUM_ENTRY_4m:
                return 1;       /* Skip these mems */

        }
    }
#endif
    return 0;
}

/**
 *  check if memory is real memory
 *  or alias/format
 *  (we skip on alias/format mems in tr6-8 due to  inconsistency
 *  between main memory attributes && alias/format atrributes
 *  that failed the test
 *
 * @author elem (17/12/2014)
 *
 * @param unit
 * @param mem
 *
 * @return STATIC int
 */
#endif
STATIC int
is_main_mem(
    int unit,
    soc_mem_t mem)
{
    soc_mem_t orig_mem = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);
    return orig_mem == mem ? 1 : 0;
}
#if defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)

void
memtest_mask_get_tr8(
    int unit,
    soc_mem_t mem,
    uint32 *mask)
{
    uint32 *tcammask = NULL;
    uint32 *eccmask = NULL;
    uint32 accum_tcammask;
    int dw, i;

    if ((tcammask = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "tcammask")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "dpp_mem_test_flipflop_iter_read_compare_callback: tcammask : Out of memory\n")));
        goto done;
    }
    if ((eccmask = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "eccmask")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "dpp_mem_test_flipflop_iter_read_compare_callback: eccmask : Out of memory\n")));
        goto done;
    }

    for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++)
    {
        mask[i] = 0;
        tcammask[i] = 0;
        eccmask[i] = 0;
    }
    dw = soc_mem_entry_words(unit, mem);

    soc_mem_datamask_rw_get(unit, mem, mask);
    soc_mem_tcammask_get(unit, mem, tcammask);
    soc_mem_eccmask_get(unit, mem, eccmask);
    accum_tcammask = 0;
    for (i = 0; i < dw; i++)
    {
        accum_tcammask |= tcammask[i];
    }

    for (i = 0; i < dw; i++)
    {
        mask[i] &= ~eccmask[i];
    }
    /*
     * exceptions:
     */
    switch (mem)
    {
        case IQM_PDMm:
            mask[0] = 0xffffffff;
            mask[1] = 0x1fc001;
            break;
        case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm:
        case SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm:
            mask[0] = 0xffffffff;
            mask[1] = 0xff;
            break;
        default:
            break;

    }
done:
    if (tcammask != NULL)
    {
        sal_free(tcammask);
    }
    if (eccmask != NULL)
    {
        sal_free(eccmask);
    }

}

int
sand_mem_test_flipflop_iter_read_compare_callback(
    int unit,
    soc_mem_t mem,
    void *data)
{
    int rv = SOC_E_NONE;
    char print_str[250];
    uint32 *mask = NULL;
    int i_array, i_index, i_bytes;
    unsigned int start_array_index, end_array_index, start_index, end_index;
    uint32 *data_read = NULL;
    tr8_dbase_t *tr8_ptr = (tr8_dbase_t *) data;
    uint32 *data_write_compare = NULL;
    uint32 incr_run_count = tr8_ptr->incr_run_count;
    uint32 error = 0;
    int fault_inject = tr8_ptr->fault_inject;
    uint32 compare_value;

    start_index = parse_memory_index(unit, mem, "min");
    end_index = parse_memory_index(unit, mem, "max");

    /*
     * since we test access to memories and since access to memory is the same if it is main table or alias/format we
     * skip over alias/format tables to reduce the tr time specially on the slow emulation 
     */

    if (!is_main_mem(unit, mem))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "Skipping alias/format memory %s \n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /*
     * Memory is valid for this test if it is valid, and not ro / wo. 
     */
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        return rv;
    }

    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /*
     * skipping excluded memories 
     */
    if (rval_test_skip_mem_flipflop(unit, mem, tr8_ptr) == 1)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    /*
     * skipping memories larger than 640b 
     */
    if (2 + soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit))
    {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        sal_sprintf(print_str, "skipping large size memory %s\n", SOC_MEM_NAME(unit, mem));
        tr8_write_dump(print_str);
        return rv;
    }

    /*
     * skipping memories with less or equal to 16 entries (hw request, most of it is not memories but management blocks
     */
    if (end_index - start_index <= 16)
    {
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit, "skipping memory < 16 entries %s, end index=%d, start index=%d\n"),
                  SOC_MEM_NAME(unit, mem), end_index, start_index));
        return rv;
    }

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }
    else
    {
        start_array_index = 0;
        end_array_index = 0;
    }

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Reading memory: table %s\n"), SOC_MEM_UFNAME(unit, mem)));
    sal_sprintf(print_str, "Reading memory: table %s\n", SOC_MEM_UFNAME(unit, mem));
    tr8_write_dump(print_str);

    if ((mask = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "mask")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "sand_mem_test_flipflop_iter_read_compare_callback: mask : Out of memory\n")));
        rv = -1;
        goto done;
    }
    memtest_mask_get_tr8(unit, mem, mask);

    if ((data_read = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "data_read")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "sand_mem_test_flipflop_iter_read_compare_callback: data_read : Out of memory\n")));
        rv = -1;
        goto done;
    }
    if ((data_write_compare = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "data_write_compare")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U
                  (unit, "sand_mem_test_flipflop_iter_read_compare_callback: data_write_compare : Out of memory\n")));
        rv = -1;
        goto done;
    }
#if defined (BCM_PETRA_SUPPORT)
    soc_sand_os_memset(data_read, 0x0, CMIC_SCHAN_WORDS(unit) * sizeof(uint32));
    soc_sand_os_memset(data_write_compare, tr8_ptr->write_value, CMIC_SCHAN_WORDS(unit) * sizeof(uint32));
#endif
    for (i_array = start_array_index; i_array <= end_array_index; i_array++)
    {
        for (i_index = start_index; i_index <= end_index; i_index++)
        {
            rv = soc_mem_array_read(unit, mem, i_array, COPYNO_ALL, i_index, data_read);
            if (rv < 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Read ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                          SOC_BLOCK_NAME(unit, COPYNO_ALL), i_index, soc_errmsg(rv)));
                goto done;
            }
            for (i_bytes = 0; i_bytes < SOC_MEM_WORDS(unit, mem); i_bytes++)
            {
                tr8_increment_bits_counter(tr8_get_bits_num(mask[i_bytes]));
                if ((fault_inject == 1) && (i_index == start_index))
                {
                    compare_value = data_write_compare[i_bytes] + 1;
                }
                else
                {
                    compare_value = data_write_compare[i_bytes];
                }
                if ((data_read[i_bytes] & mask[i_bytes]) != (compare_value & mask[i_bytes]))
                {
                    sal_sprintf(print_str,
                                "Compare error iteration number:%d in %s, array index: %d, entry index:%d, current word:%d total words:%d\n",
                                incr_run_count, SOC_MEM_UFNAME(unit, mem), i_array, i_index, i_bytes,
                                SOC_MEM_WORDS(unit, mem));
                    tr8_write_dump(print_str);
                    cli_out(print_str);
                    sal_sprintf(print_str, "Data write: 0x%x\nData read: 0x%x\nData mask: 0x%x\n",
                                compare_value, data_read[i_bytes], mask[i_bytes]);
                    tr8_write_dump(print_str);
                    cli_out(print_str);
                    error = 1;
                    goto count;
                }
            }
        }
    }
count:
    tr8_ptr->total_count++;
    tr8_ptr->error_count += error;
done:
    if (mask != NULL)
    {
        sal_free(mask);
    }
    if (data_read != NULL)
    {
        sal_free(data_read);
    }
    if (data_write_compare != NULL)
    {
        sal_free(data_write_compare);
    }

    return rv;
}
#endif
int
sand_mem_test_flipflop_iter_callback(
    int unit,
    soc_mem_t mem,
    void *data)
{
    int rv = SOC_E_NONE;
#if defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    int i_array, i_index;
    char print_str[250];
    unsigned int start_array_index, end_array_index, start_index, end_index;
    uint32 *data_write = NULL;
    tr8_dbase_t *tr8_ptr = (tr8_dbase_t *) data;
    int fault_inject = tr8_ptr->fault_inject;
    uint32 *data_write_inject = NULL;

    if ((data_write = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "data_write")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "sand_mem_test_flipflop_iter_callback : Out of memory\n")));
        rv = -1;
        goto done;
    }
    /*
     * since we test access to memories and since access to memory is the same if it is main table or alias/format we
     * skip over alias/format tables to reduce the tr time specially on the slow emulation 
     */
    if (!is_main_mem(unit, mem))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "Skipping alias/format memory %s \n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }
    sal_memset(data_write, tr8_ptr->write_value, CMIC_SCHAN_WORDS(unit) * sizeof(uint32));

    start_index = parse_memory_index(unit, mem, "min");
    end_index = parse_memory_index(unit, mem, "max");

    /*
     * Memory is valid for this test if it is valid, and not ro / wo. 
     */
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        goto done;
    }

    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    /*
     * skipping excluded memories 
     */
    if (rval_test_skip_mem_flipflop(unit, mem, tr8_ptr) == 1)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    /*
     * skipping memories larger than 640b 
     */
    if (2 + soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        goto done;
    }

    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        start_array_index = parse_memory_array_index(unit, mem, "min");
        end_array_index = parse_memory_array_index(unit, mem, "max");
    }
    else
    {
        start_array_index = 0;
        end_array_index = 0;
    }

    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Writing memory: table %s\n"), SOC_MEM_UFNAME(unit, mem)));
    sal_sprintf(print_str, "Writing memory: table %s\n", SOC_MEM_UFNAME(unit, mem));
    tr8_write_dump(print_str);

    if ((data_write_inject = sal_alloc(CMIC_SCHAN_WORDS(unit) * sizeof(uint32), "data_write_inject")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "sand_mem_test_flipflop_iter_callback , data_write_inject: Out of memory\n")));
        rv = -1;
        goto done;
    }

    for (i_array = start_array_index; i_array <= end_array_index; i_array++)
    {
        for (i_index = start_index; i_index <= end_index; i_index++)
        {
            if ((fault_inject == 1) && (i_index == start_index))
            {
                int i;
                for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++)
                {
                    data_write_inject[i] = data_write[i] + 1;
                }
                rv = soc_mem_array_write(unit, mem, i_array, COPYNO_ALL, i_index, data_write_inject);
            }
            else
            {
                rv = soc_mem_array_write(unit, mem, i_array, COPYNO_ALL, i_index, data_write);
            }

            if (rv < 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Write ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                          SOC_BLOCK_NAME(unit, COPYNO_ALL), i_index, soc_errmsg(rv)));
                goto done;
            }
        }
    }
done:
    if (data_write != NULL)
    {
        sal_free(data_write);
    }
    if (data_write_inject != NULL)
    {
        sal_free(data_write_inject);
    }

#endif
    return rv;
}

#if defined (BCM_PETRA_SUPPORT) || defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
cmd_result_t
do_mem_test_flipflop(
    int unit,
    tr8_dbase_t tr8_data,
    args_t * a)
{
    int rv = SOC_E_NONE;
    char print_str[250];
    if (tr8_data.skip_reset_and_write != 1)
    {
        sal_sprintf(print_str, "****************Write pattern to all writable memories...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);

        if ((soc_mem_iterate(unit, sand_mem_test_flipflop_iter_callback, &tr8_data)) < 0)
        {
            rv = CMD_FAIL;
        }
    }
    else
    {
        sal_sprintf(print_str, "****************Skip Write pattern...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);
    }

    tr8_data.incr_run_count = 1;
    while (tr8_data.run_count > 0)
    {
        sal_sprintf(print_str, "****************Sleep...\n");
        cli_out(print_str);
        tr8_write_dump(print_str);

        sal_sleep(tr8_data.period);

        sal_sprintf(print_str, "****************Read & compare pattern to all writable memories, Iteration #%d...\n",
                    tr8_data.incr_run_count);
        cli_out(print_str);
        tr8_write_dump(print_str);

        tr8_reset_bits_counter();
        if ((soc_mem_iterate(unit, sand_mem_test_flipflop_iter_read_compare_callback, &tr8_data)) < 0)
        {
            rv = CMD_FAIL;
        }
        else
        {
            uint32 bit_count_num = tr8_bits_counter_get();
            sal_sprintf(print_str,
                        "Iteration #%d : Bits processed : %u Mems Total(%u) Errors(%d) Unclear skipped(%d) \n",
                        tr8_data.incr_run_count, bit_count_num, tr8_data.total_count, tr8_data.error_count,
                        tr8_data.unclear_skipped_count / 2);
            cli_out(print_str);
            tr8_write_dump(print_str);
            rv = tr8_data.error_count ? CMD_FAIL : CMD_OK;

        }
        if (rv == CMD_FAIL)
        {
            break;
        }
        tr8_data.run_count--;
    }
    return rv;
}
#endif /* dpp support */

/* TR 7
 * Memories read/write for first and last memory index test 
 */
STATIC int
sand_mem_test_rw_iter_callback(
    int unit,
    soc_mem_t mem,
    void *data)
{
    int rv = SOC_E_NONE;
    void *p;
    tr7_dbase_t *tr7_ptr = (tr7_dbase_t *) data;

    /*
     * since we test access to memories and since access to memory is the same if it is main table or alias/format we
     * skip over alias/format tables to reduce the tr time specially on the slow emulation 
     */
    if (!is_main_mem(unit, mem))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "Skipping alias/format memory %s \n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }
    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping debug memory %s\n"), SOC_MEM_NAME(unit, mem)));
    /*
     * Validity check for the memory for this test - as follows 
     */
    if (tr7_ptr->enable_skip == 1)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit, "Memory %s is not valid for device.\n"), SOC_MEM_NAME(unit, mem)));
            return SOC_E_INTERNAL;
        }

        test_run_memory_stat.mem_valid_num++;

        if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) != 0)
        {
            LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping debug memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0)
        {
            LOG_VERBOSE(BSL_LS_APPL_TESTS,
                        (BSL_META_U(unit, "Skipping read only memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0)
        {
            LOG_VERBOSE(BSL_LS_APPL_TESTS,
                        (BSL_META_U(unit, "Skipping write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }

        if (rval_test_skip_mem(unit, mem, tr7_ptr) == 1)
        {
            LOG_VERBOSE(BSL_LS_APPL_TESTS,
                        (BSL_META_U(unit, "Skipping excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
            return rv;
        }
    }

    /*
     * Skipping memories larger than 640b 
     */
    if (2 + soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Testing memory %s\n"), SOC_MEM_NAME(unit, mem)));
    test_run_memory_stat.mem_test_num++;
    if (mem_test_rw_init(unit, mem, &p) < 0)
    {
        test_run_memory_stat.mem_fail_num++;
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "mem_test_rw_init() fail for %s\n"), SOC_MEM_NAME(unit, mem)));
        return CMD_FAIL;
    }
    if (mem_test(unit, data, p) < 0)
    {
        test_run_memory_stat.mem_fail_num++;
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Test fail for %s\n"), SOC_MEM_NAME(unit, mem)));
        rv = CMD_FAIL;
    }
    if (mem_test_done(unit, p) < 0)
    {
        test_run_memory_stat.mem_fail_num++;
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Mem_test_done() fail for %s\n"), SOC_MEM_NAME(unit, mem)));
        return CMD_FAIL;
    }

    if (rv == SOC_E_NONE)
    {
        test_run_memory_stat.mem_pass_num++;
    }

    return rv;
}

STATIC void
diag_sand_check_global(
    int unit,
    soc_mem_t mem,
    char *s,
    int *is_global)
{
    soc_field_info_t *fld;
    soc_mem_info_t *m = &SOC_MEM_INFO(unit, mem);
    char *eqpos;

    eqpos = strchr(s, '=');
    if (eqpos != NULL)
    {
        *eqpos++ = 0;
    }
    for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++)
    {
        if (!sal_strcasecmp(s, SOC_FIELD_NAME(unit, fld->field)) && (fld->flags & SOCF_GLOBAL))
        {
            break;
        }
    }
    if (fld == &m->fields[m->nFields])
    {
        *is_global = 0;
    }
    else
    {
        *is_global = 1;
    }
}

STATIC int
diag_sand_collect_args_with_view(
    args_t * a,
    char *valstr,
    char *first,
    char *view,
    int unit,
    soc_mem_t mem)
{
    char *s, *s_copy = NULL, *f_copy = NULL;
    int is_global, rv = 0;

    if ((f_copy = sal_alloc(strlen(first) + 1, "first")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "diag_sand_collect_args_with_view : Out of memory\n")));
        rv = -1;
        goto done;
    }
    memset(f_copy, 0, strlen(first) + 1);
    sal_strcpy(f_copy, first);

    /*
     * Check if field is global before applying view prefix 
     */
    diag_sand_check_global(unit, mem, f_copy, &is_global);
    if (!is_global)
    {
        sal_strcpy(valstr, view);
        strcat(valstr, first);
    }
    else
    {
        sal_strcpy(valstr, first);
    }

    while ((s = ARG_GET(a)) != 0)
    {
        if ((s_copy = sal_alloc(strlen(s) + 1, "s_copy")) == NULL)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "diag_sand_collect_args_with_view : Out of memory\n")));
            rv = -1;
            goto done;
        }
        memset(s_copy, 0, strlen(s) + 1);
        sal_strcpy(s_copy, s);
        diag_sand_check_global(unit, mem, s_copy, &is_global);
        sal_free(s_copy);
        strcat(valstr, ",");
        if (!is_global)
        {
            strcat(valstr, view);
            strcat(valstr, s);
        }
        else
        {
            strcat(valstr, s);
        }
    }
done:
    if (f_copy != NULL)
    {
        sal_free(f_copy);
    }
    return rv;
}

/*
 * diag_sand_mem_modify_fields
 *
 *   Verify similar to modify_reg_fields (see reg.c) but works on
 *   memory table entries instead of register values.  Handles fields
 *   of any length.
 *
 *   If mask is non-NULL, it receives an entry which is a mask of all
 *   fields modified.
 *
 *   Values may be specified with optional increment or decrement
 *   amounts; for example, a MAC address could be 0x1234+2 or 0x1234-1
 *   to specify an increment of +2 or -1, respectively.
 *
 *   If incr is FALSE, the increment is ignored and the plain value is
 *   stored in the field (e.g. 0x1234).
 *
 *   If incr is TRUE, the value portion is ignored.  Instead, the
 *   increment value is added to the existing value of the field.  The
 *   field value wraps around on overflow.
 *
 *   Returns -1 on failure, 0 on success.
 */

STATIC int
diag_sand_mem_modify_fields(
    int unit,
    soc_mem_t mem,
    uint32 *entry,
    uint32 *mask,
    char *mod,
    int incr)
{
    soc_field_info_t *fld;
    char *fmod, *fval, *s;
    char *modstr = NULL;
    uint32 fvalue[SOC_MAX_MEM_FIELD_WORDS];
    uint32 fincr[SOC_MAX_MEM_FIELD_WORDS];
    int i, entry_dw;
    soc_mem_info_t *m = &SOC_MEM_INFO(unit, mem);
    char *tokstr;

    entry_dw = BYTES2WORDS(m->bytes);
    if ((modstr = sal_alloc(ARGS_BUFFER, "modify_mem")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "diag_sand_mem_modify_fields : Out of memory\n")));
        return CMD_FAIL;
    }

    strncpy(modstr, mod, ARGS_BUFFER);  /* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;
    mod = modstr;

    if (mask)
    {
        memset(mask, 0, entry_dw * 4);
    }

    while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0)
    {
        mod = NULL;     /* Pass strtok NULL next time */
        fval = strchr(fmod, '=');
        if (fval != NULL)
        {       /* Point fval to arg, NULL if none */
            *fval++ = 0;        /* Now fmod holds only field name. */
        }
        if (fmod[0] == 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Null field name\n")));
            sal_free(modstr);
            return -1;
        }
        if (!sal_strcasecmp(fmod, "clear"))
        {
            memset(entry, 0, entry_dw * sizeof(*entry));
            if (mask)
            {
                memset(mask, 0xff, entry_dw * sizeof(*entry));
            }
            continue;
        }
        for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++)
        {
            if (!sal_strcasecmp(fmod, SOC_FIELD_NAME(unit, fld->field)))
            {
                break;
            }
        }
        if (fld == &m->fields[m->nFields])
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "No such field \"%s\" in memory \"%s\".\n"), fmod, SOC_MEM_UFNAME(unit, mem)));
            sal_free(modstr);
            return -1;
        }
        if (!fval)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Missing %d-bit value to assign to \"%s\" field \"%s\".\n"), fld->len,
                      SOC_MEM_UFNAME(unit, mem), SOC_FIELD_NAME(unit, fld->field)));
            sal_free(modstr);
            return -1;
        }
        s = strchr(fval, '+');
        if (s == NULL)
        {
            s = strchr(fval, '-');
        }
        if (s == fval)
        {
            s = NULL;
        }
        if (incr)
        {
            if (s != NULL)
            {
                parse_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS, s[1] ? &s[1] : "1");
                if (*s == '-')
                {
                    neg_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS);
                }
                if (fld->len & 31)
                {
                    /*
                     * Proper treatment of sign extension 
                     */
                    fincr[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
                }
                soc_mem_field_get(unit, mem, entry, fld->field, fvalue);
                add_long_integer(fvalue, fincr, SOC_MAX_MEM_FIELD_WORDS);
                if (fld->len & 31)
                {
                    /*
                     * Proper treatment of sign extension 
                     */
                    fvalue[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
                }
                soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
            }
        }
        else
        {
            if (s != NULL)
            {
                *s = 0;
            }
            parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
            for (i = fld->len; i < SOC_MAX_MEM_FIELD_BITS; i++)
            {
                if (fvalue[i / 32] & 1 << (i & 31))
                {
                    LOG_INFO(BSL_LS_APPL_COMMON,
                             (BSL_META_U(unit, "Value \"%s\" too large for %d-bit field \"%s\".\n"), fval, fld->len,
                              SOC_FIELD_NAME(unit, fld->field)));
                    sal_free(modstr);
                    return -1;
                }
            }
            soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
        }
        if (mask)
        {
            memset(fvalue, 0, sizeof(fvalue));
            for (i = 0; i < fld->len; i++)
            {
                fvalue[i / 32] |= 1 << (i & 31);
            }
            soc_mem_field_set(unit, mem, mask, fld->field, fvalue);
        }
    }

    sal_free(modstr);
    return 0;
}

STATIC int
diag_sand_parse_dwords(
    int unit,
    int count,
    uint32 *dw,
    args_t * a)
{
    char *s;
    int i;

    for (i = 0; i < count; i++)
    {
        if ((s = ARG_GET(a)) == NULL)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Not enough data values (have %d, need %d)\n"), i, count));
            return -1;
        }
        dw[i] = parse_integer(s);
    }

    if (ARG_CNT(a) > 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "Ignoring extra data on command line (only %d words needed)\n"), count));
    }

    return 0;
}

/*
 * Print a one line summary for matching memories
 * If substr_match is NULL, match all memories.
 * If substr_match is non-NULL, match any memories whose name
 * or user-friendly name contains that substring.
 */
STATIC void
diag_sand_mem_list_summary(
    int unit,
    char *substr_match)
{
    soc_mem_t mem;
    int i, copies, dlen;
    int found = 0;
    char *dstr;

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!soc_mem_is_valid(unit, mem))
        {
            continue;
        }

        if (substr_match != NULL &&
            strcaseindex(SOC_MEM_NAME(unit, mem), substr_match) == NULL &&
            strcaseindex(SOC_MEM_UFNAME(unit, mem), substr_match) == NULL)
        {
            continue;
        }

        copies = 0;
        SOC_MEM_BLOCK_ITER(unit, mem, i)
        {
            copies += 1;
        }

        dlen = strlen(SOC_MEM_DESC(unit, mem));
        if (dlen > 38)
        {
            dlen = 34;
            dstr = "...";
        }
        else
        {
            dstr = "";
        }
        if (!found)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        " %-6s  %-22s%5s/%-4s %s\n", "Flags", "Name", "Entry", "Copy", "Description");
            found = 1;
        }

        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    " %c%c%c%c%c%c  %-22s %5d",
                    soc_mem_is_readonly(unit, mem) ? 'r' : '-',
                    soc_mem_is_debug(unit, mem) ? 'd' : '-',
                    soc_mem_is_sorted(unit, mem) ? 's' :
                    soc_mem_is_hashed(unit, mem) ? 'h' :
                    soc_mem_is_cam(unit, mem) ? 'A' : '-',
                    soc_mem_is_cbp(unit, mem) ? 'c' : '-',
                    (soc_mem_is_bistepic(unit, mem) ||
                     soc_mem_is_bistffp(unit, mem) ||
                     soc_mem_is_bistcbp(unit, mem)) ? 'b' : '-',
                    soc_mem_is_cachable(unit, mem) ? 'C' : '-',
                    SOC_MEM_UFNAME(unit, mem), soc_mem_index_count(unit, mem));
        if (copies == 1)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%5s %*.*s%s\n",
                        "", dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "/%-4d %*.*s%s\n",
                        copies, dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
        }
    }

    if (found)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "Flags: (r)eadonly, (d)ebug, (s)orted, (h)ashed\n"
                    "       C(A)M, (c)bp, (b)ist-able, (C)achable\n");
    }
    else if (substr_match != NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "No memory found with the substring '%s' in its name.\n"), substr_match));
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

}

STATIC int
diag_sand_mem_list_cache(
    int unit,
    soc_mem_t mem,
    void *en)
{
    if (soc_mem_cache_get(unit, mem, COPYNO_ALL))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s \n", SOC_MEM_NAME(unit, mem));
    }

    return BCM_E_NONE;
}

STATIC int
diag_sand_mem_list_dynamic(
    int unit,
    soc_mem_t mem,
    void *en)
{
#ifdef BCM_PETRA_SUPPORT        /* SDK-117365 */
    if (soc_mem_is_signal(unit, mem))
#endif
        if (FALSE)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s \n", SOC_MEM_NAME(unit, mem));
        }

    return BCM_E_NONE;
}

STATIC int
diag_sand_mem_list_ser_action(
    int unit)
{
    int nof_interrupts;
    int inter = 0;
    soc_interrupt_db_t *interrupts;
    int rc = BCM_E_NONE;
    uint32 is_enable = 0;
    bcm_switch_event_control_t bcm_switch_event_control;
    char *act_action_name, *recur_action_name;

    if (!sand_is_ser_action_support(unit))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                    "This command does not support on this device.\n");
        return CMD_OK;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "No interrupts for device.\n")));
        return CMD_FAIL;
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (rc != SOC_E_NONE)
    {
        return CMD_FAIL;
    }

    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.index = 0;

#if defined(SOC_NO_NAMES)
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                "Interrupt Id, Enable, SER Action                         , Cycle Time, Cycle Count, Recurring Action \n");
#else
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                "Interrupt Name                                                   , Enable, SER Action                         , Cycle Time, Cycle Count, Recurring Action \n");
#endif

    for (inter = 0; inter < nof_interrupts; inter++)
    {
        is_enable = 0;

        bcm_switch_event_control.event_id = inter;
        rc = bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control,
                                          &is_enable);
        if (rc != BCM_E_NONE)
        {
#if defined(SOC_NO_NAMES)
/* { */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " Get Interrupt status failed.\n");
/* } */
#else
/* { */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                        " Get Interrupt %s status failed.\n", interrupts[inter].name);
/* } */
#endif
            continue;
        }

        act_action_name = sand_intr_action_name_by_func(unit, interrupts[inter].func_arr);
        recur_action_name = sand_intr_action_name_by_func(unit, interrupts[inter].func_arr_recurring_action);

#if defined(SOC_NO_NAMES)
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%-12d, %-6d, %-35s, %-10d, %-11d, %s\n",
                    inter, is_enable,
                    act_action_name,
                    interrupts[inter].recurring_action_cycle_time,
                    interrupts[inter].recurring_action_cycle_counting, recur_action_name);
#else
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%-65s, %-6d, %-35s, %-10d, %-11d, %s\n",
                    interrupts[inter].name, is_enable,
                    act_action_name,
                    interrupts[inter].recurring_action_cycle_time,
                    interrupts[inter].recurring_action_cycle_counting, recur_action_name);
#endif
    }

    return CMD_OK;
}

/*
 * List the tables, or fields of a table entry
 */
char cmd_sand_mem_list_usage[] =
    "Parameters: [<TABLE> [<DATA> ...]]\n\t"
    "If no parameters are given, displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If TABLE is given, displays the entry fields for that table.\n\t"
    "If TABLE is cache-table, displays all cached tables.\n\t"
    "If TABLE is ser-action, displays all SER action.\n\t"
    "If TABLE is dynamic-table, displays all dynamic tables.\n\t"
    "If DATA is given, decodes the data into entry fields.\n";

cmd_result_t
cmd_sand_mem_list(
    int unit,
    args_t * a)
{
    soc_mem_info_t *m;
    soc_field_info_t *fld;
    char *tab, *s;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mask[SOC_MAX_MEM_WORDS];
    int have_entry, i, dw, copyno;
    int copies, disabled, dmaable;
    char *dmastr;
    uint32 flags;
    int minidx, maxidx;
    uint8 acc_type;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        return CMD_FAIL;
    }

    if (!soc_property_get(unit, spn_MEMLIST_ENABLE, 1))
    {
        return CMD_OK;
    }

    tab = ARG_GET(a);

    if ((tab != NULL) && (sal_strcasecmp(tab, "cache-table") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current enabled cached memories: \n");
        if (soc_mem_iterate(unit, diag_sand_mem_list_cache, NULL) < 0)
        {
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "ERROR: Get all cache tables error.\n")));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if ((tab != NULL) && (sal_strcasecmp(tab, "ser-action") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current all SER action: \n");
        if (diag_sand_mem_list_ser_action(unit) < 0)
        {
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "ERROR: List SER action error.\n")));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if ((tab != NULL) && (sal_strcasecmp(tab, "dynamic-table") == 0))
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Current dynamic memories: \n");
        if (soc_mem_iterate(unit, diag_sand_mem_list_dynamic, NULL) < 0)
        {
            LOG_ERROR(BSL_LS_APPL_SOCMEM, (BSL_META_U(unit, "ERROR: Get all dynamic tables error.\n")));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (!tab)
    {
        diag_sand_mem_list_summary(unit, NULL);
        return CMD_OK;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0)
    {
        if ((s = strchr(tab, '.')) != NULL)
        {
            *s = 0;
        }
        diag_sand_mem_list_summary(unit, tab);
        return CMD_OK;
    }

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: Memory \"%s\" not valid for this unit\n"), tab));
        return CMD_FAIL;
    }

    if (copyno < 0)
    {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    else if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno))
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: Invalid copy number %d for memory %s\n"), copyno, tab));
        return CMD_FAIL;
    }

    m = &SOC_MEM_INFO(unit, mem);
    flags = m->flags;

    dw = BYTES2WORDS(m->bytes);

    if ((s = ARG_GET(a)) == 0)
    {
        have_entry = 0;
    }
    else
    {
        for (i = 0; i < dw; i++)
        {
            if (s == 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Not enough data specified (%d words needed)\n"), dw));
                return CMD_FAIL;
            }
            entry[i] = parse_integer(s);
            s = ARG_GET(a);
        }
        if (s)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Extra data specified (ignored)\n")));
        }
        have_entry = 1;
    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Memory: %s.%s", SOC_MEM_UFNAME(unit, mem),
                SOC_BLOCK_NAME(unit, copyno));
    s = SOC_MEM_UFALIAS(unit, mem);
    if (s && *s && strcmp(SOC_MEM_UFNAME(unit, mem), s) != 0)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " alias %s \n", s);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " address 0x%08x\n",
                soc_mem_addr_get(unit, mem, 0, copyno, 0, &acc_type));

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Flags:");
    if (flags & SOC_MEM_FLAG_READONLY)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " read-only");
    }

    if (flags & SOC_MEM_FLAG_WRITEONLY)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " write-only");
    }
    if (flags & SOC_MEM_FLAG_SIGNAL)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " signal/dynamic");
    }

    if (flags & SOC_MEM_FLAG_VALID)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " valid");
    }
    if (flags & SOC_MEM_FLAG_DEBUG)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " debug");
    }
    if (flags & SOC_MEM_FLAG_SORTED)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " sorted");
    }
    if (flags & SOC_MEM_FLAG_CBP)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " cbp");
    }
    if (flags & SOC_MEM_FLAG_CACHABLE)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " cachable");
    }
    if (flags & SOC_MEM_FLAG_BISTCBP)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " bist-cbp");
    }
    if (flags & SOC_MEM_FLAG_BISTEPIC)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " bist-epic");
    }
    if (flags & SOC_MEM_FLAG_BISTFFP)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " bist-ffp");
    }
    if (flags & SOC_MEM_FLAG_UNIFIED)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " unified");
    }
    if (flags & SOC_MEM_FLAG_HASHED)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " hashed");
    }
    if (flags & SOC_MEM_FLAG_WORDADR)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " word-addressed");
    }
    if (flags & SOC_MEM_FLAG_BE)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " big-endian");
    }
    if (flags & SOC_MEM_FLAG_IS_ARRAY)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  array[%u-%u]",
                    SOC_MEM_FIRST_ARRAY_INDEX(unit, mem), SOC_MEM_NUMELS(unit,
                                                                         mem) - 1 + SOC_MEM_FIRST_ARRAY_INDEX(unit,
                                                                                                              mem));

    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Blocks: ");
    copies = disabled = dmaable = 0;
    SOC_MEM_BLOCK_ITER(unit, mem, i)
    {
        if (SOC_INFO(unit).block_valid[i])
        {
            dmastr = "";
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " %s%s", SOC_BLOCK_NAME(unit, i),
                        dmastr);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " [%s]", SOC_BLOCK_NAME(unit, i));
            disabled += 1;
        }
        copies += 1;
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " (%d cop%s", copies,
                copies == 1 ? "y" : "ies");
    if (disabled)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", %d disabled", disabled);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ")\n");

    minidx = soc_mem_index_min(unit, mem);
    maxidx = soc_mem_index_max(unit, mem);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Entries: %d with indices %d-%d (0x%x-0x%x)",
                maxidx - minidx + 1, minidx, maxidx, minidx, maxidx);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", each %d bytes %d words\n", m->bytes, dw);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Entry mask:");
    soc_mem_datamask_get(unit, mem, mask);
    for (i = 0; i < dw; i++)
    {
        if (mask[i] == 0xffffffff)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " -1");
        }
        else if (mask[i] == 0)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " 0");
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " 0x%08x", mask[i]);
        }
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");

    s = SOC_MEM_DESC(unit, mem);
    if (s && *s)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Description: %s\n", s);
    }

    for (fld = &m->fields[m->nFields - 1]; fld >= &m->fields[0]; fld--)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s<%d",
                    SOC_FIELD_NAME(unit, fld->field), fld->bp + fld->len - 1);
        if (fld->len > 1)
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ":%d", fld->bp);
        }
        if (have_entry)
        {
            uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
            char tmp[132];

            memset(fval, 0, sizeof(fval));
            soc_mem_field_get(unit, mem, entry, fld->field, fval);
            format_long_integer(tmp, fval, SOC_MAX_MEM_FIELD_WORDS);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "> = %s\n", tmp);
        }
        else
        {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    return CMD_OK;
}

static cmd_result_t
diag_sand_do_dump_table(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    int count,
    int flags)
{
    int k, i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char lineprefix[256];
    int entry_dw;
    int rv = CMD_FAIL;
    uint8 acc_type;

    assert(copyno >= 0);

    entry_dw = soc_mem_entry_words(unit, mem);

    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    for (k = index; k < index + count; k++)
    {

        if (!(flags & DUMP_DISABLE_CACHE))
        {
            i = soc_mem_array_read(unit, mem, array_index, copyno, k, entry);
        }
        else
        {
            i = soc_mem_array_read_flags(unit, mem, array_index, copyno, k, entry, SOC_MEM_DONT_USE_CACHE);
        }
        if (i < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Read ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                      SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i)));
            goto done;
        }

        if (!(flags & DUMP_TABLE_ALL))
        {
            
        }

        if (flags & DUMP_TABLE_HEX)
        {
            for (i = 0; i < entry_dw; i++)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%08x\n", entry[i]);
            }
        }
        else if (flags & DUMP_TABLE_CHANGED)
        {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY)
            {
                sal_sprintf(lineprefix, "%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            else
            {
                sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            }
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        }
        else
        {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY)
            {
                if (flags & DUMP_ADDR_ONLY)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s[%d].[%d]: 0x%x",
                                SOC_BLOCK_NAME(unit, copyno), array_index, k, soc_mem_addr_get(unit, mem, array_index,
                                                                                               copyno, k, &acc_type));
                }
                else
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s[%d].%s[%d]: ",
                                SOC_MEM_UFNAME(unit, mem), array_index, SOC_BLOCK_NAME(unit, copyno), k);
                }
            }
            else
            {
                if (flags & DUMP_ADDR_ONLY)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s.[%d]: 0x%x",
                                SOC_BLOCK_NAME(unit, copyno), k, soc_mem_addr_get(unit, mem, 0, copyno, k, &acc_type));
                }
                else
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s.%s[%d]: ",
                                SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
                }
            }
            if (flags & DUMP_TABLE_RAW)
            {
                for (i = 0; i < entry_dw; i++)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "0x%08x ", entry[i]);
                }
            }
            else
            {
                if (!(flags & DUMP_ADDR_ONLY))
                {
                    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
                }
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

    rv = CMD_OK;

done:
    return rv;
}
STATIC cmd_result_t
diag_sand_do_dump_table_field(
    int unit,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    int count,
    int flags,
    char *field)
{
    int k, i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char lineprefix[256];
    int entry_dw;
    int rv = CMD_FAIL;
    assert(copyno >= 0);
    entry_dw = soc_mem_entry_words(unit, mem);
    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
    for (k = index; k < index + count; k++)
    {
        if (!(flags & DUMP_DISABLE_CACHE))
        {
            i = soc_mem_array_read(unit, mem, array_index, copyno, k, entry);
        }
        else
        {
            i = soc_mem_array_read_flags(unit, mem, array_index, copyno, k, entry, SOC_MEM_DONT_USE_CACHE);
        }
        if (i < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Read ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                      SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i)));
            goto done;
        }
        if (!(flags & DUMP_TABLE_ALL))
        {
        }
        if (flags & DUMP_TABLE_HEX)
        {
            for (i = 0; i < entry_dw; i++)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%08x\n", entry[i]);
            }
        }
        else if (flags & DUMP_TABLE_CHANGED)
        {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY)
            {
                sal_sprintf(lineprefix, "%s[%d].%s[%d]: ", SOC_MEM_UFNAME(unit, mem), array_index,
                            SOC_BLOCK_NAME(unit, copyno), k);
            }
            else
            {
                sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            }
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        }
        else
        {
            if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s[%d].%s[%d]: ",
                            SOC_MEM_UFNAME(unit, mem), array_index, SOC_BLOCK_NAME(unit, copyno), k);
            }
            else
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s.%s[%d]: ",
                            SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            }
            if (flags & DUMP_TABLE_RAW)
            {
                for (i = 0; i < entry_dw; i++)
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "0x%08x ", entry[i]);
                }
            }
            else
            {
                diag_sand_mem_entry_dump_field(unit, mem, entry, field);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");
    rv = CMD_OK;

done:
    return rv;
}

/*
 * Function:
 *    soc_mem_entry_dump
 * Purpose:
 *    Debug routine to dump a formatted table entry.
 *
 *    Note:  Prefix != NULL : Dump chg command
 *           Prefix == NULL : Dump     command
 *             (Actually should pass dump_chg flag but keeping for simplicity)
 */
static void
diag_sand_mem_entry_dump_field(
    int unit,
    soc_mem_t mem,
    void *buf,
    char *field_name)
{
    soc_field_info_t *fieldp;
    soc_mem_info_t *memp;
    int f;
    int field_found = 0;
#if !defined(SOC_NO_NAMES)
    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
    char tmp[(SOC_MAX_MEM_FIELD_WORDS * 8) + 3];
#endif
    /*
     * Max nybbles + "0x" + null terminator 
     */

    memp = &SOC_MEM_INFO(unit, mem);

    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Memory not valid for unit\n")));
    }
    else
    {
        for (f = memp->nFields - 1; f >= 0; f--)
        {
            fieldp = &memp->fields[f];
            if (sal_strcasecmp(SOC_FIELD_NAME(unit, fieldp->field), field_name))
            {
                continue;
            }
            else if (field_found++ == 0)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "<");
            }

#if !defined(SOC_NO_NAMES)
            {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s=",
                            soc_fieldnames[fieldp->field]);
                sal_memset(fval, 0, sizeof(fval));
                soc_mem_field_get(unit, mem, buf, fieldp->field, fval);
                _shr_format_long_integer(tmp, fval, BITS2BYTES(fieldp->len));
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s%s", tmp, f > 0 ? "," : "");
            }

#endif
        }
    }

    if (field_found)
    {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">");
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "field: \"%s\" didn't found\n"), field_name));
    }

}

STATIC int
diag_sand_filter_mems(
    int unit,
    int mem)
{
    switch (mem)
    {
            /*
             * Not a real table but the actual external DRAM and its huge and slow 
             */
        case MMU_DRAM_ADDRESS_SPACEm:

            return 1;
    }
    return 0;
}

STATIC int
diag_sand_mem_get_all(
    int unit,
    int flags)
{
    soc_mem_t mem;
    int i, rv;

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!soc_mem_is_valid(unit, mem) || diag_sand_filter_mems(unit, mem))
        {
            continue;
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "unit %d, mem %d\n", unit, mem);
        SOC_MEM_BLOCK_ITER(unit, mem, i)
        {
            int index = SOC_MEM_INFO(unit, mem).index_min;
            int count = SOC_MEM_INFO(unit, mem).index_max - SOC_MEM_INFO(unit, mem).index_min + 1;
            if (soc_mem_entry_words(unit, mem) + 1 > CMIC_SCHAN_WORDS(unit))
            {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit, "Error: can't read memory %s\n"), SOC_MEM_UFNAME(unit, mem)));
                rv = CMD_FAIL;
            }
            else
            {
                rv = diag_sand_do_dump_table(unit, mem, 0, i, index, count, flags);
            }
            if (rv != CMD_OK)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "failed to dump table %d copy %d indexd %d unit %d\n"), mem, i, index,
                          unit));
            }
        }
    }

    return CMD_OK;
}

cmd_result_t
_soc_arad_mem_array_wide_access(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    void *entry_data,
    unsigned operation)         /* operation should be 1 for read and 0 for write */
{
    int rv = CMD_FAIL;
    uint32 data32, address, indirect_size = ARAD_INDIRECT_SIXE;
    uint8 acc_type;
    int blk, entry_words;
    soc_timeout_t to;
    uint8 orig_read_mode = SOC_MEM_FORCE_READ_THROUGH(unit);
    uint32 cache_consistency_check = 0;

    if (mem != OCB_OCBM_EVENm && mem != OCB_OCBM_ODDm)
    {
        cli_out("Error: can't access unknown wide memory %s\n", SOC_MEM_UFNAME(unit, mem));
        goto done;
    }

    assert(operation <= 1);
    if (index < 0)
    {
        index = -index; /* get rid of cache marking, do not support cache */
    }

    entry_words = soc_mem_entry_words(unit, mem);

    /*
     * Write to one or all copies of the memory 
     */

    MEM_LOCK(unit, mem);

    /*
     * loop over the blocks 
     */
    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {
        if (copyno != COPYNO_ALL && copyno != blk)
        {
            continue;
        }

        if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE)
        {
            if (operation == 0)
            {
                _soc_mem_write_cache_update(unit, mem, blk, 0, index, array_index, entry_data, NULL, NULL, NULL);
            }
            else
            {
                SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);
                if (TRUE ==
                    _soc_mem_read_cache_attempt(unit, flags, mem, blk, index, array_index, entry_data, NULL,
                                                &cache_consistency_check))
                {
                    rv = 0;
                    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_mode);
                    goto done;
                }
            }
        }

        data32 = 0;
        soc_reg_field_set(unit, OCB_OCB_CONFIGURATION_REGISTERr, &data32, IND_ENf, 1);
        if (WRITE_OCB_OCB_CONFIGURATION_REGISTERr(unit, data32) != SOC_E_NONE)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Failed OCB_OCB_CONFIGURATION_REGISTERr(0x%x, 1)\n"), unit));
            goto done;
        }
        if (!operation)
        {       /* write operation */
            if (WRITE_OCB_INDIRECT_WRITE_DATA_0r(unit, entry_data) != SOC_E_NONE)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Failed WRITE_OCB_INDIRECT_WRITE_DATA_0r(0x%x, %p)\n"), unit,
                          (void *) entry_data));
                goto done;
            }
            if ((entry_words - indirect_size / 32) > 0)
            {
                if (WRITE_OCB_INDIRECT_WRITE_DATA_1r(unit, ((uint32 *) entry_data) + (indirect_size / 32)) !=
                    SOC_E_NONE)
                {
                    LOG_INFO(BSL_LS_APPL_COMMON,
                             (BSL_META_U(unit, "Failed WRITE_OCB_INDIRECT_WRITE_DATA_1r(0x%x, %p)\n"), unit,
                              (void *) (((uint32 *) entry_data) + (indirect_size / 32))));
                    goto done;
                }
            }
        }
        data32 = 0;     /* not really needed as all bets are later set */
        address = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        /*
         * cli_out(" address: 0x%x\n", address); 
         */
        soc_reg_field_set(unit, OCB_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_ADDRf, address);
        soc_reg_field_set(unit, OCB_INDIRECT_COMMAND_ADDRESSr, &data32, INDIRECT_COMMAND_TYPEf, operation);
        if (WRITE_OCB_INDIRECT_COMMAND_ADDRESSr(unit, SOC_CORE_ALL, data32) != SOC_E_NONE)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Failed WRITE_OCB_INDIRECT_COMMAND_ADDRESSr(0x%x, 0x%x)\n"), unit, data32));
            goto done;
        }
        if (READ_OCB_INDIRECT_COMMANDr(unit, SOC_CORE_ALL, &data32) != SOC_E_NONE)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Failed READ_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n"), unit, data32));
            goto done;
        }
        soc_reg_field_set(unit, OCB_INDIRECT_COMMANDr, &data32, INDIRECT_COMMAND_TRIGGERf, 1);
        if (WRITE_OCB_INDIRECT_COMMANDr(unit, SOC_CORE_ALL, data32) != SOC_E_NONE)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Failed WRITE_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n"), unit, data32));
            goto done;
        }

        /*
         * wait for trigger to become 0 
         */
        soc_timeout_init(&to, 5000, 10);
        while (1)
        {
            if (READ_OCB_INDIRECT_COMMANDr(unit, SOC_CORE_ALL, &data32) != SOC_E_NONE)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Failed READ_OCB_INDIRECT_COMMANDr(0x%x, 0x%x)\n"), unit, data32));
                goto done;
            }
            if (soc_reg_field_get(unit, OCB_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGERf))
            {
                if (soc_timeout_check(&to))
                {
                    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "indirect wide memory operation timed out\n")));
                    goto done;
                }
            }
            else
            {
                break;
            }
        }
        if (operation)
        {       /* read operation */
            soc_reg_above_64_val_t above_64_val;
            if (READ_OCB_INDIRECT_READ_DATA_0r(unit, above_64_val) != SOC_E_NONE)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Failed READ_OCB_INDIRECT_READ_DATA_0r(0x%x, %p)\n"), unit,
                          (void *) entry_data));
                goto done;
            }
            memcpy(entry_data, above_64_val, (indirect_size / 8));
            if (READ_OCB_INDIRECT_READ_DATA_1r(unit, above_64_val) != SOC_E_NONE)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Failed READ_OCB_INDIRECT_READ_DATA_1r(0x%x, %p)\n"), unit,
                          (void *) entry_data));
                goto done;
            }
            memcpy(((uint32 *) entry_data) + (indirect_size / 32), above_64_val, (indirect_size / 8));
        }
        soc_reg_field_set(unit, OCB_OCB_CONFIGURATION_REGISTERr, &data32, IND_ENf, 0);
        if (WRITE_OCB_OCB_CONFIGURATION_REGISTERr(unit, data32) != SOC_E_NONE)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Failed OCB_OCB_CONFIGURATION_REGISTERr(0x%x, 0)\n"), unit));
            goto done;
        }

    }   /* finished looping over blocks */

    rv = CMD_OK;

done:

    MEM_UNLOCK(unit, mem);
    return rv;
}

static void
diag_sand_pci_print_config(
    int unit)
{
    uint32 data;
    int cap_len, cap_base, next_cap_base, i;

    if ((soc_cm_get_dev_type(unit) & BDE_PCI_DEV_TYPE) == 0)
    {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "Error in %s(): Device does not support PCI interface."), FUNCTION_NAME()));
        return;
    }

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_VENDOR_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  DeviceID=%04x  VendorID=%04x\n",
                PCI_CONF_VENDOR_ID, data, (data & 0xffff0000) >> 16, (data & 0x0000ffff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_COMMAND);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  Status=%04x  Command=%04x\n",
                PCI_CONF_COMMAND, data, (data & 0xffff0000) >> 16, (data & 0x0000ffff) >> 0);
    cap_len = (data >> 16) & 0x10 ? 4 : 0;

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_REVISION_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  ClassCode=%06x  RevisionID=%02x\n",
                PCI_CONF_REVISION_ID, data, (data & 0xffffff00) >> 8, (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_CACHE_LINE_SIZE);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BIST=%02x  HeaderType=%02x  "
                "LatencyTimer=%02x  CacheLineSize=%02x\n",
                PCI_CONF_CACHE_LINE_SIZE, data,
                (data & 0xff000000) >> 24,
                (data & 0x00ff0000) >> 16, (data & 0x0000ff00) >> 8, (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR0);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress0=%08x\n",
                PCI_CONF_BAR0, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR1);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress1=%08x\n",
                PCI_CONF_BAR1, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR2);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress2=%08x\n",
                PCI_CONF_BAR2, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR3);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress3=%08x\n",
                PCI_CONF_BAR3, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR4);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress4=%08x\n",
                PCI_CONF_BAR4, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_BAR5);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  BaseAddress5=%08x\n",
                PCI_CONF_BAR5, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_CB_CIS_PTR);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  CardbusCISPointer=%08x\n",
                PCI_CONF_CB_CIS_PTR, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_SUBSYS_VENDOR_ID);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                "%04x: %08x  SubsystemID=%02x  SubsystemVendorID=%02x\n", PCI_CONF_SUBSYS_VENDOR_ID, data,
                (data & 0xffff0000) >> 16, (data & 0x0000ffff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_EXP_ROM);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  ExpansionROMBaseAddress=%08x\n",
                PCI_CONF_EXP_ROM, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x34);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,
                "%04x: %08x  Reserved=%06x  CapabilitiesPointer=%02x\n", 0x34, data, (data & 0xffffff00) >> 8,
                (data & 0x000000ff) >> 0);
    cap_base = cap_len ? data & 0xff : 0;

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x38);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  Reserved=%08x\n",
                0x38, data, data);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, PCI_CONF_INTERRUPT_LINE);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  Max_Lat=%02x  Min_Gnt=%02x  "
                "InterruptPin=%02x  InterruptLine=%02x\n",
                PCI_CONF_INTERRUPT_LINE, data,
                (data & 0xff000000) >> 24,
                (data & 0x00ff0000) >> 16, (data & 0x0000ff00) >> 8, (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x40);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  Reserved=%02x  "
                "RetryTimeoutValue=%02x  TRDYTimeoutValue=%02x\n",
                0x40, data, (data & 0xffff0000) >> 16, (data & 0x0000ff00) >> 8, (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x44);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  PLLConf=%01x\n",
                0x44, data, (data & 0x000000ff) >> 0);

    data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, 0x48);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  -\n", 0x48, data);

    while (cap_base)
    {
        data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, cap_base);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  CapabilityID=%02x "
                    "CapabilitiesPointer=%02x ", cap_base, data, data & 0xff, (data >> 8) & 0xff);
        next_cap_base = (data >> 8) & 0xff;
        switch (data & 0xff)
        {
            case 0x01:
                cap_len = 2 * 4;
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "PWR-MGMT\n");
                break;
            case 0x03:
                cap_len = 2 * 4;
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "VPD\n");
                break;
            case 0x05:
                cap_len = 6 * 4;        /* 3 to 6 DWORDS */
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "MSI\n");
                break;
            case 0x10:
                cap_len = 3 * 4;
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "PCIE\n");
                break;
            case 0x11:
                cap_len = 3 * 4;
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "MSI-X\n");
                break;
            default:
                break;
        }
        for (i = 4; i < cap_len; i += 4)
        {
            data = CMVEC(unit).pci_conf_read(&CMDEV(unit).dev, cap_base + i);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%04x: %08x  -\n", cap_base + i, data);
        }
        cap_base = next_cap_base;
    }
}

static int
diag_sand_hostmem_w_print(
    int unit,
    args_t * a)
{
    char *an_arg;
    char *count;
    sal_vaddr_t vaddr;
    uint32 t2, i;

    an_arg = ARG_GET(a);
    count = ARG_GET(a);
    if (!an_arg)
    {
        cli_out("Dumping memory requires address and optional count\n");
        return CMD_FAIL;
    }
    vaddr = parse_address(an_arg) & ~3;
    t2 = count ? parse_integer(count) : 1;
    for (i = 0; i < t2; i++, vaddr += 4)
    {
        uint32 *memptr = INT_TO_PTR(vaddr);
        if ((i % 4) == 0)
        {
            cli_out("%p: ", (void *) memptr);
        }
        cli_out("%08x%c", *memptr, ((i + 1) % 4) == 0 ? '\n' : ' ');
    }
    if (i % 4)
    {
        cli_out("\n");
    }

    return BCM_E_NONE;
}

STATIC int
dump_single_memory(
    int unit,
    soc_mem_t mem,
    soc_block_t blk)
{
    int index, count;
    int array_index;
    int rv;

    index = soc_mem_index_min(unit, mem);
    count = soc_mem_index_max(unit, mem) - index + 1;
    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_IS_ARRAY)
    {
        for (array_index = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
             array_index < (SOC_MEM_FIRST_ARRAY_INDEX(unit, mem) + SOC_MEM_NUMELS(unit, mem)); array_index++)
        {
            rv = diag_sand_do_dump_table(unit, mem, array_index, blk, index, count, 0);
            if (rv != CMD_OK)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "failed to dump table %d copy %d indexd %d unit %d\n"), mem, count, index,
                          unit));
            }
        }
    }
    else
    {
        rv = diag_sand_do_dump_table(unit, mem, 0, blk, index, count, 0);
        if (rv != CMD_OK)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "failed to dump table %d copy %d indexd %d unit %d\n"), mem, count, index,
                      unit));
        }
    }

    return CMD_OK;
}

char cmd_sand_mem_get_usage[] = "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false] [raw] [hex] [all] [chg] [disable_cache]\n\t"
    "        <TABLE[[ARRAYINDEX]]>[.<COPYNO>][<INDEX>] [<COUNT>]\n\t"
    "        [-filter <FIELD>=<VALUE>[,...]]\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If all is specified, show even empty or invalid entries\n\t"
    "      If addr is specified, show block instance and address instead of memory name and index\n\t"
    "      If block is specified, show all of the memories in the block\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      If disable_cache is specified - dosn't read from cached memory, if exist\n\t "
    "      (Use \"listmem\" command to show a list of valid tables)\n"
    "DUMP MW [<START> [<COUNT>]]   (System memory, 32 bits)\n"
#endif
    ;

cmd_result_t
cmd_sand_mem_get(
    int unit,
    args_t * a)
{
    soc_mem_t mem;
    char *arg1, *arg2, *arg3;
    volatile int flags = 0;
    int copyno, blk;
    soc_block_t blk_all;
    volatile int rv = CMD_FAIL;
    parse_table_t pt;
    volatile char *fname = "";
    int append = FALSE;
    unsigned array_index;
    char *next_name;
    volatile int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf ctrl_c;
    uint32 is_debug = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        goto done;
    }
    next_name = NULL;
    if (parse_arg_eq(a, &pt) < 0)
    {
        rv = CMD_USAGE;
        goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0)
    {
        /*
         * Catch control-C in case if using file output option. 
         */
#ifndef NO_CTRL_C
        if (setjmp(ctrl_c))
        {
            rv = CMD_INTR;
            goto done;
        }
#endif
        sh_push_ctrl_c(&ctrl_c);
        pushed_ctrl_c = TRUE;

        if (bslfile_is_enabled())
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "%s: Can't dump to file while logging is enabled\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        if (bslfile_open((char *) fname, append) < 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s: Could not start log file\n"), ARG_CMD(a)));
            rv = CMD_FAIL;
            goto done;
        }

        bslcons_enable(FALSE);
        console_disabled = 1;
    }

    arg1 = ARG_GET(a);
    for (;;)
    {
        if (arg1 != NULL && !sal_strcasecmp(arg1, "raw"))
        {
            flags |= DUMP_TABLE_RAW;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "hex"))
        {
            flags |= DUMP_TABLE_HEX;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "all"))
        {
            flags |= DUMP_TABLE_ALL;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "chg"))
        {
            flags |= DUMP_TABLE_CHANGED;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "debug"))
        {
            is_debug = 1;
            flags |= REG_PRINT_ADDR;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "disable_cache"))
        {
            flags |= DUMP_DISABLE_CACHE;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "addr"))
        {
            flags |= DUMP_ADDR_ONLY;
            arg1 = ARG_GET(a);
        }
        else if (arg1 != NULL && !sal_strcasecmp(arg1, "block"))
        {
            flags |= DUMP_ALL_FROM_BLOCK;
            arg1 = ARG_GET(a);
        }
        else
        {
            break;
        }
    }

    if (arg1 == NULL)
    {
        rv = CMD_USAGE;
        goto done;
    }

    if (!sal_strcasecmp(arg1, "soc"))
    {
        rv = diag_sand_reg_get_all(unit, is_debug, NULL);
        goto done;
    }

    if (!sal_strcasecmp(arg1, "socmem"))
    {
        rv = diag_sand_mem_get_all(unit, flags);
        goto done;
    }

    if (!sal_strcasecmp(arg1, "pcic"))
    {
        diag_sand_pci_print_config(unit);
        goto done;
    }

    if (!sal_strcasecmp(arg1, "mw"))
    {
        diag_sand_hostmem_w_print(unit, a);
        goto done;
    }

    /*
     * Dumping a memories tables from block 
     */
    if (flags & DUMP_ALL_FROM_BLOCK)
    {
        int length, blk_name_length;
        char block_name[16];
        int instance = 0;
        int block_type;
        int printed = 0;
        int is_block_instance;
        length = sal_strlen(arg1);
        is_block_instance = utilex_char_is_digit(arg1[length - 1]);
        sal_memset(block_name, 0, sizeof(block_name));

        /*
         * Looping all over the blocks and finding its instance id in order to test it 
         */
        for (blk_all = 0; SOC_BLOCK_INFO(unit, blk_all).type >= 0; blk_all++)
        {
            /*
             * Checking if the last character is not a digit and storing the block instance for testing 
             */
            if (!is_block_instance)
            {
                sal_strncpy(block_name, SOC_BLOCK_NAME(unit, blk_all), sizeof(block_name) - sizeof(char));
                blk_name_length = sal_strlen(block_name);
                block_name[blk_name_length - 1] = '\0';
                if (sal_strcasecmp(block_name, arg1) == 0)
                {
                    instance = 1;
                    break;
                }
            }
            else
            {
                if (sal_strcasecmp(SOC_BLOCK_NAME(unit, blk_all), arg1) == 0)
                {
                    instance = 1;
                    break;
                }
            }
        }
        if (instance == 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Error: Block was not found in the device.\n")));
            goto done;
        }
        block_type = SOC_BLOCK_INFO(unit, blk_all).type;
        /*
         * Looping all over the memories to access them using the found block 
         */
        for (mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            if (!SOC_MEM_IS_VALID(unit, mem))
            {
                continue;
            }
            if ((SOC_MEM_INFO(unit, mem).flags & (SOC_MEM_FLAG_WRITEONLY)) || (SOC_MEM_IS_ALIAS(unit, mem))
                || SOC_BLOCK_INFO(unit, SOC_MEM_BLOCK_MIN(unit, mem)).type != block_type)
            {
                continue;
            }
            /*
             * Checking if the last character is digit. If it is we are performing the test with the stored blocks else 
             * doing it only for the needed instance 
             */
            if (!is_block_instance)
            {
                for (blk_all = SOC_MEM_BLOCK_MIN(unit, mem); blk_all <= SOC_MEM_BLOCK_MAX(unit, mem); blk_all++)
                {
                    rv = dump_single_memory(unit, mem, blk_all);
                    printed = 1;
                }
            }
            else
            {
                if (blk_all >= SOC_MEM_BLOCK_MIN(unit, mem) && blk_all <= SOC_MEM_BLOCK_MAX(unit, mem))
                {
                    rv = dump_single_memory(unit, mem, blk_all);
                    printed = 1;
                }
                else
                {
                    LOG_INFO(BSL_LS_APPL_COMMON,
                             (BSL_META_U(unit, "Memory %s belongs to the block type not to the given instance\n"),
                              SOC_MEM_NAME(unit, mem)));
                }
            }
        }
        /*
         * If no memories were printed that means the block is not valid or no memories were found 
         */
        if (printed == 0)
        {
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Error: No memoreis were printed\n")));
        }
        goto done;
    }
    /*
     * See if dumping a memory table 
     */
    if (parse_memory_name(unit, &mem, arg1, &copyno, &array_index) >= 0)
    {
        int index, count;

        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Error: Memory %s not valid for chip %s.\n"), SOC_MEM_UFNAME(unit, mem),
                      SOC_UNIT_NAME(unit)));
            goto done;
        }
        if (arg2)
        {
            index = parse_memory_index(unit, mem, arg2);
            count = (arg3 ? parse_integer(arg3) : 1);
        }
        else
        {
            index = soc_mem_index_min(unit, mem);
            if (soc_mem_is_sorted(unit, mem) && !(flags & DUMP_TABLE_ALL))
            {
                count = soc_mem_entries(unit, mem, copyno);
            }
            else
            {
                count = soc_mem_index_max(unit, mem) - index + 1;
            }
        }

        next_name = ARG_GET(a);

        SOC_MEM_BLOCK_ITER(unit, mem, blk)
        {

            if (copyno != COPYNO_ALL && copyno != blk)
            {
                continue;
            }

            if (next_name != NULL)
            {
                rv = diag_sand_do_dump_table_field(unit, mem, array_index, blk, index, count, flags, next_name);
            }
            else
            {
                rv = diag_sand_do_dump_table(unit, mem, array_index, blk, index, count, flags);
            }
        }
        goto done;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "Unknown option or memory to dump (use 'help dump' for more info)\n")));

    rv = CMD_FAIL;

done:

    if (fname[0] != 0)
    {
        bslfile_close();
    }

    if (console_disabled && console_was_on)
    {
        bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c)
    {
        sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}

STATIC cmd_result_t
diag_sand_mem_write(
    int unit,
    args_t * a,
    int mod)
{
    int i, index, start, count, copyno, blk;
    unsigned array_index;
    char *tab, *idx, *cnt, *s, *memname;
    soc_mem_t mem, mem_orig;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int entry_dw, view_len;
    char copyno_str[8];
    int r, update;
    int rv = CMD_FAIL;
    char *valstr = NULL, *view = NULL;
    int no_cache = 0;

    if (!sh_check_attached(ARG_CMD(a), unit))
    {
        goto done;
    }

    tab = ARG_GET(a);
    if (tab != NULL && sal_strcasecmp(tab, "nocache") == 0)
    {
        no_cache = 1;
        tab = ARG_GET(a);
    }
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /*
     * you will need at least one value and all the args .. 
     */
    if (!tab || !idx || !cnt || !s || !isint(cnt))
    {
        return CMD_USAGE;
    }

    /*
     * Deal with VIEW:MEMORY if applicable 
     */
    memname = strstr(tab, ":");
    view_len = 0;
    if (memname != NULL)
    {
        memname++;
        view_len = memname - tab;
    }
    else
    {
        memname = tab;
    }

    if (parse_memory_name(unit, &mem, memname, &copyno, &array_index) < 0)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: unknown table \"%s\"\n"), tab));
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit, "Error: Memory %s not valid for chip %s.\n"), SOC_MEM_UFNAME(unit, mem),
                   SOC_UNIT_NAME(unit)));
        goto done;
    }

    if (soc_mem_is_readonly(unit, mem))
    {
        LOG_ERROR(BSL_LS_APPL_SHELL, (BSL_META_U(unit, "ERROR: Table %s is read-only\n"), SOC_MEM_UFNAME(unit, mem)));
        goto done;
    }

    mem_orig = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem_orig);

    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

    if (copyno == COPYNO_ALL)
    {
        copyno_str[0] = 0;
    }
    else
    {
        sal_sprintf(copyno_str, ".%d", copyno);
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    if ((valstr = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "cmd_esw_mem_write : Out of memory\n")));
        goto done;
    }

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */
    if (!isint(s))
    {
        /*
         * List of fields 
         */
        if (view_len == 0)
        {
            diag_sand_utils_collect_comma_args(a, valstr, s);
        }
        else
        {
            if ((view = sal_alloc(view_len + 1, "view_name")) == NULL)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "cmd_esw_mem_write : Out of memory\n")));
                goto done;
            }
            memset(view, 0, view_len + 1);
            memcpy(view, tab, view_len);
            if (diag_sand_collect_args_with_view(a, valstr, s, view, unit, mem) < 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Out of memory: aborted\n")));
                goto done;
            }
        }

        memset(entry, 0, sizeof(entry));

        if (0 == mod)
        {
            if (diag_sand_mem_modify_fields(unit, mem, entry, NULL, valstr, FALSE) < 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Syntax error: aborted\n")));
                goto done;
            }
        }

        update = TRUE;
    }
    else
    {
        /*
         * List of numeric values 
         */
        ARG_PREV(a);
        if (diag_sand_parse_dwords(unit, entry_dw, entry, a) < 0)
        {
            goto done;
        }
        update = FALSE;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk)
    {

        if (copyno != COPYNO_ALL && copyno != blk)
        {
            continue;
        }

        for (index = start; index < start + count; index++)
        {

            if (1 == mod)
            {
                r = soc_mem_array_read_flags(unit, mem, array_index, blk, index, entry, SOC_MEM_DONT_USE_CACHE);
                if (r < 0)
                {
                    LOG_INFO(BSL_LS_APPL_COMMON,
                             (BSL_META_U(unit, "READ ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                              copyno_str, index, soc_errmsg(r)));
                    goto done;
                }

                if (diag_sand_mem_modify_fields(unit, mem, entry, NULL, valstr, FALSE) < 0)
                {
                    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Syntax error: aborted\n")));
                    goto done;
                }
            }

            if (index == start)
            {
                if (bsl_check(bslLayerAppl, bslSourceSocmem, bslSeverityNormal, unit))
                {
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "WRITE[%s%s], DATA:",
                                SOC_MEM_UFNAME(unit, mem), copyno_str);
                    for (i = 0; i < entry_dw; i++)
                    {
                        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " 0x%x", entry[i]);
                    }
                    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
                }
            }

            r = soc_mem_array_write_extended(unit, no_cache ? SOC_MEM_DONT_USE_CACHE : SOC_MEM_NO_FLAGS, mem_orig,
                                             array_index, blk, index, entry);
            if (r < 0)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "Write ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem), copyno_str,
                          index, soc_errmsg(r)));
                goto done;
            }

            if (mod && update)
            {
                diag_sand_mem_modify_fields(unit, mem, entry, NULL, valstr, TRUE);
            }
        }
    }
    rv = CMD_OK;

done:
    if (valstr != NULL)
    {
        sal_free(valstr);
    }
    if (view != NULL)
    {
        sal_free(view);
    }
    return rv;
}

char cmd_sand_mem_write_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t" "Writes entry(s) into table index(es).\n";

cmd_result_t
cmd_sand_mem_write(
    int unit,
    args_t * a)
{
    return diag_sand_mem_write(unit, a, 0);
}

char cmd_sand_mem_modify_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        <FIELD>=<VALUE>[,...]\n\t" "Read/modify/write field(s) of a table entry(s).\n";

cmd_result_t
cmd_sand_mem_modify(
    int unit,
    args_t * a)
{
    return diag_sand_mem_write(unit, a, 1);
}

cmd_result_t
do_mem_test_rw(
    int unit,
    tr7_dbase_t tr7_data,
    args_t * a)
{
    int rv = SOC_E_NONE;

    sal_memset(&test_run_memory_stat, 0x0, sizeof(test_run_memory_stat_t));

    if ((soc_mem_iterate(unit, sand_mem_test_rw_iter_callback, &tr7_data)) < 0)
    {
        rv = CMD_FAIL;
    }

    /*
     * Print statistics summary 
     */
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Test statistics:\n")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "Number of Valid memories: %d\n"), test_run_memory_stat.mem_valid_num));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "Number of Tested memories: %d (Skiped %d)\n"), test_run_memory_stat.mem_test_num,
              test_run_memory_stat.mem_valid_num - test_run_memory_stat.mem_test_num));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of Pass memories: %d\n"), test_run_memory_stat.mem_pass_num));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of Fail memories: %d\n"), test_run_memory_stat.mem_fail_num));

    return rv;
}

/* TR 6
 * Go over all memories and read them. 
 */

/* TR 6
 * Go over all memories and read them. 
 */
#ifdef BCM_DNX_SUPPORT
STATIC cmd_result_t
do_dump_table_array(
    int unit,
    soc_mem_t mem,
    int copyno,
    int array_idx,
    int index,
    int count,
    int flags)
{
    int k, i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char lineprefix[256];
    int entry_dw;
    int rv = CMD_FAIL;

    assert(copyno >= 0);

    entry_dw = soc_mem_entry_words(unit, mem);

    for (k = index; k < index + count; k++)
    {
        i = soc_mem_read_no_cache(unit, 0, mem, array_idx, copyno, k, entry);
        if (i < 0)
        {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit, "Read ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                       SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i)));
            goto done;
        }

        if (flags & DUMP_TABLE_HEX)
        {
            for (i = 0; i < entry_dw; i++)
            {
                LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%08x\n"), entry[i]));
            }
        }
        else if (flags & DUMP_TABLE_CHANGED)
        {
            sal_snprintf(lineprefix, 255, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        }
        else
        {
            LOG_DEBUG(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit, "%s.%s[%d]: "), SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k));

            if (flags & DUMP_TABLE_RAW)
            {
                for (i = 0; i < entry_dw; i++)
                {
                    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "0x%08x "), entry[i]));
                }
            }
            else
            {
                if (bsl_check(bslLayerAppl, bslSourceTests, bslSeverityDebug, unit))
                {
                    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
                }
            }

            LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        }
    }

    rv = CMD_OK;

done:
    return rv;
}
#endif

STATIC cmd_result_t
do_dump_table_single(
    int unit,
    soc_mem_t mem,
    int copyno,
    int index,
    int count,
    int flags)
{
    int k, i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char lineprefix[256];
    int entry_dw;
    int rv = CMD_FAIL;

    assert(copyno >= 0);

    entry_dw = soc_mem_entry_words(unit, mem);

    for (k = index; k < index + count; k++)
    {
        i = soc_mem_read(unit, mem, copyno, k, entry);
        if (i < 0)
        {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit, "Read ERROR: table %s.%s[%d]: %s\n"), SOC_MEM_UFNAME(unit, mem),
                       SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i)));
            goto done;
        }

        if (flags & DUMP_TABLE_HEX)
        {
            for (i = 0; i < entry_dw; i++)
            {
                LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "%08x\n"), entry[i]));
            }
        }
        else if (flags & DUMP_TABLE_CHANGED)
        {
            sal_sprintf(lineprefix, "%s.%s[%d]: ", SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k);
            soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
        }
        else
        {
            LOG_DEBUG(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit, "%s.%s[%d]: "), SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), k));

            if (flags & DUMP_TABLE_RAW)
            {
                for (i = 0; i < entry_dw; i++)
                {
                    LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "0x%08x "), entry[i]));
                }
            }
            else
            {
                if (bsl_check(bslLayerAppl, bslSourceTests, bslSeverityDebug, unit))
                {
                    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
                }
            }

            LOG_DEBUG(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "\n")));
        }
    }

    rv = CMD_OK;

done:
    return rv;
}

/* The function rval_test_skip_mem contains skip list for TR 6 */
int
sand_mem_dump_iter_callback(
    int unit,
    soc_mem_t mem,
    void *data)
{
    int copyno, index_min, count;
    int rv = SOC_E_NONE;
    int flags = ((int *) data)[0];
#ifdef BCM_PETRA_SUPPORT
    uint8 include_port_macros = ((int *) data)[1];
    uint32 start_from = ((int *) data)[2];
    uint32 max_size = ((int *) data)[3];
#endif
#ifdef BCM_DNX_SUPPORT
    int min_array_index, max_array_index, array_idx;
#endif
    /*
     * since we test access to memories and since access to memory is the same if it is main table or alias/format we
     * skip over alias/format tables to reduce the tr time specially on the slow emulation 
     */

    if (!is_main_mem(unit, mem))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "Skipping alias/format memory %s \n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit, "Memory %s is not valid for device.\n"), SOC_MEM_NAME(unit, mem)));
        return SOC_E_INTERNAL;
    }

    test_run_memory_stat.mem_valid_num++;

    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping debug memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY)
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping Write only memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }

    if (2 + soc_mem_entry_words(unit, mem) > CMIC_SCHAN_WORDS(unit))
    {
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Skipping large size memory %s\n"), SOC_MEM_NAME(unit, mem)));
        return rv;
    }
#ifdef BCM_PETRA_SUPPORT

    if (SOC_IS_ARAD(unit))
    {
        if (mem_too_large_for_test(unit, mem, max_size))
        {
            return 1;
        }
        switch (mem)
        {
            case IHB_TCAM_BANK_COMMANDm:
            case IHB_TCAM_BANK_REPLYm:
            case BRDC_FSRD_FSRD_WL_EXT_MEMm:
            case IQM_MEM_8000000m:
            case NBI_TBINS_MEMm:
            case NBI_RBINS_MEMm:
            case PORT_WC_UCMEM_DATAm:
            case MMU_DRAM_ADDRESS_SPACEm:
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Arad excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
    }

    if (SOC_IS_JERICHO(unit))
    {
        soc_block_type_t block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
        if (mem < start_from)
        {
            return rv;
        }

        if (mem_in_skip_list_for_jericho_emulation(unit, mem))
        {
            return rv;
        }

        if (!include_port_macros)
        {
            if (block_can_be_disabled(block))
            {
                return rv;
            }
        }
        switch (mem)
        {
            case IQM_PAKCET_DESCRIPTOR_MEMORY_ECC_DYNAMICm:    /* asic internal mem */
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping JEr/Qmx excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
        /*
         * need to check why iproc mems doesnt have an access on emu while iproc regs have
         */
        if (block == SOC_BLK_IPROC)
        {
            return rv;
        }
    }

    if (SOC_IS_ARDON(unit))
    {
        switch (mem)
        {
            case EDB_ESEM_MANAGEMENT_REQUESTm:
            case EDB_ESEM_STEP_TABLEm:
            case EPNI_ESEM_MANAGEMENT_REQUESTm:
            case EPNI_ESEM_STEP_TABLEm:
            case EPNI_MEM_520000m:
            case EPNI_MEM_630000m:
            case EPNI_MEM_640000m:
            case EPNI_MEM_760000m:
            case EPNI_MEM_770000m:
            case EPNI_MEM_5A0000m:
            case EPNI_MEM_5B0000m:
            case EPNI_MEM_6C0000m:
            case EPNI_MEM_6D0000m:
            case IHB_TCAM_BANKm:

                /*
                 * NonExisting Tabels (DOC) 
                 */
            case IHB_DESTINATION_STATUSm:
            case IHB_FEC_ECMPm:
            case IHB_FEC_ENTRYm:
            case IHB_FEC_ENTRY_ACCESSEDm:
            case IHB_FEC_ENTRY_FORMAT_Am:
            case IHB_FEC_ENTRY_FORMAT_Bm:
            case IHB_FEC_ENTRY_FORMAT_Cm:
            case IHB_FEC_ENTRY_FORMAT_NULLm:
            case IHB_FEC_ENTRY_GENERALm:
            case IHB_FEC_SUPER_ENTRYm:
            case IHB_PATH_SELECTm:
            case PPDB_A_TCAM_BANKm:    /* Alias to NonExisting */
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Ardon excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
    }
    if (SOC_IS_QAX(unit))
    {
        switch (mem)
        {
            case OAMP_FLEX_VER_MASK_TEMPm:
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Ardon excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        soc_block_type_t block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));

        if (!SOC_MEM_IS_VALID(unit, mem)
            || (soc_mem_flags(unit, mem) & (SOC_MEM_FLAG_READONLY | SOC_MEM_FLAG_WRITEONLY | SOC_MEM_FLAG_SIGNAL)) != 0)
        {
            return 1;
        }
        if (soc_mem_is_signal(unit, mem))
        {
            return 1;
        }
        if (block == SOC_BLK_FSCL)
        {
            return 1;
        }
        switch (mem)
        {
                /*
                 * this memory has to be skipped
                 */
                /*
                 * The HBM memories are not real memories 
                 */
            case BRDC_HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case BRDC_HBC_HBM_DRAM_CPU_ACCESSm:
            case BRDC_FSRD_FSRD_PROM_MEMm:
            case HBC_HBM_DRAM_CPU_ACCESSm:
            case HBC_HBM_PHY_CH_REGISTER_ACCESSm:
            case HBMC_HBM_PHY_CHM_REGISTER_ACCESSm:
                /*
                 * Not a real memory 
                 */
            case FSRD_FSRD_PROM_MEMm:

                /*
                 * The CDU instrumentation and rx/tx memories do not have to be initialized and can not be written by
                 * DMA 
                 */
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMBm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMCm:
            case CDU_INSTRUMENTATION_STATS_MEM_PFC_MEMDm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMAm:
            case CDU_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMBm:
            case CDU_RX_MEMA_CTRLm:
            case CDU_RX_MEMB_CTRLm:
            case CDU_RX_MEMC_CTRLm:
            case CDU_RX_MEMD_CTRLm:
            case CDU_RX_MEME_CTRLm:
            case CDU_RX_MEMF_CTRLm:
            case CDU_RX_MEMG_CTRLm:
            case CDU_RX_MEMH_CTRLm:
            case CDU_RX_MEMI_CTRLm:
            case CDU_RX_MEMJ_CTRLm:
            case CDU_RX_MEMK_CTRLm:
            case CDU_RX_MEML_CTRLm:
            case CDU_RX_MEMM_CTRLm:
            case CDU_RX_MEMN_CTRLm:
            case CDU_RX_MEMO_CTRLm:
            case CDU_RX_MEMP_CTRLm:
            case CDU_TX_MEMA_CTRLm:
            case CDU_TX_MEMB_CTRLm:
            case CDU_TX_MEMC_CTRLm:
            case CDU_TX_MEMD_CTRLm:
            case CDU_TX_MEME_CTRLm:
            case CDU_TX_MEMF_CTRLm:
            case CDU_TX_MEMG_CTRLm:
            case CDU_TX_MEMH_CTRLm:
            case CDU_TX_MEMI_CTRLm:
            case CDU_TX_MEMJ_CTRLm:

                /*
                 * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization 
                 */
            case DDHB_MACRO_0_ABK_BANK_Am:
            case DDHB_MACRO_0_ABK_BANK_Bm:
            case DDHB_MACRO_0_ENTRY_BANKm:
            case DDHB_MACRO_1_ABK_BANK_Am:
            case DDHB_MACRO_1_ABK_BANK_Bm:
            case DDHB_MACRO_1_ENTRY_BANKm:
            case DDHA_MACRO_0_ABK_BANK_Am:
            case DDHA_MACRO_0_ABK_BANK_Bm:
            case DDHA_MACRO_0_ENTRY_BANKm:
            case DDHA_MACRO_1_ABK_BANK_Am:
            case DDHA_MACRO_1_ABK_BANK_Bm:
            case DDHA_MACRO_1_ENTRY_BANKm:
            case DHC_MACRO_ABK_BANK_Am:
            case DHC_MACRO_ABK_BANK_Bm:
            case DHC_MACRO_ENTRY_BANKm:

                /*
                 * These are address spaces saved for ilkn memory mapped registers. 
                 */
            case ILE_PORT_0_CPU_ACCESSm:
            case ILE_PORT_1_CPU_ACCESSm:

                /*
                 * All below are either not real memories or dont have protection. - Mosi Ravia 
                 */
            case KAPS_RPB_TCAM_CPU_COMMANDm:
            case KAPS_TCAM_ECC_MEMORYm:
            case MDB_ARM_KAPS_TCMm:
            case MDB_MEM_2300000m:
            case MDB_EEDB_ABK_BANKm:
            case MACT_CPU_REQUESTm:
            case ECI_SAM_CTRLm:

                /*
                 * SCH_SCHEDULER_INIT is not a real memory 
                 */
            case SCH_SCHEDULER_INITm:
            case SQM_DEQ_QSTATE_PENDING_FIFOm:
            case SQM_ENQ_COMMAND_RXIm:
            case SQM_PDB_LINK_LISTm:
            case SQM_PDMm:
            case SQM_QUEUE_DATA_MEMORYm:
            case SQM_TX_BUNDLE_MEMORYm:
            case SQM_TX_PACKET_DESCRIPTOR_FIFOS_MEMORYm:
            case SCH_FORCE_STATUS_MESSAGEm:
            case SCH_RESERVED_37m:
            case SCH_RESERVED_54m:
            case EPS_CAL_CAL_INDX_MUXm:
            case EPS_OTM_CALENDAR_CRDT_TABLEm:
            case EPS_OTM_HP_CRDT_TABLEm:
            case EPS_OTM_LP_CRDT_TABLEm:
            case EPS_QP_CREDIT_TABLEm:
            case EPS_TCG_CREDIT_TABLEm:
            case AM_TABLEm:
            case CDPORT_TSC_UCMEM_DATAm:
                /*
                 * The *_DEBUG_UNIT and *_HIT_INDICATION memories are not real 
                 */
            case IPPD_MEM_F000000m:
            case ERPP_EPMFCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_HIT_INDICATIONm:
            case IPPA_VTECS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPACS_TCAM_HIT_INDICATIONm:
            case IPPB_FLPBCS_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSA_TCAM_HIT_INDICATIONm:
            case IPPC_PMFACSB_TCAM_HIT_INDICATIONm:
            case IPPD_PMFBCS_TCAM_HIT_INDICATIONm:
            case IPPE_PRTCAM_TCAM_HIT_INDICATIONm:
            case IPPE_LLRCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTACS_TCAM_HIT_INDICATIONm:
            case IPPF_VTCCS_TCAM_HIT_INDICATIONm:
            case IPPF_VTBCS_TCAM_HIT_INDICATIONm:
            case KAPS_RPB_TCAM_HIT_INDICATIONm:
            case KAPS_TCAM_HIT_INDICATIONm:
            case IPPA_VTDCS_TCAM_BANKm:
            case IPPA_VTECS_TCAM_BANKm:
            case IPPB_FLPACS_TCAM_BANKm:
            case IPPB_FLPBCS_TCAM_BANKm:
            case MDB_EEDB_ENTRY_BANKm:
            case MDB_MEM_02000000m:
                /*
                 * NOT real memories. 
                 */
            case MDB_EOEM_0m:
            case MDB_EOEM_1m:
            case MDB_ESEMm:
            case MDB_EXEM_1m:
            case MDB_EXEM_2m:
            case MDB_GLEM_0m:
            case MDB_GLEM_1m:
            case MDB_IOEM_0m:
            case MDB_IOEM_1m:
            case MDB_ISEM_1m:
            case MDB_ISEM_2m:
            case MDB_ISEM_3m:
            case MDB_MC_IDm:
            case MDB_RMEPm:
            case MDB_ARM_MEM_0m:
                /*
                 * Dynamic and should be skipped. - ASIC 
                 */
            case EVNT_ASSIGNED_CMIC_ENGINE_TABLEm:
            case ERPP_EPMFCS_TCAM_BANKm:
            case IPPC_PMFACSA_TCAM_BANKm:
            case IPPC_PMFACSB_TCAM_BANKm:
            case IPPD_PMFBCS_TCAM_BANKm:
            case IPPE_LLRCS_TCAM_BANKm:
            case IPPE_PRTCAM_TCAM_BANKm:
            case IPPF_VTACS_TCAM_BANKm:
            case IPPF_VTBCS_TCAM_BANKm:
            case IPPF_VTCCS_TCAM_BANKm:
            case IPPB_MPLS_LABEL_TYPES_CAM_0m:
            case IPPB_MPLS_LABEL_TYPES_CAM_1m:
            case TCAM_TCAM_ACTION_HIT_INDICATIONm:
            case TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm:
            case TCAM_TCAM_BANKm:

                /*
                 * After consulting with ASIC not real memories
                 */
            case CLPORT_WC_UCMEM_DATAm:
            case DPC_DRAM_PHY_REGISTER_ACCESSm:
            case FLEXEWP_TINY_MAC_MEMORIESm:
            case FLEXEWP_TINY_MAC_REGISTERSm:
            case FSCL_FLEXE_CORE_REGISTERSm:
            case OAMP_CLS_FLEX_CRC_TCAMm:
            case OAMP_CLS_TRAP_CODE_TCAMm:
            case OAMP_PE_0_PROG_TCAMm:
            case OAMP_PE_1_PROG_TCAMm:
            case OAMP_RSP_TCAMm:
            case OAMP_RX_OAM_ID_TCAMm:
                /*
                 * Not a real memory 
                 */
            case OAMP_RXP_R_MEP_INDEX_FIFOm:
            case CFC_NIF_ILU_RX_CALm:
            case CFC_NIF_ILU_TX_CALm:
            case FASIC_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm:
            case FASIC_BRCM_RAM_S_W_R_70_6400_R_2_ECC_RBUSm:
            case FSAR_BRCM_RAM_S_W_R_60_80_R_2_ECC_RBUSm:
            /** CDPORT block excluded memories */
            case RX_LKUP_1588_MEM_400Gm:
            case RX_LKUP_1588_MEM_MPP0m:
            case RX_LKUP_1588_MEM_MPP1m:
            case SPEED_ID_TABLEm:
            case SPEED_PRIORITY_MAP_TBLm:
            case TX_LKUP_1588_MEM_400Gm:
            case TX_LKUP_1588_MEM_MPP0m:
            case TX_LKUP_1588_MEM_MPP1m:
            case UM_TABLEm:
            case CDMIB_MEMm:
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(unit, "Skipping Jericho 2 excluded memory %s\n"), SOC_MEM_NAME(unit, mem)));
                return rv;
            default:
                break;
        }
    }
#endif
    index_min = soc_mem_index_min(unit, mem);
    count = soc_mem_index_count(unit, mem);
#ifdef BCM_DNX_SUPPORT
    min_array_index = max_array_index = 0;
    if (SOC_IS_DNX(unit) && SOC_MEM_IS_ARRAY(unit, mem))
    {
        min_array_index = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        max_array_index = (SOC_MEM_NUMELS(unit, mem) - 1) + min_array_index;
    }
#endif

    SOC_MEM_BLOCK_ITER(unit, mem, copyno)
    {
        /*
         * Perfom test 
         */
        test_run_memory_stat.mem_test_num++;
        LOG_VERBOSE(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Testing memory %s\n"), SOC_MEM_NAME(unit, mem)));
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)
            && (SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_MIN(unit, mem)) == SOC_BLK_CDMAC
                || SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_MIN(unit, mem)) == SOC_BLK_CDPORT))
        {
            bcm_pbmp_t valid_ports;
            bcm_port_t valid_port;
            int phy_port = 0;
            int cd_block = 0;
            if (dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &valid_ports) < 0)   /* Get
                                                                                                                         * *
                                                                                                                         * NIF
                                                                                                                         * *
                                                                                                                         * logical 
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * port 
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * * *
                                                                                                                         * bitmap 
                                                                                                                         */
            {
                break;
            }
            BCM_PBMP_ITER(valid_ports, valid_port)
            {
                phy_port = SOC_INFO(unit).port_l2p_mapping[valid_port]; /* Get NIF port first phy */
                cd_block = SOC_PORT_BLOCK(unit, phy_port);
                if (copyno == cd_block)
                {
                    copyno = copyno;
                    break;
                }
            }
            if (copyno != cd_block)
            {
                continue;
            }
        }
#endif
        /** perform array memory */
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit) && SOC_MEM_IS_ARRAY(unit, mem))
        {
            for (array_idx = min_array_index; array_idx <= max_array_index; array_idx++)
            {
                if (do_dump_table_array(unit, mem, copyno, array_idx, index_min, count, flags) != CMD_OK)
                {
                    test_run_memory_stat.mem_fail_num++;
                    rv = SOC_E_FAIL;
                    break;
                }
            }
            if (array_idx > max_array_index)
            {
                test_run_memory_stat.mem_pass_num++;
            }
        }
        else
#endif
        if ((do_dump_table_single(unit, mem, copyno, index_min, count, flags)) != CMD_OK)
        {
            test_run_memory_stat.mem_fail_num++;
            rv = SOC_E_FAIL;
            /*
             * break; 
             */
        }
        else
        {
            test_run_memory_stat.mem_pass_num++;
        }
    }

    return rv;
}

char tr6_test_usage[] = "TR6 memory write & periodic read usage:\n" " \n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
    "diff <value>                  -  (1 - dump table changes  0  - dump table all)\n"
    "Help=<1/0>                      -  Specifies if tr 8 help is on and exit or off (off by default)\n"
    "IncPm <1/0>                     -  Specifies if to include port macro blocks in tr (1-include 0-exclude(default))\n"
    "StartFrom <integer>             -  Specifies from what mem id to start the tr(default 0)\n"
    "MaxSize <integer|-1>            -  table with size in bytes above MaxSize will excluded from the test(default -1 all tables included)\n"
    "\n";
#endif /* COMPILER_STRING_CONST_LIMIT */

cmd_result_t
do_dump_memories(
    int unit,
    args_t * a)
{
    char *an_arg;
    int rv = CMD_OK;
    uint32 data[] = { DUMP_TABLE_ALL, 0, 0, -1 };

    an_arg = ARG_GET(a);

    sal_memset(&test_run_memory_stat, 0x0, sizeof(test_run_memory_stat_t));

    while (an_arg)
    {
        if (sal_strcasecmp(an_arg, "diff") == 0)
        {
            data[0] = DUMP_TABLE_CHANGED;
        }
        else if (SOC_IS_JERICHO(unit) && sal_strcasecmp(an_arg, "IncPm") == 0)
        {
            data[1] = 1;
        }
        else if (sal_strcasecmp(an_arg, "Help") == 0)
        {
            cli_out("%s\n", tr6_test_usage);
            return CMD_OK;
        }
        else if (sal_strcasecmp(an_arg, "StartFrom") == 0)
        {
            char *sf = ARG_GET(a);
            data[2] = sal_ctoi(sf, 0);
        }
        else if (sal_strcasecmp(an_arg, "MaxSize") == 0)
        {
            char *sf = ARG_GET(a);
            data[3] = sal_ctoi(sf, 0);
        }
        else
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U
                      (unit, "ERROR: unrecognized argument to DUMP SOC: %s\n Run TR without arguments or use Help\n "),
                      an_arg));
            return CMD_FAIL;
        }

        an_arg = ARG_GET(a);
    }

    if ((soc_mem_iterate(unit, sand_mem_dump_iter_callback, data)) < 0)
    {
        rv = CMD_FAIL;
    }

    /*
     * Print statistics summary 
     */
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Test statistics:\n")));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "Number of Valid memories: %d\n"), test_run_memory_stat.mem_valid_num));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit, "Number of Tested memories: %d (Skiped %d)\n"), test_run_memory_stat.mem_test_num,
              test_run_memory_stat.mem_valid_num - test_run_memory_stat.mem_test_num));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of Pass memories: %d\n"), test_run_memory_stat.mem_pass_num));
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Number of Fail memories: %d\n"), test_run_memory_stat.mem_fail_num));

    return rv;
}

int
sand_mem_test_iter_callback(
    int unit,
    soc_mem_t mem,
    void *data)
{
    int rv = SOC_E_NONE;
    void *p;

    /*
     * Memory is valid for this test if it is valid, and not ro / wo. 
     */
    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) || (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY))
    {
        return rv;
    }

#if defined (BCM_DFE_SUPPORT)
    if (SOC_IS_DFE(unit))
    {
        int is_filtered = 0;

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_mem_filter, (unit, mem, &is_filtered));
        if (rv != SOC_E_NONE)
        {
            return rv;
        }
        if (is_filtered)
        {
            return SOC_E_NONE;
        }

    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        switch (mem)
        {
            case BRDC_QRH_CUCTm:
            case BRDC_QRH_DUCTm:
            case BRDC_QRH_FFLBm:
            case BRDC_QRH_MCLBTm:
            case BRDC_QRH_MCSFFm:
            case BRDC_QRH_MNOLm:
            case FSRD_FSRD_PROM_MEMm:
            case QRH_CUCTm:
            case QRH_DUCTm:
            case QRH_FFLBm:
            case QRH_MCLBTm:
            case QRH_MCSFFm:
            case QRH_MNOLm:
            case ECI_AUTO_DOC_NAME_48m:
            case MCT_MCTm:
            case RTP_FLGMm:
            case RTP_GCILBTm:
            case RTP_MCLBTPm:
            case RTP_MCLBTSm:
            case RTP_MCSFFPm:
            case RTP_MCTm:
            case RTP_MNOLGm:
            case RTP_MNOLRm:
            case RTP_MULTI_CAST_TABLE_UPDATEm:
            case RTP_MULTI_TBm:
            case RTP_RCGLBTm:
            case RTP_RMHMTm:
            case RTP_RRMAm:
            case RTP_SCTINCm:
            case RTP_SGRm:
            case RTP_SLSCTm:
            case RTP_TOTSFm:
                return 1;       /* Skip these tables */
            default:
                break;
        }
    }
#endif
    if (mem_test_default_init(unit, mem, &p) < 0)
    {
        return CMD_FAIL;
    }

    if (mem_test(unit, data, p) < 0)
    {
        rv = CMD_FAIL;
    }

    if (mem_test_done(unit, p) < 0)
    {
        return CMD_FAIL;
    }

    return rv;
}

cmd_result_t
do_mem_test(
    int unit,
    args_t * a)
{
    int rv = SOC_E_NONE;
    if ((soc_mem_iterate(unit, sand_mem_test_iter_callback, a)) < 0)
    {
        rv = CMD_FAIL;
    }

    return rv;
}
