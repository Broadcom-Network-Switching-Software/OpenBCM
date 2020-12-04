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

#define MEMBER0_MAX_FIELD 8
static const uint32_t fbmp_mem0_id[MEMBER0_MAX_FIELD] = {
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD0,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD1,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD2,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD3,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_RH_FIELD0,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_RH_FIELD1,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_RH_FIELD2,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_RH_FIELD3,
};

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
bcmcth_trunk_vp_find_group(int unit,
                           bcmcth_trunk_vp_grp_type_t gtype,
                           uint32_t mstart_idx)
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

        if (BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) ==
           (int)mstart_idx) {
            return trunk_vp_id;
        }
    }

    return SHR_E_NOT_FOUND;
}

/*!
 * \brief Defragment TRUNK_VP group member table entries.
 *
 * This function performs defragmentation of TRUNK_VP
 * member table entries used by
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
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
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


    sal_memset(BCMCTH_TRUNK_VP_WALI_PTR(unit),
             0, sizeof(*BCMCTH_TRUNK_VP_WALI_PTR(unit)));
    mpb = shr_pb_create();
    gpb = shr_pb_create();
    shr_pb_printf(mpb,
                  "\n\tTRUNK_VP_ID\tmstart_idx(s/d)\t "
                  "SG.sz ==> DG.sz(Gap)\tgtype"
                  "\tSID\n");
    shr_pb_printf(mpb,
                  "\n\t=======\t===============\t=====================\t====="
                  "\t===\n");
    /*
     * Initialize group member start index to the
     * table Min index value as we have
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
        m_src.trunk_vp_id = bcmcth_trunk_vp_find_group(unit,
                              gtype, itbm_blk[i].first_elem);
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
                      BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                                    BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                         member, gtype)), false));
                return rv;
            }

            m_src.mstart_idx = m_dest.mstart_idx;
            m_src.gsize = m_dest.gsize;

        }
        BCMCTH_TRUNK_VP_TBL_ENT_DEFRAG_STAGED_SET(
                BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY), m_src.trunk_vp_id);
        BCMCTH_TRUNK_VP_GRP_TRANS_ID(unit, m_src.trunk_vp_id) =
                      entry->op_arg->trans_id;

        if (atomic_enable) {
            BCMCTH_TRUNK_VP_TBL_BK_ENT_DEFRAG_STAGED_SET(
                BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                   BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY), m_src.trunk_vp_id);
            BCMCTH_TRUNK_VP_GRP_TRANS_ID_BK(unit, m_src.trunk_vp_id) =
                      entry->op_arg->trans_id;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT(shr_itbm_list_defrag_end(
                      BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
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
        if (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2,
                          BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2,
                          BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) != INVALIDm)) {
            max_idx += BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member2,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) + 1;
        }
    } else if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) {
        max_idx = BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) + 1;
        if (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2,
                          BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) &&
            (BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member2,
                          BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) != INVALIDm)) {
            max_idx += BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member2,
                                      BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) + 1;
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
bcmcth_trunk_vp_group_rh_realloc(
            int unit,
            bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    int i;

    SHR_FUNC_ENTER(unit);
    /* Initialize group TRUNK_VP_ID and group type local variables. */
    trunk_vp_id = lt_entry->trunk_vp_id;
    if (BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)) {
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i)) {
                BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, \
                             trunk_vp_id, i));
            }
        }
        if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id)) {
            BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                              unit, trunk_vp_id));
        }
        if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id)) {
            BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                             trunk_vp_id));
        }
    }

    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)
        && BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)) {
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(unit, trunk_vp_id, i)) {
                BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                             unit, trunk_vp_id, i));
            }
        }
        if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id)) {
            BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                             unit, trunk_vp_id));
        }
        if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, trunk_vp_id)) {
            BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit,
                                                                trunk_vp_id));
        }
    }
    /* Allocate memory to store RH-TRUNK_VP group entries. */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        BCMCTH_TRUNK_VP_ALLOC
            (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
             lt_entry->max_members *
             sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
             unit, trunk_vp_id, i))),
             "bcmcthTrunkVpRhGrpAddGrpRhEntArr");
    }
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            BCMCTH_TRUNK_VP_ALLOC
                (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(unit, trunk_vp_id, i),
                 lt_entry->max_members *
                 sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(unit,
                          trunk_vp_id, i))),
                 "bcmcthTrunkVpRhGrpAddGrpRhEntArrBk");
        }
    }


    /*
     * Allocate memory to store group RH-TRUNK_VP entries mapped member
     * index information.
     */
    BCMCTH_TRUNK_VP_ALLOC
        (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
         lt_entry->max_members *
         sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id))),
         "bcmcthTrunkVpRhGrpAddGrpRhMindexArr");
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_ALLOC
            (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id),
             lt_entry->max_members *
             sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id))),
             "bcmcthTrunkVpRhGrpAddGrpRhMindexArrBk");
    }

    /*
     * Allocate memory to store RH-TRUNK_VP members entries usage count
     * information.
     */
    BCMCTH_TRUNK_VP_ALLOC
        (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
         lt_entry->max_members *
         sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id))),
         "bcmcthTrunkVpRhGrpAddGrpRhMembEntCntArr");
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_ALLOC
            (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, trunk_vp_id),
             lt_entry->max_members *
             sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR
                            (unit, trunk_vp_id))),
             "bcmcthTrunkVpRhGrpAddGrpRhMembEntCntArrBk");
    }
    exit:
        SHR_FUNC_EXIT();
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
        if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
            SHR_IF_ERR_EXIT
                    (bcmcth_trunk_vp_group_rh_realloc(unit, lt_entry));
        }
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
                if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
                    (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
                    SHR_IF_ERR_EXIT
                    (bcmcth_trunk_vp_group_rh_realloc(unit, lt_entry));
                }
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
                /*
                 * Defragment the member table
                 * and retry for index allocation.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_vp_itbm_groups_defragment(unit, lt_entry));
                old_mstart_idx = BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(
                     unit, trunk_vp_id);
                /* Check if it is possible to icrease max grp size in place. */
                if (old_mstart_idx + new_max_members - 1 <=
                    bcmcth_trunk_vp_mbr_tbl_max(unit, gtype)) {
                    rv = shr_itbm_list_block_resize(
                        BCMCTH_TRUNK_VP_TBL_ITBM_LIST
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

        if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
            SHR_IF_ERR_EXIT
                    (bcmcth_trunk_vp_group_rh_realloc(unit, lt_entry));
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
 * \brief Fill TRUNK_VP member fields into lt_mfield array from cur entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_mfield Array of lt entry member fields.
 * \param [in] cur_mfield Array of current entry member fields.
 * \param [in] lt_entry pointer to lt entry.
 * \param [in] member_idx field number in the lt entry.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL failure.
 */
static int
bcmcth_trunk_vp_member_fields_fill(int unit,
                                bcmcth_trunk_vp_lt_entry_t *cur_lt_entry,
                                bcmcth_trunk_vp_lt_entry_t *lt_entry,
                                bool replaceInvalidWithZeroes)
{
    uint32_t idx;
    bcmcth_trunk_drv_var_t *drv_var= NULL;
    const bcmcth_trunk_vp_drv_var_t *vp_drv_var= NULL;
    const bcmcth_trunk_drv_var_ids_t *ids = NULL;
    const bcmcth_trunk_vp_member_info_t *member0_info = NULL;
    const bcmcth_trunk_vp_member_info_t *rmember0_info = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    drv_var = bcmcth_trunk_drv_var_get(unit);
    /* Verify for null pointer input params and return error. */
    SHR_NULL_CHECK(drv_var, SHR_E_INTERNAL);

    vp_drv_var = drv_var->trunk_vp_var;
    SHR_NULL_CHECK(vp_drv_var, SHR_E_INTERNAL);

    ids = vp_drv_var->ids;
    SHR_NULL_CHECK(ids, SHR_E_INTERNAL);
    member0_info = vp_drv_var->member0_info;
    rmember0_info = vp_drv_var->rmember0_info;

    if (lt_entry == NULL) {
       return SHR_E_INTERNAL;
    }
    if (!BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode)) {
        if (member0_info) {
            for (i = 0; i < member0_info->flds_count; i++) {
                if (!(BCMCTH_TRUNK_VP_LT_FIELD_GET(
                      lt_entry->fbmp, fbmp_mem0_id[i]))
                      || (lt_entry->member_count[i] < lt_entry->max_members)) {
                    for (idx = 0; idx < lt_entry->max_members; idx++) {
                        if ((int)(lt_entry->member0_field[i][idx]) ==
                                                     BCMCTH_TRUNK_VP_INVALID) {
                            if (replaceInvalidWithZeroes &&
                                ((int)(cur_lt_entry->member0_field[i][idx]) ==
                                 BCMCTH_TRUNK_VP_INVALID)) {
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
        }
    } else {
        if (rmember0_info) {
            for (i = 0; i < rmember0_info->flds_count; i++) {
                if (!(BCMCTH_TRUNK_VP_LT_FIELD_GET(
                      lt_entry->fbmp, fbmp_mem0_id[i+4]))
                      || (lt_entry->member_count[i] < lt_entry->max_members)) {
                    for (idx = 0; idx < lt_entry->max_members; idx++) {
                        if ((int)(lt_entry->member0_field[i][idx]) ==
                                                     BCMCTH_TRUNK_VP_INVALID) {
                            if (replaceInvalidWithZeroes &&
                                ((int)(cur_lt_entry->member0_field[i][idx]) ==
                                 BCMCTH_TRUNK_VP_INVALID)) {
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
        }
    }
    exit:
        SHR_FUNC_EXIT();
}

static inline int
bcmcth_trunk_vp_compare_idx(int unit,
                            bcmcth_trunk_vp_lt_entry_t *entry1,
                            int idx1,
                            int idx2)
{
    int i;
    bcmcth_trunk_vp_lt_entry_t *entry2 = BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(
                                                 unit);
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        if (entry1->member0_field[i][idx1] !=
            entry2->member0_field[i][idx2]) {
            return 0;
        }
    }
    return 1;
}

/*!
 * \brief Returns a random member index value within an index range.
 *
 * Generates and returns a random member index value given an index range.
 *
 * \param [in] unit Unit number.
 * \param [in] trunk_vp_id TRUNK_VP group ID.
 * \param [in] rand_max Max index value limit for random index generation.
 * \param [out] rand_index Pointer to generated randon index value.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_rand_memb_index_get(int unit,
                               uint32_t trunk_vp_id,
                               uint32_t rand_max,
                               uint32_t *rand_index)
{
    uint32_t modulus;
    uint32_t rand_seed_shift = 16;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(rand_index, SHR_E_PARAM);

    modulus = rand_max + 1;
    /* Modulus should not be greater than 16 bits. */
    if (modulus > (uint32_t)(1 << (32 - rand_seed_shift))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id) =
                   BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id) *
                   1103515245 + 12345;

    *rand_index = (BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id) >>
                   rand_seed_shift)
                                                                  % modulus;

    exit:
        SHR_FUNC_EXIT();
}


/*!
 * \brief RH TRUNK_VP groups pre-config function for warm start sequence.
 *
 * For all RH TRUNK_VP groups that have non-zero
 * MEMBER_CNT value set during insert
 * operation in cold boot mode, their load balanced members state has to be
 * reconstructed for use during update operation post warm boot. This function
 * identifies such RH TRUNK_VP groups during warm start pre_config sequence.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE No errors.
 */
int
bcmcth_trunk_vp_rh_groups_preconfig(int unit)
{
    bcmcth_trunk_vp_id_t trunk_vp_id;     /* Group identifier. */
    bcmcth_trunk_vp_grp_type_t gtype; /* Group type. */

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "%s.\n"), __func__));

     gtype = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;

    /*
     * Check and perform groups HA init, only when TRUNK_VP feature has been
     * initialized successfuly for this BCM unit.
     */
    if (!(BCMCTH_TRUNK_VP_CTRL(unit)->init)) {
        SHR_EXIT();
    }

    /*
     * Check and skip invalid TRUNK_VP group types as no groups can exist at
     * these uninitialized TRUNK_VP group levels.
     */
    if (BCMCTH_TRUNK_VP_TBL_LTD_SID(unit, group, gtype)
        == BCMLTD_SID_INVALID) {
        SHR_EXIT();
    }

    /*
     * For RH TRUNK_VP groups, load balanced members state must be recovered
     * prior to members update operation. Check and set the pre_config
     * flag for this type of group.
     */
    for (trunk_vp_id =
           BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID(unit, group, gtype);
         trunk_vp_id <=
           BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID(unit, group, gtype);
         trunk_vp_id++) {

         if ((BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) != 0) &&
             (BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id) ==
               BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
              (BCMCTH_TRUNK_VP_GRP_IS_RH
                 (BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id)))) {

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "TRUNK_VP_ID[MEMBER_CNT=%u]=%u:"
                                    " in-use.\n"),
                         BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id),
                         trunk_vp_id));
            BCMCTH_TRUNK_VP_ENT_PRECONFIG_SET
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype), trunk_vp_id);
        }
        continue;
    }


    exit:
        SHR_FUNC_EXIT();
}

