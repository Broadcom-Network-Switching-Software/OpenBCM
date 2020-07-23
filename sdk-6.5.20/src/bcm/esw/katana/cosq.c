/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#if defined(BCM_KATANA_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/katana.h>
#include <soc/macutil.h>
#include <soc/profile_mem.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/multicast.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm/types.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
#include <soc/katana2.h>
#endif

#define _BCM_COSQ_PARENT_BMAP_SET(bmap, bit)          \
    if (bmap != NULL) {                               \
        SHR_BITSET(bmap, bit);                        \
    } 

#define _BCM_COSQ_PARENT_BMAP_CLR(bmap, bit)          \
    if (bmap != NULL) {                               \
        SHR_BITCLR(bmap, bit);                        \
    }                                                       

#define _BCM_KT_IS_UC_QUEUE(mmu_info,qid)   \
         (((qid) < mmu_info->num_base_queues) ? 1 : 0)
    

#define KT_CELL_FIELD_MAX       0x3ffff

#define KT_QUEUE_FLUSH_BURST_MANTISSA   124
#define KT_QUEUE_FLUSH_BURST_EXP        14
#define KT_QUEUE_FLUSH_CYCLE_SEL        0

/* Sum of all of max bandwidth configured on all children
 * nodes should be less than or equal to the parent node's
 * max bandwidth. Hence using 90% of parent's node BW and dividing
 * it by the num of children nodes when configuring the children nodes
 * shpaer config.
 */
#define KT_COSQ_SHAPER_PERC             90

/* MMU cell size in bytes */
#define _BCM_KT_COSQ_CELLSIZE   192
#define _BCM_KT_COSQ_EXT_CELLSIZE   768

#define _BCM_KT_NUM_UCAST_QUEUE_GROUP            8
#define _BCM_KT_NUM_VLAN_UCAST_QUEUE_GROUP       16

/* ==========Separate file for KT2 is surely required now ===== */
/* For KT: #define _BCM_KT_NUM_PORT_SCHEDULERS              36  */
/* For KT2: #define _BCM_KT_NUM_PORT_SCHEDULERS              42 */
/* ============================================================ */
#define _BCM_KT_NUM_PORT_SCHEDULERS              42
#define _BCM_KT_NUM_L0_SCHEDULER                 256
#define _BCM_KT_NUM_L1_SCHEDULER                 1024
#define _BCM_KT_NUM_L2_QUEUES                    4096
#define _BCM_KT_NUM_TOTAL_SCHEDULERS             (_BCM_KT_NUM_PORT_SCHEDULERS + \
                                                  _BCM_KT_NUM_L0_SCHEDULER +    \
                                                  _BCM_KT_NUM_L1_SCHEDULER)

#define _BCM_KT_COSQ_LIST_NODE_INIT(list, node)         \
    list[node].in_use            = FALSE;               \
    list[node].wrr_in_use        = 0;                   \
    list[node].gport             = -1;                  \
    list[node].base_index        = -1;                  \
    list[node].numq              = 0;                   \
    list[node].hw_index          = -1;                  \
    list[node].level             = -1;                  \
    list[node].cosq_attached_to  = -1;                  \
    list[node].num_child         = 0;                   \
    list[node].first_child       = 4095;                \
    list[node].cosq_map          = NULL;                \
    list[node].parent            = NULL;                \
    list[node].sibling           = NULL;                \
    list[node].child             = NULL;

#define _BCM_KT_COSQ_NODE_INIT(node)                    \
    node->in_use            = FALSE;                    \
    node->wrr_in_use        = 0;                        \
    node->gport             = -1;                       \
    node->base_index        = -1;                       \
    node->numq              = 0;                        \
    node->hw_index          = -1;                       \
    node->level             = -1;                       \
    node->cosq_attached_to  = -1;                       \
    node->num_child         = 0;                        \
    node->first_child       = 4095;                     \
    node->cosq_map          = NULL;                     \
    node->parent            = NULL;                     \
    node->sibling           = NULL;                     \
    node->child             = NULL;

typedef enum {
    _BCM_KT_COSQ_STATE_NO_CHANGE,
    _BCM_KT_COSQ_STATE_DISABLE,
    _BCM_KT_COSQ_STATE_ENABLE,
    _BCM_KT_COSQ_STATE_SPRI_TO_WRR,
    _BCM_KT_COSQ_STATE_WRR_TO_SPRI,
    _BCM_KT_COSQ_STATE_MAX
}_bcm_kt_cosq_state_t;

typedef struct _bcm_kt_cosq_list_s {
    int count;
    SHR_BITDCL *bits;
}_bcm_kt_cosq_list_t;

typedef struct _bcm_kt_cosq_port_info {
    int q_offset;
    int q_limit;
}_bcm_kt_cosq_port_info_t;

typedef struct _bcm_kt_mmu_info_s {
    int num_base_queues;   /* Number of classical queues */
    int num_ext_queues;    /* Number of extended queues */
    int num_sub_queues;    /* Number of subscriber queues */
    int base_sub_queue;    /* Base Subscriber queue */
    int qset_size;         /* subscriber queue set size */
    uint32 num_queues;     /* total number of queues */
    uint32 num_nodes;      /* max number of sched nodes */
    uint32 max_nodes[_BCM_KT_COSQ_NODE_LEVEL_MAX]; /* max nodes in each level */
    bcm_gport_t *l1_gport_pair;         /* L1 gport mapping for dynamic update */
    _bcm_kt_cosq_port_info_t *port;     /* port information */
    _bcm_kt_cosq_list_t ext_qlist;      /* list of extended queues */
    _bcm_kt_cosq_list_t sub_qlist;      /* list of subscriber queues */
    _bcm_kt_cosq_list_t sched_list;     /* list of sched nodes */
    _bcm_kt_cosq_list_t l0_sched_list;  /* list of l0 sched nodes */
    _bcm_kt_cosq_list_t l1_sched_list;  /* list of l1 sched nodes */
    _bcm_kt_cosq_list_t l2_base_qlist;  /* list of l2  base queue index list */
    _bcm_kt_cosq_list_t l2_ext_qlist;   /* list of l2  base queue index list */
    _bcm_kt_cosq_list_t l2_sub_qlist;   /* list of l2  subscriber queue index list */

    _bcm_kt_cosq_node_t sched_node[_BCM_KT_NUM_TOTAL_SCHEDULERS];     /* sched nodes */
    _bcm_kt_cosq_node_t queue_node[_BCM_KT_NUM_L2_QUEUES];            /* queue nodes */
    uint32  intf_ref_cnt[BCM_MULTICAST_PORT_MAX]; /* Interface reference count
                                                     for queues */
}_bcm_kt_mmu_info_t;

STATIC _bcm_kt_mmu_info_t *_bcm_kt_mmu_info[BCM_MAX_NUM_UNITS];  /* MMU info */

STATIC soc_profile_mem_t *_bcm_kt_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_kt_wred_profile[BCM_MAX_NUM_UNITS];

STATIC int 
_bcm_kt_cosq_child_node_at_input(_bcm_kt_cosq_node_t *node, int cosq,
                                  _bcm_kt_cosq_node_t **child_node);
/* Number of COSQs configured for this device */
STATIC int _bcm_kt_num_cosq[SOC_MAX_NUM_DEVICES];

extern sal_mutex_t cosq_sync_lock[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *     _bcm_kt_cosq_localport_resolve
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
_bcm_kt_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result;

    if (!BCM_GPORT_IS_SET(gport)) {
        /* Below SOC check is used for time-being for KT2 */
        if (SOC_IS_KATANA(unit)) {
            if (!SOC_PORT_VALID(unit, gport)) {
                return BCM_E_PORT;
            }
        }
        *local_port = gport;
        return BCM_E_NONE;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
               BCM_GPORT_IS_SCHEDULER(gport) ||
               BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id));

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PORT;
    }

    *local_port = port;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_node_index_get
 * Purpose:
 *     Allocate free index from the given list
 * Parameters:
 *     unit       - (IN) unit number
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     end        - (IN) end index
 *     qset       - (IN) size of queue set
 *     range      - (IN) range bits
 *     id         - (OUT) start index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_node_index_get(int unit, SHR_BITDCL *list, int start, int end,
                       int qset, int range, int *id)
{
    int i, j, inc;
    _bcm_kt_mmu_info_t *mmu_info;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt_mmu_info[unit];

    *id = -1;

    if (range != qset) {
        inc = 1;
        for (i = start; i < end; i = (i + inc)) {
             inc = 1;
            for (j = i; j < (i + range); j++, inc++) {
                if (SHR_BITGET(list, j) != 0) {
                    break;
                }
            }

            if (j == (i + range) && (mmu_info->intf_ref_cnt[i] == 0)) {
                *id = i;
                return BCM_E_NONE;
            }
        }
        if (i == end) {
            return BCM_E_RESOURCE;
         }
    }

    for (i = start; i < end;  i = i + range) {
        for (j = i; j < (i + range); j++) {
            if (SHR_BITGET(list, j) != 0) {
                break;
            }
        }

        if (j == (i + range) && (mmu_info->intf_ref_cnt[i] == 0)) {
            *id  =  i;
            return BCM_E_NONE;
           }
    }

    if (i == end) {
        return BCM_E_RESOURCE;
      }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_node_index_set
 * Purpose:
 *     Mark indices as allocated
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     range      - (IN) range bits
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_node_index_set(_bcm_kt_cosq_list_t *list, int start, int range)
{
    list->count += range;
    SHR_BITSET_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_node_index_clear
 * Purpose:
 *     Mark indices as free
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     range      - (IN) range bits
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_node_index_clear(_bcm_kt_cosq_list_t *list, int start, int range)
{
    list->count -= range;
    SHR_BITCLR_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_map_index_get
 * Purpose:
 *     Allocate free index from the given list
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     end        - (IN) end index
 *     id         - (OUT) start index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_map_index_get(SHR_BITDCL *list, int start, int end, int *id)
{
    int i;

    for (i = start; i < end; i++) {
        if (SHR_BITGET(list, i) != 0) {
            continue;
        }
        *id  =  i;
        return BCM_E_NONE;
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     _bcm_kt_cosq_map_index_set
 * Purpose:
 *     Mark indices as allocated
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     range      - (IN) range bits
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_map_index_set(SHR_BITDCL *list, int start, int range)
{
    SHR_BITSET_RANGE(list, start, range);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_map_index_clear
 * Purpose:
 *     Mark indices as free
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     range      - (IN) range bits
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_map_index_clear(SHR_BITDCL *list, int start, int range)
{
    SHR_BITCLR_RANGE(list, start, range);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_map_index_is_set
 * Purpose:
 *     Check index is allocated
 * Parameters:
 *     list       - (IN) bit list
 *     index      - (IN) index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_map_index_is_set(SHR_BITDCL *list, int index)
{
    if (SHR_BITGET(list, index) != 0) {
        return TRUE;
    } 
    
    return FALSE;
}

STATIC int 
_bcm_kt_cosq_max_nodes_get(int unit, _bcm_kt_cosq_node_level_t level,
                           int *id)
{
    _bcm_kt_mmu_info_t *mmu_info;

    if (level < _BCM_KT_COSQ_NODE_LEVEL_ROOT ||
        level >= _BCM_KT_COSQ_NODE_LEVEL_MAX) {
        return BCM_E_PARAM;
    }
    mmu_info = _bcm_kt_mmu_info[unit];
    *id = mmu_info->max_nodes[level];

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_alloc_clear
 * Purpose:
 *     Allocate cosq memory and clear
 * Parameters:
 *     size       - (IN) size of memory required
 *     description- (IN) description about the structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC void *
_bcm_kt_cosq_alloc_clear(unsigned int size, char *description)
{
    void *block_p;

    block_p = sal_alloc(size, description);

    if (block_p != NULL) {
        sal_memset(block_p, 0, size);
    }

    return block_p;
}

/*
 * Function:
 *     _bcm_kt_cosq_free_memory
 * Purpose:
 *     Free memory allocated for mmu info members
 * Parameters:
 *     mmu_info_p       - (IN) MMU info pointer
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_kt_cosq_free_memory(_bcm_kt_mmu_info_t *mmu_info_p)
{
    sal_free(mmu_info_p->port);
    sal_free(mmu_info_p->l1_gport_pair);
    sal_free(mmu_info_p->ext_qlist.bits);
    sal_free(mmu_info_p->sched_list.bits);
    sal_free(mmu_info_p->l0_sched_list.bits);
    sal_free(mmu_info_p->l1_sched_list.bits);
    sal_free(mmu_info_p->l2_base_qlist.bits);
    sal_free(mmu_info_p->l2_ext_qlist.bits);

    if (mmu_info_p->sub_qlist.bits != NULL) {
        sal_free(mmu_info_p->sub_qlist.bits);
    }
    if (mmu_info_p->l2_sub_qlist.bits != NULL) {
        sal_free(mmu_info_p->l2_sub_qlist.bits);
    }
}

/*
 * Function:
 *     _bcm_kt_cosq_node_get
 * Purpose:
 *     Get internal information of queue group or scheduler type GPORT
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
_bcm_kt_cosq_node_get(int unit, bcm_gport_t gport, bcm_module_t *modid,
                      bcm_port_t *port, int *id, _bcm_kt_cosq_node_t **node)
{
    soc_info_t *si;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *node_base;
    bcm_module_t modid_out = 0;
    bcm_port_t port_out = 0;
    uint32 encap_id = 0;
    int index;

    si = &SOC_INFO(unit);

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        encap_id = BCM_GPORT_LOCAL_GET(gport);
        port_out = encap_id;
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        /* get the local_port */
        BCM_IF_ERROR_RETURN(
           _bcm_kt_cosq_localport_resolve(unit, gport, &port_out));
        encap_id = port_out;
    } else {
        return BCM_E_PORT;
    }

    /* Below SOC check is used for time-being for KT2 */
    if (SOC_IS_KATANA(unit)) {
        if (!SOC_PORT_VALID(unit, port_out)) {
            return BCM_E_PORT;
        }
    }

    mmu_info = _bcm_kt_mmu_info[unit];

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (index < mmu_info->num_base_queues) {
            node_base = mmu_info->queue_node;
        } else {
            if (si->port_num_ext_cosq[port_out] == 0) {
                return BCM_E_PORT;
            }
            node_base = mmu_info->queue_node;
        }
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (mmu_info->num_sub_queues > 0) {
            index += mmu_info->base_sub_queue;
        } else {
            index += mmu_info->num_base_queues;
        }
        node_base = mmu_info->queue_node;
    }
    else { 
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            /* scheduler */
            encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;
            if (encap_id == 0) {
                encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
            }
        }
        index = encap_id;
        if (index >= _BCM_KT_NUM_TOTAL_SCHEDULERS) {
            return BCM_E_PORT;
        }
        node_base = mmu_info->sched_node;
    }

    if (!(BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_MODPORT(gport)) &&
         node_base[index].numq == 0) {
        return BCM_E_NOT_FOUND;
    }

    if (modid != NULL) {
        *modid = modid_out;
    }
    if (port != NULL) {
        *port = port_out;
    }
    if (id != NULL) {
        *id = encap_id;
    }
    if (node != NULL) {
        *node = &node_base[index];
    }

    return BCM_E_NONE;
}

int
_bcm_kt_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum)
{
    _bcm_kt_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, modid, port, NULL, &node));
    *trunk_id = -1;

    if (qnum) {
        if (node->hw_index == -1) {
            return BCM_E_PARAM;
        }
        *qnum = node->hw_index;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_gport_traverse
 * Purpose:
 *     Walks through the valid COSQ GPORTs and calls
 *     the user supplied callback function for each entry.
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN)
 *     cb         - (IN) Callback function.
 *     user_data  - (IN) User data to be passed to callback function
 * Returns:
 *     BCM_E_NONE - Success.
 *     BCM_E_XXX
 */
int
_bcm_kt_cosq_gport_traverse(int unit, bcm_gport_t gport,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    _bcm_kt_cosq_node_t *node;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    if (node != NULL) {
       if (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) {
          flags = BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport) ?
                  BCM_COSQ_GPORT_UCAST_QUEUE_GROUP :
                  BCM_COSQ_GPORT_SUBSCRIBER;
       }
       rv = cb(unit, gport, node->numq, flags,
                  node->gport, user_data);
       if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
           if (SOC_CB_ABORT_ON_ERR(unit)) {
               return rv;
           }
#endif
       }
    } else {
            return BCM_E_NONE;
    }

    if (node->sibling != NULL) {
        _bcm_kt_cosq_gport_traverse(unit, node->sibling->gport, cb, user_data);
    }

    if (node->child != NULL) {
        _bcm_kt_cosq_gport_traverse(unit, node->child->gport, cb, user_data);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_index_resolve
 * Purpose:
 *     Find hardware index for the given port/cosq
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number or GPORT identifier
 *     cosq       - (IN) COS queue number
 *     style      - (IN) index style (_BCM_KA_COSQ_INDEX_STYLE_XXX)
 *     local_port - (OUT) local port number
 *     index      - (OUT) result index
 *     count      - (OUT) number of index
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_kt_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_kt_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count)
{
    _bcm_kt_cosq_node_t *node, *cur_node;
    bcm_port_t resolved_port;
    int resolved_index;
    int id, startq, numq = 0;
    _bcm_kt_mmu_info_t *mmu_info;

    if (cosq < -1) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        startq = 0;
    } else {
        startq = cosq;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) || 
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, port, NULL, &resolved_port, &id,
                                   &node));
        if ((node->cosq_attached_to < 0) || (node->hw_index == -1)) {
            /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        if (BCM_GPORT_IS_SCHEDULER(port) && 
             (style != _BCM_KT_COSQ_INDEX_STYLE_BUCKET)) {
            numq = node->numq;
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
        } 
        else {
             numq=node->numq;
        }    
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = 0;
    }

    switch (style) {
    case _BCM_KT_COSQ_INDEX_STYLE_BUCKET:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level > _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
                resolved_index = node->hw_index;
            } else {
                    return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit - 
                    mmu_info->port[resolved_port].q_offset;
            
            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;

    case _BCM_KT_COSQ_INDEX_STYLE_WRED:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level != _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
                resolved_index = node->hw_index;
            } else {
                    return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit - 
                   mmu_info->port[resolved_port].q_offset;

            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;

    case _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER:
        if (node != NULL) {
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                for (cur_node = node->child; cur_node != NULL;
                    cur_node = cur_node->sibling) {
                    if (cur_node->cosq_attached_to == startq) {
                        break;
                    }
                }

                if (cur_node == NULL) {
                    /* this cosq is not yet attached */
                    return BCM_E_INTERNAL;
                }

                if (node->numq <= 8) {
                    resolved_index = node->base_index + startq;
                } else {
                    resolved_index = cur_node->hw_index;
                } 
            } else { /* unicast queue group or multicast queue group */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit -
                   mmu_info->port[resolved_port].q_offset;

            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;
    case _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE:
        mmu_info = _bcm_kt_mmu_info[unit];
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level != _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
                resolved_index = node->base_index;
            } else {
                return BCM_E_PARAM;
            }
            
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index -= mmu_info->port[resolved_port].q_offset;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit -
                   mmu_info->port[resolved_port].q_offset;

            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;        
        }
        break;
    case _BCM_KT_COSQ_INDEX_STYLE_QUEUE_REPLICATION:
            mmu_info = _bcm_kt_mmu_info[unit];
            if (node != NULL) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                    resolved_index = node->hw_index;
                } else {
                    return BCM_E_PARAM;
                }
                
            } else { /* node == NULL */
                mmu_info = _bcm_kt_mmu_info[unit];
                numq = mmu_info->port[resolved_port].q_limit -
                       mmu_info->port[resolved_port].q_offset;
   
               if (startq >= numq) {
                   return BCM_E_PARAM;
               }
               resolved_index = startq + mmu_info->port
                                        [resolved_port].q_offset;        
           }
            break;
    default:
        return BCM_E_INTERNAL;
    }

    if (local_port != NULL) {
        *local_port = resolved_port;
    }
    if (index != NULL) {
        *index = resolved_index;
    }
    if (count != NULL) {
        *count = (cosq == -1) ? numq : 1;
    }

    return BCM_E_NONE;
}

int 
bcm_kt_sw_hw_queue(int unit, int *queue_array)
{
    _bcm_kt_mmu_info_t *mmu_info;
    int index = 0;
    mmu_info = _bcm_kt_mmu_info[unit];
    
    for (index = 0; index < mmu_info->num_queues; index++){
        if (mmu_info->queue_node[index].hw_index == *queue_array){
            *queue_array = index;
            return BCM_E_NONE;
        }        
    }
    index = -1;
    return BCM_E_PARAM;
}

int
bcm_kt_is_uc_queue(int unit,int queue_id,int *is_ucq)
{
    _bcm_kt_mmu_info_t *mmu_info;
    mmu_info = _bcm_kt_mmu_info[unit];
   
    if(_BCM_KT_IS_UC_QUEUE(mmu_info, queue_id)) 
    {
      *is_ucq = TRUE;
    }
    else 
    {
      *is_ucq = FALSE  ;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_kt_cosq_node_resolve
 * Purpose:
 *     Resolve queue number in the specified scheduler tree and its subtree
 * Parameters:
 *     unit       - (IN) unit number
 *     node       - (IN) node structure for the specified scheduler node
 *     cosq       - (IN) COS queue to attach to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_node_resolve(int unit, _bcm_kt_cosq_node_t *node,
                          bcm_cos_queue_t cosq)
{
    _bcm_kt_cosq_node_t *parent;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_list_t *list;
    bcm_port_t local_port;
    int alloc_size;
    int numq, id = 0;
    int offset, limit;

    parent = node->parent;
    if (parent == NULL) {
        /* Not attached, should never happen */
        return BCM_E_NOT_FOUND;
    }

    if (parent->numq == 0 || parent->numq < -1) {
        return BCM_E_PARAM;
    }

    if (parent->cosq_map == NULL) {
        alloc_size = SHR_BITALLOCSIZE(parent->numq);
        parent->cosq_map = _bcm_kt_cosq_alloc_clear(alloc_size,
                                           "cosq_map");
        if (parent->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (cosq < 0) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_map_index_get(parent->cosq_map,
                                        0, parent->numq, &id));    
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_map_index_set(parent->cosq_map, id, 1));
        node->cosq_attached_to = id;
    } else {
        if (_bcm_kt_cosq_map_index_is_set(parent->cosq_map, 
                                  cosq) == TRUE) {
            return BCM_E_EXISTS;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_map_index_set(parent->cosq_map,cosq, 1));
        node->cosq_attached_to = cosq;
    }   
 
    mmu_info = _bcm_kt_mmu_info[unit];
    numq = (parent->numq > 8) ? 8 : parent->numq;

    switch (parent->level) {
        case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
            list = &mmu_info->l0_sched_list;
            if (parent->base_index < 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_node_index_get(unit, list->bits, 0, mmu_info->max_nodes[node->level],
                                    mmu_info->qset_size, numq, &id));
                _bcm_kt_node_index_set(list, id, numq);
                parent->base_index = id;
                
            } else if (node->cosq_attached_to >= 8) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_node_index_get(unit, list->bits, 0, mmu_info->max_nodes[node->level],
                                    mmu_info->qset_size, 1, &id));
                _bcm_kt_node_index_set(list, id, 1);

            }    
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L0:
            list = &mmu_info->l1_sched_list;
            if (parent->base_index < 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_node_index_get(unit, list->bits, 0, mmu_info->max_nodes[node->level],
                                    mmu_info->qset_size, numq, &id));
                _bcm_kt_node_index_set(list, id, numq);
                parent->base_index = id;
            } else if (node->cosq_attached_to >= 8) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_node_index_get(unit, list->bits, 0, mmu_info->max_nodes[node->level],
                                    mmu_info->qset_size, 1, &id));
                _bcm_kt_node_index_set(list, id, 1);

            }            
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_qlist;
                local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(node->gport);
                offset = mmu_info->port[local_port].q_offset;
                limit =  mmu_info->port[local_port].q_limit;
            }  else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                local_port = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET
                                                         (node->gport);
                offset = 0;
                if (mmu_info->num_sub_queues > 0) {
                    list = &mmu_info->l2_sub_qlist;
                    limit = mmu_info->num_sub_queues;
                } else {
                    list = &mmu_info->l2_ext_qlist;
                    limit = mmu_info->num_ext_queues;
                }
            } else {
                list = &mmu_info->l2_ext_qlist;
                offset = 0;
                limit = mmu_info->num_ext_queues;
            }
            if (node->cosq_attached_to >= 8) {
                numq = 1;
            }    

            if (node->hw_index < 0) {
                if ((BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                           (parent->base_index < 0) &&
                           (node->cosq_attached_to < 8)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt_node_index_get(unit, list->bits, offset, limit,
                                                 mmu_info->qset_size, numq, &id));

                    _bcm_kt_node_index_set(list, id, numq);
                    if (mmu_info->num_sub_queues > 0) {
                        parent->base_index = id + mmu_info->base_sub_queue ;
                    } else {
                        parent->base_index = id + mmu_info->num_base_queues ;
                    }
                    node->base_index = parent->base_index;
                } else if((parent->base_index < 0) && (node->cosq_attached_to < 8)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt_node_index_get(unit, list->bits, offset, limit,
                                                mmu_info->qset_size, numq, &id));
                    _bcm_kt_node_index_set(list, id, numq);
                    parent->base_index = (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) ? id :
                                         (id + mmu_info->num_base_queues);
                    node->base_index = parent->base_index;  
                } else if (node->cosq_attached_to >= 8) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt_node_index_get(unit, list->bits, offset, limit,
                                                mmu_info->qset_size, numq, &id));
                    _bcm_kt_node_index_set(list, id, numq);
                }
                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    if (mmu_info->num_sub_queues > 0) {
                        id += mmu_info->base_sub_queue ;
                    } else {
                        id += mmu_info->num_base_queues;
                    }
                } else {
                    id += (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) ?
                       0 : mmu_info->num_base_queues;    
                }
            } else {
                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    /* Input 0 should be atttached
                     * first to get the correct base
                     */
                    if (node->cosq_attached_to < 8) {
                        if ((BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                            parent->base_index < 0) {
                            parent->base_index = node->hw_index;
                        }
                    }
                }
            }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    if (parent->numq <= 8) {
        if (parent->level <= _BCM_KT_COSQ_NODE_LEVEL_L1) {
            if (node->hw_index < 0) {
                node->hw_index = parent->base_index + node->cosq_attached_to;
            }    
        }

        parent->num_child++;
        /* get the lowest cosq as a first child */
        if (node->hw_index < parent->first_child) {
            parent->first_child = node->hw_index;
        }
    } else {
        if (parent->level <= _BCM_KT_COSQ_NODE_LEVEL_L1) {
            if (node->hw_index < 0) {
                node->hw_index = (node->cosq_attached_to < 8) ? 
                    (parent->base_index + node->cosq_attached_to) : id;
            }
            if (node->cosq_attached_to >= 8) {
                node->wrr_in_use = 1;
            }    
        }
        parent->num_child++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_node_unresolve
 * Purpose:
 *     Unresolve queue number in the specified scheduler tree and its subtree
 * Parameters:
 *     unit       - (IN) unit number
 *     node       - (IN) node structure for the specified scheduler node
 *     cosq       - (IN) COS queue to attach to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_node_unresolve(int unit, _bcm_kt_cosq_node_t *node, bcm_cos_queue_t cosq)
{
    _bcm_kt_cosq_node_t *cur_node, *parent;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_list_t *list;
    uint32 map;
    int min_index;
    int id, offset, numq;
    int min_flag = 0;

    if (node->cosq_attached_to < 0) {
        /* Has been unresolved already */
        return BCM_E_NONE;
    }

    parent = node->parent;

    if (parent->numq == 0 || parent->numq < -1) {
        return BCM_E_PARAM;
    }

    if (parent->numq <= 8) {
         /* find the current cosq_attached_to usage */
        map = 0;
        if (parent->first_child == node->hw_index && node->wrr_in_use == 0) {
            min_flag = 1;
            min_index = (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) ? 4095 :
                        ((node->level == _BCM_KT_COSQ_NODE_LEVEL_L1) ? 1023 : 255);
        } else {
            min_index = parent->first_child;
        }

        for (cur_node = parent->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->cosq_attached_to >= 0) {
                map |= 1 << cur_node->cosq_attached_to;
                /* dont update the min node is not this node */
                if (min_flag && cur_node->hw_index != node->hw_index) {
                if (cur_node->hw_index < min_index) {
                    min_index = cur_node->hw_index;
                }
               }
            }
        }

        if (!(map & (1 << node->cosq_attached_to))) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_map_index_clear(parent->cosq_map, 
             node->cosq_attached_to, 1));
        parent->first_child = min_index;

    } else {

        /* check whether the entry is available */
        for (cur_node = parent->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->gport == node->gport) {
                break;
            }
        }

        if (cur_node == NULL) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_map_index_clear(parent->cosq_map, 
             cur_node->cosq_attached_to, 1));
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    numq = (parent->numq > 8) ? 8 : parent->numq;

    switch (parent->level) {
        case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
            list = &mmu_info->l0_sched_list;
            parent->num_child--;
            if (parent->num_child >= 0) {
                if (node->cosq_attached_to >= 8) {
                    _bcm_kt_node_index_clear(list, node->hw_index, 1);
                } 
                if (parent->num_child == 0 &&
                    parent->base_index >= 0) {
                    /* release contiguous queue ids */
                    _bcm_kt_node_index_clear(list, parent->base_index, numq);
                    parent->base_index = -1;
                }
            } 
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L0:
            list = &mmu_info->l1_sched_list;
            parent->num_child--;
            if (parent->num_child >= 0) {
                if (node->cosq_attached_to >= 8) {
                    _bcm_kt_node_index_clear(list, node->hw_index, 1);
                } 
                if (parent->num_child == 0 &&
                    parent->base_index >= 0) {
                    /* release contiguous queue ids */
                    _bcm_kt_node_index_clear(list, parent->base_index, numq);
                    parent->base_index = -1;
                }
            } 
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_qlist;
                offset = 0;
            } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)
                       && (mmu_info->num_sub_queues > 0)) {
                list = &mmu_info->l2_sub_qlist;
                offset = mmu_info->base_sub_queue;
            } else {
                list = &mmu_info->l2_ext_qlist;
                offset = mmu_info->num_base_queues;
            }
            parent->num_child--;
            id = node->hw_index - offset;
            if (parent->num_child >= 0) {
                if (node->cosq_attached_to >= 8) {
                    _bcm_kt_node_index_clear(list, id, 1);
                } 
                if (parent->num_child == 0 &&
                    parent->base_index >= 0) {
                    /* release contiguous queue ids */
                    id = parent->base_index - offset;
                    _bcm_kt_node_index_clear(list, id, numq);
                    parent->base_index = -1;
                }
            } 
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_valid_werr_node(int unit, _bcm_kt_cosq_node_t *node, 
                                    int cosq, int spri)
{
    _bcm_kt_cosq_node_t *cur_node;
    uint8 spri_vector = 0;
    uint8 mask, mask_lower, mask_upper;

    if (cosq >= 8) {
        return 1;
    }    
    for (cur_node = node->child; cur_node != NULL;
         cur_node = cur_node->sibling) {
         if ((cur_node->cosq_attached_to < 8) && (cur_node->wrr_in_use == 0)) {
             spri_vector |= (1 << cur_node->cosq_attached_to);   
         }
    }
    mask_lower = ~(0xFF << cosq);
    mask_upper =  0xFF << (cosq + 1);

    if ((spri_vector & mask_lower) && (spri_vector & mask_upper)) {
        /* if queues are not contiguous return invalid */
        return 0;
    }    
   
    if (((spri_vector & mask_lower) == 0) && ((spri_vector & mask_upper) != 0)) {
        mask = spri_vector & mask_upper; 
        if ((spri == 0) || (mask & (1 << (cosq + 1)))) {
            /* check the next queue also in spri if cosq is spri */
            return 1;    
        }
    } else if (((spri_vector & mask_lower) != 0) && ((spri_vector & mask_upper) == 0)) {
        mask = spri_vector & mask_lower;
        if ((spri == 0) || (mask & (1 << (cosq - 1)))) {
            /* check previous queue also in spri if cosq is spri */
            return 1;
        }    
   } else {
       /* this is the first queue */
       return 1; 
   }
          
   return 0;
}

