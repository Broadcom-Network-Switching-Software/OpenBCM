/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC FIFO DMA
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/fifodma.h>

#ifdef BCM_DNXF_SUPPORT

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_ha.h>

#include <soc/dnxf/cmn/dnxf_fifo_dma.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>

soc_dnxf_fifo_dma_t *_soc_dnxf_fifo_dma[SOC_MAX_NUM_DEVICES] = { 0 };

shr_error_e
soc_dnxf_fifo_dma_init(
    int unit)
{
    int rc = 0;
    int channel;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_fifodma_init(unit));

    SHR_ALLOC_SET_ZERO(_soc_dnxf_fifo_dma[unit],
                       sizeof(soc_dnxf_fifo_dma_t) * dnxf_data_fabric.fifo_dma.nof_channels_get(unit),
                       "_soc_dnxf_fifo_dma[unit]", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (channel = 0; channel < dnxf_data_fabric.fifo_dma.nof_channels_get(unit); channel++)
    {

        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

        rc = soc_dnxf_fifo_dma_channel_init(unit, channel);

        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

        SHR_IF_ERR_EXIT(rc);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_deinit(
    int unit)
{
    int channel;
    SHR_FUNC_INIT_VARS(unit);

    if (_soc_dnxf_fifo_dma[unit] != NULL)
    {
        for (channel = 0; channel < dnxf_data_fabric.fifo_dma.nof_channels_get(unit); channel++)
        {
            if (_soc_dnxf_fifo_dma[unit][channel].enable)
            {
                SHR_IF_ERR_CONT(soc_dnxf_fifo_dma_channel_deinit(unit, channel));
            }
        }

        SHR_FREE(_soc_dnxf_fifo_dma[unit]);
    }

    SHR_IF_ERR_CONT(soc_fifodma_deinit(unit));

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_channel_init(
    int unit,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_init,
                                          (unit, channel, &_soc_dnxf_fifo_dma[unit][channel])));

    _soc_dnxf_fifo_dma[unit][channel].enable = 1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_channel_deinit(
    int unit,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_deinit,
                                          (unit, channel, &_soc_dnxf_fifo_dma[unit][channel])));

exit:
    _soc_dnxf_fifo_dma[unit][channel].enable = 0;
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxf_fifo_dma_channel_entry_get(
    int unit,
    int channel,
    uint32 max_entry_size,
    uint32 nof_fifo_dma_buf_entries,
    uint8 *entry)
{
    int rv;
    uint32 buffer_offset;
    SHR_FUNC_INIT_VARS(unit);

    if (_soc_dnxf_fifo_dma[unit][channel].nof_unread_entries < nof_fifo_dma_buf_entries)
    {

        _soc_dnxf_fifo_dma[unit][channel].read_entries =
            nof_fifo_dma_buf_entries - _soc_dnxf_fifo_dma[unit][channel].nof_unread_entries;
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fifo_dma_channel_read_entries,
                                   (unit, channel, &_soc_dnxf_fifo_dma[unit][channel]));
        SHR_IF_ERR_EXIT(rv);

        sal_usleep(100);
    }

    if (_soc_dnxf_fifo_dma[unit][channel].nof_unread_entries >= nof_fifo_dma_buf_entries)
    {
        if (_soc_dnxf_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries > max_entry_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Required bigger entry\n");
        }

        buffer_offset =
            _soc_dnxf_fifo_dma[unit][channel].current_entry_id * _soc_dnxf_fifo_dma[unit][channel].config.entry_size;
        sal_memcpy(entry, &_soc_dnxf_fifo_dma[unit][channel].buffer[buffer_offset],
                   sizeof(uint8) * _soc_dnxf_fifo_dma[unit][channel].config.entry_size * nof_fifo_dma_buf_entries);
        _soc_dnxf_fifo_dma[unit][channel].current_entry_id =
            (_soc_dnxf_fifo_dma[unit][channel].current_entry_id +
             nof_fifo_dma_buf_entries) % _soc_dnxf_fifo_dma[unit][channel].config.max_entries;

        _soc_dnxf_fifo_dma[unit][channel].nof_unread_entries -= nof_fifo_dma_buf_entries;
        _soc_dnxf_fifo_dma[unit][channel].read_entries = 0;

    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_EMPTY);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

#undef BSL_LOG_MODULE