static int
bcmcth_trunk_vp_compare_rh_ent_idx(
                         int unit,
                         bcmcth_trunk_vp_lt_entry_t *entry1,
                         int idx1,
                         bcmcth_trunk_vp_lt_entry_t *entry2,
                         int idx2)
{
    volatile int i;
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        if (((int)(entry1->member0_field[i][idx1])
                     != (int)(BCMCTH_TRUNK_VP_INVALID)) &&
            (((entry1->member0_field[i][idx1])) !=
               entry2->rh_entries_arr[i][idx2])) {
            return 0;
        }
    }
    return 1;
}

/*!
 * \brief Add new member to an existing RH TRUNK_VP group.
 *
 * This function adds a new member to an existing
 * RH TRUNK_VP group and re-balances
 * the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] new_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_memb_add(int unit,
                        uint32_t new_mindex,
                        bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t ubound = 0, lbound = 0; /* Upper bound and lower bound. */
    uint32_t threshold = 0;          /* Upper threshold value. */
    uint32_t eindex = 0;      /* Selected entry index. */
    uint32_t next_eindex = 0; /* Next entry index. */
    uint32_t *ecount = NULL;  /* New member entries usage count. */
    uint32_t mindex;          /* member index. */
    bool new_memb_sel = FALSE; /* New member selected. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s - n_mindex=%u n_nhop=%d.\n"),
             __func__,
             new_mindex,
             (lt_entry->member0_field[0][new_mindex])));

    lbound = lt_entry->max_members / lt_entry->rh_member_cnt;
    ubound = (lt_entry->max_members % lt_entry->rh_member_cnt)
                    ? (lbound + 1) : lbound;
    threshold = ubound;

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
            (bcmcth_trunk_vp_rh_rand_memb_index_get
                (unit, lt_entry->trunk_vp_id, (lt_entry->max_members - 1),
                 &eindex));
        if (lt_entry->rh_mindex_arr[eindex] == new_mindex) {
            new_memb_sel = TRUE;
        } else {
            new_memb_sel = FALSE;
            mindex = lt_entry->rh_mindex_arr[eindex];
        }

        if (!new_memb_sel && (lt_entry->rh_memb_ecnt_arr[mindex] > threshold)) {

            /*lt_entry->rh_entries_arr[eindex] = new_nhop_id;*/

            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
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
            next_eindex = (eindex + 1) % lt_entry->max_members;
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
                    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                        lt_entry->rh_entries_arr[i][next_eindex] =
                                    lt_entry->member0_field[i][new_mindex];
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
                    next_eindex = (next_eindex + 1) % lt_entry->max_members;
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
 * \brief Returnes RH member array index where the next-hop value must be set.
 *
 * Calculates and returns the RH member array index where the next-hop member
 * value must be set. Maximum entries and additional overflow entries that a RH
 * group's member can occupy in TRUNK_VP member table
 * are used in this calculation
 * for RH member array index selection.
 *
 * \param [in] unit Unit number.
 * \param [in] max_ent_cnt Maximum entries a RH group member can occupy.
 * \param [in] overflow_cnt Surplus entries that RH group members can occupy.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 * \param [out] selected_index Pointer to selected to next-hop member index.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_INTERNAL Internal error.
 */
