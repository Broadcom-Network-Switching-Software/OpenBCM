/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FIFO DMA
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/cmic.h>
#include <soc/cmicx.h>
#include <soc/fifodma.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fifo_dma.h>

#include <soc/dnxf/ramon/ramon_fifo_dma.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

#define _SOC_RAMON_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE         (80) 
#define _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MAX_ENTRIES        (0x4000)
#define _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MIN_ENTRIES        (0x40)
#define _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MAX_BUFFER_SIZE        (_SOC_RAMON_FIFO_DMA_FABRIC_CELL_MAX_ENTRIES * _SOC_RAMON_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE)
#define _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MIN_BUFFER_SIZE        (_SOC_RAMON_FIFO_DMA_FABRIC_CELL_MIN_ENTRIES * _SOC_RAMON_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE)

typedef enum soc_ramon_fifo_dma_channel_e
{
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_0 = 0,
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_1 = 1,
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_2 = 2, 
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_3 = 3,
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_4 = 4,
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_5 = 5,
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_6 = 6, 
    soc_ramon_fifo_dma_channel_fabric_cell_dcml_7 = 7
} soc_ramon_fifo_dma_channel_t;

static shr_error_e
soc_ramon_fifo_dma_channel_enable(int unit, int channel)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    
    rv = soc_fifodma_status_clear(unit, channel);
    SHR_IF_ERR_EXIT(rv);

    
    rv = soc_fifodma_intr_enable(unit, channel);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_ramon_fifo_dma_channel_clear(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info)
{
    int rv;
    int num_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    rv = soc_fifodma_num_entries_get(unit, channel, &num_entries);
    if (rv != _SHR_E_NONE && rv != _SHR_E_EMPTY)
    {
        SHR_IF_ERR_EXIT(rv);
    }

    
    SHR_IF_ERR_EXIT(soc_fifodma_set_entries_read(unit, channel, num_entries));

    
    fifo_dma_info->nof_unread_entries = 0;
    fifo_dma_info->current_entry_id = 0;
    fifo_dma_info->read_entries = 0;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fifo_dma_channel_init(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info)
{
    int blk;
    uint32 reg,
        fifo_dma_timeout,
        host_mem_size_in_bytes;
    int rv;
    uint64 reg64_val;
    int max_entries;
    schan_msg_t msg;
    int dont, care, endian;

    SHR_FUNC_INIT_VARS(unit);

    
    switch (channel)
    {
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_0:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_1:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_2:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_3:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_4:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_5:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_6:
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_7:
            fifo_dma_info->config.entry_size = _SOC_RAMON_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE;
            fifo_dma_info->config.is_mem = TRUE;
            fifo_dma_info->config.mem = DCML_AUTO_DOC_NAME_55m;
            fifo_dma_info->config.reg = INVALIDr;
            max_entries = dnxf_data_fabric.cell.fifo_dma_buffer_size_get(unit) / fifo_dma_info->config.entry_size;
            
            fifo_dma_info->config.max_entries = _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MIN_ENTRIES;
            while (fifo_dma_info->config.max_entries * 2 < max_entries
                   && fifo_dma_info->config.max_entries * 2 < _SOC_RAMON_FIFO_DMA_FABRIC_CELL_MAX_ENTRIES)
            {
                fifo_dma_info->config.max_entries *= 2;
            }

           break;

       default:
           SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFO DMA channel is not supported\n");
           break;
    }
  
    
    SHR_IF_ERR_EXIT(READ_ECI_FIFO_DMA_SELr(unit, &reg64_val));
    switch (channel)
    {
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_0:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_0_SELf, 0x0);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_1:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_1_SELf, 0x1);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_2:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_2_SELf, 0x2);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_3:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_3_SELf, 0x3);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_4:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_4_SELf, 0x4);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_5:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_5_SELf, 0x5);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_6:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_6_SELf, 0x6);
            break;
        case soc_ramon_fifo_dma_channel_fabric_cell_dcml_7:
            soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_7_SELf, 0x7);
            break;
    }
    SHR_IF_ERR_EXIT(WRITE_ECI_FIFO_DMA_SELr(unit, reg64_val));

    
    host_mem_size_in_bytes = fifo_dma_info->config.entry_size * fifo_dma_info->config.max_entries;
    fifo_dma_info->buffer =  soc_cm_salloc(unit, host_mem_size_in_bytes, "FIFO DMA"); 
    LOG_VERBOSE(BSL_LS_SOC_FABRIC,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!fifo_dma_info->buffer) {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Memory allocation failed!\n");
    }
    sal_memset(fifo_dma_info->buffer, 0x0,  host_mem_size_in_bytes);

    
    rv = soc_fifodma_stop(unit, channel);
    SHR_IF_ERR_EXIT(rv);

    
    rv = soc_fifodma_start(unit,
                           channel, 
                           TRUE ,
                           fifo_dma_info->config.mem,
                           0,
                           channel,
                           fifo_dma_info->config.entry_size,
                           fifo_dma_info->config.max_entries, 
                           fifo_dma_info->buffer);
    SHR_IF_ERR_EXIT(rv);

    
    blk = SOC_BLOCK2SCH(unit, SOC_INFO(unit).dcml_block[channel]);
    schan_msg_clear(&msg);
    soc_schan_header_cmd_set(unit, &msg.header, READ_MEMORY_CMD_MSG, blk, 0, 0, fifo_dma_info->config.entry_size, 1, 0);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_OPCODE_OFFSET(channel), msg.dwords[0]);

    
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(channel));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_HOSTMEM_THRESHOLDr, &reg, THRESHOLDf,
                      dnxf_data_fabric.cell.fifo_dma_threshold_get(unit));
    rv = soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(channel), reg);
    SHR_IF_ERR_EXIT(rv);

    
    fifo_dma_timeout = dnxf_data_fabric.cell.fifo_dma_timeout_get(unit);
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(channel));
    CMIC_COMMON_POOL_FIFO_USEC_TO_DMA_CFG_TIMEOUT_COUNT(fifo_dma_timeout);
    fifo_dma_timeout &= 0x3fff;
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, fifo_dma_timeout);
    rv = soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(channel), reg);
    SHR_IF_ERR_EXIT(rv);

    fifo_dma_timeout = dnxf_data_fabric.cell.fifo_dma_timeout_get(unit);
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_1_OFFSET(channel));
    CMIC_COMMON_POOL_FIFO_USEC_TO_DMA_CFG_TIMEOUT_COUNT(fifo_dma_timeout);
    fifo_dma_timeout &= 0xFFFFC000;
    fifo_dma_timeout >>= 14;
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFG_1r, &reg, TIMEOUT_COUNT_MSB_BITSf, fifo_dma_timeout);
    rv = soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_1_OFFSET(channel), reg);
    SHR_IF_ERR_EXIT(rv);

    
    soc_endian_get(unit, &dont, &care, &endian);
    SHR_IF_ERR_EXIT(soc_fifodma_ch_endian_set(unit, channel, (endian != 0)));

    
    rv = soc_ramon_fifo_dma_channel_enable(unit, channel);
    SHR_IF_ERR_EXIT(rv);

    
    rv = soc_ramon_fifo_dma_channel_clear(unit, channel, fifo_dma_info);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fifo_dma_channel_deinit(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_CONT(soc_fifodma_stop(unit, channel));

    
    if (fifo_dma_info->buffer != NULL) {
        soc_cm_sfree(unit, fifo_dma_info->buffer);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_fifo_dma_channel_read_entries(int unit, int channel, soc_dnxf_fifo_dma_t *fifo_dma_info)
{
    int num_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT_EXCEPT_IF(soc_fifodma_num_entries_get(unit, channel, &num_entries), _SHR_E_EMPTY);

    if (num_entries >= fifo_dma_info->read_entries)
    {
        
        SHR_IF_ERR_EXIT(soc_fifodma_set_entries_read(unit, channel, fifo_dma_info->read_entries));
        
        fifo_dma_info->nof_unread_entries += fifo_dma_info->read_entries;
    } else {
        SHR_IF_ERR_EXIT(soc_ramon_fifo_dma_channel_enable(unit, channel));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fifo_dma_fabric_cell_validate(int unit)
{
    uint32 host_mem_num_entries = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_fabric.cell.fifo_dma_enable_get(unit))
    {
        
        host_mem_num_entries =
            dnxf_data_fabric.cell.fifo_dma_buffer_size_get(unit) / _SOC_RAMON_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE;
        if (host_mem_num_entries < dnxf_data_fabric.cell.fifo_dma_threshold_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "fabric_cell_fifo_dma_buffer size is small compared to fabric_cell_fifo_dma_threhold");
        }
        
        
        if (dnxf_data_fabric.cell.fifo_dma_buffer_size_get(unit) <
            dnxf_data_fabric.cell.fifo_dma_min_nof_entries_get(unit) * dnxf_data_fabric.cell.fifo_dma_entry_size_get(unit)
            || dnxf_data_fabric.cell.fifo_dma_buffer_size_get(unit) >
            dnxf_data_fabric.cell.fifo_dma_max_nof_entries_get(unit) * dnxf_data_fabric.cell.fifo_dma_entry_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric_cell_fifo_dma_buffer_size is out of range (%d)",
                         dnxf_data_fabric.cell.fifo_dma_buffer_size_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
