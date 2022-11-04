
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC SAND MEM
 */
 
#include <shared/bsl.h>
#define BSL_LOG_MODULE BSL_LS_SOC_MEM
#include <soc/sand/shrextend/shrextend_debug.h>

#include <soc/sand/sand_mem.h>
#include <soc/drv.h>

#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#endif 

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#ifdef BCM_88950_A0
#include <soc/dfe/fe3200/fe3200_fabric_cell.h>
#endif
#endif 

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#endif 

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_cell.h>
#endif 

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_mem.h>
#endif

#include <soc/memory.h>
#include <soc/mem.h>
#include <soc/sand/sand_aux_access.h>

#define MAX_ENTRIES_IN_WRITE (16 * 1024 - 1)

#ifdef MEASURE_INDIRECT_ACCESS
    sal_mutex_t indirect_access_summary_mutex[SOC_MAX_NUM_DEVICES];
    sal_usecs_t indirect_access_min_time[SOC_MAX_NUM_DEVICES], indirect_access_max_time[SOC_MAX_NUM_DEVICES];
    uint32 indirect_access_nof_operations[SOC_MAX_NUM_DEVICES];
    uint64 indirect_access_total_time[SOC_MAX_NUM_DEVICES];
    uint32 indirect_access_min_waits[SOC_MAX_NUM_DEVICES], indirect_access_max_waits[SOC_MAX_NUM_DEVICES],
           indirect_access_total_waits[SOC_MAX_NUM_DEVICES], indirect_access_nof_writes[SOC_MAX_NUM_DEVICES];
#endif


uint32 sand_alloc_mem(
    const int unit,
    void      *mem_ptr,        
    const unsigned size,       
    const char     *alloc_name 
)
{
    SHR_FUNC_INIT_VARS(unit);
    {
        {
            if (mem_ptr == NULL || alloc_name == NULL)
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
            }
            else if (*(void**)mem_ptr != NULL)
            {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "memory pointer value is not NULL, when attempted to allocate %s%s%s\n", alloc_name, EMPTY, EMPTY);
            }
        }
        if ((*(void**)mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL)
        {
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes for %s%s\n", size, alloc_name, EMPTY);
        }
    }
    sal_memset(*(void**)mem_ptr, 0, size); 
exit:
    SHR_FUNC_EXIT;
}


uint32 sand_free_mem(
    const int unit,
    void **mem_ptr 
)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mem_ptr, _SHR_E_PARAM, "mem_ptr");
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    sal_free(*mem_ptr);
    *mem_ptr = NULL;
exit:
    SHR_FUNC_EXIT;
}


void sand_free_mem_if_not_null(
    const int unit,
    void **mem_ptr 
)
{
    if (mem_ptr != NULL && *mem_ptr != NULL) {
        sal_free(*mem_ptr);
        *mem_ptr = NULL;
    }
}




uint32 sand_alloc_dma_mem(
    const int unit,
    const uint8     is_slam,    
    void            **mem_ptr,  
    const unsigned  size,       
    const char      *alloc_name 
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (mem_ptr == NULL || alloc_name == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (*mem_ptr != NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "memory pointer value is not NULL, when attempted to allocate %s%s%s\n", alloc_name, EMPTY, EMPTY);
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { 
        if ((*mem_ptr = soc_cm_salloc(unit, size, alloc_name)) == NULL){
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes of DMA memory for %s%s\n", size, alloc_name, EMPTY);
        }
    } else {
        if ((*mem_ptr = sal_alloc(size, (char*)alloc_name)) == NULL){
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Failed to allocate %u bytes of memory for %s%s\n", size, alloc_name, EMPTY);
        }
    }
    sal_memset(*mem_ptr, 0, size); 
exit:
    SHR_FUNC_EXIT;
}


uint32 sand_free_dma_mem(
    const int   unit,
    const uint8 is_slam,  
    void        **mem_ptr 
)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mem_ptr, _SHR_E_PARAM, "mem_ptr");
    if (mem_ptr == NULL || *mem_ptr == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null parameter %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    if (is_slam == FALSE ? soc_mem_dmaable(unit, 0, 0) : soc_mem_slamable(unit, 0, 0)) { 
        soc_cm_sfree(unit, *mem_ptr);
    } else {
        sal_free(*mem_ptr);
    }
    *mem_ptr = NULL;
exit:
    SHR_FUNC_EXIT;
}





#define INDIRECT_COMMAND_BLOCKS_OLD_DEF \
               SOC_BLK_CGM: \
          case SOC_BLK_BDM: \
          case SOC_BLK_DQM: \
          case SOC_BLK_PDM: \
          case SOC_BLK_IPS: \
          case SOC_BLK_IQM: \
          case SOC_BLK_SQM: \
          case SOC_BLK_FDT

#define INDIRECT_COMMAND_BLOCKS_PARTIAL_OLD_DEF \
               SOC_BLK_FLEXMAC: \
          case SOC_BLK_MACT: \
          case SOC_BLK_OLP: \
          case SOC_BLK_OCBM


#define ADDR_IND_CMD 0x80




int sand_indirect_command_reg_get(int unit, int soc_blk_inst_id, uint64 *data)
{
#ifdef PLISIM
    *data = 0;
    return _SHR_E_NONE;
#else
    int needs_special_treatment = SOC_IS_J2X(unit);
    uint32 reg_width_in_uint32s = 1;
    uint32 uint32_data[2] = {0};
    soc_block_info_t *blk_inst_info = &SOC_BLOCK_INFO(unit, soc_blk_inst_id);
    SHR_FUNC_INIT_VARS(unit);

    if (needs_special_treatment) {
        switch (blk_inst_info->type) { 
          case INDIRECT_COMMAND_BLOCKS_OLD_DEF:
          case INDIRECT_COMMAND_BLOCKS_PARTIAL_OLD_DEF:
            break;
          default:
            reg_width_in_uint32s = 2;
            needs_special_treatment = 0;
        }
    }

    SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, blk_inst_info->cmic, ADDR_IND_CMD, reg_width_in_uint32s, uint32_data));
    if (needs_special_treatment) {
        *data = (uint64)(uint32_data[0] & 0xffff) | ((uint64)(uint32_data[0] & 0xffff0000) << 4);
    } else {
        COMPILER_64_SET(*data, uint32_data[1], uint32_data[0]);
    }

