/*! \file bcmecmp_group.c
 *
 * This file contains ECMP group management functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <sal/sal_types.h>
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmecmp/bcmecmp_rh_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Resilient hash ECMP group data print debug function.
 *
 * Resilient hash ECMP group data print debug function.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_id Resilient hash group identifier.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
rh_grp_data_debug_print(int unit,
                        char *str,
                        bcmecmp_id_t ecmp_id,
                        bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_rh_grp_info_t *rh_group = NULL;
    bcmecmp_lt_entry_t *cur_entry = NULL;
    uint32_t grp_npaths = 0;

    uint32_t idx; /* Index iterator variable. */
    shr_pb_t *pb = NULL; /* Print buffer. */
    bool rh_ov = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMECMP_GRP_RHG_PTR(unit, ecmp_id), SHR_E_INTERNAL);

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    rh_group = ecmp_inf->rhg[lt_entry->ecmp_id];
    cur_entry = ecmp_inf->current_lt_ent;

    if ((bcmecmp_grp_is_overlay(lt_entry->grp_type) &&
        ecmp_inf->ovlay_memb_count)) {
        rh_ov = true;
    }

    pb = shr_pb_create();
    if (str) {
        shr_pb_printf(pb, "%s\n", str);
    } else {
        shr_pb_printf(pb, "\n");
    }

    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    shr_pb_printf(pb,
                  "\tECMP_ID=%-4d num_path=%-4d members[rh_entries_cnt=%-4d]:\n",
                  ecmp_id,
                  grp_npaths,
                  lt_entry->rh_entries_cnt);
    shr_pb_printf
        (pb, "\tFORMAT {eindex: rh_entries_arr[eindex] / mindex}");
    for (idx = 0; idx < lt_entry->rh_entries_cnt; idx++) {
        if (!(idx % 3)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      "%4d: %5d / %-4d",
                      idx,
                      lt_entry->rh_entries_arr[idx],
                      lt_entry->rh_mindex_arr[idx]);
    }
    shr_pb_printf(pb, "\n\n");
    shr_pb_printf
        (pb, "\t %s[npaths=%-4d sort=%s]:\n",
         rh_ov ? "rh overlay entry" : "rh underlay entry",
         lt_entry->rh_num_paths,
         lt_entry->nhop_sorted ? "true" : "false");
    shr_pb_printf
        (pb, "\t=====================================\n");
    for (idx = 0; idx < lt_entry->rh_num_paths; idx++) {
        if (rh_ov) {
        shr_pb_printf
            (pb,
             "\t%4d) [NHOP_ID=Cur:%-5d New:%-5d / "
             "Replicas=Cur:%-4u New:%-4u]\n",
             idx,
             cur_entry->u_ecmp_nhop[idx] ?
                cur_entry->uecmp_id[idx] : cur_entry->nhop_id[idx],
             lt_entry->u_ecmp_nhop[idx] ?
                 lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx],
             rh_group->rh_memb_ecnt_arr[idx],
             lt_entry->rh_memb_ecnt_arr[idx]);
        } else {
        shr_pb_printf
            (pb,
             "\t%4d) [NHOP_ID=Cur:%-5d New:%-5d / "
             "Replicas=Cur:%-4u New:%-4u]\n",
             idx,
             BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) ?
                BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[idx]
                    : BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[idx],
             lt_entry->ecmp_nhop
                ? lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx],
             BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx],
             lt_entry->rh_memb_ecnt_arr[idx]);
        }
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    exit:
        SHR_FUNC_EXIT();
}

static bool
rh_is_ov_group_member(int unit, uint32_t gtype)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;

    if ((bcmecmp_grp_is_overlay(gtype) &&
        ecmp_inf->ovlay_memb_count)) {
        return true;
    }

    return false;
}

/* Specify the index of destination path is used to RH group member of entry. */
static int
rh_group_member_set(int unit,
                    bcmecmp_lt_entry_t *lt_entry,
                    rh_op_e opc,
                    uint32_t mindex,
                    uint32_t eindex,
                    uint32_t arg)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_rh_grp_info_t *rh_group = NULL;
    bcmecmp_lt_entry_t *cur_entry = NULL;
    bool rh_is_ov = false;
    char *rh_ov_opc_str[] = {"RH_INIT", "RH_ADD", "RH_DEL"};

    SHR_FUNC_ENTER(unit);

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    rh_group = ecmp_inf->rhg[lt_entry->ecmp_id];
    cur_entry = ecmp_inf->current_lt_ent;

    rh_is_ov = rh_is_ov_group_member(unit, lt_entry->grp_type);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n%s opc=%s "
                            "mindex=%d eindex=%d\n"),
                            rh_is_ov ? "RH_OVERLAY":
                                       "RH_UNDERLAY",
                            rh_ov_opc_str[opc], mindex, eindex));

    /*
     * If group type of OVERLAY is applicated with
     * various kind of group member types.
     */
    if (rh_is_ov) {
        /* Underlay group member is assigned to ECMP group or NHOP. */
        lt_entry->rh_entries_arr[eindex] = \
            (lt_entry->u_ecmp_nhop[mindex]) ?
                lt_entry->uecmp_id[mindex] :
                lt_entry->nhop_id[mindex];

        /*
         * Indicator to underlay group member of ECMP group from NHOP.
         * ECMP group is true, else is NHOP.
         */
        lt_entry->rh_u_ecmp_nhop_arr[eindex] = \
            lt_entry->u_ecmp_nhop[mindex];

        /* Overlay group member is assigned to NHOP. */
        lt_entry->rh_o_entries_arr[eindex] = \
            lt_entry->o_nhop_id[mindex];

        /* Deletion update current entry. */
        if (opc == RH_DEL) {
            /* Decrement deleted members entry usage count value.  */
            rh_group->rh_memb_ecnt_arr[arg] -= 1;

            /* Remove deletion index from current entry. */
            if (cur_entry->u_ecmp_nhop[arg]) {
                cur_entry->uecmp_id[arg] = BCMECMP_INVALID;
            } else {
                cur_entry->o_nhop_id[arg] = BCMECMP_INVALID;
            }
        }
    } else {
        /* Select Underlay ECMP group or NHOP to be assigned to group member. */
        lt_entry->rh_entries_arr[eindex] = \
            (lt_entry->ecmp_nhop) ? lt_entry->uecmp_id[mindex] :
                                    lt_entry->nhop_id[mindex];

        if (opc == RH_DEL) {
            /* Decrement deleted members entry usage count value.  */
            rh_group->rh_memb_ecnt_arr[arg] -= 1;

            /*
             * If all the entries have been replaced, update next-hop value as
             * invalid.
             */
            if (!rh_group->rh_memb_ecnt_arr[arg]) {
                if (cur_entry->ecmp_nhop) {
                    cur_entry->uecmp_id[arg] = BCMECMP_INVALID;
                } else {
                    cur_entry->nhop_id[arg] = BCMECMP_INVALID;
                }
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief  This function is registered for RH group member handler.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
rh_group_member_cb(int unit, void *param)
{
    rh_memb_info_t *minfo = NULL;
    bcmecmp_lt_entry_t *lt_entry = NULL;

    SHR_FUNC_ENTER(unit);

    if (param) {
        minfo = (rh_memb_info_t *)param;
        lt_entry = (bcmecmp_lt_entry_t *)minfo->entry;
        if (lt_entry == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Destination path assigned to associated group member. */
        SHR_IF_ERR_EXIT
            (rh_group_member_set(unit,
                                 lt_entry,
                                 minfo->opc,
                                 minfo->mindex,
                                 minfo->eindex,
                                 minfo->arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/* Extract the existing overlay group member information to current entry. */
static void
rh_ov_group_memb_info_extract(int unit,
                              bool use_cnt,
                              uint32_t grp_npaths,
                              bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_rh_grp_info_t *info = NULL;

    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Pointer to rh group member cache. */
    info = ecmp_inf->rhg[ecmp_id];

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               info->rh_entries_arr,
               (sizeof(*(lt_entry->rh_entries_arr)) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               info->rh_mindex_arr,
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries usage count array into LT entry's RH
     * entries usage count array for update operation.
     */
    if (use_cnt) {
        sal_memcpy(lt_entry->rh_memb_ecnt_arr,
                   info->rh_memb_ecnt_arr,
                   (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * grp_npaths));
    }

    /*
     * For update operation, copy existing overlay group RH entries array
     * into LT entry's overlay RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_o_entries_arr,
               info->rh_o_entries_arr,
               (sizeof(*(lt_entry->rh_o_entries_arr)) *
                lt_entry->rh_entries_cnt));

    sal_memcpy(lt_entry->rh_u_ecmp_nhop_arr,
               info->rh_u_ecmp_nhop_arr,
               (sizeof(*(lt_entry->rh_u_ecmp_nhop_arr)) *
                lt_entry->rh_entries_cnt));
}

static int
rh_ov_group_member_assign(int unit,
                          bcmecmp_lt_entry_t *lt_entry,
                          uint32_t idx,
                          uint32_t uidx)
{
    int entry_memb_id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_INTERNAL);

    entry_memb_id = \
        (lt_entry->u_ecmp_nhop[idx]) ?
            lt_entry->uecmp_id[idx] :
            lt_entry->nhop_id[idx];

    lt_entry->rh_entries_arr[uidx] = entry_memb_id;

    lt_entry->rh_u_ecmp_nhop_arr[uidx] = \
        lt_entry->u_ecmp_nhop[idx];

    lt_entry->rh_o_entries_arr[uidx] = \
        lt_entry->o_nhop_id[idx];

exit:
    SHR_FUNC_EXIT();
}

/* Compare existing group members with members in entry updatetion. */
static bool
rh_ov_group_member_compare(int unit,
                           bcmecmp_lt_entry_t *lt_entry,
                           uint32_t idx,
                           uint32_t uidx,
                           bool rh_entry)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_lt_entry_t *cur_entry = NULL;
    int cur_memb_id = -1, entry_memb_id = -1;
    bool rv = false;

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    cur_entry = ecmp_inf->current_lt_ent;

    cur_memb_id = \
        (cur_entry->u_ecmp_nhop[idx]) ?
            cur_entry->uecmp_id[idx] :
            cur_entry->nhop_id[idx];

    entry_memb_id = \
        (lt_entry->u_ecmp_nhop[uidx]) ?
            lt_entry->uecmp_id[uidx] :
            lt_entry->nhop_id[uidx];

    if (rh_entry) {
        if ((cur_memb_id == lt_entry->rh_entries_arr[uidx]) &&
            (cur_entry->u_ecmp_nhop[idx] ==
             lt_entry->rh_u_ecmp_nhop_arr[uidx]) &&
            (cur_entry->o_nhop_id[idx] ==
             lt_entry->rh_o_entries_arr[uidx])) {
            rv = true;
        }
    } else {
        if ((cur_memb_id == entry_memb_id) &&
            (cur_entry->u_ecmp_nhop[idx] == lt_entry->u_ecmp_nhop[uidx]) &&
            (cur_entry->o_nhop_id[idx] == lt_entry->o_nhop_id[uidx])) {
            rv = true;
        }
    }

    return (rv);
}

static int
rh_ov_group_member_add(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t new_mindex; /* New member array index. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /* Extract overlay group member information. */
    rh_ov_group_memb_info_extract(unit, true, grp_npaths, lt_entry);

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "PREV rh ov data add",
                                        ecmp_id, lt_entry)));

    /*
     * Call RH member add function for every new member in the
     * next-hop array. New members must be set in the array after the
     * current next-hop member elements.
     */
    for (new_mindex = grp_npaths;
         new_mindex < lt_entry->rh_num_paths;
         new_mindex++) {
        SHR_IF_ERR_EXIT
            (bcmecmp_rh_elem_member_add(unit,
                                        new_mindex,
                                        lt_entry,
                                        rh_group_member_cb));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "POST rh ov data add",
                                        ecmp_id, lt_entry)));

exit:
    SHR_FUNC_EXIT();
}

static int
rh_ov_group_member_delete(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted members indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t del_idx = 0; /* Deleted members array index iterator. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    ecmp_id = lt_entry->ecmp_id;

    /*
     * Check if num_paths is non-zero in this delete (update) operation. If
     * there are valid members after delete, the remaining members must be
     * re-balanced.
     *
     * If there no members left in the group after this delete operation,
     * re-balance operation is not required. Clear the entries in the member
     * table, clear the mindex value for all entries in this group, reset
     * entries usage count and return.
     */
    if (lt_entry->rh_num_paths) {
        grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        /* Determine how many members have been deleted. */
        deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

        /* Extract overlay group member information. */
        rh_ov_group_memb_info_extract(unit, false, grp_npaths, lt_entry);

        BCMECMP_ALLOC(deleted_mindex,
                      deleted_cnt * sizeof(*deleted_mindex),
                      "bcmecmpRhGrpMembsDelMindexArr");

        BCMECMP_ALLOC(shrd_mindex,
                      (grp_npaths * sizeof(*shrd_mindex)),
                      "bcmecmpRhGrpMembsDelShrdMindexArr");
        sal_memset(shrd_mindex,
                   BCMECMP_INVALID,
                   (grp_npaths * sizeof(*shrd_mindex)));

        /*
         * The set of new group members are a sub-set of the current members
         * list as some of the existing members have been deleted from the list
         * as part of update operation.
         *
         * So, for every member in the existing members array (outer for-loop),
         * check if this member exists in the updated members array
         * (inner for-loop). If it's not present, store this member index in the
         * deleted member indices array.
         */
        for (idx = 0; idx < grp_npaths; idx++) {
            /*
             * num_paths value must be less than grp_npaths as it's member/s
             * delete operation and number of elements in umemb_arr is equal to
             * num_paths.
             */
            for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
                /*
                 * Compare existing nhop members with members in updated members
                 * array, confirm rh_memb_ecnt_arr value is equal to zero for
                 * the updated nhop member to skip duplicate members.
                 */
                if (rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               false)
                    && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                    lt_entry->rh_memb_ecnt_arr[uidx] =
                            BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                    shrd_cnt++;
                    if (idx != uidx) {
                        /*
                         * Save new mindex mapped to the old mindex value for
                         * update after the delete operation.
                         */
                        shrd_mindex[idx] = uidx;
                        shrd_new_mindex_cnt++;

                        /*
                         * Reset the entry usage counter value as this member
                         * will be maintained in the new index location "uidx"
                         * after the delete operation.
                         */
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                    }
                    break;
                }
            }

            /*
             * If the member is present in the updated nhop members array, move
             * to the next member in existing array.
             */
            if (uidx != lt_entry->rh_num_paths) {
                continue;
            }

            /*
             * Member not found in the updated array, add this member index to
             * be deleted members list.
             */
            if (del_idx < deleted_cnt) {
                deleted_mindex[del_idx++] = idx;
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "del_idx = %u > del_max_idx = %u.\n"),
                            del_idx,
                            (deleted_cnt - 1)));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }

        for (idx = 0; idx < deleted_cnt; idx++) {
            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        rh_grp_data_debug_print(unit, "PREV rh data delete",
                                                ecmp_id, lt_entry)));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_elem_member_delete(unit,
                                               deleted_mindex[idx],
                                               lt_entry,
                                               rh_group_member_cb));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        rh_grp_data_debug_print(unit, "POST rh data delete",
                                                ecmp_id, lt_entry)));
        }

        /*
         * For the shared members, check and update the member index value in
         * the LT entry mindex array if it has changed.
         */
        if (shrd_new_mindex_cnt) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n\t[Shared mindex update]\n");
            shr_pb_printf(pb, "\t======================\n");
            for (idx = 0; idx < grp_npaths; idx++) {
                new_idx = shrd_mindex[idx];
                if (new_idx == (uint32_t)BCMECMP_INVALID) {
                    continue;
                }
                for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                    if (rh_ov_group_member_compare(unit,
                                                   lt_entry,
                                                   idx,
                                                   uidx,
                                                   true)
                        && idx == lt_entry->rh_mindex_arr[uidx]) {
                        shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=]"
                                      "[n_mindex=%u / n_nhop=%-5d]\n",
                                      idx,
                                      new_idx,
                                      lt_entry->rh_entries_arr[uidx]);

                        /* Update mindex with new mindex value. */
                        lt_entry->rh_mindex_arr[uidx] = new_idx;
                    }
                }
                shrd_new_mindex_cnt--;
            }
            shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                          shrd_new_mindex_cnt);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    } else {
        /* Clear RH-ECMP group member table entries. */
        sal_memset(lt_entry->rh_entries_arr,
                   0,
                   (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group member indices data. */
        sal_memset(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group members entry usage count data. */
        sal_memset(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    BCMECMP_LT_MAX_RH_NPATHS(unit, group, lt_entry->grp_type)));
    }

    exit:
        if (lt_entry->rh_num_paths) {
            /* Free memory allocated to store mindex array. */
            BCMECMP_FREE(deleted_mindex);
            BCMECMP_FREE(shrd_mindex);
        }
        SHR_FUNC_EXIT();
}

static int
rh_ov_group_member_replace(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t idx, uidx;       /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id;     /* ECMP group identifier. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    ecmp_id = lt_entry->ecmp_id;

    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For replace operation, update num_paths must be equal to group's current
     * num_paths value.
     */
    if (grp_npaths - lt_entry->rh_num_paths) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Extract overlay group member information. */
    rh_ov_group_memb_info_extract(unit, true, grp_npaths, lt_entry);

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "PREV rh replace",
                                        ecmp_id, lt_entry)));

    /*
     * Find and replace the existing nhop member with the new nhop member
     * ID value.
     */
    for (idx = 0; idx < lt_entry->rh_num_paths; idx++) {
        if (rh_ov_group_member_compare(unit,
                                       lt_entry,
                                       idx,
                                       idx,
                                       false) == false) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n");
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if ((rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               true) == true)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    SHR_IF_ERR_EXIT
                        (rh_ov_group_member_assign(unit, lt_entry, idx, uidx));
                    shr_pb_printf(pb, "\t%5d) o_nhop =  n_nhop=%-5d\n",
                                  uidx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
            shr_pb_printf(pb, "\n");
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    }

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "POST rh replace",
                                        ecmp_id, lt_entry)));

    exit:
        SHR_FUNC_EXIT();
}

static int
rh_ov_group_member_add_replace(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t added_cnt = 0; /* Number of members added to the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_mindex_cnt = 0; /* Count of shared members with new mindex. */
    uint32_t rep_idx = 0; /* Added member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "rh_ov_group_member_add_replace.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /* Extract overlay group member information. */
    rh_ov_group_memb_info_extract(unit, false, grp_npaths, lt_entry);

    BCMECMP_ALLOC(replace_mindex,
                  (grp_npaths * sizeof(*replace_mindex)),
                  "bcmecmpRhGrpMembsAddRepReplaceMindexArr");
    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsAddRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Identify the members that are shared between the updated NHOP array and
     * the current NHOP members up to the group's current NUM_PATHS value.
     * Members in the updated array after current NUM_PATHS value will be
     * treated as member add operation.
     * For members up to grp_npaths that are not shared with the updated members
     * array will be candidates for replace
     * operation.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            if (rh_ov_group_member_compare(unit,
                                           lt_entry,
                                           idx,
                                           uidx,
                                           false)
                && (lt_entry->rh_memb_ecnt_arr[uidx] == 0)) {
                /* Update entry usage count. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];

                /* Increment shared entry count. */
                shrd_cnt++;

                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_mindex_cnt++;
                }

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * Existing member at current index is also part of the updated
         * next-hop members array, continue to the next member.
         */
        if (uidx != lt_entry->rh_num_paths) {
            continue;
        }

        /* Member not shared hence add to replace member elements list. */
        replace_mindex[replace_cnt++] = idx;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[grp_npaths=%u Shrd_cnt=%u Rep_cnt=%u]\n",
                  grp_npaths, shrd_cnt, replace_cnt);
    shr_pb_printf(pb, "\t======================================\n");

    if (replace_cnt) {
        BCMECMP_ALLOC(new_mindex,
                      replace_cnt * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsAddRepNewMindexArr");

        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {

            /* Get the old to be replaced index value. */
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < grp_npaths; uidx++) {

                if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                    /* Already updated, move to next member. */
                    continue;
                }

                /* Update entry count value. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Store the new mindex mapped to this old mindex for updating
                 * the mindex and rh_entries_arr arrays.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                          "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                           idx,
                           rep_idx,
                           uidx);
                break;
            }
        }
    }

    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               true)
                    && idx == lt_entry->rh_mindex_arr[uidx]) {

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                }
            }
            shrd_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                      shrd_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               true)
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    SHR_IF_ERR_EXIT
                        (rh_ov_group_member_assign(unit, lt_entry, new_idx, uidx));

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));

    for (new_idx = 0; new_idx < lt_entry->rh_num_paths;
         new_idx++) {
        if (lt_entry->rh_memb_ecnt_arr[new_idx]) {
            continue;
        }
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    rh_grp_data_debug_print(unit, "PREV rh data add",
                                            ecmp_id, lt_entry)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_elem_member_add(unit,
                                        new_idx,
                                        lt_entry,
                                        rh_group_member_cb));
        added_cnt++;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n\tadded_cnt=%u n_mindex=%u n_nhop=\n"),
                     added_cnt, new_idx));
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

static int
rh_ov_group_member_delete_replace(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    uint32_t grp_npaths = 0; /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted member/s indices. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t del_idx = 0; /* Deleted members array index. */
    uint32_t rep_idx = 0; /* Replace member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "rh_ov_group_member_delete_replace.\n")));

    /*
     * NUM_PATHS must be non-zero when this function is called for member delete
     * operation, check and return if this condition is not met.
     */
    if (!lt_entry->rh_num_paths) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the group's ECMP_ID value and current NUM_PATHS values. */
    ecmp_id = lt_entry->ecmp_id;
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Determine how many members have been deleted based on the updated
     * NUM_PATHS value.
     */
    deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

    /* Extract overlay group member information. */
    rh_ov_group_memb_info_extract(unit, false, grp_npaths, lt_entry);

    BCMECMP_ALLOC(deleted_mindex,
                  deleted_cnt * sizeof(*deleted_mindex),
                  "bcmecmpRhGrpMembsDelRepDelMindexArr");

    BCMECMP_ALLOC(replace_mindex,
                  (lt_entry->rh_num_paths) * sizeof(*replace_mindex),
                  "bcmecmpRhGrpMembsDelRepReplaceMindexArr");

    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsDelRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Order of NHOP members in updated NHOP array is not same as the stored
     * NHOP members array and some of the members in the updated array might be
     * shared with the stored members. Identify these shared members, deleted
     * members and members to be replaced.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            /*
             * Identify the common members and copy the stored member entry
             * usage count value to LT entry usage count element at the new
             * member index for delete and rebalance operation.
             */
            if (rh_ov_group_member_compare(unit,
                                           lt_entry,
                                           idx,
                                           uidx,
                                           false)
                && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                shrd_cnt++;
                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_new_mindex_cnt++;
                    /*
                     * Reset the entry usage counter value as this member
                     * will be maintained in the new index location "uidx"
                     * after the delete operation.
                     */
                }
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * If the member is a shared member, then continue to the next member
         * in the list.
         */
        if (uidx != lt_entry->rh_num_paths) {
            continue;
        }

        /*
         * Member not found in the updated array, add this old member index
         * value be the deleted members array.
         */
        if (del_idx < deleted_cnt) {
            deleted_mindex[del_idx++] = idx;
        } else {
            /*
             * If this member is not a shared member and members delete count
             * has already been reached, then save this member for replacing
             * with a new member in the updated members array.
             */
            replace_mindex[replace_cnt++] = idx;
        }
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[Shrd_cnt=%u / Shrd_n_mindex_cnt=%u Del_cnt=%u"
                      " Rep_cnt=%u]\n",
                  shrd_cnt,
                  shrd_new_mindex_cnt,
                  deleted_cnt,
                  replace_cnt);
    shr_pb_printf(pb, "\t=================================================\n");

    /*
     * If there are members to be replaced, identify the members from the
     * updated NHOP members array that will be replacing the current members.
     * Save the new member index value that is replacing a current member and
     * also assign the current members entry usage count value to the new
     * member in LT entry array for use during delete member rebalance
     * calculations.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");
        BCMECMP_ALLOC(new_mindex,
                      (replace_cnt) * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsNewMindexArr");
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            /*
             * Shared member entry count is already found, skip to next
             * member.
             */
            if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                continue;
            }
            for (idx = 0; idx < replace_cnt; idx++) {
                rep_idx = replace_mindex[idx];
                if (!BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx]) {
                    /*
                     * This replacement member candidate has already been
                     * associate with a new member from the updated array, move
                     * on to the next member in the replace mindex array.
                     */
                    continue;
                }
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Save the new member index value that is replacing the current
                 * replacement member.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                              "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                              idx,
                              rep_idx,
                              uidx);
                break;

            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

    /* Perform members delete and rebalance operation. */
    for (idx = 0; idx < deleted_cnt; idx++) {
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    rh_grp_data_debug_print(unit, "PREV rh data delete",
                                            ecmp_id, lt_entry)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_elem_member_delete(unit,
                                           deleted_mindex[idx],
                                           lt_entry,
                                           rh_group_member_cb));
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_new_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               true)
                    && idx == lt_entry->rh_mindex_arr[uidx]) {
                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
            shrd_new_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                      shrd_new_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");
        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               rep_idx,
                                               uidx,
                                               true)
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    SHR_IF_ERR_EXIT
                        (rh_ov_group_member_assign(unit, lt_entry, new_idx, uidx));

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(deleted_mindex);
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