STATIC int 
_bcm_kt_cosq_dynamic_bw_update(int unit, soc_mem_t config_mem, 
                               int index, int hw_index, 
                               lls_l2_shaper_config_lower_entry_t l2_shaper_entry)
{
    soc_mem_t bucket_mem;
    lls_l2_shaper_bucket_lower_entry_t l2_bucket_max_entry;
    lls_l2_shaper_config_lower_entry_t l2_temp_entry;
    uint32 i;
    int rv = BCM_E_NONE;
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f
    };    
    static const soc_field_t not_active_fields[] = {
       NOT_ACTIVE_IN_LLS_0f, NOT_ACTIVE_IN_LLS_1f,
       NOT_ACTIVE_IN_LLS_2f, NOT_ACTIVE_IN_LLS_3f    
    };      
    
    /* Change L2 max shaper configuration back to original setting. */
    bucket_mem = (config_mem == LLS_L2_SHAPER_CONFIG_LOWERm) ?
                  LLS_L2_SHAPER_BUCKET_LOWERm :
                  LLS_L2_SHAPER_BUCKET_UPPERm;
    sal_memset(&l2_bucket_max_entry, 0, sizeof(lls_l2_shaper_bucket_lower_entry_t));
    sal_memcpy(&l2_temp_entry, &l2_shaper_entry, 
               sizeof(lls_l2_shaper_config_lower_entry_t));    
    for (i = 0; i < 4; i++) {
        soc_mem_field32_set(unit, config_mem, &l2_temp_entry,
                        rate_exp_fields[i], 0);
        soc_mem_field32_set(unit, config_mem, &l2_temp_entry,
                        rate_mant_fields[i], 0);
        soc_mem_field32_set(unit, bucket_mem, &l2_bucket_max_entry,
                        not_active_fields[i], 1);
    }    
    SOC_EGRESS_METERING_LOCK(unit);
    rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                       index/8, &l2_temp_entry);
    if (rv < 0) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }

    rv = soc_mem_write(unit, bucket_mem, MEM_BLOCK_ALL,
                       index/8, &l2_bucket_max_entry);
    if (rv < 0) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }

    rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                       index/8, &l2_shaper_entry);
    SOC_EGRESS_METERING_UNLOCK(unit);
    return rv;
}
 STATIC int
 _bcm_kt_cosq_inject_max_shaper_update(int unit,_bcm_kt_cosq_node_t *node)
{

     _bcm_kt_cosq_node_t *parentl0_node = NULL;
     _bcm_kt_cosq_node_t *cur_node = NULL;
     uint32 rval = 0;
     if((!node) || !(node->parent) ||
               (node->level != _BCM_KT_COSQ_NODE_LEVEL_L2)) 
     {
         return BCM_E_INTERNAL; 
     }
     parentl0_node = node->parent->parent;
     for(cur_node = parentl0_node->child ;cur_node;cur_node=cur_node->sibling) {
         if(cur_node->child){
            /* Inject max shaper enqueue. */
            SOC_IF_ERROR_RETURN(READ_LLS_DEBUG_INJECT_ACTIVATIONr(unit, &rval));
            soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, INJECTf, 1);
            soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, ENTITY_TYPEf, 1);
            soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, ENTITY_LEVELf, 3);
            soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, ENTITY_IDENTIFIERf,
                             cur_node->child->hw_index);
            SOC_IF_ERROR_RETURN(WRITE_LLS_DEBUG_INJECT_ACTIVATIONr(unit, rval));
         }
     }
     return BCM_E_NONE;
 }

    
STATIC int
_bcm_kt_cosq_dynamic_sched_update(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                  int mode, _bcm_kt_cosq_node_t *node)
{
    soc_timeout_t timeout;
    uint32 timeout_val;
    lls_l2_child_state1_entry_t l2_child_state;
    lls_l1_parent_state_entry_t l1_parent_state;
    lls_l2_shaper_config_lower_entry_t l2_shaper_entry_upper;
    lls_l2_shaper_config_lower_entry_t l2_shaper_entry_lower;
    lls_l2_shaper_config_lower_entry_t l2_temp_entry_upper; 
    lls_l2_shaper_config_lower_entry_t l2_temp_entry_lower; 
    lls_l2_shaper_config_lower_entry_t *entry; 
    uint32 wrr0_not_empty, wrr1_not_empty;
    uint32 poll_active = 1;
    int parent_id, old_l1_index;
    bcm_gport_t remap_gport;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *prev_node, *l1_node;
    uint32 spare, min_list, ef_list;
    bcm_port_t local_port;
    int index;
    int num_cos = 0;
    int cos = 0;
    int rv = BCM_E_NONE;
    soc_mem_t config_mem, config_mem1, config_mem2;
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f
    };
    static const soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f
    };
 
    if (node == NULL) {
        return BCM_E_INTERNAL;
    }
    bcm_kt_cosq_config_get(unit,&num_cos);
    /* Change the max shaper configuration for the queue to minimum threshold (64B) and
     * minimum rate (2kbps) 
     */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                    _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));
    SOC_EGRESS_METERING_LOCK(unit);
    config_mem1 =  LLS_L2_SHAPER_CONFIG_LOWERm;
    config_mem2 =  LLS_L2_SHAPER_CONFIG_UPPERm;        
    rv = soc_mem_read(unit, config_mem1, MEM_BLOCK_ALL,
                      index/8, &l2_shaper_entry_lower);
    if ( rv < 0 ){
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_read(unit, config_mem2, MEM_BLOCK_ALL,
                      index/8, &l2_shaper_entry_upper);
    if ( rv < 0 ){
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }
    sal_memcpy(&l2_temp_entry_lower, &l2_shaper_entry_lower, 
               sizeof(lls_l2_shaper_config_lower_entry_t)); 
    sal_memcpy(&l2_temp_entry_upper, &l2_shaper_entry_upper, 
               sizeof(lls_l2_shaper_config_lower_entry_t));
    for(cos = 0; cos<num_cos; cos++) {
        config_mem = ((cos < 4) ? LLS_L2_SHAPER_CONFIG_LOWERm :
                      LLS_L2_SHAPER_CONFIG_UPPERm);
        entry = ((cos < 4) ? (&l2_temp_entry_lower) : (&l2_temp_entry_upper));
        soc_mem_field32_set(unit, config_mem, entry, 
                                    rate_exp_fields[cos % 4], 0);
        soc_mem_field32_set(unit, config_mem, entry ,
                                    rate_mant_fields[cos % 4], 1); 
        soc_mem_field32_set(unit, config_mem, entry, 
                                    burst_exp_fields[cos % 4], 5);
    }

    rv = soc_mem_write(unit, config_mem1, MEM_BLOCK_ALL,
                       index/8, &l2_temp_entry_lower);
    if ( rv < 0 ){
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }
    rv = soc_mem_write(unit, config_mem2, MEM_BLOCK_ALL,
                       index/8, &l2_temp_entry_upper);
    if ( rv < 0 ){
        SOC_EGRESS_METERING_UNLOCK(unit);
        return rv;
    }
    SOC_EGRESS_METERING_UNLOCK(unit);
    /* Poll L2 child state until observe node is not active within the scheduler hierarchy.*/
    timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
    soc_timeout_init(&timeout, timeout_val, 0);

    while (poll_active) {
        SOC_IF_ERROR_RETURN
            (READ_LLS_L2_CHILD_STATE1m(unit, MEM_BLOCK_ALL, node->hw_index,
                                                  &l2_child_state));
         if (SOC_MEM_FIELD_VALID(unit, LLS_L2_CHILD_STATE1m, SPAREf)) {
             soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                               SPAREf, &spare);
         } else {
             soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                               C_ON_WERR_LISTf, &spare);
         }
         soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state, 
                                        C_ON_MIN_LIST_0f, &min_list);
         soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state, 
                                        C_ON_EF_LIST_0f, &ef_list);
         if (!spare && !min_list && !ef_list) {
             poll_active = 0;
         }
         if (soc_timeout_check(&timeout)) {
             BCM_IF_ERROR_RETURN
             (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem1, index, node->hw_index,
                                                     l2_shaper_entry_lower));
             BCM_IF_ERROR_RETURN
             (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem1, index, node->hw_index,
                                                     l2_shaper_entry_upper));
             BCM_IF_ERROR_RETURN
             (_bcm_kt_cosq_inject_max_shaper_update(unit,node));

             LOG_ERROR(BSL_LS_BCM_COMMON,
                       (BSL_META_U(unit,
                                   "ERROR: Queue %d L2 child state timeout: spare %d min_list %d ef_list %d\n"), 
                        node->hw_index, spare, min_list, ef_list));
             return BCM_E_TIMEOUT; 
         }         
    }

    /* Change L2 parent to point to new L1 */
    mmu_info = _bcm_kt_mmu_info[unit];
    if (node->parent == NULL) {
        return BCM_E_INTERNAL;
    }
    parent_id = (BCM_GPORT_SCHEDULER_GET(node->parent->gport) >> 8) & 0x7ff;
    remap_gport = mmu_info->l1_gport_pair[parent_id];

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, node->parent->gport, NULL, NULL, NULL,
                               &l1_node));
    old_l1_index = l1_node->hw_index;

    /* unresolve the node - delete this node from parent's child list */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_unresolve(unit, node, 0));

    /* now remove from the sw tree */
    if (node->parent != NULL) {
        if (node->parent->child == node) {
            node->parent->child = node->sibling;
        } else {
            prev_node = node->parent->child;
            while (prev_node != NULL && prev_node->sibling != node) {
                prev_node = prev_node->sibling;
            }
            if (prev_node == NULL) {
                return BCM_E_INTERNAL;
            }
            prev_node->sibling = node->sibling;
       }
        node->parent = NULL;
        node->sibling = NULL;
        node->hw_index = -1;
        node->wrr_in_use = 0;
        node->cosq_attached_to = -1;
     }

    node->wrr_in_use = (mode != BCM_COSQ_STRICT) ? 1 : 0;
    BCM_IF_ERROR_RETURN
         (bcm_kt_cosq_gport_attach(unit, node->gport, remap_gport, 0));

    /* Read L2 child state to check L2 is not active in the scheduler. */
    SOC_IF_ERROR_RETURN
        (READ_LLS_L2_CHILD_STATE1m(unit, MEM_BLOCK_ALL, node->hw_index,
                                   &l2_child_state));  
    if (SOC_MEM_FIELD_VALID(unit, LLS_L2_CHILD_STATE1m, SPAREf)) {
        soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                          SPAREf, &spare);
    } else {
        soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                          C_ON_WERR_LISTf, &spare);
    }

    /* if(spare == 1) check whether L2 is on old L1. */
    if (spare == 1) {
        SOC_IF_ERROR_RETURN
            (READ_LLS_L1_PARENT_STATEm(unit, MEM_BLOCK_ALL, old_l1_index,
                                             &l1_parent_state)); 
        soc_mem_field_get(unit, LLS_L1_PARENT_STATEm, (uint32 *)&l1_parent_state, 
                                P_WRR_0_NOT_EMPTYf, &wrr0_not_empty);
        soc_mem_field_get(unit, LLS_L1_PARENT_STATEm, (uint32 *)&l1_parent_state, 
                                P_WRR_1_NOT_EMPTYf, &wrr1_not_empty);
        if (wrr0_not_empty || wrr1_not_empty) {
            timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
            soc_timeout_init(&timeout, timeout_val, 0);
           
            poll_active = 1; 
            while (poll_active) {
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L2_CHILD_STATE1m(unit, MEM_BLOCK_ALL, node->hw_index,
                                                      &l2_child_state));
                if (SOC_MEM_FIELD_VALID(unit, LLS_L2_CHILD_STATE1m, SPAREf)) {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                                            SPAREf, &spare);
                } else {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state,
                                            C_ON_WERR_LISTf, &spare);
                }
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state, 
                                        C_ON_MIN_LIST_0f, &min_list);
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, (uint32 *)&l2_child_state, 
                                        C_ON_EF_LIST_0f, &ef_list);
                if (!spare && !min_list && !ef_list) {
                    poll_active = 0;
                }
                if (soc_timeout_check(&timeout)) {
                    (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem1, index, node->hw_index,
                                              l2_shaper_entry_lower));
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem2, index, node->hw_index,
                                                l2_shaper_entry_upper));
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_inject_max_shaper_update(unit,node));

                    LOG_ERROR(BSL_LS_BCM_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Queue %d old L1 index %d wrr0/1_not_empty %d %d\n"), 
                               node->hw_index, old_l1_index, wrr0_not_empty, 
                               wrr1_not_empty));
                    LOG_ERROR(BSL_LS_BCM_COMMON,
                              (BSL_META_U(unit,
                                          "spare %d min_list %d ef_list %d\n"),
                               spare, min_list, 
                               ef_list)); 
                    return BCM_E_TIMEOUT; 
                 }                    
            }    
        }                            
    }    
     BCM_IF_ERROR_RETURN
     (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem1, index, node->hw_index,
                                           l2_shaper_entry_lower));
     BCM_IF_ERROR_RETURN
     (_bcm_kt_cosq_dynamic_bw_update(unit, config_mem2, index, node->hw_index,
                                            l2_shaper_entry_upper));
     BCM_IF_ERROR_RETURN
     (_bcm_kt_cosq_inject_max_shaper_update(unit,node));

    return rv;
}

/*
 * Function:
 *     _bcm_kt_cosq_valid_dynamic_request
 * Purpose:
 *     Validate the new dynamic request
 * Parameters:
 *     unit       - (IN) unit number
 *     port      - (IN) port
 *     level      - (IN) scheduler level
 *     node_id    - (IN) node index
 *     parent_id  - (IN) parent index
 *     rval        - (IN) dynamic reg value
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt_cosq_valid_dynamic_request(int unit, bcm_port_t port, int node_level, 
                                   int node_id, int parent_id, uint32 rval) 
{
    soc_reg_t dyn_reg = LLS_SP_WERR_DYN_CHANGE_0Ar;
    lls_l0_parent_entry_t l0_parent;
    lls_l1_parent_entry_t l1_parent;
    bcm_port_t req_port, req_parent;

    /* dont allow more than one req per parent  */
    req_parent = soc_reg_field_get(unit, dyn_reg, rval, PARENT_IDf);
    if ((node_level == soc_reg_field_get(unit, dyn_reg, rval, NODE_LEVELf)) &&
        (parent_id == req_parent)) {
        return FALSE;
    }

    switch (node_level) {
        case _BCM_KT_COSQ_NODE_LEVEL_L0:
            req_port = parent_id;
            break;
        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            SOC_IF_ERROR_RETURN
                (READ_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, req_parent,
                                     &l0_parent));
            req_port = soc_mem_field32_get(unit, LLS_L0_PARENTm, &l0_parent, 
                                           C_PARENTf); 
            break;
        case _BCM_KT_COSQ_NODE_LEVEL_L2:
            SOC_IF_ERROR_RETURN
                (READ_LLS_L1_PARENTm(unit, MEM_BLOCK_ALL, req_parent,
                                     &l1_parent));
            req_port = soc_mem_field32_get(unit, LLS_L1_PARENTm, &l1_parent, 
                                           C_PARENTf); 
            SOC_IF_ERROR_RETURN
                (READ_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, req_port,
                                     &l0_parent));
            req_port = soc_mem_field32_get(unit, LLS_L0_PARENTm, &l0_parent, 
                                           C_PARENTf);    
            break;
        default:
            return FALSE;
    }

    /* dont allow more than one req per port */
    if (port == req_port) {
        return FALSE;
    }    
    return TRUE;
}

/*
 * Function:
 *     _bcm_kt_cosq_dynamic_sp_werr_change
 * Purpose:
 *     Set LLS dynamic scheduler registers based on level and hw index
 * Parameters:
 *     unit       - (IN) unit number
 *     level      - (IN) scheduler level
 *     node_id    - (IN) node index
 *     parent_id  - (IN) parent index
 *     config       - (IN) config details
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_dynamic_sp_werr_change(int unit, bcm_port_t port, int node_level, 
                                    int node_id, int parent_id, uint32 config,
                                    _bcm_kt_cosq_state_t state)
{
    soc_timeout_t timeout;
    uint32 timeout_val, rval;
    int i, in_use;
    soc_reg_t dyn_reg_a, dyn_reg_b;
    lls_l0_xoff_entry_t l0_xoff;
    soc_reg_t dyn_change_a[] = 
        {
        LLS_SP_WERR_DYN_CHANGE_0Ar,
        LLS_SP_WERR_DYN_CHANGE_1Ar,
        LLS_SP_WERR_DYN_CHANGE_2Ar,
        LLS_SP_WERR_DYN_CHANGE_3Ar
        };
    soc_reg_t dyn_change_b[] = 
        {
        LLS_SP_WERR_DYN_CHANGE_0Br,
        LLS_SP_WERR_DYN_CHANGE_1Br,
        LLS_SP_WERR_DYN_CHANGE_2Br,
        LLS_SP_WERR_DYN_CHANGE_3Br
        };

    /* Get one of the free register to place the request */    
    for (i = 0; i < 4; i++) {
        dyn_reg_a =  dyn_change_a[i];
        dyn_reg_b =  dyn_change_b[i];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, dyn_reg_a, REG_PORT_ANY, 
                                          0, &rval));
        if (SAL_BOOT_SIMULATION) {
            break;
        }    
        
        if (soc_reg_field_get(unit, dyn_reg_a, rval, IN_USEf) == 0) {
            break;
        } else {
        /* verify any other req pending from this port or same parent */
            if (_bcm_kt_cosq_valid_dynamic_request(unit, port, node_level, 
                     node_id, parent_id, rval) == FALSE) {
                return BCM_E_UNAVAIL;     
            }                                                     
        }
    }    

    if (i == 4) {
        /* None of the registers are available */
        return BCM_E_UNAVAIL;
    }    

    rval = 0;
    soc_reg_field_set(unit, dyn_reg_a, &rval, NODE_LEVELf, node_level);
    soc_reg_field_set(unit, dyn_reg_a, &rval, NODE_IDf, node_id);
    soc_reg_field_set(unit, dyn_reg_a, &rval, PARENT_IDf, parent_id);
    soc_reg_field_set(unit, dyn_reg_a, &rval, IN_USEf, 1);

    /* Write DYN_CHANGE_XB register first */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_b, REG_PORT_ANY, 
                                      0, config));
    /* Write DYN_CHANGE_XA register next. this triggers the request */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_a, REG_PORT_ANY, 
                                      0, rval));
    if (SAL_BOOT_SIMULATION) {
        return BCM_E_NONE;
    }
    
    in_use = 1;
    timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                   _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT);
    soc_timeout_init(&timeout, timeout_val, 0);

    while (in_use) {                              
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, dyn_reg_a, REG_PORT_ANY, 0, &rval)); 
        in_use = soc_reg_field_get(unit, dyn_reg_a, rval, IN_USEf);
        if (in_use && soc_timeout_check(&timeout)) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: dynamic sp <-> werr change operation failed \n")));
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  " node_level %d node_id %d parent_id %d \n"), 
                       node_level, node_id, parent_id));
            soc_reg_field_set(unit, dyn_reg_a, &rval, IN_USEf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_a, REG_PORT_ANY, 
                                0, rval));
            return BCM_E_TIMEOUT;
         }
    }

    if (node_level == _BCM_KT_COSQ_NODE_LEVEL_L1 && 
        state == _BCM_KT_COSQ_STATE_WRR_TO_SPRI) {
        sal_memset(&l0_xoff, 0, sizeof(lls_l0_xoff_entry_t));
        soc_mem_field32_set(unit, LLS_L0_XOFFm, &l0_xoff, C_XOFFf,
                            (1 << (parent_id & 0xf)));     
        SOC_IF_ERROR_RETURN
            (WRITE_LLS_L0_XOFFm(unit, MEM_BLOCK_ALL, ((parent_id >> 4) & 0xf),
                                &l0_xoff));
        /* Add single lls register read delay */
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, dyn_reg_a, REG_PORT_ANY, 
                                          0, &rval));
        soc_mem_field32_set(unit, LLS_L0_XOFFm, &l0_xoff, C_XOFFf, 0);     
        SOC_IF_ERROR_RETURN
            (WRITE_LLS_L0_XOFFm(unit, MEM_BLOCK_ALL, ((parent_id >> 4) & 0xf),
                                &l0_xoff));
        SOC_IF_ERROR_RETURN(READ_LLS_DEBUG_INJECT_ACTIVATIONr(unit, &rval));
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          INJECTf, 1);
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_TYPEf, 2);
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_LEVELf, 1);
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_IDENTIFIERf, parent_id);
        SOC_IF_ERROR_RETURN(WRITE_LLS_DEBUG_INJECT_ACTIVATIONr(unit, rval));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_shaper_value_get(int unit, int port, int max_speed,
                              uint32 lvl, int *node_count,
                              uint32 *exp, uint32 *mant)
{
    int speed;
    int rv;
    int l0_speed = 0, l1_speed = 0;

    if (NULL == exp || NULL == mant) {
        return BCM_E_PARAM;
    }

    speed = max_speed * 1000;


    if (lvl == _SOC_KT_COSQ_NODE_LEVEL_L0) {
        speed = (( KT_COSQ_SHAPER_PERC * speed) / 100);
        if (node_count[_SOC_KT_COSQ_NODE_LEVEL_L1]) {
            speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L1];
        }
    } else if (lvl == _SOC_KT_COSQ_NODE_LEVEL_L1) {
        l0_speed = (( KT_COSQ_SHAPER_PERC * speed) / 100);
        if(node_count[_SOC_KT_COSQ_NODE_LEVEL_L1]) {
            l0_speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L0];
        }
        speed = (( KT_COSQ_SHAPER_PERC * l0_speed) / 100);
        if (node_count[_SOC_KT_COSQ_NODE_LEVEL_L1]) {
            speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L1];
        }
    } else if (lvl == _SOC_KT_COSQ_NODE_LEVEL_L2) {
        l0_speed = (( KT_COSQ_SHAPER_PERC * speed) / 100);
        if (node_count[_SOC_KT_COSQ_NODE_LEVEL_L0]) {
            l0_speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L0];
        }
        l1_speed = (( KT_COSQ_SHAPER_PERC * l0_speed) / 100);
        if (node_count[_SOC_KT_COSQ_NODE_LEVEL_L1]) {
            l1_speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L1];
        }
        speed = (( KT_COSQ_SHAPER_PERC * l1_speed) / 100);
        if (node_count[_SOC_KT_COSQ_NODE_LEVEL_L2]) {
            speed /= node_count[_SOC_KT_COSQ_NODE_LEVEL_L2];
        }
    } else {
        speed = (( KT_COSQ_SHAPER_PERC * speed) / 100);
    }

    rv = soc_katana_get_shaper_rate_info(unit, speed, mant, exp);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_sched_bucket_set(int unit, int port,
                        int index, uint32 level, int *node_count, int max_speed)
{
    uint32 idx = 0;
    uint32 i = 0;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    uint32 exp = 0, mant = 0;
    soc_mem_t config_mem[2];
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    static const soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
    };
    static const soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };
    static const soc_field_t cycle_sel_fields[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,
        C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
        C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
    };

    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t burst_exp_f[] = {
        C_MAX_THLD_EXPf, C_MIN_THLD_EXPf
    };
    soc_field_t burst_mant_f[] = {
        C_MAX_THLD_MANTf, C_MIN_THLD_MANTf
    };
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SELf, C_MIN_CYCLE_SELf
    };


    SOC_IF_ERROR_RETURN
           (_bcm_kt_cosq_shaper_value_get(unit, port, max_speed, level, node_count, &exp, &mant));
    switch (level)  {
        case _SOC_KT_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;

            sal_memset(&port_entry, 0,
                       sizeof(lls_port_shaper_config_c_entry_t));
            SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry));
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_exp_f[0], exp);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_mant_f[0], mant);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_exp_f[0], KT_QUEUE_FLUSH_BURST_EXP);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_mant_f[0], KT_QUEUE_FLUSH_BURST_MANTISSA);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                cycle_sel_f[0], KT_QUEUE_FLUSH_CYCLE_SEL);
            SOC_IF_ERROR_RETURN
                   (soc_mem_write(unit, config_mem[0],
                                       MEM_BLOCK_ALL, index, &port_entry));
           break;
        case _SOC_KT_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l0_entry, 0,
                             sizeof(lls_l0_shaper_config_c_entry_t));
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index, &l0_entry));
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_exp_f[i], exp);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_mant_f[i], mant);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_exp_f[i], KT_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_mant_f[i], KT_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    cycle_sel_f[i], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                         (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, index, &l0_entry));
            }
            break;
        case _SOC_KT_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                sal_memset(&l1_entry, 0,
                            sizeof(lls_l1_shaper_config_c_entry_t));
                idx = (i * 4) + (index % 4);
                SOC_IF_ERROR_RETURN
                       (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                   index/4, &l1_entry));
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_exp_fields[idx], exp);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_mant_fields[idx], mant);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_exp_fields[idx], KT_QUEUE_FLUSH_BURST_EXP);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_mant_fields[idx], KT_QUEUE_FLUSH_BURST_MANTISSA);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    cycle_sel_fields[idx], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                     (soc_mem_write(unit, config_mem[i],
                                    MEM_BLOCK_ALL, index/4, &l1_entry));
            }
            break;
        case _SOC_KT_COSQ_NODE_LEVEL_L2:
            if ((index % 8) < 4) {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_LOWERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_LOWER_Cm;
            } else {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_UPPERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_UPPER_Cm;
            }

            for ( i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);

                sal_memset(&l2_entry, 0,
                           sizeof(lls_l2_shaper_config_lower_entry_t));
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                  index/8, &l2_entry));
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_exp_fields[idx], exp);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_mant_fields[idx], mant);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_exp_fields[idx], KT_QUEUE_FLUSH_BURST_EXP );
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_mant_fields[idx], KT_QUEUE_FLUSH_BURST_MANTISSA );
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    cycle_sel_fields[idx], KT_QUEUE_FLUSH_CYCLE_SEL);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem[i],
                                   MEM_BLOCK_ALL, index/8, &l2_entry));
            }
            break;
        default:
            return SOC_E_NONE;

    }

    return SOC_E_NONE;
}


