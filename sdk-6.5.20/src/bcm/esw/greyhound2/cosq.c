/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/greyhound2.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/ecn.h>
#include <bcm_int/esw/tas.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/greyhound2.h>
#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define GH2_DRR_KBYTES   (1)
#define GH2_DRR_WEIGHT_MAX       0x7f
#define GH2_WRR_WEIGHT_MAX       0x7f

#define GH2_MTU_QUANTA_ID_COUNT 4
#define GH2_COS_WEIGHT_MAX      0x7f

static int MTU_GH2_Quanta[GH2_MTU_QUANTA_ID_COUNT] = {
    2 *  GH2_DRR_KBYTES,     /* Quanta of 2048 bytes */
    4 *  GH2_DRR_KBYTES,     /* Quanta of 4096 bytes */
    8 *  GH2_DRR_KBYTES,     /* Quanta of 8192 bytes */
   16 *  GH2_DRR_KBYTES      /* Quanta of 16384 bytes */
};


/* COS Map profile entries per set */
#define GH2_COS_MAP_ENTRIES_PER_SET    16

#define GH2_BURST_GRANULARITY         64 /* 64bytes */
#define GH2_BURST_GRANULARITY_DEFAULT GH2_BURST_GRANULARITY
#define GH2_BURST_GRANULARITY_128B    128 /* 128 bytes */
#define GH2_BW_FIELD_MAX              0x3FFFF

/* Cache of COS_MAP Profile Table */
STATIC soc_profile_mem_t *gh2_cos_map_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* WRED definition */
/* WRED update interval unit : us */
STATIC int gh2_wred_update_interval[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *gh2_wred_profile[BCM_MAX_NUM_UNITS];

#define GH2_CELL_FIELD_MAX       0x7fff
#define FL_CELL_FIELD_MAX        0x3fff

/* MMU cell size in bytes */
#define GH2_BYTES_PER_CELL 144   /* bytes */
#define GH2_BYTES_TO_CELLS(_byte_)  \
            (((_byte_) + GH2_BYTES_PER_CELL - 1) / GH2_BYTES_PER_CELL)

#define GH2_PORT_LVL_SCHEDULER_PER_PORT 1
#define GH2_QGROUP_LVL_SCHEDULER_PER_PORT 1
#define GH2_QLAYER_LVL_SCHEDULER_PER_PORT 8
/* 8 ports support 2 level scheduling */
#define GH2_NUM_TWO_LEVEL_SCHEDULER_PORT \
            (SOC_GH2_64Q_MMU_PORT_IDX_MAX - SOC_GH2_64Q_MMU_PORT_IDX_MIN + 1)
/* 10 Scheduler node per port */
#define GH2_NUM_SCHEDULERS_PER_PORT \
            (GH2_PORT_LVL_SCHEDULER_PER_PORT + \
             GH2_QGROUP_LVL_SCHEDULER_PER_PORT + \
             GH2_QLAYER_LVL_SCHEDULER_PER_PORT)

/*
 * The first 8 Scheduler nodes are reserved for PORT_LEVEL scheduler of
 * the 8 MMU port .
 * The other 72 Scheduler nodes are for QGROUP and QLAYER schedulers
 * of the MMU ports.
 */
/* 8/80 Schedulres node for PORT_LEVEL */
#define GH2_NUM_PORT_SCHEDULERS 8
/* 8/80 Schedulres node for QGROUP */
#define GH2_NUM_QGROUP_SCHEDULERS \
            (GH2_QGROUP_LVL_SCHEDULER_PER_PORT * \
             GH2_NUM_TWO_LEVEL_SCHEDULER_PORT)
/* 64/80 Schedulres node for QLAYER */
#define GH2_NUM_QLAYER_SCHEDULERS \
            (GH2_QLAYER_LVL_SCHEDULER_PER_PORT * \
             GH2_NUM_TWO_LEVEL_SCHEDULER_PORT)
/* 80 Scheduler node total */
#define GH2_NUM_TOTAL_SCHEDULERS (GH2_NUM_PORT_SCHEDULERS + \
                                  GH2_NUM_QGROUP_SCHEDULERS + \
                                  GH2_NUM_QLAYER_SCHEDULERS)

/* Queue node total */
#define GH2_NUM_TOTAL_QUEUES (GH2_NUM_TWO_LEVEL_SCHEDULER_PORT * \
                              SOC_GH2_QLAYER_COSQ_PER_PORT_NUM)


typedef enum {
    GH2_COSQ_LEVEL_PORT = 0, /* port level */
    GH2_COSQ_LEVEL_QGROUP,   /* QGroup level (L0) */
    GH2_COSQ_LEVEL_QLAYER,   /* QLayer level (L1) */
    GH2_COSQ_LEVEL_QUEUE     /* Queue level (L2)*/
} gh2_cosq_level_type_t;

typedef struct gh2_cosq_node_s {
    int in_use;
    int node_id; /* internal node id */
    bcm_gport_t node_gport; /* node gport id */
    gh2_cosq_level_type_t node_level; /* QGROUP, QLAYER, QUEUE */
    int max_num_child;

    /* parameters for node_level = QGROUP, QLAYER, QUEUE */
    bcm_gport_t parent_gport; /* parent node's gport id */

    /* parameters for node_level = PORT, QGROUP, QLAYER */
    /* number of child, value 0~8 */
    int num_child;
    /* all childrens' gport id */
    bcm_gport_t child_gport[GH2_QLAYER_LVL_SCHEDULER_PER_PORT];

    /* parameters for node_level = QLAYER, QUEUE */
    /* the X-st child of parent node, valid X is 0-7 */
    int child_idx;

    /* QLAYER's queue range's min, value=child_idx * 8 */
    int qlayer_cosq_min;
    /* QLAYER's queue range's max, value=(child_idx+1)*8-1 */
    int qlayer_cosq_max;
} gh2_cosq_node_t;

typedef struct gh2_mmu_info_s {
    /* scheduler nodes */
    gh2_cosq_node_t sched_node[GH2_NUM_TOTAL_SCHEDULERS];
    /* queue nodes */
    gh2_cosq_node_t queue_node[GH2_NUM_TOTAL_QUEUES];
} gh2_mmu_info_t;

/* MMU info of scheduler and queue nodes */
STATIC gh2_mmu_info_t *gh2_mmu_info[BCM_MAX_NUM_UNITS];

/* Number of COSQs configured for this device */
STATIC int gh2_num_cosq[SOC_MAX_NUM_DEVICES];

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      bcmi_gh2_cosq_sync
 * Purpose:
 *      Record COSQ module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;
    gh2_mmu_info_t *mmu_info;

    mmu_info = gh2_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &cosq_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));
    /* Number COSQ */
    num_cosq = gh2_num_cosq[unit];
    sal_memcpy(cosq_scache_ptr, &num_cosq, sizeof(num_cosq));

    /* 2-level scheduling scheduler nodes */
    cosq_scache_ptr += sizeof(num_cosq);
    sal_memcpy(cosq_scache_ptr,
               gh2_mmu_info[unit], sizeof(gh2_mmu_info_t));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_reinit(int unit)
{
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;
    int i, profile_index;
    mmu_wred_config_entry_t qentry;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &cosq_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);

    if (rv == BCM_E_NOT_FOUND) {
        cosq_scache_ptr = NULL;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (cosq_scache_ptr != NULL) {
        /* Number COSQ */
        sal_memcpy(&num_cosq, cosq_scache_ptr, sizeof(num_cosq));
        gh2_num_cosq[unit] = num_cosq;

        cosq_scache_ptr += sizeof(num_cosq);

        /* 2-level scheduling scheduler nodes */
        if (gh2_mmu_info[unit] != NULL) {
            sal_memcpy(gh2_mmu_info[unit], cosq_scache_ptr,
                       sizeof(gh2_mmu_info_t));
        }
    } else {
        gh2_num_cosq[unit] = _bcm_esw_cosq_config_property_get(unit);
    }

    if (soc_feature(unit, soc_feature_discard_ability)) {
        for (i = soc_mem_index_min(unit, MMU_WRED_CONFIGm);
             i <= soc_mem_index_max(unit, MMU_WRED_CONFIGm); i++) {
            BCM_IF_ERROR_RETURN(
                  soc_mem_read(unit, MMU_WRED_CONFIGm,
                               MEM_BLOCK_ALL, i, &qentry));

            profile_index = soc_mem_field32_get(unit, MMU_WRED_CONFIGm,
                                                &qentry, PROFILE_INDEXf);

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, gh2_wred_profile[unit],
                                           profile_index, 1));
        }

        for (i = soc_mem_index_min(unit, MMU_WRED_CONFIG_QGROUPm);
             i <= soc_mem_index_max(unit, MMU_WRED_CONFIG_QGROUPm); i++) {
            BCM_IF_ERROR_RETURN(
                  soc_mem_read(unit, MMU_WRED_CONFIG_QGROUPm,
                               MEM_BLOCK_ALL, i, &qentry));

            profile_index = soc_mem_field32_get(unit, MMU_WRED_CONFIG_QGROUPm,
                                                &qentry, PROFILE_INDEXf);

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, gh2_wred_profile[unit],
                                           profile_index, 1));
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcmi_gh2_cosq_node_t_init
 * Purpose:
 *     Initialize node information
 * Parameters:
 *     node       - (IN) node structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
bcmi_gh2_cosq_node_t_init(gh2_cosq_node_t *node)
{
    int i;

    node->in_use            = FALSE;
    node->node_id           = -1;
    node->node_gport        = -1;
    node->node_level        = -1;
    node->parent_gport      = -1;
    node->max_num_child     = 0;
    node->num_child         = 0;
    node->child_idx         = -1;
    node->qlayer_cosq_min   = -1;
    node->qlayer_cosq_max   = -1;
    for (i = 0; i < GH2_QLAYER_LVL_SCHEDULER_PER_PORT; i++) {
        node->child_gport[i] = BCM_GPORT_INVALID;
    }
}

/*
 * Function:
 *     bcmi_gh2_cosq_node_get
 * Purpose:
 *     Get internal information of scheduler type GPORT
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 *     modid      - (OUT) module identifier
 *     port       - (OUT) port number
 *     id         - (OUT) queue group or scheduler identifier
 *     node       - (OUT) node structure for the specified GPORT
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_node_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_module_t *modid, bcm_port_t *port,
                       int *id, gh2_cosq_node_t **node)
{
    gh2_mmu_info_t *mmu_info;
    gh2_cosq_node_t *node_base = NULL;
    bcm_module_t modid_out = 0;
    bcm_port_t port_out = 0;
    uint32 encap_id = -1;
    int index;

    if ((mmu_info = gh2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
    } else {
        return BCM_E_PORT;
    }

    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }

    if (port != NULL) {
        *port = port_out;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        node_base = mmu_info->queue_node;
    } else { /* scheduler */
        encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x3fff;
        index = encap_id;
        node_base = mmu_info->sched_node;
    }

    if (node_base[index].in_use == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    if (modid != NULL) {
        *modid = modid_out;
    }
    if (id != NULL) {
        *id = encap_id;
    }
    if (node != NULL) {
        *node = &node_base[index];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_index_resolve
 * Purpose:
 *     Find hardware index for the given gport
 * Parameters:
 *     unit      - (IN) unit number
 *     gport     - (IN) GPORT identifier
 *     cosq      - (OUT) result cosq of hw index
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_index_resolve(int unit, bcm_gport_t gport, int *cosq)
{
    gh2_cosq_node_t *node, *parent_node;
    int cosq_base;
    int cosq_offset;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (cosq == NULL) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* HW cosq index is only available Queue node */
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                NULL, NULL, &node));

    if (node != NULL) {
        cosq_offset = node->child_idx;
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, node->parent_gport, 0, NULL,
                                    NULL, NULL, &parent_node));
        cosq_base = parent_node->qlayer_cosq_min;
        *cosq = cosq_base + cosq_offset;
    } else {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_port_resolve
 * Purpose:
 *     Resolve gport and get IDs
 * Parameters:
 *     unit      - (IN) unit number
 *     gport     - (IN) GPORT identifier
 *     modid     - (OUT) module id
 *     port      - (OUT) local port id
 *     trunk_id  - (OUT) trunk id
 *     id        - (OUT) Queue node or scheduler identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                           bcm_port_t *port, bcm_trunk_t *trunk_id, int *id)
{
    gh2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, modid, port, id, &node));
    *trunk_id = -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_localport_resolve
 * Purpose:
 *     Resolve GRPOT if it is a local port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     local_port - (OUT) local port number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_localport_resolve(int unit, bcm_gport_t gport,
                                bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result;

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module,
                                &port, &trunk, &id));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, module, &result));

    if (TRUE != result) {
        return BCM_E_PARAM;
    }

    *local_port = port;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_bandwidth_port_resolve
 * Purpose:
 *     Resolve GRPOT and cosq to get the cosq range for programming
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     cosq       - (IN) cosq id
 *     local_port - (OUT) local port number
 *     cosq_start - (OUT) start index
 *     cosq_end   - (OUT) end index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_gport_bandwidth_port_resolve(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_port_t *local_port,
                                           bcm_cos_queue_t *cosq_start,
                                           bcm_cos_queue_t *cosq_end)
{
    int is_64q;
    gh2_cosq_node_t *node = NULL;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* shaping is supported on scheduler node */
        return BCM_E_PORT;
    }

    /*
     * gport is local port:
     *     cosq range is 0~7
     * gport is GPORT_SCHEDULER QGROUP
     *     cosq(num of qlayer cosq of the qgroup) range is 0~7
     * gport is GPORT_SCHEDULER QLAYER
     *     cosq(num of qlayer cosq of the port) range is 0~7
     */
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                        local_port, NULL, &node));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, gport, local_port));
        }
    } else {
        *local_port = gport;
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
    }

    if (node != NULL) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, *local_port,
                                                   &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, *local_port,
                                                   &is_64q));
        }
        if (!is_64q) {
            return BCM_E_PORT;
        }

        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            if (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                *cosq_start = 0;
                *cosq_end = SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM - 1;
            } else {
                *cosq_start = *cosq_end = cosq;
            }
        } else {
            /* node->node_level is GH2_COSQ_LEVEL_QLAYER */
            if (cosq >= gh2_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                *cosq_start = 0;
                *cosq_end = gh2_num_cosq[unit] - 1;
            } else {
                *cosq_start = *cosq_end = cosq;
            }
        }
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        if (cosq >= gh2_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq < 0) {
            *cosq_start = 0;
            *cosq_end = gh2_num_cosq[unit] - 1;
        } else {
            *cosq_start = *cosq_end = cosq;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_qlayer_scheduler_resolve
 * Purpose:
 *     Find hardware cosq index and local port for the given gport
 *     The function only resolve QLayer scheduler gport type
 * Parameters:
 *     unit      - (IN) unit number
 *     gport     - (IN) GPORT identifier,
 *                 accept QLayer scheduler gport or local port id
 *     cosq      - (IN) cosq id or input id, valid 0~7
 *     out_port  - (OUT) local port id resolved
 *     out_hw_idx- (OUT) cosq of hw index resolved
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_qlayer_scheduler_resolve(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_port_t *out_port, int *out_hw_idx)
{
    bcm_gport_t local_gport;
    int is_64q_port = 0;
    gh2_cosq_node_t *node = NULL;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_qlayer_scheduler_resolve: unit=%d \
                          gport=0x%x cosq=%d\n"),
              unit, gport, cosq));

        /* Get the local port and cosq hw index */
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            /* The gport id is a Scheduler node */
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL, NULL, NULL,
                                        &node));
            /* Only accept QLAYER scheduler in this case */
            if (node->node_level != GH2_COSQ_LEVEL_QLAYER) {
                return BCM_E_PORT;
            }

            /* Get the child gport of the QLAYER scheduler */
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_child_get(unit, gport, cosq,
                                               &local_gport));
            /* Get the hw cosq index (0~63) */
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_index_resolve(unit, local_gport, out_hw_idx));
        } else {
            local_gport = gport;
            /* Get the hw cosq index (0~7) */
            *out_hw_idx = cosq;
        }

        /* Get the local port id */
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, local_gport, out_port));

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, *out_port,
                                                   &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, *out_port,
                                                   &is_64q_port));
        }
        if (is_64q_port) {
            if ((*out_hw_idx < 0) ||
                (*out_hw_idx >= SOC_GH2_2LEVEL_QUEUE_NUM)) {
                return BCM_E_PARAM;
            }
        } else {
            if ((*out_hw_idx < 0) ||
                (*out_hw_idx >= SOC_GH2_LEGACY_QUEUE_NUM)) {
                return BCM_E_PARAM;
            }
        }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_qlayer_scheduler_resolve: unit=%d \
                          out_port=0x%x out_hw_idx=%d\n"),
              unit, *out_port, *out_hw_idx));
    return BCM_E_NONE;
}

STATIC int
bcmi_gh2_cosq_perq_xmt_counter_base_init(int unit)
{
    uint32 regval32 = 0;
    soc_port_t port = 0;
    int base_index = 0, max_index = 0;

    max_index = soc_mem_index_max(unit, EGR_PERQ_XMT_COUNTERSm);

    PBMP_ALL_ITER(unit, port) {
        /* Get the index of MMU_MAX/MIN_BUCKET_QLAYER table by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(
                soc_fl_mmu_bucket_qlayer_index(unit, port, 0, &base_index));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(
                soc_gh2_mmu_bucket_qlayer_index(unit, port, 0, &base_index));
        }

        if (base_index > max_index) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                                  "bcmi_gh2_cosq_perq_xmt_counter_base_init "
                                  "error\n")));
            return BCM_E_RESOURCE;
        }

        BCM_IF_ERROR_RETURN(
            READ_PERQ_XMT_COUNTER_BASEr(unit, port, &regval32));

        soc_reg_field_set(unit, PERQ_XMT_COUNTER_BASEr,
                          &regval32, INDEXf, base_index);

        BCM_IF_ERROR_RETURN(
            WRITE_PERQ_XMT_COUNTER_BASEr(unit, port, regval32));
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_gh2_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_init(int unit)
{
    STATIC int _gh2_max_cosq = -1;
    int num_cos = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint32              cosq_scache_size;
    uint8               *cosq_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_PROFILE_GREENm,
        MMU_WRED_DROP_PROFILE_YELLOWm,
        MMU_WRED_DROP_PROFILE_REDm,
        MMU_WRED_MARK_PROFILE_GREENm,
        MMU_WRED_MARK_PROFILE_YELLOWm,
        MMU_WRED_MARK_PROFILE_REDm
    };
    uint32 interval;
    int entry_words[6];
    gh2_mmu_info_t *mmu_info;
    int alloc_size;
    int i;

    if (_gh2_max_cosq < 0) {
        _gh2_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_detach(unit, 0));
        if (soc_feature(unit, soc_feature_discard_ability)) {
            BCM_IF_ERROR_RETURN(
                soc_mem_clear(unit, MMU_WRED_CONFIGm, COPYNO_ALL, TRUE));
            BCM_IF_ERROR_RETURN(
                soc_mem_clear(unit, MMU_WRED_CONFIG_QGROUPm, COPYNO_ALL, TRUE));
        }
    }

    num_cos = _bcm_esw_cosq_config_property_get(unit);

    alloc_size = sizeof(gh2_mmu_info_t) * 1;
    if (gh2_mmu_info[unit] == NULL) {
        gh2_mmu_info[unit] =
            sal_alloc(alloc_size, "gh2_mmu_info");

        if (gh2_mmu_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(gh2_mmu_info[unit], 0, alloc_size);
    }

    mmu_info = gh2_mmu_info[unit];

    for (i = 0; i < GH2_NUM_TOTAL_SCHEDULERS; i++) {
        bcmi_gh2_cosq_node_t_init(&mmu_info->sched_node[i]);
    }

    for (i = 0; i < GH2_NUM_TOTAL_QUEUES; i++) {
        bcmi_gh2_cosq_node_t_init(&mmu_info->queue_node[i]);
    }

    if (soc_feature(unit, soc_feature_discard_ability)) {
        /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
        if (gh2_wred_profile[unit] == NULL) {
            gh2_wred_profile[unit] =
                sal_alloc(sizeof(soc_profile_mem_t), "WRED Profile Mem");
            if (gh2_wred_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(gh2_wred_profile[unit]);
        } else {
            soc_profile_mem_destroy(unit, gh2_wred_profile[unit]);
        }

        entry_words[0] =
            sizeof(mmu_wred_drop_profile_green_entry_t) / sizeof(uint32);
        entry_words[1] =
            sizeof(mmu_wred_drop_profile_yellow_entry_t) / sizeof(uint32);
        entry_words[2] =
            sizeof(mmu_wred_drop_profile_red_entry_t) / sizeof(uint32);
        entry_words[3] =
            sizeof(mmu_wred_mark_profile_green_entry_t) / sizeof(uint32);
        entry_words[4] =
            sizeof(mmu_wred_mark_profile_yellow_entry_t) / sizeof(uint32);
        entry_words[5] =
            sizeof(mmu_wred_mark_profile_red_entry_t) / sizeof(uint32);
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_create(unit, wred_mems, entry_words, 6,
                                    gh2_wred_profile[unit]));

        /*
         * WRED update inetrval
         * = 1040entries * core_clk_cycle_time * 1.125(sbus overhead),
         * = 1170 * core_clk_cycle_time
         * = 1170 / core_frequency
         *
         * GH2:
         * 200M : interval=5
         * 300M : interval=3
         * 400M : interval=2
         * 500M : interval=2
         * 600M : interval=1
         */
        interval =  1170 / SOC_INFO(unit).frequency;
        BCM_IF_ERROR_RETURN(
            soc_reg_field32_modify(unit, TIME_DOMAINr,
                                   REG_PORT_ANY, WRED_UPDATE_INTERVALf,
                                   interval));
        gh2_wred_update_interval[unit] = interval + 1;
    }

    /* Need to configure the base id within EGR_PERQ_XMT_COUNTERSm per port */
    BCM_IF_ERROR_RETURN(
        bcmi_gh2_cosq_perq_xmt_counter_base_init(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_reinit(unit));
        num_cos = gh2_num_cosq[unit];
    } else {
        /* Warm boot level 2 cache size */
        cosq_scache_size = sizeof(uint32); /* Number COSQ */
        cosq_scache_size += sizeof(gh2_mmu_info_t);

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                    (0 == SOC_WARM_BOOT(unit)),
                                    cosq_scache_size, &cosq_scache_ptr,
                                    BCM_WB_DEFAULT_VERSION, NULL);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return bcmi_gh2_cosq_config_set(unit, num_cos);
}

