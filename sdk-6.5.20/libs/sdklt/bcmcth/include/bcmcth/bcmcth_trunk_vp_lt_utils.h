/*! \file bcmcth_trunk_vp_lt_utils.h
 *
 * This file contains TRUNK_VP custom table handler utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_LT_UTILS_H
#define BCMCTH_TRUNK_VP_LT_UTILS_H

#include <sal/sal_types.h>
#include <bcmcth/bcmcth_trunk_vp_db_internal.h>
#include <bcmcth/bcmcth_trunk_vp_types.h>

/*! First integer value greather than Second integer value.  */
#define BCMCTH_TRUNK_VP_CMP_GREATER       (1)

/*! First integer value equal to the Second integer value.  */
#define BCMCTH_TRUNK_VP_CMP_EQUAL         (0)

/*! First integer value less than the Second integer value.  */
#define BCMCTH_TRUNK_VP_CMP_LESS         (-1)

/*!
 * \brief Compares two integer values and returns the status.
 *
 * \param [in] a Pointer to the First integer value to compare.
 * \param [in] b Pointer to the Second integer value to compare with.
 *
 * \returns BCMCTH_TRUNK_VP_CMP_GREATER When first value is greater than second.
 * \returns BCMCTH_TRUNK_VP_CMP_EQUAL When both values are equal.
 * \returns BCMCTH_TRUNK_VP_CMP_LESS When first value is lesser than second.
 */
extern int
bcmcth_trunk_vp_cmp_int(const void *a,
                const void *b);

/*!
 * \brief Compares member table start indices of the defragmentation blocks.
 *
 * \param [in] a First defragmentation block to compare.
 * \param [in] b Second defragmentation block to compare with.
 *
 * \returns BCMCTH_TRUNK_VP_CMP_GREATER When first value is greater than second.
 * \returns BCMCTH_TRUNK_VP_CMP_EQUAL When both values are equal.
 * \returns BCMCTH_TRUNK_VP_CMP_LESS When first value is lesser than second.
 */
extern int
bcmcth_trunk_vp_defrag_grps_cmp(const void *a,
                        const void *b);


/*!
 * \brief This function allocates HA memory for storing HW entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in] ha_blk_id ha block ID string.
 * \param [in,out] alloc_ptr ptr of ptr of hw entry array info.
 * \param [in,out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmcth_trunk_vp_hw_entry_ha_alloc(int unit, bool warm,
                          uint32_t array_size, uint8_t submod_id,
                          uint32_t ref_sign,
                          const char *ha_blk_id,
                          bcmcth_trunk_vp_hw_entry_ha_blk_t **alloc_ptr,
                          uint32_t* out_ha_alloc_sz);

/*!
 * \brief This function allocates HA memory for storing group attr.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] array_size hw entries array size.
 * \param [in] submod_id sub module ID.
 * \param [in] ref_sign expected signature.
 * \param [in] ha_blk_id ha block ID string.
 * \param [in,out] alloc_ptr ptr of ptr of group attr array info.
 * \param [in,out] out_ha_alloc_sz out ha allocated size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
extern int
bcmcth_trunk_vp_group_attr_ha_alloc(int unit, bool warm,
                            uint32_t array_size, uint8_t submod_id,
                            uint32_t ref_sign,
                            const char *ha_blk_id,
                            bcmcth_trunk_vp_grp_ha_blk_t **alloc_ptr,
                            uint32_t* out_ha_alloc_sz);
/*!
 * \brief Check whether an TRUNK_VP group is in use.
 *
 * \param [in] unit Unit number.
 * \param [in] trunk_vp_id TRUNK_VP Identifier.
 * \param [in] gtype group type.
 * \param [out] in_use flag true if entry is in use.
 * \param [out] comp_id component using the group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmcth_trunk_vp_grp_in_use(int unit,
                           uint32_t trunk_vp_id,
                           bcmcth_trunk_vp_grp_type_t gtype,
                           bool *in_use,
                           uint32_t *comp_id);


#endif /* BCMCTH_TRUNK_VP_LT_UTILS_H */
