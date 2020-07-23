/*! \file bcmptm_ser_misc_internal.h
 *
 * Interface functions for TCAM entry mode or slice mode change
 *
 * SER TCAM S/W scan the entry with right mode (narrow or wide).
 *
 * Interface functions only can be used by SER sub-component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SER_MISC_INTERNAL_H
#define BCMPTM_SER_MISC_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmptm/bcmptm_internal.h>


/*!
 * \brief Allows RM to provide info about Tcam slices that it is managing.
 * \n Can be used by both FP, L3_DEFIP RM.
 *
 * \param [in] unit Unit number
 * \param [in] slice_depth Array with 'num_slice' elements. Nth element
 * specifies depth of Nth slice. Slice depth could be different for different
 * slices in IFP.
 * \param [in] num_slice Num of elements in slice_depth array.
 * \param [in] sid_list Array with 'num_sid' elements. Identifies narrow, wide
 * view that will be used to access the slices
 * \param [in] num_sid Num of elements in sid_list array.
 * \param [in] slice_mode Initial slice_mode consistent with 'initial'
 * slice_mode as maintained in RM. Same value for all slices.
 * \param [in] is_overlay_tcam In the same slice, some entries can be used as 'wide' mode,
 * others can be used as 'narrow' mode, e.g: L3_DEFIPm/L3_DEFIP_PAIR_128m.
 *
 * \param [out] slice_group_id ID by which RM can identify slice mode resources
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_group_register(int unit,
                                int *slice_depth, int num_slice,
                                bcmdrd_sid_t *sid_list, int num_sid,
                                bcmptm_rm_slice_mode_t slice_mode,
                                int is_overlay_tcam,

                                uint32_t *slice_group_id);

/*!
 * \brief Provides mutex corresponding to slice_group_id
 * \n Will be called by WAL reader before sending slice_mode change related ops
 * to HW.
 *
 * \param [in] unit Unit number
 * \param [in] slice_group_id ID of slice_group managed by RM
 *
 * \param [out] slice_mode_mutex mutex corresponding to slice_group_id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_mode_mutex_info(int unit, uint32_t slice_group_id,
                                 sal_mutex_t *slice_mode_mutex);

/*!
 * \brief Provides mutex corresponding to tables for which we track mode on per
 * entry-basis (L3_DEFIP).
 * \n SERC logic maintains common mutex for L3_DEFIP tables related slice_mode,
 * entry_mode changes.
 *
 * \param [in] unit Unit number.
 *
 * \param [out] entry_mode_group_id ID of corresponding L3_DEFIP tables slice
 * group
 * \param [out] entry_mode_mutex mutex corresponding to L3_DEFIP tables.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_entry_mode_mutex_info(int unit,
                                 uint32_t *entry_mode_group_id,
                                 sal_mutex_t *entry_mode_mutex);

/*!
 * \brief Provides slice_mode_change info corresponding to slice_group_id
 *
 * \param [in] unit Unit number
 * \param [in] slice_mode_change_info Info needed for SERC to update slice_mode
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_slice_mode_change(int unit,
                             bcmptm_rm_slice_change_t *slice_mode_change_info);

/*!
 * \brief Provides entry_mode_change info
 *
 * \param [in] unit Unit number
 * \param [in] entry_mode_change_info Info needed for SERC to update entry_mode
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_entry_mode_change(int unit,
                             bcmptm_rm_entry_mode_change_t *entry_mode_change_info);

/*!
 * \brief This API is used only by TCAM memscan, if index is invalid of wide or narrow view PT,
 * index will be adjust as valid index. This API is used for IFPm.
 *
 * \param [in] unit                        Unit number
 * \param [in] sid                         SID
 * \param [in & out] index             the beginning of scan index
 * \param [out] stride                   Number of scan index
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_index_adjust(int unit, bcmdrd_sid_t sid,
                              int *index, int *stride);

/*!
 * \brief This API is used only by TCAM memscan, if index is located in narrow slices,
 * the 'stride' will be 'slice_depth-index % slice_depth'.
 * If the index is located in wide slices, and the corresponding entry mode of index
 * is matched with 'sid', the 'stride' will be 1, otherwise, the 'stride' will be 0.
 * This API is used for L3_DEFIPm/L3_DEF_IP_PAIR_128m only.
 *
 * \param [in] unit                        Unit number
 * \param [in] sid                         SID
 * \param [in] index                      the beginning of scan index
 * \param [out] stride                   Number of scan index
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_overlay_index_stride_get(int unit, bcmdrd_sid_t sid,
                                    int index, int *stride);

/*!
 * \brief Print all slice mode information.
 *
 * \param [in] unit                        Unit number
 * \param [in] overlay_info_print     Print entry bitmap of overlay TCAM
 *
 * \retval
 */
extern void
bcmptm_ser_slice_mode_info_print(int unit, int overlay_info_print);

/*!
 * \brief Check index valid for TCAMs which support narrow mode and wide mode.
 *
 * \param [in] unit          Unit number.
 * \param [in] sid           PT ID.
 * \param [in] index        index needs to be checked.
 * \param [out] valid       index is valid or is in using in SID view.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_paired_tcam_index_valid(int unit, bcmdrd_sid_t sid,
                                   int index, int *valid);

/*!
 * \brief Allocate memory space used to save slice mode information.
 *
 * \param [in] unit          Unit number.
 * \param [in] warm        Warm boot.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_mode_info_init(int unit, bool warm);


/*!
 * \brief Free memory space used to save slice mode information.
 *
 * \param [in] unit          Unit number.
 *
 * \retval
 */
extern void
bcmptm_ser_slice_mode_info_deinit(int unit, bool warm);

/*!
 * \brief Get slice_mode_mutex according to physical table ID.
 *
 * \param [in] unit          Unit number.
 * \param [in] sid           PT ID.
 * \param [out] slice_mode_mutex           Slice mode mutex.
 *
 * \retval SHR_E_NONE for success
 */
extern int
bcmptm_ser_slice_mode_mutex_get(int unit, bcmdrd_sid_t sid, sal_mutex_t *slice_mode_mutex);

#endif /* BCMPTM_SER_MISC_INTERNAL_H */