STATIC int
_bcm_kt_egress_metering_freeze(int unit, soc_port_t port, void **setting)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 *bmap, *bmap_list[5];
    int word_count, wdc[5], blk_size=0;
    int lvl, j, index, word, bit, index_count, count;
    int idx, upper_flag = 0;
    int prev_idx, prev_upper_flag;
    int node_count[4], speed = 0;
    mem_entry_t *buffer;
    soc_info_t *si;
    static const soc_mem_t cfg_mems[][2] = {
        { LLS_PORT_SHAPER_CONFIG_Cm, INVALIDm },
        { LLS_L0_SHAPER_CONFIG_Cm, LLS_L0_MIN_CONFIG_Cm },
        { LLS_L1_SHAPER_CONFIG_Cm, LLS_L1_MIN_CONFIG_Cm },
        { LLS_L2_SHAPER_CONFIG_LOWERm, LLS_L2_MIN_CONFIG_LOWER_Cm },
        { LLS_L2_SHAPER_CONFIG_UPPERm, LLS_L2_MIN_CONFIG_UPPER_Cm },
    };

    SOC_EGRESS_METERING_LOCK(unit);

    si = &SOC_INFO(unit);
    bmap_list[1] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
    bmap_list[2] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
    bmap_list[3] = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
    count = 0; /* One entry for port level */

    for (lvl = _BCM_KT_COSQ_NODE_LEVEL_L0;
          lvl < _BCM_KT_COSQ_NODE_LEVEL_MAX; lvl++) {
        node_count[lvl] = 0;
    }

    if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)) {
        speed = si->port_speed_max[port];
    } else {
        rv =  bcm_esw_port_speed_get(unit, port, &speed);
        if (BCM_FAILURE(rv)) {
            goto _ka_freeze_abort;
        }
        if (speed == 0) {
            rv =  bcm_esw_port_speed_max(unit, port, &speed);
            if (BCM_FAILURE(rv)) {
                goto _ka_freeze_abort;
            }
        }
    }

    for (lvl = _BCM_KT_COSQ_NODE_LEVEL_L0;
          lvl < _BCM_KT_COSQ_NODE_LEVEL_MAX; lvl++) {
        bmap = bmap_list[lvl];
        mem = cfg_mems[lvl][0];
        index_count = soc_mem_index_count(unit, mem);
        /*
         * Allocation done using LLS_LX_PARENT
         * reading also done using same memories
         */
        if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L0) {
            index_count = soc_mem_index_count(unit, LLS_L0_PARENTm);
        } else if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L1) {
            index_count = soc_mem_index_count(unit, LLS_L1_PARENTm);
        } else if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) {
            index_count = soc_mem_index_count(unit, LLS_L2_PARENTm);
        }
        wdc[lvl] = _SHR_BITDCLSIZE(index_count);
        word_count = wdc[lvl];
        for (word = 0; word < word_count; word++) {
            if (bmap[word] != 0) {
                if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L0) {
                    node_count[_BCM_KT_COSQ_NODE_LEVEL_L0] += _shr_popcount(bmap[word]);
                } else if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    node_count[_BCM_KT_COSQ_NODE_LEVEL_L1] += _shr_popcount(bmap[word]);
                } else if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) {
                    node_count[_BCM_KT_COSQ_NODE_LEVEL_L2] += _shr_popcount(bmap[word]);
                }
            }
        }
    }

    for (lvl = _BCM_KT_COSQ_NODE_LEVEL_L0;
         lvl < _BCM_KT_COSQ_NODE_LEVEL_MAX; lvl++) {
        count += node_count[lvl];
    }
    count = (count * 8) + 1;
    node_count[_BCM_KT_COSQ_NODE_LEVEL_ROOT] = 1;

    buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
            "shaper buffer");

    if (buffer == NULL) {
        rv = BCM_E_MEMORY;
        goto _ka_freeze_abort;
    }

    count = 0;
    /* port */
    mem = LLS_PORT_SHAPER_CONFIG_Cm;
    index = port & 0x3f;
    /* Save the entry */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index,
            &buffer[count].entry);

    if (BCM_FAILURE(rv)) {
        sal_free(buffer);
        goto _ka_freeze_abort;
    }
    buffer[count].mem = mem;
    buffer[count].index = index;
    count++;
    /* Set the threshold for the port to max shaper rate */
    rv = _bcm_kt_cosq_sched_bucket_set(unit, port, index,
            _SOC_KT_COSQ_NODE_LEVEL_ROOT, node_count, speed);
    if (BCM_FAILURE(rv)) {
        sal_free(buffer);
        goto _ka_freeze_abort;
    }

    /* L0, L1, L2 meter values read and store*/
    for (lvl = _BCM_KT_COSQ_NODE_LEVEL_L0;
         lvl < _BCM_KT_COSQ_NODE_LEVEL_MAX; lvl++) {
        idx = upper_flag = 0;
        prev_idx = prev_upper_flag = -1;
        blk_size = (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) ? 8:
                    ((lvl == _BCM_KT_COSQ_NODE_LEVEL_L1)? 4:1);
        bmap = bmap_list[lvl];
        word_count = wdc[lvl];
        for (word = 0; word < word_count; word++) {
            if (bmap[word] == 0) {
                continue;
            }
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (!(bmap[word] & (1 << bit))) {
                    continue;
                }
                index = word * SHR_BITWID + bit;
                idx = index / blk_size;
                for (j = 0; j < 2; j++) {
                    mem = cfg_mems[lvl][j];
                    if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) {
                        if ((index % 8) >= 4) {
                            mem = cfg_mems[lvl + 1][j];
                            upper_flag = 1;
                        } else {
                            mem = cfg_mems[lvl][j];
                            upper_flag = 0;
                        }
                    }

                    /* Store the Threshold MIN/MAX table entries only
                     * once per table index as for the
                     * next consecutive L1/L2 indices falling on
                     * to the same table index need not be stored
                     */
                    if ((prev_idx != idx) ||
                            ((lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) && 
                            (prev_upper_flag != upper_flag))) {
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                                &buffer[count].entry);
                        if (BCM_FAILURE(rv)) {
                            sal_free(buffer);
                            goto _ka_freeze_abort;
                        }
                        buffer[count].mem = mem;
                        buffer[count].index = idx;
                        count++;
                    }

                }/*END of for j */
                prev_idx = idx;
                prev_upper_flag = upper_flag;
            } /* END of the valid bit */
        } /* END of the word */
    }
    /* L0, L1, L2 meter */
    for (lvl = _BCM_KT_COSQ_NODE_LEVEL_L0;
         lvl < _BCM_KT_COSQ_NODE_LEVEL_MAX; lvl++) {
        idx = upper_flag = 0;
        blk_size = (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) ? 8:
                    ((lvl == _BCM_KT_COSQ_NODE_LEVEL_L1)? 4:1);
        bmap = bmap_list[lvl];
        word_count = wdc[lvl];
        for (word = 0; word < word_count; word++) {
            if (bmap[word] == 0) {
                continue;
            }
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (!(bmap[word] & (1 << bit))) {
                    continue;
                }
                index = word * SHR_BITWID + bit;
                for (j = 0; j < 2; j++) {
                    mem = cfg_mems[lvl][j];
                    if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2) {
                        if ((index % 8) >= 4) {
                            mem = cfg_mems[lvl][j];
                        } 
                    }
                    /* set new shaper values */
                    if (lvl == _BCM_KT_COSQ_NODE_LEVEL_L2){
                        rv = _bcm_kt_cosq_sched_bucket_set(unit, port,
                                index, lvl, node_count, speed);
                    } else if ((lvl != _BCM_KT_COSQ_NODE_LEVEL_L2) && (j == 1)) {
                        rv = _bcm_kt_cosq_sched_bucket_set(unit, port,
                                index, lvl, node_count, speed);
                    }
                    if (BCM_FAILURE(rv)) {
                        sal_free(buffer);
                        goto _ka_freeze_abort;
                    }
                }/*END of for j */
            } /* END of the valid bit */
        } /* END of the word */
    } /* END of lvl */

    buffer[count].mem = INVALIDm;
    *setting = buffer;
_ka_freeze_abort:
    return rv;
}

