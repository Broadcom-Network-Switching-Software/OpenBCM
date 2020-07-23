/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_MEM

#include <shared/bsl.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/mem.h>


#include <soc/mcm/allenum.h>

#include <soc/chip.h>


#if defined (BCM_SAND_SUPPORT)
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_intr_corr_act_func.h>
#include <soc/sand/sand_ser_correction.h>
#endif


#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dcmn/dcmn_intr_handler.h>
#endif /* defined(BCM_PETRA_SUPPORT) || defined (BCM_DFE_SUPPORT) */

#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_mgmt.h>
#endif

#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/counters.h>
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <soc/dpp/ARAD/arad_interrupts.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/mdb_ser.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#endif

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_mem.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#endif

#include "testlist.h"

extern int bcm_common_linkscan_enable_set(int,int);

#if defined (BCM_SAND_SUPPORT)

#define MAX_NOF_ATTAMPTS_TO_GET_A_RW_BIT 100
#define BIT_IN_UINT32 (sizeof(uint32)*8)
#define MAX_SER_RETRIES 10

typedef enum {
    ECC1 = 0,
    ECC2
} error_type;

/* ser_test_params */
typedef struct ser_test_params_s {
    int unit;
    char* mem_name_parse;           /* memory name to parse */
    soc_mem_t mem;                  /* memory to test*/
    char* index_parse;              /* index to parse */
    int index;                      /* index to test */
    char* array_index_parse;        /* array index to parse */
    int array_index;                /* array index to test */
    int copyno;                     /* block number to test */
    int cache_state;                /* use cache state test */
    uint32 interrupt;  /* interrupt to check result */
    error_type error;               /* error type to generate (ECC1, ECC2)*/
    uint8 run_all;                  /* if set we run the tr for each of the chip memories,otherwise for a specific memory as given to mem_name_parse*/
    int start_from;              /* strat from id relevant if run_all set */
    uint32 count;              /* how many memories to run on */
    uint32 total_counter;            /* total number of memories  run the test*/
    uint32 skipped_counter;           /* total number of memories  skipped the test*/
    uint32 unjustified_skipped_counter; /* total number of memories  unjustified skipped the test and need to be check individually*/
    uint32 error_counter;            /* total number of memories  failed the test*/

    uint32 param_counter;            /* total number of memories of param error */
    uint32 timeout_counter;            /* total number of memories of timeout error */
    
    int help;                       /* show usage */

    soc_mem_t error_mem_list[NUM_SOC_MEM];  /* id list of memories  failed the test  */
    int test_dynamic;
    int xor_test;

#ifdef BCM_DNX_SUPPORT
    uint8   mdb;
    uint32  mdb_error_counter;
#endif
}ser_test_params_t;



#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
typedef struct arad_sch_ser_map_s {
    soc_mem_t mem;
    int    interrupt;
    soc_reg_t error_initiate_reg;
    soc_field_t error_initiate_field;
    int nof_errors;  

}arad_sch_ser_map_t;

static arad_sch_ser_map_t arad_sch_ser_map[] = {

    {SCH_HR_SCHEDULER_CONFIGURATION_SHCm, ARAD_INT_SCH_SHCPARERROR,SCH_PAR_ERR_INITIATEr,SHC_PAR_ERROR_INITf,1},
    {SCH_CL_SCHEDULERS_TYPE__SCTm, ARAD_INT_SCH_SCTPARERROR,SCH_PAR_ERR_INITIATEr,SCT_PAR_ERROR_INITf,1},
    {SCH_MEM_04700000m, ARAD_INT_SCH_CSDTPARERROR,SCH_PAR_ERR_INITIATEr,CSDT_PAR_ERROR_INITf,1},
    {SCH_DUAL_SHAPER_MEMORY__DSMm, ARAD_INT_SCH_DSMPARERROR,SCH_PAR_ERR_INITIATEr,DSM_PAR_ERROR_INITf,1},
    {SCH_TOKEN_MEMORY_CONTROLLER__TMCm, ARAD_INT_SCH_TMCPARERROR,SCH_PAR_ERR_INITIATEr,TMC_LSB_PAR_ERROR_INITf,1},
    {SCH_CL_SCHEDULERS_TYPE_SCTm, ARAD_INT_SCH_SCTPARERROR,SCH_PAR_ERR_INITIATEr,SCT_PAR_ERROR_INITf,1},
    {SCH_MEM_03300000m, ARAD_INT_SCH_FLTCLECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLTCL_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03300000m, ARAD_INT_SCH_FLTCLECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLTCL_2B_ECC_ERROR_INITf,2},
    {SCH_PIR_SHAPERS_STATIC_TABEL__PSSTm, ARAD_INT_SCH_PSSTPARERROR,SCH_PAR_ERR_INITIATEr,PSST_PAR_ERROR_INITf,1},
    {SCH_MEM_03000000m, ARAD_INT_SCH_FLHHRECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLHHR_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03000000m, ARAD_INT_SCH_FLHHRECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLHHR_2B_ECC_ERROR_INITf,2},
    {SCH_CIR_SHAPERS_STATIC_TABEL__CSSTm, ARAD_INT_SCH_CSSTPARERROR,SCH_PAR_ERR_INITIATEr,CSST_PAR_ERROR_INITf,1},
    {SCH_MEM_01300000m, ARAD_INT_SCH_FDMDECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FDMD_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_01300000m, ARAD_INT_SCH_FDMDECCERROR,SCH_ECC_ERR_2B_INITIATEr,FDMD_2B_ECC_ERROR_INITf,2},
    {SCH_DEVICE_RATE_MEMORY__DRMm, ARAD_INT_SCH_DRMPARERROR,SCH_PAR_ERR_INITIATEr,DRM_PAR_ERROR_INITf,1},
    {SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm, ARAD_INT_SCH_FDMSPARERROR,SCH_PAR_ERR_INITIATEr,FDMS_PAR_ERROR_INITf,1},
    {SCH_FLOW_SUB_FLOW__FSFm, ARAD_INT_SCH_FSFPARERROR,SCH_PAR_ERR_INITIATEr,FSF_PAR_ERROR_INITf,1},
    {SCH_MEM_01700000m, ARAD_INT_SCH_DCDECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,DCD_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_01700000m, ARAD_INT_SCH_DCDECCERROR,SCH_ECC_ERR_2B_INITIATEr,DCD_2B_ECC_ERROR_INITf,2},
    {SCH_MEM_01400000m, ARAD_INT_SCH_SHDDECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,SHDD_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_01400000m, ARAD_INT_SCH_SHDDECCERROR,SCH_ECC_ERR_2B_INITIATEr,SHDD_2B_ECC_ERROR_INITf,2},
    {SCH_MEM_04D00000m, ARAD_INT_SCH_PSDDPARERROR,SCH_PAR_ERR_INITIATEr,PSDD_PAR_ERROR_INITf,1},
    {SCH_MEM_01C00000m, ARAD_INT_SCH_DPNPARERROR,SCH_PAR_ERR_INITIATEr,DPN_PAR_ERROR_INITf,1},
    {SCH_PORT_SCHEDULER_WEIGHTS_PSWm, ARAD_INT_SCH_PSWPARERROR,SCH_PAR_ERR_INITIATEr,PSW_PAR_ERROR_INITf,1},
    {SCH_CL_SCHEDULERS_CONFIGURATION_SCCm, ARAD_INT_SCH_SCCPARERROR,SCH_PAR_ERR_INITIATEr,SCC_PAR_ERROR_INITf,1},
    {SCH_MEM_03400000m, ARAD_INT_SCH_FLHFQECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLHFQ_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03400000m, ARAD_INT_SCH_FLHFQECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLHFQ_2B_ECC_ERROR_INITf,2},
    {SCH_MEM_03100000m, ARAD_INT_SCH_FLTHRECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLTHR_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03100000m, ARAD_INT_SCH_FLTHRECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLTHR_2B_ECC_ERROR_INITf,2},
    {SCH_FLOW_TO_FIP_MAPPING_FFMm, ARAD_INT_SCH_FFMPARERROR,SCH_PAR_ERR_INITIATEr,FFM_PAR_ERROR_INITf,1},
    {SCH_SHARED_DEVICE_RATE_SHARED_DRMm, ARAD_INT_SCH_DRMPARERROR,SCH_PAR_ERR_INITIATEr,DRM_PAR_ERROR_INITf,1},
    {SCH_FLOW_GROUP_MEMORY_FGMm, ARAD_INT_SCH_FGMPARERROR,SCH_PAR_ERR_INITIATEr,FGM_PAR_ERROR_INITf,1},
    {SCH_MEM_03500000m, ARAD_INT_SCH_FLTFQECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLTFQ_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03500000m, ARAD_INT_SCH_FLTFQECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLTFQ_2B_ECC_ERROR_INITf,2},
    {SCH_FLOW_TO_FIP_MAPPING__FFMm, ARAD_INT_SCH_FFMPARERROR,SCH_PAR_ERR_INITIATEr,FFM_PAR_ERROR_INITf,1},
    {SCH_CL_SCHEDULERS_CONFIGURATION__SCCm, ARAD_INT_SCH_SCCPARERROR,SCH_PAR_ERR_INITIATEr,SCC_PAR_ERROR_INITf,1},
    {SCH_PORT_SCHEDULER_WEIGHTS__PSWm, ARAD_INT_SCH_PSWPARERROR,SCH_PAR_ERR_INITIATEr,PSW_PAR_ERROR_INITf,1},
    {SCH_DUAL_SHAPER_MEMORY_DSMm, ARAD_INT_SCH_DSMPARERROR,SCH_PAR_ERR_INITIATEr,DSM_PAR_ERROR_INITf,1},
    {SCH_FLOW_GROUP_MEMORY__FGMm, ARAD_INT_SCH_FGMPARERROR,SCH_PAR_ERR_INITIATEr,FGM_PAR_ERROR_INITf,1},
    {SCH_FLOW_TO_QUEUE_MAPPING__FQMm, ARAD_INT_SCH_FQMPARERROR,SCH_PAR_ERR_INITIATEr,FQM_PAR_ERROR_INITf,1},
    {SCH_FLOW_STATUS_MEMORY__FSMm, ARAD_INT_SCH_FSMECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FSM_PORT_A_1B_ECC_ERROR_INITf,2},
    {SCH_FLOW_STATUS_MEMORY__FSMm, ARAD_INT_SCH_FSMECCERROR,SCH_ECC_ERR_2B_INITIATEr,FSM_PORT_A_2B_ECC_ERROR_INITf,2},
    {SCH_HR_SCHEDULER_CONFIGURATION__SHCm, ARAD_INT_SCH_SHCPARERROR,SCH_PAR_ERR_INITIATEr,SHC_PAR_ERROR_INITf,1},
    {SCH_FLOW_TO_QUEUE_MAPPING_FQMm, ARAD_INT_SCH_FQMPARERROR,SCH_PAR_ERR_INITIATEr,FQM_PAR_ERROR_INITf,1},
    {SCH_MEM_03200000m, ARAD_INT_SCH_FLHCLECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,FLHCL_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_03200000m, ARAD_INT_SCH_FLHCLECCERROR,SCH_ECC_ERR_2B_INITIATEr,FLHCL_2B_ECC_ERROR_INITf,2},
    {SCH_PIR_SHAPERS_STATIC_TABEL_PSSTm, ARAD_INT_SCH_PSSTPARERROR,SCH_PAR_ERR_INITIATEr,PSST_PAR_ERROR_INITf,1},
    {SCH_MEM_01600000m, ARAD_INT_SCH_DHDECCERRORFIXED,SCH_ECC_ERR_1B_INITIATEr,DHD_1B_ECC_ERROR_INITf,2},
    {SCH_MEM_01600000m, ARAD_INT_SCH_DHDECCERROR,SCH_ECC_ERR_2B_INITIATEr,DHD_2B_ECC_ERROR_INITf,2},
    {SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, ARAD_INT_SCH_FDMSPARERROR,SCH_PAR_ERR_INITIATEr,FDMS_PAR_ERROR_INITf,1},
    {SCH_CIR_SHAPERS_STATIC_TABEL_CSSTm, ARAD_INT_SCH_CSSTPARERROR,SCH_PAR_ERR_INITIATEr,CSST_PAR_ERROR_INITf,1},
    {SCH_FLOW_SUB_FLOW_FSFm, ARAD_INT_SCH_FSFPARERROR,SCH_PAR_ERR_INITIATEr,FSF_PAR_ERROR_INITf,1},
    {SCH_MEM_04A00000m, ARAD_INT_SCH_PSDTPARERROR,SCH_PAR_ERR_INITIATEr,PSDT_PAR_ERROR_INITf,1},
    {INVALIDm}
};
/* } */
#endif
static ser_test_params_t *ser_parameters[SOC_MAX_NUM_DEVICES];
void update_mem_int(int unit)
{
    ser_test_params_t *ser_test_params = ser_parameters[unit];
    soc_block_type_t block;
    soc_mem_t mem = ser_test_params->mem;
    sand_block_control_info_t*  sand_bloc_info;

    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));

    sand_bloc_info = sand_get_block_control_info(unit);
    ser_test_params->interrupt = sand_bloc_info[block].parity_int;
    if (SOC_MEM_FIELD_VALID(unit,mem,ECCf)) {
         ser_test_params->interrupt = ser_test_params->error ==ECC1 ? sand_bloc_info[block].ecc1_int : sand_bloc_info[block].ecc2_int;
    }
}


