/** \file dnx_port_nif_ofr_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_NIF_OFR_INTERNAL_H_
#define _PORT_NIF_OFR_INTERNAL_H_

#include <bcm/types.h>
#include <bcm/port.h>
#include <shared/shrextend/shrextend_error.h>

#define DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerLow)

#define DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerHigh)

#define DNX_PORT_NIF_OFR_PRIORITY_IS_LOW_OR_HIGH(priority_group) \
        ((DNX_PORT_NIF_OFR_PRIORITY_IS_LOW(priority_group)) || (DNX_PORT_NIF_OFR_PRIORITY_IS_HIGH(priority_group)))

#define DNX_PORT_NIF_OFR_PRIORITY_IS_TDM(priority_group) \
        (priority_group.sch_priority == bcmPortNifSchedulerTDM)

shr_error_e dnx_port_ofr_info_init(
    int unit);

#endif /* _PORT_NIF_OFR_INTERNAL_H_ */
