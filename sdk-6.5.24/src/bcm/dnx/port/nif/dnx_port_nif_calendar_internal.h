/** \file dnx_port_nif_calendar_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_CALENDAR_INTERNAL_H_
#define _PORT_NIF_CALENDAR_INTERNAL_H_

#include <shared/shrextend/shrextend_error.h>

typedef enum
{
    DNX_PORT_NIF_CALENDAR_FIRST = 0,

    DNX_PORT_NIF_ARB_RX_QPM_CALENDAR = DNX_PORT_NIF_CALENDAR_FIRST,

    DNX_PORT_NIF_ARB_RX_SCH_CALENDAR = 1,

    DNX_PORT_NIF_ARB_TX_ETH_CALENDAR = 2,

    DNX_PORT_NIF_ARB_TX_TMAC_CALENDAR = 3,

    DNX_PORT_NIF_OFT_CALENDAR = 4,

    DNX_PORT_NIF_CALENDAR_NOF
} dnx_port_nif_calendar_type_e;

shr_error_e dnx_port_calendar_set(
    int unit,
    dnx_port_nif_calendar_type_e calendar_type);

#endif /* _PORT_NIF_CALENDAR_INTERNAL_H_ */
