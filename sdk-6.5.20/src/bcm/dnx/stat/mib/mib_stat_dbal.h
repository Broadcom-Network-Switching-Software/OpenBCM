/** \file mib_stat_dbal.h
 * General MIB - contains internal functions and definitions for
 * MIB feature support 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef MIB_STAT_DBAL_H_INCLUDED
/*
 * {
 */
#define MIB_STAT_DBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/stat/mib/mib_stat.h>

shr_error_e mib_stat_init(
    int unit);

shr_error_e mib_stat_fabric_init(
    int unit);

shr_error_e mib_stat_fabric_fmac_hw_init(
    int unit,
    int fmac_idx);

shr_error_e dnx_mib_fmac_stat_dbal_get(
    int unit,
    uint32 fmac_idx,
    uint32 lane_idx,
    int hw_counter_id,
    uint64 *val);

/**
* \brief
*   Get mib counters dbal table for a given eth nif port and dbal field
* \param [in] unit - unit id
* \param [in] port - logic port
* \param [in] field_id - counter dbal field id
* \param [out] table_id - dbal table id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_mib_nif_eth_stat_dbal_table_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    dbal_tables_e * table_id);

shr_error_e dnx_mib_nif_eth_stat_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 *val);

shr_error_e dnx_mib_nif_eth_stat_dbal_set(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 val);

/**
 * \brief - This function get dbal field id by counter id
 * 
 * \param [in]  unit     - unit id.
 * \param [in]  port - logic port num.
 * \param [in]  counter_id - control counter id.
 * \param [out] field_id - dbal_field
 * \param [out] clear_on_read - indication whether the field is clear on read
 *   
 * \return
 *   shr_error_e
 */
shr_error_e dnx_mib_counter_id_to_dbal_field_id_get(
    int unit,
    int port,
    dnx_mib_counter_t counter_id,
    dbal_fields_e * field_id,
    uint32 *clear_on_read);

/**
 * \brief - Initialize the database for storing counter values per row
 * 
 * \param [in]  unit     - unit id.
 * \return
 *   shr_error_e
 */
void dnx_mib_counter_values_database_init(
    int unit);
/**
 * \brief - Deinitialize the database for storing counter values per row
 * 
 * \param [in]  unit     - unit id.
 * \return
 *   shr_error_e
 */
void dnx_mib_counter_values_database_deinit(
    int unit);

/**
 * \brief - This function get one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] counter_id - counter id.
 * \param [in] port - logic port num.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e dnx_mib_nif_cdu_optimized_stat_dbal_get(
    int unit,
    bcm_port_t port,
    int counter_id,
    uint64 *val);

/**
 * \brief - This function clear cdmib counter per row
 * 
 * \param [in] unit     - unit id.
 * \param [in] counter_id - counter id.
 * \param [in] port - logic port num.
 *   
 * \return
 *   shr_error_e
 */

shr_error_e dnx_mib_nif_cdu_optimized_stat_dbal_clear(
    int unit,
    bcm_port_t port,
    int counter_id);

#endif /* INTERNAL_OAM_INCLUDED */
