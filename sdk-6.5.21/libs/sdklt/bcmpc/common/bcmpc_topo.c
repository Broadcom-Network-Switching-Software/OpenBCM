/*! \file bcmpc_topo.c
 *
 * BCMPC Topology APIs for PM Instance
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_util_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_TOPO

/*!
 * \brief Port macro type entry structure.
 *
 * The deployed PM types are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM type id.
 */
typedef struct bcmpc_topo_entry_type_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Type info. */
    bcmpc_topo_type_t info;

} bcmpc_topo_entry_type_t;

/*!
 * \brief Port macro instance entry structure.
 *
 * The deployed PM instances are stored within a linked list.
 * This data structure represents the element of the list.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM instance id.
 */
typedef struct bcmpc_topo_entry_inst_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! Instance info. */
    bcmpc_topo_t info;

} bcmpc_topo_entry_inst_t;

/* PM type database */
static bcmpc_db_entry_t *topo_type_db[BCMPC_NUM_UNITS_MAX];

/* PM instance database */
static bcmpc_db_entry_t *topo_inst_db[BCMPC_NUM_UNITS_MAX];


/*******************************************************************************
 * Internal public functions
 */

void
bcmpc_topo_type_t_init(bcmpc_topo_type_t *tinfo)
{
    sal_memset(tinfo, 0, sizeof(bcmpc_topo_type_t));
    tinfo->num_aggr = 1;
    tinfo->num_plls = 1;
}