static int
rh_ov_group_member_oper_get(int unit,
                            bcmecmp_lt_entry_t *lt_entry,
                            bcmecmp_rh_oper_t *rh_oper)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths; /* Group current NUM_PATHS value. */
    uint32_t idx = 0, uidx = 0; /* Next-hop members iterator variables. */
    uint32_t shared_nhop_cnt = 0; /* Shared NHOPs count. */
    uint32_t new_nhop_cnt = 0; /* New NHOPs count. */
    uint32_t alloc_sz = 0; /* Alloc size current and new. */
    int *nhop_arr = NULL; /* NHOP members array. */
    bool mindex_changed = FALSE; /* Member index has changed in NHOP array. */
    const char * const rh_op_str[BCMECMP_RH_OPER_COUNT] = BCMECMP_RH_OPER_NAME;
    int start_idx = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rh_oper, SHR_E_PARAM);

    COMPILER_REFERENCE(ecmp_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "rh_ov_group_member_oper_get.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /*
     * If there are no members in the RH-ECMP group, then there is no SW load
     * balancing operation to performed for this group in the UPDATE operation.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 &&
        lt_entry->rh_num_paths == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "(RH_NUM_PATHS = %u): BCMECMP_RH_OPER_NONE.\n"),
                    lt_entry->rh_num_paths));

        *rh_oper = BCMECMP_RH_OPER_NONE;
        SHR_EXIT();
    }

    /*
     * This UPDATE operation is equivalent to members add via an INSERT
     * operation as all the members are newly added to this group. These new
     * members must be SW load balanced by using bcmecmp_group_rh_entries_set()
     * function and not expected to be handled in this function, return
     * SHR_E_INTERNAL error.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 &&
        lt_entry->rh_num_paths > 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members to members (RH_NUM_PATHS = %u) rv=%s"
                                ".\n"),
                     lt_entry->rh_num_paths,
                     shr_errmsg(SHR_E_INTERNAL)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * If the RH-ECMP group previously had members and there are no members
     * in the group as part of this UPDATE operation, then set 'rh_oper' to
     * BCMECMP_RH_OPER_DELETE and return from this function. There are no
     * shared members between the old and new members for this group.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) > 0 &&
        lt_entry->rh_num_paths == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "(RH_NUM_PATHS = %u): BCMECMP_RH_OPER_DELETE.\n"),
                    lt_entry->rh_num_paths));

        *rh_oper = BCMECMP_RH_OPER_DELETE;
        SHR_EXIT();
    }

    /* Validate NUM_PATHS value before proceeding further in this function. */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 ||
        lt_entry->rh_num_paths == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unexpected RH_NUM_PATHS value: GNUM_PATHS=%u"
                                " RH_NUM_PATHS=%u.\n"),
                     BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                     lt_entry->rh_num_paths));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    alloc_sz = sizeof(*nhop_arr) * grp_npaths;

    if (alloc_sz) {
        BCMECMP_ALLOC(nhop_arr,
                      alloc_sz,
                      "bcmecmpRhOperGetNhopArr");
    }

    if (nhop_arr) {
        /*
         * Identify the shared members and the new members in the updated
         * next-hop members array.
         */
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            for (idx = start_idx; idx < grp_npaths; idx++) {
                if ((nhop_arr[idx] != BCMECMP_INVALID) &&
                    rh_ov_group_member_compare(unit,
                                               lt_entry,
                                               idx,
                                               uidx,
                                               false)) {
                    /* Old NHOP also member of the updated NHOP array. */
                    shared_nhop_cnt++;
                    nhop_arr[idx] = BCMECMP_INVALID;
                    /*
                     * Check if member index has changed in the updated
                     * members array.
                     */
                    if (idx != uidx) {
                        mindex_changed = TRUE;
                    }
                    break;
                }
            }  /* inter loop */

            if (idx == grp_npaths) {
                new_nhop_cnt++;
            }
            while (nhop_arr[start_idx] == BCMECMP_INVALID) {
                start_idx++;
            }
        } /* outer loop */
    }

    /* Determine the type of operation to be performed on RH group members. */
    if (lt_entry->rh_num_paths > grp_npaths) {
        if (shared_nhop_cnt == grp_npaths && !mindex_changed) {
            *rh_oper = BCMECMP_RH_OPER_ADD;
        } else {
            *rh_oper = BCMECMP_RH_OPER_ADD_REPLACE;
        }
    } else if (lt_entry->rh_num_paths < grp_npaths) {
        if (shared_nhop_cnt == lt_entry->rh_num_paths) {
            /*
             * This condition is TRUE when all the members passed in the
             * member/s delete-update operation are already members of this
             * group (Their mindex could have changed but they are still common
             * members between the NHOP members list).
             * This condition is also TRUE when all the members have been
             * deleted in the update operation i.e. 'lt_entry->num_paths == 0',
             * means there are no shared members in the group in this delete
             * operation. As shared_nhop_cnt default value is initialized to '0'
             * at the start of this function, This condition will be TRUE.
             */
            *rh_oper = BCMECMP_RH_OPER_DELETE;
        } else {
            *rh_oper = BCMECMP_RH_OPER_DELETE_REPLACE;
        }
    } else if (lt_entry->rh_num_paths == grp_npaths) {
        *rh_oper = BCMECMP_RH_OPER_REPLACE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t[grp_npaths=%-4u rh_num_paths=%-4u"
                            "shared_nhop_cnt=%-4u\n"
                            "\t new_nhop_cnt=%-4u mindex_changed=%s "
                            "rh_oper=(%s)]\n"),
                 grp_npaths,
                 lt_entry->rh_num_paths,
                 shared_nhop_cnt,
                 new_nhop_cnt,
                 mindex_changed ? "TRUE" : "FALSE",
                 rh_op_str[*rh_oper]));

    exit:
        BCMECMP_FREE(nhop_arr);
        SHR_FUNC_EXIT();
}

static int
rh_group_member_balance_init(int unit,
                             bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t idx;              /* loop iterator variable. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /*
     * Validate NUM_PATHS value and confirm it's non-zero to perform RH load
     * balancing. If value is zero, there are no members to perform load
     * balancing.
     */
    if (!lt_entry->rh_num_paths) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members RH_NUM_PATHS=%u.\n"),
                     lt_entry->rh_num_paths));
        SHR_EXIT();
    }

    if (lt_entry->rh_num_paths && !lt_entry->rh_entries_arr) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NULL RH_ENTRIES_ARR, RH_NUM_PATHS=%u.\n"),
                     lt_entry->rh_num_paths));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (idx = 0; idx < lt_entry->rh_entries_cnt; idx++) {
        SHR_IF_ERR_EXIT
            (bcmecmp_rh_elem_member_init(unit,
                                         idx,
                                         lt_entry,
                                         rh_group_member_cb));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reconstruct overlay RH-ECMP group mebers load balanced state.
 *
 * This function reconstructs RH-ECMP group members load balanced state post
 * warm start sequence to support subsequent members update operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
rh_ov_group_members_recover(int unit,
                            bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_lt_entry_t *rhg_entry = NULL; /* Resilient hash group entry. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    uint32_t idx = 0, memb_idx;      /* Index iterator variables. */
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);

    /*
     * Allocate memory for resilient hashing group entry for performing the get
     * operation.
     */
    BCMECMP_ALLOC(rhg_entry, sizeof(*rhg_entry), "bcmecmpBcmRhgEntry");
    BCMECMP_ALLOC(rhg_entry->rh_entries_arr,
                  sizeof(*(rhg_entry->rh_entries_arr))
                  * lt_entry->rh_entries_cnt, "bcmecmpBcmRhgRhEntArr");
    BCMECMP_ALLOC
        (rhg_entry->rh_o_entries_arr, lt_entry->rh_entries_cnt *
         sizeof(*(rhg_entry->rh_o_entries_arr)),
         "bcmecmpRhGrpAddLtEntRhOvlayEntArr");
    BCMECMP_ALLOC
        (rhg_entry->rh_u_ecmp_nhop_arr, lt_entry->rh_entries_cnt *
         sizeof(*(rhg_entry->rh_u_ecmp_nhop_arr)),
         "bcmecmpRhGrpAddLtEntRhUnlayEcmpNhopArr");
    /*
     * Setup input attribute values necessary for reading the RH group hardware
     * table entries.
     */
    rhg_entry->glt_sid = lt_entry->glt_sid;
    rhg_entry->op_arg = lt_entry->op_arg;
    rhg_entry->grp_type = lt_entry->grp_type;
    rhg_entry->ecmp_id = lt_entry->ecmp_id;
    ecmp_id = rhg_entry->ecmp_id;

    /* Get RH-ECMP Group entry from hardware tables. */
    SHR_IF_ERR_EXIT
        (BCMECMP_FNCALL_EXEC(unit, rh_grp_get)(unit, rhg_entry));

    lbound = (BCMECMP_GRP_RH_ENTRIES_CNT
                    (unit, ecmp_id) / BCMECMP_GRP_NUM_PATHS(unit, ecmp_id));
    ubound = ((BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) %
                    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id))
                    ? (lbound + 1) : lbound);
    threshold = ubound;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\tLBND=%u UBND=%u THRSLD=%u LB_MODE=%u "
                            "rh_entries_cnt=%u mstart_idx=%d\n"),
                lbound,
                ubound,
                threshold,
                rhg_entry->lb_mode,
                rhg_entry->rh_entries_cnt,
                rhg_entry->mstart_idx));

    /*
     * Verify retrieved total RH group member table entries count matches
     * stored/expected value before further processing for this RH group.
     */
    if (rhg_entry->rh_entries_cnt
        != BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for RHG members structure. */
    BCMECMP_ALLOC(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                  sizeof(*BCMECMP_GRP_RHG_PTR(unit, ecmp_id)),
                  "bcmecmpRhGrpMembRcvrRhgInfo");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id),
                      sizeof(*BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)),
                      "bcmecmpRhGrpMembRcvrRhgInfoBk");
    }

    /* Allocate memory to store RHG member table entries. */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                  (rhg_entry->rh_entries_cnt *
                   sizeof(*BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))),
                  "bcmecmpRhGrpMembRcvrRhEntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                      (rhg_entry->rh_entries_cnt *
                       sizeof(*BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))),
                      "bcmecmpRhGrpMembRcvrRhEntArrBk");
    }

    /* Copy load balanced RH members array. */
    sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               rhg_entry->rh_entries_arr,
               sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
               rhg_entry->rh_entries_cnt);
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                   rhg_entry->rh_entries_arr,
                   sizeof(*(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))) *
                   rhg_entry->rh_entries_cnt);
    }

    /* Allocate memory to store RH-ECMP group entries. */
    BCMECMP_ALLOC
        (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
         rhg_entry->rh_entries_cnt * sizeof(int),
         "bcmecmpRhGrpAddGrpRhOvlayEntArr");
    BCMECMP_ALLOC
        (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
         rhg_entry->rh_entries_cnt * sizeof(bool),
         "bcmecmpRhGrpAddGrpRhUnlayEntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC
            (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
             rhg_entry->rh_entries_cnt * sizeof(int),
             "bcmecmpRhGrpAddGrpRhOvlayEntArrBk");
        BCMECMP_ALLOC
            (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
             rhg_entry->rh_entries_cnt * sizeof(bool),
             "bcmecmpRhGrpAddGrpRhUnlayEntArrBk");
    }

    /* Copy load balanced RH members array. */
    sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
               rhg_entry->rh_o_entries_arr,
               rhg_entry->rh_entries_cnt * sizeof(int));
    sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
               rhg_entry->rh_u_ecmp_nhop_arr,
               rhg_entry->rh_entries_cnt * sizeof(bool));
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        sal_memcpy(ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
                   rhg_entry->rh_o_entries_arr,
                   rhg_entry->rh_entries_cnt * sizeof(int));
        sal_memcpy(ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
                   rhg_entry->rh_u_ecmp_nhop_arr,
                   rhg_entry->rh_entries_cnt * sizeof(bool));
    }

    /*
     * Allocate memory to store group RH-ECMP entries mapped member
     * index information.
     */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                  (rhg_entry->rh_entries_cnt *
                   sizeof(*(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMindexArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                      (rhg_entry->rh_entries_cnt *
                       sizeof(*(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMindexArrBk");
    }

    /*
     * Allocate memory to store RH-ECMP members entries usage count
     * information.
     */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                  (BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) *
                   sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMembEntCntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                      (BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) *
                       sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMembEntCntArrBk");
    }

    for (memb_idx = 0;
         memb_idx < BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->rh_entries_cnt; idx++) {
            if (rhg_entry->rh_entries_arr[idx] != BCMECMP_INVALID
                && rh_ov_group_member_compare
                       (unit, rhg_entry, memb_idx, idx, true)
                && BCMECMP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < lbound) {
                /* Update member index mapping */
                BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /*
                 * Update as invalid as it's been matched.
                 * Only set underlay rh entries.
                 */
                rhg_entry->rh_entries_arr[idx] = BCMECMP_INVALID;

                /* Move on to the next member as lower bound as been reached. */
                if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
                    == lbound) {
                    break;
                }
            }
        }
    }

    /*
     * For remaining members compare the upper limit and update the member index
     * and member entries usage count values.
     */
    for (memb_idx = 0;
         memb_idx < BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->rh_entries_cnt; idx++) {
            if (rhg_entry->rh_entries_arr[idx] != BCMECMP_INVALID
                && rh_ov_group_member_compare
                       (unit, rhg_entry, memb_idx, idx, true)
                && BCMECMP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < ubound) {
                /* Update member index mapping */
                BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /*
                 * Update as invalid as it's been matched.
                 * Only set underlay rh entries.
                 */
                rhg_entry->rh_entries_arr[idx] = BCMECMP_INVALID;

                /* Move on to the next member as upper bound as been reached. */
                if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
                    == ubound) {
                    break;
                }
            }
        }
    }

    if (BCMECMP_ATOMIC_TRANS(unit)) {
        /*
         * Copy group entries mapped member index array elements into the backup
         * array.
         */
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) *
                    rhg_entry->rh_entries_cnt));

        /*
         * Copy members entry usage count array elements into the backup
         * array.
         */
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) *
                    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id)));
    }

    

    exit:
        if (rhg_entry) {
            BCMECMP_FREE(rhg_entry->rh_entries_arr);
            BCMECMP_FREE(rhg_entry->rh_o_entries_arr);
            BCMECMP_FREE(rhg_entry->rh_u_ecmp_nhop_arr);
            BCMECMP_FREE(rhg_entry);
        }
        if (SHR_FUNC_ERR() && ecmp_id != BCMECMP_INVALID) {
            if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));
                BCMECMP_FREE(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr);
                BCMECMP_FREE(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr);
                BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
            }

            if (BCMECMP_ATOMIC_TRANS(unit)
                && BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr);
                BCMECMP_FREE(ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr);
                BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
            }
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Initalize bcmecmp_grp_defrag_t structure members.
 *
 * \param [in] frag Pointer to bcmecmp_grp_defrag_t structure.
 *
 * \return Nothing.
 */
static void
bcmecmp_grp_defrag_t_init(bcmecmp_grp_defrag_t *frag)
{
    if (frag) {
        frag->oper_flags = 0;
        frag->glt_sid = BCMLTD_SID_INVALID;
        frag->ecmp_id = BCMECMP_INVALID;
        frag->gtype = BCMECMP_GRP_TYPE_COUNT;
        frag->mstart_idx = BCMECMP_INVALID;
        frag->gsize = 0;
        frag->op_arg = NULL;
    }
    return;
}

static int
bcmecmp_member_weight_free_idx_get(int unit, bcmecmp_tbl_op_t *op_data)
{
    int imin, imax, start_idx, idx, num_idx;
    uint32_t oper_flags;
    bcmecmp_tbl_prop_t *tbl_ptr;
    bcmecmp_id_t grp_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_data, SHR_E_PARAM);
    SHR_NULL_CHECK(op_data->tbl_ptr, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcmecmp_tbl_free_idx_get.\n")));

    tbl_ptr = op_data->tbl_ptr;
    imin = tbl_ptr->imin;
    imax = tbl_ptr->imax;
    num_idx = op_data->num_ent *
              BCMECMP_FNCALL_EXEC(unit, member_weight_index_width)();
    oper_flags = op_data->oper_flags;

    if (bcmecmp_grp_list_get_head(unit,
                                  BCMECMP_NON_MEMBER_WEIGHT_LIST(unit),
                                  &grp_id) == SHR_E_NONE) {
        if (BCMECMP_GRP_IS_WEIGHTED(BCMECMP_GRP_TYPE(unit, grp_id))) {
            imin = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_id) +
                   BCMECMP_GRP_MAX_PATHS(unit, grp_id);
        } else {
            if (BCMECMP_GRP_LB_MODE(unit, grp_id) ==
                BCMECMP_LB_MODE_RESILIENT) {
                imin = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_id) +
                       BCMECMP_GRP_RH_ENTRIES_CNT(unit, grp_id);
            } else {
                imin = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_id) +
                       BCMECMP_GRP_MAX_PATHS(unit, grp_id);
            }
        }
    }

    for (start_idx = imax; start_idx >= imin; start_idx--) {
        /* skip index zero if required by hardware. */
        if (!start_idx && (oper_flags & BCMECMP_OPER_SKIP_ZERO)) {
            continue;
        }

        if (!BCMECMP_TBL_REF_CNT(tbl_ptr, start_idx)) {
            for (idx = start_idx;
                 idx > (start_idx - num_idx) && (idx >= imin);
                 idx--) {
                if (BCMECMP_TBL_REF_CNT(tbl_ptr, idx)) {
                    break;
                }
            }
            if (start_idx == (idx + num_idx)) {
                op_data->free_idx = idx + 1;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "free_idx=%d\n"), op_data->free_idx));
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_FULL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Returns free indices range from requested hardware table.
 *
 * This function checks for a range of free hardware table indices and
 * returns the start index of the free index range in op_data free_idx
 * structure member variable.
 *
 * \param [in] unit Unit number.
 * \param [in] op_data Pointer to bcmecmp_tbl_op_t structure.
 * \param [in] min2max Find free index from min to max index.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 */
static int
bcmecmp_tbl_free_idx_get(int unit, bcmecmp_tbl_op_t *op_data, bool min2max)
{
    int imin, imax, start_idx, idx;
    uint32_t num_ent;
    uint32_t oper_flags;
    bcmecmp_tbl_prop_t *tbl_ptr;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_data, SHR_E_PARAM);
    SHR_NULL_CHECK(op_data->tbl_ptr, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcmecmp_tbl_free_idx_get.\n")));

    tbl_ptr = op_data->tbl_ptr;
    imin = tbl_ptr->imin;
    imax = tbl_ptr->imax;
    num_ent = op_data->num_ent;
    oper_flags = op_data->oper_flags;
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "drd_sid=%d ltd_sid=%d imin=%d imax=%d num_ent=%d"
                          " oper_flags=0x%x.\n"),
              tbl_ptr->drd_sid,
              tbl_ptr->ltd_sid,
              imin,
              imax,
              num_ent,
              oper_flags));

    if (BCMECMP_USE_INDEX_REMAP(unit)) {
        bcmecmp_id_t grp_id;
        if (min2max) {
            if (bcmecmp_grp_list_get_head(unit,
                                          BCMECMP_MEMBER_WEIGHT_LIST(unit),
                                          &grp_id) == SHR_E_NONE) {
                imax = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, grp_id) - 1;
            }
        } else {
            SHR_ERR_EXIT
                (bcmecmp_member_weight_free_idx_get(unit, op_data));
        }
    }

    for (start_idx = imin; start_idx <= imax; start_idx++) {
        /* skip index zero if required by hardware. */
        if (!start_idx && (oper_flags & BCMECMP_OPER_SKIP_ZERO)) {
            continue;
        }

        if (!BCMECMP_TBL_REF_CNT(tbl_ptr, start_idx)) {
            for (idx = start_idx;
                 idx < (start_idx + (int)num_ent) && (idx <= imax);
                 idx++) {
                if (BCMECMP_TBL_REF_CNT(tbl_ptr, idx)) {
                    break;
                }
            }
            if (idx == (start_idx + (int)num_ent)) {
                op_data->free_idx = start_idx;
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "free_idx=%d\n"), op_data->free_idx));
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(SHR_E_FULL);
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Prints the reference count of the table entries in print buffer.
 *
 * Prints the reference count of the table entries pointed by tbl_ptr, beginning
 * from the start_idx and for count number of entries.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_ptr Pointer to table properties structure.
 * \param [in] start_idx Start index.
 * \param [in] count Number of entries.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_tbl_ref_cnt_print(int unit,
                          bcmecmp_tbl_prop_t *tbl_ptr,
                          int start_idx,
                          uint32_t count)
{
    uint32_t idx = 0;
    int offset = 0;
    shr_pb_t *pb = NULL; /* Print buffer. */
    bcmecmp_hw_entry_attr_t *ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_PARAM);

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n\t[start=%d cnt=%u] idx.(ref_cnt):", start_idx, count);
    shr_pb_printf(pb, "\n\t===============================\n");
    for (idx = 0; idx < count; idx++) {
        if (!(idx % 5)) {
            shr_pb_printf(pb, "\n\t");
        }
        offset = start_idx + idx;
        ent = (((bcmecmp_hw_entry_attr_t *) tbl_ptr->ent_arr )+ offset);
        shr_pb_printf(pb, "%5d.(%2d) ",
                      offset,
                      ent->ref_cnt);
    }
    shr_pb_printf(pb, "\n");
    LOG_DEBUG(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s"), shr_pb_str(pb)));

    exit:
        shr_pb_destroy(pb);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Prints the table entries that do not have same reference count values.
 *
 * Prints the table entries that do not have the same reference count values in
 * the active and backup table entries array elements.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_ptr Pointer to table properties structure.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_tbl_ref_cnt_diff_print(int unit,
                               bcmecmp_tbl_prop_t *tbl_ptr)
{
    int idx = 0;
    shr_pb_t *pb_ref_cnt = NULL; /* Print buffer. */
    bcmecmp_hw_entry_attr_t *ent = NULL;
    bcmecmp_hw_entry_attr_t *ent_bk = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tbl_ptr, SHR_E_PARAM);

    pb_ref_cnt = shr_pb_create();
    shr_pb_printf(pb_ref_cnt,
                  "\n\t[DRD_ID=%u imin=%d imax=%u] - idx.ref_cnt(A!=Bk):",
                  tbl_ptr->drd_sid,
                  tbl_ptr->imin,
                  tbl_ptr->imax);
    shr_pb_printf(pb_ref_cnt,
                  "\n\t==================================================\n\t");

    if (BCMECMP_ATOMIC_TRANS(unit)) {
        for (idx = tbl_ptr->imin; idx <= tbl_ptr->imax; idx++) {
            ent = (((bcmecmp_hw_entry_attr_t *) tbl_ptr->ent_arr)+ idx);
            ent_bk = (((bcmecmp_hw_entry_attr_t *) tbl_ptr->ent_bk_arr)+ idx);
            if ((ent->ref_cnt
                 != ent_bk->ref_cnt)
                || (ent_bk->ref_cnt > 1
                    || ent->ref_cnt > 1)) {
                if (!(idx % 5)) {
                    shr_pb_printf(pb_ref_cnt, "\n\t");
                }
                shr_pb_printf(pb_ref_cnt, "%5d.%1d/%-1d ",
                              idx,
                              ent->ref_cnt,
                              ent_bk->ref_cnt);
            }
        }
    }

    shr_pb_printf(pb_ref_cnt, "\n");
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s"), shr_pb_str(pb_ref_cnt)));

    exit:
        shr_pb_destroy(pb_ref_cnt);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Prints the table entries that do not have same reference count values.
 *
 * Prints the table entries that do not have the same reference count values in
 * the active and backup table entries array elements.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_ptr Pointer to table properties structure.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_wali_print(int unit,
                   uint32_t wal_mentries,
                   shr_pb_t *mpb)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mpb, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMECMP_WALI_PTR(unit), SHR_E_PARAM);

    shr_pb_printf(mpb,
                  "\n\t========================================"
                  "===============================");
    shr_pb_printf(mpb,
                  "\n\tHW TBL NAME           Ent_Size(in words)"
                  " RD-ecount WR-ecount CLR-ecount\n");
    shr_pb_printf(mpb,
                  "\t----------------------------------------"
                  "-------------------------------\n");
    shr_pb_printf(mpb,
                  "\n\tINITIAL_L3_ECMP_GROUP %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMECMP_WALI_PTR(unit)->igwsize,
                  BCMECMP_WALI_PTR(unit)->igr_ecount,
                  BCMECMP_WALI_PTR(unit)->igw_ecount,
                  0);
    shr_pb_printf(mpb,
                  "\n\tL3_ECMP_COUNT         %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMECMP_WALI_PTR(unit)->gwsize,
                  BCMECMP_WALI_PTR(unit)->gr_ecount,
                  BCMECMP_WALI_PTR(unit)->gw_ecount,
                  0);
    shr_pb_printf(mpb,
                  "\n\tINITIAL_L3_ECMP       %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMECMP_WALI_PTR(unit)->imwsize,
                  BCMECMP_WALI_PTR(unit)->imr_ecount,
                  BCMECMP_WALI_PTR(unit)->imw_ecount,
                  BCMECMP_WALI_PTR(unit)->imclr_ecount);
    shr_pb_printf(mpb,
                  "\n\tL3_ECMP               %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMECMP_WALI_PTR(unit)->mwsize,
                  BCMECMP_WALI_PTR(unit)->mr_ecount,
                  BCMECMP_WALI_PTR(unit)->mw_ecount,
                  BCMECMP_WALI_PTR(unit)->mclr_ecount);
    shr_pb_printf(mpb,
                  "\n\tSW Member TBL entries successfuly queued "
                  "in WAL(RD+WR+CLR)=%-5u.\n",
                  wal_mentries);
    shr_pb_printf(mpb,
                  "\n\t========================================"
                  "===============================\n");
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "\n%s"), shr_pb_str(mpb)));
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief This function moves the member table entries from src to dest region.
 *
 * This function copies the member table entries belonging to a group from the
 * src region to the dest region in the member table. It also updates the
 * group's member table start index pointer, clears the member table entries in
 * the src region and updates the reference count for the member table entries
 * in the src and dest regions.
 *
 * \param [in] unit Unit number.
 * \param [in] pb ECMP group type.
 * \param [in] src Pointer to member table entries in the source fragment.
 * \param [in] dest Pointer to member table entries in the destination fragment.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmecmp_groups_defrag_oper(int unit,
                           shr_pb_t *pb,
                           bcmecmp_grp_type_t gtype,
                           uint32_t gap_sz,
                           const bcmecmp_grp_defrag_t *src,
                           const bcmecmp_grp_defrag_t *dest)
{
    const bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    uint32_t idx_width = 1;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);
    SHR_NULL_CHECK(dest, SHR_E_PARAM);

    mtbl_ptr = BCMECMP_TBL_PTR(unit, member, src->gtype);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    list_hdl = ecmp_info->member[gtype].list_hdl;

    shr_pb_printf(pb,
                  "\n\t%7d\t%5u ==> %-5u\t%5u ==> %-5u(%-5u)\t%5u\t%3u\n",
                  src->ecmp_id,
                  src->mstart_idx,
                  dest->mstart_idx,
                  src->gsize,
                  dest->gsize,
                  gap_sz,
                  src->gtype,
                  src->glt_sid);

    /* Copy entries from source region to destination region in member table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_cpy)(unit, dest, src));

    if (!list_hdl) {
        /*
         * Increment destination member table entries reference count in both
         *  active and backup/abort DB.
         */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_incr(mtbl_ptr, dest->mstart_idx, dest->gsize,
                                      idx_width));
    }

    /* Update Group's member table start index in HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, gmstart_updt)(unit, dest));

    /* Update SW copies of the group's member table start index value. */
    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, dest->ecmp_id) = dest->mstart_idx;

    /*
     * Clear entries in the source region in member table that were previously
     * copied.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_clr)(unit, src));

    if (!list_hdl) {
        /*
         * Decrement source member table entries reference count in both active
         *  and backup/abort DB.
         */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_decr(mtbl_ptr, src->mstart_idx, src->gsize,
                                      idx_width));
    }

    exit:
        SHR_FUNC_EXIT();
}

