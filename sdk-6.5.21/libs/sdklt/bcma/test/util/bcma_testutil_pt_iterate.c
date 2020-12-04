/*! \file bcma_testutil_pt_iterate.c
 *
 * Physical table iteration utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST
#define BCMA_TESTUTIL_PT_ITERATE_LIMIT  (100000000)

static int
handle_entity(int unit, bcmdrd_sid_t sid, int addr_idx,
              bool process_individual_pipe,
              bcma_testutil_pt_entity_handle_f handle_cb,
              void *user_data)
{
    bcmbd_pt_dyn_info_t dyn_info;
    int inst_num, inst_idx, rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(handle_cb, SHR_E_PARAM);

    inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);

    /* Iterate all table instance */
    for (inst_idx = 0; inst_idx < inst_num; inst_idx++) {

        if (!bcmdrd_pt_index_valid(unit, sid, inst_idx, addr_idx)) {
            continue;
        }

        dyn_info.index = addr_idx;
        dyn_info.tbl_inst = inst_idx;

        /* handle entity without override info */
        rv = handle_cb(unit, sid, &dyn_info, NULL, user_data);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        /* handle entity with override info if need */
        if (process_individual_pipe == true &&
            bcmdrd_feature_is_sim(unit) == false) {
            bcma_testutil_drv_t *drv = NULL;
            drv = bcma_testutil_drv_get(unit);

            SHR_NULL_CHECK(drv, SHR_E_UNIT);
            SHR_NULL_CHECK(drv->pt_entity_override, SHR_E_UNAVAIL);

            rv = drv->pt_entity_override(unit, sid, &dyn_info,
                                         handle_cb, user_data);
            if (SHR_FAILURE(rv)) {
                SHR_ERR_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
handle_table(bcmdrd_sid_t sid, const bcma_testutil_pt_iter_t *pt_iter)
{
    bcmdrd_sym_info_t sinfo;
    uint32_t idx;
    int rv;
    int entity_handle_id;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(pt_iter->unit, sid, &sinfo));

    if (pt_iter->table_handle != NULL) {
        rv = pt_iter->table_handle(pt_iter->unit, sid, pt_iter->user_data);
        if (SHR_FAILURE(rv)) {
            /*
             * SHR_E_UNAVAIL means caller don't expect entity_handle to be executed
             */
            if (rv == SHR_E_UNAVAIL) {
                rv = SHR_E_NONE;
            }
            SHR_ERR_EXIT(rv);
        }
    }

    /* Iterate all entity handler */
    for (entity_handle_id = 0;
         entity_handle_id < BCMA_TESTUTIL_PT_ENTITY_HANDLE_MAX;
         entity_handle_id++) {

        bcma_testutil_pt_entity_handle_f handle_cb =
            pt_iter->entity_handle[entity_handle_id];

        if (handle_cb == NULL) {
            continue;
        }

        /* Iterate all address index */
        if (pt_iter->entity_next_id == NULL) {
            for (idx = sinfo.index_min; idx <= sinfo.index_max; idx++) {
                rv = handle_entity(pt_iter->unit, sid, idx,
                                   pt_iter->entity_handle_pipes[entity_handle_id],
                                   handle_cb, pt_iter->user_data);
                if (SHR_FAILURE(rv)) {
                    /*
                     * SHR_E_UNAVAIL means caller expect to skip the rest of entities
                     */
                    if (rv == SHR_E_UNAVAIL) {
                        rv = SHR_E_NONE;
                    }
                    SHR_ERR_EXIT(rv);
                }
            }
        } else {
            int cnt = 0;
            int next_id;
            while (1) {
                next_id = pt_iter->entity_next_id(pt_iter->unit,
                                                  pt_iter->user_data);
                if (next_id == -1) {
                    break;
                }
                rv = handle_entity(pt_iter->unit, sid, next_id,
                                   pt_iter->entity_handle_pipes[entity_handle_id],
                                   handle_cb, pt_iter->user_data);
                if (SHR_FAILURE(rv)) {
                    /*
                     * SHR_E_UNAVAIL means caller expect to skip the rest of entities
                     */
                    if (rv == SHR_E_UNAVAIL) {
                        rv = SHR_E_NONE;
                    }
                    SHR_ERR_EXIT(rv);
                }
                cnt++;
                if (cnt > BCMA_TESTUTIL_PT_ITERATE_LIMIT) {
                    BSL_LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META("Possible infinite loop detected.\n")));
                    SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

exit:
    /* Cleanup initial conditions. */
    if (pt_iter->table_cleanup != NULL) {
        SHR_IF_ERR_EXIT(
            pt_iter->table_cleanup(pt_iter->unit, sid));
    }

    SHR_FUNC_EXIT();
}

void
bcma_testutil_pt_iterate_init(bcma_testutil_pt_iter_t *pt_iter)
{
    if (pt_iter != NULL) {
        int i;
        sal_memset(pt_iter, 0, sizeof(bcma_testutil_pt_iter_t));
        for (i = 0; i < BCMA_TESTUTIL_PT_ENTITY_HANDLE_MAX; i++) {
            pt_iter->entity_handle_pipes[i] = false;
        }
    }
}

int
bcma_testutil_pt_iterate(const bcma_testutil_pt_iter_t *pt_iter)
{
    bcmdrd_sid_t *sid_list = NULL;
    bcmdrd_sid_t sid;
    bcmdrd_sym_info_t sinfo;
    size_t num_of_sid, i;
    const char *name = NULL;
    uint32_t pflags = 0, aflags = 0;
    int exact_match = 0, count = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(pt_iter, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_iter->name, SHR_E_PARAM);

    if (sal_strcmp(pt_iter->name, BCMA_TESTUTIL_PT_ALL_REGS) == 0) {
        pflags = BCMDRD_SYMBOL_FLAG_REGISTER;
        aflags = BCMDRD_SYMBOL_FLAG_MEMMAPPED;
        name = "*";
    } else if (sal_strcmp(pt_iter->name, BCMA_TESTUTIL_PT_ALL_MEMS) == 0) {
        pflags = BCMDRD_SYMBOL_FLAG_MEMORY;
        name = "*";
    } else {
        name = pt_iter->name;
        exact_match = 1;
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(pt_iter->unit, 0, NULL, &num_of_sid));

    if (num_of_sid == 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sid_list = sal_alloc(sizeof(bcmdrd_sid_t) * num_of_sid, "bcmaTestPtIteration");
    if (sid_list == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_sid_list_get(pt_iter->unit, num_of_sid, sid_list, &num_of_sid));

    /* Iterate sid list */
    for (i = 0; i < num_of_sid; i++) {
        sid = sid_list[i];

        if (!bcmdrd_pt_is_valid(pt_iter->unit, sid)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_info_get(pt_iter->unit, sid, &sinfo));

        /* Check present flags */
        if (pflags && ((pflags & sinfo.flags) == 0)) {
            continue;
        }

        /* Check absent flags */
        if (aflags && ((aflags & sinfo.flags) != 0)) {
            continue;
        }

        if (pt_iter->blktype != BCMA_TESTUTIL_PT_ALL_BLOCK_TYPE) {
            int block = bcmdrd_pt_blktype_get(pt_iter->unit, sid, 0);
            if (block != pt_iter->blktype) {
                continue;
            }
        }

        /* Check name */
        if (exact_match && sal_strcasecmp(name, sinfo.name) != 0) {
            continue;
        }

        count++;

        SHR_IF_ERR_EXIT
            (handle_table(sid, pt_iter));

        if (exact_match) {
            break;
        }
    }

    if (count <= 0) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (sid_list != NULL) {
        sal_free(sid_list);
    }

    SHR_FUNC_EXIT();
}