#ifdef BCM_DNX_SUPPORT
uint32 sal_ser_data_rand(void) 
{
    static uint32 next = 1;
    
    next = next * 1103515245 + 12345;
    return next;
}


int dnx_mdb_phymem_inject_ecc1b(
    int unit,
    int interrupt,
    soc_mem_t phy_mem,
    int copyno,
    uint32 array_idx,
    uint32 entry_idx)
{
    int         blk, random_bit=0;
    soc_reg_t   gmo_reg = ECI_GLOBAL_MEM_OPTIONSr;
    bcm_switch_event_control_t type;
    bcm_switch_event_control_t stat;
    uint32 ii, succeed, old_count=0, new_count=0, sleep_bit = 2000000;
    uint32 int_mask;
    uint32 init_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 read_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 orig_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 new_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];

    SOC_INIT_FUNC_DEFS;

    sal_memset(entry_data, 0, sizeof(entry_data));
    sal_memset(read_entry_data, 0, sizeof(read_entry_data));
    sal_memset(orig_entry_data, 0, sizeof(orig_entry_data));
    sal_memset(new_entry_data, 0, sizeof(new_entry_data));
    sal_memset(init_entry_data, 0, sizeof(init_entry_data));

    blk = copyno + SOC_MEM_BLOCK_MIN(unit, phy_mem);

    type.action     = bcmSwitchEventMask;
    type.event_id   = interrupt; 
    type.index      = 0;
    bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, &int_mask);
    if (int_mask) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "Interrupt %d was masked so skipping the test for Mem:%s\n"), 
                interrupt,SOC_MEM_NAME(unit, phy_mem))); 
        _SOC_IF_ERR_EXIT(SOC_E_PARAM);
    }

    /* Record the given interrupt's count */
    stat.event_id = interrupt; 
    stat.index    = copyno;
    stat.action   = bcmSwitchEventStat;
    bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, stat, &old_count);

    /* First initital MDB entry with random value */
    init_entry_data[0] = sal_ser_data_rand();
    init_entry_data[1] = sal_ser_data_rand();
    init_entry_data[2] = sal_ser_data_rand();
    init_entry_data[3] = sal_ser_data_rand() & 0x00FFFFFF;
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, phy_mem, blk, array_idx, entry_idx, init_entry_data));

    /* Read entry */
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_idx, entry_idx, entry_data));
    sal_memcpy(orig_entry_data, entry_data, sizeof(orig_entry_data));

    /* flip 1bit of entry data at random position */
    random_bit = sal_rand() % MDB_SER_ENTRY_SIZE_IN_BITS;
    entry_data[random_bit/BIT_IN_UINT32] ^= (1u << random_bit%BIT_IN_UINT32);

    /** Bypass PARITY and ECC **/
    _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, copyno, CPU_BYPASS_ECC_PARf, 0x1));

    /** Write modified entry **/ 
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, phy_mem, blk, array_idx, entry_idx, entry_data));

    /** Disable Bypass PARITY and ECC **/
    _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, copyno, CPU_BYPASS_ECC_PARf, 0x0));

    /** Re-activate PARITY and ECC and Read memory --> Should Trigger the Interrupt **/
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_idx, entry_idx, read_entry_data));

    /** Check again given interrupt's count **/
    sal_usleep(sleep_bit);    
    bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, stat, &new_count);
    succeed=0;
    for (ii=0; ii<MAX_SER_RETRIES; ii++) 
    {
        if (new_count <= old_count) 
        {
            /* if the counter test failed lets try read test maybe the nmemory already fixed*/
            sal_usleep(sleep_bit);
            cli_out("retry reading counter %d\n", ii);
            bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, stat, &new_count);
            continue;
        }
        succeed=1;
        break;
    }
    if (!succeed)
    {
        _SOC_IF_ERR_EXIT(SOC_E_TIMEOUT);
    }

    /* If ecc1 correction succeed. we compare the corrected data with the original data*/
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_idx, entry_idx, new_entry_data));

    if (sal_memcmp(new_entry_data, orig_entry_data, sizeof(new_entry_data))) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "Warning:Memory %s value doesnt corrected to it original value\n"), 
                        SOC_MEM_NAME(unit, phy_mem)));
        _SOC_IF_ERR_EXIT(SOC_E_INTERNAL);
    }

exit:
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\n\n********dnx_mdb_phymem_inject_ecc1b, _rv=%d********\n"),
            _rv));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "phy_mem=%s, copyno=%d, array=%d, entry=%d, random_bit=%d\n"),
            SOC_MEM_NAME(unit, phy_mem), copyno, array_idx, entry_idx, random_bit));

    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\tinterrupt=%d, old_count=%d, new_count=%d\n"),
            interrupt, old_count, new_count));

    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "init_entry_data", init_entry_data[0], init_entry_data[1], init_entry_data[2], init_entry_data[3]));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "orig_entry_data", orig_entry_data[0], orig_entry_data[1], orig_entry_data[2], orig_entry_data[3]));

    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "write entry_data", entry_data[0], entry_data[1], entry_data[2], entry_data[3]));

    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "read entry_data", read_entry_data[0], read_entry_data[1], read_entry_data[2], read_entry_data[3]));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "new_entry_data", new_entry_data[0], new_entry_data[1], new_entry_data[2], new_entry_data[3]));
    
    SOC_FUNC_RETURN;
}

int dnx_mdb_phymem_write_read_check(
    int unit,
    soc_mem_t phy_mem,
    int copyno,
    uint32 array_idx,
    uint32 entry_idx)
{
    int    blk;
    uint32 init_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 read_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ecc=0, noMatch=0;
    mdb_macro_types_e macro_type;
    uint32 macro_index;

    SOC_INIT_FUNC_DEFS;

    sal_memset(init_entry_data, 0, sizeof(init_entry_data));
    sal_memset(read_entry_data, 0, sizeof(read_entry_data));

    blk = copyno + SOC_MEM_BLOCK_MIN(unit, phy_mem);

    _SOC_IF_ERR_EXIT(mdb_ser_map_mem_to_macro_index(unit, blk, phy_mem, &macro_type, &macro_index));

    /* First initital MDB entry with random value */
    init_entry_data[0] = sal_ser_data_rand();
    init_entry_data[1] = sal_ser_data_rand();
    init_entry_data[2] = sal_ser_data_rand();
    init_entry_data[3] = sal_ser_data_rand() & 0x00FFFFFF;
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, phy_mem, blk, array_idx, entry_idx, init_entry_data));

    /* Read entry */
    _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_idx, entry_idx, read_entry_data));

    ecc = (read_entry_data[3] & 0xFF000000) >> 24;
    read_entry_data[3] = read_entry_data[3] & 0x00FFFFFF;
    noMatch = 0;
    if (sal_memcmp(init_entry_data, read_entry_data, sizeof(init_entry_data))) 
    {
        noMatch = 1;
    }

