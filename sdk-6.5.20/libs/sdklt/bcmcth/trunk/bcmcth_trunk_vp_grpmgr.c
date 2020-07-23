/*! \file bcmcth_trunk_vp_grpmgr.c
 *
 * This file contains TRUNK_VP group management functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmcth/bcmcth_trunk_vp_common_imm.h>
#include <bcmcth/bcmcth_trunk_vp_lt_utils.h>
#include <bcmcth/bcmcth_trunk_vp_grpmgr.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Private functions
 */

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
bcmcth_trunk_vp_wali_print(int unit,
                   uint32_t wal_mentries,
                   shr_pb_t *mpb)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mpb, SHR_E_PARAM);
    SHR_NULL_CHECK(BCMCTH_TRUNK_VP_WALI_PTR(unit), SHR_E_PARAM);

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
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->igwsize,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->igr_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->igw_ecount,
                  0);
    shr_pb_printf(mpb,
                  "\n\tL3_ECMP_COUNT         %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->gwsize,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->gr_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->gw_ecount,
                  0);
    shr_pb_printf(mpb,
                  "\n\tINITIAL_L3_ECMP       %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->imwsize,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->imr_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->imw_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->imclr_ecount);
    shr_pb_printf(mpb,
                  "\n\tL3_ECMP               %-18u %-9u %-9u "
                  "%-10u\n",
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->mwsize,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->mr_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->mw_ecount,
                  BCMCTH_TRUNK_VP_WALI_PTR(unit)->mclr_ecount);
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
 * \brief Initalize bcmcth_trunk_vp_tbl_op_t structure.
 *
 * \param [in] op_info Pointer to bcmcth_trunk_vp_tbl_op_t_init structure.
 *
 * \return Nothing.
 */
static void
bcmcth_trunk_vp_tbl_op_t_init(bcmcth_trunk_vp_tbl_op_t *op_info)
{
    if (op_info) {
        op_info->oper_flags = BCMCTH_TRUNK_VP_OPER_NONE;
        op_info->num_ent = 0;
        op_info->free_idx = BCMCTH_TRUNK_VP_INVALID;
        op_info->tbl_ptr = NULL;
    }
    return;
}

/*!
 * \brief Initalize bcmcth_trunk_vp_grp_defrag_t structure members.
 *
 * \param [in] frag Pointer to bcmcth_trunk_vp_grp_defrag_t structure.
 *
 * \return Nothing.
 */
static void
bcmcth_trunk_vp_grp_defrag_t_init(bcmcth_trunk_vp_grp_defrag_t *frag)
{
    if (frag) {
        frag->oper_flags = 0;
        frag->glt_sid = BCMLTD_SID_INVALID;
        frag->trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
        frag->gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
        frag->mstart_idx = BCMCTH_TRUNK_VP_INVALID;
        frag->gsize = 0;
        frag->op_arg = NULL;
    }
    return;
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
 * \param [in] pb TRUNK_VP group type.
 * \param [in] src Pointer to member table entries in the source fragment.
 * \param [in] dest Pointer to member table entries in the destination fragment.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */
static int
bcmcth_trunk_vp_groups_defrag_oper(int unit,
                           shr_pb_t *pb,
                           bcmcth_trunk_vp_grp_type_t gtype,
                           uint32_t gap_sz,
                           const bcmcth_trunk_vp_grp_defrag_t *src,
                           const bcmcth_trunk_vp_grp_defrag_t *dest)
{
    const bcmcth_trunk_vp_tbl_prop_t *mtbl_ptr = NULL;
           /* Pointer to member table. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pb, SHR_E_PARAM);
    SHR_NULL_CHECK(src, SHR_E_PARAM);
    SHR_NULL_CHECK(dest, SHR_E_PARAM);

    mtbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit, member, src->gtype);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    shr_pb_printf(pb,
                  "\n\t%7d\t%5u ==> %-5u\t%5u ==> %-5u(%-5u)\t%5u\t%3u\n",
                  src->trunk_vp_id,
                  src->mstart_idx,
                  dest->mstart_idx,
                  src->gsize,
                  dest->gsize,
                  gap_sz,
                  src->gtype,
                  src->glt_sid);

    /* Copy entries from source region to destination region in member table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_vp_memb_cpy(unit, dest, src));

    /* Update Group's member table start index in HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_vp_gmstart_updt(unit, dest));

    /* Update SW copies of the group's member table start index value. */
    BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, dest->trunk_vp_id) =
            dest->mstart_idx;

    /*
     * Clear entries in the source region in member table that were previously
     * copied.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_drv_vp_memb_clr(unit, src));

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
bcmcth_trunk_vp_find_group(int unit, bcmcth_trunk_vp_grp_type_t gtype, uint32_t mstart_idx)
{
    int trunk_vp_id, gstart_idx, gend_idx, rv;
    bool in_use = FALSE;
    uint32_t comp_id = 0;

    gstart_idx = BCMCTH_TRUNK_VP_TBL_IDX_MIN(unit, group, gtype);

    gend_idx = gstart_idx + BCMCTH_TRUNK_VP_TBL_SIZE(unit, member, gtype) - 1;

    for (trunk_vp_id = gstart_idx; trunk_vp_id <= gend_idx; trunk_vp_id++) {
        /* Verify the group is in use. */
        rv = bcmcth_trunk_vp_grp_in_use(unit,
             trunk_vp_id,
             gtype,
             &in_use,
             &comp_id);
        if (rv != SHR_E_NONE) {
            return rv;
        }
        if (!in_use || (comp_id != BCMMGMT_TRUNK_COMP_ID)) {
            continue;
        }

        if (BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) == (int)mstart_idx) {
            return trunk_vp_id;
        }
    }

    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Defragment TRUNK_VP group member table entries.
 *
 * This function performs defragmentation of TRUNK_VP member table entries used by
 * TRUNK_VP groups, to create space for a new group.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to a LT entry strcture.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Member table pointer is NULL.
 */

