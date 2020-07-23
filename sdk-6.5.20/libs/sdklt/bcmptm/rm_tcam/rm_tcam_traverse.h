/*! \file rm_tcam_traverse.h
 *
 * Utils, defines RM-TCAM traversal
 * This file contains utils, defines of TCAM traversal
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_TRAVERSE_H
#define RM_TCAM_TRAVERSE_H

/*******************************************************************************
 * Typedefs
 */
typedef struct bcmptm_rm_tcam_traverse_pipe_info_s {

    /* Hash size to be considered while building hash DB */
    int        hash_size;

    /*  Total number of TCAM entries */
    uint32_t   num_entries;

    /*  Number of Free TCAM entries  */
    uint32_t   free_entries;

    /* Holds hash DB info */
    uint32_t   *hash_info;

    /* Holds entry DB info, can be either bcmptm_rm_tcam_prio_eid_entry_info_t or
       bcmptm_rm_tcam_prio_only_entry_info_t */
    void       *entry_info;

    /* Holds array of uint8_t to mark if entry is valid or not ,
       will be marked as deleted if entry was delete during traversal */
    uint8_t    *valid_info;

    /* Holds array KEY's in the HW */
    uint32_t   *key_db[BCMPTM_RM_TCAM_MAX_WIDTH_PARTS];
} bcmptm_rm_tcam_traverse_pipe_info_t;


typedef struct bcmptm_rm_tcam_traversal_info_s {
    /* LTID */
    bcmltd_sid_t                             ltid;

    /* Max Pipe count of the LT */
    int                                      pipe_count;

    /* Bitmap represents valid pipe */
    uint32_t                                 valid_bitmap;

    /*
     * Size of each row in HW in multiples of uint32_t
     * For aggr view will include data bits also.
     */
    uint16_t                                 key_size;

    /* Array of pipe info based on pipe count */
    bcmptm_rm_tcam_traverse_pipe_info_t      *pipe_info;

    /* Address to the next traversal node */
    struct bcmptm_rm_tcam_traversal_info_s   *next;
} bcmptm_rm_tcam_traversal_info_t;

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Return first found key and data through response info.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_entry_get_first_without_snapshot(int unit,
                                            uint64_t req_flags,
                                            bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                                            bcmptm_rm_tcam_req_t *req_info,
                                            bcmptm_rm_tcam_rsp_t *rsp_info,
                                            bcmltd_sid_t *rsp_ltid,
                                            uint32_t *rsp_flags,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Get next valid entry to the entry from req info
 * \n for the given LTID.
 * \n Return next valid key and data through response info.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_entry_get_next_without_snapshot(int unit,
                                            uint64_t req_flags,
                                            bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                                            bcmptm_rm_tcam_req_t *req_info,
                                            bcmptm_rm_tcam_rsp_t *rsp_info,
                                            bcmltd_sid_t *rsp_ltid,
                                            uint32_t *rsp_flags,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Build snapshot for the given LTID.
 * \n Return first found key and data through response info.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_entry_get_first_with_snapshot(int unit,
                                            uint64_t req_flags,
                                            bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                                            bcmptm_rm_tcam_req_t *req_info,
                                            bcmptm_rm_tcam_rsp_t *rsp_info,
                                            bcmltd_sid_t *rsp_ltid,
                                            uint32_t *rsp_flags,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Get next valid entry to the entry from req info in snapshot
 * \n for the given LTID.
 * \n Return next valid key and data through response info.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_entry_get_next_with_snapshot(int unit,
                                            uint64_t req_flags,
                                            bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                                            bcmptm_rm_tcam_req_t *req_info,
                                            bcmptm_rm_tcam_rsp_t *rsp_info,
                                            bcmltd_sid_t *rsp_ltid,
                                            uint32_t *rsp_flags,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Build snapshot for the given LTID.
 * \n Return first found key and data through response in fo.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavio r
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing  the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_info_entry_get_first(int unit,
                                             uint64_t req_flags,
                                             bcmltd_sid_t ltid,
                                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                                             bcmbd_pt_dyn_info_t *pt_dyn_info,
                                             bcmptm_rm_tcam_req_t *req_info,
                                             bcmptm_rm_tcam_rsp_t *rsp_info,
                                             bcmltd_sid_t *rsp_ltid,
                                             uint32_t *rsp_flags,
                                             bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Get next valid entry to the entry from req info in snapshot
 * \n for the given LTID.
 * \n Return next valid key and data through response info.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
 *             (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_EMPTY When there are not valid entries in LTID.
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_traverse_info_entry_get_next(int unit,
                                            uint64_t req_flags,
                                            bcmltd_sid_t ltid,
                                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                                            bcmptm_rm_tcam_req_t *req_info,
                                            bcmptm_rm_tcam_rsp_t *rsp_info,
                                            bcmltd_sid_t *rsp_ltid,
                                            uint32_t *rsp_flags,
                                            bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Search the deleted entry in snapshot and mark it as invalid
 * \n for the given LTID.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_traverse_info_entry_delete(int unit,
                                          bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                                          bcmptm_rm_tcam_req_t *req_info);
/*!
 * \brief Free all the traverse snapshot for the given unit
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_traverse_info_delete_all(int unit);

#endif /* RM_TCAM_TRAVERSE_H */