static int
bcmcth_trunk_vp_group_rh_memb_select(
                                  int unit,
                                  uint32_t max_ent_cnt,
                                  uint32_t *overflow_cnt,
                                  bcmcth_trunk_vp_lt_entry_t *lt_entry,
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
                          "bcmcth_trunk_vp_group_rh_memb_select.\n")));
    /*
     * Member array index is zero based, hence mindex must be less than
     * "member_cnt - 1".
     */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_vp_rh_rand_memb_index_get
            (unit, lt_entry->trunk_vp_id, (lt_entry->rh_member_cnt - 1),
             &mindex));

    if (lt_entry->rh_memb_ecnt_arr[mindex] < max_ent_cnt) {
        lt_entry->rh_memb_ecnt_arr[mindex] += 1;
        *selected_index = mindex;
    } else {
        /* Overflow entries. */
        nindex = (mindex + 1) % lt_entry->rh_member_cnt;
        while (nindex != mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex] < max_ent_cnt) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                break;
            } else {
                nindex = (nindex + 1) % lt_entry->rh_member_cnt;
            }
        }
        if (nindex == mindex) {
            if (lt_entry->rh_memb_ecnt_arr[nindex]< (max_ent_cnt + 1)
                && (0 != *overflow_cnt)) {
                lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                *selected_index = nindex;
                *overflow_cnt -= 1;
            } else {
                nindex = (mindex + 1) % lt_entry->rh_member_cnt;
                while (nindex != mindex) {
                    if (lt_entry->rh_memb_ecnt_arr[nindex] < (max_ent_cnt + 1)
                        && (0 != *overflow_cnt)) {
                        lt_entry->rh_memb_ecnt_arr[nindex] += 1;
                        *selected_index = nindex;
                        *overflow_cnt -= 1;
                        break;
                    } else {
                        nindex = (nindex + 1) % lt_entry->rh_member_cnt;
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
 * \brief Add member/s to an existing RH TRUNK_VP group.
 *
 * This function adds member or members to an existing RH TRUNK_VP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_members_add(
                           int unit,
                           bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID; /* Group TRUNK_VP_ID. */
    uint32_t grp_member_cnt = 0;  /* Current member_cnt value of the group. */
    uint32_t new_mindex; /* New member array index. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmcth_trunk_vp_rh_grp_members_add.\n")));

    /* Initialize the TRUNK_VP group identifier value. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    /* Get group's current member_cnt value. */
    grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                   (sizeof(*(
                   BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i)))
                   * lt_entry->max_members));
    }
    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->max_members));
    /*
     * Copy RH group's existing entries usage count array into LT entry's RH
     * entries usage count array for update operation.
     */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
               BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
               (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * grp_member_cnt));

    /*
     * Call RH member add function for every new member in the
     * next-hop array. New members must be set in the array after the
     * current next-hop member elements.
     */
    for (new_mindex = grp_member_cnt; new_mindex < lt_entry->rh_member_cnt;
         new_mindex++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_vp_rh_grp_memb_add(unit, new_mindex, lt_entry));
    }

    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an existing RH TRUNK_VP group.
 *
 * This function deletes a member from an existing RH TRUNK_VP group and
 * re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] del_mindex New RH group member array index.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_memb_delete(int unit,
                           uint32_t del_mindex,
                           bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t idx; /* Index iterator variable. */
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t selected_index = (uint32_t)BCMCTH_TRUNK_VP_INVALID;
    uint32_t trunk_vp_id; /* TRUNK_VP group identifier. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(
                  unit,
                  "bcmcth_trunk_vp_rh_grp_memb_delete: mindex=%u.\n"),
                  del_mindex));

    max_ent_cnt = (lt_entry->max_members / lt_entry->rh_member_cnt);
    overflow_cnt = (lt_entry->max_members % lt_entry->rh_member_cnt);
    trunk_vp_id = lt_entry->trunk_vp_id;


    for (idx = 0; idx < lt_entry->max_members; idx++) {
        /*
         * If current member is not the member to be deleted, continue to the
         * next member.
         */
        if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
            unit, trunk_vp_id)[idx] != del_mindex) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_group_rh_memb_select(unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));

        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            lt_entry->rh_entries_arr[i][idx] =
                  lt_entry->member0_field[i][selected_index];
        }

        /*
         * Delay this update until all members are deleted so that mindex array
         * has old mindex member value for multi member delete operation.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;

        /* Decrement deleted members entry usage count value.  */
        BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)[del_mindex] -= 1;

    }
    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete member/s from an existing RH TRUNK_VP group.
 *
 * This function deletes member or members from an existing RH TRUNK_VP group
 * and re-balances the group with no impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_members_delete(int unit,
                              bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t deleted_cnt = 0; /* Number of members deleted from the group. */
    uint32_t grp_member_cnt = 0;  /* Current member_cnt value of the group. */
    uint32_t *deleted_mindex = NULL; /* Array of deleted members indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t shrd_new_mindex_cnt = 0; /* Count of shared members with new
                                         mindex. */
    uint32_t shrd_cnt = 0; /* Number of members replaced in the group. */
    uint32_t del_idx = 0; /* Deleted members array index iterator. */
    uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    bcmcth_trunk_vp_id_t trunk_vp_id; /* TRUNK_VP group identifier. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp__rh_grp_members_delete.\n")));

    trunk_vp_id = lt_entry->trunk_vp_id;

    /*
     * Check if member_cnt is non-zero in this delete (update) operation. If
     * there are valid members after delete, the remaining members must be
     * re-balanced.
     *
     * If there no members left in the group after this delete operation,
     * re-balance operation is not required. Clear the entries in the member
     * table, clear the mindex value for all entries in this group, reset
     * entries usage count and return.
     */
    if (lt_entry->rh_member_cnt) {
        grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

        /* Determine how many members have been deleted. */
        deleted_cnt = (grp_member_cnt - lt_entry->rh_member_cnt);

        /*
         * For update operation, copy existing group RH entries array
         * into LT entry's RH entries array for updation.
         */
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            sal_memcpy(lt_entry->rh_entries_arr[i],
                       BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                       (sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                        unit, trunk_vp_id, i))) *
                            lt_entry->max_members));
        }
        /*
         * Copy RH group's existing entries mapped member index array into LT
         * entry's RH entries mapped member index array for update operation.
         */
        sal_memcpy(lt_entry->rh_mindex_arr,
                   BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                   (sizeof(*(lt_entry->rh_mindex_arr)) *
                    lt_entry->max_members));

        BCMCTH_TRUNK_VP_ALLOC(deleted_mindex,
                      deleted_cnt * sizeof(*deleted_mindex),
                      "bcmcthTrunkVpRhGrpMembsDelMindexArr");

        BCMCTH_TRUNK_VP_ALLOC(shrd_mindex,
                      (grp_member_cnt * sizeof(*shrd_mindex)),
                      "bcmcthTrunkVpRhGrpMembsDelShrdMindexArr");
        sal_memset(shrd_mindex,
                   BCMCTH_TRUNK_VP_INVALID,
                   (grp_member_cnt * sizeof(*shrd_mindex)));

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
        for (idx = 0; idx < grp_member_cnt; idx++) {
            /*
             * member_cnt value must be less
             * than grp_member_cnt as it's members
             * delete operation and number of elements in umemb_arr is equal to
             * member_cnt.
             */
            for (uidx = 0; uidx < lt_entry->rh_member_cnt; uidx++) {
                /*
                 * Compare existing members with members in updated
                 * array, confirm rh_memb_ecnt_arr value is equal to zero for
                 * the updated nhop member to skip duplicate members.
                 */
                if (bcmcth_trunk_vp_compare_idx(unit, lt_entry, idx, uidx)
                    && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                    lt_entry->rh_memb_ecnt_arr[uidx] =
                            BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                            unit, trunk_vp_id)[idx];
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
                        BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                        unit, trunk_vp_id)[idx] = 0;
                    }
                    break;
                }
            }

            /*
             * If the member is present in the updated nhop members array, move
             * to the next member in existing array.
             */
            if (uidx != lt_entry->rh_member_cnt) {
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
                (bcmcth_trunk_vp_rh_grp_memb_delete(unit,
                                            deleted_mindex[idx],
                                            lt_entry));
        }
        /*
         * For the shared members, check and update the member index value in
         * the LT entry mindex array if it has changed.
         */
        if (shrd_new_mindex_cnt) {
            for (idx = 0; idx < grp_member_cnt; idx++) {
                new_idx = shrd_mindex[idx];
                if (new_idx == (uint32_t)BCMCTH_TRUNK_VP_INVALID) {
                    continue;
                }
                for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                    if (bcmcth_trunk_vp_compare_idx(unit, lt_entry, idx, uidx)
                        && idx == lt_entry->rh_mindex_arr[uidx]) {
                        /* Update mindex with new mindex value. */
                        lt_entry->rh_mindex_arr[uidx] = new_idx;
                    }
                }
                shrd_new_mindex_cnt--;
            }
        }
    } else {
        /* Clear RH TRUNK_VP group member table entries. */
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            sal_memset(lt_entry->rh_entries_arr[i],
                       0,
                       (sizeof(*(
                        BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                        unit, trunk_vp_id, i))) *
                        lt_entry->max_members));
        }

        /* Clear stored RH TRUNK_VP group member indices data. */
        sal_memset(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                   0,
                   (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                    unit, trunk_vp_id)) *
                    lt_entry->max_members));

        /* Clear stored RH TRUNK_VP group members entry usage count data. */
        sal_memset(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
                   0,
                   (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)) *
                    lt_entry->rh_member_cnt));
    }


    exit:
        if (lt_entry->rh_member_cnt) {
            /* Free memory allocated to store mindex array. */
            BCMCTH_TRUNK_VP_FREE(deleted_mindex);
            BCMCTH_TRUNK_VP_FREE(shrd_mindex);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Replace member/s from an existing RH TRUNK_VP group.
 *
 * This function replaces member or members from an existing RH TRUNK_VP group
 * and re-balances the group with limited impact to existing flows.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_members_replace(int unit,
                                    bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t grp_member_cnt = 0;  /* Current member_cnt value of the group. */
    uint32_t idx, uidx;       /* Array index iterator variables. */
    bcmcth_trunk_vp_id_t trunk_vp_id;     /* TRUNK_VP group identifier. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_rh_grp_members_replace.\n")));

    trunk_vp_id = lt_entry->trunk_vp_id;

    grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

    /*
     * For replace operation, update member_cnt must be equal to group's current
     * member_cnt value.
     */
    if (grp_member_cnt - lt_entry->rh_member_cnt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Initialize member array pointers based on the group's next-hop
     * type.
     */
    SHR_NULL_CHECK(
         BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id), SHR_E_INTERNAL);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
           BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
           (sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
           unit, trunk_vp_id, i))) *
           lt_entry->max_members));
    }

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->max_members));

    /* Copy members entry usage count information. */
    sal_memcpy(lt_entry->rh_memb_ecnt_arr,
          BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
          (sizeof(*(lt_entry->rh_memb_ecnt_arr)) * lt_entry->rh_member_cnt));

    /*
     * Find and replace the existing nhop member with the new nhop member
     * ID value.
     */
    for (idx = 0; idx < lt_entry->rh_member_cnt; idx++) {
        if (bcmcth_trunk_vp_compare_idx(unit, lt_entry, idx, idx) == 0) {
            for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                if (bcmcth_trunk_vp_compare_rh_ent_idx(
                    unit, BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                    idx, lt_entry, uidx)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                        lt_entry->rh_entries_arr[i][uidx] =
                           lt_entry->member0_field[i][idx];
                    }
                }
            }
        }
    }

    exit:
        SHR_FUNC_EXIT();
}


/*!
 * \brief Determine the type of operation being
 * performed on the RH TRUNK_VP group.
 *
 * This function determines the type of operation being performed on the
 * member/s of a RH TRUNK_VP group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 * \param [out] rh_oper Resilient hashing TRUNK_VP group members operation type.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 */
