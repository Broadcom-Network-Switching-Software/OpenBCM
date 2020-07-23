/*! \file bcmecmp_flex_group.c
 *
 * This file contains ECMP flexible group management functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmecmp/bcmecmp_common_imm.h>
#include <bcmecmp/bcmecmp_group_util.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

#define MEMBER0_MAX_FIELD 4
#define MEMBER1_MAX_FIELD 3
static const uint32_t fbmp_mem0_id[MEMBER0_MAX_FIELD] = {
      BCMECMP_LT_FIELD_MEMBER0_FIELD0,
      BCMECMP_LT_FIELD_MEMBER0_FIELD1,
      BCMECMP_LT_FIELD_MEMBER0_FIELD2,
      BCMECMP_LT_FIELD_MEMBER0_FIELD3,
};

static const uint32_t fbmp_mem1_id[MEMBER1_MAX_FIELD] = {
      BCMECMP_LT_FIELD_MEMBER1_FIELD0,
      BCMECMP_LT_FIELD_MEMBER1_FIELD1,
      BCMECMP_LT_FIELD_MEMBER1_FIELD2,
};

/*******************************************************************************
 * Private functions
 */
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
bcmecmp_flex_find_group(int unit,
                        bcmecmp_grp_type_t gtype,
                        uint32_t mstart_idx)
{
    int ecmp_id, gstart_idx, gend_idx, rv;
    bool in_use = FALSE;
    uint32_t comp_id = 0;

    gstart_idx = BCMECMP_TBL_IDX_MIN(unit, group, gtype);

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL1) ||
        (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED)) {
        gstart_idx += BCMECMP_TBL_IDX_MAX
            (unit, group, BCMECMP_GRP_TYPE_LEVEL0) + 1;
    }
    gend_idx = gstart_idx + BCMECMP_TBL_SIZE(unit, member, gtype) - 1;

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

        if (BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id)
            == (int)mstart_idx) {
            return ecmp_id;
        }
    }

    return SHR_E_NOT_FOUND;
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
bcmecmp_flex_wali_print(int unit,
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
                  "\n\tL3_ECMP_COUNT         %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMECMP_WALI_PTR(unit)->gwsize,
                  BCMECMP_WALI_PTR(unit)->gr_ecount,
                  BCMECMP_WALI_PTR(unit)->gw_ecount,
                  0);
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
 * \param [in] pb ECMP flexible group type.
 * \param [in] src Pointer to member table entries in the source fragment.
 * \param [in] dest Pointer to member table entries in the destination fragment.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmecmp_flex_groups_defrag_oper(int unit,
                                shr_pb_t *pb,
                                bcmecmp_grp_type_t gtype,
                                uint32_t gap_sz,
                                const bcmecmp_grp_defrag_t *src,
                                const bcmecmp_grp_defrag_t *dest)
{
    const bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);
    SHR_NULL_CHECK(dest, SHR_E_PARAM);

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

    /* Copy entries from source region to destination region in member table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_cpy)(unit, dest, src));

    /* Update Group's member table start index in HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, gmstart_updt)(unit, dest));
    /* Update SW copies of the group's member table start index value. */
    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, dest->ecmp_id) = dest->mstart_idx;
    /*
     * Clear entries in the source region in member table that were previously
     * copied.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_clr)(unit, src));
    exit:
        SHR_FUNC_EXIT();
}

static int
bcmecmp_flex_groups_resize_oper(int unit,
                                shr_pb_t *pb,
                                bcmecmp_grp_type_t gtype,
                                uint32_t gap_sz,
                                bcmecmp_grp_defrag_t *src,
                                bcmecmp_grp_defrag_t *dest)
{
    const bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    bcmecmp_grp_defrag_t ldest; /* local destination block copy. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);
    SHR_NULL_CHECK(dest, SHR_E_PARAM);

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

    /* Update Group's member table start index in HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, gmstart_updt)(unit, dest));

    /* Update SW copies of the group's member table start index value. */
    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, dest->ecmp_id) = dest->mstart_idx;

    /*
     * Clear entries in the source region in member table that were previously
     * copied.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (BCMECMP_FNCALL_EXEC(unit, memb_clr)(unit, src));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Defragment ECMP flexible group member table entries.
 *
 * This function performs defragmentation of ECMP member table entries used by
 * ECMP flexible groups, to create space for a new group.
 * Group type input parameter of
 * this function determines which portion of the member table
 * i.e. Overlay or Underlay portion is to be defragmented.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 * \param [in] req_size Minimum size of free block required to satisfy request.
 * \param [in] mstart_idx start idx of the block that needs to be updated.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_RESOURCE Insufficient Resources.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */

static int
bcmecmp_flex_itbm_groups_defragment(int unit,
                                    const bcmecmp_flex_lt_entry_t *entry,
                                    uint32_t req_size, int mstart_idx)
{
    shr_itbm_blk_t *itbm_blk = NULL, *free_blk = NULL, largest_free_blk;
    uint32_t tot_wal_mentries = 0; /* Total member table entries moved. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group type. */
    int i, j, rv = SHR_E_NONE, last_idx;
    shr_pb_t *mpb = NULL; /* Member table debug info print buffer. */
    shr_pb_t *gpb = NULL; /* Gap debug info print buffer. */
    bcmecmp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    shr_itbm_defrag_blk_mseq_t *m_seq;
    uint32_t itbm_blk_count;
    bcmecmp_grp_defrag_t m_src, m_dest;
    uint32_t hw_ecmp_id, free_count = 0;
    uint32_t new_mstart_idx = 0;
    int req_complete = FALSE;

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

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\tgtype=%u G_tbl_sz=%u.\n"),
              gtype,
              BCMECMP_TBL_SIZE(unit, group, gtype)));


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
     * Initialize group member start index to
     * the table Min index value as we have
     * to start the compression operation
     * from the first (Min) index of the
     * table.
     */
    SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_start(BCMECMP_TBL_ITBM_LIST(
                      BCMECMP_TBL_PTR(unit, member, gtype)),
                      &itbm_blk_count,
                      &itbm_blk,
                      &m_seq, &free_count, &free_blk, &largest_free_blk));

    req_complete = FALSE;
    if (free_count <= 0) {
        req_complete = FALSE;
    } else if (largest_free_blk.ecount >= req_size) {
        req_complete = TRUE;
    } else if (mstart_idx != BCMECMP_INVALID) {
        /* check if in-place update will work. */
        for (i = 0; i < (int)itbm_blk_count; ++i) {
            if ((uint32_t)mstart_idx != itbm_blk[i].first_elem) {
                continue;
            }
            last_idx = (int)(m_seq[i].mcount - 1);
            new_mstart_idx = m_seq[i].first_elem[last_idx];
            for (j = 0; j < (int)free_count; ++j) {
                if (free_blk[j].first_elem ==
                           new_mstart_idx + itbm_blk[i].ecount) {
                    if (free_blk[j].ecount + itbm_blk[i].ecount >= req_size) {
                        req_complete = TRUE;
                    }
                    break;
                }
            }
        }
    }

    if (req_complete == FALSE)
    {
        /*
         * The free space available after defragmentation is
         * not sufficient to satisfy this request.
         */
        SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(BCMECMP_TBL_ITBM_LIST(
                          BCMECMP_TBL_PTR(unit, member, gtype)), false));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    bcmecmp_grp_defrag_t_init(&m_src);
    m_src.op_arg = entry->op_arg;
    m_src.glt_sid = entry->glt_sid;
    m_src.gtype = gtype;
    bcmecmp_grp_defrag_t_init(&m_dest);
    m_dest.op_arg = entry->op_arg;
    m_dest.glt_sid = entry->glt_sid;
    m_dest.gtype = gtype;
    for (i = 0; i < (int)itbm_blk_count; ++i) {

        m_src.mstart_idx = itbm_blk[i].first_elem;
        m_src.gsize = itbm_blk[i].ecount;
        m_src.ecmp_id =
             bcmecmp_flex_find_group(unit, gtype, itbm_blk[i].first_elem);
        m_dest.ecmp_id = m_src.ecmp_id;
        for (j = 0; j < (int)(m_seq[i].mcount); ++j) {
            /*Move to first_elem[j]*/
            m_dest.mstart_idx = m_seq[i].first_elem[j];
            m_dest.gsize = m_seq[i].ecount;

            rv = bcmecmp_flex_groups_defrag_oper(
                              unit,
                              mpb,
                              gtype,
                              m_dest.gsize,
                              &m_src,
                              &m_dest);

            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(
                      BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit,
                                         member, gtype)), false));
                return rv;
            }

            m_src.mstart_idx = m_dest.mstart_idx;
            m_src.gsize = m_dest.gsize;
        }
        hw_ecmp_id = m_src.ecmp_id;
        if ((gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) ||
            (gtype == BCMECMP_GRP_TYPE_LEVEL1)) {
            hw_ecmp_id = m_src.ecmp_id -
                     BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        }

        if (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit,
                                  member, gtype))) {
            BCMECMP_FLEX_TBL_ENT_DEFRAG_STAGED_SET(
                    BCMECMP_TBL_PTR(unit, group,
                       gtype), hw_ecmp_id);
            BCMECMP_FLEX_GRP_TRANS_ID(unit, m_src.ecmp_id) = entry->op_arg->trans_id;

            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_FLEX_TBL_BK_ENT_DEFRAG_STAGED_SET(
                    BCMECMP_TBL_PTR(unit, group, gtype), hw_ecmp_id);
                BCMECMP_FLEX_GRP_TRANS_ID_BK(unit, m_src.ecmp_id) =
                entry->op_arg->trans_id;
            }
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(BCMECMP_TBL_ITBM_LIST(
                      BCMECMP_TBL_PTR(unit, member, gtype)), true));

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
                     bcmecmp_flex_wali_print(unit, tot_wal_mentries, mpb)));
            shr_pb_destroy(mpb);
        }

        SHR_FUNC_EXIT();
}

