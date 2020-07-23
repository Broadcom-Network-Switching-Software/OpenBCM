/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 */
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <shared/bsl.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
/** allow drv.h include excplictly */
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
/** allow soc/mcm/memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif
#include <soc/drv.h> /** needed for access */
#include <soc/mcm/memregs.h> /** needed for SOC_REG_NAME */

#include <bcm/l3.h>
#include <appl/diag/diag.h>

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnxc/diag_dnxc_diag.h>
#include <shared/utilex/utilex_bitstream.h>

#include "diag_dnx_flexe.h"
#include "diag_flexe_adapt.h"

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/sand/sand_mem.h>
#include <bcm_int/dnx/port/port_flexe_reg_access.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>

#ifdef INCLUDE_FLEXE_DBG
#include <reg_operate.h>
#endif

#if 1
shr_error_e
diag_flexe_algo_port_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s * port_info)
{
    shr_error_e rc;

    rc = dnx_algo_port_info_get(unit, logical_port, port_info);
    if (rc != _SHR_E_NONE)
    {
        sal_memset(port_info, 0, sizeof(dnx_algo_port_info_s));
    }

    return _SHR_E_NONE;
}

/**
 * \brief
 *   wrapper function if group is valid
 * \param [in] unit - The unit number.
 * \param [in] group_index - group index.
 * \return
 *   \retval 0 or 1
 */
int
diag_flexe_is_group_valid(
    int unit,
    uint32 group_index)
{
    int rc;
    int valid;

    /** verify group range */
    if (group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        return 0;
    }

    rc = dnx_algo_flexe_general_db.group_info.valid.get(unit, group_index, &valid);
    if (rc != _SHR_E_NONE)
    {
        return 0;
    }
    return valid;
}

/**
 * \brief
 *   wrapper function to get flexe clients bitmap
 * \param [in] unit - The unit number.
 * \param [in] flags - port type flags: DIAG_FLEXE_PORT_TYPE_CLIENT etc.
 * \param [in] logicals - port bitmap of requeied port type.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_flexe_port_logicals_get(
    int unit,
    uint32 flags,
    bcm_pbmp_t * logicals)
{
    bcm_pbmp_t pbmp;
    dnx_algo_port_state_e state;
    bcm_port_t logical_port;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(logicals, _SHR_E_PARAM, "logicals");
    BCM_PBMP_CLEAR(*logicals);

    if (flags & DIAG_FLEXE_PORT_TYPE_PHY)
    {
        BCM_PBMP_CLEAR(pbmp);
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY, 0, &pbmp));
        BCM_PBMP_OR(*logicals, pbmp);
    }
    if (flags & DIAG_FLEXE_PORT_TYPE_CLIENT)
    {
        BCM_PBMP_CLEAR(pbmp);
        
        /*
         * SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_BUS_A, 0,
         * &pbmp));
         */
        for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
        {
            /** filter free ports */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
            if (state == DNX_ALGO_PORT_STATE_INVALID)
            {
                continue;
            }

            /** filter port types different from FLEXE_MAC / MAC_L1*/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
            {
                continue;
            }

            /** add to bitmap */
            BCM_PBMP_PORT_ADD(pbmp, logical_port);
        }
        BCM_PBMP_OR(*logicals, pbmp);

    }
    if (flags & DIAG_FLEXE_PORT_TYPE_CLIENT_MAC)
    {
        BCM_PBMP_CLEAR(pbmp);
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC, 0, &pbmp));
        BCM_PBMP_OR(*logicals, pbmp);
    }
    if (flags & DIAG_FLEXE_PORT_TYPE_CLIENT_SAR)
    {
        BCM_PBMP_CLEAR(pbmp);
        
        /*
         * SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_BUS_C, 0,
         * &pbmp));
         */
        for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
        {
            /** filter free ports */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
            if (state == DNX_ALGO_PORT_STATE_INVALID)
            {
                continue;
            }

            /** filter port types different from FLEXE_MAC / MAC_L1*/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
            {
                continue;
            }

            /** add to bitmap */
            BCM_PBMP_PORT_ADD(pbmp, logical_port);
        }
        BCM_PBMP_OR(*logicals, pbmp);
    }
    if (flags & DIAG_FLEXE_PORT_TYPE_ETH)
    {
        /*
         * only L1 eth is concern
         */
        BCM_PBMP_CLEAR(pbmp);
        for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
        {
            /** filter free ports */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
            if (state == DNX_ALGO_PORT_STATE_INVALID)
            {
                continue;
            }

            /** filter port types different from FLEXE_MAC / MAC_L1*/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info))
            {
                continue;
            }

            /** add to bitmap */
            BCM_PBMP_PORT_ADD(pbmp, logical_port);
        }
        BCM_PBMP_OR(*logicals, pbmp);
    }
    if (flags & DIAG_FLEXE_PORT_TYPE_ILKN)
    {
        BCM_PBMP_CLEAR(pbmp);
        /*
         * SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
         * DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK, &pbmp));
         */
        for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
        {
            /** filter free ports */
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.state.get(unit, logical_port, &state));
            if (state == DNX_ALGO_PORT_STATE_INVALID)
            {
                continue;
            }

            /** filter port types different from FLEXE_MAC / MAC_L1*/
            SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.info.get(unit, logical_port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
            {
                continue;
            }

            /** add to bitmap */
            BCM_PBMP_PORT_ADD(pbmp, logical_port);
        }

        BCM_PBMP_OR(*logicals, pbmp);
    }
    if (flags & DIAG_FLEXE_PORT_GROUP)
    {
        uint32 group_index;

        BCM_PBMP_CLEAR(pbmp);
        for (group_index = 0; group_index < DIAG_FLEXE_MAX_GROUP_INDEX; group_index++)
        {
            if (diag_flexe_is_group_valid(unit, group_index))
            {
                BCM_PBMP_PORT_ADD(pbmp, group_index);
            }
        }

        BCM_PBMP_OR(*logicals, pbmp);
    }

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_flexe_core_channel_name(
    uint32 channel,
    int len,
    char *str_name)
{
    if (channel < 80)
    {
        sal_snprintf(str_name, len, "BusA_%d", channel);
    }
    else if (channel < 160)
    {
        sal_snprintf(str_name, len, "BusB_%d", channel - 80);
    }
    else if (channel < 240)
    {
        sal_snprintf(str_name, len, "BusC_%d", channel - 160);
    }
    else
    {
        sal_snprintf(str_name, len, "Invalid_%d", channel);
    }
}