STATIC int
_bcm_kt_egress_metering_thaw(int unit, soc_port_t port, void *setting)
{
    int       rv;
    int index;
    mem_entry_t *buffer;
    if (setting == NULL) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return SOC_E_NONE;
    }
    rv = SOC_E_NONE;

    buffer = setting;
    for (index = 0; buffer[index].mem != INVALIDm ;index++) {
         rv = soc_mem_write(unit, buffer[index].mem, MEM_BLOCK_ANY,
         buffer[index].index, &buffer[index].entry);
         if (BCM_FAILURE(rv)) {
             break;
         }
    }
    sal_free(setting);
    SOC_EGRESS_METERING_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *     _bcm_kt_cosq_sched_config_set
 * Purpose:
 *     Set LLS scheduler registers based on GPORT
 * Parameters:
 *     unit       - (IN) unit number
 *     gport     - (IN) queue group/scheduler GPORT identifier
 *     state     - (IN) sched state change
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_sched_config_set(int unit, bcm_gport_t gport, 
                              _bcm_kt_cosq_state_t state)
{
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *node, *parent;
    mmu_rqe_queue_op_node_map_entry_t rqe_op_node;
    mmu_wred_queue_op_node_map_entry_t wred_op_node;
    lls_l2_child_state1_entry_t l2_child_state;
    uint32 entry_c[SOC_MAX_MEM_WORDS];
    uint32 entry_p[SOC_MAX_MEM_WORDS];
    int port, start_spri = 0;
    int flush_active = 0;
    int update_spri_vector = 0;
    uint8 spri_vector = 0;
    int parent_index;
    uint8 vec_3_0, vec_7_4;
    uint32 rval = 0, timeout_val;
    int retval_temp = 0;
    uint32 spare, min_list, ef_list, not_empty;
    soc_timeout_t timeout;
    soc_mem_t config_mem, parent_mem;
    uint32 *bmap = NULL;
    soc_mem_t mem_c[] = { 
                        LLS_PORT_CONFIGm, 
                        LLS_L0_CONFIGm,
                        LLS_L1_CONFIGm
                      };
    soc_mem_t mem_p[] = { 
                        LLS_L0_PARENTm, 
                        LLS_L1_PARENTm,
                        LLS_L2_PARENTm
                      };
    
    void *setting = NULL;
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, &port, NULL,
                               &node));
    parent = node->parent;

    if (node->cosq_attached_to < 0 || (node->cosq_attached_to < 8 && 
        parent->base_index < 0)) {
        return BCM_E_PARAM;
    }

    config_mem = mem_c[parent->level];    
    if ((node->cosq_attached_to < 8) && (parent->hw_index != -1)) {
        start_spri = parent->base_index; 
        spri_vector = 1 << (node->hw_index - start_spri);
        update_spri_vector =  1;

        sal_memset(entry_c, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);    
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, parent->hw_index, 
                          &entry_c));
        vec_3_0 = soc_mem_field32_get(unit, config_mem, &entry_c, 
                                      P_NUM_SPRIf);
        vec_7_4 = soc_mem_field32_get(unit, config_mem, &entry_c, 
                                      P_VECT_SPRI_7_4f);
        if (state == _BCM_KT_COSQ_STATE_ENABLE || 
            state == _BCM_KT_COSQ_STATE_WRR_TO_SPRI) {
            vec_3_0 |= (spri_vector & 0xF);
            vec_7_4 |= (spri_vector >> 4); 
        } else if (state == _BCM_KT_COSQ_STATE_DISABLE ||
                   state == _BCM_KT_COSQ_STATE_SPRI_TO_WRR) {
            vec_3_0 &= ~(spri_vector & 0xF);
            vec_7_4 &= ~(spri_vector >> 4);
        } 

        /* update LLS_CONFIG with the updated vector */ 
        soc_mem_field32_set(unit, config_mem, &entry_c, P_START_SPRIf, 
                            start_spri);
        soc_mem_field32_set(unit, config_mem, &entry_c, P_NUM_SPRIf, 
                            vec_3_0);
        soc_mem_field32_set(unit, config_mem, &entry_c, P_VECT_SPRI_7_4f, 
                            vec_7_4);    
    }

    parent_mem = mem_p[parent->level]; 
    sal_memset(entry_p, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    sal_memset(&rqe_op_node, 0, sizeof(mmu_rqe_queue_op_node_map_entry_t));
    sal_memset(&wred_op_node, 0, sizeof(mmu_wred_queue_op_node_map_entry_t));

    if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
        bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
    } else if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L0) {
        bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
    } else if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
        bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
    }
    switch (state) {
        case _BCM_KT_COSQ_STATE_ENABLE:
            if ((parent_mem == LLS_L0_PARENTm) &&
                SOC_MEM_FIELD_VALID(unit, LLS_L0_PARENTm, C_TYPEf)) {
                soc_mem_field32_set(unit, parent_mem, &entry_p, C_TYPEf, 1); 
            }
            soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf, parent->hw_index);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, node->hw_index, &entry_p));
            if (update_spri_vector) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, parent->hw_index, 
                                   &entry_c));
            }
            
            if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
                /* Read the entry from the RQE map in order to preserve the E_TYPE field */
                SOC_IF_ERROR_RETURN
                    (READ_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                     &rqe_op_node));
                soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, &rqe_op_node,
                                    OP_NODEf, parent->hw_index);           
                soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, &wred_op_node,
                                    OP_NODEf, parent->hw_index);    
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                      &rqe_op_node));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                       &wred_op_node));            
                _soc_kt_cosq_end_port_flush(unit,node->hw_index);
            }
            _BCM_COSQ_PARENT_BMAP_SET(bmap, node->hw_index);
            break;
        case _BCM_KT_COSQ_STATE_DISABLE:
            mmu_info = _bcm_kt_mmu_info[unit];
            if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
                parent_index = (1 << soc_mem_field_length(unit, parent_mem, C_PARENTf)) - 1;
            } else {    
                parent_index = mmu_info->max_nodes[parent->level] - 1;
            }    
            soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf, parent_index); 
                                        
            if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
                /* flush the queue  */
                if (!(SAL_BOOT_SIMULATION)) {
                    BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval)); 
                    flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                                                     Q_FLUSH_ACTIVEf);
                    if (flush_active == 1) {
                        /* some other queue is in flush state, return failure */
                        LOG_ERROR(BSL_LS_BCM_COMMON,
                                  (BSL_META_U(unit,
                                              "ERROR: Queue %d is already in flush state \n"),
                                   (soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                                   Q_FLUSH_ID0f))));
                        return BCM_E_BUSY;  
                    }

                    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ID0f, 
                                       node->hw_index);
                    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_NUMf, 1);
                    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ACTIVEf, 1);
                    soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval,
                                      Q_FLUSH_EXTERNALf,
                                      (IS_EXT_MEM_PORT(unit, port) ? 1: 0));
                    BCM_IF_ERROR_RETURN(WRITE_TOQ_QUEUE_FLUSH0r(unit, rval)); 
                    flush_active = 1;
                    timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
                    soc_timeout_init(&timeout, timeout_val, 0);

                    while (flush_active) {                              
                        BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval)); 
                        flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                                                     Q_FLUSH_ACTIVEf);
                        if (soc_timeout_check(&timeout)) {
                            LOG_ERROR(BSL_LS_BCM_COMMON,
                                      (BSL_META_U(unit,
                                                  "ERROR: Queue %d flush operation failed \n"), 
                                       node->hw_index));
                            return (BCM_E_TIMEOUT);
                        }
                    }
                }
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L2_CHILD_STATE1m(unit, MEM_BLOCK_ALL,
                                               node->hw_index,
                                               &l2_child_state));
                if (SOC_MEM_FIELD_VALID(unit, LLS_L2_CHILD_STATE1m, SPAREf)) {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                      (uint32 *)&l2_child_state, SPAREf,
                                      &spare);
                } else {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                      (uint32 *)&l2_child_state, C_ON_WERR_LISTf,
                                      &spare);
                }
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                  (uint32 *)&l2_child_state,
                                  C_ON_MIN_LIST_0f, &min_list);
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                  (uint32 *)&l2_child_state,
                                  C_ON_EF_LIST_0f, &ef_list);
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                  (uint32 *)&l2_child_state,
                                  C_NOT_EMPTY_0f, &not_empty);
                /* check LLS_L2_CHILD_STATE[7:4] == 4'b0000 to confirm
                 * that the node is empty */
                if (spare || min_list || ef_list || not_empty) {
                    return BCM_E_BUSY;
                }
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L2_CHILD_STATE1m(unit, MEM_BLOCK_ALL, 
                                               node->hw_index,
                                               &l2_child_state));
                if (SOC_MEM_FIELD_VALID(unit, LLS_L2_CHILD_STATE1m, SPAREf)) {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                      (uint32 *)&l2_child_state, SPAREf,
                                      &spare);
                } else {
                    soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m,
                                      (uint32 *)&l2_child_state, C_ON_WERR_LISTf,
                                      &spare);
                }
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, 
                                  (uint32 *)&l2_child_state, 
                                  C_ON_MIN_LIST_0f, &min_list);
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, 
                                  (uint32 *)&l2_child_state, 
                                  C_ON_EF_LIST_0f, &ef_list);
                soc_mem_field_get(unit, LLS_L2_CHILD_STATE1m, 
                                  (uint32 *)&l2_child_state, 
                                  C_NOT_EMPTY_0f, &not_empty);
                /* check LLS_L2_CHILD_STATE[7:4] == 4'b0000 to confirm 
                 * that the node is empty */
                if (spare || min_list || ef_list || not_empty) {
                    return BCM_E_BUSY;  
                }    
                SOC_IF_ERROR_RETURN
                    (READ_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                 &rqe_op_node));
                soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, &rqe_op_node,
                                OP_NODEf, parent_index);
                soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, &wred_op_node,
                                OP_NODEf, parent_index);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, node->hw_index, &entry_p));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                      &rqe_op_node));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                       &wred_op_node));                                            
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, node->hw_index, &entry_p));
            }
            if (update_spri_vector) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, parent->hw_index, 
                                   &entry_c));
            }
            _BCM_COSQ_PARENT_BMAP_CLR(bmap, node->hw_index);
            break;
        case _BCM_KT_COSQ_STATE_WRR_TO_SPRI:
        case _BCM_KT_COSQ_STATE_SPRI_TO_WRR:
            if (update_spri_vector) {
                SOC_IF_ERROR_RETURN(_bcm_kt_egress_metering_freeze(unit,
                            port, &setting));
                retval_temp = _bcm_kt_cosq_dynamic_sp_werr_change(unit, port, node->level, 
                                node->hw_index, parent->hw_index, entry_c[0], state);
                SOC_IF_ERROR_RETURN(_bcm_kt_egress_metering_thaw(unit, port, setting));
                if (BCM_FAILURE(retval_temp)) {
                    return retval_temp;
                }
            }
            break;
        default:
            break;
    }            

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_sched_node_set
 * Purpose:
 *     Set LLS scheduler registers based on GPORT
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_sched_node_set(int unit, bcm_gport_t gport, 
                                    _bcm_kt_cosq_state_t state)
{
    _bcm_kt_cosq_node_t *node, *parent;
    _bcm_kt_cosq_node_t *cur_node;
    lls_port_config_entry_t port_cfg;
    lls_l0_parent_entry_t l0_parent;
    lls_l0_config_entry_t l0_cfg;
    lls_l1_config_entry_t l1_cfg;
    lls_l1_parent_entry_t l1_parent;
    lls_l2_parent_entry_t l2_parent;
    mmu_rqe_queue_op_node_map_entry_t rqe_op_node;
    mmu_wred_queue_op_node_map_entry_t wred_op_node;
    uint32 num_pri = 0, wrr_use = 0, start_pri = 0;
    int port;
    uint32 rval = 0;
    int spri_nodes =0;
    int set_wrr = 0;
    int update_mem = 0;
    int flush_active = 0;
    int min_index;
    soc_timeout_t timeout;
    uint32 timeout_val;
    uint32 *bmap = NULL;

    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        return _bcm_kt_cosq_sched_config_set(unit, gport, state);
    }    
    
    /*  parent ->level == root
      * config LLS_PORT_CONFIG, LLS_L0_PARENT points to port.

      * parent->level == L0
      * config LLS_L0_CONFIG and LLS_L1_PARENT points to L0 queue

      * parent->level == L1
      * config LLS_L1_CONFIG and LLS_L2_PARENT points to L1 queue
     */

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, &port, NULL,
                               &node));

   /* read lls_config[level], if num_pri and num_child is diffent reprogram
     * write lls_parent[level with node->parent->hw_index indexed on node_hw_index
     */

    parent = node->parent;

    min_index = (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) ? 4095 :
                ((node->level == _BCM_KT_COSQ_NODE_LEVEL_L1) ? 1023 : 255);
                    
    if (parent->numq <= 8) {
        for (cur_node = parent->child; cur_node != NULL;
             cur_node = cur_node->sibling) {
            if (cur_node->wrr_in_use == 0) {
                spri_nodes++;
                if (cur_node->hw_index < min_index) {
                    min_index = cur_node->hw_index;
                 }                     
            } else {
               set_wrr = 1;
            }
        }
        if (spri_nodes == 1 && state == _BCM_KT_COSQ_STATE_DISABLE) {
            /* this is the last spri node which gets deleted */
            spri_nodes = 0;
            min_index = (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) ? 4095 :
                        ((node->level == _BCM_KT_COSQ_NODE_LEVEL_L1) ? 1023 : 255);     
        } 
        
        parent->first_child = min_index;     
    }

    if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
        bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
        SOC_IF_ERROR_RETURN
            (READ_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &port_cfg));

        wrr_use = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg, P_WRR_IN_USEf);
        num_pri = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg, P_NUM_SPRIf);
        start_pri = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg, P_START_SPRIf);

        if (wrr_use != set_wrr) {
            soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, P_WRR_IN_USEf, set_wrr);
            update_mem = 1;
        }    
        if (num_pri != spri_nodes) {
            soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, P_NUM_SPRIf, spri_nodes);
            update_mem = 1;     
        }    
        if (start_pri != min_index) {
            soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, P_START_SPRIf, min_index);
            update_mem = 1;
        }    
        
        if (update_mem == 1) {
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &port_cfg));
        }    

        sal_memset(&l0_parent, 0, sizeof(lls_l0_parent_entry_t));  
        if (state == _BCM_KT_COSQ_STATE_ENABLE) {  
            soc_mem_field32_set(unit, LLS_L0_PARENTm, &l0_parent, C_PARENTf, parent->hw_index);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, &l0_parent));
            _BCM_COSQ_PARENT_BMAP_SET(bmap, node->hw_index);
        } 
        if (state == _BCM_KT_COSQ_STATE_DISABLE) {  
            soc_mem_field32_set(unit, LLS_L0_PARENTm, &l0_parent, C_PARENTf, 0x3f);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, &l0_parent));
            _BCM_COSQ_PARENT_BMAP_CLR(bmap, node->hw_index);
        }
    } else if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L0) {
        bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
        SOC_IF_ERROR_RETURN
            (READ_LLS_L0_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &l0_cfg));

        wrr_use = soc_mem_field32_get(unit, LLS_L0_CONFIGm, &l0_cfg, P_WRR_IN_USEf);
        num_pri = soc_mem_field32_get(unit, LLS_L0_CONFIGm, &l0_cfg, P_NUM_SPRIf);
        start_pri = soc_mem_field32_get(unit, LLS_L0_CONFIGm, &l0_cfg, P_START_SPRIf);

        if (wrr_use != set_wrr) {
            soc_mem_field32_set(unit, LLS_L0_CONFIGm, &l0_cfg, P_WRR_IN_USEf, set_wrr);
            update_mem = 1;
        }    
        if (num_pri != spri_nodes) {
            soc_mem_field32_set(unit, LLS_L0_CONFIGm, &l0_cfg, P_NUM_SPRIf, spri_nodes);
            update_mem = 1;     
        }    
        if (start_pri != min_index) {
            soc_mem_field32_set(unit, LLS_L0_CONFIGm, &l0_cfg, P_START_SPRIf, min_index);
            update_mem = 1;
        }    
        
        if (update_mem == 1) {
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L0_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &l0_cfg));
        }    

        sal_memset(&l1_parent, 0, sizeof(lls_l1_parent_entry_t));
        if (state == _BCM_KT_COSQ_STATE_ENABLE) {
            soc_mem_field32_set(unit, LLS_L1_PARENTm, &l1_parent, C_PARENTf, parent->hw_index);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L1_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, &l1_parent));
            _BCM_COSQ_PARENT_BMAP_SET(bmap, node->hw_index);
        }
        if (state == _BCM_KT_COSQ_STATE_DISABLE) {
            soc_mem_field32_set(unit, LLS_L1_PARENTm, &l1_parent, C_PARENTf, 0xff);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L1_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, &l1_parent));
            _BCM_COSQ_PARENT_BMAP_CLR(bmap, node->hw_index);
        }
    } else if (parent->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
        bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
        if (soc_feature(unit, soc_feature_dynamic_sched_update)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                set_wrr = 0;  
                spri_nodes = 0;
                min_index = 0;
            }
        } else {
            SOC_IF_ERROR_RETURN
                (READ_LLS_L1_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &l1_cfg));
            num_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_cfg, P_NUM_SPRIf);
            wrr_use = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_cfg, P_WRR_IN_USEf);
            if (SOC_IS_KATANA2(unit)) {
                /* Changed in the latest QT Regfile. MMU Implementor to revisit / reconfirm */
                start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_cfg, P_START_UC_SPRIf);
            } else {
                start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_cfg, P_START_SPRIf);
            }
        }    

        if (wrr_use != set_wrr) {
            soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_cfg, P_WRR_IN_USEf, set_wrr);
            update_mem = 1;
        }    
        if (num_pri != spri_nodes) {
            soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_cfg, P_NUM_SPRIf, spri_nodes);
            update_mem = 1;     
        }    
        if (start_pri != min_index) {
            if (SOC_IS_KATANA2(unit)) {
                soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_cfg, P_START_UC_SPRIf, min_index);
            } else {
                soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_cfg, P_START_SPRIf, min_index);
            }
            update_mem = 1;
        }
        if (state != _BCM_KT_COSQ_STATE_DISABLE && update_mem == 1) {
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L1_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &l1_cfg));
        }

        sal_memset(&l2_parent, 0, sizeof(lls_l2_parent_entry_t));
        sal_memset(&rqe_op_node, 0, sizeof(mmu_rqe_queue_op_node_map_entry_t));
        sal_memset(&wred_op_node, 0, sizeof(mmu_wred_queue_op_node_map_entry_t));
        
        if (state == _BCM_KT_COSQ_STATE_ENABLE) {            
            soc_mem_field32_set(unit, LLS_L2_PARENTm, &l2_parent, 
                                C_PARENTf, parent->hw_index);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, 
                                      &l2_parent));
            /* Read the entry from the RQE map in order to preserve the E_TYPE field */
            SOC_IF_ERROR_RETURN
                (READ_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                 &rqe_op_node));
            soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, &rqe_op_node,
                                OP_NODEf, parent->hw_index);           
            soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, &wred_op_node,
                                OP_NODEf, parent->hw_index);    
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                  &rqe_op_node));
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                   &wred_op_node));            
            _soc_kt_cosq_end_port_flush(unit,node->hw_index);
            _BCM_COSQ_PARENT_BMAP_SET(bmap, node->hw_index);
        } else if (state == _BCM_KT_COSQ_STATE_DISABLE) {
            /* deletion case , flush the queue  */
            BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval)); 
            flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                                             Q_FLUSH_ACTIVEf);
            if (flush_active == 1) {
                /* some other queue is in flush state, return failure */
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Queue %d is already in flush state \n"),
                           (soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                           Q_FLUSH_ID0f))));
                return BCM_E_BUSY;  
            }

            if (!(SAL_BOOT_SIMULATION)) {
                soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ID0f, 
                                  node->hw_index);
                soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_NUMf, 1);
                soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ACTIVEf, 1);
                soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval,
                                  Q_FLUSH_EXTERNALf,
                                  (IS_EXT_MEM_PORT(unit, port) ? 1: 0));

                BCM_IF_ERROR_RETURN(WRITE_TOQ_QUEUE_FLUSH0r(unit, rval)); 
            }
            flush_active = 1;
            timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
            soc_timeout_init(&timeout, timeout_val, 0);

            while (flush_active) {                              
                BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval)); 
                flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval, 
                                             Q_FLUSH_ACTIVEf);
                if (soc_timeout_check(&timeout)) {
                    LOG_ERROR(BSL_LS_BCM_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Queue %d flush operation failed \n"), 
                               node->hw_index));
                    return (BCM_E_TIMEOUT);
                }
            }

            soc_mem_field32_set(unit, LLS_L2_PARENTm, &l2_parent, 
                                C_PARENTf, 0x3ff);
            soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, &rqe_op_node,
                                OP_NODEf, 0x3ff);
            soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, &wred_op_node,
                                OP_NODEf, 0x3ff);
            SOC_IF_ERROR_RETURN
                (WRITE_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL, node->hw_index, 
                                      &l2_parent));
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                  &rqe_op_node));
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, node->hw_index,
                                                   &wred_op_node));   
            if (update_mem == 1) {
                SOC_IF_ERROR_RETURN
                    (WRITE_LLS_L1_CONFIGm(unit, MEM_BLOCK_ALL, parent->hw_index, &l1_cfg));
            }
            _BCM_COSQ_PARENT_BMAP_CLR(bmap, node->hw_index);
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_sched_set
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (IN) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (IN) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_sched_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int mode, int num_weights, const int weights[])
{
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *node, *cur_node, *l0_node;
    bcm_port_t local_port;
    int index;
    int i;
    int max_weight;
    lls_l0_child_weight_cfg_cnt_entry_t l0_weight_cfg;
    lls_l1_child_weight_cfg_cnt_entry_t l1_weight_cfg; 
    lls_l2_child_weight_cfg_cnt_entry_t l2_weight_cfg;
    lls_l2_parent_entry_t l2_parent;
    _bcm_kt_cosq_state_t state = _BCM_KT_COSQ_STATE_NO_CHANGE;
    int parent_id;
    bcm_gport_t remap_gport;
    int numq;
    uint32 fval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int rv;

    if (cosq < 0) { 
        if (cosq == -1) {
            /* caller needs to resolve the wild card value */
            return BCM_E_INTERNAL;
        } else {
            /* reject all other negative value */
            return BCM_E_PARAM;
        } 
    }
    
    /* Validate weight values */
    if (mode == BCM_COSQ_ROUND_ROBIN ||
        mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN ||
        mode == BCM_COSQ_DEFICIT_ROUND_ROBIN ||
        mode == BCM_COSQ_STRICT) {
        max_weight =
            (1 << soc_mem_field_length(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, C_WEIGHTf)) - 1;
        for (i = 0; i < num_weights; i++) {
            if (weights[i] < 0 || weights[i] > max_weight) {
                return BCM_E_PARAM;
            }
        }
    } else {
        return BCM_E_PARAM;
    }

    /* To support legacy device compatibility to configure STRICT
       mode if weight is "0".
     */
    if ((num_weights == 1) && (weights[0] == BCM_COSQ_WEIGHT_STRICT)) {
        mode =  BCM_COSQ_STRICT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    
    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                   &node));        
    }else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {   
            if ((node = l0_node->child) == NULL) {
                return BCM_E_PARAM;
            }    
        } else {
            node = l0_node;
        } 
        if (IS_CPU_PORT(unit, local_port)) {
            /*
            * In case when number of cpu queues are not multiples of 8
            * there is chance last siblings gets numq < 8,
            * which may result in failure , So node is moved to the
            * first sibling, which guarantee to have numq = 8
            */
            for (cur_node = l0_node; cur_node->sibling != NULL;
                    cur_node = cur_node->sibling) {
                  if (cur_node->numq == 5) {
                     break;
                  }
               }
            for (cur_node = cur_node->child; cur_node->sibling != NULL;
                    cur_node = cur_node->sibling);
            node = cur_node;
        }

    }

    if ((mode != BCM_COSQ_STRICT) && 
        (node->numq > 0 && (cosq + num_weights) > node->numq)) {
        return BCM_E_PARAM;
    }

    
    if (node->cosq_attached_to < 0) { /* Not resolved yet */
        return BCM_E_NOT_FOUND;
    }

    if (!soc_feature(unit, soc_feature_dynamic_sched_update) ||
        BCM_GPORT_IS_SCHEDULER(port)) { 
        for (cur_node = node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->cosq_attached_to == cosq) {
                break;
            }
        }
    } else {        
        numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
        for (cur_node = node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->child != NULL && (cur_node->cosq_attached_to == cosq ||
                cur_node->cosq_attached_to == (cosq + numq))) {
                break;
            }
        } 

        if (cur_node != NULL) {
            cur_node = cur_node->child;
        }
    }
    
    if (cur_node == NULL) {
        /* this cosq is not yet attached */
        return BCM_E_INTERNAL;
    }

    if (cur_node->wrr_in_use == 1 && mode == BCM_COSQ_STRICT) {
        if (cur_node->cosq_attached_to >= 8) {
            return BCM_E_PARAM;
        }  
        if (!soc_feature(unit, soc_feature_dynamic_sched_update) &&
            !soc_feature(unit, soc_feature_vector_based_spri) &&
            _bcm_kt_cosq_valid_werr_node(unit, node, cur_node->cosq_attached_to, 1) == 0)
            {
            return BCM_E_PARAM;
        }        
        state = _BCM_KT_COSQ_STATE_WRR_TO_SPRI;        
    }    

    if (cur_node->wrr_in_use == 0 && mode != BCM_COSQ_STRICT) {
        if (!soc_feature(unit, soc_feature_dynamic_sched_update) &&
            !soc_feature(unit, soc_feature_vector_based_spri) &&
            _bcm_kt_cosq_valid_werr_node(unit, node, cur_node->cosq_attached_to, 0) == 0)
            {
            return BCM_E_PARAM;
        }
        state = _BCM_KT_COSQ_STATE_SPRI_TO_WRR;
    }    

    if (state == _BCM_KT_COSQ_STATE_SPRI_TO_WRR && 
        cur_node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) {
        SOC_IF_ERROR_RETURN
            (READ_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL, cur_node->hw_index, 
                                 &l2_parent));
        if (soc_mem_field32_get(unit, LLS_L2_PARENTm, 
                                &l2_parent, C_EFf) == 1) {
            return BCM_E_PARAM;
        }
    }

    if (soc_feature(unit, soc_feature_dynamic_sched_update) &&
        !BCM_GPORT_IS_SCHEDULER(port)) {
        if (state == _BCM_KT_COSQ_STATE_WRR_TO_SPRI ||
            state == _BCM_KT_COSQ_STATE_SPRI_TO_WRR) {
        parent_id = (BCM_GPORT_SCHEDULER_GET(cur_node->parent->gport) >> 8) & 0x7ff;
        remap_gport = mmu_info->l1_gport_pair[parent_id];
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_dynamic_sched_update(unit, port, cosq, mode, cur_node));
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, remap_gport, NULL, &local_port, NULL,
                                   &node));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_node_get(unit, cur_node->parent->gport, 
                                       NULL, &local_port, NULL,
                                       &node));
        }
        port = node->parent->gport;
        cosq = node->cosq_attached_to;   
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                   &node));        
    } else {
        cur_node->wrr_in_use = (mode != BCM_COSQ_STRICT) ? 1 : 0;
        rv = _bcm_kt_cosq_sched_node_set(unit, cur_node->gport, state);
        if (BCM_FAILURE(rv)) {
            if (state == _BCM_KT_COSQ_STATE_SPRI_TO_WRR) {
                cur_node->wrr_in_use = 0;
            } else if (state == _BCM_KT_COSQ_STATE_WRR_TO_SPRI) {
                cur_node->wrr_in_use = 1;
            }
        }
    }

    if (mode != BCM_COSQ_STRICT) {
        mem = LLS_PORT_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, 
                                         local_port, &entry));
        fval = ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
                (mode == BCM_COSQ_ROUND_ROBIN)) ? 1 : 0;
        if (soc_mem_field32_get(unit, mem, entry, 
                                PACKET_MODE_WRR_ACCOUNTING_ENABLEf) != fval) {
            soc_mem_field32_set(unit, mem, entry,
                                PACKET_MODE_WRR_ACCOUNTING_ENABLEf, fval);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                local_port, &entry)); 
        } 
    }

    switch (node->level) {
    case _BCM_KT_COSQ_NODE_LEVEL_ROOT: /* port scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                       NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l0_weight_cfg));
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l0_weight_cfg, 
                                C_WEIGHTf, (mode == BCM_COSQ_ROUND_ROBIN) ? 1: weights[i]); 
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l0_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l0_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l0_weight_cfg));

        }
        break;

    case _BCM_KT_COSQ_NODE_LEVEL_L0: /* L0 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l1_weight_cfg));
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l1_weight_cfg, 
                                C_WEIGHTf, (mode == BCM_COSQ_ROUND_ROBIN) ? 1: weights[i]); 
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l1_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l1_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l1_weight_cfg));

        }
        break;

    case _BCM_KT_COSQ_NODE_LEVEL_L1: /* L1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l2_weight_cfg));
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l2_weight_cfg, 
                                C_WEIGHTf, (mode == BCM_COSQ_ROUND_ROBIN) ? 1: weights[i]); 
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l2_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l2_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l2_weight_cfg));

        }
        break;

    default:
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_sched_get
 * Purpose:
 *     Get scheduling mode setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (OUT) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (OUT) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int *mode, int num_weights, int weights[])
{
    _bcm_kt_mmu_info_t *mmu_info; 
    _bcm_kt_cosq_node_t *node, *l0_node = NULL;
    _bcm_kt_cosq_node_t *cur_node;
    bcm_port_t local_port;
    int index;
    int i;
    lls_l0_child_weight_cfg_cnt_entry_t l0_weight_cfg;
    lls_l1_child_weight_cfg_cnt_entry_t l1_weight_cfg;
    lls_l2_child_weight_cfg_cnt_entry_t l2_weight_cfg;
    int numq;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value */
            return BCM_E_INTERNAL;
        } else {
            /* reject all other negative value */
            return BCM_E_PARAM;
        } 
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
           BCM_IF_ERROR_RETURN
               (_bcm_kt_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                      &node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));
        mmu_info = _bcm_kt_mmu_info[unit];

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {
            if ((node = l0_node->child) == NULL) {
                return BCM_E_PARAM;
            }    
        } else {
            node = l0_node;
        }
        if (IS_CPU_PORT(unit, local_port)) {
            /*
            * In case when number of cpu queues are not multiples of 8
            * there is chance last siblings gets numq < 8,
            * which may result in failure , So node is moved to the
            * first sibling, which guarantee to have numq = 8
            */
            for (cur_node = l0_node; cur_node->sibling != NULL;
                    cur_node = cur_node->sibling) {
                /* After warm boot first sibling will have numq = 4
                 * check for the parent node in which all childs will have the numq = 8
                 * so we wont see failure 
                 */
                if (cur_node->numq == 5) {
                    break;
                }
            }
            for (cur_node = cur_node->child; cur_node->sibling != NULL;
                    cur_node = cur_node->sibling);
            node = cur_node;
        }
    }

    if (node->numq > 0 && (cosq + num_weights) > node->numq) {
        return BCM_E_PARAM;
    }

    if (node->cosq_attached_to < 0) { /* Not resolved yet */
        return BCM_E_NOT_FOUND;
    }

    if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {
        for (cur_node = node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->cosq_attached_to == cosq) {
                break;
            }
        }
    } else {    
        numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
        for (cur_node = node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->child != NULL && (cur_node->cosq_attached_to == cosq ||
                cur_node->cosq_attached_to == (cosq + numq))) {
                break;
            }
        } 

        if (cur_node != NULL) {
            port = l0_node->gport;
            cosq = cur_node->cosq_attached_to;
        }
    }
    
    switch (node->level) {
    case _BCM_KT_COSQ_NODE_LEVEL_ROOT: /* port scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l0_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l0_weight_cfg, C_WEIGHTf);
        }
        break;

    case _BCM_KT_COSQ_NODE_LEVEL_L0: /* L0 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l1_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l1_weight_cfg, C_WEIGHTf);

        }
        break;

    case _BCM_KT_COSQ_NODE_LEVEL_L1: /* L1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, index, &l2_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, (uint32 *)&l2_weight_cfg, C_WEIGHTf);

        }
        break;

    default:
        return BCM_E_INTERNAL;
    }

    if (cur_node != NULL && cur_node->wrr_in_use == 0) {
        *mode = BCM_COSQ_STRICT;   
    } else {   
        mem = LLS_PORT_CONFIGm;
        BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, local_port, &entry));
        if (soc_mem_field32_get(unit, mem, entry,
                   PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
            *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        } else {
            *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        }

    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_port_sched_set
 * Purpose:
 *     Set scheduling mode setting for a port
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (IN) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (IN) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_port_sched_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                   int mode, int num_weights, int *weights)
{
    int i;

    for (i = 0; i < num_weights; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_sched_set(unit, port, (cosq + i), mode, 1, &weights[i])); 
                                     
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_port_sched_get
 * Purpose:
 *     Set scheduling mode setting for a port
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (OUT) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (OUT) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_port_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                   int *mode, int num_weights, int *weights)
{
    int i;

    for (i = 0; i < num_weights; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_sched_get(unit, port, (cosq + i), mode, 1, &weights[i])); 
                                     
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_mapping_set_regular
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (IN) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_mapping_set_regular(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_queue_t cosq)
{
    _bcm_kt_mmu_info_t *mmu_info;
    bcm_port_t port;
    int numq;
    bcm_cos_queue_t hw_cosq;
    soc_field_t field = COSf;
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];     
    uint32 old_index, new_index;
    int rv;
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    int cpu_hg_index = 0;
#endif
    
    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    hw_cosq = cosq;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve
            (unit, gport, cosq, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
             &port, &hw_cosq, NULL));
    } else {
        if (BCM_GPORT_IS_SET(gport)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
        } else {
            port = gport;
        }

        if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
            return BCM_E_PORT;
        }
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    numq = mmu_info->port[port].q_limit - 
           mmu_info->port[port].q_offset;

    if (cosq >= numq) {
        return BCM_E_PARAM;
    }
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        field = (numq <= 8) ? COSf : HG_COSf;
    }

    entries[0] = &cos_map_entries;

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
    old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                    SELECTf);
    old_index *= 16;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_kt_cos_map_profile[unit],
                             old_index, 16, entries));
    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority], field,
                        hw_cosq);

    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_delete(unit, _bcm_kt_cos_map_profile[unit],
                                old_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port)) {
        rv = soc_profile_mem_delete(unit, _bcm_kt_cos_map_profile[unit],
                                    old_index);
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_kt_cos_map_profile[unit], entries,
                             16, &new_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port)) {
        rv = soc_profile_mem_reference(unit, _bcm_kt_cos_map_profile[unit],
                                       new_index, 16);
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    if (rv == SOC_E_RESOURCE) {
        (void)soc_profile_mem_reference(unit, _bcm_kt_cos_map_profile[unit],
                                        old_index, 16);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_CPU_PORT(unit, port)) {
            (void)soc_profile_mem_reference(unit,
                                            _bcm_kt_cos_map_profile[unit],
                                            old_index, 16);
        }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
    }

    soc_mem_unlock(unit, PORT_COS_MAPm);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_mem_field32_set(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf,
                        new_index / 16);
    BCM_IF_ERROR_RETURN
        (WRITE_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (IS_CPU_PORT(unit, port)) {
        cpu_hg_index = SOC_IS_KATANA2(unit) ?
                       SOC_INFO(unit).cpu_hg_pp_port_index :
                       SOC_INFO(unit).cpu_hg_index;
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    cpu_hg_index, SELECTf,
                                    new_index / 16));
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_bucket_set
 * Purpose:
 *     Configure COS queue bandwidth control bucket
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (IN) kbps or packet/second
 *     max_quantum   - (IN) kbps or packet/second
 *     burst_quantum - (IN) kbps or packet/second
 *     flags         - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_kt_cosq_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 burst_min_quantum, uint32 burst_max_quantum,
                        uint32 flags)
{
    int rv;
    _bcm_kt_cosq_node_t *node;
    bcm_port_t local_port;
    int index;
    soc_mem_t config_mem[2];
    soc_mem_t bucket_mem;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_port_shaper_config_c_entry_t port_null_entry;
    lls_port_shaper_bucket_c_entry_t port_bucket_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    lls_l0_min_config_c_entry_t  l0_null_entry;
    lls_l0_min_bucket_c_entry_t  l0_bucket_entry; 
    lls_l1_min_config_c_entry_t  l1_null_entry;
    lls_l1_min_bucket_c_entry_t  l1_bucket_entry;
    lls_l2_min_config_lower_c_entry_t l2_null_entry; 
    lls_l2_min_bucket_lower_c_entry_t l2_bucket_entry;
    lls_l2_shaper_config_lower_entry_t l2_null_max_entry; 
    lls_l2_shaper_bucket_lower_entry_t l2_bucket_max_entry;    
    uint32 rate_exp[2], rate_mantissa[2];
    uint32 burst_exp[2], burst_mantissa[2];
    uint32 cycle_sel[2];
    uint32 burst_min, burst_max;
    int i, idx;
    int level = _BCM_KT_COSQ_NODE_LEVEL_L2;
    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t burst_exp_f[] = {
        C_MAX_THLD_EXPf, C_MIN_THLD_EXPf
    };
    soc_field_t burst_mant_f[] = {
        C_MAX_THLD_MANTf, C_MIN_THLD_MANTf
    };
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SELf, C_MIN_CYCLE_SELf
    };
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    static const soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
    };
    static const soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };
    soc_field_t cycle_sel_fields[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f,    
        C_MIN_CYCLE_SEL_0f, C_MIN_CYCLE_SEL_1f,
        C_MIN_CYCLE_SEL_2f, C_MIN_CYCLE_SEL_3f
    };

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }


    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                    _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        level = node->level;   
        if ((level== _BCM_KT_COSQ_NODE_LEVEL_ROOT) &&
            (min_quantum > 0)) {
            /* min shaper is not supported */
            return BCM_E_PARAM;
        } 
    }     

    /* compute exp and mantissa and program the registers */
    rv = soc_katana_get_shaper_rate_info(unit, max_quantum,
                                         &rate_mantissa[0], &rate_exp[0]);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_get_shaper_rate_info(unit, min_quantum,
                                         &rate_mantissa[1], &rate_exp[1]);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    burst_max = burst_max_quantum;
    burst_min = burst_min_quantum;
    rv = soc_katana_get_shaper_burst_info(unit, burst_max, &burst_mantissa[0], 
                                          &burst_exp[0], flags);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }
    
    rv = soc_katana_get_shaper_burst_info(unit, burst_min, &burst_mantissa[1], 
                                          &burst_exp[1], flags);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }    

    if (!(flags & _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION)) {
        /* set recommended burst settings */
        for (i = 0; i < 2; i++) {
            if (rate_exp[i] != 0) {
                if (rate_exp[i] < 10) {
                    burst_exp[i] = 5;
                    burst_mantissa[i] = 0;
                } else {
                    burst_exp[i] = rate_exp[i] - 4;
                    burst_mantissa[i] = (rate_mantissa[i] >> 3);
                }
            }
        }
        /* set recommended refresh rate/cycle_sel settings */
        for (i = 0; i < 2; i++) {
            if (rate_exp[i] > 4) {
                cycle_sel[i] = 0;
            } else if (rate_exp[i] > 1) {
                cycle_sel[i] = 5 - rate_exp[i];
            } else {
                 cycle_sel[i] = 4;
            }
        }
    } else {
        for (i = 0; i < 2; i++) {
            if ((burst_exp[i] >= 14) && (burst_mantissa[i] >= 124)) {
                /* maximum supported is 2064384 byte */
                burst_exp[i] = 14;
                burst_mantissa[i] = 124;
            }
            rv =  soc_katana_compute_refresh_rate(unit, rate_exp[i],
                                                  rate_mantissa[i], burst_exp[i],
                                                  burst_mantissa[i], &cycle_sel[i]);
            if (BCM_FAILURE(rv)) {
                return(rv);
            }
        }
    }
   
    if (soc_feature(unit, soc_feature_dynamic_shaper_update) && 
        (level > _BCM_KT_COSQ_NODE_LEVEL_L0)) { 
        /* set cycle_sel = 0 for L1 and L2 in A0 */
        for (i = 0; i < 2; i++) {
            cycle_sel[i] = 0;
        }
    }    

    SOC_EGRESS_METERING_LOCK(unit);
    switch (level) {
        case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;
            sal_memset(&port_null_entry, 0, sizeof(lls_port_shaper_config_c_entry_t));
            sal_memset(&port_bucket_entry, 0, sizeof(lls_port_shaper_bucket_c_entry_t));
            
            rv = soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
            }
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_exp_f[0], rate_exp[0]);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                rate_mant_f[0], rate_mantissa[0]);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_exp_f[0], burst_exp[0]);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                burst_mant_f[0], burst_mantissa[0]);
            soc_mem_field32_set(unit, config_mem[0], &port_entry,
                                cycle_sel_f[0], cycle_sel[0]); 
            if (soc_feature(unit, soc_feature_dynamic_shaper_update)) {
                bucket_mem = LLS_PORT_SHAPER_BUCKET_Cm;
                rv = soc_mem_write(unit, config_mem[0],
                                   MEM_BLOCK_ALL, index, &port_null_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                soc_mem_field32_set(unit, bucket_mem, &port_bucket_entry,
                                    NOT_ACTIVE_IN_LLSf, 1);   
                rv = soc_mem_write(unit, bucket_mem,
                                       MEM_BLOCK_ALL, index, &port_bucket_entry);   
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
            }
            rv = soc_mem_write(unit, config_mem[0],
                               MEM_BLOCK_ALL, index, &port_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
            }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;
            sal_memset(&l0_null_entry, 0, sizeof(lls_l0_min_config_c_entry_t));
            sal_memset(&l0_bucket_entry, 0, sizeof(lls_l0_min_bucket_c_entry_t));
            
            for (i = 0; i < 2; i++) {
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                   index, &l0_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_exp_f[i], rate_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    rate_mant_f[i], rate_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_exp_f[i], burst_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    burst_mant_f[i], burst_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l0_entry,
                                    cycle_sel_f[i], cycle_sel[i]);   
                if (soc_feature(unit, soc_feature_dynamic_shaper_update)) {
                    bucket_mem = (config_mem[i] == LLS_L0_MIN_CONFIG_Cm) ?
                                 LLS_L0_MIN_BUCKET_Cm :
                                 LLS_L0_SHAPER_BUCKET_Cm;
                    rv = soc_mem_write(unit, config_mem[i],
                                       MEM_BLOCK_ALL, index, &l0_null_entry);
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                    soc_mem_field32_set(unit, bucket_mem, &l0_bucket_entry,
                                        NOT_ACTIVE_IN_LLSf, 1);        
                    rv = soc_mem_write(unit, bucket_mem,
                                       MEM_BLOCK_ALL, index, &l0_bucket_entry);                    
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                }    
                rv = soc_mem_write(unit, config_mem[i],
                                     MEM_BLOCK_ALL, index, &l0_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
             }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;
            sal_memset(&l1_null_entry, 0, sizeof(lls_l1_min_config_c_entry_t));
            sal_memset(&l1_bucket_entry, 0, sizeof(lls_l1_min_bucket_c_entry_t));
            
            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                   index/4, &l1_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_exp_fields[idx], rate_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    rate_mant_fields[idx], rate_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_exp_fields[idx], burst_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    burst_mant_fields[idx], burst_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l1_entry,
                                    cycle_sel_fields[idx], cycle_sel[i]);               
                if (soc_feature(unit, soc_feature_dynamic_shaper_update)) {
                    bucket_mem = (config_mem[i] == LLS_L1_MIN_CONFIG_Cm) ?
                                 LLS_L1_MIN_BUCKET_Cm :
                                 LLS_L1_SHAPER_BUCKET_Cm;            
                    rv = soc_mem_write(unit, config_mem[i],
                                          MEM_BLOCK_ALL, index/4, &l1_null_entry);
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                    soc_mem_field32_set(unit, bucket_mem, &l1_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_0f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l1_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_1f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l1_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_2f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l1_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_3f, 1);                    
                    rv = soc_mem_write(unit, bucket_mem,
                                          MEM_BLOCK_ALL, index/4, &l1_bucket_entry);                    
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                }    

                rv = soc_mem_write(unit, config_mem[i],
                                   MEM_BLOCK_ALL, index/4, &l1_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
            }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L2:
            if ((index % 8) < 4) {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_LOWERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_LOWER_Cm;
            } else {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_UPPERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_UPPER_Cm;
            }
            sal_memset(&l2_null_entry, 0, sizeof(lls_l2_min_config_lower_c_entry_t));
            sal_memset(&l2_bucket_entry, 0, sizeof(lls_l2_min_bucket_lower_c_entry_t));
            sal_memset(&l2_null_max_entry, 0, sizeof(lls_l2_shaper_config_lower_entry_t));
            sal_memset(&l2_bucket_max_entry, 0, sizeof(lls_l2_shaper_bucket_lower_entry_t));

            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index/8, &l2_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_exp_fields[idx], rate_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    rate_mant_fields[idx], rate_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_exp_fields[idx], burst_exp[i]);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    burst_mant_fields[idx], burst_mantissa[i]);
                soc_mem_field32_set(unit, config_mem[i], &l2_entry,
                                    cycle_sel_fields[idx], cycle_sel[i]);      
                if (soc_feature(unit, soc_feature_dynamic_shaper_update) &&
                    (config_mem[i] == LLS_L2_MIN_CONFIG_LOWER_Cm ||
                     config_mem[i] == LLS_L2_MIN_CONFIG_UPPER_Cm)) {
                    bucket_mem = (config_mem[i] == LLS_L2_MIN_CONFIG_LOWER_Cm) ?
                                 LLS_L2_MIN_BUCKET_LOWER_Cm :
                                 LLS_L2_MIN_BUCKET_UPPER_Cm;
                    rv = soc_mem_write(unit, config_mem[i],
                                          MEM_BLOCK_ALL, index/8, &l2_null_entry);
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_0f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_1f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_2f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_entry,
                                        NOT_ACTIVE_IN_LLS_3f, 1);                    
                    rv = soc_mem_write(unit, bucket_mem,
                                          MEM_BLOCK_ALL, index/8, &l2_bucket_entry);                       
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                }    
                if (soc_feature(unit, soc_feature_dynamic_shaper_update) &&
                    (config_mem[i] == LLS_L2_SHAPER_CONFIG_LOWERm ||
                     config_mem[i] == LLS_L2_SHAPER_CONFIG_UPPERm)) {
                    bucket_mem = (config_mem[i] == LLS_L2_SHAPER_CONFIG_LOWERm) ?
                                 LLS_L2_SHAPER_BUCKET_LOWERm :
                                 LLS_L2_SHAPER_BUCKET_UPPERm;
                    rv = soc_mem_write(unit, config_mem[i],
                                          MEM_BLOCK_ALL, index/8, &l2_null_max_entry);
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_max_entry,
                                        NOT_ACTIVE_IN_LLS_0f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_max_entry,
                                        NOT_ACTIVE_IN_LLS_1f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_max_entry,
                                        NOT_ACTIVE_IN_LLS_2f, 1);
                    soc_mem_field32_set(unit, bucket_mem, &l2_bucket_max_entry,
                                        NOT_ACTIVE_IN_LLS_3f, 1);                    
                    rv = soc_mem_write(unit, bucket_mem,
                                          MEM_BLOCK_ALL, index/8, &l2_bucket_max_entry);                       
                    if (BCM_FAILURE(rv)) {
                        SOC_EGRESS_METERING_UNLOCK(unit);
                        return(rv);
                    }
                }  
                rv = soc_mem_write(unit, config_mem[i],
                                   MEM_BLOCK_ALL, index/8, &l2_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
            }
            break;

        default:
            SOC_EGRESS_METERING_UNLOCK(unit);
            return BCM_E_PARAM;
    }
        
    SOC_EGRESS_METERING_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_bucket_get
 * Purpose:
 *     Get COS queue bandwidth control bucket setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (OUT) kbps or packet/second
 *     max_quantum   - (OUT) kbps or packet/second
 *     burst_quantum - (OUT) kbps or packet/second
 *     flags         - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_kt_cosq_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *burst_min_quantum, uint32 *burst_max_quantum,
                        uint32 flags)
{
    int rv;
    _bcm_kt_cosq_node_t *node;
    bcm_port_t local_port;
    int index;
    soc_mem_t config_mem[2];
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    uint32 rate_exp[2], rate_mantissa[2];
    uint32 burst_exp[2], burst_mantissa[2];
    int i, idx;
    int level = _BCM_KT_COSQ_NODE_LEVEL_L2;
    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXPf, C_MIN_REF_RATE_EXPf
    };
    soc_field_t rate_mant_f[] = {
        C_MAX_REF_RATE_MANTf, C_MIN_REF_RATE_MANTf
    };
    soc_field_t burst_exp_f[] = {
        C_MAX_THLD_EXPf, C_MIN_THLD_EXPf
    };
    soc_field_t burst_mant_f[] = {
        C_MAX_THLD_MANTf, C_MIN_THLD_MANTf
    };
    static const soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    static const soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    static const soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f       
    };
    static const soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    if (min_quantum == NULL || max_quantum == NULL || 
        burst_min_quantum == NULL || burst_max_quantum == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                    _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        level = node->level;    
    } 

    SOC_EGRESS_METERING_LOCK(unit);
    switch (level) {
        case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;
            rv = soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
            }
            rate_exp[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                rate_exp_f[0]);
            rate_mantissa[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                rate_mant_f[0]);
            rate_exp[1] = rate_exp[0];
            rate_mantissa[1] = rate_mantissa[0];
            burst_exp[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                burst_exp_f[0]);
            burst_mantissa[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                burst_mant_f[0]);
            burst_exp[1] = burst_exp[0];
            burst_mantissa[1] = burst_mantissa[0];
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index, &l0_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l0_entry,
                                    rate_exp_f[i]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l0_entry,
                                    rate_mant_f[i]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l0_entry,
                                    burst_exp_f[i]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l0_entry,
                                    burst_mant_f[i]);
            }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index/4, &l1_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l1_entry,
                                    rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l1_entry,
                                    rate_mant_fields[idx]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l1_entry,
                                    burst_exp_fields[idx]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l1_entry,
                                    burst_mant_fields[idx]);
             }
            break;

        case _BCM_KT_COSQ_NODE_LEVEL_L2:
            if ((index % 8) < 4) {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_LOWERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_LOWER_Cm;
            } else {
                config_mem[0] = LLS_L2_SHAPER_CONFIG_UPPERm;
                config_mem[1] = LLS_L2_MIN_CONFIG_UPPER_Cm;
            }

            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index/8, &l2_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l2_entry,
                                    rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l2_entry,
                                    rate_mant_fields[idx]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], &l2_entry,
                                    burst_exp_fields[idx]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], &l2_entry,
                                    burst_mant_fields[idx]);
            }
            break;

        default:
            SOC_EGRESS_METERING_UNLOCK(unit);
            return BCM_E_PARAM;
    }
 
    SOC_EGRESS_METERING_UNLOCK(unit);
    /* convert exp and mantissa to bps */
    rv = soc_katana_compute_shaper_rate(unit, rate_mantissa[0], rate_exp[0],
                                        max_quantum);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_compute_shaper_rate(unit, rate_mantissa[1], rate_exp[1],
                                        min_quantum);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    /* Reset min shaping rate for port */
    if(level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
       *min_quantum = 0;
    }

    rv = soc_katana_compute_shaper_burst(unit, burst_mantissa[0], burst_exp[0],
                                         burst_max_quantum);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_compute_shaper_burst(unit, burst_mantissa[1], burst_exp[1],
                                         burst_min_quantum);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }    

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_dynamic_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                        uint32 min_quantum, uint32 max_quantum,
                                        uint32 burst_min_quantum, uint32 burst_max_quantum,
                                        uint32 flags)
{
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *node, *cur_node, *l0_node;
    bcm_port_t local_port;
    int numq;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_set(unit, port, cosq, min_quantum,
                                     max_quantum, burst_min_quantum,
                                     burst_max_quantum, flags));  
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));
        mmu_info = _bcm_kt_mmu_info[unit];

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_set(unit, port, cosq, 0,
                                     max_quantum, 0,
                                     burst_max_quantum, flags));  
        
        numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
        for (cur_node = l0_node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->cosq_attached_to == cosq ||
                cur_node->cosq_attached_to == (cosq + numq)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_bucket_set(unit, cur_node->gport, 0, 
                                             min_quantum,
                                             0, burst_min_quantum,
                                             0, flags));  
                continue;
            }
        } 

    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_dynamic_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *burst_min_quantum, uint32 *burst_max_quantum,
                        uint32 flags)
{
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *node, *cur_node, *l0_node;
    bcm_port_t local_port;
    int numq;
    uint32 temp_val;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_get(unit, port, cosq,
                                     min_quantum, max_quantum,
                                     burst_min_quantum, burst_max_quantum, 
                                     flags)); 
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));
        mmu_info = _bcm_kt_mmu_info[unit];

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_get(unit, port, cosq, &temp_val,
                                     max_quantum, &temp_val,
                                     burst_max_quantum, flags));  
        
        numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
        for (cur_node = l0_node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->child != NULL && (cur_node->cosq_attached_to == cosq ||
                cur_node->cosq_attached_to == (cosq + numq))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_bucket_get(unit, cur_node->gport, 0,
                                             min_quantum,
                                             &temp_val, burst_min_quantum,
                                             &temp_val, flags));  
                break;
            }
        } 

    }

    return BCM_E_NONE;
}

/*
 *  Convert HW drop probability to percent value
 */
