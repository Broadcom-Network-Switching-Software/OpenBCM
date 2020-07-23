/*! \file bcmptm_rm_tcam_fp_internal.h
 *
 * Low Level Functions for FP based TCAMs
 * This file contains low level functions for FP based TCAMs
 * to do operations like TCAM entry Insert/Lookup/Delete/Move.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_TCAM_FP_INTERNAL_H
#define BCMPTM_RM_TCAM_FP_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/generated/bcmptm_rm_tcam_fp_ha.h>

/*******************************************************************************
 * Defines
 */
/* FP SLICE FLAGS */
#define BCMPTM_SLICE_INTRASLICE_CAPABLE       (1 << 0)
#define BCMPTM_SLICE_IN_USE                   (1 << 1)
#define BCMPTM_SLICE_SUPPORT_AUTO_EXPANSION   (1 << 2)
#define BCMPTM_SLICE_TYPE_SMALL               (1 << 3)
#define BCMPTM_SLICE_TYPE_LARGE               (1 << 4)
#define BCMPTM_SLICE_MODE_SINGLE              (1 << 5)
#define BCMPTM_SLICE_MODE_DOUBLE              (1 << 6)
#define BCMPTM_SLICE_MODE_TRIPLE              (1 << 7)
#define BCMPTM_SLICE_MODE_DOUBLE_CAPABLE      (1 << 8)
#define BCMPTM_SLICE_MODE_TRIPLE_CAPABLE      (1 << 9)


#define BCMPTM_RM_TCAM_MAX_TILES 3
/*******************************************************************************
 * Typedefs
 */
typedef struct bcmptm_slice_prio_map_s {
    uint16_t slice_id;
    uint16_t slice_priority;
} bcmptm_slice_prio_map_t;

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Calculate the memory required for FP based TCAM
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] total_size Memory required for FP.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int bcmptm_rm_tcam_fp_calc_mem_required(int unit,
                                        bcmltd_sid_t ltid,
                                        bcmptm_rm_tcam_lt_info_t *ltid_info,
                                        void *mem_size);

/*!
 * \brief Initialize the metadata of FP based TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int bcmptm_rm_tcam_fp_init_metadata(int unit, bool warm,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info);

/*!
 * \brief Initialize the metadata of FP based TCAM during cold boot.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] start_ptr Start pointer to TCAM
 *                       resource manager shared memory
 * \param [in] sub_component_id Sub component ID used in HA block creation.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int bcmptm_rm_tcam_fp_init_metadata_cb(int unit,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           uint8_t pipe,
                                           void *start_pointer,
                                           uint8_t sub_component_id);


/*!
 * \brief Copy the metadata of FP based TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] old_ptr Start pointer to old TCAM
 *                       resource manager shared memory
 * \param [in] new_ptr Start pointer to new TCAM
 *                       resource manager shared memory
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int bcmptm_rm_tcam_fp_copy_metadata(int unit,
                                           void *old_ptr,
                                           void *new_ptr);
/*!
 * \brief To Fetch Stage information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [out] stage structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_stage_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 int pipe,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_fp_stage_t **stage_fc);

/*!
 * \brief To Fetch Slice information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [out] number of slices in that stage.
 * \param [out] pointer to first slice structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_slice_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 uint8_t *num_slices,
                                 int pipe_id,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_slice_t **slice_fc);

/*!
 * \brief To Fetch group information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [in] group id
 * \param [out] group structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 */
extern int bcmptm_rm_tcam_fp_group_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 int pipe_id,
                                 uint32_t group_id,
                                 bcmptm_rm_tcam_fp_stage_t *stage_fc,
                                 bcmptm_rm_tcam_fp_group_t **group_fc);

/*!
 * \brief Get flags supported in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] stage_flags Stage flags.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_stage_flags_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint64_t *stage_flags);
/*!
 * \brief Get slice bundle size in this stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] slice_bundle_size Number of pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Stage is not supported.
 */
extern int bcmptm_rm_tcam_stage_attr_slice_bundle_size_get(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  uint8_t *slice_bundle_size);

/*!
 * \brief To Fetch lt config information
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe number
 * \param [in] stage id
 * \param [out] lt config structure pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 */
extern int bcmptm_rm_tcam_fp_lt_config_info_get(int unit,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 int pipe_id,
                                 bcmptm_rm_tcam_fp_stage_id_t stage_id,
                                 bcmptm_rm_tcam_lt_config_t **lt_config);

/*!
 * \brief To allocate slice for ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Ouput metadata
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage/group exists
 * \retval SHR_E_RESOURCE No slice avaliable to allocate
 */
