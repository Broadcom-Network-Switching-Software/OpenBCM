/** \file access_init_mem.c
 * Slim SoC module to allow bcm actions.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MEM
/* *INDENT-OFF* */
#ifdef BCM_ACCESS_SUPPORT

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/gport.h>
#include <shared/utilex/utilex_str.h>
#include <bcm_int/control.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingress_cs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <sal/core/boot.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <soc/access/access.h>
#include <soc/access/auto_generated/common_enum.h>
#include <soc/dnxc/dnxc_access.h>

#define DNX_MEM_BRINGUP_DONE
/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc property functions */
#include <soc/dnx/drv.h>

#ifdef DNX_EMULATION
#define PRINT_MEM_INIT_TIME
#endif /* DNX_EMULATION */
/*
 * }
 */
/*
 * Typedefs.
 * {
 */
/*
 *
 */


/* registers to enable J2P dynamic memory writes */
const access_regmem_id_t access_init_mem_j2p_dynamic_mem_enable_regs[] = {
    rCDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCDBM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCFC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCGM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCRPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDHA_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDHB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rECGM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rECI_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEPNI_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rERPP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rESB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rETPPA_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rETPPB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rETPPC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEVNT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFCR_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFCT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFDT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFDR_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFSRD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rHBMC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rILE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPA_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPF_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIRE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rITPP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rITPPD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rKAPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMACT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMCP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMRPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMTM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOAMP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOCBM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOLP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPDM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPEM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rRQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rRTP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rSCH_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rSPB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rSQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rTCAM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    ACCESS_INVALID_LOCAL_REGMEM_ID
};

/* registers to disable J2P dynamic memory writes */
const access_regmem_id_t access_init_mem_j2p_dynamic_mem_disable_regs[] = {
    rCDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCDBM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCFC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCGM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rCRPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDHA_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDHB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDDP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rDQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rECGM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rECI_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEPNI_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rERPP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rESB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rEVNT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFCR_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFCT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFDT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFDR_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rFSRD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rILE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPA_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPC_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPPF_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIPT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rIRE_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rITPP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rITPPD_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rKAPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMACT_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMCP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMDB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMRPS_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rMTM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOAMP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOCBM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rOLP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPDM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPEM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rPQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rRQP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rRTP_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rSPB_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rSQM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    rTCAM_ENABLE_DYNAMIC_MEMORY_ACCESS,
    ACCESS_INVALID_LOCAL_REGMEM_ID
};

/**
 * \brief - Holds the information about the indirect memory write currently performed for a block instance.
 * The active structures (operations) are also held in a cyclic linked list.
 */
typedef struct
{
    access_local_block_instance_id_t blk_inst_id;       /* cast ID whose filling is tracked (block_instance id till
                                                         * casts are implemented) */
    access_local_regmem_id_t regmem_id; /* local regmem ID of memory being filled */
    uint32 index;               /* index of next memory range to be filled (index = mem_index + nof_indices *
                                 * (array_index-first_array_index)) */
    uint32 end_index;           /* set and not changed to nof_indices * nof_array_indices */
    uint32 not_done_counter;    /* number of times we checked if the operation was done, and it was not. */
    sal_usecs_t start_time;
    access_local_block_instance_id_t prev_blk_inst_id;
    access_local_block_instance_id_t next_blk_inst_id;
} access_indirect_mem_init_blk_inst_id_location_t;

shr_error_e access_init_mem_reset_cache_single_update(
    access_runtime_info_t * runtime_info,
    access_regmem_id_t mem,
    void *entry_data);