STATIC int
_bcm_kt_hw_drop_prob_to_percent[] = {
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
_bcm_kt_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= _bcm_kt_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_kt_drop_prob_to_percent(int drop_prob) {
   return (_bcm_kt_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * Function:
 *     _bcm_kt_cosq_wred_set
 * Purpose:
 *     Configure unicast queue WRED setting
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
_bcm_kt_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 flags, uint32 min_thresh, uint32 max_thresh,
                      int drop_probability, int gain, int ignore_enable_flags)
{
    soc_reg_t reg = 0;
    soc_field_t field;
    uint32 rval = 0;
    bcm_port_t local_port;
    int index;
    uint32 profile_index, old_profile_index, partner_index;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    soc_mem_t mems[6];
    _bcm_kt_cosq_node_t *node;
    soc_mem_t wred_mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
    soc_mem_t partner_mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
    mmu_wred_queue_config_buffer_entry_t qentry, entry;
    int rate, i;
    int wred_enabled = 0;
    int ext_mem_port_count = 0;

    reg = WRED_CONFIGr;
    if (flags & BCM_COSQ_DISCARD_DEVICE) {
/* For port -1 , set global wred discard profile for all ports */
        if ((port == -1) || (flags & BCM_COSQ_DISCARD_PACKETS)) {
            reg = GLOBAL_WRED_CONFIG_QENTRYr;
            field = WRED_ENABLEf;
            local_port = 0;
            index = 0; /* mmu init code uses service pool 0 only */
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (cosq == -1) {
           return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED,
                                        &local_port, &index, NULL));
        /* WRED doesn't work on CPU/LB Ports */
        if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }

        if (flags & BCM_COSQ_DISCARD_PACKETS) {
            wred_mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
            partner_mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                       &node));
 
                if (node->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    wred_mem = (flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_QENTRYm : 
                                MMU_WRED_OPN_CONFIG_BUFFERm;
                    partner_mem = (flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_BUFFERm : 
                                MMU_WRED_OPN_CONFIG_QENTRYm;                                
                }
            }
        field = WRED_ENf;
    }

    old_profile_index = 0xffffffff;
    profile_index = 0xffffffff;
    if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, index, &rval));
    } else {
        SOC_IF_ERROR_RETURN
           (soc_mem_read(unit, wred_mem, MEM_BLOCK_ALL,
                         index, &qentry));
    }
    if (flags & (BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL)) {
        if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
            old_profile_index = soc_reg_field_get(unit, reg, rval, PROFILE_INDEXf);
        } else {
            old_profile_index = soc_mem_field32_get(unit, wred_mem,
                                                    &qentry, PROFILE_INDEXf);
        } 
        entries[0] = &entry_tcp_green;
        entries[1] = &entry_tcp_yellow;
        entries[2] = &entry_tcp_red;
        entries[3] = &entry_nontcp_green;
        entries[4] = &entry_nontcp_yellow;
        entries[5] = &entry_nontcp_red;
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_get(unit, _bcm_kt_wred_profile[unit],
                                 old_profile_index, 1, entries));
        for (i = 0; i < 6; i++) {
            mems[i] = INVALIDm;
        }
        if (!(flags & BCM_COSQ_DISCARD_NONTCP)) {
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[0] = MMU_WRED_DROP_CURVE_PROFILE_0m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[1] = MMU_WRED_DROP_CURVE_PROFILE_1m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[2] = MMU_WRED_DROP_CURVE_PROFILE_2m;
            }
        } else if (!(flags & (BCM_COSQ_DISCARD_COLOR_GREEN |
                              BCM_COSQ_DISCARD_COLOR_YELLOW |
                              BCM_COSQ_DISCARD_COLOR_RED))) {
            mems[3] = MMU_WRED_DROP_CURVE_PROFILE_3m;
            mems[4] = MMU_WRED_DROP_CURVE_PROFILE_4m;
            mems[5] = MMU_WRED_DROP_CURVE_PROFILE_5m;

        } else {
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[3] = MMU_WRED_DROP_CURVE_PROFILE_3m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[4] = MMU_WRED_DROP_CURVE_PROFILE_4m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[5] = MMU_WRED_DROP_CURVE_PROFILE_5m;
            }
        }
        rate = _bcm_kt_percent_to_drop_prob(drop_probability);
        for (i = 0; i < 6; i++) {
            if (mems[i] == INVALIDm) {
                continue;
            }
            soc_mem_field32_set(unit, mems[i], entries[i], MIN_THDf,
                                min_thresh);
            soc_mem_field32_set(unit, mems[i], entries[i], MAX_THDf,
                                max_thresh);
            soc_mem_field32_set(unit, mems[i], entries[i], MAX_DROP_RATEf,
                                rate);
        }
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_kt_wred_profile[unit], entries, 1,
                                 &profile_index));
        if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
            soc_reg_field_set(unit, reg, &rval, PROFILE_INDEXf, profile_index);
            soc_reg_field_set(unit, reg, &rval, WEIGHTf, gain);
        } else {
            soc_mem_field32_set(unit, wred_mem, &qentry, PROFILE_INDEXf, profile_index);
            soc_mem_field32_set(unit, wred_mem, &qentry, WEIGHTf, gain);
        }
    }

    /* Some APIs only modify profiles */
    if (!ignore_enable_flags) {
        if ( reg == GLOBAL_WRED_CONFIG_QENTRYr) {
            soc_reg_field_set(unit, reg, &rval, CAP_AVERAGEf,
                              flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
            soc_reg_field_set(unit, reg, &rval, field,
                              flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
        } else {
            soc_mem_field32_set(unit, wred_mem, &qentry, CAP_AVERAGEf,
                              flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, field,
                              flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
            if (wred_mem == MMU_WRED_QUEUE_CONFIG_BUFFERm || 
                wred_mem == MMU_WRED_QUEUE_CONFIG_QENTRYm) {
                /* ECN marking is applicable only for queues */
                soc_mem_field32_set(unit, wred_mem, &qentry, ECN_MARKING_ENf,
                                    flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?
                                    1 : 0);
                if (wred_mem == MMU_WRED_QUEUE_CONFIG_QENTRYm) {
                    /* ECN marking needs to be enabled in BUFFER entry */
                    SOC_IF_ERROR_RETURN
                           (soc_mem_read(unit, MMU_WRED_QUEUE_CONFIG_BUFFERm, 
                                         MEM_BLOCK_ALL, index, &entry));
                    soc_mem_field32_set(unit, MMU_WRED_QUEUE_CONFIG_BUFFERm, 
                                       &entry, ECN_MARKING_ENf,
                                       flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?
                                       1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, MMU_WRED_QUEUE_CONFIG_BUFFERm, 
                                       MEM_BLOCK_ALL, index, &entry));
                }    
            }
        }
    }

    partner_index = 0xffffffff;
    if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, index, rval));

        /* update interna/external buffer entries based on port configuration */
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, GLOBAL_WRED_CONFIG_BUFFERIr,
                                          local_port, index, &rval));
        soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFERIr, &rval, field,
                          flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
        soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFERIr, &rval,
                          PROFILE_INDEXf, 0);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, GLOBAL_WRED_CONFIG_BUFFERIr,
                            local_port, index, rval));
        BCM_PBMP_COUNT(PBMP_EXT_MEM (unit), ext_mem_port_count);
        if (ext_mem_port_count > 0) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, GLOBAL_WRED_CONFIG_BUFFEREr,
                                              local_port, index, &rval));
            soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFEREr, &rval, field,
                              flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFEREr, &rval,
                              PROFILE_INDEXf, 0);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, GLOBAL_WRED_CONFIG_BUFFEREr,
                                local_port, index, rval));
        }
    } else {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, wred_mem, MEM_BLOCK_ALL,
                           index, &qentry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, partner_mem, MEM_BLOCK_ALL,
                              index, &qentry));
        wred_enabled = soc_mem_field32_get(unit, partner_mem,
                                           &qentry, WRED_ENf);
        
        /* if buffer/qentry_mem WRED enabled, make sure the partner_mem (qentry/buffer_mem) 
         * also WRED enabled.
         * if buffer/qentry_mem WRED disabled, make sure the partner_mem (qentry/buffer_mem) 
         * also WRED disabled. 
         */
        if (((flags & BCM_COSQ_DISCARD_ENABLE) && (wred_enabled == 0)) ||
            (!(flags & BCM_COSQ_DISCARD_ENABLE) && (wred_enabled == 1))) {
            partner_index = soc_mem_field32_get(unit, partner_mem,
                                                &qentry, PROFILE_INDEXf);                     
            soc_mem_field32_set(unit, partner_mem, &qentry, PROFILE_INDEXf, 0);
            soc_mem_field32_set(unit, partner_mem, &qentry, WRED_ENf, 
                                flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, partner_mem, MEM_BLOCK_ALL,
                               index, &qentry));    
        }    
    }

/* Replacing old profiles with new global profiles when wred discard 
   profile is set for all ports */

    if ((port == -1) && (flags & BCM_COSQ_DISCARD_DEVICE) &&
                        (profile_index != 0xffffffff)) {
        for (i = 1; i < SOC_REG_NUMELS(unit, reg); i++) {
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, 
                        _bcm_kt_wred_profile[unit], entries, 1, 
                        &profile_index));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, i, rval));
        }
    }

    if (old_profile_index != 0xffffffff && old_profile_index > 0) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_kt_wred_profile[unit],
                                    old_profile_index));
        if((port==-1) && (flags & BCM_COSQ_DISCARD_DEVICE)) {
            for(i = 1; i < SOC_REG_NUMELS(unit, reg); i++) {
                BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                             _bcm_kt_wred_profile[unit], 
                             old_profile_index));
            }
        }
    }
    
    if (partner_index != 0xffffffff && partner_index > 0) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_kt_wred_profile[unit],
                                    partner_index));
    }    

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_wred_get
 * Purpose:
 *     Get unicast queue WRED setting
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
_bcm_kt_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                      int *drop_probability, int *gain)
{
    soc_reg_t reg = 0;
    soc_field_t field;
    uint32 rval;
    bcm_port_t local_port;
    int index, profile_index;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    void *entry_p;
    soc_mem_t mem;
    _bcm_kt_cosq_node_t *node;
    soc_mem_t wred_mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
    mmu_wred_queue_config_buffer_entry_t qentry;
    int rate;

    if (*flags & BCM_COSQ_DISCARD_DEVICE) {
        if ((port == -1) || (*flags & BCM_COSQ_DISCARD_PACKETS)) {
            reg = GLOBAL_WRED_CONFIG_QENTRYr;
            field = WRED_ENABLEf;
            local_port = 0;
            index = 0; /* mmu init code uses service pool 0 only */
            wred_mem = 0;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (cosq == -1) {
           return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED,
                                        &local_port, &index, NULL));
        /* WRED doesn't work on CPU/LB Ports */
        if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
        if (*flags & BCM_COSQ_DISCARD_PACKETS) {
            wred_mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port)) {        
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                       &node));
                if (node->level == _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    wred_mem = (*flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_QENTRYm : 
                                MMU_WRED_OPN_CONFIG_BUFFERm;
                }
            }
         
        field = WRED_ENf;
    }

    if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, index, &rval));
        profile_index = soc_reg_field_get(unit, reg, rval, PROFILE_INDEXf);
    } else {
            SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, wred_mem, MEM_BLOCK_ALL,
                             index, &qentry));
            profile_index = soc_mem_field32_get(unit, wred_mem,
                                                &qentry, PROFILE_INDEXf);
    }

    if (!(*flags & BCM_COSQ_DISCARD_NONTCP)) {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
            entry_p = &entry_tcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_1m;
            entry_p = &entry_tcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_2m;
            entry_p = &entry_tcp_red;
        } else {
           mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
           entry_p = &entry_tcp_green;
        }
    } else {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
            entry_p = &entry_nontcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_4m;
            entry_p = &entry_nontcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_5m;
            entry_p = &entry_nontcp_red;
        } else {
           mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
           entry_p = &entry_nontcp_green;
        }
    }
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_kt_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_kt_drop_prob_to_percent(rate);
    }

    if (gain != NULL && reg == GLOBAL_WRED_CONFIG_QENTRYr) {
        *gain = soc_reg_field_get(unit, reg, rval, WEIGHTf);
    }
    if (gain != NULL && wred_mem != 0) {
        *gain = soc_mem_field32_get(unit, wred_mem, &qentry, WEIGHTf);
    }

    *flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE | BCM_COSQ_DISCARD_ENABLE);
    if ((reg == GLOBAL_WRED_CONFIG_QENTRYr) &&
        soc_reg_field_get(unit, reg, rval, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if ((wred_mem != 0) &&
         soc_mem_field32_get(unit, wred_mem, &qentry, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }

    if ((reg == GLOBAL_WRED_CONFIG_QENTRYr) &&
        soc_reg_field_get(unit, reg, rval, field)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if ((wred_mem != 0) &&
         soc_mem_field32_get(unit, wred_mem, &qentry, field)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }

    if (wred_mem == MMU_WRED_QUEUE_CONFIG_BUFFERm || 
        wred_mem == MMU_WRED_QUEUE_CONFIG_QENTRYm) {
        if (soc_mem_field32_get(unit, wred_mem, &qentry, ECN_MARKING_ENf)) {
            *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
    }

    return BCM_E_NONE;
}
    
STATIC int
_bcm_kt_cosq_dynamic_hierarchy_create(int unit, bcm_gport_t port, int numq)
{
    bcm_gport_t port_sched, l0_sched, l1_sched[16];
    bcm_gport_t queue;
    int i, encap_id;
    _bcm_kt_mmu_info_t *mmu_info;

    BCM_IF_ERROR_RETURN
        (bcm_kt_cosq_gport_add(unit, port, 1, 0, &port_sched));

    /* coverity[uninit_use_in_call] */
    BCM_IF_ERROR_RETURN
        (bcm_kt_cosq_gport_add(unit, port, 16, BCM_COSQ_GPORT_SCHEDULER, 
                               &l0_sched));
    BCM_IF_ERROR_RETURN
        (bcm_kt_cosq_gport_attach(unit, l0_sched, port_sched, 0));

    for (i = 0; i < (2 * numq); i++) {
        BCM_IF_ERROR_RETURN
            (bcm_kt_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_SCHEDULER, 
                                   &l1_sched[i]));
        BCM_IF_ERROR_RETURN
            (bcm_kt_cosq_gport_attach(unit, l1_sched[i], l0_sched, i));         
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    
    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_kt_cosq_gport_add(unit, port, 1,
                                   BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
        BCM_IF_ERROR_RETURN
            (bcm_kt_cosq_gport_attach(unit, queue, l1_sched[i], 0));  
        encap_id = (BCM_GPORT_SCHEDULER_GET(l1_sched[i]) >> 8) & 0x7ff;
        mmu_info->l1_gport_pair[encap_id] = l1_sched[i + numq];
        encap_id = (BCM_GPORT_SCHEDULER_GET(l1_sched[i + numq]) >> 8) & 0x7ff;
        mmu_info->l1_gport_pair[encap_id] = l1_sched[i];        
    }    
    
    return 0;
}

STATIC int
_bcm_kt_cosq_gport_delete_all(int unit, bcm_gport_t gport, int *rv)
{
    _bcm_kt_cosq_node_t *node;
    _bcm_kt_mmu_info_t *mmu_info;
    int encap_id;    
    int local_port;
    
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    if (node->child != NULL) {
        _bcm_kt_cosq_gport_delete_all(unit, node->child->gport, rv);
        BCM_IF_ERROR_RETURN(*rv);
    }

    if (node->sibling != NULL) {
        _bcm_kt_cosq_gport_delete_all(unit, node->sibling->gport, rv);
        BCM_IF_ERROR_RETURN(*rv);
    }

    if (node->level != _BCM_KT_COSQ_NODE_LEVEL_ROOT && node->cosq_attached_to >= 0) {
        *rv = bcm_kt_cosq_gport_detach(unit, node->gport, node->parent->gport, 
                                       node->cosq_attached_to);
        BCM_IF_ERROR_RETURN(*rv);
    }

    mmu_info = _bcm_kt_mmu_info[unit];

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) { /* unicast queue group */
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        /* subscriber queue group */
        encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
        if (mmu_info->num_sub_queues > 0) {
            _bcm_kt_node_index_clear(&mmu_info->sub_qlist,
                    encap_id , 1);
        } else {

            _bcm_kt_node_index_clear(&mmu_info->ext_qlist,
                                encap_id, 1);
        }
    } else {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            /* scheduler node */
            encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;

            if (encap_id == 0) {
                encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
            }
        } else if ((BCM_GPORT_IS_LOCAL(gport)) || (BCM_GPORT_IS_MODPORT(gport))) {
            BCM_IF_ERROR_RETURN
               (_bcm_kt_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            encap_id = local_port;
        } else {
            return BCM_E_PARAM;
        }

        _bcm_kt_node_index_clear(&mmu_info->sched_list, encap_id, 1);

   }
   
   if (node->cosq_map != NULL) {
       sal_free(node->cosq_map);
   } 

   _BCM_KT_COSQ_NODE_INIT(node);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_intf_ref_cnt_increment
 * Purpose:
 *     Increase interface reference count for queue id
 * Parameters:
 *     unit  - (IN) unit number
 *     queue_id - (IN) HW queue id
 *     count  - (IN) Interface count
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_kt_intf_ref_cnt_increment(int unit, uint32 queue_id, int count)
{
    _bcm_kt_mmu_info_t *mmu_info;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    mmu_info->intf_ref_cnt[queue_id] = mmu_info->intf_ref_cnt[queue_id]
                                       + count;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_intf_ref_cnt_decrement
 * Purpose:
 *     Decrease interface reference count for queue id
 * Parameters:
 *     unit  - (IN) unit number
 *     queue_id - (IN) HW queue id
 *     count  - (IN) Interface count
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_kt_intf_ref_cnt_decrement(int unit, uint32 queue_id, int count)
{
    _bcm_kt_mmu_info_t *mmu_info;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    if (mmu_info->intf_ref_cnt[queue_id] > 0) {
        mmu_info->intf_ref_cnt[queue_id] = mmu_info->intf_ref_cnt[queue_id]
                                           - count;
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_add
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
bcm_kt_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    soc_info_t *si;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_port_info_t *port_info;
    _bcm_kt_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    int id, max_nodes;
    uint32 sched_encap;
    _bcm_kt_cosq_list_t *list;
    int max_queues;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt_cosq_gport_add: unit=%d port=0x%x numq=%d flags=0x%x\n"),
              unit, port, numq, flags));

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
    case BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if ((flags ==  BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP) &&
            (si->port_num_ext_cosq[local_port] == 0)) {
            return BCM_E_PARAM;
        }    

        port_info = &mmu_info->port[local_port];
        
        for (id = port_info->q_offset; id < port_info->q_limit; id++) {
            if (mmu_info->queue_node[id].numq == 0) {
                break;
            }
        }
        
        if (id == port_info->q_limit) {
            return BCM_E_RESOURCE;
        }            

        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        break;

    case BCM_COSQ_GPORT_SUBSCRIBER:
        if (numq != 1) {
            return BCM_E_PARAM;
        }
        /* chcek for pre allocated subscriber queues */
        if (mmu_info->num_sub_queues == 0) {
            list = &mmu_info->ext_qlist;
            max_queues = mmu_info->num_ext_queues;
        } else {
            list = &mmu_info->sub_qlist;
            max_queues = mmu_info->num_sub_queues;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_node_index_get(unit, list->bits, 0, max_queues,
                                mmu_info->qset_size, 1, &id));
        _bcm_kt_node_index_set(list, id, 1);
        BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(*gport,
                                                 local_port, id);
        if (mmu_info->num_sub_queues == 0) {
            id += mmu_info->num_base_queues;
        } else {
            id += mmu_info->base_sub_queue;
        }
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        break;

    case (BCM_COSQ_GPORT_WITH_ID |  BCM_COSQ_GPORT_SUBSCRIBER):
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(*gport)
              != local_port) {
            return BCM_E_PARAM;
        }

        id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(*gport);
        if ((id >= mmu_info->num_sub_queues) ||
             (mmu_info->num_sub_queues == 0) ||
             (mmu_info->intf_ref_cnt[id] != 0)) {
            return BCM_E_PARAM;
        }
        max_queues = mmu_info->num_sub_queues;

        node = &mmu_info->queue_node[id + mmu_info->base_sub_queue];
        if(node->numq != 0) {
            return BCM_E_EXISTS;
        }
        _bcm_kt_node_index_set(&mmu_info->sub_qlist, id, 1);
        _bcm_kt_node_index_set(&mmu_info->l2_sub_qlist, id, 1);

        id += mmu_info->base_sub_queue;
        node->hw_index  = id;
        node->gport = *gport;
        node->numq = numq;
        node->level = _BCM_KT_COSQ_NODE_LEVEL_L2;
        node->type = _BCM_KT_NODE_SUBSCRIBER_GPORT_ID;
        break;

    case BCM_COSQ_GPORT_SCHEDULER:
        /* passthru */
    case 0:  
        /*
         * Can not validate actual number of cosq until attach time.
         * Maximum number of input is 8 for SP or WRR node
         * expect -1 if number of inputs are unknown at this stage(WRR only)
         */
        if (numq == 0 || numq < -1) {
            return BCM_E_PARAM;
        }
        
        if (flags == BCM_COSQ_GPORT_SCHEDULER) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_max_nodes_get(unit, _BCM_KT_COSQ_NODE_LEVEL_L2, 
                                            &max_nodes));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_max_nodes_get(unit, _BCM_KT_COSQ_NODE_LEVEL_L0, 
                                            &max_nodes));
        }
        if (numq > max_nodes) {
            return BCM_E_PARAM;
        }

        if ( flags == 0) {
            /* this is a port level scheduler */
            id = local_port;

            if ( id < 0 || id >= _BCM_KT_NUM_PORT_SCHEDULERS) {
                return BCM_E_PARAM;
            }

            _bcm_kt_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            sched_encap = (id << 8) | local_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->gport = *gport;
            node->level = _BCM_KT_COSQ_NODE_LEVEL_ROOT;
            node->hw_index = id;
            node->numq = numq;
            node->cosq_attached_to = 0;
        } else {
             /* allocate from sched_list, index will start from 36,
              * 0-35 is used for port(root) schedulers. */
            BCM_IF_ERROR_RETURN
                (_bcm_kt_node_index_get(unit, mmu_info->sched_list.bits,
                                _BCM_KT_NUM_PORT_SCHEDULERS,
                                mmu_info->num_nodes, 1, 1, &id));
            _bcm_kt_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            sched_encap = (id << 8) | local_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->gport = *gport;
            node->numq = numq;
            node->cosq_attached_to = -1;
       }
        break;
        
    case BCM_COSQ_GPORT_WITH_ID:
        if (BCM_GPORT_IS_SCHEDULER(*gport)) {
            if (numq == 0 || numq < -1 || numq > 8) {
                return BCM_E_PARAM;
            }
            id = BCM_GPORT_SCHEDULER_GET(*gport);

            if (id != local_port) {
                return BCM_E_PARAM;
            }    
            
            /* allow only port level scheduler with id */
            if ( id < 0 || id >= _BCM_KT_NUM_PORT_SCHEDULERS) {
                return BCM_E_PARAM;
            }

            _bcm_kt_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            node->gport = *gport;
            node->level = _BCM_KT_COSQ_NODE_LEVEL_ROOT;
            node->hw_index = id;
            node->numq = numq;
            node->cosq_attached_to = 0;
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(*gport)) {
            if (numq != 1) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(*gport) != local_port) {
                return BCM_E_PARAM;
            }    
            
            id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(*gport);
            port_info = &mmu_info->port[local_port];
            /* allow only base queues */
            if (id < port_info->q_offset || id >= port_info->q_limit) {
                return BCM_E_PARAM;
            }

            list = &mmu_info->l2_base_qlist; 
            if (SHR_BITGET(list->bits, id) != 0) {
                return BCM_E_RESOURCE;
            }    

            node = &mmu_info->queue_node[id];
            if (node->numq != 0) {
                return BCM_E_EXISTS;
            }
            
            _bcm_kt_node_index_set(&mmu_info->l2_base_qlist, id, 1);
            node->hw_index  = id;  
            node->gport = *gport;
            node->numq = numq;          
        } else {
            return BCM_E_PARAM;
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
 *     bcm_kt_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_kt_cosq_node_t *node;
    _bcm_kt_mmu_info_t *mmu_info;
    int encap_id;
    int rv = BCM_E_NONE;
    _bcm_kt_cosq_list_t *list;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt_mmu_info[unit];
    
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    if (node->gport < 0) {
        return BCM_E_NOT_FOUND;
    }    
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        if (node->cosq_attached_to >= 0) {
            BCM_IF_ERROR_RETURN
                (bcm_kt_cosq_gport_detach(unit, gport, node->parent->gport, 
                                          node->cosq_attached_to));
        }
        if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            if (mmu_info->num_sub_queues == 0) {
                list = &mmu_info->ext_qlist;
            } else {
                list = &mmu_info->sub_qlist;
            }
            _bcm_kt_node_index_clear(list, encap_id, 1);
        }    
        _BCM_KT_COSQ_NODE_INIT(node);
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_gport_delete_all(unit, gport, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_get
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
bcm_kt_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_kt_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, &local_port, &id, &node));

    if (SOC_USE_GPORT(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        dest.modid = modid;
        dest.port = local_port;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, port));
    } else {
        *port = local_port;
    }

    *numq = node->numq;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
            
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_SUBSCRIBER;
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        *flags = BCM_COSQ_GPORT_SCHEDULER;
    } else {
        *flags = 0;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                       port=0x%x numq=%d flags=0x%x\n"),
              *port, *numq, *flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_traverse
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
bcm_kt_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    _bcm_kt_cosq_node_t *port_info;
    _bcm_kt_mmu_info_t *mmu_info;
    bcm_port_t local_port;
    bcm_port_t port;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt_mmu_info[unit];

    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, port, &local_port));

        if (local_port < 0) {
            return BCM_E_PORT;
        }
        /* root node */
        port_info = &mmu_info->sched_node[local_port];

        if (port_info->gport >= 0) {
            _bcm_kt_cosq_gport_traverse(unit, port_info->gport, cb, user_data);
        }
      }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_mapping_set
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (IN) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_queue_t cosq)
{ 
    bcm_pbmp_t ports;
    bcm_port_t local_port;

    if (gport == -1) {    /* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else {
        return _bcm_kt_cosq_mapping_set_regular(unit, gport, priority, cosq);
    }

    PBMP_ITER(ports, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_mapping_set_regular(unit, local_port, priority, cosq));

    }    

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt_cosq_mapping_get
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (OUT) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_queue_t *cosq)
{
    _bcm_kt_mmu_info_t *mmu_info;
    bcm_port_t port;
    int numq;
    bcm_cos_queue_t hw_cosq;
    soc_field_t field = COSf;
    int index;
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entry_p;
    bcm_pbmp_t ports;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (gport == -1) {
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            BCM_IF_ERROR_RETURN
                (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
            index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                        SELECTf);
            index *= 16;

            entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_kt_cos_map_profile[unit],
                                            port_cos_map_entry_t *,
                                            index + priority);
            *cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, field);

            return BCM_E_NONE;
        }
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve
            (unit, gport, -1, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
             &port, NULL, NULL));
    } else {
        if (BCM_GPORT_IS_SET(gport)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
        } else {
            port = gport;
        }

        if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
            return BCM_E_PORT;
        }
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    numq = mmu_info->port[port].q_limit - 
           mmu_info->port[port].q_offset;
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        field = (numq <= 8) ? COSf : HG_COSf;
    }

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_kt_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    index + priority);
    hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, field);

    if (hw_cosq >= numq) {
        return BCM_E_NOT_FOUND;
    }
    *cosq = hw_cosq;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_port_sched_set
 * Purpose:
 *     Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit    - (IN) unit number
 *     pbm     - (IN) port bitmap
 *     mode    - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weights - (IN) Weights for each COS queue
 *     delay   - This parameter is not used
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For non-ETS style scheduler (CPU port) only.
 */
int
bcm_kt_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    bcm_port_t port;
    int num_cos;
    int num_weights;

    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
    
    BCM_PBMP_ITER(pbm, port) {
        num_weights = IS_CPU_PORT(unit, port) ? BCM_COS_COUNT : num_cos;
            
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_port_sched_set(unit, port, 0, mode, num_weights,
                                         (int *)weights));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_port_sched_get
 * Purpose:
 *     Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit     - (IN) unit number
 *     pbm      - (IN) port bitmap
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_XXX)
 *     weights  - (OUT) Weights for each COS queue
 *     delay    - This parameter is not used
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     For non-ETS style scheduler (CPU port) only.
 */
int
bcm_kt_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    bcm_port_t port;
    int num_cos;
    int num_weights;
    
    num_cos = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
    
    BCM_PBMP_ITER(pbm, port) {
        num_weights = IS_CPU_PORT(unit, port) ? BCM_COS_COUNT : num_cos;
        
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_port_sched_get(unit, port, 0, mode, num_weights, 
                                         weights));        
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_sched_weight_max_get
 * Purpose:
 *     Retrieve maximum weights for given COS policy
 * Parameters:
 *     unit    - (IN) unit number
 *     mode    - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight_max - (OUT) Maximum weight for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        *weight_max =
            (1 << soc_mem_field_length(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, C_WEIGHTf)) - 1;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (IN)
 *     max_quantum   - (IN)
 *     burst_quantum - (IN)
 *     flags         - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_kt_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags)
{
    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    return _bcm_kt_cosq_bucket_set(unit, port, cosq, min_quantum,
                                   max_quantum, min_quantum, 
                                   burst_quantum, flags);
}