/*
 * Function:
 *      bcmi_gh2_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_detach(int unit, int software_state_only)
{
    bcm_port_t port;
    bcm_pbmp_t port_all;
    int weights[8];
    int cosq;
    int is_64q = 0;
    uint32 reg_val;

    if (gh2_mmu_info[unit]) {
        sal_free(gh2_mmu_info[unit]);
        gh2_mmu_info[unit] = NULL;
    }

    for (cosq = 0; cosq < 8; cosq++) {
        weights[cosq] = 0;
    }

    if (!software_state_only) {
        /* clear shaper tables */
        BCM_IF_ERROR_RETURN(
            soc_mem_clear(unit, MMU_MAX_BUCKET_QLAYERm, MEM_BLOCK_ALL, 0));
        BCM_IF_ERROR_RETURN(
            soc_mem_clear(unit, MMU_MIN_BUCKET_QLAYERm, MEM_BLOCK_ALL, 0));
        BCM_IF_ERROR_RETURN(
            soc_mem_clear(unit, MMU_MAX_BUCKET_QGROUPm, MEM_BLOCK_ALL, 0));
        BCM_IF_ERROR_RETURN(
            soc_mem_clear(unit, MMU_MIN_BUCKET_QGROUPm, MEM_BLOCK_ALL, 0));

        /* disable 2-level scheduling register */
        PBMP_ALL_ITER(unit, port) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                       &is_64q));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                       &is_64q));
            }
            if (is_64q) {
                READ_TWO_LAYER_SCH_MODEr(unit, port, &reg_val);
                soc_reg_field_set(unit, TWO_LAYER_SCH_MODEr, &reg_val,
                                  SCH_MODEf,  0);
                WRITE_TWO_LAYER_SCH_MODEr(unit, port, reg_val);
            }
        }
        /* Clear scheduling settings on port */
        port_all = PBMP_ALL(unit);
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_sched_set(unit, port_all,
                                          BCM_COSQ_WEIGHTED_ROUND_ROBIN,
                                          weights, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - Unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_config_set(int unit, int numq)
{
    int cos, prio, ratio, remain;
    uint32 index;
    soc_mem_t mem = PORT_COS_MAPm;
    int entry_words = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    bcm_pbmp_t ports;
    bcm_port_t port;
    port_cos_map_entry_t cos_map_array[GH2_COS_MAP_ENTRIES_PER_SET];
    void *entries[GH2_COS_MAP_ENTRIES_PER_SET];
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_PROFILE_GREENm,
        MMU_WRED_DROP_PROFILE_YELLOWm,
        MMU_WRED_DROP_PROFILE_REDm,
        MMU_WRED_MARK_PROFILE_GREENm,
        MMU_WRED_MARK_PROFILE_YELLOWm,
        MMU_WRED_MARK_PROFILE_REDm
    };
    int ii, wred_prof_count;
    uint32 profile_index;
    mmu_wred_drop_profile_green_entry_t entry_tcp_green;
    mmu_wred_drop_profile_yellow_entry_t entry_tcp_yellow;
    mmu_wred_drop_profile_red_entry_t entry_tcp_red;
    mmu_wred_mark_profile_green_entry_t entry_mark_green;
    mmu_wred_mark_profile_yellow_entry_t entry_mark_yellow;
    mmu_wred_mark_profile_red_entry_t entry_mark_red;
    void *wred_entries[6];
    int cell_field_max;

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    /* Map the eight 802.1 priority levels to the active cosqs */
    if (numq > 8) {
        numq = 8;
    }

    sal_memset(cos_map_array, 0,
               GH2_COS_MAP_ENTRIES_PER_SET * sizeof(port_cos_map_entry_t));

    if (gh2_cos_map_profile[unit] == NULL) {
        gh2_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                              "COS_MAP Profile Mem");
        if (gh2_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(gh2_cos_map_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               gh2_cos_map_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {    /* Warm Boot */
        int     i;
        uint32  val;

        /* Gather reference count for cosq map profile table */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
            index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
            index *= GH2_COS_MAP_ENTRIES_PER_SET;
            for (i = 0; i < GH2_COS_MAP_ENTRIES_PER_SET; i++) {
                SOC_PROFILE_MEM_REFERENCE(unit, gh2_cos_map_profile[unit],
                                          index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit,
                                                gh2_cos_map_profile[unit],
                                                index + i,
                                                GH2_COS_MAP_ENTRIES_PER_SET);
            }
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
            if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
                SOC_IF_ERROR_RETURN(READ_ICOS_MAP_SELr(unit, port, &val));
                index = soc_reg_field_get(unit, ICOS_MAP_SELr, val, SELECTf);
                index *= GH2_COS_MAP_ENTRIES_PER_SET;
                for (i = 0; i < GH2_COS_MAP_ENTRIES_PER_SET; i++) {
                    SOC_PROFILE_MEM_REFERENCE(unit, gh2_cos_map_profile[unit],
                                              index + i, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(
                        unit,
                        gh2_cos_map_profile[unit],
                        index + i,
                        GH2_COS_MAP_ENTRIES_PER_SET);
                }
            }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
        }

        gh2_num_cosq[unit] = numq;

        return BCM_E_NONE;
    }

    /* Cold Boot */
    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit, PORT_COS_MAPm, HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }

        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
        entries[prio] = (void *) &cos_map_array[prio];
    }

    /* Map remaining internal priority levels to highest priority cosq */
    cos = numq - 1;
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit, PORT_COS_MAPm, HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }
        entries[prio] = (void *) &cos_map_array[prio];
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

    /* Add a profile mem entry for each port */
    BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, gh2_cos_map_profile[unit],
                                 (void *) &entries,
                                 GH2_COS_MAP_ENTRIES_PER_SET, &index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, port,
                                    SELECTf,
                                    (index / GH2_COS_MAP_ENTRIES_PER_SET)));
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, gh2_cos_map_profile[unit],
                                     (void *) &entries,
                                     GH2_COS_MAP_ENTRIES_PER_SET, &index));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, port,
                                        SELECTf,
                                        (index / GH2_COS_MAP_ENTRIES_PER_SET)));
        }
#endif
    }

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* identity mapping for higig ports */

    /* map prio0->cos0, prio1->cos1, ... , prio7->cos7 */
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, prio);
        if (soc_mem_field_valid(unit, PORT_COS_MAPm, HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }
    }
    /* Map remaining internal priority levels to highest priority cosq */
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, 7);
        if (soc_mem_field_valid(unit, PORT_COS_MAPm, HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }
    }
    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, gh2_cos_map_profile[unit],
                                     (void *) &entries,
                                     GH2_COS_MAP_ENTRIES_PER_SET, &index));
            soc_reg_field32_modify(unit, ICOS_MAP_SELr, port,
                                   SELECTf,
                                   (index / GH2_COS_MAP_ENTRIES_PER_SET));
        }
    }
#endif

    gh2_num_cosq[unit] = numq;

    if (soc_feature(unit, soc_feature_fl)) {
        cell_field_max = FL_CELL_FIELD_MAX;
    } else {
        cell_field_max = GH2_CELL_FIELD_MAX;
    }

    if (soc_feature(unit, soc_feature_discard_ability)) {
      /* Add default entries for MMU_WRED_DROP_CURVE_PROFILE_x memory profile */
        sal_memset(&entry_tcp_green, 0, sizeof(entry_tcp_green));
        sal_memset(&entry_tcp_yellow, 0, sizeof(entry_tcp_yellow));
        sal_memset(&entry_tcp_red, 0, sizeof(entry_tcp_red));
        sal_memset(&entry_mark_green, 0, sizeof(entry_mark_green));
        sal_memset(&entry_mark_yellow, 0, sizeof(entry_mark_yellow));
        sal_memset(&entry_mark_red, 0, sizeof(entry_mark_red));
        wred_entries[0] = &entry_tcp_green;
        wred_entries[1] = &entry_tcp_yellow;
        wred_entries[2] = &entry_tcp_red;
        wred_entries[3] = &entry_mark_green;
        wred_entries[4] = &entry_mark_yellow;
        wred_entries[5] = &entry_mark_red;
        for (ii = 0; ii < 6; ii++) {
            soc_mem_field32_set(unit, wred_mems[ii], wred_entries[ii],
                                MIN_THDf, cell_field_max);
            soc_mem_field32_set(unit, wred_mems[ii], wred_entries[ii],
                                MAX_THDf, cell_field_max);
        }
        profile_index = 0xffffffff;
        wred_prof_count = soc_mem_index_count(unit, MMU_WRED_CONFIGm);
        while (wred_prof_count) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit,
                                         gh2_wred_profile[unit],
                                         wred_entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                               gh2_wred_profile[unit],
                                               profile_index, 0));
            }
            wred_prof_count -= 1;
        }
        profile_index = 0xffffffff;
        wred_prof_count =
            soc_mem_index_count(unit, MMU_WRED_CONFIG_QGROUPm);
        while (wred_prof_count) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit,
                                         gh2_wred_profile[unit],
                                         wred_entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                               gh2_wred_profile[unit],
                                               profile_index, 0));
            }
            wred_prof_count -= 1;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_esw_ecn_init(unit));

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_config_get(int unit, int *numq)
{
    if (gh2_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = gh2_num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_bandwidth_set
 * Purpose:
 *      Set bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port to configure
 *      cosq - COS queue to configure
 *      kbits_sec_min - minimum bandwidth, kbits/sec.
 *      kbits_sec_max - maximum bandwidth, kbits/sec.
 *      flags
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_port_bandwidth_set(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 uint32 kbits_sec_min,
                                 uint32 kbits_sec_max,
                                 uint32 kbits_sec_burst,
                                 uint32 flags)
{
    uint32 regval, rval = 0;
    uint32 burst_size = 0;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
    int    refresh_bitsize, bucket_bitsize;
    uint32 fval = 0;
    uint32 eav_gran = GH2_BURST_GRANULARITY_DEFAULT;
    int memidx;
    soc_mem_t config_mem = INVALIDm;
    soc_reg_t config_reg = INVALIDr;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int is_64q;
    int shift;
    bcm_port_t local_port;
    gh2_cosq_node_t *node = NULL;
    int local_cosq;
    soc_mem_t max_bucket_mem = INVALIDm;
    soc_mem_t min_bucket_mem = INVALIDm;
    soc_reg_t min_gran_reg = INVALIDr;
    soc_reg_t max_gran_reg = INVALIDr;
    int gran_shift = 0;
    soc_reg_t eav_bucket_reg = INVALIDr;
#ifdef BCM_FIRELIGHT_SUPPORT
    soc_reg_t min_refresh_unit_reg = INVALIDr;
    soc_reg_t max_refresh_unit_reg = INVALIDr;
    int min_greater_than_50g = 0;
    int max_greater_than_50g = 0;
#endif /* BCM_FIRELIGHT_SUPPORT */

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* shaping is supported on scheduler node */
        return BCM_E_PORT;
    }

    /*
     * Parameter gport cases
     * gport is local port:
     *     parameter cosq valid : 0~7
     * gport is GPORT_SCHEDULER QGROUP
     *     parameter cosq valid : 0~7
     * gport is GPORT_SCHEDULER QLAYER
     *     parameter cosq valid : 0~7
     */
    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                   &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                   &is_64q));
        }
        if (!is_64q) {
            return BCM_E_PORT;
        }
        if (node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* Shaper is not available on this level */
            return BCM_E_UNAVAIL;
        } else if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            max_bucket_mem = MMU_MAX_BUCKET_QGROUPm;
            min_bucket_mem = MMU_MIN_BUCKET_QGROUPm;
            min_gran_reg = SHAPING_Q_GRAN_QGROUPr;
            max_gran_reg = SHAPING_Q_GRAN_QGROUPr;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QGROUPr;
                max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QGROUPr;
            }
#endif /* BCM_FIRELIGHT_SUPPORT */
            gran_shift = cosq;
            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QGROUPr;
        } else {
            max_bucket_mem = MMU_MAX_BUCKET_QLAYERm;
            min_bucket_mem = MMU_MIN_BUCKET_QLAYERm;

            /* transfered to hw queue index (0~63) */
            local_cosq = node->qlayer_cosq_min + cosq;
            if (local_cosq < 32) {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN0r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX0r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN0r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX0r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = local_cosq;
            } else {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN1r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX1r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN1r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX1r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = local_cosq - 32;
            }

            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QLAYERr;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, gport, &local_port));

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q));
        }
        max_bucket_mem = MMU_MAX_BUCKET_QLAYERm;
        min_bucket_mem = MMU_MIN_BUCKET_QLAYERm;

        if (is_64q) {
            if (cosq < 32) {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN0r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX0r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN0r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX0r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = cosq;
            } else {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN1r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX1r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN1r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX1r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = cosq - 32;
            }
            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QLAYERr;
        } else {
            min_gran_reg = SHAPING_Q_GRANr;
            max_gran_reg = SHAPING_Q_GRANr;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                min_refresh_unit_reg = SHAPING_Q_REFRESH_UNITr;
                max_refresh_unit_reg = SHAPING_Q_REFRESH_UNITr;
            }
#endif /* BCM_FIRELIGHT_SUPPORT */
            gran_shift = cosq;
            eav_bucket_reg = EAVBUCKETCONFIG_EXTr;
        }
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET, MINBUCKET
     * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    if (node) {
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            /*
             * Get the hw index of MMU_MAX/MIN_BUCKET_QGROUP table by
             * (port, cosq)
             */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(
                    soc_fl_mmu_bucket_qgroup_index(unit,
                                                    local_port,
                                                    cosq,
                                                    &memidx));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    soc_gh2_mmu_bucket_qgroup_index(unit,
                                                    local_port,
                                                    cosq,
                                                    &memidx));
            }
        } else {
            /* node->node_level is GH2_COSQ_LEVEL_QLAYER */
            /*
             * Get the hw index of MMU_MAX/MIN_BUCKET_QLAYER table by
             * (port, cosq)
             */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(
                    soc_fl_mmu_bucket_qlayer_index(unit,
                                                local_port,
                                                node->qlayer_cosq_min + cosq,
                                                &memidx));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    soc_gh2_mmu_bucket_qlayer_index(unit,
                                                local_port,
                                                node->qlayer_cosq_min + cosq,
                                                &memidx));
            }
        }
    } else {
        /* Get the index of MMU_MAX/MIN_BUCKET_QLAYER table by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                (unit, local_port, cosq, &memidx));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                (unit, local_port, cosq, &memidx));
        }
    }

    /* Disable egress metering for this port */
    config_mem = max_bucket_mem; /* MMU_MAX_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, 0);
    soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));

    config_mem = min_bucket_mem; /* MMU_MIN_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, 0);
    soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));

    /* reset the MAXBUCKET register */
    config_mem = max_bucket_mem; /* MMU_MAX_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    soc_mem_field32_set(unit, config_mem, &entry, BUCKETf, 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));

    /* reset the MINBUCKET register value */
    config_mem = min_bucket_mem; /* MMU_MIN_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    soc_mem_field32_set(unit, config_mem, &entry, BUCKETf, 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));


    refresh_bitsize =
        soc_mem_field_length(unit, MMU_MIN_BUCKET_QLAYERm, REFRESHf);
    bucket_bitsize =
        soc_mem_field_length(unit, MMU_MIN_BUCKET_QLAYERm, THD_SELf);

    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY_SELECTIVE;
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY_SELECT3;
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY_SELECT5;
    meter_flags |= _BCM_XGS_METER_FLAG_BUCKET_IN_8KB;

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit) && (kbits_sec_min > 50000000)) {
        kbits_sec_min = kbits_sec_min / 2;
        min_greater_than_50g = 1;
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min, kbits_sec_min,
                                           meter_flags, refresh_bitsize,
                                           bucket_bitsize,
                                           &refresh_rate, &bucketsize,
                                           &granularity));
    config_reg = min_gran_reg;
    shift = gran_shift;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
    fval = soc_reg_field_get(unit, config_reg,
                             regval, MIN_Q_GRANf);
    if (granularity == 5) {
        fval |= (1 << shift);
    } else {
        fval &= ~(1 << shift);
    }
    soc_reg_field_set(unit, config_reg, &regval,
                      MIN_Q_GRANf, fval);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, config_reg, local_port, 0, regval));

#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            config_reg = min_refresh_unit_reg;
            shift = gran_shift;
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
            fval = soc_reg_field_get(unit, config_reg,
                                     regval, MIN_Q_REFRESH_UNITf);
            if (min_greater_than_50g) {
                fval |= (1 << shift);
            } else {
                fval &= ~(1 << shift);
            }
            soc_reg_field_set(unit, config_reg, &regval,
                              MIN_Q_REFRESH_UNITf, fval);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, config_reg, local_port, 0, regval));
        }
#endif

    config_mem = min_bucket_mem; /* MMU_MIN_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, refresh_rate);
    soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, bucketsize);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));


    refresh_bitsize =
        soc_mem_field_length(unit, MMU_MAX_BUCKET_QLAYERm, REFRESHf);
    bucket_bitsize =
        soc_mem_field_length(unit, MMU_MAX_BUCKET_QLAYERm, THD_SELf);

#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit) && (kbits_sec_max > 50000000)) {
            kbits_sec_max = kbits_sec_max / 2;
            max_greater_than_50g = 1;
        }
#endif /* BCM_FIRELIGHT_SUPPORT */

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max, kbits_sec_burst,
                                           meter_flags, refresh_bitsize,
                                           bucket_bitsize,
                                           &refresh_rate, &bucketsize,
                                           &granularity));
    config_reg = max_gran_reg;
    shift = gran_shift;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
    fval = soc_reg_field_get(unit, config_reg,
                             regval, MAX_Q_GRANf);
    if (granularity == 5) {
        fval |= (1 << shift);
    } else {
        fval &= ~(1 << shift);
    }
    soc_reg_field_set(unit, config_reg, &regval,
                      MAX_Q_GRANf, fval);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, config_reg, local_port, 0, regval));

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        config_reg = max_refresh_unit_reg;
        shift = gran_shift;
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
        fval = soc_reg_field_get(unit, config_reg,
                                 regval, MAX_Q_REFRESH_UNITf);
        if (max_greater_than_50g) {
            fval |= (1 << shift);
        } else {
            fval &= ~(1 << shift);
        }
        soc_reg_field_set(unit, config_reg, &regval,
                          MAX_Q_REFRESH_UNITf, fval);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, config_reg, local_port, 0, regval));
    }
