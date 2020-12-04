/*! \file sbr_util.h
 *
 * SBR sub-component utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SBR_UTIL_H
#define SBR_UTIL_H
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_sbr_internal.h>

/*!
 * \brief Generic hardware mem/reg table read function for SBR component.
 *
 * \param [in] unit Unit number.
 * \param [in] only_check_rsrc Resource check only call.
 * \param [in] lt_id Logical table identifier.
 * \param [in] op_arg Operation arguments.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to read.
 * \param [in] entry_size Size of entry_data.
 * \param [out] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_sbr_lt_hw_read(int unit,
                      bool only_check_rsrc,
                      bcmltd_sid_t lt_id,
                      const bcmltd_op_arg_t *op_arg,
                      bcmdrd_sid_t pt_id,
                      int index,
                      uint32_t entry_size,
                      void *entry_data);

/*!
 * \brief Generic hardware mem/reg table write function for SBR component.
 *
 * \param [in] unit Unit number.
 * \param [in] only_check_rsrc Resource check only call.
 * \param [in] lt_id Logical table identifier.
 * \param [in] op_arg Operation arguments.
 * \param [in] pt_id Memory table identifier.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_sbr_lt_hw_write(int unit,
                       bool only_check_rsrc,
                       bcmltd_sid_t lt_id,
                       const bcmltd_op_arg_t *op_arg,
                       bcmdrd_sid_t pt_id,
                       int index,
                       void *entry_data);

/*!
 * \brief Get SBR profile table entry usage mode name.
 *
 * \param [in] usage SBR profile table entry usage mode value.
 *
 * \retval Pointer to SBR profile table entry usage mode string.
 */
extern const char *
bcmptm_sbr_ent_usage_str(bcmptm_sbr_entry_usage_t usage);

/*!
 * \brief Get Tile LT.LOOKUP type string.
 *
 * \param [in] lkup Tile LT.LOOKUP type value.
 *
 * \retval Pointer Tile LT.LOOKUP type string.
 */
extern const char *
bcmptm_sbr_lt_lkup_str(bcmptm_sbr_lt_lookup_t lkup);

/*!
 * \brief Get the SBR profile table entry operation name.
 *
 * \param [in] spt_eoper SBR profile table entry operation.
 *
 * \retval Pointer to SBR profile table entry operation string.
 */
extern const char *
bcmptm_sbr_pt_ent_oper_str(sbr_pt_ent_oper_t spt_eoper);

/*!
 * \brief Get the tile LT.LOOKUP operation name.
 *
 * \param [in] ltlkup_oper LT.LOOKUP operation.
 *
 * \retval Pointer to LT.LOOKUP operation string.
 */
extern const char *
bcmptm_sbr_tm_lt_lookup_oper_str(sbr_tm_lt_lookup_oper_t ltlkup_oper);

/*!
 * \brief Initialize a tile state structure instance members.
 *
 * \param [in] unit Unit number.
 * \param [in] ts Pointer to \ref bcmptm_sbr_tile_state_t structure instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On null \c ts pointer.
 */
extern int
bcmptm_sbr_tile_state_t_init(int unit,
                             bcmptm_sbr_tile_state_t *ts);
/*!
 * \brief Initialize a tile LT.LOOKUP info structure instance members.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] ltlkup Pointer to \ref bcmptm_sbr_lt_lookup_state_tile_t tile
 *                    LT.LOOKUP info structure instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On null \c di or \c ltlkup pointer.
 */
extern int
bcmptm_sbr_lt_lookup_state_tile_t_init(
                                    int unit,
                                    sbr_dev_info_t *di,
                                    bcmptm_sbr_lt_lookup_state_tile_t *ltlkup);
/*!
 * \brief Initialize a SBR profile table state structure instance members.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] spt Pointer to \ref bcmptm_sbr_pt_state_t SBR profile table
 *                 state structure instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On null \c di or \c spt pointer.
 */
extern int
bcmptm_sbr_pt_state_t_init(int unit,
                           sbr_dev_info_t *di,
                           bcmptm_sbr_pt_state_t *spt);
/*!
 * \brief Initialize a SBR profile table entry state structure instance members.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] spt_estate Pointer to \ref bcmptm_sbr_entry_state_t SBR profile
 *                        table entry state structure instance.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On null \c di or \c spt pointer.
 */
extern int
bcmptm_sbr_entry_state_t_init(int unit,
                              sbr_dev_info_t *di,
                              bcmptm_sbr_entry_state_t *spt_estate);
/*!
 * \brief Get the index of the tile that supports a given tile_mode.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Pointer to variant driver structure instance.
 * \param [in] tile_mode Tile mode.
 * \param [out] tile_index Index of the tile that supports the given tile mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On invalid input parameter value.
 * \retval SHR_E_NOT_FOUND No tiles support the given \c tile_mode.
 */
extern int
bcmptm_sbr_find_tile_by_tile_mode(int unit,
                                  const bcmptm_sbr_var_drv_t *drv,
                                  uint32_t tile_mode,
                                  uint16_t *tile_index);
/*!
 * \brief Find the index of a Tile in tile_state_ha array by searching for it
 *        using its tile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] tile_id Tile ID.
 * \param [out] tile_index Index of given tile \c tile_id in the variant driver
 *                         sbr_tile_info array \ref bcmptm_sbr_tile_info_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On invalid input parameter value.
 * \retval SHR_E_NOT_FOUND When a match is not found.
 */
extern int
bcmptm_sbr_find_tile_by_tile_id(int unit,
                                sbr_dev_info_t *di,
                                uint32_t tile_id,
                                uint16_t *tile_index);
/*!
 * \brief Find the index of a SBR profile table in spt_state_ha array by
 *        searching for it using its drd_sid.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] spt_drd_sid SBR profile table DRD_SID.
 * \param [out] spt_index Index of given SBR profile table \c spt_drd_sid in
                          the variant driver sbr_profile_table array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On invalid input parameter value.
 * \retval SHR_E_NOT_FOUND When a match is not found.
 */
extern int
bcmptm_sbr_find_spt_by_spt_id(int unit,
                              sbr_dev_info_t *di,
                              uint32_t spt_drd_sid,
                              uint16_t *spt_index);
/*!
 * \brief Find the index of a LT.LOOKUP in tile_lkup_state_ha array by
 *        by searching for it using LT ID and lookup type pair.
 *        its drd_sid.
 *
 * \param [in] unit Unit number.
 * \param [in] di Pointer to \ref sbr_dev_info_t device info structure instance.
 * \param [in] tile_index Tile index.
 * \param [in] ltid Logical Table ID.
 * \param [in] lookup_type Tile lookup instance.
 * \param [out] ltlkup_index Index of given \c ltid and \c lookup_type pair in
 *                           the tile \ref bcmptm_sbr_lt_lookup_info_tile_t
 *                           array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM On invalid input parameter value.
 * \retval SHR_E_NOT_FOUND When a match is not found.
 */
extern int
bcmptm_sbr_find_tile_ltlkup_by_lkupltid(int unit,
                                        sbr_dev_info_t *di,
                                        uint16_t tile_index,
                                        uint32_t ltid,
                                        uint16_t lookup_type,
                                        uint16_t *ltlkup_index);

#endif /* SBR_UTIL_H */
