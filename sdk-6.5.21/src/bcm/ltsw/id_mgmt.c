/*! \file port.c
 *
 * ID Management Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <shared/hash_tbl.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/generated/id_mgmt_ha.h>
#include <bcm_int/ltsw/id_mgmt.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/stack.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>


#define BSL_LOG_MODULE BSL_LS_BCM_PORT

#define BCMINT_ID_MGMT_SUB_COMP_ID_GPORT 0
#define BCMINT_ID_MGMT_SUB_COMP_ID_L2_IF 1

#define ID_MGMT_LOCK(u)       \
        sal_mutex_take(id_mgmt_ctrl[(u)]->lock, sal_mutex_FOREVER)
#define ID_MGMT_UNLOCK(u)     \
        sal_mutex_give(id_mgmt_ctrl[(u)]->lock)

typedef struct link_node_s {
    struct link_node_s  *prev;
    struct link_node_s  *next;
} link_node_t;

typedef struct link_s {
    int         len;
    link_node_t *head;
    link_node_t *tail;
    link_node_t *node;
} link_t;

typedef struct gport_ctrl_s {
    int                     size;
    bcmint_id_gport_info_t  *info;
    link_t                  pool;
    shr_htb_hash_table_t    hash;
} gport_ctrl_t;

#define FREE    0x0
#define ALLOC   0x1
#define RESERVE 0x2
#define MAPPED  0x4     /* for l2_if only */
#define L2_IF   0x4     /* for gport only */
#define LAG     0x8     /* for gport only */

typedef struct l2_if_ctrl_s {
    int                     size;
    bcmint_id_l2_if_info_t  *info;
    link_t                  pool;
} l2_if_ctrl_t;

typedef struct id_mgmt_ctrl_s {
    sal_mutex_t     lock;
    gport_ctrl_t    gport;
    l2_if_ctrl_t    l2_if;
} id_mgmt_ctrl_t;

static id_mgmt_ctrl_t *id_mgmt_ctrl[BCM_MAX_NUM_UNITS];


/* link init, link all nodes */
static int
link_init(link_t *link, link_node_t *buf, int size)
{
    int i;

    link->len = size;
    link->node = buf;
    link->head = link->node;
    link->tail = link->node + size - 1;
    for (i = 0; i < size; i ++) {
        link->node[i].prev = &link->node[i - 1];
        link->node[i].next = &link->node[i + 1];
    }
    link->node[0].prev = NULL;
    link->node[size - 1].next = NULL;

    return SHR_E_NONE;
}


/* insert the node to head of the link */
static int
link_insert(link_t *link, link_node_t *node)
{
    node->next = link->head;
    node->prev = NULL;
    link->head = node;
    if (node->next) {
        node->next->prev = node;
    }
    link->len ++;

    return SHR_E_NONE;
}


/* remove the node from the link */
static int
link_remove(link_t *link, link_node_t *node)
{
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        link->head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        link->tail = node->prev;
    }
    node->next = NULL;
    node->prev = NULL;
    link->len --;

    return SHR_E_NONE;
}


/* link print */
static int
link_dump(link_t *link, int size)
{
    link_node_t *node, *base;
    int cnt;

    for (node = link->head, base = link->node, cnt = 0;
         node && (node - base < size);
         node = node->next) {
        printf("%8d", (int)(node - base));
        if (cnt % 8 == 7) {
            printf("\n");
        }
        if (cnt ++ > size) {
            printf("ID pool corruption!\n");
            return SHR_E_INTERNAL;
        }
    }
    printf("\n");

    return SHR_E_NONE;
}