/**
 * \brief - Enable/disable dynamic memory writes using the given dynamic memory write control registers
 * \param [in] unit - unit
 * \param [in] regs - control registers to write to, terminated by INVALIDr
 * \param [in] val - value (0/1) to write to the registers
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
access_init_mem_dynamic_mem_writes(
    const int unit,
    const access_regmem_id_t * regs,
    const uint32 val)
{
    const access_regmem_id_t *r = regs;
    soc_reg_above_64_val_t data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    if (val)
    {
        SOC_REG_ABOVE_64_ALLONES(data);
    }

    for (; *r != ACCESS_INVALID_LOCAL_REGMEM_ID; ++r)
    {
        SHR_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE, *r, ACCESS_ALL_BLOCK_INSTANCES, 0, 0, data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - reset the tcam hit indication memories.
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem - local mem id
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
access_init_mem_tcam_hit_indication_reset(
    access_runtime_info_t * runtime_info,
    access_regmem_id_t mem)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + mem;
    access_local_block_id_t local_block_id;
    uint32 nof_instances;
    uint32 blk_inst_id;
    uint32 array_index, array_index_min, array_index_max;
    uint32 index, index_min, index_max;
    uint32 reg_val[2] = { 0 };
    uint32 cmd_type;
    uint32 wr_mask = 0xFFFFFFFF;
    uint32 mem_addr;
    soc_reg_above_64_val_t reg_val_zero;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    /** get mem info about table */
    local_block_id = rm_info->local_block_id;

    nof_instances = device_info->blocks[local_block_id].nof_instances;
    index_min = 0;
    index_max = rm_info->u.mem.mem_nof_elements - 1;
    array_index_min = rm_info->first_array_index;
    array_index_max = array_index_min + (rm_info->nof_array_indices - 1);

    /** add memory commands to database */
    for (blk_inst_id = 0; blk_inst_id < nof_instances; blk_inst_id++) /** loop over block instance numbers */
    {
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            for (index = index_min; index <= index_max; index++)
            {
                mem_addr = rm_info->base_address + array_index * rm_info->array_skip + index;
                if (dnx_data_field.tcam.tcam_indirect_command_version_get(unit) == 1)
                {
                    reg_val[0] = 0xE00003;
                    SHR_IF_ERR_EXIT(access_regmem
                                    (unit, FLAG_ACCESS_IS_WRITE, rTCAM_INDIRECT_COMMAND, ACCESS_ALL_BLOCK_INSTANCES, 0,
                                     0, reg_val));

                }
                else if (dnx_data_field.tcam.tcam_indirect_command_version_get(unit) == 2)
                {
                    reg_val[0] = 0xE000003;
                    SHR_IF_ERR_EXIT(access_regmem
                                    (unit, FLAG_ACCESS_IS_WRITE, rTCAM_INDIRECT_COMMAND, ACCESS_ALL_BLOCK_INSTANCES, 0,
                                     0, reg_val));
                }

                reg_val[0] = 0;
                reg_val[1] = 0;
                cmd_type = 0;
                SHR_IF_ERR_EXIT(access_field_set
                                (unit, rTCAM_INDIRECT_COMMAND_ADDRESS_fINDIRECT_COMMAND_ADDR, reg_val, &mem_addr));
                SHR_IF_ERR_EXIT(access_field_set
                                (unit, rTCAM_INDIRECT_COMMAND_ADDRESS_fINDIRECT_COMMAND_TYPE, reg_val, &cmd_type));

                SHR_IF_ERR_EXIT(access_regmem
                                (unit, FLAG_ACCESS_IS_WRITE, rTCAM_INDIRECT_COMMAND_ADDRESS, ACCESS_ALL_BLOCK_INSTANCES,
                                 0, 0, reg_val));

                SHR_IF_ERR_EXIT(access_regmem
                                (unit, FLAG_ACCESS_IS_WRITE, rTCAM_INDIRECT_WR_MASK, ACCESS_ALL_BLOCK_INSTANCES, 0, 0,
                                 &wr_mask));
                SHR_IF_ERR_EXIT(access_regmem
                                (unit, FLAG_ACCESS_IS_WRITE, rTCAM_INDIRECT_COMMAND_WR_DATA, ACCESS_ALL_BLOCK_INSTANCES,
                                 0, 0, reg_val_zero));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check if a memory is excluded from memory initialization
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem - local mem id
 *
 * \return
 *   0: exclude memory 1: init memory
 * \remark
 *   * None
 * \see
 *   * None
 */

static int
access_init_mem_exclude_list(
    access_runtime_info_t * runtime_info,
    access_local_regmem_id_t mem)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * * for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + mem;
    const access_device_block_t *local_block = device_info->blocks + rm_info->local_block_id;

    if (local_block->block_type == SOC_BLK_FSCL)
    {
        /** Excelude the entire block */
        return 0;
    }
    switch (rm_info->global_regmem_id)
    {
            /*
             * Should ignore - for indirect write.
             * The following five memories can not be accessed using the indirect mechanism, and do not need to be initialized with all the memories. Phymod handles them.
             */
        case mCLPORT_WC_UCMEM_DATA:
        case mSPEED_ID_TABLE:
        case mSPEED_PRIORITY_MAP_TBL:

            /*
             * The HBM memories are not real memories
             */
        case mBRDC_HBC_HBM_DRAM_CPU_ACCESS:
        case mHBC_HBM_DRAM_CPU_ACCESS:

            /*
             * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization
             */
        case mDDHB_MACRO_0_ABK_BANK_A:
        case mDDHB_MACRO_0_ABK_BANK_B:
        case mDDHB_MACRO_0_ENTRY_BANK:
        case mDDHB_MACRO_1_ABK_BANK_A:
        case mDDHB_MACRO_1_ABK_BANK_B:
        case mDDHB_MACRO_1_ENTRY_BANK:
        case mDDHA_MACRO_0_ABK_BANK_A:
        case mDDHA_MACRO_0_ABK_BANK_B:
        case mDDHA_MACRO_0_ENTRY_BANK:
        case mDDHA_MACRO_1_ABK_BANK_A:
        case mDDHA_MACRO_1_ABK_BANK_B:
        case mDDHA_MACRO_1_ENTRY_BANK:
        case mDDHB_MACRO_2_ABK_BANK_A:
        case mDDHB_MACRO_2_ABK_BANK_B:
        case mDDHB_MACRO_2_ENTRY_BANK:
        case mDDHA_MACRO_2_ABK_BANK_A:
        case mDDHA_MACRO_2_ABK_BANK_B:
        case mDDHA_MACRO_2_ENTRY_BANK:
            /*
             * These are address spaces saved for ilkn memory mapped registers.
             */
        case mILE_PORT_0_CPU_ACCESS:
        case mILE_PORT_1_CPU_ACCESS:

            /*
             * All below are either not real memories or don't have protection. - Mosi Ravia
             */
        case mKAPS_RPB_TCAM_CPU_COMMAND:
        case mKAPS_TCAM_ECC_MEMORY:
        case mMDB_ARM_KAPS_TCM:
        case mMACT_CPU_REQUEST:
        case mECI_SAM_CTRL:

            /*
             * SCH_SCHEDULER_INIT is not a real memory
             */
        case mSCH_SCHEDULER_INIT:
        case mSQM_PDM:
        case mERPP_EPMFCS_TCAM_HIT_INDICATION:
        case mIPPA_VTDCS_TCAM_HIT_INDICATION:
        case mIPPA_VTECS_TCAM_HIT_INDICATION:
        case mIPPB_FLPACS_TCAM_HIT_INDICATION:
        case mIPPB_FLPBCS_TCAM_HIT_INDICATION:
        case mIPPC_PMFACSA_TCAM_HIT_INDICATION:
        case mIPPC_PMFACSB_TCAM_HIT_INDICATION:
        case mIPPD_PMFBCS_TCAM_HIT_INDICATION:
        case mIPPE_PRTCAM_TCAM_HIT_INDICATION:
        case mIPPE_LLRCS_TCAM_HIT_INDICATION:
        case mIPPF_VTACS_TCAM_HIT_INDICATION:
        case mIPPF_VTCCS_TCAM_HIT_INDICATION:
        case mIPPF_VTBCS_TCAM_HIT_INDICATION:
        case mKAPS_RPB_TCAM_HIT_INDICATION:
        case mKAPS_TCAM_HIT_INDICATION:
            /*
             * J2P Need different initialization
             */
        case mAM_TABLE:
        case mCDMIB_MEM:
        case mCDPORT_TSC_UCMEM_DATA:
        case mRX_LKUP_1588_MEM_MPP0:
        case mRX_LKUP_1588_MEM_MPP1:
        case mTX_LKUP_1588_MEM_MPP0:
        case mTX_LKUP_1588_MEM_MPP1:
        case mUM_TABLE:
        case mCDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMA:
        case mCDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMB:
        case mCDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMC:
        case mCDBM_INSTRUMENTATION_STATS_MEM_PFC_MEMD:
        case mCDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMA:
        case mCDBM_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMB:
        case mCDBM_RX_MEMA_CTRL:
        case mCDBM_RX_MEMB_CTRL:
        case mCDBM_RX_MEMC_CTRL:
        case mCDBM_RX_MEMD_CTRL:
        case mCDB_INSTRUMENTATION_STATS_MEM_PFC_MEMA:
        case mCDB_INSTRUMENTATION_STATS_MEM_PFC_MEMB:
        case mCDB_INSTRUMENTATION_STATS_MEM_PFC_MEMC:
        case mCDB_INSTRUMENTATION_STATS_MEM_PFC_MEMD:
        case mCDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMA:
        case mCDB_INSTRUMENTATION_STATS_MEM_TRAFFIC_MEMB:
        case mCDB_RX_MEMA_CTRL:
        case mCDB_RX_MEMB_CTRL:
        case mCDB_RX_MEMC_CTRL:
        case mCDB_RX_MEMD_CTRL:
        case mMACSEC_ESEC_SC_TABLE:
        case mMACSEC_SUB_PORT_POLICY_TABLE:
        case mMACSEC_ESEC_MIB_MISC:
        case mMACSEC_ESEC_MIB_SA:
        case mMACSEC_ESEC_MIB_SC:
        case mMACSEC_ESEC_MIB_SC_CTRL:
        case mMACSEC_ESEC_MIB_SC_UN_CTRL:
        case mMACSEC_ESEC_SA_HASH_TABLE:
        case mMACSEC_ESEC_SA_TABLE:
        case mMACSEC_ISEC_MIB_SA:
        case mMACSEC_ISEC_MIB_SC:
        case mMACSEC_ISEC_MIB_SP_CTRL_1:
        case mMACSEC_ISEC_MIB_SP_CTRL_2:
        case mMACSEC_ISEC_MIB_SP_UNCTRL:
        case mMACSEC_ISEC_PORT_COUNTERS:
        case mMACSEC_ISEC_SA_HASH_TABLE:
        case mMACSEC_ISEC_SA_TABLE:
        case mMACSEC_ISEC_SCTCAM_HIT_COUNT:
        case mMACSEC_ISEC_SC_TABLE:
        case mMACSEC_ISEC_SC_TCAM:
        case mMACSEC_ISEC_SPTCAM_HIT_COUNT:
        case mMACSEC_ISEC_SP_TCAM:
        case mMACSEC_ISEC_SVTAG_CPU_FLEX_MAP:
        case mMACSEC_MACSEC_TDM_2_CALENDAR:
        case mMACSEC_MACSEC_TDM_CALENDAR:
        case mMACSEC_SC_MAP_TABLE:
        case mMACSEC_SUB_PORT_MAP_TABLE:
            /*
             * Should ignore - Investigated
             */
        case mOCBM_OCB_CPU_ACCESS:
        case mDDP_BDBC_UC:
        case mHBMC_HBM_PHY_REGISTER_ACCESS:
        case mOCBM_FBM_BANK_MEM:
        case mPDM_MEM_0000000:
        case mPDM_MEM_2000000:
        case mPDM_MEM_2100000:

            return 0;

        default:
            return 1;
    }
}

/**
 * \brief - Fill the whole table with the given entry, uses fast DMA filling when run on real hardware
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] regmem - the memory to fill
 * \param [in] inst - The block instance number to access. Blocks may have multiple instances
 * \param [in] entry_data - the entry data
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
uint32
access_init_mem_fill_memory_with_entry(
    access_runtime_info_t * runtime_info,
    access_local_regmem_id_t regmem,
    access_block_instance_num_t inst,   /* The block instance number to access. Blocks may have multiple instances */
    uint32 *entry_data)
{
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + regmem;
    int unit = runtime_info->unit;
    uint32 array_index, array_index_min, array_index_max;

    SHR_FUNC_INIT_VARS(unit);

    if (rm_info->array_skip == rm_info->u.mem.mem_nof_elements)
    {
        SHR_IF_ERR_EXIT(access_fill_partial_local_regmem(runtime_info, 0, regmem, inst, rm_info->first_array_index, 0,
                                                         rm_info->u.mem.mem_nof_elements * rm_info->nof_array_indices,
                                                         0, entry_data));
    }
    else
    {
        array_index_min = rm_info->first_array_index;
        array_index_max = array_index_min + (rm_info->nof_array_indices - 1);
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            SHR_IF_ERR_EXIT(access_fill_partial_local_regmem
                            (runtime_info, 0, regmem, inst, array_index, 0, rm_info->u.mem.mem_nof_elements, 0,
                             entry_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Initialize (write software init value) all valid writeable memories which are not included in mem_exclude_bitmap using SLAM DMA.
 *
 * \param [in] unit - unit #
 * \param [in] mem_exclude_bitmap - Each bit represent the memories that should be excluded (will be skipped)
 * \param [in] reset_value - Init value (only LSB) - used for testing - should be zero for standard initialization.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
access_init_mems_using_dma(
    int unit,
    uint32 *mem_exclude_bitmap,
    uint32 reset_value)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info;
    access_regmem_id_t local_mem;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 nof_mems_left;
    uint64 regmem_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** set reset value */
    entry[0] = reset_value;

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (rm_info = device_info->local_mems, local_mem = device_info->nof_regs, nof_mems_left = device_info->nof_mems;
         nof_mems_left; nof_mems_left--, local_mem++, rm_info++)
    {
        regmem_flags = rm_info->flags;

        /** Skip the zeroing of various memories */
        if (((regmem_flags & (ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_IS_ALIAS)) != 0) || /** filter read only and alias memories */
            SHR_BITGET(mem_exclude_bitmap, local_mem - device_info->nof_regs) ||  /** filter memories that were already filled, or marked not to be filled */
            access_init_mem_exclude_list(runtime_info, local_mem) == 0) /** filter special memories - should be moved to another function */
        {
            continue;
        }
        /*
         * The ECGM and CFC memories are filled separately
         */

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Zeros memory # %d - %s%s%s\n", local_mem,
                     access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]], EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                        (unit,
                         access_init_mem_fill_memory_with_entry(runtime_info, local_mem, ACCESS_ALL_BLOCK_INSTANCES,
                                                                entry)));
    }

    SHR_IF_ERR_EXIT(access_init_mem_tcam_hit_indication_reset(runtime_info, mTCAM_TCAM_ACTION_HIT_INDICATION));
    SHR_IF_ERR_EXIT(access_init_mem_tcam_hit_indication_reset(runtime_info, mTCAM_TCAM_ACTION_HIT_INDICATION_SMALL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Check if the last indirect operation for the block instance is done.
*
* \param [in] runtime_info - Access runtime info of the device
* \param [in] block_inst_id - specify the block instance id
* \param [out] is_done - specify the last indirect operation status. 1 - done, 0 - otherwise.
*
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
int
access_init_mem_indirect_operation_is_done(
    access_runtime_info_t * runtime_info,
    access_block_instance_num_t block_inst_id,
    uint32 *is_done)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    uint32 regval[2];
    uint32 count;
    uint32 width_in_uint32s;
    int sbus_blk_inst_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(access_regmem_get_width_in_uint32s(unit, rECI_INDIRECT_COMMAND, &width_in_uint32s));

    *is_done = 0;

    /*
     * Get indirect memory access registers cmic block
     */
    sbus_blk_inst_id = device_info->block_instances[block_inst_id].sbus_info.sbus_block_id;
    /** Verify last indirect operation over block is done */
    SHR_IF_ERR_EXIT(access_direct_regmem(unit, 0, sbus_blk_inst_id, ACCESS_REG_INDIRECT_COMMAND, width_in_uint32s, regval));
    SHR_IF_ERR_EXIT(access_field_get(unit, rECI_INDIRECT_COMMAND_fINDIRECT_COMMAND_COUNT, regval, &count));
    if (!count)
    {
        *is_done = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Start an indirect write to a memory, of continuous entries in the memory.
*          This function is used to initialize device memories in different block instances in parallel..
*          (Making sure the last operation in the block is done, but do not wait for current operation to finish)
*          For wide memories the contents of the INDIRECT_COMMAND_WR_DATA is used repeatedly to fill each entry.
*
* \param [in] runtime_info - Access runtime info of the device
* \param [in] write_operation - write operation parameters
* \param [in] mem_index - The memory element=index if this is a memory, or 0
* \param [in] nof_entries - number of entried to write
* \param [in] value - value of a single entry
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
int
access_init_mems_using_indirect_write(
    access_runtime_info_t * runtime_info,
    access_indirect_mem_init_blk_inst_id_location_t * write_operation,
    uint32 mem_index,
    int nof_entries,
    uint32 *value)
{
    int unit = runtime_info->unit;
    uint32 address, val;
    uint32 regval[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    unsigned i, entry_size;
    access_sbus_block_id_t sbus_blk_inst_id;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + write_operation->regmem_id;
    const access_device_block_t *local_block = device_info->blocks + rm_info->local_block_id;
    const unsigned indirect_write_size = local_block->indirect_write_reg_nof_uint32s;
    uint32 width_in_uint32s;

    SHR_FUNC_INIT_VARS(unit);

    entry_size = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);
    if (indirect_write_size == 0)
    {
        SHR_ERR_EXIT(SOC_E_PARAM, "Block %s of memory %s does not support indirect writes.\n",
                     access_global_block_types[local_block->block_type].block_name,
                     access_global_names[global_regmem_name_indices[rm_info->global_regmem_id]]);
    }

    /** Get indirect memory access registers addresses and cmic block */
    address = rm_info->base_address + mem_index;
    sbus_blk_inst_id = device_info->block_instances[write_operation->blk_inst_id].sbus_info.sbus_block_id;

    /** perform indirect write */
    SHR_IF_ERR_EXIT(access_field32_set
                    (runtime_info, rECI_INDIRECT_COMMAND_fINDIRECT_COMMAND_TRIGGER_ON_DATA, regval, 1));
    /** configure access timeout */
    SHR_IF_ERR_EXIT(access_field32_set(runtime_info, rECI_INDIRECT_COMMAND_fINDIRECT_COMMAND_TIMEOUT, regval, 0xe00));
    /** number of entries to write */
    SHR_IF_ERR_EXIT(access_field32_set
                    (runtime_info, rECI_INDIRECT_COMMAND_fINDIRECT_COMMAND_COUNT, regval, nof_entries));

    SHR_IF_ERR_EXIT(access_regmem_get_width_in_uint32s(unit, rECI_INDIRECT_COMMAND, &width_in_uint32s));

    SHR_IF_ERR_EXIT(access_direct_regmem
                    (unit, FLAG_ACCESS_IS_WRITE, sbus_blk_inst_id, ACCESS_REG_INDIRECT_COMMAND, width_in_uint32s,
                     regval));
    SHR_IF_ERR_EXIT(access_direct_regmem
                    (unit, FLAG_ACCESS_IS_WRITE, sbus_blk_inst_id, ACCESS_REG_INDIRECT_COMMAND_ADDRESS, 1, &address));

    write_operation->start_time = sal_time_usecs();

    if (entry_size > indirect_write_size)
    {
        val = 1;
        SHR_IF_ERR_EXIT(access_direct_regmem
                        (unit, FLAG_ACCESS_IS_WRITE, sbus_blk_inst_id, ACCESS_REG_INDIRECT_WIDE, 1, &val));
    }

    for (i = 0; i < (entry_size + indirect_write_size - 1) / indirect_write_size; i++)
    {
        SHR_IF_ERR_EXIT(access_direct_regmem
                        (unit, FLAG_ACCESS_IS_WRITE, sbus_blk_inst_id, ACCESS_REG_INDIRECT_COMMAND_WR_DATA,
                         indirect_write_size, value));
    }

    if (entry_size > indirect_write_size)
    {
        val = 0;
        SHR_IF_ERR_EXIT(access_direct_regmem
                        (unit, FLAG_ACCESS_IS_WRITE, sbus_blk_inst_id, ACCESS_REG_INDIRECT_WIDE, 1, &val));
    }

exit:
    if (SHR_FUNC_ERR())
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  ("Error: indirect memory write failed in memory %s (blk_index=%d, index=%d).\n",
                   ACCESS_REGMEM_NAME(rm_info->global_regmem_id), write_operation->blk_inst_id, mem_index));
    }
    SHR_FUNC_EXIT;
}

/* function to find the next memory to fill in the given block from the given memory */
static access_local_regmem_id_t
access_find_next_mem_to_fill(
    access_runtime_info_t * runtime_info,
    const access_device_block_t * local_block,/** block to return a memory from */
    access_local_regmem_id_t start_mem_id, /** local regmem ID of the memory belonging to the block from which to find a memory to fill */
    uint32 *mem_exclude_bitmap) /** exclude bitmap */
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    access_mem_sw_init_values_t *mem_sw_init_values = runtime_info->mem_sw_init_values;
    const access_device_regmem_t *rm_info;
    access_local_regmem_id_t memories_end, blk_local_mem_id;    /* ID of the first memory of the block, and the block
                                                                 * memories are continuous */
    uint64 regmem_flags;

    memories_end = local_block->memories_start + local_block->nof_memories;

    for (blk_local_mem_id = start_mem_id; blk_local_mem_id < memories_end; blk_local_mem_id++)
    {
        rm_info = device_info->local_regs + blk_local_mem_id;
        regmem_flags = rm_info->flags;
        /** Skip the zeroing of various memories */
        if ((regmem_flags & (ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_IS_ALIAS)) != 0 ||
                                                                 /** filter read only and alias memories */
            SHR_BITGET(mem_exclude_bitmap, blk_local_mem_id - device_info->nof_regs) ||  /** filter memories that were already filled, or marked not to be filled */
            (access_init_mem_exclude_list(runtime_info, blk_local_mem_id) == 0) || /** filter special memories - should be moved to another function */
            (soc_sand_is_emulation_system(unit) &&
             mem_sw_init_values[blk_local_mem_id - device_info->nof_regs].init_mode != dnx_sw_init_mem_mode_custom_fill_value))  /*** filter not custom memories in emulation */
                                              
        {
            continue;
        }

        return blk_local_mem_id;
    }

    return ACCESS_INVALID_LOCAL_REGMEM_ID;
}

/**
* \brief - Fill the memories with zero value using indirect writes.
*          Work on the writable memories of the device which are not included in mem_exclude_bitmap.
*
* \param [in] runtime_info - Access runtime info of the device
* \param [in] mem_exclude_bitmap - Each bit represent the memories that should be excluded (will be skipped)
* \return
*   See shr_error_e
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
access_init_mems_using_indirect(
    access_runtime_info_t * runtime_info,
    uint32 *mem_exclude_bitmap)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info;
    const access_device_block_t *local_block;
    uint32 blk_index, blk_inst_end_index;
    access_local_block_instance_id_t blk_inst_id;
    access_local_regmem_id_t local_mem_id;
    access_local_block_instance_id_t list_head = ACCESS_INVALID_LOCAL_BLOCK_INSTANCE_ID;
    access_indirect_mem_init_blk_inst_id_location_t *indirect_mem_init_blk_inst_id_list_ptr = NULL;
    access_indirect_mem_init_blk_inst_id_location_t *write_operation;
    uint32 is_done, block_instance_init_done;
    uint32 max_entries_per_indirect_operation = SOC_IS_J2P(unit) ? (1 << 14) - 1 : (1 << 18) - 1;
    uint32 index_in_array_element, operation_mem_index, operation_array_index, operation_nof_entries;

    SHR_FUNC_INIT_VARS(unit);

    /** create cast linked list */
    SHR_ALLOC_SET_ZERO(indirect_mem_init_blk_inst_id_list_ptr,
                       device_info->nof_block_instances * sizeof(access_indirect_mem_init_blk_inst_id_location_t),
                       "indirect_mem_init_cast_list", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** init block instance linked list */
    for (blk_index = 0; blk_index < device_info->nof_blocks; blk_index++)
    {
        local_block = &device_info->blocks[blk_index];
        /** get first memory to fill in the block */
        local_mem_id =
            access_find_next_mem_to_fill(runtime_info, local_block, local_block->memories_start, mem_exclude_bitmap);

        if (local_mem_id != ACCESS_INVALID_LOCAL_REGMEM_ID)
        {
            rm_info = device_info->local_regs + local_mem_id;
            operation_nof_entries = rm_info->u.mem.mem_nof_elements;
            if (operation_nof_entries > max_entries_per_indirect_operation)
            {
                operation_nof_entries = max_entries_per_indirect_operation;
            }
            operation_mem_index = rm_info->first_array_index * rm_info->array_skip;

            blk_inst_end_index = local_block->instances_start_index + local_block->nof_instances - 1;
            for (blk_inst_id = local_block->instances_start_index; blk_inst_id <= blk_inst_end_index; blk_inst_id++)
            {
                if ((runtime_info->block_instance_flags[blk_inst_id] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED) !=
                    0)
                {
                    continue; /** skip disabled block instances */
                }

                write_operation = &indirect_mem_init_blk_inst_id_list_ptr[blk_inst_id];
                write_operation->blk_inst_id = blk_inst_id;
                write_operation->regmem_id = local_mem_id;
                /** store the end of current write operation including the array offset */
                write_operation->index = operation_mem_index + operation_nof_entries;
                /** set and not changed to nof_indices nof_array_indices */
                write_operation->end_index =
                    operation_mem_index + rm_info->u.mem.mem_nof_elements * rm_info->nof_array_indices;

                /** Add write_operation to the linked list */
                if (list_head == ACCESS_INVALID_LOCAL_BLOCK_INSTANCE_ID)
                {
                    list_head = blk_inst_id;
                    write_operation->prev_blk_inst_id = write_operation->next_blk_inst_id = blk_inst_id;
                }
                else
                {
                    write_operation->prev_blk_inst_id =
                        indirect_mem_init_blk_inst_id_list_ptr[list_head].prev_blk_inst_id;
                    write_operation->next_blk_inst_id = list_head;
                    indirect_mem_init_blk_inst_id_list_ptr[list_head].prev_blk_inst_id = blk_inst_id;
                    indirect_mem_init_blk_inst_id_list_ptr[write_operation->prev_blk_inst_id].next_blk_inst_id =
                        blk_inst_id;
                    if (indirect_mem_init_blk_inst_id_list_ptr[list_head].next_blk_inst_id == list_head)
                    {
                        indirect_mem_init_blk_inst_id_list_ptr[list_head].next_blk_inst_id = blk_inst_id;
                    }
                }

                write_operation->not_done_counter = 0;
                /** Start write operation here */
                SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                                (unit,
                                 access_init_mems_using_indirect_write(runtime_info, write_operation,
                                                                       operation_mem_index, operation_nof_entries,
                                                                       runtime_info->mem_sw_init_values
                                                                       [write_operation->regmem_id -
                                                                        device_info->nof_regs].u.custom_init_value)));
            }
        }
    }

    for (blk_inst_id = list_head; list_head != ACCESS_INVALID_LOCAL_BLOCK_INSTANCE_ID;)
    {
        block_instance_init_done = FALSE;

        /** check if last indirect operation finished */
        SHR_IF_ERR_EXIT(access_init_mem_indirect_operation_is_done(runtime_info, blk_inst_id, &is_done));
        if (is_done)
        {
            write_operation = &indirect_mem_init_blk_inst_id_list_ptr[blk_inst_id];
            /** check if last indirect operation memory is fully initialized */
            rm_info = device_info->local_regs + write_operation->regmem_id;
            if (write_operation->index < write_operation->end_index)
            { /** We are still in the same memory */
                index_in_array_element = write_operation->index % rm_info->u.mem.mem_nof_elements;
                if (index_in_array_element)
                {
                    operation_nof_entries = rm_info->u.mem.mem_nof_elements - index_in_array_element;
                    if (operation_nof_entries > max_entries_per_indirect_operation)
                    {
                        operation_nof_entries = max_entries_per_indirect_operation;
                    }

                }
                else
                { /** move to memory next array */
                    operation_nof_entries = rm_info->u.mem.mem_nof_elements;
                    if (operation_nof_entries > max_entries_per_indirect_operation)
                    {
                        operation_nof_entries = max_entries_per_indirect_operation;
                    }
                }
                operation_array_index = write_operation->index / rm_info->u.mem.mem_nof_elements;
                operation_mem_index = index_in_array_element + operation_array_index * rm_info->array_skip;

                write_operation->index += operation_nof_entries;
            }
            else
            { /** previous memory is fully initialized move to next memory */
                /** get next memory to fill in the block */
                local_block = &device_info->blocks[rm_info->local_block_id];
                local_mem_id =
                    access_find_next_mem_to_fill(runtime_info, local_block, write_operation->regmem_id + 1,
                                                 mem_exclude_bitmap);
                if (local_mem_id != ACCESS_INVALID_LOCAL_REGMEM_ID)
                {
                    rm_info = device_info->local_regs + local_mem_id;
                    operation_nof_entries = rm_info->u.mem.mem_nof_elements;
                    if (operation_nof_entries > max_entries_per_indirect_operation)
                    {
                        operation_nof_entries = max_entries_per_indirect_operation;
                    }
                    operation_mem_index = rm_info->first_array_index * rm_info->array_skip;

                    write_operation->regmem_id = local_mem_id;
                    /** store the end of current write opweration including the array offset */
                    write_operation->index = operation_mem_index + operation_nof_entries;
                    /** set and not changed to nof_indices nof_array_indices */
                    write_operation->end_index =
                        operation_mem_index + rm_info->u.mem.mem_nof_elements * rm_info->nof_array_indices;

                }
                else
                /** block instance memories are all initialized remove block instance from list */
                {
                    block_instance_init_done = TRUE;
                    if (write_operation->prev_blk_inst_id == blk_inst_id)
                    {
                        /** no more block instances to initialize */
                        assert(write_operation->next_blk_inst_id == blk_inst_id);
                        list_head = ACCESS_INVALID_LOCAL_BLOCK_INSTANCE_ID;
                    }
                    else
                    {
                        indirect_mem_init_blk_inst_id_list_ptr[write_operation->prev_blk_inst_id].next_blk_inst_id =
                            write_operation->next_blk_inst_id;
                        indirect_mem_init_blk_inst_id_list_ptr[write_operation->next_blk_inst_id].prev_blk_inst_id =
                            write_operation->prev_blk_inst_id;
                    }
                }
            }

            if (!block_instance_init_done)
            {
                write_operation->not_done_counter = 0;
                /** Start new indirect operation */
                SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                                (unit,
                                 access_init_mems_using_indirect_write(runtime_info, write_operation,
                                                                       operation_mem_index, operation_nof_entries,
                                                                       runtime_info->mem_sw_init_values
                                                                       [write_operation->regmem_id -
                                                                        device_info->nof_regs].u.custom_init_value)));
            }

        }
        else
        {
            write_operation = &indirect_mem_init_blk_inst_id_list_ptr[blk_inst_id];
            /** Check if the timeout has passed, and if it has print an error and move to the next block_instanse
             */
            if (write_operation->not_done_counter >= 2)
            {
                if (sal_time_usecs() - write_operation->start_time > runtime_info->tableDmaTimeout)
                {
                    rm_info = device_info->local_regs + write_operation->regmem_id;
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Timeout on block %d, memory %s local_mem_id %d, count %d\n",
                                 (blk_inst_id), ACCESS_REGMEM_NAME(rm_info->global_regmem_id),
                                 write_operation->regmem_id, write_operation->not_done_counter);
                    /** blk is in a bad state remove block instanse from list */
                    if (write_operation->prev_blk_inst_id == blk_inst_id)
                    {
                        /** no more block instances to initialize */
                        assert(write_operation->next_blk_inst_id == blk_inst_id);
                        list_head = ACCESS_INVALID_LOCAL_BLOCK_INSTANCE_ID;
                    }
                    else
                    {
                        indirect_mem_init_blk_inst_id_list_ptr[write_operation->prev_blk_inst_id].next_blk_inst_id =
                            write_operation->next_blk_inst_id;
                        indirect_mem_init_blk_inst_id_list_ptr[write_operation->next_blk_inst_id].prev_blk_inst_id =
                            write_operation->prev_blk_inst_id;
                    }
                }
            }
            write_operation->not_done_counter++;
        }

        /** advance to next block instance*/
        blk_inst_id = indirect_mem_init_blk_inst_id_list_ptr[blk_inst_id].next_blk_inst_id;
    }