#endif

    config_mem = max_bucket_mem; /* MMU_MAX_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));

    soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, refresh_rate);
    if ((flags & (BCM_COSQ_BW_EAV_MODE | BCM_COSQ_BW_EAV_TAS_MODE)) ==
        (BCM_COSQ_BW_EAV_MODE | BCM_COSQ_BW_EAV_TAS_MODE)) {
        return BCM_E_CONFIG;
    }
    if ((flags & BCM_COSQ_BW_EAV_MODE) ||
        (flags & BCM_COSQ_BW_EAV_TAS_MODE)) {
        if (granularity == 5) {
            eav_gran = GH2_BURST_GRANULARITY_128B;
        } else {
            eav_gran = GH2_BURST_GRANULARITY_DEFAULT;
        }
        burst_size = (((kbits_sec_burst * 1000) / 8) +
                         (eav_gran - 1)) / eav_gran;
        if (burst_size > GH2_BW_FIELD_MAX) {
            burst_size = GH2_BW_FIELD_MAX;
        }

        if (flags & BCM_COSQ_BW_EAV_MODE) {
        soc_mem_field32_set(unit, config_mem, &entry, EAV_MODEf, 1);
        } else {
            soc_mem_field32_set(unit, config_mem, &entry, EAV_MODEf, 2);
        }
        config_reg = eav_bucket_reg;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg,
                                          local_port, cosq, &rval));
        soc_reg_field_set(unit, config_reg, &rval, EAV_THD_SEL_6LSBf,
                          burst_size & 0x3f);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, config_reg,
                                          local_port, cosq, rval));

        soc_mem_field32_set(unit, config_mem, &entry, THD_SELf,
                            (burst_size >> 6 & 0xFFF));
    } else {
        soc_mem_field32_set(unit, config_mem, &entry, EAV_MODEf, 0);
        soc_mem_field32_set(unit, config_mem, &entry, THD_SELf,
                            bucketsize);
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));

    config_reg = eav_bucket_reg;
    if (SOC_REG_FIELD_VALID(unit, config_reg, GATE_CLOSE_FREEZE_MODEf)) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg,
                                          local_port, cosq, &rval));
        if (flags & BCM_COSQ_BW_TAS_TOKEN_FREEZE) {
            soc_reg_field_set(unit, config_reg, &rval, GATE_CLOSE_FREEZE_MODEf,
                              1);
        } else {
            soc_reg_field_set(unit, config_reg, &rval, GATE_CLOSE_FREEZE_MODEf,
                              0);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, config_reg,
                                          local_port, cosq, rval));
    }

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_bandwidth_get
 * Purpose:
 *      Retrieve bandwidth values for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - port to configure
 *      cosq - COS queue to configure
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_port_bandwidth_get(int unit, bcm_gport_t gport,
                                 bcm_cos_queue_t cosq,
                                 uint32 *kbits_sec_min,
                                 uint32 *kbits_sec_max,
                                 uint32 *kbits_sec_burst,
                                 uint32 *flags)
{
    uint32 regval;
    uint32 refresh_rate = 0, bucketsize = 0,
           granularity = 3, meter_flags = 0;
    uint32 kbits_min_burst; /* Placeholder, since burst is unused for min */
    uint32 rval = 0;
    uint32 bucket_lo = 0;
    uint32 eav_mode = 0;
    uint32 fval = 0;
    uint32 eav_gran = GH2_BURST_GRANULARITY_DEFAULT;
    int memidx;
    int is_64q;
    int shift;
    soc_mem_t config_mem = INVALIDm;
    soc_reg_t config_reg = INVALIDr;
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_port_t local_port;
    gh2_cosq_node_t *node = NULL;
    int local_cosq;
    soc_mem_t max_bucket_mem = INVALIDm;
    soc_mem_t min_bucket_mem = INVALIDm;
    soc_reg_t min_gran_reg = INVALIDr;
    soc_reg_t max_gran_reg = INVALIDr;
    int gran_shift = 0;
    soc_reg_t eav_bucket_reg = INVALIDr;
#ifdef BCM_FIRELIGHT_SUPPORT
    soc_reg_t min_refresh_unit_reg = INVALIDr;
    soc_reg_t max_refresh_unit_reg = INVALIDr;
#endif /* BCM_FIRELIGHT_SUPPORT */

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                   &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                   &is_64q));
        }
        if (!is_64q) {
            return BCM_E_PORT;
        }
        if (node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* Shaper is not available on this level */
            return BCM_E_UNAVAIL;
        } else if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            max_bucket_mem = MMU_MAX_BUCKET_QGROUPm;
            min_bucket_mem = MMU_MIN_BUCKET_QGROUPm;
            min_gran_reg = SHAPING_Q_GRAN_QGROUPr;
            max_gran_reg = SHAPING_Q_GRAN_QGROUPr;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QGROUPr;
                max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QGROUPr;
            }
#endif /* BCM_FIRELIGHT_SUPPORT */
            gran_shift = cosq;
            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QGROUPr;
        } else {
            max_bucket_mem = MMU_MAX_BUCKET_QLAYERm;
            min_bucket_mem = MMU_MIN_BUCKET_QLAYERm;

            /* transfered to hw queue index (0~63) */
            local_cosq = node->qlayer_cosq_min + cosq;
            if (local_cosq < 32) {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN0r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX0r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN0r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX0r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = local_cosq;
            } else {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN1r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX1r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN1r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX1r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = local_cosq - 32;
            }
            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QLAYERr;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, gport, &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q));
        }
        max_bucket_mem = MMU_MAX_BUCKET_QLAYERm;
        min_bucket_mem = MMU_MIN_BUCKET_QLAYERm;
        if (is_64q) {
            if (cosq < 32) {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN0r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX0r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN0r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX0r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = cosq;
            } else {
                min_gran_reg = SHAPING_Q_GRAN_QLAYER_MIN1r;
                max_gran_reg = SHAPING_Q_GRAN_QLAYER_MAX1r;
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit)) {
                    min_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MIN1r;
                    max_refresh_unit_reg = SHAPING_Q_REFRESH_UNIT_QLAYER_MAX1r;
                }
#endif /* BCM_FIRELIGHT_SUPPORT */
                gran_shift = cosq - 32;
            }
            eav_bucket_reg = EAVBUCKETCONFIG_EXT_QLAYERr;
        } else {
            min_gran_reg = SHAPING_Q_GRANr;
            max_gran_reg = SHAPING_Q_GRANr;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (SOC_IS_FIRELIGHT(unit)) {
                min_refresh_unit_reg = SHAPING_Q_REFRESH_UNITr;
                max_refresh_unit_reg = SHAPING_Q_REFRESH_UNITr;
            }
#endif /* BCM_FIRELIGHT_SUPPORT */
            gran_shift = cosq;
            eav_bucket_reg = EAVBUCKETCONFIG_EXTr;
        }
    }

    if (node) {
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            /*
             * Get the hw index of MMU_MAX/MIN_BUCKET_QGROUP table by
             * (port, cosq)
             */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(
                    soc_fl_mmu_bucket_qgroup_index(unit,
                                                    local_port,
                                                    cosq,
                                                    &memidx));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    soc_gh2_mmu_bucket_qgroup_index(unit,
                                                    local_port,
                                                    cosq,
                                                    &memidx));
            }
        } else {
            /* node->node_level is GH2_COSQ_LEVEL_QLAYER */
            /*
             * Get the hw index of MMU_MAX/MIN_BUCKET_QLAYER table by
             * (port, cosq)
             */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(
                    soc_fl_mmu_bucket_qlayer_index(unit,
                                                local_port,
                                                node->qlayer_cosq_min + cosq,
                                                &memidx));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(
                    soc_gh2_mmu_bucket_qlayer_index(unit,
                                                local_port,
                                                node->qlayer_cosq_min + cosq,
                                                &memidx));
            }
        }
    } else {
        /* Get the index of MMU_MAX/MIN_BUCKET_QLAYER table by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                (unit, local_port, cosq, &memidx));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                (unit, local_port, cosq, &memidx));
        }
    }

    if (!kbits_sec_min || !kbits_sec_max || !kbits_sec_burst || !flags) {
        return (BCM_E_PARAM);
    }

    *flags = 0;

    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;
    granularity = 3; /* reset to default */

    config_reg = max_gran_reg;
    shift = gran_shift;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
    if (soc_reg_field_valid(unit, config_reg, MAX_Q_GRANf)) {
       fval = soc_reg_field_get(unit, config_reg,
                                regval, MAX_Q_GRANf);
       if (fval & (1 << shift)) {
           granularity = 5;
       }
    }

    config_mem = max_bucket_mem; /* MMU_MAX_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    refresh_rate = soc_mem_field32_get(unit, config_mem,
                                       &entry, REFRESHf);
    bucketsize = soc_mem_field32_get(unit, config_mem,
                                     &entry, THD_SELf);
    eav_mode = soc_mem_field32_get(unit, config_mem,
                                   &entry, EAV_MODEf);
    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           kbits_sec_max, kbits_sec_burst));

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        config_reg = max_refresh_unit_reg;
        shift = gran_shift;

        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
        if (soc_reg_field_valid(unit, config_reg, MAX_Q_REFRESH_UNITf)) {
            fval = soc_reg_field_get(unit, config_reg,
                                     regval, MAX_Q_REFRESH_UNITf);
            if (fval & (1 << shift)) {
                *kbits_sec_max = (*kbits_sec_max) * 2;
            }
        }
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    if ((eav_mode == 1) || (eav_mode == 2)) {
        *flags |= (eav_mode == 1) ? BCM_COSQ_BW_EAV_MODE :
                  BCM_COSQ_BW_EAV_TAS_MODE;

        config_reg = eav_bucket_reg;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg,
                                          local_port, cosq, &rval));
        bucket_lo = soc_reg_field_get(unit, config_reg,
                                      rval, EAV_THD_SEL_6LSBf);
        if (granularity == 5) {
            eav_gran = GH2_BURST_GRANULARITY_128B;
        } else {
            eav_gran = GH2_BURST_GRANULARITY_DEFAULT;
        }
        *kbits_sec_burst = (((bucketsize << 6) | bucket_lo) *
                               eav_gran * 8) / 1000;
    }
    config_reg = eav_bucket_reg;
    if (SOC_REG_FIELD_VALID(unit, config_reg, GATE_CLOSE_FREEZE_MODEf)) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg,
                                          local_port, cosq, &rval));
        if (soc_reg_field_get(unit, config_reg,
                              rval, GATE_CLOSE_FREEZE_MODEf)) {
            *flags |= BCM_COSQ_BW_TAS_TOKEN_FREEZE;
        }
    }

    config_mem = min_bucket_mem; /* MMU_MIN_BUCKET_QLAYERm; */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, memidx, &entry));
    refresh_rate = soc_mem_field32_get(unit, config_mem,
                                       &entry, REFRESHf);

    meter_flags = 0; /* reset to default */
    granularity = 3; /* reset to default */
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    config_reg = min_gran_reg;
    shift = gran_shift;

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
    if (soc_reg_field_valid(unit, config_reg, MIN_Q_GRANf)) {
        fval = soc_reg_field_get(unit, config_reg,
                                 regval, MIN_Q_GRANf);
        if (fval & (1 << shift)) {
            granularity = 5;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, 0,
                                           granularity, meter_flags,
                                           kbits_sec_min, &kbits_min_burst));

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        config_reg = min_refresh_unit_reg;
        shift = gran_shift;

        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, config_reg, local_port, 0, &regval));
        if (soc_reg_field_valid(unit, config_reg, MIN_Q_REFRESH_UNITf)) {
            fval = soc_reg_field_get(unit, config_reg,
                                     regval, MIN_Q_REFRESH_UNITf);
            if (fval & (1 << shift)) {
                *kbits_sec_min = (*kbits_sec_min) * 2;
            }
        }
    }
#endif /* BCM_FIRELIGHT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_sched_weight_max_get
 * Purpose:
 *      Retrieve maximum weights for given COS policy.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weight_max - (output) Maximum weight for COS queue.
 *              For DRR mode Weight is specified in Kbytes
 *              0 if mode is BCM_COSQ_STRICT.
 *              1 if mode is BCM_COSQ_ROUND_ROBIN.
 *              -1 if not applicable to mode.
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        *weight_max = GH2_WRR_WEIGHT_MAX;
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        /* max weight in kb will be
        max weight * max value of MTU quanta sel */
        *weight_max = GH2_DRR_WEIGHT_MAX * \
                      MTU_GH2_Quanta[GH2_MTU_QUANTA_ID_COUNT - 1];
        break;
    default:
        *weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_gh2_cos_max_weight
 * Purpose:
 *      Get the max weight from the input weights.
 * Parameters:
 *      weight - array of weight value.
 * Returns:
 *      BCM_E_XXX
 */
static int
bcmi_gh2_cos_max_weight(const int weight[])
{
    int i, max_weight = 0;

    /* find max weight */
    for (i = 0; i < 8; i++) {
        if (max_weight < weight[i]) {
            max_weight = weight[i];
        }
    }
    return max_weight;
}

/*
 * Calculates the Quanta needed for COS according to its weight
 */
static int
bcmi_gh2_cos_drr_weights_to_quanta(const int weight[])
{
    int i, max_weight = 0;

    max_weight = bcmi_gh2_cos_max_weight(weight);

    for (i = 0; i < GH2_MTU_QUANTA_ID_COUNT; i++) {
        if (max_weight <= MTU_GH2_Quanta[i] * GH2_COS_WEIGHT_MAX) {
            return i;   /* Right Quanta was found */
        }
    }
    return -1;  /* In case of too big weight return negative value */
}

/*
 * Convert the encoded weights to number of kbytes that can transmtted
 * in one run.
 */
static int
bcmi_gh2_mtu_cos_drr_weight_to_kbytes(int weight, int MTUQuantaSel)
{
    return (weight * MTU_GH2_Quanta[MTUQuantaSel]);
}

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding
 */
static int
bcmi_gh2_mtu_cos_drr_kbytes_to_weight(int kbytes, int MTUQuantaSel)
{
    int cos_weight = 0;
    int weight_found = FALSE, low_weight_boundary, hi_weight_boundary;

    /* Search for right weight */
    low_weight_boundary = 1;
    hi_weight_boundary = GH2_COS_WEIGHT_MAX;

    /* Boundary conditions */
    if (kbytes >= hi_weight_boundary * MTU_GH2_Quanta[MTUQuantaSel]) {
        cos_weight = hi_weight_boundary;
        weight_found = TRUE;
    }
    if (kbytes <= low_weight_boundary * MTU_GH2_Quanta[MTUQuantaSel]) {
        cos_weight = low_weight_boundary;
        weight_found = TRUE;
    }
    while (!weight_found) {
        cos_weight =  (hi_weight_boundary + low_weight_boundary) / 2;
        if (kbytes <= cos_weight * MTU_GH2_Quanta[MTUQuantaSel]) {
           if (kbytes > ((cos_weight - 1) * MTU_GH2_Quanta[MTUQuantaSel])) {
               weight_found = TRUE;
           } else {
               hi_weight_boundary = cos_weight;
           }
        } else {
            if (kbytes <= ((cos_weight + 1) * MTU_GH2_Quanta[MTUQuantaSel])) {
                cos_weight++;
                weight_found = TRUE;
            } else {
                low_weight_boundary = cos_weight;
            }
        }
    } /* Here weight should be found */
    return cos_weight;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_sched_set
 * Purpose:
 *      Set class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit     - StrataSwitch unit number.
 *      pbm      - port bitmap
 *      mode     - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *      weights  - (IN) Weights for each COS queue
 *                      Only for BCM_COSQ_WEIGHTED_ROUND_ROBIN and
 *                      BCM_COSQ_DEFICIT_ROUND_ROBIN mode.
 *                 For DRR Weight is specified in Kbytes
 *      delay    - This parameter is not used
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                             int mode, const int weights[], int delay)
{
    uint32 wrr;
    int    port;
    uint32 cosarbsel;
    int mbits = 0;
    int i, max_weight;
    int enc_weights[8];
    int MTUQuantaSel = 0;
    soc_reg_t config_reg = INVALIDr;
    int is_64q_port;

    COMPILER_REFERENCE(delay);
    mbcm_driver[unit]->mbcm_cosq_sched_weight_max_get(unit, mode, &max_weight);
    if ((mode != BCM_COSQ_STRICT) && (mode != BCM_COSQ_ROUND_ROBIN) &&
        (max_weight != BCM_COSQ_WEIGHT_UNLIMITED)) {
        for (i = 0; i < NUM_COS(unit); i++) {
            if ((weights[i] < 0) || (weights[i] > max_weight)) {
                return BCM_E_PARAM;
            }
        }
    }

    /* Initialize all weights 0 (Strict Priority). */
    for (i = 0; i < 8; i++) {
        enc_weights[i] = 0;
    }

    switch (mode) {
    case BCM_COSQ_STRICT:
        mbits = 0;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        mbits = 1;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        mbits = 2;
        /*
         * All weight values must fit within 7 bits.
         * If weight is 0, this queue is run in strict mode,
         * others run in WRR mode.
         */

        for (i = 0; i < NUM_COS(unit); i++) {
            enc_weights[i] = weights[i];
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            MTUQuantaSel = bcmi_gh2_cos_drr_weights_to_quanta(weights);
            if (MTUQuantaSel < 0) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < NUM_COS(unit); i++) {
                if (weights[i]) {
                    enc_weights[i] =
                        bcmi_gh2_mtu_cos_drr_kbytes_to_weight(weights[i],
                                                              MTUQuantaSel);
                } else {
                    enc_weights[i] = weights[i];
                }
            }
        } /* soc_feature_linear_drr_weight */
        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    PBMP_ITER(pbm, port) {
        cosarbsel = 0;
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                      &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                       &is_64q_port));
        }
        if (is_64q_port) {
            config_reg = XQCOSARBSEL_QLAYERr;
        } else {
            config_reg = XQCOSARBSELr;
        }
        soc_reg_field_set(unit, config_reg, &cosarbsel, COSARBf, mbits);
        if ((mode == BCM_COSQ_DEFICIT_ROUND_ROBIN) &&
                (soc_feature(unit, soc_feature_linear_drr_weight))) {
            soc_reg_field_set(unit, config_reg, &cosarbsel,
                              MTU_QUANTA_SELECTf, MTUQuantaSel);
        }
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, config_reg, port, 0, cosarbsel));
    }

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /*
         * Weighted Fair Queueing scheduling among vaild COSs
         */
        PBMP_ITER(pbm, port) {
            if (soc_feature(unit, soc_feature_linear_drr_weight)) {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                              &is_64q_port));
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                               &is_64q_port));
                }
                if (is_64q_port) {
                    config_reg = WRRWEIGHT_COS_QLAYERr;
                } else {
                    config_reg = WRRWEIGHT_COSr;
                }
                for (i = 0; i < NUM_COS(unit); i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg32_get(unit, config_reg, port, i, &wrr));
                    soc_reg_field_set(unit, config_reg, &wrr,
                                      WEIGHTf, enc_weights[i]);
                    BCM_IF_ERROR_RETURN
                        (soc_reg32_set(unit, config_reg, port, i, wrr));
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_sched_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit     - StrataSwitch unit number.
 *      pbm      - port bitmap
 *      mode     - (output) Scheduling mode, one of BCM_COSQ_xxx
 *      weights  - (output) Weights for each COS queue
 *                          Only for BCM_COSQ_WEIGHTED_ROUND_ROBIN and
 *                          BCM_COSQ_DEFICIT_ROUND_ROBIN mode.
 *                 For DRR Weight is specified in Kbytes
 *      delay    - This parameter is not used
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Actually just returns data for the first port in the bitmap
 */