static int
bcmcth_trunk_vp_group_rh_oper_get(int unit,
                          bcmcth_trunk_vp_lt_entry_t *lt_entry,
                          bcmcth_trunk_vp_rh_oper_t *rh_oper)
{
    uint32_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID; /* Group TRUNK_VP_ID. */
    uint32_t grp_member_cnt; /* Group current MEMBER_CNT value. */
    uint32_t grp_rh_entries_cnt; /* Group current RH entries count value. */
    volatile uint32_t idx = 0, uidx = 0; /* Members iterator variables. */
    uint32_t shared_member_cnt = 0; /* Shared NHOPs count. */
    uint32_t new_member_cnt = 0; /* New NHOPs count. */
    uint32_t alloc_sz = 0; /* Alloc size current and new. */
    int *member_field_arr = NULL; /* NHOP members array. */
    bool mindex_changed = FALSE; /* Member index has changed in NHOP array. */
    const char * const rh_op_str[BCMCTH_TRUNK_VP_RH_OPER_COUNT] = \
        BCMCTH_TRUNK_VP_RH_OPER_NAME;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(rh_oper, SHR_E_PARAM);

    COMPILER_REFERENCE(trunk_vp_id);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_rh_oper_get.\n")));
    trunk_vp_id = lt_entry->trunk_vp_id;

    /*
     * If there are no members in the
     * RH TRUNK_VP group, then there is no SW load
     * balancing operation to performed for this group in the UPDATE operation.
     */
    if (BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) == 0 &&
        lt_entry->rh_member_cnt == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(
                        unit,
                        "(RH_MEMBER_CNT = %u): BCMCTH_TRUNK_VP_RH_OPER_NONE."
                        "\n"),
                    lt_entry->rh_member_cnt));

        *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_NONE;
        SHR_EXIT();
    }

    /*
     * This UPDATE operation is equivalent to members add via an INSERT
     * operation as all the members are newly added to this group. These new
     * members must be SW load balanced by using
     * bcmcth_trunk_vp_group_rh_entries_set()
     * function and not expected to be handled in this function, return
     * SHR_E_INTERNAL error.
     */
    if (BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) == 0
        && lt_entry->rh_member_cnt > 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(
                       unit,
                       "No members to members (RH_MEMBER_CNT = %u) rv=%s"
                       ".\n"),
                     lt_entry->rh_member_cnt,
                     shr_errmsg(SHR_E_INTERNAL)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     * If the RH TRUNK_VP group previously had members and there are no members
     * in the group as part of this UPDATE operation, then set 'rh_oper' to
     * BCMCTH_TRUNK_VP_RH_OPER_DELETE and return
     * from this function. There are no
     * shared members between the old and new members for this group.
     */
    if (BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) > 0
        && lt_entry->rh_member_cnt == 0) {

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "(RH_MEMBER_CNT = %u): BCMCTH_TRUNK_VP_RH_OPER_DELETE.\n"),
                    lt_entry->rh_member_cnt));

        *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_DELETE;
        SHR_EXIT();
    }

    /* Validate MEMBER_CNT value before proceeding further in this function. */
    if (BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) == 0
        || lt_entry->rh_member_cnt == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Unexpected RH_MEMBER_CNT value: GMEMBER_CNT=%u"
                                " RH_MEMBER_CNT=%u.\n"),
                     BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id),
                     lt_entry->rh_member_cnt));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get group's current member_cnt value. */
    grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);
    grp_rh_entries_cnt = BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id);
    alloc_sz = sizeof(*member_field_arr) * grp_member_cnt;

    if (alloc_sz) {
        BCMCTH_TRUNK_VP_ALLOC(member_field_arr,
                      alloc_sz,
                      "bcmcthTrunkVpRhOperGetNhopArr");
    }

    /*
     * Identify the shared members and the new members in the updated
     * next-hop members array.
     */
    if (member_field_arr) {
        int start_idx = 0;
        sal_memset(member_field_arr, 0, alloc_sz);

        for (uidx = 0; uidx < lt_entry->rh_member_cnt; uidx++) {
            for (idx = start_idx; idx < grp_member_cnt; idx++) {
                if ((member_field_arr[idx] != -1)
                     && bcmcth_trunk_vp_compare_idx(
                        unit, lt_entry, idx, uidx)) {
                    /* Old memebr also member of the updated member array. */
                    shared_member_cnt++;
                    member_field_arr[idx] = -1;
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
            if (idx == grp_member_cnt) {
                new_member_cnt++;
            }
            while (member_field_arr[start_idx] == -1) {
                start_idx++;
            }
        }
    }

    /* Determine the type of operation to be performed on RH group members. */
    if (lt_entry->rh_member_cnt > grp_member_cnt) {
        if ((shared_member_cnt == grp_member_cnt) && !mindex_changed) {
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_ADD;
        } else {
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_ADD_REPLACE;
        }
    } else if (lt_entry->rh_member_cnt < grp_member_cnt) {
        if (shared_member_cnt == lt_entry->rh_member_cnt) {
            /*
             * This condition is TRUE when all the members passed in the
             * member/s delete-update operation are already members of this
             * group (Their mindex could have changed but they are still common
             * members between the NHOP members list).
             * This condition is also TRUE when all the members have been
             * deleted in the update operation i.e. 'lt_entry->member_cnt == 0',
             * means there are no shared members in the group in this delete
             * operation. As shared_member_cnt
             * default value is initialized to '0'
             * at the start of this function, This condition will be TRUE.
             */
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_DELETE;
        } else {
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_DELETE_REPLACE;
        }
    } else if (lt_entry->rh_member_cnt == grp_member_cnt) {
        if (lt_entry->max_members != grp_rh_entries_cnt) {
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_ADD_REPLACE;
        } else {
            *rh_oper = BCMCTH_TRUNK_VP_RH_OPER_REPLACE;
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[grp_member_cnt=%-4u rh_member_cnt=%-4u"
                            "shared_member_cnt=%-4u\n"
                            "\t new_member_cnt=%-4u mindex_changed=%s "
                            "rh_oper=(%s)]\n"),
                 grp_member_cnt,
                 lt_entry->rh_member_cnt,
                 shared_member_cnt,
                 new_member_cnt,
                 mindex_changed ? "TRUE" : "FALSE",
                 rh_op_str[*rh_oper]));

    exit:
        BCMCTH_TRUNK_VP_FREE(member_field_arr);
        SHR_FUNC_EXIT();
}

/*!
 * \brief Add, replace and re-balance member/s in an existing RH TRUNK_VP group.
 *
 * This function performs member/s add, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmcth_trunk_vp_rh_grp_members_add_replace(int unit,
                                   bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID; /* Group TRUNK_VP_ID. */
    uint32_t grp_member_cnt = 0;  /* Current member_cnt value of the group. */
    uint32_t *replace_mindex = NULL; /* Array of replaced member/s indices. */
    uint32_t *shrd_mindex = NULL; /* Array of shared member/s indices. */
    uint32_t *new_mindex = NULL; /* Array of new member/s indices. */
    uint32_t added_cnt = 0; /* Number of members added to the group. */
    uint32_t replace_cnt = 0; /* Number of members replaced in the group. */
    volatile uint32_t shrd_cnt = 0; /* Number of members replaced in a group. */
    uint32_t shrd_mindex_cnt = 0; /* Count of shared members with new mindex. */
    volatile uint32_t rep_idx = 0; /* Added member array index. */
    volatile uint32_t new_idx = 0; /* New member array index. */
    uint32_t idx, uidx; /* Array index iterator variables. */
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "bcmcth_trunk_vp_rh_grp_members_add_replace.\n")));

    /* Initialize the TRUNK_VP group identifier value. */
    trunk_vp_id = lt_entry->trunk_vp_id;
    /* Get group's current member_cnt value. */
    grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

    /*
     * For update operation, copy existing group RH entries array
     * into LT entry's RH entries array for updation.
     */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                   (sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                   unit, trunk_vp_id, i))) *
                    lt_entry->max_members));
    }

    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) * lt_entry->max_members));

    BCMCTH_TRUNK_VP_ALLOC(replace_mindex,
                  (grp_member_cnt * sizeof(*replace_mindex)),
                  "bcmcthTrunkVpRhGrpMembsAddRepReplaceMindexArr");
    BCMCTH_TRUNK_VP_ALLOC(shrd_mindex,
                  (grp_member_cnt * sizeof(*shrd_mindex)),
                  "bcmcthTrunkVpRhGrpMembsAddRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMCTH_TRUNK_VP_INVALID,
               (grp_member_cnt * sizeof(*shrd_mindex)));

    /*
     * Identify the members that are shared between the updated NHOP array and
     * the current NHOP members up to the group's current MEMBER_CNT value.
     * Members in the updated array after current MEMBER_CNT value will be
     * treated as member add operation.
     * For members up to grp_member_cnt that are not
     * shared with the updated members
     * array will be candidates for replace
     * operation.
     */
    for (idx = 0; idx < grp_member_cnt; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_member_cnt; uidx++) {
            if (bcmcth_trunk_vp_compare_idx(unit, lt_entry, uidx, idx)
                && (lt_entry->rh_memb_ecnt_arr[uidx] == 0)) {
                /* Update entry usage count. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)[idx];

                /* Increment shared entry count. */
                shrd_cnt++;

                if (idx != uidx) {
                    shrd_mindex[idx] = uidx;
                    shrd_mindex_cnt++;
                }

                /* Clear old entry usage count value. */
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                   unit, trunk_vp_id)[idx] = 0;
                break;
            }
        }

        /*
         * Existing member at current index is also part of the updated
         * next-hop members array, continue to the next member.
         */
        if (uidx != lt_entry->rh_member_cnt) {
            continue;
        }

        /* Member not shared hence add to replace member elements list. */
        replace_mindex[replace_cnt++] = idx;
    }

    if (replace_cnt) {
        BCMCTH_TRUNK_VP_ALLOC(new_mindex,
                      replace_cnt * sizeof(*new_mindex),
                      "bcmcthTrunkVpRhGrpMembsAddRepNewMindexArr");

        for (idx = 0; idx < replace_cnt; idx++) {

            /* Get the old to be replaced index value. */
            rep_idx = replace_mindex[idx];
            for (uidx = 0; uidx < grp_member_cnt; uidx++) {

                if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                    /* Already updated, move to next member. */
                    continue;
                }

                /* Update entry count value. */
                lt_entry->rh_memb_ecnt_arr[uidx] =
                    BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                       unit, trunk_vp_id)[rep_idx];

                /* Clear old entry usage count value. */
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                       unit, trunk_vp_id)[rep_idx] = 0;

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
        for (idx = 0; idx < grp_member_cnt; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMCTH_TRUNK_VP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                if (bcmcth_trunk_vp_compare_rh_ent_idx(unit,
                    BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                    idx,
                    lt_entry,
                    uidx)
                    && (idx == lt_entry->rh_mindex_arr[uidx])) {
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                }
            }
           shrd_mindex_cnt--;
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

            for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                if (bcmcth_trunk_vp_compare_rh_ent_idx(
                      unit,
                      BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                                                             rep_idx,
                                                             lt_entry,
                                                             uidx)
                    && (rep_idx == lt_entry->rh_mindex_arr[uidx])) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                        lt_entry->rh_entries_arr[i][uidx] =
                        lt_entry->member0_field[i][new_idx];
                    }
                }
            }
        }
    }


    for (new_idx = 0; new_idx < lt_entry->rh_member_cnt;
         new_idx++) {
        if (lt_entry->rh_memb_ecnt_arr[new_idx]) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_vp_rh_grp_memb_add(unit, new_idx, lt_entry));
        added_cnt++;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "added_cnt=%u n_mindex=%u\n"),
                     added_cnt, new_idx));
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMCTH_TRUNK_VP_FREE(replace_mindex);
        BCMCTH_TRUNK_VP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMCTH_TRUNK_VP_FREE(new_mindex);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Delete, replace and re-balance member/s
 *  in an existing RH TRUNK_VP group.
 *
 * This function performs member/s delete, replace and re-balance operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_MEMORY Memory allocation error.
 * \return SHR_E_INTERNAL Invalid configuration.
 */