static int
bcmecmp_member_clear(int unit, const bcmecmp_grp_defrag_t *memb_block)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(memb_block, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_clr)(unit, memb_block));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d, mstart_idx=%u, gsize=%u.\n"),
                 memb_block->ecmp_id,
                 memb_block->mstart_idx,
                 memb_block->gsize));

    exit:
        SHR_FUNC_EXIT();
}

static int
bcmecmp_groups_resize_oper(int unit,
                           shr_pb_t *pb,
                           bcmecmp_grp_type_t gtype,
                           uint32_t gap_sz,
                           bcmecmp_grp_defrag_t *src,
                           bcmecmp_grp_defrag_t *dest)
{
    const bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    bcmecmp_grp_defrag_t ldest; /* local destination block copy. */
    uint32_t idx_width = 1;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);
    SHR_NULL_CHECK(dest, SHR_E_PARAM);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    list_hdl = ecmp_info->member[gtype].list_hdl;

    mtbl_ptr = BCMECMP_TBL_PTR(unit, member, src->gtype);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    shr_pb_printf(pb,
                  "\n\t%7d\t%5u ==> %-5u\t%5u ==> %-5u(%-5u)\t%5u\t%3u\n",
                  src->ecmp_id,
                  src->mstart_idx,
                  dest->mstart_idx,
                  src->gsize,
                  dest->gsize,
                  gap_sz,
                  src->gtype,
                  src->glt_sid);
    /*
     * Only source count of entries must be copied from the source location to
     * destination region. So, pass src gsize value in dest for member table
     * entries copy operation in the hardware table.
     */
    bcmecmp_grp_defrag_t_init(&ldest);
    ldest = *dest;
    ldest.gsize = src->gsize;

    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_cpy)(unit, &ldest, src));

    if (!list_hdl) {
        /*
         * Increment destination member table entries reference count in active
         * DB.
         */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_incr(mtbl_ptr, dest->mstart_idx, dest->gsize,
                                      idx_width));
    }

    /* Update Group's member table start index in HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, gmstart_updt)(unit, dest));

    /* Update SW copies of the group's member table start index value. */
    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, dest->ecmp_id) = dest->mstart_idx;

    /*
     * Clear entries in the source region in member table that were previously
     * copied.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_clr)(unit, src));

    if (!list_hdl) {
        /* Decrement source member table entries reference count in active DB. */
        SHR_IF_ERR_EXIT
            (bcmecmp_tbl_ref_cnt_decr(mtbl_ptr, src->mstart_idx, src->gsize,
                                      idx_width));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Print defragmentation candidate groups details.
 *
 * Print defragmentation candidate groups mstart_idx, ECMP_ID and group size
 * values, either sorted by mstart_idx or unsorted.
 *
 * \param [in] unit Unit number.
 * \param [in] dfg_arr Pointer to defragmentation group array elements.
 * \param [in] cnt dfg_arr elements count.
 * \param [in] gtype ECMP group resolution type.
 * \param [in] sorted dfg_arr array elements sorted by mstart_index status.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_dfg_print(int unit,
                  bcmecmp_grp_defrag_t *dfg_arr,
                  uint32_t cnt,
                  bcmecmp_grp_type_t gtype,
                  bool sorted)
{
    uint32_t f;
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(dfg_arr, SHR_E_PARAM);

    /* Validate input group type value. */
    if (gtype < BCMECMP_GRP_TYPE_SINGLE
        || gtype > BCMECMP_GRP_TYPE_UNDERLAY) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (dfg_arr && cnt > 0) {
        /* Debug print of Pre-Sorted Underlay ECMP Group IDs/NHOP_IDs list. */
        pb = shr_pb_create();
        shr_pb_printf(pb, "\n");
        shr_pb_printf(pb, "\t=============================================\n");
        shr_pb_printf(pb, "\tGroups %s (MTBL_SZ=%-5u gtype=%-1u):\n",
                          (sorted ? "Sorted by mstart_idx" : "Unsorted"),
                          BCMECMP_TBL_SIZE(unit, member, gtype),
                          gtype);
        shr_pb_printf(pb, "\t=============================================\n");
        shr_pb_printf(pb, "\tS.No. \tMSTART_IDX \tECMP_ID \tGSIZE\n");
        shr_pb_printf(pb, "\t----- \t---------- \t------- \t-----\n");
        for (f = 0; f < cnt; f++) {
            shr_pb_printf(pb,
                          "\t%5u \t%10d \t%7d \t%5u\n",
                          f,
                          dfg_arr[f].mstart_idx,
                          dfg_arr[f].ecmp_id,
                          dfg_arr[f].gsize);
        }
        shr_pb_printf(pb, "\n");
        LOG_DEBUG(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
        shr_pb_destroy(pb);
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Defragment ECMP group member table entries.
 *
 * This function performs defragmentation of ECMP member table entries used by
 * ECMP groups, to create space for a new group. Group type input parameter of
 * this function determines if the entire ECMP member table is to be
 * defragmented or only a portion of the member table i.e. Overlay or Underlay
 * portion is to be defragmented.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmecmp_groups_defragment(int unit,
                          const bcmecmp_lt_entry_t *entry)
{
    bcmecmp_id_t ecmp_id;    /* Group identifier. */
    uint32_t alloc_size = 0; /* sizeof defrag array. */
    uint32_t max_gsize = 0;  /* Max group size among dfg_arr[] elements. */
    int dfg_cnt = 0;    /* Number of groups for defragmentation. */
    int f;              /* Fragments iterator variable. */
    uint32_t tot_wal_mentries = 0; /* Total member table entries moved. */
    uint32_t gp_sz = 0; /* Gap size. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_grp_defrag_t rsvd_frag; /* Reserved (largest free) fragment. */
    bcmecmp_grp_defrag_t free_frag; /* Current free fragment info. */
    bcmecmp_grp_defrag_t hop; /* Temporary copy of the entries in rsvd_frag. */
    bcmecmp_grp_defrag_t fg;  /* Fragmented Group info. */
    bcmecmp_grp_defrag_t gap; /* Gap fragment info. */
    bcmecmp_grp_defrag_t *dfg_arr = NULL; /* Groups for defragmentation. */
    shr_pb_t *mpb = NULL; /* Member table debug info print buffer. */
    shr_pb_t *gpb = NULL; /* Gap debug info print buffer. */
    bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    bool use_rsvd_as_gap = FALSE; /* Use reserved region as gap for fg move. */
    bool cross_boundary_enabled = FALSE; /* The group cross OVERLAY and UNDERLAY boundary. */
    int gstart_idx, gend_idx; /* Group table start and end index. */
    int mstart_idx, mend_idx; /* Member table start and end index. */
    int member_weight_dfg_idx = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "%s.\n"), __func__));

    /* Get the group type. */
    gtype = entry->grp_type;

    /* Get member table pointer. */
    mtbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    alloc_size = sizeof(*dfg_arr) * BCMECMP_TBL_SIZE(unit, group, gtype);
    BCMECMP_ALLOC(dfg_arr, alloc_size, "bcmecmpGrpDefragArr");

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\tgtype=%u G_tbl_sz=%u.\n"),
              gtype,
              BCMECMP_TBL_SIZE(unit, group, gtype)));

    /* Get default group and member table start and end index. */
    gstart_idx = BCMECMP_TBL_IDX_MIN(unit, group, gtype);
    gend_idx = BCMECMP_TBL_IDX_MAX(unit, group, gtype);
    mstart_idx = BCMECMP_TBL_IDX_MIN(unit, member, gtype);
    mend_idx = BCMECMP_TBL_IDX_MAX(unit, member, gtype);

    /*
     * Single level and two level resolution coexist in Hierarchical mode.
     */
    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)
        && (BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) != BCMLTD_SID_INVALID)) {
        cross_boundary_enabled = TRUE;

        /*
         * For single level resolution in Hierarchical mode, ECMP_OVERLAY_ID can
         * address ECMP_OVERLAY + ECMP_UNDERLAY region.
         */
        if (gtype == BCMECMP_GRP_TYPE_UNDERLAY) {
            gstart_idx = 0;
        }
    }

    for (ecmp_id = gstart_idx; ecmp_id <= gend_idx; ecmp_id++) {
        /* If the group is not in-use continue. */
        if (!BCMECMP_TBL_REF_CNT
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
            continue;
        }

        /*
         * For resilient hashing LB_MODE groups, RH_SIZE encoding value
         * i.e. RH_ENTRIES_CNT determines the number of member table entries
         * used by this group, so use this value for 'gsize' for Regular,
         * Random and Round-Robin use MAX_PATHS value as the 'gsize' value.
         */
        if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT) {
            gp_sz = BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);
        } else {
            gp_sz = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
        }

        if (BCMECMP_USE_INDEX_REMAP(unit)) {
            if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, ecmp_id)) {
                gp_sz *= BCMECMP_FNCALL_EXEC(unit, member_weight_index_width)();
            }
        }

        if (cross_boundary_enabled) {
            if (gtype == BCMECMP_GRP_TYPE_OVERLAY) {
                if (BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) >
                    BCMECMP_TBL_IDX_MAX(unit, member, gtype)) {
                    continue;
                } else {
                    if ((BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) + (int)gp_sz - 1) >
                        BCMECMP_TBL_IDX_MAX(unit, member, gtype)) {
                        mend_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) + gp_sz - 1;
                    }
                }
            } else if (gtype == BCMECMP_GRP_TYPE_UNDERLAY) {
                if ((BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) + (int)gp_sz - 1) <
                    BCMECMP_TBL_IDX_MIN(unit, member, gtype)) {
                    continue;
                } else {
                    if (BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) <
                        BCMECMP_TBL_IDX_MIN(unit, member, gtype)) {
                        /*
                         * The group which cross boundary is handled by OVERLAY part,
                         * so skipped it.
                         */
                        mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) + gp_sz - 1;
                        continue;
                    }
                }
            }
        }

        /* Update group, transaction ID and LT SID information. */
        dfg_arr[dfg_cnt].op_arg = entry->op_arg;
        dfg_arr[dfg_cnt].glt_sid = entry->glt_sid;
        dfg_arr[dfg_cnt].ecmp_id = ecmp_id;
        dfg_arr[dfg_cnt].gtype = BCMECMP_GRP_TYPE(unit, ecmp_id);
        dfg_arr[dfg_cnt].mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
        dfg_arr[dfg_cnt].gsize = gp_sz;

        /* Check and update Max group size value. */
        if (dfg_arr[dfg_cnt].gsize > max_gsize) {
            max_gsize = dfg_arr[dfg_cnt].gsize;
        }

        /* Increment number of groups for defragmentation. */
        dfg_cnt++;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\tmax_gsize:%u dfg_cnt(Grps-in-use)=%u "
                         "ecmp_id(min_id=%-4u, max_id=%-4u)\n"),
              max_gsize,
              dfg_cnt,
              BCMECMP_TBL_IDX_MIN(unit, group, gtype),
              BCMECMP_TBL_IDX_MAX(unit, group, gtype)));

    /* Sort the defragmentation groups based on their mstart_idx values. */
    sal_qsort((void *)dfg_arr, dfg_cnt, sizeof(*dfg_arr),
              bcmecmp_defrag_grps_cmp);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\trv=%d\n"),
              bcmecmp_dfg_print(unit, dfg_arr, dfg_cnt, gtype, 1)));

    /* Initialize Reserved and Free fragments structure member variables. */
    bcmecmp_grp_defrag_t_init(&rsvd_frag);
    bcmecmp_grp_defrag_t_init(&free_frag);

    /*
     * Set initial free start index to table Min index, which is dependent on
     * the group type/ECMP MODE (FULL, FAST or HIERARCHICAL).
     */
    free_frag.mstart_idx = mstart_idx;

    /*
     * Check the gaps in the member table and update the reserved fragment
     * entries count (gsize) and reserved fragment start index values.
     */
    for (f = 0; f < dfg_cnt; f++) {
        free_frag.gsize = dfg_arr[f].mstart_idx - free_frag.mstart_idx;
        if (free_frag.gsize > rsvd_frag.gsize) {
            rsvd_frag.gsize = free_frag.gsize;
            rsvd_frag.mstart_idx = free_frag.mstart_idx;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\n\t[Init: rsvd_frag(mstart_idx=%u, gsize=%d)"
                                 "]\n"),
                      rsvd_frag.mstart_idx,
                      rsvd_frag.gsize));
        }

        /* Update free fragment mstart_idx value. */
        free_frag.mstart_idx = dfg_arr[f].mstart_idx + dfg_arr[f].gsize;
    }

    /*
     * 'free_frag.mstart_idx' here is pointing to the first index after the
     * last used entry in the ECMP member table after the above for loop
     * execution is completed.
     *
     * 'rsvd_frag.mstart_idx' is pointing to the largest free fragment that is
     * before the first group or the largest fragement that present between the
     * groups in use.
     *
     *                          INITIAL_L3_ECMP/L3_ECMP
     *                           ______________________
     *   rsvd_frag.mstart_idx-->| Free fragement       |
     *                          |                      |
     *                          |----------------------|
     *                          | GRP_1:dfg_arr[0]     |
     *                          |----------------------|
     *                          | GRP_2:dfg_arr[1]     |
     *                          |                      |
     *                          |----------------------|
     *                          | .......              |
     *                          |----------------------|
     *                          | GRP_N:dfg_arr[N - 1] |
     *                          |                      |
     *                          |----------------------|
     *  free_frag.mstart_idx--->|                      |
     *                          | Largest free         |
     *                          | fragment             |
     *                          |                      |
     *                          |                      |
     *  BCMECMP_TBL_SIZE()-----> ----------------------
     *
     *                                  (or)
     *
     *                          INITIAL_L3_ECMP/L3_ECMP
     *                           ______________________
     *                          | Free                 |
     *                          |----------------------|
     *                          | GRP_1:dfg_arr[0]     |
     *                          |--------------------- |
     *                          | GRP_2:dfg_arr[1]     |
     *                          |                      |
     *                          |----------------------|
     *   rsvd_frag.mstart_idx-->| Free                 |
     *                          |                      |
     *                          |                      |
     *                          |----------------------|
     *                          | .......              |
     *                          |----------------------|
     *                          | GRP_N:dfg_arr[N - 1] |
     *                          |                      |
     *                          |----------------------|
     *  free_frag.mstart_idx--->|                      |
     *                          | Largest free         |
     *                          | fragment             |
     *                          |                      |
     *                          |                      |
     *  BCMECMP_TBL_SIZE()-----> ----------------------
     */

    /*
     * Calculate number of free entries between the end of the last used member
     * table entry and the end of the member table.
     */
    free_frag.gsize =  mend_idx + 1 - free_frag.mstart_idx;

    /*
     * Update the reserved fragment information if the size of the new free
     * fragment is bigger than previously stored reserved fragment.
     */
    if (free_frag.gsize > rsvd_frag.gsize) {
        rsvd_frag.mstart_idx = free_frag.mstart_idx;
        rsvd_frag.gsize = free_frag.gsize;
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "\n\t[Updt: rsvd_frag(mstart_idx=%u, gsize=%d)]"
                             "\n"),
                  rsvd_frag.mstart_idx,
                  rsvd_frag.gsize));
    }

    /*
     * 'rsvd_frag.mstart_idx' is now pointing to the largest free fragment in
     * the member table only if the number of free entries (gsize) in
     * this fragment (free_frag) is greater than gsize in current
     * reserved fragment.
     *
     *                          INITIAL_L3_ECMP/L3_ECMP
     *                           ______________________
     *                          |Free fragement        |
     *                          |                      |
     *                          |----------------------|
     *                          | GRP_1:dfg_arr[0]     |
     *                          |----------------------|
     *                          | GRP_2:dfg_arr[1]     |
     *                          |                      |
     *                          |----------------------|
     *                          | .......              |
     *                          |----------------------|
     *                          | GRP_N:dfg_arr[N - 1] |
     *                          |                      |
     *                          |----------------------|
     *  rsvd_frag.mstart_idx--->|                      |
     *                          |                      |
     *                          | Largest free         |
     *                          | fragment             |
     *                          |                      |
     *                          |                      |
     *  BCMECMP_TBL_SIZE()-----> ----------------------
     *
     *                                   (or)
     *
     *                          INITIAL_L3_ECMP/L3_ECMP
     *                           ______________________
     *                          | Free                 |
     *                          |----------------------|
     *                          | GRP_1:dfg_arr[0]     |
     *                          |--------------------- |
     *                          | GRP_2:dfg_arr[1]     |
     *                          |                      |
     *                          |----------------------|
     *                          | Free                 |
     *                          |                      |
     *                          |                      |
     *                          |----------------------|
     *                          | .......              |
     *                          |----------------------|
     *                          | GRP_N:dfg_arr[N - 1] |
     *                          |                      |
     *                          |----------------------|
     *  rsvd_frag.mstart_idx--->|                      |
     *                          |                      |
     *                          | Largest free         |
     *                          | fragment             |
     *                          |                      |
     *                          |                      |
     *  BCMECMP_TBL_SIZE()-----> ----------------------
     */

    /*
     * Clear and initialize ECMP WALI buffer status for capturing current
     * defragmentation info.
     */
    sal_memset(BCMECMP_WALI_PTR(unit), 0, sizeof(*BCMECMP_WALI_PTR(unit)));
    mpb = shr_pb_create();
    gpb = shr_pb_create();
    shr_pb_printf(mpb,
                  "\n\tECMP_ID\tmstart_idx(s/d)\t SG.sz ==> DG.sz(Gap)\tgtype"
                  "\tSID\n");
    shr_pb_printf(mpb,
                  "\n\t=======\t===============\t=====================\t====="
                  "\t===\n");

    /*
     * Initialize gap member start index to the table Min index value as we have
     * to start the compression operation from the first (Min) index of the
     * table.
     */
    bcmecmp_grp_defrag_t_init(&gap);
    gap.mstart_idx = mstart_idx;

    if (BCMECMP_USE_INDEX_REMAP(unit)) {
        member_weight_dfg_idx = dfg_cnt;
    }

    /*
     * Iterate over the in-use groups and compress the gaps between the group's
     * member table entries.
     */
    for (f = 0; f < dfg_cnt; f++) {
        if (BCMECMP_USE_INDEX_REMAP(unit)) {
            if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, dfg_arr[f].ecmp_id)) {
                member_weight_dfg_idx = f;
                break;
            }
        }

        /* Get the current in-use group's details. */
        bcmecmp_grp_defrag_t_init(&fg);
        fg.mstart_idx = dfg_arr[f].mstart_idx;
        fg.gsize = dfg_arr[f].gsize;
        fg.ecmp_id = dfg_arr[f].ecmp_id;
        fg.op_arg = dfg_arr[f].op_arg;
        fg.glt_sid = dfg_arr[f].glt_sid;
        fg.gtype = dfg_arr[f].gtype;

        /*
         * Determine the gap size between the gap.mstart_index and the current
         * in-use (fg) group's mstart index.
         */
        gap.gsize = fg.mstart_idx - gap.mstart_idx;
        if (fg.mstart_idx > rsvd_frag.mstart_idx) {
            if (gap.mstart_idx <= rsvd_frag.mstart_idx) {
                if (gap.mstart_idx == rsvd_frag.mstart_idx
                    && gap.gsize >= fg.gsize
                    && gap.gsize >= rsvd_frag.gsize) {
                    use_rsvd_as_gap = TRUE;
                    shr_pb_printf(gpb,
                                  "\n\t%4u.(gap.mstart_idx(%d) == "
                                  "rsvd_frag.mstart_idx(%d) &&"
                                  "\n\t      gap.sz(%u) >= fg.gsize(%u) && "
                                  "\n\t      gap.sz(%u) >= rsvd_frag.gsize(%u))"
                                  " ECMP_ID=%-4d\n",
                                  f,
                                  gap.mstart_idx,
                                  rsvd_frag.mstart_idx,
                                  gap.gsize,
                                  fg.gsize,
                                  gap.gsize,
                                  rsvd_frag.gsize,
                                  fg.ecmp_id);

                } else if (gap.gsize < max_gsize) {
                    shr_pb_printf(gpb,
                                  "\n\t%4u.(gap.mstart_idx(%d) <= "
                                  "rsvd_frag.mstart_idx(%d) && "
                                  "\n\t      gap.sz(%u) < max_gsize(%u)) "
                                  "ECMP_ID=%-4d\n",
                                  f,
                                  gap.mstart_idx,
                                  rsvd_frag.mstart_idx,
                                  gap.gsize,
                                  max_gsize,
                                  fg.ecmp_id);
                    /* Skip the reserved fragment. */
                    gap.mstart_idx = rsvd_frag.mstart_idx + rsvd_frag.gsize;
                    gap.gsize = fg.mstart_idx - gap.mstart_idx;
                }
            }
        }

        if (gap.gsize == 0) {
            shr_pb_printf(gpb,
                          "\n\t%4u.(gap.sz(=%u)):(Skp) ECMP_ID=%-4d\n",
                          f,
                          gap.gsize,
                          fg.ecmp_id);

            gap.mstart_idx = fg.mstart_idx + fg.gsize;
        } else if (gap.gsize > 0) {
            if (fg.gsize <= gap.gsize) {
                gp_sz = gap.gsize;
                shr_pb_printf(gpb,
                              "\n\t%4u.(gap.sz(=%u) > 0 "
                              "&& fg.gsz(=%u) <= gap.sz(=%u)):"
                              "(Mv) ECMP_ID=%-4d\n",
                              f,
                              gap.gsize,
                              fg.gsize,
                              gap.gsize,
                              fg.ecmp_id);
                /*
                 * Update gap members (Dest) with fg (Src) group details to
                 * perform the move operation.
                 */
                gap.ecmp_id = fg.ecmp_id;
                gap.op_arg = fg.op_arg;
                gap.glt_sid = fg.glt_sid;
                gap.gtype = fg.gtype;
                gap.gsize = fg.gsize;

                /*
                 * Perform member move to gap region operation and update the
                 * group base pointer with the new mstart_idx value.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &fg, &gap));

                /*
                 * Update reserved fragment start index and gsize as it has
                 * moved to the new gap location as group (fg) has been moved
                 * into the previous rsvd_frag (mstart_idx and gsize) location.
                 */
                if (use_rsvd_as_gap) {
                    rsvd_frag.mstart_idx = gap.mstart_idx + fg.gsize;
                    rsvd_frag.gsize = (fg.mstart_idx + fg.gsize) -
                                            (gap.mstart_idx + fg.gsize);
                    use_rsvd_as_gap = FALSE;
                    shr_pb_printf(gpb,
                                  "\n\t%4u.[New: rsvd_frag(mstart_idx=%u, "
                                  "gsize=%d)]\n",
                                  f,
                                  rsvd_frag.mstart_idx,
                                  rsvd_frag.gsize);
                }
                /*
                 * Move the gap's member table start index to point to the entry
                 * after the currently copied group (fg).
                 */
                gap.mstart_idx += fg.gsize;
                tot_wal_mentries += fg.gsize;

            } else if (fg.gsize <= rsvd_frag.gsize) {
                shr_pb_printf(gpb,
                              "\n\t%4u.(gap.sz(=%u) > 0 && "
                              "fg.gsz(=%u) <= rsvd.sz(=%u)):"
                              "(2Mv) ECMP_ID=%-4d\n",
                              f,
                              gap.gsize,
                              fg.gsize,
                              rsvd_frag.gsize,
                              fg.ecmp_id);
                /*
                 * Use the reserved fragment as an hop for the member entries
                 * before moving them into the gap (final location).
                 */
                bcmecmp_grp_defrag_t_init(&hop);
                hop = fg;
                hop.mstart_idx = rsvd_frag.mstart_idx;
                gp_sz = rsvd_frag.gsize;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &fg, &hop));
                tot_wal_mentries += fg.gsize;
                gp_sz = gap.gsize + fg.gsize;

                /*
                 * Now move the mebers in to the gap. Update the gap.gize value
                 * to fg.gsize value so that only entries belonging to the
                 * group are copied from reserved region (hop) to final gap
                 * region.
                 */
                gap.ecmp_id = fg.ecmp_id;
                gap.op_arg = fg.op_arg;
                gap.glt_sid = fg.glt_sid;
                gap.gtype = fg.gtype;
                gap.gsize = fg.gsize;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &hop, &gap));

                /*
                 * Move the gap's member table start index to point to the entry
                 * after the currently copied group (fg).
                 */
                gap.mstart_idx += fg.gsize;
                tot_wal_mentries += fg.gsize;
            } else {
                shr_pb_printf(gpb,
                              "\n\t%4u.(fg.gsz(=%u) > (rsvd.sz(=%u)"
                              " && gap.sz(=%u))):(Skp) ECMP_ID=%-4d\n",
                              f,
                              fg.gsize,
                              rsvd_frag.gsize,
                              gap.gsize,
                              fg.ecmp_id);
                gap.mstart_idx = fg.mstart_idx + fg.gsize;
            }
        } else {
            /* Gap size should not be negative, return error. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    if (BCMECMP_USE_INDEX_REMAP(unit)) {
        int gap_mend_idx = mend_idx;
        int idx_width = BCMECMP_FNCALL_EXEC(unit, member_weight_index_width)();
        int rsvd_frag_next_idx;

        if (rsvd_frag.mstart_idx % idx_width != 0) {
            rsvd_frag_next_idx = (rsvd_frag.mstart_idx / idx_width + 1) *
                                 idx_width;
            if (rsvd_frag_next_idx < (int)(rsvd_frag.mstart_idx +
                                           rsvd_frag.gsize)) {
                rsvd_frag.gsize -= (rsvd_frag_next_idx - rsvd_frag.mstart_idx);
                rsvd_frag.mstart_idx = rsvd_frag_next_idx;
            } else {
                rsvd_frag.gsize = 0;
            }
        }

        bcmecmp_grp_defrag_t_init(&gap);

        for (f = (dfg_cnt - 1); f >= member_weight_dfg_idx; f--) {
            bcmecmp_grp_defrag_t_init(&fg);
            fg.mstart_idx = dfg_arr[f].mstart_idx;
            fg.gsize = dfg_arr[f].gsize;
            fg.ecmp_id = dfg_arr[f].ecmp_id;
            fg.op_arg = dfg_arr[f].op_arg;
            fg.glt_sid = dfg_arr[f].glt_sid;
            fg.gtype = dfg_arr[f].gtype;

            gap.mstart_idx = fg.mstart_idx + fg.gsize;
            gap.gsize = gap_mend_idx - gap.mstart_idx + 1;
            if (fg.mstart_idx < rsvd_frag.mstart_idx) {
                if (gap.mstart_idx >= rsvd_frag.mstart_idx) {
                    if (gap.mstart_idx == rsvd_frag.mstart_idx
                        && gap.gsize >= fg.gsize
                        && gap.gsize >= rsvd_frag.gsize) {
                        use_rsvd_as_gap = TRUE;
                    } else if (gap.gsize < max_gsize) {
                        /* Skip the reserved fragment. */
                        gap.mstart_idx = fg.mstart_idx + fg.gsize;
                        gap.gsize = rsvd_frag.mstart_idx - gap.mstart_idx;
                    }
                }
            }

            if (gap.gsize == 0) {
                gap_mend_idx = fg.mstart_idx - 1;
            } else if (gap.gsize > 0) {
                if (fg.gsize <= gap.gsize) {
                    gap.ecmp_id = fg.ecmp_id;
                    gap.op_arg = fg.op_arg;
                    gap.glt_sid = fg.glt_sid;
                    gap.gtype = fg.gtype;
                    gap.gsize = fg.gsize;
                    gap.mstart_idx = gap_mend_idx - fg.gsize + 1;
                    gp_sz = fg.gsize;

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &fg, &gap));

                    if (use_rsvd_as_gap) {
                        rsvd_frag.mstart_idx = fg.mstart_idx + fg.gsize;
                        rsvd_frag.gsize = gap.mstart_idx - fg.mstart_idx;
                        use_rsvd_as_gap = FALSE;
                    }

                    gap_mend_idx -= fg.gsize;
                    tot_wal_mentries += fg.gsize;
                } else if (fg.gsize <= rsvd_frag.gsize) {
                    bcmecmp_grp_defrag_t_init(&hop);
                    hop = fg;
                    hop.mstart_idx = rsvd_frag.mstart_idx;
                    gp_sz = rsvd_frag.gsize;

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &fg, &hop));

                    tot_wal_mentries += fg.gsize;
                    gp_sz = gap.gsize + fg.gsize;

                    gap.ecmp_id = fg.ecmp_id;
                    gap.op_arg = fg.op_arg;
                    gap.glt_sid = fg.glt_sid;
                    gap.gtype = fg.gtype;
                    gap.gsize = fg.gsize;
                    gap.mstart_idx = gap_mend_idx - fg.gsize + 1;

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmecmp_groups_defrag_oper(unit, mpb, gtype, gp_sz, &hop, &gap));

                    gap_mend_idx -= fg.gsize;
                    tot_wal_mentries += fg.gsize;
                } else {
                    gap_mend_idx = fg.mstart_idx - 1;
                }
            } else {
                /* Gap size should not be negative, return error. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "rv=%d.\n"),
             bcmecmp_tbl_ref_cnt_diff_print(unit, mtbl_ptr)));

    exit:
        if (gpb) {
            LOG_DEBUG(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "\n%s"), shr_pb_str(gpb)));
            shr_pb_destroy(gpb);
        }
        if (mpb) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "rv=%d.\n"),
                     bcmecmp_wali_print(unit, tot_wal_mentries, mpb)));
            shr_pb_destroy(mpb);
        }
        BCMECMP_FREE(dfg_arr);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Move ECMP group member table entries.
 *
 * This function performs move of ECMP member table entries from OVERLAY or
 * UNDERLAY region to another.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 * \param [in] overlay2underlay Indicates move from OVERLAY to UNDERLAY region.
 * \param [inout] move_size The number of entries move.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmecmp_groups_move(int unit, const bcmecmp_lt_entry_t *entry,
                    bool overlay2underlay, uint32_t *move_size)
{
    bcmecmp_id_t ecmp_id;
    bcmecmp_tbl_op_t tbl_op;
    bcmecmp_grp_defrag_t src_frag, dst_frag;
    shr_pb_t *mpb = NULL;
    int boundary = BCMECMP_TBL_IDX_MIN(unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
    int rv;
    uint32_t tmp_size = 0, gsize;
    bool min2max = TRUE;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(move_size, SHR_E_PARAM);

    if (BCMECMP_MODE_HIER != BCMECMP_MODE(unit)
        || (BCMECMP_TBL_LTD_SID
                (unit, group, BCMECMP_GRP_TYPE_SINGLE) == BCMLTD_SID_INVALID)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_memset(BCMECMP_WALI_PTR(unit), 0, sizeof(*BCMECMP_WALI_PTR(unit)));
    mpb = shr_pb_create();

    bcmecmp_tbl_op_t_init(&tbl_op);
    tbl_op.oper_flags |= BCMECMP_OPER_ADD;

    if (overlay2underlay) {
        tbl_op.tbl_ptr = BCMECMP_TBL_PTR
                             (unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
    } else {
        tbl_op.tbl_ptr = BCMECMP_TBL_PTR
                             (unit, member, BCMECMP_GRP_TYPE_OVERLAY);
    }

    /*
     * For single level and two level resolution coexist in Hierarchical mode,
     * only OVERLAY part group can be moved.
     */
    for (ecmp_id = BCMECMP_TBL_IDX_MIN(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id <= BCMECMP_TBL_IDX_MAX(unit, group, BCMECMP_GRP_TYPE_OVERLAY);
         ecmp_id++) {
        /* If the group is not in-use continue. */
        if (!BCMECMP_TBL_REF_CNT
                (BCMECMP_TBL_PTR(unit, group, BCMECMP_GRP_TYPE_OVERLAY), ecmp_id)) {
            continue;
        }

        if (BCMECMP_GRP_TYPE(unit, ecmp_id) != BCMECMP_GRP_TYPE_SINGLE) {
            continue;
        }

        if ((BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) < boundary && overlay2underlay)
            || (BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) >= boundary && !overlay2underlay)) {
            if (BCMECMP_GRP_LB_MODE(unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT) {
                gsize = BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);
            } else {
                gsize = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
            }

            tbl_op.num_ent = gsize;
            rv = bcmecmp_tbl_free_idx_get(unit, &tbl_op, min2max);
            if (rv == SHR_E_FULL) {
                continue;
            }

            bcmecmp_grp_defrag_t_init(&src_frag);
            bcmecmp_grp_defrag_t_init(&dst_frag);

            src_frag.mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
            src_frag.gsize = gsize;
            src_frag.ecmp_id = ecmp_id;
            src_frag.op_arg = entry->op_arg;
            src_frag.glt_sid = BCMECMP_GRP_LT_SID(unit, ecmp_id);
            src_frag.gtype = BCMECMP_GRP_TYPE(unit, ecmp_id);

            dst_frag.mstart_idx = tbl_op.free_idx;
            dst_frag.gsize = src_frag.gsize;
            dst_frag.ecmp_id = src_frag.ecmp_id;
            dst_frag.op_arg = src_frag.op_arg;
            dst_frag.glt_sid = src_frag.glt_sid;
            dst_frag.gtype = src_frag.gtype;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_groups_defrag_oper(unit, mpb,
                       ((overlay2underlay) ? BCMECMP_GRP_TYPE_OVERLAY:
                       BCMECMP_GRP_TYPE_UNDERLAY), gsize, &src_frag, &dst_frag));

            tmp_size += gsize;
            if (tmp_size >= *move_size) {
                break;
            }
        }
    }

    *move_size = tmp_size;

    exit:
        if (mpb) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "rv=%d.\n"),
                     bcmecmp_wali_print(unit, tmp_size, mpb)));
            shr_pb_destroy(mpb);
        }

       SHR_FUNC_EXIT();
}

