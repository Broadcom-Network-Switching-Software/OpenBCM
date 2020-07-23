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
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/katana.h>
#include <soc/katana2.h>
#include <soc/profile_mem.h>
#include <soc/macutil.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/saber2.h>
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif
    
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/oob.h>
#include <bcm_int/esw/link.h>
#include <bcm/types.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <bcm_int/esw/multicast.h>
#endif

#define KT2_CELL_FIELD_MAX      (SOC_IS_SABER2(unit) ? \
                                0xffff : 0x3ffff)

/* MMU cell size in bytes */
#define _BCM_KT2_COSQ_CELLSIZE   192
#define _BCM_KT2_COSQ_EXT_CELLSIZE  (SOC_IS_SABER2(unit) ? \
        _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE :_BCM_KT2_COSQ_CELLSIZE * 4)
#define _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE  _BCM_KT2_COSQ_CELLSIZE * 15 
#define _BCM_KT2_MAX_PRIORITY_GROUPS 8

#define _BCM_KT2_NUM_UCAST_QUEUE_GROUP            8
#define _BCM_KT2_NUM_VLAN_UCAST_QUEUE_GROUP       16
#define  _BCM_KT2_NUM_INTERNAL_PRI                16 
#define  _BCM_KT2_NUM_PFC_CLASS                   8  
#define _BCM_KT2_NUM_SAFC_CLASS                   16
#define _SOC_KT2_COSQ_MAX_INDEX_S0                128
#define _SOC_KT2_COSQ_MAX_INDEX_S1                256

#if defined (BCM_SABER2_SUPPORT)
#define _SOC_SB2_COSQ_MAX_INDEX_S0                16
#endif

#if defined (BCM_METROLITE_SUPPORT)
#define _SOC_ML_COSQ_MAX_INDEX_S0                 24
#endif

extern kt2_port_to_mxqblock_subports_t *kt2_port_to_mxqblock_subports[SOC_MAX_NUM_DEVICES];

#define _BCM_KT2_COSQ_LIST_NODE_INIT(list, node)        \
    list[node].in_use            = FALSE;               \
    list[node].wrr_in_use        = 0;                   \
    list[node].wrr_mode          = 0;                   \
    list[node].gport             = -1;                  \
    list[node].base_index        = -1;                  \
    list[node].mc_base_index     = -1;                  \
    list[node].numq              = 0;                   \
    list[node].hw_index          = -1;                  \
    list[node].level             = -1;                  \
    list[node].cosq_attached_to  = -1;                  \
    list[node].num_child         = 0;                   \
    list[node].first_child       = 4095;                \
    list[node].linkphy_enabled   = 0;                   \
    list[node].subport_enabled   = 0;                   \
    list[node].cosq_map          = NULL;                \
    list[node].parent            = NULL;                \
    list[node].sibling           = NULL;                \
    list[node].child             = NULL;                \
    list[node].local_port        = -1;                  \
    list[node].wred_disabled     = 0;                   \
    list[node].burst_calculation_enable     = 0;        \
    BCM_PBMP_CLEAR(list[node].fabric_pbmp);              \
    list[node].remote_modid      = -1;                  \
    list[node].fabric_modid      = -1;                  \
    list[node].remote_port      = -1;

#define _BCM_KT2_COSQ_NODE_INIT(node)                   \
    node->in_use            = FALSE;                    \
    node->wrr_in_use        = 0;                        \
    node->wrr_mode          = 0;                        \
    node->gport             = -1;                       \
    node->base_index        = -1;                       \
    node->mc_base_index     = -1;                       \
    node->numq              = 0;                        \
    node->hw_index          = -1;                       \
    node->level             = -1;                       \
    node->cosq_attached_to  = -1;                       \
    node->num_child         = 0;                        \
    node->first_child       = 4095;                     \
    node->linkphy_enabled   = 0;                        \
    node->subport_enabled   = 0;                        \
    node->wred_disabled     = 0;                        \
    node->burst_calculation_enable = 0;                 \
    node->cosq_map          = NULL;                     \
    node->parent            = NULL;                     \
    node->sibling           = NULL;                     \
    node->child             = NULL;                     \
    BCM_PBMP_CLEAR(node->fabric_pbmp);                  \
    node->remote_modid      = -1;                       \
    node->fabric_modid      = -1;                       \
    node->remote_port       = -1;
 
#define _BCM_KT2_LLS_DYN_CHG_REG_B 0x3FF
#define _BCM_SB2_LLS_DYN_CHG_REG_B 0x3F
#define _BCM_ML_LLS_DYN_CHG_REG_B  0xF
#define _BCM_KT2_LLS_DYN_CHG_REG_C 0xFFFFFFFF

#define _BCM_KT2_LB_BASE_QUEUE     0x40

typedef enum {
    _BCM_KT2_COSQ_FC_PAUSE = 0,
    _BCM_KT2_COSQ_FC_PFC,
    _BCM_KT2_COSQ_FC_VOQFC,
    _BCM_KT2_COSQ_FC_E2ECC,
    _BCM_KT2_COSQ_FC_E2ECC_COE,
    _BCM_KT2_COSQ_FC_SAFC
} _bcm_kt2_fc_type_t;

typedef enum {
    _BCM_KT2_COSQ_DMVOQ = 0,
    _BCM_KT2_COSQ_DPVOQ,
    _BCM_KT2_COSQ_E2ECC_COE
} _bcm_kt2_voq_type_t;


typedef enum {
    _BCM_KT2_COSQ_STATE_NO_CHANGE,
    _BCM_KT2_COSQ_STATE_DISABLE,
    _BCM_KT2_COSQ_STATE_ENABLE,
    _BCM_KT2_COSQ_STATE_SPRI_TO_WRR,
    _BCM_KT2_COSQ_STATE_WRR_TO_SPRI,
    _BCM_KT2_COSQ_STATE_MAX
}_bcm_kt2_cosq_state_t;

typedef struct _bcm_kt2_cosq_list_s {
    int count;
    SHR_BITDCL *bits;
}_bcm_kt2_cosq_list_t;

typedef struct _bcm_kt2_cosq_port_info {
    int q_offset;
    int q_limit;
    int mcq_offset;
    int mcq_limit;
    int uc_vlan_base;
}_bcm_kt2_cosq_port_info_t;

typedef struct _bcm_kt2_mmu_info_s {
    int num_base_queues;   /* Number of classical queues */
    int num_ext_queues;    /* Number of extended queues */
    int qset_size;         /* subscriber queue set size */
    int num_dmvoq_queues;  /* Number of dmvoq queues */
    int base_dmvoq_queue;  /* Base DMVOQ queue */
    int num_sub_queues;    /* Number of subscriber queues */
    int base_sub_queue;    /* Base subscriber queue */
    uint32 num_queues;     /* total number of queues */
    uint32 num_nodes;      /* max number of sched nodes */
    uint32 max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_MAX];/* max nodes in each level */
    bcm_gport_t *l1_gport_pair;        /* L1 gport mapping for dynamic update */
    _bcm_kt2_cosq_port_info_t *port;   /* port information */
    _bcm_kt2_cosq_list_t global_qlist;    /* list of extended queues */
    _bcm_kt2_cosq_list_t l2_global_qlist;    /* list of extended queues */
    _bcm_kt2_cosq_list_t dmvoq_qlist;    /* list of dmvoq queues */
    _bcm_kt2_cosq_list_t sched_list;   /* list of sched nodes */
    _bcm_kt2_cosq_list_t sub_qlist;    /* list of subscriber queues */
    _bcm_kt2_cosq_list_t sub_mcqlist;  /* list of multicast subscriber queues */
    _bcm_kt2_cosq_list_t sched_hw_list[_BCM_KT2_COSQ_NODE_LEVEL_MAX]; 
                                       /* list of sched hw list */
    _bcm_kt2_cosq_list_t l2_base_qlist;  
                           /* list of l2  base uc queue index list */
    _bcm_kt2_cosq_list_t l2_base_mcqlist;  
                           /* list of l2  base mc queue index list */ 
    _bcm_kt2_cosq_list_t l2_dmvoq_qlist;   
                           /* list of l2  dmvoq queue index list */
    _bcm_kt2_cosq_list_t l2_sub_qlist;
                           /* list of l2  subscriber queue index list */
    _bcm_kt2_cosq_list_t l2_sub_mcqlist;
                           /* list of l2 multicast subscriber queue index list */
    _bcm_kt2_cosq_node_t *sched_node;
                                         /* sched nodes */
    _bcm_kt2_cosq_node_t *queue_node;
                                         /* queue nodes */
    int     curr_merger_index[4];    
    uint32  intf_ref_cnt[BCM_MULTICAST_PORT_MAX]; /* Interface reference count
                                                     for queues */
}_bcm_kt2_mmu_info_t;


typedef struct  _pfc_port_support_s { 
    int                   supported;
    /* if PFC is supported for given port will be true*/
    int                   mmu_port; 
    int                   index1;/*index in to PFC_ST_TBL for given port*/
    int                   index2;/*index in to PFC_ST_TBL for given port*/
}_pfc_port_support_t;
_pfc_port_support_t kt2_pfc_support[KT2_MAX_LOGICAL_PORTS] = { 
   {0, -1, -1, -1},
   {1, 0, 0, 1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 1, 2, 3},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 2, 4, 5},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 3, 6, 7},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 4, 8, 9},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 5, 10, 11},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 6, 12, 13},
   {1, 8, 16, 17},
   {1, 10, 20, 21},
   {1, 12, 24, 25},
   {0, -1, -1, -1},
   {1, 13, 26, 27},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 11, 22, 23},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 7, 14, 15},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 9, 18, 19},
   {0, -1, -1, -1},
   {0, -1, -1, -1}
 };

#ifdef BCM_SABER2_SUPPORT
_pfc_port_support_t sb2_pfc_support[SB2_MAX_LOGICAL_PORTS] =  {
   {0, -1, -1, -1},
   {1, 0, 0, 1,},
   {1, 1, 2, 3},
   {1, 2, 4, 5},
   {1, 3, 6, 7},
   {1, 4, 8, 9},
   {1, 5, 10, 11},
   {1, 6, 12, 13},
   {1, 7, 14, 15},
   {1, 8, 16, 17},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 9, 18, 19},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 10, 20, 21},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {0, -1, -1, -1},
   {1, 11, 22, 23},
   {1, 12, 24, 25},
   {1, 13, 26, 27},
   {1, 14, 28, 29},
   {1, 15, 30, 31},
   {1, 16, 32, 33},
   {1, 17, 34, 35},
   {1, 18, 36, 37},
   {0, -1, -1, -1}
};
#endif 
#ifdef BCM_METROLITE_SUPPORT
_pfc_port_support_t ml_pfc_support[ML_MAX_LOGICAL_PORTS] =  {
   {0, -1, -1, -1},
   {1, 0, 0, 1,},
   {1, 1, 2, 3},
   {1, 2, 4, 5},
   {1, 3, 6, 7},
   {1, 4, 8, 9},
   {1, 5, 10, 11},
   {1, 6, 12, 13},
   {1, 7, 14, 15},
   {1, 8, 16, 17},
   {1, 9, 18, 19},
   {1, 10,20, 21},
   {1, 11,22, 23},
   {0, -1, -1, -1}
};
#endif

STATIC _bcm_kt2_mmu_info_t *_bcm_kt2_mmu_info[BCM_MAX_NUM_UNITS];  

STATIC soc_profile_mem_t *_bcm_kt2_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_kt2_egr_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_kt2_wred_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_kt2_ifp_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_kt2_llfc_profile[BCM_MAX_NUM_UNITS];

/* Number of COSQs configured for this device */
STATIC int _bcm_kt2_num_cosq[SOC_MAX_NUM_DEVICES];

STATIC int num_pp_ports[BCM_MAX_NUM_UNITS];
STATIC int num_port_schedulers[BCM_MAX_NUM_UNITS];

STATIC int num_s0_schedulers[BCM_MAX_NUM_UNITS];
STATIC int num_s1_schedulers[BCM_MAX_NUM_UNITS];
STATIC int num_l0_schedulers[BCM_MAX_NUM_UNITS];
STATIC int num_l1_schedulers[BCM_MAX_NUM_UNITS];
STATIC int num_total_schedulers[BCM_MAX_NUM_UNITS];

STATIC int num_l2_queues[BCM_MAX_NUM_UNITS];
STATIC int max_l1_shaper_bucket[BCM_MAX_NUM_UNITS];
STATIC int max_l2_shaper_bucket[BCM_MAX_NUM_UNITS];
STATIC soc_mem_t l2_shaper_memory[BCM_MAX_NUM_UNITS][4];
STATIC uint8 pg_profile_refcount[BCM_MAX_NUM_UNITS][4];
STATIC int soc_max_num_pp_ports[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_kt2_cosq_packing_mode_get(int unit, int index, int *set);

STATIC int
_bcm_kt2_subport_cosq_config_set(int unit, int numq);
int
_bcm_kt2_cosq_subport_get(int unit, bcm_gport_t sched_gport,
                          int *pp_port);
STATIC int
_bcm_kt2_cosq_cpu_fc_set(int unit); 

STATIC int
_bcm_kt2_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg);

STATIC int
_bcm_kt2_cosq_child_node_at_input(_bcm_kt2_cosq_node_t *node, int cosq,
                                  _bcm_kt2_cosq_node_t **child_node);

STATIC int 
_bcm_kt2_init_all_queue_counters(int unit);

extern sal_mutex_t cosq_sync_lock[SOC_MAX_NUM_DEVICES];

/* Function:
 *    _bcm_kt2_cosq_source_intf_set
 * Purpose:
 *    Function to set the SRC_INTF table and
 *    return the index
 * Parameters:
 *     unit       - (IN) unit number
 *     src_modid  - (IN) source module ID
 *     src_port_num - (IN) source port number
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt2_cosq_source_intf_set(int unit, bcm_port_t src_modid, 
        bcm_port_t src_port_num, _bcm_kt2_voq_type_t type)
{
    int index, free_index = -1;
    uint32 rval, modid = 0, port_num = 0, valid;

    for (index = 0; index < SOC_REG_NUMELS(unit, SRC_INTFr); index++) {
        modid=0;
        port_num=0;
        BCM_IF_ERROR_RETURN(READ_SRC_INTFr(unit, index, &rval));
        valid = soc_reg_field_get(unit, SRC_INTFr, rval, SRC_MODID_VALIDf);
        if (1 == valid) {
            modid = soc_reg_field_get(unit, SRC_INTFr, rval, SOURCEf);
            if (src_modid == modid) {
                return index;
            }
        } 
        valid = soc_reg_field_get(unit, SRC_INTFr, rval, SRC_PORT_NUM_VALIDf);
        if (1 == valid) {
            port_num = soc_reg_field_get(unit, SRC_INTFr, rval, SOURCEf);
            if (src_port_num == port_num) {
                return index;
            }
        }
        if ((port_num == 0) && (modid == 0) &&
                (free_index == -1)) {
            free_index = index;
        }
    }
    if (free_index == -1) {
        return BCM_E_RESOURCE;
    }

    rval = 0;
    if ((type == _BCM_KT2_COSQ_DMVOQ) || 
            (type == _BCM_KT2_COSQ_DPVOQ)) {
        soc_reg_field_set(unit, SRC_INTFr, &rval, SRC_MODID_VALIDf, 1);
        soc_reg_field_set(unit, SRC_INTFr, &rval, SOURCEf, src_modid);
        if (type != _BCM_KT2_COSQ_DPVOQ) {
            if (SOC_IS_SABER2(unit)) {
                if (SOC_IS_METROLITE(unit)) {
                    soc_reg_field_set(unit, SRC_INTFr, &rval, IF_IDf, 0);
                } else {
                    soc_reg_field_set(unit, SRC_INTFr, &rval, IF_IDf, 
                            free_index > 1 ? 0 :free_index);
                }
            } else {
                soc_reg_field_set(unit, SRC_INTFr, &rval, IF_IDf, 
                        free_index > 3 ? 0 :free_index);
            }
        }
    } else if (type == _BCM_KT2_COSQ_E2ECC_COE) {
        soc_reg_field_set(unit, SRC_INTFr, &rval, SRC_PORT_NUM_VALIDf, 1);
        soc_reg_field_set(unit, SRC_INTFr, &rval, SOURCEf, src_port_num);
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(WRITE_SRC_INTFr(unit, free_index, rval));

    return free_index;
}

/* Function:
 *    _bcm_kt2_cosq_source_intf_profile_get
 * Purpose:
 *    Function to get the SRC_INTF table and match with SOURCE field
 *    return the IF_ID 
 * Parameters:
 *     unit       - (IN) unit number
 *     src_modid  - (IN) source module ID 
 *     src_port_num - (IN) source port number
 *     type  - (IN) voq type
 * Returns:
 *     profile_index 
 */


STATIC int
_bcm_kt2_cosq_source_intf_profile_get(int unit, bcm_port_t src_modid, 
        bcm_port_t src_port_num, _bcm_kt2_voq_type_t type)
{
    int index = -1;
    int profile_id = -1; 
    uint32 rval, modid = 0, port_num = 0, valid;

    for (index = 0; index < 16; index++) {
        modid=0;
        port_num=0;
        BCM_IF_ERROR_RETURN(READ_SRC_INTFr(unit, index, &rval));
        valid = soc_reg_field_get(unit, SRC_INTFr, rval, SRC_MODID_VALIDf);
        if (1 == valid) {
            modid = soc_reg_field_get(unit, SRC_INTFr, rval, SOURCEf);
            if (src_modid == modid) {
                profile_id = soc_reg_field_get(unit, SRC_INTFr, rval, IF_IDf);
                break;
            }
        } 
        valid = soc_reg_field_get(unit, SRC_INTFr, rval, SRC_PORT_NUM_VALIDf);
        if (1 == valid) {
            port_num = soc_reg_field_get(unit, SRC_INTFr, rval, SOURCEf);
            if (src_port_num == port_num) {
            profile_id = soc_reg_field_get(unit, SRC_INTFr, rval, IF_IDf);
            break;
            }
        }
    }
    return profile_id; 
}

/*
 * Function:
 *  _bcm_kt2_cosq_map_fc_status_to_node    
 * Purpose:
 *     Function will fill the appropriate indexes and SEL
 *     in MMU_INTFI_FC_MAP_TBL as per the table requirement
 * Parameters:
 *     unit      - (IN) unit number
 *     fct       - (IN) type is PFC/VOQ 
 *     hw_index  - (IN) hw_index of the queue 
 *     level     - (IN) Node level L2/L1/L0 
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt2_cosq_map_fc_status_to_node(int unit, _bcm_kt2_fc_type_t fct,
                              uint32 hw_index, int level, uint32 base_offset )
{
    int map_entry_index, eindex, pfc;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    static const soc_field_t self[] = {
        SEL0f, SEL1f
    };
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f
    };
    static const soc_mem_t mem[] = {
        INVALIDm,
        MMU_INTFI_FC_MAP_TBL0m,
        MMU_INTFI_FC_MAP_TBL1m,
        MMU_INTFI_FC_MAP_TBL2m
    };
    if ((fct == _BCM_KT2_COSQ_FC_PFC) || 
                  (fct == _BCM_KT2_COSQ_FC_SAFC)) { 
        pfc = 1; 
    }else { 
        pfc = 0; 
    }

    map_entry_index = hw_index / 16;
    eindex = (hw_index % 16) / 8;
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem[level], MEM_BLOCK_ALL, map_entry_index, &map_entry));
    soc_mem_field32_set(unit, mem[level], &map_entry,
            indexf[eindex], base_offset);
    soc_mem_field32_set(unit, mem[level], &map_entry, self[eindex], pfc);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem[level],
                MEM_BLOCK_ALL, map_entry_index, &map_entry));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_localport_resolve
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
_bcm_kt2_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id/*, result*/;

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID_RANGE(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
        return BCM_E_NONE;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id));

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_modport_to_pp_port_get(unit, module, port, &port));

    *local_port = port;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_node_get
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
_bcm_kt2_cosq_node_get(int unit, bcm_gport_t gport, bcm_module_t *modid,
                      bcm_port_t *port, int *id, _bcm_kt2_cosq_node_t **node)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node_base;
    bcm_module_t modid_out = 0;
    bcm_port_t port_out = 0;
    uint32 encap_id = 0;
    int index;


    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);    
    } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        encap_id = BCM_GPORT_LOCAL_GET(gport);
        port_out = encap_id;
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        encap_id = BCM_GPORT_MODPORT_PORT_GET(gport);
        port_out = encap_id;
    } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        encap_id = BCM_GPORT_SUBPORT_PORT_GET(gport);
        port_out = encap_id;
    } else {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        encap_id = (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) ?
                   BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport) :
                   BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        node_base = mmu_info->queue_node;
    } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {  
        encap_id = BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(gport); 
        index = encap_id;
        if (mmu_info->num_dmvoq_queues > 0) { 
            index += mmu_info->base_dmvoq_queue;
        }
        node_base = mmu_info->queue_node;
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
               BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        encap_id = (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ?
                    BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport):
                    BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);

        index = encap_id;
        if (mmu_info->num_sub_queues > 0) {
            if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
                index += (mmu_info->base_sub_queue + mmu_info->num_queues/2);
            } else {
                index += mmu_info->base_sub_queue;
            }
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
        if (index >= num_total_schedulers[unit]) {
            return BCM_E_PORT;
        }
        node_base = mmu_info->sched_node;
    }

    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        /*
         * Get S1 node from extended sched list if pp_port is reused
         */
        if (BCM_GPORT_IS_SUBPORT_PORT(gport) &&
            _SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, index) &&
            (index <= num_port_schedulers[unit])) {
            index += SOC_INFO(unit).cpu_hg_pp_port_index;
        }
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

STATIC int
_bcm_kt2_voq_base_node_get(int unit, int modid, int fabric_modid,
                      _bcm_kt2_cosq_node_t **node)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node_base;
    int index;
    *node = NULL; 
    mmu_info = _bcm_kt2_mmu_info[unit];
    node_base = mmu_info->queue_node;
    mmu_info = _bcm_kt2_mmu_info[unit];
    for(index = 0 ; 
            index < mmu_info->num_queues ; index++) {
        if ((node_base[index].remote_modid == modid) &&
            (node_base[index].fabric_modid == fabric_modid)) { 
            *node = &node_base[index];
            break;
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_kt2_node_index_get
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
_bcm_kt2_node_index_get(int unit, SHR_BITDCL *list, int start, int end,
                       int qset, int range, int *id)
{
    int i, j, inc;
    _bcm_kt2_mmu_info_t *mmu_info;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt2_mmu_info[unit];
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

            if ((j == (i + range)) && (mmu_info->intf_ref_cnt[i] == 0)) {
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

        if ((j == (i + range)) && (mmu_info->intf_ref_cnt[i] == 0)) {
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
 *     _bcm_kt2_node_index_set
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
_bcm_kt2_node_index_set(_bcm_kt2_cosq_list_t *list, int start, int range)
{
    list->count += range;
    SHR_BITSET_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_node_index_clear
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
_bcm_kt2_node_index_clear(_bcm_kt2_cosq_list_t *list, int start, int range)
{
    list->count -= range;
    SHR_BITCLR_RANGE(list->bits, start, range);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_map_index_get
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
_bcm_kt2_cosq_map_index_get(SHR_BITDCL *list, int start, int end, int *id)
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
 *     _bcm_kt2_cosq_map_index_set
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
_bcm_kt2_cosq_map_index_set(SHR_BITDCL *list, int start, int range)
{
    SHR_BITSET_RANGE(list, start, range);
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_map_index_clear
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
_bcm_kt2_cosq_map_index_clear(SHR_BITDCL *list, int start, int range)
{
    if (list != NULL) {
        SHR_BITCLR_RANGE(list, start, range);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_map_index_is_set
 * Purpose:
 *     Check index is allocated
 * Parameters:
 *     list       - (IN) bit list
 *     index      - (IN) index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_cosq_map_index_is_set(SHR_BITDCL *list, int index)
{
    if (SHR_BITGET(list, index) != 0) {
        return TRUE;
    }

    return FALSE;
}

STATIC int
_bcm_kt2_cosq_max_nodes_get(int unit, _bcm_kt2_cosq_node_level_t level,
                           int *id)
{
    _bcm_kt2_mmu_info_t *mmu_info;

    if (level < _BCM_KT2_COSQ_NODE_LEVEL_ROOT ||
        level >= _BCM_KT2_COSQ_NODE_LEVEL_MAX) {
        return BCM_E_PARAM;
    }
    mmu_info = _bcm_kt2_mmu_info[unit];
    *id = mmu_info->max_nodes[level];

    return BCM_E_NONE;
}

typedef struct _bcm_kt2_lls_info_s {
    int node_count[_BCM_KT2_COSQ_NODE_LEVEL_MAX];
    uint32 *min_entries[_BCM_KT2_COSQ_NODE_LEVEL_MAX];
    uint32 *max_entries[_BCM_KT2_COSQ_NODE_LEVEL_MAX];
    uint32 *max_burst_entries[_BCM_KT2_COSQ_NODE_LEVEL_MAX];
    uint32 *min_burst_entries[_BCM_KT2_COSQ_NODE_LEVEL_MAX];
} _bcm_kt2_lls_info_t;

/* Max 1G rate to expedite draining packets during congestion */
#define _BCM_KT2_COSQ_MAX_FLUSH_RATE   1000000 

STATIC int 
_bcm_kt2_cosq_lls_shapers_remove(int unit, int port, 
                            _bcm_kt2_cosq_node_t *node,
                            _bcm_kt2_lls_info_t *lls_tree)
{
    int alloc_size, speed;
    _bcm_kt2_cosq_node_t *cur_node, *parent;
    uint32 *min_entries, *max_entries, *min_burst_entries, *max_burst_entries;
    uint32 max_shaper, min, max, flags, burst_max, burst_min;
    uint32 burst, mode;
    int count = 0;
    int num_entries = 0;

    parent = node->parent;
    if ((parent == NULL) && 
        (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT)) {
        return BCM_E_INTERNAL;
    }

    num_entries = (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT) ?
                  parent->num_child : 1;

    /* Allocate memory based on child nodes */
    alloc_size = sizeof(uint32) * num_entries;
    lls_tree->min_entries[node->level] = sal_alloc(alloc_size, "lls_war_buf");
    lls_tree->max_entries[node->level] = sal_alloc(alloc_size, "lls_war_buf");
    lls_tree->max_burst_entries[node->level] = sal_alloc(alloc_size, "lls_war_buf");
    lls_tree->min_burst_entries[node->level] = sal_alloc(alloc_size, "lls_war_buf");

    min_entries = lls_tree->min_entries[node->level];
    max_entries = lls_tree->max_entries[node->level];
    min_burst_entries = lls_tree->min_burst_entries[node->level];
    max_burst_entries = lls_tree->max_burst_entries[node->level];

    sal_memset(min_entries, 0, alloc_size);
    sal_memset(max_entries, 0, alloc_size);
    sal_memset(min_burst_entries, 0, alloc_size);
    sal_memset(max_burst_entries, 0, alloc_size);
    lls_tree->node_count[node->level] = num_entries; 

    /*
     *  Reserve 10% for the node which is being flushes and 
     *  distribute remaining bandwidth across other nodes
     */
    speed = _BCM_KT2_COSQ_MAX_FLUSH_RATE;
    max_shaper = ((speed * 9)/10)/lls_tree->node_count[node->level];

    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
        BCM_IF_ERROR_RETURN
            (bcm_kt_port_rate_egress_get(unit, port, &max, &burst, &mode));
        min_entries[count] = 0;
        max_entries[count] = max;
        BCM_IF_ERROR_RETURN
            (bcm_kt_port_rate_egress_set(unit, port, speed, burst, mode));
        return BCM_E_NONE;
    }

    /* store configured bandwidth of all the nodes */
    for (cur_node = parent->child; cur_node != NULL;
         cur_node = cur_node->sibling) {
         BCM_IF_ERROR_RETURN
             (bcm_kt2_cosq_gport_bandwidth_get(unit, cur_node->gport, 0, 
                                  &min, &max, &burst_min, &burst_max, &flags));
         min_entries[count] = min;
         max_entries[count] = max;
         min_burst_entries[count] = burst_min;
         max_burst_entries[count] = burst_max;
         count++;
         if (cur_node->hw_index == node->hw_index) {
             BCM_IF_ERROR_RETURN
                 (bcm_kt2_cosq_gport_bandwidth_set(unit, cur_node->gport, 0, 
                                          (speed / 10), speed, flags));
         } else {
             BCM_IF_ERROR_RETURN
                 (bcm_kt2_cosq_gport_bandwidth_set(unit, cur_node->gport, 0, 
                                              0, max_shaper, flags));
         }
    }

    return BCM_E_NONE;
}

STATIC int 
_bcm_kt2_cosq_lls_shapers_restore(int unit, int port,
                            _bcm_kt2_cosq_node_t *node,
                            _bcm_kt2_lls_info_t *lls_tree)
{
    _bcm_kt2_cosq_node_t *cur_node, *parent;
    uint32 *min_entries, *max_entries, *burst_max_entries, *burst_min_entries;
    uint32 min, max, burst_max, burst_min;
    uint32 burst, mode;
    int count = 0;

    parent = node->parent;
    if ((parent == NULL) && 
        (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT)) {
        return BCM_E_INTERNAL;
    }

    min_entries = lls_tree->min_entries[node->level];
    max_entries = lls_tree->max_entries[node->level];
    burst_max_entries = lls_tree->max_burst_entries[node->level];
    burst_min_entries = lls_tree->min_burst_entries[node->level];

    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
        BCM_IF_ERROR_RETURN
            (bcm_kt_port_rate_egress_get(unit, port, &max, &burst, &mode));
        max = max_entries[count];    
        BCM_IF_ERROR_RETURN
            (bcm_kt_port_rate_egress_set(unit, port, max, burst, mode));
        return BCM_E_NONE;
    }

    /* restore bandwidth */
    for (cur_node = parent->child; cur_node != NULL;
         cur_node = cur_node->sibling) {
        min = min_entries[count];
        max = max_entries[count];
        burst_max = burst_max_entries[count];
        burst_min = burst_min_entries[count];
        count++;

        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_bandwidth_set(unit, cur_node->gport, 0, 
                                         min, max, 0));
#if defined(BCM_SABER2_SUPPORT)
          if (SOC_IS_SABER2(unit) && (soc_property_get(unit,
                          spn_COSQ_CONTROL_BURST_NODE_SELECT, 0)) &&
                      (BCM_GPORT_IS_SET(cur_node->gport)) &&
                              (BCM_GPORT_IS_SCHEDULER(cur_node->gport))) {
            if (cur_node->child != NULL) {
              BCM_IF_ERROR_RETURN(bcm_kt2_cosq_gport_bandwidth_burst_set(unit,
                          cur_node->gport, cur_node->child->cosq_attached_to, burst_min, burst_max));
            }
          } else
#endif
          {
            BCM_IF_ERROR_RETURN(bcm_kt2_cosq_gport_bandwidth_burst_set(unit,
                        cur_node->gport, 0, burst_min, burst_max));
          }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_poll_flush_active(int unit, bcm_port_t port, 
                                _bcm_kt2_cosq_node_t *node)
{
    soc_timeout_t timeout;
    int flush_active = 1;
    uint32 timeout_val;
    uint32 rval = 0;
    int packing_mode = 0;

    if (soc_feature(unit, soc_feature_mmu_packing)) {
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_packing_mode_get(unit, node->hw_index, &packing_mode));
        
    }
    if (packing_mode == 1) {
        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_PACKING_DEFAULT);
    } else {

        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                    _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT);
    }
    soc_timeout_init(&timeout, timeout_val, 0);

    while (flush_active) {                              
        BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval)); 
        flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, 
                                         rval, FLUSH_ACTIVEf);
        if (soc_timeout_check(&timeout)) {
            return (BCM_E_TIMEOUT);
         }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_adjust_lls_bw(int unit, bcm_gport_t gport, 
                            _bcm_kt2_cosq_node_t *node)
{
    int rv = BCM_E_NONE;
    _bcm_kt2_lls_info_t lls_tree;
    _bcm_kt2_cosq_node_t *cur_node;
    bcm_port_t port;
    int count = 0, i;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    int index = 0 , eindex = 0;
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f
    };
    int map_offset = 0;

    /* resolve the port */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, &port, NULL,
                                   NULL));
        if (node->hw_index > 0) {
            index = node->hw_index / 16 ;
            eindex = (node->hw_index % 16) / 8;
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, MMU_INTFI_FC_MAP_TBL2m,
                              MEM_BLOCK_ALL, index, &map_entry));
            map_offset = soc_mem_field32_get(unit, MMU_INTFI_FC_MAP_TBL2m, &map_entry,
                    indexf[eindex]);
            if (map_offset) {
                soc_mem_field32_set(unit, MMU_INTFI_FC_MAP_TBL2m, &map_entry,
                        indexf[eindex], 0);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_FC_MAP_TBL2m,
                            MEM_BLOCK_ALL, index, &map_entry));

            }
        }
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, gport, &port));
        if (port < 0) {
            return BCM_E_PORT;
        }
    }

    sal_memset(&lls_tree, 0, sizeof(_bcm_kt2_lls_info_t));
    cur_node =  node;
    while (cur_node != NULL) {
        count++;
        rv = _bcm_kt2_cosq_lls_shapers_remove(unit, port, cur_node, &lls_tree); 
        if (rv != BCM_E_NONE) {
            goto cleanup;
        }

        rv = _bcm_kt2_cosq_poll_flush_active(unit, port, node);
        if (rv == BCM_E_NONE) {
            break;
        } else {
            cur_node = cur_node->parent;
        }
    }

cleanup:
    cur_node = node; 
    for (i = 0; i < count; i++) {
        rv = _bcm_kt2_cosq_lls_shapers_restore(unit, port, cur_node, &lls_tree);
        if (rv != BCM_E_NONE) {
            return rv;
        }
        cur_node = cur_node->parent;
    } 
    if (map_offset) {
        soc_mem_field32_set(unit, MMU_INTFI_FC_MAP_TBL2m, &map_entry,
                indexf[eindex], map_offset);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_FC_MAP_TBL2m,
                    MEM_BLOCK_ALL, index, &map_entry));

    }
    for (i = _BCM_KT2_COSQ_NODE_LEVEL_ROOT; i < _BCM_KT2_COSQ_NODE_LEVEL_MAX;
         i++) {
        if (lls_tree.min_entries[i]) {
            sal_free(lls_tree.min_entries[i]);
            lls_tree.min_entries[i] = NULL;
        }
        if (lls_tree.max_entries[i]) {
            sal_free(lls_tree.max_entries[i]);
            lls_tree.max_entries[i] = NULL;
        }
        if (lls_tree.max_burst_entries[i]) {
            sal_free(lls_tree.max_burst_entries[i]);
            lls_tree.max_burst_entries[i] = NULL;
        }
        if (lls_tree.min_burst_entries[i]) {
            sal_free(lls_tree.min_burst_entries[i]);
            lls_tree.min_burst_entries[i] = NULL;
        }
    }

    return rv;
}


/*
 * Function:
 *     _bcm_kt2_cosq_alloc_clear
 * Purpose:
 *     Allocate cosq memory and clear
 * Parameters:
 *     size       - (IN) size of memory required
 *     description- (IN) description about the structure
 * Returns:
 *     BCM_E_XXX
 */
STATIC void *
_bcm_kt2_cosq_alloc_clear(unsigned int size, char *description)
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
 *     _bcm_kt2_cosq_free_memory
 * Purpose:
 *     Free memory allocated for mmu info members
 * Parameters:
 *     mmu_info_p       - (IN) MMU info pointer
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_kt2_cosq_free_memory(_bcm_kt2_mmu_info_t *mmu_info_p)
{
    int i;

    if (mmu_info_p->port != NULL) {
        sal_free(mmu_info_p->port);
        mmu_info_p->port = NULL;
    }
    if (mmu_info_p->l1_gport_pair != NULL) {
        sal_free(mmu_info_p->l1_gport_pair);
        mmu_info_p->l1_gport_pair = NULL;
    }
    if (mmu_info_p->global_qlist.bits != NULL) {
        sal_free(mmu_info_p->global_qlist.bits);
        mmu_info_p->global_qlist.bits = NULL;
    }
    if (mmu_info_p->dmvoq_qlist.bits != NULL) {
        sal_free(mmu_info_p->dmvoq_qlist.bits);
        mmu_info_p->dmvoq_qlist.bits = NULL;
    }
    if (mmu_info_p->sched_list.bits != NULL) {
        sal_free(mmu_info_p->sched_list.bits);
        mmu_info_p->sched_list.bits = NULL;
    }
    if (mmu_info_p->sub_qlist.bits != NULL) {
        sal_free(mmu_info_p->sub_qlist.bits);
        mmu_info_p->sub_qlist.bits = NULL;
    }
    if (mmu_info_p->sub_mcqlist.bits != NULL) {
        sal_free(mmu_info_p->sub_mcqlist.bits);
        mmu_info_p->sub_mcqlist.bits = NULL;
    }
    if (mmu_info_p->l2_base_qlist.bits != NULL) {
        sal_free(mmu_info_p->l2_base_qlist.bits);
        mmu_info_p->l2_base_qlist.bits = NULL;
    }
    if (mmu_info_p->l2_base_mcqlist.bits != NULL) {
        sal_free(mmu_info_p->l2_base_mcqlist.bits);
        mmu_info_p->l2_base_mcqlist.bits = NULL;
    }    
    if (mmu_info_p->l2_global_qlist.bits != NULL) {
        sal_free(mmu_info_p->l2_global_qlist.bits);
        mmu_info_p->l2_global_qlist.bits = NULL;
    }
    if (mmu_info_p->l2_dmvoq_qlist.bits != NULL) {
        sal_free(mmu_info_p->l2_dmvoq_qlist.bits);
        mmu_info_p->l2_dmvoq_qlist.bits = NULL;
    }
    if (mmu_info_p->l2_sub_qlist.bits != NULL) {
        sal_free(mmu_info_p->l2_sub_qlist.bits);
        mmu_info_p->l2_sub_qlist.bits = NULL;
    }
    if (mmu_info_p->l2_sub_mcqlist.bits != NULL) {
        sal_free(mmu_info_p->l2_sub_mcqlist.bits);
        mmu_info_p->l2_sub_mcqlist.bits = NULL;
    }
    for (i = 0; i < _BCM_KT2_COSQ_NODE_LEVEL_L2; i++) { 
        if (mmu_info_p->sched_hw_list[i].bits != NULL) {
            sal_free(mmu_info_p->sched_hw_list[i].bits);
            mmu_info_p->sched_hw_list[i].bits = NULL;
        }
    }
    if (mmu_info_p->sched_node != NULL) {
        for (i = 0; i < mmu_info_p->num_nodes; i++) {
            if (((&mmu_info_p->sched_node[i]) != NULL) &&
                ((&mmu_info_p->sched_node[i])->cosq_map != NULL)) {
                sal_free((&mmu_info_p->sched_node[i])->cosq_map);
                (&mmu_info_p->sched_node[i])->cosq_map = NULL;
            }
        }
        sal_free(mmu_info_p->sched_node);
        mmu_info_p->sched_node = NULL;
    }
    if (mmu_info_p->queue_node != NULL) {
        for (i = 0; i < mmu_info_p->num_queues; i++) {
            if (((&mmu_info_p->queue_node[i]) != NULL) &&
                ((&mmu_info_p->queue_node[i])->cosq_map != NULL)) {
                sal_free((&mmu_info_p->queue_node[i])->cosq_map);
                (&mmu_info_p->queue_node[i])->cosq_map = NULL;
            }
        }
        sal_free(mmu_info_p->queue_node);
        mmu_info_p->queue_node = NULL;
    }
}
STATIC int
_bcm_kt2_port_is_coe_linkphy(_bcm_kt2_cosq_node_t *node)
{
   while(node != NULL) {
         if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ||
             (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1)) {
             return TRUE;
         }
         node = node->parent;
   }
   return FALSE;
}

/*
 * _bcm_is_port_linkphy_subport(int unit, int port, int min_port_index) {
 *
 * IN:
 * unit :
 * port : pp_port or physical port
 * min_port_index : Start of subport pp_port. This can be -1 in which case
 *                  min_port_index will be automatically fetched.
 * OUT:
 * TRUE - if port is a subport/linkphy pp_port
 * FALSE - if port is a physical port
 */

STATIC int
_bcm_is_port_linkphy_subport(int unit, bcm_gport_t gport, int min_port_index)
{
    soc_info_t *si;
    int port, encap_id, rv;
    _bcm_kt2_cosq_node_t *node = NULL;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        rv = _bcm_kt2_cosq_node_get(unit, gport, NULL, &port, &encap_id, &node);
    } else {
        rv = _bcm_kt2_cosq_localport_resolve(unit, gport, &port);
        encap_id = port;
    }

    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unable to retrieve port info.\n")));
    }

#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        if ((node && (encap_id > num_port_schedulers[unit]) && (encap_id <= num_pp_ports[unit])) ||
            _bcm_kt2_port_is_coe_linkphy(node) || 
            (!node && (BCM_GPORT_IS_SUBPORT_PORT(gport) ||
             _SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port)))) {
            return TRUE;
        }
    } else
#endif
    {
        if (min_port_index == -1) {
            si = &SOC_INFO(unit);
            min_port_index = si->pp_port_index_min;
        }
        if (port >= min_port_index) {
            return TRUE;
        }
    }
    return FALSE;
}

int
_bcm_kt2_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum)
{
    _bcm_kt2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, modid, port, NULL, &node));
    *trunk_id = -1;

    if (_bcm_is_port_linkphy_subport(unit, gport, num_port_schedulers[unit])) {
        BCM_IF_ERROR_RETURN(
        _bcm_kt2_pp_port_to_modport_get(unit,*port,
                            modid, port));
    }

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
 *     _bcm_kt2_cosq_gport_traverse
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
_bcm_kt2_cosq_gport_traverse(int unit, bcm_gport_t gport,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t port, port_out;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = BCM_E_NONE;
    bcm_module_t modid, modid_out;
    bcm_gport_t gport_out;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, &modid, &port, NULL, &node));

    if (node != NULL) {
       if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
           if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
               flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
           } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
               flags = BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP; 
           } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
               flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
           } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
               flags = BCM_COSQ_GPORT_SUBSCRIBER;
           } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
               flags = (BCM_COSQ_GPORT_SUBSCRIBER | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP);;
           }       
       }
       if (BCM_GPORT_IS_SCHEDULER(gport)) {
           if (node->type == _BCM_KT2_NODE_SCHEDULER_WFQ) {
               if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
                   flags = BCM_COSQ_GPORT_SCHEDULER_WFQ;
               } else {
                   flags = BCM_COSQ_GPORT_SCHEDULER_WFQ |
                           BCM_COSQ_GPORT_SCHEDULER;
               }
           }
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
        _bcm_kt2_cosq_gport_traverse(unit, node->sibling->gport, cb, user_data);
    }

    if (node->child != NULL) {
        _bcm_kt2_cosq_gport_traverse(unit, node->child->gport, cb, user_data);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_index_resolve
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
_bcm_kt2_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_kt2_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count)
{
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t   *cur_node = NULL;
    bcm_port_t resolved_port;
    int resolved_index;
    int id, startq, numq = 0;
    _bcm_kt2_mmu_info_t *mmu_info;

    if (cosq < -1) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        startq = 0;
    } else {
        startq = cosq;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) || 
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, port, NULL, &resolved_port, &id,
                                   &node));
        if ((node->cosq_attached_to < 0) || (node->hw_index == -1)) { 
            /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        if (BCM_GPORT_IS_SCHEDULER(port) &&
            (style != _BCM_KT2_COSQ_INDEX_STYLE_BUCKET)) {
            numq = node->numq;
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
        } else {
            numq = node->numq;
        }    
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = 0;
    }

    switch (style) {
    case _BCM_KT2_COSQ_INDEX_STYLE_BUCKET:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level > _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
                resolved_index = node->hw_index;
            } else {
                    return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt2_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit - 
                    mmu_info->port[resolved_port].q_offset;
            
            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;

    case _BCM_KT2_COSQ_INDEX_STYLE_WRED:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1 && 
                    node->level !=_BCM_KT2_COSQ_NODE_LEVEL_L0) {
                    return BCM_E_PARAM;
                }
                _bcm_kt2_cosq_child_node_at_input(node ,startq ,
                                                  &cur_node); 
                resolved_index = cur_node->hw_index;
            } else {
                    return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt2_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit - 
                   mmu_info->port[resolved_port].q_offset;

            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;

    case _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER:
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
                if ((cur_node->type == _BCM_KT2_NODE_VOQ_ID) ||
                     (cur_node->type == _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID)) {
                    numq = node->num_child;
                }
                resolved_index = cur_node->hw_index;
            } else { /* unicast queue group or multicast queue group */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt2_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit -
                   mmu_info->port[resolved_port].q_offset;

            if (cosq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = mmu_info->port[resolved_port].q_offset + startq;
        }
        break;
    case _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE:
        mmu_info = _bcm_kt2_mmu_info[unit];
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                    return BCM_E_PARAM;
                }
                resolved_index = node->base_index;
            } else {
                return BCM_E_PARAM;
            }
            
            if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) ||
                 (!(soc_feature(unit, soc_feature_mmu_packing)) &&  
                                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))) {
                resolved_index -= mmu_info->port[resolved_port].q_offset;
            }
        } else { /* node == NULL */
            mmu_info = _bcm_kt2_mmu_info[unit];
            numq = mmu_info->port[resolved_port].q_limit -
                   mmu_info->port[resolved_port].q_offset;

            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;        
        }
        break;

    case _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION:
        mmu_info = _bcm_kt2_mmu_info[unit];
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
                resolved_index = node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
            
        } else { /* node == NULL */
            mmu_info = _bcm_kt2_mmu_info[unit];
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
        if (resolved_index < 0) {
            return BCM_E_INTERNAL;
        }
        *index = resolved_index;
    }
    if (count != NULL) {
        *count = (cosq == -1) ? numq : 1;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_kt2_cosq_get_dmvoq_index
 * Purpose:
 *  This logic is to make sure that the base index is always
 *  multiple of 8
 *     unit       - (IN) unit number
 *     mmu_info       - (IN) MMU info structure 
 *     list       - (IN)list from which index need to be selected 
 *     numq       - (IN) number of queues
 *     id       - (IN) START ID 
 *     limit       - (IN) limit
 * Returns:
 *     BCM_E_XXX
 */


STATIC int
_bcm_kt2_cosq_get_dmvoq_index(
        int unit,
        _bcm_kt2_mmu_info_t *mmu_info,
        _bcm_kt2_cosq_list_t *list,
        int numq, int *id , int limit ) 
{
     while (((mmu_info->base_dmvoq_queue + *id) % 8) != 0) {
              *id = (*id + 8 - ((mmu_info->base_dmvoq_queue + *id) % 8)); 
               BCM_IF_ERROR_RETURN
                  (_bcm_kt2_node_index_get(unit, list->bits, *id, limit,
                                           mmu_info->qset_size, numq, id));
           }
     return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_kt2_cosq_node_resolve
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
_bcm_kt2_cosq_node_resolve(int unit, _bcm_kt2_cosq_node_t *node,
                          bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *parent;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_list_t *list;
    bcm_port_t local_port;
    int start_idx = 0, end_idx = 0;
    int base_idx;
    int alloc_size;
    int numq, id = 0, rv = BCM_E_NONE;
    int offset, limit;
    _bcm_kt2_subport_info_t info;
    bcm_gport_t pp_port;
    int dev_int_stream_id = 0;
    soc_pbmp_t pbmp_linkphy_one_stream_per_subport;

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
        parent->cosq_map = _bcm_kt2_cosq_alloc_clear(alloc_size,
                                           "parent->cosq_map");
        if (parent->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }
 
    if (cosq < 0) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_map_index_get(parent->cosq_map,
                                        0, parent->numq, &id));
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_map_index_set(parent->cosq_map, id, 1));
        node->cosq_attached_to = id;
    } else {
        if (_bcm_kt2_cosq_map_index_is_set(parent->cosq_map,
                                  cosq) == TRUE) {
            return BCM_E_EXISTS;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_map_index_set(parent->cosq_map, cosq, 1));
        node->cosq_attached_to = cosq;
    } 

    mmu_info = _bcm_kt2_mmu_info[unit];
    numq = (parent->wrr_mode == 0) ? ((parent->numq > 8) ? 8 : parent->numq) : 1;

    switch (parent->level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
        case _BCM_KT2_COSQ_NODE_LEVEL_S1:    
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            list = &mmu_info->sched_hw_list[node->level];
            if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_subport_get(unit, node->gport, &pp_port));
                BCM_IF_ERROR_RETURN
                   (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                &info));
                 node->hw_index = (BCM_GPORT_SCHEDULER_GET(node->gport) >> 8);
                 node->hw_index &= 0x7FF;
                 if (info.port_type == _BCM_SUBPORT_COE_TYPE_LINKPHY) {
                   /* linkphy
                    * s0 index = stream_id/num_streams
                    */
                    dev_int_stream_id = info.dev_int_stream_id[0];

                    SOC_PBMP_CLEAR(pbmp_linkphy_one_stream_per_subport);
                    pbmp_linkphy_one_stream_per_subport =
                    soc_property_get_pbmp(unit,
                    spn_PBMP_LINKPHY_ONE_STREAM_PER_SUBPORT, 0);
                    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_node_get(
                                        unit, parent->gport, NULL, &local_port,
                                        NULL, NULL));
                    if (SOC_IS_METROLITE(unit) || (SOC_IS_SABER2(unit) &&
                        SOC_PBMP_MEMBER(pbmp_linkphy_one_stream_per_subport,
                        local_port))) {
                        node->hw_index = dev_int_stream_id;
                    } else if (SOC_IS_SABER2(unit) && (info.num_streams == 1)) {

                        /* When num_streams is 1, only 4 subports are supported in SB2 */
                        if ((dev_int_stream_id % 8) > 3) {
                            return BCM_E_PARAM;
                        }

                        /* The following is the mapping in S1 when num_streams = 1.
                         *   S1          S0
                         *   0 - 3       0 - 3
                         *   8 - 11      4 - 7
                         *   16 - 19     8 - 11
                         *   24 - 27     12 - 15
                         *
                         * The formula used is (4 * (S1 / 8) + (S1 % 8)).
                         */
                        node->hw_index = (4 * (dev_int_stream_id / 8)) +
                                            (dev_int_stream_id %4);

                    } else {
                        node->hw_index = (dev_int_stream_id / info.num_streams);
                    }
                    /* S0 is always WRR */
                    node->wrr_in_use = 1;

                } else
#if defined BCM_METROLITE_SUPPORT
                if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                    if (info.port_type == _BCM_SUBPORT_COE_TYPE_SUBTAG) {
                        BCM_IF_ERROR_RETURN
                            (bcm_kt2_subport_pp_port_s1_index_get(unit, pp_port, &node->hw_index));
                        /* S0 is always WRR */
                        node->wrr_in_use = 1;
                    }
                } else 
#endif
                {
                    node->hw_index -= num_port_schedulers[unit];
                }
                if (parent->base_index < 0) {
                    parent->base_index = node->hw_index;
                }

                /*for single stream cases in LINKPhy we donot need to configure S0 as 
                   S1 gets scheduled */
                if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                    if (node->hw_index < 128){
                        _bcm_kt2_node_index_set(list, node->hw_index, 1);
                    }
                } else {
                    _bcm_kt2_node_index_set(list, node->hw_index, 1);
                }
                break;
            }
            if (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S1 &&
                node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0 &&
                parent->linkphy_enabled) {
                /* S1 == L0 incase of linkphy */
                parent->base_index = parent->hw_index;
                node->hw_index = parent->hw_index;
                break;
            }

            /* Indexes from 0 to num_s1_schedulers[unit] is used for S1 nodes,
               remaining num_s1_schedulers[unit]++ are used for L0 and S0 */
            start_idx = (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) ? 
                         num_s1_schedulers[unit] : 0;
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                start_idx = 0;
            }
            end_idx = mmu_info->max_nodes[node->level];
            if (node->hw_index > 0) {
            if (parent->base_index < 0) {
                    parent->base_index = node->hw_index;
                }
            } else {
                if ( (parent->base_index < 0) && (parent->wrr_mode == 0) ) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, start_idx, end_idx,
                                                 mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    _bcm_kt2_node_index_set(list, id, numq);
                    parent->base_index = id;
                } else if ((node->cosq_attached_to >= numq) || (parent->wrr_mode == 1)) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, start_idx, end_idx,
                                                 mmu_info->qset_size, 1, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    _bcm_kt2_node_index_set(list, id, 1);
                    if (parent->wrr_mode == 1) {
                        if (parent->base_index < 0 ) {
                            parent->base_index = id;
                        }
                    }
                }   
            }    
            break;
            
        case _BCM_KT2_COSQ_NODE_LEVEL_S0:
            list = &mmu_info->sched_hw_list[node->level];
            if (parent->linkphy_enabled &&
                        !soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_subport_get(unit, parent->gport, &pp_port));
                BCM_IF_ERROR_RETURN
                   (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                &info));
                if (node->cosq_attached_to > 1) {
                    return BCM_E_PARAM;
                }
                if ((info.num_streams == 1) &&
                    (node->cosq_attached_to > 0)) {
                    _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                            node->cosq_attached_to, 1);
                    return BCM_E_PARAM;
                }
                if (SOC_IS_SABER2(unit) && (info.num_streams == 1)) {
                    node->hw_index = info.dev_int_stream_id[0];
                } else {
                    node->hw_index = (parent->hw_index * info.num_streams) +
                                    node->cosq_attached_to;
                }
                if (parent->base_index < 0) {
                    parent->base_index = node->hw_index;
                }
                node->linkphy_enabled = 1;
                _bcm_kt2_node_index_set(list, node->hw_index, 1);
            } else {
                if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L0) {
                    return BCM_E_PARAM;
                }
                start_idx = num_s1_schedulers[unit];
                end_idx = mmu_info->max_nodes[node->level];
                if (node->hw_index > 0) {
                    if (parent->base_index < 0) {
                        parent->base_index = node->hw_index;
                    }
                } else {
                    if ((parent->base_index < 0) && (parent->wrr_mode == 0)) {
                        rv = _bcm_kt2_node_index_get(unit, list->bits, start_idx, end_idx,
                                                     mmu_info->qset_size, numq, &id);
                        if (BCM_FAILURE(rv)) {
                            _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                    node->cosq_attached_to, 1);
                            node->cosq_attached_to = -1;
                            return rv;
                        }
                        _bcm_kt2_node_index_set(list, id, numq);
                        parent->base_index = id;
                    } else if ((node->cosq_attached_to >= numq) || (parent->wrr_mode == 1)) {
                        rv = _bcm_kt2_node_index_get(unit, list->bits, start_idx, end_idx,
                                                     mmu_info->qset_size, 1, &id);
                        if (BCM_FAILURE(rv)) {
                            _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                    node->cosq_attached_to, 1);
                            node->cosq_attached_to = -1;
                            return rv;
                        }
                        _bcm_kt2_node_index_set(list, id, 1);
                        if (parent->wrr_mode == 1) {
                            if (parent->base_index < 0 ) {
                                parent->base_index = id;
                            }
                        }
                    }
                }
            }
            break;
        
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_qlist;
                local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET
                                                         (node->gport);
                offset = mmu_info->port[local_port].q_offset;
                limit =  mmu_info->port[local_port].q_limit;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_mcqlist;
                local_port = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET
                                                         (node->gport);
                if (soc_feature(unit, soc_feature_mmu_packing)) {
                    offset = mmu_info->port[local_port].mcq_offset;
                    limit =  mmu_info->port[local_port].mcq_limit;
                } 
                else { 
                         offset = mmu_info->port[local_port].q_offset;
                         limit =  mmu_info->port[local_port].q_limit;
                }     
            } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(node->gport)) {
                local_port = BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET
                                                         (node->gport);
                offset = 0;
                if (mmu_info->num_dmvoq_queues > 0) {
                    list = &mmu_info->l2_dmvoq_qlist;
                    limit = mmu_info->num_dmvoq_queues;
                } else {
                    list = &mmu_info->l2_global_qlist;
                    limit = mmu_info->num_queues;
                }
            } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport) ||
                       BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                local_port =
                   ((BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) ?
                     BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET
                      (node->gport):
                     BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET
                      (node->gport));
                offset = 0;
                if (mmu_info->num_sub_queues > 0) {
                    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                        list = &mmu_info->l2_sub_qlist;
                    } else {
                        list = &mmu_info->l2_sub_mcqlist;
                    }
                    limit = mmu_info->num_sub_queues;
                } else {
                    list = &mmu_info->l2_global_qlist;
                    limit = mmu_info->num_queues;
                }
            } else {
                list = &mmu_info->l2_global_qlist;
                offset = 0;
                limit = mmu_info->num_queues;
            }
            if (node->hw_index < 0) {    
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) && 
                    parent->mc_base_index < 0 && 
                    node->cosq_attached_to < 8) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                            mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    _bcm_kt2_node_index_set(list, id, numq);
                    parent->mc_base_index = id;
                    node->mc_base_index = parent->mc_base_index;  
                } else if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) &&
                       parent->base_index < 0 && 
                       node->cosq_attached_to < 8) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                            mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    _bcm_kt2_node_index_set(list, id, numq);
                    parent->base_index = id ;
                    node->base_index = parent->base_index;  
                } else if ((BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                           (((parent->base_index < 0) &&
                           (node->cosq_attached_to < 8)) || parent->wrr_mode == 1)) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                                 mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }

                    _bcm_kt2_node_index_set(list, id, numq);
                    if (mmu_info->num_sub_queues > 0) {
                        parent->base_index = id + mmu_info->base_sub_queue ;
                    } else {
                        parent->base_index = id  ;
                    }
                    node->base_index = parent->base_index;
                } else if (
                     (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                     (((parent->mc_base_index < 0) &&
                     (node->cosq_attached_to < 8)) || parent->wrr_mode == 1)) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                                 mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }

                    _bcm_kt2_node_index_set(list, id, numq);
                    if (mmu_info->num_sub_queues > 0) {
                        parent->mc_base_index = id + mmu_info->base_sub_queue ;
                    } else {
                        parent->mc_base_index = id; 
                    }
                    node->mc_base_index = parent->mc_base_index;
                } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(node->gport) && 
                       (( parent->base_index < 0 && 
                        node->cosq_attached_to < 8) || parent->wrr_mode == 1)) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                                 mmu_info->qset_size, numq, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    if (((mmu_info->base_dmvoq_queue + id) % 8) != 0) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_get_dmvoq_index(unit, mmu_info, list,
                                                           numq, &id, limit)); 
                    }
                    _bcm_kt2_node_index_set(list, id, numq);
                    if  (mmu_info->num_dmvoq_queues > 0) { 
                        parent->base_index = id + mmu_info->base_dmvoq_queue ;
                    } else {
                        parent->base_index = id ;
                    }
                    node->base_index = parent->base_index;  
                } else if (node->cosq_attached_to >= 8) {
                    rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                           mmu_info->qset_size, 1, &id);
                    if (BCM_FAILURE(rv)) {
                        _bcm_kt2_cosq_map_index_clear(parent->cosq_map,
                                                node->cosq_attached_to, 1);
                        node->cosq_attached_to = -1;
                        return rv;
                    }
                    if (node->type == _BCM_KT2_NODE_VOQ) {
                        return BCM_E_PARAM; 
                    }
                    _bcm_kt2_node_index_set(list, id, 1);                
                }
                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    if (mmu_info->num_sub_queues > 0 ) {
                        id += mmu_info->base_sub_queue;
                    } 
                }
            } else {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    /*input 0 should be atttached first to get the correct base*/
                    if (node->cosq_attached_to < 8) {
                        if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport) ||
                             BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(node->gport) ||
                             BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                             (parent->base_index < 0)) {
                            parent->base_index = node->hw_index;
                        }    
                        if ((BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)  ||
                             BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) &&
                             (parent->mc_base_index < 0)) {
                            parent->mc_base_index = node->hw_index;
                        }
                    }
                    
                } 
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    if (parent->numq <= 8) {
        if (parent->level <= _BCM_KT2_COSQ_NODE_LEVEL_L1) {
            if (node->hw_index < 0) {
                base_idx = 
                      (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                       BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) ?
                            parent->mc_base_index : parent->base_index;
                node->hw_index = (parent->wrr_mode == 1) ? id : base_idx + node->cosq_attached_to;
            }    
        }

        parent->num_child++;
        /* get the lowest cosq as a first child */
        if (node->hw_index < parent->first_child) {
            parent->first_child = node->hw_index;
        }
    } else {
        if (parent->level <= _BCM_KT2_COSQ_NODE_LEVEL_L1) {
            if (node->hw_index < 0) {
                base_idx = (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) ?
                            parent->mc_base_index : parent->base_index;
                node->hw_index = (node->cosq_attached_to < numq) ? 
                    (base_idx + node->cosq_attached_to) : id;
            }
            if (node->cosq_attached_to >= numq) {
                node->wrr_in_use = 1;
            }    
        }
        parent->num_child++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_node_unresolve
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
_bcm_kt2_cosq_node_unresolve(int unit, _bcm_kt2_cosq_node_t *node, 
                             bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *cur_node, *parent;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_list_t *list;
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
            min_index = (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) ? 4095 :
                        ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) ? 1023 :
                          255);
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
            (_bcm_kt2_cosq_map_index_clear(parent->cosq_map,
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
            (_bcm_kt2_cosq_map_index_clear(parent->cosq_map,
             cur_node->cosq_attached_to, 1));
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
    numq = (parent->wrr_mode == 0) ? ((parent->numq > 8) ? 8 : parent->numq) : 1;

    switch (parent->level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
        case _BCM_KT2_COSQ_NODE_LEVEL_S0:
        case _BCM_KT2_COSQ_NODE_LEVEL_S1:
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:    
            list = &mmu_info->sched_hw_list[node->level];
            parent->num_child--;
            if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_S0) && 
                (parent->num_child >= 0)) {
                if (node->cosq_attached_to >= numq) {
                    _bcm_kt2_node_index_clear(list, node->hw_index, 1);
                }
                if (parent->num_child == 0 &&
                    parent->base_index >= 0) {
                    /* release contiguous queue ids */
                    _bcm_kt2_node_index_clear(list, parent->base_index, numq);
                    parent->base_index = -1;
                }    
            } 
            if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                if ( node->hw_index < 128) { 
                    _bcm_kt2_node_index_clear(list, node->hw_index, 1);
                }
            }
            break;
            
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_qlist;
                offset = 0;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
                list = &mmu_info->l2_base_mcqlist;
                offset = 0;
            } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(node->gport) 
                    && (mmu_info->num_dmvoq_queues > 0)) {
                list = &mmu_info->l2_dmvoq_qlist;
                offset = mmu_info->base_dmvoq_queue;
            } else if ((BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport) ||
                        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport))
                    && (mmu_info->num_sub_queues > 0)) {
                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    list = &mmu_info->l2_sub_qlist;
                    offset = mmu_info->base_sub_queue;
                } else {
                    list = &mmu_info->l2_sub_mcqlist;
                    offset = mmu_info->base_sub_queue + mmu_info->num_queues/2;
                }
            } else {
                list = &mmu_info->l2_global_qlist;
                offset = 0 ; 
            }            

            parent->num_child--;
            id = node->hw_index - offset;
            if (parent->num_child >= 0) {
                if (node->cosq_attached_to >= 8) {
                    _bcm_kt2_node_index_clear(list, id, 1);
                }
                if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                    if (parent->num_child == 0 &&
                        parent->mc_base_index >= 0) {
                        /* release contiguous queue ids */
                        id = parent->mc_base_index - offset;
                        _bcm_kt2_node_index_clear(list, id, numq);
                        parent->mc_base_index = -1;
                    }
                } else {
                    if (parent->num_child == 0 &&
                        parent->base_index >= 0) {
                        /* release contiguous queue ids */
                        id = parent->base_index - offset;
                        _bcm_kt2_node_index_clear(list, id, numq);
                        parent->base_index = -1;
                    }
                }
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
            /* passthru */
        default:
            return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_valid_dynamic_request
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
_bcm_kt2_cosq_valid_dynamic_request(int unit, bcm_port_t port, int node_level, 
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
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            req_port = parent_id;
            break;
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            SOC_IF_ERROR_RETURN
                (READ_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, req_parent,
                                     &l0_parent));
            req_port = soc_mem_field32_get(unit, LLS_L0_PARENTm, &l0_parent, 
                                           C_PARENTf); 
            break;
        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
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
 *     _bcm_kt2_cosq_dynamic_sp_werr_change
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
_bcm_kt2_cosq_dynamic_sp_werr_change(int unit, bcm_port_t port, int node_level, 
                                    int node_id, int parent_id, uint32 *config,
                                    _bcm_kt2_cosq_state_t state)
{
    soc_timeout_t timeout;
    uint32 timeout_val, rval;
    int i, in_use;
    soc_reg_t dyn_reg_a, dyn_reg_b, dyn_reg_c;
    uint32 entry_b, entry_c;
    lls_l0_parent_entry_t l0_parent;
    int packing_mode = 0, abort;
    
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
    soc_reg_t dyn_change_c[] = 
        {
        LLS_SP_WERR_DYN_CHANGE_0Cr,
        LLS_SP_WERR_DYN_CHANGE_1Cr,
        LLS_SP_WERR_DYN_CHANGE_2Cr,
        LLS_SP_WERR_DYN_CHANGE_3Cr
        };


    /* New value for the node_ids parent config bits 41:32
     * to be programmed into dynamic change register
     * LLS_SP_WERR_DYN_CHANGE_XB
     */
    if (SOC_IS_METROLITE(unit)) {
        entry_b = (config[1] & _BCM_ML_LLS_DYN_CHG_REG_B);
    } else if (SOC_IS_SABER2(unit)) {
        entry_b = (config[1] & _BCM_SB2_LLS_DYN_CHG_REG_B);
    } else {
        entry_b = (config[1] & _BCM_KT2_LLS_DYN_CHG_REG_B);
    }

    /* New value for the node_ids parent config bits 31:0.
     * to be programmed into dynamic change register 
     * LLS_SP_WERR_DYN_CHANGE_XC
     */
    entry_c = (config[0] & _BCM_KT2_LLS_DYN_CHG_REG_C);

    /* Get one of the free register to place the request */    
    for (i = 0; i < 4; i++) {
        dyn_reg_a =  dyn_change_a[i];
        dyn_reg_b =  dyn_change_b[i];
        dyn_reg_c =  dyn_change_c[i];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, dyn_reg_a, REG_PORT_ANY, 
                                          0, &rval));
        if (SAL_BOOT_SIMULATION) {
            break;
        }    
        
        if (soc_reg_field_get(unit, dyn_reg_a, rval, IN_USEf) == 0) {
            break;
        } else {
        /* verify any other req pending from this port or same parent */
            if (_bcm_kt2_cosq_valid_dynamic_request(unit, port, node_level, 
                     node_id, parent_id, rval) == FALSE) {
                return BCM_E_UNAVAIL;     
            }                                                     
        }
    }    

    if (i == 4) {
        /* None of the registers are available */
        return BCM_E_UNAVAIL;
    }    

    rval = 0 ;
    /* subtract 2 due to addional level of s0 and s1 */
    soc_reg_field_set(unit, dyn_reg_a, &rval, NODE_LEVELf, (node_level - 2));
    soc_reg_field_set(unit, dyn_reg_a, &rval, NODE_IDf, node_id);
    soc_reg_field_set(unit, dyn_reg_a, &rval, IN_USEf, 1);
    if (node_level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
        SOC_IF_ERROR_RETURN
            (READ_LLS_L0_PARENTm(unit, MEM_BLOCK_ALL, node_id,
                                 &l0_parent));
        if (soc_mem_field32_get(unit, LLS_L0_PARENTm,
                                &l0_parent, C_TYPEf) == 1) {
            if (SOC_IS_METROLITE(unit)) {
                /*
                 * For Ports, set parent_id[6:5] = 0x1,
                 * set parent_id[5:0] = {1b0, port_id[3:0]}.
                 */
                parent_id = ((0x1 << 5) | (parent_id & 0xF));
            } else if (SOC_IS_SABER2(unit)) {
                /* 
                   For Ports, set parent_id[7:6] = 0x1, 
                   set parent_id[5:0] = {1b0, port_id[4:0]}.
                 */
                parent_id = ((0x1 << 6) | (parent_id & 0x1F));
            } else if (SOC_IS_KATANA2(unit)) {
                /* 
                   For Ports, set parent_id[9:7] = 0x2, 
                   set parent_id[6:0] = {1b0, port_id[5:0]}.
                 */
                parent_id = ((0x2 << 7) | (parent_id & 0x3F));
            }
        }
    }
    soc_reg_field_set(unit, dyn_reg_a, &rval, PARENT_IDf, parent_id);

    /* Write DYN_CHANGE_XB register first */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_c, REG_PORT_ANY, 
                                      0, entry_c));
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_b, REG_PORT_ANY, 
                                      0, entry_b));
    /* Write DYN_CHANGE_XA register next. this triggers the request */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_a, REG_PORT_ANY, 
                                      0, rval));
    if (SAL_BOOT_SIMULATION) {
        return BCM_E_NONE;
    }
    
    in_use = 1;
    if (soc_feature(unit, soc_feature_mmu_packing)) {
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_packing_mode_get(unit, node_id, &packing_mode));
        
    }
    if (packing_mode == 1) {
        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_PACKING_DEFAULT);
    } else {

        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                    _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT);
    }
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
            abort = 0;
            if (packing_mode == 1) {
                timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                        _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_PACKING_DEFAULT);
            } else {

                timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                            _BCM_COSQ_QUEUE_FLUSH_TIMEOUT_DEFAULT);
            }
            soc_timeout_init(&timeout, timeout_val, 0);
            while (!abort) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, dyn_reg_a, REG_PORT_ANY, 0, &rval)); 
                /* ABORT_BEFORE_COMPLETE would be set by hw if operation is aborted */
                abort = soc_reg_field_get(unit, dyn_reg_a, rval, ABORT_BEFORE_COMPLETEf);
                if (abort) {
                    soc_reg_field_set(unit, dyn_reg_a, &rval, ABORT_BEFORE_COMPLETEf, 0);
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, dyn_reg_a, REG_PORT_ANY, 
                                        0, rval));
                    break;
                }
                if (soc_timeout_check(&timeout)) { 
                    return BCM_E_TIMEOUT;
                }
            }
            return BCM_E_TIMEOUT;
         }
    }

    return BCM_E_NONE;
}

int
_bcm_kt2_cosq_subport_get(int unit, bcm_gport_t sched_gport,
                          int *pp_port)
{
    int rv = BCM_E_NOT_FOUND;
    bcm_cos_queue_t cosq;
    bcm_gport_t gport;
    _bcm_kt2_cosq_node_t *node;
    int encap_id = 0;
    bcm_gport_t input_gport;

    gport = sched_gport; 
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL,
                               &node));

    while (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
        BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_attach_get(unit, gport, &input_gport, &cosq));
        gport = input_gport;
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            encap_id = (BCM_GPORT_SCHEDULER_GET(node->gport) >> 8) & 0x7ff;
            if (node->subport_enabled == 1) {
                *pp_port = encap_id;
                /*BCM_GPORT_SUBPORT_PORT_SET(*input_gport, encap_id);*/
                rv = BCM_E_NONE;
                break;
            }
        }
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL,
                               &node));
    }
#if defined BCM_METROLITE_SUPPORT
    if ((rv == BCM_E_NONE) && 
               soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        if (*pp_port > SOC_INFO(unit).cpu_hg_pp_port_index) {
            *pp_port -= SOC_INFO(unit).cpu_hg_pp_port_index;
        }
    }
#endif

    return rv;
}

int
_bcm_kt2_cosq_packing_mode_get(int unit, int index, int *set)
{
    mmu_toq_qpack_mode_entry_t qpack_entry;
    uint32 mode = 0;
    
    /* MMU_TOQ_QPACK_MODE register not present on Saber_2 */
    if (SOC_IS_SABER2(unit)) {
        return BCM_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (READ_MMU_TOQ_QPACK_MODEm(unit, MEM_BLOCK_ALL, 
                                  index/16, &qpack_entry));
    soc_mem_field_get(unit, MMU_TOQ_QPACK_MODEm,
                      (uint32 *)&qpack_entry,
                      QMODEf, &mode);

    *set = (mode & (1 << (index % 16))) ? 1 : 0;
    return BCM_E_NONE;
}

int
_bcm_kt2_cosq_packing_mode_set(int unit, int index, int set)
{
    int rv = BCM_E_NONE;
    mmu_toq_qpack_mode_entry_t qpack_entry;
    uint32 mode = 0;
    
    /* MMU_TOQ_QPACK_MODE register not present on Saber_2 */ 
    if (SOC_IS_SABER2(unit)) {
        return BCM_E_NONE;
    }
    SOC_IF_ERROR_RETURN
        (READ_MMU_TOQ_QPACK_MODEm(unit, MEM_BLOCK_ALL, 
                                  index/16, &qpack_entry));
    soc_mem_field_get(unit, MMU_TOQ_QPACK_MODEm,
                      (uint32 *)&qpack_entry,
                      QMODEf, &mode);
    if (set) {
        mode |= (1 << (index % 16));
    } else {
        mode &= ~(1 << (index % 16));
    }

    soc_mem_field32_set(unit, MMU_TOQ_QPACK_MODEm, 
                        &qpack_entry, QMODEf, mode);           
    SOC_IF_ERROR_RETURN
        (WRITE_MMU_TOQ_QPACK_MODEm(unit, MEM_BLOCK_ALL, 
                                   index/16, &qpack_entry));

    return rv;
}

STATIC 
int _bcm_kt2_cosq_flush_queues(int unit, 
                               bcm_port_t port,
                               _bcm_kt2_cosq_node_t *node, 
                               bcm_gport_t gport)
{
   int flush_active = 0;
   uint32 rval = 0; 
   uint32 timeout_val = 0;
   soc_timeout_t timeout;

   /* flush the queue  */
   if (!(SAL_BOOT_SIMULATION)) {
       BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval)); 
       flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, rval, FLUSH_ACTIVEf);
       if (flush_active == 1) {
           /* some other queue is in flush state, return failure */
           LOG_ERROR(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "ERROR: Queue %d is already in flush state \n"),
                      (soc_reg_field_get(unit, TOQ_FLUSH0r, rval, FLUSH_ID0f))));
           return BCM_E_BUSY;  
       }
       soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_TYPEf, 0);
       soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ID0f, node->hw_index);
       soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_NUMf, 1);
       soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ACTIVEf, 1);
       if (IS_EXT_MEM_PORT(unit, port)) {
           soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_EXTERNALf, 1);
       } else {
	       if (soc_reg_field_valid(unit, TOQ_FLUSH0r, FLUSH_EXTERNALf)){
	           soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_EXTERNALf, 0);
	       }
       }
       BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, rval)); 
       flush_active = 1;
       timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 
                                       20000);
       soc_timeout_init(&timeout, timeout_val, 0);

       while (flush_active) {                              
              BCM_IF_ERROR_RETURN (READ_TOQ_FLUSH0r(unit, &rval)); 
              flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, rval, 
                                               FLUSH_ACTIVEf);
              if (soc_timeout_check(&timeout)) {
                  BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_adjust_lls_bw(unit, gport, 
                                                                  node));
                  BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval)); 
                  flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, 
                                                   rval, FLUSH_ACTIVEf);
                  if (flush_active) {
                      LOG_ERROR(BSL_LS_BCM_COMMON,
                                (BSL_META_U(unit,
                                            "ERROR: Queue %d flush operation failed \n"), 
                                 node->hw_index));
                      return (BCM_E_TIMEOUT);
                  }
              }
       }
   }
   return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_l2_max_bucket_set(int unit, int port,
                                _bcm_kt2_cosq_node_t *node)
{
    int rv;

    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        for (node = node->child; node; node = node->sibling) {
            rv = _bcm_kt2_cosq_l2_max_bucket_set(unit, port, node);
            if (!BCM_SUCCESS(rv)) {
                return rv;
            }
        }
        return BCM_E_NONE;
    }

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        rv = soc_sb2_cosq_max_bucket_set(unit, port, node->hw_index,
                                    _BCM_KT2_COSQ_NODE_LEVEL_L2);
    } else 
#endif
    {
        rv = soc_kt2_cosq_max_bucket_set(unit, port, node->hw_index,
                                    _BCM_KT2_COSQ_NODE_LEVEL_L2);
    }
    return rv;
}

/*
 * Function:
 *        _bcm_kt2_cosq_min_clear(unit, physical_port, node);
 * Parameters:
 *      unit -  (IN) unit number
 *      port - (IN) physical port
 *      node - (IN) node undergoing switchover
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
 _bcm_kt2_cosq_min_clear(int unit, int port, 
                         _bcm_kt2_cosq_node_t *node) 
{
    int rv = BCM_E_NONE;
#if defined(BCM_SABER2_SUPPORT) 
    if (SOC_IS_SABER2(unit)) {
        rv = soc_sb2_cosq_min_clear(unit, port, node->hw_index,
                node->level);
    } else 
#endif
    {
        rv = soc_kt2_cosq_min_clear(unit, port, node->hw_index,
                node->level);
    }
    return rv;
}

int
_bcm_egress_metering_freeze(int unit, bcm_gport_t gport, void **setting)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    uint32 *bmap, *bmap_list[6];
    int word_count, wdc[6], blk_size=0;
    int lvl, j, index, word, bit, index_count, count;
    int idx, upper_flag = 0;
    int prev_idx, prev_upper_flag;
    mem_entry_t *buffer;
    uint32 mant = 0, cycle = 0 , f_idx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 lvl_count = 0;
    uint32 min = 0;
    uint32 exp = 0;
    uint32 sched_mode = 0;
    int wt = 0;
    uint32 burst_mant = 0;
    uint32 burst_exp = 0;
    uint32 *l2bmap = NULL;
    int alloc_size;
    bcm_port_t port;
    soc_info_t *si;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *l1node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;

    static const soc_mem_t parent_mems[] = {
        0,
        LLS_L0_PARENTm,
        LLS_L0_PARENTm,
        LLS_L0_PARENTm,
        LLS_L1_PARENTm,
        LLS_L2_PARENTm
    };

    static const soc_mem_t cfg_mems[][2] = {
        { LLS_PORT_SHAPER_CONFIG_Cm, INVALIDm },
        { LLS_S0_SHAPER_CONFIG_Cm, INVALIDm },
        { LLS_S1_SHAPER_CONFIG_Cm, INVALIDm },
        { LLS_L0_SHAPER_CONFIG_Cm, LLS_L0_MIN_CONFIG_Cm },
        { LLS_L1_SHAPER_CONFIG_Cm, LLS_L1_MIN_CONFIG_Cm },
        { LLS_L2_SHAPER_CONFIG_LOWERm, LLS_L2_MIN_CONFIG_LOWER_Cm },
        { LLS_L2_SHAPER_CONFIG_UPPERm, LLS_L2_MIN_CONFIG_UPPER_Cm },
    };

    static const soc_field_t rate_mant_fields[] = {
        C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
        C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
        C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
        C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };

    static const soc_field_t rate_exp_fields[] = {
        C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
        C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
        C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
        C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
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

    SOC_EGRESS_METERING_LOCK(unit);

    alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, LLS_L2_PARENTm));

    l2bmap = sal_alloc(alloc_size, "l2bmap");
    if(l2bmap == NULL) {
        rv = SOC_E_MEMORY;
        goto _ka2_freeze_abort;
    }

    sal_memset(l2bmap, 0, alloc_size);
    rv = _bcm_kt2_cosq_node_get(unit, gport, NULL, &port, NULL, &node);
    if (rv < 0) {
        sal_free(l2bmap);
        goto _ka2_freeze_abort;
    }

    l1node = node->parent;
    for (cur_node = l1node->child; cur_node != NULL ; cur_node = cur_node->sibling) {
         SHR_BITSET(l2bmap, cur_node->hw_index);
    }

    bmap_list[0] = NULL;
    bmap_list[1] = SOC_CONTROL(unit)->port_lls_s0_bmap[port];
    bmap_list[2] = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
    bmap_list[3] = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
    bmap_list[4] = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
    bmap_list[5] = l2bmap;

    si = &SOC_INFO(unit);
    /*
     * On KTX we can have both s0 and s1 node or only
     * s1 node but can not have only s0 node
     */
    bmap = bmap_list[2];
    index_count = _SOC_KT2_COSQ_MAX_INDEX_S1;
    word_count = _SHR_BITDCLSIZE(index_count);
    for (word = 0; word < word_count; word++) {
        if (bmap[word] != 0) {
            break;
        }
    }
    if (word == word_count) {
        /* s1 list is zero */
        bmap =  bmap_list[1];
        index_count = _SOC_KT2_COSQ_MAX_INDEX_S0;
        word_count = _SHR_BITDCLSIZE(index_count);
        for (word = 0; word < word_count; word++) {
            if (bmap[word] != 0) {
                break;
            }
        }
        if (word < word_count) {
            /* s0 list is non zero */
            /* swap  s0 ans s1 list */
            bmap = bmap_list[2];
            bmap_list[2] = bmap_list[1];
            bmap_list[1] = bmap;
        }
    }
    count = 0;
    /* Count the number of the valid Root,S0,S1
     * L0/L1/L2 entries per port.
     * To allocate the buffer to store the Min, Max shaper
     * entries for the valid indices
     */
    for (lvl = 0; lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
        lvl_count = 0;
        if (lvl == _SOC_KT2_COSQ_NODE_LVL_ROOT) {
            /* Port entry */
            lvl_count = 1;
            wdc[lvl] = 0;
        } else {
            bmap = bmap_list[lvl];
            mem = parent_mems[lvl];
            index_count = soc_mem_index_count(unit, mem);

            if (lvl == _SOC_KT2_COSQ_NODE_LVL_S0) {
                index_count = _SOC_KT2_COSQ_MAX_INDEX_S0;
            } else
                if (lvl == _SOC_KT2_COSQ_NODE_LVL_S1) {
                    index_count =_SOC_KT2_COSQ_MAX_INDEX_S1;
                }
            wdc[lvl] = _SHR_BITDCLSIZE(index_count);
            word_count = wdc[lvl];

            for (word = 0; word < word_count; word++) {
                if (bmap[word] != 0) {
                    lvl_count += _shr_popcount(bmap[word]);
                }
            }
            if (lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) {
                /* for MIN/MAX  shaper config */
                /* count allocation is based on worst case
                 * estimate that entries might fall into different
                 * table entries
                 */
                lvl_count = (lvl_count * 2);
            }
        }
        count += lvl_count;
    }
    count = count + 1;


    buffer = sal_alloc((count + 1) * sizeof(mem_entry_t),
            "shaper buffer");

    if (buffer == NULL) {
        rv = SOC_E_MEMORY;
        sal_free(l2bmap);
        goto _ka2_freeze_abort;
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
        sal_free(l2bmap);
        goto _ka2_freeze_abort;
    }
    buffer[count].mem = mem;
    buffer[count].index = index;
    count++;

    /* Set the threshold for the port to max shaper rate */
    rv = soc_kt2_cosq_max_bucket_set(unit, port, index,
            _SOC_KT2_COSQ_NODE_LVL_ROOT);
    if (BCM_FAILURE(rv)) {
        sal_free(buffer);
        sal_free(l2bmap);
        goto _ka2_freeze_abort;
    }

    /* S0, S1, L0, L1, L2 meter */
    for (lvl = _SOC_KT2_COSQ_NODE_LVL_S0;
            lvl < _SOC_KT2_COSQ_NODE_LVL_MAX; lvl++) {
        idx = upper_flag = 0;
        prev_idx = prev_upper_flag = -1;
        blk_size = ((lvl == _SOC_KT2_COSQ_NODE_LVL_L2) ? 8:
                ((lvl == _SOC_KT2_COSQ_NODE_LVL_L1)? 4: 1));
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

                    if (mem == INVALIDm) {
                        continue;
                    }

                    if (lvl == _SOC_KT2_COSQ_NODE_LVL_L2) {
                        if ((index % 8) >= 4) {
                            mem = cfg_mems[lvl+1][j];
                            upper_flag = 1;
                        } else {
                            upper_flag = 0;
                        }
                    }

                    /* Store the Threshold MIN/MAX table entries only
                     * once per table index as for the
                     * next consecutive L1/L2 indices falling on
                     * to the same table index need not be stored
                     */
                    if ((prev_idx != idx) ||
                            ((lvl == _SOC_KT2_COSQ_NODE_LVL_L2) &&
                             (prev_upper_flag != upper_flag))) {
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx,
                                &buffer[count].entry);
                        if (BCM_FAILURE(rv)) {
                            sal_free(buffer);
                            sal_free(l2bmap);
                            goto _ka2_freeze_abort;
                        }
                        buffer[count].mem = mem;
                        buffer[count].index = idx;
                        count++;
                    }

                    if (lvl == _SOC_KT2_COSQ_NODE_LVL_L2){
                        if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
                            /* coverity[leaked_storage] */
                            rv = soc_kt2_cosq_get_sched_mode(unit, port, lvl,
                                    index, &sched_mode, &wt);
                        } else {
                            /* coverity[leaked_storage] */
                            /* coverity[resource_leak : FALSE] */
                            rv = soc_kt2_cosq_get_sched_mode(unit, port, lvl - 2,
                                    index, &sched_mode, &wt);
                        }
                        if (BCM_FAILURE(rv)) {
                            sal_free(buffer);
                            sal_free(l2bmap);
                            goto _ka2_freeze_abort;
                        }

                        if ( sched_mode == BCM_COSQ_STRICT )
                        {
                            rv = _soc_kt2_cosq_min_bucket_get(unit, port,
                                    index, lvl,
                                    &min, &mant,
                                    &exp, &cycle);
                            if (BCM_FAILURE(rv)) {
                                sal_free(buffer);
                                sal_free(l2bmap);
                                goto _ka2_freeze_abort;
                            }
                            if (min) {
                                if (exp != 0) {
                                    if (exp < 10) {
                                        burst_exp = 5;
                                        burst_mant = 0;
                                    } else {
                                        burst_exp = exp - 4;
                                        burst_mant = (mant >> 3);
                                    }
                                }
                                else {
                                    burst_mant = 0;
                                    burst_exp = 0;
                                }
                            }
                        }
                        if (!min)
                        {
                            /* Set the  MIN/MAX thresholds for L2 node
                             * to 2 Kbps
                             */
                            mant = 1;
                            cycle = 4;
                            exp = 0;
                        }
                        f_idx = (j * 4) + (index % 4);
                        sal_memset(entry, 0, (sizeof(uint32) * SOC_MAX_MEM_WORDS));
                        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                idx, &entry);
                        if (BCM_FAILURE(rv)) {
                            sal_free(buffer);
                            sal_free(l2bmap);
                            goto _ka2_freeze_abort;
                        }

                        soc_mem_field32_set(unit, mem, &entry,
                                rate_mant_fields[f_idx], mant);
                        soc_mem_field32_set(unit, mem, &entry,
                                rate_exp_fields[f_idx], exp);
                        soc_mem_field32_set(unit, mem, &entry,
                                burst_exp_fields[f_idx], burst_exp);
                        soc_mem_field32_set(unit, mem, &entry,
                                burst_mant_fields[f_idx], burst_mant);
                        soc_mem_field32_set(unit, mem, &entry,
                                cycle_sel_fields[f_idx], cycle);
                        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY,
                                idx, &entry);
                        min = 0;
                        sched_mode = 0;
                        cycle = 0;
                        exp = 0;
                        mant = 0;
                        burst_exp = 0;
                        burst_mant = 0;
                    } else if (((lvl >= _SOC_KT2_COSQ_NODE_LVL_L0) && (j == 1)) ||
                            (lvl < _SOC_KT2_COSQ_NODE_LVL_L0)) {
                        /* Set the MIN/MAX thresholds for L0/L1 node
                         * to max shaper rate
                         */
                        rv = soc_kt2_cosq_max_bucket_set(unit, port,
                                index, lvl);
                    }
                    if (BCM_FAILURE(rv)) {
                        sal_free(buffer);
                        sal_free(l2bmap);
                        goto _ka2_freeze_abort;
                    }
                }/*END of for j */
                prev_idx = idx;
                prev_upper_flag = upper_flag;
            } /* END of the valid bit */
        } /* END of the word */
    } /* END of lvl */

    buffer[count].mem = INVALIDm;
    *setting = buffer;
_ka2_freeze_abort:
    if (BCM_FAILURE(rv)) {
        /* UNLOCK if fail */
        SOC_EGRESS_METERING_UNLOCK(unit);
    }
    return rv;
}

int
_bcm_egress_metering_thaw(int unit, void *setting)
{
    int  rv = BCM_E_NONE;
    mem_entry_t *buffer;
    int index;

    if (setting == NULL) {
        SOC_EGRESS_METERING_UNLOCK(unit);
        return SOC_E_NONE;
    }
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
 *     _bcm_kt2_cosq_sched_config_set
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
_bcm_kt2_cosq_sched_config_set(int unit, bcm_gport_t gport, 
                               _bcm_kt2_cosq_state_t state)
{
    int rv = BCM_E_NONE;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node, *parent;
    mmu_rqe_queue_op_node_map_entry_t rqe_op_node;
    mmu_wred_queue_op_node_map_entry_t wred_op_node;
    egr_queue_to_pp_port_map_entry_t pp_port_map;
    _bcm_kt2_subport_info_t info;
    bcm_gport_t pp_port;
    lls_l2_child_state1_entry_t l2_child_state;
    uint32 entry_c[SOC_MAX_MEM_WORDS];
    uint32 entry_p[SOC_MAX_MEM_WORDS];
    int port, physical_port, start_spri = 0;
    int update_spri_vector = 0;
    uint8 spri_vector = 0, spri_select = 0;
    int parent_index, numq;
    uint8 vec_3_0, vec_7_4;
    uint32 spare, min_list, ef_list, not_empty;
    soc_mem_t config_mem, parent_mem;
    soc_field_t start_spri_field = P_START_UC_SPRIf;
    bcm_port_t local_port;
    uint32 *bmap = NULL;
    void *setting = NULL;
    int s1_index = -1, link_state = BCM_PORT_LINK_STATUS_UP;
    _bcm_kt2_cosq_node_t *s1_node = NULL;
    soc_mem_t mem_c[] = {
                        LLS_PORT_CONFIGm,
                        LLS_S1_CONFIGm,
                        INVALIDm,
                        LLS_L0_CONFIGm,
                        LLS_L1_CONFIGm
                      };
    soc_mem_t mem_p[] = {
                        LLS_L0_PARENTm,
                        LLS_L0_PARENTm,
                        INVALIDm,
                        LLS_L1_PARENTm, 
                        LLS_L2_PARENTm
                      };
    mmu_thdo_opnconfig_cell_entry_t     mmu_thdo_opnconfig_cell_entry={{0}};
    mmu_thdo_opnconfig_qentry_entry_t   mmu_thdo_opnconfig_qentry_entry={{0}};
    mmu_enq_src_ppp_to_s1_lookup_entry_t ppp_to_s1;

    memset(&ppp_to_s1, 0, sizeof(ppp_to_s1));
    
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &port, NULL,
                               &node));

    /* MMU_ENQ_SRC_PPP_TO_S1_LOOKUP is programmed
     * to map the PP ports to S1 node associated with
     * COE port for E2ECC COE Flow Control
     */
    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
        rv = _bcm_kt2_cosq_subport_get(unit, gport, &pp_port);
        if (rv == BCM_E_NONE) {
            BCM_IF_ERROR_RETURN
               (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                &info));
            if (info.port_type == _BCM_KT2_SUBPORT_TYPE_SUBTAG) {
                SOC_IF_ERROR_RETURN
                     (READ_MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm(unit, MEM_BLOCK_ALL,
                   pp_port, &ppp_to_s1));
                if (state == _BCM_KT2_COSQ_STATE_ENABLE) {
                    soc_mem_field32_set(unit, MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm,
                                        &ppp_to_s1, S1_VLDf, 1);
                    soc_mem_field32_set(unit, MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm,
                                        &ppp_to_s1, S1f, node->hw_index);
                    SOC_IF_ERROR_RETURN
                      (WRITE_MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm(unit,
                             MEM_BLOCK_ALL,
                             pp_port, &ppp_to_s1));

                } else if (state == _BCM_KT2_COSQ_STATE_DISABLE) {
                    SOC_IF_ERROR_RETURN
                      (WRITE_MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm(unit,
                          MEM_BLOCK_ALL,
                          pp_port,
                          (soc_mem_entry_null(unit,
                          MMU_ENQ_SRC_PPP_TO_S1_LOOKUPm))));
                }

            }
        }
    }
    parent = node->parent;
    /*
     * Programming the LLS_L0_PARENT and LLS_S1_CONFIG so that
     *  credits are exchanged between the TXLP and MMU is proper in case of LinkPHY
     */
    if (soc_feature(unit, soc_feature_discontinuous_pp_port) &&
        parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
        sal_memset(entry_p, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        parent_mem = LLS_L0_PARENTm;
        soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf,
                parent->hw_index);
        soc_mem_field32_set(unit, parent_mem, &entry_p, C_TYPEf, 0);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, 
                           node->hw_index, &entry_p));
        config_mem = LLS_S1_CONFIGm;
        sal_memset(entry_c, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        /* Programming the L0 start node, since l0==s1,
         * using parent->hw_index
         */
        soc_mem_field32_set(unit, config_mem, &entry_c, P_START_SPRIf, 
                node->hw_index);
        soc_mem_field32_set(unit, config_mem, &entry_c, P_NUM_SPRIf, 
                1);
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, 
                                    parent->hw_index, 
                                    &entry_c));
    } else if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) &&
        (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S1)) {
        sal_memset(entry_p, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
        parent_mem = LLS_L0_PARENTm;
        soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf,
                parent->hw_index);
        soc_mem_field32_set(unit, parent_mem, &entry_p, C_TYPEf, 0);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, 
                           parent->hw_index, &entry_p));
        if ((parent->hw_index >= 0) && (parent->hw_index <=127)) {
            config_mem = LLS_S1_CONFIGm;
            sal_memset(entry_c, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
            /* Programming the L0 start node, since l0==s1,
             * using parent->hw_index
             */
            soc_mem_field32_set(unit, config_mem, &entry_c, P_START_SPRIf, 
                    parent->hw_index);
            soc_mem_field32_set(unit, config_mem, &entry_c, P_NUM_SPRIf, 
                   1);
            (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, 
                                       parent->hw_index, 
                                       &entry_c));
        }

    }
    
    physical_port = port;
    rv = _bcm_kt2_cosq_subport_get(unit, gport, &pp_port);
    if (rv == BCM_E_NONE) {
        BCM_IF_ERROR_RETURN
            (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                      &info));
        physical_port = info.port;
        if (state == _BCM_KT2_COSQ_STATE_ENABLE) {
            node->local_port = physical_port;
        }
    }
 
    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
        bmap = SOC_CONTROL(unit)->port_lls_s0_bmap[physical_port];
    } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) {
        bmap = SOC_CONTROL(unit)->port_lls_s1_bmap[physical_port];
    } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
        bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[physical_port];
    } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
        bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[physical_port];
    } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[physical_port];
    } else {
        return BCM_E_PARAM;
    }

    if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0 ||
        node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) &&
        (state == _BCM_KT2_COSQ_STATE_ENABLE ||
         state == _BCM_KT2_COSQ_STATE_DISABLE) &&
        (!SOC_IS_METROLITE(unit) || !node->linkphy_enabled)) {
        if (state == _BCM_KT2_COSQ_STATE_ENABLE) { 
            SHR_BITSET(bmap, node->hw_index);
        } else if (state == _BCM_KT2_COSQ_STATE_DISABLE) {
            SHR_BITCLR(bmap, node->hw_index);
        }      
        return BCM_E_NONE;
    }

    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0 &&
        (state == _BCM_KT2_COSQ_STATE_WRR_TO_SPRI ||
         state == _BCM_KT2_COSQ_STATE_SPRI_TO_WRR)) {
         return BCM_E_NONE;
    }
    

    if (((parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S1)  ||
        (SOC_IS_METROLITE(unit) && parent->linkphy_enabled)) &&
        node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
        return BCM_E_NONE;
    }

    numq = (parent->wrr_mode == 0) ? ((parent->numq > 8) ? 8 : parent->numq) : 1;
    if ((node->cosq_attached_to < numq) && (parent->base_index < 0) && 
        (parent->mc_base_index < 0)) {
        return BCM_E_PARAM;
    }

    config_mem = mem_c[parent->level];
    if ((node->cosq_attached_to < numq) && (parent->hw_index != -1)) {
        start_spri = (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                      BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) ?
                      parent->mc_base_index : parent->base_index; 
        spri_vector = 1 << node->cosq_attached_to;
        update_spri_vector =  1;

        sal_memset(entry_c, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);   

        if ((parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S0)
                && (parent->hw_index >= 128)) {
            config_mem = LLS_L0_CONFIGm; 
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, parent->hw_index, 
                          &entry_c));
        vec_3_0 = soc_mem_field32_get(unit, config_mem, &entry_c, 
                                      P_NUM_SPRIf);
        vec_7_4 = soc_mem_field32_get(unit, config_mem, &entry_c, 
                                      P_VECT_SPRI_7_4f);
        if (state == _BCM_KT2_COSQ_STATE_ENABLE || 
            state == _BCM_KT2_COSQ_STATE_WRR_TO_SPRI) {
            vec_3_0 |= (spri_vector & 0xF);
            vec_7_4 |= (spri_vector >> 4); 
        } else if ((state == _BCM_KT2_COSQ_STATE_SPRI_TO_WRR) ||
               (state == _BCM_KT2_COSQ_STATE_DISABLE)) {
            vec_3_0 &= ~(spri_vector & 0xF);
            vec_7_4 &= ~(spri_vector >> 4);
        } 

        /* update LLS_CONFIG with the updated vector */ 
        if (config_mem == LLS_L1_CONFIGm) {
            spri_select = soc_mem_field32_get(unit, config_mem, &entry_c,
                                              P_SPRI_SELECTf);
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
                start_spri_field = P_START_MC_SPRIf;
                spri_select |= spri_vector;    
            } else {
                spri_select &= ~(spri_vector);
            }
            /* Valid if num_spri is not 0 */
            if (vec_3_0 != 0) {
                soc_mem_field32_set(unit, config_mem, &entry_c, start_spri_field, start_spri);
            }
            soc_mem_field32_set(unit, config_mem, &entry_c, P_SPRI_SELECTf, 
                                spri_select);
        } else {
            /* Valid if num_spri is not 0 */
            if (vec_3_0 != 0) {
                soc_mem_field32_set(unit, config_mem, &entry_c, P_START_SPRIf, start_spri);
            }
        }
        soc_mem_field32_set(unit, config_mem, &entry_c, P_NUM_SPRIf, vec_3_0);
        soc_mem_field32_set(unit, config_mem, &entry_c, P_VECT_SPRI_7_4f, vec_7_4); 
    }

    parent_mem = mem_p[parent->level]; 
    sal_memset(entry_p, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    sal_memset(&wred_op_node, 0, sizeof(mmu_wred_queue_op_node_map_entry_t));

    switch (state) {
        case _BCM_KT2_COSQ_STATE_ENABLE:
            if ((parent_mem == LLS_L0_PARENTm) &&
                (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) &&
                (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0)) {
                soc_mem_field32_set(unit, parent_mem, &entry_p, C_TYPEf, 1); 
            }
            if (parent->level != _BCM_KT2_COSQ_NODE_LEVEL_S1) {
                soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf, 
                                    parent->hw_index);
                if (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                    soc_mem_field32_set(unit, parent_mem, &entry_p, C_TYPEf, 0);
                }
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, 
                                   node->hw_index, &entry_p));
            }
            if (update_spri_vector && parent->linkphy_enabled == 0) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, 
                                   parent->hw_index, &entry_c));
            }

            if (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                /* Read the entry from the RQE map in order to preserve 
                 * the E_TYPE field 
                 */
                SOC_IF_ERROR_RETURN
                    (READ_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                     node->hw_index,
                                                     &rqe_op_node));
                soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, 
                                    &rqe_op_node,
                                    OP_NODEf, parent->hw_index);           
                soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, 
                                    &wred_op_node,
                                    OP_NODEf, parent->hw_index);    
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                      node->hw_index,
                                                      &rqe_op_node));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                       node->hw_index,
                                                       &wred_op_node)); 

                if ((soc_feature(unit, soc_feature_mmu_packing) &&
                    IS_EXT_MEM_PORT(unit, port)) &&
                    (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                     BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
                     BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_cosq_packing_mode_set(unit, node->hw_index, 1));
                }

                /*
                 * Enable queue only if link is up. If port is down then
                 * queue will be enabaled during link up event.
                 */
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_node_get(unit, gport, NULL ,&local_port, NULL,
                                            NULL));
                rv =  _bcm_esw_link_get(unit, local_port, &link_state);
                if (rv == BCM_E_NONE) {
                    if (link_state == BCM_PORT_LINK_STATUS_UP) {
                        _soc_kt2_cosq_end_port_flush(unit,node->hw_index);
                    }
                } else {
                    _soc_kt2_cosq_end_port_flush(unit,node->hw_index);
                }

                if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
                    BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
                    BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
                    SOC_IF_ERROR_RETURN
                        (READ_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL, 
                                                        node->hw_index,
                                                        &pp_port_map));
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_cosq_node_get(unit, gport, NULL ,&local_port, NULL,
                                                NULL));

                    soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm, 
                            &pp_port_map, PP_PORTf, local_port);
                    SOC_IF_ERROR_RETURN
                        (WRITE_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL, 
                                                         node->hw_index, 
                                                         &pp_port_map));    
                }              
                
                rv = _bcm_kt2_cosq_subport_get(unit, gport, &pp_port);
                if (rv == BCM_E_NONE) {
                    BCM_IF_ERROR_RETURN
                       (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                        &info));
                    port = info.port;
                    SOC_IF_ERROR_RETURN
                    (READ_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                                    node->hw_index,
                                                    &pp_port_map));
                    soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm,
                                        &pp_port_map, PP_PORTf, pp_port);

                    if (info.port_type == _BCM_KT2_SUBPORT_TYPE_LINKPHY) {
                        /* LinkPhy - two streams per S0 node
                         * To understand the queue association with the streams
                         * read the parent L1 and S1 associated with the queue
                         * read the stream ID
                         */
                        if ((parent->parent) && (parent->parent->parent)) {
                           s1_node = parent->parent->parent;
                           if (SOC_IS_METROLITE(unit)) {
                               /* Metrolite doesn't have an S0 node in hardware.
                                * However, SW treats the level as S0 so as to be
                                * consistant with subport APIs.
                                */
                               if (s1_node->level != _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                                   return BCM_E_INTERNAL;
                               }
                           } else if (s1_node->level != _BCM_KT2_COSQ_NODE_LEVEL_S1) { 
                               return BCM_E_INTERNAL;
                           }
                           s1_index = s1_node->hw_index;
                           if (s1_index < 0) {
                               return BCM_E_INTERNAL;
                           }
                        } else {
                           return BCM_E_INTERNAL;
                        }
                        soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm, 
                                    &pp_port_map, DEVICE_STREAM_IDf,
                                    s1_index);
                    } else {
                        soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm,
                                    &pp_port_map, SUBPORT_TAGf,
                                    info.subtag);
                    }
                    SOC_IF_ERROR_RETURN
                    (WRITE_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                                     node->hw_index,
                                                     &pp_port_map));
                }
                SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_CELLm(unit,
                                    MEM_BLOCK_ANY, parent->hw_index,     
                                    &mmu_thdo_opnconfig_cell_entry));
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                    &mmu_thdo_opnconfig_cell_entry, 
                                    PIDf, port);
                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_CELLm(unit,
                                    MEM_BLOCK_ALL, parent->hw_index,     
                                    &mmu_thdo_opnconfig_cell_entry));
                SOC_IF_ERROR_RETURN(READ_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                                    MEM_BLOCK_ANY, parent->hw_index ,        
                                    &mmu_thdo_opnconfig_qentry_entry));
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                    &mmu_thdo_opnconfig_qentry_entry,
                                    PIDf, port);
                SOC_IF_ERROR_RETURN(WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit,
                                    MEM_BLOCK_ALL, parent->hw_index ,        
                                    &mmu_thdo_opnconfig_qentry_entry));
            }
            SHR_BITSET(bmap, node->hw_index);
            break;
        case _BCM_KT2_COSQ_STATE_DISABLE:
            mmu_info = _bcm_kt2_mmu_info[unit];
            if (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
                parent_index = (1 << soc_mem_field_length(unit, parent_mem, 
                                                          C_PARENTf)) - 1;
            } else {    
                parent_index = mmu_info->max_nodes[parent->level] - 1;
            }    
            soc_mem_field32_set(unit, parent_mem, &entry_p, C_PARENTf, 
                                parent_index); 
                                        
            if (parent->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                /* flush the queue  */
                BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_flush_queues(
                                    unit, physical_port, node, gport));
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
                                      (uint32 *)&l2_child_state, 
                                      C_ON_WERR_LISTf, &spare);
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
                                      (uint32 *)&l2_child_state, 
                                      C_ON_WERR_LISTf, &spare);
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

                if ((soc_feature(unit, soc_feature_mmu_packing) &&
                    IS_EXT_MEM_PORT(unit, port)) &&
                    (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                     BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
                     BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_cosq_packing_mode_set(unit, node->hw_index, 0));
                }

                SOC_IF_ERROR_RETURN
                    (READ_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                     node->hw_index,
                                                     &rqe_op_node));
                soc_mem_field32_set(unit, MMU_RQE_QUEUE_OP_NODE_MAPm, 
                                    &rqe_op_node,
                                    OP_NODEf, parent_index);
                soc_mem_field32_set(unit, MMU_WRED_QUEUE_OP_NODE_MAPm, 
                                    &wred_op_node,
                                    OP_NODEf, parent_index);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, 
                                   node->hw_index, &entry_p));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_RQE_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                      node->hw_index,
                                                      &rqe_op_node));
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_WRED_QUEUE_OP_NODE_MAPm(unit, MEM_BLOCK_ALL, 
                                                       node->hw_index,
                                                       &wred_op_node)); 
            }else {
                if (parent->level != _BCM_KT2_COSQ_NODE_LEVEL_S1) { 
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, parent_mem, MEM_BLOCK_ANY, 
                                       node->hw_index, 
                                       &entry_p));
                }
            }
            if (update_spri_vector && parent->linkphy_enabled == 0) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, config_mem, MEM_BLOCK_ANY, 
                                   parent->hw_index, 
                                   &entry_c));
            }
            SHR_BITCLR(bmap, node->hw_index);
            break;
        case _BCM_KT2_COSQ_STATE_WRR_TO_SPRI:
        case _BCM_KT2_COSQ_STATE_SPRI_TO_WRR:
            if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) {
                SOC_IF_ERROR_RETURN
                    (soc_kt2_cosq_s0_sched_set(unit, parent->hw_index,
                     (state == _BCM_KT2_COSQ_STATE_WRR_TO_SPRI) ? 1 : 0));
                return BCM_E_NONE;
            }

            if (update_spri_vector) {
#ifdef BCM_KATANA2_SUPPORT
                if ( SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit) && !SOC_IS_METROLITE(unit) &&
                     (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
                            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) ) {
                        SOC_IF_ERROR_RETURN(_bcm_egress_metering_freeze(unit,
                                    gport, &setting));

                        rv = soc_kt2_cosq_max_bucket_set(unit, physical_port, node->parent->hw_index, _SOC_KT2_COSQ_NODE_LVL_L1);
                        if (BCM_FAILURE(rv)) {
                            SOC_IF_ERROR_RETURN(_bcm_egress_metering_thaw(unit, setting));
                            return rv;
                        }

                        rv = _bcm_kt2_cosq_l2_max_bucket_set(unit, physical_port, node);
                        if (BCM_FAILURE(rv)) {
                            SOC_IF_ERROR_RETURN(_bcm_egress_metering_thaw(unit, setting));
                            return rv;
                        }
                        /*
                         *  for WERR to SP it is possible to have the node in two list min and WERR which
                         *  may lead to time out. To avoid this we need to configure the min as 0
                         */
                        rv = _bcm_kt2_cosq_min_clear(unit, physical_port, node);
                        if (BCM_FAILURE(rv)) {
                            SOC_IF_ERROR_RETURN(_bcm_egress_metering_thaw(unit, setting));
                            return rv;
                        }
                        rv = _bcm_kt2_cosq_dynamic_sp_werr_change(unit, port,
                                node->level,
                                node->hw_index,
                                parent->hw_index, entry_c,
                                state);
                        SOC_IF_ERROR_RETURN(_bcm_egress_metering_thaw(unit, setting));
                        if (BCM_FAILURE(rv)) {
                            return rv;
                        }
                }else 
#endif                 
                {
                    SOC_IF_ERROR_RETURN(_soc_egress_metering_freeze(unit,
                                physical_port, &setting));

                    rv = _bcm_kt2_cosq_l2_max_bucket_set(unit, physical_port, node);
                    if (BCM_FAILURE(rv)) {
                        SOC_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit, physical_port, setting));
                        return rv;
                    }
                    /*
                     *  for WERR to SP it is possible to have the node in two list min and WERR which
                     *  may lead to time out. To avoid this we need to configure the min as 0
                     */
                    rv = _bcm_kt2_cosq_min_clear(unit, physical_port, node);
                    if (BCM_FAILURE(rv)) {
                        SOC_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit, physical_port, setting));
                        return rv;
                    }
                    rv = _bcm_kt2_cosq_dynamic_sp_werr_change(unit, port,
                            node->level,
                            node->hw_index,
                            parent->hw_index, entry_c,
                            state);
                    SOC_IF_ERROR_RETURN(_soc_egress_metering_thaw(unit, physical_port, setting));
                    if (!BCM_SUCCESS(rv)) {
                        return rv;
                    }
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
 *     _bcm_kt2_cosq_sched_node_set
 * Purpose:
 *     Set LLS scheduler registers based on GPORT
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_cosq_sched_node_set(int unit, bcm_gport_t gport, 
                                    _bcm_kt2_cosq_state_t state)
{
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        return _bcm_kt2_cosq_sched_config_set(unit, gport, state);
    }    
    
    return BCM_E_PARAM;
}

/*
 * Function:
 *     _bcm_kt2_cosq_port_base_queue_get
 * Purpose:
 *     Get base queue node for a port given any gport
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     node                - (OUT) Base queue node 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_cosq_port_base_queue_get(int unit, bcm_port_t port, int cosq,
                                  _bcm_kt2_cosq_node_t **node)
{
    int queue_idx, numq, local_port;
    _bcm_kt2_mmu_info_t *mmu_info;

    if (node == NULL) {
        return BCM_E_PARAM;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
    numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;
    if (cosq > numq) {
        return BCM_E_PARAM;
    }
    queue_idx = mmu_info->port[local_port].q_offset + cosq;

    if (queue_idx > mmu_info->num_queues) {
        return BCM_E_INTERNAL;
    }
    *node = &mmu_info->queue_node[queue_idx];
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_sched_set
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
_bcm_kt2_cosq_sched_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int mode, int num_weights, const int weights[])
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node, *cur_node = NULL, *l0_node, *temp_node = NULL;
    bcm_port_t local_port;
    int index;
    int i, default_weight = -1;
    int max_weight;
    uint32 fval;
    lls_l0_child_weight_cfg_cnt_entry_t l0_weight_cfg;
    lls_l1_child_weight_cfg_cnt_entry_t l1_weight_cfg; 
    lls_l2_child_weight_cfg_cnt_entry_t l2_weight_cfg;
    lls_l2_parent_entry_t l2_parent;
    _bcm_kt2_cosq_state_t state = _BCM_KT2_COSQ_STATE_NO_CHANGE;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv;
    int status = 0;
    int physical_port;
  
    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    /* Validate weight values */
    if (mode == BCM_COSQ_ROUND_ROBIN ||
        mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN ||
        mode == BCM_COSQ_DEFICIT_ROUND_ROBIN ||
        mode == BCM_COSQ_STRICT) {
        max_weight =
            (1 << soc_mem_field_length(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                       C_WEIGHTf)) - 1;
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

    mmu_info = _bcm_kt2_mmu_info[unit];

    if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_port_base_queue_get(unit, port, cosq, &cur_node));
        if (cur_node) {
            node = cur_node->parent;
        }
    }
    
    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                   &node));        
        if ((node->wrr_mode == 1) && (mode == BCM_COSQ_STRICT)) {
            return BCM_E_PARAM;
        }
    } else if (!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) &&
            ((node = node->child) == NULL)) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }
        if (((node = l0_node->child) == NULL) && !(IS_CPU_PORT(unit, local_port))) {
            return BCM_E_PARAM;
        }
        /*
         * Adding HG and LB port too. These two ports have 24 queues
         * resulting in three L1 nodes. During scheduler configuration
         * we need to find first L1 nodes which is last in sibling list.
         */
        if (IS_CPU_PORT(unit, local_port) ||
            IS_HG_PORT(unit, local_port)  ||
            IS_LB_PORT(unit, local_port)) { 
            /*
             * In case when number of cpu queues are not multiples of 8
             * there is chance last siblings gets numq < 8,
             * which may result in failure , So node is moved to the 
             * first sibling, which guarantee to have numq = 8
             */
            for (cur_node = l0_node; cur_node->sibling != NULL;
                 cur_node = cur_node->sibling);
            for (cur_node = cur_node->child; cur_node->sibling != NULL;
                 cur_node = cur_node->sibling);
            node = cur_node;
        }
        if (soc_feature(unit, soc_feature_mmu_packing)) {
            for (cur_node = node; cur_node != NULL;
                    cur_node = cur_node->sibling) {
                if(cur_node->cosq_attached_to == cosq) {
                    node = cur_node;
                    break;
                }
            }
            if (cur_node == NULL) {
                /* this cosq is not yet attached */
                return BCM_E_INTERNAL;
            }
        }
    }

    if (!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        if (!soc_feature(unit, soc_feature_mmu_packing)) {
            if ((mode != BCM_COSQ_STRICT) && 
                    (node->numq > 0 && (cosq + num_weights) > node->numq)) {
                return BCM_E_PARAM;
            }


            if (node->cosq_attached_to < 0) { /* Not resolved yet */
                return BCM_E_NOT_FOUND;
            }

            for (cur_node = node->child; cur_node != NULL;
                    cur_node = cur_node->sibling) {
                if (cur_node->cosq_attached_to == cosq) {
                    break;
                }
            }
        } else {
            for (cur_node = node->child; cur_node != NULL;
                    cur_node = cur_node->sibling) {
                if (cur_node->cosq_attached_to == cosq) {
                    break;
                }
            }
        }
    }

    if (cur_node == NULL) {
        /* this cosq is not yet attached */
        return BCM_E_INTERNAL;
    }

    if ((cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) &&
        (mode == BCM_COSQ_STRICT)) {
        /* S0 supports only RR. */
        return BCM_E_PARAM;
    }

    if (cur_node->wrr_in_use == 1 && mode == BCM_COSQ_STRICT) {
        if (cur_node->cosq_attached_to >= 8) {
            return BCM_E_PARAM;
        }  
        state = _BCM_KT2_COSQ_STATE_WRR_TO_SPRI;        
    }    

    if (cur_node->wrr_in_use == 0 && mode != BCM_COSQ_STRICT) {
        state = _BCM_KT2_COSQ_STATE_SPRI_TO_WRR;
    }    

    if (state == _BCM_KT2_COSQ_STATE_SPRI_TO_WRR && 
        cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        SOC_IF_ERROR_RETURN
            (READ_LLS_L2_PARENTm(unit, MEM_BLOCK_ALL, cur_node->hw_index, 
                                 &l2_parent));
        if (soc_mem_field32_get(unit, LLS_L2_PARENTm, 
                                &l2_parent, C_EFf) == 1) {
            return BCM_E_PARAM;
        }
    }

    /* Protect LLS hierachy schedule mode change as
     * port flush on link flap might be effected
     * when there is a change in the LLS hierarchy
     */
    SOC_LLS_SCHEDULER_LOCK(unit);
    soc_kt_port_flush_state_get(unit, local_port, &status);
    if (status) {

        LOG_ERROR(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
         "Unit %d Port %d is in flush state,"
         "Cannot change LLS hierarchy scheduler mode for Cosq hw_cosq=%d.\n"),
        unit, local_port, cur_node->cosq_attached_to));
        SOC_LLS_SCHEDULER_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    cur_node->wrr_in_use = (mode != BCM_COSQ_STRICT) ? 1 : 0;
    if ((cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) &&
        (node->num_child == 2)) {
        if (cur_node->sibling != NULL) {
            temp_node = cur_node->sibling;
        } else if (node->child != NULL) {
            temp_node = node->child;
        }
        if (temp_node) {
            temp_node->wrr_in_use = (mode != BCM_COSQ_STRICT) ? 1 : 0;
        }
    }

    rv = _bcm_kt2_cosq_sched_node_set(unit, cur_node->gport, state);
    SOC_LLS_SCHEDULER_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        if (state == _BCM_KT2_COSQ_STATE_SPRI_TO_WRR) {
            cur_node->wrr_in_use = 0;
        } else if (state == _BCM_KT2_COSQ_STATE_WRR_TO_SPRI) {
            cur_node->wrr_in_use = 1;
        }
        if ((cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) &&
            (node->num_child == 2)) {
            if (cur_node->sibling != NULL) {
                temp_node = cur_node->sibling;
            } else if (node->child != NULL) {
                temp_node = node->child;
            }
            if (temp_node) {
                if (state == _BCM_KT2_COSQ_STATE_SPRI_TO_WRR) {
                    temp_node->wrr_in_use = 0;
                } else if (state == _BCM_KT2_COSQ_STATE_WRR_TO_SPRI) {
                    temp_node->wrr_in_use = 1;
                }
            }
        }
        return rv;
    }

    if ((mode != BCM_COSQ_STRICT))  {
        if (_bcm_is_port_linkphy_subport(unit, port, -1)) {
            BCM_IF_ERROR_RETURN(_bcm_subport_physical_port_get(unit, 
                                            local_port, &physical_port));
            local_port = physical_port;
        }
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

    /*
     * Check mode and if it is BCM_COSQ_STRICT or BCM_COSQ_ROUND_ROBIN then 
     * configure default_weight 0 or 1 respectively.
     */
    if (mode == BCM_COSQ_STRICT) {
        default_weight = 0;
    } else if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ||
               (cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ||
               (mode == BCM_COSQ_ROUND_ROBIN)) {
        default_weight = 1; 
    }

    switch (node->level) {
    case _BCM_KT2_COSQ_NODE_LEVEL_ROOT: /* port scheduler */
    case _BCM_KT2_COSQ_NODE_LEVEL_S0: /* S0 scheduler */
    case _BCM_KT2_COSQ_NODE_LEVEL_S1: /* S1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                       NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                      index, &l0_weight_cfg));
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l0_weight_cfg, 
                                C_WEIGHTf, (default_weight == -1) ? weights[i] : default_weight); 
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l0_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l0_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                       index, &l0_weight_cfg));
            if ((cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) &&
                (node->num_child == 2)) {
                if (cur_node->sibling != NULL) {
                    index = cur_node->sibling->hw_index;
                } else if (node->child != NULL) {
                    index = node->child->hw_index;
                }
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                        index, &l0_weight_cfg));
                soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                    (uint32 *)&l0_weight_cfg, 
                                    C_WEIGHTf, (default_weight == -1) ? weights[i] : default_weight); 
                soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                    (uint32 *)&l0_weight_cfg, 
                                    C_WEIGHT_SURPLUS_COUNTf, 0);
                soc_mem_field32_set(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                    (uint32 *)&l0_weight_cfg, 
                                    C_WEIGHT_COUNTf, 0);
                SOC_IF_ERROR_RETURN
                    (WRITE_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                        index, &l0_weight_cfg));
            }

        }
        break;

    case _BCM_KT2_COSQ_NODE_LEVEL_L0: /* L0 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                      index, &l1_weight_cfg));
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l1_weight_cfg, 
                                C_WEIGHTf, (default_weight == -1) ? weights[i] : default_weight); 
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l1_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l1_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                       index, &l1_weight_cfg));

        }
        break;

    case _BCM_KT2_COSQ_NODE_LEVEL_L1: /* L1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                   (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                       _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                   (READ_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                      index, &l2_weight_cfg));
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l2_weight_cfg, 
                                C_WEIGHTf, (default_weight == -1) ? weights[i] : default_weight); 
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l2_weight_cfg, 
                                C_WEIGHT_SURPLUS_COUNTf, 0);
            soc_mem_field32_set(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm, 
                                (uint32 *)&l2_weight_cfg, 
                                C_WEIGHT_COUNTf, 0);
            SOC_IF_ERROR_RETURN
                   (WRITE_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                       index, &l2_weight_cfg));

        }
        break;

    default:
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_sched_get
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
_bcm_kt2_cosq_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int *mode, int num_weights, int weights[])
{
    _bcm_kt2_mmu_info_t *mmu_info; 
    _bcm_kt2_cosq_node_t *node = NULL, *l0_node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;
    bcm_port_t local_port;
    int index;
    int i;
    lls_l0_child_weight_cfg_cnt_entry_t l0_weight_cfg;
    lls_l1_child_weight_cfg_cnt_entry_t l1_weight_cfg;
    lls_l2_child_weight_cfg_cnt_entry_t l2_weight_cfg;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int physical_port;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_port_base_queue_get(unit, port, cosq, &cur_node));
        if (cur_node) {
            node = cur_node->parent;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
           BCM_IF_ERROR_RETURN
               (_bcm_kt2_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                      &node));
    } else if (!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            local_port = _BCM_KT2_SUBPORT_PORT_ID_GET(port);
        } else {
             BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
        }  
        mmu_info = _bcm_kt2_mmu_info[unit];

        node = &mmu_info->sched_node[local_port];
        
        if (node->gport < 0) {
            return BCM_E_PARAM;
        }

        if ((node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) &&
            ((node = node->child) == NULL)) {
            return BCM_E_PARAM;
        }

        if ((l0_node = node->child) == NULL) {
            return BCM_E_PARAM;
        }
        if (((node = l0_node->child) == NULL) && !(IS_CPU_PORT(unit, local_port))) {
            return BCM_E_PARAM;
        }
        /*
         * Adding HG and LB port too. These two ports have 24 queues
         * resulting in three L1 nodes. During scheduler configuration
         * we need to find first L1 nodes which is last in sibling list.
         */
        if (IS_CPU_PORT(unit, local_port) ||
            IS_HG_PORT(unit, local_port)  ||
            IS_LB_PORT(unit, local_port)) {
            /*
             * In case when number of cpu queues are not multiples of 8
             * there is chance last siblings gets numq < 8,
             * which may result in failure , So node is moved to the 
             * first sibling, which guarantee to have numq = 8
             */
            for (cur_node = l0_node; cur_node->sibling != NULL;
                 cur_node = cur_node->sibling);
            for (cur_node = cur_node->child; cur_node->sibling != NULL;
                 cur_node = cur_node->sibling);
            node = cur_node;
        }
    }

    if (node == NULL) {
        return BCM_E_INTERNAL;
    }
    if (node->numq > 0 && (cosq + num_weights) > node->numq) {
        return BCM_E_PARAM;
    }

    if (node->cosq_attached_to < 0) { /* Not resolved yet */
        return BCM_E_NOT_FOUND;
    }

    if (!BCM_GPORT_IS_SUBPORT_PORT(port)) {
        for (cur_node = node->child; cur_node != NULL;
            cur_node = cur_node->sibling) {
            if (cur_node->cosq_attached_to == cosq) {
                break;
            }
        }
    }

    if (cur_node == NULL) {
        /* this cosq is not yet attached */
        return BCM_E_INTERNAL;
    }
    
    switch (node->level) {
    case _BCM_KT2_COSQ_NODE_LEVEL_ROOT: /* port scheduler */
    case _BCM_KT2_COSQ_NODE_LEVEL_S0: /* S0 scheduler */
    case _BCM_KT2_COSQ_NODE_LEVEL_S1: /* S1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L0_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                index, &l0_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm,
                                            (uint32 *)&l0_weight_cfg, 
                                            C_WEIGHTf);
        }
        break;

    case _BCM_KT2_COSQ_NODE_LEVEL_L0: /* L0 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L1_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL,
                                                   index, &l1_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L1_CHILD_WEIGHT_CFG_CNTm,
                                             (uint32 *)&l1_weight_cfg, 
                                             C_WEIGHTf);

        }
        break;

    case _BCM_KT2_COSQ_NODE_LEVEL_L1: /* L1 scheduler */
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve(unit, port, cosq + i,
                                        _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, &index, NULL));
            SOC_IF_ERROR_RETURN
                (READ_LLS_L2_CHILD_WEIGHT_CFG_CNTm(unit, MEM_BLOCK_ALL, 
                                                   index, &l2_weight_cfg));
            weights[i] = soc_mem_field32_get(unit, LLS_L2_CHILD_WEIGHT_CFG_CNTm,
                                             (uint32 *)&l2_weight_cfg, 
                                             C_WEIGHTf);

        }
        break;
    default:
        return BCM_E_INTERNAL;
    }

    if (cur_node->wrr_in_use == 0) {
        *mode = BCM_COSQ_STRICT;   
    } else {
        if (_bcm_is_port_linkphy_subport(unit, port, -1)) {
            BCM_IF_ERROR_RETURN(_bcm_subport_physical_port_get(unit, 
                                            local_port, &physical_port));
            local_port = physical_port;
        }
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
 *     _bcm_kt2_cosq_port_sched_set
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
_bcm_kt2_cosq_port_sched_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             int mode, int num_weights, int *weights)
{
    int i;

    for (i = 0; i < num_weights; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_sched_set(unit, port, (cosq + i), 
                                     mode, 1, &weights[i])); 
                                     
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_port_sched_get
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
_bcm_kt2_cosq_port_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             int *mode, int num_weights, int *weights)
{
    int i;

    for (i = 0; i < num_weights; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_sched_get(unit, port, (cosq + i), 
                                     mode, 1, &weights[i])); 
                                     
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_gport_egress_mapping_set_regular
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
_bcm_kt2_cosq_gport_egress_mapping_set_regular(int unit, bcm_port_t gport, 
                                  bcm_cos_t priority,
                                  bcm_cos_queue_t cosq , int flags)
{
    bcm_port_t port;
    soc_field_t field = REDIRECT_COSf;
    egr_port_entry_t egr_port_entry;
    egr_cos_map_entry_t cos_map_entries[16];
    void *entries[1];     
    uint32 old_index, new_index;
    int rv;
    
    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
    } else {
        port = gport;
    }

    if (cosq < 0 || cosq > 7) {
        return BCM_E_PARAM;
    }
    
    if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
        return BCM_E_PORT;
    }


    entries[0] = &cos_map_entries;

    BCM_IF_ERROR_RETURN
        (READ_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &egr_port_entry));
    old_index = soc_mem_field32_get(unit, EGR_PORTm, &egr_port_entry,
                                    EGR_COS_MAP_SELf);
    old_index *= 16;

    soc_mem_lock(unit, EGR_PORTm);

    rv = soc_profile_mem_get(unit, _bcm_kt2_egr_cos_map_profile[unit],
                             old_index, 16, entries);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, EGR_COS_MAPm, &cos_map_entries[priority], field,
                            cosq);

        rv = soc_profile_mem_delete(unit, _bcm_kt2_egr_cos_map_profile[unit],
                                    old_index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, EGR_PORTm);
            return rv;
        }

        rv = soc_profile_mem_add(unit, _bcm_kt2_egr_cos_map_profile[unit], entries,
                                16, &new_index);
        if (rv == SOC_E_RESOURCE) {
            (void)soc_profile_mem_reference(unit, _bcm_kt2_egr_cos_map_profile[unit],
                                            old_index, 16);
        }
    } else {
        rv = soc_profile_mem_add(unit, _bcm_kt2_egr_cos_map_profile[unit], entries,
                16, &new_index);
    }

    soc_mem_unlock(unit, EGR_PORTm);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_mem_field32_set(unit, EGR_PORTm, &egr_port_entry, EGR_COS_MAP_SELf, 
                        new_index / 16);
    BCM_IF_ERROR_RETURN
        (WRITE_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &egr_port_entry));

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_kt2_cosq_mapping_set_regular
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit           - (IN) unit number
 *     gport          - (IN) queue group GPORT identifier
 *     count          - (IN) number of entries to be configured
 *     priority_array - (IN) priority array value to map
 *     cosq_array     - (IN) COS queue array to map to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_cosq_mapping_set_regular(int unit, bcm_port_t gport,
                                  int count, 
                                  bcm_cos_t *priority_array,
                                  bcm_cos_queue_t *cosq_array)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    bcm_port_t port = 0;
    int numq = 0;
    bcm_cos_queue_t hw_cosq[16];
    soc_field_t field = COSf;
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];     
    uint32 old_index = 0, new_index = 0;
    int count_index= 0;
    int rv = BCM_E_NONE;
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    int cpu_hg_index = 0;
#endif
    
    if (count < 1 || count > 16) {
        return BCM_E_PARAM;
    }

    if (priority_array == NULL || cosq_array == NULL){
        return BCM_E_PARAM;
    }

    for (count_index = 0; count_index < count; count_index++) {
        if (priority_array[count_index] < 0 || priority_array[count_index] >= 16) {
            return BCM_E_PARAM;
        }

        hw_cosq[count_index] = cosq_array[count_index];
    }
 
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        for (count_index = 0; count_index < count; count_index++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve
                 (unit, gport, cosq_array[count_index], _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &port, &(hw_cosq[count_index]), NULL));
        }
    } else if(_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT (unit, gport)) {
        port = _BCM_KT2_SUBPORT_PORT_ID_GET(gport);
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

    mmu_info = _bcm_kt2_mmu_info[unit];
    numq = mmu_info->port[port].q_limit - 
        mmu_info->port[port].q_offset;
   
    for (count_index = 0; count_index < count; count_index++) {
        if (cosq_array[count_index] >= numq) {
            return BCM_E_PARAM;
        }

    }
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        field = (numq <= 8) ? COSf : HG_COSf;
    }

    entries[0] = &cos_map_entries;

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
    old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                    SELECTf);
    old_index *= 16;

    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_get(unit, _bcm_kt2_cos_map_profile[unit],
                             old_index, 16, entries);
    if (BCM_SUCCESS(rv)) {
        for (count_index = 0; count_index < count; count_index++) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority_array[count_index]], field,
                    hw_cosq[count_index]);
        }


        rv = soc_profile_mem_delete(unit, _bcm_kt2_cos_map_profile[unit],
                                    old_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port)) {
            rv = soc_profile_mem_delete(unit, _bcm_kt2_cos_map_profile[unit],
                                        old_index);
        }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_COS_MAPm);
            return rv;
        }

        rv = soc_profile_mem_add(unit, _bcm_kt2_cos_map_profile[unit], entries,
                                16, &new_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port)) {
            rv = soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit],
                                        new_index, 16);
        }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

        if (rv == SOC_E_RESOURCE) {
            (void)soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit],
                                            old_index, 16);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
            if (IS_CPU_PORT(unit, port)) {
                (void)soc_profile_mem_reference(unit,
                                                _bcm_kt2_cos_map_profile[unit],
                                                old_index, 16);
            }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
        }
    } else {
        rv = soc_profile_mem_add(unit, _bcm_kt2_cos_map_profile[unit], entries,
                16, &new_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, port)) {
            rv = soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit],
                    new_index, 16);
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
        cpu_hg_index = SOC_INFO(unit).cpu_hg_pp_port_index;
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
 *     _bcm_kt2_cosq_bucket_set
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
_bcm_kt2_cosq_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 burst_min_quantum, uint32 burst_max_quantum,
                        uint32 flags)
{
    int rv;
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    int index;
    soc_mem_t config_mem[2];
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    lls_s0_shaper_config_c_entry_t s0_entry;
    uint32 rate_exp[2], rate_mantissa[2];
    uint32 burst_exp[2], burst_mantissa[2];
    uint32 cycle_sel[2];
    uint32 burst_min, burst_max;
    int i, idx, status;
    int level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
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
    soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f
    };
    soc_field_t burst_mant_fields[] = {
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
    int shaper_bucket_index = 0;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }


    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) || 
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        level = node->level;    

        if ((level == _BCM_KT2_COSQ_NODE_LEVEL_S0 || 
            level == _BCM_KT2_COSQ_NODE_LEVEL_S1  || 
            level== _BCM_KT2_COSQ_NODE_LEVEL_ROOT) && 
            (min_quantum > 0)) {
            /* min shaper is not supported */
            return BCM_E_PARAM;
        }
    }

    for (i=0; i<2; i++) {
       sal_memset(&rate_mantissa[i], 0, sizeof(rate_mantissa[i]));
       sal_memset(&rate_exp[i], 0, sizeof(rate_exp[i]));
       sal_memset(&burst_mantissa[i] , 0, sizeof(burst_mantissa[i]));
       sal_memset(&burst_exp[i], 0, sizeof(burst_exp[i]));
       sal_memset(&cycle_sel[i], 0, sizeof(cycle_sel[i]));
    }
    if (!(flags & _BCM_XGS_METER_FLAG_RESET)) {

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
        rv = soc_katana_get_shaper_burst_info(unit, burst_max,
                                              &burst_mantissa[0],
                                              &burst_exp[0], flags);
        if (BCM_FAILURE(rv)) {
            return(rv);
        }

        rv = soc_katana_get_shaper_burst_info(unit, burst_min,
                                              &burst_mantissa[1],
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
    }
    
    /* 
     * At this point, port might be in flush state.
     * If so, return.
     */
    SOC_EGRESS_METERING_LOCK(unit);
    soc_kt_port_flush_state_get(unit, local_port, &status);
    if (status) {

        LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                "Unit %d Port %d is in flush state.\n"), unit, local_port));

        SOC_EGRESS_METERING_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    switch (level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;            
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
            rv = soc_mem_write(unit, config_mem[0],
                              MEM_BLOCK_ALL, index, &port_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
            }

            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;            
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
                rv = soc_mem_write(unit, config_mem[i],
                                  MEM_BLOCK_ALL, index, &l0_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
             }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;            
            if (SOC_IS_SABER2(unit)) {
                rate_exp_fields[0] = C_MAX_REF_RATE_EXPf;
                rate_exp_fields[1] = INVALIDf;
                rate_exp_fields[2] = INVALIDf;
                rate_exp_fields[3] = INVALIDf;
                rate_exp_fields[4] = C_MIN_REF_RATE_EXPf;

                rate_mant_fields[0] = C_MAX_REF_RATE_MANTf;
                rate_mant_fields[1] = INVALIDf;
                rate_mant_fields[2] = INVALIDf;
                rate_mant_fields[3] = INVALIDf;
                rate_mant_fields[4] = C_MIN_REF_RATE_MANTf;

                burst_exp_fields[0] = C_MAX_THLD_EXPf;
                burst_exp_fields[1] = INVALIDf;
                burst_exp_fields[2] = INVALIDf;
                burst_exp_fields[3] = INVALIDf;
                burst_exp_fields[4] = C_MIN_THLD_EXPf;

                burst_mant_fields[0] = C_MAX_THLD_MANTf;
                burst_mant_fields[1] = INVALIDf;
                burst_mant_fields[2] = INVALIDf;
                burst_mant_fields[3] = INVALIDf;
                burst_mant_fields[4] = C_MIN_THLD_MANTf;

                cycle_sel_fields[0] = C_MAX_CYCLE_SELf;
                cycle_sel_fields[1] = INVALIDf;
                cycle_sel_fields[2] = INVALIDf;
                cycle_sel_fields[3] = INVALIDf;
                cycle_sel_fields[4] = C_MIN_CYCLE_SELf;

                shaper_bucket_index = 0;
            } else {
                shaper_bucket_index = index % max_l1_shaper_bucket[unit];
            }
            for (i = 0; i < 2; i++) {
                 idx = (i * 4) + shaper_bucket_index;
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index / max_l1_shaper_bucket[unit], &l1_entry);
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
                rv = soc_mem_write(unit, config_mem[i], MEM_BLOCK_ALL, 
                                     index / max_l1_shaper_bucket[unit], &l1_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
            if ((index % max_l2_shaper_bucket[unit]) < 4) {
                 config_mem[0] = l2_shaper_memory[unit][0];
                 config_mem[1] = l2_shaper_memory[unit][1];
            } else {
                 config_mem[0] = l2_shaper_memory[unit][2];
                 config_mem[1] = l2_shaper_memory[unit][3];
            }
            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index/max_l2_shaper_bucket[unit], &l2_entry);
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
                rv = soc_mem_write(unit, config_mem[i], MEM_BLOCK_ALL, 
                                  index/max_l2_shaper_bucket[unit], &l2_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return(rv);
                }
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_S0:
        case _BCM_KT2_COSQ_NODE_LEVEL_S1:
            config_mem[0] = (level == _BCM_KT2_COSQ_NODE_LEVEL_S0 &&
                             (node->linkphy_enabled) && !SOC_IS_METROLITE(unit)) ?
                            LLS_S0_SHAPER_CONFIG_Cm :
                            LLS_S1_SHAPER_CONFIG_Cm;

            if ((level == _BCM_KT2_COSQ_NODE_LEVEL_S1) 
                 && (node->hw_index > 127 && node->hw_index < 256)){
               config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            } else {
#if defined (BCM_SABER2_SUPPORT)
                if (SOC_IS_SABER2(unit)) {
#if defined (BCM_METROLITE_SUPPORT)
                    if (SOC_IS_METROLITE(unit)) {
                        if (level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                            if ((node->hw_index >= _SOC_ML_COSQ_MAX_INDEX_S0) &&
                                node->linkphy_enabled) {
                                SOC_EGRESS_METERING_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                        }
                    } else
#endif /* BCM_METROLITE_SUPPORT */
                    {
                        if (level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                            if ((node->hw_index >= _SOC_SB2_COSQ_MAX_INDEX_S0) &&
                                node->linkphy_enabled) {
                                SOC_EGRESS_METERING_UNLOCK(unit);
                                return BCM_E_PARAM;
                            }
                        }
                    }
                } else
#endif /* BCM_SABER2_SUPPORT */
                if ((level == _BCM_KT2_COSQ_NODE_LEVEL_S0) &&
                    (node->hw_index >= _SOC_KT2_COSQ_MAX_INDEX_S0)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return BCM_E_NONE;
                }
            }

            rv = soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &s0_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
            }
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                rate_exp_f[0], rate_exp[0]);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                rate_mant_f[0], rate_mantissa[0]);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                burst_exp_f[0], burst_exp[0]);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                burst_mant_f[0], burst_mantissa[0]);
            soc_mem_field32_set(unit, config_mem[0], &s0_entry,
                                cycle_sel_f[0], cycle_sel[0]);
            rv = soc_mem_write(unit, config_mem[0],
                               MEM_BLOCK_ALL, index, &s0_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return(rv);
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
 *     _bcm_kt2_cosq_bucket_get
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
_bcm_kt2_cosq_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *burst_min_quantum, uint32 *burst_max_quantum,
                        uint32 flags)
{
    int rv;
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    int index;
    soc_mem_t config_mem[2];
    lls_port_shaper_config_c_entry_t port_entry;
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l2_shaper_config_lower_entry_t l2_entry;
    lls_s0_shaper_config_c_entry_t s0_entry;
    uint32 rate_exp[2], rate_mantissa[2];
    uint32 burst_exp[2], burst_mantissa[2];
    int i, idx, status;
    int level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
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
    soc_field_t rate_exp_fields[] = {
       C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
       C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f,
       C_MIN_REF_RATE_EXP_0f, C_MIN_REF_RATE_EXP_1f,
       C_MIN_REF_RATE_EXP_2f, C_MIN_REF_RATE_EXP_3f
    };
    soc_field_t rate_mant_fields[] = {
       C_MAX_REF_RATE_MANT_0f, C_MAX_REF_RATE_MANT_1f,
       C_MAX_REF_RATE_MANT_2f, C_MAX_REF_RATE_MANT_3f,
       C_MIN_REF_RATE_MANT_0f, C_MIN_REF_RATE_MANT_1f,
       C_MIN_REF_RATE_MANT_2f, C_MIN_REF_RATE_MANT_3f
    };
    soc_field_t burst_exp_fields[] = {
       C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
       C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f,
       C_MIN_THLD_EXP_0f, C_MIN_THLD_EXP_1f,
       C_MIN_THLD_EXP_2f, C_MIN_THLD_EXP_3f       
    };
    soc_field_t burst_mant_fields[] = {
       C_MAX_THLD_MANT_0f, C_MAX_THLD_MANT_1f,
       C_MAX_THLD_MANT_2f, C_MAX_THLD_MANT_3f,
       C_MIN_THLD_MANT_0f, C_MIN_THLD_MANT_1f,
       C_MIN_THLD_MANT_2f, C_MIN_THLD_MANT_3f
    };
    int shaper_bucket_index = 0;

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
        (_bcm_kt2_cosq_index_resolve(unit, port, cosq,
                                    _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) || 
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        level = node->level;    
    } 

    /*
     * At this point, port might be in flush state.
     * If so, the max shaper might have been set to a large
     * value temporarily which must not be returned.
     */
    SOC_EGRESS_METERING_LOCK(unit);
    soc_kt_port_flush_state_get(unit, local_port, &status);
    if (status) {

        LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                "Unit %d Port %d is in flush state.\n"), unit, local_port));

        SOC_EGRESS_METERING_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    switch (level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
            config_mem[0] = LLS_PORT_SHAPER_CONFIG_Cm;
            rv = (soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &port_entry));
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return rv;
            }
            rate_exp[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                rate_exp_f[0]);
            rate_mantissa[0] = soc_mem_field32_get(unit, config_mem[0], 
                                                   &port_entry,
                                                   rate_mant_f[0]);
            rate_exp[1] = rate_exp[0];
            rate_mantissa[1] = rate_mantissa[0];
            burst_exp[0] = soc_mem_field32_get(unit, config_mem[0], &port_entry,
                                burst_exp_f[0]);
            burst_mantissa[0] = soc_mem_field32_get(unit, config_mem[0],
                                                    &port_entry,
                                                    burst_mant_f[0]);
            burst_exp[1] = burst_exp[0];
            burst_mantissa[1] = burst_mantissa[0];
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            config_mem[0] = LLS_L0_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L0_MIN_CONFIG_Cm;

            for (i = 0; i < 2; i++) {
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index, &l0_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return rv;
                }

                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                  &l0_entry,
                                                  rate_exp_f[i]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                       &l0_entry,
                                                       rate_mant_f[i]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                   &l0_entry,
                                                   burst_exp_f[i]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                        &l0_entry,
                                                        burst_mant_f[i]);
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            config_mem[0] = LLS_L1_SHAPER_CONFIG_Cm;
            config_mem[1] = LLS_L1_MIN_CONFIG_Cm;
            if (SOC_IS_SABER2(unit)) {
                rate_exp_fields[0] = C_MAX_REF_RATE_EXPf;
                rate_exp_fields[1] = INVALIDf;
                rate_exp_fields[2] = INVALIDf;
                rate_exp_fields[3] = INVALIDf;
                rate_exp_fields[4] = C_MIN_REF_RATE_EXPf;

                rate_mant_fields[0] = C_MAX_REF_RATE_MANTf;
                rate_mant_fields[1] = INVALIDf;
                rate_mant_fields[2] = INVALIDf;
                rate_mant_fields[3] = INVALIDf;
                rate_mant_fields[4] = C_MIN_REF_RATE_MANTf;

                burst_exp_fields[0] = C_MAX_THLD_EXPf;
                burst_exp_fields[1] = INVALIDf;
                burst_exp_fields[2] = INVALIDf;
                burst_exp_fields[3] = INVALIDf;
                burst_exp_fields[4] = C_MIN_THLD_EXPf;

                burst_mant_fields[0] = C_MAX_THLD_MANTf;
                burst_mant_fields[1] = INVALIDf;
                burst_mant_fields[2] = INVALIDf;
                burst_mant_fields[3] = INVALIDf;
                burst_mant_fields[4] = C_MIN_THLD_MANTf;

                shaper_bucket_index = 0;
            } else {
                shaper_bucket_index = index % max_l1_shaper_bucket[unit];
            }
            for (i = 0; i < 2; i++) {
                idx = (i * 4) + shaper_bucket_index;
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                     index / max_l1_shaper_bucket[unit], &l1_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return rv;
                }
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                  &l1_entry,
                                                  rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                       &l1_entry,
                                                       rate_mant_fields[idx]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                   &l1_entry,
                                                   burst_exp_fields[idx]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                        &l1_entry,
                                                        burst_mant_fields[idx]);
             }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
            if ((index % max_l2_shaper_bucket[unit]) < 4) {
                 config_mem[0] = l2_shaper_memory[unit][0];
                 config_mem[1] = l2_shaper_memory[unit][1];
            } else {
                 config_mem[0] = l2_shaper_memory[unit][2];
                 config_mem[1] = l2_shaper_memory[unit][3];
            }

            for (i = 0; i < 2; i++) {
                idx = (i * 4) + (index % 4);
                rv = soc_mem_read(unit, config_mem[i], MEM_BLOCK_ALL,
                                 index/max_l2_shaper_bucket[unit], &l2_entry);
                if (BCM_FAILURE(rv)) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return rv;
                }
                rate_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                  &l2_entry,
                                                  rate_exp_fields[idx]);
                rate_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                       &l2_entry,
                                                       rate_mant_fields[idx]);
                burst_exp[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                   &l2_entry,
                                                   burst_exp_fields[idx]);
                burst_mantissa[i] = soc_mem_field32_get(unit, config_mem[i], 
                                                        &l2_entry,
                                                        burst_mant_fields[idx]);
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_S0:
        case _BCM_KT2_COSQ_NODE_LEVEL_S1:
#if defined (BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && (level == _BCM_KT2_COSQ_NODE_LEVEL_S0)) {
                if ((node->hw_index >= _SOC_SB2_COSQ_MAX_INDEX_S0) &&
                    node->linkphy_enabled) {
                    SOC_EGRESS_METERING_UNLOCK(unit);
                    return BCM_E_PARAM;
                }
            } else
#endif
            if ((level == _BCM_KT2_COSQ_NODE_LEVEL_S0) &&
                (node->hw_index >= _SOC_KT2_COSQ_MAX_INDEX_S0)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return BCM_E_NONE;
            }
            config_mem[0] = (level == _BCM_KT2_COSQ_NODE_LEVEL_S0 &&
                             (node->linkphy_enabled) && !SOC_IS_METROLITE(unit)) ?
                            LLS_S0_SHAPER_CONFIG_Cm :
                            LLS_S1_SHAPER_CONFIG_Cm;
            rv = soc_mem_read(unit, config_mem[0], MEM_BLOCK_ALL,
                             index, &s0_entry);
            if (BCM_FAILURE(rv)) {
                SOC_EGRESS_METERING_UNLOCK(unit);
                return rv;
            }
            rate_exp[0] = soc_mem_field32_get(unit, config_mem[0], &s0_entry,
                                              rate_exp_f[0]);
            rate_mantissa[0] = soc_mem_field32_get(unit, config_mem[0], 
                                                   &s0_entry,
                                                   rate_mant_f[0]);
            burst_exp[0] = soc_mem_field32_get(unit, config_mem[0], &s0_entry,
                                               burst_exp_f[0]);
            burst_mantissa[0] = soc_mem_field32_get(unit, config_mem[0], 
                                                    &s0_entry,
                                                    burst_mant_f[0]);
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
    if((level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) ||
       (level == _BCM_KT2_COSQ_NODE_LEVEL_S0)  ||
       (level == _BCM_KT2_COSQ_NODE_LEVEL_S1)) 
    {
        *min_quantum=0; 
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

/*
 *  Convert HW drop probability to percent value
 */
STATIC int
_bcm_kt2_hw_drop_prob_to_percent[] = {
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
_bcm_kt2_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= _bcm_kt2_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_kt2_drop_prob_to_percent(int drop_prob) {
   return (_bcm_kt2_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * Function:
 *     _bcm_kt2_cosq_wred_set
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
_bcm_kt2_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
    _bcm_kt2_cosq_node_t *node;
    soc_mem_t wred_mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
    soc_mem_t partner_mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
    mmu_wred_queue_config_buffer_entry_t qentry, entry;
    int rate, i;
    int wred_enabled = 0;
    int ext_mem_port_count = 0;
    int wred_disable_request = 0;
    _bcm_kt2_mmu_info_t *mmu_info;

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
        wred_disable_request = (!ignore_enable_flags) &&
                           (!(flags & BCM_COSQ_DISCARD_ENABLE));
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, port, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_WRED,
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
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) || 
            BCM_GPORT_IS_SCHEDULER(port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                       &node));
                node->wred_disabled = wred_disable_request;
                if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
                    wred_mem = (flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_QENTRYm : 
                                MMU_WRED_OPN_CONFIG_BUFFERm;
                    partner_mem = (flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_BUFFERm : 
                                MMU_WRED_OPN_CONFIG_QENTRYm; 
                }
        } else {
            mmu_info = _bcm_kt2_mmu_info[unit];
            if (cosq >= (mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset)) {
                return BCM_E_PARAM;
            }
            node = &mmu_info->queue_node[mmu_info->port[local_port].q_offset + cosq];
            node->wred_disabled = wred_disable_request;
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
    /*
     * Note that there is issue with dynamic disable of WRED
     * if low rate shaper is configured. There will be huge
     * queue build up and average queue size calculated will
     * be high resulting in WRED entering into tail drop. At
     * this time if WRED gets disabled then re-claculation of
     * average queue size will not be done by WRED and it will
     * remain in tail drop state even if queue is emptied. So
     * instead of diabling WRED attach default WRED profile
     * which have very high min and max threshold and zero
     * dropping rate.
     */
    if (wred_disable_request && (reg != GLOBAL_WRED_CONFIG_QENTRYr)) {
            old_profile_index = soc_mem_field32_get(unit, wred_mem,
                                                    &qentry, PROFILE_INDEXf);
            soc_mem_field32_set(unit, wred_mem, &qentry, PROFILE_INDEXf, 0);
            soc_mem_field32_set(unit, wred_mem, &qentry, WEIGHTf, 0);
    } else
    if (flags & (BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL)) {
        if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
            old_profile_index = soc_reg_field_get(unit, reg, rval, 
                                                  PROFILE_INDEXf);
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
            (soc_profile_mem_get(unit, _bcm_kt2_wred_profile[unit],
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
        rate = _bcm_kt2_percent_to_drop_prob(drop_probability);
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
            (soc_profile_mem_add(unit, _bcm_kt2_wred_profile[unit], entries, 1,
                                 &profile_index));
        if (reg == GLOBAL_WRED_CONFIG_QENTRYr) {
            soc_reg_field_set(unit, reg, &rval, PROFILE_INDEXf, profile_index);
            soc_reg_field_set(unit, reg, &rval, WEIGHTf, gain);
        } else {
            soc_mem_field32_set(unit, wred_mem, &qentry, PROFILE_INDEXf, 
                                profile_index);
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
            if (flags & BCM_COSQ_DISCARD_ENABLE) {
                soc_mem_field32_set(unit, wred_mem, &qentry, field, 1);
            }
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
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, GLOBAL_WRED_CONFIG_BUFFERE_POOL0r,
                                              local_port, index, &rval));
            soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFERE_POOL0r, &rval, field,
                              flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
            soc_reg_field_set(unit, GLOBAL_WRED_CONFIG_BUFFERE_POOL0r, &rval,
                              PROFILE_INDEXf, 0);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, GLOBAL_WRED_CONFIG_BUFFERE_POOL0r,
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
        
        /* if buffer/qentry_mem WRED enabled, make sure the partner_mem 
         * (qentry/buffer_mem) also WRED enabled.
         * if buffer/qentry_mem WRED disabled, make sure the partner_mem
         *  (qentry/buffer_mem) also WRED disabled. 
         */
        if (((flags & BCM_COSQ_DISCARD_ENABLE) && (wred_enabled == 0)) ||
            (!(flags & BCM_COSQ_DISCARD_ENABLE) && (wred_enabled == 1))) {
            partner_index = soc_mem_field32_get(unit, partner_mem,
                                                &qentry, PROFILE_INDEXf); 
            soc_mem_field32_set(unit, partner_mem, &qentry, PROFILE_INDEXf, 0);
            if (flags & BCM_COSQ_DISCARD_ENABLE) {
                soc_mem_field32_set(unit, partner_mem, &qentry, WRED_ENf, 1);
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, partner_mem, MEM_BLOCK_ALL,
                               index, &qentry));    
        }    
    }

/* Replacing old profiles with new global profiles when wred discard 
      profile is set for all ports */

    if ((port== -1) && (flags & BCM_COSQ_DISCARD_DEVICE) &&
                       (profile_index != 0xffffffff)) {
        for(i = 1; i < SOC_REG_NUMELS(unit, reg); i++) {
            BCM_IF_ERROR_RETURN(soc_profile_mem_add(unit, 
                        _bcm_kt2_wred_profile[unit], entries, 1, 
                        &profile_index));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, i, rval));
        }
     }
    
    if (old_profile_index != 0xffffffff && old_profile_index > 0) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_kt2_wred_profile[unit],
                                    old_profile_index));
        if ((port==-1) && (flags & BCM_COSQ_DISCARD_DEVICE)) {
            for (i = 1; i < SOC_REG_NUMELS(unit, reg);  i++) {
                BCM_IF_ERROR_RETURN(soc_profile_mem_delete(unit, 
                            _bcm_kt2_wred_profile[unit],old_profile_index));
            }
        }
    }
    
    if (partner_index != 0xffffffff && partner_index > 0) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_kt2_wred_profile[unit],
                                    partner_index));
    }    

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_wred_get
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
_bcm_kt2_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
    _bcm_kt2_cosq_node_t *node;
    soc_mem_t wred_mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
    mmu_wred_queue_config_buffer_entry_t qentry;
    int rate;
    int wred_disabled = 0;
    _bcm_kt2_mmu_info_t *mmu_info;

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
            (_bcm_kt2_cosq_index_resolve(unit, port, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_WRED,
                                        &local_port, &index, NULL));
        /* WRED doesn't work on CPU/LB Ports */
        if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
        if (*flags & BCM_COSQ_DISCARD_PACKETS) {
            wred_mem = MMU_WRED_QUEUE_CONFIG_QENTRYm;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) || 
            BCM_GPORT_IS_SCHEDULER(port)) {        
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                       &node));
                if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                    wred_mem = (*flags & BCM_COSQ_DISCARD_PACKETS) ? 
                                MMU_WRED_OPN_CONFIG_QENTRYm : 
                                MMU_WRED_OPN_CONFIG_BUFFERm;
                } else {
                    wred_disabled = node->wred_disabled;
                }
        } else {
            mmu_info = _bcm_kt2_mmu_info[unit];
            if (cosq >= (mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset)) {
                return BCM_E_PARAM;
            }
            node = &mmu_info->queue_node[mmu_info->port[local_port].q_offset + cosq];
            wred_disabled = node->wred_disabled;
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
        (soc_profile_mem_get(unit, _bcm_kt2_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_kt2_drop_prob_to_percent(rate);
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
    if ((wred_mem != 0) && (wred_disabled == 0) &&
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
bcm_kt2_handle_queue_min_accounting(int unit,
                                    bcm_port_t port,
                                    int queue_id,
                                    int active)
{
    int shared_size, conf_size, recalc_type;
    _bcm_kt2_mmu_info_t *mmu_info_p;
    mmu_thdo_qconfig_cell_entry_t     mmu_thdo_qconfig_cell_entry={{0}};

    /* Currently it is handled only for Saber2 */
#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit)) {
            return BCM_E_NONE;
        }
#endif
    } else
#endif
    {
        return BCM_E_NONE;
    }
    /* Do run time accounting only for front panel port */
    if ((port <= 0) || (port >= LB_PORT(unit))) {
        return BCM_E_NONE;
    }
    /* Check if queue is classical queue then return */
    mmu_info_p = _bcm_kt2_mmu_info[unit];
    if ((queue_id >= mmu_info_p->port[port].q_offset) &&
        (queue_id < mmu_info_p->port[port].q_limit)) {
        return BCM_E_NONE;
    }
    if ((queue_id >= mmu_info_p->port[port].mcq_offset) &&
        (queue_id < mmu_info_p->port[port].mcq_limit)) {
        return BCM_E_NONE;
    }
    if ((queue_id <= 0) || (queue_id >= soc_mem_index_count(
        unit, MMU_THDO_QCONFIG_CELLm))) {
        return BCM_E_NONE;
    }
    if (active) {
        /* queue is going to be used */
        if (IS_EXT_MEM_PORT(unit, port)) {
            soc_kt2_mmu_get_shared_size(unit, NULL, &shared_size);
            soc_sb2_get_queue_min_size(unit, NULL, &conf_size);
            recalc_type = BCM_KT2_RECALC_SHARED_EXT_DECREMENT;
            if (shared_size < conf_size) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\nGlobal external"
                                                             " shared buffer exhausted")));
                    return BCM_E_RESOURCE;
            }
            BCM_IF_ERROR_RETURN(
                soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                     shared_size - conf_size,
                                                     recalc_type));
        } else {
            soc_kt2_mmu_get_shared_size(unit, &shared_size, NULL);
            soc_sb2_get_queue_min_size(unit, &conf_size, NULL);
            recalc_type = BCM_KT2_RECALC_SHARED_INT_DECREMENT;
            if (shared_size < conf_size) {
                    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "\nGlobal internal"
                                                             " shared buffer exhausted")));
                    return BCM_E_RESOURCE;
            }
            BCM_IF_ERROR_RETURN(
                soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                     shared_size - conf_size,
                                                     recalc_type));
        }
        BCM_IF_ERROR_RETURN(
            READ_MMU_THDO_QCONFIG_CELLm(unit,
                                        MEM_BLOCK_ANY, queue_id,
                                        &mmu_thdo_qconfig_cell_entry));
        soc_mem_field32_set(unit, MMU_THDO_QCONFIG_CELLm,
                            &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf,
                            conf_size);
        BCM_IF_ERROR_RETURN(WRITE_MMU_THDO_QCONFIG_CELLm(unit,
                            MEM_BLOCK_ANY, queue_id,
                            &mmu_thdo_qconfig_cell_entry));
    } else {
        /* queue is going to be freed */
        BCM_IF_ERROR_RETURN(
            READ_MMU_THDO_QCONFIG_CELLm(unit,
                                        MEM_BLOCK_ANY, queue_id,
                                        &mmu_thdo_qconfig_cell_entry));
        conf_size = soc_mem_field32_get(unit, MMU_THDO_QCONFIG_CELLm,
                                        &mmu_thdo_qconfig_cell_entry, Q_MIN_CELLf);
        if (IS_EXT_MEM_PORT(unit, port)) {
            soc_kt2_mmu_get_shared_size(unit, NULL, &shared_size);
            recalc_type = BCM_KT2_RECALC_SHARED_EXT_INCREMENT;
            BCM_IF_ERROR_RETURN(
                soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                     shared_size + conf_size,
                                                     recalc_type));
        } else {
            soc_kt2_mmu_get_shared_size(unit, &shared_size, NULL);
            recalc_type = BCM_KT2_RECALC_SHARED_INT_INCREMENT;
            BCM_IF_ERROR_RETURN(
                soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                     shared_size + conf_size,
                                                     recalc_type));
        }
    }
    return BCM_E_NONE;
}
STATIC int
_bcm_kt2_cosq_gport_delete_node(int unit, bcm_gport_t gport)
{
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info;
    int encap_id;    

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    /* reset the shaper configuration for the
     * node
     */
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT && node->cosq_attached_to >= 0) {
        BCM_IF_ERROR_RETURN (bcm_kt2_cosq_gport_bandwidth_set(unit,
                             node->gport, 0, 0, 0, _BCM_XGS_METER_FLAG_RESET));
    }

    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT && 
            node->cosq_attached_to >= 0) {
        BCM_IF_ERROR_RETURN (bcm_kt2_cosq_gport_detach(unit, node->gport,
                    node->parent->gport, 
                    node->cosq_attached_to));
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        /* scheduler node */
        encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;

        if (encap_id == 0) {
            encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
        }
    } else if (BCM_GPORT_IS_LOCAL(gport)) {    
        encap_id = BCM_GPORT_LOCAL_GET(gport);
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        encap_id = BCM_GPORT_MODPORT_PORT_GET(gport);
    } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        encap_id = BCM_GPORT_SUBPORT_PORT_GET(gport);
    } else {
        return BCM_E_PARAM;
    }

    _bcm_kt2_node_index_clear(&mmu_info->sched_list, encap_id, 1);

    if (node->cosq_map != NULL) {
        sal_free(node->cosq_map);
        node->cosq_map = NULL;
    }

    _BCM_KT2_COSQ_NODE_INIT(node);

    return BCM_E_NONE;
}


STATIC int
_bcm_kt2_cosq_gport_delete_all(int unit, bcm_gport_t gport, int *rv)
{
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_port_info_t *port_info;
    int vlan_queue_present = 1;
    bcm_port_t local_port;
    uint32 rval = 0;
    int encap_id;    
    
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL, &node));

    if (node->child != NULL) {
        _bcm_kt2_cosq_gport_delete_all(unit, node->child->gport, rv);
        BCM_IF_ERROR_RETURN(*rv);
    }

    if (node->sibling != NULL) {
        _bcm_kt2_cosq_gport_delete_all(unit, node->sibling->gport, rv);
        BCM_IF_ERROR_RETURN(*rv);
    }

    /* reset the shaper configuration for the
     * node
     */
    if (node->cosq_attached_to >= 0) {
       BCM_IF_ERROR_RETURN (bcm_kt2_cosq_gport_bandwidth_set(unit,
                            node->gport, 0, 0, 0, _BCM_XGS_METER_FLAG_RESET));
    }

    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT && 
        node->cosq_attached_to >= 0) {

        *rv = bcm_kt2_cosq_gport_detach(unit, node->gport, node->parent->gport, 
                                       node->cosq_attached_to);
        BCM_IF_ERROR_RETURN(*rv);
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) { /* unicast queue group */
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) { 
        /* multicast queue group */
        encap_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport); 
    } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(gport); 
        if (mmu_info->num_dmvoq_queues > 0) {
            _bcm_kt2_node_index_clear(&mmu_info->dmvoq_qlist,
                    encap_id , 1);
            encap_id += mmu_info->base_dmvoq_queue;
        } else {
            _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                    encap_id , 1);
        }
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        /* subscriber queue group */
        encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
        if (mmu_info->num_sub_queues > 0) {
            _bcm_kt2_node_index_clear(&mmu_info->sub_qlist,
                    encap_id , 1);
            encap_id += mmu_info->base_sub_queue;
        } else {

            _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                                encap_id, 1);
        }
        /* Check if all unicast vlan queues of this port are deleted */
        BCM_IF_ERROR_RETURN(
            bcm_kt2_cosq_is_attached(unit, local_port, 1,
                                     &vlan_queue_present));
        port_info = &mmu_info->port[local_port];
        if (!vlan_queue_present) {
            port_info->uc_vlan_base = mmu_info->num_queues - 1;
            SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, local_port, &rval));
            soc_reg_field_set(unit, ING_COS_MODEr, &rval,
                              SERVICE_BASE_QUEUE_NUMf, port_info->uc_vlan_base);
            SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, local_port, rval));
        }
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        /* subscriber queue group */
        encap_id = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
        if (mmu_info->num_sub_queues > 0) {
            _bcm_kt2_node_index_clear(&mmu_info->sub_mcqlist,
                    encap_id , 1);
            encap_id += mmu_info->base_sub_queue;
        } else {

            _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                                encap_id, 1);
        }
    } else {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
           /* scheduler node */
           encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;
        
           if (encap_id == 0) {
               encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
           }
        } else if (BCM_GPORT_IS_LOCAL(gport)) {    
           encap_id = BCM_GPORT_LOCAL_GET(gport);
        } else if (BCM_GPORT_IS_MODPORT(gport)) {
           encap_id = BCM_GPORT_MODPORT_PORT_GET(gport);
        } else if (BCM_GPORT_IS_SUBPORT_PORT(gport)) {
           encap_id = BCM_GPORT_SUBPORT_PORT_GET(gport);
        } else {
            return BCM_E_PARAM;
        }

        _bcm_kt2_node_index_clear(&mmu_info->sched_list, encap_id, 1);
   }

   if (node->cosq_map != NULL) {
       sal_free(node->cosq_map);
       node->cosq_map = NULL;
   }
   if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
       BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
       BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
       BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
       BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
       BCM_IF_ERROR_RETURN(
           bcm_kt2_handle_queue_min_accounting(unit, local_port,
                                               encap_id, 0));
   }
    _BCM_KT2_COSQ_NODE_INIT(node);

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_queue_map_set(int unit, _bcm_kt2_cosq_node_t *node,
                            bcm_port_t port)
{
    _bcm_kt2_cosq_node_t *cur_node, *l1_node;
    egr_queue_to_pp_port_map_entry_t    pp_port_map;
    mmu_thdo_opnconfig_cell_entry_t     opnconfig_cell;
    mmu_thdo_opnconfig_qentry_entry_t   opnconfig_qentry;
    int update_opn = 0;

    switch(node->level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            for (l1_node = node->child; l1_node != NULL;
                 l1_node = l1_node->sibling) {
                 update_opn = 0;
                 for (cur_node = l1_node->child; cur_node != NULL;
                      cur_node = cur_node->sibling) {
                     if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP
                         (cur_node->gport) ||
                         BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP
                         (cur_node->gport) ) {
                         SOC_IF_ERROR_RETURN
                             (READ_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                     cur_node->hw_index, &pp_port_map));
                         soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm,
                                     &pp_port_map, PP_PORTf, port);
                         SOC_IF_ERROR_RETURN
                             (WRITE_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                                         cur_node->hw_index,
                                                         &pp_port_map));
                         update_opn = 1;
                     }
                }
                if (update_opn) {
                    SOC_IF_ERROR_RETURN
                        (READ_MMU_THDO_OPNCONFIG_CELLm(unit, MEM_BLOCK_ANY,
                                    l1_node->hw_index, &opnconfig_cell));
                    soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                        &opnconfig_cell, PIDf, port);
                    SOC_IF_ERROR_RETURN
                        (WRITE_MMU_THDO_OPNCONFIG_CELLm(unit, MEM_BLOCK_ALL,
                                    l1_node->hw_index, &opnconfig_cell));
                    SOC_IF_ERROR_RETURN
                        (READ_MMU_THDO_OPNCONFIG_QENTRYm(unit, MEM_BLOCK_ANY,
                                    l1_node->hw_index, &opnconfig_qentry));
                    soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                    &opnconfig_qentry, PIDf, port);
                    SOC_IF_ERROR_RETURN
                        (WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit, MEM_BLOCK_ALL,
                                    l1_node->hw_index, &opnconfig_qentry));
                }
            }
            break;
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            for (cur_node = node->child; cur_node != NULL;
                 cur_node = cur_node->sibling) {
                 if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(cur_node->gport) ||
                     BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(cur_node->gport)) {
                     SOC_IF_ERROR_RETURN
                        (READ_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                     cur_node->hw_index, &pp_port_map));
                     soc_mem_field32_set(unit, EGR_QUEUE_TO_PP_PORT_MAPm,
                                     &pp_port_map, PP_PORTf, port);
                     SOC_IF_ERROR_RETURN
                        (WRITE_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                                         cur_node->hw_index,
                                                         &pp_port_map));
                     update_opn = 1;
                 }
            }
            if (update_opn) {
                SOC_IF_ERROR_RETURN
                    (READ_MMU_THDO_OPNCONFIG_CELLm(unit, MEM_BLOCK_ANY,
                                    node->hw_index, &opnconfig_cell));
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_CELLm,
                                    &opnconfig_cell, PIDf, port);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_THDO_OPNCONFIG_CELLm(unit, MEM_BLOCK_ALL,
                                    node->hw_index, &opnconfig_cell));
                SOC_IF_ERROR_RETURN
                    (READ_MMU_THDO_OPNCONFIG_QENTRYm(unit, MEM_BLOCK_ANY,
                                    node->hw_index, &opnconfig_qentry));
                soc_mem_field32_set(unit, MMU_THDO_OPNCONFIG_QENTRYm,
                                    &opnconfig_qentry, PIDf, port);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_THDO_OPNCONFIG_QENTRYm(unit, MEM_BLOCK_ALL,
                                    node->hw_index, &opnconfig_qentry));
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_bucket_start(int unit,
           int index, int level, int cycle_sel, int rate_exp, int thld_exp,
           int max_bucket)
{
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l0_shaper_bucket_c_entry_t  l0_bucket;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l1_shaper_bucket_c_entry_t  l1_bucket;
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f
    };
    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
        C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f
    };
    soc_field_t thld_exp_f[] = {
        C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
        C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f
    };
    soc_field_t active_in_lls_f[] = {
        NOT_ACTIVE_IN_LLS_0f, NOT_ACTIVE_IN_LLS_1f,
        NOT_ACTIVE_IN_LLS_2f, NOT_ACTIVE_IN_LLS_3f
    };
    soc_field_t max_bucket_f[] = {
        C_MAX_BUCKET_0f, C_MAX_BUCKET_1f,
        C_MAX_BUCKET_2f, C_MAX_BUCKET_3f
    };
    soc_field_t not_empty_f[] = {
        NOT_EMPTY_0f, NOT_EMPTY_1f,
        NOT_EMPTY_2f, NOT_EMPTY_3f
    };

    switch(level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L0_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                              index, &l0_entry));
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_CYCLE_SELf, cycle_sel);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_REF_RATE_EXPf, rate_exp?rate_exp:0xe);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_THLD_EXPf, thld_exp);
            SOC_IF_ERROR_RETURN
                 (soc_mem_write(unit, LLS_L0_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                                index, &l0_entry));

            sal_memset(&l0_bucket, 0, sizeof(lls_l0_shaper_bucket_c_entry_t));
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                NOT_ACTIVE_IN_LLSf, 1);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                C_MAX_BUCKETf, max_bucket);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                NOT_EMPTYf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L0_SHAPER_BUCKET_Cm,
                               MEM_BLOCK_ALL, index, &l0_bucket));
            break;
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                              (index / 4), &l1_entry));
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                cycle_sel_f[index % 4], cycle_sel);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                rate_exp_f[index % 4], rate_exp);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                thld_exp_f[index % 4], thld_exp);
            SOC_IF_ERROR_RETURN
                 (soc_mem_write(unit, LLS_L1_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                                (index / 4), &l1_entry));

            sal_memset(&l1_bucket, 0, sizeof(lls_l1_shaper_bucket_c_entry_t));
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                active_in_lls_f[index % 4], 1);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                max_bucket_f[index % 4], max_bucket);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                not_empty_f[index % 4], 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L1_SHAPER_BUCKET_Cm,
                               MEM_BLOCK_ALL, (index / 4), &l1_bucket));
            break;
        default:
            return BCM_E_PARAM;
     }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt_cosq_bucket_stop(int unit,
            int index, int level, int *cycle_sel, int *rate_exp, int *thld_exp,
            int *max_bucket)
{
    lls_l0_shaper_config_c_entry_t  l0_entry;
    lls_l0_child_state1_entry_t     l0_state;
    lls_l0_shaper_bucket_c_entry_t  l0_bucket;
    lls_l1_shaper_config_c_entry_t  l1_entry;
    lls_l1_child_state1_entry_t     l1_state;
    lls_l1_shaper_bucket_c_entry_t  l1_bucket;
#if defined(BROADCOM_DEBUG)
    int cycle_sel1, rate_exp1, thld_exp1, max_bucket1;
#endif
    int werr_list, min_list, ef_list, i;
    int count = 0;
    soc_field_t cycle_sel_f[] = {
        C_MAX_CYCLE_SEL_0f, C_MAX_CYCLE_SEL_1f,
        C_MAX_CYCLE_SEL_2f, C_MAX_CYCLE_SEL_3f
    };
    soc_field_t rate_exp_f[] = {
        C_MAX_REF_RATE_EXP_0f, C_MAX_REF_RATE_EXP_1f,
        C_MAX_REF_RATE_EXP_2f, C_MAX_REF_RATE_EXP_3f
    };
    soc_field_t thld_exp_f[] = {
        C_MAX_THLD_EXP_0f, C_MAX_THLD_EXP_1f,
        C_MAX_THLD_EXP_2f, C_MAX_THLD_EXP_3f
    };
    soc_field_t active_in_lls_f[] = {
        NOT_ACTIVE_IN_LLS_0f, NOT_ACTIVE_IN_LLS_1f,
        NOT_ACTIVE_IN_LLS_2f, NOT_ACTIVE_IN_LLS_3f
    };
    soc_field_t max_bucket_f[] = {
        C_MAX_BUCKET_0f, C_MAX_BUCKET_1f,
        C_MAX_BUCKET_2f, C_MAX_BUCKET_3f
    };
    soc_field_t not_empty_f[] = {
        NOT_EMPTY_0f, NOT_EMPTY_1f,
        NOT_EMPTY_2f, NOT_EMPTY_3f
    };

    switch(level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L0_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                              index, &l0_entry));
            *cycle_sel = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                                             &l0_entry, C_MAX_CYCLE_SELf);
            *rate_exp = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                                             &l0_entry, C_MAX_REF_RATE_EXPf);
            *thld_exp = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                                             &l0_entry, C_MAX_THLD_EXPf);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_CYCLE_SELf, 0xF);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_REF_RATE_EXPf, 0xE);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_CONFIG_Cm, &l0_entry,
                                C_MAX_THLD_EXPf, 9);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L0_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                               index, &l0_entry));
            sal_usleep(10);

            sal_memset(&l0_bucket, 0, sizeof(lls_l0_shaper_bucket_c_entry_t));
             SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L0_SHAPER_BUCKET_Cm, MEM_BLOCK_ALL,
                              index, &l0_bucket));
            *max_bucket = soc_mem_field32_get(unit, LLS_L0_SHAPER_BUCKET_Cm,
                                             &l0_bucket, C_MAX_BUCKETf);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                NOT_ACTIVE_IN_LLSf, 0);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                C_MAX_BUCKETf, 0);
            soc_mem_field32_set(unit, LLS_L0_SHAPER_BUCKET_Cm, &l0_bucket,
                                NOT_EMPTYf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L0_SHAPER_BUCKET_Cm,
                               MEM_BLOCK_ALL, index, &l0_bucket));

            sal_usleep(1000000);
            count = 0;

            /* check twice to avoid any transient states */
            for (i = 0; i < 20; i++) {
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L0_CHILD_STATE1m(unit, MEM_BLOCK_ALL,
                                               index, &l0_state));
                werr_list = soc_mem_field32_get(unit, LLS_L0_CHILD_STATE1m,
                                                &l0_state, C_ON_WERR_LISTf);
                min_list = soc_mem_field32_get(unit, LLS_L0_CHILD_STATE1m,
                                               &l0_state, C_ON_MIN_LISTf);
                ef_list = soc_mem_field32_get(unit, LLS_L0_CHILD_STATE1m,
                                              &l0_state, C_ON_EF_LISTf);
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, LLS_L0_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                                  index, &l0_entry));
#if defined(BROADCOM_DEBUG)
                cycle_sel1 = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                        &l0_entry, C_MAX_CYCLE_SELf);
                rate_exp1 = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                        &l0_entry, C_MAX_REF_RATE_EXPf);
                thld_exp1 = soc_mem_field32_get(unit, LLS_L0_SHAPER_CONFIG_Cm,
                        &l0_entry, C_MAX_THLD_EXPf);
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, LLS_L0_SHAPER_BUCKET_Cm, MEM_BLOCK_ALL,
                                  index, &l0_bucket));
                max_bucket1 = soc_mem_field32_get(unit, LLS_L0_SHAPER_BUCKET_Cm,
                        &l0_bucket, C_MAX_BUCKETf);


                LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit,
                                    "Iteration No :%d \n"
                        "LLS_L0_SHAPER_CONFIG_C.C_MAX_CYCLE_SELf = %d \n"
                        "LLS_L0_SHAPER_CONFIG_C.C_MAX_REF_RATE_EXPf = %d \n"
                        "LLS_L0_SHAPER_CONFIG_C.C_MAX_THLD_EXPf = %d \n"
                        "LLS_L0_SHAPER_BUCKET_C.C_MAX_BUCKET =%d \n"
                        "LLS_L0_CHILD_STATE1.C_ON_WERR_LIST= %d \n"
                        "LLS_L0_CHILD_STATE1.C_ON_MIN_LIST= %d \n"
                        "LLS_L0_CHILD_STATE1.C_ON_EF_LISTf = %d \n"),
                        i, cycle_sel1, rate_exp1, thld_exp1,max_bucket1,
                        werr_list, min_list, ef_list));
#endif


                if ((i < 19) && (werr_list || min_list || ef_list)) {
                    /* If any of the above states are set during first read then
                    * wait for 1 second and read again.*/
                    sal_usleep(1000000);
                    count = 0;
                }
                if ((i == 19) && (werr_list || min_list || ef_list)) {
                    /* If any of the above states are set during second read then
                    * return appropriate error */
                    return BCM_E_BUSY;
                }
                if (count < 4) {
                    /* forcefully iterating for four times,
                       so that transient state is ignored */
                    sal_usleep(1000000);
                    count++ ;
                }
                if ( count == 4) {
                    break;
                }
            }
            break;

        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                              (index / 4), &l1_entry));
            *cycle_sel = soc_mem_field32_get(unit, LLS_L1_SHAPER_CONFIG_Cm,
                                             &l1_entry, cycle_sel_f[index % 4]);
            *rate_exp = soc_mem_field32_get(unit, LLS_L1_SHAPER_CONFIG_Cm,
                                             &l1_entry, rate_exp_f[index % 4]);
            *thld_exp = soc_mem_field32_get(unit, LLS_L1_SHAPER_CONFIG_Cm,
                                             &l1_entry, thld_exp_f[index % 4]);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                cycle_sel_f[index % 4], 0xF);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                rate_exp_f[index % 4], 0xE);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_CONFIG_Cm, &l1_entry,
                                thld_exp_f[index % 4], 9);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L1_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                               (index / 4), &l1_entry));
            sal_usleep(10);

            sal_memset(&l1_bucket, 0, sizeof(lls_l1_shaper_bucket_c_entry_t));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, LLS_L1_SHAPER_BUCKET_Cm, MEM_BLOCK_ALL,
                              (index / 4), &l1_bucket));
            *max_bucket = soc_mem_field32_get(unit, LLS_L1_SHAPER_BUCKET_Cm,
                                         &l1_bucket, max_bucket_f[index % 4]);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                active_in_lls_f[index % 4], 0);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                max_bucket_f[index % 4], 0);
            soc_mem_field32_set(unit, LLS_L1_SHAPER_BUCKET_Cm, &l1_bucket,
                                not_empty_f[index % 4], 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, LLS_L1_SHAPER_BUCKET_Cm,
                               MEM_BLOCK_ALL, (index / 4), &l1_bucket));

            sal_usleep(10);

            /* check twice to avoid any transient states */
            for (i = 0; i < 2; i++) {
                SOC_IF_ERROR_RETURN
                    (READ_LLS_L1_CHILD_STATE1m(unit, MEM_BLOCK_ALL,
                                               index, &l1_state));
                werr_list = soc_mem_field32_get(unit, LLS_L1_CHILD_STATE1m,
                                                &l1_state, C_ON_WERR_LISTf);
                min_list = soc_mem_field32_get(unit, LLS_L1_CHILD_STATE1m,
                                               &l1_state, C_ON_MIN_LIST_0f);
                ef_list = soc_mem_field32_get(unit, LLS_L1_CHILD_STATE1m,
                                              &l1_state, C_ON_EF_LIST_0f);
                if ((i == 0) && (werr_list || min_list || ef_list)) {
                    /* If any of the above states are set during first read then
                    * wait for 1 second and read again.*/
                    sal_usleep(1000000);
                }
                if ((i == 1) && (werr_list || min_list || ef_list)) {
                    /* If any of the above states are set during second read then
                    * return appropriate error */
                    return BCM_E_BUSY;
                }
            }
            break;

        default:
            return BCM_E_PARAM;
     }

     return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_child_node_at_input(_bcm_kt2_cosq_node_t *node, int cosq,
                                  _bcm_kt2_cosq_node_t **child_node)
{
    _bcm_kt2_cosq_node_t *cur_node;

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

int 
bcm_kt2_sw_hw_queue(int unit, int *queue_array)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    int index = 0;
    mmu_info = _bcm_kt2_mmu_info[unit];

    for (index = 0; index <  mmu_info->num_queues; index++){
        if( mmu_info->queue_node[index].hw_index == *queue_array){
            *queue_array = index;
            return BCM_E_NONE;
        }  
    }
    *queue_array = -1;
    return BCM_E_PARAM;
}

int
bcm_kt2_is_uc_queue(int unit,int queue_id,int *is_ucq)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_list_t *list; 
   
    mmu_info = _bcm_kt2_mmu_info[unit];
    
    list = &mmu_info->l2_base_qlist;

    if(SHR_BITGET(list->bits, queue_id))
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
 *      bcm_kt2_cosq_subscriber_qid_set
 * Purpose:
 *      Setting a relative queue id as per the configured value  
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 *     queue_id  - (IN) QUEUE identifier 
 * Returns:
 *     BCM_E_XXX
 */

int
bcm_kt2_cosq_subscriber_qid_set(int unit,
    bcm_gport_t *gport,
    int queue_id) 
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node = NULL;
    mmu_info = _bcm_kt2_mmu_info[unit];
    if (mmu_info == NULL) {
        return BCM_E_INIT;
    }

    node = &mmu_info->queue_node[queue_id];
    if (node->cosq_attached_to < 0) {
        return BCM_E_PARAM;
    }
    if (mmu_info->num_sub_queues) {
        queue_id -= mmu_info->base_sub_queue;
    }
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
        BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(*gport, queue_id);
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)) {
        BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(*gport, queue_id);
    } else {
        return BCM_E_INIT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_subscriber_sw_qid_set
 * Purpose:
 *      Set queue id for the subscriber software queue index
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (OUT) GPORT identifier
 *     queue_id  - (IN) QUEUE identifier
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_kt2_subscriber_sw_qid_set(int unit,
                               bcm_gport_t *gport,
                               int queue_id)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    mmu_info = _bcm_kt2_mmu_info[unit];
    if (mmu_info == NULL) {
        return BCM_E_INIT;
    }
    if (mmu_info->num_sub_queues) {
        queue_id -= mmu_info->base_sub_queue;
    }
    BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_SET(*gport, queue_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_intf_ref_cnt_increment
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
_bcm_kt2_intf_ref_cnt_increment(int unit, uint32 queue_id, int count)
{
    _bcm_kt2_mmu_info_t *mmu_info;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
    mmu_info->intf_ref_cnt[queue_id] = mmu_info->intf_ref_cnt[queue_id]
                                       + count;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_intf_ref_cnt_decrement
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
_bcm_kt2_intf_ref_cnt_decrement(int unit, uint32 queue_id, int count)
{
    _bcm_kt2_mmu_info_t *mmu_info;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
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
 *     bcm_kt2_cosq_gport_add
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
bcm_kt2_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_port_info_t *port_info;
    _bcm_kt2_cosq_node_t *node = NULL;
    bcm_module_t local_modid, modid_out;
    bcm_port_t local_port, port_out;
    int id, max_nodes;
    uint32 sched_encap;
    _bcm_kt2_cosq_list_t *list;
    int max_queues;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_gport_add: unit=%d port=0x%x numq=%d flags=0x%x\n"),
              unit, port, numq, flags));

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
    port_info = &mmu_info->port[local_port];
    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }
        
        for (id = port_info->q_offset; id < port_info->q_limit; id++) {
            if (mmu_info->queue_node[id].numq == 0) {
                break;
            }
        }
        
        if (id == port_info->q_limit) {
            return BCM_E_RESOURCE;
        }            

        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        list = &mmu_info->l2_base_qlist;
        _bcm_kt2_node_index_set(&mmu_info->l2_base_qlist, id, 1);
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->hw_index = id;
        break;

    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if ((numq != 1) ||
            (!soc_feature(unit, soc_feature_mmu_packing))) {
            return BCM_E_PARAM;
        }
        for (id = port_info->mcq_offset; id < port_info->mcq_limit; id++) {
            if (mmu_info->queue_node[id].numq == 0) {
                break;
            }
        }
        
        if (id == port_info->mcq_limit) {
            return BCM_E_RESOURCE;
        }            

        BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->hw_index = id;
        break;        

    case BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP:
    case BCM_COSQ_GPORT_SUBSCRIBER:
    case (BCM_COSQ_GPORT_SUBSCRIBER | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP):
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if ((flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) &&
            (!soc_feature(unit, soc_feature_mmu_packing))) {
            return BCM_E_PARAM;
        }

        if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
            /* check for pre allocated subscriber queues */
            if (mmu_info->num_sub_queues == 0) {
                list = &mmu_info->global_qlist;
                max_queues = mmu_info->num_queues;
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_get(unit, list->bits, mmu_info->num_queues/2,
                                             max_queues, mmu_info->qset_size, 1, &id));
                _bcm_kt2_node_index_set(list, id, 1);
                BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(*gport,
                                                                  local_port, id);
            } else {
                list = &mmu_info->sub_mcqlist;
                max_queues = mmu_info->num_sub_queues;
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_get(unit, list->bits, 0, max_queues,
                                             mmu_info->qset_size, 1, &id));
                _bcm_kt2_node_index_set(list, id, 1);
                BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(*gport,
                                                                  local_port, id);
                id += (mmu_info->base_sub_queue + mmu_info->num_queues / 2);
            }
        } else {
            /* check for pre allocated subscriber queues */
            if (mmu_info->num_sub_queues == 0) {
                list = &mmu_info->global_qlist;
                max_queues = (soc_feature(unit, soc_feature_mmu_packing) ?
                              mmu_info->num_queues / 2 : mmu_info->num_queues);
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_get(unit, list->bits, 0, max_queues,
                                             mmu_info->qset_size, 1, &id));
                _bcm_kt2_node_index_set(list, id, 1);
                BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(*gport,
                                                                  local_port, id);
            } else {
                list = &mmu_info->sub_qlist;
                max_queues = mmu_info->num_sub_queues;
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_get(unit, list->bits, 0, max_queues,
                                             mmu_info->qset_size, 1, &id));
                _bcm_kt2_node_index_set(list, id, 1);
                BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(*gport,
                                                                  local_port, id);
                id += mmu_info->base_sub_queue;
            }
        }
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        if (flags & BCM_COSQ_GPORT_SUBSCRIBER) {
            node->hw_index = id;
        }
        if (flags == BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP) {
            node->type = _BCM_KT2_NODE_VLAN_UCAST;
        }
        if (id < port_info->uc_vlan_base) {
            port_info->uc_vlan_base = id;
        }
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
        _bcm_kt2_node_index_set(&mmu_info->sub_qlist, id, 1);
        _bcm_kt2_node_index_set(&mmu_info->l2_sub_qlist, id, 1);

        id += mmu_info->base_sub_queue;
        node->hw_index  = id;
        node->gport = *gport;
        node->numq = numq;
        node->type = _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID;
        node->level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
        if (id < port_info->uc_vlan_base) {
            port_info->uc_vlan_base = id;
        }
        break;

    case (BCM_COSQ_GPORT_WITH_ID |  BCM_COSQ_GPORT_SUBSCRIBER |
          BCM_COSQ_GPORT_MCAST_QUEUE_GROUP):

        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if ((flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) &&
            (!soc_feature(unit, soc_feature_mmu_packing))) {
            return BCM_E_PARAM;
        }

        if (BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(*gport)
              != local_port) {
            return BCM_E_PARAM;
        }

        id = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(*gport);
        if ((id >= mmu_info->num_sub_queues) ||
             (mmu_info->num_sub_queues == 0) ||
             (mmu_info->intf_ref_cnt[id] != 0)) {
            return BCM_E_PARAM;
        }
        max_queues = mmu_info->num_sub_queues;

        node = &mmu_info->queue_node[id + mmu_info->base_sub_queue + mmu_info->num_queues/2];
        if(node->numq != 0) {
            return BCM_E_EXISTS;
        }
        _bcm_kt2_node_index_set(&mmu_info->sub_mcqlist, id, 1);
        _bcm_kt2_node_index_set(&mmu_info->l2_sub_mcqlist, id, 1);

        id += (mmu_info->base_sub_queue + mmu_info->num_queues/2);
        node->hw_index  = id;
        node->gport = *gport;
        node->numq = numq;
        node->type = _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID;
        node->level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
        break;

    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (!IS_HG_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
        if (mmu_info->num_dmvoq_queues == 0) { 
            list = &mmu_info->global_qlist;
            max_queues = mmu_info->num_queues;
        } else {
            list = &mmu_info->dmvoq_qlist;
            max_queues = mmu_info->num_dmvoq_queues;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_kt2_node_index_get(unit, list->bits, 0, max_queues,
                                     mmu_info->qset_size, 1, &id));
        _bcm_kt2_node_index_set(list, id, 1);
        BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        if (mmu_info->num_dmvoq_queues == 1) {
            id  +=  mmu_info->base_dmvoq_queue;  
        }
        node = &mmu_info->queue_node[id];
        node->gport = *gport;
        node->numq = numq;
        node->level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
        node->type = _BCM_KT2_NODE_VOQ;
        break;

    case (BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP):
        if (numq != 1) {
            return BCM_E_PARAM;
        }

        if (!IS_HG_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }

        if (BCM_GPORT_DESTMOD_QUEUE_GROUP_SYSPORTID_GET(*gport) 
                != local_port) {
            return BCM_E_PARAM;
        }    

        id = BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(*gport);
        if ((id >= mmu_info->num_dmvoq_queues) ||
             (mmu_info->num_dmvoq_queues == 0)) {
            return BCM_E_PARAM;
        }
        max_queues = mmu_info->num_dmvoq_queues;
        node = &mmu_info->queue_node[id + mmu_info->base_dmvoq_queue];
        if(node->numq != 0) {
            return BCM_E_EXISTS;
        }
        _bcm_kt2_node_index_set(&mmu_info->dmvoq_qlist, id, 1);
        _bcm_kt2_node_index_set(&mmu_info->l2_dmvoq_qlist, id, 1);
        id += mmu_info->base_dmvoq_queue;
        node->hw_index  = id;  
        node->gport = *gport;
        node->numq = numq;          
        node->level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
        node->type = _BCM_KT2_NODE_VOQ_ID;
        break;
       
    case BCM_COSQ_GPORT_SCHEDULER_WFQ:
    case BCM_COSQ_GPORT_SCHEDULER:
    case (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_WFQ):
        /* passthru */
    case 0:  

        if (numq == 0 || numq < -1) {
            return BCM_E_PARAM;
        }

        if (flags & BCM_COSQ_GPORT_SCHEDULER) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_max_nodes_get(unit, _BCM_KT2_COSQ_NODE_LEVEL_L2,
                                            &max_nodes));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_max_nodes_get(unit, _BCM_KT2_COSQ_NODE_LEVEL_L0,
                                            &max_nodes));
        }
        if (numq > max_nodes) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, local_modid,
                                     local_port, &modid_out, &port_out));

        if ((flags == 0) || (flags == BCM_COSQ_GPORT_SCHEDULER_WFQ)) {
            /* this is a port level scheduler */
            id = port_out;

            if ( id < 0 || id >= num_port_schedulers[unit]) {
                return BCM_E_PARAM;
            }

            _bcm_kt2_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            sched_encap = (id << 8) | port_out;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->gport = *gport;
            node->level = _BCM_KT2_COSQ_NODE_LEVEL_ROOT;
            node->hw_index = id;
            node->numq = numq;
            node->cosq_attached_to = 0;
            if (flags == BCM_COSQ_GPORT_SCHEDULER_WFQ) {
                node->wrr_mode = 1;
                node->type = _BCM_KT2_NODE_SCHEDULER_WFQ;
            }
        } else {
            if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
                id = BCM_GPORT_SUBPORT_PORT_GET(port);
#if defined BCM_METROLITE_SUPPORT
                if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                    /*
                     * Allocate S1 node from extended sched list if pp_port
                     * is reused
                     */
                    if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, id) &&
                        (id <= num_port_schedulers[unit])) {
                        id += SOC_INFO(unit).cpu_hg_pp_port_index;
                    }
                }
#endif
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_get(unit, mmu_info->sched_list.bits,
                                            soc_max_num_pp_ports[unit] + 1,
                                            mmu_info->num_nodes, 1, 1, &id));
            }    
            _bcm_kt2_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            sched_encap = (id << 8) | port_out;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node->gport = *gport;
            node->numq = numq;
            node->cosq_attached_to = -1;
            if (flags & BCM_COSQ_GPORT_SCHEDULER_WFQ) {
                node->wrr_mode = 1;
                node->type = _BCM_KT2_NODE_SCHEDULER_WFQ;
            }
            if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
                node->subport_enabled = 1;
            }
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
            
            /* allow only port or subport level scheduler with id */
            if ( id < 0 || id >= soc_max_num_pp_ports[unit]) {
                return BCM_E_PARAM;
            }

            _bcm_kt2_node_index_set(&mmu_info->sched_list, id, 1);
            node = &mmu_info->sched_node[id];
            node->gport = *gport;
            node->level = _BCM_KT2_COSQ_NODE_LEVEL_ROOT;
            node->hw_index = id;
            node->numq = numq;
            node->cosq_attached_to = 0;
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(*gport)) {
            if (numq != 1) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(*gport) 
                                                   != local_port) {
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
            
            _bcm_kt2_node_index_set(&mmu_info->l2_base_qlist, id, 1);
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
    if ((flags == BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) ||
        (flags == BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) ||
        (flags == BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP) ||
        (flags == BCM_COSQ_GPORT_SUBSCRIBER) ||
        (flags == (BCM_COSQ_GPORT_SUBSCRIBER | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)) ||
        (flags == (BCM_COSQ_GPORT_WITH_ID |  BCM_COSQ_GPORT_SUBSCRIBER)) ||
        (flags == (BCM_COSQ_GPORT_WITH_ID |  BCM_COSQ_GPORT_SUBSCRIBER |
                   BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)) ||
        (flags == BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP) ||
        (flags == (BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP))) {
        BCM_IF_ERROR_RETURN(
            bcm_kt2_handle_queue_min_accounting(unit, local_port,
                                                node->hw_index, 1));
    }
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                       gport=0x%x\n"),
              *gport));

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_kt2_cosq_is_attached
 * Purpose:
 *     To check if any queue is attached to port
 * Parameters:
 *     unit  - (IN) unit number
 *     port  - (IN) Physical port number
 *     exclude_default - (IN) if set skip default queues
 *     vlan_queue_present - (OUT) Set TRUE if queue is attached
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_is_attached(int unit,
                         bcm_port_t port,
                         int exclude_default,
                         int *vlan_queue_present)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_port_info_t *port_info;
    uint32 *bmap = NULL;
    int max_word_count, word, index, bit;

    /* Parameter sanity check */
    if (port < 0) {
        return BCM_E_PORT;
    }
    if (vlan_queue_present == NULL) {
        return BCM_E_PARAM;
    }
    max_word_count = _SHR_BITDCLSIZE(soc_mem_index_count(unit,
                                              LLS_L2_PARENTm));
    bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

    /*
     * Caller of this routine may invoke it with logical
     * port number for coe, linkphy or mpls applications.
     * In these cases bmap will be null. If it is NULL then
     * return BCM_E_NONE without changing vlan_queue_present.
     */
    if (bmap == NULL) {
        return BCM_E_NONE;
    }

    for (word = 0; word < max_word_count; word++) {
         if (bmap[word] == 0) {
             continue;
         }
         for (bit = 0; bit < SHR_BITWID; bit++) {
              if (!(bmap[word] & (1 << bit))) {
                    continue;
              }
              index = word * SHR_BITWID + bit;
              /* check if index is for default queue */
              mmu_info = _bcm_kt2_mmu_info[unit];
              port_info = &mmu_info->port[port];
              if (exclude_default &&
                  ((index >= port_info->q_offset &&
                   index < port_info->q_limit) ||
                  (index >= port_info->mcq_offset &&
                   index < port_info->mcq_limit))) {
                   continue;
              }
              *vlan_queue_present = 1;
              return BCM_E_NONE;
         }
    }
    *vlan_queue_present = 0;
    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_kt2_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_port_info_t *port_info;
    int vlan_queue_present = 1;
    bcm_port_t local_port;
    uint32 rval = 0;
    int encap_id = 0;
    int rv = BCM_E_NONE;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt2_mmu_info[unit];
    
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL, &node));

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        if (node->cosq_attached_to >= 0) {
            /* reset the shaper configuration for the
             * node
             */

            BCM_IF_ERROR_RETURN (bcm_kt2_cosq_gport_bandwidth_set(unit,
                         node->gport, 0, 0, 0, _BCM_XGS_METER_FLAG_RESET));
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_detach(unit, gport, node->parent->gport, 
                                          node->cosq_attached_to));
        }
        if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            if (mmu_info->num_sub_queues > 0) {
                _bcm_kt2_node_index_clear(&mmu_info->sub_qlist,
                        encap_id, 1);
                encap_id += mmu_info->base_sub_queue;
            } else {
                _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                                  encap_id, 1);
            }
            /* Check if all unicast vlan queues of this port are deleted */
            BCM_IF_ERROR_RETURN(
                bcm_kt2_cosq_is_attached(unit, local_port, 1,
                                         &vlan_queue_present));
            port_info = &mmu_info->port[local_port];
            if (!vlan_queue_present) {
                port_info->uc_vlan_base = mmu_info->num_queues - 1;
                SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, local_port, &rval));
                soc_reg_field_set(unit, ING_COS_MODEr, &rval,
                                  SERVICE_BASE_QUEUE_NUMf, port_info->uc_vlan_base);
                SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, local_port, rval));
            }
        }
        if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            if (mmu_info->num_sub_queues > 0) {
                _bcm_kt2_node_index_clear(&mmu_info->sub_mcqlist,
                        encap_id - mmu_info->num_queues/2, 1);
                encap_id += mmu_info->base_sub_queue;
            } else {
                _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                                  encap_id, 1);
            }
        }

        if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(gport);
            if (mmu_info->num_dmvoq_queues > 0) {
                _bcm_kt2_node_index_clear(&mmu_info->dmvoq_qlist,
                        encap_id, 1);
                encap_id += mmu_info->base_dmvoq_queue;
            } else {
                _bcm_kt2_node_index_clear(&mmu_info->global_qlist,
                        encap_id, 1);
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_kt2_handle_queue_min_accounting(unit, local_port,
                                                encap_id, 0));
        _BCM_KT2_COSQ_NODE_INIT(node);
    } else {
        /* 
         *  Deleting the SUB-Tree 
         */
        if (node->child != NULL) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_gport_delete_all(unit, node->child->gport, &rv));
        }
        /* 
         *  Deleting the parent Node 
         */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_gport_delete_node(unit, gport));
    }

    return rv;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_get
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
bcm_kt2_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_kt2_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, &id, &node));

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
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP; 
    } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP; 
    } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_SUBSCRIBER;
        if (node->type == _BCM_KT2_NODE_VLAN_UCAST) {
            *flags = BCM_COSQ_GPORT_VLAN_UCAST_QUEUE_GROUP;
        }
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        *flags = (BCM_COSQ_GPORT_SUBSCRIBER |
                  BCM_COSQ_GPORT_MCAST_QUEUE_GROUP);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        *flags = BCM_COSQ_GPORT_SCHEDULER;
        if (node->type == _BCM_KT2_NODE_SCHEDULER_WFQ) {
            if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
                *flags = BCM_COSQ_GPORT_SCHEDULER_WFQ;
            } else {
                *flags = BCM_COSQ_GPORT_SCHEDULER_WFQ |
                         BCM_COSQ_GPORT_SCHEDULER;
            }
        }
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
 *     bcm_kt2_cosq_gport_traverse
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
bcm_kt2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    _bcm_kt2_cosq_node_t *port_info;
    _bcm_kt2_mmu_info_t *mmu_info;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port, port_out;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt2_mmu_info[unit];

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    my_modid, port, &modid_out, &port_out));
        /* root node */
        port_info = &mmu_info->sched_node[port_out];

        if (port_info->gport >= 0) {
            _bcm_kt2_cosq_gport_traverse(unit, port_info->gport, cb, user_data);
        }
      }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_egress_mapping_set
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
bcm_kt2_cosq_gport_egress_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_t cosq, uint32 flags)
{ 
    bcm_pbmp_t ports;
    bcm_port_t local_port;
    BCM_PBMP_CLEAR(ports);

    if (gport == -1) {    /* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        if ( soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_kt2_flexio_pbmp_update(unit,
                        &ports));
        }
    } else {
        return _bcm_kt2_cosq_gport_egress_mapping_set_regular(unit, gport, priority, cosq, flags);
    }

    PBMP_ITER(ports, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_gport_egress_mapping_set_regular(unit, local_port, priority, 
                                               cosq, flags));

    }    

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt2_cosq_mapping_get
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
bcm_kt2_cosq_gport_egress_mapping_get(int unit, bcm_port_t gport, bcm_cos_t *priority,
                        bcm_cos_queue_t *cosq, uint32 flags)
{
    bcm_port_t port;
    soc_field_t field = REDIRECT_COSf;
    int index;
    egr_port_entry_t egr_port_entry;
    void *entry_p;

    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
    } else {
        port = gport;
    }

    if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &egr_port_entry));
    index = soc_mem_field32_get(unit, EGR_PORTm, &egr_port_entry,
                                EGR_COS_MAP_SELf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_kt2_egr_cos_map_profile[unit],
                                    egr_cos_map_entry_t *,
                                    index + *priority);
    *cosq = soc_mem_field32_get(unit, EGR_COS_MAPm, entry_p, field);


    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_kt2_cosq_mapping_set
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
bcm_kt2_cosq_mapping_set(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_queue_t cosq)
{ 
    bcm_pbmp_t ports;
    bcm_port_t local_port;
    BCM_PBMP_CLEAR(ports);

    if (gport == -1) {    /* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        if ( soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_kt2_flexio_pbmp_update(unit,
                        &ports));
        }
    } else {
        return _bcm_kt2_cosq_mapping_set_regular(unit, gport, 1, &priority, &cosq);
    }

    PBMP_ITER(ports, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_mapping_set_regular(unit, local_port, 1, &priority, 
                                               &cosq));

    }    

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_port_mapping_multi_set
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit           - (IN) unit number
 *     gport          - (IN) queue group GPORT identifier
 *     count          - (IN) number of entries to be configured (or retrieved)
 *     priority_array - (IN) array of priority value to map
 *     cosq_array     - (IN) array of COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_port_mapping_multi_set(int unit, bcm_port_t gport,int count,
                        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array)
{
    bcm_pbmp_t ports;
    bcm_port_t local_port;
    BCM_PBMP_CLEAR(ports);

    if (gport == -1) {    /* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        if ( soc_feature(unit, soc_feature_flex_port)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_kt2_flexio_pbmp_update(unit,
                        &ports));
        }
    } else {
        return _bcm_kt2_cosq_mapping_set_regular(unit, gport, count,
                                            priority_array, cosq_array);
    }

    PBMP_ITER(ports, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_mapping_set_regular(unit, local_port, count,
                                    priority_array, cosq_array));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_mapping_get_regular
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit           - (IN) unit number
 *     gport          - (IN) queue group GPORT identifier
 *     count          - (IN) number of entries to be retrieved
 *     priority_array - (IN) priority array values 
 *     cosq_array     - (OUT) COS queue values mapped to priority 
 * Returns:
 *     BCM_E_XXX
 */

STATIC
int _bcm_kt2_cosq_mapping_get_regular (int unit, bcm_port_t gport,int count,
                    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    bcm_port_t port;
    int numq;
    bcm_cos_queue_t hw_cosq;
    soc_field_t field = COSf;
    int index;
    int count_index = 0;
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entry_p;
    bcm_pbmp_t ports;


    if (count < 1 || count > 16){
        return BCM_E_PARAM;
    }

    if ((NULL == priority_array) || (NULL == cosq_array)){
        return BCM_E_PARAM;
    }

    for (count_index = 0; count_index < count; count_index++) { 
        if (priority_array[count_index] < 0 || priority_array[count_index] >= 16) {
            return BCM_E_PARAM;
        }
    }

    if (gport == -1) {
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            BCM_IF_ERROR_RETURN
                (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, 
                                   &cos_map_sel_entry));
            index = soc_mem_field32_get(unit, COS_MAP_SELm, 
                                        &cos_map_sel_entry,
                                        SELECTf);
            index *= 16;

            entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_kt2_cos_map_profile[unit],
                    port_cos_map_entry_t *,
                    index + priority_array[0]); /*only return for first priority*/
            cosq_array[0] = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, field);
            return BCM_E_NONE;
        }
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve
            (unit, gport, -1, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
             &port, NULL, NULL));
    } else if(_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT (unit, gport)) {
        port = _BCM_KT2_SUBPORT_PORT_ID_GET(gport);
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

    mmu_info = _bcm_kt2_mmu_info[unit];
    numq = mmu_info->port[port].q_limit - 
           mmu_info->port[port].q_offset;
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        field = (numq <= 8) ? COSf : HG_COSf;
    }

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= 16;

    for (count_index = 0; count_index < count; count_index++) {
        entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_kt2_cos_map_profile[unit],
                port_cos_map_entry_t *,
                index + priority_array[count_index]);
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, field);

        if (hw_cosq >= numq) {
            return BCM_E_NOT_FOUND;
        }
        cosq_array[count_index] = hw_cosq;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_port_mapping_multi_get
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit           - (IN) unit number
 *     gport          - (IN) queue group GPORT identifier
 *     count          - (IN) number of entries to be retrieved
 *     priority_array - (IN) priority array values 
 *     cosq_array     - (OUT) COS queue values mapped to priority 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_port_mapping_multi_get(int unit, bcm_port_t gport, int count,
                    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array)
{
    return _bcm_kt2_cosq_mapping_get_regular (unit, gport,count,
                        priority_array, cosq_array);
}

/*
 * Function:
 *     bcm_kt2_cosq_mapping_get
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
bcm_kt2_cosq_mapping_get(int unit, bcm_port_t gport, bcm_cos_t priority,
                        bcm_cos_queue_t *cosq)
{
    return _bcm_kt2_cosq_mapping_get_regular (unit, gport, 1,
                        &priority, cosq);

}

/*
 * Function:
 *     bcm_kt2_cosq_port_sched_set
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
bcm_kt2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    bcm_port_t port;
    int num_cos=0;
    int num_weights;

    bcm_kt2_cosq_config_get(unit, &num_cos);  
    BCM_PBMP_ITER(pbm, port) {
        num_weights = IS_CPU_PORT(unit, port) ? BCM_COS_COUNT : num_cos;
            
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_port_sched_set(unit, port, 0, mode, num_weights,
                                         (int *)weights));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_port_sched_get
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
bcm_kt2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    bcm_port_t port;
    int num_cos=0;
    int num_weights;
    
    
    bcm_kt2_cosq_config_get(unit, &num_cos);  
    BCM_PBMP_ITER(pbm, port) {
        num_weights = IS_CPU_PORT(unit, port) ? BCM_COS_COUNT : num_cos;
        
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_port_sched_get(unit, port, 0, mode, num_weights, 
                                         weights));        
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_sched_weight_max_get
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
bcm_kt2_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
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
            (1 << soc_mem_field_length(unit, LLS_L0_CHILD_WEIGHT_CFG_CNTm, 
                                       C_WEIGHTf)) - 1;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_bandwidth_set
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
bcm_kt2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags)
{
    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    return _bcm_kt2_cosq_bucket_set(unit, port, cosq, min_quantum,
                                   max_quantum, min_quantum, 
                                   burst_quantum, flags);
}

/*
 * Function:
 *     bcm_kt2_cosq_bandwidth_get
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
bcm_kt2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_bucket_get(unit, port, cosq, min_quantum,
                                                max_quantum, burst_quantum,
                                                burst_quantum, *flags));
    *flags = 0;

    return BCM_E_NONE;
}

int
bcm_kt2_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst_min, burst_max;
    lls_port_config_entry_t port_cfg;
    int packet_mode, new_mode;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    packet_mode = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg,
            PACKET_MODE_WRR_ACCOUNTING_ENABLEf);
    if (pps == 0) {
        /* Packet mode shaper is being cleared */
        new_mode = 0;
    } else {
        /* Packet mode shaper is being enabled */
        new_mode = 1;
    }
    if (packet_mode != new_mode) {
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg,
                PACKET_MODE_WRR_ACCOUNTING_ENABLEf, new_mode);
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg,
                PACKET_MODE_SHAPER_ACCOUNTING_ENABLEf, new_mode);
        SOC_IF_ERROR_RETURN
            (WRITE_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                 &burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));

    return _bcm_kt2_cosq_bucket_set(unit, port, cosq, min, pps, burst_min,
                                   burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE);
}

int
bcm_kt2_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst_min, burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                 &burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_kt2_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
        (_bcm_kt2_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst_min,
                             &cur_burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_kt2_cosq_bucket_set(unit, port, cosq, min, max, cur_burst_min,
                               burst, _BCM_XGS_METER_FLAG_PACKET_MODE);
}

int
bcm_kt2_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst_min, cur_burst_max;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst_min,
                              &cur_burst_max, _BCM_XGS_METER_FLAG_PACKET_MODE));
    *burst = cur_burst_max;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_sched_set
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
bcm_kt2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    int rv, numq, i, count;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    count = 0;
    for (i = 0; i < numq; i++) {
        rv = _bcm_kt2_cosq_sched_set(unit, gport, cosq + i, mode, 1, &weight);
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
 *     bcm_kt2_cosq_gport_sched_get
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
bcm_kt2_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int *mode, int *weight)
{
    int rv, numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        rv = _bcm_kt2_cosq_sched_get(unit, gport, cosq + i, mode, 1, weight);
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
 *     bcm_kt2_cosq_gport_child_node_bandwidth_set
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
bcm_kt2_cosq_gport_child_node_bandwidth_set(int unit,
                                bcm_gport_t gport, bcm_cos_queue_t cosq,
                                uint32 kbits_sec_min, uint32 kbits_sec_max,
                                uint32 flags)
{
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    int start_cos;
    int end_cos;
    int numq, i;
    uint32 temp_kbits_sec_min = 0;
    uint32 temp_kbits_sec_max = 0;
    uint32 kbits_sec_burst_min = 0;
    uint32 kbits_sec_burst_max = 0;
    uint32 child_found_flag=0;
    bcm_port_t local_port;
    _bcm_kt2_mmu_info_t *mmu_info;

    cur_gport = gport;

    BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                         &local_port, NULL, &numq));
    if (cosq == -1) {
        start_cos = 0;
        end_cos = numq - 1;

    } else {
        start_cos = end_cos = cosq;
    }

    if ((BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
    }

    if (node) {
        /* For Port gport, L0, L1 scheduler nodes */
        if (cosq >= node->numq) {
            return BCM_E_PARAM;
        }
        child_found_flag = 0;
        for (i = 0, cur_node = node->child; cur_node; 
                    cur_node = cur_node->sibling, i++) {
            
            /* get the child node at cosq */
            if ((cosq != -1) && (cur_node->cosq_attached_to != cosq)) {
                continue;
            }
            child_found_flag = 1;
            cur_gport = cur_node->gport;

            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_bucket_get(unit, cur_gport
                                     ,cur_node->cosq_attached_to,
                                     &temp_kbits_sec_min,
                                     &temp_kbits_sec_max,
                                     &kbits_sec_burst_min,
                                     &kbits_sec_burst_max, 0));
            if ((kbits_sec_burst_min || kbits_sec_burst_max) &&
                (!(flags & BCM_COSQ_BW_BURST_CALCULATE)))
            {
                flags = _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_bucket_set(unit, cur_gport,
                                    cur_node->cosq_attached_to,  kbits_sec_min,
                                    kbits_sec_max, kbits_sec_burst_min,
                                    kbits_sec_burst_max, flags));
            if (flags & BCM_COSQ_BW_BURST_CALCULATE) {
                cur_node->burst_calculation_enable = 1;
            } else {
                cur_node->burst_calculation_enable = 0;
            }
        }
        if (child_found_flag == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        /* For Port and L2 queues */
        for (i = start_cos; i <= end_cos; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_bucket_get(unit, cur_gport, i,
                                     &temp_kbits_sec_min,
                                     &temp_kbits_sec_max,
                                     &kbits_sec_burst_min,
                                     &kbits_sec_burst_max, 0));

            if ((kbits_sec_burst_min || kbits_sec_burst_max) &&
                (!(flags & BCM_COSQ_BW_BURST_CALCULATE)))
            {
                flags = _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_bucket_set(unit, cur_gport,
                                          i, kbits_sec_min,
                                          kbits_sec_max,
                                          kbits_sec_burst_min,
                                          kbits_sec_burst_max, flags));
            mmu_info = _bcm_kt2_mmu_info[unit];
            cur_node = &mmu_info->queue_node[mmu_info->port[
                                             local_port].q_offset + i];
            if (flags & BCM_COSQ_BW_BURST_CALCULATE) {
                cur_node->burst_calculation_enable = 1;
            } else {
                cur_node->burst_calculation_enable = 0;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_child_node_bandwidth_get
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
bcm_kt2_cosq_gport_child_node_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 burst_min, burst_max;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    int i;
    bcm_port_t local_port;
    _bcm_kt2_mmu_info_t *mmu_info;

    i = (cosq == -1) ? 0 : cosq;

    cur_gport = gport;
    if ((BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
       /* get the child node at cosq
        */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
        if (node != NULL) {
            if (cosq >= node->numq) { 
                return BCM_E_PARAM; 
            }
            /* resolve the child attached to input cosq */
            BCM_IF_ERROR_RETURN(
               _bcm_kt2_cosq_child_node_at_input(node, i, &cur_node));
            cur_gport = cur_node->gport;

        } else {
            return BCM_E_PARAM;
        }

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, i,
                                         _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                         &local_port, NULL, NULL));
        mmu_info = _bcm_kt2_mmu_info[unit];
        cur_node = &mmu_info->queue_node[mmu_info->port[
                                         local_port].q_offset + i];
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, cur_gport, i,
                                  kbits_sec_min, kbits_sec_max,
                                  &burst_min, &burst_max, *flags));

    *flags = 0;
    if (cur_node->burst_calculation_enable == 1) {
        *flags |= BCM_COSQ_BW_BURST_CALCULATE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_bandwidth_set
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
bcm_kt2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int numq, i;
    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }
    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                      kbits_sec_max, kbits_sec_min,
                                      kbits_sec_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any COS queue
 *     kbits_sec_min - (OUT) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (OUT) maximum bandwidth, kbits/sec
 *     burst_min - (OUT) minimum burst rate
 *     burst_max - (OUT) maximum burst rate
 *     flags  - (OUT) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *burst_min,
                                uint32 *burst_max, uint32 *flags)
{
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, gport, 
                                  cosq == -1 ? 0 : cosq,
                                  kbits_sec_min, kbits_sec_max,
                                  burst_min, burst_max, *flags));
    
    *flags = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_bandwidth_burst_set
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
bcm_kt2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int numq, i;
    uint32 kbits_sec_min, kbits_sec_max;
    uint32 kbits_sec_burst_min, kbits_sec_burst_max;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    uint8 child_node_found = 0;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    if ((soc_property_get(unit,
        spn_COSQ_CONTROL_BURST_NODE_SELECT, 0)) &&
        (BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
        if (node) {
            /* For Port gport, L0, L1 scheduler nodes */
            if (cosq >= node->numq) {
                return BCM_E_PARAM;
            }
            for (cur_node = node->child; cur_node; cur_node = cur_node->sibling) {
                 /* get the child node at cosq */
                 if ((cosq != -1) && (cur_node->cosq_attached_to != cosq)) {
                     continue;
                 }
                 child_node_found = 1;
                 cur_gport = cur_node->gport;

                 BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_bucket_get(unit,
                                              cur_gport,
                                              cur_node->cosq_attached_to,
                                              &kbits_sec_min,
                                              &kbits_sec_max,
                                              &kbits_sec_burst_min,
                                              &kbits_sec_burst_max,
                                              0));
                 BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_bucket_set(unit,
                                              cur_gport,
                                              cur_node->cosq_attached_to,
                                              kbits_sec_min,
                                              kbits_sec_max,
                                              kbits_burst_min,
                                              kbits_burst_max,
                                              _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION));
                 if (cosq != -1) {
                     break;
                 }
            }
            if (child_node_found == 0) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            /* It should never come here */
            return BCM_E_INTERNAL;
        }
    } else {
        for (i = 0; i < numq; i++) {
             BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_bucket_get(unit, gport, cosq + i, &kbits_sec_min,
                                           &kbits_sec_max, &kbits_sec_burst_min,
                                           &kbits_sec_burst_max, 0));
             BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                           kbits_sec_max, kbits_burst_min,
                                           kbits_burst_max,
                                           _BCM_XGS_METER_FLAG_NON_BURST_CORRECTION));
       }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_bandwidth_burst_get
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
bcm_kt2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *cur_node = NULL;
    bcm_gport_t cur_gport;
    uint8 child_node_found = 0;

    if ((soc_property_get(unit,
        spn_COSQ_CONTROL_BURST_NODE_SELECT, 0)) &&
        (BCM_GPORT_IS_SET(gport)) &&
        (BCM_GPORT_IS_SCHEDULER(gport))) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
        if (node) {

            /* For Port gport, L0, L1 scheduler nodes */
            if (cosq >= node->numq) {
                return BCM_E_PARAM;
            }
            for (cur_node = node->child; cur_node; cur_node = cur_node->sibling) {
                 /* get the child node at cosq */
                 if ((cosq != -1) && (cur_node->cosq_attached_to != cosq)) {
                     continue;
                 }
                 cur_gport = cur_node->gport;
                 child_node_found = 1;
                 BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_bucket_get(unit,
                                              cur_gport,
                                              cur_node->cosq_attached_to,
                                              &kbits_sec_min,
                                              &kbits_sec_max,
                                              kbits_burst_min,
                                              kbits_burst_max,
                                              0));
                 break;
            }
            if (child_node_found == 0) {
                return BCM_E_NOT_FOUND;
            }
        } else {
            /* It should never come here */
            return BCM_E_INTERNAL;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                      &kbits_sec_min, &kbits_sec_max,
                                      kbits_burst_min, kbits_burst_max, 0));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_discard_set
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
bcm_kt2_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    int numq, i;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;
    int local_port;
    soc_info_t    *si= &SOC_INFO(unit);

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100) {
        return BCM_E_PARAM;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }
    cell_size = _BCM_KT2_COSQ_CELLSIZE;
    cell_field_max = KT2_CELL_FIELD_MAX;

    if (gport != -1) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) || 
            BCM_GPORT_IS_SCHEDULER(gport)) {
                BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                        NULL));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_localport_resolve(unit, gport, &local_port));
        }
        if (IS_EXT_MEM_PORT(unit, local_port)) {
            if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
                cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
            } else {
                cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
            }
            cell_field_max = soc_kt2_get_max_buffer_size(unit, 1, cell_field_max);
        } else {
            cell_field_max = soc_kt2_get_max_buffer_size(unit, 0, cell_field_max);
        }
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
        if ((min_thresh > KT2_CELL_FIELD_MAX) ||
            (max_thresh > KT2_CELL_FIELD_MAX)) {
            return BCM_E_PARAM;
        }
    }

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_KT2_COSQ_INDEX_STYLE_WRED,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_wred_set(unit, gport, cosq + i, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_discard_get
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
bcm_kt2_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;
    int local_port;
    soc_info_t    *si= &SOC_INFO(unit);


    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }
    cell_size = _BCM_KT2_COSQ_CELLSIZE;
    cell_field_max = KT2_CELL_FIELD_MAX;

    if (gport != -1) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) || 
            BCM_GPORT_IS_SCHEDULER(gport)) {
                BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                        NULL));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_localport_resolve(unit, gport, &local_port));
        }
        if (IS_EXT_MEM_PORT(unit, local_port)) {
            if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
                cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
            } else {
                cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
            }
            cell_field_max = soc_kt2_get_max_buffer_size(unit, 1, cell_field_max);
        } else {
            cell_field_max = soc_kt2_get_max_buffer_size(unit, 0, cell_field_max);
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain));

    /*
     *  During cosq init we associated same wred profile for all (internal memory and
     *  external memory) port. So need to cap get threshold for external memory port.
     */
    if (min_thresh > cell_field_max) {
        min_thresh = cell_field_max;
    }
    if (max_thresh > cell_field_max) {
        max_thresh = cell_field_max;
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
 *     bcm_kt2_cosq_discard_set
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     flags   - (IN) flags
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_discard_set(int unit, uint32 flags)
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
            (_bcm_kt2_cosq_index_resolve(unit, port, -1,
                                        _BCM_KT2_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_wred_set(unit, port, cosq, flags, 0, 0, 0, 0,
                                       FALSE));
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcm_kt2_cosq_discard_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     flags   - (OUT) flags
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_kt2_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL);
    }

    return BCM_E_NOT_FOUND;

}

/*
 * Function:
 *     bcm_kt2_cosq_discard_port_set
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
bcm_kt2_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time)
{
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_kt2_cosq_discard_port_get
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
bcm_kt2_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    return SOC_E_UNAVAIL;

}
STATIC int
_bcm_kt2_cosq_current_stat_set_get(int unit,
                                   bcm_gport_t gport,
                                   bcm_cos_queue_t cosq,
                                   bcm_cosq_stat_t stat,
                                   int set,
                                   uint64 *value)
{
    bcm_port_t local_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 sw_val, hw_val;
    soc_mem_t mem = INVALIDm;
    soc_field_t field;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int startq, numq, cell_size = _BCM_KT2_COSQ_CELLSIZE;
    int rv = BCM_E_NONE;
    soc_info_t    *si= &SOC_INFO(unit);


    rv = _bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_PORT)) {
         return BCM_E_PARAM;
    }
    if (rv == BCM_E_PORT) {
        if ((gport <= CMIC_PORT(unit)) || (gport >= LB_PORT(unit))) {
            return BCM_E_PARAM;
        } else {
            local_port = gport;
        }
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node));
        }

        startq = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }

    switch (stat) {
    case bcmCosqStatEgressUCQueueBytesCurrent:
        mem = MMU_THDO_QCOUNT_CELLm;
        field = TOTAL_COUNTf;
        break;
    default:
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    if (set) {
        COMPILER_64_TO_32_LO(sw_val, *value);
        hw_val = (sw_val + cell_size - 1) / cell_size;
        soc_mem_field32_set(unit, mem, entry, field, hw_val);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else {
        hw_val = soc_mem_field32_get(unit, mem, entry, field);
        sw_val = hw_val * cell_size;
        COMPILER_64_SET(*value, 0, sw_val);
    }
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    soc_counter_non_dma_id_t dma_id;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *child_node = NULL;
    int first_child = 1; 
    uint64  value64;
    uint64  zero;
#ifdef BCM_SABER2_SUPPORT
    uint32 drop_type, total_drop_mask, wred_drop_mask;
    uint32 rval;
#endif

    if (stat == bcmCosqStatEgressUCQueueBytesCurrent) {
        return _bcm_kt2_cosq_current_stat_set_get(unit, port,
                                                  cosq, stat,
                                                  1, &value);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve
        (unit, port, cosq, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if(SOC_IS_METROLITE(unit)) {
           total_drop_mask = BCM_ML_COSQ_TOTAL_DROP_MASK;
           wred_drop_mask = BCM_ML_COSQ_WRED_DROP_MASK;
        } else 
#endif
        {
           total_drop_mask = BCM_SB2_COSQ_TOTAL_DROP_MASK;
           wred_drop_mask = BCM_SB2_COSQ_WRED_DROP_MASK;
        }  
        SOC_IF_ERROR_RETURN(READ_THDO_DROP_CTR_CONFIGr(unit, &rval));
        drop_type = soc_reg_field_get(unit, THDO_DROP_CTR_CONFIGr, rval, OP_DROP_MASKf); 

        if (drop_type == total_drop_mask) {
            if (!((stat == bcmCosqStatDroppedPackets)||
                  (stat == bcmCosqStatDroppedBytes)||
                  (stat == bcmCosqStatRedCongestionDroppedPackets)||
                  (stat == bcmCosqStatRedCongestionDroppedBytes)||
                  (stat == bcmCosqStatYellowCongestionDroppedPackets)||
                  (stat == bcmCosqStatYellowCongestionDroppedBytes)||
                  (stat == bcmCosqStatOutPackets)||(stat == bcmCosqStatOutBytes))){
                return BCM_E_PARAM;
            }

        }
        else if (drop_type == wred_drop_mask) {
            if (!((stat == bcmCosqStatDiscardDroppedPackets)||
                  (stat == bcmCosqStatDiscardDroppedBytes)||
                  (stat == bcmCosqStatRedDiscardDroppedPackets)||
                  (stat == bcmCosqStatRedDiscardDroppedBytes)||
                  (stat == bcmCosqStatYellowDiscardDroppedPackets)||
                  (stat == bcmCosqStatYellowDiscardDroppedBytes)||
                  (stat == bcmCosqStatGreenDiscardDroppedBytes)||
                  (stat == bcmCosqStatGreenDiscardDroppedPackets)||
                  (stat == bcmCosqStatOutPackets)||(stat == bcmCosqStatOutBytes))){
                return BCM_E_PARAM;
            }
        }
    }
#endif

    switch (stat) {
        case bcmCosqStatDroppedPackets:
        case bcmCosqStatDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
            break;

        case bcmCosqStatDroppedBytes:
        case bcmCosqStatDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
            break;


        case bcmCosqStatRedCongestionDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
            break;

        case bcmCosqStatRedCongestionDroppedBytes:
        case bcmCosqStatRedDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED;
            break;
    
        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatYellowDiscardDroppedPackets :
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
            break;

        case bcmCosqStatYellowCongestionDroppedBytes:
        case bcmCosqStatYellowDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_YELLOW;
            break;

        case bcmCosqStatGreenDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN;
            break;

        case bcmCosqStatGreenDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_GREEN;
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
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
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
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port, dma_id, i, i == 0 ? value : zero ));
            }
        }
    } else {
         
      BCM_IF_ERROR_RETURN
       (_bcm_kt2_cosq_node_get(unit, port, 0, 
                  &local_port, NULL, &node));
       if (cosq == -1) {  
            for (i = 0; i < node->numq; i++) {
                 if ((_bcm_kt2_cosq_child_node_at_input(node, i, &child_node))
                                                        == BCM_E_NOT_FOUND) {
                       continue;
                 }
                              
                 port = child_node->gport;
                         
                 BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_index_resolve
                 (unit, port, 0, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &local_port, &startq, &numq));

                 if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                     BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                     BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port)) {
                    /* pass port as queue number */
                    local_port = startq;
                    startq = 0;
                 }
                 if(first_child) { 
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

            if ((_bcm_kt2_cosq_child_node_at_input(node, cosq, &child_node))
                                                     == BCM_E_NOT_FOUND) {
                 return BCM_E_NOT_FOUND; 
            }
                             
            port = child_node->gport;
                       
            BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve
              (unit, port, 0, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
               &local_port, &startq, &numq));

            if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port)) {
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
_bcm_kt2_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
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
        *type = _BCM_KT2_NODE_UCAST;
        break;
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass0McastQueue:
        *type = _BCM_KT2_NODE_MCAST;
        break;
/*    case bcmSwitchPFCClass7DestmodQueue:
    case bcmSwitchPFCClass6DestmodQueue:
    case bcmSwitchPFCClass5DestmodQueue:
    case bcmSwitchPFCClass4DestmodQueue:
    case bcmSwitchPFCClass3DestmodQueue:
    case bcmSwitchPFCClass2DestmodQueue:
    case bcmSwitchPFCClass1DestmodQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *type = _BCM_KT2_COSQ_TYPE_EXT_UCAST;
        break;
*/
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
#ifdef BCM_SABER2_SUPPORT
/*
 * Function:
 *    _bcm_kt2_port_pfc_supported
 * Purpose:
 *     To check if given port support 8 PGs or not.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 * Returns:
 *     1 if port support 8 PGs else 0
 */
STATIC int
_bcm_kt2_port_pfc_supported(int unit, bcm_port_t local_port)
{

#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit) && (local_port < ML_MAX_LOGICAL_PORTS))  {
       return ml_pfc_support[local_port].supported ;
    } else
#endif
    if(SOC_IS_SABER2(unit) && (local_port < SB2_MAX_LOGICAL_PORTS))  {
       return sb2_pfc_support[local_port].supported ;
    }
    return BCM_E_NONE;
}
STATIC int
bcm_kt2_pg_profile_refcount_init(int unit)
{
    uint64 rval64;
    int    i;
    uint32 low, high, temp;

    pg_profile_refcount[unit][0] = 0;
    pg_profile_refcount[unit][1] = 0;
    pg_profile_refcount[unit][2] = 0;
    pg_profile_refcount[unit][3] = 0;
    if(SOC_IS_SABER2(unit))  {
       SOC_IF_ERROR_RETURN(
           READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, &rval64));
       low = COMPILER_64_LO(rval64);
       high = COMPILER_64_HI(rval64);
       for(i = 0; i < 40; i = i + 2) {
           if (i < 32) {
               temp = 0x3 & (low >> i);
           } else {
               temp = 0x3 & (high >> (i -32));
           }
           pg_profile_refcount[unit][temp] = pg_profile_refcount[unit][temp] + 1;
       }
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     _bcm_kt2_fc_status_map_gport
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
_bcm_kt2_fc_status_map_gport(int unit, int port, 
                bcm_gport_t gport, _bcm_kt2_fc_type_t fct, 
                uint32 *cos_bmp)
{
    int hw_index, hw_index1;
    int base_index = 0;
    int hw_cosq, hw_cosq1;
    int id, lvl;
    bcm_port_t local_port, resolved_port;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_cosq_port_info_t *port_info;
    _bcm_kt2_mmu_info_t *mmu_info;
    int supported = 0 ;

    
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit))  {
       supported = ml_pfc_support[local_port].supported ;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit))  {
       supported = sb2_pfc_support[local_port].supported ; 
    } else 
#endif 
    {
       supported = kt2_pfc_support[local_port].supported ; 
    }
    if (!supported) { 
        return BCM_E_PARAM;  
    }
    mmu_info = _bcm_kt2_mmu_info[unit];
    port_info = &mmu_info->port[local_port];
    
    hw_index = hw_index1 = -1;
    hw_cosq = hw_cosq1 = -1;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_node_get(unit, gport, 
                            0,  &resolved_port, &id, &node));
    /* hardware supports pfc at l0 and l1 level but  the index needs to be 
     * assigned in multiple of 8 for pfc to work.Since we are not doing it 
     * returning E_PARAM for scheduler node as of now can be corrected if 
     * index allocation is changed .Rest of the code programmed to handle
     * schedulers  
     */ 
        if (!(BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)
            )) {
            return BCM_E_PARAM; 
        }
        hw_index = node->hw_index;
        if (!IS_HG_PORT(unit,local_port)) {
            hw_cosq =  node->hw_index % NUM_COS(unit);
        }
        else { 
                hw_cosq =  node->hw_index - port_info->q_offset ;
        }

        lvl = node->level;
    } else {
            hw_cosq = gport;
            node = &mmu_info->queue_node[port_info->q_offset + hw_cosq];
            if ((node->cosq_attached_to == -1)) {
                return BCM_E_PARAM;
            }
            hw_index = node->hw_index;
        if (soc_feature(unit, soc_feature_mmu_packing)) {
            node = &mmu_info->queue_node[port_info->mcq_offset + hw_cosq];
            if (!(node->cosq_attached_to == -1)) {
                hw_index1 = node->hw_index;
                hw_cosq1 = hw_cosq ;
            }
        }
        lvl = _BCM_KT2_COSQ_NODE_LEVEL_L2;
    }
    if ((hw_cosq < 0) || (hw_cosq >= _BCM_KT2_NUM_INTERNAL_PRI)) {
        return BCM_E_PARAM;
    }
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit))  {
       base_index = ml_pfc_support[local_port].index1 + hw_cosq / 8 ;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit))  {
       base_index = sb2_pfc_support[local_port].index1 + hw_cosq / 8 ; 
    } else 
#endif 
    {
       base_index = kt2_pfc_support[local_port].index1 + hw_cosq / 8 ; 
    }
    BCM_IF_ERROR_RETURN(
         _bcm_kt2_cosq_map_fc_status_to_node(unit, fct,
                                        hw_index, lvl - 2, base_index ));
    if (cos_bmp != NULL) {
        *cos_bmp |= 1 << hw_cosq;
    }
    if (hw_cosq1 >= 0) {
        BCM_IF_ERROR_RETURN(
            _bcm_kt2_cosq_map_fc_status_to_node(unit, fct,
                                           hw_index1, lvl - 2, base_index));
        if (cos_bmp != NULL) {
            *cos_bmp |= 1 << hw_cosq1;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_port_pfc_profile_set
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
_bcm_kt2_port_pfc_profile_set(int unit, int port, _bcm_cosq_op_t op,
                              int entry_cnt, int *pfc_class, uint32 *cos_bmp)
{
    soc_reg_t   reg;
    bcm_port_t local_port;
    int rv, entry, cur_class;
    int mmu_port;
    uint32 profile_index, old_profile_index;
    uint32 fval, tmp32, cur_bmp;
    uint64 rval64[16], tmp, *rval64s[1], rval, index_map;
#ifdef BCM_METROLITE_SUPPORT
    uint32 rval32, index_map32;
#endif
    if (pfc_class == NULL) {
        return BCM_E_PARAM;
    }
    if (cos_bmp == NULL) {
        return BCM_E_PARAM;
    }
    if ((entry_cnt < 0 ) ) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN(
        _bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       mmu_port = ml_pfc_support[local_port].mmu_port;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT 
    if(SOC_IS_SABER2(unit)) { 
       mmu_port = sb2_pfc_support[local_port].mmu_port;
    } else 
#endif 
   {
       mmu_port = kt2_pfc_support[local_port].mmu_port;
   }
   if (mmu_port == -1) { 
       return BCM_E_PARAM;
   } 
    reg = PORT_PFC_CFG0r;
    rval64s[0] = rval64;
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval32));
       index_map32 = soc_reg_field_get(unit, reg, rval32, PROFILE_INDEXf);
       index_map32 = index_map32 >> (mmu_port * 2);
       old_profile_index = (index_map32 & 3) * 16;
    } else
#endif
    { 
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));
    
    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    old_profile_index = (tmp32 & 3)*16;
    }
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_kt2_llfc_profile[unit],
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
        (soc_profile_reg_delete(unit, _bcm_kt2_llfc_profile[unit],
                                old_profile_index));

    rv = soc_profile_reg_add(unit, _bcm_kt2_llfc_profile[unit], rval64s,
                             16, &profile_index);
    if (SOC_FAILURE(rv)) {
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, _bcm_kt2_llfc_profile[unit],
                                       old_profile_index, 16));
        return rv;
    }

#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       index_map32 = soc_reg_field_get(unit, reg, rval32, PROFILE_INDEXf);
       tmp32 = 3 << (mmu_port * 2);
       tmp32 = ~tmp32;
       index_map32 = index_map32 & tmp32;
       tmp32 = (profile_index / 16) << (mmu_port * 2);
       index_map32 = index_map32 | tmp32;
       soc_reg_field_set(unit, reg, &rval32, PROFILE_INDEXf,
                        index_map32);
       BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, 0, 0, rval32));
    } else
#endif
    {
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
    }
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count)
{
    int type, class = -1, index;
    uint32 cos_bmp;
    _bcm_kt2_fc_type_t fct;

    if (gport_count < 0) {
        return BCM_E_PARAM;
    }
    if (!SOC_IS_SABER2(unit) && !IS_XE_PORT(unit,port)) {
        return BCM_E_PARAM; 
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_pfc_class_resolve(sctype, &type, &class));

    cos_bmp = 0;
    fct = (op == _BCM_COSQ_OP_CLEAR)? 0 : _BCM_KT2_COSQ_FC_PFC;
    for (index = 0; index < gport_count; index++) {
        BCM_IF_ERROR_RETURN(
            _bcm_kt2_fc_status_map_gport(unit, port, gport[index],
                                         fct, &cos_bmp));
    }

    if (op == _BCM_COSQ_OP_CLEAR) {
        cos_bmp = (1 << _BCM_KT2_NUM_INTERNAL_PRI) - 1;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_port_pfc_profile_set(unit, port, op,
                                                      1, &class, &cos_bmp));
    
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    int type = -1, class = -1, hw_cosq, count = 0;
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1], rval, index_map;
    uint32 tmp32, bmp;
    _bcm_kt2_mmu_info_t *mmu_info;
    int  mmu_port;
    soc_reg_t   reg;
    int node_port = 0 ;
    _bcm_kt2_cosq_port_info_t *port_info;
    static const soc_field_t self[] = {
        SEL0f, SEL1f, 
    };
    int j, hw_index, eindex;
    soc_mem_t mem;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
#ifdef BCM_METROLITE_SUPPORT
    uint32 rval32, index_map32;
#endif
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
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));

    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_pfc_class_resolve(sctype, &type, &class));

    mmu_info = _bcm_kt2_mmu_info[unit];
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       mmu_port = ml_pfc_support[local_port].mmu_port;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT 
    if(SOC_IS_SABER2(unit)) { 
       mmu_port = sb2_pfc_support[local_port].mmu_port;
    } else 
#endif 
   {
    mmu_port = kt2_pfc_support[local_port].mmu_port;
   }
    if (mmu_port == -1) { 
        return BCM_E_PARAM; 
    }
    port_info = &mmu_info->port[local_port];

    rval64s[0] = rval64;
    reg = PORT_PFC_CFG0r;
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval32));
       index_map32 = soc_reg_field_get(unit, reg, rval32, PROFILE_INDEXf); 
       index_map32 = index_map32 >> (mmu_port * 2);
       profile_index = (index_map32 & 3) * 16;
    } else
#endif
    {
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));

    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    profile_index = (tmp32 & 3)*16;
    }
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_kt2_llfc_profile[unit],
                             profile_index, 16, rval64s));

    bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[class],
                                COS_BMPf);
     
    for (hw_cosq = 0; hw_cosq < 8; hw_cosq++) {
        if (!(bmp & (1 << hw_cosq))) {
            continue;
        }
            for (j = num_pp_ports[unit] ;
                 j < num_total_schedulers[unit]; j++) {
                node = &mmu_info->sched_node[j];
                if (node->cosq_attached_to == -1) { 
                    continue ;
                } 
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_node_get(unit, node->gport, NULL 
                     ,&node_port, NULL, NULL));
                if ( (node_port != local_port) ||
                    ((node->hw_index % NUM_COS(unit)) != hw_cosq)) {
                    continue;
                }

                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16)/8;
                if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
                    mem = MMU_INTFI_FC_MAP_TBL0m ; 
                } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                    mem = MMU_INTFI_FC_MAP_TBL1m; 
                } else {
                    continue;
                }

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0 && count < gport_count) {
                        gport[count] = node->gport;
                    }
                    count++;
                    break;
                }
            }
            for (j = port_info->q_offset; 
                 (j < port_info->q_limit); j++) {
                node = &mmu_info->queue_node[j];
                if (node->cosq_attached_to == -1) { 
                    continue ;
                } 
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_node_get(unit, node->gport, NULL ,&node_port, 
                               NULL, NULL));
                if ((node_port != local_port)) {
                    continue;
                }
                if ((node->hw_index % NUM_COS(unit)) != hw_cosq) {
                    continue;
                }
                mem = MMU_INTFI_FC_MAP_TBL2m ;
                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16) / 8; 

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                      
                    if (gport_count > 0 && count < gport_count) {
                         
                        gport[count] = node->gport;
                    }
                    count++;
                    break;
                }
            }
            if(soc_feature(unit, soc_feature_mmu_packing)) {   
               for (j = port_info->mcq_offset; 
                    (j < port_info->mcq_limit); j++) {
                    node = &mmu_info->queue_node[j];
                    if (node->cosq_attached_to == -1) { 
                        continue ;
                    } 
                    BCM_IF_ERROR_RETURN
                     (_bcm_kt2_cosq_node_get(unit, node->gport, NULL ,&node_port, 
                                   NULL, NULL));
                    if ((node_port != local_port)) {
                        continue;
                    }
                    if ((node->hw_index % NUM_COS(unit)) != hw_cosq) {
                        continue;
                    }
                    mem = MMU_INTFI_FC_MAP_TBL2m ;
                    hw_index = (node->hw_index / 16);
                    eindex = (node->hw_index % 16) / 8; 

                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                            MEM_BLOCK_ALL, hw_index, &map_entry));

                    if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                        if (gport_count > 0 && count < gport_count ) {
                            gport[count] = node->gport;
                    }
                    count++;
                    break;
                    }
               }
            }
            if (mmu_info->num_sub_queues) {
                for (j=mmu_info->base_sub_queue; 
                        j < (mmu_info->base_sub_queue+mmu_info->num_sub_queues); j++) {

                    node = &mmu_info->queue_node[j];
                    if (node->cosq_attached_to == -1) {
                        continue;
                    }
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_cosq_node_get(unit, node->gport, NULL ,&node_port,
                                                NULL, NULL));
                    if (node_port != local_port) {
                        continue;
                    }
                    if ((node->hw_index % NUM_COS(unit)) != hw_cosq) {
                        continue;
                    }
                    mem = MMU_INTFI_FC_MAP_TBL2m ;
                    hw_index = (node->hw_index / 16);
                    eindex = (node->hw_index % 16) / 8;  

                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem,  
                                MEM_BLOCK_ALL, hw_index, &map_entry));

                    if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                        if (gport_count > 0 && count < gport_count ) {
                            gport[count] = node->gport;
                        }
                        count++;
                        break;
                    }


                } 

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
 *     bcm_kt2_cosq_pfc_class_mapping_set
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
bcm_kt2_cosq_safc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_safc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0, pfc_class[_BCM_KT2_NUM_SAFC_CLASS];
    bcm_gport_t cur_gport = 0;
    int local_port = 0;
    uint32 cos_bmp[_BCM_KT2_NUM_SAFC_CLASS];

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
    /*safc is supported only on higig interfaces */ 
    if (!IS_HG_PORT(unit, local_port)) {
         return BCM_E_PARAM;  
    }
    if ((array_count < 0) || (array_count > _BCM_KT2_NUM_SAFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_KT2_NUM_SAFC_CLASS)) {
            return BCM_E_PARAM;
        } else {
            cos_bmp[cur_class] = 0;
        }
        
        for (index = 0; index < BCM_COSQ_SAFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            
            BCM_IF_ERROR_RETURN(_bcm_kt2_fc_status_map_gport(unit, port, 
                cur_gport, _BCM_KT2_COSQ_FC_SAFC, &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_KT2_NUM_SAFC_CLASS; index++) {
         pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_port_pfc_profile_set(unit, port,
        _BCM_COSQ_OP_SET, _BCM_KT2_NUM_SAFC_CLASS, pfc_class, cos_bmp));
    
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_port_safc_get(int unit, bcm_port_t port,
                        int cur_class,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    int hw_cosq, count = 0;
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1], rval, index_map;
    uint32 tmp32, bmp;
    _bcm_kt2_mmu_info_t *mmu_info;
    int  mmu_port;
    soc_info_t *si;
    soc_reg_t   reg;
    int node_port = 0 ;
    int supported = 0 ;
    _bcm_kt2_cosq_port_info_t *port_info;
    static const soc_reg_t llfc_cfgr[] = {
        PORT_PFC_CFG0r,
    };
    static const soc_field_t self[] = {
        SEL0f, SEL1f, 
    };
    int j, hw_index, eindex;
    soc_mem_t mem;
    uint32 map_entry[SOC_MAX_MEM_WORDS];

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
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
     
    si = &SOC_INFO(unit);
    mmu_info = _bcm_kt2_mmu_info[unit];
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       mmu_port = ml_pfc_support[local_port].mmu_port;
       supported = ml_pfc_support[local_port].supported;
    } else
#endif
#ifdef BCM_SABER2_SUPPORT 
    if(SOC_IS_SABER2(unit)) { 
       mmu_port = sb2_pfc_support[local_port].mmu_port;
       supported = sb2_pfc_support[local_port].supported;
    } else 
#endif 
   {
    mmu_port = kt2_pfc_support[local_port].mmu_port;
    supported = kt2_pfc_support[local_port].supported; 
   }
   if (!supported) {
       return BCM_E_PARAM; 

   }
    port_info = &mmu_info->port[local_port];

    rval64s[0] = rval64;
    reg = llfc_cfgr[mmu_port/32];
    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval));

    index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
    COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
    COMPILER_64_TO_32_LO(tmp32, index_map);
    profile_index = (tmp32 & 3)*16;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_kt2_llfc_profile[unit],
                             profile_index, 16, rval64s));

    bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILEr, rval64[cur_class],
                                COS_BMPf);
     
    num_port_schedulers[unit] = si->cpu_hg_index;

    for (hw_cosq = 0; hw_cosq < 8; hw_cosq++) {
        if (!(bmp & (1 << hw_cosq))) {
            continue;
        }
            for (j = num_pp_ports[unit] ;
                 j < num_total_schedulers[unit] ; j++) {
                node = &mmu_info->sched_node[j];
                if (node->cosq_attached_to == -1) { 
                    continue ;
                } 
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_node_get(unit, node->gport, NULL 
                     ,&node_port, NULL, NULL));
                if ( (node_port != local_port) ||
                    ((node->hw_index - port_info->q_offset) != hw_cosq)) {
                    continue;
                }

                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16)/8;
                if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
                    mem = MMU_INTFI_FC_MAP_TBL0m ; 
                } else if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                    mem = MMU_INTFI_FC_MAP_TBL1m; 
                } else {
                    continue;
                }

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                    if (gport_count > 0 && count < gport_count) {
                        gport[count] = node->gport;
                    }
                    count++;
                    break;
                }
            }
            for (j = port_info->q_offset; 
                 (j < port_info->q_limit); j++) {
                node = &mmu_info->queue_node[j];
                if (node->cosq_attached_to == -1) { 
                    continue ;
                } 
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_node_get(unit, node->gport, NULL ,&node_port, 
                               NULL, NULL));
                if ((node_port != local_port)) {
                    continue;
                }
                if ((node->hw_index - port_info->q_offset) != hw_cosq) {
                    continue;
                }
                mem = MMU_INTFI_FC_MAP_TBL2m ;
                hw_index = (node->hw_index / 16);
                eindex = (node->hw_index % 16) / 8; 

                BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                        MEM_BLOCK_ALL, hw_index, &map_entry));

                if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                      
                    if (gport_count > 0 && count < gport_count) {
                         
                        gport[count] = node->gport;
                    }
                    count++;
                    break;
                }
            }
            if(soc_feature(unit, soc_feature_mmu_packing)) {   
               for (j = port_info->mcq_offset; 
                    (j < port_info->mcq_limit); j++) {
                    node = &mmu_info->queue_node[j];
                    if (node->cosq_attached_to == -1) { 
                        continue ;
                    } 
                    BCM_IF_ERROR_RETURN
                     (_bcm_kt2_cosq_node_get(unit, node->gport, NULL ,&node_port, 
                                   NULL, NULL));
                    if ((node_port != local_port)) {
                        continue;
                    }
                    if ((node->hw_index % NUM_COS(unit)) != hw_cosq) {
                        continue;
                    }
                    mem = MMU_INTFI_FC_MAP_TBL2m ;
                    hw_index = (node->hw_index / 16);
                    eindex = (node->hw_index % 16) / 8; 

                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, 
                            MEM_BLOCK_ALL, hw_index, &map_entry));

                    if (soc_mem_field32_get(unit, mem, &map_entry, self[eindex])) {
                        if (gport_count > 0 && count < gport_count ) {
                            gport[count] = node->gport;
                    }
                    count++;
                    break;
                    }
               }
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

                    
int
bcm_kt2_cosq_safc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    int *array_count,
    bcm_cosq_safc_class_mapping_t *mapping_array)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    int local_port = 0 ;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
    if (!IS_HG_PORT(unit,local_port)) {
        return BCM_E_PARAM; 
    }
    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }
    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    for (cur_class = 0; cur_class < _BCM_KT2_NUM_SAFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        if (array_max > 0) {
            rv = bcm_kt2_cosq_port_safc_get(unit, local_port, cur_class, 
                        mapping_array[class_count].gport_list,
                        BCM_COSQ_PFC_GPORT_COUNT, &actual_gport_count);
        } else {
            rv = bcm_kt2_cosq_port_safc_get(unit, local_port, cur_class,
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
 *     bcm_kt2_cosq_pfc_class_mapping_set
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
bcm_kt2_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0, pfc_class[_BCM_KT2_NUM_PFC_CLASS];
    bcm_gport_t cur_gport = 0;
    int local_port = 0;
    uint32 cos_bmp[_BCM_KT2_NUM_PFC_CLASS];

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
    if (!SOC_IS_SABER2(unit) && !IS_XE_PORT(unit,local_port)) {
        return BCM_E_PARAM; 
    }
    if ((array_count < 0) || (array_count > _BCM_KT2_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_KT2_NUM_PFC_CLASS)) {
            return BCM_E_PARAM;
        } else {
            cos_bmp[cur_class] = 0;
        }
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            
            BCM_IF_ERROR_RETURN(_bcm_kt2_fc_status_map_gport(unit, port, 
                cur_gport, _BCM_KT2_COSQ_FC_PFC, &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_KT2_NUM_PFC_CLASS; index++) {
         pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_port_pfc_profile_set(unit, port,
        _BCM_COSQ_OP_SET, _BCM_KT2_NUM_PFC_CLASS, pfc_class, cos_bmp));
    
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_pfc_class_mapping_get(int unit,
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
        (_bcm_kt2_cosq_localport_resolve(unit, port, &local_port));
    if (!SOC_IS_SABER2(unit) && !IS_XE_PORT(unit,local_port)) {
        return BCM_E_PARAM; 
    }
    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }
    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    for (cur_class = 0; cur_class < _BCM_KT2_NUM_PFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        if (array_max > 0) {
            rv = bcm_kt2_cosq_port_pfc_get(unit, local_port, sc[cur_class], 
                        mapping_array[class_count].gport_list,
                        BCM_COSQ_PFC_GPORT_COUNT, &actual_gport_count);
        } else {
            rv = bcm_kt2_cosq_port_pfc_get(unit, local_port, sc[cur_class],
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

int
bcm_kt2_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64 sum, value64;
    soc_counter_non_dma_id_t dma_id;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *child_node = NULL;
#ifdef BCM_SABER2_SUPPORT
    uint32 drop_type, total_drop_mask, wred_drop_mask;
    uint32 rval;
#endif
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);

    if (value == NULL) {
        return BCM_E_PARAM;
    }
    if (stat == bcmCosqStatEgressUCQueueBytesCurrent) {
        return _bcm_kt2_cosq_current_stat_set_get(unit, port,
                                                  cosq, stat,
                                                  0, value);
    }

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve
        (unit, port, cosq, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
        /* pass port as queue number */
        local_port = startq;
        startq = 0;
    }

#ifdef BCM_SABER2_SUPPORT
    if(SOC_IS_SABER2(unit)) {
#ifdef BCM_METROLITE_SUPPORT
        if(SOC_IS_METROLITE(unit)) {
           total_drop_mask = BCM_ML_COSQ_TOTAL_DROP_MASK;
           wred_drop_mask = BCM_ML_COSQ_WRED_DROP_MASK;
        } else
#endif
        {
           total_drop_mask = BCM_SB2_COSQ_TOTAL_DROP_MASK;
           wred_drop_mask = BCM_SB2_COSQ_WRED_DROP_MASK;
        }
        SOC_IF_ERROR_RETURN(READ_THDO_DROP_CTR_CONFIGr(unit, &rval));
        drop_type = soc_reg_field_get(unit, THDO_DROP_CTR_CONFIGr, rval, OP_DROP_MASKf); 

        if (drop_type == total_drop_mask) {
            if (!((stat == bcmCosqStatDroppedPackets)||
                 (stat == bcmCosqStatDroppedBytes)||
                 (stat == bcmCosqStatRedCongestionDroppedPackets)||
                 (stat == bcmCosqStatRedCongestionDroppedBytes)||
                 (stat == bcmCosqStatYellowCongestionDroppedPackets)||
                 (stat == bcmCosqStatYellowCongestionDroppedBytes)||
                 (stat == bcmCosqStatOutPackets)||(stat == bcmCosqStatOutBytes))){
                return BCM_E_PARAM;
            }   

        }
        else if (drop_type == wred_drop_mask) {
            if (!((stat == bcmCosqStatDiscardDroppedPackets)||
                  (stat == bcmCosqStatDiscardDroppedBytes)||
                  (stat == bcmCosqStatRedDiscardDroppedPackets)||
                  (stat == bcmCosqStatRedDiscardDroppedBytes)||
                  (stat == bcmCosqStatYellowDiscardDroppedPackets)||
                  (stat == bcmCosqStatYellowDiscardDroppedBytes)||
                  (stat == bcmCosqStatGreenDiscardDroppedBytes)||
                  (stat == bcmCosqStatGreenDiscardDroppedPackets)||
                  (stat == bcmCosqStatOutPackets)||(stat == bcmCosqStatOutBytes))){
                return BCM_E_PARAM;
            }
        }
    }
#endif
    switch (stat) {
        case bcmCosqStatDroppedPackets:
        case bcmCosqStatDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
            break;

        case bcmCosqStatDroppedBytes:
        case bcmCosqStatDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
            break;

        case bcmCosqStatRedCongestionDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
            break;

        case bcmCosqStatRedCongestionDroppedBytes:
        case bcmCosqStatRedDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED;
            break;

        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatYellowDiscardDroppedPackets :
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
            break;

        case bcmCosqStatYellowCongestionDroppedBytes:
        case bcmCosqStatYellowDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_YELLOW;
            break;

        case bcmCosqStatGreenDiscardDroppedPackets:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN;
            break;

        case bcmCosqStatGreenDiscardDroppedBytes:
            dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_GREEN;
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
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
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
           (_bcm_kt2_cosq_node_get(unit, port, 0, 
                            &local_port, NULL, &node));
           COMPILER_64_ZERO(sum);
           if (cosq == -1) { 
               for (i = 0; i < node->numq; i++) {
                    if ((_bcm_kt2_cosq_child_node_at_input(node, i, &child_node))
                                                      == BCM_E_NOT_FOUND) {
                         continue;
                    }
                     port = child_node->gport;
                         
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_index_resolve
                    (unit, port, 0, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                    &local_port, &startq, &numq));

                    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port)) {
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

               if ((_bcm_kt2_cosq_child_node_at_input(node, cosq, &child_node))
                                                  == BCM_E_NOT_FOUND) {
                    return BCM_E_NOT_FOUND;  
               }
               port = child_node->gport;
                     
               BCM_IF_ERROR_RETURN
               (_bcm_kt2_cosq_index_resolve
                (unit, port, 0, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
                 &local_port, &startq, &numq));

               if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
                   BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port)) {
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
bcm_kt2_cosq_gport_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                                 bcm_cosq_gport_stats_t stat, uint64 value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64 value64;
    soc_counter_non_dma_id_t dma_id;

    if (BCM_GPORT_IS_SCHEDULER(port) && cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve
        (unit, port, cosq, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
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
bcm_kt2_cosq_gport_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                                 bcm_cosq_gport_stats_t stat, uint64 *value)
{
    bcm_port_t local_port;
    int startq, numq, i;
    uint64 sum, value64;
    soc_counter_non_dma_id_t dma_id;
    
    if (value == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(port) && cosq != BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }    

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve
        (unit, port, cosq, _BCM_KT2_COSQ_INDEX_STYLE_UCAST_QUEUE,
         &local_port, &startq, &numq));

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
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
             (soc_counter_get(unit, local_port, dma_id, startq, value)); 
    } else {
        COMPILER_64_ZERO(sum);
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (soc_counter_get(unit, local_port, dma_id, startq + i, 
                                 &value64));
            COMPILER_64_ADD_64(sum, value64);
        }
        *value = sum;
    }

    return BCM_E_NONE;
}

#ifdef BCM_KATANA2_SUPPORT
STATIC
void _bcm_kt2_flex_config_set (int unit, bcm_cosq_stat_t *stat_array,
                               int stat_count)
{
    _sb2_cosq_counter_mem_map_t *mem_map;
    int num_elem = 0, elem_size = 4;
    int index = 0;
    int i = 0;
    char *cname_pkt = NULL;
    char *cname_byte = NULL;
    soc_counter_non_dma_id_t non_dma_id = 0;
    uint16 dev_id;
    uint8 rev_id;

    soc_sb2_cosq_counter_mem_map_get(unit, &num_elem, &mem_map);
    if (SOC_IS_SABER2(unit)) {
       elem_size = 2;
    }
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56233_DEVICE_ID) ||
        (SOC_IS_METROLITE(unit))) {
        elem_size = 1;
    }

    /*updating the mem map for cosq counters*/ 
    for (index = 0; index < stat_count; index++) {
        switch (stat_array[index]) {
            case bcmCosqStatDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
                cname_pkt = "DROP_PKT"; 
                cname_byte = "DROP_BYTE"; 
                break;
            case bcmCosqStatDiscardDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
                cname_pkt = "WRED_DROP_PKT"; 
                cname_byte = "WRED_DROP_BYTE"; 
                break;
            case bcmCosqStatYellowCongestionDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
                cname_pkt = "DROP_PKT_Y"; 
                cname_byte = "DROP_BYTE_Y"; 
                break;
            case bcmCosqStatYellowDiscardDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
                cname_pkt = "WRED_DROP_PKT_Y"; 
                cname_byte = "WRED_DROP_BYTE_Y"; 
                break;

            case bcmCosqStatRedCongestionDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
                cname_pkt = "DROP_PKT_R"; 
                cname_byte = "DROP_BYTE_R"; 
                break;
            case bcmCosqStatRedDiscardDroppedPackets:
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
                cname_pkt = "WRED_DROP_PKT_R"; 
                cname_byte = "WRED_DROP_BYTE_R"; 
                break;

            case bcmCosqStatGreenDiscardDroppedPackets: 
                non_dma_id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN; 
                cname_pkt = "WRED_DROP_PKT_G"; 
                cname_byte = "WRED_DROP_BYTE_G"; 
                break;

            case bcmCosqStatOutPackets :
                non_dma_id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
                cname_pkt = "PERQ_PKT"; 
                cname_byte ="PERQ_BYTE"; 
                break;
            default:
                break;
        }
        for ( i = 0; i < elem_size; i++) {
            mem_map[index*elem_size + i].non_dma_id = non_dma_id;
            mem_map[index*elem_size + i].cname_pkt = cname_pkt;
            mem_map[index*elem_size + i].cname_byte = cname_byte;
        }
    } 
}

int 
bcm_kt2_cosq_stat_config_set(int unit, int stat_count, 
                    bcm_cosq_stat_t  *stat_array)
{
    int index = 0;
    uint32 drop_type; 
    uint32 type = 0;
    int rv;
    pbmp_t      pbmp;
    sal_usecs_t     interval;
    uint32      flags;
    soc_port_t port;
#ifdef BCM_KATANA2_SUPPORT
    uint32 total_drop_mask, wred_drop_mask, default_drop_mask;
#endif

#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit)) {
       total_drop_mask = BCM_ML_COSQ_TOTAL_DROP_MASK;
       wred_drop_mask = BCM_ML_COSQ_WRED_DROP_MASK;
       default_drop_mask = BCM_ML_COSQ_DEFAULT_DROP_MASK;
       drop_type = BCM_ML_COSQ_DEFAULT_DROP_MASK;
   } else
#endif
   {
       total_drop_mask = BCM_SB2_COSQ_TOTAL_DROP_MASK;
       wred_drop_mask = BCM_SB2_COSQ_WRED_DROP_MASK;
       default_drop_mask = BCM_SB2_COSQ_DEFAULT_DROP_MASK;
       drop_type = BCM_SB2_COSQ_DEFAULT_DROP_MASK; 
    }

    if ((stat_count != BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED) || 
            (stat_array == NULL)) {
        return BCM_E_PARAM;
    }

    /*validating the stat_types, aligning the stats order generating the type*/
    for ( index = 0; index < stat_count; index++) {
        switch(stat_array[index]) {
            case bcmCosqStatDroppedPackets:
                /*swap with position 0*/
                stat_array[index] = stat_array[0];
                stat_array[0] = bcmCosqStatDroppedPackets;
                /*fall through*/
            case bcmCosqStatYellowCongestionDroppedPackets:
            case bcmCosqStatRedCongestionDroppedPackets:
                type = total_drop_mask;
                break;

            case bcmCosqStatDiscardDroppedPackets:
                /*swap with position 0*/
                stat_array[index] = stat_array[0];
                stat_array[0] = bcmCosqStatDiscardDroppedPackets; 
                /*fall through*/
            case bcmCosqStatGreenDiscardDroppedPackets: 
            case bcmCosqStatYellowDiscardDroppedPackets:
            case bcmCosqStatRedDiscardDroppedPackets:
                type = wred_drop_mask; 
                break;
            case bcmCosqStatOutPackets :
                break;
            default:
                return BCM_E_PARAM;
        }
        if (stat_array[index] != bcmCosqStatOutPackets) {
            if (drop_type != default_drop_mask) {
                if (BCM_SB2_COSQ_CHECK_STAT_TYPE(drop_type,type)) {
                    return BCM_E_PARAM;
                }
            }
            else {
                drop_type = type;
            }
        }

    }
    /* 
     * populate the configuration variable. 
     * we need to configure super set first
     */
    _bcm_kt2_flex_config_set (unit, stat_array, stat_count);

    /*detaching and disabling the counter threads*/
    rv = soc_counter_detach(unit);
    BCM_IF_ERROR_RETURN(rv); 
    
    rv = soc_counter_attach(unit);
    BCM_IF_ERROR_RETURN(rv);
    if (SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(soc_saber2_perq_flex_counters_init(unit,drop_type));
    } else {
        SOC_IF_ERROR_RETURN(_soc_katana2_perq_flex_counters_init(unit,drop_type));
    }

    /*starting  the counter thread*/
    SOC_PBMP_CLEAR(pbmp);
    for (port =  SOC_INFO(unit).cmic_port;
                 port <= SOC_INFO(unit).lb_port;
                 port++) {
        if (SOC_IS_SABER2(unit) && (!SOC_IS_METROLITE(unit)) &&
            soc_saber2_port_is_unused(unit, port)) {
            continue;
        }
        if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,port)]) {
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "BlockName:%s \n"),
                     SOC_BLOCK_NAME(unit, SOC_PORT_BLOCK(unit,port))));
            SOC_PBMP_PORT_ADD(pbmp, port);
        }
    }
    interval = (SAL_BOOT_BCMSIM) ?
               SOC_COUNTER_INTERVAL_SIM_DEFAULT : SOC_COUNTER_INTERVAL_DEFAULT;
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, interval);
    flags =
        soc_property_get(unit, spn_BCM_STAT_FLAGS, SOC_COUNTER_F_DMA);

    rv = soc_counter_start(unit, flags, interval,pbmp); 
    BCM_IF_ERROR_RETURN(rv); 
    
    /*reset all the previous configured counters*/
    BCM_IF_ERROR_RETURN(_bcm_kt2_init_all_queue_counters(unit));
    return BCM_E_NONE;
}


int bcm_kt2_cosq_stat_config_get (int unit, int stat_count,
                                bcm_cosq_stat_t  *stat_array)
{
    _sb2_cosq_counter_mem_map_t *mem_map;
    int num_elem, index = 0, elem_size = 4;
    uint32 rval;
    uint32 drop_type;
    uint16 dev_id;
    uint8 rev_id;
#ifdef BCM_KATANA2_SUPPORT
    uint32 wred_drop_mask, default_drop_mask;
#endif
    if (SOC_IS_SABER2(unit)) {
        elem_size = 2;
    }
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id == BCM56233_DEVICE_ID) ||
        (SOC_IS_METROLITE(unit))) {
        elem_size = 1;
    }

#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        wred_drop_mask = BCM_ML_COSQ_WRED_DROP_MASK;
        default_drop_mask = BCM_ML_COSQ_DEFAULT_DROP_MASK;
    } else
#endif
    {
        wred_drop_mask = BCM_SB2_COSQ_WRED_DROP_MASK;
        default_drop_mask = BCM_SB2_COSQ_DEFAULT_DROP_MASK;
    }
    if ((stat_array == NULL)|| (stat_count != BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED)) {
        return BCM_E_PARAM;
    }
    soc_sb2_cosq_counter_mem_map_get(unit, &num_elem, &mem_map);
    
    /*get the type of THDO counters configured */  
    
    SOC_IF_ERROR_RETURN(READ_THDO_DROP_CTR_CONFIGr(unit, &rval));
    drop_type = soc_reg_field_get(unit, THDO_DROP_CTR_CONFIGr, rval, OP_DROP_MASKf); 

    for (index = 0; index < num_elem; index+= elem_size) {
        switch (mem_map[index].non_dma_id) {
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                if ((drop_type & default_drop_mask) == wred_drop_mask ) {
                    stat_array[index/elem_size] = bcmCosqStatDiscardDroppedPackets;
                } else {
                    stat_array[index/elem_size] = bcmCosqStatDroppedPackets;
                }
                break;

            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW:
                if ((drop_type & default_drop_mask) == wred_drop_mask) {
                    stat_array[index/elem_size] = bcmCosqStatYellowDiscardDroppedPackets;
                } else {
                    stat_array[index/elem_size] = bcmCosqStatYellowCongestionDroppedPackets;
                }
                break;

            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED:
                if ((drop_type & default_drop_mask) == wred_drop_mask) {
                    stat_array[index/elem_size] = bcmCosqStatRedDiscardDroppedPackets;
                } else {
                    stat_array[index/elem_size] = bcmCosqStatRedCongestionDroppedPackets;
                }
                break;

            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN:
                if ((drop_type & default_drop_mask) == wred_drop_mask) {
                    stat_array[index/elem_size] = bcmCosqStatGreenDiscardDroppedPackets;
                }
                break;

            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
                stat_array[index/elem_size] = bcmCosqStatOutPackets;
                break;
            default:
                break;
        }
    }
    return BCM_E_NONE;
}

#endif

int bcm_kt2_cosq_port_get(int unit, int queue_id, bcm_port_t *port) 
{
    _bcm_kt2_mmu_info_t *mmu_info;
    lls_l0_parent_entry_t l0_parent;
    lls_l1_parent_entry_t l1_parent;
    lls_l2_parent_entry_t l2_parent;
    egr_queue_to_pp_port_map_entry_t pp_port_map;
    int index;
    int ctype;
    int rv = BCM_E_NONE;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];
    
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

    /*
     * Ctype 1 indicates parent is a port, 0 indicates parent is an s1.
     */
    ctype  = soc_mem_field32_get(unit, LLS_L0_PARENTm, &l0_parent, C_TYPEf);;
    if (ctype) {
        *port = soc_mem_field32_get(unit, LLS_L0_PARENTm,
                &l0_parent, C_PARENTf);
    } else {
        /*
         * Get the subport that this queue is associated with.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_QUEUE_TO_PP_PORT_MAPm(unit, MEM_BLOCK_ALL,
                                            queue_id, &pp_port_map));
        *port = soc_mem_field32_get(unit, EGR_QUEUE_TO_PP_PORT_MAPm,
                &pp_port_map, PP_PORTf);
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                     SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                     SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                     SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                     SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                     SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                     SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                     SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                     SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION                 BCM_WB_VERSION_1_8

#define _BCM_KT2_COSQ_WB_END_NODE_VALUE         0xffffffff

#define _BCM_KT2_COSQ_WB_NODE_COSQ_MASK         0x3f    /* LW-1 [5:0] */
#define _BCM_KT2_COSQ_WB_NODE_COSQ_SHIFT        0
#define _BCM_KT2_COSQ_WB_NODE_NUMQ_MASK         0x3f    /* LW-1 [11:6] */
#define _BCM_KT2_COSQ_WB_NODE_NUMQ_SHIFT        6
#define _BCM_KT2_COSQ_WB_NODE_HW_INDEX_MASK     0xfff   /* LW-1 [23:12] */
#define _BCM_KT2_COSQ_WB_NODE_HW_INDEX_SHIFT    12
#define _BCM_KT2_COSQ_WB_NODE_LEVEL_MASK        0x7     /* LW-1 [26:24] */
#define _BCM_KT2_COSQ_WB_NODE_LEVEL_SHIFT       24
#define _BCM_KT2_COSQ_WB_NODE_WRR_MASK          0x1     /* LW-1 [27] */
#define _BCM_KT2_COSQ_WB_NODE_WRR_SHIFT         27
#define _BCM_KT2_COSQ_WB_NODE_SUBPORT_MASK      0x1     /* LW-1 [28] */
#define _BCM_KT2_COSQ_WB_NODE_SUBPORT_SHIFT     28
#define _BCM_KT2_COSQ_WB_NODE_LINKPHY_MASK      0x1     /* LW-1 [29] */
#define _BCM_KT2_COSQ_WB_NODE_LINKPHY_SHIFT     29
#define _BCM_KT2_COSQ_WB_NODE_IDBASED_MASK      0x1     /* LW-1 [30] */
#define _BCM_KT2_COSQ_WB_NODE_IDBASED_SHIFT     30 
#define _BCM_KT2_COSQ_WB_NODE_NUMQ_MAX_MASK     0x1     /* LW-1 [31] */
#define _BCM_KT2_COSQ_WB_NODE_NUMQ_MAX_SHIFT    31

#define _BCM_KT2_COSQ_WB_NUM_COS_MASK         0xf    /* [3:0] */
#define _BCM_KT2_COSQ_WB_NUM_COS_SHIFT        0

#define _BCM_KT2_COSQ_WB_NODE_TYPE_MASK       0xf
#define _BCM_KT2_COSQ_WB_NODE_TYPE_SHIFT        0     /*[0:3] 8*/
#define _BCM_KT2_COSQ_WB_NODE_WRR_MODE_MASK   0x1
#define _BCM_KT2_COSQ_WB_NODE_WRR_MODE_SHIFT    4     /*[4]*/
#define _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_MASK   0x1
#define _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_SHIFT    5 /*[5]*/
#define _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_MASK   0x1
#define _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_SHIFT    6 /*[6]*/

#define SET_FIELD(_field, _value)                       \
    (((_value) & _BCM_KT2_COSQ_WB_## _field## _MASK) <<  \
     _BCM_KT2_COSQ_WB_## _field## _SHIFT)
#define GET_FIELD(_field, _byte)                        \
    (((_byte) >> _BCM_KT2_COSQ_WB_## _field## _SHIFT) &  \
     _BCM_KT2_COSQ_WB_## _field## _MASK)

#define _BCM_KT2_NODE_TYPE_WRR_SET(ptr, node)  \
    do {*((uint8 *)ptr) = node->type; \
     *((uint8 *)ptr) |= (node->wrr_mode << _BCM_KT2_COSQ_WB_NODE_WRR_MODE_SHIFT);\
    }while(0);

#define _BCM_KT2_NODE_WRED_DISABLE_SET(ptr, node)  \
    do {\
     *((uint8 *)ptr) |= ((node->wred_disabled & _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_MASK)\
      << _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_SHIFT);\
    }while(0);

#define _BCM_KT2_NODE_BW_BURST_CAL_SET(ptr, node)  \
    do {\
     *((uint8 *)ptr) |= ((node->burst_calculation_enable & _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_MASK)\
      << _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_SHIFT);\
    }while(0);

#define _BCM_KT2_NODE_TYPE_WRR_GET(ptr, node)   \
    do { node->type = ((*(uint8 *)ptr) & _BCM_KT2_COSQ_WB_NODE_TYPE_MASK); \
      node->wrr_mode = ((*(uint8 *)ptr) >>  _BCM_KT2_COSQ_WB_NODE_WRR_MODE_SHIFT);\
    }while(0); 

#define _BCM_KT2_NODE_WRED_DISABLE_GET(ptr, node)   \
    do {\
      node->wred_disabled  = (((*(uint8 *)ptr) >>  _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_SHIFT) \
                              & _BCM_KT2_COSQ_WB_NODE_WRED_DISABLE_MASK);\
    }while(0);

#define _BCM_KT2_NODE_BW_BURST_CAL_GET(ptr, node)   \
    do {\
      node->burst_calculation_enable  = (((*(uint8 *)ptr) >>  _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_SHIFT) \
                              & _BCM_KT2_COSQ_WB_NODE_BW_BURST_CAL_MASK);\
    }while(0);

/*
 * This funtion encodes the 1 byte of the common  data to be written in to
 * scache.Currently only last 4 bits of the byte are used for storing
 * num_cos value rest can be used if necessary later.
 */
STATIC void
_bcm_kt2_cosq_wb_encode_data(int unit, uint8 *scache_ptr)
 {
     uint8 data = 0;
     int numq = 0;
     bcm_kt2_cosq_config_get(unit,&numq);
     data  = SET_FIELD(NUM_COS, numq);
     *scache_ptr = data;
}

STATIC int
_bcm_kt2_cosq_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    _bcm_kt2_mmu_info_t *mmu_info;
    int rv, alloc_size;

    mmu_info = _bcm_kt2_mmu_info[unit];

    alloc_size = 0;
    alloc_size += sizeof(mmu_info->num_base_queues);
    alloc_size += sizeof(mmu_info->num_ext_queues);
    alloc_size += sizeof(mmu_info->qset_size);
    alloc_size += sizeof(mmu_info->num_queues);
    alloc_size += sizeof(mmu_info->num_nodes);
    alloc_size += sizeof(mmu_info->max_nodes);
    alloc_size += (num_total_schedulers[unit] * sizeof(uint32) * 3);
    alloc_size += (num_l2_queues[unit] * sizeof(uint32) * 2);
    alloc_size += sizeof(uint32);

    /* Following is the addition memory for Version 1_1 */

    /* Allocating memory for storing fabric_pbmp and we require
     fabric_pbmp only for l2 nodes*/ 
    alloc_size += (num_l2_queues[unit] * sizeof(bcm_pbmp_t));
    /* Allocating memory for storing remote_modid and we require
     remote_modid only for l2 nodes*/ 
    alloc_size += (num_l2_queues[unit] * sizeof(uint32));
    /* Allocating memory for storing fabric_modid and we require
       fabric_modid only for l2 nodes*/ 
    alloc_size += (num_l2_queues[unit] * sizeof(uint32));
    /* Allocating memory for storing curr_merger_index */
    alloc_size += (4 * sizeof(int));
    /* Storing the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
    alloc_size += sizeof(uint32);
    /* Storing NUM_COS */
    alloc_size += sizeof(uint8);

    /* Allocating memory for storing the multicast mc base index
       for the L1 nodes*/
    alloc_size += (num_l1_schedulers[unit] * sizeof(uint32));
    /* Storing the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
    alloc_size += sizeof(uint32);
   /* Allocating memory for storing the IFP_COS_MAP refcount */
    alloc_size += (4 * sizeof(uint32));
    /* Storing the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
    alloc_size += sizeof(uint32);
   
    /*Storing the COSQ_STAT_CONFIG*/ 
    alloc_size += (sizeof(bcm_cosq_stat_t)* 3);

    /*Storing the node data*/
    alloc_size += sizeof(uint8)*(num_total_schedulers[unit] + num_l2_queues[unit]);
    
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

STATIC uint32
_bcm_kt2_cosq_wb_encode_node(int unit, _bcm_kt2_cosq_node_t *node)
{
    uint32 data = 0;
    int max_numq;

    data  = SET_FIELD(NODE_COSQ, node->cosq_attached_to);
    data |= SET_FIELD(NODE_NUMQ, node->numq);
    max_numq = ((1 << _BCM_KT2_COSQ_WB_NODE_NUMQ_SHIFT) - 1);
    if (node->numq > max_numq) {
        data |= SET_FIELD(NODE_NUMQ_MAX, 1);
    }
    data |= SET_FIELD(NODE_LEVEL, node->level);
    data |= SET_FIELD(NODE_HW_INDEX, node->hw_index);
    data |= SET_FIELD(NODE_WRR, node->wrr_in_use);
    data |= SET_FIELD(NODE_SUBPORT, node->subport_enabled);
    data |= SET_FIELD(NODE_LINKPHY, node->linkphy_enabled);
    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        if ((node->type == _BCM_KT2_NODE_VOQ_ID) ||
            (node->type == _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID)) {
            data |= SET_FIELD(NODE_IDBASED, 1);
        }
    }
    
    return data;
}

STATIC int
_bcm_kt2_cosq_update_scache(int unit, uint8 *scache_ptr, 
                            _bcm_kt2_cosq_node_t *node, int *length)
{
    uint8 *ptr;
    uint32 node_data;
    bcm_gport_t sub_gport = 0;
    int id;
    int port = 0;
    _bcm_kt2_mmu_info_t *mmu_info;

    ptr = scache_ptr;
    mmu_info = _bcm_kt2_mmu_info[unit];

    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)){
        id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(node->gport);
        if(mmu_info->num_sub_queues) {
            id += mmu_info->base_sub_queue;
        }
        port = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(node->gport);
        BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(sub_gport, port, id);
        *(((uint32 *)ptr)) = sub_gport;
        ptr += sizeof(uint32);
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport)){
        id = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(node->gport);
        if(mmu_info->num_sub_queues) {
            id += mmu_info->base_sub_queue;
        }
        port =
        BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(node->gport);
        BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(sub_gport, port, id);
        *(((uint32 *)ptr)) = sub_gport;
        ptr += sizeof(uint32);
    } else {
        *(((uint32 *)ptr)) = node->gport;
        ptr += sizeof(uint32);
    }
            
    node_data = _bcm_kt2_cosq_wb_encode_node(unit, node);
    *(((uint32 *)ptr)) = node_data;
    ptr += sizeof(uint32);

    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        *(((uint32 *)ptr)) = node->base_index;
        ptr += sizeof(uint32);
    }

    *length = ptr - scache_ptr;
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_update_l0_tree(int unit, uint8 *scache_ptr, 
                              _bcm_kt2_cosq_node_t *node, 
                             int *length)
{
    uint8 *ptr;
    _bcm_kt2_cosq_node_t *l0_node, *l1_node, *l2_node;
    int inc;

    ptr = scache_ptr;

    for (l0_node = node; l0_node != NULL;
         l0_node = l0_node->sibling) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_update_scache(unit, ptr, l0_node, &inc));
        ptr += inc;

        for (l1_node = l0_node->child; l1_node != NULL;
             l1_node = l1_node->sibling) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_scache(unit, ptr, l1_node, &inc));
            ptr += inc;
                
            for (l2_node = l1_node->child; l2_node != NULL;
                 l2_node = l2_node->sibling) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_update_scache(unit, ptr, l2_node, &inc));
                ptr += inc;
            }
        }
    }

    *length = ptr - scache_ptr;
    return BCM_E_NONE;
}


int
bcm_kt2_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *port_node, *l0_node;
    _bcm_kt2_cosq_node_t *s0_node, *s1_node;
    bcm_port_t port;
    int i, inc;
    _bcm_kt2_cosq_node_t *l1_node, *l2_node;
    soc_info_t *si;
    uint32 profile_index;
    int ref_count;

#ifdef BCM_KATANA2_SUPPORT      
    bcm_cosq_stat_t stat_array[BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED];
#endif
    si = &SOC_INFO(unit);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

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

    for (i = 0; i < _BCM_KT2_COSQ_NODE_LEVEL_MAX; i++) {
        *(((uint32 *)ptr)) = mmu_info->max_nodes[i];
        ptr += sizeof(uint32);
    }

    for (port = 0; port < (si->lb_port + 1) ; port++) {
        if (SOC_IS_SABER2(unit) && (!SOC_IS_METROLITE(unit)) &&
            soc_saber2_port_is_unused(unit, port)) {
            continue;
        }
        port_node = &mmu_info->sched_node[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }

        if (port_node->child != NULL) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_scache(unit, ptr, port_node, &inc));
            ptr += inc;
        }

        s0_node = port_node->child;
        if (s0_node != NULL && 
                s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
            for (s0_node = port_node->child; s0_node != NULL;
                    s0_node = s0_node->sibling) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_update_scache(unit, ptr, s0_node, &inc));
                ptr += inc;

                s1_node = s0_node->child;
                if (s1_node != NULL && 
                        s1_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) {
                    for (s1_node = s0_node->child; s1_node != NULL;
                            s1_node = s1_node->sibling) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_update_scache(unit, ptr, s1_node, &inc));
                        ptr += inc;
                        for (l0_node = s1_node->child; l0_node != NULL;
                                l0_node = l0_node->sibling) {
                            BCM_IF_ERROR_RETURN
                                (_bcm_kt2_cosq_update_l0_tree(unit, ptr, l0_node, &inc));
                            ptr += inc;
                        }
                    }
                } else {
                    l0_node = s1_node;
                    BCM_IF_ERROR_RETURN
                        (_bcm_kt2_cosq_update_l0_tree(unit, ptr, l0_node, &inc));
                    ptr += inc;
                }
            }
        } else {
            l0_node = port_node->child;
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_l0_tree(unit, ptr, l0_node, &inc));
            ptr += inc;
        }
    }

    *(((uint32 *)ptr)) = _BCM_KT2_COSQ_WB_END_NODE_VALUE;

    ptr += sizeof(uint32) ;

    for (i = 0; i < 4; i++) {
        *(((uint32 *)ptr)) = mmu_info->curr_merger_index[i];
        ptr += sizeof(uint32);
    }

    PBMP_ALL_ITER(unit, port) {
        if (!(IS_HG_PORT(unit, port)|| IS_HL_PORT(unit, port))) {
            continue;
        }
        port_node = &mmu_info->sched_node[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }

        s0_node = port_node->child;
        if (s0_node != NULL && 
                s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
            continue;
        } else {
            l0_node = port_node->child;
            for (; l0_node != NULL;
                    l0_node = l0_node->sibling) {
                for (l1_node = l0_node->child; l1_node != NULL;
                        l1_node = l1_node->sibling) {
                    for (l2_node = l1_node->child; l2_node != NULL;
                            l2_node = l2_node->sibling) {
                        sal_memcpy(ptr, &l2_node->fabric_pbmp, sizeof(bcm_pbmp_t));
                        ptr += sizeof(bcm_pbmp_t);
                        *(((uint32 *)ptr)) = l2_node->remote_modid;
                        ptr += sizeof(uint32);
                        *(((uint32 *)ptr)) = l2_node->fabric_modid;
                        ptr += sizeof(uint32);
                    }
                }
            }
        }
    }
    *(((uint32 *)ptr)) = _BCM_KT2_COSQ_WB_END_NODE_VALUE;
    ptr += sizeof(uint32);

    /* add NUM_COS to the scache */
    _bcm_kt2_cosq_wb_encode_data( unit, (uint8 *)ptr);
    ptr += sizeof(uint8);

    /* Store the mc base index for L1 node */
    PBMP_ALL_ITER(unit, port) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit)) {
            if((soc_property_get(unit, spn_SAT_ENABLE, 0) != 0) &&
               (port == SOC_SB2_SAT_OAMP_PHY_PORT_NUMBER)) {
               /* no need to store SAT port*/
               continue;
            }
        }
#endif
        port_node = &mmu_info->sched_node[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }
        if (!port_node->child) {
            continue;
        }
        s0_node = (port_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ?
            port_node->child : port_node;

        do {
            s1_node = (s0_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) ?
                s0_node->child : s0_node;
            do {
                for (l0_node = s1_node->child;
                        l0_node != NULL;
                        l0_node = l0_node->sibling) {
                    for (l1_node = l0_node->child;
                            l1_node != NULL;
                            l1_node = l1_node->sibling) {
                        *(((uint32 *)ptr)) =  l1_node->mc_base_index;
                        ptr += sizeof(uint32);

                    }
                }
                if (s1_node == s0_node) {
                    break;
                }
                /* It comes here
                   only in case s1 is valid */
                s1_node = s1_node->sibling;
            } while (s1_node != NULL);
            if (s0_node == port_node) {
                break;
            }
            /* It comes here
               only in case s0 is valid */
            s0_node = s0_node->sibling;
        } while (s0_node != NULL);

    }

    *(((uint32 *)ptr)) = _BCM_KT2_COSQ_WB_END_NODE_VALUE;

    ptr += sizeof(uint32);
    for (profile_index = 0 ; profile_index < 4 ; profile_index++) {
        BCM_IF_ERROR_RETURN (soc_profile_mem_ref_count_get(unit,
                    _bcm_kt2_ifp_cos_map_profile[unit],
                    (profile_index * 16), &ref_count));
        *(((uint32 *)ptr)) = ref_count;
        ptr += sizeof(uint32);
    }

    *(((uint32 *)ptr)) = _BCM_KT2_COSQ_WB_END_NODE_VALUE;

    /*saving the counter values*/
#ifdef BCM_KATANA2_SUPPORT 
    if (SOC_IS_KATANA2(unit)) {
        bcm_kt2_cosq_stat_config_get(unit, BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED, stat_array);
        for (profile_index = 0; profile_index < 3; profile_index++) {
            *(((uint32 *)ptr)) = stat_array[profile_index];
            ptr += sizeof(uint32);
        }
    }
    else {
        ptr += (sizeof(uint32) * 3);
    }
#else
    ptr += (sizeof(uint32) * 3);
#endif

    PBMP_ALL_ITER(unit, port) {
        port_node = &mmu_info->sched_node[port];
        if (port_node->cosq_attached_to < 0) {
            continue;
        }
        _BCM_KT2_NODE_TYPE_WRR_SET(ptr, port_node);
        _BCM_KT2_NODE_BW_BURST_CAL_SET(ptr, port_node);
        ptr += sizeof(uint8);
        s0_node = port_node->child;
        if (s0_node != NULL && 
                s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
            continue;
        } else {
            /*copy the node type to the node*/
            for (l0_node = port_node->child;l0_node != NULL;
                    l0_node = l0_node->sibling) {
                /*copy the node type to the node*/
                _BCM_KT2_NODE_TYPE_WRR_SET(ptr, l0_node);
                _BCM_KT2_NODE_BW_BURST_CAL_SET(ptr, port_node);
                ptr += sizeof(uint8);
                for (l1_node = l0_node->child; l1_node != NULL;
                        l1_node = l1_node->sibling) {
                    /*copy the node type to the node*/
                    _BCM_KT2_NODE_TYPE_WRR_SET(ptr, l1_node);
                    _BCM_KT2_NODE_BW_BURST_CAL_SET(ptr, port_node);
                    ptr += sizeof(uint8);
                    for (l2_node = l1_node->child; l2_node != NULL;
                            l2_node = l2_node->sibling) {
                        /*copy the node type to the node*/
                        _BCM_KT2_NODE_TYPE_WRR_SET(ptr,l2_node);
                        _BCM_KT2_NODE_BW_BURST_CAL_SET(ptr, port_node);
                        _BCM_KT2_NODE_WRED_DISABLE_SET(ptr,l2_node);
                        ptr += sizeof(uint8);
                    }
                }
            }
        }    
    }

    /* copy interface reference count values for all queues into scache ptr */
    sal_memcpy(ptr, mmu_info->intf_ref_cnt,
               (sizeof(uint32) * BCM_MULTICAST_PORT_MAX));
    ptr += (sizeof(uint32) * BCM_MULTICAST_PORT_MAX);
    return BCM_E_NONE; 
}

/*
 * Function:
 *     _bcm_kt2_cosq_map_alloc_set
 * Purpose:
 *     Allocate the cosq_map bitmap and 
 *     restore  
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_kt2_cosq_map_alloc_set(_bcm_kt2_cosq_node_t *node)
{
    _bcm_kt2_cosq_node_t *child;
    int alloc_size;
    if (node->cosq_map == NULL) {
        alloc_size = SHR_BITALLOCSIZE(node->numq);
        node->cosq_map = _bcm_kt2_cosq_alloc_clear(alloc_size,
                                           "node->cosq_map");
        if (node->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }
    child = node->child;
    while (child != NULL) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_map_index_set(node->cosq_map, 
                                         child->cosq_attached_to, 1));
        child = child->sibling;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_map_recover_next_level_node  
 * Purpose:
 *     Used to recover cos_map for S0,S1,L0,L1 levels   
 * Parameters:
 *     node       - (IN) cosq_node 
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt2_cosq_map_recover_next_level_node(_bcm_kt2_cosq_node_t *node) 
{
    if (!node) {
        return BCM_E_NONE;
    }
    if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_alloc_set(node));
    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_recover_next_level_node(node->sibling));
    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_recover_next_level_node(node->child));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_update_wred_profile_ref_count
 * Purpose:
 *     Used to recover MMU_WRED_DROP_CURVE_PROFILE_x memory profile ref count
 * Parameters:
 *     unit        - (IN) Device unit
 *     index       - (IN) Hardware index for the cosq
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
_bcm_kt2_cosq_update_wred_profile_ref_count(int unit, int index) {
    mmu_wred_queue_config_buffer_entry_t qentry;
    mmu_wred_queue_config_qentry_entry_t qentry1;
    int set_index;
    soc_mem_t mem;
    soc_profile_mem_t *profile_mem;

    profile_mem = _bcm_kt2_wred_profile[unit];

    /* scan byte based wred table */
    mem = MMU_WRED_QUEUE_CONFIG_BUFFERm;
    memset(&qentry, 0, sizeof(mmu_wred_queue_config_qentry_entry_t));
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

    return BCM_E_NONE;

}

int
bcm_kt2_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *ptr;
    _bcm_kt2_cosq_node_t *port_info;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *port_node = NULL;
    _bcm_kt2_cosq_node_t *queue_node = NULL;
    _bcm_kt2_cosq_node_t *node = NULL;
    _bcm_kt2_cosq_node_t *temp_node = NULL;
    _bcm_kt2_cosq_node_t *s0_node = NULL;
    _bcm_kt2_cosq_node_t *s1_node = NULL;
    _bcm_kt2_cosq_node_t *l0_node = NULL;
    _bcm_kt2_cosq_node_t *l1_node = NULL;
    _bcm_kt2_cosq_node_t *tmp_node = NULL;
    uint32 *s0_bmap = NULL;
    uint32 *s1_bmap = NULL;
    uint32 *l0_bmap = NULL;
    uint32 *l1_bmap = NULL;
    uint32 *l2_bmap = NULL;
    int additional_scache_size = 0;
    int rv, stable_size = 0;
    bcm_port_t port = 0;
    bcm_gport_t gport;
    int i, index, numq;
    uint32 node_data;
    uint32 encap_id;
    int set_index, cosq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_profile_mem_t *profile_mem;
    _bcm_kt2_cosq_list_t *list;
    _bcm_kt2_cosq_node_t *l2_node;
    int cpu_hg_index = 0;
    bcm_pbmp_t all_pbmp;
    bcm_port_t tmp_port = 0;
    bcm_gport_t tmp_gport = 0;
    uint16 recovered_ver = 0;
    int offset = 0;
    int node_id_based = 0;
    uint8 data;
    int num_cos = 0;
    soc_profile_reg_t *profile_reg;
    soc_reg_t reg;
    uint64 rval,index_map; 
    int mmu_port = 0 ;
    uint32  tmp32;
    int max_numq;
    uint32 refcount = 0, profile_index;
#ifdef BCM_KATANA2_SUPPORT
    bcm_cosq_stat_t stat_array[BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED];
#endif
#ifdef BCM_METROLITE_SUPPORT
    uint32 rval32, index_map32;
#endif
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr,
            BCM_WB_DEFAULT_VERSION,  &recovered_ver);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

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

    for (i = 0; i < _BCM_KT2_COSQ_NODE_LEVEL_MAX; i++) {
        mmu_info->max_nodes[i] = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
    }

    while (*(((uint32 *)ptr)) != _BCM_KT2_COSQ_WB_END_NODE_VALUE) {
        gport = *(((uint32 *)ptr));
        ptr += sizeof(uint32);
        node_data = *(((uint32 *)ptr));
        ptr += sizeof(uint32);

        if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            tmp_port = 0;
            tmp_gport = 0;

            if (mmu_info->num_sub_queues ==0)  {
                list = &mmu_info->global_qlist;
            } else {
                list = &mmu_info->sub_qlist;
            }
            encap_id = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id];

            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_wred_profile_ref_count(unit, encap_id));

            if (mmu_info->num_sub_queues)  {
                encap_id -= mmu_info->base_sub_queue;
            }
            tmp_port = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(gport);
            BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(tmp_gport, tmp_port, encap_id);
            gport = tmp_gport;
            _bcm_kt2_node_index_set(list, encap_id, 1);
         } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            tmp_port = 0;
            tmp_gport = 0;
            if (mmu_info->num_sub_queues ==0)  {
                list = &mmu_info->global_qlist;
            } else {
                list = &mmu_info->sub_mcqlist;
            }
            encap_id = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id];

            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_wred_profile_ref_count(unit, encap_id));

            if (mmu_info->num_sub_queues)  {
                encap_id = encap_id - mmu_info->base_sub_queue - mmu_info->num_queues/2;
            }
            tmp_port =
            BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSPORTID_GET(gport);
            BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_SYSQID_SET(tmp_gport,
                                                     tmp_port, encap_id);
            gport = tmp_gport;
            _bcm_kt2_node_index_set(list, encap_id, 1);
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id]; 
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
            node = &mmu_info->queue_node[encap_id];        
        } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
            encap_id = BCM_GPORT_DESTMOD_QUEUE_GROUP_QID_GET(gport);
            if (mmu_info->num_dmvoq_queues ==0)  { 
                list = &mmu_info->global_qlist;
            } else {
                list = &mmu_info->dmvoq_qlist;
            }
            if (mmu_info->num_dmvoq_queues == 1)  {
                encap_id += mmu_info->base_dmvoq_queue;
            }
            node = &mmu_info->queue_node[encap_id];
        } else {
            encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0x7ff;
            if (encap_id == 0) {
                encap_id = (BCM_GPORT_SCHEDULER_GET(gport) & 0xff);
            }
            list = &mmu_info->sched_list;
            _bcm_kt2_node_index_set(list, encap_id, 1);
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
        node->subport_enabled = GET_FIELD(NODE_SUBPORT, node_data);
        node->linkphy_enabled = GET_FIELD(NODE_LINKPHY, node_data);
        node_id_based = GET_FIELD(NODE_IDBASED, node_data);

        switch (node->level) {
            case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
                port_node = node;
                if (port_node != NULL) {
                    port_node->parent = NULL;
                    port_node->sibling = NULL;
                    port_node->child = NULL;
                    s1_node = NULL;
                    s0_node = NULL;
                }
                break;

            case _BCM_KT2_COSQ_NODE_LEVEL_S0:
                s0_node = node;
                list = &mmu_info->sched_hw_list[_BCM_KT2_COSQ_NODE_LEVEL_S0];

                if (port_node != NULL && s0_node != NULL) {
                    s0_node->parent = port_node;
                    tmp_node = port_node;
                    if (tmp_node->child == NULL) {
                        tmp_node->child = s0_node;
                        s0_node->sibling = NULL;
                    } else {
                        temp_node = tmp_node->child;
                        while (temp_node->sibling != NULL) {
                            temp_node = temp_node->sibling;
                        }
                        temp_node->sibling = s0_node;
                    }
                    port_node->num_child++;
                    if ((port_node->numq > 0) &&
                        (port_node->numq != mmu_info->max_nodes[s0_node->level])) {
                        if (s0_node->hw_index < port_node->first_child) {
                            port_node->first_child = s0_node->hw_index;
                        }
                        if (port_node->num_child == 1) {
                            if ((port_node->base_index + port_node->numq - 1)
                                        < soc_mem_index_count(unit, LLS_S1_CONFIGm)) {
                                _bcm_kt2_node_index_set(list, port_node->base_index,
                                    port_node->numq);
                            }
                        }
                    } else if (s0_node->hw_index < 128) {
                        _bcm_kt2_node_index_set(list, s0_node->hw_index, 1);
                    }
                    if (port_node->numq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_max_nodes_get(unit, s0_node->level, 
                                                            &max_numq));
                        port_node->numq = max_numq;
                    }
                }
                break;

            case _BCM_KT2_COSQ_NODE_LEVEL_S1:
                s1_node = node;
                list = &mmu_info->sched_hw_list[_BCM_KT2_COSQ_NODE_LEVEL_S1];

                if (s0_node != NULL && s1_node != NULL) {
                    s1_node->parent = s0_node;
                    tmp_node = s0_node;
                    if (tmp_node->child == NULL) {
                        tmp_node->child = s1_node;
                        s1_node->sibling = NULL;
                    } else {
                        temp_node = tmp_node->child;
                        while (temp_node->sibling != NULL) {
                            temp_node = temp_node->sibling;
                        }
                        temp_node->sibling = s1_node;
                    }
                    s0_node->num_child++;

                    if ((s0_node->numq > 0) &&
                        (s0_node->numq != mmu_info->max_nodes[node->level])) {
                        if (s1_node->hw_index < s0_node->first_child) {
                            s0_node->first_child = s1_node->hw_index;
                        }
                        if (s0_node->num_child == 1) {
                            _bcm_kt2_node_index_set(list, s0_node->base_index, 
                                    s0_node->numq);
                        }
                    } else {
                        _bcm_kt2_node_index_set(list, s1_node->hw_index, 1);
                    }
                    if (s0_node->numq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_max_nodes_get(unit, s1_node->level, 
                                                            &max_numq));
                        s0_node->numq = max_numq;
                    }
                }
                break;

            case _BCM_KT2_COSQ_NODE_LEVEL_L0:
                l0_node = node;
                list = &mmu_info->sched_hw_list[_BCM_KT2_COSQ_NODE_LEVEL_L0];

                tmp_node = (s1_node != NULL) ? 
                    s1_node : ((s0_node != NULL) ? s0_node : port_node);
                if (tmp_node != NULL && l0_node != NULL) {
                    l0_node->parent = tmp_node;
                    if (tmp_node->child == NULL) {
                        tmp_node->child = l0_node;
                        l0_node->sibling = NULL;
                    } else {
                        temp_node = tmp_node->child;
                        while (temp_node->sibling != NULL) {
                            temp_node = temp_node->sibling;
                        }
                        temp_node->sibling = l0_node;
                    }
                    tmp_node->num_child++;
                    if ((tmp_node->numq > 0) &&
                        (tmp_node->numq != mmu_info->max_nodes[node->level])) {
                        if (l0_node->hw_index < tmp_node->first_child) {
                            tmp_node->first_child = l0_node->hw_index;
                        }
                        if (tmp_node->num_child == 1) {
                            _bcm_kt2_node_index_set(list, tmp_node->base_index, 
                                    tmp_node->numq);
                        }
                    } else {
                        _bcm_kt2_node_index_set(list, l0_node->hw_index, 1);
                    }
                    if (tmp_node->numq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_max_nodes_get(unit, l0_node->level, 
                                                            &max_numq));
                        tmp_node->numq = max_numq;
                    }
                }
                break;

            case _BCM_KT2_COSQ_NODE_LEVEL_L1:
                l1_node = node;
                list = &mmu_info->sched_hw_list[_BCM_KT2_COSQ_NODE_LEVEL_L1];
                if (l0_node != NULL && l1_node != NULL) {
                    l1_node->parent = l0_node;
                    if (l0_node->child == NULL) {
                        l0_node->child = l1_node;
                        l1_node->sibling = NULL;
                    } else {
                        temp_node = l0_node->child;
                        while (temp_node->sibling != NULL) {
                            temp_node = temp_node->sibling;
                        }
                        temp_node->sibling = l1_node;
                    }
                    l0_node->num_child++;

                    if ((l0_node->numq > 0) &&
                        (l0_node->numq != mmu_info->max_nodes[node->level])) {
                        if (l1_node->hw_index < l0_node->first_child) {
                            l0_node->first_child = l1_node->hw_index;
                        }
                        if (l0_node->num_child == 1) {
                            _bcm_kt2_node_index_set(list, l0_node->base_index, 
                                    l0_node->numq);
                        }
                    } else {
                        _bcm_kt2_node_index_set(list, l1_node->hw_index, 1);
                    }
                    if (l0_node->numq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_max_nodes_get(unit, l1_node->level, 
                                                            &max_numq));
                        l0_node->numq = max_numq;
                    }
                }
                break;

            case _BCM_KT2_COSQ_NODE_LEVEL_L2:
                queue_node = node;
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    list = &mmu_info->l2_base_qlist;
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                    list = &mmu_info->l2_base_mcqlist;
                } else if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {

                    if (mmu_info->num_sub_queues == 0)  {
                        list = &mmu_info->l2_global_qlist;
                    } else {
                        list = &mmu_info->l2_sub_qlist;
                    }
                    if (node_id_based) {
                        queue_node->type = _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID;
                    }
                } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
                    if (mmu_info->num_sub_queues == 0)  {
                        list = &mmu_info->l2_global_qlist;
                    } else {
                        list = &mmu_info->l2_sub_mcqlist;
                    }
                    if (node_id_based) {
                        queue_node->type = _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID;
                    }
                } else if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
                    if (mmu_info->num_dmvoq_queues ==0)  { 

                        list = &mmu_info->l2_global_qlist;
                        offset = 0 ; 
                    } else {
                        list = &mmu_info->l2_dmvoq_qlist;
                        offset = mmu_info->base_dmvoq_queue;
                    }
                    if (node_id_based) {
                        queue_node->type = _BCM_KT2_NODE_VOQ_ID; 
                    } else {
                        queue_node->type = _BCM_KT2_NODE_VOQ; 
                    }
                } else {
                    list = &mmu_info->l2_global_qlist;
                }
                if (l1_node != NULL && queue_node != NULL) {
                    queue_node->parent = l1_node;
                    queue_node->hw_index = GET_FIELD(NODE_HW_INDEX, node_data);
                    /* Creating LLS tree how it is created */
                    if (l1_node->child == NULL) {
                        l1_node->child = queue_node;
                        queue_node->sibling = NULL;
                    } else {
                        temp_node = l1_node->child;
                        while (temp_node->sibling != NULL) {
                            temp_node = temp_node->sibling;
                        }
                        temp_node->sibling = queue_node;
                    }
                    l1_node->num_child++;

                    if ((l1_node->numq > 0) &&
                        (l1_node->numq != mmu_info->max_nodes[node->level])) {
                        if (queue_node->hw_index < l1_node->first_child) {
                            l1_node->first_child = queue_node->hw_index;
                        }
                        if (BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) {
                            if (node_id_based) {
                                _bcm_kt2_node_index_set(list, encap_id - offset , 1);
                            } else if (encap_id % 8 == 0) {
                                _bcm_kt2_node_index_set(list, encap_id - offset , l1_node->numq);
                            }
                        } else if (l1_node->num_child == 1) {
                            if (
                             BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
                             BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
                                if(node_id_based) {
                                    _bcm_kt2_node_index_set(list,
                                            encap_id,
                                            1);
                                } else {
                                    _bcm_kt2_node_index_set(list,
                                            encap_id,
                                            l1_node->numq);
                                }
                            } else {
                                _bcm_kt2_node_index_set(list, l1_node->base_index,
                                        l1_node->numq);
                            }
                        }    
                    } else {
                        _bcm_kt2_node_index_set(list, queue_node->hw_index, 1);
                    }
                    if (l1_node->numq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_kt2_cosq_max_nodes_get(unit, queue_node->level, 
                                                            &max_numq));
                        l1_node->numq = max_numq;
                    }
                }
                break;

            default:
                break;
        }
        if (GET_FIELD(NODE_NUMQ_MAX, node_data)) {
            node->numq  = -1;
        }
    }
    /* Update reference counts for DSCP_TABLE profile*/
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &all_pbmp));
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &all_pbmp);
    }
#endif


    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_kt2_cos_map_profile[unit];
    PBMP_ITER(all_pbmp, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }
    /* Update EGR_COS_MAP memory profile reference counter */
    profile_mem = _bcm_kt2_egr_cos_map_profile[unit];
    PBMP_ITER(all_pbmp, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EGR_PORTm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, EGR_PORTm, &entry, EGR_COS_MAP_SELf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }

    /* Update ING_QUEUE_OFFSET_MAPPING_TABLE memory profile reference counter */
    for (i = 0; i < soc_mem_index_count(unit, SERVICE_COS_MAPm); i++) {
         SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, SERVICE_COS_MAPm, MEM_BLOCK_ALL, i, &entry));
        if (!soc_mem_field32_get(unit, SERVICE_COS_MAPm, &entry, VALIDf)) {
            continue;
        }
        set_index = soc_mem_field32_get(unit, SERVICE_COS_MAPm,
                                        &entry, QUEUE_OFFSET_PROFILE_INDEXf);
        SOC_IF_ERROR_RETURN
            (_bcm_offset_map_table_entry_reference(unit, set_index * 16, 16));
    }

    cpu_hg_index = SOC_INFO(unit).cpu_hg_pp_port_index;
    profile_mem = _bcm_kt2_cos_map_profile[unit];
    if (cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL,
                    cpu_hg_index, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }

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

    /* Update MMU_WRED_DROP_CURVE_PROFILE_x memory profile reference counter */
    PBMP_PORT_ITER(unit, port) {

        /*
         * Wred data can be in two tables depending on packet based or byte based
         * we scan both and update the software profile
         */
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, port, -1,
                                         _BCM_KT2_COSQ_INDEX_STYLE_WRED, NULL,
                                         NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve(unit, port, cosq,
                                             _BCM_KT2_COSQ_INDEX_STYLE_WRED,
                                             NULL, &index, NULL));
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_update_wred_profile_ref_count(unit, index));
        }
    }

    /* Restoring the cosq_map */
    PBMP_ITER(all_pbmp, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        node = NULL;

        /* root node */
        port_info = &mmu_info->sched_node[port];
        if (port_info->gport >= 0) {
            /* Should not be checking for return value, as we are check for node not NULL */
            /* coverity[unchecked_value] */
            (void)_bcm_kt2_cosq_node_get(unit, port_info->gport, NULL, NULL, NULL, &node);
            if (node != NULL) {
                SOC_IF_ERROR_RETURN (
                        _bcm_kt2_cosq_map_recover_next_level_node(node));
            }
        }
    }

    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        ptr += sizeof(uint32);
        for (i = 0; i < 4; i++) {
            mmu_info->curr_merger_index[i] = *(((uint32 *)ptr));
            ptr += sizeof(uint32);
        }
        PBMP_ALL_ITER(unit, port) {
            if (!(IS_HG_PORT(unit, port)|| IS_HL_PORT(unit, port))) {
                continue;
            }
            port_node = &mmu_info->sched_node[port];

            if (port_node->cosq_attached_to < 0) {
                continue;
            }

            s0_node = port_node->child;
            if (s0_node != NULL && 
                    s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                continue;
            } else {
                for (l0_node = port_node->child; l0_node != NULL;
                        l0_node = l0_node->sibling) {
                    for (l1_node = l0_node->child; l1_node != NULL;
                            l1_node = l1_node->sibling) {
                        for (l2_node = l1_node->child; l2_node != NULL;
                                l2_node = l2_node->sibling) {
                            sal_memcpy(&l2_node->fabric_pbmp, ptr, sizeof(bcm_pbmp_t));
                            ptr += sizeof(bcm_pbmp_t);
                            l2_node->remote_modid = *(((uint32 *)ptr));
                            ptr += sizeof(uint32);
                            l2_node->fabric_modid = *(((uint32 *)ptr));
                            ptr += sizeof(uint32);
                        }
                    }
                }
            }

        }
    } else {
        /* Extra memory for storing fabric_pbmp and we require
           fabric_pbmp only for l2 nodes*/ 
        additional_scache_size += (num_l2_queues[unit] * sizeof(bcm_pbmp_t));
        /* Extra memory for storing remote_modid and we require
           remote_modid only for l2 nodes*/ 
        additional_scache_size += (num_l2_queues[unit] * sizeof(uint32));
        /* Extra memory for storing fabric_modid and we require
           fabric_modid only for l2 nodes*/ 
        additional_scache_size += (num_l2_queues[unit] * sizeof(uint32));
        /* Extra memory for storing curr_merger_index */
        additional_scache_size += (sizeof(int) * 4);
        /* Extra memory forStoring the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
        additional_scache_size += sizeof(uint32);
    }
    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        ptr += sizeof(uint32);
        data = *((uint8 *) ptr);
        num_cos = GET_FIELD(NUM_COS, data);
        if (_bcm_kt2_num_cosq[unit] == 0) {
            return BCM_E_INIT;
        }
        _bcm_kt2_num_cosq[unit] = num_cos;
    } else {
        additional_scache_size += sizeof(uint8);
    }

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        ptr += sizeof(uint8);
        PBMP_ALL_ITER(unit, port) {
            port_node = &mmu_info->sched_node[port];
            if (port_node->cosq_attached_to < 0) {
                continue;
            }

            if (port_node->child == NULL) { 
                continue;  
            } 
            s0_node = (port_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ?
                port_node->child : port_node;

            do {
               if (s0_node->child) {
                   s1_node = (s0_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) ?
                   s0_node->child : s0_node;
                   do {
                      for (l0_node = s1_node->child;
                               l0_node != NULL;
                               l0_node = l0_node->sibling) {
                           for (l1_node = l0_node->child;
                                      l1_node != NULL;
                                      l1_node = l1_node->sibling) {
                                l1_node->mc_base_index = *(((uint32 *)ptr));
                                ptr += sizeof(uint32);

                           }
                      }
                      if (s1_node == s0_node) {
                          break;
                      }
                      /* It comes here
                         only in case s1 is valid */
                      s1_node = s1_node->sibling;
                   } while (s1_node != NULL);
                }
                if (s0_node == port_node) {
                    break;
                }
                /* It comes here
                   only in case s0 is valid */
                s0_node = s0_node->sibling;
            } while (s0_node != NULL);
        }
    } else {
        /* Extra memory for storing mc base index for L1 nodes*/
        additional_scache_size += (num_l1_schedulers[unit] * sizeof(uint32));
        /* Extra memory forStoring the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
        additional_scache_size += sizeof(uint32);
    }
    if (recovered_ver >= BCM_WB_VERSION_1_4) {
        /* Update PORT_COS_MAP memory profile reference counter */
        profile_mem = _bcm_kt2_ifp_cos_map_profile[unit];
        ptr += sizeof(uint32);
        for (profile_index = 0; profile_index < 4 ; profile_index++) {
            refcount = *(((uint32 *)ptr));
            while (refcount) {
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit, profile_mem, profile_index * 16, 16));
                refcount--;
            }
            ptr += sizeof(uint32);
        }
    } else {
        /* Extra memory for storing IFP_COS_MAP refcount*/
        additional_scache_size += (4 * sizeof(uint32));
        /* Extra memory forStoring the _BCM_KT2_COSQ_WB_END_NODE_VALUE */
        additional_scache_size += sizeof(uint32);
    }
    if (recovered_ver >= BCM_WB_VERSION_1_5) {
#ifdef BCM_KATANA2_SUPPORT
        /* flex counter warm boot support for saber2*/ 
        if (SOC_IS_KATANA2(unit)) {
            for (index =0 ; index < BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED; index++) {
                stat_array[index] = *(((uint32 *)ptr));
                ptr += sizeof(uint32);
            }
            bcm_kt2_cosq_stat_config_set(unit, BCM_SB2_COSQ_FLEX_COUNT_SUPPORTED,
                    stat_array);
        } 
        else {
            ptr += (sizeof(uint32) *3);
        }
#else
        ptr += (sizeof(uint32) *3);
#endif
    }
    else {
        /*Extra memory for cosq stat config*/
        additional_scache_size += (sizeof(bcm_cosq_stat_t)* 3);        
    } 

    /*populating node type and wrr mode to the node from scache*/
    if (recovered_ver >= BCM_WB_VERSION_1_6) {
        PBMP_ALL_ITER(unit, port) {
            port_node = &mmu_info->sched_node[port];
            if (port_node->cosq_attached_to < 0) {
                continue;
            }
            /*copy the node type to the node*/
            _BCM_KT2_NODE_TYPE_WRR_GET(ptr, port_node);
            if (recovered_ver >= BCM_WB_VERSION_1_8) {
                _BCM_KT2_NODE_BW_BURST_CAL_GET(ptr, port_node);
            }
            ptr += sizeof(uint8);
            s0_node = port_node->child;
            if (s0_node != NULL && 
                    s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                continue;
            } else {
                for ( l0_node = port_node->child; l0_node != NULL;
                        l0_node = l0_node->sibling) {
                    /*copy the node type to the node*/
                    _BCM_KT2_NODE_TYPE_WRR_GET(ptr, l0_node);
                    if (recovered_ver >= BCM_WB_VERSION_1_8) {
                        _BCM_KT2_NODE_BW_BURST_CAL_GET(ptr, port_node);
                    }
                    ptr += sizeof(uint8);
                    for (l1_node = l0_node->child; l1_node != NULL;
                            l1_node = l1_node->sibling) {
                        /*copy the node type to the node*/
                        _BCM_KT2_NODE_TYPE_WRR_GET(ptr, l1_node);
                        if (recovered_ver >= BCM_WB_VERSION_1_8) {
                            _BCM_KT2_NODE_BW_BURST_CAL_GET(ptr, port_node);
                        }
                        ptr += sizeof(uint8);
                        for (l2_node = l1_node->child; l2_node != NULL;
                                l2_node = l2_node->sibling) {
                            /*copy the node type to the node*/
                            _BCM_KT2_NODE_TYPE_WRR_GET(ptr,l2_node);
                            if (recovered_ver >= BCM_WB_VERSION_1_8) {
                                _BCM_KT2_NODE_WRED_DISABLE_GET(ptr,l2_node);
                                _BCM_KT2_NODE_BW_BURST_CAL_GET(ptr, port_node);
                            }
                            ptr += sizeof(uint8);
                        }
                    }
                }
            }    
        }
    } else {
        /* Extra memory for storing mc base index for L1 nodes*/
        additional_scache_size += ((num_total_schedulers[unit] + num_l2_queues[unit])  * sizeof(uint8));
    }

    if (recovered_ver >= BCM_WB_VERSION_1_7) {
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

    /* Iterating through all the ports to update the bmaps */
    for (port = CMIC_PORT(unit); port < (LB_PORT(unit)+1); port++) {
        port_node = &mmu_info->sched_node[port];
        s0_bmap = SOC_CONTROL(unit)->port_lls_s0_bmap[port];
        s1_bmap = SOC_CONTROL(unit)->port_lls_s1_bmap[port];
        l0_bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
        l1_bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
        l2_bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];

        if (port_node->cosq_attached_to < 0) {
            continue;
        }
        if (port_node->child == NULL) { 
            continue;  
        }  
        s0_node = (port_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) ?
            port_node->child : port_node;

        do {
            if (s0_node->child) {
                s1_node = (s0_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) ?
                  s0_node->child : s0_node;
                do {
                   for (l0_node = s1_node->child;
                             l0_node != NULL;
                             l0_node = l0_node->sibling) {
                        SHR_BITSET(l0_bmap, l0_node->hw_index);
                        for (l1_node = l0_node->child;
                            l1_node != NULL;
                            l1_node = l1_node->sibling) {
                            SHR_BITSET(l1_bmap, l1_node->hw_index);
                            for (l2_node = l1_node->child;
                                l2_node != NULL;
                                l2_node = l2_node->sibling) {
                                SHR_BITSET(l2_bmap, l2_node->hw_index);
                            }
                        }
                   }
                   if (s1_node == s0_node) {
                       break;
                   }
                   /* It comes here
                      only in case s1 is valid */
                      SHR_BITSET(s1_bmap, s1_node->hw_index);
                      s1_node = s1_node->sibling;
                } while (s1_node != NULL);
            }       
            if (s0_node == port_node) {
                break;
            }
            /* It comes here
               only in case s0 is valid */
            SHR_BITSET(s0_bmap, s0_node->hw_index);
            s0_node = s0_node->sibling;
        } while (s0_node != NULL);

    }
    /* Update PRIO2COS_LLFC register profile reference counter */
    profile_reg = _bcm_kt2_llfc_profile[unit];
    PBMP_PORT_ITER(unit, port) {
#ifdef BCM_METROLITE_SUPPORT
        if (SOC_IS_METROLITE(unit)) {
            mmu_port = ml_pfc_support[port].mmu_port;
        } else
#endif
#ifdef BCM_SABER2_SUPPORT 
        if (SOC_IS_SABER2(unit)) { 
            mmu_port = sb2_pfc_support[port].mmu_port;
        } else 
#endif 
        {
            mmu_port = kt2_pfc_support[port].mmu_port;
        }
        if (mmu_port == -1) {
            continue; 
        }
        reg = PORT_PFC_CFG0r;
#ifdef BCM_METROLITE_SUPPORT
        if(SOC_IS_METROLITE(unit)) {
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg,
                                 REG_PORT_ANY, 0, &rval32));
            index_map32 = soc_reg_field_get(unit, reg,
                                              rval32, PROFILE_INDEXf);
            index_map32 = index_map32 >> (mmu_port * 2);
            set_index = (index_map32 & 3);
        } else
#endif
        {
             BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg, 0, 0, &rval));

             index_map = soc_reg64_field_get(unit, reg, rval, PROFILE_INDEXf);
             COMPILER_64_SHR(index_map, ((mmu_port % 32) * 2));
             COMPILER_64_TO_32_LO(tmp32, index_map);
             set_index = (tmp32 & 3);
        }
        SOC_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, profile_reg,
                                             set_index * 16, 16));
    }
    SOC_INFO(unit).mmu_ext_buf_size =
        SOC_DDR3_NUM_MEMORIES(unit) *
        SOC_DDR3_NUM_ROWS(unit) *
        SOC_DDR3_NUM_COLUMNS(unit) *
        SOC_DDR3_NUM_BANKS(unit) /
        (1024 * 1024);
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

int
_bcm_kt2_subport_config_set(int unit)
{
    soc_info_t *si;
    bcm_port_t port;
    bcm_port_t lp_ports = 0;
    int start = 0, end = 0;
    int min_subport_index;
    int num_max_stream = 0;
    soc_pbmp_t pbmp_linkphy_one_stream_per_subport;

    si = &SOC_INFO(unit);
    min_subport_index = si->pp_port_index_min;

    num_max_stream = si->lp_streams_per_subport;

    SOC_PBMP_CLEAR(pbmp_linkphy_one_stream_per_subport);
    if (SOC_IS_SABER2(unit)) {
        pbmp_linkphy_one_stream_per_subport =
        soc_property_get_pbmp(unit, spn_PBMP_LINKPHY_ONE_STREAM_PER_SUBPORT, 0);
    }

    SOC_PBMP_ITER(si->linkphy_pbm, port) {
        /* coverity[dead_error_condition] */
        if (port >= si->lb_port) {
            /* coverity[dead_error_begin] */
            break;
        }
        start = si->port_linkphy_s1_base[port];
        end = start +
          si->port_num_subport[port] *
          (SOC_PBMP_MEMBER(pbmp_linkphy_one_stream_per_subport, port) ?
           1 : num_max_stream) - 1;
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_s1_range_set(unit, port, start, end, 1)); 
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_port_coe_linkphy_status_set(unit, port, 1));
        lp_ports++;
    }

    /*
     * setup linkphy destination stream mapping if
     * linkphy is enabled on the ports OR
     * the linkphy_allowed_pbmp is not NULL, meaning
     * there are ports on which linkphy could be
     * enabled in future.
     */
    if ((lp_ports > 0) ||
        (SOC_PBMP_NOT_NULL(si->linkphy_allowed_pbm))) {
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_stream_mapping_set(unit));
    }

    SOC_PBMP_ITER(si->subtag_pbm, port) {
        /* coverity[dead_error_condition] */
        if (port >= si->lb_port) {
            /* coverity[dead_error_begin] */
            break;
        }
        start = si->port_subport_base[port] - min_subport_index;
        end = start + si->port_num_subport[port] - 1;
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_s1_range_set(unit, port, start, end, 0)); 
        SOC_IF_ERROR_RETURN
            (soc_kt2_cosq_port_coe_linkphy_status_set(unit, port, 1));
    }

    return BCM_E_NONE;
}

int
_bcm_kt2_packing_config_set(int unit, bcm_port_t port, int numq)
{
    bcm_gport_t port_sched, l0_sched;
    bcm_gport_t l1_sched;
    bcm_gport_t cosq;
    int cos, i;
    uint32 type;
    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_add(unit, port, 1, 0, &port_sched));

    /*    coverity[uninit_use_in_call : FALSE]    */
    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_add(unit, port, numq, 
                                 BCM_COSQ_GPORT_SCHEDULER, 
                                 &l0_sched));
     BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 

    for (cos = 0; cos < numq; cos++) {
        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_add(unit, port, 2, BCM_COSQ_GPORT_SCHEDULER, 
                                    &l1_sched));
        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_attach(unit, l1_sched, l0_sched, cos));
        for (i = 0; i < 2; i++) {
            type = (i == 0) ? BCM_COSQ_GPORT_UCAST_QUEUE_GROUP :
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1, type, &cosq));
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_attach(unit, cosq, l1_sched, i)); 
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_sched_set(unit, l1_sched, i, 
                                          BCM_COSQ_WEIGHTED_ROUND_ROBIN, 1));
        }        
    }

    return BCM_E_NONE;
} 
    
int
_bcm_kt2_port_lls_config_set(int unit, 
                             bcm_port_t port, 
                             int startq, 
                             int total_queues, 
                             int numq)
{
    bcm_gport_t port_sched, l0_sched;
    bcm_gport_t l1_sched;
    bcm_gport_t cosq;
    int cos, i;
    uint32 type;
    uint32 total_opnodes = (total_queues + 7)/8; 

    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_add(unit, port, 1, 0, &port_sched));
 
    /*    coverity[uninit_use_in_call : FALSE]    */
    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_add(unit, port, total_opnodes,
                                 BCM_COSQ_GPORT_SCHEDULER, 
                                 &l0_sched));
     BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 

    for (i = 0; i < total_opnodes; i++) {
         if (IS_HG_PORT(unit, port) && soc_feature(unit, soc_feature_mmu_packing))  { 
             BCM_IF_ERROR_RETURN
             (bcm_kt2_cosq_gport_add(unit, port, numq * 2 , BCM_COSQ_GPORT_SCHEDULER, 
                                    &l1_sched));
         }
         else { 

             BCM_IF_ERROR_RETURN
             (bcm_kt2_cosq_gport_add(unit, port, numq, BCM_COSQ_GPORT_SCHEDULER, 
                                    &l1_sched));
         }

        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_attach(unit, l1_sched, l0_sched, i));
        for (cos = 0; cos < numq; cos++) {
            type = BCM_COSQ_GPORT_WITH_ID;
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(cosq, port, (startq + cos)); 
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1, type, &cosq));
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_attach(unit, cosq, l1_sched, cos)); 
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_sched_set(unit, l1_sched, cos, 
                                          BCM_COSQ_WEIGHTED_ROUND_ROBIN, 1));
         if (IS_HG_PORT(unit, port) && soc_feature(unit, soc_feature_mmu_packing))  { 
             BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1, 
                         BCM_COSQ_GPORT_MCAST_QUEUE_GROUP, &cosq));
             BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_attach(unit, cosq, l1_sched, cos + numq)); 
             BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_sched_set(unit, l1_sched, (cos + numq)  , 
                                          BCM_COSQ_WEIGHTED_ROUND_ROBIN, 1));
         }
        }
        startq += 8;
    }

    return BCM_E_NONE;
} 

int 
_bcm_kt2_create_default_lls_tree(int unit, bcm_port_t port) 
{
    soc_info_t *si;
    int numq = 0, cosq;
    bcm_gport_t port_sched, l0_sched, l1_sched, queue;

    si = &SOC_INFO(unit);
    bcm_kt2_cosq_config_get(unit, &numq);
    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }
    if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_port_lls_config_set(unit, port, 
                                          si->port_uc_cosq_base[port], 
                                          si->port_num_uc_cosq[port], 
                                          numq));
    } else {
        if (!soc_feature(unit, soc_feature_mmu_packing)) {
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1, 0, &port_sched));

            /*    coverity[uninit_use_in_call : FALSE]    */
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1, 
                                        BCM_COSQ_GPORT_SCHEDULER, 
                                        &l0_sched));
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, numq, 
                                        BCM_COSQ_GPORT_SCHEDULER, 
                                        &l1_sched));
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_attach(unit, l1_sched, l0_sched, 0));  
            for (cosq = 0; cosq < numq; cosq++) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_add(unit, port, 1,
                                            BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_attach(unit, queue, l1_sched, 
                                               cosq)); 
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_sched_set(unit, l1_sched, cosq,
                                                  BCM_COSQ_STRICT, 0)); 
            }
        }else {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_packing_config_set(unit, port, numq));
        }
    }

    return BCM_E_NONE;

}

int 
bcm_katana2_cosq_reconfigure_flexio(int unit, bcm_port_t port, int lanes)
{
   bcm_gport_t gport; 
   int i;
   BCM_IF_ERROR_RETURN
   (bcm_esw_port_gport_get(unit, port , &gport));

    /* Deleting the LLS tree */
    BCM_IF_ERROR_RETURN
        ( bcm_kt2_cosq_gport_delete(unit, gport));
   /* Creating the default LLS tree */
   BCM_IF_ERROR_RETURN
       ( _bcm_kt2_create_default_lls_tree(unit, port));
   for (i = 1; i < lanes; i++) {
        BCM_IF_ERROR_RETURN
        (bcm_esw_port_gport_get(unit, port + i , &gport));
        /* Deleting the LLS tree */
        BCM_IF_ERROR_RETURN
        ( bcm_kt2_cosq_gport_delete(unit, gport));
   }
   return BCM_E_NONE;
}

STATIC int 
_bcm_kt2_init_all_queue_counters(int unit)
{  
    uint32 max_queue = soc_mem_index_count(unit, LLS_L2_PARENTm);
    uint32 queue_index;
    uint64 val;
#ifdef BCM_KATANA2_SUPPORT
    int num_elem = 0;
    int index = 0, increment = 4;
    _sb2_cosq_counter_mem_map_t *mem_map;
    soc_reg_t non_dma_id_1 = 0, non_dma_id_2 = 0;   
    if (SOC_IS_SABER2(unit)) {
        uint16 dev_id;
        uint8 rev_id;
        increment = 2;
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (dev_id == BCM56233_DEVICE_ID) {
            increment = 1;
        }
    }
#ifdef BCM_METROLITE_SUPPORT
    if(SOC_IS_METROLITE(unit))  {
       increment = 1;
    }
#endif 
    soc_sb2_cosq_counter_mem_map_get(unit, &num_elem, &mem_map);
    COMPILER_64_ZERO(val);
    /* This API is mainly for clearing the extended queues only,
       So Looping only for queue_id > SOC_MAX_NUM_PP_PORTS */

    for ( index = 0; index < num_elem; index += increment ) {
        switch (mem_map[index].non_dma_id) {
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                non_dma_id_1 = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
                non_dma_id_2 = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
                break;
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW:
                non_dma_id_1 = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW;
                non_dma_id_2 = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_YELLOW;
                break;
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED:
                non_dma_id_1 = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED;
                non_dma_id_2 = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED;
                break;
            case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN:
                non_dma_id_1 = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN;
                non_dma_id_2 = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_GREEN;
                break;
            case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
                non_dma_id_1 = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
                non_dma_id_2 = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
                break;
            default:
                break;
        }
        for (queue_index = soc_max_num_pp_ports[unit] + 1;
                queue_index < max_queue; queue_index++) {
            BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                        non_dma_id_1, 0, val));
            BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                        non_dma_id_2, 0, val));
        }
    }
#else
     COMPILER_64_ZERO(val);
    /* This API is mainly for clearing the extended queues only,
       So Looping only for queue_id > SOC_MAX_NUM_PP_PORTS */
    for (queue_index = SOC_MAX_NUM_PP_PORTS + 1;
            queue_index < max_queue; queue_index++) {
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, 0, val));
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, 0, val));
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED, 0, val));
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_RED, 0, val));
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, 0, val));
        BCM_IF_ERROR_RETURN(soc_counter_set(unit, queue_index, 
                    SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, 0, val));
    }     

#endif
    return BCM_E_NONE;
}
int
bcm_kt2_init_fc_map_tbl(int unit)
{
    int max_index;
    int max_nodes;
    uint32 map_entry[SOC_MAX_MEM_WORDS];
    int mem;
    int index;
    int field ;
    static const soc_field_t indexf[] = {
        INDEX0f, INDEX1f 
    };
    static const soc_mem_t memx[] = {
        MMU_INTFI_FC_MAP_TBL0m,
        MMU_INTFI_FC_MAP_TBL1m,
        MMU_INTFI_FC_MAP_TBL2m
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
 *     bcm_kt2_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_init(int unit)
{
    soc_info_t *si;
    STATIC int _kt2_max_cosq = -1;
    int cosq, numq, alloc_size, ext_pbmp_count;
    bcm_port_t port;
    soc_reg_t mem;
    soc_pbmp_t pbmp_linkphy, ext_pbmp;
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
    uint64 rval64s[16], *prval64s[1];
    uint32 profile_index,rval;
    _bcm_kt2_mmu_info_t *mmu_info_p;
    int i, cmc;
    bcm_gport_t port_sched, l0_sched, l1_sched, queue;
    uint32 max_threshold, min_threshold;
    int num_queues, num_inputs;
    int num_base_queues = 0, total_sub_queues = 0;
    int start_pp_port = 0, end_pp_port = 0;
    int pre_alloc_queues = 0;
    int pp_port_cos = 0;
    uint32 qbase = 0;
    uint32 qoffset = 0;
    soc_reg_t reg;

    /* Initialize static variables */
    num_pp_ports[unit]=-1;
    num_port_schedulers[unit]=-1;
    num_s0_schedulers[unit]=-1;
    num_s1_schedulers[unit]=-1;
    num_l0_schedulers[unit]=-1;
    num_l1_schedulers[unit]=-1;
    num_total_schedulers[unit]=-1;
    num_l2_queues[unit]=-1;
    max_l1_shaper_bucket[unit]=-1;
    max_l2_shaper_bucket[unit]=-1;
    soc_max_num_pp_ports[unit] = SOC_MAX_NUM_PP_PORTS;

    if (_kt2_max_cosq < 0) {
        _kt2_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }
 
    if (soc_property_get(unit, spn_MMU_MULTI_PACKETS_PER_CELL, 0)) {
        ext_pbmp = soc_property_get_pbmp(unit, spn_PBMP_EXT_MEM, 0);
        SOC_PBMP_COUNT(ext_pbmp, ext_pbmp_count); 
        if (ext_pbmp_count == 0){
            LOG_ERROR(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "With packing mode set you need have"
                             "the pbmp_ext_mem bitmap for the ports"
                             "for those requires external memory \n")));
            return BCM_E_CONFIG; 
        }
    }

    BCM_IF_ERROR_RETURN (bcm_kt2_cosq_detach(unit, 0));

    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    /* Create profile for PORT_COS_MAP table */
    if (_bcm_kt2_cos_map_profile[unit] == NULL) {
        _bcm_kt2_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_kt2_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt2_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_kt2_cos_map_profile[unit]));

    /* Create profile for EGR_COS_MAP table */
    if (_bcm_kt2_egr_cos_map_profile[unit] == NULL) {
        _bcm_kt2_egr_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "EGR_COS_MAP Profile Mem");
        if (_bcm_kt2_egr_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt2_egr_cos_map_profile[unit]);
    }
    mem = EGR_COS_MAPm;
    entry_words[0] = sizeof(egr_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_kt2_egr_cos_map_profile[unit]));
    
    /* Create profile for PRIO2COS_LLFC register */
    if (_bcm_kt2_llfc_profile[unit] == NULL) {
        _bcm_kt2_llfc_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "PRIO2COS_LLFC Profile Reg");
        if (_bcm_kt2_llfc_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_kt2_llfc_profile[unit]);
    }
    reg = PRIO2COS_PROFILEr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1, _bcm_kt2_llfc_profile[unit]));

    /* Inititilise pg profile refcount */
#if defined(BCM_SABER2_SUPPORT)
    BCM_IF_ERROR_RETURN(bcm_kt2_pg_profile_refcount_init(unit));
#endif
    /* Create profile for IFP_COS_MAP table. */
    if (_bcm_kt2_ifp_cos_map_profile[unit] == NULL) {
        _bcm_kt2_ifp_cos_map_profile[unit]
            = sal_alloc(sizeof(soc_profile_mem_t), "IFP_COS_MAP Profile Mem");
        if (_bcm_kt2_ifp_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt2_ifp_cos_map_profile[unit]);
    }
    mem = IFP_COS_MAPm;
    entry_words[0] = sizeof(ifp_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_kt2_ifp_cos_map_profile[unit]));    

    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_kt2_wred_profile[unit] == NULL) {
        _bcm_kt2_wred_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                               "WRED Profile Mem");
        if (_bcm_kt2_wred_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_kt2_wred_profile[unit]);
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
                                               _bcm_kt2_wred_profile[unit]));

    alloc_size = sizeof(_bcm_kt2_mmu_info_t) * 1;
    if (_bcm_kt2_mmu_info[unit] == NULL) {
        _bcm_kt2_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_kt2_mmu_info");

        if (_bcm_kt2_mmu_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        _bcm_kt2_cosq_free_memory(_bcm_kt2_mmu_info[unit]);
    }

    sal_memset(_bcm_kt2_mmu_info[unit], 0, alloc_size);

    mmu_info_p = _bcm_kt2_mmu_info[unit];

    /* ======================== */
    /* Time Being commenting below part under if 0 */
    /* ======================== */
    /*
       if (soc_feature(unit, soc_feature_ddr3)) {
       mmu_info_p->num_queues = soc_mem_index_count(unit, LLS_L2_PARENTm);
       } else {

       mmu_info_p->num_queues = 1024;
       }
     */
    mmu_info_p->num_queues = soc_mem_index_count(unit, LLS_L2_PARENTm);
    
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_ROOT] = 
                                  soc_mem_index_count(unit, LLS_PORT_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_S0]   = 
                                  soc_mem_index_count(unit, LLS_S1_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_S1]   = 
                                  2 * soc_mem_index_count(unit, LLS_S1_CONFIGm);
    if (SOC_IS_SABER2(unit)) {
        mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_S1]   = 
            soc_mem_index_count(unit, LLS_S1_CONFIGm);
#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            /* Allocate additional nodes equal to amount of physical nodes.
             * This is to hold the subport nodes which are reused with physical nodes */
            mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_S1] += SOC_INFO(unit).cpu_hg_index;
        }
#endif
    }
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_L0]   = 
                                  soc_mem_index_count(unit, LLS_L0_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_L1]   = 
                                  soc_mem_index_count(unit, LLS_L1_CONFIGm);
    mmu_info_p->max_nodes[_BCM_KT2_COSQ_NODE_LEVEL_L2]   = 
                                  mmu_info_p->num_queues;

    mmu_info_p->num_nodes = 0;


    for (i= 0; i < _BCM_KT2_COSQ_NODE_LEVEL_L2; i++) {
        mmu_info_p->num_nodes += mmu_info_p->max_nodes[i];
    }

    si = &SOC_INFO(unit);

    num_pp_ports[unit] =  si->cpu_hg_pp_port_index;
    num_port_schedulers[unit] = si->cpu_hg_index;
#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        num_pp_ports[unit] += num_port_schedulers[unit];
    }
#endif

    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);

    if (SOC_PBMP_NOT_NULL(pbmp_linkphy)) {
#ifdef BCM_METROLITE_SUPPORT
        if(SOC_IS_METROLITE(unit))  {
           /* Metrolite doesn't have s0 schedulers */
           num_s0_schedulers[unit] = 0;
        } else
#endif
        {
           num_s0_schedulers[unit] = soc_mem_index_count(unit, LLS_S0_SHAPER_CONFIG_Cm);
        }
        num_s1_schedulers[unit] = soc_mem_index_count(unit, LLS_S1_SHAPER_CONFIG_Cm);
    } else {
        num_s0_schedulers[unit] = 0;
        num_s1_schedulers[unit] = 0;
    }
    num_l0_schedulers[unit] = soc_mem_index_count(unit,LLS_L0_PARENTm);
    num_l1_schedulers[unit] = soc_mem_index_count(unit,LLS_L1_PARENTm);;
    num_l2_queues[unit]     = soc_mem_index_count(unit,LLS_L2_PARENTm);
    max_l1_shaper_bucket[unit] = num_l1_schedulers[unit] /
                                   soc_mem_index_count(unit,
                                   LLS_L1_SHAPER_CONFIG_Cm);
    if (SOC_IS_SABER2(unit)) {
        max_l2_shaper_bucket[unit] = num_l2_queues[unit] /
                                   soc_mem_index_count(unit,
                                   LLS_L2_SHAPER_CONFIG_Cm);
        l2_shaper_memory[unit][0] = LLS_L2_SHAPER_CONFIG_Cm;
        l2_shaper_memory[unit][1] = LLS_L2_MIN_CONFIG_Cm;
        l2_shaper_memory[unit][2] = INVALIDm;
        l2_shaper_memory[unit][3] = INVALIDm;
        soc_max_num_pp_ports[unit] = num_pp_ports[unit];
    } else {
        max_l2_shaper_bucket[unit] = num_l2_queues[unit] /
                                   soc_mem_index_count(unit,
                                   LLS_L2_SHAPER_CONFIG_LOWERm);
        l2_shaper_memory[unit][0] = LLS_L2_SHAPER_CONFIG_LOWERm;
        l2_shaper_memory[unit][1] = LLS_L2_MIN_CONFIG_LOWER_Cm;
        l2_shaper_memory[unit][2] = LLS_L2_SHAPER_CONFIG_UPPERm;
        l2_shaper_memory[unit][3] = LLS_L2_MIN_CONFIG_UPPER_Cm;
    }

    num_total_schedulers[unit] = mmu_info_p->num_nodes;
    LOG_VERBOSE(BSL_LS_BCM_COSQ,
              (BSL_META_U(unit,
                          "Useful Info:\n"
                          "num_pp_ports[unit]:%d num_port_schedulers[unit]:%d num_s0_schedulers[unit]:%d\n"
                          "num_s1_schedulers[unit]:%d num_l0_schedulers[unit]:%d num_l1_schedulers[unit]:%d\n"
                          "num_l2_queues[unit]:%d num_total_schedulers[unit]:%d\n"),
               num_pp_ports[unit], num_port_schedulers[unit], num_s0_schedulers[unit],
               num_s1_schedulers[unit], num_l0_schedulers[unit], num_l1_schedulers[unit],
               num_l2_queues[unit], num_total_schedulers[unit]));

    mmu_info_p->port = _bcm_kt2_cosq_alloc_clear((soc_max_num_pp_ports[unit] *
                                            sizeof(_bcm_kt2_cosq_port_info_t)),
                                            "port_info");
    if (mmu_info_p->port == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->sched_node = _bcm_kt2_cosq_alloc_clear(((num_total_schedulers[unit]) *
                                            sizeof(_bcm_kt2_cosq_node_t)),
                                            "sched_node");
    if (mmu_info_p->sched_node == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->queue_node = _bcm_kt2_cosq_alloc_clear(((num_l2_queues[unit]) *
                                            sizeof(_bcm_kt2_cosq_node_t)),
                                            "queue_node");
    if (mmu_info_p->queue_node == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->global_qlist.bits  = _bcm_kt2_cosq_alloc_clear(
                                  SHR_BITALLOCSIZE(mmu_info_p->num_queues),
                                   "global_qlist");
    if (mmu_info_p->global_qlist.bits == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }
    
    mmu_info_p->l2_global_qlist.bits  = _bcm_kt2_cosq_alloc_clear(
                                  SHR_BITALLOCSIZE(mmu_info_p->num_queues),
                                   "l2_global_qlist");
    if (mmu_info_p->l2_global_qlist.bits == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }
    for (port = 0; port < (si->lb_port + 1); port++) {

        if (SOC_IS_SABER2(unit) && (!SOC_IS_METROLITE(unit)) &&
            soc_saber2_port_is_unused(unit, port)) {
            continue;
        }

        SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
        if(SOC_WARM_BOOT(unit) && !(IS_LB_PORT(unit, port))) {
           /* In case of warmboot the queue base is restored 
            * from the hardware.
            */ 
           qbase = soc_reg_field_get(unit, ING_COS_MODEr, rval,
                                         BASE_QUEUE_NUMf);
           mmu_info_p->port[port].q_offset =qbase;
           
           i = soc_reg_field_get(unit, ING_COS_MODEr, rval,
                                 SERVICE_BASE_QUEUE_NUMf);
           mmu_info_p->port[port].uc_vlan_base = i ? i :
                                                 mmu_info_p->num_queues -1;

           if (IS_HG_PORT(unit, port)) {
               qoffset = KT2_HG_NUM_PHYSICAL_QUEUES ; 
           
           } else if (IS_CPU_PORT(unit,port)) { 
                      qoffset = KT2_CPU_NUM_PHYSICAL_QUEUES ;
           } else {
                      qoffset = KT2_DEFAULT_NUM_PHYSICAL_QUEUES;
           }
           mmu_info_p->port[port].q_limit = qbase + qoffset; 
           si->port_uc_cosq_base[port] =  mmu_info_p->port[port].q_offset ; 
           si->port_num_uc_cosq[port] = qoffset;
        }
        else {  
           mmu_info_p->port[port].q_offset = si->port_uc_cosq_base[port];
            mmu_info_p->port[port].q_limit = si->port_uc_cosq_base[port] + 
                                         si->port_num_uc_cosq[port];
        } 
        /* reserve the queues for the ports from the global bitmap */ 
        BCM_IF_ERROR_RETURN
         (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                 mmu_info_p->port[port].q_offset,
                                 (mmu_info_p->port[port].q_limit -
                                 mmu_info_p->port[port].q_offset)));
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                 mmu_info_p->port[port].q_offset,
                                 (mmu_info_p->port[port].q_limit -
                                 mmu_info_p->port[port].q_offset)));


        soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                      mmu_info_p->port[port].q_offset);
        if (IS_LB_PORT(unit, port)) {
            /* Loopback port base starts from 64 */
            soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                              _BCM_KT2_LB_BASE_QUEUE);
        }
        if (IS_HG_PORT(unit, port)) {
            soc_reg_field_set(unit, ING_COS_MODEr, &rval, COS_MODEf, 2);  
        }
        SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));
        
        if (si->port_num_subport[port] > 0) {
            start_pp_port = si->port_subport_base[port];
            end_pp_port = si->port_subport_base[port] +
            si->port_num_subport[port];

#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), i) {
                    SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, i, &rval));
                    if(SOC_WARM_BOOT(unit)) { 
                    qbase =soc_reg_field_get(unit,ING_COS_MODEr, rval,
                                                            BASE_QUEUE_NUMf);
                    mmu_info_p->port[i].q_offset =qbase;
                    mmu_info_p->port[i].q_limit = qbase + 8 ; 
                    si->port_uc_cosq_base[i] = mmu_info_p->port[i].q_offset;
                    si->port_num_uc_cosq[i] = qoffset ;
                    }
                    else {  
                    mmu_info_p->port[i].q_offset = si->port_uc_cosq_base[i];
                    mmu_info_p->port[i].q_limit = si->port_uc_cosq_base[i] +
                                                si->port_num_uc_cosq[i];
                    } 
            
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                    mmu_info_p->port[i].q_offset,
                                    (mmu_info_p->port[i].q_limit -
                                    mmu_info_p->port[i].q_offset)));
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                    mmu_info_p->port[i].q_offset,
                                    (mmu_info_p->port[i].q_limit -
                                    mmu_info_p->port[i].q_offset)));

                    soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                                    mmu_info_p->port[i].q_offset);
                    SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, i, rval)); 
                }
            } else 
#endif
            {
                for (i = start_pp_port; i < end_pp_port; i++) {
                    SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, i, &rval));
                    if(SOC_WARM_BOOT(unit)) { 
                    qbase =soc_reg_field_get(unit,ING_COS_MODEr, rval,
                                                            BASE_QUEUE_NUMf);
                    mmu_info_p->port[i].q_offset =qbase;
                    mmu_info_p->port[i].q_limit = qbase + 8 ; 
                    si->port_uc_cosq_base[i] = mmu_info_p->port[i].q_offset;
                    si->port_num_uc_cosq[i] = qoffset ;
                    }
                    else {  
                    mmu_info_p->port[i].q_offset = si->port_uc_cosq_base[i];
                    mmu_info_p->port[i].q_limit = si->port_uc_cosq_base[i] +
                                                si->port_num_uc_cosq[i];
                    } 
            
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                    mmu_info_p->port[i].q_offset,
                                    (mmu_info_p->port[i].q_limit -
                                    mmu_info_p->port[i].q_offset)));
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                    mmu_info_p->port[i].q_offset,
                                    (mmu_info_p->port[i].q_limit -
                                    mmu_info_p->port[i].q_offset)));

                    soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                                    mmu_info_p->port[i].q_offset);
                    SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, i, rval)); 
                }
            }
        }

        SOC_IF_ERROR_RETURN(READ_RQE_PP_PORT_CONFIGr(unit, port, &rval));

        if (soc_feature(unit, soc_feature_mmu_packing)) {
            if(SOC_WARM_BOOT(unit)) { 
               qbase =  soc_reg_field_get(unit, RQE_PP_PORT_CONFIGr, 
                                                   rval, BASE_QUEUEf);
               mmu_info_p->port[port].mcq_offset =qbase ;
               if (IS_HG_PORT(unit, port)) {
                   qoffset = KT2_HG_NUM_PHYSICAL_QUEUES ; 
               } else if (IS_CPU_PORT(unit, port)) { 
                          qoffset = KT2_CPU_NUM_PHYSICAL_QUEUES ;
               } else {
                          qoffset = KT2_DEFAULT_NUM_PHYSICAL_QUEUES;
               }
               mmu_info_p->port[port].mcq_limit = qbase + qoffset;  
               si->port_cosq_base[port] = mmu_info_p->port[port].mcq_offset;
               si->port_num_cosq[port] =  qoffset;
           }
           else { 
                   mmu_info_p->port[port].mcq_offset = si->port_cosq_base[port];
                   mmu_info_p->port[port].mcq_limit = si->port_cosq_base[port] +
                                                      si->port_num_cosq[port];                                         
           } 
            BCM_IF_ERROR_RETURN
            (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                 mmu_info_p->port[port].mcq_offset,
                                 (mmu_info_p->port[port].mcq_limit -
                                 mmu_info_p->port[port].mcq_offset)));
            BCM_IF_ERROR_RETURN
            (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                 mmu_info_p->port[port].mcq_offset,
                                 (mmu_info_p->port[port].mcq_limit -
                                 mmu_info_p->port[port].mcq_offset)));
        }
        soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                          (soc_feature(unit, soc_feature_mmu_packing)) ? 
                          mmu_info_p->port[port].mcq_offset :
                          mmu_info_p->port[port].q_offset);

        if (IS_EXT_MEM_PORT(unit, port)) {
            if (SOC_REG_FIELD_VALID(unit, RQE_PP_PORT_CONFIGr, BUFF_TYPEf)) {
                soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                                  &rval, BUFF_TYPEf, 1);
            }
        }    
        if (IS_LB_PORT(unit, port)) {
            soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                              _BCM_KT2_LB_BASE_QUEUE);
        }
        if (IS_HG_PORT(unit, port)) {
            soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                              &rval, COS_MODEf, 2);
        }   
        SOC_IF_ERROR_RETURN(WRITE_RQE_PP_PORT_CONFIGr(unit, port, rval));

        if (si->port_num_subport[port] > 0) {
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, port), i) {
                    soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                                    (soc_feature(unit, soc_feature_mmu_packing)) ?
                                    mmu_info_p->port[port].mcq_offset :
                                    mmu_info_p->port[i].q_offset);

                    SOC_IF_ERROR_RETURN(WRITE_RQE_PP_PORT_CONFIGr(unit, i, rval)); 
                }
            } else 
#endif
            {
                for (i = start_pp_port; i < end_pp_port; i++) {
                    soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                                    (soc_feature(unit, soc_feature_mmu_packing)) ?
                                    mmu_info_p->port[port].mcq_offset :
                                    mmu_info_p->port[i].q_offset);

                    SOC_IF_ERROR_RETURN(WRITE_RQE_PP_PORT_CONFIGr(unit, i, rval)); 
                }
            }
        }
        
        rval = 0;
        soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, BASE_QUEUEf,
                          mmu_info_p->port[port].q_offset);
        if (IS_EXT_MEM_PORT(unit, port)) {
            soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval, 
                              BUFF_TYPEf, 1);  
        }    
        if (!IS_LB_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(WRITE_OP_E2ECC_PORT_CONFIGr(unit, port, rval));
        }
        for (i = mmu_info_p->port[port].q_offset;
             i < mmu_info_p->port[port].q_limit; i++) {
            SOC_IF_ERROR_RETURN (soc_mem_field32_modify(unit,
                    EGR_QUEUE_TO_PP_PORT_MAPm, i, PP_PORTf, port));
        }

        if (soc_feature(unit, soc_feature_mmu_packing)) {
            for (i = mmu_info_p->port[port].mcq_offset;
                 i < mmu_info_p->port[port].mcq_limit; i++) {
                SOC_IF_ERROR_RETURN (soc_mem_field32_modify(unit,
                              EGR_QUEUE_TO_PP_PORT_MAPm, i, PP_PORTf, port));
            }
        }
        if (!IS_LB_PORT(unit, port)) {
            num_base_queues = mmu_info_p->port[port].q_limit;
        }
        if(!SOC_WARM_BOOT(unit)) {
           mmu_info_p->port[port].uc_vlan_base = mmu_info_p->num_queues - 1;
        }
    }

    /* mmu_info-> num_base_queues is the maximum number of queue configured 
     * or available with the system */
    mmu_info_p->num_base_queues = mmu_info_p->num_queues; 

    mmu_info_p->num_dmvoq_queues = soc_property_get(unit, 
                                                   spn_MMU_NUM_DMVOQ_QUEUES, 
                                                   0);

    /* Config variable defining
     * the number of pre reserved subscriber queues
     * spn_MMU_NUM_SUBSCRIBER_QUEUES
     */

    mmu_info_p->num_sub_queues = soc_property_get(unit,
                                                   spn_MMU_NUM_SUBSCRIBER_QUEUES,
                                                   0);

    mmu_info_p->num_ext_queues  = mmu_info_p->num_queues ; 
                                  

    /* Pre reserved queues include DVMOQ and subscriber queues
     * from extended queues
     */
    pre_alloc_queues = mmu_info_p->num_dmvoq_queues +
                       mmu_info_p->num_sub_queues;


    if (pre_alloc_queues > mmu_info_p->num_queues  ) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    mmu_info_p->base_dmvoq_queue =  (num_base_queues + 7) & 0xfffffff8;
    
    /* Check if number of dmvoq queue configured is not crossing limit */
    if((mmu_info_p->base_dmvoq_queue + 
        mmu_info_p->num_dmvoq_queues) > num_l2_queues[unit]) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "Configured value of number of DMVOQ"
                             " queue %d more than allowed number %d\n"),
                             mmu_info_p->num_dmvoq_queues,
                             num_l2_queues[unit] - mmu_info_p->base_dmvoq_queue));
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_CONFIG; 
    }

    BCM_IF_ERROR_RETURN
    (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                             mmu_info_p->base_dmvoq_queue,
                             mmu_info_p->num_dmvoq_queues 
                            ));
   BCM_IF_ERROR_RETURN
   (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                             mmu_info_p->base_dmvoq_queue,
                             mmu_info_p->num_dmvoq_queues 
                            ));

    mmu_info_p->qset_size       = soc_property_get(unit, 
                                              spn_MMU_SUBSCRIBER_NUM_COS_LEVEL, 
                                              1);

    mmu_info_p->l1_gport_pair = _bcm_kt2_cosq_alloc_clear(
                                 (mmu_info_p->num_nodes * sizeof(bcm_gport_t)),
                                 "l1_pair_info");
    if (mmu_info_p->l1_gport_pair == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }


    if (mmu_info_p->num_dmvoq_queues > 0) {
        mmu_info_p->dmvoq_qlist.bits  = _bcm_kt2_cosq_alloc_clear(
                SHR_BITALLOCSIZE(mmu_info_p->num_dmvoq_queues),
                "dmvoq_qlist");
        if (mmu_info_p->dmvoq_qlist.bits == NULL) {
            _bcm_kt2_cosq_free_memory(mmu_info_p);
            return BCM_E_MEMORY;
        }
    }


    mmu_info_p->sched_list.bits = _bcm_kt2_cosq_alloc_clear(
                                  SHR_BITALLOCSIZE(mmu_info_p->num_nodes),
                                  "sched_list");
    if (mmu_info_p->sched_list.bits == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }

    for (i = _BCM_KT2_COSQ_NODE_LEVEL_S0; i < _BCM_KT2_COSQ_NODE_LEVEL_L2; 
              i++) { 
        mmu_info_p->sched_hw_list[i].bits = _bcm_kt2_cosq_alloc_clear(
                                            SHR_BITALLOCSIZE
                                            (mmu_info_p->max_nodes[i]),
                                            "sched_hw_list");
        if (mmu_info_p->sched_hw_list[i].bits == NULL) {
            _bcm_kt2_cosq_free_memory(mmu_info_p);
            return BCM_E_MEMORY;
        }
    }

    mmu_info_p->l2_base_qlist.bits = _bcm_kt2_cosq_alloc_clear(
                                     SHR_BITALLOCSIZE
                                     (mmu_info_p->num_queues),
                                     "l2_base_qlist");
    if (mmu_info_p->l2_base_qlist.bits == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }
    
    mmu_info_p->l2_base_mcqlist.bits = _bcm_kt2_cosq_alloc_clear(
                                     SHR_BITALLOCSIZE
                                     (mmu_info_p->num_queues),
                                     "l2_base_mcqlist");
    if (mmu_info_p->l2_base_mcqlist.bits == NULL) {
        _bcm_kt2_cosq_free_memory(mmu_info_p);
        return BCM_E_MEMORY;
    }


    if (mmu_info_p->num_dmvoq_queues > 0) {
        mmu_info_p->l2_dmvoq_qlist.bits = _bcm_kt2_cosq_alloc_clear(
                SHR_BITALLOCSIZE
                (mmu_info_p->num_dmvoq_queues),
                "l2_dmvoq_qlist");
        if (mmu_info_p->l2_dmvoq_qlist.bits == NULL) {
            _bcm_kt2_cosq_free_memory(mmu_info_p);
            return BCM_E_MEMORY;
        }
    }

    for (i = 0; i < num_total_schedulers[unit]; i++) {
        _BCM_KT2_COSQ_LIST_NODE_INIT(mmu_info_p->sched_node, i);
    }

    for (i = 0; i < num_l2_queues[unit]; i++) {
        _BCM_KT2_COSQ_LIST_NODE_INIT(mmu_info_p->queue_node, i);
    }

    /* reserve last entry of S0, S1, L0 and L1 nodes */
    for (i = _BCM_KT2_COSQ_NODE_LEVEL_S0; i < _BCM_KT2_COSQ_NODE_LEVEL_L2; 
             i++) { 
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_node_index_set(&mmu_info_p->sched_hw_list[i], 
            (mmu_info_p->max_nodes[i] - 1), 1));
    }

    if (mmu_info_p->num_sub_queues) {
       if (mmu_info_p->num_dmvoq_queues) {
            /* Pre reserved DVMOQ
             * Revisit as DVMOQ used the indices in multiples of 8
             * hence used shift
             */
            mmu_info_p->base_sub_queue = (mmu_info_p->base_dmvoq_queue +
                                         mmu_info_p->num_dmvoq_queues);

        }
        else {
            mmu_info_p->base_sub_queue = num_base_queues;
        }
        /* Check if number of sub queue configured is not crossing limit.
         * Config variable mmu_num_subscriber_queues used to configure this */
        total_sub_queues = mmu_info_p->base_sub_queue +
                                                mmu_info_p->num_sub_queues;
        if (total_sub_queues > (soc_feature(unit, soc_feature_mmu_packing) ?
                                num_l2_queues[unit]/2 : num_l2_queues[unit] )) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Configured value of number of subscriber"
                                " queue %d more than allowed number %d\n"),
                     mmu_info_p->num_sub_queues,
                     ((soc_feature(unit, soc_feature_mmu_packing) ?
                      num_l2_queues[unit] / 2 : num_l2_queues[unit]) - mmu_info_p->base_sub_queue)));
            _bcm_kt2_cosq_free_memory(mmu_info_p);
            return BCM_E_CONFIG;
        }

        /*setting base sub queue as multiple of 8 same as DMVOQ*/
        mmu_info_p->base_sub_queue =  (mmu_info_p->base_sub_queue + 7) & 0xfffffff8;
        
        mmu_info_p->sub_qlist.bits  = _bcm_kt2_cosq_alloc_clear(
             SHR_BITALLOCSIZE(mmu_info_p->num_sub_queues),
             "sub_qlist");
        if (mmu_info_p->sub_qlist.bits == NULL) {
            _bcm_kt2_cosq_free_memory(mmu_info_p);
            return BCM_E_MEMORY;
        }

        mmu_info_p->l2_sub_qlist.bits = _bcm_kt2_cosq_alloc_clear(
                 SHR_BITALLOCSIZE
                 (mmu_info_p->num_sub_queues),
                 "l2_sub_qlist");

        if (mmu_info_p->l2_sub_qlist.bits == NULL) {
             _bcm_kt2_cosq_free_memory(mmu_info_p);
             return BCM_E_MEMORY;
        }

        BCM_IF_ERROR_RETURN
         (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                  mmu_info_p->base_sub_queue, 
                                  mmu_info_p->num_sub_queues));
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                  mmu_info_p->base_sub_queue,
                                  mmu_info_p->num_sub_queues));
        if (soc_feature(unit, soc_feature_mmu_packing)) {
            mmu_info_p->sub_mcqlist.bits  = _bcm_kt2_cosq_alloc_clear(
            SHR_BITALLOCSIZE(mmu_info_p->num_sub_queues),
            "sub_mcqlist");
            if (mmu_info_p->sub_mcqlist.bits == NULL) {
                _bcm_kt2_cosq_free_memory(mmu_info_p);
                return BCM_E_MEMORY;
            }

            mmu_info_p->l2_sub_mcqlist.bits = _bcm_kt2_cosq_alloc_clear(
                 SHR_BITALLOCSIZE
                 (mmu_info_p->num_sub_queues),
                 "l2_sub_mcqlist");

            if (mmu_info_p->l2_sub_mcqlist.bits == NULL) {
                _bcm_kt2_cosq_free_memory(mmu_info_p);
                return BCM_E_MEMORY;
            }

            BCM_IF_ERROR_RETURN
            (_bcm_kt2_node_index_set(&mmu_info_p->l2_global_qlist,
                                  mmu_info_p->base_sub_queue +
                                  mmu_info_p->num_queues/2,
                                  mmu_info_p->num_sub_queues));
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_node_index_set(&mmu_info_p->global_qlist,
                                  mmu_info_p->base_sub_queue +
                                  mmu_info_p->num_queues/2,
                                  mmu_info_p->num_sub_queues));
        }

    }else {
       mmu_info_p->base_sub_queue = 0;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        /* create LLS tree for CPU */
        port = KT2_CMIC_PORT;
        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_add(unit, port, SOC_CMCS_NUM(unit) + 1,
                           0, &port_sched));
        for (cmc = 0; cmc < SOC_CMCS_NUM(unit); cmc++) {
             if (NUM_CPU_ARM_COSQ(unit, cmc) == 0) {
                 continue;
             }
             cosq = (NUM_CPU_ARM_COSQ(unit, cmc) + 7) / 8;

             /*    coverity[uninit_use_in_call : FALSE]    */
             BCM_IF_ERROR_RETURN
                 (bcm_kt2_cosq_gport_add(unit, port, cosq, 
                               BCM_COSQ_GPORT_SCHEDULER, &l0_sched));
             BCM_IF_ERROR_RETURN
                 (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, cmc));
             cosq = 0;
             num_queues = NUM_CPU_ARM_COSQ(unit, cmc);
             for (i = 0; i < NUM_CPU_ARM_COSQ(unit, cmc); i++) {
                 if (i % 8 == 0) {
                     num_inputs = (num_queues >= 8) ? 8 : num_queues;
                     BCM_IF_ERROR_RETURN
                         (bcm_kt2_cosq_gport_add(unit, port, num_inputs,
                                        BCM_COSQ_GPORT_SCHEDULER,
                                        &l1_sched));
                     BCM_IF_ERROR_RETURN
                          (bcm_kt2_cosq_gport_attach(unit, l1_sched, 
                                                     l0_sched, cosq));  
                     cosq++;
                     num_queues -= num_inputs;
                 } 
                 BCM_IF_ERROR_RETURN
                     (bcm_kt2_cosq_gport_add(unit, port, 1,
                                  BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
                 BCM_IF_ERROR_RETURN
                     (bcm_kt2_cosq_gport_attach(unit, queue, l1_sched, 
                                                (i % 8)));  

             }
        }

        /* Reserve additional L0 node for RPE as indicated for
         * flow control from MMU to CMIC block, Refer the register
         * LLS_FC_CONFIG - FC_CFG_COSMASK_L0_MAP
         */
        /*    coverity[uninit_use_in_call : FALSE]    */
        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_add(unit, port, -1,
                               BCM_COSQ_GPORT_SCHEDULER, &l0_sched));
        BCM_IF_ERROR_RETURN
            (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, cmc));

        /* create LLS tree for Loopback port*/
        port = LB_PORT(unit);;
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_port_lls_config_set(unit, port, 
             si->port_uc_cosq_base[port], si->port_num_uc_cosq[port], numq));

        
        for (port = 1; port < si->lb_port ; port++) {
             /* don't create lls for invalid port */
             if (!SOC_PORT_VALID(unit, port)) {
                 continue;
             }
             if (SOC_IS_SABER2(unit) && (!SOC_IS_METROLITE(unit)) &&
                 soc_saber2_port_is_unused(unit, port)) {
                 continue;
             }
#if defined BCM_METROLITE_SUPPORT
             if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PORT_LINKPHY_SUBTAG(unit, port)) {
                    numq = si->port_num_uc_cosq[port];
                } else {
                    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);
                    if (numq < 1) {
                        numq = 1;
                    } else if (numq > 8) {
                        numq = 8;
                    }
                }
                if (!(_SOC_IS_PORT_LINKPHY_SUBTAG(unit, port)) &&
                    _SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port)) {
                    continue;
                }
             }
#endif
             if (IS_HG_PORT(unit, port)) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_kt2_port_lls_config_set(unit, port, 
                      si->port_uc_cosq_base[port], 
                      si->port_num_uc_cosq[port], 
                      numq));
             } else {
                 if (!soc_feature(unit, soc_feature_mmu_packing)) {
                    BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_add(unit, port, 1, 0, &port_sched));
                    BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_add(unit, port, 1, 
                                        BCM_COSQ_GPORT_SCHEDULER, 
                                        &l0_sched));
                    BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 
                    BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_add(unit, port, numq, 
                                        BCM_COSQ_GPORT_SCHEDULER, 
                                        &l1_sched));
                    BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_attach(unit, l1_sched, l0_sched, 0));  
                    for (cosq = 0; cosq < numq; cosq++) {
                        BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_gport_add(unit, port, 1,
                                BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
                        BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_gport_attach(unit, queue, l1_sched, 
                                                cosq)); 
                        BCM_IF_ERROR_RETURN
                         (bcm_kt2_cosq_gport_sched_set(unit, l1_sched, cosq,
                                                       BCM_COSQ_STRICT, 0)); 
                   }
                 }else {
                    BCM_IF_ERROR_RETURN
                    (_bcm_kt2_packing_config_set(unit, port, numq));
                 }
             }
        }
   } 
    /* Initialize oob driver */
    BCM_IF_ERROR_RETURN(_bcm_oob_init(unit));

    /* Add default entries for PORT_COS_MAP memory profile 
     * for front panel ports 
     */
    BCM_IF_ERROR_RETURN(bcm_kt2_cosq_config_set(unit, numq));

    /* Add default entries for PORT_COS_MAP memory 
     * profile for subports 
     */
    pp_port_cos = soc_property_port_get(unit, port,
                                        spn_NUM_SUBPORT_COS, 4);

    BCM_IF_ERROR_RETURN(_bcm_kt2_subport_cosq_config_set(unit, pp_port_cos));

    /* setup subport specific LLS configuration */
    BCM_IF_ERROR_RETURN(_bcm_kt2_subport_config_set(unit));

    if (!cosq_sync_lock[unit]) {
        cosq_sync_lock[unit] = sal_mutex_create("KT2 cosq sync lock");
        if (cosq_sync_lock[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        if (!SOC_IS_METROLITE(unit)) {
            SOC_PBMP_CLEAR(si->pbm_ext_mem);
            for (port = 1; port < si->lb_port; port++) {
                 SOC_IF_ERROR_RETURN(READ_RQE_PP_PORT_CONFIGr(unit, port, &rval));
                 if (soc_reg_field_get(unit, RQE_PP_PORT_CONFIGr, rval, BUFF_TYPEf)) {
                     SOC_PBMP_PORT_ADD(si->pbm_ext_mem, port);
                 }
            }
        }
        return bcm_kt2_cosq_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    
   sal_memset(rval64s, 0, sizeof(rval64s));
   prval64s[0] = rval64s;
   profile_index = 0xffffffff;
   PBMP_PORT_ITER(unit, port) {
     if (profile_index == 0xffffffff) {
         BCM_IF_ERROR_RETURN
         (soc_profile_reg_add(unit, _bcm_kt2_llfc_profile[unit],
                              prval64s, 16, &profile_index));
     } else {
         BCM_IF_ERROR_RETURN
         (soc_profile_reg_reference(unit, _bcm_kt2_llfc_profile[unit],
                                               profile_index, 0));
     }
   }
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
    max_threshold = (1 << soc_mem_field_length(unit, 
                     MMU_WRED_DROP_CURVE_PROFILE_0m, MAX_THDf)) - 1;
    min_threshold = (1 << soc_mem_field_length(unit, 
                     MMU_WRED_DROP_CURVE_PROFILE_0m, MIN_THDf)) - 1;    
    for (i = 0; i < 6; i++) {
        soc_mem_field32_set(unit, wred_mems[i], entries[i], MIN_THDf,
                            max_threshold);
        soc_mem_field32_set(unit, wred_mems[i], entries[i], MAX_THDf,
                            min_threshold);
    }    
    profile_index = 0xffffffff;
    for (port = 0; port < (si->lb_port + 1); port++) {
        if (SOC_IS_SABER2(unit) && (!SOC_IS_METROLITE(unit)) &&
            soc_saber2_port_is_unused(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve(unit, port, -1,
                                        _BCM_KT2_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, _bcm_kt2_wred_profile[unit],
                                         entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                               _bcm_kt2_wred_profile[unit],
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
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ID0f, i);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_NUMf, 1);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_ACTIVEf, 1);
            if (SOC_PBMP_MEMBER (PBMP_EXT_MEM (unit), CMIC_PORT(unit))) {
                soc_reg_field_set(unit, TOQ_FLUSH0r, &rval, FLUSH_EXTERNALf, 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, rval));
            while (rval) {
                SOC_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                rval = soc_reg_field_get(unit, TOQ_FLUSH0r, rval,
                                         FLUSH_ACTIVEf);
            }
            rval = 0;
            soc_reg_field_set(unit, TOQ_ERROR2r, &rval, FLUSH_COMPLETEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOQ_ERROR2r(unit, rval));
        }
    }

    mmu_info_p->curr_merger_index[0] = 1;
    mmu_info_p->curr_merger_index[1] = 0;
    mmu_info_p->curr_merger_index[2] = 0;
    mmu_info_p->curr_merger_index[3] = 0;

    if (SOC_IS_SABER2(unit)) {
        if (!SOC_WARM_BOOT(unit)) { /* Cold Boot */
            WRITE_CMIC_CMC0_CH1_COS_CTRL_RX_1r(unit, 0xfffff);
        }
    }
    /* Clear Counters */
    if (!SOC_WARM_BOOT(unit)) {
       BCM_IF_ERROR_RETURN(_bcm_kt2_init_all_queue_counters(unit));
    } 
    /*
     * Enable the MMU to CPU flow control
     * per L0 nodes
     */
    BCM_IF_ERROR_RETURN
       (_bcm_kt2_cosq_cpu_fc_set(unit)); 
     BCM_IF_ERROR_RETURN
        (bcm_kt2_init_fc_map_tbl(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_detach(int unit, int software_state_only)
{
    if (_bcm_kt2_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                            _bcm_kt2_cos_map_profile[unit]));
        sal_free(_bcm_kt2_cos_map_profile[unit]);
        _bcm_kt2_cos_map_profile[unit] = NULL;
    }
    if (_bcm_kt2_egr_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                            _bcm_kt2_egr_cos_map_profile[unit]));
        sal_free(_bcm_kt2_egr_cos_map_profile[unit]);
        _bcm_kt2_egr_cos_map_profile[unit] = NULL;
    }
    if (_bcm_kt2_ifp_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                            _bcm_kt2_ifp_cos_map_profile[unit]));
        sal_free(_bcm_kt2_ifp_cos_map_profile[unit]);
        _bcm_kt2_ifp_cos_map_profile[unit] = NULL;
    }
    if (_bcm_kt2_wred_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                            _bcm_kt2_wred_profile[unit]));
        sal_free(_bcm_kt2_wred_profile[unit]);
        _bcm_kt2_wred_profile[unit] = NULL;
    }
    if (_bcm_kt2_llfc_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                            _bcm_kt2_llfc_profile[unit]));
        sal_free(_bcm_kt2_llfc_profile[unit]);
        _bcm_kt2_llfc_profile[unit] = NULL;
    }
    if (_bcm_kt2_mmu_info[unit] != NULL) {
        _bcm_kt2_cosq_free_memory(_bcm_kt2_mmu_info[unit]);
        sal_free(_bcm_kt2_mmu_info[unit]);
        _bcm_kt2_mmu_info[unit] = NULL;
    }
    if (cosq_sync_lock[unit]) {
        sal_mutex_destroy(cosq_sync_lock[unit]);
        cosq_sync_lock[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_subport_cosq_config_set
 * Purpose:
 *     Set the number of COS queues for subports.
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_kt2_subport_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];
    uint32 index , egr_index;
    int count;
    bcm_port_t port;
    int cos, prio;
    uint32 i;
    bcm_pbmp_t      subport_pbmp;

    BCM_PBMP_CLEAR(subport_pbmp);
    BCM_PBMP_ASSIGN(subport_pbmp, SOC_INFO(unit).subtag_pp_port_pbm);
    BCM_PBMP_OR(subport_pbmp, SOC_INFO(unit).linkphy_pp_port_pbm);

    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }
    SOC_PBMP_COUNT(subport_pbmp,count);
    if (count == 0) {
        return BCM_E_NONE;
    }

    /* Distribute first 8 internal priority levels into the specified number
     * of cosq, map remaining internal priority levels to highest priority
     * cosq.
     */
    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;
    prio = 0;
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                COSf, cos);
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                HG_COSf, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio], COSf,
                            numq - 1);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio], HG_COSf,
                            numq - 1);
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt2_cos_map_profile[unit], entries, 16,
                             &index));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt2_egr_cos_map_profile[unit], entries, 16,
                             &egr_index));

    PBMP_ITER(subport_pbmp, port) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                    index / 16));
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, EGR_PORTm, port, EGR_COS_MAP_SELf,
                                    egr_index / 16));

    }

    for (i = 0; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit], index,
                                  0);
        soc_profile_mem_reference(unit, _bcm_kt2_egr_cos_map_profile[unit], egr_index,
                0);

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_config_set
 * Purpose:
 *     Set the number of COS queues
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1], *hg_entries[1];
    uint32 index, hg_index, egr_index;
    int count, hg_count, egr_count;
    bcm_port_t port;
    int cos, prio;
    uint32 i;
    int cpu_hg_index = 0;
#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    port_cos_map_entry_t hg_cos_map_entries[16];
#endif
    bcm_pbmp_t      all_pbmp;
    int *profile_ref_count, profile_count, ref_count, rv;
    cos_map_sel_entry_t cos_map_sel_entry;

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

 
    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }

    if(SOC_WARM_BOOT(unit)) {
        _bcm_kt2_num_cosq[unit] = numq;
        return BCM_E_NONE;
    }

    profile_count = (soc_mem_index_count(unit, PORT_COS_MAPm) / 16);
    profile_ref_count = sal_alloc(profile_count * sizeof(int), 
                                    "Profile reference count");

    sal_memset(profile_ref_count, 0, profile_count * sizeof(int));

    if ( soc_feature(unit, soc_feature_flex_port)) {
        rv = _bcm_kt2_flexio_pbmp_update(unit, &all_pbmp);
        if (BCM_FAILURE(rv)) {
            sal_free(profile_ref_count);
            return rv;
	}
    }

    /* Get the profile reference count for front panel ports. */
    PBMP_ITER(all_pbmp, port) {
        rv = READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry);
        if (BCM_FAILURE(rv)) {
            sal_free(profile_ref_count);
            return rv;
        }
        index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf);
        if (index >= profile_count) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit, 
                 "Error. COS_MAP_SEL value %d greater than expected %d.\n"), index, profile_count));
            sal_free(profile_ref_count);
            return BCM_E_INTERNAL;
        }
        profile_ref_count[index]++;
    }

    cpu_hg_index = SOC_INFO(unit).cpu_hg_pp_port_index;
    if (cpu_hg_index != -1) {
        rv = READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, cpu_hg_index, &cos_map_sel_entry);
        if (BCM_FAILURE(rv)) {
            sal_free(profile_ref_count);
            return rv;
        }
        index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf);
        if (index >= profile_count) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit, 
                 "Error. COS_MAP_SEL value %d greater than expected %d.\n"), index, profile_count));
            sal_free(profile_ref_count);
            return BCM_E_INTERNAL;
        }
        profile_ref_count[index]++;
    }

    /* Clear out old profile referenced with front panel ports */
    index = 0;
    while (index < profile_count) {
        rv = soc_profile_mem_ref_count_get(unit,
                                    _bcm_kt2_cos_map_profile[unit],
                                    (index * 16), &ref_count);
        if (BCM_FAILURE(rv)) {
            sal_free(profile_ref_count);
            return rv;
        }
        while(profile_ref_count[index] && ref_count) {
            rv = soc_profile_mem_delete(unit,
                                _bcm_kt2_cos_map_profile[unit], (index * 16));
            if (BCM_FAILURE(rv)) {
                sal_free(profile_ref_count);
                return rv;
            }
            profile_ref_count[index]--;
            ref_count--;
        }
        /* Clear egress cos_map profile */
        rv = soc_profile_mem_ref_count_get(unit,
                                    _bcm_kt2_egr_cos_map_profile[unit],
                                    (index * 16), &egr_count);
        if (BCM_FAILURE(rv)) {
            sal_free(profile_ref_count);
            return rv;
        }
        while(egr_count) {
            rv = soc_profile_mem_delete(unit,
                                _bcm_kt2_egr_cos_map_profile[unit], (index * 16));
            if (BCM_FAILURE(rv)) {
                sal_free(profile_ref_count);
                return rv;
            }
            egr_count--;
        }
        index++;
    }
    sal_free(profile_ref_count);

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
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                 HG_COSf, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio], COSf,
                            numq - 1);
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],HG_COSf,
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
        (soc_profile_mem_add(unit, _bcm_kt2_cos_map_profile[unit], entries, 16,
                             &index));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt2_cos_map_profile[unit], hg_entries,
                             16, &hg_index));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_kt2_egr_cos_map_profile[unit], entries, 16,
                             &egr_index));
    count = 0;
    hg_count = 0;

    PBMP_ITER(all_pbmp, port) {
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
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, EGR_PORTm, port, EGR_COS_MAP_SELf,
                                    egr_index / 16));
        soc_profile_mem_reference(unit, _bcm_kt2_egr_cos_map_profile[unit], egr_index,
                0);
    }

    cpu_hg_index = SOC_INFO(unit).cpu_hg_pp_port_index;
    if (cpu_hg_index != -1) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    cpu_hg_index, SELECTf,
                                    hg_index / 16));
        hg_count++;
    }

    for (i = 1; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit], index,
                                  0);
    }
    for (i = 1; i < hg_count; i++) {
        soc_profile_mem_reference(unit, _bcm_kt2_cos_map_profile[unit],
                                  hg_index, 0);
    }

    _bcm_kt2_num_cosq[unit] = numq;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_config_get(int unit, int *numq)
{
    if (_bcm_kt2_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _bcm_kt2_num_cosq[unit];
    }

    return BCM_E_NONE;
}

int
bcm_kt2_cosq_gport_reattach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *sched_node;
    _bcm_kt2_mmu_info_t  *mmu_info;
    _bcm_kt2_cosq_node_t *parent , *new_parent;
    int cycle_sel = 0, index, rate_exp = 0;
    int thld_exp = 0, max_bucket = 0;
    bcm_port_t old_port, new_port;
    int alloc_size;
    int max_nodes;
    int temp_index;
    _bcm_kt2_cosq_list_t *list;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PARAM;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, sched_gport, NULL, NULL, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to < 0) {
        /* Not yet attached */
        return BCM_E_PARAM;
    }

    if (!(sched_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0 ||
          sched_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L1)) {
        return BCM_E_PARAM;
    }

   
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, input_gport, NULL, &new_port, NULL,
                                &new_parent));
    if ((new_parent->wrr_mode == 0) || (sched_node->parent->wrr_mode == 0)) {
        return BCM_E_PARAM;
    }

    if (new_parent->cosq_map == NULL) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_max_nodes_get(unit, sched_node->level, &max_nodes));
        if (new_parent->numq == -1) {
            new_parent->numq = max_nodes;
        } else {
            if (new_parent->numq > max_nodes) {
                return BCM_E_PARAM;
            }
        }

        alloc_size = SHR_BITALLOCSIZE(new_parent->numq);
        new_parent->cosq_map = _bcm_kt2_cosq_alloc_clear(alloc_size,
                "new_parent->cosq_map");
        if (new_parent->cosq_map == NULL) {
            return BCM_E_MEMORY;
        }
    }

    if (cosq < 0) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_map_index_get(new_parent->cosq_map,
                                         0, new_parent->numq, &cosq));
    } else {
        if (_bcm_kt2_cosq_map_index_is_set(new_parent->cosq_map,
                    cosq) == TRUE) {
            return BCM_E_EXISTS;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_index_resolve(unit, sched_gport, sched_node->cosq_attached_to,
                                    _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                                    NULL, &index, NULL));

    /* set max shaper with cycle_sel = 0xF, and store old cycle_sel */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_stop(unit, index, sched_node->level,
                                  &cycle_sel, &rate_exp, &thld_exp, &max_bucket));

    parent = sched_node->parent;
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, parent->gport, NULL, &old_port, NULL,
                                NULL));
    temp_index = sched_node->hw_index; 

    /* detach from old parent */
    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_detach(unit, sched_gport, parent->gport,
                                   sched_node->cosq_attached_to));
    mmu_info = _bcm_kt2_mmu_info[unit];
    list = &mmu_info->sched_hw_list[sched_node->level];
    _bcm_kt2_node_index_set(list, temp_index, 1);

    sched_node->hw_index = temp_index; 

    /* attach to new parent */
    BCM_IF_ERROR_RETURN
        (bcm_kt2_cosq_gport_attach(unit, sched_gport, input_gport, cosq));

    /* update queue map to reflect new port */
    if (old_port != new_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_queue_map_set(unit, sched_node, new_port));
    }

    /* restore cycle_sel */
    BCM_IF_ERROR_RETURN
        (_bcm_kt_cosq_bucket_start(unit, index, sched_node->level,
                           cycle_sel, rate_exp, thld_exp, max_bucket));

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_kt2_cosq_gport_attach
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
bcm_kt2_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *sched_node, *input_node;
    bcm_port_t sched_port, input_port, local_port;
    int rv, max_nodes;
    int status = 0; 
    _bcm_kt2_subport_info_t subport_info;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport) || 
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(input_gport)) {
        return BCM_E_PORT;
    }

    if(!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
         BCM_GPORT_IS_SCHEDULER(sched_gport))) {
        return BCM_E_PORT;
     }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to >= 0) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport) ||
        BCM_GPORT_IS_LOCAL(input_gport) ||
        BCM_GPORT_IS_MODPORT(input_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                   &input_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, input_gport, &input_port));
        input_node = NULL;
    }

    if (input_node != NULL) {
        if (!BCM_GPORT_IS_SCHEDULER(input_gport)) {
            if (input_node->numq == 0) {
                local_port = (BCM_GPORT_IS_LOCAL(input_gport)) ?
                              BCM_GPORT_LOCAL_GET(input_gport) :
                              BCM_GPORT_MODPORT_PORT_GET(input_gport);
                input_node->gport = input_gport;
                input_node->hw_index = local_port;
                input_node->level = _BCM_KT2_COSQ_NODE_LEVEL_ROOT;
                input_node->cosq_attached_to = 0;
                input_node->numq = 4; /* Assign max of 4 L0 nodes */
                input_node->base_index = -1;
            }

            if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
                return BCM_E_PARAM;
            }

            sched_node->level = (!_bcm_is_port_linkphy_subport(unit, sched_gport, 
                                    num_port_schedulers[unit])) ?
                                    _BCM_KT2_COSQ_NODE_LEVEL_L0 : 
                                    _BCM_KT2_COSQ_NODE_LEVEL_S0; 
        } else {
             if (input_node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
                 if (!BCM_GPORT_IS_SCHEDULER(input_gport)) {
                     return BCM_E_PARAM;
                 }
                 sched_node->level = 
                                (!_bcm_is_port_linkphy_subport(unit, sched_gport,
                                    num_port_schedulers[unit])) ?
                                    _BCM_KT2_COSQ_NODE_LEVEL_L0 : 
                                    _BCM_KT2_COSQ_NODE_LEVEL_S0; 
             }

             if (BCM_GPORT_IS_SCHEDULER(input_gport) &&
                 BCM_GPORT_IS_SCHEDULER(sched_gport)) {
                 if (input_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                     BCM_IF_ERROR_RETURN
                         (bcm_kt2_subport_pp_port_subport_info_get(unit, 
                                                         input_port, 
                                                         &subport_info));
                     if (subport_info.port_type == _BCM_KT2_SUBPORT_TYPE_LINKPHY) {
                         if (SOC_IS_METROLITE(unit)) {
                             sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_L0;
                         } else { 
                             sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_S1;
                         }
                         input_node->linkphy_enabled = 1;
                     } else {
                         sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_L0; 
                     }
                 }
                 if (input_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) {
                      sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_L0;
                 }
                 if (input_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L0) {
                      sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_L1;
                 }    
             }

             if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport)) {
                  if (!BCM_GPORT_IS_SCHEDULER(input_gport)) {
                     return BCM_E_PARAM;
                  }
                 if (input_node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) {
                     return BCM_E_PARAM;
                 }
                 sched_node->level = _BCM_KT2_COSQ_NODE_LEVEL_L2;
             }

             BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_max_nodes_get(unit, sched_node->level, &max_nodes));
             if (input_node->numq == -1) {
                 input_node->numq = max_nodes;
             } else {
                 if (input_node->numq > max_nodes) {
                     return BCM_E_PARAM;
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

    if (BCM_GPORT_IS_SCHEDULER(input_gport) || 
        BCM_GPORT_IS_LOCAL(input_gport) ||
        BCM_GPORT_IS_MODPORT(input_gport)) {
        if (input_node->cosq_attached_to < 0) {
            /* dont allow to attach to a node that has already attached */
            return BCM_E_PARAM;
        }

        sched_node->parent = input_node;
        sched_node->sibling = input_node->child;
        input_node->child = sched_node;
        /* resolve the nodes */
        rv = _bcm_kt2_cosq_node_resolve(unit, sched_node, cosq);
        if (BCM_FAILURE(rv)) {
            input_node->child = sched_node->sibling;
            return rv;
        }
       
        /* Protect LLS hierachy change as 
         * port flush on link flap might be effected 
         * when there is a change in the LLS hierarchy
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

        rv = _bcm_kt2_cosq_sched_node_set(unit, sched_gport, 
                            _BCM_KT2_COSQ_STATE_ENABLE);
        SOC_LLS_SCHEDULER_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Set the default sched mode to SP. In case where
         * BCM_COSQ_GPORT_SCHEDULER_WFQ flag is set or packing mode is set
         * or cosq offset is greater than 8, we need to set to WRR.
         */
        if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
            if (input_node->wrr_mode ||
                ((sched_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) &&
                (soc_feature(unit, soc_feature_mmu_packing))) ||
                (sched_node->cosq_attached_to >= 8)) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_sched_set(unit, input_gport,
                                                sched_node->cosq_attached_to,
                                                BCM_COSQ_WEIGHTED_ROUND_ROBIN,
                                                1));
            } else if (sched_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_subport_pp_port_subport_info_get(unit, sched_port,
                                                              &subport_info));
                if (subport_info.port_type == _BCM_KT2_SUBPORT_TYPE_LINKPHY) {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_gport_sched_set(unit, input_gport,
                                                      sched_node->cosq_attached_to,
                                                      BCM_COSQ_WEIGHTED_ROUND_ROBIN,
                                                      1));
                }
            } else {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_gport_sched_set(unit, input_gport,
                                                sched_node->cosq_attached_to,
                                                BCM_COSQ_STRICT, 0));
            }
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

int
bcm_kt2_cosq_gport_validate(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *sched_node, *input_node;
    bcm_port_t sched_port, input_port;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport) || 
            BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(input_gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport) ||
            BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(input_gport)) {
        return BCM_E_PORT;
    }

    if(!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_SCHEDULER(sched_gport))) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                                &sched_node));

    if (BCM_GPORT_IS_SCHEDULER(input_gport) ||
            BCM_GPORT_IS_LOCAL(input_gport) ||
            BCM_GPORT_IS_MODPORT(input_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                    &input_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_localport_resolve(unit, input_gport, &input_port));
        input_node = NULL;
    }

#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        if ((sched_port < num_port_schedulers[unit]) &&
            (input_port < num_port_schedulers[unit]) &&
            (sched_port != input_port) &&
            (!(_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, sched_port) ||
               _SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, input_port)))) {
            return BCM_E_PORT;
        }
    } else 
#endif
    {
    if ((sched_port < num_port_schedulers[unit]) &&
            (input_port < num_port_schedulers[unit]) &&
            (sched_port != input_port)) {
        return BCM_E_PORT;
    }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_kt2_cosq_gport_detach
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
bcm_kt2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *sched_node, *input_node, *prev_node;
    bcm_port_t sched_port, input_port;
    int rv;
    int status = 0;
      
    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(input_gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
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
                (_bcm_kt2_cosq_node_get(unit, input_gport, NULL, &input_port, 
                                        NULL, &input_node));
        }
        else {
            if (!(BCM_GPORT_IS_SCHEDULER(sched_gport) || 
                  BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) ||
                  BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(sched_gport) )) {
                return BCM_E_PARAM;
            }
            else {
                BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_localport_resolve(unit, input_gport,
                                                &input_port));
                input_node = NULL;
            }
        }
    }

    /* When local port is passed as gport, BCM_E_PORT is thrown as this
       is not handled in attach scenario */
    if (input_node == NULL) {
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

    rv = _bcm_kt2_cosq_sched_node_set(unit, sched_gport, 
                                     _BCM_KT2_COSQ_STATE_DISABLE);
    SOC_LLS_SCHEDULER_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
       return rv;
    }
    /* unresolve the node - delete this node from parent's child list */
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_unresolve(unit, sched_node, cosq));

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
        if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
              BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport) ||        
              BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(input_gport) ||
              BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(input_gport) ||
              BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(input_gport))) {
            if (sched_node->type  !=
                 _BCM_KT2_NODE_SUBSCRIBER_GPORT_ID) {
                sched_node->hw_index = -1;
            }
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
 *     bcm_kt2_cosq_gport_attach_get
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
bcm_kt2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_kt2_cosq_node_t *sched_node;
    bcm_module_t modid, modid_out;
    bcm_port_t port, port_out;

    if (input_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, sched_gport, &modid, &port, NULL,
                               &sched_node));

    if (sched_node->level == _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
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
_bcm_kt2_cosq_ef_update_war(int unit, bcm_gport_t gport,  bcm_cos_queue_t cosq,
                           int hw_index, lls_l2_parent_entry_t *entry)
{
    lls_l1_config_entry_t l1_config;
    uint32 min_bw, max_bw, min_burst, max_burst, flags = 0;
    int parent_index, spri_vector;
    uint32 start_pri, vec_3_0 =0, vec_7_4 = 0;
    uint32 rval = 0, bit_offset;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "_bcm_kt2_cosq_ef_update_war:unit=%d gport=0x%x cosq=%d idx=%d\n"),
              unit, gport, cosq, hw_index));
   
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_bucket_get(unit, gport, cosq, &min_bw, &max_bw,
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
        start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, &l1_config, 
                                        P_START_UC_SPRIf); 
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
            (_bcm_kt2_cosq_bucket_set(unit, gport, cosq, 0, 0,
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
            (_bcm_kt2_cosq_bucket_set(unit, gport, cosq, min_bw, max_bw,
                                     min_burst, max_burst, flags)); 
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_valid_ef_node(int unit, lls_l1_config_entry_t *entry, 
                           int hw_index)
{
    lls_l2_parent_entry_t l2_parent;
    int index, start_pri, spri_vector, num_spri, vec_7_4;
    uint32 bit_offset;
    int i;

    start_pri = soc_mem_field32_get(unit, LLS_L1_CONFIGm, entry, 
                                    P_START_UC_SPRIf); 
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
_bcm_kt2_cosq_ef_op_at_index(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
            if (_bcm_kt2_cosq_valid_ef_node(unit, &l1_config, 
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
                return _bcm_kt2_cosq_ef_update_war(unit, gport, cosq, 
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
_bcm_kt2_cosq_port_qnum_get(int unit, bcm_gport_t gport,
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
        (_bcm_kt2_cosq_localport_resolve(unit, gport, &local_port));
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

STATIC int
_bcm_kt2_cosq_ef_op(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq, int *arg,
                   _bcm_cosq_op_t op)
{
    bcm_port_t local_port;
    int index, numq, from_cos, to_cos, ci;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve
             (unit, gport, cosq, _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
              &local_port, &index, NULL));
        return _bcm_kt2_cosq_ef_op_at_index(unit, gport, cosq, 
                                           index, op, arg);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_index_resolve
             (unit, gport, cosq, _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
              &local_port, &index, &numq));
        if (cosq == BCM_COS_INVALID) {
            /* Maximum 1 spri can be EF enabled */
            from_cos = to_cos = 0;
        } else {
            from_cos = to_cos = cosq;
        }
        for (ci = from_cos; ci <= to_cos; ci++) {
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_index_resolve
                 (unit, gport, ci, _BCM_KT2_COSQ_INDEX_STYLE_BUCKET,
                  &local_port, &index, NULL));

            BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_ef_op_at_index(unit,
                                gport, ci, index, op, arg));
            if (op == _BCM_COSQ_OP_GET) {
                return BCM_E_NONE;
            }
        }
    }
    return BCM_E_NONE;
}

int 
_bcm_kt2_cosq_redirect_parameter_set (int unit, soc_reg_t reg, 
        soc_field_t field, bcm_cos_queue_t cosq, int arg)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, cosq, 
                 &rval));
    soc_reg_field_set(unit, reg, &rval, field, arg); 
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, cosq, 
                                      rval));
    return BCM_E_NONE;
}

#if defined(BCM_SABER2_SUPPORT)
STATIC int
_bcm_sb2_cosq_port_pg_idx_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          int *midx) 
{
    bcm_port_t local_port;
    soc_reg_t reg = INVALIDr;
    int port_pg, pg_profile = 0;
    uint64 rval64;
    uint32 rval;

    soc_reg_t pri_to_prigrp_reg = MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r;
    soc_field_t pri_to_prigrp_field[SB2_MAX_LOGICAL_PORTS]={
            SRCPORT_0f,  
            SRCPORT_1f,  SRCPORT_2f,  SRCPORT_3f,  SRCPORT_4f,  
            SRCPORT_5f,  SRCPORT_6f,  SRCPORT_7f,  SRCPORT_8f,  
            SRCPORT_9f,  SRCPORT_10f, SRCPORT_11f, SRCPORT_12f, 
            SRCPORT_13f, SRCPORT_14f, SRCPORT_15f, SRCPORT_16f, 
            SRCPORT_17f, SRCPORT_18f, SRCPORT_19f, SRCPORT_20f, 
            SRCPORT_21f, SRCPORT_22f, SRCPORT_23f, SRCPORT_24f, 
            SRCPORT_25f, SRCPORT_26f, SRCPORT_27f, SRCPORT_28f, 
            SRCPORT_29f
            };                 

    static const soc_reg_t prigroup_reg[] = {
        PROFILE0_PRI_GRP0r, PROFILE0_PRI_GRP1r,
        PROFILE1_PRI_GRP0r, PROFILE1_PRI_GRP1r,
        PROFILE2_PRI_GRP0r, PROFILE2_PRI_GRP1r,
        PROFILE3_PRI_GRP0r, PROFILE3_PRI_GRP1r
    };
    static const soc_field_t prigroup_field0[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf
    };

    static const soc_field_t prigroup_field1[] = {
        PRI8_GRPf,  PRI9_GRPf,  PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };

    if (midx == NULL) {
        return BCM_E_PARAM;
    }

    if (cosq > 15 || cosq < 0) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                NULL));        

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* The valid fields below are those which correspond to valid
     * PFC ports
     */
    READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, &rval64);
    if (soc_reg_field_valid(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r, 
                            pri_to_prigrp_field[local_port])) {
        pg_profile = soc_reg64_field32_get(unit, pri_to_prigrp_reg, rval64, 
                                    pri_to_prigrp_field[local_port]);

        if (cosq < 8) {
            reg = prigroup_reg[pg_profile * 2];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, local_port, 0, &rval));
            port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field0[cosq]);
        } else {
            reg = prigroup_reg[(pg_profile * 2) + 1];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, local_port, 0, &rval));
            port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field1[cosq - 8]);
        }
    } else {
        port_pg = 0;
    }
#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        *midx = ML_GET_THDI_PORT_PG(local_port) - 7 + port_pg;
    } else
#endif 
    {
        *midx = (local_port * SB2_MAX_PRIORITY_GROUPS) + port_pg;
    }

    if (*midx < 0) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
#endif

STATIC int
_bcm_kt2_cosq_port_pg_idx_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          int *midx) 
{
    bcm_port_t local_port;
    soc_reg_t reg = INVALIDr;
    int port_pg, pg_profile = 0;
    uint32 rval;

    soc_reg_t pri_to_prigrp_reg = MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r;
    soc_field_t pri_to_prigrp_field[KT2_MAX_LOGICAL_PORTS]={
            SRCPORT_0f,  
            SRCPORT_1f,  SRCPORT_2f,  SRCPORT_3f,  SRCPORT_4f,  
            SRCPORT_5f,  SRCPORT_6f,  SRCPORT_7f,  SRCPORT_8f,  
            SRCPORT_9f,  SRCPORT_10f, SRCPORT_11f, SRCPORT_12f, 
            SRCPORT_13f, SRCPORT_14f, SRCPORT_15f, SRCPORT_16f, 
            SRCPORT_17f, SRCPORT_18f, SRCPORT_19f, SRCPORT_20f, 
            SRCPORT_21f, SRCPORT_22f, SRCPORT_23f, SRCPORT_24f, 
            SRCPORT_25f, SRCPORT_26f, SRCPORT_27f, SRCPORT_28f, 
            SRCPORT_29f, SRCPORT_30f, SRCPORT_31f, SRCPORT_32f, 
            SRCPORT_33f, SRCPORT_34f, SRCPORT_35f, SRCPORT_36f, 
            SRCPORT_37f, SRCPORT_38f, SRCPORT_39f, SRCPORT_40f,
            SRCPORT_41f
            };                 

    static const soc_reg_t prigroup_reg[] = {
        MMU_ENQ_PROFILE_0_PRI_GRP0r, MMU_ENQ_PROFILE_0_PRI_GRP1r,
        MMU_ENQ_PROFILE_1_PRI_GRP0r, MMU_ENQ_PROFILE_1_PRI_GRP1r,
        MMU_ENQ_PROFILE_2_PRI_GRP0r, MMU_ENQ_PROFILE_2_PRI_GRP1r,
        MMU_ENQ_PROFILE_3_PRI_GRP0r, MMU_ENQ_PROFILE_3_PRI_GRP1r
    };

    static const soc_field_t prigroup_field0[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf
    };

    static const soc_field_t prigroup_field1[] = {
        PRI8_GRPf,  PRI9_GRPf,  PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };

    if (midx == NULL) {
        return BCM_E_PARAM;
    }

    if (cosq > 15 || cosq < 0) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                NULL));        

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    /* The valid fields below are those which correspond to valid
     * PFC ports
     */
    READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r_REG32(unit, &rval);
    if (soc_reg_field_valid(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r, 
                            pri_to_prigrp_field[local_port])) {
        pg_profile = soc_reg_field_get(unit, pri_to_prigrp_reg, rval, 
                                    pri_to_prigrp_field[local_port]);

        if (cosq < 8) {
            reg = prigroup_reg[pg_profile * 2];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, local_port, 0, &rval));
            port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field0[cosq]);
        } else {
            reg = prigroup_reg[(pg_profile * 2) + 1];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, local_port, 0, &rval));
            port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field1[cosq - 8]);
        }
    } else {
        port_pg = 0;
    }

    *midx = (local_port * _BCM_KT2_MAX_PRIORITY_GROUPS) + port_pg;

    if (*midx < 0) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_kt2_cosq_egr_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    int startq;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_mem_t mem2 = INVALIDm;
    soc_field_t fld_limit = INVALIDf, fld_limit2 = INVALIDf;
    int granularity = 1;
    bcm_port_t local_port;
    int cell_size = _BCM_KT2_COSQ_CELLSIZE;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node;
    int numq;
    soc_info_t    *si= &SOC_INFO(unit);

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }
        startq = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

    if (type != bcmCosqControlEgressPoolLimitEnable) {
        arg = arg / cell_size;
    }

    mem = MMU_THDO_QCONFIG_CELLm;
    if (type == bcmCosqControlEgressPoolRedLimitBytes) {
        mem = MMU_THDO_QOFFSET_CELLm;
        mem2 = MMU_THDO_QCONFIG_CELLm;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;
        fld_limit2 = Q_LIMIT_ENABLE_CELLf;
        granularity = 1;
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        fld_limit = LIMIT_YELLOW_CELLf;
        fld_limit2 = Q_COLOR_ENABLE_CELLf;
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        fld_limit = LIMIT_RED_CELLf;
        fld_limit2 = Q_COLOR_ENABLE_CELLf;
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolLimitEnable:
        fld_limit = Q_LIMIT_ENABLE_CELLf;
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if (IS_EXT_MEM_PORT(unit, local_port)) {
        max_val = soc_kt2_get_max_buffer_size(unit, 1, max_val);
    } else {
        max_val = soc_kt2_get_max_buffer_size(unit, 0, max_val);
    }
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        if (fld_limit2 != INVALIDr) {
            if (mem2 != INVALIDm) {
                soc_mem_field32_set(unit, mem2, entry, fld_limit2, 1);
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit2, 1);
            }
        }
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, startq1;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf, fld_limit2 = INVALIDf;
    int granularity = 1;
    int shared_size, cur_val, rv, post_update;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int cell_size = _BCM_KT2_COSQ_CELLSIZE;
    _bcm_kt2_cosq_node_t *node;
    int limit, numq;
    bcm_kt2_cosq_recalc_type_t recalc_type;
    soc_info_t    *si= &SOC_INFO(unit);

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

    arg /= cell_size;

    mem = MMU_THDO_QCONFIG_CELLm;

    if ((type == bcmCosqControlEgressUCQueueSharedLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueMinLimitBytes)) {
        if (node && (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                     BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport))) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if ((type == bcmCosqControlEgressMCQueueSharedLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueMinLimitBytes)) {
        if (node && (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport) ||
                     BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport))) {
            return BCM_E_PARAM;
        }
        if(soc_feature(unit, soc_feature_mmu_packing)) {
            startq = startq1;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    switch (type) {
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;
        fld_limit2 = Q_LIMIT_ENABLE_CELLf;

        /* Disable queuing when both MIN and SHARED LIMITS are zero.
         * Enable it when either MIN or SHARED LIMIT is non-zero.
         */
        limit = soc_mem_field32_get(unit, mem, entry, Q_MIN_CELLf);
        if (arg == 0 && limit == 0) {
            soc_kt2_cosq_queue_enable(unit, startq, 0);
        } else {
            soc_kt2_cosq_queue_enable(unit, startq, 1);
        }
        granularity = 1;
        break;
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        fld_limit = Q_MIN_CELLf;

        /* Disable queuing when both MIN and SHARED LIMITS are zero.
         * Enable it when either MIN or SHARED LIMIT is non-zero.
         */
        limit = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMIT_CELLf);
        if (arg == 0 && limit == 0) {
            soc_kt2_cosq_queue_enable(unit, startq, 0);
        } else {
            soc_kt2_cosq_queue_enable(unit, startq, 1);
        }
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    /* Recalculate Shared Values if Min Changed */
    if (IS_EXT_MEM_PORT(unit, local_port)) {
        soc_kt2_mmu_get_shared_size(unit, NULL, &shared_size);
    } else {
        soc_kt2_mmu_get_shared_size(unit, &shared_size, NULL);
    }
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
        if (IS_EXT_MEM_PORT(unit, local_port)) {
            recalc_type = BCM_KT2_RECALC_SHARED_EXT_DECREMENT; 
        } else {
            recalc_type = BCM_KT2_RECALC_SHARED_INT_DECREMENT; 
        }
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            rv = soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                    shared_size - delta, 
                                                    recalc_type);
        } else 
#endif
        {
            rv = soc_kt2_mmu_config_shared_buf_recalc(unit,
                                                    shared_size - delta, 
                                                    recalc_type);
        }
        if (rv < 0) { 
            return rv;
        }
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if (IS_EXT_MEM_PORT(unit, local_port)) {
        max_val = soc_kt2_get_max_buffer_size(unit, 1, max_val);
    } else {
        max_val = soc_kt2_get_max_buffer_size(unit, 0, max_val);
    }
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        if (fld_limit2 != INVALIDr) {
            soc_mem_field32_set(unit, mem, entry, fld_limit2, 1);
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if (post_update && 
        ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
         (type == bcmCosqControlEgressMCQueueMinLimitBytes))) {
        int delta = 0;
        delta = (cur_val - (arg / granularity)) * granularity;
        if (delta > shared_size) {
            return BCM_E_RESOURCE;
        }
        if (IS_EXT_MEM_PORT(unit, local_port)) {
            recalc_type = BCM_KT2_RECALC_SHARED_EXT_INCREMENT; 
        } else {
            recalc_type = BCM_KT2_RECALC_SHARED_INT_INCREMENT; 
        }
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            rv = soc_sb2_mmu_config_shared_buf_recalc(unit,
                                                    shared_size + delta, 
                                                    recalc_type);
        } else 
#endif
        {
            rv = soc_kt2_mmu_config_shared_buf_recalc(unit,
                                                    shared_size + delta, 
                                                    recalc_type);
        }
        if (rv < 0) { 
            return rv;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int midx, cell_size = _BCM_KT2_COSQ_CELLSIZE;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    soc_info_t    *si= &SOC_INFO(unit);

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                NULL));        

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

    arg /= cell_size;

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
    } else 
#endif
    {
        BCM_IF_ERROR_RETURN(
            _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
    }

    mem = THDI_PORT_PG_CONFIGm;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlIngressPortPGSharedLimitBytes:
        fld_limit = PG_SHARED_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGMinLimitBytes:
        fld_limit = PG_MIN_LIMITf;
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
    default:
        return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        max_val = soc_kt2_get_max_buffer_size(unit, 1, max_val);
    } else {
        max_val = soc_kt2_get_max_buffer_size(unit, 0, max_val);
    }

    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_alpha_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    int startq = -1;
    int midx; 
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int alpha;
    int dynamic_thresh_mode;    
    _bcm_kt2_cosq_node_t *node;

    switch(arg) {
    case bcmCosqControlDropLimitAlpha_1_64:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64;
        break;
    case bcmCosqControlDropLimitAlpha_1_32:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_32;
        break;
    case bcmCosqControlDropLimitAlpha_1_16:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_16;
        break;
    case bcmCosqControlDropLimitAlpha_1_8:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_8;
        break;
    case bcmCosqControlDropLimitAlpha_1_4:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_4;
        break;
    case bcmCosqControlDropLimitAlpha_1_2:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_2;
        break;
    case bcmCosqControlDropLimitAlpha_1:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1;
        break;   
    case bcmCosqControlDropLimitAlpha_2:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_2;
        break;
    case bcmCosqControlDropLimitAlpha_4:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_4;
        break;
    case bcmCosqControlDropLimitAlpha_8:
        alpha = SOC_KT2_COSQ_DROP_LIMIT_ALPHA_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL,
                                   &node));        
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }
        startq = node->hw_index;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || 
                   BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        }

        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        } 
        
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_SHARED_ALPHA_CELLf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));                 
    } else {
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        }

        mem = THDI_PORT_PG_CONFIGm;

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
_bcm_kt2_cosq_dynamic_thresh_enable_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, startq1;
    int midx, numq; 
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    _bcm_kt2_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }
    
    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        }

        mem = THDI_PORT_PG_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            PG_SHARED_DYNAMICf, arg ? 1 : 0);
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                    PG_SHARED_LIMITf, SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));           
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (node && 
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        
        mem = MMU_THDO_QCONFIG_CELLm;
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                    Q_SHARED_ALPHA_CELLf, SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));         
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (node && BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }

        mem = MMU_THDO_QCONFIG_CELLm;
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq1, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
        if (arg) {
            soc_mem_field32_set(unit, mem, entry,
                    Q_SHARED_ALPHA_CELLf, SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq1, entry));    
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
                                                 bcm_cos_queue_t cosq,
                                                 bcm_cosq_control_t type,
                                                 int arg)
{
    bcm_port_t local_port;
    int startq, startq1, numq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }
    

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (node && BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLE_CELLf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLE_CELLf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    }else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (node && BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq1, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLE_CELLf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLE_CELLf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq1, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_kt2_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_control_set: unit=%d gport=0x%x cosq=%d \
                         type=%d arg=%d\n"),
              unit, gport, cosq, type, arg));
    switch (type) {
    case bcmCosqControlEfPropagation:
        return _bcm_kt2_cosq_ef_op(unit, gport, cosq, &arg, _BCM_COSQ_OP_SET);
   
    case bcmCosqControlRedirectQueueSharedLimitBytes:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_SHARED_LIMITf, cosq, arg);
   
    case bcmCosqControlRedirectQueueMinLimitBytes:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_MINf, cosq, arg);

    case bcmCosqControlRedirectSharedDynamicEnable:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_LIMIT_DYNAMICf, cosq, arg);

    case bcmCosqControlRedirectQueueLimitEnable:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_LIMIT_ENABLEf, cosq, arg);

    case bcmCosqControlRedirectColorDynamicEnable:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_COLOR_DYNAMICf, cosq, arg);

    case bcmCosqControlRedirectColorEnable:
        return _bcm_kt2_cosq_redirect_parameter_set(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_COLOR_ENABLEf, cosq, arg);
    
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
        return _bcm_kt2_cosq_egr_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_kt2_cosq_egr_queue_set(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
        return _bcm_kt2_cosq_ing_res_set(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_kt2_cosq_alpha_set(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_kt2_cosq_dynamic_thresh_enable_set(unit, gport, cosq,
                                                       type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
        return _bcm_kt2_cosq_egr_queue_limit_enable_set(unit, gport, cosq, type, arg);

    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
_bcm_kt2_cosq_redirect_parameter_get (int unit, soc_reg_t reg,
        soc_field_t field, bcm_cos_queue_t cosq, int *arg)
{
    uint32 rval;
    if (!arg) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, cosq,
                 &rval));
    *arg = soc_reg_field_get(unit, reg, rval, field);

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{

    int startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    bcm_port_t local_port;
    _bcm_kt2_cosq_node_t *node;
    int cell_size = _BCM_KT2_COSQ_CELLSIZE;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int numq;
    soc_info_t    *si= &SOC_INFO(unit);

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;

    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

    mem = MMU_THDO_QCONFIG_CELLm;
    if (type == bcmCosqControlEgressPoolRedLimitBytes) {
        mem = MMU_THDO_QOFFSET_CELLm;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        fld_limit = LIMIT_YELLOW_CELLf;
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        fld_limit = LIMIT_RED_CELLf;
        granularity = 8;
        break;
    case bcmCosqControlEgressPoolLimitEnable:
        fld_limit = Q_LIMIT_ENABLE_CELLf;
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    if (type == bcmCosqControlEgressPoolLimitEnable) {
        *arg = (*arg) * granularity;
    } else {
        *arg = (*arg) * cell_size * granularity;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, startq1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int cell_size = _BCM_KT2_COSQ_CELLSIZE;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int numq;
    soc_info_t    *si= &SOC_INFO(unit);

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

    mem = MMU_THDO_QCONFIG_CELLm;

    if ((type == bcmCosqControlEgressUCQueueSharedLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueMinLimitBytes)) {
        if (node && (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                     BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport))) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            startq, entry));
    } else if ((type == bcmCosqControlEgressMCQueueSharedLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueMinLimitBytes)) {
        if (node && (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport) ||
                     BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(node->gport))) {
            return BCM_E_PARAM;
        }
        if(soc_feature(unit, soc_feature_mmu_packing)) {
            startq = startq1;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            startq, entry));
    }

    switch (type) {
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
        fld_limit = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
        break;
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        fld_limit = Q_MIN_CELLf;
        granularity = 1;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * cell_size;
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int cell_size = _BCM_KT2_COSQ_CELLSIZE;
    soc_info_t    *si= &SOC_INFO(unit);

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                NULL));        

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_EXT_MEM_PORT(unit, local_port)) {
        if ((si->mmu_ext_buf_size <= 192) && (!soc_feature(unit, soc_feature_mmu_packing))) {
            cell_size = _BCM_KT2_COSQ_EXT_CELLSIZE;
        } else {
            cell_size = _BCM_KT2_COSQ_EXT_PACKING_CELLSIZE;
        }
    }    

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
    } else 
#endif
    {
        BCM_IF_ERROR_RETURN(
            _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
    }

    mem = THDI_PORT_PG_CONFIGm;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
    case bcmCosqControlIngressPortPGSharedLimitBytes:
        fld_limit = PG_SHARED_LIMITf;
        granularity = 1;
        break;
    case bcmCosqControlIngressPortPGMinLimitBytes:
        fld_limit = PG_MIN_LIMITf;
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
   default:
        return BCM_E_UNAVAIL;
    }
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * cell_size;
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_alpha_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    int startq = -1;
    int midx;  
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;    
    int alpha;
    int dynamic_thresh_mode;    
    _bcm_kt2_cosq_node_t *node;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL,
                                   &node));        
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                   BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        }
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        } 
        
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);  

    } else {
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        }

        mem = THDI_PORT_PG_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

        alpha = soc_mem_field32_get(unit, mem, entry, PG_SHARED_LIMITf);  
    } 

    switch(alpha) {
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_64:
        *arg = bcmCosqControlDropLimitAlpha_1_64;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_32:
        *arg = bcmCosqControlDropLimitAlpha_1_32;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_16:
        *arg = bcmCosqControlDropLimitAlpha_1_16;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_8:
        *arg = bcmCosqControlDropLimitAlpha_1_8;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_4:
        *arg = bcmCosqControlDropLimitAlpha_1_4;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1_2:
        *arg = bcmCosqControlDropLimitAlpha_1_2;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_1:
        *arg = bcmCosqControlDropLimitAlpha_1;
        break;   
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_2:
        *arg = bcmCosqControlDropLimitAlpha_2;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_4:
        *arg = bcmCosqControlDropLimitAlpha_4;
        break;
    case SOC_KT2_COSQ_DROP_LIMIT_ALPHA_8:
        *arg = bcmCosqControlDropLimitAlpha_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, startq1;
    int midx;  
    uint32 entry[SOC_MAX_MEM_WORDS];
    _bcm_kt2_cosq_node_t *node;
    soc_mem_t mem = INVALIDm;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int numq;
    
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_sb2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        } else 
#endif
        {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_port_pg_idx_get(unit, gport, cosq, &midx));
        }

        mem = THDI_PORT_PG_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, PG_SHARED_DYNAMICf);  
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (node && BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        
        mem = MMU_THDO_QCONFIG_CELLm;
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);        
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (node && BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }

        mem = MMU_THDO_QCONFIG_CELLm;
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq1, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);        
    } else {
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_control_t type,
                                                int *arg)
{
    bcm_port_t local_port;
    int startq, startq1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info = _bcm_kt2_mmu_info[unit];
    int numq;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));        

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        if ((node->level != _BCM_KT2_COSQ_NODE_LEVEL_L1) &&
            (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            BCM_IF_ERROR_RETURN(
                _bcm_kt2_cosq_child_node_at_input(node, cosq,
                                              &node)); 
        }

        startq = node->hw_index;
        startq1 = node->hw_index;
    } else {
        mmu_info = _bcm_kt2_mmu_info[unit];
        numq = mmu_info->port[local_port].q_limit -
                mmu_info->port[local_port].q_offset;

        if (cosq < 0 || cosq >= numq) {
            return BCM_E_PARAM;
        }
        startq = mmu_info->port[local_port].q_offset + cosq;
        startq1 = mmu_info->port[local_port].mcq_offset + cosq;
    }
    
    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (node && BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        mem = MMU_THDO_QCONFIG_CELLm;
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLE_CELLf);
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (node && BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
            return BCM_E_PARAM;
        }
        mem = MMU_THDO_QCONFIG_CELLm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq1, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLE_CELLf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_kt2_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_control_get: unit=%d gport=0x%x cosq=%d type=%d\n"),
              unit, gport, cosq, type));
    switch (type) {
    case bcmCosqControlEfPropagation:
        return _bcm_kt2_cosq_ef_op(unit, gport, cosq, arg, _BCM_COSQ_OP_GET);
    case bcmCosqControlPortQueueUcast:
    case bcmCosqControlPortQueueMcast:
        return _bcm_kt2_cosq_port_qnum_get(unit, gport, cosq, type, arg);
    case bcmCosqControlRedirectQueueSharedLimitBytes:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_SHARED_LIMITf, cosq, arg);
    case bcmCosqControlRedirectQueueMinLimitBytes:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_MINf, cosq, arg);
    case bcmCosqControlRedirectSharedDynamicEnable:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_LIMIT_DYNAMICf, cosq, arg);
    case bcmCosqControlRedirectQueueLimitEnable:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG_THDORDEQr,
                Q_LIMIT_ENABLEf, cosq, arg);
    case bcmCosqControlRedirectColorDynamicEnable:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_COLOR_DYNAMICf, cosq, arg);
    case bcmCosqControlRedirectColorEnable:
        return _bcm_kt2_cosq_redirect_parameter_get(unit, OP_QUEUE_CONFIG1_THDORDEQr,
                Q_COLOR_ENABLEf, cosq, arg);
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
        return _bcm_kt2_cosq_egr_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_kt2_cosq_egr_queue_get(unit, gport, cosq, type, arg);
    case bcmCosqControlIngressPortPGSharedLimitBytes:
    case bcmCosqControlIngressPortPGMinLimitBytes:
    case bcmCosqControlIngressPortPGHeadroomLimitBytes:
    case bcmCosqControlIngressPortPGResetFloorBytes:
        return _bcm_kt2_cosq_ing_res_get(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_kt2_cosq_alpha_get(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_kt2_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                       type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
        return _bcm_kt2_cosq_egr_queue_limit_enable_get(unit, gport, cosq, type, arg);
    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_kt2_cosq_port_flush(int unit, 
                        bcm_port_t port,
                        bcm_cosq_flush_type_t type) 
{
    _bcm_kt2_mmu_info_t  *mmu_info;
    _bcm_kt2_cosq_node_t *port_node = NULL;
    _bcm_kt2_cosq_node_t *s0_node = NULL;
    _bcm_kt2_cosq_node_t *s1_node = NULL;
    _bcm_kt2_cosq_node_t *l0_node = NULL;
    _bcm_kt2_cosq_node_t *l1_node = NULL;
    _bcm_kt2_cosq_node_t *l2_node = NULL;
    uint32                flush_reg=0;

    if (!SOC_PORT_VALID(unit, port)) {
        LOG_CLI((BSL_META_U(unit,
                            "Invalid Port:%d \n"), port));
        return BCM_E_PORT;
    }
    switch (type) {
    case bcmCosqFlushTypePort :
        BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &flush_reg));
        soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg,
                                FLUSH_ACTIVEf,1);
        soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_TYPEf,1);
        soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, FLUSH_NUMf,1);
        soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg,
                                FLUSH_ID0f, port);
        if (IS_EXT_MEM_PORT(unit, port)) {
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                              FLUSH_EXTERNALf, 1);
        } else {
            soc_reg_field_set(unit, TOQ_FLUSH0r, &flush_reg, 
                              FLUSH_EXTERNALf, 0);
        }
        BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, flush_reg));
        break; 
    case bcmCosqFlushTypeQueue:
    default:
         /* Queue Flushing one by one so ... */
    mmu_info = _bcm_kt2_mmu_info[unit];
    /* get the root node */
    port_node = &mmu_info->sched_node[port];
    if (port_node && port_node->cosq_attached_to < 0) {
        LOG_CLI((BSL_META_U(unit,
                            "No Scheduling node found for port:%d\n"), port));
        return BCM_E_NONE;
    }   
    if (port_node->child == NULL) {
        /*Flushing the Queue when only Root Node is present with no child*/ 
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_flush_queues(unit, port,
                                             port_node, port_node->gport));
        break;
     }
     s0_node = port_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S0 ? 
                   port_node->child : port_node;
     if (s0_node->child == NULL) {
        /*Flushing the Queue when S0 Node is present with no child*/ 
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_flush_queues(unit, port,
                                             s0_node, s0_node->gport));
        break;
     }

         do {
             s1_node = s0_node->child->level == _BCM_KT2_COSQ_NODE_LEVEL_S1 ? 
                       s0_node->child : s0_node;
             do {
                 for (l0_node = s1_node->child; 
                      l0_node != NULL; 
         l0_node = l0_node->sibling) {
                      for (l1_node = l0_node->child; 
                           l1_node != NULL; 
              l1_node = l1_node->sibling) {
                           for (l2_node = l1_node->child; 
                                l2_node != NULL; 
                                l2_node = l2_node->sibling) {
                                /* Get Queue from L2 Node */
                                LOG_VERBOSE(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                                                    "Flushing Port:%d => "
                                                    "QueueNumber:%d \n"),
                                         port, l2_node->hw_index));
                                /* Now flush the queue  */
                                BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_flush_queues(
                                                    unit, port,
                                                    l2_node, l2_node->gport));
                                /* SDK-116300 After queue flush operation l2 nodes are getting disabled.
                                 Need to enable the l2 nodes after flush. */ 
                                BCM_IF_ERROR_RETURN(soc_kt2_cosq_queue_enable(unit, l2_node->hw_index, 1));
              }
                   }
              } 
                if (s1_node == s0_node) {
                    break;
              }
                s1_node = s1_node->sibling;
            } while (s1_node != NULL);
            if (s0_node == port_node) {
                break;
         }
            s0_node = s0_node->sibling;
        } while (s0_node != NULL);
        break;
    }
    return BCM_E_NONE;
}

int
bcm_kt2_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    uint32 rval;
    uint64        r64val;

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
    
    COMPILER_64_ZERO(r64val);
    soc_reg64_field32_set(unit, MMU_INTFO_CONGST_STr,
            &r64val, ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_INTFO_CONGST_STr(unit,
                port, r64val));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_id_create
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
bcm_kt2_cosq_field_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    int rv;
    int ref_count = 0;
    int ent_per_set = 16;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < SOC_MEM_SIZE(unit, IFP_COS_MAPm);) {
        rv = soc_profile_mem_ref_count_get
                (unit, _bcm_kt2_ifp_cos_map_profile[unit], i, &ref_count);
        if (SOC_E_NONE != rv) {
            return (rv);
        }
        if (0 == ref_count) {
            break;
        }
        i = i + ent_per_set;
    }

    if (i >= SOC_MEM_SIZE(unit, IFP_COS_MAPm) && ref_count != 0) {
        *classifier_id = 0;
        return (BCM_E_RESOURCE);
    }

    _BCM_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, (i / ent_per_set));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_get
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
bcm_kt2_cosq_field_classifier_get(
    int unit,
    int classifier_id,
    bcm_cosq_classifier_t *classifier)
{
    sal_memset(classifier, 0, sizeof(bcm_cosq_classifier_t));

    if (_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        classifier->flags |= BCM_COSQ_CLASSIFIER_FIELD;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_cosq_service_classifier_id_create
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
bcm_kt2_cosq_service_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }
    /* XXX check classifier->vlan for valid vlan */
    if (classifier->flags & BCM_COSQ_CLASSIFIER_VLAN) {
        if (!BCM_VLAN_VALID(classifier->vlan)) {
            return BCM_E_PARAM;
        }
        _BCM_COSQ_CLASSIFIER_SERVICE_SET(*classifier_id,
            classifier->vlan);
    }
    /* XXX check classifier->vfi_index for valid vfi index */
    if (classifier->flags & BCM_COSQ_CLASSIFIER_VFI) {
        /* coverity[unsigned_compare] */
        if ((classifier->vfi_index < 0) || 
                (classifier->vfi_index > BCM_VLAN_MAX)) {
            /* KT2 supports 4096 vfi indices */
            return BCM_E_PARAM;
        }
        _BCM_COSQ_CLASSIFIER_SERVICE_SET(*classifier_id,
            (classifier->vfi_index + BCM_VLAN_MAX + 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_cosq_service_classifier_id_destroy
 * Purpose:
 *      free resource associated with this service classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */     
int
bcm_kt2_cosq_service_classifier_id_destroy(
    int unit,
    int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_map_set
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
bcm_kt2_cosq_field_classifier_map_set(int unit,
                                      int classifier_id,
                                      int count,
                                      bcm_cos_t *priority_array,
                                      bcm_cos_queue_t *cosq_array)
{
    int rv;
    int i;
    int max_entries = 16;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t *ent_buf;
    int field_width;
    int ref_count = 0;
 
    /* Input parameter check. */
    if (!_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        return (BCM_E_PARAM);
    }

    if (count > max_entries) {
        return (BCM_E_PARAM);
    }

    ent_buf = sal_alloc(sizeof(ifp_cos_map_entry_t) * max_entries,
                        "IFP_COS_MAP entry");
    if (ent_buf == NULL) {
        return (BCM_E_MEMORY);
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
    rv = soc_profile_mem_ref_count_get
         (unit, _bcm_kt2_ifp_cos_map_profile[unit],
          index * max_entries, &ref_count);
    if (SOC_E_NONE != rv) {
        sal_free(ent_buf);
        return (rv);
    }
    if (ref_count == 0) {
        rv = soc_profile_mem_add(unit, _bcm_kt2_ifp_cos_map_profile[unit],
                                 entries, max_entries, &index);
    } else {
        rv = soc_profile_mem_set(unit, _bcm_kt2_ifp_cos_map_profile[unit],
                                 entries, index * max_entries);
        /* Now decrement the ref_count which got incremented by mem_set */
        BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_kt2_ifp_cos_map_profile[unit],
                                index * max_entries));
    }
    sal_free(ent_buf); 
    return (rv);
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_map_get
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
bcm_kt2_cosq_field_classifier_map_get(int unit,
                                      int classifier_id,
                                      int array_max,
                                      bcm_cos_t *priority_array,
                                      bcm_cos_queue_t *cosq_array,
                                      int *array_count)
{
    int rv;
    int i;
    int ent_per_set = 16;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t *ent_buf;

    /* Input parameter check. */
    if (NULL == priority_array || NULL == cosq_array || NULL == array_count) {
        return (BCM_E_PARAM);
    }

    /* Allocate entry buffer. */
    ent_buf = sal_alloc(sizeof(ifp_cos_map_entry_t) * ent_per_set,
                        "IFP_COS_MAP entry");
    if (ent_buf == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * ent_per_set);
    entries[0] = ent_buf;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Read out entries from profile table into allocated buffer. */
    rv = soc_profile_mem_get(unit, _bcm_kt2_ifp_cos_map_profile[unit],
                             index * ent_per_set, ent_per_set, entries);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        sal_free(ent_buf);
        return rv;
    }

    *array_count = array_max > ent_per_set ? ent_per_set : array_max;

    /* Copy values into API OUT parameters. */
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i] >= 16) {
            sal_free(ent_buf);
            return BCM_E_PARAM;
        }
        cosq_array[i] = soc_mem_field32_get(unit, IFP_COS_MAPm,
                                            &ent_buf[priority_array[i]],
                                            IFP_COSf);
    }

    sal_free(ent_buf);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_map_clear
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
bcm_kt2_cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    int ent_per_set = 16;
    uint32 index;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Delete the profile entries set. */
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_kt2_ifp_cos_map_profile[unit],
                                index * ent_per_set));
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_kt2_cosq_field_classifier_id_destroy
 * Purpose:
 *      Free resource associated with this field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_kt2_cosq_field_classifier_id_destroy(int unit, int classifier_id)
{
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_kt2_cosq_service_map_set
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
 * Comments:
 *      <service_cos> =
 *      SERVICE_COS_MAP[VLAN].SERVICE_COS +
 *      ING_QUEUE_OFFSET_MAPPING_TABLE[PRI].QUEUE_OFFSET
 *
 *      Unicast Service Queue Number =
 *      ING_COS_MODE[PORT].SERVICE_BASE_QUEUE_NUM +
 *      PHYSICAL_PORT_BASE_QUEUE[PORT].BASE_QUEUE_NUMBER +
 *      <service_cos>
 *
 *      Multicast Service Queue Number =
 *         RQE_PP_PORT_CONFIG[PORT].BASE_QUEUE + <service_cos>
 */
int
bcm_kt2_cosq_service_map_set(
    int unit,
    bcm_port_t port,
    int classifier_id,
    bcm_gport_t queue_group,
    int count,
    bcm_cos_t *priority,
    bcm_cos_queue_t *cosq)
{
    int rv = BCM_E_NONE;
    int i;
    service_cos_map_entry_t scm;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    void *entries[1];
    uint32 old_index = 0;
    uint32 new_index = 0;
    int vid;
    int valid_entry;
    _bcm_kt2_cosq_node_t *node;
    uint32 rval = 0, rval1 = 0;
    uint32 uc_base = 0, required_base = 0;
    uint32 service_cos = 0;
    bcm_port_t local_port;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_port_info_t *port_info;

    if (!_BCM_COSQ_CLASSIFIER_IS_SERVICE(classifier_id)) {
        return BCM_E_PARAM;
    }

    /* index above 4K is for VFI */
    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);
    
    /* validate the vlan id */
    if (vid < 0 || vid >= SOC_MEM_SIZE(unit, SERVICE_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_node_get(unit, queue_group, NULL, 
                                               &local_port, NULL, &node));

    if (node->cosq_attached_to < 0) {
        return BCM_E_PARAM;
    }    

    /* validate the user parameters */
    if (count > 16) {
        return BCM_E_PARAM;
    }

    /* queues allocated for this queue_group should be contiguous.*/
    /* check if user's queue number is valid */
    for (i = 0; i < count; i++) {
        if (cosq[i] >= 8 || priority[i] >= 16) {
            return BCM_E_PARAM;
        }
    }

    sal_memset(map_entries, 0, sizeof(map_entries));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &scm));
    valid_entry = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm, VALIDf);

    SOC_IF_ERROR_RETURN(READ_RQE_PP_PORT_CONFIGr(unit, port, &rval1));
    SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));

    if ((soc_reg_field_get(unit, RQE_PP_PORT_CONFIGr, rval1, COS_MODEf) != 3) &&
        (soc_reg_field_get(unit, ING_COS_MODEr, rval, COS_MODEf) == 3)) {
         uc_base = soc_reg_field_get(unit, ING_COS_MODEr, rval,
                                     SERVICE_BASE_QUEUE_NUMf);
         mmu_info = _bcm_kt2_mmu_info[unit];
         port_info = &mmu_info->port[local_port];
         required_base = port_info->uc_vlan_base < mmu_info->num_queues -1 ?
                         port_info->uc_vlan_base :
                         ((uc_base == 0) ? node->hw_index : uc_base);
         if (uc_base != required_base) {
             uc_base = required_base;
             soc_reg_field_set(unit, ING_COS_MODEr, &rval,
                          SERVICE_BASE_QUEUE_NUMf, uc_base);
             SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));
         }
    } else {
        uc_base = soc_reg_field_get(unit, ING_COS_MODEr, rval, BASE_QUEUE_NUMf);
        if (uc_base != soc_reg_field_get(unit, ING_COS_MODEr, rval,
                                         SERVICE_BASE_QUEUE_NUMf)) {
            /* First time service queuing being configured on this port */
            soc_reg_field_set(unit, ING_COS_MODEr, &rval,
                              SERVICE_BASE_QUEUE_NUMf, uc_base);
            SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));
        }
    }

    service_cos = node->hw_index - uc_base;

    entries[0] = &map_entries;
    if (valid_entry) {
        uint32 current_service_cos;
        /* 
         * We reached here, so at least one mapping must have been configured.
         * For next configuration base queue must be same.
         */
        current_service_cos = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm, SERVICE_COSf);
        if (current_service_cos != service_cos) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
                                  "Error: Cannot satisfy service queue offset "
				  "requirement for new map on port %d/VID %d, "
				  "current offset: %d; required offset: %d\n"),
                                  port, vid, current_service_cos, service_cos));
            return BCM_E_PARAM;
        }
        old_index = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm,
                                    QUEUE_OFFSET_PROFILE_INDEXf);
        old_index *= 16;

        BCM_IF_ERROR_RETURN
            (_bcm_offset_map_table_entry_get(unit, old_index, 16, entries));
        rv = _bcm_offset_map_table_entry_delete(unit, old_index);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
    } else {
        soc_mem_field32_set(unit, SERVICE_COS_MAPm, &scm, VALIDf, 1);
    }

    for (i = 0; i < count; i++) {
        soc_mem_field32_set(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm, 
                &map_entries[priority[i]], QUEUE_OFFSETf, cosq[i]);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_offset_map_table_entry_add(unit, entries, 16, &new_index));
    
    soc_mem_field32_set(unit, SERVICE_COS_MAPm, &scm, QUEUE_OFFSET_PROFILE_INDEXf,
                        new_index / 16);
    soc_mem_field32_set(unit, SERVICE_COS_MAPm, &scm, SERVICE_COSf,
                        service_cos);
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, SERVICE_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &scm));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_cosq_service_classifier_get
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
bcm_kt2_cosq_service_classifier_get(
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
 *      bcm_kt2_cosq_service_map_get
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
bcm_kt2_cosq_service_map_get(
    int unit,
    bcm_port_t port,
    int classifier_id,
    bcm_gport_t *queue_group,
    int array_max,
    bcm_cos_t *priority,
    bcm_cos_queue_t *cosq,
    int *array_count)
{
    service_cos_map_entry_t scm;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    void *entries[1];
    int index;
    int vid;
    int valid_entry;
    int i;
    int cos_offset;
    uint32 uc_base = 0;
    uint32 rval = 0;

    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, SERVICE_COS_MAPm, MEM_BLOCK_ANY, (int)vid, &scm));

    valid_entry = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm, VALIDf);
    if (!valid_entry) {
        return BCM_E_CONFIG;  /* service queue is not configured */
    }

    index = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm,
                                QUEUE_OFFSET_PROFILE_INDEXf);
    index *= 16;
    entries[0] = &map_entries;
    BCM_IF_ERROR_RETURN
        (_bcm_offset_map_table_entry_get(unit, index, 16, entries));
    
    cos_offset = soc_mem_field32_get(unit, SERVICE_COS_MAPm,
                                     &scm, SERVICE_COSf);

    BCM_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
    uc_base = soc_reg_field_get(unit, ING_COS_MODEr, rval, SERVICE_BASE_QUEUE_NUMf);

    bcm_kt2_cosq_subscriber_qid_set(unit, queue_group, cos_offset+uc_base);
    *queue_group |= (port << 16);
    *array_count = array_max > 16 ? 16 :
                   array_max;
                   
    for (i = 0; i < *array_count; i++) {
        if (priority[i] >= 16) {
            return BCM_E_PARAM;
        }
        cosq[i] = soc_mem_field32_get(unit,ING_QUEUE_OFFSET_MAPPING_TABLEm,
                               &map_entries[priority[i]], QUEUE_OFFSETf);
    }
 
    return BCM_E_NONE;      
}

/*
 * Function:
 *      bcm_kt2_cosq_service_map_clear
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
bcm_kt2_cosq_service_map_clear(
    int unit,
    bcm_gport_t port,
    int classifier_id)
{
    int rv = BCM_E_NONE;
    service_cos_map_entry_t scm;
    int index;
    int vid;
    int valid_entry;

    vid = _BCM_COSQ_CLASSIFIER_SERVICE_GET(classifier_id);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, SERVICE_COS_MAPm, MEM_BLOCK_ANY,
                        (int)vid, &scm));

    valid_entry = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm, VALIDf);
    if (!valid_entry) {
        return BCM_E_NONE;  /* service queue is already cleared  */
    }

    index = soc_mem_field32_get(unit, SERVICE_COS_MAPm, &scm,
                               QUEUE_OFFSET_PROFILE_INDEXf);
    index *= 16;

    /* delete the profile */
    rv = _bcm_offset_map_table_entry_delete(unit, index);
    if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
        return rv;
    }

    sal_memset(&scm,0, sizeof(service_cos_map_entry_t));

    /* Write back updated buffer. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, SERVICE_COS_MAPm, MEM_BLOCK_ALL,
                       (int)vid, &scm));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_kt2_cosq_gport_dmvoq_config_set 
 * Purpose:
 *      Setting SOURCE_MODMAP , FC table and BASEINDEX table
 *      appropriately as per the architecture
 * Parameters:
 *      unit           - (IN) Unit number.
 *      gport          - (IN) gport identifier 
 *      cos            - (IN) cos value 
 *      fabric_modid   - (IN) Fabric Mod ID
 *      dest_modid     - (IN) Destination Mod ID
 *      fabric_port    - (IN) Fabric port ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_kt2_cosq_gport_dmvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t dest_modid, 
                                bcm_module_t fabric_modid,
                                bcm_port_t fabric_port)
{
    int  intf_index, rv;
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_cosq_node_t *base_node = NULL;
    bcm_port_t local_port;
    mmu_intfi_base_index_tbl_entry_t base_tbl_entry;
    uint32  index;
    uint32  val;
    int stack_port;
    uint32  modport_map_index;
    modport_map_sw_entry_t sw_entry;
    ing_queue_map_entry_t ing_queue_entry;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    void *entries[1];
    int map_offset = 0, fabric_port_count, port;
    int count = 0, cng_offset;
    mmu_intfi_offset_map_tbl_entry_t offset_map_tbl_entry; 
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node_base;
    int enable, base_queue = 0;
    int istrunk;
    entries[0] = &map_entries;
    mmu_info = _bcm_kt2_mmu_info[unit];

    if (fabric_modid >= 64) {
        return BCM_E_PARAM;
    }

    if (dest_modid >= 256) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL ,&local_port, NULL, &node));

    if (!node) {
        return BCM_E_NOT_FOUND;
    }
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_voq_base_node_get(
                unit, dest_modid, fabric_modid, &base_node));
    if (base_node == NULL) {
        base_node = node ;
    }
    intf_index = _bcm_kt2_cosq_source_intf_set(unit, fabric_modid, 
            0, _BCM_KT2_COSQ_DMVOQ);

    if (intf_index < 0) {
        return BCM_E_RESOURCE;
    }
    if (SOC_IS_METROLITE(unit)) {
        intf_index = 0;
    } else if (SOC_IS_SABER2(unit)) {
        if (intf_index > 1) {
            intf_index = 0;
        }
    } else {
        if (intf_index > 3) {
            intf_index = 0;
        }
    }

    BCM_PBMP_COUNT(base_node->fabric_pbmp, fabric_port_count);

    if (fabric_port_count == 0) {
        map_offset = mmu_info->curr_merger_index[intf_index];
        if (map_offset >= soc_mem_index_count(unit, MMU_INTFI_MERGE_ST_TBLm)) {
            return BCM_E_RESOURCE;
        }
    } else {
        BCM_PBMP_ITER(base_node->fabric_pbmp, port) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port + (intf_index * 64), &offset_map_tbl_entry));
            map_offset = soc_mem_field32_get(unit, MMU_INTFI_OFFSET_MAP_TBLm,
                    &offset_map_tbl_entry,
                    MAP_OFFSETf);
            break;
        }
    }

    /* clean up fabric pbmp code if fabric mod_id and dest mod id are different
       if the mod id are different then we need to update the pbmp else it will 
       be appended as in case of trunk.
     */
    if ((base_node->remote_modid != dest_modid) || 
            (base_node->fabric_modid != fabric_modid)) {
        if (node->hw_index % 8 == 0) { 
            BCM_PBMP_CLEAR(base_node->fabric_pbmp);

        }
    } 
    if (!BCM_PBMP_MEMBER(base_node->fabric_pbmp, fabric_port)) {
        if (map_offset == mmu_info->curr_merger_index[intf_index]) {
            mmu_info->curr_merger_index[intf_index]++;
        }

        BCM_PBMP_PORT_ADD (base_node->fabric_pbmp, fabric_port);
        BCM_PBMP_COUNT(base_node->fabric_pbmp, fabric_port_count);
        BCM_PBMP_ITER(base_node->fabric_pbmp, port) {
            count++;
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port + (intf_index * 64), &offset_map_tbl_entry));
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    MAP_OFFSETf, map_offset);
            soc_mem_field32_set(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                    &offset_map_tbl_entry,
                    LASTf, count < fabric_port_count ? 0 : 1 );
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_OFFSET_MAP_TBLm, 
                        MEM_BLOCK_ALL, 
                        port + (intf_index * 64), &offset_map_tbl_entry));
        }
    }



    if (node->hw_index % 8 == 0) {

        node->remote_modid = dest_modid;
        node->fabric_modid = fabric_modid;
        node_base = mmu_info->queue_node;
        count = 0;
        SOC_IF_ERROR_RETURN
            (READ_MODPORT_MAP_SELr(unit, local_port, &val));
        index = soc_reg_field_get(unit, MODPORT_MAP_SELr, val,
                MODPORT_MAP_INDEX_UPPERf);
        if (SOC_IS_METROLITE(unit)) {
            modport_map_index  = (index * 64) + dest_modid;
        } else {
            modport_map_index  = (index * 256) + dest_modid;
        }

        soc_mem_lock(unit, MODPORT_MAP_SWm);
        rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY, modport_map_index,
                &sw_entry);
        istrunk = soc_mem_field32_get(unit, MODPORT_MAP_SWm, &sw_entry, 
                ISTRUNK0f); 
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        for(index = mmu_info->base_dmvoq_queue ; 
                index < mmu_info->num_queues; index++) {
            if ((node_base[index].remote_modid == dest_modid) &&
                    ((node_base[index].fabric_modid == fabric_modid) || (istrunk))) {
                count++;
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_node_get(unit, node_base[index].gport, 
                                            NULL ,&port, NULL, NULL));
                SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &val));

                base_queue = soc_reg_field_get(unit, ING_COS_MODEr, val, BASE_QUEUE_NUMf);

            }
        }
        if(count == 1) {
            SOC_IF_ERROR_RETURN
                (READ_MODPORT_MAP_SELr(unit, local_port, &val));

            index = soc_reg_field_get(unit, MODPORT_MAP_SELr, val,
                    MODPORT_MAP_INDEX_UPPERf);

            if (SOC_IS_METROLITE(unit)) {
                modport_map_index  = (index * 64) + dest_modid;
            } else {
                modport_map_index  = (index * 256) + dest_modid;
            } 

            soc_mem_lock(unit, MODPORT_MAP_SWm);
            rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY, modport_map_index,
                    &sw_entry);
            if (SOC_SUCCESS(rv)) {
                enable = soc_mem_field32_get(unit, MODPORT_MAP_SWm, &sw_entry, 
                        ENABLE0f); 
                if (!enable) { 
#ifdef BCM_METROLITE_SUPPORT
                    if(SOC_IS_METROLITE(unit))  {
                       stack_port = ((local_port << 5) | local_port);
                    } else
#endif
                    {
                      stack_port = ((local_port << (SOC_IS_SABER2(unit)? 7: 8)) | local_port);
                    } 
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, DEST0f,
                            stack_port);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ISTRUNK0f,0);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ADD_DESTINATION_PORTf,
                            0);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ENABLE0f, 1);
                }
                soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, PER_MODID_QUEUEING_ENABLEf,
                        1);
                soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, QUEUE_MAP_POINTERf,
                        node->hw_index);
                rv = soc_mem_write(unit, MODPORT_MAP_SWm, MEM_BLOCK_ALL, modport_map_index,
                        &sw_entry);
            }
            soc_mem_unlock(unit, MODPORT_MAP_SWm);

            sal_memset(map_entries, 0, sizeof(map_entries));
            /* Hardcoding the ING_QUEUE_OFFSET_MAPPING_TABLE */
            for (index = 0; index < 16; index++) {
                soc_mem_field32_set(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm,
                        &map_entries[index], QUEUE_OFFSETf, index > 7 ? 7 : index);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_offset_map_table_entry_add(unit, entries, 16, &index));

            sal_memset(&ing_queue_entry, 0, sizeof(ing_queue_map_entry_t));
            soc_mem_field32_set(unit, ING_QUEUE_MAPm,
                    &ing_queue_entry, QUEUE_SET_BASEf, 
                    node->hw_index - base_queue);
            soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                    QUEUE_OFFSET_PROFILE_INDEXf, index/16);
            soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                    ADD_EGRESS_PORT_BASE_QUEUEf, 1);
            BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, ING_QUEUE_MAPm,
                        node->hw_index, &ing_queue_entry));
        }

        /*
         *  SRC_INTF and MMU_INTFI_BASE_INDEX_TBL is mapped one to one,
         *  so writing the base index at same index where SRC_INTF is written
         */

        cng_offset = intf_index * 64 ;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, MMU_INTFI_BASE_INDEX_TBLm, 
                          MEM_BLOCK_ALL, intf_index, &base_tbl_entry));
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                BASE_ADDRf, cng_offset);
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                ENf, (node->level - 3 ));

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_BASE_INDEX_TBLm,
                    MEM_BLOCK_ALL, intf_index, &base_tbl_entry));

        /* set the mapping from congestion bits in flow control table
         * to corresponding hw_index */
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_fc_status_to_node(unit, 
                    _BCM_KT2_COSQ_FC_VOQFC,  
                    node->hw_index, (node->level - 2), cng_offset + map_offset));


    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt2_cosq_gport_e2ecc_coe_config_set
 * Purpose:
 *      Setting SOURCE_MODMAP, FC table and BASEINDEX table
 *      appropriately as per the architecture
 * Parameters:
 *      unit           - (IN) Unit number.
 *      gport          - (IN) gport identifier
 *      cos            - (IN) cos value
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_kt2_cosq_gport_e2ecc_coe_config_set(int unit, bcm_gport_t gport)
{
    soc_info_t *si;
    int  intf_index;
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_port_t subport;
    int subport_start = 0;
    int subport_end = 0;
    mmu_intfi_base_index_tbl_entry_t base_tbl_entry;
    int cng_offset;
    int subport_offset = 0;
    uint32 regval;
    int num_subports = 0;
#if defined BCM_METROLITE_SUPPORT 
    int pp_port = 0;
#endif 
 
    si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL ,
                               &subport, NULL, &node));

    if (!node) {
        return BCM_E_NOT_FOUND;
    }
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        return BCM_E_PARAM;
    }

    local_port = node->local_port;

    /* Subport base for the COE port */
    subport_start = si->port_subport_base[local_port];
    subport_end = si->port_subport_base[local_port] +
                   si->port_num_subport[local_port];

    /* number of subports on the COE port
     * indicating maximum number of FC bytes
     * that can be received on a COE
     * port
     */
    num_subports = si->port_num_subport[local_port];

    if (num_subports == 0) {
        return BCM_E_INIT;
    }

    /* Maximum of 64 FC bytes are supported in E2ECC
     * message. Hence limiting the number of subports
     * to 64 when more number of subports configured
     * on a COE port.
     */
    num_subports = ((num_subports > 64) ? 64 : num_subports);
#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        if (!_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, subport)) {
             return BCM_E_INIT;
        }
        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, local_port), pp_port) { 
        if (subport != pp_port) { 
            subport_offset++; 
            continue;
        } else { 
            break;
           }  
        }   
    } else 
#endif
    {
        subport_offset = subport - subport_start;
        if ((subport < subport_start) ||
             (subport > subport_end)) {
              return BCM_E_INIT;
         }
    }
    if (node->hw_index % 8 == 0) {

        intf_index = _bcm_kt2_cosq_source_intf_set(unit, 0,
                local_port, _BCM_KT2_COSQ_E2ECC_COE);
        if (intf_index < 0) {
            return BCM_E_RESOURCE;
        }

        /*
         *  SRC_INTF and MMU_INTFI_BASE_INDEX_TBL is mapped one to one,
         *  so writing the base index at same index where SRC_INTF is written
         */

        cng_offset = (intf_index * num_subports) ;
        /* set the congestion state bytes max to number of subports on a port
         *  as max num subports  FC bytes  can be received for  for src_intf
         */
        soc_reg_field_set(unit, CONGESTION_STATE_BYTESr, &regval,
                    DATAf, num_subports);
        SOC_IF_ERROR_RETURN(WRITE_CONGESTION_STATE_BYTESr(unit,
                        intf_index, regval));

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, MMU_INTFI_BASE_INDEX_TBLm,
                          MEM_BLOCK_ALL, intf_index, &base_tbl_entry));
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry,
                BASE_ADDRf, cng_offset);
        /* set Enf - 2 for level 2 */
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry,
                ENf, (node->level - 3 ));

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_BASE_INDEX_TBLm,
                    MEM_BLOCK_ALL, intf_index, &base_tbl_entry));

        /* set the mapping from congestion bits in flow control table
         * to corresponding hw_index */
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_fc_status_to_node(unit,
                    _BCM_KT2_COSQ_FC_E2ECC_COE,
                    node->hw_index, (node->level - 2),
                    (cng_offset + (subport_offset))));


    }
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_gport_dpvoq_config_set(int unit, bcm_gport_t gport, 
                                bcm_cos_queue_t cosq,
                                bcm_module_t dest_modid, 
                                bcm_port_t dest_port)
{
    int  intf_index, rv;
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    mmu_intfi_base_index_tbl_entry_t base_tbl_entry;
    uint32  index;
    uint32  val;
    int stack_port;
    uint32  modport_map_index;
    modport_map_sw_entry_t sw_entry;
    ing_queue_map_entry_t ing_queue_entry;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    physical_port_base_queue_entry_t phy_port_entry;
    void *entries[1];
    int count = 0, cng_offset;
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *node_base;
    int enable, base_queue;
    int phy_base_index = 0, offset = 0 , hw_index = 0;
    int port;
    entries[0] = &map_entries;
    mmu_info = _bcm_kt2_mmu_info[unit];

    if (dest_modid >= 64) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL ,&local_port, NULL, &node));

    if (!node) {
        return BCM_E_NOT_FOUND;
    }
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        return BCM_E_PARAM;
    }

    if (node->hw_index % 8 == 0) {

        node->remote_modid = dest_modid;
        node_base = mmu_info->queue_node;
        count = 0;
        for(index = mmu_info->base_dmvoq_queue ; 
                index < mmu_info->num_queues ; index++) {
            if (node_base[index].remote_modid == dest_modid) {
                count++;
                BCM_IF_ERROR_RETURN
                    (_bcm_kt2_cosq_node_get(unit, node_base[index].gport, 
                                            NULL ,&port, NULL, NULL));
                SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &val));
                base_queue = soc_reg_field_get(unit, ING_COS_MODEr, val, BASE_QUEUE_NUMf);

                SOC_IF_ERROR_RETURN (soc_mem_read(unit, PHYSICAL_PORT_BASE_QUEUEm,
                            MEM_BLOCK_ANY, port,
                            &phy_port_entry));
                phy_base_index = soc_mem_field32_get(unit, PHYSICAL_PORT_BASE_QUEUEm,
                        &phy_port_entry, BASE_QUEUE_NUMBERf);
                if (!soc_mem_field32_get(unit, PHYSICAL_PORT_BASE_QUEUEm,
                            &phy_port_entry, BASE_QUEUE_NUMBERf)) { 
                    soc_mem_field32_set(unit, PHYSICAL_PORT_BASE_QUEUEm,
                            &phy_port_entry, BASE_QUEUE_NUMBERf, 
                            (node_base[index].hw_index - base_queue) );
                    SOC_IF_ERROR_RETURN (soc_mem_write(unit, PHYSICAL_PORT_BASE_QUEUEm,
                                MEM_BLOCK_ALL, port,
                                &phy_port_entry));
                    offset = 0;
                } else {
                    if ( phy_base_index == node_base[index].hw_index - base_queue) {
                        if ( port == local_port ) {
                            offset = node->hw_index - node_base[index].hw_index;
                        }
                    }
                }
            }
        }
        SOC_IF_ERROR_RETURN
            (READ_MODPORT_MAP_SELr(unit, local_port, &val));

        index = soc_reg_field_get(unit, MODPORT_MAP_SELr, val,
                MODPORT_MAP_INDEX_UPPERf);

        modport_map_index  = (index * 256) + dest_modid;

        soc_mem_lock(unit, MODPORT_MAP_SWm);
        rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY, modport_map_index,
                &sw_entry);

        if(count == 1) {
            if (SOC_SUCCESS(rv)) {
                enable = soc_mem_field32_get(unit, MODPORT_MAP_SWm, &sw_entry, 
                        ENABLE0f); 
                if (!enable) { 
                    stack_port = ((local_port << 8) | local_port);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, DEST0f,
                            stack_port);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ISTRUNK0f,0);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ADD_DESTINATION_PORTf,
                            0);
                    soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ENABLE0f, 1);
                }
                soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ADD_DESTINATION_PORTf,
                        1);
                soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, PER_MODID_QUEUEING_ENABLEf,
                        1);
                soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, QUEUE_MAP_POINTERf,
                        node->hw_index);
                rv = soc_mem_write(unit, MODPORT_MAP_SWm, MEM_BLOCK_ALL, modport_map_index,
                        &sw_entry);
            }
            soc_mem_unlock(unit, MODPORT_MAP_SWm);

            sal_memset(map_entries, 0, sizeof(map_entries));
            /* Hardcoding the ING_QUEUE_OFFSET_MAPPING_TABLE */
            for (index = 0; index < 16; index++) {
                soc_mem_field32_set(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm,
                        &map_entries[index], QUEUE_OFFSETf, index > 7 ? 7 : index);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_offset_map_table_entry_add(unit, entries, 16, &index));
            SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, local_port, &val));
            base_queue = soc_reg_field_get(unit, ING_COS_MODEr, val, BASE_QUEUE_NUMf);

            for (port = 0;  port < KT2_LPBK_PORT; port++) {
                sal_memset(&ing_queue_entry, 0, sizeof(ing_queue_map_entry_t));
                soc_mem_field32_set(unit, ING_QUEUE_MAPm,
                        &ing_queue_entry, QUEUE_SET_BASEf, base_queue);
                soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                        QUEUE_OFFSET_PROFILE_INDEXf, index/16);
                soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                        ADD_EGRESS_PORT_BASE_QUEUEf, 0);
                BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, ING_QUEUE_MAPm,
                            node->hw_index + port , &ing_queue_entry));
            }

            sal_memset(&ing_queue_entry, 0, sizeof(ing_queue_map_entry_t));
            soc_mem_field32_set(unit, ING_QUEUE_MAPm,
                    &ing_queue_entry, QUEUE_SET_BASEf, offset);
            soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                    QUEUE_OFFSET_PROFILE_INDEXf, index/16);
            soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                    ADD_EGRESS_PORT_BASE_QUEUEf, 1);
            BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit, ING_QUEUE_MAPm,
                        node->hw_index + dest_port, &ing_queue_entry));

        } else if (offset > 0) {
            if (SOC_SUCCESS(rv)) {
                hw_index = soc_mem_field32_get(unit, MODPORT_MAP_SWm, &sw_entry, 
                        QUEUE_MAP_POINTERf); 
                soc_mem_unlock(unit, MODPORT_MAP_SWm);
                sal_memset(&ing_queue_entry, 0, sizeof(ing_queue_map_entry_t));
                rv = BCM_XGS3_MEM_READ(unit, ING_QUEUE_MAPm,
                        hw_index, &ing_queue_entry);
                if (!SOC_SUCCESS(rv)) {
                    return rv;
                }
                soc_mem_field32_set(unit, ING_QUEUE_MAPm,
                        &ing_queue_entry, QUEUE_SET_BASEf, offset);
                soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                        ADD_EGRESS_PORT_BASE_QUEUEf, 1);
                rv = BCM_XGS3_MEM_WRITE(unit, ING_QUEUE_MAPm,
                        hw_index + dest_port, &ing_queue_entry);
                if (!SOC_SUCCESS(rv)) {
                    return rv;
                }

            }
        } else {
            soc_mem_unlock(unit, MODPORT_MAP_SWm);
        }

        intf_index = _bcm_kt2_cosq_source_intf_set(unit, dest_modid, 
                0, _BCM_KT2_COSQ_DPVOQ);
        if (intf_index < 0) {
            return BCM_E_RESOURCE;
        }

        /*
         *  SRC_INTF and MMU_INTFI_BASE_INDEX_TBL is mapped one to one,
         *  so writing the base index at same index where SRC_INTF is written
         */

        cng_offset = intf_index * 64 ;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, MMU_INTFI_BASE_INDEX_TBLm, 
                          MEM_BLOCK_ALL, intf_index, &base_tbl_entry));
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                BASE_ADDRf, cng_offset);
        soc_mem_field32_set(unit, MMU_INTFI_BASE_INDEX_TBLm, &base_tbl_entry, 
                ENf, (node->level - 3 ));

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_INTFI_BASE_INDEX_TBLm,
                    MEM_BLOCK_ALL, intf_index, &base_tbl_entry));

        /* set the mapping from congestion bits in flow control table
         * to corresponding hw_index */
        BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_map_fc_status_to_node(unit, 
                    _BCM_KT2_COSQ_FC_VOQFC,  
                    node->hw_index , (node->level - 2), cng_offset + dest_port));


    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_kt2_cosq_gport_congestion_config_set 
 * Purpose:
 *      Setting the dmvoq/dpvoq related configuration
 * Parameters:
 *      unit           - (IN) Unit number.
 *      gport          - (IN) gport identifier 
 *      cos            - (IN) cos value 
 *      config         - (IN) congestion data 
 * Returns:
 *      BCM_E_xxx
 */

int bcm_kt2_cosq_gport_congestion_config_set(int unit, bcm_gport_t gport, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    /* config is used by DMVOQ/DPVOQ
     * E2ECC COE FC will ignore the config
     * and cos parameters.
     */

    bcm_port_t subport;
    _bcm_kt2_cosq_node_t *node;
    int max_subport = SOC_INFO(unit).pp_port_index_max;
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL , &subport, NULL, &node));

    /* E2ECC on COE -queue added to subport */
    if (soc_feature(unit, soc_feature_subtag_coe) &&
        (_bcm_is_port_linkphy_subport(unit, gport, -1) &&
        (subport <= max_subport))) {
        if (!IS_SUBTAG_PORT(unit, node->local_port)) {
            return BCM_E_PORT;
        }
        return _bcm_kt2_cosq_gport_e2ecc_coe_config_set(unit, gport);
    }

    if (!config) {
        return BCM_E_PARAM;
    }
/* when user specify dest_modid, fabric_port, fabric_modid 
   then we consider as DMVOQ */
    if ((config->dest_modid != -1)  && 
            (config->fabric_port != -1) &&
            (config->fabric_modid != -1)) {
        return _bcm_kt2_cosq_gport_dmvoq_config_set(unit, gport, cosq, 
                                               config->dest_modid,
                                               config->fabric_modid,
                                               config->fabric_port);
    }
/* when user specify dest_modid, dest_port with fabric_port, fabric_modid as -1
   then we consider as DPVOQ */
 
    if ((config->dest_modid != -1)  && 
            (config->dest_port != -1)) {
        return _bcm_kt2_cosq_gport_dpvoq_config_set(unit, gport, cosq, 
                config->dest_modid,
                config->dest_port);
    } 
    return BCM_E_PARAM;
}

/*
 * Function:
 *      bcm_kt2_cosq_gport_congestion_config_set 
 * Purpose:
 *      Setting the dmvoq/dpvoq related configuration
 * Parameters:
 *      unit           - (IN) Unit number.
 *      gport          - (IN) gport identifier 
 *      cos            - (IN) cos value 
 *      config         - (IN) congestion data 
 * Returns:
 *      BCM_E_xxx
 */


int bcm_kt2_cosq_gport_congestion_config_get(int unit, bcm_gport_t port, 
                                         bcm_cos_queue_t cosq, 
                                         bcm_cosq_congestion_info_t *config)
{
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_mmu_info_t *mmu_info;
    int fabric_port = 0;
    int base_index = 0;
    mmu_info = _bcm_kt2_mmu_info[unit];

    if (!config) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, port, NULL, NULL, NULL, &node));


    if (!node) {
        return BCM_E_NOT_FOUND;
    }

    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
        return BCM_E_PARAM;
    }


    BCM_PBMP_ITER(node->fabric_pbmp, fabric_port) {
        config->fabric_port = fabric_port;
        break;
    }
    if ( node->hw_index % 8 != 0){
        /*base hw_index */
        base_index = node->hw_index - (node->hw_index % 8); 
        bcm_kt2_sw_hw_queue(unit, &base_index);  
        node = &(mmu_info->queue_node[base_index]); 
    }

    config->dest_modid = node->remote_modid;
    config->dest_port = node->remote_port;
    config->fabric_modid = node->fabric_modid;


    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_kt2_cosq_mapping_set
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
_bcm_kt2_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    _bcm_kt2_cosq_node_t *node;
    bcm_port_t local_port;
    ing_queue_map_entry_t ing_queue_entry;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    void *entries[1];
    int rv;
    uint32 old_index = 0;
    uint32 new_index = 0;
 
    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (gport == -1) {
        return BCM_E_UNAVAIL;
    }

    switch (flags) {
        case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_node_get(unit, gport, NULL ,&local_port, NULL, &node));
            if ((!BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(gport)) && (node->parent == NULL)) {
                return BCM_E_PARAM;
            }

            if ( node->hw_index % 8 == 0) {
                sal_memset(map_entries, 0, sizeof(map_entries));

                BCM_IF_ERROR_RETURN(
                        soc_mem_read(unit, ING_QUEUE_MAPm, MEM_BLOCK_ANY, node->hw_index,
                            &ing_queue_entry));
                old_index = soc_mem_field32_get(unit, ING_QUEUE_MAPm,
                        &ing_queue_entry, QUEUE_OFFSET_PROFILE_INDEXf);
                old_index *= 16;

                entries[0] = &map_entries;
                BCM_IF_ERROR_RETURN
                    (_bcm_offset_map_table_entry_get(unit, old_index, 16, entries));
                rv = _bcm_offset_map_table_entry_delete(unit, old_index);
                if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
                    return rv;
                }
                soc_mem_field32_set(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm, 
                        &map_entries[priority], QUEUE_OFFSETf, cosq);

                BCM_IF_ERROR_RETURN
                    (_bcm_offset_map_table_entry_add(unit, entries, 16, &new_index));

                soc_mem_field32_set(unit, ING_QUEUE_MAPm, &ing_queue_entry,
                        QUEUE_OFFSET_PROFILE_INDEXf,
                        new_index / 16);
                BCM_IF_ERROR_RETURN(
                        soc_mem_write(unit, ING_QUEUE_MAPm, MEM_BLOCK_ANY, 
                            node->hw_index, &ing_queue_entry));
            }


            break;
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_kt2_cosq_mapping_get
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
_bcm_kt2_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    _bcm_kt2_cosq_node_t *node;
    void *entries[1];
    bcm_port_t local_port;
    ing_queue_map_entry_t ing_queue_entry;
    ing_queue_offset_mapping_table_entry_t map_entries[16];
    uint32 index = 0;
 
    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }


    switch (flags) {
        case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
            BCM_IF_ERROR_RETURN
                (_bcm_kt2_cosq_node_get(unit, *gport, NULL ,&local_port, NULL, &node));
            if ((!BCM_GPORT_IS_DESTMOD_QUEUE_GROUP(*gport)) && (node->parent == NULL)) {
                return BCM_E_PARAM;
            }

            sal_memset(map_entries, 0, sizeof(map_entries));

            entries[0] = &map_entries;
            BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, ING_QUEUE_MAPm, MEM_BLOCK_ANY, node->hw_index,
                        &ing_queue_entry));
            index = soc_mem_field32_get(unit, ING_QUEUE_MAPm,
                    &ing_queue_entry, QUEUE_OFFSET_PROFILE_INDEXf);
            index *= 16;

            BCM_IF_ERROR_RETURN
                (_bcm_offset_map_table_entry_get(unit, index, 16, entries));
            *cosq = soc_mem_field32_get(unit, ING_QUEUE_OFFSET_MAPPING_TABLEm, 
                    &map_entries[priority], QUEUE_OFFSETf);

            break;
        default:
            return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}
int
bcm_kt2_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    return _bcm_kt2_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

int
bcm_kt2_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    return _bcm_kt2_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

int
bcm_kt2_cosq_congestion_mapping_set(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config)
{
    int profile_index , index, desired_congestion_state;
    int i ,j , value, bit_position, max_value = 0, max_congestion_state;
    mmu_intfi_st_trans_tbl_entry_t st_trans_entry;
    int desired_congestion_value[8] = 
    {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80} ;
   
    /* CHECK if valid fabric_modid is passed. */
    if (fabric_modid < 0) {
        return BCM_E_BADID;
    }
    profile_index = _bcm_kt2_cosq_source_intf_profile_get(unit, fabric_modid, 
            0, _BCM_KT2_COSQ_DMVOQ);
    if (profile_index == -1 ) {
        return BCM_E_PARAM;
    }
    
    /* Find the Max flow control bit */
    for (i = 0; i < 8 ; i++) {
        if (max_value < config->flow_control_bits[i]) {
            max_value =  config->flow_control_bits[i];
        }
    }
    max_congestion_state = 1 << (max_value + 1) ;

    if (max_congestion_state > 256 ) {
        return BCM_E_PARAM;
    }

    /* Configure all the congestion states as per 
       the flow control bits passed */
    for ( i = 0; i < max_congestion_state ; i++) {
        desired_congestion_state = 0;
        value = i;
        bit_position = 0;
        /* Loop to set all the congestion states bit by bit */
        while ( value != 0 ) {
            if (value & 0x1) {
                for (j = 0 ; j < 8 ; j++) {
                    if (config->flow_control_bits[j] == bit_position) { 
                        desired_congestion_state |= 
                            desired_congestion_value[j];
                    }
                }
            }
            bit_position++;
            value = value >> 1;
        }

        index = (profile_index  * 256) + i ;
        SOC_IF_ERROR_RETURN(READ_MMU_INTFI_ST_TRANS_TBLm(
                    unit, MEM_BLOCK_ALL, index,
                    &st_trans_entry));
        soc_mem_field32_set(unit, MMU_INTFI_ST_TRANS_TBLm,
                &st_trans_entry, FC_ST_XLATEf,
                desired_congestion_state);
        SOC_IF_ERROR_RETURN(WRITE_MMU_INTFI_ST_TRANS_TBLm(
                    unit, MEM_BLOCK_ALL, index,
                    &st_trans_entry));


    }
    /* Configure remaining the congestion states as zero */ 
 
    if (max_congestion_state < 256 ) {
        for ( i = max_congestion_state; i < 256 ; i++) {
            index = (profile_index  * 256) + i ;
            SOC_IF_ERROR_RETURN(READ_MMU_INTFI_ST_TRANS_TBLm(
                        unit, MEM_BLOCK_ALL, index,
                        &st_trans_entry));
            soc_mem_field32_set(unit, MMU_INTFI_ST_TRANS_TBLm,
                    &st_trans_entry, FC_ST_XLATEf,
                    0);
            SOC_IF_ERROR_RETURN(WRITE_MMU_INTFI_ST_TRANS_TBLm(
                        unit, MEM_BLOCK_ALL, index,
                        &st_trans_entry));
        }
    }


    return BCM_E_NONE;

}

int 
bcm_kt2_cosq_congestion_mapping_get(int unit,int fabric_modid ,
        bcm_cosq_congestion_mapping_info_t *config)
{
    int index, profile_index, i;
    mmu_intfi_st_trans_tbl_entry_t st_trans_entry;
    int congestion_data , bit_position;
  
    /* CHECK if valid fabric_modid is passed. */
    if (fabric_modid < 0) {
        return BCM_E_BADID;
    }
    profile_index = _bcm_kt2_cosq_source_intf_profile_get(unit, fabric_modid, 
            0, _BCM_KT2_COSQ_DMVOQ);
    if (profile_index == -1 ) {
        return BCM_E_PARAM;
    }

    /* Initially fill all the control bits as -1 */

    for (i = 0; i < 8 ; i++) {
        config->flow_control_bits[i] = -1;
    }
    /* Read the congestion state at index 1,2,4,8,16,32,64,128 and update the control bits
       accordingly */
    for (i = 0; i < 8 ; i++) {
        index = ((profile_index  * 256) + (1 << i)) ;

        SOC_IF_ERROR_RETURN(READ_MMU_INTFI_ST_TRANS_TBLm(
                    unit, MEM_BLOCK_ALL, index,
                    &st_trans_entry));
        congestion_data = 
            soc_mem_field32_get(unit, MMU_INTFI_ST_TRANS_TBLm,
                    &st_trans_entry, FC_ST_XLATEf);
        bit_position = 0;
        /* Loop to check all the bits in the congestion data */
        while ( congestion_data != 0 ) {
            if (congestion_data & 0x1) {
                config->flow_control_bits[bit_position] = i;
            }
            bit_position++;
            congestion_data = congestion_data >> 1;
        }
    }
    return BCM_E_NONE;
}


int
_bcm_kt2_cosq_dynamic_port_encap_change(int unit,bcm_port_t port) 
{

   bcm_gport_t gport;
   _bcm_kt2_mmu_info_t *mmu_info;
   _bcm_kt2_cosq_list_t *list;
   int numq,cosq; 
   int num_cos = 0;
   int id = 0 ;
   int mcid = 0;
   uint32 rval;
   bcm_gport_t port_sched, l0_sched, l1_sched, queue;
   int rv;
   int limit,offset;
   soc_info_t *si;
   int queuenum = 0;

   si = &SOC_INFO(unit);
   mmu_info = _bcm_kt2_mmu_info[unit];
   
#if defined BCM_METROLITE_SUPPORT
   if (soc_feature(unit, soc_feature_discontinuous_pp_port) &&
       (SOC_PBMP_MEMBER(si->linkphy_pbm, port) ||
        SOC_PBMP_MEMBER(si->subtag_pbm, port))) {
      /* Port encap change should not be called for Linkphy ports */
      return BCM_E_PARAM;
   }
#endif

   bcm_kt2_cosq_config_get(unit, &num_cos);
   BCM_IF_ERROR_RETURN
   (bcm_esw_port_gport_get(unit, port , &gport));


   list = &mmu_info->global_qlist;
   offset = 0;

   /* We do not want to recreate lls tree if we're not involving
    * HG ports.
    */
   if ((si->port_num_uc_cosq[port] == KT2_DEFAULT_NUM_PHYSICAL_QUEUES) && 
       (!IS_HG_PORT(unit, port))) {
       return BCM_E_NONE;
   }
   if ((si->port_num_uc_cosq[port] == KT2_HG_NUM_PHYSICAL_QUEUES) &&
       (IS_HG_PORT(unit, port))) {
        return BCM_E_NONE;
   }
   if (IS_HG_PORT(unit, port)) { 
       numq = KT2_HG_NUM_PHYSICAL_QUEUES; 
   }
   else { 
       numq = KT2_DEFAULT_NUM_PHYSICAL_QUEUES ; 
   }
   limit = mmu_info->num_base_queues;

   rv = _bcm_kt2_node_index_get(unit, list->bits, offset, limit,
                                            mmu_info->qset_size, numq, &id);
   if( rv != BCM_E_NONE) { 
      return BCM_E_RESOURCE; 

   }
   if (soc_feature(unit, soc_feature_mmu_packing)) {
   /* reserving queues for mc if its packing */
       rv = _bcm_kt2_node_index_get(unit, list->bits, (id + numq), limit,
                                            mmu_info->qset_size, numq, &mcid);
       if (rv != BCM_E_NONE) { 
           return BCM_E_RESOURCE; 
       }
   }
   BCM_IF_ERROR_RETURN
   ( bcm_kt2_cosq_gport_delete(unit , gport));

   BCM_IF_ERROR_RETURN
   (_bcm_kt2_node_index_clear(list, si->port_uc_cosq_base[port] ,
                              si->port_num_uc_cosq[port]));

   BCM_IF_ERROR_RETURN
   (_bcm_kt2_node_index_set(list ,id, numq)); 
    si->port_uc_cosq_base[port] = id ; 
    si->port_cosq_base[port] = id ; 
    si->port_num_uc_cosq[port] = numq ; 
    mmu_info->port[port].q_offset = id;
    mmu_info->port[port].q_limit = id + numq;
    if (soc_feature(unit, soc_feature_mmu_packing)) {
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_node_index_clear(list, si->port_cosq_base[port] ,
                              si->port_num_cosq[port]));
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_node_index_set(list, mcid, numq)); 
        si->port_cosq_base[port] = mcid ; 
        si->port_num_cosq[port] = numq; 
        mmu_info->port[port].mcq_offset = mcid;
        mmu_info->port[port].mcq_limit = mcid + numq  ;
    }

    SOC_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
    soc_reg_field_set(unit, ING_COS_MODEr, &rval, BASE_QUEUE_NUMf,
                      mmu_info->port[port].q_offset);
    if (IS_HG_PORT(unit, port)) {
        soc_reg_field_set(unit, ING_COS_MODEr, &rval, COS_MODEf, 2);  
    }
    else { 
        soc_reg_field_set(unit, ING_COS_MODEr, &rval, COS_MODEf, 0);  
    }
    SOC_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));
   
    SOC_IF_ERROR_RETURN(READ_RQE_PP_PORT_CONFIGr(unit, port, &rval));
    soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr, &rval, BASE_QUEUEf,
                      (soc_feature(unit, soc_feature_mmu_packing)) ? 
                       mmu_info->port[port].mcq_offset :
                       mmu_info->port[port].q_offset);

    if (IS_HG_PORT(unit, port)) {
        soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                            &rval, COS_MODEf, 2);
    }
    else { 
        soc_reg_field_set(unit, RQE_PP_PORT_CONFIGr,
                            &rval, COS_MODEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_RQE_PP_PORT_CONFIGr(unit, port, rval));
            
    for (queuenum = mmu_info->port[port].q_offset;
         queuenum < mmu_info->port[port].q_limit; queuenum++) {
         SOC_IF_ERROR_RETURN (soc_mem_field32_modify(unit,
              EGR_QUEUE_TO_PP_PORT_MAPm, queuenum, PP_PORTf, port));
    }

    if (soc_feature(unit, soc_feature_mmu_packing)) {
        for (queuenum = mmu_info->port[port].mcq_offset;
             queuenum < mmu_info->port[port].mcq_limit; queuenum++) {
             SOC_IF_ERROR_RETURN (soc_mem_field32_modify(unit,
                       EGR_QUEUE_TO_PP_PORT_MAPm, queuenum, PP_PORTf, port));
        }
   }
    if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
        (_bcm_kt2_port_lls_config_set(unit, port, 
         si->port_uc_cosq_base[port], 
         si->port_num_uc_cosq[port], 
        num_cos));
    } else {
      if (!soc_feature(unit, soc_feature_mmu_packing)) {
          BCM_IF_ERROR_RETURN
          (bcm_kt2_cosq_gport_add(unit, gport, 1, 0, &port_sched));
          BCM_IF_ERROR_RETURN
           (bcm_kt2_cosq_gport_add(unit, port, 1, 
           BCM_COSQ_GPORT_SCHEDULER, 
                           &l0_sched));
          BCM_IF_ERROR_RETURN
           (bcm_kt2_cosq_gport_attach(unit, l0_sched, port_sched, 0)); 
          BCM_IF_ERROR_RETURN
           (bcm_kt2_cosq_gport_add(unit, port,num_cos, 
                                 BCM_COSQ_GPORT_SCHEDULER, 
                                     &l1_sched));
          BCM_IF_ERROR_RETURN
           (bcm_kt2_cosq_gport_attach(unit, l1_sched, l0_sched, 0));  
          for (cosq = 0; cosq < num_cos; cosq++) {
               BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_gport_add(unit, port, 1,
                BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue));
               BCM_IF_ERROR_RETURN
                  (bcm_kt2_cosq_gport_attach(unit, queue, l1_sched, 
                                               cosq));  
               }
      }else {
            BCM_IF_ERROR_RETURN
            (_bcm_kt2_packing_config_set(unit, port, num_cos ));
      }
  }
return BCM_E_NONE; 

}

/*
 * Function:
 *     _bcm_kt2_port_enqueue_set
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
_bcm_kt2_port_enqueue_set(int unit, bcm_port_t gport,
                          int enable)
{
    bcm_port_t local_port;
    uint64 tmp64, r64val, rval64;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, gport, &local_port));

    if (!BCM_PBMP_MEMBER((PBMP_ALL(unit)), local_port)) {
        return BCM_E_PORT; 
    }
    /* Enable in TX direction*/
    BCM_IF_ERROR_RETURN
        (soc_katana2_mmu_port_enable_set(unit, local_port, enable));

    /* Enable in RX direction*/
    SOC_IF_ERROR_RETURN(READ_INPUT_PORT_RX_ENABLE_64r(unit, &r64val));
    rval64 = soc_reg64_field_get(unit, INPUT_PORT_RX_ENABLE_64r, r64val,
                                 INPUT_PORT_RX_ENABLEf);
    COMPILER_64_SET(tmp64, 0, 1);
    COMPILER_64_SHL(tmp64, (local_port));
    if (enable) {
        COMPILER_64_OR(rval64, tmp64);
    } else {
        COMPILER_64_NOT(tmp64);
        COMPILER_64_AND(rval64, tmp64);
    }
    
    soc_reg64_field_set(unit, INPUT_PORT_RX_ENABLE_64r, &r64val,
                          INPUT_PORT_RX_ENABLEf, rval64);
    SOC_IF_ERROR_RETURN(WRITE_INPUT_PORT_RX_ENABLE_64r(unit, r64val));
    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_kt2_cosq_gport_e2ecc_transmit_set 
 * Purpose:
 *     Enable or Disable queue of a given port for e2ecc reporting. 
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port
 *     cosq       - (IN) cosq number (0-7) -1 for all queues.
 *     type       - (IN) Flag indicating e2ecc transmit.
 *     arg        - (IN) TRUE to enable
 *                       FALSE to disable
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_gport_e2ecc_transmit_set(int unit, bcm_gport_t gport, 
                                      bcm_cos_queue_t cosq,
                                      bcm_cosq_control_t type, 
                                      int arg)
{
    bcm_port_t port;
    uint32     rval;
    uint32     fval;
    uint64     r64val;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_gport_e2ecc_transmit_set: \
                          unit=%d gport=0x%x cosq=%d \
                          type=%d arg=%d\n"),
                          unit, gport, cosq, type, arg));
    switch (type) {
    case bcmCosqControlE2ECCTransmitEnable:
         if ((!BCM_COSQ_QUEUE_VALID(unit, cosq)) && (cosq != -1)) {
                return BCM_E_PARAM;
         }
         if (BCM_GPORT_IS_SET(gport)) {
             BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
         } else {
             port = gport;
         }
         if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
             return BCM_E_PORT;
         }
 
         BCM_IF_ERROR_RETURN(READ_OP_E2ECC_PORT_CONFIGr(unit, port, &rval)); 
         fval = soc_reg_field_get(unit, OP_E2ECC_PORT_CONFIGr,
                                  rval, COS_MASKf);
         if (cosq != -1) {
             if (arg) {
                 fval |= (1 << cosq);
             } else {
                 fval &= ~(1 << cosq);
             }
         } else {
             if (arg) {
                 fval = 0xff;
             } else {
                 fval = 0;
             }
        }
        soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr,
                          &rval, COS_MASKf, fval);
        soc_reg_field_set(unit, OP_E2ECC_PORT_CONFIGr, &rval,
                          E2ECC_RPT_ENf, arg ? 1 : 0);
        BCM_IF_ERROR_RETURN(WRITE_OP_E2ECC_PORT_CONFIGr(unit, port, rval));
        
        /* Now enable monitoring in INTFO if at least one queue active */
        COMPILER_64_ZERO(r64val);
        soc_reg64_field32_set(unit, MMU_INTFO_CONGST_STr,
                              &r64val, ENf, fval ? 1: 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_INTFO_CONGST_STr(unit,
                            port, r64val));
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_gport_e2ecc_transmit_get
 * Purpose:
 *     Get the Enable or Disable value for a queue of a given port for e2ecc reporting.
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port
 *     cosq       - (IN) cosq number (0-7) -1 for all queues.
 *     type       - (IN) Flag indicating e2ecc transmit.
 *     arg        - (OUT)TRUE to enable
 *                       FALSE to disable
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_kt2_cosq_gport_e2ecc_transmit_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      bcm_cosq_control_t type,
                                      int *arg)
{
    bcm_port_t port;
    uint32     rval;
    uint32     fval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_kt2_cosq_gport_e2ecc_transmit_set: \
                          unit=%d gport=0x%x cosq=%d \
                          type=%d \n"),
                          unit, gport, cosq, type));
    switch (type) {
    case bcmCosqControlE2ECCTransmitEnable:
         if (!BCM_COSQ_QUEUE_VALID(unit, cosq)) {
                return BCM_E_PARAM;
         }
         if (BCM_GPORT_IS_SET(gport)) {
             BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, gport, &port));
         } else {
             port = gport;
         }
         if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
             return BCM_E_PORT;
         }
 
         BCM_IF_ERROR_RETURN(READ_OP_E2ECC_PORT_CONFIGr(unit, port, &rval));
         fval = soc_reg_field_get(unit, OP_E2ECC_PORT_CONFIGr,
                                  rval, COS_MASKf);
         fval = fval & (1 << cosq);  
         *arg = fval ? 1 : 0; 
         break;
    default:
         return BCM_E_UNAVAIL; 
    }

    return BCM_E_NONE;
}
#if defined(BCM_SABER2_SUPPORT)
STATIC int
_bcm_kt2_get_pg_profile_mapping(int unit, int profile_index,
                                uint32 *reg0, uint32 *reg1)
{
    uint32 rval, rval1;

    if (profile_index < 0 || profile_index > 3) {
        return BCM_E_PARAM;
    }
    if (profile_index == 0) {
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_0_PRI_GRP0r(unit, &rval));
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_0_PRI_GRP1r(unit, &rval1));
    } else if (profile_index == 1) {
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_1_PRI_GRP0r(unit, &rval));
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_1_PRI_GRP1r(unit, &rval1));
    } else if (profile_index == 2) {
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_2_PRI_GRP0r(unit, &rval));
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_2_PRI_GRP1r(unit, &rval1));
    } else {
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_3_PRI_GRP0r(unit, &rval));
        SOC_IF_ERROR_RETURN
                (READ_MMU_ENQ_PROFILE_3_PRI_GRP1r(unit, &rval1));
   }
   *reg0 = rval;
   *reg1 = rval1;
   return BCM_E_NONE;
}

STATIC int
_bcm_kt2_set_pg_profile_mapping(int unit, int profile_index,
                                uint32 reg0, uint32 reg1)
{

    if (profile_index < 0 || profile_index > 3) {
        return BCM_E_PARAM;
    }
    if (profile_index == 0) {
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_0_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_0_PRI_GRP1r(unit, reg1));
    } else if (profile_index == 1) {
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_1_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_1_PRI_GRP1r(unit, reg1));
    } else if (profile_index == 2) {
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_2_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_2_PRI_GRP1r(unit, reg1));
    } else {
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_3_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_MMU_ENQ_PROFILE_3_PRI_GRP1r(unit, reg1));
   }
   return BCM_E_NONE;
}

STATIC int
_bcm_kt2_get_port_profile_index(int unit, bcm_port_t port, int *p_index)
{
    uint64 rval64;
    soc_field_t sb2_pri_to_prigrp_field[SB2_MAX_LOGICAL_PORTS]={
            SRCPORT_0f,
            SRCPORT_1f,  SRCPORT_2f,  SRCPORT_3f,  SRCPORT_4f,
            SRCPORT_5f,  SRCPORT_6f,  SRCPORT_7f,  SRCPORT_8f,
            SRCPORT_9f,  SRCPORT_10f, SRCPORT_11f, SRCPORT_12f,
            SRCPORT_13f, SRCPORT_14f, SRCPORT_15f, SRCPORT_16f,
            SRCPORT_17f, SRCPORT_18f, SRCPORT_19f, SRCPORT_20f,
            SRCPORT_21f, SRCPORT_22f, SRCPORT_23f, SRCPORT_24f,
            SRCPORT_25f, SRCPORT_26f, SRCPORT_27f, SRCPORT_28f,
            SRCPORT_29f
            };
    if(SOC_IS_SABER2(unit))  {
       SOC_IF_ERROR_RETURN(
           READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, &rval64));
       if (soc_reg_field_valid(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r,
                               sb2_pri_to_prigrp_field[port])) {
           *p_index = soc_reg64_field32_get(unit,
                                            MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r,
                                            rval64,
                                            sb2_pri_to_prigrp_field[port]);
            return BCM_E_NONE;
       }
    }
    return BCM_E_PARAM;
}
STATIC int
_bcm_kt2_config_all_pg_profile_register(int unit, bcm_port_t port)
{
    int  profile_index;
    uint32 reg0, reg1;
    uint64 rval64;

    BCM_IF_ERROR_RETURN(_bcm_kt2_get_port_profile_index(unit, port, &profile_index));
    BCM_IF_ERROR_RETURN(_bcm_kt2_get_pg_profile_mapping(unit, profile_index,
                                                        &reg0, &reg1));
    if (profile_index < 0 || profile_index > 3) {
        return BCM_E_PARAM;
    }
    if (profile_index == 0) {
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE0_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE0_PRI_GRP1r(unit, reg1));
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE0_PRI_GRP0r)) { 
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE0_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE0_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE0_PRI_GRP1r(unit, reg1));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE0_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE0_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE0_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE0_PRI_GRP1r(unit, reg1));
        }
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE0_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE0_PRI_GRP1r(unit, reg1));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE0_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE0_PRI_GRP1r(unit, reg1));

    } else if (profile_index == 1) {
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE1_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE1_PRI_GRP1r(unit, reg1));
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE1_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE1_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE1_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE1_PRI_GRP1r(unit, reg1));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE1_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE1_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE1_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE1_PRI_GRP1r(unit, reg1));
        } 
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE1_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE1_PRI_GRP1r(unit, reg1));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE1_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE1_PRI_GRP1r(unit, reg1));
    } else if (profile_index == 2) {
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE2_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE2_PRI_GRP1r(unit, reg1));
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE2_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE2_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE2_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE2_PRI_GRP1r(unit, reg1));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE2_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE2_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE2_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE2_PRI_GRP1r(unit, reg1));
        }
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE2_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE2_PRI_GRP1r(unit, reg1));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE2_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE2_PRI_GRP1r(unit, reg1));
    } else {
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE3_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_PROFILE3_PRI_GRP1r(unit, reg1));
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE3_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE3_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEXT_PROFILE3_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEXT_PROFILE3_PRI_GRP1r(unit, reg1));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE3_PRI_GRP0r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE3_PRI_GRP0r(unit, reg0));
        }
        if (SOC_REG_IS_VALID(unit, THDIEMA_PROFILE3_PRI_GRP1r)) {
            SOC_IF_ERROR_RETURN
                    (WRITE_THDIEMA_PROFILE3_PRI_GRP1r(unit, reg1));
        } 
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE3_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIRQE_PROFILE3_PRI_GRP1r(unit, reg1));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE3_PRI_GRP0r(unit, reg0));
        SOC_IF_ERROR_RETURN
                (WRITE_THDIQEN_PROFILE3_PRI_GRP1r(unit, reg1));
    }
    SOC_IF_ERROR_RETURN(
            READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, &rval64));

    SOC_IF_ERROR_RETURN(
         WRITE_PORT_PROFILE_MAPr(unit, rval64));
    if (SOC_REG_IS_VALID(unit, THDIEXT_PORT_PROFILE_MAPr)) {
        SOC_IF_ERROR_RETURN(
            WRITE_THDIEXT_PORT_PROFILE_MAPr(unit, rval64));
    }
    if (SOC_REG_IS_VALID(unit, THDIEMA_PORT_PROFILE_MAPr)) {
        SOC_IF_ERROR_RETURN(
            WRITE_THDIEMA_PORT_PROFILE_MAPr(unit, rval64));
    }
    SOC_IF_ERROR_RETURN(
         WRITE_THDIRQE_PORT_PROFILE_MAPr(unit, rval64));
    SOC_IF_ERROR_RETURN(
         WRITE_THDIQEN_PORT_PROFILE_MAPr(unit, rval64));
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_check_port_pg_status(int unit, int port, int new_pg, int old_pg)
{
    uint32    new_mem_idx, old_mem_idx;
    thdi_port_pg_cntrs_entry_t thdi_port_pg_cntrs={{0}};
    thdiext_thdi_port_pg_cntrs_entry_t thdiext_thdi_port_pg_cntrs={{0}};
    thdiema_thdi_port_pg_cntrs_entry_t thdiema_thdi_port_pg_cntrs={{0}};
    thdirqe_thdi_port_pg_cntrs_entry_t thdirqe_thdi_port_pg_cntrs={{0}};
    thdiqen_thdi_port_pg_cntrs_entry_t thdiqen_thdi_port_pg_cntrs = {{0}};
    
    new_mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + new_pg;
    old_mem_idx= (port * SB2_MAX_PRIORITY_GROUPS) + old_pg;

    /* Checm if pg min count is zero or not */
    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    new_mem_idx, &thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDI_PORT_PG_CNTRSm,
                &thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }
    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    old_mem_idx, &thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDI_PORT_PG_CNTRSm,
                &thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }
    if (SOC_MEM_IS_VALID(unit, THDIEXT_THDI_PORT_PG_CNTRSm)) {
        SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIEXT_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    new_mem_idx, &thdiext_thdi_port_pg_cntrs));
        if (soc_mem_field32_get(unit, THDIEXT_THDI_PORT_PG_CNTRSm,
                    &thdiext_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
            return BCM_E_BUSY;
        }
        SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIEXT_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    old_mem_idx, &thdiext_thdi_port_pg_cntrs));
        if (soc_mem_field32_get(unit, THDIEXT_THDI_PORT_PG_CNTRSm,
                    &thdiext_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
            return BCM_E_BUSY;
        }
    }
    if (SOC_MEM_IS_VALID(unit, THDIEMA_THDI_PORT_PG_CNTRSm)) { 
        SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIEMA_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    new_mem_idx, &thdiema_thdi_port_pg_cntrs));
        if (soc_mem_field32_get(unit, THDIEMA_THDI_PORT_PG_CNTRSm,
                    &thdiema_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
            return BCM_E_BUSY;
        }
        SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIEMA_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    old_mem_idx, &thdiema_thdi_port_pg_cntrs));
        if (soc_mem_field32_get(unit, THDIEMA_THDI_PORT_PG_CNTRSm,
                    &thdiema_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
            return BCM_E_BUSY;
        }
    } 
    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIRQE_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    new_mem_idx, &thdirqe_thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDIRQE_THDI_PORT_PG_CNTRSm,
                &thdirqe_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }
    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIRQE_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    old_mem_idx, &thdirqe_thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDIRQE_THDI_PORT_PG_CNTRSm,
                &thdirqe_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }

    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIQEN_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    new_mem_idx, &thdiqen_thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDIQEN_THDI_PORT_PG_CNTRSm,
                &thdiqen_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }
    SOC_IF_ERROR_RETURN (soc_mem_read(
                    unit, THDIQEN_THDI_PORT_PG_CNTRSm, MEM_BLOCK_ALL,
                    old_mem_idx, &thdiqen_thdi_port_pg_cntrs));
    if (soc_mem_field32_get(unit, THDIQEN_THDI_PORT_PG_CNTRSm,
                &thdiqen_thdi_port_pg_cntrs, PG_MIN_COUNTf)) {
        return BCM_E_BUSY;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_kt2_port_priority_group_mapping_set
 * Purpose:
 *      Assign a Priority Group for the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport      - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (IN) priority group ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_port_priority_group_mapping_set(int unit, bcm_gport_t gport,
                                        int prio, int priority_group)
{
    bcm_port_t port;
    uint32 old_reg0, old_reg1, new_reg0, new_reg1, temp = 0x7;
    int old_profile_index, new_profile_index, free_profile_index = -1;
    uint64 rval64;
    int old_pg = 0x7, new_pg = priority_group;
    soc_field_t sb2_srcport_field_names[SB2_MAX_LOGICAL_PORTS]={
            SRCPORT_0f,
            SRCPORT_1f,  SRCPORT_2f,  SRCPORT_3f,  SRCPORT_4f,
            SRCPORT_5f,  SRCPORT_6f,  SRCPORT_7f,  SRCPORT_8f,
            SRCPORT_9f,  SRCPORT_10f, SRCPORT_11f, SRCPORT_12f,
            SRCPORT_13f, SRCPORT_14f, SRCPORT_15f, SRCPORT_16f,
            SRCPORT_17f, SRCPORT_18f, SRCPORT_19f, SRCPORT_20f,
            SRCPORT_21f, SRCPORT_22f, SRCPORT_23f, SRCPORT_24f,
            SRCPORT_25f, SRCPORT_26f, SRCPORT_27f, SRCPORT_28f,
            SRCPORT_29f
            };

    /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
    if ((prio < KT2_SAFC_INPUT_PRIORITY_MIN) ||
        (prio > KT2_SAFC_INPUT_PRIORITY_MAX)) {
         return BCM_E_PARAM;
    }

    if ((priority_group < KT2_PRIOROTY_GROUP_ID_MIN) ||
        (priority_group > KT2_PRIOROTY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_localport_resolve(unit, gport, &port));
    if ((SB2_MAX_PHYSICAL_PORTS <= port) ||
        !_bcm_kt2_port_pfc_supported(unit, port)) {
        return BCM_E_PORT;
    }
    /* Get the profile index and correponding profile for the port */
    BCM_IF_ERROR_RETURN(_bcm_kt2_get_port_profile_index(unit, port, &old_profile_index));
    BCM_IF_ERROR_RETURN(_bcm_kt2_get_pg_profile_mapping(unit, old_profile_index,
                                                         &old_reg0, &old_reg1));
    if (prio < 8) {
        old_pg = ((old_pg << (prio * 3)) & old_reg0) >> (prio * 3);
    } else {
        old_pg = ((old_pg << ((prio - 8) * 3)) & old_reg1) >> ((prio - 8) * 3);
    }
    if (pg_profile_refcount[unit][old_profile_index] == 1) {
        free_profile_index = old_profile_index;
    }
    if (prio < 8) {
        temp = temp << (prio * 3);
        temp = ~temp;
        temp = old_reg0 & temp;
        temp = temp | (priority_group << (prio * 3));
        for (new_profile_index = 0; new_profile_index < 4; new_profile_index ++) {
             if (pg_profile_refcount[unit][new_profile_index] != 0) {
                 BCM_IF_ERROR_RETURN(_bcm_kt2_get_pg_profile_mapping(unit,
                                                            new_profile_index,
                                                            &new_reg0, &new_reg1));
                 if ((new_reg0 == temp) && (new_reg1 == old_reg1)) {
                      break;
                 }
             } else {
                 if (free_profile_index == -1) {
                     free_profile_index = new_profile_index;
                 }
             }
        }
        if ((new_profile_index == 4) && (free_profile_index == -1)) {
             return BCM_E_RESOURCE;
        }
    } else {
        temp = temp << ((prio - 8) * 3);
        temp = ~temp;
        temp = old_reg1 & temp;
        temp = temp | (priority_group << ((prio - 8) * 3));
        for (new_profile_index = 0; new_profile_index < 4; new_profile_index ++) {
             if (pg_profile_refcount[unit][new_profile_index] != 0) {
                 BCM_IF_ERROR_RETURN(_bcm_kt2_get_pg_profile_mapping(unit,
                                                            new_profile_index,
                                                            &new_reg0, &new_reg1));
                 if ((new_reg1 == temp) && (new_reg0 == old_reg0)) {
                     break;
                 }
             } else {
                 if (free_profile_index == -1) {
                     free_profile_index = new_profile_index;
                 }
             }
         }
         if ((new_profile_index == 4) && (free_profile_index == -1)) {
             return BCM_E_RESOURCE;
         }

    }
    /* We reached here either because of an existing profile match or a free profile */
    if (new_profile_index == 4) {
        new_profile_index = free_profile_index;
        if (prio < 8) {
            new_reg0 = temp;
            new_reg1 = old_reg1;
        } else {
            new_reg0 = old_reg0;
            new_reg1 = temp;
        }
    }
    /*
     * For dynamic configuration we need to check status of pgs before 
     * updating it. 
     */
    BCM_IF_ERROR_RETURN(_bcm_kt2_check_port_pg_status(unit, port, new_pg, old_pg));

    SOC_IF_ERROR_RETURN(
            READ_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, &rval64));
    BCM_IF_ERROR_RETURN(_bcm_kt2_set_pg_profile_mapping(unit, new_profile_index,
                                                        new_reg0, new_reg1));
    if (soc_reg_field_valid(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r,
                             sb2_srcport_field_names[port])) {
        soc_reg64_field32_set(unit, MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r,
                          &rval64, sb2_srcport_field_names[port],
                          new_profile_index);
    }
    SOC_IF_ERROR_RETURN(
         WRITE_MMU_ENQ_IP_PRI_TO_PG_PROFILE_0r(unit, rval64));

    BCM_IF_ERROR_RETURN(
            _bcm_kt2_config_all_pg_profile_register(unit, port));
    /* Now update refcount */
    pg_profile_refcount[unit][old_profile_index] = pg_profile_refcount[unit][old_profile_index] - 1;
    pg_profile_refcount[unit][new_profile_index] = pg_profile_refcount[unit][new_profile_index] + 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt2_port_priority_group_mapping_get
 * Purpose:
 *      Return Priority Group for the input priority.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport      - (IN) generic port.
 *      prio       - (IN) input priority.
 *      priority_group  - (OUT) priority group ID that the input priority mapped to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt2_port_priority_group_mapping_get(int unit, bcm_gport_t gport,
                                        int prio, int *priority_group)
{
    bcm_port_t port;
    uint32 reg0, reg1,temp = 0x7;
    int profile_index;

    /* PFC : 0-7, SAFC : 0-15. Taking the larger range for comparison */
    if ((prio < KT2_SAFC_INPUT_PRIORITY_MIN) ||
        (prio > KT2_SAFC_INPUT_PRIORITY_MAX)) {
         return BCM_E_PARAM;
    }

    if (priority_group == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_kt2_cosq_localport_resolve(unit, gport, &port));
    if ((SB2_MAX_PHYSICAL_PORTS <= port) ||
        !_bcm_kt2_port_pfc_supported(unit, port)) {
        return BCM_E_PORT;
    }
    /* Get the profile index and correponding profile for the port */
    BCM_IF_ERROR_RETURN(_bcm_kt2_get_port_profile_index(unit, port, &profile_index));
    BCM_IF_ERROR_RETURN(_bcm_kt2_get_pg_profile_mapping(unit, profile_index,
                                                        &reg0, &reg1));
    if (prio < 8) {
        temp = temp << (prio * 3);
        temp = reg0 & temp;
        temp = temp >> (prio * 3);
        *priority_group = temp;
    } else {
        temp = temp << ((prio - 8) * 3);
        temp = reg1 & temp;
        temp = temp >> ((prio - 8) * 3);
    }
    *priority_group = temp;
    return BCM_E_NONE;
}
#endif

STATIC int
_bcm_kt2_dump_config(int unit, bcm_port_t port,
                       _bcm_kt2_cosq_node_t *node)
{
    int num_spri, first_child, first_mc_child;
    uint32 spmap, ucmap;
    soc_kt2_sched_mode_e sched_mode = SOC_KT2_SCHED_MODE_UNKNOWN;
    int wt = 0;
    char *lvl_name[] = { "Root", "S0", "S1", "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};
    int subport = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si;
    int physical_port;
    bcm_gport_t pp_port;
    _bcm_kt2_subport_info_t info;
    soc_kt2_sched_mode_e mode = 0;
    int rv = BCM_E_NONE;
    int soc_node_level = node->level;
    int is_mc_queue;

    si = &SOC_INFO(unit);
    if ((!(SOC_PBMP_MEMBER(si->linkphy_pbm, port))) &&
                (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT)) {
        /* KT2 soc level does not include S0 and S1 */
        soc_node_level = soc_node_level - 2; 
    }

    /* get sched config */
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_L2) {
       SOC_IF_ERROR_RETURN(
            soc_kt2_sched_get_node_config(unit, port,
                        soc_node_level, node->hw_index,
                        &num_spri, &first_child,
                        &first_mc_child, &ucmap, &spmap));

    }
    sched_mode = 0;
    if (node->level != _BCM_KT2_COSQ_NODE_LEVEL_ROOT) {
        SOC_IF_ERROR_RETURN(
         soc_kt2_cosq_get_sched_mode(unit, port, soc_node_level,
                                node->hw_index, &sched_mode, &wt));
    }

    if (wt && SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {

        /* Re-write the node info if num_streams is 1 */
        if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_subport_get(unit, node->gport, &pp_port));
                BCM_IF_ERROR_RETURN
                   (bcm_kt2_subport_pp_port_subport_info_get(unit, pp_port,
                                                &info));
            if (info.num_streams == 1) {
                first_child = node->hw_index;
                num_spri = 1;
                spmap = 1;
                rv = soc_kt2_cosq_s0_sched_get(unit, node->hw_index, &mode);
                if (rv == BCM_E_NONE) {
                    num_spri = (mode == SOC_KT2_SCHED_MODE_STRICT) ? 1 : 0;
                    spmap = (mode == SOC_KT2_SCHED_MODE_STRICT) ? 1 : 0;
                }

#ifdef BCM_METROLITE_SUPPORT
                if(SOC_IS_METROLITE(unit))  {
                   first_child = node->first_child;
                }
#endif 
            } else {
                rv = soc_kt2_cosq_s0_sched_get(unit, node->hw_index, &mode);
                if (rv == BCM_E_NONE) {
                    num_spri = (mode == SOC_KT2_SCHED_MODE_STRICT) ? 2 : 0;
                    spmap = (mode == SOC_KT2_SCHED_MODE_STRICT) ? 3 : 0;
                }
            }
        }

        if (_bcm_is_port_linkphy_subport(unit, port, -1)) {
            SOC_IF_ERROR_RETURN(_bcm_subport_physical_port_get(unit, port, &physical_port));

            SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, LLS_PORT_CONFIGm, MEM_BLOCK_ALL, physical_port, &entry));
            if (soc_mem_field32_get(unit, LLS_PORT_CONFIGm, entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
                sched_mode = SOC_KT2_SCHED_MODE_WRR;
            } else {
                sched_mode = SOC_KT2_SCHED_MODE_WDRR;
            }
        }
    }
    switch (node->level) {
        case _BCM_KT2_COSQ_NODE_LEVEL_ROOT:
        case _BCM_KT2_COSQ_NODE_LEVEL_S1:
        case _BCM_KT2_COSQ_NODE_LEVEL_L0:
        case _BCM_KT2_COSQ_NODE_LEVEL_L1:
                 LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d FMC=%d "
                            "UCMAP=0x%08x SPMAP=0x%08x MODE=%s WT=%d\n"),
                 lvl_name[node->level], node->cosq_attached_to,
                 node->hw_index, num_spri,
                 first_child, first_mc_child, ucmap, spmap,
                 sched_modes[sched_mode], wt));
        break;
        case _BCM_KT2_COSQ_NODE_LEVEL_S0:
                BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_subport_get(unit, node->gport, &subport));
                LOG_CLI((BSL_META_U(unit,
                "-------%s subport %d (LLS)------\n"),SOC_PORT_NAME(unit,
                                                      (port)), subport));
                 LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d FMC=%d "
                            "UCMAP=0x%08x SPMAP=0x%08x MODE=%s WT=%d\n"),
                 lvl_name[node->level], node->cosq_attached_to,
                 node->hw_index, num_spri,
                 first_child, first_mc_child, ucmap, spmap,
                 sched_modes[sched_mode], wt));
        break;
        case _BCM_KT2_COSQ_NODE_LEVEL_L2:
                is_mc_queue = BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport) ||
                    BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(node->gport);
                LOG_CLI((BSL_META_U(unit,
                              "     L2.%d.%s INDEX=%d Mode=%s WEIGHT=%d\n"),
                                   node->cosq_attached_to,
                                   is_mc_queue?"mc":"uc",
                                   node->hw_index,
                                   sched_modes[sched_mode], wt));
        break;
        default:
             return BCM_E_INTERNAL;

    }
    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_dump_sched(int unit, bcm_port_t port, bcm_gport_t gport)
{
    _bcm_kt2_cosq_node_t *node;
    _bcm_kt2_cosq_node_t *cur_node;
    int i = 0;
    int start_cos = 0;
    int end_cos = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

    if (node != NULL) {
        if (node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
            return BCM_E_INTERNAL;
        }

        start_cos = 0;
        end_cos = node->numq;
        _bcm_kt2_dump_config(unit, port, node);

        for (i = start_cos; i < end_cos; i++)
        {
            if(_bcm_kt2_cosq_child_node_at_input(node, i, &cur_node)
                       == BCM_E_NOT_FOUND) {
                continue;
            }
            if (cur_node->level == _BCM_KT2_COSQ_NODE_LEVEL_L2) {
                 BCM_IF_ERROR_RETURN
                   (_bcm_kt2_dump_config(unit, port, cur_node));
            } else {
                BCM_IF_ERROR_RETURN
                   (_bcm_kt2_dump_sched(unit, port, cur_node->gport));
            }
        }
    }
    return BCM_E_NONE;

}

STATIC int
_bcm_kt2_cosq_cpu_fc_set(int unit) 
{
    bcm_pbmp_t pbmp_linkphy;
    uint32 rval;
    uint32 cos_bmp_value = 0;
    uint64 lls_l2;
    uint32 l2_mask=0;

    pbmp_linkphy = soc_property_get_pbmp(unit, spn_PBMP_LINKPHY, 0);
    BCM_IF_ERROR_RETURN(READ_LLS_FC_CONFIGr(unit, &rval));

    /* Allows CMIC Cosmask flow control to update the
     * XON-XOFF state for those queues 47:0 */
    soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, FC_CFG_DISABLE_L2_COSMASK_XOFFf, 0);

    if (!SOC_IS_METROLITE(unit) && BCM_PBMP_NOT_NULL(pbmp_linkphy)) {
        soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, FC_CFG_COSMASK_L0_MAPf, 2);
    } else {
        soc_reg_field_set(unit, LLS_FC_CONFIGr, &rval, FC_CFG_COSMASK_L0_MAPf, 1);
    }
    BCM_IF_ERROR_RETURN(WRITE_LLS_FC_CONFIGr(unit, rval));

    COMPILER_64_ZERO(lls_l2);
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, LLS_L2_XOFFm, MEM_BLOCK_ANY, 0, &lls_l2));
    l2_mask = COMPILER_64_HI(lls_l2);
    l2_mask &= 0xffff0000;
    COMPILER_64_SET(lls_l2, l2_mask, 0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, LLS_L2_XOFFm, MEM_BLOCK_ALL, 0, &lls_l2));

    cos_bmp_value = 0;
    cos_bmp_value |= (1 << 16);
    rval = 0;
    BCM_IF_ERROR_RETURN(READ_CMIC_CMC0_CH1_COS_CTRL_RX_1r(unit, &rval));
    soc_reg_field_set(unit, CMIC_CMC0_CH1_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
    WRITE_CMIC_CMC0_CH1_COS_CTRL_RX_1r(unit, rval);

    BCM_IF_ERROR_RETURN(READ_CMIC_CMC0_CH2_COS_CTRL_RX_1r(unit, &rval));
    soc_reg_field_set(unit, CMIC_CMC0_CH2_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
    WRITE_CMIC_CMC0_CH2_COS_CTRL_RX_1r(unit, rval);

    BCM_IF_ERROR_RETURN(READ_CMIC_CMC0_CH3_COS_CTRL_RX_1r(unit, &rval));
    soc_reg_field_set(unit, CMIC_CMC0_CH3_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
    WRITE_CMIC_CMC0_CH3_COS_CTRL_RX_1r(unit, rval);

    cos_bmp_value = 0;
    cos_bmp_value |= (1 << 17);
    rval = 0;
    BCM_IF_ERROR_RETURN(READ_CMIC_CMC1_CH1_COS_CTRL_RX_1r(unit, &rval));
    soc_reg_field_set(unit, CMIC_CMC1_CH1_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
    WRITE_CMIC_CMC1_CH1_COS_CTRL_RX_1r(unit, rval);
    
    cos_bmp_value = 0;
    cos_bmp_value |= (1 << 18);
    rval = 0;
    BCM_IF_ERROR_RETURN(READ_CMIC_CMC2_CH1_COS_CTRL_RX_1r(unit, &rval));
    soc_reg_field_set(unit, CMIC_CMC2_CH1_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
    WRITE_CMIC_CMC2_CH1_COS_CTRL_RX_1r(unit, rval);

    /* SABER2 has 2 channels mapped to one CMC2 */
    if (SOC_IS_SABER2(unit)) {
        cos_bmp_value = 0;
        cos_bmp_value |= (1 << 18);
        rval = 0;
        BCM_IF_ERROR_RETURN(READ_CMIC_CMC2_CH3_COS_CTRL_RX_1r(unit, &rval));
        soc_reg_field_set(unit, CMIC_CMC2_CH3_COS_CTRL_RX_1r, &rval, COS_BMPf, cos_bmp_value);
        WRITE_CMIC_CMC2_CH3_COS_CTRL_RX_1r(unit, rval);
    }
    
    return BCM_E_NONE; 
}

STATIC int
_bcm_kt2_dump_port_lls(int unit, bcm_port_t port)
{
    _bcm_kt2_cosq_node_t *port_info;
    _bcm_kt2_mmu_info_t *mmu_info;
    bcm_port_t port_out;

    if (_bcm_kt2_mmu_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    mmu_info = _bcm_kt2_mmu_info[unit];

    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_localport_resolve(unit, port, &port_out));

    /* root node */
    port_info = &mmu_info->sched_node[port_out];

    if (port_info->gport >= 0) {
       _bcm_kt2_dump_sched(unit, port, port_info->gport);
    }
    return BCM_E_NONE;
}


int bcm_kt2_dump_port_lls(int unit, bcm_port_t port)
{

    LOG_CLI((BSL_META_U(unit,
      "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));
    _bcm_kt2_dump_port_lls(unit, port);
    return SOC_E_NONE;
}


#ifndef BCM_SW_STATE_DUMP_DISABLE

STATIC int
_bcm_kt2_cosq_node_info_dump(int unit, _bcm_kt2_cosq_node_t *node) 
{
    LOG_CLI((BSL_META_U(unit,
           "base = %d mc_base_index = %d numq = %d hw_index = %d \n"),
           node->base_index, node->mc_base_index, node->numq, node->hw_index));
    LOG_CLI((BSL_META_U(unit,
                        "level = %d cosq_attach = %d \n"),
             node->level, node->cosq_attached_to));
    LOG_CLI((BSL_META_U(unit,
                        "num_child = %d first_child = %d wrr_in_use = %d \n"), 
             node->num_child, node->first_child,
             node->wrr_in_use));
    LOG_CLI((BSL_META_U(unit,
                        "subport_enabled = %d linkphy_enabled = %d \n"),
             node->subport_enabled,
             node->linkphy_enabled)); 
    if (node->parent != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  parent gport = 0x%08x\n"), node->parent->gport));
    }
    if (node->sibling != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  sibling gport = 0x%08x\n"), node->sibling->gport));
    }
    if (node->child != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "  child  gport = 0x%08x\n"), node->child->gport));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_l0_tree_dump(int unit, _bcm_kt2_cosq_node_t *node) 
{
    _bcm_kt2_cosq_node_t *l0_node, *l1_node, *l2_node;

    for (l0_node = node; l0_node != NULL;
         l0_node = l0_node->sibling) {
         LOG_CLI((BSL_META_U(unit,
                             "=== L0 gport 0x%08x\n"), l0_node->gport));
         BCM_IF_ERROR_RETURN
             (_bcm_kt2_cosq_node_info_dump(unit, l0_node));

        for (l1_node = l0_node->child; l1_node != NULL;
             l1_node = l1_node->sibling) {
              LOG_CLI((BSL_META_U(unit,
                                  "=== L1 gport 0x%08x\n"), l1_node->gport));
              BCM_IF_ERROR_RETURN
                  (_bcm_kt2_cosq_node_info_dump(unit, l1_node));
                
            for (l2_node = l1_node->child; l2_node != NULL;
                 l2_node = l2_node->sibling) {
                 LOG_CLI((BSL_META_U(unit,
                                     "=== L2 gport 0x%08x\n"), l2_node->gport));
                 BCM_IF_ERROR_RETURN
                      (_bcm_kt2_cosq_node_info_dump(unit, l2_node));
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_kt2_cosq_port_info_dump(int unit, bcm_port_t port) 
{
    _bcm_kt2_mmu_info_t *mmu_info;
    _bcm_kt2_cosq_node_t *port_node = NULL;
    _bcm_kt2_cosq_node_t *l0_node = NULL;
    _bcm_kt2_cosq_node_t *s0_node = NULL;
    _bcm_kt2_cosq_node_t *s1_node = NULL;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    mmu_info = _bcm_kt2_mmu_info[unit];

    /* get the root node */
    port_node = &mmu_info->sched_node[port];

    if (port_node && port_node->cosq_attached_to < 0) {
        return BCM_E_NONE;
    }    

    LOG_CLI((BSL_META_U(unit,
                        "=== port %d\n"), port));
    BCM_IF_ERROR_RETURN
        (_bcm_kt2_cosq_node_info_dump(unit, port_node));

    s0_node = port_node->child;
    if (s0_node != NULL && 
        s0_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S0) {
        for (s0_node = port_node->child; s0_node != NULL;
             s0_node = s0_node->sibling) {
             LOG_CLI((BSL_META_U(unit,
                                 "=== S0 gport 0x%08x\n"), s0_node->gport));
             BCM_IF_ERROR_RETURN
                 (_bcm_kt2_cosq_node_info_dump(unit, s0_node));

             s1_node = s0_node->child;
             if (s1_node != NULL && 
                 s1_node->level == _BCM_KT2_COSQ_NODE_LEVEL_S1) {
                 for (s1_node = s0_node->child; s1_node != NULL;
                      s1_node = s1_node->sibling) {
                      LOG_CLI((BSL_META_U(unit,
                                          "=== S1 gport 0x%08x\n"), s1_node->gport));
                      BCM_IF_ERROR_RETURN
                          (_bcm_kt2_cosq_node_info_dump(unit, s1_node));
                      for (l0_node = s1_node->child; l0_node != NULL;
                           l0_node = l0_node->sibling) {
                           BCM_IF_ERROR_RETURN
                                (_bcm_kt2_cosq_l0_tree_dump(unit, l0_node));
                        }
                    }
             } else {
                 l0_node = s1_node;
                 BCM_IF_ERROR_RETURN
                     (_bcm_kt2_cosq_l0_tree_dump(unit, l0_node));
            }
        }
    } else {
        l0_node = port_node->child;
        BCM_IF_ERROR_RETURN
            (_bcm_kt2_cosq_l0_tree_dump(unit, l0_node));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_kt2_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_kt2_cosq_sw_dump(int unit)
{
    _bcm_kt2_mmu_info_t *mmu_info;
    bcm_port_t port;
    int i;

    mmu_info = _bcm_kt2_mmu_info[unit];

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));

    for (port = 0; port <= KT2_LPBK_PORT; port++) {
        (void)_bcm_kt2_cosq_port_info_dump(unit, port);
    }

    LOG_CLI((BSL_META_U(unit,
                        "global_qlist = %d \n"), mmu_info->global_qlist.count));
    LOG_CLI((BSL_META_U(unit,
                        "sched_list = %d \n"), mmu_info->sched_list.count));
    LOG_CLI((BSL_META_U(unit,
                        "l2_base_qlist = %d \n"), mmu_info->l2_base_qlist.count));
    LOG_CLI((BSL_META_U(unit,
                        "l2_base_mcqlist = %d \n"), mmu_info->l2_base_mcqlist.count));
    LOG_CLI((BSL_META_U(unit,
                        "l2_global_qlist = %d \n"), mmu_info->l2_global_qlist.count));
   
    for (i = 0; i < _BCM_KT2_COSQ_NODE_LEVEL_MAX; i++) {
        LOG_CLI((BSL_META_U(unit,
                            "Level %d count = %d \n"), 
                 i, mmu_info->sched_hw_list[i].count));
    }
    
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_KATANA2_SUPPORT */
typedef int _kt2_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_KATANA2_SUPPORT */ 