static int
bcmcth_trunk_vp_itbm_groups_defragment(
                          int unit,
                          const bcmcth_trunk_vp_lt_entry_t *entry)
{

    shr_itbm_blk_t *itbm_blk = NULL;
    uint32_t itbm_blk_count;
    uint32_t tot_wal_mentries = 0; /* Total member table entries moved. */
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT; /* Group type. */
    uint32_t i, j;
    int rv = SHR_E_NONE;
    shr_pb_t *mpb = NULL; /* Member table debug info print buffer. */
    shr_pb_t *gpb = NULL; /* Gap debug info print buffer. */
    bcmcth_trunk_vp_tbl_prop_t *mtbl_ptr = NULL; /* Pointer to member table. */
    shr_itbm_defrag_blk_mseq_t *m_seq;
    bcmcth_trunk_vp_grp_defrag_t m_src, m_dest;
    shr_itbm_list_hdl_t hdl;
    bool atomic_enable = FALSE;
    uint32_t dfg_fblk_count = 0;
    shr_itbm_blk_t *dfg_fblk_arr = NULL;
    shr_itbm_blk_t dfg_lfblk;
    int prev_mstart_idx = -1;
    int new_mstart_idx = -1;
    uint32_t last_idx = 0;
    bool defrag_start = FALSE;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "%s.\n"), __func__));

    /* Get the group type. */
    gtype = entry->grp_type;
    atomic_enable = BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit);
    prev_mstart_idx =
         BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, entry->trunk_vp_id);

    /* Get the itbm list hdl for the gtype. */
    hdl = BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                      BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype));

    /* Get member table pointer. */
    mtbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype);
    SHR_NULL_CHECK(mtbl_ptr, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n\tgtype=%u G_tbl_sz=%u.\n"),
              gtype,
              BCMCTH_TRUNK_VP_TBL_SIZE(unit, group, gtype)));


    sal_memset(BCMCTH_TRUNK_VP_WALI_PTR(unit), 0, sizeof(*BCMCTH_TRUNK_VP_WALI_PTR(unit)));
    mpb = shr_pb_create();
    gpb = shr_pb_create();
    shr_pb_printf(mpb,
                  "\n\tTRUNK_VP_ID\tmstart_idx(s/d)\t SG.sz ==> DG.sz(Gap)\tgtype"
                  "\tSID\n");
    shr_pb_printf(mpb,
                  "\n\t=======\t===============\t=====================\t====="
                  "\t===\n");
    /*
     * Initialize group member start index to the table Min index value as we have
     * to start the compression operation from the first (Min) index of the
     * table.
     */
    shr_itbm_blk_t_init(&dfg_lfblk);
    SHR_IF_ERR_VERBOSE_EXIT
        (shr_itbm_list_defrag_start(hdl,
                                    &itbm_blk_count,
                                    &itbm_blk,
                                    &m_seq,
                                    &dfg_fblk_count,
                                    &dfg_fblk_arr,
                                    &dfg_lfblk));
    if (dfg_fblk_count) {
        /*
         * Free block size available.
         * 1. Check for the largest lock if availble for the
         * requested resources.
         * 2. Check if the free block sie meets the criteria for
         * resize.
         */
        if (dfg_lfblk.ecount  >= entry->max_members) {
            defrag_start = TRUE;
        } else {
            for (i = 0; i < itbm_blk_count; i++) {
                if (prev_mstart_idx != (int) itbm_blk[i].first_elem) {
                    continue;
                }
                last_idx = (int) (m_seq[i].mcount - 1);
                new_mstart_idx = m_seq[i].first_elem[last_idx];
                for (j = 0; j < dfg_fblk_count; j++) {
                    if (dfg_fblk_arr[j].first_elem ==
                       new_mstart_idx + itbm_blk[i].ecount) {
                        if ((dfg_fblk_arr[j].ecount +
                             itbm_blk[i].ecount) >= entry->max_members) {
                            defrag_start = TRUE;
                        }
                        break;
                    }
                }
            }
        }
    }

    if (defrag_start == FALSE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                 "GROUP_ID=%d cannot be inserted due to lack of resources."),
                  entry->trunk_vp_id));
        SHR_IF_ERR_VERBOSE_EXIT(
            shr_itbm_list_defrag_end(
                  BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                  BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                  false));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    bcmcth_trunk_vp_grp_defrag_t_init(&m_src);
    m_src.glt_sid = entry->glt_sid;
    m_src.gtype = gtype;
    m_src.op_arg = entry->op_arg;
    bcmcth_trunk_vp_grp_defrag_t_init(&m_dest);
    m_dest.glt_sid = entry->glt_sid;
    m_dest.gtype = gtype;
    m_dest.op_arg = entry->op_arg;
    for (i = 0; i < itbm_blk_count; ++i) {
        m_src.mstart_idx = itbm_blk[i].first_elem;
        m_src.gsize = itbm_blk[i].ecount;
        m_src.trunk_vp_id = bcmcth_trunk_vp_find_group(unit, gtype, itbm_blk[i].first_elem);
        m_dest.trunk_vp_id = m_src.trunk_vp_id;
        for (j = 0; j < (m_seq[i].mcount); ++j) {
            /*Move to first_elem[j]*/
            m_dest.mstart_idx = m_seq[i].first_elem[j];
            m_dest.gsize = m_seq[i].ecount;

            rv = bcmcth_trunk_vp_groups_defrag_oper(unit,
                                                    mpb,
                                                    gtype,
                                                    m_dest.gsize,
                                                    &m_src,
                                                    &m_dest);

            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(
                      BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                         member, gtype)), false));
                return rv;
            }

            m_src.mstart_idx = m_dest.mstart_idx;
            m_src.gsize = m_dest.gsize;

        }
        BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_SET(
                BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY), m_src.trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_TRANS_ID(unit, m_src.trunk_vp_id) = entry->op_arg->trans_id;

        if (atomic_enable) {
            BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_SET(
                BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY), m_src.trunk_vp_id);
            BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, m_src.trunk_vp_id) = entry->op_arg->trans_id;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                      BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)), true));

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
                     bcmcth_trunk_vp_wali_print(unit, tot_wal_mentries, mpb)));
            shr_pb_destroy(mpb);
        }
        SHR_FUNC_EXIT();
}