/**
 * \brief
 *   wrapper function to conver interlaken channel to logical port
 * \param [in] unit - The unit number.
 * \param [in] channel - interlaken channel.
 * \param [in] ilkn_port - interlaken port in core.
 * \param [in] logical_port - logical port #.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_flexe_ilkn_channel_2_logicals_port(
    int unit,
    uint32 channel,
    uint32 ilkn_port,
    bcm_port_t * logical_port)
{
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    int ilkn_channel;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(logical_port, _SHR_E_PARAM, "logical_port");

    SHR_IF_ERR_EXIT(dnx_flexe_port_logicals_get(unit, DIAG_FLEXE_PORT_TYPE_ILKN, &pbmp));
    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &ilkn_channel));
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
        if ((channel == ilkn_channel) && (ilkn_port == ilkn_access_info.port_in_core))
        {
            *logical_port = port;
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to read flexE core register
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_reg_read(
    int unit,
    int address,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_flexe_reg_read(unit, address, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to read flexE core memory
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index.
 * \param [in] width - width of memory.
 * \param [out] value - memory value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_mem_read(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_flexe_mem_read(unit, address, index, width, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to read flexE core register
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_reg_write(
    int unit,
    int address,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_flexe_reg_write(unit, address, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   wrapper function to read flexE core memory
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index.
 * \param [in] width - width of memory.
 * \param [in] value - memory value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_mem_write(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    SHR_IF_ERR_EXIT(dnx_flexe_mem_write(unit, address, index, width, value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_core_b66_switch_get(
    int unit,
    int ram_id,
    uint32 src_channel,
    uint32 *dst_num,
    uint32 *dst_channel)
{
    uint32 num, rd_val;
    uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 channel;
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(src_channel, DIAG_FLEXE_CHANNEL_NUM - 1, _SHR_E_PARAM, "Invald src_channel %d", src_channel);
    SHR_NULL_CHECK(dst_num, _SHR_E_PARAM, "dst_num");
    SHR_NULL_CHECK(dst_channel, _SHR_E_PARAM, "dst_channel");

    sal_memset(data_32, 0, sizeof(data_32));
    SHR_IF_ERR_EXIT(diag_flexe_mem_read(unit, B66SWITCH_BASE_ADDR + ram_id, src_channel, 24, data_32));
    rd_val = data_32[0];
    num = 0;
    channel = rd_val & 0xFF;
    if (channel != 0xFF)
    {
        dst_channel[num] = channel;
        num++;
    }
    channel = (rd_val & 0xFF00) >> 8;
    if (channel != 0xFF)
    {
        dst_channel[num] = channel;
        num++;
    }
    channel = (rd_val & 0xFF0000) >> 16;
    if (channel != 0xFF)
    {
        dst_channel[num] = channel;
        num++;
    }
    *dst_num = num;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_core_b66_sar_tx_get(
    int unit,
    uint32 sar_channel,
    uint32 *seg_en,
    uint32 *tx_chan_map)
{
    uint32 rd_val[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(sar_channel, DIAG_FLEXE_CHANNEL_NUM - 1, _SHR_E_PARAM, "Invald src_channel %d", sar_channel);
    SHR_NULL_CHECK(seg_en, _SHR_E_PARAM, "seg_en");
    SHR_NULL_CHECK(tx_chan_map, _SHR_E_PARAM, "tx_chan_map");

    sal_memset(rd_val, 0, sizeof(rd_val));
    SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, B66SAR_TX_BASE_ADDR + 2 + sar_channel, rd_val));

    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(rd_val, 0, 1, seg_en));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(rd_val, 8, 10, tx_chan_map));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_core_b66_sar_rx_get(
    int unit,
    uint32 sar_channel,
    uint32 *mon_en,
    uint32 *rx_chan_map)
{
    uint32 rd_val[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    SHR_FUNC_INIT_VARS(unit);

    SHR_MAX_VERIFY(sar_channel, DIAG_FLEXE_CHANNEL_NUM - 1, _SHR_E_PARAM, "Invald src_channel %d", sar_channel);
    SHR_NULL_CHECK(mon_en, _SHR_E_PARAM, "mon_en");
    SHR_NULL_CHECK(rx_chan_map, _SHR_E_PARAM, "rx_chan_map");

    sal_memset(rd_val, 0, sizeof(rd_val));
    SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, B66SAR_RX_BASE_ADDR + 2 + sar_channel, rd_val));

    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(rd_val, 0, 1, mon_en));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(rd_val, 8, 10, rx_chan_map));

exit:
    SHR_FUNC_EXIT;
}

#endif

#if 1
/**
 * \brief
 *   wrapper function to conver flexe channel to logical port
 * \param [in] unit - The unit number.
 * \param [in] channel - flexe channel: 0-79: Bus_A; 80-159: Bus_B; 160-239: Bus_C.
 * \param [in] logical_port - logical port #.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_flexe_core_channel_2_logicals_port(
    int unit,
    uint32 channel,
    bcm_port_t * logical_port)
{
    int rc;
    dnx_algo_port_type_e port_type;
    uint32 client_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(logical_port, _SHR_E_PARAM, "logical_port");
    if (channel < 80)
    {
        port_type = DNX_ALGO_PORT_TYPE_FLEXE_CLIENT;
        client_index = channel;
    }
    else if (channel < 160)
    {
        port_type = DNX_ALGO_PORT_TYPE_FLEXE_SAR;
        client_index = channel - 80;
    }
    else if (channel < 240)
    {
        port_type = DNX_ALGO_PORT_TYPE_FLEXE_MAC;
        client_index = channel - 160;
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    rc = dnx_algo_port_flexe_client_index_to_port_get(unit, port_type, client_index, logical_port);
    if (SHR_FAILURE(rc))
    {
        DIAG_FLEXE_CHANNEL_GPORT_SET(*logical_port, channel);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow connect at flexe core internal.
*/
shr_error_e
dnx_diag_flexe_flow_get_client(
    int unit,
    bcm_port_t src_port,
    uint32 *dst_num,
    bcm_port_t * dst_port)
{
    int src_channel;
    dnx_algo_port_info_s src_port_info;
    uint32 rd_val;
    uint32 data_32[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    int ram_id;
    uint32 num, channel;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_num, _SHR_E_PARAM, "dst_num");
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    SHR_IF_ERR_EXIT(diag_flexe_algo_port_info_get(unit, src_port, &src_port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &src_channel));
    if (src_port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_CLIENT)
    {
        ram_id = 0;
    }
    else if (src_port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_MAC)
    {
        ram_id = 2;
    }
    else if (src_port_info.port_type == DNX_ALGO_PORT_TYPE_FLEXE_SAR)
    {
        ram_id = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    sal_memset(data_32, 0, sizeof(data_32));
    SHR_IF_ERR_EXIT(diag_flexe_mem_read(unit, B66SWITCH_BASE_ADDR + ram_id, src_channel, 24, data_32));
    rd_val = data_32[0];
    num = 0;
    channel = rd_val & 0xFF;
    if (channel != 0xFF)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_core_channel_2_logicals_port(unit, channel, dst_port + num));
        num++;
    }
    channel = (rd_val & 0xFF00) >> 8;
    if (channel != 0xFF)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_core_channel_2_logicals_port(unit, channel, dst_port + num));
        num++;
    }
    channel = (rd_val & 0xFF0000) >> 16;
    if (channel != 0xFF)
    {
        SHR_IF_ERR_EXIT(dnx_flexe_core_channel_2_logicals_port(unit, channel, dst_port + num));
        num++;
    }
    *dst_num = num;

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow connect from tiny_mac to nif(eth or inerlaken).
*/
shr_error_e
dnx_diag_flexe_flow_tmac_2_nif_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    int src_channel;
    dnx_algo_port_rmc_info_t rmc;
    uint32 entry_handle_id;
    uint32 traffic_type, port_or_channel, ilkn_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get FLEXE MAC channel
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &src_channel));

    /*
     * get FlexE RMC info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, src_port, 0, 0, &rmc));
    if (rmc.rmc_id == DNX_ALGO_PORT_INVALID)
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "The RMC of FlexE port %d is invalid.%s%s\n", src_port, EMPTY, EMPTY);
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc.rmc_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAFFIC_TYPE, INST_SINGLE, &traffic_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PORT_OR_CHANNEL, INST_SINGLE, &port_or_channel);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ILKN_PORT, INST_SINGLE, &ilkn_port);

    /** Preforming the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * RMC traffic type:
     *
     * 000 IRE LP,
     * 100 IRE HP,
     * 001 ILU LP,
     * 101 ILU HP,
     * 010 ESB
     */
    if (traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ESB)
    {
        /*
         * ESB, port_or_channel 0 will be valid while flexe phy was at CDU1
         */
        if (port_or_channel == 255)
        {
            *dst_port = SOC_PORT_INVALID;
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, port_or_channel, 0, 0, dst_port));
        }
    }
    else if ((traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_LP) ||
             (traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_HP))
    {
        /*
         * ILU LP or ILU HP, in this case, port_or_channel 0 will be valid.
         */
        SHR_IF_ERR_EXIT(dnx_flexe_ilkn_channel_2_logicals_port(unit, port_or_channel, ilkn_port, dst_port));
    }
    else
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * get the flow connect from sar to interlaken.
 */
