/*
 * $Id: dnx_ire_packet_utils.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_IRE_PACKET_UTILS_H_INCLUDED

#define DNX_IRE_PACKET_UTILS_H_INCLUDED

#include <sal/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#define DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES             64
#define DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS             BYTES2WORDS(DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_BYTES)

typedef enum
{
    DNX_IRE_PACKET_MODE_SINGLE_SHOT = 0,
    DNX_IRE_PACKET_MODE_CONST_RATE = 2
} dnx_ire_packet_mode_e;

typedef struct
{

    dnx_ire_packet_mode_e packet_mode;

    int pakcet_rate_khz;

    int nof_packets;

    int valid_bytes;

    uint32 packet_data_0[DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS];
    uint32 packet_data_1[DNX_IRE_PACKET_MAX_PACKET_SIZE_IN_WORDS];

    int ptc_0;
    int ptc_1;

} dnx_ire_packet_control_info_t;

shr_error_e dnx_ire_packet_init(
    int unit,
    dnx_ire_packet_control_info_t * packet_info);

shr_error_e dnx_ire_packet_start(
    int unit);

shr_error_e dnx_ire_packet_end(
    int unit);

#endif