static uint32_t
bcmcth_trunk_vp_mbr_tbl_max(int unit, bcmcth_trunk_vp_grp_type_t gtype) {
    uint32_t max_idx = 0;
    if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) {
        max_idx = BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) + 1;
        if (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)) {
            max_idx += BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member2,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) + 1;
        }
    } else if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) {
        max_idx = BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) + 1;
        if (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)) {
            max_idx += BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member2,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) + 1;
        }
    }
    return max_idx;
}

/*!
 * \brief update the max_members of trunk_vp group.
 *
 * Update the ref-count of reserved entries in member table and member start
 * idx in group:
 * If free entries are available in member table, use them;
 * If free entries are not enough, defragment the groups and try again.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_group_max_members_update(
                                   int unit,
                                   bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    uint32_t new_max_members;
    uint32_t old_max_members;
    int old_mstart_idx;
    int new_mstart_idx = 0;
    bcmcth_trunk_vp_tbl_op_t tbl_op; /* Table operation info. */
    int rv = SHR_E_NONE;                  /* Return value. */
    shr_pb_t *mpb = NULL;
    bcmcth_trunk_vp_grp_defrag_t src_block;  /* Fragmented Group info. */
    bcmcth_trunk_vp_grp_defrag_t dst_block; /* Gap fragment info. */
    uint32_t bucket = SHR_ITBM_INVALID;
    const bcmltd_op_arg_t *op_arg = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_max_members_update.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

    /* Operational argument. */
    op_arg = lt_entry->op_arg;

    /* Initialize group TRUNK_VP_ID and group type local variables. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    new_max_members = lt_entry->max_members;
    old_max_members = BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);
    old_mstart_idx = BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id);

    if (new_max_members < old_max_members) {
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                  member, gtype))) {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_resize
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     old_max_members,
                     SHR_ITBM_INVALID,
                     old_mstart_idx,
                     new_max_members));
        }
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) = new_max_members;
        SHR_EXIT();
    }

    if (new_max_members > old_max_members) {
         /* Check if it is possible to increase max grp size in-place. */
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                  member, gtype))) {
            if (old_mstart_idx + new_max_members >
                bcmcth_trunk_vp_mbr_tbl_max(unit, gtype)) {
                rv = SHR_E_RESOURCE;
            } else {
                rv = shr_itbm_list_block_resize
                        (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                            (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                         old_max_members,
                         SHR_ITBM_INVALID,
                         old_mstart_idx,
                         new_max_members);
            }
            if (SHR_SUCCESS(rv)) {
                BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) =
                                       new_max_members;
            }
        }

        if (rv != SHR_E_RESOURCE) {
            SHR_ERR_EXIT(rv);
        }

        /* A new start index in member table is needed. */
        /* Initialize table operation structure and set the operation type. */
        bcmcth_trunk_vp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMCTH_TRUNK_VP_OPER_ADD;
        tbl_op.num_ent = new_max_members;
        tbl_op.tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype);
        /* Get free base index from member table. */
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                  member, gtype))) {
            uint32_t free_idx;
            rv = shr_itbm_list_block_alloc
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     lt_entry->max_members,
                     &bucket,
                     &free_idx);
            LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "RM rv=%d free %d %d.\n"), rv, free_idx, gtype));
            if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {
                /* Defragment the member table and retry for index allocation. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_vp_itbm_groups_defragment(unit, lt_entry));
                old_mstart_idx = BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id);
                /* Check if it is possible to icrease max grp size in place. */
                if (old_mstart_idx + new_max_members - 1 <=
                    bcmcth_trunk_vp_mbr_tbl_max(unit, gtype)) {
                    rv = shr_itbm_list_block_resize(BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                        old_max_members,
                        SHR_ITBM_INVALID,
                        old_mstart_idx,
                        new_max_members);
                    if (SHR_E_NONE == rv) {
                        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(
                                             unit,
                                             trunk_vp_id) = new_max_members;
                        SHR_ERR_EXIT(rv);
                    }
                }
                rv = shr_itbm_list_block_alloc
                        (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                            (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                         lt_entry->max_members,
                         &bucket,
                         &free_idx);
            }
            if (SHR_E_NONE != rv) {
                SHR_ERR_EXIT(rv);
            }
            /*move to the new start index*/
            new_mstart_idx = free_idx;
            lt_entry->mstart_idx = free_idx;
        }
        sal_memset(BCMCTH_TRUNK_VP_WALI_PTR(unit), 0,
                    sizeof(*BCMCTH_TRUNK_VP_WALI_PTR(unit)));
        mpb = shr_pb_create();
        shr_pb_printf(mpb,
              "\n\tTRUNK_VP_ID\tmstart_idx(s/d)\t SG.sz ==> DG.sz(Gap)\tgtype"
              "\tSID\n");

        bcmcth_trunk_vp_grp_defrag_t_init(&src_block);
        bcmcth_trunk_vp_grp_defrag_t_init(&dst_block);

        src_block.trunk_vp_id = dst_block.trunk_vp_id = lt_entry->trunk_vp_id;
        src_block.op_arg = dst_block.op_arg = op_arg;
        src_block.glt_sid = dst_block.glt_sid = lt_entry->glt_sid;
        src_block.gtype = dst_block.gtype = lt_entry->grp_type;

        src_block.mstart_idx = old_mstart_idx;
        src_block.gsize = old_max_members;
        dst_block.mstart_idx = new_mstart_idx;
        dst_block.gsize =  old_max_members;

        SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_vp_groups_defrag_oper(
                     unit,
                     mpb,
                     gtype,
                     0,
                     &src_block,
                     &dst_block));
        /*
         * Free the ITBM block resource starting from old_mstart_idx.
         */
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                 old_max_members,
                 SHR_ITBM_INVALID,
                 old_mstart_idx));

        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) =
                     new_mstart_idx;
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
 * \brief Add an TRUNK_VP Group logical table entry to hardware tables.
 *
 * Reserves hardware table resources and installs TRUNK_VP logical table entry
 * fields to corresponding hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