uint32_t
bcmecmp_flex_mbr_tbl_max(int unit, bcmecmp_grp_type_t gtype) {
    uint32_t max_idx = 0;

    /* Weighted groups use the same HW table as overlay and underlay.*/
    if (gtype == BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED) {
        gtype = BCMECMP_GRP_TYPE_LEVEL1;
    } else if (gtype == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) {
        gtype = BCMECMP_GRP_TYPE_LEVEL0;
    }

    if (gtype == BCMECMP_GRP_TYPE_LEVEL0) {
        max_idx = BCMECMP_TBL_IDX_MAX(unit, member,
                                      BCMECMP_GRP_TYPE_LEVEL0);
        if (BCMECMP_TBL_DRD_SID(unit, member2, BCMECMP_GRP_TYPE_LEVEL0)) {
            max_idx += BCMECMP_TBL_IDX_MAX(unit, member2,
                                      BCMECMP_GRP_TYPE_LEVEL0) + 1;
        }
    } else if (gtype == BCMECMP_GRP_TYPE_LEVEL1) {
        max_idx = BCMECMP_TBL_IDX_MAX(unit, member,
                                      BCMECMP_GRP_TYPE_LEVEL1);
        if (BCMECMP_TBL_DRD_SID(unit, member2, BCMECMP_GRP_TYPE_LEVEL1)) {
            max_idx += BCMECMP_TBL_IDX_MAX(unit, member2,
                                      BCMECMP_GRP_TYPE_LEVEL1) + 1;
        }
    }
    return max_idx;
}


/*!
 * \brief Reallocate RH memory for the ecmp group to account for change in size.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_RESOURCE if there is no space
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_flex_group_rh_realloc(int unit,
                              bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID;    /* Group ECMP_ID. */
    int i;

    SHR_FUNC_ENTER(unit);
    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    if (lt_entry->rh_mode) {
        if (BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                if (BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i)) {
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, \
                                 ecmp_id, i));
                }
            }
            if (BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id));
            }
            if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                 ecmp_id));
            }
        }

        if (BCMECMP_ATOMIC_TRANS(unit)
            && BCMECMP_FLEX_GRP_RHG_BK_PTR(unit, ecmp_id)) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                if (BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i)) {
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(
                                 unit, ecmp_id, i));
                }
            }
            if (BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
            }
            if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) {
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                    ecmp_id));
            }
        }
        /* Allocate memory to store RH-ECMP group entries. */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            BCMECMP_ALLOC
                (BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))),
                 "bcmecmpRhGrpAddGrpRhEntArr");
        }
        if (BCMECMP_ATOMIC_TRANS(unit)) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_ALLOC
                    (BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i),
                     lt_entry->max_paths *
                     sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit,
                              ecmp_id, i))),
                     "bcmecmpRhGrpAddGrpRhEntArrBk");
            }
        }

        /*
         * Allocate memory to store group RH-ECMP entries mapped member
         * index information.
         */
        BCMECMP_ALLOC
            (BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
             lt_entry->max_paths *
             sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id))),
             "bcmecmpRhGrpAddGrpRhMindexArr");
        if (BCMECMP_ATOMIC_TRANS(unit)) {
            BCMECMP_ALLOC
                (BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMindexArrBk");
        }

        /*
         * Allocate memory to store RH-ECMP members entries usage count
         * information.
         */
        BCMECMP_ALLOC
            (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
             lt_entry->max_paths *
             sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))),
             "bcmecmpRhGrpAddGrpRhMembEntCntArr");
        if (BCMECMP_ATOMIC_TRANS(unit)) {
            BCMECMP_ALLOC
                (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR
                                (unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMembEntCntArrBk");
        }
    }
    exit:
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
bcmecmp_flex_group_max_paths_update(int unit,
                                    bcmecmp_flex_lt_entry_t *lt_entry)
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
    uint32_t idx_width = 1;
    uint32_t free_idx;
    uint32_t bucket = SHR_ITBM_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_max_paths_update.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

    /* Initialize group ECMP_ID and group type local variables. */
    ecmp_id = lt_entry->ecmp_id;

    new_max_paths = lt_entry->max_paths;
    old_max_paths = BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id);
    old_mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);

    if (new_max_paths < old_max_paths) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_resize(BCMECMP_TBL_ITBM_LIST
                (BCMECMP_TBL_PTR(unit, member, gtype)),
                old_max_paths,
                SHR_ITBM_INVALID,
                old_mstart_idx,
                new_max_paths));
        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        if (lt_entry->rh_mode) {
            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_rh_realloc(unit, lt_entry));
        }
        SHR_EXIT();
    }

    if (new_max_paths > old_max_paths) {
        /* Check if it is possible to increase max grp size in-place. */
        if (old_mstart_idx + new_max_paths - 1 >
            bcmecmp_flex_mbr_tbl_max(unit, gtype)) {
            rv = SHR_E_RESOURCE;
        } else {
            rv = shr_itbm_list_block_resize(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, member, gtype)),
                    old_max_paths,
                    SHR_ITBM_INVALID,
                    old_mstart_idx,
                    new_max_paths);
        }
        if (rv == SHR_E_NONE) {
            BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        }

        if (rv != SHR_E_RESOURCE) {
            /* return E_RESOURCE if the underlying membership table is full */
            if (rv == SHR_E_FULL) {
                rv = SHR_E_RESOURCE;
            }
            if (lt_entry->rh_mode) {
                rv = bcmecmp_flex_group_rh_realloc(unit, lt_entry);
            }
            SHR_ERR_EXIT(rv);
        }

        /*a new start index in member table is needed*/
        /* Initialize table operation structure and set the operation type. */
        bcmecmp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMECMP_OPER_ADD;
        tbl_op.num_ent = new_max_paths;

        /*
         * For HECMP Single lookup groups that are in
         * Underlay ECMP flexible group ID
         * range, member table entries must be allocated from Underlay member
         * table indicies range in HW.
         */
        tbl_op.tbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);

        /* Get free base index from member table. */
        rv = shr_itbm_list_block_alloc
                (BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR(unit, member, gtype)),
                 lt_entry->max_paths,
                 &bucket,
                 &free_idx);

        LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                         "RM rv=%d free %d %d.\n"), rv, free_idx, gtype));
        if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {
            old_mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
            /* Defragment the member table and retry for index allocation. */
            rv = bcmecmp_flex_itbm_groups_defragment(unit, lt_entry,
                            new_max_paths, old_mstart_idx);
            old_mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id);
            if (SHR_SUCCESS(rv)) {
                /* Check if it is possible to increase max grp size in-place. */
                if (old_mstart_idx + new_max_paths - 1 <=
                    bcmecmp_flex_mbr_tbl_max(unit, gtype)) {
                    rv = shr_itbm_list_block_resize(BCMECMP_TBL_ITBM_LIST
                            (BCMECMP_TBL_PTR(unit, member, gtype)),
                            old_max_paths,
                            SHR_ITBM_INVALID,
                            old_mstart_idx,
                            new_max_paths);
                    if (SHR_E_NONE == rv) {
                        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
                        SHR_ERR_EXIT(rv);
                    }
                }

                rv = shr_itbm_list_block_alloc(BCMECMP_TBL_ITBM_LIST
                        (BCMECMP_TBL_PTR(unit, member, gtype)),
                        lt_entry->max_paths,
                        &bucket,
                        &free_idx);
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
        new_mstart_idx = free_idx;
        lt_entry->mstart_idx = free_idx;

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
            (bcmecmp_flex_groups_resize_oper(unit,
                                        mpb,
                                        lt_entry->grp_type,
                                        0,
                                        &src_block,
                                        &dst_block));

        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST
                (BCMECMP_TBL_PTR(unit, member, gtype)),
                old_max_paths,
                SHR_ITBM_INVALID,
                old_mstart_idx));

        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = new_max_paths;
        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = new_mstart_idx;

        if (lt_entry->rh_mode) {
            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_rh_realloc(unit, lt_entry));
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

/*!
 * \brief Returns a random member index value within an index range.
 *
 * Generates and returns a random member index value given an index range.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_id ECMP group ID.
 * \param [in] rand_max Max index value limit for random index generation.
 * \param [out] rand_index Pointer to generated randon index value.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_flex_rh_rand_memb_index_get(int unit,
                               uint32_t ecmp_id,
                               uint32_t rand_max,
                               uint32_t *rand_index)
{
    uint32_t modulus;
    uint32_t rand_seed_shift = 16;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rand_index, SHR_E_PARAM);

    modulus = rand_max + 1;
    if (modulus > (uint32_t)(1 << (32 - rand_seed_shift))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id) =
                   BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id) * 1103515245 + 12345;

    *rand_index = (BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id) >> rand_seed_shift)
                                                                  % modulus;

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
 * \param [in] max_ent_cnt Maximum entries a RH group member can occupy.
 * \param [in] overflow_cnt Surplus entries that RH group members can occupy.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 * \param [out] selected_index Pointer to selected to next-hop member index.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Internal error.
 */
