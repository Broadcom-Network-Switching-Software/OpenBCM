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

#if defined(BCM_APACHE_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/l3x.h>
#include <soc/macutil.h>

#include <soc/trident2.h>
#include <soc/apache.h>


#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/apache.h>
#include <bcm_int/esw/oob.h>
#include <bcm_int/esw/trx.h>

#include <bcm_int/esw_dispatch.h>

#include <bcm_int/bst.h>
#include <bcm_int/common/lock.h>

#include <bcm/subport.h>
#include <bcm_int/esw/xgs5.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#define AP_CELL_FIELD_MAX       0x1ffff
#define _BCM_PORT_SERVICE_POOL        20480
#define _BCM_WRED_GLOBAL_SERVICE_POOL 20776

/* MMU cell size in bytes */
#define _BCM_AP_BYTES_PER_CELL            208   /* bytes */
#define _BCM_AP_BYTES_TO_CELLS(_byte_)  \
    (((_byte_) + _BCM_AP_BYTES_PER_CELL - 1) / _BCM_AP_BYTES_PER_CELL)
#define _BCM_AP_NUM_PIPES                        1 
#define _BCM_AP_NUM_PORT_SCHEDULERS              76 
#define _BCM_AP_NUM_S1_SCHEDULER_PER_PIPE        328 
#define _BCM_AP_NUM_L0_SCHEDULER_PER_PIPE        1024  
#define _BCM_AP_NUM_L0_SCHEDULER                 _BCM_AP_NUM_L0_SCHEDULER_PER_PIPE * _BCM_AP_NUM_PIPES 
#define _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE        4224 /* Reserved for default val */
#define _BCM_AP_NUM_L1_SCHEDULER                 _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE * _BCM_AP_NUM_PIPES 
#define _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE        16384 /* Reserved for default val */
#define _BCM_AP_NUM_L2_UC_LEAVES                 _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE * _BCM_AP_NUM_PIPES 
#define _BCM_AP_NUM_L2_MC_LEAVES_PER_PIPE        778  
#define _BCM_AP_NUM_L2_MC_LEAVES                 _BCM_AP_NUM_L2_MC_LEAVES_PER_PIPE * _BCM_AP_NUM_PIPES 
#define _BCM_AP_NUM_L2_LEAVES_PER_PIPE     (_BCM_AP_NUM_L2_MC_LEAVES_PER_PIPE + \
                                             _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE)
#define _BCM_AP_NUM_L2_LEAVES              _BCM_AP_NUM_L2_LEAVES_PER_PIPE * _BCM_AP_NUM_PIPES 
#define _BCM_AP_NUM_TOTAL_SCHEDULERS     (_BCM_AP_NUM_PORT_SCHEDULERS + \
                                           _BCM_AP_NUM_S1_SCHEDULER_PER_PIPE + \
                                           _BCM_AP_NUM_L0_SCHEDULER +    \
                                           _BCM_AP_NUM_L1_SCHEDULER)
#define _AP_SPM_ENTRIES_PER_SET            128
#define _AP_VPM_ENTRIES_PER_SET            128
#define _AP_SCM_ENTRIES_PER_SET            16

#define _BCM_AP_LLS_DYN_CHG_REG_B 0x3FFF
#define _BCM_AP_LLS_DYN_CHG_REG_C 0xFFFFFFFF

/* All CPU leaves Ranges from 17114-17161.*/
#define _BCM_AP_NUM_L2_CPU_LEAVES                48 
/* All  Lb leaves Ranges from 17104-17111.*/
#define _BCM_AP_NUM_L2_LB_LEAVES                8 

#define _AP_NUM_COS    NUM_COS
#define _AP_NUM_INTERNAL_PRI          16

#define _BCM_AP_NUM_PFC_CLASS          8
#define  _BCM_AP_NUM_SAFC_CLASS        16 
#define _BCM_AP_COSQ_LIST_NODE_INIT(list, node)         \
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
    list[node].node_align_value  = 0;                   \
    list[node].remote_modid      = -1;                  \
    list[node].remote_port       = -1;                  \
    list[node].cosq_map          = NULL;                \
    BCM_PBMP_CLEAR(list[node].fabric_pbmp);              \
    list[node].local_port        = -1;                  \
    list[node].parent            = NULL;                \
    list[node].sibling           = NULL;                \
    list[node].child             = NULL;                \
    list[node].coe_base_index = -1;                     \
    list[node].coe_num_l2 = 0;                          \
    list[node].port_gport = -1;                                

#define _BCM_AP_COSQ_NODE_INIT(node)                    \
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
    node->node_align_value  = 0;                        \
    node->remote_modid      = -1;                       \
    node->remote_port       = -1;                       \
    node->cosq_map          = NULL;                \
    BCM_PBMP_CLEAR(node->fabric_pbmp);              \
    node->local_port        = -1;                       \
    node->parent            = NULL;                     \
    node->sibling           = NULL;                     \
    node->child             = NULL;                     \
    node->coe_base_index = -1;                          \
    node->coe_num_l2 = 0;                               \
    node->port_gport = -1;                                

typedef enum {
    _BCM_AP_COSQ_FC_PAUSE = 0,
    _BCM_AP_COSQ_FC_PFC,
    _BCM_AP_COSQ_FC_VOQFC,
    _BCM_AP_COSQ_FC_E2ECC,
    _BCM_AP_COSQ_FC_SAFC
} _bcm_ap_fc_type_t;

typedef enum {
    _BCM_AP_NODE_UNKNOWN = 0,
    _BCM_AP_NODE_UCAST,
    _BCM_AP_NODE_MCAST,
    _BCM_AP_NODE_VOQ,
    _BCM_AP_NODE_VLAN_UCAST,
    _BCM_AP_NODE_VM_UCAST,
    _BCM_AP_NODE_SERVICE_UCAST,
    _BCM_AP_NODE_SCHEDULER
} _bcm_ap_node_type_e;

typedef enum {
    _BCM_AP_COSQ_STATE_NO_CHANGE,
    _BCM_AP_COSQ_STATE_DISABLE,
    _BCM_AP_COSQ_STATE_ENABLE,
    _BCM_AP_COSQ_STATE_SPRI_TO_WRR,
    _BCM_AP_COSQ_STATE_WRR_TO_SPRI,
    _BCM_AP_COSQ_STATE_MAX
}_bcm_ap_cosq_state_t;

typedef struct _bcm_ap_cosq_node_s {
    struct _bcm_ap_cosq_node_s *parent;
    struct _bcm_ap_cosq_node_s *sibling;
    struct _bcm_ap_cosq_node_s *child;
    bcm_gport_t gport;
    int in_use;
    int base_index;
    uint16 numq_expandable;
    uint16 base_size;
    int numq;
    int hw_index;
    soc_apache_node_lvl_e level;
    _bcm_ap_node_type_e    type;
    int attached_to_input;
    int hw_cosq;
    int wrr_in_use;
    int node_align_value;

    /* DPVOQ specific information */
    bcm_port_t   local_port;
    bcm_module_t remote_modid;
    bcm_port_t   remote_port;
    bcm_pbmp_t   fabric_pbmp;
    SHR_BITDCL *cosq_map;
    int coe_num_l2;/* Number of active l2 queues for the subport*/
/*Used in S1 node to store the base index for that sub port*/
    int coe_base_index;
/* the port gport the node was created with*/
    bcm_gport_t port_gport;
}_bcm_ap_cosq_node_t;

typedef struct _bcm_ap_lls_info_s {
    int child_lvl; /* level of the node */
    int child_hw_index; /* hw index of the node */
    int node_count[SOC_APACHE_NODE_LVL_MAX]; /* total number of nodes in the lls */
    int count[SOC_APACHE_NODE_LVL_MAX]; /* number of nodes in each level */
    int offset[SOC_APACHE_NODE_LVL_MAX]; /* index of the starting node at each level */

    int kbit_max; /* max speed for the node */
    int kbit_min; /* min speed for the node */

    uint32  *mtro_entries; /* entries to maintain mmu mtro level mem */
} _bcm_ap_lls_info_t;

typedef struct _bcm_ap_cosq_list_s {
    int count;
    SHR_BITDCL *bits;
}_bcm_ap_cosq_list_t;

#define _BCM_AP_HSP_PORT_MAX_ROOT         16
#define _BCM_AP_HSP_PORT_MAX_L0           5
#define _BCM_AP_HSP_PORT_MAX_L1           10
#define _BCM_AP_HSP_PORT_MAX_L2           10
#define _BCM_AP_HSP_PORT_MAX_L2_PER_L1    2
#define _BCM_AP_HSP_PORT_MAX_COS          8

#define _BCM_AP_CPU_PORT_L0_MIN    1018 
#define _BCM_AP_CPU_PORT_L0_MAX    1023 
#define _BCM_AP_MAX_PORT           76

#define _BCM_AP_ROOT_SCHED_INDEX(u,mmu_port)       \
                ((mmu_port >= 74) ? (mmu_port - 74) : (mmu_port))

#define _BCM_AP_HSP_L0_INDEX(u,mmu_port,cosq)       \
 ((((mmu_port >= 74) ? (mmu_port - 74) : (mmu_port)) * _BCM_AP_HSP_PORT_MAX_L0) +\
  (cosq))

#define _BCM_AP_HSP_L1_INDEX(u,mmu_port,cosq)       \
 ((((mmu_port >= 74) ? (mmu_port - 74) : (mmu_port)) * _BCM_AP_HSP_PORT_MAX_L1) +\
  (cosq))

#define _BCM_AP_HSP_L2_INDEX(u,mmu_port,cosq)       \
 ((((mmu_port >= 74) ? (mmu_port - 74) : (mmu_port)) * _BCM_AP_HSP_PORT_MAX_L2) +\
  (cosq))


typedef struct _bcm_ap_pipe_resources_s {
    int num_base_queues;   /* Number of classical queues */

    _bcm_ap_cosq_list_t l2_queue_list;  /* list of l2 queues */
    _bcm_ap_cosq_list_t hsp_l2_queue_list;  /* list of l2 queues */
    _bcm_ap_cosq_list_t l0_sched_list;  /* list of l0 sched nodes */
    _bcm_ap_cosq_list_t hsp_l0_sched_list;  /* list of l0 sched nodes */
    _bcm_ap_cosq_list_t cpu_l0_sched_list;  /* list of l0 sched nodes */
    _bcm_ap_cosq_list_t l1_sched_list;  /* list of l1 sched nodes */
    _bcm_ap_cosq_list_t hsp_l1_sched_list;  /* list of l1 sched nodes */
    _bcm_ap_cosq_list_t s1_sched_list;  /* list of s1 sched nodes */

    _bcm_ap_cosq_node_t *p_queue_node;
    _bcm_ap_cosq_node_t *p_mc_queue_node;
} _bcm_ap_pipe_resources_t;

typedef struct _bcm_ap_cosq_port_info_s {
    int mc_base;
    int mc_limit;
    int uc_base;
    int uc_limit;
    int eq_base;
    int eq_limit;
    _bcm_ap_pipe_resources_t *resources;
} _bcm_ap_cosq_port_info_t;

typedef struct _bcm_ap_cosq_cpu_cosq_config_s {
    /* All MC queues have DB and MCQE space */
    int enable;
    int q_min_limit[2];
    int q_shared_limit[2];
    uint8 q_limit_dynamic[2];
    uint8 q_limit_enable[2];
    uint8 q_color_limit_enable[2];
} _bcm_ap_cosq_cpu_cosq_config_t;

#define _AP_XPIPE  0
#define _AP_YPIPE  1
#define _AP_NUM_PIPES  1

#define _BCM_AP_PORT_TO_PIPE(u, p) _AP_XPIPE 

typedef struct _bcm_ap_mmu_info_s {

    _bcm_ap_cosq_node_t sched_node[_BCM_AP_NUM_TOTAL_SCHEDULERS]; /* sched nodes */
    _bcm_ap_cosq_node_t queue_node[_BCM_AP_NUM_L2_UC_LEAVES];    /* queue nodes */
    _bcm_ap_cosq_node_t mc_queue_node[_BCM_AP_NUM_L2_MC_LEAVES]; /* queue nodes */
    _bcm_ap_cosq_port_info_t port_info[_BCM_AP_MAX_PORT];
    
    _bcm_ap_pipe_resources_t   pipe_resources[_AP_NUM_PIPES];
    
    int     ets_mode;
    int     curr_shared_limit;
    int     curr_merger_index;
/*  this should be programmed by reading the soc_info which will have
 *  the info from the software OTP 
 */
    int     num_l2_queues;
    int     _bcm_ap_default_lls;
}_bcm_ap_mmu_info_t;

#define _BCM_DEFAULT_LLS_SET(mmu_info)          \
    do {                                        \
        mmu_info->_bcm_ap_default_lls = 1;      \
    } while(0);

#define _BCM_DEFAULT_LLS_IS_SET(mmu_info)       \
    (mmu_info->_bcm_ap_default_lls)

#define _BCM_DEFAULT_LLS_RESET(mmu_info)        \
    do {                                        \
        mmu_info->_bcm_ap_default_lls = 0;      \
    } while(0);

#define _BCM_AP_START_INDEX_ALIGN_PFC       4
#define _BCM_AP_START_INDEX_ALIGN_DEFAULT   1
#define _AP_NUM_TIME_DOMAIN    4

typedef struct _bcm_ap_cosq_time_info_s {
    soc_field_t field;
    uint32 ref_count;   
} _bcm_ap_cosq_time_info_t;

/* WRED can be enabled at per UC queue / queue group/ port / service pool basis
MMU_WRED_AVG_QSIZE_X_PIPE used entry: (16384 + 4096 + 296 + 4) = 20780
16383 ~ 0: Unicast Queue
20479 ~ 16384: Queue Group per PIPE
20775 ~ 20480: Port Service Pool (Port 0 to 73)
20779 ~ 20776: Global Service Pool
*/
STATIC _bcm_ap_cosq_time_info_t time_domain1[BCM_MAX_NUM_UNITS][_AP_NUM_TIME_DOMAIN];

#define _AP_NUM_SERVICE_POOL    4

#define _BCM_AP_PRESOURCES(mi, p)  (&(mi)->pipe_resources[(p)])

STATIC _bcm_ap_mmu_info_t *_bcm_ap_mmu_info[BCM_MAX_NUM_UNITS];  /* MMU info */

STATIC soc_profile_mem_t *_bcm_ap_wred_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_ap_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_ap_sample_int_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_ap_feedback_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_ap_llfc_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_ap_voq_port_map_profile[BCM_MAX_NUM_UNITS];
static soc_profile_mem_t *_bcm_ap_ifp_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_ap_service_port_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_ap_service_cos_map_profile[BCM_MAX_NUM_UNITS];

STATIC _bcm_ap_cosq_cpu_cosq_config_t *_bcm_ap_cosq_cpu_cosq_config[BCM_MAX_NUM_UNITS][_BCM_AP_NUM_L2_CPU_LEAVES];

STATIC int
_bcm_ap_node_index_set(_bcm_ap_cosq_list_t *list, int start, int range);

STATIC int _bcm_ap_voq_next_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_ap_cosq_node_t **base_node);
STATIC int
_bcm_ap_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq);

STATIC int
_bcm_ap_map_fc_status_to_node(int unit, int port, _bcm_ap_fc_type_t fct,
                    int spad_offset, int cosq, uint32 hw_index, int level);
STATIC int
_bcm_ap_cosq_max_nodes_get(int unit, soc_apache_node_lvl_e level,
                           int *id);

typedef enum {
    _BCM_AP_COSQ_TYPE_UCAST,
    _BCM_AP_COSQ_TYPE_MCAST,
    _BCM_AP_COSQ_TYPE_EXT_UCAST
} _bcm_ap_cosq_type_t;

#define IS_AP_HSP_PORT(u,p)   \
    ((_soc_apache_port_sched_type_get((u),(p)) == SOC_APACHE_SCHED_HSP) ? 1 : 0)

typedef struct _bcm_ap_endpoint_s {
    uint32 flags;       /* BCM_COSQ_CLASSIFIER_xxx flags */
    bcm_vlan_t vlan;    /* VLAN */
    bcm_mac_t mac;      /* MAC address */
    bcm_vrf_t vrf;      /* Virtual Router Instance */
    bcm_ip_t ip_addr;   /* IPv4 address */
    bcm_ip6_t ip6_addr; /* IPv6 address */
    bcm_gport_t gport;  /* GPORT ID */
} _bcm_ap_endpoint_t;

typedef struct _bcm_ap_endpoint_queuing_info_s {
    int num_endpoint; /* Number of endpoints */
    _bcm_ap_endpoint_t **ptr_array; /* Array of pointers to endpoints */
    soc_profile_mem_t *cos_map_profile; /* Internal priority to CoS queue
                                           mapping profile */
} _bcm_ap_endpoint_queuing_info_t;

STATIC int 
_bcm_ap_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        int *p_pool_start, int *p_pool_end);

STATIC _bcm_ap_endpoint_queuing_info_t *_bcm_ap_endpoint_queuing_info[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_ap_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg);

STATIC int
_bcm_ap_fc_status_map_gport(int unit, int port, 
                bcm_gport_t gport, _bcm_ap_fc_type_t fct, uint32 *cos_bmp);
STATIC int
_bcm_ap_port_fc_profile_set(int unit, int port, _bcm_cosq_op_t op,
                              int entry_cnt, int *pfc_class, uint32 *cos_bmp);
STATIC int 
_bcm_ap_cosq_node_get(int unit, bcm_gport_t gport,
    bcm_cos_queue_t cosq,bcm_module_t *modid, bcm_port_t *port,
    int *id, _bcm_ap_cosq_node_t **node);

int _bcm_ap_get_s1_node(int unit, bcm_gport_t subport,
                      _bcm_ap_cosq_node_t **s1_node) ;
STATIC int
_bcm_ap_cosq_sched_parent_child_set(int unit, int port, int level,
                                     int sched_index, int child_index,
                                     soc_apache_sched_mode_e sched_mode, 
                                     int weight, _bcm_ap_cosq_node_t *child_node);
#define _BCM_AP_NUM_ENDPOINT(unit) \
    (_bcm_ap_endpoint_queuing_info[unit]->num_endpoint)
    
#define _BCM_AP_ENDPOINT(unit, id) \
    (_bcm_ap_endpoint_queuing_info[unit]->ptr_array[id])

#define _BCM_AP_ENDPOINT_IS_USED(unit, id) \
    (_bcm_ap_endpoint_queuing_info[unit]->ptr_array[id] != NULL)

#define _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit) \
    (_bcm_ap_endpoint_queuing_info[unit]->cos_map_profile)


/*
 * Function:
 *     _bcm_ap_cosq_map_index_set
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
_bcm_ap_cosq_map_index_set(SHR_BITDCL *list, int start, int range)
{
    SHR_BITSET_RANGE(list, start, range);
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)

#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_2

STATIC int
_bcm_ap_cosq_wb_alloc(int unit)
{
    _bcm_ap_mmu_info_t *mmu_info;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size, node_list_sizes[3], ii, jj;
    uint32 shadow_size = 0;
    
    mmu_info = _bcm_ap_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    node_list_sizes[0] = _BCM_AP_NUM_L2_UC_LEAVES;
    node_list_sizes[1] = _BCM_AP_NUM_L2_MC_LEAVES;
    node_list_sizes[2] = _BCM_AP_NUM_TOTAL_SCHEDULERS;

    alloc_size = 0;
    for(ii = 0; ii < 3; ii++) {
        alloc_size += sizeof(uint32);
        for(jj = 0; jj < node_list_sizes[ii]; jj++) {
            alloc_size += 8 * sizeof(uint32);
        }
    }

    /* Size of  TIME_DOMAINr field and count reference */
    alloc_size += _AP_NUM_TIME_DOMAIN * sizeof(_bcm_ap_cosq_time_info_t);

    soc_apache_fc_map_shadow_size_get(unit, &shadow_size);
    alloc_size += shadow_size;

    /* added in BCM_WB_VERSION_1_1
     * to sync _bcm_ap_mmu_info[unit]->ets_mode/curr_shared_limit
     */
    alloc_size += 2 * sizeof(int);

    /* added in BCM_WB_VERSION_1_1
     * to sync the reference count of IFP_COS_MAPm 
     */
    alloc_size += (SOC_MEM_SIZE(unit,IFP_COS_MAPm) / _AP_NUM_INTERNAL_PRI) *
                  sizeof(uint16);

    alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) /
                  _AP_SCM_ENTRIES_PER_SET;
    alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) /
                  _AP_SPM_ENTRIES_PER_SET;

    alloc_size += sizeof(uint16) * SOC_MEM_SIZE(unit, VOQ_PORT_MAPm) /
                  _AP_VPM_ENTRIES_PER_SET;
    /*
     * Sync num_cos value
     */
    alloc_size += sizeof(int);

    /*
     *   Added in BCM_WB_VERSION_1_1 to sync _bcm_ap_endpoint_queueing_info
     */
    if (soc_feature(unit, soc_feature_endpoint_queuing)) {
        alloc_size += ((_BCM_AP_NUM_ENDPOINT(unit) - 1 ) *
                (sizeof(uint32) + sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t)));

        /* Added in BCM_WB_VERSION_1_2 to sync ENDPOINT_COS_MAP memory profile */
        alloc_size += (SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) / _AP_NUM_INTERNAL_PRI) *
                       sizeof(uint16);
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

#define _BCM_AP_WB_NTYPE_UCAST 0
#define _BCM_AP_WB_NTYPE_MCAST 1
#define _BCM_AP_WB_NTYPE_SCHED 2


/* 2 bit node type, 15 bit nid, 15 hw cosq */
#define _BCM_AP_WB_SET_NODE_W1(ntype,nid,hwcosq)    \
    (((ntype & 3)) | \
     (((nid) & 0x7fff) << 2) | \
     (((hwcosq) & 0x7fff) << 17))

#define _BCM_AP_WB_GET_NODE_TYPE_W1(w1)     ((w1) & 3)
#define _BCM_AP_WB_GET_NODE_ID_W1(w1)       (((w1) >> 2) & 0x7fff)
#define _BCM_AP_WB_GET_NODE_HW_COSQ_W1(w1)  (((w1) >> 17) & 0x7fff)


/* 15 bit hw index, 13 bit parent id, 3 bit level, 1 bit wrr_in_use
 */

#define _BCM_AP_WB_SET_NODE_W2(hwindex,parentid,lvl,wrr_in_use) \
    ((hwindex & 0x7fff) | \
     ((parentid & 0x1fff) << 15) | \
     ((lvl & 0x7) << 28) | \
     ((wrr_in_use & 0x1) << 31))

#define _BCM_AP_WB_GET_NODE_HW_INDEX_W2(w1)  ((w1) & 0x7fff) 
#define _BCM_AP_WB_GET_NODE_PARENTID_W2(w1)  (((w1) >> 15) & 0x1fff)
#define _BCM_AP_WB_GET_NODE_LEVEL_W2(w1)     (((w1) >> 28) & 0x7)
#define _BCM_AP_WB_GET_NODE_WRR_IN_USE_W2(w1)   (((w1) >> 31) & 0x1)

#define _BCM_AP_WB_SET_NODE_W3(gport)    (gport)
#define _BCM_AP_WB_SET_NODE_W4(gport)    (gport)

/* 15 bit numq, 15 bit cosq, 1 bit dmvoq_en, 1 bit coe_en
 */
#define _BCM_AP_WB_SET_NODE_W5(numq,cosq,dmvoq_en,coe_en) \
    ((numq & 0x7fff) | \
    ((cosq & 0x7fff) << 15) | \
    ((dmvoq_en &0x01) << 30) | \
    ((coe_en & 0x01) << 31))

#define _BCM_AP_WB_GET_NODE_NUMQ_W5(w1)      ((w1) & 0x7fff)
#define _BCM_AP_WB_GET_NODE_COSQATTT_W5(w1)  (((w1) >> 15) & 0x7fff)
#define _BCM_AP_WB_GET_NODE_DMVOQ_EN_W5(w1)  (((w1) >> 30) & 0x1)
#define _BCM_AP_WB_GET_NODE_COE_EN_W5(w1)    (((w1) >> 31) & 0x1)

/* 10 bit base size, 15 bit base_index, 1 bit expand, 4 bit type */
/* Encoding node_align_value in 1 bit 0->0, 1->4 */
#define _BCM_AP_WB_SET_NODE_W6(base_size,base_index,expand,type, align) \
    ((base_size & 0x3ff) | \
     ((base_index & 0x7fff) << 10) | \
     ((expand & 0x1) << 25) | \
     ((type & 0xf) << 26) | \
     ((align & 0x1) << 30))

#define _BCM_AP_WB_GET_BASE_SIZE_W6(w1)   ((w1) & 0x3ff)
#define _BCM_AP_WB_GET_BASE_INDEX_W6(w1)  (((w1) >> 10) & 0x7fff)
#define _BCM_AP_WB_GET_EXPAND_W6(w1)      (((w1) >> 25) & 0x1)
#define _BCM_AP_WB_TYPE_W6(w1)            (((w1) >> 26) & 0xf)
#define _BCM_AP_WB_GET_ALIGN_W6(w1)       (((w1) >> 30) & 0x1)

/* 9 bit remote_modid, 9 bit remote_port.
 * Present only if dmvoq bit is set in W6
 */
#define _BCM_AP_WB_SET_NODE_W7(remote_modid,remote_port)    \
    ((((remote_modid) & 0x1ff)) | \
     (((remote_port) & 0x1ff) << 9))

#define _BCM_AP_WB_GET_NODE_DEST_MODID_W7(w1) ((w1) & 0x1ff)
#define _BCM_AP_WB_GET_NODE_DEST_PORT_W7(w1)  (((w1) >> 9) & 0x1ff)

/* 14 bit coe base index, 8 bit coe_num_l2 */
#define _BCM_AP_WB_SET_NODE_W8(coe_base_index, coe_num_l2) \
       ((coe_base_index & 0x3fff) | \
       ((coe_num_l2 & 0xff) << 14))

#define _BCM_AP_WB_GET_COE_BASE_INDEX_W8(w1) ((w1) & 0x3fff)
#define _BCM_AP_WB_GET_COE_NUM_L2_W8(w1) (((w1) >> 14) & 0xff)

#define _BCM_AP_NODE_ID(np,np0) (np - np0)

STATIC void *
_bcm_ap_cosq_alloc_clear(void *cp, unsigned int size, char *description);


STATIC int
_bcm_ap_child_level_get(int unit, _bcm_ap_cosq_node_t *node , int *child_level)
{
   
   if (!node) 
   {
       return BCM_E_INTERNAL;
   }
   if (node->level == SOC_APACHE_NODE_LVL_ROOT)
   {
       *child_level = SOC_APACHE_NODE_LVL_S1;
 
   }else if (node->level == SOC_APACHE_NODE_LVL_S1) 
   {
       *child_level = SOC_APACHE_NODE_LVL_L0;
   }else if (node->level == SOC_APACHE_NODE_LVL_L0) 
   {
       *child_level = SOC_APACHE_NODE_LVL_L1;
   }else if (node->level == SOC_APACHE_NODE_LVL_L1) 
   {
       *child_level = SOC_APACHE_NODE_LVL_L2;
   }
   return BCM_E_NONE; 
}
/*
 * Function:
 *     _bcm_ap_cosq_map_alloc_set
 * Purpose:
 *     Allocate the cosq_map bitmap and 
 *     restore  
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_ap_cosq_map_alloc_set(int unit, _bcm_ap_cosq_node_t *node)
{
    _bcm_ap_cosq_node_t *child;
    int alloc_size;
    int max_nodes = 0; 
    int child_level = 0;
    
    BCM_IF_ERROR_RETURN
    (_bcm_ap_child_level_get(unit, node, &child_level));
    BCM_IF_ERROR_RETURN
    (_bcm_ap_cosq_max_nodes_get(unit, child_level, &max_nodes)); 
    if (node->cosq_map == NULL) {
        if ( node->numq == -1 ) { 
             alloc_size = SHR_BITALLOCSIZE(max_nodes); 
        } else { 
             alloc_size = SHR_BITALLOCSIZE(node->numq);
        }
        node->cosq_map = _bcm_ap_cosq_alloc_clear(node->cosq_map, 
                                           alloc_size,
                                           "cosq_map");
        if (node->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }
    child = node->child;
    while (child != NULL) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_map_index_set(node->cosq_map, 
                                         child->attached_to_input, 1));
        child = child->sibling;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_map_node_recover 
 * Purpose:
 *     Used to recover cos_map for S0,S1,L0,L1 levels   
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_ap_cosq_map_node_recover(int unit ,_bcm_ap_cosq_node_t *node)
{
    if (!node) {
        return BCM_E_NONE;
    }
    if (node->level == SOC_APACHE_NODE_LVL_L2) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_map_alloc_set(unit , node));
    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_map_node_recover(unit, node->sibling));
    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_map_node_recover(unit, node->child));
    return BCM_E_NONE;
}
/*
 * Function : 
 *       bcm_ap_cosq_endpoint_recover
 * Purpose :
 *       Recover endpoint info from scache
 * Parameters:
 *      unit       -(IN) unit number 
 *      scache_ptr -(IN/OUT)scache pointer
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
bcm_ap_cosq_endpoint_recover(int unit, uint8 **scache_ptr)
{
    int entry_size;
    int flags;
    int ii;

    entry_size = sizeof(uint32) + sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t);
    /* entry 0 was reserved and is not synced. */
    for (ii = 1; ii < _BCM_AP_NUM_ENDPOINT(unit); ii++) {
        sal_memcpy(&flags, (*scache_ptr), sizeof(uint32));

        if ((flags & BCM_COSQ_CLASSIFIER_L2) ||
                (flags & BCM_COSQ_CLASSIFIER_L3) ||
                (flags & BCM_COSQ_CLASSIFIER_GPORT)) {
            _BCM_AP_ENDPOINT(unit, ii) = sal_alloc(
                    sizeof(_bcm_ap_endpoint_t), "Endpoint Info");
            if (_BCM_AP_ENDPOINT(unit, ii) == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_BCM_AP_ENDPOINT(unit, ii), 0,
                    sizeof(_bcm_ap_endpoint_t));
            _BCM_AP_ENDPOINT(unit, ii)->flags = flags;
            if (flags & BCM_COSQ_CLASSIFIER_L2) {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_AP_ENDPOINT(unit, ii)->vlan),
                        (*scache_ptr), sizeof(bcm_vlan_t));
                (*scache_ptr) += sizeof(bcm_vlan_t);
                sal_memcpy(_BCM_AP_ENDPOINT(unit, ii)->mac, (*scache_ptr),
                        sizeof(bcm_mac_t));
                (*scache_ptr) += (entry_size - sizeof(uint32) -
                        sizeof(bcm_vlan_t));
            } else if (flags & BCM_COSQ_CLASSIFIER_L3) {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_AP_ENDPOINT(unit, ii)->vrf),
                        (*scache_ptr), sizeof(bcm_vrf_t));
                (*scache_ptr) += sizeof(bcm_vrf_t);
                if (flags & BCM_COSQ_CLASSIFIER_IP6) {
                    sal_memcpy(_BCM_AP_ENDPOINT(unit, ii)->ip6_addr, (*scache_ptr),
                            sizeof(bcm_ip6_t));
                } else {
                    sal_memcpy(&(_BCM_AP_ENDPOINT(unit, ii)->ip_addr), (*scache_ptr),
                            sizeof(bcm_ip_t));
                }
                (*scache_ptr) += (entry_size - sizeof(uint32) -
                        sizeof(bcm_vrf_t));
            } else {
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy(&(_BCM_AP_ENDPOINT(unit, ii)->gport),
                        (*scache_ptr), sizeof(bcm_gport_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            }
        } else {
            (*scache_ptr) += entry_size;
        }
    }

    return BCM_E_NONE;
}

int
bcm_ap_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    uint32 *u32_scache_ptr, wval, fval;
    uint16 *u16_scache_ptr;
    _bcm_ap_cosq_node_t *node;
    bcm_port_t port;
    int rv, ii, jj, set_index;
    _bcm_ap_cosq_node_t *nodes[3];
    int xnode_id, stable_size, count, profile_index;
    _bcm_ap_mmu_info_t *mmu_info;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_profile_mem_t *profile_mem;
    _bcm_ap_pipe_resources_t *res;
    soc_mem_t mem;
    mmu_wred_config_entry_t qentry;
    int alloc_size;
    int additional_scache_size = 0;
    uint16 recovered_ver = 0;
    int index=0;
    bcm_pbmp_t all_pbmp;
    soc_profile_reg_t *profile_reg;
    soc_info_t *si;
    bcm_port_t phy_port, mmu_port;
    soc_reg_t reg;
	uint32 tmp32;
    int dmvoq_en = 0, coe_en = 0;
    uint64 rval, index_map;
    static const soc_reg_t llfc_cfgr[] = {
                PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };
    int num;
    _bcm_ap_cosq_node_t *port_info;

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

    mmu_info = _bcm_ap_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    res = _BCM_AP_PRESOURCES(mmu_info, _AP_XPIPE);
    u32_scache_ptr = (uint32*) scache_ptr;

    nodes[0] = &mmu_info->queue_node[0];
    nodes[1] = &mmu_info->mc_queue_node[0];
    nodes[2] = &mmu_info->sched_node[0];

    for(ii = 0; ii < 3; ii++) {
        count = *u32_scache_ptr;
        u32_scache_ptr++;
        for(jj = 0; jj < count; jj++) {
            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            xnode_id = _BCM_AP_WB_GET_NODE_ID_W1(wval);
            node = nodes[ii] + xnode_id;
            node->in_use = TRUE;
            node->numq = 1;
            fval = _BCM_AP_WB_GET_NODE_HW_COSQ_W1(wval);
            node->hw_cosq = (fval == 0x7fff)? -1 : fval;

            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            fval = _BCM_AP_WB_GET_NODE_HW_INDEX_W2(wval);
            node->hw_index = (fval == 0x7fff) ? -1 : fval;
            fval = _BCM_AP_WB_GET_NODE_PARENTID_W2(wval);
            if (fval == 0x1fff) {
                node->parent = NULL;
            } else {
                node->parent = &mmu_info->sched_node[fval];
                if (node->parent->child) {
                    node->sibling = node->parent->child;
                }
                node->parent->child = node;
            }
            node->level = _BCM_AP_WB_GET_NODE_LEVEL_W2(wval);
            node->wrr_in_use = _BCM_AP_WB_GET_NODE_WRR_IN_USE_W2(wval);

            node->gport = *u32_scache_ptr++;
            node->port_gport = *u32_scache_ptr++;

            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            node->numq = _BCM_AP_WB_GET_NODE_NUMQ_W5(wval);
             /* numq of -1 will be recovered as 32767 need to update back as -1  */
            if (node->numq == 32767)
            {
                node->numq = -1 ;
            }
            fval = _BCM_AP_WB_GET_NODE_COSQATTT_W5(wval);
            node->attached_to_input = (fval == 0x7fff) ? -1 : fval;
            dmvoq_en = _BCM_AP_WB_GET_NODE_DMVOQ_EN_W5(wval);
            coe_en = _BCM_AP_WB_GET_NODE_COE_EN_W5(wval);

            wval = *u32_scache_ptr;
            u32_scache_ptr++;
            node->base_size = _BCM_AP_WB_GET_BASE_SIZE_W6(wval);
            fval = _BCM_AP_WB_GET_BASE_INDEX_W6(wval);
            node->base_index = (fval == 0x7fff) ? -1 : fval;
            node->numq_expandable = _BCM_AP_WB_GET_EXPAND_W6(wval);
            node->type = _BCM_AP_WB_TYPE_W6(wval);
            node->node_align_value = (_BCM_AP_WB_GET_ALIGN_W6(wval) ? 4 : 0);

            if (dmvoq_en) {
                wval = *u32_scache_ptr;
                u32_scache_ptr++;
                fval = _BCM_AP_WB_GET_NODE_DEST_MODID_W7(wval);
                node->remote_modid = (fval == 0x1ff) ? -1 : fval;
                fval = _BCM_AP_WB_GET_NODE_DEST_PORT_W7(wval);
                node->remote_port = (fval == 0x1ff) ? -1 : fval;
            }

            if (soc_feature(unit, soc_feature_mmu_hqos_four_level) &&
                (node->level == SOC_APACHE_NODE_LVL_S1) &&
                coe_en) {
                wval = *u32_scache_ptr;
                u32_scache_ptr++;
                node->coe_base_index = _BCM_AP_WB_GET_COE_BASE_INDEX_W8(wval);
                if (node->coe_base_index == 16383 ) 
                {
                    node->coe_base_index = -1;
                }  
                node->coe_num_l2 = _BCM_AP_WB_GET_COE_NUM_L2_W8(wval);
            }

            /*
             * Not checking the return value intentionally
             * to take care of warmboot case.
             */
            /* coverity[unchecked_value] */
            _bcm_ap_cosq_node_get(unit, node->gport, 0, NULL,
                &node->local_port, NULL, NULL);

            if (node->hw_index != -1) {
                if (node->level == SOC_APACHE_NODE_LVL_S1) {
                    _bcm_ap_node_index_set(&res->s1_sched_list, 
                            node->hw_index, 1);
                } else if (node->level == SOC_APACHE_NODE_LVL_L0) {
                    _bcm_ap_node_index_set(&res->l0_sched_list, 
                            node->hw_index, 1);
                    if (IS_AP_HSP_PORT(unit, node->local_port)) {
                        _bcm_ap_node_index_set(&res->hsp_l0_sched_list, 
                                node->hw_index, 1);
                    }
                    if (IS_CPU_PORT(unit, node->local_port)) {
                        _bcm_ap_node_index_set(&res->cpu_l0_sched_list, 
                                node->hw_index - _BCM_AP_CPU_PORT_L0_MIN, 1);
                    }
                } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
                    _bcm_ap_node_index_set(&res->l1_sched_list, 
                            node->hw_index, 1);
                    if (IS_AP_HSP_PORT(unit, node->local_port)) {
                        _bcm_ap_node_index_set(&res->hsp_l1_sched_list, 
                                node->hw_index, 1);
                    }
                } else if (node->level == SOC_APACHE_NODE_LVL_L2) {
                    _bcm_ap_node_index_set(&res->l2_queue_list, 
                            node->hw_index, 1);
                    if (IS_AP_HSP_PORT(unit, node->local_port)) {
                        _bcm_ap_node_index_set(&res->hsp_l2_queue_list, 
                                node->hw_index, 1);
                    }
                }
            }
        }
    }

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

    /* Restoring the cosq_map */ 
    PBMP_ITER(all_pbmp, port) {
        node = NULL;

        /* root node */
        port_info = &mmu_info->sched_node[port];
        if (port_info && (port_info->gport >= 0)) {
            SOC_IF_ERROR_RETURN (
                _bcm_ap_cosq_map_node_recover(unit, port_info));
        }
    }

    /* Recovery TIME_DOMAINr field and count reference */
    alloc_size = _AP_NUM_TIME_DOMAIN * sizeof(_bcm_ap_cosq_time_info_t);
    sal_memcpy(&time_domain1[unit][0], u32_scache_ptr, alloc_size);
    time_domain1[unit][0].field = TIME_0f;
    time_domain1[unit][1].field = TIME_1f;
    time_domain1[unit][2].field = TIME_2f;
    time_domain1[unit][3].field = TIME_3f;
    u32_scache_ptr += alloc_size / sizeof(uint32);
    soc_apache_fc_map_shadow_load(unit, &u32_scache_ptr);

    mmu_info->curr_shared_limit = *u32_scache_ptr++;

    u16_scache_ptr = (uint16 *)u32_scache_ptr;

    /* Update IFP_COS_MAP memory profile reference counter */
    profile_mem = _bcm_ap_ifp_cos_map_profile[unit];
    num = SOC_MEM_SIZE(unit,IFP_COS_MAPm) / _AP_NUM_INTERNAL_PRI;
    for (ii = 0; ii < num; ii++) {
        for (jj = 0; jj < *u16_scache_ptr; jj++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, profile_mem,
                                            ii * _AP_NUM_INTERNAL_PRI,
                                            _AP_NUM_INTERNAL_PRI));
        }
        u16_scache_ptr++;
    }

    profile_mem = _bcm_ap_service_cos_map_profile[unit];
    num = SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) / _AP_SCM_ENTRIES_PER_SET;
    for (ii = 0; ii < num; ii++) {
        for (jj = 0; jj < *u16_scache_ptr; jj++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, profile_mem,
                                            ii * _AP_SCM_ENTRIES_PER_SET,
                                            _AP_SCM_ENTRIES_PER_SET));
        }
        u16_scache_ptr++;
    }

    profile_mem = _bcm_ap_service_port_map_profile[unit];
    num = SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) / _AP_SPM_ENTRIES_PER_SET;
    for (ii = 0; ii < num; ii++) {
        for (jj = 0; jj < *u16_scache_ptr; jj++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, profile_mem,
                                            ii * _AP_SPM_ENTRIES_PER_SET,
                                            _AP_SPM_ENTRIES_PER_SET));
        }
        u16_scache_ptr++;
    }

    profile_mem = _bcm_ap_voq_port_map_profile[unit];
    num = SOC_MEM_SIZE(unit,VOQ_PORT_MAPm) / _AP_VPM_ENTRIES_PER_SET;
    for (ii = 0; ii < num; ii++) {
         for (jj = 0; jj < *u16_scache_ptr; jj++) {
              SOC_IF_ERROR_RETURN(
              soc_profile_mem_reference(unit, profile_mem,
                                        ii * _AP_VPM_ENTRIES_PER_SET,
                                        _AP_VPM_ENTRIES_PER_SET));
         }
         u16_scache_ptr++;
    }

    u32_scache_ptr = (uint32 *)u16_scache_ptr;       
    NUM_COS(unit) = *u32_scache_ptr++;
    scache_ptr = (uint8*)u32_scache_ptr;
    
    /*
     * BCM_WB_VERSION_1_1 sync endpoint info
     */
    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            scache_ptr = (uint8 *)u32_scache_ptr;
            BCM_IF_ERROR_RETURN(
                    bcm_ap_cosq_endpoint_recover(unit, &scache_ptr));
        }
    } else {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            additional_scache_size += ((_BCM_AP_NUM_ENDPOINT(unit) - 1)*
                    (sizeof(uint32) + sizeof(bcm_vrf_t)+ sizeof(bcm_ip6_t)));
        }
    }

    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            u16_scache_ptr = (uint16 *)scache_ptr;
            /*
             * BCM_WB_VERSION_1_2
             * Update ENDPOINT_COS_MAP memory profile reference counter
             */
            profile_mem = _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile;
            num = SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) / _AP_NUM_INTERNAL_PRI;
            for (ii = 0; ii < num; ii++) {
                for (jj = 0; jj < *u16_scache_ptr; jj++) {
                    SOC_IF_ERROR_RETURN(soc_profile_mem_reference(unit, profile_mem,
                                                                  ii * _AP_NUM_INTERNAL_PRI,
                                                                  _AP_NUM_INTERNAL_PRI));
                }
                u16_scache_ptr++;
            }
        }
    } else {
        if (soc_feature(unit, soc_feature_endpoint_queuing)) {
            additional_scache_size += (SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) / _AP_NUM_INTERNAL_PRI) *
                                       sizeof(uint16);
        }
    }

    if (additional_scache_size > 0) {
    /* coverity[dead_error_begin] */
        BCM_IF_ERROR_RETURN(
            soc_scache_realloc(unit, scache_handle, additional_scache_size));
    }

    /* Update reference counts for DSCP_TABLE profile*/
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
    profile_mem = _bcm_ap_cos_map_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)){
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
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16,  16));
    }

    mem = MMU_WRED_CONFIG_X_PIPEm;
    for (ii = soc_mem_index_min(unit, mem);
            ii <= soc_mem_index_max(unit, mem); ii++) {
        BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &qentry));
            
        profile_index = soc_mem_field32_get(unit, mem, &qentry, PROFILE_INDEXf);
            
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, _bcm_ap_wred_profile[unit],
                                        profile_index, 1));
    }
    
    profile_reg = _bcm_ap_llfc_profile[unit];
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
 * Function : 
 *      bcm_ap_cosq_endpoint_sync
 * Purpose :
 *      Sync endpoint info to scache
 * Parameters:
 *      unit      -(IN) unit number
 *      scache_ptr-(IN/OUT) scache pointer
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_ap_cosq_endpoint_sync(int unit, uint8 **scache_ptr)
{
    uint32 endpoint_flags, entry_size;
    int ii;

    /* entry 0 was reserved and is not synced. */
    entry_size = sizeof(uint32)+ sizeof(bcm_vrf_t) + sizeof(bcm_ip6_t);
    for (ii = 1; ii < _BCM_AP_NUM_ENDPOINT(unit); ii++) {
        if (_BCM_AP_ENDPOINT_IS_USED(unit, ii)) {
            endpoint_flags = _BCM_AP_ENDPOINT(unit, ii)->flags;
            if (endpoint_flags & BCM_COSQ_CLASSIFIER_L2) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_AP_ENDPOINT(unit, ii)->vlan),
                        sizeof(bcm_vlan_t));
                (*scache_ptr) += sizeof(bcm_vlan_t);
                sal_memcpy((*scache_ptr), _BCM_AP_ENDPOINT(unit, ii)->mac,
                        sizeof(bcm_mac_t));
                (*scache_ptr) += (entry_size - sizeof(uint32) - sizeof(bcm_vlan_t));
            } else if (endpoint_flags & BCM_COSQ_CLASSIFIER_L3) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_AP_ENDPOINT(unit, ii)->vrf),
                        sizeof(bcm_vrf_t));
                (*scache_ptr) += sizeof(bcm_vrf_t);
                if (endpoint_flags & BCM_COSQ_CLASSIFIER_IP6) {
                    sal_memcpy((*scache_ptr),
                            _BCM_AP_ENDPOINT(unit, ii)->ip6_addr,
                            sizeof(bcm_ip6_t));
                } else {
                    sal_memcpy((*scache_ptr),
                            &(_BCM_AP_ENDPOINT(unit, ii)->ip_addr),
                            sizeof(bcm_ip_t));
                }
                (*scache_ptr) += (entry_size - sizeof(uint32) - sizeof(bcm_vrf_t));
            } else if (endpoint_flags & BCM_COSQ_CLASSIFIER_GPORT) {
                sal_memcpy((*scache_ptr), &endpoint_flags, sizeof(uint32));
                (*scache_ptr) += sizeof(uint32);
                sal_memcpy((*scache_ptr), &(_BCM_AP_ENDPOINT(unit, ii)->gport),
                        sizeof(bcm_gport_t));
                (*scache_ptr) += (entry_size - sizeof(uint32));
            } else {
                return BCM_E_INTERNAL;
            }
        } else {
            sal_memset((*scache_ptr), 0, entry_size);
            (*scache_ptr) += entry_size;
        }
    }

    return BCM_E_NONE; 
}


int
bcm_ap_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    uint32 *u32_scache_ptr;
    uint16 *u16_scache_ptr;
    _bcm_ap_cosq_node_t *node;
    int node_list_sizes[3], ii, jj, cosq;
    _bcm_ap_cosq_node_t *nodes[3];
    int xnode_id;  /*, count, pipe; */
    _bcm_ap_mmu_info_t *mmu_info;
    int alloc_size;
    int rv = BCM_E_NONE;
    int ref_count;
    int profile_num, count;
    soc_profile_mem_t *profile_mem;
    int dmvoq_en = 0, coe_en = 0;
    uint32 *psize;

    mmu_info = _bcm_ap_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    u32_scache_ptr = (uint32*) scache_ptr;

    nodes[0] = &mmu_info->queue_node[0];
    node_list_sizes[0] = _BCM_AP_NUM_L2_UC_LEAVES;
    nodes[1] = &mmu_info->mc_queue_node[0];
    node_list_sizes[1] = _BCM_AP_NUM_L2_MC_LEAVES;
    nodes[2] = &mmu_info->sched_node[0];
    node_list_sizes[2] = _BCM_AP_NUM_TOTAL_SCHEDULERS;

    for(ii = 0; ii < 3; ii++) {
        psize = u32_scache_ptr++;
        for(count = 0, jj = 0; jj < node_list_sizes[ii]; jj++) {
            node = nodes[ii] + jj;
            if (node->in_use == FALSE) {
                continue;
            }
            dmvoq_en = 0;
            coe_en = 0;
            count += 1;

            if (node->parent == NULL) {
                xnode_id = 0x1fff;
            } else {
                xnode_id = _BCM_AP_NODE_ID(node->parent, mmu_info->sched_node);
            }
            cosq = ((node->attached_to_input == -1) ? 0x7fff : node->attached_to_input);

            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W1(ii, jj, 
                                    ((node->hw_cosq == -1) ? 0x7fff : node->hw_cosq));

            if ((node->remote_modid != -1) || (node->remote_port != -1)) {
                dmvoq_en = 1;
            }
            if (soc_feature(unit, soc_feature_mmu_hqos_four_level) && 
                (node->level == SOC_APACHE_NODE_LVL_S1)) { 
                coe_en = 1;
            }

            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W2(
                   (node->hw_index == -1) ? 0x7fff : node->hw_index, xnode_id,
                    node->level, node->wrr_in_use);

            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W3(node->gport);
            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W4(node->port_gport);
            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W5(node->numq, cosq, 
                                        dmvoq_en, coe_en);

            *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W6(
                                    (node->base_size),
                                    ((node->base_index == -1 ) ? 0x7fff : 
                                                        node->base_index),
                                    (node->numq_expandable), (node->type),
                                    ((node->node_align_value) ? 1 : 0));

            if (dmvoq_en) {
                *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W7(
                    (node->remote_modid == -1) ? 0x1ff : node->remote_modid,
                    (node->remote_port == -1) ? 0x1ff : node->remote_port);
            }

            if (soc_feature(unit, 
                      soc_feature_mmu_hqos_four_level)) { 
                if (node->level == SOC_APACHE_NODE_LVL_S1) { 
                    *u32_scache_ptr++ = _BCM_AP_WB_SET_NODE_W8(node->coe_base_index,
                                                                  node->coe_num_l2);
                }
            } 
        }
        *psize = count;
    }
    
    /* Sync TIME_DOMAINr field and count reference */
    alloc_size = _AP_NUM_TIME_DOMAIN * sizeof(_bcm_ap_cosq_time_info_t);
    sal_memcpy(u32_scache_ptr, &time_domain1[unit][0], alloc_size);
    u32_scache_ptr += alloc_size / sizeof(uint32);
    soc_apache_fc_map_shadow_sync(unit, &u32_scache_ptr);

    *u32_scache_ptr++ = mmu_info->curr_shared_limit;
    u16_scache_ptr = (uint16 *)u32_scache_ptr;

    for (ii = 0; 
         ii < (SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _AP_NUM_INTERNAL_PRI);
         ii++) {
        rv = soc_profile_mem_ref_count_get(unit, 
                                           _bcm_ap_ifp_cos_map_profile[unit],
                                           ii * _AP_NUM_INTERNAL_PRI,
                                           &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }

    profile_num = SOC_MEM_SIZE(unit, SERVICE_COS_MAPm) /
                  _AP_SCM_ENTRIES_PER_SET;
    profile_mem = _bcm_ap_service_cos_map_profile[unit];
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _AP_SCM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }

    profile_num = SOC_MEM_SIZE(unit, SERVICE_PORT_MAPm) /
                  _AP_SPM_ENTRIES_PER_SET;
    profile_mem = _bcm_ap_service_port_map_profile[unit];
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _AP_SPM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }
    
    profile_num = SOC_MEM_SIZE(unit, VOQ_PORT_MAPm) /
                  _AP_SPM_ENTRIES_PER_SET;
    profile_mem = _bcm_ap_voq_port_map_profile[unit];
    for (ii = 0; ii < profile_num; ii++) {
        ref_count = 0;
        rv = soc_profile_mem_ref_count_get(
                 unit, profile_mem, ii * _AP_VPM_ENTRIES_PER_SET, &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }
    u32_scache_ptr = (uint32 *)u16_scache_ptr;
    
    *u32_scache_ptr++ = NUM_COS(unit);
    scache_ptr = (uint8*)u32_scache_ptr;

    /*
     * BCM_WB_VERSION_1_1 sync endpoint info.
     */
    if (soc_feature(unit, soc_feature_endpoint_queuing)) {
        scache_ptr = (uint8 *)u32_scache_ptr;
        BCM_IF_ERROR_RETURN(
                bcm_ap_cosq_endpoint_sync(unit, &scache_ptr));

        u16_scache_ptr = (uint16 *)scache_ptr;
        /* BCM_WB_VERSION_1_2 sync ENPOINT_COS_MAP memory profile */
        profile_mem = _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile;
        profile_num = (SOC_MEM_SIZE(unit, ENDPOINT_COS_MAPm) / _AP_NUM_INTERNAL_PRI);
        for (ii = 0; ii < profile_num; ii++) {
            rv = soc_profile_mem_ref_count_get(unit, profile_mem,
                                               ii * _AP_NUM_INTERNAL_PRI, &ref_count);
            if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
                return rv;
            }
            *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
        }
    }
    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_ap_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_ap_cosq_sw_dump(int unit)
{

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *     _bcm_ap_cosq_localport_resolve
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
_bcm_ap_cosq_localport_resolve(int unit, bcm_gport_t gport,
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
_bcm_ap_cosq_port_has_ets(int unit, bcm_port_t port)
{
/* In Apache it is always ETS mode */
    return TRUE;
}

/*
 * Function:
 *     _bcm_ap_node_index_get
 * Purpose:
 *     Allocate free index from the given list
 * Parameters:
 *     list       - (IN) bit list
 *     start      - (IN) start index
 *     end        - (IN) end index
 *     count       - (IN) size of queue set
 *     align      - (IN) adjusting the bits
 *     id         - (OUT) start index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_ap_node_index_get(SHR_BITDCL *list, int start, int end,
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
        /* Make sure the j doesn not cross the end */
        for (j = i; ((j < (i + count)) && ( j < end)); j++) {
            if (SHR_BITGET(list, j) != 0) {
                range_empty = 0;
                break;
            }
        }

        if (range_empty) {
            *id = i;
            return BCM_E_NONE;
        }
    }
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     _bcm_ap_node_index_set
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
_bcm_ap_node_index_set(_bcm_ap_cosq_list_t *list, int start, int range)
{
   if ( range == 0) 
   {    
        return BCM_E_NONE;
   }  
   if ((start < 0) ||
        (range < 0) ||
        ((start + range) > list->count)) {
        return BCM_E_RESOURCE;
   }

   SHR_BITSET_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_node_index_clear
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
_bcm_ap_node_index_clear(_bcm_ap_cosq_list_t *list, int start, int range)
{    
    if ( range == 0) 
    {    
        return BCM_E_NONE;
    }  
    if ((start < 0) ||
        (range < 0) ||
        ((start + range) > list->count)) {
        return BCM_E_RESOURCE;
    }

    SHR_BITCLR_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}
STATIC int
_bcm_ap_cosq_max_nodes_get(int unit, soc_apache_node_lvl_e level,
                           int *id)
{
    _bcm_ap_mmu_info_t *mmu_info;     
    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }
    if (id == NULL) {
        return BCM_E_PARAM;
    }
    if (level == SOC_APACHE_NODE_LVL_S1) {
        *id = _BCM_AP_NUM_S1_SCHEDULER_PER_PIPE;
    } else if (level == SOC_APACHE_NODE_LVL_L0) {
        *id = _BCM_AP_NUM_L0_SCHEDULER_PER_PIPE;
    } else if (level == SOC_APACHE_NODE_LVL_L1) {
        *id = _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE;
    } else if (level == SOC_APACHE_NODE_LVL_L2) {
        *id = mmu_info->num_l2_queues + _BCM_AP_NUM_L2_MC_LEAVES;
    }
    return BCM_E_NONE;

}
/*
 * Function:
 *     _bcm_ap_cosq_alloc_clear
 * Purpose:
 *     Allocate cosq memory and clear
 * Parameters:
 *     size       - (IN) size of memory required
 *     description- (IN) description about the structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC void *
_bcm_ap_cosq_alloc_clear(void *cp, unsigned int size, char *description)
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
 *     _bcm_ap_cosq_free_memory
 * Purpose:
 *     Free memory allocated for mmu info members
 * Parameters:
 *     mmu_info       - (IN) MMU info pointer
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_ap_cosq_free_memory(_bcm_ap_mmu_info_t *mmu_info_p)
{
    int i;
    _bcm_ap_pipe_resources_t *res;


    if (mmu_info_p == NULL) {
        return;
    }

    for (i = _AP_XPIPE; i < _AP_YPIPE; i++) {
        res = _BCM_AP_PRESOURCES(mmu_info_p, i);
        if (res->l2_queue_list.bits != NULL) {
            sal_free(res->l2_queue_list.bits);
            res->l2_queue_list.bits = NULL;
        }
        if (res->l1_sched_list.bits != NULL) {
            sal_free(res->l1_sched_list.bits);
            res->l1_sched_list.bits = NULL;
        }
        if (res->l0_sched_list.bits != NULL) {
            sal_free(res->l0_sched_list.bits);
            res->l0_sched_list.bits = NULL;
        }
        if (res->cpu_l0_sched_list.bits != NULL) {
            sal_free(res->cpu_l0_sched_list.bits);
            res->cpu_l0_sched_list.bits = NULL;
        }
        if (res->hsp_l2_queue_list.bits != NULL) {
            sal_free(res->hsp_l2_queue_list.bits);
            res->hsp_l2_queue_list.bits = NULL;
        }
        if (res->hsp_l1_sched_list.bits != NULL) {
            sal_free(res->hsp_l1_sched_list.bits);
            res->hsp_l1_sched_list.bits = NULL;
        }
        if (res->hsp_l0_sched_list.bits != NULL) {
            sal_free(res->hsp_l0_sched_list.bits);
            res->hsp_l0_sched_list.bits = NULL;
        }
        if (res->s1_sched_list.bits != NULL) {
            sal_free(res->s1_sched_list.bits);
            res->s1_sched_list.bits = NULL;
        }
    }

    for (i = 0; i < _BCM_AP_NUM_TOTAL_SCHEDULERS; i++) {
        if (((&mmu_info_p->sched_node[i])->cosq_map) != NULL) {
            sal_free((&mmu_info_p->sched_node[i])->cosq_map);
            (&mmu_info_p->sched_node[i])->cosq_map = NULL;
        }
    }

    for (i = 0; i < _BCM_AP_NUM_L2_UC_LEAVES; i++) {
        if (((&mmu_info_p->queue_node[i])->cosq_map) != NULL) {
            sal_free((&mmu_info_p->queue_node[i])->cosq_map);
            (&mmu_info_p->queue_node[i])->cosq_map = NULL;
        }
    }

    for (i = 0; i < _BCM_AP_NUM_L2_MC_LEAVES; i++) {
        if (((&mmu_info_p->mc_queue_node[i])->cosq_map) != NULL) {
            sal_free((&mmu_info_p->mc_queue_node[i])->cosq_map);
            (&mmu_info_p->mc_queue_node[i])->cosq_map = NULL;
        }
    }
}

/*
 * Function:
 *     _bcm_ap_cosq_node_get
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
_bcm_ap_cosq_node_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_module_t *modid, bcm_port_t *port, 
                       int *id, _bcm_ap_cosq_node_t **node)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node_base = NULL;
    bcm_module_t modid_out = 0;
    bcm_port_t port_out = 0;
    uint32 encap_id = -1;
    int index;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
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
#if 0
    if (!_bcm_ap_cosq_port_has_ets(unit, port_out)) {
        return BCM_E_NOT_FOUND;
    }
#endif

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
            _bcm_ap_cosq_node_t *tmp_node;
            tmp_node = *node;
            if (tmp_node->type == _BCM_AP_NODE_SERVICE_UCAST) {
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
_bcm_ap_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum)
{
    _bcm_ap_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, modid, port, id, &node));
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
 *     _bcm_ap_cosq_gport_traverse
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
_bcm_ap_cosq_gport_traverse(int unit, bcm_gport_t gport,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    _bcm_ap_cosq_node_t *node;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    bcm_port_t port, port_out;
    bcm_module_t modid, modid_out;
    bcm_gport_t gport_out;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, &modid, &port, NULL, &node));

    if (node != NULL) {
        if (!soc_feature(unit, soc_feature_mmu_hqos_four_level) && 
            node->level == SOC_APACHE_NODE_LVL_ROOT) 
        {
            node = node->child;
            if (!node) 
            { 
                return BCM_E_NONE; 
            }
        }
        if (node->level == SOC_APACHE_NODE_LVL_L2) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
                flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
            }
        } else {
            flags = BCM_COSQ_GPORT_SCHEDULER;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port,
                                    &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(gport_out, modid_out, port_out);

        rv = cb(unit, gport_out, node->numq, flags,
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
        _bcm_ap_cosq_gport_traverse(unit, node->sibling->gport, cb, user_data);
    }

    if (node->child != NULL) {
        _bcm_ap_cosq_gport_traverse(unit, node->child->gport, cb, user_data);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_child_node_at_input(_bcm_ap_cosq_node_t *node, int cosq,
                                  _bcm_ap_cosq_node_t **child_node)
{
    _bcm_ap_cosq_node_t *cur_node;

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
_bcm_ap_cosq_l2_gport(int unit, int port, int cosq,
                       _bcm_ap_node_type_e type, bcm_gport_t *gport,
                       _bcm_ap_cosq_node_t **p_node)
{
    int i, max;
    _bcm_ap_cosq_node_t *node, *fnode;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_pipe_resources_t *res;

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[port];
    res = port_info->resources;
    
    max  = 0;
    if ((type == _BCM_AP_NODE_UCAST) || (type == _BCM_AP_NODE_VOQ) ||
        (type == _BCM_AP_NODE_VLAN_UCAST) || (type == _BCM_AP_NODE_VM_UCAST) ||
         (type == _BCM_AP_NODE_SERVICE_UCAST)) {
        max = _BCM_AP_NUM_L2_UC_LEAVES;
        fnode = &res->p_queue_node[0];
    } else if (type == _BCM_AP_NODE_MCAST) {
        max = _BCM_AP_NUM_L2_MC_LEAVES;
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
 *     _bcm_ap_cosq_index_resolve
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
_bcm_ap_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq, 
                           _bcm_ap_cosq_index_style_t style,                 
                           bcm_port_t *local_port, int *index, int *count)
{
    _bcm_ap_cosq_node_t *node, *cur_node , *s1_node;
    bcm_port_t resolved_port;
    int resolved_index = -1;
    int id, startq, numq;
    soc_info_t *si;
    int phy_port, mmu_port;
    uint32 entry0[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    _bcm_ap_mmu_info_t *mmu_info;
    si = &SOC_INFO(unit);

    mmu_info = _bcm_ap_mmu_info[unit];
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
            (_bcm_ap_cosq_node_get(unit, port, cosq, NULL, &resolved_port, &id,
                                   &node));
        if (node->attached_to_input < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        numq = (node->numq != -1) ? node->numq : 64;

    } else if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
         BCM_IF_ERROR_RETURN(
        _bcmi_coe_subport_physical_port_get(unit, port, &resolved_port));
        BCM_IF_ERROR_RETURN
             (_bcm_ap_get_s1_node(unit, port, &s1_node)); 
        node =  &mmu_info->queue_node[s1_node->coe_base_index + startq ]; 
        if (node->attached_to_input < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        numq =  _AP_NUM_COS(unit);
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, port, &resolved_port));
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

    switch (style) {
    case _BCM_AP_COSQ_INDEX_STYLE_BUCKET:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port) || BCM_GPORT_IS_MODPORT(port)) {
                /* resolve the child attached to input cosq */
                BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, startq, &cur_node));
                resolved_index = cur_node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port)) {
                resolved_index = SOC_INFO(unit).port_cosq_base[resolved_port] +
                                 startq;
                resolved_index = soc_apache_l2_hw_index(unit, resolved_index, 0);
            } else {
                resolved_index = soc_apache_l2_hw_index(unit,
                        si->port_uc_cosq_base[resolved_port] + startq, 1);
                if (!IS_AP_HSP_PORT(unit, resolved_port)) { 
                     mem = _SOC_APACHE_NODE_PARENT_MEM(unit, port, SOC_APACHE_NODE_LVL_L2);
                     SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index, &entry0));
                     resolved_index = soc_mem_field32_get(unit, mem, entry0, C_PARENTf);
                }
            }
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_QCN:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            if (IS_CPU_PORT(unit, resolved_port)) {
                return BCM_E_PARAM;
            } else {
                resolved_index = soc_apache_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
            }
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_WRED:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index + startq;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, startq, &cur_node));
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cur_node->gport)) {
                    resolved_index = cur_node->hw_index;
                } else {
                    return BCM_E_PARAM;
                }
            } else if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
                resolved_index = node->hw_index ;
            } else if (BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN(_bcm_ap_cosq_l2_gport(unit, node->local_port, 
                                   startq, _BCM_AP_NODE_UCAST, NULL, &cur_node));
                resolved_index = cur_node->hw_index  ;
            } else {
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            resolved_index = soc_apache_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
            numq = 1;
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_WRED_PORT:
        resolved_index = _BCM_PORT_SERVICE_POOL + (mmu_port * 4);
        numq = 4;
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_WRED_POOL:
        if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                /* regular unicast queue */
                resolved_index = soc_apache_l2_hw_index(unit, node->hw_index + startq, 1);
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
        mem =  MMU_THDU_XPIPE_Q_TO_QGRP_MAPm; 
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                    entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);

        break;

    case _BCM_AP_COSQ_INDEX_STYLE_SCHEDULER:
        if (node != NULL) {
            if (!BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_ap_cosq_child_node_at_input(node, startq, &cur_node));
            resolved_index = cur_node->hw_index;
        } else { /* node == NULL */
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            if (IS_CPU_PORT(unit, resolved_port)) {
                resolved_index = soc_apache_l2_hw_index(unit,
                si->port_cosq_base[resolved_port] + startq, 0);
            } else {
                resolved_index = soc_apache_l2_hw_index(unit,
                        si->port_uc_cosq_base[resolved_port] + startq, 1);
            }
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_COS:
        if (node) {
            if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) ||
                (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))) {
                resolved_index = node->hw_cosq;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, startq, &cur_node));
                if (BCM_GPORT_IS_SCHEDULER(cur_node->gport)) {
                    return BCM_E_PARAM;
                }
                resolved_index = cur_node->hw_cosq;
            } else if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
                resolved_index = node->hw_cosq ;
            }
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            resolved_index = startq;
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE:
    case _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP:
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
                    (style == _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = soc_apache_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
        }
        if (style == _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP) {
            mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                        resolved_index, entry0));
            if (soc_mem_field32_get(unit, mem, entry0, QGROUP_VALIDf)) {
                resolved_index = soc_mem_field32_get(unit, mem, entry0, QGROUPf);
            } else {
                return BCM_E_NOT_FOUND;
            }
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE:
        if (node) {
            resolved_index = node->hw_index + startq;
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = soc_apache_l2_hw_index(unit, 
                si->port_cosq_base[resolved_port] + startq, 0);
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL:
        if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    /* regular unicast queue */
                resolved_index = soc_apache_l2_hw_index(unit, node->hw_index + startq, 1);
                mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = soc_apache_l2_hw_index(unit,
                    si->port_cosq_base[resolved_port] + startq, 0) -
                     _BCM_AP_NUM_L2_UC_LEAVES;
                mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
            } else { /* scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = soc_apache_l2_hw_index(unit, 
	                    si->port_cosq_base[resolved_port] + startq, 0) - 
                            _BCM_AP_NUM_L2_UC_LEAVES ; 
            mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                            entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);

        break;
     case _BCM_AP_COSQ_INDEX_STYLE_UC_EGR_POOL:
        if (IS_CPU_PORT(unit, resolved_port) ||
            IS_LB_PORT(unit, resolved_port)) {
            return BCM_E_PARAM;
        }

        numq = si->port_num_uc_cosq[resolved_port];
        if (startq >= numq) {
            return BCM_E_PARAM;
        }

        resolved_index = soc_apache_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port] + startq, 1);
        mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                            entry0));

        resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST:
        if (node) {
            resolved_index = node->hw_index + startq;
            resolved_index -= soc_apache_l2_hw_index(unit,
                si->port_uc_cosq_base[resolved_port], 1);
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
            numq = 1;
            resolved_index = startq;
        }
        break;

    case _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST:
        if (node) {
            resolved_index = node->hw_index + startq;
            resolved_index -= soc_apache_l2_hw_index(unit,
                si->port_cosq_base[resolved_port], 0);
            numq = 1;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, resolved_port));
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

STATIC int _bcm_ap_cosq_min_child_index(_bcm_ap_cosq_node_t *child, int lvl, int uc)
{
    int min_index = -1;

    if ((lvl == SOC_APACHE_NODE_LVL_L2) && (uc)) {
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
    } else if ((lvl == SOC_APACHE_NODE_LVL_L2) && (!uc)) {
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
_bcm_ap_child_state_check(int unit, bcm_port_t gport,
                           bcm_cos_queue_t cosq, int ets_mode)
{
    _bcm_ap_cosq_node_t *node, *child_node = NULL;
    bcm_port_t local_port;
    int level, sch_index, empty = 0;
    int rv = BCM_E_NONE;
    soc_timeout_t timeout;
    uint32 timeout_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    static const soc_mem_t memx[] = {
        INVALIDm,
        LLS_L0_CHILD_STATE1m,
        LLS_L1_CHILD_STATE1m,
        LLS_L2_CHILD_STATE1m
    };

    /* Timeout val = Static for now. Need to quantify the max wait time */
    timeout_val = 2000000;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    if (ets_mode & (!IS_AP_HSP_PORT(unit, local_port))) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, gport, 0, NULL,
                                    &local_port, NULL, &node));

        /* Get the Child node for the Given Gport */
        rv = _bcm_ap_cosq_child_node_at_input(node, cosq, &child_node);

        if ((!child_node) || (rv == BCM_E_NOT_FOUND)) {
            /* No Child present */
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

        level = child_node->level;
        sch_index = child_node->hw_index;

        /* Child State Check is NOT necessary for Non-L0/L1/L2 levels */
        if ((level < SOC_APACHE_NODE_LVL_L0) || (level > SOC_APACHE_NODE_LVL_L2)) {
            /* Sanity for Node's level should be done in the caller */
            return BCM_E_NONE;
        }

        mem = memx[level]; 

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
            empty += soc_mem_field32_get(unit, mem, &entry, C_ACTIVE_0f);
            empty += soc_mem_field32_get(unit, mem, &entry, C_ON_MIN_LIST_0f);
            empty += soc_mem_field32_get(unit, mem, &entry, C_NOT_EMPTY_0f);

            if (!empty) {
                break;
            }
        }
    } else {
        /* TBD: Non-ETS mode not supported. May not be possible to support the same on HSP queues */
        return BCM_E_NONE;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_node_queue_state_check(int unit, 
        _bcm_ap_cosq_node_t *node, int ets_mode)
{
    int cosq = 0;
    
    if (node == NULL) {
        return BCM_E_PARAM;
    }

    for (cosq = 0; cosq < node->numq; cosq++) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_child_state_check(unit, node->gport,
                                          cosq, ets_mode));
    }

    if ((node->level <= SOC_APACHE_NODE_LVL_L1) &&
        (node->child != NULL)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_queue_state_check(unit,
                                 node->child,ets_mode));
    }
        
    if (node->sibling != NULL) {

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_queue_state_check(unit,
                               node->sibling,ets_mode));
    }

    return BCM_E_NONE;
}

int 
_bcm_ap_cosq_port_queue_state_check(int unit, bcm_port_t gport)
{
    bcm_port_t local_port;
    int   ets_mode;
    _bcm_ap_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
    ets_mode = _bcm_ap_cosq_port_has_ets(unit, local_port);
    if (!ets_mode) {
        return BCM_E_NONE;
    }
    
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_gport_get(unit, local_port,
                                         &gport));

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
                                  NULL, NULL, &node));
    
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_queue_state_check(unit,
                                    node,ets_mode));
    return BCM_E_NONE;
}


/*
 * Function:
 * _bcm_ap_mmu_rx_enable_set
 * Purpose:
 *     enable/disable mmu rx traffic from the gport.
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     enable     - (IN) enable/disable
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_ap_mmu_rx_enable_set(int unit, bcm_port_t local_port,
                           int enable)
{
    uint32 rval;

    /* Disable/Enable INPUT_PORT_RX to the port */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, THDI_INPUT_PORT_XON_ENABLESr,
                       local_port, 0, &rval));

    if (!enable) {
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                INPUT_PORT_RX_ENABLEf,
                0);
    } else {
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                INPUT_PORT_RX_ENABLEf,
                1);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, THDI_INPUT_PORT_XON_ENABLESr,
                      local_port, 0, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_attach_node_in_hw(int unit, _bcm_ap_cosq_node_t *node)
{
    int local_port, fc = 0, fmc = 0;
    _bcm_ap_cosq_node_t *child_node;
    int num_spri = 0, first_sp_child = 0, first_sp_mc_child;
    uint32 ucmap = 0, spmap = 0;
    soc_apache_sched_type_t sched_type;

    local_port = node->local_port;
 
    if (node->level == SOC_APACHE_NODE_LVL_ROOT) {
        return 0;
    } else if (node->level == SOC_APACHE_NODE_LVL_L2) {
        fc = _bcm_ap_cosq_min_child_index(node->parent->child, SOC_APACHE_NODE_LVL_L2, 1);
        if (fc < 0) {
            fc = 0;
        }
        fmc = _bcm_ap_cosq_min_child_index(node->parent->child, SOC_APACHE_NODE_LVL_L2, 0);
        if (fmc < 0) {
            fmc = _BCM_AP_NUM_L2_UC_LEAVES;
        }
    } else {
        fc = _bcm_ap_cosq_min_child_index(node->parent->child, node->level, 0);
        fmc = 0;
    }

    /* If node is scheduler, reset the scheduler and attach it to the parent.*/
    BCM_IF_ERROR_RETURN(
        soc_apache_cosq_set_sched_parent(unit, local_port, node->level, 
                                      node->hw_index, node->parent->hw_index, 1));

    sched_type = _soc_apache_port_sched_type_get(unit, local_port);

    if (sched_type == SOC_APACHE_SCHED_LLS) {
        if (node->parent->level != SOC_APACHE_NODE_LVL_ROOT) { 
            BCM_IF_ERROR_RETURN(
                    soc_apache_cosq_get_sched_child_config(unit, local_port,
                        node->parent->level,
                        node->parent->hw_index,
                        &num_spri, &first_sp_child,
                        &first_sp_mc_child, &ucmap, &spmap));
        }
        /* set the node by default in WRR mode with wt=1 */
        BCM_IF_ERROR_RETURN(soc_apache_cosq_set_sched_config(unit,
                        local_port, node->parent->level,
                        node->parent->hw_index, node->hw_index,
                        num_spri, fc, fmc, ucmap, spmap,
                        SOC_APACHE_SCHED_MODE_WRR, 1));
    } 


    if (node->child) {
        child_node = node->child;
        while (child_node) {
            BCM_IF_ERROR_RETURN(_bcm_ap_attach_node_in_hw(unit, child_node));
            child_node = child_node->sibling;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_ap_cosq_map_index_get
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
_bcm_ap_cosq_map_index_get(SHR_BITDCL *list, int start, int end, int *id)
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
 *     _bcm_ap_cosq_map_index_clear
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
_bcm_ap_cosq_map_index_clear(SHR_BITDCL *list, int start, int range)
{
    SHR_BITCLR_RANGE(list, start, range);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_map_index_is_set
 * Purpose:
 *     Check index is allocated
 * Parameters:
 *     list       - (IN) bit list
 *     index      - (IN) index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_ap_cosq_map_index_is_set(SHR_BITDCL *list, int index)
{
    if (SHR_BITGET(list, index) != 0) {
        return TRUE;
    }

    return FALSE;
}
/*
 * Function:
 *     _bcm_ap_cosq_node_resolve
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
_bcm_ap_cosq_node_resolve(int unit, _bcm_ap_cosq_node_t *node,
                          bcm_cos_queue_t cosq)
{
    _bcm_ap_cosq_node_t *cur_node, *parent;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_pipe_resources_t * res;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_list_t *list;
    int numq, id = -1 , prev_base = -1, rv;
    int valid_index_lo, valid_index_hi;
    bcm_port_t port;
    int alloc_size;
    int phy_port, mmu_port;
    int max_nodes = 0;
    uint32 rval = 0;
    uint32 mc_group_mode = 0;

    if ((parent = node->parent) == NULL) {
        /* Not attached, should never happen */
        return BCM_E_NOT_FOUND;
    }

    if (parent->numq == 0 || parent->numq < -1) {
        return BCM_E_PARAM;
    }

    port = node->local_port;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_max_nodes_get(unit, node->level, &max_nodes));
    if (parent->cosq_map == NULL) {
        if (parent->numq == -1) {
            alloc_size = SHR_BITALLOCSIZE(max_nodes);
        } else {
            alloc_size = SHR_BITALLOCSIZE(parent->numq);
        }
        parent->cosq_map = _bcm_ap_cosq_alloc_clear(parent->cosq_map,
                alloc_size,
                "cosq_map");
        if (parent->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }
   if (cosq < 0) {
       BCM_IF_ERROR_RETURN
           (_bcm_ap_cosq_map_index_get(parent->cosq_map,
                                       0, 
                                       parent->numq == -1 ?
                                       max_nodes: parent->numq, &id));
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_map_index_set(parent->cosq_map, id, 1));
        node->attached_to_input = id;
    } else {
        if (_bcm_ap_cosq_map_index_is_set(parent->cosq_map,
                    cosq) == TRUE) {
            return BCM_E_EXISTS;
        }
        if (cosq > ((parent->numq == -1) ? max_nodes : parent->numq)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_map_index_set(parent->cosq_map, cosq, 1));
        node->attached_to_input = cosq;
    } 

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[port];
    res = port_info->resources;
    numq = (parent->numq == -1) ? 1 : parent->numq;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    switch (parent->level) {
        case SOC_APACHE_NODE_LVL_ROOT:
            list = &res->s1_sched_list;
            if (IS_AP_HSP_PORT(unit, port)) {
                list = &res->hsp_l0_sched_list;
            }
            if (parent->numq_expandable && parent->base_index >= 0 &&
                    parent->base_size != parent->numq) {
                _bcm_ap_node_index_clear(list, parent->base_index, 
                        parent->base_size);
                prev_base = parent->base_index;
                parent->base_index =  -1;
            }
            if (IS_AP_HSP_PORT(unit, port)) {
                valid_index_lo = _BCM_AP_HSP_L0_INDEX(unit, mmu_port, 0);
                valid_index_hi = _BCM_AP_HSP_L0_INDEX(unit, (mmu_port + 1),
                        0);
            } else {
                valid_index_lo = 0;
                valid_index_hi = _BCM_AP_NUM_S1_SCHEDULER_PER_PIPE;
            }

            if (((parent->base_index < 0) && (node->attached_to_input < numq))
                        || (parent->numq == -1)) {
               rv = _bcm_ap_node_index_get(list->bits, valid_index_lo, 
                        valid_index_hi, numq, 1, &id);
                if (rv) {
                    return rv;
                }

                _bcm_ap_node_index_set(list, id, numq);
                if (parent->numq != -1) {
                    parent->base_index = id;
                    node->hw_index = parent->base_index + node->attached_to_input;
                    parent->base_size = numq;
                    if ((prev_base >= 0) && (prev_base != id)) {
                        for (cur_node = parent->child; cur_node; 
                                cur_node = cur_node->sibling) {
                            if (cur_node->attached_to_input >= 0) {
                                cur_node->hw_index = parent->base_index + 
                                    cur_node->attached_to_input;
                                BCM_IF_ERROR_RETURN(_bcm_ap_attach_node_in_hw(unit, cur_node));
                            }
                        }
                    }
                } else {
                    node->hw_index = id;
                }
            } else if (node->attached_to_input >= numq) {
                rv = _bcm_ap_node_index_get(list->bits, valid_index_lo, 
                        valid_index_hi, 1, 1, &id);
                if (rv) {
                    return rv;
                }
                _bcm_ap_node_index_set(list, id, 1);
                node->hw_index = id;

            } else {
                node->hw_index = parent->base_index + node->attached_to_input;
            }
            node->hw_cosq = node->attached_to_input;
            node->wrr_in_use = 1;

            break;

        case SOC_APACHE_NODE_LVL_S1:
            if (IS_AP_HSP_PORT(unit, port)) {
                return BCM_E_PARAM;
            }
            list = &res->l0_sched_list;
            if (parent->numq_expandable && parent->base_index >= 0 &&
                    parent->base_size != parent->numq) {
                _bcm_ap_node_index_clear(list, parent->base_index, 
                        parent->base_size);
                prev_base = parent->base_index;
                parent->base_index =  -1;
            }
            valid_index_lo = 0;
            valid_index_hi = _BCM_AP_NUM_L0_SCHEDULER_PER_PIPE;
            if (IS_CPU_PORT(unit, port)) {
                list = &res->cpu_l0_sched_list;
                valid_index_lo = 0;
                valid_index_hi = _BCM_AP_CPU_PORT_L0_MAX - _BCM_AP_CPU_PORT_L0_MIN;
            }

            if (((parent->base_index < 0) && (node->attached_to_input < numq))
                        || (parent->numq == -1)) {
                if (_BCM_DEFAULT_LLS_IS_SET(mmu_info)) {
                    rv = _bcm_ap_node_index_get(list->bits, valid_index_lo, 
                            valid_index_hi, numq, _BCM_AP_START_INDEX_ALIGN_PFC,
                            &id);
                } else if (node->node_align_value) {
                    rv = _bcm_ap_node_index_get(list->bits, valid_index_lo,
                            valid_index_hi, numq, node->node_align_value,
                            &id);
                } else {
                    rv = _bcm_ap_node_index_get(list->bits, valid_index_lo, 
                            valid_index_hi, numq, _BCM_AP_START_INDEX_ALIGN_DEFAULT,
                            &id);
                }
                if (rv) {
                    return rv;
                }

                _bcm_ap_node_index_set(list, id, numq);
                if (IS_CPU_PORT(unit, port)) {
                    id += _BCM_AP_CPU_PORT_L0_MIN; 
                }
                if (parent->numq != -1) {
                    parent->base_index = id;
                    node->hw_index = parent->base_index + node->attached_to_input;
                    parent->base_size = numq;
                    if ((prev_base >= 0) && (prev_base != id)) {
                        for (cur_node = parent->child; cur_node; 
                                cur_node = cur_node->sibling) {
                            if (cur_node->attached_to_input >= 0) {
                                cur_node->hw_index = parent->base_index + 
                                    cur_node->attached_to_input;
                                BCM_IF_ERROR_RETURN(_bcm_ap_attach_node_in_hw(unit, cur_node));
                            }
                        }
                    }
                } else {
                    node->hw_index = id;
                }
            } else if (node->attached_to_input >= numq) {
                rv = _bcm_ap_node_index_get(list->bits, valid_index_lo, 
                        valid_index_hi, 1, 1, &id);
                if (rv) {
                    return rv;
                }
                _bcm_ap_node_index_set(list, id, 1);
                node->hw_index = id;
            } else {
                node->hw_index = parent->base_index + node->attached_to_input;
            }
            node->hw_cosq = node->attached_to_input;

            break;


        case SOC_APACHE_NODE_LVL_L0:
            if (IS_AP_HSP_PORT(unit, port)) {
                if (node->attached_to_input >= _BCM_AP_HSP_PORT_MAX_L1) {
                    return BCM_E_PARAM;
                }
                
                if ((parent->hw_index % _BCM_AP_HSP_PORT_MAX_L0) == 0) {
                    /* L0.0 could only be attached by MC nodes */
                    if (node->type == _BCM_AP_NODE_MCAST) {
                        SOC_IF_ERROR_RETURN(
                            READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
                        mc_group_mode  = soc_reg_field_get(
                                            unit, HSP_SCHED_PORT_CONFIGr, 
                                            rval, MC_GROUP_MODEf);
                        /* check mc group mode and the node offset */
                        if ((!mc_group_mode) ||
                            (((node->hw_index -_BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE) % _BCM_AP_HSP_PORT_MAX_L1) >=
                             _BCM_AP_HSP_PORT_MAX_COS)) {
                            return BCM_E_PARAM;
                        }
                        break;
                    } else {
                        return BCM_E_PARAM;
                    }
                }

                list = &res->hsp_l1_sched_list;
                valid_index_lo = _BCM_AP_HSP_L1_INDEX(unit, mmu_port, 0);
                valid_index_hi = _BCM_AP_HSP_L1_INDEX(unit, (mmu_port + 1),
                        0);
                BCM_IF_ERROR_RETURN(_bcm_ap_node_index_get(list->bits, valid_index_lo, 
                            valid_index_hi, 1, 1, &id));
                if (parent->base_index == -1) {
                    parent->base_index = id;
                }
                node->hw_index = id;

                if ((node->hw_index % _BCM_AP_HSP_PORT_MAX_L1) >= 8) {
                    if ((parent->hw_index % _BCM_AP_HSP_PORT_MAX_L0) != 4) {
                        return BCM_E_PARAM;
                    }
                } else {
                    if ((parent->hw_index % _BCM_AP_HSP_PORT_MAX_L0) == 4) {
                        return BCM_E_PARAM;
                    }
                }
                _bcm_ap_node_index_set(list, id, 1);
            } else {
                list = &res->l1_sched_list;
                if (((parent->base_index < 0) && (node->attached_to_input < numq))
                        || (parent->numq == -1)) {
                    if (_BCM_DEFAULT_LLS_IS_SET(mmu_info)) { 
                        BCM_IF_ERROR_RETURN(_bcm_ap_node_index_get(list->bits, 0, 
                                    _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE, numq,
                                    _BCM_AP_START_INDEX_ALIGN_PFC, &id));
                    } else if (node->node_align_value) {
                        BCM_IF_ERROR_RETURN(_bcm_ap_node_index_get(list->bits, 0,
                                    _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE, numq,
                                    node->node_align_value, &id));
                    } else {
                        BCM_IF_ERROR_RETURN(_bcm_ap_node_index_get(list->bits, 0, 
                                    _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE, numq,
                                    _BCM_AP_START_INDEX_ALIGN_DEFAULT, &id));
                    }

                    _bcm_ap_node_index_set(list, id, numq);
                    parent->base_size = numq;
                    if (parent->base_index == -1) {
                        parent->base_index = id;
                    }
                    if (parent->numq != -1) {
                        node->hw_index = parent->base_index + node->attached_to_input;
                    } else {
                        node->hw_index = id;
                    }
                } else if (node->attached_to_input >= numq) {

                    BCM_IF_ERROR_RETURN(_bcm_ap_node_index_get(list->bits, 0, 
                            _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE, 1, 1, &id));
                    _bcm_ap_node_index_set(list, id, 1);
                    node->hw_index = id;
                } else {
                    node->hw_index = parent->base_index + node->attached_to_input;
                }
            }
            node->hw_cosq = node->attached_to_input;

            break;

        case SOC_APACHE_NODE_LVL_L1:
            if (IS_AP_HSP_PORT(unit, port)) {
                uint32     rval = 0;
                uint32     mc_group_mode = 0;
                int        qnum;
                SOC_IF_ERROR_RETURN(
                    READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
                mc_group_mode  = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                                   rval, MC_GROUP_MODEf);
                qnum = (node->hw_index > _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE) ?
                       (node->hw_index - _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE) : node->hw_index;
                if (mc_group_mode &&
                    ((qnum % _BCM_AP_HSP_PORT_MAX_L1) < _BCM_AP_HSP_PORT_MAX_COS)) {
                    if (node->attached_to_input >= 1) {
                        return BCM_E_PARAM;
                    }
                    if (node->type == _BCM_AP_NODE_MCAST) {
                        return BCM_E_PARAM;
                    }
                } else {
                    if (node->attached_to_input >= _BCM_AP_HSP_PORT_MAX_L2_PER_L1) {
                        return BCM_E_PARAM;
                    }
                    if ((node->sibling) &&
                        (node->type == node->sibling->type)) {
                        return BCM_E_PARAM;
                    }
                }
            }
            if ((node->hw_index == -1) && 
                    (node->type != _BCM_AP_NODE_VOQ)) {
                return BCM_E_PARAM;
            } else if (node->type == _BCM_AP_NODE_VOQ) {
                if (parent->base_index < 0) {
                    BCM_IF_ERROR_RETURN(
                            _bcm_ap_node_index_get(res->l2_queue_list.bits,
                                res->num_base_queues,
                               _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE  , 
                               _AP_NUM_COS(unit), 
                                _AP_NUM_COS(unit), &id));
                    /* The following logic is to make sure the id is always
                     *  multiple of 8 
                     */
                    while ( id % 8 != 0) {
                        id = ((id + 8) & 0xfffffff8);
                        if (id >  _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE ) {
                            return BCM_E_RESOURCE;
                        }
                        BCM_IF_ERROR_RETURN(
                                _bcm_ap_node_index_get(res->l2_queue_list.bits, id,
                                    _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE, 
                                    NUM_COS(unit), 
                                    NUM_COS(unit), &id));
                    }
                    _bcm_ap_node_index_set(&res->l2_queue_list, id, 
                            (parent->numq == -1) ? _AP_NUM_COS(unit) : parent->numq);
                    parent->base_index = id ;
                    node->hw_index = parent->base_index;
                    node->hw_cosq = node->hw_index %
                                ((parent->numq == -1) ? _AP_NUM_COS(unit) : parent->numq);
                } else {
                    node->hw_index = parent->base_index + 
                        node->attached_to_input;
                    node->hw_cosq = node->hw_index % NUM_COS(unit);
                }
            } else if((node->hw_index != -1) && (parent->base_index < 0)) {
                parent->base_index = node->hw_index;
            }
            break;

        case SOC_APACHE_NODE_LVL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    BCM_IF_ERROR_RETURN(_bcm_ap_attach_node_in_hw(unit, node));
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_detach_node_in_hw(int unit, _bcm_ap_cosq_node_t *node)
{
    int local_port;

    local_port = node->local_port;

    if (!IS_AP_HSP_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_sched_parent_child_set(unit,
                local_port, node->parent->level, node->parent->hw_index,
                node->hw_index, SOC_TD2_SCHED_MODE_WRR, 1, node));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_clear_child_list
 * Purpose:
 *     Reset the child list to 0. To be called when deleting the node.
 * Parameters:
 *     unit       - (IN) unit number
 *     node       - (IN) node structure for the specified scheduler node
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_ap_cosq_clear_child_list(int unit, _bcm_ap_cosq_node_t *node)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t * res;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_cosq_list_t *list;
    int start_index = 0, numq = 0;
    int port;

    mmu_info = _bcm_ap_mmu_info[unit];
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, node->gport, 0, NULL, &port, NULL, NULL));

    port_info = &mmu_info->port_info[port];
    res = port_info->resources;

    list = NULL;
    switch (node->level) {
        case SOC_APACHE_NODE_LVL_ROOT:
            if (IS_AP_HSP_PORT(unit, port)) {
                list = &res->hsp_l0_sched_list;
                start_index = node->base_index;
                numq = (node->numq == -1) ?
                        _BCM_AP_HSP_PORT_MAX_L0 : node->numq;
            }
            break;
        default:
            return BCM_E_NONE;
    }

    if(list){
        _bcm_ap_node_index_clear(list, start_index, numq);
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_ap_cosq_node_unresolve
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
_bcm_ap_cosq_node_unresolve(int unit, _bcm_ap_cosq_node_t *node, 
                             bcm_cos_queue_t cosq)
{
    _bcm_ap_cosq_node_t *parent;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t * res;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_cosq_list_t *list;
    int numq = 1, port;
    int hw_index;

    if (node->attached_to_input < 0) {
        /* Has been unresolved already */
        return BCM_E_NONE;
    }

    parent = node->parent;

    if (!parent) {
        return BCM_E_PARAM;
    }

    mmu_info = _bcm_ap_mmu_info[unit];

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, node->gport, 0, NULL, &port, NULL, NULL));

    port_info = &mmu_info->port_info[port];
    res = port_info->resources;

    list = NULL;

    switch (parent->level) {
        case SOC_APACHE_NODE_LVL_ROOT:
            if (IS_AP_HSP_PORT(unit, port)) {
                list = &res->hsp_l0_sched_list;
            } else {
                list = &res->s1_sched_list;
            }
            break;

        case SOC_APACHE_NODE_LVL_S1:
            if (IS_AP_HSP_PORT(unit, port)) {
                return BCM_E_PARAM;
            } else if (IS_CPU_PORT(unit, port)) {
                list = &res->cpu_l0_sched_list;
            } else {
                list = &res->l0_sched_list;
            }
            break;

        case SOC_APACHE_NODE_LVL_L0:
            if (IS_AP_HSP_PORT(unit, port)) {
                list = &res->hsp_l1_sched_list;
            } else {

                list = &res->l1_sched_list;
            }
            break;

        case SOC_APACHE_NODE_LVL_L1:
            break;

        case SOC_APACHE_NODE_LVL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    if ((list) && (node->attached_to_input >= 8)) {
        hw_index = node->hw_index;
        if (IS_CPU_PORT(unit, port)) {
            hw_index = node->hw_index - _BCM_AP_CPU_PORT_L0_MIN;
        }
        _bcm_ap_node_index_clear(list, hw_index, numq);
    } else if((list) && (IS_AP_HSP_PORT(unit, port))){
        _bcm_ap_node_index_clear(list, node->hw_index, 1);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_map_index_clear(node->parent->cosq_map,
                                       node->attached_to_input , 1));
    node->attached_to_input = -1;

    BCM_IF_ERROR_RETURN(_bcm_ap_detach_node_in_hw(unit, node));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_port_enqueue_set
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
_bcm_ap_port_enqueue_set(int unit, bcm_port_t gport, int enable)
{
    bcm_port_t local_port;
   
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    SOC_IF_ERROR_RETURN(
        soc_apache_port_mmu_tx_enable_set(unit, local_port, enable));
    SOC_IF_ERROR_RETURN(
        _bcm_ap_mmu_rx_enable_set(unit, local_port, enable));

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_ap_port_enqueue_get
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
_bcm_ap_port_enqueue_get(int unit, bcm_port_t gport,
                          int *enable)
{
    uint64 rval64;
    int mmu_port, phy_port, i, reg_grp = 0;
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
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    COMPILER_64_ZERO(rval64);
    if (mmu_port  < 64 ) {
        mmu_port &= 0x3f;
    } else {
        mmu_port &= 0xf;
        reg_grp = 1;
    }

    for (i = 0; i < max_reg; i++) {
        reg_tmp = reg[i][reg_grp];

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg_tmp, REG_PORT_ANY, 0, &rval64));

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
 *       _bcm_ap_get_s1_node  
 * Purpose:
 *       finds the s1 node for given subport 
 * Parameters:
 *     unit       - (IN) unit number
 *     subport      - (IN)gport of the subport 
 *     s1_node     - (OUT) pointer to the s1 node 
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_ap_get_s1_node(int unit, bcm_gport_t subport,
                      _bcm_ap_cosq_node_t **s1_node) 
{

   int local_port = 0;
   _bcm_ap_cosq_node_t *port_node;
   _bcm_ap_cosq_node_t *child_node;
   _bcm_ap_mmu_info_t *mmu_info;
   int s1_found = 0; 
   if (!s1_node)
   {
       return BCM_E_INTERNAL; 
   }
   if (!soc_feature(unit, 
                    soc_feature_mmu_hqos_four_level)) { 
        return BCM_E_PARAM; 
   }
   mmu_info = _bcm_ap_mmu_info[unit];
   if(!(BCM_GPORT_IS_SET(subport) &&
      _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, subport))) {
        return BCM_E_PARAM; 
   }
    BCM_IF_ERROR_RETURN
    (_bcmi_coe_subport_physical_port_get(unit, subport, &local_port));
    port_node = &mmu_info->sched_node[local_port];
    if (!port_node->in_use) { 
         return BCM_E_INTERNAL; 
    }
    child_node = port_node->child;
    while (child_node) { 
           if((child_node->port_gport == subport))
           {
               s1_found = 1 ; 
               break; 
           }
           child_node = child_node->sibling; 
    }
    if (!s1_found) 
    {
        return BCM_E_NOT_FOUND; 
    }
    *s1_node = child_node; 
    return BCM_E_NONE; 
}

/*
 * Function:
 *       _bcm_ap_allocate_coe_queues  
 * Purpose:
 *        allocates deallocated l2 queues for given subport 
 * Parameters:
 *     unit       - (IN) unit number
 *     s1_node      - (IN) subport s1 node  
 *     enable     - (IN) flag to enable/disable 
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_ap_allocate_coe_queues(int unit ,_bcm_ap_cosq_node_t *s1_node,
                           int enable)
{
    int id = 0;
    int pipe = 0 ;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_mmu_info_t *mmu_info;

    mmu_info = _bcm_ap_mmu_info[unit];
    pipe = 0;
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);

    if (enable) { 
    /* The following logic is to make sure the id is always
     *  multiple of 8 
     */
        BCM_IF_ERROR_RETURN(
        _bcm_ap_node_index_get(res->l2_queue_list.bits,
                               res->num_base_queues,
                               _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE  , 
                               _AP_NUM_COS(unit), 
                               _AP_NUM_COS(unit), &id));
        while (id % 8 != 0) {
               id = ((id + 8) & 0xfffffff8);
               if (id >  _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE ) {
                   return BCM_E_RESOURCE;
               }
               BCM_IF_ERROR_RETURN(
               _bcm_ap_node_index_get(res->l2_queue_list.bits, id,
                                      _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE, 
                                      NUM_COS(unit), 
                                      NUM_COS(unit), &id));
        }
         _bcm_ap_node_index_set(&res->l2_queue_list, id, 
                                _AP_NUM_COS(unit));
         s1_node->coe_base_index = id; 
    } else 
    {
         _bcm_ap_node_index_clear(&res->l2_queue_list, id, 
                                  _AP_NUM_COS(unit));
         s1_node->coe_base_index = -1; 
    }
    return BCM_E_NONE; 
}

/*
 * Function:
 *       _bcm_ap_coe_queue_offset_add  
 * Purpose:
 *       Programs the offset in voq_mod_map and voq_port_map table  
 * Parameters:
 *     unit       - (IN) unit number
 *     subport      - (IN) subport gport 
 *     queue_id     - (IN) base queue_id for the subport 
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_ap_coe_queue_offset_add(int unit, int queue_id,
                            bcm_gport_t subport)
{
    bcm_module_t modid = 0;
    bcm_port_t port  = 0;
    voq_port_map_entry_t *voq_port_map_entries = NULL;
    voq_mod_map_entry_t voq_mod_map;
    int rv = BCM_E_NONE;
    void *entries[1];
    int port_offset = 0 ;
    int vpm_idx_old = 0 ; 
    int baseq_old = 0 ; 
    uint32 profile = 0 ;
    bcm_trunk_t trunk_id;
    int id;
    int port_base = 0;
    uint64 rval;
    int local_port = 0 ;
     BCM_IF_ERROR_RETURN(
     _bcm_esw_gport_resolve(unit, subport, &modid, &port, &trunk_id, &id));
     BCM_IF_ERROR_RETURN
    (_bcmi_coe_subport_physical_port_get(unit, subport, &local_port));
    /* program the voq port profiles. */
    voq_port_map_entries = sal_alloc(sizeof(voq_port_map_entry_t)*128, 
                                        "voq port map entries");
    if (!voq_port_map_entries) {
        goto fail;
    }
    sal_memset(voq_port_map_entries, 0, sizeof(voq_port_map_entry_t)*128);
    entries[0] = voq_port_map_entries;
    rv = READ_VOQ_MOD_MAPm(unit, MEM_BLOCK_ALL, modid, &voq_mod_map);
    if (rv) {
        goto fail;
    }     
    BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
    port_base = soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, 
                                  BASE_QUEUE_NUM_0f);
    if (!soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                             &voq_mod_map, VOQ_VALIDf)) {
        port_offset = 1;
        soc_mem_field32_set(unit, VOQ_PORT_MAPm, &voq_port_map_entries[port],
                            VOQ_PORT_OFFSETf, port_offset);
        soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                             VOQ_MOD_OFFSETf, queue_id - 1 - port_base );
        soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                             VOQ_VALIDf, 1);
    } else {
        vpm_idx_old = _AP_VPM_ENTRIES_PER_SET *
                      soc_mem_field32_get(unit, VOQ_MOD_MAPm, &voq_mod_map,
                                          VOQ_MOD_PORT_PROFILE_INDEXf);
        rv = soc_profile_mem_get(unit, _bcm_ap_voq_port_map_profile[unit], 
                                 vpm_idx_old, _AP_VPM_ENTRIES_PER_SET, entries );
                                 
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            goto fail;
        }

        baseq_old = soc_mem_field32_get(unit, VOQ_MOD_MAPm,
                                       &voq_mod_map,VOQ_MOD_OFFSETf);
         
        port_offset = (queue_id - baseq_old - port_base) & 0x3FFF;
        soc_mem_field32_set(unit, VOQ_PORT_MAPm, &voq_port_map_entries[port],
                                VOQ_PORT_OFFSETf, port_offset);
        rv =soc_profile_mem_delete(unit, _bcm_ap_voq_port_map_profile[unit], 
                              vpm_idx_old); 
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            goto fail;
        }
    }
    rv = soc_profile_mem_add(unit, 
                           _bcm_ap_voq_port_map_profile[unit], 
                           entries, 128, &profile);
    if (rv) {
        goto fail;
    }
           
    soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                       VOQ_MOD_PORT_PROFILE_INDEXf, profile/128);
    rv = soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, 
                               modid, &voq_mod_map);
    if (rv) {
        goto fail;
    }
fail : 
    if (voq_port_map_entries) 
    {
       sal_free(voq_port_map_entries); 
    }
   return rv; 
}

/*
 * Function:
 *       _bcm_ap_coe_queue_offset_clear  
 * Purpose:
 *     clears the offset in voq_mod_map and voq_port_map table for the subport  
 * Parameters:
 *     unit       - (IN) unit number
 *     subport      - (IN) subport gport 
 *     queue_id     - (IN) base queue_id for the subport 
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_ap_coe_queue_offset_clear(int unit, int queue_id,
                            bcm_gport_t subport)
{

    bcm_module_t modid = 0;
    bcm_port_t port  = 0;
    voq_port_map_entry_t *voq_port_map_entries = NULL;
    voq_mod_map_entry_t voq_mod_map;
    int rv = BCM_E_NONE;
    void *entries[1];
    int vpm_idx_old = 0 ;
    int port_offset = 0 ;
    int i =0 ; 
    int module_in_use = 0; 
    uint32 profile = 0 ; 
    bcm_trunk_t trunk_id;
    int id;

     BCM_IF_ERROR_RETURN(
     _bcm_esw_gport_resolve( unit, subport, &modid, &port, &trunk_id, &id));
   /* program the voq port profiles. */
    voq_port_map_entries = sal_alloc(sizeof(voq_port_map_entry_t)*128, 
                                        "voq port map entries");
    if (!voq_port_map_entries) {
        goto fail;
    }
    sal_memset(voq_port_map_entries, 0, sizeof(voq_port_map_entry_t)*128);
    entries[0] = voq_port_map_entries;
    rv = READ_VOQ_MOD_MAPm(unit, MEM_BLOCK_ALL, modid, &voq_mod_map);
    if (rv) {
        goto fail;
    }     
    if (!soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                             &voq_mod_map, VOQ_VALIDf)) {
        rv = BCM_E_NOT_FOUND;
        goto fail;
    }

    vpm_idx_old = _AP_VPM_ENTRIES_PER_SET *
                      soc_mem_field32_get(unit, VOQ_MOD_MAPm, &voq_mod_map,
                                          VOQ_MOD_PORT_PROFILE_INDEXf);
    rv = soc_profile_mem_get(unit, _bcm_ap_voq_port_map_profile[unit], 
                             vpm_idx_old, _AP_VPM_ENTRIES_PER_SET, entries );
                                 
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
         goto fail;
    }

         
   
    port_offset = 0;
    soc_mem_field32_set(unit, VOQ_PORT_MAPm, &voq_port_map_entries[port],
                                VOQ_PORT_OFFSETf, port_offset);
   rv =soc_profile_mem_delete(unit, _bcm_ap_voq_port_map_profile[unit], 
                              vpm_idx_old); 
   if (rv) {
       goto fail;
   }
    
   for (i = 0; i < _AP_VPM_ENTRIES_PER_SET; i++) {
        if (soc_mem_field32_get(unit, VOQ_PORT_MAPm,
                                &voq_port_map_entries[i], VOQ_PORT_OFFSETf)) {
            module_in_use = 1;
            break;
        }
   }
   if (module_in_use) {
       rv = soc_profile_mem_add(unit, 
                             _bcm_ap_voq_port_map_profile[unit], 
                              entries, 128, &profile);
       if (rv) {
           goto fail;
       }
       soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                          VOQ_MOD_PORT_PROFILE_INDEXf, profile/128);
   } else 
   {
     soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                         VOQ_VALIDf, 0);
     soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                             VOQ_MOD_OFFSETf, 0 );
   }

   rv = soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, 
                               modid, &voq_mod_map);
fail : 
   if (voq_port_map_entries) 
   {
        sal_free(voq_port_map_entries); 
   }
   return rv; 
}

/*
 * Function:
 *       _bcm_ap_mmu_port_coe_control  
 * Purpose:
 *     Programs the ing_cos_mode to enable/disable subport in the physical port 
 * Parameters:
 *     unit       - (IN) unit number
 *     local_port      - (IN) physical port  
 *     eanble     - (IN) flag to control enable/disable 
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_ap_mmu_port_coe_control(int unit ,int local_port, int enable)
{
  uint64 rval;

  BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
  soc_reg64_field32_set(unit, ING_COS_MODE_64r, &rval, USE_MODf, enable) ;
  soc_reg64_field32_set(unit, ING_COS_MODE_64r, &rval, USE_PORTf, enable) ;
  BCM_IF_ERROR_RETURN(WRITE_ING_COS_MODE_64r(unit, local_port, rval));
  return BCM_E_NONE;

}

/*
 * Function:
 *     bcm_ap_cosq_gport_add
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
bcm_ap_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node = NULL;
    _bcm_ap_cosq_node_t *s1_node = NULL ;
    _bcm_ap_cosq_port_info_t *port_info;
    uint32 sched_encap;
    _bcm_ap_pipe_resources_t *res;
    int phy_port, mmu_port, local_port;
    int id, ii, q_base, pipe, qmin, qmax;
    bcm_gport_t tmp_gport;
    bcm_gport_t s1_sched;


    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_ap_cosq_gport_add: unit=%d port=0x%x numq=%d flags=0x%x\n"),
              unit, port, numq, flags));
    
    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_ap_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (!soc_feature(unit, 
                    soc_feature_mmu_hqos_four_level)) { 
        if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
           return BCM_E_UNAVAIL;
        }   
    }
    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
       BCM_IF_ERROR_RETURN(
        _bcmi_coe_subport_physical_port_get(unit, port, &local_port));
    }  else { 
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
    }
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_ap_mmu_info[unit];

    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);
#if 0
    if (!mmu_info->ets_mode) {
        /* cleanup the default lls setup */
        SOC_IF_ERROR_RETURN(soc_apache_lls_reset(unit));
        SOC_IF_ERROR_RETURN(soc_apache_lb_lls_init(unit));
        mmu_info->ets_mode = 1;
    }
#endif
    /* Check Alignment request is not for unsupported type */
    if (flags & BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN) {
        if (!(flags == (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN))) {
            return BCM_E_PARAM;
        }
    }
  
    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port)) {
            return BCM_E_PARAM;
        }
         
        if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
            BCM_IF_ERROR_RETURN(
            _bcm_ap_get_s1_node( unit, port, &s1_node));
            if (s1_node->coe_base_index == -1) {  
                _bcm_ap_allocate_coe_queues(unit, s1_node, 1);  
                _bcm_ap_coe_queue_offset_add(unit, s1_node->coe_base_index,
                                             port);
            }    
            for (id = s1_node->coe_base_index; 
                  id < (s1_node->coe_base_index + _AP_NUM_COS(unit)); id++) {
                 if (!res->p_queue_node[id].in_use) {
                     s1_node->coe_num_l2++;       
                     break;
                 }
            }
            if (id == (s1_node->coe_base_index + NUM_COS(unit)))
            {
                return BCM_E_RESOURCE;       
            }
        } else {
           for (id = port_info->uc_base; id < port_info->uc_limit; id++) {
                if (res->p_queue_node[id].numq == 0) {
                    break;
                }
           }
           if ((id == port_info->uc_limit) && 
                  (IS_AP_HSP_PORT(unit, local_port))) {
               return BCM_E_RESOURCE;
           }
           if (id == port_info->uc_limit) {
               BCM_IF_ERROR_RETURN(
                    _bcm_ap_node_index_get(res->l2_queue_list.bits, 0,
                        _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE, numq, 
                        1, &id));

               _bcm_ap_node_index_set(&res->l2_queue_list, id, 1);
           }
        }

        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &res->p_queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->level = SOC_APACHE_NODE_LVL_L2;
        if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port))
        {
           node->hw_cosq = id - s1_node->coe_base_index;
        } else
        { 
           node->hw_cosq = id - port_info->uc_base;
        } 
        if (IS_AP_HSP_PORT(unit, local_port)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            node->hw_index = _BCM_AP_HSP_L2_INDEX(unit, mmu_port, node->hw_cosq);
        } else {
            node->hw_index = soc_apache_l2_hw_index(unit, id, 1);
        }
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->type = _BCM_AP_NODE_UCAST;
        node->in_use = TRUE;
        node->port_gport = port;
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
        node->level = SOC_APACHE_NODE_LVL_L2;
        node->type = _BCM_AP_NODE_MCAST;
        node->hw_cosq = id - port_info->mc_base;
        if (IS_AP_HSP_PORT(unit, local_port)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
            node->hw_index = _BCM_AP_HSP_L2_INDEX(unit, mmu_port, node->hw_cosq);
            node->hw_index = soc_apache_l2_hw_index(unit, node->hw_index, 0);
        } else {
            node->hw_index = soc_apache_l2_hw_index(unit, id, 0);
        }
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->in_use = TRUE;
        node->port_gport = port;
        break;



    case BCM_COSQ_GPORT_SUBSCRIBER:
    case BCM_COSQ_GPORT_VIRTUAL_PORT:
    case BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP:
        /* Service Queueing are outside default queues. Hence NO alignment
         * required.
         */
        BCM_IF_ERROR_RETURN(
            _bcm_ap_node_index_get(res->l2_queue_list.bits, res->num_base_queues,
                                     _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE, numq, 
                                     1, &q_base));

        _bcm_ap_node_index_set(&res->l2_queue_list, q_base, numq);
        qmin = res->num_base_queues + (pipe * _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE);
        qmax = (pipe + 1) * _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;

        id = q_base;

        for (ii = 0; ii < numq; ii++, id++) {
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(tmp_gport, local_port, id);
            node= &res->p_queue_node[id];
            node->gport = tmp_gport;
            node->numq = numq - ii;
            node->level = SOC_APACHE_NODE_LVL_L2;
            node->type = (flags == BCM_COSQ_GPORT_VIRTUAL_PORT) ?
                            _BCM_AP_NODE_VM_UCAST : _BCM_AP_NODE_SERVICE_UCAST;
            node->hw_index = q_base + ii + (pipe * _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE);
            /* by default assign the cosq on order of order of alocation,
             * can be overriddedn by cos_mapping_set */
            node->hw_cosq = ii;
            node->local_port = local_port;
            node->remote_modid = -1;
            node->remote_port  = -1;
            node->in_use = TRUE;
            node->port_gport = port;
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

    qmin = res->num_base_queues;
    qmax = _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;
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
        node->level = SOC_APACHE_NODE_LVL_L2;
        node->type = _BCM_AP_NODE_VOQ;
        node->hw_index = -1;
        /* by default assign the cosq on order of order of alocation,
         * can be overriddedn by cos_mapping_set */
        node->hw_cosq = -1;
        node->local_port = local_port;
        node->remote_modid = -1;
        node->remote_port  = -1;
        node->in_use = TRUE;
        node->port_gport = port;
        break;

    case BCM_COSQ_GPORT_SCHEDULER:
    case BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN:
        if (numq < -1) {
            return BCM_E_PARAM;
        }
        for (id = _BCM_AP_NUM_PORT_SCHEDULERS; 
                id < _BCM_AP_NUM_TOTAL_SCHEDULERS; id++) {
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
        node->type = _BCM_AP_NODE_SCHEDULER;
        node->attached_to_input = -1;
        node->port_gport = port;
        if (flags & BCM_COSQ_GPORT_SCHEDULER_PFC_ALIGN) {
            node->node_align_value = _BCM_AP_START_INDEX_ALIGN_PFC;
        } 
        break;

    case BCM_COSQ_GPORT_WITH_ID:
    case 0:
        if (numq < -1) {
            return BCM_E_PARAM;
        }

        /* this is a port level scheduler */
        id = local_port;

        if ( id < 0 || id >= _BCM_AP_NUM_PORT_SCHEDULERS) {
            return BCM_E_PARAM;
        }
        if (IS_AP_HSP_PORT(unit, local_port)) {
           if (numq > _BCM_AP_HSP_PORT_MAX_L0) {
           return BCM_E_PARAM;
          }
        } 
        node = &mmu_info->sched_node[id];
        sched_encap = (id << 8) | local_port;
        BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
        node->gport = *gport;
        node->level = SOC_APACHE_NODE_LVL_ROOT;
        node->type = _BCM_AP_NODE_SCHEDULER;
        phy_port = SOC_INFO(unit).port_l2p_mapping[local_port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        node->hw_index = mmu_port;
        if (IS_AP_HSP_PORT(unit, local_port)) {
            node->numq = _BCM_AP_HSP_PORT_MAX_L0;
        } else {
            node->numq = -1;
        }
        node->local_port = local_port;
        node->in_use = TRUE;
        node->attached_to_input = 0;
        node->port_gport = port;
        if (!(soc_feature(unit,soc_feature_mmu_hqos_four_level)) && 
             !(IS_AP_HSP_PORT(unit, local_port)))  
        {
            node->numq = 1;
            BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, numq,
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &s1_sched));
            BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, s1_sched, *gport, 0));
            *gport = s1_sched ; 
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
 *     bcm_ap_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of parent_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     parent_port - (IN) GPORT to detach from
 *     cosq       - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    _bcm_ap_cosq_node_t *sched_node, *parent_node = NULL, *prev_node;
    _bcm_ap_cosq_node_t *child_node = NULL; 
    bcm_port_t sched_port, parent_port = -1;
    _bcm_ap_cosq_node_t *parent;
    _bcm_ap_cosq_list_t *list = NULL;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t * res;
    _bcm_ap_cosq_port_info_t *port_info;
    int start = 0, end = 0;
    int base_index;
    int subport_enabled = 0 ;
    int hw_index = 0 ; 
    if (_bcm_ap_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(parent_gport)) ||
        (BCM_GPORT_IS_MCAST_QUEUE_GROUP(parent_gport)) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(parent_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, sched_gport, 0, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->attached_to_input < 0) {
        /* Not attached yet */
        return BCM_E_PORT;
    }

    if (parent_gport != BCM_GPORT_INVALID) {
        if (BCM_GPORT_IS_SCHEDULER(parent_gport) || 
            BCM_GPORT_IS_MODPORT(parent_gport)   || 
            BCM_GPORT_IS_LOCAL(parent_gport)) {
               BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, parent_gport, 0, NULL, 
                                    &parent_port, NULL, &parent_node));
        } else {
            if (!(BCM_GPORT_IS_SCHEDULER(sched_gport) || 
                        BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                        BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport)) ||
                    (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(parent_gport))) {
                return BCM_E_PARAM;
            }
            else {
                BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, parent_gport,
                                                &parent_port));
                parent_node = NULL;
            }
        }
    }

    if (parent_port == -1) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[parent_port];
    res = port_info->resources;

    if (sched_port != parent_port) {
        return BCM_E_PORT;
    }

    if (sched_node->parent != parent_node) {
        return BCM_E_PORT;
    }

    if ((cosq < -1) || 
        (parent_node && (parent_node->numq != -1) && (cosq >= parent_node->numq))) {
        return BCM_E_PARAM;
    }

     if (cosq != -1) {
        if (sched_node->attached_to_input != cosq) {
            return BCM_E_PARAM;
        }
    }

    if (sched_node->type == _BCM_AP_NODE_SERVICE_UCAST) {
        end = sched_node->numq;
    } else {
        end = 1;
    }

    for (start = 0; start < end; start++) {
        if (sched_node->parent != NULL) {
            BCM_IF_ERROR_RETURN(
                    soc_apache_cosq_set_sched_parent(unit, parent_port, 
                        sched_node->level, sched_node->hw_index, 
                        sched_node->parent->hw_index, 0));
            /* unresolve the node - delete this node from parent's child list */
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_node_unresolve(unit, sched_node, cosq));

            /* now remove from the sw tree */
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
            hw_index = sched_node->hw_index; 
            if (!IS_AP_HSP_PORT(unit, parent_port)) {
                switch (parent->level) {
                    case SOC_APACHE_NODE_LVL_ROOT:
                        list = &res->s1_sched_list;
                        sched_node->hw_index = -1;
                        break;
                    case SOC_APACHE_NODE_LVL_S1:
                        if (IS_CPU_PORT(unit, parent_port)) {
                            list = &res->cpu_l0_sched_list;
                        } else {
                            list = &res->l0_sched_list;
                        }
                        sched_node->hw_index = -1;
                        break;
                    case SOC_APACHE_NODE_LVL_L0:
                        list = &res->l1_sched_list;
                        sched_node->hw_index = -1;
                        break;
                    case SOC_APACHE_NODE_LVL_L1:
                        list = &res->l2_queue_list;
                        break;
                    default:
                        break;
                }
             
                if (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport)){ 
                    if (parent->numq == -1) { 
                        BCM_IF_ERROR_RETURN(_bcm_ap_node_index_clear(list, 
                                    hw_index, 1));
                    } else if ((parent->child == NULL) &&
                               (sched_node->level != SOC_APACHE_NODE_LVL_L2)) {
                              if (list) {
                                 base_index =  parent->base_index; 
                                 if (IS_CPU_PORT(unit, parent_port) &&  
                                     (sched_node->level == SOC_APACHE_NODE_LVL_L0)) {
                                      base_index =  parent->base_index - _BCM_AP_CPU_PORT_L0_MIN;
                                 }
                                 BCM_IF_ERROR_RETURN(_bcm_ap_node_index_clear(list, 
                                    base_index, parent->base_size));
                              }
                             list = NULL;
                             parent->base_index = -1;
                             parent->base_size  = 0;
                   } 
                }
                /* Indexes that are allocated statically is not released to the pool*/  
                if ((sched_node->level == SOC_APACHE_NODE_LVL_L2) && 
                        (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport))){ 
                    if ((sched_node->type == _BCM_AP_NODE_VOQ)) { 
                        _bcm_ap_node_index_clear(list, sched_node->hw_index, 1);
                        sched_node->hw_index = -1;
                    }
                }
            }
        }
        if ((sched_node->type == _BCM_AP_NODE_SERVICE_UCAST) && 
            ((start + 1) < end)){
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, sched_gport, start + 1, NULL,
                                        &sched_port, NULL, &sched_node));
            if (!sched_node) {
                return BCM_E_NOT_FOUND;
            }
        }
    }

    if (soc_feature(unit, 
          soc_feature_mmu_hqos_four_level) &&  
          sched_node->level == SOC_APACHE_NODE_LVL_S1) {
        child_node = parent_node->child; 
        while(child_node) {
              if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                 child_node->port_gport)) {
                 subport_enabled = 1;    
                 break; 
               }
               child_node = child_node->sibling;   
        } 
        if (!subport_enabled) { 
            BCM_IF_ERROR_RETURN(
                _bcm_ap_mmu_port_coe_control(unit, 
                                 sched_node->local_port, 0)); 
        }
    }
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                         hw_cosq=%d\n"),
              sched_node->attached_to_input));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_ap_cosq_node_t *node, *tnode, *base_node;
    _bcm_ap_cosq_node_t * s1_node = NULL ;
    int ii, local_port, phy_port, mmu_port, hw_index;
    _bcm_ap_mmu_info_t *mmu_info;
    soc_info_t *si;
    bcm_gport_t parent_gport = 0 ;
    int level = 0 ;
    _bcm_ap_pipe_resources_t * res;
    _bcm_ap_cosq_port_info_t *port_info;
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_ap_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    node = NULL;
    base_node= NULL;

    mmu_info = _bcm_ap_mmu_info[unit];
    si = &SOC_INFO(unit);
    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));
        port_info = &mmu_info->port_info[local_port];
        res = port_info->resources;

    } else if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
        BCM_IF_ERROR_RETURN
         (_bcm_ap_get_s1_node(unit, gport, &node));
         BCM_IF_ERROR_RETURN
        (_bcmi_coe_subport_physical_port_get(unit, gport, &local_port));
        port_info = &mmu_info->port_info[local_port];
        res = port_info->resources;

    } else {
         BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &local_port));
        if (local_port >= _BCM_AP_MAX_PORT) {
            return BCM_E_PORT;
        }
       
        port_info = &mmu_info->port_info[local_port];
        res = port_info->resources;
        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];
        hw_index = _BCM_AP_ROOT_SCHED_INDEX(unit, mmu_port);

        for (ii = 0; ii < _BCM_AP_NUM_TOTAL_SCHEDULERS; ii++) {
            tnode = &mmu_info->sched_node[ii];
            if (tnode->in_use == FALSE) {
                continue;
            }
            if ((tnode->level == SOC_APACHE_NODE_LVL_ROOT) &&
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
    if (!node->in_use) { 
         return BCM_E_NONE; 
    }  
    level = node->level;
    if (node->parent)
    {
        parent_gport = node->parent->gport; 
    }
    while(node->child != NULL) {
        BCM_IF_ERROR_RETURN(bcm_ap_cosq_gport_delete(unit, node->child->gport));
    }

    if (!soc_feature(unit, soc_feature_mmu_hqos_four_level) &&
        (level == SOC_APACHE_NODE_LVL_ROOT) && (!node->in_use)) 
    {
        return BCM_E_NONE;  
    }
    if (node->level != SOC_APACHE_NODE_LVL_ROOT && node->attached_to_input >= 0) {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_detach(unit, node->gport, 
                                node->parent->gport, node->attached_to_input));
    }


    if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) { 
        if ((node->level == SOC_APACHE_NODE_LVL_L2)  &&
             _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, node->port_gport)) {
             BCM_IF_ERROR_RETURN
             (_bcm_ap_get_s1_node(unit, node->port_gport, &s1_node));
             s1_node->coe_num_l2--;
             if (!s1_node->coe_num_l2) { 
                 BCM_IF_ERROR_RETURN
                 (_bcm_ap_coe_queue_offset_clear(unit, s1_node->coe_base_index,
                                                  node->port_gport));
                  BCM_IF_ERROR_RETURN
                  (_bcm_ap_allocate_coe_queues(unit, s1_node, 0));  
             }
        }
    }     
    if ((node->type == _BCM_AP_NODE_VOQ) && 
            (node->remote_modid > 0)) {
        _bcm_ap_voq_next_base_node_get (unit, local_port,
                node->remote_modid, &base_node);
        if (base_node != NULL) {
            BCM_PBMP_ASSIGN(base_node->fabric_pbmp,
                    node->fabric_pbmp);
        }
    }
    
    if ((node->level == SOC_APACHE_NODE_LVL_L2) && 
        (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) &&  
                        (node->type != _BCM_AP_NODE_VOQ) && 
        ((node->hw_index < port_info->uc_base) ||  
                            (node->hw_index >= port_info->uc_limit)))    
    {     
         _bcm_ap_node_index_clear(&res->l2_queue_list, 
                                  node->hw_index, 1);
         node->hw_index = -1;
    }
   if (node->cosq_map != NULL) {
       sal_free(node->cosq_map);
   }
   if ((!(soc_feature(unit, soc_feature_mmu_hqos_four_level)) && 
       node->level == SOC_APACHE_NODE_LVL_S1) && parent_gport )  
   { 
       BCM_IF_ERROR_RETURN(bcm_ap_cosq_gport_delete(unit, parent_gport));
   }
    if (IS_AP_HSP_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_clear_child_list(unit, node));
    }
    _BCM_AP_COSQ_NODE_INIT(node);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_get
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
bcm_ap_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_ap_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_ap_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, &id, &node));

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
        if (node->type == _BCM_AP_NODE_SERVICE_UCAST) {
            *flags = BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP;
        } else if (node->type == _BCM_AP_NODE_VM_UCAST) {
            *flags = BCM_COSQ_GPORT_VIRTUAL_PORT;
        } else if (node->type == _BCM_AP_NODE_VOQ) {
            *flags = BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP;
        } else {
            *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        *flags = BCM_COSQ_GPORT_SCHEDULER;
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
 *     bcm_ap_cosq_gport_traverse
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
bcm_ap_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    _bcm_ap_cosq_node_t *port_info;
    _bcm_ap_mmu_info_t *mmu_info;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port, port_out;

    if (_bcm_ap_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_ap_mmu_info[unit];

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    my_modid, port, &modid_out, &port_out));

        /* root node */
        port_info = &mmu_info->sched_node[port_out];

        if (port_info->gport >= 0) {
            _bcm_ap_cosq_gport_traverse(unit, port_info->gport, cb, user_data);
        }
      }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_ap_cosq_gport_attach
 * Purpose:
 *     Attach sched_port to the specified index (cosq) of parent_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     parent_port - (IN) GPORT to attach to
 *     cosq       - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_gport_attach(int unit, bcm_gport_t gport, 
                           bcm_gport_t parent_gport, 
                           bcm_cos_queue_t to_cosq)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node, *parent_node, *child_node, *s1_node;
    bcm_port_t port, to_port, local_port = 0, mmu_port;
    int rv, tmp;
    int max_nodes = 0;
    int subport_enabled = 0;
    uint32 rval = 0;
    uint32 mc_group_mode = 0; 
    
    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(parent_gport)) ||
        (BCM_GPORT_IS_MCAST_QUEUE_GROUP(parent_gport)) ||
        (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(parent_gport))) {
        return BCM_E_PARAM;
    }

    if(!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_SCHEDULER(gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &port, NULL, &node));

    if (node->attached_to_input >= 0) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }
    tmp = node->attached_to_input;

    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, parent_gport, 0, NULL, &to_port, NULL,
                                   &parent_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, parent_gport, &to_port));
        parent_node = &mmu_info->sched_node[to_port];
    }

    if (port != to_port) {
        return BCM_E_PORT;
    }
    if (!parent_node)  
    {
        return BCM_E_NOT_FOUND; 
    }

    if ((parent_node->level == SOC_APACHE_NODE_LVL_ROOT) && 
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, node->port_gport))  { 
        rv = _bcm_ap_get_s1_node(unit, node->port_gport, 
         &s1_node);      
        if( rv != BCM_E_NOT_FOUND) { 
            return BCM_E_EXISTS;
        }  

    }
    if (parent_node->level != SOC_APACHE_NODE_LVL_ROOT &&
        parent_node->port_gport != node->port_gport)
    {
        return BCM_E_PARAM;     
    }
    mmu_port = SOC_APACHE_MMU_PORT(unit, port);
    
    /* Identify the levels of schedulers
     * parent_port == phy_port && gport == scheduler => gport = S1
     * parent_port == scheduler && level = S1 && gport == scheduler => gport = L0
     * parent_port == schduler && lvel = L0 && gport == scheduler => parent_port = L0 and gport = L1
     * parent_port == scheduler && port == queue_group => parent_port = L1 and gport = L2
     */
     if (!BCM_GPORT_IS_SCHEDULER(parent_gport)) {
         if ((parent_node->numq == 0) || (parent_node->numq_expandable)) {
             local_port = (BCM_GPORT_IS_LOCAL(parent_gport)) ?
                           BCM_GPORT_LOCAL_GET(parent_gport) :
                           BCM_GPORT_MODPORT_PORT_GET(parent_gport);

             if (!SOC_PORT_VALID(unit, local_port)) {
                 return BCM_E_PORT;
             }
             parent_node->in_use = TRUE;
             parent_node->local_port = port;
             if (IS_AP_HSP_PORT(unit, local_port)) {
                 parent_node->base_index = _BCM_AP_HSP_L0_INDEX(unit, mmu_port, 0);
                 parent_node->numq = _BCM_AP_HSP_PORT_MAX_L0;
             } else {
                 parent_node->gport = parent_gport;
                 parent_node->hw_index = _BCM_AP_ROOT_SCHED_INDEX(unit, mmu_port);
                 parent_node->level = SOC_APACHE_NODE_LVL_ROOT;
                 parent_node->attached_to_input = 0;
                 parent_node->numq_expandable = 1;
                 if (to_cosq == -1) {
                     parent_node->numq += 1;
                 } else {
                     if (to_cosq >= parent_node->numq) {
                         parent_node->numq = to_cosq + 1;
                     }
                 }
             }
         }

         if (!BCM_GPORT_IS_SCHEDULER(gport)) {
             return BCM_E_PARAM;
         }

         if (IS_AP_HSP_PORT(unit, to_port)) {
             node->level = SOC_APACHE_NODE_LVL_L0;
         } else {
             node->level = SOC_APACHE_NODE_LVL_S1;
         } 
     } else {
         if (parent_node->level == SOC_APACHE_NODE_LVL_ROOT) {
             if (IS_AP_HSP_PORT(unit, to_port)) {
                 node->level = SOC_APACHE_NODE_LVL_L0;
             } else {
                 node->level = SOC_APACHE_NODE_LVL_S1;
             }
         }
         if (parent_node->level == SOC_APACHE_NODE_LVL_S1) {
             node->level = SOC_APACHE_NODE_LVL_L0;
         }
         if (parent_node->level == SOC_APACHE_NODE_LVL_L0) {
             node->level = SOC_APACHE_NODE_LVL_L1;

             /*only for hsp ports with mc-group-mode enabled 2 l2 mulicast
               can be attached directly to L0.0 */
             if (IS_AP_HSP_PORT(unit, to_port)) {
                 SOC_IF_ERROR_RETURN(
                         READ_HSP_SCHED_PORT_CONFIGr(unit, to_port, &rval));
                 mc_group_mode = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                         rval, MC_GROUP_MODEf);
                 if ((mc_group_mode) && (parent_node->attached_to_input == 0))
                     node->level = SOC_APACHE_NODE_LVL_L2;
             } 
         }
         if (parent_node->level == SOC_APACHE_NODE_LVL_L1) {
             node->level = SOC_APACHE_NODE_LVL_L2;
         }
         BCM_IF_ERROR_RETURN
             (_bcm_ap_cosq_max_nodes_get(unit, node->level, &max_nodes));
         if (parent_node->numq > max_nodes) {
            return BCM_E_PARAM;
         }
    }

    if ((to_cosq < -1) || ((parent_node->numq != -1) && (to_cosq >= parent_node->numq))) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(parent_gport) || BCM_GPORT_IS_LOCAL(parent_gport) ||
        BCM_GPORT_IS_MODPORT(parent_gport)) {
        int start = 0, end = 0;
        if (parent_node->attached_to_input < 0) {
            /* dont allow to attach to a node that has already attached */
            return BCM_E_PARAM;
        }

        if (node->type == _BCM_AP_NODE_SERVICE_UCAST) {
            end = node->numq;
        } else {
            end = 1;
        }

        for (start = 0; start < end; start++) {
            node->parent = parent_node;
            node->sibling = parent_node->child;
            parent_node->child = node;
            /* resolve the nodes */
            rv = _bcm_ap_cosq_node_resolve(unit, node, to_cosq + start);
            if (BCM_FAILURE(rv)) {
                parent_node->child = node->sibling;
                node->parent = NULL;
                node->attached_to_input = tmp;
                return rv;
            }
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "                         hw_cosq=%d\n"),
                      node->attached_to_input));

            if ((node->type == _BCM_AP_NODE_SERVICE_UCAST) && 
                ((start + 1) < end)){
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, gport, start + 1, NULL,
                                            &port, NULL, &node));
                if (!node) {
                    return BCM_E_NOT_FOUND;
                }
            }
        }
        if (soc_feature(unit, 
                soc_feature_mmu_hqos_four_level) &&  
                node->level == SOC_APACHE_NODE_LVL_S1) {
            child_node = parent_node->child; 
            while(child_node) {
                  if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit,
                     child_node->port_gport)) {
                     subport_enabled++ ;
                     if(subport_enabled > 1) {  
                        break;   
                     }   
                  }
                  child_node = child_node->sibling; 
            }
            if (subport_enabled == 1) { 
                BCM_IF_ERROR_RETURN(
                _bcm_ap_mmu_port_coe_control(unit, 
                                node->local_port, 1)); 
            } 
        }
    } else {
            return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_attach_get
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     parent_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_ap_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *parent_gport, bcm_cos_queue_t *cosq)
{
    _bcm_ap_cosq_node_t *sched_node;
    bcm_module_t modid, modid_out;
    bcm_port_t port, port_out;

    if (parent_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, sched_gport, 0, &modid, &port, NULL,
                               &sched_node));

    if (sched_node->attached_to_input < 0) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (sched_node->parent != NULL) { /* sched_node is not an S1 scheduler */
        *parent_gport = sched_node->parent->gport;
    } else {  /* sched_node is an S1 scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port,
                                    &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(*parent_gport, modid_out, port_out);
    }
    *cosq = sched_node->attached_to_input;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_gport_child_get
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
bcm_ap_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                            bcm_cos_queue_t cosq,
                            bcm_gport_t *out_gport)
{
    _bcm_ap_cosq_node_t *in_node = NULL;
    _bcm_ap_cosq_node_t *cur_node = NULL;
    _bcm_ap_cosq_node_t *tmp_node = NULL;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_port_t local_port;

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
        (_bcm_ap_cosq_node_get(unit, in_gport, 0, &modid, &port, NULL,
                               &in_node));
    if ((in_node->child == NULL) && (in_node->level == SOC_APACHE_NODE_LVL_L2)) {
       return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_mmu_hqos_four_level))
    {
        cur_node = in_node->child; 
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit,in_gport, &local_port));
        if ( (in_node->level == SOC_APACHE_NODE_LVL_ROOT)  && !IS_AP_HSP_PORT(unit, local_port) ) { 
            if ((in_node->child == NULL) || (in_node->child->child == NULL) ) {
                return BCM_E_PARAM;
            }
            cur_node = in_node->child->child; 
        } else {
            cur_node = in_node->child; 
        }
    }
    for (; cur_node != NULL; cur_node = cur_node->sibling) {
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
_bcm_ap_hw_drop_prob_to_percent[] = {
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
_bcm_ap_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= _bcm_ap_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_ap_drop_prob_to_percent(int drop_prob) {
   return (_bcm_ap_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * index: degree, value: contangent(degree) * 100
 * max value is 0xffff (16-bit) at 0 degree
 */
STATIC int
_bcm_ap_cotangent_lookup_table[] =
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
_bcm_ap_angle_to_cells(int angle)
{
    return (_bcm_ap_cotangent_lookup_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_ap_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_ap_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}

/*
 * Function:
 *     _bcm_ap_cosq_bucket_set
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
_bcm_ap_cosq_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 kbits_burst_min, uint32 kbits_burst_max, 
                        uint32 flags)
{
    _bcm_ap_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    int index;
    uint32 rval;
    uint32 meter_flags;
    uint32 bucketsize_max, bucketsize_min;
    uint32 granularity_max, granularity_min;
    uint32 refresh_rate_max, refresh_rate_min;
    int refresh_bitsize = 0, bucket_bitsize = 0;
    soc_mem_t config_mem = INVALIDm;
    uint32 mc_group_mode = 0;
    int max_l1_table = 2,max_l2_table = 8;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mtro_l1[] = { MMU_MTRO_L1_MEM_0m,
                            MMU_MTRO_L1_MEM_1m };
    soc_mem_t mtro_l2[] = { MMU_MTRO_L2_MEM_0m,
                            MMU_MTRO_L2_MEM_1m,
                            MMU_MTRO_L2_MEM_2m,
                            MMU_MTRO_L2_MEM_3m,
                            MMU_MTRO_L2_MEM_4m,
                            MMU_MTRO_L2_MEM_5m,
                            MMU_MTRO_L2_MEM_6m,
                            MMU_MTRO_L2_MEM_7m };

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
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_SET(port) && 
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, port, cosq, NULL, 
                                    NULL, NULL, &node));
        if ((node->level == SOC_APACHE_NODE_LVL_ROOT) &&
            !IS_AP_HSP_PORT(unit, local_port) &&
            (min_quantum > 0)) {
            /* min shaper is not supported */
            return BCM_E_PARAM;
        }
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(port) || BCM_GPORT_IS_MODPORT(port)) {
            if (node->level == SOC_APACHE_NODE_LVL_ROOT) {
                if (IS_AP_HSP_PORT(unit, local_port)) {
                    config_mem = MMU_MTRO_L0_MEM_0m;
                } else {
                    config_mem = MMU_MTRO_S1_MEM_0m;
                }
            } else if (node->level == SOC_APACHE_NODE_LVL_S1) {
                config_mem = MMU_MTRO_L0_MEM_0m;
            } else if (node->level == SOC_APACHE_NODE_LVL_L0) {
                if (IS_AP_HSP_PORT(unit, local_port)){
                    SOC_IF_ERROR_RETURN(
                        READ_HSP_SCHED_PORT_CONFIGr(unit, local_port, &rval));

                    mc_group_mode = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                                      rval, MC_GROUP_MODEf);
                }
                if (mc_group_mode) { 
                    config_mem = mtro_l2[index % max_l2_table];
                    index /= max_l2_table;
                }
                else {
                    config_mem = mtro_l1[index % max_l1_table];
                    index /= max_l1_table;
                }
            } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
                config_mem = mtro_l2[index % max_l2_table ];
                index /= max_l2_table;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            config_mem = mtro_l2[index % max_l2_table];
            index /= max_l2_table;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (IS_CPU_PORT(unit, local_port)) {
            config_mem = mtro_l2[index % max_l2_table]; 
            index /= max_l2_table;
        } else if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT
                 (unit, port)) 
        { 
            config_mem = mtro_l2[index % max_l2_table]; 
            index /= max_l2_table;
        } else {
            config_mem = mtro_l1[index % max_l1_table];
            index /= max_l1_table; 
        }
    }

    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        refresh_bitsize = soc_mem_field_length(unit, config_mem, REFRESHf);
        bucket_bitsize = soc_mem_field_length(unit, config_mem, THD_SELf);
    } else {
        refresh_bitsize = soc_mem_field_length(unit, config_mem, MAX_REFRESHf);
        bucket_bitsize = soc_mem_field_length(unit, config_mem, MAX_THD_SELf);
    }

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
    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        soc_mem_field32_set(unit, config_mem, &entry, METER_GRANf, granularity_max);
        soc_mem_field32_set(unit, config_mem, &entry, REFRESHf, refresh_rate_max);
        soc_mem_field32_set(unit, config_mem, &entry, THD_SELf, bucketsize_max);
    } else {
        soc_mem_field32_set(unit, config_mem, &entry, MAX_METER_GRANf, granularity_max);
        soc_mem_field32_set(unit, config_mem, &entry, MAX_REFRESHf, refresh_rate_max);
        soc_mem_field32_set(unit, config_mem, &entry, MAX_THD_SELf, bucketsize_max);

        soc_mem_field32_set(unit, config_mem, &entry, MIN_METER_GRANf, granularity_min);
        soc_mem_field32_set(unit, config_mem, &entry, MIN_REFRESHf, refresh_rate_min);
        soc_mem_field32_set(unit, config_mem, &entry, MIN_THD_SELf, bucketsize_min);

        soc_mem_field32_set(unit, config_mem, &entry, SHAPER_CONTROLf,
                            (flags & BCM_COSQ_BW_PACKET_MODE) ? 1 : 0);
    }

    soc_mem_field32_set(unit, config_mem, &entry, EAV_ENABLEf, 
                         (flags & BCM_COSQ_BW_EAV_MODE) ? 1 : 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_bucket_get
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
_bcm_ap_cosq_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                        uint32 *flags)
{
    _bcm_ap_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    int index;
    uint32 rval;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;
    soc_mem_t config_mem = INVALIDm;
    uint32 mc_group_mode = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int max_l1_table = 2,max_l2_table = 8;
    soc_mem_t mtro_l1[] = { MMU_MTRO_L1_MEM_0m,
                            MMU_MTRO_L1_MEM_1m };
    soc_mem_t mtro_l2[] = { MMU_MTRO_L2_MEM_0m,
                            MMU_MTRO_L2_MEM_1m,
                            MMU_MTRO_L2_MEM_2m,
                            MMU_MTRO_L2_MEM_3m,
                            MMU_MTRO_L2_MEM_4m,
                            MMU_MTRO_L2_MEM_5m,
                            MMU_MTRO_L2_MEM_6m,
                            MMU_MTRO_L2_MEM_7m };

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_SET(port) && 
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, port, cosq, NULL, 
                                    &local_port, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(port) || BCM_GPORT_IS_MODPORT(port)) {
            if (node->level == SOC_APACHE_NODE_LVL_ROOT) {
                if (IS_AP_HSP_PORT(unit, local_port)) {
                    config_mem = MMU_MTRO_L0_MEM_0m;
                } else {
                    config_mem = MMU_MTRO_S1_MEM_0m;
                }
            } else if (node->level == SOC_APACHE_NODE_LVL_S1) {
                config_mem = MMU_MTRO_L0_MEM_0m;
            } else if (node->level == SOC_APACHE_NODE_LVL_L0) {
                if (IS_AP_HSP_PORT(unit, local_port)) {
                    SOC_IF_ERROR_RETURN(
                        READ_HSP_SCHED_PORT_CONFIGr(unit,
                                local_port, &rval));
                    mc_group_mode = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                            rval, MC_GROUP_MODEf);
                }
                if (mc_group_mode){
                    config_mem = mtro_l2[index % max_l2_table ];
                    index /= max_l2_table;
                } else {
                    config_mem = mtro_l1[index % max_l1_table];
                    index /= max_l1_table; 
                }
            } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
                config_mem = mtro_l2[index % max_l2_table ];
                index /= max_l2_table;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            config_mem = mtro_l2[index % max_l2_table];
            index /= max_l2_table;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (IS_CPU_PORT(unit, local_port)) {
            config_mem = mtro_l2[index % max_l2_table];
            index /= max_l2_table;
        } else if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT
                 (unit, port))
        {
            config_mem = mtro_l2[index % max_l2_table]; 
            index /= max_l2_table;
        } else {
            config_mem = mtro_l1[index % max_l1_table ];
            index /= max_l1_table;
        }
    }

    if (min_quantum == NULL || max_quantum == NULL || 
        kbits_burst_max == NULL || kbits_burst_min == NULL) {
        return BCM_E_PARAM;
    }


    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    meter_flags = 0;
    *flags = 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }
    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        if (soc_mem_field32_get(unit, config_mem, &entry, MODEf)) {
            meter_flags |= _BCM_TD_METER_FLAG_PACKET_MODE;
            *flags |= BCM_COSQ_BW_PACKET_MODE;
        }
    } else {
        if (soc_mem_field32_get(unit, config_mem, &entry, SHAPER_CONTROLf)) {
            meter_flags |= _BCM_TD_METER_FLAG_PACKET_MODE;
            *flags |= BCM_COSQ_BW_PACKET_MODE;
        }
    }

    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        granularity = soc_mem_field32_get(unit, config_mem, &entry, METER_GRANf);
        refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, REFRESHf);
        bucketsize = soc_mem_field32_get(unit, config_mem, &entry, THD_SELf);
    } else {
        granularity = soc_mem_field32_get(unit, config_mem, &entry, MAX_METER_GRANf);
        refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MAX_REFRESHf);
        bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MAX_THD_SELf);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           max_quantum, kbits_burst_max));

    if (config_mem != MMU_MTRO_S1_MEM_0m) {
        granularity = soc_mem_field32_get(unit, config_mem, &entry, MIN_METER_GRANf);
        refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MIN_REFRESHf);
        bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MIN_THD_SELf);

        BCM_IF_ERROR_RETURN
            (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                            granularity, meter_flags,
                                           min_quantum, kbits_burst_min));
    }

    if (soc_mem_field32_get(unit, config_mem, &entry, EAV_ENABLEf)) {
        *flags |= BCM_COSQ_BW_EAV_MODE;
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_ap_cosq_time_domain_get(int unit, int time_id, int *time_value)
{
    uint32 rval;

    if (time_id < 0 || time_id > _AP_NUM_TIME_DOMAIN - 1) {
        return SOC_E_PARAM;
    }

    if (time_value == NULL) {
        return SOC_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TIME_DOMAINr(unit, &rval));
    *time_value = soc_reg_field_get(unit, TIME_DOMAINr, rval, 
                                      time_domain1[unit][time_id].field);

    return SOC_E_NONE;
}




STATIC int
_bcm_ap_cosq_time_domain_set(int unit, int time_value, int *time_id)
{
    uint32 rval;
    int rv = SOC_E_NONE;
    int id;

    if (time_value < 0 || time_value > 63) {
        return SOC_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TIME_DOMAINr(unit, &rval));
    for (id = 0; id < _AP_NUM_TIME_DOMAIN; id++) {/* Find a unset field from  TIME_0 to TIME_3 */
        if (!time_domain1[unit][id].ref_count) {
            soc_reg_field_set(unit, TIME_DOMAINr, &rval, time_domain1[unit][id].field,
                                time_value);
            time_domain1[unit][id].ref_count++;
            break;
        }
    }
    
    if(id == _AP_NUM_TIME_DOMAIN){/* No field in TIME_DOMAINr is free,return ERR*/
        rv = BCM_E_RESOURCE;
    }

    if(time_id){
        *time_id = id;
    }
        
    SOC_IF_ERROR_RETURN(WRITE_TIME_DOMAINr(unit, rval));
    return rv;
}

STATIC int
_bcm_ap_cosq_wred_index_resolve(int unit, bcm_port_t port,
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
        if((port == -1) || (cosq == -1)) {
            index = _BCM_WRED_GLOBAL_SERVICE_POOL;
            to_index = index + 3;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_index_resolve(unit, port, cosq,
                        _BCM_AP_COSQ_INDEX_STYLE_WRED_POOL,
                        NULL, &start_pool_idx, NULL));
            index = to_index = _BCM_WRED_GLOBAL_SERVICE_POOL + start_pool_idx;
        }
    } else {
        if ((lflags & BCM_COSQ_DISCARD_PORT) || (flags & BCM_COSQ_DISCARD_PORT)) {
            if (port == -1) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_WRED_PORT,
                                             local_port, &index, NULL));
            if (cosq != -1) {
                BCM_IF_ERROR_RETURN(_bcm_ap_cosq_index_resolve(unit, port, cosq,
                        _BCM_AP_COSQ_INDEX_STYLE_WRED_POOL,
                        NULL, &start_pool_idx, NULL));
                index = to_index = index + start_pool_idx;
            } else {
                to_index = index + 3;
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, port, cosq == -1 ? 0:cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_WRED,
                                     &local_p, &index, NULL));

            si = &SOC_INFO(unit);
            phy_port = si->port_l2p_mapping[local_p];
            mmu_port = si->port_p2m_mapping[phy_port];

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, port, cosq == -1 ? 0:cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_WRED_POOL,
                                            NULL, &start_pool_idx, NULL));
            service_pool = start_pool_idx;

            if(cosq == -1) {
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
 *     _bcm_ap_cosq_wred_set
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
_bcm_ap_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
    int rate, i, jj, to_index = -1, exists = 0;
    int time_id,time,old_time,current_time_sel;
    int port_id = 0, pool_id = 0;
    int rv = SOC_E_NONE;
    uint32 rval;
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm
    };


    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_wred_index_resolve(unit, port, cosq,
                            flags, lflags, &local_port, &index, &to_index,
                            &port_id, &pool_id));

    /* WRED doesn't work on CPU/LB Ports */
    if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    for(jj = index; jj <= to_index; jj++) {
        wred_mem = MMU_WRED_CONFIG_X_PIPEm;

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
                (soc_profile_mem_get(unit, _bcm_ap_wred_profile[unit],
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
                    mems[0] = MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[1] = MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[2] = MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm;
                }
            }
            if (flags & BCM_COSQ_DISCARD_NONTCP) {
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[3] = MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[4] = MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm;
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[5] = MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm;
                }
            }
            rate = _bcm_ap_percent_to_drop_prob(drop_probability);
            for (i = 0; i < 6; i++) {
                exists = 0;
                if ((soc_mem_field32_get(unit, wred_mems[i], entries[i], 
                                MIN_THDf) != 0x1ffff) && (mems[i] == INVALIDm)) {
                    mems[i] = wred_mems[i];
                    exists = 1;
                } else {
                    soc_mem_field32_set(unit, wred_mems[i], entries[i], 
                            MIN_THDf, (mems[i] == INVALIDm) ? 0x1ffff : min_thresh);
                }

                if ((soc_mem_field32_get(unit, wred_mems[i], entries[i], 
                        MAX_THDf) != 0x1ffff) && ((mems[i] == INVALIDm) || exists)) {
                    mems[i] = wred_mems[i];
                    exists = 1;
                } else {
                    soc_mem_field32_set(unit, wred_mems[i], entries[i], 
                            MAX_THDf, (mems[i] == INVALIDm) ? 0x1ffff :max_thresh);
                }

                if (!exists) {
                    soc_mem_field32_set(unit, wred_mems[i], entries[i], 
                        MAX_DROP_RATEf, (mems[i] == INVALIDm) ? 0 : rate);
                }
            }
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_ap_wred_profile[unit], 
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
        for (time_id = 0; time_id < _AP_NUM_TIME_DOMAIN; time_id++) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_time_domain_get(unit, time_id, &old_time));            
            if (time == old_time) {
                /* Only set exist flag if this entry reference count already update,
                              otherwise update reference count */
                if(time_id != current_time_sel){ 
                    soc_mem_field32_set(unit, wred_mem, &qentry, TIME_DOMAIN_SELf, 
                                        time_id);
                    time_domain1[unit][time_id].ref_count++;
                    time_domain1[unit][current_time_sel].ref_count--;
                }
                exists = 1;
                break;
            }
        }

        if(!exists){
            rv = _bcm_ap_cosq_time_domain_set(unit, time, &time_id);
            if(rv == SOC_E_NONE){
                soc_mem_field32_set(unit, wred_mem, &qentry, TIME_DOMAIN_SELf, 
                                    time_id);
                time_domain1[unit][current_time_sel].ref_count--;
            } else {
                return rv;  
            }
        }
/*
 *Disabling the wred sampling and writing to wred mem
 *This was required   for an hardware bug and can be taken out for apache b0
 */ 
        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));  
        
        BCM_IF_ERROR_RETURN(
              soc_mem_write(unit, wred_mem, MEM_BLOCK_ALL, jj, &qentry));

        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, REFRESH_ENf, 1);
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));  

        if (old_profile_index != 0xffffffff) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _bcm_ap_wred_profile[unit],
                                        old_profile_index));
        }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_ap_cosq_wred_get
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
_bcm_ap_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
    int rate, time_id,time;


    if ((port == -1 ) && 
            !((*flags & BCM_COSQ_DISCARD_DEVICE) || (lflags & BCM_COSQ_DISCARD_DEVICE))) {
        return (BCM_E_PORT); 
    }

    local_port = -1; 
    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_wred_index_resolve(unit, port, cosq,
                            *flags, lflags, &local_port, &index, NULL, NULL, NULL));

    /* WRED doesn't work on CPU/LB Ports */
    if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }
    wred_mem = MMU_WRED_CONFIG_X_PIPEm;

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, wred_mem, MEM_BLOCK_ALL, index, &qentry));
    profile_index = soc_mem_field32_get(unit, wred_mem,
                                        &qentry, PROFILE_INDEXf);

    if (!(*flags & BCM_COSQ_DISCARD_NONTCP)) {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm;
            entry_p = &entry_tcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm;
            entry_p = &entry_tcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm;
            entry_p = &entry_tcp_red;
        } else {
            mem = MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm;
            entry_p = &entry_tcp_green;
        }
    } else {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm;
            entry_p = &entry_nontcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm;
            entry_p = &entry_nontcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm;
            entry_p = &entry_nontcp_red;
        } else {
            mem = MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm;
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
        (soc_profile_mem_get(unit, _bcm_ap_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_ap_drop_prob_to_percent(rate);
    }

    if (gain) {
        *gain = soc_mem_field32_get(unit, wred_mem, &qentry, WEIGHTf);
    }

    if (refresh_time) {
        time_id = soc_mem_field32_get(unit, wred_mem, &qentry, TIME_DOMAIN_SELf);
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_time_domain_get(unit,time_id,&time));
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

/*
 * Function:
 *     _bcm_ap_cosq_sched_get
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
_bcm_ap_cosq_sched_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                       int *mode, int *weight)
{
    _bcm_ap_cosq_node_t *node, *child_node , *s1_node;
    bcm_port_t local_port;
    int sch_index, lvl = SOC_APACHE_NODE_LVL_L1;
    soc_apache_sched_mode_e    sched_mode;
    int index, numq;
    _bcm_ap_mmu_info_t *mmu_info;
    soc_info_t *si;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
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
     
    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)){
       BCM_IF_ERROR_RETURN(
        _bcmi_coe_subport_physical_port_get(unit, gport, &local_port));
    }  else { 
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
    } 
    si = &SOC_INFO(unit);
    if (BCM_GPORT_IS_SCHEDULER(gport)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL,
                                   &node));        
        if ((node->numq >= 0) && (cosq > node->numq)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_child_node_at_input(node, cosq, &child_node));
     } else if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT
                (unit, gport)) {  
                if (soc_feature(unit, 
                    soc_feature_mmu_hqos_four_level)) {
                    BCM_IF_ERROR_RETURN
                   (_bcm_ap_get_s1_node(unit, gport, 
                   &s1_node));      
                   index = s1_node->coe_base_index; 
                   child_node = &mmu_info->queue_node[index + cosq ];       
                   if (!child_node) {
                       return BCM_E_INTERNAL;
                   }
                   if (child_node->attached_to_input == -1) {
                       return BCM_E_NOT_FOUND;
                   }
                   node = child_node->parent ;
                   if (!node) {
                       return BCM_E_INTERNAL;
                   }
  
                } else { 
                   return BCM_E_PARAM;
                } 
     } else { 
            if (IS_CPU_PORT(unit, local_port)) {
                numq = SOC_INFO(unit).num_cpu_cosq;
                if (cosq >= numq) {
                    return BCM_E_PARAM;
                }
                index = soc_apache_l2_hw_index(unit,
                    si->port_cosq_base[local_port] + cosq, 0);
            } else {
                index = soc_apache_l2_hw_index(unit,
                    si->port_uc_cosq_base[local_port], 1);
            }

            child_node = &mmu_info->queue_node[index];
            if (!child_node) {
                return BCM_E_INTERNAL;
            }
            if (child_node->attached_to_input == -1) {
                return BCM_E_NOT_FOUND;
            }
            node = child_node->parent;
            if (node && !IS_CPU_PORT(unit, local_port)) {
                child_node = node;
                node = node->parent;
                if (!node) {
                    return BCM_E_INTERNAL;
                }
                if ((node->numq >= 0) && (cosq > node->numq)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_child_node_at_input(node, cosq, 
                                                &child_node));
            }
    }

    if (!child_node) {
        return BCM_E_INTERNAL;
    }

    sch_index = child_node->hw_index;
    lvl = child_node->level;

    BCM_IF_ERROR_RETURN(
            soc_apache_cosq_get_sched_mode(unit, local_port, lvl, sch_index,
                            &sched_mode, weight));

    switch(sched_mode) {
        case SOC_APACHE_SCHED_MODE_STRICT:
            *mode = BCM_COSQ_STRICT;
        break;
        case SOC_APACHE_SCHED_MODE_WRR:
            *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
        case SOC_APACHE_SCHED_MODE_WDRR:
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
 *     _bcm_ap_lls_node_count
 * Purpose:
 *     This function counts the number of nodes at a particular level
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number 
 *     level      - (IN) level of the node in LLS
 *     hw_index   - (IN) h/w index of the node
 *     cookie     - (IN) state of the LLS structure  
 */
STATIC int _bcm_ap_lls_node_count(int unit, int port, int level, 
                                   int hw_index, void *cookie)
{
    _bcm_ap_lls_info_t *i_lls_tree = (_bcm_ap_lls_info_t *) cookie;
    i_lls_tree->node_count[level] += 1;
    return 0;
}

/*
 * Function:
 *     _bcm_ap_num_ones
 * Purpose:
 *     This function calculates the numbers of 1s in the binary format
 *     of the number 
 * Parameters:
 *     m       - (IN) number
 * Returns:
 *     The number of 1s in the binary format of the number
 */
STATIC uint32 
_bcm_ap_num_ones(uint32 m)
{
    uint32 i = 0;

    while (m) {
        m &= m - 1;
        i +=  1;
    }
    return i;
}

STATIC int
_bcm_ap_cosq_traverse_lls_tree(int unit, int port, _bcm_ap_cosq_node_t *node,
                                _soc_apache_lls_traverse_cb cb, void *cookie)
{
    if (node->child) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_traverse_lls_tree(unit, port, 
                                                            node->child,
                                                            cb, cookie));
    }

    if (node->sibling) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_traverse_lls_tree(unit, port, 
                                                node->sibling, cb, cookie));
    }

    if (node->level != SOC_APACHE_NODE_LVL_ROOT) {
        BCM_IF_ERROR_RETURN(cb(unit, port, node->level, node->hw_index, cookie));
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_lls_shapers_remove
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
STATIC int _bcm_ap_lls_shapers_remove(int unit, int port, int level, 
                                       int hw_index, void *cookie)
{
    _bcm_ap_lls_info_t *i_lls_tree = (_bcm_ap_lls_info_t *) cookie;
    soc_mem_t config_mem;
    uint32  *p_entry;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, kbit_max, kbit_min;
    uint32 meter_flags;
    int max_l1_table = 2 ,max_l2_table = 8 ; 
    uint32 bucketsize_max, granularity_max, refresh_rate_max,
           refresh_bitsize = 0, bucket_bitsize = 0;

    soc_mem_t mtro_l1[] = { MMU_MTRO_L1_MEM_0m,
                            MMU_MTRO_L1_MEM_1m };
    soc_mem_t mtro_l2[] = { MMU_MTRO_L2_MEM_0m,
                            MMU_MTRO_L2_MEM_1m,
                            MMU_MTRO_L2_MEM_2m,
                            MMU_MTRO_L2_MEM_3m,
                            MMU_MTRO_L2_MEM_4m,
                            MMU_MTRO_L2_MEM_5m,
                            MMU_MTRO_L2_MEM_6m,
                            MMU_MTRO_L2_MEM_7m };

    if (level == SOC_APACHE_NODE_LVL_MAX) {
        return SOC_E_PARAM;
    }

    if (i_lls_tree->child_lvl != level) {
        return BCM_E_NONE;
    }

    if (level == SOC_APACHE_NODE_LVL_S1) {
        config_mem = MMU_MTRO_S1_MEM_0m;
    } else if (level == SOC_APACHE_NODE_LVL_L0) {
        config_mem = MMU_MTRO_L0_MEM_0m;
    } else if (level == SOC_APACHE_NODE_LVL_L1) {
        config_mem = mtro_l1[hw_index % max_l1_table];
        hw_index /= max_l1_table;
    } else if (level == SOC_APACHE_NODE_LVL_L2) {
        config_mem = mtro_l2[hw_index % max_l2_table];
        hw_index /= max_l2_table;
    } else {
        return BCM_E_PARAM;
    }

    if (config_mem ==  INVALIDm) {
        return BCM_E_PARAM;
    }

    index = i_lls_tree->offset[level] + i_lls_tree->count[level];
    p_entry = &i_lls_tree->mtro_entries[index * SOC_MAX_MEM_WORDS];
    
    if (*p_entry == 0) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, hw_index, 
                                         p_entry));
    }

    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    
            
    meter_flags = 0;

    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        refresh_bitsize = soc_mem_field_length(unit, config_mem, REFRESHf);
        bucket_bitsize = soc_mem_field_length(unit, config_mem, THD_SELf);
    } else {
        refresh_bitsize = soc_mem_field_length(unit, config_mem, MAX_REFRESHf);
        bucket_bitsize = soc_mem_field_length(unit, config_mem, MAX_THD_SELf);
    }

    /* Setting the max metering configuration */
    kbit_max = i_lls_tree->kbit_max;
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, kbit_max, kbit_max,
                                       meter_flags, refresh_bitsize,
                                       bucket_bitsize, &refresh_rate_max,
                                       &bucketsize_max, &granularity_max));

    if (config_mem == MMU_MTRO_S1_MEM_0m) {
        soc_mem_field32_set(unit, config_mem, entry, METER_GRANf,
                            granularity_max);
        soc_mem_field32_set(unit, config_mem, entry, REFRESHf,
                            refresh_rate_max);
        soc_mem_field32_set(unit, config_mem, entry, THD_SELf, 
                            bucketsize_max);
    } else {
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
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, hw_index, entry));
  
    sal_usleep(100); 
    i_lls_tree->count[level] += 1;
    return 0;
}

/*
 * Function:
 *     _bcm_ap_lls_shapers_restore
 * Purpose:
 *     This function restores the shapers on the nodes  
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number 
 *     level      - (IN) level of the node in LLS
 *     hw_index   - (IN) h/w index of the node
 *     cookie     - (IN) state of the LLS structure  
 */
STATIC int _bcm_ap_lls_shapers_restore(int unit, int port, int level, 
                                        int hw_index, void *cookie)
{
    _bcm_ap_lls_info_t *i_lls_tree = (_bcm_ap_lls_info_t *) cookie;
    soc_mem_t config_mem;
    uint32  *p_entry;
    int index;
    int max_l1_table = 2, max_l2_table = 8;
    uint32 entry[SOC_MAX_MEM_WORDS];    

    soc_mem_t mtro_l1[] = { MMU_MTRO_L1_MEM_0m,
                            MMU_MTRO_L1_MEM_1m };
    soc_mem_t mtro_l2[] = { MMU_MTRO_L2_MEM_0m,
                            MMU_MTRO_L2_MEM_1m,
                            MMU_MTRO_L2_MEM_2m,
                            MMU_MTRO_L2_MEM_3m,
                            MMU_MTRO_L2_MEM_4m,
                            MMU_MTRO_L2_MEM_5m,
                            MMU_MTRO_L2_MEM_6m,
                            MMU_MTRO_L2_MEM_7m };

    if (level == SOC_APACHE_NODE_LVL_MAX) {
        return SOC_E_PARAM;
    }

    if (level == SOC_APACHE_NODE_LVL_S1) {
        config_mem = MMU_MTRO_S1_MEM_0m;
    } else if (level == SOC_APACHE_NODE_LVL_L0) {
        config_mem = MMU_MTRO_L0_MEM_0m;
    } else if (level == SOC_APACHE_NODE_LVL_L1) {
        config_mem = mtro_l1[hw_index % max_l1_table];
        hw_index /=  max_l1_table;
    } else if (level == SOC_APACHE_NODE_LVL_L2) {
        config_mem = mtro_l2[hw_index % max_l2_table];
        hw_index /= max_l2_table;
    } else {
        return BCM_E_PARAM;
    }

    if (i_lls_tree->child_lvl  != level) {
        return BCM_E_NONE;
    }

    if (config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }


    if (config_mem != MMU_MTRO_S1_MEM_0m) {
        sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

        soc_mem_field32_set(unit, config_mem, entry, MIN_REFRESHf,
                            500);
        soc_mem_field32_set(unit, config_mem, entry, MIN_THD_SELf,
                            3);
    
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, hw_index, entry));

        sal_usleep(100);
    }

    index = i_lls_tree->offset[level] + i_lls_tree->count[level];
    p_entry = &i_lls_tree->mtro_entries[index * SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, 
                                      hw_index, p_entry));
    
    i_lls_tree->count[level] += 1;

    return 0;
}

/*
 * Function:
 *     _bcm_ap_adjust_lls_bw
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
_bcm_ap_adjust_lls_bw(int unit, bcm_port_t port, _bcm_ap_cosq_node_t *node, 
                       int child_lvl, int child_hw_index, int start, 
                       _bcm_ap_lls_info_t *i_lls_tree)
{
    int rv = BCM_E_NONE, count, mem_size, ii, speed, jj;
    soc_info_t *si;
    if (!soc_feature(unit, soc_feature_vector_based_spri)) {
        return BCM_E_NONE;
    }
    
    if (IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }       
    if (start) {
        sal_memset(i_lls_tree, 0, sizeof(_bcm_ap_lls_info_t));
        i_lls_tree->child_lvl = child_lvl;
        i_lls_tree->child_hw_index = child_hw_index;

        if (node) {
            /* get to top node */
            while(node->parent) { node = node->parent; }

            if ((rv = _bcm_ap_cosq_traverse_lls_tree(unit, port, node, 
                                    _bcm_ap_lls_node_count, i_lls_tree))) {
                goto cleanup;
            }
        }          

        for (count = 0, ii = 0; ii < SOC_APACHE_NODE_LVL_MAX; ii++) {
            count += i_lls_tree->node_count[ii];
            for (jj = ii - 1; jj >= 0; jj--) {
                i_lls_tree->offset[ii] += i_lls_tree->node_count[jj];
            }
        }
        
        if (count == 0) {
            return BCM_E_INIT;
        }
        
        mem_size = sizeof(uint32) * SOC_MAX_MEM_WORDS * count;
        i_lls_tree->mtro_entries = sal_alloc(mem_size, "lls_war_buf");

        si = &SOC_INFO(unit);
        speed = si->port_speed_max[port];

        i_lls_tree->kbit_min = 0;
        i_lls_tree->kbit_max = ((speed)/10)/i_lls_tree->node_count[child_lvl];

        for (count = 0, ii = 0; ii < SOC_APACHE_NODE_LVL_MAX; ii++) {
            i_lls_tree->count[ii] = 0;
        }
 
        sal_memset(i_lls_tree->mtro_entries, 0, mem_size);

        if (node) {

            /* get to top node */
            while(node->parent) { node = node->parent; }

            if ((rv = _bcm_ap_cosq_traverse_lls_tree(unit, port, node, 
                                    _bcm_ap_lls_shapers_remove, i_lls_tree))) {
  
                goto cleanup;
            }
        }
    } else {
        for (count = 0, ii = 0; ii < SOC_APACHE_NODE_LVL_MAX; ii++) {
            i_lls_tree->count[ii] = 0;
        }

        if (node) {
            /* get to top node */
            while(node->parent) { node = node->parent; }

            if ((rv = _bcm_ap_cosq_traverse_lls_tree(unit, port, node, 
                                    _bcm_ap_lls_shapers_restore, i_lls_tree))) {
                goto cleanup;
            }
        }
       
       if (i_lls_tree->mtro_entries) {
            sal_free(i_lls_tree->mtro_entries);
            i_lls_tree->mtro_entries = NULL;
       }
 
    }

    return rv;

cleanup:
    /* cleanup */
    if (i_lls_tree->mtro_entries) {
        sal_free(i_lls_tree->mtro_entries);
        i_lls_tree->mtro_entries = NULL;
    }
     
    return rv;
}

/*
 * Function:
 *     _bcm_apache_compute_lls_config
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
_bcm_apache_compute_lls_config (int unit, int port, int level, 
                                 int *first_sp_child, 
                                 int *first_sp_mc_child,
                                 int *num_spri,
                                 uint32 *ucmap, 
                                 uint32 *spmap, 
                                 int child_index, 
                                 soc_apache_sched_mode_e cur_mode,
                                 soc_apache_sched_mode_e mode,
                                 int max_children)
{
    uint32 ucm = 0, mcm = 0, cur_spmap, cur_num_spri, lucmap = 0;
    int ii, *pfc, uc, mc, fc, oc, *use_fc;

    cur_num_spri = _bcm_ap_num_ones(*spmap);
    cur_spmap = *spmap;

    if (level == SOC_APACHE_NODE_LVL_L1) {
        if (IS_CPU_PORT(unit, port)) {
            use_fc = first_sp_mc_child;
        } else {
            use_fc = (child_index >= _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE) ? 
                        first_sp_mc_child : first_sp_child;
        }
    } else {
        use_fc = first_sp_child;
    }
    
    if (cur_num_spri == 0) {
        *ucmap = 0;
        *spmap = 0;
    }

    if (mode == SOC_APACHE_SCHED_MODE_STRICT) {
        if (child_index > *use_fc) {
            if ((child_index - *use_fc) >= max_children && cur_num_spri > 0) {
                 return BCM_E_UNAVAIL;
            }
        } else if (child_index < *use_fc) {
            return BCM_E_UNAVAIL;
        }
    }
 
    if (level == SOC_APACHE_NODE_LVL_L1) {
        for (mcm = 0, mc = 0, ucm = 0, uc = 0, ii = 0; ii < 8; ii++) {
            if (*ucmap & (1 << ii)) {
                mcm |= (cur_spmap & (1 << ii)) ? (1 << mc) : 0;
                mc += 1;
            } else {
                ucm |= (cur_spmap & (1 << ii)) ? (1 << uc) : 0;
                uc += 1;
            }
        }

        if (child_index < _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE) {
            fc = *first_sp_child;
            oc =  _bcm_ap_num_ones(ucm);
            BCM_IF_ERROR_RETURN(_bcm_apache_compute_lls_config(unit, port,
                        level - 1, &fc, NULL, &oc, &lucmap, &ucm, child_index, 
                        cur_mode, mode, 8));
        } else {
            fc = *first_sp_mc_child;
            oc =  _bcm_ap_num_ones(mcm);
            BCM_IF_ERROR_RETURN(_bcm_apache_compute_lls_config(unit, port,
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
        if (mode == SOC_APACHE_SCHED_MODE_STRICT) {
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

    *num_spri = _bcm_ap_num_ones(cur_spmap);

    *spmap = cur_spmap;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_compute_lls_config
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
_bcm_ap_compute_lls_config(int unit, int port, int level, 
                            int *first_sp_child, 
                            int *first_sp_mc_child,
                            int *num_spri,
                            uint32 *ucmap, 
                            uint32 *spmap, 
                            int child_index, 
                            soc_apache_sched_mode_e cur_mode,
                            soc_apache_sched_mode_e mode)
{
    if (!soc_feature(unit, soc_feature_vector_based_spri)) {
        return BCM_E_UNAVAIL;
    }

    if (((cur_mode != SOC_APACHE_SCHED_MODE_STRICT) &&
        (mode != SOC_APACHE_SCHED_MODE_STRICT)) ||
       ((cur_mode == SOC_APACHE_SCHED_MODE_STRICT) && 
        (mode == SOC_APACHE_SCHED_MODE_STRICT))) {
        return BCM_E_NONE;
    }

    return _bcm_apache_compute_lls_config(unit, port, level, first_sp_child,
                                          first_sp_mc_child, num_spri,
                                          ucmap, spmap, child_index,
                                          cur_mode, mode, 8);
}

STATIC int
_bcm_ap_cosq_sched_parent_child_set(int unit, int port, int level,
                                     int sched_index, int child_index,
                                     soc_apache_sched_mode_e sched_mode, 
                                     int weight, _bcm_ap_cosq_node_t *child_node)
{
    int wt, num_spri, first_sp_child, first_sp_mc_child;
    uint32 ucmap = 0, spmap = 0;
    soc_apache_sched_mode_e cur_sched_mode;
    _bcm_ap_lls_info_t lls_tree_info;
    int rv = 0, rv1 = 0, child_level;
    if (child_node == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_apache_cosq_get_sched_config(unit, port, level,
                sched_index, child_index, &num_spri, &first_sp_child,
                &first_sp_mc_child, &ucmap, &spmap, &cur_sched_mode, 
                &wt));    
    /* If the dynamic switchover feature is enabled, then compute the LLS 
       configuration and adjust the bandwidth at the level such that each 
       node has sufficient bandwidth to perform the switchover. Also set
       the new scheduler config based on the request */ 
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        if(((cur_sched_mode == SOC_APACHE_SCHED_MODE_WRR) &&
                    (sched_mode == SOC_APACHE_SCHED_MODE_WRR)) ||
                ((cur_sched_mode == SOC_APACHE_SCHED_MODE_WDRR) &&
                 (sched_mode == SOC_APACHE_SCHED_MODE_WDRR))) {
            if (child_index >= 0) {
                soc_apache_get_child_type(unit, port, level, &child_level);
                SOC_IF_ERROR_RETURN(soc_apache_cosq_set_sched_mode(unit, port,
                            child_level,
                            child_index, sched_mode, weight));
            }
        } else {
            if ((rv = _bcm_ap_compute_lls_config(unit, port, level, &first_sp_child,
                            &first_sp_mc_child, &num_spri,
                            &ucmap, &spmap, child_index,
                            cur_sched_mode, sched_mode))) {
                return rv;
            }
            if (cur_sched_mode != sched_mode) {
                SOC_EGRESS_METERING_LOCK(unit);
                if ((rv = _bcm_ap_adjust_lls_bw(unit, port, child_node,
                                child_node->level, child_index, 1, &lls_tree_info))){
                    goto error;
                }
            }
            if ((rv = soc_apache_cosq_set_sched_config(unit, port, level,
                            sched_index, child_index,
                            num_spri, first_sp_child,
                            first_sp_mc_child, ucmap,
                            spmap, sched_mode, weight))){
                goto error;
            }
        }
        if (cur_sched_mode != sched_mode) {
            /* restore the default bw */
            goto error;
        }
    }

    return BCM_E_NONE;

/* If there is an error during the switchover process, then restore the 
   bandwidth to the previous state */
error:
    if (cur_sched_mode != sched_mode) {
        rv1 = _bcm_ap_adjust_lls_bw(unit, port, child_node,
                child_node->level,
                child_index,
                0, &lls_tree_info);
        SOC_EGRESS_METERING_UNLOCK(unit);
        if (rv1) {
            return rv1;
        }
    }
    return rv;
}

/*
 * Function:
 *     _bcm_ap_cosq_sched_set
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
_bcm_ap_cosq_sched_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        int mode, int weight)
{
    _bcm_ap_cosq_node_t *node = NULL, *child_node , *s1_node;
    bcm_port_t local_port;
    int lwts = 1, index, numq;
    soc_apache_sched_mode_e sched_mode;
    _bcm_ap_mmu_info_t *mmu_info;
    soc_info_t *si;
    void *setting = NULL;
    int rv = BCM_E_NONE;

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

    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)){
       BCM_IF_ERROR_RETURN(
        _bcmi_coe_subport_physical_port_get(unit, gport, &local_port));
    }  else { 
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
    }
    si = &SOC_INFO(unit);
    mmu_info = _bcm_ap_mmu_info[unit];
    
    switch(mode) {
        case BCM_COSQ_STRICT:
            sched_mode = SOC_APACHE_SCHED_MODE_STRICT;
            lwts = 0;
        break;
        case BCM_COSQ_ROUND_ROBIN:
            sched_mode = SOC_APACHE_SCHED_MODE_WRR;
            lwts = 1;
        break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            sched_mode = SOC_APACHE_SCHED_MODE_WRR;
            lwts = weight;
        break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            sched_mode = SOC_APACHE_SCHED_MODE_WDRR;
            lwts = weight;
        break;
        default:
            return BCM_E_PARAM;
    }

    if (lwts == 0) {
        sched_mode = SOC_APACHE_SCHED_MODE_STRICT;
    }

    if (BCM_GPORT_IS_SCHEDULER(gport)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL,
                                   &node));        
        if ((node->numq >= 0) && (cosq > node->numq)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_child_node_at_input(node, cosq, &child_node));
     } else if (_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT
                (unit, gport)) {  
                if (soc_feature(unit, 
                    soc_feature_mmu_hqos_four_level)) {
                    BCM_IF_ERROR_RETURN
                   (_bcm_ap_get_s1_node(unit, gport, 
                   &s1_node));      
                   index = s1_node->coe_base_index; 
                   child_node = &mmu_info->queue_node[index + cosq ];       
                   if (!child_node) {
                       return BCM_E_INTERNAL;
                   }
                   if (child_node->attached_to_input == -1) {
                       return BCM_E_NOT_FOUND;
                   }
                   node = child_node->parent ;
                   if (!node) {
                       return BCM_E_INTERNAL;
                   }
  
                } else { 
                   return BCM_E_PARAM;
                } 
     } else { 
            if (IS_CPU_PORT(unit, local_port)) {
                numq = SOC_INFO(unit).num_cpu_cosq;
                if (cosq >= numq) {
                    return BCM_E_PARAM;
                }
                index = soc_apache_l2_hw_index(unit,
                    si->port_cosq_base[local_port] + cosq, 0);
            } else {   
                index = soc_apache_l2_hw_index(unit,
                    si->port_uc_cosq_base[local_port], 1);
            }

            child_node = &mmu_info->queue_node[index];
            if (!child_node) {
                return BCM_E_INTERNAL;
            }
            if (child_node->attached_to_input == -1) {
                return BCM_E_NOT_FOUND;
            }
            node = child_node->parent;
            if (node && !IS_CPU_PORT(unit, local_port)) {
                child_node = node;
                node = node->parent;
                if (!node) {
                    return BCM_E_INTERNAL;
                }
                if ((node->numq >= 0) && (cosq > node->numq)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_child_node_at_input(node, cosq, 
                                                &child_node));
            }
    }

    if (!node || !child_node) {
        return BCM_E_INTERNAL;
    }

    /* S1 is always in WRR in Apache */
    if (!IS_AP_HSP_PORT(unit, local_port) &&
        (soc_feature(unit, soc_feature_mmu_hqos_four_level)) &&
        (node->level == SOC_APACHE_NODE_LVL_ROOT) &&
        (mode == BCM_COSQ_STRICT)) {
        return BCM_E_PARAM;
    }

    if (node->attached_to_input < 0) { /* Not resolved yet */
        return BCM_E_NOT_FOUND;
    }

    /* HSP/LLS - child node at cosq index of parent gport
        * is resloved to set it's schedule mode and weight
        */
    if (IS_AP_HSP_PORT(unit, local_port)) {
        rv = soc_apache_cosq_set_sched_mode(unit, local_port,
                        child_node->level, child_node->hw_index, sched_mode, lwts);
    } else {
        BCM_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                            local_port, &setting));
        rv = _bcm_ap_cosq_sched_parent_child_set(unit,
                local_port, node->level, node->hw_index,
                child_node->hw_index, sched_mode, lwts, child_node);
        BCM_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit, local_port, setting));
        if (rv < 0) {
            return rv;
        }
        if (!BCM_GPORT_IS_SCHEDULER(gport) && 
            !BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) &&
            !BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) &&
            node && IS_CPU_PORT(unit, local_port)) {
            child_node = node;
            node = node->parent;
            lwts = (sched_mode == SOC_APACHE_SCHED_MODE_STRICT) ? 0 : 1;
            BCM_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                                  local_port, &setting));
            rv = _bcm_ap_cosq_sched_parent_child_set(unit,
                    local_port, node->level, node->hw_index,
                    child_node->hw_index, sched_mode, lwts, child_node);
            BCM_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit, local_port, setting));
            if (rv < 0) {
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     bcm_ap_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_detach(int unit, int software_state_only)
{
    int i;
    soc_info_t *si;

    if (!_bcm_ap_mmu_info[unit] ) {
         return BCM_E_NONE; 
    } 

    (void) _bcm_bst_detach(unit);

    if (NULL != _bcm_ap_endpoint_queuing_info[unit]) {
        if (NULL != _bcm_ap_endpoint_queuing_info[unit]->ptr_array) {
            for (i = 0; i < _BCM_AP_NUM_ENDPOINT(unit); i++) {
                if (_BCM_AP_ENDPOINT_IS_USED(unit, i)) {
                    sal_free(_BCM_AP_ENDPOINT(unit, i));
                    _BCM_AP_ENDPOINT(unit, i) = NULL;
                }
            }
            sal_free(_bcm_ap_endpoint_queuing_info[unit]->ptr_array);
            _bcm_ap_endpoint_queuing_info[unit]->ptr_array = NULL;
        }
        if (NULL != _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile) {
            soc_profile_mem_destroy(unit,
                    _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile);
            sal_free(_bcm_ap_endpoint_queuing_info[unit]->cos_map_profile);
            _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile = NULL;
        }
        sal_free(_bcm_ap_endpoint_queuing_info[unit]);
        _bcm_ap_endpoint_queuing_info[unit] = NULL;
    }

    si = &SOC_INFO(unit); 
    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_ap_cosq_cpu_cosq_config[unit][i] != NULL) { 
            sal_free(_bcm_ap_cosq_cpu_cosq_config[unit][i]);
            _bcm_ap_cosq_cpu_cosq_config[unit][i] = NULL; 
        }
    }
    if (_bcm_ap_wred_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_wred_profile[unit]);
        sal_free(_bcm_ap_wred_profile[unit]);
        _bcm_ap_wred_profile[unit] = NULL; 
    } 
    if (_bcm_ap_cos_map_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_cos_map_profile[unit]);
        sal_free(_bcm_ap_cos_map_profile[unit]);
        _bcm_ap_cos_map_profile[unit] = NULL;
    } 
    if (_bcm_ap_sample_int_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_sample_int_profile[unit]);
        sal_free(_bcm_ap_sample_int_profile[unit]);
        _bcm_ap_sample_int_profile[unit] = NULL;
    } 
    if (_bcm_ap_feedback_profile[unit] != NULL )
    {
        soc_profile_reg_destroy(unit, _bcm_ap_feedback_profile[unit]);
        sal_free(_bcm_ap_feedback_profile[unit]);
         _bcm_ap_feedback_profile[unit] = NULL ;
    } 
    if (_bcm_ap_llfc_profile[unit] != NULL )
    {
        soc_profile_reg_destroy(unit, _bcm_ap_llfc_profile[unit]);
        sal_free(_bcm_ap_llfc_profile[unit]);
        _bcm_ap_llfc_profile[unit] = NULL ;
    } 
    if (_bcm_ap_voq_port_map_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_voq_port_map_profile[unit]);
        sal_free(_bcm_ap_voq_port_map_profile[unit]);
        _bcm_ap_voq_port_map_profile[unit] = NULL;
    } 
    if (_bcm_ap_ifp_cos_map_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_ifp_cos_map_profile[unit]);
        sal_free(_bcm_ap_ifp_cos_map_profile[unit]);
        _bcm_ap_ifp_cos_map_profile[unit] = NULL;  
    } 
    if (_bcm_ap_service_port_map_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_service_port_map_profile[unit]);
        sal_free(_bcm_ap_service_port_map_profile[unit]);
        _bcm_ap_service_port_map_profile[unit] = NULL;
    } 
    if (_bcm_ap_service_cos_map_profile[unit] != NULL )
    {
        soc_profile_mem_destroy(unit, _bcm_ap_service_cos_map_profile[unit]);
        sal_free(_bcm_ap_service_cos_map_profile[unit]);
        _bcm_ap_service_cos_map_profile[unit] = NULL;  
    }
    if (_bcm_ap_mmu_info[unit] != NULL) {
        _bcm_ap_cosq_free_memory(_bcm_ap_mmu_info[unit]);
        sal_free(_bcm_ap_mmu_info[unit]);
        _bcm_ap_mmu_info[unit] = NULL;
    }
    soc_apache_fc_map_shadow_free(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_port_cosq_config_set
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
STATIC int
bcm_ap_port_cosq_config_set(int unit, bcm_port_t  port, int enable)
{
    port_cos_map_entry_t cos_map_entries[16];
    port_cos_map_entry_t hg_cos_map_entries[16];
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entries[1], *hg_entries[1];
    uint32 index,  old_index, hg_index;
    int cos, prio, numq, rv;
    uint32 i;

    numq = _AP_NUM_COS(unit);

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

            rv  =  soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit],
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

            rv =  soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit],
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

        rv = soc_profile_mem_delete(unit, _bcm_ap_cos_map_profile[unit],
                old_index);

        soc_mem_unlock(unit, PORT_COS_MAPm);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_config_set
 * Purpose:
 *     Set the number of COS queues
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    port_cos_map_entry_t hg_cos_map_entries[16];
    void *entries[1], *hg_entries[1];
    uint32 index, hg_index;
    bcm_port_t port;
    int cos, prio;
    uint32 i = 0;
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
                                      _bcm_ap_cos_map_profile[unit],
                                      index, &ref_count));
        if (ref_count > 0) {
            while (ref_count) {
                if (!soc_profile_mem_delete(unit, _bcm_ap_cos_map_profile[unit], index)) {
                    ref_count -= 1;
                }
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

    soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[14],
                               HG_COSf, 8);
    soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[15],
                               HG_COSf, 9);

    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        
        if (IS_HG_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit],
                                     hg_entries, 16, &hg_index));
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        hg_index / 16));
        } else {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit],
                                     entries, 16, &index));
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        index / 16));
        }
    }

    cpu_hg_index = SOC_INFO(unit).cpu_hg_index;
    if (cpu_hg_index != -1) {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit], 
                                     entries, 16, &index));
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                cpu_hg_index, SELECTf, index / 16));
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
    for (prio = numq; prio < 16; prio++) {
        soc_mem_field32_set(unit, VOQ_COS_MAPm, &voq_cos_map, VOQ_COS_OFFSETf, numq - 1);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, VOQ_COS_MAPm, MEM_BLOCK_ANY, prio,
                          &voq_cos_map));
    }

    _AP_NUM_COS(unit) = numq;

    return BCM_E_NONE;
}

STATIC int 
_bcm_ap_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
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
        (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));

    *p_pool_start = *p_pool_end = pool;
    return BCM_E_NONE;
}

STATIC int 
_bcm_ap_cosq_ingress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp, 
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
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

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
_bcm_ap_cosq_ingress_pg_get(int unit, bcm_gport_t gport, bcm_cos_queue_t pri, 
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
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

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
_bcm_ap_bst_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                    bcm_bst_stat_id_t bid, int *pipe, int *start_hw_index, 
                    int *end_hw_index, int *rcb_data, void **cb_data, int *bcm_rv)
{
    int phy_port, mmu_port, local_port;
    _bcm_ap_cosq_node_t *node = NULL;
    soc_info_t *si;
    uint32 rval = 0;
    int pool_idx;

    *bcm_rv = BCM_E_NONE;

#define _BST_IF_ERROR_GOTO_EXIT(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) { goto error_exit; } } while(0)

    _BST_IF_ERROR_GOTO_EXIT
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    *pipe = 0;

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
        _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_egress_sp_get(unit, gport, cosq, 
                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdEgrMCastPool) {
        /* Egress Multicast Pool */
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            goto error_exit;
        }
        _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_egress_sp_get(unit, gport, cosq, 
                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;        
    } else if (bid == bcmBstStatIdIngPool) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_ingress_sp_get(unit, gport, cosq, 
                    start_hw_index, end_hw_index));
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdRQEPool) {
        /* RQE pool */
        if (cosq == BCM_COS_INVALID) {
            *start_hw_index = 0;
            *end_hw_index = _APACHE_MMU_NUM_POOL - 1;
        }else if ((cosq < 0) || (cosq >= _APACHE_MMU_NUM_RQE_QUEUES)) {
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
            *end_hw_index = _APACHE_MMU_NUM_RQE_QUEUES - 1;
        }else if ((cosq < 0) || (cosq >= _APACHE_MMU_NUM_RQE_QUEUES)) {
            return BCM_E_PARAM;
        } else {
            *start_hw_index = *end_hw_index = cosq;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdUCQueueGroup) {
        /* unicast queue group */
        _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit,
                    gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE_GROUP,
                    NULL, start_hw_index, NULL));
        *end_hw_index = *start_hw_index;
        return _BCM_BST_RV_OK;
    }

    if (bid == bcmBstStatIdPortPool) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_ap_cosq_ingress_sp_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port) * 4) + *start_hw_index;
        *end_hw_index = ((mmu_port) * 4) + *end_hw_index;
    } else if ((bid == bcmBstStatIdPriGroupShared) || 
               (bid == bcmBstStatIdPriGroupHeadroom)) {
        /* ingress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_ap_cosq_ingress_pg_get(unit, gport, cosq, 
                                    start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port) * 8) + *start_hw_index;
        *end_hw_index = ((mmu_port) * 8) + *end_hw_index;
    } else if (bid == bcmBstStatIdEgrPortPoolSharedUcast ||
               bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        /* egress pool */
        _BST_IF_ERROR_GOTO_EXIT(
                _bcm_ap_cosq_egress_sp_get(unit, gport, cosq,
                                           start_hw_index, end_hw_index));
        *start_hw_index = ((mmu_port & 0x3f) * _APACHE_MMU_NUM_POOL) +
                          *start_hw_index;
        *end_hw_index = ((mmu_port & 0x3f) * _APACHE_MMU_NUM_POOL) +
                        *end_hw_index;
    } else {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (bid != bcmBstStatIdUcast) {
                goto error_exit;
            }
            _BST_IF_ERROR_GOTO_EXIT
                (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
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
                (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
                                        &local_port, NULL, &node));
            if (!node) {
                goto error_exit;
            }
            *start_hw_index = *end_hw_index = node->hw_index - _BCM_AP_NUM_L2_UC_LEAVES;
        } else { 
            _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_port_has_ets(unit, local_port));
            
            if (cosq < 0) {
                if (bid == bcmBstStatIdUcast) {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, 0, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, start_hw_index, NULL));

                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, NUM_COS(unit) - 1, 
                          _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, end_hw_index, NULL));

                } else {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, 0, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *start_hw_index -= _BCM_AP_NUM_L2_UC_LEAVES;

                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, NUM_COS(unit) - 1, 
                          _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, end_hw_index, NULL));
                    *end_hw_index -= _BCM_AP_NUM_L2_UC_LEAVES;
                }
            } else {
                if (bid == bcmBstStatIdUcast) {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *end_hw_index = *start_hw_index;
                } else {
                    _BST_IF_ERROR_GOTO_EXIT(_bcm_ap_cosq_index_resolve(unit, 
                          local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE, 
                          NULL, start_hw_index, NULL));
                    *start_hw_index -= _BCM_AP_NUM_L2_UC_LEAVES;
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
_bcm_ap_cosq_bst_map_resource_to_gport_cos(int unit, bcm_bst_stat_id_t bid, int port, 
                         int index, bcm_gport_t *gport, bcm_cos_t *cosq)
{
    soc_info_t *si;
    _bcm_ap_cosq_node_t *node;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t   *pres;
    int ii, pipe;
    int hw_index, mmu_port, phy_port;

    pipe = _AP_XPIPE;
    mmu_info = _bcm_ap_mmu_info[unit];
    pres = _BCM_AP_PRESOURCES(mmu_info, pipe);
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
        *cosq = index % _APACHE_MMU_NUM_RQE_QUEUES;
        break;

    case bcmBstStatIdUCQueueGroup:
        *gport = index;
        *cosq =  -1;
        break;

    case bcmBstStatIdPortPool:
    case bcmBstStatIdEgrPortPoolSharedUcast:
    case bcmBstStatIdEgrPortPoolSharedMcast:
        mmu_port = index/_APACHE_MMU_NUM_POOL;
        phy_port = si->port_m2p_mapping[mmu_port];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % _APACHE_MMU_NUM_POOL;
        break;

    case bcmBstStatIdPriGroupHeadroom:
    case bcmBstStatIdPriGroupShared:
        mmu_port = index/8;
        phy_port = si->port_m2p_mapping[mmu_port];
        *gport = si->port_p2l_mapping[phy_port];
        *cosq = index % 8;
        break;

    case bcmBstStatIdMcast:
        pipe = _AP_XPIPE;
        hw_index = soc_apache_l2_hw_index(unit, index, 0);

        if (mmu_info->ets_mode) {
            for (ii = 0; ii < _BCM_AP_NUM_L2_MC_LEAVES; ii++) {
                node = &pres->p_queue_node[ii];
                if (node->in_use == TRUE) {

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
        pipe = _AP_XPIPE;
        hw_index = soc_apache_l2_hw_index(unit, index, 1);

        if (mmu_info->ets_mode) {
            for (ii = 0; ii < _BCM_AP_NUM_L2_UC_LEAVES; ii++) {
                node = &pres->p_queue_node[ii];
                if (node->in_use == TRUE) {

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
    default:
        break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cpu_lls_config_set(int unit, int port)
{
    bcm_gport_t port_sched, s1_sched, l0_sched;
    bcm_gport_t l1_sched;
    bcm_gport_t mc_cosq;
    int i, j, k, l2_offset;
    uint32 num_l0[3];
    uint32 num_l1[3];
    uint32 num_l2[3]; /* 0 = PCI ; 1 = UC0; 2 = UC1 */
    _bcm_ap_mmu_info_t *mmu_info;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
         return BCM_E_INIT;
    }

    num_l2[1] = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 0));
    num_l2[2] = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 1));
    num_l2[0] = 48 - (num_l2[1] + num_l2[2]);

    num_l1[0] = (num_l2[0] + 7) / 8;
    num_l1[1] = (num_l2[1] + 7) / 8;
    num_l1[2] = (num_l2[2] + 7) / 8;

    num_l0[0] = 1;
    num_l0[1] = (num_l1[1] ? 1 : 0);
    num_l0[2] = (num_l1[2] ? 1 : 0);

    _BCM_DEFAULT_LLS_SET(mmu_info);

    if (soc_feature(unit, soc_feature_mmu_hqos_four_level))  {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 1, 0, &port_sched));
 
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 3,
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &s1_sched));
         BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, s1_sched, port_sched, 0));
    } else {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 3, 0, &port_sched));
    }
    for (i = 0; i < 3; i++) {
        if (num_l0[i] == 0) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, num_l1[i],
                                    BCM_COSQ_GPORT_SCHEDULER, 
                                    &l0_sched));
        if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_attach(unit, l0_sched, s1_sched, i)); 
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_attach(unit, l0_sched, port_sched, i)); 
        }
        for (j = 0; j < num_l1[i]; j++) {
            l2_offset = (num_l2[i] - (j * 8));
            l2_offset = (l2_offset > 8) ? 8 : l2_offset;
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_add(unit, port, l2_offset,
                                    BCM_COSQ_GPORT_SCHEDULER, 
                                    &l1_sched));

            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_attach(unit, l1_sched, l0_sched, j));

            for (k = 0; k < l2_offset; k++) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                        &mc_cosq));
                BCM_IF_ERROR_RETURN
                    (bcm_ap_cosq_gport_attach(unit, mc_cosq, l1_sched, k));
            }
        }

    }
    _BCM_DEFAULT_LLS_RESET(mmu_info);
    return BCM_E_NONE;
}
int
_bcm_ap_port_lls_config_set(int unit, 
                             bcm_port_t port) 
{
    bcm_gport_t port_sched, s1_sched, l0_sched;
    bcm_gport_t l1_sched;
    bcm_gport_t uc_cosq, mc_cosq;
    int i;
    uint32 num_of_l1 = 10; 
    uint32 num_of_l2_uc = 1; 
    uint32 num_of_l2_mc = 1;
    int uc, mc;
    int skip_lls = 0; 
    _bcm_ap_mmu_info_t *mmu_info;
    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
         return BCM_E_INIT;
    }

    skip_lls = soc_ap_is_skip_default_lls_creation(unit); 
    if (skip_lls && !IS_CPU_PORT(unit, port) && !IS_LB_PORT(unit, port)) 
    {
        BCM_IF_ERROR_RETURN(
         _bcm_ap_port_enqueue_set(unit , port, 0)); 
         return BCM_E_NONE; 
    }
    if(IS_CPU_PORT(unit, port)) {
        return _bcm_ap_cpu_lls_config_set(unit, port);
    }
    if(IS_LB_PORT(unit, port)) {
        num_of_l1 = 9;
        num_of_l2_uc = 0;
        num_of_l2_mc = 1;
    }
    _BCM_DEFAULT_LLS_SET(mmu_info);

    BCM_IF_ERROR_RETURN
        (bcm_ap_cosq_gport_add(unit, port, 1, 0, &port_sched));
 
    if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) 
    {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 1,
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &s1_sched));
         BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, s1_sched, port_sched, 0));
    } 
    BCM_IF_ERROR_RETURN
        (bcm_ap_cosq_gport_add(unit, port, num_of_l1,
                                BCM_COSQ_GPORT_SCHEDULER, 
                                &l0_sched));
    if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) 
    {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, l0_sched, s1_sched, 0)); 
    } else
    {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 
    }
    for (i = 0; i < num_of_l1; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, (num_of_l2_uc + num_of_l2_mc),
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &l1_sched));

        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, l1_sched, l0_sched, i));
        for (uc = 0; uc < num_of_l2_uc; uc++) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                       &uc_cosq));
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_attach(unit, uc_cosq, l1_sched, uc));
        }
        for (mc = uc; mc < (num_of_l2_uc + num_of_l2_mc); mc++) {
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                       &mc_cosq));
            BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_attach(unit, mc_cosq, l1_sched, mc));
        }
    }
    _BCM_DEFAULT_LLS_RESET(mmu_info);
    return BCM_E_NONE;
} 

int
_bcm_ap_port_hsp_config_set(int unit, 
                             bcm_port_t port) 
{
    bcm_gport_t port_sched, l0_sched[5];
    bcm_gport_t l1_sched;
    bcm_gport_t uc_cosq, mc_cosq;
    int i;
    uint32 num_of_l0 = 5; 
    uint32 num_of_l1 = 10; 
    int index;


    BCM_IF_ERROR_RETURN
        (bcm_ap_cosq_gport_add(unit, port, num_of_l0 , 0, &port_sched));
 
    for (i = 0; i < num_of_l0; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 8,
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &l0_sched[i]));
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, l0_sched[i], port_sched, i));
    }

    for (i = 0; i < num_of_l1; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 2,
                                   BCM_COSQ_GPORT_SCHEDULER, 
                                   &l1_sched));
        index = i < 8 ? 1 : 4;

        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, l1_sched, l0_sched[index],
                                      i >= 8 ? i - 8 :i));
       BCM_IF_ERROR_RETURN
                (bcm_ap_cosq_gport_sched_set(unit, l0_sched[index], (i >= 8 ? i - 8 :i),
                                          BCM_COSQ_DEFICIT_ROUND_ROBIN, 1));
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                   &uc_cosq));
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, uc_cosq, l1_sched, 0));
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_add(unit, port, 1, BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                   &mc_cosq));
        BCM_IF_ERROR_RETURN
            (bcm_ap_cosq_gport_attach(unit, mc_cosq, l1_sched, 1));
    }
    return BCM_E_NONE;
}

int 
bcm_ap_init_fc_map_tbl(int unit) 
{
    int max_index;
    int max_nodes; 
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    int mem;
    int index;
    int field ;
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f, INDEX2f, INDEX3f
    };
    static const soc_mem_t memx[] = {
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };
    static const soc_mem_t memy[] = {
        MMU_INTFI_FC_ST_TBL0m,
        MMU_INTFI_FC_ST_TBL1m,
        MMU_INTFI_FC_ST_TBL2m,
    };
 
    for ( mem = 0 ; mem < 3 ; mem++) 
    { 
       max_index = soc_mem_index_count(unit, memy[mem]) - 1;
       max_nodes = soc_mem_index_count(unit,memx[mem]); 
       for ( index = 0 ; index < max_nodes ; index++) 
       {
         SOC_IF_ERROR_RETURN(soc_mem_read(unit, memx[mem],
                MEM_BLOCK_ALL, index, &map_entry));
         for (field = 0; field  <  COUNTOF(indexf); field++)
         { 
              soc_mem_field32_set(unit, memx[mem],
                                   &map_entry, indexf[field], max_index);
         }
         SOC_IF_ERROR_RETURN(soc_mem_write(unit, memx[mem],
                        MEM_BLOCK_ALL, index, &map_entry));
       }
     } 
     return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_ap_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_init(int unit)
{
    int numq, alloc_size, ii, index;
    soc_info_t *si;
    bcm_port_t port;
    soc_reg_t mem;
    soc_reg_t reg;
    int count;
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_CURVE_PROFILE_0_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_1_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_2_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_3_X_PIPEm,
        MMU_WRED_DROP_CURVE_PROFILE_4_X_PIPEm, 
        MMU_WRED_DROP_CURVE_PROFILE_5_X_PIPEm
    };

    int entry_words[6], pipe;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    uint32 profile_index, rval32;
    _bcm_ap_mmu_info_t *mmu_info;
    int i, qnum, wred_prof_count;
    _bcm_bst_device_handlers_t bst_callbks;
    _bcm_ap_pipe_resources_t   *pres;
    uint64 rval64;
    _bcm_ap_cosq_list_t *list = NULL;
    int baseq = 0;
    int numl2uc = 0; 
    int phy_port ,mmu_port ; 
    uint64 rval ; 

    BCM_IF_ERROR_RETURN (bcm_ap_cosq_detach(unit, 0));

    si = &SOC_INFO(unit);

    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    NUM_COS(unit) = numq;
    soc_apache_fc_map_shadow_create(unit);

    /*Initialize time_domain1 on this unit */
    time_domain1[unit][0].field = TIME_0f;
    time_domain1[unit][1].field = TIME_1f;
    time_domain1[unit][2].field = TIME_2f;
    time_domain1[unit][3].field = TIME_3f;
    time_domain1[unit][0].ref_count = 20780;
    time_domain1[unit][1].ref_count = 0;
    time_domain1[unit][2].ref_count = 0;
    time_domain1[unit][3].ref_count = 0;

    /* Create profile for PORT_COS_MAP table */
    if (_bcm_ap_cos_map_profile[unit] == NULL) {
        _bcm_ap_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_ap_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_ap_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_ap_cos_map_profile[unit]));

    /* Create profile for IFP_COS_MAP table. */
    if (_bcm_ap_ifp_cos_map_profile[unit] == NULL) {
        _bcm_ap_ifp_cos_map_profile[unit]
            = sal_alloc(sizeof(soc_profile_mem_t), "IFP_COS_MAP Profile Mem");
        if (_bcm_ap_ifp_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_ap_ifp_cos_map_profile[unit]);
    }
    mem = IFP_COS_MAPm;
    entry_words[0] = sizeof(ifp_cos_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_ap_ifp_cos_map_profile[unit]));

    /* Create profile for SERVICE_COS_MAP table */
    if (_bcm_ap_service_cos_map_profile[unit] == NULL) {
        alloc_size = sizeof(soc_profile_mem_t);
        _bcm_ap_service_cos_map_profile[unit] = sal_alloc(alloc_size,
                                                           "SERVICE_COS_MAP");
        if (_bcm_ap_service_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_ap_service_cos_map_profile[unit]);
    }
    mem = SERVICE_COS_MAPm;
    entry_words[0] = sizeof(service_cos_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_create(unit, &mem, entry_words, 1,
                               _bcm_ap_service_cos_map_profile[unit]));

    /* Create profile for SERVICE_PORT_MAP table */
    if (_bcm_ap_service_port_map_profile[unit] == NULL) {
        alloc_size = sizeof(soc_profile_mem_t);
        _bcm_ap_service_port_map_profile[unit] = sal_alloc(alloc_size,
                                                            "SERVICE_PORT_MAP");
        if (_bcm_ap_service_port_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_ap_service_port_map_profile[unit]);
    }

    mem = SERVICE_PORT_MAPm;
    entry_words[0] = sizeof(service_port_map_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(
        soc_profile_mem_create(unit, &mem, entry_words, 1,
                               _bcm_ap_service_port_map_profile[unit]));

    alloc_size = sizeof(_bcm_ap_mmu_info_t) * 1;
    if (_bcm_ap_mmu_info[unit] == NULL) {
        _bcm_ap_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_ap_mmu_info");

        if (_bcm_ap_mmu_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(_bcm_ap_mmu_info[unit], 0, alloc_size);
    }

    /* Create profile for PRIO2COS_PROFILE register */
    if (_bcm_ap_llfc_profile[unit] == NULL) {
        _bcm_ap_llfc_profile[unit] =
        sal_alloc(sizeof(soc_profile_reg_t), "PRIO2COS_LLFC Profile Reg");
        if (_bcm_ap_llfc_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_ap_llfc_profile[unit]);
    }
    reg = PRIO2COS_PROFILEr;
    BCM_IF_ERROR_RETURN
         (soc_profile_reg_create(unit, &reg, 1, _bcm_ap_llfc_profile[unit]));

    mmu_info = _bcm_ap_mmu_info[unit];
    mmu_info->ets_mode = 0;
    mmu_info->pipe_resources[_AP_XPIPE].num_base_queues = 0;
     
    if (soc_feature(unit, soc_feature_mmu_3k_uc_queue)) {
       mmu_info->num_l2_queues = _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE; 
    } else if (soc_feature(unit, soc_feature_mmu_1k_uc_queue)) {
       mmu_info->num_l2_queues = _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE; 
    } else { 
       mmu_info->num_l2_queues = _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE; 
    }

    for (pipe = _AP_XPIPE; pipe < _AP_YPIPE; pipe++) {
        pres = _BCM_AP_PRESOURCES(mmu_info, pipe);
        pres->p_queue_node = &mmu_info->queue_node[0];
        pres->p_mc_queue_node = &mmu_info->mc_queue_node[0];
        pres->l2_queue_list.count = mmu_info->num_l2_queues;
        pres->l2_queue_list.bits = _bcm_ap_cosq_alloc_clear(
                pres->l2_queue_list.bits,
                SHR_BITALLOCSIZE(mmu_info->num_l2_queues), 
                                    "l2_queue_list");
        if (pres->l2_queue_list.bits == NULL) {
            _bcm_ap_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }

        pres->l0_sched_list.count = _BCM_AP_NUM_L0_SCHEDULER_PER_PIPE;
        pres->l0_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                pres->l0_sched_list.bits,
                SHR_BITALLOCSIZE(_BCM_AP_NUM_L0_SCHEDULER_PER_PIPE),
                                    "l0_sched_list");
        if (pres->l0_sched_list.bits == NULL) {
            _bcm_ap_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }

        pres->l1_sched_list.count = _BCM_AP_NUM_L1_SCHEDULER_PER_PIPE;
        pres->l1_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                pres->l1_sched_list.bits,
                SHR_BITALLOCSIZE(_BCM_AP_NUM_L1_SCHEDULER_PER_PIPE),
                                    "l1_sched_list");
        if (pres->l1_sched_list.bits == NULL) {
            _bcm_ap_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }
        pres->s1_sched_list.count = _BCM_AP_NUM_S1_SCHEDULER_PER_PIPE;
        pres->s1_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                pres->s1_sched_list.bits,
                SHR_BITALLOCSIZE(_BCM_AP_NUM_S1_SCHEDULER_PER_PIPE),
                "s1_sched_list");
        if (pres->s1_sched_list.bits == NULL) {
            _bcm_ap_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }

    }
    /* assign queues to ports */
    PBMP_ALL_ITER(unit, port) {
        pipe = _BCM_AP_PORT_TO_PIPE(unit, port);
       
        if ((SOC_WARM_BOOT(unit) && !IS_LB_PORT(unit, port)) && 
                          ! IS_CPU_PORT(unit, port)) {
            numl2uc =  IS_AP_HSP_PORT(unit, port) ? 10 : 16;
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            COMPILER_64_ZERO(rval);
            BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, port, &rval));
            baseq = soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, 
                                  BASE_QUEUE_NUM_0f);
           si->port_num_cosq[port] = 10;
           si->port_cosq_base[port] =  (mmu_port * si->port_num_cosq[port]);
           si->port_num_uc_cosq[port] = numl2uc ;
           si->port_uc_cosq_base[port] = baseq;   
        }
        mmu_info->port_info[port].resources = &mmu_info->pipe_resources[pipe];
        mmu_info->port_info[port].mc_base = si->port_cosq_base[port];
        mmu_info->port_info[port].mc_limit = si->port_cosq_base[port] + 
                                                si->port_num_cosq[port];

        mmu_info->port_info[port].uc_base = si->port_uc_cosq_base[port];
        mmu_info->port_info[port].uc_limit = si->port_uc_cosq_base[port] + 
                                                si->port_num_uc_cosq[port];
        if (mmu_info->port_info[port].resources->num_base_queues <
                                      mmu_info->port_info[port].uc_limit) {
            mmu_info->port_info[port].resources->num_base_queues =
                                                 mmu_info->port_info[port].uc_limit;
        } 
        if (!SOC_WARM_BOOT(unit)) {
            COMPILER_64_ZERO(rval64);
            if(IS_AP_HSP_PORT(unit, port)) {
                SOC_IF_ERROR_RETURN(soc_apache_sched_hw_index_get(unit,
                            port, SOC_APACHE_NODE_LVL_L1, 0, &qnum));
            } else {
                qnum = soc_apache_logical_qnum_hw_qnum(unit, port, 
                        si->port_uc_cosq_base[port], 1);
            }
            soc_reg64_field32_set(unit, ING_COS_MODE_64r, 
                                            &rval64, BASE_QUEUE_NUM_0f, qnum);
            soc_reg64_field32_set(unit, ING_COS_MODE_64r, 
                                            &rval64, BASE_QUEUE_NUM_1f, qnum);
            BCM_IF_ERROR_RETURN(soc_reg_set(unit, ING_COS_MODE_64r, port, 0, rval64));
        }
    }


    for (i = 0; i < _BCM_AP_NUM_TOTAL_SCHEDULERS; i++) {
        _BCM_AP_COSQ_LIST_NODE_INIT(mmu_info->sched_node, i);
    }

    for (i = 0; i < _BCM_AP_NUM_L2_UC_LEAVES; i++) {
        _BCM_AP_COSQ_LIST_NODE_INIT(mmu_info->queue_node, i);
    }

    for (i = 0; i < _BCM_AP_NUM_L2_MC_LEAVES; i++) {
        _BCM_AP_COSQ_LIST_NODE_INIT(mmu_info->mc_queue_node, i);
    }

    for (pipe = _AP_XPIPE; pipe < _AP_YPIPE; pipe++) {
        pres = _BCM_AP_PRESOURCES(mmu_info, pipe);
        for (i = SOC_APACHE_NODE_LVL_L0; i <= SOC_APACHE_NODE_LVL_L2; i++) {
            index = _soc_apache_invalid_parent_index(unit, i);
            if (i == SOC_APACHE_NODE_LVL_L0) {
                list = &pres->s1_sched_list;
            } else if (i == SOC_APACHE_NODE_LVL_L1) {
                list = &pres->l0_sched_list;
            } else if (i == SOC_APACHE_NODE_LVL_L2) {
                list = &pres->l1_sched_list;
            } else {
                continue;
            }
            if (index >= 0) {
                _bcm_ap_node_index_set(list, index, 1);
            }
        }
    }

    /* reserve the resources for HSP ports */
    count = _BCM_AP_HSP_PORT_MAX_ROOT;

    for (pipe = _AP_XPIPE; pipe < _AP_YPIPE; pipe++) {
        pres = _BCM_AP_PRESOURCES(mmu_info, pipe);
        _bcm_ap_node_index_set(&pres->l0_sched_list, 0, 5 * count);
        _bcm_ap_node_index_set(&pres->l1_sched_list, 0, 10 * count);
        _bcm_ap_node_index_set(&pres->l2_queue_list, 0, 10 * count);

        pres->cpu_l0_sched_list.count = (_BCM_AP_CPU_PORT_L0_MAX - 
                _BCM_AP_CPU_PORT_L0_MIN);
        pres->cpu_l0_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                pres->cpu_l0_sched_list.bits,
                SHR_BITALLOCSIZE(pres->cpu_l0_sched_list.count),
                "cpu_l0_sched_list");
        if (pres->cpu_l0_sched_list.bits == NULL) {
            _bcm_ap_cosq_free_memory(mmu_info);
            return BCM_E_MEMORY;
        }
        _bcm_ap_node_index_set(&pres->l0_sched_list, _BCM_AP_CPU_PORT_L0_MIN,
                pres->cpu_l0_sched_list.count);




        if (count > 0) {
            pres->hsp_l0_sched_list.count = 5 * count;
            pres->hsp_l0_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                    pres->hsp_l0_sched_list.bits,
                    SHR_BITALLOCSIZE(pres->hsp_l0_sched_list.count),
                    "hsp_l0_sched_list");
            if (pres->hsp_l0_sched_list.bits == NULL) {
                _bcm_ap_cosq_free_memory(mmu_info);
                return BCM_E_MEMORY;
            }
            pres->hsp_l1_sched_list.count = 10 * count;
            pres->hsp_l1_sched_list.bits = _bcm_ap_cosq_alloc_clear(
                    pres->hsp_l1_sched_list.bits,
                    SHR_BITALLOCSIZE(pres->hsp_l1_sched_list.count),
                    "hsp_l1_sched_list");
            if (pres->hsp_l1_sched_list.bits == NULL) {
                _bcm_ap_cosq_free_memory(mmu_info);
                return BCM_E_MEMORY;
            }
            pres->hsp_l2_queue_list.count = 10 * count;
            pres->hsp_l2_queue_list.bits = _bcm_ap_cosq_alloc_clear(
                    pres->hsp_l2_queue_list.bits,
                    SHR_BITALLOCSIZE(pres->hsp_l2_queue_list.count),
                    "hsp_l2_queue_list");
            if (pres->hsp_l2_queue_list.bits == NULL) {
                _bcm_ap_cosq_free_memory(mmu_info);
                return BCM_E_MEMORY;
            }
        }
    }
      
    for (pipe = _AP_XPIPE; pipe < _AP_YPIPE; pipe++) {
        pres = _BCM_AP_PRESOURCES(mmu_info, pipe);
        PBMP_ALL_ITER(unit, port) {
            _bcm_ap_node_index_set(&pres->l2_queue_list, 
                    mmu_info->port_info[port].uc_base,
                    si->port_num_uc_cosq[port]);
        }
    }

    /* Initialize oob driver */
    BCM_IF_ERROR_RETURN(_bcm_oob_init(unit));
        
    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_ap_wred_profile[unit] == NULL) {
        _bcm_ap_wred_profile[unit] = 
            sal_alloc(sizeof(soc_profile_mem_t), "WRED Profile Mem");
        if (_bcm_ap_wred_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_ap_wred_profile[unit]);
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
        (soc_profile_mem_create(unit, wred_mems, entry_words, 6,
                                _bcm_ap_wred_profile[unit]));

        /* Create profile for MMU_QCN_SITB table */
        if (_bcm_ap_sample_int_profile[unit] == NULL) {
            _bcm_ap_sample_int_profile[unit] =
                sal_alloc(sizeof(soc_profile_mem_t), "QCN sample Int Profile Mem");
            if (_bcm_ap_sample_int_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(_bcm_ap_sample_int_profile[unit]);
        }
        mem = MMU_QCN_SITBm;
        entry_words[0] = sizeof(mmu_qcn_sitb_entry_t) / sizeof(uint32);
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                    _bcm_ap_sample_int_profile[unit]));

        /* Create profile for MMU_QCN_CPQ_SEQ register */
        if (_bcm_ap_feedback_profile[unit] == NULL) {
            _bcm_ap_feedback_profile[unit] =
                sal_alloc(sizeof(soc_profile_reg_t), "QCN Feedback Profile Reg");
            if (_bcm_ap_feedback_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_reg_t_init(_bcm_ap_feedback_profile[unit]);
        }
        reg = MMU_QCN_CPQ_SEQr;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_create(unit, &reg, 1,
                                    _bcm_ap_feedback_profile[unit]));


        /* Create profile for VOQ_MOD_MAP table */
        if (_bcm_ap_voq_port_map_profile[unit] == NULL) {
            _bcm_ap_voq_port_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                    "VOQ_PORT_MAP Profile Mem");
            if (_bcm_ap_voq_port_map_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(_bcm_ap_voq_port_map_profile[unit]);
        }
        mem = VOQ_PORT_MAPm;
        entry_words[0] = sizeof(voq_port_map_entry_t) / sizeof(uint32);
        BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                    _bcm_ap_voq_port_map_profile[unit]));

    _BCM_DEFAULT_LLS_RESET(mmu_info);
    SOC_IF_ERROR_RETURN(READ_LLS_FC_CONFIGr(unit, &rval32));
    soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval32, FC_CFG_DISABLE_ALL_XOFFf, 0);
    SOC_IF_ERROR_RETURN(WRITE_LLS_FC_CONFIGr(unit, rval32));

    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &_bcm_ap_bst_index_resolve;
    bst_callbks.reverse_resolve_index = &_bcm_ap_cosq_bst_map_resource_to_gport_cos;
    BCM_IF_ERROR_RETURN(_bcm_bst_attach(unit, &bst_callbks));


    /* Endpoint queuing initialization */
    if (soc_feature(unit, soc_feature_endpoint_queuing)) {
        if (_bcm_ap_endpoint_queuing_info[unit] == NULL) {
            _bcm_ap_endpoint_queuing_info[unit] =
                sal_alloc(sizeof(_bcm_ap_endpoint_queuing_info_t),
                        "Endpoint Queuing Info");
            if (_bcm_ap_endpoint_queuing_info[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_ap_endpoint_queuing_info[unit], 0,
                    sizeof(_bcm_ap_endpoint_queuing_info_t));

            /* The maximum number of endpoints is limited to 2^14,
             * since the endpoint ID field is limited to 14 bits in
             * Higig2 header.
             */
            _BCM_AP_NUM_ENDPOINT(unit) = 1 << 14;
            _bcm_ap_endpoint_queuing_info[unit]->ptr_array = sal_alloc(
                    _BCM_AP_NUM_ENDPOINT(unit) * sizeof(_bcm_ap_endpoint_t *),
                    "Endpoint Pointer Array");
            if (_bcm_ap_endpoint_queuing_info[unit]->ptr_array == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_ap_endpoint_queuing_info[unit]->ptr_array, 0,
                    _BCM_AP_NUM_ENDPOINT(unit) * sizeof(_bcm_ap_endpoint_t *));

            /* Reserve endpoint 0 */
            _bcm_ap_endpoint_queuing_info[unit]->ptr_array[0] = sal_alloc(
                    sizeof(_bcm_ap_endpoint_t), "Endpoint Info");
            if (_bcm_ap_endpoint_queuing_info[unit]->ptr_array[0] == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_ap_endpoint_queuing_info[unit]->ptr_array[0], 0,
                    sizeof(_bcm_ap_endpoint_t));


            _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile = sal_alloc(
                    sizeof(soc_profile_mem_t), "Priority to CoS Map Profile");
            if (_bcm_ap_endpoint_queuing_info[unit]->cos_map_profile == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init
                (_bcm_ap_endpoint_queuing_info[unit]->cos_map_profile);
            mem = ENDPOINT_COS_MAPm;
            entry_words[0] = sizeof(endpoint_cos_map_entry_t) / sizeof(uint32);
            SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                        _bcm_ap_endpoint_queuing_info[unit]->cos_map_profile));
        }
    }

    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_ap_cosq_cpu_cosq_config[unit][i] == NULL) { 
            _bcm_ap_cosq_cpu_cosq_config[unit][i] =
                sal_alloc(sizeof(_bcm_ap_cosq_cpu_cosq_config_t), "CPU Cosq Config");

            if (_bcm_ap_cosq_cpu_cosq_config[unit][i] == NULL) { 
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_ap_cosq_cpu_cosq_config[unit][i], 0,
                       sizeof(_bcm_ap_cosq_cpu_cosq_config_t));
            _bcm_ap_cosq_cpu_cosq_config[unit][i]->enable = 1;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_ap_cosq_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    PBMP_ALL_ITER(unit, port) {
        if ((port == 74) || IS_AP_HSP_PORT( unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN (_bcm_ap_port_lls_config_set(unit, port));
    }
    PBMP_ALL_ITER(unit, port) {
        if (!IS_AP_HSP_PORT( unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_apache_cosq_enable_hsp_sched(unit,port));
        BCM_IF_ERROR_RETURN (_bcm_ap_port_hsp_config_set(unit, port));
    }
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
        soc_mem_field32_set(unit, wred_mems[ii], entries[ii], 
                MIN_THDf, 0x1ffff);
        soc_mem_field32_set(unit, wred_mems[ii], entries[ii], 
                MAX_THDf, 0x1ffff);
    }
    profile_index = 0xffffffff;
    wred_prof_count = soc_mem_index_count(unit, MMU_WRED_CONFIG_X_PIPEm);
    while(wred_prof_count) {
        if (profile_index == 0xffffffff) {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, 
                                        _bcm_ap_wred_profile[unit],
                                        entries, 1, &profile_index));
        } else {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit,
                                            _bcm_ap_wred_profile[unit],
                                            profile_index, 0));
        }
        wred_prof_count -= 1;
    }
     /* Add default entries for PRIO2COS_PROFILE register profile */
     PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN(bcm_ap_cosq_default_llfc_profile_attach(unit, port));
     }
    /* Add default entries for PORT_COS_MAP memory profile */
    BCM_IF_ERROR_RETURN(bcm_ap_cosq_config_set(unit, numq));

    /* Starting the curr_merger_index from 2,
     *  since 0 is configured in all the FC_MAP_TBL2 by default,
     *  so it might effect the traffic of other queues
     */
    mmu_info->curr_merger_index = 2;
    if (!mmu_info->curr_shared_limit) {
        BCM_IF_ERROR_RETURN(soc_apache_mmu_get_shared_size(unit,
                      &mmu_info->curr_shared_limit));
    }
    BCM_IF_ERROR_RETURN(bcm_ap_init_fc_map_tbl(unit)); 
    /* reset time_domain ref_count so that entries can be added upon
     * rc */
    time_domain1[unit][0].ref_count = 20780;
    time_domain1[unit][1].ref_count = 0;
    time_domain1[unit][2].ref_count = 0;
    time_domain1[unit][3].ref_count = 0;
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_ap_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_config_get(int unit, int *numq)
{
    if (numq != NULL) {
        *numq = _AP_NUM_COS(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_mapping_set
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
_bcm_ap_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port, outport = -1;
    bcm_cos_queue_t hw_cosq;
    soc_field_t field = INVALIDf, field2 = INVALIDf, field3 = INVALIDf;
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];
    uint32 old_index, new_index, max_val;
    voq_cos_map_entry_t voq_cos_map;
    int rv;
    int valid = 0;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport != -1) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, gport, &outport));
    }


    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = cosq;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_COS,
                  NULL, &hw_cosq, NULL));

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
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_COS,
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
                (_bcm_ap_cosq_index_resolve
                 (unit, gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_COS,
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

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                    SELECTf);
    old_index *= 16;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_ap_cos_map_profile[unit],
                             old_index, 16, entries));
    max_val = soc_property_port_get(unit, local_port, spn_LLS_NUM_L2UC, 16);
    if (hw_cosq < 0 || hw_cosq >= max_val) {
        return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority], field,
                        hw_cosq);
    if (field2 != INVALIDf) {
    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                        field2, hw_cosq);
    }
    if (field3 != INVALIDf) {
    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                        field3, hw_cosq);
    }

    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_delete(unit, _bcm_ap_cos_map_profile[unit],
                                old_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_ap_cos_map_profile[unit], entries,
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
 *     bcm_ap_cosq_mapping_set
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
bcm_ap_cosq_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
                         bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp, pbmp1;
    bcm_port_t local_port;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(pbmp1);

    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        BCM_PBMP_ASSIGN(pbmp1, PBMP_HG_ALL(unit));
        BCM_PBMP_REMOVE(pbmp, pbmp1);
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
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
    }

    if ((cosq < 0) || (cosq >= _AP_NUM_COS(unit))) {
        return BCM_E_PARAM;
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         * mcast */
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_mapping_set(unit, local_port, 
                                    priority,
                                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                    -1, cosq));
    }

    PBMP_ITER(pbmp1, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         * mcast */
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_mapping_set(unit, local_port, 
                                    priority,
                                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                    -1, cosq));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_mapping_get
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
_bcm_ap_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                          uint32 flags, bcm_gport_t *gport,
                          bcm_cos_queue_t *cosq)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_cos_queue_t hw_cosq = BCM_COS_INVALID;
    int index, ii;
    cos_map_sel_entry_t cos_map_sel_entry;
    voq_cos_map_entry_t voq_cos_map;
    void *entry_p;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport) {
        *gport = BCM_GPORT_INVALID;
    }

    *cosq = BCM_COS_INVALID;

     BCM_IF_ERROR_RETURN
            (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                               &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_ap_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    index + priority);

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    res = _BCM_AP_PRESOURCES(mmu_info, _BCM_AP_PORT_TO_PIPE(unit, local_port));

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, UC_COS1f);
        if ((IS_CPU_PORT(unit, local_port) == FALSE) && gport) { 
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
        if (gport) {
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
        if (gport) {
            for (ii = res->num_base_queues; ii < _BCM_AP_NUM_L2_UC_LEAVES; ii++) {
                node = &mmu_info->queue_node[ii];
                
                if (node->numq > 0 && node->hw_cosq == hw_cosq) {
                    *gport = node->gport;
                    break;
                }
            }
            if (ii == _BCM_AP_NUM_L2_UC_LEAVES) {
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
bcm_ap_cosq_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
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

        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_mapping_get(unit, local_port, 
                 priority, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL, cosq));
        break;
    }    
    return BCM_E_NONE;
}

int
bcm_ap_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    return _bcm_ap_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

int
bcm_ap_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    return _bcm_ap_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

/*
 * Function:
 *     bcm_ap_cosq_port_sched_set
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
bcm_ap_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    bcm_port_t port;
    int        num_weights, i;
    int        cur_mode, cur_weight; 
    int        reset_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN; 

    num_weights = _AP_NUM_COS(unit);
    BCM_PBMP_ITER(pbm, port) {
        /*WRR mode is a property per port, 
         * so just get cosq=0 schedule mode*/
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_sched_get(unit, port, 0,
                                     &cur_mode, &cur_weight));
        if (cur_mode == BCM_COSQ_STRICT) {
            reset_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        } else {
            reset_mode = cur_mode;
        }
        
        /* Schedule mode switch need base on default wrr mode */        
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_sched_set(unit, port, i, 
                                         reset_mode, 1));
        }
        
        for (i = 0; i < num_weights; i++) {         
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_sched_set(unit, port, i, mode, weights[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_port_sched_get
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
bcm_ap_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    bcm_port_t port;
    int i, num_weights;

    BCM_PBMP_ITER(pbm, port) {
        if (IS_CPU_PORT(unit, port) && SOC_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
            continue;
        }
        num_weights = _AP_NUM_COS(unit);
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_ap_cosq_sched_get(unit, port, i, mode, &weights[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_sched_weight_max_get
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
bcm_ap_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        /* AP - weights can be calculated based on LLS/HSP based 
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
 *     bcm_ap_cosq_bandwidth_set
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
bcm_ap_cosq_port_bandwidth_set(int unit, bcm_port_t port,
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

    return _bcm_ap_cosq_bucket_set(unit, port, cosq, min_quantum, max_quantum,
                                    burst_min, burst_max, flags);
}

/*
 * Function:
 *     bcm_ap_cosq_bandwidth_get
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
bcm_ap_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{
    uint32 kbit_burst_min;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_bucket_get(unit, port, cosq,
                        min_quantum, max_quantum, &kbit_burst_min,
                        burst_quantum, flags));
    return BCM_E_NONE;
}

int
bcm_ap_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst, burst_min, flags;
    bcm_port_t  local_port =  port;
    
    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_ap_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_l2_gport(unit, port, cosq, 
                                            _BCM_AP_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                    &burst, &flags));

    min = pps;
    burst_min = (min > 0) ?
          _bcm_td_default_burst_size(unit, local_port, min) : 0;
    burst = burst_min;        

    return _bcm_ap_cosq_bucket_set(unit, port, cosq, min, pps, burst_min, burst,
                                    flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_ap_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_ap_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_l2_gport(unit, port, cosq,
                                          _BCM_AP_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                  &flags));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_ap_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst)
{
    uint32 min, max, cur_burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_ap_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_l2_gport(unit, port, cosq,
                                                _BCM_AP_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst,
                                  &cur_burst, &flags));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_ap_cosq_bucket_set(unit, port, cosq, min, max, burst, burst,
                                    flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_ap_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst, cur_burst_min, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (_bcm_ap_cosq_port_has_ets(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_l2_gport(unit, port, cosq, 
                                            _BCM_AP_NODE_MCAST, &port, NULL));
        cosq = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, port, cosq, &min, &max, 
                                  &cur_burst_min, &cur_burst, &flags));
    *burst = cur_burst;

    return BCM_E_NONE;
}

int
bcm_ap_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    _bcm_ap_mmu_info_t *mmu_info;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }
    
    flags &= ~(BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL);

    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_wred_set(unit, port, 0, flags, 0, 0, 0, 0,
                                    FALSE, BCM_COSQ_DISCARD_PORT,8));
    }

    return BCM_E_NONE;
}

int
bcm_ap_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_ap_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL, BCM_COSQ_DISCARD_PORT,NULL);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_ap_cosq_discard_port_set
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
bcm_ap_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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

    if ( min_thresh == shared_limit ) {
       min_thresh = AP_CELL_FIELD_MAX; 
    }
    /* Calculate the max threshold. For a given slope (angle in
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_ap_angle_to_cells(drop_slope);
    if (max_thresh > AP_CELL_FIELD_MAX) {
        max_thresh = AP_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        numq = 1;
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_wred_set(unit, port, cosq + i, color,
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
                    (_bcm_ap_cosq_wred_set(unit, local_port, cosq + i,
                                           color, min_thresh, max_thresh, 100,
                                           gain, TRUE, 0,8));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_discard_port_get
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
bcm_ap_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
        (_bcm_ap_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                               &color, &min_thresh, &max_thresh, &drop_prob,
                               &gain, 0,&refresh_time));

    /*
     * average queue size is reculated every 4us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain) * refresh_time;
    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
    shared_limit = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
                                     rval, SHARED_LIMITf);
    if (shared_limit == 0) {
        return BCM_E_RESOURCE;
    }

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
    *drop_slope = _bcm_ap_cells_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_distribute_u64(uint64 value, uint64 *l, uint64 *r)
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
_bcm_ap_rqe_drop_counters_set(int unit, bcm_gport_t port,
        bcm_cos_queue_t cosq, bcm_cosq_stat_t stat, uint64 value) {
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        return BCM_E_PARAM;
    }
    if ((cosq < 0) || (cosq >= _APACHE_MMU_NUM_RQE_QUEUES)) {
        return BCM_E_PARAM;
    }
    if (stat == bcmCosqStatRQEGreenDroppedPackets) {
        /*Combnation of other stats*/
        return BCM_E_UNAVAIL;
    }

    if (stat == bcmCosqStatRQETotalDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_DROP_RQ_PKT;
    } else if (stat == bcmCosqStatRQETotalDroppedBytes) {
        ctr_reg = SOC_COUNTER_NON_DMA_DROP_RQ_BYTE;
    } else if (stat == bcmCosqStatRQERedDroppedPackets) {
        ctr_reg = SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED;
    } else { /* (stat == bcmCosqStatRQEYellowDroppedPackets) */
        ctr_reg = SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW;
    }

    ctrl_info.instance = 0;
    BCM_IF_ERROR_RETURN
        (soc_counter_generic_set(unit, ctr_reg,
                                 ctrl_info, 0, cosq, value));

    return BCM_E_NONE;
}

int
bcm_ap_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i, from_cos, to_cos, ci;
    uint64  l, r;
    uint64  max;
    soc_reg_t ctr_reg;
    soc_ctr_control_info_t ctrl_info;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_port_info_t *port_info;

    COMPILER_64_SET(max, 0, 0xffffffff);
    if ((cosq < -1) || COMPILER_64_GT(value, max)) {
        return BCM_E_PARAM;
    }
    _bcm_ap_distribute_u64(value, &l, &r);

    mmu_info = _bcm_ap_mmu_info[unit];

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
             BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        NULL, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, -1,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                    &local_port, &startq, NULL));
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base || 
                        node->hw_index >= port_info->uc_limit) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, 0,
                                 value));
                } else {
                   BCM_IF_ERROR_RETURN
                       (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, startq,
                                 value));
                }
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                         (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        NULL, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, -1,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                     &local_port, &startq, NULL));
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base || 
                          node->hw_index >= port_info->uc_limit) {
                   BCM_IF_ERROR_RETURN
                       (soc_counter_set(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, 0,
                                 value));
                } else {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, startq,
                                 value));
                } 
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                   SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, startq , value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                       (_bcm_ap_cosq_index_resolve
                        (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                    &local_port, &startq, NULL));
                    /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base 
                        || node->hw_index >= port_info->uc_limit) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, 0,
                                 value));
                } else {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, startq,
                                 value));
                } 
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
             BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        &local_port, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                    &local_port, &startq, NULL));
                 /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base || 
                         node->hw_index >= port_info->uc_limit) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, 0,
                                 value));
                } else {
                     BCM_IF_ERROR_RETURN
                         (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, startq,
                                 value));
                } 
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve
                (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                 &local_port, &startq, NULL));        
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM, startq, value));
        break;
    
    case bcmCosqStatHighPriDroppedPackets:
    case bcmCosqStatOBMLossyHighPriDroppedPackets:
    case bcmCosqStatLowPriDroppedPackets:
    case bcmCosqStatOBMLossyLowPriDroppedPackets:
    case bcmCosqStatHighPriDroppedBytes:
    case bcmCosqStatOBMLossyHighPriDroppedBytes:
    case bcmCosqStatLowPriDroppedBytes:
    case bcmCosqStatOBMLossyLowPriDroppedBytes:
    case bcmCosqStatOBMLossless0DroppedPackets:
    case bcmCosqStatOBMLossless1DroppedPackets:
    case bcmCosqStatOBMLossless0DroppedBytes:
    case bcmCosqStatOBMLossless1DroppedBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        if ((stat == bcmCosqStatHighPriDroppedPackets) ||
            (stat == bcmCosqStatOBMLossyHighPriDroppedPackets)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
        } else if ((stat == bcmCosqStatLowPriDroppedPackets) ||
                   (stat == bcmCosqStatOBMLossyLowPriDroppedPackets)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
        } else if ((stat == bcmCosqStatHighPriDroppedBytes) ||
                   (stat == bcmCosqStatOBMLossyHighPriDroppedBytes)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
        } else if ((stat == bcmCosqStatLowPriDroppedBytes) ||
                   (stat == bcmCosqStatOBMLossyLowPriDroppedBytes)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
        } else if (stat == bcmCosqStatOBMLossless0DroppedPackets) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
        } else if (stat == bcmCosqStatOBMLossless1DroppedPackets) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
        } else if (stat == bcmCosqStatOBMLossless0DroppedBytes) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
        } else { /* (stat == bcmCosqStatOBMLossless1DroppedBytes)  */
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
        }
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        ctrl_info.instance = local_port;
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                     0, value));

        break;

    case bcmCosqStatRQETotalDroppedPackets:
    case bcmCosqStatRQETotalDroppedBytes:
    case bcmCosqStatRQEGreenDroppedPackets:
    case bcmCosqStatRQEYellowDroppedPackets:
    case bcmCosqStatRQERedDroppedPackets:
        return _bcm_ap_rqe_drop_counters_set(unit, port,
                cosq, stat, value);

    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


static int 
_bcm_ap_cosq_uc_stat_get(int unit, bcm_port_t port, uint64 *value, soc_reg_t regs1,
                        soc_reg_t regs2, int sync_mode, int cosq)
{
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    bcm_port_t local_port;
    _bcm_ap_cosq_node_t *node;
    _bcm_ap_cosq_port_info_t *port_info;

    _bcm_ap_mmu_info_t *mmu_info;
    mmu_info = _bcm_ap_mmu_info[unit];
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                               &local_port, NULL, &node));
    port_info = &mmu_info->port_info[local_port];
    if (node->hw_index < port_info->uc_base || 
            node->hw_index >= port_info->uc_limit) {
        /* coverity[NEGATIVE_RETURNS: FALSE] */
        BCM_IF_ERROR_RETURN
            (counter_get(unit, node->hw_index,
                         regs2, 0,
                         value));
    } else {


        /* coverity[NEGATIVE_RETURNS: FALSE] */

        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                         regs1, cosq,
                         value));
    }
    return BCM_E_NONE;
}

static int
_bcm_ap_cosq_mc_stat_get(int unit, bcm_port_t port, uint64 *value, soc_reg_t regs, 
                        int sync_mode, int cosq)
{
    int startq;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    bcm_port_t local_port;
    
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve
         (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
          &local_port, &startq, NULL));
    /* coverity[NEGATIVE_RETURNS: FALSE] */
    BCM_IF_ERROR_RETURN
        (counter_get(unit, local_port, regs, startq, value));
    return BCM_E_NONE;
}


/* get the l2 queues for the scheduler node*/
STATIC int
_bcm_ap_cosq_stat_get(int unit, bcm_port_t port, _bcm_ap_cosq_node_t *node,int sync_mode, 
                        uint64 *value, soc_reg_t uc_regs, soc_reg_t mc_regs)
{
    uint64 sum;
    uint64  value64;

    COMPILER_64_ZERO(value64);
    COMPILER_64_ZERO(sum);
    /*the function will be called recursively */    
    if (node->level == SOC_APACHE_NODE_LVL_L2) {
        while (node){
            port = node->gport;
            /*
             *check if the port is uc or mc and get the stats for the needed 
             *type    
             */
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (uc_regs == -1){
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(
                        _bcm_ap_cosq_uc_stat_get(unit, port, &value64, uc_regs, mc_regs,
                            sync_mode, node->hw_cosq));
                COMPILER_64_ADD_64(sum, value64);
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                if (mc_regs == -1){
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(
                        _bcm_ap_cosq_mc_stat_get(unit, port, &value64, mc_regs, 
                            sync_mode, node->hw_cosq));
                COMPILER_64_ADD_64(sum, value64);
            }           
            node = node->sibling;
        }
    }
    else {
        if(node->sibling){
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, node->sibling, sync_mode, 
                        value, uc_regs, mc_regs));
        }
        if(node->child){
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, node->child, sync_mode, 
                        value, uc_regs, mc_regs));
        }
    }
    COMPILER_64_ADD_64(*value, sum);
    return BCM_E_NONE; 

}



int
bcm_ap_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i, from_cos, to_cos, ci, start_hw_index, end_hw_index;
    uint64 sum, value64;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);
    soc_reg_t ctr_reg;
    soc_ctr_control_info_t ctrl_info;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_port_info_t *port_info;
    if ((value == NULL) || (cosq < -1)) {
        return BCM_E_PARAM;
    }
    

    mmu_info = _bcm_ap_mmu_info[unit];

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        
        if (BCM_GPORT_IS_SCHEDULER(port)) {
             BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            
             if (cosq == BCM_COS_INVALID) {
                 node = node->child;
                 if (node){
                    COMPILER_64_ZERO(value64);
                     BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, 
                                 node, sync_mode, &value64,SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT));
                     *value = value64;
                     break; 
                 }
             }
            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
             BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        NULL, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, -1,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                     &local_port, &startq, NULL));
                     /* coverity[NEGATIVE_RETURNS: FALSE] */
            
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                   &local_port, NULL, &node));
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base || 
                             node->hw_index >= port_info->uc_limit) 
                {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, node->hw_index,
                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, 0,
                                 value));
                } else {                                     
                     BCM_IF_ERROR_RETURN
                         (counter_get(unit, local_port,
                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, startq,
                                 value));
                }
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                    SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, startq, value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                  (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            if (cosq == BCM_COS_INVALID) {
                 node = node->child;
                 if (node){
                    COMPILER_64_ZERO(value64);
                     BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, 
                                 node, sync_mode, &value64,SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE));
                     *value = value64;
                     break; 
                 }
             }

                BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL,
                                        NULL, NULL, &node));
            if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, -1,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, node->hw_index,
                                     value));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                    (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                     &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                   &local_port, NULL, &node));
                port_info = &mmu_info->port_info[local_port];
                if (node->hw_index < port_info->uc_base || 
                          node->hw_index >= port_info->uc_limit) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, 0,
                                 value));
                } else { 
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, startq,
                                 value));
                }
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                  SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, startq, value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                       (_bcm_ap_cosq_index_resolve
                       (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
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
            (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            if (cosq == BCM_COS_INVALID) {
                 node = node->child;
                 if (node){
                    COMPILER_64_ZERO(value64);
                     BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, 
                                 node, sync_mode, &value64,SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT));
                     *value = value64;
                     break; 
                 }
             }


            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                       &local_port, NULL, &node));
            port_info = &mmu_info->port_info[local_port];
            if (node->hw_index < port_info->uc_base || 
                     node->hw_index >= port_info->uc_limit) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, 0,
                                 value));
            } else if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT, node->hw_index,
                                     value));

            } else {
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                  &local_port, &startq, NULL));
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, startq,
                                 value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
            
            if (cosq == BCM_COS_INVALID) {
                 node = node->child;
                 if (node){
                    COMPILER_64_ZERO(value64);
                     BCM_IF_ERROR_RETURN(_bcm_ap_cosq_stat_get(unit, port, 
                                 node, sync_mode, &value64,SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE));
                     *value = value64;
                     break; 
                 }
             }

            BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL ,
                                        &local_port, NULL, &node));
            port_info = &mmu_info->port_info[local_port];
            if (node->hw_index < port_info->uc_base ||
                          node->hw_index >= port_info->uc_limit) {
                BCM_IF_ERROR_RETURN
                (counter_get(unit, node->hw_index,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, 0,
                                 value));
            } else if (node->type == _BCM_AP_NODE_VM_UCAST) {
                /* coverity[NEGATIVE_RETURNS: FALSE] */
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT, node->hw_index,
                                     value));

            } else {

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, startq,
                                 value));
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                if (!IS_CPU_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
                    (_bcm_ap_cosq_index_resolve
                     (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve
                (unit, port, cosq, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                 &local_port, &startq, NULL));  
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM, startq, value));
        break;
    case bcmCosqStatIngressPortPoolSharedBytesPeak:

        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_sp_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));

        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_POOL_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;
        break;
    case bcmCosqStatIngressPortPoolSharedBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_sp_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

        COMPILER_64_ZERO(sum);
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_POOL_CURRENT,
                                             i, &value64));
           COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;
        break;

    case bcmCosqStatIngressPortPGMinBytesPeak:

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq, 
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_MIN_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
          }

        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;
        break;
    case bcmCosqStatIngressPortPGMinBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_MIN_CURRENT,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;
        break;     

    case bcmCosqStatIngressPortPGSharedBytesPeak:

        BCM_IF_ERROR_RETURN
           (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_PG_SHARED_PEAK,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;
        break;

    case bcmCosqStatIngressPortPGSharedBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                          SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT,
                                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatIngressPortPGHeadroomBytesPeak:

        BCM_IF_ERROR_RETURN
               (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);

        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                       SOC_COUNTER_NON_DMA_PG_HDRM_PEAK,
                                       i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatIngressPortPGHeadroomBytesCurrent:

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_ingress_pg_get(unit, port, cosq,
                                    &start_hw_index, &end_hw_index));

        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        COMPILER_64_ZERO(sum);
        for (i = start_hw_index; i <= end_hw_index; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                 (counter_get(unit, local_port,
                            SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT,
                                i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
        *value = sum;

        break;

    case bcmCosqStatEgressMCQueueBytesPeak:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            if (cosq == BCM_COS_INVALID ) 
            { 
                 return BCM_E_PARAM; 
            }  
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
                   
            BCM_IF_ERROR_RETURN(
             _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
            if (node->level != SOC_APACHE_NODE_LVL_L2) {
                return BCM_E_PARAM;
            }   
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, 0, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_QUEUE_PEAK, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
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
            COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
            *value = sum;
        }
        break;
    case bcmCosqStatEgressMCQueueBytesCurrent:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
           if (cosq == BCM_COS_INVALID) {
               return BCM_E_PARAM;
           }
           BCM_IF_ERROR_RETURN(
                   _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
           if (node->level != SOC_APACHE_NODE_LVL_L2) {
               return BCM_E_PARAM;
           }   
           port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, 0, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_QUEUE_CURRENT, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                 BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                        (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_MCAST,
                        &local_port, &startq, &numq));
                 for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                      BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_QUEUE_CURRENT,
                                             startq + i, &value64));
                     COMPILER_64_ADD_64(sum, value64);
                }
            }
            COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
            *value = sum;
        }
        break;

    case bcmCosqStatEgressUCQueueBytesPeak:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                        &local_port, NULL, &node));
           if (cosq == BCM_COS_INVALID) {
               return BCM_E_PARAM;
           }
           BCM_IF_ERROR_RETURN(
                  _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
           if (node->level != SOC_APACHE_NODE_LVL_L2) {
               return BCM_E_PARAM;
           }   
            port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, 0, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            if (IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PARAM;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                      (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
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
            COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
            *value = sum;
        }
        break;
    case bcmCosqStatEgressUCQueueBytesCurrent:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, port, 0, NULL, 
                                       &local_port, NULL, &node));

            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
           BCM_IF_ERROR_RETURN(
                    _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
           if (node->level != SOC_APACHE_NODE_LVL_L2) {
               return BCM_E_PARAM;
           }   
           port = node->gport;
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, port, 0, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                  &local_port, &startq, NULL));
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT, startq,
                                 value));
            COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, port));

            if (IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PARAM;
            }
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                from_cos = 0;
                to_cos = (IS_CPU_PORT(unit, local_port)) ? 
                            NUM_CPU_COSQ(unit) - 1 : _AP_NUM_COS(unit) - 1;
            } else {
                from_cos = to_cos = cosq;
            }

            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                        (_bcm_ap_cosq_index_resolve
                            (unit, port, ci, _BCM_AP_COSQ_INDEX_STYLE_EGR_PERQ_UCAST,
                             &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                        /* coverity[NEGATIVE_RETURNS: FALSE] */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                             SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT,
                                             startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
               }
            }
            COMPILER_64_UMUL_32(sum, _BCM_AP_BYTES_PER_CELL);
            *value = sum;
        }
        break;

    case bcmCosqStatGlobalHeadroomBytes0Peak:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes0Current:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes1Peak:
         BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_PEAK,
                            0, value));
        COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        break;
    case bcmCosqStatGlobalHeadroomBytes1Current:
        BCM_IF_ERROR_RETURN
            (counter_get(unit, -1,
                             SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_CURRENT,
                             0, value));
        COMPILER_64_UMUL_32(*value, _BCM_AP_BYTES_PER_CELL);
        break;
    case bcmCosqStatHighPriDroppedPackets:
    case bcmCosqStatOBMLossyHighPriDroppedPackets:
    case bcmCosqStatLowPriDroppedPackets:
    case bcmCosqStatOBMLossyLowPriDroppedPackets:
    case bcmCosqStatHighPriDroppedBytes:
    case bcmCosqStatOBMLossyHighPriDroppedBytes:
    case bcmCosqStatLowPriDroppedBytes:
    case bcmCosqStatOBMLossyLowPriDroppedBytes:
    case bcmCosqStatOBMLossless0DroppedPackets:
    case bcmCosqStatOBMLossless1DroppedPackets:
    case bcmCosqStatOBMLossless0DroppedBytes:
    case bcmCosqStatOBMLossless1DroppedBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_SCHEDULER(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_localport_resolve(unit, port, &local_port));
        if ((stat == bcmCosqStatHighPriDroppedPackets) ||
            (stat == bcmCosqStatOBMLossyHighPriDroppedPackets)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
        } else if ((stat == bcmCosqStatLowPriDroppedPackets) ||
                   (stat == bcmCosqStatOBMLossyLowPriDroppedPackets)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
        } else if ((stat == bcmCosqStatHighPriDroppedBytes) ||
                   (stat == bcmCosqStatOBMLossyHighPriDroppedBytes)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
        } else if ((stat == bcmCosqStatLowPriDroppedBytes) ||
                   (stat == bcmCosqStatOBMLossyLowPriDroppedBytes)) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
        } else if (stat == bcmCosqStatOBMLossless0DroppedPackets) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
        } else if (stat == bcmCosqStatOBMLossless1DroppedPackets) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
        } else if (stat == bcmCosqStatOBMLossless0DroppedBytes) {
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
        } else { /* (stat == bcmCosqStatOBMLossless1DroppedBytes)  */
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
        }
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        ctrl_info.instance = local_port;
        BCM_IF_ERROR_RETURN
            (soc_counter_generic_get(unit, ctr_reg, ctrl_info, 0,
                                     0, value));
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_quantize_table_set
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
_bcm_ap_cosq_quantize_table_set(int unit, int profile_index, int weight_code,
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
        (soc_profile_reg_ref_count_get(unit, _bcm_ap_feedback_profile[unit],
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
_bcm_ap_cosq_sample_int_table_set(int unit, int min, int max,
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

    return soc_profile_mem_add(unit, _bcm_ap_sample_int_profile[unit], entries,
                               64, (uint32 *)profile_index);
}

/*
 * Function:
 *     bcm_ap_cosq_congestion_queue_set
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
bcm_ap_cosq_congestion_queue_set(int unit, bcm_port_t port,
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
    int maxindex = 0 ;
    if (cosq < 0 || cosq >= _AP_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }

    if ((index < -1) || (index >= _BCM_AP_NUM_L2_UC_LEAVES)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    mem = MMU_QCN_ENABLE_0m;
    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
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
            (soc_profile_reg_delete(unit, _bcm_ap_feedback_profile[unit],
                                    profile_index));

        profile_index =
            soc_mem_field32_get(unit, mem, &enable_entry, SITB_SELf);

        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_ap_sample_int_profile[unit],
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
            (soc_profile_reg_add(unit, _bcm_ap_feedback_profile[unit],
                                 rval64s, 1, (uint32 *)&profile_index));

        /* Update quantized feedback calculation lookup table */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_quantize_table_set(unit, profile_index, weight_code,
                                             set_point, &active_offset));

        /* Pick some sample interval */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_sample_int_table_set(unit, 13, 127,
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
 *     bcm_ap_cosq_congestion_queue_get
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
bcm_ap_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq, int *index)
{
    bcm_port_t local_port;
    int qindex;
    mmu_qcn_enable_entry_t entry;
    soc_mem_t mem;
    int maxindex = 0;
    if (cosq < 0 || cosq >= _AP_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }
    
    if (index == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));

    mem = MMU_QCN_ENABLE_0m;
    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
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
 *     bcm_ap_cosq_congestion_quantize_set
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
bcm_ap_cosq_congestion_quantize_set(int unit, bcm_port_t port,
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
    int maxindex = 0;
    if (cosq < 0 || cosq >= _AP_NUM_COS(unit)) {
        return BCM_E_PARAM;
    }

    
    mem = MMU_QCN_ENABLE_0m;

    BCM_IF_ERROR_RETURN
        (bcm_ap_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq, 
                _BCM_AP_COSQ_INDEX_STYLE_QCN, &local_port, &qindex, NULL));

    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
    }
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
        (soc_profile_reg_add(unit, _bcm_ap_feedback_profile[unit], rval64s,
                             1, (uint32 *)&profile_index));
    /* Delete original feedback parameter profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_ap_feedback_profile[unit],
                                old_profile_index));

    /* Update quantized feedback calculation lookup table */
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_quantize_table_set(unit, profile_index, weight_code,
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
 *     bcm_ap_cosq_congestion_quantize_get
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
bcm_ap_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *weight_code,
                                    int *set_point)
{
    bcm_port_t local_port;
    int cpq_index, profile_index, qindex;
    uint32 rval;
    mmu_qcn_enable_entry_t enable_entry;
    soc_mem_t mem;
    int maxindex = 0;

    BCM_IF_ERROR_RETURN
        (bcm_ap_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    mem = MMU_QCN_ENABLE_0m;
    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
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
bcm_ap_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int min, int max)
{
    bcm_port_t local_port;
    mmu_qcn_enable_entry_t entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int qindex;
    uint32 profile_index, old_profile_index;
    soc_mem_t mem;
    int maxindex  = 0;
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));

    mem = MMU_QCN_ENABLE_0m;
    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
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
        (_bcm_ap_cosq_sample_int_table_set(unit, min, max, &profile_index));

    /* Delete original sample interval profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, _bcm_ap_sample_int_profile[unit],
                                old_profile_index * 64));

    soc_mem_field32_set(unit, mem, &entry, SITB_SELf, 
                        profile_index / 64);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                      qindex, &entry));
    return BCM_E_NONE;
}

int
bcm_ap_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *min, int *max)
{
    bcm_port_t local_port;
    mmu_qcn_enable_entry_t entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int qindex;
    uint32 old_profile_index;
    soc_mem_t mem;
    int maxindex = 0 ; 

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_index_resolve(unit, port, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_QCN,
                                    &local_port, &qindex, NULL));
    
    mem = MMU_QCN_ENABLE_0m;
    maxindex =  soc_mem_index_count(unit, mem); 
    if ((qindex < -1) || (qindex >= maxindex)) {
        return BCM_E_PARAM;
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
_bcm_ap_cosq_ing_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
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
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

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
_bcm_ap_cosq_ing_pool_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t pri_grp,
        bcm_cosq_control_t type, int *arg)
{
    int pool_start_idx = 0, pool_end_idx =0;
    BCM_IF_ERROR_RETURN(
            _bcm_ap_cosq_ingress_sp_get(
                unit, gport, pri_grp, &pool_start_idx, &pool_end_idx));
    *arg = pool_start_idx;
    return BCM_E_NONE;
}

 /*
 * Function:
 *     _bcm_ap_cosq_port_qnum_get
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
_bcm_ap_cosq_port_qnum_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int hw_index;
    int uc;
    int qnum;
    int id, startq, numq;
    _bcm_ap_cosq_node_t *node;
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
            (_bcm_ap_cosq_node_get(unit, gport, cosq, NULL, &local_port, &id,
                                   &node));
        if (node->attached_to_input < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }

        startq = 0;
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        hw_index = soc_apache_l2_hw_index(unit,
            si->port_uc_cosq_base[local_port] + startq, uc);
    }

    qnum = soc_apache_hw_index_logical_num(unit,local_port,hw_index,uc);
    *arg = soc_apache_logical_qnum_hw_qnum(unit,local_port,qnum,uc);

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_egr_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));

    mem = MMU_THDM_DB_PORTSP_CONFIG_0m;

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            midx, entry));

        soc_mem_field32_set(unit, mem, entry, SHARED_LIMIT_ENABLEf, !!arg);

        return soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry);
    }
    if((type == bcmCosqControlEgressPool) || (type == bcmCosqControlUCEgressPool) ||
        (type == bcmCosqControlMCEgressPool)) {
        if(( arg < 0 ) || (arg > 3 )) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
            mem =  MMU_THDM_MCQE_QUEUE_CONFIG_0m;
            mem2 = MMU_THDM_DB_QUEUE_CONFIG_0m;
        } else  {
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
                mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
                startq -= _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;
                 /* MC queues comes after UC queues */
                mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
                mem2 = MMU_THDM_DB_QUEUE_CONFIG_0m;
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

    arg = arg / _BCM_AP_BYTES_PER_CELL;

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
        /* must have default. Otherwise, compilation error */
        /* coverity[dead_error_begin : FALSE] */
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

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
        mem2 = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;

    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        mem = MMU_THDM_DB_PORTSP_CONFIG_0m;
        startq = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
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
_bcm_ap_cosq_egr_port_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));
    mem = MMU_THDU_XPIPE_CONFIG_PORTm;
    midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
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

    arg = arg / _BCM_AP_BYTES_PER_CELL;

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
_bcm_ap_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, pool, midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int granularity = 1;

    if ((!arg) || (cosq < 0))  {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlUCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UC_EGR_POOL,
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
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
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
        *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;

        return BCM_E_NONE;
    }
    mem = MMU_THDM_DB_PORTSP_CONFIG_0m;

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));

        *arg = soc_mem_field32_get(unit, mem, entry, SHARED_LIMIT_ENABLEf);
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        mem = MMU_THDM_DB_PORTSP_CONFIG_0m;
        startq = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
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
    *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
    return BCM_E_NONE;
}


STATIC int
_bcm_ap_cosq_egr_port_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
      (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                   _BCM_AP_COSQ_INDEX_STYLE_EGR_POOL,
                                   &local_port, &pool, NULL));
    mem = MMU_THDU_XPIPE_CONFIG_PORTm;
    midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
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

	
    *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
    return BCM_E_NONE;
}



STATIC int
_bcm_ap_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
    int cur_val, rv, post_update, limit = 0;
    _bcm_ap_mmu_info_t *mmu_info = _bcm_ap_mmu_info[unit];

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg /= _BCM_AP_BYTES_PER_CELL;

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                     (unit, local_port, ci, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;

        mem2 = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;

        fld_sp = Q_SPIDf; 
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                     (unit, local_port, ci, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        mem2 = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        fld_sp = Q_SPIDf;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));

    switch (type) {
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;

        if (mem2 != INVALIDm) {
            soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);
        }

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

    /* Recalculate Shared Values if Min Changed */
    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, spid, &rval));
    shared_size = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, rval, SHARED_LIMITf); 
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
        rv = soc_apache_mmu_config_res_limits_update(unit,spid,
                                                  shared_size - delta, 1);
        if (rv < 0) { 
            return rv;
        }
        mmu_info->curr_shared_limit = shared_size - delta;
    }


    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        /* Get the latest entry data and update. */
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                         startq, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if (mem2 != INVALIDm) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
    }

    if (post_update && 
        ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
         (type == bcmCosqControlEgressMCQueueMinLimitBytes))) {
        int delta = 0;
        delta = (cur_val - (arg / granularity)) * granularity;
        rv = soc_apache_mmu_config_res_limits_update(unit,spid,
                                                  shared_size + delta, 0);
        if (rv < 0) { 
            return rv;
        }
        mmu_info->curr_shared_limit = shared_size + delta;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                     (unit, local_port, ci, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve
                     (unit, local_port, ci, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
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
    *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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

    arg /= _BCM_AP_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        (type == bcmCosqControlIngressPortPGMinLimitBytes) ||
        (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) ||
        (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
        
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);

        mem = THDI_PORT_SP_CONFIG_Xm;
        /* get index for Port-SP */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg_pool);
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

STATIC int
_bcm_ap_cosq_state_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

            if (!SOC_PORT_VALID(unit, local_port)) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN (_bcm_ap_cosq_egress_sp_get(unit, gport, cosq, &start_pool_idx, 
                        &end_pool_idx)); 
            bitno = start_pool_idx; 
            break;
   
        case bcmCosqControlPGPortLimitState:
        case bcmCosqControlPGPortXoffState:
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

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
_bcm_ap_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        (type == bcmCosqControlIngressPortPGMinLimitBytes) ||
        (type == bcmCosqControlIngressPortPGHeadroomLimitBytes)||
        (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        reg = THDI_PORT_PG_SPIDr;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg_pool = soc_reg_field_get(unit, reg, rval, prigroup_spid_field[port_pg]);

        mem = THDI_PORT_SP_CONFIG_Xm;
        /* get index for Port-SP */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg_pool);
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
        granularity =1;
        break;
    case bcmCosqControlIngressPortPGResetFloorBytes:
        fld_limit = PG_RESET_FLOORf;
        granularity =1;
        break;
    /* coverity[dead_error_begin] */
    default:
        return BCM_E_UNAVAIL;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
    return BCM_E_NONE;
}



/*
 * This function is used to get ingress pool
 * limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_ap_cosq_ing_res_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
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
       (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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

    *arg = (*arg) * _BCM_AP_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * This function is used to set ingress pool
 * limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_ap_cosq_ing_res_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
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

    arg /= _BCM_AP_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
_bcm_ap_cosq_ef_op_at_index(int unit, int port, int hw_index, _bcm_cosq_op_t op, 
                             int *ef_val)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_apache_sched_type_t sched_type;
    soc_mem_t mem;

    sched_type = _soc_apache_port_sched_type_get(unit, port);

    if (sched_type == SOC_APACHE_SCHED_LLS) {
        mem = _SOC_APACHE_NODE_PARENT_MEM(unit, port, SOC_APACHE_NODE_LVL_L2);
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
_bcm_ap_cosq_ef_op(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, int *arg,
                    _bcm_cosq_op_t op)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    int index, numq, from_cos, to_cos, ci;

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
                                    &local_port, NULL, &node));
        BCM_IF_ERROR_RETURN(
               _bcm_ap_cosq_child_node_at_input(node, cosq, &node));
        gport = node->gport;
        cosq = 0;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve
             (unit, gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
              &local_port, &index, NULL));
        return _bcm_ap_cosq_ef_op_at_index(unit, local_port, index, op, arg);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve
             (unit, gport, cosq, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
              &local_port, &index, NULL));
        return _bcm_ap_cosq_ef_op_at_index(unit, local_port, index, op, arg);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_port_has_ets(unit, local_port));
        if (cosq == BCM_COS_INVALID) {
            from_cos = 0;
            to_cos = _AP_NUM_COS(unit) - 1;
        } else {
            from_cos = to_cos = cosq;
        }
        for (ci = from_cos; ci <= to_cos; ci++) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, gport, ci, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &local_port, &index, &numq));

            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_ef_op_at_index(unit, 
                                local_port, index, op, arg));
            if (op == _BCM_COSQ_OP_GET) {
                return BCM_E_NONE;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, gport, ci, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &index, &numq));

            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_ef_op_at_index(unit, 
                                local_port, index, op, arg));
        }
    }
    return BCM_E_NONE;
}
STATIC int
_bcm_ap_cosq_qcn_proxy_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int *arg)
{
    int local_port;
    uint32 rval;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));

    BCM_IF_ERROR_RETURN(READ_QCN_CNM_PRP_CTRLr(unit, local_port, &rval));
    *arg = soc_reg_field_get(unit, QCN_CNM_PRP_CTRLr, rval, ENABLEf);

    return BCM_E_NONE;
}
    
STATIC int
_bcm_ap_cosq_qcn_proxy_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcm_cosq_control_t type, int arg)
{
    int local_port;
    uint32 rval;
    
    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
    
    BCM_IF_ERROR_RETURN(READ_QCN_CNM_PRP_CTRLr(unit, local_port, &rval));
    soc_reg_field_set(unit, QCN_CNM_PRP_CTRLr, &rval, ENABLEf, !!arg);
    soc_reg_field_set(unit, QCN_CNM_PRP_CTRLr, &rval, PRIORITY_BMAPf, 0xff);
    BCM_IF_ERROR_RETURN(WRITE_QCN_CNM_PRP_CTRLr(unit, local_port, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_alpha_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int startq;
    int midx; 
    uint32 rval;     
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
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

    switch(arg) {
    case bcmCosqControlDropLimitAlpha_1_128:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_128;
        break;
    case bcmCosqControlDropLimitAlpha_1_64:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_64;
        break;
    case bcmCosqControlDropLimitAlpha_1_32:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_32;
        break;
    case bcmCosqControlDropLimitAlpha_1_16:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_16;
        break;
    case bcmCosqControlDropLimitAlpha_1_8:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_8;
        break;
    case bcmCosqControlDropLimitAlpha_1_4:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_4;
        break;
    case bcmCosqControlDropLimitAlpha_1_2:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_2;
        break;
    case bcmCosqControlDropLimitAlpha_1:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1;
        break;   
    case bcmCosqControlDropLimitAlpha_2:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_2;
        break;
    case bcmCosqControlDropLimitAlpha_4:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_4;
        break;
    case bcmCosqControlDropLimitAlpha_8:
        alpha = SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        } 
        
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
        
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_SHARED_ALPHA_CELLf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));                 
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem =  MMU_THDM_DB_QUEUE_CONFIG_0m; 
        mem2 = MMU_THDM_MCQE_QUEUE_CONFIG_0m;

        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
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
        BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlIngressPortPGSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
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
_bcm_ap_cosq_alpha_get(int unit, 
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
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);  
    
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;

        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHAf);        
           
    } else {
        BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlIngressPortPGSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, PG_SHARED_LIMITf);  
        
    } 

    switch(alpha) {
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_128:
        *arg = bcmCosqControlDropLimitAlpha_1_128;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_64:
        *arg = bcmCosqControlDropLimitAlpha_1_64;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_32:
        *arg = bcmCosqControlDropLimitAlpha_1_32;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_16:
        *arg = bcmCosqControlDropLimitAlpha_1_16;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_8:
        *arg = bcmCosqControlDropLimitAlpha_1_8;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_4:
        *arg = bcmCosqControlDropLimitAlpha_1_4;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1_2:
        *arg = bcmCosqControlDropLimitAlpha_1_2;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_1:
        *arg = bcmCosqControlDropLimitAlpha_1;
        break;   
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_2:
        *arg = bcmCosqControlDropLimitAlpha_2;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_4:
        *arg = bcmCosqControlDropLimitAlpha_4;
        break;
    case SOC_APACHE_COSQ_DROP_LIMIT_ALPHA_8:
        *arg = bcmCosqControlDropLimitAlpha_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_dynamic_thresh_enable_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq;
    int from_cos = 0, to_cos = 0; 
    
    int ci;     
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

    if ((cosq < 0) || (cosq > 15)){
        return BCM_E_PARAM;
    }   

 
    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            from_cos = to_cos = cosq;

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, local_port, ci,
                          _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                          NULL, &startq, NULL));
                
            }
        }
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm; 
        
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
        else {
            from_cos = to_cos = cosq;

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));
              
            }
        }
        
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m; 
        mem2 = MMU_THDM_MCQE_QUEUE_CONFIG_0m; 

        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
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
_bcm_ap_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq;
    int from_cos = 0, to_cos = 0; 
    int ci;   
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

    if ((cosq < 0) || (cosq > 15)){
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
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
        mem = THDI_PORT_PG_CONFIG_Xm;
        /* get index for Port-PG */
        midx = SOC_APACHE_MMU_PIPED_MEM_INDEX(unit, local_port, mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 midx, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, PG_SHARED_DYNAMICf);  
        
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            from_cos = to_cos = cosq;
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, local_port, ci,
                          _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                          NULL, &startq, NULL));
            }
        }
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm; 

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);        
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
        else {
            from_cos = to_cos = cosq;
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));
            }
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m; 

        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMICf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
/*
 * Function: _bcm_ap_cosq_egr_queue_color_limit_set
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
_bcm_ap_cosq_egr_queue_color_limit_set(int unit, bcm_gport_t gport,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_RED_CELLf;
        } else {
            fld_limit = LIMIT_YELLOW_CELLf;
        }
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _BCM_AP_BYTES_PER_CELL - 1) / _BCM_AP_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg < 0) || ((arg/granularity) > max_val)) {
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = RED_SHARED_LIMITf;
        } else {
            fld_limit = YELLOW_SHARED_LIMITf;
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _BCM_AP_BYTES_PER_CELL - 1) / _BCM_AP_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg < 0) || ((arg/granularity) > max_val)) {
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
 * Function: _bcm_ap_cosq_egr_queue_color_limit_get
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
_bcm_ap_cosq_egr_queue_color_limit_get(int unit, bcm_gport_t gport,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_RED_CELLf;
        } else {
            fld_limit = LIMIT_YELLOW_CELLf;
        }
        mem =  MMU_THDU_XPIPE_CONFIG_QUEUEm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
        }
    } else if ((type == bcmCosqControlEgressMCQueueRedLimit) ||
               (type == bcmCosqControlEgressMCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = RED_SHARED_LIMITf;
        } else {
            fld_limit = YELLOW_SHARED_LIMITf;
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;
        }

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function: _bcm_ap_cosq_egr_queue_color_limit_mode_set
 * Purpose: Set color limit mode per queue to static (0) or
 *          dynamic (1)
*/

STATIC int
_bcm_ap_cosq_egr_queue_color_limit_mode_set(int unit, bcm_gport_t gport,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        mem =  MMU_THDU_XPIPE_CONFIG_QUEUEm;
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, RED_SHARED_LIMITf, 0);
            soc_mem_field32_set(unit, mem, entry, YELLOW_SHARED_LIMITf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_ap_cosq_egr_queue_color_limit_mode_get
 * Purpose: Get color limit mode per queue.
 * Returns: 0 (static) or 1 (dynamic)
*/
STATIC int
_bcm_ap_cosq_egr_queue_color_limit_mode_get(int unit, bcm_gport_t gport,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }
        mem =  MMU_THDU_XPIPE_CONFIG_QUEUEm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMIC_CELLf);
    } else if (type == bcmCosqControlEgressMCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        startq -= _BCM_AP_NUM_L2_UC_LEAVES;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_DYNAMICf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 bcm_cosq_control_t type,
                                                 int arg)
{
    bcm_port_t local_port;
    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_name = INVALIDf;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if ((type == bcmCosqControlEgressUCQueueLimitEnable) ||  
        (type == bcmCosqControlEgressUCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        if (type == bcmCosqControlEgressUCQueueLimitEnable) {
            fld_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
            fld_name = Q_COLOR_ENABLE_CELLf;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, fld_name);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, fld_name, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    }else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */

        mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }

        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */

        mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }

        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_COLOR_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_COLOR_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_qgroup_limit_enable_set
 * Purpose:
 *     Enable/Disable COS queue Queue group
 * Parameters:
 *      unit   - (IN) unit number
 *      gport  - (IN) GPORT identifier
 *      cosq   - (IN) COS queue to configure
 *      type   - (IN) Operation type
 *      enable - (IN) Enable/Disable
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_ap_cosq_qgroup_limit_enable_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int enable)
{
    bcm_port_t  local_port;
    soc_mem_t   mem = INVALIDm;
    soc_field_t field = INVALIDf;
    uint32      entry[SOC_MAX_MEM_WORDS];
    int         startq;
    int         qgroup_valid;

    if (!((enable == 0) || (enable == 1))) {
        return BCM_E_PARAM;
    }

    if ((type == bcmCosqControlEgressUCQueueGroupMinEnable) || 
            (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) ||
            (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable)) {

        /* Resolve cosq for port or take from input argument */
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            /* Operation not valid for the multicast queue */
            return BCM_E_PARAM;
        } else {
            if (cosq < -1) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            NULL, &startq, NULL));
        } /* if BCM_GPORT_IS_UCAST_QUEUE_GROUP */

        /* Update Table */
        mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    startq, entry));
        qgroup_valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
        if (!qgroup_valid) {
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
            mem = MMU_THDU_XPIPE_CONFIG_QGROUPm;
                field = Q_LIMIT_DYNAMIC_CELLf;
        }
        if ((mem != INVALIDm) && (field != INVALIDf)) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
            soc_mem_field32_set(unit, mem, entry, field, enable);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    } /* If type */

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_cosq_qgroup_limit_enable_get
 * Purpose:
 *     Enable/Disable COS queue Queue group
 * Parameters:
 *      unit   - (IN) unit number
 *      gport  - (IN) GPORT identifier
 *      cosq   - (IN) COS queue to configure
 *      type   - (IN) Operation type
 *      enable - (OUT) Enable/Disable status
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_ap_cosq_qgroup_limit_enable_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int *enable)
{
    bcm_port_t  local_port;
    soc_mem_t   mem = INVALIDm;
    uint32      entry[SOC_MAX_MEM_WORDS], valid;
    int         startq;
    soc_field_t field = INVALIDf;

    if ((type == bcmCosqControlEgressUCQueueGroupMinEnable) || 
            (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) ||
            (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable)) {
        /* Resolve cosq for port or take from input argument */
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            /* Operation not valid for the multicast queue */
            return BCM_E_PARAM;
        } else {
            if (cosq < -1) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                NULL, &startq, NULL));
        } /* if BCM_GPORT_IS_UCAST_QUEUE_GROUP */

        /* Retireve data from the table */
        mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    startq, entry));
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
        } 
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *enable = soc_mem_field32_get(unit, mem, entry, field);
    } else {
        return BCM_E_PARAM;
    } /* If type */

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
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
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                          _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                          &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                    (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                    NULL, &startq, NULL));
        }

        mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        if (type == bcmCosqControlEgressUCQueueLimitEnable) {
            field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
            field_name = Q_COLOR_ENABLE_CELLf;
        }
        *arg = soc_mem_field32_get(unit, mem, entry, field_name);
    }else if ((type == bcmCosqControlEgressMCQueueLimitEnable) || 
             (type == bcmCosqControlEgressMCQueueColorLimitEnable)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_index_resolve
                (unit, local_port, cosq, _BCM_AP_COSQ_INDEX_STYLE_MCAST_QUEUE,
                NULL, &startq, NULL));
        }
        mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        if (type == bcmCosqControlEgressMCQueueLimitEnable) {
           field_name = Q_LIMIT_ENABLEf;
        } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
           field_name = Q_COLOR_LIMIT_ENABLEf;
        }
        startq -= _BCM_AP_NUM_L2_UC_LEAVES; /* MC queues comes after UC queues */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, field_name);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_ap_cosq_qgroup_limit_set(int unit, bcm_gport_t gport,
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

    arg /= _BCM_AP_BYTES_PER_CELL;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem =  MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
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

    mem = MMU_THDU_XPIPE_CONFIG_QGROUPm;
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
        rv = soc_apache_mmu_config_res_limits_update(unit, spid,
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
        rv = soc_apache_mmu_config_res_limits_update(unit, spid,
                                                  shared_size + delta, 0);
        if (rv < 0) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_cosq_qgroup_limit_get(int unit, bcm_gport_t gport,
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
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, local_port, cosq,
                                         _BCM_AP_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));
    }

    mem = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    valid = soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf);
    if (!valid) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                 "UCQ doesn't associate with a queue group!\n")));
        return BCM_E_UNAVAIL;
    }

    startq = soc_mem_field32_get(unit, mem, entry, QGROUPf);

    mem = MMU_THDU_XPIPE_CONFIG_QGROUPm;
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
    *arg = (*arg) * granularity * _BCM_AP_BYTES_PER_CELL;

    return BCM_E_NONE;
}

STATIC
int _bcm_ap_cosq_obm_threshold_get_reg_field(bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int *reg_index, soc_field_t *field)
{

    switch (cosq) {
    case BCM_COSQ_OBM_CLASS_LOSSLESS0:
        *reg_index = 0;
        *field = LOSSLESS0_DISCARDf;
        break;
    case BCM_COSQ_OBM_CLASS_LOSSLESS1:
        *reg_index = 0;
        *field = LOSSLESS1_DISCARDf;
        break;
    case BCM_COSQ_OBM_CLASS_LOSSY_LOW:
        *reg_index = 0;
        *field = LOSSY_LOW_PRIf;
        break;
    case BCM_COSQ_OBM_CLASS_LOSSY:
        *reg_index = 0;
        if (type == bcmCosqControlObmDiscardLimit) {
            *field = LOSSY_DISCARDf;
        } else { /*type must be bcmCosqControlObmMinLimit */
            *field = LOSSY_MINf;
        }
        break;
    case BCM_COSQ_OBM_CLASS_ALL:
        *reg_index = 0;
        *field = DISCARD_LIMITf;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC
int _bcm_ap_cosq_obm_threshold_set_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       bcm_cosq_control_t type,
                                       int *arg, int set)
{
    soc_info_t *si;
    int pgw, pgw_inst, subport, obm;
    int phy_port;
    bcm_port_t port;
    int reg_index;
    soc_reg_t reg;
    soc_field_t field;
    uint64 rval64;

    static const soc_reg_t obm_threshold_regs[][9] = {
        {IDB_OBM0_THRESHOLDr, IDB_OBM1_THRESHOLDr,
        IDB_OBM2_THRESHOLDr, IDB_OBM3_THRESHOLDr,
        IDB_OBM4_THRESHOLDr, IDB_OBM5_THRESHOLDr,
        IDB_OBM6_THRESHOLDr, IDB_OBM7_THRESHOLDr,
        IDB_OBM8_THRESHOLDr}
    };

    si = &SOC_INFO(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &port));
    phy_port = si->port_l2p_mapping[port];

    if (phy_port == -1) {
        return BCM_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        return BCM_E_NONE;
    }
    if (cosq == BCM_COSQ_OBM_CLASS_EXPRESS) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "ERROR: obm class express is not supported"
                            " port %d\n"), port));
        return BCM_E_UNAVAIL;
    }
    obm = si->port_serdes[port];
    obm %= _AP_TSCS_PER_PGW;
    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % _AP_PORTS_PER_TSC;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_obm_threshold_get_reg_field(cosq, type,
                                                  &reg_index, &field));

    if (type == bcmCosqControlObmDiscardLimit) {
        if ((cosq < BCM_COSQ_OBM_CLASS_LOSSLESS0) ||
            (cosq > BCM_COSQ_OBM_CLASS_ALL)) {
            return BCM_E_PARAM;
        }
    } else if (cosq == bcmCosqControlObmMinLimit) {
        if (cosq != BCM_COSQ_OBM_CLASS_LOSSY) {
            return BCM_E_PARAM;
        }
    }
    COMPILER_64_ZERO(rval64);
    reg = obm_threshold_regs[reg_index][obm];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    if (set) {
        soc_reg64_field32_set(unit, reg, &rval64, field, *arg & 0x7ff);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));
    } else {
        *arg = soc_reg64_field32_get(unit, reg, rval64, field);
    }
    return BCM_E_NONE;
}

int
bcm_ap_cosq_priority_mapping_get_all(int unit, bcm_gport_t gport, int index,
        bcm_cosq_priority_mapping_t type,
        int pri_max, int *pri_array,
        int *pri_count)
{
    bcm_port_t port;
    int i, pg, sp, count = 0;

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    switch (type) {
        case bcmCosqPriorityGroup:
            if ((index < AP_PRIOROTY_GROUP_ID_MIN) ||
                    (index > AP_PRIOROTY_GROUP_ID_MAX)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < _AP_NUM_INTERNAL_PRI; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_port_priority_group_mapping_get(unit, gport,
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
            if ((index < 0) || (index >= _APACHE_MMU_NUM_POOL)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < _AP_NUM_INTERNAL_PRI; i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_ap_port_priority_group_mapping_get(unit, gport,
                                                             i, &pg));
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_cosq_ing_pool_get(unit, gport, pg,
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

int
bcm_ap_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        return _bcm_ap_cosq_egr_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEfPropagation:
        return _bcm_ap_cosq_ef_op(unit, gport, cosq, &arg, _BCM_COSQ_OP_SET);
    case bcmCosqControlCongestionProxy:
        return _bcm_ap_cosq_qcn_proxy_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_ap_cosq_egr_queue_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPoolMinLimitBytes:
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
        return _bcm_ap_cosq_ing_res_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPoolLimitBytes:
        return _bcm_ap_cosq_ing_res_limit_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPool:
        return _bcm_ap_cosq_ing_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_ap_cosq_alpha_set(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_ap_cosq_dynamic_thresh_enable_set(unit, gport, cosq, 
                                                       type, arg);
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
        return _bcm_ap_cosq_egr_port_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
        return _bcm_ap_cosq_egr_queue_limit_enable_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
        return _bcm_ap_cosq_qgroup_limit_enable_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
         return _bcm_ap_cosq_qgroup_limit_set(unit, gport, cosq, type, arg);
    case bcmCosqControlObmDiscardLimit:
    case bcmCosqControlObmMinLimit:
         return _bcm_ap_cosq_obm_threshold_set_get(unit, gport, cosq, type, &arg, 1);

    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
         return _bcm_ap_cosq_egr_queue_color_limit_mode_set(unit, gport, cosq,
                 type, arg);
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
         return _bcm_ap_cosq_egr_queue_color_limit_set(unit, gport, cosq, type, arg);

    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_ap_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        return _bcm_ap_cosq_egr_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEfPropagation:
        return _bcm_ap_cosq_ef_op(unit, gport, cosq, arg, _BCM_COSQ_OP_GET);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_ap_cosq_egr_queue_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPoolMaxLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPoolMinLimitBytes:
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
        return _bcm_ap_cosq_ing_res_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPoolLimitBytes:   
        return _bcm_ap_cosq_ing_res_limit_get(unit, gport, cosq, type, arg);
    case bcmCosqControlSPPortLimitState:
    case bcmCosqControlPGPortLimitState:
    case bcmCosqControlPGPortXoffState:
    case bcmCosqControlPoolRedDropState:
    case bcmCosqControlPoolYellowDropState:
    case bcmCosqControlPoolGreenDropState:
        return _bcm_ap_cosq_state_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPool:
        return _bcm_ap_cosq_ing_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_ap_cosq_alpha_get(unit, gport, cosq,  
                            (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_ap_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                                       type, arg);
    case bcmCosqControlPortQueueUcast:
    case bcmCosqControlPortQueueMcast:
        return _bcm_ap_cosq_port_qnum_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressPortPoolYellowLimitBytes:
    case bcmCosqControlEgressPortPoolRedLimitBytes:
        return _bcm_ap_cosq_egr_port_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueColorLimitEnable:
    case bcmCosqControlEgressUCQueueColorLimitEnable:
        return _bcm_ap_cosq_egr_queue_limit_enable_get(unit, gport, cosq, type, arg);
    case bcmCosqControlCongestionProxy:
        return _bcm_ap_cosq_qcn_proxy_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinEnable:
    case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
    case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
        return _bcm_ap_cosq_qgroup_limit_enable_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
    case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
        return _bcm_ap_cosq_qgroup_limit_get(unit, gport, cosq, type, arg);
    case bcmCosqControlObmDiscardLimit:
    case bcmCosqControlObmMinLimit:
        return _bcm_ap_cosq_obm_threshold_set_get(unit, gport, cosq, type, arg, 0);
    case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
    case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
        return _bcm_ap_cosq_egr_queue_color_limit_mode_get(unit, gport, cosq,
                type, arg);
    case bcmCosqControlEgressUCQueueRedLimit:
    case bcmCosqControlEgressUCQueueYellowLimit:
    case bcmCosqControlEgressMCQueueRedLimit:
    case bcmCosqControlEgressMCQueueYellowLimit:
        return _bcm_ap_cosq_egr_queue_color_limit_get(unit, gport, cosq, type, arg);
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_ap_cosq_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    static const soc_field_t color_enable_fields[] = {
        PORTSP_COLOR_LIMIT_ENABLE_0f, PORTSP_COLOR_LIMIT_ENABLE_1f, 
        PORTSP_COLOR_LIMIT_ENABLE_2f, PORTSP_COLOR_LIMIT_ENABLE_3f
    };

    if (id < 0 || (id > _AP_NUM_SERVICE_POOL - 1)) {
        return BCM_E_PARAM;
    } 

    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    switch (cosq_service_pool.type) {
    case bcmCosqServicePoolPortColorAware:
        fld_enable = color_enable_fields[id];
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
bcm_ap_cosq_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    static const soc_field_t color_enable_fields[] = {
        PORTSP_COLOR_LIMIT_ENABLE_0f, PORTSP_COLOR_LIMIT_ENABLE_1f, 
        PORTSP_COLOR_LIMIT_ENABLE_2f, PORTSP_COLOR_LIMIT_ENABLE_3f
    };

    if (cosq_service_pool == NULL) {
        return BCM_E_PARAM;
    }

    if (id < 0 || (id > _AP_NUM_SERVICE_POOL - 1)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
    switch (cosq_service_pool->type) {
    case bcmCosqServicePoolPortColorAware:
        fld_enable = color_enable_fields[id];
        break;
    default:
        return BCM_E_PARAM;
    }

    cosq_service_pool->enabled = soc_reg_field_get(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, rval,
                                                   fld_enable);
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_bandwidth_set
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
bcm_ap_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int i, start_cos, end_cos, local_port;
    _bcm_ap_cosq_node_t *node;
    uint32 burst_min, burst_max;

    if (cosq <= -1) {
        if (BCM_GPORT_IS_SET(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
                                        NULL, NULL, &node));
            start_cos = 0;
            if (node->numq == -1) {
                end_cos = 0;
            } else {
                end_cos = node->numq - 1;
            }
        } else {
            start_cos = 0;
            end_cos = _AP_NUM_COS(unit) - 1;
        }
    } else {
        start_cos = end_cos = cosq;
    }
     
    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)){
       BCM_IF_ERROR_RETURN(
        _bcmi_coe_subport_physical_port_get(unit, gport, &local_port));
    }  else { 
       BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
    } 
    burst_min = (kbits_sec_min > 0) ?
          _bcm_td_default_burst_size(unit, local_port, kbits_sec_min) : 0;

    burst_max = (kbits_sec_max > 0) ?
          _bcm_td_default_burst_size(unit, local_port, kbits_sec_max) : 0;

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                         kbits_sec_max, burst_min, burst_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_bandwidth_get
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
bcm_ap_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 kbits_sec_burst;

    if (cosq == -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, gport, cosq,
                                 kbits_sec_min, kbits_sec_max,
                                 &kbits_sec_burst, &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_bandwidth_burst_set
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
bcm_ap_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int i, start_cos, end_cos;
    uint32 kbits_sec_min = 0; 
    uint32 kbits_sec_max = 0;
    uint32 kbits_sec_burst = 0;
    uint32 flags = 0;
    _bcm_ap_cosq_node_t *node;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    if (cosq == -1) {
        if (BCM_GPORT_IS_SET(gport) && 
             !(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport))) {
            BCM_IF_ERROR_RETURN
                (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, 
                                        NULL, NULL, &node));
            start_cos = 0;
            end_cos = node->numq - 1;
        } else {
            start_cos = 0;
            end_cos = _AP_NUM_COS(unit) - 1;
        }
    } else {
        start_cos = end_cos = cosq;
    }

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_bucket_get(unit, gport, i, &kbits_sec_min,
                                     &kbits_sec_max, &kbits_sec_burst,
                                     &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                     kbits_sec_max, kbits_burst_min, 
                                     kbits_burst_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_bandwidth_burst_get
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
bcm_ap_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max, flags;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 &kbits_sec_min, &kbits_sec_max, kbits_burst_min,
                                 kbits_burst_max, &flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_sched_set
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
bcm_ap_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    int rv, numq, i, count;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_AP_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    count = 0;
    for (i = 0; i < numq; i++) {
        rv = _bcm_ap_cosq_sched_set(unit, gport, cosq + i, mode, weight);
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
 *     bcm_ap_cosq_gport_sched_get
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
bcm_ap_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int *mode, int *weight)
{
    int rv, numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_index_resolve(unit, gport, 0,
                                        _BCM_AP_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        rv = _bcm_ap_cosq_sched_get(unit, gport, cosq + i, mode, weight);
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
 *     bcm_ap_cosq_gport_discard_set
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
bcm_ap_cosq_gport_discard_set(int unit, bcm_gport_t gport,
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
        discard->refresh_time <= 0 || discard->refresh_time > 512) {
        return BCM_E_PARAM;
    }
    if ((discard->min_thresh < 0) ||
        (discard->max_thresh < 0) ||
        (discard->min_thresh > discard->max_thresh)) {
        return BCM_E_PARAM;
    }

    cell_size = _BCM_AP_BYTES_PER_CELL;
    cell_field_max = AP_CELL_FIELD_MAX;

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

    /* Min equals Max is an invalid config except
     * in case of default (ResetVal)
     */
    if ((min_thresh == max_thresh) &&
        (min_thresh != AP_CELL_FIELD_MAX)) {
        return BCM_E_PARAM;
    }

    if (gport != BCM_GPORT_INVALID) {
       BCM_IF_ERROR_RETURN
           (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
       if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
           return BCM_E_PORT;
       }
    }
 
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_wred_set(unit, gport, cosq, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE, flags,discard->refresh_time));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_gport_discard_get
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
bcm_ap_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;
    bcm_port_t local_port = -1;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (gport != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_ap_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain, 
                               0,&discard->refresh_time));

    /* Convert number of cells to number of bytes */
    discard->min_thresh = min_thresh * _BCM_AP_BYTES_PER_CELL;
    discard->max_thresh = max_thresh * _BCM_AP_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_subport_flow_control_set
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
bcm_ap_cosq_subport_flow_control_set(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t sched_port)
{
#ifdef BCM_HGPROXY_COE_SUPPORT 
    uint16 coe_fc_index ;
    uint32 rval;
    bcm_gport_t subport_id, resolved_port;
    _bcm_ap_cosq_node_t *node; 
    int hw_index, rv, logical_hw_index;
    soc_apache_node_lvl_e level;
    subport_tag_sgpp_map_entry_t entry;
    bcm_port_t phy_port, mmu_port;
    soc_info_t *si;
    _bcm_ap_fc_type_t fct;
    int s1_level = 0;
    int index = 0 ;
    int cos = 0 ;

    /* Find subport id from port to index into SUBPORT_TAG_SGPP_MAP */
    BCM_IF_ERROR_RETURN(_bcmi_coe_subport_tcam_idx_get(unit, subport, &subport_id));

    /* From sched_port, need to find if it's port, L0, L1, or L2 */
    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_node_get(unit, sched_port, 0, NULL, NULL, NULL, &node));

    BCM_IF_ERROR_RETURN
    (_bcmi_coe_subport_physical_port_get(unit, subport, &resolved_port));
    /* Port, L0, L1, or L2 from node */
    level = node->level;

    /* index into L0, L1, or L2 */
    hw_index = node->hw_index;

    /* row and column are used to index in to the FC State Table, which is 4 bits wide */

    /* Get the port associated with the passed in gport */
    if (resolved_port < 0) {
        return BCM_E_PORT;
    }

    si = &SOC_INFO(unit);
    /* Getting MMU port.  Used to disable cut through for it at the end as well as
       to index into MMU_INTFI_PORT_COEFC_TB if we're setting flow control for port */
    phy_port = si->port_l2p_mapping[resolved_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    if (IS_AP_HSP_PORT( unit, resolved_port )) {
     /* SC/QM L1/L2 nodes do not support FC.
      * Hence, L1/L2 indices 8, 9, 18, 19 ... should not be configured
      * for FC.
      * similarly the last L0 of the HSP port does not support flow control
      */
        if (level == SOC_APACHE_NODE_LVL_L0) {
            if ((hw_index % 5) > 3) {
                 return BCM_E_PARAM;
            }
            cos = hw_index % 5;
        } else  { 
            if ((hw_index % 10) > 7) {
                 return BCM_E_PARAM;
            }
            cos = ((hw_index % 10) % 4);
        }
    } else { 
         cos = hw_index % 4 ;
    }
    switch(level) {
        case SOC_APACHE_NODE_LVL_ROOT:
            /* At the port level we have to do it a bit different since it's indexing into
               MMU_INTFI_PORT_COEFC_TB, which is a 128 entry x 1 bit table with each entry signifying
               an MMU Port 
            */
             index = hw_index; 
             break;
        case SOC_APACHE_NODE_LVL_L0:

             if (IS_AP_HSP_PORT( unit, resolved_port )) {
        /* The last L0 does not support  FC and so each index in fc_st_tbl represents
         * 5 l0 nodes instead of 4 the last 1 is masked by the hardware
         */ 
                  index = hw_index / 5;
                  break;  
             } else { 
                 index = hw_index / 4; 
             }
        case SOC_APACHE_NODE_LVL_L1:
        case SOC_APACHE_NODE_LVL_L2:
             if (IS_AP_HSP_PORT( unit, resolved_port )) {
        /* FC not supported on last two L1/L2 nodes The hardwate internally adjusts the value 
         * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
         */   
                 index = mmu_port * 2 + 
                   (((hw_index % 10) >= 4)? 1 : 0);
             } else { 
                index = hw_index / 4; 
             }
             break; 
        case SOC_APACHE_NODE_LVL_S1:
             index = hw_index / 4; 
             break; 
        default:
            return BCM_E_PARAM;

    }
    if ( level == SOC_APACHE_NODE_LVL_S1) 
    {
         s1_level = 1;   

    }
    if (level)  {
        level--;/* reducing one level for S1*/
    } 
    /* set coe_fc_index for us to program into the register */
    coe_fc_index = ((s1_level << 15) | (index << 4)| cos << 2 | (level)); 

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
    fct = _BCM_AP_COSQ_FC_E2ECC;

    logical_hw_index = hw_index / 4  ;

    /* To pass the proper FC STATE Table entries, we're modifying the FC MAP Table, one of its
       4 indexes, with the appropriate row in the FC STATE Table. FC MAP Table at index 0
       will handle the rows 0-3 of FC STATE Table, at index 1 will handle rows 4-7 of the
       FC STATE Table, and so on.
     
       resolved_port is passed so that the function will know which pipe to use (X or Y).
       fct will select FC STATE Table on the mux so that PFC STATE Table won't be selected.
       logical_hw_index is passed in this first time so that a proper row index for the FC STATE Table will be selected.
       node->hw_index is passed in a second time because the proper FC MAP Table index will be calculated.
       node->level is passed in so that the function will know with FC MAP Table (L0, L1, L2) to use.
       */
    if(level != SOC_APACHE_NODE_LVL_ROOT) {
        BCM_IF_ERROR_RETURN(_bcm_ap_map_fc_status_to_node(unit, resolved_port, fct, logical_hw_index, 
                                                           0 , node->hw_index, node->level));
    }

    /* Disable Cut-Through for the port specified or L0/L1/L2 node associated with it */
    BCM_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, resolved_port, &rval));
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, UC_ASF_ENABLEf, 0);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, MC_ASF_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, resolved_port, rval));

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif /* BCM_HGPROXY_COE_SUPPORT */
}

int
bcm_ap_cosq_subport_flow_control_get(
    int unit, 
    bcm_gport_t subport, 
    bcm_gport_t * sched_port)
{
#ifdef BCM_HGPROXY_COE_SUPPORT 
    int  level, i;
    int hw_index = 0;
    uint16 coe_fc_index = 0; 
    bcm_gport_t subport_id;
    _bcm_ap_cosq_node_t * node;
    _bcm_ap_mmu_info_t * mmu_info;
    subport_tag_sgpp_map_entry_t entry;
    int s1_level ; 
    int index = 0;
    int cos = 0 ;
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
    cos = (coe_fc_index >> 2) & 0x3;
    index = (coe_fc_index >> 4) & 0x7FF; 
    s1_level = (coe_fc_index >>15) ; 
    if((level) || (s1_level))  { 
        level++;/*This is needed as we decrement while storing*/ 
    }
    switch(level) {
        case SOC_APACHE_NODE_LVL_ROOT:
            /* At the port level we have to do it a bit different since it's indexing into
               MMU_INTFI_PORT_COEFC_TB, which is a 128 entry x 1 bit table with each entry signifying
               an MMU Port 
            */
             hw_index = index; 
             break; 
        case SOC_APACHE_NODE_LVL_L0:
              if (IS_AP_HSP_PORT( unit, resolved_port )) {
                  hw_index = index * 5 + cos ; 
              } else {  
                  hw_index = index * 4 + cos ; 
              }
              break; 
        case SOC_APACHE_NODE_LVL_L1:
        case SOC_APACHE_NODE_LVL_L2:
        /* FC not supported on last two L1/L2 nodes The hardwate internally adjusts the value 
         * say index 24 which for non hsp port will result in index 6 will result in 5 for HSP
         */   
              if (IS_AP_HSP_PORT( unit, resolved_port )) {
                  hw_index = (mmu_port * 10) + ((index % 2)? 4: 0) + cos ; 
              } else {  
                  hw_index = index * 4 + cos ; 
              }
              break; 
        case SOC_APACHE_NODE_LVL_S1:
             hw_index = index * 4 + cos ; 
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
    /* calculate the hw_index */

    mmu_info = _bcm_ap_mmu_info[unit];

    if (!mmu_info) {
        return BCM_E_INIT;
    }


    /* Fetch the node based upon level and hw_index */
    switch(level) {
        /* port, L0, and L1 are all scheduling nodes */
        case SOC_APACHE_NODE_LVL_L0:
        case SOC_APACHE_NODE_LVL_ROOT:
        case SOC_APACHE_NODE_LVL_L1:
        case SOC_APACHE_NODE_LVL_S1:
            for(i = 0; i < _BCM_AP_NUM_TOTAL_SCHEDULERS; i++) {
                node = &mmu_info->sched_node[i];
                if(node->level == level) {
                    if(node->hw_index == hw_index) {
                       *sched_port = node->gport;
                       return BCM_E_NONE;
                    }
                }
            }
            break;
        case SOC_APACHE_NODE_LVL_L2:
            for(i = 0; i < _BCM_AP_NUM_L2_UC_LEAVES; i++) {
                node = &mmu_info->queue_node[i];
                if(node->level == level) {
                    if(node->hw_index == hw_index) {
                       *sched_port = node->gport;
                       return BCM_E_NONE;
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
_bcm_ap_cosq_port_safc_get(int unit, bcm_port_t port,
                         int cur_class,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    int   hw_cosq, count = 0;
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1], rval, index_map;
    uint32 tmp32, bmp;
    _bcm_ap_mmu_info_t *mmu_info;
    int phy_port, mmu_port;
    soc_info_t *si;
    soc_reg_t   reg;
    int mc_index = 0;
    int mcq_offset = 0; 
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    int j, inv_mapped, hw_index, eindex;
    soc_mem_t mem;
    uint32 map_entry[SOC_MAX_MEM_WORDS];


    if (gport == NULL || gport_count <= 0 || actual_gport_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));


    si = &SOC_INFO(unit);
    mmu_info = _bcm_ap_mmu_info[unit];
    res = _BCM_AP_PRESOURCES(mmu_info, 0);
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
        (soc_profile_reg_get(unit, _bcm_ap_llfc_profile[unit],
                             profile_index, 16, rval64s));

    bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[cur_class],
                                COS_BMPf);
    for (hw_cosq = 0; hw_cosq < _BCM_AP_NUM_SAFC_CLASS ; hw_cosq++) {
        if (!(bmp & (1 << hw_cosq))) {
            continue;
        }
        if (_bcm_ap_cosq_port_has_ets(unit, local_port)) {
            inv_mapped = 0;
            for (j = _BCM_AP_NUM_PORT_SCHEDULERS; 
                 j < _BCM_AP_NUM_TOTAL_SCHEDULERS; j++) {
                node = &mmu_info->sched_node[j];
                if ((!node->in_use) || (node->local_port != local_port) ||
                    (node->hw_cosq != hw_cosq)) {
                    continue;
                }

                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16)/4;
                if (node->level == SOC_APACHE_NODE_LVL_L0) {
                    mem = MMU_INTFI_XPIPE_FC_MAP_TBL0m;
                } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
                    mem = MMU_INTFI_XPIPE_FC_MAP_TBL1m;
                } else {
                    continue;
                }

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));
                 /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                         inv_mapped = 1;
                         break;
                    }
                }
            }

            for (j = port_info->uc_base; 
                 inv_mapped==0 && (j < port_info->uc_limit); j++) {
                node = &res->p_queue_node[j];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem =   MMU_INTFI_XPIPE_FC_MAP_TBL2m ;
                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16)/4;
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                 /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                         inv_mapped = 1;
                         break;
                    }
                }
            }
            for (j = port_info->mc_base; 
                 inv_mapped==0 && (j < port_info->mc_limit); j++) {
                mc_index = soc_apache_l2_hw_index(unit, j, 0);
                node = &res->p_queue_node[mc_index];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem =   MMU_INTFI_XPIPE_FC_MAP_TBL2m ;
                mcq_offset = node->hw_index % _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;
                if (((mcq_offset % 10) > 7) ) {
                     /* FC support only for Q0-7 */
                      continue; 
                }
                hw_index = node->hw_index - (mcq_offset * 2 / 10);
                hw_index /= 16;
                eindex = (mcq_offset % 10)/4;
                eindex += ((mcq_offset /10) % 2) ? 0 : 2;
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
                gport[count++] = hw_cosq;
            } 
        }
        if (count == gport_count) {
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
 *     bcm_ap_cosq_safc_class_mapping_set
 * Purpose:
 *     Set SAFC mapping for port.
 *     SAFC class is mapped to cosq or scheduler node gports.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_ap_cosq_safc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_safc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0;
    int safc_class[_BCM_AP_NUM_SAFC_CLASS];
    bcm_gport_t cur_gport = 0;
    uint32 cos_bmp[_BCM_AP_NUM_SAFC_CLASS] = {0};
    bcm_port_t local_port = -1;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    if ((array_count < 0) || (array_count > _BCM_AP_NUM_SAFC_CLASS)) {
        return BCM_E_PARAM;
    }

    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }
    if (!IS_HG_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    } 

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_AP_NUM_SAFC_CLASS)) {
            return BCM_E_PARAM;
        } else {
           cos_bmp[cur_class] = 0;
        }

        for (index = 0; index < BCM_COSQ_SAFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }

            if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(cur_gport)) ||
                (BCM_GPORT_IS_MCAST_QUEUE_GROUP(cur_gport))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_ap_fc_status_map_gport(unit, local_port, cur_gport,
                                                 _BCM_AP_COSQ_FC_SAFC, 
                                                 &cos_bmp[cur_class]));
            } else {
                return BCM_E_PARAM;
            }
        }
    }

    for (index = 0; index < _BCM_AP_NUM_SAFC_CLASS; index++) {
        safc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_ap_port_fc_profile_set(unit, local_port,
        _BCM_COSQ_OP_SET, _BCM_AP_NUM_SAFC_CLASS, safc_class, cos_bmp));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_ap_cosq_safc_class_mapping_get
 * Purpose:
 *     Get SAFC mapping for port.
 *     Retrieves cosq or scheduler node gports
 *     associated to SAFC classes.
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
_bcm_ap_cosq_safc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_safc_class_mapping_t *mapping_array,
                                   int *array_count)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    bcm_port_t local_port = -1;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }
    if (!IS_HG_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    if (((mapping_array == NULL) && (array_max > 0)) ||
        ((mapping_array != NULL) && (array_max <= 0)) ||
        (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    if (array_max <= 0) {
        *array_count = _BCM_AP_NUM_SAFC_CLASS;
        return BCM_E_NONE;
    }

    for (cur_class = 0; cur_class < _BCM_AP_NUM_SAFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        rv = _bcm_ap_cosq_port_safc_get(unit, port, cur_class,
                        mapping_array[cur_class].gport_list,
                        BCM_COSQ_SAFC_GPORT_COUNT, &actual_gport_count);

        if (rv == BCM_E_NONE) {
            mapping_array[cur_class].class_id = cur_class;
            if (actual_gport_count < BCM_COSQ_SAFC_GPORT_COUNT) {
                mapping_array[cur_class].gport_list[actual_gport_count] =
                                                        BCM_GPORT_INVALID;
            }
            class_count++;
        } else if (rv == BCM_E_NOT_FOUND) {
            return rv;
        }

        if (class_count == array_max) {
            break;
        }
    }

    *array_count = class_count;
    if (class_count == 0) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

int
_bcm_ap_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
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
        *type = _BCM_AP_COSQ_TYPE_UCAST;
        break;
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass0McastQueue:
        *type = _BCM_AP_COSQ_TYPE_MCAST;
        break;
    case bcmSwitchPFCClass7DestmodQueue:
    case bcmSwitchPFCClass6DestmodQueue:
    case bcmSwitchPFCClass5DestmodQueue:
    case bcmSwitchPFCClass4DestmodQueue:
    case bcmSwitchPFCClass3DestmodQueue:
    case bcmSwitchPFCClass2DestmodQueue:
    case bcmSwitchPFCClass1DestmodQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *type = _BCM_AP_COSQ_TYPE_EXT_UCAST;
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

STATIC int
_bcm_ap_clear_fc_status_to_node(int unit, int port)
{ 
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    static const soc_field_t self[]={
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f, INDEX2f, INDEX3f
    };
    static const soc_mem_t memx[] = {
        MMU_INTFI_XPIPE_FC_MAP_TBS1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };
    uint32 s_value, i_value;
    int base, table, index, count, field;
    int mmu_port, phy_port, update;

    soc_info_t *soc_info;
    soc_info = &SOC_INFO(unit);

    phy_port = soc_info->port_l2p_mapping[port];
    mmu_port = soc_info->port_p2m_mapping[phy_port];
    base = mmu_port * 4;
    for (table = 0; table < COUNTOF(memx); table++){
        count = soc_mem_index_count(unit, memx[table]);
        for (index = 0; index < count; index++){
            update = 0;
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, memx[table],
                    MEM_BLOCK_ALL, index, &map_entry));         
            for (field = 0; field  <  COUNTOF(indexf); field++){
                s_value =  soc_mem_field32_get(unit, memx[table],
                                    &map_entry, self[field]);
                i_value =  soc_mem_field32_get(unit, memx[table],
                                    &map_entry, indexf[field]);

                if (s_value && (i_value >= base) && (i_value < (base + 4))) {
                    soc_mem_field32_set(unit, memx[table],
                                        &map_entry, self[field], 0);
                    soc_mem_field32_set(unit, memx[table],
                                        &map_entry, indexf[field], 0);
                    update = 1;
                }
            }
            if (update){
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, memx[table],
                        MEM_BLOCK_ALL, index, &map_entry));
            }
        }    
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_ap_map_fc_status_to_node(int unit, int port, _bcm_ap_fc_type_t fct,
                    int spad_offset, int cosq, uint32 hw_index, int level)
{
    int map_entry_index, eindex, pfc;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f, INDEX2f, INDEX3f
    };
    static const soc_mem_t memx[] = {
        INVALIDm,
        MMU_INTFI_XPIPE_FC_MAP_TBS1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };
    soc_mem_t mem;
    int port_num = 0;
    int map_entry_index_num = 0;
    
    if ((fct == _BCM_AP_COSQ_FC_PFC) ||
        (fct == _BCM_AP_COSQ_FC_SAFC)) {
        pfc = 1;
    } else {
        pfc = 0;
    }
    mem =  memx[level]; 
    if (mem ==  INVALIDm) {
        return BCM_E_PARAM;
    }
    
    /* FC_MAP_TBL[Level] has 4 Index[0-3], whose values become Index to PFC_ST_TBL.
     * Each Index of PFC_ST_TBL can point to congestion state of 4 Qs. 
     * Hence each Entry of FC_MAP_TBL can refer to 16 Qs.
     *
     * NOTE: Index of FC_MAP_TBL CANNOT be used to point to Qs/Nodes belonging
     *       to Different Ports.
     */
    if (hw_index < _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE ) {
        /* UC queue */
        if (IS_AP_HSP_PORT( unit, port )) {
            if (level == SOC_APACHE_NODE_LVL_L0) {
                /* SC/QM L0 nodes do not support FC.
                 * Hence, L0 indices 4, 9, 14, 19 ... should not be configured
                 * for FC.
                 */
                if ((hw_index % 5) > 3) {
                    return BCM_E_PARAM;
                }
                /* Since 5th L0 index is not FC mapped, each Entry of
                 * FC_MAP_TBL can refer to 20 L0 nodes with each field mapped
                 * to first 4 L0 of each port.
                 */
                map_entry_index = hw_index / 20;
                eindex = (hw_index % 20) / 5;
            } else {
                /* SC/QM L1/L2 nodes do not support FC.
                 * Hence, L1/L2 indices 8, 9, 18, 19 ... should not be configured
                 * for FC.
                 */
                if ((hw_index % 10) > 7) {
                    return BCM_E_PARAM;
                }
                /* A 2 bit shift occurs in HW for HSP ports to align with multiple of 8.
                 * Thus 10-17 becomes 8-15, etc.
                 */
                port_num = hw_index / 10;
                map_entry_index_num = port_num * 2 + (((hw_index % 10) >= 4)? 1 : 0);
                map_entry_index = map_entry_index_num / 4; /* each entry contains 4 indexes */
                eindex = map_entry_index_num & 0x3;
            }
        } else {
            map_entry_index = hw_index/16;
            eindex = (hw_index % 16) / 4;
        }
    } else {
        /* MC queue - Index Calculation
         *   PFC can work only on 8 out of 10 MC queues of each port.
         *   So in FC_MAP_TBL2, each Port will use only 2 Index(4 Qs each) for
         *   MC Qs.
         *   MC queue base starts at 16384, 16384/16 =  1024
         *   Hence MC Qs starts at = Map_Tbl2_Idx[92] and Index2.
         */
        int mcq_offset = hw_index % _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;

        if (((mcq_offset % 10) > 7) || (level != SOC_APACHE_NODE_LVL_L2)) {
            /* FC support only for Q0-7 */
            return BCM_E_INTERNAL;
        }
        map_entry_index = hw_index - (mcq_offset * 2 / 10);
        map_entry_index /= 16;
        eindex = (mcq_offset % 10)/4;
        eindex += ((mcq_offset /10) % 2) ? 0 : 2;

    }
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
 *     _bcm_ap_fc_status_map_gport
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
 */
STATIC int
_bcm_ap_fc_status_map_gport(int unit, int port, 
                bcm_gport_t gport, _bcm_ap_fc_type_t fct, uint32 *cos_bmp)
{
    int hw_index, hw_index1;
    int hw_cosq, hw_cosq1;
    int id, lvl;
    int pipe, mmu_port, phy_port;
    bcm_port_t local_port, resolved_port;
    _bcm_ap_cosq_node_t *node;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    soc_info_t *si;
    _bcm_ap_mmu_info_t *mmu_info;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);
    
    hw_index = hw_index1 = -1;
    hw_cosq = hw_cosq1 = -1;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_node_get(unit, gport, 
                            0, NULL, &resolved_port, &id, &node));
        hw_index = node->hw_index;
        hw_cosq = node->hw_cosq;
        lvl = node->level;
    } else {
        if (_bcm_ap_cosq_port_has_ets(unit, local_port)) {
            hw_cosq = gport;
            node = &res->p_queue_node[port_info->uc_base + hw_cosq];
            hw_index = node->hw_index;
            node = &res->p_mc_queue_node[port_info->mc_base + hw_cosq];
            hw_index1 = node->hw_index;
            hw_cosq1 = hw_cosq;
            lvl = SOC_APACHE_NODE_LVL_L2;
        } else {
            lvl = SOC_APACHE_NODE_LVL_L1;
            /* gport[] represnts the physical Cos value */
            hw_cosq = gport;
            BCM_IF_ERROR_RETURN(soc_apache_sched_hw_index_get(unit, local_port, 
                                SOC_APACHE_NODE_LVL_L1, hw_cosq, &hw_index));
        }
    }
    BCM_IF_ERROR_RETURN(
        _bcm_ap_map_fc_status_to_node(unit, local_port, fct,
                                       mmu_port*4, hw_cosq, hw_index, lvl));
    if (cos_bmp != NULL) {
        *cos_bmp |= 1 << hw_cosq;
    }
    if (hw_cosq1 >= 0) {
        BCM_IF_ERROR_RETURN(
            _bcm_ap_map_fc_status_to_node(unit, local_port, fct,
                                       mmu_port*4, hw_cosq1, hw_index1, lvl));
        if (cos_bmp != NULL) {
            *cos_bmp |= 1 << hw_cosq1;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_ap_port_fc_profile_set
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
_bcm_ap_port_fc_profile_set(int unit, int port, _bcm_cosq_op_t op,
                              int entry_cnt, int *pfc_class, uint32 *cos_bmp)
{
    soc_reg_t   reg;
    bcm_port_t local_port;
    int rv, entry, cur_class;
    int mmu_port, phy_port;
    uint32 profile_index, old_profile_index;
    uint32 fval, tmp32, cur_bmp;
    uint64 rval64[16], tmp, *rval64s[1], rval, index_map;
    soc_info_t *si;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };

    if (pfc_class == NULL) {
        return BCM_E_PARAM;
    }
    if (cos_bmp == NULL) {
        return BCM_E_PARAM;
    }
    if ((entry_cnt < 0 ) || (entry_cnt > _BCM_AP_NUM_SAFC_CLASS)) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(
        _bcm_ap_cosq_localport_resolve(unit, port, &local_port));

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
        (soc_profile_reg_get(unit, _bcm_ap_llfc_profile[unit],
                             old_profile_index, 16, rval64s));

    for (entry = 0; entry < entry_cnt; entry++) {
        cur_class = pfc_class[entry];
        cur_bmp = cos_bmp[entry];
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
        (soc_profile_reg_delete(unit, _bcm_ap_llfc_profile[unit],
                                old_profile_index));

    rv = soc_profile_reg_add(unit, _bcm_ap_llfc_profile[unit], rval64s,
                             16, &profile_index);
    if (SOC_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, _bcm_ap_llfc_profile[unit],
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
 *     _bcm_ap_fc_map_gport_check
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
_bcm_ap_fc_map_gport_check(int unit,
    bcm_gport_t port,
    bcm_gport_t gport)
{
    bcm_port_t local_port = -1, resolved_port;
    int hw_cosq, pipe;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);
    
    hw_cosq = -1;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_node_get(unit, gport, 
                            0, NULL, &resolved_port, NULL, &node));
        if (!((node->type == _BCM_AP_NODE_UCAST) ||
              (node->type == _BCM_AP_NODE_MCAST) ||
              (node->type == _BCM_AP_NODE_SCHEDULER))) {
            return BCM_E_PARAM;
        }
        if (resolved_port != local_port) {
            return BCM_E_PARAM;
        }
        hw_cosq = node->hw_cosq;
    } else {
        hw_cosq = gport;
        if (_bcm_ap_cosq_port_has_ets(unit, local_port)) {
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
    if ((hw_cosq < 0) || (hw_cosq >= _AP_NUM_INTERNAL_PRI)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ap_pfc_class_mapping_check
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
_bcm_ap_pfc_class_mapping_check(int unit,
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
        if ((cur_class < 0) || (cur_class >= _BCM_AP_NUM_PFC_CLASS)) {
            return BCM_E_PARAM;
        }
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            BCM_IF_ERROR_RETURN(_bcm_ap_fc_map_gport_check(unit,
                                                            port, cur_gport));
        }
    }

    return BCM_E_NONE;
}


int
bcm_ap_cosq_port_pfc_op(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                        bcm_gport_t *gport, int gport_count)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port, resolved_port;
    int rv, type, class = -1, id, index, hw_cosq ;
    uint32 profile_index, old_profile_index;
    uint64 rval64[16], tmp, *rval64s[1], rval, index_map;
    uint32 fval, cos_bmp, tmp32;
    int hw_index;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    soc_reg_t   reg;
    int phy_port, mmu_port, lvl, pipe;
    soc_info_t *si;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r 
    };

    if (gport_count < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));


    for (index = 0; index < gport_count; index++) {
        BCM_IF_ERROR_RETURN(_bcm_ap_fc_map_gport_check(unit,port, gport[index]));
    }

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_pfc_class_resolve(sctype, &type, &class));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    mmu_info = _bcm_ap_mmu_info[unit];
    port_info = &mmu_info->port_info[local_port];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);

    cos_bmp = 0;
    for (index = 0; index < gport_count; index++) {
        hw_index =  -1;
        hw_cosq =  -1;
        if (BCM_GPORT_IS_SET(gport[index])) {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_node_get(unit, gport[index], 
                                0, NULL, &resolved_port, &id, &node));
            hw_index = node->hw_index;
            hw_cosq = node->hw_cosq;
            lvl = node->level;
        } else {
            hw_cosq = gport[index];
            node = &res->p_queue_node[port_info->uc_base + hw_cosq];
            node = node->parent; 
            hw_index = node->hw_index;
            lvl = SOC_APACHE_NODE_LVL_L1;
        }
        if ((hw_cosq < 0) || (hw_cosq > 15)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_ap_map_fc_status_to_node(unit, local_port,
                            (op == _BCM_COSQ_OP_CLEAR) ? 0 : _BCM_AP_COSQ_FC_PFC,
                                           mmu_port*4, hw_cosq, hw_index, lvl));
        cos_bmp |= 1 << hw_cosq;
    }

    if (op == _BCM_COSQ_OP_CLEAR) {
        cos_bmp = (1 << _AP_NUM_INTERNAL_PRI) - 1;
    }

    reg = llfc_cfgr[mmu_port/32];
    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));
    if (op == _BCM_COSQ_OP_SET || cos_bmp != 0) {
        index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
        COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
        COMPILER_64_TO_32_LO(tmp32, index_map);
        old_profile_index = (tmp32 & 3)*16;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_get(unit, _bcm_ap_llfc_profile[unit],
                                 old_profile_index, 16, rval64s));
        if (op == _BCM_COSQ_OP_SET) {
            soc_reg64_field32_set(unit, PRIO2COS_PROFILEr, &rval64[class],
                                  COS_BMPf, cos_bmp);
        } else if (cos_bmp != 0) {
            fval = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[class],
                                         COS_BMPf);
            if (op == _BCM_COSQ_OP_ADD) {
                fval |= cos_bmp;
            } else { /* _BCM_COSQ_OP_DELETE */
                fval &= ~cos_bmp;
            }
            soc_reg64_field32_set(unit, PRIO2COS_PROFILEr, &rval64[class],
                                  COS_BMPf, fval);
        }
        
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, _bcm_ap_llfc_profile[unit],
                                    old_profile_index));
        rv = soc_profile_reg_add(unit, _bcm_ap_llfc_profile[unit], rval64s,
                                 16, &profile_index);
        if (rv < 0) {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, _bcm_ap_llfc_profile[unit],
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

    }

    BCM_IF_ERROR_RETURN(soc_reg64_set(unit, reg, 0, 0, rval));
    return BCM_E_NONE;
}

int
bcm_ap_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    int type = -1, class = -1, hw_cosq, count = 0;
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1], rval, index_map;
    uint32 tmp32, bmp, bmp_len;
    _bcm_ap_mmu_info_t *mmu_info;
    int phy_port, mmu_port;
    soc_info_t *si;
    soc_reg_t   reg;
    int mc_index = 0 ;
    int mcq_offset = 0 ;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_cosq_port_info_t *port_info;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };
    static const soc_field_t self[] = {
        SEL0f, SEL1f, SEL2f, SEL3f
    };
    int j, inv_mapped, hw_index, eindex;
    soc_mem_t mem;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    int port_num = 0 ;

    if (((gport == NULL) && (gport_count > 0)) || actual_gport_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_pfc_class_resolve(sctype, &type, &class));

    si = &SOC_INFO(unit);
    mmu_info = _bcm_ap_mmu_info[unit];
    res = _BCM_AP_PRESOURCES(mmu_info, 0);
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
        (soc_profile_reg_get(unit, _bcm_ap_llfc_profile[unit],
                             profile_index, 16, rval64s));

    bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[class],
                                COS_BMPf);
    bmp_len = soc_reg_field_length(unit, PRIO2COS_PROFILEr, COS_BMPf);
    for (hw_cosq = 0; hw_cosq < bmp_len; hw_cosq++) {
        if (!(bmp & (1 << hw_cosq))) {
            continue;
        }
        if (_bcm_ap_cosq_port_has_ets(unit, local_port)) {
            inv_mapped = 0;
            for (j = _BCM_AP_NUM_PORT_SCHEDULERS; 
                 j < _BCM_AP_NUM_TOTAL_SCHEDULERS; j++) {
                node = &mmu_info->sched_node[j];
                if ((!node->in_use) || (node->local_port != local_port) ||
                    (node->hw_cosq != hw_cosq)) {
                    continue;
                }

                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16)/4;
                if (node->level == SOC_APACHE_NODE_LVL_L0) {
                    mem = MMU_INTFI_XPIPE_FC_MAP_TBL0m;
                    if (IS_AP_HSP_PORT(unit, port)) {
                        hw_index = node->hw_index;
                        /* SC/QM L0 nodes do not support FC */
                        if ((hw_index % 5) > 3) {
                            break;
                        }
                        /* Since 5th L0 index is not FC mapped, each Entry of
                         * FC_MAP_TBL can refer to 20 L0 nodes with each field mapped
                         * to first 4 L0 of each port.
                         */
                        eindex = (hw_index % 20) / 5;
                        hw_index = hw_index / 20;
                    }
                } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
                    mem = MMU_INTFI_XPIPE_FC_MAP_TBL1m;
                    if (IS_AP_HSP_PORT(unit,port)) {
                        hw_index = node->hw_index;
                        if ((hw_index % 10) > 7) {
                            /* FC support only for Q0-7 */
                            break;
                        }
                        port_num = hw_index / 10;
                        hw_index = port_num * 2 + (((hw_index % 10) >= 4)? 1 : 0);
                        eindex = hw_index & 0x3;
                        hw_index = hw_index / 4; /* each entry contains 4 indexes */
                    }
                } else {
                    continue;
                }

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                 /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                         inv_mapped = 1;
                         break;
                    }
                }
            }

            for (j = port_info->uc_base; 
                 inv_mapped==0 && (j < port_info->uc_limit); j++) {
                node = &res->p_queue_node[j];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem =   MMU_INTFI_XPIPE_FC_MAP_TBL2m ;
               if (IS_AP_HSP_PORT(unit,port)) {
                   hw_index = node->hw_index;
                   if ((hw_index % 10) > 7) {
                        /* FC support only for Q0-7 */
                         break;
                    } 
                    port_num = hw_index / 10;
                    hw_index = port_num * 2 +
                                (((hw_index % 10) >= 4)? 1 : 0);
                    eindex = hw_index & 0x3;
                    hw_index = hw_index / 4; /* each entry contains 4 indexes */
               } else 
               { 
                      hw_index = (node->hw_index / 16);
                      eindex = (node->hw_index % 16)/4;
               }
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                 /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {   
                        gport[count] = node->gport;
                    } 
                    count++ ; 
                    if ((count == gport_count) && (gport_count > 0)) {
                         inv_mapped = 1;
                         break;
                    }  
                }
            }
            for (j = port_info->mc_base; 
                inv_mapped==0 && (j < port_info->mc_limit); j++) {
                mc_index = soc_apache_l2_hw_index(unit, j, 0);
                node = &res->p_queue_node[mc_index];
                if ((!node->in_use) || (node->local_port != local_port)) {
                    continue;
                }
                if (node->hw_cosq != hw_cosq) {
                    continue;
                }
                mem =   MMU_INTFI_XPIPE_FC_MAP_TBL2m ;
                mcq_offset = node->hw_index % _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;
                if (((mcq_offset % 10) > 7) ) {
                     /* FC support only for Q0-7 */
                      continue; 
                }
                hw_index = node->hw_index - (mcq_offset * 2 / 10);
                hw_index /= 16;
                eindex = (mcq_offset % 10)/4;
                eindex += ((mcq_offset /10) % 2) ? 0 : 2;
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                 /* coverity[overrun-local : FALSE] */
                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0) {
                        gport[count] = node->gport;
                    }
                    count++;
                    if ((count == gport_count) && (gport_count > 0)) {
                        /* coverity[assigned_value : FALSE] */                        
                        inv_mapped = 1;
                        break;
                    }
                }
            }
        } else {
            if (gport_count > 0) {
                gport[count++] = hw_cosq;
            } 
        }
        if (count == gport_count) {
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
 *     bcm_ap_cosq_pfc_class_mapping_set
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
_bcm_ap_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0, pfc_class[_BCM_AP_NUM_PFC_CLASS];
    bcm_gport_t cur_gport = 0;
    uint32 cos_bmp[_BCM_AP_NUM_PFC_CLASS];
    int local_port = 0 ;

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }
    if ((array_count < 0) || (array_count > _BCM_AP_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN( _bcm_ap_pfc_class_mapping_check(unit,port, 
                        array_count,mapping_array));
    _bcm_ap_clear_fc_status_to_node(unit, port);

    sal_memset(cos_bmp, 0, sizeof(cos_bmp));

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            
            BCM_IF_ERROR_RETURN(_bcm_ap_fc_status_map_gport(unit, port, 
                cur_gport, _BCM_AP_COSQ_FC_PFC, &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_AP_NUM_PFC_CLASS; index++) {
        pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_ap_port_fc_profile_set(unit, port,
        _BCM_COSQ_OP_SET, _BCM_AP_NUM_PFC_CLASS, pfc_class, cos_bmp));
    
    return BCM_E_NONE;
}

int
_bcm_ap_cosq_pfc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    int *array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    int local_port = 0 ;
    bcm_switch_control_t sc[] = { bcmSwitchPFCClass0Queue,
                                  bcmSwitchPFCClass1Queue,
                                  bcmSwitchPFCClass2Queue,
                                  bcmSwitchPFCClass3Queue,
                                  bcmSwitchPFCClass4Queue,
                                  bcmSwitchPFCClass5Queue,
                                  bcmSwitchPFCClass6Queue,
                                  bcmSwitchPFCClass7Queue
                                };

    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }
    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }
    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    for (cur_class = 0; cur_class < _BCM_AP_NUM_PFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        if (array_max > 0) {
            rv = bcm_ap_cosq_port_pfc_get(unit, port, sc[cur_class],
                        mapping_array[class_count].gport_list,
                        BCM_COSQ_PFC_GPORT_COUNT, &actual_gport_count);
        } else {
            rv = bcm_ap_cosq_port_pfc_get(unit, port, sc[cur_class],
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
 *     bcm_ap_cosq_default_llfc_profile_attach
 * Purpose:
 *     Add default entries for PRIO2COS_PROFILE register profile
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_default_llfc_profile_attach(int unit, int port)
{
    soc_reg_t   reg;
    soc_info_t *si;
    bcm_port_t local_port;
    int mmu_port, phy_port;
    uint32 profile_index;
    uint64 tmp, rval, index_map;
    uint64 rval64s[16], *prval64s[1];
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };

    BCM_IF_ERROR_RETURN(
        _bcm_ap_cosq_localport_resolve(unit, port, &local_port));

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    reg = llfc_cfgr[mmu_port/32];

    /* Add default entries for PRIO2COS_PROFILE register profile */
    sal_memset(rval64s, 0, sizeof(rval64s));
    prval64s[0] = rval64s;

    BCM_IF_ERROR_RETURN
            (soc_profile_reg_add(unit, _bcm_ap_llfc_profile[unit],
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
 *     bcm_ap_cosq_llfc_profile_detach
 * Purpose:
 *     Delete old entries for PRIO2COS_PROFILE register profile
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_ap_cosq_llfc_profile_detach(int unit, int port)
{
    soc_reg_t   reg;
    bcm_port_t local_port;
    int mmu_port, phy_port;
    uint32 old_profile_index;
    uint32 tmp32;
    uint64 rval, index_map;
    soc_info_t *si;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r, PORT_PFC_CFG1r, PORT_PFC_CFG2r
    };

    BCM_IF_ERROR_RETURN(
        _bcm_ap_cosq_localport_resolve(unit, port, &local_port));

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
        (soc_profile_reg_delete(unit, _bcm_ap_llfc_profile[unit],
                                old_profile_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_port_priority_group_mapping_set
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
bcm_ap_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
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
    if ((prio < AP_SAFC_INPUT_PRIORITY_MIN) ||
            (prio > AP_SAFC_INPUT_PRIORITY_MAX)) {
        return BCM_E_PARAM;
    }

    if ((priority_group < AP_PRIOROTY_GROUP_ID_MIN) ||
            (priority_group > AP_PRIOROTY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, gport, &port));
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
 *      bcm_ap_port_priority_group_mapping_get
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
bcm_ap_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
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
    if ((prio < AP_SAFC_INPUT_PRIORITY_MIN) ||
            (prio > AP_SAFC_INPUT_PRIORITY_MAX)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ap_cosq_localport_resolve(unit, gport, &port));
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


int
bcm_ap_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    _bcm_ap_mmu_info_t *mmu_info;
    uint32 rval;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(READ_MMU_THDM_DB_QUEUE_E2E_DS_EN_0r(unit, port, &rval));
    soc_reg_field_set(unit, MMU_THDM_DB_QUEUE_E2E_DS_EN_0r, &rval, Q_COS_E2E_DS_ENf,
                      0xff);
    BCM_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_QUEUE_E2E_DS_EN_0r(unit, port,  rval));

    BCM_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, EARLY_E2E_SELECTf,
                      enable ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_voq_min_hw_index(int unit, bcm_port_t port, 
                          bcm_module_t modid, 
                          bcm_port_t remote_port,
                          int *q_offset)
{
    int i, hw_index_base = -1, pipe;
    _bcm_ap_cosq_node_t *node;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_mmu_info_t *mmu_info;

    mmu_info = _bcm_ap_mmu_info[unit];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);

    for (i = res->num_base_queues; i < _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE; i++) {
        node = &res->p_queue_node[i];
        if ((node->in_use == FALSE) || (node->hw_index == -1) || 
                            (node->type != _BCM_AP_NODE_VOQ)) {
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
_bcm_ap_voq_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_ap_cosq_node_t **base_node)
{
    int i,  pipe;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node;

    mmu_info = _bcm_ap_mmu_info[unit];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);
    for ( i  = res->num_base_queues ; 
            i < _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE ; i++) {
        node = &res->p_queue_node[i];
        if ((node->in_use == FALSE) || (node->hw_index == -1)) {
            continue;
        }
        if (node->remote_modid == modid) {
            *base_node = node ;
            break;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_voq_next_base_node_get(int unit, bcm_port_t port, 
        int modid,
        _bcm_ap_cosq_node_t **base_node)
{
    int i,  pipe;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *node;
    int found = FALSE;

    mmu_info = _bcm_ap_mmu_info[unit];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, port);
    for (pipe = _AP_XPIPE ; pipe < _AP_NUM_PIPES ; pipe++) { 
        res = _BCM_AP_PRESOURCES(mmu_info, pipe);
        for ( i  = res->num_base_queues ;
                i < _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE ; i++) {
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
_bcm_ap_port_voq_base_set(int unit, bcm_port_t local_port, int base)
{
    uint64 rval;

    BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
    soc_reg64_field32_set(unit, ING_COS_MODE_64r, &rval, BASE_QUEUE_NUM_1f, base);
    BCM_IF_ERROR_RETURN(WRITE_ING_COS_MODE_64r(unit, local_port, rval));
    return BCM_E_NONE;
}

STATIC int
_bcm_ap_port_voq_base_get(int unit, bcm_port_t local_port, int *base)
{
    uint64 rval;

    BCM_IF_ERROR_RETURN(READ_ING_COS_MODE_64r(unit, local_port, &rval));
    if (soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, QUEUE_MODEf) == 1) {
        *base = soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval, 
                                  BASE_QUEUE_NUM_1f);
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_ap_voq_min_hw_index(unit, local_port, -1, -1, base));

    return BCM_E_NONE;
}

STATIC int
_bcm_ap_resolve_dmvoq_hw_index(int unit, _bcm_ap_cosq_node_t *node, 
                    int cosq, bcm_module_t modid, bcm_port_t local_port)
{
    int try, preffered_offset = -1, alloc_size;
    int from_offset, max_offset,  port, rv, q_base, port_voq_base, pipe;
    _bcm_ap_pipe_resources_t *res;
    _bcm_ap_mmu_info_t *mmu_info;

    mmu_info = _bcm_ap_mmu_info[unit];
    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);

    from_offset  = res->num_base_queues;
    max_offset = _BCM_AP_NUM_L2_UC_LEAVES_PER_PIPE;
    alloc_size = _AP_NUM_COS(unit);

    for (try = 0; try < 2; try++) {
        PBMP_HG_ITER(unit, port) {
            if ((try == 1) && (port == local_port)) {
                continue;
            }
            
            if ((try == 0) && (port != local_port)) {
                continue;
           }

            rv = _bcm_ap_voq_min_hw_index(unit, port, modid, -1, &q_base);
            if (!rv) {
                if (port == local_port) {
                    /* Found VOQ for the same port/dest modid */
                    node->hw_index = q_base + cosq;
                    return BCM_E_NONE;
                } else if (modid == -1) {
                    rv = _bcm_ap_port_voq_base_get(unit, port, &port_voq_base);
                    if (rv) {
                        continue;
                    }
                    preffered_offset = q_base - port_voq_base;
                    rv = _bcm_ap_port_voq_base_get(unit, local_port, 
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
        rv = _bcm_ap_node_index_get(res->l2_queue_list.bits, 
                from_offset, max_offset, 
                alloc_size, _AP_NUM_COS(unit), &q_base);
        if (rv) {
            return BCM_E_RESOURCE;
        }

        if (_bcm_ap_port_voq_base_get(unit, local_port, &port_voq_base)) {
            _bcm_ap_port_voq_base_set(unit, local_port, q_base);
        }
        q_base = q_base + alloc_size - _AP_NUM_COS(unit);
        node->hw_index = q_base + cosq;
        node->hw_cosq = node->hw_index % _AP_NUM_COS(unit);
        node->remote_modid = modid;
        _bcm_ap_node_index_set(&res->l2_queue_list, q_base, _AP_NUM_COS(unit));
    } else {
        BCM_IF_ERROR_RETURN(
                _bcm_ap_voq_min_hw_index(unit, local_port, -1, -1, &port_voq_base));

        if (port_voq_base == node->hw_index) {
            _bcm_ap_port_voq_base_set(unit, local_port, node->hw_index);
        }
        node->remote_modid = modid;
        rv = BCM_E_NONE;
    }
    return rv;
}

typedef struct _bcm_ap_voq_info_s {
    int          valid;
    _bcm_ap_cosq_node_t *node;
    bcm_module_t remote_modid;
    bcm_port_t   remote_port;
    bcm_port_t   local_port;
    int          hw_cosq;
    int          hw_index;
} _bcm_ap_voq_info_t;

STATIC int
_bcm_ap_cosq_sort_voq_list(void *a, void *b)
{
    _bcm_ap_voq_info_t *va, *vb;
    int sa, sb;

    va = (_bcm_ap_voq_info_t*)a;
    vb = (_bcm_ap_voq_info_t*)b;
    
    sa = (va->remote_modid << 19) | (va->remote_port << 11) |
         (va->local_port << 3) | (va->hw_cosq);

    sb = (vb->remote_modid << 19) | (vb->remote_port << 11) |
         (vb->local_port << 3) | (vb->hw_cosq);

    return sa - sb;
}
    
STATIC int
_bcm_ap_get_queue_skip_count_on_cos(uint32 cbmp, int cur_cos, int next_cos)
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
_bcm_ap_msg_buf_get(int unit, bcm_port_t srcid)
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
_bcm_ap_gport_dpvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t remote_modid,
                                bcm_port_t remote_port)
{
    uint32 profile = 0xFFFFFFFF;
    _bcm_ap_cosq_node_t *node;
    bcm_port_t local_port;
    uint32 cng_offset;
    uint8 cbmp = 0, nib_cbmp = 0;
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_voq_info_t *plist = NULL, *vlist = NULL;
    int pcount, ii, vcount = 0;
    voq_port_map_entry_t *voq_port_map_entries = NULL;
    int rv = BCM_E_NONE, mod_base = 0, num_cos = 0, dm = -1;
    voq_mod_map_entry_t voq_mod_map;
    void *entries[1];
    int cng_bytes_per_e2ecc_msg = 64;
    int p2ioff[128], ioff, port, skip, *dm2off, count, pipe;
    _bcm_ap_pipe_resources_t *res;
    voq_cos_map_entry_t voq_cos_map;
    int priority = 0;
    int rp, port_q_base = 0;
    int base_tbl_entry = 0, intf_index = -1;
    uint64 rval64;
    soc_info_t *si;
    int phy_port, mmu_port;

    if ((cosq < 0) || (cosq > _AP_NUM_COS(unit))) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));

    pipe = _BCM_AP_PORT_TO_PIPE(unit, local_port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);

    node->remote_modid = remote_modid;
    node->remote_port = remote_port;
    node->hw_cosq = cosq;

    dm2off = sal_alloc(sizeof(int) * 256, "voq_vlist");
    if (!dm2off) {
        return BCM_E_MEMORY;
    }
    sal_memset(dm2off, 0, sizeof(int)*256);

    /* alloc temp vlist */
    vlist = sal_alloc(sizeof(_bcm_ap_voq_info_t) * _BCM_AP_NUM_L2_UC_LEAVES, "voq_vlist");
    if (!vlist) {
        sal_free(dm2off);
        return BCM_E_MEMORY;
    }
    sal_memset(vlist, 0, sizeof(_bcm_ap_voq_info_t)*_BCM_AP_NUM_L2_UC_LEAVES);

    for (ii = 0; ii < _BCM_AP_NUM_L2_UC_LEAVES; ii++) {
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
    _shr_sort(vlist, vcount, sizeof(_bcm_ap_voq_info_t), 
                _bcm_ap_cosq_sort_voq_list);

    for (ii = 0; ii < 128; ii++) {
        p2ioff[ii] = -1;
    }
    
    si = &SOC_INFO(unit);
    PBMP_HG_ITER(unit, port) {
        /* For DPVOQ, SRC_ID should be the mmu_port number */
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        ioff = _bcm_ap_msg_buf_get(unit, mmu_port);
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

    plist = sal_alloc(sizeof(_bcm_ap_voq_info_t) * _BCM_AP_NUM_L2_UC_LEAVES, "voq_vlist");
    if (!plist) {
        sal_free(dm2off);
        sal_free(vlist);
        return BCM_E_MEMORY;
    }
    sal_memset(plist, 0, sizeof(_bcm_ap_voq_info_t) * _BCM_AP_NUM_L2_UC_LEAVES);

    for (pcount = 0, ii = 0; ii < vcount; ii++) {
        if (pcount % 4) {
            if ((plist[pcount - 1].remote_modid != vlist[ii].remote_modid) ||
                (plist[pcount - 1].remote_port != vlist[ii].remote_port)) {
                pcount = (pcount + 3) & ~3;
            } else if (plist[pcount - 1].local_port != vlist[ii].local_port) {
                skip = (p2ioff[vlist[ii].local_port] - 
                            p2ioff[plist[pcount - 1].local_port] - 1) * num_cos;
                skip +=  _bcm_ap_get_queue_skip_count_on_cos(nib_cbmp, 
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
                pcount += _bcm_ap_get_queue_skip_count_on_cos(nib_cbmp, 
                                        plist[pcount - 1].hw_cosq,
                                        vlist[ii].hw_cosq);
            }
        } else {
            pcount += _bcm_ap_get_queue_skip_count_on_cos(nib_cbmp, -1,
                                                    vlist[ii].hw_cosq);
        }
       
        plist[pcount].valid = 1;
        plist[pcount].local_port = vlist[ii].local_port;
        plist[pcount].remote_modid = vlist[ii].remote_modid;
        plist[pcount].remote_port = vlist[ii].remote_port;
        plist[pcount].hw_cosq = vlist[ii].hw_cosq;
        plist[pcount].node = vlist[ii].node;
        
        #ifdef DEBUG_AP_COSQ
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
    rp = -1;
    for (ii = 0; ii <= pcount; ii++) {
        if (plist[ii].valid == 1) {
            rv = READ_ING_COS_MODE_64r(unit, plist[ii].local_port, &rval64);
            if (rv) {
                goto fail;
            }
            port_q_base = soc_reg64_field32_get(unit, ING_COS_MODE_64r, rval64,
                                BASE_QUEUE_NUM_1f);
        }

        if ((ii == pcount) ||
            ((dm != -1) && (plist[ii].valid) &&
            (plist[ii].remote_modid != dm))) {
            
            rv = READ_VOQ_MOD_MAPm(unit, MEM_BLOCK_ALL, dm, &voq_mod_map);
            if (rv) {
                goto fail;
            }     
            if (soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                                                &voq_mod_map, VOQ_VALIDf)) {
                profile = soc_mem_field32_get(unit, VOQ_MOD_MAPm, 
                              &voq_mod_map, VOQ_MOD_PORT_PROFILE_INDEXf);
                
                rv = soc_profile_mem_delete(unit, 
                            _bcm_ap_voq_port_map_profile[unit], profile*128);
                if (rv) {
                    goto fail;
                }
            }
            rv = soc_profile_mem_add(unit, 
                                _bcm_ap_voq_port_map_profile[unit], 
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
            soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map, 
                                VOQ_MOD_USE_PORTf, 1);
            rv = soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, 
                               dm, &voq_mod_map);
            if (rv) {
                goto fail;
            }

            sal_memset(voq_port_map_entries, 0, sizeof(voq_port_map_entry_t)*128);

            if (ii != pcount) {
                if (plist[ii].node) {
                    mod_base = (plist[ii].node->hw_index & ~3) - port_q_base;
                }
            }
        }

        if (ii == pcount) {
            break;
        }

        if (plist[ii].valid == 0) {
            continue;
        }

        if (dm == -1) {
            if (plist[ii].node) {
                mod_base = (plist[ii].node->hw_index & ~3) - port_q_base;
            }
        }

        dm = plist[ii].remote_modid;

        if ((rp == -1) || 
            (rp != plist[ii].remote_port)) {
            rp = plist[ii].remote_port;
            soc_mem_field32_set(unit, VOQ_PORT_MAPm, 
                                &voq_port_map_entries[plist[ii].remote_port], 
                                VOQ_PORT_OFFSETf, 
                                (plist[ii].node->hw_index - mod_base - port_q_base) & 0x3fff);
        }
    }

    for (priority = 0; priority < 16; priority++) {
        rv = READ_VOQ_COS_MAPm(unit, MEM_BLOCK_ALL, priority, &voq_cos_map);
        if (rv) {
            goto fail;
        }
        
        /* Setting the  VOQ_PORT_BASE_SELECT,
           so that BASE_QUEUE_NUM_1 is used for queue
           calculation */

        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                &voq_cos_map, VOQ_PORT_BASE_SELECTf, pcount ? 1 : 0);
        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                            &voq_cos_map, VOQ_COS_USE_MODf, pcount ? 1 : 0);
        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                            &voq_cos_map, VOQ_COS_USE_PORTf, pcount ? 1 : 0);
 
        rv = soc_mem_write(unit, VOQ_COS_MAPm, MEM_BLOCK_ANY, priority,
                    &voq_cos_map);
        if (rv) {
            goto fail;
        }
    }

    dm = -1;
    for (ii = 0; ii < pcount; ii += 1) {
        if (plist[ii].valid == 0) {
            continue;
        }
        phy_port = si->port_l2p_mapping[plist[ii].local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        cng_offset = (dm2off[plist[ii].remote_modid] - 1) * 
                        cng_bytes_per_e2ecc_msg;

        /*
         * Base index table addr = {< interface number>,BUF_CFG<interface number>__PROFILE_ID}
         * If incoming message mmu port number matches say interface 4's SRC_ID,
         * then it will address base_index_table as {0, BUF_CFG4_PROFILE_ID}.
         * Since profile_id is 2 bits, we index base_index_table in multiple of 4.
         */

        intf_index = _bcm_ap_msg_buf_get(unit, mmu_port);
        if (intf_index < 0) {
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, MMU_INTFI_BASE_INDEX_TBLm, 
                        MEM_BLOCK_ALL, intf_index * 4, &base_tbl_entry));
        
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                BASE_ADDRf, cng_offset);
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                ENf, 2);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_BASE_INDEX_TBLm,
                    MEM_BLOCK_ALL, intf_index * 4, &base_tbl_entry));

        /* Each byte of message should correspond to 2 entries in FC_ST_TBL2.
         * Hence port * 2.
         */
        _bcm_ap_map_fc_status_to_node(unit, 
                                       plist[ii].local_port,
                                       _BCM_AP_COSQ_FC_E2ECC,
                                       cng_offset + (plist[ii].remote_port * 2), 
                                       plist[ii].hw_cosq,
                                       plist[ii].node->hw_index,
                                       SOC_APACHE_NODE_LVL_L2);
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
_bcm_ap_gport_dmvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t fabric_modid, 
                                bcm_module_t dest_modid,
                                bcm_port_t fabric_port)
{
    int port_voq_base, intf_index;
    _bcm_ap_cosq_node_t *node;
    _bcm_ap_cosq_node_t *base_node = NULL;
    bcm_port_t local_port;
    voq_mod_map_entry_t voq_mod_map_entry;
    mmu_intfi_offset_map_tbl_entry_t offset_map_tbl_entry;
    mmu_intfi_base_index_tbl_entry_t base_tbl_entry;
    uint32 cng_offset = 0;
    _bcm_ap_mmu_info_t *mmu_info;
    int fabric_port_count, count = 0;
    int map_offset = 0;
    int port = 0;

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    if (fabric_modid >= 64) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, &local_port, NULL, &node));

    if (!node) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_ap_resolve_dmvoq_hw_index(unit, node, cosq,
                dest_modid, local_port));
    BCM_IF_ERROR_RETURN(_bcm_ap_voq_base_node_get(
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
    BCM_IF_ERROR_RETURN(_bcm_ap_port_voq_base_get(unit, local_port, 
                &port_voq_base));
    soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map_entry,
            VOQ_VALIDf, 1);
    soc_mem_field32_set(unit, VOQ_MOD_MAPm, &voq_mod_map_entry,
                    VOQ_MOD_OFFSETf, ((node->hw_index & ~7) - port_voq_base));

    BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, VOQ_MOD_MAPm, MEM_BLOCK_ALL, dest_modid, 
                &voq_mod_map_entry));

    intf_index = _bcm_ap_msg_buf_get(unit, fabric_modid);
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
    BCM_IF_ERROR_RETURN(_bcm_ap_map_fc_status_to_node(unit, local_port,
                _BCM_AP_COSQ_FC_VOQFC, cng_offset + map_offset, 
                cosq, node->hw_index, SOC_APACHE_NODE_LVL_L2));

    #ifdef DEBUG_AP_COSQ
    LOG_CLI((BSL_META_U(unit,
                        "Gport=0x%08x hw_index=%d cosq=%d\n"),
             gport, node->hw_index, cosq));
    #endif

    return BCM_E_NONE;
}

int bcm_ap_cosq_gport_congestion_config_set(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    _bcm_ap_mmu_info_t *mmu_info;

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    /* determine if this gport is DPVOQ or DMVOQ */
    if ((config->fabric_port != -1) && (config->dest_modid != -1)) {
        return _bcm_ap_gport_dmvoq_config_set(unit, gport, cosq, 
                                               config->fabric_modid,
                                               config->dest_modid,
                                               config->fabric_port);
    } else if ((config->dest_modid != -1) && (config->dest_port != -1)) {
        return _bcm_ap_gport_dpvoq_config_set(unit, gport, cosq, 
                                               config->dest_modid,
                                               config->dest_port);
    }
    
    return BCM_E_PARAM;
}

int bcm_ap_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    _bcm_ap_mmu_info_t *mmu_info;
    bcm_port_t local_port;
    _bcm_ap_cosq_node_t *node;

    if (!config) {
        return BCM_E_PARAM;
    }

    if ((mmu_info = _bcm_ap_mmu_info[unit]) == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
      (_bcm_ap_cosq_node_get(unit, port, 0, NULL, &local_port, NULL, &node));


    if (!node) {
        return BCM_E_NOT_FOUND;
    }

    config->dest_modid = node->remote_modid;
    config->dest_port = node->remote_port;

    return BCM_E_NONE;
}



int bcm_ap_cosq_bst_profile_set(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(_bcm_bst_cmn_profile_set
        (unit, port, cosq, BCM_BST_DUP_XPE, bid, profile));
    return BCM_E_NONE;
}

int bcm_ap_cosq_bst_profile_get(int unit, 
                                 bcm_gport_t port, 
                                 bcm_cos_queue_t cosq, 
                                 bcm_bst_stat_id_t bid,
                                 bcm_cosq_bst_profile_t *profile)
{
    BCM_IF_ERROR_RETURN(_bcm_bst_cmn_profile_get
        (unit, port, cosq, BCM_BST_DUP_XPE, bid, profile));
    return BCM_E_NONE;
}

int bcm_ap_cosq_bst_stat_get(int unit, 
                              bcm_gport_t port, 
                              bcm_cos_queue_t cosq, 
                              bcm_bst_stat_id_t bid, 
                              uint32 options,
                              uint64 *pvalue)
{
    return _bcm_bst_cmn_stat_get(unit, port, cosq, -1, bid, options, pvalue);
}

int bcm_ap_cosq_bst_stat_multi_get(int unit,
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

int bcm_ap_cosq_bst_stat_clear(int unit, bcm_gport_t port, 
                            bcm_cos_queue_t cosq, bcm_bst_stat_id_t bid)
{
    return _bcm_bst_cmn_stat_clear(unit, port, cosq, bid);
}

int bcm_ap_cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid)
{
    return _bcm_bst_cmn_stat_sync(unit, bid);
}


/*
 * Function:
 *      _bcm_ap_cosq_endpoint_l2_create
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
_bcm_ap_cosq_endpoint_l2_create(
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
 *      _bcm_ap_cosq_endpoint_l2_destroy
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
_bcm_ap_cosq_endpoint_l2_destroy(
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
 *      _bcm_ap_cosq_endpoint_ip4_create
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
_bcm_ap_cosq_endpoint_ip4_create(
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
 *      _bcm_ap_cosq_endpoint_ip4_destroy
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
_bcm_ap_cosq_endpoint_ip4_destroy(
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
 *      _bcm_ap_cosq_endpoint_ip6_create
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
_bcm_ap_cosq_endpoint_ip6_create(
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
 *      _bcm_ap_cosq_endpoint_ip6_destroy
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
_bcm_ap_cosq_endpoint_ip6_destroy(
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

#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_ap_cosq_endpoint_gport_create
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
_bcm_ap_cosq_endpoint_gport_create(
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
 *      _bcm_ap_cosq_endpoint_gport_destroy
 * Purpose:
 *      Destroy a GPORT endpoint.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      gport       - (IN) GPORT ID 
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_ap_cosq_endpoint_gport_destroy(
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
 *      bcm_ap_cosq_endpoint_create
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
bcm_ap_cosq_endpoint_create(
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
        if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
            return BCM_E_PARAM;
        }
        if (_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
            return BCM_E_EXISTS;
        }
    } else {
        /* Get a free endpoint ID */
        for (i = 0; i < _BCM_AP_NUM_ENDPOINT(unit); i++) {
            if (!_BCM_AP_ENDPOINT_IS_USED(unit, i)) {
                endpoint_id = i;
                break;
            }
        }
        if (i == _BCM_AP_NUM_ENDPOINT(unit)) {
            /* No available endpoint ID */
            return BCM_E_RESOURCE;
        }
    }
    _BCM_AP_ENDPOINT(unit, endpoint_id) = sal_alloc(
            sizeof(_bcm_ap_endpoint_t), "Endpoint Info");
    if (NULL == _BCM_AP_ENDPOINT(unit, endpoint_id)) {
        return BCM_E_MEMORY;
    }
    sal_memset(_BCM_AP_ENDPOINT(unit, endpoint_id), 0,
            sizeof(_bcm_ap_endpoint_t));

    if (classifier->flags & BCM_COSQ_CLASSIFIER_L2) {
        _BCM_AP_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_L2;
        _BCM_AP_ENDPOINT(unit, endpoint_id)->vlan = classifier->vlan;
        sal_memcpy(_BCM_AP_ENDPOINT(unit, endpoint_id)->mac, classifier->mac,
                sizeof(bcm_mac_t));
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_l2_create(unit,
                    endpoint_id, classifier->vlan, classifier->mac));
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_L3) {
#if defined(INCLUDE_L3)
        _BCM_AP_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_L3;
        _BCM_AP_ENDPOINT(unit, endpoint_id)->vrf = classifier->vrf;
        if (classifier->flags & BCM_COSQ_CLASSIFIER_IP6) {
            _BCM_AP_ENDPOINT(unit, endpoint_id)->flags |= BCM_COSQ_CLASSIFIER_IP6;
            sal_memcpy(_BCM_AP_ENDPOINT(unit, endpoint_id)->ip6_addr,
                    classifier->ip6_addr, BCM_IP6_ADDRLEN);
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_ip6_create(unit,
                        endpoint_id, classifier->vrf, classifier->ip6_addr));
        } else {
            _BCM_AP_ENDPOINT(unit, endpoint_id)->ip_addr = classifier->ip_addr;
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_ip4_create(unit,
                        endpoint_id, classifier->vrf, classifier->ip_addr));
        }
#endif /* INCLUDE_L3 */
    } else if (classifier->flags & BCM_COSQ_CLASSIFIER_GPORT) {
        _BCM_AP_ENDPOINT(unit, endpoint_id)->flags = BCM_COSQ_CLASSIFIER_GPORT;
        _BCM_AP_ENDPOINT(unit, endpoint_id)->gport = classifier->gport;
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_gport_create(unit,
                    endpoint_id, classifier->gport));
    } else {
        return BCM_E_PARAM;
    }

    _BCM_COSQ_CLASSIFIER_ENDPOINT_SET(*classifier_id, endpoint_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_endpoint_destroy
 * Purpose:
 *      Destroy an endpoint.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_ap_cosq_endpoint_destroy(
    int unit, 
    int classifier_id)
{
    int endpoint_id;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_NOT_FOUND;
    }

    if (_BCM_AP_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_L2) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_l2_destroy(unit,
                    _BCM_AP_ENDPOINT(unit, endpoint_id)->vlan,
                    _BCM_AP_ENDPOINT(unit, endpoint_id)->mac));
    } else if (_BCM_AP_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_L3) {
#if defined(INCLUDE_L3)
        if (_BCM_AP_ENDPOINT(unit, endpoint_id)->flags &
                BCM_COSQ_CLASSIFIER_IP6) {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_ip6_destroy(unit,
                        _BCM_AP_ENDPOINT(unit, endpoint_id)->vrf,
                        _BCM_AP_ENDPOINT(unit, endpoint_id)->ip6_addr));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_ip4_destroy(unit,
                        _BCM_AP_ENDPOINT(unit, endpoint_id)->vrf,
                        _BCM_AP_ENDPOINT(unit, endpoint_id)->ip_addr));
        }
#endif
    } else if (_BCM_AP_ENDPOINT(unit, endpoint_id)->flags &
            BCM_COSQ_CLASSIFIER_GPORT) {
        BCM_IF_ERROR_RETURN(_bcm_ap_cosq_endpoint_gport_destroy(unit,
                    _BCM_AP_ENDPOINT(unit, endpoint_id)->gport));
    } else {
        return BCM_E_INTERNAL;
    }

    sal_free(_BCM_AP_ENDPOINT(unit, endpoint_id));
    _BCM_AP_ENDPOINT(unit, endpoint_id) = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_endpoint_get
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
bcm_ap_cosq_endpoint_get(
    int unit, 
    int classifier_id,
    bcm_cosq_classifier_t *classifier)
{
    int endpoint_id;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_NOT_FOUND;
    }

    classifier->flags = _BCM_AP_ENDPOINT(unit, endpoint_id)->flags;
    classifier->vlan = _BCM_AP_ENDPOINT(unit, endpoint_id)->vlan;
    sal_memcpy(classifier->mac, _BCM_AP_ENDPOINT(unit, endpoint_id)->mac,
            sizeof(bcm_mac_t));
    classifier->vrf = _BCM_AP_ENDPOINT(unit, endpoint_id)->vrf;
    classifier->ip_addr = _BCM_AP_ENDPOINT(unit, endpoint_id)->ip_addr;
    sal_memcpy(classifier->ip6_addr,
            _BCM_AP_ENDPOINT(unit, endpoint_id)->ip6_addr, BCM_IP6_ADDRLEN);
    classifier->gport = _BCM_AP_ENDPOINT(unit, endpoint_id)->gport;
   
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_ap_cosq_endpoint_map_set
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
bcm_ap_cosq_endpoint_map_set(
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
    void *entries = NULL;
    endpoint_queue_map_entry_t qmap_key, qmap_data, qmap_entry;
    int qmap_index;
    int old_profile_index, new_profile_index;
    int old_qid;
    int i;
    int priority;
    uint64 rval64;
    uint32 endpoint_cos_map_prof_idx;
    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(queue_group)) {
        qid = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(queue_group);
    } else {
        return BCM_E_PARAM;
    }

    /* Allocate an Endpoint CoS Map profile */
    num_entries_per_profile = 16;
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
        entries = entry_array;
        /* 
         * The index of ENDPOINT_COS_MAP = 
         * ENDPOINT_COS_MAP_PROFILE_INDEX * 16 + INT_PRI
         */
        old_profile_index = endpoint_cos_map_prof_idx * num_entries_per_profile;
        rv = soc_profile_mem_get(unit, _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit),
                old_profile_index, num_entries_per_profile, &entries);
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
        if ((priority < 0) || (priority >= 16)) {
            sal_free(entry_array);
            return BCM_E_PARAM;
        }
        soc_ENDPOINT_COS_MAPm_field32_set(unit, &entry_array[priority],
                ENDPOINT_COS_OFFSETf, cosq_array[i]);
    }
    entries = entry_array;
    rv = soc_profile_mem_add(unit, _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit),
            &entries, num_entries_per_profile, (uint32 *)&new_profile_index);
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
                                    _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit),
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
 *      bcm_ap_cosq_endpoint_map_get
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
bcm_ap_cosq_endpoint_map_get(
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
    void *entries = NULL;
    int profile_index;
    int i;
    int priority;
    uint32 endpoint_cos_map_prof_idx;

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
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
    BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*queue_group, port, qid);

    /* Get Endpoint CoS Map profile */
    num_entries_per_profile = 16;
    alloc_size = sizeof(endpoint_cos_map_entry_t) * num_entries_per_profile;
    entry_array = sal_alloc(alloc_size, "Endpoint CoS Map Profile");
    if (NULL == entry_array) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_array, 0, alloc_size);
    entries = entry_array;
    endpoint_cos_map_prof_idx = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit, &qmap_data,
            ENDPOINT_COS_MAP_PROFILE_INDEXf);
    /*
     * The index of ENDPOINT_COS_MAP =
     * ENDPOINT_COS_MAP_PROFILE_INDEX * 16 + INT_PRI
     */
    profile_index = endpoint_cos_map_prof_idx * num_entries_per_profile;
    rv = soc_profile_mem_get(unit, _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit),
            profile_index, num_entries_per_profile, &entries);
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
            if (priority >= 16 || priority < 0) {
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
 *      bcm_ap_cosq_endpoint_map_clear
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
bcm_ap_cosq_endpoint_map_clear(
    int unit, 
    bcm_gport_t port, 
    int classifier_id)
{
    int endpoint_id;
    endpoint_queue_map_entry_t qmap_key, qmap_data;
    int profile_index;
    int num_entries_per_set = 16; 

    if (!_BCM_COSQ_CLASSIFIER_IS_ENDPOINT(classifier_id)) {
        return BCM_E_PARAM;
    }
    endpoint_id = _BCM_COSQ_CLASSIFIER_ENDPOINT_GET(classifier_id);
    if (endpoint_id >= _BCM_AP_NUM_ENDPOINT(unit)) {
        return BCM_E_PARAM;
    }
    if (!_BCM_AP_ENDPOINT_IS_USED(unit, endpoint_id)) {
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
    profile_index = soc_ENDPOINT_QUEUE_MAPm_field32_get(unit, &qmap_data,
            ENDPOINT_COS_MAP_PROFILE_INDEXf);
    profile_index = profile_index * num_entries_per_set; 
    SOC_IF_ERROR_RETURN(soc_profile_mem_delete(unit,
                _BCM_AP_ENDPOINT_COS_MAP_PROFILE(unit), profile_index));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_field_classifier_get
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
bcm_ap_cosq_field_classifier_get(
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
 *      bcm_ap_cosq_field_classifier_id_create
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
bcm_ap_cosq_field_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    int rv;
    int ref_count = 0;
    int ent_per_set = _AP_NUM_INTERNAL_PRI;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < SOC_MEM_SIZE(unit, IFP_COS_MAPm); i += ent_per_set) {
        rv = soc_profile_mem_ref_count_get(
                unit, _bcm_ap_ifp_cos_map_profile[unit], i, &ref_count);
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
 *      bcm_ap_cosq_field_classifier_map_set
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
bcm_ap_cosq_field_classifier_map_set(
    int unit,
    int classifier_id,
    int count,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array)
{
    int rv;
    int i;
    int max_entries = _AP_NUM_INTERNAL_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_AP_NUM_INTERNAL_PRI];
    int ref_count = 0;
    int field_width = 0 ;  
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
        soc_profile_mem_ref_count_get(unit, _bcm_ap_ifp_cos_map_profile[unit],
                                      index * max_entries, &ref_count));
     
    if (ref_count == 0) {
        rv = soc_profile_mem_add(unit, _bcm_ap_ifp_cos_map_profile[unit],
                                 entries, max_entries, &index);
    } else {
        rv = soc_profile_mem_set(unit, _bcm_ap_ifp_cos_map_profile[unit],
                                 entries, index * max_entries);
        if (rv == SOC_E_NONE) {
            /* decrease the profile ref_count increased by mem_set */
            rv = soc_profile_mem_delete(unit,
                                        _bcm_ap_ifp_cos_map_profile[unit],
                                        index * max_entries);
        }
    }
    return rv;
}


/*
 * Function:
 *      bcm_ap_cosq_field_classifier_map_get
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
bcm_ap_cosq_field_classifier_map_get(
    int unit,
    int classifier_id,
    int array_max,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array,
    int *array_count)
{
    int rv;
    int i;
    int ent_per_set = _AP_NUM_INTERNAL_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_AP_NUM_INTERNAL_PRI];

    /* Input parameter check. */
    if (NULL == priority_array || NULL == cosq_array || NULL == array_count) {
        return BCM_E_PARAM;
    }

    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * ent_per_set);
    entries[0] = ent_buf;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Read out entries from profile table into allocated buffer. */
    rv = soc_profile_mem_get(unit, _bcm_ap_ifp_cos_map_profile[unit],
                             index * ent_per_set, ent_per_set, entries);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    *array_count = array_max > ent_per_set ? ent_per_set : array_max;

    /* Copy values into API OUT parameters. */
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i] >= _AP_NUM_INTERNAL_PRI) {
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
 *      bcm_ap_cosq_field_classifier_map_clear
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
bcm_ap_cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    int ent_per_set = _AP_NUM_INTERNAL_PRI;
    uint32 index;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Delete the profile entries set. */
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_ap_ifp_cos_map_profile[unit],
                                index * ent_per_set));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_ap_cosq_field_classifier_id_destroy
 * Purpose:
 *      Free resource associated with this field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_ap_cosq_field_classifier_id_destroy(int unit, int classifier_id)
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
bcm_ap_cosq_cpu_cosq_enable_set(
    int unit, 
    bcm_cos_queue_t cosq, 
    int enable)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int i, index, enable_status;
    _bcm_ap_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    soc_mem_t thdm_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIG_0m,
        MMU_THDM_MCQE_QUEUE_CONFIG_0m
    };

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_ap_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }
    if (enable) {
        enable = 1;
    } 

    BCM_IF_ERROR_RETURN(bcm_ap_cosq_cpu_cosq_enable_get(unit, cosq, &enable_status));
    if (enable == enable_status) {
        return BCM_E_NONE;
    }

    /* CPU MC cosq's range from 17114 - 17161. 16384 -17113 are MC's of Regular ports */
    index = 730 + cosq; /* 17114-16384*/

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
            cpu_cosq->q_color_limit_enable[i] = soc_mem_field32_get(unit,
                                                              thdm_mem[i], &entry,
                                                              Q_COLOR_LIMIT_ENABLEf);  
            soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_MIN_LIMITf, 0);
            soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_SHARED_LIMITf, 0);
        }
        soc_mem_field32_set(unit, thdm_mem[i], &entry,
                            Q_LIMIT_DYNAMICf, enable ? cpu_cosq->q_limit_dynamic[i] : 0);
        soc_mem_field32_set(unit, thdm_mem[i], &entry,
                            Q_LIMIT_ENABLEf, enable ? cpu_cosq->q_limit_enable[i] : 1);
        soc_mem_field32_set(unit, thdm_mem[i], &entry,
                            Q_COLOR_LIMIT_ENABLEf, enable ? cpu_cosq->q_color_limit_enable[i] : 1);
        cpu_cosq->enable = enable;
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, thdm_mem[i], MEM_BLOCK_ALL, index, &entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_cpu_cosq_enable_get
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
bcm_ap_cosq_cpu_cosq_enable_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *enable)
{
    _bcm_ap_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_field_t dynamic_enable;
    soc_info_t *si;
    int index, hw_enable;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t min_limit, shared_limit, limit_enable;
    soc_mem_t thd_mem = MMU_THDM_DB_QUEUE_CONFIG_0m;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_ap_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }

    /* CPU MC cosq's range from 17114 - 17161.
       16384 -17113 are MC's of Regular ports */
    index = 730 + cosq; /* 17114-16384*/
    hw_enable = 1;

    /*sync up software record with hardware status*/
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, thd_mem, MEM_BLOCK_ALL, 
                        index, &entry));
    min_limit = soc_mem_field32_get( unit, thd_mem, &entry, 
                        Q_MIN_LIMITf); 
    shared_limit = soc_mem_field32_get(unit, thd_mem, &entry, 
                        Q_SHARED_LIMITf);
    limit_enable = soc_mem_field32_get(unit, thd_mem, &entry, 
                        Q_LIMIT_ENABLEf);

    dynamic_enable = soc_mem_field32_get(unit, thd_mem, &entry,
                        Q_LIMIT_DYNAMICf);
    if (limit_enable && (dynamic_enable == 0)&&
        (min_limit == 0) && (shared_limit == 0)) {
        hw_enable = 0;
    }
    cpu_cosq->enable = hw_enable;

    *enable = cpu_cosq->enable;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_flex_port_configure
 * Purpose
 *  Delete old default tree and create new tree as per the port type
 *  Reconfigure all the queues of the port and configure the oversub config 
 *  if required
 * Parameters:
 *      unit              -   (IN) Unit number.
 *      old_port_list     -   (IN) List old ports getting deleted 
 *      num_of_old_ports  -   (IN) Number of old ports getting deleted 
 *      new_port_list     -   (IN) List new ports getting added 
 *      num_of_new_ports  -   (IN) Number of new ports getting added 
 *      num_port_flags    -   (IN) new ports flags 
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_ap_cosq_flex_port_update(
        int unit,
        bcm_port_t port, int enable) 
{

    _bcm_ap_mmu_info_t *mmu_info;
    soc_info_t *si;
    int qnum = 0 ;
    _bcm_ap_pipe_resources_t *res; 
    int phy_port,mmu_port;
    int id; 
    uint64 rval64;
    int pipe;

    si = &SOC_INFO(unit);
    mmu_info = _bcm_ap_mmu_info[unit];
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    pipe = _BCM_AP_PORT_TO_PIPE(unit, port);
    res = _BCM_AP_PRESOURCES(mmu_info, pipe);
    if (enable) { 
        si->port_num_cosq[port] = 10;
        si->port_cosq_base[port] =  (mmu_port * si->port_num_cosq[port]);
        if(IS_AP_HSP_PORT(unit, port)) {
           si->port_num_uc_cosq[port] = 10;
           si->port_uc_cosq_base[port] = mmu_port * si->port_num_cosq[port]; 
        } else { 
           si->port_num_uc_cosq[port] = 16;
           BCM_IF_ERROR_RETURN(
            _bcm_ap_node_index_get(res->l2_queue_list.bits, 160,
                     mmu_info->num_l2_queues, si->port_num_uc_cosq[port],
                     8, &id));
           _bcm_ap_node_index_set(&res->l2_queue_list, id, si->port_num_uc_cosq[port]);
           si->port_uc_cosq_base[port] = id;   
        }

        mmu_info->port_info[port].resources = &mmu_info->pipe_resources[pipe];
        mmu_info->port_info[port].mc_base = si->port_cosq_base[port];
        mmu_info->port_info[port].mc_limit = si->port_cosq_base[port] + 
                                             si->port_num_cosq[port];
        mmu_info->port_info[port].uc_base = si->port_uc_cosq_base[port];
        mmu_info->port_info[port].uc_limit = si->port_uc_cosq_base[port] + 
                                             si->port_num_uc_cosq[port];
        COMPILER_64_ZERO(rval64);
        if(IS_AP_HSP_PORT(unit, port)) {
           SOC_IF_ERROR_RETURN(soc_apache_sched_hw_index_get(unit,
                       port, SOC_APACHE_NODE_LVL_L1, 0, &qnum));
        } else {
            qnum = soc_apache_logical_qnum_hw_qnum(unit, port,
                     si->port_uc_cosq_base[port], 1);
        }
        soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                          &rval64, BASE_QUEUE_NUM_0f, qnum);
        soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                          &rval64, BASE_QUEUE_NUM_1f, qnum);
        BCM_IF_ERROR_RETURN(soc_reg_set(unit, ING_COS_MODE_64r, port, 0, rval64));
    } else {
        if(!IS_AP_HSP_PORT(unit, port)) {
           _bcm_ap_node_index_clear(&res->l2_queue_list, 
                              si->port_uc_cosq_base[port], si->port_num_uc_cosq[port]);
        } else {
            /* Reset to default Mode 0 */
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                        HSP_SCHED_PORT_CONFIGr, port, MC_GROUP_MODEf, 0));
        }

        si->port_num_cosq[port] = 0;
        si->port_cosq_base[port] = 0;
        si->port_num_uc_cosq[port] = 0;
        si->port_uc_cosq_base[port] = 0;

        COMPILER_64_ZERO(rval64);
        mmu_info->port_info[port].resources = &mmu_info->pipe_resources[pipe];
        mmu_info->port_info[port].mc_base = si->port_cosq_base[port];
        mmu_info->port_info[port].mc_limit = si->port_cosq_base[port] + 
                                             si->port_num_cosq[port];
        mmu_info->port_info[port].uc_base = si->port_uc_cosq_base[port];
        mmu_info->port_info[port].uc_limit = si->port_uc_cosq_base[port] + 
                                             si->port_num_uc_cosq[port];
        soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                     &rval64, BASE_QUEUE_NUM_0f, 0);
        soc_reg64_field32_set(unit, ING_COS_MODE_64r,
                                     &rval64, BASE_QUEUE_NUM_1f, 0);
        BCM_IF_ERROR_RETURN(soc_reg_set(unit, ING_COS_MODE_64r, port, 0, rval64));
   }

   BCM_IF_ERROR_RETURN(bcm_ap_port_cosq_config_set(unit, port, enable));
   return BCM_E_NONE; 

}

int
_bcm_ap_cosq_gport_dump_subtree( int unit, bcm_gport_t gport)
{
    _bcm_ap_cosq_node_t *node;
    int num_spri = 0, first_child = 0, first_mc_child;
    uint32 ucmap, spmap;
    soc_apache_sched_mode_e sched_mode;
    int wt = 0;
    char *lvl_name[] = { "Root", "S1",  "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};


    BCM_IF_ERROR_RETURN
        (_bcm_ap_cosq_node_get(unit, gport, 0, NULL, NULL, NULL, &node));

   if (( node->level != SOC_APACHE_NODE_LVL_ROOT) && 
           (node->level != SOC_APACHE_NODE_LVL_L2)) {
        BCM_IF_ERROR_RETURN(
                soc_apache_cosq_get_sched_child_config(unit, node->local_port,
                    node->level,
                    node->hw_index,
                    &num_spri, &first_child,
                    &first_mc_child, &ucmap, &spmap));
    }
    sched_mode = 0;
    if (node->level != SOC_APACHE_NODE_LVL_ROOT) { 
        SOC_IF_ERROR_RETURN(
                soc_apache_cosq_get_sched_mode(unit, node->local_port, node->level, 
                    node->hw_index, &sched_mode, &wt));
    }
    if (node->level == SOC_APACHE_NODE_LVL_ROOT) {
        if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) 
        {
             LOG_CLI((BSL_META_U(unit,
                      "  %s.%d : INDEX=%d NUM_SPRI=0 FC=0 MODE=X Wt=0\n"),
                      lvl_name[node->level], 
                      node->attached_to_input,
                      _soc_apache_root_scheduler_index(unit, node->local_port)));
        }
    } else if (node->level == SOC_APACHE_NODE_LVL_S1) {
        if (soc_feature(unit, soc_feature_mmu_hqos_four_level)) 
        {
            LOG_CLI((BSL_META_U(unit,
                     "    %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                    lvl_name[node->level], 
                    node->attached_to_input, 
                    node->hw_index, 
                    num_spri, first_child,
                    sched_modes[sched_mode],
                    wt));
         } else 
         {
            LOG_CLI((BSL_META_U(unit,
                     "    %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                    lvl_name[node->level - 1], 
                    node->attached_to_input, 
                    node->hw_index, 
                    num_spri, first_child,
                    sched_modes[sched_mode],
                    wt));
         } 

    } else if (node->level == SOC_APACHE_NODE_LVL_L0) {
       LOG_CLI((BSL_META_U(unit,
                        "      %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                    lvl_name[node->level], 
                    node->attached_to_input, 
                    node->hw_index, 
                    num_spri, first_child,
                    sched_modes[sched_mode],
                    wt));

    } else if (node->level == SOC_APACHE_NODE_LVL_L1) {
        LOG_CLI((BSL_META_U(unit,
                        "        %s.%d : INDEX=%d NUM_SP=%d FC=%d "
                        "FMC=%d UCMAP=0x%08x MODE=%s WT=%d\n"),
                    lvl_name[node->level], 
                    node->attached_to_input, 
                    node->hw_index, 
                    num_spri, 
                    first_child, 
                    first_mc_child, 
                    ucmap, 
                    sched_modes[sched_mode],
                    wt));
    } else {
        LOG_CLI((BSL_META_U(unit,
                        "         %s.%s : INDEX=%d MODE=%s Wt=%d\n"),
                    lvl_name[node->level],
                    node->hw_index < 16384 ? "uc" : "mc",
                    node->hw_index, 
                    sched_modes[sched_mode],
                    wt));
    }

    if (node->child != NULL) {
        _bcm_ap_cosq_gport_dump_subtree(unit, node->child->gport);
    }


    if (node->sibling != NULL)  {
        _bcm_ap_cosq_gport_dump_subtree(unit, node->sibling->gport);
    }

    return SOC_E_NONE;
}

int
bcm_apache_dump_port_lls_sw( int unit, int port)
{
    _bcm_ap_mmu_info_t *mmu_info;
    _bcm_ap_cosq_node_t *port_info;

    if (_soc_apache_port_sched_type_get(unit, port) == SOC_APACHE_SCHED_HSP) {
        return SOC_E_NONE;
    }
    LOG_CLI((BSL_META_U(unit,
                    "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));

    mmu_info = _bcm_ap_mmu_info[unit];

    /* root node */
    port_info = &mmu_info->sched_node[port];

    if (port_info->gport >= 0) {
        _bcm_ap_cosq_gport_dump_subtree(unit, port_info->gport);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_service_classifier_id_create
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
bcm_ap_cosq_service_classifier_id_create(
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
 *      bcm_ap_cosq_service_classifier_id_destroy
 * Purpose:
 *      free resource associated with this service classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_ap_cosq_service_classifier_id_destroy(
    int unit,
    int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_ap_cosq_service_classifier_get
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
bcm_ap_cosq_service_classifier_get(
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
 *      bcm_ap_cosq_service_map_set
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
bcm_ap_cosq_service_map_set(
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
    int                        i, numq, alloc_size, exists = 0;
    int                        local_port, vid, qid, cos_offset = 0;
    int                        port_offset, port_offset_old;
    int                        qptr_old, qptr_new;
    uint32                     scm_idx, scm_idx_old = 0, spm_idx, spm_idx_old = 0;
    _bcm_ap_cosq_node_t       *node;
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

    if (array_count > _AP_SCM_ENTRIES_PER_SET) {
        return BCM_E_PARAM;
    }
    scm_profile_mem = _bcm_ap_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_ap_service_port_map_profile[unit];

    BCM_IF_ERROR_RETURN(
        _bcm_ap_cosq_node_get(unit, queue_group, 0, NULL,
                               &local_port, &qid, &node));
    if (node->attached_to_input < 0) {
        return BCM_E_PARAM;
    }
    numq = node->parent->numq;
    qid = node->hw_index;

    for (i = 0; i < array_count; i++) {
        if (cosq_array[i] >= numq) {
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

    alloc_size = sizeof(service_port_map_entry_t) * _AP_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    alloc_size = sizeof(service_cos_map_entry_t) * _AP_SCM_ENTRIES_PER_SET;
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
            if (priority_array[i] < _AP_SCM_ENTRIES_PER_SET) {
                soc_mem_field32_set(unit, SERVICE_COS_MAPm,
                                    &scm[priority_array[i]],
                                    SERVICE_COS_OFFSETf, cosq_array[i]);
            }
        }

        rv = soc_profile_mem_add(unit, scm_profile_mem, scm_entries,
                                 _AP_SCM_ENTRIES_PER_SET, &scm_idx);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }
    } else {
        /*
         * only one service_cos_map profile is allowed per vlan/vfi.
         * If the subsequent profile doesn't match the existing one,
         * Then update the old profile, the new map may be added by other port.
         */
        scm_idx_old = _AP_SCM_ENTRIES_PER_SET *
                  soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                      SERVICE_COS_PROFILE_INDEXf);
        rv = soc_profile_mem_get(unit, scm_profile_mem, scm_idx_old,
                                 _AP_SCM_ENTRIES_PER_SET, scm_entries);
        if (rv != SOC_E_NONE) {
            goto cleanup;
        }

        for (i = 0; i < array_count; i++) {
            cos_offset = soc_mem_field32_get(unit, SERVICE_COS_MAPm,
                                             &scm[priority_array[i]],
                                             SERVICE_COS_OFFSETf);
            if (cosq_array[i] != cos_offset) {
                if (priority_array[i] < _AP_SCM_ENTRIES_PER_SET) {
                    soc_mem_field32_set(unit, SERVICE_COS_MAPm,
                            &scm[priority_array[i]],
                            SERVICE_COS_OFFSETf, cosq_array[i]);
                }
            }
        }

        rv = soc_profile_mem_add(unit, scm_profile_mem, scm_entries,
                _AP_SCM_ENTRIES_PER_SET, &scm_idx);
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
        spm_idx_old = _AP_SPM_ENTRIES_PER_SET *
                      soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                          SERVICE_PORT_PROFILE_INDEXf);
        rv = soc_profile_mem_get(unit, spm_profile_mem, spm_idx_old,
                                 _AP_SPM_ENTRIES_PER_SET, spm_entries);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            goto cleanup;
        }

        qptr_old = soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm,
                                       &sqm, SERVICE_QUEUE_PTRf);
        if (qid <= qptr_old) {
            qptr_new = qid - VALID_PORT_OFFSET_MIN;
            for (i = 0; i < _AP_SPM_ENTRIES_PER_SET; i++) {
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
                             spm_entries, _AP_SPM_ENTRIES_PER_SET, &spm_idx);
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
                        scm_idx / _AP_SCM_ENTRIES_PER_SET);
    soc_mem_field32_set(unit, SERVICE_QUEUE_MAPm, &sqm,
                        SERVICE_PORT_PROFILE_INDEXf,
                        spm_idx / _AP_SPM_ENTRIES_PER_SET);
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
 *      bcm_ap_cosq_service_map_get
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
bcm_ap_cosq_service_map_get(
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
    int                        spm_index, scm_index;
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

    scm_profile_mem = _bcm_ap_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_ap_service_port_map_profile[unit];

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ANY, (int)vid, &sqm));

    if (!soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf)) {
        return BCM_E_CONFIG;
    }

    qptr = soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm,
                               &sqm, SERVICE_QUEUE_PTRf);
    spm_index = _AP_SPM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                    SERVICE_PORT_PROFILE_INDEXf);
    scm_index = _AP_SCM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                               SERVICE_COS_PROFILE_INDEXf);

    alloc_size = sizeof(service_port_map_entry_t) * _AP_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    alloc_size = sizeof(service_cos_map_entry_t) * _AP_SCM_ENTRIES_PER_SET;
    scm = sal_alloc(alloc_size, "SERVICE_COS_MAP temp Mem");
    if (scm == NULL) {
        goto cleaup;
    }
    sal_memset(scm, 0, alloc_size);
    scm_entries[0] = scm;

    rv = soc_profile_mem_get(unit, spm_profile_mem,
                             spm_index, _AP_SPM_ENTRIES_PER_SET, spm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleaup;
    }

    rv = soc_profile_mem_get(unit, scm_profile_mem,
                             scm_index, _AP_SCM_ENTRIES_PER_SET, scm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleaup;
    }

    port_offset = soc_mem_field32_get(unit, SERVICE_PORT_MAPm,
                                      &spm[port], SERVICE_PORT_OFFSETf);
    if (port_offset == 0) {
        rv = BCM_E_NOT_FOUND;
        goto cleaup;
    }
    BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*queue_group, port,
                                           (port_offset + qptr));

    *array_count = array_max > _AP_SCM_ENTRIES_PER_SET ?
                   _AP_SCM_ENTRIES_PER_SET : array_max;
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i] >= _AP_SCM_ENTRIES_PER_SET) {
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
 *      bcm_ap_cosq_service_map_clear
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
bcm_ap_cosq_service_map_clear(
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

    scm_profile_mem = _bcm_ap_service_cos_map_profile[unit];
    spm_profile_mem = _bcm_ap_service_port_map_profile[unit];

    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_QUEUE_MAPm, MEM_BLOCK_ANY, (int)vid, &sqm));

    if (!soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm, VALIDf)) {
        return BCM_E_NONE;
    }

    /*  1. program the SERVICE_PORT_MAPm. */
    alloc_size = sizeof(service_port_map_entry_t) * _AP_SPM_ENTRIES_PER_SET;
    spm = sal_alloc(alloc_size, "SERVICE_PORT_MAP temp Mem");
    if (spm == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(spm, 0, alloc_size);
    spm_entries[0] = spm;

    spm_index = _AP_SPM_ENTRIES_PER_SET *
                soc_mem_field32_get(unit, SERVICE_QUEUE_MAPm, &sqm,
                                    SERVICE_PORT_PROFILE_INDEXf);
    rv = soc_profile_mem_get(unit, spm_profile_mem,
                             spm_index, _AP_SPM_ENTRIES_PER_SET, spm_entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        goto cleanup;
    }

    soc_mem_field32_set(unit, SERVICE_PORT_MAPm,
                        &spm[port], SERVICE_PORT_OFFSETf, 0);
    classifier_in_use = 0;
    for (i = 0; i < _AP_SPM_ENTRIES_PER_SET; i++) {
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
                                spm_entries, _AP_SPM_ENTRIES_PER_SET,
                                &spm_index_new);
       if (rv != SOC_E_NONE) {
           goto cleanup;
       }
    }

    /*  2. program the SERVICE_COS_MAPm. */
    if (!classifier_in_use) {
        scm_index = _AP_SCM_ENTRIES_PER_SET *
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
                            spm_index_new / _AP_SPM_ENTRIES_PER_SET);
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
/******************************************************************/
/*                         OBM CLASSIFIER  START                  */
/******************************************************************/

/*
 * Function:
 *      bcm_apache_switch_obm_dscp_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of dscp classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_count              - (IN) Number of dscp entries.
 *      switch_obm_classifier    - (IN) Array of dscp entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_dscp_classifier_mapping_multi_set(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_count,
                 bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int                             i, code_point, ob_priority;
    int                             entry_index[4];
    idb_obm0_dscp_map_port0_entry_t entry[4];
    bcm_switch_obm_classifier_t     *classifier_ptr = NULL;
    soc_mem_t                       mem;

    static const soc_mem_t obm_dscp_map_port_mem[] = {
        IDB_OBM0_DSCP_MAP_PORT0m, IDB_OBM0_DSCP_MAP_PORT1m,
        IDB_OBM0_DSCP_MAP_PORT2m, IDB_OBM0_DSCP_MAP_PORT3m,
        IDB_OBM1_DSCP_MAP_PORT0m, IDB_OBM1_DSCP_MAP_PORT1m,
        IDB_OBM1_DSCP_MAP_PORT2m, IDB_OBM1_DSCP_MAP_PORT3m,
        IDB_OBM2_DSCP_MAP_PORT0m, IDB_OBM2_DSCP_MAP_PORT1m,
        IDB_OBM2_DSCP_MAP_PORT2m, IDB_OBM2_DSCP_MAP_PORT3m,
        IDB_OBM3_DSCP_MAP_PORT0m, IDB_OBM3_DSCP_MAP_PORT1m,
        IDB_OBM3_DSCP_MAP_PORT2m, IDB_OBM3_DSCP_MAP_PORT3m,
        IDB_OBM4_DSCP_MAP_PORT0m, IDB_OBM4_DSCP_MAP_PORT1m,
        IDB_OBM4_DSCP_MAP_PORT2m, IDB_OBM4_DSCP_MAP_PORT3m,
        IDB_OBM5_DSCP_MAP_PORT0m, IDB_OBM5_DSCP_MAP_PORT1m,
        IDB_OBM5_DSCP_MAP_PORT2m, IDB_OBM5_DSCP_MAP_PORT3m,
        IDB_OBM6_DSCP_MAP_PORT0m, IDB_OBM6_DSCP_MAP_PORT1m,
        IDB_OBM6_DSCP_MAP_PORT2m, IDB_OBM6_DSCP_MAP_PORT3m,
        IDB_OBM7_DSCP_MAP_PORT0m, IDB_OBM7_DSCP_MAP_PORT1m,
        IDB_OBM7_DSCP_MAP_PORT2m, IDB_OBM7_DSCP_MAP_PORT3m,
        IDB_OBM8_DSCP_MAP_PORT0m, IDB_OBM8_DSCP_MAP_PORT1m,
        IDB_OBM8_DSCP_MAP_PORT2m, IDB_OBM8_DSCP_MAP_PORT3m
    };
    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    mem = obm_dscp_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    for (i = 0; i < 4; i++) {
         entry_index[i] = 0;
         sal_memset(&entry[i], 0, sizeof(entry[i]));
         SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                             PGW_CL_BLOCK(unit, pgw), i, &entry[i]));
    }

    for (i = 0; i < array_count; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = classifier_ptr->obm_priority;
         if (code_point < BCM_OBM_DSCP_CHUNK_SIZE) {
             soc_mem_field32_set(unit, mem, &entry[0],
                                 offset_ob_prio[code_point], ob_priority);
             entry_index[0] = 1;
         } else if (code_point < 2 * BCM_OBM_DSCP_CHUNK_SIZE ) {
             soc_mem_field32_set(unit, mem, &entry[1],
                                 offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE],
                                 ob_priority);
            entry_index[1] = 1;
         } else if (code_point < 3 * BCM_OBM_DSCP_CHUNK_SIZE) {
             soc_mem_field32_set(unit, mem, &entry[2],
                                 offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE],
                                 ob_priority);
             entry_index[2] = 1;
         } else {
             soc_mem_field32_set(unit, mem, &entry[3],
                                 offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE],
                                 ob_priority);
             entry_index[3] = 1;
         }
    }
    for (i = 0; i < 4; i++) {
         if (entry_index[i]) {
             SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, PGW_CL_BLOCK(unit, pgw),
                                               i, &entry[i]));
         }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_apache_switch_obm_dscp_classifier_mapping_multi_get
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of dscp classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_max                - (IN) Maximum number of entries.
 *      switch_obm_classifier    - (IN/OUT) Array of dscp entries.
 *      array_count              - (OUT) Number of dscp entries.
 * Returns:
 *      BCM_E_xxx
 */
STATIC
int bcm_apache_switch_obm_dscp_classifier_mapping_multi_get(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_max,
                 bcm_switch_obm_classifier_t *switch_obm_classifier,
                 int *array_count)
{
    int                             i, code_point, ob_priority;
    idb_obm0_dscp_map_port0_entry_t entry[4];
    bcm_switch_obm_classifier_t     *classifier_ptr = NULL;
    soc_mem_t                       mem;

    static const soc_mem_t obm_dscp_map_port_mem[] = {
        IDB_OBM0_DSCP_MAP_PORT0m, IDB_OBM0_DSCP_MAP_PORT1m,
        IDB_OBM0_DSCP_MAP_PORT2m, IDB_OBM0_DSCP_MAP_PORT3m,
        IDB_OBM1_DSCP_MAP_PORT0m, IDB_OBM1_DSCP_MAP_PORT1m,
        IDB_OBM1_DSCP_MAP_PORT2m, IDB_OBM1_DSCP_MAP_PORT3m,
        IDB_OBM2_DSCP_MAP_PORT0m, IDB_OBM2_DSCP_MAP_PORT1m,
        IDB_OBM2_DSCP_MAP_PORT2m, IDB_OBM2_DSCP_MAP_PORT3m,
        IDB_OBM3_DSCP_MAP_PORT0m, IDB_OBM3_DSCP_MAP_PORT1m,
        IDB_OBM3_DSCP_MAP_PORT2m, IDB_OBM3_DSCP_MAP_PORT3m,
        IDB_OBM4_DSCP_MAP_PORT0m, IDB_OBM4_DSCP_MAP_PORT1m,
        IDB_OBM4_DSCP_MAP_PORT2m, IDB_OBM4_DSCP_MAP_PORT3m,
        IDB_OBM5_DSCP_MAP_PORT0m, IDB_OBM5_DSCP_MAP_PORT1m,
        IDB_OBM5_DSCP_MAP_PORT2m, IDB_OBM5_DSCP_MAP_PORT3m,
        IDB_OBM6_DSCP_MAP_PORT0m, IDB_OBM6_DSCP_MAP_PORT1m,
        IDB_OBM6_DSCP_MAP_PORT2m, IDB_OBM6_DSCP_MAP_PORT3m,
        IDB_OBM7_DSCP_MAP_PORT0m, IDB_OBM7_DSCP_MAP_PORT1m,
        IDB_OBM7_DSCP_MAP_PORT0m, IDB_OBM7_DSCP_MAP_PORT1m,
        IDB_OBM8_DSCP_MAP_PORT2m, IDB_OBM8_DSCP_MAP_PORT3m,
        IDB_OBM8_DSCP_MAP_PORT2m, IDB_OBM8_DSCP_MAP_PORT3m
    };
    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    mem = obm_dscp_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    for (i = 0; i < 4; i++) {
         sal_memset(&entry[i], 0, sizeof(entry[i]));
         SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                             PGW_CL_BLOCK(unit, pgw), i, &entry[i]));
    }

    for (i = 0; i < array_max; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         if (code_point < BCM_OBM_DSCP_CHUNK_SIZE) {
             ob_priority = soc_mem_field32_get(unit, mem, &entry[0],
                                               offset_ob_prio[code_point]);
         } else if (code_point < 2 * BCM_OBM_DSCP_CHUNK_SIZE) {
             ob_priority = soc_mem_field32_get(unit, mem, &entry[1],
                          offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE]);
         } else if (code_point < 3 * BCM_OBM_DSCP_CHUNK_SIZE) {
             ob_priority = soc_mem_field32_get(unit, mem, &entry[2],
                          offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE]);
         } else {
             ob_priority = soc_mem_field32_get(unit, mem, &entry[3],
                          offset_ob_prio[code_point % BCM_OBM_DSCP_CHUNK_SIZE]);
        }
        classifier_ptr->obm_priority = ob_priority;
   }
   return BCM_E_NONE;

}


/*
 * Function:
 *      bcm_apache_switch_obm_etag_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of etag classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_count              - (IN) Number of etag entries.
 *      switch_obm_classifier    - (IN) Array of etag entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_etag_classifier_mapping_multi_set(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_count,
                 bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int                             i, code_point, ob_priority;
    idb_obm0_etag_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t     *classifier_ptr = NULL;
    soc_mem_t                       mem;

    static const soc_mem_t obm_etag_map_port_mem[] = {
        IDB_OBM0_ETAG_MAP_PORT0m, IDB_OBM0_ETAG_MAP_PORT1m,
        IDB_OBM0_ETAG_MAP_PORT2m, IDB_OBM0_ETAG_MAP_PORT3m,
        IDB_OBM1_ETAG_MAP_PORT0m, IDB_OBM1_ETAG_MAP_PORT1m,
        IDB_OBM1_ETAG_MAP_PORT2m, IDB_OBM1_ETAG_MAP_PORT3m,
        IDB_OBM2_ETAG_MAP_PORT0m, IDB_OBM2_ETAG_MAP_PORT1m,
        IDB_OBM2_ETAG_MAP_PORT2m, IDB_OBM2_ETAG_MAP_PORT3m,
        IDB_OBM3_ETAG_MAP_PORT0m, IDB_OBM3_ETAG_MAP_PORT1m,
        IDB_OBM3_ETAG_MAP_PORT2m, IDB_OBM3_ETAG_MAP_PORT3m,
        IDB_OBM4_ETAG_MAP_PORT0m, IDB_OBM4_ETAG_MAP_PORT1m,
        IDB_OBM4_ETAG_MAP_PORT2m, IDB_OBM4_ETAG_MAP_PORT3m,
        IDB_OBM5_ETAG_MAP_PORT0m, IDB_OBM5_ETAG_MAP_PORT1m,
        IDB_OBM5_ETAG_MAP_PORT2m, IDB_OBM5_ETAG_MAP_PORT3m,
        IDB_OBM6_ETAG_MAP_PORT0m, IDB_OBM6_ETAG_MAP_PORT1m,
        IDB_OBM6_ETAG_MAP_PORT2m, IDB_OBM6_ETAG_MAP_PORT3m,
        IDB_OBM7_ETAG_MAP_PORT0m, IDB_OBM7_ETAG_MAP_PORT1m,
        IDB_OBM7_ETAG_MAP_PORT2m, IDB_OBM7_ETAG_MAP_PORT3m,
        IDB_OBM8_ETAG_MAP_PORT0m, IDB_OBM8_ETAG_MAP_PORT1m,
        IDB_OBM8_ETAG_MAP_PORT2m, IDB_OBM8_ETAG_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf,
        OFFSET2_OB_PRIORITYf, OFFSET3_OB_PRIORITYf,
        OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    mem = obm_etag_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                     PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_count; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = classifier_ptr->obm_priority;
         soc_mem_field32_set(unit, mem, &entry,
                             offset_ob_prio[code_point], ob_priority);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                      PGW_CL_BLOCK(unit, pgw), 0, &entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_etag_classifier_mapping_multi_get
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of etag classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_max                - (IN) Maximum number of entries.
 *      switch_obm_classifier    - (IN/OUT) Array of etag entries.
 *      array_count              - (OUT) Number of etag entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_etag_classifier_mapping_multi_get(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_max,
                 bcm_switch_obm_classifier_t *switch_obm_classifier,
                 int *array_count)
{
    int                             i, code_point, ob_priority;
    idb_obm0_etag_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t     *classifier_ptr = NULL;
    soc_mem_t                       mem;

    static const soc_mem_t obm_etag_map_port_mem[] = {
        IDB_OBM0_ETAG_MAP_PORT0m, IDB_OBM0_ETAG_MAP_PORT1m,
        IDB_OBM0_ETAG_MAP_PORT2m, IDB_OBM0_ETAG_MAP_PORT3m,
        IDB_OBM1_ETAG_MAP_PORT0m, IDB_OBM1_ETAG_MAP_PORT1m,
        IDB_OBM1_ETAG_MAP_PORT2m, IDB_OBM1_ETAG_MAP_PORT3m,
        IDB_OBM2_ETAG_MAP_PORT0m, IDB_OBM2_ETAG_MAP_PORT1m,
        IDB_OBM2_ETAG_MAP_PORT2m, IDB_OBM2_ETAG_MAP_PORT3m,
        IDB_OBM3_ETAG_MAP_PORT0m, IDB_OBM3_ETAG_MAP_PORT1m,
        IDB_OBM3_ETAG_MAP_PORT2m, IDB_OBM3_ETAG_MAP_PORT3m,
        IDB_OBM4_ETAG_MAP_PORT0m, IDB_OBM4_ETAG_MAP_PORT1m,
        IDB_OBM4_ETAG_MAP_PORT2m, IDB_OBM4_ETAG_MAP_PORT3m,
        IDB_OBM5_ETAG_MAP_PORT0m, IDB_OBM5_ETAG_MAP_PORT1m,
        IDB_OBM5_ETAG_MAP_PORT2m, IDB_OBM5_ETAG_MAP_PORT3m,
        IDB_OBM6_ETAG_MAP_PORT0m, IDB_OBM6_ETAG_MAP_PORT1m,
        IDB_OBM6_ETAG_MAP_PORT2m, IDB_OBM6_ETAG_MAP_PORT3m,
        IDB_OBM7_ETAG_MAP_PORT0m, IDB_OBM7_ETAG_MAP_PORT1m,
        IDB_OBM7_ETAG_MAP_PORT2m, IDB_OBM7_ETAG_MAP_PORT3m,
        IDB_OBM8_ETAG_MAP_PORT0m, IDB_OBM8_ETAG_MAP_PORT1m,
        IDB_OBM8_ETAG_MAP_PORT2m, IDB_OBM8_ETAG_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf,
        OFFSET2_OB_PRIORITYf, OFFSET3_OB_PRIORITYf,
        OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    mem = obm_etag_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                     mem, PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_max; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = soc_mem_field32_get(unit, mem, &entry,
                                           offset_ob_prio[code_point]);
         classifier_ptr->obm_priority = ob_priority;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_vlan_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of vlan classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_count              - (IN) Number of vlan entries.
 *      switch_obm_classifier    - (IN) Array of vlan entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_vlan_classifier_mapping_multi_set(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_count,
                 bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int                            i, code_point, ob_priority;
    idb_obm0_pri_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t    *classifier_ptr = NULL;
    soc_mem_t                      mem;

    static const soc_mem_t obm_pri_map_port_mem[] = {
        IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
        IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m,
        IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
        IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m,
        IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
        IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m,
        IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
        IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf,
        OFFSET2_OB_PRIORITYf, OFFSET3_OB_PRIORITYf,
        OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    mem = obm_pri_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                     mem, PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_count; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = classifier_ptr->obm_priority;
         soc_mem_field32_set(unit, mem, &entry,
                             offset_ob_prio[code_point], ob_priority);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                      PGW_CL_BLOCK(unit, pgw), 0, &entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_vlan_classifier_mapping_multi_get
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of vlan classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_max                - (IN) Maximum number of entries.
 *      switch_obm_classifier    - (IN/OUT) Array of vlan entries.
 *      array_count              - (OUT) Number of vlan entries.
 * Returns:
 *      BCM_E_xxx
 */
STATIC
int bcm_apache_switch_obm_vlan_classifier_mapping_multi_get(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_max,
                 bcm_switch_obm_classifier_t *switch_obm_classifier,
                 int *array_count)
{
    int                            i, code_point, ob_priority;
    idb_obm0_pri_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t    *classifier_ptr = NULL;
    soc_mem_t                      mem;

    static const soc_mem_t obm_pri_map_port_mem[] = {
        IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
        IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m,
        IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
        IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m,
        IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
        IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m,
        IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
        IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf,
        OFFSET2_OB_PRIORITYf, OFFSET3_OB_PRIORITYf,
        OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
    };

    mem = obm_pri_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                     PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_max; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = soc_mem_field32_get(unit, mem, &entry,
                                           offset_ob_prio[code_point]);
         classifier_ptr->obm_priority = ob_priority;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_higig2_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of higig2 classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_count              - (IN) Number of higig2 entries.
 *      switch_obm_classifier    - (IN) Array of higig2 entries.
 * Returns:
 *      BCM_E_xxx
 */
STATIC
int bcm_apache_switch_obm_higig2_classifier_mapping_multi_set(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_count,
                 bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    int                            i, code_point, ob_priority;
    idb_obm0_pri_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t    *classifier_ptr = NULL;
    soc_mem_t                      mem;

    static const soc_mem_t obm_pri_map_port_mem[] = {
        IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
        IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m,
        IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
        IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m,
        IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
        IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m,
        IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
        IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    mem = obm_pri_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                     PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_count; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = classifier_ptr->obm_priority;
         soc_mem_field32_set(unit, mem, &entry,
                             offset_ob_prio[code_point], ob_priority);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                      PGW_CL_BLOCK(unit, pgw), 0, &entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_higig2_classifier_mapping_multi_get
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of higig2 classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_max                - (IN) Maximum number of entries.
 *      switch_obm_classifier    - (IN/OUT) Array of higig2 entries.
 *      array_count              - (OUT) Number of higig2 entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_higig2_classifier_mapping_multi_get(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_max,
                 bcm_switch_obm_classifier_t *switch_obm_classifier,
                 int *array_count)
{
    int                            i, code_point, ob_priority;
    idb_obm0_pri_map_port0_entry_t entry;
    bcm_switch_obm_classifier_t    *classifier_ptr = NULL;
    soc_mem_t                      mem;

    static const soc_mem_t obm_pri_map_port_mem[] = {
        IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
        IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m,
        IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
        IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m,
        IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
        IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m,
        IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
        IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m
    };

    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf
    };

    mem = obm_pri_map_port_mem[(phy_port - 1) % _AP_PORTS_PER_PGW];
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                     PGW_CL_BLOCK(unit, pgw), 0, &entry));

    for (i = 0; i < array_max; i++) {
         classifier_ptr = switch_obm_classifier + i;
         code_point = classifier_ptr->obm_code_point;
         ob_priority = soc_mem_field32_get(unit, mem, &entry,
                                           offset_ob_prio[code_point]);
         classifier_ptr->obm_priority = ob_priority;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_apache_switch_obm_field_classifier_mapping_multi_set
 * Purpose:
 *      Match based on destination_mac, destination_mac_mask,
 *      ethertype and ethertype_mask and assign bcm_obm_priority.
 *      for multiple entries of mpls classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_count              - (IN) Number of field entries.
 *      switch_obm_classifier    - (IN) Array of field entries.
 * Returns:
 *      BCM_E_xxx
 */
STATIC
int bcm_apache_switch_obm_field_classifier_mapping_multi_set(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_count,
                 bcm_switch_obm_classifier_t *switch_obm_classifier)
{
    uint64                       rval64;
    uint32                       rval_hi, rval_lo;
    bcm_switch_obm_classifier_t  *classifier_ptr = NULL;
    soc_reg_t                    reg;
    int                          i, pgw_inst;

    static const soc_reg_t obm_proto_control0_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_0r, IDB_OBM1_PROTOCOL_CONTROL_0r,
        IDB_OBM2_PROTOCOL_CONTROL_0r, IDB_OBM3_PROTOCOL_CONTROL_0r,
        IDB_OBM4_PROTOCOL_CONTROL_0r, IDB_OBM5_PROTOCOL_CONTROL_0r,
        IDB_OBM6_PROTOCOL_CONTROL_0r, IDB_OBM7_PROTOCOL_CONTROL_0r,
        IDB_OBM8_PROTOCOL_CONTROL_0r
    };
    static const soc_reg_t obm_proto_control1_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_1r, IDB_OBM1_PROTOCOL_CONTROL_1r,
        IDB_OBM2_PROTOCOL_CONTROL_1r, IDB_OBM3_PROTOCOL_CONTROL_1r,
        IDB_OBM4_PROTOCOL_CONTROL_1r, IDB_OBM5_PROTOCOL_CONTROL_1r,
        IDB_OBM6_PROTOCOL_CONTROL_1r, IDB_OBM7_PROTOCOL_CONTROL_1r,
        IDB_OBM8_PROTOCOL_CONTROL_1r
    };
    static const soc_reg_t obm_proto_control2_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_2r, IDB_OBM1_PROTOCOL_CONTROL_2r,
        IDB_OBM2_PROTOCOL_CONTROL_2r, IDB_OBM3_PROTOCOL_CONTROL_2r,
        IDB_OBM4_PROTOCOL_CONTROL_2r, IDB_OBM5_PROTOCOL_CONTROL_2r,
        IDB_OBM6_PROTOCOL_CONTROL_2r, IDB_OBM7_PROTOCOL_CONTROL_2r,
        IDB_OBM8_PROTOCOL_CONTROL_2r
    };

    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);

    for (i = 0; i < array_count; i++) {
        classifier_ptr = switch_obm_classifier + i;

        reg = obm_proto_control0_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        rval_hi |= ((classifier_ptr->obm_destination_mac[5] & 0xff) << 8);
        rval_hi |= (classifier_ptr->obm_destination_mac[4] & 0xff);
        rval_lo |= ((classifier_ptr->obm_destination_mac[3] & 0xff) << 24);
        rval_lo |= ((classifier_ptr->obm_destination_mac[2] & 0xff) << 16);
        rval_lo |= ((classifier_ptr->obm_destination_mac[1] & 0xff) << 8);
        rval_lo |= (classifier_ptr->obm_destination_mac[0] & 0xff);
        COMPILER_64_SET(rval64, rval_hi, rval_lo);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg,
                                        pgw_inst, i, rval64));

        reg = obm_proto_control1_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        rval_hi |= ((classifier_ptr->obm_destination_mac_mask[5] & 0xff) << 8);
        rval_hi |= (classifier_ptr->obm_destination_mac_mask[4] & 0xff);
        rval_lo |= ((classifier_ptr->obm_destination_mac_mask[3] & 0xff) << 24);
        rval_lo |= ((classifier_ptr->obm_destination_mac_mask[2] & 0xff) << 16);
        rval_lo |= ((classifier_ptr->obm_destination_mac_mask[1] & 0xff) << 8);
        rval_lo |= (classifier_ptr->obm_destination_mac_mask[0] & 0xff);
        COMPILER_64_SET(rval64, rval_hi, rval_lo);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg,
                                        pgw_inst, i, rval64));
        reg = obm_proto_control2_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        rval_lo |= (classifier_ptr->obm_ethertype & 0xffff);
        rval_lo |= ((classifier_ptr->obm_ethertype_mask & 0xffff) << 16);
        rval_hi |= (classifier_ptr->obm_priority & 0x3);
        rval_hi |= (0x1 << 3);
        COMPILER_64_SET(rval64, rval_hi, rval_lo);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg,
                                        pgw_inst, i, rval64));
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_field_classifier_mapping_multi_get
 * Purpose:
 *      Get the mapping of bcm_obm_priority for a given
 *      destination_mac, destination_mac_mask, ethertype and
 *      ethertype mask for multiple entries of field classifier type.
 * Parameters:
 *      unit                     - (IN) Unit number.
 *      phy_port                 - (IN) Physical port.
 *      pgw                      - (IN) PGW index (0 0r 1).
 *      array_max                - (IN) Maximum number of entries.
 *      switch_obm_classifier    - (IN/OUT) Array of field entries.
 *      array_count              - (OUT) Number of field entries.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_field_classifier_mapping_multi_get(
                 int unit,
                 int phy_port,
                 int pgw,
                 int array_max,
                 bcm_switch_obm_classifier_t *switch_obm_classifier,
                 int *array_count)
{
    uint64                       rval64;
    uint32                       rval_hi, rval_lo;
    bcm_switch_obm_classifier_t  *classifier_ptr = NULL;
    soc_reg_t                    reg;
    int                          i, pgw_inst;

    static const soc_reg_t obm_proto_control0_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_0r, IDB_OBM1_PROTOCOL_CONTROL_0r,
        IDB_OBM2_PROTOCOL_CONTROL_0r, IDB_OBM3_PROTOCOL_CONTROL_0r,
        IDB_OBM4_PROTOCOL_CONTROL_0r, IDB_OBM5_PROTOCOL_CONTROL_0r,
        IDB_OBM6_PROTOCOL_CONTROL_0r, IDB_OBM7_PROTOCOL_CONTROL_0r,
        IDB_OBM8_PROTOCOL_CONTROL_0r

    };
    static const soc_reg_t obm_proto_control1_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_1r, IDB_OBM1_PROTOCOL_CONTROL_1r,
        IDB_OBM2_PROTOCOL_CONTROL_1r, IDB_OBM3_PROTOCOL_CONTROL_1r,
        IDB_OBM4_PROTOCOL_CONTROL_1r, IDB_OBM5_PROTOCOL_CONTROL_1r,
        IDB_OBM6_PROTOCOL_CONTROL_1r, IDB_OBM7_PROTOCOL_CONTROL_1r,
        IDB_OBM8_PROTOCOL_CONTROL_1r 
    };
    static const soc_reg_t obm_proto_control2_regs[] = {
        IDB_OBM0_PROTOCOL_CONTROL_2r, IDB_OBM1_PROTOCOL_CONTROL_2r,
        IDB_OBM2_PROTOCOL_CONTROL_2r, IDB_OBM3_PROTOCOL_CONTROL_2r,
        IDB_OBM4_PROTOCOL_CONTROL_2r, IDB_OBM5_PROTOCOL_CONTROL_2r,
        IDB_OBM6_PROTOCOL_CONTROL_2r, IDB_OBM7_PROTOCOL_CONTROL_2r,
        IDB_OBM8_PROTOCOL_CONTROL_2r
    };

    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);

    /* Do validation of params */
    if (array_max > BCM_OBM_CLASSIFIER_NUM_ENTRY_FIELD) {
        *array_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_FIELD;
    } else {
        *array_count = array_max;
    }

    for (i = 0; i < *array_count; i++) {
        classifier_ptr = switch_obm_classifier + i;

        reg = obm_proto_control0_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg,
                                        pgw_inst, i, &rval64));
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        classifier_ptr->obm_destination_mac[5] = (rval_hi >> 8) & 0xff;
        classifier_ptr->obm_destination_mac[4] = rval_hi & 0xff;
        classifier_ptr->obm_destination_mac[3] = (rval_lo >> 24) & 0xff;
        classifier_ptr->obm_destination_mac[2] = (rval_lo >> 16) & 0xff;
        classifier_ptr->obm_destination_mac[1] = (rval_lo >> 8) & 0xff;
        classifier_ptr->obm_destination_mac[0] = rval_lo & 0xff;

        reg = obm_proto_control1_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg,
                                        pgw_inst, i, &rval64));
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        classifier_ptr->obm_destination_mac_mask[5] = (rval_hi >> 8) & 0xff;
        classifier_ptr->obm_destination_mac_mask[4] = rval_hi & 0xff;
        classifier_ptr->obm_destination_mac_mask[3] = (rval_lo >> 24) & 0xff;
        classifier_ptr->obm_destination_mac_mask[2] = (rval_lo >> 16) & 0xff;
        classifier_ptr->obm_destination_mac_mask[1] = (rval_lo >> 8) & 0xff;
        classifier_ptr->obm_destination_mac_mask[0] = rval_lo & 0xff;

        reg = obm_proto_control2_regs[((phy_port - 1) % _AP_PORTS_PER_PGW) / 4];
        COMPILER_64_ZERO(rval64);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg,
                                        pgw_inst, i, &rval64));
        COMPILER_64_TO_32_HI(rval_hi, rval64);
        COMPILER_64_TO_32_LO(rval_lo, rval64);
        classifier_ptr->obm_ethertype = rval_lo & 0xffff;
        classifier_ptr->obm_ethertype_mask = (rval_lo >>16) & 0xffff;
        classifier_ptr->obm_priority = rval_hi & 0x3;
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_classifier_validate
 * Purpose:
 *      Internal function for validation of config params.
 * Parameters:
 *      unit                      - (IN) Unit number.
 *      port                      - (IN) phyical port.
 *      switch_obm_classifier_type- (IN) type of switch_obm_classifier.
 *      array_count               - (IN) Number of elements in switch_obm_classifier.
 *      switch_obm_classifier     - (IN) Array of bcm_switch_obm_classifier_t.
 * Returns:
 *      BCM_E_xxx
 */

STATIC
int bcm_apache_switch_obm_classifier_validate(
    int unit,
    bcm_port_t port,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int *array_count,
    bcm_switch_obm_classifier_t *switch_obm_classifier,
    int set)
{
    int                          i, max_count;
    bcm_switch_obm_classifier_t  *classifier_ptr = NULL;

    if ((port <= 0) || (port > 72)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                 "ERROR: Invalid port %d\n"), port));
        return BCM_E_PORT;
    }

    if (switch_obm_classifier_type == bcmSwitchObmClassifierMpls) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "ERROR: OBM MPLS classifier not supported"
                            " port %d\n"), port));
        return BCM_E_UNAVAIL;
    }

    if (IS_HG_PORT(unit, port)) {
        if (switch_obm_classifier_type == bcmSwitchObmClassifierVlan) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                     "ERROR: OBM Vlan classifier not allowed on Higig"
                     " port %d\n"), port));
            return BCM_E_PARAM;
        }
    } else {
        if (switch_obm_classifier_type == bcmSwitchObmClassifierHigig2) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                     "ERROR: OBM Higig2 classifier not allowed on non Higig"
                     " port %d\n"), port));
            return BCM_E_PARAM;
        }
    }
    switch (switch_obm_classifier_type) {
        case bcmSwitchObmClassifierDscp:
            max_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_DSCP;
            break;
        case bcmSwitchObmClassifierEtag:
            max_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_ETAG;
            break;
        case bcmSwitchObmClassifierVlan:
            max_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_VLAN;
            break;
        case bcmSwitchObmClassifierHigig2:
            max_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_HIGIG2;
            break;
        case bcmSwitchObmClassifierField:
            max_count = BCM_OBM_CLASSIFIER_NUM_ENTRY_FIELD;
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                     "ERROR: switch_obm_classifier_type is not correct on"
                     " port %d\n"), port));
            return BCM_E_PARAM;
    }
    /* For get API array count may be more than max_count */
    if (*array_count > max_count) {
        if (set) {
            return BCM_E_PARAM;
        } else {
            *array_count = max_count;
        }
    }
    for (i = 0; i < *array_count; i++) {
         classifier_ptr = switch_obm_classifier + i;
         if (switch_obm_classifier_type != bcmSwitchObmClassifierField) {
             if (classifier_ptr->obm_code_point >= max_count) {
                 LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                          "ERROR: Invalid obm_code_point configured on"
                          " port %d\n"), port));
                 return BCM_E_PARAM;
             }
         }
         if (set) {
             if (switch_obm_classifier_type == bcmSwitchObmClassifierMpls) {
                 LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                          "ERROR: Invalid obm_classifier configured on"
                          " port %d\n"), port));
                 return BCM_E_PARAM;
             }

             if (classifier_ptr->obm_priority > BCM_OBM_PRIORITY_MAX) {
                 LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                          "ERROR: Invalid obm_priority configured on"
                          " port %d\n"), port));
                 return BCM_E_PARAM;
             }
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_apache_switch_obm_classifier_mapping_multi_set
 * Purpose:
 *      Set the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of same type of classifier.  Maximum
 *      number of entries to be programmed are different depending
 *      type of classifier. For type bcmSwitchObmClassifierField
 *      bcm_obm_code_point will not be used and mapping will happen
 *      based on destination_mac/mask and ethertype/mask.
 * Parameters:
 *      unit                       - (IN) Unit number.
 *      gport                      - (IN) gport for a given port.
 *      switch_obm_classifier_type - (IN) type of switch_obm_classifier.
 *      array_count                - (IN) Number of elements in switch_obm_classifier.
 *      switch_obm_classifier      - (IN) Array of bcm_switch_obm_classifier_t.
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_apache_switch_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t  *switch_obm_classifier)
{
    soc_info_t *si;
    int pgw;
    int phy_port;
    bcm_port_t port;


    si = &SOC_INFO(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &port));
    phy_port = si->port_l2p_mapping[port];
    pgw = si->port_group[port];

    /* Validate port and classifier compatibility */
    BCM_IF_ERROR_RETURN(
       bcm_apache_switch_obm_classifier_validate(unit,
                                             port,
                                             switch_obm_classifier_type,
                                             &array_count,
                                             switch_obm_classifier,
                                             1));

    switch (switch_obm_classifier_type) {
        case bcmSwitchObmClassifierDscp:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_dscp_classifier_mapping_multi_set(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_count,
                                             switch_obm_classifier));
            break;
        case bcmSwitchObmClassifierEtag:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_etag_classifier_mapping_multi_set(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_count,
                                             switch_obm_classifier));
            break;
        case bcmSwitchObmClassifierVlan:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_vlan_classifier_mapping_multi_set(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_count,
                                             switch_obm_classifier));
            break;
        case bcmSwitchObmClassifierHigig2:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_higig2_classifier_mapping_multi_set(
                                              unit,
                                              phy_port,
                                              pgw,
                                              array_count,
                                              switch_obm_classifier));
            break;
        case bcmSwitchObmClassifierField:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_field_classifier_mapping_multi_set(
                                              unit,
                                              phy_port,
                                              pgw,
                                              array_count,
                                              switch_obm_classifier));
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_apache_switch_obm_classifier_mapping_multi_get
 * Purpose:
 *      Get the mapping of bcm_obm_code_point to bcm_obm_priority
 *      for multiple entries of same type of classifier.  Maximum
 *      number of entries to be programmed are different depending
 *      type of classifier. For type bcmSwitchObmClassifierField
 *      bcm_obm_code_point will not be used and mapping will happen
 *      based on destination_mac/mask and ethertype/mask.
 * Parameters:
 *      unit                       - (IN) Unit number.
 *      gport                      - (IN) gport for a given port.
 *      switch_obm_classifier_type - (IN) type of switch_obm_classifier.
 *      array_max                  - (IN) Maximum number of elements in switch_obm_classifier.
 *      switch_obm_classifier      - (IN/OUT) Array of bcm_switch_obm_classifier_t.
 *      array_count                - (OUT) Number of elements in switch_obm_classifier.
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_apache_switch_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t    *switch_obm_classifier,
    int *array_count)

{
    soc_info_t *si;
    int pgw;
    int phy_port;
    bcm_port_t port;

    si = &SOC_INFO(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &port));
    phy_port = si->port_l2p_mapping[port];
    pgw = si->port_group[port];

    /* Validate port and classifier compatibility */
    BCM_IF_ERROR_RETURN(
       bcm_apache_switch_obm_classifier_validate(unit,
                                             port,
                                             switch_obm_classifier_type,
                                             array_count,
                                             switch_obm_classifier,
                                             0));
    switch (switch_obm_classifier_type) {
        case bcmSwitchObmClassifierDscp:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_dscp_classifier_mapping_multi_get(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_max,
                                             switch_obm_classifier,
                                             array_count));
            break;
        case bcmSwitchObmClassifierEtag:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_etag_classifier_mapping_multi_get(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_max,
                                             switch_obm_classifier,
                                             array_count));
            break;
        case bcmSwitchObmClassifierVlan:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_vlan_classifier_mapping_multi_get(
                                             unit,
                                             phy_port,
                                             pgw,
                                             array_max,
                                             switch_obm_classifier,
                                             array_count));
            break;
        case bcmSwitchObmClassifierHigig2:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_higig2_classifier_mapping_multi_get(
                                               unit,
                                               phy_port,
                                               pgw,
                                               array_max,
                                               switch_obm_classifier,
                                               array_count));
            break;
        case bcmSwitchObmClassifierField:
            BCM_IF_ERROR_RETURN(
            bcm_apache_switch_obm_field_classifier_mapping_multi_get(
                                              unit,
                                              phy_port,
                                              pgw,
                                              array_max,
                                              switch_obm_classifier,
                                              array_count));
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_apache_obm_classifier_port_control_set
 * Description:
 *      Internal function to Enable/Disable specified
 *      OBM port feature and congigure ethertype.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Enum value of the OBM feature
 *      value - value to be set
 * Return Value:
 *      BCM_E_XXXX
 */
int
bcm_apache_obm_classifier_port_control_set(int unit,
                                       bcm_port_t port,
                                       bcm_port_control_t type,
                                       int value)
{
    soc_info_t *si;
    int pgw, pgw_inst, subport, obm;
    int phy_port;
    soc_reg_t  reg;
    soc_field_t field;
    int vntag = 0, port_control = 0, val;
    uint32 rval32;

    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr,
        IDB_OBM8_PORT_CONFIGr
    };

    static const soc_reg_t obm_niv_ethertype_config_regs[] = {
        IDB_OBM0_NIV_ETHERTYPEr, IDB_OBM1_NIV_ETHERTYPEr,
        IDB_OBM2_NIV_ETHERTYPEr, IDB_OBM3_NIV_ETHERTYPEr,
        IDB_OBM4_NIV_ETHERTYPEr, IDB_OBM5_NIV_ETHERTYPEr,
        IDB_OBM6_NIV_ETHERTYPEr, IDB_OBM7_NIV_ETHERTYPEr,
        IDB_OBM8_NIV_ETHERTYPEr
    };

    static const soc_reg_t obm_pe_ethertype_config_regs[] = {
        IDB_OBM0_PE_ETHERTYPEr, IDB_OBM1_PE_ETHERTYPEr,
        IDB_OBM2_PE_ETHERTYPEr, IDB_OBM3_PE_ETHERTYPEr,
        IDB_OBM4_PE_ETHERTYPEr, IDB_OBM5_PE_ETHERTYPEr,
        IDB_OBM6_PE_ETHERTYPEr, IDB_OBM7_PE_ETHERTYPEr,
        IDB_OBM8_PE_ETHERTYPEr 
    };

    si = &SOC_INFO(unit);
    /* Add check on ports */
    obm = si->port_serdes[port];
    obm %= _AP_TSCS_PER_PGW;
    phy_port = si->port_l2p_mapping[port];

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) { 
        /*  Do not allow programming on non-idb ports */
        return BCM_E_PARAM;
    }


    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % _AP_PORTS_PER_TSC;

    switch (type) {
        case bcmPortControlObmClassifierPriority:
          port_control = 1;
          field = PORT_PRIf;
          val = value & 0x7;
          break;
        case bcmPortControlObmClassifierEnableDscp:
          port_control = 1;
          field = TRUST_DSCPf;
          val = value & 0x1;
          break;
        case bcmPortControlObmClassifierEnableEtag:
          port_control = 1;
          field = PHB_FROM_ETAGf;
          val = value & 0x1;
          break;
        case bcmPortControlObmClassifierEtagEthertype:
          field = ETHERTYPEf;
          val = value & 0xffff;
          break;
        case bcmPortControlObmClassifierVntagEthertype:
          vntag = 1;
          field = ETHERTYPEf;
          val = value & 0xffff;
          break;
        case bcmPortControlObmClassifierEnableMpls:
          return BCM_E_UNAVAIL;
        default:
          return BCM_E_PARAM;
     }
     if (port_control == 1) {
         reg = obm_port_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         soc_reg_field_set(unit, reg, &rval32, field, val);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     } else if (vntag == 1) {
         reg = obm_niv_ethertype_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         soc_reg_field_set(unit, reg, &rval32, field, val);
         soc_reg_field_set(unit, reg, &rval32, ENABLEf, val ? 1 : 0);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     } else {
         reg = obm_pe_ethertype_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         soc_reg_field_set(unit, reg, &rval32, field, val);
         soc_reg_field_set(unit, reg, &rval32, ENABLEf, val ? 1 : 0);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     }
     return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_apache_obm_classifier_port_control_get
 * Description:
 *      Internal function to  get value specified
 *      OBM port feature and congigure ethertype.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      type - Enum value of the OBM feature
 *      value - value to get
 * Return Value:
 *      BCM_E_XXXX
 */
int
bcm_apache_obm_classifier_port_control_get(int unit,
                                       bcm_port_t port,
                                       bcm_port_control_t type,
                                       int *value)
{
    soc_info_t *si;
    int pgw, pgw_inst, subport, obm;
    int phy_port;
    soc_reg_t  reg;
    soc_field_t field;
    int vntag = 0, port_control = 0, val;
    uint32 rval32;

    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr,
        IDB_OBM8_PORT_CONFIGr
    };

    static const soc_reg_t obm_niv_ethertype_config_regs[] = {
        IDB_OBM0_NIV_ETHERTYPEr, IDB_OBM1_NIV_ETHERTYPEr,
        IDB_OBM2_NIV_ETHERTYPEr, IDB_OBM3_NIV_ETHERTYPEr,
        IDB_OBM4_NIV_ETHERTYPEr, IDB_OBM5_NIV_ETHERTYPEr,
        IDB_OBM6_NIV_ETHERTYPEr, IDB_OBM7_NIV_ETHERTYPEr,
        IDB_OBM8_NIV_ETHERTYPEr
    };

    static const soc_reg_t obm_pe_ethertype_config_regs[] = {
        IDB_OBM0_PE_ETHERTYPEr, IDB_OBM1_PE_ETHERTYPEr,
        IDB_OBM2_PE_ETHERTYPEr, IDB_OBM3_PE_ETHERTYPEr,
        IDB_OBM4_PE_ETHERTYPEr, IDB_OBM5_PE_ETHERTYPEr,
        IDB_OBM6_PE_ETHERTYPEr, IDB_OBM7_PE_ETHERTYPEr,
        IDB_OBM8_PE_ETHERTYPEr 
    };

    si = &SOC_INFO(unit);
    /* Add check on ports */
    obm = si->port_serdes[port];
    obm %= _AP_TSCS_PER_PGW;

    phy_port = si->port_l2p_mapping[port];

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
        /*  Do not allow programming on non-idb ports */
        return BCM_E_PARAM;
    }


    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % _AP_PORTS_PER_TSC;

    switch (type) {
        case bcmPortControlObmClassifierPriority:
          port_control = 1;
          field = PORT_PRIf;
          break;
        case bcmPortControlObmClassifierEnableDscp:
          port_control = 1;
          field = TRUST_DSCPf;
          break;
        case bcmPortControlObmClassifierEnableEtag:
          port_control = 1;
          field = PHB_FROM_ETAGf;
          break;
        case bcmPortControlObmClassifierEtagEthertype:
          field = ETHERTYPEf;
          break;
        case bcmPortControlObmClassifierVntagEthertype:
          vntag = 1;
          field = ETHERTYPEf;
          break;
        case bcmPortControlObmClassifierEnableMpls:
          return BCM_E_UNAVAIL;
        default:
          return BCM_E_PARAM;
     }
     if (port_control == 1) {
         reg = obm_port_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         val = soc_reg_field_get(unit, reg, rval32, field);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     } else if (vntag == 1) {
         reg = obm_niv_ethertype_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         val = soc_reg_field_get(unit, reg, rval32, field);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     } else {
         reg = obm_pe_ethertype_config_regs[obm];
         SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
         val = soc_reg_field_get(unit, reg, rval32, field);
         SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));
     }
     *value = val;
     return BCM_E_NONE;
}

#endif /* defined(BCM_APACHE_SUPPORT) */
