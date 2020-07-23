
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
#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/l3x.h>
#include <soc/macutil.h>

#include <soc/esw/portctrl.h>
#include <soc/trident2.h>
#include <soc/td2_td2p.h>

#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trx.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/bst.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TD2_CELL_FIELD_MAX       0x1ffff

/* MMU cell size in bytes */
#define _BCM_TD2_BYTES_PER_CELL            208   /* bytes */
#define _BCM_TD2_BYTES_TO_CELLS(_byte_)  \
    (((_byte_) + _BCM_TD2_BYTES_PER_CELL - 1) / _BCM_TD2_BYTES_PER_CELL)

#define _BCM_TD2_NUM_PORT_SCHEDULERS              106
#define _BCM_TD2_NUM_L0_SCHEDULER_PER_PIPE        265
#define _BCM_TD2_NUM_L0_SCHEDULER                 _BCM_TD2_NUM_L0_SCHEDULER_PER_PIPE * 2
#define _BCM_TD2_NUM_L1_SCHEDULER_PER_PIPE        (1024 - 4) /* Reserved for default val */
#define _BCM_TD2_NUM_L1_SCHEDULER                 _BCM_TD2_NUM_L1_SCHEDULER_PER_PIPE * 2
#define _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE        (1480 - 4) /* Reserved for default val */
#define _BCM_TD2_NUM_L2_UC_LEAVES                 _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE * 2
#define _BCM_TD2_NUM_L2_MC_LEAVES_PER_PIPE        568
#define _BCM_TD2_NUM_L2_MC_LEAVES                 _BCM_TD2_NUM_L2_MC_LEAVES_PER_PIPE * 2
#define _BCM_TD2_NUM_L2_LEAVES_PER_PIPE     (_BCM_TD2_NUM_L2_MC_LEAVES_PER_PIPE + \
                                             _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE)
#define _BCM_TD2_NUM_L2_LEAVES              _BCM_TD2_NUM_L2_LEAVES_PER_PIPE * 2
#define _BCM_TD2_NUM_TOTAL_SCHEDULERS     (_BCM_TD2_NUM_PORT_SCHEDULERS + \
                                           _BCM_TD2_NUM_L0_SCHEDULER +    \
                                           _BCM_TD2_NUM_L1_SCHEDULER)
#define _TD2_SPM_ENTRIES_PER_SET            128
#define _TD2_SCM_ENTRIES_PER_SET            16
/* ENDPOINT_COS_MAPm */
#define _TD2_EPCM_ENTRIES_PER_SET           16
/* All CPU leaves are MC and belong to X Pipe only. Ranges from 520-567.*/
#define _BCM_TD2_NUM_L2_CPU_LEAVES                48 

#define _TD2_NUM_COS    NUM_COS
#define _TD2_NUM_INTERNAL_PRI          16

/* TD2 supports eight PFC priority */
#define _BCM_TD2_NUM_PFC_CLASS          8

#define _BCM_TD2_COSQ_LIST_NODE_INIT(list, node)         \
    list[node].in_use            = FALSE;               \
    list[node].gport             = -1;                  \
    list[node].base_index        = -1;                  \
    list[node].numq              = 0;                   \
    list[node].base_size         = 0;                   \
    list[node].numq_expandable   = 0;                   \
    list[node].hw_index          = -1;                  \
    list[node].level             = -1;                  \
    list[node].attached_to_input       = -1;                  \
    list[node].hw_cosq           = 0;                   \
    list[node].node_align_value  = 1;                   \
    list[node].remote_modid      = -1;                  \
    list[node].remote_port       = -1;                  \
    BCM_PBMP_CLEAR(list[node].fabric_pbmp);              \
    list[node].local_port        = -1;                  \
    list[node].parent            = NULL;                \
    list[node].sibling           = NULL;                \
    list[node].child             = NULL;

#define _BCM_TD2_COSQ_NODE_INIT(node)                    \
    node->in_use            = FALSE;                    \
    node->gport             = -1;                       \
    node->base_index        = -1;                       \
    node->numq              = 0;                        \
    node->base_size         = 0;                        \
    node->numq_expandable   = 0;                        \
    node->hw_index          = -1;                       \
    node->level             = -1;                       \
    node->attached_to_input = -1;                       \
    node->hw_cosq           = 0;                        \
    node->node_align_value  = 1;                        \
    node->remote_modid      = -1;                       \
    node->remote_port       = -1;                       \
    BCM_PBMP_CLEAR(node->fabric_pbmp);              \
    node->local_port        = -1;                       \
    node->parent            = NULL;                     \
    node->sibling           = NULL;                     \
    node->child             = NULL;

typedef enum {
    _BCM_TD2_COSQ_FC_PAUSE = 0,
    _BCM_TD2_COSQ_FC_PFC,
    _BCM_TD2_COSQ_FC_VOQFC,
    _BCM_TD2_COSQ_FC_E2ECC
} _bcm_td2_fc_type_t;

typedef enum {
    _BCM_TD2_NODE_UNKNOWN = 0,
    _BCM_TD2_NODE_UCAST,
    _BCM_TD2_NODE_MCAST,
    _BCM_TD2_NODE_VOQ,
    _BCM_TD2_NODE_VLAN_UCAST,
    _BCM_TD2_NODE_VM_UCAST,
    _BCM_TD2_NODE_SERVICE_UCAST,
    _BCM_TD2_NODE_SCHEDULER
} _bcm_td2_node_type_e;

typedef struct _bcm_td2_cosq_node_s {
    struct _bcm_td2_cosq_node_s *parent;
    struct _bcm_td2_cosq_node_s *sibling;
    struct _bcm_td2_cosq_node_s *child;
    bcm_gport_t gport;
    int in_use;
    int base_index;
    uint16 numq_expandable;
    uint16 base_size;
    int numq;
    int hw_index;
    soc_td2_node_lvl_e level;
    _bcm_td2_node_type_e    type;
    int attached_to_input;
    int hw_cosq;
    int node_align_value;

    /* DPVOQ specific information */
    bcm_port_t   local_port;
    bcm_module_t remote_modid;
    bcm_port_t   remote_port;
    bcm_pbmp_t   fabric_pbmp;
}_bcm_td2_cosq_node_t;

typedef struct _bcm_td2_cosq_list_s {
    int count;
    SHR_BITDCL *bits;
}_bcm_td2_cosq_list_t;

#define _BCM_TD2_HSP_PORT_MAX_L0    5
#define _BCM_TD2_HSP_PORT_MAX_L1    10
#define _BCM_TD2_HSP_PORT_MAX_L2    2
#define _BCM_TD2_HSP_PORT_MAX_COS   8

#define _BCM_TD2_MAX_PORT   106

#define _BCM_TD2_ROOT_SCHED_INDEX(u,mmu_port)       \
                ((mmu_port >= 64) ? (mmu_port - 64) : (mmu_port))

#define _BCM_TD2_HSP_L0_INDEX(u,mmu_port,cosq)       \
 ((((mmu_port >= 64) ? (mmu_port - 64) : (mmu_port)) * _BCM_TD2_HSP_PORT_MAX_L0) +\
  (cosq))

#define _BCM_TD2_HSP_L1_INDEX(u,mmu_port,cosq)       \
 ((((mmu_port >= 64) ? (mmu_port - 64) : (mmu_port)) * _BCM_TD2_HSP_PORT_MAX_L1) +\
  (cosq))


typedef struct _bcm_td2_pipe_resources_s {
    int num_base_queues;   /* Number of classical queues */

    _bcm_td2_cosq_list_t ext_qlist;      /* list of extended queues */
    _bcm_td2_cosq_list_t l0_sched_list;  /* list of l0 sched nodes */
    _bcm_td2_cosq_list_t l1_sched_list;  /* list of l1 sched nodes */

    _bcm_td2_cosq_node_t *p_queue_node;
    _bcm_td2_cosq_node_t *p_mc_queue_node;
} _bcm_td2_pipe_resources_t;

typedef struct _bcm_td2_cosq_port_info_s {
    int mc_base;
    int mc_limit;
    int uc_base;
    int uc_limit;
    int eq_base;
    int eq_limit;
    _bcm_td2_pipe_resources_t *resources;
} _bcm_td2_cosq_port_info_t;

typedef struct _bcm_td2_cosq_cpu_cosq_config_s {
    /* All MC queues have DB and MCQE space */
    int enable;
    int q_min_limit[2];
    int q_shared_limit[2];
    uint8 q_limit_dynamic[2];
    uint8 q_limit_enable[2];
} _bcm_td2_cosq_cpu_cosq_config_t;

#define _TD2_XPIPE  0
#define _TD2_YPIPE  1
#define _TD2_NUM_PIPES  2

#define _BCM_TD2_PORT_TO_PIPE(u, p) \
   (SOC_PBMP_MEMBER(SOC_INFO(unit).xpipe_pbm, (p)) ? _TD2_XPIPE : _TD2_YPIPE)

typedef struct _bcm_td2_mmu_info_s {

    _bcm_td2_cosq_node_t sched_node[_BCM_TD2_NUM_TOTAL_SCHEDULERS]; /* sched nodes */
    _bcm_td2_cosq_node_t queue_node[_BCM_TD2_NUM_L2_UC_LEAVES];    /* queue nodes */
    _bcm_td2_cosq_node_t mc_queue_node[_BCM_TD2_NUM_L2_MC_LEAVES]; /* queue nodes */
    _bcm_td2_cosq_port_info_t port_info[_BCM_TD2_MAX_PORT];
    
    _bcm_td2_pipe_resources_t   pipe_resources[_TD2_NUM_PIPES];
    
    int     ets_mode;
    int     curr_shared_limit;
    int     curr_merger_index;
}_bcm_td2_mmu_info_t;

/* Node hardware index alignment. */
#define _BCM_TD2_START_INDEX_ALIGN_DEFAULT   1
#define _BCM_TD2_START_INDEX_ALIGN_PFC       4

#define _TD2_NUM_TIME_DOMAIN    4

typedef struct _bcm_td2_cosq_time_info_s {
    soc_field_t field;
    uint32 ref_count;   
} _bcm_td2_cosq_time_info_t;

/* WRED can be enabled at per UC queue / queue group/ port / service pool basis
MMU_WRED_AVG_QSIZE_X/Y_PIPE used entry:2 * (1480 + 128 + 208 + 4) = 3640
1479 ~ 0: Unicast Queue
1487 ~ 1480: Not used.
1615 ~ 1488: Queue Group per PIPE
1823 ~ 1616: Port Service Pool (Port 0 to 52)
1839 ~ 1824: Not used.
1843 ~ 1840: Global Service Pool
*/
STATIC _bcm_td2_cosq_time_info_t time_domain[_TD2_NUM_TIME_DOMAIN] = {
    {TIME_0f, 3640}, /* Default all entries are pointing to TIME_0 */
    {TIME_1f, 0},
    {TIME_2f, 0},
    {TIME_3f, 0}
};

#define _TD2_NUM_SERVICE_POOL    4

#define _BCM_TD2_PRESOURCES(mi, p)  (&(mi)->pipe_resources[(p)])

STATIC _bcm_td2_mmu_info_t *_bcm_td2_mmu_info[BCM_MAX_NUM_UNITS];  /* MMU info */

STATIC soc_profile_mem_t *_bcm_td2_wred_profile[_TD2_NUM_PIPES][BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td2_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td2_sample_int_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_td2_feedback_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_td2_llfc_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td2_voq_port_map_profile[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_td2_ifp_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td2_service_port_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td2_service_cos_map_profile[BCM_MAX_NUM_UNITS];

STATIC _bcm_td2_cosq_cpu_cosq_config_t *_bcm_td2_cosq_cpu_cosq_config[BCM_MAX_NUM_UNITS][_BCM_TD2_NUM_L2_CPU_LEAVES];

STATIC int bcm_td2p_cosq_rx_chan_l0_map[BCM_RX_CHANNELS] =
                                         {-1, 259, 260, 261};
STATIC int bcm_td2p_cmic_to_mmu_cosmask_map[_TD2PLUS_COSQ_NUM_CPU_L0_NODES] =
                                         {58, 59, 60, 61, 62, 63};

STATIC int
_bcm_td2_node_index_set(_bcm_td2_cosq_list_t *list, int start, int range);

STATIC int _bcm_td2_voq_next_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_td2_cosq_node_t **base_node);
STATIC int
_bcm_td2_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq);
STATIC int
_bcm_td2_cosq_set_scheduler_states(int unit, bcm_port_t port, int busy);

typedef enum {
    _BCM_TD2_COSQ_TYPE_UCAST,
    _BCM_TD2_COSQ_TYPE_MCAST,
    _BCM_TD2_COSQ_TYPE_EXT_UCAST
} _bcm_td2_cosq_type_t;

#define IS_TD2_HSP_PORT(u,p)   \
    ((_soc_trident2_port_sched_type_get((u),(p)) == SOC_TD2_SCHED_HSP) ? 1 : 0)

typedef struct _bcm_td2_endpoint_s {
    uint32 flags;       /* BCM_COSQ_CLASSIFIER_xxx flags */
    bcm_vlan_t vlan;    /* VLAN */
    bcm_mac_t mac;      /* MAC address */
    bcm_vrf_t vrf;      /* Virtual Router Instance */
    bcm_ip_t ip_addr;   /* IPv4 address */
    bcm_ip6_t ip6_addr; /* IPv6 address */
    bcm_gport_t gport;  /* GPORT ID */
    bcm_if_t egress_if; /* Egress object */
} _bcm_td2_endpoint_t;

typedef struct _bcm_td2_endpoint_queuing_info_s {
    int num_endpoint; /* Number of endpoints */
    _bcm_td2_endpoint_t **ptr_array; /* Array of pointers to endpoints */
    soc_profile_mem_t *cos_map_profile; /* Internal priority to CoS queue
                                           mapping profile */
} _bcm_td2_endpoint_queuing_info_t;

STATIC int 
_bcm_td2_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        int *p_pool_start, int *p_pool_end);

STATIC _bcm_td2_endpoint_queuing_info_t *_bcm_td2_endpoint_queuing_info[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_td2_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg);

STATIC int 
_bcm_td2_cosq_node_get(int unit, bcm_gport_t gport,
    bcm_cos_queue_t cosq,bcm_module_t *modid, bcm_port_t *port,
    int *id, _bcm_td2_cosq_node_t **node);

STATIC int
_bcm_td2_cosq_sched_parent_child_set(int unit, int port, int level,
                                     int sched_index, int child_index,
                                     soc_td2_sched_mode_e sched_mode, 
                                     int weight, _bcm_td2_cosq_node_t *node);

#define _BCM_TD2_NUM_ENDPOINT(unit) \
    (_bcm_td2_endpoint_queuing_info[unit]->num_endpoint)
    
#define _BCM_TD2_ENDPOINT(unit, id) \
    (_bcm_td2_endpoint_queuing_info[unit]->ptr_array[id])

#define _BCM_TD2_ENDPOINT_IS_USED(unit, id) \
    (_bcm_td2_endpoint_queuing_info[unit]->ptr_array[id] != NULL)

#define _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit) \
    (_bcm_td2_endpoint_queuing_info[unit]->cos_map_profile)

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)

#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_7

STATIC int
_bcm_td2_cosq_wb_alloc(int unit)
{
    _bcm_td2_mmu_info_t *mmu_info;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size, node_list_sizes[3], ii, jj;
    uint32 shadow_size = 0;
    
    mmu_info = _bcm_td2_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) { 
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/  
        return BCM_E_NONE;
    }

    node_list_sizes[0] = _BCM_TD2_NUM_L2_UC_LEAVES;
    node_list_sizes[1] = _BCM_TD2_NUM_L2_MC_LEAVES;
    node_list_sizes[2] = _BCM_TD2_NUM_TOTAL_SCHEDULERS;

    alloc_size = 0;
    for(ii = 0; ii < 3; ii++) {
        alloc_size += sizeof(uint32);
        for(jj = 0; jj < node_list_sizes[ii]; jj++) {
            alloc_size += 4 * sizeof(uint32);
        }
    }

    /* Size of  TIME_DOMAINr field and count reference */
    alloc_size += _TD2_NUM_TIME_DOMAIN * sizeof(_bcm_td2_cosq_time_info_t);

    soc_trident2_fc_map_shadow_size_get(unit, &shadow_size);
    alloc_size += shadow_size;

    /* added in BCM_WB_VERSION_1_1
     * to sync _bcm_td2_mmu_info[unit]->hw_cosq/attached_to_input/
     * type/numq_expandable/numq/base_index/base_size
     */
    for (ii = 0; ii < 3; ii++) {
        for (jj = 0; jj < node_list_sizes[ii]; jj++) {
            alloc_size += 2 * sizeof(uint32);
        }
    }

    /* added in BCM_WB_VERSION_1_1
     * to sync _bcm_td2_mmu_info[unit]->ets_mode/curr_shared_limit
     */
    alloc_size += 2 * sizeof(int);

    /* added in BCM_WB_VERSION_1_2
     * to sync the reference count of IFP_COS_MAPm 
     */
    alloc_size += (SOC_MEM_SIZE(unit,IFP_COS_MAPm) / _TD2_NUM_INTERNAL_PRI) *
                  sizeof(uint16);

    alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) /
                  _TD2_SCM_ENTRIES_PER_SET;
    alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) /
                  _TD2_SPM_ENTRIES_PER_SET;

    BCM_IF_ERROR_RETURN(soc_td2_mmu_traffic_ctrl_size(unit, &alloc_size));

    /*
     * Added in BCM_WB_VERSION_1_5
     * Sync num_cos value
     */
    alloc_size += sizeof(int);

    /*
     * Added in BCM_WB_VERSION_1_6
     * to sync _bcm_td2_mmu_info[unit]->curr_merger_index/node->fabric_pbmp
     */
    alloc_size += sizeof(int);  /* to sync sizeof(bcm_pbmp_t) */
    for(ii = 0; ii < 3; ii++) {
        alloc_size += (node_list_sizes[ii] * sizeof(bcm_pbmp_t));
    }
    alloc_size += sizeof(int);

    /*
     * Added in BCM_WB_VERSION_1_7
     * to sync _bcm_td2_endpoint_queuing_info[unit]->ptr_array[id]
     */
     if (soc_feature(unit, soc_feature_endpoint_queuing)) {
         alloc_size += ((_BCM_TD2_NUM_ENDPOINT(unit) - 1) *
                        (sizeof(uint32) + sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t)));
         alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) /
                  _TD2_EPCM_ENTRIES_PER_SET;
     }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

#define _BCM_TD2_WB_NTYPE_UCAST 0
#define _BCM_TD2_WB_NTYPE_MCAST 1
#define _BCM_TD2_WB_NTYPE_SCHED 2

#define _BCM_TD2_WB_SET_NODE_W1(ntype,nid,sz,parentid,pipe)    \
    (((ntype & 3)) | \
     (((sz) & 7) << 2) | \
     (((nid) & 0x1fff) << 5) | \
     (((parentid) & 0xfff) << 18) | ((pipe) & 1) << 31)

#define _BCM_TD2_WB_GET_NODE_ID_W1(w1) (((w1) >> 5) & 0x1fff)

#define _BCM_TD2_WB_GET_NODE_TYPE_W1(w1) ((w1) & 3)

#define _BCM_TD2_WB_GET_NODE_SIZE_W1(w1) (((w1) >> 2) & 7)

#define _BCM_TD2_WB_GET_NODE_PARENT_W1(w1) (((w1) >> 18) & 0xfff)

#define _BCM_TD2_WB_GET_PIPE_W1(w1) (((w1) >> 31) & 1)

/* 12 bit hw index, 11 bit sibling, 3 bit hw cosq */
/* Encoding node_align_value in 1 bit 0->1, 1->4 */
#define _BCM_TD2_WB_SET_NODE_W2(hwindex,hwcosq,cosq,lvl,align)    \
    ((((hwindex) & 0x1fff)) | (((hwcosq) & 0xf) << 13) | \
        (((cosq) & 0xf) << 17) | (((lvl) & 0x7) << 21) | \
        (((align) & 0x1) << 24))

#define _BCM_TD2_WB_GET_NODE_HW_INDEX_W2(w1) ((w1) & 0x1fff)

#define _BCM_TD2_WB_GET_NODE_HW_COSQ_W2(w1) (((w1) >> 13) & 0xf)

#define _BCM_TD2_WB_GET_NODE_COSQATTT_W2(w1) (((w1) >> 17) & 0xf)

#define _BCM_TD2_WB_GET_NODE_LEVEL_W2(w1) (((w1) >> 21) & 0x7)

#define _BCM_TD2_WB_GET_ALIGN_W2(w1) (((w1) >> 24) & 0x1)

#define _BCM_TD2_WB_SET_NODE_W3(gport)    (gport)

#define _BCM_TD2_WB_SET_NODE_W4(remote_modid,remote_port,voq_cosq)    \
    ((((remote_modid) & 0x1ff)) | \
     (((remote_port) & 0x1ff) << 9) | \
     (((voq_cosq) & 0xf) << 18))

#define _BCM_TD2_WB_GET_NODE_DEST_MODID_W4(w1) ((w1) & 0x1ff)

#define _BCM_TD2_WB_GET_NODE_DEST_PORT_W4(w1) (((w1) >> 9) & 0x1ff)

#define _BCM_TD2_WB_GET_NODE_VOQ_COSQ_W4(w1) (((w1) >> 18) & 0xf)

#define _BCM_TD2_WB_SET_NODE_W5(expand,type,cosq,hw_cosq)    \
    (((expand) & 0x7) | (((type) & 0x1f) << 3) | \
    (((cosq) & 0xfff) << 8)  | (((hw_cosq) & 0xfff) << 20))

#define _BCM_TD2_WB_GET_EXPAND_W5(w1) ((w1) & 0x7)

#define _BCM_TD2_WB_TYPE_W5(w1) (((w1) >> 3) & 0x1f)

#define _BCM_TD2_WB_COSQ_W5(w1) (((w1) >> 8) & 0xfff)

#define _BCM_TD2_WB_HW_COSQ_W5(w1) (((w1) >> 20) & 0xfff)

#define _BCM_TD2_WB_SET_NODE_W6(base_size,base_index,numq)    \
    (((base_size) & 0x3ff) | (((base_index) & 0x7ff) << 10) | \
    (((numq) & 0x7ff) << 21))

#define _BCM_TD2_WB_GET_BASE_SIZE_W6(w1) ((w1) & 0x3ff)

#define _BCM_TD2_WB_GET_BASE_INDEX_W6(w1) (((w1) >> 10) & 0x7ff)

#define _BCM_TD2_WB_GET_NUMQ_W6(w1) (((w1) >> 21) & 0x7ff)


#define _BCM_TD2_NODE_ID(np,np0) (np - np0)

/*
 * Function:
 *      bcm_td2_cosq_endpoint_recover
 * Purpose:
 *      Recover endpoint info from scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcm_td2_cosq_endpoint_recover(int unit, uint8 **scache_ptr)
{
    int entry_size;
    int flags;
    int ii;
    int profile_num;
    soc_profile_mem_t *profile_mem;
    uint16 *u16_scache_ptr;
    int    ref_count;

    entry_size = sizeof(uint32) + sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t);
    /* entry 0 was reserved and is not synced. */
    for (ii = 1; ii < _BCM_TD2_NUM_ENDPOINT(unit); ii++) {
        sal_memcpy(&flags, (*scache_ptr), sizeof(uint32));

        if ((flags & BCM_COSQ_CLASSIFIER_L2) ||
            (flags & BCM_COSQ_CLASSIFIER_L3) ||
            (flags & BCM_COSQ_CLASSIFIER_GPORT) || 
            (flags & BCM_COSQ_CLASSIFIER_L3_EGRESS)) {
            _BCM_TD2_ENDPOINT(unit, ii) = sal_alloc(
                sizeof(_bcm_td2_endpoint_t), "Endpoint Info");
            if (_BCM_TD2_ENDPOINT(unit, ii) == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_BCM_TD2_ENDPOINT(unit, ii), 0,
                       sizeof(_bcm_td2_endpoint_t));
            _BCM_TD2_ENDPOINT(unit, ii)->flags = flags;
            if (flags & BCM_COSQ_CLASSIFIER_L2) {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_TD2_ENDPOINT(unit, ii)->vlan),
                           (*scache_ptr), sizeof(bcm_vlan_t));
                (*scache_ptr) += sizeof(bcm_vlan_t);
                sal_memcpy(_BCM_TD2_ENDPOINT(unit, ii)->mac, (*scache_ptr),
                           sizeof(bcm_mac_t));
                (*scache_ptr) += (entry_size - sizeof(uint32) -
                                  sizeof(bcm_vlan_t));
            } else if (flags & BCM_COSQ_CLASSIFIER_L3) {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_TD2_ENDPOINT(unit, ii)->vrf),
                           (*scache_ptr), sizeof(bcm_vrf_t));
                (*scache_ptr) += sizeof(bcm_vrf_t);
                if (flags & BCM_COSQ_CLASSIFIER_IP6) {
                    sal_memcpy(_BCM_TD2_ENDPOINT(unit, ii)->ip6_addr, (*scache_ptr),
                               sizeof(bcm_ip6_t));
                } else {
                    sal_memcpy(&(_BCM_TD2_ENDPOINT(unit, ii)->ip_addr), (*scache_ptr),
                               sizeof(bcm_ip_t));
                }
                (*scache_ptr) += (entry_size - sizeof(uint32) -
                                  sizeof(bcm_vrf_t));
            } else if (flags & BCM_COSQ_CLASSIFIER_L3_EGRESS) {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_TD2_ENDPOINT(unit, ii)->egress_if),
                           (*scache_ptr), sizeof(bcm_if_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            } else {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_TD2_ENDPOINT(unit, ii)->gport),
                           (*scache_ptr), sizeof(bcm_gport_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            }
        } else {
            (*scache_ptr) += entry_size;
        }
    }

    profile_mem = _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit);
    profile_num = SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) / _TD2_EPCM_ENTRIES_PER_SET;
    u16_scache_ptr = (uint16 *)(*scache_ptr);

    for (ii = 0; ii < profile_num; ii++) {
        for (ref_count = 0; ref_count < *u16_scache_ptr; ref_count++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, profile_mem,
                                          ii * _TD2_EPCM_ENTRIES_PER_SET,
                                          _TD2_EPCM_ENTRIES_PER_SET));
        }
        u16_scache_ptr++;
    }

    *scache_ptr = (uint8 *)(u16_scache_ptr);

    return BCM_E_NONE;
}

int
bcm_td2_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    uint32 *u32_scache_ptr, *u32_base_ptr, *u32_base_ptr_1, wval, fval;
    uint16 *u16_scache_ptr = NULL;
    _bcm_td2_cosq_node_t *node;
    bcm_port_t port;
    int rv, ii, jj, set_index;
    _bcm_td2_cosq_node_t *nodes[3];
    int xnode_id, stable_size, node_size, count, pipe, profile_index;
    _bcm_td2_mmu_info_t *mmu_info;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_profile_mem_t *profile_mem;
    _bcm_td2_pipe_resources_t *res;
    soc_mem_t mem;
    mmu_wred_config_entry_t qentry;
    int alloc_size;
    int incr_size = 0;
    uint16 recovered_ver = 0;
    int    node_list_sizes[3];
    int index=0;
    bcm_pbmp_t all_pbmp;
    soc_profile_reg_t *profile_reg;
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port;
    soc_reg_t reg;
	uint32 tmp32, scache_pbmp_size, pbmp_restore_size;
    uint64 rval, index_map;
    static const soc_reg_t llfc_cfgr[] = {
            PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r, PORT_PFC_CFG3r
    };
    int num;
    _bcm_td2_cosq_time_info_t tmp_timedomain;
    uint8 *u8_scache_ptr = NULL;

    si = &SOC_INFO(unit);

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

    mmu_info = _bcm_td2_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    u32_scache_ptr = (uint32*) scache_ptr;

    nodes[0] = &mmu_info->queue_node[0];
    nodes[1] = &mmu_info->mc_queue_node[0];
    nodes[2] = &mmu_info->sched_node[0];

    u32_base_ptr = u32_scache_ptr;
    u32_base_ptr_1 = u32_scache_ptr;
    for(ii = 0; ii < 3; ii++) {
        count = *u32_scache_ptr;
        u32_scache_ptr++;
        for(jj = 0; jj < count; jj++) {
            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            xnode_id = _BCM_TD2_WB_GET_NODE_ID_W1(wval);
            node = nodes[ii] + xnode_id;
            node->in_use = TRUE;
            node->numq = 1;
            node_size = _BCM_TD2_WB_GET_NODE_SIZE_W1(wval);
            fval =  _BCM_TD2_WB_GET_NODE_PARENT_W1(wval);
            if (fval & 0x800) {
            } else {
                node->parent = &mmu_info->sched_node[fval];
                if (node->parent->child) {
                    node->sibling = node->parent->child;
                }
                node->parent->child = node;
            }

            pipe = _BCM_TD2_WB_GET_PIPE_W1(wval);
            res = _BCM_TD2_PRESOURCES(mmu_info, pipe);

            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            
            fval = _BCM_TD2_WB_GET_NODE_HW_INDEX_W2(wval);
            if (fval & 0x1000) {
                node->hw_index = -1;
            } else {
                node->hw_index = fval;
            }

            fval = _BCM_TD2_WB_GET_NODE_HW_COSQ_W2(wval);
            if (fval & 8) {
                node->hw_cosq = -1;
            } else {
                node->hw_cosq = fval;
            }

            fval = _BCM_TD2_WB_GET_NODE_COSQATTT_W2(wval);
            if (fval & 8) {
                node->attached_to_input = -1;
            } else {
                node->attached_to_input = fval;
            }

            node->level = _BCM_TD2_WB_GET_NODE_LEVEL_W2(wval);

            node->node_align_value = (_BCM_TD2_WB_GET_ALIGN_W2(wval) ? \
                                      _BCM_TD2_START_INDEX_ALIGN_PFC : \
                                      _BCM_TD2_START_INDEX_ALIGN_DEFAULT);

            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            node->gport = wval;
            /*
             * Not checking the return value intentionally
             * to take care of warmboot case.
             */
            /* coverity[unchecked_value] */
            /* coverity[check_return] */
            _bcm_td2_cosq_node_get(unit, node->gport, 0, NULL,
                &node->local_port, NULL, NULL);

            if (node->hw_index != -1) {
                if (node->level == SOC_TD2_NODE_LVL_L0) {
                    _bcm_td2_node_index_set(&res->l0_sched_list, 
                                            node->hw_index, 1);
                } else if (node->level == SOC_TD2_NODE_LVL_L1) {
                    _bcm_td2_node_index_set(&res->l1_sched_list, 
                                            node->hw_index, 1);
                } else if (node->level == SOC_TD2_NODE_LVL_L2) {
                    _bcm_td2_node_index_set(&res->ext_qlist,
                                            node->hw_index, 1);
                }
            }

            if (node_size > 3) {
                wval = *u32_scache_ptr;
                u32_scache_ptr++;
    
                fval = _BCM_TD2_WB_GET_NODE_DEST_MODID_W4(wval);
                if (fval & 0x100) {
                    node->remote_modid = -1;
                } else {
                    node->remote_modid = fval;
                }

                fval = _BCM_TD2_WB_GET_NODE_DEST_PORT_W4(wval);
                if (fval & 0x100) {
                    node->remote_port = -1;
                } else {
                    node->remote_port = fval;
                }

                fval = _BCM_TD2_WB_GET_NODE_VOQ_COSQ_W4(wval);
                if (fval & 0xf) {
                    node->hw_cosq = -1;
                } else {
                    node->hw_cosq = fval;
                }
            }
        }
    }

    /* Recovery TIME_DOMAINr field and count reference
     * Do not overwrite time_domian[i].field data,
     * TIME_0/1/2/3f(file value) itself can be changed during version update
     * Just update time_domain[i].ref_count */
    for (ii = 0; ii <  _TD2_NUM_TIME_DOMAIN; ii++) {
        sal_memset(&tmp_timedomain, 0, sizeof(tmp_timedomain));
        alloc_size =  sizeof(_bcm_td2_cosq_time_info_t);
        sal_memcpy(&tmp_timedomain, u32_scache_ptr, alloc_size);
        time_domain[ii].ref_count = tmp_timedomain.ref_count;
        u32_scache_ptr += alloc_size / sizeof(uint32);
    }
    soc_trident2_fc_map_shadow_load(unit, &u32_scache_ptr);

    node_list_sizes[0] = _BCM_TD2_NUM_L2_UC_LEAVES;
    node_list_sizes[1] = _BCM_TD2_NUM_L2_MC_LEAVES;
    node_list_sizes[2] = _BCM_TD2_NUM_TOTAL_SCHEDULERS;
    /* 1.In the vesion before BCM_WB_VERSION_1_1, the node->attached_to_input/
     * hw_cosq/numq were synced/recovered incorrectly(The range of them were
     * incorrect).
     * 2.The node->base_inde/base_size/_bcm_td2_mmu_info[unit]->ets_mode/
     * curr_shared_limit were not synced/recovered in the version before
     * BCM_WB_VERSION_1_1.
     */
    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        for (ii = 0; ii < 3; ii++) {
            count = *u32_base_ptr++;
            for (jj = 0; jj < count; jj++) {
                wval = *u32_base_ptr++;
                xnode_id = _BCM_TD2_WB_GET_NODE_ID_W1(wval);
                node = nodes[ii] + xnode_id;
                node_size = _BCM_TD2_WB_GET_NODE_SIZE_W1(wval);
    
                wval = *u32_scache_ptr++;
                node->numq_expandable = _BCM_TD2_WB_GET_EXPAND_W5(wval);
                node->type = _BCM_TD2_WB_TYPE_W5(wval);
    
                fval = _BCM_TD2_WB_COSQ_W5(wval);
                if (fval & 0x800) {
                    node->attached_to_input = -1;
                } else {
                    node->attached_to_input = fval;
                }
    
                fval = _BCM_TD2_WB_HW_COSQ_W5(wval);
                if (fval & 0x800) {
                    node->hw_cosq = -1;
                } else {
                    node->hw_cosq = fval;
                }
    
                wval = *u32_scache_ptr++;
                node->base_size = _BCM_TD2_WB_GET_BASE_SIZE_W6(wval);
                fval = _BCM_TD2_WB_GET_NUMQ_W6(wval);
                if (fval & 0x600) {
                    node->numq = -1;
                } else {
                    node->numq = fval;
                }
                fval = _BCM_TD2_WB_GET_BASE_INDEX_W6(wval);
                if (fval & 0x400) {
                    node->base_index = -1;
                } else {
                    node->base_index = fval;
                }
    
                u32_base_ptr += 2;
                if (node_size > 3) {
                    u32_base_ptr++;
                }
            }
        }
    
        mmu_info->ets_mode = *u32_scache_ptr++;
        mmu_info->curr_shared_limit = *u32_scache_ptr++;
    } else {
        /* added in BCM_WB_VERSION_1_1
         * to sync _bcm_td2_mmu_info[unit]->hw_cosq/attached_to_input/
         * type/numq_expandable/numq/base_index/base_size
         */
        for (ii = 0; ii < 3; ii++) {
            for (jj = 0; jj < node_list_sizes[ii]; jj++) {
                incr_size += 2 * sizeof(uint32);
            }
        }

        /* added in BCM_WB_VERSION_1_1
         * to sync _bcm_td2_mmu_info[unit]->ets_mode/curr_shared_limit
         */
        incr_size += 2 * sizeof(int);
    }

    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        /* Update IFP_COS_MAP memory profile reference counter */
        profile_mem = _bcm_td2_ifp_cos_map_profile[unit];
        num = SOC_MEM_SIZE(unit,IFP_COS_MAPm) / _TD2_NUM_INTERNAL_PRI;
        u16_scache_ptr = (uint16 *)u32_scache_ptr;
        for (ii = 0; ii < num; ii++) {
            for (jj = 0; jj < *u16_scache_ptr; jj++) {
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, profile_mem,
                                              ii * _TD2_NUM_INTERNAL_PRI,
                                              _TD2_NUM_INTERNAL_PRI));
            }
            u16_scache_ptr++;
        }
    } else {
        incr_size +=
            (SOC_MEM_SIZE(unit,IFP_COS_MAPm) / _TD2_NUM_INTERNAL_PRI) * 
             sizeof(uint16);
    }

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        profile_mem = _bcm_td2_service_cos_map_profile[unit];
        num = SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) / _TD2_SCM_ENTRIES_PER_SET;
        for (ii = 0; ii < num; ii++) {
            for (jj = 0; jj < *u16_scache_ptr; jj++) {
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, profile_mem,
                                              ii * _TD2_SCM_ENTRIES_PER_SET,
                                              _TD2_SCM_ENTRIES_PER_SET));
            }
            u16_scache_ptr++;
        }

        profile_mem = _bcm_td2_service_port_map_profile[unit];
        num = SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) / _TD2_SPM_ENTRIES_PER_SET;
        for (ii = 0; ii < num; ii++) {
            for (jj = 0; jj < *u16_scache_ptr; jj++) {
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, profile_mem,
                                              ii * _TD2_SPM_ENTRIES_PER_SET,
                                              _TD2_SPM_ENTRIES_PER_SET));
            }
            u16_scache_ptr++;
        }
    } else {
        incr_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) /
                     _TD2_SCM_ENTRIES_PER_SET;
        incr_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) /
                     _TD2_SPM_ENTRIES_PER_SET;
    }

    if (recovered_ver >= BCM_WB_VERSION_1_4) {
        u8_scache_ptr = (uint8 *)u16_scache_ptr;
        BCM_IF_ERROR_RETURN(soc_td2_mmu_traffic_ctrl_wb_restore(unit, &u8_scache_ptr));
    } else {
        BCM_IF_ERROR_RETURN(soc_td2_mmu_traffic_ctrl_size(unit, &incr_size));
    }

    /* Retriving num_cos value */
    if (recovered_ver >= BCM_WB_VERSION_1_5) {
        u32_scache_ptr = (uint32 *) u8_scache_ptr;
        NUM_COS(unit) = *u32_scache_ptr++;
    } else {
        incr_size += sizeof(int);
    }

    /* Retriving node->fabric_pbmp & mmu_info->curr_merger_index value */
    if (recovered_ver >= BCM_WB_VERSION_1_6) {
        scache_pbmp_size = *u32_scache_ptr++;
        u8_scache_ptr = (uint8 *)u32_scache_ptr;
        pbmp_restore_size = (scache_pbmp_size < sizeof(bcm_pbmp_t))? \
                            scache_pbmp_size : sizeof(bcm_pbmp_t);
        for (ii = 0; ii < 3; ii++) {
            count = *u32_base_ptr_1++;
            for (jj = 0; jj < count; jj++) {
                wval = *u32_base_ptr_1++;
                xnode_id = _BCM_TD2_WB_GET_NODE_ID_W1(wval);
                node = nodes[ii] + xnode_id;
                node_size = _BCM_TD2_WB_GET_NODE_SIZE_W1(wval);
    
                sal_memcpy(&node->fabric_pbmp, u8_scache_ptr,
                           pbmp_restore_size);
                u8_scache_ptr += scache_pbmp_size;
    
                u32_base_ptr_1 += 2;
                if (node_size > 3) {
                    u32_base_ptr_1++;
                }
            }
        }
        u32_scache_ptr = (uint32 *)u8_scache_ptr;
        mmu_info->curr_merger_index = *u32_scache_ptr++;
    } else {
        incr_size += sizeof(int);
        for(ii = 0; ii < 3; ii++) {
            incr_size += (node_list_sizes[ii] * sizeof(bcm_pbmp_t));
        }
        incr_size += sizeof(int);
    }

    /* BCM_WB_VERSION_1_7 Sync endpoint info.
     * In order to save scache, each entry size is fixed.
     * entry_size = sizeof(uint32)+ sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t).
     * entry 0 was reserved and is not synced.
     */
    if (recovered_ver >= BCM_WB_VERSION_1_7) {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            u8_scache_ptr = (uint8 *)u32_scache_ptr;
            BCM_IF_ERROR_RETURN(
                bcm_td2_cosq_endpoint_recover(unit, &u8_scache_ptr));
        }
    } else {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            incr_size += ((_BCM_TD2_NUM_ENDPOINT(unit) - 1) *
                          (sizeof(uint32) + sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t)));
            incr_size += sizeof(uint16) * SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) /
                         _TD2_EPCM_ENTRIES_PER_SET;
        }
    }

    if (incr_size > 0) {
        BCM_IF_ERROR_RETURN(
            soc_scache_realloc(unit, scache_handle, incr_size));
    }

    /* Update reference counts for DSCP_TABLE profile*/
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

    PBMP_ITER(all_pbmp, port) {
        if (IS_LB_PORT(unit, port)) {
            /* Loopback port base starts from 64 */
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ALL,
                                          port, &entry));
        index = soc_mem_field32_get(unit, PORT_TABm, &entry, TRUST_DSCP_PTRf);

        SOC_IF_ERROR_RETURN(_bcm_dscp_table_entry_reference(unit, index * 64,
                                                            64));
    }

    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_td2_cos_map_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL,
                                         SOC_INFO(unit).cpu_hg_index, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }

    for (pipe = 0; pipe < 2; pipe++) {
        mem = (pipe == _TD2_XPIPE) ? 
                    MMU_WRED_CONFIG_X_PIPEm : MMU_WRED_CONFIG_Y_PIPEm;
        for (ii = soc_mem_index_min(unit, mem);
             ii <= soc_mem_index_max(unit, mem); ii++) {
            BCM_IF_ERROR_RETURN(
                  soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &qentry));
             
            profile_index = soc_mem_field32_get(unit, mem, &qentry, PROFILE_INDEXf);
             
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, _bcm_td2_wred_profile[pipe][unit],
                                           profile_index, 1));
        }
    }

    /* Update PRIO2COS_LLFC register profile reference counter */
    profile_reg = _bcm_td2_llfc_profile[unit];
    PBMP_PORT_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (mmu_port == -1) {
            continue;
        }
        reg = llfc_cfgr[mmu_port/32];
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));

        index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
        COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
        COMPILER_64_TO_32_LO(tmp32, index_map);
        set_index = (tmp32 & 3);
        SOC_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, profile_reg, set_index * 16, 16));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_sync
 * Purpose:
 *      Sync endpoint info to scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcm_td2_cosq_endpoint_sync(int unit, uint8 **scache_ptr)
{
    uint32 endpoint_flags, entry_size;
    int ii;
    int profile_num;
    soc_profile_mem_t *profile_mem;
    uint16 *u16_scache_ptr;
    int ref_count;
    int rv = BCM_E_NONE;

    /* entry 0 was reserved and is not synced. */
    entry_size = sizeof(uint32)+ sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t);
    for (ii = 1; ii < _BCM_TD2_NUM_ENDPOINT(unit); ii++) {
        if (_BCM_TD2_ENDPOINT_IS_USED(unit, ii)) {
            endpoint_flags = _BCM_TD2_ENDPOINT(unit, ii)->flags;
            if (endpoint_flags & BCM_COSQ_CLASSIFIER_L2) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_TD2_ENDPOINT(unit, ii)->vlan),
                           sizeof(bcm_vlan_t));
                (*scache_ptr) += sizeof(bcm_vlan_t);
                sal_memcpy((*scache_ptr), _BCM_TD2_ENDPOINT(unit, ii)->mac,
                           sizeof(bcm_mac_t));
                (*scache_ptr) += (entry_size - sizeof(uint32) - sizeof(bcm_vlan_t));
            } else if (endpoint_flags & BCM_COSQ_CLASSIFIER_L3) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_TD2_ENDPOINT(unit, ii)->vrf),
                           sizeof(bcm_vrf_t));
                (*scache_ptr) += sizeof(bcm_vrf_t);
                if (endpoint_flags & BCM_COSQ_CLASSIFIER_IP6) {
                    sal_memcpy((*scache_ptr),
                               _BCM_TD2_ENDPOINT(unit, ii)->ip6_addr,
                               sizeof(bcm_ip6_t));
                } else {
                    sal_memcpy((*scache_ptr),
                               &(_BCM_TD2_ENDPOINT(unit, ii)->ip_addr),
                               sizeof(bcm_ip_t));
                }
                (*scache_ptr) += (entry_size - sizeof(uint32) - sizeof(bcm_vrf_t));
            } else if (endpoint_flags & BCM_COSQ_CLASSIFIER_GPORT) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_TD2_ENDPOINT(unit, ii)->gport),
                           sizeof(bcm_gport_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            } else if (endpoint_flags & BCM_COSQ_CLASSIFIER_L3_EGRESS) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_TD2_ENDPOINT(unit, ii)->egress_if),
                           sizeof(bcm_if_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            } else {
                return BCM_E_INTERNAL;
            }
        } else {
            sal_memset((*scache_ptr), 0, entry_size);
            (*scache_ptr) += entry_size;
        }
    }

    u16_scache_ptr = (uint16 *)(*scache_ptr);

    profile_num = SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) /
                  _TD2_EPCM_ENTRIES_PER_SET;
    profile_mem = _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit);
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _TD2_EPCM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }
    *scache_ptr = (uint8 *)u16_scache_ptr;

    return BCM_E_NONE;
}

int
bcm_td2_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *u8_scache_ptr;
    uint32 *u32_scache_ptr;
    uint16 *u16_scache_ptr;
    _bcm_td2_cosq_node_t *node;
    int node_list_sizes[3], ii, jj, cosq;
    _bcm_td2_cosq_node_t *nodes[3];
    int xnode_id, node_size, count, pipe;
    uint32 *psize;
    _bcm_td2_mmu_info_t *mmu_info;
    int alloc_size;
    int rv = BCM_E_NONE;
    int ref_count;
    int profile_num;
    soc_profile_mem_t *profile_mem;

    mmu_info = _bcm_td2_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }
    if ((SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) { 
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/  
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    u32_scache_ptr = (uint32*) scache_ptr;

    nodes[0] = &mmu_info->queue_node[0];
    node_list_sizes[0] = _BCM_TD2_NUM_L2_UC_LEAVES;
    nodes[1] = &mmu_info->mc_queue_node[0];
    node_list_sizes[1] = _BCM_TD2_NUM_L2_MC_LEAVES;
    nodes[2] = &mmu_info->sched_node[0];
    node_list_sizes[2] = _BCM_TD2_NUM_TOTAL_SCHEDULERS;

    for(ii = 0; ii < 3; ii++) {
        psize = u32_scache_ptr++;
        for(count = 0, jj = 0; jj < node_list_sizes[ii]; jj++) {
            node = nodes[ii] + jj;
            if (node->in_use == FALSE) {
                continue;
            }
            count += 1;
            node_size = 3;
            if ((node->remote_modid != -1) || (node->remote_port != -1) ||
                (node->hw_cosq != -1)) {
                node_size += 1;
            }
            
            if (node->parent == NULL) {
                xnode_id = 0x800;
            } else {
                xnode_id = _BCM_TD2_NODE_ID(node->parent, mmu_info->sched_node);
            }

            pipe = _BCM_TD2_PORT_TO_PIPE(unit, node->local_port);

            *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W1(ii, jj, node_size, xnode_id, pipe);
            cosq = ((node->attached_to_input == -1) ? 0x8 : node->attached_to_input);

            *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W2(
                   (node->hw_index == -1) ? 0x1000 : node->hw_index,
                    ((node->hw_cosq == -1) ? 8 : node->hw_cosq), cosq, node->level,
                    ((node->node_align_value == _BCM_TD2_START_INDEX_ALIGN_PFC) ? 1 : 0));

            *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W3(node->gport);

            if ((node->remote_modid != -1) || (node->remote_port != -1) ||
                (node->hw_cosq != -1)) {
                node_size += 1;
                *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W4(
                    (node->remote_modid == -1) ? 0x100 : node->remote_modid,
                    (node->remote_port == -1) ? 0x100 : node->remote_port,
                    (node->hw_cosq == -1) ? 0x8 : node->hw_cosq);
            }
        }
        *psize = count;
    }
    
    /* Sync TIME_DOMAINr field and count reference */
    alloc_size = _TD2_NUM_TIME_DOMAIN * sizeof(_bcm_td2_cosq_time_info_t);
    sal_memcpy(u32_scache_ptr, &time_domain[0], alloc_size);
    u32_scache_ptr += alloc_size / sizeof(uint32);
    soc_trident2_fc_map_shadow_sync(unit, &u32_scache_ptr);

    for (ii = 0; ii < 3; ii++) {
        for (count = 0, jj = 0; jj < node_list_sizes[ii]; jj++) {
            node = nodes[ii] + jj;
            if (node->in_use == FALSE) {
                continue;
            }

            *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W5(
                (node->numq_expandable), (node->type),
                ((node->attached_to_input == -1) ? 0x800 : 
                                             node->attached_to_input),
                ((node->hw_cosq == -1) ? 0x800 : node->hw_cosq));

            *u32_scache_ptr++ = _BCM_TD2_WB_SET_NODE_W6(
                (node->base_size),
                ((node->base_index == -1 ) ? 0x400 : node->base_index),
                ((node->numq == -1 ) ? 0x600 : node->numq));
        }
    }
    
    *u32_scache_ptr++ = mmu_info->ets_mode;
    *u32_scache_ptr++ = mmu_info->curr_shared_limit;

    u16_scache_ptr = (uint16 *)u32_scache_ptr;
    for (ii = 0; 
         ii < (SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _TD2_NUM_INTERNAL_PRI);
         ii++) {
        rv = soc_profile_mem_ref_count_get(unit, 
                                           _bcm_td2_ifp_cos_map_profile[unit],
                                           ii * _TD2_NUM_INTERNAL_PRI,
                                           &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }

    profile_num = SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) /
                  _TD2_SCM_ENTRIES_PER_SET;
    profile_mem = _bcm_td2_service_cos_map_profile[unit];
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _TD2_SCM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }

    profile_num = SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) /
                  _TD2_SPM_ENTRIES_PER_SET;
    profile_mem = _bcm_td2_service_port_map_profile[unit];
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _TD2_SPM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }
    
    scache_ptr = (uint8 *)u16_scache_ptr;
    
    BCM_IF_ERROR_RETURN(soc_td2_mmu_traffic_ctrl_wb_sync(unit, &scache_ptr));

    u32_scache_ptr = (uint32 *) scache_ptr;
    *u32_scache_ptr++ = NUM_COS(unit);
    scache_ptr = (uint8*)u32_scache_ptr;

    /* BCM_WB_VERSION_1_6: Sync DMVOQ fabric parameter & curr_merger_index. */
    *u32_scache_ptr++ = sizeof(bcm_pbmp_t);
    u8_scache_ptr = (uint8 *)u32_scache_ptr;
    for (ii = 0; ii < 3; ii++) {
        for (count = 0, jj = 0; jj < node_list_sizes[ii]; jj++) {
            node = nodes[ii] + jj;
            if (node->in_use == FALSE) {
                continue;
            }
            sal_memcpy(u8_scache_ptr, &node->fabric_pbmp, sizeof(bcm_pbmp_t));
            u8_scache_ptr += sizeof(bcm_pbmp_t);
        }
    }
    u32_scache_ptr = (uint32 *)u8_scache_ptr;
    *u32_scache_ptr++ = mmu_info->curr_merger_index;

    /* BCM_WB_VERSION_1_7 Sync endpoint info.
     * In order to save scache, each entry size is fixed.
     * entry_size = sizeof(uint32)+ sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t).
     * entry 0 was reserved and is not synced.
     */
    if (soc_feature(unit, soc_feature_endpoint_queuing)) {
        u8_scache_ptr = (uint8 *)u32_scache_ptr;
        BCM_IF_ERROR_RETURN(
            bcm_td2_cosq_endpoint_sync(unit, &u8_scache_ptr));
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
STATIC char*
_bcm_td2_cosq_node_lvl_str(soc_td2_node_lvl_e lvl)
{
    switch(lvl) {
        case SOC_TD2_NODE_LVL_ROOT:
            return "Root";
            break;
        case SOC_TD2_NODE_LVL_L0:
            return " L0";
            break;
        case SOC_TD2_NODE_LVL_L1:
            return "  L1";
            break;
        case SOC_TD2_NODE_LVL_L2:
            return "   Q";
            break;
        case SOC_TD2_NODE_LVL_MAX:
            break;
    }
    return "";
}

STATIC void
_bcm_td2_cosq_dump_node(_bcm_td2_cosq_node_t *node, int loff)
{
    LOG_CLI((BSL_META("%s.%d index=%d cosq=%d gport=0x%08x numq=%d\n"),
                _bcm_td2_cosq_node_lvl_str(node->level),
                        loff,
                        node->hw_index,
                        node->attached_to_input,
                        node->gport,
                        node->numq));

    if (node->child) {
        _bcm_td2_cosq_dump_node(node->child, 0);
    }

    if (node->sibling) {
        _bcm_td2_cosq_dump_node(node->sibling, loff+1);
    }
}

STATIC void
_bcm_td2_cosq_port_sw_dump(int unit, bcm_port_t port)
{
    _bcm_td2_mmu_info_t *mmu_info;
    int phy_port, mmu_port;
    soc_info_t *si;
    bcm_port_t local_port;
    int rv;
    _bcm_td2_cosq_node_t *node = NULL;

    si = &SOC_INFO(unit);
    mmu_info = _bcm_td2_mmu_info[unit];

    phy_port = si->port_l2p_mapping[port];

    if (phy_port == -1) {
        return;
    }

    mmu_port = si->port_p2m_mapping[phy_port];

    if (mmu_port == -1) {
        return;
    }

    rv = _bcm_td2_cosq_localport_resolve(unit, port, &local_port);
    if (BCM_FAILURE(rv) || (local_port < 0)) {
        return;
    }
    node = &mmu_info->sched_node[local_port];

    if (node == NULL) {
        return;
    }

    LOG_CLI((BSL_META("Logical Port: %d, Physical Port: %d\n"),
                        port,
                        phy_port));

    _bcm_td2_cosq_dump_node(node, 0);

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLEP */

/*
 * Function:
 *     bcm_td2_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td2_cosq_sw_dump(int unit)
{
#ifndef BCM_SW_STATE_DUMP_DISABLE
    bcm_port_t port;

    PBMP_ALL_ITER(unit, port) {
        _bcm_td2_cosq_port_sw_dump(unit, port);
    }
#endif
    return;
}

/*
 * Function:
 *     _bcm_td2_cosq_localport_resolve
 * Purpose:
 *     Resolve GRPOT if it is a local port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     local_port - (OUT) local port number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2_cosq_localport_resolve(int unit, bcm_gport_t gport,
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
        (_bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id));

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }

    *local_port = port;

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_port_has_ets(int unit, bcm_port_t port)
{
    _bcm_td2_mmu_info_t *mmu_info = _bcm_td2_mmu_info[unit];
    return mmu_info->ets_mode;
}

/*
 * Function:
 *     _bcm_td2_node_index_get
 * Purpose:
 *     Allocate free index from the given list
 * Parameters:
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
_bcm_td2_node_index_get(SHR_BITDCL *list, int start, int end,
                       int count, int align, int *id)
{
    int i, j, range_empty;

    *id = -1;

    if ((align <= 0) || (count == 0)) {
        return BCM_E_PARAM;
    }

    if (start & (align - 1)) {
        start = (start + align - 1) & ~(align - 1);
    }

    end = end - align + 1;

    for (i = start; i < end; i += align) {
        range_empty = 1;
        if ((i + count) <= end){
            for (j = i; j < (i + count); j++) {
                if (SHR_BITGET(list, j) != 0) {
                    range_empty = 0;
                    break;
                }
            }

            if (range_empty) {
                *id = i;
                return BCM_E_NONE;
            }
        } else {
            return BCM_E_RESOURCE;
        }
    }
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     _bcm_td2_node_index_set
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
_bcm_td2_node_index_set(_bcm_td2_cosq_list_t *list, int start, int range)
{
    if (range == 0) {
        return BCM_E_NONE;
    }
    if ((start < 0) ||
        (range < 0) ||
        ((start + range) > list->count)) {
        return BCM_E_INTERNAL;
    }
    SHR_BITSET_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_node_index_clear
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
_bcm_td2_node_index_clear(_bcm_td2_cosq_list_t *list, int start, int range)
{
    if (range == 0) {
        return BCM_E_NONE;
    }
    if ((start < 0) ||
        (range < 0) ||
        ((start + range) > list->count)) {
        return BCM_E_INTERNAL;
    }
    SHR_BITCLR_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_cosq_alloc_clear
 * Purpose:
 *     Allocate cosq memory and clear
 * Parameters:
 *     size       - (IN) size of memory required
 *     description- (IN) description about the structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC void *
_bcm_td2_cosq_alloc_clear(void *cp, unsigned int size, char *description)
{
    void *block_p;

    block_p = (cp) ? cp : sal_alloc(size, description);

    if (block_p != NULL) {
        sal_memset(block_p, 0, size);
    }

    return block_p;
}

/*
 * Function:
 *     _bcm_td2_cosq_free_memory
 * Purpose:
 *     Free memory allocated for mmu info members
 * Parameters:
 *     mmu_info       - (IN) MMU info pointer
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_td2_cosq_free_memory(_bcm_td2_mmu_info_t *mmu_info_p)
{
    int pipe;
    _bcm_td2_pipe_resources_t *res;

    if (mmu_info_p == NULL) {
        return;
    }
    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        res = _BCM_TD2_PRESOURCES(mmu_info_p, pipe);
        if (res->ext_qlist.bits != NULL) {
            sal_free(res->ext_qlist.bits);
            res->ext_qlist.bits = NULL;
        }
        if (res->l0_sched_list.bits != NULL) {
            sal_free(res->l0_sched_list.bits);
            res->l0_sched_list.bits = NULL;
        }
        if (res->l1_sched_list.bits != NULL) {
            sal_free(res->l1_sched_list.bits);
            res->l1_sched_list.bits = NULL;
        }
    }
}

/*
 * Function:
 *     _bcm_td2_cosq_node_get
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
_bcm_td2_cosq_node_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_module_t *modid, bcm_port_t *port, 
                       int *id, _bcm_td2_cosq_node_t **node)
{
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node_base = NULL;
    bcm_module_t modid_out = 0;
    bcm_port_t port_out = 0;
    uint32 encap_id = -1;
    int index;

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = (gport >> 16) & 0xff;
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        encap_id = BCM_GPORT_LOCAL_GET(gport);
        port_out = encap_id;
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        modid_out = SOC_GPORT_MODPORT_MODID_GET(gport);
        port_out = SOC_GPORT_MODPORT_PORT_GET(gport);
        encap_id = port_out;
    } else {
        return BCM_E_PORT;
    }

    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }

    if (port != NULL) {
        *port = port_out;
    }

    if (!_bcm_td2_cosq_port_has_ets(unit, port_out)) {
        return BCM_E_NOT_FOUND;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        node_base = mmu_info->queue_node;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        node_base = mmu_info->mc_queue_node;
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        node_base = mmu_info->queue_node;
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x3fff;
        index = encap_id;
        node_base = mmu_info->sched_node;
    } else {
        index = encap_id;
        node_base = mmu_info->sched_node;
    }

    if (index < 0) {
        return BCM_E_NOT_FOUND;
    }

    if (node_base[index].numq == 0) {
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
        if (*node) {
            _bcm_td2_cosq_node_t *tmp_node;
            tmp_node = *node;
            if (tmp_node->type == _BCM_TD2_NODE_SERVICE_UCAST) {
                *node = &node_base[index + cosq];
                if (id != NULL) {
                    *id = tmp_node->hw_index;
                }
            }
        }
 
    }

    return BCM_E_NONE;
}

int
_bcm_td2_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum)
{
    _bcm_td2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, gport, 0, modid, port, id, &node));
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
 *     _bcm_td2_cosq_gport_traverse
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
_bcm_td2_cosq_gport_traverse(int unit, bcm_gport_t gport,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    _bcm_td2_cosq_node_t *node;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, NULL, NULL, &node));

    if (node != NULL) {
        if (node->level == SOC_TD2_NODE_LVL_L2) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
                flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
            }
        } else {
            flags = BCM_COSQ_GPORT_SCHEDULER;
            if (node->node_align_value == _BCM_TD2_START_INDEX_ALIGN_PFC) {
                flags |= BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN;
            }
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
        _bcm_td2_cosq_gport_traverse(unit, node->sibling->gport, cb, user_data);
    }

    if (node->child != NULL) {
        _bcm_td2_cosq_gport_traverse(unit, node->child->gport, cb, user_data);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_child_node_at_input(_bcm_td2_cosq_node_t *node, int cosq,
                                  _bcm_td2_cosq_node_t **child_node)
{
    _bcm_td2_cosq_node_t *cur_node;

    for (cur_node = node->child; cur_node; cur_node = cur_node->sibling) {
        if (cur_node->attached_to_input == cosq) {
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
_bcm_td2_cosq_l2_gport(int unit, int port, int cosq,
                       _bcm_td2_node_type_e type, bcm_gport_t *gport,
                       _bcm_td2_cosq_node_t **p_node)
{
    int i, max;
    _bcm_td2_cosq_node_t *node, *fnode;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_pipe_resources_t *res;

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[port];
    res = port_info->resources;
    
    max  = 0;
    if ((type == _BCM_TD2_NODE_UCAST) || (type == _BCM_TD2_NODE_VOQ) ||
        (type == _BCM_TD2_NODE_VLAN_UCAST) || (type == _BCM_TD2_NODE_VM_UCAST) ||
         (type == _BCM_TD2_NODE_SERVICE_UCAST)) {
        max = _BCM_TD2_NUM_L2_UC_LEAVES;
        fnode = &res->p_queue_node[0];
    } else if (type == _BCM_TD2_NODE_MCAST) {
        max = _BCM_TD2_NUM_L2_MC_LEAVES;
        fnode = &res->p_mc_queue_node[0];
    } else {
        return BCM_E_PARAM;
    }

    for (i = 0; i < max; i++) {
        node = fnode + i;
        if (node->in_use && (node->local_port == port) &&
            (node->type == type) && (node->hw_cosq == cosq)) {
            if (gport) {
                *gport = node->gport;
            }
            if (p_node) {
                *p_node = node;
            }
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     _bcm_td2_cosq_index_resolve
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
_bcm_td2_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq, 
                           _bcm_td2_cosq_index_style_t style,                 
                           bcm_port_t *local_port, int *index, int *count)
{
    _bcm_td2_cosq_node_t *node, *cur_node;
    bcm_port_t resolved_port;
    int resolved_index = -1;
    int id, startq, numq;
    soc_info_t *si;
    int phy_port, mmu_port, mmu_port_pipe_off;
    uint32 entry0[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    si = &SOC_INFO(unit);

    if (cosq < -1) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        startq = 0;
        numq = NUM_COS(unit);
    } else {
        startq = cosq;
        numq = 1;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, port, cosq, NULL, &resolved_port, &id,
                                   &node));
        if (node->attached_to_input < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        numq = (node->numq != -1) ? node->numq : 64;
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = (IS_CPU_PORT(unit, resolved_port)) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    if (startq >= numq) {
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[resolved_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    mmu_port_pipe_off = (mmu_port >= 64) ? mmu_port - 64 : mmu_port;

    if ((node == NULL) && (BCM_GPORT_IS_MODPORT(port)) &&
        (_bcm_td2_cosq_port_has_ets(unit, resolved_port))) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, port, 0, 
                                                    NULL, NULL, NULL, &node));
    }
    
    switch (style) {
    case _BCM_TD2_COSQ_INDEX_STYLE_BUCKET:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port) || BCM_GPORT_IS_MODPORT(port)) {
                /* resolve the child attached to input cosq */
                BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, startq, &cur_node));
                resolved_index = cur_node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            if (_bcm_td2_cosq_port_has_ets(unit, resolved_port)) {
                LOG_ERROR(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit, "Not a GPORT type port in ETS mode.\n")));
                return BCM_E_PARAM;
            }
            if (IS_CPU_PORT(unit, resolved_port)) {
                resolved_index = SOC_INFO(unit).port_cosq_base[resolved_port] +
                                 startq;
                resolved_index = soc_td2_l2_hw_index(unit, resolved_index, 0);
            } else {
                BCM_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit, 
                                     resolved_port, SOC_TD2_NODE_LVL_L1, 
                                     startq, &resolved_index));
            }
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_QCN:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            if (IS_CPU_PORT(unit, resolved_port)) {
                return BCM_E_PARAM;
            } else {
                resolved_index = soc_td2_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
            }
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_WRED:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index + startq;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, startq, &cur_node));
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cur_node->gport)) {
                    resolved_index = cur_node->hw_index;
                } else {
                    return BCM_E_PARAM;
                }
            } else if (BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(_bcm_td2_cosq_l2_gport(unit, node->local_port, 
                                   startq, _BCM_TD2_NODE_UCAST, NULL, &cur_node));
                resolved_index = cur_node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            resolved_index = soc_td2_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
            numq = 1;
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_WRED_PORT:
        resolved_index = 1616 + (mmu_port_pipe_off*4);
        numq = 4;
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_WRED_POOL:
        if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    /* regular unicast queue */
                resolved_index = soc_td2_l2_hw_index(unit, node->hw_index + startq, 1);
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_uc_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = soc_td2_l2_hw_index(unit,
                    si->port_uc_cosq_base[resolved_port] + startq, 1);
        }
        mem = SOC_TD2_PMEM(unit, resolved_port,
                MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                            entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);

        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER:
        if (node != NULL) {
            if (!BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_child_node_at_input(node, startq, &cur_node));
            resolved_index = cur_node->hw_index;
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            if (IS_CPU_PORT(unit, resolved_port)) {
                resolved_index = soc_td2_l2_hw_index(unit,
                si->port_cosq_base[resolved_port] + startq, 0);
            } else {
                BCM_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit, 
                                    resolved_port, SOC_TD2_NODE_LVL_L1, startq, 
                                    &resolved_index));
            }
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_COS:
        if (node) {
            if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) ||
                (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))) {
                resolved_index = node->hw_cosq;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, startq, &cur_node));
                if (BCM_GPORT_IS_SCHEDULER(cur_node->gport)) {
                    return BCM_E_PARAM;
                }
                resolved_index = cur_node->hw_cosq;
            }
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            resolved_index = startq;
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE:
    case _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP:
        if (IS_CPU_PORT(unit, resolved_port) ||
            IS_LB_PORT(unit, resolved_port)) {
            return BCM_E_PARAM;
        }
        if (node) {
            resolved_index = node->hw_index + startq;
            numq = 1;
        } else {
            /* Not support -1 for queue group index resolving, as group may not consecutive */
            if ((cosq == -1) &&
                (style == _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = soc_td2_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
        }
        if (style == _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP) {
            mem = SOC_TD2_PMEM(unit, resolved_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                                           MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                             resolved_index, entry0));
            if (soc_mem_field32_get(unit, mem, entry0, QGROUP_VALIDf)) {
                resolved_index = soc_mem_field32_get(unit, mem, entry0, QGROUPf);
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE:
        if (node) {
            resolved_index = node->hw_index + startq;
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = soc_td2_l2_hw_index(unit, 
                si->port_cosq_base[resolved_port] + startq, 0);
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL:
        if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    /* regular unicast queue */
                resolved_index = soc_td2_l2_hw_index(unit, node->hw_index + startq, 1);
                mem = SOC_TD2_PMEM(unit, resolved_port, 
                        MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = soc_td2_l2_hw_index(unit,
                    si->port_cosq_base[resolved_port] + startq, 0) - 1480;
                mem = SOC_TD2_PMEM(unit, resolved_port, 
                        MMU_THDM_MCQE_QUEUE_CONFIG_0m, MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            } else { /* scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = soc_td2_l2_hw_index(unit, 
                    si->port_cosq_base[resolved_port] + startq, 0) - 1480;
            mem = SOC_TD2_PMEM(unit, resolved_port, 
                    MMU_THDM_MCQE_QUEUE_CONFIG_0m, MMU_THDM_MCQE_QUEUE_CONFIG_1m);
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                            entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);

        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_UC_EGR_POOL:
        if (IS_CPU_PORT(unit, resolved_port) ||
            IS_LB_PORT(unit, resolved_port)) {
            return BCM_E_PARAM;
        }

        numq = si->port_num_uc_cosq[resolved_port];
        if (startq >= numq) {
            return BCM_E_PARAM;
        }

        resolved_index = soc_td2_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
        mem = SOC_TD2_PMEM(unit, resolved_port, 
                MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                            entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST:
        if (node) {
            resolved_index = node->hw_index + startq;
            resolved_index -= soc_td2_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port], 1);
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = startq;
        }
        break;

    case _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST:
        if (node) {
            resolved_index = node->hw_index + startq;
            resolved_index -= soc_td2_l2_hw_index(unit,
                si->port_cosq_base[resolved_port], 0);
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = startq;
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

STATIC int _bcm_td2_cosq_min_child_index(_bcm_td2_cosq_node_t *child, int lvl, int uc)
{
    int min_index = -1;

    if ((lvl == SOC_TD2_NODE_LVL_L2) && (uc)) {
        while (child) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(child->gport)) {
                if (min_index == -1) {
                    min_index = child->hw_index;
                }
                
                if (child->hw_index < min_index) {
                    min_index = child->hw_index;
                }
            }
            child = child->sibling;
        }
    } else if ((lvl == SOC_TD2_NODE_LVL_L2) && (!uc)) {
        while (child) {
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(child->gport)) {
                if (min_index == -1) {
                    min_index = child->hw_index;
                }
                
                if (child->hw_index < min_index) {
                    min_index = child->hw_index;
                }
            }
            child = child->sibling;
        }
    } else {
        while (child) {
            if (min_index == -1) {
                min_index = child->hw_index;
            }
            
            if (child->hw_index < min_index) {
                min_index = child->hw_index;
            }
            child = child->sibling;
        }
    }

    return min_index;
}

STATIC int
_bcm_td2_child_state_check(int unit, bcm_port_t gport,
                           bcm_cos_queue_t cosq, int ets_mode)
{
    _bcm_td2_cosq_node_t *node, *child_node = NULL;
    bcm_port_t local_port;
    int level, sch_index, empty = 0;
    int rv = BCM_E_NONE;
    soc_timeout_t timeout;
    uint32 timeout_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    static const soc_mem_t memx[] = {
        INVALIDm,
        ES_PIPE0_LLS_L0_CHILD_STATE1m,
        ES_PIPE0_LLS_L1_CHILD_STATE1m,
        ES_PIPE0_LLS_L2_CHILD_STATE1m
    };
    static const soc_mem_t memy[] = {
        INVALIDm,
        ES_PIPE1_LLS_L0_CHILD_STATE1m,
        ES_PIPE1_LLS_L1_CHILD_STATE1m,
        ES_PIPE1_LLS_L2_CHILD_STATE1m
    };

    /* Timeout val = Static for now. Need to quantify the max wait time */
    timeout_val = 2000000;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    if (ets_mode & (!IS_TD2_HSP_PORT(unit, local_port))) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));

        /* Get the Child node for the Given Gport */
        rv = _bcm_td2_cosq_child_node_at_input(node, cosq, &child_node);

        if ((!child_node) || (rv == BCM_E_NOT_FOUND)) {
            /* No Child present */
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

        level = child_node->level;
        sch_index = child_node->hw_index;

        /* Child State Check is NOT necessary for Non-L0/L1/L2 levels */
        if ((level < SOC_TD2_NODE_LVL_L0) || (level > SOC_TD2_NODE_LVL_L2)) {
            /* Sanity for Node's level should be done in the caller */
            return BCM_E_NONE;
        }

        mem = SOC_TD2_PMEM(unit, local_port, memx[level], memy[level]); 

        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        soc_timeout_init(&timeout, timeout_val, 0);

        for(;;) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                             sch_index, entry));
            empty = 0;
            if (soc_timeout_check(&timeout)) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Timeout during Child lists Not zero\n")));
                return BCM_E_BUSY;
            }
            empty += soc_mem_field32_get(unit, mem, &entry, C_ON_ACTIVE_LISTf);
            empty += soc_mem_field32_get(unit, mem, &entry, C_ON_MIN_LISTf);
            empty += soc_mem_field32_get(unit, mem, &entry, C_NOT_EMPTYf);

            if (!empty) {
                break;
            }
        }
    } else {
        /* TBD: Need to implement for Non-ETS mode */
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_node_queue_state_check(int unit, 
        _bcm_td2_cosq_node_t *node, int ets_mode)
{
    int cosq = 0;
    
    if (node == NULL) {
        return BCM_E_PARAM;
    }

    for (cosq = 0; cosq < node->numq; cosq++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_child_state_check(unit, node->gport,
                                          cosq, ets_mode));
    }

    if ((node->level <= SOC_TD2_NODE_LVL_L1) &&
        (node->child != NULL)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_queue_state_check(unit,
                                 node->child,ets_mode));
    }
        
    if (node->sibling != NULL) {

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_queue_state_check(unit,
                               node->sibling,ets_mode));
    }

    return BCM_E_NONE;
}

int 
_bcm_td2_cosq_port_queue_state_check(int unit, bcm_port_t gport)
{
    bcm_port_t local_port;
    int   ets_mode;
    _bcm_td2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    ets_mode = _bcm_td2_cosq_port_has_ets(unit, local_port);
    if (!ets_mode) {
        return BCM_E_NONE;
    }
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_gport_get(unit, local_port,
                                         &gport));

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                  NULL, NULL, &node));
    
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_queue_state_check(unit,
                                    node,ets_mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_dynamic_sched_update_begin
 * Purpose:
 *     Set settings to enable Dynamic Sched update.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_dynamic_sched_update_begin(int unit, bcm_port_t gport,
                                    bcm_cos_queue_t cosq)
{
    int ets_mode = 0;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;

    if (SAL_BOOT_SIMULATION) {
       return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    SOC_IF_ERROR_RETURN(
        soc_td2_port_traffic_egr_enable_set(unit, local_port, 0));

    /* CPU port and loopback port need no cell drain to happen */
    if (!IS_CPU_PORT(unit, local_port) && !IS_LB_PORT(unit, local_port)) {
        /* Drain the cells assoiciated to the port */
        _bcm_td2_port_drain_cells(unit, local_port);
    }

    ets_mode = _bcm_td2_cosq_port_has_ets(unit, local_port);
    /* Poll Child state to verify if Child is still in Parent list */
    rv = _bcm_td2_child_state_check(unit, gport, cosq, ets_mode);
    if (rv != BCM_E_NONE) {
        /* Logging the Error. Not stopping the flow for now. */
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "ERROR: During Child State Check(rv %d)\n"),
                   rv));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_dynamic_sched_reset_thd(int unit, bcm_port_t gport,
                                 bcm_cos_queue_t cosq, int ets_mode)
{
    _bcm_td2_cosq_node_t *node, *child_node = NULL;
    bcm_port_t local_port;
    int level, sch_index;
    int rv = BCM_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    static const soc_mem_t memx[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_0m,
        MMU_MTRO_L1_MEM_0m,
        MMU_MTRO_L2_MEM_0m
    };
    static const soc_mem_t memy[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_1m,
        MMU_MTRO_L1_MEM_1m,
        MMU_MTRO_L2_MEM_1m
    };

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    if (ets_mode & (!IS_TD2_HSP_PORT(unit, local_port))) {
        uint32 min_thd = 0;
        uint32 max_thd = 0;

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));

        /* Get the Child node for the Given Gport */
        rv = _bcm_td2_cosq_child_node_at_input(node, cosq, &child_node);

        if ((!child_node) || (rv == BCM_E_NOT_FOUND)) {
            /* No Child present */
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

        level = child_node->level;
        sch_index = child_node->hw_index;

        /* Child State Check is NOT necessary for Non-L0/L1/L2 levels */
        if ((level < SOC_TD2_NODE_LVL_L0) || (level > SOC_TD2_NODE_LVL_L2)) {
            /* Sanity for Node's level should be done in the caller */
            return BCM_E_NONE;
        }

        mem = SOC_TD2_PMEM(unit, local_port, memx[level], memy[level]); 

        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_EGRESS_METERING_LOCK(unit);
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                         sch_index, entry);
        if (rv != BCM_E_NONE) {
            SOC_EGRESS_METERING_UNLOCK(unit);
            return rv;
        }
        min_thd = soc_mem_field32_get(unit, mem, &entry, MIN_THD_SELf);
        max_thd = soc_mem_field32_get(unit, mem, &entry, MAX_THD_SELf);

        if (min_thd || max_thd) {
            /* Set Min_Thd of the node to 0 in MTRO_CONFIG_xxx_MEM */
            soc_mem_field32_set(unit, mem, entry, MIN_THD_SELf, 0);
            soc_mem_field32_set(unit, mem, entry, MAX_THD_SELf, 0);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                              sch_index, entry);
            if (rv != BCM_E_NONE) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return rv;
            }

            /* A uSec delay */
            sal_usleep(1);
            /* Reset MIN_THD value back to original one.
             * This step is to activate the MIN_THD to LLS.
             */
            soc_mem_field32_set(unit, mem, entry, MIN_THD_SELf, min_thd);
            soc_mem_field32_set(unit, mem, entry, MAX_THD_SELf, max_thd);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                              sch_index, entry);
            if (rv != BCM_E_NONE) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return rv;
            }
        }
        SOC_EGRESS_METERING_UNLOCK(unit);
    } else {
        /* TBD: Need to implement for Non-ETS mode */
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_dynamic_sched_update_end
 * Purpose:
 *     Reset settings back to original state after Dynamic Sched update is complete.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_dynamic_sched_update_end(int unit, bcm_port_t gport,
                                  bcm_cos_queue_t cosq)
{
    int ets_mode = 0;
    bcm_port_t local_port;
    int rv = BCM_E_NONE;

    if (SAL_BOOT_SIMULATION) {
       return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    ets_mode = _bcm_td2_cosq_port_has_ets(unit, local_port);
    /* Reset MIN_THD for the respective Node/Queue */
    rv = _bcm_td2_dynamic_sched_reset_thd(unit, gport, cosq, ets_mode); 
    if (rv != BCM_E_NONE) {
        /* Logging the Error. Not stopping the flow for now. */
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "ERROR: During Reset MIN_THD process(rv %d)\n"),
                   rv));
    }
    
    SOC_IF_ERROR_RETURN(
        soc_td2_port_traffic_egr_enable_set(unit, local_port, 1));

    return BCM_E_NONE;

}

STATIC int
_bcm_td2_attach_node_in_hw(int unit, _bcm_td2_cosq_node_t *node)
{
    int local_port, fc = 0, fmc = 0;
    _bcm_td2_cosq_node_t *child_node;
    int num_spri = 0, first_sp_child = 0, first_sp_mc_child = 0;
    uint32 ucmap = 0, spmap = 0;
    soc_trident2_sched_type_t sched_type;

    local_port = node->local_port;

    if (node->level == SOC_TD2_NODE_LVL_ROOT) {
        return 0;
    } else if (node->level == SOC_TD2_NODE_LVL_L2) {
        fc = _bcm_td2_cosq_min_child_index(node->parent->child, SOC_TD2_NODE_LVL_L2, 1);
        if (fc < 0) {
            fc = 0;
        }
        fmc = _bcm_td2_cosq_min_child_index(node->parent->child, SOC_TD2_NODE_LVL_L2, 0);
        if (fmc < 0) {
            fmc = 1480;
        }
    } else {
        fc = _bcm_td2_cosq_min_child_index(node->parent->child, node->level, 0);
        fmc = 0;
    }

    /* If node is scheduler, reset the scheduler and attach it to the parent.*/
    BCM_IF_ERROR_RETURN(
        soc_td2_cosq_set_sched_parent(unit, local_port, node->level, 
                                      node->hw_index, node->parent->hw_index));

    sched_type = _soc_trident2_port_sched_type_get(unit, local_port);

    if (sched_type == SOC_TD2_SCHED_LLS) {
        /* Get num_spri and ucmap config from parent node. */
        BCM_IF_ERROR_RETURN(
            soc_td2_cosq_get_sched_child_config(unit, local_port,
                                                node->parent->level,
                                                node->parent->hw_index,
                                                &num_spri, &first_sp_child,
                                                &first_sp_mc_child, &ucmap,
                                                &spmap));

        /*
         * If there is no SP child under a schedule node, fc of this node will
         * be set to the index of min child.
         * Else, fc of this node will be set to the index of first SP child.
         */
        if (num_spri == 0) {
            if (fc != first_sp_child && fc == node->hw_index) {
                first_sp_child = fc;
            }

            if (node->level == SOC_TD2_NODE_LVL_L2 && fmc != first_sp_mc_child
                                      && fmc == node->hw_index) {
                first_sp_mc_child = fmc;
            }
        }

        /* Retain the num_spri only if the current node is NOT the first_sp_child */
        if (first_sp_child == node->hw_index ||
            first_sp_mc_child == node->hw_index) {
            num_spri = 0;
            ucmap = 0;
        }

        /* set the node by default in WRR mode with wt=1 */
        BCM_IF_ERROR_RETURN(soc_td2_cosq_set_sched_config(unit,
                            local_port, node->parent->level,
                            node->parent->hw_index, node->hw_index,
                            num_spri, first_sp_child, first_sp_mc_child, ucmap,
                            0, SOC_TD2_SCHED_MODE_WRR, 1));
    }


    if (node->child) {
        child_node = node->child;
        while (child_node) {
            BCM_IF_ERROR_RETURN(_bcm_td2_attach_node_in_hw(unit, child_node));
            child_node = child_node->sibling;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_hsp_port_count_per_pipe_get(int unit, int pipe)
{
    bcm_port_t port;
    int count = 0;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if(SOC_IS_TD2P_TT2P(unit)) {
        BCM_IF_ERROR_RETURN(
                soc_td2p_num_hsp_reserved_per_pipe(unit, pipe, &count));
    } else
#endif
    {
        PBMP_ALL_ITER(unit, port) {
            if (IS_TD2_HSP_PORT(unit, port) && 
                pipe == _BCM_TD2_PORT_TO_PIPE(unit, port)) {
                count++;
            }
        }
    }
    return count;
}

/*
 * Function:
 *     _bcm_td2_cosq_node_resolve
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
_bcm_td2_cosq_node_resolve(int unit, _bcm_td2_cosq_node_t *node,
                          bcm_cos_queue_t cosq)
{
    _bcm_td2_cosq_node_t *cur_node, *parent;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_pipe_resources_t * res;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_list_t *list;
    int numq, id, ii, rv, max_cosq = -1;
    int valid_index_lo, valid_index_hi;
    bcm_port_t port;
    int phy_port, mmu_port;
    int pipe;
    int old_base_index = -1;
    int old_base_size = -1;
    /* An array was used to find first free slot in L0/L1/L2 table
       So choose the one of the longest length of the three table */
    SHR_BIT_DCL_CLR_NAME(map, 1480);

    if ((parent = node->parent) == NULL) {
        /* Not attached, should never happen */
        return BCM_E_NOT_FOUND;
    }

    if (parent->numq == 0 || parent->numq < -1) {
        return BCM_E_PARAM;
    }

    port = node->local_port;

    if (parent->numq != -1) {
        /* find the current attached_to_input usage */
        for (cur_node = parent->child; cur_node != NULL;
             cur_node = cur_node->sibling) {
            if (cur_node->attached_to_input >= 0) {
                SHR_BITSET(map, cur_node->attached_to_input);
            }
        }

        if (cosq < 0) {
            for (ii = 0; ii < parent->numq; ii++) {
                if (!SHR_BITGET(map, ii)) {
                    node->attached_to_input = ii;
                    cosq = ii;
                    break;
                }
            }
            if (ii == parent->numq) {
                return BCM_E_PARAM;
            }
        } else {
            if (SHR_BITGET(map, cosq)) {
                return BCM_E_EXISTS;
            }
            node->attached_to_input = cosq;
        }
    } else if (parent->numq == -1) {
        for (cur_node = parent->child; cur_node != NULL;
             cur_node = cur_node->sibling) {
            if (cur_node->attached_to_input >= 0) {
                SHR_BITSET(map, cur_node->attached_to_input);
                max_cosq = (cur_node->attached_to_input > max_cosq)? \
                           cur_node->attached_to_input : max_cosq;
            }
        }
        if (cosq >= 0) {
            if (SHR_BITGET(map, cosq)) {
                return BCM_E_EXISTS;
            }
        } else {
            /* Attach anywhere. find first free slot */
            for (ii = 0; ii < 1480; ii++) {
                if (!SHR_BITGET(map, ii)) {
                    cosq = ii;
                    break;
                }
            }
            if (cosq < 0) { /* No available slot. */
                return BCM_E_FULL;
            }
        }
        node->attached_to_input = cosq;
        max_cosq = (node->attached_to_input > max_cosq)? \
                   node->attached_to_input : max_cosq;
    }

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[port];
    res = port_info->resources;
    numq = (parent->numq == -1) ? (max_cosq + 1) : parent->numq;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);

    switch (parent->level) {
        case SOC_TD2_NODE_LVL_ROOT:
            list = &res->l0_sched_list;
            if ((parent->numq == -1) && parent->base_index >= 0 &&
                parent->base_size != numq) {
                old_base_index = parent->base_index;
                old_base_size = parent->base_size;
                _bcm_td2_node_index_clear(list, parent->base_index,
                                          parent->base_size);
                parent->base_index = -1;
            }

            if (parent->base_index < 0) {
                if (IS_CPU_PORT(unit, port)) {
                    valid_index_lo = (SOC_IS_TD2P_TT2P(unit)?259:261);
                    valid_index_hi = _BCM_TD2_NUM_L0_SCHEDULER_PER_PIPE;
                } else if (IS_TD2_HSP_PORT(unit, port)) {
                    valid_index_lo = _BCM_TD2_HSP_L0_INDEX(unit, mmu_port, 0);
                    valid_index_hi = _BCM_TD2_HSP_L0_INDEX(unit, (mmu_port + 1),
                                                           0);
                } else {
                    valid_index_lo = _bcm_td2_cosq_hsp_port_count_per_pipe_get(unit, pipe) * 
                                     _BCM_TD2_HSP_PORT_MAX_L0;
                    /* upper 4 L0 nodes reserverd for cpu */
                    valid_index_hi = (SOC_IS_TD2P_TT2P(unit)?259:261);
                }

                if (IS_CPU_PORT(unit, port) ||
                    IS_TD2_HSP_PORT(unit, port)) {
                    rv = _bcm_td2_node_index_get(list->bits, valid_index_lo, 
                                               valid_index_hi, numq, 1, &id);
                } else {
                    rv = _bcm_td2_node_index_get(list->bits, valid_index_lo, 
                                                 valid_index_hi, numq,
                                                 node->node_align_value, &id);
                }
                if (rv) {
                    parent->base_index = old_base_index;
                    _bcm_td2_node_index_set(list, old_base_index,
                                              old_base_size);
                    return rv;
                }

                _bcm_td2_node_index_set(list, id, numq);
                parent->base_index = id;
                node->hw_index = parent->base_index + node->attached_to_input;
                parent->base_size = numq;
                if (parent->base_index != old_base_index){
                    for (id = old_base_index;
                         id < old_base_index + old_base_size;
                         id++) {
                        BCM_IF_ERROR_RETURN(soc_td2_cosq_set_sched_parent(unit,
                                port, node->level, id,
                                _soc_td2_invalid_parent_index(unit, node->level)));
                    }
                }
                for (cur_node = parent->child; cur_node;
                     cur_node = cur_node->sibling) {
                    if (cur_node->attached_to_input >= 0) {
                        cur_node->hw_index = parent->base_index +
                                             cur_node->attached_to_input;
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_attach_node_in_hw(unit, cur_node));
                    }
                }
            } else {
                node->hw_index = parent->base_index + node->attached_to_input;
            }
            node->hw_cosq = node->attached_to_input;

            break;

        case SOC_TD2_NODE_LVL_L0:
            list = &res->l1_sched_list;
            if (IS_TD2_HSP_PORT(unit, port)) {
                if (node->attached_to_input >= _BCM_TD2_HSP_PORT_MAX_L1) {
                    return BCM_E_PARAM;
                }

                if ((parent->hw_index % _BCM_TD2_HSP_PORT_MAX_L0) == 0) {
                    /* L0.0 could only be attached by MC nodes */
                    if (node->type == _BCM_TD2_NODE_MCAST) {
                        uint32 rval = 0;
                        uint32 mc_group_mode = 0;
                        SOC_IF_ERROR_RETURN(
                            READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
                        mc_group_mode  = soc_reg_field_get(
                                            unit, HSP_SCHED_PORT_CONFIGr, 
                                            rval, MC_GROUP_MODEf);
                        /* check mc group mode and the node offset */
                        if ((!mc_group_mode) ||
                            ((node->hw_index % _BCM_TD2_HSP_PORT_MAX_L1) >=
                             _BCM_TD2_HSP_PORT_MAX_COS)) {
                            return BCM_E_PARAM;
                        }
                        break;
                    } else {
                        return BCM_E_PARAM;
                    }
                }

                BCM_IF_ERROR_RETURN(_bcm_td2_node_index_get(list->bits, 
                                    _BCM_TD2_HSP_PORT_MAX_L1 * (mmu_port & 0x3f), 
                                    (_BCM_TD2_HSP_PORT_MAX_L1) * ((mmu_port & 0x3f) + 1),
                                    1, 1, &id));
                if (parent->base_index == -1) {
                    parent->base_index = id;
                }
                node->hw_index = id;

                if ((node->hw_index % _BCM_TD2_HSP_PORT_MAX_L1) >= 8) {
                    if ((parent->hw_index % _BCM_TD2_HSP_PORT_MAX_L0) != 4) {
                        return BCM_E_PARAM;
                    }
                } else {
                    if ((parent->hw_index % _BCM_TD2_HSP_PORT_MAX_L0) == 4) {
                        return BCM_E_PARAM;
                    }
                }
                _bcm_td2_node_index_set(list, id, 1);
            } else {
                /* Delete current COSQs if numq needs to be expanded. */
                if ((parent->base_index >= 0) &&
                    (parent->numq == -1) && (cosq == max_cosq)) {
                    old_base_index = parent->base_index;
                    old_base_size = parent->base_size;
                    _bcm_td2_node_index_clear(list, parent->base_index,
                                              parent->base_size);
                    parent->base_index = -1;
                }
                if (parent->base_index < 0) {
                    rv = _bcm_td2_node_index_get(list->bits,
                                                _BCM_TD2_HSP_PORT_MAX_L1 * _bcm_td2_cosq_hsp_port_count_per_pipe_get(unit, pipe), 
                                                _BCM_TD2_NUM_L1_SCHEDULER_PER_PIPE, 
                                                numq, node->node_align_value, &id);
                    if (rv) {
                        _bcm_td2_node_index_set(list, old_base_index,
                                                  old_base_size);
                        return rv;
                    }
                    _bcm_td2_node_index_set(list, id, numq);
                    parent->base_size = numq;
                    parent->base_index = id;
                    node->hw_index = parent->base_index + node->attached_to_input;
                    if (parent->base_index != old_base_index){
                        for (id = old_base_index;
                             id < old_base_index + old_base_size;
                             id++) {
                            BCM_IF_ERROR_RETURN(soc_td2_cosq_set_sched_parent(unit,
                                    port, node->level, id,
                                    _soc_td2_invalid_parent_index(unit, node->level)));
                        }
                    }
                    for (cur_node = parent->child; cur_node;
                         cur_node = cur_node->sibling) {
                        if (cur_node->attached_to_input >= 0) {
                            cur_node->hw_index = parent->base_index +
                                                 cur_node->attached_to_input;
                            BCM_IF_ERROR_RETURN(
                                _bcm_td2_attach_node_in_hw(unit, cur_node));
                        }
                    }
                } else {
                    node->hw_index = parent->base_index + node->attached_to_input;
                }
            }
            node->hw_cosq = node->attached_to_input;

            break;

        case SOC_TD2_NODE_LVL_L1:
            if (IS_TD2_HSP_PORT(unit, port)) {
                uint32     rval = 0;
                uint32     mc_group_mode = 0;
                SOC_IF_ERROR_RETURN(
                    READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
                mc_group_mode  = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                                   rval, MC_GROUP_MODEf);
                if (mc_group_mode &&
                    ((node->hw_index % _BCM_TD2_HSP_PORT_MAX_L1) <
                     _BCM_TD2_HSP_PORT_MAX_COS)) {
                    if (node->attached_to_input >= 1) {
                        return BCM_E_PARAM;
                    }
                    if (node->type == _BCM_TD2_NODE_MCAST) {
                        return BCM_E_PARAM;
                    }
                } else {
                    if (node->attached_to_input >= _BCM_TD2_HSP_PORT_MAX_L2) {
                        return BCM_E_PARAM;
                    }
                    if ((node->sibling) &&
                        (node->type == node->sibling->type)) {
                        return BCM_E_PARAM;
                    }
                }
            }
            if ((node->hw_index == -1) && 
                    (node->type != _BCM_TD2_NODE_VOQ)) {
                return BCM_E_PARAM;
            } else if (node->type == _BCM_TD2_NODE_VOQ) {
                if (parent->base_index < 0) {
                    BCM_IF_ERROR_RETURN(
                            _bcm_td2_node_index_get(res->ext_qlist.bits,
                                res->num_base_queues,
                               _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE  , 
                               _TD2_NUM_COS(unit), 
                                _TD2_NUM_COS(unit), &id));
                    /* The following logic is to make sure the id is always
                     *  multiple of 8 
                     */
                    while ( id % 8 != 0) {
                        id = ((id + 8) & 0xfffffff8);
                        if (id >  _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE ) {
                            return BCM_E_RESOURCE;
                        }
                        BCM_IF_ERROR_RETURN(
                                _bcm_td2_node_index_get(res->ext_qlist.bits, id,
                                    _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE, 
                                    NUM_COS(unit), 
                                    NUM_COS(unit), &id));
                    }
                    _bcm_td2_node_index_set(&res->ext_qlist, id, 
                            _TD2_NUM_COS(unit));
                    parent->base_index = id ;
                    node->hw_index = parent->base_index;
                    node->hw_cosq = node->hw_index % NUM_COS(unit);
                } else {
                    node->hw_index = parent->base_index + 
                        node->attached_to_input;
                    node->hw_cosq = node->hw_index % NUM_COS(unit);
                }
            }
            break;

        case SOC_TD2_NODE_LVL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    BCM_IF_ERROR_RETURN(_bcm_td2_attach_node_in_hw(unit, node));
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_detach_node_in_hw(int unit, _bcm_td2_cosq_node_t *node)
{
    int local_port;

    local_port = node->local_port;

    if (!IS_TD2_HSP_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_sched_parent_child_set(unit,
                            local_port, node->parent->level,
                            node->parent->hw_index, node->hw_index, 
                            SOC_TD2_SCHED_MODE_WRR, 1, node));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_cosq_node_unresolve
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
_bcm_td2_cosq_node_unresolve(int unit, _bcm_td2_cosq_node_t *node, 
                             bcm_cos_queue_t cosq)
{
    _bcm_td2_cosq_node_t *parent;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t * res;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_cosq_list_t *list;
    int numq = 1, port;

    if (node->attached_to_input < 0) {
        /* Has been unresolved already */
        return BCM_E_NONE;
    }

    parent = node->parent;

    if (!parent) {
        return BCM_E_PARAM;
    }

    mmu_info = _bcm_td2_mmu_info[unit];

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, node->gport, 0, NULL, &port, NULL, NULL));

    port_info = &mmu_info->port_info[port];
    res = port_info->resources;

    list = NULL;

    switch (parent->level) {
        case SOC_TD2_NODE_LVL_ROOT:
            break;

        case SOC_TD2_NODE_LVL_L0:
            if (IS_TD2_HSP_PORT(unit, port) &&
                (node->type != _BCM_TD2_NODE_MCAST)) {    /* L0.0 of HSP port. */
                list = &res->l1_sched_list;
            }
            break;

        case SOC_TD2_NODE_LVL_L1:
            break;

        case SOC_TD2_NODE_LVL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    BCM_IF_ERROR_RETURN(_bcm_td2_detach_node_in_hw(unit, node));
    
    if (list) {
        _bcm_td2_node_index_clear(list, node->hw_index, numq);
    }
    node->attached_to_input = -1;
    
    return BCM_E_NONE;
}

typedef struct _bcm_td2p_lls_info_s {
    int child_lvl; /* level of the node */
    int child_hw_index; /* hw index of the node */
    int node_count[SOC_TD2_NODE_LVL_MAX]; /* total number of nodes in the lls */
    int count[SOC_TD2_NODE_LVL_MAX]; /* number of nodes in each level */
    int offset[SOC_TD2_NODE_LVL_MAX]; /* index of the starting node at each level */
   
    int kbit_max; /* max speed for the node */
    int kbit_min; /* min speed for the node */
    uint32  *mtro_entries; /* entries to maintain mmu mtro level mem */
    int mtro_restore; /*indicate available data in mtro_entries*/
} _bcm_td2p_lls_info_t;

/*
 * Function:
 *     _bcm_td2_lls_node_count
 * Purpose:
 *     This function counts the number of nodes at a particular level
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number 
 *     level      - (IN) level of the node in LLS
 *     hw_index   - (IN) h/w index of the node
 *     cookie     - (IN) state of the LLS structure  
 */
STATIC int _bcm_td2_lls_node_count(int unit, int port, int level, 
                                   int hw_index, void *cookie)
{
    _bcm_td2p_lls_info_t *i_lls_tree = (_bcm_td2p_lls_info_t *) cookie;
    i_lls_tree->node_count[level] += 1;
    return 0;
}

/*
 * Function:
 *     _bcm_td2_lls_shapers_init
 * Purpose:
 *     This function reset MTRO shapers so that each node can only schedule upto
 *     the max rate.
 * Parameters:
 *     unit       - (IN) unit number
 */
STATIC int
_bcm_td2_lls_shapers_init(int unit)
{
    uint8 *mbuf = NULL;
    int table, alloc_size, rv = SOC_E_NONE;
    soc_mem_t config_mem;
    static const soc_mem_t mem[] = {
        MMU_MTRO_L0_MEM_0m,
        MMU_MTRO_L1_MEM_0m,
        MMU_MTRO_L2_MEM_0m,
        MMU_MTRO_L0_MEM_1m,
        MMU_MTRO_L1_MEM_1m,
        MMU_MTRO_L2_MEM_1m
    };

    for (table = 0; table < COUNTOF(mem); table++) {
        config_mem = mem[table];
        alloc_size = SOC_MEM_TABLE_BYTES(unit, config_mem);
        mbuf = soc_cm_salloc(unit, alloc_size, "MMU_MTRO entries");
        if (NULL == mbuf) {
            return BCM_E_MEMORY;
        }
        sal_memset(mbuf, 0, alloc_size);
        rv = soc_mem_write_range(unit, config_mem, MEM_BLOCK_ANY,
                                 soc_mem_index_min(unit, config_mem),
                                 soc_mem_index_max(unit, config_mem),
                                 mbuf);
        soc_cm_sfree(unit, mbuf);
        mbuf = NULL;
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_lls_shapers_remove
 * Purpose:
 *     This function removes shapers on the nodes by setting them 
 *     to the max rate and setting min to 0  so that each node can only schedule upto
 *     the max rate. It also calculates the leaky bucket parameters based on 
 *     the kbit max
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number 
 *     level      - (IN) level of the node in LLS
 *     hw_index   - (IN) h/w index of the node
 *     cookie     - (IN) state of the LLS structure  
 */
STATIC int _bcm_td2_lls_shapers_remove(int unit, int port, int level, 
                                       int hw_index, void *cookie)
{
    _bcm_td2p_lls_info_t *i_lls_tree = (_bcm_td2p_lls_info_t *) cookie;
    soc_mem_t config_mem;
    uint32  *p_entry;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, kbit_max, kbit_min;
    uint32 meter_flags;
    uint32 bucketsize_max, granularity_max, refresh_rate_max,
           refresh_bitsize = 0, bucket_bitsize = 0;

    static const soc_mem_t memx[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_0m,
        MMU_MTRO_L1_MEM_0m,
        MMU_MTRO_L2_MEM_0m
    };

    static const soc_mem_t memy[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_1m,
        MMU_MTRO_L1_MEM_1m,
        MMU_MTRO_L2_MEM_1m
    };
 
    if (level == SOC_TD2_NODE_LVL_MAX) {
        return SOC_E_PARAM;
    }

    if (i_lls_tree->child_lvl != level) {
        return BCM_E_NONE;
    }

    config_mem = SOC_TD2_PMEM(unit, port, memx[level], memy[level]);

    if (config_mem ==  INVALIDm) {
        return BCM_E_PARAM;
    }

    index = i_lls_tree->offset[level] + i_lls_tree->count[level];
    p_entry = &i_lls_tree->mtro_entries[index * SOC_MAX_MEM_WORDS];
    
    if (*p_entry == 0) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, hw_index,
                                         p_entry));
        i_lls_tree->mtro_restore = TRUE;
    }

    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    
            
    meter_flags = 0;

    refresh_bitsize = soc_mem_field_length(unit, config_mem, MAX_REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, config_mem, MAX_THD_SELf);

    /* Setting the max metering configuration */
    kbit_max = i_lls_tree->kbit_max;
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, kbit_max, kbit_max,
                                       meter_flags, refresh_bitsize,
                                       bucket_bitsize, &refresh_rate_max,
                                       &bucketsize_max, &granularity_max));

    soc_mem_field32_set(unit, config_mem, entry, MAX_METER_GRANf,
                        granularity_max);
    soc_mem_field32_set(unit, config_mem, entry, MAX_REFRESHf,
                        refresh_rate_max);
    soc_mem_field32_set(unit, config_mem, entry, MAX_THD_SELf, 
                        bucketsize_max);

    
    /* Setting the min metering configuration */
    kbit_min = i_lls_tree->kbit_min;
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, kbit_min, kbit_min,
                                       meter_flags, refresh_bitsize,
                                       bucket_bitsize, &refresh_rate_max,
                                       &bucketsize_max, &granularity_max));

    soc_mem_field32_set(unit, config_mem, entry, MIN_METER_GRANf,
                        granularity_max);
    soc_mem_field32_set(unit, config_mem, entry, MIN_REFRESHf,
                        refresh_rate_max);
    soc_mem_field32_set(unit, config_mem, entry, MIN_THD_SELf, 
                        bucketsize_max);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, hw_index, entry));
  
    sal_usleep(100); 
    i_lls_tree->count[level] += 1;
    return 0;
}

/*
 * Function:
 *     _bcm_td2_lls_shapers_restore
 * Purpose:
 *     This function restores the shapers on the nodes  
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number 
 *     level      - (IN) level of the node in LLS
 *     hw_index   - (IN) h/w index of the node
 *     cookie     - (IN) state of the LLS structure  
 */
STATIC int _bcm_td2_lls_shapers_restore(int unit, int port, int level, 
                                        int hw_index, void *cookie)
{
    _bcm_td2p_lls_info_t *i_lls_tree = (_bcm_td2p_lls_info_t *) cookie;
    soc_mem_t config_mem;
    uint32  *p_entry;
    int index;
    uint32 entry[SOC_MAX_MEM_WORDS];    

    static const soc_mem_t memx[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_0m,
        MMU_MTRO_L1_MEM_0m,
        MMU_MTRO_L2_MEM_0m
    };

     static const soc_mem_t memy[] = {
        INVALIDm,
        MMU_MTRO_L0_MEM_1m,
        MMU_MTRO_L1_MEM_1m,
        MMU_MTRO_L2_MEM_1m
    };

    if (level == SOC_TD2_NODE_LVL_MAX) {
        return SOC_E_PARAM;
    }

    config_mem = SOC_TD2_PMEM(unit, port, memx[level], memy[level]);

    if (i_lls_tree->child_lvl  != level) {
        return BCM_E_NONE;
    }

    if (config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }


    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    soc_mem_field32_set(unit, config_mem, entry, MIN_REFRESHf,
                        500);
    soc_mem_field32_set(unit, config_mem, entry, MIN_THD_SELf,
                        3);
 
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, hw_index, entry));

    sal_usleep(100);

    index = i_lls_tree->offset[level] + i_lls_tree->count[level];
    if (i_lls_tree->mtro_entries != NULL) {
        p_entry = &i_lls_tree->mtro_entries[index * SOC_MAX_MEM_WORDS];

        if (i_lls_tree->mtro_restore == TRUE) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                                              hw_index, p_entry));
        }
    }
    i_lls_tree->count[level] += 1;

    return 0;
}

/*
 * Function:
 *     _bcm_td2_num_ones
 * Purpose:
 *     This function calculates the numbers of 1s in the binary format
 *     of the number 
 * Parameters:
 *     m       - (IN) number
 * Returns:
 *     The number of 1s in the binary format of the number
 */
STATIC uint32 
_bcm_td2_num_ones(uint32 m)
{
    uint32 i = 0;

    while (m) {
        m &= m - 1;
        i +=  1;
    }
    return i;
}

/*
 * Function:
 *     _bcm_td2plus_compute_lls_config
 * Purpose:
 *     This function computes the LLS configuration at each level
 * Parameters:
 *     unit              - (IN) unit number
 *     port              - (IN) local port number 
 *     level             - (IN) level of the node in LLS
 *     first_sp_child    - (OUT) first sp child 
 *     first_sp_mc_child - (OUT) first so multicast child
 *     ucmap             - (OUT) unicast node map
 *     spmap             - (OUT) SPRI node map
 *     child_index       - (IN) index of the child node
 *     cur_mode          - (IN) current schedling mode of the node  
 *     mode              - (IN) new scheduling mode of the node
 *     max_children      - (IN) maximum number of children in the level
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2plus_compute_lls_config (int unit, int port, int level, 
                                 int *first_sp_child, 
                                 int *first_sp_mc_child,
                                 int *num_spri,
                                 uint32 *ucmap, 
                                 uint32 *spmap, 
                                 int child_index, 
                                 soc_td2_sched_mode_e cur_mode,
                                 soc_td2_sched_mode_e mode,
                                 int max_children)
{
    uint32 ucm = 0, mcm = 0, cur_spmap, cur_num_spri, lucmap = 0;
    int ii, *pfc, uc, mc, fc, oc, *use_fc;

    cur_num_spri = _bcm_td2_num_ones(*spmap);
    cur_spmap = *spmap;

    if (level == SOC_TD2_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            use_fc = first_sp_mc_child;
        } else {
            use_fc = (child_index >= 1480) ? first_sp_mc_child : first_sp_child;
        }
    } else {
        use_fc = first_sp_child;
    }
    
    if (cur_num_spri == 0) {
        *ucmap = 0;
        *spmap = 0;
    }

    if (mode == SOC_TD2_SCHED_MODE_STRICT) {
        if (child_index > *use_fc) {
            if ((child_index - *use_fc) >= max_children && cur_num_spri > 0) {
                 return BCM_E_UNAVAIL;
            }
        } else if (child_index < *use_fc) {
            return BCM_E_UNAVAIL;
        }
    }
 
    if (level == SOC_TD2_NODE_LVL_L1) {
        for (mcm = 0, mc = 0, ucm = 0, uc = 0, ii = 0; ii < 8; ii++) {
            if (*ucmap & (1 << ii)) {
                mcm |= (cur_spmap & (1 << ii)) ? (1 << mc) : 0;
                mc += 1;
            } else {
                ucm |= (cur_spmap & (1 << ii)) ? (1 << uc) : 0;
                uc += 1;
            }
        }

        if (child_index < 1480) {
            fc = *first_sp_child;
            oc =  _bcm_td2_num_ones(ucm);
            BCM_IF_ERROR_RETURN(_bcm_td2plus_compute_lls_config(unit, port,
                        level - 1, &fc, NULL, &oc, &lucmap, &ucm, child_index, 
                        cur_mode, mode, 8));
        } else {
            fc = *first_sp_mc_child;
            oc =  _bcm_td2_num_ones(mcm);
            BCM_IF_ERROR_RETURN(_bcm_td2plus_compute_lls_config(unit, port,
                    level - 1, &fc, NULL, &oc, &lucmap, &mcm, 
                    child_index, cur_mode, mode, 8));
        }
        
        cur_spmap = 0;
        *ucmap = 0;
        for (fc = 0, ii = 0; ii < 8; ii++) {
            if (ucm) {
                cur_spmap |= (ucm & (1 << ii)) ? 1 << fc : 0;
                fc += 1;
                ucm &= ~(1 << ii);
            }
            
            if (mcm) {
                cur_spmap |= (mcm & (1 << ii)) ? 1 << fc : 0;
                *ucmap |= 1 << fc;
                fc += 1;
                mcm &= ~(1 << ii);
            }

            if (fc > max_children) {
                return BCM_E_PARAM;
            }

            if ((mcm == 0) && (ucm == 0)) {
                break;
            }
        }

    } else {
        pfc = first_sp_child;
        if (mode == SOC_TD2_SCHED_MODE_STRICT) {
            if (*pfc > child_index) {
                cur_spmap <<= (*pfc - child_index);
                cur_spmap |= 1;
                *pfc = child_index;
                *num_spri += 1;
            } else {
                if (((1 << (child_index - *pfc)) & cur_spmap) == 0) {
                    cur_spmap |= 1 << (child_index - *pfc);
                    *num_spri += 1;
                }
            }
        } else {
            if (1 << (child_index - *pfc) & cur_spmap) {
                cur_spmap &= ~(1 << (child_index - *pfc));
                *num_spri -= 1;
            }
        }
    }

    *num_spri = _bcm_td2_num_ones(cur_spmap);

    *spmap = cur_spmap;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_compute_lls_config
 * Purpose:
 *     This function computes the LLS configuration at each level
 * Parameters:
 *     unit              - (IN) unit number
 *     port              - (IN) local port number 
 *     level             - (IN) level of the node in LLS
 *     first_sp_child    - (OUT) first sp child 
 *     first_sp_mc_child - (OUT) first so multicast child
 *     ucmap             - (OUT) unicast node map
 *     spmap             - (OUT) SPRI node map
 *     child_index       - (IN) index of the child node
 *     cur_mode          - (IN) current schedling mode of the node  
 *     mode              - (IN) new scheduling mode of the node
 *     max_children      - (IN) maximum number of children in the level
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_compute_lls_config(int unit, int port, int level, 
                            int *first_sp_child, 
                            int *first_sp_mc_child,
                            int *num_spri,
                            uint32 *ucmap, 
                            uint32 *spmap, 
                            int child_index, 
                            soc_td2_sched_mode_e cur_mode,
                            soc_td2_sched_mode_e mode)
{
    if (!soc_feature(unit, soc_feature_vector_based_spri)) {
        return BCM_E_UNAVAIL;
    }

    if (((cur_mode != SOC_TD2_SCHED_MODE_STRICT) &&
        (mode != SOC_TD2_SCHED_MODE_STRICT)) ||
       ((cur_mode == SOC_TD2_SCHED_MODE_STRICT) && 
        (mode == SOC_TD2_SCHED_MODE_STRICT))) {
        return BCM_E_NONE;
    }

    return _bcm_td2plus_compute_lls_config(unit, port, level, first_sp_child,
                                          first_sp_mc_child, num_spri,
                                          ucmap, spmap, child_index,
                                          cur_mode, mode, 8);
}

STATIC int
_bcm_td2_cosq_traverse_lls_tree(int unit, int port, _bcm_td2_cosq_node_t *node,
                                _soc_td2_lls_traverse_cb cb, void *cookie)
{
    if (node->child) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_traverse_lls_tree(unit, port, 
                                                            node->child,
                                                            cb, cookie));
    }

    if (node->sibling) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_traverse_lls_tree(unit, port, 
                                                node->sibling, cb, cookie));
    }

    if (node->level != SOC_TD2_NODE_LVL_ROOT) {
        BCM_IF_ERROR_RETURN(cb(unit, port, node->level, node->hw_index, cookie));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_adjust_lls_bw
 * Purpose:
 *     This function adjusts the bandwidth at the level in which the node is
 *     being switched so that each node gets sufficient bandwidth to do the
 *     switchover.
 * Parameters:
 *     unit              - (IN) unit number
 *     port              - (IN) local port number 
 *     node              - (IN) cosq node structure
 *     child_lvl         - (IN) level of the child node
 *     child_hw_index    - (IN) h/w index of the child node
 *     start             - (IN) flag  
 *     i_lls_tree        - (OUT) LLS info 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_adjust_lls_bw(int unit, bcm_port_t port, _bcm_td2_cosq_node_t *node, 
                       int child_lvl, int child_hw_index, int start, 
                       _bcm_td2p_lls_info_t *i_lls_tree)
{
    int rv = BCM_E_NONE, count, mem_size, ii, speed, jj;
    soc_info_t *si;
    
    if (!soc_feature(unit, soc_feature_vector_based_spri)) {
        return BCM_E_NONE;
    }
    
    if (start) {
        
        sal_memset(i_lls_tree, 0, sizeof(_bcm_td2p_lls_info_t));
        i_lls_tree->child_lvl = child_lvl;
        i_lls_tree->child_hw_index = child_hw_index;

        if (node) {

            /* get to top node */
            while(node->parent) { node = node->parent; }

            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_traverse_lls_tree(unit, port,
                                node,_bcm_td2_lls_node_count, i_lls_tree));
        } else {
            BCM_IF_ERROR_RETURN(soc_td2_port_lls_traverse(unit, port,
                                      _bcm_td2_lls_node_count, i_lls_tree));
        }          

        for (count = 0, ii = 0; ii < SOC_TD2_NODE_LVL_MAX; ii++) {
            count += i_lls_tree->node_count[ii];
            for (jj = ii - 1; jj >= 0; jj--) {
                i_lls_tree->offset[ii] += i_lls_tree->node_count[jj];
            }
        }
        
        if (count == 0) {
            return BCM_E_INIT;
        }
        
        mem_size = sizeof(uint32)*SOC_MAX_MEM_WORDS*count;
        i_lls_tree->mtro_entries = sal_alloc(mem_size, "lls_war_buf");
        
        if (port != CMIC_PORT(unit)) {
            rv = soc_esw_portctrl_speed_get(unit, port, &speed);
            if (rv == BCM_E_UNAVAIL) {
                si = &SOC_INFO(unit);
                speed = si->port_speed_max[port];
            }
        } else {
            speed = 10000;
        }
        i_lls_tree->kbit_min = 0;
        i_lls_tree->kbit_max = ((speed)/10)/i_lls_tree->node_count[child_lvl];

        for (count = 0, ii = 0; ii < SOC_TD2_NODE_LVL_MAX; ii++) {
            i_lls_tree->count[ii] = 0;
        }
 
        sal_memset(i_lls_tree->mtro_entries, 0, mem_size);
        i_lls_tree->mtro_restore = FALSE;

        if (node) {

            /* get to top node */
            while(node->parent) { node = node->parent; }

            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_traverse_lls_tree(unit, port, node,
                                    _bcm_td2_lls_shapers_remove, i_lls_tree));
        } else {
            BCM_IF_ERROR_RETURN(soc_td2_port_lls_traverse(unit, port,
                                    _bcm_td2_lls_shapers_remove, i_lls_tree));
        }
    } else {
        for (count = 0, ii = 0; ii < SOC_TD2_NODE_LVL_MAX; ii++) {
            i_lls_tree->count[ii] = 0;
        }

        if (node) {
            /* get to top node */
            while(node->parent) { node = node->parent; }

            rv = _bcm_td2_cosq_traverse_lls_tree(unit, port, node,
                                    _bcm_td2_lls_shapers_restore, i_lls_tree);
        } else {
            rv = soc_td2_port_lls_traverse(unit, port,
                                    _bcm_td2_lls_shapers_restore, i_lls_tree);
        }
       
       if (i_lls_tree->mtro_entries) {
            sal_free(i_lls_tree->mtro_entries);
            i_lls_tree->mtro_entries = NULL;
       }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_td2_port_enqueue_set
 * Purpose:
 *     Enable or Disable the enqueing the packets to the port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port 
 *     enable     - (IN) TRUE to enable
 *                      FALSE to disable
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_td2_port_enqueue_set(int unit, bcm_port_t gport, int enable)
{
    bcm_port_t local_port;
   
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    SOC_IF_ERROR_RETURN(
        soc_td2_port_mmu_tx_enable_set(unit, local_port, enable));
    SOC_IF_ERROR_RETURN(
        soc_td2_mmu_rx_enable_set(unit, local_port, enable));

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_td2_port_enqueue_get
 * Purpose:
 *      Getting the enqueing state of the port 
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port 
 *     enable     - (IN) TRUE to enable
 *                      FALSE to disable
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_td2_port_enqueue_get(int unit, bcm_port_t gport,
                          int *enable)
{
    uint64 rval64;
    int mmu_port, phy_port, i;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    soc_info_t *si;
    soc_reg_t reg_tmp;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE0_64r,
            THDU_OUTPUT_PORT_RX_ENABLE1_64r
        },
        {
            MMU_THDM_DB_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_DB_PORTSP_RX_ENABLE1_64r
        },
        {
            MMU_THDM_MCQE_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_MCQE_PORTSP_RX_ENABLE1_64r
        }
    };
    int max_reg = 3;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    COMPILER_64_ZERO(rval64);

    /* Disable enqueue to the port */
    for (i = 0; i < max_reg; i++) {
        reg_tmp = SOC_TD2_PREG(unit, local_port, reg[i][0], reg[i][1]);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg_tmp, REG_PORT_ANY, 0, &rval64));

        mmu_port &= 0x3f;
        if (COMPILER_64_BITTEST(rval64 , mmu_port)) {
            *enable = TRUE;
        } else {
            *enable = FALSE;
        }
    }
    return rv;
}

/*
 * Function:
 *     bcm_td2_lb_lls_init
 * Purpose:
 *     Construct cosq hierarchy for Loopback port in ETS mode
 * Parameters:
 *     unit  - (IN) unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcm_td2_lb_lls_init(int unit)
{
    soc_port_t local_port;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node = NULL;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_pipe_resources_t *res;
    bcm_gport_t gport, gport_l0, gport_l1[9], gport_l2[9];
    int phy_port, mmu_port;
    uint32 sched_encap;
    int id, pipe, cos;
    int l1_node_cnt = 9;
    int l2_node_cnt = 9;    /* 8 MCQs + 1 QCN queue */

    mmu_info = _bcm_td2_mmu_info[unit];
    if(mmu_info == NULL) {
        return BCM_E_INIT;
    }

    /* coverity[illegal_address: FALSE] */
    SOC_PBMP_ITER(PBMP_LB(unit), local_port) {
        if (local_port >= _BCM_TD2_MAX_PORT) {
            return BCM_E_INTERNAL;
        }
        port_info = &mmu_info->port_info[local_port];
        pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
        res = _BCM_TD2_PRESOURCES(mmu_info, pipe);

        /* Root node*/
        node = &mmu_info->sched_node[local_port];
        sched_encap = (local_port << 8) | local_port;
        BCM_GPORT_SCHEDULER_SET(gport, sched_encap);
        node->gport = gport;
        node->level = SOC_TD2_NODE_LVL_ROOT;
        node->type = _BCM_TD2_NODE_SCHEDULER;
        phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        node->hw_index = mmu_port % 64;
        node->numq = 1;
        node->local_port = local_port;
        node->in_use = TRUE;
        node->attached_to_input = 0;

        /* L0.0 node */
        node = NULL;
        for (id = _BCM_TD2_NUM_PORT_SCHEDULERS;
             id < _BCM_TD2_NUM_TOTAL_SCHEDULERS; id++) {
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
        sched_encap = (id << 8) | local_port;
        BCM_GPORT_SCHEDULER_SET(gport_l0, sched_encap);
        node->gport = gport_l0;
        node->numq = l1_node_cnt;
        node->local_port = local_port;
        node->type = _BCM_TD2_NODE_SCHEDULER;
        node->attached_to_input = -1;

        /* Attach to Root node */
        BCM_IF_ERROR_RETURN
            (bcm_td2_cosq_gport_attach(unit, gport_l0, gport, 0));

        /* L1.0 ~ L1.8 node */
        for (cos = 0; cos < l1_node_cnt; cos++) {
            node = NULL;
            for (id = _BCM_TD2_NUM_PORT_SCHEDULERS;
                 id < _BCM_TD2_NUM_TOTAL_SCHEDULERS; id++) {
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
            sched_encap = (id << 8) | local_port;
            BCM_GPORT_SCHEDULER_SET(gport_l1[cos], sched_encap);
            node->gport = gport_l1[cos];
            node->numq = 1;
            node->local_port = local_port;
            node->type = _BCM_TD2_NODE_SCHEDULER;
            node->attached_to_input = -1;

            /* Attach to L0 node */
            BCM_IF_ERROR_RETURN
                (bcm_td2_cosq_gport_attach(unit, gport_l1[cos], gport_l0, cos));
        }

        /* L2 mc queue & QCN queue */
        for (cos = 0; cos < l2_node_cnt; cos++) {
            for (id = port_info->mc_base; id < port_info->mc_limit; id++) {
                if (res->p_mc_queue_node[id].numq == 0) {
                    break;
                }
            }

            if (id == port_info->mc_limit) {
                return BCM_E_RESOURCE;
            }

            /* set index bits */
            BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(gport_l2[cos], local_port, id);
            node = &res->p_mc_queue_node[id];
            node->gport = gport_l2[cos];
            node->numq = 1;
            node->level = SOC_TD2_NODE_LVL_L2;
            node->type = _BCM_TD2_NODE_MCAST;
            node->hw_cosq = id - port_info->mc_base;
            node->hw_index = soc_td2_l2_hw_index(unit, id, 0);
            node->local_port = local_port;
            node->remote_modid = -1;
            node->remote_port  = -1;
            node->in_use = TRUE;

            /* Attach to L1 node */
            BCM_IF_ERROR_RETURN
                (bcm_td2_cosq_gport_attach(unit, gport_l2[cos],
                                           gport_l1[cos], 0));
         }

    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_td2_cosq_hw_idx_get
 * Purpose:
 *     Get cosq hardware index
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_td2_cosq_hw_idx_get(int unit, int max_count, bcm_gport_t *port, int *hw_idx, int* count)
{
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t *res;
    int pipe = 0, qmax, qmin, id, i = 0;
    if (port == NULL || hw_idx == NULL || count == NULL) {
        return BCM_E_PARAM;
    }

    mmu_info = _bcm_td2_mmu_info[unit];
    for (pipe = 0; pipe < 2; pipe++) {
        res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
        qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        qmin = res->num_base_queues +
                       (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
        for (id = qmin; id < qmax; id++) {
            if (res->p_queue_node[id].numq == 0 ||
                res->p_queue_node[id].level != SOC_TD2_NODE_LVL_L2) {
                continue;
            }
            if (i >= max_count) {
                return BCM_E_CONFIG;
            }
            /*for Y pipe, uc queue starts at hw_idx 2048*/
            hw_idx[i] = res->p_queue_node[id].hw_index + pipe * 2048;
            port[i] = res->p_queue_node[id].local_port;
            i++;
        }
    }
    *count = i;
    return BCM_E_NONE;
 }

/*
 * Function:
 *     bcm_td2_cosq_gport_add
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
bcm_td2_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node = NULL;
    _bcm_td2_cosq_port_info_t *port_info;
    uint32 sched_encap;
    _bcm_td2_pipe_resources_t *res;
    int phy_port, mmu_port, local_port;
    int id, ii, q_base, pipe, qmin, qmax;
    bcm_gport_t tmp_gport;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td2_cosq_gport_add: unit=%d port=0x%x numq=%d flags=0x%x\n"),
              unit, port, numq, flags));
    
    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_td2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* Check Alignment request is not for unsupported type */
    if (flags & BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN) {
        if (!(flags == (BCM_COSQ_GPORT_SCHEDULER |
                        BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN))) {
            return BCM_E_PARAM;
        }
    }

    mmu_info = _bcm_td2_mmu_info[unit];

    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);

    if (!mmu_info->ets_mode) {

        /* cleanup the default lls setup */
        SOC_IF_ERROR_RETURN(soc_td2_lls_reset(unit));

        mmu_info->ets_mode = 1;

        BCM_IF_ERROR_RETURN(bcm_td2_lb_lls_init(unit));
    }

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (IS_CPU_PORT(unit, local_port)) {
            return BCM_E_PARAM;
        }

        for (id = port_info->uc_base; id < port_info->uc_limit; id++) {
            if (res->p_queue_node[id].numq == 0) {
                break;
            }
        }

        qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        if (id == port_info->uc_limit) {
            if (IS_TD2_HSP_PORT(unit, local_port)) {
                return BCM_E_RESOURCE;
            }
            qmin = res->num_base_queues +
                   (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
            for (id = qmin; id < qmax; id++) {
                if (res->p_queue_node[id].numq == 0) {
                    break;
                }
            }
        }

        if (id == qmax) {
            return BCM_E_RESOURCE;
        }

        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &res->p_queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->level = SOC_TD2_NODE_LVL_L2;
        node->hw_cosq = id - port_info->uc_base;
        node->hw_index = soc_td2_l2_hw_index(unit, id, 1);
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->type = _BCM_TD2_NODE_UCAST;
        node->in_use = TRUE;
        break;

    case BCM_COSQ_GPORT_SUBSCRIBER:
    case BCM_COSQ_GPORT_VIRTUAL_PORT:
    case BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP:
        if (IS_TD2_HSP_PORT(unit, local_port)) {
            return BCM_E_PARAM;
        }
        /* Service Queueing are outside default queues. Hence NO alignment
         * required.
         */
        BCM_IF_ERROR_RETURN(
            _bcm_td2_node_index_get(res->ext_qlist.bits, res->num_base_queues,
                                     _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE, numq, 
                                     1, &q_base));

        _bcm_td2_node_index_set(&res->ext_qlist, q_base, numq);
        qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
        qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;

        for (ii = 0, id = qmin; id < qmax; id++) {
            if (res->p_queue_node[id].numq > 0) {
                ii = 0;
            } else {
                ii += 1;

                if (ii == numq) {
                    break;
                }
            }
        }

        if (ii != numq) {
            return BCM_E_RESOURCE;
        }

        id = id - (numq - 1);

        for (ii = 0; ii < numq; ii++, id++) {
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(tmp_gport, local_port, id);
            node= &res->p_queue_node[id];
            node->gport = tmp_gport;
            node->numq = numq - ii;
            node->level = SOC_TD2_NODE_LVL_L2;
            node->type = (flags == BCM_COSQ_GPORT_VIRTUAL_PORT) ?
                            _BCM_TD2_NODE_VM_UCAST : _BCM_TD2_NODE_SERVICE_UCAST;
            node->hw_index = q_base + ii;
            /* by default assign the cosq on order of order of alocation,
             * can be overriddedn by cos_mapping_set */
            node->hw_cosq = ii;
            node->local_port = local_port;
            node->remote_modid = -1;
            node->remote_port  = -1;
            node->in_use = TRUE;

            if (ii == 0) {
                *gport = tmp_gport;
            }
        }
        break;

    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (!IS_HG_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }

    qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
    qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        for (id = qmin; id < qmax; id++) {
            if (res->p_queue_node[id].numq == 0) {
                break;
            }
        }

        if (id >= qmax) {
            return BCM_E_RESOURCE;
        }

        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &res->p_queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->level = SOC_TD2_NODE_LVL_L2;
        node->type = _BCM_TD2_NODE_VOQ;
        node->hw_index = -1;
        /* by default assign the cosq on order of order of alocation,
         * can be overriddedn by cos_mapping_set */
        node->hw_cosq = -1;
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->in_use = TRUE;
        break;

    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        for (id = port_info->mc_base; id < port_info->mc_limit; id++) {
            if (res->p_mc_queue_node[id].numq == 0) {
                break;
            }
        }
        
        if (id == port_info->mc_limit) {
            return BCM_E_RESOURCE;
        }            

        /* set index bits */
        BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &res->p_mc_queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->level = SOC_TD2_NODE_LVL_L2;
        node->type = _BCM_TD2_NODE_MCAST;
        node->hw_cosq = id - port_info->mc_base;
        node->hw_index = soc_td2_l2_hw_index(unit, id, 0);
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->in_use = TRUE;
        break;

    case BCM_COSQ_GPORT_SCHEDULER:
    case BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN:
        if (numq < -1) {
            return BCM_E_PARAM;
        }
        for (id = _BCM_TD2_NUM_PORT_SCHEDULERS;
             id < _BCM_TD2_NUM_TOTAL_SCHEDULERS; id++) {
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
        sched_encap = (id << 8) | local_port;
        BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
        node->gport = *gport;
        node->numq = numq;
        node->local_port = local_port;
        node->type = _BCM_TD2_NODE_SCHEDULER;
        node->attached_to_input = -1;
        if (flags & BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN) {
            node->node_align_value = _BCM_TD2_START_INDEX_ALIGN_PFC;
        } else {
            node->node_align_value = _BCM_TD2_START_INDEX_ALIGN_DEFAULT;
        }
        break;

    case BCM_COSQ_GPORT_WITH_ID:
    case 0:
        if (numq < -1) {
            return BCM_E_PARAM;
        }

        /* this is a port level scheduler */
        id = local_port;

        if ( id < 0 || id >= _BCM_TD2_NUM_PORT_SCHEDULERS) {
            return BCM_E_PARAM;
        }

        if (numq >= _BCM_TD2_NUM_L0_SCHEDULER_PER_PIPE) {
            return BCM_E_RESOURCE;
        }

        node = &mmu_info->sched_node[id];
        sched_encap = (id << 8) | local_port;
        BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
        node->gport = *gport;
        node->level = SOC_TD2_NODE_LVL_ROOT;
        node->type = _BCM_TD2_NODE_SCHEDULER;
        phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        node->hw_index = mmu_port % 64;

        if (IS_TD2_HSP_PORT(unit, local_port)) {
            /* free any schedulers in use by legacy setup */
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_set_scheduler_states(unit, local_port, 0));
            BCM_IF_ERROR_RETURN(soc_td2_lls_port_uninit(unit, local_port));
        }

        if (IS_TD2_HSP_PORT(unit, local_port)) {
            node->numq = _BCM_TD2_HSP_PORT_MAX_L0;
        } else {
            node->numq = numq;
        }
        node->local_port = local_port;
        node->in_use = TRUE;
        node->attached_to_input = 0;
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
 *     bcm_td2_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to detach from
 *     cosq       - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_td2_cosq_node_t *sched_node, *input_node = NULL, *prev_node;
    bcm_port_t sched_port, input_port = -1;
    _bcm_td2_cosq_node_t *parent;
    _bcm_td2_cosq_list_t *list;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t * res;
    _bcm_td2_cosq_port_info_t *port_info;
    int start = 0, end = 0, q_base, hw_index;

    if (_bcm_td2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport)) ||
        (BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, sched_gport, 0, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->attached_to_input < 0) {
        /* Not attached yet */
        return BCM_E_PORT;
    }

    if (input_gport != BCM_GPORT_INVALID) {
        if (BCM_GPORT_IS_SCHEDULER(input_gport) || 
            BCM_GPORT_IS_MODPORT(input_gport)   || 
            BCM_GPORT_IS_LOCAL(input_gport)) {
               BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, input_gport, 0, NULL, 
                                    &input_port, NULL, &input_node));
        } else {
            if (!(BCM_GPORT_IS_SCHEDULER(sched_gport) || 
                  BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport))) {
                return BCM_E_PARAM;
            }
            else {
                BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, input_gport,
                                                &input_port));
                input_node = NULL;
            }
        }
    }

    if (input_port == -1) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[input_port];
    res = port_info->resources;

    if (sched_port != input_port) {
        return BCM_E_PORT;
    }

    if (sched_node->parent != input_node) {
        return BCM_E_PORT;
    }

    if ((cosq < -1) || 
        (input_node && (input_node->numq != -1) && (cosq >= input_node->numq))) {
        return BCM_E_PARAM;
    }

     if (cosq != -1) {
        if (sched_node->attached_to_input != cosq) {
            return BCM_E_PARAM;
        }
    }

    if (sched_node->type == _BCM_TD2_NODE_SERVICE_UCAST) {
        end = sched_node->numq;
    } else {
        end = 1;
    }

    for (start = 0; start < end; start++) {
        /* unresolve the node - delete this node from parent's child list */
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_unresolve(unit, sched_node, cosq));
        
        BCM_IF_ERROR_RETURN(
            soc_td2_cosq_set_sched_parent(unit, input_port, 
                        sched_node->level, sched_node->hw_index, 
                        _soc_td2_invalid_parent_index(unit, sched_node->level)));

        /* now remove from the sw tree */
        if (sched_node->parent != NULL) {
            parent = sched_node->parent;
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
            sched_node->attached_to_input = -1;
            if (0 == start) {
                hw_index = sched_node->hw_index;
                q_base = hw_index;
                if ((sched_node->type == _BCM_TD2_NODE_VM_UCAST) || 
                    (sched_node->type == _BCM_TD2_NODE_SERVICE_UCAST)) { 
                        BCM_IF_ERROR_RETURN(
                            _bcm_td2_node_index_clear(&res->ext_qlist, 
                                                      q_base, sched_node->numq)); 
                }
            }

            if (parent->child == NULL) {
                list = NULL;
                switch (parent->level) {
                    case SOC_TD2_NODE_LVL_ROOT:
                        list = &res->l0_sched_list;
                        sched_node->hw_index = -1;
                        break;
                    case SOC_TD2_NODE_LVL_L0:
                        list = &res->l1_sched_list;
                        sched_node->hw_index = -1;
                        break;
                    case SOC_TD2_NODE_LVL_L1:
                        if (sched_node->type == _BCM_TD2_NODE_VOQ) {
                            list = &res->ext_qlist;
                        }
                        break;
                    default:
                        break;
                }

                if (list) {
                    BCM_IF_ERROR_RETURN(_bcm_td2_node_index_clear(list, 
                                parent->base_index, parent->base_size));
                    parent->base_index = -1;
                    parent->base_size  = 0;
                }
            }
        }
        if ((sched_node->type == _BCM_TD2_NODE_SERVICE_UCAST) && 
            ((start + 1) < end)){
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, sched_gport, start + 1, NULL,
                                        &sched_port, NULL, &sched_node));
            if (!sched_node) {
                return BCM_E_NOT_FOUND;
            }
        }
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                         hw_cosq=%d\n"),
              sched_node->attached_to_input));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_gport_delete_all(int unit, bcm_gport_t gport)
{
    _bcm_td2_cosq_node_t *node, *tnode, *base_node;
    int ii, local_port, phy_port, mmu_port, hw_index;
    _bcm_td2_mmu_info_t *mmu_info;
    soc_info_t *si;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td2_cosq_gport_delete_all: unit=%d gport=0x%x\n"),
              unit, gport));

    node = NULL;
    base_node= NULL;

    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));
    } else {
        local_port = (BCM_GPORT_IS_LOCAL(gport)) ?
          BCM_GPORT_LOCAL_GET(gport) : BCM_GPORT_MODPORT_PORT_GET(gport);

        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }
       
        mmu_info = _bcm_td2_mmu_info[unit];
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];
        hw_index = _BCM_TD2_ROOT_SCHED_INDEX(unit, mmu_port);

        for (ii = 0; ii < _BCM_TD2_NUM_TOTAL_SCHEDULERS; ii++) {
            tnode = &mmu_info->sched_node[ii];
            if (tnode->in_use == FALSE) {
                continue;
            }
            if ((tnode->level == SOC_TD2_NODE_LVL_ROOT) &&
                (tnode->hw_index == hw_index) && 
                (tnode->local_port == local_port)) {   
                /* check both mmu_port(unique only within a pipe)
                   and local_port(unique witin system) to get the 
                   correct node */
                node = tnode;
                break;
            }
        }
        if (node == NULL) {
            return BCM_E_NONE;
        }
    }

    if (node->child != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_gport_delete_all(unit, node->child->gport));
    }

    if (node->sibling != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_gport_delete_all(unit, node->sibling->gport));
    }

    if (node->level != SOC_TD2_NODE_LVL_ROOT && node->attached_to_input >= 0) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_cosq_gport_detach(unit, node->gport, 
                                node->parent->gport, node->attached_to_input));
    }
    if ((node->type == _BCM_TD2_NODE_VOQ) && 
            (node->remote_modid > 0)) {
        _bcm_td2_voq_next_base_node_get (unit, local_port,
                node->remote_modid, &base_node);
        if (base_node != NULL) {
            BCM_PBMP_ASSIGN(base_node->fabric_pbmp,
                    node->fabric_pbmp);
        }
    }

    _BCM_TD2_COSQ_NODE_INIT(node);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_td2_cosq_node_t *node, *tnode, *base_node;
    int ii, local_port, phy_port, mmu_port, hw_index;
    _bcm_td2_mmu_info_t *mmu_info;
    soc_info_t *si;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td2_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    node = NULL;
    base_node= NULL;

    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));
    } else {
        local_port = (BCM_GPORT_IS_LOCAL(gport)) ?
          BCM_GPORT_LOCAL_GET(gport) : BCM_GPORT_MODPORT_PORT_GET(gport);

        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }
       
        mmu_info = _bcm_td2_mmu_info[unit];
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];
        hw_index = _BCM_TD2_ROOT_SCHED_INDEX(unit, mmu_port);

        for (ii = 0; ii < _BCM_TD2_NUM_TOTAL_SCHEDULERS; ii++) {
            tnode = &mmu_info->sched_node[ii];
            if (tnode->in_use == FALSE) {
                continue;
            }
            if ((tnode->level == SOC_TD2_NODE_LVL_ROOT) &&
                (tnode->hw_index == hw_index) && 
                (tnode->local_port == local_port)) {   
                /* check both mmu_port(unique only within a pipe)
                   and local_port(unique witin system) to get the 
                   correct node */
                node = tnode;
                break;
            }
        }
        if (node == NULL) {
            return BCM_E_NONE;
        }
    }

    if (node->child != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_gport_delete_all(unit, node->child->gport));
    }

    if (node->level != SOC_TD2_NODE_LVL_ROOT && node->attached_to_input >= 0) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_cosq_gport_detach(unit, node->gport, 
                                node->parent->gport, node->attached_to_input));
    }
    if ((node->type == _BCM_TD2_NODE_VOQ) && 
            (node->remote_modid > 0)) {
        _bcm_td2_voq_next_base_node_get (unit, local_port,
                node->remote_modid, &base_node);
        if (base_node != NULL) {
            BCM_PBMP_ASSIGN(base_node->fabric_pbmp,
                    node->fabric_pbmp);
        }
    }

    _BCM_TD2_COSQ_NODE_INIT(node);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_get
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
bcm_td2_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_td2_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td2_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, &id, &node));

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
        if (node->type == _BCM_TD2_NODE_SERVICE_UCAST) {
            *flags = BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP;
        } else if (node->type == _BCM_TD2_NODE_VM_UCAST) {
            *flags = BCM_COSQ_GPORT_VIRTUAL_PORT;
        } else if (node->type == _BCM_TD2_NODE_VOQ) {
            *flags = BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP;
        } else {
            *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        *flags = BCM_COSQ_GPORT_SCHEDULER;
        if (node->node_align_value == _BCM_TD2_START_INDEX_ALIGN_PFC) {
            *flags |= BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
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
 *     bcm_td2_cosq_gport_traverse
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
bcm_td2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    _bcm_td2_cosq_node_t *port_info;
    _bcm_td2_mmu_info_t *mmu_info;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port, port_out;

    if (_bcm_td2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_td2_mmu_info[unit];

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    my_modid, port, &modid_out, &port_out));

        /* root node */
        port_info = &mmu_info->sched_node[port_out];

        if (port_info->gport >= 0) {
            _bcm_td2_cosq_gport_traverse(unit, port_info->gport, cb, user_data);
        }
      }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_attach
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
bcm_td2_cosq_gport_attach(int unit, bcm_gport_t gport, 
                           bcm_gport_t to_gport, 
                           bcm_cos_queue_t to_cosq)
{
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node, *to_node;
    bcm_port_t port, to_port, local_port, mmu_port;
    int rv, tmp;

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(to_gport)) ||
        (BCM_GPORT_IS_MCAST_QUEUE_GROUP(to_gport)) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(to_gport))) {
        return BCM_E_PARAM;
    }

    if(!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_SCHEDULER(gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &port, NULL, &node));

    if (node->attached_to_input >= 0) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }
    tmp = node->attached_to_input;

    if (BCM_GPORT_IS_SCHEDULER(to_gport)) {

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, to_gport, 0, NULL, &to_port, NULL,
                                   &to_node));

    } else {

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, to_gport, &to_port));

        to_node = &mmu_info->sched_node[to_port];
    }

    if (port != to_port) {
        return BCM_E_PORT;
    }

    mmu_port = SOC_TD2_MMU_PORT(unit, port);
    
    /* Identify the levels of schedulers
     * to_port == phy_port && gport == scheduler => gport = L0
     * to_port == schduler && gport == scheduler => to_port = L0 and port = L1
     * to_port == scheduler && port == queue_group => to_port = L1 and port = L2
     */
    if (to_node != NULL) {
        if (((to_node->level == SOC_TD2_NODE_LVL_ROOT) && (to_cosq >= 265)) ||
             ((to_node->level == SOC_TD2_NODE_LVL_L0) && (to_cosq >= 1024)) ||
             ((to_node->level == SOC_TD2_NODE_LVL_L1) && (to_cosq >= 1480))) {
            return BCM_E_RESOURCE;
        }

        if (!BCM_GPORT_IS_SCHEDULER(to_gport)) {
            if ((to_node->numq == 0) || (to_node->numq_expandable)) {
                local_port = (BCM_GPORT_IS_LOCAL(to_gport)) ?
                              BCM_GPORT_LOCAL_GET(to_gport) :
                              BCM_GPORT_MODPORT_PORT_GET(to_gport);

                if (!SOC_PORT_VALID(unit, local_port)) {
                    return BCM_E_PORT;
                }
                to_node->in_use = TRUE;
                to_node->local_port = port;
                if (IS_TD2_HSP_PORT(unit, local_port)) {
                    to_node->base_index = _BCM_TD2_HSP_L0_INDEX(unit, mmu_port, 0);
                    to_node->numq = _BCM_TD2_HSP_PORT_MAX_L0;
                } else {
                    to_node->gport = to_gport;
                    to_node->hw_index = _BCM_TD2_ROOT_SCHED_INDEX(unit, mmu_port);
                    to_node->level = SOC_TD2_NODE_LVL_ROOT;
                    to_node->attached_to_input = 0;
                    to_node->numq_expandable = 1;
                    if (to_cosq == -1) {
                        to_node->numq += 1;
                    } else {
                        if (to_cosq >= to_node->numq) {
                            to_node->numq = to_cosq + 1;
                        }
                    }
                }
            }

            if (!BCM_GPORT_IS_SCHEDULER(gport)) {

                return BCM_E_PARAM;
            }

            node->level = SOC_TD2_NODE_LVL_L0;
        } else {
             if (to_node->level == SOC_TD2_NODE_LVL_ROOT) {
                 node->level = SOC_TD2_NODE_LVL_L0;
             }

             if ((to_node->level == -1)) {
                 to_node->level = (BCM_GPORT_IS_SCHEDULER(gport)) ?
                     SOC_TD2_NODE_LVL_L0 : SOC_TD2_NODE_LVL_L1;
             }

             if (node->level == -1) {
                 node->level = 
                     (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                      BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                      BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ?
                     SOC_TD2_NODE_LVL_L2 : SOC_TD2_NODE_LVL_L1;
             }
        }
    } else {
        return BCM_E_NOT_FOUND;
    }

    if ((to_cosq < -1) || ((to_node->numq != -1) && (to_cosq >= to_node->numq))) {

        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(to_gport) || BCM_GPORT_IS_LOCAL(to_gport) ||
        BCM_GPORT_IS_MODPORT(to_gport)) {
        int start = 0, end = 0;
        if (to_node->attached_to_input < 0) {
            /* dont allow to attach to a node that has already attached */

            return BCM_E_PARAM;
        }

        if (node->type == _BCM_TD2_NODE_SERVICE_UCAST) {
            end = node->numq;
        } else {
            end = 1;
        }

        for (start = 0; start < end; start++) {
            node->parent = to_node;
            node->sibling = to_node->child;
            to_node->child = node;
            /* resolve the nodes */
            rv = _bcm_td2_cosq_node_resolve(unit, node, to_cosq + start);
            if (BCM_FAILURE(rv)) {
                to_node->child = node->sibling;
                node->parent = NULL;
                node->attached_to_input = tmp;
                return rv;
            }
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "                         hw_cosq=%d\n"),
                      node->attached_to_input));

            if ((node->type == _BCM_TD2_NODE_SERVICE_UCAST) && 
                ((start + 1) < end)){

                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_node_get(unit, gport, start + 1, NULL,
                                            &port, NULL, &node));

                if (!node) {
                    return BCM_E_NOT_FOUND;
                }
            }
        }
 
    } else {
            return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_attach_get
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
bcm_td2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_td2_cosq_node_t *sched_node;
    bcm_module_t modid, modid_out;
    bcm_port_t port, port_out;

    if (input_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, sched_gport, 0, &modid, &port, NULL,
                               &sched_node));

    if (sched_node->attached_to_input < 0) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (sched_node->parent != NULL) { /* sched_node is not an S1 scheduler */
        *input_gport = sched_node->parent->gport;
    } else {  /* sched_node is an S1 scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port,
                                    &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(*input_gport, modid_out, port_out);
    }
    *cosq = sched_node->attached_to_input;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_gport_child_get
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
bcm_td2_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                            bcm_cos_queue_t cosq,
                            bcm_gport_t *out_gport)
{
    _bcm_td2_cosq_node_t *in_node = NULL;
    _bcm_td2_cosq_node_t *cur_node = NULL;
    _bcm_td2_cosq_node_t *tmp_node = NULL;
    bcm_module_t modid;
    bcm_port_t port;

    if (out_gport == NULL) {
        return BCM_E_PARAM;
    }

    /* get the child of the input gport node
     * at an index of cosq
     */

    if ((cosq < 0) || ( cosq >= 64)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_node_get(unit, in_gport, 0, &modid, &port, NULL,
                               &in_node));
    if ((in_node->child == NULL) && (in_node->level == SOC_TD2_NODE_LVL_L2)) {
       return BCM_E_PARAM;
    }

    for (cur_node = in_node->child; cur_node != NULL;
         cur_node = cur_node->sibling) {
        if (cur_node->attached_to_input == cosq) {
           tmp_node = cur_node;
           break;
        }
    }
    if (tmp_node == NULL) {
        return BCM_E_NOT_FOUND;
    }
    *out_gport = tmp_node->gport;

    return BCM_E_NONE;
}

/*
 *  Convert HW drop probability to percent value
 */
STATIC int
_bcm_td2_hw_drop_prob_to_percent[] = {
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
_bcm_td2_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= _bcm_td2_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_td2_drop_prob_to_percent(int drop_prob) {
   return (_bcm_td2_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * index: degree, value: contangent(degree) * 100
 * max value is 0xffff (16-bit) at 0 degree
 */
STATIC int
_bcm_td2_cotangent_lookup_table[] =
{
    /*  0.. 5 */  65535, 5728, 2863, 1908, 1430, 1143,
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
_bcm_td2_angle_to_cells(int angle)
{
    return (_bcm_td2_cotangent_lookup_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_td2_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_td2_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}

/*
 * Function:
 *     _bcm_td2_cosq_bucket_set
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
_bcm_td2_cosq_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 kbits_burst_min, uint32 kbits_burst_max, 
                        uint32 flags)
{
    _bcm_td2_cosq_node_t *node = NULL;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    int index;
    uint32 rval;
    uint32 meter_flags;
    uint32 bucketsize_max, bucketsize_min;
    uint32 granularity_max, granularity_min;
    uint32 refresh_rate_max, refresh_rate_min;
    int refresh_bitsize = 0, bucket_bitsize = 0;
    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];

    /* caller should resolve the cosq. */
    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_SET(port) && 
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, port, cosq, NULL, 
                                    NULL, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            if (node->level == SOC_TD2_NODE_LVL_ROOT) {
                config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L0_MEM_0m, MMU_MTRO_L0_MEM_1m);
            } else if (node->level == SOC_TD2_NODE_LVL_L0) {
                uint32 mc_group_mode = 0;
                if (IS_TD2_HSP_PORT(unit, local_port)) {
                    SOC_IF_ERROR_RETURN(
                            READ_HSP_SCHED_PORT_CONFIGr(unit,
                                local_port, &rval));
                    mc_group_mode  = soc_reg_field_get(unit,
                                                       HSP_SCHED_PORT_CONFIGr,
                                                       rval, MC_GROUP_MODEf);
                }
                if (mc_group_mode) {
                    config_mem = SOC_TD2_PMEM(unit, local_port, 
                                              MMU_MTRO_L2_MEM_0m,
                                              MMU_MTRO_L2_MEM_1m);
                } else {
                    config_mem = SOC_TD2_PMEM(unit, local_port, 
                                              MMU_MTRO_L1_MEM_0m,
                                              MMU_MTRO_L1_MEM_1m);
                }
            } else if (node->level == SOC_TD2_NODE_LVL_L1) {
                config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (IS_CPU_PORT(unit, local_port)) {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
        } else {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L1_MEM_0m, MMU_MTRO_L1_MEM_1m);
        }
    }


    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    refresh_bitsize = soc_mem_field_length(unit, config_mem, MAX_REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, config_mem, MAX_THD_SELf);

    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, max_quantum, kbits_burst_max,
                                           meter_flags, refresh_bitsize,
                                           bucket_bitsize, &refresh_rate_max,
                                           &bucketsize_max, &granularity_max));

    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, min_quantum, kbits_burst_min,
                                           meter_flags, refresh_bitsize,
                                           bucket_bitsize, &refresh_rate_min,
                                           &bucketsize_min, &granularity_min));

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    soc_mem_field32_set(unit, config_mem, &entry, MAX_METER_GRANf, granularity_max);
    soc_mem_field32_set(unit, config_mem, &entry, MAX_REFRESHf, refresh_rate_max);
    soc_mem_field32_set(unit, config_mem, &entry, MAX_THD_SELf, bucketsize_max);

    soc_mem_field32_set(unit, config_mem, &entry, MIN_METER_GRANf, granularity_min);
    soc_mem_field32_set(unit, config_mem, &entry, MIN_REFRESHf, refresh_rate_min);
    soc_mem_field32_set(unit, config_mem, &entry, MIN_THD_SELf, bucketsize_min);

    soc_mem_field32_set(unit, config_mem, &entry, SHAPER_CONTROLf,
                        (flags & BCM_COSQ_BW_PACKET_MODE) ? 1 : 0);
    soc_mem_field32_set(unit, config_mem, &entry, EAV_ENABLEf,
                        (flags & BCM_COSQ_BW_EAV_MODE) ? 1 : 0);

    SOC_EGRESS_METERING_LOCK(unit);
    rv = soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, index, &entry);
    SOC_EGRESS_METERING_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *     _bcm_td2_cosq_bucket_get
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
_bcm_td2_cosq_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                        uint32 *flags)
{
    _bcm_td2_cosq_node_t *node = NULL;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    int index;
    uint32 rval;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;
    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_SET(port) && 
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, port, cosq, NULL, 
                                    &local_port, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            if (node->level == SOC_TD2_NODE_LVL_ROOT) {
                config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L0_MEM_0m, MMU_MTRO_L0_MEM_1m);
            } else if (node->level == SOC_TD2_NODE_LVL_L0) {
                uint32 mc_group_mode = 0;
                if (IS_TD2_HSP_PORT(unit, local_port)) {
                    SOC_IF_ERROR_RETURN(
                            READ_HSP_SCHED_PORT_CONFIGr(unit,
                                local_port, &rval));
                    mc_group_mode  = soc_reg_field_get(unit,
                                                       HSP_SCHED_PORT_CONFIGr,
                                                       rval, MC_GROUP_MODEf);
                }
                if (mc_group_mode) {
                    config_mem = SOC_TD2_PMEM(unit, local_port, 
                                              MMU_MTRO_L2_MEM_0m,
                                              MMU_MTRO_L2_MEM_1m);
                } else {
                    config_mem = SOC_TD2_PMEM(unit, local_port, 
                                              MMU_MTRO_L1_MEM_0m,
                                              MMU_MTRO_L1_MEM_1m);
                }
            } else if (node->level == SOC_TD2_NODE_LVL_L1) {
                config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (IS_CPU_PORT(unit, local_port)) {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L2_MEM_0m, MMU_MTRO_L2_MEM_1m);
        } else {
            config_mem = SOC_TD2_PMEM(unit, local_port, 
                                      MMU_MTRO_L1_MEM_0m, MMU_MTRO_L1_MEM_1m);
        }
    }

    if (min_quantum == NULL || max_quantum == NULL || 
        kbits_burst_max == NULL || kbits_burst_min == NULL) {
        return BCM_E_PARAM;
    }

    SOC_EGRESS_METERING_LOCK(unit);
    rv = soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, index, &entry);
    SOC_EGRESS_METERING_UNLOCK(unit);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    meter_flags = 0;
    *flags = 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }
    if (soc_mem_field32_get(unit, config_mem, &entry, SHAPER_CONTROLf)) {
        meter_flags |= _BCM_TD_METER_FLAG_PACKET_MODE;
        *flags |= BCM_COSQ_BW_PACKET_MODE;
    }
    if (soc_mem_field32_get(unit, config_mem, &entry, EAV_ENABLEf)) {
        *flags |= BCM_COSQ_BW_EAV_MODE;
    }

    granularity = soc_mem_field32_get(unit, config_mem, &entry, MAX_METER_GRANf);
    refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MAX_REFRESHf);
    bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MAX_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           max_quantum, kbits_burst_max));

    granularity = soc_mem_field32_get(unit, config_mem, &entry, MIN_METER_GRANf);
    refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MIN_REFRESHf);
    bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MIN_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           min_quantum, kbits_burst_min));
    return BCM_E_NONE;
}



STATIC int
_bcm_td2_cosq_time_domain_get(int unit, int time_id, int *time_value)
{
    uint32 rval;

    if (time_id < 0 || time_id > _TD2_NUM_TIME_DOMAIN - 1) {
        return SOC_E_PARAM;
    }

    if (time_value == NULL) {
        return SOC_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TIME_DOMAINr(unit, &rval));
    *time_value = soc_reg_field_get(unit, TIME_DOMAINr, rval, 
                                      time_domain[time_id].field);

    return SOC_E_NONE;
}




STATIC int
_bcm_td2_cosq_time_domain_set(int unit, int time_value, int *time_id)
{
    uint32 rval;
    int rv = SOC_E_NONE;
    int id;

    if (time_value < 0 || time_value > 63) {
        return SOC_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TIME_DOMAINr(unit, &rval));
    for (id = 0; id < _TD2_NUM_TIME_DOMAIN; id++) {/* Find a unset field from  TIME_0 to TIME_3 */
        if (!time_domain[id].ref_count) {
            soc_reg_field_set(unit, TIME_DOMAINr, &rval, time_domain[id].field, 
                                time_value);
            time_domain[id].ref_count++;
            break;
        }
    }
    
    if(id == _TD2_NUM_TIME_DOMAIN){/* No field in TIME_DOMAINr is free,return ERR*/
        rv = BCM_E_RESOURCE;
    }

    if(time_id){
        *time_id = id;
    }
        
    SOC_IF_ERROR_RETURN(WRITE_TIME_DOMAINr(unit, rval));
    return rv;
}

STATIC int
_bcm_td2_cosq_wred_index_resolve(int unit, bcm_port_t port,
                      bcm_cos_queue_t cosq, uint32 flags, uint32 lflags,
                      bcm_port_t *local_port, int *index_start, int *index_end,
                      int *port_id, int *pool_id)
{
    int index;
    int to_index = -1;
    int start_pool_idx;
    soc_info_t *si;
    int phy_port, mmu_port = 0;
    int service_pool = 0;
    bcm_port_t local_p = -1;

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    
    if((lflags & BCM_COSQ_DISCARD_DEVICE) || (flags & BCM_COSQ_DISCARD_DEVICE)) {
        if ((port == -1) || (cosq == -1)) {
            index = 1840;
            to_index = index + 3;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_WRED_POOL,
                                             NULL, &start_pool_idx, NULL));
            index = to_index = 1840 + start_pool_idx;
        }
    } else {
        if((lflags & BCM_COSQ_DISCARD_PORT) || (flags & BCM_COSQ_DISCARD_PORT)) {
            if (port == -1) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_WRED_PORT,
                                            &local_p, &index, NULL));
            if (cosq != -1) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                                 _BCM_TD2_COSQ_INDEX_STYLE_WRED_POOL,
                                                 NULL, &start_pool_idx, NULL));
                index = to_index = index + start_pool_idx;
            } else {
                to_index = index + 3;
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq == -1 ? 0:cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_WRED,
                                     &local_p, &index, NULL));

            si = &SOC_INFO(unit);
            phy_port = si->port_l2p_mapping[local_p];
            mmu_port = si->port_p2m_mapping[phy_port];
            mmu_port = (mmu_port >= 64) ? (mmu_port - 64) : (mmu_port);

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq == -1 ? 0:cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_WRED_POOL,
                                             NULL, &start_pool_idx, NULL));
            service_pool = start_pool_idx;

            if (cosq == -1) {
                to_index = index + NUM_COS(unit) - 1;
            } else {
                to_index = index;
            }
        }
    }

    if (index < 0) {
        return BCM_E_PARAM;
    }

    if (local_port != NULL) {
        *local_port = local_p;
    }
    if (index_start != NULL) {
        *index_start = index;
    }
    if (index_end != NULL) {
        *index_end = to_index;
    }
    if (port_id != NULL) {
        *port_id = mmu_port;
    }
    if (pool_id != NULL) {
        *pool_id = service_pool;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_cosq_wred_set
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
_bcm_td2_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 flags, uint32 min_thresh, uint32 max_thresh,
                      int drop_probability, int gain, int ignore_enable_flags,
                      uint32 lflags,int refresh_time)
{
    soc_mem_t wred_mem;
    bcm_port_t local_port = -1;
    int index;
    uint32 profile_index, old_profile_index;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    mmu_wred_config_entry_t qentry;
    void *entries[6];
    soc_mem_t mems[6];
    int rate, i, pipe, fpipe, tpipe, jj, to_index = -1, exists = 0;
    int time_id,time,old_time,current_time_sel;
    int port_id = 0;
    int pool_id = 0;
    int rv = SOC_E_NONE;
    static soc_mem_t wred_mems[2][6] = {
        {
        MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm
        },
        {
        MMU_WRED_DROP_CURVE_PROFILE_0_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_Y_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_Y_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_Y_PIPEm
        }
    };

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_wred_index_resolve(unit, port, cosq,
                            flags, lflags, &local_port, &index, &to_index,
                            &port_id, &pool_id));

    if (local_port >= 0) {
        fpipe = tpipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    } else {
        fpipe = _TD2_XPIPE;
        tpipe = _TD2_YPIPE;
    }

    /* WRED doesn't work on CPU/LB Ports */
    if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    for (pipe = fpipe; pipe <= tpipe; pipe++) {
      for(jj = index; jj <= to_index; jj++) {
        wred_mem = (pipe == _TD2_XPIPE) ? 
                    MMU_WRED_CONFIG_X_PIPEm : MMU_WRED_CONFIG_Y_PIPEm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, wred_mem, MEM_BLOCK_ALL, jj, &qentry));

        old_profile_index = 0xffffffff;
        if (flags & (BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL |
                     BCM_COSQ_DISCARD_TCP)) {
            old_profile_index = soc_mem_field32_get(unit, wred_mem, 
                                                    &qentry, PROFILE_INDEXf);
            entries[0] = &entry_tcp_green;
            entries[1] = &entry_tcp_yellow;
            entries[2] = &entry_tcp_red;
            entries[3] = &entry_nontcp_green;
            entries[4] = &entry_nontcp_yellow;
            entries[5] = &entry_nontcp_red;
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_get(unit, _bcm_td2_wred_profile[pipe][unit],
                                     old_profile_index, 1, entries));

            for (i = 0; i < 6; i++) {
                mems[i] = INVALIDm;
            }
            if (!(flags & (BCM_COSQ_DISCARD_COLOR_GREEN |
                           BCM_COSQ_DISCARD_COLOR_YELLOW |
                           BCM_COSQ_DISCARD_COLOR_RED))) {
                flags |= BCM_COSQ_DISCARD_COLOR_ALL;
            }
            if (!(flags & BCM_COSQ_DISCARD_NONTCP) || (flags & BCM_COSQ_DISCARD_TCP)) {
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[0] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_0_Y_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[1] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_1_Y_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[2] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_2_Y_PIPEm;
                }
            }
            if (flags & BCM_COSQ_DISCARD_NONTCP) {
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[3] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_3_Y_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[4] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_4_Y_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[5] = (pipe == _TD2_XPIPE) ? 
                            MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm : 
                            MMU_WRED_DROP_CURVE_PROFILE_5_Y_PIPEm;
                }
            }
            rate = _bcm_td2_percent_to_drop_prob(drop_probability);
            for (i = 0; i < 6; i++) {
                exists = 0;
                if ((soc_mem_field32_get(unit, wred_mems[pipe][i], entries[i], 
                              MIN_THDf) != 0x1ffff) && (mems[i] == INVALIDm)) {
                    mems[i] = wred_mems[pipe][i];
                    exists = 1;
                } else {
                    soc_mem_field32_set(unit, wred_mems[pipe][i], entries[i], 
                            MIN_THDf, (mems[i] == INVALIDm) ? 0x1ffff : min_thresh);
                }

                if ((soc_mem_field32_get(unit, wred_mems[pipe][i], entries[i], 
                        MAX_THDf) != 0x1ffff) && ((mems[i] == INVALIDm) || exists)) {
                    mems[i] = wred_mems[pipe][i];
                    exists = 1;
                } else {
                    soc_mem_field32_set(unit, wred_mems[pipe][i], entries[i], 
                            MAX_THDf, (mems[i] == INVALIDm) ? 0x1ffff :max_thresh);
                }

                if (!exists) {
                    soc_mem_field32_set(unit, wred_mems[pipe][i], entries[i], 
                        MAX_DROP_RATEf, (mems[i] == INVALIDm) ? 0 : rate);
                }
            }

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_td2_wred_profile[pipe][unit], 
                                                 entries, 1, &profile_index));
            soc_mem_field32_set(unit, wred_mem, &qentry, PROFILE_INDEXf, profile_index);
            soc_mem_field32_set(unit, wred_mem, &qentry, WEIGHTf, gain);
        }

        /* Some APIs only modify profiles */
        if (!ignore_enable_flags) {
            soc_mem_field32_set(unit, wred_mem, &qentry, CAP_AVERAGEf,
                              flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, WRED_ENf,
                              flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, ECN_MARKING_ENf,
                              flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?  1 : 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, PORTIDf,
                              port_id ?  port_id : 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, SPIDf,
                              pool_id ?  pool_id : 0);
        }

        current_time_sel = soc_mem_field32_get(unit, wred_mem, &qentry, TIME_DOMAIN_SELf);
        time = (refresh_time + 7) / 8 - 1; /* Round it up to avoid negative value */
        exists = 0;
        /* If the time value exist, update reference count only */
        for (time_id = 0; time_id < _TD2_NUM_TIME_DOMAIN; time_id++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_time_domain_get(unit, time_id, &old_time));
            if (time == old_time) {
                /* Only set exist flag if this entry reference count already update,
                              otherwise update reference count */
                if(time_id != current_time_sel){ 
                    soc_mem_field32_set(unit, wred_mem, &qentry, TIME_DOMAIN_SELf, 
                                          time_id);
                    time_domain[time_id].ref_count++;
                    time_domain[current_time_sel].ref_count--;
                }
                exists = 1;
                break;
            }
        }

        if(!exists){
            rv = _bcm_td2_cosq_time_domain_set(unit, time, &time_id);
            if(rv == SOC_E_NONE){
                soc_mem_field32_set(unit, wred_mem, &qentry, TIME_DOMAIN_SELf, 
                                      time_id);
                time_domain[current_time_sel].ref_count--;
            } else {
                return rv;
            }
        }
        
        BCM_IF_ERROR_RETURN(
              soc_mem_write(unit, wred_mem, MEM_BLOCK_ALL, jj, &qentry));

        if (old_profile_index != 0xffffffff) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _bcm_td2_wred_profile[pipe][unit],
                                        old_profile_index));
        }
      }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_td2_cosq_wred_get
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
_bcm_td2_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                      int *drop_probability, int *gain, uint32 lflags,int *refresh_time)
{
    bcm_port_t local_port = -1;
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
    soc_mem_t wred_mem = 0;
    mmu_wred_config_entry_t qentry;
    int rate, pipe,time_id,time;


    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_wred_index_resolve(unit, port, cosq,
                            *flags, lflags, &local_port, &index, NULL, NULL, NULL));

    if (local_port < 0) {
        /*
         *  local_port is -1, when flag is BCM_COSQ_DISCARD_DEVICE and port = -1 
         *  global config is set in both X_PIPE and Y_PIPE, since there are identical
         *  just read the config of X PIPE should be sufficient
         */
        pipe = _TD2_XPIPE;
    } else {
        pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    }

    /* WRED doesn't work on CPU/LB Ports */
    if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    wred_mem = (pipe == _TD2_XPIPE) ? MMU_WRED_CONFIG_X_PIPEm : MMU_WRED_CONFIG_Y_PIPEm;

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, wred_mem, MEM_BLOCK_ALL, index, &qentry));
    profile_index = soc_mem_field32_get(unit, wred_mem,
                                        &qentry, PROFILE_INDEXf);

    if (!(*flags & BCM_COSQ_DISCARD_NONTCP)) {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_0_Y_PIPEm;
            entry_p = &entry_tcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_1_Y_PIPEm;
            entry_p = &entry_tcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_2_Y_PIPEm;
            entry_p = &entry_tcp_red;
        } else {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_0_Y_PIPEm;
           entry_p = &entry_tcp_green;
        }
    } else {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_3_Y_PIPEm;
            entry_p = &entry_nontcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_4_Y_PIPEm;
            entry_p = &entry_nontcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_5_Y_PIPEm;
            entry_p = &entry_nontcp_red;
        } else {
            mem = (pipe == _TD2_XPIPE) ? MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm :
                                         MMU_WRED_DROP_CURVE_PROFILE_3_Y_PIPEm;
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
        (soc_profile_mem_get(unit, _bcm_td2_wred_profile[pipe][unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_td2_drop_prob_to_percent(rate);
    }

    if (gain) {
        *gain = soc_mem_field32_get(unit, wred_mem, &qentry, WEIGHTf);
    }

    if (refresh_time) {
        time_id = soc_mem_field32_get(unit, wred_mem, &qentry, TIME_DOMAIN_SELf);
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_time_domain_get(unit,time_id,&time));
        *refresh_time = (time + 1) * 8;
    }

    *flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE | BCM_COSQ_DISCARD_ENABLE);
    if (soc_mem_field32_get(unit, wred_mem, &qentry, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if (soc_mem_field32_get(unit, wred_mem, &qentry, WRED_ENf)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if (soc_mem_field32_get(unit, wred_mem, &qentry, ECN_MARKING_ENf)) {
        *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_decode_sp_masks(int num_sp, uint32 ucmap, uint32 *umap, int *numuc,
                            uint32 *mmap, int *nummc)
{
    int i, uindex, mindex;
    uint32 tmp_umap = 0, tmp_mmap = 0;
    
    mindex = uindex = 0;

    for (i = 0; i < num_sp; i++) {
        if (ucmap & (1 << i)) {
            /* MC */
            tmp_mmap |= 1 << mindex;
            mindex += 1;
        } else {
            /* UC */
            tmp_umap |= 1 << uindex;
            uindex += 1;
        }
    }

    if (numuc) {
        *numuc = uindex;
    }

    if (umap) {
        *umap = tmp_umap;
    }

    if (mmap) {
        *mmap = tmp_mmap;
    }

    if (nummc) {
        *nummc = mindex;
    }

    return 0;
}

STATIC int
_bcm_td2_sched_check_constraints(int unit, int level, 
                                 int *first_sp_child, 
                                 int *first_sp_mc_child,
                                 int *num_spri,
                                 uint32 *ucmap, 
                                 int child_index, 
                                 soc_td2_sched_mode_e cur_mode,
                                 soc_td2_sched_mode_e mode)
{
    int cur_first_child;
    int cur_num_spri;
    int cur_ucmap;
    int diff;

    uint32 umap = 0, mmap = 0;
    int  numuc = 0, nummc = 0;
    uint32 pnumn = 0;

    if (((cur_mode != SOC_TD2_SCHED_MODE_STRICT) &&
        (mode != SOC_TD2_SCHED_MODE_STRICT)) ||
       ((cur_mode == SOC_TD2_SCHED_MODE_STRICT) && 
        (mode == SOC_TD2_SCHED_MODE_STRICT))) {
        return BCM_E_NONE;
    }

    cur_num_spri = *num_spri;
    cur_ucmap = *ucmap;
  
    if (level == SOC_TD2_NODE_LVL_L1) {
        _bcm_td2_decode_sp_masks(cur_num_spri, cur_ucmap, &umap, 
                                            &numuc, &mmap, &nummc);
        if (child_index < 1480) {
            /* UC */
            pnumn = numuc;
            cur_num_spri = numuc;
            cur_first_child = *first_sp_child;
        } else {
            /* MC */
            pnumn = nummc;
            cur_num_spri = nummc;
            cur_first_child = *first_sp_mc_child;
        }
    } else {
        pnumn = cur_num_spri;
        cur_first_child = *first_sp_child;
    }

    if (cur_mode == SOC_TD2_SCHED_MODE_STRICT) {
        if ((child_index == cur_first_child) && (pnumn > 1)) {
            /* First SP node changed. Hence increment first_sp to next node */
            if (level == SOC_TD2_NODE_LVL_L1) {
                if (child_index < 1480) {
                    *first_sp_child = *first_sp_child + 1;
                } else {
                    *first_sp_mc_child = *first_sp_mc_child + 1;
                }
            } else {
                *first_sp_child = *first_sp_child + 1;
            }
        } else if (child_index != (cur_first_child + pnumn - 1)) {
            /* this node is not in the middle of SP nodes */
            return BCM_E_UNAVAIL;
        }

        cur_num_spri -= 1;
        pnumn -= 1;

        if (level == SOC_TD2_NODE_LVL_L1) {
            if (child_index < 1480) {
                *num_spri = pnumn + nummc;
            } else {
                *num_spri = pnumn + numuc;
                *ucmap >>= 1;
            }
        } else {
            *num_spri = cur_num_spri;
            *ucmap = cur_num_spri ? (1 << cur_num_spri) -1 : 0;
        }
        if (*num_spri == 0) {
            *ucmap = 0;
        }
    } else {
        if (cur_num_spri) {         
            if (cur_num_spri >=  8) {
                return BCM_E_PARAM;
            } else {
                /* adding new sp */
                if (child_index >= cur_first_child) {
                    diff = child_index - cur_first_child;
                    if (diff != cur_num_spri) {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    /* Setting of SP node prior to the current first SP child,
                     * need to make sure SP nodes are contiguous.
                     */
                    if ((child_index + 1) != cur_first_child) {
                        return BCM_E_UNAVAIL;
                    }
                    if (level == SOC_TD2_NODE_LVL_L1) {
                        if (child_index < 1480) {
                            *first_sp_child = child_index;
                        } else {
                            *first_sp_mc_child = child_index;
                        }
                    } else {
                        *first_sp_child = child_index;
                    }
                }

                if (level == SOC_TD2_NODE_LVL_L1) {
                    if (child_index < 1480) {
                        pnumn += 1;
                        *num_spri = pnumn + nummc;
                    } else {
                        pnumn += 1;
                        *num_spri = pnumn + numuc;
                        *ucmap = ((*ucmap ) << 1) + 1;
                    }
                } else {
                    pnumn += 1;
                    *num_spri = pnumn;
                    *ucmap = (1 << pnumn) - 1;
                }
            }
        } else {
            *num_spri = 1;
            *ucmap = 1;
            if (level == SOC_TD2_NODE_LVL_L1) {
                if (child_index < 1480) {
                    *first_sp_child = child_index;
                    *ucmap = 0;
                } else {
                    *first_sp_mc_child = child_index;
                }
            } else {
                *first_sp_child = child_index;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_sched_parent_child_set(int unit, int port, int level,
                                     int sched_index, int child_index,
                                     soc_td2_sched_mode_e sched_mode, 
                                     int weight, _bcm_td2_cosq_node_t *child_node)
{
    int wt, num_spri, first_sp_child, first_sp_mc_child;
    uint32 ucmap = 0, spmap = 0;
    soc_td2_sched_mode_e cur_sched_mode;
    _bcm_td2p_lls_info_t lls_tree_info;
    int rv;

    BCM_IF_ERROR_RETURN(soc_td2_cosq_get_sched_config(unit, port, level,
                sched_index, child_index, &num_spri, &first_sp_child,
                &first_sp_mc_child, &ucmap, &spmap, &cur_sched_mode, 
                &wt));    
    /* If the dynamic switchover feature is enabled, then compute the LLS 
       configuration and adjust the bandwidth at the level such that each 
       node has sufficient bandwidth to perform the switchover. Also set
       the new scheduler config based on the request */ 
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        
        if ((rv = _bcm_td2_compute_lls_config(unit, port, level, &first_sp_child, 
                    &first_sp_mc_child, &num_spri, 
                    &ucmap, &spmap, child_index, 
                    cur_sched_mode, sched_mode))) {
            return rv;
        }

        if (cur_sched_mode != sched_mode) {
            SOC_EGRESS_METERING_LOCK(unit);
            if ((rv = _bcm_td2_adjust_lls_bw(unit, port, child_node,
                          level + 1, child_index, 1, &lls_tree_info))){
                goto restore;
            }
        }

        if ((rv = soc_td2_cosq_set_sched_config(unit, port, level,
                    sched_index, child_index,
                    num_spri, first_sp_child,
                    first_sp_mc_child, ucmap,
                    spmap, sched_mode, weight))){
            goto restore;
        }

        if (cur_sched_mode != sched_mode) {
            goto restore;
        }

    } else {
        if (_bcm_td2_sched_check_constraints(unit, level, &first_sp_child, 
                    &first_sp_mc_child, &num_spri, &ucmap,
                    child_index, cur_sched_mode, sched_mode)) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(soc_td2_cosq_set_sched_config(unit, port, level,
                    sched_index, child_index, num_spri, first_sp_child,
                    first_sp_mc_child, ucmap, spmap, sched_mode, weight));

    }

    return BCM_E_NONE;

/* If there is an error during the switchover process, then restore the 
   bandwidth to the previous state */
restore:
    if (cur_sched_mode != sched_mode) {
        rv = _bcm_td2_adjust_lls_bw(unit, port, child_node,
                                    level + 1,
                                    child_index,
                                    0, &lls_tree_info);
        SOC_EGRESS_METERING_UNLOCK(unit);
    }
    return rv;

}

/*
 * Function:
 *     _bcm_td2_cosq_sched_get
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
_bcm_td2_cosq_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int *mode, int *weight)
{
    _bcm_td2_cosq_node_t *node, *child_node;
    bcm_port_t local_port;
    int sch_index, lvl = SOC_TD2_NODE_LVL_L1, numq;
    soc_td2_sched_mode_e    sched_mode;
    _bcm_td2_mmu_info_t *mmu_info;

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    if (_bcm_td2_cosq_port_has_ets(unit, local_port) && BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
          (_bcm_td2_cosq_node_get(unit, port, 0, NULL, &local_port, NULL, &node));
        sch_index = node->hw_index;
        numq = node->numq;
        if ((numq != -1) && (cosq >= numq)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_td2_cosq_child_node_at_input(node, cosq, &child_node));
        lvl = child_node->level;
        sch_index = child_node->hw_index;
    } else {
        numq = (IS_CPU_PORT(unit, local_port)) ? SOC_INFO(unit).num_cpu_cosq : NUM_COS(unit);

        if (cosq >= numq) {
            return BCM_E_PARAM;
        }

        lvl = (IS_CPU_PORT(unit, local_port)) ? 
                            SOC_TD2_NODE_LVL_L2 : SOC_TD2_NODE_LVL_L1;

        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                   _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER, NULL, &sch_index, NULL));

    }

    BCM_IF_ERROR_RETURN(
            soc_td2_cosq_get_sched_mode(unit, local_port, lvl, sch_index,
                            &sched_mode, weight));

    switch(sched_mode) {
        case SOC_TD2_SCHED_MODE_STRICT:
            *mode = BCM_COSQ_STRICT;
        break;
        case SOC_TD2_SCHED_MODE_WRR:
            *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
        case SOC_TD2_SCHED_MODE_WDRR:
            *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
        default:
            return BCM_E_INTERNAL;
        break;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_td2_cosq_sched_set
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
_bcm_td2_cosq_sched_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        int mode, int weight)
{
    _bcm_td2_cosq_node_t *node = NULL, *child_node;
    bcm_port_t local_port;
    int sch_index, lvl = SOC_TD2_NODE_LVL_L0, cpu_l0_index;
    int numq, lwts = 1, child_index, cpu_l1_index, roff = 0;
    soc_td2_sched_mode_e sched_mode;
    int rv = BCM_E_NONE;
    int     cur_mode, cur_weight, new_mode;
    /* 1 - indicates dynamic change allowed without emptying the queue. */
    int     dynamic_change = 0;
    void *setting = NULL;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    if ((weight < 0) || (weight > 127)) {
        return BCM_E_PARAM;
    }

    switch(mode) {
        case BCM_COSQ_STRICT:
            sched_mode = SOC_TD2_SCHED_MODE_STRICT;
            lwts = 0;
        break;
        case BCM_COSQ_ROUND_ROBIN:
            sched_mode = SOC_TD2_SCHED_MODE_WRR;
            lwts = 1;
        break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            sched_mode = SOC_TD2_SCHED_MODE_WRR;
            lwts = weight;
        break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            sched_mode = SOC_TD2_SCHED_MODE_WDRR;
            lwts = weight;
        break;
        default:
            return BCM_E_PARAM;
    }

    if ( lwts == 0) {
        sched_mode = SOC_TD2_SCHED_MODE_STRICT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_sched_get(unit, gport, cosq, &cur_mode, &cur_weight));
    
    new_mode = (weight == 0) ? BCM_COSQ_STRICT : mode;
    
    /*The weight for both WDRR and WRR schemes can be changed dynamically.*/
    if ((cur_mode == new_mode) && 
        (new_mode != BCM_COSQ_STRICT)){
        dynamic_change = 1;
    } else {
        dynamic_change = 0;
    }

    if (!dynamic_change) {
        /* don't apply it for TD2+ if dynamic scheduling feature is enabled*/
        if (!soc_feature(unit, soc_feature_vector_based_spri)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_dynamic_sched_update_begin(unit, gport, cosq));
        }

    }

    if (_bcm_td2_cosq_port_has_ets(unit, local_port)) {
        rv = _bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node);
        if (BCM_SUCCESS(rv)) {
            numq = node->numq;
            /* HSP/LLS - child node at cosq index of parent gport
             * is resloved to set it's schedule mode and weight
             */
            rv = _bcm_td2_cosq_child_node_at_input(node, cosq, &child_node);
            if (BCM_SUCCESS(rv)) {
                child_index = child_node->hw_index;
                if (IS_TD2_HSP_PORT(unit, local_port)) {
                    rv = soc_td2_cosq_set_sched_mode(unit, local_port,
                                 child_node->level, child_node->hw_index, sched_mode, lwts);
                } else {
                    BCM_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                                        local_port, &setting));
                    rv = _bcm_td2_cosq_sched_parent_child_set(unit,
                          local_port, node->level, node->hw_index,
                          child_node->hw_index, sched_mode, lwts, child_node);
                    BCM_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit,
                                                        local_port, setting));
                }
            }
        }
    } else {
        numq = (IS_CPU_PORT(unit, local_port)) ? SOC_INFO(unit).num_cpu_cosq : NUM_COS(unit);

        if (cosq >= numq) {
            rv = BCM_E_PARAM;
        }

        if (BCM_SUCCESS(rv)) {
            if (IS_CPU_PORT(unit, local_port)) {
                lvl = SOC_TD2_NODE_LVL_L1;
                roff = cosq/8;
            } else if (IS_TD2_HSP_PORT(unit, local_port)) {
                lvl = SOC_TD2_NODE_LVL_L0;
                roff = 1;
            } else {
                lvl = SOC_TD2_NODE_LVL_L0;
            }

            rv = soc_td2_sched_hw_index_get(unit, local_port, 
                                            lvl, roff, &sch_index);
        }
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                   _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER, NULL, &child_index, NULL);
        } 
        
        if (BCM_SUCCESS(rv)) {
            if (IS_TD2_HSP_PORT(unit, local_port)) {
                rv = soc_td2_cosq_set_sched_mode(unit, local_port,
                                                 SOC_TD2_NODE_LVL_L1,
                                                 child_index, 
                                                 sched_mode, lwts);
            } else {
                if (IS_CPU_PORT(unit, local_port)) {
                    /* set the L0.0 index as parent, L0.(cosq/8) as child in 
                    * schduling mode */
                    rv = soc_td2_sched_hw_index_get(unit, local_port, 
                                     SOC_TD2_NODE_LVL_L0, 0, &cpu_l0_index);
                    if (BCM_SUCCESS(rv)) {
                        rv = soc_td2_sched_hw_index_get(unit, local_port, 
                                    SOC_TD2_NODE_LVL_L1, cosq/8, &cpu_l1_index);
                    }
                    if (BCM_SUCCESS(rv)) {
                        BCM_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                                            local_port, &setting));
                        rv = _bcm_td2_cosq_sched_parent_child_set(unit, 
                                  local_port, SOC_TD2_NODE_LVL_L0, cpu_l0_index, 
                                  cpu_l1_index, sched_mode, 1, NULL);
                        BCM_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit,
                                                            local_port, setting));
                    }
                }
                if (BCM_SUCCESS(rv)) {
                    BCM_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                                        local_port, &setting));
                    rv = _bcm_td2_cosq_sched_parent_child_set(unit, local_port,
                            lvl, sch_index, child_index, sched_mode, lwts, NULL);
                    BCM_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit,
                                                        local_port, setting));
                }
            }
        }
    }

    if (!dynamic_change) {
        /* don't apply it for TD2+ if dynamic scheduling feature is enabled*/
        if (!soc_feature(unit, soc_feature_vector_based_spri)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_dynamic_sched_update_end(unit, gport, cosq));
        } 
    }

    return rv;
}


/*
 * Function:
 *     bcm_td2_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_detach(int unit, int software_state_only)
{
    int i, pipe;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    (void) _bcm_bst_detach(unit);

    if (NULL != _bcm_td2_endpoint_queuing_info[unit]) {
        if (NULL != _bcm_td2_endpoint_queuing_info[unit]->ptr_array) {
            for (i = 0; i < _BCM_TD2_NUM_ENDPOINT(unit); i++) {
                if (_BCM_TD2_ENDPOINT_IS_USED(unit, i)) {
                    sal_free(_BCM_TD2_ENDPOINT(unit, i));
                    _BCM_TD2_ENDPOINT(unit, i) = NULL;
                }
            }
            sal_free(_bcm_td2_endpoint_queuing_info[unit]->ptr_array);
            _bcm_td2_endpoint_queuing_info[unit]->ptr_array = NULL;
        }
        if (NULL != _bcm_td2_endpoint_queuing_info[unit]->cos_map_profile) {
            (void) soc_profile_mem_destroy(unit,
                       _bcm_td2_endpoint_queuing_info[unit]->cos_map_profile);
            sal_free(_bcm_td2_endpoint_queuing_info[unit]->cos_map_profile);
            _bcm_td2_endpoint_queuing_info[unit]->cos_map_profile = NULL;
        }
        sal_free(_bcm_td2_endpoint_queuing_info[unit]);
        _bcm_td2_endpoint_queuing_info[unit] = NULL;
    }

    /* Cleanup profile for PORT_COS_MAP table */
    if (_bcm_td2_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td2_cos_map_profile[unit]));
        sal_free(_bcm_td2_cos_map_profile[unit]);
        _bcm_td2_cos_map_profile[unit] = NULL;
    }

    /* Cleanup profile for IFP_COS_MAP table. */
    if (_bcm_td2_ifp_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td2_ifp_cos_map_profile[unit]));
        sal_free(_bcm_td2_ifp_cos_map_profile[unit]);
        _bcm_td2_ifp_cos_map_profile[unit] = NULL;
    }

    /* Cleanup profile for SERVICE_COS_MAP table */
    if (_bcm_td2_service_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td2_service_cos_map_profile[unit]));
        sal_free(_bcm_td2_service_cos_map_profile[unit]);
        _bcm_td2_service_cos_map_profile[unit] = NULL;
    }

    /* Cleanup profile for SERVICE_PORT_MAP table */
    if (_bcm_td2_service_port_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td2_service_port_map_profile[unit]));
        sal_free(_bcm_td2_service_port_map_profile[unit]);
        _bcm_td2_service_port_map_profile[unit] = NULL;
    }

    /* Cleanup profile for MMU_QCN_SITB table */
    if (_bcm_td2_sample_int_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                 _bcm_td2_sample_int_profile[unit]));
        sal_free(_bcm_td2_sample_int_profile[unit]);
        _bcm_td2_sample_int_profile[unit] = NULL;
    }

    /* Cleanup profile for MMU_QCN_CPQ_SEQ register */
    if (_bcm_td2_feedback_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                                _bcm_td2_feedback_profile[unit]));
        sal_free(_bcm_td2_feedback_profile[unit]);
        _bcm_td2_feedback_profile[unit] = NULL;
    }

    /* Cleanup profile for PRIO2COS_LLFC register */
    if (_bcm_td2_llfc_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                                _bcm_td2_llfc_profile[unit]));
        sal_free(_bcm_td2_llfc_profile[unit]);
        _bcm_td2_llfc_profile[unit] = NULL;
    }

    /* Cleanup profile for VOQ_MOD_MAP table */
    if (_bcm_td2_voq_port_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td2_voq_port_map_profile[unit]));
        sal_free(_bcm_td2_voq_port_map_profile[unit]);
        _bcm_td2_voq_port_map_profile[unit] = NULL;
    }

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        /* Cleanup profile for MMU_WRED_DROP_CURVE_PROFILE_x/y tables */
        if (_bcm_td2_wred_profile[pipe][unit] != NULL) {
            SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                    _bcm_td2_wred_profile[pipe][unit]));
            sal_free(_bcm_td2_wred_profile[pipe][unit]);
            _bcm_td2_wred_profile[pipe][unit] = NULL;
        }
    }

    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_td2_cosq_cpu_cosq_config[unit][i] != NULL) {
            sal_free(_bcm_td2_cosq_cpu_cosq_config[unit][i]);
            _bcm_td2_cosq_cpu_cosq_config[unit][i] = NULL;
        }
    }

    if (_bcm_td2_mmu_info[unit] != NULL) {
        _bcm_td2_cosq_free_memory(_bcm_td2_mmu_info[unit]);
        sal_free(_bcm_td2_mmu_info[unit]);
        _bcm_td2_mmu_info[unit] = NULL;
    }

    soc_trident2_fc_map_shadow_free(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_config_set
 * Purpose:
 *     Set the number of COS queues
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    port_cos_map_entry_t hg_cos_map_entries[16];
    void *entries[1], *hg_entries[1];
    uint32 index, hg_index;
    int count, hg_count;
    bcm_port_t port;
    int cos, prio;
    uint32 i;
    voq_cos_map_entry_t voq_cos_map;
    int ref_count;
    int cpu_hg_index = 0;

    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }

    /* clear out old profiles. */
    index = 0;
    while (index < soc_mem_index_count(unit, PORT_COS_MAPm)) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit,
                                      _bcm_td2_cos_map_profile[unit],
                                      index, &ref_count));
        if (ref_count > 0) {
            while (ref_count) {
                BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit, 
                                 _bcm_td2_cos_map_profile[unit], index));
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
    hg_entries[0] = &hg_cos_map_entries;
    prio = 0;
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                UC_COS1f, cos);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                MC_COS1f, cos);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                HG_COSf, cos);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                RQE_Q_NUMf, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                            UC_COS1f, numq - 1);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                            MC_COS1f, numq - 1);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                            HG_COSf, numq - 1);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                            RQE_Q_NUMf, numq - 1);
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* Use identical mapping for Higig port */
    sal_memset(hg_cos_map_entries, 0, sizeof(hg_cos_map_entries));
    prio = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            UC_COS1f, prio);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            MC_COS1f, prio);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            RQE_Q_NUMf, prio);
    }
    for (prio = 8; prio < 13; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            UC_COS1f, 7);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            MC_COS1f, 7);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            RQE_Q_NUMf, 7);
    }
#else /* BCM_COSQ_HIGIG_MAP_DISABLE */
    sal_memcpy(hg_cos_map_entries, cos_map_entries, sizeof(cos_map_entries));
#endif /* !BCM_COSQ_HIGIG_MAP_DISABLE */

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_td2_cos_map_profile[unit], 
                                            entries, 16, &index));
    soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[14],
                               HG_COSf, 8);
    soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[15],
                               HG_COSf, 9);

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_td2_cos_map_profile[unit], 
                                            hg_entries, 16, &hg_index));
    count = 0;
    hg_count = 0;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        
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
    cpu_hg_index = SOC_INFO(unit).cpu_hg_index;
    if (cpu_hg_index != -1) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                cpu_hg_index, SELECTf, index / 16));
        count++;
    }

    for (i = 1; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_td2_cos_map_profile[unit], index, 0);
    }
    for (i = 1; i < hg_count; i++) {
        soc_profile_mem_reference(unit, _bcm_td2_cos_map_profile[unit],
                                  hg_index, 0);
    }
    /* Delete the default higig cos map profile for the non-higig system */
    if (hg_count == 0) {
        BCM_IF_ERROR_RETURN(
            soc_profile_mem_delete(unit, _bcm_td2_cos_map_profile[unit],
                                   hg_index));
    }

    sal_memset(&voq_cos_map, 0, sizeof(voq_cos_map));
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (i < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, VOQ_COS_MAPm, &voq_cos_map, VOQ_COS_OFFSETf, cos);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, VOQ_COS_MAPm, MEM_BLOCK_ANY, cos,
                          &voq_cos_map));
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, VOQ_COS_MAPm, &voq_cos_map, VOQ_COS_OFFSETf, numq - 1);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, VOQ_COS_MAPm, MEM_BLOCK_ANY, prio,
                          &voq_cos_map));
    }

    _TD2_NUM_COS(unit) = numq;

    return BCM_E_NONE;
}

#ifdef BCM_TRIDENT2PLUS_SUPPORT
/*
 * Function:
 *     bcm_td2p_port_cosq_config_set
 * Purpose:
 *     called during flexport during port attach/detach
 *     to update the cos_map profile for the port
 * Parameters:
 *     unit   - unit number
 *     port   - port number
 *     enable - add/delete
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2p_port_cosq_config_set(int unit, bcm_port_t  port, int enable)
{
    port_cos_map_entry_t cos_map_entries[16];
    port_cos_map_entry_t hg_cos_map_entries[16];
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entries[1], *hg_entries[1];
    uint32 index,  old_index, hg_index;
    int cos, prio, numq, rv;
    uint32 i;

    numq = _TD2_NUM_COS(unit);

    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;
    hg_entries[0] = &hg_cos_map_entries;
    prio = 0;
    if (enable) {
        for (cos = 0; cos < numq; cos++) {
            for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
                soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                        UC_COS1f, cos);
                soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                        MC_COS1f, cos);
                soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                        HG_COSf, cos);
                soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                        RQE_Q_NUMf, cos);
                prio++;
            }
        }
        for (prio = 8; prio < 16; prio++) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                    UC_COS1f, numq - 1);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                    MC_COS1f, numq - 1);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                    HG_COSf, numq - 1);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                    RQE_Q_NUMf, numq - 1);
        }


#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
        /* Use identical mapping for Higig port */
        sal_memset(hg_cos_map_entries, 0, sizeof(hg_cos_map_entries));
        prio = 0;
        for (prio = 0; prio < 8; prio++) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    UC_COS1f, prio);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    MC_COS1f, prio);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    RQE_Q_NUMf, prio);
        }
        for (prio = 8; prio < 13; prio++) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    UC_COS1f, 7);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    MC_COS1f, 7);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                    RQE_Q_NUMf, 7);
        }
#else /* BCM_COSQ_HIGIG_MAP_DISABLE */
        sal_memcpy(hg_cos_map_entries, cos_map_entries, sizeof(cos_map_entries));
#endif /* !BCM_COSQ_HIGIG_MAP_DISABLE */
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[14],
                HG_COSf, 8);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[15],
                HG_COSf, 9);

        if (IS_HG_PORT(unit, port)) {
            soc_mem_lock(unit, PORT_COS_MAPm);

            rv  =  soc_profile_mem_add(unit, _bcm_td2_cos_map_profile[unit],
                    hg_entries, 16, &hg_index);

            soc_mem_unlock(unit, PORT_COS_MAPm);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        hg_index / 16));
        } else {
            soc_mem_lock(unit, PORT_COS_MAPm);

            rv =  soc_profile_mem_add(unit, _bcm_td2_cos_map_profile[unit],
                    entries, 16, &index);

            soc_mem_unlock(unit, PORT_COS_MAPm);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        index / 16));
        }

    } else {
        BCM_IF_ERROR_RETURN
            (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port,
                               &cos_map_sel_entry));
        old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                SELECTf);
        old_index *= 16;

        soc_mem_lock(unit, PORT_COS_MAPm);

        rv = soc_profile_mem_delete(unit, _bcm_td2_cos_map_profile[unit],
                old_index);

        soc_mem_unlock(unit, PORT_COS_MAPm);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return BCM_E_NONE;
}
#endif

STATIC int 
_bcm_td2_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        int *p_pool_start, int *p_pool_end)
{
    int local_port;
    int pool;
    if (cosq == BCM_COS_INVALID) {
        *p_pool_start = 0;
        *p_pool_end = 3;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));

    *p_pool_start = *p_pool_end = pool;
    return BCM_E_NONE;
}

STATIC int 
_bcm_td2_cosq_ingress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp, 
                         int *p_pool_start, int *p_pool_end)
{
    int local_port;
    uint32 rval, pool;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    
    if (pri_grp == BCM_COS_INVALID) {
        *p_pool_start = 0;
        *p_pool_end = 3;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= 8)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, local_port, &rval));
    pool = soc_reg_field_get(unit, THDI_PORT_PG_SPIDr, rval, 
                                prigroup_spid_field[pri_grp]);
    
    *p_pool_start = *p_pool_end = pool;
    return BCM_E_NONE;
}

STATIC int 
_bcm_td2_cosq_ingress_pg_get(int unit, bcm_gport_t gport, bcm_cos_queue_t pri, 
                         int *p_pg_start, int *p_pg_end)
{
    bcm_port_t local_port;
    uint32  rval,pool;
    soc_reg_t reg = INVALIDr;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };

    if (pri == BCM_COS_INVALID) {
        *p_pg_start = 0;
        *p_pg_end = 7;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (pri > 15) {
        return BCM_E_PARAM;
    }
       /* get PG for port using Port+Cos */
    if (pri < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
        pool = soc_reg_field_get(unit, reg, rval, prigroup_field[pri]);

    *p_pg_start = *p_pg_end = pool;
    return BCM_E_NONE;
}


STATIC _bcm_bst_cb_ret_t
_bcm_td2_bst_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                    bcm_bst_stat_id_t bid, int *pipe, int *start_hw_index, 
                    int *end_hw_index, int *rcb_data, void **cb_data, int *bcm_rv)
{
    int phy_port, mmu_port, local_port;
    _bcm_td2_cosq_node_t *node = NULL;
    soc_info_t *si;
    uint32 rval = 0;
    int pool_idx;

    *bcm_rv = BCM_E_NONE;

#define _BST_IF_ERROR_GOTO_EXIT(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) { goto error_exit; } } while(0)

    _BST_IF_ERROR_GOTO_EXIT
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    *pipe = (mmu_port < 64) ? 0 : 1;

    if (bid == bcmBstStatIdDevice) {
        *start_hw_index = *end_hw_index = 0;
        *bcm_rv = BCM_E_NONE;
        return _BCM_BST_RV_OK;
    }

    if (bid == bcmBstStatIdEgrPool) {
        /* Egress Unicast Pool */
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            goto error_exit;
        }        
        _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_egress_sp_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdEgrMCastPool) {
        /* Egress Multicast Pool */
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            goto error_exit;
        }
        _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_egress_sp_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;        
    } else if (bid == bcmBstStatIdIngPool) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_ingress_sp_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdRQEPool) {
        /* RQE pool */
        if (cosq == BCM_COS_INVALID) {
            *start_hw_index = 0;
            *end_hw_index = _TD2_MMU_NUM_POOL - 1;
        }else if ((cosq < 0) || (cosq >= _TD2_MMU_NUM_RQE_QUEUES)) {
            return BCM_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN
                (READ_MMU_THDR_DB_CONFIG1_PRIQr(unit, cosq, &rval));

            pool_idx = soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                                               SPIDf);
            *start_hw_index = *end_hw_index = pool_idx;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdRQEQueue) {
        /* RQE queue */
        if (cosq == BCM_COS_INVALID) {
            *start_hw_index = 0;
            *end_hw_index = _TD2_MMU_NUM_RQE_QUEUES - 1;
        }else if ((cosq < 0) || (cosq >= _TD2_MMU_NUM_RQE_QUEUES)) {
            return BCM_E_PARAM;
        } else {
            *start_hw_index = *end_hw_index = cosq;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdUCQueueGroup) {
        /* unicast queue group */
        _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit,
              gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP,
              NULL, start_hw_index, NULL));
        *end_hw_index = *start_hw_index;
        return _BCM_BST_RV_OK;
    }

    if (bid == bcmBstStatIdPortPool) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_td2_cosq_ingress_sp_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port & 0x3f) * 4) + *start_hw_index;
        *end_hw_index = ((mmu_port & 0x3f) * 4) + *end_hw_index;
    } else if ((bid == bcmBstStatIdPriGroupShared) || 
               (bid == bcmBstStatIdPriGroupHeadroom)) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_td2_cosq_ingress_pg_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port & 0x3f) * 8) + *start_hw_index;
        *end_hw_index = ((mmu_port & 0x3f) * 8) + *end_hw_index;
    } else if (bid == bcmBstStatIdEgrPortPoolSharedUcast ||
               bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        /* egress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_td2_cosq_egress_sp_get(unit, gport, cosq,
                                            start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port & 0x3f) * 4) + *start_hw_index;
        *end_hw_index = ((mmu_port & 0x3f) * 4) + *end_hw_index;
    } else {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (bid != bcmBstStatIdUcast) {
                goto error_exit;
            }
            _BST_IF_ERROR_GOTO_EXIT
                (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                        &local_port, NULL, &node));
            if (!node) {
                goto error_exit;
            }
            *start_hw_index = *end_hw_index = node->hw_index;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (bid != bcmBstStatIdMcast) {
                goto error_exit;
            }
            _BST_IF_ERROR_GOTO_EXIT
                (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                        &local_port, NULL, &node));
            if (!node) {
                goto error_exit;
            }
            *start_hw_index = *end_hw_index = node->hw_index - 1480;
        } else { 
            _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_port_has_ets(unit, local_port));
            
            if (cosq < 0) {
                if (bid == bcmBstStatIdUcast) {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, 0, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, start_hw_index, NULL));

                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, NUM_COS(unit) - 1, 
                          _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, end_hw_index, NULL));

                } else {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, 0, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *start_hw_index -= 1480;

                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, NUM_COS(unit) - 1, 
                          _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, end_hw_index, NULL));
                    *end_hw_index -= 1480;
                }
            } else {
                if (bid == bcmBstStatIdUcast) {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *end_hw_index = *start_hw_index;
                } else {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_td2_cosq_index_resolve(unit, 
                          local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *start_hw_index -= 1480;
                    *end_hw_index = *start_hw_index;
                }
            } /* if (cosq < 0) */
        } /* if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) { */
    } /* if (bid == bcmBstStatIdPortPool) { */
   
    *bcm_rv = BCM_E_NONE;
    return _BCM_BST_RV_OK;

error_exit:
    *bcm_rv = BCM_E_PARAM;
    return _BCM_BST_RV_ERROR;
}

STATIC int
_bcm_td2_cosq_set_scheduler_states(int unit, bcm_port_t port, int busy)
{
    int lvl, lvl_offset, hw_index, rv;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_cosq_list_t *list;

    mmu_info = _bcm_td2_mmu_info[unit];
    res = _BCM_TD2_PRESOURCES(mmu_info, _BCM_TD2_PORT_TO_PIPE(unit, port));

    for (lvl = SOC_TD2_NODE_LVL_L0; lvl <= SOC_TD2_NODE_LVL_L1; lvl++) {
        lvl_offset = 0;

        list = NULL;
        if (lvl == SOC_TD2_NODE_LVL_L0) {
            list = &res->l0_sched_list;
        } else if (lvl == SOC_TD2_NODE_LVL_L1) {
            list = &res->l1_sched_list;
        }

        if (!list) {
            continue;
        }
        while ((rv = soc_td2_sched_hw_index_get(unit, port, lvl, 
                                   lvl_offset, &hw_index)) == SOC_E_NONE) {
        if (busy) {
                _bcm_td2_node_index_set(list, hw_index, 1);
        } else {
                _bcm_td2_node_index_clear(list, hw_index, 1);
        }
            lvl_offset += 1;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_bst_map_resource_to_gport_cos(int unit, bcm_bst_stat_id_t bid, int port, 
                         int index, bcm_gport_t *gport, bcm_cos_t *cosq)
{
    soc_info_t *si;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t   *pres;
    int ii, pipe;
    int phy_port, mmu_port, hw_index;
    _bcm_bst_resource_info_t *resInfo;
    int pipe_offset = 0;

    if (port == -2) {
        pipe = _TD2_YPIPE;
    } else {
        pipe = _TD2_XPIPE;
    }

    resInfo = _BCM_BST_RESOURCE(unit, bid);
    pipe_offset = ((resInfo->index_max + 1) / NUM_PIPE(unit)) * pipe;
    mmu_info = _bcm_td2_mmu_info[unit];
    pres = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    si = &SOC_INFO(unit);

    switch (bid) {
    case bcmBstStatIdDevice:
        *gport = -1;
        *cosq = BCM_COS_INVALID;
        break;

    case bcmBstStatIdIngPool:
    case bcmBstStatIdEgrPool:
    case bcmBstStatIdEgrMCastPool:
    case bcmBstStatIdRQEPool:
        *gport = -1;
        *cosq = index;
        break;

    case bcmBstStatIdRQEQueue:
        *gport = -1;
        *cosq = index % _TD2_MMU_NUM_RQE_QUEUES;
        break;

    case bcmBstStatIdUCQueueGroup:
        *gport = index;
        *cosq =  -1;
        break;

    case bcmBstStatIdPortPool:
    case bcmBstStatIdEgrPortPoolSharedUcast:
    case bcmBstStatIdEgrPortPoolSharedMcast:
        mmu_port= (index - pipe_offset)/4;
        if (pipe == _TD2_YPIPE) {
            mmu_port = mmu_port + 64;
        }
        phy_port = si->port_m2p_mapping[mmu_port];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % 4;
        break;

    case bcmBstStatIdPriGroupHeadroom:
    case bcmBstStatIdPriGroupShared:
        mmu_port= (index - pipe_offset)/8;
        if (pipe == _TD2_YPIPE) {
            mmu_port = mmu_port + 64;
        }
        phy_port = si->port_m2p_mapping[mmu_port];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % 8;
        break;
 
    case bcmBstStatIdMcast:
        pipe = _TD2_XPIPE;
        if (index >= 568) {
            pipe = _TD2_YPIPE;
        }
        hw_index = soc_td2_l2_hw_index(unit, index, 0);

        if (mmu_info->ets_mode) {
            for (ii = 0; ii < _BCM_TD2_NUM_L2_MC_LEAVES; ii++) {
                node = &pres->p_mc_queue_node[ii];
                if (node->in_use == TRUE) {
                    phy_port = si->port_l2p_mapping[node->local_port];
                    mmu_port = si->port_p2m_mapping[phy_port];

                    if (((pipe == _TD2_YPIPE) && mmu_port < 64) ||
                        ((pipe == _TD2_XPIPE) && mmu_port > 64)) {
                        continue;
                    }

                    if (node->hw_index == hw_index) {
                        *gport = node->gport;
                        *cosq = 0;
                        break;
                    }
                }
            }
        } else {
            int port_mc_base = 0;
            int port_num_mc = 0;
            PBMP_ALL_ITER(unit, port) {
                phy_port = si->port_l2p_mapping[port];
                mmu_port = si->port_p2m_mapping[phy_port];

                if (((pipe == _TD2_YPIPE) && mmu_port < 64) ||
                    ((pipe == _TD2_XPIPE) && mmu_port > 64)) {
                    continue;
                }
                if (IS_LB_PORT(unit, port)) {
                    continue;
                }

                port_num_mc = si->port_num_cosq[port];
                port_mc_base = si->port_cosq_base[port];

                if ((index >= port_mc_base) &&
                    (index < port_mc_base + port_num_mc)) {
                    *gport = port;
                    *cosq = index - port_mc_base;
                    break;
                }
            }
        }
        break;

    case bcmBstStatIdUcast:
        pipe = _TD2_XPIPE;
        if (index >= 1480) {
            pipe = _TD2_YPIPE;
        }
        hw_index = soc_td2_l2_hw_index(unit, index, 1);

        if (mmu_info->ets_mode) {
            for (ii = 0; ii < _BCM_TD2_NUM_L2_UC_LEAVES; ii++) {
                node = &pres->p_queue_node[ii];
                if (node->in_use == TRUE) {
                    phy_port = si->port_l2p_mapping[node->local_port];
                    mmu_port = si->port_p2m_mapping[phy_port];

                    if (((pipe == _TD2_YPIPE) && mmu_port < 64) ||
                        ((pipe == _TD2_XPIPE) && mmu_port > 64)) {
                        continue;
                    }

                    if (node->hw_index == hw_index) { 
                        *gport = node->gport;
                        *cosq = 0;
                        break;
                    }
                }
            }
        } else {
            int port_uc_base = 0;
            int port_num_uc = 0;
            PBMP_ALL_ITER(unit, port) {
                phy_port = si->port_l2p_mapping[port];
                mmu_port = si->port_p2m_mapping[phy_port];

                if (((pipe == _TD2_YPIPE) && mmu_port < 64) ||
                    ((pipe == _TD2_XPIPE) && mmu_port > 64)) {
                    continue;
                }
                if (IS_LB_PORT(unit, port)) {
                    continue;
                }

                port_num_uc = si->port_num_uc_cosq[port];
                port_uc_base = si->port_uc_cosq_base[port];

                if ((index >= port_uc_base) &&
                    (index < port_uc_base + port_num_uc)) {
                    *gport = port;
                    *cosq = index - port_uc_base;
                    break;
                }
            }
        }
        break;
    default:
        break;
    }

    return BCM_E_NONE;
}
#ifdef BCM_TRIDENT2PLUS_SUPPORT
/*
 * Function:
 *     _bcm_td2p_update_base_queue_num
 * Purpose:
 *     After flexing, update the base queue numbers in the _bcm_td2_mmu_info_t
 *     structure and update the associated registers between base
 *     queues and logical ports.
 * Parameters:
 *     unit             - (IN) unit number
 *     nport            - (IN) number of entries in flexport_data
 *     ports            - (IN) contains the ports for what new
 *                             ports are being added via flexport
 *                             and need queues updated so we only touch those.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_update_base_queue_num(int unit, int nport, int * ports)
{
    uint64 rval64;
    int qnum, pipe, i;
    soc_info_t *si;
    bcm_port_t port;
    _bcm_td2_mmu_info_t *mmu_info;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    int phy_port, mmu_port;
#endif

    si = &SOC_INFO(unit);

    COMPILER_64_ZERO(rval64);

    /* This info needs to be updated when flexport occurs.
       Set this information based upon bcm_td2_cosq_init.*/
    mmu_info = _bcm_td2_mmu_info[unit];
    mmu_info->pipe_resources[_TD2_XPIPE].num_base_queues = 0;
    mmu_info->pipe_resources[_TD2_YPIPE].num_base_queues = 0;

    /* assign queues to ports */
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
            continue;
        }
        pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
        mmu_info->port_info[port].resources = &mmu_info->pipe_resources[pipe];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        /* This needs to be done for Trident 2+ since queues are no longer
           based upon a running number but are instead indexed by MMU Port
           number.  Example: If MMU port 0 and 1 are using VBS, they'll only
           use queues 0-9 and 10-19.  If MMU port 2 is LLS, it will start
           at queue 24 rather than 20.  We need to add this total to
           port_num_uc_cosq so that the device won't think it has overrun
           its queues. */
        if (SOC_IS_TD2P_TT2P(unit)) {
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];

            /* Make sure num_base_queues accounts for not only the port's
               base queue, but all the queues assigned to it as well */
            if (mmu_info->port_info[port].resources->num_base_queues <
                ((mmu_port%64) + 1) * si->port_num_uc_cosq[port]) {
                mmu_info->port_info[port].resources->num_base_queues = 
                    ((mmu_port%64) + 1) * si->port_num_uc_cosq[port];
            }
        } else 
#endif
        {
            mmu_info->port_info[port].resources->num_base_queues +=
                                                si->port_num_uc_cosq[port];
        }
        
        /* iterate through the newly flexed ports and only update those. */
        for (i = 0; i < nport; i++) {
            if (port == ports[i]) {
                break;
            }
        }
        if (i != nport) {
            mmu_info->port_info[port].mc_base = si->port_cosq_base[port];
            mmu_info->port_info[port].mc_limit = si->port_cosq_base[port] +
                                                    si->port_num_cosq[port];

            mmu_info->port_info[port].uc_base = si->port_uc_cosq_base[port];
            mmu_info->port_info[port].uc_limit = si->port_uc_cosq_base[port] +
                                                    si->port_num_uc_cosq[port];

            if (!SOC_WARM_BOOT(unit)) {
                COMPILER_64_ZERO(rval64);
                qnum = soc_td2_logical_qnum_hw_qnum(unit, port,
                                                si->port_uc_cosq_base[port], 1);
                soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                                &rval64, BASE_QUEUE_NUM_0f, qnum);
                soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                                &rval64, BASE_QUEUE_NUM_1f, qnum);
                BCM_IF_ERROR_RETURN(soc_reg_set(unit, ING_COS_MODE_64r, port, 0, rval64));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_set_flexport_port_q_cb
 * Purpose:
 *     Set the callback for updating the logical port to queue association.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_td2p_set_flexport_port_q_cb(int unit)
{
    BCM_IF_ERROR_RETURN(
       soc_td2p_set_flexport_port_q_callback(unit, 
                                             _bcm_td2p_update_base_queue_num));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_set_ets_mode_q_cb
 * Purpose:
 *     Set the callback for checking ets mode
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_td2p_set_ets_mode_q_cb(int unit)
{
    BCM_IF_ERROR_RETURN(
       soc_td2p_set_ets_mode_q_callback(unit, 
                                             _bcm_td2_cosq_port_has_ets));
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     bcm_td2_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_init(int unit)
{
    int numq, alloc_size, phy_port, mmu_port, ii, index;
    soc_info_t *si;
    bcm_port_t port;
    soc_reg_t mem;
    soc_reg_t reg;
    static soc_mem_t wred_mems[2][6] = {
        {
        MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm
        },
        {
        MMU_WRED_DROP_CURVE_PROFILE_0_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_Y_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_Y_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_Y_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_Y_PIPEm
        }
    };

    int entry_words[6], pipe;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    uint32 profile_index;
    _bcm_td2_mmu_info_t *mmu_info;
    int i, qnum, wred_prof_count, endpoint_num;
    _bcm_bst_device_handlers_t bst_callbks;
    _bcm_td2_pipe_resources_t   *pres;
    uint64 rval64;
    _bcm_td2_cosq_list_t *list = NULL;

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN (bcm_td2_cosq_detach(unit, 0));
    }

    si = &SOC_INFO(unit);

    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    NUM_COS(unit) = numq;
    soc_trident2_fc_map_shadow_create(unit);


    /* Create profile for PORT_COS_MAP table */
    if (_bcm_td2_cos_map_profile[unit] == NULL) {
        _bcm_td2_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_td2_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_td2_cos_map_profile[unit]));

    /* Create profile for IFP_COS_MAP table. */
    if (_bcm_td2_ifp_cos_map_profile[unit] == NULL) {
        _bcm_td2_ifp_cos_map_profile[unit]
            = sal_alloc(sizeof(soc_profile_mem_t), "IFP_COS_MAP Profile Mem");
        if (_bcm_td2_ifp_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_ifp_cos_map_profile[unit]);
    }
    mem = IFP_COS_MAPm;
    entry_words[0] = sizeof(ifp_cos_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_td2_ifp_cos_map_profile[unit]));

    /* Create profile for SERVICE_COS_MAP table */
    if (_bcm_td2_service_cos_map_profile[unit] == NULL) {
        alloc_size = sizeof(soc_profile_mem_t);
        _bcm_td2_service_cos_map_profile[unit] = sal_alloc(alloc_size,
                                                           "SERVICE_COS_MAP");
        if (_bcm_td2_service_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_service_cos_map_profile[unit]);
    }
    mem = SERVICE_COS_MAPm;
    entry_words[0] = sizeof(service_cos_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_create(unit, &mem, entry_words, 1,
                               _bcm_td2_service_cos_map_profile[unit]));

    /* Create profile for SERVICE_PORT_MAP table */
    if (_bcm_td2_service_port_map_profile[unit] == NULL) {
        alloc_size = sizeof(soc_profile_mem_t);
        _bcm_td2_service_port_map_profile[unit] = sal_alloc(alloc_size,
                                                            "SERVICE_PORT_MAP");
        if (_bcm_td2_service_port_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_service_port_map_profile[unit]);
    }

    mem = SERVICE_PORT_MAPm;
    entry_words[0] = sizeof(service_port_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_create(unit, &mem, entry_words, 1,
                               _bcm_td2_service_port_map_profile[unit]));

    /* Create profile for MMU_QCN_SITB table */
    if (_bcm_td2_sample_int_profile[unit] == NULL) {
        _bcm_td2_sample_int_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "QCN sample Int Profile Mem");
        if (_bcm_td2_sample_int_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_sample_int_profile[unit]);
    }
    mem = MMU_QCN_SITBm;
    entry_words[0] = sizeof(mmu_qcn_sitb_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_td2_sample_int_profile[unit]));

    /* Create profile for MMU_QCN_CPQ_SEQ register */
    if (_bcm_td2_feedback_profile[unit] == NULL) {
        _bcm_td2_feedback_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "QCN Feedback Profile Reg");
        if (_bcm_td2_feedback_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_td2_feedback_profile[unit]);
    }
    reg = MMU_QCN_CPQ_SEQr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1,
                                _bcm_td2_feedback_profile[unit]));

    /* Create profile for PRIO2COS_LLFC register */
    if (_bcm_td2_llfc_profile[unit] == NULL) {
        _bcm_td2_llfc_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "PRIO2COS_LLFC Profile Reg");
        if (_bcm_td2_llfc_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_td2_llfc_profile[unit]);
    }
    reg = PRIO2COS_PROFILEr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1, _bcm_td2_llfc_profile[unit]));

    /* Create profile for VOQ_MOD_MAP table */
    if (_bcm_td2_voq_port_map_profile[unit] == NULL) {
        _bcm_td2_voq_port_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "VOQ_PORT_MAP Profile Mem");
        if (_bcm_td2_voq_port_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td2_voq_port_map_profile[unit]);
    }
    mem = VOQ_PORT_MAPm;
    entry_words[0] = sizeof(voq_port_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                           _bcm_td2_voq_port_map_profile[unit]));

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
        if (_bcm_td2_wred_profile[pipe][unit] == NULL) {
            _bcm_td2_wred_profile[pipe][unit] =
                    sal_alloc(sizeof(soc_profile_mem_t), "WRED Profile Mem");
            if (_bcm_td2_wred_profile[pipe][unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(_bcm_td2_wred_profile[pipe][unit]);
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
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_create(unit, wred_mems[pipe], entry_words, 6,
                                    _bcm_td2_wred_profile[pipe][unit]));
    }

    alloc_size = sizeof(_bcm_td2_mmu_info_t) * 1;
    if (_bcm_td2_mmu_info[unit] == NULL) {
        _bcm_td2_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_td2_mmu_info");

        if (_bcm_td2_mmu_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(_bcm_td2_mmu_info[unit], 0, alloc_size);
    }

    mmu_info = _bcm_td2_mmu_info[unit];
    mmu_info->ets_mode = 0;
    mmu_info->pipe_resources[_TD2_XPIPE].num_base_queues = 0;
    mmu_info->pipe_resources[_TD2_YPIPE].num_base_queues = 0;

    /* assign queues to ports */
    PBMP_ALL_ITER(unit, port) {
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
        mmu_info->port_info[port].resources = &mmu_info->pipe_resources[pipe];
        mmu_info->port_info[port].mc_base = si->port_cosq_base[port];
        mmu_info->port_info[port].mc_limit = si->port_cosq_base[port] + 
                                                si->port_num_cosq[port];

        mmu_info->port_info[port].uc_base = si->port_uc_cosq_base[port];
        mmu_info->port_info[port].uc_limit = si->port_uc_cosq_base[port] + 
                                                si->port_num_uc_cosq[port];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        /* This needs to be done for Trident 2+ since queues are no longer
           based upon a running number but are instead indexed by MMU Port
           number.  Example: If MMU port 0 and 1 are using VBS, they'll only
           use queues 0-9 and 10-19.  If MMU port 2 is LLS, it will start
           at queue 24 rather than 20.  We need to add this total to
           port_num_uc_cosq so that the device won't think it has overrun
           its queues. */
        if (SOC_IS_TD2P_TT2P(unit)) {
           phy_port = si->port_l2p_mapping[port];
           mmu_port = si->port_p2m_mapping[phy_port];

           if (mmu_info->port_info[port].resources->num_base_queues < 
               (mmu_port%64) * si->port_num_uc_cosq[port]) {
              mmu_info->port_info[port].resources->num_base_queues = 
                 (mmu_port%64) * si->port_num_uc_cosq[port];
           }

           mmu_info->port_info[port].resources->num_base_queues +=
                                                  si->port_num_uc_cosq[port];
        } else 
#endif
        {
            mmu_info->port_info[port].resources->num_base_queues +=
                                                si->port_num_uc_cosq[port];
        }
        if (!SOC_WARM_BOOT(unit)) {
            COMPILER_64_ZERO(rval64);
            qnum = soc_td2_logical_qnum_hw_qnum(unit, port, 
                                            si->port_uc_cosq_base[port], 1);
            soc_reg64_field32_set(unit, ING_COS_MODE_64r, 
                                            &rval64, BASE_QUEUE_NUM_0f, qnum);
            soc_reg64_field32_set(unit, ING_COS_MODE_64r, 
                                            &rval64, BASE_QUEUE_NUM_1f, qnum);
            BCM_IF_ERROR_RETURN(soc_reg_set(unit, ING_COS_MODE_64r, port, 0, rval64));
        }
    }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TD2P_TT2P(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_td2p_set_flexport_port_q_cb(unit));
        BCM_IF_ERROR_RETURN(_bcm_td2p_set_ets_mode_q_cb(unit));
    }
#endif

    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_td2_cosq_cpu_cosq_config[unit][i] == NULL) {
            _bcm_td2_cosq_cpu_cosq_config[unit][i] =
                sal_alloc(sizeof(_bcm_td2_cosq_cpu_cosq_config_t), "CPU Cosq Config");

            if (_bcm_td2_cosq_cpu_cosq_config[unit][i] == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_td2_cosq_cpu_cosq_config[unit][i], 0,
                       sizeof(_bcm_td2_cosq_cpu_cosq_config_t));
            _bcm_td2_cosq_cpu_cosq_config[unit][i]->enable = 1;
        }
    }

    if (soc_feature(unit, soc_feature_endpoint_queuing)) {
        if (_bcm_td2_endpoint_queuing_info[unit] == NULL) {
            _bcm_td2_endpoint_queuing_info[unit] =
                sal_alloc(sizeof(_bcm_td2_endpoint_queuing_info_t),
                        "Endpoint Queuing Info");
            if (_bcm_td2_endpoint_queuing_info[unit] == NULL) {
                return BCM_E_MEMORY;
            }
        }
        sal_memset(_bcm_td2_endpoint_queuing_info[unit], 0,
                sizeof(_bcm_td2_endpoint_queuing_info_t));

        /* The maximum number of endpoints is limited to 2^14,
         * since the endpoint ID field is limited to 14 bits in
         * Higig2 header.
         */
        endpoint_num = (1U << 14);
        _bcm_td2_endpoint_queuing_info[unit]->ptr_array = sal_alloc(
                endpoint_num * sizeof(_bcm_td2_endpoint_t *),
                "Endpoint Pointer Array");
        if (_bcm_td2_endpoint_queuing_info[unit]->ptr_array == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(_bcm_td2_endpoint_queuing_info[unit]->ptr_array, 0,
                endpoint_num * sizeof(_bcm_td2_endpoint_t *));
        _BCM_TD2_NUM_ENDPOINT(unit) = endpoint_num;
        /* Reserve endpoint 0 */
        _bcm_td2_endpoint_queuing_info[unit]->ptr_array[0] =
            sal_alloc(sizeof(_bcm_td2_endpoint_t), "Endpoint Info");
        if (_bcm_td2_endpoint_queuing_info[unit]->ptr_array[0] == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(_bcm_td2_endpoint_queuing_info[unit]->ptr_array[0], 0,
                   sizeof(_bcm_td2_endpoint_t));

        _bcm_td2_endpoint_queuing_info[unit]->cos_map_profile = sal_alloc(
                sizeof(soc_profile_mem_t), "Priority to CoS Map Profile");
        if (_bcm_td2_endpoint_queuing_info[unit]->cos_map_profile == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init
            (_bcm_td2_endpoint_queuing_info[unit]->cos_map_profile);
        mem = ENDPOINT_COS_MAPm;
        entry_words[0] = sizeof(endpoint_cos_map_entry_t) / sizeof(uint32);
        SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                    _bcm_td2_endpoint_queuing_info[unit]->cos_map_profile));
    }

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        pres = _BCM_TD2_PRESOURCES(mmu_info, pipe);
        pres->p_queue_node = &mmu_info->queue_node[0];
        pres->p_mc_queue_node = &mmu_info->mc_queue_node[0];
        pres->ext_qlist.count = _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        pres->ext_qlist.bits = _bcm_td2_cosq_alloc_clear(
                pres->ext_qlist.bits,
                SHR_BITALLOCSIZE(pres->ext_qlist.count), 
                                    "ext_qlist");
        if (pres->ext_qlist.bits == NULL) {
            _bcm_td2_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }

        pres->l0_sched_list.count =
            soc_mem_index_count(unit, ES_PIPE0_LLS_L0_PARENTm);
        pres->l0_sched_list.bits = _bcm_td2_cosq_alloc_clear(
                pres->l0_sched_list.bits,
                SHR_BITALLOCSIZE(pres->l0_sched_list.count),
                                    "l0_sched_list");
        if (pres->l0_sched_list.bits == NULL) {
            _bcm_td2_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }

        pres->l1_sched_list.count =
            soc_mem_index_count(unit, ES_PIPE0_LLS_L1_PARENTm);
        pres->l1_sched_list.bits = _bcm_td2_cosq_alloc_clear(
                pres->l1_sched_list.bits,
                SHR_BITALLOCSIZE(pres->l1_sched_list.count),
                                    "l1_sched_list");
        if (pres->l1_sched_list.bits == NULL) {
            _bcm_td2_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }
    }

    for (i = 0; i < _BCM_TD2_NUM_TOTAL_SCHEDULERS; i++) {
        _BCM_TD2_COSQ_LIST_NODE_INIT(mmu_info->sched_node, i);
    }

    for (i = 0; i < _BCM_TD2_NUM_L2_UC_LEAVES; i++) {
        _BCM_TD2_COSQ_LIST_NODE_INIT(mmu_info->queue_node, i);
    }

    for (i = 0; i < _BCM_TD2_NUM_L2_MC_LEAVES; i++) {
        _BCM_TD2_COSQ_LIST_NODE_INIT(mmu_info->mc_queue_node, i);
    }

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        pres = _BCM_TD2_PRESOURCES(mmu_info, pipe);
        for (i = SOC_TD2_NODE_LVL_L1; i <= SOC_TD2_NODE_LVL_L2; i++) {
            index = _soc_td2_invalid_parent_index(unit, i);
            if (i == SOC_TD2_NODE_LVL_L1) {
                list = &pres->l0_sched_list;
            } else if (i == SOC_TD2_NODE_LVL_L2) {
                list = &pres->l1_sched_list;
            } else {
                continue;
            }
            if (index >= 0) {
                _bcm_td2_node_index_set(list, index, 1);
            }
        }
    }

    /* reserve the resources for HSP ports */
    PBMP_ALL_ITER(unit, port) {
        if (IS_TD2_HSP_PORT(unit, port)) {
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];

            pres = _BCM_TD2_PRESOURCES(mmu_info, (mmu_port >= 64) ? 1 : 0);
            _bcm_td2_node_index_set(&pres->l0_sched_list, (mmu_port & 0x3f)*5, 5);
            _bcm_td2_node_index_set(&pres->l1_sched_list, (mmu_port & 0x3f)*10, 10);
        }
    }

    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &_bcm_td2_bst_index_resolve;
    bst_callbks.reverse_resolve_index = &_bcm_td2_cosq_bst_map_resource_to_gport_cos;
    BCM_IF_ERROR_RETURN(_bcm_bst_attach(unit, &bst_callbks));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_td2_cosq_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Add default entries for PORT_COS_MAP memory profile */
    BCM_IF_ERROR_RETURN(bcm_td2_cosq_config_set(unit, numq));

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        /* Add default entries for MMU_WRED_DROP_CURVE_PROFILE_x memory profile */
        sal_memset(&entry_tcp_green, 0, sizeof(entry_tcp_green));
        sal_memset(&entry_tcp_yellow,0, sizeof(entry_tcp_yellow));
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
        for (ii = 0; ii < 6; ii++) {
            soc_mem_field32_set(unit, wred_mems[pipe][ii], entries[ii], 
                                MIN_THDf, 0x1ffff);
            soc_mem_field32_set(unit, wred_mems[pipe][ii], entries[ii], 
                                MAX_THDf, 0x1ffff);
        }
        profile_index = 0xffffffff;
    wred_prof_count = soc_mem_index_count(unit, MMU_WRED_CONFIG_X_PIPEm);
    while(wred_prof_count) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, 
                                    _bcm_td2_wred_profile[pipe][unit],
                                    entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                   _bcm_td2_wred_profile[pipe][unit],
                                   profile_index, 0));
            }
        wred_prof_count -= 1;
        }
    }

    /* Add default entries for PRIO2COS_PROFILE register profile */
    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_cosq_default_llfc_profile_attach(unit, port));
    }

    if (!mmu_info->curr_shared_limit) {
        BCM_IF_ERROR_RETURN(soc_td2_mmu_get_shared_size(unit,
                                            &mmu_info->curr_shared_limit));
    }
    /* Starting the curr_merger_index from 2,
     *  since 0 is configured in all the FC_MAP_TBL2 by default,
     *  so it might effect the traffic of other queues
     */
    mmu_info->curr_merger_index = 2;
    /* Reset shapers. */
    BCM_IF_ERROR_RETURN(_bcm_td2_lls_shapers_init(unit));

    /* reset time_domain ref_count so that entries can be added upon
     * rc */
    time_domain[0].ref_count = 3640;
    time_domain[1].ref_count = 0;
    time_domain[2].ref_count = 0;
    time_domain[3].ref_count = 0;

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_td2_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_config_get(int unit, int *numq)
{
    if (numq != NULL) {
        *numq = _TD2_NUM_COS(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_cosq_mapping_set
 * Purpose:
 *     Set COS queue for the specified priority of an ingress port
 * Parameters:
 *     unit     - (IN) unit number
 *     ing_port - (IN) ingress port
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *     gport    - (IN) queue group GPORT identifier
 *     cosq     - (IN) COS queue number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port, outport = -1;
    bcm_cos_queue_t hw_cosq;
    soc_field_t field = INVALIDf, field2 = INVALIDf, field3 = INVALIDf;
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];
    uint32 old_index, new_index;
    voq_cos_map_entry_t voq_cos_map;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t *res = NULL;
    int rv, pipe = -1;
    int valid = 0;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport != -1) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &outport));
        mmu_info = _bcm_td2_mmu_info[unit];
        pipe = _BCM_TD2_PORT_TO_PIPE(unit, outport);
        res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    }

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = cosq;
        } else {
            if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_COS,
                  NULL, &hw_cosq, NULL));

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, gport, cosq, NULL, 
                                        NULL, NULL, &node));
                if (node->hw_index >= (res->num_base_queues + 
                                   (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE))) {
                    return BCM_E_PARAM;
                }
            }
        }
        if ((outport != -1) && (IS_HG_PORT(unit, outport))) {
            field = HG_COSf;
        } else {
            field = UC_COS1f;
        }
        break;
    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = cosq;
        } else {
            if (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_COS,
                  NULL, &hw_cosq, NULL));
        }
        field = MC_COS1f;
        field2 = RQE_Q_NUMf;
        break;
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = cosq;
        } else {
            return BCM_E_PARAM;
        }
        field = UC_COS1f;
        field2 = MC_COS1f;
        field3 = RQE_Q_NUMf;
        break;
    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = cosq;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_COS,
                  NULL, &hw_cosq, NULL));
        }

        BCM_IF_ERROR_RETURN(
            READ_VOQ_COS_MAPm(unit, MEM_BLOCK_ALL, priority, &voq_cos_map));
        valid = soc_mem_field32_get(unit, VOQ_COS_MAPm, &voq_cos_map,
                                    VOQ_COS_USE_MODf);
        
        /* Setting the  VOQ_PORT_BASE_SELECT,
           so that BASE_QUEUE_NUM_1 is used for queue
           calculation */

        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                &voq_cos_map, VOQ_PORT_BASE_SELECTf, 1);
        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                            &voq_cos_map, VOQ_COS_USE_MODf, 1);
 
        if (valid && (soc_mem_field32_get(unit, VOQ_COS_MAPm, &voq_cos_map, 
                        VOQ_COS_OFFSETf) != hw_cosq)) {
            soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                    &voq_cos_map, VOQ_COS_OFFSETf, hw_cosq);
        }
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, VOQ_COS_MAPm, MEM_BLOCK_ANY, priority,
                    &voq_cos_map));
        return BCM_E_NONE;
        break;
    default:
        return BCM_E_PARAM;
    }

    entries[0] = &cos_map_entries;

    if (hw_cosq < 0 || hw_cosq >= 16) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                    SELECTf);
    old_index *= 16;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_td2_cos_map_profile[unit],
                             old_index, 16, entries));
    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority], field,
                        hw_cosq);
    if (field2 != INVALIDf) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                            field2,hw_cosq);
    }
    if (field3 != INVALIDf) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                            field3,hw_cosq);
    }
    
    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_delete(unit, _bcm_td2_cos_map_profile[unit],
                                old_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_td2_cos_map_profile[unit], entries,
                             16, &new_index);

    soc_mem_unlock(unit, PORT_COS_MAPm);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_mem_field32_set(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf,
                        new_index / 16);
    BCM_IF_ERROR_RETURN
        (WRITE_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                            &cos_map_sel_entry));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (IS_CPU_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    new_index / 16));
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_mapping_set
 * Purpose:
 *     Set which cosq a given priority should fall into
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (IN) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */

int
bcm_td2_cosq_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
                         bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;

    BCM_PBMP_CLEAR(pbmp);

    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            } else if (BCM_GPORT_IS_LOCAL(port)) {
                local_port = BCM_GPORT_LOCAL_GET(port);
            } else if (BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &local_port));
            } else {
                return BCM_E_PARAM;
            }
        } else {
            local_port = port;
        }

        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }

        BCM_PBMP_PORT_ADD(pbmp, local_port);
    }

    PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
    }

    if ((cosq < 0) || (cosq >= _TD2_NUM_COS(unit))) {
        return BCM_E_PARAM;
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         * mcast */
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_mapping_set(unit, local_port, 
                                    priority,
                                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                    -1, cosq));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_mapping_get
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
_bcm_td2_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                          uint32 flags, bcm_gport_t *gport,
                          bcm_cos_queue_t *cosq)
{
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_cos_queue_t hw_cosq = BCM_COS_INVALID;
    int index, ii;
    cos_map_sel_entry_t cos_map_sel_entry;
    voq_cos_map_entry_t voq_cos_map;
    void *entry_p;
    int ets_mode;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport) {
        *gport = BCM_GPORT_INVALID;
    }

    *cosq = BCM_COS_INVALID;

    if (local_port == CMIC_PORT(unit)) {
        /* CPU has two indices: 
        *  index_0 for Eth packet and index_106 for HiGig packet.
        *  Getting cos mapping from index_106 instead of index_0,
        *  although they are identical. Because previous version only write 
        *  index_106. Keep this for warmboot backward compatability.
        */
        BCM_IF_ERROR_RETURN
            (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, SOC_INFO(unit).cpu_hg_index,
                               &cos_map_sel_entry));
    } else {
        BCM_IF_ERROR_RETURN
            (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                               &cos_map_sel_entry));
    }
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_td2_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    index + priority);

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    res = _BCM_TD2_PRESOURCES(mmu_info, _BCM_TD2_PORT_TO_PIPE(unit, local_port));
    ets_mode = _bcm_td2_cosq_port_has_ets(unit, local_port);

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, UC_COS1f);
        if ((IS_CPU_PORT(unit, local_port) == FALSE) && gport && ets_mode) {
            for (ii = port_info->uc_base; ii < port_info->uc_limit; ii++) {
                node = &mmu_info->queue_node[ii];
                if (node->numq > 0 && node->hw_cosq == hw_cosq) {
                    *gport = node->gport;
                    *cosq = 0;
                    break;
                }
            }
            if (ii == port_info->uc_limit) {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, MC_COS1f);
        if (gport && ets_mode) {
            for (ii = port_info->mc_base; ii < port_info->mc_limit; ii++) {
                node = &mmu_info->mc_queue_node[ii];
                if (node->numq > 0 && node->hw_cosq == hw_cosq) {
                    *gport = node->gport;
                    *cosq = 0;
                    break;
                }
            }
            if (ii == port_info->mc_limit) {
                return BCM_E_NOT_FOUND;
            }
        }
        break;

    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        BCM_IF_ERROR_RETURN(
            READ_VOQ_COS_MAPm(unit, MEM_BLOCK_ALL, priority, &voq_cos_map));
        hw_cosq = soc_mem_field32_get(unit, VOQ_COS_MAPm, 
                                        &voq_cos_map, VOQ_COS_OFFSETf);
        if (gport && ets_mode) {
            for (ii = res->num_base_queues; ii < _BCM_TD2_NUM_L2_UC_LEAVES; ii++) {
                node = &mmu_info->queue_node[ii];
                
                if (node->numq > 0 && node->hw_cosq == hw_cosq) {
                    *gport = node->gport;
                    break;
                }
            }
            if (ii == _BCM_TD2_NUM_L2_UC_LEAVES) {
                return BCM_E_NOT_FOUND;
            } 
        }
        break;
    }

    *cosq = hw_cosq;

    if (((gport &&
        (*gport == BCM_GPORT_INVALID) && (*cosq == BCM_COS_INVALID))) ||
        (*cosq == BCM_COS_INVALID)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
                        bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);

    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            } else if (BCM_GPORT_IS_LOCAL(port)) {
                local_port = BCM_GPORT_LOCAL_GET(port);
            } else if (BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, port, &local_port));
            } else {
                return BCM_E_PARAM;
            }
        } else {
            local_port = port;
        }

        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }

        BCM_PBMP_PORT_ADD(pbmp, local_port);
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_mapping_get(unit, local_port, 
                 priority, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL, cosq));
        break;
    }    
    return BCM_E_NONE;
}

int
bcm_td2_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    return _bcm_td2_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

int
bcm_td2_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    return _bcm_td2_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

/*
 * Function:
 *     bcm_td2_cosq_port_sched_set
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
 */
int
bcm_td2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    bcm_port_t port;
    int        num_weights, i;
    int        cur_mode, cur_weight; 
    int        reset_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN; 

    num_weights = _TD2_NUM_COS(unit);
    BCM_PBMP_ITER(pbm, port) {
        /*WRR mode is a property per port, 
         * so just get cosq=0 schedule mode*/
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_sched_get(unit, port, 0,
                                     &cur_mode, &cur_weight));
        if (cur_mode == BCM_COSQ_STRICT) {
            reset_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        } else {
            reset_mode = cur_mode;
        }
        
        /* Schedule mode switch need base on default wrr mode */        
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_sched_set(unit, port, i, 
                                         reset_mode, 1));
        }

        for (i = 0; i < num_weights; i++) {         
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_sched_set(unit, port, i, mode, weights[i]));
            
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_port_sched_get
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
 */
int
bcm_td2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    bcm_port_t port;
    int i, num_weights;

    BCM_PBMP_ITER(pbm, port) {
        /* coverity[illegal_address : FALSE] */
        if (IS_CPU_PORT(unit, port) && SOC_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
            continue;
        }
        num_weights = _TD2_NUM_COS(unit);
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_sched_get(unit, port, i, mode, &weights[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_sched_weight_max_get
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
bcm_td2_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        /* TD2 - weights can be calculated based on LLS/HSP based 
         * scheduling type associated to the port, in either case 
         * the maximum weight can be 127  
         */ 
        *weight_max = 
            (1 << soc_reg_field_length(unit, HSP_SCHED_L0_NODE_WEIGHTr, 
                                        WEIGHTf)) - 1; 
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_bandwidth_set
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
bcm_td2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags)
{
    uint32 burst_min, burst_max;

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    burst_min = (min_quantum > 0) ?
          _bcm_td_default_burst_size(unit, port, min_quantum) : 0;

    burst_max = (max_quantum > 0) ?
          _bcm_td_default_burst_size(unit, port, max_quantum) : 0;

    return _bcm_td2_cosq_bucket_set(unit, port, cosq, min_quantum, max_quantum,
                                    burst_min, burst_max, flags);
}

/*
 * Function:
 *     bcm_td2_cosq_bandwidth_get
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
bcm_td2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{
    uint32 kbit_burst_min;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_bucket_get(unit, port, cosq,
                        min_quantum, max_quantum, &kbit_burst_min,
                        burst_quantum, flags));
    return BCM_E_NONE;
}

int
bcm_td2_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst, burst_min, flags;
    bcm_port_t  local_port =  port;
    
    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_td2_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_l2_gport(unit, port, cosq, 
                                            _BCM_TD2_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                    &burst, &flags));

    min = pps;
    burst_min = (min > 0) ?
          _bcm_td_default_burst_size(unit, local_port, min) : 0;
    burst = burst_min;        

    return _bcm_td2_cosq_bucket_set(unit, port, cosq, min, pps, burst_min, burst,
                                    flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_td2_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_td2_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_l2_gport(unit, port, cosq,
                                          _BCM_TD2_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                  &flags));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_td2_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst)
{
    uint32 min, max, cur_burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_td2_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_l2_gport(unit, port, cosq,
                                                _BCM_TD2_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst,
                                  &cur_burst, &flags));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_td2_cosq_bucket_set(unit, port, cosq, min, max, burst, burst,
                                    flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_td2_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst, cur_burst_min, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_td2_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_l2_gport(unit, port, cosq, 
                                            _BCM_TD2_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, port, cosq, &min, &max, 
                                  &cur_burst_min, &cur_burst, &flags));
    *burst = cur_burst;

    return BCM_E_NONE;
}

int
bcm_td2_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    _bcm_td2_mmu_info_t *mmu_info;

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }
    
    if (flags & ~(BCM_COSQ_DISCARD_TCP |
                  BCM_COSQ_DISCARD_NONTCP |
                  BCM_COSQ_DISCARD_COLOR_ALL |
                  BCM_COSQ_DISCARD_COLOR_GREEN |
                  BCM_COSQ_DISCARD_COLOR_YELLOW |
                  BCM_COSQ_DISCARD_COLOR_RED |
                  BCM_COSQ_DISCARD_CAP_AVERAGE |
                  BCM_COSQ_DISCARD_ENABLE |
                  BCM_COSQ_DISCARD_MARK_CONGESTION)) {
        return BCM_E_PARAM;
    }

    flags &= ~(BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL);

    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_wred_set(unit, port, 0, flags, 0, 0, 0, 0,
                                    FALSE, BCM_COSQ_DISCARD_PORT,8));
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_td2_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL, BCM_COSQ_DISCARD_PORT,NULL);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_td2_cosq_discard_port_set
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
bcm_td2_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval, bits, flags = 0;
    int numq, i;

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    /*
     * average queue size is reculated every 8us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     * gain = log2(average_time / 8)
     */
    bits = (average_time / 8) & 0xffff;
    if (bits != 0) {
        bits |= bits >> 1;
        bits |= bits >> 2;
        bits |= bits >> 4;
        bits |= bits >> 8;
        gain = _shr_popcount(bits) - 1;
    } else {
        gain = 0;
    }

    /*
     * per-port cell limit may be disabled.
     * per-port per-cos cell limit may be set to use dynamic method.
     * therefore drop_start percentage is based on per-device total shared
     * cells.
     */
    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
    shared_limit = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                     rval, SHARED_LIMITf);
    min_thresh = ((drop_start * shared_limit) + (100 - 1)) / 100;

    /* Calculate the max threshold. For a given slope (angle in
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_td2_angle_to_cells(drop_slope);
    if (max_thresh > TD2_CELL_FIELD_MAX) {
        max_thresh = TD2_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        numq = 1;
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_wred_set(unit, port, cosq + i, color,
                                       min_thresh, max_thresh, 100, gain,
                                       TRUE, flags,8));
        }
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }

        BCM_PBMP_ITER(pbmp, local_port) {
            numq = 1;
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_wred_set(unit, local_port, cosq + i,
                                           color, min_thresh, max_thresh, 100,
                                           gain, TRUE, 0,8));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_discard_port_get
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
bcm_td2_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
    int refresh_time = 0;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval;

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        local_port = port;
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
        BCM_PBMP_ITER(pbmp, local_port) {
            break;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                               &color, &min_thresh, &max_thresh, &drop_prob,
                               &gain, 0, &refresh_time));

    /*
     * average queue size is reculated every 8us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain) * refresh_time;

    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
    shared_limit = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                     rval, SHARED_LIMITf);
    if (min_thresh > shared_limit) {
        min_thresh = shared_limit;
    }

    if (max_thresh > shared_limit) {
        max_thresh = shared_limit;
    }

    *drop_start = min_thresh * 100 / shared_limit;

    /* Calculate the slope using the min and max threshold.
     * The angle is calculated knowing drop probability at min
     * threshold is 0% and drop probability at max threshold is 100%.
     */
    *drop_slope = _bcm_td2_cells_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_distribute_u64(uint64 value, uint64 *l, uint64 *r)
{
    uint64 tmp, tmp1;

    tmp = value;
    COMPILER_64_SHR(tmp, 1);
    *l = tmp;

    COMPILER_64_SET(tmp1, 0, 1);
    COMPILER_64_AND(tmp1, value);
    COMPILER_64_ADD_64(tmp1, tmp);
    *r = tmp1;
    return 0;
}

STATIC int
_port_l2c_mapping(int unit, bcm_port_t port, bcm_cosq_stat_t stat, int *xlp_port)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int i, xlp_blk, xlp_bindex = -1, pgw_blk, pgw_bindex = -1; 

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
        xlp_blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        xlp_bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
        if (SOC_BLOCK_INFO(unit, xlp_blk).type == SOC_BLK_XLPORT) {
            break;
        }
    }
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
        pgw_blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        pgw_bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
        if (SOC_BLOCK_INFO(unit, pgw_blk).type == SOC_BLK_PGW_CL) {
            break;
        }
    }
    *xlp_port = xlp_bindex;

    if(stat == bcmCosqStatHighPriDroppedPackets){
        switch (pgw_bindex % 16) {
        case 0:
        case 1:  
        case 2:
        case 3:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_HIGH_PRI;
        case 4:
        case 5:
        case 6:
        case 7:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_HIGH_PRI;
        case 8:
        case 9:  
        case 10:
        case 11:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_HIGH_PRI;
        case 12:
        case 13:  
        case 14:
        case 15:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_HIGH_PRI;
        default:
            return SOC_E_INTERNAL;
        }
    }
        
    if(stat == bcmCosqStatLowPriDroppedPackets){
        switch (pgw_bindex % 16) {
        case 0:
        case 1:  
        case 2:
        case 3:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_LOW_PRI;
        case 4:
        case 5:
        case 6:
        case 7:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_LOW_PRI;
        case 8:
        case 9:  
        case 10:
        case 11:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_LOW_PRI;
        case 12:
        case 13:  
        case 14:
        case 15:
            return SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_LOW_PRI;
        default:
            return SOC_E_INTERNAL;
        }
    }

    if(stat == bcmCosqStatHighPriDroppedBytes){
        switch (pgw_bindex % 16) {
        case 0:
        case 1:  
        case 2:
        case 3:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_HIGH_PRI;
        case 4:
        case 5:
        case 6:
        case 7:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_HIGH_PRI;
        case 8:
        case 9:  
        case 10:
        case 11:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_HIGH_PRI;
        case 12:
        case 13:  
        case 14:
        case 15:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_HIGH_PRI;
        default:
            return SOC_E_INTERNAL;
        }
    }

    if(stat == bcmCosqStatLowPriDroppedBytes){
        switch (pgw_bindex % 16) {
        case 0:
        case 1:  
        case 2:
        case 3:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_LOW_PRI;
        case 4:
        case 5:
        case 6:
        case 7:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_LOW_PRI;
        case 8:
        case 9:  
        case 10:
        case 11:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_LOW_PRI;
        case 12:
        case 13:  
        case 14:
        case 15:
            return SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_LOW_PRI;
        default:
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_INTERNAL;
}

int
bcm_td2_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i, from_cos, to_cos, ci;
    uint64  l, r;
    soc_counter_non_dma_id_t id;
    int xlp_port;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }
    _bcm_td2_distribute_u64(value, &l, &r);

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                         (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                          &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                      BCM_IF_ERROR_RETURN
                           (soc_counter_set(unit, local_port,
                                           SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                         startq + i, l));
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                         startq + i , r));
                }
            }
        }
        break;
    case bcmCosqStatDroppedBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                   SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, startq , value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                       (_bcm_td2_cosq_index_resolve
                        (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                         &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                       BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                         startq + i, l));
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                           SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                         startq + i , r));
                }
            }
        }
        break;
    case bcmCosqStatYellowCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0,
                             value));
        break;
    case bcmCosqStatGreenDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, 0,
                             value));
        break;
    case bcmCosqStatYellowDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (soc_counter_set(unit, local_port,
                                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                             startq + i, l));
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         startq + i, r));
                }
            }
        }
        break;
    case bcmCosqStatOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (soc_counter_set(unit, local_port,
                                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                             startq + i, l));
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         startq + i, r));
                }
            }
        }
        break;
    case bcmCosqStatUCOutPackets:
    case bcmCosqStatUCOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            soc_reg_t ctr_reg_uc;
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT :
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT;
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port, ctr_reg_uc, node->hw_index, value));
            } else {
                ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC :
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port, ctr_reg_uc, startq, value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            soc_reg_t ctr_reg_uc;
            ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (soc_counter_set(unit, local_port, ctr_reg_uc, startq + i, value));
                    }
                }
            }
        }
        break;
    case bcmCosqStatMCOutPackets:
    case bcmCosqStatMCOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            soc_reg_t ctr_reg_mc = (stat == bcmCosqStatMCOutPackets) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port, ctr_reg_mc, startq, value));
        } else {
            soc_reg_t ctr_reg_mc = (stat == bcmCosqStatMCOutPackets) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port, ctr_reg_mc, startq + i, value));
                }
            }
        }
        break;
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve
                (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                 &local_port, &startq, NULL));        
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM, startq, value));
        break;
    case bcmCosqStatHighPriDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatHighPriDroppedPackets,&xlp_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatLowPriDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatLowPriDroppedPackets,&xlp_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatHighPriDroppedBytes:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatHighPriDroppedBytes,&xlp_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatLowPriDroppedBytes:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatLowPriDroppedBytes,&xlp_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatSourcePortDroppedPackets:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        }

        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN(
            soc_counter_set(unit, local_port,
            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING, cosq, value));
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i, from_cos, to_cos, ci, start_hw_index, end_hw_index;
    uint64 sum, value64;
    uint32 sum32, value32;
    soc_counter_non_dma_id_t id;
    int xlp_port;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    int pool, midx, valid;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if ((value == NULL) || (cosq < -1)) {
        return BCM_E_PARAM;
    }

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;
    COMPILER_64_ZERO(*value);
    value32 = 0;

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                    SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, startq, value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                         BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                         startq + i, &value64));
                         COMPILER_64_ADD_64(sum, value64);
                    }
                }
                BCM_IF_ERROR_RETURN
                  (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));

                for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatDroppedBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                  SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, startq, value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                       (_bcm_td2_cosq_index_resolve
                       (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                        &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                     /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                          (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                         startq + i, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatYellowCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0,
                             value));
        break;
    case bcmCosqStatGreenDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, 0,
                             value));
        break;
    case bcmCosqStatYellowDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT, node->hw_index,
                                     value));

            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                             startq + i, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT, node->hw_index,
                                     value));

            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, startq,
                                     value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                            &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                             startq + i, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatUCOutPackets:
    case bcmCosqStatUCOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if ((node->type == _BCM_TD2_NODE_VM_UCAST) ||
                (node->type == _BCM_TD2_NODE_SERVICE_UCAST)) {
                soc_reg_t ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT :
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT;
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port, ctr_reg_uc, node->hw_index, value));

            } else {
                soc_reg_t ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC :
                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                      &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port, ctr_reg_uc, startq, value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            soc_reg_t ctr_reg_uc = (stat == bcmCosqStatUCOutPackets) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                            &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port, ctr_reg_uc, startq + i, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatMCOutPackets:
    case bcmCosqStatMCOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
          return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            soc_reg_t ctr_reg_mc = ( stat == bcmCosqStatMCOutPackets ) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port, ctr_reg_mc, startq, value));
        } else {
            soc_reg_t ctr_reg_mc = ( stat == bcmCosqStatMCOutPackets ) ?
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT :
                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port, ctr_reg_mc, startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve
                (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                 &local_port, &startq, NULL));  
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM, startq, value));
        break;
    case bcmCosqStatIngressPortPoolSharedBytesPeak:

        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_sp_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));

        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_POOL_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;
        break;
    case bcmCosqStatIngressPortPoolSharedBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_sp_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

        COMPILER_64_ZERO(sum);
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_POOL_CURRENT,
                                             i, &value64));
           COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;
        break;

    case bcmCosqStatIngressPortPGMinBytesPeak:

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_MIN_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
          }

        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;
        break;
    case bcmCosqStatIngressPortPGMinBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_MIN_CURRENT,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;
        break;     

    case bcmCosqStatIngressPortPGSharedBytesPeak:

        BCM_IF_ERROR_RETURN
           (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_SHARED_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;
        break;

    case bcmCosqStatIngressPortPGSharedBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                          SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatIngressPortPGHeadroomBytesPeak:

        BCM_IF_ERROR_RETURN
               (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                       SOC_COUNTER_NON_DMA_PG_HDRM_PEAK,
                                       i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatIngressPortPGHeadroomBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));

        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                            SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT,
                                i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatEgressMCQueueBytesPeak:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            if (node->level != SOC_TD2_NODE_LVL_ROOT) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_QUEUE_PEAK, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                             &local_port, &startq, &numq));
                    for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_QUEUE_PEAK,
                                             startq + i, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
            }
            COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
            *value = sum;
        }
        break;

    case bcmCosqStatEgressUCQueueBytesPeak:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            if (node->level != SOC_TD2_NODE_LVL_ROOT) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(
                   _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            if (IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PARAM;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                      (_bcm_td2_cosq_index_resolve
                            (unit, port, ci, _BCM_TD2_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                     BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK,
                                             startq + i, &value64));
                     COMPILER_64_ADD_64(sum, value64);
                }
            }
            COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
            *value = sum;
        }
        break;

    case bcmCosqStatHighPriDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatHighPriDroppedPackets,&xlp_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatLowPriDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatLowPriDroppedPackets,&xlp_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatHighPriDroppedBytes:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatHighPriDroppedBytes,&xlp_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatLowPriDroppedBytes:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (id = _port_l2c_mapping(unit,local_port,bcmCosqStatLowPriDroppedBytes,&xlp_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port, id, xlp_port, value));
        break;
    case bcmCosqStatGlobalHeadroomBytes0Peak:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes0Current:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes1Peak:
         BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_PEAK,
                            0, value));
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes1Current:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_CURRENT,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;

    case bcmCosqStatEgressPortPoolTotalBytesCurrent:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, MMU_THDM_DB_PORTSP_TOTAL_COUNTr, local_port, 0, &rval));
        value32 = soc_reg_field_get(unit, MMU_THDM_DB_PORTSP_TOTAL_COUNTr, rval, COUNTf);
        COMPILER_64_ADD_32(*value, value32);
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;

    case bcmCosqStatEgressPortPoolSharedBytesCurrent:
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                        _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                        &local_port, &pool, NULL));
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, MMU_THDM_DB_PORTSP_SHARED_COUNTr, local_port, pool, &rval));
        value32 = soc_reg_field_get(unit, MMU_THDM_DB_PORTSP_SHARED_COUNTr, rval, COUNTf);
        COMPILER_64_ADD_32(*value, value32);
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;

    case bcmCosqStatEgressMCQueueMinBytesCurrent:
    case bcmCosqStatEgressMCQueueBytesCurrent:
        if (stat == bcmCosqStatEgressMCQueueMinBytesCurrent) {
            field = MIN_COUNTf;
        } else if (stat == bcmCosqStatEgressMCQueueBytesCurrent) {
            field = SHARED_COUNTf;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));

            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_COUNT_0m,
                               MMU_THDM_DB_QUEUE_COUNT_1m);
            startq -= 1480; /* MC queues comes after UC queues */

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                     startq, entry));
            value32 = soc_mem_field32_get(unit, mem, entry,  field);
            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            sum32 =0;
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            if (local_port < 0) {
                return BCM_E_PORT;
            }

            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_COUNT_0m,
                               MMU_THDM_DB_QUEUE_COUNT_1m);

            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
                startq -= 1480; /* MC queues comes after UC queues */
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                         startq, entry));
                sum32 += soc_mem_field32_get(unit, mem, entry, field);
            }
            COMPILER_64_ADD_32(sum, sum32);
            COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
            *value = sum;
        }
        break;

    case bcmCosqStatEgressUCQueuePortBytesCurrent:
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UC_EGR_POOL,
                                         &local_port, &pool, NULL));

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_COUNTER_PORTm,
                   MMU_THDU_YPIPE_COUNTER_PORTm);

        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));
        value32 = soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);

        COMPILER_64_ADD_32(*value, value32);
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;

    case bcmCosqStatEgressQgroupMinBytesCurrent:
    case bcmCosqStatEgressQgroupBytesCurrent:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             NULL, &startq, NULL));
        }

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                           MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
        if (!valid) {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                     "UCQ doesn't associate with a queue group!\n")));
            return BCM_E_UNAVAIL;
        }

        startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_COUNTER_QGROUPm,
                           MMU_THDU_YPIPE_COUNTER_QGROUPm);
        if (stat == bcmCosqStatEgressQgroupMinBytesCurrent) {
            field = MIN_COUNTf;
        } else if (stat == bcmCosqStatEgressQgroupBytesCurrent) {
            field = SHARED_COUNTf;
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        value32 = soc_mem_field32_get(unit, mem, entry, field);

        COMPILER_64_ADD_32(*value, value32);
        COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        break;

    case bcmCosqStatEgressUCQueueMinBytesCurrent:
    case bcmCosqStatEgressUCQueueBytesCurrent:
        if (stat == bcmCosqStatEgressUCQueueMinBytesCurrent) {
            field = MIN_COUNTf;
        } else if (stat == bcmCosqStatEgressUCQueueBytesCurrent) {
            field = SHARED_COUNTf;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));

            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_COUNTER_QUEUEm,
                       MMU_THDU_YPIPE_COUNTER_QUEUEm);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                     startq, entry));
            value32 = soc_mem_field32_get(unit, mem, entry, field);
            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _BCM_TD2_BYTES_PER_CELL);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, port));

            if (IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PARAM;
            }
            COMPILER_64_ZERO(sum);
            sum32 =0;
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ?
                            NUM_CPU_COSQ(unit) - 1 : _TD2_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            if (local_port < 0) {
                return BCM_E_PORT;
            }
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_COUNTER_QUEUEm,
                               MMU_THDU_YPIPE_COUNTER_QUEUEm);
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                         startq, entry));
                sum32 += soc_mem_field32_get(unit, mem, entry, field);
            }
            COMPILER_64_ADD_32(sum, sum32);
            COMPILER_64_UMUL_32(sum, _BCM_TD2_BYTES_PER_CELL);
            *value = sum;
        }
        break;
    case bcmCosqStatSourcePortDroppedPackets:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        }

        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_td2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN(
            counter_get(unit, local_port,
            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING, cosq, value));
        break;

    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_cosq_quantize_table_set
 * Purpose:
 *     Generate quantized feedback lookup table
 * Parameters:
 *     unit          - (IN) unit number
 *     profile_index - (IN) profile index
 *     weight_code   - (IN) weight encoding
 *     set_point     - (IN) queue size set point
 *     active_offset - (OUT) most significant non-zero bit position
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     The formular to quantize feedback value is:
 *     quantized_feedbad = celling(feedback / max_feedback * 63)
 *         where max_feedback = (2 * cpW + 1) * cpQsp
 *         cpW is feedback weight
 *         cpQsp is queue size set point
 */
STATIC int
_bcm_td2_cosq_quantize_table_set(int unit, int profile_index, int weight_code,
                                int set_point, int *active_offset)
{
    int ref_count;
    int weight_x_4, feedback, max_feedback, quantized_feedback, shift;
    int base_index, index;

    /* weight = 2 ** (weight_code - 2) e.g. 0 for 1/4, 1 for 1/2, ...  */
    weight_x_4 = 1 << weight_code;
    max_feedback = ((2 * weight_x_4 + 4) * set_point + 3) / 4;
    shift = 0;
    while ((max_feedback >> shift) > 127) {
        shift++;
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_ref_count_get(unit, _bcm_td2_feedback_profile[unit],
                                       profile_index, &ref_count));

    /*
     * Hardware use lookup table to find quantized feedback in order to avoid
     * division, use most significant 7-bit of feedback value as lookup table
     * index to find quantized_feedback value
     */
    if (ref_count == 1) {
        base_index = profile_index << 7;
        for (index = 0; index < 128; index++) {
            feedback = index << shift;
            if (feedback > max_feedback) {
                quantized_feedback = 63;
            } else {
                quantized_feedback = (feedback * 63 + max_feedback - 1) /
                    max_feedback;
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, MMU_QCN_QFBTBm,
                                        base_index  + index, CPQ_QNTZFBf,
                                        quantized_feedback));
        }
    }

    *active_offset = shift + 6;

    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_sample_int_table_set(int unit, int min, int max,
                                  uint32 *profile_index)
{
    mmu_qcn_sitb_entry_t sitb_entries[64];
    void *entries[1];
    int i, j, center, inc, value, index;

    sal_memset(sitb_entries, 0, sizeof(sitb_entries));
    entries[0] = &sitb_entries;
    for (i = 0; i < 8; i++) {
        center = max - (max - min) * i / 7;
        inc = (max - min) / 7 / 8;
        for (j = 0; j < 8; j++, inc = -inc) {
            index = i * 8 + j;
            value = center + (j + 1) / 2 * inc;
            if (value > 255) {
                value = 255;
            } else if (value < 1) {
                value = 1;
            }
            soc_mem_field32_set(unit, MMU_QCN_SITBm, &sitb_entries[index],
                                CPQ_SIf, value);
        }
    }

    return soc_profile_mem_add(unit, _bcm_td2_sample_int_profile[unit], entries,
                               64, (uint32 *)profile_index);
}

/*
 * Function:
 *     bcm_td2_cosq_congestion_queue_set
 * Purpose:
 *     Enable/Disable a cos queue as congestion managed queue
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     index         - (IN) congestion managed queue index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_congestion_queue_set(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq, int index)
{
    bcm_port_t local_port;
    uint32 rval;
    uint64 rval64, *rval64s[1];
    mmu_qcn_enable_entry_t enable_entry;
    int active_offset, qindex;
    uint32 profile_index, sample_profile_index;
    int weight_code, set_point;
    soc_mem_t mem;
    int pipe;

    if (cosq < 0 || cosq >= _TD2_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }

    if ((index < -1) || (index >= 1480)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;    
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }
    

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                        qindex, &enable_entry));
    if (index == -1) {
        if (!soc_mem_field32_get(unit, mem, &enable_entry, CPQ_ENf)) {
            return BCM_E_NONE;
        }

        index = soc_mem_field32_get(unit, mem, &enable_entry, CPQ_PROFILE_INDEXf);
        soc_mem_field32_set(unit, mem, &enable_entry, CPQ_ENf, 0);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, qindex, &enable_entry));

        profile_index = soc_mem_field32_get(unit, mem, &enable_entry, EQTB_INDEXf);

        BCM_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, _bcm_td2_feedback_profile[unit],
                                    profile_index));

        profile_index =
            soc_mem_field32_get(unit, mem, &enable_entry, SITB_SELf);

        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_td2_sample_int_profile[unit],
                                profile_index * 64));
    } else {
        if (soc_mem_field32_get(unit, mem, &enable_entry, CPQ_ENf)) {
            return BCM_E_BUSY;
        }

        /* Use hardware reset value as default quantize setting */
        weight_code = 3;
        set_point = 0x96;
        rval = 0;
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPWf, weight_code);
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPQEQf, set_point);
        COMPILER_64_SET(rval64, 0, rval);
        rval64s[0] = &rval64;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_add(unit, _bcm_td2_feedback_profile[unit],
                                 rval64s, 1, (uint32 *)&profile_index));

        /* Update quantized feedback calculation lookup table */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_quantize_table_set(unit, profile_index, weight_code,
                                             set_point, &active_offset));

        /* Pick some sample interval */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_sample_int_table_set(unit, 13, 127,
                                               &sample_profile_index));

        soc_mem_field32_set(unit, mem, &enable_entry,
                            QNTZ_ACT_OFFSETf, active_offset);

        soc_mem_field32_set(unit, mem, &enable_entry,
                            SITB_SELf, sample_profile_index/64);

        soc_mem_field32_set(unit, mem, &enable_entry, EQTB_INDEXf, 
                            profile_index);

        soc_mem_field32_set(unit, mem, &enable_entry, CPQ_IDf, index);

        soc_mem_field32_set(unit, mem, &enable_entry, CPQ_ENf, 1);

        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, qindex, &enable_entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_congestion_queue_get
 * Purpose:
 *     Get congestion managed queue index of the specified cos queue
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     index         - (OUT) congestion managed queue index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq, int *index)
{
    bcm_port_t local_port;
    int qindex;
    mmu_qcn_enable_entry_t entry;
    soc_mem_t mem;
    int pipe;

    if (cosq < 0 || cosq >= _TD2_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }
    
    if (index == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }

    
    
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, qindex, &entry));
    if (soc_mem_field32_get(unit, mem, &entry, CPQ_ENf)) {
        *index = soc_mem_field32_get(unit, mem, &entry, CPQ_IDf);
    } else {
        *index = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_congestion_quantize_set
 * Purpose:
 *     Set quantized congestion feedback algorithm parameters
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     weight_code   - (IN) weight encoding (use -1 for unchange)
 *     set_point     - (IN) queue size set point (use -1 for unchange)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_congestion_quantize_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int weight_code,
                                    int set_point)
{
    bcm_port_t local_port;
    uint32 rval;
    uint64 rval64, *rval64s[1];
    mmu_qcn_enable_entry_t enable_entry;
    int cpq_index, active_offset, qindex;
    uint32 profile_index, old_profile_index;
    soc_mem_t mem;
    int pipe;

    if (cosq < 0 || cosq >= _TD2_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }

    BCM_IF_ERROR_RETURN
        (bcm_td2_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq, 
                _BCM_TD2_COSQ_INDEX_STYLE_QCN, &local_port, &qindex, NULL));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, qindex, &enable_entry));
    old_profile_index =
        soc_mem_field32_get(unit, mem, &enable_entry, EQTB_INDEXf);

    BCM_IF_ERROR_RETURN(READ_MMU_QCN_CPQ_SEQr(unit, old_profile_index, &rval));
    if (weight_code == -1) {
        weight_code = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPWf);
    } else {
        if (weight_code < 0 || weight_code > 7) {
            return BCM_E_PARAM;
        }
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPWf, weight_code);
    }
    if (set_point == -1) {
        set_point = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPQEQf);
    } else {
        if (set_point < 0 || set_point > 0xffff) {
            return BCM_E_PARAM;
        }
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPQEQf, set_point);
    }
    COMPILER_64_SET(rval64, 0, rval);
    rval64s[0] = &rval64;
    /* Add new feedback parameter profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_add(unit, _bcm_td2_feedback_profile[unit], rval64s,
                             1, (uint32 *)&profile_index));
    /* Delete original feedback parameter profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_td2_feedback_profile[unit],
                                old_profile_index));

    /* Update quantized feedback calculation lookup table */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_quantize_table_set(unit, profile_index, weight_code,
                                         set_point, &active_offset));

    soc_mem_field32_set(unit, mem, &enable_entry,
                            QNTZ_ACT_OFFSETf, active_offset);
    soc_mem_field32_set(unit, mem, &enable_entry,
                            EQTB_INDEXf, profile_index);
    BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ANY, qindex,
                           &enable_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_congestion_quantize_get
 * Purpose:
 *     Get quantized congestion feedback algorithm parameters
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     weight_code   - (OUT) weight encoding
 *     set_point     - (OUT) queue size set point
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *weight_code,
                                    int *set_point)
{
    bcm_port_t local_port;
    int cpq_index, profile_index, qindex;
    uint32 rval;
    mmu_qcn_enable_entry_t enable_entry;
    soc_mem_t mem;
    int pipe;

    BCM_IF_ERROR_RETURN
        (bcm_td2_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                     MEM_BLOCK_ANY, qindex, &enable_entry));

    profile_index =
        soc_mem_field32_get(unit, mem, &enable_entry, EQTB_INDEXf);

    BCM_IF_ERROR_RETURN(READ_MMU_QCN_CPQ_SEQr(unit, profile_index, &rval));
    if (weight_code != NULL) {
        *weight_code = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPWf);
    }
    if (set_point != NULL) {
        *set_point = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPQEQf);
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int min, int max)
{
    bcm_port_t local_port;
    mmu_qcn_enable_entry_t entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int qindex;
    uint32 profile_index, old_profile_index;
    soc_mem_t mem;
    int pipe;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }


    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, qindex, &entry));

    if (!soc_mem_field32_get(unit, mem, &entry, CPQ_ENf)) {
        return BCM_E_PARAM;
    }

    old_profile_index =
        soc_mem_field32_get(unit, mem, &entry, SITB_SELf);

    if (max == -1) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         old_profile_index * 64, &sitb_entry));
        max = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    } else {
        if (max < 1 || max > 255) {
            return BCM_E_PARAM;
        }
    }
    if (min == -1) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         old_profile_index * 64 + 63,
                                         &sitb_entry));
        min = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    } else {
        if (min < 1 || min > 255) {
            return BCM_E_PARAM;
        }
    }

    /* Add new sample interval profile */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_sample_int_table_set(unit, min, max, &profile_index));

    /* Delete original sample interval profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, _bcm_td2_sample_int_profile[unit],
                                old_profile_index * 64));

    soc_mem_field32_set(unit, mem, &entry, SITB_SELf, 
                        profile_index / 64);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                      qindex, &entry));
    return BCM_E_NONE;
}

int
bcm_td2_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *min, int *max)
{
    bcm_port_t local_port;
    mmu_qcn_enable_entry_t entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int qindex;
    uint32 old_profile_index;
    soc_mem_t mem;
    int pipe;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    if (pipe) {
        mem = MMU_QCN_ENABLE_1m;
    } else {
        mem = MMU_QCN_ENABLE_0m;
    }
    
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, qindex, &entry));

    if (!soc_mem_field32_get(unit, mem, &entry, CPQ_ENf)) {
        return BCM_E_PARAM;
    }

    old_profile_index =
    soc_mem_field32_get(unit, mem, &entry, SITB_SELf);

    if (max) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                     old_profile_index * 64, &sitb_entry));
        *max = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    }

    if (min) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                     old_profile_index * 64 + 63,
                                     &sitb_entry));
        *min = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ing_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
        bcm_cosq_control_t type, int arg)
{
    int local_port;
    uint32 rval;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    if(( arg < 0 ) || (arg > 3 )) {
            return BCM_E_PARAM;
    }
      
    BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }
    /* coverity fix : pri_grp can take value from 0 to 7 */  
    if ((pri_grp < 0) || (pri_grp >= 8)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, local_port, &rval));
    soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval, 
                                prigroup_spid_field[pri_grp],arg);
    BCM_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, local_port, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ing_pool_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t pri_grp,
        bcm_cosq_control_t type, int *arg)
{
    int pool_start_idx = 0, pool_end_idx =0;
    BCM_IF_ERROR_RETURN(
            _bcm_td2_cosq_ingress_sp_get(
                unit, gport, pri_grp, &pool_start_idx, &pool_end_idx));
    *arg = pool_start_idx;
    return BCM_E_NONE;
}

 /*
 * Function:
 *     _bcm_td2_cosq_port_qnum_get
 * Purpose:
 *     Get the hw queue number used to tx packets on respective Queue
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     type          - (IN) unicast/multicast queue indicator
 *     arg           - (out) hw queue number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_cosq_port_qnum_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int hw_index;
    int uc;
    int qnum;
    int id, startq, numq;
    _bcm_td2_cosq_node_t *node;
    soc_info_t *si = &SOC_INFO(unit);

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (cosq <= -1) {
        return BCM_E_PARAM;
    } else {
        startq = cosq;
        numq = 1;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, cosq, NULL, &local_port, &id,
                                   &node));
        if (node->attached_to_input < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }

        startq = 0;
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = (IS_CPU_PORT(unit, local_port)) ?
                                NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    if (startq >= numq) {
        return BCM_E_PARAM;
    }

    uc =(type == bcmCosqControlPortQueueUcast) ? 1 : 0;

    if (node) {
        hw_index = node->hw_index;
    } else {
        hw_index = soc_td2_l2_hw_index(unit,
            si->port_uc_cosq_base[local_port] + startq, uc);
    }

    qnum = soc_td2_hw_index_logical_num(unit,local_port,hw_index,uc);
    *arg = soc_td2_logical_qnum_hw_qnum(unit,local_port,qnum,uc);

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, pool, midx;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;

    if ((arg < 0) || (cosq < 0)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_PORTSP_CONFIG_0m, 
                                         MMU_THDM_DB_PORTSP_CONFIG_1m);

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            midx, entry));

        soc_mem_field32_set(unit, mem, entry, SHARED_LIMIT_ENABLEf, !!arg);

        return soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry);
    }
    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        if(( arg < 0 ) || (arg > 3 )) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            mem = SOC_TD2_PMEM(unit, local_port, 
                    MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            startq -= 1480; /* MC queues comes after UC queues */
            mem = SOC_TD2_PMEM(unit, local_port, 
                    MMU_THDM_MCQE_QUEUE_CONFIG_0m, MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            mem2 = SOC_TD2_PMEM(unit, local_port, 
                    MMU_THDM_DB_QUEUE_CONFIG_0m, MMU_THDM_DB_QUEUE_CONFIG_1m);
        } else  { /* local port */
            if (BCM_GPORT_IS_SCHEDULER(gport) &&
                (type != bcmCosqControlEgressPool)) {
                return BCM_E_PARAM;
            }
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
                mem = SOC_TD2_PMEM(unit, local_port, 
                                   MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, 
                                   MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
                startq -= 1480; /* MC queues comes after UC queues */
                mem = SOC_TD2_PMEM(unit, local_port, 
                                   MMU_THDM_MCQE_QUEUE_CONFIG_0m, 
                                   MMU_THDM_MCQE_QUEUE_CONFIG_1m);
                mem2 = SOC_TD2_PMEM(unit, local_port, 
                                    MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                    MMU_THDM_DB_QUEUE_CONFIG_1m);
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_SPIDf, arg);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        if (mem2 != INVALIDm) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
            soc_mem_field32_set(unit, mem2, entry2, Q_SPIDf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
        }
        return BCM_E_NONE;
    }

    arg = arg / _BCM_TD2_BYTES_PER_CELL;

    /* per service pool settings */
    if (type == bcmCosqControlEgressPoolSharedLimitBytes || 
        type == bcmCosqControlEgressPoolResumeLimitBytes ||
        type == bcmCosqControlEgressPoolYellowSharedLimitBytes || 
        type == bcmCosqControlEgressPoolYellowResumeLimitBytes ||
        type == bcmCosqControlEgressPoolRedSharedLimitBytes ||
        type == bcmCosqControlEgressPoolRedResumeLimitBytes) {
        uint32 rval;
        soc_reg_t reg;
        switch (type) {
        case bcmCosqControlEgressPoolSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_SHARED_LIMITr;    
            fld_limit = SHARED_LIMITf;
            granularity = 1;
            break;
        case bcmCosqControlEgressPoolResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_RESUME_LIMITr;    
            fld_limit = RESUME_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;    
            fld_limit = YELLOW_SHARED_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;    
            fld_limit = YELLOW_RESUME_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;    
            fld_limit = RED_SHARED_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;    
            fld_limit = RED_RESUME_LIMITf;
            granularity = 8;
            break;
        /*
         * COVERITY
         *
         * This default is unreachable but needed for some compiler.
         */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
        }
        
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, pool, &rval));
        
        /* Check for min/max values */
        max_val = (1 << soc_reg_field_length(unit, reg, fld_limit)) - 1;
        if ((arg < 0) || ((arg/granularity) > max_val)) {
            return BCM_E_PARAM;
        } else {
            soc_reg_field_set(unit, reg, &rval, fld_limit, (arg/granularity));
        }
        
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, pool, rval));

        return BCM_E_NONE;
    }

    /* per port settings */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);

        mem2 = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, 
                            MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_PORTSP_CONFIG_0m, 
                           MMU_THDM_DB_PORTSP_CONFIG_1m);
        startq = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if (mem2 != INVALIDm) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
    }

    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            fld_limit = Q_SHARED_LIMIT_CELLf;

            if (mem2 != INVALIDm) {
                soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            fld_limit = Q_SHARED_LIMITf;
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, 1);
        } else {
            fld_limit = SHARED_LIMITf;
            soc_mem_field32_set(unit, mem, entry, SHARED_LIMIT_ENABLEf, 1);
        }
        granularity = 1;
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            fld_limit = LIMIT_YELLOW_CELLf;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            fld_limit = YELLOW_SHARED_LIMITf;
        } else {
            fld_limit = YELLOW_SHARED_LIMITf;
        }
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            fld_limit = LIMIT_RED_CELLf;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            fld_limit = RED_SHARED_LIMITf;
        } else {
            fld_limit = RED_SHARED_LIMITf;
        }
        granularity = 8;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if (mem2 != INVALIDm) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
    }
    return BCM_E_NONE;
}



STATIC int
_bcm_td2_cosq_egr_port_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int pool, midx;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));
    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_PORTm,
                                         MMU_THDU_YPIPE_CONFIG_PORTm);
    midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
        fld_limit = YELLOW_LIMITf;
        granularity = 8;
        break;
    case bcmCosqControlEgressPortPoolRedLimitBytes:
        fld_limit = RED_LIMITf;
        granularity = 8;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    arg = arg / _BCM_TD2_BYTES_PER_CELL;

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));

    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, pool, midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int granularity = 1;

    if ((!arg) || (cosq < 0)){
        return BCM_E_PARAM;
    }
    
    if (type == bcmCosqControlUCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UC_EGR_POOL,
                                         &local_port, &pool, NULL));
        *arg = pool;
        return BCM_E_NONE;
    } else {
        if (type == bcmCosqControlMCEgressPool) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_SCHEDULER(gport)) {
                return BCM_E_PARAM;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                         &local_port, &pool, NULL));
        if (type == bcmCosqControlMCEgressPool ||
            type == bcmCosqControlEgressPool) {
            *arg = pool;
            return BCM_E_NONE;
        }
    }
    
    /* per service pool settings */
    if (type == bcmCosqControlEgressPoolSharedLimitBytes || 
        type == bcmCosqControlEgressPoolResumeLimitBytes ||
        type == bcmCosqControlEgressPoolYellowSharedLimitBytes || 
        type == bcmCosqControlEgressPoolYellowResumeLimitBytes ||
        type == bcmCosqControlEgressPoolRedSharedLimitBytes ||
        type == bcmCosqControlEgressPoolRedResumeLimitBytes) {
        uint32 rval;
        soc_reg_t reg;
        soc_field_t fld_limit = INVALIDf;
        switch (type) {
        case bcmCosqControlEgressPoolSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_SHARED_LIMITr;    
            fld_limit = SHARED_LIMITf;
            granularity = 1;
            break;
        case bcmCosqControlEgressPoolResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_RESUME_LIMITr;    
            fld_limit = RESUME_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;    
            fld_limit = YELLOW_SHARED_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr;    
            fld_limit = YELLOW_RESUME_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
            reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;    
            fld_limit = RED_SHARED_LIMITf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
            reg = MMU_THDM_DB_POOL_RED_RESUME_LIMITr;    
            fld_limit = RED_RESUME_LIMITf;
            granularity = 8;
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, pool, &rval));

        *arg = soc_reg_field_get(unit, reg, rval, fld_limit);
        *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;

        return BCM_E_NONE;
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_PORTSP_CONFIG_0m, 
                                         MMU_THDM_DB_PORTSP_CONFIG_1m);

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));

        *arg = soc_mem_field32_get(unit, mem, entry, SHARED_LIMIT_ENABLEf);
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_PORTSP_CONFIG_0m,
                           MMU_THDM_DB_PORTSP_CONFIG_1m);
        startq = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
    }
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                             startq, entry));

    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMIT_CELLf);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMITf);
        } else {
            *arg = soc_mem_field32_get(unit, mem, entry, SHARED_LIMITf);
        }
        granularity = 1;
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_YELLOW_CELLf);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, YELLOW_SHARED_LIMITf);
        } else {
            *arg = soc_mem_field32_get(unit, mem, entry, YELLOW_SHARED_LIMITf);
        }
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_RED_CELLf);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            *arg = soc_mem_field32_get(unit, mem, entry, RED_SHARED_LIMITf);
        } else {
            *arg = soc_mem_field32_get(unit, mem, entry, RED_SHARED_LIMITf);
        }
        granularity = 8;
        break;
    default:
        return BCM_E_UNAVAIL;
    }
    *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;
    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_egr_port_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int granularity = 1;
    int pool, midx;
    
    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TD2_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));
    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_PORTm,
                                         MMU_THDU_YPIPE_CONFIG_PORTm);
    midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
        *arg = soc_mem_field32_get(unit, mem, entry, YELLOW_LIMITf);
        granularity = 8;
        break;
    case bcmCosqControlEgressPortPoolRedLimitBytes:
		*arg = soc_mem_field32_get(unit, mem, entry, RED_LIMITf);
        granularity = 8;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

	
    *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;
    return BCM_E_NONE;
}



STATIC int
_bcm_td2_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 max_val, rval = 0;
    uint32 shared_size, spid;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    soc_field_t fld_sp = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;
    int cur_val, rv, post_update;
    int limit;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg /= _BCM_TD2_BYTES_PER_CELL;

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);

        mem2 = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, 
                            MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        fld_sp = Q_SPIDf;
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        mem2 = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                            MMU_THDM_MCQE_QUEUE_CONFIG_1m);
        fld_sp = Q_SPIDf;
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));

    switch (type) {
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;

        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);

        /* Set MMU_THDU_XPIPE_Q_TO_QGRP_MAP.DISABLE_QUEUING to 1 when both MIN
         * and SHARED LIMIT are 0. Set it to 0 when either MIN or SHARED LIMIT
         * is non-zero.
         */
        limit = soc_mem_field32_get(unit, mem, entry, Q_MIN_LIMIT_CELLf);
        if (arg == 0 && limit == 0) {
            soc_mem_field32_set(unit, mem2, entry2, DISABLE_QUEUINGf, 1);
        } else if (arg != 0 || limit != 0) {
            soc_mem_field32_set(unit, mem2, entry2, DISABLE_QUEUINGf, 0);
        }
        spid = soc_mem_field32_get(unit, mem2, entry2, fld_sp);
        granularity = 1;
        break;
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMITf;
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, 1);

        /* Config LIMIT in THDO_MC_QE: in 4-enties granularity */
        granularity = 4;
        soc_mem_field32_set(unit, mem2, entry2, fld_limit, (arg/granularity));
        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);
        spid = soc_mem_field32_get(unit, mem, entry, fld_sp);
        granularity = 1;
        break;
    case bcmCosqControlEgressUCQueueMinLimitBytes:
        fld_limit = Q_MIN_LIMIT_CELLf;

        /* Set MMU_THDU_XPIPE_Q_TO_QGRP_MAP.DISABLE_QUEUING to 1 when both MIN
                * and SHARED LIMIT are 0. Set it to 0 when either MIN or SHARED LIMIT is non-zero
                */
        limit = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMIT_CELLf);
        if (arg == 0 && limit == 0) {
            soc_mem_field32_set(unit, mem2, entry2, DISABLE_QUEUINGf, 1);
        } else if (arg != 0 || limit != 0) {
            soc_mem_field32_set(unit, mem2, entry2, DISABLE_QUEUINGf, 0);
        }
        spid = soc_mem_field32_get(unit, mem2, entry2, fld_sp);
        granularity = 1;
        break;
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        fld_limit = Q_MIN_LIMITf;

        /* Config LIMIT in THDO_MC_QE: in 4-enties granularity */
        granularity = 4;
        soc_mem_field32_set(unit, mem2, entry2, fld_limit, (arg/granularity));
        spid = soc_mem_field32_get(unit, mem, entry, fld_sp);
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));


    /* Recalculate Shared Values if Min Changed */
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, spid, &rval));
    shared_size = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                    rval, SHARED_LIMITf);
    cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);
        
    if ((arg / granularity) > cur_val) {
        /* New Min Val > Cur Min Val
         * So reduce the Shared values first and then 
         * Increase the Min Value for given resource
         */
        post_update = 0;
    } else if ((arg / granularity) < cur_val) {
        /* New Min Val < Cur Min Val
         * So reduce the Min values first and then 
         * Increase Shared values everywhere
         */
        post_update = 1;
    } else {
        return BCM_E_NONE;
    }
 
    if (!post_update &&
        ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
         (type == bcmCosqControlEgressMCQueueMinLimitBytes))) {
        int delta = 0; /* In Cells */
        delta = ((arg / granularity) - cur_val) * granularity;
        if (delta > shared_size) {
            return BCM_E_RESOURCE;
        }
        rv = soc_td2_mmu_config_res_limits_update(unit, spid,
                                                  shared_size - delta, 1);
        if (rv < 0) { 
            return rv;
        }
    }


    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        /* Get the latest entry data and update. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                         startq, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    if (post_update && 
        ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
         (type == bcmCosqControlEgressMCQueueMinLimitBytes))) {
        int delta = 0;
        delta = (cur_val - (arg / granularity)) * granularity;
        rv = soc_td2_mmu_config_res_limits_update(unit, spid,
                                                  shared_size + delta, 0);
        if (rv < 0) { 
            return rv;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                             startq, entry));

    switch (type) {
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
        break;
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlEgressUCQueueMinLimitBytes:
        fld_limit = Q_MIN_LIMIT_CELLf;
        granularity = 1;
        break;
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        fld_limit = Q_MIN_LIMITf;
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_mc_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    if ((type == bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueSharedLimitEntries)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, gport, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, &local_port, &startq,
                NULL));
        } else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN(
                    _bcm_td2_cosq_index_resolve(unit, local_port, ci,
                    _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, NULL, &startq,
                    NULL));
            }
        }

        if (type == bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes) {
            arg /= _BCM_TD2_BYTES_PER_CELL;
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            granularity = 1;
        } else {
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            granularity = 4;
        }
        fld_limit = Q_SHARED_LIMITf;
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        return BCM_E_PARAM;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, 1);
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_mc_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if ((type == bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueSharedLimitEntries)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, gport, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, &local_port, &startq,
                NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN(
                    _bcm_td2_cosq_index_resolve(unit, local_port, ci,
                    _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, NULL, &startq,
                    NULL));
            }
        }
        if (type == bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes) {
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            granularity = 1;
        } else {
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            granularity = 4;
        }
        fld_limit = Q_SHARED_LIMITf;
        startq -= 1480; /* MC queues comes after UC queues */
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity;
    if (type == bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes) {
        *arg = (*arg) * _BCM_TD2_BYTES_PER_CELL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 max_val, rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    soc_reg_t reg = INVALIDr;
    int port_pg, port_pg_pool, granularity = 1;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (cosq > 15 || cosq < 0) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg /= _BCM_TD2_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);

 
    if ((type == bcmCosqControlIngressPortPGSharedLimitBytes) ||
        (type == bcmCosqControlIngressPortPGMinLimitBytes)||
        (type == bcmCosqControlIngressPortPGHeadroomLimitBytes)||
        (type == bcmCosqControlIngressPortPGResetOffsetBytes)||
        (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
        
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);

        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_SP_CONFIG_Xm,
                                             THDI_PORT_SP_CONFIG_Ym);
        /* get index for Port-SP */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg_pool);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (midx < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlIngressPortPGSharedLimitBytes:
        fld_limit = PG_SHARED_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
        fld_limit = PORT_SP_MAX_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGMinLimitBytes:
        fld_limit = PG_MIN_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPoolMinLimitBytes:
        fld_limit = PORT_SP_MIN_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        fld_limit = PG_HDRM_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGResetFloorBytes:
        fld_limit = PG_RESET_FLOORf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGResetOffsetBytes:
        fld_limit = PG_RESET_OFFSETf;
        granularity = 1;
        break;

    /* coverity[dead_error_begin] */
    default:
        return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));

    return BCM_E_NONE;
}

/*
 * This function is used to get the
 * enable status of ingress PG or SP
 */
STATIC int
_bcm_td2_cosq_ing_res_enable_get(int unit, bcm_gport_t gport,
                          bcm_cos_queue_t cosq, bcm_cosq_control_t type,
                          int *arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 rval;
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_en = INVALIDf;
    int port_pg;
    soc_reg_t reg = INVALIDr;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };

    if (cosq < 0 || cosq >= _TD2_NUM_COS(unit)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg == NULL) {
        return BCM_E_PARAM;
    }
    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }


    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);


    if (type == bcmCosqControlIngressPortPGGlobalHeadroomEnable) {
          mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (midx < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            fld_en = PG_GBL_HDRM_ENf;
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, fld_en);

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ing_res_enable_set(int unit, bcm_gport_t gport,
                          bcm_cos_queue_t cosq, bcm_cosq_control_t type,
                          int arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 max_val, rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_en = INVALIDf;
    int port_pg;
    soc_reg_t reg = INVALIDr;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };

    if (cosq < 0 || cosq >= _TD2_NUM_COS(unit)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }
    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }


    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);

    if (type == bcmCosqControlIngressPortPGGlobalHeadroomEnable) {
          mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (midx < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            fld_en = PG_GBL_HDRM_ENf;
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_en)) - 1;
    if ((arg < 0) || (arg > max_val)) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPortPGGlobalHeadroomEnable) {
        soc_mem_field32_set(unit, mem, entry, fld_en, arg ? 1 : 0);
        /* coverity[negative_returns] */
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_state_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    uint32 rval, state , mask = 1 , bitno = 0;
    static const soc_field_t pool_field[] = {
        POOL_0f, POOL_1f, POOL_2f, POOL_3f }; 
    int start_pool_idx = 0,end_pool_idx = 0;    
    if (!arg) {
        return BCM_E_PARAM;
    }
    switch (type) {
        case bcmCosqControlSPPortLimitState:
        case bcmCosqControlPoolRedDropState:
        case bcmCosqControlPoolYellowDropState:
        case bcmCosqControlPoolGreenDropState:
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

            if (!SOC_PORT_VALID(unit, local_port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN (_bcm_td2_cosq_egress_sp_get(unit, gport, cosq, &start_pool_idx, 
                        &end_pool_idx)); 
            bitno = start_pool_idx; 
            break;
   
        case bcmCosqControlPGPortLimitState:
        case bcmCosqControlPGPortXoffState:
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

            if (!SOC_PORT_VALID(unit, local_port)) {
                return BCM_E_PORT;
            }
            if ((cosq < 0 ) || (cosq > 7)) {
                return BCM_E_PARAM;
            }
            bitno = cosq;
            break;
        default:
            return BCM_E_PARAM;
    }
    switch (type) {
        case bcmCosqControlSPPortLimitState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_PORT_LIMIT_STATESr(unit, local_port, &rval));
            state = soc_reg_field_get(unit, THDI_PORT_LIMIT_STATESr, rval,
                    SP_LIMIT_STATEf);
            break;
        case bcmCosqControlPGPortLimitState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_PORT_LIMIT_STATESr(unit, local_port, &rval));
            state = soc_reg_field_get(unit, THDI_PORT_LIMIT_STATESr, rval,
                    PG_LIMIT_STATEf);
            break;
        case bcmCosqControlPGPortXoffState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_FLOW_CONTROL_XOFF_STATEr(unit, local_port, &rval));
            state = soc_reg_field_get(unit, THDI_FLOW_CONTROL_XOFF_STATEr, rval,
                    PG_BMPf);
            break;
        case bcmCosqControlPoolRedDropState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_POOL_DROP_STATEr(unit, &rval));
            state = soc_reg_field_get(unit, THDI_POOL_DROP_STATEr, rval,
                    pool_field[start_pool_idx]);
            bitno = 2;
            break;
        case bcmCosqControlPoolYellowDropState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_POOL_DROP_STATEr(unit, &rval));
            state = soc_reg_field_get(unit, THDI_POOL_DROP_STATEr, rval,
                    pool_field[start_pool_idx]);
            bitno = 1;
            break;
        case bcmCosqControlPoolGreenDropState:
            SOC_IF_ERROR_RETURN(
                    READ_THDI_POOL_DROP_STATEr(unit, &rval));
            state = soc_reg_field_get(unit, THDI_POOL_DROP_STATEr, rval,
                    pool_field[start_pool_idx]);
            bitno = 0;
            break;
        /*
         * COVERITY
         *
         * This default is unreachable but needed for some compiler. 
         */ 
        /* coverity[dead_error_begin] */
        default :
            return BCM_E_PARAM;
    }
    mask = mask<<bitno; 
    state = state & mask ;
    if (state) {
        *arg = 1 ;
    } else {
        *arg = 0 ;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    soc_reg_t reg = INVALIDr;
    int midx, port_pg, port_pg_pool, granularity = 1;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (cosq > 15 || cosq < 0) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }
    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);

 
    if ((type == bcmCosqControlIngressPortPGSharedLimitBytes) ||
        (type == bcmCosqControlIngressPortPGMinLimitBytes)||
        (type == bcmCosqControlIngressPortPGHeadroomLimitBytes)||
        (type == bcmCosqControlIngressPortPGResetOffsetBytes)||
        (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);

        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_SP_CONFIG_Xm,
                                             THDI_PORT_SP_CONFIG_Ym);
        /* get index for Port-SP */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg_pool);
    } else {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlIngressPortPGSharedLimitBytes:
        fld_limit = PG_SHARED_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
        fld_limit = PORT_SP_MAX_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGMinLimitBytes:
        fld_limit = PG_MIN_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPoolMinLimitBytes:
        fld_limit = PORT_SP_MIN_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        fld_limit = PG_HDRM_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGResetFloorBytes:
        fld_limit = PG_RESET_FLOORf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGResetOffsetBytes:
        fld_limit = PG_RESET_OFFSETf;
        granularity = 1;
        break;
    /* coverity[dead_error_begin] */
    default:
        return BCM_E_UNAVAIL;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;
    return BCM_E_NONE;
}

/*
 * This function is used to get ingress pool
 * limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_td2_cosq_ing_res_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    uint32 rval;
    soc_reg_t reg = INVALIDr;
    int port_pg, port_pg_pool;
    static const soc_reg_t prigroup_reg[] = {
       THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
       PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
       PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
       PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
       PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
       PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
       PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (cosq > 15 || cosq < 0) {
       /* Error. Input pri > Max Pri_Grp */
       return BCM_E_PARAM;
    }
    if (!arg) {
       return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
       (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
       return BCM_E_PORT;
    }
    
    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
       (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);


    if (type == bcmCosqControlIngressPoolLimitBytes) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);

        SOC_IF_ERROR_RETURN
            (READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, port_pg_pool, &rval));
        *arg = soc_reg_field_get(unit, THDI_BUFFER_CELL_LIMIT_SPr, rval, LIMITf);
    } else {
        return BCM_E_PARAM;
    }

    *arg = (*arg) * _BCM_TD2_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * This function is used to set ingress pool
 * limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_td2_cosq_ing_res_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    uint32 max_val, rval;
    soc_reg_t reg = INVALIDr;
    int port_pg, port_pg_pool;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (cosq > 15 || cosq < 0) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg /= _BCM_TD2_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* get PG for port using Port+Cos */
    if (cosq < 8) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);

    if (type == bcmCosqControlIngressPoolLimitBytes) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);
        
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, port_pg_pool, &rval));
        /* Check for min/max values */
        max_val = (1 << soc_reg_field_length(unit, THDI_BUFFER_CELL_LIMIT_SPr, LIMITf)) - 1;
        if ((arg < 0) || (arg > max_val)) {
            return BCM_E_PARAM;
        } else {
            soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, arg);
        }
        
        SOC_IF_ERROR_RETURN(WRITE_THDI_BUFFER_CELL_LIMIT_SPr(unit, port_pg_pool, rval));
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_ef_op_at_index(int unit, int port, int hw_index, _bcm_cosq_op_t op, 
                             int *ef_val)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_trident2_sched_type_t sched_type;
    soc_mem_t mem;

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_LLS) {
        mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, SOC_TD2_NODE_LVL_L2);
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));

        if (op == _BCM_COSQ_OP_GET) {
            *ef_val = soc_mem_field32_get(unit, mem, &entry, C_EFf);
        } else {
            soc_mem_field32_set(unit, mem, &entry, C_EFf, *ef_val);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, 
                                hw_index, &entry));
        }
    } else {
        return BCM_E_UNAVAIL;
    }
   
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_ef_op(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, int *arg,
                    _bcm_cosq_op_t op)
{
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    int index, numq, from_cos, to_cos, ci;

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                    &local_port, NULL, &node));
        BCM_IF_ERROR_RETURN(
               _bcm_td2_cosq_child_node_at_input(node, cosq, &node));
        gport = node->gport;
        cosq = 0;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve
             (unit, gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
              &local_port, &index, NULL));
        return _bcm_td2_cosq_ef_op_at_index(unit, local_port, index, op, arg);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve
             (unit, gport, cosq, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
              &local_port, &index, NULL));
        return _bcm_td2_cosq_ef_op_at_index(unit, local_port, index, op, arg);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_port_has_ets(unit, local_port));
        if (cosq == BCM_COS_INVALID) {
            from_cos = 0;
            to_cos = _TD2_NUM_COS(unit) - 1;
        } else {
            from_cos = to_cos = cosq;
        }
        for (ci = from_cos; ci <= to_cos; ci++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, gport, ci, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &local_port, &index, &numq));

            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_ef_op_at_index(unit, 
                                local_port, index, op, arg));
            if (op == _BCM_COSQ_OP_GET) {
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, gport, ci, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &index, &numq));

            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_ef_op_at_index(unit, 
                                local_port, index, op, arg));
        }
    }
    return BCM_E_NONE;
}
    
STATIC int
_bcm_td2_cosq_qcn_proxy_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcm_cosq_control_t type, int arg)
{
    int local_port;
    uint32 rval;
    
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    
    BCM_IF_ERROR_RETURN(READ_QCN_CNM_PRP_CTRLr(unit, local_port, &rval));
    soc_reg_field_set(unit, QCN_CNM_PRP_CTRLr, &rval, ENABLEf, !!arg);
    soc_reg_field_set(unit, QCN_CNM_PRP_CTRLr, &rval, PRIORITY_BMAPf, 0xff);
    BCM_IF_ERROR_RETURN(WRITE_QCN_CNM_PRP_CTRLr(unit, local_port, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qcn_proxy_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int *arg)
{
    int local_port;
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));

    BCM_IF_ERROR_RETURN(READ_QCN_CNM_PRP_CTRLr(unit, local_port, &rval));
    *arg = soc_reg_field_get(unit, QCN_CNM_PRP_CTRLr, rval, ENABLEf);

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_alpha_bcm_to_soc(int unit,
    bcm_cosq_control_drop_limit_alpha_value_t arg,
    soc_td2_drop_limit_alpha_value_t *val)
{
    if (!val) {
        return BCM_E_PARAM;
    }

    switch(arg) {
    case bcmCosqControlDropLimitAlpha_1_128:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_128;
        break;
    case bcmCosqControlDropLimitAlpha_1_64:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_64;
        break;
    case bcmCosqControlDropLimitAlpha_1_32:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_32;
        break;
    case bcmCosqControlDropLimitAlpha_1_16:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_16;
        break;
    case bcmCosqControlDropLimitAlpha_1_8:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_8;
        break;
    case bcmCosqControlDropLimitAlpha_1_4:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_4;
        break;
    case bcmCosqControlDropLimitAlpha_1_2:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_2;
        break;
    case bcmCosqControlDropLimitAlpha_1:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1;
        break;
    case bcmCosqControlDropLimitAlpha_2:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_2;
        break;
    case bcmCosqControlDropLimitAlpha_4:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_4;
        break;
    case bcmCosqControlDropLimitAlpha_8:
        *val = SOC_TD2_COSQ_DROP_LIMIT_ALPHA_8;
        break;
    default:
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_alpha_soc_to_bcm(int unit,
    soc_td2_drop_limit_alpha_value_t arg,
    bcm_cosq_control_drop_limit_alpha_value_t *val)
{
    if (!val) {
        return BCM_E_PARAM;
    }

    switch(arg) {
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_128:
        *val = bcmCosqControlDropLimitAlpha_1_128;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_64:
        *val = bcmCosqControlDropLimitAlpha_1_64;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_32:
        *val = bcmCosqControlDropLimitAlpha_1_32;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_16:
        *val = bcmCosqControlDropLimitAlpha_1_16;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_8:
        *val = bcmCosqControlDropLimitAlpha_1_8;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_4:
        *val = bcmCosqControlDropLimitAlpha_1_4;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1_2:
        *val = bcmCosqControlDropLimitAlpha_1_2;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_1:
        *val = bcmCosqControlDropLimitAlpha_1;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_2:
        *val = bcmCosqControlDropLimitAlpha_2;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_4:
        *val = bcmCosqControlDropLimitAlpha_4;
        break;
    case SOC_TD2_COSQ_DROP_LIMIT_ALPHA_8:
        *val = bcmCosqControlDropLimitAlpha_8;
        break;
    default:
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_alpha_set(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int startq;
    int midx;
    uint32 rval, alpha;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    int dynamic_thresh_mode;
    soc_reg_t reg = INVALIDr;
    int port_pg;
    soc_td2_drop_limit_alpha_value_t aget;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };      

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_alpha_bcm_to_soc(unit, arg, &aget));
    alpha = (uint32)aget;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        } 
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm, 
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_SHARED_ALPHA_CELLf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));                 
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        mem2 = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m, 
                                           MMU_THDM_MCQE_QUEUE_CONFIG_1m);

        startq -= 1480; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_SHARED_ALPHAf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));    

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
        soc_mem_field32_set(unit, mem2, entry2, Q_SHARED_ALPHAf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));              
    } else {
        if (cosq > 15 || cosq < 0) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlIngressPortPGSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            PG_SHARED_LIMITf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry)); 
    } 
    
    return BCM_E_NONE;
}


STATIC int
_bcm_td2_cosq_alpha_get(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    bcm_port_t local_port;
    int startq;
    int midx;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int alpha;
    int dynamic_thresh_mode;
    soc_reg_t reg = INVALIDr;
    int port_pg;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };      

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm, 
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);  
    
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                           MMU_THDM_DB_QUEUE_CONFIG_1m);

        startq -= 1480; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHAf);        
           
    } else {
        if (cosq > 15 || cosq < 0) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlIngressPortPGSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, PG_SHARED_LIMITf);  
        
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_alpha_soc_to_bcm(unit,
            (soc_td2_drop_limit_alpha_value_t)alpha, arg));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_dynamic_thresh_enable_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq;
    int from_cos, to_cos, ci;      
    int midx; 
    uint32 rval;     
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_reg_t reg = INVALIDr;
    int port_pg;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };    

    
    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if ((cosq < 0) || (cosq >= _TD2_NUM_INTERNAL_PRI)) {
            return BCM_E_PARAM;
        }

        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            PG_SHARED_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                                PG_SHARED_LIMITf, bcmCosqControlDropLimitAlpha_1_128);
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));           
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, local_port, ci,
                          _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                          NULL, &startq, NULL));
                
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm, 
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                                Q_SHARED_ALPHA_CELLf, bcmCosqControlDropLimitAlpha_1_128);
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));         
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));
              
            }
        }
        
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        mem2 = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m, 
                                           MMU_THDM_MCQE_QUEUE_CONFIG_1m);

        startq -= 1480; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                                Q_SHARED_ALPHAf, bcmCosqControlDropLimitAlpha_1_128);
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));    

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem2, entry2,
                                Q_SHARED_ALPHAf, bcmCosqControlDropLimitAlpha_1_128);
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));         
       
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq;
    int from_cos, to_cos, ci;   
    int midx;  
    uint32 rval;      
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    int port_pg;
    static const soc_reg_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };    

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if ((cosq < 0) || (cosq >= _TD2_NUM_INTERNAL_PRI)) {
            return BCM_E_PARAM;
        }

        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        mem = SOC_TD2_PMEM(unit, local_port, THDI_PORT_PG_CONFIG_Xm,
                                             THDI_PORT_PG_CONFIG_Ym);
        /* get index for Port-PG */
        midx = SOC_TD2_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, PG_SHARED_DYNAMICf);  
        
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, local_port, ci,
                          _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                          NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm, 
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);        
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));
            }
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                           MMU_THDM_DB_QUEUE_CONFIG_1m);

        startq -= 1480; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMICf);         
    } else {
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 bcm_cosq_control_t type,
                                                 int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int enable;
    soc_field_t field_name = INVALIDf;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if ((type == bcmCosqControlEgressUCQueueLimitEnable) ||
        (type == bcmCosqControlEgressUCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                        MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        if (type == bcmCosqControlEgressUCQueueLimitEnable) {
            field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
            field_name = Q_COLOR_ENABLE_CELLf;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, field_name);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, field_name, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    }else if ((type == bcmCosqControlEgressMCQueueLimitEnable) ||
        (type == bcmCosqControlEgressMCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                           MMU_THDM_MCQE_QUEUE_CONFIG_1m);
        if (type == bcmCosqControlEgressMCQueueLimitEnable) {
            field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
            field_name = Q_COLOR_LIMIT_ENABLEf;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, field_name);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, field_name, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, field_name);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, field_name, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_control_t type,
                                                int *arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field_name = INVALIDf;

    if ((type == bcmCosqControlEgressUCQueueLimitEnable) ||
        (type == bcmCosqControlEgressUCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                          _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                          &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                    (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                    NULL, &startq, NULL));
        }

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, 
                 MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
        if (type == bcmCosqControlEgressUCQueueLimitEnable) {
            field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
            field_name = Q_COLOR_ENABLE_CELLf;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, field_name);
    }else if ((type == bcmCosqControlEgressMCQueueLimitEnable) ||
               (type == bcmCosqControlEgressMCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                NULL, &startq, NULL));
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m, 
                        MMU_THDM_MCQE_QUEUE_CONFIG_1m);
        if (type == bcmCosqControlEgressMCQueueLimitEnable) {
           field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
           field_name = Q_COLOR_LIMIT_ENABLEf;
        }
        startq -= 1480; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, field_name);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_limit_enable_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 bcm_cosq_control_t type,
                                                 int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS], valid;
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
        field = USE_QGROUP_MINf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) {
        field = QGROUP_LIMIT_ENABLEf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable) {
        startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                           MMU_THDU_YPIPE_CONFIG_QGROUPm);
        field = Q_LIMIT_DYNAMIC_CELLf;
    } else {
        return BCM_E_PARAM;
    }

    if ((mem != INVALIDm) && (field != INVALIDf)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, field, arg);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_limit_enable_get(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_control_t type,
                                                int *arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS], valid;
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
        field = USE_QGROUP_MINf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) {
        field = QGROUP_LIMIT_ENABLEf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable) {
        startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                           MMU_THDU_YPIPE_CONFIG_QGROUPm);
        field = Q_LIMIT_DYNAMIC_CELLf;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    *arg = soc_mem_field32_get(unit, mem, entry, field);

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_limit_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int arg)
{
    bcm_port_t local_port;
    int startq, granularity = 1;
    int cur_val, rv, post_update;
    int delta = 0; /* In Cells */
    uint32 entry[SOC_MAX_MEM_WORDS], valid;
    uint32 shared_size, spid, max_val, rval = 0;
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg /= _BCM_TD2_BYTES_PER_CELL;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    spid = soc_mem_field32_get(unit, mem, entry, Q_SPIDf);
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                       MMU_THDU_YPIPE_CONFIG_QGROUPm);
    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
        fld_limit = Q_MIN_LIMIT_CELLf;
        granularity = 1;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        fld_limit = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
    } else {
        return BCM_E_PARAM;
    }

    /* Recalculate Shared Values if Min Changed */
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, spid, &rval));
    shared_size = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                    rval, SHARED_LIMITf);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);

    if ((arg / granularity) > cur_val) {
        /* New Min Val > Cur Min Val
         * So reduce the Shared values first and then
         * Increase the Min Value for given resource
         */
        post_update = 0;
    } else if ((arg / granularity) < cur_val) {
        /* New Min Val < Cur Min Val
         * So reduce the Min values first and then
         * Increase Shared values everywhere
         */
        post_update = 1;
    } else {
        return BCM_E_NONE;
    }

    if (!post_update &&
        (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes)) {
        delta = ((arg / granularity) - cur_val) * granularity;
        if (delta > shared_size) {
            return BCM_E_RESOURCE;
        }
        rv = soc_td2_mmu_config_res_limits_update(unit, spid,
                                                  shared_size - delta, 1);
        if (rv < 0) {
            return rv;
        }
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        /* Get the latest entry data and update. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    if (post_update &&
        (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes)) {
        delta = (cur_val - (arg / granularity)) * granularity;
        rv = soc_td2_mmu_config_res_limits_update(unit, spid,
                                                  shared_size + delta, 0);
        if (rv < 0) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_limit_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type,
                               int *arg)
{
    bcm_port_t local_port;
    int startq, granularity = 1;
    uint32 entry[SOC_MAX_MEM_WORDS], valid;
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                       MMU_THDU_YPIPE_CONFIG_QGROUPm);
    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
        fld_limit = Q_MIN_LIMIT_CELLf;
        granularity = 1;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        fld_limit = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _BCM_TD2_BYTES_PER_CELL;

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_alpha_set(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS], valid, alpha;
    uint32 dyn_mode;
    soc_mem_t mem = INVALIDm;
    soc_td2_drop_limit_alpha_value_t aget;

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_alpha_bcm_to_soc(unit, arg, &aget));
    alpha = (uint32)aget;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);
    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                       MMU_THDU_YPIPE_CONFIG_QGROUPm);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    dyn_mode = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);
    if (!dyn_mode){
        return BCM_E_CONFIG;
    }
    soc_mem_field32_set(unit, mem, entry, Q_SHARED_ALPHA_CELLf, alpha);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_cosq_qgroup_alpha_get(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS], valid, alpha;
    uint32 dyn_mode;
    soc_mem_t mem = INVALIDm;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm,
                       MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);
    mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QGROUPm,
                       MMU_THDU_YPIPE_CONFIG_QGROUPm);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    dyn_mode = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);
    if (!dyn_mode){
        return BCM_E_CONFIG;
    }
    alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_alpha_soc_to_bcm(unit,
            (soc_td2_drop_limit_alpha_value_t)alpha, arg));

    return BCM_E_NONE;
}

int
bcm_td2_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport, int index,
                                      bcm_cosq_priority_mapping_t type,
                                      int pri_max, int *pri_array,
                                      int *pri_count)
{
    bcm_port_t port;
    int i, pg, sp, count = 0;

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    switch (type) {
        case bcmCosqPriorityGroup:
            if ((index < TD2_PRIOROTY_GROUP_ID_MIN) ||
                (index > TD2_PRIOROTY_GROUP_ID_MAX)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < _TD2_NUM_INTERNAL_PRI; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_port_priority_group_mapping_get(unit, gport,
                                                        i, &pg));
                if (pg == index) {
                    if (pri_max != 0 && pri_array != NULL) {
                        if (count < pri_max) {
                            pri_array[count] = i;
                        }
                    }
                    count++;
                }
            }
            break;
        case bcmCosqIngressPool:
            if ((index < 0) || (index >= _TD2_NUM_SERVICE_POOL)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < _TD2_NUM_INTERNAL_PRI; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_td2_port_priority_group_mapping_get(unit, gport,
                                                        i, &pg));
                BCM_IF_ERROR_RETURN
                    (_bcm_td2_cosq_ing_pool_get(unit, gport, pg,
                                               bcmCosqControlIngressPool,
                                               &sp));
                if (sp == index) {
                    if (pri_max != 0 && pri_array != NULL) {
                        if (count < pri_max) {
                            pri_array[count] = i;
                        }
                    }
                    count++;
                }
            }
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    if ((count < pri_max) && (pri_array != NULL)) {
        for (i = count; i < pri_max; i++) {
            pri_array[i] = -1;
        }
        *pri_count = count;
    } else if (pri_max == 0) {
        *pri_count = count;
    } else {
        *pri_count = pri_max;
    }
    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_queue_color_limit_set
 * Purpose: Set Red and Yellow color limit per queue
 *          IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_td2_cosq_egr_queue_color_limit_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity, max_val;

    if (arg < 0) {
        return BCM_E_PARAM;
    }
    granularity = 8;

    if ((type == bcmCosqControlEgressUCQueueRedLimit) ||
        (type == bcmCosqControlEgressUCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_RED_CELLf;
        } else {
            fld_limit = LIMIT_YELLOW_CELLf;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _TD2_MMU_BYTES_PER_CELL - 1) / _TD2_MMU_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg/granularity) > max_val) {
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
            }
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if ((type == bcmCosqControlEgressMCQueueRedLimit) ||
               (type == bcmCosqControlEgressMCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */
        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = RED_SHARED_LIMITf;
        } else {
            fld_limit = YELLOW_SHARED_LIMITf;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                           MMU_THDM_DB_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _TD2_MMU_BYTES_PER_CELL - 1) / _TD2_MMU_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg/granularity) > max_val) {
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
            }
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_queue_color_limit_get
 * Purpose: Get Red and Yellow color limit per queue
 * Return value: IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_td2_cosq_egr_queue_color_limit_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int* arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    granularity = 8;

    if ((type == bcmCosqControlEgressUCQueueRedLimit) ||
        (type == bcmCosqControlEgressUCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_RED_CELLf;
        } else {
            fld_limit = LIMIT_YELLOW_CELLf;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _TD2_MMU_BYTES_PER_CELL;
        }
    } else if ((type == bcmCosqControlEgressMCQueueRedLimit) ||
               (type == bcmCosqControlEgressMCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */
        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = RED_SHARED_LIMITf;
        } else {
            fld_limit = YELLOW_SHARED_LIMITf;
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                          MMU_THDM_DB_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _TD2_MMU_BYTES_PER_CELL;
        }

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_mc_queue_color_limit_set
 * Purpose: Set Red and Yellow color limit per queue for multicast
 *          IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_td2_cosq_egr_mc_queue_color_limit_set(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_control_t type,
                                           int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity, max_val;

    if (arg < 0) {
        return BCM_E_PARAM;
    }
    granularity = 8;

    if ((type == bcmCosqControlEgressMCQueueDataBufferRedLimit) ||
        (type == bcmCosqControlEgressMCQueueEntryRedLimit) ||
        (type == bcmCosqControlEgressMCQueueDataBufferYellowLimit) ||
        (type == bcmCosqControlEgressMCQueueEntryYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, gport, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, &local_port, &startq,
                NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */

        switch (type) {
        case bcmCosqControlEgressMCQueueDataBufferRedLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            fld_limit = RED_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueEntryRedLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            fld_limit = RED_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            fld_limit = YELLOW_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueEntryYellowLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            fld_limit = YELLOW_SHARED_LIMITf;
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry,
                                         Q_COLOR_LIMIT_DYNAMICf);
        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            if ((type == bcmCosqControlEgressMCQueueDataBufferRedLimit) ||
                (type == bcmCosqControlEgressMCQueueDataBufferYellowLimit)) {
                arg = (arg + _TD2_MMU_BYTES_PER_CELL - 1) /
                      _TD2_MMU_BYTES_PER_CELL;
            }

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg/granularity) > max_val) {
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(
                    unit, mem, entry, fld_limit, (arg/granularity));
            }
        }
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_mc_queue_color_limit_get
 * Purpose: Get Red and Yellow color limit per queue for multicast
 * Return value: IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_td2_cosq_egr_mc_queue_color_limit_get(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_cosq_control_t type,
                                           int* arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    granularity = 8;

    if ((type == bcmCosqControlEgressMCQueueDataBufferRedLimit) ||
        (type == bcmCosqControlEgressMCQueueEntryRedLimit) ||
        (type == bcmCosqControlEgressMCQueueDataBufferYellowLimit) ||
        (type == bcmCosqControlEgressMCQueueEntryYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, gport, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, &local_port, &startq,
                NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE, NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */
        switch (type) {
        case bcmCosqControlEgressMCQueueDataBufferRedLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            fld_limit = RED_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueEntryRedLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            fld_limit = RED_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                               MMU_THDM_DB_QUEUE_CONFIG_1m);
            fld_limit = YELLOW_SHARED_LIMITf;
            break;
        case bcmCosqControlEgressMCQueueEntryYellowLimit:
            mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                               MMU_THDM_MCQE_QUEUE_CONFIG_1m);
            fld_limit = YELLOW_SHARED_LIMITf;
            break;
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(
                        unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity;

            if ((type == bcmCosqControlEgressMCQueueDataBufferRedLimit) ||
                (type == bcmCosqControlEgressMCQueueDataBufferYellowLimit)) {
                *arg = (*arg) * _TD2_MMU_BYTES_PER_CELL;
            }
        }

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_queue_color_limit_mode_set
 * Purpose: Set color limit mode per queue to static (0) or
 *          dynamic (1)
*/

STATIC int
_bcm_td2_cosq_egr_queue_color_limit_mode_set(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, LIMIT_RED_CELLf, 0);
            soc_mem_field32_set(unit, mem, entry, LIMIT_YELLOW_CELLf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                          MMU_THDM_DB_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, RED_SHARED_LIMITf, 0);
            soc_mem_field32_set(unit, mem, entry, YELLOW_SHARED_LIMITf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }

        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_MCQE_QUEUE_CONFIG_0m,
                           MMU_THDM_MCQE_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, RED_SHARED_LIMITf, 0);
            soc_mem_field32_set(unit, mem, entry, YELLOW_SHARED_LIMITf, 0);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_td2_cosq_egr_queue_color_limit_mode_get
 * Purpose: Get color limit mode per queue.
 * Returns: 0 (static) or 1 (dynamic)
*/
STATIC int
_bcm_td2_cosq_egr_queue_color_limit_mode_get(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int* arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlEgressUCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDU_XPIPE_CONFIG_QUEUEm,
                           MMU_THDU_YPIPE_CONFIG_QUEUEm);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);
    } else if (type == bcmCosqControlEgressMCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= 1480; /* MC queues comes after UC queues */
        mem = SOC_TD2_PMEM(unit, local_port, MMU_THDM_DB_QUEUE_CONFIG_0m,
                          MMU_THDM_DB_QUEUE_CONFIG_1m);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    switch (type) {
    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
    case bcmCosqControlEgressPoolSharedLimitBytes: 
    case bcmCosqControlEgressPoolResumeLimitBytes:
    case bcmCosqControlEgressPoolYellowSharedLimitBytes: 
    case bcmCosqControlEgressPoolYellowResumeLimitBytes:
    case bcmCosqControlEgressPoolRedSharedLimitBytes:
    case bcmCosqControlEgressPoolRedResumeLimitBytes: 
        return _bcm_td2_cosq_egr_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEfPropagation:
        return _bcm_td2_cosq_ef_op(unit, gport, cosq, &arg, _BCM_COSQ_OP_SET);
    case bcmCosqControlCongestionProxy:
        return _bcm_td2_cosq_qcn_proxy_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_td2_cosq_egr_queue_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPoolMinLimitBytes:   
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
    case bcmCosqControlIngressPortPGResetOffsetBytes:
        return _bcm_td2_cosq_ing_res_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPoolLimitBytes: 
        return _bcm_td2_cosq_ing_res_limit_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            return _bcm_td2_cosq_ing_res_enable_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPool:
        return _bcm_td2_cosq_ing_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_td2_cosq_alpha_set(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_td2_cosq_dynamic_thresh_enable_set(unit, gport, cosq, 
                                                       type, arg);
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
        return _bcm_td2_cosq_egr_port_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
        return _bcm_td2_cosq_egr_queue_limit_enable_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
        return _bcm_td2_cosq_qgroup_limit_enable_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
        return _bcm_td2_cosq_qgroup_limit_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
        return _bcm_td2_cosq_qgroup_alpha_set(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t)arg);
    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
      return _bcm_td2_cosq_egr_queue_color_limit_mode_set(unit, gport, cosq,
                                                     type, arg);
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
      return _bcm_td2_cosq_egr_queue_color_limit_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes :
    case bcmCosqControlEgressMCQueueSharedLimitEntries:
        return _bcm_td2_cosq_egr_mc_queue_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressMCQueueDataBufferRedLimit:
    case bcmCosqControlEgressMCQueueEntryRedLimit:
    case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
    case bcmCosqControlEgressMCQueueEntryYellowLimit:
        return _bcm_td2_cosq_egr_mc_queue_color_limit_set(unit, gport, cosq,
                                                          type, arg);
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_td2_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    switch (type) {
    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
    case bcmCosqControlEgressPoolSharedLimitBytes: 
    case bcmCosqControlEgressPoolResumeLimitBytes:
    case bcmCosqControlEgressPoolYellowSharedLimitBytes: 
    case bcmCosqControlEgressPoolYellowResumeLimitBytes:
    case bcmCosqControlEgressPoolRedSharedLimitBytes:
    case bcmCosqControlEgressPoolRedResumeLimitBytes:         
        return _bcm_td2_cosq_egr_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEfPropagation:
        return _bcm_td2_cosq_ef_op(unit, gport, cosq, arg, _BCM_COSQ_OP_GET);
    case bcmCosqControlCongestionProxy:
        return _bcm_td2_cosq_qcn_proxy_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_td2_cosq_egr_queue_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPoolMinLimitBytes:
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:        
    case bcmCosqControlIngressPortPGResetOffsetBytes:
        return _bcm_td2_cosq_ing_res_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPoolLimitBytes: 
        return _bcm_td2_cosq_ing_res_limit_get(unit, gport, cosq, type, arg);        
    case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            return _bcm_td2_cosq_ing_res_enable_get(unit, gport, cosq, type, arg);
    case bcmCosqControlSPPortLimitState:
    case bcmCosqControlPGPortLimitState:
    case bcmCosqControlPGPortXoffState:
    case bcmCosqControlPoolRedDropState:
    case bcmCosqControlPoolYellowDropState:
    case bcmCosqControlPoolGreenDropState:
        return _bcm_td2_cosq_state_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPool:
        return _bcm_td2_cosq_ing_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_td2_cosq_alpha_get(unit, gport, cosq,  
                            (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_td2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                                       type, arg);
    case bcmCosqControlPortQueueUcast:
    case bcmCosqControlPortQueueMcast:
        return _bcm_td2_cosq_port_qnum_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
        return _bcm_td2_cosq_egr_port_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
        return _bcm_td2_cosq_egr_queue_limit_enable_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
        return _bcm_td2_cosq_qgroup_limit_enable_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
        return _bcm_td2_cosq_qgroup_limit_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
        return _bcm_td2_cosq_qgroup_alpha_get(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
      return _bcm_td2_cosq_egr_queue_color_limit_mode_get(unit, gport, cosq,
                                                     type, arg);
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
      return _bcm_td2_cosq_egr_queue_color_limit_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressMCQueueDataBufferSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitEntries:
        return _bcm_td2_cosq_egr_mc_queue_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressMCQueueDataBufferRedLimit:
    case bcmCosqControlEgressMCQueueEntryRedLimit:
    case bcmCosqControlEgressMCQueueDataBufferYellowLimit:
    case bcmCosqControlEgressMCQueueEntryYellowLimit:
        return _bcm_td2_cosq_egr_mc_queue_color_limit_get(unit, gport, cosq,
                                                          type, arg);

    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_td2_cosq_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    static const soc_field_t pool_port_color_enable_fields[] = {
        PORTSP_COLOR_LIMIT_ENABLE_0f, PORTSP_COLOR_LIMIT_ENABLE_1f, 
        PORTSP_COLOR_LIMIT_ENABLE_2f, PORTSP_COLOR_LIMIT_ENABLE_3f
    };
    static const soc_field_t pool_color_enable_fields[] = {
        POOL_COLOR_LIMIT_ENABLE_0f, POOL_COLOR_LIMIT_ENABLE_1f, 
        POOL_COLOR_LIMIT_ENABLE_2f, POOL_COLOR_LIMIT_ENABLE_3f
    };

    if (id < 0 || (id > _TD2_NUM_SERVICE_POOL - 1)) {
        return BCM_E_PARAM;
    } 

    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    switch (cosq_service_pool.type) {
    case bcmCosqServicePoolPortColorAware:
        fld_enable = pool_port_color_enable_fields[id];
        break;
    case bcmCosqServicePoolColorAware:
        fld_enable = pool_color_enable_fields[id];
        break;
    default:
        return BCM_E_PARAM;
    }

    soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                      fld_enable, cosq_service_pool.enabled ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));
    return BCM_E_NONE;
}


int
bcm_td2_cosq_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    static const soc_field_t pool_port_color_enable_fields[] = {
        PORTSP_COLOR_LIMIT_ENABLE_0f, PORTSP_COLOR_LIMIT_ENABLE_1f, 
        PORTSP_COLOR_LIMIT_ENABLE_2f, PORTSP_COLOR_LIMIT_ENABLE_3f
    };
    static const soc_field_t pool_color_enable_fields[] = {
        POOL_COLOR_LIMIT_ENABLE_0f, POOL_COLOR_LIMIT_ENABLE_1f, 
        POOL_COLOR_LIMIT_ENABLE_2f, POOL_COLOR_LIMIT_ENABLE_3f
    };

    if (cosq_service_pool == NULL) {
        return BCM_E_PARAM;
    }

    if (id < 0 || (id > _TD2_NUM_SERVICE_POOL - 1)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    switch (cosq_service_pool->type) {
    case bcmCosqServicePoolPortColorAware:
        fld_enable = pool_port_color_enable_fields[id];
        break;
    case bcmCosqServicePoolColorAware:
        fld_enable = pool_color_enable_fields[id];
        break;
    default:
        return BCM_E_PARAM;
    }

    cosq_service_pool->enabled = soc_reg_field_get(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, rval,
                                                   fld_enable);
    return BCM_E_NONE;
}



int
_bcm_td2_cosq_gport_detach(int unit, bcm_port_t port)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_bandwidth_set
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
bcm_td2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int i, start_cos, end_cos, local_port, ets_mode;
    _bcm_td2_cosq_node_t *node, *child_node;
    uint32 burst_min, burst_max;
    int mode = 0, weight = 0, weight_sum = 0;

    if (cosq <= -1) {
        if (BCM_GPORT_IS_SET(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                        NULL, NULL, &node));
            start_cos = 0;
            end_cos = node->numq - 1;
        } else {
            start_cos = 0;
            end_cos = _TD2_NUM_COS(unit) - 1;
        }
    } else {
        start_cos = end_cos = cosq;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
    ets_mode = _bcm_td2_cosq_port_has_ets(unit, local_port);
    for (i = start_cos; i <= end_cos; i++) {
        /* Workaround for bandwidth distriution inaccuracy when max bandwidth
         * shaping is used in conjunction with round robin scheduling.
         * Burst Size=SUM((RR_weights[except for Queue_i]+1))*MTU*ShapedRate/AvailableExcessBWRate
         */
        if (BCM_GPORT_IS_SET(gport) && ets_mode && SOC_IS_TRIDENT2(unit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, gport, 0, NULL,
                                        NULL, NULL, &node));
            /* SUM((RR_weights[except for Queue_i]+1)) */
            for (child_node = node->child;
                 child_node != NULL;
                 child_node = child_node->sibling) {
                if (child_node->attached_to_input != i) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2_cosq_gport_sched_get(
                            unit, gport, child_node->attached_to_input, &mode, &weight));
                    if (mode != SOC_TD2_SCHED_MODE_STRICT) {
                        weight_sum += weight + 1;
                    }
                }
            }
        }

        burst_min = (kbits_sec_min > 0) ?
              _bcm_td_default_burst_size_by_weight(
                      unit, local_port, kbits_sec_min, weight_sum) : 0;

        burst_max = (kbits_sec_max > 0) ?
              _bcm_td_default_burst_size_by_weight(
                      unit, local_port, kbits_sec_max, weight_sum) : 0;


        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                         kbits_sec_max, burst_min, burst_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_bandwidth_get
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
bcm_td2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 kbits_sec_burst;

    if (cosq == -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, gport, cosq,
                                 kbits_sec_min, kbits_sec_max,
                                 &kbits_sec_burst, &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_bandwidth_burst_set
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
bcm_td2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int i, start_cos, end_cos;
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst, flags;
    _bcm_td2_cosq_node_t *node;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    if (cosq == -1) {
        if (BCM_GPORT_IS_SET(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, 
                                        NULL, NULL, &node));
            start_cos = 0;
            end_cos = node->numq - 1;
        } else {
            start_cos = 0;
            end_cos = _TD2_NUM_COS(unit) - 1;
        }
    } else {
        start_cos = end_cos = cosq;
    }

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_bucket_get(unit, gport, i, &kbits_sec_min,
                                     &kbits_sec_max, &kbits_sec_burst,
                                     &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                     kbits_sec_max, kbits_burst_min, 
                                     kbits_burst_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_bandwidth_burst_get
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
bcm_td2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max, flags;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 &kbits_sec_min, &kbits_sec_max, kbits_burst_min,
                                 kbits_burst_max, &flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_sched_set
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
bcm_td2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    int rv, numq, i, count;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    count = 0;
    for (i = 0; i < numq; i++) {
        rv = _bcm_td2_cosq_sched_set(unit, gport, cosq + i, mode, weight);
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
 *     bcm_td2_cosq_gport_sched_get
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
bcm_td2_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int *mode, int *weight)
{
    int rv, numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_index_resolve(unit, gport, 0,
                                        _BCM_TD2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        rv = _bcm_td2_cosq_sched_get(unit, gport, cosq + i, mode, weight);
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
 *     bcm_td2_cosq_gport_discard_set
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
bcm_td2_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh, flags = 0;
    int cell_size, cell_field_max;
    bcm_port_t local_port = -1;

    /* refresh_time  from 1 to 7 be set the same as default 8,so the reasonable 
           range of refresh_time  is from 1 to 512 */
    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100 ||
        discard->refresh_time <= 0 || discard->refresh_time > 512 ||
        (discard->min_thresh > discard->max_thresh) ||
        (discard->min_thresh < 0)) {
        return BCM_E_PARAM;
    }

    cell_size = _BCM_TD2_BYTES_PER_CELL;
    cell_field_max = TD2_CELL_FIELD_MAX;

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
    } else {
        /* Packet mode not supported */
        return BCM_E_PARAM;
    }

    if (gport != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
    }

        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_wred_set(unit, gport, cosq, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE, flags,discard->refresh_time));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_gport_discard_get
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
bcm_td2_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;
    bcm_port_t local_port = -1;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (discard->flags & BCM_COSQ_DISCARD_PACKETS) {
        /* Packet mode not supported */
        return BCM_E_PARAM;
    }

    if (gport != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_td2_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain, 
                               0,&discard->refresh_time));

    /* Convert number of cells to number of bytes */
    discard->min_thresh = min_thresh * _BCM_TD2_BYTES_PER_CELL;
    discard->max_thresh = max_thresh * _BCM_TD2_BYTES_PER_CELL;

    return BCM_E_NONE;
}

int
_bcm_td2_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
                               int *class)
{
    switch (sctype) {
    case bcmSwitchPFCClass7Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass0Queue:
        *type = _BCM_TD2_COSQ_TYPE_UCAST;
        break;
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass0McastQueue:
        *type = _BCM_TD2_COSQ_TYPE_MCAST;
        break;
    case bcmSwitchPFCClass7DestmodQueue:
    case bcmSwitchPFCClass6DestmodQueue:
    case bcmSwitchPFCClass5DestmodQueue:
    case bcmSwitchPFCClass4DestmodQueue:
    case bcmSwitchPFCClass3DestmodQueue:
    case bcmSwitchPFCClass2DestmodQueue:
    case bcmSwitchPFCClass1DestmodQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *type = _BCM_TD2_COSQ_TYPE_EXT_UCAST;
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (sctype) {
    case bcmSwitchPFCClass7Queue:
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass7DestmodQueue:
        *class = 7;
        break;
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass6DestmodQueue:
        *class = 6;
        break;
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass5DestmodQueue:
        *class = 5;
        break;
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass4DestmodQueue:
        *class = 4;
        break;
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass3DestmodQueue:
        *class = 3;
        break;
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass2DestmodQueue:
        *class = 2;
        break;
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass1DestmodQueue:
        *class = 1;
        break;
    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass0McastQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *class = 0;
        break;
    /*
     * COVERITY
     *
     * This default is unreachable but needed for some compiler. 
     */ 
    /* coverity[dead_error_begin] */
    default:
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_pfc_map_index_resolve
 * Purpose:
 *     Resolve table index and field of MMU_INTFI_X/YPIPE_FC_MAP_TBL0/1/2.
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) port number
 *     hw_index - (IN) Gport node hw_index
 *     level    - (IN) Sched level
 *     index    - (OUT) Resolved table index
 *     field    - (OUT) Resolved index field
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_pfc_map_index_resolve(int unit, int port,
                               uint32 hw_index, int level,
                               int *index, int *field)
{
    int map_entry_index, eindex;
    int port_num = 0;
    int map_entry_index_num = 0;
    int mcq_offset;
    uint16 dev_id;
    uint8 rev_id;

    if ((index == NULL) || (field == NULL)) {
        return BCM_E_PARAM;
    }
    if ((level != SOC_TD2_NODE_LVL_L0) &&
        (level != SOC_TD2_NODE_LVL_L1) &&
        (level != SOC_TD2_NODE_LVL_L2)) {
        return BCM_E_PARAM;
    }

   /* FC_MAP_TBL[Level] has 4 Index[0-3], whose values become Index to PFC_ST_TBL.
    * Each Index of PFC_ST_TBL can point to congestion state of 4 Qs. 
    * Hence each Entry of FC_MAP_TBL can refer to 16 Qs.
    *
    * NOTE: Index of FC_MAP_TBL CANNOT be used to point to Qs/Nodes belonging
    *       to Different Ports.
    */
    if (level == SOC_TD2_NODE_LVL_L0) {
        if (IS_TD2_HSP_PORT(unit, port)) {
            if ((hw_index % _BCM_TD2_HSP_PORT_MAX_L0) > 3) {
                return BCM_E_UNAVAIL;
            }
            port_num = hw_index / _BCM_TD2_HSP_PORT_MAX_L0;

            map_entry_index_num = port_num;
            /* each entry contains 4 indices */
            map_entry_index = map_entry_index_num / 4;
            eindex = map_entry_index_num & 0x3;
        } else {
            map_entry_index = hw_index/16;
            eindex = (hw_index % 16) / 4;
        }
    } else if (level == SOC_TD2_NODE_LVL_L1) {
        if (IS_TD2_HSP_PORT(unit, port)) {
            if ((hw_index % _BCM_TD2_HSP_PORT_MAX_L1) > 7) {
                return BCM_E_UNAVAIL;
            }
            port_num = hw_index / _BCM_TD2_HSP_PORT_MAX_L1;
            soc_cm_get_id_otp(unit, &dev_id, &rev_id);
            if ((dev_id == BCM56850_DEVICE_ID) &&
                (rev_id <= BCM56850_A1_REV_ID)) {
            /* A0 & A1: even port takes 2 entries, odd port takes 3 entries. */
                map_entry_index_num = (port_num * 2) + port_num / 2;
                if (port_num % 2) {
                    map_entry_index_num += ((hw_index % 10) + 2) / 4;
                } else {
                    map_entry_index_num += ((hw_index % 10) >= 4)? 1 : 0;
                }
            } else {
                map_entry_index_num = port_num * 2 +
                                      (((hw_index % 10) >= 4)? 1 : 0);
            }
            map_entry_index = map_entry_index_num / 4;
            eindex = map_entry_index_num & 0x3;
        } else {
            map_entry_index = hw_index/16;
            eindex = (hw_index % 16) / 4;
        }
    } else {
        if (hw_index < 1480) {
        /* UC queue */
            if (IS_TD2_HSP_PORT(unit, port)) {
                if ((hw_index % 10) > 7) {
                    return BCM_E_UNAVAIL;
                }
                port_num = hw_index / 10;
                map_entry_index_num = port_num * 2 +
                                      (((hw_index % 10) >= 4)? 1 : 0);
                map_entry_index = map_entry_index_num / 4;
                eindex = map_entry_index_num & 0x3;
            } else {
                map_entry_index = hw_index/16;
                eindex = (hw_index % 16) / 4;
            }
        } else {
        /* MC queue - Index Calculation
         *   PFC can work only on 8 out of 10 MC queues of each port.
         *   So in FC_MAP_TBL2, each Port will use only 2 Index(4 Qs each) for
         *   MC Qs.
         *   MC queue base starts at 1480, 1480/16 = 92.5 
         *   Hence MC Qs starts at = Map_Tbl2_Idx[92] and Index2.
         */
            mcq_offset = hw_index % 1480;
            if ((mcq_offset % 10) > 7) {
                /* FC support only for Q0-7 */
                return BCM_E_INTERNAL;
            }
            map_entry_index = hw_index - (mcq_offset * 2 / 10);
            map_entry_index /= 16;
            eindex = (mcq_offset % 10)/4;
            eindex += ((mcq_offset /10) % 2) ? 0 : 2;
        }
    }

    *index = map_entry_index;
    *field = eindex;
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_map_fc_status_to_node(int unit, int port, _bcm_td2_fc_type_t fct,
                    int spad_offset, int cosq, uint32 hw_index, int level)
{
    int map_entry_index, eindex, pipe, pfc;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f, INDEX2f, INDEX3f
    };
    static const soc_mem_t memx[] = {
        INVALIDm,
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };
    static const soc_mem_t memy[] = {
        INVALIDm,
        MMU_INTFI_YPIPE_FC_MAP_TBL0m,
        MMU_INTFI_YPIPE_FC_MAP_TBL1m,
        MMU_INTFI_YPIPE_FC_MAP_TBL2m
    };
    soc_mem_t mem;
    int port_num = 0;
    uint16 dev_id;
    uint8 rev_id;

    pfc = (fct == _BCM_TD2_COSQ_FC_PFC) ? 1 : 0;
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    mem = (pipe == _TD2_XPIPE) ? memx[level] : memy[level];
    if (mem ==  INVALIDm) {
        return BCM_E_PARAM;
    }

    if (IS_TD2_HSP_PORT(unit, port)) {
        port_num = hw_index / 10;
        soc_cm_get_id_otp(unit, &dev_id, &rev_id);
        if ((dev_id == BCM56850_DEVICE_ID) &&
            (rev_id <= BCM56850_A1_REV_ID) &&
            ((level == SOC_TD2_NODE_LVL_L1) ||
             (level == SOC_TD2_NODE_LVL_L2))) {
            if (port_num % 2) {
                cosq += 2;
            }
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_pfc_map_index_resolve(unit, port,
                            hw_index, level, &map_entry_index, &eindex));
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, map_entry_index, &map_entry));
        /* coverity[overrun-local : FALSE] */
        soc_mem_field32_set(unit, mem, &map_entry, 
                            indexf[eindex], spad_offset + cosq/4);
        /* coverity[overrun-local : FALSE] */
        soc_mem_field32_set(unit, mem, &map_entry, self[eindex], !!pfc);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                            MEM_BLOCK_ALL, map_entry_index, &map_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_clear_fc_status_to_node
 * Purpose:
 *     Clear MMU_INTFI_PIPE_FC_MAP_TBL setting of a port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_clear_fc_status_to_node(int unit, int port)
{
    uint32 map_entry[SOC_MAX_MEM_WORDS], i_value, s_value;
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f, INDEX2f, INDEX3f
    };
    static const soc_mem_t memx[] = {
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };
    static const soc_mem_t memy[] = {
        MMU_INTFI_YPIPE_FC_MAP_TBL0m,
        MMU_INTFI_YPIPE_FC_MAP_TBL1m,
        MMU_INTFI_YPIPE_FC_MAP_TBL2m
    };
    static const soc_mem_t *mem;
    int base, table, index, count, field;
    int mmu_port, phy_port, update;
    bcm_port_t local_port;
    soc_info_t *si;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    mem = (mmu_port < SOC_TD2_NUM_MMU_PORTS_PER_PIPE)? memx : memy;

    base = mmu_port * 4;
    for (table = 0; table < COUNTOF(memx); table++) {
        count = soc_mem_index_count(unit, mem[table]);
        for (index = 0; index < count; index++) {
            update = 0;
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem[table],
                MEM_BLOCK_ALL, index, &map_entry));
            for (field = 0; field < COUNTOF(indexf); field++) {
                s_value = soc_mem_field32_get(unit, mem[table],
                                              &map_entry, self[field]);
                i_value = soc_mem_field32_get(unit, mem[table],
                                              &map_entry, indexf[field]);
                if (s_value && (i_value >= base) && (i_value < (base + 4))) {
                    soc_mem_field32_set(unit, mem[table],
                                        &map_entry, self[field], 0);
                    soc_mem_field32_set(unit, mem[table],
                                        &map_entry, indexf[field], 0);
                    update = 1;
                }
            }
            if (update) {
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem[table],
                    MEM_BLOCK_ALL, index, &map_entry));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_fc_status_map_gport
 * Purpose:
 *     Set mapping in FC_MAP_TBL for gport, and update cos_bmp with gport cosq.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 *     gport      - (IN) gport to be mapped
 *     fct        - (IN) FC type 
 *     cos_map    - (OUT) hw_cosq of gport
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     This function assumes gport has been validated by
 *     "_bcm_td2_fc_map_gport_check".
 */
STATIC int
_bcm_td2_fc_status_map_gport(int unit, int port, 
                bcm_gport_t gport, _bcm_td2_fc_type_t fct, uint32 *cos_bmp)
{
    int hw_index, hw_index1;
    int hw_cosq, hw_cosq1;
    int id, lvl;
    int pipe, mmu_port, phy_port;
    bcm_port_t local_port, resolved_port;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_cosq_port_info_t *port_info;
    soc_info_t *si;
    _bcm_td2_mmu_info_t *mmu_info;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    
    hw_index = hw_index1 = -1;
    hw_cosq = hw_cosq1 = -1;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, gport, 
                            0, NULL, &resolved_port, &id, &node));
        hw_index = node->hw_index;
        hw_cosq = node->hw_cosq;
        lvl = node->level;
    } else {
        if (_bcm_td2_cosq_port_has_ets(unit, local_port)) {
            hw_cosq = gport;
            node = &res->p_queue_node[port_info->uc_base + hw_cosq];
            hw_index = node->hw_index;
            
            node = &res->p_mc_queue_node[port_info->mc_base + hw_cosq];
            hw_index1 = node->hw_index;
            hw_cosq1 = hw_cosq;
            lvl = SOC_TD2_NODE_LVL_L2;
        } else {
            lvl = SOC_TD2_NODE_LVL_L1;
            /* gport[] represnts the physical Cos value */
            hw_cosq = gport;
            BCM_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit, local_port, 
            SOC_TD2_NODE_LVL_L1, hw_cosq, &hw_index));
        }
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_map_fc_status_to_node(unit, local_port, fct,
                                       mmu_port*4, hw_cosq, hw_index, lvl));
    if (cos_bmp != NULL) {
        *cos_bmp |= 1 << hw_cosq;
    }
    
    if (hw_cosq1 >= 0) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_map_fc_status_to_node(unit, local_port, fct,
                                       mmu_port*4, hw_cosq1, hw_index1, lvl));
        if (cos_bmp != NULL) {
            *cos_bmp |= 1 << hw_cosq1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_default_llfc_profile_attach
 * Purpose:
 *     Add default entries for PRIO2COS_PROFILE register profile
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_default_llfc_profile_attach(int unit, int port)
{
    soc_reg_t   reg;
    soc_info_t *si;
    bcm_port_t local_port;
    int mmu_port, phy_port;
    uint32 profile_index;
    uint64 tmp, rval, index_map;
    uint64 rval64s[16], *prval64s[1];
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r, PORT_PFC_CFG3r
    };

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    reg = llfc_cfgr[mmu_port/32];

    /* Add default entries for PRIO2COS_PROFILE register profile */
    sal_memset(rval64s, 0, sizeof(rval64s));
    prval64s[0] = rval64s;

    BCM_IF_ERROR_RETURN
            (soc_profile_reg_add(unit, _bcm_td2_llfc_profile[unit],
                                 prval64s, 16, &profile_index));

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));
    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);

    COMPILER_64_SET(tmp, 0, 3);
    COMPILER_64_SHL(tmp, (mmu_port % 32) * 2);
    COMPILER_64_NOT(tmp);
    COMPILER_64_AND(index_map, tmp);
    COMPILER_64_SET(tmp, 0, profile_index/16);
    COMPILER_64_SHL(tmp, (mmu_port % 32) * 2);
    COMPILER_64_OR(index_map, tmp);

    soc_reg64_field_set(unit, reg, &rval, PROFILE_INDEXf,
                        index_map);

    BCM_IF_ERROR_RETURN(soc_reg64_set(unit, reg, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_llfc_profile_detach
 * Purpose:
 *     Delete old entries for PRIO2COS_PROFILE register profile
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_llfc_profile_detach(int unit, int port)
{
    soc_reg_t   reg;
    bcm_port_t local_port;
    int mmu_port, phy_port;
    uint32 old_profile_index;
    uint32 tmp32;
    uint64 rval, index_map;
    soc_info_t *si;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r, PORT_PFC_CFG3r
    };

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    reg = llfc_cfgr[mmu_port/32];
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));

    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    old_profile_index = (tmp32 & 3)*16;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_td2_llfc_profile[unit],
                                old_profile_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_port_pfc_profile_set
 * Purpose:
 *     Set PFC mapping profile for port. Updates PRIO2COS_PROFILE & PORT_PFC_CFG.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 *     op         - (IN) operation type
 *     entry_cnt  - (IN) count of bmp 
 *     pfc_class  - (IN) class list
 *     cos_bmp    - (IN) bmp list
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_port_pfc_profile_set(int unit, int port, _bcm_cosq_op_t op,
                              int entry_cnt, int *pfc_class, uint32 *cos_bmp)
{
    soc_reg_t   reg;
    bcm_port_t local_port;
    int rv, entry, cur_class;
    int mmu_port, phy_port, p_mmu_port;
    uint16 dev_id;
    uint8  rev_id;
    uint32 profile_index, old_profile_index;
    uint32 fval, tmp32, cur_bmp;
    uint64 rval64[16], tmp, *rval64s[1], rval, index_map;
    soc_info_t *si;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r, PORT_PFC_CFG3r
    };

    if (pfc_class == NULL) {
        return BCM_E_PARAM;
    }
    if (cos_bmp == NULL) {
        return BCM_E_PARAM;
    }
    if ((entry_cnt < 0 ) || (entry_cnt > _BCM_TD2_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    
    reg = llfc_cfgr[mmu_port/32];
    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));
    
    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    old_profile_index = (tmp32 & 3)*16;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_td2_llfc_profile[unit],
                             old_profile_index, 16, rval64s));

    for (entry = 0; entry < entry_cnt; entry++) {
        cur_class = pfc_class[entry];
        cur_bmp = cos_bmp[entry];
        /* A0 & A1: odd HSP port bitmap needs shifting 2 bits to left. */
        if (IS_TD2_HSP_PORT(unit, local_port)) {
            soc_cm_get_id_otp(unit, &dev_id, &rev_id);
            p_mmu_port = _BCM_TD2_ROOT_SCHED_INDEX(unit, mmu_port);
            if ((dev_id == BCM56850_DEVICE_ID) &&
                (rev_id <= BCM56850_A1_REV_ID) &&
                (p_mmu_port % 2)) {
                cur_bmp = cur_bmp << 2;
            }
        }
        if (op == _BCM_COSQ_OP_SET) {
            soc_reg64_field32_set(unit, PRIO2COS_PROFILEr, &rval64[cur_class],
                                  COS_BMPf, cur_bmp);
        } else {
            if (cur_bmp != 0) {
                fval = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr,
                                             rval64[cur_class], COS_BMPf);
                if (op == _BCM_COSQ_OP_ADD) {
                    fval |= cur_bmp;
                } else { /* _BCM_COSQ_OP_DELETE */
                    fval &= ~cur_bmp;
                }
                soc_reg64_field32_set(unit, PRIO2COS_PROFILEr,
                                      &rval64[cur_class], COS_BMPf, fval);
            }
        }
    }
    
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_td2_llfc_profile[unit],
                                old_profile_index));

    rv = soc_profile_reg_add(unit, _bcm_td2_llfc_profile[unit], rval64s,
                             16, &profile_index);
    if (SOC_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, _bcm_td2_llfc_profile[unit],
                                       old_profile_index, 16));
        return rv;
    }


    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);

    COMPILER_64_SET(tmp, 0, 3);
    COMPILER_64_SHL(tmp, (mmu_port % 32) * 2);
    COMPILER_64_NOT(tmp);
    COMPILER_64_AND(index_map, tmp);
    COMPILER_64_SET(tmp, 0, profile_index/16);
    COMPILER_64_SHL(tmp, (mmu_port % 32) * 2);
    COMPILER_64_OR(index_map, tmp);
    
    soc_reg64_field_set(unit, reg, &rval, PROFILE_INDEXf,
                        index_map);

    BCM_IF_ERROR_RETURN(soc_reg64_set(unit, reg, 0, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_fc_map_gport_check
 * Purpose:
 *     Validate if a gport can be mapped to a port's PFC/FC status.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     gport            - (IN) gport that will be mapped
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_fc_map_gport_check(int unit,
    bcm_gport_t port,
    bcm_gport_t gport)
{
    bcm_port_t local_port = -1, resolved_port;
    int hw_cosq, pipe;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_cosq_port_info_t *port_info;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_td2_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    
    hw_cosq = -1;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, gport, 
                            0, NULL, &resolved_port, NULL, &node));
        if (!((node->type == _BCM_TD2_NODE_UCAST) ||
              (node->type == _BCM_TD2_NODE_MCAST) ||
              (node->type == _BCM_TD2_NODE_SCHEDULER))) {
            return BCM_E_PARAM;
        }
        if ((node->type == _BCM_TD2_NODE_SCHEDULER) &&
            (node->node_align_value != _BCM_TD2_START_INDEX_ALIGN_PFC) &&
            (node->level != SOC_TD2_NODE_LVL_ROOT)) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                      "Gport without BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN!\n")));
            return BCM_E_PARAM;
        }
        if (resolved_port != local_port) {
            return BCM_E_PARAM;
        }
        hw_cosq = node->hw_cosq;
    } else {
        hw_cosq = gport;
        if (_bcm_td2_cosq_port_has_ets(unit, local_port)) {
            node = &res->p_queue_node[port_info->uc_base + hw_cosq];
            if ((!node->in_use) || (node->attached_to_input == -1)) {
                return BCM_E_PARAM;
            }
            node = &res->p_mc_queue_node[port_info->mc_base + hw_cosq];
            if ((!node->in_use) || (node->attached_to_input == -1)) {
                return BCM_E_PARAM;
            }
        }
    }
    if ((hw_cosq < 0) || (hw_cosq >= _TD2_NUM_INTERNAL_PRI)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2_pfc_class_mapping_check
 * Purpose:
 *     Validate param of bcm_cosq_pfc_class_mapping_t.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping array
 *     mapping_array    - (IN) mapping array list
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2_pfc_class_mapping_check(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int count, index, cur_class;
    bcm_gport_t cur_gport = 0;

    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }
    
    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_TD2_NUM_PFC_CLASS)) {
            return BCM_E_PARAM;
        }
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            BCM_IF_ERROR_RETURN(_bcm_td2_fc_map_gport_check(unit,
                                                            port, cur_gport));
        }
    }

    return BCM_E_NONE;
}

int
bcm_td2_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count)
{
    int type, class = -1, index;
    uint32 cos_bmp;
    _bcm_td2_fc_type_t fct;

    if (gport_count < 0) {
        return BCM_E_PARAM;
    }
    for (index = 0; index < gport_count; index++) {
        BCM_IF_ERROR_RETURN(_bcm_td2_fc_map_gport_check(unit,
                                                        port, gport[index]));
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_pfc_class_resolve(sctype, &type, &class));

    cos_bmp = 0;
    fct = (op == _BCM_COSQ_OP_CLEAR)? 0 : _BCM_TD2_COSQ_FC_PFC;
    for (index = 0; index < gport_count; index++) {
        BCM_IF_ERROR_RETURN(
            _bcm_td2_fc_status_map_gport(unit, port, gport[index],
                                         fct, &cos_bmp));
    }

    if (op == _BCM_COSQ_OP_CLEAR) {
        cos_bmp = (1 << _TD2_NUM_INTERNAL_PRI) - 1;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_port_pfc_profile_set(unit, port, op,
                                                      1, &class, &cos_bmp));
    
    return BCM_E_NONE;
}

int
bcm_td2_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    int type = -1, class = -1, hw_cosq, count = 0;
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1], rval, index_map;
    uint32 tmp32, bmp, bmp_len;
    _bcm_td2_mmu_info_t *mmu_info;
    int phy_port, mmu_port, p_mmu_port;
    soc_info_t *si;
    soc_reg_t   reg;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_cosq_port_info_t *port_info;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r, PORT_PFC_CFG3r
    };
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    int j, finished = 0, hw_index, pipe, eindex;
    soc_mem_t mem;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    uint16 dev_id;
    uint8 rev_id;

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if ((gport == NULL) && (gport_count > 0)) {
        return BCM_E_PARAM;
    }
    
    if (actual_gport_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, port, &local_port));

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_pfc_class_resolve(sctype, &type, &class));

    si = &SOC_INFO(unit);
    mmu_info = _bcm_td2_mmu_info[unit];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    port_info = &mmu_info->port_info[local_port];

    rval64s[0] = rval64;
    reg = llfc_cfgr[mmu_port/32];
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));

    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    profile_index = (tmp32 & 3)*16;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_td2_llfc_profile[unit],
                             profile_index, 16, rval64s));

    bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[class],
                                COS_BMPf);
    bmp_len = soc_reg_field_length(unit, PRIO2COS_PROFILEr, COS_BMPf);
    /* A0 & A1: odd HSP port bitmap has been shifted 2 bits to left. */
    if (IS_TD2_HSP_PORT(unit, port)) {
        soc_cm_get_id_otp(unit, &dev_id, &rev_id);
        p_mmu_port = _BCM_TD2_ROOT_SCHED_INDEX(unit, mmu_port);
        if ((dev_id == BCM56850_DEVICE_ID) &&
            (rev_id <= BCM56850_A1_REV_ID) &&
            (p_mmu_port % 2)) {
            bmp = bmp >> 2;
        }
    }
    for (hw_cosq = 0; hw_cosq < bmp_len; hw_cosq++) {
        if (!(bmp & (1 << hw_cosq))) {
            continue;
        }
        if (_bcm_td2_cosq_port_has_ets(unit, local_port)) {
            for (j = _BCM_TD2_NUM_PORT_SCHEDULERS; 
                 j < _BCM_TD2_NUM_TOTAL_SCHEDULERS; j++) {
                node = &mmu_info->sched_node[j];
                if ((!node->in_use) || (node->local_port != local_port) ||
                    (node->hw_cosq != hw_cosq)) {
                    continue;
                }

                if (node->level == SOC_TD2_NODE_LVL_L0) {
                    mem = (pipe == _TD2_XPIPE) ? MMU_INTFI_XPIPE_FC_MAP_TBL0m :
                                                 MMU_INTFI_YPIPE_FC_MAP_TBL0m;
                } else if (node->level == SOC_TD2_NODE_LVL_L1) {
                    mem = (pipe == _TD2_XPIPE) ? MMU_INTFI_XPIPE_FC_MAP_TBL1m :
                                                 MMU_INTFI_YPIPE_FC_MAP_TBL1m;
                } else {
                    continue;
                }
                BCM_IF_ERROR_RETURN(_bcm_td2_pfc_map_index_resolve(unit, port,
                    node->hw_index, node->level, &hw_index, &eindex));
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                    MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                        finished = 1;
                        break;
                    }
                }
            }

            for (j = port_info->uc_base; 
                 (finished == 0) && (j < port_info->uc_limit); j++) {
                node = &res->p_queue_node[j];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem = (pipe == _TD2_XPIPE) ? MMU_INTFI_XPIPE_FC_MAP_TBL2m :
                                             MMU_INTFI_YPIPE_FC_MAP_TBL2m;
                BCM_IF_ERROR_RETURN(_bcm_td2_pfc_map_index_resolve(unit, port,
                    node->hw_index, node->level, &hw_index, &eindex));
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                    MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                        finished = 1;
                        break;
                    }
                }
            }
            for (j = port_info->mc_base; 
                 (finished == 0) && (j < port_info->mc_limit); j++) {
                node = &res->p_mc_queue_node[j];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem = (pipe == _TD2_XPIPE) ? MMU_INTFI_XPIPE_FC_MAP_TBL2m :
                                             MMU_INTFI_YPIPE_FC_MAP_TBL2m;
                BCM_IF_ERROR_RETURN(_bcm_td2_pfc_map_index_resolve(unit, port,
                    node->hw_index, node->level, &hw_index, &eindex));
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                    MEM_BLOCK_ALL, hw_index, &map_entry));
                /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                        break;
                    }
                }
            }
        } else {
            if (gport_count > 0) {
                gport[count] = hw_cosq;
            }
            count++;
        }
        if ((count == gport_count) && (gport_count > 0)) {
            break;
        }
    }

    if (count == 0) {
        return BCM_E_NOT_FOUND;
    }
    *actual_gport_count = count;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_pfc_class_mapping_set
 * Purpose:
 *     Set PFC mapping for port. PFC class is mapped to cosq or scheduler node gports.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0, pfc_class[_BCM_TD2_NUM_PFC_CLASS];
    bcm_gport_t cur_gport = 0;
    uint32 cos_bmp[_BCM_TD2_NUM_PFC_CLASS];


    if ((array_count < 0) || (array_count > _BCM_TD2_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_td2_pfc_class_mapping_check(unit, port,
        array_count, mapping_array));

    (void) _bcm_td2_clear_fc_status_to_node(unit, port);
    sal_memset(cos_bmp, 0, sizeof(cos_bmp));
    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            
            BCM_IF_ERROR_RETURN(_bcm_td2_fc_status_map_gport(unit, port, 
                cur_gport, _BCM_TD2_COSQ_FC_PFC, &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_TD2_NUM_PFC_CLASS; index++) {
        pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_td2_port_pfc_profile_set(unit, port,
        _BCM_COSQ_OP_SET, _BCM_TD2_NUM_PFC_CLASS, pfc_class, cos_bmp));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td2_cosq_pfc_class_mapping_get
 * Purpose:
 *     Get PFC mapping for port.
 *     Retrieves cosq or scheduler node gports
 *     associated to PFC classes.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_max        - (IN) max retrieve count
 *     mapping_array    - (OUT) mappings
 *     array_count      - (OUT) retrieved mapping count
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2_cosq_pfc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_pfc_class_mapping_t *mapping_array,
    int *array_count)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    bcm_switch_control_t sc[] = { bcmSwitchPFCClass0Queue,
                                  bcmSwitchPFCClass1Queue,
                                  bcmSwitchPFCClass2Queue,
                                  bcmSwitchPFCClass3Queue,
                                  bcmSwitchPFCClass4Queue,
                                  bcmSwitchPFCClass5Queue,
                                  bcmSwitchPFCClass6Queue,
                                  bcmSwitchPFCClass7Queue
                                };

    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }
    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    for (cur_class = 0; cur_class < _BCM_TD2_NUM_PFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        if (array_max > 0) {
            rv = bcm_td2_cosq_port_pfc_get(unit, port, sc[cur_class],
                        mapping_array[class_count].gport_list,
                        BCM_COSQ_PFC_GPORT_COUNT, &actual_gport_count);
        } else {
            rv = bcm_td2_cosq_port_pfc_get(unit, port, sc[cur_class],
                         NULL, -1, &actual_gport_count);
        }
        if (rv == BCM_E_NONE) {
            if (array_max > 0) {
                mapping_array[class_count].class_id = cur_class;
                if (actual_gport_count < BCM_COSQ_PFC_GPORT_COUNT) {
                    mapping_array[class_count].gport_list[actual_gport_count] = 
                                                             BCM_GPORT_INVALID;
                }
            }
            class_count++;
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }

        if ((class_count == array_max) && (array_max > 0)) {
            break;
        }
    }

    *array_count = class_count;
    if (class_count == 0) {
        return BCM_E_NOT_FOUND;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_priority_group_mapping_set
 * Purpose:
 *      Assign a Priority Group for the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (IN) PG ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group)
{
    bcm_port_t port;
    uint32 value;
    int field_count;
    soc_field_t fields_0[]={PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                            PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf};
    soc_field_t fields_1[]={PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
                            PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf};

    if (!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
    if ((prio < TD2_SAFC_INPUT_PRIORITY_MIN) || 
        (prio > TD2_SAFC_INPUT_PRIORITY_MAX)) {
        return BCM_E_PARAM;
    }

    if ((priority_group < TD2_PRIOROTY_GROUP_ID_MIN) || 
        (priority_group > TD2_PRIOROTY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    field_count = COUNTOF(fields_0);
    if (prio < field_count) {
        /* input priority 0~7 */
        BCM_IF_ERROR_RETURN(READ_THDI_PORT_PRI_GRP0r(unit, port, &value));
        soc_reg_field_set(unit, THDI_PORT_PRI_GRP0r, &value, 
                          fields_0[prio], priority_group);
        BCM_IF_ERROR_RETURN(WRITE_THDI_PORT_PRI_GRP0r(unit, port, value));
    } else {
        /* input priority 8~15 */
        BCM_IF_ERROR_RETURN(READ_THDI_PORT_PRI_GRP1r(unit, port, &value));
        soc_reg_field_set(unit, THDI_PORT_PRI_GRP1r, &value, 
                          fields_1[prio - field_count], priority_group);
        BCM_IF_ERROR_RETURN(WRITE_THDI_PORT_PRI_GRP1r(unit, port, value));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_priority_group_mapping_get
 * Purpose:
 *      Get the assigned Priority Group ID of the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (OUT) PG ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group)
{
    bcm_port_t port;
    uint32 value;
    int field_count;
    soc_field_t fields_0[]={PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
                            PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf};
    soc_field_t fields_1[]={PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
                            PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf};

    if (!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    if (priority_group == NULL) {
        return BCM_E_PARAM;
    }
    /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
    if ((prio < TD2_SAFC_INPUT_PRIORITY_MIN) || 
        (prio > TD2_SAFC_INPUT_PRIORITY_MAX)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    field_count = COUNTOF(fields_0);
    if (prio < field_count) {
        /* input priority 0~7 */
        BCM_IF_ERROR_RETURN(READ_THDI_PORT_PRI_GRP0r(unit, port, &value));
        *priority_group = soc_reg_field_get(unit, THDI_PORT_PRI_GRP0r,
                                            value, fields_0[prio]);
    } else {
        /* input priority 8~15 */
        BCM_IF_ERROR_RETURN(READ_THDI_PORT_PRI_GRP1r(unit, port, &value));
        *priority_group = soc_reg_field_get(unit, THDI_PORT_PRI_GRP1r,
                                            value, 
                                            fields_1[prio - field_count]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_priority_group_config_set
 * Purpose:
 *      Set the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (IN) structure describes port PG configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_priority_group_config_set(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    bcm_port_t port;
    uint32 rval, pri_bmp;

    if (!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    if ((priority_group < TD2_PRIOROTY_GROUP_ID_MIN) || 
        (priority_group > TD2_PRIOROTY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }
    if (prigrp_config == NULL) {
        return BCM_E_PARAM;
    }
    if (prigrp_config->shared_pool_xoff_enable ||
        prigrp_config->shared_pool_discard_enable ||
        prigrp_config->priority_enable_vector_mask) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_INPUT_PORT_XON_ENABLESr(unit, port, &rval));
    pri_bmp = soc_reg_field_get(unit, THDI_INPUT_PORT_XON_ENABLESr, rval,
                                PORT_PRI_XON_ENABLEf);

    if (prigrp_config->pfc_transmit_enable) {
        pri_bmp |= (1 << priority_group);
    } else {
        pri_bmp &= ~(1 << priority_group);
    }
    /* To cover higher 8 PFC classes that have no corresponding PG. */
    if ((pri_bmp & 0xff) == 0) {
        pri_bmp = 0;
    }
    
    BCM_IF_ERROR_RETURN(_bcm_sc_tr_port_pfc_tx_set(unit, port, pri_bmp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_port_priority_group_config_get
 * Purpose:
 *      Get the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (OUT) structure describes port PG configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2_port_priority_group_config_get(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    bcm_port_t port;
    uint32 rval, pri_bmp;

    if (!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    if ((priority_group < TD2_PRIOROTY_GROUP_ID_MIN) || 
        (priority_group > TD2_PRIOROTY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }
    if (prigrp_config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_INPUT_PORT_XON_ENABLESr(unit, port, &rval));
    pri_bmp = soc_reg_field_get(unit, THDI_INPUT_PORT_XON_ENABLESr, rval,
                                PORT_PRI_XON_ENABLEf);
    if (pri_bmp & (1U << priority_group)) {
        prigrp_config->pfc_transmit_enable = 1;
    } else {
        prigrp_config->pfc_transmit_enable = 0;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_cosq_subport_flow_control_set
 * Purpose:
 *      Set a port or node (L0, L1, L2) to flow control when
 *      a CoE control frame is received on a subtended port.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      subport            - (IN) Subtended port
 *      sched_port      - (IN) Node (Port, L0, L1, L2) to flow control 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_td2p_cosq_subport_flow_control_set(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t sched_port)
{
#ifdef BCM_HGPROXY_COE_SUPPORT 
    uint16 coe_fc_index, row, column;
    uint32 rval;
    bcm_gport_t subport_id, resolved_port;
    _bcm_td2_cosq_node_t *node; 
    int hw_index, rv, pipe;
    soc_td2_node_lvl_e level;
    subport_tag_sgpp_map_entry_t entry;
    bcm_port_t phy_port, mmu_port;
    soc_info_t *si;
    _bcm_td2_fc_type_t fct;

    /* Find subport id from port to index into SUBPORT_TAG_SGPP_MAP */
    BCM_IF_ERROR_RETURN(_bcmi_coe_subport_tcam_idx_get(unit, subport, &subport_id));

    /* From sched_port, need to find if it's port, L0, L1, or L2 */
    BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, sched_port, 0, NULL, NULL, NULL, &node));

    /* Port, L0, L1, or L2 from node */
    level = node->level;

    /* index into L0, L1, or L2 */
    hw_index = node->hw_index;

    /* Get the port associated with the passed in gport */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, sched_port, &resolved_port));
    if (resolved_port < 0) {
        return BCM_E_PORT;
    }

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, resolved_port);


    si = &SOC_INFO(unit);

    /* Getting MMU port.  Used to disable cut through for it at the end as well as
       to index into MMU_INTFI_PORT_COEFC_TB if we're setting flow control for port */
    phy_port = si->port_l2p_mapping[resolved_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (IS_TD2_HSP_PORT( unit, resolved_port )) {
        /* SC/QM L1/L2 nodes do not support FC.
        * Hence, L1/L2 indices 8, 9, 18, 19 ... should not be configured
        * for FC.
        * similarly the last L0 of the HSP port does not support flow control
        */
        if (level == SOC_TD2_NODE_LVL_L0) {
            if ((hw_index % 5) > 3) {
                 return BCM_E_PARAM;
            }
            column = hw_index % 5;
        } else  { 
            if ((hw_index % 10) > 7) {
                 return BCM_E_PARAM;
            }
            column = ((hw_index % 10) % 4);
        }
    } else { 
         column = hw_index % 4 ;
    }

    /* Checking valid reference to address for each entry of FC_STATE tables per
       MMU - INTFI Table 1-9.  We also need to offset the hw_index properly per pipe.
       We are dividing the FC STATE tables in half with the X Pipe taking the lower half
       and the Y Pipe taking the upper half, hence 134/2, 256/2, 1024/2. */
    switch(level) {
        case SOC_TD2_NODE_LVL_ROOT:
            /* At the port level we have to do it a bit different since it's indexing into
               MMU_INTFI_PORT_COEFC_TB, which is a 128 entry x 1 bit table with each entry signifying
               an MMU Port, so we need to use the MMU Port number as the row. Column we don't care
               for. */
            row = mmu_port;
            column = 0;

            if ( (row > 52 && row < 64) || (row > 116) ) {
                return BCM_E_PARAM;
            }
            break;
        case SOC_TD2_NODE_LVL_L0:
            if (IS_TD2_HSP_PORT( unit, resolved_port )) {
                /* The last L0 does not support  FC and so each index in fc_st_tbl represents
                 * 5 l0 nodes instead of 4 the last 1 is masked by the hardware
                 */ 
                row = hw_index / 5;
            } else { 
                row = hw_index / 4; 
            }

            if(pipe == _TD2_YPIPE) {
                row += 134/2;
            }
            if (row > 133) {
                return BCM_E_PARAM;
            }
            break;
    case SOC_TD2_NODE_LVL_L1:

        if (IS_TD2_HSP_PORT( unit, resolved_port )) {
            /* FC not supported on last two L1/L2 nodes The hardware internally adjusts the value 
             * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
             */   
            row = mmu_port * 2 + 
              (((hw_index % 10) >= 4)? 1 : 0);
        } else { 
            row = hw_index / 4; 
        }
            if(pipe == _TD2_YPIPE) {
                row += 512/2;
            }
            if (row > 511) {
                return BCM_E_PARAM;
            }

            break;
        case SOC_TD2_NODE_LVL_L2:
            if (IS_TD2_HSP_PORT( unit, resolved_port )) {
               /* FC not supported on last two L1/L2 nodes The hardware internally adjusts the value 
                * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
                */   
                row = mmu_port * 2 + 
                      (((hw_index % 10) >= 4)? 1 : 0);
            } else { 
               row = hw_index / 4; 
            }
            if(pipe == _TD2_YPIPE) {
                row += 1024/2;
            }
            if (row > 1023) {
                return BCM_E_PARAM;
            }
            break;
        default:
            return BCM_E_PARAM;

    }

    /* Column is only 4 bits wide */
    if (column > 3) {
        return BCM_E_PARAM;
    }

    /* set coe_fc_index for us to program into the register */
    coe_fc_index = (row << 5) |     /* row bits 15:5 */
                   (column << 2) |  /* column bits 4:2 */
                   (level);         /* type bits 1:0 */

    /* Program SUBPORT_TAG_SGPP_MAP with the subport as the index and
       COE_FC_INDEX as the data */
    rv = READ_SUBPORT_TAG_SGPP_MAPm(unit,
             MEM_BLOCK_ANY, subport_id, &entry);
    if (SOC_SUCCESS(rv)) {
        soc_SUBPORT_TAG_SGPP_MAPm_field32_set(unit,
                                             (uint32 *)&entry, COE_FC_INDEXf, coe_fc_index);
    }
    WRITE_SUBPORT_TAG_SGPP_MAPm(unit, MEM_BLOCK_ANY, subport_id, &entry);

    /* Program FC Map Table to select FC State Table (vs PFC State Table) for the node */
    fct = _BCM_TD2_COSQ_FC_E2ECC;

    /* To pass the proper FC STATE Table entries, we're modifying the FC MAP Table, one of its
       4 indexes, with the appropriate row in the FC STATE Table. FC MAP Table at index 0
       will handle the rows 0-3 of FC STATE Table, at index 1 will handle rows 4-7 of the
       FC STATE Table, and so on.
     
       resolved_port is passed so that the function will know which pipe to use (X or Y).
       fct will select FC STATE Table on the mux so that PFC STATE Table won't be selected.
       row is passed in this first time so that a proper row index for the FC STATE Table will be selected.
       node->hw_index is passed in because the proper FC MAP Table index will be calculated.
       node->level is passed in so that the function will know with FC MAP Table (L0, L1, L2) to use.
       */
    if(level != SOC_TD2_NODE_LVL_ROOT) {
        BCM_IF_ERROR_RETURN(_bcm_td2_map_fc_status_to_node(unit, resolved_port, fct, row, 
                                                           0, node->hw_index, node->level));
    }

    /* Disable Cut-Through for the port specified or L0/L1/L2 node associated with it */
    BCM_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, mmu_port, &rval));
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, UC_ASF_ENABLEf, 0);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, MC_ASF_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, mmu_port, rval));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_HGPROXY_COE_SUPPORT */
}

int
bcm_td2p_cosq_subport_flow_control_get(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t * sched_port)
{
#ifdef BCM_HGPROXY_COE_SUPPORT 
    int row, column, level, hw_index, i, temp_pipe, coe_fc_index = 0;
    int pipe = _TD2_XPIPE;
    bcm_gport_t subport_id;
    _bcm_td2_cosq_node_t * node;
    _bcm_td2_mmu_info_t * mmu_info;
    subport_tag_sgpp_map_entry_t entry;

    int resolved_port = 0;
    int mmu_port = 0;
    int phy_port = 0;
    soc_info_t *si;

    /* Find subport id from port to index into SUBPORT_TAG_SGPP_MAP */
    BCM_IF_ERROR_RETURN(_bcmi_coe_subport_tcam_idx_get(unit, subport, &subport_id));

    BCM_IF_ERROR_RETURN(
    _bcmi_coe_subport_physical_port_get(unit, subport, &resolved_port));
    si = &SOC_INFO(unit);
    /* Getting MMU port.  Used to disable cut through for it at the end as well as
       to index into MMU_INTFI_PORT_COEFC_TB if we're setting flow control for port */
    phy_port = si->port_l2p_mapping[resolved_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN(READ_SUBPORT_TAG_SGPP_MAPm(unit,
             MEM_BLOCK_ANY, subport_id, &entry));
    coe_fc_index = soc_SUBPORT_TAG_SGPP_MAPm_field32_get(unit,
                                             (uint32 *)&entry, COE_FC_INDEXf);

    level = coe_fc_index & 0x3;
    column = (coe_fc_index >> 2) & 0x7;
    row = (coe_fc_index >> 5);

    /* Get the proper row to calculate hw_index because this could be in either pipe.
       We are dividing the FC STATE tables in half with the X Pipe taking the lower half
       and the Y Pipe taking the upper half, hence 134/2, 256/2, 1024/2. */
    switch(level) {
        case SOC_TD2_NODE_LVL_ROOT:
            if(row >= 64) {
                row -= 64;
                pipe = _TD2_YPIPE;
            }

            hw_index = row;

            break;
        case SOC_TD2_NODE_LVL_L0:
            if(row >= 134/2) {
                pipe = _TD2_YPIPE;
                row -= 134/2;
            }
            if (row > 133) {
                return BCM_E_PARAM;
            }

            if (IS_TD2_HSP_PORT( unit, resolved_port )) {
                hw_index = row * 5 + column ; 
            } else {  
                hw_index = row * 4 + column ; 
            }

            break;
        case SOC_TD2_NODE_LVL_L1:
            if(row >= 512/2) {
                pipe = _TD2_YPIPE;
                row -= 512/2;
            }
            if (row > 511) {
                return BCM_E_PARAM;
            }        
            /* FC not supported on last two L1/L2 nodes The hardware internally adjusts the value 
            * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
            */   
            if (IS_TD2_HSP_PORT( unit, resolved_port )) {
                hw_index = ((mmu_port%_TD2_PORTS_PER_PIPE) * 10) + ((row % 2)? 4: 0) + column ; 
            } else {  
                hw_index = row * 4 + column ; 
            }
            break;
        case SOC_TD2_NODE_LVL_L2:
            if(row >= 1024/2) {
                pipe = _TD2_YPIPE;
                row -= 1024/2;
            }
            if (row > 1023) {
                return BCM_E_PARAM;
            }

            /* FC not supported on last two L1/L2 nodes The hardware internally adjusts the value 
            * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
            */   
            if (IS_TD2_HSP_PORT( unit, resolved_port )) {
                hw_index = ((mmu_port%_TD2_PORTS_PER_PIPE) * 10) + ((row % 2)? 4: 0) + column ; 
            } else {  
                hw_index = row * 4 + column ; 
            }
            break;
        /*
         * COVERITY
         *
         * This default is unreachable but needed for some compiler. 
         */ 
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_PARAM;

    }

    mmu_info = _bcm_td2_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    /* Fetch the node based upon level and hw_index */
    switch(level) {
        /* port, L0, and L1 are all scheduling nodes */
        case SOC_TD2_NODE_LVL_ROOT:
        case SOC_TD2_NODE_LVL_L0:
        case SOC_TD2_NODE_LVL_L1:
            for(i = 0; i < _BCM_TD2_NUM_TOTAL_SCHEDULERS; i++) {
                node = &mmu_info->sched_node[i];
                if(node->level == level) {
                    if(node->hw_index == hw_index) {
                        /* if the hw_index matches, check the pipe. Must pass in local port */
                        temp_pipe = _BCM_TD2_PORT_TO_PIPE(unit, node->local_port);

                        /* if the pipe matches too, we've found the associated gport */
                        if(pipe == temp_pipe) {
                            *sched_port = node->gport;
                            return BCM_E_NONE;
                        }
                    }
                }
            }
            break;
        case SOC_TD2_NODE_LVL_L2:
            for(i = 0; i < _BCM_TD2_NUM_L2_UC_LEAVES; i++) {
                node = &mmu_info->queue_node[i];
                if(node->level == level) {
                    if(node->hw_index == hw_index) {
                        /* if the hw_index matches, check the pipe */
                        temp_pipe = _BCM_TD2_PORT_TO_PIPE(unit, node->local_port);

                        /* if the pipe matches too, we've found the associated gport */
                        if(pipe == temp_pipe) {
                            *sched_port = node->gport;
                            return BCM_E_NONE;
                        }
                    }
                }
            }
            break;
        /* Intentional process */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_PARAM;

    }

    /* not found */
    return BCM_E_PARAM;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_HGPROXY_COE_SUPPORT */
}

int
bcm_td2_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    _bcm_td2_mmu_info_t *mmu_info;
    soc_info_t *si;
    int count, idx, pipe, base;
    uint32 rval;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_pipe_resources_t *res;
    soc_mem_t mem;
    soc_reg_t reg;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 e2ecc_ds_config;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    uint32 qgroup_valid;
#endif

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    si = &SOC_INFO(unit);

    mem = SOC_TD2_PMEM(unit, port, MMU_THDM_DB_QUEUE_CONFIG_0m, 
                                   MMU_THDM_DB_QUEUE_CONFIG_1m);
    reg = SOC_TD2_PREG(unit, port, MMU_THDM_DB_QUEUE_E2E_DS_EN_0r,
                                   MMU_THDM_DB_QUEUE_E2E_DS_EN_1r);


    count = si->port_num_cosq[port];
    SOC_IF_ERROR_RETURN(_bcm_td2_cosq_index_resolve
                 (unit, port, 0, _BCM_TD2_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &port, &base, NULL));

    for (idx = 0; idx < count; idx++) {
        if ( SOC_REG_IS_VALID(unit, reg) ) {
            SOC_IF_ERROR_RETURN (
                soc_reg_field32_modify(unit, reg, port,
                                       Q_COS_E2E_DS_ENf, enable));
        } else {
            SOC_IF_ERROR_RETURN (
                soc_mem_field32_modify(unit, mem, base + idx - 1480,
                                        Q_E2E_DS_ENf, enable));
        }
    }

    mem = SOC_TD2_PMEM(unit, port, MMU_THDU_XPIPE_Q_TO_QGRP_MAPm, 
                                   MMU_THDU_YPIPE_Q_TO_QGRP_MAPm);
    SOC_IF_ERROR_RETURN(_bcm_td2_cosq_index_resolve
                 (unit, port, 0, _BCM_TD2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &port, &base, NULL));
    count = si->port_num_uc_cosq[port];
    for (idx = 0; idx < count; idx++) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry));
        soc_mem_field32_set(unit, mem, entry, Q_E2E_DS_EN_CELLf, !!enable);
        e2ecc_ds_config = 1;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            if (IS_TD2_HSP_PORT(unit, port)) {
                qgroup_valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
                if ((idx < 8) && (qgroup_valid == 0)) {
                    /* If HSP port does not use qgroup for its lower 8 queues */
                    e2ecc_ds_config = 2;
                }
            }
        }
#endif
        soc_mem_field32_set(unit, mem, entry, Q_E2ECC_DS_CONFIGf, e2ecc_ds_config);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx, entry));
    }

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    for (idx = res->num_base_queues; idx < _BCM_TD2_NUM_L2_UC_LEAVES; idx++) {
        node = &res->p_queue_node[idx];
        if ((node->in_use == TRUE) && (node->local_port == port)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, node->hw_index, entry));
            soc_mem_field32_set(unit, mem, entry, Q_E2E_DS_EN_CELLf, !!enable);
            soc_mem_field32_set(unit, mem, entry, Q_E2ECC_DS_CONFIGf, 1);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, node->hw_index, entry));
        }
    }

    BCM_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, EARLY_E2E_SELECTf,
                      enable ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_voq_min_hw_index(int unit, bcm_port_t port, 
                          bcm_module_t modid, 
                          bcm_port_t remote_port,
                          int *q_offset)
{
    int i, hw_index_base = -1, pipe;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_mmu_info_t *mmu_info;
    int qmin, qmax;

    mmu_info = _bcm_td2_mmu_info[unit];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
    qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
    qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
 
    for (i = qmin; i < qmax; i++) {
        node = &res->p_queue_node[i];
        if ((node->in_use == FALSE) || (node->hw_index == -1)) {
            continue;
        }

        if (((modid == -1) ? 1 : (node->remote_modid == modid)) && 
            (node->remote_port == remote_port) &&
            ((port == -1) ? 1 : (port == node->local_port))) {
            if (hw_index_base == -1) {
                hw_index_base = node->hw_index;
                break;
            }
        }
    }
   
    if (hw_index_base != -1) {
        *q_offset = hw_index_base & ~7;
        return BCM_E_NONE;
    }

    return BCM_E_NOT_FOUND;
}

STATIC int
_bcm_td2_voq_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_td2_cosq_node_t **base_node)
{
    int i,  pipe;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node;
    int qmin, qmax;

    mmu_info = _bcm_td2_mmu_info[unit];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    for (pipe = _TD2_XPIPE ; pipe < _TD2_NUM_PIPES ; pipe++) { 
        res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
        qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
        qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        for ( i  = qmin; 
                i < qmax ; i++) {
            node = &res->p_queue_node[i];
            if ((node->in_use == FALSE) || (node->hw_index == -1)) {
                continue;
            }
            if (node->remote_modid == modid) {
                *base_node = node ;
                break;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_voq_next_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_td2_cosq_node_t **base_node)
{
    int i,  pipe, qmin, qmax;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_cosq_node_t *node;
    int found = FALSE;

    mmu_info = _bcm_td2_mmu_info[unit];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    for (pipe = _TD2_XPIPE ; pipe < _TD2_NUM_PIPES ; pipe++) { 
        res = _BCM_TD2_PRESOURCES(mmu_info, pipe);
        qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
        qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
        for ( i  = qmin ;
                i < qmax ; i++) {
            node = &res->p_queue_node[i];
            if ((node->in_use == FALSE) || (node->hw_index == -1)) {
                continue;
            }
            if (node->remote_modid == modid) {
                if ( found == TRUE) {
                    *base_node = node;
                    break;
                }
                found = TRUE;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_port_voq_base_set(int unit, bcm_port_t local_port, int base)
{
    uint64 rval;
    int pipe;
    
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
    soc_reg64_field32_set(unit, ING_COS_MODE_64r, &rval, BASE_QUEUE_NUM_1f,
            pipe == _TD2_XPIPE ? base : base + 2048);
    BCM_IF_ERROR_RETURN(WRITE_ING_COS_MODE_64r(unit, local_port, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_td2_port_voq_base_get(int unit, bcm_port_t local_port, int *base)
{
    uint64 rval;
    int pipe;

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
    if (soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, QUEUE_MODEf) == 1) {
        *base = soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, 
                                  BASE_QUEUE_NUM_1f);
        if (pipe == _TD2_YPIPE) {
            *base -= 2048;
        }
 
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_td2_voq_min_hw_index(unit, local_port, -1, -1, base));

    return BCM_E_NONE;
}

STATIC int
_bcm_td2_resolve_dmvoq_hw_index(int unit, _bcm_td2_cosq_node_t *node, 
                    int cosq, bcm_module_t modid, bcm_port_t local_port)
{
    int try, preffered_offset = -1, alloc_size;
    int from_offset, max_offset,  port, rv, q_base, port_voq_base, pipe;
    _bcm_td2_pipe_resources_t *res;
    _bcm_td2_mmu_info_t *mmu_info;

    mmu_info = _bcm_td2_mmu_info[unit];
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);

    from_offset  = res->num_base_queues + 
        (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
    max_offset = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;
    alloc_size = _TD2_NUM_COS(unit);

    for (try = 0; try < 2; try++) {
        PBMP_HG_ITER(unit, port) {
            if ((try == 1) && (port == local_port)) {
                continue;
            }
            
            if ((try == 0) && (port != local_port)) {
                continue;
           }

            rv = _bcm_td2_voq_min_hw_index(unit, port, modid, -1, &q_base);
            if (!rv) {
                if (port == local_port) {
                    /* Found VOQ for the same port/dest modid */
                    node->hw_index = q_base + cosq;
                    return BCM_E_NONE;
                } else if (modid == -1) {
                    rv = _bcm_td2_port_voq_base_get(unit, port, &port_voq_base);
                    if (rv) {
                        continue;
                    }
                    preffered_offset = q_base - port_voq_base;
                    rv = _bcm_td2_port_voq_base_get(unit, local_port, 
                                                    &port_voq_base);
                    if (rv) {
                        /* No VOQ on the port yet */
                        alloc_size += preffered_offset;
                    } else {
                        /* port has already VOQs */
                        from_offset = port_voq_base + preffered_offset;
                    }
                    break;
                }
            }
        }
        if (preffered_offset != -1) {
            break;
        }
    }
    if (modid == -1) {
        /* Alloc hw index. */
        rv = _bcm_td2_node_index_get(res->ext_qlist.bits, 
                from_offset, max_offset, 
                alloc_size, _TD2_NUM_COS(unit), &q_base);
        if (rv) {
            return BCM_E_RESOURCE;
        }

        if (_bcm_td2_port_voq_base_get(unit, local_port, &port_voq_base)) {
            _bcm_td2_port_voq_base_set(unit, local_port, q_base);
        }
        q_base = q_base + alloc_size - _TD2_NUM_COS(unit);
        node->hw_index = q_base + cosq;
        node->hw_cosq = node->hw_index % _TD2_NUM_COS(unit);
        node->remote_modid = modid;
        _bcm_td2_node_index_set(&res->ext_qlist, q_base, _TD2_NUM_COS(unit));
    } else {
        _bcm_td2_port_voq_base_set(unit, local_port, node->hw_index);
        node->remote_modid = modid;
        rv = BCM_E_NONE;
    }
    return rv;
}

typedef struct _bcm_td2_voq_info_s {
    int          valid;
    _bcm_td2_cosq_node_t *node;
    bcm_module_t remote_modid;
    bcm_port_t   remote_port;
    bcm_port_t   local_port;
    int          hw_cosq;
    int          hw_index;
} _bcm_td2_voq_info_t;

STATIC int
_bcm_td2_cosq_sort_voq_list(void *a, void *b)
{
    _bcm_td2_voq_info_t *va, *vb;
    int sa, sb;

    va = (_bcm_td2_voq_info_t*)a;
    vb = (_bcm_td2_voq_info_t*)b;
    
    sa = (va->remote_modid << 19) | (va->remote_port << 11) |
         (va->local_port << 3) | (va->hw_cosq);

    sb = (vb->remote_modid << 19) | (vb->remote_port << 11) |
         (vb->local_port << 3) | (vb->hw_cosq);

    return sa - sb;
}
    
STATIC int
_bcm_td2_get_queue_skip_count_on_cos(uint32 cbmp, int cur_cos, int next_cos)
{
    int i,skip = 0;

    if ((next_cos >= 4) && (cur_cos/4 != next_cos/4)) {
        cur_cos = -1;
        next_cos -= 4;
    }
    
    for (i = cur_cos + 1; i < next_cos; i++) {
        if (cbmp & (1 << i)) {
            skip += 1;
        }
    }
    return skip;
}

STATIC int
_bcm_td2_msg_buf_get(int unit, bcm_port_t srcid)
{
    int ii, hole = -1;
    uint32 rval, csrc;

    for (ii = 0; ii < 16; ii++) {
        BCM_IF_ERROR_RETURN(READ_BUF_CFGr(unit, ii, &rval));
        if (srcid == (csrc = soc_reg_field_get(unit, BUF_CFGr, rval, SRC_IDf))) {
            return ii;
        }
        if ((hole == -1) && (csrc == 0)) {
            hole = ii;
        }
    }

    if (hole == -1) {
        return BCM_E_RESOURCE;
    }
   
    rval = 0;
    soc_reg_field_set(unit, BUF_CFGr, &rval, SRC_IDf, srcid);
    BCM_IF_ERROR_RETURN(WRITE_BUF_CFGr(unit, hole, rval));
    
    return hole;
}

STATIC int
_bcm_td2_gport_dpvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t remote_modid,
                                bcm_port_t remote_port)
{
    uint32 profile = 0xFFFFFFFF;
    _bcm_td2_cosq_node_t *node;
    bcm_port_t local_port;
    uint32 cng_offset;
    uint8 cbmp = 0, nib_cbmp = 0;
    _bcm_td2_mmu_info_t *mmu_info;
    _bcm_td2_voq_info_t *plist = NULL, *vlist = NULL;
    int pcount, ii, vcount = 0;
    voq_port_map_entry_t *voq_port_map_entries = NULL;
    int rv = BCM_E_NONE, mod_base = 0, num_cos = 0, dm = -1, eq_base;
    voq_mod_map_entry_t voq_mod_map;
    void *entries[1];
    int cng_bytes_per_e2ecc_msg = 32;
    int p2ioff[64], ioff, port, skip, *dm2off, count, pipe;
    _bcm_td2_pipe_resources_t *res;

    if ((cosq < 0) || (cosq > _TD2_NUM_COS(unit))) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, local_port);
    res = _BCM_TD2_PRESOURCES(mmu_info, pipe);

    eq_base = (res->num_base_queues + 7) & ~7;
    
    node->remote_modid = remote_modid;
    node->remote_port = remote_port;
    node->hw_cosq = cosq;

    dm2off = sal_alloc(sizeof(int) * 256, "voq_vlist");
    if (!dm2off) {
        return BCM_E_MEMORY;
    }
    sal_memset(dm2off, 0, sizeof(int)*256);

    /* alloc temp vlist */
    vlist = sal_alloc(sizeof(_bcm_td2_voq_info_t) * 1480, "voq_vlist");
    if (!vlist) {
        sal_free(dm2off);
        return BCM_E_MEMORY;
    }
    sal_memset(vlist, 0, sizeof(_bcm_td2_voq_info_t)*1480);

    for (ii = 0; ii < _BCM_TD2_NUM_L2_UC_LEAVES; ii++) {
        node = &res->p_queue_node[ii];
        if ((node->remote_modid == -1) || (node->remote_port == -1)) {
            continue;
        }
        vlist[vcount].remote_modid = node->remote_modid;
        vlist[vcount].remote_port  = node->remote_port; 
        vlist[vcount].local_port   = node->local_port;
        vlist[vcount].hw_cosq     = node->hw_cosq; 
        vlist[vcount].node         = node;
        vcount += 1;
    }

    /* Sort the nodes */
    _shr_sort(vlist, vcount, sizeof(_bcm_td2_voq_info_t), 
                _bcm_td2_cosq_sort_voq_list);

    for (ii = 0; ii < 64; ii++) {
        p2ioff[ii] = -1;
    }
    
    PBMP_HG_ITER(unit, port) {
        ioff = _bcm_td2_msg_buf_get(unit, port);
        p2ioff[port] = ioff;
    }

    for (ii = 0; ii < vcount; ii++) {
        cbmp |= 1 << vlist[ii].hw_cosq;
    }

    nib_cbmp = (cbmp >> 4) | (cbmp & 0xf);

    for (ii = 0; ii < 4; ii++) {
        if ((1 << ii) & nib_cbmp) {
            num_cos += 1;
        }
    }

    for (count = 1, ii = 0; ii < vcount; ii++) {
        if (dm2off[vlist[ii].remote_modid]==0) {
            dm2off[vlist[ii].remote_modid] = count;
        }
    }

    plist = sal_alloc(sizeof(_bcm_td2_voq_info_t) * 1480, "voq_vlist");
    if (!plist) {
        sal_free(dm2off);
        sal_free(vlist);
        return BCM_E_MEMORY;
    }
    sal_memset(plist, 0, sizeof(_bcm_td2_voq_info_t)*1480);

    for (pcount = 0, ii = 0; ii < vcount; ii++) {
        if (pcount % 4) {
            if ((plist[pcount - 1].remote_modid != vlist[ii].remote_modid) ||
                (plist[pcount - 1].remote_port != vlist[ii].remote_port)) {
                pcount = (pcount + 3) & ~3;
            } else if (plist[pcount - 1].local_port != vlist[ii].local_port) {
                skip = (p2ioff[vlist[ii].local_port] - 
                            p2ioff[plist[pcount - 1].local_port] - 1) * num_cos;
                skip +=  _bcm_td2_get_queue_skip_count_on_cos(nib_cbmp, 
                                        plist[pcount - 1].hw_cosq,
                                        vlist[ii].hw_cosq);
                if (skip >= 4) {
                    pcount = ((pcount + 3) & ~3) + (skip % 4);
                } else {
                    pcount += skip;
                }
            } else if ((vlist[ii].hw_cosq - plist[pcount - 1].hw_cosq) > 1) {
                if (plist[pcount - 1].hw_cosq/4 != vlist[ii].hw_cosq/4) {
                    pcount = (pcount + 3) & ~3;
                }
                pcount += _bcm_td2_get_queue_skip_count_on_cos(nib_cbmp, 
                                        plist[pcount - 1].hw_cosq,
                                        vlist[ii].hw_cosq);
            }
        } else {
            pcount += _bcm_td2_get_queue_skip_count_on_cos(nib_cbmp, -1,
                                                    vlist[ii].hw_cosq);
        }
       
        plist[pcount].valid = 1;
        plist[pcount].local_port = vlist[ii].local_port;
        plist[pcount].remote_modid = vlist[ii].remote_modid;
        plist[pcount].remote_port = vlist[ii].remote_port;
        plist[pcount].hw_cosq = vlist[ii].hw_cosq;
        plist[pcount].node = vlist[ii].node;
        vlist[ii].node->hw_index = pcount + eq_base;
        
        #ifdef DEBUG_TD2_COSQ
          LOG_CLI((BSL_META_U(unit,
                              "DPVOQ DM=%d DP=%d LocalPort=%d COS=%d HWIndex=%d\n"),
                   plist[pcount].remote_modid,
                   plist[pcount].remote_port,
                   plist[pcount].local_port,
                   plist[pcount].hw_cosq,
                   plist[pcount].node->hw_index));
        #endif
        pcount += 1;
    }

    /* program the voq port profiles. */
    voq_port_map_entries = sal_alloc(sizeof(voq_port_map_entry_t)*128, 
                                        "voq port map entries");
    if (!voq_port_map_entries) {
        goto fail;
    }
    sal_memset(voq_port_map_entries, 0, sizeof(voq_port_map_entry_t)*128);

    entries[0] = voq_port_map_entries;
    dm = -1;
    for (ii = 0; ii <= pcount; ii++) {
        if ((ii == pcount) ||
            ((dm != -1) && (plist[ii - 1].remote_modid != dm))) {
            
            rv = READ_VOQ_MOD_MAPm(unit, MEM_BLOCK_ALL, dm, &voq_mod_map);
            if (rv) {
                goto fail;
            }     
            if (soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                                                &voq_mod_map, VOQ_VALIDf)) {
                profile = soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                              &voq_mod_map, VOQ_MOD_PORT_PROFILE_INDEXf);
                
                rv = soc_profile_mem_delete(unit, 
                            _bcm_td2_voq_port_map_profile[unit], profile*128);
                if (rv) {
                    goto fail;
                }
            }
            rv = soc_profile_mem_add(unit, 
                                _bcm_td2_voq_port_map_profile[unit], 
                                entries, 128, &profile);
            if (rv) {
                goto fail;
            }
           
            soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                                VOQ_MOD_PORT_PROFILE_INDEXf, profile/128);
            soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                                VOQ_VALIDf, 1);
            soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                                VOQ_MOD_OFFSETf, mod_base);
            rv = soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, 
                               dm, &voq_mod_map);
            if (rv) {
                goto fail;
            }
            sal_memset(voq_port_map_entries, 0, sizeof(voq_port_map_entry_t)*128);
            if (ii != pcount) {
                mod_base = plist[ii].hw_index & ~3;
                dm = plist[ii].remote_modid;
            }
        }

        if (ii == pcount) {
            break;
        }

        if (plist[ii].valid == 0) {
            continue;
        }

        dm = plist[ii].remote_modid;
        soc_mem_field32_set(unit, VOQ_PORT_MAPm, 
                            &voq_port_map_entries[plist[ii].remote_port], 
                            VOQ_PORT_OFFSETf, 
                            plist[ii].node->hw_index - mod_base);
    }

    dm = -1;
    for (ii = 0; ii < pcount; ii += 1) {
        if (plist[ii].valid == 0) {
            continue;
        }

        cng_offset = (dm2off[plist[ii].remote_modid] - 1) * 
                        cng_bytes_per_e2ecc_msg;

        _bcm_td2_map_fc_status_to_node(unit, 
                                       local_port,
                                       _BCM_TD2_COSQ_FC_E2ECC,
                                       cng_offset + plist[ii].remote_port, 
                                       plist[ii].hw_cosq,
                                       plist[ii].node->hw_index,
                                       SOC_TD2_NODE_LVL_L2);
    }
   
fail:
    if (vlist) {
        sal_free(vlist);
    }

    if (plist) {
        sal_free(plist);
    }

    if (dm2off) {
        sal_free(dm2off);
    }
    
    if (voq_port_map_entries) {
        sal_free(voq_port_map_entries);
    }

    if (rv) {
        node->remote_modid = -1;
        node->remote_port = -1;
    }
    return rv;
}

STATIC int
_bcm_td2_gport_dmvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t fabric_modid, 
                                bcm_module_t dest_modid,
                                bcm_port_t fabric_port)
{
    int intf_index;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_cosq_node_t *base_node = NULL;
    bcm_port_t local_port;
    voq_mod_map_entry_t voq_mod_map_entry;
    mmu_intfi_offset_map_tbl_entry_t offset_map_tbl_entry;
    mmu_intfi_base_index_tbl_entry_t base_tbl_entry;
    uint32 cng_offset = 0;
    _bcm_td2_mmu_info_t *mmu_info;
    int fabric_port_count, count = 0;
    int map_offset = 0;
    int port = 0;

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if (fabric_modid >= 64) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_td2_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));

    if (!node) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_td2_resolve_dmvoq_hw_index(unit, node, cosq,
                dest_modid, local_port));
    BCM_IF_ERROR_RETURN(_bcm_td2_voq_base_node_get(
                unit, local_port, dest_modid, &base_node));
    if (base_node == NULL) {
        base_node = node ;
    }
    BCM_PBMP_COUNT(base_node->fabric_pbmp, fabric_port_count);
    if (fabric_port_count == 0) {
        map_offset = mmu_info->curr_merger_index;
        if (map_offset >= soc_mem_index_count(unit, MMU_INTFI_MERGE_ST_TBLm)) {
            return BCM_E_RESOURCE;
        }
    } else {
        BCM_PBMP_ITER(base_node->fabric_pbmp, port) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port * 2, &offset_map_tbl_entry));
            map_offset = soc_mem_field32_get(unit, MMU_INTFI_OFFSET_MAP_TBLm,
                    &offset_map_tbl_entry,
                    MAP_OFFSETf);
            break;
        }
    }
    if (!BCM_PBMP_MEMBER(base_node->fabric_pbmp, fabric_port)) {
        if (map_offset == mmu_info->curr_merger_index ) {
            mmu_info->curr_merger_index += 2;
        }

        BCM_PBMP_PORT_ADD (base_node->fabric_pbmp, fabric_port);
        BCM_PBMP_COUNT(base_node->fabric_pbmp, fabric_port_count);
        BCM_PBMP_ITER(base_node->fabric_pbmp, port) {
            count++;
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port * 2, &offset_map_tbl_entry));
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    MAP_OFFSETf, map_offset);
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    LASTf, count < fabric_port_count ? 0 : 1 );
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port * 2, &offset_map_tbl_entry));

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        (port * 2) + 1, &offset_map_tbl_entry));
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    MAP_OFFSETf, map_offset + 1);
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    LASTf, count < fabric_port_count ? 0 : 1 );
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        (port * 2) + 1, &offset_map_tbl_entry));
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, 
                dest_modid, &voq_mod_map_entry));
    soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map_entry,
            VOQ_VALIDf, 1);

    soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map_entry,
            VOQ_MOD_OFFSETf, 0);

    BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, dest_modid, 
                &voq_mod_map_entry));

    intf_index = _bcm_td2_msg_buf_get(unit, fabric_modid);
    if (intf_index < 0) {
        return BCM_E_INTERNAL;
    }

    cng_offset = intf_index * 64 * 2;

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_INTFI_BASE_INDEX_TBLm, 
                      MEM_BLOCK_ALL, fabric_modid, &base_tbl_entry));
    
    soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
            BASE_ADDRf, cng_offset);
    soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
            ENf, 2);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_BASE_INDEX_TBLm,
                MEM_BLOCK_ALL, fabric_modid, &base_tbl_entry));

    /* set the mapping from congestion bits in flow control table
     * to corresponding cosq. */
    BCM_IF_ERROR_RETURN(_bcm_td2_map_fc_status_to_node(unit, local_port,
                _BCM_TD2_COSQ_FC_VOQFC, cng_offset + map_offset, 
                cosq, node->hw_index, SOC_TD2_NODE_LVL_L2));

    #ifdef DEBUG_TD2_COSQ
    LOG_CLI((BSL_META_U(unit,
                        "Gport=0x%08x hw_index=%d cosq=%d\n"),
             gport, node->hw_index, cosq));
    #endif

    return BCM_E_NONE;
}

int bcm_td2_cosq_gport_congestion_config_set(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    _bcm_td2_mmu_info_t *mmu_info;

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    /* determine if this gport is DPVOQ or DMVOQ */
    if ((config->fabric_port != -1) && (config->dest_modid != -1)) {
        return _bcm_td2_gport_dmvoq_config_set(unit, gport, cosq, 
                                               config->fabric_modid,
                                               config->dest_modid,
                                               config->fabric_port);
    } else if ((config->dest_modid != -1) && (config->dest_port != -1)) {
        return _bcm_td2_gport_dpvoq_config_set(unit, gport, cosq, 
                                               config->dest_modid,
                                               config->dest_port);
    }
    
    return BCM_E_PARAM;
}

int bcm_td2_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    _bcm_td2_mmu_info_t *mmu_info;
    bcm_port_t local_port;
    _bcm_td2_cosq_node_t *node;

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_td2_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_td2_cosq_node_get(unit, port, 0, NULL, &local_port, NULL, &node));


    if (!node) {
        return BCM_E_NOT_FOUND;
    }

    config->dest_modid = node->remote_modid;
    config->dest_port = node->remote_port;

    return BCM_E_NONE;
}



int bcm_td2_cosq_bst_profile_set(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set
        (unit, port, cosq, BCM_BST_DUP_XPE, bid, profile));
    return BCM_E_NONE;
}

int bcm_td2_cosq_bst_profile_get(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(_bcm_bst_cmn_profile_get
        (unit, port, cosq, BCM_BST_DUP_XPE, bid, profile));
    return BCM_E_NONE;
}

int bcm_td2_cosq_bst_stat_get(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid, 
                              uint32 options,
                              uint64 *pvalue)
{
    return _bcm_bst_cmn_stat_get(unit, port, cosq, -1, bid, options, pvalue);
}

int bcm_td2_cosq_bst_stat_multi_get(int unit,
                                bcm_gport_t port,
                                bcm_cos_queue_t cosq,
                                uint32 options,
                                int max_values,
                                bcm_bst_stat_id_t *id_list,
                                uint64 *pvalues)
{
    return _bcm_bst_cmn_stat_multi_get(unit, port, cosq, options, max_values, 
                                    id_list, pvalues);
}

int bcm_td2_cosq_bst_stat_clear(int unit, bcm_gport_t port, 
                            bcm_cos_queue_t cosq, bcm_bst_stat_id_t bid)
{
    return _bcm_bst_cmn_stat_clear(unit, port, cosq, bid);
}

int bcm_td2_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid)
{
    return _bcm_bst_cmn_stat_sync(unit, bid);
}


/*
 * Function:
 *      _bcm_td2_cosq_endpoint_l2_create
 * Purpose:
 *      Create a L2 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      endpoint_id - (IN) Endpoint ID
 *      vlan        - (IN) VLAN
 *      mac         - (IN) MAC address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_l2_create(
    int unit, 
    int endpoint_id,
    bcm_vlan_t vlan,
    bcm_mac_t mac)
{
    int rv;
    bcm_l2_addr_t l2addr;
    l2_endpoint_id_entry_t l2_endpoint_entry;
    int vfi;

    /* Make sure (vlan, mac) entry is already in L2 table */
    rv = bcm_esw_l2_addr_get(unit, mac, vlan, &l2addr);
    if (rv == BCM_E_NOT_FOUND) {
        /* The (vlan, mac) entry needs to be added to L2 table
         * before configuring it for endpoint queuing.
         */
        return BCM_E_CONFIG;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Add (vlan, mac) entry to L2_ENDPOINT_ID table */
    sal_memcpy(&l2_endpoint_entry, soc_mem_entry_null(unit, L2_ENDPOINT_IDm),
            sizeof(l2_endpoint_id_entry_t));
    soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, VALIDf, 1);
    if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
        _BCM_MPLS_VPN_GET(vfi, _BCM_MPLS_VPN_TYPE_VPLS, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VFIf,
                vfi);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_VFI);
    } else if (_BCM_IS_MIM_VPN(vlan)) {
        _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VFIf,
                vfi);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_VFI);
    } else {
        VLAN_CHK_ID(unit, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VLAN_IDf,
                vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_BRIDGE);
    }
    soc_L2_ENDPOINT_IDm_mac_addr_set(unit, &l2_endpoint_entry, L2__MAC_ADDRf,
            mac);
    soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, EH_TAG_TYPEf, 2);
    soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, EH_QUEUE_TAGf,
            endpoint_id);
    SOC_IF_ERROR_RETURN(soc_mem_insert(unit, L2_ENDPOINT_IDm, MEM_BLOCK_ANY,
                &l2_endpoint_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_l2_destroy
 * Purpose:
 *      Destroy a L2 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      vlan        - (IN) VLAN
 *      mac         - (IN) MAC address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_l2_destroy(
    int unit, 
    bcm_vlan_t vlan,
    bcm_mac_t mac)
{
    l2_endpoint_id_entry_t l2_endpoint_entry;
    int vfi;

    /* Delete (vlan, mac) entry from L2_ENDPOINT_ID table */
    sal_memcpy(&l2_endpoint_entry, soc_mem_entry_null(unit, L2_ENDPOINT_IDm),
            sizeof(l2_endpoint_id_entry_t));
    if (_BCM_MPLS_VPN_IS_VPLS(vlan)) {
        _BCM_MPLS_VPN_GET(vfi, _BCM_MPLS_VPN_TYPE_VPLS, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VFIf,
                vfi);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_VFI);
    } else if (_BCM_IS_MIM_VPN(vlan)) {
        _BCM_MIM_VPN_GET(vfi, _BCM_MIM_VPN_TYPE_MIM, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VFIf,
                vfi);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_VFI);
    } else {
        VLAN_CHK_ID(unit, vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, L2__VLAN_IDf,
                vlan);
        soc_L2_ENDPOINT_IDm_field32_set(unit, &l2_endpoint_entry, KEY_TYPEf,
                TD2_L2_HASH_KEY_TYPE_BRIDGE);
    }
    soc_L2_ENDPOINT_IDm_mac_addr_set(unit, &l2_endpoint_entry, L2__MAC_ADDRf,
            mac);
    SOC_IF_ERROR_RETURN(soc_mem_delete(unit, L2_ENDPOINT_IDm, MEM_BLOCK_ANY,
                &l2_endpoint_entry));

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3
/*
 * Function:
 *      _bcm_td2_cosq_endpoint_ip4_create
 * Purpose:
 *      Create an IPv4 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      endpoint_id - (IN) Endpoint ID
 *      vrf         - (IN) Virtual router instance
 *      ip_addr     - (IN) IPv4 address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_ip4_create(
    int unit, 
    int endpoint_id,
    bcm_vrf_t vrf,
    bcm_ip_t ip_addr)
{
    int rv;
    _bcm_l3_cfg_t l3cfg;

    if ((vrf > SOC_VRF_MAX(unit)) || 
        (vrf < BCM_L3_VRF_DEFAULT)) {
        return BCM_E_PARAM;
    }

    L3_LOCK(unit);

    /* Make sure (vrf, ip_addr) entry is already in L3 host table */
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_vrf = vrf;
    l3cfg.l3c_ip_addr = ip_addr;
    rv = mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg);
    if (rv == BCM_E_NOT_FOUND) {
        /* The (vrf, ip_addr) entry needs to be added to L3 table
         * before configuring it for endpoint queuing.
         */
        L3_UNLOCK(unit);
        return BCM_E_CONFIG;
    } else if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    } else {
        /* Make sure the L3 ENTRY is in extender view */
        if (!BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg.l3c_intf, 
                                              BCM_XGS3_L3_INVALID_INDEX)) {
            L3_UNLOCK(unit);
            return BCM_E_CONFIG;
        }
    }

    if (l3cfg.l3c_eh_q_tag_type != 0) {
        /* A queue tag has already been configured for this L3 entry. */
        L3_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    /* Replace (vrf, ip_addr) entry's endpoint_id */
    l3cfg.l3c_flags |= BCM_L3_REPLACE;
    l3cfg.l3c_eh_q_tag_type = 2;
    l3cfg.l3c_eh_q_tag = endpoint_id;
    rv = mbcm_driver[unit]->mbcm_l3_ip4_replace(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    }

    L3_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_ip4_destroy
 * Purpose:
 *      Destroy an IPv4 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      vrf         - (IN) Virtual router instance
 *      ip_addr     - (IN) IPv4 address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_ip4_destroy(
    int unit, 
    bcm_vrf_t vrf,
    bcm_ip_t ip_addr)
{
    int rv;
    _bcm_l3_cfg_t l3cfg;

    if ((vrf > SOC_VRF_MAX(unit)) || 
        (vrf < BCM_L3_VRF_DEFAULT)) {
        return BCM_E_PARAM;
    }

    L3_LOCK(unit);

    /* Get (vrf, ip_addr) entry */
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_vrf = vrf;
    l3cfg.l3c_ip_addr = ip_addr;
    rv = mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    } else {
        /* Make sure the L3 ENTRY is in extender view */
        if (!BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg.l3c_intf, 
                                              BCM_XGS3_L3_INVALID_INDEX)) {
            L3_UNLOCK(unit);
            return BCM_E_CONFIG;
        }
    }

    /* Delete (vrf, ip_addr) entry's endpoint_id */
    l3cfg.l3c_flags |= BCM_L3_REPLACE;
    l3cfg.l3c_eh_q_tag_type = 0;
    l3cfg.l3c_eh_q_tag = 0;
    rv = mbcm_driver[unit]->mbcm_l3_ip4_replace(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    }

    L3_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_ip6_create
 * Purpose:
 *      Create an IPv6 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      endpoint_id - (IN) Endpoint ID
 *      vrf         - (IN) Virtual router instance
 *      ip6_addr    - (IN) IPv6 address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_ip6_create(
    int unit, 
    int endpoint_id,
    bcm_vrf_t vrf,
    bcm_ip6_t ip6_addr)
{
    int rv;
    _bcm_l3_cfg_t l3cfg;

    if ((vrf > SOC_VRF_MAX(unit)) || 
        (vrf < BCM_L3_VRF_DEFAULT)) {
        return BCM_E_PARAM;
    }

    L3_LOCK(unit);

    /* Make sure (vrf, ip6_addr) entry is already in L3 host table */
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_flags = BCM_L3_IP6;
    l3cfg.l3c_vrf = vrf;
    sal_memcpy(l3cfg.l3c_ip6, ip6_addr, BCM_IP6_ADDRLEN);
    rv = mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg);
    if (rv == BCM_E_NOT_FOUND) {
        /* The (vrf, ip6_addr) entry needs to be added to L3 table
         * before configuring it for endpoint queuing.
         */
        L3_UNLOCK(unit);
        return BCM_E_CONFIG;
    } else if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    } else {
        /* Make sure the L3 ENTRY is in extender view */
        if (!BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg.l3c_intf, 
                                              BCM_XGS3_L3_INVALID_INDEX)) {
            L3_UNLOCK(unit);
            return BCM_E_CONFIG;
        }
    }

    if (l3cfg.l3c_eh_q_tag_type != 0) {
        /* A queue tag has already been configured for this L3 entry. */
        L3_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    /* Replace (vrf, ip_addr) entry's endpoint_id */
    l3cfg.l3c_flags |= BCM_L3_REPLACE;
    l3cfg.l3c_eh_q_tag_type = 2;
    l3cfg.l3c_eh_q_tag = endpoint_id;
    rv = mbcm_driver[unit]->mbcm_l3_ip6_replace(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    }

    L3_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_ip6_destroy
 * Purpose:
 *      Destroy an IPv6 endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      vrf         - (IN) Virtual router instance
 *      ip6_addr    - (IN) IPv6 address
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_ip6_destroy(
    int unit, 
    bcm_vrf_t vrf,
    bcm_ip6_t ip6_addr)
{
    int rv;
    _bcm_l3_cfg_t l3cfg;

    if ((vrf > SOC_VRF_MAX(unit)) || 
        (vrf < BCM_L3_VRF_DEFAULT)) {
        return BCM_E_PARAM;
    }

    L3_LOCK(unit);

    /* Get (vrf, ip6_addr) entry */
    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
    l3cfg.l3c_flags = BCM_L3_IP6;
    l3cfg.l3c_vrf = vrf;
    sal_memcpy(l3cfg.l3c_ip6, ip6_addr, BCM_IP6_ADDRLEN);
    rv = mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    } else {
        /* Make sure the L3 ENTRY is in extender view */
        if (!BCM_TD2_L3_USE_EMBEDDED_NEXT_HOP(unit, l3cfg.l3c_intf, 
                                              BCM_XGS3_L3_INVALID_INDEX)) {
            L3_UNLOCK(unit);
            return BCM_E_CONFIG;
        }
    }

    /* Delete (vrf, ip_addr) entry's endpoint_id */
    l3cfg.l3c_flags |= BCM_L3_REPLACE;
    l3cfg.l3c_eh_q_tag_type = 0;
    l3cfg.l3c_eh_q_tag = 0;
    rv = mbcm_driver[unit]->mbcm_l3_ip6_replace(unit, &l3cfg);
    if (BCM_FAILURE(rv)) {
        L3_UNLOCK(unit);
        return rv;
    }

    L3_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      _bcm_td2_cosq_endpoint_L3_egress_create
 * Purpose:
 *      Create a L3 egress endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      endpoint_id - (IN) Endpoint ID
 *      egress_if   - (IN) egress object ID 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_l3_egress_create(
    int unit, 
    int endpoint_id,
    bcm_if_t egress_if)
{    
    int nh_index;
    ing_l3_next_hop_entry_t nh_entry;
    int eh_tag_type;
    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, egress_if)) {
        return BCM_E_PARAM;
    }
    nh_index = egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
    /* Set endpoint ID in next hop entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &nh_entry));
    eh_tag_type = soc_ING_L3_NEXT_HOPm_field32_get(unit, &nh_entry,
            EH_TAG_TYPEf);
    if (eh_tag_type != 0) {
        /* A queue tag has already been configured for this next hop entry. */
        return BCM_E_EXISTS;
    }
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_TAG_TYPEf, 2);
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_QUEUE_TAGf, endpoint_id);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index, &nh_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_gport_destroy
 * Purpose:
 *      Destroy a GPORT endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      egress_if   - (IN) Egress object.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_l3_egress_destroy(
    int unit, 
    bcm_if_t egress_if)
{    
    int nh_index;
    ing_l3_next_hop_entry_t nh_entry;
    if (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, egress_if)) {
        return BCM_E_PARAM;
    }
    nh_index = egress_if - BCM_XGS3_EGRESS_IDX_MIN(unit);
    /* Set endpoint ID in next hop entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &nh_entry));
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_TAG_TYPEf, 0);
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_QUEUE_TAGf, 0);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index, &nh_entry));
    return BCM_E_NONE;
}

#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_gport_create
 * Purpose:
 *      Create a GPORT endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      endpoint_id - (IN) Endpoint ID
 *      gport       - (IN) GPORT ID 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_gport_create(
    int unit, 
    int endpoint_id,
    bcm_gport_t gport)
{
    int vp;
    ing_dvp_table_entry_t dvp_entry;
    int nh_index;
    ing_l3_next_hop_entry_t nh_entry;
    int eh_tag_type;

    /* Get virtual port value */
    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else {
       return BCM_E_PARAM;
    } 

    /* Get VP's next hop index */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
            NEXT_HOP_INDEXf);

    /* Set endpoint ID in next hop entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &nh_entry));
    eh_tag_type = soc_ING_L3_NEXT_HOPm_field32_get(unit, &nh_entry,
            EH_TAG_TYPEf);
    if (eh_tag_type != 0) {
        /* A queue tag has already been configured for this next hop entry. */
        return BCM_E_EXISTS;
    }
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_TAG_TYPEf, 2);
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_QUEUE_TAGf, endpoint_id);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index, &nh_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2_cosq_endpoint_gport_destroy
 * Purpose:
 *      Destroy a GPORT endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      gport       - (IN) GPORT ID 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_td2_cosq_endpoint_gport_destroy(
    int unit, 
    bcm_gport_t gport)
{
    int vp;
    ing_dvp_table_entry_t dvp_entry;
    int nh_index;
    ing_l3_next_hop_entry_t nh_entry;

    /* Get virtual port value */
    if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = BCM_GPORT_VLAN_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = BCM_GPORT_NIV_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeNiv)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = BCM_GPORT_MPLS_PORT_ID_GET(gport);
#if defined(INCLUDE_L3)
        if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeMpls)) {
            /* VP is not configured yet */
            return BCM_E_CONFIG;
        }
#endif
    } else {
       return BCM_E_PARAM;
    } 

    /* Get VP's next hop index */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));
    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
            NEXT_HOP_INDEXf);

    /* Delete endpoint ID in next hop entry */
    BCM_IF_ERROR_RETURN
        (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &nh_entry));
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_TAG_TYPEf, 0);
    soc_ING_L3_NEXT_HOPm_field32_set(unit, &nh_entry, EH_QUEUE_TAGf, 0);
    BCM_IF_ERROR_RETURN
        (WRITE_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ALL, nh_index, &nh_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_create
 * Purpose:
 *      Create an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier    - (IN) Classifier attributes
 *      classifier_id - (IN/OUT) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_create(
    int unit, 
    bcm_cosq_classifier_t *classifier, 
    int *classifier_id)
{
    int endpoint_id = 0;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    if (classifier->flags & BCM_COSQ_CLASSIFIER_WITH_ID) {
        if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(*classifier_id)) {
            return BCM_E_PARAM;
        }
        endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(*classifier_id);
        if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
            return BCM_E_PARAM;
        }
        if (_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
            return BCM_E_EXISTS;
        }
    } else {
        /* Get a free endpoint ID */
        for (i = 0; i < _BCM_TD2_NUM_ENDPOINT(unit); i++) {
            if (!_BCM_TD2_ENDPOINT_IS_USED(unit, i)) {
                endpoint_id = i;
                break;
            }
        }
        if (i == _BCM_TD2_NUM_ENDPOINT(unit)) {
            /* No available endpoint ID */
            return BCM_E_RESOURCE;
        }
    }
    _BCM_TD2_ENDPOINT(unit, endpoint_id) = sal_alloc(
            sizeof(_bcm_td2_endpoint_t), "Endpoint Info");
    if (NULL == _BCM_TD2_ENDPOINT(unit, endpoint_id)) {
        return BCM_E_MEMORY;
    }
    sal_memset(_BCM_TD2_ENDPOINT(unit, endpoint_id), 0,
            sizeof(_bcm_td2_endpoint_t));

    if (classifier->flags & BCM_COSQ_CLASSIFIER_L2) {
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_L2;
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->vlan = classifier->vlan;
        sal_memcpy(_BCM_TD2_ENDPOINT(unit, endpoint_id)->mac, classifier->mac,
                sizeof(bcm_mac_t));
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_l2_create(unit,
                    endpoint_id, classifier->vlan, classifier->mac));
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_L3) {
#if defined(INCLUDE_L3)
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_L3;
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->vrf = classifier->vrf;
        if (classifier->flags & BCM_COSQ_CLASSIFIER_IP6) {
            _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags |= BCM_COSQ_CLASSIFIER_IP6;
            sal_memcpy(_BCM_TD2_ENDPOINT(unit, endpoint_id)->ip6_addr,
                    classifier->ip6_addr, BCM_IP6_ADDRLEN);
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_ip6_create(unit,
                        endpoint_id, classifier->vrf, classifier->ip6_addr));
        } else {
            _BCM_TD2_ENDPOINT(unit, endpoint_id)->ip_addr = classifier->ip_addr;
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_ip4_create(unit,
                        endpoint_id, classifier->vrf, classifier->ip_addr));
        }
#endif /* INCLUDE_L3 */
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_GPORT) {
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_GPORT;
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->gport = classifier->gport;
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_gport_create(unit,
                    endpoint_id, classifier->gport));
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_L3_EGRESS) {
#if defined(INCLUDE_L3)
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_L3_EGRESS;
        _BCM_TD2_ENDPOINT(unit, endpoint_id)->egress_if = classifier->egress_if;
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_l3_egress_create(unit,
                    endpoint_id, classifier->egress_if));
#endif /* INCLUDE_L3 */
    } else {
        return BCM_E_PARAM;
    }

    _BCM_COSQ_CLASSIFIER_ENDPOINT_SET(*classifier_id, endpoint_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_destroy
 * Purpose:
 *      Destroy an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_destroy(
    int unit, 
    int classifier_id)
{
    int endpoint_id;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_NOT_FOUND;
    }

    if (_BCM_TD2_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_L2) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_l2_destroy(unit,
                    _BCM_TD2_ENDPOINT(unit, endpoint_id)->vlan,
                    _BCM_TD2_ENDPOINT(unit, endpoint_id)->mac));
    } else if (_BCM_TD2_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_L3) {
#if defined(INCLUDE_L3)
        if (_BCM_TD2_ENDPOINT(unit, endpoint_id)->flags &
                BCM_COSQ_CLASSIFIER_IP6) {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_ip6_destroy(unit,
                        _BCM_TD2_ENDPOINT(unit, endpoint_id)->vrf,
                        _BCM_TD2_ENDPOINT(unit, endpoint_id)->ip6_addr));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_ip4_destroy(unit,
                        _BCM_TD2_ENDPOINT(unit, endpoint_id)->vrf,
                        _BCM_TD2_ENDPOINT(unit, endpoint_id)->ip_addr));
        }
#endif
    } else if (_BCM_TD2_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_GPORT) {
        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_endpoint_gport_destroy(unit,
                    _BCM_TD2_ENDPOINT(unit, endpoint_id)->gport));
    } else if (_BCM_TD2_ENDPOINT(unit, endpoint_id)->flags &
               BCM_COSQ_CLASSIFIER_L3_EGRESS) {
#if defined(INCLUDE_L3)
        BCM_IF_ERROR_RETURN(
            _bcm_td2_cosq_endpoint_l3_egress_destroy(
                unit, _BCM_TD2_ENDPOINT(unit, endpoint_id)->egress_if));
#endif
    } else {
        return BCM_E_INTERNAL;
    }

    sal_free(_BCM_TD2_ENDPOINT(unit, endpoint_id));
    _BCM_TD2_ENDPOINT(unit, endpoint_id) = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_get
 * Purpose:
 *      Get info about an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 *      classifier    - (OUT) Classifier info
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_get(
    int unit, 
    int classifier_id,
    bcm_cosq_classifier_t *classifier)
{
    int endpoint_id;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_NOT_FOUND;
    }

    classifier->flags = _BCM_TD2_ENDPOINT(unit, endpoint_id)->flags;
    classifier->vlan = _BCM_TD2_ENDPOINT(unit, endpoint_id)->vlan;
    sal_memcpy(classifier->mac, _BCM_TD2_ENDPOINT(unit, endpoint_id)->mac,
            sizeof(bcm_mac_t));
    classifier->vrf = _BCM_TD2_ENDPOINT(unit, endpoint_id)->vrf;
    classifier->ip_addr = _BCM_TD2_ENDPOINT(unit, endpoint_id)->ip_addr;
    sal_memcpy(classifier->ip6_addr,
            _BCM_TD2_ENDPOINT(unit, endpoint_id)->ip6_addr, BCM_IP6_ADDRLEN);
    classifier->gport = _BCM_TD2_ENDPOINT(unit, endpoint_id)->gport;
    classifier->egress_if = _BCM_TD2_ENDPOINT(unit, endpoint_id)->egress_if;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_map_set
 * Purpose:
 *      Set the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) local port ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (IN) Base queue ID
 *      array_count    - (IN) Number of elements in priority_array and
 *                            cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (IN) Array of COS queues
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_map_set(
    int unit, 
    bcm_port_t port, 
    int classifier_id, 
    bcm_gport_t queue_group, 
    int array_count, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array)
{
    int rv = BCM_E_NONE;
    int endpoint_id;
    int qid;
    int num_entries_per_profile;
    int alloc_size;
    endpoint_cos_map_entry_t *entry_array = NULL;
    void *entries[1];
    endpoint_queue_map_entry_t qmap_key, qmap_data, qmap_entry;
    int qmap_index;
    int old_profile_index, new_profile_index;
    int endpoint_cos_map_prof_idx;
    int old_qid;
    int i;
    int priority;
    uint64 rval64;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(queue_group)) {
        int pipe;
        _bcm_td2_cosq_node_t *node;

        BCM_IF_ERROR_RETURN(_bcm_td2_cosq_node_get(unit, queue_group, 0, NULL,
                                                   NULL, NULL, &node));
        if (port != node->local_port) {
            return BCM_E_PARAM;
        }
        pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
        qid  = (pipe == _TD2_XPIPE)? node->hw_index: node->hw_index + 2048;
    } else {
        return BCM_E_PARAM;
    }

    /* Allocate an Endpoint CoS Map profile */
    num_entries_per_profile = _TD2_EPCM_ENTRIES_PER_SET;
    alloc_size = sizeof(endpoint_cos_map_entry_t) * num_entries_per_profile;
    entry_array = sal_alloc(alloc_size, "Endpoint CoS Map Profile");
    if (NULL == entry_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_array, 0, alloc_size);

    /* Search Endpoint Queue Map table */
    sal_memcpy(&qmap_key, soc_mem_entry_null(unit,
                ENDPOINT_QUEUE_MAPm), sizeof(endpoint_queue_map_entry_t));
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, KEY_TYPEf, 0);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, DEST_PORTf, port);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, EH_QUEUE_TAGf,
            endpoint_id);
    rv = soc_mem_search(unit, ENDPOINT_QUEUE_MAPm, MEM_BLOCK_ANY,
            &qmap_index, &qmap_key, &qmap_data, 0);
    if (rv == SOC_E_NOT_FOUND) {
        old_profile_index = -1;
    } else if (rv == SOC_E_NONE) {
        /* Existing entry found. Compare base queue ID and extract
         * old Endpoint CoS Map profile.
         */
        old_qid = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit,
                &qmap_data, ENDPOINT_QUEUE_BASEf);
        if (old_qid != qid) {
            /* The classifier is already mapped to another queue group */
            sal_free(entry_array);
            return BCM_E_EXISTS;
        }
        endpoint_cos_map_prof_idx = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit,
                &qmap_data, ENDPOINT_COS_MAP_PROFILE_INDEXf);
        entries[0] = entry_array;
        /* 
         * The index of ENDPOINT_COS_MAP = 
         * ENDPOINT_COS_MAP_PROFILE_INDEX * 16 + INT_PRI
         */
        old_profile_index = endpoint_cos_map_prof_idx * num_entries_per_profile;
        rv = soc_profile_mem_get(unit, _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit),
                old_profile_index, num_entries_per_profile, entries);
        if (SOC_FAILURE(rv)) {
            sal_free(entry_array);
            return rv;
        }
    } else {
        sal_free(entry_array);
        return rv;
    }

    /* Construct or modify Endpoint CoS Map profile and add it */
    for (i = 0; i < array_count; i++) {
        priority = priority_array[i];
        if (priority < 0 || priority >= 16) {
            sal_free(entry_array);
            return BCM_E_PARAM;
        }
        soc_ENDPOINT_COS_MAPm_field32_set(unit, &entry_array[priority],
                ENDPOINT_COS_OFFSETf, cosq_array[i]);
    }
    entries[0] = entry_array;
    rv = soc_profile_mem_add(unit, _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit),
            entries, num_entries_per_profile, (uint32 *)&new_profile_index);
    sal_free(entry_array);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Insert or replace Endpoint Queue Map entry */
    sal_memcpy(&qmap_entry, &qmap_key, sizeof(endpoint_queue_map_entry_t));
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_entry, VALIDf, 1);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_entry,
            ENDPOINT_QUEUE_BASEf, qid);
    endpoint_cos_map_prof_idx = new_profile_index / num_entries_per_profile;
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_entry,
            ENDPOINT_COS_MAP_PROFILE_INDEXf, endpoint_cos_map_prof_idx);
    rv = soc_mem_insert(unit, ENDPOINT_QUEUE_MAPm, MEM_BLOCK_ALL, &qmap_entry);
    if (SOC_FAILURE(rv) && rv != SOC_E_EXISTS) {
        return rv;
    }

    /* Delete old Endpoint CoS Map profile */
    if (old_profile_index != -1) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit,
                                    _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit),
                                    old_profile_index));
    }

    /* Set port's queuing mode to be endpoint queuing */
    SOC_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, port, &rval64));
    if (4 != soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval64,
                QUEUE_MODEf)) {
        soc_reg64_field32_set(unit, ING_COS_MODE_64r, &rval64, QUEUE_MODEf, 4);
        SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODE_64r(unit, port, rval64));
    }

    return rv;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_map_get
 * Purpose:
 *      Get the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Local port ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (OUT) Queue group
 *      array_max      - (IN) Size of priority_array and cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (OUT) Array of COS queues
 *      array_count    - (OUT) Size of cosq_array
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_map_get(
    int unit, 
    bcm_port_t port, 
    int classifier_id, 
    bcm_gport_t *queue_group, 
    int array_max, 
    bcm_cos_t *priority_array, 
    bcm_cos_queue_t *cosq_array, 
    int *array_count)
{
    int rv = BCM_E_NONE;
    int endpoint_id;
    endpoint_queue_map_entry_t qmap_key, qmap_data;
    int qmap_index;
    int qid;
    int num_entries_per_profile;
    int alloc_size;
    endpoint_cos_map_entry_t *entry_array = NULL;
    void *entries[1];
    int profile_index;
    int endpoint_cos_map_prof_idx;
    int i;
    int priority;
    int pipe, hw_index, qmin, qmax;
    _bcm_td2_pipe_resources_t *res = NULL;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_PARAM;
    }

    /* Search Endpoint Queue Map table */
    sal_memcpy(&qmap_key, soc_mem_entry_null(unit,
                ENDPOINT_QUEUE_MAPm), sizeof(endpoint_queue_map_entry_t));
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, KEY_TYPEf, 0);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, DEST_PORTf, port);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, EH_QUEUE_TAGf,
            endpoint_id);
    SOC_IF_ERROR_RETURN(soc_mem_search(unit, ENDPOINT_QUEUE_MAPm, MEM_BLOCK_ANY,
                &qmap_index, &qmap_key, &qmap_data, 0));

    /* Get queue group */
    qid = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit, &qmap_data,
            ENDPOINT_QUEUE_BASEf);
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    res = _BCM_TD2_PRESOURCES(_bcm_td2_mmu_info[unit], pipe);
    hw_index = (pipe == _TD2_XPIPE)? qid: qid - 2048;

    qmin = res->num_base_queues + (pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE);
    qmax = (pipe + 1) * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE;

    for (i = qmin; i < qmax; i++) {
        if ((res->p_queue_node[i].local_port == port) &&
            (res->p_queue_node[i].hw_index == hw_index)) {
            *queue_group = res->p_queue_node[i].gport;
            break;
        }
    }

    if (i == qmax) {
        return BCM_E_NOT_FOUND;
    }

    /* Get Endpoint CoS Map profile */
    num_entries_per_profile = _TD2_EPCM_ENTRIES_PER_SET;
    alloc_size = sizeof(endpoint_cos_map_entry_t) * num_entries_per_profile;
    entry_array = sal_alloc(alloc_size, "Endpoint CoS Map Profile");
    if (NULL == entry_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_array, 0, alloc_size);
    entries[0] = entry_array;
    endpoint_cos_map_prof_idx = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit, &qmap_data,
            ENDPOINT_COS_MAP_PROFILE_INDEXf);
    /* 
     * The index of ENDPOINT_COS_MAP = 
     * ENDPOINT_COS_MAP_PROFILE_INDEX * 16 + INT_PRI
     */
    profile_index = endpoint_cos_map_prof_idx * num_entries_per_profile;
    rv = soc_profile_mem_get(unit, _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit),
            profile_index, num_entries_per_profile, entries);
    if (SOC_FAILURE(rv)) {
        sal_free(entry_array);
        return rv;
    }

    /* Get internal priority to CoS mapping */
    if (array_max == 0) {
        if (NULL != array_count) {
            *array_count = num_entries_per_profile;
        }
    } else {
        *array_count = 0;
        for (i = 0; i < array_max; i++) {
            priority = priority_array[i];
            if (priority < 0 || priority >= 16) {
                sal_free(entry_array);
                return BCM_E_PARAM;
            }
            cosq_array[i] = soc_ENDPOINT_COS_MAPm_field32_get(unit,
                    &entry_array[priority], ENDPOINT_COS_OFFSETf);
            (*array_count)++;
        }
    }

    sal_free(entry_array);
    return rv;
}

/*
 * Function:
 *      bcm_td2_cosq_endpoint_map_clear
 * Purpose:
 *      Clear the mapping from port, classifier, and internal priorities
 *      to COS queues in a queue group.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) Local port ID
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_endpoint_map_clear(
    int unit, 
    bcm_gport_t port, 
    int classifier_id)
{
    int endpoint_id;
    endpoint_queue_map_entry_t qmap_key, qmap_data;
    int profile_index;
    int endpoint_cos_map_prof_idx;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_TD2_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_TD2_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_PARAM;
    }

    /* Delete (port, endpoint_id) entry from Endpoint Queue Map table */
    sal_memcpy(&qmap_key, soc_mem_entry_null(unit,
                ENDPOINT_QUEUE_MAPm), sizeof(endpoint_queue_map_entry_t));
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, KEY_TYPEf, 0);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, DEST_PORTf, port);
    soc_ENDPOINT_QUEUE_MAPm_field32_set(unit, &qmap_key, EH_QUEUE_TAGf,
            endpoint_id);
    SOC_IF_ERROR_RETURN(soc_mem_delete_return_old(unit, ENDPOINT_QUEUE_MAPm,
                MEM_BLOCK_ALL, &qmap_key, &qmap_data));

    /* Delete priority to CoS mapping profile */
    endpoint_cos_map_prof_idx = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit, &qmap_data,
            ENDPOINT_COS_MAP_PROFILE_INDEXf);
    /* 
     * The index of ENDPOINT_COS_MAP = 
     * ENDPOINT_COS_MAP_PROFILE_INDEX * 16 + INT_PRI
     */
    profile_index = endpoint_cos_map_prof_idx * 16;
    SOC_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                _BCM_TD2_ENDPOINT_COS_MAP_PROFILE(unit), profile_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_field_classifier_get
 * Purpose:
 *      Get classifier type information.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 *      classifier    - (OUT) Classifier info
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_field_classifier_get(
    int unit,    
    int classifier_id,
    bcm_cosq_classifier_t *classifier)
{
    if (classifier == NULL) {
        return BCM_E_PARAM;
    }
    sal_memset(classifier, 0, sizeof(bcm_cosq_classifier_t));

    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        classifier->flags |= BCM_COSQ_CLASSIFIER_FIELD;
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      bcm_td2_cosq_field_classifier_id_create
 * Purpose:
 *      Create a cosq classifier.
 * Parameters: 
 *      unit          - (IN) Unit number.
 *      classifier    - (IN) Classifier attributes
 *      classifier_id - (OUT) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */     
int
bcm_td2_cosq_field_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    int rv;
    int ref_count = 0;
    int ent_per_set = _TD2_NUM_INTERNAL_PRI;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < SOC_MEM_SIZE(unit, IFP_COS_MAPm); i += ent_per_set) {
        rv = soc_profile_mem_ref_count_get(
                unit, _bcm_td2_ifp_cos_map_profile[unit], i, &ref_count);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
        if (0 == ref_count) {
            break;
        }
    }

    if (i >= SOC_MEM_SIZE(unit, IFP_COS_MAPm) && ref_count != 0) {
        *classifier_id = 0;
        return BCM_E_RESOURCE;
    }

    _BCM_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, (i / ent_per_set));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2_cosq_field_classifier_map_set
 * Purpose:
 *      Set internal priority to ingress field processor CoS queue
 *      override mapping.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      classifier_id  - (IN) Classifier ID
 *      count          - (IN) Number of elements in priority_array and
 *                            cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (IN) Array of COS queues
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_field_classifier_map_set(
    int unit,
    int classifier_id,
    int count,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array)
{
    int rv;
    int i;
    int max_entries = _TD2_NUM_INTERNAL_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_TD2_NUM_INTERNAL_PRI];
    int field_width;
    int ref_count = 0;

    /* Input parameter check. */
    if (!_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        return BCM_E_PARAM;
    }

    if (NULL == priority_array || NULL == cosq_array) {
        return BCM_E_PARAM;
    }

    if (count > max_entries) {
        return BCM_E_PARAM;
    }

    field_width = soc_mem_field_length(unit, IFP_COS_MAPm, IFP_COSf);
    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * max_entries);
    entries[0] = ent_buf;
    
    for (i = 0; i < count; i++) {
        if (priority_array[i] < max_entries) {
            if (cosq_array[i] >= (1 << field_width)) {
                return BCM_E_PARAM;
            }
            soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                IFP_COSf, cosq_array[i]);
        }
    }
    
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_ref_count_get(unit, _bcm_td2_ifp_cos_map_profile[unit],
                                      index * max_entries, &ref_count));

    if (ref_count == 0) {
        rv = soc_profile_mem_add(unit, _bcm_td2_ifp_cos_map_profile[unit],
                                 entries, max_entries, &index);
    } else {
        rv = soc_profile_mem_set(unit, _bcm_td2_ifp_cos_map_profile[unit],
                                 entries, index * max_entries);
        if (rv == SOC_E_NONE) {
            /* decrease the profile ref_count increased by mem_set */
            rv = soc_profile_mem_delete(unit,
                                        _bcm_td2_ifp_cos_map_profile[unit],
                                        index * max_entries);
        }
    }
    return rv;
}


/*
 * Function:
 *      bcm_td2_cosq_field_classifier_map_get
 * Purpose:
 *      Get internal priority to ingress field processor CoS queue
 *      override mapping information.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      classifier_id  - (IN) Classifier ID
 *      array_max      - (IN) Size of priority_array and cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (OUT) Array of COS queues
 *      array_count    - (OUT) Size of cosq_array
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_field_classifier_map_get(
    int unit,
    int classifier_id,
    int array_max,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array,
    int *array_count)
{
    int rv;
    int i;
    int ent_per_set = _TD2_NUM_INTERNAL_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_TD2_NUM_INTERNAL_PRI];

    /* Input parameter check. */
    if (NULL == priority_array || NULL == cosq_array || NULL == array_count) {
        return BCM_E_PARAM;
    }

    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * ent_per_set);
    entries[0] = ent_buf;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Read out entries from profile table into allocated buffer. */
    rv = soc_profile_mem_get(unit, _bcm_td2_ifp_cos_map_profile[unit],
                             index * ent_per_set, ent_per_set, entries);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    *array_count = array_max > ent_per_set ? ent_per_set : array_max;

    /* Copy values into API OUT parameters. */
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i] >= _TD2_NUM_INTERNAL_PRI) {
            return BCM_E_PARAM;
        }
        cosq_array[i] = soc_mem_field32_get(unit, IFP_COS_MAPm,
                                            &ent_buf[priority_array[i]],
                                            IFP_COSf);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2_cosq_field_classifier_map_clear
 * Purpose:
 *      Delete an internal priority to ingress field processor CoS queue
 *      override mapping profile set.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    int ent_per_set = _TD2_NUM_INTERNAL_PRI;
    uint32 index;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Delete the profile entries set. */
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_td2_ifp_cos_map_profile[unit],
                                index * ent_per_set));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2_cosq_field_classifier_id_destroy
 * Purpose:
 *      Free resource associated with this field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_field_classifier_id_destroy(int unit, int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_cosq_cpu_cosq_enable_set
 * Purpose:
 *      To enable/disable Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (IN) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_cpu_cosq_enable_set(
    int unit, 
    bcm_cos_queue_t cosq, 
    int enable)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int i, index, enable_status;
    _bcm_td2_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    soc_mem_t thdm_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIG_0m,
        MMU_THDM_MCQE_QUEUE_CONFIG_0m
    };

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_td2_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }
    if (enable) {
        enable = 1;
    }
    BCM_IF_ERROR_RETURN(bcm_td2_cosq_cpu_cosq_enable_get(unit, cosq,
                                                         &enable_status));
    if (enable == enable_status) {
        return BCM_E_NONE;
    }

    /* CPU MC cosq's range from 520 - 568. 0-519 are MC's of Regular ports */
    index = 520 + cosq;

    for (i = 0; i < 2; i++) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, thdm_mem[i], MEM_BLOCK_ALL, index, &entry));

        if (enable) {
            soc_mem_field32_set(unit, thdm_mem[i], &entry,
                                Q_MIN_LIMITf, cpu_cosq->q_min_limit[i]);
            soc_mem_field32_set(unit, thdm_mem[i], &entry,
                                Q_SHARED_LIMITf, cpu_cosq->q_shared_limit[i]);
        }
        else {
            cpu_cosq->q_min_limit[i] = soc_mem_field32_get(unit,
                                                           thdm_mem[i],
                                                           &entry,
                                                           Q_MIN_LIMITf);
            cpu_cosq->q_shared_limit[i] = soc_mem_field32_get(unit,
                                                              thdm_mem[i],
                                                              &entry,
                                                              Q_SHARED_LIMITf);
            cpu_cosq->q_limit_dynamic[i] = soc_mem_field32_get(unit,
                                                               thdm_mem[i],
                                                               &entry,
                                                               Q_LIMIT_DYNAMICf);
            cpu_cosq->q_limit_enable[i] = soc_mem_field32_get(unit,
                                                              thdm_mem[i],
                                                              &entry,
                                                              Q_LIMIT_ENABLEf);
            soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_MIN_LIMITf, 0);
            soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_SHARED_LIMITf, 0);
        }
        soc_mem_field32_set(unit, thdm_mem[i], &entry,
                            Q_LIMIT_DYNAMICf, enable ? cpu_cosq->q_limit_dynamic[i] : 0);
        soc_mem_field32_set(unit, thdm_mem[i], &entry,
                            Q_LIMIT_ENABLEf, enable ? cpu_cosq->q_limit_enable[i] : 1);
        cpu_cosq->enable = enable;
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, thdm_mem[i], MEM_BLOCK_ALL, index, &entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_cosq_cpu_cosq_enable_get
 * Purpose:
 *      To get enable/disable status on Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (OUT) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_cpu_cosq_enable_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *enable)
{
    _bcm_td2_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    int index, hw_enable;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t min_limit, shared_limit, limit_enable, dynamic_enable;
    soc_mem_t thdm_mem = MMU_THDM_DB_QUEUE_CONFIG_0m;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_td2_cosq_cpu_cosq_config[unit][cosq];
    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }

    /* CPU MC cosq's range from 520 - 568. 0-519 are MC's of Regular ports */
    index = 520 + cosq;
    hw_enable = 1;
    /* Sync up software record with hardware status. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, thdm_mem, MEM_BLOCK_ALL,
                                     index, &entry));
    min_limit = soc_mem_field32_get(unit, thdm_mem, &entry,
                                    Q_MIN_LIMITf);
    shared_limit = soc_mem_field32_get(unit, thdm_mem, &entry,
                                    Q_SHARED_LIMITf);
    limit_enable = soc_mem_field32_get(unit, thdm_mem, &entry,
                                    Q_LIMIT_ENABLEf);
    dynamic_enable = soc_mem_field32_get(unit, thdm_mem, &entry,
                                    Q_LIMIT_DYNAMICf);
    if (limit_enable && (dynamic_enable == 0) &&
        (min_limit == 0) && (shared_limit == 0)) {
        hw_enable = 0;
    }
    cpu_cosq->enable = hw_enable;

    *enable = cpu_cosq->enable;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_service_classifier_id_create
 * Purpose:
 *      Create an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier    - (IN) Classifier attributes
 *      classifier_id - (OUT) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    _BCM_COSQ_CLASSIFIER_SERVICE_SET(*classifier_id, classifier->vlan);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_service_classifier_id_destroy
 * Purpose:
 *      free resource associated with this service classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_classifier_id_destroy(
    int unit,
    int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_service_classifier_get
 * Purpose:
 *      Get info about an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 *      classifier    - (OUT) Classifier info
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_classifier_get(
    int unit,
    int classifier_id,
    bcm_cosq_classifier_t *classifier)
{
    int val = 0;

    sal_memset(classifier, 0, sizeof(bcm_cosq_classifier_t));

    val = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);

    if (val > BCM_VLAN_MAX) {
        /* VFI classifier */
        classifier->flags = BCM_COSQ_CLASSIFIER_VFI;
        classifier->vfi_index = val - BCM_VLAN_MAX - 1;
    } else {
        classifier->flags = BCM_COSQ_CLASSIFIER_VLAN;
        classifier->vlan = val;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2_cosq_service_map_set
 * Purpose:
 *      Set the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) local port ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (IN) Base queue ID
 *      array_count    - (IN) Number of elements in priority_array and
 *                            cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (IN) Array of COS queues
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_map_set(
    int unit,
    bcm_port_t port,
    int classifier_id,
    bcm_gport_t queue_group,
    int array_count,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array)
{
    service_queue_map_entry_t  sqm;
    service_port_map_entry_t   *spm = NULL;
    service_cos_map_entry_t    *scm = NULL;
    void                       *spm_entries[1];
    void                       *scm_entries[1];
    int                        rv = BCM_E_NONE;
    int                        i, valid_cosq, alloc_size, exists = 0;
    int                        local_port, vid, qid, cos_offset = 0;
    int                        port_offset, port_offset_old;
    int                        qptr_old, qptr_new;
    int                        pipe;
    uint32                     scm_idx, scm_idx_old = 0, spm_idx, spm_idx_old = 0;
    _bcm_td2_cosq_node_t       *node,*cur_node;
    soc_profile_mem_t          *spm_profile_mem, *scm_profile_mem;
    /*
     * The port_offset= 0 is treated as the invalid offset so that
     * the ports without being service map set can be recognized easily by software.
     * Accordingly, The valid port_offset needs to be at least 1 for a given port.
     * the queue base to be written into SERVICE_QUEUE_PTRf will
     * be (the actual base - 1).
     */
   #define VALID_PORT_OFFSET_MIN          1

    if (!_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
        return BCM_E_PARAM;
    }

    /* index above 4K is for VFI */
    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);
    if (vid < 0 || vid >= SOC_MEM_SIZE(unit, SERVICE_QUEUE_MAPm)) {
        return BCM_E_PARAM;
    }

    if (array_count > _TD2_SCM_ENTRIES_PER_SET) {
        return BCM_E_PARAM;
    }
    scm_profile_mem = _bcm_td2_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_td2_service_port_map_profile[unit];

    BCM_IF_ERROR_RETURN(
        _bcm_td2_cosq_node_get(unit, queue_group, 0, NULL,
                               &local_port, &qid, &node));
    if (node->attached_to_input < 0) {
        return BCM_E_PARAM;
    }

    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    qid  = (pipe == _TD2_XPIPE)? node->hw_index: node->hw_index + 2048;

    for (i = 0; i < array_count; i++) {
        valid_cosq= 0;
        for (cur_node = node->parent->child;
             cur_node != NULL;
             cur_node = cur_node->sibling) {
            if (cosq_array[i] == cur_node->attached_to_input) {
                valid_cosq = 1;
                break;
            }
        }
        if(valid_cosq == 0){
            return BCM_E_PARAM;
        }
    }

    sal_memset(&sqm, 0, sizeof(service_queue_map_entry_t));
    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ANY, (int)vid, &sqm));

    if (soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf)) {
        exists = 1;
    } else {
        exists = 0;
    }

    alloc_size = sizeof(service_port_map_entry_t) * _TD2_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    alloc_size = sizeof(service_cos_map_entry_t) * _TD2_SCM_ENTRIES_PER_SET;
    scm = sal_alloc(alloc_size, "SERVICE_COS_MAP temp Mem");
    if (scm == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(scm, 0, alloc_size);
    scm_entries[0] = scm;

    /*  1. program the SERVICE_COS_MAPm. */
    if (!exists) {
        for (i = 0; i < array_count; i++) {
            if (priority_array[i] < _TD2_SCM_ENTRIES_PER_SET) {
                soc_mem_field32_set(unit, SERVICE_COS_MAPm,
                                    &scm[priority_array[i]],
                                    SERVICE_COS_OFFSETf, cosq_array[i]);
            }
        }

        rv = soc_profile_mem_add(unit, scm_profile_mem, scm_entries,
                                 _TD2_SCM_ENTRIES_PER_SET, &scm_idx);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }
    } else {
        /*
         * only one service_cos_map profile is allowed per vlan/vfi.
         * If the subsequent profile doesn't match the existing one,
         * Then update the old profile, the new map may be added by other port.
         */
        scm_idx_old = _TD2_SCM_ENTRIES_PER_SET *
                  soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                      SERVICE_COS_PROFILE_INDEXf);
        rv = soc_profile_mem_get(unit, scm_profile_mem, scm_idx_old,
                                 _TD2_SCM_ENTRIES_PER_SET, scm_entries);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }

        for (i = 0; i < array_count; i++) {
            cos_offset = soc_mem_field32_get(unit, SERVICE_COS_MAPm,
                                             &scm[priority_array[i]],
                                             SERVICE_COS_OFFSETf);
            if (cosq_array[i] != cos_offset) {
                if (priority_array[i] < _TD2_SCM_ENTRIES_PER_SET) {
                    soc_mem_field32_set(unit, SERVICE_COS_MAPm,
                                        &scm[priority_array[i]],
                                        SERVICE_COS_OFFSETf, cosq_array[i]);
                }
            }
        }

        rv = soc_profile_mem_add(unit, scm_profile_mem, scm_entries,
                         _TD2_SCM_ENTRIES_PER_SET, &scm_idx);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }

        if (scm_idx_old != scm_idx) {
            rv = soc_profile_mem_delete(unit, scm_profile_mem, scm_idx_old);
            if (rv != SOC_E_NONE) {
                goto cleanup;
            }
        }

    }

    /*  2. program the SERVICE_PORT_MAPm. */
    if (!exists) {
        port_offset = VALID_PORT_OFFSET_MIN;
        soc_mem_field32_set(unit, SERVICE_PORT_MAPm, &spm[local_port],
                            SERVICE_PORT_OFFSETf, port_offset);
    } else {
        spm_idx_old = _TD2_SPM_ENTRIES_PER_SET *
                      soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                          SERVICE_PORT_PROFILE_INDEXf);
        rv = soc_profile_mem_get(unit, spm_profile_mem, spm_idx_old,
                                 _TD2_SPM_ENTRIES_PER_SET, spm_entries);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            goto cleanup;
        }

        qptr_old = soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm,
                                       &sqm, SERVICE_QUEUE_PTRf);
        if (qid <= qptr_old) {
            qptr_new = qid - VALID_PORT_OFFSET_MIN;
            for (i = 0; i < _TD2_SPM_ENTRIES_PER_SET; i++) {
                port_offset_old = soc_mem_field32_get(unit, SERVICE_PORT_MAPm,
                                                      &spm[i],
                                                      SERVICE_PORT_OFFSETf);
                if (i == local_port) {
                    port_offset = VALID_PORT_OFFSET_MIN;
                }  else if (!port_offset_old) {
                    /*
                     * For the old port_offsets with 0,
                     * the new port_offsest are still 0.
                     */
                    continue;
                } else {
                    port_offset = qptr_old + port_offset_old - qptr_new;
                }
                soc_mem_field32_set(unit, SERVICE_PORT_MAPm, &spm[i],
                                    SERVICE_PORT_OFFSETf, port_offset);
            }
        } else {
            port_offset = qid - qptr_old;
            soc_mem_field32_set(unit, SERVICE_PORT_MAPm, &spm[local_port],
                                SERVICE_PORT_OFFSETf, port_offset);
        }
    }
    rv = soc_profile_mem_add(unit, spm_profile_mem,
                             spm_entries, _TD2_SPM_ENTRIES_PER_SET, &spm_idx);
    if (rv != SOC_E_NONE) {
        goto cleanup;
    }

    /* 3. program the SERVICE_QUEUE_MAPm */
    if (!exists) {
        qptr_new = qid - VALID_PORT_OFFSET_MIN;
    } else {
        qptr_old = soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm,
                                       &sqm, SERVICE_QUEUE_PTRf);
        if (qid <= qptr_old) {
            qptr_new = qid - VALID_PORT_OFFSET_MIN;
        } else {
            qptr_new = qptr_old;
        }
    }

    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                        SERVICE_QUEUE_PTRf, qptr_new);
    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                        SERVICE_COS_PROFILE_INDEXf,
                        scm_idx / _TD2_SCM_ENTRIES_PER_SET);
    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                        SERVICE_PORT_PROFILE_INDEXf,
                        spm_idx / _TD2_SPM_ENTRIES_PER_SET);
    /* use port indexed and cos indexed for this base queue */
    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                        SERVICE_QUEUE_MODELf, 3);
    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf, 1);

    rv = soc_mem_write(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ALL, (int)vid, &sqm);

    if (exists) {
        if (spm_idx_old != spm_idx) {
            rv = soc_profile_mem_delete(unit, spm_profile_mem, spm_idx_old);
        }
    }

cleanup:
    if (spm != NULL) {
        sal_free(spm);
    }
    if (scm != NULL) {
        sal_free(scm);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_cosq_service_map_get
 * Purpose:
 *      Get the mapping from port, classifier, and multiple internal priorities
 *      to multiple COS queues in a queue group.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Local port ID
 *      classifier_id  - (IN) Classifier ID
 *      queue_group    - (OUT) Queue group
 *      array_max      - (IN) Size of priority_array and cosq_array
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (OUT) Array of COS queues
 *      array_count    - (OUT) Size of cosq_array
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_map_get(
    int unit,
    bcm_port_t port,
    int classifier_id,
    bcm_gport_t *queue_group,
    int array_max,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array,
    int *array_count)
{
    int                        alloc_size, port_offset, i, vid, qptr;
    int                        spm_index, scm_index, pipe;
    int                        rv = BCM_E_NONE;
    void                       *spm_entries[1];
    void                       *scm_entries[1];
    service_queue_map_entry_t  sqm;
    service_port_map_entry_t   *spm;
    service_cos_map_entry_t    *scm;
    soc_profile_mem_t          *spm_profile_mem, *scm_profile_mem;

    if (priority_array == NULL || cosq_array == NULL ||
        array_count == NULL || queue_group == NULL) {
        return BCM_E_PARAM;
    }

    if (!_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
        return BCM_E_PARAM;
    }

    /* index above 4K is for VFI */
    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);
    if (vid < 0 || vid >= SOC_MEM_SIZE(unit, SERVICE_QUEUE_MAPm)) {
        return BCM_E_PARAM;
    }

    scm_profile_mem = _bcm_td2_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_td2_service_port_map_profile[unit];

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ANY, (int)vid, &sqm));

    if (!soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf)) {
        return BCM_E_CONFIG;
    }

    qptr = soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm,
                               &sqm, SERVICE_QUEUE_PTRf);
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    qptr = (pipe == _TD2_XPIPE)? qptr: qptr - 2048;
    spm_index = _TD2_SPM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                    SERVICE_PORT_PROFILE_INDEXf);
    scm_index = _TD2_SCM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                               SERVICE_COS_PROFILE_INDEXf);

    alloc_size = sizeof(service_port_map_entry_t) * _TD2_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    alloc_size = sizeof(service_cos_map_entry_t) * _TD2_SCM_ENTRIES_PER_SET;
    scm = sal_alloc(alloc_size, "SERVICE_COS_MAP temp Mem");
    if (scm == NULL) {
        goto cleaup;
    }
    sal_memset(scm, 0, alloc_size);
    scm_entries[0] = scm;

    rv = soc_profile_mem_get(unit, spm_profile_mem,
                             spm_index, _TD2_SPM_ENTRIES_PER_SET, spm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleaup;
    }

    rv = soc_profile_mem_get(unit, scm_profile_mem,
                             scm_index, _TD2_SCM_ENTRIES_PER_SET, scm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleaup;
    }

    port_offset = soc_mem_field32_get(unit, SERVICE_PORT_MAPm,
                                      &spm[port], SERVICE_PORT_OFFSETf);
    if (port_offset == 0) {
        rv = BCM_E_NOT_FOUND;
        goto cleaup;
    }
    pipe = _BCM_TD2_PORT_TO_PIPE(unit, port);
    BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*queue_group, port,
        ((pipe * _BCM_TD2_NUM_L2_UC_LEAVES_PER_PIPE) + port_offset + qptr));

    *array_count = array_max > _TD2_SCM_ENTRIES_PER_SET ?
                   _TD2_SCM_ENTRIES_PER_SET : array_max;
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i] >= _TD2_SCM_ENTRIES_PER_SET) {
            rv = BCM_E_PARAM;
            goto cleaup;
        }
        cosq_array[i] = soc_mem_field32_get(unit, SERVICE_COS_MAPm,
                                            &scm[priority_array[i]],
                                            SERVICE_COS_OFFSETf);
    }

cleaup:
    if (spm != NULL) {
        sal_free(spm);
    }
    if (scm != NULL) {
        sal_free(scm);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td2_cosq_service_map_clear
 * Purpose:
 *      Clear the mapping from port, classifier, and internal priorities
 *      to COS queues in a queue group.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) Local port ID
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2_cosq_service_map_clear(
    int unit,
    bcm_gport_t port,
    int classifier_id)
{
    int                        i, alloc_size, vid, classifier_in_use;
    int                        rv = BCM_E_NONE;
    int                        scm_index;
    uint32                     spm_index, spm_index_new;
    void                       *spm_entries[1];
    service_port_map_entry_t   *spm;
    service_queue_map_entry_t  sqm;
    soc_profile_mem_t          *spm_profile_mem, *scm_profile_mem;

    if (!_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
        return BCM_E_PARAM;
    }

    /* index above 4K is for VFI */
    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);
    if (vid < 0 || vid >= SOC_MEM_SIZE(unit, SERVICE_QUEUE_MAPm)) {
        return BCM_E_PARAM;
    }

    scm_profile_mem = _bcm_td2_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_td2_service_port_map_profile[unit];

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ANY, (int)vid, &sqm));

    if (!soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf)) {
        return BCM_E_NONE;
    }

    /*  1. program the SERVICE_PORT_MAPm. */
    alloc_size = sizeof(service_port_map_entry_t) * _TD2_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    spm_index = _TD2_SPM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                    SERVICE_PORT_PROFILE_INDEXf);
    rv = soc_profile_mem_get(unit, spm_profile_mem,
                             spm_index, _TD2_SPM_ENTRIES_PER_SET, spm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleanup;
    }

    soc_mem_field32_set(unit, SERVICE_PORT_MAPm,
                        &spm[port], SERVICE_PORT_OFFSETf, 0);
    classifier_in_use = 0;
    for (i = 0; i < _TD2_SPM_ENTRIES_PER_SET; i++) {
        if (soc_mem_field32_get(unit, SERVICE_PORT_MAPm,
                                &spm[i], SERVICE_PORT_OFFSETf)) {
            classifier_in_use = 1;
            break;
        }
    }

    rv = soc_profile_mem_delete(unit, spm_profile_mem, spm_index);
    if (rv != SOC_E_NONE) {
        goto cleanup;
    }

    if (classifier_in_use) {
       rv = soc_profile_mem_add(unit, spm_profile_mem,
                                spm_entries, _TD2_SPM_ENTRIES_PER_SET,
                                &spm_index_new);
       if (rv != SOC_E_NONE) {
           goto cleanup;
       }
    }

    /*  2. program the SERVICE_COS_MAPm. */
    if (!classifier_in_use) {
        scm_index = _TD2_SCM_ENTRIES_PER_SET *
                    soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                        SERVICE_COS_PROFILE_INDEXf);
        rv = soc_profile_mem_delete(unit, scm_profile_mem, scm_index);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }
    }

    /*  3. program the SERVICE_QUEUE_MAPm. */
    if (classifier_in_use) {
        soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                            SERVICE_PORT_PROFILE_INDEXf,
                            spm_index_new / _TD2_SPM_ENTRIES_PER_SET);
    } else {
        sal_memset(&sqm,0, sizeof(service_queue_map_entry_t));
    }

    rv =  soc_mem_write(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ALL,
                       (int)vid, &sqm);

cleanup:
    if (spm != NULL) {
        sal_free(spm);
    }
    return rv;
}

/*
 * Function:
 *     bcm_td2p_rx_queue_channel_set
 * Purpose:
 *     Validate if the queue id to channel id mapping
 *     is in accordance with the LLS Hierarchy created
 *     If queue_id is -1, find the LLS Hierarchy & program
 *     CMIC accordingly
 *     JIRA:- SDK-57255
 * Parameters:
 *     unit  - (IN) unit number
 *     queue_id - CPU cos queue index (0 - (max cosq - 1))
 *                                    (Negative for all)
 *     chan_id - channel index (0 - (BCM_RX_CHANNELS-1))
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td2p_rx_queue_channel_set(int unit, bcm_cos_queue_t queue_id,
                              bcm_rx_chan_t chan_id)
{
#ifdef BCM_CMICM_SUPPORT
    uint32 chan_off;
    uint32 l0_index;
    uint32 l0_hw_index;
    bcm_gport_t cpu_gport;
    _bcm_td2_cosq_node_t *node;
    _bcm_td2_cosq_node_t *l0_node;
    _bcm_td2_cosq_node_t *l1_node;
    _bcm_td2_cosq_node_t *l2_queue;
    bcm_port_t local_port;
    int i,id = 0;
    int bit = 0;
    int pci_cmc = SOC_PCI_CMC(unit);
    int queue_in_tree = FALSE;
    bcm_cos_queue_t new_queue_id;
    bcm_rx_chan_t new_chan_id;
    _bcm_td2_mmu_info_t *mmu_info;

    mmu_info = _bcm_td2_mmu_info[unit];
    cpu_gport = BCM_GPORT_LOCAL_CPU;

    /* Find the cosq node for cpu port */
    BCM_IF_ERROR_RETURN
        (_bcm_td2_cosq_localport_resolve(unit, cpu_gport, &local_port));

    /* We are passing a gport to _bcm_td2_cosq_localport_resolve
     * So local_port is NOT returning gport but resolving it.
     * Ignoring the coverity error
 */
    /* coverity[overrun-local] */
    if (!IS_CPU_PORT(unit, local_port)) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                        "Cannot find CPU port. cpu gport 0x%x local port %d\n"),
                        cpu_gport, local_port));
        return BCM_E_PARAM;
    }

    id = local_port;
    node = &mmu_info->sched_node[id];

    SHR_BITCLR_RANGE(CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc),58,6);

    if (queue_id == -1) {
        /* No verification. Map all queues to corresponding channel */
        /* loop through all l0 nodes */
        /* 
         * COVERITY
         * We passed a gport to _bcm_td2_cosq_localport_resolve
         * So local_port is NOT returning gport but resolving it.
         * Ignoring the coverity error
 */
        /* coverity[overrun-local] */
        l0_node = node->child;
        while (l0_node) {
            /* loop through all l1 nodes */
            l0_hw_index = l0_node->hw_index;
            new_chan_id = -1;
            for (i = 0; i< BCM_RX_CHANNELS; i++) {
                if (bcm_td2p_cosq_rx_chan_l0_map[i] == l0_hw_index) {
                    new_chan_id = i;
                    break;
                }
            }
            if (new_chan_id == -1) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                                "Cannot find channel id for l0 node %d"
                                " Check the LLS hierarchy"
                                " configured on this device\n"),l0_hw_index));
                return BCM_E_PARAM;
            }
            SHR_BITCLR_RANGE(CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc),58,6);
            bit = bcm_td2p_cmic_to_mmu_cosmask_map[(l0_hw_index % SOC_TD2_COSQ_CPU_RESERVED_L0_BASE(unit))%_TD2PLUS_COSQ_NUM_CPU_L0_NODES];
            LOG_INFO(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "Setting bit %d & channel id %d"
                            " configured on l0 index %d\n "),
                        bit, new_chan_id, l0_hw_index));
            SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc), bit);
            l1_node = l0_node->child;
            while (l1_node) {
            /* loop through all l2 queues */
                l2_queue = l1_node->child;
                while (l2_queue) {
                    /* CPU queues start from 2000 in HW. Deriving the logical cpu queue number (0 to 48) here */
                    new_queue_id = (l2_queue->hw_index-2000)%NUM_CPU_ARM_COSQ(unit, pci_cmc);
                    BCM_IF_ERROR_RETURN(_bcm_common_rx_queue_channel_set_helper(
                                unit,
                                new_queue_id,
                                new_chan_id,
                                pci_cmc));

                    LOG_INFO(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                                    "Queue id %d & channel id %d"
                                    " configured on l0 index %d\n "),
                                new_queue_id, new_chan_id, l0_hw_index));
                    l2_queue = l2_queue->sibling;
                }
                l1_node = l1_node->sibling;
            }
            l0_node = l0_node->sibling;
        }
    } else {
        chan_off = chan_id % BCM_RX_CHANNELS;
        l0_index = bcm_td2p_cosq_rx_chan_l0_map[chan_off];
        l0_node = node->child;
        while (l0_node) {
            if (l0_node->hw_index == l0_index) {
                break;
            }
            l0_node = l0_node->sibling;
        }

        if (!l0_node) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit, "Invalid input, channel_id %d\n"), chan_id));
            return BCM_E_PARAM;
        }

        if (l0_node->hw_index != l0_index) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit, "MMU L0 node not found for this channel %d\n"), chan_id));
            return BCM_E_NOT_FOUND;
        }

        /* Verify if the given queue_id is in correct LLS hierarchy */
        l1_node = l0_node->child;
        queue_in_tree = FALSE;
        while (l1_node) {
            l2_queue = l1_node->child;
            while (l2_queue) {
                /* CPU queues start from 2000 in HW. Deriving the logical cpu queue number (0 to 48) here */
                if (queue_id == (l2_queue->hw_index-2000)%NUM_CPU_ARM_COSQ(unit, pci_cmc)) {
                    queue_in_tree = TRUE;
                    break;
                }
                l2_queue = l2_queue->sibling;
            }
            if (queue_in_tree) {
                break;
            }
            l1_node = l1_node->sibling;
        }

        if (!queue_in_tree) {
            /* Verification failed. Throw error */
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "Queue id %d & channel id %d"
                            " params do not match the LLS hierarchy"
                            " configured on this device\n"),
                        queue_id, chan_id));
            return BCM_E_PARAM;
        }
        else {
            bit = bcm_td2p_cmic_to_mmu_cosmask_map[(l0_index % SOC_TD2_COSQ_CPU_RESERVED_L0_BASE(unit))%_TD2PLUS_COSQ_NUM_CPU_L0_NODES];
            LOG_INFO(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "Setting bit %d & channel id %d"
                            " configured on l0 index %d\n "),
                        bit, chan_off, l0_index));
            SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc), bit);
            /* Verification suceeded. Program CMIC */
            BCM_IF_ERROR_RETURN(_bcm_common_rx_queue_channel_set_helper(
                        unit,
                        queue_id,
                        chan_off ,
                        pci_cmc));

            LOG_INFO(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "Queue id %d & channel id %d"
                            " configured on l0 index %d\n "),
                        queue_id, chan_off, l0_index));
        }
    }

#endif /* CMICM Support */
    return BCM_E_NONE;
}

int
bcm_td2p_cosq_ets_mode(int unit)
{
    /* second argument not being used by the function.
     * passing 0 as a workaround.
     * Should be passing local port for cpu port.
 */
    return _bcm_td2_cosq_port_has_ets(unit,0);
}

#endif /* defined(BCM_TRIDENT2_SUPPORT) */