static int
bcmcth_trunk_vp_rh_grp_members_delete_replace(int unit,
                                      bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t trunk_vp_id; /* TRUNK_VP group identifier. */
    uint32_t grp_member_cnt = 0; /* Current member_cnt value of the group. */
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

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_rh_grp_members_delete_replace.\n")));

    /*
     * MEMBER_CNT must be non-zero when this
     * function is called for member delete
     * operation, check and return if this condition is not met.
     */
    if (!lt_entry->rh_member_cnt) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the group's TRUNK_VP_ID value and current MEMBER_CNT values. */
    trunk_vp_id = lt_entry->trunk_vp_id;
    grp_member_cnt = BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

    /*
     * Determine how many members have been deleted based on the updated
     * MEMBER_CNT value.
     */
    deleted_cnt = (grp_member_cnt - lt_entry->rh_member_cnt);

    /*
     * Copy existing group RH entries array into LT entry RH entries array for
     * members delete + replace operation.
     */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        sal_memcpy(lt_entry->rh_entries_arr[i],
                   BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                   (sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                    unit, trunk_vp_id, i))) *
                    lt_entry->max_members));
    }
    /*
     * Copy RH group's existing entries mapped member index array into LT
     * entry's RH entries mapped member index array for update operation.
     */
    sal_memcpy(lt_entry->rh_mindex_arr,
               BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
               (sizeof(*(lt_entry->rh_mindex_arr)) *
                lt_entry->max_members));

    BCMCTH_TRUNK_VP_ALLOC(deleted_mindex,
                  deleted_cnt * sizeof(*deleted_mindex),
                  "bcmcthTrunkVpRhGrpMembsDelRepDelMindexArr");

    BCMCTH_TRUNK_VP_ALLOC(replace_mindex,
                  (lt_entry->rh_member_cnt) * sizeof(*replace_mindex),
                  "bcmcthTrunkVpRhGrpMembsDelRepReplaceMindexArr");

    BCMCTH_TRUNK_VP_ALLOC(shrd_mindex,
                  (grp_member_cnt * sizeof(*shrd_mindex)),
                  "bcmcthTrunkVpRhGrpMembsDelRepShrdMindexArr");
    sal_memset(shrd_mindex,
               BCMCTH_TRUNK_VP_INVALID,
               (grp_member_cnt * sizeof(*shrd_mindex)));

    /*
     * Order of NHOP members in updated NHOP array is not same as the stored
     * NHOP members array and some of the members in the updated array might be
     * shared with the stored members. Identify these shared members, deleted
     * members and members to be replaced.
     */
    for (idx = 0; idx < grp_member_cnt; idx++) {
        for (uidx = 0; uidx < lt_entry->rh_member_cnt; uidx++) {
            /*
             * Identify the common members and copy the stored member entry
             * usage count value to LT entry usage count element at the new
             * member index for delete and rebalance operation.
             */
            if (bcmcth_trunk_vp_compare_idx(unit, lt_entry, uidx, idx)
                && (0 == lt_entry->rh_memb_ecnt_arr[uidx])) {
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                        unit, trunk_vp_id)[idx];
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
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                unit, trunk_vp_id)[idx] = 0;
                break;
            }
        }

        /*
         * If the member is a shared member, then continue to the next member
         * in the list.
         */
        if (uidx != lt_entry->rh_member_cnt) {
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
        BCMCTH_TRUNK_VP_ALLOC(new_mindex,
                      (replace_cnt) * sizeof(*new_mindex),
                      "bcmcthTrunkVpRhGrpMembsNewMindexArr");
        for (uidx = 0; uidx < lt_entry->rh_member_cnt; uidx++) {
            /*
             * Shared member entry count is already found, skip to next
             * member.
             */
            if (lt_entry->rh_memb_ecnt_arr[uidx]) {
                continue;
            }
            for (idx = 0; idx < replace_cnt; idx++) {
                rep_idx = replace_mindex[idx];
                if (!BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                     unit, trunk_vp_id)[rep_idx]) {
                    /*
                     * This replacement member candidate has already been
                     * associate with a new member from the updated array, move
                     * on to the next member in the replace mindex array.
                     */
                    continue;
                }
                lt_entry->rh_memb_ecnt_arr[uidx] =
                        BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                        unit, trunk_vp_id)[rep_idx];
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                        unit, trunk_vp_id)[rep_idx] = 0;

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
            (bcmcth_trunk_vp_rh_grp_memb_delete(unit,
                                        deleted_mindex[idx],
                                        lt_entry));
    }
    /*
     * For the shared members, check and update the member index value in the
     * LT entry mindex array if it has changed.
     */
    if (shrd_new_mindex_cnt) {
        for (idx = 0; idx < grp_member_cnt; idx++) {
            new_idx = shrd_mindex[idx];
            if (new_idx == (uint32_t)BCMCTH_TRUNK_VP_INVALID) {
                continue;
            }
            for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                if (bcmcth_trunk_vp_compare_rh_ent_idx(unit,
                      BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                      idx, lt_entry, uidx)
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
            for (uidx = 0; uidx < lt_entry->max_members; uidx++) {
                if (bcmcth_trunk_vp_compare_rh_ent_idx(unit,
                    BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                    rep_idx,
                    lt_entry,
                    uidx)
                    && (rep_idx == lt_entry->rh_mindex_arr[uidx])) {

                    new_idx = new_mindex[idx];
                    /* Update mindex with new mindex value. */
                    lt_entry->rh_mindex_arr[uidx] = new_idx;
                    /* Update with new member value. */
                    for (i = 0;
                         i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS;
                         ++i) {
                        lt_entry->rh_entries_arr[i][uidx] =
                        lt_entry->member0_field[i][new_idx];
                    }
                }
            }
        }
    }

    exit:
        /* Free memory allocated to store mindex array. */
        BCMCTH_TRUNK_VP_FREE(deleted_mindex);
        BCMCTH_TRUNK_VP_FREE(replace_mindex);
        BCMCTH_TRUNK_VP_FREE(shrd_mindex);
        if (replace_cnt) {
            BCMCTH_TRUNK_VP_FREE(new_mindex);
        }
        SHR_FUNC_EXIT();
}

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes MEMBER_CNT group members among RH_SIZE member table entries for
 * installation in TRUNK_V_RESILIENTP hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_group_rh_entries_set(
                                  int unit,
                                  bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    uint32_t max_ent_cnt = 0;  /* Maxiumum entries a member can occupy. */
    uint32_t overflow_cnt = 0; /* Overflow count. */
    uint32_t idx;              /* loop iterator variable. */
    uint32_t selected_index = (uint32_t)BCMCTH_TRUNK_VP_INVALID;
    int i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /*
     * Validate MEMBER_CNT value and confirm it's non-zero to perform RH load
     * balancing. If value is zero, there are no members to perform load
     * balancing.
     */
    if (!lt_entry->rh_member_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No members RH_MEMBER_CNT=%u.\n"),
                     lt_entry->rh_member_cnt));
        lt_entry->member_cnt = 0;
        SHR_EXIT();
    }

    max_ent_cnt = (lt_entry->max_members / lt_entry->rh_member_cnt);
    overflow_cnt = (lt_entry->max_members % lt_entry->rh_member_cnt);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                     "[max_members=%-4u, "
                     "rh_member_cnt=%-4d, max_ent_cnt:%-4u, ovf_cnt=%u].\n"),
                 lt_entry->max_members,
                 lt_entry->rh_member_cnt,
                 max_ent_cnt,
                 overflow_cnt));

    for (idx = 0; idx < lt_entry->max_members; idx++) {
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_group_rh_memb_select(
                                          unit,
                                          max_ent_cnt,
                                          &overflow_cnt,
                                          lt_entry,
                                          &selected_index));

            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                lt_entry->rh_entries_arr[i][idx] =
                    lt_entry->member0_field[i][selected_index];
            }

        /*
         * Store selected array member index value for use during
         * Add/Delete/Replace operations.
         */
        lt_entry->rh_mindex_arr[idx] = selected_index;
    }


    exit:
        SHR_FUNC_EXIT();
}

/*!
 * \brief Distributes group members as per RH load balance algorithm.
 *
 * Distributes MEMBER_CNT group members among RH_SIZE member table entries for
 * installation in TRUNK_VP hardware tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_group_rh_entries_update(int unit,
                                     bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_rh_oper_t rh_oper = BCMCTH_TRUNK_VP_RH_OPER_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_rh_entries_update.\n")));

    /* Get the type of RH TRUNK_VP group operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_group_rh_oper_get(unit, lt_entry, &rh_oper));

    /* Invoke operation dependent function. */
    switch (rh_oper) {
        case BCMCTH_TRUNK_VP_RH_OPER_NONE:
            SHR_EXIT();
            break;
        case BCMCTH_TRUNK_VP_RH_OPER_ADD:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_rh_grp_members_add(unit, lt_entry));
            break;
        case BCMCTH_TRUNK_VP_RH_OPER_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_rh_grp_members_delete(unit, lt_entry));
            break;
        case BCMCTH_TRUNK_VP_RH_OPER_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_rh_grp_members_replace(unit, lt_entry));
            break;
        case BCMCTH_TRUNK_VP_RH_OPER_ADD_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_rh_grp_members_add_replace(unit, lt_entry));
            break;
        case BCMCTH_TRUNK_VP_RH_OPER_DELETE_REPLACE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_vp_rh_grp_members_delete_replace(unit, lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    exit:
        SHR_FUNC_EXIT();
}


/*!
 * \brief Reconstruct RH TRUNK_VP group mebers load balanced state.
 *
 * This function reconstructs RH TRUNK_VP group members load balanced state post
 * warm start sequence to support subsequent members update operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
static int
bcmcth_trunk_vp_rh_grp_members_recover(int unit,
                               bcmcth_trunk_vp_lt_entry_t *lt_entry)
{

    bcmcth_trunk_vp_lt_entry_t *rhg_entry = NULL;
    uint32_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID; /* Group identifier. */
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
    BCMCTH_TRUNK_VP_ALLOC(rhg_entry,
                          sizeof(*rhg_entry),
                          "bcmcthTrunkVpBcmRhgEntry");
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        BCMCTH_TRUNK_VP_ALLOC
            (rhg_entry->rh_entries_arr[i],
             lt_entry->max_members * sizeof(*(lt_entry->rh_entries_arr[i])),
             "bcmcthTrunkVpRhGrpAddLtEntRhEntArr");
    }

    /*
     * Setup input attribute values necessary for reading the RH group hardware
     * table entries.
     */
    rhg_entry->glt_sid = lt_entry->glt_sid;
    rhg_entry->op_arg = lt_entry->op_arg;
    rhg_entry->grp_type = lt_entry->grp_type;
    rhg_entry->trunk_vp_id = lt_entry->trunk_vp_id;
    rhg_entry->max_members = lt_entry->max_members;
    trunk_vp_id = rhg_entry->trunk_vp_id;

    /* Get RH TRUNK_VP Group entry from hardware tables. */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_drv_vp_grp_get(unit, rhg_entry));

    lbound = (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT
                    (unit, trunk_vp_id) /
              BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id));
    ubound = ((BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id) %
                    BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id))
                    ? (lbound + 1) : lbound);
    threshold = ubound;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "\tLBND=%u UBND=%u THRSLD=%u LB_MODE=%u "
                            "max_members=%u mstart_idx=%d\n"),
                lbound,
                ubound,
                threshold,
                rhg_entry->lb_mode,
                rhg_entry->max_members,
                rhg_entry->mstart_idx));

    /*
     * Verify retrieved total RH group member table entries count matches
     * stored/expected value before further processing for this RH group.
     */
    if (rhg_entry->max_members
        != BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate memory for RHG members structure. */
    BCMCTH_TRUNK_VP_ALLOC(BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(unit, trunk_vp_id),
                  sizeof(*BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)),
                  "bcmcthTrunkVpRhGrpMembRcvrRhgInfo");
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_ALLOC(BCMCTH_TRUNK_VP_GRP_VOID_RHG_BK_PTR(
                  unit, trunk_vp_id),
                  sizeof(*BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)),
                  "bcmcthTrunkVpRhGrpMembRcvrRhgInfoBk");
    }

    /* Allocate memory to store RHG member table entries. */
    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
        BCMCTH_TRUNK_VP_ALLOC(
              BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
              (rhg_entry->max_members *
               sizeof(*BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
               unit, trunk_vp_id, i))),
              "bcmcthTrunkVpRhGrpMembRcvrRhEntArr");
        if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
            BCMCTH_TRUNK_VP_ALLOC(
               BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(unit, trunk_vp_id, i),
               (rhg_entry->max_members *
               sizeof(*BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
               unit, trunk_vp_id, i))),
               "bcmcthTrunkVpRhGrpMembRcvrRhEntArrBk");
        }

        /* Copy load balanced RH members array. */
        sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                   rhg_entry->rh_entries_arr[i],
                   sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                   unit, trunk_vp_id, i))) *
                   rhg_entry->max_members);
        if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
            sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                   unit, trunk_vp_id, i),
                       rhg_entry->rh_entries_arr[i],
                       sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                       unit, trunk_vp_id, i))) *
                       rhg_entry->max_members);
        }
    }
    /*
     * Allocate memory to store group RH TRUNK_VP entries mapped member
     * index information.
     */
    BCMCTH_TRUNK_VP_ALLOC(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                  (rhg_entry->max_members *
                   sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                   unit, trunk_vp_id)))),
                  "bcmcthTrunkVpRhGrpMembRcvrRhMindexArr");
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_ALLOC(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                      unit, trunk_vp_id),
                      (rhg_entry->max_members *
                       sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                      unit, trunk_vp_id)))),
                      "bcmcthTrunkVpRhGrpMembRcvrRhMindexArrBk");
    }

    /*
     * Allocate memory to store RH TRUNK_VP members entries usage count
     * information.
     */
    BCMCTH_TRUNK_VP_ALLOC(
               BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
               (BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) *
               sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
               unit, trunk_vp_id)))),
               "bcmcthTrunkVpRhGrpMembRcvrRhMembEntCntArr");
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        BCMCTH_TRUNK_VP_ALLOC(
               BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, trunk_vp_id),
               (BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) *
               sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR
                       (unit, trunk_vp_id)))),
               "bcmcthTrunkVpRhGrpMembRcvrRhMembEntCntArrBk");
    }

    for (memb_idx = 0;
         memb_idx < BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->max_members; idx++) {
            if ((rhg_entry->rh_entries_arr[0][idx] !=
                 (uint32_t)(BCMCTH_TRUNK_VP_INVALID)) &&
                (bcmcth_trunk_vp_compare_rh_ent_idx(unit,
                                   BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                                   memb_idx, rhg_entry, idx))
                && BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, trunk_vp_id)[memb_idx] < lbound) {
                /* Update member index mapping */
                BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id)[idx] =
                     memb_idx;

                /* Update member table entries usage count for this member. */
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                     unit, trunk_vp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[0][idx] = BCMCTH_TRUNK_VP_INVALID;

                /* Move on to the next member as lower bound as been reached. */
                if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)[idx]
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
         memb_idx < BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);
         memb_idx++) {
        for (idx = 0; idx < rhg_entry->max_members; idx++) {
            if ((rhg_entry->rh_entries_arr[0][idx] !=
                (uint32_t)(BCMCTH_TRUNK_VP_INVALID)) &&
                (bcmcth_trunk_vp_compare_rh_ent_idx(unit,
                                BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit),
                                memb_idx, rhg_entry, idx))
                && BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR
                        (unit, trunk_vp_id)[memb_idx] < ubound) {
                /* Update member index mapping */
                BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id)[idx] =
                    memb_idx;

                /* Update member table entries usage count for this member. */
                BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)[memb_idx] += 1;

                /* Update as invalid as it's been matched. */
                rhg_entry->rh_entries_arr[0][idx] = BCMCTH_TRUNK_VP_INVALID;

                /* Move on to the next member as upper bound as been reached. */
                if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                    unit, trunk_vp_id)[idx]
                    == ubound) {
                    break;
                }
            }
        }
    }

    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        /*
         * Copy group entries mapped member index array elements into the backup
         * array.
         */
        sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id),
                   BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                   (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                   unit, trunk_vp_id)) *
                    rhg_entry->max_members));

        /*
         * Copy members entry usage count array elements into the backup
         * array.
         */
        sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, trunk_vp_id),
           BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
           (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
            unit, trunk_vp_id)) *
            BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id)));
    }

    exit:
        if (rhg_entry) {
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                BCMCTH_TRUNK_VP_FREE(rhg_entry->rh_entries_arr[i]);
            }
            BCMCTH_TRUNK_VP_FREE(rhg_entry);
        }
        if (SHR_FUNC_ERR() && trunk_vp_id !=
              (uint32_t)(BCMCTH_TRUNK_VP_INVALID)) {
            if (BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)) {
                for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                    BCMCTH_TRUNK_VP_FREE(
                      BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i));
                }
                BCMCTH_TRUNK_VP_FREE(
                   BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id));
                BCMCTH_TRUNK_VP_FREE(
                   BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id));
                BCMCTH_TRUNK_VP_FREE(
                   BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(unit, trunk_vp_id));
            }

            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)
                && BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)) {
                for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                    BCMCTH_TRUNK_VP_FREE(
                      BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                      unit, trunk_vp_id, i));
                }
                BCMCTH_TRUNK_VP_FREE(
                  BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id));
                BCMCTH_TRUNK_VP_FREE(
                  BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(unit, trunk_vp_id));
                BCMCTH_TRUNK_VP_FREE(
                  BCMCTH_TRUNK_VP_GRP_VOID_RHG_BK_PTR(unit, trunk_vp_id));
            }
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
 * \param [in] lt_entry Pointer to TRUNK_VP logical table entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 */