static shr_error_e
dnx_diag_flexe_flow_sar_2_ilkn_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    uint32 sar_en, tx_chan_map;
    int sar_channel;
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    /*
     * get SAR channel 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &sar_channel));

    SHR_IF_ERR_EXIT(READ_FSCL_SAR_TX_CTRL_CFGr(unit, sar_channel, &reg_val));

    sar_en = soc_reg_field_get(unit, FSCL_SAR_TX_CTRL_CFGr, reg_val, SEG_ENf);
    tx_chan_map = soc_reg_field_get(unit, FSCL_SAR_TX_CTRL_CFGr, reg_val, TX_CHAN_MAPf);

    if ((sar_en) && (tx_chan_map != 0x200))
    {
        SHR_IF_ERR_EXIT(dnx_flexe_ilkn_channel_2_logicals_port(unit, tx_chan_map, 0, dst_port));
        SHR_EXIT();
    }
    else
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * get the flow connect from interlaken to sar.
 */
static shr_error_e
dnx_diag_flexe_flow_ilkn_2_sar_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    uint32 mon_en, rx_chan_map, reg_val;
    int sar_channel, ilu_channel;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    /*
     * interlaken channel 
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &ilu_channel));

    for (sar_channel = 0; sar_channel < DIAG_FLEXE_CHANNEL_NUM; sar_channel++)
    {
        SHR_IF_ERR_EXIT(READ_FSCL_SAR_RX_CTRL_CFGr(unit, sar_channel, &reg_val));

        mon_en = soc_reg_field_get(unit, FSCL_SAR_RX_CTRL_CFGr, reg_val, MON_ENf);
        rx_chan_map = soc_reg_field_get(unit, FSCL_SAR_RX_CTRL_CFGr, reg_val, RX_CHAN_MAPf);

        if ((mon_en) && (ilu_channel == rx_chan_map))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get(unit, DNX_ALGO_PORT_TYPE_FLEXE_SAR,
                                                                         sar_channel, dst_port));
            SHR_EXIT();
        }
    }

    *dst_port = SOC_PORT_INVALID;
    SHR_EXIT();
    /*
     * SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
     */

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_core_counter_clear(
    int unit)
{
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clear mcmac cnt. 
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_MACRX_STATISTIC_ALL_CLRr, &reg_val, STA_ALL_CLRf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACRX_STATISTIC_ALL_CLRr(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_MACTX_STATISTIC_ALL_CLRr, &reg_val, STA_ALL_CLRf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_MACTX_STATISTIC_ALL_CLRr(unit, reg_val));
    /*
     * Clear cpb egress cnt. 
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CLR_EGr, &reg_val, CLR_EGf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CLR_EGr(unit, reg_val));

    reg_val = 0;
    soc_reg_field_set(unit, FSCL_CLR_EGr, &reg_val, CLR_EGf, 0);
    SHR_IF_ERR_EXIT(WRITE_FSCL_CLR_EGr(unit, reg_val));
    /*
     * Clear flexe macrx cnt. 
     */
    reg_val = 0;
    soc_reg_field_set(unit, FSCL_FLEXE_MACRX_STATISTIC_ALL_CLRr, &reg_val, STA_ALL_CLRf, 1);
    SHR_IF_ERR_EXIT(WRITE_FSCL_FLEXE_MACRX_STATISTIC_ALL_CLRr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow from virtual_clien to eth/ILKN.
*/
shr_error_e
dnx_diag_flexe_flow_get_virtual_client(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dst_port, _SHR_E_PARAM, "dst_port");

    SHR_IF_ERR_EXIT(diag_flexe_algo_port_info_get(unit, src_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_sar_2_ilkn_get(unit, src_port, dst_port));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_tmac_2_nif_get(unit, src_port, dst_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow connect from Eth port to Tiny mac(Bus B).
*/
shr_error_e
dnx_diag_flexe_flow_eth_2_tmac_get(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    int rc;
    int first_phy_port;
    uint32 entry_handle_id;
    uint32 target_client, target_tmc;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * get NIF port TMC index(= first PHY ?)
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, src_port, 0, &first_phy_port));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, first_phy_port);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TARGET_CLIENT, INST_SINGLE, &target_client);

    /** Preforming the action after this call the pointers that we set in field32_request() */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, entry_handle_id));

    /*
     * For Q2A-B0, 0x7F is default Invalid target client value
     */
    if (target_client == 0x7F)
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT();
    }
    else if (target_client != 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_index_to_port_get(unit, DNX_ALGO_PORT_TYPE_FLEXE_MAC,
                                                                     target_client, dst_port));
    }
    else
    {
        rc = dnx_algo_port_flexe_client_index_to_port_get(unit, DNX_ALGO_PORT_TYPE_FLEXE_MAC, 0, dst_port);
        if (rc == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, *dst_port, 0, 0, &rmc));
            if ((rmc.rmc_id != DNX_ALGO_PORT_INVALID) && (rmc.sch_priority == bcmPortNifSchedulerTDM))
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_RX_CTRL, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, rmc.rmc_id);
                dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_ALARM_TARGET_TMC,
                                           INST_SINGLE, &target_tmc);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                if (target_tmc == first_phy_port)
                {
                    SHR_EXIT_WITH_LOG(_SHR_E_NONE, "The target_client is 0(OK!). src_port=%d(%d)%s\n", src_port,
                                      first_phy_port, EMPTY);
                }
            }
        }
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "The target_client is 0(not Found). src_port=%d(%d)%s\n", src_port,
                          first_phy_port, EMPTY);
        /*
         * SHR_EXIT();
         */
        /*
         * SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
         */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_flow_get_eth_ilkn(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_flexe_algo_port_info_get(unit, src_port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 1))
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_eth_2_tmac_get(unit, src_port, dst_port));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 0))
    {
        *dst_port = SOC_PORT_INVALID;
        SHR_EXIT();
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_ilkn_2_sar_get(unit, src_port, dst_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow from ETH to virtual client.
*/
shr_error_e
dnx_diag_flexe_flow_get_eth(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_eth_ilkn(unit, src_port, dst_port));

exit:
    SHR_FUNC_EXIT;
}

/*
    get the flow from ILKN to virtual client.
*/
shr_error_e
dnx_diag_flexe_flow_get_ilkn(
    int unit,
    bcm_port_t src_port,
    bcm_port_t * dst_port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_diag_flexe_flow_get_eth_ilkn(unit, src_port, dst_port));

exit:
    SHR_FUNC_EXIT;
}
#endif