exit:
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\n\n********dnx_mdb_phymem_write_read_check, noMatch=%d********\n"), noMatch));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "phy_mem=%s, copyno=%d, array=%d, entry=%d, macro=%s-%d\n"),
            SOC_MEM_NAME(unit, phy_mem), copyno, array_idx, entry_idx,
            macro_type == MDB_MACRO_A ? "MDB_MACRO_A" : "MDB_MACRO_B", macro_index));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "phy_mem=%s, copyno=%d, array=%d, entry=%d\n"),
            SOC_MEM_NAME(unit, phy_mem), copyno, array_idx, entry_idx));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
            "init_entry_data", init_entry_data[0], init_entry_data[1], init_entry_data[2], init_entry_data[3]));
    LOG_INFO(BSL_LS_APPL_TESTS,
        (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X,  ecc=0x%02x\n"),
            "read_entry_data", read_entry_data[0], read_entry_data[1], read_entry_data[2], read_entry_data[3], ecc));

    SOC_FUNC_RETURN;
}


int dnx_mdb_phymem_cluster_write_read_check(
    int unit,
    soc_mem_t phy_mem,
    int copyno,
    uint32 array_idx)
{
    int    blk;
    uint32 init_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 read_entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ecc=0, noMatch=0;
    mdb_macro_types_e macro_type;
    uint32 min_index, max_index, entry_idx, ii;
    uint32 macro_index;

    SOC_INIT_FUNC_DEFS;

    min_index = parse_memory_index(unit, phy_mem, "min");
    max_index = parse_memory_index(unit, phy_mem, "max");

    blk = copyno + SOC_MEM_BLOCK_MIN(unit, phy_mem);
    _SOC_IF_ERR_EXIT(mdb_ser_map_mem_to_macro_index(unit, blk, phy_mem, &macro_type, &macro_index));
    cli_out("\nMDB memory:%s(%d):  %s-%d, Array=%d", SOC_MEM_NAME(unit, phy_mem), copyno, 
            macro_type == MDB_MACRO_A ? "MDB_MACRO_A" : "MDB_MACRO_B", macro_index, array_idx);

    for (entry_idx=min_index; entry_idx<=max_index; entry_idx++)
    {
        if ((entry_idx % 1024) == 0)
        {
            cli_out(".");
        }
        sal_memset(init_entry_data, 0, sizeof(init_entry_data));
        sal_memset(read_entry_data, 0, sizeof(read_entry_data));

        /* First initital MDB entry with random value */
        init_entry_data[0] = sal_ser_data_rand();
        init_entry_data[1] = sal_ser_data_rand();
        init_entry_data[2] = sal_ser_data_rand();
        init_entry_data[3] = sal_ser_data_rand() & 0x00FFFFFF;
        _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, phy_mem, blk, array_idx, entry_idx, init_entry_data));

        for (ii=0; ii<2; ii++)
        {
            /* Read entry */
            _SOC_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_idx, entry_idx, read_entry_data));

            ecc = (read_entry_data[3] & 0xFF000000) >> 24;
            read_entry_data[3] = read_entry_data[3] & 0x00FFFFFF;
            noMatch = 0;
            if (sal_memcmp(init_entry_data, read_entry_data, sizeof(init_entry_data))) 
            {
                noMatch = 1;
                break;
            }
        }

        if (noMatch)
        {
            break;
        }
    }

exit:
    if (noMatch)
    {
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\n\n********dnx_mdb_phymem_cluster_write_read_check, noMatch!!!!********\n")));
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "phy_mem=%s, copyno=%d, array=%d, entry=%d, macro=%s-%d\n"),
                SOC_MEM_NAME(unit, phy_mem), copyno, array_idx, entry_idx,
                macro_type == MDB_MACRO_A ? "MDB_MACRO_A" : "MDB_MACRO_B", macro_index));
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "phy_mem=%s, copyno=%d, array=%d, entry=%d\n"),
                SOC_MEM_NAME(unit, phy_mem), copyno, array_idx, entry_idx));
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X\n"),
                "init_entry_data", init_entry_data[0], init_entry_data[1], init_entry_data[2], init_entry_data[3]));
        LOG_INFO(BSL_LS_APPL_TESTS,
            (BSL_META_U(unit, "\t%-26s: %08X %08X %08X %08X,  ecc=0x%02x\n"),
                "read_entry_data", read_entry_data[0], read_entry_data[1], read_entry_data[2], read_entry_data[3], ecc));

        _SOC_IF_ERR_CONT(SOC_E_MEMORY);
    }
    SOC_FUNC_RETURN;
}


int dnx_mdb_phymem_ser_verify(int unit, soc_mem_t mem, uint8 mdb_flag)
{
    soc_block_type_t block;
    int         rc, instance, copyno, interrupt;
    soc_mem_t   phy_mem;
    uint32 array_idx, entry_idx;
    sand_block_control_info_t*  sand_bloc_info;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_mdb_mem_map_t *mdb_mem_map;
    uint32 index;
    ser_test_params_t *ser_test_params = ser_parameters[unit];

    SOC_INIT_FUNC_DEFS;
    if (!SOC_IS_DNX(unit))
    {
        SOC_EXIT;
    }

    sand_bloc_info = sand_get_block_control_info(unit);
    if (mem != INVALIDm)
    {
        phy_mem = mem;
        block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, phy_mem)));
        interrupt = sand_bloc_info[block].ecc1_int;
        copyno = ser_test_params->copyno;
        array_idx = ser_test_params->array_index;
        entry_idx = ser_test_params->index;

        if (mdb_flag == 1)
        {
            rc = dnx_mdb_phymem_inject_ecc1b(unit, interrupt, phy_mem, copyno, array_idx, entry_idx);
        }
        else
        {
            rc = dnx_mdb_phymem_write_read_check(unit, phy_mem, copyno, array_idx, entry_idx);
        }
        if (rc != SOC_E_NONE)
        {
            ser_test_params->mdb_error_counter++;
        }
        _SOC_IF_ERR_EXIT(rc);
    }
    else
    {
        table_info = dnx_data_intr.ser.mdb_mem_map_info_get(unit);
        for (index = 0; index < table_info->key_size[0]; index++)
        {
            mdb_mem_map = dnx_data_intr.ser.mdb_mem_map_get(unit, index);
            phy_mem = mdb_mem_map->phy_mem;

            copyno  = 0;
            if (!SOC_MEM_IS_VALID(unit, phy_mem))
            {
                continue;
            }
            SOC_MEM_BLOCK_ITER(unit, phy_mem, instance)
            {
                int    rand_idx;
                uint32 min_array, max_array;
                uint32 min_index, max_index;

                /* get random array index */
                min_array = parse_memory_array_index(unit, phy_mem, "min");
                max_array = parse_memory_array_index(unit, phy_mem, "max");
                for (array_idx=min_array; array_idx<=max_array; array_idx++)
                {
                    /* get random entry index */
                    min_index = parse_memory_index(unit, phy_mem, "min");
                    max_index = parse_memory_index(unit, phy_mem, "max");
                    rand_idx  = sal_rand() % (max_index - min_index);
                    entry_idx = min_index + rand_idx;

                    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, phy_mem)));
                    interrupt = sand_bloc_info[block].ecc1_int;

                    if (mdb_flag == 1)
                    {
                        rc = dnx_mdb_phymem_inject_ecc1b(unit, interrupt, phy_mem, copyno, array_idx, entry_idx);
                    }
                    else if (mdb_flag == 2)
                    {
                        rc = dnx_mdb_phymem_write_read_check(unit, phy_mem, copyno, array_idx, entry_idx);
                    }
                    else
                    {
                        rc = dnx_mdb_phymem_cluster_write_read_check(unit, phy_mem, copyno, array_idx);
                    }
                    if (rc != SOC_E_NONE)
                    {
                        ser_test_params->mdb_error_counter++;
                    }
                }

                copyno++; 
            }
        }
        if (ser_test_params->mdb_error_counter)
        {
            _SOC_IF_ERR_EXIT(_SHR_E_FAIL);
        }
    }

exit:
    if (ser_test_params->mdb_error_counter)
    {
        test_error(unit, "MDB Ser Test Failed!\n");
    }
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "MDB mem SER run mdb_flag=%d: mdb_error_counter=%d\n"), 
                                mdb_flag, ser_test_params->mdb_error_counter));
    SOC_FUNC_RETURN;
}
#endif

/*
 * Function:
 *      ser_test_get_block_num_unsafe
 * Purpose:
 *      finds a given memory block number after providing its copy number
 * Parameters:
 *      unit    - Device Number
 *      mem     - Memory to find its block number 
 *      copyno  - block copy number to look for
 * Returns:
 *      on success - block num
 *      on failure - -1
 */
int ser_test_get_block_num_unsafe(int unit, soc_mem_t mem, int copyno)
{
    int block_iter, min_block, max_block;

    /* Define range of blocks to iterate over */
    min_block = SOC_MEM_INFO(unit, mem).minblock;
    max_block = SOC_MEM_INFO(unit, mem).maxblock;

    /* Iterate over range and look for relevant copyno, and return the actual block number */
    for (block_iter = min_block; block_iter <= max_block; ++block_iter) 
    {
        if (SOC_BLOCK_INFO(unit, block_iter).number == copyno) 
        {
            return block_iter ;
        }
    }

    /* relevant copy of blk was not found */
    return -1;
}

char tr153_test_usage[] = 
"TR 153 memory ser test:\n"
" \n"
  "Memory=<value>                  -  Specifies a memory name to run test on - default is \"\"\n" 
  "Index=<value>                   -  Specifies index in memory to run test on - default is min index\n" 
  "ArrayIndex=<value>              -  Specifies array index in memory to run test on - default is min array index\n" 
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
  
  "CopyNo=<file name>              -  Specifies copy number of memory (for memory which exists in several blocks) - default is 0 \n" 
  "CacheState=<1/0>                -  Specifies if need to validate the Ser fix from the cache or that the interrupt appearance is sufficant - default is 0 \n"
  "ErrorType=<1/0>                 -  Specifies if Error type is <0> ECC 1bit or <1> ECC 2bit - if memory is protected by PARITY, this argument is ignored - default is 0\n"
  "RunAll=<1/0>                    -  Specifies if the test run on specific memory<0> Or perform for each memory<1> \n"
  "StartFrom=memName                    -  Specifies mem name to start from  relevant only if RunAll set \n"
  "MemCount=id                    -   Specifies how many memories to run on \n"
  "Help=<1/0>                      -  Specifies if tr 153 help is on and exit or off - default is off\n"
  "\n"