exit:
    SHR_FREE(indirect_mem_init_blk_inst_id_list_ptr);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initializes cache of memories to zero, for emulation/adapter, for memories that would be set to zero if not on emulation/adapter.
 * \param [in] unit - unit #
 * \param [in] mem_exclude_bitmap - bitmap for memories initalization status

 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
access_init_mem_reset_cache_update(
    int unit,
    uint32 *mem_exclude_bitmap)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    const access_device_regmem_t *rm_info;
    uint64 regmem_flags;
    access_regmem_id_t local_mem;
    uint32 mem_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set the rest of the tables to zero
     */
    /** iterate over all memories */
    for (rm_info = device_info->local_mems, local_mem = device_info->nof_regs, mem_index = device_info->nof_mems;
         mem_index; mem_index--, local_mem++, rm_info++)
    {
        regmem_flags = rm_info->flags;

        /** Skip the zeroing of various memories */
        if (((regmem_flags & (ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_IS_ALIAS)) != 0) || /** filter alias and read only mem */
            SHR_BITGET(mem_exclude_bitmap, local_mem) ||                /** filter memories that were already filled, or marked not to be filled */
            access_init_mem_exclude_list(runtime_info, local_mem) == 0) /** filter special memories - should be moved to another function */
        {
            continue;
        }

        SHR_IF_ERR_EXIT(access_init_mem_reset_cache_single_update(runtime_info, local_mem, entry));

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fills the cache of the given memory with the given entry value.
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem - the local mem id
 * \param [in] entry_data - entry data
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
access_init_mem_reset_cache_single_update(
    access_runtime_info_t * runtime_info,
    access_regmem_id_t mem,
    void *entry_data)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info = device_info->local_regs + mem;
    uint16 array_index_min, array_index_max;
    uint32 index_min, index_max;
    SHR_FUNC_INIT_VARS(unit);

    /** get mem info about table */
    index_min = 0;
    index_max = rm_info->u.mem.mem_nof_elements - 1;
    array_index_min = rm_info->first_array_index;
    array_index_max = array_index_min + (rm_info->nof_array_indices - 1);

    /** add memory commands to database */
    access_local_regmem_fill_cache_update_range(runtime_info,
                                                0,
                                                mem,
                                                ACCESS_ALL_BLOCK_INSTANCES,
                                                array_index_min, array_index_max, index_min, index_max, entry_data);
    SHR_FUNC_EXIT;
}
#if 0
/**
 * \brief - enable/disable caching for the given memory (all instances)
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] local_regmem - local mem id
 * \param [in] enable - enable/disable
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
access_init_mem_cache_enable_set(
    access_runtime_info_t * runtime_info,
    access_regmem_id_t local_regmem,
    int enable)
{
    int unit = runtime_info->unit;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(access_local_regmem_cache_set(runtime_info, local_regmem, enable));

exit:
    SHR_FUNC_EXIT;
}
#endif
/**
 * \brief - write the memory according to custom values got from a callback using DMA.
 * \param [in] runtime_info - Access runtime info of the device
 * \param [in] mem - local mem id
 * \param [in] field_id - local field id
 * \param [in] sw_init_mem_cb - ptr to sw mem init callback function
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
access_init_custom_mem_per_entry(
    access_runtime_info_t * runtime_info,
    access_local_regmem_id_t mem,
    access_local_field_id_t field_id,
    dnx_sw_init_mem_custom_value_get_f sw_init_mem_cb)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_device_regmem_t *rm_info;
    uint32 *entries = NULL, *entry = NULL;
    uint32 entry_words;
    int index, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    dnx_sw_init_mem_info mem_info;

    SHR_FUNC_INIT_VARS(unit);

    rm_info = device_info->local_regs + mem;

    mem_info.mem = mem;
    mem_info.field_id = field_id;

    /** Allocate dma buffer */
    SHR_IF_ERR_EXIT(sand_alloc_dma_mem
                    (unit, 0, (void **) &entries,
                     rm_info->u.mem.mem_nof_elements * ACESSS_REGMEM_T_SBUS_WIDTH_IN_BYTES(rm_info),
                     "access_init_custom_mem_per_entry"));

    /** get info about table */
    entry_words = ACESSS_REGMEM_T_WIDTH_IN_UINT32S(rm_info);

    index_max = rm_info->u.mem.mem_nof_elements - 1;
    array_index_min = rm_info->first_array_index;
    array_index_max = array_index_min + (rm_info->nof_array_indices - 1);

    /** add memory commands to database */
    for (array_index = array_index_min; array_index <= array_index_max; array_index++)
    {
        for (index = 0; index <= index_max; index++)
        {
            entry = entries + (entry_words * index);
            SHR_IF_ERR_EXIT(sw_init_mem_cb(runtime_info, &mem_info, array_index, index, entry));
        }
        SHR_IF_ERR_EXIT(access_local_regmem_sbusdma
                        (runtime_info, FLAG_ACCESS_IS_WRITE, mem, ACCESS_ALL_BLOCK_INSTANCES,
                         array_index, 0, rm_info->u.mem.mem_nof_elements, 0, entries));
    }