extern int
bcmptm_rm_tcam_fp_slice_allocate(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief To convert logical entry_id to physical entry id
 *
 * \param [in]  unit        Logical device id
 * \param [in]  iomd        IN/OUT metadata to/from RM-TCAM
 * \param [in]  rm_flags    RM-TCAM flags
 * \param [in]  logical_idx Logical entry idx.
 * \param [in]  width_part  Width part number.
 * \param [out] depth_part  Depth part number.
 * \param [out] phy_idx     Physical entry idx.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int
bcmptm_rm_tcam_fp_tindex_to_sindex(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd,
                                   uint32_t rm_flags,
                                   uint32_t logical_idx,
                                   uint8_t width_part,
                                   uint8_t *depth_part,
                                   uint32_t *phy_idx);

/*!
 * \brief To get entry parts count from group mode
 *
 * \param [in]  unit Logical device id
 * \param [in]  group mode
 * \param [in]  stage flags
 * \param [out] entry part count.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int bcmptm_rm_tcam_fp_entry_tcam_width_parts_count(int unit,
                                 uint32_t group_mode,
                                 uint32_t stage_flags,
                                 int *part_count);
/*!
 * \brief To get entry depth parts count from entry attrs
 *
 * \param [in]  unit Logical device id
 * \param [in]  iomd Input and Ouput metadata
 * \param [out] entry depth part count.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */
extern int
bcmptm_rm_tcam_fp_entry_tcam_depth_parts_count(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd,
                                         int *part_count);

/*!
 * \brief To expand an existing group
 *  Allocate new slice/slices for existing ifp group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [out] updated the num_entires in
 *               field group structure
 * \param [out] new slice id allocated
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_RESOURCE No slice availble to expand
 */
extern int bcmptm_rm_tcam_fp_slice_expand(int unit,
                                          uint8_t *new_slice_id,
                                          bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief To expand an existing group
 *  free unused slice/slices from existing ifp groups
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_group_compress(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief To map the virtual slices with the physical slices
 *
 * \param [in] unit Logical device id
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_virtual_slice_map_install(int unit,
                                         bcmltd_sid_t ltid,
                                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                                         int pipe_id,
                                         bcmptm_rm_tcam_fp_stage_id_t stage_id);
/*!
 * \brief Write Slice information to hardware.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 * \param [in] group_mode
 * \param [in] slice_idx
 * \param [in] enable
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_enable_set(int unit, bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_id_t stage_id,
                               uint32_t group_mode,
                               uint8_t pri_slice_idx, int enable);
/*!
 * \brief Write logical table action priortiy information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage structure pointer.
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_group_lt_action_prio_install(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_t *stage_fc);
/*!
 * \brief Write logical table action priortiy information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] fg Field group information.
 * \param [in] pipe_id Pipe Number
 * \param [in] stage id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_group_lt_partition_prio_install(int unit,
                               bcmltd_sid_t ltid,
                               bcmptm_rm_tcam_lt_info_t *ltid_info,
                               bcmptm_rm_tcam_fp_group_t *fg,
                               int pipe_id,
                               bcmptm_rm_tcam_fp_stage_id_t stage_id);
/*!
 * \brief Write Slice selcodes information to hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_install(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id);
/*!
 * \brief Copy Slice selcodes information from primary to new slice in hardware
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_copy(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t pri_slice_id,
                          uint8_t slice_id);
/*!
 * \brief Reset Slice selcodes information in hardware.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry attrs structure
 * \param [in] slice id
 *
 * \retval SHR_E_NONE     Success
 */
extern int
bcmptm_rm_tcam_fp_slice_selcodes_reset(int unit, bcmltd_sid_t ltid,
                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                          uint8_t slice_id);
/*!
 * \brief Compare two group priorities linked to logical tables.
 *
 * \param [in] a Pointer to lt config
 * \param [in]  b Pointer to lt config
 *
 * \retval -1, 0 or 1
 */
extern int
bcmptm_rm_tcam_fp_lt_group_priority_compare(const void *a, const void *b);
/*!
 * \brief To delete the hw presel entries of the given slice
 *  if it is primary slice, clear the s/w hash too
 *
 * \param [in] unit Logical device id
 * \param [in] iomd IN/OUT metadata to/from RM-TCAM
 * \param [in] slice_id Slice Number
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_presel_entries_clear(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint8_t slice_id);
/*!
 * \brief To release the group's resources when no entries are
 *  present in the group
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output metadata
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */
int
bcmptm_rm_tcam_fp_free_group_resources(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief To reset lt_config structure when a group is deleted
 *    Also clear group's slice's enable flags
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] stage_fc Field stage structure.
 * \param [in] pri_slice_idx Primary slice index
 * \param [in] fg  Field group structure.
 * \param [in] slice_fc Field slice structure.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */

int
bcmptm_rm_tcam_fp_group_priority_reset(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_fp_stage_t *stage_fc,
                              bcmptm_rm_tcam_fp_group_t *fg,
                              bcmptm_rm_tcam_slice_t *slice_fc);
/*
 * \brief To deallocate group unused slices.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output metadata
 * \param [in] fg   Field group structure.
 * \param [in] clear_presel Flag to free presel entires
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_INTERNAL Failure
 */
int
bcmptm_rm_tcam_fp_group_free_unused_slices(int unit,
                                           bcmptm_rm_tcam_entry_iomd_t *iomd,
                                           bcmptm_rm_tcam_fp_group_t *fg,
                                           uint8_t *count_free_slices,
                                           uint8_t clear_presel);

extern int
bcmptm_rm_tcam_fp_slice_profiles_reserve(int unit,
                       bcmltd_sid_t req_ltid,
                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                       uint8_t slice_id,
                       uint8_t num_slices,
                       bcmptm_rm_tcam_fp_stage_t *stage_fc,
                       bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                       bool per_group_profiles);
/*
 * \brief Validate and return tentative entry count based on group mode
 *
 * \param [in] unit BCM device unit number
 * \param [in] iomd Input and Output metadata
 * \param [out] entry_count Tentative entry count.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_RESOURCE No slice availble to allocate
 */
int
bcmptm_rm_tcam_fp_group_tentative_entry_calculate(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd,
                                  uint32_t *entry_count);
/*
 * \brief update slice info in all auto expanded slices
 *
 * \param [in] unit BCM device unit number
 * \param [in] ltid Logical table ID.
 * \param [in] ltid_info Logical table ID device specific information.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Param error
 */

int
bcmptm_rm_tcam_fp_em_slice_info_update(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info);
/*!
 * \brief Free last slice of the group if entries
 *  of the group can fit into n-1 slices where n is number
    of slices used by the group
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_NOT_FOUND No group/stage/slice exists
 */
extern int
bcmptm_rm_tcam_fp_group_free_last_slice(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd);
#endif /* BCMPTM_RM_TCAM_FP_INTERNAL_H */