/*!
 * \brief Defragment and move ECMP group member table entries.
 *
 * This function performs move and defragmentation of ECMP member table entries
 * used by ECMP groups, to create space for a new group.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmecmp_groups_defragment_and_move(int unit,
                                   const bcmecmp_lt_entry_t *entry)
{
    bcmecmp_lt_entry_t *tmp_entry = NULL;
    uint32_t move_size = 0;
    bool overlay2underlay = FALSE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMECMP_ALLOC(tmp_entry, sizeof(*tmp_entry), "bcmecmpGrpDfgEnt");

    /* Single level and two level resolution coexist in Hierarchical mode. */
    if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)
        && (entry->grp_type == BCMECMP_GRP_TYPE_SINGLE)) {
        sal_memcpy(tmp_entry, entry, sizeof(bcmecmp_lt_entry_t));

        tmp_entry->grp_type = BCMECMP_GRP_TYPE_UNDERLAY;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_groups_defragment(unit, tmp_entry));

        tmp_entry->grp_type = BCMECMP_GRP_TYPE_OVERLAY;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_groups_defragment(unit, tmp_entry));

        if (entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
            move_size = entry->rh_entries_cnt;
        } else {
            move_size = entry->max_paths;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_groups_move(unit, entry, overlay2underlay, &move_size));

        if (move_size > 0) {
            tmp_entry->grp_type = BCMECMP_GRP_TYPE_UNDERLAY;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_groups_defragment(unit, tmp_entry));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_groups_defragment(unit, entry));
    }

    exit:
        if (tmp_entry) {
            BCMECMP_FREE(tmp_entry);
        }

        SHR_FUNC_EXIT();
}

/*!
 * \brief ECMP group next-hop members print debug function.
 *
 * ECMP group next-hop members print debug function.
 *
 * \param [in] unit Unit number.
 * \param [in] pre_sort Set to TRUE to print unsorted next-hop array elements.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_nhops_debug_print(int unit,
                                bool pre_sort,
                                bcmecmp_lt_entry_t *lt_entry)
{
    shr_pb_t *pb = NULL; /* Print buffer. */
    uint32_t idx;        /* Local iterator variable. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Debug print of Pre-Sorted Underlay ECMP Group IDs/NHOP_IDs list. */
    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t%s", pre_sort ? "Pre-Sorted:" : "Sorted:");
    shr_pb_printf(pb,
        "[npaths=%-4d sort=%d]:\n",
        lt_entry->num_paths,
        lt_entry->nhop_sorted);
    shr_pb_printf
        (pb, "\t=============================");
    for (idx = 0; idx < lt_entry->num_paths; idx++) {
        if (!(idx % 5)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      " %4d) %-5d",
                      idx,
                      (lt_entry->ecmp_nhop
                        ? lt_entry->uecmp_id[idx] : lt_entry->nhop_id[idx]));
    }
    shr_pb_printf(pb, "\n");
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Sorts group next-hop array elements.
 *
 * Sorts list of next-hop members belonging to the group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
int
bcmecmp_group_nhops_sort(int unit, bcmecmp_lt_entry_t *lt_entry)
{
    size_t nmemb; /* Number of members in group. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Debug print of Pre-Sorted Underlay ECMP Group IDs/NHOP_IDs list. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_group_nhops_debug_print(unit, TRUE, lt_entry)));

    /* Get number of elements in the array that's to be sorted. */
    nmemb = (size_t)lt_entry->num_paths;

    /* Check NHOP_ECMP status. */
    if (lt_entry->ecmp_nhop) {
        /* Sort supplied ECMP_UNDERLAY_IDs. */
        sal_qsort((void *)lt_entry->uecmp_id, nmemb,
                  sizeof(lt_entry->uecmp_id[0]), bcmecmp_cmp_int);
    } else {
        /* Sort supplied NHOP_IDs. */
        sal_qsort((void *)lt_entry->nhop_id, nmemb,
                  sizeof(lt_entry->nhop_id[0]), bcmecmp_cmp_int);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_group_nhops_debug_print(unit, FALSE, lt_entry)));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add member/s to an existing RH-ECMP group.
 *
 * This function adds member or members to an existing RH-ECMP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_add(int unit,
                           bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t new_mindex; /* New member array index. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_members_add.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->rh_entries_cnt));
    /*
     * Copy RH group's existing entries usage count array into LT entry's RH
     * entries usage count array for update operation.
     */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * grp_npaths));

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "PREV rh data add",
                                        ecmp_id, lt_entry)));

    /*
     * Call RH member add function for every new member in the
     * next-hop array. New members must be set in the array after the
     * current next-hop member elements.
     */
    for (new_mindex = grp_npaths;
         new_mindex < lt_entry->rh_num_paths;
         new_mindex++) {
        SHR_IF_ERR_EXIT
            (bcmecmp_rh_elem_member_add(unit,
                                        new_mindex,
                                        lt_entry,
                                        rh_group_member_cb));
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "POST rh data add",
                                        ecmp_id, lt_entry)));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete member/s from an existing RH-ECMP group.
 *
 * This function deletes member or members from an existing RH-ECMP group
 * and re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_delete(int unit,
                              bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted members indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t del_idx = 0; /* Deleted members array index iterator. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_delete.\n")));

    ecmp_id = lt_entry->ecmp_id;

    /*
     * Check if num_paths is non-zero in this delete (update) operation. If
     * there are valid members after delete, the remaining members must be
     * re-balanced.
     *
     * If there no members left in the group after this delete operation,
     * re-balance operation is not required. Clear the entries in the member
     * table, clear the mindex value for all entries in this group, reset
     * entries usage count and return.
     */
    if (lt_entry->rh_num_paths) {
        grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        /* Determine how many members have been deleted. */
        deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

        /*
         * Initialize member array pointers based on the group's next-hop
         * type.
         */
        if (lt_entry->ecmp_nhop) {
            memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
            umemb_arr = lt_entry->uecmp_id;
        } else {
            memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
            umemb_arr = lt_entry->nhop_id;
        }

        /* Valid member array must be preset, else it's an internal error. */
        SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
        SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

        /*
         * For update operation, copy existing group RH entries array
         * into LT entry's RH entries array for updation.
         */
        sal_memcpy(lt_entry->rh_entries_arr,
                   BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                    lt_entry->rh_entries_cnt));
        /*
         * Copy RH group's existing entries mapped member index array into LT
         * entry's RH entries mapped member index array for update operation.
         */
        sal_memcpy(lt_entry->rh_mindex_arr,
                   BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*(lt_entry->rh_mindex_arr)) *
                    lt_entry->rh_entries_cnt));

        BCMECMP_ALLOC(deleted_mindex,
                      deleted_cnt * sizeof(*deleted_mindex),
                      "bcmecmpRhGrpMembsDelMindexArr");

        BCMECMP_ALLOC(shrd_mindex,
                      (grp_npaths * sizeof(*shrd_mindex)),
                      "bcmecmpRhGrpMembsDelShrdMindexArr");
        sal_memset(shrd_mindex,
                   BCMECMP_INVALID,
                   (grp_npaths * sizeof(*shrd_mindex)));

        /*
         * The set of new group members are a sub-set of the current members
         * list as some of the existing members have been deleted from the list
         * as part of update operation.
         *
         * So, for every member in the existing members array (outer for-loop),
         * check if this member exists in the updated members array
         * (inner for-loop). If it's not present, store this member index in the
         * deleted member indices array.
         */
        for (idx = 0; idx < grp_npaths; idx++) {
            /*
             * num_paths value must be less than grp_npaths as it's member/s
             * delete operation and number of elements in umemb_arr is equal to
             * num_paths.
             */
            for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
                /*
                 * Compare existing nhop members with members in updated members
                 * array, confirm rh_memb_ecnt_arr value is equal to zero for
                 * the updated nhop member to skip duplicate members.
                 */
                if (memb_arr[idx] == umemb_arr[uidx]
                    && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                    lt_entry->rh_memb_ecnt_arr[uidx] =
                            BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                    shrd_cnt++;
                    if (idx != uidx) {
                        /*
                         * Save new mindex mapped to the old mindex value for
                         * update after the delete operation.
                         */
                        shrd_mindex[idx] = uidx;
                        shrd_new_mindex_cnt++;

                        /*
                         * Reset the entry usage counter value as this member
                         * will be maintained in the new index location "uidx"
                         * after the delete operation.
                         */
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                    }
                    break;
                }
            }

            /*
             * If the member is present in the updated nhop members array, move
             * to the next member in existing array.
             */
            if (uidx != lt_entry->rh_num_paths) {
                continue;
            }

            /*
             * Member not found in the updated array, add this member index to
             * be deleted members list.
             */
            if (del_idx < deleted_cnt) {
                deleted_mindex[del_idx++] = idx;
            } else {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "del_idx = %u > del_max_idx = %u.\n"),
                            del_idx,
                            (deleted_cnt - 1)));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }

        for (idx = 0; idx < deleted_cnt; idx++) {
            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        rh_grp_data_debug_print(unit, "PREV rh data delete",
                                                ecmp_id, lt_entry)));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_rh_elem_member_delete(unit,
                                               deleted_mindex[idx],
                                               lt_entry,
                                               rh_group_member_cb));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        rh_grp_data_debug_print(unit, "POST rh data delete",
                                                ecmp_id, lt_entry)));
        }

        /*
         * For the shared members, check and update the member index value in
         * the LT entry mindex array if it has changed.
         */
        if (shrd_new_mindex_cnt) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n\t[Shared mindex update]\n");
            shr_pb_printf(pb, "\t======================\n");
            for (idx = 0; idx < grp_npaths; idx++) {
                new_idx = shrd_mindex[idx];
                if (new_idx == (uint32_t)BCMECMP_INVALID) {
                    continue;
                }
                for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                    if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                        && idx == lt_entry->rh_mindex_arr[uidx]) {
                        shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                      "[n_mindex=%u / n_nhop=%-5d]\n",
                                      idx,
                                      memb_arr[idx],
                                      new_idx,
                                      lt_entry->rh_entries_arr[uidx]);

                        /* Update mindex with new mindex value. */
                        lt_entry->rh_mindex_arr[uidx] = new_idx;
                    }
                }
                shrd_new_mindex_cnt--;
            }
            shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                          shrd_new_mindex_cnt);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    } else {
        /* Clear RH-ECMP group member table entries. */
        sal_memset(lt_entry->rh_entries_arr,
                   0,
                   (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group member indices data. */
        sal_memset(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                    lt_entry->rh_entries_cnt));

        /* Clear stored RH-ECMP group members entry usage count data. */
        sal_memset(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    BCMECMP_LT_MAX_RH_NPATHS(unit, group, lt_entry->grp_type)));
    }

    exit:
        if (lt_entry->rh_num_paths) {
            /* Free memory allocated to store mindex array. */
            BCMECMP_FREE(deleted_mindex);
            BCMECMP_FREE(shrd_mindex);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Replace member/s from an existing RH-ECMP group.
 *
 * This function replaces member or members from an existing RH-ECMP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_replace(int unit,
                               bcmecmp_lt_entry_t *lt_entry)
{
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t idx, uidx;       /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id;     /* ECMP group identifier. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_replace.\n")));

    ecmp_id = lt_entry->ecmp_id;

    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For replace operation, update num_paths must be equal to group's current
     * num_paths value.
     */
    if (grp_npaths - lt_entry->rh_num_paths) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /* Valid member array must be preset, else it's an internal error. */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(BCMECMP_GRP_RHG_PTR(unit, ecmp_id), SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->rh_entries_cnt));

    /* Copy members entry usage count information. */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * lt_entry->rh_num_paths));

    /* RH-Group data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                rh_grp_data_debug_print(unit, "PREV rh replace",
                                        ecmp_id, lt_entry)));

    /*
     * Find and replace the existing nhop member with the new nhop member
     * ID value.
     */
    for (idx = 0; idx < lt_entry->rh_num_paths; idx++) {
        if (memb_arr[idx] != umemb_arr[idx]) {
            pb = shr_pb_create();
            shr_pb_printf(pb, "\n");
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[idx];
                    shr_pb_printf(pb, "\t%5d) o_nhop = %-5d n_nhop=%-5d\n",
                                  uidx,
                                  memb_arr[idx],
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
            shr_pb_printf(pb, "\n");
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "%s"), shr_pb_str(pb)));
            shr_pb_destroy(pb);
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add, replace and re-balance member/s in an existing RH-ECMP group.
 *
 * This function performs member/s add, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmecmp_rh_grp_members_add_replace(int unit,
                                   bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t added_cnt = 0; /* Number of members added to the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_mindex_cnt = 0; /* Count of shared members with new mindex. */
    uint32_t rep_idx = 0; /* Added member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_members_add_replace.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /*
     * Current and updated member array pointers must be valid, check and
     * return error if this condition is not met.
     */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->rh_entries_cnt));

    BCMECMP_ALLOC(replace_mindex,
                  (grp_npaths * sizeof(*replace_mindex)),
                  "bcmecmpRhGrpMembsAddRepReplaceMindexArr");
    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsAddRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Identify the members that are shared between the updated NHOP array and
     * the current NHOP members up to the group's current NUM_PATHS value.
     * Members in the updated array after current NUM_PATHS value will be
     * treated as member add operation.
     * For members up to grp_npaths that are not shared with the updated members
     * array will be candidates for replace
     * operation.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            if ((memb_arr[idx] == umemb_arr[uidx])
                && (lt_entry->rh_memb_ecnt_arr[uidx] == 0)) {
                /* Update entry usage count. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];

                /* Increment shared entry count. */
                shrd_cnt++;

                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_mindex_cnt++;
                }

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * Existing member at current index is also part of the updated
         * next-hop members array, continue to the next member.
         */
        if (uidx != lt_entry->rh_num_paths) {
            continue;
        }

        /* Member not shared hence add to replace member elements list. */
        replace_mindex[replace_cnt++] = idx;
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[grp_npaths=%u Shrd_cnt=%u Rep_cnt=%u]\n",
                  grp_npaths, shrd_cnt, replace_cnt);
    shr_pb_printf(pb, "\t======================================\n");

    if (replace_cnt) {
        BCMECMP_ALLOC(new_mindex,
                      replace_cnt * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsAddRepNewMindexArr");

        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {

            /* Get the old to be replaced index value. */
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < grp_npaths; uidx++) {

                if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                    /* Already updated, move to next member. */
                    continue;
                }

                /* Update entry count value. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];

                /* Clear old entry usage count value. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Store the new mindex mapped to this old mindex for updating
                 * the mindex and rh_entries_arr arrays.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                          "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                           idx,
                           rep_idx,
                           uidx);
                break;
            }
        }
    }

    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && idx == lt_entry->rh_mindex_arr[uidx]) {

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  memb_arr[idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                }
            }
            shrd_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                      shrd_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");

        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[rep_idx] == lt_entry->rh_entries_arr[uidx]
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[new_idx];

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  memb_arr[rep_idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));

    for (new_idx = 0; new_idx < lt_entry->rh_num_paths;
         new_idx++) {
        if (lt_entry->rh_memb_ecnt_arr[new_idx]) {
            continue;
        }
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    rh_grp_data_debug_print(unit, "PREV rh data add",
                                            ecmp_id, lt_entry)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_elem_member_add(unit,
                                        new_idx,
                                        lt_entry,
                                        rh_group_member_cb));
        added_cnt++;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n\tadded_cnt=%u n_mindex=%u n_nhop=%d\n"),
                     added_cnt, new_idx, umemb_arr[new_idx]));
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete, replace and re-balance member/s in an existing RH-ECMP group.
 *
 * This function performs member/s delete, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmecmp_rh_grp_members_delete_replace(int unit,
                                      bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* ECMP group identifier. */
    uint32_t grp_npaths = 0; /* Current num_paths value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted member/s indices. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t del_idx = 0; /* Deleted members array index. */
    uint32_t rep_idx = 0; /* Replace member array index. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    const int *memb_arr = NULL;  /* Group next-hop members array. */
    const int *umemb_arr = NULL; /* Group updated next-hop members array. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_rh_grp_members_delete_replace.\n")));

    /*
     * NUM_PATHS must be non-zero when this function is called for member delete
     * operation, check and return if this condition is not met.
     */
    if (!lt_entry->rh_num_paths) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the group's ECMP_ID value and current NUM_PATHS values. */
    ecmp_id = lt_entry->ecmp_id;
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Determine how many members have been deleted based on the updated
     * NUM_PATHS value.
     */
    deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (lt_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->uecmp_id;
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
        umemb_arr = lt_entry->nhop_id;
    }

    /*
     * Current and updated member array pointers must be valid, check and
     * return error if this condition is not met.
     */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);
    SHR_NULL_CHECK(umemb_arr, SHR_E_INTERNAL);

    /*
     * Copy existing group RH entries array into LT entry RH entries array for
     * members delete + replace operation.
     */
    sal_memcpy(lt_entry->rh_entries_arr,
               BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               (sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                lt_entry->rh_entries_cnt));
    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->rh_entries_cnt));

    BCMECMP_ALLOC(deleted_mindex,
                  deleted_cnt * sizeof(*deleted_mindex),
                  "bcmecmpRhGrpMembsDelRepDelMindexArr");

    BCMECMP_ALLOC(replace_mindex,
                  (lt_entry->rh_num_paths) * sizeof(*replace_mindex),
                  "bcmecmpRhGrpMembsDelRepReplaceMindexArr");

    BCMECMP_ALLOC(shrd_mindex,
                  (grp_npaths * sizeof(*shrd_mindex)),
                  "bcmecmpRhGrpMembsDelRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMECMP_INVALID,
               (grp_npaths * sizeof(*shrd_mindex)));

    /*
     * Order of NHOP members in updated NHOP array is not same as the stored
     * NHOP members array and some of the members in the updated array might be
     * shared with the stored members. Identify these shared members, deleted
     * members and members to be replaced.
     */
    for (idx = 0; idx < grp_npaths; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            /*
             * Identify the common members and copy the stored member entry
             * usage count value to LT entry usage count element at the new
             * member index for delete and rebalance operation.
             */
            if (memb_arr[idx] == umemb_arr[uidx]
                && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
                shrd_cnt++;
                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_new_mindex_cnt++;
                    /*
                     * Reset the entry usage counter value as this member
                     * will be maintained in the new index location "uidx"
                     * after the delete operation.
                     */
                }
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
                break;
            }
        }

        /*
         * If the member is a shared member, then continue to the next member
         * in the list.
         */
        if (uidx != lt_entry->rh_num_paths) {
            continue;
        }

        /*
         * Member not found in the updated array, add this old member index
         * value be the deleted members array.
         */
        if (del_idx < deleted_cnt) {
            deleted_mindex[del_idx++] = idx;
        } else {
            /*
             * If this member is not a shared member and members delete count
             * has already been reached, then save this member for replacing
             * with a new member in the updated members array.
             */
            replace_mindex[replace_cnt++] = idx;
        }
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "\t[Shrd_cnt=%u / Shrd_n_mindex_cnt=%u Del_cnt=%u"
                      " Rep_cnt=%u]\n",
                  shrd_cnt,
                  shrd_new_mindex_cnt,
                  deleted_cnt,
                  replace_cnt);
    shr_pb_printf(pb, "\t=================================================\n");

    /*
     * If there are members to be replaced, identify the members from the
     * updated NHOP members array that will be replacing the current members.
     * Save the new member index value that is replacing a current member and
     * also assign the current members entry usage count value to the new
     * member in LT entry array for use during delete member rebalance
     * calculations.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Members replacment list]\n");
        shr_pb_printf(pb, "\t=========================\n");
        BCMECMP_ALLOC(new_mindex,
                      (replace_cnt) * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsNewMindexArr");
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            /*
             * Shared member entry count is already found, skip to next
             * member.
             */
            if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                continue;
            }
            for (idx = 0; idx < replace_cnt; idx++) {
                rep_idx = replace_mindex[idx];
                if (!BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx]) {
                    /*
                     * This replacement member candidate has already been
                     * associate with a new member from the updated array, move
                     * on to the next member in the replace mindex array.
                     */
                    continue;
                }
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Save the new member index value that is replacing the current
                 * replacement member.
                 */
                new_mindex[idx] = uidx;

                shr_pb_printf(pb,
                              "\trep_arr.(idx=%u) o_mindex=%u n_mindex=%u\n",
                              idx,
                              rep_idx,
                              uidx);
                break;

            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
    pb = NULL;

    /* Perform members delete and rebalance operation. */
    for (idx = 0; idx < deleted_cnt; idx++) {
        /* RH-Group data print debug function. */
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "rv=%d"),
                    rh_grp_data_debug_print(unit, "PREV rh data delete",
                                            ecmp_id, lt_entry)));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_rh_elem_member_delete(unit,
                                           deleted_mindex[idx],
                                           lt_entry,
                                           rh_group_member_cb));
    }

    pb = shr_pb_create();
    shr_pb_printf(pb, "\n");
    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_new_mindex_cnt) {
        shr_pb_printf(pb, "\t[Shared mindex update]\n");
        shr_pb_printf(pb, "\t======================\n");
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[idx] == lt_entry->rh_entries_arr[uidx]
                    && idx == lt_entry->rh_mindex_arr[uidx]) {
                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  idx,
                                  memb_arr[idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
            shrd_new_mindex_cnt--;
        }
        shr_pb_printf(pb, "\t Remaining shrd_entries for update=%u\n",
                      shrd_new_mindex_cnt);
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        shr_pb_printf(pb, "\t[Replaced mindex update]\n");
        shr_pb_printf(pb, "\t========================\n");
        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];
            for (uidx = 0; uidx < lt_entry->rh_entries_cnt; uidx++) {
                if (memb_arr[rep_idx] == lt_entry->rh_entries_arr[uidx]
                    && rep_idx == lt_entry->rh_mindex_arr[uidx]) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    lt_entry->rh_entries_arr[uidx] = umemb_arr[new_idx];

                    shr_pb_printf(pb, "\t[o_mindex=%u / o_nhop=%-5d]"
                                  "[n_mindex=%u / n_nhop=%-5d]\n",
                                  rep_idx,
                                  memb_arr[rep_idx],
                                  new_idx,
                                  lt_entry->rh_entries_arr[uidx]);
                }
            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));
    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(deleted_mindex);
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
        if (pb) {
            shr_pb_destroy(pb);
        }
        SHR_FUNC_EXIT();
}