exit:
    if (entries != NULL)
    {
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, 0, (void **) &entries));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset TCAM CS tables
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
access_init_mem_tcam_cs_reset(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Reset ingress CS tcam tables */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_PRT_VIRTUAL_PORT_TCAM));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LLR_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT1_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT2_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT3_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT4_CONTEXT_SELECTION));
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_VT5_CONTEXT_SELECTION));
    }
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD1_CONTEXT_SELECTION));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD_ACL_CONTEXT_SELECTION));
    /** Reset PMF CS tables   */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF1));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF2));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_IPMF3));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_TCAM_CS_EPMF));
    /** Reset IPPB_MPLS_LABEL_TYPES_CAM_0/1 */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LOAD_BALANCING_MPLS_1ST_STACK_FORCE_LABELS_HL_TCAM));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_LOAD_BALANCING_MPLS_2ND_STACK_FORCE_LABELS_HL_TCAM));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Handle simulation and emulation tables init - verify tables were set externally.
 *
 * \param [in] unit - unit #
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
access_init_mem_sim_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * in case of emulation, use external script to init all memories (even non zero memories)
     * if such a script provided, otherwise assume all memories were already set to 0 externally.
     */
    if (soc_sand_is_emulation_system(unit) && dnx_data_dev_init.mem.emul_ext_init_get(unit))
    {
    }
    else
    {
        /** If we rely on memories to be externally zeroed before the run, check for it */
        if (!dnx_data_dev_init.mem.force_zeros_get(unit))
        {
            {
                uint32 entry = 0xffffffff;
                SHR_IF_ERR_EXIT(access_regmem
                                (unit, 0, mCGM_REP_CMD_FIFO_OCCUPANCY_RJCT_TH, ACCESS_ALL_BLOCK_INSTANCES, 0, 0,
                                 &entry));
                if (entry != 0)
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_MEMORY,
                                      "This run expects all memories to be zeroed before the run, and they were not%s%s%s",
                                      EMPTY, EMPTY, EMPTY);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifndef DNX_MEM_BRINGUP_DONE
#define SAND_IN_BRINGUP
/*
 * initialize all memories to zero, except for exception array, and marked not to be zeroed.
 */
shr_error_e
access_init_mem_mems_zero(
    access_runtime_info_t * runtime_info,
    uint32 *mem_exclude_bitmap)
{
    int unit = runtime_info->unit;
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * * for the device type */
    uint32 entry0[128] = { 0 };
    uint32 mem_index;
    access_regmem_id_t local_mem;
    const access_device_regmem_t *rm_info;
    uint64 regmem_flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Zero tables if not running in emulation/simulation
     */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_tbls_to_zero", !(
#ifdef PLISIM
                                                                                            SAL_BOOT_PLISIM ||
                          /** not pcid and not emulation */
#endif
                                                                                            soc_sand_is_emulation_system
                                                                                            (unit))))
    {
        /*
         * Zero all the memories that need it. Memories excluding read only, not aliases, not filled earlier...
         */
        /** iterate over all memories */
        for (rm_info = device_info->local_mems, local_mem = device_info->nof_regs, mem_index = device_info->nof_mems;
             mem_index; mem_index--, local_mem++, rm_info++)
        {
            regmem_flags = rm_info->flags;

            /** Skip the zeroing of various memories */
            if ((regmem_flags & (ACCESS_REGMEM_FLAG_RO | ACCESS_REGMEM_FLAG_IS_ALIAS)) != 0 || /** filter read only and alias memories */
                SHR_BITGET(mem_exclude_bitmap, local_mem - device_info->nof_regs) || /** filter memories that were already filled, or marked not to be filled */
                access_init_mem_exclude_list(runtime_info, local_mem) == 0) /** filter special memories - should be moved to another function */
            {
                continue;
            }

            /*
             * reset memory - set all values to 0
             */
            LOG_VERBOSE(BSL_LS_SOC_INIT,
                        (BSL_META_U(unit, "Reseting memory # %d - %s\n"), local_mem,
                         ACCESS_REGMEM_NAME(rm_info->global_regmem_id)));

#ifndef SAND_IN_BRINGUP
            SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                            (unit,
                             access_init_mem_fill_memory_with_entry(runtime_info, local_mem, ACCESS_ALL_BLOCK_INSTANCES,
                                                                    entry0)));
#else
            /*
             * on startup report memory filling errors instead of stopping
             */
            if (access_init_mem_fill_memory_with_entry(runtime_info, local_mem, ACCESS_ALL_BLOCK_INSTANCES, entry0) !=
                _SHR_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit, "Failed filling memory # %d - %s\n"), local_mem,
                           ACCESS_REGMEM_NAME(rm_info->global_regmem_id)));
            }
