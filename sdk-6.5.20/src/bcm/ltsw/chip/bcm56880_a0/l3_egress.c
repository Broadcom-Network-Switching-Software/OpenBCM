/*! \file l3_egress.c
 *
 * BCM56880_A0 L3 Egress Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/xfs/l3_egress.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/******************************************************************************
 * Private functions
 */

/*!
 * \brief clear all source ECMP table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_clear(int unit)
{
    int i, num;
    const char *ltname[] = {
        ING_L3_SRC_ECMP_GROUP_A_TABLEs,
        ING_L3_SRC_ECMP_GROUP_B_TABLEs
    };

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Fill entry handle of ING_L3_SRC_ECMP_GROUP_A/B_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [in] cfg Data of ING_L3_SRC_ECMP_GROUP_A/B_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_src_fill(int unit, bcmlt_entry_handle_t eh, ecmp_src_cfg_t *cfg)
{
    uint32_t i = 0;
    uint32_t start_idx = 0;
    static const char *l3_oif_str[] = {L3_OIF0_VALUE_TYPEs,
                                       L3_OIF1_VALUE_TYPEs,
                                       L3_OIF2_VALUE_TYPEs,
                                       L3_OIF3_VALUE_TYPEs,
                                       L3_OIF4_VALUE_TYPEs,
                                       L3_OIF5_VALUE_TYPEs,
                                       L3_OIF6_VALUE_TYPEs,
                                       L3_OIF7_VALUE_TYPEs};

    SHR_FUNC_ENTER(unit);

    if ((cfg->start_idx +
         cfg->num_of_unset + cfg->num_of_set) > ECMP_SRC_GROUP_MAX_MEMBER) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_ECMP_GROUP_INDEXs,
                               cfg->l3_src_ecmp_group_index));

    if (cfg->fld_bmp & ES_FLD_COUNT) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, COUNTs,
                                   (cfg->count > 0) ? (cfg->count - 1) : 0));
    }

    if (cfg->fld_bmp & ES_FLD_COUNT_GT8) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, COUNT_GT8s, cfg->count_gt8));
    }

    if (cfg->fld_bmp & ES_FLD_L3_OIF) {

        start_idx = cfg->start_idx;

        for (i = 0; i < cfg->num_of_set; i++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(eh, l3_oif_str[start_idx + i],
                                       cfg->l3_oif[start_idx + i]));
        }

        start_idx = cfg->start_idx + cfg->num_of_set;

        for (i = 0; i < cfg->num_of_unset; i++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(eh, l3_oif_str[start_idx + i], 0));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse entry handle of ING_L3_SRC_ECMP_GROUP_A/B_TABLEt.
 *
 * \param [in] unit Unit number
 * \param [in] eh LT entry handle.
 * \param [out] cfg Data of ING_L3_SRC_ECMP_GROUP_A/B_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lt_ecmp_src_parse(int unit, bcmlt_entry_handle_t eh, ecmp_src_cfg_t *cfg)
{
    uint32_t i = 0;
    uint32_t start_idx = 0;
    uint64_t first_l3_oif = 0;
    static const char *l3_oif_str[] = {L3_OIF0_VALUE_TYPEs,
                                       L3_OIF1_VALUE_TYPEs,
                                       L3_OIF2_VALUE_TYPEs,
                                       L3_OIF3_VALUE_TYPEs,
                                       L3_OIF4_VALUE_TYPEs,
                                       L3_OIF5_VALUE_TYPEs,
                                       L3_OIF6_VALUE_TYPEs,
                                       L3_OIF7_VALUE_TYPEs};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, L3_SRC_ECMP_GROUP_INDEXs,
                               &cfg->l3_src_ecmp_group_index));

    if (cfg->fld_bmp & ES_FLD_COUNT) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, COUNTs, &cfg->count));

        if (cfg->count == 0) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(eh, l3_oif_str[0], &first_l3_oif));

            if (first_l3_oif) {
                cfg->count += 1;
            }
        } else {
            cfg->count += 1;
        }
    }

    if (cfg->fld_bmp & ES_FLD_COUNT_GT8) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(eh, COUNT_GT8s, &cfg->count_gt8));
    }

    if (cfg->fld_bmp & ES_FLD_L3_OIF) {
        start_idx = cfg->start_idx;
        for (i = 0; i < ECMP_SRC_GROUP_TABLE_MAX_MEMBER; i++) {
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_get(eh, l3_oif_str[start_idx + i],
                                       &(cfg->l3_oif[start_idx + i])));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get source ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [out] cfg Data of ING_L3_SRC_ECMP_GROUP_A/B_TABLEt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
secmp_get(int unit, int idx, ecmp_src_cfg_t *cfg)
{
    int dunit;
    const char *ltname;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    cfg->l3_src_ecmp_group_index = idx;

    dunit = bcmi_ltsw_dev_dunit(unit);

    if ((cfg->fld_bmp & ES_FLD_COUNT) ||
        (cfg->fld_bmp & ES_FLD_COUNT_GT8) ||
        (cfg->fld_bmp & ES_FLD_L3_OIF)) {

        ltname = ING_L3_SRC_ECMP_GROUP_A_TABLEs;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_SRC_ECMP_GROUP_INDEXs,
                                   cfg->l3_src_ecmp_group_index));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        cfg->start_idx = 0;

        SHR_IF_ERR_EXIT
            (lt_ecmp_src_parse(unit, eh, cfg));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

    if (cfg->fld_bmp & ES_FLD_L3_OIF) {

        ltname = ING_L3_SRC_ECMP_GROUP_B_TABLEs;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ltname, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(eh, L3_SRC_ECMP_GROUP_INDEXs,
                                   cfg->l3_src_ecmp_group_index));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        cfg->start_idx = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;

        SHR_IF_ERR_EXIT
            (lt_ecmp_src_parse(unit, eh, cfg));

        cfg->start_idx = 0;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set source ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_set(
    int unit,
    int idx,
    bcm_l3_egress_ecmp_t *ecmp_info,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count)
{
    int i = 0;
    ecmp_src_cfg_t secmp;
    uint32_t num_of_set = 0;
    uint32_t l3_oif = 0;
    int dunit = 0;
    const char *ltname;
    bcmlt_opcode_t op;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&secmp, 0, sizeof(ecmp_src_cfg_t));

    secmp.l3_src_ecmp_group_index = idx;

    for (i = 0; i < count; i++) {

        if (member[i].nhop_1 == 0) {
            continue;
        }

        if (num_of_set >= ECMP_SRC_GROUP_MAX_MEMBER) {
            secmp.count_gt8 = 1;
            break;
        }

        SHR_IF_ERR_EXIT
            (xfs_ltsw_snh_ing_l3_oif_get(unit, member[i].nhop_1, &l3_oif));

        secmp.l3_oif[num_of_set++] = l3_oif;
    }

    secmp.count = num_of_set;
    secmp.start_idx = 0;
    secmp.num_of_set = (num_of_set >= ECMP_SRC_GROUP_TABLE_MAX_MEMBER) ?
                       (ECMP_SRC_GROUP_TABLE_MAX_MEMBER) : num_of_set;
    if (ecmp_info->flags & BCM_L3_REPLACE) {
        secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER - secmp.num_of_set;
    }

    ltname = ING_L3_SRC_ECMP_GROUP_A_TABLEs;
    secmp.fld_bmp = ES_FLD_COUNT | ES_FLD_COUNT_GT8 | ES_FLD_L3_OIF;

    if (ecmp_info->flags & BCM_L3_REPLACE) {
        op = BCMLT_OPCODE_UPDATE;
    } else {
        op = BCMLT_OPCODE_INSERT;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ltname, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    ltname = ING_L3_SRC_ECMP_GROUP_B_TABLEs;
    secmp.fld_bmp = ES_FLD_L3_OIF;
    secmp.start_idx = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;
    secmp.num_of_set = (num_of_set > ECMP_SRC_GROUP_TABLE_MAX_MEMBER) ?
                       (num_of_set - ECMP_SRC_GROUP_TABLE_MAX_MEMBER) : 0;
    if (ecmp_info->flags & BCM_L3_REPLACE) {
        secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER - secmp.num_of_set;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_B_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, op, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of source ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_del(int unit, int idx)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_A_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_ECMP_GROUP_INDEXs, idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_B_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, L3_SRC_ECMP_GROUP_INDEXs, idx));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer to ECMP member info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_member_add(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member)
{
    int dunit;
    ecmp_src_cfg_t secmp;
    uint32_t l3_oif = 0;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (member->nhop_1 == 0) {
         SHR_EXIT();
    }

    sal_memset(&secmp, 0, sizeof(ecmp_src_cfg_t));

    secmp.fld_bmp = ES_FLD_COUNT | ES_FLD_COUNT_GT8;

    SHR_IF_ERR_EXIT
        (secmp_get(unit, idx, &secmp));

    secmp.fld_bmp = 0;

    if (secmp.count < ECMP_SRC_GROUP_MAX_MEMBER) {
        /* Update count and l3_oif. */
        secmp.start_idx = secmp.count;
        secmp.num_of_set = 1;

        SHR_IF_ERR_EXIT
            (xfs_ltsw_snh_ing_l3_oif_get(unit, member->nhop_1, &l3_oif));
        secmp.l3_oif[secmp.start_idx] = l3_oif;

        secmp.fld_bmp = ES_FLD_COUNT;
        secmp.count++;
    } else {
        if (secmp.count_gt8 == 0) {
            /* Update gt8 flag. */
            secmp.fld_bmp = ES_FLD_COUNT_GT8;
            secmp.count_gt8 = 1;

        } else {
            /* Nothing to be updated. */
            SHR_EXIT();
        }
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    if ((secmp.fld_bmp & ES_FLD_COUNT_GT8) ||
        (secmp.fld_bmp & ES_FLD_COUNT)) {

        /* Update Group A. */
        if ((secmp.fld_bmp & ES_FLD_COUNT) &&
            (secmp.start_idx < ECMP_SRC_GROUP_TABLE_MAX_MEMBER)) {
            secmp.fld_bmp |= ES_FLD_L3_OIF;
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_A_TABLEs, &eh));

        SHR_IF_ERR_EXIT
            (lt_ecmp_src_fill(unit, eh, &secmp));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_free(eh));
        eh = BCMLT_INVALID_HDL;
    }

    if ((secmp.fld_bmp & ES_FLD_COUNT) &&
        (secmp.start_idx >= ECMP_SRC_GROUP_TABLE_MAX_MEMBER)) {

        /* Update Group B. */
        secmp.fld_bmp = ES_FLD_L3_OIF;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_B_TABLEs, &eh));

        SHR_IF_ERR_EXIT
            (lt_ecmp_src_fill(unit, eh, &secmp));

        SHR_IF_ERR_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 * \param [in] del_member Information to ECMP member to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_member_del(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count,
    bcmi_ltsw_l3_ecmp_member_info_t *del_member)
{
    int dunit, i;
    bool del = false;
    uint32_t num_of_set = 0;
    uint32_t l3_oif = 0;
    ecmp_src_cfg_t secmp;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (del_member->nhop_1 == 0) {
        SHR_EXIT();
    }

    sal_memset(&secmp, 0, sizeof(ecmp_src_cfg_t));

    secmp.l3_src_ecmp_group_index = idx;

    for (i = 0; i < count; i++) {

        if (member[i].nhop_1 == 0) {
            continue;
        }

        if ((del == false) && (del_member->nhop_1 == member[i].nhop_1)) {
            del = true;
            continue;
        }

        if (num_of_set >= ECMP_SRC_GROUP_MAX_MEMBER) {
            secmp.count_gt8 = 1;
            break;
        }

        SHR_IF_ERR_EXIT
            (xfs_ltsw_snh_ing_l3_oif_get(unit, member[i].nhop_1, &l3_oif));

        secmp.l3_oif[num_of_set++] = l3_oif;
    }

    secmp.count = num_of_set;
    secmp.start_idx = 0;
    secmp.num_of_set = (num_of_set >= ECMP_SRC_GROUP_TABLE_MAX_MEMBER) ?
                       (ECMP_SRC_GROUP_TABLE_MAX_MEMBER) : num_of_set;
    secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER - secmp.num_of_set;

    secmp.fld_bmp = ES_FLD_COUNT | ES_FLD_COUNT_GT8 | ES_FLD_L3_OIF;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_A_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    secmp.fld_bmp = ES_FLD_L3_OIF;
    secmp.start_idx = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;
    secmp.num_of_set = (num_of_set > ECMP_SRC_GROUP_TABLE_MAX_MEMBER) ?
                       (num_of_set - ECMP_SRC_GROUP_TABLE_MAX_MEMBER) : 0;
    secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER - secmp.num_of_set;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_B_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all members from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56880_a0_l3_secmp_member_del_all(int unit, int idx)
{
    int dunit;
    ecmp_src_cfg_t secmp;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&secmp, 0, sizeof(ecmp_src_cfg_t));

    dunit = bcmi_ltsw_dev_dunit(unit);

    secmp.l3_src_ecmp_group_index = idx;
    secmp.count = 0;
    secmp.start_idx = 0;
    secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;
    secmp.fld_bmp = ES_FLD_COUNT | ES_FLD_COUNT_GT8 | ES_FLD_L3_OIF;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_A_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));
    eh = BCMLT_INVALID_HDL;

    secmp.fld_bmp = ES_FLD_L3_OIF;
    secmp.start_idx = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;
    secmp.num_of_unset = ECMP_SRC_GROUP_TABLE_MAX_MEMBER;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, ING_L3_SRC_ECMP_GROUP_B_TABLEs, &eh));

    SHR_IF_ERR_EXIT
        (lt_ecmp_src_fill(unit, eh, &secmp));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_l3_egress_drv_t bcm56880_a0_ltsw_l3_egr_drv = {
    .l3_egress_l2_rsv = xfs_ltsw_l3_egress_l2_rsv,
    .l3_egress_init = xfs_ltsw_l3_egress_init,
    .l3_egress_deinit = xfs_ltsw_l3_egress_deinit,
    .l3_egress_id_range_get = xfs_ltsw_l3_egress_id_range_get,
    .l3_egress_create = xfs_ltsw_l3_egress_create,
    .l3_egress_destroy = xfs_ltsw_l3_egress_destroy,
    .l3_egress_get = xfs_ltsw_l3_egress_get,
    .l3_egress_find = xfs_ltsw_l3_egress_find,
    .l3_egress_traverse = xfs_ltsw_l3_egress_traverse,
    .l3_egress_obj_id_resolve = xfs_ltsw_l3_egress_obj_id_resolve,
    .l3_egress_obj_id_construct = xfs_ltsw_l3_egress_obj_id_construct,
    .l3_egress_obj_info_get = xfs_ltsw_l3_egress_obj_info_get,
    .l3_egress_obj_cnt_get = xfs_ltsw_l3_egress_obj_cnt_get,
    .l3_egress_dvp_set = xfs_ltsw_l3_egress_dvp_set,
    .l3_egress_flexctr_attach = xfs_ltsw_l3_egress_flexctr_attach,
    .l3_egress_flexctr_detach = xfs_ltsw_l3_egress_flexctr_detach,
    .l3_egress_flexctr_info_get = xfs_ltsw_l3_egress_flexctr_info_get,
    .l3_egress_flexctr_object_set = xfs_ltsw_l3_egress_flexctr_object_set,
    .l3_egress_flexctr_object_get = xfs_ltsw_l3_egress_flexctr_object_get,
    .l3_egress_ifa_create = xfs_ltsw_l3_egress_ifa_create,
    .l3_egress_ifa_destroy = xfs_ltsw_l3_egress_ifa_destroy,
    .l3_egress_ifa_get = xfs_ltsw_l3_egress_ifa_get,
    .l3_egress_ifa_traverse = xfs_ltsw_l3_egress_ifa_traverse,
    .l3_ecmp_create = xfs_ltsw_l3_ecmp_create,
    .l3_ecmp_destroy = xfs_ltsw_l3_ecmp_destroy,
    .l3_ecmp_get = xfs_ltsw_l3_ecmp_get,
    .l3_ecmp_member_add = xfs_ltsw_l3_ecmp_member_add,
    .l3_ecmp_member_del = xfs_ltsw_l3_ecmp_member_del,
    .l3_ecmp_member_del_all = xfs_ltsw_l3_ecmp_member_del_all,
    .l3_ecmp_find = xfs_ltsw_l3_ecmp_find,
    .l3_ecmp_trav = xfs_ltsw_l3_ecmp_trav,
    .l3_ecmp_max_paths_get = xfs_ltsw_l3_ecmp_max_paths_get,
    .l3_ecmp_member_status_set = xfs_ltsw_l3_ecmp_member_status_set,
    .l3_ecmp_member_status_get = xfs_ltsw_l3_ecmp_member_status_get,
    .l3_ecmp_dlb_mon_config_set = xfs_ltsw_l3_ecmp_dlb_mon_config_set,
    .l3_ecmp_dlb_mon_config_get = xfs_ltsw_l3_ecmp_dlb_mon_config_get,
    .ecmp_member_dest_alloc = xfs_ltsw_ecmp_member_dest_alloc,
    .ecmp_member_dest_get = xfs_ltsw_ecmp_member_dest_get,
    .ecmp_member_dest_free = xfs_ltsw_ecmp_member_dest_free,
    .ecmp_member_dest_update = xfs_ltsw_ecmp_member_dest_update,
    .l3_ecmp_dlb_stat_set = xfs_ltsw_l3_ecmp_dlb_stat_set,
    .l3_ecmp_dlb_stat_get = xfs_ltsw_l3_ecmp_dlb_stat_get,
    .l3_ecmp_dlb_stat_sync_get = xfs_ltsw_l3_ecmp_dlb_stat_sync_get,
    .l3_secmp_clear = bcm56880_a0_l3_secmp_clear,
    .l3_secmp_set = bcm56880_a0_l3_secmp_set,
    .l3_secmp_del = bcm56880_a0_l3_secmp_del,
    .l3_secmp_member_add = bcm56880_a0_l3_secmp_member_add,
    .l3_secmp_member_del = bcm56880_a0_l3_secmp_member_del,
    .l3_secmp_member_del_all = bcm56880_a0_l3_secmp_member_del_all,
    .l3_egress_trunk_member_del = xfs_ltsw_l3_egress_trunk_member_delete,
    .l3_egress_trunk_member_add = xfs_ltsw_l3_egress_trunk_member_add
};

/******************************************************************************
 * Public functions
 */

int
bcm56880_a0_ltsw_l3_egress_drv_attach(int unit)
{
    return mbcm_ltsw_l3_egress_drv_set(unit, &bcm56880_a0_ltsw_l3_egr_drv);
}