exit:
    SHR_FUNC_EXIT;
#endif 
}


int sand_indirect_command_reg_set(int unit, int soc_blk_inst_id, uint64 data)
{
#ifdef PLISIM
    return _SHR_E_NONE;
#else
    uint32 reg_width_in_uint32s = 1;
    uint32 uint32_data[2] = { COMPILER_64_LO(data), COMPILER_64_HI(data)};
    soc_block_info_t *blk_inst_info = &SOC_BLOCK_INFO(unit, soc_blk_inst_id);
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_J2X(unit)) {
        switch (blk_inst_info->type) { 
          case INDIRECT_COMMAND_BLOCKS_OLD_DEF:
            uint32_data[0] = (data & 0xffff) | ((data & COMPILER_64_LITERAL(0xffff00000)) >> 4);
            break;
          default:
            reg_width_in_uint32s = 2;
            uint32_data[0] = COMPILER_64_LO(data);
            uint32_data[1] = COMPILER_64_HI(data);
        }
    }

    SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, blk_inst_info->cmic, ADDR_IND_CMD, reg_width_in_uint32s, uint32_data));

exit:
    SHR_FUNC_EXIT;
#endif 
}





void *sand_dma_buffers[SOC_MAX_NUM_DEVICES] = {0};
volatile sal_mutex_t sand_dma_buf_mutexes[SOC_MAX_NUM_DEVICES] = {0};


uint32 sand_init_fill_table(
    const  int unit
)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sand_dma_buf_mutexes[unit]==0 && soc_mem_slamable(unit, 0, 0)) { 
        SHR_IF_ERR_EXIT(sand_alloc_dma_mem(unit, TRUE, sand_dma_buffers + unit, SAND_MAX_U32S_IN_MEM_ENTRY * sizeof(uint32), "fill_table"));
        sand_dma_buf_mutexes[unit] = sal_mutex_create("sand_fill_table");
    }
exit:
    SHR_FUNC_EXIT;
}


uint32 sand_deinit_fill_table(
    const  int unit
)
{
    sal_mutex_t mutex = sand_dma_buf_mutexes[unit];
    SHR_FUNC_INIT_VARS(unit);
    if (sand_dma_buffers[unit] != NULL) {
        SHR_IF_ERR_EXIT(sal_mutex_take(mutex, sal_mutex_FOREVER));
        SHR_IF_ERR_EXIT(sand_free_dma_mem(unit, TRUE, sand_dma_buffers + unit));
        sand_dma_buf_mutexes[unit] = NULL;
        SHR_IF_ERR_EXIT(sal_mutex_give(mutex));
        sal_mutex_destroy(mutex);
    }
exit:
    SHR_FUNC_EXIT;
}


uint32 sand_fill_table_with_entry(
    const int       unit,
    const soc_mem_t mem,        
    const int       copyno,     
    const void      *entry_data 
  )
{
    int should_release = 0;
    void *buffer = sand_dma_buffers[unit];
#if defined BCM_DPP_SUPPORT && defined(PALLADIUM_BACKDOOR)
    int mem_size;
#endif
#ifdef DCMN_RUNTIME_DEBUG
    sal_time_t start_time = sal_time();
    unsigned unsigned_i;
#endif 

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DPP_SUPPORT
#ifndef PALLADIUM_BACKDOOR
#ifndef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
    if (mem == IRR_MCDBm && SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->emulation_system) {
        goto fast_exit;
    }
#endif 
#else 
    mem_size = SOC_MEM_INFO(unit, mem).index_max - SOC_MEM_INFO(unit, mem).index_min +1;

    
    if ((SOC_IS_JERICHO(unit)) && !SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->emulation_system && (mem_size > 256) && !SOC_IS_JERICHO_PLUS(unit)
#ifdef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
      && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "no_backdoor", 0) != 1
#endif
      ) {
        int array_mem_idx;
        char array_mem_name[1024];
        soc_error_t rv;
#ifdef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
        soc_mem_t exceptions[] = {
            FDT_IPT_MESH_MCm,
            EPNI_TX_TAG_TABLEm,
            PPDB_A_FEC_SUPER_ENTRY_BANKm
        };
        int exception_idx;
        const int nof_exceptions = sizeof(exceptions) / sizeof(exceptions[0]);

        
        for (exception_idx = 0; exception_idx < nof_exceptions; exception_idx++) {
            if (mem == exceptions[exception_idx]) {
                break;
            }
        }
#endif 

        
        if (
#ifndef JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR
          mem == IRR_MCDBm || mem == EDB_EEDB_BANKm || mem == IHB_DESTINATION_STATUSm
#else
          exception_idx == nof_exceptions
#endif
          ) {
            
            if (SOC_MEM_IS_ARRAY(unit, mem))
            {
                
                for (array_mem_idx = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem); array_mem_idx < SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem); array_mem_idx++)
                {
                    sal_sprintf(array_mem_name, "%s%d", SOC_MEM_NAME(unit, mem), array_mem_idx);
                    rv = _arad_palladium_backdoor_dispatch_full_table_write(unit, array_mem_name, entry_data, soc_mem_entry_words(unit, mem));
                    SHR_IF_ERR_EXIT(rv);
                }
            } else {
                rv = _arad_palladium_backdoor_dispatch_full_table_write(unit, SOC_MEM_NAME(unit, mem), entry_data, soc_mem_entry_words(unit, mem));
                SHR_IF_ERR_EXIT(rv);
            }
            
            SOC_EXIT;
        }
    }
