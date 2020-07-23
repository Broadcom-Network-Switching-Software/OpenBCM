/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Crash Recovery Mechanism.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */

#ifndef _HW_LOG_H
#define _HW_LOG_H

#ifdef CRASH_RECOVERY_SUPPORT
#include <sal/core/thread.h>
#include <sal/core/time.h>
#include <soc/register.h>

extern int
soc_hw_log_get_working_mode(int unit);

extern int
soc_hw_log_do_hwlog_read_write(int unit);

#endif /* CRASH_RECOVERY_SUPPORT */

extern int
soc_hw_log_suppress(int unit);

extern int
soc_hw_log_unsuppress(int unit);

extern int
soc_hw_log_ensure_not_suppressed(int unit);

#if defined(BCM_PETRA_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
#define         BCM_UNIT_DO_HW_READ_WRITE(unit)                 soc_hw_log_do_hwlog_read_write(unit)
#else
#define         BCM_UNIT_DO_HW_READ_WRITE(unit)                 (0)
#endif

#ifdef CRASH_RECOVERY_SUPPORT

#define HWLOG_MAX(a,b) ((a) > (b) ? (a) : (b))
#define HWLOG_MIN(a,b) ((a) < (b) ? (a) : (b))

#define HW_LOG_DMA_BUFFER_SIZE 1048576

typedef enum LogEntry_Type
{
    LOG_ENTRY_TYPE_UnKnown = 0,
    LOG_ENTRY_TYPE_Memory,
    LOG_ENTRY_TYPE_Register32,
    LOG_ENTRY_TYPE_Register64,
    LOG_ENTRY_TYPE_Register_Above64,
    LOG_ENTRY_TYPE_Polling,
    LOG_ENTRY_TYPE_FastRegData,
    LOG_ENTRY_TYPE_DirectRegWrite,
    LOG_ENTRY_TYPE_SocRegData,
    LOG_ENTRY_TYPE_SocRegNoCacheData,
    LOG_ENTRY_TYPE_SbusDMA
} LogEntry_Type_t;

typedef struct SbusDMA
{
    int         flags;
    soc_mem_t   mem;
    unsigned    array_index_start;
    unsigned    array_index_end;
    int         copyno;
    int         index_begin;
    int         index_end;
    void        *buffer;
    uint8       mem_clear;
    int         clear_buf_ent;
    int         vchan;
    uint8       IsCoreAll;
} SbusDMA_t;

typedef struct MemData
{
    soc_mem_t               Mem;
    uint32                  ArrayIndex;
    int                     Blk /*(copyno)*/;
    uint8                   IsCoreAll;
    int                     Index;
    char                    EntryData[SOC_REG_ABOVE_64_MAX_SIZE_U32 * sizeof(int)];
    uint8                   DescDMA;
} MemData_t;

typedef struct RegData
{
    soc_reg_t               Reg;
    int                     Port;
    int                     Index;
    soc_reg_above_64_val_t  EntryData;
} RegData_t;

typedef struct PollingData
{
    sal_usecs_t             TimeOut;
    int32                   MinPolls;
    soc_reg_t               Reg;
    int32                   Port;
    int32                   Index;
    soc_field_t             Field;
    int32                   ExpectedValue;
} PollingData_t;

typedef struct DirectReg_Data_s
{
    int                     CmicBlock;
    uint32                  Addr;
    uint32                  DwcWrite;
    uint32                  EntryData;
} DirectRegData_t;

typedef struct FastReg_Data_s
{
    soc_reg_t               Reg;
    int                     AccType;
    int                     Addr;
    int                     Block;
    soc_reg_above_64_val_t  EntryData;
} FastReg_Data_t;

typedef union LogEntry_Data
{
    MemData_t               MemData;
    RegData_t               RegData;
    PollingData_t           PollingData;
    DirectRegData_t         DirectRegData;
    FastReg_Data_t          FastRegData;
    SbusDMA_t               SbusDMA;
} LogEntry_Data_t;

typedef struct LogEntry_s
{
    LogEntry_Type_t   Type;
    LogEntry_Data_t   Data;
} LogEntry_t;

typedef struct HwLogEntry_s
{
    LogEntry_t Entry;
} HwLogEntry_t;

typedef struct hw_log_header_s {
    uint32 max_elements;
    uint32 nof_elements;
    uint32 hw_log_commit_index;
    uint32 dma_allocated_size;
} hw_log_header_t;

typedef struct LogList
{
    uint8               init;
    hw_log_header_t    *header;
    HwLogEntry_t       *journal;
    uint32              size;
    uint8               ImmediateAccess;    /* Bypass Hw Log */
    uint32              access_ct;
    uint8               brcd_blocks[SOC_MAX_NUM_BLKS];
    uint8               is_suppressed;      /* Commit and bypass Hw Log to the end of current API */
    soc_hw_log_access_t Access_cb;          /* Access callbacks*/
} LogList_t;

extern int
soc_hw_log_init(int unit, uint32 size);

extern int
soc_hw_log_deinit(int unit);

extern int
soc_hw_log_commit(int unit);

extern int
soc_hw_log_sync(int unit);

extern int
soc_hw_log_purge(int unit);

extern int
soc_hw_log_mem_test(int unit, soc_mem_t mem, void* data);

extern int
soc_hw_log_print_list(int unit);

extern int
soc_hw_log_reg_test(int unit);

extern int
soc_mem_single_test(int unit, soc_mem_t mem);

extern int 
soc_hw_reset_immediate_hw_access_counter(int unit);

extern int
soc_hw_set_immediate_hw_access(int unit);

extern int
soc_hw_restore_immediate_hw_access(int unit);

extern int
soc_hw_ensure_immediate_hw_access_disabled(int unit);

extern LogList_t Hw_Log_List[SOC_MAX_NUM_DEVICES];

#endif 
#endif /*_HW_LOG_H*/