/* subsys_pcs.pcs_rx.pcs_rx_basic */
sh_flexe_core_counter_t flexe_core_counter_pcs_rx_basic[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"word_err_cnt_100g_0", 0x20, 0, 0, 0, 0},
    {"word_err_cnt_100g_1", 0x21, 0, 0, 0, 0},
    {"word_err_cnt_50g_0", 0x22, 0, 0, 0, 0},
    {"word_err_cnt_50g_1", 0x23, 0, 0, 0, 0},
    {"word_err_cnt_50g_2", 0x24, 0, 0, 0, 0},
    {"word_err_cnt_50g_3", 0x25, 0, 0, 0, 0},
    {"bip_err_cnt_100g_0", 0x26, 0, 0, 0, 0},
    {"bip_err_cnt_100g_1", 0x27, 0, 0, 0, 0},
    {"bip_err_cnt_50g_0", 0x28, 0, 0, 0, 0},
    {"bip_err_cnt_50g_1", 0x29, 0, 0, 0, 0},
    {"bip_err_cnt_50g_2", 0x2a, 0, 0, 0, 0},
    {"bip_err_cnt_50g_3", 0x2b, 0, 0, 0, 0},
    {"am_invld_cnt_100g_0", 0x2c, 0, 0, 0, 0},
    {"am_invld_cnt_100g_1", 0x2d, 0, 0, 0, 0},
    {"am_invld_cnt_50g_0", 0x2e, 0, 0, 0, 0},
    {"am_invld_cnt_50g_1", 0x2f, 0, 0, 0, 0},
    {"am_invld_cnt_50g_2", 0x30, 0, 0, 0, 0},
    {"am_invld_cnt_50g_3", 0x31, 0, 0, 0, 0},
    {"rx_test_err_cnt_0", 0x32, 0, 0, 0, 0},
    {"rx_test_err_cnt_1", 0x33, 0, 0, 0, 0},
    {"rx_test_err_cnt_2", 0x34, 0, 0, 0, 0},
    {"rx_test_err_cnt_3", 0x35, 0, 0, 0, 0},
    {"fec_one2zero_cnt_0", 0x36, 0, 0, 0, 0},
    {"fec_one2zero_cnt_1", 0x37, 0, 0, 0, 0},
    {"fec_one2zero_cnt_2", 0x38, 0, 0, 0, 0},
    {"fec_one2zero_cnt_3", 0x39, 0, 0, 0, 0},
    {"fec_zero2one_cnt_0", 0x3a, 0, 0, 0, 0},
    {"fec_zero2one_cnt_1", 0x3b, 0, 0, 0, 0},
    {"fec_zero2one_cnt_2", 0x3c, 0, 0, 0, 0},
    {"fec_zero2one_cnt_3", 0x3d, 0, 0, 0, 0},
    {"fec_err_symbol_cnt_0", 0x3e, 0, 0, 0, 0},
    {"fec_err_symbol_cnt_1", 0x3f, 0, 0, 0, 0},
    {"fec_err_symbol_cnt_2", 0x40, 0, 0, 0, 0},
    {"fec_err_symbol_cnt_3", 0x41, 0, 0, 0, 0},
    {"fec_cw_uncorr_flg_0", 0x42, 0, 0, 0, 0},
    {"fec_cw_uncorr_flg_1", 0x43, 0, 0, 0, 0},
    {"fec_cw_uncorr_flg_2", 0x44, 0, 0, 0, 0},
    {"fec_cw_uncorr_flg_3", 0x45, 0, 0, 0, 0},
};