int
bcmcth_trunk_vp_group_update_entry_set(int unit,
                                    bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    uint32_t comp_id = BCMMGMT_MAX_COMP_ID;
    bool in_use = FALSE;
    uint32_t rh_entries_cnt = 0;
    bcmcth_trunk_vp_lt_entry_t *cur_lt_entry = NULL; /* current LT entry.*/

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_group_update_entry_set.\n")));

    /* Get group type value. */
    gtype = lt_entry->grp_type;

    /* Initialize group TRUNK_VP_ID and group type local variables. */
    trunk_vp_id = lt_entry->trunk_vp_id;

    /* Get logical table entry buffer pointer. */
    cur_lt_entry = BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit);

    /* Verify the group is in use. */
    SHR_IF_ERR_EXIT(
     bcmcth_trunk_vp_grp_in_use(unit,
                                trunk_vp_id,
                                gtype,
                                &in_use,
                                &comp_id));
    if (!in_use || (comp_id != BCMMGMT_TRUNK_COMP_ID)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d not found.\n"), trunk_vp_id));
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "TRUNK_VP_ID=%d in use.\n"), trunk_vp_id));
    /*
     * Use stored group lb_mode value for update operation if it has not been
     * supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_LB_MODE)
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
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_WEIGHTED_SIZE)
        && BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {

        /* Retrive and use group lb_mode value. */
        lt_entry->weighted_size = BCMCTH_TRUNK_VP_GRP_LB_MODE(
                                  unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "WEIGHTED_SIZE=%d.\n"),
                     lt_entry->weighted_size));
    }

    /*
     * Use stored rh_size value for update operation if it has not been
     * supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_RH_SIZE)
        && (lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)
        && BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode)) {

        /* Retrive and use group lb_mode value. */
        rh_entries_cnt = BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(
                             unit,
                             lt_entry->trunk_vp_id);
        /*
         * If the group
         * RESILIENT: recover the RH_SIZE from the rh_entries_cnt.
         * RANDOM/REG_HASH: recover the RH_SIZE from the cur_lt_entry.
         * RH_SIZE could be supplied in LT operation when group is
         * not resilinet.
         */

        if (rh_entries_cnt) {
            lt_entry->rh_size =
            bcmcth_trunk_vp_member_cnt_convert_to_rh_size(rh_entries_cnt);
            lt_entry->max_members = rh_entries_cnt;
        } else {
            lt_entry->rh_size = cur_lt_entry->rh_size;
            lt_entry->max_members =
               bcmcth_trunk_vp_rh_size_convert_to_member_cnt(
                      cur_lt_entry->rh_size);
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_SIZE=%d.\n"),
                     lt_entry->rh_size));
    }

    /*
     * Use stored group max_members value for
     * update operation if it has not been
     * supplied.
     */
    if ((!BCMCTH_TRUNK_VP_LT_FIELD_GET(
           lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS)) &&
        (!BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {

        /* Retrive and use group max_members value. */
        lt_entry->max_members =
               BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MAX_MEMBERS=%u.\n"), lt_entry->max_members));
    }

    /*
     * Use stored group member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
           lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT)) {

        /* Retrive and use group member_cnt value. */
        lt_entry->member_cnt = cur_lt_entry->member_cnt;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MEMBER_CNT=%u.\n"), lt_entry->member_cnt));
    }


    /*
     * Use stored group rh_member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
          lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_RH_RANDOM_SEED)) {

        /* Retrive and use group rh_mode value. */
        if (BCMCTH_TRUNK_VP_GRP_RH_INFO_PTR(unit)) {
            lt_entry->rh_rand_seed =
            BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id);
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_rand_seed));
    }

    /*
     * Use stored group rh_member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
              lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_RH_MEMBER_CNT)) {

        /*
         * Retrive and use group member_cnt value from previous LT operation. */
        lt_entry->rh_member_cnt = cur_lt_entry->rh_member_cnt;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "RH_MEMBER_CNT=%u.\n"), lt_entry->rh_member_cnt));
    }

    /*
     * Fill the meber fields from the cur_lt_entry if not
     * provided in current lt operations.
     */
    if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
        (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_member_fields_fill(unit, cur_lt_entry,
                                             lt_entry, FALSE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_member_fields_fill(unit, cur_lt_entry,
                                             lt_entry, TRUE));
    }
    exit:
        SHR_FUNC_EXIT();
}

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
    uint32_t free_idx = (uint32_t) BCMCTH_TRUNK_VP_INVALID;
    int i = 0;

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
                    unit,
                    trunk_vp_id,
                    gtype,
                    &in_use,
                    &comp_id));

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
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
           BCMCTH_TRUNK_VP_TBL_PTR(unit,
                                    member,
                                    gtype))) {
            rv = shr_itbm_list_block_alloc
                    (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                        (BCMCTH_TRUNK_VP_TBL_PTR(
                             unit,
                             member,
                             gtype)),
                        lt_entry->max_members,
                        &bucket,
                        &free_idx);

            if ((SHR_E_FULL == rv) || (SHR_E_RESOURCE == rv)) {
                /*
                 * Defragment the member table and
                 * retry for index allocation.
                 */
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
        /* Allocate the ITBM resource for Group ID and mark in use. */
        if (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
            BCMCTH_TRUNK_VP_TBL_PTR(
                              unit,
                              group,
                              gtype))) {
            SHR_IF_ERR_EXIT
                (shr_itbm_list_block_alloc_id
                     (BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                     (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                     1,
                     bucket,
                     trunk_vp_id));
            BCMCTH_TRUNK_VP_GRP_ITBM_USE(unit, trunk_vp_id) = 1;
        }

        /*
         * These are required for abort to call free
         * with the right parameters.
         */
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) =
              lt_entry->max_members;
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) = free_idx;
        if (SHR_E_NONE != rv) {
            SHR_ERR_EXIT(rv);
        }

        if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
            /*
             * Check if group's LB_MODE is RESILIENT and call the corresponding
             * group install routine.
             */
            member_cnt =
                lt_entry->member_cnt =
                lt_entry->rh_member_cnt;

            if (lt_entry->rh_member_cnt
                > lt_entry->max_members) {
                rv = SHR_E_PARAM;
                SHR_ERR_EXIT(rv);
            }
            /*
             * Allocate memory to store the RH balanced members for
             * installation in hardware tables.
             */
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                BCMCTH_TRUNK_VP_ALLOC
                    (lt_entry->rh_entries_arr[i],
                     lt_entry->max_members *
                     sizeof(*(lt_entry->rh_entries_arr[i])),
                     "bcmcthTrunkVpRhGrpAddLtEntRhEntArr");
            }

            /*
             * Allocate memory to store the RH group entry's mapped member index
             * data.
             */
            BCMCTH_TRUNK_VP_ALLOC
                (lt_entry->rh_mindex_arr,
                 lt_entry->max_members * sizeof(*(lt_entry->rh_mindex_arr)),
                 "bcmcthTrunkVpRhGrpAddLtEntRhMindexArr");

            /*
             * Allocate memory to store the RH group members entries usage count
             * data.
             */
            if (lt_entry->rh_member_cnt) {
                BCMCTH_TRUNK_VP_ALLOC
                    (lt_entry->rh_memb_ecnt_arr,
                     lt_entry->rh_member_cnt *
                     sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                     "bcmcthTrunkVpRhGrpAddLtEntRhMembEntCntArr");
            }

            /*
             * Set RH random seed from the user,
             * which is used for selecting member
             */
            BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id) =
                 lt_entry->rh_rand_seed;

            SHR_IF_ERR_EXIT
                (bcmcth_trunk_vp_group_rh_entries_set(unit, lt_entry));


            BCMCTH_TRUNK_VP_ALLOC(
                       BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(unit, trunk_vp_id),
                       sizeof(*BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)),
                       "bcmcthTrunkVpRhGrpAddRhgInfo");

            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
                BCMCTH_TRUNK_VP_ALLOC(
                   BCMCTH_TRUNK_VP_GRP_VOID_RHG_BK_PTR(unit, trunk_vp_id),
                   sizeof(*BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)),
                   "bcmcthTrunkVpRhGrpAddRhgInfoBk");
            }

            /* Allocate memory to store RH TRUNK_VP group entries. */
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                BCMCTH_TRUNK_VP_ALLOC
                    (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                     lt_entry->max_members *
                     sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                     unit, trunk_vp_id, i))),
                     "bcmcthTrunkVpRhGrpAddGrpRhEntArr");
            }
            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
                for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                    BCMCTH_TRUNK_VP_ALLOC
                        (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                         unit, trunk_vp_id, i),
                         lt_entry->max_members *
                         sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(unit,
                                  trunk_vp_id, i))),
                         "bcmcthTrunkVpRhGrpAddGrpRhEntArrBk");
                }
            }
            /* Copy load balanced RH members array. */
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                           unit, trunk_vp_id, i),
                           lt_entry->rh_entries_arr[i],
                           sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit,
                                  trunk_vp_id, i))) * lt_entry->max_members);
            }

            /*
             * Allocate memory to store group RH TRUNK_VP entries mapped member
             * index information.
             */
            BCMCTH_TRUNK_VP_ALLOC
                (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                 lt_entry->max_members *
                 sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                  unit, trunk_vp_id))),
                 "bcmcthTrunkVpRhGrpAddGrpRhMindexArr");
            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
                BCMCTH_TRUNK_VP_ALLOC
                    (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id),
                     lt_entry->max_members *
                     sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                     unit, trunk_vp_id))),
                     "bcmcthTrunkVpRhGrpAddGrpRhMindexArrBk");
            }

            /* Copy group entries mapped member index array. */
            sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                       lt_entry->rh_mindex_arr,
                       (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                        unit, trunk_vp_id)) *
                        lt_entry->max_members));

            /*
             * Allocate memory to store RH TRUNK_VP members entries usage count
             * information.
             */
            BCMCTH_TRUNK_VP_ALLOC
                (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
                 max_members *
                 sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                 unit, trunk_vp_id))),
                 "bcmcthTrunkVpRhGrpAddGrpRhMembEntCntArr");
            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
                BCMCTH_TRUNK_VP_ALLOC
                    (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
                     unit, trunk_vp_id),
                     max_members *
                     sizeof(*(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR
                                    (unit, trunk_vp_id))),
                     "bcmcthTrunkVpRhGrpAddGrpRhMembEntCntArrBk");
            }

            /* Copy members entry usage count information. */
            sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                       unit, trunk_vp_id),
                       lt_entry->rh_memb_ecnt_arr,
                       (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                        trunk_vp_id)) * member_cnt));

        }

        SHR_IF_ERR_EXIT
            (bcmcth_trunk_drv_vp_grp_insert(unit, lt_entry));

        /* Store TRUNK_VP Group information in Per-Group Info Array. */
        BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id) = lt_entry->grp_type;
        BCMCTH_TRUNK_VP_GRP_LT_SID(unit, trunk_vp_id) = lt_entry->glt_sid;
        if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                   lt_entry->weighted_size;
        } else {
            if (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode)) {
                BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id) =
                         max_members;
                BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                       lt_entry->lb_mode;
            } else {
                BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                       lt_entry->lb_mode;
            }
        }
        BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) = max_members;
        BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) = member_cnt;
        BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id) =
                   lt_entry->mstart_idx;


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
        if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                BCMCTH_TRUNK_VP_FREE(lt_entry->rh_entries_arr[i]);
                lt_entry->rh_entries_arr[i] = NULL;
            }
            if (lt_entry->rh_mindex_arr) {
                BCMCTH_TRUNK_VP_FREE(lt_entry->rh_mindex_arr);
            }
            if (lt_entry->rh_mindex_arr) {
                BCMCTH_TRUNK_VP_FREE(lt_entry->rh_mindex_arr);
            }
            /* Free temp RH group member entries usage count array. */
            if (lt_entry->rh_memb_ecnt_arr) {
                BCMCTH_TRUNK_VP_FREE(lt_entry->rh_memb_ecnt_arr);
                lt_entry->rh_memb_ecnt_arr = NULL;
            }
            lt_entry->rh_mindex_arr = NULL;
        }
        /* Check if it's error return case. */
        if (SHR_FUNC_ERR()) {
            /*
             * On error return, free memory allocated
             * for managing the RH TRUNK_VP
             * group members.
             */
            if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
                (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
                if (BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)) {
                    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                        if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                           unit, trunk_vp_id, i)) {
                            BCMCTH_TRUNK_VP_FREE(
                               BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, \
                                         trunk_vp_id, i));
                        }
                    }
                    if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id)) {
                        BCMCTH_TRUNK_VP_FREE(
                          BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id));
                    }
                    if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                        unit, trunk_vp_id)) {
                        BCMCTH_TRUNK_VP_FREE(
                          BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit,
                                                                 trunk_vp_id));
                    }
                    BCMCTH_TRUNK_VP_FREE(
                          BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(unit, trunk_vp_id));
                }

                if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)
                    && BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)) {
                    for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                        if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                              unit, trunk_vp_id, i)) {
                            BCMCTH_TRUNK_VP_FREE(
                              BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                              unit, trunk_vp_id, i));
                        }
                    }
                    if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                              unit, trunk_vp_id)) {
                        BCMCTH_TRUNK_VP_FREE(
                             BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit,
                                                                  trunk_vp_id));
                    }
                    if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
                             unit, trunk_vp_id)) {
                        BCMCTH_TRUNK_VP_FREE(
                             BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
                                                            unit,
                                                            trunk_vp_id));
                    }
                    BCMCTH_TRUNK_VP_FREE(
                            BCMCTH_TRUNK_VP_GRP_VOID_RHG_BK_PTR(unit,
                                                                trunk_vp_id));
                }
            }
        }

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
    bcmcth_trunk_vp_lt_entry_t *cur_lt_entry;
    uint32_t rh_entries_cnt = 0;
    int rv = SHR_E_NONE;
    int i = 0;

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

    /*
     * Use stored rh_size value for update operation if it has not been
     * supplied.
     */
    if ((gtype == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
        BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode)) {
        if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
            lt_entry->fbmp,
            BCMCTH_TRUNK_VP_LT_FIELD_RH_SIZE)) {

            /* Retrive and use group lb_mode value. */
            rh_entries_cnt = BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(
                                 unit,
                                 lt_entry->trunk_vp_id);
            /* Retrive and use group lb_mode value. */
            lt_entry->rh_size =
            bcmcth_trunk_vp_member_cnt_convert_to_rh_size(rh_entries_cnt);

            lt_entry->max_members =
                    bcmcth_trunk_vp_rh_size_convert_to_member_cnt(
                     lt_entry->rh_size);

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                        "RH_SIZE=%d.\n"), lt_entry->rh_size));
        } else if (lt_entry->rh_entries_cnt !=
                   BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id)) {
            /*resize the TRUNK_VP group*/
            SHR_IF_ERR_EXIT(
              bcmcth_trunk_vp_group_max_members_update(unit, lt_entry));
        }
    } else {
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

    /*
     * Use stored group rh_member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
             lt_entry->fbmp,
             BCMCTH_TRUNK_VP_LT_FIELD_RH_RANDOM_SEED)) {

        /* Retrive and use group rh_mode value. */
        if (BCMCTH_TRUNK_VP_GRP_RH_INFO_PTR(unit)) {
            lt_entry->rh_rand_seed =
                BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id);
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "RH_MODE=%u.\n"), lt_entry->rh_rand_seed));
    }

    /*
     * Use stored group rh_member_cnt value for update operation if it has not
     * been supplied.
     */
    if (!BCMCTH_TRUNK_VP_LT_FIELD_GET(
              lt_entry->fbmp,
              BCMCTH_TRUNK_VP_LT_FIELD_RH_MEMBER_CNT)) {

        /* Retrive and use group member_cnt value. */
        lt_entry->rh_member_cnt =
             BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                    "RH_MEMBER_CNT=%u.\n"), lt_entry->rh_member_cnt));
    }

    /* Get logical table entry buffer pointer. */
    cur_lt_entry = BCMCTH_TRUNK_VP_CURRENT_LT_ENT_PTR(unit);

    if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
        (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_member_fields_fill(unit, cur_lt_entry,
                                             lt_entry, FALSE));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_vp_member_fields_fill(unit, cur_lt_entry,
                                             lt_entry, TRUE));
    }

    /* Initialize Start index value used by hardware write operation. */
    lt_entry->mstart_idx = BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX
                                (unit, trunk_vp_id);

    max_members = lt_entry->max_members;
    member_cnt = lt_entry->member_cnt;

    if (max_members < member_cnt) {
        LOG_VERBOSE(BSL_LOG_MODULE,
              (BSL_META_U(unit,
              "TRUNK_VP_ID=%d MAX_MEMBERS=%d cannot "
              "be less than MEMBER_CNT=%d.\n"),
               trunk_vp_id, max_members, member_cnt));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Check if group's LB_MODE is RESILIENT and call the corresponding
     * group install routine.
     */
    if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
        (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
        lt_entry->member_cnt = lt_entry->rh_member_cnt;
        member_cnt = lt_entry->rh_member_cnt;
        if (lt_entry->rh_member_cnt > lt_entry->max_members) {
            rv = SHR_E_PARAM;
            SHR_ERR_EXIT(rv);
        }
        /*
         * Allocate memory to store the RH balanced nhop members for
         * installation in hardware tables.
         */
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            BCMCTH_TRUNK_VP_ALLOC
                (lt_entry->rh_entries_arr[i],
                 lt_entry->max_members * sizeof(*(lt_entry->rh_entries_arr[i])),
                 "bcmcthTrunkVpRhGrpAddLtEntRhEntArr");
        }

        /*
         * Allocate memory to store the RH group entry's mapped member index
         * data.
         */
        BCMCTH_TRUNK_VP_ALLOC
            (lt_entry->rh_mindex_arr,
             lt_entry->max_members * sizeof(*(lt_entry->rh_mindex_arr)),
             "bcmcthTrunkVpRhGrpAddLtEntRhMindexArr");

        /*
         * Allocate memory to store the RH group members entries usage count
         * data.
         */
        if (lt_entry->rh_member_cnt) {
            BCMCTH_TRUNK_VP_ALLOC
                (lt_entry->rh_memb_ecnt_arr,
                 lt_entry->rh_member_cnt *
                 sizeof(*(lt_entry->rh_memb_ecnt_arr)),
                 "bcmcthTrunkVpRhGrpAddLtEntRhMembEntCntArr");
        }

        /*
         * Set RH random seed from the user, which is used for selecting member
         */
        BCMCTH_TRUNK_VP_GRP_RH_RAND_SEED(unit, trunk_vp_id) =
              lt_entry->rh_rand_seed;

        /*
         * If MEMBER_CNT value for this RH-group was zero prior to this update
         * operation and valid member fields are being added to this RH-group
         * in this UPDATE operation i.e. MEMBER_CNT > 0, then this UPDATE
         * operation is equivalent to new next-hop members INSERT operation for
         * this group. So, use RH entries SET function for load balance
         * operation.
         */
        if ((BCMCTH_TRUNK_VP_LT_FIELD_GET
                (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_RH_MEMBER_CNT)
                && member_cnt
                && !BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id)) ||
            (lt_entry->max_members != cur_lt_entry->max_members) ||
            (BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) == 0)) {
            SHR_IF_ERR_EXIT
                (bcmcth_trunk_vp_group_rh_entries_set(unit, lt_entry));
        } else {
            /* Check if pre-config flag is set for this group. */
            if (BCMCTH_TRUNK_VP_ENT_PRECONFIG_GET
                  (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype), trunk_vp_id)) {

                /*
                 * Rebuild is not needed since the entry is already present
                 * in the HA grp pointers.
                 */
                SHR_IF_ERR_EXIT
                    (bcmcth_trunk_vp_rh_grp_members_recover(unit, lt_entry));

                /* Clear pre-config flag for this RH TRUNK_VP group. */
                BCMCTH_TRUNK_VP_ENT_PRECONFIG_CLEAR
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype), trunk_vp_id);
            }

            /*
             * This is an existing RH group members Add/Delete/Replace type of
             * operation. So, use RH entries UPDATE function to perform the
             * rebalancing work.
             */
            SHR_IF_ERR_EXIT
                (bcmcth_trunk_vp_group_rh_entries_update(unit, lt_entry));
        }
    }

    /* Install TRUNK_VP Group in hardware tables. */
    SHR_IF_ERR_EXIT
            (bcmcth_trunk_drv_vp_grp_insert(unit, lt_entry));
    if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
        (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {
        /* Copy updated group load balanced RH member entries array. */
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(unit, trunk_vp_id, i),
                       lt_entry->rh_entries_arr[i],
                       (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                       unit, trunk_vp_id, i)) *
                              lt_entry->max_members));
        }

        /* Copy updated group entries mapped member index array. */
        sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id),
                   lt_entry->rh_mindex_arr,
                   (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                   unit, trunk_vp_id)) *
                          lt_entry->max_members));

        /* Copy updated members entry usage count array. */
        sal_memcpy(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id),
                   lt_entry->rh_memb_ecnt_arr,
                   (sizeof(*BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                   unit, trunk_vp_id)) *
                    member_cnt));
    }

    if (BCMCTH_TRUNK_VP_GRP_IS_WEIGHTED(gtype)) {
        BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
               lt_entry->weighted_size;
    } else {
        if (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode)) {
            BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_CNT(unit, trunk_vp_id) =
                     max_members;
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                       lt_entry->lb_mode;
        } else {
            BCMCTH_TRUNK_VP_GRP_LB_MODE(unit, trunk_vp_id) =
                   lt_entry->lb_mode;
        }
    }

    BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id) = lt_entry->max_members;
    BCMCTH_TRUNK_VP_GRP_MEMBER_CNT(unit, trunk_vp_id) = member_cnt;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "[EID=%d]: Max_Paths=%u Num_Paths=%u LB=%u.\n"),
                trunk_vp_id,
                max_members,
                member_cnt,
                lt_entry->lb_mode));

    exit:
        /* Free memory allocated for RH group re-balance operation. */
        if ((lt_entry->grp_type == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
            (BCMCTH_TRUNK_VP_GRP_IS_RH(lt_entry->lb_mode))) {

            /* Free temp RH group entries array. */
            for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                BCMCTH_TRUNK_VP_FREE(lt_entry->rh_entries_arr[i]);
                lt_entry->rh_entries_arr[i] = NULL;
            }

            /* Free temp RH group entry mapped member index array. */
            BCMCTH_TRUNK_VP_FREE(lt_entry->rh_mindex_arr);

            /* Free temp RH group member entries usage count array. */
            BCMCTH_TRUNK_VP_FREE(lt_entry->rh_memb_ecnt_arr);
            lt_entry->rh_mindex_arr = NULL;
            lt_entry->rh_memb_ecnt_arr = NULL;
        }
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
    int i = 0;

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

    if ((BCMCTH_TRUNK_VP_GRP_TYPE(unit, trunk_vp_id)
         == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) &&
         BCMCTH_TRUNK_VP_GRP_IS_RH(BCMCTH_TRUNK_VP_GRP_LB_MODE(
         unit, trunk_vp_id))
        && (BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)))  {

        if (BCMCTH_TRUNK_VP_ENT_PRECONFIG_GET
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype), trunk_vp_id)) {
            /* Clear pre-config flag for this RH-TRUNK_VP group. */
            BCMCTH_TRUNK_VP_ENT_PRECONFIG_CLEAR
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype), trunk_vp_id);
        }

        /*
         * Free memory allocated for storing per RH entry mapped member's
         * index information.
         */
        BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
         unit, trunk_vp_id));

        /*
         * Free memory allocated for storing RH load balanced group
         * entries.
         */
        for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
            BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
            unit, trunk_vp_id, i));
        }

        /*
         * Free memory allocated for storing per member RH entries usage
         * count.
         */
        BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
             unit, trunk_vp_id));

        /* Free memory allocated for storing group's RH info. */
        BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(
             unit, trunk_vp_id));

    }
    /* Free ITBM block member pool resources. */
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

    /* Free ITBM block group resources. */
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
/*!
 * \breif Cleanup the resilient hashing group info data.
 *
 * Free the allocated memoery for resilient hashing
 * group trunk_vp info for both the group and
 * group backup data.
 *
 * \param [in] unit     Unit number.
 *
 * \return Nothing.
 */
