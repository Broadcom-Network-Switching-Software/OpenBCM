/*! \file rm_tcam_prio_atomicity.h
 *
 * Utils, defines internal to PRIO TCAM Atomicity changes
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_PRIO_ATOMICITY_H
#define RM_TCAM_PRIO_ATOMICITY_H

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Update the entry words to a particular TCAM index.
 * \n Updates both HW and SW state
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
    - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
    - tbl_inst member of pt_dyn_info may indicate pipe num.
    - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
      read requests.
    - Need pipe num to support unique mode for FP tables (eg: IFP, EFP, VFP)
    - Value of -1 means Global mode.
    - pipe_num for all underlying SIDs (TCAM_ONLY, TDATA_ONLY views)
      must be same for a given LT.
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] found_index TCAM index of the entry_id which needs the update.
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_entry_update(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd);


/*!
 * \brief Update the entry words to a particular TCAM index.
 * \n Updates only the data portion of the TCAM index.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd input and output metadata
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_entry_update_data_only(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief To verify if the requested update is a key change or a data change
 *
 * \param [in] unit Logical device id
 * \param [out] update_type Data only or Key only or both change
 * \param [in] iomd input and output metadata
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_update_type_get(int unit,
                        uint8_t *update_type,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

#endif /* RM_TCAM_PRIO_ATOMICITY_H */