#endif 
#endif 

   if (entry_data == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (!soc_mem_is_valid(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Invalid memory for unit %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if ((buffer != NULL) && (soc_mem_entry_words(unit, mem) <= SAND_MAX_U32S_IN_MEM_ENTRY)) {
        SHR_IF_ERR_EXIT(sal_mutex_take(sand_dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SHR_IF_ERR_EXIT(soc_mem_fill(unit, mem, copyno, buffer));
    
exit:
    if (should_release && sal_mutex_give(sand_dma_buf_mutexes[unit])) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Mutex give failed\n")));
        _func_rv = SOC_E_FAIL;
    }
#ifdef DCMN_RUNTIME_DEBUG
    if (((unsigned_i = sal_time() - start_time)) >= 5) {
        LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_table_with_entry(%s) ran for %u:%2.2u\n",
          SOC_MEM_NAME(unit, mem), unsigned_i / 60, unsigned_i % 60));
    }
    if (sand_runtime_debug_per_device[unit].run_stage <= sand_runtime_debug_state_initializing) {
        for (unsigned_i = 0; unsigned_i < soc_mem_entry_words(unit, mem); ++unsigned_i) {
            if (((const uint32*)entry_data)[unsigned_i]) { 
                break;
            }
        }
        if (unsigned_i >= soc_mem_entry_words(unit, mem)) {
            LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_table_with_entry(%s) received a zero entry\n", SOC_MEM_NAME(unit, mem)));
        }
    }
#endif 
#if defined(BCM_DPP_SUPPORT) && !defined(PALLADIUM_BACKDOOR) && !defined(JERICHO_EMULATION_OLD_ACCELERATION_BEHAVIOR)
    fast_exit:
#endif
    SHR_FUNC_EXIT;
}


uint32 sand_fill_table_with_entry_skip_emul_err(
    const int       unit,
    const soc_mem_t mem,        
    const int       copyno,     
    const void      *entry_data 
  )
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

#if defined(INCLUDE_CPU_I2C)
    
    if (SAL_BOOT_I2C_ACCESS) {
        rv = sand_fill_mem_indirect_reset_write(unit, mem, copyno, 0, 0, 0, -1, entry_data);
    } else
#endif
    {
        rv = sand_fill_table_with_entry(unit, mem, copyno, entry_data); 
    }

    
    if (!soc_sand_is_emulation_system(unit))
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}


uint32 sand_fill_partial_table_with_entry(
    const int       unit,
    const soc_mem_t mem,               
    const unsigned  array_index_start, 
    const unsigned  array_index_end,   
    const int       copyno,            
    const int       index_start,       
    const int       index_end,         
    const void      *entry_data        
  )
{
    int should_release = 0;
    void *buffer = sand_dma_buffers[unit];
#ifdef DCMN_RUNTIME_DEBUG
    sal_time_t start_time = sal_time();
    unsigned unsigned_i;
#endif 
    SHR_FUNC_INIT_VARS(unit);
    if (entry_data == NULL) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "null buffer %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (!soc_mem_is_valid(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Invalid memory for unit %s%s%s\n", EMPTY, EMPTY, EMPTY);
    } else if (buffer != NULL) {
        if (soc_mem_entry_words(unit, mem) > SAND_MAX_U32S_IN_MEM_ENTRY) {
            SHR_EXIT_WITH_LOG(SOC_E_MEMORY, "Memory entry is too big for the operation %s%s%s\n", EMPTY, EMPTY, EMPTY);
        }
        SHR_IF_ERR_EXIT(sal_mutex_take(sand_dma_buf_mutexes[unit], sal_mutex_FOREVER));
        should_release = 1;
        sal_memcpy(buffer, entry_data, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    } else {
        buffer = (void*)entry_data;
    }
    SHR_IF_ERR_EXIT(soc_mem_array_fill_range(unit, mem, array_index_start, array_index_end, copyno, index_start, index_end, buffer));
exit:
    if (should_release && sal_mutex_give(sand_dma_buf_mutexes[unit])) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Mutex give failed\n")));
        _func_rv = SOC_E_FAIL;
    }
#ifdef DCMN_RUNTIME_DEBUG
    if (((unsigned_i = sal_time() - start_time)) >= 5) {
        LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_partial_table_with_entry(%s) ran for %u:%2.2u\n",
          SOC_MEM_NAME(unit, mem), unsigned_i / 60, unsigned_i % 60));
    }
    if (sand_runtime_debug_per_device[unit].run_stage <= sand_runtime_debug_state_initializing) {
        for (unsigned_i = 0; unsigned_i < soc_mem_entry_words(unit, mem); ++unsigned_i) {
            if (((const uint32*)entry_data)[unsigned_i]) { 
                break;
            }
        }
        if (unsigned_i >= soc_mem_entry_words(unit, mem)) {
            LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_partial_table_with_entry(%s) received a zero entry\n", SOC_MEM_NAME(unit, mem)));
        }
    }
#endif 
    SHR_FUNC_EXIT;
}


int soc_sand_indirect_mem_access_init(int unit)
{
    int i;
    sal_mutex_t *block_inst_indirect_access_mutexes; 
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(sand_alloc_mem(unit, &SOC_CONTROL(unit)->block_inst_indirect_access_mutexes,
                    sizeof(sal_mutex_t) * SOC_DRIVER(unit)->nof_instances_per_device,
                    "blk_inst_indirect_access_locks"));
    block_inst_indirect_access_mutexes = SOC_CONTROL(unit)->block_inst_indirect_access_mutexes;
    for (i = SOC_DRIVER(unit)->nof_instances_per_device; i ; --i, ++block_inst_indirect_access_mutexes) {
        *block_inst_indirect_access_mutexes = sal_mutex_create("blk_inst_indirect_access");
        if (*block_inst_indirect_access_mutexes == NULL) {
            SHR_ERR_EXIT(SOC_E_FAIL, "Failed to create mutexes for indirect memory access\n");
        }
    }
#ifdef MEASURE_INDIRECT_ACCESS
    block_inst_indirect_access_mutexes = indirect_access_summary_mutex + unit;
    if (!*block_inst_indirect_access_mutexes) {
        *block_inst_indirect_access_mutexes = sal_mutex_create("indirect_access_summary_mutex");
        if (*block_inst_indirect_access_mutexes == NULL) {
            SHR_ERR_EXIT(SOC_E_FAIL, "Failed to create mutex for indirect memory access measurement summary\n");
        }
    }
    indirect_access_min_time[unit] = -1;
    indirect_access_max_time[unit] = 0;
    indirect_access_nof_operations[unit] = 0;
    indirect_access_total_time[unit] = 0;
    indirect_access_min_waits[unit] = -1; indirect_access_max_waits[unit] = indirect_access_total_waits[unit] = indirect_access_nof_writes[unit] =0;
#endif

exit:
    SHR_FUNC_EXIT;
}


int soc_sand_indirect_mem_access_deinit(int unit)
{
    int i;
    sal_mutex_t *block_inst_indirect_access_mutexes; 
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_CONTROL(unit) &&
        (block_inst_indirect_access_mutexes = SOC_CONTROL(unit)->block_inst_indirect_access_mutexes) != NULL) {
        for (i = SOC_DRIVER(unit)->nof_instances_per_device; i ; --i, ++block_inst_indirect_access_mutexes) {
            if (*block_inst_indirect_access_mutexes != NULL) {
                sal_mutex_destroy(*block_inst_indirect_access_mutexes);
            }
        }
        SHR_IF_ERR_EXIT(sand_free_mem(unit, (void**)&SOC_CONTROL(unit)->block_inst_indirect_access_mutexes));
    }
#ifdef MEASURE_INDIRECT_ACCESS
    block_inst_indirect_access_mutexes = indirect_access_summary_mutex + unit;
    if (*block_inst_indirect_access_mutexes) {
        SHR_IF_ERR_EXIT(sal_mutex_take(indirect_access_summary_mutex[unit], sal_mutex_FOREVER));
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "indirect access summary: %u operations in us: min=%u avg="
          COMPILER_64_PRINTF_FORMAT " max=%u  writes:%u waits: min=%u avg=%u max=%u\n"),
          indirect_access_nof_operations[unit], indirect_access_min_time[unit],
          (indirect_access_total_time[unit] + indirect_access_nof_operations[unit] / 2) / indirect_access_nof_operations[unit],
          indirect_access_max_time[unit],
          indirect_access_nof_writes[unit], indirect_access_min_waits[unit],
          (indirect_access_total_waits[unit] + indirect_access_nof_writes[unit] / 2) / indirect_access_nof_writes[unit],
          indirect_access_max_waits[unit]));
          sal_mutex_destroy(*block_inst_indirect_access_mutexes);
        *block_inst_indirect_access_mutexes = NULL;
    }
#endif

exit:
    SHR_FUNC_EXIT;
}


int soc_sand_indirect_mem_access_block_instance_lock(
    const int unit,
    int blk)
{
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(sal_mutex_take(SOC_CONTROL(unit)->block_inst_indirect_access_mutexes[blk], sal_mutex_FOREVER));

exit:
    SHR_FUNC_EXIT;
}


int soc_sand_indirect_mem_access_block_instance_unlock(
    const int unit,
    int blk)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(sal_mutex_give(SOC_CONTROL(unit)->block_inst_indirect_access_mutexes[blk]));
exit:
    SHR_FUNC_EXIT;
}


int sand_mem_array_wide_access(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data,
    unsigned do_read) 
{
    uint8 acc_type;
    int rv, blk, cmic_block, need_to_unlock = 0, need_to_unlock_inst = 0;
    uint32 nof_words, words_left, words_in_operation, w_i, access_size, reg32,
           addr_address, addr_write_data, addr_read_data, cache_consistency_check = 0,
           *entry_words, *buf;
    unsigned nof_test_reads = 0;
    uint64 reg64 = 0;
#ifdef MEASURE_INDIRECT_ACCESS
    sal_usecs_t start_time, operation_time;
#endif
    soc_reg_above_64_val_t reg_above_64;
    soc_reg_access_info_t access_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!soc_mem_is_valid(unit, mem) || index < soc_mem_index_min(unit, mem) || index > soc_mem_index_max(unit, mem)) {
        SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Invalid memory or index %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }
    if (do_read) {
        do_read = 1;
    }

    nof_words = soc_mem_entry_words(unit, mem);

    
    rv = soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_address = access_info.offset;
    rv |= soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_write_data = access_info.offset;
    rv |= soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info);
    addr_read_data = access_info.offset;
    if (rv != SOC_E_NONE) {
        SHR_EXIT_WITH_LOG(SOC_E_FAIL, "Invalid memory or index %s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    
    access_size = (soc_feature(unit, soc_feature_sbus_width_120B) && !SOC_IS_J2P(unit)) ? 30 : 20;
    flags |= SOC_MEM_IGNORE_FORCE_READ_THROUGH;

    MEM_LOCK(unit, mem);
    need_to_unlock = 1;

    
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != SOC_CORE_ALL && copyno != blk) {
            continue;
        }
#ifdef MEASURE_INDIRECT_ACCESS
        start_time = sal_time_usecs();
#endif

        if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE) {
            if (!do_read) {
                _soc_mem_write_cache_update(unit, mem, blk, 0, index, array_index, entry_data, NULL, NULL, NULL);
            } else {
                if (TRUE == _soc_mem_read_cache_attempt(unit, flags, mem, blk, index, array_index, entry_data, NULL, &cache_consistency_check)) {
                    SHR_EXIT();
                }
            }
        }

        cmic_block = SOC_BLOCK_INFO(unit, blk).cmic;

        
        reg32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &reg32, INDIRECT_COMMAND_ADDRf,
          soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type));
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, &reg32, INDIRECT_COMMAND_TYPEf, do_read);

        SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_block_instance_lock(unit, blk));
        need_to_unlock_inst = 1;

        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_block, addr_address, 1, &reg32));

        if (do_read) { 
            soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_TRIGGERf, 1);
        } else { 
            soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1);
            soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_COUNTf, 1);
        }
        soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_TIMEOUTf, SOC_IS_J2X(unit) ? SOC_CONTROL(unit)->bistTimeout : 0x3fff); 
        SHR_IF_ERR_EXIT(sand_indirect_command_reg_set(unit, blk, reg64));

        
        entry_words = entry_data;
        for (words_left = nof_words; words_left > 0; words_left -= words_in_operation)
        {
            words_in_operation = access_size < words_left ? access_size : words_left;
            if (do_read) {  
                SHR_IF_ERR_EXIT(soc_direct_reg_get(unit, cmic_block, addr_read_data, words_in_operation, reg_above_64));
                for (buf = reg_above_64, w_i = words_in_operation; w_i; --w_i) {
                    *(entry_words++) = *(buf++);
                } 
            } else { 
                for (w_i = 0; w_i < words_in_operation; ++w_i) {
                    reg_above_64[w_i] = *(entry_words++);
                } 
                SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_block, addr_write_data, words_in_operation, reg_above_64));
            }
        }
        if (!do_read) {
            
            
            
            do {
                SHR_IF_ERR_EXIT(sand_indirect_command_reg_get(unit, blk, &reg64));
            } while (soc_reg64_field32_get(unit, ECI_INDIRECT_COMMANDr, reg64, INDIRECT_COMMAND_COUNTf) && ++nof_test_reads < 10);
            if (nof_test_reads >= 10) {
                SHR_EXIT_WITH_LOG(SOC_E_PARAM, "Indirect operation did not finish%s%s%s\n", EMPTY, EMPTY, EMPTY);
            }
        }
        need_to_unlock_inst = 0;
        SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_block_instance_unlock(unit, blk));
#ifdef MEASURE_INDIRECT_ACCESS
        operation_time = sal_time_usecs() - start_time;
        SHR_IF_ERR_EXIT(sal_mutex_take(indirect_access_summary_mutex[unit], sal_mutex_FOREVER));
        indirect_access_total_time[unit] += operation_time;
        indirect_access_nof_operations[unit]++;
        if (operation_time < indirect_access_min_time[unit]) {
            indirect_access_min_time[unit] = operation_time;
        }
        if (operation_time > indirect_access_max_time[unit]) {
            indirect_access_max_time[unit] = operation_time;
        }
        if (!do_read) {
            indirect_access_total_waits[unit] += nof_test_reads;
            indirect_access_nof_writes[unit]++;
            if (nof_test_reads < indirect_access_min_waits[unit]) {
                indirect_access_min_waits[unit] = nof_test_reads;
            }
            if (nof_test_reads > indirect_access_max_waits[unit]) {
                indirect_access_max_waits[unit] = nof_test_reads;
            }
        }
        SHR_IF_ERR_EXIT(sal_mutex_give(indirect_access_summary_mutex[unit]));
#endif

    } 

exit:
    if (need_to_unlock) {
        MEM_UNLOCK(unit, mem);
    }
    if (need_to_unlock_inst) {
        SHR_IF_ERR_CONT(soc_sand_indirect_mem_access_block_instance_unlock(unit, blk));
    }
    SHR_FUNC_EXIT;
}


int sand_tbl_is_dynamic(int unit,soc_mem_t mem)
{
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit) && soc_dfe_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif 

#ifdef BCM_DPP_SUPPORT
    if(SOC_IS_DPP(unit) && dpp_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif 

#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit) && soc_dnxf_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif 
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit) && dnx_tbl_is_dynamic(unit, mem)) {
        return TRUE;
    }
#endif 


    return FALSE;
}


int sand_mem_contain_one_of_the_fields(int unit, const soc_mem_t mem, soc_field_t *fields)
{
    int i;
    for (i=0;fields[i]!=NUM_SOC_FIELD;i++) 
    {
        if (SOC_MEM_FIELD_VALID(unit,mem,fields[i])) 
        {
            return 1;
        }
    }
    return 0;
}




int soc_sand_cache_table_update_all(int unit)
{
    int  res = _SHR_E_NONE;
    int  blk, mem, i ,index_cnt;
    soc_memstate_t *mem_state;
    uint32 rd_data[120];
    int min_array_index, max_array_index, array_idx;
    uint32 table_size, *table;
#ifdef BCM_DNX_SUPPORT
    uint32 mux_switched = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        
        if (!SOC_MEM_IS_VALID(unit, mem) || (!SOC_MEM_FIELD_VALID(unit, mem, ECCf) && !SOC_MEM_FIELD_VALID(unit, mem, PARITYf)))
        {
            continue;
        }
        max_array_index = min_array_index = 0;
        if (SOC_MEM_IS_ARRAY(unit, mem))
        {
            soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
            if (maip)
            {
                max_array_index = (maip->numels - 1) + maip->first_array_index;
                min_array_index = maip->first_array_index;
            }
        }

#ifdef BCM_DNX_SUPPORT
        if ((mem == MDB_EEDB_ENTRY_BANKm) && (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_eedb_bank_traffic_lock))) {
            continue;
        }
