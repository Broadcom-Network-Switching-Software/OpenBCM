/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FIFO DMA
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#ifdef BCM_DFE_SUPPORT

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_fifo_dma.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_drv.h>



soc_dfe_fifo_dma_t _soc_dfe_fifo_dma[SOC_MAX_NUM_DEVICES][SOC_DFE_IMP_DEFS_MAX(FIFO_DMA_NOF_CHANNELS)];

soc_error_t
soc_dfe_fifo_dma_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    sal_memset(_soc_dfe_fifo_dma[unit], 0, sizeof(soc_dfe_fifo_dma_t) *  SOC_DFE_IMP_DEFS_MAX(FIFO_DMA_NOF_CHANNELS));

    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_fifo_dma_deinit(int unit)
{
    int channel;
    SOCDNX_INIT_FUNC_DEFS;

    
    for (channel = 0; channel  < SOC_DFE_IMP_DEFS_MAX(FIFO_DMA_NOF_CHANNELS); channel++)
    {
        if (_soc_dfe_fifo_dma[unit][channel].enable)
        {
            SOCDNX_IF_ERR_EXIT(soc_dfe_fifo_dma_channel_deinit(unit, channel));
            _soc_dfe_fifo_dma[unit][channel].enable = 0;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_fifo_dma_channel_init(int unit, int channel)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fifo_dma_channel_init, (unit, channel, &_soc_dfe_fifo_dma[unit][channel]));
    SOCDNX_IF_ERR_EXIT(rv);
    _soc_dfe_fifo_dma[unit][channel].enable = 1;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_fifo_dma_channel_deinit(int unit, int channel)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fifo_dma_channel_deinit, (unit, channel, &_soc_dfe_fifo_dma[unit][channel]));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_dfe_fifo_dma_channel_entry_get(int unit, int channel, uint32  max_entry_size, uint32 nof_fifo_dma_buf_entries, uint8 *entry)
{
    int rv;
    uint32 buffer_offset;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (_soc_dfe_fifo_dma[unit][channel].nof_unread_entries < nof_fifo_dma_buf_entries)
    {
        
        _soc_dfe_fifo_dma[unit][channel].read_entries = nof_fifo_dma_buf_entries - _soc_dfe_fifo_dma[unit][channel].nof_unread_entries;
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fifo_dma_channel_read_entries, (unit, channel, &_soc_dfe_fifo_dma[unit][channel]));
        SOCDNX_IF_ERR_EXIT(rv);
        
        sal_usleep(100);
    }

    
    if (_soc_dfe_fifo_dma[unit][channel].nof_unread_entries > 0)
    {
        if (_soc_dfe_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries > max_entry_size)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Required bigger entry\n")));
        }

        buffer_offset = _soc_dfe_fifo_dma[unit][channel].current_entry_id * _soc_dfe_fifo_dma[unit][channel].config.entry_size;
        sal_memcpy(entry, &_soc_dfe_fifo_dma[unit][channel].buffer[buffer_offset], sizeof(uint8) * _soc_dfe_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries);
        _soc_dfe_fifo_dma[unit][channel].current_entry_id = (_soc_dfe_fifo_dma[unit][channel].current_entry_id + nof_fifo_dma_buf_entries) % _soc_dfe_fifo_dma[unit][channel].config.max_entries; 

        _soc_dfe_fifo_dma[unit][channel].nof_unread_entries -=  nof_fifo_dma_buf_entries;
        _soc_dfe_fifo_dma[unit][channel].read_entries = 0;

    } else {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_EMPTY);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_dfe_fifo_dma_channel_clear(int unit, int channel)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fifo_dma_channel_clear, (unit, channel, &_soc_dfe_fifo_dma[unit][channel]));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