int
bcmi_gh2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                             int *mode, int weights[], int *delay)
{
    uint32 cosarbsel, wrr;
    int    mbits, port, i;
    int    MTUQuantaSel = -1;
    soc_reg_t config_reg = INVALIDr;
    int is_64q_port;

    mbits = -1;
    PBMP_ITER(pbm, port) {

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                        &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                        &is_64q_port));
        }

        if (is_64q_port) {
            config_reg = XQCOSARBSEL_QLAYERr;
        } else {
            config_reg = XQCOSARBSELr;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, config_reg, port, 0, &cosarbsel));
        mbits = soc_reg_field_get(unit, config_reg, cosarbsel, COSARBf);
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            MTUQuantaSel =
                soc_reg_field_get(unit, config_reg, cosarbsel,
                                  MTU_QUANTA_SELECTf);
        }
        break;
    }
    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
    default:
        return BCM_E_INTERNAL;
    }
    if ((mbits == 2) || (mbits == 3)) {
        wrr = 0;
        if (soc_feature(unit, soc_feature_linear_drr_weight)) {
            PBMP_ITER(pbm, port) {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                               &is_64q_port));
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                               &is_64q_port));
                }
                if (is_64q_port) {
                    config_reg = WRRWEIGHT_COS_QLAYERr;
                } else {
                    config_reg = WRRWEIGHT_COSr;
                }


                for (i = 0; i < NUM_COS(unit); i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg32_get(unit, config_reg, port, i, &wrr));
                    weights[i] = soc_reg_field_get(unit, config_reg,
                                                   wrr, WEIGHTf);
                }
            }
        }
        if (mbits == 3) {
            int i;
            for (i = 0; i < NUM_COS(unit); i++) {
                if (soc_feature(unit, soc_feature_linear_drr_weight)) {
                        weights[i] =
                            bcmi_gh2_mtu_cos_drr_weight_to_kbytes(
                                weights[i], MTUQuantaSel);
                }
            }
        }
    }
    if (delay) {
        *delay = 0;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_mapping_set
 * Purpose:
 *      Set which cosq a given priority should fall into
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value to map
 *      cosq - COS queue to map to
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_mapping_set(int unit, bcm_port_t port,
                          bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    uint32 val, old_index, new_index;
    int i;
    port_cos_map_entry_t cos_map_array[GH2_COS_MAP_ENTRIES_PER_SET], *entry_p;
    void *entries[GH2_COS_MAP_ENTRIES_PER_SET];
    bcm_pbmp_t ports;
    bcm_port_t local_port;

    if (priority < 0 || priority >= 16) {
        return (BCM_E_PARAM);
    }

    if (cosq < 0 || cosq >= 8) {
        return (BCM_E_PARAM);
    }

    if (port == -1) { /* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port, &local_port));
        } else {
            local_port = port;
        }

        if (!SOC_PORT_VALID(unit, local_port) || \
            !IS_ALL_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
        BCM_PBMP_CLEAR(ports);
        BCM_PBMP_PORT_ADD(ports, local_port);
    }

    PBMP_ITER(ports, local_port) {
        SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, local_port, &val));
        old_index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
        old_index *= GH2_COS_MAP_ENTRIES_PER_SET;

        /* get current mapping profile values */
        for (i = 0; i < GH2_COS_MAP_ENTRIES_PER_SET; i++) {
            entry_p = SOC_PROFILE_MEM_ENTRY(unit, gh2_cos_map_profile[unit],
                                            port_cos_map_entry_t *,
                                            (old_index + i));
            memcpy(&cos_map_array[i], entry_p, sizeof(*entry_p));
            entries[i] = (void *) &cos_map_array[i];
        }
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[priority], COSf, cosq);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, gh2_cos_map_profile[unit],
                                 (void *) &entries,
                                 GH2_COS_MAP_ENTRIES_PER_SET, &new_index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, local_port,
                                    SELECTf,
                                    new_index / GH2_COS_MAP_ENTRIES_PER_SET));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit,
                                    gh2_cos_map_profile[unit], old_index));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, local_port) || local_port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, gh2_cos_map_profile[unit],
                                     (void *) &entries,
                                     GH2_COS_MAP_ENTRIES_PER_SET, &new_index));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, local_port,
                                        SELECTf,
                                        new_index / GH2_COS_MAP_ENTRIES_PER_SET)
                                        );
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit,
                                        gh2_cos_map_profile[unit], old_index));
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      priority - Priority value
 *      cosq - (Output) COS queue number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_gh2_cosq_mapping_get(int unit, bcm_port_t port,
                          bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint32 val;
    int index;
    port_cos_map_entry_t *entry_p;
    bcm_port_t local_port;

    if (priority < 0 || priority >= 16) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {
        local_port = REG_PORT_ANY;
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port, &local_port));
        } else {
            local_port = port;
        }

        if (!SOC_PORT_VALID(unit, local_port) || \
            !IS_ALL_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
    }

    SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, local_port, &val));
    index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
    index *= GH2_COS_MAP_ENTRIES_PER_SET;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, gh2_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    (index + priority));
    *cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, COSf);
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_gh2_cosq_gport_bandwidth_set
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (IN) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (IN) maximum bandwidth, kbits/sec.
 *      flags - (IN) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    bcm_port_t local_port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                    &cosq_start, &cosq_end));

    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_bandwidth_set(unit, gport, i,
                                              kbits_sec_min,
                                              kbits_sec_max,
                                              kbits_sec_max,
                                              flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_bandwidth_get
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_sec_min - (OUT) minimum bandwidth, kbits/sec.
 *      kbits_sec_max - (OUT) maximum bandwidth, kbits/sec.
 *      flags - (OUT) BCM_COSQ_BW_*
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                  uint32 *kbits_sec_max, uint32 *flags)
{
    bcm_port_t local_port;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_burst;    /* Dummy variable */

    *kbits_sec_min = *kbits_sec_max = *flags = 0;

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                    &cosq_start, &cosq_end));
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_port_bandwidth_get(unit, gport, cosq_start,
                                          kbits_sec_min, kbits_sec_max,
                                          &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*
 * Convert the number of kbytes (1024 bytes) that can transmtted in one run
 * to weight encoding for
 */
static int
bcmi_gh2_cos_drr_kbytes_to_weight(int kbytes, int MTUQuantaSel)
{
    int cos_weight = 0;
    int weight_found = FALSE, low_weight_boundary, hi_weight_boundary;

    /* Search for right weight */
    low_weight_boundary = 1;
    hi_weight_boundary = GH2_COS_WEIGHT_MAX;

    /* Boundary conditions */
    if (kbytes >= hi_weight_boundary * MTU_GH2_Quanta[MTUQuantaSel]) {
        cos_weight = hi_weight_boundary;
        weight_found = TRUE;
    }
    if (kbytes <= low_weight_boundary * MTU_GH2_Quanta[MTUQuantaSel]) {
        cos_weight = low_weight_boundary;
        weight_found = TRUE;
    }

    while (!weight_found) {
        cos_weight =  (hi_weight_boundary + low_weight_boundary) / 2;
        if (kbytes <= cos_weight * MTU_GH2_Quanta[MTUQuantaSel]) {
           if (kbytes > ((cos_weight - 1) * MTU_GH2_Quanta[MTUQuantaSel])) {
               weight_found = TRUE;
           } else {
               hi_weight_boundary = cos_weight;
           }
        } else {
            if (kbytes <= ((cos_weight + 1) * MTU_GH2_Quanta[MTUQuantaSel])) {
                cos_weight++;
                weight_found = TRUE;
            } else {
                low_weight_boundary = cos_weight;
            }
        }
    } /* Here weight should be found */

    return cos_weight;
}

/*
 * Function:
 *     bcmi_gh2_cosq_port_schedule_set
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit                - (IN) unit number
 *     config_reg          - (IN) register for configuration mode
 *     weight_reg          - (IN) register for configuratin weight
 *     port                - (IN) port number or GPORT identifier
 *     qlayer_sch_id       - (IN) scheduler id
 *     start_cosq          - (IN) COS queue number
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (IN) weights array
 *     mode                - (IN) scheduling mode (BCM_COSQ_XXX)
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_port_schedule_set(int unit, soc_reg_t config_reg,
                                soc_reg_t weight_reg, bcm_port_t port,
                                int qlayer_sch_id,
                                bcm_cos_queue_t start_cosq,
                                int num_weights, const int weights[],
                                int mode)
{
    int t, i, cosq;
    uint32 wrr;
    uint32 reg_val;
    int mbits = 0;
    int enc_weights[8];
    int MTUQuantaSel = 0;

    switch (mode) {
    case BCM_COSQ_STRICT:
        mbits = 0;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        mbits = 1;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        mbits = 2;
        /*
         * All weight values must fit within 7 bits.
         * If weight is 0, this queue is run in strict mode,
         * others run in WRR mode.
         */
        t = 0;
        for (i = 0; i < num_weights; i++) {
            t |= weights[i];
        }
        if ((t & ~0x7f) != 0) {
            return BCM_E_PARAM;
        }
        for (i = 0; i < num_weights; i++) {
            enc_weights[i] = weights[i];
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;
        MTUQuantaSel = bcmi_gh2_cos_drr_weights_to_quanta(weights);
        if (MTUQuantaSel < 0) {
            return BCM_E_PARAM;
        }

        for (i = 0; i < num_weights; i++) {
            if (weights[i]) {
                enc_weights[i] =
                    bcmi_gh2_cos_drr_kbytes_to_weight(weights[i],
                                                      MTUQuantaSel);
            } else {
                enc_weights[i] = weights[i];
            }

        }
        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    /* Program the scheduling mode */
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, port, qlayer_sch_id, &reg_val));
    soc_reg_field_set(unit, config_reg, &reg_val, COSARBf, mbits);
    if ((mode == BCM_COSQ_DEFICIT_ROUND_ROBIN) &&
        (soc_feature(unit, soc_feature_linear_drr_weight))) {
        soc_reg_field_set(unit, config_reg, &reg_val,
                          MTU_QUANTA_SELECTf, MTUQuantaSel);
    }
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, config_reg, port, qlayer_sch_id, reg_val));

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /*
         * Weighted Fair Queueing scheduling among vaild COSs
         */
        for (cosq = start_cosq, i = 0; i < num_weights; cosq++, i++) {
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, weight_reg, port, cosq, &wrr));
            soc_reg_field_set(unit, weight_reg, &wrr,
                              WEIGHTf, enc_weights[i]);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, weight_reg, port, cosq, wrr));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_sched_set
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      mode - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *      weight - (IN) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq, int mode, int weight)
{
    bcm_port_t local_port;
    int i;
    gh2_cosq_node_t *node = NULL;
    int qlayer_sch_id = 0;
    bcm_cos_queue_t cosq_start = 0;
    int num_weights = 1, weights[8];
    soc_reg_t config_reg = INVALIDr;
    soc_reg_t weight_reg = INVALIDr;
    int is_64q_port;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* scheduling is supported on scheduler node */
        return BCM_E_PORT;
    }

    sal_memset(&weights, 0, sizeof(int) * 8);

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                        &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                        &is_64q_port));
        }
        if (!is_64q_port) {
            return BCM_E_PORT;
        }

        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            config_reg = XQCOSARBSEL_QGROUPr;
            weight_reg = WRRWEIGHT_COS_QGROUPr;
        } else {
            config_reg = XQCOSARBSEL_QLAYERr;
            weight_reg = WRRWEIGHT_COS_QLAYERr;
            qlayer_sch_id = node->child_idx;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, gport, &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        }

        if (is_64q_port) {
            config_reg = XQCOSARBSEL_QLAYERr;
            weight_reg = WRRWEIGHT_COS_QLAYERr;
        } else {
            config_reg = XQCOSARBSELr;
            weight_reg = WRRWEIGHT_COSr;
        }
    }

    if (cosq >= gh2_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq < 0) {
        if (node && (node->node_level == GH2_COSQ_LEVEL_QLAYER)) {
            cosq_start = node->qlayer_cosq_min;
        } else {
            cosq_start = 0;
        }
        num_weights = 8;
        for (i = 0; i < num_weights; i++) {
            if (i < gh2_num_cosq[unit]) {
                weights[i] = weight;
            } else {
                weights[i] = 0;
            }
        }
    } else {
        if (node && (node->node_level == GH2_COSQ_LEVEL_QLAYER)) {
            cosq_start = node->qlayer_cosq_min + cosq;
        } else {
            cosq_start = cosq;
        }
        num_weights = 1;
        weights[0] = weight;
    }
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_port_schedule_set(unit, config_reg, weight_reg,
                                         local_port, qlayer_sch_id,
                                         cosq_start, num_weights,
                                         weights, mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_port_schedule_get
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit                - (IN) unit number
 *     config_reg          - (IN) register for configuration mode
 *     weight_reg          - (IN) register for configuratin weight
 *     port                - (IN) port number or GPORT identifier
 *     qlayer_sch_id       - (IN) scheduler id
 *     start_cosq          - (IN) COS queue number
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (IN) weights array
 *     mode                - (OUT) scheduling mode (BCM_COSQ_XXX)
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_gh2_cosq_port_schedule_get(int unit, soc_reg_t config_reg,
                                soc_reg_t weight_reg, bcm_port_t port,
                                int qlayer_sch_id,
                                bcm_cos_queue_t start_cosq,
                                int num_weights, int weights[], int *mode)
{
    uint32 escfg, wrr;
    int mbits, i, cosq;
    int MTUQuantaSel = -1;

    if (mode == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, config_reg, port, qlayer_sch_id, &escfg));
    mbits = soc_reg_field_get(unit, config_reg, escfg, COSARBf);
    MTUQuantaSel = soc_reg_field_get(unit, config_reg, escfg,
                                     MTU_QUANTA_SELECTf);

    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
    default:
        return BCM_E_INTERNAL;
    }

    if ((mbits == 2) || (mbits == 3)) {
        wrr = 0;
        for (cosq = start_cosq, i = 0; i < num_weights; cosq++, i++) {
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, weight_reg, port, cosq, &wrr));
            weights[i] = soc_reg_field_get(unit, weight_reg, wrr,
                                           WEIGHTf);
        }

        if (mbits == 3) {
            int i;
            for (i = 0; i < num_weights; i++) {
                weights[i] =
                    bcmi_gh2_mtu_cos_drr_weight_to_kbytes(weights[i],
                                                          MTUQuantaSel);
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_sched_get
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue
 *      mode - (OUT) Scheduling mode, one of BCM_COSQ_xxx
 *      weight - (OUT) Weight for the specified COS queue(s)
 *               Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq, int *mode, int *weight)
{
    bcm_port_t local_port;
    soc_reg_t config_reg = INVALIDr;
    soc_reg_t weight_reg = INVALIDr;
    int is_64q_port;
    gh2_cosq_node_t *node = NULL;
    int qlayer_sch_id = 0;
    bcm_cos_queue_t cosq_start = 0;

    if ((mode == NULL) || (weight == NULL)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* scheduling is supported on scheduler node */
        return BCM_E_PORT;
    }

    *mode = *weight = 0;

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        }
        if (!is_64q_port) {
            return BCM_E_PORT;
        }

        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            config_reg = XQCOSARBSEL_QGROUPr;
            weight_reg = WRRWEIGHT_COS_QGROUPr;
        } else {
            config_reg = XQCOSARBSEL_QLAYERr;
            weight_reg = WRRWEIGHT_COS_QLAYERr;
            qlayer_sch_id = node->child_idx;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, gport, &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        }

        if (is_64q_port) {
            config_reg = XQCOSARBSEL_QLAYERr;
            weight_reg = WRRWEIGHT_COS_QLAYERr;
        } else {
            config_reg = XQCOSARBSELr;
            weight_reg = WRRWEIGHT_COSr;
        }
    }

    if (cosq >= gh2_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq < 0) {
        cosq = 0;
    }

    if (node && (node->node_level == GH2_COSQ_LEVEL_QLAYER)) {
        cosq_start = node->qlayer_cosq_min + cosq;
    } else {
        cosq_start = cosq;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_port_schedule_get(unit, config_reg, weight_reg,
                                         local_port, qlayer_sch_id,
                                         cosq_start, 1,
                                         weight, mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_bandwidth_burst_set
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_burst - (IN) maximum burst, kbits.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        uint32 kbits_burst)
{
    bcm_port_t local_port, port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_min, kbits_sec_max, flags;
    uint32 kbits_sec_burst; /* Dummy variable */

    if (gport < 0) {
        PBMP_ALL_ITER(unit, port) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_bandwidth_port_resolve(unit, port, cosq,
                                                            &local_port,
                                                            &cosq_start,
                                                            &cosq_end));
            for (i = cosq_start; i <= cosq_end; i++) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_port_bandwidth_get(
                        unit, local_port, i,
                        &kbits_sec_min,
                        &kbits_sec_max,
                        &kbits_sec_burst, &flags));
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_port_bandwidth_set(unit, local_port, i,
                                                      kbits_sec_min,
                                                      kbits_sec_max,
                                                      kbits_burst,
                                                      flags));
            }
        }
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                    &cosq_start, &cosq_end));

    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_bandwidth_get(unit, gport, cosq_start,
                                             &kbits_sec_min, &kbits_sec_max,
                                             &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_bandwidth_set(unit, gport, i,
                                              kbits_sec_min,
                                              kbits_sec_max,
                                              kbits_burst,
                                              flags));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_bandwidth_burst_get
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_burst - (OUT) maximum burst, kbits.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        uint32 *kbits_burst)
{
    bcm_port_t local_port, port;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_min, kbits_sec_max, flags;    /* Dummy variables */

    kbits_sec_min = kbits_sec_max = flags = 0;

    if (gport < 0) {
        port = SOC_PORT_MIN(unit, all);

        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_gport_bandwidth_port_resolve(
                unit, port, cosq,
                &local_port,
                &cosq_start, &cosq_end));
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_bandwidth_get(unit, local_port, cosq_start,
                                              &kbits_sec_min, &kbits_sec_max,
                                              kbits_burst, &flags));
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_gport_bandwidth_port_resolve(
                unit, gport, cosq,
                &local_port,
                &cosq_start, &cosq_end));
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_port_bandwidth_get(unit, gport, cosq_start,
                                              &kbits_sec_min, &kbits_sec_max,
                                              kbits_burst, &flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_control_set
 * Purpose:
 *      Set specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (IN) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_gh2_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t port;
    int is_64q_port;
    soc_reg_t config_reg = INVALIDr;
    uint32 rval;
    uint32 fval = 0;
    int offset = 0;
    int hw_cosq_idx;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_control_set: unit=%d gport=0x%x \
                         cosq=%d type=%d arg=%d\n"),
              unit, gport, cosq, type, arg));

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport, cosq,
                                                &port, &hw_cosq_idx));
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port, &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port, &is_64q_port));
    }

    switch (type) {
    case bcmCosqControlEEEQueueThresholdProfileSelect:
        if (is_64q_port) {
            config_reg = EEE_PROFILE_SEL_QLAYERr;
        } else {
            config_reg = EEE_PROFILE_SELr;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, hw_cosq_idx, &rval));
        soc_reg_field_set(unit, config_reg, &rval,
                          EEE_THRESH_SELf, arg);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, config_reg, port, hw_cosq_idx, rval));
        return BCM_E_NONE;
        break;
    case bcmCosqControlEEEPacketLatencyProfileSelect:
        if (is_64q_port) {
            config_reg = EEE_PROFILE_SEL_QLAYERr;
        } else {
            config_reg = EEE_PROFILE_SELr;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, hw_cosq_idx, &rval));
        soc_reg_field_set(unit, config_reg, &rval,
                          EEE_LATENCY_SELf, arg);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, config_reg, port, hw_cosq_idx, rval));
        return BCM_E_NONE;
        break;
    case bcmCosqControlAlternateStoreForward:
        if (is_64q_port) {
            if (hw_cosq_idx < 32) {
                config_reg = ASF_ENABLE_0_64Qr;
            } else {
                config_reg = ASF_ENABLE_1_64Qr;
            }
            offset = hw_cosq_idx % 32;
        } else {
            config_reg = ASF_ENABLE_8Qr;
            offset = hw_cosq_idx;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, 0, &rval));
        fval = soc_reg_field_get(unit, config_reg,
                                 rval, ASF_ENABLEf);

        if (arg == bcmCosqAsfModeAlternateStoreForward) {
            fval |= (1 << offset);
        } else {
            fval &= ~(1 << offset);
        }
        soc_reg_field_set(unit, config_reg, &rval,
                          ASF_ENABLEf, fval);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, config_reg, port, 0, rval));
        return BCM_E_NONE;
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmi_gh2_cosq_control_get
 * Purpose:
 *      Get specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      type - (IN) feature
 *      arg  - (OUT) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_gh2_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t port;
    int is_64q_port;
    soc_reg_t config_reg = INVALIDr;
    uint32 rval;
    uint32 fval = 0;
    int offset = 0;
    int hw_cosq_idx;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_control_get: unit=%d gport=0x%x \
                         cosq=%d type=%d\n"),
                         unit, gport, cosq, type));
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport, cosq,
                                                &port, &hw_cosq_idx));
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port, &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port, &is_64q_port));
    }

    switch (type) {
    case bcmCosqControlEEEQueueThresholdProfileSelect:
        if (is_64q_port) {
            config_reg = EEE_PROFILE_SEL_QLAYERr;
        } else {
            config_reg = EEE_PROFILE_SELr;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, hw_cosq_idx, &rval));
        *arg = soc_reg_field_get(unit, config_reg,
                                 rval, EEE_THRESH_SELf);
        return BCM_E_NONE;
        break;
    case bcmCosqControlEEEPacketLatencyProfileSelect:
        if (is_64q_port) {
            config_reg = EEE_PROFILE_SEL_QLAYERr;
        } else {
            config_reg = EEE_PROFILE_SELr;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, hw_cosq_idx, &rval));
        *arg = soc_reg_field_get(unit, config_reg,
                                 rval, EEE_LATENCY_SELf);
        return BCM_E_NONE;
        break;
    case bcmCosqControlAlternateStoreForward:
        if (is_64q_port) {
            if (hw_cosq_idx < 32) {
                config_reg = ASF_ENABLE_0_64Qr;
            } else {
                config_reg = ASF_ENABLE_1_64Qr;
            }
            offset = hw_cosq_idx % 32;
        } else {
            config_reg = ASF_ENABLE_8Qr;
            offset = hw_cosq_idx;
        }

        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, config_reg, port, 0, &rval));
        fval = soc_reg_field_get(unit, config_reg,
                                 rval, ASF_ENABLEf);
        if (fval & (1 << offset)) {
            *arg = bcmCosqAsfModeAlternateStoreForward;
        } else {
            *arg = bcmCosqAsfModeDisabled;
        }
        return BCM_E_NONE;
        break;
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmi_gh2_cosq_num_validate
 * Purpose:
 *      validate cosq id
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_gh2_cosq_num_validate(int unit, bcm_port_t port, int cosq)
{
    int max_cosq_per_port;
    int is_64q_port;

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port, &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port, &is_64q_port));
    }

    if (is_64q_port) {
        max_cosq_per_port = SOC_GH2_2LEVEL_QUEUE_NUM;
    } else {
        max_cosq_per_port = SOC_GH2_LEGACY_QUEUE_NUM;
    }

    if (((cosq >= 0) && (cosq < max_cosq_per_port)) ||
        (cosq == BCM_COS_INVALID)) {
        return BCM_E_NONE;
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *      bcmi_gh2_cosq_stat_get
 * Parameters:
 *      unit  - (IN) Unit number
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be retrieved.
 *      value - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_gh2_cosq_stat_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    bcm_port_t local_port;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    uint64 val64;
    int i, rv;
    int is_64q_port = 0;
    int total_cosq = 0;
    int cosq_idx = 0;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_stat_get: unit=%d gport=0x%x \
                          cosq=%d stat=%d sync_mode=%d\n"),
              unit, gport, cosq, stat, sync_mode));

    if (value == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * if sync-mode is set, update the software cached counter value,
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1) ? soc_counter_sync_get : soc_counter_get;
    COMPILER_64_ZERO(*value);
    COMPILER_64_ZERO(val64);

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        if (cosq == BCM_COS_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_localport_resolve(unit, gport,
                                                                &local_port));
            /*
             * SOC_COUNTER_NON_DMA_COSQ_DROP_PKT gets the
             * EGRDROPPKTCOUNT_COSr which is a register array with
             * 64 elements per-port.
             */
            for (i = 0; i < SOC_GH2_2LEVEL_QUEUE_NUM; i++) {
                rv = counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, i, &val64);
                if (SOC_SUCCESS(rv)) {
                    COMPILER_64_ADD_64(*value, val64);
                } else {
                    break;
                }
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport,
                                                        cosq,
                                                        &local_port,
                                                        &cosq_idx));

            SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                            cosq_idx, value));
        }
        break;
    case bcmCosqStatOutPackets:
        if (cosq == BCM_COS_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_localport_resolve(unit, gport,
                                                                &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (!is_64q_port) {
                total_cosq = SOC_GH2_LEGACY_QUEUE_NUM;
            } else {
                total_cosq = SOC_GH2_2LEVEL_QUEUE_NUM;
            }
            for (i = 0; i < total_cosq; i++) {
                rv = counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                 i, &val64);
                if (SOC_SUCCESS(rv)) {
                    COMPILER_64_ADD_64(*value, val64);
                } else {
                    break;
                }
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport,
                                                        cosq,
                                                        &local_port,
                                                        &cosq_idx));
            SOC_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                             cosq_idx, value));
        }
        break;
    case bcmCosqStatOutBytes:
        if (cosq == BCM_COS_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_localport_resolve(unit, gport,
                                                                &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (!is_64q_port) {
                total_cosq = SOC_GH2_LEGACY_QUEUE_NUM;
            } else {
                total_cosq = SOC_GH2_2LEVEL_QUEUE_NUM;
            }
            for (i = 0; i < total_cosq; i++) {
                rv = counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                 i, &val64);
                if (SOC_SUCCESS(rv)) {
                    COMPILER_64_ADD_64(*value, val64);
                } else {
                    break;
                }
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport,
                                                        cosq,
                                                        &local_port,
                                                        &cosq_idx));
            SOC_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                             cosq_idx, value));
        }
        break;
    case bcmCosqStatYellowCongestionDroppedPackets:
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport,
                                                         &local_port));
        BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_num_validate(unit, local_port, cosq));
        if (cosq != BCM_COS_INVALID) {
           /* Yellow dropped packet counters are available only on a per
            * port basis.
            */
           return BCM_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                         SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0, value));
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport,
                                                         &local_port));
        BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_num_validate(unit, local_port, cosq));
        if (cosq != BCM_COS_INVALID) {
            /* Red dropped packet counters are available only on a per
             * port basis.
             */
            return BCM_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(counter_get(unit, local_port,
                                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED,
                                        0, value));
        break;
    case bcmCosqStatDiscardDroppedPackets:
        if (soc_feature(unit, soc_feature_wred_drop_counter_per_port)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport,
                                                             &local_port));
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_num_validate(unit, local_port,
                                                           cosq));
            COMPILER_64_ZERO(*value);
            if (cosq == BCM_COS_INVALID) {
                SOC_IF_ERROR_RETURN
                    (soc_counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT,
                                     0, value));
            } else {
                return BCM_E_UNAVAIL;
            }
        }
        break;
    case bcmCosqStatTASTxOverrunPackets:
        if (soc_feature(unit, soc_feature_tas)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, gport, &local_port));
            BCM_IF_ERROR_RETURN(
                bcmi_gh2_tas_port_validate(unit, local_port));
            if (cosq != BCM_COS_INVALID) {
               /* Only support per port basis */
               return BCM_E_UNAVAIL;
            }
            COMPILER_64_ZERO(*value);
            COMPILER_64_ZERO(val64);
            SOC_IF_ERROR_RETURN(
                counter_get(unit, local_port,
                            SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_PREEMPT,
                            0, &val64));
            COMPILER_64_ADD_64(*value, val64);

            COMPILER_64_ZERO(val64);
            SOC_IF_ERROR_RETURN(
                counter_get(unit, local_port,
                            SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS,
                            0, &val64));
            COMPILER_64_ADD_64(*value, val64);
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_stat_set
 * Purpose:
 *      Set statistics
 * Parameters:
 *      unit  - (IN) Unit number.
 *      gport - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be set.
 *      value - (IN) Statistic value to be set.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_gh2_cosq_stat_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_stat_t stat, uint64 value)
{
    bcm_port_t local_port;
    uint64 val64_zero;
    int i, rv;
    int is_64q_port = 0;
    int total_cosq = 0;
    int cosq_idx = 0;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_stat_set: unit=%d gport=0x%x \
                          cosq=%d stat=%d\n"),
              unit, gport, cosq, stat));

    switch (stat) {
    case bcmCosqStatOutPackets:
        COMPILER_64_ZERO(val64_zero);
        if (cosq == BCM_COS_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_localport_resolve(unit, gport,
                                                                &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (!is_64q_port) {
                total_cosq = SOC_GH2_LEGACY_QUEUE_NUM;
            } else {
                total_cosq = SOC_GH2_2LEVEL_QUEUE_NUM;
            }
            /* Write given value to first COS queue */
            rv = soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                 0, value);

            /* Write zero to all other COS queues */
            for (i = 1; i < total_cosq; i++) {
                rv = soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                     i, val64_zero);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport,
                                                        cosq,
                                                        &local_port,
                                                        &cosq_idx));
            SOC_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                 cosq_idx, value));
        }
        break;
    case bcmCosqStatOutBytes:
        COMPILER_64_ZERO(val64_zero);
        if (cosq == BCM_COS_INVALID) {
            BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_localport_resolve(unit, gport,
                                                                &local_port));
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (!is_64q_port) {
                total_cosq = SOC_GH2_LEGACY_QUEUE_NUM;
            } else {
                total_cosq = SOC_GH2_2LEVEL_QUEUE_NUM;
            }

            /* Write given value to first COS queue */
            rv = soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                 0, value);

            /* Write zero to all other COS queues */
            for (i = 1; i < total_cosq; i++) {
                rv = soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                     i, val64_zero);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_qlayer_scheduler_resolve(unit, gport,
                                                        cosq,
                                                        &local_port,
                                                        &cosq_idx));
            SOC_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                 cosq_idx, value));
        }
        break;
    case bcmCosqStatDiscardDroppedPackets:
        if (soc_feature(unit, soc_feature_wred_drop_counter_per_port)) {
             BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, gport,
                                                              &local_port));
            if (cosq == BCM_COS_INVALID) {
                SOC_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT,
                                     0, value));
            } else {
                return BCM_E_UNAVAIL;
            }
        }
        break;
    case bcmCosqStatTASTxOverrunPackets:
        if (soc_feature(unit, soc_feature_tas)) {
            /* read-only register, only take value=0 to clear it */
            if (!COMPILER_64_IS_ZERO(value)) {
                return BCM_E_UNAVAIL;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_gport_validate(unit, gport, &local_port));
            if (cosq == BCM_COS_INVALID) {
                SOC_IF_ERROR_RETURN(
                    soc_counter_set(unit, local_port,
                                    SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS,
                                    0, value));
            } else {
                return BCM_E_UNAVAIL;
            }
        }
        break;
    case bcmCosqStatDroppedPackets:
    case bcmCosqStatYellowCongestionDroppedPackets:
    case bcmCosqStatRedCongestionDroppedPackets:
        /* registers are read-only */
        return BCM_E_UNAVAIL;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 *  Convert HW drop probability to percent value
 */
