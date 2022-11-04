/**
 * \file bcm_int/dnx/l2/l2_addr.h
 * Internal DNX L2 APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef L2_ADDR_H_INCLUDED
/*
 * {
 */
#define L2_ADDR_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>

#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm/types.h>
/*
 * }
 */

/*
 * DEFINES
 * {
 */
/*
 * This flags represents all the MAC Tables result-type fields where each result-type hold a subset of the following
 * flags
 */
#define L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF             SAL_BIT(0)
#define L2_ADDR_DBAL_FIELD_SOURCE_DROP              SAL_BIT(1)
#define L2_ADDR_DBAL_FIELD_EEI                      SAL_BIT(2)
#define L2_ADDR_DBAL_FIELD_STAT_OBJECT              SAL_BIT(3)
#define L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF_2ND         SAL_BIT(4)

/*
 * Maps each result-type to the flags that represents the special fields that it is composed of
 */
#define L2_ADDR_FWD_MACT_RESULT_DEST_LIF_STAT (L2_ADDR_DBAL_FIELD_SOURCE_DROP | L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF | L2_ADDR_DBAL_FIELD_STAT_OBJECT)
#define L2_ADDR_FWD_MACT_RESULT_DEST_STAT (L2_ADDR_DBAL_FIELD_SOURCE_DROP | L2_ADDR_DBAL_FIELD_STAT_OBJECT)
#define L2_ADDR_FWD_MACT_RESULT_DOUBLE_OUTLIF (L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF | L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF_2ND)
#define L2_ADDR_FWD_MACT_RESULT_EEI_FEC (L2_ADDR_DBAL_FIELD_EEI)
#define L2_ADDR_FWD_MACT_RESULT_NO_OUTLIF (0)
#define L2_ADDR_FWD_MACT_RESULT_SINGLE_OUTLIF (L2_ADDR_DBAL_FIELD_GLOB_OUT_LIF)
/*
 * In IVL mode the user should set a VLAN value.
 * Since there is no VLAN field in bcm_l2_addr_t we are currently using the modid for setting the VLAN.
 * modid!=0 is used to distinguish between MACT and MACT_IVL.
 */
#define L2_ADDR_IS_MACT_IVL(modid) modid ? TRUE : FALSE

/* L2 MACT vlan options */
typedef enum l2_addr_mact_vlan_learning_type_e
{
    l2AddrMactSvl,
    l2AddrMactIvl
} l2_addr_mact_vlan_learning_type_t;
/*
 * }
 */

/*
  * Internal functions.
  * {
  */

/**
 * \brief - Get an handle to a FWD_MACT table. The handle should have one entry fetched in it.
 * The function is retrieving the entry and parse it to l2addr struct.
 *
 * \param [in] unit - unit id.
 * \param [in] entry_handle_id - handle to FWD_MACT with one entry fetched.
 * \param [in] table_id - table ID associated with the entry_handle_id.
 * \param [in] l2addr - bcm_l2_addr_t struct that will be filled by the function.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_addr_from_dbal_to_struct(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    bcm_l2_addr_t * l2addr);

/**
 * \brief - Get destination information according to the l2 addr struct given
 * \param [in] unit - unit id.
 * \param [in] flags - used to distinguish between SA and DA MACT in devices where the two MACTs are separated.
 *                     is L2_ADDR_ADD_LRN flag is set then the output will be set according to the learning MACT,
 *                     if not then it'll be set according to the forwarding MACT
 * \param [in] l2addr - bcm_l2_addr_t struct.
 * \param [out] destination - the destination.
 * \param [out] outlif - the outlif if relevant.
 * \param [out] result_type - the result type.
 * \param [out] eei - the eei fec if relevant.
 * \param [out] field_flags - The flags that represents the result type's fields.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_convert_l2_addr_to_dest(
    int unit,
    uint32 flags,
    bcm_l2_addr_t * l2addr,
    uint32 *destination,
    uint32 *outlif,
    uint32 *result_type,
    uint32 *eei,
    uint32 *field_flags);

/**
 * \brief -
 * This function sets the DBAL commit flags for static transplant-able L2 entry.
 *
 * \param [in]  unit - relevant unit
 * \param [in]  dbal_commit_flags - DBAL commit flags to use when adding static transplant-able L2 entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
shr_error_e dnx_l2_static_transplant_commit_flags_set(
    int unit,
    uint32 dbal_commit_flags);

/**
 * \brief -
 * This function gets the DBAL commit flags for static transplant-able L2 entry.
 *
 * \param [in]  unit - relevant unit
 * \param [in]  dbal_commit_flags - DBAL commit flags to use when adding static transplant-able L2 entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
shr_error_e dnx_l2_static_transplant_commit_flags_get(
    int unit,
    uint32 *dbal_commit_flags);
/*
 * }
 */
#endif /* L2_ADDR_H_INCLUDED */
