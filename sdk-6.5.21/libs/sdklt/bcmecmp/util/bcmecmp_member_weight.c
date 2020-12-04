/*! \file bcmecmp_member_weight.c
 *
 * Member weight group utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmecmp/bcmecmp_member_weight.h>
#include <bcmecmp/bcmecmp_util_bucket.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

static uint32_t
average_weight_compute(uint32_t *weight_list, int num_paths)
{
    uint32_t tot_weight = 0, ave_weight;
    int idx;

    for (idx = 0; idx < num_paths; idx++) {
        tot_weight += weight_list[idx];
    }

    if (tot_weight % num_paths) {
        /* Scale up the weight of all members. */
        for (idx = 0; idx < num_paths; idx++) {
            weight_list[idx] *= num_paths;
        }
        ave_weight = tot_weight;
    } else {
        ave_weight = tot_weight / num_paths;
    }

    return ave_weight;
}

static int
bucket_weight_distribute(int unit,
                         bcmecmp_nhop_id_t *nhop_list, uint32_t *weight_list,
                         bcmecmp_member_weight_grp_t *member_weight_grp)
{
    bcmecmp_bucket_handle_t pos_credit_bucket = BCMECMP_BUCKET_INVALID_HANDLE;
    bcmecmp_bucket_handle_t neg_credit_bucket = BCMECMP_BUCKET_INVALID_HANDLE;
    uint32_t my_credit = 0, max_credit = 0, min_credit = 0, max_idx, min_idx;
    int idx, pos_credit_bucket_size = 0, neg_credit_bucket_size = 0, grp_size = 0;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < member_weight_grp->group_size; idx++) {
        if (weight_list[idx] > member_weight_grp->ave_weight) {
            my_credit = weight_list[idx] - member_weight_grp->ave_weight;
            if (my_credit > max_credit) {
                max_credit = my_credit;
            }
            pos_credit_bucket_size++;
        } else if (weight_list[idx] < member_weight_grp->ave_weight) {
            my_credit = member_weight_grp->ave_weight - weight_list[idx];
            if (my_credit > min_credit) {
                min_credit = my_credit;
            }
            neg_credit_bucket_size++;
        } else {
            member_weight_grp->nhop_id_a[idx] = nhop_list[idx];
            member_weight_grp->nhop_id_b[idx] = nhop_list[idx];
            member_weight_grp->weight_a[idx] = member_weight_grp->ave_weight;
            member_weight_grp->weight_b[idx] = 0;
            grp_size++;
        }
    }

    /* All members already get balance. */
    if (member_weight_grp->group_size == grp_size) {
        SHR_EXIT();
    }

    pos_credit_bucket = bcmecmp_bucket_create(pos_credit_bucket_size,
                                              max_credit);
    if (pos_credit_bucket == BCMECMP_BUCKET_INVALID_HANDLE) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    neg_credit_bucket = bcmecmp_bucket_create(neg_credit_bucket_size,
                                              min_credit);
    if (neg_credit_bucket == BCMECMP_BUCKET_INVALID_HANDLE) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Set up positive and negative credit bucket. */
    for (idx = 0; idx < member_weight_grp->group_size; idx++) {
        if (weight_list[idx] > member_weight_grp->ave_weight) {
            my_credit = weight_list[idx] - member_weight_grp->ave_weight;
            bcmecmp_bucket_insert(pos_credit_bucket, my_credit, idx);
        } else if (weight_list[idx] < member_weight_grp->ave_weight) {
            my_credit = member_weight_grp->ave_weight - weight_list[idx];
            bcmecmp_bucket_insert(neg_credit_bucket, my_credit, idx);
        }
    }

    /*
     * The basic idea is based on the following theory:
     * Wmax + Wmin > Wavg
     * For each iteration, first identify a Wmax member and a Wmin member,
     * then move Wmax member's weight to fill up Wmin member's weight up
     * to Wavg.
     * So after the rebalance:
     * NHImax: Wmax - (Wavg-Wmin)
     * NHImin: Wmin + (Wavg-Wmin)
     * After this rebalance the NHImin is full(done) and can be taken out from
     * the iteration.
     * Repeat the same process again until all weight distributed.
     */
    while (bcmecmp_bucket_remove_max(pos_credit_bucket, &max_credit, &max_idx) == 0) {
        if (bcmecmp_bucket_remove_max(neg_credit_bucket, &min_credit, &min_idx) != 0) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* Transfer the credit from Wmax to Wmin. */
        my_credit = min_credit;
        weight_list[max_idx] -= my_credit;
        weight_list[min_idx] += my_credit;

        /* Put the Wmax back into credit heap. */
        if (weight_list[max_idx] > member_weight_grp->ave_weight) {
            my_credit = weight_list[max_idx] - member_weight_grp->ave_weight;
            bcmecmp_bucket_insert(pos_credit_bucket, my_credit, max_idx);
        } else if (weight_list[max_idx] < member_weight_grp->ave_weight) {
            my_credit = member_weight_grp->ave_weight - weight_list[max_idx];
            bcmecmp_bucket_insert(neg_credit_bucket, my_credit, max_idx);
        } else {
            member_weight_grp->nhop_id_a[max_idx] = nhop_list[max_idx];
            member_weight_grp->nhop_id_b[max_idx] = nhop_list[max_idx];
            member_weight_grp->weight_a[max_idx] = member_weight_grp->ave_weight;
            member_weight_grp->weight_b[max_idx] = 0;
            grp_size++;
        }

        /* Something is wrong. */
        if (weight_list[min_idx] != member_weight_grp->ave_weight) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        member_weight_grp->nhop_id_a[min_idx] = nhop_list[max_idx];
        member_weight_grp->nhop_id_b[min_idx] = nhop_list[min_idx];
        member_weight_grp->weight_a[min_idx] = min_credit;
        member_weight_grp->weight_b[min_idx] = member_weight_grp->ave_weight -
                                               min_credit;
        grp_size++;
    }

    if (member_weight_grp->group_size != grp_size) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (pos_credit_bucket != BCMECMP_BUCKET_INVALID_HANDLE) {
        bcmecmp_bucket_destroy(pos_credit_bucket);
    }

    if (neg_credit_bucket != BCMECMP_BUCKET_INVALID_HANDLE) {
        bcmecmp_bucket_destroy(neg_credit_bucket);
    }

    SHR_FUNC_EXIT();
}