static bool
bcmecmp_group_rh_member_compare(int unit,
                                bcmecmp_lt_entry_t *lt_entry,
                                uint32_t idx,
                                uint32_t uidx)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_lt_entry_t *cur_entry = NULL;
    int cur_memb_id = -1, entry_memb_id = -1;

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    cur_entry = ecmp_inf->current_lt_ent;

    cur_memb_id = \
        cur_entry->ecmp_nhop ?
            cur_entry->uecmp_id[idx] :
            cur_entry->nhop_id[idx];

    entry_memb_id = \
        lt_entry->ecmp_nhop ?
            lt_entry->uecmp_id[uidx] :
            lt_entry->nhop_id[uidx];

    if (cur_memb_id == entry_memb_id) {
        return true;
    }
    return false;
}

/*!
 * \brief Determine the type of operation being performed on the RH-ECMP group.
 *
 * This function determines the type of operation being performed on the
 * member/s of a RH-ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 * \param [out] rh_oper Resilient hashing ECMP group members operation type.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 */
static int
bcmecmp_group_rh_oper_get(int unit,
                          bcmecmp_lt_entry_t *lt_entry,
                          bcmecmp_rh_oper_t *rh_oper)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths; /* Group current NUM_PATHS value. */
    uint32_t idx = 0, uidx = 0; /* Next-hop members iterator variables. */
    uint32_t shared_nhop_cnt = 0; /* Shared NHOPs count. */
    uint32_t new_nhop_cnt = 0; /* New NHOPs count. */
    uint32_t alloc_sz = 0;/* Alloc size current and new. */
    int *nhop_arr = NULL; /* NHOP members array. */
    bool mindex_changed = FALSE; /* Member index has changed in NHOP array. */
    const char * const rh_op_str[BCMECMP_RH_OPER_COUNT] = BCMECMP_RH_OPER_NAME;
    int start_idx = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rh_oper, SHR_E_PARAM);

    COMPILER_REFERENCE(ecmp_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_rh_oper_get.\n")));

    if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
        rv = rh_ov_group_member_oper_get(unit, lt_entry, rh_oper);
        SHR_ERR_EXIT(rv);
    }

    ecmp_id = lt_entry->ecmp_id;

    /*
     * If there are no members in the RH-ECMP group, then there is no SW load
     * balancing operation to performed for this group in the UPDATE operation.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 && lt_entry->rh_num_paths == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "(RH_NUM_PATHS = %u): BCMECMP_RH_OPER_NONE.\n"),
                    lt_entry->rh_num_paths));

        *rh_oper = BCMECMP_RH_OPER_NONE;
        SHR_EXIT();
    }

    /*
     * This UPDATE operation is equivalent to members add via an INSERT
     * operation as all the members are newly added to this group. These new
     * members must be SW load balanced by using bcmecmp_group_rh_entries_set()
     * function and not expected to be handled in this function, return
     * SHR_E_INTERNAL error.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 && lt_entry->rh_num_paths > 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members to members (RH_NUM_PATHS = %u) rv=%s"
                                ".\n"),
                     lt_entry->rh_num_paths,
                     shr_errmsg(SHR_E_INTERNAL)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * If the RH-ECMP group previously had members and there are no members
     * in the group as part of this UPDATE operation, then set 'rh_oper' to
     * BCMECMP_RH_OPER_DELETE and return from this function. There are no
     * shared members between the old and new members for this group.
     */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) > 0 && lt_entry->rh_num_paths == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "(RH_NUM_PATHS = %u): BCMECMP_RH_OPER_DELETE.\n"),
                    lt_entry->rh_num_paths));

        *rh_oper = BCMECMP_RH_OPER_DELETE;
        SHR_EXIT();
    }

    /* Validate NUM_PATHS value before proceeding further in this function. */
    if (BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) == 0 || lt_entry->rh_num_paths == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unexpected RH_NUM_PATHS value: GNUM_PATHS=%u"
                                " RH_NUM_PATHS=%u.\n"),
                     BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                     lt_entry->rh_num_paths));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    alloc_sz = sizeof(*nhop_arr) * grp_npaths;

    if (alloc_sz) {
        BCMECMP_ALLOC(nhop_arr,
                      alloc_sz,
                      "bcmecmpRhOperGetNhopArr");
    }

    /*
     * Identify the shared members and the new members in the updated
     * next-hop members array.
     */
    if (nhop_arr) {
        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            for (idx = start_idx; idx < grp_npaths; idx++) {
                if ((nhop_arr[idx] != BCMECMP_INVALID)
                     && bcmecmp_group_rh_member_compare(unit,
                                                        lt_entry,
                                                        idx,
                                                        uidx)) {
                    /* Old NHOP also member of the updated NHOP array. */
                    shared_nhop_cnt++;
                    nhop_arr[idx] = BCMECMP_INVALID;
                    /*
                     * Check if member index has changed in the updated
                     * members array.
                     */
                    if (idx != uidx) {
                        mindex_changed = TRUE;
                    }
                    break;
                }
            }
            if (idx == grp_npaths) {
                new_nhop_cnt++;
            }
            while (nhop_arr[start_idx] == BCMECMP_INVALID) {
                start_idx++;
            }
        }
    }

    /* Determine the type of operation to be performed on RH group members. */
    if (lt_entry->rh_num_paths > grp_npaths) {
        if ((shared_nhop_cnt == grp_npaths) && !mindex_changed) {
            *rh_oper = BCMECMP_RH_OPER_ADD;
        } else {
            *rh_oper = BCMECMP_RH_OPER_ADD_REPLACE;
        }
    } else if (lt_entry->rh_num_paths < grp_npaths) {
        if (shared_nhop_cnt == lt_entry->rh_num_paths) {
            /*
             * This condition is TRUE when all the members passed in the
             * member/s delete-update operation are already members of this
             * group (Their mindex could have changed but they are still common
             * members between the NHOP members list).
             * This condition is also TRUE when all the members have been
             * deleted in the update operation i.e. 'lt_entry->num_paths == 0',
             * means there are no shared members in the group in this delete
             * operation. As shared_nhop_cnt default value is initialized to '0'
             * at the start of this function, This condition will be TRUE.
             */
            *rh_oper = BCMECMP_RH_OPER_DELETE;
        } else {
            *rh_oper = BCMECMP_RH_OPER_DELETE_REPLACE;
        }
    } else if (lt_entry->rh_num_paths == grp_npaths) {
        *rh_oper = BCMECMP_RH_OPER_REPLACE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\n\t[grp_npaths=%-4u rh_num_paths=%-4u"
                            "shared_nhop_cnt=%-4u\n"
                            "\t new_nhop_cnt=%-4u mindex_changed=%s "
                            "rh_oper=(%s)]\n"),
                 grp_npaths,
                 lt_entry->rh_num_paths,
                 shared_nhop_cnt,
                 new_nhop_cnt,
                 mindex_changed ? "TRUE" : "FALSE",
                 rh_op_str[*rh_oper]));

    exit:
        BCMECMP_FREE(nhop_arr);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes NUM_PATHS group members among RH_SIZE member table entries for
 * installation in ECMP hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_rh_entries_update(int unit,
                                bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_rh_oper_t rh_oper = BCMECMP_RH_OPER_NONE; /* RH oper type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_rh_entries_update.\n")));

    /* Get the type of RH-ECMP group operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_group_rh_oper_get(unit, lt_entry, &rh_oper));

    /* Invoke operation dependent function. */
    switch (rh_oper) {
        case BCMECMP_RH_OPER_NONE:
            SHR_EXIT();
            break;
        case BCMECMP_RH_OPER_ADD:
            if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (rh_ov_group_member_add(unit, lt_entry));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_rh_grp_members_add(unit, lt_entry));
            }
            break;
        case BCMECMP_RH_OPER_DELETE:
            if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (rh_ov_group_member_delete(unit, lt_entry));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_rh_grp_members_delete(unit, lt_entry));
            }
            break;
        case BCMECMP_RH_OPER_REPLACE:
            if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (rh_ov_group_member_replace(unit, lt_entry));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_rh_grp_members_replace(unit, lt_entry));
            }
            break;
        case BCMECMP_RH_OPER_ADD_REPLACE:
            if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (rh_ov_group_member_add_replace(unit, lt_entry));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_rh_grp_members_add_replace(unit, lt_entry));
            }
            break;
        case BCMECMP_RH_OPER_DELETE_REPLACE:
            if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (rh_ov_group_member_delete_replace(unit, lt_entry));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                     (bcmecmp_rh_grp_members_delete_replace(unit, lt_entry));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Resilient hash ECMP group members data print debug function.
 *
 * Resilient hash ECMP group members data print debug function.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_id Resilient hash group identifier.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Invalid resilient hash group structure pointer value.
 */
static int
bcmecmp_rh_grp_membs_data_print(int unit,
                                bcmecmp_id_t ecmp_id,
                                bcmecmp_lt_entry_t *rhg_entry)
{
    uint32_t idx; /* Index iterator variable. */
    shr_pb_t *pb = NULL; /* Print buffer. */

    SHR_FUNC_ENTER(unit);

    /* Validate input parameter and pointers to be accessed in this function. */
    SHR_NULL_CHECK(rhg_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rhg_entry->rh_entries_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMECMP_GRP_RHG_PTR(unit, ecmp_id), SHR_E_INTERNAL);
    SHR_NULL_CHECK(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id), SHR_E_INTERNAL);
    SHR_NULL_CHECK(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id), SHR_E_INTERNAL);
    SHR_NULL_CHECK(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   SHR_E_INTERNAL);

    /* Allocate the print buffer. */
    pb = shr_pb_create();

    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb,
                  "\tECMP_ID=%-4d RH_ENTRIES_ARR[rh_entries_cnt==%-4d]:\n",
                  ecmp_id,
                  BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id));
    shr_pb_printf(pb, "\t=================================================");
    for (idx = 0; idx < BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id); idx++) {
        if (!(idx % 3)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      "%4d) %5d / midx=%-4d",
                      idx,
                      BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id)[idx],
                      BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx]);
    }
    shr_pb_printf(pb, "\n\n");

    shr_pb_printf(pb, "\t[npaths=%-4d sort=%d]:\n",
                  BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                  BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id));
    shr_pb_printf(pb, "\t=====================================\n");
    for (idx = 0; idx < BCMECMP_GRP_NUM_PATHS(unit, ecmp_id); idx++) {
        shr_pb_printf(pb,
                      "\t%4d) [NHOP_ID=Cur:%-5d / RH_ENT_CNT=Cur:%-4u]\n",
                      idx,
                      BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id) ?
                            BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[idx]
                            : BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[idx],
                      BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]);
    }
    shr_pb_printf(pb, "\n");

    shr_pb_printf(pb, "\tRetrieved RHG memebers yet to be mapped.\n");
    shr_pb_printf(pb, "\t=====================================");
    for (idx = 0; idx < rhg_entry->rh_entries_cnt; idx++) {
        if (!(idx % 3)) {
            shr_pb_printf(pb, "\n\t");
        }
        shr_pb_printf(pb,
                      "%4d) %5d",
                      idx,
                      rhg_entry->rh_entries_arr[idx] != BCMECMP_INVALID
                        ? rhg_entry->rh_entries_arr[idx] : BCMECMP_INVALID);
    }
    shr_pb_printf(pb, "\n\n");


    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s"), shr_pb_str(pb)));

    /* Free print buffer. */
    shr_pb_destroy(pb);
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Reconstruct RH-ECMP group mebers load balanced state.
 *
 * This function reconstructs RH-ECMP group members load balanced state post
 * warm start sequence to support subsequent members update operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_rh_grp_members_recover(int unit,
                               bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_lt_entry_t *rhg_entry = NULL; /* Resilient hash group entry. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    uint32_t idx = 0, memb_idx;      /* Index iterator variables. */
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    const int *memb_arr = NULL;      /* Group next-hop members array. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));

    if (rh_is_ov_group_member(unit, lt_entry->grp_type)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (rh_ov_group_members_recover(unit, lt_entry));
        SHR_EXIT();
    }

    /*
     * Allocate memory for resilient hashing group entry for performing the get
     * operation.
     */
    BCMECMP_ALLOC(rhg_entry, sizeof(*rhg_entry), "bcmecmpBcmRhgEntry");
    BCMECMP_ALLOC(rhg_entry->rh_entries_arr,
                  sizeof(*(rhg_entry->rh_entries_arr))
                  * lt_entry->rh_entries_cnt, "bcmecmpBcmRhgRhEntArr");
    /*
     * Setup input attribute values necessary for reading the RH group hardware
     * table entries.
     */
    rhg_entry->glt_sid = lt_entry->glt_sid;
    rhg_entry->op_arg = lt_entry->op_arg;
    rhg_entry->grp_type = lt_entry->grp_type;
    rhg_entry->ecmp_id = lt_entry->ecmp_id;
    ecmp_id = rhg_entry->ecmp_id;

    /* Get RH-ECMP Group entry from hardware tables. */
    SHR_IF_ERR_EXIT
        (BCMECMP_FNCALL_EXEC(unit, rh_grp_get)(unit, rhg_entry));

    lbound = (BCMECMP_GRP_RH_ENTRIES_CNT
                    (unit, ecmp_id) / BCMECMP_GRP_NUM_PATHS(unit, ecmp_id));
    ubound = ((BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) %
                    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id))
                    ? (lbound + 1) : lbound);
    threshold = ubound;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\tLBND=%u UBND=%u THRSLD=%u LB_MODE=%u "
                            "rh_entries_cnt=%u mstart_idx=%d\n"),
                lbound,
                ubound,
                threshold,
                rhg_entry->lb_mode,
                rhg_entry->rh_entries_cnt,
                rhg_entry->mstart_idx));

    /*
     * Verify retrieved total RH group member table entries count matches
     * stored/expected value before further processing for this RH group.
     */
    if (rhg_entry->rh_entries_cnt
        != BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for RHG members structure. */
    BCMECMP_ALLOC(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                  sizeof(*BCMECMP_GRP_RHG_PTR(unit, ecmp_id)),
                  "bcmecmpRhGrpMembRcvrRhgInfo");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id),
                      sizeof(*BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)),
                      "bcmecmpRhGrpMembRcvrRhgInfoBk");
    }

    /* Allocate memory to store RHG member table entries. */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                  (rhg_entry->rh_entries_cnt *
                   sizeof(*BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))),
                  "bcmecmpRhGrpMembRcvrRhEntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                      (rhg_entry->rh_entries_cnt *
                       sizeof(*BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))),
                      "bcmecmpRhGrpMembRcvrRhEntArrBk");
    }

    /* Copy load balanced RH members array. */
    sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
               rhg_entry->rh_entries_arr,
               sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
               rhg_entry->rh_entries_cnt);
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                   rhg_entry->rh_entries_arr,
                   sizeof(*(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))) *
                   rhg_entry->rh_entries_cnt);
    }

    /*
     * Allocate memory to store group RH-ECMP entries mapped member
     * index information.
     */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                  (rhg_entry->rh_entries_cnt *
                   sizeof(*(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMindexArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                      (rhg_entry->rh_entries_cnt *
                       sizeof(*(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMindexArrBk");
    }

    /*
     * Allocate memory to store RH-ECMP members entries usage count
     * information.
     */
    BCMECMP_ALLOC(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                  (BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) *
                   sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMembEntCntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                      (BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) *
                       sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMembEntCntArrBk");
    }

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    if (rhg_entry->ecmp_nhop) {
        memb_arr = BCMECMP_GRP_UECMP_PTR(unit, ecmp_id);
    } else {
        memb_arr = BCMECMP_GRP_NHOP_PTR(unit, ecmp_id);
    }

    /* Member array pointer must be valid else return error. */
    SHR_NULL_CHECK(memb_arr, SHR_E_INTERNAL);

    for (memb_idx = 0;
         memb_idx < BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->rh_entries_cnt; idx++) {
            if (rhg_entry->rh_entries_arr[idx] != BCMECMP_INVALID
                && memb_arr[memb_idx] == rhg_entry->rh_entries_arr[idx]
                && BCMECMP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < lbound) {
                /* Update member index mapping */
                BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[idx] = BCMECMP_INVALID;

                /* Move on to the next member as lower bound as been reached. */
                if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
                    == lbound) {
                    break;
                }
            }
        }
    }

    /*
     * For remaining members compare the upper limit and update the member index
     * and member entries usage count values.
     */
    for (memb_idx = 0;
         memb_idx < BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->rh_entries_cnt; idx++) {
            if (rhg_entry->rh_entries_arr[idx] != BCMECMP_INVALID
                && memb_arr[memb_idx] == rhg_entry->rh_entries_arr[idx]
                && BCMECMP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < ubound) {
                /* Update member index mapping */
                BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[idx] = BCMECMP_INVALID;

                /* Move on to the next member as upper bound as been reached. */
                if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
                    == ubound) {
                    break;
                }
            }
        }
    }

    if (BCMECMP_ATOMIC_TRANS(unit)) {
        /*
         * Copy group entries mapped member index array elements into the backup
         * array.
         */
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) *
                    rhg_entry->rh_entries_cnt));

        /*
         * Copy members entry usage count array elements into the backup
         * array.
         */
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                   BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) *
                    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id)));
    }

    /* RH-Group members data print debug function. */
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "rv=%d"),
                bcmecmp_rh_grp_membs_data_print(unit, ecmp_id, rhg_entry)));

    exit:
        if (rhg_entry) {
            BCMECMP_FREE(rhg_entry->rh_entries_arr);
            BCMECMP_FREE(rhg_entry);
        }
        if (SHR_FUNC_ERR() && ecmp_id != BCMECMP_INVALID) {
            if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
            }

            if (BCMECMP_ATOMIC_TRANS(unit)
                && BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
            }
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Set NHOP_ID[]/ECMP_UNDERLAY_ID[] elements at unspecified indices.
 *
 * Set unspecifed NHOP_ID[]/ECMP_UNDERLAY_ID[] array element values to default
 * value, based on the logical table entry's group type.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_add_member_fields_set(int unit,
                                    bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    uint32_t idx; /* Index iterator. */
    uint32_t def_max_paths = 0;
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_lt_field_attrs_t * lt_fattr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_group_add_member_fields_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;
    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);

    if (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
        def_max_paths = BCMECMP_LT_MAX_PATHS(unit, group, gtype);
    } else {
        def_max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
    }

    /*
     * Check the group type and update NHOP_ID[] or ECMP_UNDERLAY_ID[] array
     * elements with default/min value if a value has not been specified by
     * the end-user for this field.
     */
    if (bcmecmp_grp_is_overlay(gtype) &&
            ecmp_ctrl->ecmp_info.ovlay_memb_count) {
        lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl->ecmp_info.group[gtype].lt_fattrs;
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID))
              || (lt_entry->o_nhop_ids_count < def_max_paths)) {
            for (idx = lt_entry->o_nhop_ids_count; idx < def_max_paths;
                 idx++) {
                lt_entry->o_nhop_id[idx] = lt_fattr->min_o_nhop_id;
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP))
              || (lt_entry->u_ecmp_nhop_count < def_max_paths)) {
            for (idx = 0; idx < def_max_paths; idx++) {
                if (lt_entry->u_ecmp_nhop[idx] == (uint8_t)BCMECMP_INVALID) {
                    lt_entry->u_ecmp_nhop[idx] = 0;
                }
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < def_max_paths)) {
            for (idx = 0; idx < def_max_paths; idx++) {
                if (lt_entry->u_ecmp_nhop[idx]) {
                    continue;
                }
                if (lt_entry->nhop_id[idx] == BCMECMP_INVALID) {
                    lt_entry->nhop_id[idx] = lt_fattr->min_nhop_id;
                }
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < def_max_paths)) {
            for (idx = 0; idx < def_max_paths; idx++) {
                if (!lt_entry->u_ecmp_nhop[idx]) {
                    continue;
                }
                if (lt_entry->uecmp_id[idx] == BCMECMP_INVALID) {
                    lt_entry->uecmp_id[idx] = lt_fattr->min_uecmp_id;
                }
            }
        }
    } else if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < def_max_paths)) {
            /* Set unspecified ECMP_UNDERLAY_ID[] fields to default value. */
            for (idx = lt_entry->uecmp_ids_count; idx < def_max_paths;
                 idx++) {
                lt_entry->uecmp_id[idx] = BCMECMP_LT_MIN_UECMP_ID
                                                (unit, group, gtype);
            }
        }
    } else {
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < def_max_paths)) {
            /*
             * Set unspecified NHOP_ID[] fields to default value based on the
             * group type (LT).
             */
            for (idx = lt_entry->nhop_ids_count; (idx < def_max_paths)
                                    && (idx < BCMECMP_NHOP_ARRAY_SIZE);
                 idx++) {
                lt_entry->nhop_id[idx] = BCMECMP_LT_MIN_NHOP_ID
                                                (unit, group, gtype);
            }
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Set NHOP_ID[]/ECMP_UNDERLAY_ID[] elements at unspecified indices.
 *
 * Set unspecified NHOP_ID[]/ECMP_UNDERLAY_ID[] array element values from the
 * ECMP group database.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_update_member_fields_set(int unit,
                                       bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;            /* Group ECMP_ID. */
    uint32_t idx; /* Index iterator. */
    uint32_t max_paths = 0;
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_lt_field_attrs_t * lt_fattr = NULL;
    bcmecmp_lt_entry_t *current_lt_ent = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    COMPILER_REFERENCE(ecmp_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_update_member_fields_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;
    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    if (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
        max_paths = lt_entry->max_paths;
    } else {
        max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
    }

    /*
     * Check the group type and update NHOP_ID[] or ECMP_UNDERLAY_ID[] array
     * elements with the stored value, if a value has not been specified by
     * the end-user for this field.
     */
    if (bcmecmp_grp_is_overlay(gtype) &&
            ecmp_ctrl->ecmp_info.ovlay_memb_count) {
        lt_fattr = (bcmecmp_lt_field_attrs_t *)
                       ecmp_ctrl->ecmp_info.group[gtype].lt_fattrs;
        current_lt_ent =
                      (bcmecmp_lt_entry_t *)ecmp_ctrl->ecmp_info.current_lt_ent;
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_O_NHOP_ID))
              || (lt_entry->o_nhop_ids_count < max_paths)) {
            for (idx = 0; idx < max_paths; idx++) {
                if (lt_entry->o_nhop_id[idx] == BCMECMP_INVALID) {
                    if (current_lt_ent->o_nhop_id[idx] == BCMECMP_INVALID) {
                        lt_entry->o_nhop_id[idx] = lt_fattr->min_o_nhop_id;
                    } else {
                        lt_entry->o_nhop_id[idx] =
                                                 current_lt_ent->o_nhop_id[idx];
                    }
                }
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP))
              || (lt_entry->u_ecmp_nhop_count < max_paths)) {
            for (idx = 0; idx < max_paths; idx++) {
                if (lt_entry->u_ecmp_nhop[idx] == (uint8_t)BCMECMP_INVALID) {
                    if (current_lt_ent->u_ecmp_nhop[idx] == (uint8_t)BCMECMP_INVALID) {
                        lt_entry->u_ecmp_nhop[idx] = 0;
                    } else {
                        lt_entry->u_ecmp_nhop[idx] =
                            current_lt_ent->u_ecmp_nhop[idx];
                    }
                }
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < max_paths)) {
            for (idx = 0; idx < max_paths; idx++) {
                if (lt_entry->u_ecmp_nhop[idx]) {
                    continue;
                }
                if (lt_entry->nhop_id[idx] == BCMECMP_INVALID) {
                    if (current_lt_ent->nhop_id[idx] == BCMECMP_INVALID) {
                        lt_entry->nhop_id[idx] = lt_fattr->min_nhop_id;
                    } else {
                        lt_entry->nhop_id[idx] = current_lt_ent->nhop_id[idx];
                    }
                }
            }
        }
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < max_paths)) {
            for (idx = 0; idx < max_paths; idx++) {
                if (!lt_entry->u_ecmp_nhop[idx]) {
                    continue;
                }
                if (lt_entry->uecmp_id[idx] == BCMECMP_INVALID) {
                    if (current_lt_ent->uecmp_id[idx] == BCMECMP_INVALID) {
                        lt_entry->uecmp_id[idx] = lt_fattr->min_uecmp_id;
                    } else {
                        lt_entry->uecmp_id[idx] = current_lt_ent->uecmp_id[idx];
                    }
                }
            }
        }
    } else if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
        /*
         * If ECMP_UNDERLAY_ID[] field values are not specified (or) if the
         * total count of ECMP_UNDERLAY_ID[] fields set is less than the
         * group's MAX_PATHS value, then for unspecified ECMP_UNDERLAY_ID[]
         * array elements, set the values from the group's stored database.
         */
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_UECMP_ID))
              || (lt_entry->uecmp_ids_count < max_paths)) {
            for (idx = 0;
                 idx < max_paths;
                 idx++) {
                if (lt_entry->uecmp_id[idx] == BCMECMP_INVALID) {
                   if (BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[idx]
                                              == BCMECMP_INVALID) {
                       lt_entry->uecmp_id[idx] = BCMECMP_LT_MIN_UECMP_ID
                                                           (unit, group, gtype);
                   } else {
                       lt_entry->uecmp_id[idx] = (BCMECMP_GRP_UECMP_PTR(unit,
                                                                 ecmp_id)[idx]);
                   }
                }
            }
        }
    } else {
        /*
         * If NHOP_ID[] field values are not specified (or) if the total count
         * of NHOP_ID[] fields set is less than the group's MAX_PATHS value,
         * then for unspecified NHOP_ID[] array elements, set the values from
         * the group's stored database.
         */
        if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_ID))
              || (lt_entry->nhop_ids_count < max_paths)) {
            for (idx = 0;
                 idx < max_paths;
                 idx++) {
                if (lt_entry->nhop_id[idx] == BCMECMP_INVALID) {
                   if (BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[idx]
                                              == BCMECMP_INVALID) {
                       lt_entry->nhop_id[idx] = BCMECMP_LT_MIN_NHOP_ID
                                                           (unit, group, gtype);
                   } else {
                       lt_entry->nhop_id[idx] = (BCMECMP_GRP_NHOP_PTR(unit,
                                                                 ecmp_id)[idx]);
                   }
                }
            }
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

static int
bcmecmp_group_add_weight_fields_set(int unit,
                                    bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    uint32_t idx;
    uint32_t def_max_paths = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    gtype = lt_entry->grp_type;

    if (!BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_entry)) {
        SHR_EXIT();
    }

    def_max_paths = BCMECMP_LT_MAX_PATHS(unit, group, gtype);

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT) ||
        (lt_entry->weight_count < def_max_paths)) {
        for (idx = lt_entry->weight_count; idx < def_max_paths; idx++) {
            lt_entry->weight[idx] = BCMECMP_LT_MIN_WEIGHT(unit, group, gtype);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmecmp_group_update_weight_fields_set(int unit,
                                       bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT;
    uint32_t idx;
    uint32_t max_paths = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    gtype = lt_entry->grp_type;

    if (!BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_entry)) {
        SHR_EXIT();
    }

    max_paths = lt_entry->max_paths;

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHT) ||
        (lt_entry->weight_count < max_paths)) {
        for (idx = 0; idx < max_paths; idx++) {
            if (lt_entry->weight[idx] == (uint32_t)BCMECMP_INVALID) {
                if (BCMECMP_GRP_WEIGHT_PTR(unit, ecmp_id)[idx] ==
                    (uint32_t)BCMECMP_INVALID) {
                    lt_entry->weight[idx] = BCMECMP_LT_MIN_WEIGHT
                                                (unit, group, gtype);
                } else {
                    lt_entry->weight[idx] = (BCMECMP_GRP_WEIGHT_PTR
                                                (unit, ecmp_id)[idx]);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief If following free entries are available in member table when resize group, use them
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_RESOURCE No hw resource error.
 */
static int
bcmecmp_group_in_place_members_set(int unit,
                                    bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;            /* Group ECMP_ID. */
    int idx; /* Index iterator. */
    int new_max_paths;
    int old_max_paths;
    int old_mstart_idx;
    uint32_t idx_width = 1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcmecmp_group_in_place_members_check.\n")));

    gtype = lt_entry->grp_type;
    ecmp_id = lt_entry->ecmp_id;
    new_max_paths = lt_entry->max_paths;
    old_max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
    old_mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

    if (BCMECMP_USE_INDEX_REMAP(unit)) {
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, lt_entry)) {
            idx_width = BCMECMP_FNCALL_EXEC(unit, member_weight_index_width)();
        }
    }

    for (idx = old_mstart_idx + old_max_paths * (int)idx_width;
         (idx < (old_mstart_idx + new_max_paths * (int)idx_width)) &&
            (idx <= BCMECMP_TBL_IDX_MAX(unit, member, gtype));
          idx++) {
        if (BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, member, gtype), idx)) {
            break;
        }
    }

    if (idx == old_mstart_idx + new_max_paths * (int)idx_width) {
        SHR_IF_ERR_EXIT
        (bcmecmp_tbl_ref_cnt_decr(BCMECMP_TBL_PTR(unit, member, gtype),
             old_mstart_idx, old_max_paths, idx_width));
        SHR_IF_ERR_EXIT
        (bcmecmp_tbl_ref_cnt_incr(BCMECMP_TBL_PTR(unit, member, gtype),
             old_mstart_idx, new_max_paths, idx_width));
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        SHR_EXIT();
    }

    SHR_ERR_EXIT(SHR_E_RESOURCE);
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Find the group corresponding to the given member start index.
 *
 * This function returns the group that points to
 * the given member index.
 *
 * \param [in] unit Unit number.
 * \param [in] gtype group type
 * \param [in] mstart_idx starting index of member block
 *
 * \return group id or BCM_E_NOT_FOUND if not found.
 */
static int
bcmecmp_find_group(int unit,
                   bcmecmp_grp_type_t gtype,
                   uint32_t mstart_idx)
{
    int ecmp_id, gstart_idx, gend_idx, rv;
    bool in_use = FALSE;
    uint32_t comp_id = 0;
    uint32_t temp_gtype = 0;
    uint32_t weight_mode = 0;
    bcmecmp_control_t *ecmp_ctrl_ptr = NULL;
    bcmecmp_info_t *ecmp_info;
    bcmecmp_grp_attr_t *grp_attr = NULL;
    fn_shr_grp_type_get_t shr_grp_func = NULL;

    ecmp_ctrl_ptr = bcmecmp_get_ecmp_control(unit);
    ecmp_info = &(ecmp_ctrl_ptr->ecmp_info);

    gstart_idx = ecmp_info->group[gtype].imin;
    gend_idx = ecmp_info->group[gtype].imax;
    if (bcmecmp_grp_is_overlay(gtype)) {
        gstart_idx += ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY].imax + 1;
        gend_idx += ecmp_info->group[BCMECMP_GRP_TYPE_UNDERLAY].imax + 1;
    }
    for (ecmp_id = gstart_idx; ecmp_id <= gend_idx; ecmp_id++) {
        /* Verify the group is in use. */
        rv = bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use,
                                &comp_id);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        if (!in_use || (comp_id != BCMMGMT_ECMP_COMP_ID)) {
            continue;
        }

        grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr) + ecmp_id;
        temp_gtype = grp_attr->grp_type;
        weight_mode = grp_attr->weighted_mode;
        if (bcmecmp_grp_is_member_replication(temp_gtype, weight_mode)) {
            if ((shr_grp_func = ecmp_ctrl_ptr->ecmp_drv->shr_grp_type_get)) {
            /* MEMBER_REPLICATION member is configured in member ECMP view. */
                temp_gtype = shr_grp_func(temp_gtype);
            }
        }

        if (grp_attr->mstart_idx == (int)mstart_idx &&
            temp_gtype == gtype) {
            return ecmp_id;
        }
    }

    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Defragment ECMP group member table entries.
 *
 * This function performs defragmentation of ECMP member table entries used by
 * ECMP flexible groups, to create space for a new group.
 * Group type input parameter of
 * this function determines which portion of the member table
 * i.e. Overlay or Underlay portion is to be defragmented.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 * \param [in] req_size requested max block size.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 * \return SHR_E_RESOURCE Resource error.
 */