;
#endif



/*
 * Function:
 *      memory_ser_test_init
 * Purpose:
 *      takes care of all of init process for the ser test, get arguments and make a perliminery sanity check for given args
 * Parameters:
 *      unit    - Device Number
 *      a       - count of arguments 
 *      p       - actual arguments
 * Returns:
 *      BCM_E_XXX
 */
extern int memory_ser_test_init(int unit, args_t *a, void **p)
{
    ser_test_params_t *ser_test_params = NULL;
    int num_soc_mem = NUM_SOC_MEM;
    parse_table_t parse_table;
    int rv = BCM_E_INIT;
    char *start_from;

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_no_wb_test_set(unit, 1);
#endif
#endif

    /** allocate memory for DB for test parameters **/
    ser_test_params = ser_parameters[unit];
    if (ser_test_params == NULL) 
    {
        ser_test_params = sal_alloc(sizeof(ser_test_params_t), "ser_test");
        if (ser_test_params == NULL) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: cannot allocate memory test data\n"), ARG_CMD(a)));
            return -1;
        }
        sal_memset(ser_test_params, 0, sizeof(ser_test_params_t));
        ser_parameters[unit] = ser_test_params;
    }
    ser_test_params->start_from = 0;
    ser_test_params->count = NUM_SOC_MEM;
    ser_test_params->copyno = -1;

    /** seting default values **/
    parse_table_init(unit, &parse_table);
    parse_table_add(&parse_table,  "Memory",        PQ_STRING, "",       &(ser_test_params->mem_name_parse),    NULL);
    parse_table_add(&parse_table,  "Index",         PQ_STRING, "min",    &(ser_test_params->index_parse),       NULL);
    parse_table_add(&parse_table,  "ArrayIndex",    PQ_STRING, "min",    &(ser_test_params->array_index_parse), NULL);
    parse_table_add(&parse_table,  "CopyNo",        PQ_INT | PQ_DFL, 0,  &(ser_test_params->copyno),            NULL);
    parse_table_add(&parse_table,  "CacheState",    PQ_BOOL,   0,        &(ser_test_params->cache_state),       NULL);
    parse_table_add(&parse_table,  "ErrorType",     PQ_INT,    0,        &(ser_test_params->error),             NULL);
    parse_table_add(&parse_table,  "StartFrom",     PQ_STRING, "",       &(start_from),                         NULL);
    parse_table_add(&parse_table,  "MemCount",      PQ_DFL|PQ_INT,  &num_soc_mem,        &(ser_test_params->count),             NULL);
    parse_table_add(&parse_table,  "RunAll",        PQ_INT8,    0,        &(ser_test_params->run_all),           NULL);
    parse_table_add(&parse_table,  "Help",          PQ_INT,    0,        &(ser_test_params->help),              NULL);
    parse_table_add(&parse_table,  "TestDynamic",   PQ_INT,    0,        &(ser_test_params->test_dynamic),      NULL);
    parse_table_add(&parse_table,  "XOR",           PQ_INT,    0,        &(ser_test_params->xor_test),          NULL);

#ifdef BCM_DNX_SUPPORT
    parse_table_add(&parse_table,  "mdb",           PQ_INT8,    0,        &(ser_test_params->mdb),              NULL);
#endif
    ser_test_params->unit = unit;

    /** print usage and exit **/ 
    if (ser_test_params->help) 
    {
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }

    /** parsing arguments and checking seting needed values, checking validity of given options **/
    if (parse_arg_eq(a, &parse_table) < 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: Invalid option: %s\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }

    /** making sure no extra options were given **/ 
    if (ARG_CNT(a) != 0) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s: extra options starting with \"%s\"\n"), ARG_CMD(a), ARG_CUR(a)));
        /* print usage */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"%s"), tr153_test_usage));
        goto done;
    }

    if (ser_test_params->xor_test)
    {
        if (parse_memory_name(unit, &(ser_test_params->mem), ser_test_params->mem_name_parse, NULL, NULL) < 0) 
        {
            ser_test_params->mem = INVALIDm;
        }
    }
    else if (!ser_test_params->run_all) {
#ifdef BCM_DNX_SUPPORT
        if (ser_test_params->mdb)
        {
            if (parse_memory_name(unit, &(ser_test_params->mem), ser_test_params->mem_name_parse, NULL, NULL) < 0) 
            {
                ser_test_params->mem = INVALIDm;
            }
            else
            {
                ser_test_params->index = parse_memory_index(unit, ser_test_params->mem, ser_test_params->index_parse);
                ser_test_params->array_index = parse_memory_array_index(unit, ser_test_params->mem, ser_test_params->array_index_parse);
            }
            rv = BCM_E_NONE;
            goto done;
        }
#endif
        /** validating arguments values **/
        /* validate memory name and store mem enumerator in ser_test_params->mem */
        if (parse_memory_name(unit, &(ser_test_params->mem), ser_test_params->mem_name_parse, NULL, NULL) < 0) 
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" is invalid\n"), ser_test_params->mem_name_parse));
            goto done;
        }
        if (!SOC_MEM_FIELD_VALID(unit,ser_test_params->mem,ECCf) &&
            !SOC_MEM_FIELD_VALID(unit,ser_test_params->mem,PARITYf)) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" must contain PARITY or ECC fields\n"), ser_test_params->mem_name_parse));
            goto done;
        }

        if (soc_mem_is_readonly(unit, ser_test_params->mem)) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" is readonly register\n"), ser_test_params->mem_name_parse));
            goto done;
        }
        /* validate memory index and store index in ser_test_params->index */
        ser_test_params->index = parse_memory_index(unit, ser_test_params->mem, ser_test_params->index_parse);
        if ( (ser_test_params->index < parse_memory_index(unit, ser_test_params->mem, "min")) || 
             (ser_test_params->index > parse_memory_index(unit, ser_test_params->mem, "max"))    ) 
        {

            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"index %d is invalid for memory \"%s\"\n"), ser_test_params->index, ser_test_params->mem_name_parse));
            goto done;
        }

        /* validate memory array index and store array index in ser_test_params->array_index */
        ser_test_params->array_index = parse_memory_array_index(unit, ser_test_params->mem, ser_test_params->array_index_parse);
        if ( (ser_test_params->array_index < parse_memory_array_index(unit, ser_test_params->mem, "min")) || 
             (ser_test_params->array_index > parse_memory_array_index(unit, ser_test_params->mem, "max"))    ) 
        {

            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"array index %d is invalid for memory \"%s\"\n"), ser_test_params->index, ser_test_params->mem_name_parse));
            goto done;
        }

        /* validate memory copy number */
        if (ser_test_params->copyno != -1)
        {
            if (!SOC_MEM_BLOCK_VALID(unit, ser_test_params->mem, ser_test_params->copyno + SOC_MEM_BLOCK_MIN(unit, ser_test_params->mem))) 
            {
                LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Copy Number %d is invalid for memory \"%s\"\n"), ser_test_params->copyno, ser_test_params->mem_name_parse));
                goto done;
            }
        }

    } else {
        if (sal_strlen(start_from)) {
            if (parse_memory_name(unit, &(ser_test_params->start_from), start_from, NULL, NULL) < 0) 
            {
                LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"Memory \"%s\" is invalid\n"), start_from));
                goto done;
            }
        } else{
            ser_test_params->start_from=0;
        }
    }
    /* validate error type is acceptable */
    if ((ser_test_params->error != ECC1) && (ser_test_params->error != ECC2)) 
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"error type number %d is invalid\n"), ser_test_params->error ));
        goto done;
    }
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit,"choosing Interrupt number %d \n"), ser_test_params->interrupt ));
    /** Turn off other Threads **/
    bcm_common_linkscan_enable_set(unit,0);
    soc_counter_stop(unit);
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        rv = bcm_dpp_counter_bg_enable_set(unit, FALSE);
        if (BCM_E_NONE == rv)
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"counter processor background accesses suspended\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"counter processor background access suspend failed: %d (%s)\n"),rv, _SHR_ERRMSG(rv)));
        }
    }
#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO_PLUS_ONLY(unit))
    {
        jer_mgmt_pvt_monitor_correction_disable(unit);
    }
#endif /* BCM_JERICHO_SUPPORT */
#endif /* BCM_PETRA_SUPPORT */

    rv = BCM_E_NONE;

done:
    parse_arg_eq_done(&parse_table);
    if (rv != BCM_E_NONE) 
    {
        test_error(unit, "There was a problem with parameters, they were not entered correctly\n");
    }
    return rv; 
}

static  soc_field_t fields[] = {ECCf,PARITYf,NUM_SOC_FIELD};


static int mem_is_ff_or_regarray(int unit, soc_mem_t mem)
{

    /*  flipflops and regarrays doesnt generate ser interrupts*/

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        switch (mem) {
            case IRE_NIF_PORT_TO_CTXT_BIT_MAPm:
            case IRR_SNOOP_MIRROR_TABLE_1m:
            case IRR_SNOOP_MIRROR_TABLE_0m:
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on FF Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                return 1;
        }
    }

    return 0;
}

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
STATIC
arad_sch_ser_map_t  *get_sch_ser_map_entry(soc_mem_t mem,uint32 nof_errors)
{

    int i;

    for (i=0;arad_sch_ser_map[i].mem!=INVALIDm;i++) {
        if (arad_sch_ser_map[i].mem == mem &&
            arad_sch_ser_map[i].nof_errors==nof_errors) {
            return &(arad_sch_ser_map[i]);
        }
    }
    return NULL;
}
/* } */
#endif

