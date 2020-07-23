/*! \file bcmcth_tnl_decap_internal.h
 *
 * TNL_IPV4/IPV6/MPLS_ENCAP Custom Handler header file
 * This file contains APIs, defines for IPv4, IPv6, MPLS encap
 * handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_DECAP_INTERNAL_H
#define BCMCTH_TNL_DECAP_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>


/*! TNL decap LT to HW entry map. */
typedef int (*tnl_mpls_decap_hw_entry_map_f)(int unit,
                                             bcmltd_sid_t lt_sid,
                                             bcmdrd_sid_t sid,
                                             const bcmltd_fields_t *in,
                                             uint32_t **ekw,
                                             uint32_t **edw);
/*! TNL decap HW to LT entry map. */
typedef int (*tnl_mpls_decap_lt_entry_map_f)(int unit,
                                             bcmltd_sid_t lt_sid,
                                             bcmdrd_sid_t sid,
                                             uint32_t **ekw,
                                             uint32_t **edw,
                                             bcmltd_fields_t *out);

/*! Structure to define TNL decap driver attributes. */
typedef struct bcmcth_tnl_decap_drv_s {
    /*! Number of PTs associated with this LT. */
    int         sid_count;
    /*! List of PT names. */
    uint32_t                        *sid_list;
    /*! List of PT modes. */
    bcmptm_rm_hash_entry_mode_t     *entry_attrs_list;
    /*! Function pointer to map TNL_MPLS_DECAP LT entry to HW entry. */
    tnl_mpls_decap_hw_entry_map_f   mpls_decap_lt_to_hw_map;
    /*! Function pointer to map TNL_MPLS_DECAP HW entry to LT entry. */
    tnl_mpls_decap_lt_entry_map_f   mpls_decap_hw_to_lt_map;
} bcmcth_tnl_decap_drv_t;

/*! BCMCTH_TNL decap driver for this unit. */
#define BCMCTH_TNL_DECAP_DRV(unit)                  (tnl_decap_drv[unit])
/*! BCMCTH_TNL decap driver PT count for this unit. */
#define BCMCTH_TNL_DECAP_SID_COUNT(unit)            BCMCTH_TNL_DECAP_DRV(unit)->sid_count
/*! BCMCTH_TNL decap driver PT list for this unit. */
#define BCMCTH_TNL_DECAP_SID_LIST(unit)             BCMCTH_TNL_DECAP_DRV(unit)->sid_list
/*! BCMCTH_TNL decap driver PT RM entry modes for this unit. */
#define BCMCTH_TNL_DECAP_ENTRY_ATTRS_LIST(unit)     BCMCTH_TNL_DECAP_DRV(unit)->entry_attrs_list
/*! Function pointer to map TNL_MPLS_DECAP LT entry to HW entry for this unit. */
#define BCMCTH_TNL_MPLS_DECAP_LT_TO_HW_MAP(unit)    BCMCTH_TNL_DECAP_DRV(unit)->mpls_decap_lt_to_hw_map
/*! Function pointer to map TNL_MPLS_DECAP HW entry to LT entry for this unit. */
#define BCMCTH_TNL_MPLS_DECAP_HW_TO_LT_MAP(unit)    BCMCTH_TNL_DECAP_DRV(unit)->mpls_decap_hw_to_lt_map

/*! Convert uint32 array into one uint64 variable. */
#define BCMCTH_TNL_DECAP_UINT32_TO_UINT64(val32, val64) \
    do {                                                \
        val64 = val32[1];                               \
        val64 = ((val64 << 32) | val32[0]);             \
    } while(0)

/*! Convert uint64 variable into uint32 array. */
#define BCMCTH_TNL_DECAP_UINT64_TO_UINT32(val64, val32) \
    do {                                                \
        val32[0] = (uint32_t)val64;                     \
        val32[1] = (uint32_t)(val64 >> 32);             \
    } while(0)

/*!
 * \brief TNL_MPLS_DECAP validation function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          LT opcode.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Data Validation of received arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_validate(int unit,
                               bcmlt_opcode_t opcode,
                               bcmltd_sid_t sid,
                               const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP insert function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Insert values to physical table with entry allocation.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_insert(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP lookup function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              Input Values.
 * \param [in]  sid             Logical table ID.
 * \param [out] out             Fetched Field Values.
 *
 * Lookup values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_lookup(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_sid_t sid,
                             bcmltd_fields_t *out);
/*!
 * \brief TNL_MPLS_DECAP delete function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Delete entry from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_delete(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP update function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  sid             Logical table ID.
 * \param [in]  in              Input Values.
 *
 * Update existing entry with new values in physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t sid,
                             const bcmltd_fields_t *in);
/*!
 * \brief TNL_MPLS_DECAP traverse function
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              Input Values.
 * \param [in]  sid             Logical table ID.
 * \param [out] out             Fetched Field Values.
 *
 * Lookup values from physical table.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
int
bcmcth_tnl_decap_mpls_traverse(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_sid_t sid,
                               bcmltd_fields_t *out);

/*!
 * \brief Set pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Chip driver structure.
 *
 * \return SHR_E_NONE
 */
extern int
bcmcth_tnl_decap_drv_set(int unit, bcmcth_tnl_decap_drv_t *drv);

/*!
 * \brief Get pointer to tunnel decap driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern int
bcmcth_tnl_decap_drv_get(int unit, bcmcth_tnl_decap_drv_t **drv);

#endif /* BCMCTH_TNL_DECAP_INTERNAL_H */