#endif
        }
    }
#ifndef SAND_IN_BRINGUP
exit:
#endif
    SHR_FUNC_EXIT;
}

/*
 * Initialize memory zeroing exclusion to the given array
 * Sets the mems_bitmap exclusion bitmap using local IDs,
 * given the mem_exclude_list array of global memory IDs
 */
void
access_init_mem_mems_zero_init(
    int unit,
    access_regmem_id_t * mem_exclude_list,
    uint32 *mems_bitmap)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * * for the device */
    access_regmem_id_t *excluded_list = mem_exclude_list;       /* excluded memory list iterator */
    access_local_regmem_id_t local_mem_id;
    int rv;

    /*
     * Add excluded memories to the exclusion bitmap
     */
    for (; *excluded_list != ACCESS_INVALID_LOCAL_REGMEM_ID; ++excluded_list)
    {   /* iterate on the excluded memories */
        rv = access_regmem_global2local(runtime_info, *excluded_list, &local_mem_id);
        if (rv == _SHR_E_NONE)
        {
            SHR_BITSET(mems_bitmap, local_mem_id - device_info->nof_regs);      /* set the bits of excluded memories in 
                                                                                 * the bitmap */
        }
    }

}

/*
 * Function:
 *      access_init_mem_soc_dnx_mems_init
 * Purpose:
 *      zero all memories relevant for DNX.
 * Parameters:
 *      unit -  unit number
 *
 * Returns:
 *      SOC_E_XXX
 */