/*
 * Function:
 *      memory_ser_test_run
 * Purpose:
 *      run the test, ser test should simulate ser error and check its handling
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 *          BCM_E_PARAM: unjustified skipped.
 *          BCM_E_TIMEOUT: get bits to change in Memory timed-out.
 *          BCM_E_INTERNAL: Other BCM/SOC API failure.
 *          BCM_E_FAIL/BCM_E_UNAVAIL:  SER test failure of SER test was not avaliable.
 */
STATIC
int memory_ser_test_run_mem(int unit, args_t *a, void *p)
{
    uint32 mem_buff[SOC_MAX_MEM_WORDS], mask[SOC_MAX_MEM_WORDS], mem_buff_restored_from_cache[SOC_MAX_MEM_WORDS],orig_mem_buff[SOC_MAX_MEM_WORDS];
    uint32 interrupt_count, new_interrupt_count, sleep_for_a_bit = 2500000;
    int rv = BCM_E_NONE, test_failed = 0, entry_length;
    int nof_needed_bits, nof_bits_found = 0, random_bit, attampt_counter, toggle_counter, block_num=0;
    int bits_found[] = {-1, -1};
    ser_test_params_t *ser_test_params = NULL;
    soc_mem_t mem = 0;
    bcm_switch_event_control_t get_interrupt_count;
    uint8 succeed=0;
    uint32 i;
    soc_block_t block ;
    soc_reg_t   gmo_reg ;
    bcm_switch_event_control_t type;
    uint32 int_mask;
    uint32 copyno=0;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
    arad_sch_ser_map_t  *sch_ser_map_entry = NULL;
/* } */
#endif
    sand_block_control_info_t*  sand_bloc_info;
#ifdef BCM_DNX_SUPPORT
    uint32 eps_protect_flag[2] = {0, 0};
    soc_reg_above_64_val_t eps_shaper_set[2];
#endif

    SOC_INIT_FUNC_DEFS;

    /** Check Validity of Test Parameters **/ 
    ser_test_params = ser_parameters[unit];
    if (ser_test_params == NULL) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Invalid test params\n")));
        test_failed = 1;
        rv = BCM_E_UNAVAIL;
        goto done;
    }

    mem = ser_test_params->mem;
    /** Print Test Parameters - Temp chunk, need to be removed once test is completed **/
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Recieved Arguments are:\n" 
                                                  "Memory name : %s\t"             "Memory num  : %d\t"          "Index       : %d\t"        "Array Index : %d\n"
                                                  "CopyNo      : %d\t"             "Cache State : %d\t"          "Interrupt   : %d\t"        "Error Type  : %d\n"),
                                         SOC_MEM_NAME(unit, ser_test_params->mem), ser_test_params->mem,         ser_test_params->index,     ser_test_params->array_index,
                                                  ser_test_params->copyno,         ser_test_params->cache_state, ser_test_params->interrupt, ser_test_params->error));


    nof_needed_bits = ser_test_params->error == ECC2 && SOC_MEM_FIELD_VALID(unit,mem,ECCf) ? 2 : 1;
    block = sand_blktype_to_index(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)));
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
    if ((SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) == SOC_BLK_SCH) && SOC_IS_ARADPLUS_AND_BELOW(unit)) {

        sch_ser_map_entry = get_sch_ser_map_entry(mem,nof_needed_bits);
        if (sch_ser_map_entry == NULL) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s not connect to an interrupt\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_UNAVAIL;
            goto done;
        }
        type.event_id = sch_ser_map_entry->interrupt; 
        get_interrupt_count.event_id =type.event_id;
    } else
/* } */
#endif
    {
        type.event_id = ser_test_params->interrupt; 
        get_interrupt_count.event_id = ser_test_params->interrupt;
    }
    type.action=bcmSwitchEventMask;
    type.index=0;
    bcm_switch_event_control_get(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,type,&int_mask);
    if (int_mask) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Interrupt %d Masked sow skipping the test for Mem:%s\n"), ser_test_params->interrupt,SOC_MEM_NAME(unit, ser_test_params->mem))); 
        test_failed = 1;
        rv = BCM_E_UNAVAIL;
        goto done;
    }

    /** Get Parameters and Preperations **/
    sal_memset(mask, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(mem_buff, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    sal_memset(mem_buff_restored_from_cache, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    copyno = ser_test_params->copyno;

    sand_bloc_info = sand_get_block_control_info(unit);
    gmo_reg = sand_bloc_info[block].gmo_reg;

    if (!SOC_REG_IS_VALID(unit,gmo_reg) || SOC_REG_ARRAY(unit,gmo_reg) || !SOC_REG_FIELD_VALID(unit,gmo_reg,CPU_BYPASS_ECC_PARf)) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s block(%d) doesnt have general configuration register\n"), SOC_MEM_NAME(unit, mem), block));

        if (!SOC_REG_IS_VALID(unit,gmo_reg))
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "gmo_reg is not valid: %d\n"), gmo_reg));
        }
        else if (SOC_REG_ARRAY(unit,gmo_reg))
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "gmo_reg is array\n")));
        }
        else
        {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "gmo_reg has no field CPU_BYPASS_ECC_PARf\n")));
        }

        test_failed = 1;
        rv = BCM_E_UNAVAIL;
        goto done;
    }
    block_num = ser_test_get_block_num_unsafe(unit, mem, ser_test_params->copyno);
    get_interrupt_count.index = ser_test_params->copyno;

#ifdef BCM_DNX_SUPPORT
    _SOC_IF_ERR_EXIT(dnx_mem_read_eps_protect_start(unit, mem, block_num, eps_protect_flag, eps_shaper_set));
#endif

    /* start !sch*/
    if (!((SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) == SOC_BLK_SCH) && SOC_IS_ARADPLUS_AND_BELOW(unit))) {
        sal_srand(sal_time());

        /** Check Memory is not W/O or R/O **/
        if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Write Only memory - Invalid memory\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_UNAVAIL;
            goto done;
        }

        if (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Read Only memory - Invalid memory\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_UNAVAIL;
            goto done;
        }

        /** get given interrupt's count **/
        get_interrupt_count.action = bcmSwitchEventStat;

        /** Get Bit/s to Toggle **/
        entry_length = soc_mem_entry_bits(unit, mem);
        if (entry_length == 0) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s entry size is 0, invalid size\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_FAIL;
            goto done;
        }
        soc_mem_datamask_rw_get(unit, mem, mask); 
        /* get random bit, check if not r/o or w/o, and save it, repeat untill got all needed bits, or timed out. */
        for ( attampt_counter = 0; (attampt_counter < MAX_NOF_ATTAMPTS_TO_GET_A_RW_BIT) && (nof_needed_bits > nof_bits_found); ++attampt_counter) {
            random_bit = sal_rand() % entry_length ;
            /* check if bit is r/w */
            if ( mask[random_bit / BIT_IN_UINT32] & (1u << random_bit % BIT_IN_UINT32) ) {
                /* Save found Bit */
                bits_found[nof_bits_found++] = random_bit;
                /* mark bit as not r/w - inorder not to get the same one if more then 1 is needed */
                mask[random_bit / BIT_IN_UINT32] ^= (1u << random_bit % BIT_IN_UINT32);
            } else {
                LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "bit %d/%d of Memory %s is not r/w bit and is being skipped, skip count = %d\n"), 
                                                       random_bit, entry_length - 1, SOC_MEM_NAME(unit, mem), attampt_counter ));
            }
        }

        /** check if loop timed-out **/
        if (nof_needed_bits != nof_bits_found) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "attampt to get bits to change in Memory %s timed-out\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_TIMEOUT;
            goto done;
        }


        /** Change Entry not in cache **/
        /* Read entry */
        bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &interrupt_count);

        _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem,
                                               ser_test_params->array_index,
                                               block_num, ser_test_params->index,
                                               mem_buff));

        /* change requiered bits */
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "change bits at pos:")));
        sal_memcpy(orig_mem_buff,mem_buff,sizeof(orig_mem_buff));
        for (toggle_counter = 0; toggle_counter < nof_bits_found; ++toggle_counter) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "(%d) "), bits_found[toggle_counter]));
            mem_buff[bits_found[toggle_counter]/BIT_IN_UINT32] ^= (1u << bits_found[toggle_counter]%BIT_IN_UINT32);
        }
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "\n")));

        /** Bypass PARITY and ECC **/
        _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, copyno, CPU_BYPASS_ECC_PARf, 0x1));

        /** Write modified entry **/ 
        _SOC_IF_ERR_EXIT(soc_mem_array_write_extended(unit, SOC_MEM_DONT_USE_CACHE, mem, ser_test_params->array_index, block_num, ser_test_params->index, mem_buff));

        /** Re-activate PARITY and ECC and Read memory --> Should Trigger the Interrupt **/
        _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, gmo_reg, copyno, CPU_BYPASS_ECC_PARf, 0x0)); 
        _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem,
                                               ser_test_params->array_index,
                                               block_num, ser_test_params->index,
                                               mem_buff_restored_from_cache));
         sal_usleep(sleep_for_a_bit);
         /* end !sch*/
    }
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
    else {
        /**
         *  sequence by ashraf  
         *  ser sequence for creating ser in sch memory
         *  BCM.0>  s SCH_ERROR_INITIATION_DATA 1
         *  BCM.0>  m SCH_PAR_ERR_INITIATE FDMS_PAR_ERROR_INIT=0
         *  BCM.0>  m SCH_PAR_ERR_INITIATE FDMS_PAR_ERROR_INIT=1
         *  BCM.0> d disable_cache SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMS 0 1
 */
        sch_ser_map_entry = get_sch_ser_map_entry(mem,nof_needed_bits);
        if (sch_ser_map_entry == NULL) {
            LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s not connect to an interrupt\n"), SOC_MEM_NAME(unit, mem)));
            test_failed = 1;
            rv = BCM_E_UNAVAIL;
            goto done;
        }

        get_interrupt_count.action = bcmSwitchEventStat;
        bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &interrupt_count);

        _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem,
                                               ser_test_params->array_index,
                                               block_num, ser_test_params->index,
                                               mem_buff));

        _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, SCH_ERROR_INITIATION_DATAr, REG_PORT_ANY,
                                                 ERR_WRf, 0x1));

        _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, sch_ser_map_entry->error_initiate_reg, REG_PORT_ANY,
                                                 sch_ser_map_entry->error_initiate_field, 0x0));

        _SOC_IF_ERR_EXIT(soc_reg_field32_modify(unit, sch_ser_map_entry->error_initiate_reg, REG_PORT_ANY,
                                                 sch_ser_map_entry->error_initiate_field, 0x1));
        _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem,
                                               ser_test_params->array_index,
                                               block_num, ser_test_params->index,
                                               mem_buff_restored_from_cache));
    }