STATIC int
gh2_hw_drop_prob_to_percent[] = {
    0,     /* 0  */
    1,     /* 1  */
    2,     /* 2  */
    3,     /* 3  */
    4,     /* 4  */
    5,     /* 5  */
    6,     /* 6  */
    7,     /* 7  */
    8,     /* 8  */
    9,     /* 9  */
    10,    /* 10 */
    25,    /* 11 */
    50,    /* 12 */
    75,    /* 13 */
    100,   /* 14 */
    -1     /* 15 */
};

STATIC int
bcmi_gh2_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= gh2_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
bcmi_gh2_drop_prob_to_percent(int drop_prob) {
   return (gh2_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * index: degree, value: contangent(degree) * 100
 * max value is 0x7fff (15-bit) at 0 degree
 */
STATIC int
bcmi_gh2_cotangent_lookup_table[] =
{
    /*  0.. 5 */  32767, 5728, 2863, 1908, 1430, 1143,
    /*  6..11 */    951,  814,  711,  631,  567,  514,
    /* 12..17 */    470,  433,  401,  373,  348,  327,
    /* 18..23 */    307,  290,  274,  260,  247,  235,
    /* 24..29 */    224,  214,  205,  196,  188,  180,
    /* 30..35 */    173,  166,  160,  153,  148,  142,
    /* 36..41 */    137,  132,  127,  123,  119,  115,
    /* 42..47 */    111,  107,  103,  100,   96,   93,
    /* 48..53 */     90,   86,   83,   80,   78,   75,
    /* 54..59 */     72,   70,   67,   64,   62,   60,
    /* 60..65 */     57,   55,   53,   50,   48,   46,
    /* 66..71 */     44,   42,   40,   38,   36,   34,
    /* 72..77 */     32,   30,   28,   26,   24,   23,
    /* 78..83 */     21,   19,   17,   15,   14,   12,
    /* 84..89 */     10,    8,    6,    5,    3,    1,
    /* 90     */      0
};

/*
 * Given a slope (angle in degrees) from 0 to 90, return the number of cells
 * in the range from 0% drop probability to 100% drop probability.
 */
STATIC int
bcmi_gh2_angle_to_cells(int angle)
{
    return (bcmi_gh2_cotangent_lookup_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
bcmi_gh2_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= bcmi_gh2_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}

/*
 * Function:
 *     bcmi_gh2_cosq_wred_set
 * Purpose:
 *     Configure queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (IN)
 *     max_thresh          - (IN)
 *     drop_probablity     - (IN)
 *     gain                - (IN)
 *     ignore_enable_flags - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
bcmi_gh2_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       uint32 flags, uint32 min_thresh, uint32 max_thresh,
                       int drop_probability, int gain, int ignore_enable_flags,
                       uint32 lflags)
{
    int index;
    uint32 profile_index, old_profile_index;
    mmu_wred_drop_profile_green_entry_t entry_tcp_green;
    mmu_wred_drop_profile_yellow_entry_t entry_tcp_yellow;
    mmu_wred_drop_profile_red_entry_t entry_tcp_red;
    mmu_wred_mark_profile_green_entry_t entry_mark_green;
    mmu_wred_mark_profile_yellow_entry_t entry_mark_yellow;
    mmu_wred_mark_profile_red_entry_t entry_mark_red;
    mmu_wred_config_entry_t qentry;
    void *entries[6];
    soc_mem_t mems[6];
    int rate, i, exists = 0;
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_PROFILE_GREENm,
        MMU_WRED_DROP_PROFILE_YELLOWm,
        MMU_WRED_DROP_PROFILE_REDm,
        MMU_WRED_MARK_PROFILE_GREENm,
        MMU_WRED_MARK_PROFILE_YELLOWm,
        MMU_WRED_MARK_PROFILE_REDm
    };
    int is_64q_port = 0;
    soc_mem_t config_mem = INVALIDm;
    gh2_cosq_node_t *node;
    bcm_port_t local_port;
    int local_cosq = 0;
    int cell_field_max;

    /*
     * if (port is GPORT scheduler)
     *     cosq is the child index of the GPORT object (QGROUP or QLAYER)
     *     valid value : 0~7
     * otherwise (just resolve the local port id)
     *     cosq is COSQ number, value 0~7 or 0~63
     */
    if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, port, 0, NULL,
                                    &local_port, NULL, &node));
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            if ((cosq < 0) ||
                (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }

            config_mem = MMU_WRED_CONFIG_QGROUPm;
            /* Get the MMU_WRED_CONFIG_QGROUPm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qgroup_index
                                    (unit, local_port, cosq, &index));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qgroup_index
                                    (unit, local_port, cosq, &index));
            }
        } else if (node->node_level == GH2_COSQ_LEVEL_QLAYER) {
            if ((cosq < 0) ||
                (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }
            config_mem = MMU_WRED_CONFIGm;
            local_cosq = node->qlayer_cosq_min + cosq;
            /* Get the MMU_WRED_CONFIGm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                    (unit, local_port, local_cosq, &index));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                    (unit, local_port, local_cosq, &index));
            }
        } else {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, port, &local_port));

        if (!SOC_PORT_VALID(unit, local_port) ||
            (CMIC_PORT(unit) == local_port)) {
            return BCM_E_PORT;
        }

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        }
        /* cosq parameter is value */
        if (is_64q_port) {
            if ((cosq < 0) || cosq >= SOC_GH2_2LEVEL_QUEUE_NUM) {
                return BCM_E_PARAM;
            }
        } else {
            if ((cosq < 0) || cosq >= SOC_GH2_LEGACY_QUEUE_NUM) {
                return BCM_E_PARAM;
            }
        }

        local_cosq = cosq;
        config_mem = MMU_WRED_CONFIGm;
        /* Get the MMU_WRED_CONFIGm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                (unit, local_port, local_cosq, &index));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                (unit, local_port, local_cosq, &index));
        }
    }

    if (index < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, SOC_BLOCK_ALL, index, &qentry));

    old_profile_index = 0xffffffff;

    if (flags & BCM_COSQ_DISCARD_NONTCP) {
        /* Greyhound-based WRED  doesnot support non-responsive dropping */
        return BCM_E_UNAVAIL;
    }

    if (soc_feature(unit, soc_feature_fl)) {
        cell_field_max = FL_CELL_FIELD_MAX;
    } else {
        cell_field_max = GH2_CELL_FIELD_MAX;
    }

    if (flags & (BCM_COSQ_DISCARD_TCP | BCM_COSQ_DISCARD_COLOR_ALL |
                BCM_COSQ_DISCARD_MARK_CONGESTION)) {
            old_profile_index = soc_mem_field32_get(unit, config_mem,
                                                    &qentry, PROFILE_INDEXf);
        entries[0] = &entry_tcp_green;
        entries[1] = &entry_tcp_yellow;
        entries[2] = &entry_tcp_red;
        entries[3] = &entry_mark_green;
        entries[4] = &entry_mark_yellow;
        entries[5] = &entry_mark_red;
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_get(unit, gh2_wred_profile[unit],
                                 old_profile_index, 1, entries));
        for (i = 0; i < 6; i++) {
            mems[i] = INVALIDm;
        }
        if ((flags & BCM_COSQ_DISCARD_TCP) ||
                  !(flags & BCM_COSQ_DISCARD_MARK_CONGESTION)) {
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[0] = MMU_WRED_DROP_PROFILE_GREENm;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[1] = MMU_WRED_DROP_PROFILE_YELLOWm;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[2] = MMU_WRED_DROP_PROFILE_REDm;
            }
        }
        if (flags & BCM_COSQ_DISCARD_MARK_CONGESTION) {
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[3] = MMU_WRED_MARK_PROFILE_GREENm;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[4] = MMU_WRED_MARK_PROFILE_YELLOWm;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[5] = MMU_WRED_MARK_PROFILE_REDm;
            }
        }
        rate = bcmi_gh2_percent_to_drop_prob(drop_probability);
        for (i = 0; i < 6; i++) {
            exists = 0;
            if ((soc_mem_field32_get(unit, wred_mems[i], entries[i],
                    MIN_THDf) != cell_field_max) && (mems[i] == INVALIDm)) {
                mems[i] = wred_mems[i];
                exists = 1;
            } else {
                soc_mem_field32_set(unit, wred_mems[i], entries[i],
                        MIN_THDf, (mems[i] == INVALIDm) ?
                        cell_field_max : min_thresh);
            }

            if ((soc_mem_field32_get(unit, wred_mems[i], entries[i],
                        MAX_THDf) != cell_field_max) &&
                        ((mems[i] == INVALIDm) || exists)) {
                mems[i] = wred_mems[i];
                exists = 1;
            } else {
                soc_mem_field32_set(unit, wred_mems[i], entries[i],
                        MAX_THDf, (mems[i] == INVALIDm) ?
                        cell_field_max :max_thresh);
            }

            if (!exists) {
                if (soc_mem_field_valid(unit, wred_mems[i],
                                                        MAX_DROP_RATEf)) {
                    soc_mem_field32_set(unit, wred_mems[i], entries[i],
                        MAX_DROP_RATEf, (mems[i] == INVALIDm) ? 0 : rate);
                } else if (soc_mem_field_valid(unit, wred_mems[i],
                                                        MAX_MARK_RATEf)) {
                    soc_mem_field32_set(unit, wred_mems[i], entries[i],
                        MAX_MARK_RATEf, (mems[i] == INVALIDm) ? 0 : rate);
                }
            }
        }
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, gh2_wred_profile[unit],
                                             entries, 1, &profile_index));
        soc_mem_field32_set(unit, config_mem,
                    &qentry, PROFILE_INDEXf, profile_index);
        soc_mem_field32_set(unit, config_mem, &qentry, WEIGHTf, gain);
    }
    /* Some APIs only modify profiles */
    if (!ignore_enable_flags) {
        soc_mem_field32_set(unit, config_mem, &qentry, CAP_AVGf,
                          flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
        soc_mem_field32_set(unit, config_mem, &qentry, WRED_ENf,
                          flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
        soc_mem_field32_set(unit, config_mem, &qentry, ECN_MARKING_ENf,
                          flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?  1 : 0);
    }

    BCM_IF_ERROR_RETURN(
          soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, index, &qentry));

    if (old_profile_index != 0xffffffff) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, gh2_wred_profile[unit],
                                    old_profile_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_wred_get
 * Purpose:
 *     Get queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN/OUT) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (OUT)
 *     max_thresh          - (OUT)
 *     drop_probablity     - (OUT)
 *     gain                - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