int
bcmpc_topo_type_create(int unit, int tid)
{
    bcmpc_topo_entry_type_t *new = NULL;
    bcmpc_db_entry_t *dbe;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    /* Check if tid is duplicated */
    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    if (dbe) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Allocate a new entry */
    SHR_ALLOC(new, sizeof(bcmpc_topo_entry_type_t), "bcmpcTopoType");
    SHR_NULL_CHECK(new, SHR_E_MEMORY);

    /* Init the the value of the entry */
    sal_memset(new, 0, sizeof(bcmpc_topo_entry_type_t));

    /* Type ID is the key of the table */
    new->dbe.keys[0] = tid;

    /* Add the new entry to DB table */
    bcmpc_db_entry_add(&topo_type_db[unit], &new->dbe);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_destroy(int unit, int tid)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_del(&topo_type_db[unit], 1, &key);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_set(int unit, int tid, bcmpc_topo_type_t *tinfo)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    sal_memcpy(&(entry->info), tinfo, sizeof(bcmpc_topo_type_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_get(int unit, int tid, bcmpc_topo_type_t *tinfo)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_type_t *entry;
    uint32_t key = tid;

    SHR_FUNC_ENTER(unit);

    bcmpc_topo_type_t_init(tinfo);

    dbe = bcmpc_db_entry_search(topo_type_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
    sal_memcpy(tinfo, &(entry->info), sizeof(bcmpc_topo_type_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_type_traverse(int unit, bcmpc_topo_type_traverse_cb_f cb_func,
                         void *cookie)
{
    int rv = SHR_E_NONE;
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_type_t *entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    dbe = bcmpc_db_entry_next(topo_type_db[unit], NULL);
    while (dbe) {
        next = bcmpc_db_entry_next(topo_type_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_type_t, dbe);
        rv = (*cb_func)(unit, entry->dbe.keys[0], &entry->info, cookie);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        dbe = next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_id_get(int unit, bcmpc_pport_t pport, int *pm_id)
{
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_inst_t *entry;
    int phys_pm_port;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_next(topo_inst_db[unit], NULL);

    while (dbe) {
        bcmpc_topo_t *pm_info;
        bcmpc_topo_type_t tinfo;

        next = bcmpc_db_entry_next(topo_inst_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
        pm_info = &entry->info;

        SHR_IF_ERR_EXIT
            (bcmpc_topo_type_get(unit, pm_info->tid, &tinfo));

        if ((pport >= pm_info->base_pport) &&
            (pport < pm_info->base_pport + tinfo.num_ports) &&
             pm_info->is_active) {
                /* Check if the physical port is invalid. */
                phys_pm_port = pport - pm_info->base_pport;
                if (SHR_BITGET(pm_info->invalid_pbmp, phys_pm_port)) {
                    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                }
                *pm_id = dbe->keys[0];
                SHR_EXIT();
        }

        dbe = next;
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_topo_t_init(bcmpc_topo_t *pm_info)
{
    sal_memset(pm_info, 0, sizeof(bcmpc_topo_t));
}

int
bcmpc_topo_create(int unit, bool warm, int pm_id)
{
    bcmpc_topo_entry_inst_t *new = NULL;
    pm_info_t *pm_info = NULL;
    pm_ha_info_t *pm_ha_info = NULL;
    bcmpc_db_entry_t *dbe;
    uint32_t key = pm_id;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;

    SHR_FUNC_ENTER(unit);

    /* Check if pm_id is duplicated */
    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (dbe) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    /* Allocate a new entry */
    SHR_ALLOC(new, sizeof(bcmpc_topo_entry_inst_t), "bcmpcTopoInst");
    SHR_NULL_CHECK(new, SHR_E_MEMORY);

    /* Initialize the the value of the entry */
    sal_memset(new, 0, sizeof(bcmpc_topo_entry_inst_t));

    /* Allocate memory for storing Port Macro information */
    SHR_ALLOC(pm_info, sizeof(pm_info_t), "bcmpcPmInfo");
    SHR_NULL_CHECK(pm_info, SHR_E_MEMORY);

    /* Initialize the the value of the entry */
    sal_memset(pm_info, 0, sizeof(pm_info_t));

    pm_info->pm_lock = sal_mutex_create("bcmpcPmDbLock");
    SHR_NULL_CHECK(pm_info->pm_lock, SHR_E_MEMORY);

    new->info.pm_info = pm_info;

    /* Initialize PM specific HA information. */
    ha_req_size = sizeof(pm_ha_info_t);
    ha_alloc_size = ha_req_size;

    pm_ha_info = shr_ha_mem_alloc(unit, BCMMGMT_PC_COMP_ID, pm_id,
                                  "bcmpcPmInfo", &ha_alloc_size);
    SHR_NULL_CHECK(pm_ha_info, SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        sal_memset(pm_ha_info, 0, ha_alloc_size);
        bcmissu_struct_info_report(unit, BCMMGMT_PC_COMP_ID,
                                   pm_id, 0, sizeof(pm_ha_info_t), 1,
                                   PM_HA_INFO_T_ID);
    }

    pm_info->pm_data.pm_ha_info = pm_ha_info;

    /* Instance ID is the key of the table */
    new->dbe.keys[0] = pm_id;

    /* Add the new entry to DB table */
    bcmpc_db_entry_add(&topo_inst_db[unit], &new->dbe);

exit:
    if (SHR_FUNC_ERR()) {
        if (pm_ha_info) {
            shr_ha_mem_free(unit, pm_ha_info);
        }
        if (pm_info && pm_info->pm_lock) {
            sal_mutex_destroy(pm_info->pm_lock);
            pm_info->pm_lock = NULL;
        }
        SHR_FREE(pm_info);
        SHR_FREE(new);
    }

    SHR_FUNC_EXIT();
}

int
bcmpc_topo_destroy(int unit, int pm_id)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    pm_info_t  *pm_info;
    bcmpc_topo_type_t tinfo;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
    pm_info = entry->info.pm_info;

    SHR_IF_ERR_EXIT(bcmpc_topo_type_get(unit, entry->info.tid, &tinfo));

    /* Free the port macro resources by calling pm_destroy. */
    if ((tinfo.pm_drv != NULL) && (tinfo.pm_drv->pm_destroy != NULL)) {
        SHR_IF_ERR_EXIT(tinfo.pm_drv->pm_destroy(unit, pm_info));
    }

    if (pm_info->pm_lock != NULL) {
        sal_mutex_destroy(pm_info->pm_lock);
        pm_info->pm_lock = NULL;
    }

    dbe = bcmpc_db_entry_del(&topo_inst_db[unit], 1, &key);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_FREE(pm_info);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_set(int unit, int pm_id, bcmpc_topo_t *pm_info)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    pm_info_t *pm_db_info;
    uint32_t key = pm_id;
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t pm_offset = 0;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);

    pm_db_info = entry->info.pm_info;

    sal_memcpy(&(entry->info), pm_info, sizeof(bcmpc_topo_t));

    entry->info.pm_info = pm_db_info;

    SHR_IF_ERR_EXIT
        (bcmpc_pm_type_params_set(unit, entry->info.tid, pm_db_info));


    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->pm_offset_get) {
        SHR_IF_ERR_EXIT(pc_drv->pm_offset_get(unit, pm_id, &pm_offset));
        pm_db_info->pm_data.pm_offset = pm_offset;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_get(int unit, int pm_id, bcmpc_topo_t *pm_info)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
    sal_memcpy(pm_info, &(entry->info), sizeof(bcmpc_topo_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_traverse(int unit, bcmpc_topo_traverse_cb_f cb_func,
                    void *cookie)
{
    int rv = SHR_E_NONE;
    bcmpc_db_entry_t *dbe, *next;
    bcmpc_topo_entry_inst_t *entry;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb_func, SHR_E_PARAM);

    dbe = bcmpc_db_entry_next(topo_inst_db[unit], NULL);
    while (dbe) {
        next = bcmpc_db_entry_next(topo_inst_db[unit], dbe);

        entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);
        rv = (*cb_func)(unit, entry->dbe.keys[0], &entry->info, cookie);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        dbe = next;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_update(int unit, int pm_id, bcmpc_topo_t *topo_info)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_topo_entry_inst_t *entry;
    pm_info_t *pm_db_info;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(topo_inst_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_topo_entry_inst_t, dbe);

    pm_db_info = entry->info.pm_info;
    sal_memcpy(&(entry->info), topo_info, sizeof(bcmpc_topo_t));
    if (pm_db_info != topo_info->pm_info) {
        entry->info.pm_info = pm_db_info;
        sal_memcpy((entry->info.pm_info), topo_info->pm_info,
                   sizeof(pm_info_t));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_instances_create(int unit, int blktype, int type_id, bool warm,
                            int pm_id_start, int *inst_cnt)
{
    int pm_id = pm_id_start;
    bcmpc_topo_t pm_info;
    const bcmdrd_chip_info_t *ci;
    bcmpc_pport_t pport;
    bcmdrd_pbmp_t pbmp, pbmp_inactive;
    bcmpc_drv_t *pc_drv = NULL;
    bool skip_pm_instance = FALSE;

    SHR_FUNC_ENTER(unit);

    ci = bcmdrd_dev_chip_info_get(unit);
    if (ci == NULL) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* need to check pm instance needs to be skipped for certain blk */
    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (bcmpc_block_pbmp_get(unit, ci, blktype, &pbmp, &pbmp_inactive));
    BCMDRD_PBMP_ITER(pbmp, pport) {
        if (pc_drv && pc_drv->skip_pm_instance) {
            /* need to skip this block PM instance creation */
            SHR_IF_ERR_EXIT
                (pc_drv->skip_pm_instance(unit, pport, &skip_pm_instance));
            if (skip_pm_instance) {
                continue;
            }
        }

        SHR_IF_ERR_EXIT
            (bcmpc_topo_create(unit, warm, pm_id));
        bcmpc_topo_t_init(&pm_info);
        pm_info.tid = type_id;
        pm_info.base_pport = pport;
        if (!BCMDRD_PBMP_MEMBER(pbmp_inactive, pport)) {
            pm_info.is_active = TRUE;
        }
        SHR_IF_ERR_EXIT
            (bcmpc_topo_set(unit, pm_id, &pm_info));
        pm_id++;
    }

    *inst_cnt = pm_id - pm_id_start;

exit:
    SHR_FUNC_EXIT();
}