static uint32_t
weight_quantize(uint32_t memb_weight, uint32_t ave_weight,
                uint32_t quant_factor)
{
    uint64_t val1, val2;

    val1 = (memb_weight * (uint64_t)quant_factor) / ave_weight;
    val2 = (memb_weight * (uint64_t)quant_factor) % ave_weight;

    /* Round to the nearest integer. */
    if (val2 > (ave_weight / 2)) {
        val1++;
    }

    return (uint32_t)val1;
}

static int
bcmecmp_member_weight_quantize(int unit, uint32_t quant_factor,
                               bcmecmp_member_weight_grp_t *member_weight_grp)
{
    uint32_t sum, w_a, w_b;
    int idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(member_weight_grp, SHR_E_PARAM);

    for (idx = 0; idx < member_weight_grp->group_size; idx++) {
        sum = 0;

        w_a = weight_quantize(member_weight_grp->weight_a[idx],
                              member_weight_grp->ave_weight,
                              quant_factor);

        w_b = weight_quantize(member_weight_grp->weight_b[idx],
                              member_weight_grp->ave_weight,
                              quant_factor);

        sum = w_a + w_b;
        if (sum != quant_factor) {
            LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Weight quantize failed. Input weight: A = %u, \
                              B = %u, averge = %u, Output weight: A = %u, \
                              B = %u\n"),
                   member_weight_grp->weight_a[idx],
                   member_weight_grp->weight_b[idx],
                   member_weight_grp->ave_weight, w_a, w_b));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        member_weight_grp->weight_a[idx] = w_a;
        member_weight_grp->weight_b[idx] = w_b;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_member_weight_distribute(int unit,
                                 bcmecmp_nhop_id_t *nhop_list,
                                 uint32_t *weight_list,
                                 int num_paths,
                                 uint32_t quant_factor,
                                 bcmecmp_member_weight_grp_t *member_weight_grp)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(nhop_list, SHR_E_PARAM);
    SHR_NULL_CHECK(weight_list, SHR_E_PARAM);
    SHR_NULL_CHECK(member_weight_grp, SHR_E_PARAM);

    if (num_paths <= 0) {
        SHR_EXIT();
    }

    member_weight_grp->group_size = num_paths;

    /* Calculate average weight. */
    member_weight_grp->ave_weight = average_weight_compute(weight_list,
                                                           num_paths);

    /* Distribute weight. */
    SHR_IF_ERR_EXIT
        (bucket_weight_distribute(unit, nhop_list, weight_list,
                                  member_weight_grp));

    /* Quantize weight. */
    SHR_IF_ERR_EXIT
        (bcmecmp_member_weight_quantize(unit, quant_factor, member_weight_grp));

exit:
    SHR_FUNC_EXIT();
}