/* } */
#endif

    /** Check again given interrupt's count **/
    bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &new_interrupt_count);
    succeed=0;
    for (i=0;i<MAX_SER_RETRIES;i++) {
        if (new_interrupt_count <= interrupt_count) {
            /* if the counter test failed lets try read test maybe the nmemory already fixed*/
                sal_usleep(sleep_for_a_bit);
                cli_out("retry reading counter  %d\n",i);
                bcm_switch_event_control_get(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, get_interrupt_count, &new_interrupt_count);
                continue;
        }
        succeed=1;
        break;

        }    

    if (!succeed) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Memory %s Interrupt doesnt trigger(counter=%d) check to see if the interrupt number %d supplied  correct\n"), SOC_MEM_NAME(unit, mem), new_interrupt_count, ser_test_params->interrupt));
        test_failed = 1;
        rv = BCM_E_FAIL;
        goto done;
    }
    /**
     * we not perform comparisson in the following situation 
     * because we cant fix the memory 
     */
    if ( 
        soc_mem_is_writeonly(unit, mem) ||
        sand_tbl_is_dynamic(unit,mem) ||
        (SOC_MEM_FIELD_VALID(unit,mem,ECCf) && soc_mem_field_length(unit,mem,ECCf)<=1)||
        (SOC_MEM_FIELD_VALID(unit,mem,PARITYf) && soc_mem_field_length(unit,mem,PARITYf)!=1)||
        (!soc_mem_cache_get(unit,mem,SOC_BLOCK_ALL) && (!SOC_MEM_FIELD_VALID(unit,mem,ECCf) || 
        (SOC_MEM_ECC_TYPE(unit,mem) != SOC_MEM_ECC_TYPE_NORMAL && SOC_MEM_ECC_TYPE(unit,mem) != SOC_MEM_ECC_TYPE_WIDE) ||
                                                        SOC_MEM_TYPE(unit,mem) == SOC_MEM_TYPE_XOR)) ||
        !(ser_test_params->error == ECC1  && 
        (SOC_MEM_FIELD_VALID(unit, mem, ECCf)  || 
         (SOC_MEM_FIELD_VALID(unit, mem, PARITYf) && soc_mem_cache_get(unit, mem, copyno))))) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip Comparing Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        goto done;
    }


    /*  we expected from ecc protected memories  or  cached parity memories
     *   to be fix by the interrupt handler
     */
        /* in case of ecc1 test if our correction by shadow or by ecc1 correction succeed. we compare the corrected data with the original data*/
    _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem,
                                           ser_test_params->array_index,
                                           block_num, ser_test_params->index,
                                           mem_buff_restored_from_cache));
    if (sal_memcmp(mem_buff_restored_from_cache, orig_mem_buff, sizeof(orig_mem_buff))) {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Warning:Memory %s value doesnt corrected to it original value \n"), SOC_MEM_NAME(unit, mem)));
        test_failed = 1;
        rv = BCM_E_FAIL;
    }

done:

    if (test_failed) {
        test_error(unit, "Memory Ser Test Failed!\n");
    }
#ifdef BCM_DNX_SUPPORT
    _SOC_IF_ERR_CONT(dnx_mem_read_eps_protect_end(unit, mem, block_num, eps_protect_flag, eps_shaper_set));
#endif
    return rv;

exit:
#ifdef BCM_DNX_SUPPORT
    _SOC_IF_ERR_CONT(dnx_mem_read_eps_protect_end(unit, mem, block_num, eps_protect_flag, eps_shaper_set));
#endif
    test_error(unit, "Memory Ser Test Failed!\n");
    _rv = BCM_E_INTERNAL;
    SOC_FUNC_RETURN;
}


STATIC
int mem_ser_test_cb(int unit, soc_mem_t mem,void *p)
{
    bcm_error_t rv = BCM_E_NONE;
    ser_test_params_t *ser_test_params = ser_parameters[unit];
    uint32 min_index,max_index;
    uint64 index_addition;
    uint64 sal_rand_max = COMPILER_64_INIT(0, SAL_RAND_MAX);
    int instance;
    uint16 dev_id;
    uint8 rev_id;
 
    
    /* we skip on memory if it not in reuiered range*/
    if (mem < ser_test_params->start_from ||
        mem > ser_test_params->start_from  + ser_test_params->count) 
    {
        ser_test_params->skipped_counter++;       
        return rv;
    }

    /* we skip on memory if it doesnt contain ECC  and is not cacheble*/
    if ( 
        !sand_mem_contain_one_of_the_fields(unit,mem,fields) ||
        mem_is_ff_or_regarray(unit,mem) ||
        soc_mem_is_readonly(unit, mem) ||
        sal_strstr(SOC_MEM_NAME(unit,mem), "BRDC_") != NULL  ||
        SOC_MEM_IS_ALIAS(unit, mem) ||
        SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))==SOC_BLK_IPROC ||
        SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))==SOC_BLK_CMIC ||
        SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem))==SOC_BLK_FF  ) {
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        ser_test_params->skipped_counter++;       
        return rv;
    }

    if (SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) == SOC_BLK_IPSEC) {
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if(dev_id == BCM88271_DEVICE_ID || dev_id == BCM88272_DEVICE_ID || dev_id == BCM88273_DEVICE_ID
           || dev_id == BCM88276_DEVICE_ID || dev_id == BCM88278_DEVICE_ID) {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->skipped_counter++;
            return rv;
        }
    }

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        switch (mem) {
        /** 
         *  block at arad_ser_init
         *  due to false positive interrupts
         */ 
        case IRR_SNOOP_MIRROR_DEST_TABLEm: 
        case IRE_CTXT_MEM_CONTROLm:
        case IRE_NIF_PORT_TO_CTXT_BIT_MAPm:
        case IRR_SNOOP_MIRROR_TABLE_0m:
        case IRR_SNOOP_MIRROR_TABLE_1m:
        case NBI_RLENG_MEMm:
        case NBI_TLENG_MEMm:
        case NBI_RTYPE_MEMm:
        case NBI_TTYPE_MEMm:
        case NBI_RPKTS_MEMm:
        case NBI_TPKTS_MEMm:
 /**
  * IPT - EGQDATA has ECC only on the upper 24 MSB's. make sure 
  * you generate the error in one of those bits : 511:488 
  * Yael Konforty  
  */
        case IPT_EGQDATAm:

      /** 
       * the IPS_QDESC is not accesible by cpu 
       * while the  IPS_QDESC_TABLE divided into 2 parts 
       * one protected by parity and two by ecc 
       * Yael Konforty  
 */ 
        case IPS_QDESCm:
        case IPS_QDESC_TABLEm:

            /* 
             * the following are alias with ECC instead of parity 
             * should be removed from here after fix at SDK-89304 
             */
        case PPDB_B_LARGE_EM_FID_COUNTER_DBm:
        case PPDB_B_LARGE_EM_FID_PROFILE_DBm:
        case PPDB_B_LARGE_EM_PORT_MINE_TABLE_LAG_PORTm:
        case PPDB_B_LARGE_EM_PORT_MINE_TABLE_PHYSICAL_PORTm:

            /*
             * Regarding mrps,
             * MRPS_MCDA_PRFCFG_SHARING_DIS
             *MRPS_MCDA_PRFCFG_SHARING_EN
             * Are not memories. They are descriptions of 2    
             *possibilities for configuring McdaPrfcfg memory.              
             *Inbal
             */
        case MRPS_MCDA_PRFCFG_SHARING_DISm:
        case MRPS_MCDA_PRFCFG_SHARING_ENm:



            /**
             * cant have ser unless traffic(ariels)
             */
        case IQM_PDMm:
        case IQM_BDBLLm:
        case IQM_PQDMDm:
            /**
             * dont have initiate doesnt work(ariels)
             */
        case IDR_MCDA_PRFCFG_0m:
        case IDR_MCDB_PRFCFG_0m:

            /**
             * dont have cpu write support (ariels)
             */
        case IQM_VSQ_D_MX_OCm:
        case IQM_VSQ_F_MX_OCm:



            /**
             * recommended by ashraf to skip this memories 
             *  SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CAL
             *  divided into regions each region can get different interrupt
             *  
             *  SCH_SCHEDULER_ENABLE_MEMORY_SEM
             *  represent 2 memories
             *  you cant expect which region get the intterupt
 */
        case SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm:
        case SCH_SCHEDULER_ENABLE_MEMORY_SEMm:




            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->skipped_counter++;       
            return rv;
        }