/* subsys_pkt.cpb.cpb_ingress */
sh_flexe_core_counter_t flexe_core_counter_cpb_ingress[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"cpb_in_rx_pkt_num", 0x3f, 1, 64, 40, 0},
    {"cpb_in_rx_sop_num", 0x41, 1, 64, 40, 0},
    {"cpb_in_tx_pkt_num", 0x53, 1, 64, 40, 0},
    {"cpb_in_tx_sop_num", 0x54, 1, 64, 40, 0},
    {"adp_in_tx_sop_num", 0x55, 1, 64, 40, 0},
    {"adp_in_tx_eop_num", 0x56, 1, 64, 40, 0},
    {"cpb_in_rx_err_num", 0x67, 1, 64, 40, 0},
};

/* subsys_mac.mac.mcmacrx */
sh_flexe_core_counter_t flexe_core_counter_mcmacrx[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"macrx_out_sop", 0x20, 0, 0, 0, 0},
    {"macrx_out_eop", 0x21, 0, 0, 0, 0},
    {"macrx_decode_err_code", 0xb0, 1, 64, 80, 0},
    {"macrx_total_bytes", 0xc0, 1, 64, 80, 0},
    {"macrx_good_bytes", 0xc1, 1, 64, 80, 0},
    {"macrx_bad_bytes", 0xc2, 1, 64, 80, 0},
    {"macrx_fragment_bytes", 0xc3, 1, 64, 80, 0},
    {"macrx_jabber_bytes", 0xc4, 1, 64, 80, 0},
    {"macrx_oversize_bytes", 0xc5, 1, 64, 80, 0},
    {"macrx_unicast_bytes", 0xc6, 1, 64, 80, 0},
    {"macrx_multicast_bytes", 0xc7, 1, 64, 80, 0},
    {"macrx_broadcast_bytes", 0xc8, 1, 64, 80, 0},
    {"macrx_b64_bytes", 0xc9, 1, 64, 80, 0},
    {"macrx_b65_mpl_bytes", 0xca, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
    {"macrx_total_frame ", 0xd0, 1, 64, 80, 0},
    {"macrx_good_frame", 0xd1, 1, 64, 80, 0},
    {"macrx_bad_frame", 0xd2, 1, 64, 80, 0},
    {"macrx_fragment_frame", 0xd3, 1, 64, 80, 0},
    {"macrx_jabber_frame", 0xd4, 1, 64, 80, 0},
    {"macrx_oversize_frame", 0xd5, 1, 64, 80, 0},
    {"macrx_unicast_frame", 0xd6, 1, 64, 80, 0},
    {"macrx_multicast_frame", 0xd7, 1, 64, 80, 0},
    {"macrx_broadcast_frame", 0xd8, 1, 64, 80, 0},
    {"macrx_b64_frame", 0xd9, 1, 64, 80, 0},
    {"macrx_b65_mpl_frame", 0xda, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
};

/* subsys_mac.mac.mcmactx */
sh_flexe_core_counter_t flexe_core_counter_mcmactx[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"mactx_decode_err_code", 0xb0, 1, 64, 80, 0},
    {"mactx_encode_err_code", 0xb1, 1, 64, 80, 0},
    {"mactx_total_bytes", 0xc0, 1, 64, 80, 0},
    {"mactx_good_bytes", 0xc1, 1, 64, 80, 0},
    {"mactx_bad_bytes", 0xc2, 1, 64, 80, 0},
    {"mactx_sar_fragment_bytes", 0xc4, 1, 64, 80, 0},
    {"mactx_sar_jabber_bytes", 0xc6, 1, 64, 80, 0},
    {"mactx_sar_oversize_bytes ", 0xc7, 1, 64, 80, 0},
    {"mactx_unicast_bytes", 0xc8, 1, 64, 80, 0},
    {"mactx_multicast_bytes", 0xc9, 1, 64, 80, 0},
    {"mactx_broadcast_bytes ", 0xca, 1, 64, 80, 0},
    {"mactx_b64_bytes", 0xcb, 1, 64, 80, 0},
    {"mactx_b65_mpl_bytes", 0xcc, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
    {"mactx_total_frame ", 0xd0, 1, 64, 80, 0},
    {"mactx_good_frame", 0xd1, 1, 64, 80, 0},
    {"mactx_bad_frame", 0xd2, 1, 64, 80, 0},
    {"mactx_sar_fragment_frame", 0xd4, 1, 64, 80, 0},
    {"mactx_sar_jabber_frame", 0xd6, 1, 64, 80, 0},
    {"mactx_sar_oversize_frame", 0xd7, 1, 64, 80, 0},
    {"mactx_unicast_frame", 0xd8, 1, 64, 80, 0},
    {"mactx_multicast_frame", 0xd9, 1, 64, 80, 0},
    {"mactx_broadcast_frame", 0xda, 1, 64, 80, 0},
    {"mactx_b64_frame", 0xdb, 1, 64, 80, 0},
    {"mactx_b65_mpl_frame", 0xdc, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
};

/* subsys_mac.mac.b66sar_rx */
sh_flexe_core_counter_t flexe_core_counter_b66sar_rx[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"sar_rx_pkt_cnt", 0x54, 0, 0, 0, 0},
    {"sar_rx_pkt_cnt_256", 0xb1, 0, 0, 0, 0},
};

/* subsys_mac.mac.b66sar_tx */
sh_flexe_core_counter_t flexe_core_counter_b66sar_tx[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"sar_tx_pkt_cnt", 0x54, 0, 0, 0, 0},
    {"sar_tx_pkt_cnt_256", 0x59, 0, 0, 0, 0},
};

/* subsys_mac.cpb_egress_mac.cpb_egress */
sh_flexe_core_counter_t flexe_core_counter_cpb_egress_mac[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"cnt_max_eg", 0x20, 0, 16, 80, 0},
    {"cnt_depth_eg", 0x70, 0, 0, 0, 0},
    {"sar_pathdelay_stat", 0x7e, 0, 0, 0, 0},
    {"sar_pathdelay_real", 0x7f, 0, 0, 0, 0},
    {"link_full_eg", 0x76, 0, 80, 0, 0},
    {"link_empty_eg", 0x79, 0, 80, 0, 0},
    {"err_cnt_eg_tb", 0x1, 1, 64, 80, 0},
    {"sop_in_cnt_egress ", 0x2, 1, 64, 80, 0},
    {"sop_out_cnt_egress", 0x3, 1, 64, 80, 0},
    {"cell_in_cnt_egress", 0xa, 1, 64, 80, 0},
    {"cell_out_cnt_egress ", 0xb, 1, 64, 80, 0},

};

