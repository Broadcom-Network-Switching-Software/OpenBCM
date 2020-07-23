/*! \file bcmecmp_group_util.h
 *
 * This file contains ECMP group shared functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_GROUP_UTIL_H
#define BCMECMP_GROUP_UTIL_H

#include <bcmecmp/bcmecmp_db_internal.h>

/*!
 * \brief ECMP_OVERLAY LT entry fields fill routine.
 *
 * Parse ECMP_OVERLAY logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in_fld Logical table database input field array.
 * \param [out] ltentry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
extern int
bcmecmp_overlay_grp_lt_fields_fill(int unit,
                                   const void *in_fld,
                                   void *ltentry);

/*!
 * \brief ECMP_UNDERLAY LT entry fields fill routine.
 *
 * Parse ECMP_UNDERLAY logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in_fld Logical table database input field array.
 * \param [out] ltentry Logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
extern int
bcmecmp_underlay_grp_lt_fields_fill(int unit,
                                    const void *in_fld,
                                    void *ltentry);

/*!
 * \brief Check whether the given group is in use and returns the status.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 * \param [in] memb_dest true if this corresponds to member0/member1 LTs.
 * \param [in] memb_level level (0/1) if this is member0/member1 LT.
 * \param [in] gtype Group type.
 * \param [out] in_use Specifies whether the group is in use.
 * \param [out] comp_id Component ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet the group information.
 */
extern int
bcmecmp_grp_in_use(int unit,
                   uint32_t grp_id,
                   bool memb_dest,
                   int memb_level,
                   bcmecmp_grp_type_t gtype,
                   bool *in_use,
                   uint32_t *comp_id);
/*!
 * \brief Check whether the given group is in use in bake up HA.
 *
 * \param [in] unit Unit number.
 * \param [in] grp_id Group ID.
 * \param [in] memb_dest Specifies whether this group is member destination.
 * \param [in] level Specifies whether this call is for level 0 or 1.
 * \param [in] gtype Group type.
 * \param [out] in_use Specifies whether this group is in use.
 * \param [out] component Component ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get the group information.
 */
extern int
bcmecmp_grp_in_use_bk(int unit,
                      uint32_t grp_id,
                      bool memb_dest,
                      int level,
                      bcmecmp_grp_type_t gtype,
                      bool *in_use,
                      uint32_t *component);
/*!
 * \brief This function allocates HA memory for storing HW entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of hw entry array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_hw_entry_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size,
                          uint8_t submod_id,
                          uint32_t ref_sign,
                          bcmecmp_hw_entry_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz);

/*!
 * \brief This function allocates HA memory for storing group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_grp_attr_ha_alloc(int unit,
                          bool warm,
                          uint32_t array_size,
                          uint8_t submod_id,
                          uint32_t ref_sign,
                          bcmecmp_grp_ha_blk_t **alloc_ptr,
                          uint32_t *out_ha_alloc_sz);

/*!
 * \brief This function allocates HA memory for storing group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_flex_member_dest_grp_attr_ha_alloc(int unit,
                                           bool warm,
                                           uint32_t array_size,
                                           uint8_t submod_id,
                                           uint32_t ref_sign,
                                           bcmecmp_member_dest_grp_ha_blk_t **alloc_ptr,
                                           uint32_t *out_ha_alloc_sz);
/*!
 * \brief Insert a group into the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be inserted into the group list.
 * \param [in] min2max Sort groups from min to max into the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_MEMORY Allocate resource failed.
 */
extern int
bcmecmp_grp_list_insert(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id,
                        bool min2max);

/*!
 * \brief Delete a group from the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be deleted from the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 */
extern int
bcmecmp_grp_list_delete(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id);

/*!
 * \brief Get the head of the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [out] grp_id The group at the head of the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_FAIL The group is empty.
 */
extern int
bcmecmp_grp_list_get_head(int unit,
                          bcmecmp_grp_list_t *grp_list,
                          bcmecmp_id_t *grp_id);

/*!
 * \brief Destroy the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 */
extern int
bcmecmp_grp_list_destroy(int unit, bcmecmp_grp_list_t *grp_list);