int
bcmcth_trunk_vp_group_add(int unit,
                          bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_tbl_op_t tbl_op; /* Table operation info. */
    bool in_use = FALSE;
    uint32_t max_members = 0, member_cnt = 0;
    int rv = SHR_E_NONE;
    uint32_t comp_id = 0;
    uint32_t bucket = SHR_ITBM_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_add.\n")));
    /* Get TRUNK_VP group identifier value. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    /* Get TRUNK_VP group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT(bcmcth_trunk_vp_grp_in_use(
                    unit, trunk_vp_id, gtype,
                    &in_use, &comp_id));

    if (in_use) {
        if (comp_id == BCMMGMT_TRUNK_COMP_ID) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "TRUNK_VP_ID=%d already Exists.\n"), trunk_vp_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Group ID = %d is used by another component %d"),
                             trunk_vp_id, comp_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "TRUNK_VP_ID=%d is New.\n"), trunk_vp_id));

        /* Initialize table operation structure and set the operation type. */
        bcmcth_trunk_vp_tbl_op_t_init(&tbl_op);
        tbl_op.oper_flags |= BCMCTH_TRUNK_VP_OPER_ADD;

        /*
         * Initialize number of entries to be reserved in member table based on
         * group's LB_MODE.
         */
        tbl_op.num_ent = lt_entry->max_members;
        max_members = lt_entry->max_members;
        member_cnt = lt_entry->member_cnt;

        tbl_op.tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype);

        /* Get free base index from member table. */
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                  member, gtype))) {
            uint32_t free_idx;
            rv = shr_itbm_list_block_alloc
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     lt_entry->max_members,
                     &bucket,
                     &free_idx);

            if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {
                /* Defragment the member table and retry for index allocation. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_vp_itbm_groups_defragment(unit, lt_entry));
                rv = shr_itbm_list_block_alloc
                        (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                            (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                         lt_entry->max_members,
                         &bucket,
                         &free_idx);
                if (SHR_E_NONE != rv) {
                    SHR_ERR_EXIT(rv);
                }
            }
            lt_entry->mstart_idx = free_idx;
        }
        if (SHR_E_NONE != rv) {
            SHR_ERR_EXIT(rv);
        }
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_drv_vp_grp_insert(unit, lt_entry));

        /* Store TRUNK_VP Group information in Per-Group Info Array. */
        BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id) = lt_entry->grp_type;
        BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id) = lt_entry->glt_sid;
        if (!BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) = lt_entry->lb_mode;
        } else {
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                   lt_entry->weighted_size;
        }
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) = max_members;
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) = member_cnt;
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) =
                   lt_entry->mstart_idx;

        /* Allocate the ITBM resource for Group ID and mark in use. */
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
                              group, gtype))) {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_alloc_id
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                     1,
                     bucket,
                     trunk_vp_id));
            BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id) = 1;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "[TRUNK_VP_ID=%d]: mstart_idx=%d.\n"),
                    trunk_vp_id,
                    BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id)));

        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Add: TRUNK_VP_ID=%d ITBM_IN_USE=%d "
                              "SID[%d] != [%d].\n"),
                   trunk_vp_id,
                   BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id),
                   BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id),
                   lt_entry->glt_sid));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Update installed TRUNK_VP logical table entry field values.
 *
 * Update an existing TRUNK_VP logical table entry field values.
 * Multiple fields can
 * be updated in a single update call.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 * \return SHR_E_UNAVAIL Feature not supported yet.
 */
