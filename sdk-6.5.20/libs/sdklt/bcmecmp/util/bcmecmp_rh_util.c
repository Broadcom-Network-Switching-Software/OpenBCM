/*! \file bcmecmp_rh_util.c
 *
 * Resilient hashing re-balance utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmecmp/bcmecmp_rh_util.h>
#include <bcmecmp/bcmecmp_db_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*!
 * \brief Data structure for resilient hashing
 */
typedef struct bcmecmp_rh_elem_s {

    /*! Resource ID for unique identifier. */
    int res_id;

    /*! Max entry number of a specific resource. */
    uint32_t entry_max;

    /*! Number of destination. */
    uint32_t num_dest;

    /*!
     * Index of resilient hashing load balanced member
     * mapped to resource entry.
     */
    uint32_t *mindex_arr;

    /*! Replica of resource entry. */
    uint32_t *replicas;

} bcmecmp_rh_elem_t;

static int
rh_entry_elem_convert(int unit,
                      bcmecmp_lt_entry_t *lt_entry,
                      bcmecmp_rh_elem_t *elem)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(elem, SHR_E_PARAM);

    elem->res_id = lt_entry->ecmp_id;
    elem->num_dest = lt_entry->rh_num_paths;
    elem->entry_max = lt_entry->rh_entries_cnt;
    elem->mindex_arr = lt_entry->rh_mindex_arr;
    elem->replicas = lt_entry->rh_memb_ecnt_arr;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Returns a random member index value within an index range.
 *
 * Generates and returns a random member index value given an index range.
 *
 * \param [in] unit Unit number.
 * \param [in] id Constant for random index generation.
 * \param [in] rand_max Max index value limit for random index generation.
 * \param [out] rand_index Pointer to generated randon index value.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