static int
bcmecmp_itbm_groups_defragment(int unit,
                               const bcmecmp_lt_entry_t *entry,
                               uint32_t req_size)
{
    shr_itbm_blk_t *itbm_blk = NULL;
    uint32_t tot_wal_mentries = 0; /* Total member table entries moved. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    int i, j, rv = SHR_E_NONE;
    shr_pb_t *mpb = NULL; /* Member table debug info print buffer. */
    bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    shr_itbm_defrag_blk_mseq_t *m_seq;
    uint32_t itbm_blk_count;
    bcmecmp_grp_defrag_t m_src, m_dest;
    uint32_t hw_ecmp_id;
    bcmecmp_control_t *ecmp_ctrl_ptr = NULL;
    bcmecmp_info_t *ecmp_info = NULL;
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_grp_attr_t *grp_attr;
    bcmecmp_hw_entry_attr_t *hw_ent_attr;
    uint32_t weight_mode = BCMECMP_WEIGHTED_MODE_COUNT;
    fn_shr_grp_type_get_t shr_grp_func = NULL;
    shr_itbm_list_hdl_t rev_lhdl = NULL;
    shr_itbm_blk_t fblk, rev_fblk;
    bcmecmp_grp_type_t shr_gtype; /* Shared group type. */
    uint32_t largest_cnt; /* Largest block's elements count. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    ecmp_ctrl_ptr = bcmecmp_get_ecmp_control(unit);
    SHR_NULL_CHECK(ecmp_ctrl_ptr, SHR_E_INTERNAL);
    ecmp_info = &(ecmp_ctrl_ptr->ecmp_info);
    /* Get the group type. */
    gtype = entry->grp_type;

    /* Get member table pointer. */
    SHR_NULL_CHECK(ecmp_info, SHR_E_INTERNAL);
    mtbl_ptr = &(ecmp_info->member[gtype]);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\tgtype=%u G_tbl_sz=%u.\n"),
              gtype,
              ecmp_info->group[gtype].tbl_size));

    SHR_NULL_CHECK(ecmp_ctrl_ptr->ecmp_drv, SHR_E_INTERNAL);
    if ((shr_grp_func = ecmp_ctrl_ptr->ecmp_drv->shr_grp_type_get)) {
        shr_gtype = shr_grp_func(gtype);
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, entry)) {
            /* FWD list handle. */
            list_hdl = ecmp_info->member[shr_gtype].list_hdl;
            /* Rev list handle. */
            rev_lhdl = ecmp_info->member[gtype].list_hdl;
        } else {
            /* FWD list handle. */
            list_hdl = ecmp_info->member[gtype].list_hdl;
            /* Rev list handle. */
            rev_lhdl = ecmp_info->member[shr_gtype].list_hdl;
        }
        shr_itbm_blk_t_init(&fblk);
        shr_itbm_blk_t_init(&rev_fblk);
        rv = shr_itbm_list_defrag_free_block_get(list_hdl,
                                                 rev_lhdl,
                                                 &fblk,
                                                 &rev_fblk);
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "fwd(gtype=%u): epbkt=%u bkt=%d first=%u, "
                              "ecnt=%u comp_id=%d\n rev(gtype=%u): epbkt=%u "
                              "bkt=%d first=%u ecnt=%u comp_id=%d\n"),
                   gtype, fblk.entries_per_bucket, (int)fblk.bucket,
                   fblk.first_elem, fblk.ecount, (int)fblk.comp_id,
                   shr_grp_func(gtype), rev_fblk.entries_per_bucket,
                   (int)rev_fblk.bucket, rev_fblk.first_elem, rev_fblk.ecount,
                   (int)rev_fblk.comp_id));
        if (BCMECMP_LT_GRP_IS_MEMBER_WEIGHT(unit, entry)) {
            largest_cnt = rev_fblk.ecount;
        } else {
            largest_cnt = fblk.ecount;
        }

        if (rv == SHR_E_NOT_FOUND
            || (SHR_SUCCESS(rv) && largest_cnt < req_size)) {
            SHR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        SHR_IF_ERR_EXIT(rv);
    }

    sal_memset(ecmp_info->wali, 0, sizeof(*(ecmp_info->wali)));
    mpb = shr_pb_create();
    shr_pb_printf(mpb,
                  "\n\tECMP_ID\tmstart_idx(s/d)\t SG.sz ==> DG.sz(Gap)\tgtype"
                  "\tSID\n");
    shr_pb_printf(mpb,
                  "\n\t=======\t===============\t=====================\t====="
                  "\t===\n");

    list_hdl = ecmp_info->member[gtype].list_hdl;
    if (BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        weight_mode = BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_itbm_list_defrag_start(list_hdl, &itbm_blk_count, &itbm_blk,
                                    &m_seq, NULL, NULL, NULL));

    bcmecmp_grp_defrag_t_init(&m_src);
    m_src.op_arg = entry->op_arg;
    m_src.glt_sid = entry->glt_sid;
    m_src.gtype = gtype;
    bcmecmp_grp_defrag_t_init(&m_dest);
    m_dest.op_arg = entry->op_arg;
    m_dest.glt_sid = entry->glt_sid;
    m_dest.gtype = gtype;
    for (i = 0; i < (int)itbm_blk_count; i++) {
        m_src.mstart_idx = bcmecmp_itbm_mstart_index_get
                               (unit, gtype, weight_mode,
                                itbm_blk[i].bucket,
                                itbm_blk[i].first_elem);
        m_src.gsize = itbm_blk[i].ecount;
        m_src.ecmp_id = bcmecmp_find_group(unit, gtype, m_src.mstart_idx);
        if (m_src.ecmp_id < 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_defrag_end(list_hdl, false));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        m_dest.ecmp_id = m_src.ecmp_id;
        for (j = 0; j < (int)(m_seq[i].mcount); j++) {
            /*Move to first_elem[j]*/
            m_dest.mstart_idx = bcmecmp_itbm_mstart_index_get
                                    (unit, gtype, weight_mode,
                                     m_seq[i].bucket[j],
                                     m_seq[i].first_elem[j]);
            m_dest.gsize = m_seq[i].ecount;

            rv = bcmecmp_groups_defrag_oper(unit,
                                            mpb,
                                            gtype,
                                            m_dest.gsize,
                                            &m_src,
                                            &m_dest);
            tot_wal_mentries += m_dest.gsize;

            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (shr_itbm_list_defrag_end(list_hdl, false));
                return rv;
            }

            m_src.mstart_idx = m_dest.mstart_idx;
            m_src.gsize = m_dest.gsize;
        }
        hw_ecmp_id = m_src.ecmp_id;
        if (list_hdl) {
            hw_ent_attr =
                (bcmecmp_hw_entry_attr_t *)(ecmp_info->group[gtype].ent_arr)
                + hw_ecmp_id;
            hw_ent_attr->flags |= BCMECMP_ENTRY_DEFRAG_STAGED;
            grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr) + hw_ecmp_id;
            grp_attr->trans_id = entry->op_arg->trans_id;

            if (ecmp_ctrl_ptr->atomic_trans) {
                hw_ent_attr =
                    (bcmecmp_hw_entry_attr_t *)
                    (ecmp_info->group[gtype].ent_bk_arr)
                    + hw_ecmp_id;
                hw_ent_attr->flags |= BCMECMP_ENTRY_DEFRAG_STAGED;
                grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr)
                           + hw_ecmp_id;
                grp_attr->trans_id = entry->op_arg->trans_id;
            }
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_itbm_list_defrag_end(list_hdl, true));

    if ((shr_grp_func = ecmp_ctrl_ptr->ecmp_drv->shr_grp_type_get)) {
        gtype = shr_grp_func(gtype);
        list_hdl = ecmp_info->member[gtype].list_hdl;
        if (BCMECMP_GRP_IS_WEIGHTED(gtype)) {
            weight_mode = BCMECMP_WEIGHTED_MODE_MEMBER_WEIGHT;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_itbm_list_defrag_start(list_hdl, &itbm_blk_count, &itbm_blk,
                                        &m_seq, NULL, NULL, NULL));
        m_src.gtype = gtype;
        m_dest.gtype = gtype;
        for (i = 0; i < (int)itbm_blk_count; i++) {
            m_src.mstart_idx = bcmecmp_itbm_mstart_index_get
                                   (unit, gtype, weight_mode,
                                    itbm_blk[i].bucket,
                                    itbm_blk[i].first_elem);
            m_src.gsize = itbm_blk[i].ecount;
            m_src.ecmp_id = bcmecmp_find_group(unit, gtype, m_src.mstart_idx);
            if (m_src.ecmp_id < 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (shr_itbm_list_defrag_end(list_hdl, false));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            m_dest.ecmp_id = m_src.ecmp_id;
            for (j = 0; j < (int)(m_seq[i].mcount); j++) {
                /* Move to m_seq[j] */
                m_dest.mstart_idx = bcmecmp_itbm_mstart_index_get
                                        (unit, gtype, weight_mode,
                                         m_seq[i].bucket[j],
                                         m_seq[i].first_elem[j]);
                m_dest.gsize = m_seq[i].ecount;

                rv = bcmecmp_groups_defrag_oper(unit,
                                                mpb,
                                                gtype,
                                                m_dest.gsize,
                                                &m_src,
                                                &m_dest);
                tot_wal_mentries += m_dest.gsize;

                if (SHR_FAILURE(rv)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (shr_itbm_list_defrag_end(list_hdl, false));
                    return rv;
                }

                m_src.mstart_idx = m_dest.mstart_idx;
                m_src.gsize = m_dest.gsize;
            }
            hw_ecmp_id = m_src.ecmp_id;
            if (list_hdl) {
                hw_ent_attr =
                   (bcmecmp_hw_entry_attr_t *)(ecmp_info->group[gtype].ent_arr)
                   + hw_ecmp_id;
                hw_ent_attr->flags |= BCMECMP_ENTRY_DEFRAG_STAGED;
                grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr)
                           + hw_ecmp_id;
                grp_attr->trans_id = entry->op_arg->trans_id;

                if (ecmp_ctrl_ptr->atomic_trans) {
                    hw_ent_attr =
                        (bcmecmp_hw_entry_attr_t *)
                        (ecmp_info->group[gtype].ent_bk_arr)
                        + hw_ecmp_id;
                    hw_ent_attr->flags |= BCMECMP_ENTRY_DEFRAG_STAGED;
                    grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_bk_arr)
                               + hw_ecmp_id;
                    grp_attr->trans_id = entry->op_arg->trans_id;
                }
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (shr_itbm_list_defrag_end(list_hdl, true));
    }

exit:
    if (mpb) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "rv=%d.\n"),
                 bcmecmp_wali_print(unit, tot_wal_mentries, mpb)));
        shr_pb_destroy(mpb);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief update the max_paths of ecmp group.
 *
 * Update the ref-count of reserved entries in member table and member start
 * idx in group:
 * If free entries are available in member table, use them;
 * If free entries are not enough, defragment the groups and try again.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_RESOURCE if membership table has no more space
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_group_max_paths_update(int unit,
                               bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;            /* Group ECMP_ID. */
    uint32_t new_max_paths;
    uint32_t old_max_paths;
    int old_mstart_idx;
    int new_mstart_idx;
    bcmecmp_tbl_op_t tbl_op; /* Table operation info. */
    int rv;                  /* Return value. */
    shr_pb_t *mpb = NULL;
    bcmecmp_grp_defrag_t src_block;  /* Fragmented Group info. */
    bcmecmp_grp_defrag_t dst_block; /* Gap fragment info. */
    uint32_t idx, idx_width = 1;
    bool min2max = TRUE;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_grp_attr_t *grp_attr = NULL;
    uint32_t bucket = 0, first = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_max_paths_update.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;
    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    list_hdl = ecmp_info->member[gtype].list_hdl;
    if (bcmecmp_grp_is_member_replication(gtype, lt_entry->weighted_mode)) {
        list_hdl = ecmp_info->member[gtype].list_hdl_1;
    }

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;
    grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr) + ecmp_id;

    new_max_paths = lt_entry->max_paths;
    old_max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
    old_mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

    if (new_max_paths < old_max_paths) {
        if (list_hdl) {
            bcmecmp_itbm_bucket_first_get(unit, gtype, lt_entry->weighted_mode,
                                          old_mstart_idx, &bucket, &first);
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_resize(list_hdl, old_max_paths,
                                            bucket, first, new_max_paths));
        } else {
            /*
             * Decrement Member table entries reference count by MAX_PATHS
             * value of the group.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_decr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     old_mstart_idx,
                     old_max_paths,
                     idx_width));
            /*
             * Increment Member table entries reference count by NEW MAX_PATHS
             * value of the group.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_incr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     old_mstart_idx,
                     new_max_paths,
                     idx_width));
        }
        bcmecmp_grp_defrag_t_init(&src_block);
        src_block.ecmp_id = lt_entry->ecmp_id;
        src_block.op_arg = lt_entry->op_arg;
        src_block.glt_sid = lt_entry->glt_sid;
        src_block.gtype = lt_entry->grp_type;
        src_block.mstart_idx = old_mstart_idx + new_max_paths;
        src_block.gsize = old_max_paths - new_max_paths;

        /* Clear the unused member block from new_max_paths to old_max_paths. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_member_clear(unit, &src_block));

        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        SHR_EXIT();
    }

    if (new_max_paths > old_max_paths) {
         /* Check if it is possible to increase max grp size in-place. */
        if (list_hdl) {
            bcmecmp_itbm_bucket_first_get(unit, gtype,
                                          lt_entry->weighted_mode,
                                          old_mstart_idx, &bucket, &first);
            rv = shr_itbm_list_block_resize(list_hdl, old_max_paths, bucket,
                                            first, new_max_paths);
            if (rv == SHR_E_NONE) {
                grp_attr->max_paths = new_max_paths;
            }
        } else {
            rv = bcmecmp_group_in_place_members_set(unit, lt_entry);
        }

        if (SHR_E_NONE == rv) {
            SHR_EXIT();
        } else if (rv != SHR_E_RESOURCE) {
            /* return E_RESOURCE if the underlying membership table is full */
            if (rv == SHR_E_FULL) {
                rv = SHR_E_RESOURCE;
            }
            SHR_ERR_EXIT(rv);
        }

        /* Get free base index from member table. */
        if (list_hdl) {
            uint32_t free_idx;
            rv = shr_itbm_list_block_alloc(list_hdl, lt_entry->max_paths,
                                           &bucket, &first);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,"RM rv=%d\n"), rv));
            if (SHR_E_RESOURCE == rv) {
                /* Defragment the member table and retry for index allocation. */
                rv = bcmecmp_itbm_groups_defragment(unit, lt_entry,
                                                    new_max_paths);
                if (SHR_SUCCESS(rv)) {
                    old_mstart_idx = grp_attr->mstart_idx;
                    /* Check if it is possible to increase max grp size in-place. */
                    bcmecmp_itbm_bucket_first_get(unit, gtype,
                                                  lt_entry->weighted_mode,
                                                  old_mstart_idx,
                                                  &bucket, &first);
                    rv = shr_itbm_list_block_resize(list_hdl, old_max_paths,
                                                    bucket, first,
                                                    new_max_paths);
                    if (SHR_E_NONE == rv) {
                        SHR_EXIT();
                    }

                    rv = shr_itbm_list_block_alloc(list_hdl, lt_entry->max_paths,
                                                   &bucket, &first);
                }
            }
            if (SHR_E_NONE != rv) {
                /* return E_RESOURCE if the underlying membership table is full */
                if (rv == SHR_E_FULL) {
                    rv = SHR_E_RESOURCE;
                }
                SHR_ERR_EXIT(rv);
            }
            /*move to the new start index*/
            free_idx = bcmecmp_itbm_mstart_index_get(unit, gtype,
                                                     grp_attr->weighted_mode,
                                                     bucket, first);
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Free %d gtype %d.\n"),
                        free_idx, gtype));
            new_mstart_idx = free_idx;
            lt_entry->mstart_idx = free_idx;
        } else {
            /*a new start index in member table is needed*/
            /* Initialize table operation structure and set the operation type. */
            bcmecmp_tbl_op_t_init(&tbl_op);
            tbl_op.oper_flags |= BCMECMP_OPER_ADD;
            tbl_op.num_ent = new_max_paths;

            /*
             * For HECMP Single lookup groups that are in Underlay ECMP group ID
             * range, member table entries must be allocated from Underlay member
             * table indicies range in HW.
             */
            if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)
                && (lt_entry->glt_sid ==
                      BCMECMP_TBL_LTD_SID(unit, group, BCMECMP_GRP_TYPE_SINGLE))
                && (ecmp_id >= BCMECMP_TBL_IDX_MIN
                                   (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))) {
                tbl_op.tbl_ptr = BCMECMP_TBL_PTR
                                     (unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
            } else {
                tbl_op.tbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);
            }

            rv = bcmecmp_tbl_free_idx_get(unit, &tbl_op, min2max);
            if (SHR_E_FULL == rv) {
                /* Defragment the member table and retry for index allocation. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_groups_defragment_and_move(unit, lt_entry));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "rv=%d.\n"),
                             bcmecmp_tbl_ref_cnt_print
                                 (unit,
                                  tbl_op.tbl_ptr,
                                  tbl_op.tbl_ptr->imin,
                                  tbl_op.tbl_ptr->tbl_size)));
                /*update old_mstart_idx after defragment*/
                old_mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
                /* Check if it is possible to increase max grp size in-place. */
                rv = bcmecmp_group_in_place_members_set(unit, lt_entry);
                if (rv != SHR_E_RESOURCE) {
                    /* return E_RESOURCE if the underlying membership table is full */
                    if (rv == SHR_E_FULL) {
                        rv = SHR_E_RESOURCE;
                    }
                    SHR_ERR_EXIT(rv);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_tbl_free_idx_get(unit, &tbl_op, min2max));
            } else if (SHR_E_NONE != rv) {
                SHR_ERR_EXIT(rv);
            }
            /*move to the new start index*/
            new_mstart_idx = tbl_op.free_idx;
        }

        sal_memset(BCMECMP_WALI_PTR(unit), 0, sizeof(*BCMECMP_WALI_PTR(unit)));
        mpb = shr_pb_create();
        shr_pb_printf(mpb,
                  "\n\tECMP_ID\tmstart_idx(s/d)\t SG.sz ==> DG.sz(Gap)\tgtype"
                  "\tSID\n");

        bcmecmp_grp_defrag_t_init(&src_block);
        bcmecmp_grp_defrag_t_init(&dst_block);

        src_block.ecmp_id = dst_block.ecmp_id = lt_entry->ecmp_id;
        src_block.op_arg = dst_block.op_arg = lt_entry->op_arg;
        src_block.glt_sid = dst_block.glt_sid = lt_entry->glt_sid;
        src_block.gtype = dst_block.gtype = lt_entry->grp_type;

        src_block.mstart_idx = old_mstart_idx;
        src_block.gsize = old_max_paths * idx_width;
        dst_block.mstart_idx = new_mstart_idx;
        dst_block.gsize =  new_max_paths * idx_width;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_groups_resize_oper(unit,
                                        mpb,
                                        lt_entry->grp_type,
                                        0,
                                        &src_block,
                                        &dst_block));
        if (list_hdl) {
            bcmecmp_itbm_bucket_first_get(unit, gtype, lt_entry->weighted_mode,
                                          old_mstart_idx, &bucket, &first);
            SHR_IF_ERR_EXIT(shr_itbm_list_block_free(list_hdl,
                                                     old_max_paths,
                                                     bucket,
                                                     first));
        }

        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = new_mstart_idx;

        for (idx = old_max_paths; idx < new_max_paths; idx++) {
            if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
                (BCMECMP_GRP_UECMP_PTR(unit, ecmp_id)[idx]) =
                     BCMECMP_LT_MIN_UECMP_ID(unit, group, gtype);
            } else {
                (BCMECMP_GRP_NHOP_PTR(unit, ecmp_id)[idx]) =
                     BCMECMP_LT_MIN_NHOP_ID(unit, group, gtype);
            }
        }
    }

    exit:
        if (mpb) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,"rv=%s.\n"),
                                    shr_pb_str(mpb)));
            shr_pb_destroy(mpb);
        }
        SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */
/*!
 * \brief set the unspecified fields of update entry .
 *
 * Set the unspecified fields of update entry with value of existing
 * group:
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
int
bcmecmp_group_update_entry_set(int unit,
                                       bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    uint32_t member_ctr = 0, comp_id = BCMMGMT_MAX_COMP_ID;
    bool in_use = FALSE;
    bcmecmp_control_t *ecmp_ctrl = NULL;
    bcmecmp_info_t *ecmp_inf = NULL;
    bcmecmp_lt_entry_t *cur_entry = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_update_entry_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;
    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    ecmp_inf = &ecmp_ctrl->ecmp_info;
    cur_entry = ecmp_inf->current_lt_ent;

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    /* Verify the group is in use. */
    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use, &comp_id));
    if (!in_use || (comp_id != BCMMGMT_ECMP_COMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d in use.\n"), ecmp_id));

    /*
     * Use stored group lb_mode value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
        && !BCMECMP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMECMP_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored wcmp_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
        && BCMECMP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size = BCMECMP_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_SIZE=%d.\n"), lt_entry->weighted_size));
    }

    /*
     * Use stored group max_paths value for update operation if it has not been
     * supplied.
     */
    if (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
        if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)) {

            /* Retrive and use group max_paths value. */
            lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "MAX_PATHS=%u.\n"), lt_entry->max_paths));
        }
    }

    /*
     * Use stored group nhop_sorted value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED)) {

        /* Retrive and use group nhop_sorted value. */
        lt_entry->nhop_sorted = BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NHOP_SORTED=%d.\n"), lt_entry->nhop_sorted));
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
        && lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = cur_entry->num_paths;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    /*
     * Use stored group ecmp_nhop value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)) {

        /*
         * Retrive and use group ecmp_nhop value. This field can be TRUE only
         * for Overlay ECMP Groups that have Underlay ECMP Group/s as Next-hops.
         */
        lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_NHOP=%d.\n"), lt_entry->ecmp_nhop));
    }

    /*
     * Use stored group rh_size_enc value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)) {

        /* Retrive and use group rh_size_enc value. */
        lt_entry->rh_size_enc = BCMECMP_GRP_RH_SIZE(unit, ecmp_id);
        lt_entry->rh_entries_cnt = BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_SIZE[CNT=%u]=%u.\n"),
                     lt_entry->rh_entries_cnt,
                     lt_entry->rh_size_enc));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)) {

        /* Retrive and use group rh_num_paths value. */
        lt_entry->rh_num_paths = cur_entry->rh_num_paths;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_NUM_PATHS=%u.\n"), lt_entry->rh_num_paths));
    }

    /*
     * Use stored aggregation group monitor pool value for update operation if
     * it has not been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_POOL)) {

        /* Retrive and use aggregation group monitor pool value. */
        lt_entry->agm_pool = BCMECMP_GRP_MON_AGM_POOL(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MON_AGM_POOL=%d.\n"), lt_entry->agm_pool));
    }

    /*
     * Use stored aggregation group monitor ID value for update operation if
     * it has not been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_ID)) {

        /* Retrive and use aggregation group monitor ID value. */
        lt_entry->agm_id = BCMECMP_GRP_MON_AGM_ID(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MON_AGM_ID=%d.\n"), lt_entry->agm_id));
    }

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                              BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID)) {
        lt_entry->ctr_ing_eflex_action_id =
            BCMECMP_GRP_CTR_ING_EFLEX_ACTION_ID(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "CTR_ING_EFLEX_ACTION_ID=%d.\n"),
                                lt_entry->ctr_ing_eflex_action_id));
    }

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                              BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT)) {
        lt_entry->ctr_ing_eflex_object =
            BCMECMP_GRP_CTR_ING_EFLEX_OBJECT(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "CTR_ING_EFLEX_OBJECT=%d.\n"),
                                lt_entry->ctr_ing_eflex_object));
    }

    /* Check and update NHOP_ID[] field values if it's not specified. */
    SHR_IF_ERR_EXIT(bcmecmp_group_update_member_fields_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcmecmp_group_update_weight_fields_set(unit, lt_entry));

    if (lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
        member_ctr = lt_entry->rh_num_paths;
    } else {
        member_ctr = lt_entry->num_paths;
    }

    if (bcmecmp_grp_is_overlay(gtype) &&
            ecmp_ctrl->ecmp_info.ovlay_memb_count) {
        lt_entry->o_nhop_ids_count = member_ctr;
        lt_entry->u_ecmp_nhop_count = member_ctr;
        lt_entry->uecmp_ids_count = member_ctr;
        lt_entry->nhop_ids_count = member_ctr;
    }else if (gtype == BCMECMP_GRP_TYPE_OVERLAY && lt_entry->ecmp_nhop) {
        lt_entry->uecmp_ids_count = member_ctr;
    } else {
        lt_entry->nhop_ids_count = member_ctr;
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "nhop_ids_count=%d, uecmp_ids_count=%d.\n"),
                 lt_entry->nhop_ids_count,
                 lt_entry->uecmp_ids_count));

    /* Sort the nhop array elements if it's been requested. */
    if (lt_entry->nhop_sorted) {
        SHR_IF_ERR_EXIT(bcmecmp_group_nhops_sort(unit, lt_entry));
    }

    exit:
        SHR_FUNC_EXIT();
}


/*!
 * \brief Initalize bcmecmp_tbl_op_t structure.
 *
 * \param [in] op_info Pointer to bcmecmp_tbl_op_t_init structure.
 *
 * \return Nothing.
 */
void
bcmecmp_tbl_op_t_init(bcmecmp_tbl_op_t *op_info)
{
    if (op_info) {
        op_info->oper_flags = BCMECMP_OPER_NONE;
        op_info->num_ent = 0;
        op_info->free_idx = BCMECMP_INVALID;
        op_info->tbl_ptr = NULL;
    }
    return;
}


/*!
 * \brief Add an ECMP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs ECMP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL group table is full.
 * \return SHR_E_RESOURCE member table is full.
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
int
bcmecmp_group_add(int unit,
                  bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_tbl_op_t tbl_op; /* Table operation info. */
    bool in_use = FALSE, rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */
    bool min2max = TRUE;
    uint32_t max_paths = 0, num_paths = 0, idx_width = 1;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    int rv;
    bcmecmp_info_t *ecmp_info = NULL;
    shr_itbm_list_hdl_t list_hdl = NULL;
    bcmecmp_grp_attr_t *grp_attr = NULL;
    uint32_t bucket = SHR_ITBM_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_add.\n")));
    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;
    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr) + ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use, &comp_id));

    if (in_use) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_ID=%d already Exists.\n"), ecmp_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is New.\n"), ecmp_id));

        /* Set values for NHOP_ID and NUM_PATHS fields if not specified. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_group_add_member_fields_set(unit, lt_entry));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_group_add_weight_fields_set(unit, lt_entry));

        /* Get RH LB_MODE status for further group configuration. */
        rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == lt_entry->lb_mode);

        /* Initialize table operation structure and set the operation type. */
        bcmecmp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMECMP_OPER_ADD;

        /*
         * Initialize number of entries to be reserved in member table based on
         * group's LB_MODE.
         */
        if (rh_lb_mode) {
            tbl_op.num_ent = lt_entry->rh_entries_cnt;
            max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
            num_paths = lt_entry->rh_num_paths;
        } else {
            tbl_op.num_ent = lt_entry->max_paths;
            max_paths = lt_entry->max_paths;
            num_paths = lt_entry->num_paths;
        }

        /*
         * For HECMP Single lookup groups that are in Underlay ECMP group ID
         * range, member table entries must be allocated from Underlay member
         * table indicies range in HW.
         */
        if (BCMECMP_MODE_HIER == BCMECMP_MODE(unit)
            && (lt_entry->glt_sid == BCMECMP_TBL_LTD_SID
                                        (unit, group, BCMECMP_GRP_TYPE_SINGLE))
            && (ecmp_id >= BCMECMP_TBL_IDX_MIN
                                (unit, group, BCMECMP_GRP_TYPE_UNDERLAY))) {
            tbl_op.tbl_ptr = BCMECMP_TBL_PTR
                                (unit, member, BCMECMP_GRP_TYPE_UNDERLAY);
        } else {
            tbl_op.tbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);
        }

        if (ecmp_info->itbm) {
            uint32_t free_idx;

            list_hdl = ecmp_info->member[gtype].list_hdl;
            /*
             * If no bank is allocated for the type of the group, the member
             * itbm list could be NULL. In this case, SHR_E_RESOURCE is returned.
             */
            SHR_NULL_CHECK(list_hdl, SHR_E_RESOURCE);

            if (bcmecmp_grp_is_member_replication(gtype,
                                                  lt_entry->weighted_mode)) {
                list_hdl = ecmp_info->member[gtype].list_hdl_1;
            }
            rv = shr_itbm_list_block_alloc(list_hdl, tbl_op.num_ent,
                                           &bucket, &free_idx);

            if (SHR_E_FULL == rv) {
                /*
                 * for member table running out of space, E_RESOURCE
                 * is returned (instead of E_FULL)
                 */
                rv = SHR_E_RESOURCE;
                SHR_ERR_EXIT(rv);
            } else if (SHR_E_RESOURCE == rv) {
                /* Defragment the member table and retry for index allocation. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_itbm_groups_defragment(unit, lt_entry,
                                                    tbl_op.num_ent));
                rv = shr_itbm_list_block_alloc(list_hdl, tbl_op.num_ent,
                                               &bucket, &free_idx);
                if (SHR_E_NONE != rv) {
                    /*
                     * for member table running out of space, E_RESOURCE
                     * is returned (instead of E_FULL)
                     */
                    rv = SHR_E_RESOURCE;
                    SHR_ERR_EXIT(rv);
                }
            } else if (SHR_E_NONE != rv) {
                SHR_ERR_EXIT(rv);
            }

            free_idx = bcmecmp_itbm_mstart_index_get(unit, gtype,
                                                     lt_entry->weighted_mode,
                                                     bucket, free_idx);
            lt_entry->mstart_idx = free_idx;
            /* Assign group ID. */
            list_hdl = ecmp_info->group[gtype].list_hdl;
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_alloc_id(list_hdl, 1,
                                              SHR_ITBM_INVALID, ecmp_id));
            grp_attr->in_use_itbm = 1;
            /* These are required for abort to call free with the right parameters */
            grp_attr->max_paths = max_paths;
            grp_attr->mstart_idx = free_idx;
            grp_attr->weighted_mode = lt_entry->weighted_mode;
        } else {
            rv = bcmecmp_tbl_free_idx_get(unit, &tbl_op, min2max);
            if ((SHR_E_FULL == rv)) {
                /* Defragment the member table and retry for index allocation. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmecmp_groups_defragment_and_move(unit, lt_entry));
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "rv=%d.\n"),
                             bcmecmp_tbl_ref_cnt_print(unit,
                                                   tbl_op.tbl_ptr,
                                                   tbl_op.tbl_ptr->imin,
                                                   tbl_op.tbl_ptr->tbl_size)));
                rv = bcmecmp_tbl_free_idx_get(unit, &tbl_op, min2max);
            }
            lt_entry->mstart_idx = tbl_op.free_idx;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "REGULAR gtype=%d free %d.\n"),
                      gtype, tbl_op.free_idx));
        }

        if (SHR_E_NONE != rv) {
            SHR_ERR_EXIT(rv);
        }

        /*
         * Check if group's LB_MODE is RESILIENT and call the corresponding
         * group install routine.
         */
        if (rh_lb_mode) {
            /*
             * Allocate memory to store the RH balanced nhop members for
             * installation in hardware tables.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_entries_arr,
                 lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_entries_arr)),
                 "bcmecmpRhGrpAddLtEntRhEntArr");

            if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
                BCMECMP_ALLOC
                    (lt_entry->rh_o_entries_arr, lt_entry->rh_entries_cnt *
                     sizeof(*(lt_entry->rh_o_entries_arr)),
                     "bcmecmpRhGrpAddLtEntRhOvlayEntArr");
                BCMECMP_ALLOC
                    (lt_entry->rh_u_ecmp_nhop_arr, lt_entry->rh_entries_cnt *
                     sizeof(*(lt_entry->rh_u_ecmp_nhop_arr)),
                     "bcmecmpRhGrpAddLtEntRhUnlayEcmpNhopArr");
            }

            /*
             * Allocate memory to store the RH group entry's mapped member index
             * data.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_mindex_arr,
                 lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_mindex_arr)),
                 "bcmecmpRhGrpAddLtEntRhMindexArr");

            /*
             * Allocate memory to store the RH group members entries usage count
             * data.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_memb_ecnt_arr,
                 max_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                 "bcmecmpRhGrpAddLtEntRhMembEntCntArr");

            /*
             * Set RH random seed from the user, which is used for selecting member
             */
            BCMECMP_RH_RAND_SEED(unit, ecmp_id) = lt_entry->rh_rand_seed;

            SHR_IF_ERR_EXIT
                (rh_group_member_balance_init(unit, lt_entry));

            /* Install RH-ECMP Group in hardware tables. */
            SHR_IF_ERR_EXIT
                (BCMECMP_FNCALL_EXEC(unit, rh_grp_ins)(unit, lt_entry));

            if (!list_hdl) {
                /*
                 * Increment Member table entries reference count by group's
                 * RH_SIZE i.e. rh_entries_cnt value.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_tbl_ref_cnt_incr
                        (BCMECMP_TBL_PTR(unit, member, gtype),
                         lt_entry->mstart_idx,
                         lt_entry->rh_entries_cnt,
                         idx_width));
            }

            BCMECMP_ALLOC(BCMECMP_GRP_RHG_PTR(unit, ecmp_id),
                          sizeof(*BCMECMP_GRP_RHG_PTR(unit, ecmp_id)),
                          "bcmecmpRhGrpAddRhgInfo");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id),
                              sizeof(*BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)),
                              "bcmecmpRhGrpAddRhgInfoBk");
            }

            /* Allocate memory to store RH-ECMP group entries. */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhEntArr");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC
                    (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id),
                     lt_entry->rh_entries_cnt *
                     sizeof(*(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id))),
                     "bcmecmpRhGrpAddGrpRhEntArrBk");
            }

            /* Copy load balanced RH members array. */
            sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                       lt_entry->rh_entries_arr,
                       sizeof(*(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id))) *
                              lt_entry->rh_entries_cnt);

            if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
                /* Allocate memory to store RH-ECMP group entries. */
                BCMECMP_ALLOC
                    (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                     lt_entry->rh_entries_cnt * sizeof(int),
                     "bcmecmpRhGrpAddGrpRhOvlayEntArr");
                BCMECMP_ALLOC
                    (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                     lt_entry->rh_entries_cnt * sizeof(bool),
                     "bcmecmpRhGrpAddGrpRhUnlayEntArr");
                if (BCMECMP_ATOMIC_TRANS(unit)) {
                    BCMECMP_ALLOC
                        (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr,
                         lt_entry->rh_entries_cnt * sizeof(int),
                         "bcmecmpRhGrpAddGrpRhOvlayEntArrBk");
                    BCMECMP_ALLOC
                        (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr,
                         lt_entry->rh_entries_cnt * sizeof(bool),
                         "bcmecmpRhGrpAddGrpRhUnlayEntArrBk");
                }

                /* Copy load balanced RH members array. */
                sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                           lt_entry->rh_o_entries_arr,
                           lt_entry->rh_entries_cnt * sizeof(int));
                sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                           lt_entry->rh_u_ecmp_nhop_arr,
                           lt_entry->rh_entries_cnt * sizeof(bool));
            }

            /*
             * Allocate memory to store group RH-ECMP entries mapped member
             * index information.
             */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                 lt_entry->rh_entries_cnt *
                 sizeof(*(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMindexArr");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC
                    (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                     lt_entry->rh_entries_cnt *
                     sizeof(*(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id))),
                     "bcmecmpRhGrpAddGrpRhMindexArrBk");
            }

            /* Copy group entries mapped member index array. */
            sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                       lt_entry->rh_mindex_arr,
                       (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                              lt_entry->rh_entries_cnt));

            /*
             * Allocate memory to store RH-ECMP members entries usage count
             * information.
             */
            BCMECMP_ALLOC
                (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                 max_paths *
                 sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMembEntCntArr");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC
                    (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                     max_paths *
                     sizeof(*(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, ecmp_id))),
                     "bcmecmpRhGrpAddGrpRhMembEntCntArrBk");
            }

            /* Copy members entry usage count information. */
            sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                       lt_entry->rh_memb_ecnt_arr,
                       (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))
                        * num_paths));
        } else {
            /* Sort the nhop array elements if it's been requested. */
            if (lt_entry->nhop_sorted) {
                SHR_IF_ERR_EXIT
                    (bcmecmp_group_nhops_sort(unit, lt_entry));
            }

            /* Install ECMP Group in hardware tables. */
            SHR_IF_ERR_EXIT
                (BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));

            if (!list_hdl) {
                /*
                 * Increment Member table entries reference count by MAX_PATHS
                 * value of the group.
                 */
                SHR_IF_ERR_EXIT
                    (bcmecmp_tbl_ref_cnt_incr
                        (BCMECMP_TBL_PTR(unit, member, gtype),
                         lt_entry->mstart_idx,
                         max_paths,
                         idx_width));
            }
        }

        /* Store ECMP Group information in Per-Group Info Array. */
        BCMECMP_GRP_TYPE(unit, ecmp_id) = lt_entry->grp_type;
        BCMECMP_GRP_LT_SID(unit, ecmp_id) = lt_entry->glt_sid;
        if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
            BCMECMP_GRP_LB_MODE(unit, ecmp_id) = lt_entry->lb_mode;
        } else {
            BCMECMP_GRP_LB_MODE(unit, ecmp_id) = lt_entry->weighted_size;
        }
        BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id) = lt_entry->weighted_mode;
        BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = lt_entry->nhop_sorted;
        BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = max_paths;
        BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = num_paths;
        BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) = lt_entry->rh_entries_cnt;
        BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = lt_entry->mstart_idx;

        if (!list_hdl) {
            /* Increment Group ID reference count. */
            BCMECMP_TBL_REF_CNT
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id) = 1;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "[ECMP_ID=%d]: mstart_idx=%d.\n"),
                    ecmp_id,
                    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id)));

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add: ECMP_ID=%d Ref_cnt=%d "
                              "SID[%d] != [%d].\n"),
                   ecmp_id,
                   BCMECMP_TBL_REF_CNT
                        (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                   BCMECMP_GRP_LT_SID(unit, ecmp_id),
                   lt_entry->glt_sid));
    }

    exit:
        if (rh_lb_mode) {
            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d\n"),
                        rh_grp_data_debug_print(unit, "ecmp group add",
                                                ecmp_id, lt_entry)));

            /* Free memory allocated for re-balance operation. */
            BCMECMP_FREE(lt_entry->rh_entries_arr);
            if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
                BCMECMP_FREE(lt_entry->rh_o_entries_arr);
                BCMECMP_FREE(lt_entry->rh_u_ecmp_nhop_arr);
            }
            BCMECMP_FREE(lt_entry->rh_mindex_arr);
            BCMECMP_FREE(lt_entry->rh_memb_ecnt_arr);
        }

        /* Check if it's error return case. */
        if (SHR_FUNC_ERR() && (BCMECMP_INVALID != ecmp_id)) {
            /*
             * On error return, free memory allocated for managing the RH-ECMP
             * group members.
             */
            if (rh_lb_mode) {
                if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                    if (BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                    }
                    if ((bcmecmp_grp_is_overlay(gtype)
                        && ecmp_info->ovlay_memb_count)) {
                        BCMECMP_FREE
                            (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr);
                        BCMECMP_FREE
                            (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr);
                    }
                    if (BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                    ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
                }

                if (BCMECMP_ATOMIC_TRANS(unit)
                    && BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                    if (BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                                   ecmp_id));
                    }
                    if (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr) {
                        BCMECMP_FREE
                            (ecmp_info->rhg_bk[ecmp_id]->rh_o_entries_arr);
                    }
                    if (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr) {
                        BCMECMP_FREE
                            (ecmp_info->rhg_bk[ecmp_id]->rh_u_ecmp_nhop_arr);
                    }
                    if (BCMECMP_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  ecmp_id));
                    }
                    if (BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                      ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                }
            }
        }

        SHR_FUNC_EXIT();
}