int
bcmcth_trunk_vp_group_update(
                     int unit,
                     bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bool in_use;
    uint32_t max_members = 0, member_cnt = 0;
    uint32_t comp_id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_group_update.\n")));

    /* Get TRUNK_VP group identifier value. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    /* Get TRUNK_VP group level value. */
    gtype = lt_entry->grp_type;

    SHR_IF_ERR_EXIT(bcmcth_trunk_vp_grp_in_use(unit,
               trunk_vp_id, gtype, &in_use,
               &comp_id));

    /* Verify the group is in use. */
    if (!in_use) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d not found.\n"), trunk_vp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        if (comp_id == BCMMGMT_TRUNK_COMP_ID) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "TRUNK_VP_ID=%d in use.\n"), trunk_vp_id));
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Group ID = %d is used by another component %d"),
                             trunk_vp_id, comp_id));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /*
     * Use stored group lb_mode value for update operation if it has not been
     * supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
        BCMCTH_TRUNK_VP_LT_FIELD_LB_MODE)
        && !BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->lb_mode = BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "LB_MODE=%d.\n"), lt_entry->lb_mode));
    }

    /*
     * Use stored wcmp_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
        BCMCTH_TRUNK_VP_LT_FIELD_WEIGHTED_SIZE)
        && BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size =
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id);

        lt_entry->max_members =
                bcmcth_trunk_vp_weighted_size_convert_to_member_cnt(
                 lt_entry->weighted_size);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "WEIGHTED_SIZE=%d.\n"), lt_entry->weighted_size));
    }

    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
        BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)) {

        /* Retrive and use group max_members value. */
        lt_entry->max_members =
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                   "MAX_MEMBERS=%u.\n"), lt_entry->max_members));
    } else if (lt_entry->max_members !=
               BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id)) {
        /*resize the TRUNK_VP group*/
        SHR_IF_ERR_EXIT(
             bcmcth_trunk_vp_group_max_members_update(unit, lt_entry));
    }

    /*
     * Use stored group member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(lt_entry->fbmp,
        BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)) {
        /* Retrive and use group member_cnt value. */
        lt_entry->member_cnt =
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MEMBER_CNT=%u.\n"), lt_entry->member_cnt));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX
                                (unit, trunk_vp_id);

    max_members = lt_entry->max_members;
    member_cnt = lt_entry->member_cnt;

    if (max_members < member_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit,
              "TRUNK_VP_ID=%d MAX_MEMBERS=%d cannot be less than MEMBER_CNT=%d.\n"),
               trunk_vp_id, max_members, member_cnt));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Install TRUNK_VP Group in hardware tables. */
    SHR_IF_ERR_EXIT
            (bcmcth_trunk_drv_vp_grp_insert(unit, lt_entry));
    BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) = lt_entry->max_members;
    BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) = lt_entry->lb_mode;
    BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) = member_cnt;
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[EID=%d]: Max_Paths=%u Num_Paths=%u LB=%u.\n"),
                trunk_vp_id,
                max_members,
                member_cnt,
                lt_entry->lb_mode));

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an TRUNK_VP Group logical table entry from hardware tables.
 *
 * Delete an TRUNK_VP Group logical table entry from hardware tables.
 * TRUNK_VP member table resources consumed by the group
 * are returned to the free pool as part
 * of this delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_NOT_FOUND Entry supplied for update not found.
 */