/*     unjustified skip list*/
        switch (mem) {


            /**
             * the list of the below crps mems produce 
             * too many ints 
             * This is expected behavior for CRPS as well. (Inbal)
             */

        case CRPS_CRPS_0_OVTH_MEMm:
                /**
                 * for sered EGQ_QP_CBM or EGQ_TCG_CBM 
                 * we must set EGQ_EGRESS_SHAPER_ENABLE_SETTING to 0 else the 
                 * desing will block the access for this memories
                 */
            case EGQ_QP_CBMm:
            case EGQ_TCG_CBMm:

                /**
                 * for sered IPS_QSZ 
                 * we must set  IPS_CREDIT_WATCHDOG_CONFIGURATION to 0 
                 * else the desing will read infinitly from IPS_QSZ 
                 * and will generate infinitly interrupts 
                 */
            case IPS_QSZm:

                /* make problem in tr 153 RunAll=1,but fix correct at tr 153 small run*/
            case PPDB_A_OEMB_STEP_TABLEm:


            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->unjustified_skipped_counter++;       
            return rv;
        }




    } else if (SOC_IS_JERICHO(unit)) {

       /*  jericho and above*/

        /*     unjustified skip list*/
        switch (mem) {
            /** 
             *  list of unjustified skipped memories
             */ 



                /**
                 * the list of the below crps mems produce 
                 * too many ints 
                 * This is expected behavior for CRPS as well. (Inbal)
                 */
            case CRPS_CRPS_13_CNTS_MEMm:
            case CRPS_CRPS_13_OVTH_MEMm:
            case CRPS_CRPS_14_CNTS_MEMm:
            case CRPS_CRPS_14_OVTH_MEMm:
            case CRPS_CRPS_15_CNTS_MEMm:
            case CRPS_CRPS_15_OVTH_MEMm:
            case CRPS_CRPS_16_CNTS_MEMm:
            case CRPS_CRPS_16_OVTH_MEMm:
            case CRPS_CRPS_17_CNTS_MEMm:
            case CRPS_CRPS_17_OVTH_MEMm:
            case CRPS_CRPS_1_CNTS_MEMm:
            case CRPS_CRPS_1_OVTH_MEMm:
            case CRPS_CRPS_2_CNTS_MEMm:
            case CRPS_CRPS_2_OVTH_MEMm:
            case CRPS_CRPS_3_CNTS_MEMm:
            case CRPS_CRPS_3_OVTH_MEMm:
            case CRPS_CRPS_4_CNTS_MEMm:
            case CRPS_CRPS_4_OVTH_MEMm:
            case CRPS_CRPS_5_CNTS_MEMm:
            case CRPS_CRPS_5_OVTH_MEMm:
            case CRPS_CRPS_6_CNTS_MEMm:
            case CRPS_CRPS_6_OVTH_MEMm:
            case CRPS_CRPS_7_CNTS_MEMm:
            case CRPS_CRPS_7_OVTH_MEMm:
            case CRPS_CRPS_8_CNTS_MEMm:
            case CRPS_CRPS_8_OVTH_MEMm:
            case CRPS_CRPS_9_CNTS_MEMm:
            case CRPS_CRPS_9_OVTH_MEMm:
            case CRPS_CRPS_10_OVTH_MEMm:
            case CRPS_CRPS_11_CNTS_MEMm:
            case CRPS_CRPS_11_OVTH_MEMm:
            case CRPS_CRPS_12_CNTS_MEMm:
            case CRPS_CRPS_12_OVTH_MEMm:
            case CRPS_CRPS_0_CNTS_MEMm:
            case CRPS_CRPS_0_OVTH_MEMm:
            case  CRPS_CRPS_10_CNTS_MEMm:


                /**
                 * All these memories behave the same as Arad.
                 * Same reason for not seeing 1b SER reaction.
                 * Ariels
                 */
            case IQM_IQM_READ_PENDING_FIFOm:
            case IQM_PAKCET_DESCRIPTOR_MEMORY_ECC_DYNAMICm:
            case IQMT_BDBLLm:
            case IQMT_PDM_0m:
            case IQMT_PDM_1m:
            case IQMT_PDM_2m:
            case IQMT_PDM_3m:
            case IQMT_PDM_4m:
            case IQMT_PDM_5m:
            case IQM_VSQD_MX_OCm:
            case IQM_VSQE_MX_OCm:
            case IQM_VSQF_MX_OCm:


                /**
                 * this mem produce 
                 * too many ints 
                 * For RTP this is expected  then 
                 * Uri 
                 */
            case RTP_UNICAST_DISTRIBUTION_MEMORYm:

            /*   the ser for this block disabled at init see soc_dcmn_ser_init_cb*/
            case ECI_MBU_MEMm:

                /**
                 * 1b/2b ECC Ignore 
                 */
            case OCB_OCB_ADDRESS_SPACEm:

            /*SKIP QAX Dynamic memories, these momeories are written failure occasionally*/
            case IEP_MCDB_DYNAMICm:
            case IEP_MCDA_DYNAMICm:
            case IMP_MCDB_DYNAMICm:
            case IMP_MCDA_DYNAMICm:

            /*SKIP JER Dynamic memories, these momeories are written failure occasionally*/
            case SCH_CIR_SHAPERS_DYNAMIC_TABEL_CSDTm:
            case SCH_PIR_SHAPERS_DYNAMIC_TABEL_PSDTm:
            case MRPS_MCDA_DYNAMICm:
            case MRPS_MCDB_DYNAMICm:
            case MTRPS_EM_MCDA_DYNAMICm:
            case MTRPS_EM_MCDB_DYNAMICm:

            
            case EDB_EEDB_BANKm:
            case EDB_EEDB_TOP_BANKm:

            /*QAX SKIP memories*/
            case OAMP_FLEX_VER_MASK_TEMPm:
            case CRPS_CRPS_CNTS_MEMm:
            case CRPS_CRPS_OVTH_MEMm:
            case CRPS_CRPS_PRE_READ_MEMm:
            case ECI_ZIKARON_MBUm:
            case EDB_OUTRIF_TABLEm:
            case EDB_RIF_PROFILE_TABLEm:

            /* OAMP_FLOW_STAT_ACCUM_ENTRY_4 is alias for OAMP_SAT_RX_FLOW_STATS, should not contain ECC field  */
            case OAMP_FLOW_STAT_ACCUM_ENTRY_4m:
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                ser_test_params->unjustified_skipped_counter++;       
                return rv;
        }
    }

    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
       /*  jericho plus only*/

        /*     unjustified skip list*/
        switch (mem) {
            /**
             *  list of unjustified skipped memories
             */
            case IHB_FEC_PATH_SELECTm:
            case IHP_VTT_PATH_SELECTm:
            case IHB_FIFO_PIPE_40_TO_50m:
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                ser_test_params->unjustified_skipped_counter++;
                return rv;
        }
    }

    if (SOC_IS_QUX(unit)) {
       /*  Qux */

        switch (mem) {

            
            case OAMP_RMEP_DB_EXTm:
                /* if cache is enabled, SER monitoring is enabled on  OAMP_RMEP_DB_EXT  */
                if (soc_mem_cache_get(unit, OAMP_RMEP_DB_EXTm, MEM_BLOCK_ALL)) {
                    break;
                }
            case IPSEC_SPU_WRAPPER_TOP_SPU_INPUT_FIFO_MEM_Hm:
            case IPSEC_SPU_WRAPPER_TOP_SPU_OUTPUT_FIFO_MEM_Hm:
            case IPSEC_TUNNEL_DATAm:

                 LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                ser_test_params->unjustified_skipped_counter++;
                return rv;
        }
    }

    if (SOC_IS_DNX(unit))
    {
        /* Jericho2 */
        switch (mem)
        {
            
            case EPS_HEADR_CMPNS_CAL_HP_CBMm:
            case EPS_HEADR_CMPNS_CAL_LP_CBMm:
            case EPS_OTM_HIGH_CAL_CRDT_BALANCEm:
            case EPS_OTM_LOW_CAL_CRDT_BALANCEm:
            
            case RTP_UNICAST_DISTRIBUTION_MEMORYm:
            /* need hardware reset device, skip it. */
            case SCH_TOKEN_MEMORY_CONTROLLER_TMCm:
            /* those two memories unaccessable */
            case MDB_ARM_MEM_0m:
            case MDB_ARM_MEM_10000m:
            case MDB_ARM_MEM_30000m:
            {
                LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                ser_test_params->unjustified_skipped_counter++;
                return rv;
            }
            case MDB_EEDB_ENTRY_BANKm:
            {
                if (SOC_IS_JERICHO2_B(unit))
                {
                    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
                    ser_test_params->unjustified_skipped_counter++;
                    return rv;
                }
            }
        }
    }

    if (SOC_IS_Q2A(unit))
    {
        soc_block_type_t block_type = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem));
        if ((block_type == SOC_BLK_FSAR) || (block_type == SOC_BLK_FSCL) || (block_type == SOC_BLK_FASIC))
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->unjustified_skipped_counter++;
            return rv;
        }

        if (mem == IPS_MEM_2E8000m)
        {
            /*
             * Write access is forbidden for this memory 
             */
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip unjustified on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->unjustified_skipped_counter++;
            return rv;
        }        
    }

    if (!ser_test_params->test_dynamic)
    {
        if (soc_mem_is_signal(unit, mem))
        {
            LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Skip dynamic memory Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
            ser_test_params->unjustified_skipped_counter++;
            return rv;
        }
    }

    ser_test_params->total_counter++;
    ser_test_params->mem = mem;
    min_index =  parse_memory_index(unit, mem, "min");
    max_index =  parse_memory_index(unit, mem, "max");
    COMPILER_64_SET(index_addition, 0, max_index - min_index);
    COMPILER_64_UMUL_32(index_addition, sal_rand());
    COMPILER_64_UDIV_64(index_addition, sal_rand_max);
    ser_test_params->index = min_index + COMPILER_64_LO(index_addition);

    min_index = parse_memory_array_index(unit, mem, "min");
    max_index = parse_memory_array_index(unit, mem, "max");
    COMPILER_64_SET(index_addition, 0, max_index - min_index);
    COMPILER_64_UMUL_32(index_addition, sal_rand());
    COMPILER_64_UDIV_64(index_addition, sal_rand_max);
    ser_test_params->array_index = min_index + COMPILER_64_LO(index_addition);

    update_mem_int(unit);

    ser_test_params->copyno = 0;
    SOC_MEM_BLOCK_ITER(unit,mem,instance){

        rv = memory_ser_test_run_mem(unit,NULL,NULL);
        if (rv != BCM_E_NONE) {
            break;
        }
        ser_test_params->copyno++; 
    }
     

    switch (rv) {
    case BCM_E_PARAM:
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"second skip list(param) on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        ser_test_params->param_counter++;  
        break;
    case BCM_E_TIMEOUT:
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"second skip list(timeout) on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        ser_test_params->timeout_counter++;
        ser_test_params->error_mem_list[ser_test_params->error_counter++] = mem;
        break;

    case BCM_E_UNAVAIL:
    case BCM_E_FAIL:
    case BCM_E_INTERNAL:
        LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit,"Found Error  on Mem:%s\n"),SOC_MEM_NAME(unit, mem)));
        ser_test_params->error_mem_list[ser_test_params->error_counter] = mem;
        ser_test_params->error_counter++;
    default: 
        break;
    }
    return rv; 

}