void
bcmcth_trunk_vp_rh_groups_cleanup(int unit)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = 0; /* Group identifier. */
    uint32_t     i = 0;
    bcmcth_trunk_vp_info_t *trunk_vp_info;
    bcmcth_trunk_vp_control_t *trunk_vp_ctrl;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcmcth_trunk_vp_rh_groups_cleanup.\n")));

    trunk_vp_ctrl = bcmcth_trunk_vp_ctrl_get(unit);
    trunk_vp_info = &(trunk_vp_ctrl->trunk_vp_info);


    if ((trunk_vp_info == NULL ) ||
        (trunk_vp_info->rhg  == NULL) ||
        ((BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit) &&
         trunk_vp_info->rhg_bk == NULL))) {
        return;
    }

    if (BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR(unit, group, gtype)) {
        for (trunk_vp_id =
                  BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID(unit, group, gtype);
            trunk_vp_id <=
                  BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID(unit, group, gtype);
             trunk_vp_id++) {

            if (BCMCTH_TRUNK_VP_GRP_RHG_PTR(unit, trunk_vp_id)) {
                for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                    if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                                         unit,
                                         trunk_vp_id,
                                         i)) {
                        BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_PTR(
                                         unit, \
                                         trunk_vp_id,
                                         i));
                    }
                }
                if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(unit, trunk_vp_id)) {
                    BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_PTR(
                                         unit,
                                         trunk_vp_id));
                }
                if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(unit, trunk_vp_id)) {
                    BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_PTR(
                                          unit,
                                          trunk_vp_id));
                }
                BCMCTH_TRUNK_VP_FREE(
                      BCMCTH_TRUNK_VP_GRP_VOID_RHG_PTR(unit, trunk_vp_id));
            }
            if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)
                && BCMCTH_TRUNK_VP_GRP_RHG_BK_PTR(unit, trunk_vp_id)) {
                for (i = 0; i < BCMCTH_TRUNK_VP_MAX_MEMBER_FLDS; ++i) {
                    if (BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                                         unit,
                                         trunk_vp_id, i)) {

                        BCMCTH_TRUNK_VP_FREE(
                         BCMCTH_TRUNK_VP_GRP_RH_ENTRIES_BK_PTR(
                                         unit,
                                         trunk_vp_id,
                                         i));
                    }
                }
                if (BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(unit, trunk_vp_id)) {
                    BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_RH_MINDEX_BK_PTR(
                                         unit,
                                         trunk_vp_id));
                }
                if (BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
                          unit, trunk_vp_id)) {
                    BCMCTH_TRUNK_VP_FREE(
                     BCMCTH_TRUNK_VP_GRP_RH_MEMB_ENTCNT_BK_PTR(
                                         unit,
                                         trunk_vp_id));
                }
                BCMCTH_TRUNK_VP_FREE(BCMCTH_TRUNK_VP_GRP_VOID_RHG_BK_PTR(
                                         unit,
                                         trunk_vp_id));
            }

        }
    }
    return;
}

