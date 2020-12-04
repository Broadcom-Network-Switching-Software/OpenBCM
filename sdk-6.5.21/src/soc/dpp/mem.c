/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC memory access implementation for DPP
 */

#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_MEM

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/cmic.h>


typedef struct soc_dpp_mem_access_info_s {
    int     block;                  
    int     block_offset;           
    int     wr_reg_offset;          
    int     rd_reg_offset;          
    int     wr_rd_width;            
    int     cmd_offset;             
    int     cmd_addr_offset;        
    int     cmd_data_incr_offset;   
} soc_dpp_mem_access_info_t;

CONST soc_dpp_mem_access_info_t soc_petrab_mem_access_info [] = {
    {SOC_BLK_CFC,   0x4600, 0x20,   0x30,   44,     0x40,   0x41,   0x42},
    {SOC_BLK_DPI,   0x0c00, 0x20,   0x30,   56,     0x40,   0x41,   0x0},
    {SOC_BLK_DRC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_ECI,   0x0,    0x40,   0x50,   36,     0x60,   0x61,   0x62},
    {SOC_BLK_EGQ,   0x5800, 0x20,   0x30,   256,    0x40,   0x41,   0x42},
    {SOC_BLK_EPNI,  0x3a00, 0x20,   0x30,   256,    0x40,   0x41,   0x42},
    {SOC_BLK_FCR,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_FCT,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MMU,   0x0a00, 0x20,   0x30,   512,    0x40,   0x41,   0x42},
    {SOC_BLK_FDR,   0x0,    0x0,    0x0,     0,     0x0,    0x0,    0x0},
    {SOC_BLK_FDT,   0x2c00, 0x0,    0x30,   244,    0x40,   0x41,   0x0},
    {SOC_BLK_MESH_TOPOLOGY,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_IDR,   0x2800, 0x20,   0x30,   245,    0x40,   0x41,   0x42},
    {SOC_BLK_IHB,   0x6400, 0x20,   0x30,   309,    0x40,   0x41,   0x42},
    {SOC_BLK_IHP,   0x6000, 0x20,   0x30,   309,    0x40,   0x41,   0x42},
    {SOC_BLK_IPS,   0x0200, 0x20,   0x30,   64,     0x40,   0x41,   0x42},
    {SOC_BLK_IPT,   0x0800, 0x20,   0x30,   512,    0x40,   0x41,   0x42},
    {SOC_BLK_IQM,   0x0400, 0x20,   0x30,   180,    0x40,   0x41,   0x42},
    {SOC_BLK_IRE,   0x2400, 0x20,   0x30,   128,    0x40,   0x41,   0x42},
    {SOC_BLK_IRR,   0x2a00, 0x20,   0x30,   0,      0x40,   0x41,   0x42},
    {SOC_BLK_MAC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MBU,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_MCC,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_NBI,   0x4a00, 0x20,   0x30,   128,    0x40,   0x41,   0x42},
    {SOC_BLK_NIF,   0x0,    0x0,    0x0,    0,      0x0,    0x0,    0x0},
    {SOC_BLK_OLP,   0x4400, 0x20,   0x30,   32,     0x40,   0x41,   0x42},
    {SOC_BLK_QDR,   0x0600, 0x20,   0x30,   32,     0x40,   0x41,   0x42},
    {SOC_BLK_RTP,   0x3600, 0x20,   0x30,   72,     0x40,   0x41,   0x42},
    {SOC_BLK_SCH,   0x4200, 0x20,   0x30,   32,     0x40,   0x41,   0x42}
};

#define SOC_PETRAB_MEM_ACCESS_INFO_LEN (sizeof(soc_petrab_mem_access_info)/ \
                                        sizeof(soc_dpp_mem_access_info_t))