static int
port_validate(int unit, int port_in, int *port_out)
{
    SHR_FUNC_ENTER(unit);

    if ((!BCM_GPORT_IS_SET(port_in)) || BCM_GPORT_IS_LOCAL(port_in)) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port_in, port_out));
    } else if (BCM_GPORT_IS_TRUNK(port_in) || BCM_GPORT_IS_BLACK_HOLE(port_in)){
        *port_out = port_in;
    } else if (BCM_GPORT_IS_MODPORT(port_in)) {
        int mymodid;
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &mymodid));
        /* Local port with modport flag. */
        if (mymodid == BCM_GPORT_MODPORT_MODID_GET(port_in)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_port_gport_validate
                    (unit, BCM_GPORT_MODPORT_PORT_GET(port_in), port_out));
        } else {
            *port_out = port_in;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


static int
port_to_gid(int unit, int gport, int *gid)
{
    shr_htb_data_t data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_htb_find(id_mgmt_ctrl[unit]->gport.hash,
                      (shr_htb_key_t)&gport, &data, FALSE));
    *gid = (long)data;

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_mgmt_init(int unit, int size)
{
    int alloc_size, hash_size, rv;
    int warm = bcmi_warmboot_get(unit);
    uint32_t ha_req_size, ha_alloc_size, offset;
    void *ptr;

    SHR_FUNC_ENTER(unit);

    SHR_FREE(id_mgmt_ctrl[unit]);
    alloc_size = sizeof(id_mgmt_ctrl_t)
               + sizeof(link_node_t) * size
               + sizeof(link_node_t) * size;
    SHR_ALLOC(id_mgmt_ctrl[unit], alloc_size, "id_mgmt_ctrl");
    SHR_NULL_CHECK(id_mgmt_ctrl[unit], SHR_E_MEMORY);

    id_mgmt_ctrl[unit]->lock = sal_mutex_create("id_mgmt_lock");
    id_mgmt_ctrl[unit]->gport.size = size;
    id_mgmt_ctrl[unit]->l2_if.size = size;

    /* gport pool */
    SHR_IF_ERR_EXIT
        (link_init(&id_mgmt_ctrl[unit]->gport.pool,
                   (link_node_t*)(id_mgmt_ctrl[unit] + 1), size));

    /* hash table of used gport */
    hash_size = 1;
    while (hash_size < size * 4) {  /* larger hash table for less conflict */
        hash_size <<= 1;
    }
    SHR_IF_ERR_EXIT
        (shr_htb_create(&id_mgmt_ctrl[unit]->gport.hash, hash_size, sizeof(int),
                        "gport_hash"));

    /* gport info saved in HA */
    ha_req_size = sizeof(bcmint_id_gport_info_t) * size;
    ha_alloc_size = ha_req_size;
    ptr = bcmi_ltsw_ha_mem_alloc
            (unit, BCMI_HA_COMP_ID_ID_MGMT, BCMINT_ID_MGMT_SUB_COMP_ID_GPORT,
             "bcmGportInfo", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm) {
        rv = bcmi_ltsw_issu_struct_info_get
                (unit, BCMINT_ID_GPORT_INFO_T_ID, BCMI_HA_COMP_ID_ID_MGMT,
                 BCMINT_ID_MGMT_SUB_COMP_ID_GPORT, &offset);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_issu_struct_info_report
                    (unit, BCMI_HA_COMP_ID_ID_MGMT,
                     BCMINT_ID_MGMT_SUB_COMP_ID_GPORT,
                     0, sizeof(bcmint_id_gport_info_t), size,
                     BCMINT_ID_GPORT_INFO_T_ID));
            sal_memset(ptr, 0, ha_alloc_size);
        } else {
            ptr = (void *)((uint8_t *)ptr + offset);
        }
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmi_ltsw_issu_struct_info_report
                (unit, BCMI_HA_COMP_ID_ID_MGMT, BCMINT_ID_MGMT_SUB_COMP_ID_GPORT,
                 0, sizeof(bcmint_id_gport_info_t), size,
                 BCMINT_ID_GPORT_INFO_T_ID),
                 SHR_E_EXISTS);
        sal_memset(ptr, 0, ha_alloc_size);
    }
    id_mgmt_ctrl[unit]->gport.info = (bcmint_id_gport_info_t*)ptr;

    /* l2_if pool */
    SHR_IF_ERR_EXIT
        (link_init(&id_mgmt_ctrl[unit]->l2_if.pool,
                   id_mgmt_ctrl[unit]->gport.pool.node + size, size));

    /* l2_if info saved in HA */
    ha_req_size = sizeof(bcmint_id_l2_if_info_t) * size;
    ha_alloc_size = ha_req_size;
    ptr = bcmi_ltsw_ha_mem_alloc
            (unit, BCMI_HA_COMP_ID_ID_MGMT, BCMINT_ID_MGMT_SUB_COMP_ID_L2_IF,
             "bcmL2IfInfo", &ha_alloc_size);
    SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
    if (warm) {
        rv = bcmi_ltsw_issu_struct_info_get
                (unit, BCMINT_ID_L2_IF_INFO_T_ID, BCMI_HA_COMP_ID_ID_MGMT,
                 BCMINT_ID_MGMT_SUB_COMP_ID_L2_IF, &offset);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_issu_struct_info_report
                    (unit, BCMI_HA_COMP_ID_ID_MGMT,
                     BCMINT_ID_MGMT_SUB_COMP_ID_L2_IF,
                     0, sizeof(bcmint_id_gport_info_t), size,
                     BCMINT_ID_L2_IF_INFO_T_ID));
            sal_memset(ptr, 0, ha_alloc_size);
        } else {
            ptr = (void *)((uint8_t *)ptr + offset);
        }
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmi_ltsw_issu_struct_info_report
                (unit, BCMI_HA_COMP_ID_ID_MGMT, BCMINT_ID_MGMT_SUB_COMP_ID_L2_IF,
                 0, sizeof(bcmint_id_gport_info_t), size,
                 BCMINT_ID_L2_IF_INFO_T_ID),
                 SHR_E_EXISTS);
        sal_memset(ptr, 0, ha_alloc_size);
    }
    id_mgmt_ctrl[unit]->l2_if.info = (bcmint_id_l2_if_info_t*)ptr;

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_mgmt_deinit(int unit)
{
    int alloc_size;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_htb_destroy(&id_mgmt_ctrl[unit]->gport.hash, NULL));

    sal_mutex_destroy(id_mgmt_ctrl[unit]->lock);

    alloc_size = sizeof(id_mgmt_ctrl_t)
               + sizeof(link_node_t) * id_mgmt_ctrl[unit]->gport.size
               + sizeof(link_node_t) * id_mgmt_ctrl[unit]->l2_if.size;
    sal_memset(id_mgmt_ctrl[unit], 0x0, alloc_size);
    SHR_FREE(id_mgmt_ctrl[unit]);

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_mgmt_recover(int unit)
{
    int l2_if, gid;
    gport_ctrl_t *gport_ctrl = &id_mgmt_ctrl[unit]->gport;
    l2_if_ctrl_t *l2_if_ctrl = &id_mgmt_ctrl[unit]->l2_if;

    SHR_FUNC_ENTER(unit);

    for (gid = 0; gid < gport_ctrl->size; gid ++) {
        if (gport_ctrl->info[gid].flags & (ALLOC | RESERVE)) {
            SHR_IF_ERR_EXIT
                (link_remove(&gport_ctrl->pool, &gport_ctrl->pool.node[gid]));
            SHR_IF_ERR_EXIT
                (shr_htb_insert(gport_ctrl->hash,
                    (shr_htb_key_t)&gport_ctrl->info[gid].gport,
                    (shr_htb_data_t)(long)gid));
        }
    }

    for (l2_if = 0; l2_if < l2_if_ctrl->size; l2_if ++) {
        if (l2_if_ctrl->info[l2_if].flags & (ALLOC | RESERVE)) {
            SHR_IF_ERR_EXIT
                (link_remove(&l2_if_ctrl->pool, &l2_if_ctrl->pool.node[l2_if]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_reserve(int unit, int l2_if, int num)
{
    int i, end;
    l2_if_ctrl_t *ctrl = &id_mgmt_ctrl[unit]->l2_if;
    link_node_t *prev, *next;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    if ((l2_if < 0) || (num < 0) || (l2_if + num > ctrl->size)
        || (num > ctrl->pool.len)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    for (i = l2_if, end = l2_if + num; i < end; i ++) {
        if (ctrl->info[i].flags != FREE) {
            /* already reserved */
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

    prev = ctrl->pool.node[l2_if].prev;
    next = ctrl->pool.node[l2_if + num - 1].next;
    if (prev) {
        prev->next = next;
    } else {
        ctrl->pool.head = next;
    }
    if (next) {
        next->prev = prev;
    } else {
        ctrl->pool.tail = prev;
    }
    ctrl->pool.len -= num;
    for (i = l2_if, end = l2_if + num; i < end; i ++) {
        ctrl->info[i].flags = RESERVE;
        ctrl->pool.node[i].next = NULL;
        ctrl->pool.node[i].prev = NULL;
    }

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_alloc(int unit, int *l2_if)
{
    link_node_t *node;
    l2_if_ctrl_t *ctrl = &id_mgmt_ctrl[unit]->l2_if;
    bcmint_id_l2_if_info_t *info;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    SHR_NULL_CHECK(l2_if, SHR_E_PARAM);

    node = ctrl->pool.head;
    *l2_if = node - ctrl->pool.node;
    info = ctrl->info + (*l2_if);

    if ((ctrl->pool.len > 0) && (info->flags == FREE)) {
        SHR_IF_ERR_EXIT
            (link_remove(&ctrl->pool, node));
        info->flags = ALLOC;
    } else {
        SHR_ERR_EXIT(SHR_E_EMPTY);
    }

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_free(int unit, int l2_if)
{
    link_node_t *node;
    l2_if_ctrl_t *ctrl = &id_mgmt_ctrl[unit]->l2_if;
    bcmint_id_l2_if_info_t *info;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    if ((l2_if < 0) || (l2_if >= ctrl->size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    node = ctrl->pool.node + l2_if;
    info = ctrl->info + l2_if;
    if ((ctrl->pool.len < ctrl->size) && (info->flags & (ALLOC | RESERVE))) {
        SHR_IF_ERR_EXIT
            (link_insert(&ctrl->pool, node));
        info->flags = FREE;
    } else {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_reserve(int unit, int gport)
{
    int gid;
    link_node_t *node;
    gport_ctrl_t *ctrl = &id_mgmt_ctrl[unit]->gport;
    bcmint_id_gport_info_t *info;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    /* do nothing if gport already exists */
    if (SHR_SUCCESS(port_to_gid(unit, gport, &gid))) {
        SHR_EXIT();
    }

    node = ctrl->pool.head;
    gid = node - ctrl->pool.node;
    info = ctrl->info + gid;

    if ((ctrl->pool.len > 0) && (info->flags == FREE)) {
        SHR_IF_ERR_EXIT
            (link_remove(&ctrl->pool, node));
        info->flags = ALLOC;
        info->gport = gport;
    } else {
        SHR_ERR_EXIT(SHR_E_EMPTY);
    }

    SHR_IF_ERR_EXIT
        (shr_htb_insert
            (ctrl->hash, (shr_htb_key_t)&gport, (shr_htb_data_t)(long)gid));

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_free(int unit, int gport)
{
    int gid;
    link_node_t *node;
    gport_ctrl_t *ctrl = &id_mgmt_ctrl[unit]->gport;
    shr_htb_data_t data;
    bcmint_id_gport_info_t *info;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    SHR_IF_ERR_EXIT
        (shr_htb_find(ctrl->hash, (shr_htb_key_t)&gport, &data, TRUE));
    gid = (long)data;

    if ((gid < 0) || (gid >= ctrl->size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    node = ctrl->pool.node + gid;
    info = ctrl->info + gid;
    if ((ctrl->pool.len < ctrl->size) && (info->flags & ALLOC)) {
        SHR_IF_ERR_EXIT
            (link_insert(&ctrl->pool, node));
        info->flags = FREE;
        info->gport = 0;
    } else {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_map_port(int unit, int l2_if, int gport)
{
    int gid;
    gport_ctrl_t *gport_ctrl = &id_mgmt_ctrl[unit]->gport;
    l2_if_ctrl_t *l2_if_ctrl = &id_mgmt_ctrl[unit]->l2_if;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    if ((l2_if < 0) || (l2_if >= l2_if_ctrl->size)) {
        return SHR_E_PARAM;
    }

    SHR_IF_ERR_EXIT
        (port_to_gid(unit, gport, &gid));
    gport_ctrl->info[gid].l2_if = l2_if;
    gport_ctrl->info[gid].flags |= L2_IF;

    l2_if_ctrl->info[l2_if].gid = gid;
    l2_if_ctrl->info[l2_if].flags |= MAPPED;

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_demap_port(int unit, int l2_if)
{
    int gid;
    gport_ctrl_t *gport_ctrl = &id_mgmt_ctrl[unit]->gport;
    l2_if_ctrl_t *l2_if_ctrl = &id_mgmt_ctrl[unit]->l2_if;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    if ((l2_if < 0) || (l2_if >= l2_if_ctrl->size)) {
        return SHR_E_PARAM;
    }

    gid = l2_if_ctrl->info[l2_if].gid;
    l2_if_ctrl->info[l2_if].gid = 0;
    l2_if_ctrl->info[l2_if].flags &= ~MAPPED;

    gport_ctrl->info[gid].l2_if = 0;
    gport_ctrl->info[gid].flags &= ~L2_IF;

    SHR_EXIT();

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_to_port(int unit, int l2_if, int *port)
{
    int gid, use_gport, modid;

    SHR_FUNC_ENTER(unit);

    if ((l2_if < 0) || (l2_if >= id_mgmt_ctrl[unit]->l2_if.size)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!(id_mgmt_ctrl[unit]->l2_if.info[l2_if].flags & MAPPED)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    gid = id_mgmt_ctrl[unit]->l2_if.info[l2_if].gid;
    *port = id_mgmt_ctrl[unit]->gport.info[gid].gport;

    if (!BCM_GPORT_IS_SET(*port)) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &modid));
        SHR_IF_ERR_EXIT
            (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &use_gport));
        if (use_gport || (modid != 0)) {
            /*
             * Local port with module id != 0, will always return gport type to
             * restore the correct module id.
             */
            SHR_IF_ERR_EXIT
                (bcm_ltsw_port_gport_get(unit, *port, port));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_to_l2_if(int unit, int port, int *l2_if)
{
    int gid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_validate(unit, port, &port));

    SHR_IF_ERR_EXIT
        (port_to_gid(unit, port, &gid));

    *l2_if = id_mgmt_ctrl[unit]->gport.info[gid].l2_if;

    if (!(id_mgmt_ctrl[unit]->l2_if.info[*l2_if].flags & MAPPED)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_lag_set(int unit, int gport, int gport_lag)
{
    int gid;

    SHR_FUNC_ENTER(unit);
    ID_MGMT_LOCK(unit);

    SHR_IF_ERR_EXIT
        (port_validate(unit, gport, &gport));

    SHR_IF_ERR_EXIT
        (port_to_gid(unit, gport, &gid));

    if (gport_lag == BCM_TRUNK_INVALID) {
        id_mgmt_ctrl[unit]->gport.info[gid].lag = 0;
        id_mgmt_ctrl[unit]->gport.info[gid].flags &= ~LAG;
    } else {
        id_mgmt_ctrl[unit]->gport.info[gid].lag = gport_lag;
        id_mgmt_ctrl[unit]->gport.info[gid].flags |= LAG;
    }

exit:
    ID_MGMT_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_lag_get(int unit, int gport, int *gport_lag)
{
    int gid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_validate(unit, gport, &gport));

    SHR_IF_ERR_EXIT
        (port_to_gid(unit, gport, &gid));

    if (!(id_mgmt_ctrl[unit]->gport.info[gid].flags & LAG)) {
        *gport_lag = BCM_TRUNK_INVALID;
    } else {
        *gport_lag = id_mgmt_ctrl[unit]->gport.info[gid].lag;
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_l2_if_dump_sw(int unit)
{
    l2_if_ctrl_t *ctrl = NULL;
    bcmint_id_l2_if_info_t *info;
    int l2_if, gport;

    SHR_FUNC_ENTER(unit);

    if (id_mgmt_ctrl[unit] == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    ctrl = &id_mgmt_ctrl[unit]->l2_if;

    LOG_CLI((BSL_META_U(unit, "\nSW Information L2_IF - Unit %d\n\n"),
             unit));

    LOG_CLI((BSL_META_U(unit, "  Used L2_IF list: %d/%d\n\n"),
             ctrl->size - ctrl->pool.len, ctrl->size));
    LOG_CLI((BSL_META_U(unit, "  l2_if     port    flag\n")));
    LOG_CLI((BSL_META_U(unit, "  =======================\n")));
    for (l2_if = 0; l2_if < ctrl->size; l2_if ++) {
        info = &ctrl->info[l2_if];
        if (info->flags == FREE) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "  %4d"), l2_if));
        gport = id_mgmt_ctrl[unit]->gport.info[info->gid].gport;
        if (!(info->flags & MAPPED)) {
            LOG_CLI((BSL_META_U(unit, "           ")));
        } else if (BCM_GPORT_IS_SET(gport)) {
            LOG_CLI((BSL_META_U(unit, " 0x%08x"), gport));
        } else {
            LOG_CLI((BSL_META_U(unit, "   %8d"), gport));
        }
        LOG_CLI((BSL_META_U(unit, "  0x%04X\n"), info->flags));
    }

    LOG_CLI((BSL_META_U(unit, "\n  Free L2_IF pool: %d/%d\n"),
             ctrl->pool.len, ctrl->size));
    LOG_CLI((BSL_META_U(unit, "\n  ========================= Free L2_IF =========================\n\n")));
    SHR_IF_ERR_EXIT
        (link_dump(&ctrl->pool, ctrl->size));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_CLI((BSL_META_U(unit, "Retrun fail!\n")));
    }
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_id_port_dump_sw(int unit)
{
    gport_ctrl_t *ctrl = NULL;
    bcmint_id_gport_info_t *info;
    int gid;

    SHR_FUNC_ENTER(unit);

    if (id_mgmt_ctrl[unit] == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    ctrl = &id_mgmt_ctrl[unit]->gport;

    LOG_CLI((BSL_META_U(unit, "\nSW Information Gport - Unit %d\n\n"),
             unit));

    LOG_CLI((BSL_META_U(unit, "  Used Gport list: %d/%d\n\n"),
             ctrl->size - ctrl->pool.len, ctrl->size));
    LOG_CLI((BSL_META_U(unit, "  gid     port   l2_if     lag      flag\n")));
    LOG_CLI((BSL_META_U(unit, "  =========================================\n")));
    for (gid = 0; gid < ctrl->size; gid ++) {
        info = &ctrl->info[gid];
        if (info->flags == FREE) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "%4d"), gid));
        if (BCM_GPORT_IS_SET(info->gport)) {
            LOG_CLI((BSL_META_U(unit, " 0x%08x"), info->gport));
        } else {
            LOG_CLI((BSL_META_U(unit, "   %8d"), info->gport));
        }
        if (!(info->flags & L2_IF)) {
            LOG_CLI((BSL_META_U(unit, "      ")));
        } else {
            LOG_CLI((BSL_META_U(unit, "  %4d"), info->l2_if));
        }
        if (!(info->flags & LAG)) {
            LOG_CLI((BSL_META_U(unit, "            ")));
        } else {
            LOG_CLI((BSL_META_U(unit, "  0x%08X"), info->lag));
        }
        LOG_CLI((BSL_META_U(unit, "  0x%04X\n"), info->flags));
    }

    LOG_CLI((BSL_META_U(unit, "\n  Free Gid pool: %d/%d\n"),
             ctrl->pool.len, ctrl->size));
    LOG_CLI((BSL_META_U(unit, "\n  ========================= Free Gid =========================\n\n")));
    SHR_IF_ERR_EXIT
        (link_dump(&ctrl->pool, ctrl->size));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_CLI((BSL_META_U(unit, "Retrun fail!\n")));
    }
    SHR_FUNC_EXIT();
}