/*!
 * \brief Update installed ECMP logical table entry field values.
 *
 * Update an existing ECMP logical table entry field values. Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
int
bcmecmp_group_update(int unit,
                     bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use = FALSE, rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */
    uint32_t max_paths = 0, num_paths = 0, comp_id = BCMMGMT_MAX_COMP_ID;
    bcmecmp_info_t *ecmp_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_update.\n")));

    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use, &comp_id));

    /* Verify the group is in use. */
    if (!in_use || (comp_id != BCMMGMT_ECMP_COMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d in use.\n"), ecmp_id));

    /*
     * Use stored group lb_mode value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_LB_MODE)
        && !BCMECMP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMECMP_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored wcmp_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
        && BCMECMP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size = BCMECMP_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_SIZE=%d.\n"), lt_entry->weighted_size));
    }

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_MODE)
        && BCMECMP_GRP_IS_WEIGHTED(gtype)) {

        lt_entry->weighted_mode = BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_MODE=%d.\n"), lt_entry->weighted_mode));
    }

    /*
     * Use stored group max_paths value for update operation if it has not been
     * supplied.
     */
    if (lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {
        if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)) {

            /* Retrive and use group max_paths value. */
            lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "MAX_PATHS=%u.\n"), lt_entry->max_paths));
        } else if (lt_entry->max_paths != BCMECMP_GRP_MAX_PATHS(unit, ecmp_id)) {
            /*resize the ECMP group*/
            SHR_IF_ERR_EXIT(bcmecmp_group_max_paths_update(unit, lt_entry));
        }
    }

    /*
     * Use stored group nhop_sorted value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NHOP_SORTED)) {

        /* Retrive and use group nhop_sorted value. */
        lt_entry->nhop_sorted = BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NHOP_SORTED=%d.\n"), lt_entry->nhop_sorted));
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)
        && lt_entry->lb_mode != BCMECMP_LB_MODE_RESILIENT) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    /*
     * Use stored group ecmp_nhop value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_ECMP_NHOP)) {

        /*
         * Retrive and use group ecmp_nhop value. This field can be TRUE only
         * for Overlay ECMP Groups that have Underlay ECMP Group/s as Next-hops.
         */
        lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_NHOP=%d.\n"), lt_entry->ecmp_nhop));
    }

    /*
     * Use stored group rh_size_enc value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_SIZE)) {

        /* Retrive and use group rh_size_enc value. */
        lt_entry->rh_size_enc = BCMECMP_GRP_RH_SIZE(unit, ecmp_id);
        lt_entry->rh_entries_cnt = BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_SIZE[CNT=%u]=%u.\n"),
                     lt_entry->rh_entries_cnt,
                     lt_entry->rh_size_enc));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)) {

        /* Retrive and use group rh_num_paths value. */
        lt_entry->rh_num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_NUM_PATHS=%u.\n"), lt_entry->rh_num_paths));
    }

    /*
     * Use stored aggregation group monitor pool value for update operation if
     * it has not been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_POOL)) {

        /* Retrive and use aggregation group monitor pool value. */
        lt_entry->agm_pool = BCMECMP_GRP_MON_AGM_POOL(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MON_AGM_POOL=%d.\n"), lt_entry->agm_pool));
    }

    /*
     * Use stored aggregation group monitor ID value for update operation if
     * it has not been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MON_AGM_ID)) {

        /* Retrive and use aggregation group monitor ID value. */
        lt_entry->agm_id = BCMECMP_GRP_MON_AGM_ID(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MON_AGM_ID=%d.\n"), lt_entry->agm_id));
    }

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                              BCMECMP_LT_FIELD_CTR_ING_EFLEX_ACTION_ID)) {
        lt_entry->ctr_ing_eflex_action_id =
            BCMECMP_GRP_CTR_ING_EFLEX_ACTION_ID(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "CTR_ING_EFLEX_ACTION_ID=%d.\n"),
                                lt_entry->ctr_ing_eflex_action_id));
    }

    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp,
                              BCMECMP_LT_FIELD_CTR_ING_EFLEX_OBJECT)) {
        lt_entry->ctr_ing_eflex_object =
            BCMECMP_GRP_CTR_ING_EFLEX_OBJECT(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "CTR_ING_EFLEX_OBJECT=%d.\n"),
                                lt_entry->ctr_ing_eflex_object));
    }

    /* Check and update NHOP_ID[] field values if it's not specified. */
    SHR_IF_ERR_EXIT(bcmecmp_group_update_member_fields_set(unit, lt_entry));

    SHR_IF_ERR_EXIT
        (bcmecmp_group_update_weight_fields_set(unit, lt_entry));

    /* Sort the nhop array elements if it's been requested. */
    if (lt_entry->nhop_sorted) {
        SHR_IF_ERR_EXIT(bcmecmp_group_nhops_sort(unit, lt_entry));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMECMP_GRP_MEMB_TBL_START_IDX
                                (unit, ecmp_id);

    /*
     * Get group's load balancing mode for performing the correct delete
     * function call and member table reference count decrement operations.
     */
    rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                    (unit, ecmp_id));

    if (rh_lb_mode) {
        max_paths = BCMECMP_LT_MAX_RH_NPATHS(unit, group, gtype);
        num_paths = lt_entry->rh_num_paths;

        /*
         * Allocate memory to store the RH balanced nhop members for
         * installation in hardware tables.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_entries_arr,
             lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_entries_arr)),
             "bcmecmpRhGrpUpdtLtEntRhEntArr");

        if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
            BCMECMP_ALLOC
                (lt_entry->rh_o_entries_arr, lt_entry->rh_entries_cnt *
                 sizeof(*(lt_entry->rh_o_entries_arr)),
                 "bcmecmpRhGrpAddLtEntRhOvlayEntArr");
            BCMECMP_ALLOC
                (lt_entry->rh_u_ecmp_nhop_arr, lt_entry->rh_entries_cnt *
                 sizeof(*(lt_entry->rh_u_ecmp_nhop_arr)),
                 "bcmecmpRhGrpAddLtEntRhUnlayEcmpNhopArr");
        }

        /*
         * Allocate memory to store the RH group entry's mapped member index
         * data.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_mindex_arr,
             lt_entry->rh_entries_cnt * sizeof(*(lt_entry->rh_mindex_arr)),
             "bcmecmpRhGrpUpdtLtEntRhMindexArr");

        /*
         * Allocate memory to store the RH group members entries usage count
         * data.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_memb_ecnt_arr,
             max_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
             "bcmecmpRhGrpUpdtLtEntRhMembEntCntArr");

        /*
         * If NUM_PATHS value for this RH-group was zero prior to this update
         * operation and valid next-hop members are being added to this RH-group
         * in this UPDATE operation i.e. NUM_PATHS > 0, then this UPDATE
         * operation is equivalent to new next-hop members INSERT operation for
         * this group. So, use RH entries SET function for load balance
         * operation.
         */
        if (BCMECMP_LT_FIELD_GET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)
                && num_paths
                && !BCMECMP_GRP_NUM_PATHS(unit, ecmp_id)) {
            /* load balance the RH next-hop members. */
            SHR_IF_ERR_EXIT
                (rh_group_member_balance_init(unit, lt_entry));
        } else {

            /* Check if pre-config flag is set for this group. */
            if (BCMECMP_ENT_PRECONFIG_GET
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {

                /* Re-build RH-ECMP group members state for this group. */
                SHR_IF_ERR_EXIT
                    (bcmecmp_rh_grp_members_recover(unit, lt_entry));

                /* Clear pre-config flag for this RH-ECMP group. */
                BCMECMP_ENT_PRECONFIG_CLEAR
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id);
            }

            /*
             * This is an existing RH group members Add/Delete/Replace type of
             * operation. So, use RH entries UPDATE function to perform the
             * rebalancing work.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_group_rh_entries_update(unit, lt_entry));
        }

        /* Install RH-ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, rh_grp_ins)(unit, lt_entry));

        /* Copy updated group load balanced RH member entries array. */
        sal_memcpy(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id),
                   lt_entry->rh_entries_arr,
                   (sizeof(*BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id)) *
                          lt_entry->rh_entries_cnt));

        if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
            /* Copy load balanced RH members array. */
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_o_entries_arr,
                       lt_entry->rh_o_entries_arr,
                       lt_entry->rh_entries_cnt * sizeof(int));
            sal_memcpy(ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr,
                       lt_entry->rh_u_ecmp_nhop_arr,
                       lt_entry->rh_entries_cnt * sizeof(bool));
        }

        /* Copy updated group entries mapped member index array. */
        sal_memcpy(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   lt_entry->rh_mindex_arr,
                   (sizeof(*BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                          lt_entry->rh_entries_cnt));

        /* Copy updated members entry usage count array. */
        sal_memcpy(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   lt_entry->rh_memb_ecnt_arr,
                   (sizeof(*BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    num_paths));
    } else {
        max_paths = lt_entry->max_paths;
        num_paths = lt_entry->num_paths;

        /* Install ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));
    }

    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = lt_entry->lb_mode;
    } else {
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = lt_entry->weighted_size;
    }
    BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = lt_entry->nhop_sorted;
    BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) = num_paths;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[EID=%d]: Max_Paths=%u Num_Paths=%u LB=%u.\n"),
                ecmp_id,
                max_paths,
                num_paths,
                lt_entry->lb_mode));

    exit:
        /* Free memory allocated for RH group re-balance operation. */
        if (rh_lb_mode) {

            /* RH-Group data print debug function. */
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "rv=%d"),
                        rh_grp_data_debug_print(unit, "ecmp group update",
                                                ecmp_id, lt_entry)));

            /* Free temp RH group entries array. */
            BCMECMP_FREE(lt_entry->rh_entries_arr);

            if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
                BCMECMP_FREE(lt_entry->rh_o_entries_arr);
                BCMECMP_FREE(lt_entry->rh_u_ecmp_nhop_arr);
            }

            /* Free temp RH group entry mapped member index array. */
            BCMECMP_FREE(lt_entry->rh_mindex_arr);

            /* Free temp RH group member entries usage count array. */
            BCMECMP_FREE(lt_entry->rh_memb_ecnt_arr);
        }

        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an ECMP Group logical table entry from hardware tables.
 *
 * Delete an ECMP Group logical table entry from hardware tables. ECMP member
 * table resources consumed by the group are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_group_delete(int unit,
                     bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use = FALSE, rh_lb_mode = FALSE; /* Group's RH-LB_MODE status. */
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    uint32_t idx_width = 1;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_grp_attr_t *grp_attr = NULL;
    uint32_t bucket, first;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_delete: ECMP_ID: %d.\n"),
              lt_entry->ecmp_id));

    /* Get ECMP ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;
    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    list_hdl = ecmp_info->member[gtype].list_hdl;
    grp_attr = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr) + ecmp_id;
    if (bcmecmp_grp_is_member_replication(gtype, grp_attr->weighted_mode)) {
        list_hdl = ecmp_info->member[gtype].list_hdl_1;
    }

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use, &comp_id));

    /* Check and confirm ECMP_ID exists prior to delete operation. */
    if (!in_use || (comp_id != BCMMGMT_ECMP_COMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /*
     * Get group's load balancing mode for performing the correct delete
     * function call and member table reference count decrement operations.
     */
    rh_lb_mode = (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                    (unit, ecmp_id));

    if (rh_lb_mode) {
        /* Delete RH-ECMP Group from hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, rh_grp_del)(unit, lt_entry));

        /* Check if pre-config flag is set for this group. */
        if (BCMECMP_ENT_PRECONFIG_GET
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
            /* Clear pre-config flag for this RH-ECMP group. */
            BCMECMP_ENT_PRECONFIG_CLEAR
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id);
        }

        if (list_hdl) {
            bcmecmp_itbm_bucket_first_get(unit, gtype, grp_attr->weighted_mode,
                                          grp_attr->mstart_idx,
                                          &bucket, &first);
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_free(list_hdl,
                                          grp_attr->rh_entries_cnt,
                                          bucket,
                                          first));
        } else {
            /*
             * Decrement Member table entries reference count by group's RH_SIZE
             * i.e. rh_entries_cnt value.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_decr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id),
                     BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id),
                     idx_width));
        }

        /* Free memory allocated for RH group info. */
        if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
            /*
             * Free memory allocated for storing per RH entry mapped member's
             * index information.
             */
            BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));

            /*
             * Free memory allocated for storing RH load balanced group
             * entries.
             */
            BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));

            if ((bcmecmp_grp_is_overlay(gtype) && ecmp_info->ovlay_memb_count)) {
                BCMECMP_FREE
                    (ecmp_info->rhg[ecmp_id]->rh_o_entries_arr);

                BCMECMP_FREE
                    (ecmp_info->rhg[ecmp_id]->rh_u_ecmp_nhop_arr);
            }

            /*
             * Free memory allocated for storing per member RH entries usage
             * count.
             */
            BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));

            /* Free memory allocated for storing group's RH info. */
            BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
        }
    } else {
        /* Delete ECMP Group from hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, grp_del)(unit, lt_entry));

        if (list_hdl) {
            bcmecmp_itbm_bucket_first_get(unit, gtype, grp_attr->weighted_mode,
                                          grp_attr->mstart_idx,
                                          &bucket, &first);
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_free(list_hdl,
                                          grp_attr->max_paths,
                                          bucket,
                                          first));
        } else {
            SHR_IF_ERR_EXIT
                (bcmecmp_tbl_ref_cnt_decr
                    (BCMECMP_TBL_PTR(unit, member, gtype),
                     BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id),
                     BCMECMP_GRP_MAX_PATHS(unit, ecmp_id),
                     idx_width));
        }
    }

    list_hdl = ecmp_info->group[gtype].list_hdl;
    if (list_hdl) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free(list_hdl, 1, SHR_ITBM_INVALID, ecmp_id));
        /* Set in use flag in the bookkeeping for group attributes. */
        grp_attr->in_use_itbm = 0;
    } else {
        /* Decrement Group ID reference count. */
        BCMECMP_TBL_REF_CNT(BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id) = 0;
    }

    /* Clear/initialize ECMP Group information in Per-Group Info Array. */
    BCMECMP_GRP_TYPE(unit, ecmp_id) = BCMECMP_GRP_TYPE_COUNT;
    BCMECMP_GRP_LT_SID(unit, ecmp_id) = BCMLTD_SID_INVALID;
    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_LB_MODE_REGULAR;
    } else {
        BCMECMP_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_WEIGHTED_SIZE_256;
    }
    BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id) =
        BCMECMP_WEIGHTED_MODE_MEMBER_REPLICATION;
    BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id) = FALSE;
    BCMECMP_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_LT_MAX_PATHS
                                                        (unit, group, gtype);
    BCMECMP_GRP_RH_ENTRIES_CNT(unit, ecmp_id) = 0;
    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = BCMECMP_INVALID;

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Get an ECMP Group logical table entry data from hardware tables.
 *
 * Get an ECMP Group logical table entry data from hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmecmp_group_get(int unit,
                  bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use = FALSE;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_get.\n")));

    /* Get ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT
        (bcmecmp_grp_in_use(unit, ecmp_id, FALSE, 0, gtype, &in_use,
                            &comp_id));

    if (!in_use || (comp_id != BCMMGMT_ECMP_COMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d not found.\n"), ecmp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "ECMP_ID=%d in use.\n"), ecmp_id));

    if (BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id)) {
        /* Copy group Underlay ECMP_ID array information. */
        sal_memcpy
            (lt_entry->uecmp_id,
             BCMECMP_GRP_UECMP_PTR(unit, ecmp_id),
             sizeof(lt_entry->uecmp_id[0]) * BCMECMP_GRP_MAX_PATHS
                                                    (unit, ecmp_id));
    } else {
        /* Copy group NHOP_ID array information. */
        sal_memcpy
            (lt_entry->nhop_id,
             BCMECMP_GRP_NHOP_PTR(unit, ecmp_id),
             sizeof(lt_entry->nhop_id[0]) * BCMECMP_GRP_MAX_PATHS
                                                    (unit, ecmp_id));
    }

    if (!BCMECMP_GRP_IS_WEIGHTED(gtype)) {
        lt_entry->lb_mode = BCMECMP_GRP_LB_MODE(unit, ecmp_id);
    } else {
        lt_entry->weighted_size = BCMECMP_GRP_LB_MODE(unit, ecmp_id);
    }
    lt_entry->weighted_mode = BCMECMP_GRP_WEIGHTED_MODE(unit, ecmp_id);
    if (lt_entry->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
        lt_entry->num_paths = 0;
        lt_entry->max_paths = BCMECMP_LT_MAX_PATHS(unit, group, gtype);;
        lt_entry->rh_num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
    } else {
        lt_entry->num_paths = BCMECMP_GRP_NUM_PATHS(unit, ecmp_id);
        lt_entry->max_paths = BCMECMP_GRP_MAX_PATHS(unit, ecmp_id);
        lt_entry->rh_num_paths = 0;
    }
    lt_entry->ecmp_nhop = BCMECMP_GRP_ECMP_NHOP(unit, ecmp_id);
    lt_entry->nhop_sorted = BCMECMP_GRP_NHOP_SORTED(unit, ecmp_id);
    lt_entry->rh_size_enc = BCMECMP_GRP_RH_SIZE(unit, ecmp_id);
    lt_entry->agm_pool = BCMECMP_GRP_MON_AGM_POOL(unit, ecmp_id);
    lt_entry->agm_id = BCMECMP_GRP_MON_AGM_ID(unit, ecmp_id);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[ECMP_ID=%d]: Max_Paths=%u Num_Paths=%u"
                            " LB_MODE=%d SORTED=%d RH_SIZE=%u.\n"),
                lt_entry->ecmp_id,
                lt_entry->max_paths,
                lt_entry->num_paths,
                lt_entry->lb_mode,
                lt_entry->nhop_sorted,
                lt_entry->rh_size_enc));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse specified ECMP logical table and find inserted entries.
 *
 * This function traverses specified ECMP logical table, finds and returns the
 * first valid entry when Get-First operation is performed. For Get-Next
 * operation, it starts searching for next valid entry after the given ECMP_ID.
 * When an entry is found, it returns entry's data field values.
 *
 * When no valid entries are found for Get-First API call this function returns
 * SHR_E_EMPTY error code. For Get-Next API call when no entries are found
 * after the specified entry, it returns SHR_E_NOT_FOUND error code.
 *
 * Get-First vs Get-Next API call is indicated via the 'first' boolean input
 * parameter variable of this function.
 *
 * \param [in] unit Unit number.
 * \param [in] first Indicates get first logical table operation.
 * \param [out] lt_entry Pointer to ECMP logical table entry.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_EMPTY No entries in the logical table.
 * \return SHR_E_NOT_FOUND No entries found in the logical table.
 */
int
bcmecmp_group_find(int unit,
                   bool first,
                   bcmecmp_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_find.\n")));

    /* Get ECMP group level value. */
    gtype = lt_entry->grp_type;

    /* Initialize ecmp_id value based on get first or get next operation. */
    for (ecmp_id = (first ? BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype)
                                : (lt_entry->ecmp_id));
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
         ecmp_id++) {
        /*
         * Skip ECMP groups that are not in use or groups that do not match the
         * expected LT SID value.
         */
        if (!BCMECMP_TBL_REF_CNT
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)
            || (BCMECMP_GRP_LT_SID(unit, ecmp_id) != lt_entry->glt_sid)) {

            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Skipped: ECMP_ID=%d Ref_cnt=%d "
                                  "SID[%d] != [%d].\n"),
                       ecmp_id,
                       BCMECMP_TBL_REF_CNT
                            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                       BCMECMP_GRP_LT_SID(unit, ecmp_id),
                       lt_entry->glt_sid));

            continue;
        }

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Found: ECMP_ID=%d Ref_cnt=%d "
                              "SID[%d] != [%d].\n"),
                   ecmp_id,
                   BCMECMP_TBL_REF_CNT
                        (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id),
                   BCMECMP_GRP_LT_SID(unit, ecmp_id),
                   lt_entry->glt_sid));

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d found.\n"), ecmp_id));

        /* Update LT entry ECMP_ID value to be used for Get operation. */
        lt_entry->ecmp_id = ecmp_id;

        /* Retrieve LT entry information. */
        SHR_ERR_EXIT(bcmecmp_group_get(unit, lt_entry));
    }

    /*
     * Return entry not found error if no additional entries are found for
     * Get Next request.
     */
    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Free memory allocated to store group next-hop members.
 *
 * Free memory allocated to store group next-hop members.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
int
bcmecmp_groups_cleanup(int unit)
{
    bcmecmp_id_t ecmp_id; /* Group identifier. */
    bcmecmp_grp_type_t gtype; /* Group type. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_groups_cleanup.\n")));

    /*
     * Check and perform groups clean-up, only when ECMP feature has been
     * initialized successfuly for this BCM unit.
     */
    if (!(BCMECMP_CTRL(unit)->init)) {
        SHR_EXIT();
    }

    for (gtype = BCMECMP_GRP_TYPE_SINGLE;
            gtype < BCMECMP_GRP_TYPE_COUNT; gtype++ ) {
        if (gtype > BCMECMP_GRP_TYPE_SINGLE
                && BCMECMP_MODE(unit) != BCMECMP_MODE_HIER) {
            break;
        }

        /*
         * Check and skip invalid ECMP group types as no groups can exist at
         * these uninitialized ECMP group levels.
         */
        if (BCMECMP_TBL_LTD_SID(unit, group, gtype) == BCMLTD_SID_INVALID) {
            continue;
        }

        if (BCMECMP_FLEX_GRP_TYPE_IS_FLEX(gtype)) {
            continue;
        }

        /*
         * Check if the group type has been initialized prior to performing
         * group cleanup operation.
         */
        if (BCMECMP_LT_FIELD_ATTRS_PTR(unit, group, gtype) == NULL) {
            continue;
        }

        for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
             ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
             ecmp_id++) {
            if (BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
                if (BCMECMP_LB_MODE_RESILIENT == BCMECMP_GRP_LB_MODE
                                                        (unit, ecmp_id)) {
                    /* Free memory allocated for RH group info. */
                    if (BCMECMP_GRP_RHG_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_PTR(unit, ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                    ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RHG_PTR(unit, ecmp_id));
                    }

                    /* Free memory allocated for RH group backup info. */
                    if (BCMECMP_ATOMIC_TRANS(unit)
                        && BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_GRP_RH_ENTRIES_BK_PTR(unit,
                                                                   ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                  ecmp_id));
                        BCMECMP_FREE(BCMECMP_GRP_RHG_BK_PTR(unit, ecmp_id));
                    }
                }
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief RH-ECMP groups pre-config function for warm start sequence.
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
int
bcmecmp_rh_groups_preconfig(int unit)
{
    bcmecmp_id_t ecmp_id;     /* Group identifier. */
    bcmecmp_grp_type_t gtype; /* Group type. */
    const char *const lb_name[BCMECMP_LB_MODE_COUNT] = BCMECMP_LB_MODE_NAME;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s.\n"), __func__));

    /*
     * Check if ECMP feature has been intialized on this unit to proceed
     * further in this function.
     */
    BCMECMP_INIT(unit);

    /*
     * Check and perform groups HA init, only when ECMP feature has been
     * initialized successfuly for this BCM unit.
     */
    if (!(BCMECMP_CTRL(unit)->init)) {
        SHR_EXIT();
    }

    if (!(BCMECMP_FNCALL_CHECK(unit, rh_grp_ins))) {
        SHR_EXIT();
    }
    for (gtype = BCMECMP_GRP_TYPE_SINGLE; gtype < BCMECMP_GRP_TYPE_COUNT;
         gtype++ ) {

        /*
         * If device ECMP mode is not hierarchical, then there is no need to
         * check for valid overlay and underlay ECMP groups.
         */
        if (BCMECMP_MODE(unit) != BCMECMP_MODE_HIER
            && gtype > BCMECMP_GRP_TYPE_SINGLE) {
            break;
        }

        if (BCMECMP_FLEX_GRP_TYPE_IS_FLEX(gtype)) {
            continue;
        }

        /*
         * Check and skip invalid ECMP group types as no groups can exist at
         * these uninitialized ECMP group levels.
         */
        if (BCMECMP_TBL_LTD_SID(unit, group, gtype) == BCMLTD_SID_INVALID ||
            BCMECMP_TBL_LTD_SID(unit, group, gtype) == 0) {
            continue;
        }

        for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
             ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
             ecmp_id++) {

            /* Skip unused groups. */
            if (!BCMECMP_TBL_REF_CNT
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
                continue;
            }

            /*
             * For RH-ECMP groups, load balanced members state must be recovered
             * prior to members update operation. Check and set the pre_config
             * flag for this type of group.
             */
            if (BCMECMP_GRP_TYPE(unit, ecmp_id) == gtype
                && BCMECMP_GRP_LB_MODE
                        (unit, ecmp_id) == BCMECMP_LB_MODE_RESILIENT
                && BCMECMP_GRP_NUM_PATHS(unit, ecmp_id) != 0) {

                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "ECMP_ID[LB_MODE=%s,NUM_PATHS=%u]=%u:"
                                     " in-use.\n"),
                          lb_name[BCMECMP_GRP_LB_MODE(unit, ecmp_id)],
                          BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                          ecmp_id));
                /* Set pre-config flag for RH-ECMP group. */
                BCMECMP_ENT_PRECONFIG_SET
                    (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id);
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}

int
bcmecmp_grp_list_preconfig(int unit)
{
    bcmecmp_id_t ecmp_id;
    bcmecmp_grp_type_t gtype;

    SHR_FUNC_ENTER(unit);

    BCMECMP_INIT(unit);

    if (!(BCMECMP_CTRL(unit)->init)) {
        SHR_EXIT();
    }

    if (!BCMECMP_USE_INDEX_REMAP(unit)) {
        SHR_EXIT();
    }

    gtype = BCMECMP_GRP_TYPE_SINGLE;
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
         ecmp_id++) {

        /* Skip unused groups. */
        if (!BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
            continue;
        }

        if (BCMECMP_GRP_TYPE(unit, ecmp_id) != gtype) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmecmp_grp_list_insert(unit,
                                     BCMECMP_NON_MEMBER_WEIGHT_LIST(unit),
                                     ecmp_id, FALSE));
    }

    gtype = BCMECMP_GRP_TYPE_WEIGHTED;
    for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
         ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
         ecmp_id++) {

        /* Skip unused groups. */
        if (!BCMECMP_TBL_REF_CNT
            (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
            continue;
        }

        if (BCMECMP_GRP_TYPE(unit, ecmp_id) != gtype) {
            continue;
        }

        if (BCMECMP_GRP_IS_MEMBER_WEIGHT(unit, ecmp_id)) {
            SHR_IF_ERR_EXIT
                (bcmecmp_grp_list_insert(unit,
                                         BCMECMP_MEMBER_WEIGHT_LIST(unit),
                                         ecmp_id, TRUE));
        } else {
            SHR_IF_ERR_EXIT
                (bcmecmp_grp_list_insert(unit,
                                         BCMECMP_NON_MEMBER_WEIGHT_LIST(unit),
                                         ecmp_id, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_wb_itbm_group_preconfig(int unit)
{
    int ecmp_id, gstart_idx, gend_idx;
    uint32_t mstart_idx = 0;
    uint32_t max_members = 0;
    int rv = SHR_E_NONE;
    bcmecmp_info_t *ecmp_info;
    shr_itbm_list_hdl_t list_hdl;
    bcmecmp_grp_attr_t *grp_attr, *grp_attr_base;
    uint32_t bucket, first;
    int gtype;

    SHR_FUNC_ENTER(unit);

    ecmp_info = &(bcmecmp_get_ecmp_control(unit)->ecmp_info);
    grp_attr_base = (bcmecmp_grp_attr_t *)(ecmp_info->grp_arr);
    gstart_idx = 0;
    gend_idx = ecmp_info->grp_arr_sz;
    for (ecmp_id = gstart_idx; ecmp_id < gend_idx; ecmp_id++) {
        grp_attr = grp_attr_base + ecmp_id;
        /* check if the HA memory has the group configured. */
        if (!grp_attr->in_use_itbm) {
            continue;
        }

        gtype = grp_attr->grp_type;
        list_hdl = ecmp_info->group[gtype].list_hdl;
        rv = shr_itbm_list_block_alloc_id(list_hdl, 1, SHR_ITBM_INVALID,
                                          ecmp_id);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_ID=%d alloc_id failed - rv=%s.\n"),
                         ecmp_id, shr_errmsg(rv)));
            SHR_ERR_EXIT(rv);
        }

        mstart_idx = grp_attr->mstart_idx;
        bcmecmp_itbm_bucket_first_get(unit, gtype, grp_attr->weighted_mode,
                                      mstart_idx, &bucket, &first);
        /* Check RH group's max_members. */
        if (grp_attr->lb_mode == BCMECMP_LB_MODE_RESILIENT) {
            max_members = grp_attr->rh_entries_cnt;
        } else {
            max_members = grp_attr->max_paths;
        }
        list_hdl = ecmp_info->member[gtype].list_hdl;
        if (bcmecmp_grp_is_member_replication(gtype, grp_attr->weighted_mode)) {
            list_hdl = ecmp_info->member[gtype].list_hdl_1;
        }
        rv = shr_itbm_list_block_alloc_id(list_hdl, max_members, bucket, first);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_ID=%d member block alloc_id failed - "
                                    "rv=%s.\n"),
                         ecmp_id, shr_errmsg(rv)));
            SHR_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_limit_set(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmltd_sid_t lt_id,
                  bcmecmp_limit_t *ecmp_limit)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);

    if (ecmp_ctrl->ecmp_drv->itbm_memb_lists_update) {
        SHR_IF_ERR_EXIT
            (ecmp_ctrl->ecmp_drv->itbm_memb_lists_update
                  (unit, ecmp_limit->num_o_memb_bank));
    }

    if (ecmp_ctrl->ecmp_drv->memb_bank_set) {
        SHR_IF_ERR_EXIT
            (ecmp_ctrl->ecmp_drv->memb_bank_set(unit,
                                                op_arg,
                                                lt_id,
                                                ecmp_limit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_limit_get(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  bcmltd_sid_t lt_id,
                  bcmecmp_limit_t *ecmp_limit)
{
    bcmecmp_control_t *ecmp_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    ecmp_ctrl = bcmecmp_get_ecmp_control(unit);
    if (ecmp_ctrl->ecmp_drv->memb_bank_get) {
        SHR_IF_ERR_EXIT
            (ecmp_ctrl->ecmp_drv->memb_bank_get(unit,
                                                op_arg,
                                                lt_id,
                                                ecmp_limit));
    }

exit:
    SHR_FUNC_EXIT();
}
