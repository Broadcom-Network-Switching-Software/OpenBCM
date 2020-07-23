/*! \file rm_alpm_device.h
 *
 * RM ALPM device specific info
 *
 * This file contains device specific info which are internal to
 * ALPM Resource Manager
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_ALPM_DEVICE_H
#define RM_ALPM_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

#include "rm_alpm.h"

/*******************************************************************************
 * Defines
 */
#define ALPM_DEV(u, m)          (alpm_dev_info[u][m])
#define ALPM_DRV(u, m)          (alpm_driver[u][m])
#define ALPM_DRV_EXEC(u, m, fn)    (ALPM_DRV(u, m)->fn)

/*! Weighted vrf count including LPM, L3MC in parallel mode or combined mode. */
#define WVRF_COUNT(max_vrf)  (max_vrf + 3 + 1)
/*! Weighted vrf count including global high & global low in parallel mode. */
#define WVRF_COUNT_PARALLEL_PUBLIC  (2)
/*! Weighted vrf (L4_PORT) count for APP_COMP0 + no L4_PORT case */
#define WVRF_COUNT_FP(max_fp_op)  (max_fp_op + 1 + 1)

/*******************************************************************************
 * Function prototypes
 */
extern uint8_t
bcmptm_rm_alpm_hit_version(int u, int m);

/*!
 * \brief Get ALPM device specfic info
 *
 * \param [in] u Device u.
 *
 * \return ALPM device specfic info ptr
 */
extern alpm_dev_info_t *
bcmptm_rm_alpm_device_info_get(int u, int m);


/*!
 * \brief Encode Level-1 entry (both key and data parts).
 *
 * \param [in] u Device u.
 * \param [in] arg ALPM args
 * \param [out] de DIP entry
 * \param [out] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_encode(int u, int m, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

/*!
 * \brief Decode Level-1 entry.
 *
 * \param [in] u Device u.
 * \param [in|out] arg ALPM args
 * \param [in] de DIP entry
 * \param [in] se SIP entry
 * \param [in] l1v Level-1 view type
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_decode(int u, int m, alpm_arg_t *arg, void *de, void *se, uint8_t l1v);

extern int
bcmptm_rm_alpm_l1_unpair_sid(int u, int m, alpm_key_type_t kt, int index);

extern int
bcmptm_rm_alpm_l1_pair_sid(int u, int m, alpm_key_type_t kt, int index);
extern int
bcmptm_rm_alpm_ln_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info);
extern int
bcmptm_rm_alpm_l1_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info);
extern int
bcmptm_rm_alpm_l1_half_key_clear(int u, int m, void *de, int x, int pkm);
extern int
bcmptm_rm_alpm_l1_half_key_move(int u, int m, void *src, void *dst,
                                  int x, int y, int pkm);
extern int
bcmptm_rm_alpm_l1_sid_index_reverse_map(int u, int m, bcmdrd_sid_t hw_sid,
                                        alpm_sid_index_out_t *out_info);

/*!
 * \brief Encode level-n base entry.
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] kshift Key shift
 * \param [in] arg ALPM args
 * \param [out] entry Base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_encode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry);

/*!
 * \brief Decode level-n base entry.
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] kshift Key shift
 * \param [in] arg ALPM args
 * \param [out] entry Base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_decode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry);

/*!
 * \brief Encode level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] base_entry Base entry array
 * \param [in] entry_count Count of array
 * \param [out] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_encode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    ln_base_entry_t *base_entry,
                                    int entry_count,
                                    void *buf);

/*!
 * \brief Decode level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] base_entry Base entry array
 * \param [in] entry_count Count of array
 * \param [in] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_decode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    ln_base_entry_t *base_entry,
                                    int entry_count,
                                    void *buf);

/*!
 * \brief Set one base entry of level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [in] base_entry Base entry
 * \param [in] ent_ofs Offset of raw bucket for base entry
 * \param [out] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_entry_set(int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *buf);

/*!
 * \brief Get one base entry of level-n raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] base_entry Base entry
 * \param [in] ent_ofs Offset of raw bucket for base entry
 * \param [in] buf Raw bucket buffer
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_raw_bucket_entry_get(int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *buf);


/*!
 * \brief Get level-n base entry info
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] format_value Format value
 * \param [out] prefix_len Prefix length
 * \param [out] full Is full data type
 * \param [out] num_entry Number of base entries
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_info(int u, int m,
                                  format_type_t format_type,
                                  uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *num_entry);

/*!
 * \brief Level-n base entry move from one raw bucket to another raw bucket
 *
 * \param [in] u Device u.
 * \param [in] format_type Foramt type
 * \param [in] src_format_value Format value for src base entry
 * \param [in] src_ent_ofs Offset of bucket for src base entry
 * \param [in] src_t Key tuple of src base entry
 * \param [out] src_bucket Raw bucket buffer holding src base entry
 * \param [in] dst_value_from Format value for dst base entry
 * \param [in] dst_ent_ofs Offset of bucket for dst base entry
 * \param [in] dst_t Key tuple of dst base entry
 * \param [out] dst_bucket Raw bucket buffer holding dst base entry
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_ln_base_entry_move(int u, int m, format_type_t format_type,
                                  uint8_t src_format_value,
                                  int src_ent_ofs,
                                  key_tuple_t *src_t,
                                  void *src_bucket,
                                  alpm_data_type_t src_dt,

                                  uint8_t dst_format_value,
                                  int dst_ent_ofs,
                                  key_tuple_t *dst_t,
                                  void *dst_bucket);

/*!
 * \brief Get Level-n entry hit bit
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] ofs Hit bit offset in the hit entry
 * \param [in] hit_entry Hit entry
 * \param [out] hit hit status
 *
 * \return SHR_E_NONE, SHR_E_PARAM.
 */
extern int
bcmptm_rm_alpm_ln_hit_get(int u, int m, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t *hit);


/*!
 * \brief Set Level-n entry hit bit
 *
 * \param [in] u Device u.
 * \param [in] ln Level
 * \param [in] ofs Hit bit offset in the hit entry
 * \param [in] hit_entry Hit entry
 * \param [in] hit hit status
 *
 * \return SHR_E_NONE, SHR_E_PARAM.
 */
extern int
bcmptm_rm_alpm_ln_hit_set(int u, int m, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t hit);


/*!
 * \brief Move level-1 half entry
 *
 * \param [in] u Device u.
 * \param [in] src Source entry
 * \param [out] dst Destination entry
 * \param [in] x Source half
 * \param [in] y Destination half
 * \param [in] pkm Packing mode.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_l1_half_entry_move(int u, int m, void *src, void *dst,
                                  int x, int y, int pkm);

extern int
bcmptm_rm_alpm_wvrf_get(int u, int m, int app, alpm_vrf_type_t vt, int decode_vrf);

/*!
 * \brief Initialize RM ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_rm_alpm_device_init(int u, int m);

/*!
 * \brief Cleanup RM ALPM device module
 *
 * \param [in] u Device u.
 *
 * \return nothing.
 */
extern void
bcmptm_rm_alpm_device_cleanup(int u, int m);

#endif /* RM_ALPM_DEVICE_H */