int
bcmcth_trunk_vp_group_delete(int unit,
                             bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bool in_use;
    uint32_t comp_id;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "bcmcth_trunk_vp_group_delete.\n")));

    /* Get TRUNK_VP ID value. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    /* Get TRUNK_VP group level value. */
    gtype = lt_entry->grp_type;

    /* Get the max_members and member table start index. */
    lt_entry->max_members =
         BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);
    lt_entry->mstart_idx =
         BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id);

    SHR_IF_ERR_EXIT(
     bcmcth_trunk_vp_grp_in_use(unit,
                                trunk_vp_id,
                                gtype,
                                &in_use,
                                &comp_id));

    /* Check and confirm TRUNK_VP_ID exists prior to delete operation. */
    if (!in_use) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d not found.\n"), trunk_vp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        if (comp_id == BCMMGMT_TRUNK_COMP_ID) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "TRUNK_VP_ID=%d in use.\n"), trunk_vp_id));
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Group ID = %d is used by another component %d"),
                             trunk_vp_id, comp_id));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Delete TRUNK_VP Group from hardware tables. */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_vp_grp_delete(unit, lt_entry));

    /* Decrement Member table entries reference count. */
    if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(unit,
        group, gtype))) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                 BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id),
                 SHR_ITBM_INVALID,
                 BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id)));
     }

    /* Decrement Group ID reference count. */
    if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(BCMCTH_TRUNK_VP_TBL_PTR(
            unit, group, gtype))) {
        SHR_IF_ERR_EXIT
            (shr_itbm_list_block_free
                (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                 1,
                 SHR_ITBM_INVALID,
                 trunk_vp_id));
        /* Set in use flag in the bookkepping for group attributes. */
        BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id) = 0;
    }
    /* Clear/initialize TRUNK_VP Group information in Per-Group Info Array. */
    BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id) =
       BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id) = BCMLTD_SID_INVALID;
    if (!BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_LB_MODE_REGULAR;
    } else {
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_WEIGHTED_SIZE_256;
    }
    BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
        (unit, group, gtype);
    BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) =
        BCMCTH_TRUNK_VP_INVALID;

    exit:
        SHR_FUNC_EXIT();
}


