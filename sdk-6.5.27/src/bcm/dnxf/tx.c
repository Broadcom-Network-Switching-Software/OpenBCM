/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * DNXF TX
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <sal/core/boot.h>
#include <bcm/types.h>
#include <bcm/tx.h>
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/pkt_tx.h>
#endif
#include <bcm_int/dnxf/auto_generated/dnxf_stk_dispatch.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell.h>
#include <soc/dnxf/cmn/mbcm.h>

#include <soc/drv.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/dnxc_cells_buffer.h>
#include <soc/dnxc/dnxc_captured_buffer.h>
#include <soc/dnxc/dnxc_crc.h>

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#endif
shr_error_e
dnxf_tx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_fabric.rxtx.feature_get(unit, dnxf_data_fabric_rxtx_pkt_dma_supported))
    {
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {
            SHR_IF_ERR_EXIT(bcm_access_tx_init(unit));
        }
#endif
        if (!SOC_WARM_BOOT(unit))
        {
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cell_tx_init, (unit)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxf_tx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(bcm_access_tx_deinit(unit));
    }
exit:
#endif

    SHR_FUNC_EXIT;
}

int
_bcm_dnxf_adapter_tx(
    int unit,
    bcm_pkt_t * tx_pkt)
{
#ifdef ADAPTER_SERVER_MODE
    uint8 *adapter_pkt_data_p = NULL;
    int nof_signals = 1, src_port = 0;
    adapter_ms_id_e ms_id = ADAPTER_MS_ID_FIRST_MS;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE

    if (SAL_BOOT_PLISIM)
    {

        SHR_ALLOC_SET_ZERO_ERR_EXIT(adapter_pkt_data_p, dnxf_data_fabric.cell.max_vsc_format_size_get(unit),
                                    "Allocating adapter payload buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        sal_memcpy(adapter_pkt_data_p, tx_pkt->pkt_data->data, tx_pkt->pkt_data->len);

        SHR_IF_ERR_EXIT(adapter_send_buffer
                        (unit, (uint32) ms_id, src_port, tx_pkt->pkt_data->len, adapter_pkt_data_p, nof_signals));
    }

exit:
    if (SAL_BOOT_PLISIM)
    {
        SHR_FREE(adapter_pkt_data_p);
    }
#endif

    SHR_FUNC_EXIT;
}

int
dnxf_pkt_dma_tx(
    int unit,
    bcm_pkt_t * tx_pkt,
    void *cookie)
{

    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_IF_ERR_EXIT(_bcm_dnxf_adapter_tx(unit, tx_pkt));
        SHR_EXIT();
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {

        SHR_IF_ERR_EXIT(bcm_access_tx(unit, tx_pkt, cookie));
    }
    else
#endif
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Packet DMA requires access to be fully initialized\n");
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnxf_tx(
    int unit,
    bcm_pkt_t * tx_pkt,
    void *cookie)
{
    int rv = _SHR_E_NONE;
    int cell_data_size = 0, offset = 0;
    dnxc_dest_routed_cell_t cell;
    uint8 ftmh_header_buf[DNXC_FTMH_BASE_HEADER_SIZE_BYTES];
    uint16 crc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_fabric.rxtx.feature_get(unit, dnxf_data_fabric_rxtx_pkt_dma_supported))
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_tx_prepare, (unit, tx_pkt, NULL)));

        SHR_IF_ERR_EXIT(dnxf_pkt_dma_tx(unit, tx_pkt, cookie));
    }
    else
    {
        sal_memset(&cell, 0, sizeof(dnxc_dest_routed_cell_t));
        sal_memset(ftmh_header_buf, 0, DNXC_FTMH_BASE_HEADER_SIZE_BYTES);

        SHR_IF_ERR_EXIT(bcm_dnxf_stk_modid_get(unit, (int *) &(cell.header.source_modid)));
        cell.header.dest_modid = tx_pkt->dest_mod;

        if (tx_pkt->pkt_data->len >
            (dnxf_data_fabric.cell.max_vsc_format_size_get(unit) - DNXC_FTMH_HEADER_OFFSET_BYTES -
             DNXC_FTMH_CRC_SIZE_BYTES))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Payload size may be up to %d bytes! Provided value is %d ",
                         (dnxf_data_fabric.cell.max_vsc_format_size_get(unit) - DNXC_FTMH_HEADER_OFFSET_BYTES -
                          DNXC_FTMH_CRC_SIZE_BYTES), tx_pkt->pkt_data->len);
        }

        cell_data_size = DNXC_FTMH_HEADER_OFFSET_BYTES + tx_pkt->pkt_data->len + DNXC_FTMH_CRC_SIZE_BYTES;

        SHR_IF_ERR_EXIT(soc_dnxc_ftmh_base_build_header
                        (unit, tx_pkt->dest_port, cell_data_size, DNXC_FTMH_BASE_HEADER_SIZE_BYTES, ftmh_header_buf));

        offset = 0;
        sal_memcpy(&(cell.payload[offset]), ftmh_header_buf, DNXC_FTMH_BASE_HEADER_SIZE_BYTES);
        offset += DNXC_FTMH_BASE_HEADER_SIZE_BYTES;

        sal_memset(&(cell.payload[offset]), 0, DNXC_FTMH_BASE_HEADER_LB_EXT_MIN_SIZE_BYTES);

        offset += DNXC_FTMH_BASE_HEADER_LB_EXT_MIN_SIZE_BYTES;
        sal_memcpy(&(cell.payload[offset]), tx_pkt->pkt_data->data, tx_pkt->pkt_data->len);

        SHR_IF_ERR_EXIT(soc_dnxc_crc_calc(unit, cell.payload, cell_data_size, &crc));
        _shr_uint16_write(&(cell.payload[cell_data_size - DNXC_FTMH_CRC_SIZE_BYTES]), crc);

        cell.payload_size = cell_data_size;

        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_cpu_cell_send, (unit, &cell));
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
