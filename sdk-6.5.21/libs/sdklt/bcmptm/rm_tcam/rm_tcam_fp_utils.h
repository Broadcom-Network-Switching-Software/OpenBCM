/*! \file rm_tcam_fp_utils.h
 *
 * Utils, defines internal to FP TCAMs resource manager.
 * This file contains utils, defines which are internal to
 * FP TCAMs resource manager(i.e. these are not visible outside RM-TCAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_FP_UTILS_H
#define RM_TCAM_FP_UTILS_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include "rm_tcam_prio_eid.h"


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Function declarations (prototypes)
 */

/*!
 * \brief Fetch the address to TCAM entry information of the given LTID.
 * \n This address points to address of zeroth TCAM index entry information.
 * \n To get the address to TCAM entry information of any specific TCAM index,
 * \n increment this pointer by those many TCAM indexes as memory created to
 * \n hold TCAM entry information of all TCAM indexes is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] entry_info double pointer to hold pointer to
 * \n           start address of TCAM entry information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_info_get(int unit, bcmltd_sid_t ltid,
                                      bcmptm_rm_tcam_lt_info_t *ltid_info,
                                      void *attrs,
                                      bcmptm_rm_tcam_prio_eid_entry_info_t **entry_info);

/*!
 * \brief Fetch the address to TCAM entry hash of the given Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid..
 * \n This address points to the address of hash zero. To get the address to
 * \n any other valid hash, increment this start pointer by hash times as memory
 * \n created to hold the hash information of all valid hash values is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Internal meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_hash_get(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Fetch the TCAM flags for a given entry
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Internal meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_flags_get(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Fetch the number of free entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Internal meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_get(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Fetch the number of valid entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input output meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_valid_count_get(int unit,
                                             bcmptm_rm_tcam_entry_iomd_t *iomd);


/*!
 * \brief Fetch the hash size of entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Internal meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_hash_size_get(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Increment the entry count in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry logical index
 * \param [in] attrs Entry Attribute information.
 * \param [in] iomd Internal meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_incr(int unit,
                                             uint32_t ent_logical_idx,
                                             bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Decrement the entry count in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry logical index
 * \param [in] attrs Entry Attribute information.
 * \param [in] iomd Internal meta data
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_free_count_decr(int unit,
                                             uint32_t ent_logical_idx,
                                             bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Fetch the number of pipes in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] pipe_count Number of pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_pipe_count_get(int unit,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Fetch the total number of entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Entry Attribute information.
 * \param [out] total_count Number of entries in the TCAM.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_fp_entry_total_count_get(int unit, bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         void *entry_attrs,
                                         uint32_t *total_count);
/*!
 * \brief Update entry_info_arr in iomd structure.
 *
 * \param [in] unit Logical device id
 * \param [in] entry_info_ptr entry_info segment start pointer
 * \param [out] iomd entry_info_arr.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_iomd_entry_info_arr_update(int unit,
                        bcmptm_rm_tcam_prio_eid_entry_info_t *entry_info_ptr,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief check there exists some valid entries in a given FP LTID.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [out] 1 if there exists valid groups for the given FP ltid.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_entries_exist(int unit, bcmltd_sid_t ltid,
                                         uint8_t *entry_exists);

#endif /* RM_TCAM_FP_UTILS_H */