bcmi_gh2_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                       int *drop_probability, int *gain, uint32 lflags)
{
    int index, profile_index;
    mmu_wred_drop_profile_green_entry_t entry_tcp_green;
    mmu_wred_drop_profile_yellow_entry_t entry_tcp_yellow;
    mmu_wred_drop_profile_red_entry_t entry_tcp_red;
    mmu_wred_mark_profile_green_entry_t entry_mark_green;
    mmu_wred_mark_profile_yellow_entry_t entry_mark_yellow;
    mmu_wred_mark_profile_red_entry_t entry_mark_red;
    void *entries[6];
    void *entry_p = NULL;
    soc_mem_t profile_mem = INVALIDm;
    mmu_wred_config_entry_t qentry;
    int rate = 0;
    int is_64q_port = 0;
    soc_mem_t config_mem = INVALIDm;
    gh2_cosq_node_t *node;
    bcm_port_t local_port;
    int local_cosq = 0;

    /*
     * if (port is GPORT scheduler)
     *     cosq is the child index of the GPORT object (QGROUP or QLAYER)
     * otherwise (just resolve the local port id)
     *     cosq is COSQ number, value 0~7 or 0~63
     */
    if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, port, 0, NULL,
                                    &local_port, NULL, &node));
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            if ((cosq < 0) ||
                (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }

            config_mem = MMU_WRED_CONFIG_QGROUPm;
            /* Get the MMU_WRED_CONFIG_QGROUPm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qgroup_index
                                    (unit, local_port, cosq, &index));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qgroup_index
                                    (unit, local_port, cosq, &index));
            }
        } else if (node->node_level == GH2_COSQ_LEVEL_QLAYER) {
            if ((cosq < 0) ||
                (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }

            config_mem = MMU_WRED_CONFIGm;
            local_cosq = node->qlayer_cosq_min + cosq;
            /* Get the MMU_WRED_CONFIGm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                    (unit, local_port, local_cosq, &index));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                    (unit, local_port, local_cosq, &index));
            }
        } else {
            return BCM_E_PARAM;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, port, &local_port));

        if (!SOC_PORT_VALID(unit, local_port) ||
            (CMIC_PORT(unit) == local_port)) {
            return BCM_E_PORT;
        }

#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                   &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                   &is_64q_port));
        }

        /* cosq parameter is value */
        if (is_64q_port) {
            if ((cosq < 0) || cosq >= SOC_GH2_2LEVEL_QUEUE_NUM) {
                return BCM_E_PARAM;
            }
        } else {
            if ((cosq < 0) || cosq >= SOC_GH2_LEGACY_QUEUE_NUM) {
                return BCM_E_PARAM;
            }
        }

        local_cosq = cosq;
        config_mem = MMU_WRED_CONFIGm;
        /* Get the MMU_WRED_CONFIGm index by (port, cosq) */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_mmu_bucket_qlayer_index
                                (unit, local_port, local_cosq, &index));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_mmu_bucket_qlayer_index
                                (unit, local_port, local_cosq, &index));
        }
    }
    if (index < 0) {
        return BCM_E_PARAM;
    }


    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, index, &qentry));
    profile_index = soc_mem_field32_get(unit, config_mem,
                                        &qentry, PROFILE_INDEXf);

    if (*flags & BCM_COSQ_DISCARD_MARK_CONGESTION) {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            profile_mem = MMU_WRED_MARK_PROFILE_GREENm;
            entry_p = &entry_mark_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            profile_mem = MMU_WRED_MARK_PROFILE_YELLOWm;
            entry_p = &entry_mark_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            profile_mem = MMU_WRED_MARK_PROFILE_REDm;
            entry_p = &entry_mark_red;
        } else {
            profile_mem = MMU_WRED_MARK_PROFILE_GREENm;
            entry_p = &entry_mark_green;
        }
    } else {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            profile_mem = MMU_WRED_DROP_PROFILE_GREENm;
            entry_p = &entry_tcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            profile_mem = MMU_WRED_DROP_PROFILE_YELLOWm;
            entry_p = &entry_tcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            profile_mem = MMU_WRED_DROP_PROFILE_REDm;
            entry_p = &entry_tcp_red;
        } else {
            profile_mem = MMU_WRED_DROP_PROFILE_GREENm;
            entry_p = &entry_tcp_green;
        }
    }

    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_mark_green;
    entries[4] = &entry_mark_yellow;
    entries[5] = &entry_mark_red;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, gh2_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, profile_mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, profile_mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        if (soc_mem_field_valid(unit, profile_mem, MAX_DROP_RATEf)) {
            rate = soc_mem_field32_get(
                       unit, profile_mem, entry_p, MAX_DROP_RATEf);
        } else if (soc_mem_field_valid(unit, profile_mem, MAX_MARK_RATEf)) {
            rate = soc_mem_field32_get(
                       unit, profile_mem, entry_p, MAX_MARK_RATEf);
        }
        *drop_probability = bcmi_gh2_drop_prob_to_percent(rate);
    }

    if (gain) {
        *gain = soc_mem_field32_get(unit, config_mem, &qentry, WEIGHTf);
    }

    *flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE | BCM_COSQ_DISCARD_ENABLE);
    if (soc_mem_field32_get(unit, config_mem, &qentry, CAP_AVGf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if (soc_mem_field32_get(unit, config_mem, &qentry, WRED_ENf)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if (soc_mem_field32_get(unit, config_mem, &qentry, ECN_MARKING_ENf)) {
        *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
    }
    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    int numq, i;
    int is_64q_port = 0;

    flags &= ~(BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL);

    PBMP_PORT_ITER(unit, port) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port,
                                                       &is_64q_port));
        }
        if (is_64q_port) {
            numq = SOC_GH2_2LEVEL_QUEUE_NUM;
        } else {
            numq = SOC_GH2_LEGACY_QUEUE_NUM;
        }
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_wred_set(unit, port, i, flags, 0, 0, 0, 0,
                                        FALSE, BCM_COSQ_DISCARD_PORT));
        }
    }

    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        return bcmi_gh2_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                      NULL, BCM_COSQ_DISCARD_PORT);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_discard_set
 * Purpose:
 *     Configure port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to configure, -1 for all COS queues
 *     discard - (IN) discard settings
 * Returns:
 *     BCM_E_XXX
 */