#define SOC_PETRAB_INDIRECT_CMD_TYPE_READ       (0x80000000)
#define SOC_PETRAB_INDIRECT_CMD_TYPE_WRITE_MASK (0x7fffffff)
#define SOC_PETRAB_INDIRECT_CMD_TRIGGER         (0x1)
#define SOC_PETRAB_INDIRECT_CMD_TIMEOUT         (10000000)  
int
_soc_dpp_indirect_mem_access_info_get(int unit, soc_mem_t mem, 
                                CONST soc_dpp_mem_access_info_t **pp_mem_access_info)
{
    int             idx;
    soc_block_t     blk;
    
    if (pp_mem_access_info == NULL) {
        return SOC_E_PARAM;
    }

    blk = SOC_MEM_BLOCK_ANY(unit, mem);
    if ((blk <= 0) || (blk > SOC_MAX_NUM_BLKS)) {
        return SOC_E_INTERNAL;
    }
    for (idx = 0; idx < SOC_PETRAB_MEM_ACCESS_INFO_LEN; idx++) {
        if (soc_petrab_mem_access_info[idx].block == 
            (SOC_BLOCK_INFO(unit, blk).type)) {
            if (soc_petrab_mem_access_info[idx].wr_rd_width == 0) {
                return SOC_E_PARAM;
            }
            *pp_mem_access_info = &soc_petrab_mem_access_info[idx];
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

int
_soc_dpp_indirect_mem_access_trigger(int unit, int trig_offset)
{
    int     rv;
    uint32          data;
    sal_usecs_t     cur_time, start_time;

    
    rv = soc_reg32_read(unit, trig_offset, &data);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access failed(%s). "
                             "Unable to trigger op\n"), soc_errmsg(rv)));
        return rv;
    }
    if (data & SOC_PETRAB_INDIRECT_CMD_TRIGGER) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access aborted. "
                             "trigger already set.\n")));
        return SOC_E_BUSY;
    }

    data |= SOC_PETRAB_INDIRECT_CMD_TRIGGER;
    rv = soc_reg32_write(unit, trig_offset, data);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Indirect memory access failed(%s). "
                             "Unable to trigger op\n"), soc_errmsg(rv)));
        return rv;
    }

    
    start_time = sal_time_usecs();
    do {
        rv = soc_reg32_read(unit, trig_offset, &data);
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_MEM,
                     (BSL_META_U(unit,
                                 "Indirect memory access failed(%s). "
                                 "Unable to read trigger status\n"), soc_errmsg(rv)));
            return rv;
        }
        if (data & SOC_PETRAB_INDIRECT_CMD_TRIGGER) {
           LOG_VERBOSE(BSL_LS_SOC_MEM,
                       (BSL_META_U(unit,
                                   "Waiting for trigger operation"
                                   " to finish \n")));
            sal_usleep(50);
        } else {
            return SOC_E_NONE;
        }
        cur_time = sal_time_usecs();
    } while ((cur_time - start_time) < SOC_PETRAB_INDIRECT_CMD_TIMEOUT);

    return SOC_E_TIMEOUT;
}

