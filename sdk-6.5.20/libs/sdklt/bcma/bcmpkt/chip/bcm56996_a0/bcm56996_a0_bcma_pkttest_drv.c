/*! \file bcm56996_a0_bcma_pkttest_drv.c
 *
 * Chip specific functions for Packet I/O test.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_sleep.h>
#include <bcmlt/bcmlt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcma/bcmpkt/bcma_bcmpkt_test_internal.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static int
lt_find_first_unused_entry(int unit, const char *table_name,
                           const char *field_name, uint64_t *entry_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t data = 1;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, table_name, &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, field_name, data));

    while (1) {
        status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL);
        if (status == SHR_E_NOT_FOUND) {
            break;
        }
        SHR_IF_ERR_EXIT(status);
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, field_name, ++data));
    }
    *entry_id = data;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_ifp_enable(int unit, int port)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t fp_enable;
    int status;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "PORT_FP", &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));

    /* Get current FP_ING field value */
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NONE) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ent_hdl, "FP_ING", &fp_enable));
        if (fp_enable == TRUE) {
            SHR_EXIT();
        }
    }

    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "FP_ING", TRUE));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_port_ifp_disable(int unit, int port)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, "PORT_FP", &ent_hdl));
    SHR_IF_ERR_EXIT(bcmlt_entry_field_add(ent_hdl, "PORT_ID", port));

    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("PORT_FP %"PRId32" not exist\n"),
                  port));
        SHR_EXIT();
    }

    SHR_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_group_create(int unit, fp_qual_type_t qual_type, uint64_t *grp_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_GRP_TEMPLATE",
                                    "FP_ING_GRP_TEMPLATE_ID", grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_GRP_TEMPLATE", &ent_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", *grp_id));

    switch (qual_type) {
        case FP_QUAL_INPORT:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ENTRY_PRIORITY",
                                       IFP_GROUP_PRI));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "MODE_AUTO",
                                       IFP_GROUP_MODE_AUTO));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT_BITMAP",
                                       IFP_GROUP_INPORT_BITMAP));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_group_destroy(int unit, uint64_t grp_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_GRP_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", grp_id));

    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_GRP %"PRId64" not exist\n"),
                  grp_id));
        SHR_EXIT();
    }

    SHR_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_policy_add(int unit, int redirect_port, fp_policy_type_t policy_type,
                  uint64_t *policy_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_POLICY_TEMPLATE",
                                    "FP_ING_POLICY_TEMPLATE_ID", policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_POLICY_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               *policy_id));
    switch (policy_type) {
        case FP_POLICY_DROP:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_G_DROP", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_Y_DROP", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_R_DROP", TRUE));
            break;
        case FP_POLICY_COPYTOCPU:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_G_COPY_TO_CPU", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_Y_COPY_TO_CPU", TRUE));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "ACTION_R_COPY_TO_CPU", TRUE));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

    if (redirect_port > 0) {
        int rv;
        const char *field = "ACTION_UNMODIFIED_REDIRECT_TO_PORT";
        uint64_t field_data;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        rv = bcmlt_entry_field_get(ent_hdl, field, &field_data);
        if (rv == SHR_E_NONE) {
            field = "ACTION_UNMODIFIED_REDIRECT_TO_PORT";
        } else if (rv == SHR_E_NOT_FOUND) {
            field = "ACTION_REDIRECT_TO_PORT";
        } else {
            SHR_ERR_EXIT(rv);
        }

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(ent_hdl, field, redirect_port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_policy_delete(int unit, uint64_t policy_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_POLICY_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_POLICY %"PRId64" not exist\n"),
                  policy_id));
        SHR_EXIT();
    }

    SHR_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_rule_add(int unit, int port, fp_qual_type_t qual_type, uint64_t *rule_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (port < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_RULE_TEMPLATE",
                                    "FP_ING_RULE_TEMPLATE_ID", rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", *rule_id));
    switch (qual_type) {
        case FP_QUAL_INPORT:
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT", port));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_add(ent_hdl, "QUAL_INPORT_MASK",
                                       IFP_RULE_INPORT_MASK));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            break;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_rule_delete(int unit, uint64_t rule_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_RULE_TEMPLATE", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", rule_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_RULE %"PRId64" not exist\n"),
                  rule_id));
        SHR_EXIT();
    }

    SHR_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_entry_install(int unit, uint64_t grp_id, uint64_t policy_id,
                     uint64_t rule_id, uint64_t *entry_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* find a unused entry */
    SHR_IF_ERR_EXIT
        (lt_find_first_unused_entry(unit, "FP_ING_ENTRY",
                                    "FP_ING_ENTRY_ID", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", *entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_GRP_TEMPLATE_ID", grp_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_RULE_TEMPLATE_ID", rule_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "ENTRY_PRIORITY", PRIORITY_HIGH));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
lt_ifp_entry_remove(int unit, uint64_t entry_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
       (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", entry_id));
    status = bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_DELETE,
                                BCMLT_PRIORITY_NORMAL);
    if (status == SHR_E_NOT_FOUND) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("FP_ING_ENTRY %"PRId64" not exist\n"),
                  entry_id));
        SHR_EXIT();
    }

    SHR_ERR_EXIT(status);

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
pkttest_dpath_create(int unit, int port, int redirect_port,
                     bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    fp_qual_type_t qual_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(lt_port_ifp_enable(unit, port));

    qual_type = FP_QUAL_INPORT;

    SHR_IF_ERR_EXIT(lt_ifp_group_create(unit, qual_type, &ifp_cfg->grp_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_rule_add(unit, port, qual_type, &ifp_cfg->rule_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_policy_add(unit, -1, FP_POLICY_DROP,
                           &ifp_cfg->drop_policy_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_policy_add(unit, redirect_port, FP_POLICY_COPYTOCPU,
                           &ifp_cfg->copy_policy_id));
    SHR_IF_ERR_EXIT
        (lt_ifp_entry_install(unit, ifp_cfg->grp_id, ifp_cfg->copy_policy_id,
                              ifp_cfg->rule_id, &ifp_cfg->entry_id));
exit:
    SHR_FUNC_EXIT();
}

static int
pkttest_dpath_destroy(int unit, int port, bcma_bcmpkt_test_fp_cfg_t *ifp_cfg)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ifp_cfg, SHR_E_PARAM);

    if (ifp_cfg->copy_policy_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_policy_delete(unit, ifp_cfg->copy_policy_id));
    }
    if (ifp_cfg->drop_policy_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_policy_delete(unit, ifp_cfg->drop_policy_id));

    }
    if (ifp_cfg->rule_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_rule_delete(unit, ifp_cfg->rule_id));
    }
    if (ifp_cfg->entry_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_entry_remove(unit, ifp_cfg->entry_id));
    }
    if (ifp_cfg->grp_id > 0) {
        SHR_IF_ERR_CONT
            (lt_ifp_group_destroy(unit, ifp_cfg->grp_id));
    }
    SHR_IF_ERR_CONT
        (lt_port_ifp_disable(unit, port));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Destroy data path failed\n")));
    }
    SHR_FUNC_EXIT();
}

static int
pkttest_lt_policy_update(int unit, uint64_t entry_id, uint64_t policy_id)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "FP_ING_ENTRY", &ent_hdl));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_ENTRY_ID", entry_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(ent_hdl, "FP_ING_POLICY_TEMPLATE_ID",
                               policy_id));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

static bcma_bcmpkt_test_drv_t bcm56996_a0_pkttest_drv = {
    .dpath_create = pkttest_dpath_create,
    .dpath_destroy = pkttest_dpath_destroy,
    .lt_policy_update = pkttest_lt_policy_update,
};

int
bcma_bcmpkt_test_bcm56996_a0_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcma_bcmpkt_test_drv_set(unit, &bcm56996_a0_pkttest_drv));

exit:
    SHR_FUNC_EXIT();
}
