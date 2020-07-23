/*! \file rm_tcam.h
 *
 * Utils, defines internal to RM-TCAM
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_H
#define RM_TCAM_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/bcmptm_rm_tcam_um_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/generated/bcmptm_rm_tcam_trans_info_ha.h>

/*******************************************************************************
 * Defines
 */
/*! Divison factor to caliculate the depth of hash table for priority
 *  based TCAMs.
 */
#define BCMPTM_RM_TCAM_PRIO_HASH_DIV_FACTOR (4)

/*! Invalid TCAM index. */
#define BCMPTM_RM_TCAM_INDEX_INVALID (-1)

/*! Invalid EID(Entry ID). */
#define BCMPTM_RM_TCAM_EID_INVALID (-1)

#define BCMPTM_RM_TCAM_ENTRY_TYPE_INVALID (-1)

/*! Invalid entry priority. */
#define BCMPTM_RM_TCAM_PRIO_INVALID (-1)

/*! Invalid memory offset. */
#define BCMPTM_RM_TCAM_OFFSET_INVALID (-1)

/*! Entry operation in HW only. */
#define BCMPTM_RM_TCAM_ENTRY_OP_HW_ONLY (1 << 0)
/*! Entry operation in SW only. */
#define BCMPTM_RM_TCAM_ENTRY_OP_SW_ONLY (1 << 1)

/*! Only key part of the entry is modified. */
#define BCMPTM_RM_TCAM_ENTRY_KEY_ONLY_UPDATE (1 << 0)

/*! Only data part of the entry is modified. */
#define BCMPTM_RM_TCAM_ENTRY_DATA_ONLY_UPDATE (1 << 1)

/*! Max number of pipes across all devices. */
#define BCMPTM_RM_TCAM_MAX_PIPES (8)

/*! Signature for RM TCAM HA global information. */
#define BCMPTM_RM_TCAM_HA_SIGN_GLOBAL_INFO 0xABCDEF00

/* Transaction states for RM tcam */
typedef enum  bcmptm_rm_tcam_trans_state_e {

    /* Idle state */
    BCMPTM_RM_TCAM_STATE_IDLE = 0,

    /* Uncommited changes in Active DB */
    BCMPTM_RM_TCAM_STATE_UC_A = 1,

    /* Commiting changes(Copying Active to Backup) */
    BCMPTM_RM_TCAM_STATE_COPY_A_TO_B = 2,

    /* Aborting changes(Copying Backup to Active) */
    BCMPTM_RM_TCAM_STATE_COPY_B_TO_A = 3

} bcmptm_rm_tcam_trans_state_t;

/*******************************************************************************
 * Typedefs
 */