static int
bcmecmp_flex_group_rh_memb_select(int unit,
                                  uint32_t max_ent_cnt,
                                  uint32_t *overflow_cnt,
                                  bcmecmp_flex_lt_entry_t *lt_entry,
                                  uint32_t *selected_index)
{
    uint32_t mindex = 0; /* Random member index value. */
    uint32_t nindex = 0; /* New member index value. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(overflow_cnt, SHR_E_PARAM);
    SHR_NULL_CHECK(selected_index, SHR_E_PARAM);

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcmecmp_flex_group_rh_memb_select.\n")));
    /*
     * NHOP member array index is zero based, hence mindex must be less than
     * "num_paths - 1".
     */
    SHR_IF_ERR_EXIT
        (bcmecmp_flex_rh_rand_memb_index_get
            (unit, lt_entry->ecmp_id, (lt_entry->rh_num_paths - 1),
             &mindex));

    if (lt_entry->rh_memb_ecnt_arr[mindex] < max_ent_cnt) {
        lt_entry->rh_memb_ecnt_arr[mindex] += 1;
        *selected_index = mindex;
    } else {
        nindex = (mindex + 1) % lt_entry->rh_num_paths;
        while (nindex != mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex] < max_ent_cnt) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                break;
            } else {
                nindex = (nindex + 1) % lt_entry->rh_num_paths;
            }
        }
        if (nindex == mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex]< (max_ent_cnt + 1)
                && (0 != *overflow_cnt)) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                *overflow_cnt -= 1;
            } else {
                nindex = (mindex + 1) % lt_entry->rh_num_paths;
                while (nindex != mindex) {
                    if (lt_entry->rh_memb_ecnt_arr[nindex] < (max_ent_cnt + 1)
                        && (0 != *overflow_cnt)) {
                        lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                        *selected_index = nindex;
                        *overflow_cnt -= 1;
                        break;
                    } else {
                        nindex = (nindex + 1) % lt_entry->rh_num_paths;
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
bcmecmp_flex_group_rh_entries_set(int unit,
                                  bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t idx;              /* loop iterator variable. */
    uint32_t selected_index = (uint32_t)BCMECMP_INVALID; /* Selected member
                                                            index. */
    int i;

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
        lt_entry->num_paths = 0;
        SHR_EXIT();
    }

    max_ent_cnt = (lt_entry->max_paths / lt_entry->rh_num_paths);
    overflow_cnt = (lt_entry->max_paths % lt_entry->rh_num_paths);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[max_paths=%-4u, "
                            "rh_num_paths=%-4d, max_ent_cnt:%-4u, ovf_cnt=%u].\n"),
                 lt_entry->max_paths,
                 lt_entry->rh_num_paths,
                 max_ent_cnt,
                 overflow_cnt));

    for (idx = 0; idx < lt_entry->max_paths; idx++) {
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_group_rh_memb_select(unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));

            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                if (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED) {
                    lt_entry->rh_entries_arr[i][idx] =
                                    lt_entry->member0_field[i][selected_index];
                } else {
                    lt_entry->rh_entries_arr[i][idx] =
                                    lt_entry->member1_field[i][selected_index];
                }
            }

        /*
         * Store selected next-hop array member index value for use during
         * Add/Delete/Replace operations.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;

    }

    exit:
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
bcmecmp_flex_group_update_entry_set(int unit,
                                    bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    bool in_use = FALSE;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_group_update_entry_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

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
        && !BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored wcmp_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
        && BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size = BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_SIZE=%d.\n"),
                     lt_entry->weighted_size));
    }

    /*
     * Use stored group max_paths value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS)) {

        /* Retrive and use group max_paths value. */
        lt_entry->max_paths = BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MAX_PATHS=%u.\n"), lt_entry->max_paths));
    } else if (lt_entry->max_paths !=
               BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id)) {
        /*resize the ECMP flexible group*/
        SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_flex_group_max_paths_update(unit, lt_entry));
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    /*
     * Use stored group rh_mode value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE)) {

        /* Retrive and use group rh_mode value. */
        lt_entry->rh_mode = BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_mode));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED)) {

        /* Retrive and use group rh_mode value. */
        lt_entry->rh_rand_seed = BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_rand_seed));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->rh_num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_NUM_PATHS=%u.\n"), lt_entry->rh_num_paths));
    }

    exit:
        SHR_FUNC_EXIT();
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
bcmecmp_flex_group_add(int unit,
                       bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_tbl_op_t tbl_op; /* Table operation info. */
    bool in_use = FALSE; /* Group's in use status. */
    uint32_t max_paths = 0, num_paths = 0;
    int rv;
    uint32_t free_idx, comp_id;
    uint32_t bucket = SHR_ITBM_INVALID;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex_group_add.\n")));
    /* Get ECMP flexible group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP flexible group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT(bcmecmp_grp_in_use(unit,
                                       ecmp_id,
                                       FALSE,
                                       0,
                                       gtype,
                                       &in_use,
                                       &comp_id));

    if (in_use) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "ECMP_ID=%d already Exists.\n"), ecmp_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
    } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "ECMP_ID=%d is New.\n"), ecmp_id));
        /* Initialize table operation structure and set the operation type. */
        bcmecmp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMECMP_OPER_ADD;

        /*
         * Initialize number of entries to be reserved in member table based on
         * group's LB_MODE.
         */
        tbl_op.num_ent = lt_entry->max_paths;
        max_paths = lt_entry->max_paths;
        num_paths = lt_entry->num_paths;

        /*
         * For HECMP Single lookup groups that are in
         * Underlay ECMP flexible group ID
         * range, member table entries must be allocated from Underlay member
         * table indicies range in HW.
         */
        tbl_op.tbl_ptr = BCMECMP_TBL_PTR(unit, member, gtype);
        /* Get free base index from member table. */
        rv = shr_itbm_list_block_alloc(BCMECMP_TBL_ITBM_LIST
                (BCMECMP_TBL_PTR(unit, member, gtype)),
                lt_entry->max_paths,
                &bucket,
                &free_idx);

        if (SHR_E_FULL == rv) {
            /*
             * for underlying member table running out of space, E_RESOURCE
             * is returned (instead of E_FULL)
             */
            rv = SHR_E_RESOURCE;
            SHR_ERR_EXIT(rv);
        } else if (SHR_E_RESOURCE == rv) {
            /* Defragment the member table and retry for index allocation. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_itbm_groups_defragment(unit, lt_entry,
                                        lt_entry->max_paths, BCMECMP_INVALID));
            rv = shr_itbm_list_block_alloc(BCMECMP_TBL_ITBM_LIST
                    (BCMECMP_TBL_PTR(unit, member, gtype)),
                    lt_entry->max_paths,
                    &bucket,
                    &free_idx);
            if (SHR_E_NONE != rv) {
                /*
                 * for underlying member table running out of space, E_RESOURCE
                 * is returned (instead of E_FULL)
                 */
                rv = SHR_E_RESOURCE;
                SHR_ERR_EXIT(rv);
            }
        }
        lt_entry->mstart_idx = free_idx;
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_alloc_id(BCMECMP_TBL_ITBM_LIST
                (BCMECMP_TBL_PTR(unit, group, gtype)),
                1,
                SHR_ITBM_INVALID,
                ecmp_id));
        BCMECMP_FLEX_GRP_ITBM_USE(unit, ecmp_id) = 1;
        /*
         * These are required for abort to call free
         * with the right parameters.
         */
        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = lt_entry->max_paths;
        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = free_idx;
        if (SHR_E_NONE != rv) {
            SHR_ERR_EXIT(rv);
        }
        /*
         * Check if group's LB_MODE is RESILIENT and call the corresponding
         * group install routine.
         */
        if (lt_entry->rh_mode) {
            num_paths = lt_entry->num_paths = lt_entry->rh_num_paths;

            if (lt_entry->rh_num_paths > lt_entry->max_paths) {
                rv = SHR_E_PARAM;
                SHR_ERR_EXIT(rv);
            }
            /*
             * Allocate memory to store the RH balanced nhop members for
             * installation in hardware tables.
             */
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_ALLOC
                    (lt_entry->rh_entries_arr[i],
                     lt_entry->max_paths * sizeof(*(lt_entry->rh_entries_arr[i])),
                     "bcmecmpRhGrpAddLtEntRhEntArr");
            }

            /*
             * Allocate memory to store the RH group entry's mapped member index
             * data.
             */
            BCMECMP_ALLOC
                (lt_entry->rh_mindex_arr,
                 lt_entry->max_paths * sizeof(*(lt_entry->rh_mindex_arr)),
                 "bcmecmpRhGrpAddLtEntRhMindexArr");

            /*
             * Allocate memory to store the RH group members entries usage count
             * data.
             */
            if (lt_entry->rh_num_paths) {
                BCMECMP_ALLOC
                    (lt_entry->rh_memb_ecnt_arr,
                     lt_entry->rh_num_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                     "bcmecmpRhGrpAddLtEntRhMembEntCntArr");
            }

            /*
             * Set RH random seed from the user, which is used for selecting member
             */
            BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id) = lt_entry->rh_rand_seed;

            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_rh_entries_set(unit, lt_entry));


            BCMECMP_ALLOC(BCMECMP_FLEX_GRP_VOID_RHG_PTR(unit, ecmp_id),
                          sizeof(*BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)),
                          "bcmecmpRhGrpAddRhgInfo");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC(BCMECMP_FLEX_GRP_VOID_RHG_BK_PTR(unit, ecmp_id),
                              sizeof(*BCMECMP_FLEX_GRP_RHG_BK_PTR(unit, ecmp_id)),
                              "bcmecmpRhGrpAddRhgInfoBk");
            }

            /* Allocate memory to store RH-ECMP group entries. */
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_ALLOC
                    (BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                     lt_entry->max_paths *
                     sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))),
                     "bcmecmpRhGrpAddGrpRhEntArr");
            }
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                    BCMECMP_ALLOC
                        (BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i),
                         lt_entry->max_paths *
                         sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit,
                                  ecmp_id, i))),
                         "bcmecmpRhGrpAddGrpRhEntArrBk");
                }
            }
            /* Copy load balanced RH members array. */
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                sal_memcpy(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                           lt_entry->rh_entries_arr[i],
                           sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit,
                                  ecmp_id, i))) * lt_entry->max_paths);
            }

            /*
             * Allocate memory to store group RH-ECMP entries mapped member
             * index information.
             */
            BCMECMP_ALLOC
                (BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                 lt_entry->max_paths *
                 sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMindexArr");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC
                    (BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                     lt_entry->max_paths *
                     sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id))),
                     "bcmecmpRhGrpAddGrpRhMindexArrBk");
            }

            /* Copy group entries mapped member index array. */
            sal_memcpy(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                       lt_entry->rh_mindex_arr,
                       (sizeof(*BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                              lt_entry->max_paths));

            /*
             * Allocate memory to store RH-ECMP members entries usage count
             * information.
             */
            BCMECMP_ALLOC
                (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                 max_paths *
                 sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id))),
                 "bcmecmpRhGrpAddGrpRhMembEntCntArr");
            if (BCMECMP_ATOMIC_TRANS(unit)) {
                BCMECMP_ALLOC
                    (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                     max_paths *
                     sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, ecmp_id))),
                     "bcmecmpRhGrpAddGrpRhMembEntCntArrBk");
            }

            /* Copy members entry usage count information. */
            sal_memcpy(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                       lt_entry->rh_memb_ecnt_arr,
                       (sizeof(*BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit,
                        ecmp_id)) * num_paths));
        }

        /* Install ECMP Group in hardware tables. */
        SHR_IF_ERR_EXIT
            (BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));


        /* Store ECMP Group information in Per-Group Info Array. */
        BCMECMP_FLEX_GRP_TYPE(unit, ecmp_id) = lt_entry->grp_type;
        BCMECMP_FLEX_GRP_LT_SID(unit, ecmp_id) = lt_entry->glt_sid;
        if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {
            BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = lt_entry->lb_mode;
            BCMECMP_FLEX_GRP_WEIGHTED_MODE(unit, ecmp_id) = FALSE;
        } else {
            BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = lt_entry->weighted_size;
            BCMECMP_FLEX_GRP_WEIGHTED_MODE(unit, ecmp_id) = lt_entry->weighted_mode;
        }
        BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = max_paths;
        BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) = num_paths;
        BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) =
        lt_entry->mstart_idx;
        if (lt_entry->rh_mode) {
            BCMECMP_FLEX_GRP_RH_ENTRIES_CNT(unit, ecmp_id) = max_paths;
            BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id) = TRUE;
        }
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "[ECMP_ID=%d]: mstart_idx=%d.\n"),
                    ecmp_id,
                    BCMECMP_GRP_MEMB_TBL_START_IDX(unit, ecmp_id)));

    }

    exit:
        if (lt_entry->rh_mode) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_FREE(lt_entry->rh_entries_arr[i]);
                lt_entry->rh_entries_arr[i] = NULL;
            }
            if (lt_entry->rh_mindex_arr) {
                BCMECMP_FREE(lt_entry->rh_mindex_arr);
            }
            if (lt_entry->rh_mindex_arr) {
                BCMECMP_FREE(lt_entry->rh_mindex_arr);
            }
            lt_entry->rh_mindex_arr = NULL;
            lt_entry->rh_memb_ecnt_arr = NULL;
        }
        /* Check if it's error return case. */
        if (SHR_FUNC_ERR()) {
            /*
             * On error return, free memory allocated for managing the RH-ECMP
             * group members.
             */
            if (lt_entry->rh_mode) {
                if (BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)) {
                    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                        if (BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i)) {
                            BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, \
                                         ecmp_id, i));
                        }
                    }
                    if (BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                    }
                    if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                    ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_VOID_RHG_PTR(unit, ecmp_id));
                }

                if (BCMECMP_ATOMIC_TRANS(unit)
                    && BCMECMP_FLEX_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                        if (BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i)) {
                            BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(
                                         unit, ecmp_id, i));
                        }
                    }
                    if (BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  ecmp_id));
                    }
                    if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) {
                        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                      ecmp_id));
                    }
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_VOID_RHG_BK_PTR(unit, ecmp_id));
                }
            }
        }
       SHR_FUNC_EXIT();
}