/*
 * Function:
 *     bcm_kt_cosq_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (OUT)
 *     max_quantum   - (OUT)
 *     burst_quantum - (OUT)
 *     flags         - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_kt_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt_cosq_bucket_get(unit, port, cosq, min_quantum,
                                                max_quantum, burst_quantum,
                                                burst_quantum, *flags));
    *flags = 0;

    return BCM_E_NONE;
}

int
bcm_kt_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst_min, burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                 &burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));

    return _bcm_kt_cosq_bucket_set(unit, port, cosq, min, pps, burst_min,
                                   burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE);
}

int
bcm_kt_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst_min, burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                 &burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_kt_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst)
{
    uint32 min, max, cur_burst_min, cur_burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst_min,
                                 &cur_burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_kt_cosq_bucket_set(unit, port, cosq, min, max, cur_burst_min, burst,
                                   _BCM_XGS_METER_FLAG_PACKET_MODE);
}

int
bcm_kt_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst_min, cur_burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst_min,
                                 &cur_burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));
    *burst = cur_burst_max;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_sched_set
 * Purpose:
 *     Configure COS queue scheduler setting
 * Parameters:
 *      unit   - (IN) unit number
 *      gport  - (IN) GPORT identifier
 *      cosq   - (IN) COS queue to configure, -1 for all COS queues
 *      mode   - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *      weight - (IN) queue weight
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    int rv, numq, i, count;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    count = 0;
    for (i = 0; i < numq; i++) {
        rv = _bcm_kt_cosq_sched_set(unit, gport, cosq + i, mode, 1, &weight);
        if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        } else {
            count++;
        }
    }

    return count > 0 ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_sched_get
 * Purpose:
 *     Get COS queue scheduler setting
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any queue
 *     mode   - (OUT) Scheduling mode, one of BCM_COSQ_xxx
 *     weight - (OUT) queue weight
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int *mode, int *weight)
{
    int rv, numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        rv = _bcm_kt_cosq_sched_get(unit, gport, cosq + i, mode, 1, weight);
        if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to configure, -1 for all COS queues
 *     kbits_sec_min - (IN) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (IN) maximum bandwidth, kbits/sec
 *     flags  - (IN) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {  
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                          kbits_sec_max, kbits_sec_min,
                                          kbits_sec_max, flags));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_dynamic_bucket_set(unit, gport, cosq + i, 
                                          kbits_sec_min,
                                          kbits_sec_max, kbits_sec_min,
                                          kbits_sec_max, flags));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any COS queue
 *     kbits_sec_min - (OUT) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (OUT) maximum bandwidth, kbits/sec
 *     flags  - (OUT) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 burst_min, burst_max;

    if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                     kbits_sec_min, kbits_sec_max,
                                    &burst_min, &burst_max, *flags));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_dynamic_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                     kbits_sec_min, kbits_sec_max,
                                    &burst_min, &burst_max, *flags));    
    }
    
    *flags = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_child_node_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to configure, -1 for all COS queues
 *     kbits_sec_min - (IN) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (IN) maximum bandwidth, kbits/sec
 *     flags  - (IN) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_child_node_bandwidth_set(int unit,
                                bcm_gport_t gport, bcm_cos_queue_t cosq,
                                uint32 kbits_sec_min, uint32 kbits_sec_max,
                                uint32 flags)
{
    _bcm_kt_cosq_node_t *node = NULL;
    _bcm_kt_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    int start_cos;
    int end_cos;
    int numq, i;
    uint32 temp_kbits_sec_min = 0;
    uint32 temp_kbits_sec_max = 0;
    uint32 kbits_sec_burst_min = 0;
    uint32 kbits_sec_burst_max = 0;

    cur_gport = gport;
    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        start_cos = 0;
        end_cos = numq - 1;

    } else {
        start_cos = end_cos = cosq;
    }

    if ((BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
    }

    if (node) {
        /* For Port gport, L0, L1 scheduler nodes */
        if (cosq >= node->numq) {
            return BCM_E_PARAM;
        }
        for (i = 0, cur_node = node->child; cur_node; 
             cur_node = cur_node->sibling, i++) {
            
             /* get the child node at cosq */
             if ((cosq != -1) && (cur_node->cosq_attached_to != cosq)) {
                 continue;
             }
             cur_gport = cur_node->gport;
             BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_bucket_get(unit, cur_gport,
                                     cur_node->cosq_attached_to,
                                     &temp_kbits_sec_min,
                                     &temp_kbits_sec_max,
                                     &kbits_sec_burst_min,
                                     &kbits_sec_burst_max, 0));
             if (kbits_sec_burst_min || kbits_sec_burst_max) {
                 flags = _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION;
             }
             BCM_IF_ERROR_RETURN
              (_bcm_kt_cosq_bucket_set(unit, cur_gport,
                            cur_node->cosq_attached_to, kbits_sec_min,
                            kbits_sec_max, kbits_sec_burst_min,
                            kbits_sec_burst_max, flags));
        }
    } else {
        /* For Port and L2 queues */
        for (i = start_cos; i <= end_cos; i++) {
             BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_bucket_get(unit, cur_gport, i,
                                     &temp_kbits_sec_min,
                                     &temp_kbits_sec_max,
                                     &kbits_sec_burst_min,
                                     &kbits_sec_burst_max, 0));

            if (kbits_sec_burst_min || kbits_sec_burst_max) {
                flags = _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_bucket_set(unit, cur_gport,
                                          i, kbits_sec_min,
                                          kbits_sec_max,
                                          kbits_sec_burst_min,
                                          kbits_sec_burst_max, flags));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_child_node_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any COS queue
 *     kbits_sec_min - (OUT) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (OUT) maximum bandwidth, kbits/sec
 *     flags  - (OUT) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_child_node_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 burst_min, burst_max;
    _bcm_kt_cosq_node_t *node = NULL;
    _bcm_kt_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    int i;

    i = (cosq == -1) ? 0 : cosq;
    cur_gport = gport;
    if ((BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
        /* get the child node at cosq*/
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
        if (node != NULL) {
            if (cosq >= node->numq) { 
                return BCM_E_PARAM; 
            }
            /* resolve the child attached to input cosq */
            BCM_IF_ERROR_RETURN(
               _bcm_kt_cosq_child_node_at_input(node, i, &cur_node));
            cur_gport = cur_node->gport;

        } else {
            return BCM_E_PARAM;
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, cur_gport, i,
                                  kbits_sec_min, kbits_sec_max,
                                  &burst_min, &burst_max, *flags));
    *flags = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_bandwidth_burst_set
 * Purpose:
 *     Configure COS queue bandwidth burst setting
 * Parameters:
 *      unit        - (IN) unit number
 *      gport       - (IN) GPORT identifier
 *      cosq        - (IN) COS queue to configure, -1 for all COS queues
 *      kbits_burst - (IN) maximum burst, kbits
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int numq, i;
    uint32 kbits_sec_min, kbits_sec_max;
    uint32 kbits_sec_burst_min, kbits_sec_burst_max;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_get(unit, gport, cosq + i, &kbits_sec_min,
                                     &kbits_sec_max, &kbits_sec_burst_min, 
                                     &kbits_sec_burst_max, 0));
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                     kbits_sec_max, kbits_burst_min, 
                                     kbits_burst_max, 
                                     _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_bandwidth_burst_get
 * Purpose:
 *     Get COS queue bandwidth burst setting
 * Parameters:
 *     unit        - (IN) unit number
 *     gport       - (IN) GPORT identifier
 *     cosq        - (IN) COS queue to get, -1 for any queue
 *     kbits_burst - (OUT) maximum burst, kbits
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 &kbits_sec_min, &kbits_sec_max, 
                                 kbits_burst_min, kbits_burst_max, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_discard_set
 * Purpose:
 *     Configure gport WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to configure, -1 for all COS queues
 *     discard - (IN) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    int numq, i;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;
    bcm_port_t local_port;

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100) {
        return BCM_E_PARAM;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    cell_size = _BCM_KT_COSQ_CELLSIZE;
    if (gport != -1) {
        
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, gport, NULL, &local_port,NULL,
                                   NULL));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, gport, &local_port));
    }
    if (IS_EXT_MEM_PORT(unit, local_port)) {
        cell_size = _BCM_KT_COSQ_EXT_CELLSIZE;
    }    
   }
 

    cell_field_max = KT_CELL_FIELD_MAX;

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
        if ((min_thresh > KT_CELL_FIELD_MAX) ||
            (max_thresh > KT_CELL_FIELD_MAX)) {
            return BCM_E_PARAM;
        }
    }

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_wred_set(unit, gport, cosq + i, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_discard_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to get, -1 for any queue
 *     discard - (OUT) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;
    int cell_size;
    bcm_port_t local_port;
    
    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain));
    
    cell_size = _BCM_KT_COSQ_CELLSIZE;
    if (gport != -1) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport)) {
                BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_node_get(unit, gport, NULL, &local_port,NULL,
                                   NULL));
    } else {

        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, gport, &local_port));
    }
    if (IS_EXT_MEM_PORT(unit, local_port)) {
        cell_size = _BCM_KT_COSQ_EXT_CELLSIZE;
    }   
    }
    /* Convert number of cells to number of bytes */
    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        discard->min_thresh = min_thresh * cell_size;
        discard->max_thresh = max_thresh * cell_size;
    } else {
        discard->min_thresh = min_thresh;
        discard->max_thresh = max_thresh;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_discard_set
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     flags   - (IN) flags
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    int numq;

    if (flags & ~(BCM_COSQ_DISCARD_DEVICE |
                  BCM_COSQ_DISCARD_NONTCP |
                  BCM_COSQ_DISCARD_COLOR_ALL |
                  BCM_COSQ_DISCARD_PACKETS |
                  BCM_COSQ_DISCARD_CAP_AVERAGE |
                  BCM_COSQ_DISCARD_ENABLE |
                  BCM_COSQ_DISCARD_MARK_CONGESTION)) {
        return BCM_E_PARAM;
    }

    flags &= ~(BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL);

    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, port, -1,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_wred_set(unit, port, cosq, flags, 0, 0, 0, 0,
                                       FALSE));
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcm_kt_cosq_discard_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     flags   - (OUT) flags
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_kt_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL);
    }

    return BCM_E_NOT_FOUND;

}

/*
 * Function:
 *     bcm_kt_cosq_discard_port_set
 * Purpose:
 *     Configure port WRED setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     color         - (IN)
 *     drop_start    - (IN)
 *     drop_slot     - (IN)
 *     average_time  - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_kt_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time)
{
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_kt_cosq_discard_port_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     color         - (OUT)
 *     drop_start    - (OUT)
 *     drop_slot     - (OUT)
 *     average_time  - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_kt_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    return SOC_E_UNAVAIL;
}

int
bcm_kt_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    _bcm_kt_mmu_info_t *mmu_info;
    soc_counter_non_dma_id_t dma_id;
    _bcm_kt_cosq_node_t *node = NULL;
    _bcm_kt_cosq_node_t *child_node = NULL;
    int first_child = 1; 
    uint64  value64;
    uint64 zero;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve
        (unit, port, cosq, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    mmu_info = _bcm_kt_mmu_info[unit];
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        startq >= mmu_info->num_base_queues) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        break;
    case bcmCosqStatDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
        break;
    case bcmCosqStatOutPackets:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
        break;
    case bcmCosqStatOutBytes:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE; 
        break;
    default:
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SCHEDULER(port)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            (cosq != BCM_COS_INVALID)) {
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port, dma_id, startq, value));
        } else {
            numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
            if (numq < 1) {
                numq = 1;
            } else if (numq > 8) {
                numq = 8;
            }
            for (i = 0; i < numq; i++) {
                COMPILER_64_ZERO(zero);
                SOC_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port, dma_id, i, i == 0 ? value : zero));
            }
        }
    } else {
       BCM_IF_ERROR_RETURN
               (_bcm_kt_cosq_node_get(unit, port, 0,
                              &local_port, NULL, &node));
             if (cosq == -1) {  
                 for (i = 0; i < node->numq; i++) {
                      if ((_bcm_kt_cosq_child_node_at_input(node, i, &child_node))
                                                           == BCM_E_NOT_FOUND) {
                           continue;
                       }
                       port = child_node->gport;
                       BCM_IF_ERROR_RETURN
                       (_bcm_kt_cosq_index_resolve
                       (unit, port, 0, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
                        &local_port, &startq, &numq));
       
                        if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                            startq >= mmu_info->num_base_queues) {
                            /* pass port as queue number */
                            local_port = startq;
                            startq = 0;
                        }
                        if (first_child) {
                        /*
                         *According to api guide only first child should be set with given value
                         *all other are set to 0
                         */
                          BCM_IF_ERROR_RETURN
                          (soc_counter_set(unit, local_port, dma_id,
                                     startq , value));
                           first_child = 0;
                         }
                        else {
                          COMPILER_64_ZERO(value64);
                          BCM_IF_ERROR_RETURN
                          (soc_counter_set(unit, local_port, dma_id,
                                      startq , value64));
                         }
                   }
              }
              else { 
                    if ((_bcm_kt_cosq_child_node_at_input(node, cosq, &child_node))
                                                          == BCM_E_NOT_FOUND) {
                        return BCM_E_NOT_FOUND;  
                    }
                    port = child_node->gport;
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt_cosq_index_resolve
                      (unit, port, 0, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
                       &local_port, &startq, &numq));
       
                    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                        startq >= mmu_info->num_base_queues) {
                         /* pass port as queue number */
                         local_port = startq;
                         startq = 0;
                    }
                    BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port, dma_id,
                            startq , value));

             }
       }

    return BCM_E_NONE;
}

int
bcm_kt_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64 sum, value64;
    _bcm_kt_mmu_info_t *mmu_info;
    soc_counter_non_dma_id_t dma_id;
     _bcm_kt_cosq_node_t *node = NULL;
     _bcm_kt_cosq_node_t *child_node = NULL;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);

    if (value == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;


    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve
        (unit, port, cosq, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    mmu_info = _bcm_kt_mmu_info[unit];
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        startq >= mmu_info->num_base_queues) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        break;
    case bcmCosqStatDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
        break;
    case bcmCosqStatOutPackets:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
        break;
    case bcmCosqStatOutBytes:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
        break;
    default:
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SCHEDULER(port)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            (cosq != BCM_COS_INVALID)) {
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port, dma_id, startq, value));
        } else {
            numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
            if (numq < 1) {
                numq = 1;
            } else if (numq > 8) {
                numq = 8;
            }
            COMPILER_64_ZERO(sum);
            COMPILER_64_ZERO(value64);
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port, dma_id, i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
            *value = sum;
        }
    } else {
        BCM_IF_ERROR_RETURN
          (_bcm_kt_cosq_node_get(unit, port, 0,
                    &local_port, NULL, &node));
        COMPILER_64_ZERO(sum);
        if (cosq ==-1) {  
            for (i = 0; i < node->numq; i++) {
                 if ((_bcm_kt_cosq_child_node_at_input(node, i, &child_node))
                                                        == BCM_E_NOT_FOUND) {
                      continue;
                 }
                 port = child_node->gport;
     
                 BCM_IF_ERROR_RETURN
                   (_bcm_kt_cosq_index_resolve
                   (unit, port, 0, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
                    &local_port, &startq, &numq));
       
                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                    startq >= mmu_info->num_base_queues) {
                    /* pass port as queue number */
                    local_port = startq;
                    startq = 0;
                }
                BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port, dma_id,
                            startq , &value64));
                COMPILER_64_ADD_64(sum, value64);

           }
        }
        else { 
          if ((_bcm_kt_cosq_child_node_at_input(node, cosq, &child_node))
                                                 == BCM_E_NOT_FOUND) {
               return BCM_E_NOT_FOUND; 
          }
          port = child_node->gport;
   
          BCM_IF_ERROR_RETURN
          (_bcm_kt_cosq_index_resolve
              (unit, port, 0, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
               &local_port, &startq, &numq));
      
          if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
              startq >= mmu_info->num_base_queues) {
              /* pass port as queue number */
              local_port = startq;
              startq = 0;
          }
          BCM_IF_ERROR_RETURN
          (counter_get(unit, local_port, dma_id,
                       startq , &value64));
          COMPILER_64_ADD_64(sum, value64);
        }

        *value = sum;
    }

    return BCM_E_NONE;
}

int
bcm_kt_cosq_gport_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                 bcm_cosq_gport_stats_t stat, uint64 value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64  value64;
    _bcm_kt_mmu_info_t *mmu_info;
    soc_counter_non_dma_id_t dma_id;

    if (BCM_GPORT_IS_SCHEDULER(port) && cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve
        (unit, port, cosq, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    mmu_info = _bcm_kt_mmu_info[unit];
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        startq >= mmu_info->num_base_queues) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

    switch (stat) {
    case bcmCosqGportGreenAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_GREEN;
        break;
    case bcmCosqGportGreenAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_GREEN;
        break;
    case bcmCosqGportYellowAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_YELLOW;
        break;
    case bcmCosqGportYellowAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_YELLOW;
        break;    
    case bcmCosqGportRedAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_RED;
        break;
    case bcmCosqGportRedAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_RED;
        break;        
    case bcmCosqGportReceivedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT;
        break;
    case bcmCosqGportReceivedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE;
        break;
    case bcmCosqGportGreenDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN;
        break;
    case bcmCosqGportGreenDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_GREEN;
        break;    
    case bcmCosqGportYellowDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
        break;
    case bcmCosqGportYellowDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_YELLOW;
        break;    
    case bcmCosqGportRedDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
        break;
    case bcmCosqGportRedDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED;
        break;    
    case bcmCosqGportDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        break;
    case bcmCosqGportDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        break;
    case bcmCosqGportOutPkts:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
        break;
    case bcmCosqGportOutBytes:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
        break;    
    default:
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SCHEDULER(port)) {      
        BCM_IF_ERROR_RETURN
             (soc_counter_set(unit, local_port, dma_id, startq, value));
    } else {
        COMPILER_64_ZERO(value64);
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port, dma_id, startq + i, 
                                 value64));
        }
    }    

    return BCM_E_NONE;
}

int
bcm_kt_cosq_gport_stat_get(int unit, int sync_mode, bcm_port_t port, 
                           bcm_cos_queue_t cosq, bcm_cosq_gport_stats_t stat, 
                           uint64 *value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64 sum, value64;
    _bcm_kt_mmu_info_t *mmu_info;
    soc_counter_non_dma_id_t dma_id;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    
    if (value == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(port) && cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }    

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_index_resolve
        (unit, port, cosq, _BCM_KT_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    mmu_info = _bcm_kt_mmu_info[unit];
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        startq >= mmu_info->num_base_queues) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

    switch (stat) {
    case bcmCosqGportGreenAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_GREEN;
        break;
    case bcmCosqGportGreenAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_GREEN;
        break;
    case bcmCosqGportYellowAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_YELLOW;
        break;
    case bcmCosqGportYellowAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_YELLOW;
        break;    
    case bcmCosqGportRedAcceptedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_RED;
        break;
    case bcmCosqGportRedAcceptedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE_RED;
        break;        
    case bcmCosqGportReceivedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT;
        break;
    case bcmCosqGportReceivedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_ACCEPT_BYTE;
        break;
    case bcmCosqGportGreenDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN;
        break;
    case bcmCosqGportGreenDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_GREEN;
        break;    
    case bcmCosqGportYellowDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
        break;
    case bcmCosqGportYellowDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_YELLOW;
        break;    
    case bcmCosqGportRedDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
        break;
    case bcmCosqGportRedDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED;
        break;    
    case bcmCosqGportDroppedPkts:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
        break;
    case bcmCosqGportDroppedBytes:
        dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
        break;
    case bcmCosqGportOutPkts:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
        break;
    case bcmCosqGportOutBytes:
        dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
        break;    
    default:
        return BCM_E_PARAM;
    }

    /* if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    if (!BCM_GPORT_IS_SCHEDULER(port)) { 
        BCM_IF_ERROR_RETURN
             (counter_get(unit, local_port, dma_id, startq, value)); 
    } else {
        COMPILER_64_ZERO(sum);
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port, dma_id, startq + i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        *value = sum;
    }

    return BCM_E_NONE;
}

int bcm_kt_cosq_port_get(int unit, int queue_id, bcm_port_t *port) 
{
    _bcm_kt_mmu_info_t *mmu_info;
    lls_l0_parent_entry_t l0_parent;
    lls_l1_parent_entry_t l1_parent;
    lls_l2_parent_entry_t l2_parent;
    int index;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];
    
    if (queue_id >= mmu_info->num_queues) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
            (READ_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL, queue_id, &l2_parent));
    index = soc_mem_field32_get(unit, LLS_L2_PARENTm, &l2_parent, C_PARENTf);

    SOC_IF_ERROR_RETURN
            (READ_LLS_L1_PARENTm(unit, MEM_BLOCK_ALL, index, &l1_parent));
    index = soc_mem_field32_get(unit, LLS_L1_PARENTm, &l1_parent, C_PARENTf);

    SOC_IF_ERROR_RETURN
            (READ_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, index, &l0_parent));
    
    *port = soc_mem_field32_get(unit, LLS_L0_PARENTm, &l0_parent, C_PARENTf);;

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                     SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                     SOC_SCACHE_VERSION(1,2)
#define BCM_WB_DEFAULT_VERSION                 BCM_WB_VERSION_1_2

#define _BCM_KT_COSQ_WB_END_NODE_VALUE         0xffffffff

#define _BCM_KT_COSQ_WB_NODE_COSQ_MASK         0x3f    /* LW-1 [5:0] */
#define _BCM_KT_COSQ_WB_NODE_COSQ_SHIFT        0
#define _BCM_KT_COSQ_WB_NODE_NUMQ_MASK         0x3f    /* LW-1 [11:6] */
#define _BCM_KT_COSQ_WB_NODE_NUMQ_SHIFT        6
#define _BCM_KT_COSQ_WB_NODE_HW_INDEX_MASK     0xfff   /* LW-1 [23:12] */
#define _BCM_KT_COSQ_WB_NODE_HW_INDEX_SHIFT    12
#define _BCM_KT_COSQ_WB_NODE_LEVEL_MASK        0x3     /* LW-1 [25:24 */
#define _BCM_KT_COSQ_WB_NODE_LEVEL_SHIFT       24
#define _BCM_KT_COSQ_WB_NODE_WRR_MASK          0x1     /* LW-1 [27] */
#define _BCM_KT_COSQ_WB_NODE_WRR_SHIFT         27

#define _BCM_KT_COSQ_WB_NUM_COS_MASK         0xf    /* [3:0] */
#define _BCM_KT_COSQ_WB_NUM_COS_SHIFT        0

#define SET_FIELD(_field, _value)                       \
    (((_value) & _BCM_KT_COSQ_WB_## _field## _MASK) <<  \
     _BCM_KT_COSQ_WB_## _field## _SHIFT)
#define GET_FIELD(_field, _byte)                        \
    (((_byte) >> _BCM_KT_COSQ_WB_## _field## _SHIFT) &  \
     _BCM_KT_COSQ_WB_## _field## _MASK)

STATIC int
_bcm_kt_cosq_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    _bcm_kt_mmu_info_t *mmu_info;
    int rv, alloc_size;

    mmu_info = _bcm_kt_mmu_info[unit];

    alloc_size = 0;
    alloc_size += sizeof(mmu_info->num_base_queues);
    alloc_size += sizeof(mmu_info->num_ext_queues);
    alloc_size += sizeof(mmu_info->qset_size);
    alloc_size += sizeof(mmu_info->num_queues);
    alloc_size += sizeof(mmu_info->num_nodes);
    alloc_size += sizeof(mmu_info->max_nodes);
    alloc_size += (_BCM_KT_NUM_TOTAL_SCHEDULERS * sizeof(uint32) * 3);
    alloc_size += (_BCM_KT_NUM_L2_QUEUES * sizeof(uint32) * 2);
    alloc_size += sizeof(uint32);
    /* Storing NUM_COS */
    alloc_size += sizeof(uint8);
    /* Storing interface reference count for all queues */
    alloc_size += (BCM_MULTICAST_PORT_MAX * sizeof(uint32));

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * This funtion encodes the 1 byte of the common  data to be written in to
 * scache.Currently only last 4 bits of the byte are used for storing
 * num cos value rest can be used if necessary later.
 */
STATIC void
_bcm_kt_cosq_wb_encode_data(int unit, uint8 *scache_ptr)
 {
     uint8 data = 0;
     int   numq = 0;
     bcm_kt_cosq_config_get(unit, &numq);
     data  = SET_FIELD(NUM_COS, numq);
     *scache_ptr = data;
}

STATIC uint32
_bcm_kt_cosq_wb_encode_node(int unit, _bcm_kt_cosq_node_t *node)
{
    uint32 data = 0;

    data  = SET_FIELD(NODE_COSQ, node->cosq_attached_to);
    data |= SET_FIELD(NODE_NUMQ, node->numq);
    data |= SET_FIELD(NODE_LEVEL, node->level);
    data |= SET_FIELD(NODE_HW_INDEX, node->hw_index);
    data |= SET_FIELD(NODE_WRR, node->wrr_in_use);

    return data;
}

int
bcm_kt_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *port_node, *l0_node, *l1_node, *l2_node;
    bcm_port_t port;
    int i;
    uint32 node_data;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];

    ptr = scache_ptr;

    *(((uint32 *)ptr)) = mmu_info->num_base_queues;
    ptr += sizeof(uint32);

    *(((uint32 *)ptr)) = mmu_info->num_ext_queues;
    ptr += sizeof(uint32);

    *(((uint32 *)ptr)) = mmu_info->qset_size;
    ptr += sizeof(uint32);

    *(((uint32 *)ptr)) = mmu_info->num_queues;
    ptr += sizeof(uint32);

    *(((uint32 *)ptr)) = mmu_info->num_nodes;
    ptr += sizeof(uint32);

    for (i = 0; i < _BCM_KT_COSQ_NODE_LEVEL_MAX; i++) {
        *(((uint32 *)ptr)) = mmu_info->max_nodes[i];
        ptr += sizeof(uint32);
    }

    PBMP_ALL_ITER(unit, port) {
        port_node = &mmu_info->sched_node[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }

        if (port_node->child != NULL) {
            *(((uint32 *)ptr)) = port_node->gport;
            ptr += sizeof(uint32);
            
            node_data = _bcm_kt_cosq_wb_encode_node(unit, port_node);
            *(((uint32 *)ptr)) = node_data;
            ptr += sizeof(uint32);

            *(((uint32 *)ptr)) = port_node->base_index;
            ptr += sizeof(uint32);
        }

        for (l0_node = port_node->child; l0_node != NULL;
             l0_node = l0_node->sibling) {
            *(((uint32 *)ptr)) = l0_node->gport;
            ptr += sizeof(uint32);
            
            node_data = _bcm_kt_cosq_wb_encode_node(unit, l0_node);
            *(((uint32 *)ptr)) = node_data;
            ptr += sizeof(uint32);

            *(((uint32 *)ptr)) = l0_node->base_index;
            ptr += sizeof(uint32);

            for (l1_node = l0_node->child; l1_node != NULL;
                 l1_node = l1_node->sibling) {
                *(((uint32 *)ptr)) = l1_node->gport;
                ptr += sizeof(uint32);

                node_data = _bcm_kt_cosq_wb_encode_node(unit, l1_node);
                *(((uint32 *)ptr)) = node_data;
                ptr += sizeof(uint32);

                *(((uint32 *)ptr)) = l1_node->base_index;
                ptr += sizeof(uint32);
                
                for (l2_node = l1_node->child; l2_node != NULL;
                     l2_node = l2_node->sibling) {
                    *(((uint32 *)ptr)) = l2_node->gport;
                    ptr += sizeof(uint32);
                     
                    node_data = _bcm_kt_cosq_wb_encode_node(unit, l2_node);
                    *(((uint32 *)ptr)) = node_data;
                    ptr += sizeof(uint32);

                }
            }

        }

    }

    *(((uint32 *)ptr)) = _BCM_KT_COSQ_WB_END_NODE_VALUE;
    ptr += sizeof(uint32);

    /* add NUM_COS to the scache */
    _bcm_kt_cosq_wb_encode_data(unit, (uint8 *)ptr);
    ptr += sizeof(uint8);

    /* copy interface reference count values for all queues into scache ptr */
    sal_memcpy(ptr, mmu_info->intf_ref_cnt,
               (sizeof(uint32) * BCM_MULTICAST_PORT_MAX));
    ptr += (sizeof(uint32) * BCM_MULTICAST_PORT_MAX);

    return BCM_E_NONE; 
}
/*
 * Function:
 *     _bcm_kt_cosq_map_alloc_set
 * Purpose:
 *     Allocate the cosq_map bitmap and 
 *     restore  
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt_cosq_map_alloc_set(_bcm_kt_cosq_node_t *node)
{
    _bcm_kt_cosq_node_t *child;
    int alloc_size;
    if (node->cosq_map == NULL) {
        alloc_size = SHR_BITALLOCSIZE(node->numq);
        node->cosq_map = _bcm_kt_cosq_alloc_clear(alloc_size,
                                           "cosq_map");
        if (node->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }
    child = node->child;
    while (child != NULL) {
           BCM_IF_ERROR_RETURN
             (_bcm_kt_cosq_map_index_set(node->cosq_map, 
                                  child->cosq_attached_to, 1));
        child = child->sibling;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt_cosq_map_recover_next_level_node  
 * Purpose:
 *     Used to recover cos_map for S0,S1,L0,L1 levels   
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */


STATIC int
_bcm_kt_cosq_map_recover_next_level_node(_bcm_kt_cosq_node_t *node) 
{
    if (!node) {
        return BCM_E_NONE;
    }
    if (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(_bcm_kt_cosq_map_alloc_set(node));
    BCM_IF_ERROR_RETURN(_bcm_kt_cosq_map_recover_next_level_node(node->sibling));
    BCM_IF_ERROR_RETURN(_bcm_kt_cosq_map_recover_next_level_node(node->child));
    return BCM_E_NONE;
}

STATIC int
_bcm_set_gport_pair(int unit,_bcm_kt_cosq_node_t *node) 
{

    _bcm_kt_cosq_node_t *parent = NULL;
    _bcm_kt_cosq_node_t *curnode = NULL;
    _bcm_kt_mmu_info_t *mmu_info = NULL;
    int cosq = 0;
    int paircosq = 0;
    int numcos = 0;
    uint32 encap_id;
   
    BCM_IF_ERROR_RETURN(
       bcm_kt_cosq_config_get(unit, &numcos));
    parent = node->parent;
    if(!parent) 
    {
       return BCM_E_INTERNAL; 
    }
    cosq = node->cosq_attached_to;
    paircosq = (cosq + numcos) % (numcos * 2);
    mmu_info = _bcm_kt_mmu_info[unit];

    for(curnode = parent->child; curnode; curnode = curnode->sibling)
    {
        if (curnode->cosq_attached_to == paircosq) { 
            encap_id = (BCM_GPORT_SCHEDULER_GET(node->gport) >> 8) & 0x7ff; 
            mmu_info->l1_gport_pair[encap_id] = curnode->gport; 
            break; 
       }
    }
    return BCM_E_NONE;
}
int
bcm_kt_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    _bcm_kt_cosq_node_t *port_info;
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *port_node = NULL;
    _bcm_kt_cosq_node_t *queue_node = NULL;
    _bcm_kt_cosq_node_t *node = NULL;
    _bcm_kt_cosq_node_t *l0_node = NULL;
    _bcm_kt_cosq_node_t *l1_node = NULL;
    _bcm_kt_cosq_node_t *l2_node = NULL;
    _bcm_kt_cosq_node_t *curnode = NULL;
    _bcm_kt_cosq_node_t *l0node = NULL;
    int rv, stable_size = 0;
    bcm_port_t port = 0;
    bcm_gport_t gport;
    int i, index, numq;
    uint32 node_data;
    uint32 encap_id;
    int set_index, cosq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_profile_mem_t *profile_mem;
    _bcm_kt_cosq_list_t *list;
    soc_mem_t mem;
    mmu_wred_queue_config_buffer_entry_t qentry;
    mmu_wred_queue_config_qentry_entry_t qentry1;
    int cpu_hg_index = 0;
    uint8 data;
    int num_cos = 0;
    uint16 recovered_ver = 0;
    int additional_scache_size = 0;
    uint32 *l0_bmap = NULL;
    uint32 *l1_bmap = NULL;
    uint32 *l2_bmap = NULL;


    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];

    ptr = scache_ptr;

    mmu_info->num_base_queues = *(((uint32 *)ptr));
    ptr += sizeof(uint32);

    mmu_info->num_ext_queues = *(((uint32 *)ptr));
    ptr += sizeof(uint32);

    mmu_info->qset_size = *(((uint32 *)ptr));
    ptr += sizeof(uint32);

    mmu_info->num_queues = *(((uint32 *)ptr));
    ptr += sizeof(uint32);

    mmu_info->num_nodes = *(((uint32 *)ptr));
    ptr += sizeof(uint32);

    for (i = 0; i < _BCM_KT_COSQ_NODE_LEVEL_MAX; i++) {
        mmu_info->max_nodes[i] = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
    }

    while (*(((uint32 *)ptr)) != _BCM_KT_COSQ_WB_END_NODE_VALUE) {
        gport = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
        node_data = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
                     
        if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            list = &mmu_info->ext_qlist;
            encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id];
            encap_id -= mmu_info->num_base_queues;
            _bcm_kt_node_index_set(list, encap_id, 1);
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id];
        } else {
            encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;
            if (encap_id == 0) {
                encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
            }
            list = &mmu_info->sched_list;
            _bcm_kt_node_index_set(list, encap_id, 1);
            node = &mmu_info->sched_node[encap_id]; 
            node->first_child = 4095;
            node->base_index = *(((uint32 *)ptr));
            ptr += sizeof(uint32);
        }

        node->gport = gport;
        node->numq  = GET_FIELD(NODE_NUMQ, node_data);
        node->cosq_attached_to = GET_FIELD(NODE_COSQ, node_data);
        node->level = GET_FIELD(NODE_LEVEL, node_data);
        node->hw_index = GET_FIELD(NODE_HW_INDEX, node_data);
        node->wrr_in_use = GET_FIELD(NODE_WRR, node_data);

        switch (node->level) {
            case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
                port_node = node;
                port_node->parent = NULL;
                port_node->sibling = NULL;
                port_node->child = NULL;
                break;

            case _BCM_KT_COSQ_NODE_LEVEL_L0:
                l0_node = node;
                list = &mmu_info->l0_sched_list;
                
                if (port_node != NULL) {
                    l0_node->parent = port_node;
                    l0_node->sibling = port_node->child;
                    
                    port_node->child = l0_node;
                    port_node->num_child++;
                    
                    if (port_node->numq > 0) {
                        if (l0_node->hw_index < port_node->first_child) {
                            port_node->first_child = l0_node->hw_index;
                        }
                        if (port_node->num_child == 1) {
                            _bcm_kt_node_index_set(list, port_node->base_index, port_node->numq);
                        }
                    } else {
                        _bcm_kt_node_index_set(list, l0_node->hw_index, 1);
                    }
                }
                break;

            case _BCM_KT_COSQ_NODE_LEVEL_L1:
                l1_node = node;
                list = &mmu_info->l1_sched_list;
                if (l0_node != NULL) {
                    l1_node->parent = l0_node;
                    l1_node->sibling = l0_node->child;
                    
                    l0_node->child = l1_node;
                    l0_node->num_child++;

                    if (l0_node->numq > 0) {
                        if (l1_node->hw_index < l0_node->first_child) {
                            l0_node->first_child = l1_node->hw_index;
                        }
                        if (l0_node->num_child == 1) {
                            _bcm_kt_node_index_set(list, l0_node->base_index, l0_node->numq);
                        }
                    } else {
                        _bcm_kt_node_index_set(list, l1_node->hw_index, 1);
                    }
                }
                break;

            case _BCM_KT_COSQ_NODE_LEVEL_L2:
                queue_node = node;
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    list = &mmu_info->l2_base_qlist;
                } else {
                    list = &mmu_info->l2_ext_qlist;
                }
                if (l1_node != NULL) {
                    queue_node->parent = l1_node;
                    queue_node->sibling = l1_node->child;
                    queue_node->hw_index = GET_FIELD(NODE_HW_INDEX, node_data);
                    l1_node->child = queue_node;
                    l1_node->num_child++;

                    if (l1_node->numq > 0) {    
                        if (queue_node->hw_index < l1_node->first_child) {
                            l1_node->first_child = queue_node->hw_index;
                        }
                        if (l1_node->num_child == 1) {
                            _bcm_kt_node_index_set(list, l1_node->base_index, l1_node->numq);
                        }    
                    } else {
                        _bcm_kt_node_index_set(list, queue_node->hw_index, 1);
                    }
                }
                break;

            default:
               break;
        }
    }

    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_kt_cos_map_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }

    cpu_hg_index = SOC_IS_KATANA2(unit) ?
                   SOC_INFO(unit).cpu_hg_pp_port_index :
                   SOC_INFO(unit).cpu_hg_index;
    if (cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL,
                                         cpu_hg_index, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index, 1));
    }

    /* Update MMU_WRED_DROP_CURVE_PROFILE_x memory profile reference counter */
    profile_mem = _bcm_kt_wred_profile[unit];
    PBMP_PORT_ITER(unit, port) {
    /* 
     * Wred data can be in two tables depending on packet based or byte bases
     * we scan both and update the software profile
     */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, port, -1,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_index_resolve(unit, port, cosq,
                                            _BCM_KT_COSQ_INDEX_STYLE_WRED,
                                            NULL, &index, NULL));
            /* scan byte based wred table */ 
            mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
            SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &qentry));
            set_index = soc_mem_field32_get(unit, mem, &qentry, PROFILE_INDEXf);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, profile_mem, set_index, 1));
            
            /* scan packet based wred table */ 
            mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
            set_index = 0; 
            memset(&qentry1, 0, sizeof(mmu_wred_queue_config_qentry_entry_t));

            SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &qentry1));
            set_index = soc_mem_field32_get(unit, mem, &qentry1, PROFILE_INDEXf);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, profile_mem, set_index, 1));
        }
    }
    /* Restoring the cosq_map */ 
    PBMP_ALL_ITER(unit, port) {
    /* root node */
       port_info = &mmu_info->sched_node[port];
       node = NULL;
       if (port_info->gport >= 0) {
           BCM_IF_ERROR_RETURN(
              _bcm_kt_cosq_node_get(unit, port_info->gport, 
                               NULL, NULL, NULL, &node));
           if (node != NULL) {
                  BCM_IF_ERROR_RETURN(
                          _bcm_kt_cosq_map_recover_next_level_node(node));
                   }
        }
     }