int
bcmi_gh2_cosq_gport_discard_set(int unit, bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard)
{
    int numq, i;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int cosq_start;
    int is_64q_port = 0;

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100) {
        return BCM_E_PARAM;
    }

    if ((discard->min_thresh < 0) || (discard->max_thresh < 0) ||
                        (discard->min_thresh > discard->max_thresh)) {
        return BCM_E_PARAM;
    }

    cell_size = GH2_BYTES_PER_CELL;
    if (soc_feature(unit, soc_feature_fl)) {
        cell_field_max = FL_CELL_FIELD_MAX;
    } else {
        cell_field_max = GH2_CELL_FIELD_MAX;
    }

    min_thresh = discard->min_thresh;
    max_thresh = discard->max_thresh;
    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert bytes to cells */
        min_thresh += (cell_size - 1);
        min_thresh /= cell_size;

        max_thresh += (cell_size - 1);
        max_thresh /= cell_size;

        if ((min_thresh > cell_field_max) ||
            (max_thresh > cell_field_max)) {
            return BCM_E_PARAM;
        }
    } else { /* BCM_COSQ_DISCARD_PACKETS */
        if ((min_thresh > cell_field_max) ||
            (max_thresh > cell_field_max)) {
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (cosq < -1 || cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM) {
            return BCM_E_PARAM;
        }

        if (cosq == -1) {
            numq = SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM;
            cosq_start = 0;
        } else {
            numq = 1;
            cosq_start = cosq;
        }

        for (i = cosq_start; i < cosq_start+numq; i++) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_wred_set(unit, port, i,
                                        discard->flags,
                                        min_thresh, max_thresh,
                                        discard->drop_probability,
                                        discard->gain,
                                        FALSE, 0));
        }
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port) ||
                   (CMIC_PORT(unit) == port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }

        BCM_PBMP_ITER(pbmp, local_port) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (is_64q_port) {
                if (cosq == -1) {
                    numq = SOC_GH2_QLAYER_COSQ_PER_PORT_NUM;
                    cosq_start = 0;
                } else {
                    numq = 1;
                    cosq_start = cosq;
                }
            } else {
                if (cosq == -1) {
                    numq = SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM;
                    cosq_start = 0;
                } else {
                    numq = 1;
                    cosq_start = cosq;
                }
            }

            for (i = cosq_start; i < cosq_start + numq; i++) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_wred_set(unit, local_port, i,
                                            discard->flags,
                                            min_thresh, max_thresh,
                                            discard->drop_probability,
                                            discard->gain,
                                            FALSE, 0));
            }
        }
    }

    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_gport_discard_get(int unit, bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        local_port = port;
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port) ||
                            (CMIC_PORT(unit) == port)) {
            return BCM_E_PORT;
        } else {
            /* coverity[overrun-local] */
            BCM_PBMP_PORT_SET(pbmp, port);
        }
        BCM_PBMP_ITER(pbmp, local_port) {
            break;
        }
    }

    if (cosq < -1 || cosq >= gh2_num_cosq[unit]) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                                &discard->flags, &min_thresh, &max_thresh,
                                &discard->drop_probability, &discard->gain, 0));

    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert number of cells to number of bytes */
        min_thresh *=  GH2_BYTES_PER_CELL;
        max_thresh *=  GH2_BYTES_PER_CELL;
    }
    discard->min_thresh = min_thresh;
    discard->max_thresh = max_thresh;

    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_discard_port_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 color,
                               int drop_start,
                               int drop_slope,
                               int average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain;
    uint32 min_thresh, max_thresh, n;
    uint32 rval, bits;
    int numq, i, cosq_start;

    int is_64q_port = 0;
    int local_cosq;
    gh2_cosq_node_t *node = NULL;
    int cell_field_max;

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    /*
     * average queue size is reculated every 2us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     * gain = log2(average_time / 2)
     */
    bits = (average_time / gh2_wred_update_interval[unit]) & 0xffff;
    if (bits != 0) {
        bits |= bits >> 1;
        bits |= bits >> 2;
        bits |= bits >> 4;
        bits |= bits >> 8;
        gain = _shr_popcount(bits) - 1;
    } else {
        gain = 0;
    }

    if (soc_feature(unit, soc_feature_fl)) {
        cell_field_max = FL_CELL_FIELD_MAX;
    } else {
        cell_field_max = GH2_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        /*
         * Paramteres :
         *     port : gport id, only accept scheduler gport type
         *     cosq : Valid value 0~7, the input/child id of the gport
         *              Checked in bcmi_gh2_cosq_wred_set
         */
        if (cosq == -1) {
            numq = SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM;
            cosq_start = 0;
        } else {
            numq = 1;
            cosq_start = cosq;
        }

        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, port, 0, NULL,
                                    &local_port, NULL, &node));

        for (i = cosq_start; i < (cosq_start + numq); i++) {
            /* Get the start point as a function of the HOL limit */
            if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
                if ((i < 0) || (i >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                    return BCM_E_PARAM;
                }

                SOC_IF_ERROR_RETURN
                    (READ_HOLCOSCELLMAXLIMIT_QGROUPr(unit, local_port,
                                                     i, &rval));
                n = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, rval,
                                      CELLMAXLIMITf);
            } else if (node->node_level == GH2_COSQ_LEVEL_QLAYER) {
                if ((i < 0) || (i >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                    return BCM_E_PARAM;
                }
                /* transfered to hw queue index (0~63) */
                local_cosq = node->qlayer_cosq_min + i;

                SOC_IF_ERROR_RETURN
                    (READ_HOLCOSCELLMAXLIMIT_QLAYERr(unit, local_port,
                                                     local_cosq, &rval));
                n = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, rval,
                                      CELLMAXLIMITf);
            } else {
                /* only accept scheduler gport type */
                return BCM_E_PORT;
            }

            min_thresh = drop_start * n / 100;

            /* Calculate the max threshold. For a given slope (angle in
             * degrees), determine how many packets are in the range from
             * 0% drop probability to 100% drop probability. Add that
             * number to the min_treshold to the the max_threshold.
             */
            max_thresh = min_thresh + bcmi_gh2_angle_to_cells(drop_slope);
            if (max_thresh > cell_field_max) {
                max_thresh = cell_field_max;
            }
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_wred_set(unit, port, i,
                                        (color | BCM_COSQ_DISCARD_TCP),
                                        min_thresh, max_thresh, 100,
                                        gain, TRUE, 0));
        }
    } else {
        /*
         * Paramteres :
         *     port : local port id
         *     cosq : Valid value 0~7 or 0~63, depends on MMU port id
         *              Checked in bcmi_gh2_cosq_wred_set
         */

        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port) || (CMIC_PORT(unit) == port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }

        BCM_PBMP_ITER(pbmp, local_port) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                           &is_64q_port));
            }
            if (is_64q_port) {
                if ((cosq < -1) || cosq >= SOC_GH2_2LEVEL_QUEUE_NUM) {
                    return BCM_E_PARAM;
                }

                if (cosq == -1) {
                    numq = SOC_GH2_QLAYER_COSQ_PER_PORT_NUM;
                    cosq_start = 0;
                } else {
                    numq = 1;
                    cosq_start = cosq;
                }
            } else {
                if ((cosq < -1) || cosq >= SOC_GH2_LEGACY_QUEUE_NUM) {
                    return BCM_E_PARAM;
                }

                if (cosq == -1) {
                    numq = SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM;
                    cosq_start = 0;
                } else {
                    numq = 1;
                    cosq_start = cosq;
                }
            }

            for (i = cosq_start; i < (cosq_start + numq); i++) {
                /* Get the start point as a function of the HOL limit */
                if (is_64q_port) {
                    SOC_IF_ERROR_RETURN
                        (READ_HOLCOSCELLMAXLIMIT_QLAYERr(unit, local_port,
                                                         i, &rval));
                    n = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                                          rval, CELLMAXLIMITf);
                } else {
                    SOC_IF_ERROR_RETURN
                        (READ_HOLCOSCELLMAXLIMITr(unit, local_port, i, &rval));
                    n = soc_reg_field_get(unit, HOLCOSCELLMAXLIMITr, rval,
                                          CELLMAXLIMITf);
                }

                min_thresh = drop_start * n / 100;

                /* Calculate the max threshold. For a given slope (angle in
                 * degrees), determine how many packets are in the range from
                 * 0% drop probability to 100% drop probability. Add that
                 * number to the min_treshold to the the max_threshold.
                 */
                max_thresh = min_thresh + bcmi_gh2_angle_to_cells(drop_slope);
                if (max_thresh > cell_field_max) {
                    max_thresh = cell_field_max;
                }
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_wred_set(unit, local_port, i,
                                           (color | BCM_COSQ_DISCARD_TCP),
                                           min_thresh, max_thresh, 100,
                                           gain, TRUE, 0));
            }
        }
    }

    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_discard_port_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 color,
                               int *drop_start,
                               int *drop_slope,
                               int *average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
    uint32 min_thresh, max_thresh, pkt_limit;
    uint32 rval;
    int is_64q_port = 0;
    int local_cosq;
    gh2_cosq_node_t *node = NULL;

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        local_port = port;
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port) || (CMIC_PORT(unit) == port)) {
            return BCM_E_PORT;
        } else {
            /* coverity[overrun-local] */
            BCM_PBMP_PORT_SET(pbmp, port);
        }
        BCM_PBMP_ITER(pbmp, local_port) {
            break;
        }
    }

    /*
     * Paramteres : checked in bcmi_gh2_cosq_wred_get
     *     port is gport id, only accept scheduler gport type
     *     cosq : Valid value 0~7, the input/child id of the gport
     *
     *     port is local port id
     *     cosq : Valid value 0~7 or 0~63, depends on MMU port id
     */

    color |= BCM_COSQ_DISCARD_TCP;
    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                                &color, &min_thresh, &max_thresh, &drop_prob,
                                &gain, 0));

    /*
     * average queue size is reculated every 2(or 1)us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain) * gh2_wred_update_interval[unit];

    /* Get the total HOL limit first */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, port, 0, NULL,
                                    &local_port, NULL, &node));
        if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
            if ((cosq < -1) || (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }

            SOC_IF_ERROR_RETURN
                (READ_HOLCOSCELLMAXLIMIT_QGROUPr(unit, local_port,
                                                 cosq == -1 ? 0 : cosq, &rval));

            pkt_limit = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                                          rval, CELLMAXLIMITf);
        } else if (node->node_level == GH2_COSQ_LEVEL_QLAYER) {
            if ((cosq < -1) || (cosq >= SOC_GH2_QLAYER_COSQ_PER_QGROUP_NUM)) {
                return BCM_E_PARAM;
            }
            /* transfered to hw queue index (0~63) */
            local_cosq = node->qlayer_cosq_min + (cosq == -1 ? 0 : cosq);

            SOC_IF_ERROR_RETURN
                (READ_HOLCOSCELLMAXLIMIT_QLAYERr(unit, local_port,
                                                 local_cosq, &rval));
            pkt_limit = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                                          rval, CELLMAXLIMITf);
        } else {
            /* only accept scheduler gport type */
            return BCM_E_PORT;
        }
    } else {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                       &is_64q_port));
        }
        if (is_64q_port) {
            if ((cosq < -1) || (cosq >= SOC_GH2_2LEVEL_QUEUE_NUM)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (READ_HOLCOSCELLMAXLIMIT_QLAYERr(unit, local_port,
                                                 cosq == -1 ? 0 : cosq, &rval));
            pkt_limit = soc_reg_field_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                                          rval, CELLMAXLIMITf);
        } else {
            if ((cosq < -1) || (cosq >= SOC_GH2_LEGACY_QUEUE_NUM)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (READ_HOLCOSCELLMAXLIMITr(unit, local_port,
                                          cosq == -1 ? 0 : cosq, &rval));
            pkt_limit = soc_reg_field_get(unit, HOLCOSCELLMAXLIMITr,
                                          rval, CELLMAXLIMITf);
        }
    }
    if (min_thresh > pkt_limit) {
        min_thresh = pkt_limit;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        if (max_thresh > FL_CELL_FIELD_MAX) {
            max_thresh = FL_CELL_FIELD_MAX;
        }
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        if (max_thresh > pkt_limit) {
            max_thresh = pkt_limit;
        }
    }

    *drop_start = (min_thresh * 100 + pkt_limit - 1) / pkt_limit;

    /* Calculate the slope using the min and max threshold.
     * The angle is calculated knowing drop probability at min
     * threshold is 0% and drop probability at max threshold is 100%.
     */
    *drop_slope = bcmi_gh2_cells_to_angle(max_thresh - min_thresh);
    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_wred_port_control_set(int unit,
                                    bcm_gport_t gport,
                                    bcm_port_control_t type,
                                    int value)
{
    bcm_port_t local_port = 0;
    int is_64q_port = 0;
    uint32 val = 0;
    uint32 fld_val = 0;
    int cur_val = 0;
    int shift = 0;
    gh2_cosq_node_t *node = NULL;

    switch (type) {
        case bcmPortControlWredDropCountUpdateEnableQueueMask:
            /* parameter checking */
            if (value & 0xffffff00) {
                /* only 0xff is accepted */
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_SET(gport)) {
                /* To configure 64Q by QLayer scheduler */
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PORT;
                }
                if (BCM_GPORT_IS_SCHEDULER(gport)) {
                    BCM_IF_ERROR_RETURN
                        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                                &local_port, NULL, &node));
                    if (node->node_level != GH2_COSQ_LEVEL_QLAYER) {
                        return BCM_E_PORT;
                    }

                    if (node->child_idx < 4) {
                        shift = 8 * node->child_idx;
                        BCM_IF_ERROR_RETURN(
                            READ_WRED_DROP_CTR_CONFIG_QLAYER_0r(unit,
                                                                local_port,
                                                                &val));
                        fld_val = \
                            soc_reg_field_get(unit,
                                              WRED_DROP_CTR_CONFIG_QLAYER_0r,
                                              val, QUEUE_MASKf);
                        fld_val &= ~(0xff << shift);
                        fld_val |= ((value & 0xff) << shift);
                        soc_reg_field_set(unit, WRED_DROP_CTR_CONFIG_QLAYER_0r,
                                          &val,
                                          QUEUE_MASKf, fld_val);
                        BCM_IF_ERROR_RETURN(
                            WRITE_WRED_DROP_CTR_CONFIG_QLAYER_0r(unit,
                                                                 local_port,
                                                                 val));
                    } else {
                        shift = 8 * (node->child_idx - 4);
                        BCM_IF_ERROR_RETURN(
                            READ_WRED_DROP_CTR_CONFIG_QLAYER_1r(unit,
                                                                local_port,
                                                                &val));
                        fld_val = \
                            soc_reg_field_get(unit,
                                              WRED_DROP_CTR_CONFIG_QLAYER_1r,
                                              val, QUEUE_MASKf);
                        fld_val &= ~(0xff << shift);
                        fld_val |= ((value & 0xff) << shift);
                        soc_reg_field_set(unit, WRED_DROP_CTR_CONFIG_QLAYER_1r,
                                          &val,
                                          QUEUE_MASKf, fld_val);
                        BCM_IF_ERROR_RETURN(
                            WRITE_WRED_DROP_CTR_CONFIG_QLAYER_1r(unit,
                                                                 local_port,
                                                                 val));
                    }
                }
            } else {
                /* Legacy COSQ mode, 8 queues per port */
                local_port = gport;
                if (!SOC_PORT_VALID(unit, gport)) {
                    return BCM_E_PORT;
                }

#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit,
                                                   local_port, &is_64q_port));
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit,
                                                   local_port, &is_64q_port));
                }
                if (is_64q_port) {
                    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                        WRED_DROP_CTR_CONFIG_QLAYER_0r,
                                        local_port, QUEUE_MASKf, value));
                } else {
                    BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                        WRED_DROP_CTR_CONFIGr,
                                        local_port, QUEUE_MASKf, value));
                }
            }
            break;
        case bcmPortControlWredDropCountUpdateEnableColorMask:
            if (BCM_GPORT_IS_SET(gport)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_local_get(unit, gport, &local_port));
            } else {
                local_port = gport;
                if (!SOC_PORT_VALID(unit, local_port)) {
                    return BCM_E_PORT;
                }
            }
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit,
                                               local_port, &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit,
                                               local_port, &is_64q_port));
            }
            /* parameter checking */
            if (value & ~((0x1 << bcmColorGreen) |
                (0x1 << bcmColorYellow) |
                (0x1 << bcmColorRed))) {
                return BCM_E_PARAM;
            }
            cur_val = 0;
            if (value & (0x1 << bcmColorGreen)) {
                cur_val |= 0x4;
            }
            if (value & (0x1 << bcmColorYellow)) {
                cur_val |= 0x2;
            }
            if (value & (0x1 << bcmColorRed)) {
                cur_val |= 0x1;
            }
            if (is_64q_port) {
                BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    WRED_DROP_CTR_CONFIG_COLOR_MASK_QLAYERr,
                                    local_port, COLOR_MASKf, cur_val));
            } else {
                BCM_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                    WRED_DROP_CTR_CONFIGr,
                                    local_port, COLOR_MASKf, cur_val));
            }
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
bcmi_gh2_cosq_wred_port_control_get(int unit,
                                    bcm_gport_t gport,
                                    bcm_port_control_t type,
                                    int *value)
{
    bcm_port_t local_port = 0;
    int is_64q_port = 0;
    uint32 val = 0;
    uint32 fld_val = 0;
    int shift = 0;
    gh2_cosq_node_t *node = NULL;

    switch (type) {
        case bcmPortControlWredDropCountUpdateEnableQueueMask:
            if (BCM_GPORT_IS_SET(gport)) {
                /* To configure 64Q by QLayer scheduler */
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PORT;
                }
                if (BCM_GPORT_IS_SCHEDULER(gport)) {
                    BCM_IF_ERROR_RETURN
                        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                                &local_port, NULL, &node));
                    if (node->node_level != GH2_COSQ_LEVEL_QLAYER) {
                        return BCM_E_PORT;
                    }

                    if (node->child_idx < 4) {
                        shift = 8 * node->child_idx;
                        BCM_IF_ERROR_RETURN(
                            READ_WRED_DROP_CTR_CONFIG_QLAYER_0r(unit,
                                                                local_port,
                                                                &val));
                        fld_val = \
                            soc_reg_field_get(unit,
                                              WRED_DROP_CTR_CONFIG_QLAYER_0r,
                                              val, QUEUE_MASKf);
                        *value = (fld_val >> shift) & 0xff;
                    } else {
                        shift = 8 * (node->child_idx - 4);
                        BCM_IF_ERROR_RETURN(
                            READ_WRED_DROP_CTR_CONFIG_QLAYER_1r(unit,
                                                                local_port,
                                                                &val));
                        fld_val =
                            soc_reg_field_get(unit,
                                              WRED_DROP_CTR_CONFIG_QLAYER_1r,
                                              val, QUEUE_MASKf);
                        *value = (fld_val >> shift) & 0xff;
                    }
                }
            } else {
                /* Legacy COSQ mode, 8 queues per port */
                local_port = gport;
                if (!SOC_PORT_VALID(unit, gport)) {
                    return BCM_E_PORT;
                }

#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit,
                                                   local_port, &is_64q_port));
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit,
                                                   local_port, &is_64q_port));
                }
                if (is_64q_port) {
                    BCM_IF_ERROR_RETURN(
                        READ_WRED_DROP_CTR_CONFIG_QLAYER_0r(unit,
                                                            local_port,
                                                            &val));
                    fld_val = soc_reg_field_get(unit,
                                                WRED_DROP_CTR_CONFIG_QLAYER_0r,
                                                val, QUEUE_MASKf);
                    *value = fld_val & 0xff;
                } else {
                    BCM_IF_ERROR_RETURN(
                        READ_WRED_DROP_CTR_CONFIGr(unit, local_port, &val));
                    fld_val = soc_reg_field_get(unit, WRED_DROP_CTR_CONFIGr,
                                                val, QUEUE_MASKf);
                    *value = fld_val & 0xff;
                }
            }
            break;
        case bcmPortControlWredDropCountUpdateEnableColorMask:
            if (BCM_GPORT_IS_SET(gport)) {
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_local_get(unit, gport, &local_port));
            } else {
                local_port = gport;
                if (!SOC_PORT_VALID(unit, local_port)) {
                    return BCM_E_PORT;
                }
            }
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit,
                                               local_port, &is_64q_port));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit,
                                               local_port, &is_64q_port));
            }
            if (is_64q_port) {
                BCM_IF_ERROR_RETURN(
                    READ_WRED_DROP_CTR_CONFIG_COLOR_MASK_QLAYERr(unit,
                                                                 local_port,
                                                                 &val));
                fld_val = \
                    soc_reg_field_get(unit,
                                      WRED_DROP_CTR_CONFIG_COLOR_MASK_QLAYERr,
                                      val, COLOR_MASKf);
            } else {
                BCM_IF_ERROR_RETURN(
                    READ_WRED_DROP_CTR_CONFIGr(unit, local_port, &val));
                fld_val = soc_reg_field_get(unit, WRED_DROP_CTR_CONFIGr,
                                            val, COLOR_MASKf);
            }
            *value = 0;
            if (fld_val & 0x4) {
                *value |= (0x1 << bcmColorGreen);
            }
            if (fld_val & 0x2) {
                *value |= (0x1 << bcmColorYellow);
            }
            if (fld_val & 0x1) {
                *value |= (0x1 << bcmColorRed);
            }
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_add
 * Purpose:
 *     Create a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     port  - (IN) port number
 *     numq  - (IN) number of COS queues
 *     flags - (IN) flags (BCM_COSQ_GPORT_XXX)
 *     gport - (OUT) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                        bcm_gport_t *gport)
{
    bcm_port_t local_port;
    int phy_port, mmu_port;
    uint32 sched_encap;
    gh2_mmu_info_t *mmu_info;
    gh2_cosq_node_t *node = NULL;
    int id;
    int is_64q_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_gport_add: unit=%d port=0x%x \
                         numq=%d flags=0x%x\n"),
              unit, port, numq, flags));

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                   &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                   &is_64q_port));
    }

    if (!is_64q_port) {
        /* Only MMU port 58-65 support */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                  "bcmi_gh2_cosq_gport_add: Only MMU port 58-65 support\n")));
        return BCM_E_PORT;
    }

    mmu_info = gh2_mmu_info[unit];

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            /* to create the gport object of the 64 queues */
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            for (id = 0;
                 id < GH2_NUM_TOTAL_QUEUES; id++) {
                if (mmu_info->queue_node[id].in_use == FALSE) {
                    node = &mmu_info->queue_node[id];
                    node->in_use = TRUE;
                    break;
                }
            }
            if (!node) {
                return BCM_E_RESOURCE;
            }

            node = &mmu_info->queue_node[id];
            node->node_id = id;
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
            node->node_gport = *gport;
            node->node_level = GH2_COSQ_LEVEL_QUEUE;
            node->max_num_child = 1;
            break;
    case BCM_COSQ_GPORT_SCHEDULER:
        /* passthru */
    case BCM_COSQ_GPORT_WITH_ID:
    case (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_WITH_ID):
    case 0:
        if (numq < -1) {
            return BCM_E_PARAM;
        }

        /*
            * 80 scheduler nodes,
            * the first 8 are for each MMU port
            * the others are for QGROUP and QLAYER
            */
        if ((flags & BCM_COSQ_GPORT_WITH_ID) || (flags == 0)) {
            /* this is a Port level scheduler */
            if (numq > 1) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "bcmi_gh2_cosq_gport_add: \
                                     only one QGroup scheduler can be \
                                     attached to egress port\n")));
                return BCM_E_UNAVAIL;
            }

            phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            id = (mmu_port - SOC_GH2_64Q_MMU_PORT_IDX_MIN);

            if ( id < 0 || id >= GH2_NUM_PORT_SCHEDULERS) {
                return BCM_E_PARAM;
            }

            node = &mmu_info->sched_node[id];
            node->node_id = id;
            sched_encap = (id << 8) | local_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->node_gport = *gport;
            node->node_level = GH2_COSQ_LEVEL_PORT;
            node->max_num_child = 1;
            node->in_use = TRUE;
        } else {
            /* For QGROUP or QLAYER level scheduler */
            if (numq > 8) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "bcmi_gh2_cosq_gport_add: \
                                     only 8 member scheduler can be \
                                     attached to QGroup scheduler or \
                                     QLayer scheduler\n")));
                return BCM_E_UNAVAIL;
            }

            for (id = GH2_NUM_QGROUP_SCHEDULERS;
                 id < GH2_NUM_TOTAL_SCHEDULERS; id++) {
                if (mmu_info->sched_node[id].in_use == FALSE) {
                    node = &mmu_info->sched_node[id];
                    node->in_use = TRUE;
                    break;
                }
            }
            if (!node) {
                return BCM_E_RESOURCE;
            }

            node = &mmu_info->sched_node[id];
            node->node_id = id;
            sched_encap = (id << 8) | local_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->node_gport = *gport;
            node->max_num_child = numq;
       }
       break;

    default:
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                       gport=0x%x\n"),
              *gport));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    gh2_cosq_node_t *node;
    gh2_cosq_node_t *node_child;
    int local_port;
    int i;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_gport_delete: unit=%d gport=0x%x\n"),
                         unit, gport));

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    node = NULL;

    if (!(BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL,
                                &local_port, NULL, &node));

    if (node == NULL) {
        return BCM_E_INTERNAL;
    }

    if (node->node_level == GH2_COSQ_LEVEL_PORT) {
        /* detach child (QGroup) if existed */
        if (node->child_gport[0] != -1) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_node_get(unit, node->child_gport[0],
                                        0, NULL, NULL, NULL, &node_child));
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_detach(unit, node_child->node_gport,
                                            node->node_gport, 0));
        }
    } else if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
        /* detach all child (QLayer) if existed */
        for (i = 0; i < node->num_child; i++) {
            if (node->child_gport[i] != -1) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_node_get(unit, node->child_gport[i],
                                            0, NULL, NULL, NULL,
                                            &node_child));
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_gport_detach(unit, node_child->node_gport,
                                                node->node_gport,
                                                node_child->child_idx));
            }
        }
        /* detach from parent */
        if (node->parent_gport != -1) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_detach(unit, node->node_gport,
                                            node->parent_gport, 0));
        }
    } else if (node->node_level == GH2_COSQ_LEVEL_QLAYER) {
        /* detach all child (QLayer) if existed */
        for (i = 0; i < node->num_child; i++) {
            if (node->child_gport[i] != -1) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_node_get(unit, node->child_gport[i],
                                            0, NULL, NULL, NULL,
                                            &node_child));
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_gport_detach(unit, node_child->node_gport,
                                                node->node_gport,
                                                node_child->child_idx));
            }
        }

        /* detach parent (QGroup) */
        if (node->parent_gport != -1) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_detach(unit, node->node_gport,
                                            node->parent_gport,
                                            node->child_idx));
        }
    } else { /* GH2_COSQ_LEVEL_QUEUE */
        /* detach parent (QLayer) */
        if (node->parent_gport != -1) {
            BCM_IF_ERROR_RETURN
                (bcmi_gh2_cosq_gport_detach(unit, node->node_gport,
                                            node->parent_gport,
                                            node->child_idx));
        }
    }
    /*_BCM_GH2_COSQ_NODE_INIT(node); */
    bcmi_gh2_cosq_node_t_init(node);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_traverse
 * Purpose:
 *     Walks through the valid COSQ GPORTs and calls
 *     the user supplied callback function for each entry.
 * Parameters:
 *     unit       - (IN) unit number
 *     trav_fn    - (IN) Callback function.
 *     user_data  - (IN) User data to be passed to callback function
 * Returns:
 *     BCM_E_NONE - Success.
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                             void *user_data)
{
    gh2_mmu_info_t *mmu_info;
    gh2_cosq_node_t *node;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = BCM_E_NONE;
    int i;
    int numq;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = gh2_mmu_info[unit];

    for (i = 0 ; i < GH2_NUM_TOTAL_SCHEDULERS; i++) {
        node = &mmu_info->sched_node[i];
        if (node->in_use) {
            flags = BCM_COSQ_GPORT_SCHEDULER;
            if (node->node_level == GH2_COSQ_LEVEL_PORT) {
                numq = 1;
            } else if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
                numq = node->num_child;
            } else { /* GH2_COSQ_LEVEL_QLAYER */
                numq = 8;
            }
            rv = cb(unit, node->node_gport, numq, flags,
                    node->node_gport, user_data);
           if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
               if (SOC_CB_ABORT_ON_ERR(unit)) {
                   return rv;
               }
#endif
           }
        }
    }

    for (i = 0 ; i < GH2_NUM_TOTAL_QUEUES; i++) {
        node = &mmu_info->queue_node[i];
        if (node->in_use) {
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
            numq = 1;
            rv = cb(unit, node->node_gport, numq, flags,
                      node->node_gport, user_data);
           if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
               if (SOC_CB_ABORT_ON_ERR(unit)) {
                   return rv;
               }
#endif
           }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_get
 * Purpose:
 *     Get a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 *     port  - (OUT) port number
 *     numq  - (OUT) number of COS queues
 *     flags - (OUT) flags (BCM_COSQ_GPORT_XXX)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                        int *numq, uint32 *flags)
{
    gh2_cosq_node_t *node = NULL;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcmi_gh2_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    if (!(BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL, &local_port, &id, &node));

    if (SOC_USE_GPORT(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        dest.modid = modid;
        dest.port = local_port;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, port));
    } else {
        *port = local_port;
    }

    if (node->node_level == GH2_COSQ_LEVEL_PORT) {
        *flags = 0;
    } else if (node->node_level == GH2_COSQ_LEVEL_QUEUE) {
        *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    } else { /* GH2_COSQ_LEVEL_QGROUP or GH2_COSQ_LEVEL_QLAYER */
        *flags = BCM_COSQ_GPORT_SCHEDULER;
    }
    *numq = node->max_num_child;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                     port=0x%x numq=%d flags=0x%x\n"),
                         *port, *numq, *flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_attach
 * Purpose:
 *     Attach sched_port to the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to attach to
 *     cosq       - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_attach(int unit, bcm_gport_t gport,
                           bcm_gport_t to_gport,
                           bcm_cos_queue_t to_cosq)
{
    gh2_cosq_node_t *node, *to_node, *temp_node;
    bcm_port_t port, to_port;
    int i;
    int child_index = 0;
    uint32 reg_val = 0;
    int id;
    int phy_port, mmu_port;
    gh2_mmu_info_t *mmu_info;


    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    /*
     * Three kinds of attach supported
     * 1.QGROUP scheduler(gport) attaches to egress port(to_gport)
     *    egress port(to_gport) here can be local port id or gport id
     * 2.QLAYER scheduler(gport) attaches to QGROUP scheduler(to_gport)
     * 3.Queue node(gport) attaches to QLAYER scheduler(to_gport)
     */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(to_gport)) {
        return BCM_E_PARAM;
    }

    if (!(BCM_GPORT_IS_SCHEDULER(to_gport) ||
          BCM_GPORT_IS_LOCAL(to_gport) ||
          BCM_GPORT_IS_MODPORT(to_gport))) {
        return BCM_E_PORT;
    }

    if (!(BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))) {
        return BCM_E_PORT;
    }

    if (to_cosq < -1) {
        return BCM_E_PARAM;
    }

    to_node = NULL;
    if (BCM_GPORT_IS_SCHEDULER(to_gport)) {
        /*
         * attach cases :
         * 1.QGROUP scheduler(gport) attaches to egress port(to_gport)
         * 2.QLAYER scheduler(gport) attaches to QGROUP scheduler(to_gport)
         * 3.Queue node(gport) attaches to QLAYER scheduler(to_gport)
         */
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, to_gport, 0, NULL, &to_port, NULL,
                                    &to_node));
    } else {
        /* to_gport is local port */

        /* QGROUP scheduler(gport) attaches to egress port(to_gport) */
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, to_gport, &to_port));

        /* Does it ever created PORT scheduler?*/
        phy_port = SOC_INFO(unit).port_l2p_mapping[to_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        id = (mmu_port - SOC_GH2_64Q_MMU_PORT_IDX_MIN);

        if ( id < 0 || id >= GH2_NUM_PORT_SCHEDULERS) {
            return BCM_E_PARAM;
        }

        mmu_info = gh2_mmu_info[unit];
        temp_node = &mmu_info->sched_node[id];
        if (temp_node->in_use) {
            to_node = temp_node;
        }
    }

    if (to_node != NULL) {
        if (to_node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* only 1 QGROUP scheduler can be attached to egress port(gport) */
            if (to_cosq >= 1) {
                return BCM_E_PARAM;
            }
        } else {
            /*
             * Only 8 QLAYER scheduler can be attached to QGROUP scheduler
             * Or, only 8 queue can be attached to QLAYER scheduler
             */
            if (to_cosq >= 8) {
                return BCM_E_PARAM;
            }
        }
    } else {
        /* only 1 QGROUP scheduler can be attached to egress port(gport) */
        if (to_cosq >= 1) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL, &port, NULL, &node));

    if (port != to_port) {
        return BCM_E_PORT;
    }

    if (node->parent_gport != -1) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }

    if (to_node != NULL) {
        if (to_node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* QGROUP scheduler(gport) attaches to egress port(to_gport) */
            to_node->child_gport[0] = gport;
            to_node->num_child = 1;

            node->parent_gport = to_gport;
            node->node_level = GH2_COSQ_LEVEL_QGROUP;

            /* enable two layer scheduler mode */
            READ_TWO_LAYER_SCH_MODEr(unit, to_port, &reg_val);
            soc_reg_field_set(unit, TWO_LAYER_SCH_MODEr, &reg_val,
                              SCH_MODEf,  1);
            WRITE_TWO_LAYER_SCH_MODEr(unit, to_port, reg_val);
        } else {
            /* QLAYER scheduler(gport) attaches to QGROUP scheduler(to_gport)
             * or
             * Queue node(gport) attaches to QLAYER scheduler(to_gport)
             */
            if (to_cosq == -1) {
                /* find the first available index */
                for (i = 0; i < to_node->max_num_child; i++) {
                    if (to_node->child_gport[i] != BCM_GPORT_INVALID) {
                        child_index = i;
                    }
                }
            } else {
                child_index = to_cosq;
            }

            to_node->child_gport[child_index] = gport;
            to_node->num_child++;
            node->parent_gport = to_gport;
            node->child_idx = child_index;
            if (to_node->node_level == GH2_COSQ_LEVEL_QGROUP) {
                node->node_level = GH2_COSQ_LEVEL_QLAYER;
                node->qlayer_cosq_min = (child_index * 8);
                node->qlayer_cosq_max = ((child_index + 1) * 8) -1;
            }
        }
    } else {
        /* QGROUP scheduler(gport) attaches to egress port(to_gport) */
        node->parent_gport = to_gport;
        node->node_level = GH2_COSQ_LEVEL_QGROUP;

        /* enable two layer scheduler mode */
        READ_TWO_LAYER_SCH_MODEr(unit, to_port, &reg_val);
        soc_reg_field_set(unit, TWO_LAYER_SCH_MODEr, &reg_val,
                  SCH_MODEf,  1);
        WRITE_TWO_LAYER_SCH_MODEr(unit, to_port, reg_val);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_gh2_cosq_gport_attach_get
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport - (IN) scheduler GPORT identifier
 *     input_gport - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmi_gh2_cosq_gport_attach_get(int unit, bcm_gport_t gport,
                               bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    gh2_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t port;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (input_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, &modid, &port, NULL,
                                &node));

    if (node->parent_gport == -1) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (node->node_level == GH2_COSQ_LEVEL_PORT) {
        /* PORT level scheduler has no parent */
        return BCM_E_PORT;
    } else if (node->node_level == GH2_COSQ_LEVEL_QGROUP) {
        *cosq = 0;
    } else {
        /* GH2_COSQ_LEVEL_QLAYER or  GH2_COSQ_LEVEL_QUEUE */
        *cosq = node->child_idx;
    }
    *input_gport = node->parent_gport;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_gport_child_get
 * Purpose:
 *      Get the child node GPORT atatched to N-th index (cosq)
 *      of the scheduler GPORT.
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      in_gport   - (IN)  Scheduler GPORT ID.
 *      cosq       - (IN)  COS queue attached to.
 *      out_gport  - (OUT) child GPORT ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_gh2_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                              bcm_cos_queue_t cosq,
                              bcm_gport_t *out_gport)
{
    gh2_cosq_node_t *in_node = NULL;
    bcm_module_t in_modid = 0;
    bcm_port_t in_port = -1;
    int phy_port, mmu_port;
    int id;
    gh2_mmu_info_t *mmu_info;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = gh2_mmu_info[unit];

    if (out_gport == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(in_gport) && !BCM_GPORT_IS_SCHEDULER(in_gport)) {
        return BCM_E_PORT;
    }

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SET(in_gport)) {
        /* this is a PORT level scheduler */
        /* only 1 QGROUP scheduler can be attached to egress port */
        if (cosq >= 64) {
            return BCM_E_PARAM;
        }
        if (cosq >= 1) {
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, in_gport, &in_port));
        phy_port = SOC_INFO(unit).port_l2p_mapping[in_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        id = (mmu_port - SOC_GH2_64Q_MMU_PORT_IDX_MIN);

        if ( id < 0 || id >= GH2_NUM_PORT_SCHEDULERS) {
            return BCM_E_PARAM;
        }
        in_node = &mmu_info->sched_node[id];
        if (in_node->in_use) {
            /* The child gport is not valid (not attached) */
            if (BCM_GPORT_INVALID == in_node->child_gport[0]) {
                return BCM_E_NOT_FOUND;
            }
            *out_gport =in_node->child_gport[0];
        } else {
            return BCM_E_NOT_FOUND;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, in_gport, 0, &in_modid, &in_port,
                                    NULL, &in_node));
        if (in_node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* this is a PORT level scheduler */
            /* only 1 QGROUP scheduler can be attached to egress port */
            if (cosq >= 64) {
                return BCM_E_PARAM;
            }
            if (cosq >= 1) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            if (cosq >= 8) {
                return BCM_E_PARAM;
            }
            }

        if (in_node->node_level == GH2_COSQ_LEVEL_QUEUE) {
            /* no child will be attached to the queue node */
            return BCM_E_PARAM;
        }

        /* The child gport is not valid (not attached) */
        if (BCM_GPORT_INVALID == in_node->child_gport[cosq]) {
            return BCM_E_NOT_FOUND;
        }

        /* Get the child gport */
        *out_gport = in_node->child_gport[cosq];
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_gh2_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport - (IN) scheduler GPORT identifier
 *     input_gport - (IN) GPORT to detach from
 *     cosq       - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_gh2_cosq_gport_detach(int unit, bcm_gport_t gport,
                           bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    gh2_cosq_node_t *node, *input_node = NULL, *temp_node = NULL;
    bcm_port_t port, input_port = -1;
    int child_index;
    uint32 reg_val;
    int id;
    int phy_port, mmu_port;
    gh2_mmu_info_t *mmu_info;

    if (gh2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    /*
     * Three kinds of attach/detach supported
     * 1.QGROUP scheduler(gport) attached to egress port(input_gport)
     * 2.QLAYER scheduler(gport) attached to QGROUP scheduler(input_gport)
     * 3.Queue node(gport) attached to QLAYER scheduler(input_gport)
     */
    if (!(BCM_GPORT_IS_SCHEDULER(input_gport) ||
        BCM_GPORT_IS_MODPORT(input_gport) ||
        BCM_GPORT_IS_LOCAL(input_gport))) {
        return BCM_E_PARAM;
    }
    if (!(BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))) {
        return BCM_E_PORT;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    input_node = NULL;
    if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
        /*
         * attached cases :
         * 1.QLAYER scheduler(gport) attached to QGROUP scheduler(input_gport)
         * 2.QGROUP scheduler(gport) attached to egress port(input_gport)
         * 3.Queue node(gport) attached to QLAYER scheduler(input_gport)
         */
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_node_get(unit, input_gport, 0, NULL, &input_port,
                                    NULL, &input_node));
    } else {
        /* input_gport is local port */

        /* QGROUP scheduler(gport) attached from egress port(input_gport) */
        BCM_IF_ERROR_RETURN
            (bcmi_gh2_cosq_localport_resolve(unit, input_gport, &input_port));

        /* Does it ever created PORT scheduler?*/
        phy_port = SOC_INFO(unit).port_l2p_mapping[input_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        id = (mmu_port - SOC_GH2_64Q_MMU_PORT_IDX_MIN);

        if ( id < 0 || id >= GH2_NUM_PORT_SCHEDULERS) {
            return BCM_E_PARAM;
        }

        mmu_info = gh2_mmu_info[unit];
        temp_node = &mmu_info->sched_node[id];
        if (temp_node->in_use) {
            input_node = temp_node;
        }
    }

    if (input_node != NULL) {
        if (input_node->node_level == GH2_COSQ_LEVEL_PORT) {
            /* only 1 QGROUP scheduler can be attached to egress port(gport) */
            if (cosq >= 1) {
                return BCM_E_PARAM;
            }
        } else {
            /* only 8 QLAYER scheduler can be attached to QGROUP scheduler */
            /* only 8 QLAYER scheduler can be attached to QGROUP scheduler */
            if (cosq >= 8) {
                return BCM_E_PARAM;
            }
        }
    } else {
        /* only 1 QGROUP scheduler can be attached to egress port(gport) */
        if (cosq >= 1) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (bcmi_gh2_cosq_node_get(unit, gport, 0, NULL, &port, NULL,
                                &node));

    if (port != input_port) {
        return BCM_E_PORT;
    }
    if (node->parent_gport == -1) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (input_node != NULL) {
        if (input_node->node_level == GH2_COSQ_LEVEL_PORT) {
            /*
             * QGROUP scheduler(gport) detaches from
             * PORT scheduler(input_gport)
             */
            input_node->child_gport[0] = BCM_GPORT_INVALID;
            input_node->num_child--;
            node->parent_gport = -1;
            node->child_idx = -1;
            node->qlayer_cosq_min = -1;
            node->qlayer_cosq_max = -1;

            /* disable two layer scheduler mode */
            READ_TWO_LAYER_SCH_MODEr(unit, input_port, &reg_val);
            soc_reg_field_set(unit, TWO_LAYER_SCH_MODEr, &reg_val,
                              SCH_MODEf,  0);
            WRITE_TWO_LAYER_SCH_MODEr(unit, input_port, reg_val);
        } else {
            /* QLAYER scheduler(gport) detaches from QGROUP scheduler(to_gport)
             * or
             * Queue node(gport) detaches from QLAYER scheduler(to_gport)
             */
            if (cosq == -1) {
                /* not specified, just get input node */
                child_index = node->child_idx;
            } else {
                /* detach cosq is not the same as recorded */
                if (node->child_idx != cosq) {
                    return BCM_E_PARAM;
                }
                child_index = cosq;
            }

            input_node->child_gport[child_index] = BCM_GPORT_INVALID;
            input_node->num_child--;
            node->parent_gport = -1;
            node->child_idx = -1;
            node->qlayer_cosq_min = -1;
            node->qlayer_cosq_max = -1;
        }
    } else {
        /* QGROUP scheduler(gport) detaches from egress port(input_gport) */
        node->parent_gport = -1;

        /* disable two layer scheduler mode */
        READ_TWO_LAYER_SCH_MODEr(unit, input_port, &reg_val);
        soc_reg_field_set(unit, TWO_LAYER_SCH_MODEr, &reg_val,
                  SCH_MODEf,  0);
        WRITE_TWO_LAYER_SCH_MODEr(unit, input_port, reg_val);
    }
    return BCM_E_NONE;
}
#ifndef BCM_SW_STATE_DUMP_DISABLE
static void
bcmi_gh2_gport_scheduler_dump(int unit, bcm_gport_t port)
{
    int rv;
    bcm_gport_t lvl0 = BCM_GPORT_INVALID;
    bcm_gport_t lvl1 = BCM_GPORT_INVALID;
    bcm_gport_t lvl2[8];
    int i, j;

    /* Get LVL0 scheduler */
    rv = bcmi_gh2_cosq_gport_child_get(unit, port, 0, &lvl0);
    if (BCM_FAILURE(rv)) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "     Port : %d\n"),
                        port));
    LOG_CLI((BSL_META_U(unit,
                        "       LVL-0 scheduler : 0x%x\n"),
                        lvl0));
    for (i = 0; i < 8; i++) {
        rv = bcmi_gh2_cosq_gport_child_get(unit, lvl0, i, &lvl1);
        if (BCM_FAILURE(rv)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                            "           LVL-1 scheduler : 0x%x\n"),
                            lvl1));
        LOG_CLI((BSL_META_U(unit,
                            "               LVL-2 queues : ")));
        for (j = 0; j < 8; j++) {
            lvl2[j] = BCM_GPORT_INVALID;
            rv = bcmi_gh2_cosq_gport_child_get(unit, lvl1, j, &lvl2[j]);
            if (BCM_SUCCESS(rv)) {
                LOG_CLI((BSL_META_U(unit,
                                    "0x%x "),
                                    lvl2[j]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    return;
}

/*
 * Function:
 *     bcmi_gh2_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcmi_gh2_cosq_sw_dump(int unit)
{
    int  i;

    LOG_CLI((BSL_META_U(unit,
            "\nSW Information COSQ - Unit %d\n"), unit));

    /* Number COSQ */
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), gh2_num_cosq[unit]));

    if (gh2_mmu_info[unit]) {
        bcm_pbmp_t ports;
        bcm_port_t port;
        int is_64q_port = 0;
        int rv = 0;

        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                rv = soc_fl_64q_port_check(unit, port, &is_64q_port);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = soc_gh2_64q_port_check(unit, port, &is_64q_port);
            }
            if (SOC_SUCCESS(rv) && is_64q_port) {
                bcmi_gh2_gport_scheduler_dump(unit, port);
            }
        }
    }

    /* COSQ Map profile */
    /* Display those entries with reference count > 0 */
    LOG_CLI((BSL_META_U(unit,
            "    COSQ Map Profile:\n")));
    if (gh2_cos_map_profile[unit] != NULL) {
        int     num_entries;
        int     ref_count;
        int     entries_per_set;
        uint32  index;
        port_cos_map_entry_t *entry_p;
        uint32  cosq, hg_cosq;

        num_entries = soc_mem_index_count
            (unit, gh2_cos_map_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            "        Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "        Index RefCount EntriesPerSet - "
                            "COS HG_COS\n")));

        for (index = 0; index < num_entries;
            index += GH2_COS_MAP_ENTRIES_PER_SET) {
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               gh2_cos_map_profile[unit],
                                               index, &ref_count))) {
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (i = 0; i < GH2_COS_MAP_ENTRIES_PER_SET; i++) {
                entries_per_set =
                    gh2_cos_map_profile[unit]->tables[0].
                        entries[index + i].entries_per_set;
                LOG_CLI((BSL_META_U(unit,
                        "       %5d %8d %13d    "),
                        index + i, ref_count, entries_per_set));

                entry_p = SOC_PROFILE_MEM_ENTRY(unit,
                                                gh2_cos_map_profile[unit],
                                                port_cos_map_entry_t *,
                                                index + i);
                cosq = soc_mem_field32_get(unit,
                                           PORT_COS_MAPm,
                                           entry_p,
                                           COSf);
                LOG_CLI((BSL_META_U(unit,
                                    "%2d "), cosq));
                if (soc_mem_field_valid(unit, PORT_COS_MAPm, HG_COSf)) {
                    hg_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm,
                                                  entry_p,
                                                  HG_COSf);
                    LOG_CLI((BSL_META_U(unit,
                                        "   %2d"), hg_cosq));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
        }
    }

    return;
}
#endif /*BCM_SW_STATE_DUMP_DISABLE */