shr_error_e
access_init_mem_soc_dnx_mems_init(
    int unit)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    const access_regmem_id_t *dynamic_regs = NULL;

    /*
     * define bitmap for memory exclusions
     */
    uint32 *mems_bitmap = NULL;

    access_regmem_id_t dnx_tbls_excluded_mem_list[] = { /* memories which should be skipped when zeroing all memories */
        mSCH_SCHEDULER_INIT,    /* not a real memory, handled separately */
        /*
         * The HBM memories are not real memories
         */
        mBRDC_HBC_HBM_DRAM_CPU_ACCESS,
        mBRDC_FSRD_FSRD_PROM_MEM,
        mHBC_HBM_DRAM_CPU_ACCESS,

        /*
         * The {DDHA,DDHB,DHC}_MACRO_* memories are not real and do not need initialization
         */
        mDDHB_MACRO_0_ABK_BANK_A,
        mDDHB_MACRO_0_ABK_BANK_B,
        mDDHB_MACRO_0_ENTRY_BANK,
        mDDHB_MACRO_1_ABK_BANK_A,
        mDDHB_MACRO_1_ABK_BANK_B,
        mDDHB_MACRO_1_ENTRY_BANK,
        mDDHA_MACRO_0_ABK_BANK_A,
        mDDHA_MACRO_0_ABK_BANK_B,
        mDDHA_MACRO_0_ENTRY_BANK,
        mDDHA_MACRO_1_ABK_BANK_A,
        mDDHA_MACRO_1_ABK_BANK_B,
        mDDHA_MACRO_1_ENTRY_BANK,
        mDDHB_MACRO_2_ABK_BANK_A,
        mDDHB_MACRO_2_ABK_BANK_B,
        mDDHB_MACRO_2_ENTRY_BANK,
        mDDHA_MACRO_2_ABK_BANK_A,
        mDDHA_MACRO_2_ABK_BANK_B,
        mDDHA_MACRO_2_ENTRY_BANK,
        /*
         * Should not be initilized - Ofer Landau
         */
        mOAMP_RXP_R_MEP_INDEX_FIFO,

        /*
         * These are address spaces saved for ilkn memory mapped registers.
         */
        mILE_PORT_0_CPU_ACCESS,
        mILE_PORT_1_CPU_ACCESS,

        /*
         * All below are either not real memories or don't have protection. - Mosi Ravia
         */
        mKAPS_RPB_TCAM_CPU_COMMAND,
        mKAPS_TCAM_ECC_MEMORY,
        mMDB_ARM_KAPS_TCM,
        mMACT_CPU_REQUEST,
        mECI_SAM_CTRL,

        mSQM_PDM,

        /*
         * These memories are not real
         */
        mERPP_EPMFCS_TCAM_HIT_INDICATION,
        mIPPA_VTDCS_TCAM_HIT_INDICATION,
        mIPPA_VTECS_TCAM_HIT_INDICATION,
        mIPPB_FLPACS_TCAM_HIT_INDICATION,
        mIPPB_FLPBCS_TCAM_HIT_INDICATION,
        mIPPC_PMFACSA_TCAM_HIT_INDICATION,
        mIPPC_PMFACSB_TCAM_HIT_INDICATION,
        mIPPD_PMFBCS_TCAM_HIT_INDICATION,
        mIPPE_PRTCAM_TCAM_HIT_INDICATION,
        mIPPE_LLRCS_TCAM_HIT_INDICATION,
        mIPPF_VTACS_TCAM_HIT_INDICATION,
        mIPPF_VTCCS_TCAM_HIT_INDICATION,
        mIPPF_VTBCS_TCAM_HIT_INDICATION,
        mKAPS_RPB_TCAM_HIT_INDICATION,
        mKAPS_TCAM_HIT_INDICATION,

        /*
         * The ECGM memories are filled separately?
         */
        mECGM_FDCM,
        mECGM_FDCMAX,
        mECGM_FQSM,
        mECGM_FQSMAX,
        mECGM_PDCM,
        mECGM_PDCMAX,
        mECGM_PQSM,
        mECGM_PQSMAX,
        mECGM_QDCM,
        mECGM_QDCMAX,
        mECGM_QQSM,
        mECGM_QQSMAX,

        ACCESS_INVALID_LOCAL_REGMEM_ID  /* Has to be last memory in array */
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(mems_bitmap, SHR_BITALLOCSIZE(device_info->nof_mems),
                       "init memories bitmap", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * initialize memory zeroing exclusion to the given array.
     */
    access_init_mem_tbls_zero_init(unit, dnx_tbls_excluded_mem_list, mems_bitmap);

    /** enable dynamic memory writes */
    if (SOC_IS_J2P(unit))
    {
        dynamic_regs = access_init_mem_j2p_dynamic_mem_enable_regs;
    }
    else
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Device type is not supported%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    SHR_IF_ERR_EXIT(access_init_mem_dynamic_mem_writes(unit, dynamic_regs, 1));

    /*
     * initialize all mems to zero, except for exception array, and marked not to be zeroed.
     */
    SHR_IF_ERR_EXIT(access_init_mem_mems_zero(runtime_info, mems_bitmap));

    /** disable dynamic memory writes */
    if (SOC_IS_J2P(unit))
    {
        dynamic_regs = access_init_mem_j2p_dynamic_mem_disable_regs;
    }
    SHR_IF_ERR_EXIT(access_init_mem_dynamic_mem_writes(unit, dynamic_regs, 0));

exit:
    SHR_FUNC_EXIT;
}
#endif /* DNX_MEM_BRINGUP_DONE */

/**
 * \brief - build initial values access db for custom memories
 *
 * \param [in] unit - unit #
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
access_init_mem_sw_init_values(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
        const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                            * for the device type */
        access_mem_sw_init_values_t *mem_sw_init_value;
        const dnx_data_dev_init_mem_sw_mem_init_t *mem_init_info;
        const dnxc_data_table_info_t *table_info;
        int mem_index;
        access_regmem_id_t global_mem_id;
        access_local_regmem_id_t local_mem_id;
        access_field_id_t global_field_id;
        access_local_field_id_t local_field_id;

        /** Get sw mem init table pointer */
        table_info = dnx_data_dev_init.mem.sw_mem_init_info_get(unit);
        /*
        * Iterate over the DnxData table and set value for each mem
        * according to sw memory init mode.
        */
        for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
        {
            mem_init_info = dnx_data_dev_init.mem.sw_mem_init_get(unit, mem_index);
            global_mem_id = (unsigned) mem_init_info->mem;
            global_field_id = mem_init_info->field;

            /*
            * get the local device info for calling the local device function 
            */
            SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, global_mem_id, &local_mem_id));

            if (global_field_id != ACCESS_INVALID_FIELD_ID)
            {
                SHR_IF_ERR_EXIT(access_field_global2local(runtime_info, global_field_id, &local_field_id));
            }
            else
            {
                local_field_id = ACCESS_INVALID_LOCAL_FIELD_ID;
            }

            mem_sw_init_value = &runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs];
            mem_sw_init_value->init_mode = mem_init_info->mode;

            /** per mode action */
            switch (mem_init_info->mode)
            {
                /** Do not init memory */
                case dnx_sw_init_mem_mode_none:
                {
                    break;
                }

                /** Custom per entry, expected a callback to provide the init value. */
                case dnx_sw_init_mem_mode_custom_per_entry:
                {
                    /*
                    * verify, callback supplied
                    */
                    if (mem_init_info->sw_mem_init_get_cb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "custom per entry sw init value requires callback (mem_index %d).\n",
                                    mem_index);
                    }
                    mem_sw_init_value->u.per_entry_cb = mem_init_info->sw_mem_init_get_cb;
                    mem_sw_init_value->local_field_id = local_field_id;
                    break;
                }

                /** Custom, expected a callback to provide the init value. */
                case dnx_sw_init_mem_mode_custom_fill_value:
                {
                    /*
                     * verify, callback supplied
                     */
                    if (mem_init_info->sw_mem_init_get_fill_custom_value_cb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "custom sw init value requires fill callback (mem_index %d).\n",
                                    mem_index);
                    }

                    SHR_IF_ERR_EXIT(mem_init_info->sw_mem_init_get_fill_custom_value_cb(
                        runtime_info, &mem_sw_init_value->u.custom_init_value));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", mem_init_info->mode);
                    break;
                }

            }
        }
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Init (set software init values) for all valid writable memories.
 * This function use indirect access or sbus/SLAM DMA to efficiently write to memories.
 * If a memory appears in table dnx_data_dev_init.mem.sw_mem_init, the value specification from the table is used.
 * Otherwise the memory will be set to 0.
 *
 * In a case a memory initialized manually - memory mode should be set to 'skip'.
 * In a case a memory should be set to 0 - done automatically (no need to add to dnx data table)
 * Otherwise a memory should be add to dnx data table (instructions below)
 *
 * Defining a non-zero software init value for a memory:
 * ----------------------------------
 * 1. Go to device xml in dnx data (module='dev_init', sub_module='mem', table='sw_mem_init')
 * 2. Increase table size by 1
 * 3. Add a new entry:
 *        'index' - is just a running index.
 *        'mem' - is memory define
 *        'mode' - is one of the modes defined in enum 'dnx_sw_init_mem_mode_e'
 *        'sw_mem_init_get_cb' - is optional (set to NULL if not required).
 *                               A callback to get the sw init value data for custom per entry initialization.
 *        'sw_mem_init_get_fill_custom_value_cb' - is optional (set to NULL if not required).
 *                               A callback to get the sw init value data for custom fill value initialization.
 *         'field' - set just specific field (all the reset will be set to 0) 
 * 4. Run dnxdata autocoder
 *
 * \param [in] unit - unit #
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
access_init_mems(
    int unit)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    access_mem_sw_init_values_t *mem_sw_init_value;
    int need_sch_config = 0;
    int disable_hard_reset = 0x0;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 *mem_exclude_bitmap = NULL;
    const dnx_data_dev_init_mem_sw_mem_init_t *mem_init_info;
    const dnxc_data_table_info_t *table_info;
    dnx_init_mem_reset_mode_e reset_mode;
    int mem_index;
    access_regmem_id_t global_mem_id;
    access_local_regmem_id_t local_mem_id;
    const access_regmem_id_t *dynamic_regs = NULL;