/*! TCAM Function pointers. */
typedef struct bcmptm_rm_tcam_funct_s {

     /*! TCAM entry insert function pointer */
     int (*entry_insert)(int unit,
                         bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM entry lookup function pointer */
     int (*entry_lookup)(int unit,
                         uint64_t flags,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         bcmbd_pt_dyn_info_t *pt_dyn_info,
                         bcmptm_rm_tcam_req_t *req_info,
                         bcmptm_rm_tcam_rsp_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags,
                         uint32_t *index,
                         bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM entry delete function pointer */
     int (*entry_delete)(int unit,
                         uint64_t flags,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         bcmbd_pt_dyn_info_t *pt_dyn_info,
                         bcmptm_rm_tcam_req_t *req_info,
                         bcmptm_rm_tcam_rsp_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags,
                         bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM entry move function pointer */
     int (*entry_move)(int unit,
                       bcmltd_sid_t ltid,
                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                       uint8_t partition,
                       uint32_t prefix,
                       uint32_t from_index,
                       uint32_t to_index,
                       void *attrs,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM entry traverse first function pointer */
     int (*entry_get_first)(int unit,
                            uint64_t flags,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            bcmbd_pt_dyn_info_t *pt_dyn_info,
                            bcmptm_rm_tcam_req_t *req_info,
                            bcmptm_rm_tcam_rsp_t *rsp_info,
                            bcmltd_sid_t *rsp_ltid,
                            uint32_t *rsp_flags,
                            bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM entry get next function pointer */
     int (*entry_get_next)(int unit,
                           uint64_t flags,
                           bcmltd_sid_t ltid,
                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                           bcmbd_pt_dyn_info_t *pt_dyn_info,
                           bcmptm_rm_tcam_req_t *req_info,
                           bcmptm_rm_tcam_rsp_t *rsp_info,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags,
                           bcmptm_rm_tcam_entry_iomd_t *iomd);

     /*! TCAM table info get function. */
     int (*get_table_info)(int unit,
                           uint64_t flags,
                           bcmltd_sid_t ltid,
                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                           bcmbd_pt_dyn_info_t *pt_dyn_info,
                           bcmptm_rm_tcam_req_t *req_info,
                           bcmptm_rm_tcam_rsp_t *rsp_info,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags,
                           bcmptm_rm_tcam_entry_iomd_t *iomd);

} bcmptm_rm_tcam_funct_t;

/*! Structure to hold same key related info */
typedef struct bcmptm_rm_tcam_same_key_data_s {

    /* Same key index */
    int index;

    /* Associated Priority */
    int priority;

    /*
     * Previous index of an entry when the entry
     * is updated and moved to a new index.
     */
    int prev_index;
} bcmptm_rm_tcam_same_key_data_t;

/* To hold previously looked up index */
extern bcmptm_rm_tcam_same_key_data_t same_key_info[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * Structure to hold LTIDs and corresponding ha block pointers.
 * This structure will be stored in non HA memory and maintained
 * for every TCAM LT supported on the device.
 */
typedef struct bcmptm_rm_tcam_md_s {

    /* Logical table ID */
    bcmltd_sid_t ltid;

    /* Number of pipes */
    uint8_t num_pipes;

    /* Pointer to active HA block of LTID in all pipes. */
    void *active_ptrs[BCMPTM_RM_TCAM_MAX_PIPES];

    /* Pointer to backup HA block of LTID in all pipes. */
    void *backup_ptrs[BCMPTM_RM_TCAM_MAX_PIPES];

    /* Sub component id used for active HA allocation of LT */
    uint8_t active_sub_component_id[BCMPTM_RM_TCAM_MAX_PIPES];

    /* Sub component id used for backup HA allocation of LT */
    uint8_t backup_sub_component_id[BCMPTM_RM_TCAM_MAX_PIPES];

    /* Size of current LTID data */
    uint32_t ptrm_size;

    /* Pointer to next LTID info */
    struct bcmptm_rm_tcam_md_s *next;

} bcmptm_rm_tcam_md_t;

typedef struct bcmptm_rm_tcam_info_global_s {
    /*!
     * Transaction state of the whole RM-TCAM. This is
     * set to IDLE if no operations are done on tcam
     * based LTs in the current transaction. This is set
     * to UC_A if entries are inserted/deleted/updated in
     * any one of the tcam type LTs.
     */
    bcmptm_rm_tcam_trans_info_t trans_info;

} bcmptm_rm_tcam_info_global_t;

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Fetch the pointer that holds function pointers to basic TCAM
 *  \n operations of a TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] ltid_info LRD information
 * \param [out] funct Pointer to the required function
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Function Not found
 */
extern int
bcmptm_rm_tcam_funct_get(int unit, bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info, void **funct);


/*!
 * \brief To compare two entries of Tcam, src and dst info.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] flags Flags for special handling
 * \param [in] src_ent_key_words Key Part of Src Data
 * \param [in] src_ent_data_words Data Part of Src Data
 * \param [in] dst_ent_key_words Key Part of Dst Data
 * \param [in] dst_ent_data_words Data Part of Dst Data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FAIL Compare failed
 */

extern int
bcmptm_rm_tcam_entry_compare(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint32_t flags,
                             uint32_t **src_ent_key_words,
                             uint32_t **src_ent_data_words,
                             uint32_t **dst_ent_key_words,
                             uint32_t **dst_ent_data_words);
/*!
 * \brief Fetch the Memory address offset information of a given LTID.
 *\n offset is offset value form the start address of the single memory
 *\n chunk created for the whole RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] tcam_info memory address to tcam information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_info_get(int unit,
                        bcmltd_sid_t ltid,
                        int pipe_id,
                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                        void **tcam_info);
/*!
 * \brief Fetch the Memory address offset information of a given LTID.
 *\n offset is offset value form the start address of the single memory
 *\n chunk created for the backup of RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] tcam_info memory address to tcam information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_backup_info_get(int unit, bcmltd_sid_t ltid,
                        bcmptm_rm_tcam_lt_info_t *ltid_info, void **tcam_info);


/*!
 * \brief Move the TCAM entry from one index to the other. This function
 * \n will internall call TCAM specific move function.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved
 * \param [in] to_idx TCAM index to which entry has to be moved
 * \param [in] attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_entry_move(int unit,
                          bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          uint8_t partition,
                          uint32_t from_idx,
                          uint32_t to_idx,
                          void *attrs,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          bcmptm_rm_tcam_entry_iomd_t *iomd);


/*!
 * \brief Read entry words from all width expanded physical tables
 *  mapped to ltid.
 *
 * \param [in] unit        Logical device id
 * \param [in] iomd        IN/OUT metadata to/from RM-TCAM
 * \param [in] rm_flags    Flags of type BCMPTM_RM_TCAM_XXX to select the
 *                         sid view (TCAM Aggregated View/TCAM only view/DATA
 *                         only view)
 * \param [in] index       TCAM index to be written
 * \param [in] entry_words Array holding entry words of all width
                           expanded sid's
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_hw_read(int unit,
                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                       uint32_t rm_flags,
                       uint32_t index,
                       uint32_t **entry_words);

/*!
 * \brief Write the entry words to actual phyiscal tables mapped
 *  to givel ltid.
 *
 * \param [in] unit        Logical device id
 * \param [in] iomd        IN/OUT metadata to/from RM-TCAM
 * \param [in] rm_flags    Flags of type BCMPTM_RM_TCAM_XXX to select the
 *                         sid view (TCAM Aggregated View/TCAM only view/DATA
 *                         only view)
 * \param [in] index       TCAM index to be written
 * \param [in] entry_words Array holding entry words of all width
                           expanded sid's
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_hw_write(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                        uint32_t rm_flags,
                        uint32_t index,
                        uint32_t **entry_words);

/*!
 * \brief Calculate the total memory required for SW state of
   \n all TCAM LTIDs together.
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] user_data User Data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL initialization issues
 */
extern int
bcmptm_rm_tcam_prio_eid_calc_mem_required(int unit, bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *user_data);

/*!
 * \brief Initialize the metadata of priority based TCAM in
 *        bcmptm_rm_tcam_prio_eid_info_t format.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] start_ptr Start pointer to TCAM
 *                       resource manager shared memory
 * \param [in] offset Offset bytes from start_ptr to start of ltid data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_init_metadata(int unit, bool warm,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  void *user_data);

/*!
 *  \brief TCAM LTID can be depth expanded and every TCAM index maps
 *   to a physical slice and index in that slice. This routine is to
 *   fetch the slice roe and index in that slice row for a given TCAM
 *   index.
 *
 *  \param [in]   unit     Logical device id
 *  \param [in]   iomd     IN/OUT metadata to/from RM-TCAM
 *  \param [in]   rm_flags Flags of type BCMPTM_RM_TCAM_XXX to select
 *                         the sid view(TCAM Aggregated View/TCAM only
 *                         view/DATA only view)
 *  \param [in]  tindex    TCAM index
 *  \param [out] sindex    Slice index corresponding to tindex
 *  \param [out] slice_row Slice row corresponds to tindex
 *  \param [out] slice_col Slice column corresponds to tindex
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Inavlid parameters
 */
extern int
bcmptm_rm_tcam_tindex_to_sindex(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint32_t rm_flags,
                                uint32_t tindex,
                                uint32_t *sindex,
                                uint8_t *slice_row,
                                uint8_t *slice_col);

/*!
 *  \brief Internal function to extract bits
 *  \n from given start positionm to given length.
 *
 *  \param [in] unit Logical device id
 *  \param [in] in_data Input Data of words
 *  \param [in] start_pos Starting Position
 *  \param [in] len Length to be extracted.
 *  \param [out] out_data Output Data of extracted bits.
 *
 *  \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_data_get(int unit,
                        uint32_t *in_data,
                        uint16_t start_pos,
                        uint16_t len,
                        uint32_t *out_data);

/*!
 * \brief Fetch the hw entry information of an LT based on LT entries mode.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] attrs Attributes of entry.
 * \param [out] hw_entry_info LT entry chip specific information.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_lt_hw_entry_info_get(int unit,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  void *attrs,
                  const bcmptm_rm_tcam_hw_entry_info_t **hw_entry_info);

/*!
 * \brief To handle the transaction at RM TCAM module.
 *
 * \param [in] unit Logical device id
 * \param [in] transaction global info.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_um_trans_complete(int unit,
                          bcmptm_rm_tcam_trans_info_t *trans_info_global);
/*!
 * \brief  Fetch the number of physical indexs needed to write the entry
 * \n for the given Logical Table id.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] num physical indexes needed
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_num_index_required_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint32_t *num_index_req);

/*!
 * \brief  Check whether two LTIDs can share the tacm info or not.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid_info LRD information for the first ltid.
 * \param [in] ltid_info LRD information for the second ltid.
 * \param [out] shared flag to indicate that the two LTIDs share
 *              tcam info or not.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_ltid_tcam_info_shared_check(int unit,
                              bcmptm_rm_tcam_lt_info_t *ltid_info1,
                              bcmptm_rm_tcam_lt_info_t *ltid_info2,
                              bool *shared,
                              bool *sid_rows_diff);

/*!
 * \brief  Check whether any shared TCAM has a depth difference.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical table id.
 * \param [in] ltid_info LRD information for the first ltid.
 * \param [out] depth_differs Flag to indicate there is depth difference
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_shared_lt_depth_check(int unit,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bool *depth_differs);

/*!
 *  \brief Get the active pipe count. Even though some LTs are per pipe
 *   resources, application might be operating it in global mode.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [out] num_pipes Active pipe count.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_num_pipes_get(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             uint8_t *num_pipes);
/*!
 *  \brief Get the transaction state of the RM-TCAM module. This is
 *  different from the transaction state of individual LT transaction
 *  state.
 *
 *  \param [in] unit Logical device id
 *  \param [out] trans_info Transaction info of the whole RM-TCAM module.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_trans_info_global_get(int unit,
               bcmptm_rm_tcam_trans_info_t **trans_info);
/*!
 *  \brief If two LTs are sharing the same physical resources, then
 *  metadata generated for first LT can be copied to next LT to
 *  avoid regenerating the data again.
 *
 *  \param [in] unit Logical device ID
 *  \param [in] ltid Logical table ID
 *  \param [in] shr_ltid Logical table ID sharing the physical
 *                       resources with ltid.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_md_copy(int unit,
                       bcmltd_sid_t ltid,
                       bcmltd_sid_t shr_ltid);
/*!
 *  \brief Get the metadata generated for an LTID. This metadata
 *  is generated during bootup time and will not change run time.
 *
 *  \param [in] unit Logical device ID
 *  \param [in] ltid Logical table ID
 *  \param [out] tcam_md tcam LT metadat.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_md_get(int unit,
                      bcmltd_sid_t ltid,
                      bcmptm_rm_tcam_md_t **tcam_md);
/*!
 *  \brief Delete the metadata corresponding to an LTID.
 *
 *  \param [in] unit Logical device ID
 *  \param [in] ltid Logical table ID
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_md_delete(int unit,
                         bcmltd_sid_t ltid);

/*!
 *  \brief Get the first metadata node. This metadata is generated
 *  for all tcam LTs and are in linked list.
 *
 *  \param [in] unit Logical device ID
 *  \param [out] tcam_md First metadata node.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_md_get_first(int unit,
                            bcmptm_rm_tcam_md_t **tcam_md);
/*!
 *  \brief Get the start and end index, in logical space, of a
 *  physical table if LT depth expands to mtultiple physical tables.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [in] sid_row physical table index in a depth expaned LT.
 *  \param [out] start_idx Start index of physical table.
 *  \param [out] end_idx End index of physical table.
 *
 *  \retval SHR_E_NONE Success
 *  \retval SHR_E_PARAM Invalid parameters(No NULL pointers)
 *  \retval SHR_E_INTERNAL If sid_row is greater than or equals to
 *          depth of the LT. Valid values of sid_row are 0 to (LT depth -1).
 *  \retval SHR_E_CONFIG If any SID is INVALIDm
 */
extern int
bcmptm_rm_tcam_logical_space_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t sid_row,
                                 uint16_t *start_idx,
                                 uint16_t *end_idx);
/*!
 *  \brief Get whether the LT is in butterfly mode.
 *  Temporarily in place till a flag is provided in ptrm yml files
 *  to indicate the same.
 *
 *  \param [in] unit Logical device id
 *  \param [in] ltid Logical Table enum that is accessing the table.
 *  \param [in] ltid_info LRD information for the ltid.
 *  \param [out] whether the ltid is in butterfly mode or not.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_check_butterfly_mode(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *is_half_mode);
/*!
 *  \brief Get whether the uncommited state should be commited or aborted.
 *
 *  \param [in] unit Logical device id
 *  \param [out] undo_uc_state Undo the uncommitted sw state or not.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_undo_uc_state_get(int unit, bool *undo_uc_state);

/*!
 *  \brief Enable or Disable the RM-TCAM atomic transactions state dynamically.
 *
 *  \param [in] unit Logical device id
 *  \param [in] trans_state TRUE - Set RM-TCAM atomic transaction state.
 *                          FALSE - Unset RM-TCAM atomic transaction state.
 *
 *  \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_dynamic_trans_set(int unit, bool dynamic_trans_state);

#endif /* RM_TCAM_H */