/* subsys_flexe.flexe_mac.flexe_macrx */
sh_flexe_core_counter_t flexe_core_counter_flexe_macrx[] = {
    /*
     * name address is_memory num_bits num_entry flags
     */
    {"flexe_macrx_decode_err_code", 0xb0, 1, 64, 80, 0},
    {"flexe_macrx_total_bytes", 0xc0, 1, 64, 80, 0},
    {"flexe_macrx_good_bytes", 0xc1, 1, 64, 80, 0},
    {"flexe_macrx_bad_bytes", 0xc2, 1, 64, 80, 0},
    {"flexe_macrx_fragment_bytes", 0xc3, 1, 64, 80, 0},
    {"flexe_macrx_jabber_bytes", 0xc4, 1, 64, 80, 0},
    {"flexe_macrx_oversize_bytes", 0xc5, 1, 64, 80, 0},
    {"flexe_macrx_unicast_bytes", 0xc6, 1, 64, 80, 0},
    {"flexe_macrx_multicast_bytes ", 0xc7, 1, 64, 80, 0},
    {"flexe_macrx_broadcast_bytes ", 0xc8, 1, 64, 80, 0},
    {"flexe_macrx_b64_bytes", 0xc9, 1, 64, 80, 0},
    {"flexe_macrx_b65_mpl_bytes", 0xca, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
    {"flexe_macrx_total_frame", 0xd0, 1, 64, 80, 0},
    {"flexe_macrx_good_frame", 0xd1, 1, 64, 80, 0},
    {"flexe_macrx_bad_frame", 0xd2, 1, 64, 80, 0},
    {"flexe_macrx_fragment_frame", 0xd3, 1, 64, 80, 0},
    {"flexe_macrx_jabber_frame", 0xd4, 1, 64, 80, 0},
    {"flexe_macrx_oversize_frame", 0xd5, 1, 64, 80, 0},
    {"flexe_macrx_unicast_frame", 0xd6, 1, 64, 80, 0},
    {"flexe_macrx_multicast_frame", 0xd7, 1, 64, 80, 0},
    {"flexe_macrx_broadcast_frame", 0xd8, 1, 64, 80, 0},
    {"flexe_macrx_b64_frame", 0xd9, 1, 64, 80, 0},
    {"flexe_macrx_b65_mpl_frame", 0xda, 1, 64, 720, FLEXE_CORE_COUNTER_FLAG_MPL},
};

sh_flexe_core_module_t flexe_core_module_info[] = {
    {"mcmacrx", 0x8000, flexe_core_counter_mcmacrx, COUNTOF(flexe_core_counter_mcmacrx)},
    {"mcmactx", 0x8100, flexe_core_counter_mcmactx, COUNTOF(flexe_core_counter_mcmactx)},
    {"b66sar_rx", 0x8400, flexe_core_counter_b66sar_rx, COUNTOF(flexe_core_counter_b66sar_rx)},
    {"b66sar_tx", 0x8500, flexe_core_counter_b66sar_tx, COUNTOF(flexe_core_counter_b66sar_tx)},
    {"cpb_egress_mac", 0x8800, flexe_core_counter_cpb_egress_mac, COUNTOF(flexe_core_counter_cpb_egress_mac)},
    {"flexe_macrx", 0xCA00, flexe_core_counter_flexe_macrx, COUNTOF(flexe_core_counter_flexe_macrx)},
};

sh_flexe_core_module_t flexe_core_module_subsys_pcs[] = {
    {"pcs_rx_basic", 0x2200, flexe_core_counter_pcs_rx_basic, COUNTOF(flexe_core_counter_pcs_rx_basic)}
};

shr_error_e
dnx_diag_flexe_core_read_counter(
    int unit,
    uint32 base_addr,
    sh_flexe_core_counter_t * cnt_reg_info,
    uint32 index,
    uint32 *cnt_value,
    uint32 *cnt_bits)
{
    uint32 ii, reg_addr, num_bits;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cnt_value, _SHR_E_PARAM, "cnt_value");
    SHR_NULL_CHECK(cnt_bits, _SHR_E_PARAM, "cnt_bits");

    sal_memset(cnt_value, 0, FLEXE_CORE_COUNTER_MAX_SIZE_U32 * sizeof(uint32));
    reg_addr = base_addr + cnt_reg_info->address;
    num_bits = cnt_reg_info->num_bits;
    if (num_bits == 0)
    {
        num_bits = 32;
    }
    *cnt_bits = num_bits;
    if (cnt_reg_info->is_memory)
    {
        SHR_IF_ERR_EXIT(diag_flexe_mem_read(unit, reg_addr, index, num_bits, cnt_value));
    }
    else
    {
        reg_addr = reg_addr + index;
        for (ii = 0; ii < BITS2WORDS(num_bits); ii++)
        {
            SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, reg_addr + ii, cnt_value + ii));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
flexe_core_counter_name_get(
    int unit,
    sh_flexe_core_counter_t * counter_reg,
    uint32 index,
    char *cnt_name)
{
    uint32 channel, offset;
    static char *str_mpl_name[FLEXE_CORE_COUNTER_MPL_NUM] = {
        "65~127",
        "128~255",
        "256~511",
        "512~1023",
        "1024~1518",
        "1519~2047",
        "2048~4095",
        "4096~9216",
        "9217~MTU",
    };
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cnt_name, _SHR_E_PARAM, "cnt_name");
    if (counter_reg->num_entry > 1)
    {
        if (counter_reg->flags & FLEXE_CORE_COUNTER_FLAG_MPL)
        {
            /*
             * ?????? CHAN_NUM~2*CHAN_NUM-1 : 65~127 bytes 2*CHAN_NUM~3*CHAN_NUM-1 : 128~255 bytes
             * 3*CHAN_NUM~4*CHAN_NUM-1 : 256~511 bytes 4*CHAN_NUM~5*CHAN_NUM-1 : 512~1023 bytes
             * 5*CHAN_NUM~6*CHAN_NUM-1 : 1024~1518 bytes 6*CHAN_NUM~7*CHAN_NUM-1 : 1519~2047 bytes
             * 7*CHAN_NUM~8*CHAN_NUM-1 : 2048~4095 bytes 8*CHAN_NUM~9*CHAN_NUM-1 : 4096~9216 bytes
             * 9*CHAN_NUM~10*CHAN_NUM-1 : 9217~mtu bytes
             */
            channel = index / FLEXE_CORE_COUNTER_MPL_NUM;
            offset = index % FLEXE_CORE_COUNTER_MPL_NUM;

            sal_snprintf(cnt_name, PRT_COLUMN_WIDTH_BIG, "%s%s[%d]--%s[%d]",
                         counter_reg->is_memory ? "(M)" : "(R)",
                         counter_reg->counter_name, index, str_mpl_name[offset], channel);
        }
        else
        {
            sal_snprintf(cnt_name, PRT_COLUMN_WIDTH_BIG, "%s%s[%d]",
                         counter_reg->is_memory ? "(M)" : "(R)", counter_reg->counter_name, index);
        }
    }
    else
    {
        sal_snprintf(cnt_name, PRT_COLUMN_WIDTH_BIG, "%s%s",
                     counter_reg->is_memory ? "(M)" : "(R)", counter_reg->counter_name);
    }

exit:
    SHR_FUNC_EXIT;
}

