/** \file dnx_port_nif_calendar_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_CALENDAR_INTERNAL_H_
#define _PORT_NIF_CALENDAR_INTERNAL_H_

#include <soc/sand/shrextend/shrextend_error.h>

typedef enum
{
    DNX_PORT_NIF_CALENDAR_FIRST = 0,

    DNX_PORT_NIF_ARB_RX_QPM_CALENDAR = DNX_PORT_NIF_CALENDAR_FIRST,

    DNX_PORT_NIF_ARB_RX_SCH_CALENDAR = 1,

    DNX_PORT_NIF_ARB_TX_PPM0_CALENDAR = 2,

    DNX_PORT_NIF_ARB_TX_PPM1_CALENDAR = 3,

    DNX_PORT_NIF_ARB_TX_PPM2_CALENDAR = 4,

    DNX_PORT_NIF_ARB_TX_PPM3_CALENDAR = 5,

    DNX_PORT_NIF_ARB_TX_PPM4_CALENDAR = 6,

    DNX_PORT_NIF_ARB_TX_PPM5_CALENDAR = 7,

    DNX_PORT_NIF_ARB_TX_PPM6_CALENDAR = 8,

    DNX_PORT_NIF_ARB_TX_PPM7_CALENDAR = 9,

    DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR = 10,

    DNX_PORT_NIF_OFT_CALENDAR = 11,

    DNX_PORT_NIF_CALENDAR_NOF
} dnx_port_nif_calendar_type_e;

shr_error_e dnx_port_calendar_init(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);
shr_error_e dnx_port_calendar_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);

#endif /* _PORT_NIF_CALENDAR_INTERNAL_H_ */