STATIC int xor_mem_ser_entry_fill_check(int unit, soc_mem_t mem)
{
    int     rv;
    int     index_min, index_max, array_min, array_max;
    int     index, array_index, entry_index;
    int     copyno;
    uint32  entry_dw = soc_mem_entry_words(unit, mem);
    
    uint32  write_value[SOC_MAX_MEM_WORDS];
    uint32  read_value[SOC_MAX_MEM_WORDS];
    uint32  mem_field_mask[SOC_MAX_MEM_WORDS];
    ser_test_params_t *ser_test_params = ser_parameters[unit];

    SOC_INIT_FUNC_DEFS;

    if (SOC_IS_DPP(unit))
    {
        if (mem == IRR_MCDBm)
        {
            SOC_EXIT;
        }
    }

    index_min =  parse_memory_index(unit, mem, "min");
    index_max =  parse_memory_index(unit, mem, "max");
    array_min = parse_memory_array_index(unit, mem, "min");
    array_max = parse_memory_array_index(unit, mem, "max");

    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "XOR memory fill&SER&Check: mem=%s"), SOC_MEM_NAME(unit, mem)));

    /* get xor memory ser test mask */
    sal_memset(mem_field_mask, 0, sizeof(mem_field_mask));
    soc_mem_datamask_rw_get(unit, mem, mem_field_mask); 
    
    /* write xor memory */
    for (array_index = array_min; array_index <= array_max; ++array_index)
    {        
        SOC_MEM_BLOCK_ITER(unit, mem, copyno)
        {
            for (index = index_min; index <= index_max; index++)
            {
                /* generate write memory value */
                sal_memset(write_value, 0, sizeof(write_value));
                for (entry_index = 0; entry_index < entry_dw; entry_index++)
                {
                    write_value[entry_index] = mem + array_index + index;
                    write_value[entry_index] = write_value[entry_index] & mem_field_mask[entry_index];
                }
                _SOC_IF_ERR_EXIT(soc_mem_array_write(unit, mem, array_index, copyno, index, write_value));
            }
        }
    }        

    /* generate 1bits or 2bits error at xor memory entry */
    ser_test_params->mem = mem;
    update_mem_int(unit);
    ser_test_params->copyno = 0;
    SOC_MEM_BLOCK_ITER(unit, mem, copyno)
    {
        if (index_max == index_min)
        {
            ser_test_params->index = index_min;
        }
        else
        {
            ser_test_params->index = index_min + (sal_rand() % (index_max - index_min));
        }
        if (array_max == array_min)
        {
            ser_test_params->array_index = array_min;
        }
        else
        {
            ser_test_params->array_index = array_min + (sal_rand() % (array_max - array_min));
        }
        _SOC_IF_ERR_EXIT(memory_ser_test_run_mem(unit, NULL, NULL));
        ser_test_params->copyno++; 
    }

    /* check xor memory entry */
    for (array_index = array_min; array_index <= array_max; ++array_index)
    {
        rv = BCM_E_NONE;
        SOC_MEM_BLOCK_ITER(unit, mem, copyno)
        {
            for (index = index_min; index <= index_max; index++)
            {
                /* generate write memory value */
                for (entry_index = 0; entry_index < entry_dw; entry_index++)
                {
                    write_value[entry_index] = mem + array_index + index;
                    write_value[entry_index] = write_value[entry_index] & mem_field_mask[entry_index];
                }
                _SOC_IF_ERR_EXIT(soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem, array_index, copyno, index, read_value));
                for (entry_index = 0; entry_index < entry_dw; entry_index++)
                {
                    read_value[entry_index] = read_value[entry_index] & mem_field_mask[entry_index];
                }

                if (sal_memcmp(write_value, read_value, entry_dw*4))
                {
                    int ii;
                    cli_out("XOR Memory %s doesnt corrected to original value, array=%d, copyno=%d, index=%d\n", 
                            SOC_MEM_NAME(unit, mem), array_index, copyno, index);
                    cli_out("    %-16s", "write_value:");
                    for (ii=0; ii<entry_dw; ii++)
                    {
                        cli_out("%08X ", write_value[ii]);
                    }
                    cli_out("\n");

                    cli_out("    %-16s", "read_value:");
                    for (ii=0; ii<entry_dw; ii++)
                    {
                        cli_out("%08X ", read_value[ii]);
                    }
                    cli_out("\n");

                    cli_out("    %-16s", "mask_value:");
                    for (ii=0; ii<entry_dw; ii++)
                    {
                        cli_out("%08X ", mem_field_mask[ii]);
                    }
                    cli_out("\n---------------------------------------------------------------------\n");

                    rv = BCM_E_FAIL;
                }
            }
        }
        if (rv != BCM_E_NONE)
        {
            _SOC_IF_ERR_EXIT(SOC_E_FAIL);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int xor_mem_ser_test(int unit, soc_mem_t test_mem)
{
    soc_mem_t mem;
    int rv = SOC_E_NONE;

    SOC_INIT_FUNC_DEFS;

    if (test_mem != INVALIDm)
    {
        mem = test_mem;
        rv = BCM_E_PARAM;
        if (!SOC_MEM_IS_VALID(unit, mem)) 
        {
            _SOC_IF_ERR_EXIT(rv);
        }
        if (SOC_MEM_TYPE(unit,mem) != SOC_MEM_TYPE_XOR)
        {
            _SOC_IF_ERR_EXIT(rv);
        }
        rv = xor_mem_ser_entry_fill_check(unit, mem);
        _SOC_IF_ERR_EXIT(rv);
    }
    else
    {
        for (mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            if (!SOC_MEM_IS_VALID(unit, mem)) 
            {
                continue;
            }
            if (SOC_MEM_TYPE(unit,mem) != SOC_MEM_TYPE_XOR)
            {
                continue;
            }
            if (!sand_mem_contain_one_of_the_fields(unit,mem,fields))
            {
                continue;
            }
            if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem))
            {
                continue;
            }
            if (sal_strstr(SOC_MEM_NAME(unit,mem), "BRDC_") != NULL)
            {
                continue;
            }
            if (soc_mem_is_signal(unit, mem))
            {
                continue;
            }

            rv = xor_mem_ser_entry_fill_check(unit, mem);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (rv != BCM_E_NONE)
    {
        test_error(unit, "XOR Memory Ser Test Failed!\n");
    }
    SOC_FUNC_RETURN;
}


/*
 * Function:
 *      memory_ser_test_run
 * Purpose:
 *      run the test, ser test should simulate ser error and check its handling
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int memory_ser_test_run(int unit, args_t *a, void *p)
{
    uint32      ii;
    bcm_error_t rv = BCM_E_NONE;
    ser_test_params_t *ser_test_params = NULL;

    ser_test_params = ser_parameters[unit];

    /** enable dynamic **/
    enable_dynamic_memories_access(unit);

    ser_test_params->total_counter=0;
    ser_test_params->error_counter=0;
    ser_test_params->skipped_counter=0;
    ser_test_params->unjustified_skipped_counter=0;
    sal_srand(sal_time());
    if (ser_test_params->xor_test)
    {
        rv = xor_mem_ser_test(unit, ser_test_params->mem);
        return rv;
    }
    if (!ser_test_params->run_all) {
        int instance;
        soc_mem_t mem =  ser_test_params->mem;
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            if (ser_test_params->mdb)
            {
                rv = dnx_mdb_phymem_ser_verify(unit, mem, ser_test_params->mdb);
                return rv;
            }
        }
#endif
        update_mem_int(unit);

        if (ser_test_params->copyno == -1)
        {
            ser_test_params->copyno = 0;
            SOC_MEM_BLOCK_ITER(unit,mem,instance)
            {
                rv = memory_ser_test_run_mem(unit,NULL,NULL);
                if (rv != BCM_E_NONE) 
                {
                    break;
                }
                ser_test_params->copyno++; 
            }
        }
        else
        {
            rv = memory_ser_test_run_mem(unit,NULL,NULL);
        }
        

        return rv;
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        if (ser_test_params->mdb)
        {
            if (dnx_mdb_phymem_ser_verify(unit, INVALIDm, ser_test_params->mdb) < 0)
            {
                LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "MDB SER_MEM_TEST: unit %d  failed\n"), unit));
            }
        }
    }
#endif

    if (soc_mem_iterate(unit, mem_ser_test_cb, &unit) < 0)
    {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "SER_MEM_TEST: unit %d  failed\n"), unit));
    }

#ifdef BCM_DNX_SUPPORT
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "MDB mem SER run: failed: %d\n"), ser_test_params->mdb_error_counter));
#endif
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "Mem run(%d) skipped(%d) unjustified_skipped counter(%d)\n"), 
                                ser_test_params->total_counter,
                                ser_test_params->skipped_counter,
                                ser_test_params->unjustified_skipped_counter
                                ));
    LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "failed: %d, param counter:%d, timeout counter:%d \n"), 
                                ser_test_params->error_counter - ser_test_params->timeout_counter,
                                ser_test_params->param_counter,
                                ser_test_params->timeout_counter
                                ));

    for (ii=0; ii<ser_test_params->error_counter; ii++)
    {
        LOG_INFO(BSL_LS_APPL_TESTS,(BSL_META_U(unit, "%-6d %s\n"), 
                                    ii, SOC_MEM_NAME(unit, ser_test_params->error_mem_list[ii])));
    }

    return rv;
}

/*
 * Function:
 *      memory_ser_test_done
 * Purpose:
 *      clean-up after ser test was done
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      BCM_E_XXX
 */
extern int memory_ser_test_done(int unit, void *p)
{
    LOG_INFO(BSL_LS_APPL_TESTS, (BSL_META_U(unit, "Ser Tests Done\n")));

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#ifdef BCM_WARM_BOOT_API_TEST
    dnxc_wb_no_wb_test_set(unit, 0);
#endif
#endif

    sal_free(ser_parameters[unit]);
    ser_parameters[unit] = NULL;
    return 0;
}

#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME

