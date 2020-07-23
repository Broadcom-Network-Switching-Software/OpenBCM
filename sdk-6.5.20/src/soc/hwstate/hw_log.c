/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Hw Log journal for Crash Recovery LOG Mechanism.
 */

#include <appl/diag/parse.h>
#include <soc/dcmn/error.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <soc/sbusdma.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_chip_regs.h>
#endif

#if !defined(__KERNEL__) && defined (LINUX)
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/ha.h>
#define HW_LOG_VERSION_1_0 1
#define HW_LOG_STRUCT_SIG 0
typedef enum {
    HA_HW_LOG_SUB_ID_0 = 0
} HA_sub_id_tl;
#endif
#endif

/* ERROr Module Name */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

#define HWLOG_MARGIN sizeof(HwLogEntry_t)


/* change this flag to 1 to get debug prints */
#define HW_LOG_DEBUG_ENABLE 0
#if HW_LOG_DEBUG_ENABLE
#define HW_LOG_DEBUG(op) op
#else
#define HW_LOG_DEBUG(op)
#endif


/* ------------------------------EXTERNAL FUNCTION DECLARATION-----------------------*/
int reg_test(int unit, args_t *a, void *pa);

/* ------------------------------INTERNAL FUNCTION DECLARATION-----------------------*/
STATIC int _soc_hw_log_insert_entry_to_log(int unit, LogEntry_t *data);
STATIC int _soc_hw_log_write_memory_to_log(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
STATIC int _soc_hw_log_read_memory_entry_from_log(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
STATIC int _soc_hw_log_write_register32_to_log(int unit, soc_reg_t reg, int port, int index, uint32 entry_data);
STATIC int _soc_hw_log_read_register32_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint32 *entry_data);
STATIC int _soc_hw_log_write_register64_to_log(int unit, soc_reg_t reg, int port, int index, uint64 entry_data);
STATIC int _soc_hw_log_read_register64_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint64 *entry_data);
STATIC int _soc_hw_log_write_reg_above64_to_log(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t entry_data);
STATIC int _soc_hw_log_read_reg_above64_entry_from_log(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t entry_data);
STATIC int _soc_hw_log_polling(int unit, sal_usecs_t time_out, int32 min_polls, soc_reg_t reg, int32 port, int32 index, soc_field_t field, uint32 expected_value);
STATIC int _soc_hw_log_direct_reg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data);
STATIC int _soc_hw_log_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
STATIC int _soc_hw_log_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
STATIC int _soc_hw_log_soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 data);
STATIC int _soc_hw_log_soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data);
STATIC int _soc_hw_log_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 data);
STATIC int _soc_hw_log_is_suppressed(int unit);
STATIC int _soc_hw_is_immediate_hw_access_enabled(int unit);
STATIC uint32 _soc_hw_log_commit_counter_get(int unit);
STATIC int _soc_hw_log_commit_counter_set(int unit, uint32 count);
STATIC int _soc_hw_log_commit_counter_reset(int unit);
STATIC int _soc_hw_log_mem_sbusdma_write(int unit, int flags, soc_mem_t mem,
                             unsigned array_index_start,
                             unsigned array_index_end, int copyno,
                             int index_begin, int index_end,
                             void *buffer, uint8 mem_clear,
                             int clear_buf_ent, int vchan);
STATIC int _soc_hw_log_mem_sbusdma_read(int unit, soc_mem_t mem, unsigned array_index,
                            int copyno, int index_min, int index_max,
                            uint32 ser_flags, void *buffer, int vchan);
STATIC int _soc_hw_log_sbusdma_entry_commit(int unit, LogEntry_t *data);
STATIC int _soc_hw_log_mem_sbusdma_get_size(int unit, soc_mem_t mem, int index_begin, int index_end, uint8 mem_clear);
STATIC void *_soc_hw_log_sbusdma_buffer_allocate(int unit, uint32 size);
STATIC int _soc_hw_log_sbusdma_buffers_free(int unit);

STATIC int _soc_hw_log_descdma_write(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);


/* ------------------------------Global Variables------------------------------------*/

soc_hw_log_access_t crash_rec_access =
{
        _soc_hw_log_read_register32_entry_from_log,
        _soc_hw_log_read_register64_entry_from_log,
        _soc_hw_log_read_reg_above64_entry_from_log,
        _soc_hw_log_write_register32_to_log,
        _soc_hw_log_write_register64_to_log,
        _soc_hw_log_write_reg_above64_to_log,
        _soc_hw_log_read_memory_entry_from_log,
        _soc_hw_log_write_memory_to_log,
        _soc_hw_log_polling,
        _soc_hw_log_direct_reg_set,
        _soc_hw_log_fast_reg_set,
        _soc_hw_log_fast_reg_get,
        _soc_hw_log_soc_reg_set,
        _soc_hw_log_soc_reg_get,
        _soc_hw_log_reg_set_nocache,
        _soc_hw_log_mem_sbusdma_write,
        _soc_hw_log_mem_sbusdma_read,
        _soc_hw_log_descdma_write
};

LogList_t Hw_Log_List[SOC_MAX_NUM_DEVICES];


/* ------------------------------FUNCTION IMPLEMENTATION-----------------------------*/

STATIC uint32
_soc_hw_log_dma_get_buff_idx(int index_begin, int index_end, int entry_idx)
{
    uint32 buff_idx;

    if(index_begin <= index_end) {
        buff_idx = (entry_idx - index_begin);
    } else { /* reverse direction */
        buff_idx = (index_begin - entry_idx);
    }

    return buff_idx;
}

STATIC void
_soc_hw_log_brcd_blocks_list_init(int unit)
{
    int blk;

    soc_info_t *si = &SOC_INFO(unit);

    for(blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        Hw_Log_List[unit].brcd_blocks[blk] = (NULL == si->broadcast_blocks[blk]) ? 0 : 1;
    }
}


STATIC int
_soc_hw_log_log_list_is_empty(int unit)
{
    return !(Hw_Log_List[unit].header->nof_elements > 0);
}

int
soc_hw_log_is_initialized(int unit)
{
    return Hw_Log_List[unit].init;
}

int
soc_hw_set_immediate_hw_access(int unit)
{
    SOC_INIT_FUNC_DEFS;

    /* make sure that we are currently in a middle of a transaction */
    if (!soc_dcmn_cr_utils_is_logging(unit)) SOC_EXIT;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    /* increment the counter and set the immediate access to TRUE*/
    Hw_Log_List[unit].access_ct++;
    Hw_Log_List[unit].ImmediateAccess = TRUE;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int
soc_hw_restore_immediate_hw_access(int unit)
{
    SOC_INIT_FUNC_DEFS;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    if (Hw_Log_List[unit].access_ct <= 0) {
        SOC_EXIT;
    }
    
    /* decrease the immediate access counter */
    Hw_Log_List[unit].access_ct--;

    /* if there are no more nested levels of immediate access, then resume journaling */
    Hw_Log_List[unit].ImmediateAccess =  (Hw_Log_List[unit].access_ct > 0);
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int
soc_hw_reset_immediate_hw_access_counter(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit))
        return SOC_E_UNIT;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);

    Hw_Log_List[unit].access_ct = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_soc_hw_is_immediate_hw_access_enabled(int unit)
{
    return (Hw_Log_List[unit].ImmediateAccess);
}

int
soc_hw_ensure_immediate_hw_access_disabled(int unit)
{
    SOC_INIT_FUNC_DEFS;
    
    if(_soc_hw_is_immediate_hw_access_enabled(unit)) {
        return SOC_E_FAIL;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_do_hwlog_read_write(int unit)
{
    return (SOC_CR_ENABALED(unit)                        && /* don't jounal while crash recovery is disabled */
           soc_hw_log_is_initialized(unit)               && /* HW LOG is enabled and initialized */
           soc_dcmn_cr_utils_is_journaling_thread(unit)  && /* Allow journaling only from the journaling thread */
           !_soc_hw_is_immediate_hw_access_enabled(unit) && /* check if immediate hw access is enabled */
           !_soc_hw_log_is_suppressed(unit)              && /* check if log is suppressed */
           soc_dcmn_cr_utils_is_logging(unit));
}

int
soc_hw_log_init(int unit, uint32 size)
{
    uint8  *mem_ptr = NULL;

    SOC_INIT_FUNC_DEFS;

    /* allocation hw log in Shared memory
       (cold boot - create; warm boot - retrieve) */
    if (ha_mem_alloc(unit, HA_HW_LOG_Mem_Pool, HA_HW_LOG_SUB_ID_0, &size, (void**)&mem_ptr) != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }
    if (!mem_ptr)
    {
        LOG_ERROR(BSL_LS_SOC_HWLOG,
                       (BSL_META_U(unit, "HW LOG Allocation failed for unit %d. LogSize %d\n"), unit, size));
        return SOC_E_MEMORY;
    }

    /* set the static data (warm and cold boot) */
    Hw_Log_List[unit].Access_cb = crash_rec_access;
    Hw_Log_List[unit].ImmediateAccess = FALSE;
    Hw_Log_List[unit].access_ct = 0;
    Hw_Log_List[unit].is_suppressed = FALSE;
    Hw_Log_List[unit].header = (hw_log_header_t *) mem_ptr;
    Hw_Log_List[unit].journal = (HwLogEntry_t *) (mem_ptr + sizeof(hw_log_header_t));
    Hw_Log_List[unit].size = size;
    _soc_hw_log_brcd_blocks_list_init(unit);

    /* Reset the journal (only in Cold boot */
    if (!SOC_WARM_BOOT(unit)) {
        memset(mem_ptr, 0, size);

        Hw_Log_List[unit].header->max_elements = 
            (size - HW_LOG_DMA_BUFFER_SIZE - sizeof(hw_log_header_t) - HWLOG_MARGIN) / sizeof(HwLogEntry_t);
        Hw_Log_List[unit].header->nof_elements = 0;
        _soc_hw_log_commit_counter_reset(unit);
    }

    /* mark as initialized */
    Hw_Log_List[unit].init = TRUE;

    LOG_VERBOSE(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "HW LOG Enabled for unit %d. LogSize %d Bytes\n"), unit, size));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_deinit(int unit)
{
    Hw_Log_List[unit].ImmediateAccess = TRUE;
    Hw_Log_List[unit].init = FALSE;

    return SOC_E_NONE;
}

void
_soc_hw_log_print_list_memory_entry(int unit, LogEntry_Data_t *Data)
{
    uint32 entry_bytes = soc_mem_entry_bytes(unit, Data->MemData.Mem);
    uint32 entry_words = soc_mem_entry_words(unit, Data->MemData.Mem);

    int i;
    /* Memory entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d mem %d (%s) Array %d blk %d index %d  bytesInEntry %d wordsInEntry %d data 0x%X\n"), unit, Data->MemData.Mem,
                    SOC_MEM_NAME( unit, Data->MemData.Mem), Data->MemData.ArrayIndex, Data->MemData.Blk, /*Use COPYNO_ALL for all*/
            Data->MemData.Index, entry_bytes, entry_words, Data->MemData.EntryData[0]));

    for (i = 1; i < entry_words ; i++) /* Print the data */
    {
        LOG_ERROR(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "[%d]%X\n"), i, Data->MemData.EntryData[i]));
    }
/*    LOG_ERROR(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "\n")));*/
}

void
_soc_hw_log_print_list_register_entry(int unit, LogEntry_Data_t *Data)
{
    /* Register entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d Reg %d (%s) Port 0x%X Index %d Data 0x%X\n"), unit, Data->RegData.Reg,
                    SOC_REG_NAME( unit, Data->RegData.Reg), Data->RegData.Port,
                    Data->RegData.Index, Data->RegData.EntryData[0]));
}

void
_soc_hw_log_print_list_polling_entry(int unit, LogEntry_Data_t *Data)
{
    /* Polling entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d Polling: Reg %d (%s) Timeout %d MinPolls %d Port 0x%X Index %d ExpectedValue 0x%X\n"),
                    unit,
                    Data->PollingData.Reg,
                    SOC_REG_NAME( unit, Data->PollingData.Reg),
                    Data->PollingData.TimeOut,
                    Data->PollingData.MinPolls,
                    Data->PollingData.Port,
                    Data->PollingData.Index,
                    Data->PollingData.ExpectedValue));
}

void
_soc_hw_log_print_list_direct_reg_entry(int unit, LogEntry_Data_t *Data)
{
    /* Direct Regs entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d DirectRegSet: CmicBlock 0x%X Addr 0x%X DwcWrite 0x%X Data 0x%X\n"),
                    unit,
                    Data->DirectRegData.CmicBlock,
                    Data->DirectRegData.Addr,
                    Data->DirectRegData.DwcWrite,
                    Data->DirectRegData.EntryData));
}

void
_soc_hw_log_print_list_fast_reg_entry(int unit, LogEntry_Data_t *Data)
{
    /* Direct Regs entry print */
    LOG_ERROR(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "Unit %d FastRegSet: Reg %d (%s) AccType %d Addr 0x%X Block 0x%X Data 0x%X\n"),
                    unit,
                    Data->FastRegData.Reg,
                    SOC_REG_NAME( unit, Data->FastRegData.Reg),
                    Data->FastRegData.AccType,
                    Data->FastRegData.Addr,
                    Data->FastRegData.Block,
                    Data->FastRegData.EntryData[0]));
}
void
_soc_hw_log_print_list_soc_reg_entry(int unit, LogEntry_Data_t *Data)
{
    _soc_hw_log_print_list_register_entry(unit, Data);
}

void
soc_hw_log_print_single_entry(int unit, HwLogEntry_t *entry)
{
	if (!entry)
		return;

    if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
    {
        _soc_hw_log_print_list_memory_entry(unit, &entry->Entry.Data);
    }
    else if ((entry->Entry.Type == LOG_ENTRY_TYPE_Register32) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register64) || (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64))
    {
        _soc_hw_log_print_list_register_entry(unit, &entry->Entry.Data);
    }
    else if (entry->Entry.Type == LOG_ENTRY_TYPE_Polling)
    {
        _soc_hw_log_print_list_polling_entry(unit, &entry->Entry.Data);
    }
    else if (entry->Entry.Type == LOG_ENTRY_TYPE_DirectRegWrite)
    {
        _soc_hw_log_print_list_direct_reg_entry(unit, &entry->Entry.Data);
    }
    else if  (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
    {
        _soc_hw_log_print_list_fast_reg_entry(unit, &entry->Entry.Data);
    }
    else if ((entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData) || (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegNoCacheData))
    {
        _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);
    }
}

int
soc_hw_log_print_list(int unit)
{
    uint32 i;

    SOC_INIT_FUNC_DEFS;

    if (_soc_hw_log_log_list_is_empty(unit))
        return SOC_E_NONE;

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {
        soc_hw_log_print_single_entry(unit, &(Hw_Log_List[unit].journal[i]));
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int
soc_hw_log_purge(int unit)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].header->nof_elements = 0;
    _soc_hw_log_commit_counter_reset(unit);
    _soc_hw_log_sbusdma_buffers_free(unit);
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int _soc_hw_log_sbusdma_entry_commit(int unit, LogEntry_t *entry)
{
    void *buffer;
    soc_error_t res;
    int alloc_size = _soc_hw_log_mem_sbusdma_get_size(unit, entry->Data.SbusDMA.mem, entry->Data.SbusDMA.index_begin, entry->Data.SbusDMA.index_end, entry->Data.SbusDMA.mem_clear);

    /* create a dmaable buffer and copy the contents of the existing buffer to it */
    buffer = soc_cm_salloc(unit, alloc_size, "dma_commit phase");

    sal_memcpy(buffer,entry->Data.SbusDMA.buffer,alloc_size);

    /* perform the actual dma operationg */
    res =   _soc_mem_array_sbusdma_write(unit,
            entry->Data.SbusDMA.flags,
            entry->Data.SbusDMA.mem,
            entry->Data.SbusDMA.array_index_start,
            entry->Data.SbusDMA.array_index_end,
            entry->Data.SbusDMA.copyno,
            entry->Data.SbusDMA.index_begin,
            entry->Data.SbusDMA.index_end,
            buffer, /* use the dmaable buffer */
            entry->Data.SbusDMA.mem_clear,
            entry->Data.SbusDMA.clear_buf_ent,
            entry->Data.SbusDMA.vchan);

    /* release the dma buffer and the stored buffer */
    soc_cm_sfree(unit, buffer);

    /* no need to free the hw log buffer, dma buffers are purged at the end of the commit of the current transactions */

    return res;
}

int
soc_hw_log_commit(int unit)
{
    HwLogEntry_t   *entry;
    uint32          i;
    soc_error_t     res             = SOC_E_NONE,
                    func_res        = SOC_E_NONE;
    uint8           suspend_commit_counter = 0;

    SOC_INIT_FUNC_DEFS;

    /* If Log is empty --> nothing to commit */
    if (_soc_hw_log_log_list_is_empty(unit)) SOC_EXIT;

    LOG_VERBOSE(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "----------   COMMIT START   ----------: \n")));

    Hw_Log_List[unit].ImmediateAccess = TRUE;

    /* if a crash has occured continue committing log where we left off */
    i = _soc_hw_log_commit_counter_get(unit);

    for (; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
        {
            if(entry->Entry.Data.MemData.DescDMA) {
                res = jer_sbusdma_desc_add_mem(unit, entry->Entry.Data.MemData.Mem, entry->Entry.Data.MemData.ArrayIndex, entry->Entry.Data.MemData.Blk, entry->Entry.Data.MemData.Index, entry->Entry.Data.MemData.EntryData);
                /* Suspend the automatic commit counter increase if there are entries in the descriptor chain. There is always at least one entry in the descriptor chain after desc add so we need to suspend to commit counter */
                suspend_commit_counter = TRUE;
            } else {
                res = soc_mem_array_write(unit, entry->Entry.Data.MemData.Mem, entry->Entry.Data.MemData.ArrayIndex, entry->Entry.Data.MemData.Blk, entry->Entry.Data.MemData.Index, entry->Entry.Data.MemData.EntryData);
            }
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register32)
        {
        	res = soc_reg32_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, entry->Entry.Data.RegData.EntryData[0]);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register64)
        {
        	res = soc_reg64_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, *(uint64 *)(entry->Entry.Data.RegData.EntryData));
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64)
        {
        	res = soc_reg_above_64_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, entry->Entry.Data.RegData.EntryData);
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_Polling)
        {
/*            _soc_hw_log_print_list_polling_entry(unit, &entry->Entry.Data);*/
#ifdef BCM_ARAD_SUPPORT
            res = arad_polling(unit,  entry->Entry.Data.PollingData.TimeOut, entry->Entry.Data.PollingData.MinPolls, entry->Entry.Data.PollingData.Reg, entry->Entry.Data.PollingData.Port, entry->Entry.Data.PollingData.Index,   entry->Entry.Data.PollingData.Field, entry->Entry.Data.PollingData.ExpectedValue);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_DirectRegWrite)
        {
            _soc_hw_log_print_list_direct_reg_entry(unit, &entry->Entry.Data);
#ifdef BCM_ARAD_SUPPORT
            res = soc_direct_reg_set(unit, entry->Entry.Data.DirectRegData.CmicBlock, entry->Entry.Data.DirectRegData.Addr, entry->Entry.Data.DirectRegData.DwcWrite, &entry->Entry.Data.DirectRegData.EntryData);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
        {
/*			_soc_hw_log_print_list_fast_reg_entry(unit, &entry->Entry.Data);*/
#ifdef BCM_ARAD_SUPPORT
            res = arad_fast_reg_set(unit, entry->Entry.Data.FastRegData.Reg, entry->Entry.Data.FastRegData.AccType, entry->Entry.Data.FastRegData.Addr, entry->Entry.Data.FastRegData.Block, entry->Entry.Data.FastRegData.EntryData);
#endif
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData)
        {
/*            _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);*/
        	res = soc_reg_set(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, *(uint64 *)(entry->Entry.Data.RegData.EntryData));
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegNoCacheData)
        {
/*            _soc_hw_log_print_list_soc_reg_entry(unit, &entry->Entry.Data);*/
        	res = soc_reg_set_nocache(unit, entry->Entry.Data.RegData.Reg, entry->Entry.Data.RegData.Port, entry->Entry.Data.RegData.Index, *(uint64 *)(entry->Entry.Data.RegData.EntryData));
        }
        else if (entry->Entry.Type == LOG_ENTRY_TYPE_SbusDMA)
        {
            _soc_hw_log_sbusdma_entry_commit(unit, &entry->Entry);
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "Unit %d: Unknown Entry Type %d FAILED TO COMMIT!!!!.\n"), unit, entry->Entry.Type));
        }


        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED TO COMMIT Entry Type %d!!!.\n"), unit, entry->Entry.Type));

            soc_hw_log_print_single_entry(unit, entry);	/* Print the problematic entry */

            /* continue execution but remember the error */
        	func_res |= res;	/* Remember error result*/
        }
        /* set the index of the next element to be comitted, if commit counter is not suspended */
        if(!suspend_commit_counter) {
            _soc_hw_log_commit_counter_set(unit,i+1);
        }
    }
    
    /* NOTE: clearing the list is the caller responssibility */

    /* start logging again */
    Hw_Log_List[unit].ImmediateAccess = FALSE;

    /* reset counter after the commit is done */
    _soc_hw_log_commit_counter_reset(unit);

    /* flush the descriptor dma chain */
    jer_sbusdma_desc_wait_done(unit);

    /* free the dma buffers */
    _soc_hw_log_sbusdma_buffers_free(unit);

    LOG_VERBOSE(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "---------- END COMMIT ---------- : \n")));

    /* check for the error summarry for all entries */
    _SOC_IF_ERR_EXIT(func_res);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}



STATIC int _soc_hw_log_insert_entry_to_log(int unit, LogEntry_t *data)
{
    HwLogEntry_t *entry = NULL;

    SOC_INIT_FUNC_DEFS;

    if (!soc_hw_log_is_initialized(unit))
    {
        /* The List is not initialized */
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                   (BSL_META_U(unit,
                      "unit:%d The Hw Log List is not initialized, failed to insert.\n"), unit));
    }

    /* Check if The list is full */
    if (Hw_Log_List[unit].header->max_elements == Hw_Log_List[unit].header->nof_elements) {
        /* The journal is full */
        _SOC_EXIT_WITH_ERR(SOC_E_FULL,
                   (BSL_META_U(unit,
                      "unit:%d The Hw Log List is FULL.\n"), unit));
    }

    /* prepare ptr to the next free entry */
    entry = &(Hw_Log_List[unit].journal[Hw_Log_List[unit].header->nof_elements]);
    Hw_Log_List[unit].header->nof_elements++;

    /* Save the new content in the log list */
    sal_memcpy(&(entry->Entry), data, sizeof(LogEntry_t)/*data->data_size*/);

	LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "---   Entry Added to the HW LOG !!!   ---: \n")));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}


STATIC int _soc_hw_log_write_memory_to_log(int unit, soc_mem_t mem,
        unsigned array_index, int copyno, int index, void *entry_data)
{
    LogEntry_t entry;
    uint32 entry_words;

    sal_memset(entry.Data.MemData.EntryData, 0, sizeof(entry.Data.MemData.EntryData));

    entry_words                   = soc_mem_entry_words(unit, mem);
    /* Init the Log entry */
    entry.Data.MemData.Mem        = mem;
    entry.Data.MemData.ArrayIndex = array_index;
    entry.Data.MemData.Blk        = copyno;
    entry.Data.MemData.Index      = index;
    entry.Data.MemData.IsCoreAll  = Hw_Log_List[unit].brcd_blocks[copyno];
	entry.Data.MemData.DescDMA    = FALSE;

    entry.Type                    = LOG_ENTRY_TYPE_Memory;

    sal_memcpy(entry.Data.MemData.EntryData, ((char*) entry_data),  entry_words*sizeof(int));

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_mem_sbusdma_get_size(int unit, soc_mem_t mem, int index_begin, int index_end, uint8 mem_clear)
{
    uint32     entry_dw;
    int        count;

    entry_dw = soc_mem_entry_words(unit, mem);

    if(mem_clear) { /* SLAM DMA */
        count = 1; /* only one entry in the buffer */
    } else {
        if (index_begin > index_end) {
            count = index_begin - index_end + 1;
        } else {
            count = index_end - index_begin + 1;
        }
    }

    return (count * entry_dw * sizeof(uint32));
}

STATIC int
_soc_hw_log_mem_sbusdma_read(int unit, soc_mem_t mem, unsigned array_index,
                            int copyno, int index_begin, int index_end,
                            uint32 ser_flags, void *buffer, int vchan)
{
    HwLogEntry_t    *entry;
    uint32          entry_words;
    int             found   = FALSE;
    soc_mem_t       saved_in_hw_log_mem;
    uint32          i;
    uint32          buff_idx;
    char            *buff_entry;
    int interval_A_start, interval_A_end, interval_B_start, interval_B_end; /* used to calculate overlapping dma intervals */

    /* Get the alias memory. Note: updates 'mem' value. */
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit))
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    /* calculate the lowest and the highest indexes */
    interval_A_start = HWLOG_MIN(index_begin,index_end);
    interval_A_end = HWLOG_MAX(index_begin,index_end);

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
        {
            saved_in_hw_log_mem = entry->Entry.Data.MemData.Mem;
            /* Get the alias memory for saved mem value. Note: updates 'saved_mem' value. */
            SOC_MEM_ALIAS_TO_ORIG(unit, saved_in_hw_log_mem);

            if (   (mem             == saved_in_hw_log_mem)
                && (array_index     == entry->Entry.Data.MemData.ArrayIndex)
                && ((copyno         == entry->Entry.Data.MemData.Blk) || (entry->Entry.Data.MemData.Blk == MEM_BLOCK_ANY) || (entry->Entry.Data.MemData.IsCoreAll))
                && ((interval_A_start <= entry->Entry.Data.MemData.Index) && (entry->Entry.Data.MemData.Index <= interval_A_end)))
            {
                entry_words             = soc_mem_entry_words(unit, mem);

                buff_idx = _soc_hw_log_dma_get_buff_idx(index_begin, index_end,entry->Entry.Data.MemData.Index);  

                buff_entry = (char *)buffer + buff_idx*entry_words*sizeof(int);


                sal_memcpy(((char *) buff_entry), entry->Entry.Data.MemData.EntryData , entry_words*sizeof(int));

                found = TRUE;
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!!! .\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry));
            }
        } else if (entry->Entry.Type == LOG_ENTRY_TYPE_SbusDMA) {
            saved_in_hw_log_mem = entry->Entry.Data.SbusDMA.mem;
            SOC_MEM_ALIAS_TO_ORIG(unit, saved_in_hw_log_mem);

            /* calculate the lowest and highest indexes for stored DMA entry*/
            interval_B_start = HWLOG_MIN(entry->Entry.Data.SbusDMA.index_begin,entry->Entry.Data.SbusDMA.index_end);
            interval_B_end = HWLOG_MAX(entry->Entry.Data.SbusDMA.index_begin,entry->Entry.Data.SbusDMA.index_end);

            if ((mem == saved_in_hw_log_mem) &&
                ((array_index >= entry->Entry.Data.SbusDMA.array_index_start) &&
                 (array_index <= entry->Entry.Data.SbusDMA.array_index_end)) &&
                ((copyno == entry->Entry.Data.SbusDMA.copyno) || (entry->Entry.Data.SbusDMA.copyno == MEM_BLOCK_ANY) || (entry->Entry.Data.SbusDMA.IsCoreAll)) &&
                (HWLOG_MAX(interval_A_start,interval_B_start) <= HWLOG_MIN(interval_A_end,interval_B_end))) /* check if the two intervals are overlapping */ {
                    LOG_ERROR(BSL_LS_SOC_HWLOG,
                        (BSL_META_U(unit, "Unit %d: DMA reads cannot retrieve updated values from journaled DMA entries !!!.\n"),unit));
                    return SOC_E_FAIL;
            }
        }
    }

not_found:
    return found;

}

STATIC int
_soc_hw_log_mem_sbusdma_write(int unit, int flags, soc_mem_t mem,
                             unsigned array_index_start,
                             unsigned array_index_end, int copyno,
                             int index_begin, int index_end,
                             void *buffer, uint8 mem_clear,
                             int clear_buf_ent, int vchan)
{
    LogEntry_t entry;
    void       *hwlog_buffer;

    int alloc_size = _soc_hw_log_mem_sbusdma_get_size(unit, mem, index_begin, index_end, mem_clear);

    hwlog_buffer = _soc_hw_log_sbusdma_buffer_allocate(unit, alloc_size);
    if(NULL == hwlog_buffer) {
        return SOC_E_MEMORY;
    }

    sal_memcpy(hwlog_buffer, buffer, alloc_size);

    entry.Data.SbusDMA.flags                = flags;
    entry.Data.SbusDMA.mem                  = mem;
    entry.Data.SbusDMA.array_index_start    = array_index_start;
    entry.Data.SbusDMA.array_index_end      = array_index_end;
    entry.Data.SbusDMA.copyno               = copyno;
    entry.Data.SbusDMA.index_begin          = index_begin;
    entry.Data.SbusDMA.index_end            = index_end;
    entry.Data.SbusDMA.buffer               = hwlog_buffer; /* non-volatile buffer */
    entry.Data.SbusDMA.mem_clear            = mem_clear;
    entry.Data.SbusDMA.clear_buf_ent        = clear_buf_ent;
    entry.Data.SbusDMA.vchan                = vchan;
    entry.Data.SbusDMA.IsCoreAll            = Hw_Log_List[unit].brcd_blocks[copyno];


    entry.Type                              = LOG_ENTRY_TYPE_SbusDMA;

    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int _soc_hw_log_read_memory_entry_from_log(int unit, soc_mem_t mem,
        unsigned array_index, int copyno, int index, void *entry_data)
{
    HwLogEntry_t    *entry;
    uint32          entry_words;
    int             found   = FALSE;
    soc_mem_t       saved_in_hw_log_mem;
    uint32          i;
    uint32          buff_idx;
    char            *buff_entry;
    uint8           table_dma;
    int interval_A_start,interval_A_end; /* used for dma min and max dma entries indexes */


    /* Get the alias memory. Note: updates 'mem' value. */
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_Memory)
        {
            saved_in_hw_log_mem = entry->Entry.Data.MemData.Mem;
            /* Get the alias memory for saved mem value. Note: updates 'saved_mem' value. */
            SOC_MEM_ALIAS_TO_ORIG(unit, saved_in_hw_log_mem);

            if (    (mem                == saved_in_hw_log_mem)
                    && (array_index     == entry->Entry.Data.MemData.ArrayIndex)
                    && ((copyno         == entry->Entry.Data.MemData.Blk) || (entry->Entry.Data.MemData.Blk == MEM_BLOCK_ANY) || (entry->Entry.Data.MemData.IsCoreAll))
                    && (index           == entry->Entry.Data.MemData.Index))
            {
                entry_words             = soc_mem_entry_words(unit, mem);

                sal_memcpy(((char *) entry_data), entry->Entry.Data.MemData.EntryData , entry_words*sizeof(int));

                found = TRUE;
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!!! .\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry));
            }
        } else if (entry->Entry.Type == LOG_ENTRY_TYPE_SbusDMA) {
            saved_in_hw_log_mem = entry->Entry.Data.SbusDMA.mem;
            SOC_MEM_ALIAS_TO_ORIG(unit, saved_in_hw_log_mem);

            interval_A_start = HWLOG_MIN(entry->Entry.Data.SbusDMA.index_begin,entry->Entry.Data.SbusDMA.index_end);
            interval_A_end = HWLOG_MAX(entry->Entry.Data.SbusDMA.index_begin,entry->Entry.Data.SbusDMA.index_end);


            if ((mem == saved_in_hw_log_mem) &&

                ((array_index >= entry->Entry.Data.SbusDMA.array_index_start) && (array_index <= entry->Entry.Data.SbusDMA.array_index_end)) &&
                ((copyno == entry->Entry.Data.SbusDMA.copyno) || (entry->Entry.Data.SbusDMA.copyno == MEM_BLOCK_ANY) || (entry->Entry.Data.SbusDMA.IsCoreAll)) &&
                ((interval_A_start <= index) && (index <= interval_A_end))) {
                entry_words = soc_mem_entry_words(unit, mem);

                table_dma = (entry->Entry.Data.SbusDMA.mem_clear) ? 0 : 1;

                /* distinguish between Table and SLAM dma */
                if(table_dma) {
                    buff_idx = _soc_hw_log_dma_get_buff_idx(entry->Entry.Data.SbusDMA.index_begin, entry->Entry.Data.SbusDMA.index_end, index);
                } else {
                    buff_idx = 0;
                }

                buff_entry = (char *)entry->Entry.Data.SbusDMA.buffer + buff_idx*entry_words*sizeof(int); /* buff_entry = buffer for slam dma */
                sal_memcpy(((char *) entry_data), buff_entry, entry_words*sizeof(int));

                found = TRUE;
            }
        }
    }

not_found:
    return found;
}


STATIC int
_soc_hw_log_write_register_to_log(int unit, soc_reg_t reg, int port, int index, LogEntry_Type_t type, void * entry_data)
{
    LogEntry_t entry;
    int size = (type == LOG_ENTRY_TYPE_Register32 ? sizeof(uint32) : (((type == LOG_ENTRY_TYPE_Register64) || (type == LOG_ENTRY_TYPE_SocRegData) || (type == LOG_ENTRY_TYPE_SocRegNoCacheData)) ? sizeof(uint64) : sizeof(soc_reg_above_64_val_t)));

    sal_memset(entry.Data.RegData.EntryData, 0, sizeof(soc_reg_above_64_val_t));

    /* Init the Log entry */
    entry.Data.RegData.Reg        = reg;
    entry.Data.RegData.Port       = port;
    entry.Data.RegData.Index      = index;
    entry.Type                    = type;

    sal_memcpy(entry.Data.RegData.EntryData, (int *) entry_data, size);

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_get_size_by_type(LogEntry_Type_t type)
{
    int size;

    switch(type) {
        case LOG_ENTRY_TYPE_Register32:
            size = sizeof(uint32); 
            break;
        case LOG_ENTRY_TYPE_Register64:
            size = sizeof(uint64);
            break;
        case LOG_ENTRY_TYPE_SocRegData:
            size = sizeof(uint64);
            break;
        case LOG_ENTRY_TYPE_SocRegNoCacheData:
            size = sizeof(uint64);
            break;
        default: 
            size = sizeof(soc_reg_above_64_val_t);
    }

    return size;
}

/* check if the arch is little endian */
STATIC int 
_soc_hw_log_is_little_endian(void) {
    int i = 1;
    char *p = (char *)&i;

    if (p[0] == 1)
        return 1;
    else
        return 0;
}

/* Type-independent hwlog register entry copy mechanism. Allows all register entry read mechanisms to be able to retrieve data from all hwlog register entry types */
STATIC int
_soc_hw_log_copy_register_entry_generic(int unit, soc_reg_t reg, LogEntry_Type_t type, void* entry_data, HwLogEntry_t *entry) 
{
    int              source_size, destination_size, actual_size, added_destionation_size, register_size;
    SOC_INIT_FUNC_DEFS;


                /* Calculate source size, destination size and actual size
                 * 1. Destination size - size of the type of the structure to be copied to
                 * 2. Source size - size of type of the entry in the hwlog
                 * 3. Actual size = number of bytes to be copied from source to destination */
                destination_size = _soc_hw_log_get_size_by_type(type);
                source_size      = _soc_hw_log_get_size_by_type(entry->Entry.Type);
                actual_size      = (destination_size < source_size) ? destination_size : source_size;

                /* if the size of the source and destination types is different, we need to consider the size of the register itself */
                if(source_size != destination_size) {
                    /* get the register size */
                    register_size    = (SOC_REG_IS_ABOVE_64(unit,reg) ? sizeof(soc_reg_above_64_val_t) : (SOC_REG_IS_64(unit, reg) ? sizeof(uint64) : sizeof(uint32)));
                    /* if the register size is smaller than the actual size, than use the register size instead.
                     * This is a problem, when above 64 reg read mechanism is trying to retrieve the value of 64 bit entry that contains the value of 32 bit register.
                    */
                    if(register_size < actual_size) {
                        actual_size = register_size;
                    }
                }

                /* the write in above 64 register starts always from the lowest location. This is a problem in big-endian */
                added_destionation_size = (type == LOG_ENTRY_TYPE_Register_Above64) ? 0 : (destination_size - actual_size);

                /* depending on the endianness, use an appropriate mem copy mechanism */
                if (_soc_hw_log_is_little_endian()) {
                    sal_memcpy((uint8*)entry_data, (uint8*)entry->Entry.Data.RegData.EntryData, actual_size);
                } else {
                    sal_memcpy((uint8*)entry_data + added_destionation_size, (uint8*)entry->Entry.Data.RegData.EntryData + source_size - actual_size, actual_size);
                }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_soc_hw_log_read_register_entry_from_log(int unit, soc_reg_t reg, int port, int index, LogEntry_Type_t type, void* entry_data)
{
    HwLogEntry_t    *entry;
    uint32           i;
    int              found   = FALSE;
    soc_error_t      res     = SOC_E_NONE;

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if ((entry->Entry.Type == LOG_ENTRY_TYPE_Register32) 
            || (entry->Entry.Type == LOG_ENTRY_TYPE_Register64) 
            || (entry->Entry.Type == LOG_ENTRY_TYPE_Register_Above64)
            || (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegData)
            || (entry->Entry.Type == LOG_ENTRY_TYPE_SocRegNoCacheData))
        {
            if ((reg == entry->Entry.Data.RegData.Reg)
                    && ((port == entry->Entry.Data.RegData.Port) /* Exact match for lookup*/
                            || (entry->Entry.Data.RegData.Port == REG_PORT_ANY)
                            || (entry->Entry.Data.RegData.Port == SOC_CORE_ALL) /* Stored data is for all cores*/
                            || (port                           == REG_PORT_ANY)
                            || (port                           == SOC_CORE_ALL))    /*requested data is for all cores - we can return data from any port/core*/
                    && (index == entry->Entry.Data.RegData.Index))
            {

                _soc_hw_log_copy_register_entry_generic(unit, reg, type, entry_data, entry); 
                
                found = TRUE;

                LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "REG FOUND in LOG!!! .\n")));
                HW_LOG_DEBUG(_soc_hw_log_print_list_register_entry(unit, entry->Entry.Data));
            }
        }
    }

not_found:
    if (!found)
    {
        Hw_Log_List[unit].ImmediateAccess = TRUE;
        switch (type)
        {
        case LOG_ENTRY_TYPE_Register32:
        	res = soc_reg32_get(unit, reg, port, index, entry_data);
            break;
        case LOG_ENTRY_TYPE_Register64:
        	res = soc_reg64_get(unit, reg, port, index, entry_data);
            break;
        case LOG_ENTRY_TYPE_Register_Above64:
        	res = soc_reg_above_64_get(unit, reg, port, index, entry_data);
            break;
        case LOG_ENTRY_TYPE_SocRegData:
            res = soc_reg_get(unit, reg, port, index, entry_data);
            break;
        default:
            LOG_DEBUG(BSL_LS_SOC_HWLOG, 
                (BSL_META_U(unit, "!! Got into _soc_hw_log_read_register_entry_from_log with type %d, reg %d!! .\n"), type, reg));
        }
        Hw_Log_List[unit].ImmediateAccess = FALSE;

        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED to READ reg %d(%s) port %d index %d !!!.\n"), 
                             unit, reg, SOC_REG_NAME( unit, reg), port, index));
        }
    }

    return res;
}

STATIC int
_soc_hw_log_write_register32_to_log(int unit, soc_reg_t reg, int port, int index, uint32 entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_write_register32_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register32, &entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_register32_entry_from_log(int unit, soc_reg_t reg, int port, int index, uint32 *entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_register32_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register32, entry_data);
    return SOC_E_NONE;

}

STATIC int
_soc_hw_log_write_register64_to_log(int unit, soc_reg_t reg, int port, int index, uint64 entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
                (BSL_META_U(unit, "_soc_hw_log_write_register64_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register64, &entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_register64_entry_from_log(int unit, soc_reg_t reg, int port, int index,
        uint64 *entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_register64_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register64, entry_data);

    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_write_reg_above64_to_log(int unit, soc_reg_t reg, int port, int index,
        soc_reg_above_64_val_t entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_write_reg_above64_to_log!.\n")));

    _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_Register_Above64, entry_data);
    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_read_reg_above64_entry_from_log(int unit, soc_reg_t reg, int port, int index,
        soc_reg_above_64_val_t entry_data)
{
    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_read_reg_above64_entry_from_log!.\n")));

    _soc_hw_log_read_register_entry_from_log( unit, reg, port, index, LOG_ENTRY_TYPE_Register_Above64, entry_data);

    return SOC_E_NONE;
}



STATIC int
_soc_hw_log_polling(int unit, sal_usecs_t time_out, int32 min_polls, soc_reg_t reg, int32 port, int32 index,
        soc_field_t field, uint32 expected_value)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
       (BSL_META_U(unit, "_soc_hw_log_polling!.\n")));


    /* Init the Log entry */
    entry.Data.PollingData.TimeOut      = time_out;
    entry.Data.PollingData.MinPolls     = min_polls;
    entry.Data.PollingData.Reg          = reg;
    entry.Data.PollingData.Port         = port;
    entry.Data.PollingData.Index        = index;
    entry.Data.PollingData.Field        = field;
    entry.Data.PollingData.ExpectedValue= expected_value;
    entry.Type                          = LOG_ENTRY_TYPE_Polling;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_direct_reg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
        (BSL_META_U(unit, "_soc_hw_log_direct_reg_set!.\n")));

    entry.Data.DirectRegData.CmicBlock  = cmic_block;
    entry.Data.DirectRegData.Addr       = addr;
    entry.Data.DirectRegData.DwcWrite   = dwc_write;
    entry.Data.DirectRegData.EntryData  = *data;

    entry.Type                          = LOG_ENTRY_TYPE_DirectRegWrite;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}
STATIC int
_soc_hw_log_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block,
        soc_reg_above_64_val_t data)
{
    LogEntry_t entry;

    LOG_DEBUG(BSL_LS_SOC_HWLOG,
            (BSL_META_U(unit, "_soc_hw_log_direct_reg_set!.\n")));

    entry.Data.FastRegData.Reg        = reg;
    entry.Data.FastRegData.AccType    = acc_type;
    entry.Data.FastRegData.Addr       = addr;
    entry.Data.FastRegData.Block      = block;
    sal_memcpy(entry.Data.FastRegData.EntryData, data, sizeof(soc_reg_above_64_val_t));

    entry.Type                          = LOG_ENTRY_TYPE_FastRegData;

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}