int
_soc_dpp_indirect_mem_access(int unit, soc_mem_t mem, int copyno, int index, 
                             void *entry_data, int mem_op)
{
    soc_mem_info_t              meminfo = SOC_MEM_INFO(unit, mem);
    CONST soc_dpp_mem_access_info_t   *p_acc_info;
    int                         words_to_access, words_accessed;
    int                         rv;
    uint32                      data, tbl_entry_addr;
    uint32                      addr_reg_offset, data_buf_offset, trig_offset;

    rv = _soc_dpp_indirect_mem_access_info_get(unit, mem, &p_acc_info);
    if (SOC_FAILURE(rv)) {
        return SOC_E_PARAM; 
    }

    
    words_to_access = BYTES2WORDS(meminfo.bytes);
    tbl_entry_addr = (meminfo.base & 0xffffff) + (words_to_access * index);
    switch (mem_op) {
    case MEM_TABLE_READ:
        tbl_entry_addr |= SOC_PETRAB_INDIRECT_CMD_TYPE_READ;

        
        data_buf_offset = (p_acc_info->block_offset + 
                            p_acc_info->rd_reg_offset);
        break;
    case MEM_TABLE_WRITE:
        tbl_entry_addr &= SOC_PETRAB_INDIRECT_CMD_TYPE_WRITE_MASK;

        
        data_buf_offset = (p_acc_info->block_offset + 
                           p_acc_info->wr_reg_offset);
        break;
    default:
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "Invalid operation specified\n")));
        return SOC_E_PARAM;
    }
    
    addr_reg_offset = (p_acc_info->block_offset + 
                       p_acc_info->cmd_addr_offset);

    
    trig_offset = (p_acc_info->block_offset + 
                   p_acc_info->cmd_offset);

    words_accessed = 0;
    while (words_accessed < words_to_access) {
        if (mem_op == MEM_TABLE_READ) {
            
            data = tbl_entry_addr + words_accessed;
            rv = soc_reg32_write(unit, addr_reg_offset, data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to set read address\n"), soc_errmsg(rv)));
                return rv;
            }

            
            rv = _soc_dpp_indirect_mem_access_trigger(unit, trig_offset);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Trigger operation failed.\n"), soc_errmsg(rv)));
                return rv;
            }

            
            rv = soc_reg32_read(unit, data_buf_offset, &data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to read value\n"), soc_errmsg(rv)));
                return rv;
            }
            sal_memcpy((((char *)entry_data) + (words_accessed * sizeof(data))), 
                       &data,
                       sizeof(data));
        } else {
            
            
            sal_memcpy(&data,
                       (((char *)entry_data) + (words_accessed * sizeof(data))),
                       sizeof(data));
            rv = soc_reg32_write(unit, data_buf_offset, data);
           LOG_VERBOSE(BSL_LS_SOC_MEM,
                       (BSL_META_U(unit,
                                   "Writing data:0x%x to memory "
                                   "address 0x%x\n"),data, data_buf_offset));
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to write value\n"), soc_errmsg(rv)));
                return rv;
            }

            
            data = tbl_entry_addr + words_accessed;
            rv = soc_reg32_write(unit, addr_reg_offset, data);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Unable to set write address\n"), soc_errmsg(rv)));
                return rv;
            }

            
            rv = _soc_dpp_indirect_mem_access_trigger(unit, trig_offset);
            if (SOC_FAILURE(rv)) {
               LOG_ERROR(BSL_LS_SOC_MEM,
                         (BSL_META_U(unit,
                                     "Indirect memory access failed(%s). "
                                     "Trigger operation failed.\n"), soc_errmsg(rv)));
                return rv;
            }
        }
        words_accessed++;
    }

    return rv;
}

int
_soc_dpp_mem_access_valid(int unit, soc_mem_t mem, int copyno, int index)
{
    if (!soc_mem_is_valid(unit, mem)) {
        return SOC_E_MEMORY;
    }
    if (copyno == MEM_BLOCK_ANY) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }
    if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "invalid block %d for memory %s\n"),
                             copyno, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }
    if (!((index >= 0) && (index <= soc_mem_index_max(unit, mem)))) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "invalid index %d for memory %s\n"),
                             index, SOC_MEM_NAME(unit, mem)));
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_dpp_mem_read(int unit, soc_mem_t mem, int copyno, int index, void *entry_data)
{
    int rv;

    if (entry_data == NULL) {
        return SOC_E_PARAM;
    }

    rv = _soc_dpp_mem_access_valid(unit, mem, copyno, index);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s failed: Invalid inputs\n"), FUNCTION_NAME()));
        return rv;
    }

    rv = _soc_dpp_indirect_mem_access(unit, mem, copyno, index, entry_data, 
                                      MEM_TABLE_READ);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "soc_mem_read operation failed to read from "
                             "index %d memory %s \n"), index, SOC_MEM_NAME(unit, mem)));
    }

    return rv;
}

int
soc_dpp_mem_write(int unit, soc_mem_t mem, int copyno, int index, void *entry_data)
{
    int rv;

    if (entry_data == NULL) {
        return SOC_E_PARAM;
    }

    rv = _soc_dpp_mem_access_valid(unit, mem, copyno, index);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s failed: Invalid inputs\n"), FUNCTION_NAME()));
        return rv;
    }

    rv = _soc_dpp_indirect_mem_access(unit, mem, copyno, index, entry_data, 
                                      MEM_TABLE_WRITE);
    if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%s operation failed to write to "
                             "index %d memory %s \n"), FUNCTION_NAME(), index, 
                  SOC_MEM_NAME(unit, mem)));
    }

    return rv;
}