#ifdef PRINT_MEM_INIT_TIME
    sal_usecs_t start_usecs = sal_time_usecs(), prev_usecs = start_usecs, usecs, passed_secs, last_time;
#endif

    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);

    /** handle emulation and simulation tables init */
    if (SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit))
    {
        SHR_IF_ERR_EXIT(access_init_mem_sim_init(unit));
    }

    if (disable_hard_reset == 1)
    {
        uint32 field_val = 1;
        SHR_IF_ERR_EXIT(access_regmem_field_modify
                        (unit, 0, rSCH_SCHEDULER_CONFIGURATION_REGISTER, ACCESS_ALL_BLOCK_INSTANCES, 0, 0,
                         rSCH_SCHEDULER_CONFIGURATION_REGISTER_fDISABLE_FABRIC_MSGS, &field_val));
        need_sch_config = 1;
    }

#ifndef DNX_MEM_BRINGUP_DONE
    SHR_IF_ERR_EXIT(access_init_mem_soc_dnx_tbls_init(unit));
#endif /* DNX_MEM_BRINGUP_DONE */

    /** Allocate exclude bitmap */
    SHR_ALLOC_SET_ZERO(mem_exclude_bitmap, SHR_BITALLOCSIZE(device_info->nof_regs + device_info->nof_mems),
                       "reset memories exclude bitmap", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get sw mem init table pointer */
    table_info = dnx_data_dev_init.mem.sw_mem_init_info_get(unit);

    /** enable dynamic memory access */
    if (SOC_IS_J2P(unit))
    {
        dynamic_regs = access_init_mem_j2p_dynamic_mem_enable_regs;
    }
    else
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Device type is not supported%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    SHR_IF_ERR_EXIT(access_init_mem_dynamic_mem_writes(unit, dynamic_regs, 1));

    /*
     * Iterate over the DnxData table and set value for each mem
     * according to sw memory init mode.
     */
    for (mem_index = 0; mem_index < table_info->key_size[0]; mem_index++)
    {
        mem_init_info = dnx_data_dev_init.mem.sw_mem_init_get(unit, mem_index);
        global_mem_id = (unsigned) mem_init_info->mem;
        /*
         * get the local device info for calling the local device function 
         */
        SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, global_mem_id, &local_mem_id));

        mem_sw_init_value = &runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs];

        /** Init vars */
        sal_memset(entry, 0x0, sizeof(entry));

        if (SHR_BITGET(mem_exclude_bitmap, local_mem_id))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Memory %s initialized twice in dnx_data_dev_init.mem.sw_mem_init .\n",
                         ACCESS_REGMEM_NAME(global_mem_id));
        }

        /** per mode action */
        switch (mem_init_info->mode)
        {
            /** Do not init memory */
            case dnx_sw_init_mem_mode_none:
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Skipping memory # %d - %s\n"), local_mem_id,
                             ACCESS_REGMEM_NAME(global_mem_id)));
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, (local_mem_id - device_info->nof_regs));
                break;
            }

            /** Custom per entry, expected a callback to provide the init value. */
            case dnx_sw_init_mem_mode_custom_per_entry:
            {
#ifdef PRINT_MEM_INIT_TIME
                if (soc_sand_is_emulation_system(unit))
                {
                    passed_secs = ((usecs = sal_time_usecs()) - start_usecs) / 1000000;
                    last_time = (usecs - prev_usecs) / 100000;
                    prev_usecs = usecs;
                    LOG_ERROR(BSL_LOG_MODULE, ("%d:%.2d after %d.%ds Custom per entry memory %s\n",
                                               passed_secs / 60, passed_secs % 60, last_time / 10, last_time % 10,
                                               access_global_names[global_regmem_name_indices[global_mem_id]]));
                }
                else
#endif
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Custom per entry memory # %d - %s\n"), local_mem_id,
                                 access_global_names[global_regmem_name_indices[global_mem_id]]));
                /**set data */
                SHR_IF_ERR_EXIT(access_init_custom_mem_per_entry
                                (runtime_info, local_mem_id,
                                 mem_sw_init_value->local_field_id, mem_sw_init_value->u.per_entry_cb));
                /** mark this map as excluded (will be already handled) */
                SHR_BITSET(mem_exclude_bitmap, (local_mem_id - device_info->nof_regs));
                break;
            }

            /** Custom, expected a callback to provide the init value. */
            case dnx_sw_init_mem_mode_custom_fill_value:
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Custom memory fill# %d - %s\n"), local_mem_id,
                             access_global_names[global_regmem_name_indices[global_mem_id]]));

                 if (SAL_BOOT_PLISIM)
                {
                    /** for simulation - write custom memories */
                    SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                                    (unit,
                                     access_init_mem_fill_memory_with_entry(runtime_info, local_mem_id,
                                                                            ACCESS_ALL_BLOCK_INSTANCES,
                                                                            mem_sw_init_value->u.custom_init_value)));
                }
                break;
            }

            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", mem_init_info->mode);
                break;
            }

        }
    }

    if (!SAL_BOOT_PLISIM)
    {
        {
            /** Reset the TCAM CS tables */
            SHR_IF_ERR_EXIT(access_init_mem_tcam_cs_reset(unit));
        }
    }
    /*
     * Reset the rest of the tables to zero
     */
    /** get reset mode from dnx data */
    reset_mode = dnx_data_dev_init.mem.reset_mode_get(unit);

    /*
     * for simulation - avoid from reset memories - assuming all memories already initilized to zero
     */
    if (SAL_BOOT_PLISIM && !dnx_data_dev_init.mem.force_zeros_get(unit))
    {
        reset_mode = dnx_init_mem_reset_mode_none;
    }

    switch (reset_mode)
    {
        case dnx_init_mem_reset_mode_indirect:
            SHR_IF_ERR_EXIT(access_init_mems_using_indirect(runtime_info, mem_exclude_bitmap));
            break;
        case dnx_init_mem_reset_mode_dma:
            SHR_IF_ERR_EXIT(access_init_mems_using_dma(unit, mem_exclude_bitmap, 0));
            break;
        case dnx_init_mem_reset_mode_none:
            /*
             * do not perform reset
             */
            
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "reset mode not supported %d.\n", reset_mode);
            break;

    }

    /*
     * even that memories are not zeroed from the SDK,
     * update the cache to optimize first read
    */
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_IF_ERR_EXIT(access_init_mem_reset_cache_update(unit, mem_exclude_bitmap));
    }
    
    /** disable dynamic memory access */
    if (SOC_IS_J2P(unit))
    {
        dynamic_regs = access_init_mem_j2p_dynamic_mem_disable_regs;
    }
    SHR_IF_ERR_EXIT(access_init_mem_dynamic_mem_writes(unit, dynamic_regs, 0));

    /** Enable required dynamic memories */
    if (SOC_IS_J2P(unit))
    {
        uint32 value = 1;
        SHR_IF_ERR_EXIT(DNXC_ACCESS_SET_DYN_MEM_ACCESS
                        (unit, EVNT, TRANSACTION_ENGINE_BIT_MAPPING, ACCESS_ALL_BLOCK_INSTANCES, &value));
        SHR_IF_ERR_EXIT(DNXC_ACCESS_SET_DYN_MEM_ACCESS
                        (unit, EVNT, TX_EVENT_ID_TO_CONTEXT_ID_MAP, ACCESS_ALL_BLOCK_INSTANCES, &value));
        SHR_IF_ERR_EXIT(DNXC_ACCESS_SET_DYN_MEM_ACCESS
                        (unit, EVNT, CONTEXT_PROPERTIES_0, ACCESS_ALL_BLOCK_INSTANCES, &value));
        SHR_IF_ERR_EXIT(DNXC_ACCESS_SET_DYN_MEM_ACCESS
                        (unit, EVNT, CONTEXT_PROPERTIES_1, ACCESS_ALL_BLOCK_INSTANCES, &value));
    }