#endif
        MEM_LOCK(unit, mem);

        mem_state = SOC_MEM_STATE(unit, mem);

        
        SOC_MEM_BLOCK_ITER(unit, mem, blk)
        {
            if (!SOC_MEM_BLOCK_VALID(unit, mem, blk))
            {
                continue;
            }
            if (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_WRITE_CACHE_RESTORE) && 
                 (mem_state->cache[blk] != NULL) && 
                  CACHE_VMAP_TST(mem_state->vmap[blk], 0) && 
                  !SOC_MEM_TEST_SKIP_CACHE(unit)) 
            {
                index_cnt = soc_mem_index_count(unit, mem);
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit)){
                    res = dnx_egr_esb_nif_calendar_mux_switch_start(unit, mem, blk, &mux_switched);
                    if (res != _SHR_E_NONE) {
                        LOG_WARN(BSL_LS_BCM_INTR, (BSL_META_U(unit, "dnx_egr_esb_nif_calendar_mux_switch_start memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
                    }
                }
#endif
                if (soc_feature(unit, soc_feature_table_dma) && soc_mem_dmaable(unit, mem, blk))
                {
                    table_size = soc_mem_entry_words(unit, mem) * soc_mem_index_count(unit, mem) * 4;
                    if ((table = soc_cm_salloc(unit, table_size, "dma")) != NULL) {
                        if (SOC_MEM_IS_ARRAY(unit, mem)) {
                            for (array_idx = min_array_index; array_idx <= max_array_index; array_idx++)
                            {
                                res = soc_mem_array_read_range(unit, mem, array_idx, blk,
                                                               soc_mem_index_min(unit, mem),
                                                               soc_mem_index_max(unit, mem),
                                                               table);
                                if (res != _SHR_E_NONE) {
                                    LOG_WARN(BSL_LS_BCM_INTR, (BSL_META_U(unit, "access array memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
                                }
                            }
                        } else {
                             res = soc_mem_read_range(unit, mem, blk,
                                                      soc_mem_index_min(unit, mem),
                                                      soc_mem_index_max(unit, mem),
                                                      table);
                             if (res != _SHR_E_NONE) {
                                 LOG_WARN(BSL_LS_BCM_INTR, (BSL_META_U(unit, "access memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
                             }

                        }
                        soc_cm_sfree(unit, table);
                    }
                } else {
                    for (array_idx = min_array_index; array_idx <= max_array_index; array_idx++) {
                        for (i = 0 ; i < index_cnt ; i++) {
                            
                            res = soc_mem_array_read_flags(unit, mem, array_idx, blk, i, rd_data, SOC_MEM_DONT_USE_CACHE);
                            if (res != _SHR_E_NONE)
                            {
                                MEM_UNLOCK(unit, mem);
                                SHR_IF_ERR_EXIT(res);
                            }
                        }
                    }
                }
#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit)){
                    res = dnx_egr_esb_nif_calendar_mux_switch_end(unit, mem, blk, mux_switched);
                    if (res != _SHR_E_NONE) {
                        LOG_WARN(BSL_LS_BCM_INTR, (BSL_META_U(unit, "dnx_egr_esb_nif_calendar_mux_switch_end memory %s failed\r\n"), SOC_MEM_NAME(unit, mem)));
                    }
                }
#endif
            }
        }
        MEM_UNLOCK(unit, mem);
    } 

exit:
    SHR_FUNC_EXIT; 
}



int sand_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr)
{
    int       kk, ii;
    int       rv ;
    int       copyno ;


    rv = 0 ;

    if (mem >= NUM_SOC_MEM) {
        LOG_INFO(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit, "%s(): Illegal mem specifier: %d\n"),
            __func__,(int)mem)) ;
        rv = -1 ;
        goto exit ;
    }
    copyno = SOC_MEM_BLOCK_MIN(unit, mem) ;
    if (copyno != SOC_MEM_BLOCK_MAX(unit, mem)) {
        LOG_INFO(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit, "%s(): Memory has more than one block: table %s.%s. Num blocks %d\r\n"),
            __func__,SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), (SOC_MEM_BLOCK_MAX(unit, mem) - copyno))) ;
        rv = -2 ;
        goto exit ;
    }

    for (kk = index; kk < index + count; kk++) {
         {
           ii = soc_mem_array_read_flags(unit, mem, array_index, copyno, kk, entry_ptr, SOC_MEM_DONT_USE_CACHE);
        }
        if (ii < 0) {
            LOG_INFO(BSL_LS_APPL_COMMON,
                (BSL_META_U(unit, "%s(): Read ERROR: table %s.%s[%d]: %s\n"),
                __func__,SOC_MEM_UFNAME(unit, mem), SOC_BLOCK_NAME(unit, copyno), kk, soc_errmsg(ii))) ;
            rv = -3 ;
            goto exit ;
        }
    }
 exit:
    return rv;
}



int soc_sand_control_dynamic_mem_writes(
    const int unit,
    const soc_reg_t *regs, 
    const uint32 val) 
{
    const soc_reg_t *r = regs;
    soc_reg_above_64_val_t data = { 0 };
    int rv = SOC_E_NONE, rv2;
    SHR_FUNC_INIT_VARS(unit);

    if((SOC_IS_Q2A(unit) || SOC_IS_J2P(unit) || SOC_IS_J2X(unit)) && val)
    {
        SOC_REG_ABOVE_64_ALLONES(data);
    }
    else
    {
        data[0] = val;
    }

    for (; *r != INVALIDr; ++r)
    {
        soc_block_t blk;
        for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++)
        {
            if (SOC_REG_IS_VALID(unit, *r))
            {
                if (SOC_REG_BLOCK_IN_LIST(unit, *r, SOC_BLOCK_TYPE(unit, blk)))
                {
                    rv2 = soc_reg_above_64_set(unit, *r, SOC_BLOCK_NUMBER(unit, blk), 0, data);
                    if (rv2 != SOC_E_NONE && rv == SOC_E_NONE) {
                        rv = rv2;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT;
}




int
soc_sand_mem_is_in_soc_property(int unit, soc_mem_t mem, int en)
{
    
    char mem_name[SOC_PROPERTY_NAME_MAX];
    char *mptr;

    if (en == TRUE)    
    {
        sal_strncpy(mem_name, spn_MEM_CACHE_ENABLE, SOC_PROPERTY_NAME_MAX);
        mptr = &mem_name[sal_strlen(mem_name)];
        sal_strncpy(mptr, "_specific_", (SOC_PROPERTY_NAME_MAX - sal_strlen(mem_name)));
    }
    else            
        sal_strncpy(mem_name, "mem_nocache_",SOC_PROPERTY_NAME_MAX);

    mptr = &mem_name[sal_strlen(mem_name)];
    sal_strncpy(mptr, SOC_MEM_NAME(unit, mem), (SOC_PROPERTY_NAME_MAX - sal_strlen(mem_name)));

    if (soc_property_get(unit, mem_name, 0))
    {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("unit %d memory %d (%s), soc_prop %s\n"), unit, mem, SOC_MEM_NAME(unit, mem), mem_name));
        return TRUE;
    }
    return FALSE;
}


int sand_tbl_mem_cache_mem_set(int unit, soc_mem_t mem, void* en)
{
    int enable  = *(int*)en;
#ifdef BCM_DNX_SUPPORT
    uint32 mux_switched = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    if(!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
    {
        SHR_EXIT();
    }

#ifdef BCM_DNX_SUPPORT
    if ((mem == MDB_EEDB_ENTRY_BANKm) && (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_eedb_bank_traffic_lock))) {
        SHR_EXIT();
    }
#endif

    
    if (enable == TRUE)
    {
        if (soc_sand_mem_is_in_soc_property(unit, mem, FALSE))
        {
            SHR_EXIT();
        }
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(dnx_egr_esb_nif_calendar_mux_switch_start(unit, mem, SOC_MEM_BLOCK_MIN(unit, mem), &mux_switched));
    }
#endif
    
    if ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)
    {
        SHR_IF_ERR_EXIT(soc_mem_cache_set(unit, mem, COPYNO_ALL, enable));
    }

exit:
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_CONT(dnx_egr_esb_nif_calendar_mux_switch_end(unit, mem, SOC_MEM_BLOCK_MIN(unit, mem), mux_switched));
    }
#endif
    SHR_FUNC_EXIT;
}


int
sand_mem_indirect_poll_done(
        int unit,
        int blk_id,
        int time_out)
{
    uint64 reg64;
    int count;
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    
    soc_timeout_init(&to,time_out, 100);
    SHR_IF_ERR_EXIT(sand_indirect_command_reg_get(unit, blk_id, &reg64));
    count = soc_reg64_field32_get(unit, ECI_INDIRECT_COMMANDr, reg64, INDIRECT_COMMAND_COUNTf);
    while (count)
    {
        if (soc_timeout_check(&to))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Timeout on block %d\n", blk_id);
        }
        SHR_IF_ERR_EXIT(sand_indirect_command_reg_get(unit, blk_id, &reg64));
        count = soc_reg64_field32_get(unit, ECI_INDIRECT_COMMANDr, reg64, INDIRECT_COMMAND_COUNTf);
    }

exit:
    SHR_FUNC_EXIT;
}


int
sand_mem_indirect_reset_write(
        int unit,
        soc_mem_t mem,
        int blk_id,
        uint32 address,
        uint32 nof_entries,
        uint32 *value)
{
    uint64 reg64 = 0;
    int reg_addr, reg_data, reg_wide;
    int i;
    soc_reg_access_info_t access_info;
    int cmic_blk;
    int entry_size;
    soc_driver_t *d = SOC_DRIVER(unit);
    int block_type = d->block_info[blk_id].type; 
    int indirect_write_size = d->indirect_write_reg_nof_uint32s[block_type];
    int rv = 0;

    SHR_FUNC_INIT_VARS(unit);

    entry_size = soc_mem_entry_words(unit, mem);

    
    SHR_IF_ERR_EXIT(soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info));
    reg_addr = access_info.offset;
    SHR_IF_ERR_EXIT(soc_reg_xaddr_get(unit, ECI_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info));
    reg_data = access_info.offset;
    SHR_IF_ERR_EXIT(soc_reg_xaddr_get(unit, ECI_REG_0041r, REG_PORT_ANY, 0, SOC_REG_ADDR_OPTION_NONE, &access_info));
    reg_wide = access_info.offset;
    cmic_blk = SOC_BLOCK_INFO(unit, blk_id).cmic;

    
    rv = sand_mem_indirect_poll_done(unit, blk_id, 30000);
    SHR_IF_ERR_EXIT_WITH_LOG(rv, "Indirect poll for cmic block=%d, reg_offset=%d, mem=%s failed.\n",
                             blk_id, reg_addr, SOC_MEM_NAME(unit, mem));

    
    soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1);
    soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_TIMEOUTf, SOC_IS_J2X(unit) ? SOC_CONTROL(unit)->bistTimeout : 0xe00);
    soc_reg64_field32_set(unit, ECI_INDIRECT_COMMANDr, &reg64, INDIRECT_COMMAND_COUNTf, nof_entries);
    SHR_IF_ERR_EXIT(sand_indirect_command_reg_set(unit, blk_id, reg64));

    SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_blk, reg_addr, 1, &address));

    if (entry_size > indirect_write_size)
    {
        uint32 val = 1;
        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_blk, reg_wide, 1, &val));
    }

    if (indirect_write_size == 0) {
        SHR_ERR_EXIT(SOC_E_PARAM, "Block type %d with block id %d does not support indirect writes.\n", block_type, blk_id);
    }
    for (i = 0; i < (entry_size + indirect_write_size - 1) / indirect_write_size; i++)
    {
        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_blk, reg_data, indirect_write_size, value));
    }

    if (entry_size > indirect_write_size)
    {
        uint32 val = 0;
        SHR_IF_ERR_EXIT(soc_direct_reg_set(unit, cmic_blk, reg_wide, 1, &val));
    }

