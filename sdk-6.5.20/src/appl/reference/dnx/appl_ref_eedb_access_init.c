/** \file appl_ref_eedb_access_init.c
 * 
 *
 * L@ application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MDBDNX

 /*
  * Include files.
  * {
  */

#include <shared/utilex/utilex_seq.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/drv.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#include <src/appl/reference/dnx/appl_ref_sys_device.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include <soc/dnx/utils/dnx_ire_packet_utils.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <appl/reference/dnx/appl_ref_dynamic_port.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/*
 * See prototype definition for function description
 */

static shr_error_e
dnx_eedb_access_build_packet(
    int unit,
    uint32 in_port_0,
    uint32 in_port_1,
    uint32 dest_port,
    uint32 out_lif_0,
    uint32 out_lif_1,
    uint32 *packet_0,
    uint32 *packet_1)
{
    int ire_packet_buffer_size_bits = 512;
    int current_packet_size_bits = 80;
    int start_offset_bits = ire_packet_buffer_size_bits - current_packet_size_bits;
    int length_bits = 0;
    uint32 destination_port = 0xc0000 + dest_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Build ITMH OUTLIF extension per core */
    /** bits 0:21 - Extension value = outlif_values */
    length_bits = 22;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, out_lif_0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, out_lif_1));
    start_offset_bits += length_bits;
    /** bits 22:22 - Extension type = 0 (OutLif type) */
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 23:23 - Reserved = 0 */
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;

    /** ITMH */
    /** bits 0:0 - extension exists = 1*/
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 1));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 1));
    start_offset_bits += length_bits;
    /** bits 1:3 - TC = 0 */
    length_bits = 3;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 4:8 - Snoop traffic = 0 */
    length_bits = 5;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 9:29 - Destination = ports */
    length_bits = 21;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, destination_port));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, destination_port));
    start_offset_bits += length_bits;
    /** bits 30:31 - DP = 0 */
    length_bits = 2;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 32:32 - In_Mirror_disable = 0 */
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 33:34 - PPH Type = 0 */
    length_bits = 2;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 35:35 - Injected_AS_Extension_Present = 0 */
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 36:38 - Forward_Action_Strength = 7 */
    length_bits = 3;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 7));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 7));
    start_offset_bits += length_bits;
    /** bits 39:39 - Reserved = 0 */
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;

    /** PTCH */
    /** bits 0:7 - in pp port = in ports*/
    length_bits = 8;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, in_port_0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, in_port_1));
    start_offset_bits += length_bits;
    /** bits 8:14 - ??? = 0 */
    length_bits = 7;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;
    /** bits 15:15 - ITMH indication = 0*/
    length_bits = 1;
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_0, start_offset_bits, length_bits, 0));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field(packet_1, start_offset_bits, length_bits, 0));
    start_offset_bits += length_bits;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_eedb_access_init(
    int unit)
{
    int out_lif_0 = 0, out_lif_1 = 0;
    uint32 port_core_0 = 0, port_core_1 = 0;
    int packet_rate_khz = 1000;
    dnx_ire_packet_control_info_t packet_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * OutLifs values are stored in SW state
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock.global_lif_destination.get(unit, 0, &out_lif_0));
    SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock.global_lif_destination.get(unit, 1, &out_lif_1));

    /*
     * Ports Configurations
     */
    port_core_0 = 97;
    port_core_1 = 98;

    SHR_IF_ERR_EXIT(appl_dnx_dyn_port_add_rcy
                    (unit, port_core_0, 0, 2, BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2, BCM_SWITCH_PORT_HEADER_TYPE_ETH));
    SHR_IF_ERR_EXIT(appl_dnx_dyn_port_add_rcy
                    (unit, port_core_1, 1, 2, BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2, BCM_SWITCH_PORT_HEADER_TYPE_ETH));

    SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port_core_0, bcmPortControlDiscardEgress, TRUE));

    sal_memset(&packet_info, 0, sizeof(dnx_ire_packet_control_info_t));
    SHR_IF_ERR_EXIT(dnx_eedb_access_build_packet(unit, port_core_0, port_core_1, port_core_0,
                                                 (uint32) out_lif_0, (uint32) out_lif_1,
                                                 packet_info.packet_data_0, packet_info.packet_data_1));

    packet_info.packet_mode = DNX_IRE_PACKET_MODE_CONST_RATE;
    packet_info.pakcet_rate_khz = packet_rate_khz;
    packet_info.valid_bytes = 64;
    packet_info.ptc_0 = port_core_0;
    packet_info.ptc_1 = port_core_1;
    SHR_IF_ERR_EXIT(dnx_ire_packet_init(unit, &packet_info));

exit:
    SHR_FUNC_EXIT;
}

/** see appl_ref_eedb_access_init.h*/
shr_error_e
appl_dnx_eedb_access_cb(
    int unit,
    int *dynamic_flags)
{
#ifndef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_eedb_bank_traffic_lock))
    {
        *dynamic_flags = 0;
    }
    else
#endif
    {
        *dynamic_flags = UTILEX_SEQ_STEP_F_SKIP | UTILEX_SEQ_STEP_F_VERBOSE;
    }
    return _SHR_E_NONE;

}