/*
 * The following will recover the L1 gport pair 
 * if the dynamic sched update feature is enabled 
 */
    if (soc_feature(unit, soc_feature_dynamic_sched_update)) 
    { 
        PBMP_ALL_ITER(unit, port) {
          port_info = &mmu_info->sched_node[port];
          if (port_info->gport >= 0) {
               BCM_IF_ERROR_RETURN(
              _bcm_kt_cosq_node_get(unit, port_info->gport, 
                                  NULL, NULL, NULL, &node));
              if (node->child) {
                  l0node = node->child; 
/*The curnode is always L1 here*/
                  for (curnode = l0node->child; curnode; 
                                      curnode = curnode->sibling ) { 
                       _bcm_set_gport_pair(unit, curnode); 
                  }      
              }
          } 
       }
    }
    /* Recover NUM_COS */
    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        ptr += sizeof(uint32);
        data = *((uint8 *) ptr);
        num_cos = GET_FIELD(NUM_COS, data);
        if (_bcm_kt_num_cosq[unit] == 0) {
            return BCM_E_INIT;
        }
        _bcm_kt_num_cosq[unit] = num_cos;
    } else {
        additional_scache_size += sizeof(uint8);
    }

    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        ptr += sizeof(uint8);
        /* Update interface reference count values for all queues */
        sal_memcpy(mmu_info->intf_ref_cnt, ptr,
                   (sizeof(uint32) * BCM_MULTICAST_PORT_MAX));
        ptr += (sizeof(uint32) * BCM_MULTICAST_PORT_MAX);
    } else {
        /* Extra memory to store interface ref count of queues */
        additional_scache_size += (sizeof(uint32) * BCM_MULTICAST_PORT_MAX);
    }

    if (additional_scache_size > 0) {
        BCM_IF_ERROR_RETURN(
                soc_scache_realloc(unit, scache_handle, additional_scache_size));
    }

    for (port = CMIC_PORT(unit); port < (LB_PORT(unit)+1); port++) {
        l0_bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
        l1_bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
        l2_bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
        port_node = &mmu_info->sched_node[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }


        for (l0_node = port_node->child; l0_node != NULL;
                l0_node = l0_node->sibling) {
            _BCM_COSQ_PARENT_BMAP_SET(l0_bmap, l0_node->hw_index);
            for (l1_node = l0_node->child; l1_node != NULL;
                    l1_node = l1_node->sibling) {
                _BCM_COSQ_PARENT_BMAP_SET(l1_bmap, l1_node->hw_index);
                for (l2_node = l1_node->child; l2_node != NULL;
                        l2_node = l2_node->sibling) {
                    _BCM_COSQ_PARENT_BMAP_SET(l2_bmap, l2_node->hw_index);
                }
            }
        }
    }


    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcm_kt_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_init(int unit)
{
    soc_info_t *si;
    STATIC int _kt_max_cosq = -1;
    int cosq, numq, alloc_size;
    bcm_port_t port;
    soc_reg_t mem;
    STATIC soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_CURVE_PROFILE_0m, MMU_WRED_DROP_CURVE_PROFILE_1m,
        MMU_WRED_DROP_CURVE_PROFILE_2m, MMU_WRED_DROP_CURVE_PROFILE_3m,
        MMU_WRED_DROP_CURVE_PROFILE_4m, MMU_WRED_DROP_CURVE_PROFILE_5m
    };
    int entry_words[6];
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    uint32 profile_index, rval;
    _bcm_kt_mmu_info_t *mmu_info_p;
    int i, cmc;
    bcm_gport_t port_sched, l0_sched, l1_sched, queue;
    uint32 max_threshold, min_threshold;
    int num_queues, num_inputs;
    int num_base_queues = 0;

    if (_kt_max_cosq < 0) {
        _kt_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN (bcm_kt_cosq_detach(unit, 0));
    }

    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    /* Create profile for PORT_COS_MAP table */
    if (_bcm_kt_cos_map_profile[unit] == NULL) {
        _bcm_kt_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_kt_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_kt_cos_map_profile[unit]));

    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_kt_wred_profile[unit] == NULL) {
        _bcm_kt_wred_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                               "WRED Profile Mem");
        if (_bcm_kt_wred_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt_wred_profile[unit]);
    }
    entry_words[0] =
        sizeof(mmu_wred_drop_curve_profile_0_entry_t) / sizeof(uint32);
    entry_words[1] =
        sizeof(mmu_wred_drop_curve_profile_1_entry_t) / sizeof(uint32);
    entry_words[2] =
        sizeof(mmu_wred_drop_curve_profile_2_entry_t) / sizeof(uint32);
    entry_words[3] =
        sizeof(mmu_wred_drop_curve_profile_3_entry_t) / sizeof(uint32);
    entry_words[4] =
        sizeof(mmu_wred_drop_curve_profile_4_entry_t) / sizeof(uint32);
    entry_words[5] =
        sizeof(mmu_wred_drop_curve_profile_5_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, wred_mems, entry_words, 6,
                                               _bcm_kt_wred_profile[unit]));

    alloc_size = sizeof(_bcm_kt_mmu_info_t) * 1;
    if (_bcm_kt_mmu_info[unit] == NULL) {
        _bcm_kt_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_kt_mmu_info");

        if (_bcm_kt_mmu_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        _bcm_kt_cosq_free_memory(_bcm_kt_mmu_info[unit]);
    }

    sal_memset(_bcm_kt_mmu_info[unit], 0, alloc_size);

    mmu_info_p = _bcm_kt_mmu_info[unit];
    if (soc_feature(unit, soc_feature_ddr3)) {
        mmu_info_p->num_queues = soc_mem_index_count(unit, LLS_L2_PARENTm);
    } else {
        /* restrict to maximum 1K queues */
        mmu_info_p->num_queues = 1024;
    }
    
    mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_ROOT] = soc_mem_index_count(unit, LLS_PORT_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L0]   = soc_mem_index_count(unit, LLS_L0_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L1]   = soc_mem_index_count(unit, LLS_L1_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L2]   = mmu_info_p->num_queues;

    mmu_info_p->num_nodes = 0;

    if (!soc_feature(unit, soc_feature_ddr3)) {
        mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L0] /= 2; /* 512 */
        mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L1] /= 2; /* 128 */
    }    

    for (i=0; i<_BCM_KT_COSQ_NODE_LEVEL_L2; i++) {
        mmu_info_p->num_nodes += mmu_info_p->max_nodes[i];
    }

    mmu_info_p->port = _bcm_kt_cosq_alloc_clear((_BCM_KT_NUM_PORT_SCHEDULERS * 
                                              sizeof(_bcm_kt_cosq_port_info_t)),
                                              "port_info");
    if (mmu_info_p->port == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    si = &SOC_INFO(unit);
    
    for (port=0;port < (si->lb_port+1);port++) {  
        if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
            if (SOC_IS_KATANA(unit)) {
                continue;
            }
        }
        mmu_info_p->port[port].q_offset = si->port_cosq_base[port];
        mmu_info_p->port[port].q_limit = si->port_cosq_base[port] + 
                                         si->port_num_uc_cosq[port];

        SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
        soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                      mmu_info_p->port[port].q_offset);
        if (IS_HG_PORT(unit, port)) {
            soc_reg_field_set(unit, ING_COS_MODEr, &rval, COS_MODEf, 2);  
        }
        SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));

        if (SOC_IS_KATANA(unit)) {
            SOC_IF_ERROR_RETURN(READ_RQE_PORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, RQE_PORT_CONFIGr, &rval, BASE_QUEUEf,
                              mmu_info_p->port[port].q_offset);
            if (IS_EXT_MEM_PORT(unit, port)) {
                soc_reg_field_set(unit, RQE_PORT_CONFIGr, &rval, BUFF_TYPEf, 1);
            }    
            if (IS_HG_PORT(unit, port)) {
                soc_reg_field_set(unit, RQE_PORT_CONFIGr, &rval, COS_MODEf, 2);
            }   
            SOC_IF_ERROR_RETURN(WRITE_RQE_PORT_CONFIGr(unit, port, rval));

            rval = 0;
            soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, BASE_QUEUEf,
                              mmu_info_p->port[port].q_offset);
            if (IS_EXT_MEM_PORT(unit, port)) {
                soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, 
                                  BUFF_TYPEf, 1);  
            }    
            SOC_IF_ERROR_RETURN(WRITE_OP_E2ECC_PORT_CONFIGr(unit, port, rval));
        }

#if defined (BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(unit)) {
            SOC_IF_ERROR_RETURN(READ_RQE_PP_PORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                              mmu_info_p->port[port].q_offset);
            if (IS_EXT_MEM_PORT(unit, port)) {
                soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                    &rval, BUFF_TYPEf, 1);
            }    
            if (IS_HG_PORT(unit, port)) {
                soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                    &rval, COS_MODEf, 2);
            }   
            SOC_IF_ERROR_RETURN(WRITE_RQE_PP_PORT_CONFIGr(unit, port, rval));
     
            for (i = mmu_info_p->port[port].q_offset;
                 i <= (mmu_info_p->port[port].q_offset +
                       mmu_info_p->port[port].q_limit); i++) {
                SOC_IF_ERROR_RETURN (soc_mem_field32_modify(unit,
                    EGR_QUEUE_TO_PP_PORT_MAPm, i, PP_PORTf,port));
            }
        }
#endif
        num_base_queues = mmu_info_p->port[port].q_limit;
    }

    mmu_info_p->num_base_queues = soc_property_get(unit, spn_MMU_MAX_CLASSIC_QUEUES, 
                                                   num_base_queues);
    mmu_info_p->num_ext_queues    = mmu_info_p->num_queues - mmu_info_p->num_base_queues;
    mmu_info_p->qset_size         = soc_property_get(unit, spn_MMU_SUBSCRIBER_NUM_COS_LEVEL, 1);

    /* Config variable defining
     * the number of pre reserved subscriber queues
     * spn_MMU_NUM_SUBSCRIBER_QUEUES
     */
    mmu_info_p->num_sub_queues = soc_property_get(unit,
                                                   spn_MMU_NUM_SUBSCRIBER_QUEUES,
                                                   0);
    mmu_info_p->l1_gport_pair = _bcm_kt_cosq_alloc_clear((mmu_info_p->num_nodes * 
                                              sizeof(bcm_gport_t)),
                                              "l1_pair_info");
    if (mmu_info_p->l1_gport_pair == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->ext_qlist.bits  = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE(mmu_info_p->num_ext_queues),
                                              "ext_qlist");
    if (mmu_info_p->ext_qlist.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->sched_list.bits = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE(mmu_info_p->num_nodes),
                                              "sched_list");
    if (mmu_info_p->sched_list.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->l0_sched_list.bits = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE
                                            (mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L0]),
                                            "l0_sched_list");
    if (mmu_info_p->l0_sched_list.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->l1_sched_list.bits = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE
                                             (mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L1]),
                                             "l1_sched_list");
    if (mmu_info_p->l1_sched_list.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->l2_base_qlist.bits = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE(mmu_info_p->num_base_queues),
                                             "l2_base_qlist");
    if (mmu_info_p->l2_base_qlist.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->l2_ext_qlist.bits = _bcm_kt_cosq_alloc_clear(SHR_BITALLOCSIZE(mmu_info_p->num_ext_queues),
                                             "l2_ext_qlist");
    if (mmu_info_p->l2_ext_qlist.bits == NULL) {
        _bcm_kt_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    for (i = 0; i < _BCM_KT_NUM_TOTAL_SCHEDULERS; i++) {
        _BCM_KT_COSQ_LIST_NODE_INIT(mmu_info_p->sched_node, i);
    }

    for (i = 0; i < _BCM_KT_NUM_L2_QUEUES; i++) {
        _BCM_KT_COSQ_LIST_NODE_INIT(mmu_info_p->queue_node, i);
    }

    /* reserve last entry of L0 and L1 nodes */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_node_index_set(&mmu_info_p->l0_sched_list, 
        (mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L0] - 1), 1));
    BCM_IF_ERROR_RETURN
        (_bcm_kt_node_index_set(&mmu_info_p->l1_sched_list, 
        (mmu_info_p->max_nodes[_BCM_KT_COSQ_NODE_LEVEL_L1] - 1), 1));    

    if (mmu_info_p->num_sub_queues) {
        mmu_info_p->base_sub_queue = mmu_info_p->num_base_queues;
        mmu_info_p->sub_qlist.bits  = _bcm_kt_cosq_alloc_clear(
             SHR_BITALLOCSIZE(mmu_info_p->num_sub_queues),
             "sub_qlist");
        if (mmu_info_p->sub_qlist.bits == NULL) {
            _bcm_kt_cosq_free_memory(mmu_info_p);
            return BCM_E_MEMORY;
        }

        mmu_info_p->l2_sub_qlist.bits = _bcm_kt_cosq_alloc_clear(
                 SHR_BITALLOCSIZE
                 (mmu_info_p->num_sub_queues),
                 "l2_sub_qlist");

        if (mmu_info_p->l2_sub_qlist.bits == NULL) {
             _bcm_kt_cosq_free_memory(mmu_info_p);
             return BCM_E_MEMORY;
        }

        BCM_IF_ERROR_RETURN
         (_bcm_kt_node_index_set(&mmu_info_p->l2_ext_qlist,
                                  mmu_info_p->base_sub_queue,
                                  mmu_info_p->num_sub_queues));
        BCM_IF_ERROR_RETURN
         (_bcm_kt_node_index_set(&mmu_info_p->ext_qlist,
                                  mmu_info_p->base_sub_queue,
                                  mmu_info_p->num_sub_queues));

    } else {
       mmu_info_p->base_sub_queue = 0;
    }

    
    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        
     for (port=0;port < (si->lb_port+1);port++) {  
         if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
             if (SOC_IS_KATANA(unit)) {
                 continue;
             }
         }
        if (IS_CPU_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt_cosq_gport_add(unit, port, SOC_CMCS_NUM(unit), 
                               0, &port_sched));
            for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
                if (NUM_CPU_ARM_COSQ(unit, cmc) == 0) {
                    continue;
                } 
                cosq = (NUM_CPU_ARM_COSQ(unit, cmc) + 7) / 8;
                /* coverity[uninit_use_in_call] */
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_add(unit, port, cosq, 
                                   BCM_COSQ_GPORT_SCHEDULER, &l0_sched));
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_attach(unit, l0_sched, port_sched, cmc));  
                cosq = 0;
                num_queues = NUM_CPU_ARM_COSQ(unit, cmc);
                for (i = 0; i < NUM_CPU_ARM_COSQ(unit, cmc); i++) {
                    if (i % 8 == 0) {
                        num_inputs = (num_queues >= 8) ? 8 : num_queues;
                        BCM_IF_ERROR_RETURN
                            (bcm_kt_cosq_gport_add(unit, port, num_inputs, 
                                           BCM_COSQ_GPORT_SCHEDULER, &l1_sched));
                        BCM_IF_ERROR_RETURN
                             (bcm_kt_cosq_gport_attach(unit, l1_sched, l0_sched, cosq));  
                        cosq++;
                        num_queues -= num_inputs;
                    } 
                    BCM_IF_ERROR_RETURN
                        (bcm_kt_cosq_gport_add(unit, port, 1,
                                       BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
                    BCM_IF_ERROR_RETURN
                        (bcm_kt_cosq_gport_attach(unit, queue, l1_sched, (i % 8)));  

                }
            } 
        } else {
            if (!soc_feature(unit, soc_feature_dynamic_sched_update)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_add(unit, port, 1, 0, &port_sched));
                /* coverity[uninit_use_in_call] */
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_SCHEDULER, 
                                              &l0_sched));
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_attach(unit, l0_sched, port_sched, 0));  
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_add(unit, port, numq, BCM_COSQ_GPORT_SCHEDULER, 
                                              &l1_sched));
                BCM_IF_ERROR_RETURN
                    (bcm_kt_cosq_gport_attach(unit, l1_sched, l0_sched, 0));  
                for (cosq = 0; cosq < numq; cosq++) {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt_cosq_gport_add(unit, port, 1,
                                       BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
                    BCM_IF_ERROR_RETURN
                         (bcm_kt_cosq_gport_attach(unit, queue, l1_sched, cosq));  

                }
           } else {    
               BCM_IF_ERROR_RETURN
                   (_bcm_kt_cosq_dynamic_hierarchy_create(unit, port, numq));
           }
        }
    }
    
   }

   if (!cosq_sync_lock[unit]) {
        cosq_sync_lock[unit] = sal_mutex_create("KT cosq sync lock");
        if (cosq_sync_lock[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* Add default entries for PORT_COS_MAP memory profile */
    BCM_IF_ERROR_RETURN(bcm_kt_cosq_config_set(unit, numq));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_kt_cosq_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_kt_cosq_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    
    sal_memset(&entry_tcp_green, 0, sizeof(entry_tcp_green));
    sal_memset(&entry_tcp_yellow, 0, sizeof(entry_tcp_yellow));
    sal_memset(&entry_tcp_red, 0, sizeof(entry_tcp_red));
    sal_memset(&entry_nontcp_green, 0, sizeof(entry_nontcp_green));
    sal_memset(&entry_nontcp_yellow, 0, sizeof(entry_nontcp_yellow));
    sal_memset(&entry_nontcp_red, 0, sizeof(entry_nontcp_red));
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    max_threshold = (1 << soc_mem_field_length(unit, MMU_WRED_DROP_CURVE_PROFILE_0m, 
                     MAX_THDf)) - 1;
    min_threshold = (1 << soc_mem_field_length(unit, MMU_WRED_DROP_CURVE_PROFILE_0m, 
                     MIN_THDf)) - 1;    
    for (i = 0; i < 6; i++) {
        soc_mem_field32_set(unit, wred_mems[i], entries[i], MIN_THDf,
                            max_threshold);
        soc_mem_field32_set(unit, wred_mems[i], entries[i], MAX_THDf,
                            min_threshold);
    }    
    profile_index = 0xffffffff;
    for (port=0;port < (si->lb_port+1);port++) {  
        if (!SOC_PBMP_MEMBER((PBMP_ALL(unit)), port)) {
            if (SOC_IS_KATANA(unit)) {
                continue;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve(unit, port, -1,
                                        _BCM_KT_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, _bcm_kt_wred_profile[unit],
                                         entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                               _bcm_kt_wred_profile[unit],
                                               profile_index, 0));
            }
        }
    }
    /*
     * Set all CPU queues to disable enqueue.  Enqueue will be enabled when
     * RX DMA is enabled. This is to prevent packets from getting wedged in
     * the CMIC when they don't have a means of egressing from the CMIC.
     */
    if (!(SAL_BOOT_SIMULATION) && !SOC_IS_RCPU_ONLY(unit)) {
        for (i = 0; i < NUM_CPU_COSQ(unit); i++) {
            rval = 0;
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ID0f, i);
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_NUMf, 1);
            if (SOC_PBMP_MEMBER (PBMP_EXT_MEM (unit), CMIC_PORT(unit))) {
                soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_EXTERNALf, 1);
            }
            soc_reg_field_set(unit,
                TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ACTIVEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOQ_QUEUE_FLUSH0r(unit, rval));
            while (rval) {
                SOC_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));
                rval = soc_reg_field_get(unit,
                           TOQ_QUEUE_FLUSH0r, rval, Q_FLUSH_ACTIVEf);
            }
            rval = 0;
            soc_reg_field_set(unit, TOQ_ERRORr, &rval, FLUSH_COMPLETEf, 1); /* w1tc */
            SOC_IF_ERROR_RETURN(WRITE_TOQ_ERRORr(unit, rval));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_detach(int unit, int software_state_only)
{
   if (cosq_sync_lock[unit]) {
        sal_mutex_destroy(cosq_sync_lock[unit]);
        cosq_sync_lock[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_config_set
 * Purpose:
 *     Set the number of COS queues
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1], *hg_entries[1];
    uint32 index, hg_index;
    int count, hg_count;
    bcm_port_t port;
    int cos, prio;
    uint32 i;
    int cpu_hg_index = 0, ref_count;
#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    port_cos_map_entry_t hg_cos_map_entries[16];
#endif

    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }

    /* clear out old profiles. */
    index = 0;
    while (index < soc_mem_index_count(unit, PORT_COS_MAPm)) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit,
                                      _bcm_kt_cos_map_profile[unit],
                                      index, &ref_count));
        if (ref_count > 0) {
            while (ref_count) {
                BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                                        _bcm_kt_cos_map_profile[unit], index));
                ref_count -= 1;
            }
        }
        index += 16;
    }

    /* Distribute first 8 internal priority levels into the specified number
     * of cosq, map remaining internal priority levels to highest priority
     * cosq */
    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;
    hg_entries[0] = &cos_map_entries;
    prio = 0;
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                COSf, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio], COSf,
                            numq - 1);
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* Use identical mapping for Higig port */
    sal_memset(hg_cos_map_entries, 0, sizeof(hg_cos_map_entries));
    hg_entries[0] = &hg_cos_map_entries;
    prio = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            COSf, prio);
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            COSf, 7);
    }

