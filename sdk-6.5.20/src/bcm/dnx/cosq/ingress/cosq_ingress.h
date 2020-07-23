/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/ingress/cosq_ingress.h
 * Reserved.$ 
 */

#ifndef _SRC_BCM_DNX_COSQ_INGRESS_H_INCLUDED_
/*
 * { 
 */
#define _SRC_BCM_DNX_COSQ_INGRESS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * 
 * \brief Function to retrieve the correct dbal table id for the 
 *        ingress shapers configurations
 *  
 * \param unit - unit
 * \param gport - ingress receive gport
 * \param table_id - table_id
 * 
 * \return int 
 */
int dnx_ingress_receive_table_id_get(
    int unit,
    bcm_gport_t gport,
    int *table_id);

/**
 * 
 * \brief -Function to configure CPU, NIF and RCY ingress 
 *        interfaces
 *  
 * \param unit - unit
 * \param dbal_table_id - relevant dbal_table_id retrieved with
 *                      with dnx_ingress_receive_table_id_get
 * \param core_id - core
 * \param grant_size - grant_size
 * \param timer_cycles - timer cycles to set
 * 
 * \return shr_error_e 
 */
shr_error_e dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_set(
    int unit,
    int dbal_table_id,
    int core_id,
    uint32 grant_size,
    uint32 timer_cycles);

/**
 * \brief - Function to get actual ingress rate grant size and 
 *        timer cycles
 *  
 * \param unit - unit
 * \param dbal_table_id - relevant dbal_table_id retrieved with
 *                      with dnx_ingress_receive_table_id_get
 * \param core - core
 * \param grant_size - grant_size value
 * \param timer_cycles - timer_cycles value 
 * 
 * \return shr_error_e 
 */
shr_error_e dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_get(
    int unit,
    int dbal_table_id,
    int core,
    uint32 *grant_size,
    uint32 *timer_cycles);

#endif /** _SRC_BCM_DNX_COSQ_INGRESS_H_INCLUDED_ */
