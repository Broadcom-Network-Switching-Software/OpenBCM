/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FIFO DMA
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/mcm/memregs.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/cmic.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_88950

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fifo_dma.h>

#include <soc/dfe/fe3200/fe3200_fifo_dma.h>

#define _SOC_FE3200_FIFO_DMA_THRESHOLD                      (2) 
#define _SOC_FE3200_FIFO_DMA_TIME_OUT                       (1000)
#define _SOC_FE3200_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE         (80)    
#define _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MAX_ENTRIES        (0x4000)
#define _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MIN_ENTRIES        (0x20)
#define _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MAX_BUFFER_SIZE        (_SOC_FE3200_FIFO_DMA_FABRIC_CELL_MAX_ENTRIES * _SOC_FE3200_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE)
#define _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MIN_BUFFER_SIZE        (_SOC_FE3200_FIFO_DMA_FABRIC_CELL_MIN_ENTRIES * _SOC_FE3200_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE)


soc_error_t
soc_fe3200_fifo_dma_channel_enable(int unit, int channel)
{
    uint32 reg;
    int rv, cmc;
    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);

    
    reg = 0;
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &reg, HOSTMEM_TIMEOUTf, 1);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &reg, HOSTMEM_OVERFLOWf, 1);
    rv = soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, channel), reg);
    SOCDNX_IF_ERR_EXIT(rv);
    
    soc_cmicm_intr0_enable(unit, IRQ_CMCx_FIFO_CH_DMA(channel));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fifo_dma_channel_init(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info)
{
    int cmc, blk;
    uint32 reg,
        fifo_dma_timeout,
        host_mem_size_in_bytes;
    int rv;
    uint64 reg64_val;
    int max_entries;
    schan_msg_t msg;
    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);

    
    switch (channel)
    {
       case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_0:
       case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_1:
       case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_2:
       case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_3:
           fifo_dma_info->config.entry_size = _SOC_FE3200_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE;
           fifo_dma_info->config.is_mem = TRUE;
           fifo_dma_info->config.mem = DCL_CPU_Hm;
           fifo_dma_info->config.reg = INVALIDr;
           max_entries = SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size / fifo_dma_info->config.entry_size;
           
           fifo_dma_info->config.max_entries = 64;
           while (fifo_dma_info->config.max_entries * 2 < max_entries && fifo_dma_info->config.max_entries * 2 < 16384)
           {
               fifo_dma_info->config.max_entries *= 2;
           }

           break;

       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR("FIFO DMA channel is not supported\n")));
           break;

    }
  
    
    SOCDNX_IF_ERR_EXIT(READ_ECI_FIFO_DMA_SELr(unit, &reg64_val));
    switch (channel)
    {
    case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_0:
        soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_0_SELf, 0x0);
        break;
    case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_1:
        soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_1_SELf, 0x1);
        break;
    case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_2:
        soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_2_SELf, 0x2);
        break;
    case soc_fe3200_fifo_dma_channel_fabric_cell_dcl_3:
        soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, FIFO_DMA_3_SELf, 0x3);
        break;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FIFO_DMA_SELr(unit, reg64_val));

    
    host_mem_size_in_bytes = fifo_dma_info->config.entry_size * fifo_dma_info->config.max_entries;
    fifo_dma_info->buffer =  soc_cm_salloc(unit, host_mem_size_in_bytes, "FIFO DMA"); 
    LOG_VERBOSE(BSL_LS_SOC_FABRIC,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!fifo_dma_info->buffer) {
		SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG_STR("Memory allocation failed!\n")));
    }
    sal_memset(fifo_dma_info->buffer, 0x0,  host_mem_size_in_bytes);

    
    rv = _soc_mem_sbus_fifo_dma_stop(unit, channel);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = _soc_mem_sbus_fifo_dma_start_memreg(unit, 
                                              channel,
                                              fifo_dma_info->config.is_mem, 
                                              fifo_dma_info->config.mem, 
                                              fifo_dma_info->config.reg, channel,
                                              fifo_dma_info->config.entry_size,
                                              fifo_dma_info->config.max_entries, 
                                              fifo_dma_info->buffer);
    SOCDNX_IF_ERR_EXIT(rv);

    
    blk = SOC_BLOCK2SCH(unit, SOC_INFO(unit).dcl_block[channel]);
    schan_msg_clear(&msg);
    soc_schan_header_cmd_set(unit, &msg.header, READ_MEMORY_CMD_MSG, blk, 0, 0, fifo_dma_info->config.entry_size, 1, 0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(cmc, channel), msg.dwords[0]);


    
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, channel));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_HOSTMEM_THRESHOLDr, &reg, THRESHOLDf, SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold );
    rv = soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, channel), reg);
    SOCDNX_IF_ERR_EXIT(rv);

    
    fifo_dma_timeout = SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout;
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, channel));
    MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(fifo_dma_timeout);
    fifo_dma_timeout &= 0x3fff;
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, fifo_dma_timeout);
    rv = soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, channel), reg);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_fe3200_fifo_dma_channel_enable(unit, channel);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fifo_dma_channel_deinit(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(_soc_mem_sbus_fifo_dma_stop(unit, channel));

    
    if (fifo_dma_info->buffer != NULL) {
        soc_cm_sfree(unit, fifo_dma_info->buffer);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fifo_dma_channel_read_entries(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info)
{
    uint32 reg;
    int cmc;
    uint32 num_entries;
    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);

    if (fifo_dma_info->nof_unread_entries == 0)
    {
        
        reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(cmc, channel));;
        num_entries = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr, reg, ENTRYCOUNTf);

        if (num_entries >= fifo_dma_info->read_entries)
        {
            
            SOCDNX_IF_ERR_EXIT(_soc_mem_sbus_fifo_dma_set_entries_read(unit, channel, fifo_dma_info->read_entries));
                        
            fifo_dma_info->nof_unread_entries = fifo_dma_info->read_entries;
        } else {
           
            SOCDNX_IF_ERR_EXIT(soc_fe3200_fifo_dma_channel_enable(unit, channel));

        }
        

    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fifo_dma_channel_clear(int unit, int channel, soc_dfe_fifo_dma_t *fifo_dma_info)
{
    uint32 reg;
    int cmc,
        num_entries;
    SOCDNX_INIT_FUNC_DEFS;

    cmc = SOC_PCI_CMC(unit);

    
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(cmc, channel));;
    num_entries = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEMr, reg, ENTRYCOUNTf);

    
    SOCDNX_IF_ERR_EXIT(_soc_mem_sbus_fifo_dma_set_entries_read(unit, channel, num_entries));

    
    fifo_dma_info->nof_unread_entries = 0;
    fifo_dma_info->current_entry_id += (num_entries - fifo_dma_info->read_entries);
    fifo_dma_info->read_entries = 0;


exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fifo_dma_fabric_cell_validate(int unit)
{
    uint32  fifo_dma_timeout,
            host_mem_num_entries;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable != SOC_DFE_PROPERTY_UNAVAIL && SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable)
    {
        
        if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold > soc_reg_field_length(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_HOSTMEM_THRESHOLDr, THRESHOLDf))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("fabric_cell_fifo_dma_threhold is out of range (%d)"), SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold));
        }

        
        fifo_dma_timeout = SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout;
        MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(fifo_dma_timeout);
        if (fifo_dma_timeout > soc_reg_field_length(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, TIMEOUT_COUNTf))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("fabric_cell_fifo_dma_timeout is out of range (%d)"), SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout));
        }

        
        host_mem_num_entries = SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size / _SOC_FE3200_FIFO_DMA_FABRIC_CELL_ENTRY_SIZE;
        if (host_mem_num_entries < SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold )
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("fabric_cell_fifo_dma_buffer size is small compared to fabric_cell_fifo_dma_threhold")));
        }

        if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size  < _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MIN_BUFFER_SIZE ||
            SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size  > _SOC_FE3200_FIFO_DMA_FABRIC_CELL_MAX_BUFFER_SIZE )
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("fabric_cell_fifo_dma_buffer_size is out of range (%d)"), SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size));
        }
       
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif

#undef _ERR_MSG_MODULE_NAME
