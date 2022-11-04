/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * KNETSync Management
 */

#if defined(BCM_DNX_SUPPORT)

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_KNETSYNC

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/field/field_entry.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_onesync.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/*#include <bcm_int/dnx_dispatch.h> */

#endif /* defined(BCM_DNX_SUPPORT) */

#include <shared/bsl.h>

#include <soc/defs.h>

#include <sal/core/libc.h>
#include <shared/pack.h>
#include <soc/scache.h>
#include <soc/uc.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/onesync.h>

#include <bcm/error.h>
#include <bcm/types.h>

#include <bcm_int/common/onesync_common.h>
#include <bcm_int/common/knetsync.h>

/*
 * Function:
 *      bcm_esw_knetsync_init
 * Purpose:
 *      Initialize the KNETSync subsystem and Firmware.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_dnx_knetsync_init(
    int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    _bcm_knetsync_info_t knetsync_info;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_config_t config;
    uint32 flags;
    uint32 port_ndx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    sal_memset(&knetsync_info, 0, sizeof(knetsync_info));
    /*
     * COSQ Port (204) details 
     */
    knetsync_info.uc_port_num = dnx_data_onesync.onesync_firmware.knetsync_cosq_port_get(unit);

    SHR_IF_ERR_EXIT(bcm_dnx_port_get(unit, knetsync_info.uc_port_num, &flags, &interface_info, &mapping_info));
    /*
     * Sysport - QAX, QMX, QUX, PP Port - JR2 series devices 
     */
    knetsync_info.uc_port_sysport = mapping_info.pp_port;

    /*
     * Treat first CPU port in list as Host CPU port 
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.cpu, port_ndx)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, port_ndx, &flags, &interface_info, &mapping_info));
        if (interface_info.interface == BCM_PORT_IF_CPU)
        {
            /*
             * First pport in the list is host cpu port 
             */
            knetsync_info.host_cpu_port = port_ndx;
            /*
             * Sysport - QAX, QMX, QUX, PP Port - JR2 series devices 
             */
            knetsync_info.host_cpu_sysport = mapping_info.pp_port;
            break;
        }
    }

    bsl_printf("bcm_dnx_knetsync_init: uc_port_num %u uc_port_sysport %u host_cpu_port %u host_cpu_sysport %u\n",
               knetsync_info.uc_port_num, knetsync_info.uc_port_sysport,
               knetsync_info.host_cpu_port, knetsync_info.host_cpu_sysport);

    /*
     * UDH Header Information 
     */
    knetsync_info.udh_len = 1;

    _func_rv = bcm_common_knetsync_init(unit, knetsync_info);

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
#endif
}

/*
 * Function:
 *      bcm_esw_knetsync_deinit
 * Purpose:
 *      Deinitialize the knetsync networking subsystem and firmware.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_dnx_knetsync_deinit(
    int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    rv = bcm_common_knetsync_deinit(unit);

    return BCM_SUCCESS(rv) ? BCM_E_NONE : rv;
#endif
}

#if defined(INCLUDE_KNETSYNC)
/* { */
/**
 * \brief -  bcm_common_onesync_dnx_system_info_send, it's a internal funtion
 *          Purpose of this function is to update onesync firmware about
 *          dnx system header & other information to facilitate ptp packet porocessing
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int
_bcm_dnx_onesync_device_config_read(
    int unit,
    int uc_num)
{
    uint32 entry_handle_id;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_t in_tm_port_pp;
    bcm_port_t in_logical_port;
    int cpu_channel;
    int onesync_cosq_port;
    uint32 flags;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint8 tmp_udh_enable = 0;

    onesync_dnx_jr2_devices_system_info_t *jr2devs_sys_info = ONESYNC_INFO(unit)->jr2devs_system_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    memset(jr2devs_sys_info, 0x0, sizeof(onesync_dnx_jr2_devices_system_info_t));

    /*
     * read DNX Headers information
     */
    jr2devs_sys_info->system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                               &(jr2devs_sys_info->ftmh_lb_key_ext_size));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                               &jr2devs_sys_info->ftmh_stacking_ext_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE,
                               &jr2devs_sys_info->pph_base_size);

    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE, &tmp_udh_enable);

    jr2devs_sys_info->pph_lif_ext_size[0] = 0;
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 0,
                               &jr2devs_sys_info->pph_lif_ext_size[1]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 1,
                               &jr2devs_sys_info->pph_lif_ext_size[2]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 2,
                               &jr2devs_sys_info->pph_lif_ext_size[3]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF_EXT_SIZE, INST_SINGLE,
                               &jr2devs_sys_info->pph_lif_ext_size[4]);
    jr2devs_sys_info->pph_lif_ext_size[5] = 0;
    jr2devs_sys_info->pph_lif_ext_size[6] = 0;
    jr2devs_sys_info->pph_lif_ext_size[7] = 0;

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    jr2devs_sys_info->udh_enable = (uint32) tmp_udh_enable;

    if (dnx_data_field.udh.ext_supported_get(unit) == FALSE)
    {
        /** Suppose JR2 always enable UDH since at least UDH_BASE exists.*/
        /*
         * Each 2 bit of the UDH base mapped:
         * 0: Mapped to UDH Size: 0b
         * 1: Mapped to UDH Size: 4B
         * 2: Mapped to UDH Size: 4B
         * 3: Mapped to UDH Size: 4B
         */
        jr2devs_sys_info->udh_data_lenght_per_type[0] = dnx_data_field.udh.type_0_length_get(unit);
        jr2devs_sys_info->udh_data_lenght_per_type[1] = dnx_data_field.udh.type_1_length_get(unit);
        jr2devs_sys_info->udh_data_lenght_per_type[2] = dnx_data_field.udh.type_2_length_get(unit);
        jr2devs_sys_info->udh_data_lenght_per_type[3] = dnx_data_field.udh.type_3_length_get(unit);
    }
    else
    {
    }

    /*
     * COSQ Port (204) port details
     */
    onesync_cosq_port = dnx_data_onesync.onesync_firmware.knetsync_cosq_port_get(unit);

    /** Convert gport to logical port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, onesync_cosq_port,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
    in_logical_port = gport_info.local_port;

    SHR_IF_ERR_EXIT(bcm_dnx_port_get(unit, onesync_cosq_port, &flags, &interface_info, &mapping_info));
    in_tm_port_pp = mapping_info.pp_port;

    SHR_IF_ERR_EXIT(dnx_algo_port_in_channel_get(unit, in_logical_port, &cpu_channel));

    jr2devs_sys_info->cosq_port_pp_port = in_tm_port_pp;
    jr2devs_sys_info->cosq_port_cpu_channel = cpu_channel;

    /*
     * Send the JR2 device information to ptp-clock-driver
     */

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* } */

#else /* !defined(INCLUDE_KNETSYNC) */

int
_bcm_dnx_onesync_device_config_read(
    int unit,
    int uc_num)
{
    return BCM_E_UNAVAIL;
}
#endif