/*!
 * \brief Check if Requested space can be allocated..
 *
 * \param [in] unit     Unit number.
 * \param [in] gtype    group type.
 * \param [in] req_size Minimum size of free block
 *             required to satisfy request.
 *
 * \return TRUE Requested space can be allocated.
 * \return FAIL Requested space cannot be allocated.
 */
int
bcmcth_trunk_vp_check_member_pool_resource(
                                   int unit,
                                   const bcmcth_trunk_vp_grp_type_t gtype,
                                   uint32_t req_size)
{
    shr_itbm_blk_t *itbm_blk = NULL, *free_blk = NULL, largest_free_blk;
    int rv = SHR_E_NONE;
    shr_itbm_defrag_blk_mseq_t *m_seq;
    uint32_t itbm_blk_count;
    uint32_t free_count = 0;
    int req_complete = FALSE;
    uint32_t bucket = SHR_ITBM_INVALID;
    uint32_t free_idx;

    LOG_VERBOSE(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "%s.\n"), __func__));

    /* Get free base index from member table. */
    rv = shr_itbm_list_block_alloc
            (BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
             BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
             req_size, &bucket, &free_idx);
    if (SHR_E_NONE == rv) {
        rv = shr_itbm_list_block_free(
                     BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                     (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                     req_size, SHR_ITBM_INVALID, free_idx);
        return TRUE;
    } else if ((SHR_E_FULL != rv) && (SHR_E_RESOURCE != rv)) {
        return FALSE;
    }

    /*
     * Initialize group member start index to
     * the table Min index value as we have
     * to start the compression operation
     * from the first (Min) index of the
     * table.
     */
    rv = shr_itbm_list_defrag_start(BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                      BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                      &itbm_blk_count, &itbm_blk,
                      &m_seq, &free_count, &free_blk, &largest_free_blk);

    req_complete = FALSE;
    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "defrag start failed rv=%d\n"),
                             rv));
        return FALSE;
    }
    if (free_count <= 0) {
        req_complete = FALSE;
    } else if (largest_free_blk.ecount >= req_size) {
        req_complete = TRUE;
    }

    rv = shr_itbm_list_defrag_end(
             BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
             BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)), false);
    if (rv != SHR_E_NONE) {
        req_complete = FALSE;
        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "defrag end failed rv=%d\n"),
                             rv));
    }
    return req_complete;
}