/*!
 * \brief Update a group in the group list.
 *
 * \param [in] unit BCM unit number.
 * \param [in] grp_list The group list.
 * \param [in] grp_id The group which will be inserted into the group list.
 * \param [in] min2max Sort groups from min to max into the group list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval BCMECMP_E_PARAM The pointer of group list is NULL.
 * \retval BCMECMP_E_MEMORY Allocate resource failed.
 */
extern int
bcmecmp_grp_list_update(int unit,
                        bcmecmp_grp_list_t *grp_list,
                        bcmecmp_id_t grp_id,
                        bool min2max);

/*!
 * \brief This function allocates HA memory for storing HW entries
 * for flex groups.
 *
 * \param [in] unit BCM Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of hw entry array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_flex_hw_entry_ha_alloc(int unit,
                               bool warm,
                               uint32_t array_size,
                               uint8_t submod_id,
                               uint32_t ref_sign,
                               bcmecmp_flex_hw_entry_ha_blk_t **alloc_ptr,
                               uint32_t* out_ha_alloc_sz);

/*!
 * \brief This function allocates HA memory for storing flex group attr.
 *
 * \param [in] unit BCM Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_flex_grp_attr_ha_alloc(int unit,
                               bool warm,
                               uint32_t array_size,
                               uint8_t submod_id,
                               uint32_t ref_sign,
                               bcmecmp_flex_grp_ha_blk_t **alloc_ptr,
                               uint32_t* out_ha_alloc_sz);

/*!
 * \brief This function allocates HA memory for storing flex group RH attr.
 *
 * \param [in] unit BCM Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [inout] alloc_ptr ptr of ptr of group attr array info.
 * \param [inout] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmecmp_flex_grp_rh_attr_ha_alloc(int unit,
                                  bool warm,
                                  uint32_t array_size,
                                  uint8_t submod_id,
                                  uint32_t ref_sign,
                                  bcmecmp_flex_grp_rh_ha_blk_t **alloc_ptr,
                                  uint32_t* out_ha_alloc_sz);


/*!
 * \brief Get free member start index from IBTM bucket and first.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype Group type.
 * \param [in] weight_mode Weight mode.
 * \param [in] bucket ITBM bucket.
 * \param [in] first ITBM first.
 *
 * \return free index.
 */
extern uint32_t
bcmecmp_itbm_mstart_index_get(int unit,
                              bcmecmp_grp_type_t gtype,
                              uint32_t weight_mode,
                              uint32_t bucket,
                              uint32_t first);

/*!
 * \brief Get free member start index from IBTM bucket and first.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype Group type.
 * \param [in] weight_mode Weight mode.
 * \param [in] mstart_idx ECMP members start index.
 * \param [in] bucket Pointer to the ITBM bucket.
 * \param [in] first Pointer to the ITBM first.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_itbm_bucket_first_get(int unit,
                              bcmecmp_grp_type_t gtype,
                              uint32_t weight_mode,
                              uint32_t mstart_idx,
                              uint32_t *bucket,
                              uint32_t *first);

/*!
 * \brief Write the entry to the PT layer.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info operation info.
 * \param [in] buf Entry buffer to write.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_pt_write(int unit,
                 bcmecmp_pt_op_info_t *op_info,
                 uint32_t *buf);

/*!
 * \brief Read the entry from the PT layer.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info operation info.
 * \param [in] buf Entry buffer to read.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_pt_read(int unit,
                bcmecmp_pt_op_info_t *op_info,
                uint32_t *buf);

/*!
 * \brief Read the entry from the PT layer via ireq API.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id LT SID.
 * \param [in] pt_id PT SID.
 * \param [in] index hardware entry index.
 * \param [in] entry_data Entry buffer to read.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_pt_ireq_read(int unit,
                     bcmltd_sid_t lt_id,
                     bcmdrd_sid_t pt_id,
                     int index,
                     void *entry_data);

/*!
 * \brief RH-ECMP groups with itbm flag pre-config function for warm start sequence.
 *
 * For all RH-ECMP groups that have non-zero NUM_PATHS value set during insert
 * operation in cold boot mode, their load balanced members state has to be
 * reconstructed for use during update operation post warm boot. This function
 * identifies such RH-ECMP groups during warm start pre_config sequence.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmecmp_rh_groups_preconfig_with_itbm(int unit);
#endif /* BCMECMP_GROUP_UTIL_H */