/*!
 * \brief Fill ECMP member fields into lt_mfield array from cur entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_mfield Array of lt entry member fields.
 * \param [in] cur_mfield Array of current entry member fields.
 * \param [in] lt_entry pointer to lt entry.
 * \param [in] member_idx field number in the lt entry.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL failure.
 */
int
bcmecmp_flex_member_fields_fill(int unit,
                                bcmecmp_flex_lt_entry_t *cur_lt_entry,
                                int member_tbl_id,
                                bcmecmp_flex_lt_entry_t *lt_entry,
                                bool replaceInvalidWithZeroes)
{
    uint32_t idx;
    const bcmecmp_drv_var_t *drv_var = bcmecmp_drv_var_get(unit);
    const bcmecmp_drv_var_ids_t *ids = NULL;
    const bcmecmp_member_info_t *member0_info, *member1_info;
    int i;

    SHR_FUNC_ENTER(unit);
    /* Verify for null pointer input params and return error. */
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);

    ids = drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    member0_info = drv_var->member0_info;
    member1_info = drv_var->member1_info;

    if (lt_entry == NULL) {
       return SHR_E_INTERNAL;
    }
    if (member_tbl_id == 0) {
        for (i = 0; i < member0_info->flds_count; i++) {
            if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, fbmp_mem0_id[i]))
                  || (lt_entry->member_count[i] < lt_entry->max_paths)) {
                for (idx = 0; idx < lt_entry->max_paths; idx++) {
                    if ((int)(lt_entry->member0_field[i][idx]) ==
                                                 BCMECMP_INVALID) {
                        if (replaceInvalidWithZeroes &&
                                 ((int)(cur_lt_entry->member0_field[i][idx]) ==
                                                            BCMECMP_INVALID)) {
                             lt_entry->member0_field[i][idx] = 0;
                        } else {
                             lt_entry->member0_field[i][idx] =
                                 cur_lt_entry->member0_field[i][idx];
                        }
                        lt_entry->member_count[i]++;
                    }
                }
            }
        }
    } else {
        for (i = 0; i < member1_info->flds_count; i++) {
            if (!(BCMECMP_LT_FIELD_GET(lt_entry->fbmp, fbmp_mem1_id[i]))
                  || (lt_entry->member_count[i] < lt_entry->max_paths)) {
                for (idx = 0; idx < lt_entry->max_paths; idx++) {
                    if ((int)(lt_entry->member1_field[i][idx]) ==
                                                 BCMECMP_INVALID) {
                        if (replaceInvalidWithZeroes &&
                            ((int)(cur_lt_entry->member1_field[i][idx]) ==
                                                         BCMECMP_INVALID)) {
                             lt_entry->member1_field[i][idx] = 0;
                        } else {
                             lt_entry->member1_field[i][idx] =
                                 cur_lt_entry->member1_field[i][idx];
                        }
                        lt_entry->member_count[i]++;
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
bcmecmp_flex_rh_groups_preconfig(int unit)
{
    bcmecmp_id_t ecmp_id;     /* Group identifier. */
    bcmecmp_grp_type_t gtype; /* Group type. */
    uint32_t hw_ecmp_id;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s.\n"), __func__));

    /*
     * Check and perform groups HA init, only when ECMP feature has been
     * initialized successfuly for this BCM unit.
     */
    if (!(BCMECMP_CTRL(unit)->init)) {
        SHR_EXIT();
    }

    for (gtype = BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED; gtype <= BCMECMP_GRP_TYPE_LEVEL1_WEIGHTED;
         gtype++ ) {

        /*
         * Check and skip invalid ECMP group types as no groups can exist at
         * these uninitialized ECMP group levels.
         */
        if (BCMECMP_TBL_LTD_SID(unit, group, gtype) == BCMLTD_SID_INVALID) {
            continue;
        }

        if (BCMECMP_FLEX_GRP_TYPE_IS_FLEX(gtype)) {
            /*
             * For RH-ECMP groups, load balanced members state must be recovered
             * prior to members update operation. Check and set the pre_config
             * flag for this type of group.
             */
            for (ecmp_id = BCMECMP_LT_MIN_ECMP_ID(unit, group, gtype);
                 ecmp_id <= BCMECMP_LT_MAX_ECMP_ID(unit, group, gtype);
                 ecmp_id++) {

                if (BCMECMP_FLEX_GRP_RH_MODE (unit, ecmp_id)
                    && BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) != 0) {

                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "ECMP_ID[NUM_PATHS=%u]=%u:"
                                         " in-use.\n"),
                              BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id),
                              ecmp_id));
                    /* Set pre-config flag for RH-ECMP group. */
                    hw_ecmp_id = ecmp_id;
                    if (ecmp_id >= BCMECMP_TBL_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0)) {
                         hw_ecmp_id = ecmp_id - BCMECMP_TBL_SIZE(
                                                 unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0);
                    }
                    BCMECMP_FLEX_ENT_PRECONFIG_SET
                        (BCMECMP_TBL_PTR(unit, group, gtype), hw_ecmp_id);
                }
                continue;
            }

        }
    }

    exit:
        SHR_FUNC_EXIT();
}

static inline int
bcmecmp_flex_compare_idx(int unit, bcmecmp_flex_lt_entry_t *entry1, int idx1,
                         int idx2, int level)
{
    int i;
    bcmecmp_flex_lt_entry_t *entry2 = BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit);
    if (level == 0) {
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            if (entry1->member0_field[i][idx1] != entry2->member0_field[i][idx2]) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            if (entry1->member1_field[i][idx1] != entry2->member1_field[i][idx2]) {
                return 0;
            }
        }
    }
    return 1;
}

static int
bcmecmp_flex_compare_rh_ent_idx(int unit, bcmecmp_flex_lt_entry_t *entry1, int idx1,
                         bcmecmp_flex_lt_entry_t *entry2, int idx2, int level)
{
    volatile int i;
    if (level == 0) {
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            if (((int)(entry1->member0_field[i][idx1])
                         != (int)(BCMECMP_INVALID)) &&
                (((entry1->member0_field[i][idx1])) !=
                   entry2->rh_entries_arr[i][idx2])) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            if (((int)(entry1->member1_field[i][idx1])
                         != (int)(BCMECMP_INVALID)) &&
                (((entry1->member1_field[i][idx1]))
                  != entry2->rh_entries_arr[i][idx2])) {
                return 0;
            }
        }
    }
    return 1;
}