exit:
    if (need_sch_config == 1)
    {
        SHR_IF_ERR_CONT(access_regmem_field_modify
                        (unit, 0, rSCH_SCHEDULER_CONFIGURATION_REGISTER, ACCESS_ALL_BLOCK_INSTANCES, 0, 0,
                         rSCH_SCHEDULER_CONFIGURATION_REGISTER_fDISABLE_FABRIC_MSGS, 0x0));
    }
    if (mem_exclude_bitmap != NULL)
    {
        sal_free(mem_exclude_bitmap);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - returns none zero in is_custom if the memory has a software init value (appears on in the XML table and is not dnx_sw_init_mem_mode_none).
 * \param [in] unit - unit #
 * \param [in] mem - local mem id
 * \param [in] is_custom - indicate non standard
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mem_sw_init_is_custom(
    int unit,
    access_regmem_id_t global_mem_id,
    int *is_custom)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    access_local_regmem_id_t local_mem_id;
    dnx_sw_init_mem_mode_e init_mode;

    SHR_FUNC_INIT_VARS(unit);

    (*is_custom) = 0;

    /** Code to support new access on J2P where DBAL uses old access */
    if (SOC_IS_J2P(unit))
    {
        access_regmem_id_t access_mem_id = /* get the global regmem ID of the memory */
          (unsigned)global_mem_id >= ACCESS_NOF_MCM_MAPPED_MEMS ? ACCESS_INVALID_REGMEM_ID : access_mcm2access_mem_ids[(unsigned)global_mem_id];
        if (access_mem_id == ACCESS_INVALID_REGMEM_ID || !SOC_MEM_IS_VALID(unit, global_mem_id)) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "memory %d is not valid.\n", global_mem_id);
        }
        global_mem_id = access_mem_id;
    }

    SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, global_mem_id, &local_mem_id));
    init_mode = runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs].init_mode;

    /** per mode action */
    switch (init_mode)
    {
        /** memory should not be initialized return is_custom = 0 */
        case dnx_sw_init_mem_mode_none:
        case dnx_sw_init_mem_mode_zero:
        {
            break;
        }
        /** memory should not be initialized to special value (none zero) return is_custom = 1 */
        case dnx_sw_init_mem_mode_custom_fill_value:
        case dnx_sw_init_mem_mode_custom_per_entry:
        {
            (*is_custom) = 1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", init_mode);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the software init value for the memory. If the returned mode is dnx_sw_init_mem_mode_none then the memory does not have a software init value.
 *
 * \param [in] unit - unit #
 * \param [in] global_mem_id - required global memory id
 * \param [in] array_index - the array index
 * \param [in] index - the mem index
 * \param [out] entry - the entry value
 * \param [out] mode  - the initialization mode
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_mem_sw_init_value_get(
    int unit,
    access_regmem_id_t global_mem_id,
    int array_index,
    int index,
    uint32 *entry,
    dnx_sw_init_mem_mode_e * mode)
{
    access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);    /* Access information * for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info;      /* constant access information
                                                                                         * for the device type */
    access_local_regmem_id_t local_mem_id;
    const access_device_regmem_t *rm_info;
    dnx_sw_init_mem_info mem_info = { ACCESS_INVALID_LOCAL_REGMEM_ID, ACCESS_INVALID_LOCAL_FIELD_ID };
    dnx_sw_init_mem_mode_e sw_init_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Code to support new access on J2P where DBAL uses old access */
    if (SOC_IS_J2P(unit))
    {
        access_regmem_id_t access_mem_id = /* get the global regmem ID of the memory */
          (unsigned)global_mem_id >= ACCESS_NOF_MCM_MAPPED_MEMS ? ACCESS_INVALID_REGMEM_ID : access_mcm2access_mem_ids[(unsigned)global_mem_id];
        if (access_mem_id == ACCESS_INVALID_REGMEM_ID || !SOC_MEM_IS_VALID(unit, global_mem_id)) {
            SHR_ERR_EXIT(_SHR_E_PARAM, "memory %d is not valid.\n", global_mem_id);
        }
        global_mem_id = access_mem_id;
    }

    SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, global_mem_id, &local_mem_id));
    rm_info = device_info->local_regs + local_mem_id;

    sw_init_mode = runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs].init_mode;
    if (mode != NULL)
    {
        *mode = sw_init_mode;
    }

    /** per mode action */
    switch (sw_init_mode)
    {
        /** return pointer to init value */
        case dnx_sw_init_mem_mode_none:
        case dnx_sw_init_mem_mode_zero:
        {
            /** set entry to zero */
            sal_memset(entry, 0, ACESSS_REGMEM_T_SBUS_WIDTH_IN_BYTES(rm_info));
            break;
        }

        case dnx_sw_init_mem_mode_custom_fill_value:
        {
            sal_memcpy(entry, runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs].u.custom_init_value, ACESSS_REGMEM_T_SBUS_WIDTH_IN_BYTES(rm_info));
            break;
        }

        /** Custom per entry, expected a callback to provide the init value. */
        case dnx_sw_init_mem_mode_custom_per_entry:
        {
            mem_info.mem = local_mem_id;
            mem_info.field_id = runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs].local_field_id;
            /**set data */
            SHR_IF_ERR_EXIT(runtime_info->mem_sw_init_values[local_mem_id - device_info->nof_regs].u.per_entry_cb
                            (runtime_info, &mem_info, array_index, index, entry));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "mode not supported %d.\n", *mode);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#else
const int __keep_pedantic_compilation_happy = 0;
#endif /* BCM_ACCESS_SUPPORT */