int
flexe_core_is_counter_print(
    int print_opt,
    sh_flexe_core_counter_t * counter_reg,
    uint32 *cnt_value,
    uint32 cnt_bits)
{
    int rc = 0;

    if (print_opt == diag_flexe_counter_print_default)
    {
        /*
         * For single counter, print, for array, only print nozero
         */
        if (counter_reg->num_entry <= 1)
        {
            rc = TRUE;
        }
        else
        {
            if (SHR_BITNULL_RANGE(cnt_value, 0, cnt_bits))
            {
                rc = FALSE;
            }
            else
            {
                rc = TRUE;
            }
        }
    }
    else if (print_opt == diag_flexe_counter_print_nozero)
    {
        if (SHR_BITNULL_RANGE(cnt_value, 0, cnt_bits))
        {
            rc = FALSE;
        }
        else
        {
            rc = TRUE;
        }
    }
    else
    {
        rc = TRUE;
    }
    return rc;
}

shr_error_e
dnx_diag_flexe_core_module_counter_print(
    int unit,
    int in_hex,
    int print_opt,
    sh_flexe_core_module_t * module_info,
    prt_control_t * prt_ctr)
{
    uint32 cnt_value[FLEXE_CORE_COUNTER_MAX_SIZE_U32], cnt_bits;
    uint32 ii, index, entry_num, base_addr;
    sh_flexe_core_counter_t *counter_reg;
    int is_print;
    char value_str[PRT_COLUMN_WIDTH_BIG];
    char cnt_name[PRT_COLUMN_WIDTH_BIG];
    int is_first_row = 1;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * cli_out("################################################################################\n");
     * cli_out("##%-30s%-16s%-30s##\n", "", module_info->module_name, "");
     * cli_out("################################################################################\n"); cli_out("
     * %-44s%-10s%s\n", "CounterName", "RegAddr", "Counter_Value"); cli_out("
     * --------------------------------------------------------------------\n");
     */
    base_addr = module_info->base_address;
    for (ii = 0; ii < module_info->counter_num; ii++)
    {
        counter_reg = module_info->counter_reg + ii;
        entry_num = counter_reg->num_entry;
        if (entry_num == 0)
        {
            entry_num = 1;
        }

        for (index = 0; index < entry_num; index++)
        {
            SHR_IF_ERR_EXIT(dnx_diag_flexe_core_read_counter(unit, base_addr, counter_reg,
                                                             index, cnt_value, &cnt_bits));

            is_print = flexe_core_is_counter_print(print_opt, counter_reg, cnt_value, cnt_bits);
            if (is_print)
            {
                SHR_IF_ERR_EXIT(flexe_core_counter_name_get(unit, counter_reg, index, cnt_name));
                format_value_to_str(cnt_value, cnt_bits, in_hex, value_str);

                if (is_first_row)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    is_first_row = 0;
                    PRT_CELL_SET("%s", module_info->module_name);
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("");
                }

                PRT_CELL_SET("%s", cnt_name);
                PRT_CELL_SET("0x%x", base_addr + counter_reg->address);
                PRT_CELL_SET("%s", value_str);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_core_counter_print(
    int unit,
    int in_hex,
    int print_opt,
    uint32 module_id,
    sh_sand_control_t * sand_control)
{
    sh_flexe_core_module_t *module_info;
    uint32 module_list[diag_flexe_core_module_num];
    uint32 ii, module_num, module;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(module_list, 0, sizeof(module_list));
    if (module_id == diag_flexe_core_module_all)
    {
        module_num = diag_flexe_core_module_num;
        module_list[0] = diag_flexe_core_module_mcmaxrx;
        module_list[1] = diag_flexe_core_module_mcmaxtx;
        module_list[2] = diag_flexe_core_module_b66sar_rx;
        module_list[3] = diag_flexe_core_module_b66sar_tx;
        module_list[4] = diag_flexe_core_module_cpb_egress_mac;
        module_list[5] = diag_flexe_core_module_flexe_macrx;

    }
    else if (module_id == diag_flexe_core_module_sar)
    {
        module_num = 5;
        module_list[0] = diag_flexe_core_module_mcmaxrx;
        module_list[1] = diag_flexe_core_module_b66sar_rx;
        module_list[2] = diag_flexe_core_module_mcmaxtx;
        module_list[3] = diag_flexe_core_module_b66sar_tx;
        module_list[4] = diag_flexe_core_module_cpb_egress_mac;
    }
    else if (module_id < diag_flexe_core_module_num)
    {
        module_num = 1;
        module_list[0] = module_id;
    }
    else
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    PRT_TITLE_SET("FlexE core counter");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("CounterName");
    PRT_COLUMN_ADD("Reg address");
    PRT_COLUMN_ADD("Values");

    for (ii = 0; ii < module_num; ii++)
    {
        module = module_list[ii];
        module_info = flexe_core_module_info + module;

        SHR_IF_ERR_EXIT(dnx_diag_flexe_core_module_counter_print(unit, in_hex, print_opt, module_info, prt_ctr));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
    dump b66switch configure
*/
shr_error_e
dnx_diag_flexe_core_dump_b66switch(
    int unit,
    sh_sand_control_t * sand_control)
{
    int ram_id;
    uint32 src_channel, dst_num = 0, dst_channel[3] = { 0, 0, 0 };
    uint32 dump_num, ii;
    char channel_name[16], dst_channels_name[64];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE core b66switch");
    PRT_COLUMN_ADD("src_bus");
    PRT_COLUMN_ADD("src channel");
    PRT_COLUMN_ADD("dst channels");

    for (ram_id = 0; ram_id < 3; ram_id++)
    {
        dump_num = 0;
        for (src_channel = 0; src_channel < DIAG_FLEXE_CHANNEL_NUM; src_channel++)
        {
            SHR_IF_ERR_EXIT(diag_flexe_core_b66_switch_get(unit, ram_id, src_channel, &dst_num, dst_channel));
            if (dst_num)
            {
                if (dump_num == 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("%d", ram_id);
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("");
                }

                PRT_CELL_SET("%d", src_channel);
                sal_memset(dst_channels_name, 0, sizeof(dst_channels_name));
                for (ii = 0; ii < dst_num; ii++)
                {
                    dnx_flexe_core_channel_name(dst_channel[ii], sizeof(channel_name), channel_name);

                    sal_snprintf(dst_channels_name + sal_strlen(dst_channels_name),
                                 sizeof(dst_channels_name) - sal_strlen(dst_channels_name), "%-16s", channel_name);
                }
                PRT_CELL_SET("%s", dst_channels_name);

                dump_num++;
            }
        }
        if (dump_num == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%d", ram_id);
            PRT_CELL_SET("");
            PRT_CELL_SET("");
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
    dump b66sar configure
*/
shr_error_e
dnx_diag_flexe_core_dump_b66sar(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 sar_channel;
    uint32 seg_en = 0, tx_chan_map = 0;
    uint32 mon_en = 0, rx_chan_map = 0;
    int dump_num;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE core sar rx/tx configure");
    PRT_COLUMN_ADD("SAR channel");
    PRT_COLUMN_ADD("TX(ILKN->SAR)");
    PRT_COLUMN_ADD("RX(SAR->ILKN)");

    dump_num = 0;
    for (sar_channel = 0; sar_channel < DIAG_FLEXE_CHANNEL_NUM; sar_channel++)
    {
        SHR_IF_ERR_EXIT(diag_flexe_core_b66_sar_tx_get(unit, sar_channel, &seg_en, &tx_chan_map));
        SHR_IF_ERR_EXIT(diag_flexe_core_b66_sar_rx_get(unit, sar_channel, &mon_en, &rx_chan_map));

        if ((seg_en) || (mon_en))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", sar_channel);

            if (seg_en)
            {
                PRT_CELL_SET("%d", tx_chan_map);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            if (mon_en)
            {
                PRT_CELL_SET("%d", rx_chan_map);
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            dump_num++;
        }
    }

    if (dump_num == 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("EMPTY");
        PRT_CELL_SKIP(2);
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
    dump flexe oam alarm(include sar & flexe alarm)
*/
shr_error_e
dnx_diag_flexe_core_dump_oam_alarm(
    int unit,
    sh_sand_control_t * sand_control)
{
    int channel;
    uint16 sar_alarm = 0, client_alarm = 0;
    int dump_num;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE core OAM ALARM status");
    PRT_COLUMN_ADD("channel");
    PRT_COLUMN_ADD("CLIENT ALARM");
    PRT_COLUMN_ADD("SAR ALARM");

    dump_num = 0;
    for (channel = 0; channel < DIAG_FLEXE_CHANNEL_NUM; channel++)
    {
        SHR_IF_ERR_EXIT(flexe_core_oam_alarm_status_get(unit, channel, &sar_alarm));
        SHR_IF_ERR_EXIT(flexe_core_oam_alarm_status_get(unit, channel, &client_alarm));

        if ((sar_alarm) || (client_alarm))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", channel);
            PRT_CELL_SET("0x%02X", sar_alarm);
            PRT_CELL_SET("0x%02X", client_alarm);

            dump_num++;
        }
    }

    if (dump_num == 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("EMPTY");
        PRT_CELL_SKIP(2);
    }
    PRT_COMMITX;

    cli_out("OAM alarm status bit:\n");
    cli_out("    Bit 0: RX_LPI;    Bit 1: RX_LF,   Bit 2: RX_RF,   Bit 3: BASE_OAM_LOS\n");
    cli_out("    Bit 4: RX_SDBIP;  Bit 5: RX_CRC,  Bit 6: RX_RDI,  Bit 7: RX_PERIOD_MISMATCH\n");
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
    dump flexe phy OH alarm
*/
shr_error_e
dnx_diag_flexe_core_dump_oh_alarm(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint8_t instance_id;
    uint16 oh_status = 0;
    int dump_num;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FlexE core OH phy ALARM status");
    PRT_COLUMN_ADD("instance_id");
    PRT_COLUMN_ADD("ALARM status");

    dump_num = 0;
    for (instance_id = 0; instance_id < DIAG_FLEXE_PHY_INSTANCE_NUM; instance_id++)
    {
        oh_status = 0;
        SHR_IF_ERR_EXIT(flexe_core_oh_alarm_status_get(unit, instance_id, &oh_status));
        if (oh_status)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", instance_id);
            PRT_CELL_SET("0x%04X", oh_status);

            dump_num++;
        }
    }

    if (dump_num == 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("EMPTY");
        PRT_CELL_SKIP(2);
    }

    PRT_COMMITX;

    cli_out("OH alarm status bit:\n");
    cli_out("    Bit 0:  GIDM_ALM;  Bit 1:  phy_num mismatch,  Bit 2:  LOF,        Bit 3:  LOM\n");
    cli_out("    Bit 4:  RPF;       Bit 5:  OH1_BLOCK_ALM,     Bit 6:  C_BIT_ALM,  Bit 7:  PMM\n");
    cli_out("    Bit 8:  CCAM;      Bit 9:  CCBM,              Bit 10: FlexE_LOGA\n");
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_flexe_core_dump_oam_cnt(
    int unit,
    int channel,
    sh_sand_control_t * sand_control)
{
    diag_flexe_oam_cnt_t oam_cnt;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&oam_cnt, 0, sizeof(oam_cnt));
    SHR_IF_ERR_EXIT(diag_flexe_core_oam_cnt_get(unit, channel, &oam_cnt));
    cli_out("FlexE channel %d oam counter info:\n", channel);
    cli_out("    %-30sbip8_cnt=%-12dbei_cnt=%d", "SAR performace counter:", oam_cnt.sar_bip8_cnt, oam_cnt.sar_bei_cnt);
    cli_out("    %-30sbip8_cnt=%-12dbei_cnt=%d\ns", "FlexE performace counter:",
            oam_cnt.flexe_bip8_cnt, oam_cnt.flexe_bei_cnt);
    cli_out("    %-30s%d\n", "SAR OAM counter:", oam_cnt.sar_packet_cnt);
    cli_out("    %-30s%d\n", "FlexE OAM counter:", oam_cnt.flexe_packet_cnt);
    cli_out("    %-30s%d\n", "SAR OAM BAS counter:", oam_cnt.sar_bas_packet_cnt);
    cli_out("    %-30s%d\n", "FlexE OAM BAS counter:", oam_cnt.flexe_bas_packet_cnt);

exit:
    SHR_FUNC_EXIT;
}
