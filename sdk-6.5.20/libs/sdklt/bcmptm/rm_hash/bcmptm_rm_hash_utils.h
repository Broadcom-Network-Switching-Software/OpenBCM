/*! \file bcmptm_rm_hash_utils.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_UTILS_H
#define BCMPTM_RM_HASH_UTILS_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get SW entry bucket mode definition based on HW value.
 *
 * \param [in] unit Unit number.
 * \param [in] hw_bkt_mode_val HW bucket mode value.
 * \param [out] e_bm Pointer to a buffer holding SW bucket mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bkt_mode_mapping(int unit,
                                uint8_t hw_bkt_mode_val,
                                rm_hash_bm_t *e_bm);

/*!
 * \brief Fill the key type value into entry buffer
 *
 * \param [in] unit Unit number.
 * \param [in] key_type Key type.
 * \param [in] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [out] e_words Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_key_type_fill(int unit,
                             uint16_t key_type,
                             const bcmptm_rm_hash_view_info_t *view_info,
                             uint32_t *e_words);

/*!
 * \brief Fill the data type value into entry buffer
 *
 * \param [in] unit Unit number.
 * \param [in] pdtype_type Physical data type.
 * \param [in] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [out] e_words Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_data_type_fill(int unit,
                              uint16_t pdtype_type,
                              const bcmptm_rm_hash_view_info_t *view_info,
                              uint32_t *e_words);

/*!
 * \brief Generate signature for an entry
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry buffer.
 * \param [in] key_fields Pointer to bcmlrd_hw_field_list_t structure.
 * \param [out] ent_sig Pointer to buffer for recording entry signature.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_ent_sig_generate(int unit,
                                uint32_t *entry,
                                const bcmlrd_hw_field_list_t *key_fields,
                                uint32_t *ent_sig);

/*!
 * \brief Get software logical bucket based on hardware logical bucket
 *
 * For the provided entry bucket mode and hardware logical bucket list,
 * this function will calculate the corresponding software logical
 * bucket list and valid base bucket bitmap within each software logical
 * bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [out] inst_srch_info Pointer to rm_hash_inst_srch_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_hlb_to_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              rm_hash_inst_srch_info_t *inst_srch_info);

/*!
 * \brief Get the logical bank number corresponding to a software logical bucket
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb SLB of the entry.
 * \param [out] rbank Pointer to a buffer to contain the calculated bank.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_slb_to_rbank_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint32_t slb,
                                uint8_t *rbank);

/*!
 * \brief Get entry index from a physical table perspective
 *
 * This function get physical table index for an entry, based on the following
 * location information: software logical bucket, bank number, base bucket
 * bitmap within the software logical bucket, base entry bitmap within the
 * base bucket, etc.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] e_size Size of the entry in the unit of base entry.
 * \param [in] prbank Pointer to bank.
 * \param [out] e_index Pointer to a buffer to contain the calculated entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_index_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint32_t slb,
                             rm_hash_ent_loc_t *e_loc,
                             uint8_t e_size,
                             uint8_t *prbank,
                             uint32_t *e_index);

/*!
 * \brief Get entry bucket mode
 *
 * This function get bucket mode for an entry. For a hash operation requested
 * by LTM, the corresponding rm more info structure will be determined. Thus
 * the key type value will be determined. And thus the key type will determine
 * the bucket mode of the entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Registered bank number.
 * \param [out] e_bm Pointer to rm_hash_bm_t structure.
 * \param [out] e_nm Pointer to rm_hash_entry_narrow_mode_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmptm_rm_hash_ent_bkt_mode_get(int unit,
                                const bcmptm_rm_hash_key_format_t *key_format,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                rm_hash_bm_t *e_bm,
                                rm_hash_entry_narrow_mode_t *e_nm);

/*!
 * \brief Get all the valid SIDs of the given key format.
 *
 * \param [in] unit Unit number.
 * \param [in] bview_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] sid_list Buffer to hold valid SIDs for this key format.
 * \param [out] sid_cnt Buffer to hold the number of valid SIDs.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_key_format_sids_get(int unit,
                                   const bcmptm_rm_hash_view_info_t *bview_info,
                                   const bcmptm_rm_hash_key_format_t *key_fmt,
                                   bcmdrd_sid_t *sid_list,
                                   uint8_t *sid_cnt);

/*!
 * \brief Get all the valid view info of the given key format.
 *
 * \param [in] unit Unit number.
 * \param [in] bview_info Pointer to bcmptm_rm_hash_view_info_t structure.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] view_list Buffer to hold valid views for this key format.
 * \param [out] sid_cnt Buffer to hold the number of valid views.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_key_format_view_list_get(int unit,
                                        const bcmptm_rm_hash_view_info_t *bview_info,
                                        const bcmptm_rm_hash_key_format_t *key_fmt,
                                        const bcmptm_rm_hash_view_info_t **view_list,
                                        uint8_t *view_cnt);

/*!
 * \brief Get the mapping group number for a LT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [out] grp_cnt Pointer to uint8_t objects to hold the group count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_lt_map_grp_cnt_get(int unit,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t *grp_cnt);

/*!
 * \brief Get the group sid
 *
 * \param [in] unit Unit number.
 * \param [in] sid Physical hash table sid.
 * \param [out] ovly_sid Physical overlay hash table sid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_grp_sid_get(int unit,
                           bcmdrd_sid_t sid,
                           bcmdrd_sid_t *grp_sid);

/*!
 * \brief Get the view information for an sid.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] sid Physical table sid.
 * \param [out] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_view_info_get(int unit,
                                 bcmptm_rm_hash_lt_info_t *lt_info,
                                 uint8_t grp_idx,
                                 bcmdrd_sid_t sid,
                                 const bcmptm_rm_hash_view_info_t **view_info);

/*!
 * \brief Get the view information for a key format based on entry size.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] e_size Entry size.
 * \param [out] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_size_based_view_info_get(int unit,
                                        bcmptm_rm_hash_lt_info_t *lt_info,
                                        uint8_t grp_idx,
                                        const bcmptm_rm_hash_key_format_t *key_format,
                                        uint8_t e_size,
                                        const bcmptm_rm_hash_view_info_t **view_info);

/*!
 * \brief Get the view information for a key format.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] view_idx View index.
 * \param [out] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_key_format_view_info_get(int unit,
                                        bcmptm_rm_hash_lt_info_t *lt_info,
                                        uint8_t grp_idx,
                                        const bcmptm_rm_hash_key_format_t *key_format,
                                        uint8_t view_idx,
                                        const bcmptm_rm_hash_view_info_t **view_info);

/*!
 * \brief Get the view information for an instance in a group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [in] ldtype Logical data type value.
 * \param [out] view_info Pointer to bcmptm_rm_hash_view_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_inst_exact_view_get(int unit,
                                   bcmptm_rm_hash_lt_info_t *lt_info,
                                   uint8_t grp_idx,
                                   const bcmptm_rm_hash_key_format_t *key_format,
                                   uint16_t ldtype,
                                   const bcmptm_rm_hash_view_info_t **view_info);

/*!
 * \brief Get the base view information for a key format.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [out] bview_info Pointer to bcmptm_rm_hash_view_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_base_view_info_get(int unit,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t grp_idx,
                                  const bcmptm_rm_hash_view_info_t **bview_info);

/*!
 * \brief Get the base view information for a key format.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [out] key_info Pointer to bcmptm_rm_hash_grp_key_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_grp_key_info_get(int unit,
                                bcmptm_rm_hash_lt_info_t *lt_info,
                                uint8_t grp_idx,
                                const bcmptm_rm_hash_grp_key_info_t **key_info);

/*!
 * \brief Get the physical data type.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] ldtype Logical data type value.
 * \param [out] pdtype Physical data type value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_pdtype_get(int unit,
                          bcmptm_rm_hash_lt_info_t *lt_info,
                          uint8_t grp_idx,
                          uint8_t ldtype,
                          uint16_t *pdtype);

/*!
 * \brief Get the key format information.
 *
 * \param [in] unit Unit number.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_entry_attrs_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [out] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_key_format_get(int unit,
                              bcmptm_rm_hash_entry_attrs_t *e_attrs,
                              bcmptm_rm_hash_lt_info_t *lt_info,
                              uint8_t grp_idx,
                              const bcmptm_rm_hash_key_format_t **key_fmt);

/*!
 * \brief Get the number of view information.
 *
 * \param [in] unit Unit number.
 * \param [in] key_fmt Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] num_views Number of views.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_key_format_num_view_get(int unit,
                                       const bcmptm_rm_hash_key_format_t *key_fmt,
                                       uint8_t *num_views);

/*!
 * \brief Get the view information for a logical data type.
 *
 * \param [in] unit Unit number.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_entry_attrs_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Group index.
 * \param [in] ldtype Logical data type.
 * \param [out] view_info Pointer to bcmptm_rm_hash_view_info_t*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_exact_view_get(int unit,
                                  bcmptm_rm_hash_entry_attrs_t *e_attrs,
                                  bcmptm_rm_hash_lt_info_t *lt_info,
                                  uint8_t grp_idx,
                                  uint16_t ldtype,
                                  const bcmptm_rm_hash_view_info_t **view_info);

/*!
 * \brief Get the format information for a key format.
 *
 * \param [in] unit Unit number.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_entry_attrs_t structure.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] grp_idx Group index.
 * \param [out] fmt_info Pointer to rm_hash_fmt_info_t*.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_fmt_info_get(int unit,
                            bcmptm_rm_hash_entry_attrs_t *e_attrs,
                            rm_hash_lt_ctrl_t *lt_ctrl,
                            uint8_t grp_idx,
                            rm_hash_fmt_info_t **fmt_info);

/*!
 * \brief Exchange key fields for an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] e_attrs Pointer to bcmptm_rm_hash_entry_attrs_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_a Source group index.
 * \param [in] grp_b Destination group index.
 * \param [in] entry_a Source entry buffer.
 * \param [out] entry_b Destination entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_key_fields_exchange(int unit,
                                   bcmptm_rm_hash_entry_attrs_t *e_attrs,
                                   bcmptm_rm_hash_lt_info_t *lt_info,
                                   uint8_t grp_a,
                                   uint8_t grp_b,
                                   uint32_t *entry_a,
                                   uint32_t *entry_b);

/*!
 * \brief Exchange data fields for an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] ldtype logical data type.
 * \param [in] grp_a Source group index.
 * \param [in] grp_b Destination group index.
 * \param [in] entry_a Source entry buffer.
 * \param [out] entry_b Destination entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_data_fields_exchange(int unit,
                                    bcmptm_rm_hash_lt_info_t *lt_info,
                                    uint16_t ldtype,
                                    uint8_t grp_a,
                                    uint8_t grp_b,
                                    uint32_t *entry_a,
                                    uint32_t *entry_b);

/*!
 * \brief Get entry view information.
 *
 * \param [in] unit Unit number.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] lt_info Pointer to bcmptm_rm_hash_lt_info_t structure.
 * \param [in] grp_idx Mapping group index.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] view_info Entry view info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_req_ent_view_info_get(int unit,
                                     bcmptm_rm_hash_req_t *req_info,
                                     bcmptm_rm_hash_lt_info_t *lt_info,
                                     uint8_t grp_idx,
                                     const bcmptm_rm_hash_key_format_t *key_format,
                                     const bcmptm_rm_hash_view_info_t **view_info);
#endif /* BCMPTM_RM_HASH_UTILS_H */