#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt_cos_map_profile[unit], entries, 16,
                             &index));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt_cos_map_profile[unit], hg_entries,
                             16, &hg_index));
    count = 0;
    hg_count = 0;
    PBMP_ALL_ITER(unit, port) {
        if (IS_HG_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        hg_index / 16));
            hg_count++;
        } else {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        index / 16));
            count++;
        }
    }

    cpu_hg_index = SOC_IS_KATANA2(unit) ?
                   SOC_INFO(unit).cpu_hg_pp_port_index :
                   SOC_INFO(unit).cpu_hg_index;
    if (cpu_hg_index != -1) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    cpu_hg_index, SELECTf,
                                    hg_index / 16));
        hg_count++;
    }


    for (i = 1; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_kt_cos_map_profile[unit], index,
                                  0);
    }
    for (i = 1; i < hg_count; i++) {
        soc_profile_mem_reference(unit, _bcm_kt_cos_map_profile[unit],
                                  hg_index, 0);
    }

    _bcm_kt_num_cosq[unit] = numq;

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_config_get(int unit, int *numq)
{
    if (_bcm_kt_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _bcm_kt_num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_attach
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
bcm_kt_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt_cosq_node_t *sched_node, *input_node, *temp_parent, *temp_sibling;
    bcm_port_t sched_port, input_port, local_port;
    int rv, max_nodes;
    int status = 0;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport)) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport))) {
        return BCM_E_PORT;
    }

    if(!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_SCHEDULER(sched_gport))) {
        return BCM_E_PORT;
     }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to >= 0) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport) ||
        BCM_GPORT_IS_LOCAL(input_gport) ||
        BCM_GPORT_IS_MODPORT(input_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                   &input_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_localport_resolve(unit, input_gport, &input_port));
        input_node = NULL;
    }

    if (sched_port != input_port) {
        return BCM_E_PORT;
    }

      /* Identify the levels of schedulers
       * input_port == phy_port && sched_gport == scheduler => sched_gport = L0
       * input_port == schduler && sched_gport == scheduler => input_port = L0 and sched_port = L1
       * input_port == scheduler && sched_port == queue_group => input_port = L1 and sched_port = L2
       */
    if (input_node != NULL) {
        if (!BCM_GPORT_IS_SCHEDULER(input_gport)) {
            if (input_node->numq == 0) {
                local_port = (BCM_GPORT_IS_LOCAL(input_gport)) ?
                              BCM_GPORT_LOCAL_GET(input_gport) :
                              BCM_GPORT_MODPORT_PORT_GET(input_gport);
                input_node->gport = input_gport;
                input_node->hw_index = local_port;
                input_node->level = _BCM_KT_COSQ_NODE_LEVEL_ROOT;
                input_node->cosq_attached_to = 0;
                input_node->numq = 4; /* Assign max of 4 L0 nodes */
                input_node->base_index = -1;
            }

            if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
                return BCM_E_PARAM;
            }

            sched_node->level = _BCM_KT_COSQ_NODE_LEVEL_L0;
        } else {
             if (input_node->level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
                 /* Don't allow non-scheduler node (queues) to be attached to port */ 
                 if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
                     return BCM_E_PARAM;
                 }
                 sched_node->level = _BCM_KT_COSQ_NODE_LEVEL_L0;
                 BCM_IF_ERROR_RETURN
                     (_bcm_kt_cosq_max_nodes_get(unit, sched_node->level, &max_nodes));
                 if (input_node->numq == -1) {
                     input_node->numq = max_nodes;
                 } else {
                     if (input_node->numq > max_nodes) {
                         return BCM_E_PARAM;
                     }
                 }
             }
             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport)) {
                if (!BCM_GPORT_IS_SCHEDULER(input_gport)) {
                    return BCM_E_PARAM;
                }
                if (input_node->level != _BCM_KT_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
            }

             if ((input_node->level == -1)) {
                input_node->level = (BCM_GPORT_IS_SCHEDULER(sched_gport)) ?
                                     _BCM_KT_COSQ_NODE_LEVEL_L0 : _BCM_KT_COSQ_NODE_LEVEL_L1;
             }

             if (sched_node->level == -1) {
                 sched_node->level = (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                                      BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport)) ?
                                      _BCM_KT_COSQ_NODE_LEVEL_L2 : _BCM_KT_COSQ_NODE_LEVEL_L1;
                 BCM_IF_ERROR_RETURN
                     (_bcm_kt_cosq_max_nodes_get(unit, sched_node->level, &max_nodes));
                 if (input_node->numq == -1) {
                     input_node->numq = max_nodes;
                 } else {
                     if (input_node->numq > max_nodes) {
                         return BCM_E_PARAM;
                     }
                 }
             }
        }

        if (input_node->cosq_attached_to < 0) {
            /* Only allow to attach to a node that has already attached */
            return BCM_E_PARAM;
        }
        if (cosq < -1 || (input_node->numq != -1 && cosq >= input_node->numq)) {
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport) || BCM_GPORT_IS_LOCAL(input_gport) ||
        BCM_GPORT_IS_MODPORT(input_gport)) {
        if (input_node->cosq_attached_to < 0) {
            /* dont allow to attach to a node that has already attached */
            return BCM_E_PARAM;
        }
        temp_parent = sched_node->parent;
        temp_sibling = sched_node->sibling;

        sched_node->parent = input_node;
        sched_node->sibling = input_node->child;
        input_node->child = sched_node;
        /* resolve the nodes */
        rv = _bcm_kt_cosq_node_resolve(unit, sched_node, cosq);
        if (BCM_FAILURE(rv)) {
            input_node->child = sched_node->sibling;
            sched_node->parent = temp_parent;
            sched_node->sibling = temp_sibling;
            sched_node->cosq_attached_to = -1;
            return rv;
        }
        /* Protect LLS hierachy change as
         * port flush on link flap might be effected
         * when there is changed in the LLS hierarchy
         */
        SOC_LLS_SCHEDULER_LOCK(unit);
        soc_kt_port_flush_state_get(unit, input_port, &status); 
        if (status) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
             "Unit %d Port %d is in flush state, Cannot change LLS hierarchy."
             "\n Attach of Cosq hw_cosq=%d failed.\n"),
            unit, input_port, sched_node->cosq_attached_to));
            input_node->child = sched_node->sibling;
            SOC_LLS_SCHEDULER_UNLOCK(unit);  
            return BCM_E_RESOURCE;
        }
        rv = _bcm_kt_cosq_sched_node_set(unit, sched_gport, 
                            _BCM_KT_COSQ_STATE_ENABLE);
        SOC_LLS_SCHEDULER_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "                         hw_cosq=%d\n"),
                  sched_node->cosq_attached_to));
    } else {
            return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_detach
 * Purpose:
 *     Detach sched_port to the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to attach to
 *     cosq       - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt_cosq_node_t *sched_node, *input_node, *prev_node;
    bcm_port_t sched_port, input_port;
    int rv; 
    int status = 0; 

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport)) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to < 0) {
        /* Not attached yet */
        return BCM_E_PORT;
    }

    if (input_gport != BCM_GPORT_INVALID) {

        if (BCM_GPORT_IS_SCHEDULER(input_gport) ||
            BCM_GPORT_IS_LOCAL(input_gport) ||
            BCM_GPORT_IS_MODPORT(input_gport)) {
               BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                       &input_node));

        }
        else {

            if (!(BCM_GPORT_IS_SCHEDULER(sched_gport) ||
                  BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport))) {
                return BCM_E_PARAM;
            }
            else {
                BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_localport_resolve(unit, input_gport,
                                                &input_port));
                input_node = NULL;
            }
        }
    }

    if (sched_port != input_port) {
        return BCM_E_PORT;
    }

    if (sched_node->parent != input_node) {
        return BCM_E_PORT;
    }

    if (cosq < -1 || (input_node->numq != -1 && cosq >= input_node->numq)) {
        return BCM_E_PARAM;
    }    

    if (cosq != -1) {
        if (sched_node->cosq_attached_to != cosq) {
            return BCM_E_PARAM;
        }
    }
    /* Update the hw accordingly
     * Protect LLS hierachy change as
     * port flush on link flap might be effected
     * when there is a change in the LLS hierarchy
     */
    SOC_LLS_SCHEDULER_LOCK(unit);
    soc_kt_port_flush_state_get(unit, input_port, &status);
    if (status) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
        " Unit %d Port %d is in flush state, Cannot change LLS hierarchy."
         "\n Detach of Cosq hw_cosq=%d failed.\n"),
            unit, input_port, sched_node->cosq_attached_to));  
        SOC_LLS_SCHEDULER_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    rv = _bcm_kt_cosq_sched_node_set(unit, sched_gport,
                                     _BCM_KT_COSQ_STATE_DISABLE);
    SOC_LLS_SCHEDULER_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
       return rv;
    }
    /* unresolve the node - delete this node from parent's child list */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_unresolve(unit, sched_node, cosq));

    /* now remove from the sw tree */
    if (sched_node->parent != NULL) {
        if (sched_node->parent->child == sched_node) {
            sched_node->parent->child = sched_node->sibling;
        } else {
            prev_node = sched_node->parent->child;
            while (prev_node != NULL && prev_node->sibling != sched_node) {
                prev_node = prev_node->sibling;
            }
            if (prev_node == NULL) {
                return BCM_E_INTERNAL;
            }
            prev_node->sibling = sched_node->sibling;
       }
        sched_node->parent = NULL;
        sched_node->sibling = NULL;
        sched_node->wrr_in_use = 0;
        sched_node->cosq_attached_to = -1;
        if (sched_node->type !=
             _BCM_KT_NODE_SUBSCRIBER_GPORT_ID) {
            sched_node->hw_index = -1;
        }
     }

      LOG_INFO(BSL_LS_BCM_COSQ,
               (BSL_META_U(unit,
                           "                         hw_cosq=%d\n"),
                sched_node->cosq_attached_to));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_gport_attach_get
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_kt_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_kt_cosq_node_t *sched_node;
    bcm_module_t modid, modid_out;
    bcm_port_t port, port_out;

    if (input_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, sched_gport, &modid, &port, NULL,
                               &sched_node));

    if (sched_node->level == _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
        return BCM_E_PARAM;
    }    
    
    if (sched_node->cosq_attached_to < 0) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (sched_node->parent != NULL) { 
        *input_gport = sched_node->parent->gport;
    } else {  /* sched_node is in L2 level */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port,
                                    &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(*input_gport, modid_out, port_out);
    }
    *cosq = sched_node->cosq_attached_to;

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_ef_update_war(int unit, bcm_gport_t gport,  bcm_cos_queue_t cosq,
                           int hw_index, lls_l2_parent_entry_t *entry)
{
    lls_l1_config_entry_t l1_config;
    uint32 min_bw, max_bw, min_burst, max_burst, flags = 0;
    int parent_index, spri_vector;
    uint32 start_pri, vec_3_0 =0, vec_7_4 = 0;
    uint32 rval = 0, bit_offset;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "_bcm_kt_cosq_ef_update_war:unit=%d gport=0x%x cosq=%d idx=%d\n"),
              unit, gport, cosq, hw_index));
   
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_get(unit, gport, cosq, &min_bw, &max_bw,
                                 &min_burst, &max_burst, flags)); 

    if ((min_bw == 0) && (max_bw == 0)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                           hw_index, entry));
    } else {
        parent_index = soc_mem_field32_get(unit, LLS_L2_PARENTm, entry, 
                                           C_PARENTf); 
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, LLS_L1_CONFIGm, MEM_BLOCK_ALL, parent_index, 
                          &l1_config));
        if (SOC_IS_KATANA2(unit)) {
            start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_config, 
                                            P_START_UC_SPRIf); 
        } else {
            start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_config, 
                                            P_START_SPRIf); 
        }
        vec_3_0 = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_config, 
                                      P_NUM_SPRIf);
        vec_7_4 = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_config, 
                                      P_VECT_SPRI_7_4f);
        spri_vector = vec_3_0 | (vec_7_4 << 4);
        bit_offset = hw_index - start_pri;
        if (bit_offset >= 8) {
            return BCM_E_PARAM;
        }
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "parent=%d spri_vector=0x%x bit_offset=%d\n"),
                  parent_index, spri_vector, bit_offset));
        spri_vector &= ~(1 << bit_offset);
        soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_config, P_NUM_SPRIf, 
                            (spri_vector & 0xF));
        soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_config, P_VECT_SPRI_7_4f, 
                            ((spri_vector >> 4) & 0xF));
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_set(unit, gport, cosq, 0, 0,
                                     min_burst, max_burst, flags)); 
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                           hw_index, entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_L1_CONFIGm, MEM_BLOCK_ALL,
                           parent_index, &l1_config));

        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          INJECTf, 1);
        /* 0 = enqueue event,1 = shaper event,2 = xon event,3 = xoff event */ 
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_TYPEf, 1);
        /* 0 = port, 1 = level 0, 2 = level 1, 3 = level 2 */
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_LEVELf, 3);
        soc_reg_field_set(unit, LLS_DEBUG_INJECT_ACTIVATIONr, &rval, 
                          ENTITY_IDENTIFIERf, hw_index); 
        SOC_IF_ERROR_RETURN(WRITE_LLS_DEBUG_INJECT_ACTIVATIONr(unit, rval)); 
        spri_vector |= (1 << bit_offset);
        soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_config, P_NUM_SPRIf, 
                            (spri_vector & 0xF));
        soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_config, P_VECT_SPRI_7_4f, 
                            ((spri_vector >> 4) & 0xF));
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_L1_CONFIGm, MEM_BLOCK_ALL,
                           parent_index, &l1_config));
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_bucket_set(unit, gport, cosq, min_bw, max_bw,
                                     min_burst, max_burst, flags)); 
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_valid_ef_node(int unit, lls_l1_config_entry_t *entry, 
                           int hw_index)
{
    lls_l2_parent_entry_t l2_parent;
    int index, start_pri, spri_vector, num_spri, vec_7_4;
    uint32 bit_offset;
    int i;

    if (SOC_IS_KATANA2(unit)) {
        start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, entry, 
                                        P_START_UC_SPRIf); 
    } else {
        start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, entry, 
                                        P_START_SPRIf); 
    }
    num_spri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, entry, 
                                   P_NUM_SPRIf);
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        vec_7_4 = soc_mem_field32_get(unit, LLS_L1_CONFIGm, entry, 
                                      P_VECT_SPRI_7_4f);
        spri_vector = num_spri | (vec_7_4 << 4);
        bit_offset = hw_index - start_pri;
        if (bit_offset >= 8) {
            return BCM_E_PARAM;
        }
        if (!(spri_vector & (1 << bit_offset))) {
            /* werr node cannot be EF */
            return BCM_E_PARAM;
        }
        for (i = 0; i < 8; i++) {
            if (spri_vector & (1 << i)) {
                index = start_pri + i;
                SOC_IF_ERROR_RETURN
                     (soc_mem_read(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                                   index, &l2_parent));
                if (soc_mem_field32_get(unit, LLS_L2_PARENTm, 
                                        &l2_parent, C_EFf) == 1) {
                    /* maximum of 1 EF node is supported */
                    return FALSE;
                }
            }
        }
    } else {
        if ((hw_index < start_pri) || (hw_index >= (start_pri + num_spri))) {
            /* werr node cannot be EF */
            return BCM_E_PARAM;
        } 
        for (i = start_pri; i < (start_pri + num_spri); i++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                              i, &l2_parent));
            if (soc_mem_field32_get(unit, LLS_L2_PARENTm,
                                    &l2_parent, C_EFf) == 1) {
                /* maximum of 1 EF node is supported */
                return FALSE;
            }
        }
    }
   
    return TRUE;
}

STATIC int
_bcm_kt_cosq_ef_op_at_index(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int hw_index, _bcm_cosq_op_t op, int *ef_val)
{
    lls_l2_parent_entry_t entry;
    lls_l1_parent_entry_t l1_entry;
    lls_l1_config_entry_t l1_config;
    lls_l0_config_entry_t l0_config;
    uint32 current_ef = 0, set_ef = 0;
    uint32 index;

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL, hw_index, &entry));

    if (op == _BCM_COSQ_OP_GET) {
        *ef_val = soc_mem_field32_get(unit, LLS_L2_PARENTm, &entry, C_EFf);
    } else {
        current_ef = soc_mem_field32_get(unit, LLS_L2_PARENTm, &entry, C_EFf);
        set_ef = (*ef_val != 0) ? 1 : 0;
        soc_mem_field32_set(unit, LLS_L2_PARENTm, &entry, C_EFf, set_ef);
        if ((current_ef == 0) && (set_ef == 1)) {
            index = soc_mem_field32_get(unit, LLS_L2_PARENTm, &entry, 
                                        C_PARENTf); 
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_CONFIGm, MEM_BLOCK_ALL, 
                              index, &l1_config));
            if (_bcm_kt_cosq_valid_ef_node(unit, &l1_config, 
                                           hw_index) != TRUE) {
                 return BCM_E_PARAM;
            }  
            soc_mem_field32_set(unit, LLS_L1_CONFIGm, &l1_config, 
                                P_CFG_EF_PROPAGATEf, set_ef);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L1_CONFIGm, MEM_BLOCK_ALL,
                               index, &l1_config));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_PARENTm, MEM_BLOCK_ALL, 
                              index, &l1_entry));
            index = soc_mem_field32_get(unit, LLS_L1_PARENTm, &l1_entry, 
                                        C_PARENTf); 
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L0_CONFIGm, MEM_BLOCK_ALL, 
                              index, &l0_config));
            soc_mem_field32_set(unit, LLS_L0_CONFIGm, &l0_config, 
                                P_CFG_EF_PROPAGATEf, set_ef);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L0_CONFIGm, MEM_BLOCK_ALL,
                               index, &l0_config));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                               hw_index, &entry));
        } else if ((current_ef == 1) && (set_ef == 0)) {
            if (soc_feature(unit, soc_feature_vector_based_spri)) {
                return _bcm_kt_cosq_ef_update_war(unit, gport, cosq, 
                                                  hw_index, &entry);
            }  
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L2_PARENTm, MEM_BLOCK_ALL,
                               hw_index, &entry));
        } else {
            return BCM_E_NONE;     
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_ef_op(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, int *arg,
                   _bcm_cosq_op_t op)
{
    bcm_port_t local_port;
    int index, numq, from_cos, to_cos, ci;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve
             (unit, gport, cosq, _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
              &local_port, &index, NULL));
        return _bcm_kt_cosq_ef_op_at_index(unit, gport, cosq, 
                                           index, op, arg);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt_cosq_index_resolve
             (unit, gport, cosq, _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
              &local_port, &index, &numq));
        if (cosq == BCM_COS_INVALID) {
            /* Maximum 1 spri can be EF enabled */
            from_cos = to_cos = 0;
        } else {
            from_cos = to_cos = cosq;
        }
        for (ci = from_cos; ci <= to_cos; ci++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt_cosq_index_resolve
                 (unit, gport, ci, _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                  &local_port, &index, NULL));

            BCM_IF_ERROR_RETURN(_bcm_kt_cosq_ef_op_at_index(unit,
                                gport, ci, index, op, arg));
            if (op == _BCM_COSQ_OP_GET) {
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_port_qnum_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    if (!arg) {
        return BCM_E_PARAM;
    }

    if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (type == bcmCosqControlPortQueueUcast) {
        *arg = SOC_INFO(unit).port_uc_cosq_base[local_port] + cosq;
    } else {
        *arg = SOC_INFO(unit).port_cosq_base[local_port] + cosq;
    }

    return BCM_E_NONE;
}

int
bcm_kt_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt_cosq_control_set: unit=%d gport=0x%x cosq=%d \
                         type=%d arg=%d\n"),
              unit, gport, cosq, type, arg));
    switch (type) {
    case bcmCosqControlEfPropagation:
        return _bcm_kt_cosq_ef_op(unit, gport, cosq, &arg, _BCM_COSQ_OP_SET);
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_kt_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt_cosq_control_get: unit=%d gport=0x%x cosq=%d type=%d\n"),
              unit, gport, cosq, type));
    switch (type) {
    case bcmCosqControlEfPropagation:
        return _bcm_kt_cosq_ef_op(unit, gport, cosq, arg, _BCM_COSQ_OP_GET);
    case bcmCosqControlPortQueueUcast:
    case bcmCosqControlPortQueueMcast:
        return _bcm_kt_cosq_port_qnum_get(unit, gport, cosq, type, arg);
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_kt_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_OP_E2ECC_PORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, 
                      COS_MASKf,  enable ? 0xFF : 0x0);
    soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, 
                      E2ECC_RPT_ENf, enable ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_OP_E2ECC_PORT_CONFIGr(unit, port, rval));
    

    BCM_IF_ERROR_RETURN(READ_OP_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, EARLY_E2E_SELECTf,
                      enable ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_dump_config(int unit, bcm_port_t port,
                       _bcm_kt_cosq_node_t *node)
{
    int num_spri, first_child;
    uint32 spmap;
    soc_kt_sched_mode_e sched_mode = SOC_KT_SCHED_MODE_UNKNOWN;
    int wt = 0;
    char *lvl_name[] = { "Root", "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);


    /* get sched config */
    if (node->level != _BCM_KT_COSQ_NODE_LEVEL_L2) {
        SOC_IF_ERROR_RETURN(
            soc_kt_sched_get_node_config(unit, port, node->level,
                                         node->hw_index, &num_spri,
                                         &first_child, &spmap));
    }
    sched_mode = 0;
    if (node->level != _BCM_KT_COSQ_NODE_LEVEL_ROOT) {
        SOC_IF_ERROR_RETURN(
         soc_kt_cosq_get_sched_mode(unit, port, node->level, node->hw_index,
                                               &sched_mode, &wt));
    }
    switch (node->level) {
        case _BCM_KT_COSQ_NODE_LEVEL_ROOT:
        case _BCM_KT_COSQ_NODE_LEVEL_L0:
        case _BCM_KT_COSQ_NODE_LEVEL_L1:
            if (sp_vec) {
                LOG_CLI((BSL_META_U(unit,
                "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d SPMAP=0x%08x MODE=%s Wt=%d\n"),
                      lvl_name[node->level], node->cosq_attached_to,
                      node->hw_index, num_spri, first_child,
                      spmap, sched_modes[sched_mode], wt));
            } else {
                LOG_CLI((BSL_META_U(unit,
                "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                      lvl_name[node->level], node->cosq_attached_to,
                      node->hw_index, num_spri, first_child,
                      sched_modes[sched_mode], wt));
            }
        break;
        case _BCM_KT_COSQ_NODE_LEVEL_L2:
                LOG_CLI((BSL_META_U(unit,
                              "     L2.%d INDEX=%d Mode=%s WEIGHT=%d\n"),
                                   node->cosq_attached_to, node->hw_index,
                                   sched_modes[sched_mode], wt));
        break;
        default:
             return BCM_E_INTERNAL;

    }
    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_child_node_at_input(_bcm_kt_cosq_node_t *node, int cosq,
                                  _bcm_kt_cosq_node_t **child_node)
{
    _bcm_kt_cosq_node_t *cur_node;

    for (cur_node = node->child; cur_node; cur_node = cur_node->sibling) {
        if (cur_node->cosq_attached_to == cosq) {
            break;
        }
    }
    if (!cur_node) {
        return BCM_E_NOT_FOUND;
    }

    *child_node = cur_node;

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_dump_sched(int unit, bcm_port_t port, bcm_gport_t gport)
{
    _bcm_kt_cosq_node_t *node;
    _bcm_kt_cosq_node_t *cur_node;
    int i = 0;
    int start_cos = 0;
    int end_cos = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    if (node != NULL) {
        if (node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) {
            return BCM_E_INTERNAL;
        }

        start_cos = 0;
        end_cos = node->numq;
        _bcm_kt_dump_config(unit, port, node);

        for (i = start_cos; i < end_cos; i++)
        {
            if(_bcm_kt_cosq_child_node_at_input(node, i, &cur_node)
                       == BCM_E_NOT_FOUND) {
                continue;
            }
            if (cur_node->level == _BCM_KT_COSQ_NODE_LEVEL_L2) {
                 BCM_IF_ERROR_RETURN
                   (_bcm_kt_dump_config(unit, port, cur_node));
            } else {
                BCM_IF_ERROR_RETURN
                   (_bcm_kt_dump_sched(unit, port, cur_node->gport));
            }
        }
    }
    return BCM_E_NONE;

}

STATIC int
_bcm_kt_dump_port_lls(int unit, bcm_port_t port)
{
    _bcm_kt_cosq_node_t *port_info;
    _bcm_kt_mmu_info_t *mmu_info;
    bcm_port_t port_out;

    if (_bcm_kt_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt_mmu_info[unit];

    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_localport_resolve(unit, port, &port_out));

    /* root node */
    port_info = &mmu_info->sched_node[port_out];

    if (port_info->gport >= 0) {
       _bcm_kt_dump_sched(unit, port, port_info->gport);
    }
    return BCM_E_NONE;
}

int bcm_kt_dump_port_lls(int unit, bcm_port_t port)
{

    LOG_CLI((BSL_META_U(unit,
      "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));
    _bcm_kt_dump_port_lls(unit, port);
    return SOC_E_NONE;
}

int
bcm_kt_cosq_subscriber_qid_set(int unit,
      bcm_gport_t *gport,
       int queue_id)
{
   _bcm_kt_mmu_info_t *mmu_info;
   mmu_info = _bcm_kt_mmu_info[unit];
   if (mmu_info == NULL) {
       return BCM_E_INIT;
   }
   if (queue_id >= mmu_info->num_base_queues) {
       queue_id -= mmu_info->num_base_queues;
   }
   BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET
                            (*gport,queue_id);
   return SOC_E_NONE;
}


#ifndef BCM_SW_STATE_DUMP_DISABLE

STATIC int
_bcm_kt_cosq_port_info_dump(int unit, bcm_port_t port) 
{
    _bcm_kt_mmu_info_t *mmu_info;
    _bcm_kt_cosq_node_t *port_node = NULL;
    _bcm_kt_cosq_node_t *queue_node = NULL;
    _bcm_kt_cosq_node_t *l0_node = NULL;
    _bcm_kt_cosq_node_t *l1_node = NULL;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_kt_mmu_info[unit];

    /* get the root node */
    port_node = &mmu_info->sched_node[port];

    if (port_node && port_node->cosq_attached_to < 0) {
        return BCM_E_NONE;
    }    

    LOG_CLI((BSL_META_U(unit,
                        "=== port %d\n"), port));
    LOG_CLI((BSL_META_U(unit,
                        "base = %d numq = %d hw_index = %d level = %d cosq_attach = %d \n"),
             port_node->base_index, port_node->numq, port_node->hw_index,
             port_node->level, port_node->cosq_attached_to));
    LOG_CLI((BSL_META_U(unit,
                        "num_child = %d first_child = %d \n"), port_node->num_child,
             port_node->first_child));
    if (port_node->parent != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  parent gport = 0x%08x\n"), port_node->parent->gport));
    }
    if (port_node->sibling != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  sibling gport = 0x%08x\n"), port_node->sibling->gport));
    }
    if (port_node->child != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  child  gport = 0x%08x\n"), port_node->child->gport));
    }

    for (l0_node = port_node->child; l0_node != NULL;
         l0_node = l0_node->sibling) {

         if (l0_node->cosq_attached_to < 0) {
            return BCM_E_NONE;
         }    
         LOG_CLI((BSL_META_U(unit,
                             "=== L0 gport 0x%08x\n"), l0_node->gport));
         LOG_CLI((BSL_META_U(unit,
                             "base = %d numq = %d hw_index = %d level = %d cosq_attach = %d \n"),
                  l0_node->base_index, l0_node->numq, l0_node->hw_index,
                  l0_node->level, l0_node->cosq_attached_to));
         LOG_CLI((BSL_META_U(unit,
                             "num_child = %d first_child = %d \n"), l0_node->num_child,
                  l0_node->first_child));
         if (l0_node->parent != NULL) {
             LOG_CLI((BSL_META_U(unit,
                                 "  parent gport = 0x%08x\n"), l0_node->parent->gport));
         }
         if (l0_node->sibling != NULL) {
             LOG_CLI((BSL_META_U(unit,
                                 "  sibling gport = 0x%08x\n"), l0_node->sibling->gport));
         }
         if (l0_node->child != NULL) {
             LOG_CLI((BSL_META_U(unit,
                                 "  child    gport = 0x%08x\n"), l0_node->child->gport));
         }

         for (l1_node = l0_node->child; l1_node != NULL;
              l1_node = l1_node->sibling) {
              if (l1_node->cosq_attached_to < 0) {
                  return BCM_E_NONE;
              }
              LOG_CLI((BSL_META_U(unit,
                                  "=== L1 gport 0x%08x\n"), l1_node->gport));
              LOG_CLI((BSL_META_U(unit,
                                  "base = %d numq = %d hw_index = %d level = %d cosq_attach = %d \n"),
                       l1_node->base_index, l1_node->numq, l1_node->hw_index,
                       l1_node->level, l1_node->cosq_attached_to));
              LOG_CLI((BSL_META_U(unit,
                                  "num_child = %d first_child = %d \n"), l1_node->num_child,
                       l1_node->first_child));
              if (l1_node->parent != NULL) {
                  LOG_CLI((BSL_META_U(unit,
                                      "  parent gport = 0x%08x\n"), l1_node->parent->gport));
              }
              if (l1_node->sibling != NULL) {
                  LOG_CLI((BSL_META_U(unit,
                                      "  sibling gport = 0x%08x\n"), l1_node->sibling->gport));
              }
              if (l1_node->child != NULL) {
                  LOG_CLI((BSL_META_U(unit,
                                      "  child  gport = 0x%08x\n"), l1_node->child->gport));
              }

              for (queue_node = l1_node->child; queue_node != NULL;
                   queue_node = queue_node->sibling) {
                   if (queue_node->cosq_attached_to < 0) {
                       return BCM_E_NONE;
                   }
                   LOG_CLI((BSL_META_U(unit,
                                       "=== L2 gport 0x%08x\n"), queue_node->gport));
                   LOG_CLI((BSL_META_U(unit,
                                       "base = %d numq = %d hw_index = %d level = %d cosq_attach = %d \n"),
                            queue_node->base_index, queue_node->numq, queue_node->hw_index,
                            queue_node->level, queue_node->cosq_attached_to));
                   LOG_CLI((BSL_META_U(unit,
                                       "num_child = %d first_child = %d \n"), queue_node->num_child,
                            queue_node->first_child));
                   if (queue_node->parent != NULL) {
                       LOG_CLI((BSL_META_U(unit,
                                           "  parent gport = 0x%08x\n"), queue_node->parent->gport));
                   }
                   if (queue_node->sibling != NULL) {
                       LOG_CLI((BSL_META_U(unit,
                                           "  sibling gport = 0x%08x\n"), queue_node->sibling->gport));
                   }
                   if (queue_node->child != NULL) {
                       LOG_CLI((BSL_META_U(unit,
                                           "  child  gport = 0x%08x\n"), queue_node->child->gport));
                   }
              }        

         }        
    }        

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_kt_cosq_sw_dump(int unit)
{
    _bcm_kt_mmu_info_t *mmu_info;
    bcm_port_t port;

    mmu_info = _bcm_kt_mmu_info[unit];

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));

    PBMP_ALL_ITER(unit, port) {
        (void)_bcm_kt_cosq_port_info_dump(unit, port);
    }

    LOG_CLI((BSL_META_U(unit,
                        "ext_qlist = %d \n"), mmu_info->ext_qlist.count));
    LOG_CLI((BSL_META_U(unit,
                        "sched_list = %d \n"), mmu_info->sched_list.count));
    LOG_CLI((BSL_META_U(unit,
                        "l0_sched_list = %d \n"), mmu_info->l0_sched_list.count));
    LOG_CLI((BSL_META_U(unit,
                        "l1_sched_list = %d \n"), mmu_info->l1_sched_list.count));
    LOG_CLI((BSL_META_U(unit,
                        "l2_base_qlist = %d \n"), mmu_info->l2_base_qlist.count));
    LOG_CLI((BSL_META_U(unit,
                        "l2_ext_qlist = %d \n"), mmu_info->l2_ext_qlist.count));
    
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_KATANA_SUPPORT */
typedef int _kt_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_KATANA_SUPPORT */