exit:
    SHR_FUNC_EXIT;
}



int
sand_fill_mem_indirect_reset_write(
        const int unit,
        const soc_mem_t mem,
        const int copyno,     
        uint32 array_index_start,
        uint32 array_index_end,
        uint32 index_start,
        uint32 index_end,
        const uint32 *entry)
{
    uint32 uint32_index, nof_entries, array_element_addr_skip;
    uint32 address, start_address, array_i, entry_i;
    int blk_inst_start, blk_inst_end, blk_inst;
    uint8 access_type = 0;
    soc_mem_array_info_t *maip;
#ifdef DCMN_RUNTIME_DEBUG
    sal_time_t start_time = sal_time();
    unsigned unsigned_i;
#endif 

    SHR_FUNC_INIT_VARS(unit);

   if (entry == NULL) {
        SHR_ERR_EXIT(SOC_E_PARAM, "null entry input\n");
    } else if (!soc_mem_is_valid(unit, mem)) {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Invalid memory for unit\n");
    }
    if (copyno >= SOC_MEM_INFO(unit, mem).minblock && copyno <= SOC_MEM_INFO(unit, mem).maxblock) {
        blk_inst_start = blk_inst_end = copyno;
    } else if (copyno == COPYNO_ALL || copyno == SOC_CORE_ALL) {
        blk_inst_start = SOC_MEM_INFO(unit, mem).minblock;
        blk_inst_end = SOC_MEM_INFO(unit, mem).maxblock;
    } else {
        SHR_ERR_EXIT(SOC_E_PARAM, "Invalid block instance %d specified for memory %s\n", copyno, SOC_MEM_NAME(unit, mem));
    }

    
    if (SOC_MEM_IS_ARRAY(unit, mem) && (maip = SOC_MEM_ARRAY_INFOP(unit, mem)) != NULL) {
        array_element_addr_skip = maip->element_skip;
        if (array_index_start < maip->first_array_index) {
            array_index_start = maip->first_array_index;
        }
        if (array_index_end < array_index_start) {
            array_index_end = array_index_start;
        } else {
            uint32_index = maip->numels + maip->first_array_index; 
            if (array_index_end >= uint32_index) {
                array_index_end = uint32_index - 1;
            }
        }
    } else {
        array_element_addr_skip = 0;
        array_index_start = array_index_end = 0;
    }
    
    uint32_index = SOC_MEM_INFO(unit, mem).index_min;
    if (index_start < uint32_index) {
        index_start = uint32_index;
    }
    if (index_end < index_start) {
        index_end = index_start;
    } else {
        uint32_index = SOC_MEM_INFO(unit, mem).index_max;
        if (index_end > uint32_index) {
            index_end = uint32_index;
        }
    }

    start_address = soc_mem_addr_get(unit, mem, array_index_start, blk_inst_start, index_start, &access_type);
    
    if ( index_start == SOC_MEM_INFO(unit, mem).index_min &&
         index_end == SOC_MEM_INFO(unit, mem).index_max &&
         array_element_addr_skip == index_end + 1 - index_start ) {
        index_end += array_element_addr_skip * (array_index_end - array_index_start);
        array_index_end = array_index_start;
    }

    
    for (array_i = array_index_start; array_i <= array_index_end; ++array_i) {
        address = start_address;
        for (entry_i = index_start; entry_i <= index_end ; entry_i += MAX_ENTRIES_IN_WRITE) {
            nof_entries = index_end + 1 - entry_i; 
            if (nof_entries > MAX_ENTRIES_IN_WRITE) {
                nof_entries = MAX_ENTRIES_IN_WRITE;
            }
            
            for (blk_inst = blk_inst_start; blk_inst <= blk_inst_end; ++blk_inst) {
                SHR_IF_ERR_EXIT(sand_mem_indirect_reset_write(unit, mem, blk_inst, address, nof_entries, (uint32*)entry));
                _soc_mem_write_cache_update_range(unit, mem, array_i, array_i, blk_inst, entry_i, entry_i + nof_entries - 1, entry);
            }
            address += MAX_ENTRIES_IN_WRITE;
        }
    }

    
    for (blk_inst = blk_inst_start; blk_inst <= blk_inst_end; ++blk_inst) {
        
        SHR_IF_ERR_EXIT(sand_mem_indirect_poll_done(unit, blk_inst, 10000));
    }

exit:
#ifdef DCMN_RUNTIME_DEBUG
    if (((unsigned_i = sal_time() - start_time)) >= 5) {
        LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_mem_indirect_reset_write(%s) ran for %u:%2.2u\n",
          SOC_MEM_NAME(unit, mem), unsigned_i / 60, unsigned_i % 60));
    }
    if (sand_runtime_debug_per_device[unit].run_stage <= sand_runtime_debug_state_initializing) {
        for (unsigned_i = 0; unsigned_i < soc_mem_entry_words(unit, mem); ++unsigned_i) {
            if (((const uint32*)entry_data)[unsigned_i]) { 
                break;
            }
        }
        if (unsigned_i >= soc_mem_entry_words(unit, mem)) {
            LOG_INFO(BSL_LS_SOC_INIT, ("==> sand_fill_table_with_entry(%s) received a zero entry\n", SOC_MEM_NAME(unit, mem)));
        }
    }
#endif 
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