/*
 * Function:
 *      bcmi_gh2_cosq_drop_status_enable_set
 * Purpose:
 *      Program E2ECC mode
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) port ID
 *      enable- (IN) enable or disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_gh2_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    uint32 rval;
    bcm_port_t local_port;
    int is_64q;

    COMPILER_REFERENCE(port);
    PBMP_E_ITER (unit, local_port) {
        BCM_IF_ERROR_RETURN(READ_E2ECC_PORT_CONFIGr(unit, local_port, &rval));
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_CELL_ENf,  enable ? 0xFF : 0x0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_PKT_ENf, enable ? 0xFF : 0x0);
        BCM_IF_ERROR_RETURN(WRITE_E2ECC_PORT_CONFIGr(unit, local_port, rval));
    }

    BCM_IF_ERROR_RETURN(READ_E2ECC_MODEr(unit, &rval));
    soc_reg_field_set(unit, E2ECC_MODEr, &rval, ENf,
                      enable ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_E2ECC_MODEr(unit, rval));

    is_64q = 0;
    PBMP_E_ITER (unit, local_port) {
#ifdef BCM_FIRELIGHT_SUPPORT
        if (soc_feature(unit, soc_feature_fl)) {
            BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, local_port,
                                                      &is_64q));
        } else
#endif /* BCM_FIRELIGHT_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, local_port,
                                                      &is_64q));
        }
        if (is_64q) {
            /* E2ECC does not support 2-level scheduling mode */
            /* Apply the register setting to the first 8 COSQ */
            BCM_IF_ERROR_RETURN(
                READ_COLOR_DROP_EN_QLAYERr(unit, local_port, 0, &rval));
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                              COLOR_DROP_ENf,  enable ? 0x0 : 0xff);
            BCM_IF_ERROR_RETURN(
                WRITE_COLOR_DROP_EN_QLAYERr(unit, local_port, 0, rval));
        } else {
            BCM_IF_ERROR_RETURN(READ_COLOR_DROP_ENr(unit, local_port, &rval));
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                              COLOR_DROP_ENf,  enable ? 0x0 : 0xff);
            BCM_IF_ERROR_RETURN(WRITE_COLOR_DROP_ENr(unit, local_port, rval));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_pfc_class_resolve
 * Purpose:
 *      Resolve the hw class index from bcmSwitchPFCClassNQueue types
 * Parameters:
 *      sctype- (IN) bcmSwitchPFCClassNQueue type.
 *      class - (OUT) hw class index.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
bcmi_gh2_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *class)
{
    if (class == NULL) {
        return BCM_E_PARAM;
    }

    switch (sctype) {
    case bcmSwitchPFCClass7Queue:
        *class = 7;
        break;
    case bcmSwitchPFCClass6Queue:
        *class = 6;
        break;
    case bcmSwitchPFCClass5Queue:
        *class = 5;
        break;
    case bcmSwitchPFCClass4Queue:
        *class = 4;
        break;
    case bcmSwitchPFCClass3Queue:
        *class = 3;
        break;
    case bcmSwitchPFCClass2Queue:
        *class = 2;
        break;
    case bcmSwitchPFCClass1Queue:
        *class = 1;
        break;
    case bcmSwitchPFCClass0Queue:
        *class = 0;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_pfc_op
 * Purpose:
 *      Set or clear the cosq id mapped to the bcmSwitchPFCClassNQueue type
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) port ID
 *      sctype- (IN) bcmSwitchPFCClassNQueue type.
 *      op    - (IN) opcode, add or clear
 *      cosq  - (IN) cosq id.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_gh2_cosq_port_pfc_op(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t sctype,
    _bcm_cosq_op_t op,
    int cosq)
{
    int class;
    uint32 class_bmp;
    int i;
    int is_64q_port = 0;
    gh2_cosq_node_t *node = NULL;
    int index;
    int local_port;
    int regidx_max = SOC_REG_NUMELS(unit, PRI2COS_PFCr);

    BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_pfc_class_resolve(sctype, &class));
#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port, &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port, &is_64q_port));
    }
    if (is_64q_port) {
        if (BCM_GPORT_IS_SET(cosq)) {
            /* PFC class mapped to QG0~7(cosq is QLayer scheduler gport id) */
            if (BCM_GPORT_IS_SCHEDULER(cosq)) {
                BCM_IF_ERROR_RETURN
                    (bcmi_gh2_cosq_node_get(unit, cosq, 0, NULL,
                                            &local_port, NULL, &node));
                if (local_port != port) {
                    /* The scheduler is not belong to the port */
                    return BCM_E_PARAM;
                }
                if (node->node_level != GH2_COSQ_LEVEL_QLAYER) {
                    /* Only available on QLayer scheduler */
                    return BCM_E_PARAM;
                }
                /* Get the QGx of the QLayer scheduler*/
                index = node->child_idx;
            } else {
                /* Only available on QLayer scheduler */
                return BCM_E_PARAM;
            }
        } else {
            index = cosq;
        }
    } else {
        index = cosq;
    }

    if (op == _BCM_COSQ_OP_CLEAR) {
        for (i = 0; i < regidx_max; i++) {
            BCM_IF_ERROR_RETURN(READ_PRI2COS_PFCr(unit, port,
                                i, &class_bmp));
            class_bmp &= ~(1 << class);
            BCM_IF_ERROR_RETURN(WRITE_PRI2COS_PFCr(unit, port,
                                i, class_bmp));
        }
    } else if (op == _BCM_COSQ_OP_ADD) {
        if ((index < 0) || index > NUM_COS(unit)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(READ_PRI2COS_PFCr(unit, port, index, &class_bmp));
        class_bmp |= (1 << class);
        BCM_IF_ERROR_RETURN(WRITE_PRI2COS_PFCr(unit, port, index, class_bmp));
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_gh2_cosq_port_pfc_get
 * Purpose:
 *      Get the cosq id mapped to the bcmSwitchPFCClassNQueue type
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) port ID
 *      sctype- (IN) bcmSwitchPFCClassNQueue type.
 *      cosq  - (OUT) cosq id mapped.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_gh2_cosq_port_pfc_get(
    int unit,
    bcm_port_t port,
    bcm_switch_control_t sctype,
    int *cosq)
{
    int class;
    uint32 class_bmp;
    int i;
    int is_64q_port = 0;
    bcm_gport_t lvl0_node_gport = 0;
    bcm_gport_t lvl1_node_gport = 0;
    int regidx_max = SOC_REG_NUMELS(unit, PRI2COS_PFCr);
    int rv = BCM_E_NONE;

    if (cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcmi_gh2_cosq_pfc_class_resolve(sctype, &class));

    for (i = 0; i < regidx_max; i++) {
        BCM_IF_ERROR_RETURN(READ_PRI2COS_PFCr(unit, port, i, &class_bmp));
        if (class_bmp & (1 << class)) {
            break;
        }
    }

    if (i >= regidx_max) {
        return BCM_E_NOT_FOUND;
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (soc_feature(unit, soc_feature_fl)) {
        BCM_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, port, &is_64q_port));
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(soc_gh2_64q_port_check(unit, port, &is_64q_port));
    }
    if (is_64q_port) {
        /* Get the lvl0 scheduler(QGroup scheduler) of the port */
        rv = bcmi_gh2_cosq_gport_child_get(unit, port, 0,
                                           &lvl0_node_gport);
        if (SOC_SUCCESS(rv)) {
        /*
             * Get the i-th lvl1 scheduler(QLayer scheduler) of the
             * lvl0 scheduler
         */
        BCM_IF_ERROR_RETURN(
            bcmi_gh2_cosq_gport_child_get(unit, lvl0_node_gport, i,
                                          &lvl1_node_gport));
        *cosq = lvl1_node_gport;
        } else if (rv == BCM_E_NOT_FOUND) {
            /* cannot find child gport, it is legacy mode */
            *cosq = i;
        } else {
            /* return for other errors */
            return rv;
        }
    } else {
        *cosq = i;
    }

    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND2_SUPPORT */

