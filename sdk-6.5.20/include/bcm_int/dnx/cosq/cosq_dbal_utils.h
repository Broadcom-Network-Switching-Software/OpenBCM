/**
 * \file cosq_dbal_utils.h
 * 
 * 
 * Generic DBAL access functionality
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef COSQ_DBAL_UTILS_H_INCLUDED
/* { */
#define COSQ_DBAL_UTILS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/core/libc.h>

/*
 * Definess:
 * {
 */
/*
 * } 
 */
/*
 * Typedefs:
 * {
 */
typedef struct
{
    dbal_fields_e id;
    uint32 value;
} dnx_cosq_dbal_field_t;
/*
 * } 
 */
/*
 * Functions
 * {
 */
/**
 * \brief - set a value to dbal table entry
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] nof_keys - number of keys in the dbal table
 * \param [in] key - array of keys (id and value)
 * \param [in] result - dbal result field (id and value)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_entry_set(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result);

/**
 * \brief - get a value from dbal table entry
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] nof_keys - number of keys in the dbal table
 * \param [in] key - array of keys (id and value)
 * \param [in,out] result - dbal result field (id and value)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_entry_get(
    int unit,
    dbal_tables_e table_id,
    int nof_keys,
    dnx_cosq_dbal_field_t key[],
    dnx_cosq_dbal_field_t * result);

/**
 * \brief - return TRUE iff a key exists in the table
 *
 * \param [in] unit - unit index
 * \param [in] table_id - dbal table ID
 * \param [in] key_field_id - ID of key field
 * \param [out] exists - does key  exist in the table
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_cosq_dbal_key_exists_in_table(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e key_field_id,
    int *exists);
/* 
 * }
 */
/* } */
#endif
