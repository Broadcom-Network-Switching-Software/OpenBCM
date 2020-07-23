
/** \file src/bcm/dnx/cosq/ingress/compensation.h
 * 
 *
 * Compansation APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_SRC_COMPENSATION_H_INCLUDED_
#define _BCM_SRC_COMPENSATION_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <shared/gport.h>

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_voq_delta_set(
    int unit,
    int core,
    int voq,
    int delta);

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_voq_delta_get(
    int unit,
    int core,
    int voq,
    int *delta);

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_hdr_append_ptr_delta_set(
    int unit,
    int append_ptr,
    int delta);

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_hdr_append_ptr_delta_get(
    int unit,
    int append_ptr,
    int *delta);

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_in_port_delta_set(
    int unit,
    bcm_port_t src_port,
    int delta);

/*
 * see .c file
 */
int dnx_cosq_ingress_compensation_in_port_delta_get(
    int unit,
    bcm_port_t src_port,
    int *delta);

/*
 * see .c file
 */
shr_error_e dnx_cosq_ingress_compensation_init(
    int unit);

/*
 * see .c file
 */
shr_error_e dnx_cosq_ingress_compensation_deinit(
    int unit);

#endif /** _COMPENSATION_H_INCLUDED_ */