STATIC int
_soc_hw_log_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data)
{
    HwLogEntry_t    *entry          = NULL;
    int             found           = FALSE;
    soc_error_t     res             = SOC_E_NONE;
    uint32          i;

    if (!soc_hw_log_is_initialized(unit) || _soc_hw_log_log_list_is_empty(unit)) 
    {
        /* The List is not initialized or empty */
        goto not_found;
    }

    for (i=0 ; i < Hw_Log_List[unit].header->nof_elements; i++) {

        entry = &(Hw_Log_List[unit].journal[i]);

        if (entry->Entry.Type == LOG_ENTRY_TYPE_FastRegData)
        {
            if (       (reg         == entry->Entry.Data.FastRegData.Reg)
                    && (acc_type    == entry->Entry.Data.FastRegData.AccType)
                    && (addr        == entry->Entry.Data.FastRegData.Addr)
                    && (block       == entry->Entry.Data.FastRegData.Block ))
            {
                sal_memcpy(data, entry->Entry.Data.FastRegData.EntryData, sizeof(soc_reg_above_64_val_t));

                found = TRUE;
                LOG_DEBUG( BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "MEM FOUND in LOG!!! .\n")) );
                HW_LOG_DEBUG(_soc_hw_log_print_list_memory_entry(unit, entry->Entry.Data));
            }
        }
    }

not_found:
    if (!found)
    {
        Hw_Log_List[unit].ImmediateAccess = TRUE;
#ifdef BCM_ARAD_SUPPORT
        res = arad_fast_reg_get(unit, reg,  acc_type, addr, block, data);
#endif /*BCM_ARAD_SUPPORT*/

        Hw_Log_List[unit].ImmediateAccess = FALSE;

        if (SOC_FAILURE(res))
        {
            LOG_ERROR(BSL_LS_SOC_HWLOG,
                            (BSL_META_U(unit, "Unit %d: FAILED to FAST READ reg %d(%s)!\n"),
                             unit, reg, SOC_REG_NAME( unit, reg)));
        }
    }

    return res;

}
STATIC int
_soc_hw_log_soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 entry_data)
{
    return _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_SocRegData, &entry_data);
}

STATIC int
_soc_hw_log_soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
   
    return  _soc_hw_log_read_register_entry_from_log(unit, reg, port, index, LOG_ENTRY_TYPE_SocRegData, data);
}

STATIC int
_soc_hw_log_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 entry_data)
{
    return _soc_hw_log_write_register_to_log(unit, reg, port, index, LOG_ENTRY_TYPE_SocRegNoCacheData, &entry_data);
}

int
soc_hw_log_suppress(int unit)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_UNIT;

    if (soc_dcmn_cr_utils_is_journaling_thread(unit)) {
        /* Indicate that we have reached an unsupported feature */
        Hw_Log_List[unit].is_suppressed = TRUE;
        LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "--- HW LOG SUPPRESSED !!! --- : \n")));
    }

    return SOC_E_NONE;
}

int
soc_hw_log_unsuppress(int unit)
{

    if (!SOC_UNIT_VALID(unit))
        return SOC_E_UNIT;

    if (soc_dcmn_cr_utils_is_journaling_thread(unit)) {
        Hw_Log_List[unit].is_suppressed = FALSE;
        LOG_DEBUG(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "--- HW LOG UNSUPPRESSED !!! --- : \n")));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_hw_log_is_suppressed(int unit)
{
    return (Hw_Log_List[unit].is_suppressed);
}

int
soc_hw_log_ensure_not_suppressed(int unit) 
{
    SOC_INIT_FUNC_DEFS;
    
    if (_soc_hw_log_is_suppressed(unit)) {
        LOG_VERBOSE(BSL_LS_SOC_HWLOG, (BSL_META_U(unit, "!!! ERROR: HW LOG SUPPRESSED WHEN IT SHOULD NOT BE !!! \n")));
        return SOC_E_FAIL;
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

STATIC uint32
_soc_hw_log_commit_counter_get(int unit)
{
    return Hw_Log_List[unit].header->hw_log_commit_index;
}

STATIC int
_soc_hw_log_commit_counter_set(int unit, uint32 counter)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].header->hw_log_commit_index = counter;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int
_soc_hw_log_commit_counter_reset(int unit)
{
    SOC_INIT_FUNC_DEFS;

    _soc_hw_log_commit_counter_set(unit, 0);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC void *
_soc_hw_log_sbusdma_buffer_allocate(int unit, uint32 size)
{
    char *pt = (char *)Hw_Log_List[unit].header;

    /* check if we have sufficient space left to allocate the new dma buffer */
    if(Hw_Log_List[unit].header->dma_allocated_size + size > HW_LOG_DMA_BUFFER_SIZE) {
        return NULL;
    }

    /* increase the allocated dma buffer size */
    Hw_Log_List[unit].header->dma_allocated_size += size;

    /* return a pointer to the newly allocated buffer */
    pt +=  (Hw_Log_List[unit].size - HW_LOG_DMA_BUFFER_SIZE + Hw_Log_List[unit].header->dma_allocated_size);

    return (void *)pt;
}

STATIC int
_soc_hw_log_sbusdma_buffers_free(int unit)
{
    SOC_INIT_FUNC_DEFS;

    Hw_Log_List[unit].header->dma_allocated_size = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

STATIC int _soc_hw_log_descdma_write(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data)
{
    LogEntry_t entry;
    uint32 entry_words;

    sal_memset(entry.Data.MemData.EntryData, 0, sizeof(entry.Data.MemData.EntryData));

    entry_words                   = soc_mem_entry_words(unit, mem);
    /* Init the Log entry */
    entry.Data.MemData.Mem        = mem;
    entry.Data.MemData.ArrayIndex = array_index;
    entry.Data.MemData.Blk        = copyno;
    entry.Data.MemData.Index      = index;
    entry.Data.MemData.DescDMA    = TRUE;

    entry.Type                    = LOG_ENTRY_TYPE_Memory;

    sal_memcpy(entry.Data.MemData.EntryData, ((char*) entry_data),  entry_words*sizeof(int));

    /*Insert the log entry*/
    return _soc_hw_log_insert_entry_to_log(unit, &entry);
}


/********************************************************************************/
/***********************Diag & Tests*********************************************/
/********************************************************************************/
int
soc_hw_log_mem_test(int unit, soc_mem_t mem, void* data)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_NONE;

    soc_mem_single_test(unit, mem);

    return SOC_E_NONE;
}

int
soc_hw_log_reg_test(int unit)
{
    if (!SOC_UNIT_VALID(unit))
        return SOC_E_NONE;

    reg_test(unit, NULL, NULL); /* TR 3 */
    return SOC_E_NONE;
}

#endif /* CRASH_RECOVERY_SUPPORT */
