/*! \file bcmecmp_member_weight.h
 *
 * Member weight group utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_MEMBER_WEIGHT_H
#define BCMECMP_MEMBER_WEIGHT_H

#include <bcmecmp/bcmecmp_types.h>


/*!
 * \brief Member weight group.
 */
typedef struct bcmecmp_member_weight_grp_s {
    /*! Next hop index A. */
    bcmecmp_nhop_id_t *nhop_id_a;

    /*! Next hop index B. */
    bcmecmp_nhop_id_t *nhop_id_b;

    /*! Weight of next hop index A. */
    uint32_t *weight_a;

    /*! Weight of next hop index B. */
    uint32_t *weight_b;

    /*! Average weight, which is shared by each next hop index pair. */
    uint32_t ave_weight;

    /*! Number of group member. */
    int group_size;
} bcmecmp_member_weight_grp_t;


/*!
 * \brief Distribute the sum of weight to the member pairs.
 *
 * \param [in] unit Unit number.
 * \param [in] nhop_id_list Next hop index array.
 * \param [in] weight_list Weight array.
 * \param [in] num_paths Number of array.
 * \param [in] quant_factor Quantization factor.
 * \param [inout] member_weight_grp Pointer of member weight group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 * \retval SHR_E_FAIL Weight distribution failed.
 */
int
bcmecmp_member_weight_distribute(int unit,
                                 bcmecmp_nhop_id_t *nhop_id_list,
                                 uint32_t *weight_list,
                                 int num_paths,
                                 uint32_t quant_factor,
                                 bcmecmp_member_weight_grp_t *member_weight_grp);

#endif /* BCMECMP_MEMBER_WEIGHT_H */