rh_rand_memb_index_get(int unit,
                       uint32_t id,
                       uint32_t rand_max,
                       uint32_t *rand_index)
{
    uint32_t modulus;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rand_index, SHR_E_PARAM);

    modulus = rand_max + 1;

    if (modulus > (uint32_t)(1 << (32 - RH_RAND_SEED_SHIFT))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMECMP_RH_RAND_SEED(unit, id) =
        BCMECMP_RH_RAND_SEED(unit, id) * 1103515245 + 12345;

    *rand_index =
        (BCMECMP_RH_RAND_SEED(unit,id) >> RH_RAND_SEED_SHIFT) % modulus;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Returnes RH member array index where the next-hop value must be set.
 *
 * Calculates and returns the RH member array index where the next-hop member
 * value must be set. Maximum entries and additional overflow entries that a RH
 * group's member can occupy in ECMP member table are used in this calculation
 * for RH member array index selection.
 *
 * \param [in] unit Unit number.
 * \param [in] elem Pointer to resilient hashing data structure.
 * \param [out] selected_index Pointer to selected to next-hop member index.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Internal error.
 */
static int
rh_memb_rebalance(int unit,
                  bcmecmp_rh_elem_t *elem,
                  uint32_t *selected_index)
{
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t mindex = 0; /* Random member index value. */
    uint32_t nindex = 0; /* New member index value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(elem, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "rh_memb_rebalance.\n")));

    *selected_index = mindex;

    /*
     * Validate NUM_PATHS value and confirm it's non-zero to perform RH load
     * balancing. If value is zero, there are no members to perform load
     * balancing.
     */
    if (elem->num_dest == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members on rh destination=%u, random=%d.\n"),
                     elem->res_id, mindex));
        SHR_EXIT();
    }

    max_ent_cnt = (elem->entry_max / elem->num_dest);
    overflow_cnt = (elem->entry_max % elem->num_dest);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[rh_entries_cnt=%-4u, "
                            "rh_num_paths=%-4d, max_ent_cnt:%-4u, ovf_cnt=%u].\n"),
                 elem->entry_max,
                 elem->num_dest,
                 max_ent_cnt,
                 overflow_cnt));

    SHR_IF_ERR_EXIT
        (rh_rand_memb_index_get(unit,
                                elem->res_id,
                                (elem->num_dest - 1),
                                &mindex));

    if (elem->replicas[mindex] < max_ent_cnt) {
        elem->replicas[mindex] += 1;
        *selected_index = mindex;
    } else {
        nindex = (mindex + 1) % elem->num_dest;
        while (nindex != mindex) {
            if (elem->replicas[nindex] < max_ent_cnt) {
                elem->replicas[nindex] += 1;
                *selected_index = nindex;
                break;
            } else {
                nindex = (nindex + 1) % elem->num_dest;
            }
        }
        if (nindex == mindex) {
            if (elem->replicas[nindex] < (max_ent_cnt + 1)
                && (0 != overflow_cnt)) {
                elem->replicas[nindex] +=1;
                *selected_index = nindex;
                overflow_cnt -= 1;
            } else {
                nindex = (mindex + 1) % elem->num_dest;
                while (nindex != mindex) {
                    if (elem->replicas[nindex] < (max_ent_cnt + 1)
                        && (0 != overflow_cnt)) {
                        elem->replicas[nindex] += 1;
                        *selected_index = nindex;
                        overflow_cnt -= 1;
                        break;
                    } else {
                        nindex = (nindex + 1) % elem->num_dest;
                    }
                }
                if (nindex == mindex) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_rh_elem_member_init(int unit,
                            uint32_t eindex,
                            void *entry,
                            bcmecmp_rh_memb_fn cb)
{
    uint32_t sel_idx = (uint32_t)BCMECMP_INVALID; /* Selected member index. */
    bcmecmp_lt_entry_t *lt_entry = NULL;
    bcmecmp_rh_elem_t elem;
    rh_memb_info_t minfo; /* RH member info data. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    sal_memset(&minfo, 0, sizeof(minfo));

    lt_entry = (bcmecmp_lt_entry_t *)entry;

    SHR_IF_ERR_EXIT
        (rh_entry_elem_convert(unit, lt_entry, &elem));

    SHR_IF_ERR_EXIT
        (rh_memb_rebalance(unit,
                           &elem,
                           &sel_idx));

    /*
     * Store selected next-hop array member index value for use during
     * Add/Delete/Replace operations.
     */
    elem.mindex_arr[eindex] = sel_idx;

    if (cb) {
         minfo.entry = (void *)lt_entry;
         minfo.opc = RH_INIT;
         minfo.mindex = sel_idx;
         minfo.eindex = eindex;

         SHR_IF_ERR_EXIT((cb)(unit, &minfo));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_rh_elem_member_add(int unit,
                           uint32_t new_mindex,
                           void *entry,
                           bcmecmp_rh_memb_fn cb)
{
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    uint32_t eindex = 0;      /* Selected entry index. */
    uint32_t next_eindex = 0; /* Next entry index. */
    uint32_t *ecount = NULL;  /* New member entries usage count. */
    uint32_t mindex;          /* member index. */
    bool new_memb_sel = FALSE; /* New member selected. */
    rh_memb_info_t minfo; /* RH member info data. */
    bcmecmp_rh_elem_t elem;
    bcmecmp_lt_entry_t *lt_entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    sal_memset(&minfo, 0, sizeof(minfo));

    lt_entry = (bcmecmp_lt_entry_t *)entry;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s - n_mindex=%u\n"),
             __func__,
             new_mindex));

    SHR_IF_ERR_EXIT
        (rh_entry_elem_convert(unit, lt_entry, &elem));

    lbound = elem.entry_max / elem.num_dest;
    /* The balance requests one number between lower and upper. */
    ubound = (elem.entry_max % elem.num_dest) ? (lbound + 1) : lbound;
    threshold = ubound;

    /*
     * Set local entry count pointer to point to new member's entry usage
     * count array element address.
     */
    ecount = &(elem.replicas[new_mindex]);

    /*
     * New member initial entries usage count is expected to be zero. If it's
     * non-zero it's an error.
     */
    if (*ecount) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: new_mindex=%u non-zero ecount=%u.\n"),
                     new_mindex,
                     *ecount));
        return (SHR_E_INTERNAL);
    }

    /* Get the new next-hop member ID value. */
    /* new_nhop_id = elem->memb_arr[new_mindex]; */

    while (*ecount < lbound) {
        /* Select a random entry to replace with new member. */
        SHR_IF_ERR_EXIT
            (rh_rand_memb_index_get(unit,
                                    elem.res_id,
                                    (elem.entry_max - 1),
                                    &eindex));
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "eindex=%d\n"),
             eindex));

        if (elem.mindex_arr[eindex] == new_mindex) {
            new_memb_sel = TRUE;
        } else {
            new_memb_sel = FALSE;
            mindex = elem.mindex_arr[eindex];
        }

        if (!new_memb_sel &&
            (elem.replicas[mindex] > threshold)) {

            /* Figure out new mindex, replicates member ID to entries. */
            if (cb) {
                minfo.entry = (void *)lt_entry;
                minfo.opc = RH_ADD;
                minfo.mindex = new_mindex;
                minfo.eindex = eindex;

                SHR_IF_ERR_EXIT((cb)(unit, &minfo));
            }

            /* Store the index of replica. */
            elem.mindex_arr[eindex] = new_mindex;

            /* Decrement entry usage count value for the replaced member. */
            elem.replicas[mindex] -= 1;

            /* Increment entry usage count value for the new member. */
            *ecount += 1;
        } else {
            /*
             * Arrived here because the randomly selected entry index happened
             * to be the new member or the selected existing member's entry
             * usage count is below the threshold value.
             *
             * In either case, find the next entry index that's either not
             * populated with the new member or existing entry with entry usage
             * count value that's equal to greater than the threshold value.
             */
            next_eindex = (eindex + 1) % elem.entry_max;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "eindex=%d next=%d\n"),
                 eindex, next_eindex));

            while (next_eindex != eindex) {

                mindex = elem.mindex_arr[next_eindex];

                if (mindex == new_mindex) {
                    new_memb_sel = TRUE;
                } else {
                    new_memb_sel = FALSE;
                }

                if (!new_memb_sel
                    && (elem.replicas[mindex] > threshold)) {
                    /*
                     * Replace existing member with new member next-hop ID
                     * value.
                     */
                    if (cb) {
                        minfo.entry = (void *)lt_entry;
                        minfo.opc = RH_ADD;
                        minfo.mindex = new_mindex;
                        minfo.eindex = next_eindex;

                        SHR_IF_ERR_EXIT((cb)(unit, &minfo));
                    }

                    /*
                     * Replace existing member with the new member index
                     * value.
                     */
                    elem.mindex_arr[next_eindex] = new_mindex;

                    /*
                     * Decrement entry usage count value for the replaced
                     * member.
                     */
                    elem.replicas[mindex] -= 1;

                    /* Increment entry usage count value for the new member. */
                    *ecount += 1;
                    break;
                } else {
                    next_eindex = (next_eindex + 1) % elem.entry_max;
                }
            }
            /* In order to search index in range of lower bound. */
            if (next_eindex == eindex) {
                threshold--;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_rh_elem_member_delete(int unit,
                              uint32_t del_mindex,
                              void *entry,
                              bcmecmp_rh_memb_fn cb)
{
    uint32_t eindex; /* Index iterator variable. */
    uint32_t selected_index = (uint32_t)BCMECMP_INVALID; /* Selected member
                                                            index. */
    bcmecmp_id_t res_id; /* Resource identifier. */
    bcmecmp_rh_elem_t elem;
    bcmecmp_lt_entry_t *lt_entry = NULL;
    rh_memb_info_t minfo; /* RH member info data. */

    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_rh_grp_info_t *rh_group = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    sal_memset(&minfo, 0, sizeof(minfo));

    lt_entry = (bcmecmp_lt_entry_t *)entry;

    SHR_IF_ERR_EXIT
        (rh_entry_elem_convert(unit, lt_entry, &elem));

    res_id = elem.res_id;
    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    rh_group = ecmp_inf->rhg[res_id];

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_memb_delete: gid=%d mindex=%u.\n"),
                  res_id, del_mindex));

    for (eindex = 0; eindex < elem.entry_max; eindex++) {
        /*
         * If current member is not the member to be deleted, continue to the
         * next member.
         */
        if (rh_group->rh_mindex_arr[eindex] != del_mindex) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (rh_memb_rebalance(unit,
                               &elem,
                               &selected_index));

        /*
         * Replace deletion member with existing member next-hop ID
         * value.
         */
        if (cb) {
            minfo.entry = (void *)lt_entry;
            minfo.opc = RH_DEL;
            minfo.mindex = selected_index;
            minfo.eindex = eindex;
            minfo.arg = del_mindex;

            SHR_IF_ERR_EXIT((cb)(unit, &minfo));
        }

        /*
         * Delay this update until all members are deleted so that mindex array
         * has old mindex member value for multi member delete operation.
         */
        elem.mindex_arr[eindex] = selected_index;
    }

exit:
    SHR_FUNC_EXIT();
}