/*!
 * \brief Add new member to an existing RH-ECMP group.
 *
 * This function adds a new member to an existing RH-ECMP group and re-balances
 * the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] new_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_flex_rh_grp_memb_add(int unit,
                        uint32_t new_mindex,
                        bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    uint32_t eindex = 0;      /* Selected entry index. */
    uint32_t next_eindex = 0; /* Next entry index. */
    uint32_t *ecount = NULL;  /* New member entries usage count. */
    uint32_t mindex;          /* member index. */
    bool new_memb_sel = FALSE; /* New member selected. */
    int i;
    int level;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s - n_mindex=%u n_nhop=%d.\n"),
             __func__,
             new_mindex,
             (lt_entry->member0_field[0][new_mindex])));

    lbound = lt_entry->max_paths / lt_entry->rh_num_paths;
    ubound = (lt_entry->max_paths % lt_entry->rh_num_paths)
                    ? (lbound + 1) : lbound;
    threshold = ubound;

    level = (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)? 0: 1;
    /*
     * Set local entry count pointer to point to new member's entry usage
     * count array element address.
     */
    ecount = &(lt_entry->rh_memb_ecnt_arr[new_mindex]);

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

    while (*ecount < lbound) {
        /* Select a random entry to replace with new member. */
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_rh_rand_memb_index_get
                (unit, lt_entry->ecmp_id, (lt_entry->max_paths - 1),
                 &eindex));
        if (lt_entry->rh_mindex_arr[eindex] == new_mindex) {
            new_memb_sel = TRUE;
        } else {
            new_memb_sel = FALSE;
            mindex = lt_entry->rh_mindex_arr[eindex];
        }

        if (!new_memb_sel && (lt_entry->rh_memb_ecnt_arr[mindex] > threshold)) {

            /*lt_entry->rh_entries_arr[eindex] = new_nhop_id;*/

            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                lt_entry->rh_entries_arr[i][eindex] =
                              lt_entry->member0_field[i][new_mindex];
            }
            lt_entry->rh_mindex_arr[eindex] = new_mindex;

            /* Decrement entry usage count value for the replaced member. */
            lt_entry->rh_memb_ecnt_arr[mindex] -= 1;

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
            next_eindex = (eindex + 1) % lt_entry->max_paths;
            while (next_eindex != eindex) {

                mindex = lt_entry->rh_mindex_arr[next_eindex];

                if (mindex == new_mindex) {
                    new_memb_sel = TRUE;
                } else {
                    new_memb_sel = FALSE;
                }

                if (!new_memb_sel
                    && (lt_entry->rh_memb_ecnt_arr[mindex] > threshold)) {
                    /*
                     * Replace existing member with new member next-hop ID
                     * value.
                     */
                    if (level == 0) {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][next_eindex] =
                                        lt_entry->member0_field[i][new_mindex];
                        }
                    } else {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][next_eindex] =
                                        lt_entry->member1_field[i][new_mindex];
                        }
                    }

                    /*
                     * Replace existing member with the new member index
                     * value.
                     */
                    lt_entry->rh_mindex_arr[next_eindex] = new_mindex;

                    /*
                     * Decrement entry usage count value for the replaced
                     * member.
                     */
                    lt_entry->rh_memb_ecnt_arr[mindex] -= 1;

                    /* Increment entry usage count value for the new member. */
                    *ecount += 1;
                    break;
                } else {
                    next_eindex = (next_eindex + 1) % lt_entry->max_paths;
                }
            }
            if (next_eindex == eindex) {
                threshold--;
            }
        }
    }

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
bcmecmp_flex_rh_grp_members_add(int unit,
                           bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t new_mindex; /* New member array index. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_flex_rh_grp_members_add.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                   (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i)))
                   * lt_entry->max_paths));
    }

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->max_paths));
    /*
     * Copy RH group's existing entries usage count array into LT entry's RH
     * entries usage count array for update operation.
     */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * grp_npaths));

    /*
     * Call RH member add function for every new member in the
     * next-hop array. New members must be set in the array after the
     * current next-hop member elements.
     */
    for (new_mindex = grp_npaths; new_mindex < lt_entry->rh_num_paths;
         new_mindex++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_flex_rh_grp_memb_add(unit, new_mindex, lt_entry));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an existing RH-ECMP group.
 *
 * This function deletes a member from an existing RH-ECMP group and
 * re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] del_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to ECMP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmecmp_flex_rh_grp_memb_delete(int unit,
                           uint32_t del_mindex,
                           bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t selected_index = (uint32_t)BCMECMP_INVALID; /* Selected member
                                                            index. */
    uint32_t ecmp_id; /* ECMP group identifier. */
    int i;
    int level;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_flex_rh_grp_memb_delete: mindex=%u.\n"),
                  del_mindex));

    max_ent_cnt = (lt_entry->max_paths / lt_entry->rh_num_paths);
    overflow_cnt = (lt_entry->max_paths % lt_entry->rh_num_paths);
    ecmp_id = lt_entry->ecmp_id;

    level = (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)? 0: 1;

    for (idx = 0; idx < lt_entry->max_paths; idx++) {
        /*
         * If current member is not the member to be deleted, continue to the
         * next member.
         */
        if (BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] != del_mindex) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_group_rh_memb_select(unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));

        if (level == 0) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                lt_entry->rh_entries_arr[i][idx] =
                      lt_entry->member0_field[i][selected_index];
            }
        } else {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                lt_entry->rh_entries_arr[i][idx] =
                      lt_entry->member1_field[i][selected_index];
            }
        }

        /*
         * Delay this update until all members are deleted so that mindex array
         * has old mindex member value for multi member delete operation.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;

        /* Decrement deleted members entry usage count value.  */
        BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[del_mindex] -= 1;

    }
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
bcmecmp_flex_rh_grp_members_delete(int unit,
                              bcmecmp_flex_lt_entry_t *lt_entry)
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
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex__rh_grp_members_delete.\n")));

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
        grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

        /* Determine how many members have been deleted. */
        deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

        /*
         * For update operation, copy existing group RH entries array
         * into LT entry's RH entries array for updation.
         */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            sal_memcpy(lt_entry->rh_entries_arr[i],
                       BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                       (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                            lt_entry->max_paths));
        }
        /*
         * Copy RH group's existing entries mapped member index array into LT
         * entry's RH entries mapped member index array for update operation.
         */
        sal_memcpy(lt_entry->rh_mindex_arr,
                   BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*(lt_entry->rh_mindex_arr)) *
                    lt_entry->max_paths));

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
                 * Compare existing members with members in updated
                 * array, confirm rh_memb_ecnt_arr value is equal to zero for
                 * the updated nhop member to skip duplicate members.
                 */
                if (bcmecmp_flex_compare_idx(unit, lt_entry, idx, uidx, 0)
                    && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                    lt_entry->rh_memb_ecnt_arr[uidx] =
                            BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
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
                        BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
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
            /* Delete the member and re-balance the group entries. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_memb_delete(unit,
                                            deleted_mindex[idx],
                                            lt_entry));
        }

        /*
         * For the shared members, check and update the member index value in
         * the LT entry mindex array if it has changed.
         */
        if (shrd_new_mindex_cnt) {
            for (idx = 0; idx < grp_npaths; idx++) {
                new_idx = shrd_mindex[idx];
                if (new_idx == (uint32_t)BCMECMP_INVALID) {
                    continue;
                }
                for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                    if (bcmecmp_flex_compare_idx(unit, lt_entry, idx, uidx, 0)
                        && idx == lt_entry->rh_mindex_arr[uidx]) {
                        /* Update mindex with new mindex value. */
                        lt_entry->rh_mindex_arr[uidx] = new_idx;
                    }
                }
                shrd_new_mindex_cnt--;
            }
        }
    } else {
        /* Clear RH-ECMP group member table entries. */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            sal_memset(lt_entry->rh_entries_arr[i],
                       0,
                       (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                        lt_entry->max_paths));
        }

        /* Clear stored RH-ECMP group member indices data. */
        sal_memset(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                    lt_entry->max_paths));

        /* Clear stored RH-ECMP group members entry usage count data. */
        sal_memset(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   0,
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    lt_entry->rh_num_paths));
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
bcmecmp_flex_rh_grp_members_replace(int unit,
                                    bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t idx, uidx;       /* Array index iterator variables. */
    bcmecmp_id_t ecmp_id;     /* ECMP group identifier. */
    int i;
    int level;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex__rh_grp_members_replace.\n")));

    ecmp_id = lt_entry->ecmp_id;

    grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

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
    SHR_NULL_CHECK(BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id), SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                   (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                    lt_entry->max_paths));
    }

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->max_paths));

    /* Copy members entry usage count information. */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * lt_entry->rh_num_paths));

    level = (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)? 0: 1;

    /*
     * Find and replace the existing nhop member with the new nhop member
     * ID value.
     */
    for (idx = 0; idx < lt_entry->rh_num_paths; idx++) {
        if (bcmecmp_flex_compare_idx(unit, lt_entry, idx, idx, 0) == 0) {
            for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                if (bcmecmp_flex_compare_rh_ent_idx(unit, BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit), idx, lt_entry, uidx, 0)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                        lt_entry->rh_entries_arr[i][uidx] =
                           ((level == 0) ? lt_entry->member0_field[i][idx]:
                                           lt_entry->member1_field[i][idx]);
                    }
                }
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
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
bcmecmp_flex_group_rh_oper_get(int unit,
                          bcmecmp_flex_lt_entry_t *lt_entry,
                          bcmecmp_rh_oper_t *rh_oper)
{
    uint32_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths; /* Group current NUM_PATHS value. */
    volatile uint32_t idx = 0, uidx = 0; /* Next-hop members iterator variables. */
    uint32_t shared_nhop_cnt = 0; /* Shared NHOPs count. */
    uint32_t new_nhop_cnt = 0; /* New NHOPs count. */
    uint32_t alloc_sz = 0; /* Alloc size current and new. */
    int *nhop_arr = NULL; /* NHOP members array. */
    bool mindex_changed = FALSE; /* Member index has changed in NHOP array. */
    const char * const rh_op_str[BCMECMP_RH_OPER_COUNT] = BCMECMP_RH_OPER_NAME;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rh_oper, SHR_E_PARAM);

    COMPILER_REFERENCE(ecmp_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex_group_rh_oper_get.\n")));
    ecmp_id = lt_entry->ecmp_id;

    /*
     * If there are no members in the RH-ECMP group, then there is no SW load
     * balancing operation to performed for this group in the UPDATE operation.
     */
    if (BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) == 0 && lt_entry->rh_num_paths == 0) {

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
    if (BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) == 0 && lt_entry->rh_num_paths > 0) {
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
    if (BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) > 0 && lt_entry->rh_num_paths == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "(RH_NUM_PATHS = %u): BCMECMP_RH_OPER_DELETE.\n"),
                    lt_entry->rh_num_paths));

        *rh_oper = BCMECMP_RH_OPER_DELETE;
        SHR_EXIT();
    }

    /* Validate NUM_PATHS value before proceeding further in this function. */
    if (BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) == 0 || lt_entry->rh_num_paths == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unexpected RH_NUM_PATHS value: GNUM_PATHS=%u"
                                " RH_NUM_PATHS=%u.\n"),
                     BCMECMP_GRP_NUM_PATHS(unit, ecmp_id),
                     lt_entry->rh_num_paths));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);
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
        int start_idx = 0;
        sal_memset(nhop_arr, 0, alloc_sz);

        for (uidx = 0; uidx < lt_entry->rh_num_paths; uidx++) {
            for (idx = start_idx; idx < grp_npaths; idx++) {
                if ((nhop_arr[idx] != -1)
                     && bcmecmp_flex_compare_idx(unit, lt_entry, idx, uidx, 0)) {
                    /* Old NHOP also member of the updated NHOP array. */
                    shared_nhop_cnt++;
                    nhop_arr[idx] = -1;
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
            while (nhop_arr[start_idx] == -1) {
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
bcmecmp_flex_rh_grp_members_add_replace(int unit,
                                   bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t ecmp_id = BCMECMP_INVALID; /* Group ECMP_ID. */
    uint32_t grp_npaths = 0;  /* Current num_paths value of the group. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t added_cnt = 0; /* Number of members added to the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    volatile uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t shrd_mindex_cnt = 0; /* Count of shared members with new mindex. */
    volatile uint32_t rep_idx = 0; /* Added member array index. */
    volatile uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    int i;
    int level;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmecmp_rh_grp_members_add_replace.\n")));

    /* Initialize the ECMP group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get group's current num_paths value. */
    grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                   (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                    lt_entry->max_paths));
    }

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->max_paths));

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
            if (bcmecmp_flex_compare_idx(unit, lt_entry, uidx, idx, 0)
                && (lt_entry->rh_memb_ecnt_arr[uidx] == 0)) {
                /* Update entry usage count. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];

                /* Increment shared entry count. */
                shrd_cnt++;

                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_mindex_cnt++;
                }

                /* Clear old entry usage count value. */
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
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

    if (replace_cnt) {
        BCMECMP_ALLOC(new_mindex,
                      replace_cnt * sizeof(*new_mindex),
                      "bcmecmpRhGrpMembsAddRepNewMindexArr");

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
                    BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];

                /* Clear old entry usage count value. */
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Store the new mindex mapped to this old mindex for updating
                 * the mindex and rh_entries_arr arrays.
                 */
                new_mindex[idx] = uidx;

                break;
            }
        }
    }

    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_mindex_cnt) {
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                if (bcmecmp_flex_compare_rh_ent_idx(unit, BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit), idx, lt_entry, uidx, 0)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
            shrd_mindex_cnt--;
        }
    }

    level = (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)? 0: 1;

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];

            for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                if (bcmecmp_flex_compare_rh_ent_idx(unit, BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit), rep_idx, lt_entry, uidx, 0)
                    && (rep_idx == lt_entry->rh_mindex_arr[uidx])) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    if (level == 0) {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][uidx] = lt_entry->member0_field[i][new_idx];
                        }
                    } else {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][uidx] = lt_entry->member1_field[i][new_idx];
                        }
                    }
                }
            }
        }
    }

    for (new_idx = 0; new_idx < lt_entry->rh_num_paths;
         new_idx++) {
        if (lt_entry->rh_memb_ecnt_arr[new_idx]) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_flex_rh_grp_memb_add(unit, new_idx, lt_entry));
        added_cnt++;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "\n\tadded_cnt=%u n_mindex=%u\n"),
                     added_cnt, new_idx));
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
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
bcmecmp_flex_rh_grp_members_delete_replace(int unit,
                                      bcmecmp_flex_lt_entry_t *lt_entry)
{
    uint32_t ecmp_id; /* ECMP group identifier. */
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
    int i;
    int level;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex_rh_grp_members_delete_replace.\n")));

    /*
     * NUM_PATHS must be non-zero when this function is called for member delete
     * operation, check and return if this condition is not met.
     */
    if (!lt_entry->rh_num_paths) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the group's ECMP_ID value and current NUM_PATHS values. */
    ecmp_id = lt_entry->ecmp_id;
    grp_npaths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

    /*
     * Determine how many members have been deleted based on the updated
     * NUM_PATHS value.
     */
    deleted_cnt = (grp_npaths - lt_entry->rh_num_paths);

    /*
     * Copy existing group RH entries array into LT entry RH entries array for
     * members delete + replace operation.
     */
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                   (sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                    lt_entry->max_paths));
    }
    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->max_paths));

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

    level = (lt_entry->grp_type == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)? 0: 1;

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
            if (bcmecmp_flex_compare_idx(unit, lt_entry, uidx, idx, 0)
                && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx];
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
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx] = 0;
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

    /*
     * If there are members to be replaced, identify the members from the
     * updated NHOP members array that will be replacing the current members.
     * Save the new member index value that is replacing a current member and
     * also assign the current members entry usage count value to the new
     * member in LT entry array for use during delete member rebalance
     * calculations.
     */
    if (replace_cnt) {
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
                if (!BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx]) {
                    /*
                     * This replacement member candidate has already been
                     * associate with a new member from the updated array, move
                     * on to the next member in the replace mindex array.
                     */
                    continue;
                }
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx];
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[rep_idx] = 0;

                /*
                 * Save the new member index value that is replacing the current
                 * replacement member.
                 */
                new_mindex[idx] = uidx;

                break;

            }
        }
    }

    /* Perform members delete and rebalance operation. */
    for (idx = 0; idx < deleted_cnt; idx++) {
        /* Delete the member and re-balance the group entries. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmecmp_flex_rh_grp_memb_delete(unit,
                                        deleted_mindex[idx],
                                        lt_entry));
    }

    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_new_mindex_cnt) {
        for (idx = 0; idx < grp_npaths; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMECMP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                if (bcmecmp_flex_compare_rh_ent_idx(unit, BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit), idx, lt_entry, uidx, 0)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {

                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
            shrd_new_mindex_cnt--;
        }
    }

    /*
     * For the replaced members, check and update the member index value with
     * the new members index value and also update the NHOP member value in
     * rh_entries_array.
     */
    if (replace_cnt) {
        for (idx = 0; idx < replace_cnt; idx++) {
            rep_idx = replace_mindex[idx];
            for (uidx = 0; uidx < lt_entry->max_paths; uidx++) {
                if (bcmecmp_flex_compare_rh_ent_idx(unit, BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit), rep_idx, lt_entry, uidx, 0)
                    && (rep_idx == lt_entry->rh_mindex_arr[uidx])) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    if (level == 0) {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][uidx] = lt_entry->member0_field[i][new_idx];
                        }
                    } else {
                        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                            lt_entry->rh_entries_arr[i][uidx] = lt_entry->member1_field[i][new_idx];
                        }
                    }
                }
            }
        }
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMECMP_FREE(deleted_mindex);
        BCMECMP_FREE(replace_mindex);
        BCMECMP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMECMP_FREE(new_mindex);
        }
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
bcmecmp_flex_group_rh_entries_update(int unit,
                                     bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_rh_oper_t rh_oper = BCMECMP_RH_OPER_NONE; /* RH oper type. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmecmp_flex_group_rh_entries_update.\n")));

    /* Get the type of RH-ECMP group operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_flex_group_rh_oper_get(unit, lt_entry, &rh_oper));

    /* Invoke operation dependent function. */
    switch (rh_oper) {
        case BCMECMP_RH_OPER_NONE:
            SHR_EXIT();
            break;
        case BCMECMP_RH_OPER_ADD:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_members_add(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_members_delete(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_members_replace(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_ADD_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_members_add_replace(unit, lt_entry));
            break;
        case BCMECMP_RH_OPER_DELETE_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmecmp_flex_rh_grp_members_delete_replace(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
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
bcmecmp_flex_rh_grp_members_recover(int unit,
                               bcmecmp_flex_lt_entry_t *lt_entry)
{

    bcmecmp_flex_lt_entry_t *rhg_entry = NULL; /* Resilient hash group entry. */
    uint32_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    uint32_t idx = 0, memb_idx;      /* Index iterator variables. */
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    int i;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));

    /*
     * Allocate memory for resilient hashing group entry for performing the get
     * operation.
     */
    BCMECMP_ALLOC(rhg_entry, sizeof(*rhg_entry), "bcmecmpBcmRhgEntry");
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        BCMECMP_ALLOC
            (rhg_entry->rh_entries_arr[i],
             lt_entry->max_paths * sizeof(*(lt_entry->rh_entries_arr[i])),
             "bcmecmpRhGrpAddLtEntRhEntArr");
    }

    /*
     * Setup input attribute values necessary for reading the RH group hardware
     * table entries.
     */
    rhg_entry->glt_sid = lt_entry->glt_sid;
    rhg_entry->op_arg = lt_entry->op_arg;
    rhg_entry->grp_type = lt_entry->grp_type;
    rhg_entry->ecmp_id = lt_entry->ecmp_id;
    rhg_entry->max_paths = lt_entry->max_paths;
    ecmp_id = rhg_entry->ecmp_id;

    /* Get RH-ECMP Group entry from hardware tables. */
    SHR_IF_ERR_EXIT
        (BCMECMP_FNCALL_EXEC(unit, grp_get)(unit, rhg_entry));

    lbound = (BCMECMP_FLEX_GRP_RH_ENTRIES_CNT
                    (unit, ecmp_id) / BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id));
    ubound = ((BCMECMP_FLEX_GRP_RH_ENTRIES_CNT(unit, ecmp_id) %
                    BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id))
                    ? (lbound + 1) : lbound);
    threshold = ubound;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\tLBND=%u UBND=%u THRSLD=%u LB_MODE=%u "
                            "max_paths=%u mstart_idx=%d\n"),
                lbound,
                ubound,
                threshold,
                rhg_entry->lb_mode,
                rhg_entry->max_paths,
                rhg_entry->mstart_idx));

    /*
     * Verify retrieved total RH group member table entries count matches
     * stored/expected value before further processing for this RH group.
     */
    if (rhg_entry->max_paths
        != BCMECMP_FLEX_GRP_RH_ENTRIES_CNT(unit, ecmp_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for RHG members structure. */
    BCMECMP_ALLOC(BCMECMP_FLEX_GRP_VOID_RHG_PTR(unit, ecmp_id),
                  sizeof(*BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)),
                  "bcmecmpRhGrpMembRcvrRhgInfo");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_FLEX_GRP_VOID_RHG_BK_PTR(unit, ecmp_id),
                      sizeof(*BCMECMP_FLEX_GRP_RHG_BK_PTR(unit, ecmp_id)),
                      "bcmecmpRhGrpMembRcvrRhgInfoBk");
    }

    /* Allocate memory to store RHG member table entries. */
    for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
        BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                      (rhg_entry->max_paths *
                       sizeof(*BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))),
                      "bcmecmpRhGrpMembRcvrRhEntArr");
        if (BCMECMP_ATOMIC_TRANS(unit)) {
            BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i),
                          (rhg_entry->max_paths *
                           sizeof(*BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i))),
                          "bcmecmpRhGrpMembRcvrRhEntArrBk");
        }

        /* Copy load balanced RH members array. */
        sal_memcpy(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                   rhg_entry->rh_entries_arr[i],
                   sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i))) *
                   rhg_entry->max_paths);
        if (BCMECMP_ATOMIC_TRANS(unit)) {
            sal_memcpy(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i),
                       rhg_entry->rh_entries_arr[i],
                       sizeof(*(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i))) *
                       rhg_entry->max_paths);
        }
    }

    /*
     * Allocate memory to store group RH-ECMP entries mapped member
     * index information.
     */
    BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                  (rhg_entry->max_paths *
                   sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMindexArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                      (rhg_entry->max_paths *
                       sizeof(*(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMindexArrBk");
    }

    /*
     * Allocate memory to store RH-ECMP members entries usage count
     * information.
     */
    BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                  (BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) *
                   sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)))),
                  "bcmecmpRhGrpMembRcvrRhMembEntCntArr");
    if (BCMECMP_ATOMIC_TRANS(unit)) {
        BCMECMP_ALLOC(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                      (BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) *
                       sizeof(*(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, ecmp_id)))),
                      "bcmecmpRhGrpMembRcvrRhMembEntCntArrBk");
    }

    for (memb_idx = 0;
         memb_idx < BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->max_paths; idx++) {
            if ((rhg_entry->rh_entries_arr[0][idx] != (uint32_t)(BCMECMP_INVALID)) &&
                (bcmecmp_flex_compare_rh_ent_idx(unit,
                                   BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit),
                                   memb_idx, rhg_entry, idx, 0))
                && BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < lbound) {
                /* Update member index mapping */
                BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[0][idx] = BCMECMP_INVALID;

                /* Move on to the next member as lower bound as been reached. */
                if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
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
         memb_idx < BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->max_paths; idx++) {
            if ((rhg_entry->rh_entries_arr[0][idx] != (uint32_t)(BCMECMP_INVALID)) &&
                (bcmecmp_flex_compare_rh_ent_idx(unit,
                                BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit),
                                memb_idx, rhg_entry, idx, 0))
                && BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, ecmp_id)[memb_idx] < ubound) {
                /* Update member index mapping */
                BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)[idx] = memb_idx;

                /* Update member table entries usage count for this member. */
                BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[0][idx] = BCMECMP_INVALID;

                /* Move on to the next member as upper bound as been reached. */
                if (BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)[idx]
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
        sal_memcpy(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id),
                   BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id)) *
                    rhg_entry->max_paths));

        /*
         * Copy members entry usage count array elements into the backup
         * array.
         */
        sal_memcpy(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id),
                   BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id)) *
                    BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id)));
    }

    exit:
        if (rhg_entry) {
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_FREE(rhg_entry->rh_entries_arr[i]);
            }
            BCMECMP_FREE(rhg_entry);
        }
        if (SHR_FUNC_ERR() && ecmp_id != (uint32_t)(BCMECMP_INVALID)) {
            if (BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)) {
                for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i));
                }
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_FLEX_GRP_VOID_RHG_PTR(unit, ecmp_id));
            }

            if (BCMECMP_ATOMIC_TRANS(unit)
                && BCMECMP_FLEX_GRP_RHG_BK_PTR(unit, ecmp_id)) {
                for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                    BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_BK_PTR(unit, ecmp_id, i));
                }
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, ecmp_id));
                BCMECMP_FREE(BCMECMP_FLEX_GRP_VOID_RHG_BK_PTR(unit, ecmp_id));
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
bcmecmp_flex_group_update(int unit,
                          bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bcmecmp_flex_lt_entry_t *cur_lt_entry;
    bool in_use; /* Group's in use status. */
    uint32_t max_paths = 0, num_paths = 0, comp_id;
    int i;
    uint32_t hw_ecmp_id;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_update.\n")));

    /* Get ECMP flexible group identifier value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP flexible group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT(bcmecmp_grp_in_use(unit,
                                       ecmp_id,
                                       FALSE,
                                       0,
                                       gtype,
                                       &in_use,
                                       &comp_id));

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
        && !BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored wcmp_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_WEIGHTED_SIZE)
        && BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size = BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_SIZE=%d.\n"),
                     lt_entry->weighted_size));
    }

    /*
     * Use stored group max_paths value for update operation if it has not been
     * supplied.
     */
    if (!(lt_entry->rh_mode) &&
        (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_MAX_PATHS))) {

        /* Retrive and use group max_paths value. */
        lt_entry->max_paths = BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MAX_PATHS=%u.\n"), lt_entry->max_paths));
    } else if (lt_entry->max_paths !=
               BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id)) {
        /*resize the ECMP flexible group*/
        SHR_IF_ERR_VERBOSE_EXIT(bcmecmp_flex_group_max_paths_update(unit, lt_entry));
    }

    /*
     * Use stored group num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "NUM_PATHS=%u.\n"), lt_entry->num_paths));
    }

    /*
     * Use stored group rh_mode value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_MODE)) {

        /* Retrive and use group rh_mode value. */
        lt_entry->rh_mode = BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_mode));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_RANDOM_SEED)) {

        /* Retrive and use group rh_mode value. */
        lt_entry->rh_rand_seed = BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_rand_seed));
    }

    /*
     * Use stored group rh_num_paths value for update operation if it has not
     * been supplied.
     */
    if (!BCMECMP_LT_FIELD_GET(lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)) {

        /* Retrive and use group num_paths value. */
        lt_entry->rh_num_paths = BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_NUM_PATHS=%u.\n"), lt_entry->rh_num_paths));
    }

    /* Get logical table entry buffer pointer. */
    cur_lt_entry = BCMECMP_CURRENT_FLEX_LT_ENT_PTR(unit);

    if ((gtype == BCMECMP_GRP_TYPE_LEVEL0) ||
        (gtype == BCMECMP_GRP_TYPE_LEVEL0_WEIGHTED)) {
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_fields_fill(unit, cur_lt_entry, 0,
                                             lt_entry,
                                            (lt_entry->rh_mode)?FALSE:TRUE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmecmp_flex_member_fields_fill(unit, cur_lt_entry, 1,
                                             lt_entry,
                                             (lt_entry->rh_mode)?FALSE:TRUE));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX
                                (unit, ecmp_id);
    max_paths = lt_entry->max_paths;
    num_paths = lt_entry->num_paths;


    /*
     * Check if group's LB_MODE is RESILIENT and call the corresponding
     * group install routine.
     */
    if (lt_entry->rh_mode) {
        lt_entry->num_paths = lt_entry->rh_num_paths;
        num_paths = lt_entry->rh_num_paths;
        if (lt_entry->rh_num_paths > lt_entry->max_paths) {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
        /*
         * Allocate memory to store the RH balanced nhop members for
         * installation in hardware tables.
         */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            BCMECMP_ALLOC
                    (lt_entry->rh_entries_arr[i],
                     lt_entry->max_paths * sizeof(*(lt_entry->rh_entries_arr[i])),
                     "bcmecmpRhGrpAddLtEntRhEntArr");
        }

        /*
         * Allocate memory to store the RH group entry's mapped member index
         * data.
         */
        BCMECMP_ALLOC
            (lt_entry->rh_mindex_arr,
             lt_entry->max_paths * sizeof(*(lt_entry->rh_mindex_arr)),
             "bcmecmpRhGrpAddLtEntRhMindexArr");

        /*
         * Allocate memory to store the RH group members entries usage count
         * data.
         */
        if (lt_entry->rh_num_paths) {
            BCMECMP_ALLOC
                (lt_entry->rh_memb_ecnt_arr,
                 lt_entry->rh_num_paths * sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                 "bcmecmpRhGrpAddLtEntRhMembEntCntArr");
        }

        /*
         * Set RH random seed from the user, which is used for selecting member
         */
        BCMECMP_FLEX_GRP_RH_RAND_SEED(unit, ecmp_id) = lt_entry->rh_rand_seed;

        /*
         * If NUM_PATHS value for this RH-group was zero prior to this update
         * operation and valid next-hop members are being added to this RH-group
         * in this UPDATE operation i.e. NUM_PATHS > 0, then this UPDATE
         * operation is equivalent to new next-hop members INSERT operation for
         * this group. So, use RH entries SET function for load balance
         * operation.
         */
        if ((BCMECMP_LT_FIELD_GET
                (lt_entry->fbmp, BCMECMP_LT_FIELD_RH_NUM_PATHS)
                && num_paths
                && !BCMECMP_FLEX_GRP_RH_ENTRIES_CNT(unit, ecmp_id)) ||
            (lt_entry->max_paths != cur_lt_entry->max_paths) ||
            (BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) == 0)) {
            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_rh_entries_set(unit, lt_entry));
        } else {
            /* Check if pre-config flag is set for this group. */
            hw_ecmp_id = lt_entry->ecmp_id;
            if (lt_entry->ecmp_id >= BCMECMP_TBL_SIZE(unit, group,
                                         BCMECMP_GRP_TYPE_LEVEL0)) {
                 hw_ecmp_id = lt_entry->ecmp_id - BCMECMP_TBL_SIZE(
                                         unit, group,
                                         BCMECMP_GRP_TYPE_LEVEL0);
            }
            if (BCMECMP_FLEX_ENT_PRECONFIG_GET
                    (BCMECMP_TBL_PTR(unit, group, gtype), hw_ecmp_id)) {

                /* Re-build RH-ECMP group members state for this group. */
                SHR_IF_ERR_EXIT
                    (bcmecmp_flex_rh_grp_members_recover(unit, lt_entry));

                /* Clear pre-config flag for this RH-ECMP group. */
                BCMECMP_FLEX_ENT_PRECONFIG_CLEAR
                    (BCMECMP_TBL_PTR(unit, group, gtype), hw_ecmp_id);
            }

            /*
             * This is an existing RH group members Add/Delete/Replace type of
             * operation. So, use RH entries UPDATE function to perform the
             * rebalancing work.
             */
            SHR_IF_ERR_EXIT
                (bcmecmp_flex_group_rh_entries_update(unit, lt_entry));
        }
    }

    /* Install ECMP Group in hardware tables. */
    SHR_IF_ERR_EXIT(BCMECMP_FNCALL_EXEC(unit, grp_ins)(unit, lt_entry));

    if (lt_entry->rh_mode) {
        /* Copy updated group load balanced RH member entries array. */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            sal_memcpy(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i),
                       lt_entry->rh_entries_arr[i],
                       (sizeof(*BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i)) *
                              lt_entry->max_paths));
        }

        /* Copy updated group entries mapped member index array. */
        sal_memcpy(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id),
                   lt_entry->rh_mindex_arr,
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id)) *
                          lt_entry->max_paths));

        /* Copy updated members entry usage count array. */
        sal_memcpy(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id),
                   lt_entry->rh_memb_ecnt_arr,
                   (sizeof(*BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id)) *
                    num_paths));
    }

    if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {
        BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = lt_entry->lb_mode;
        BCMECMP_FLEX_GRP_WEIGHTED_MODE(unit, ecmp_id) = FALSE;
    } else {
        BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = lt_entry->weighted_size;
        BCMECMP_FLEX_GRP_WEIGHTED_MODE(unit, ecmp_id) = lt_entry->weighted_mode;
    }
    BCMECMP_FLEX_GRP_NUM_PATHS(unit, ecmp_id) = num_paths;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[EID=%d]: Max_Paths=%u Num_Paths=%u LB=%u.\n"),
                ecmp_id,
                max_paths,
                num_paths,
                lt_entry->lb_mode));

    exit:
        /* Free memory allocated for RH group re-balance operation. */
        if (lt_entry->rh_mode) {

            /* Free temp RH group entries array. */
            for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
                BCMECMP_FREE(lt_entry->rh_entries_arr[i]);
                lt_entry->rh_entries_arr[i] = NULL;
            }

            /* Free temp RH group entry mapped member index array. */
            BCMECMP_FREE(lt_entry->rh_mindex_arr);

            /* Free temp RH group member entries usage count array. */
            BCMECMP_FREE(lt_entry->rh_memb_ecnt_arr);
            lt_entry->rh_mindex_arr = NULL;
            lt_entry->rh_memb_ecnt_arr = NULL;
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
bcmecmp_flex_group_delete(int unit,
                          bcmecmp_flex_lt_entry_t *lt_entry)
{
    bcmecmp_id_t ecmp_id = BCMECMP_INVALID; /* Group identifier. */
    bcmecmp_grp_type_t gtype = BCMECMP_GRP_TYPE_COUNT; /* Group level. */
    bool in_use; /* Group's in use status. */
    uint32_t comp_id = 0;
    int i;
    uint32_t hw_ecmp_id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmecmp_group_delete.\n")));

    /* Get ECMP ID value. */
    ecmp_id = lt_entry->ecmp_id;

    /* Get ECMP flexible group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT(bcmecmp_grp_in_use(unit,
                                       ecmp_id,
                                       FALSE,
                                       0,
                                       gtype,
                                       &in_use,
                                       &comp_id));

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
    /* Delete ECMP Group from hardware tables. */
    SHR_IF_ERR_EXIT
        (BCMECMP_FNCALL_EXEC(unit, grp_del)(unit, lt_entry));

    /* Free memory allocated for RH group info. */
    if ((BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id)) && BCMECMP_FLEX_GRP_RHG_PTR(unit, ecmp_id)) {

        /* Check if pre-config flag is set for this group. */
        hw_ecmp_id = lt_entry->ecmp_id;
        if (lt_entry->ecmp_id >= BCMECMP_TBL_SIZE(unit, group,
                                                 BCMECMP_GRP_TYPE_LEVEL0)) {
            hw_ecmp_id = lt_entry->ecmp_id -
                         BCMECMP_TBL_SIZE(unit, group, BCMECMP_GRP_TYPE_LEVEL0);
        }
        if (BCMECMP_FLEX_ENT_PRECONFIG_GET
                (BCMECMP_TBL_PTR(unit, group, gtype), ecmp_id)) {
            /* Clear pre-config flag for this RH-ECMP group. */
            BCMECMP_FLEX_ENT_PRECONFIG_CLEAR
                (BCMECMP_TBL_PTR(unit, group, gtype), hw_ecmp_id);
        }

        /*
         * Free memory allocated for storing per RH entry mapped member's
         * index information.
         */
        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MINDEX_PTR(unit, ecmp_id));

        /*
         * Free memory allocated for storing RH load balanced group
         * entries.
         */
        for (i = 0; i < BCMECMP_MEMBER_COUNT; ++i) {
            BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_ENTRIES_PTR(unit, ecmp_id, i));
        }

        /*
         * Free memory allocated for storing per member RH entries usage
         * count.
         */
        BCMECMP_FREE(BCMECMP_FLEX_GRP_RH_MEMB_ENTCNT_PTR(unit, ecmp_id));

        /* Free memory allocated for storing group's RH info. */
        BCMECMP_FREE(BCMECMP_FLEX_GRP_VOID_RHG_PTR(unit, ecmp_id));
    }

    /* Release the ITBM resouce for Member entries. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR
            (unit, member, gtype)),
            BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id),
            SHR_ITBM_INVALID,
            BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id)));

    /* Release the ITBM resouce for Group ID. */
    SHR_IF_ERR_EXIT
        (shr_itbm_list_block_free(BCMECMP_TBL_ITBM_LIST(BCMECMP_TBL_PTR
            (unit, group, gtype)),
            1,
            SHR_ITBM_INVALID,
            ecmp_id));
    /* Set in use flag in the bookkeeping for group attributes. */
    BCMECMP_FLEX_GRP_ITBM_USE(unit, ecmp_id) = 0;

    /* Clear/initialize ECMP Group information in Per-Group Info Array. */
    BCMECMP_FLEX_GRP_TYPE(unit, ecmp_id) = BCMECMP_GRP_TYPE_COUNT;
    BCMECMP_FLEX_GRP_LT_SID(unit, ecmp_id) = BCMLTD_SID_INVALID;
    if (!BCMECMP_FLEX_GRP_IS_WEIGHTED(gtype)) {
        BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_FLEX_LB_MODE_REGULAR;
    } else {
        BCMECMP_FLEX_GRP_LB_MODE(unit, ecmp_id) = BCMECMP_FLEX_WEIGHTED_SIZE_256;
    }
    BCMECMP_FLEX_GRP_RH_MODE(unit, ecmp_id) = FALSE;
    BCMECMP_FLEX_GRP_MAX_PATHS(unit, ecmp_id) = BCMECMP_LT_MAX_PATHS
                                                        (unit, group, gtype);
    BCMECMP_FLEX_GRP_MEMB_TBL_START_IDX(unit, ecmp_id) = BCMECMP_INVALID;

    exit:
        SHR_FUNC_EXIT();
}




