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

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk3.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/pfc.h>
#include <soc/uc.h>
#include <soc/shared/qcm.h>
#include <soc/scache_dictionary.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/ecn.h>
#include <bcm_int/esw/oob.h>
#include <bcm_int/esw/xgs5.h>
#if defined(INCLUDE_PSTATS)
#include <bcm_int/esw/pstats.h>
#endif /* INCLUDE_PSTATS */
#include <bcm_int/esw/pfc_deadlock.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/bst.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif /* BCM_CMICM_SUPPORT*/

/*Local defines*/
#define _BCM_TH3_NUM_QSCHED_PROFILES 8
#define _BCM_TH3_NUM_PG_PROP_PROFILES 8
#define _BCM_TH3_NUM_INTPRI_PG_MAP_PROFILES 8
#define _BCM_TH3_NUM_PFC_CLASS_PROFILES 8
#define _BCM_TH3_NUM_OOB_EGRQUEUE_MAP_PROFILES 4
#define _BCM_TH3_NUM_WRED_PROFILES 9
#define _BCM_TH3_QSCHED_MIN_WEIGHT 0
#define _BCM_TH3_QSCHED_MAX_WEIGHT 127

#define _TH3_ITMS_PER_DEV 2
#define _BCM_TH3_COS_DEFAULT 12
#define _BCM_TH3_COS_MAX  12
#define _BCM_TH3_MCQ_MODE_DEFAULT 2
#define _BCM_TH3_MMU_INFO(unit) _bcm_th3_mmu_info[(unit)]
#define _BCM_TH3_NUM_SCHEDULER_PER_PORT 12
#define _BCM_TH3_MAX_QUEUE_PER_PORT 12
#define _BCM_TH3_MAX_NUM_UCAST_QUEUE_PER_PORT 12
#define _BCM_TH3_MAX_NUM_MCAST_QUEUE_PER_PORT 6
#define _BCM_TH3_MAX_NUM_QUEUE_PER_PORT 12
#define _BCM_TH3_NUM_CPU_MCAST_QUEUE 48

/* Number of COSQs configured for this unit */
#define _TH3_NUM_COS(unit) NUM_COS(unit)

#define _TH3_PORT_NUM_COS(unit, port) \
    ((IS_CPU_PORT(unit, port)) ? NUM_CPU_COSQ(unit) : \
        (IS_LB_PORT(unit, port)) ? 12 : _TH3_NUM_COS(unit))


#define _BCM_TH3_NUM_TIME_DOMAIN    4

typedef struct _bcm_th3_cosq_time_info_s {
    uint32 ref_count;
} _bcm_th3_cosq_time_info_t;

/* WRED can be enabled at per UC queue / port / service pool basis
MMU_WRED_AVG_QSIZE used entry: (20*12 + 20*4 + 4) = 324
*/

STATIC _bcm_th3_cosq_time_info_t th3_time_domain_info[BCM_MAX_NUM_UNITS][_BCM_TH3_NUM_TIME_DOMAIN] = {
  {
    {324}, /* Default all entries are pointing to TIME_0*/
    {0},
    {0},
    {0}
  }
};

#define TH3_WRED_CELL_FIELD_MAX       0x7ffff
#define TH3_WRED_PROFILE_INDEX_INVALID    0xffffffff
#define TH3_WRED_DROP_PROB_MAX_INDEX    14

/*
 * index: degree, value: contangent(degree) * 100
 * max value is 0x7ffff (19-bit) at 0 degree
 */
STATIC int
_bcm_th3_cotangent_lookup_table[] =
{
    /*  0.. 5 */ 262143, 5728, 2863, 1908, 1430, 1143,
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
 * Same as TD2.
 */
STATIC int
_bcm_th3_angle_to_cells(int angle)
{
    return (_bcm_th3_cotangent_lookup_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 * Same as TD2.
 */
STATIC int
_bcm_th3_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_th3_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}



/*
 *  Convert HW drop probability to percent value
 *  Same as TH2.
 */
STATIC int
_bcm_th3_hw_drop_prob_to_percent[] = {
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
_bcm_th3_percent_to_drop_prob(int percent)
{
   int i;

   for (i = TH3_WRED_DROP_PROB_MAX_INDEX; i > 0 ; i--) {
      if (percent >= _bcm_th3_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_th3_drop_prob_to_percent(int drop_prob) {
   return (_bcm_th3_hw_drop_prob_to_percent[drop_prob]);
}

/* WRED resolution table */
#define _BCM_TH3_NUM_WRED_RESOLUTION_TABLE   4
STATIC uint32 _bcm_th3_wred_resolution_tbl[_BCM_TH3_NUM_WRED_RESOLUTION_TABLE] = {
    1,
    0,
    0,
    0,
};

typedef struct _bcm_th3_cosq_node_s {
    bcm_gport_t gport;
    int numq;
    int level;
    int hw_index;
    int in_use;
    bcm_gport_t parent_gport;
    int cos;
    int strict_priority; /* port->L0 */
    /* int strict_priority_p;*/ /* L0->L1 */
    int fc_is_uc_only;
} _bcm_th3_cosq_node_t;

typedef struct _bcm_th3_cosq_cpu_cosq_config_s {
    int q_min_limit;
    int q_shared_limit;
    uint8 q_limit_dynamic;
    uint8 q_limit_enable;
    uint8 q_color_limit_enable;
    uint8 enable;
} _bcm_th3_cosq_cpu_cosq_config_t;

typedef struct _bcm_th3_cosq_port_info_s {
    _bcm_th3_cosq_node_t ucast[_BCM_TH3_MAX_NUM_UCAST_QUEUE_PER_PORT];
    _bcm_th3_cosq_node_t mcast[_BCM_TH3_MAX_NUM_MCAST_QUEUE_PER_PORT];
    _bcm_th3_cosq_node_t sched[_BCM_TH3_NUM_SCHEDULER_PER_PORT];
} _bcm_th3_cosq_port_info_t;

typedef struct _bcm_th3_cosq_cpu_port_info_s {
    _bcm_th3_cosq_node_t sched[_BCM_TH3_NUM_SCHEDULER_PER_PORT];
    _bcm_th3_cosq_node_t mcast[_BCM_TH3_NUM_CPU_MCAST_QUEUE];
} _bcm_th3_cosq_cpu_port_info_t;

typedef struct _bcm_th3_mmu_info_s {
    int gport_tree_created; /* FALSE: No GPORT tree exists */
    _bcm_th3_cosq_port_info_t *_bcm_th3_port_info;
    _bcm_th3_cosq_cpu_port_info_t *_bcm_th3_cpu_port_info;
    int shared_limit[_TH3_ITMS_PER_DEV];
} _bcm_th3_mmu_info_t;


STATIC _bcm_th3_mmu_info_t *_bcm_th3_mmu_info[BCM_MAX_NUM_UNITS]; /* MMU info */
STATIC _bcm_th3_cosq_port_info_t *_bcm_th3_cosq_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_th3_cosq_cpu_port_info_t *_bcm_th3_cosq_cpu_port_info[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_th3_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_th3_wred_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_th3_prio2cos_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_th3_ifp_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC _bcm_th3_cosq_cpu_cosq_config_t
            *_bcm_th3_cosq_cpu_cosq_config[BCM_MAX_NUM_UNITS][SOC_TH3_NUM_CPU_QUEUES];

/* Warmboot */
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)

#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1
/* v 1.1  - PFC deadlock state based recovery mode added */

STATIC int
_bcm_th3_cosq_wb_alloc(int unit)
{
    _bcm_th3_mmu_info_t *mmu_info = NULL;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    int rv, alloc_size = 0, max_size = 4096 * 11;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;

    mmu_info = _bcm_th3_mmu_info[unit];
    if (mmu_info == NULL) {
        return BCM_E_INIT;
    }

    cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }

    /*
     * Sync _bcm_th3_mmu_info[unit]->shared_limit
     */
    alloc_size += 1 * sizeof(int);

    /*
     * Sync _bcm_th3_cosq_cpu_port_info structure
     */

    alloc_size += sizeof(_bcm_th3_cosq_cpu_port_info_t);

    /*
     * Sync the ref_count of IFP_COS_MAPm/COS_MAPm
     */
    alloc_size += ((SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _TH3_MMU_NUM_INT_PRI) - 1) *
                   sizeof(uint16);

    /*
     * Sync _bcm_th3_cosq_cpu_cosq_config_t structure for all CPU queues
     */
    alloc_size += (SOC_TH3_NUM_CPU_QUEUES *
                   sizeof(_bcm_th3_cosq_cpu_cosq_config_t));
    /*
     * Sync num_cos value
     */
    alloc_size += sizeof(int);

    /*
     * Sync th3_sched_profile_info struture for all scheduler profiles
     */
    alloc_size += (SOC_TH3_MAX_NUM_SCHED_PROFILE *
                   SOC_TH3_NUM_GP_QUEUES *
                   5 * sizeof(int));

    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        alloc_size += (SOC_MAX_NUM_PORTS * 8 * sizeof(uint16));
        alloc_size += (SOC_MAX_NUM_PORTS * 8 * sizeof(uint32));

        if (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_1) {
           /* for deadlock state based recovery mode */
            alloc_size += (SOC_MAX_NUM_PORTS * 8 * sizeof(uint16));
        }
    }

   /*
     * Added TIME_DOMAIN ref count
     */
    alloc_size += sizeof(_bcm_th3_cosq_time_info_t);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_ecn_wred)) {
        uint32 scache_size = 0;
        BCM_IF_ERROR_RETURN(
            bcmi_xgs5_ecn_scache_size_get(unit, &scache_size));
        alloc_size += scache_size;
    }
#endif

    

    /*
     * Sync _bcm_th3_cosq_cpu_cosq_config_t structure for all CPU queues
     */
    alloc_size += (_BCM_TH3_NUM_WRED_RESOLUTION_TABLE *
                   sizeof(uint32));

    /* Reserving some space scache for future usecase (max_size) */
    if (alloc_size < max_size) {
        alloc_size = max_size;
    } else {
        /* Required size more than max size allocated for Cosq */
        return BCM_E_INIT;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);

    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/* Function to Set Default Scheduler Configuration for all the ports */
STATIC int
_bcm_th3_cosq_sched_default_config(int unit)
{
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    bcm_port_t port;
    int default_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN, default_weight = 1;
    int i;

    port = 0;
    cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][port];
    for (i = 0; i < _BCM_TH3_NUM_SCHEDULER_PER_PORT; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_sched_set(unit, cpu_port_info->sched[i].gport,
                -1, default_mode, default_weight));
    }
    for (i = 0; i < _BCM_TH3_NUM_CPU_MCAST_QUEUE; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_sched_set(unit, cpu_port_info->mcast[i].gport,
                -1, default_mode, default_weight));
    }

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
            continue;
        } else {
            port_info = &_bcm_th3_cosq_port_info[unit][port];
        }
        for (i = 0; i < _BCM_TH3_NUM_SCHEDULER_PER_PORT; i++) {
            if (!port_info->sched[i].in_use || (port_info->sched[i].numq == 0)) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_sched_set(unit, port_info->sched[i].gport,
                    -1, default_mode, default_weight));
        }
        for (i = 0; i < _bcm_th3_get_num_ucq(unit); i++) {
            if (!port_info->ucast[i].in_use)
                continue;
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_sched_set(unit, port_info->ucast[i].gport,
                    -1, default_mode, default_weight));
        }
        for (i = 0; i < _bcm_th3_get_num_mcq(unit); i++) {
            if (!port_info->mcast[i].in_use)
                continue;
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_sched_set(unit, port_info->mcast[i].gport,
                    -1, default_mode, default_weight));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_gport_default_config(int unit)
{
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_sched_default_config(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_node_get
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
_bcm_th3_cosq_node_get(int unit, bcm_gport_t gport, bcm_module_t *modid,
                      bcm_port_t *port, int *id, _bcm_th3_cosq_node_t **node)
{
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_th3_cosq_node_t *node_base = NULL;
    bcm_module_t modid_out;
    bcm_port_t port_out;
    uint32 encap_id;
    int index;

    if (_bcm_th3_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
    } else {
        return BCM_E_PORT;
    }

    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, port_out)) {
        cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][port_out];
    } else {
        port_info = &_bcm_th3_cosq_port_info[unit][port_out];
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (index >= _bcm_th3_get_num_ucq(unit)) {
            return BCM_E_PORT;
        }
        node_base = port_info->ucast;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (!IS_CPU_PORT(unit, port_out) &&
            (index >= _bcm_th3_get_num_mcq(unit))) {
            return BCM_E_PORT;
        }
        if (IS_CPU_PORT(unit, port_out) && (index >= _BCM_TH3_NUM_CPU_MCAST_QUEUE)) {
            return BCM_E_PORT;
        }
        if (IS_CPU_PORT(unit, port_out)) {
            node_base = cpu_port_info->mcast;
        } else {
            node_base = port_info->mcast;
        }
    } else { /* scheduler */
        encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 16) & 0x3ff;
        index = encap_id;
        if (index >= _BCM_TH3_NUM_SCHEDULER_PER_PORT) {
            return BCM_E_PORT;
        }
        if (IS_CPU_PORT(unit, port_out)) {
            node_base = cpu_port_info->sched;
        } else {
            node_base = port_info->sched;
        }
    }

    if ((node_base == NULL) ||
        (node_base[index].numq == 0) ||
        (node_base[index].in_use == 0)) {
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

/*
 * Function:
 *     _bcm_th3_rqe_q_init
 * Purpose:
 *     Initialize RQE registers
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_rqe_q_init(int unit)
{
    uint32 rval;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_CPU_LO_RQE_Q_NUMr(unit, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, CPU_LO_RQE_Q_NUMr, &rval, RQE_Q_NUMf, _TH3_A0_CPU_LO_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, CPU_LO_RQE_Q_NUMr, &rval, RQE_Q_NUMf, _TH3_CPU_LO_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN(WRITE_CPU_LO_RQE_Q_NUMr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_CPU_HI_RQE_Q_NUMr(unit, &rval));
    if (rev_id == BCM56980_A0_REV_ID) {
        soc_reg_field_set(unit, CPU_HI_RQE_Q_NUMr, &rval, RQE_Q_NUMf, _TH3_A0_CPU_HI_RQE_Q_NUM);
    } else {
        soc_reg_field_set(unit, CPU_HI_RQE_Q_NUMr, &rval, RQE_Q_NUMf, _TH3_CPU_HI_RQE_Q_NUM);
    }
    SOC_IF_ERROR_RETURN(WRITE_CPU_HI_RQE_Q_NUMr(unit, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_ecn_init
 * Purpose:
 *     Initialize (clear) all ECN memories/registers
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_ecn_init(int unit)
{
    egr_ip_to_int_cn_mapping_entry_t entry_egr_ip_int_cn;
    int count, i;
    uint32  fld_val;

    BCM_IF_ERROR_RETURN(_bcm_esw_ecn_init(unit));

    /* Perform additional Tomahawk3 specific ECN initialization */
    /* EGR_IP_TO_INT_CN_MAPPING is identical to IP_TO_INT_CN_MAPPING table
     */
    if (!SOC_WARM_BOOT(unit)) {
        count = soc_mem_index_count(unit, EGR_IP_TO_INT_CN_MAPPINGm);
        for (i = 0; i < count; i++) {
            sal_memset(&entry_egr_ip_int_cn, 0, sizeof(entry_egr_ip_int_cn));
            switch (i) {
                case 0:
                    fld_val = _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP;
                    break;
                case 1:
                    fld_val = _BCM_ECN_INT_CN_RESPONSIVE_DROP;
                    break;
                case 6:
                case 7:
                    fld_val = _BCM_ECN_INT_CN_CONGESTION_MARK_ECN;
                    break;
                default:
                    fld_val = _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN;
                    break;
            }
            soc_mem_field32_set(unit, EGR_IP_TO_INT_CN_MAPPINGm,
                &entry_egr_ip_int_cn, INT_CNf, fld_val);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_IP_TO_INT_CN_MAPPINGm, MEM_BLOCK_ALL,
                               i, (void *)&entry_egr_ip_int_cn));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_cleanup
 * Purpose:
 *     Clean resource in case of error during Cosq Init
 * Parameters:
 *     unit - unit number
 * Returns:
 *     No return value
 */
STATIC void
_bcm_th3_cosq_cleanup(int unit)
{
    if (_bcm_th3_cosq_port_info[unit] != NULL) {
        sal_free(_bcm_th3_cosq_port_info[unit]);
        _bcm_th3_cosq_port_info[unit] = NULL;
    }

    if (_bcm_th3_cosq_cpu_port_info[unit] != NULL) {
        sal_free(_bcm_th3_cosq_cpu_port_info[unit]);
        _bcm_th3_cosq_cpu_port_info[unit] = NULL;
    }

    if (_bcm_th3_mmu_info[unit] != NULL) {
        sal_free(_bcm_th3_mmu_info[unit]);
        _bcm_th3_mmu_info[unit] = NULL;
    }

    if (_bcm_th3_cos_map_profile[unit] != NULL) {
        soc_profile_mem_destroy(unit, _bcm_th3_cos_map_profile[unit]);
        sal_free(_bcm_th3_cos_map_profile[unit]);
        _bcm_th3_cos_map_profile[unit] = NULL;
    }

    if (_bcm_th3_ifp_cos_map_profile[unit] != NULL) {
        soc_profile_mem_destroy(unit, _bcm_th3_ifp_cos_map_profile[unit]);
        sal_free(_bcm_th3_ifp_cos_map_profile[unit]);
        _bcm_th3_ifp_cos_map_profile[unit] = NULL;
    }

    if (_bcm_th3_wred_profile[unit] != NULL) {
        soc_profile_mem_destroy(unit, _bcm_th3_wred_profile[unit]);
        sal_free(_bcm_th3_wred_profile[unit]);
        _bcm_th3_wred_profile[unit] = NULL;
    }

    if (_bcm_th3_prio2cos_profile[unit] != NULL) {
        soc_profile_reg_destroy(unit, _bcm_th3_prio2cos_profile[unit]);
        sal_free(_bcm_th3_prio2cos_profile[unit]);
        _bcm_th3_prio2cos_profile[unit] = NULL;
    }

#if defined(INCLUDE_PSTATS)
    if (_BCM_PSTATS_UNIT_DRIVER(unit) != NULL) {
        _bcm_pstats_deinit(unit);
    }
#endif /* INCLUDE_PSTATS */

    (void)_bcm_bst_detach(unit);

#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(INCLUDE_L3)
    bcmi_ecn_map_clean_up(unit);
#endif
    (void)_bcm_th3_pfc_deadlock_deinit(unit);

    return;
}

/*
 * Function:
 *     bcm_th3_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_detach(int unit, int software_state_only)
{
    _bcm_th3_cosq_cleanup(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_sched_get
 * Purpose:
 *     Get scheduling mode setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (OUT) scheduling mode (BCM_COSQ_XXX)
 *     weight              - (OUT) weight
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_cosq_sched_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                       int *mode, int *weight)
{
    soc_th3_sched_mode_e sched_mode;
    bcm_port_t local_port;
    int lvl = SOC_TH3_NODE_LVL_L0, mc = 0;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }


    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        lvl = SOC_TH3_NODE_LVL_L0;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
               BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        lvl = SOC_TH3_NODE_LVL_L1;
        mc = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ? 1 : 0;
    } else if (IS_CPU_PORT(unit, local_port)) {
        lvl = SOC_TH3_NODE_LVL_L1;
        mc = 1;
    }
    BCM_IF_ERROR_RETURN(
            soc_th3_cosq_sched_mode_get(unit, local_port, lvl, cosq,
                            &sched_mode, weight, mc));
    switch(sched_mode) {
        case SOC_TH3_SCHED_MODE_STRICT:
            *mode = BCM_COSQ_STRICT;
        break;
        case SOC_TH3_SCHED_MODE_WRR:
            *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
        case SOC_TH3_SCHED_MODE_WERR:
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
 *     _bcm_th3_cosq_sched_set
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (IN) scheduling mode (BCM_COSQ_XXX)
 *     weight              - (IN) weight
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_cosq_sched_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        int mode, int weight)
{
    soc_th3_sched_mode_e sched_mode;
    bcm_port_t local_port;
    int lwts = 1, child_index=0, lvl = SOC_TH3_NODE_LVL_L0, mc = 0;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }
    if ((weight < _BCM_TH3_QSCHED_MIN_WEIGHT) || (weight > _BCM_TH3_QSCHED_MAX_WEIGHT)) {
        return BCM_E_PARAM;
    }

    switch(mode) {
        case BCM_COSQ_STRICT:
            sched_mode = SOC_TH3_SCHED_MODE_STRICT;
            lwts = 0;
            break;
        case BCM_COSQ_ROUND_ROBIN:
            sched_mode = SOC_TH3_SCHED_MODE_WRR;
            lwts = 1;
            break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            sched_mode = SOC_TH3_SCHED_MODE_WRR;
            lwts = weight;
            break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            sched_mode = SOC_TH3_SCHED_MODE_WERR;
            lwts = weight;
            break;
        default:
            return BCM_E_PARAM;
    }

    /* Weight = 0 corresponds to Strict Mode */
    if (lwts == 0) {
        sched_mode = SOC_TH3_SCHED_MODE_STRICT;
    }

    child_index = cosq;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        /* We need to set lvl = L0(schedular node) so that sched mode is set at it's parent */
        lvl = SOC_TH3_NODE_LVL_L0;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
               BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        /* We need to set lvl = L1(UC/MC nodes) so that sched mode is set at it's parent */
        lvl = SOC_TH3_NODE_LVL_L1;
        mc = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ? 1 : 0;
    } else if (IS_CPU_PORT(unit, local_port)) {
        lvl = SOC_TH3_NODE_LVL_L1;
        mc = 1;
    }
    BCM_IF_ERROR_RETURN(soc_th3_cosq_sched_mode_set(unit, local_port,
                                                   lvl,
                                                   child_index,
                                                   sched_mode, lwts, mc));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahaw3_cosq_port_sched_config_get
 * Purpose:
 *     Retrieve class-of-service policy and corresponding weights and mode
 * Parameters:
 *     unit                 - (IN) unit number
 *     pbm                  - (IN) port bitmap
 *     weight_arr_size      - (IN) Size of weights array
 *     mode                 - (OUT) Scheduling mode (BCM_COSQ_XXX)
 *     weights              - (OUT) Weights for each COS queue
 *     weight_arr_count     - (OUT) Actual size of weights array
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_port_sched_config_get(int unit, bcm_pbmp_t pbm, int weight_arr_size,
                           int *weights, int *weight_arr_count, int *mode)
{
    bcm_port_t port;
    int i, num_weights;

    BCM_PBMP_ITER(pbm, port) {
        if (IS_CPU_PORT(unit, port) && SOC_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
            continue;
        }
        if (weights == NULL) {
            return BCM_E_PARAM;
        }
        if (weight_arr_count == NULL) {
            return BCM_E_PARAM;
        }
        if (weight_arr_size > _TH3_NUM_COS(unit)) {
            num_weights = _TH3_NUM_COS(unit);
        } else {
            num_weights = weight_arr_size;
        }
        *weight_arr_count = num_weights;

        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_sched_get(unit, port, i, mode, &weights[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_port_sched_config_set
 * Purpose:
 *     Set up class-of-service policy and corresponding weights and mode
 * Parameters:
 *     unit               - (IN) unit number
 *     pbm                - (IN) port bitmap
 *     mode               - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight_arr_size    - (IN) Size of the weights array
 *     weights            - (IN) Weights for each COS queue
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_port_sched_config_set(int unit, bcm_pbmp_t pbm,
                           int mode, int weight_arr_size, int *weights)
{
    bcm_port_t port;
    int num_weights, i;

    BCM_PBMP_ITER(pbm, port) {
        if (weight_arr_size > _TH3_NUM_COS(unit)) {
            return BCM_E_PARAM;
        } else {
            num_weights = weight_arr_size;
        }

        if (weights == NULL) {
            return BCM_E_PARAM;
        }
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_sched_set(unit, port, i, mode, weights[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_cosq_sched_config_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and mode
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weight_arr_size - Size of the weights array
 *    weights - Weights for each COS queue
 *        Unused if mode is BCM_COSQ_STRICT.
 *        Indicates number of packets sent before going on to
 *        the next COS queue.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_sched_config_set(int unit, int mode, int weight_arr_size, int *weights)
{
    bcm_pbmp_t pbmp;

    if (soc_feature(unit, soc_feature_ets)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_CMIC(unit));
    } else {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    }

    return (bcm_tomahawk3_cosq_port_sched_config_set(unit, pbmp, mode, weight_arr_size,
                                                        weights));
}

/*
 * Function:
 *      bcm_tomahawk3_cosq_sched_config_get
 * Purpose:
 *      Retrieve class-of-service policy and corresponding weights and mode
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      mode_ptr - (output) Scheduling mode, one of BCM_COSQ_xxx
 *    weights - (output) Weights for each COS queue
 *        Unused if mode is BCM_COSQ_STRICT.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_sched_config_get(int unit, int weight_arr_size,
                                    int *weights, int *weight_arr_count,
                                    int *mode)
{
    bcm_pbmp_t pbmp;

    if (soc_feature(unit, soc_feature_ets)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_CMIC(unit));
    } else {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    }

    return (bcm_tomahawk3_cosq_port_sched_config_get(unit, pbmp, weight_arr_size,
                                                 weights, weight_arr_count, mode));
}


/*
 * Function:
 *     bcm_th3_cosq_port_sched_get
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
bcm_th3_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_th3_cosq_port_sched_set
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
bcm_th3_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    return BCM_E_UNAVAIL;
}



int
_bcm_th3_cosq_rx_queue_channel_set(int unit,
                                  bcm_cos_queue_t parent_cos,
                                  bcm_cos_queue_t queue_id,
                                  int enable)
{
#if defined(BCM_CMICX_SUPPORT)

    if(soc_feature(unit, soc_feature_cmicx)) {
        int pci_cmc = SOC_PCI_CMC(unit);
        int chan_id = -1;
        int start_chan_id = 0;
        uint32 chan_off;
        uint32 reg_addr, reg_val;
        uint32 ix;
        uint32 max_rx_channels;

        if ((parent_cos >= 0) && (parent_cos <= 6)) {
            chan_id = parent_cos + 1;
        } else {
            return BCM_E_PARAM;
        }

        if (!SHR_BITGET(CPU_ARM_QUEUE_BITMAP(unit, pci_cmc), queue_id)) {
            return BCM_E_PARAM;
        }

        soc_dma_max_rx_channels_get(unit, &max_rx_channels);

        for (ix = start_chan_id; ix < (start_chan_id + max_rx_channels); ix++) {
            chan_off = ix % max_rx_channels;
            soc_dma_cos_ctrl_reg_addr_get(unit, pci_cmc, chan_off,
                                          queue_id, &reg_addr);
            reg_val = soc_pci_read(unit, reg_addr);

            if (enable == 1) {
                if (ix == (uint32)chan_id) {
                    reg_val |= ((1U) << (queue_id % 32));
                } else {
                    /* Clear all other channels */
                    reg_val &= ~((1U) << (queue_id % 32));
                }
            } else {
                if (ix == (uint32)chan_id) {
                    reg_val &= ~((1U) << (queue_id % 32));
                }
            }
            /* Incoporate the reserved queues (if any on this device)
             * into the CMIC programming,  */
            reg_val |=
                CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc)[queue_id / 32];
            soc_pci_write(unit, reg_addr, reg_val);
        }
    }
#endif
    return BCM_E_NONE;
}

/* Function to attach any of the 48 L1 MC Queue to one of the 10 L0 nodes
 */
STATIC int
bcm_th3_cosq_cpu_gport_attach(int unit, bcm_gport_t input_gport,
                             bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    bcm_port_t parent_port, input_port;
    _bcm_th3_cosq_node_t *input_node = NULL, *parent_node = NULL;
    int phy_port, mmu_port;
    int input_cos = 0, parent_cos = 0;
    int cpu_sched_base = 0, cpu_mc_base = 0;
    soc_info_t *si = &SOC_INFO(unit);

    /* Post Default tree created, expected Input Gport is of MC Gport
     * and expected Parent Gport is new L0 Parent
     */
    if (_BCM_TH3_MMU_INFO(unit)->gport_tree_created == FALSE) {
        if (((BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport)) &&
             (!BCM_GPORT_IS_SCHEDULER(parent_gport))) ||
            (!(BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport)) &&
             (!BCM_GPORT_IS_SCHEDULER(input_gport)))) {
            return BCM_E_PARAM;
        }
    } else if ((!BCM_GPORT_IS_SCHEDULER(parent_gport)) ||
        (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                               &input_node));
    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, parent_gport, NULL, &parent_port, NULL,
                                   &parent_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, parent_gport, &parent_port));
        parent_node = NULL;
    }

    if (!IS_CPU_PORT(unit, parent_port)) {
        return BCM_E_PARAM;
    }

    /* Return BCM_E_EXISTS when queue to be attached is
       already attached to the given gport */
    if ((_BCM_TH3_MMU_INFO(unit)->gport_tree_created == TRUE) &&
        (input_node->parent_gport == parent_gport)) {
        return BCM_E_EXISTS;
    }

    phy_port = si->port_l2p_mapping[input_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    cpu_sched_base = mmu_port * _BCM_TH3_NUM_SCHEDULER_PER_PORT;
    input_cos = cosq % _BCM_TH3_NUM_CPU_MCAST_QUEUE;
    input_node->parent_gport = parent_port;

    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
        parent_cos = (parent_node->hw_index - cpu_sched_base) %
                     _BCM_TH3_NUM_CPU_MCAST_QUEUE;
        SOC_IF_ERROR_RETURN
            (soc_th3_cosq_cpu_parent_set(unit, input_cos, SOC_TH3_NODE_LVL_L1,
                                        parent_cos));
       /*
        * L0.0 is associated with CMC0 CH1(Rx)
        * L0.1 is associated with CMC0 CH2(Rx)
        * L0.2 is associated with CMC0 CH3(Rx)
        * L0.3 is associated with CMC0 CH4(Rx)
        * L0.4 is associated with CMC0 CH5(Rx)
        * L0.5 is associated with CMC0 CH6(Rx)
        * L0.6 is associated with CMC0 CH7(Rx)
        */
        if ((parent_cos >= 0) && (parent_cos < 7)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_rx_queue_channel_set(unit, parent_cos, input_cos, 1));
        }
        input_node->parent_gport = parent_node->gport;
        if (_BCM_TH3_MMU_INFO(unit)->gport_tree_created == FALSE) {
            input_node->hw_index = cpu_mc_base + input_cos;
        }
    } else {
        input_node->hw_index = cpu_sched_base + input_cos;
    }

    return BCM_E_NONE;
}


int _bcm_th3_queue_is_unicast(int unit, int queue_num)
{
    int mc_q_mode, num_ucq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, _BCM_TH3_MCQ_MODE_DEFAULT);
    num_ucq = (mc_q_mode == 0) ? 12 : (mc_q_mode == 1) ? 10 :
              (mc_q_mode == 2) ? 8  : (mc_q_mode == 3) ? 6 : 8;
    if (queue_num < num_ucq) {
        return 1;
    } else {
        return 0;
    }
}

int _bcm_th3_get_num_ucq(int unit)
{
    int mc_q_mode, num_ucq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, _BCM_TH3_MCQ_MODE_DEFAULT);
    num_ucq = (mc_q_mode == 0) ? 12 : (mc_q_mode == 1) ? 10 :
              (mc_q_mode == 2) ? 8  : (mc_q_mode == 3) ? 6 : 8;

    return num_ucq;
}

int _bcm_th3_get_num_mcq(int unit)
{
    int mc_q_mode, num_mcq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE, _BCM_TH3_MCQ_MODE_DEFAULT);
    num_mcq = (mc_q_mode == 0) ? 0 : (mc_q_mode == 1) ? 2 :
              (mc_q_mode == 2) ? 4 : (mc_q_mode == 3) ? 6 : 4;

    return num_mcq;
}

int
_bcm_th3_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
                          int *qnum)
{
    _bcm_th3_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, gport, modid, port, id, &node));
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
 *     _bcm_th3_cosq_gport_add_attach
 * Purpose:
 *     Create a cosq gport structure for non CPU Ports
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
_bcm_th3_cosq_gport_add_attach(int unit, bcm_gport_t port, int numq, uint32 flags)
{
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    uint32 sched_encap;
    bcm_port_t local_port;
    bcm_gport_t gport;
    bcm_gport_t parent_gport;
    int id, queue_num;
    int modid;

    if (_bcm_th3_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0 || IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    port_info = &_bcm_th3_cosq_port_info[unit][local_port];

    if (flags == BCM_COSQ_GPORT_SCHEDULER) {
        port_info->sched[numq].level = SOC_TH3_NODE_LVL_L0;
        /* coverity[overrun-local] */
        port_info->sched[numq].in_use = th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].in_use;
        port_info->sched[numq].hw_index = th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].hw_index;
        port_info->sched[numq].cos = th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].cos;
        port_info->sched[numq].strict_priority = th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].sched_policy;
        port_info->sched[numq].fc_is_uc_only = th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].fc_is_uc_only;
        sched_encap = (numq << 16) | local_port;
        BCM_GPORT_SCHEDULER_SET(gport, sched_encap);
        port_info->sched[numq].gport = gport;
        /* coverity[overrun-local] */
        if (th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].in_use == 0)
            port_info->sched[numq].numq = 0; /* Number of sub-nodes */
        else if ((th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].children[0] != NULL) &&
            (th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].children[1] != NULL))
            port_info->sched[numq].numq = 2; /* Number of sub-nodes */
        else if (th3_cosq_mmu_info[unit]->th3_port_info[local_port].L0[numq].children[0] != NULL)
            port_info->sched[numq].numq = 1; /* Number of sub-nodes */
        else
            port_info->sched[numq].numq = 0; /* Number of sub-nodes */
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        BCM_GPORT_MODPORT_SET(parent_gport, modid, port);
        port_info->sched[numq].parent_gport = parent_gport;
        LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
             "SCHED add_attach port %d numq %d gport %d sched.numq %d\n"),
             port, numq, port_info->sched[numq].gport, port_info->sched[numq].numq));
    } else {
        /* coverity[overrun-local] */
        id = th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].hw_index;
        if (_bcm_th3_queue_is_unicast(unit, id)) {
            port_info->ucast[id].level = SOC_TH3_NODE_LVL_L2;
            /* coverity[overrun-local] */
            port_info->ucast[id].in_use =
                th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].in_use;
            if (port_info->ucast[id].in_use) {
                /* coverity[overrun-local] */
                port_info->ucast[id].hw_index =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].hw_index;
                /* coverity[overrun-local] */
                port_info->ucast[id].cos =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].parent->parent->cos;
                BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(gport, local_port, id);
                port_info->ucast[id].gport = gport;
                port_info->ucast[id].numq = 1;
                /* coverity[overrun-local] */
                queue_num =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].parent->parent->hw_index;
                sched_encap = (queue_num << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(parent_gport, sched_encap);
                port_info->ucast[id].parent_gport = parent_gport;
                LOG_INFO(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit,
                    "UCAST add_attach port %d id %d numq %d gport %d hw_index %d\n"),
                    port, id, numq, port_info->ucast[id].gport, port_info->ucast[id].hw_index));
            } else {
                port_info->ucast[id].numq = 0;
            }
        } else {
            id = id - _bcm_th3_get_num_ucq(unit);
            port_info->mcast[id].level = SOC_TH3_NODE_LVL_L2;
            /* coverity[overrun-local] */
            port_info->mcast[id].in_use =
                th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].in_use;
            if (port_info->mcast[id].in_use) {
                /* coverity[overrun-local] */
                port_info->mcast[id].hw_index =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].hw_index;
                /* coverity[overrun-local] */
                port_info->mcast[id].cos =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].parent->parent->cos;
                BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(gport, local_port, id);
                port_info->mcast[id].gport = gport;
                port_info->mcast[id].numq = 1;
                /* coverity[overrun-local] */
                queue_num =
                    th3_cosq_mmu_info[unit]->th3_port_info[local_port].mmuq[numq].parent->parent->hw_index;
                sched_encap = (queue_num << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(parent_gport, sched_encap);
                port_info->mcast[id].parent_gport = parent_gport;
                LOG_INFO(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit,
                    "MCAST add_attach port %d id %d numq %d gport %d hw_index %d\n"),
                    port, id, numq, gport, port_info->mcast[id].hw_index));
            } else {
                port_info->mcast[id].numq = 0;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_cosq_cpu_gport_add_attach
 * Purpose:
 *     Create a cosq gport structure for CPU Ports
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
_bcm_th3_cosq_cpu_gport_add_attach(int unit, bcm_gport_t port, int numq, uint32 flags)
{
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    uint32 sched_encap;
    bcm_port_t local_port;
    bcm_gport_t gport, parent_gport;
    int queue_num;
    soc_info_t *si = &SOC_INFO(unit);

    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    if (!IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][local_port];

    if (flags == BCM_COSQ_GPORT_SCHEDULER) {
        cpu_port_info->sched[numq].level = SOC_TH3_NODE_LVL_L0;
        cpu_port_info->sched[numq].in_use = th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[numq].in_use;
        cpu_port_info->sched[numq].hw_index = th3_cosq_mmu_info[unit]->th3_cpu_port_info.L0[numq].hw_index +
                                               si->port_cosq_base[CMIC_PORT(unit)];
        sched_encap = (numq << 16) | local_port;
        BCM_GPORT_SCHEDULER_SET(gport, sched_encap);
        cpu_port_info->sched[numq].gport = gport;
        cpu_port_info->sched[numq].numq = 4; /* default 4 cos per mmuq*/
        cpu_port_info->sched[numq].parent_gport = port;
        LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "cpu_port_info[%d] sched gport=%x parent_gport=%x\n"),
              numq, cpu_port_info->sched[numq].gport, cpu_port_info->sched[numq].parent_gport));
    } else {
        cpu_port_info->mcast[numq].level = SOC_TH3_NODE_LVL_L1;
        cpu_port_info->mcast[numq].in_use = th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[numq].in_use;
        cpu_port_info->mcast[numq].hw_index = th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[numq].hw_index +
                                               si->port_cosq_base[CMIC_PORT(unit)];
        BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(gport, local_port, numq);
        cpu_port_info->mcast[numq].gport = gport;
        cpu_port_info->mcast[numq].numq = 1;
        queue_num = th3_cosq_mmu_info[unit]->th3_cpu_port_info.L1[numq].parent->hw_index;
        sched_encap = (queue_num << 16) | local_port;
        BCM_GPORT_SCHEDULER_SET(parent_gport, sched_encap);
        cpu_port_info->mcast[numq].parent_gport = parent_gport;
        LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "cpu_port_info[%d] mcast gport=%x parent_gport=%x hw_index=%d\n"),
              numq, cpu_port_info->mcast[numq].gport, cpu_port_info->mcast[numq].parent_gport, queue_num));
    }


    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_th3_cosq_gport_add
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
bcm_th3_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_th3_cosq_node_t *node;
    uint32 sched_encap;
    bcm_port_t local_port;
    int id = 0;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));

    if (!IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][local_port];

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            return BCM_E_PARAM;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            if (IS_CPU_PORT(unit, local_port)) {
                for (id = 0; id < _BCM_TH3_NUM_CPU_MCAST_QUEUE; id++) {
                    if ((cpu_port_info->mcast[id].numq == 0) ||
                        (cpu_port_info->mcast[id].in_use == 0)) {
                        break;
                    }
                }
                if (id == _BCM_TH3_NUM_CPU_MCAST_QUEUE) {
                    return BCM_E_RESOURCE;
                }
            }
            BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
            node = &cpu_port_info->mcast[id];
            node->level = SOC_TH3_NODE_LVL_L1;
            break;
        case BCM_COSQ_GPORT_SCHEDULER:
            if (numq <= 0) {
                return BCM_E_PARAM;
            }
            for (id = 0; id < _BCM_TH3_NUM_SCHEDULER_PER_PORT; id++) {
                if (IS_CPU_PORT(unit, local_port)) {
                    if (cpu_port_info->sched[id].numq == 0) {
                        break;
                    }
                }
            }
            if (id == _BCM_TH3_NUM_SCHEDULER_PER_PORT) {
                return BCM_E_RESOURCE;
            }

            sched_encap = (id << 16) | local_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node = &cpu_port_info->sched[id];
            node->level = SOC_TH3_NODE_LVL_L0;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    node->gport = *gport;
    node->numq = numq;
    node->in_use = 1;

    return BCM_E_NONE;
}

int
_bcm_th3_cosq_gport_hw_delete(int unit, int schedq, int L0)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval;

    reg = MMU_MTRO_CPU_L1_TO_L0_MAPPINGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, schedq, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, L0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, schedq, rval));

    reg = MMU_QSCH_CPU_L1_TO_L0_MAPPINGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, schedq, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, L0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, schedq, rval));

    return BCM_E_NONE;
}

int
bcm_th3_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_th3_cosq_node_t *node = NULL;
    _bcm_th3_cosq_node_t *child_node = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    int local_port, i, j;

    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }


    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_th3_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    /* Note: currently resets mapping to resetvalue of 0*/
    if (BCM_GPORT_IS_SCHEDULER(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

        cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][local_port];
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            for (i = 0; i < _BCM_TH3_NUM_CPU_MCAST_QUEUE; i++) {
                if (cpu_port_info->mcast[i].gport == gport) {
                    node = &cpu_port_info->mcast[i];
                    if (node->in_use == 0) {
                        return BCM_E_UNAVAIL;
                    }
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_gport_hw_delete(unit, i, 0));
                    node->in_use = 0;
                    node->numq = 0;
                    node->parent_gport = 0;
                }
            }
        } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
            for (i = 0; i < _BCM_TH3_NUM_SCHEDULER_PER_PORT; i++) {
                if (cpu_port_info->sched[i].gport == gport) {
                    node = &cpu_port_info->sched[i];
                    if (node->in_use == 0) {
                        return BCM_E_UNAVAIL;
                    }
                    node->in_use = 0;
                    node->numq = 0;
                    node->parent_gport = 0;
                }
                for (j = 0; j < _BCM_TH3_NUM_CPU_MCAST_QUEUE; j++) {
                    if (cpu_port_info->mcast[i].parent_gport == gport) {
                        child_node = &cpu_port_info->mcast[i];
                        child_node->in_use = 0;
                        child_node->numq = 0;
                        child_node->parent_gport = 0;
                        BCM_IF_ERROR_RETURN
                            (_bcm_th3_cosq_gport_hw_delete(unit, j, 0));
                    }
                }
            }
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/* Function:
 *     bcm_th3_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_config_get(int unit, int *numq)
{
    int prio, max_cos = 0;
    soc_mem_t mem = PORT_COS_MAPm;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (numq == NULL) {
        return BCM_E_MEMORY;
    }

    /* Read profile0 of PORT_COS_MAP to get the max number of COS */
    for (prio = 0; prio < _TH3_MMU_NUM_INT_PRI; prio++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, prio, entry));
        max_cos = MAX(max_cos, soc_mem_field32_get(unit, mem, entry, UC_COS1f));

    }
    *numq = (max_cos + 1);

    return BCM_E_NONE;
}

int
bcm_th3_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[_TH3_MMU_NUM_INT_PRI];
    void *entries[1];
    uint32 index;
    int count, i;
    bcm_port_t port;
    int cos, prio, ref_count = -1;
    soc_mem_t mem = INVALIDm;
    int num_ucq, num_mcq, num_rqe;
    uint16 dev_id;
    uint8 rev_id;

    if (numq < 1 || numq > _BCM_TH3_COS_MAX) {
        return BCM_E_PARAM;
    }

    num_ucq = _bcm_th3_get_num_ucq(unit);
    if (numq > num_ucq) {
        return BCM_E_PARAM;
    }
    num_mcq = _bcm_th3_get_num_mcq(unit);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (rev_id == BCM56980_A0_REV_ID) {
        num_rqe = 1;
    } else {
        num_rqe = num_mcq;
    }

    index = 0;
    while (index < soc_mem_index_count(unit, PORT_COS_MAPm)) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit,
                                      _bcm_th3_cos_map_profile[unit],
                                      index, &ref_count));
        if (ref_count > 0) {
            while (ref_count) {
                if (!soc_profile_mem_delete(unit,
                            _bcm_th3_cos_map_profile[unit], index)) {
                    ref_count -= 1;
                }
            }
        }
        index += _TH3_MMU_NUM_INT_PRI;
    }

    /* Distribute first 8 internal priority levels into the specified number
     * of cosq, map remaining internal priority levels to highest priority
     * cosq */
    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;
    prio = 0;
    mem = PORT_COS_MAPm;

    for (cos = 0; cos < numq; cos++) {
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            UC_COS1f, MIN(cos, num_ucq - 1));
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            UC_COS2f,  MIN(cos, num_ucq - 1));
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            MC_COS1f, MIN(cos, num_mcq ? num_mcq - 1 : 0));
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            RQE_Q_NUMf, MIN(cos, num_rqe ? num_rqe - 1 : 0));
        prio++;
    }
    for (; prio < _TH3_MMU_NUM_INT_PRI; prio++) {
        if (prio < numq) {
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS1f,
                            MIN(prio, num_ucq - 1));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS2f,
                            MIN(prio, num_ucq - 1));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], MC_COS1f,
                            MIN(prio, num_mcq ? num_mcq - 1 : 0));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], RQE_Q_NUMf,
                            MIN(prio, num_rqe ? num_rqe - 1 : 0));
        }
        else {
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS1f,
                            MIN(numq - 1, num_ucq - 1));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS2f,
                            MIN(numq - 1, num_ucq - 1));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], MC_COS1f,
                            MIN(numq - 1, num_mcq ? num_mcq - 1 : 0));
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio], RQE_Q_NUMf,
                            MIN(numq - 1, num_rqe ? num_rqe - 1 : 0));
        }
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_th3_cos_map_profile[unit], entries,
                             _TH3_MMU_NUM_INT_PRI, &index));
    count = 0;
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                    index / _TH3_MMU_NUM_INT_PRI));
        count++;
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    index / _TH3_MMU_NUM_INT_PRI));
        count++;
    }
    for (i = 1; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_th3_cos_map_profile[unit], index,
                              0);
    }

    _TH3_NUM_COS(unit) = SOC_TH3_COS_MAX;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_attach
 * Purpose:
 *     Attach sched_port to the specified index (cosq) of input_port (CPU Port only)
 * Parameters:
 *     unit         - (IN) unit number
 *     input_gport  - (IN) GPORT id, that will attach to parent gport
 *     parent_gport - (IN) Parent Gport to which current gport will be attached
 *     cosq         - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                         bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    _bcm_th3_cosq_node_t *input_node = NULL;
    bcm_port_t input_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
        "bcm_th3_cosq_gport_attach: unit=%d parent_gport=0x%x "
        "input_gport=0x%x cosq=%d\n"),
        unit, parent_gport, input_gport, cosq));

    if (!(BCM_GPORT_IS_SCHEDULER(input_gport) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                               &input_node));

    if (!input_node) {
        return BCM_E_PARAM;
    }
    if (input_port < 0) {
        return BCM_E_PORT;
    }

    if (!IS_CPU_PORT(unit, input_port)) {
        return BCM_E_PORT;
    }
    BCM_IF_ERROR_RETURN
        (bcm_th3_cosq_cpu_gport_attach(unit, input_gport,
                                                parent_gport, cosq));

    return BCM_E_NONE;
}

/* Function to detach any of the 48 L1 MC Queue from its parent 12 L0 nodes
 * Post detach, L1 MC queue is attached to Parent L0.11 node
 */
STATIC int
bcm_th3_cosq_cpu_gport_detach(int unit, bcm_gport_t input_gport,
                             bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    bcm_port_t parent_port, input_port;
    _bcm_th3_cosq_node_t *input_node = NULL, *parent_node = NULL;
    int phy_port, mmu_port;
    int input_cos = 0, parent_cos = 0, config_cos = 0;
    int cpu_mc_base = 0, cpu_sched_base = 0;
    soc_info_t *si = &SOC_INFO(unit);

    /* Post Default tree created, expected Input Gport is of MC Gport
     * and expected Parent Gport is new L0 Parent
     */
    if ((!BCM_GPORT_IS_SCHEDULER(parent_gport)) ||
        (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                               &input_node));
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, parent_gport, NULL, &parent_port, NULL,
                               &parent_node));

    if (!IS_CPU_PORT(unit, parent_port)) {
        return BCM_E_PORT;
    }

    /* Return BCM_E_PARAM when queue to be detached is
       not attached to the given gport */
    if (input_node->parent_gport != parent_gport) {
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[input_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    cpu_sched_base = mmu_port * _BCM_TH3_NUM_SCHEDULER_PER_PORT;
    parent_cos = (parent_node->hw_index - cpu_sched_base) %
                _BCM_TH3_NUM_CPU_MCAST_QUEUE;
    cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
    input_cos = (input_node->hw_index - cpu_mc_base) %
                _BCM_TH3_NUM_CPU_MCAST_QUEUE;

    /* Reset parent to L0.11 */
    SOC_IF_ERROR_RETURN
        (soc_th3_cosq_cpu_parent_get(unit, input_cos, SOC_TH3_NODE_LVL_L1,
                                    &config_cos));
    if (config_cos != parent_cos) {
        /* Error when detaching from wrong Parent (L0 level) */
        return BCM_E_PARAM;
    }

    /*
     * L0.0 is associated with CMC0 CH1(Rx)
     * L0.1 is associated with CMC0 CH2(Rx)
     * L0.2 is associated with CMC0 CH3(Rx)
     * L0.3 is associated with CMC0 CH4(Rx)
     * L0.4 is associated with CMC0 CH5(Rx)
     * L0.5 is associated with CMC0 CH6(Rx)
     * L0.6 is associated with CMC0 CH7(Rx)
     */
    if ((parent_cos >= 0) && (parent_cos < 7)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_rx_queue_channel_set(unit, parent_cos, input_cos, 0));
    }

    /* Reset parent to L0.11 */
    SOC_IF_ERROR_RETURN
        (soc_th3_cosq_cpu_parent_set(unit, input_cos, SOC_TH3_NODE_LVL_L1,
                                    _BCM_TH3_NUM_SCHEDULER_PER_PORT-1));
    input_node->parent_gport =
        _BCM_TH3_MMU_INFO(unit)->_bcm_th3_cpu_port_info->
            sched[_BCM_TH3_NUM_SCHEDULER_PER_PORT-1].gport;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of input_port (only CPU Port)
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to detach from
 *     cosq       - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                         bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    _bcm_th3_cosq_node_t *input_node = NULL;
    bcm_port_t input_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_th3_cosq_gport_detach: unit=%d input_gport=0x%x "
                         "parent_gport=0x%x cosq=%d\n"),
              unit, input_gport, parent_gport, cosq));

    if (!(BCM_GPORT_IS_SCHEDULER(input_gport) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                               &input_node));

    if (!input_node) {
        return BCM_E_PARAM;
    }

    if (!IS_CPU_PORT(unit, input_port)) {
        return BCM_E_PORT;
    }

    if (_BCM_TH3_MMU_INFO(unit)->gport_tree_created == TRUE) {
        return bcm_th3_cosq_cpu_gport_detach(unit, input_gport,
                                            parent_gport, cosq);
    } else {
        return BCM_E_INIT;
    }

}

/*
 * Function:
 *      bcm_th3_cosq_gport_child_get
 * Purpose:
 *      Get the child node GPORT atatched to N-th index (cosq). Only CPu PORT
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
bcm_th3_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                            bcm_cos_queue_t cosq,
                            bcm_gport_t *out_gport)
{
    _bcm_th3_cosq_node_t *tmp_node = NULL;
    bcm_port_t local_port = -1;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    int index = -1, cpu_mc_base = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if (out_gport == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, in_gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
            return BCM_E_PARAM;
        }

        if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
            return BCM_E_INIT;
        }

        cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
        cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];

        if (BCM_GPORT_IS_SCHEDULER(in_gport)) {
            for (index = 0; index < _BCM_TH3_NUM_CPU_MCAST_QUEUE; index++) {
                if (cpu_port_info->mcast[index].parent_gport == in_gport) {
                    if (cosq == ((cpu_port_info->mcast[index].hw_index - cpu_mc_base) %
                                 _BCM_TH3_NUM_CPU_MCAST_QUEUE)) {
                        tmp_node = &cpu_port_info->mcast[index];
                        *out_gport = tmp_node->gport;
                        goto exit;
                    }
                }
            }
        } else {
            return BCM_E_PARAM;
        }

        if (tmp_node == NULL) {
            return BCM_E_NOT_FOUND;
        }

    } else { /* Non-CPU port */
        if (_bcm_th3_cosq_port_info[unit] == NULL) {
            return BCM_E_INIT;
        }
        if ((cosq < 0) || (cosq >= _BCM_TH3_COS_MAX)) {
            return BCM_E_PARAM;
        }
        port_info = &_bcm_th3_cosq_port_info[unit][local_port];

        if (BCM_GPORT_IS_SCHEDULER(in_gport)) {
            for (index = 0; index < _BCM_TH3_MAX_NUM_UCAST_QUEUE_PER_PORT; index++) {
                if (port_info->ucast[index].parent_gport == in_gport) {
                    if (cosq == (port_info->ucast[index].cos %
                                 _BCM_TH3_COS_MAX)) {
                        tmp_node = &port_info->ucast[index];
                        *out_gport = tmp_node->gport;
                        goto exit;
                    }
                }
            }
            if (tmp_node == NULL) {
                for (index = 0; index < _BCM_TH3_MAX_NUM_MCAST_QUEUE_PER_PORT; index++) {
                    if (port_info->mcast[index].parent_gport == in_gport) {
                        if (cosq == (port_info->mcast[index].cos  %
                                     _BCM_TH3_COS_MAX)) {
                            tmp_node = &port_info->mcast[index];
                            *out_gport = tmp_node->gport;
                            goto exit;
                        }
                    }
                }
            }
        } else {
            return BCM_E_PARAM;
        }

        if (tmp_node == NULL) {
            return BCM_E_NOT_FOUND;
        }
    }

exit:
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_th3_cosq_gport_sched_get
 * Purpose:
 *     Get COS queue schedular setting
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) GPORT identifier
 *     cosq     - (IN) COS queue to get, -1 for any queue
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_XXX)
 *     weights  - (OUT) Weights for queue
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int *mode, int *weight)
{
    _bcm_th3_cosq_node_t *node = NULL;
    int local_port = -1, cpu_mc_base = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
             BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
             BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {

            BCM_IF_ERROR_RETURN
                 (_bcm_th3_cosq_node_get(unit, gport, NULL, &local_port, NULL, &node));

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                /* For Non-CPU and non-LB ports, num UC queues are 10 */
                cosq = node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                /* For Non-CPU ports, num MC queues are 10
                 * For CPU ports, num MC queues are 48
                 */
                if (IS_CPU_PORT(unit, local_port)) {
                    cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                    cosq = (node->hw_index - cpu_mc_base) %
                           _BCM_TH3_NUM_CPU_MCAST_QUEUE;
                } else {
                    cosq = node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT;
                }
            } else {
                /* For all ports, total scheduler nodes(L0) are 12 */
                cosq = node->hw_index % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_sched_get(unit, gport, cosq, mode, weight));
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport,
                                                               &local_port));

            if ((cosq < 0) || (cosq >=  _TH3_PORT_NUM_COS(unit, local_port))) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_sched_get(unit, local_port, cosq, mode, weight));
            return BCM_E_NONE;
        }
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_sched_set
 * Purpose:
 *     Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit    - (IN) unit number
 *     gport   - (IN) GPORT ID
 *     cosq    - (IN) COS queue to configure
 *     mode    - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight - (IN) Weight for COS queue
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                           bcm_cos_queue_t cosq, int mode, int weight)
{
    _bcm_th3_cosq_node_t *node = NULL;
    soc_info_t *si = &SOC_INFO(unit);
    int local_port = -1, cpu_mc_base = 0;

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {

            BCM_IF_ERROR_RETURN
                 (_bcm_th3_cosq_node_get(unit, gport, NULL, &local_port,
                                        NULL, &node));

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                /* For Non-CPU and non-LB ports, num UC queues are 12 */
                cosq = node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT;
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                /* For Non-CPU ports, num MC queues are 12
                 * For CPU ports, num MC queues are 48
                 */
                if (IS_CPU_PORT(unit, local_port)) {
                    cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                    cosq = (node->hw_index - cpu_mc_base) %
                           _BCM_TH3_NUM_CPU_MCAST_QUEUE;
                } else {
                    cosq = node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT;
                }
            } else {
                /* For all ports, total scheduler nodes(L0) are 10 */
                cosq = node->hw_index % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_sched_set(unit, gport, cosq, mode, weight));
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport,
                                                               &local_port));

            if ((cosq < 0) || (cosq >= _TH3_PORT_NUM_COS(unit, local_port))) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_sched_set(unit, local_port, cosq, mode, weight));
            return BCM_E_NONE;
        }
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_th3_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_init(int unit)
{
    STATIC int _th3_max_cosq = -1;
    int i, alloc_size;
    int pipe = -1, itm = -1;
    uint32 pipe_map;
    bcm_port_t port;
    soc_mem_t mem = INVALIDm;
    static soc_mem_t wred_mems[_BCM_TH3_NUM_WRED_PROFILES] = {
        MMU_WRED_DROP_CURVE_PROFILE_0_0m, MMU_WRED_DROP_CURVE_PROFILE_1_0m,
        MMU_WRED_DROP_CURVE_PROFILE_2_0m, MMU_WRED_DROP_CURVE_PROFILE_3_0m,
        MMU_WRED_DROP_CURVE_PROFILE_4_0m, MMU_WRED_DROP_CURVE_PROFILE_5_0m,
        MMU_WRED_DROP_CURVE_PROFILE_6_0m, MMU_WRED_DROP_CURVE_PROFILE_7_0m,
        MMU_WRED_DROP_CURVE_PROFILE_8_0m
    };
    int entry_words[_BCM_TH3_NUM_WRED_PROFILES];
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    mmu_wred_drop_curve_profile_6_entry_t entry_ecn_green;
    mmu_wred_drop_curve_profile_7_entry_t entry_ecn_yellow;
    mmu_wred_drop_curve_profile_8_entry_t entry_ecn_red;
    void *entries[_BCM_TH3_NUM_WRED_PROFILES];
    int wred_prof_count;
    soc_mem_t wred_config_mem = INVALIDm;
    uint32 profile_index;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int min_index, max_index;
    int num_uc_q;

    _bcm_bst_device_handlers_t bst_callbks;

    _bcm_th3_cosq_cpu_cosq_config_t *cpu_cosq_config_p = NULL;
    int rv = BCM_E_NONE;

    if (_th3_max_cosq < 0) {
        _th3_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 12;
    }

    sal_memset(&entry, 0, sizeof(entry));

    if (!SOC_WARM_BOOT(unit)) {
        _bcm_th3_cosq_cleanup(unit);
    }


    /* Create Port Information structure */
    alloc_size = sizeof(_bcm_th3_cosq_port_info_t) * SOC_TH3_MAX_NUM_PORTS;
    if (_bcm_th3_cosq_port_info[unit] == NULL) {
        _bcm_th3_cosq_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_th3_cosq_port_info");
            if (_bcm_th3_cosq_port_info[unit] == NULL) {
                return BCM_E_MEMORY;
            }
    }
    sal_memset(_bcm_th3_cosq_port_info[unit], 0, alloc_size);

    /* Create CPU Port Information structure */
    alloc_size = sizeof(_bcm_th3_cosq_cpu_port_info_t);
    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        _bcm_th3_cosq_cpu_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_th3_cosq_cpu_port_info");
        if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_th3_cosq_cpu_port_info[unit], 0, alloc_size);

    /* Create MMU Port Information structure */
    alloc_size = sizeof(_bcm_th3_mmu_info_t);
    if (_bcm_th3_mmu_info[unit] == NULL) {
        _bcm_th3_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_th3_mmu_info");
        if (_bcm_th3_mmu_info[unit] == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_th3_mmu_info[unit], 0, alloc_size);
    _BCM_TH3_MMU_INFO(unit)->_bcm_th3_port_info =
            _bcm_th3_cosq_port_info[unit];
    _BCM_TH3_MMU_INFO(unit)->_bcm_th3_cpu_port_info =
            _bcm_th3_cosq_cpu_port_info[unit];

    /* Create profile for PORT_COS_MAP table */
    if (_bcm_th3_cos_map_profile[unit] == NULL) {
        _bcm_th3_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_th3_cos_map_profile[unit] == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_th3_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);

    rv = soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_th3_cos_map_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for IFP_COS_MAP table */
    if (_bcm_th3_ifp_cos_map_profile[unit] == NULL) {
        _bcm_th3_ifp_cos_map_profile[unit]
            = sal_alloc(sizeof(soc_profile_mem_t), "IFP_COS_MAP Profile Mem");
        if (_bcm_th3_ifp_cos_map_profile[unit] == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_th3_ifp_cos_map_profile[unit]);
    }
    mem = IFP_COS_MAPm;
    entry_words[0] = sizeof(ifp_cos_map_entry_t) / sizeof(uint32);

    min_index = soc_mem_index_min(unit, IFP_COS_MAPm);
    max_index = soc_mem_index_max(unit, IFP_COS_MAPm) - _TH3_MMU_NUM_INT_PRI;
    rv = soc_profile_mem_index_create(unit, &mem, entry_words,
                                &min_index, &max_index, NULL, 1,
                                _bcm_th3_ifp_cos_map_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_th3_wred_profile[unit] == NULL) {
        _bcm_th3_wred_profile[unit] =
                sal_alloc(sizeof(soc_profile_mem_t), "WRED Profile Mem");
        if (_bcm_th3_wred_profile[unit] == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_th3_wred_profile[unit]);
    }


    for (i = 0; i < _BCM_TH3_NUM_WRED_PROFILES; i++) {
        entry_words[i] =
            sizeof(mmu_wred_drop_curve_profile_0_entry_t) / sizeof(uint32);
    }

    rv = soc_profile_mem_create(unit, wred_mems, entry_words, _BCM_TH3_NUM_WRED_PROFILES,
                                _bcm_th3_wred_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    if (_bcm_th3_cosq_cpu_cosq_config[unit][0] == NULL) {
        cpu_cosq_config_p = sal_alloc(
                SOC_TH3_NUM_CPU_QUEUES * sizeof(_bcm_th3_cosq_cpu_cosq_config_t),
                "CPU Cosq Config");
        if (cpu_cosq_config_p == NULL) {
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        if (NUM_CPU_COSQ(unit) > SOC_TH3_NUM_CPU_QUEUES) {
            /* Invalid CPU Queues */
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_INIT;
        }
        for (i = 0; i < NUM_CPU_COSQ(unit); i++) {
            _bcm_th3_cosq_cpu_cosq_config[unit][i] = cpu_cosq_config_p + i;
        }
    }

    /* Assign default init values for RQE queue number */
    rv = _bcm_th3_rqe_q_init(unit);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    /* Requires ECN init to program the default Responsive/Non-responsive
     * settings in Hardware
     */
    if(soc_feature(unit, soc_feature_ecn_wred)) {
        rv = _bcm_th3_ecn_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }

    rv = _bcm_oob_init(unit);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = bcm_th3_cosq_reinit(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
        }
    /*
     * COVERITY
     *
     * This checker will not be True.
     * cpu_cosq_config_p pointer is assigned to
     * _bcm_th3_cosq_cpu_cosq_config global data
     * structure.
     */
    /* coverity[resource_leak] */
        return rv;
    } else {
        rv = _bcm_th3_cosq_wb_alloc(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Assign default init values for CPU info */
    for (i = 0; i < NUM_CPU_COSQ(unit); i++) {
        if (_bcm_th3_cosq_cpu_cosq_config[unit][i] == NULL) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_bcm_th3_cosq_cpu_cosq_config[unit][i], 0,
                   sizeof(_bcm_th3_cosq_cpu_cosq_config_t));
        _bcm_th3_cosq_cpu_cosq_config[unit][i]->enable = 1;
    }

    /* Add default entries for PORT_COS_MAP memory profile */
    num_uc_q = _bcm_th3_get_num_ucq(unit);
    rv = bcm_th3_cosq_config_set(unit, num_uc_q);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        rv = _bcm_th3_pfc_deadlock_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            soc_tomahawk3_pipe_map_get(unit, &pipe_map);
            if (pipe_map & (1 << pipe)) {
                mem = MMU_WRED_QUEUE_CONFIGm;
                rv = soc_mem_clear(unit, mem, COPYNO_ALL, 1);
                if (rv != SOC_E_NONE) {
                    if (cpu_cosq_config_p != NULL) {
                        sal_free(cpu_cosq_config_p);
                        cpu_cosq_config_p = NULL;
                    }
                    _bcm_th3_cosq_cleanup(unit);
                    return rv;
                }
            }
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            soc_tomahawk3_pipe_map_get(unit, &pipe_map);
            if (pipe_map & (1 << pipe)) {
                mem = MMU_WRED_PORTSP_CONFIGm;
                rv = soc_mem_clear(unit, mem, COPYNO_ALL, 1);
                if (rv != SOC_E_NONE) {
                    if (cpu_cosq_config_p != NULL) {
                        sal_free(cpu_cosq_config_p);
                        cpu_cosq_config_p = NULL;
                    }
                    _bcm_th3_cosq_cleanup(unit);
                    return rv;
                }
            }
        }
    }

    /* Add default entries for MMU_WRED_DROP_CURVE_PROFILE_x memory profile */
    sal_memset(&entry_tcp_green, 0, sizeof(entry_tcp_green));
    sal_memset(&entry_tcp_yellow,0, sizeof(entry_tcp_yellow));
    sal_memset(&entry_tcp_red, 0, sizeof(entry_tcp_red));
    sal_memset(&entry_nontcp_green, 0, sizeof(entry_nontcp_green));
    sal_memset(&entry_nontcp_yellow, 0, sizeof(entry_nontcp_yellow));
    sal_memset(&entry_nontcp_red, 0, sizeof(entry_nontcp_red));
    sal_memset(&entry_ecn_green, 0, sizeof(entry_ecn_green));
    sal_memset(&entry_ecn_yellow,0, sizeof(entry_ecn_yellow));
    sal_memset(&entry_ecn_red, 0, sizeof(entry_ecn_red));
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    entries[6] = &entry_ecn_green;
    entries[7] = &entry_ecn_yellow;
    entries[8] = &entry_ecn_red;

    for (i = 0; i < _BCM_TH3_NUM_WRED_PROFILES; i++) {
        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                            MIN_THDf, 0x7ffff);
        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                            MAX_THDf, 0x7ffff);
    }
    profile_index = TH3_WRED_PROFILE_INDEX_INVALID;
    wred_config_mem = MMU_WRED_QUEUE_CONFIGm;
    wred_prof_count = soc_mem_index_count(unit, wred_config_mem);
    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit, "wred_prof_count: %d\n"),
              wred_prof_count));

    
    wred_prof_count *= (2 * NUM_PIPE(unit));
    while(wred_prof_count) {
        if (profile_index == TH3_WRED_PROFILE_INDEX_INVALID) {
            rv = soc_profile_mem_add(unit, _bcm_th3_wred_profile[unit],
                                     entries, 1, &profile_index);
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "1 profile_index: %d\n"),
                    profile_index));
        } else {
            rv = soc_profile_mem_reference(unit, _bcm_th3_wred_profile[unit],
                                           profile_index, 0);
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "2 profile_index: %d\n"),
                    profile_index));
        }
        if (rv != SOC_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
        wred_prof_count -= 1;
    }

    for (i = 0; i < _BCM_TH3_NUM_WRED_RESOLUTION_TABLE; i++) {
        if (i == 0) {
            _bcm_th3_wred_resolution_tbl[i] = 1;
        } else {
            _bcm_th3_wred_resolution_tbl[i] = 0;
        }
    }

    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = FALSE;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        rv = _bcm_th3_cosq_gport_tree_create(unit, port);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }
    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = TRUE;

    rv = _bcm_th3_cosq_gport_default_config(unit);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    /* BST init */
    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &bcm_bst_th3_index_resolve;
    bst_callbks.reverse_resolve_index =
        &_bcm_th3_cosq_bst_map_resource_to_gport_cos;
    rv = _bcm_bst_attach(unit, &bst_callbks);
    /* BST init done */
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

#if defined(INCLUDE_PSTATS)
    if (soc_feature(unit, soc_feature_pstats)) {
        rv = _bcm_pstats_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }
#endif /* INCLUDE_PSTATS */

    rv = soc_th3_mmu_get_shared_size(unit,
                     _BCM_TH3_MMU_INFO(unit)->shared_limit);

    if (rv != SOC_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_th3_cosq_cleanup(unit);
        return rv;
    }

    /* reset time_domain ref_count so that entries can be added upon
     * rc */
    th3_time_domain_info[unit][0].ref_count = 324;
    th3_time_domain_info[unit][1].ref_count = 0;
    th3_time_domain_info[unit][2].ref_count = 0;
    th3_time_domain_info[unit][3].ref_count = 0;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_port_profile_set
 * Purpose:
 *     Set Port to Profile Mapping for Cosq profiles
 * Parameters:
 *     unit         - (IN) unit number
 *     port         - (IN) logical port
 *     profile_type - (IN) Profile type
 *     profile_id   - (IN) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_port_profile_set(int unit, bcm_port_t port,
        bcm_cosq_profile_type_t profile_type, int profile_id)
{
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_QSCHED_PROFILES)) {
                return BCM_E_PARAM;
            }
            if (!soc_tomahawk3_profile_exists(unit, profile_id)) {
                return BCM_E_CONFIG;
            }
            /* coverity[overrun-local] */
            BCM_IF_ERROR_RETURN(
                soc_tomahawk3_sched_profile_attach(unit, port, profile_id));
            BCM_IF_ERROR_RETURN(
                soc_tomahawk3_l1_to_l0_profile_update_per_port(unit, port, profile_id));
            BCM_IF_ERROR_RETURN(_bcm_th3_cosq_gport_tree_create(unit, port));
            BCM_IF_ERROR_RETURN(soc_th3_pfc_config_profile_id_set(unit, port,
                socTH3PFCReceiveClass, profile_id));
            break;
        case bcmCosqProfilePGProperties:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_PG_PROP_PROFILES)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_port_pg_profile_set(unit, port, profile_id));
            break;
        case bcmCosqProfileIntPriToPGMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_INTPRI_PG_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_port_inp_pri_to_pg_profile_set(unit, port, profile_id));
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_OOB_EGRQUEUE_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                    bcm_th3_oob_fx_egress_profile_id_set(unit, port, profile_id));
            break;
        default:
            return SOC_E_PARAM;
            break;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *     bcm_tomahawk3_cosq_port_profile_get
 * Purpose:
 *     Get Port to Profile Mapping for Cosq profiles
 * Parameters:
 *     unit         - (IN) unit number
 *     port         - (IN) logical port
 *     profile_type - (IN) Profile type
 *     profile_id   - (OUT) profile id
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_port_profile_get(int unit, bcm_port_t port,
        bcm_cosq_profile_type_t profile_type, int *profile_id)
{
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            BCM_IF_ERROR_RETURN(
                soc_tomahawk3_sched_profile_get(unit, port, profile_id));
            break;
        case bcmCosqProfilePGProperties:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_port_pg_profile_get(unit, port, profile_id));
            break;
        case bcmCosqProfileIntPriToPGMap:
            BCM_IF_ERROR_RETURN(
                _bcm_th3_cosq_port_inp_pri_to_pg_profile_get(unit, port, profile_id));
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            BCM_IF_ERROR_RETURN(
                    bcm_th3_oob_fx_egress_profile_id_get(unit, port, profile_id));
            break;
        default:
            return SOC_E_PARAM;
            break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_profile_property_dynamic_get
 * Purpose:
 *     Query if a profile can be modified dynamically
 * Parameters:
 *     unit         - (IN) unit number
 *     profile_type - (IN) Profile type
 *     dynamic      - (OUT) Dynamic changeable type
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_profile_property_dynamic_get(int unit,
        bcm_cosq_profile_type_t profile_type, bcm_cosq_dynamic_setting_type_t
        *dynamic)
{
    if (dynamic == NULL) {
        return BCM_E_PARAM;
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            *dynamic = bcmCosqDynamicTypeConditional;
            break;
        case bcmCosqProfilePGProperties:
            *dynamic = bcmCosqDynamicTypeFixed;
            break;
        case bcmCosqProfileIntPriToPGMap:
            *dynamic = bcmCosqDynamicTypeFlexible;
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            *dynamic = bcmCosqDynamicTypeConditional;
            break;
        default:
            return SOC_E_PARAM;
            break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_profile_info_inuse_get
 * Purpose:
 *     Query if a profile id of a profile is in use
 * Parameters:
 *     unit         - (IN) unit number
 *     profile_type - (IN) Profile type
 *     profile_id   - (IN) Profile id
 *     in_use      - (OUT) If a profile is in use
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_profile_info_inuse_get(int unit,
        bcm_cosq_profile_type_t profile_type, int profile_id, int *in_use)
{
    int port, ret_profile = -1;
    if (in_use == NULL) {
        return BCM_E_PARAM;
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_QSCHED_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfilePGProperties:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_PG_PROP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfileIntPriToPGMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_INTPRI_PG_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_OOB_EGRQUEUE_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        default:
            return BCM_E_PARAM;
            break;
    }
    PBMP_ALL_ITER(unit, port) {
        if (profile_type == bcmCosqProfilePFCAndQueueHierarchy) {
            if (IS_CPU_PORT(unit, port)) {
                continue;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_tomahawk3_cosq_port_profile_get(unit, port, profile_type,
                                                            &ret_profile));
        if (ret_profile == profile_id) {
            *in_use = 1;
            return BCM_E_NONE;
        }
    }
    *in_use = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_profile_info_portlist_get
 * Purpose:
 *     Query the list of ports using a specific profile
 * Parameters:
 *     unit         - (IN) unit number
 *     profile_type - (IN) Profile type
 *     profile_id   - (IN) Profile id
 *     port_count   - (OUT) Number of ports using this profile
 *     max_port_count - (OUT) Max Number of ports that can use this profile
 *     port_list    - (OUT) List of ports using this profile
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_profile_info_portlist_get(int unit,
        bcm_cosq_profile_type_t profile_type, int profile_id, int max_port_count,
         bcm_port_t *port_list, int *port_count)
{
    int port, ret_profile = -1;
    if (port_count == NULL || port_list == NULL) {
        return BCM_E_PARAM;
    }
    if (max_port_count <= 0 || max_port_count > SOC_TH3_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_QSCHED_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfilePGProperties:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_PG_PROP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfileIntPriToPGMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_INTPRI_PG_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            if ((profile_id < 0) || (profile_id >=
                        _BCM_TH3_NUM_OOB_EGRQUEUE_MAP_PROFILES)) {
                return BCM_E_PARAM;
            }
            break;
        default:
            return BCM_E_PARAM;
            break;
    }
    *port_count = 0;
    PBMP_ALL_ITER(unit, port) {
        if (profile_type == bcmCosqProfilePFCAndQueueHierarchy) {
            if (IS_CPU_PORT(unit, port)) {
                continue;
            }
        }
        BCM_IF_ERROR_RETURN(
            bcm_tomahawk3_cosq_port_profile_get(unit, port, profile_type,
                                                            &ret_profile));
        if (ret_profile == profile_id) {
            port_list[(*port_count)++] = port;
            if((*port_count) == max_port_count) {
                break;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * This function does two things:
 * 1. Tests whether the corresponding COS_BMP bit is set or not.
 * 2. Tests whether the queue corresponding to given cos is
 *     mapped to correct L0 or not.
 *
 * L0.0 is associated with CMC0 CH1(Rx) (pci)
 * L0.1 is associated with CMC0 CH2(Rx) (pci)
 * L0.2 is associated with CMC0 CH3(Rx) (pci)
 * L0.3 is associated with CMC0 CH4(Rx) (pci)
 * L0.4 is associated with CMC0 CH5(Rx) (pci)
 * L0.5 is associated with CMC0 CH6(Rx) (pci)
 * L0.6 is associated with CMC0 CH7(Rx) (pci)
 * L0.7 is associated with CMC1 CHx(Rx) (uc0)
 * L0.8 is associated with CMC1 CHx(Rx) (uC1)
 * L0.11 is reserved
 */
int
bcm_th3_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                  bcm_rx_chan_t chan_id)
{
#if defined(BCM_CMICX_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicx)) {
        int pci_cmc = SOC_PCI_CMC(unit);
        uint32 reg_val;
        uint32 bit;
        uint32 max_rx_channels = 0;
        _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
        bcm_gport_t cpu_l0_gport = -1;
        bcm_gport_t parent_gport = -1;

        soc_dma_max_rx_channels_get(unit, &max_rx_channels);

        if ((chan_id <= 0) || (chan_id >= max_rx_channels)) {
            return BCM_E_PARAM;
        }

        if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
            return BCM_E_INIT;
        }

        cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];

        if ((chan_id > 0) && (chan_id < 8 ))  {
            cpu_l0_gport = cpu_port_info->sched[chan_id - 1].gport;
        }

        if (queue_id < 0) { /* All COS Queues */
            for (queue_id = 0; queue_id < NUM_CPU_COSQ(unit); queue_id++) {
                soc_dma_cos_ctrl_queue_get(unit, pci_cmc, chan_id, queue_id,
                                           &reg_val);

                bit = 1 << (queue_id % 32);
                if (reg_val & bit) {
                    /* find the parent gport of this queue if parent gport is
                     * not equal to l0 gport for this channel return error */
                    parent_gport = cpu_port_info->mcast[queue_id].parent_gport;
                    if (parent_gport != cpu_l0_gport) {
                        return BCM_E_PARAM;
                    }
                }
            }
        } else {
            soc_dma_cos_ctrl_queue_get(unit, pci_cmc, chan_id, queue_id,
                                       &reg_val);
            bit = 1 << (queue_id % 32);
            if (reg_val & bit) {
                /* find the parent gport of this queue if parent gport is not
                 * equal to l0 gport for this channel return error */
                parent_gport = cpu_port_info->mcast[queue_id].parent_gport;
                if (parent_gport != cpu_l0_gport) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        }
    }
#endif /* BCM_CMICX_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_localport_resolve
 * Purpose:
 *     Resolve GPORT if it is a local port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     local_port - (OUT) local port number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result, rv;

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
        return BCM_E_NONE;
    }

    rv = _bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id);
    if (rv == BCM_E_PORT) {
        /* returning E_PARAM when invalid gport is given as param */
        return BCM_E_PARAM;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }

    *local_port = port;

    return BCM_E_NONE;
}

int
_bcm_th3_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                           int *p_pool_start, int *p_pool_end)
{
    int local_port;
    int pool;

    if (cosq == BCM_COS_INVALID) {
        if (p_pool_start && p_pool_end) {
            *p_pool_start = 0;
            *p_pool_end = 3;
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));

    if (p_pool_start) {
        *p_pool_start = pool;
    }
    if (p_pool_end) {
        *p_pool_end = pool;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_index_resolve
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
_bcm_th3_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_th3_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count)
{
    bcm_port_t resolved_port;
    soc_info_t *si;
    int resolved_index = -1;
    int id, startq, numq = 0;
    int num_uc_q, num_mc_q;
    int pipe;
    int uc_base = 0, mc_base = 0, cpu_mc_base = 0; /* Q base number */
    int chip_mmu_port; /* chip_port_num*/
    _bcm_th3_cosq_node_t *node;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;

    si = &SOC_INFO(unit);

    if (cosq < -1) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        startq = 0;
        numq = _TH3_NUM_COS(unit);
    } else {
        startq = cosq;
        numq = 1;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, port, NULL, &resolved_port, &id,
                                   &node));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = (IS_CPU_PORT(unit, resolved_port)) ? NUM_CPU_COSQ(unit) : _BCM_TH3_COS_MAX;
    }
    num_uc_q = _bcm_th3_get_num_ucq(unit);
    num_mc_q = _bcm_th3_get_num_mcq(unit);
    mc_base = si->port_cosq_base[resolved_port];
    uc_base = si->port_uc_cosq_base[resolved_port];

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, resolved_port, &pipe));

    switch (style) {
        case _BCM_TH3_COSQ_INDEX_STYLE_WRED_DEVICE:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if ((port == -1) || (cosq == -1)) {
                numq = _TH3_MMU_NUM_POOL; /* 4 Global Service Pools */
                resolved_index = 0;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_egress_sp_get(unit, port, cosq,
                                                &resolved_index,
                                                NULL));
                numq = 1;
                resolved_index += 0;
            }
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_WRED_PORT:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            resolved_index = 0;
            if (cosq != -1) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_egress_sp_get(unit, port, cosq,
                                                &resolved_index,
                                                NULL));
            }
            chip_mmu_port = SOC_TH3_MMU_CHIP_PORT(unit, resolved_port);
            resolved_index += (chip_mmu_port * 4); /* resolved_index is chip_port_num, spid.*/
            numq = (cosq == -1) ? 4: 1;
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_WRED_QUEUE:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            /* resolved_index = local_mmu_port *  _BCM_TH3_MAX_QUEUE_PER_PORT; */
            resolved_index = uc_base;
            if (cosq != -1) {
                if (cosq >= _bcm_th3_get_num_ucq(unit)) { /* WRED is only for UCQ*/
                    return BCM_E_PARAM;
                }
                resolved_index += cosq;
            }
            numq = (cosq == -1) ? _bcm_th3_get_num_ucq(unit): 1;
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL:
            if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    /* regular unicast queue */
                    resolved_index = (uc_base + (node->hw_index %
                                       num_uc_q));
                    mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                    if (IS_CPU_PORT(unit, resolved_port)) {
                        cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                        resolved_index = cpu_mc_base +
                                         ((node->hw_index - cpu_mc_base) %
                                           _BCM_TH3_NUM_CPU_MCAST_QUEUE);
                    } else {
                        resolved_index = (mc_base + (node->hw_index %
                                           num_mc_q));
                    }
                    mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
                } else { /* scheduler */
                    return BCM_E_PARAM;
                }
            } else { /* node == NULL */
                numq = si->port_num_cosq[resolved_port] +
                       si->port_num_uc_cosq[resolved_port];
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = (IS_CPU_PORT(unit, resolved_port) ? mc_base : uc_base) + startq;
                mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
            }
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                                entry));
            resolved_index = soc_mem_field32_get(unit, mem, entry, SPIDf);
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_COS:
            if (node) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                     resolved_index = node->hw_index % num_uc_q;
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                     if (IS_CPU_PORT(unit, resolved_port)) {
                         cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                         resolved_index = (node->hw_index - cpu_mc_base) %
                                          NUM_CPU_COSQ(unit);
                     } else {
                         resolved_index = node->hw_index % num_mc_q;
                     }
                } else {
                     resolved_index = node->hw_index %
                                      _BCM_TH3_NUM_SCHEDULER_PER_PORT;
                }
            } else {
                resolved_index = startq;
            }
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE:
            /* Return hw_index for given unicast queue gport */
            if (node) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                     resolved_index = (uc_base + (node->hw_index % num_uc_q));
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                if (IS_CPU_PORT(unit, resolved_port)) {
                    return BCM_E_PARAM;
                }
                if (startq >= num_uc_q) {
                    return BCM_E_PARAM;
                }
                resolved_index = uc_base + startq;
            }
            break;
        case _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE:
            /* Return hw_index for given multicast queue gport */
            if (node) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                     if (IS_CPU_PORT(unit, resolved_port)) {
                         cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                         resolved_index = cpu_mc_base +
                                          ((node->hw_index - cpu_mc_base) %
                                              _BCM_TH3_NUM_CPU_MCAST_QUEUE);
                     } else {
                         resolved_index = (mc_base + (node->hw_index % num_mc_q));
                     }
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                if (IS_CPU_PORT(unit, resolved_port)) {
                    if (startq >= numq) {
                        return BCM_E_PARAM;
                    }
                    resolved_index = mc_base + startq;
                } else {
                    if ((startq < num_uc_q) || (startq >= numq)) {
                        return BCM_E_PARAM;
                    }
                    resolved_index = mc_base + (startq - num_uc_q);
                }
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    if (index) {
        *index = resolved_index;
    }
    if (local_port) {
        *local_port = resolved_port;
    }
    if (count) {
        *count = (cosq == -1) ? numq : 1;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_cosq_mapping_set
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
int
_bcm_th3_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq1,
                         bcm_cos_queue_t cosq2)
{
    bcm_port_t local_port, out_port;
    bcm_cos_queue_t hw_cosq[3];
    int rv, idx;
    soc_field_t field[3] = {INVALIDf, INVALIDf, INVALIDf}; /* [0]:UC, [1]:MC cos */
    void *entries[1];
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[_TH3_MMU_NUM_INT_PRI];
    uint32 old_index, new_index;
    int num_ucq, num_mcq;

    num_ucq = _bcm_th3_get_num_ucq(unit);
    num_mcq = _bcm_th3_get_num_mcq(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport != -1) {
        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport,
                                                           &out_port));
    }

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            if (gport == -1) {
                hw_cosq[0] = cosq1;
            } else {
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, gport, cosq1, _BCM_TH3_COSQ_INDEX_STYLE_COS,
                      NULL, &hw_cosq[0], NULL));
            }
            hw_cosq[0] %= num_ucq;
            field[0] = UC_COS1f;
            break;
        case BCM_COSQ_GPORT_ELEPHANT_UCAST_QUEUE_GROUP:
            if (gport == -1) {
                hw_cosq[0] = cosq1;
            } else {
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, gport, cosq1, _BCM_TH3_COSQ_INDEX_STYLE_COS,
                      NULL, &hw_cosq[0], NULL));
            }
            hw_cosq[0] %= num_ucq;
            field[0] = UC_COS2f;
            break;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            if (gport == -1) {
                hw_cosq[0] = cosq1;
            } else {
                if (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, gport, cosq1, _BCM_TH3_COSQ_INDEX_STYLE_COS,
                      NULL, &hw_cosq[0], NULL));
            }
            hw_cosq[0] %= num_mcq;
            field[0] = MC_COS1f;
            break;
        case (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
              BCM_COSQ_GPORT_MCAST_QUEUE_GROUP):
            if (gport == -1) {
                hw_cosq[0] = cosq1;
                hw_cosq[1] = cosq2;
            } else {
                return BCM_E_PARAM;
            }
            /*
            hw_cosq[0] %= _BCM_TH3_NUM_SCHEDULER_PER_PORT;
            hw_cosq[1] %= _BCM_TH3_NUM_SCHEDULER_PER_PORT;
            hw_cosq[2] %= _BCM_TH3_NUM_SCHEDULER_PER_PORT;
            */
            hw_cosq[0] = MIN(hw_cosq[0], num_ucq - 1);
            hw_cosq[1] = MIN(hw_cosq[1], num_mcq ? num_mcq - 1 : 0);
            field[0] = UC_COS1f;
            field[1] = MC_COS1f;
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
    old_index *= _TH3_MMU_NUM_INT_PRI;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_th3_cos_map_profile[unit],
                             old_index, _TH3_MMU_NUM_INT_PRI, entries));

    for (idx = 0; idx < 3; idx++) {
        if (field[idx] != INVALIDf) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_entries[priority],
                                field[idx], hw_cosq[idx]);
        }
    }

    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_delete(unit, _bcm_th3_cos_map_profile[unit],
                                old_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_th3_cos_map_profile[unit], entries,
                             _TH3_MMU_NUM_INT_PRI, &new_index);

    soc_mem_unlock(unit, PORT_COS_MAPm);

    if (BCM_FAILURE(rv)) {
        return rv;
    }
    soc_mem_field32_set(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf,
                        new_index / _TH3_MMU_NUM_INT_PRI);
    BCM_IF_ERROR_RETURN
        (WRITE_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                            &cos_map_sel_entry));
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (IS_CPU_PORT(unit, local_port)) {
        if (SOC_INFO(unit).cpu_hg_index != -1) {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                        SOC_INFO(unit).cpu_hg_index, SELECTf,
                                        new_index / _TH3_MMU_NUM_INT_PRI));
        }
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_mapping_set
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
bcm_th3_cosq_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
                        bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;

    BCM_PBMP_CLEAR(pbmp);

    if ((priority < 0) || (priority >= _TH3_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (port == -1) {
        /* port value of -1 will cause the cosq mapping set for
           all types of local ports except Loopback ports.
           So we do the sanity check of cos value with Default Number
           of COS */
        if (cosq >= _TH3_NUM_COS(unit)) {
            return BCM_E_PARAM;
        }
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
        if (cosq >= _TH3_PORT_NUM_COS(unit, local_port)) {
            return BCM_E_PARAM;
        }
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         * mcast */
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_mapping_set(unit, local_port,
                                      priority,
                                      BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                      BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                      -1, cosq, cosq));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_cosq_mapping_get
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
_bcm_th3_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t *gport,
                         bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    cos_map_sel_entry_t cos_map_sel_entry;
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_th3_cosq_node_t *node;
    int ii, index, cosq_base = 0;
    void *entry_p;
    bcm_cos_queue_t hw_cosq = BCM_COS_INVALID, node_cosq = BCM_COS_INVALID;
    int numq = -1;
    soc_info_t *si = &SOC_INFO(unit);
    int num_ucq, num_mcq;

    if ((flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) &&
        (flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) &&
        (flags != BCM_COSQ_GPORT_ELEPHANT_UCAST_QUEUE_GROUP)) {
        return BCM_E_PARAM;
    }

    num_ucq = _bcm_th3_get_num_ucq(unit);
    num_mcq = _bcm_th3_get_num_mcq(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, ing_port, &local_port));

    if (gport) {
        *gport = BCM_GPORT_INVALID;
    }

    *cosq = BCM_COS_INVALID;

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= _TH3_MMU_NUM_INT_PRI;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_th3_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    index + priority);
    if (IS_CPU_PORT(unit, local_port)) {
        cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][local_port];
    } else {
        port_info = &_bcm_th3_cosq_port_info[unit][local_port];
    }

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p,
                                          UC_COS1f);
            if ((IS_CPU_PORT(unit, local_port) == FALSE) && (gport)) {
                for (ii = 0; ii < num_ucq; ii++) {
                    node = &port_info->ucast[ii];
                    if (node == NULL) {
                        return BCM_E_NOT_FOUND;
                    }
                    node_cosq = (node->hw_index % num_ucq);
                    if (node && (node_cosq == hw_cosq)) {
                        *gport = node->gport;
                        *cosq = hw_cosq;
                        break;
                    }
                }
            } else {
                *cosq = hw_cosq;
            }
            break;
        case BCM_COSQ_GPORT_ELEPHANT_UCAST_QUEUE_GROUP:
            hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p,
                                          UC_COS2f);
            if ((IS_CPU_PORT(unit, local_port) == FALSE) && (gport)) {
                for (ii = 0; ii < num_ucq; ii++) {
                    node = &port_info->ucast[ii];
                    if (node == NULL) {
                        return BCM_E_NOT_FOUND;
                    }
                    node_cosq = (node->hw_index % num_ucq);
                    if (node && (node_cosq == hw_cosq)) {
                        *gport = node->gport;
                        *cosq = hw_cosq;
                        break;
                    }
                }
            } else {
                *cosq = hw_cosq;
            }
            break;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p,
                                          MC_COS1f);
            if (gport) {
                if (IS_CPU_PORT(unit, local_port)) {
                    numq = _BCM_TH3_NUM_CPU_MCAST_QUEUE;
                    cosq_base = si->port_cosq_base[CMIC_PORT(unit)];
                } else {
                    numq = num_mcq;
                    cosq_base = 0;
                }
                for (ii = 0; ii < numq; ii++) {
                    if (IS_CPU_PORT(unit, local_port)) {
                        node = &cpu_port_info->mcast[ii];
                    } else {
                        node = &port_info->mcast[ii];
                    }
                    if (!node) {
                        return BCM_E_NOT_FOUND;
                    }
                    node_cosq = ((node->hw_index - cosq_base) % numq);
                    if (node_cosq == hw_cosq) {
                        *gport = node->gport;
                        *cosq = hw_cosq;
                        break;
                    }
                }
            } else {
                *cosq = hw_cosq;
            }
            break;
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally
         * as a defensive default for future development.
         */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_PARAM;
    }
    if (((gport &&
        (*gport == BCM_GPORT_INVALID) && (*cosq == BCM_COS_INVALID))) ||
        (*cosq == BCM_COS_INVALID)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_mapping_get
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
bcm_th3_cosq_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
                        bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    if ((priority < 0) || (priority >= _TH3_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

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
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port, &local_port));
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
        if (IS_LB_PORT(unit, local_port) || IS_CPU_PORT(unit, local_port)) {
            continue;
        }

        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_mapping_get(unit, local_port,
                 priority, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL, cosq));
        break;
    }
    return BCM_E_NONE;
}


/*
 * This function is used to set shared pool
 * for given Ingress [Port, Priority_group]
 */
int
_bcm_th3_cosq_ingress_sp_set_by_pg(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
                            int arg)
{
    int local_port;
    uint64 rval;
    int profile_index;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, local_port, -1, &rval));

    profile_index = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval, PG_PROFILE_SELf);

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, &rval));
    soc_reg64_field32_set(unit, MMU_THDI_PG_PROFILEr, &rval, prigroup_spid_field[pri_grp], arg);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, rval));


    return BCM_E_NONE;
}

/*
 * This function sets the shared limit in mmu info
*/
int _bcm_th3_mmu_set_shared_size(int unit, int itm, int shd_size)
{
    _BCM_TH3_MMU_INFO(unit)->shared_limit[itm] = shd_size;
    return BCM_E_NONE;
}

/*
 * This function retrieves the shared limit in mmu info
 */
int _bcm_th3_mmu_get_shared_size(int unit, int itm, int *shd_size)
{
    *shd_size = _BCM_TH3_MMU_INFO(unit)->shared_limit[itm];
    return BCM_E_NONE;
}


/*
 * This function is used to set headroom pool
 * for given Ingress [Port, Priority_group]
 */
int
_bcm_th3_cosq_ingress_hdrm_pool_set_by_pg(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
                            int arg)
{
    int local_port;
    uint64 rval;
    int profile_index;
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, local_port, -1, &rval));

    profile_index = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval, PG_PROFILE_SELf);

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, &rval));
    soc_reg64_field32_set(unit, MMU_THDI_PG_PROFILEr, &rval, prigroup_hpid_field[pri_grp], arg);
    SOC_IF_ERROR_RETURN
        (soc_reg64_set(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, rval));


    return BCM_E_NONE;
}


/*
 * This function is used to get shared pool
 * for given Ingress [Port, Priority_group]
 */
int
_bcm_th3_cosq_ingress_sp_get_by_pg(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
                            int *arg)
{
    int local_port;
    uint64 rval;
    int profile_index;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, local_port, -1, &rval));

    profile_index = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval, PG_PROFILE_SELf);

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, &rval));
    *arg = soc_reg64_field32_get(unit, MMU_THDI_PG_PROFILEr, rval, prigroup_spid_field[pri_grp]);


    return BCM_E_NONE;
}


/*
 * This function is used to get headroom pool
 * for given Ingress [Port, Priority_group]
 */
int
_bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(int unit, bcm_gport_t gport,
                             bcm_cos_queue_t pri_grp, int *arg)
{
    int local_port;
    uint64 rval;
    int profile_index;
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= SOC_MMU_CFG_PRI_GROUP_MAX)) {
        return BCM_E_PARAM;
    }

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, local_port, -1, &rval));

    profile_index = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval, PG_PROFILE_SELf);

    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN
        (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY, profile_index, &rval));
    *arg = soc_reg64_field32_get(unit, MMU_THDI_PG_PROFILEr, rval, prigroup_hpid_field[pri_grp]);


    return BCM_E_NONE;
}


int
_bcm_th3_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_buffer_id_t buffer,
                          bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    int startq, pool;
    int pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        } else  { /* local port */
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
            } else if (type == bcmCosqControlMCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
            } else {
                if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                } else {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                }
            }
        }
        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, SPIDf);

        return BCM_E_NONE;
    }

    /* per service pool settings */
    if (type == bcmCosqControlEgressPoolSharedLimitBytes ||
        type == bcmCosqControlEgressPoolResumeLimitBytes ||
        type == bcmCosqControlEgressPoolYellowSharedLimitBytes ||
        type == bcmCosqControlEgressPoolYellowResumeLimitBytes ||
        type == bcmCosqControlEgressPoolRedSharedLimitBytes ||
        type == bcmCosqControlEgressPoolRedResumeLimitBytes ||
        type == bcmCosqControlEgressPoolHighCongestionLimitBytes ||
        type == bcmCosqControlEgressPoolLowCongestionLimitBytes) {
        int itm;
        uint64 rval;
        soc_reg_t reg;
        soc_reg_t wred_cng_lmt[4] = {
            MMU_WRED_POOL_INST_CONG_LIMIT_0r,
            MMU_WRED_POOL_INST_CONG_LIMIT_1r,
            MMU_WRED_POOL_INST_CONG_LIMIT_2r,
            MMU_WRED_POOL_INST_CONG_LIMIT_3r,
        };

        switch (type) {
            case bcmCosqControlEgressPoolSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
                fld_limit = SHARED_LIMITf;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
                fld_limit = RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolYellowSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
                fld_limit = YELLOW_SHARED_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolYellowResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
                fld_limit = YELLOW_RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolRedSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
                fld_limit = RED_SHARED_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolRedResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
                fld_limit = RED_RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolHighCongestionLimitBytes:
                reg = wred_cng_lmt[pool];
                fld_limit = POOL_HI_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolLowCongestionLimitBytes:
                reg = wred_cng_lmt[pool];
                fld_limit = POOL_LOW_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            /* coverity[dead_error_begin] */
            default:
                return BCM_E_UNAVAIL;
        }
        /* These regs are chip unique, buffer = -1 writes on both itms
           No need to do pipe-itm checking */
        if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
            itm = -1;
        } else if (buffer <  NUM_ITM(unit)) {
            itm = buffer;
        } else {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(soc_tomahawk3_itm_reg_get
            (unit, reg, itm, itm, pool, &rval));

        *arg = soc_reg64_field32_get(unit, reg, rval, fld_limit);
        *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;

        return BCM_E_NONE;
    }

    /* per port settings: */

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            }
        }
        mem = MMU_THDO_QUEUE_CONFIGm;
        fld_limit = LIMIT_ENABLEf;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else {
        if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
    }
    mem = MMU_THDO_QUEUE_CONFIGm;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
        case bcmCosqControlEgressPoolLimitBytes:
            fld_limit = SHARED_LIMITf;
            granularity = 1;
            break;
        case bcmCosqControlEgressPoolYellowLimitBytes:
            fld_limit = LIMIT_YELLOWf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedLimitBytes:
            fld_limit = LIMIT_REDf;
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;


    return BCM_E_NONE;


}

int
_bcm_th3_cosq_egr_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_buffer_id_t buffer,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, pool;
    int midx;
    int pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_mem_t mmu_thdo_per_queue_mem[] = {
        MMU_THDO_QUEUE_CONFIGm,
        MMU_THDO_Q_TO_QGRP_MAPD0m
    };
    int mmu_thdo_per_queue_mem_cnt = 2;
    soc_field_t field_qspid[12] = {
            QUEUE0_SPIDf, QUEUE1_SPIDf, QUEUE2_SPIDf,
            QUEUE3_SPIDf, QUEUE4_SPIDf, QUEUE5_SPIDf,
            QUEUE6_SPIDf, QUEUE7_SPIDf, QUEUE8_SPIDf,
            QUEUE9_SPIDf, QUEUE10_SPIDf, QUEUE11_SPIDf
    };
    soc_field_t fld_limit = INVALIDf;
    int idx;
    int granularity = 1;
    uint32 max_val;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        } else  { /* local port */
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
            } else if (type == bcmCosqControlMCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
            } else {
                if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                } else {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                }
            }
        }

        for (idx = 0; idx < mmu_thdo_per_queue_mem_cnt; idx++) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mmu_thdo_per_queue_mem[idx], MEM_BLOCK_ALL, startq, entry));
            soc_mem_field32_set(unit, mmu_thdo_per_queue_mem[idx], &entry, SPIDf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdo_per_queue_mem[idx], MEM_BLOCK_ALL, startq, entry));
        }

        mem = MMU_THDO_PORT_QUEUE_SERVICE_POOLm;
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                     MMU_THDO_PORT_QUEUE_SERVICE_POOLm, 0);
        /* CMIC has 48 queues... so not applicalble for cmic port */
        if (!IS_CPU_PORT(unit, local_port)) {
            int cosq_idx;
            if (cosq == -1) {
                cosq_idx = 0;
            } else {
                cosq_idx = cosq;
            }
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx,
                            entry));
            soc_mem_field32_set(unit, mem,
                    entry, field_qspid[cosq_idx],
                    arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx,
                            entry));
        }

        return BCM_E_NONE;
    }

    /* per service pool settings */
    if (type == bcmCosqControlEgressPoolSharedLimitBytes ||
        type == bcmCosqControlEgressPoolResumeLimitBytes ||
        type == bcmCosqControlEgressPoolYellowSharedLimitBytes ||
        type == bcmCosqControlEgressPoolYellowResumeLimitBytes ||
        type == bcmCosqControlEgressPoolRedSharedLimitBytes ||
        type == bcmCosqControlEgressPoolRedResumeLimitBytes ||
        type == bcmCosqControlEgressPoolHighCongestionLimitBytes ||
        type == bcmCosqControlEgressPoolLowCongestionLimitBytes) {
        int itm;
        uint64 rval;
        soc_reg_t reg;
        soc_reg_t wred_cng_lmt[4] = {
            MMU_WRED_POOL_INST_CONG_LIMIT_0r,
            MMU_WRED_POOL_INST_CONG_LIMIT_1r,
            MMU_WRED_POOL_INST_CONG_LIMIT_2r,
            MMU_WRED_POOL_INST_CONG_LIMIT_3r,
        };
        arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;

        switch (type) {
            case bcmCosqControlEgressPoolSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
                fld_limit = SHARED_LIMITf;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
                fld_limit = RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolYellowSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
                fld_limit = YELLOW_SHARED_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolYellowResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
                fld_limit = YELLOW_RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolRedSharedLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
                fld_limit = RED_SHARED_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolRedResumeLimitBytes:
                reg = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
                fld_limit = RED_RESUME_LIMITf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPoolHighCongestionLimitBytes:
                reg = wred_cng_lmt[pool];
                fld_limit = POOL_HI_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolLowCongestionLimitBytes:
                reg = wred_cng_lmt[pool];
                fld_limit = POOL_LOW_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            /* coverity[dead_error_begin] */
            default:
                return BCM_E_UNAVAIL;
        }
        /* These regs are chip unique, buffer = -1 writes on both itms
           No need to do pipe-itm checking */

        if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
            itm = -1;
        } else if (buffer <  NUM_ITM(unit)) {
            itm = buffer;
        } else {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(soc_tomahawk3_itm_reg_get
            (unit, reg, itm, itm, pool, &rval));

        /* Check for min/max values */
        max_val = (1 << soc_reg_field_length(unit, reg, fld_limit)) - 1;
        if ((arg < 0) || ((arg/granularity) > max_val)) {
            return BCM_E_PARAM;
        } else {
            soc_reg64_field32_set(unit, reg, &rval, fld_limit, (arg/granularity));
        }

        SOC_IF_ERROR_RETURN(soc_tomahawk3_itm_reg_set
            (unit, reg, itm, itm, pool, rval));

        return BCM_E_NONE;
    }

    /* per port settings: Why is there under Pool */
    if (type == bcmCosqControlEgressPoolLimitEnable) {
        int limit;
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            }
        }
        mem = MMU_THDO_QUEUE_CONFIGm;
        fld_limit = LIMIT_ENABLEf;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        /* What if it is dynamic shared limit */
        limit = soc_mem_field32_get(unit, mem, entry, SHARED_LIMITf);
        if (limit == 0)
            return BCM_E_PARAM;
        soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        return BCM_E_NONE;
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
    } else {
        if (IS_CPU_PORT(unit, local_port) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        }
    }

    mem = MMU_THDO_QUEUE_CONFIGm;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
        case bcmCosqControlEgressPoolLimitBytes:
            fld_limit = SHARED_LIMITf;
            soc_mem_field32_set(unit, mem, entry, LIMIT_ENABLEf, 1);
            granularity = 1;
            break;
        case bcmCosqControlEgressPoolYellowLimitBytes:
            fld_limit = LIMIT_YELLOWf;
            granularity = 8;
            break;
        case bcmCosqControlEgressPoolRedLimitBytes:
            fld_limit = LIMIT_REDf;
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }
    /* Check for min/max values */
    arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    return BCM_E_NONE;


}

/* Get pipe for a given port */
int
_bcm_th3_cosq_pipe_get(int unit, bcm_gport_t gport, int *pipe)
{
    bcm_port_t local_port;

    if (pipe) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, pipe));
    }
    return BCM_E_NONE;
}

int
_bcm_th3_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int pipe, port_pg;
    int pool, granularity = 1;
    soc_field_t field_pgmin_limit[_TH3_MMU_NUM_PG] = {
            PG0_MIN_LIMITf, PG1_MIN_LIMITf, PG2_MIN_LIMITf,
            PG3_MIN_LIMITf, PG4_MIN_LIMITf, PG5_MIN_LIMITf,
            PG6_MIN_LIMITf, PG7_MIN_LIMITf};
    /* soc_field_t field_use_portsp[_TH3_MMU_NUM_PG] = {
            PG0_USE_PORTSP_MINf, PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf,
            PG3_USE_PORTSP_MINf, PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf,
            PG6_USE_PORTSP_MINf, PG7_USE_PORTSP_MINf}; */
    soc_field_t field_pghdrm_limit[_TH3_MMU_NUM_PG] = {
            PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
            PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
            PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    soc_field_t field_pgreset_floor[_TH3_MMU_NUM_PG] = {
            PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf,
            PG3_RESET_FLOORf, PG4_RESET_FLOORf, PG5_RESET_FLOORf,
            PG6_RESET_FLOORf, PG7_RESET_FLOORf};
    soc_field_t field_sp_min[_TH3_MMU_NUM_POOL] = {
            PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf,
            PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};
    soc_field_t field_sp_shared[_TH3_MMU_NUM_POOL] = {
            PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf,
            PORTSP2_SHARED_LIMITf, PORTSP3_SHARED_LIMITf};
    soc_field_t field_sp_resume[_TH3_MMU_NUM_POOL] = {
            PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf,
            PORTSP2_RESUME_LIMITf, PORTSP3_RESUME_LIMITf};
    soc_field_t field_pgreset_offset[_TH3_MMU_NUM_PG] = {
            PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf,
            PG3_RESET_OFFSETf, PG4_RESET_OFFSETf, PG5_RESET_OFFSETf,
            PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }
    port_pg = cosq;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_pipe_get(unit, gport, &pipe));

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_ingress_sp_get_by_pg(unit, gport, port_pg, &pool));

    sal_memset(&entry, 0, sizeof(entry));
    if (type == bcmCosqControlIngressPortPGSharedLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_SHARED_CONFIGm, 0);
    } else if (type == bcmCosqControlIngressPortPGMinLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_MIN_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_MIN_CONFIGm, 0);
    } else if (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_HDRM_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_HDRM_CONFIGm, 0);
    } else if ((type == bcmCosqControlIngressPortPGResetFloorBytes)||
               (type == bcmCosqControlIngressPortPGResetOffsetBytes)) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_RESUME_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_RESUME_CONFIGm, 0);
    } else if ((type == bcmCosqControlIngressPortPoolMinLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolSharedLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolResumeLimitBytes)) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORTSP_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, MMU_THDI_PORTSP_CONFIGm, 0);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (midx < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlIngressPortPGSharedLimitBytes:
            fld_limit = field_pgshared_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGMinLimitBytes:
            fld_limit = field_pgmin_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolMinLimitBytes:
            fld_limit = field_sp_min[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolSharedLimitBytes:
            fld_limit = field_sp_shared[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolResumeLimitBytes:
            fld_limit = field_sp_resume[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
            fld_limit = field_pghdrm_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGResetFloorBytes:
            fld_limit = field_pgreset_floor[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGResetOffsetBytes:
            fld_limit = field_pgreset_offset[port_pg];
            granularity = 1;
            break;
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally
         * as a defensive default for future development.
         */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}


int
_bcm_th3_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int pipe, port_pg;
    int pool, granularity = 1;
    int shd_size[_TH3_ITMS_PER_DEV], cur_val;
    soc_field_t field_pgmin_limit[_TH3_MMU_NUM_PG] = {
            PG0_MIN_LIMITf, PG1_MIN_LIMITf, PG2_MIN_LIMITf,
            PG3_MIN_LIMITf, PG4_MIN_LIMITf, PG5_MIN_LIMITf,
            PG6_MIN_LIMITf, PG7_MIN_LIMITf};
    /* soc_field_t field_use_portsp[_TH3_MMU_NUM_PG] = {
            PG0_USE_PORTSP_MINf, PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf,
            PG3_USE_PORTSP_MINf, PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf,
            PG6_USE_PORTSP_MINf, PG7_USE_PORTSP_MINf}; */
    soc_field_t field_pghdrm_limit[_TH3_MMU_NUM_PG] = {
            PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
            PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
            PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    soc_field_t field_pgreset_floor[_TH3_MMU_NUM_PG] = {
            PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf,
            PG3_RESET_FLOORf, PG4_RESET_FLOORf, PG5_RESET_FLOORf,
            PG6_RESET_FLOORf, PG7_RESET_FLOORf};
    soc_field_t field_sp_min[_TH3_MMU_NUM_POOL] = {
            PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf,
            PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};
    soc_field_t field_sp_shared[_TH3_MMU_NUM_POOL] = {
            PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf,
            PORTSP2_SHARED_LIMITf, PORTSP3_SHARED_LIMITf};
    soc_field_t field_sp_resume[_TH3_MMU_NUM_POOL] = {
            PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf,
            PORTSP2_RESUME_LIMITf, PORTSP3_RESUME_LIMITf};
    soc_field_t field_pgreset_offset[_TH3_MMU_NUM_PG] = {
            PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf,
            PG3_RESET_OFFSETf, PG4_RESET_OFFSETf, PG5_RESET_OFFSETf,
            PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};


    if (arg < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    
    if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }
    port_pg = cosq;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_pipe_get(unit, gport, &pipe));

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_ingress_sp_get_by_pg(unit, gport, port_pg, &pool));

    sal_memset(&entry, 0, sizeof(entry));
    if (type == bcmCosqControlIngressPortPGSharedLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_SHARED_CONFIGm, 0);
    } else if (type == bcmCosqControlIngressPortPGMinLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_MIN_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_MIN_CONFIGm, 0);
    } else if (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_HDRM_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_HDRM_CONFIGm, 0);
    } else if ((type == bcmCosqControlIngressPortPGResetFloorBytes) ||
               (type == bcmCosqControlIngressPortPGResetOffsetBytes)) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_RESUME_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                                MMU_THDI_PORT_PG_RESUME_CONFIGm, 0);
    } else if ((type == bcmCosqControlIngressPortPoolMinLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolSharedLimitBytes) ||
               (type == bcmCosqControlIngressPortPoolResumeLimitBytes)) {
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORTSP_CONFIGm)[pipe];
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, MMU_THDI_PORTSP_CONFIGm, 0);
    }
    else {
        return BCM_E_UNAVAIL;
    }

    if (midx < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlIngressPortPGSharedLimitBytes:
            fld_limit = field_pgshared_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGMinLimitBytes:
            fld_limit = field_pgmin_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolMinLimitBytes:
            fld_limit = field_sp_min[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolSharedLimitBytes:
            fld_limit = field_sp_shared[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPoolResumeLimitBytes:
            fld_limit = field_sp_resume[pool];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
            fld_limit = field_pghdrm_limit[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGResetFloorBytes:
            fld_limit = field_pgreset_floor[port_pg];
            granularity = 1;
            break;
        case bcmCosqControlIngressPortPGResetOffsetBytes:
            fld_limit = field_pgreset_offset[port_pg];
            granularity = 1;
            break;
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally
         * as a defensive default for future development.
         */
        /* coverity[dead_error_begin] */
        default:
            return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    }

    /* guarantee limits need adjust shared limits */
    if ((type == bcmCosqControlIngressPortPGMinLimitBytes) ||
        (type == bcmCosqControlIngressPortPGHeadroomLimitBytes)) {
        int itm, itm_map;
        int delta[_TH3_ITMS_PER_DEV] = {0}, update;

        cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);
        sal_memcpy(shd_size, _BCM_TH3_MMU_INFO(unit)->shared_limit,
            sizeof(shd_size));

        /* adjustment delta */
        itm_map = SOC_INFO(unit).ipipe_itm_map[pipe];
        for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm++) {
            if (itm_map & (1 << itm)) {
                delta[itm] = ((arg / granularity) - cur_val) * granularity;
            }
        }

        /* shared limits to be decreased */
        for (itm = 0, update = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
            if ((itm_map & (1 << itm)) && (delta[itm] > 0)) {
                /* no space to decrease shared limit */
                if (delta[itm] > shd_size[itm]) {
                    return BCM_E_RESOURCE;
                }
                shd_size[itm] -= delta[itm];
                update = 1;
            }
        }
        if (update) {
            /* Decrease shared limits */
            BCM_IF_ERROR_RETURN
                (soc_th3_mmu_config_res_limits_update(unit, delta, pool, 1));
        }

        /* set new guarantee */
        /* the hw value may be changed by soc_th3_mmu_config_res_limits_update
         * so read the register again in order to keep consistent  */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));

        /* shared limits to be increased */
        for (itm = 0, update = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
            if ((itm_map & (1 << itm)) && (delta[itm] < 0)) {
                shd_size[itm] -= delta[itm];
                update = 1;
            }
        }
        if (update) {
            /* Increase shared limits */
            BCM_IF_ERROR_RETURN
                (soc_th3_mmu_config_res_limits_update(unit, delta, pool, 0));
        }
        sal_memcpy(_BCM_TH3_MMU_INFO(unit)->shared_limit, shd_size,
            sizeof(shd_size));

        /* In the case of PG Headroom limit, update headroom pool limit */
        if (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) {
            uint32 rval = 0;
            int cur_hdrm = 0;
            for (itm = 0, update = 0; itm < _TH3_ITMS_PER_DEV; itm ++) {
                if (itm_map & (1 << itm)) {
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk3_itm_reg32_get(unit,
                            MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr, itm, itm, pool, &rval));
                    cur_hdrm = soc_reg_field_get(unit, MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                             rval, LIMITf);
                    cur_hdrm += delta[itm];
                    if (cur_hdrm < 0) {
                        return BCM_E_PARAM;
                    }
                    soc_reg_field_set(unit, MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr,
                                             &rval, LIMITf, cur_hdrm);
                    SOC_IF_ERROR_RETURN(
                        soc_tomahawk3_itm_reg32_set(unit,
                            MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr, itm, itm, pool, rval));
                }
            }
        }
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}

int
_bcm_th3_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    /* int pipe;  */
    int startq;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;
    int shd_size[_TH3_ITMS_PER_DEV];
    int cur_val;
    int limit;
    int disable_queuing = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueResumeOffsetBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            /* SOC_IF_ERROR_RETURN
                (soc_port_pipe_get(unit, local_port, &pipe)); */
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
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            /* SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe)); */
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        if (type == bcmCosqControlEgressUCQueueResumeOffsetBytes) {
            mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
        } else {
            mem = MMU_THDO_QUEUE_CONFIGm;
        }
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueResumeOffsetBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            /* SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));*/
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            /* SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe)); */
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        if (type == bcmCosqControlEgressMCQueueResumeOffsetBytes) {
            mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
        } else {
            mem = MMU_THDO_QUEUE_CONFIGm;
        }
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
            fld_limit = SHARED_LIMITf;

            /* Set MMU_THDO_QUEUE_CONFIG.DISABLE_QUEUING to 1 when both MIN
             * and SHARED LIMIT are 0. Set it to 0 when either MIN or SHARED LIMIT
             * is non-zero.
             */
            limit = soc_mem_field32_get(unit, mem, entry, MIN_LIMITf);
            if (arg == 0 && limit == 0) {
                disable_queuing = 1;
            } else if (arg != 0 || limit != 0) {
                disable_queuing = 0;
            }
            granularity = 1;
            break;
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
            fld_limit = MIN_LIMITf;

            /* Set MMU_THDO_QUEUE_CONFIG.DISABLE_QUEUING to 1 when both MIN
             * and SHARED LIMIT are 0. Set it to 0 when either MIN or
             * SHARED LIMIT is non-zero
             */
            limit = soc_mem_field32_get(unit, mem, entry, SHARED_LIMITf);
            if (arg == 0 && limit == 0) {
                disable_queuing = 1;
            } else if (arg != 0 || limit != 0) {
                disable_queuing = 0;
            }

            granularity = 1;
            break;
        case bcmCosqControlEgressUCQueueResumeOffsetBytes:
        case bcmCosqControlEgressMCQueueResumeOffsetBytes:
            fld_limit = RESUME_OFFSETf;
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    }

    /* guarantee limits need adjust shared limits */
    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
         (type == bcmCosqControlEgressMCQueueMinLimitBytes)) {
        int itm, itm_map;
        int delta[_TH3_ITMS_PER_DEV] = {0}, update;
        int pool;
        itm_map = SOC_INFO(unit).itm_map;

        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_egr_pool_get(unit, gport, 
            cosq, -1, bcmCosqControlEgressPool,&pool));

        cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);
        sal_memcpy(shd_size, _BCM_TH3_MMU_INFO(unit)->shared_limit,
            sizeof(shd_size));

        /* adjustment delta */
        for (itm = 0; itm < NUM_ITM(unit); itm ++) {
            if (itm_map & (1 << itm)) {
                delta[itm] = ((arg / granularity) - cur_val) * granularity;
            }
        }

        /* shared limits to be decreased */
        for (itm = 0, update = 0; itm < NUM_ITM(unit); itm ++) {
            if (itm_map & (1 << itm)) {
                if (delta[itm] > 0) {
                    /* no space to decrease shared limit */
                    if (delta[itm] > shd_size[itm]) {
                        return BCM_E_RESOURCE;
                    }
                    shd_size[itm] -= delta[itm];
                    update = 1;
                }
            }
        }
        if (update) {
            BCM_IF_ERROR_RETURN
                (soc_th3_mmu_config_res_limits_update(unit, delta, pool, 1));
        }

        /* set new guarantee */
        /* the hw value may be changed by soc_th3_mmu_config_res_limits_update
         * so read the register again in order to keep consistent  */
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

        /* shared limits to be increased */
        for (itm = 0, update = 0; itm < NUM_ITM(unit); itm ++) {
            if (itm_map & (1 << itm)) {
                if (delta[itm] < 0) {
                    shd_size[itm] -= delta[itm];
                    update = 1;
                }
            }
        }
        if (update) {
            BCM_IF_ERROR_RETURN
                (soc_th3_mmu_config_res_limits_update(unit, delta, pool, 0));
        }

        sal_memcpy(_BCM_TH3_MMU_INFO(unit)->shared_limit, shd_size,
            sizeof(shd_size));
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    /* limit enable */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        soc_mem_field32_set(unit, mem, entry, DISABLE_QUEUINGf, disable_queuing);
    }

    if ((type == bcmCosqControlEgressUCQueueSharedLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        soc_mem_field32_set(unit, mem, entry, LIMIT_ENABLEf, 1);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }

    return BCM_E_NONE;
}



int
_bcm_th3_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    int pipe, startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;

    if (!arg) {
        return BCM_E_PARAM;
    }


    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueResumeOffsetBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            SOC_IF_ERROR_RETURN
                (soc_port_pipe_get(unit, local_port, &pipe));
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
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        if (type == bcmCosqControlEgressUCQueueResumeOffsetBytes) {
            mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
        } else {
            mem = MMU_THDO_QUEUE_CONFIGm;
        }
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueResumeOffsetBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve
                     (unit, local_port, ci, _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        if (type == bcmCosqControlEgressMCQueueResumeOffsetBytes) {
            mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
        } else {
            mem = MMU_THDO_QUEUE_CONFIGm;
        }
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    switch (type) {
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
            fld_limit = SHARED_LIMITf;
            granularity = 1;
            break;
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
            fld_limit = MIN_LIMITf;
            granularity = 1;
            break;
        case bcmCosqControlEgressUCQueueResumeOffsetBytes:
        case bcmCosqControlEgressMCQueueResumeOffsetBytes:
            fld_limit = RESUME_OFFSETf;
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * This function is used to get enables
 * for Qgroup Min and Shared Limits for a given Egress Unicast Queue
 */
int
_bcm_th3_cosq_qgroup_limit_enables_get(int unit, bcm_gport_t gport,
                               bcm_cos_t cosq, bcm_cosq_control_t type,
                               int* arg)
{
    bcm_port_t local_port;
    int startq=0, pipe=0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                        &local_port, &startq, NULL));
        } else {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        if (type == bcmCosqControlEgressMCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                        &local_port, &startq, NULL));
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (cosq < 0) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  NULL, &startq, NULL));
        }
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
    mem = MMU_THDO_QUEUE_CONFIGm;
    if ((type == bcmCosqControlEgressUCQueueGroupMinEnable) ||
        (type == bcmCosqControlEgressMCQueueGroupMinEnable)) {
        field = USE_QGROUP_MINf;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    startq, entry));
    *arg = soc_mem_field32_get(unit, mem, entry, field);

    return BCM_E_NONE;
}

/*
 * This function is used to set enables
 * for Qgroup Min and Shared Limits for a given Egress Unicast Queue
 */
int
_bcm_th3_cosq_qgroup_limit_enables_set(int unit, bcm_gport_t gport,
                               bcm_cos_t cosq, bcm_cosq_control_t type,
                               int arg)
{
    bcm_port_t local_port;
    int startq=0, count=1, lc=0, to_cos=0;
    uint32 entry[SOC_MAX_MEM_WORDS], entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_mem_t mmu_thdo_per_queue_mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
    int enable;
    int valid;
    soc_field_t field = INVALIDf;
    soc_field_t field2 = INVALIDf;

    if (arg < 0) {
        return BCM_E_PARAM;
    }
    arg = arg ? 1 : 0;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                        &local_port, &startq, NULL));
        } else {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        if (type == bcmCosqControlEgressMCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                        &local_port, &startq, NULL));
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, &count));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  NULL, &startq, &count));
        }
    }

    mem = MMU_THDO_QUEUE_CONFIGm;
    if ((type == bcmCosqControlEgressUCQueueGroupMinEnable) ||
        (type == bcmCosqControlEgressMCQueueGroupMinEnable)) {
        field = USE_QGROUP_MINf;
        field2 = QGROUP_VALIDf;
    } else {
        return BCM_E_PARAM;
    }

    to_cos = startq + count;
    for (lc = startq; lc < to_cos; lc++) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    lc, entry));
        enable = soc_mem_field32_get(unit, mem, entry, field);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, field, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                        lc, entry));
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mmu_thdo_per_queue_mem,
                    MEM_BLOCK_ALL, lc, entry2));
        valid = soc_mem_field32_get(unit, mmu_thdo_per_queue_mem, entry2, field2);
        if (valid != arg) {
            soc_mem_field32_set(unit, mmu_thdo_per_queue_mem, entry2, field2, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdo_per_queue_mem,
                        MEM_BLOCK_ALL, lc, entry2));
        }
    }

    /*If qgroup min limit enable is being set,
     buffer shared limits will not be recalculated. It is the responsibility
     of the user to set the QueueMinLimitBytes to 0
     */

    return BCM_E_NONE;
}

/*
 * This function is used to get limits
 * for Qgroup Min and Shared Limits for a given Egress Unicast Queue
 */
int
_bcm_th3_cosq_qgroup_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    int pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;
    int granularity;

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) &&
                (type != bcmCosqControlEgressUCQueueGroupMinLimitBytes)) {
                return BCM_E_PARAM;
            }
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) &&
                (type != bcmCosqControlEgressMCQueueGroupMinLimitBytes)) {
                return BCM_E_PARAM;
            }
        } else { /* GPORT is local or modport type */
            if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) &&
                (cosq >= _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
            if ((type == bcmCosqControlEgressMCQueueGroupMinLimitBytes) &&
                (cosq < _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
        }
    } else { /* local port case */
            if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) &&
                (cosq >= _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
            if ((type == bcmCosqControlEgressMCQueueGroupMinLimitBytes) &&
                (cosq < _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueGroupMinLimitBytes)) {
        if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
            mem = MMU_THDO_CONFIG_UC_QGROUP0m;
        }
        else {
            mem = MMU_THDO_CONFIG_MC_QGROUPm;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                SOC_TH3_MMU_CHIP_PORT(unit, local_port), entry));
        field = MIN_LIMITf;
        granularity =1;
    } else {
        return BCM_E_PARAM;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, field);
    *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * This function is used to set limits
 * for Qgroup Min and Shared Limits for a given Egress Unicast Queue
 */
int
_bcm_th3_cosq_qgroup_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;
    int granularity;
    int shd_size[_TH3_ITMS_PER_DEV], cur_val;
    int rv, itm;
    int delta[_TH3_ITMS_PER_DEV] = {0};
    int max_val;

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) &&
                (type != bcmCosqControlEgressUCQueueGroupMinLimitBytes)) {
                return BCM_E_PARAM;
            }
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) &&
                (type != bcmCosqControlEgressMCQueueGroupMinLimitBytes)) {
                return BCM_E_PARAM;
            }
        } else { /* GPORT is local or modport type */
            if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) &&
                (cosq >= _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
            if ((type == bcmCosqControlEgressMCQueueGroupMinLimitBytes) &&
                (cosq < _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
        }
    } else { /* local port case */
            if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) &&
                (cosq >= _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
            if ((type == bcmCosqControlEgressMCQueueGroupMinLimitBytes) &&
                (cosq < _bcm_th3_get_num_ucq(unit) )) {
                return BCM_E_PARAM;
            }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueGroupMinLimitBytes)) {
        arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if ((type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) ||
        (type == bcmCosqControlEgressMCQueueGroupMinLimitBytes)) {
        int pool;
        if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
            mem = MMU_THDO_CONFIG_UC_QGROUP0m;
        }
        else {
            mem = MMU_THDO_CONFIG_MC_QGROUPm;
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                SOC_TH3_MMU_CHIP_PORT(unit, local_port), entry));
        field = MIN_LIMITf;
        granularity =1;
        /* Recalculate Shared Values if Min Changed */
        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_egr_pool_get(unit, gport, 
            cosq, -1, bcmCosqControlEgressPool,&pool));
        sal_memcpy(shd_size, _BCM_TH3_MMU_INFO(unit)->shared_limit,
             sizeof(shd_size));

        cur_val = soc_mem_field32_get(unit, mem, entry, field);
        /* adjustment delta */
        for (itm = 0; itm < NUM_ITM(unit); itm ++) {
            delta[itm] = ((arg / granularity) - cur_val) * granularity;
        }
        /* shared limits to be increased/decreased */
        for (itm = 0; itm < NUM_ITM(unit); itm ++) {
            if (delta[itm] > 0) {
                /* no space to decrease shared limit */
                if (delta[itm] > shd_size[itm]) {
                    return BCM_E_RESOURCE;
                }
                shd_size[itm] -= delta[itm];
            }
            if (delta[itm] < 0) {
                shd_size[itm] -= delta[itm];
            }
        }

        rv = soc_th3_mmu_config_res_limits_update(unit, delta, pool, 1);

        if (rv < 0) {
            return rv;
        }

        sal_memcpy(_BCM_TH3_MMU_INFO(unit)->shared_limit,shd_size,
             sizeof(shd_size));
    } else {
        return BCM_E_PARAM;
    }

    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, field)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, field, (arg/granularity));
    }
    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                   SOC_TH3_MMU_CHIP_PORT(unit, local_port), entry));
    } else {
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                    SOC_TH3_MMU_CHIP_PORT(unit, local_port), entry));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_traverse_by_port
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
bcm_th3_cosq_gport_traverse_by_port(int unit, bcm_gport_t gport,
                                    bcm_cosq_gport_traverse_cb cb,
                                    void *user_data)
{
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    bcm_port_t local_port, port_out;
    int id, rv = SOC_E_NONE;
    bcm_module_t my_modid, modid_out;
    bcm_gport_t gport_out;

    if (_bcm_th3_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_th3_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                 my_modid, gport, &modid_out, &port_out));
    BCM_GPORT_MODPORT_SET(gport_out, modid_out, port_out);

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (IS_CPU_PORT(unit, local_port)) {
        cpu_port_info = &_bcm_th3_cosq_cpu_port_info[unit][local_port];
    } else {
        port_info = &_bcm_th3_cosq_port_info[unit][local_port];
    }
    if (!IS_CPU_PORT(unit, local_port)) {
        for (id = 0; id < _bcm_th3_get_num_ucq(unit); id++) {
            if (port_info->ucast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport_out, port_info->ucast[id].numq,
                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                    port_info->ucast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
           /*
            * If CB_ABORT_ON_ERR is not defined don't return error
            * and continue.
            */
           rv = BCM_E_NONE;
#endif
        }
    }
    if (IS_CPU_PORT(unit, local_port)) {
        for (id = 0; id < _BCM_TH3_NUM_CPU_MCAST_QUEUE; id++) {
            if (cpu_port_info->mcast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport_out, cpu_port_info->mcast[id].numq,
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                    cpu_port_info->mcast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            /*
             * If CB_ABORT_ON_ERR is not defined don't return error
             * and continue.
             */
            rv = BCM_E_NONE;
#endif
        }
    } else {
        for (id = 0; id < _bcm_th3_get_num_mcq(unit); id++) {
            if (port_info->mcast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport_out, port_info->mcast[id].numq,
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                    port_info->mcast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            /*
             * If CB_ABORT_ON_ERR is not defined don't return error
             * and continue.
             */
            rv = BCM_E_NONE;
#endif
        }
    }
    for (id = 0; id < _BCM_TH3_NUM_SCHEDULER_PER_PORT; id++) {
        if (IS_CPU_PORT(unit, local_port)) {
            if (cpu_port_info->sched[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport_out, cpu_port_info->sched[id].numq,
                    BCM_COSQ_GPORT_SCHEDULER,
                    cpu_port_info->sched[id].gport, user_data);
        } else {
            if (port_info->sched[id].numq == 0 ||
                port_info->sched[id].in_use == 0) {
                continue;
            }
            rv = cb(unit, gport_out, port_info->sched[id].numq,
                    BCM_COSQ_GPORT_SCHEDULER,
                    port_info->sched[id].gport, user_data);
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        /*
         * If CB_ABORT_ON_ERR is not defined don't return error
         * and continue.
         */
        rv = BCM_E_NONE;
#endif
    }
    return rv;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_traverse
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
bcm_th3_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    bcm_port_t port;

    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_traverse_by_port(unit, port, cb, user_data));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_attach_get
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
bcm_th3_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_th3_cosq_node_t *node = NULL;
    int cpu_mc_base = 0;
    bcm_port_t local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);

    if (!(BCM_GPORT_IS_SET(sched_gport) ||
          BCM_GPORT_IS_SCHEDULER(sched_gport) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_node_get(unit, sched_gport, NULL, &local_port, NULL,
                               &node));
    if (node == NULL) {
        return BCM_E_PARAM;
    }

    if (local_port < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        if (IS_CPU_PORT(unit, local_port)) {
            *cosq = (node->hw_index - cpu_mc_base) % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
        } else {
            *cosq = node->cos % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport)) {
        *cosq = node->cos % _BCM_TH3_MAX_QUEUE_PER_PORT;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport)) {
        if (IS_CPU_PORT(unit, local_port)) {
            cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
            *cosq = (node->hw_index - cpu_mc_base) % _BCM_TH3_NUM_CPU_MCAST_QUEUE;
        } else {
            *cosq = node->cos % _BCM_TH3_MAX_QUEUE_PER_PORT;
        }
    } else {
        return BCM_E_PARAM;
    }
    *input_gport = node->parent_gport;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th3_cosq_gport_cpu_tree_create
 * Purpose:
 *      During cosq init create the default cosq gport tree for CPU.
 *      Schedualar (L0) nodes are created and attach to physical ports.
 *      UC and MC queues are added and attached to each L0 nodes.
 * Parameters:
 *      unit - (IN) unit number.
 *      port - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_cosq_gport_cpu_tree_create(int unit, bcm_port_t port)
{
    bcm_gport_t cpu_mc_gport[_BCM_TH3_NUM_CPU_MCAST_QUEUE];
    int id, pci_cmc = SOC_PCI_CMC(unit);
    int ch_l0_map;
    bcm_gport_t sched_gport[_BCM_TH3_NUM_SCHEDULER_PER_PORT];
    int numq;
    int num_queues_uc0 = soc_property_get(unit, "num_queues_uc0", 0);
    int num_queues_uc1 = soc_property_get(unit, "num_queues_uc1", 0);

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PARAM;
    }
    /* CMIC requires all 48 queues to be set and for the respective CMC */
    if ((NUM_CPU_ARM_COSQ(unit, pci_cmc) + num_queues_uc0 + num_queues_uc1) !=
         SOC_TH3_NUM_CPU_QUEUES) {
        return BCM_E_PARAM;
    }


    numq = 48;
    /* Adding L0 (schedular) nodes and attaching them to cpu port */
    for (id = 0; id < _BCM_TH3_NUM_SCHEDULER_PER_PORT; id++) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_add(unit, port, numq, BCM_COSQ_GPORT_SCHEDULER,
                                   &sched_gport[id]));
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_attach(unit, sched_gport[id], port, id));
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,
            "sched_gport[%d]=0x%x\n"),
            id, sched_gport[id]));
    }
    /* Add MC queues and attach them to Schedular nodes.
     * L0.0 node - PCI host
     * L0.7 node - uc0
     * L0.8 node - uc1
     */

    /* CMICX supports only 2 CMCs.
     * CMC0 connected to PCI.
     * CMC1 is connected to other arm cores.
     */

    for (id = 0; id < _BCM_TH3_NUM_CPU_MCAST_QUEUE; id++) {
        ch_l0_map = (id < NUM_CPU_ARM_COSQ(unit, pci_cmc)) ? pci_cmc :
                    (id < (NUM_CPU_ARM_COSQ(unit, pci_cmc) +
                        (num_queues_uc0))) ? 7 : 8;
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_add(unit, 0, 1,
                                   BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                   &cpu_mc_gport[id]));
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_attach(unit, cpu_mc_gport[id],
                                      sched_gport[ch_l0_map], id));
    }


    return SOC_E_NONE;
}


/*
 * Function:
 *      _bcm_th3_cosq_gport_tree_create
 * Purpose:
 *      During cosq init create the default cosq gport tree.
 *      Schedualar (L0) nodes are created and attach to physical ports.
 *      UC and MC queues are added and attached to each L0 nodes.
 * Parameters:
 *      unit - (IN) unit number.
 *      port - (IN) port number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_cosq_gport_tree_create(int unit, bcm_port_t port)
{
    int id;

    if (IS_CPU_PORT(unit, port)) {
        _bcm_th3_cosq_gport_cpu_tree_create(unit, port);
    } else {
        /* Adding L0 (schedular) nodes and attaching them to front panel and cpu port */
        for (id = 0; id < _BCM_TH3_NUM_SCHEDULER_PER_PORT; id++) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_gport_add_attach(unit, port, id, BCM_COSQ_GPORT_SCHEDULER));
        }

        /* Adding UC/MC queues and attaching them to Schedular nodes */
        for (id = 0; id < _BCM_TH3_MAX_NUM_QUEUE_PER_PORT; id++) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_gport_add_attach(unit, port, id, 0));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_sched_weight_max_get
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
bcm_th3_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
         /* In TH the maximum weight will be 127 */
        *weight_max =
            (1 << soc_mem_field_length(unit, MMU_QSCH_L0_WEIGHT_MEMm,
                                       WEIGHTf)) - 1;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_th3_cosq_gport_get
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
bcm_th3_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_th3_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_module_t modid;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
         BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
         BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, gport, NULL, &local_port,
                                   NULL, &node));

        *numq = node->numq;

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            *flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
        } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
            *flags = BCM_COSQ_GPORT_SCHEDULER;
        } else {
            *flags = 0;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        *flags = 0;
        *numq = _BCM_TH3_NUM_SCHEDULER_PER_PORT;
    }

    if (SOC_USE_GPORT(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        dest.modid = modid;
        dest.port = local_port;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, port));
    } else {
        *port = local_port;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
        "port=0x%x numq=%d flags=0x%x\n"),
        *port, *numq, *flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_cosq_schedq_mapping_set
 * Purpose:
 *     Get a cosq gport structure
 * Parameters:
 *     unit             - (IN) unit number
 *     profile_index    - (IN) Profile index
 *     size             - (OUT) Array size
 *     cosq_mapping_arr - (OUT) Cosq mapping
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_schedq_mapping_set(int unit, int profile_index,
    int size, bcm_cosq_mapping_t *cosq_mapping_arr)
{
    _soc_mmu_cfg_scheduler_profile_t *sched_profile;
    int cosq_idx, cos;
    int L0[_BCM_TH3_COS_MAX];
    int schedq[_BCM_TH3_COS_MAX];
    int mmuq[_BCM_TH3_COS_MAX];
    int cos_list[_BCM_TH3_COS_MAX];
    int strict_priority[_BCM_TH3_COS_MAX];
    int fc_is_uc_only[_BCM_TH3_COS_MAX];
    bcm_port_t port;
    int rv = BCM_E_NONE;

    if (profile_index < 0 || profile_index >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return BCM_E_PARAM;
    }
    if (cosq_mapping_arr == NULL) {
        return BCM_E_PARAM;
    }
    if (size < 0 || size > _BCM_TH3_COS_MAX) {
        return BCM_E_PARAM;
    }

    sched_profile = soc_mmu_cfg_scheduler_profile_alloc(unit);

    for (cosq_idx = 0; cosq_idx < _BCM_TH3_COS_MAX; cosq_idx++) {
        sched_profile[profile_index].num_unicast_queue[cosq_idx] = 0;
        sched_profile[profile_index].num_multicast_queue[cosq_idx] = 0;
        sched_profile[profile_index].strict_priority[cosq_idx] = 0;
        sched_profile[profile_index].flow_control_only_unicast[cosq_idx] = 0;
    }

    for (cosq_idx = 0; cosq_idx < size; cosq_idx++) {
        cos = cosq_mapping_arr[cosq_idx].cosq;
        sched_profile[profile_index].num_unicast_queue[cos] = cosq_mapping_arr[cosq_idx].num_ucq;
        sched_profile[profile_index].num_multicast_queue[cos] = cosq_mapping_arr[cosq_idx].num_mcq;
        sched_profile[profile_index].strict_priority[cos] = cosq_mapping_arr[cosq_idx].strict_priority;
        sched_profile[profile_index].flow_control_only_unicast[cos] = cosq_mapping_arr[cosq_idx].fc_is_uc_only;
        sched_profile[profile_index].valid = 1;
    }
    rv = _soc_mmu_tomahawk3_scheduler_profile_config_check(unit,
                        profile_index, sched_profile);
    if (rv != BCM_E_NONE) {
        goto exit;
    }

    rv = _soc_scheduler_profile_mapping_setup(unit,
                        sched_profile, profile_index, L0, schedq, mmuq, cos_list,
                        strict_priority, fc_is_uc_only);
    if (rv != BCM_E_NONE) {
        goto exit;
    }

    /*initilization of L0.i->schedq.i->mmuq.i*/
    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit, "Profile %d cos %d L0 %d schedq %d mmuq %d cos_list %d\n"),
         profile_index, cosq_idx, L0[cosq_idx], schedq[cosq_idx], mmuq[cosq_idx], cos_list[cosq_idx]));
    rv = soc_tomahawk3_sched_update_init(unit, profile_index, L0, schedq, mmuq, cos_list, strict_priority, fc_is_uc_only);
    if (rv != BCM_E_NONE) {
        goto exit;
    }

    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = FALSE;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port)) {
            continue;
        }
        rv = _bcm_th3_cosq_gport_tree_create(unit, port);
        if (rv != BCM_E_NONE) {
            goto exit;
        }
    }
    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = TRUE;

exit:
    soc_mmu_cfg_scheduler_profile_free(unit, sched_profile);

    return rv;
}


/*
 * Function:
 *     bcm_tomahawk3_cosq_schedq_mapping_get
 * Purpose:
 *     Get a cosq gport structure
 * Parameters:
 *     unit             - (IN) unit number
 *     profile_index    - (IN) Profile index
 *     size             - (OUT) Array size
 *     cosq_mapping_arr - (OUT) Cosq mapping
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_schedq_mapping_get(int unit, int profile_index,
    int array_max, bcm_cosq_mapping_t *cosq_mapping_arr, int *size)
{
    int cosq_idx, qidx;

    if (profile_index < 0 || profile_index >= SOC_TH3_MAX_NUM_SCHED_PROFILE) {
        return BCM_E_PARAM;
    }
    if (array_max < 0 || array_max > _BCM_TH3_COS_MAX) {
        return BCM_E_PARAM;
    }
    if (cosq_mapping_arr == NULL) {
        return BCM_E_PARAM;
    }
    for (cosq_idx = 0; cosq_idx < array_max; cosq_idx++) {
        cosq_mapping_arr[cosq_idx].cosq = cosq_idx;
        cosq_mapping_arr[cosq_idx].num_ucq = -1;
        cosq_mapping_arr[cosq_idx].num_mcq = -1;
        cosq_mapping_arr[cosq_idx].strict_priority = 0;
        cosq_mapping_arr[cosq_idx].fc_is_uc_only = 0;
    }

    cosq_idx = 0;
    for (qidx = 0; qidx < _BCM_TH3_MAX_NUM_QUEUE_PER_PORT; qidx++) {
        if (cosq_idx >= array_max) {
            break;
        }
        if (th3_sched_profile_info[unit][profile_index][qidx].cos == -1) {
            continue;
        }
        cosq_mapping_arr[cosq_idx].cosq = th3_sched_profile_info[unit][profile_index][qidx].cos;
        if (th3_sched_profile_info[unit][profile_index][qidx].mmuq[0] != -1) {
            cosq_mapping_arr[cosq_idx].num_ucq = 0;
            cosq_mapping_arr[cosq_idx].num_mcq = 0;
            if (th3_sched_profile_info[unit][profile_index][qidx].mmuq[0] <
                _bcm_th3_get_num_ucq(unit)) {
                cosq_mapping_arr[cosq_idx].num_ucq++;
            } else {
                cosq_mapping_arr[cosq_idx].num_mcq++;
            }
        }
        if (th3_sched_profile_info[unit][profile_index][qidx].mmuq[1] != -1) {
            if (th3_sched_profile_info[unit][profile_index][qidx].mmuq[1] <
                _bcm_th3_get_num_ucq(unit)) {
                cosq_mapping_arr[cosq_idx].num_ucq++;
            } else {
                cosq_mapping_arr[cosq_idx].num_mcq++;
            }
        }
        cosq_mapping_arr[cosq_idx].strict_priority =
            th3_sched_profile_info[unit][profile_index][qidx].strict_priority;
        cosq_mapping_arr[cosq_idx].fc_is_uc_only =
            th3_sched_profile_info[unit][profile_index][qidx].fc_is_uc_only;
        cosq_idx++;
    }

    *size = cosq_idx;

    return BCM_E_NONE;
}

/* Registers accessed for IDB APIs */
static const soc_mem_t
    obm_pri_map_mem[_TH3_PBLKS_PER_PIPE][_TH3_PORTS_PER_PBLK] = {
     {IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
      IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
      IDB_OBM0_PRI_MAP_PORT4m, IDB_OBM0_PRI_MAP_PORT5m,
      IDB_OBM0_PRI_MAP_PORT6m, IDB_OBM0_PRI_MAP_PORT7m},
     {IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
      IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
      IDB_OBM1_PRI_MAP_PORT4m, IDB_OBM1_PRI_MAP_PORT5m,
      IDB_OBM1_PRI_MAP_PORT6m, IDB_OBM1_PRI_MAP_PORT7m},
     {IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
      IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
      IDB_OBM2_PRI_MAP_PORT4m, IDB_OBM2_PRI_MAP_PORT5m,
      IDB_OBM2_PRI_MAP_PORT6m, IDB_OBM2_PRI_MAP_PORT7m},
     {IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
      IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
      IDB_OBM3_PRI_MAP_PORT4m, IDB_OBM3_PRI_MAP_PORT5m,
      IDB_OBM3_PRI_MAP_PORT6m, IDB_OBM3_PRI_MAP_PORT7m},
    };
static const soc_mem_t
    obm_dscp_map_mem[_TH3_PBLKS_PER_PIPE][_TH3_PORTS_PER_PBLK] = {
     {IDB_OBM0_DSCP_MAP_PORT0m, IDB_OBM0_DSCP_MAP_PORT1m,
      IDB_OBM0_DSCP_MAP_PORT2m, IDB_OBM0_DSCP_MAP_PORT3m,
      IDB_OBM0_DSCP_MAP_PORT4m, IDB_OBM0_DSCP_MAP_PORT5m,
      IDB_OBM0_DSCP_MAP_PORT6m, IDB_OBM0_DSCP_MAP_PORT7m},
     {IDB_OBM1_DSCP_MAP_PORT0m, IDB_OBM1_DSCP_MAP_PORT1m,
      IDB_OBM1_DSCP_MAP_PORT2m, IDB_OBM1_DSCP_MAP_PORT3m,
      IDB_OBM1_DSCP_MAP_PORT4m, IDB_OBM1_DSCP_MAP_PORT5m,
      IDB_OBM1_DSCP_MAP_PORT6m, IDB_OBM1_DSCP_MAP_PORT7m},
     {IDB_OBM2_DSCP_MAP_PORT0m, IDB_OBM2_DSCP_MAP_PORT1m,
      IDB_OBM2_DSCP_MAP_PORT2m, IDB_OBM2_DSCP_MAP_PORT3m,
      IDB_OBM2_DSCP_MAP_PORT4m, IDB_OBM2_DSCP_MAP_PORT5m,
      IDB_OBM2_DSCP_MAP_PORT6m, IDB_OBM2_DSCP_MAP_PORT7m},
     {IDB_OBM3_DSCP_MAP_PORT0m, IDB_OBM3_DSCP_MAP_PORT1m,
      IDB_OBM3_DSCP_MAP_PORT2m, IDB_OBM3_DSCP_MAP_PORT3m,
      IDB_OBM3_DSCP_MAP_PORT4m, IDB_OBM3_DSCP_MAP_PORT5m,
      IDB_OBM3_DSCP_MAP_PORT6m, IDB_OBM3_DSCP_MAP_PORT7m},
    };
static const soc_field_t obm_ob_pri_fields[8] = {
    OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
    OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
    OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf
};

static const soc_reg_t obm_port_config_regs[_TH3_PBLKS_PER_PIPE] = {
    IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
    IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr
};

/* New APIs for IDB */
STATIC int
_bcm_th3_enable_dscp_ob_pri_mapping (int unit, soc_reg_t reg, int index, int
        inst) {

    uint32 rval = 0;
    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, inst, index, &rval));
    soc_reg_field_set(unit, reg, &rval, TRUST_DSCPf, 1);
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, reg, inst, index, rval));


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_obm_port_pri_traffic_class_mapping_set
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) port number
 *     priority_type - (IN) Lookup priority type
 *     obm_tc     - (IN) OBM traffic class to be mapped
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_obm_port_pri_traffic_class_mapping_set(int unit, bcm_port_t port,
        bcm_obm_lookup_priority_type_t piority_type, int priority,
                        bcm_obm_traffic_class_t obm_tc)
{
    int phy_port, pm_num, pipe, subp;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf, reg_field = INVALIDf;
    soc_info_t *si = &SOC_INFO(unit);
    int index, field_val, reg_field_val = 0;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (priority < 0 || priority >= 8) {
        return BCM_E_PARAM;
    }

    switch(obm_tc) {
        case bcmObmClassLossyLow:
            field_val = 0;
            break;
        case bcmObmClassLossyHigh:
            field_val = 1;
            break;
        case bcmObmClassLossless0:
            field_val = 2;
            break;
        case bcmObmClassLossless1:
            field_val = 3;
            break;
        default:
            return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

    switch(piority_type) {
        case bcmObmPriorityTypeVLAN:
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_pri_map_mem[pm_num][subp],
                    pipe);
            field = obm_ob_pri_fields[priority];
            index = 0;
            break;
        case bcmObmPriorityTypeDSCP:
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_dscp_map_mem[pm_num][subp],
                    pipe);
            field = obm_ob_pri_fields[priority];
            index = 0;
            BCM_IF_ERROR_RETURN(
                _bcm_th3_enable_dscp_ob_pri_mapping(unit,
                    obm_port_config_regs[pm_num], subp, pipe));
            break;
        case bcmObmPriorityTypeUntagged:
            reg = obm_port_config_regs[pm_num];
            reg_field = PORT_PRIf;
            index = subp;
            reg_field_val = priority;
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_pri_map_mem[pm_num][subp],
                    pipe);
            field = obm_ob_pri_fields[priority];
            break;
        default:
            return BCM_E_PARAM;
    }

    if (mem!= INVALIDm && field != INVALIDf) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    index, &entry));
        soc_mem_field32_set(unit, mem, &entry, field , field_val);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
    }
    if (reg != INVALIDr && reg_field != INVALIDf) {
        rval = 0;
        BCM_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, pipe, index, &rval));
        soc_reg_field_set(unit, reg, &rval, reg_field, reg_field_val);
        BCM_IF_ERROR_RETURN(
            soc_reg32_set(unit, reg, pipe, index, rval));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_tomahawk3_obm_port_pri_traffic_class_mapping_get
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) port number
 *     priority_type - (IN) Lookup priority type
 *     obm_tc     - (OUT) OBM traffic class to be mapped
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_tomahawk3_obm_port_pri_traffic_class_mapping_get(int unit, bcm_port_t port,
        bcm_obm_lookup_priority_type_t piority_type, int priority,
                        bcm_obm_traffic_class_t *obm_tc)
{
    int phy_port, pm_num, pipe, subp;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    soc_info_t *si = &SOC_INFO(unit);
    int index, field_val;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int untag_pri;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (priority < 0 || priority >= 8) {
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

    switch(piority_type) {
        case bcmObmPriorityTypeVLAN:
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_pri_map_mem[pm_num][subp],
                    pipe);
            field = obm_ob_pri_fields[priority];
            index = 0;
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry));
            field_val = soc_mem_field32_get(unit, mem, entry, field);
            break;
        case bcmObmPriorityTypeDSCP:
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_dscp_map_mem[pm_num][subp],
                    pipe);
            field = obm_ob_pri_fields[priority];
            index = 0;
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry));
            field_val = soc_mem_field32_get(unit, mem, entry, field);
            break;
        case bcmObmPriorityTypeUntagged:
            reg = obm_port_config_regs[pm_num];
            field = PORT_PRIf;
            index = subp;
            BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, reg, index, 0, &rval));
            untag_pri = soc_reg_field_get(unit, reg, rval, field);
            field = obm_ob_pri_fields[untag_pri];
            index = 0;
            mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, obm_pri_map_mem[pm_num][subp],
                    pipe);
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, entry));
            field_val = soc_mem_field32_get(unit, mem, entry, field);
            break;
        default:
            return BCM_E_PARAM;
    }
    switch(field_val) {
        case 0:
            *obm_tc = bcmObmClassLossyLow;
            break;
        case 1:
            *obm_tc = bcmObmClassLossyHigh;
            break;
        case 2:
            *obm_tc = bcmObmClassLossless0;
            break;
        case 3:
            *obm_tc = bcmObmClassLossless1;
            break;
        default:
            return BCM_E_INTERNAL;
    }


    return BCM_E_NONE;
}

static const soc_reg_t obm_max_use_config_regs[4] = {
    IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
    IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr
};

int
bcm_tomahawk3_obm_port_max_usage_mode_get (int unit, bcm_port_t port,
        bcm_obm_max_watermark_mode_t *obm_wm_mode) {
    int pipe, pm_num, phy_port;
    uint32 rval;
    soc_field_t field = PRIORITY_SELECTf;
    soc_info_t *si = &SOC_INFO(unit);

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);

    BCM_IF_ERROR_RETURN(
                soc_reg32_get(unit, obm_max_use_config_regs[pm_num], pipe, 0,
                    &rval));
    *obm_wm_mode = soc_reg_field_get(unit, obm_max_use_config_regs[pm_num], rval,
            field);

    return BCM_E_NONE;
}

int
bcm_tomahawk3_obm_port_max_usage_mode_set (int unit, bcm_port_t port,
        bcm_obm_max_watermark_mode_t obm_wm_mode) {
    int pipe, pm_num, phy_port;
    int field_val;
    uint32 rval;
    soc_field_t field = PRIORITY_SELECTf;
    soc_info_t *si = &SOC_INFO(unit);

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);

    switch(obm_wm_mode) {
        case bcmObmMaxUsageModeAllTraffic:
            field_val = 0;
            break;
        case bcmObmMaxUsageModeLossy:
            field_val = 1;
            break;
        case bcmObmMaxUsageModeLossless0:
            field_val = 2;
            break;
        case bcmObmMaxUsageModeLossless1:
            field_val = 3;
            break;
        default:
            return BCM_E_PARAM;

    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, obm_max_use_config_regs[pm_num], pipe, 0, &rval));
    soc_reg_field_set(unit, obm_max_use_config_regs[pm_num], &rval, field,
            field_val);
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, obm_max_use_config_regs[pm_num], pipe, 0, rval));

    return BCM_E_NONE;
}

static const soc_reg_t obm_priority_profile_regs[4] = {
    IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
    IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr
};

int
bcm_tomahawk3_obm_traffic_class_pfc_priority_mapping_set (int unit, bcm_port_t port,
        bcm_obm_traffic_class_t obm_tc, int max_pri_count,
        int* priority_list) {

    int pipe, pm_num, phy_port, subp;
    uint64 rval;
    soc_field_t field;
    soc_info_t *si = &SOC_INFO(unit);
    int array_idx, pri_val;
    int field_val = 0;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (max_pri_count <= 0 || max_pri_count > 8) {
        return BCM_E_PARAM;
    }
    switch (obm_tc) {
        case bcmObmClassLossless0:
            field = LOSSLESS0_PRIORITY_PROFILEf;
            break;
        case bcmObmClassLossless1:
            field = LOSSLESS1_PRIORITY_PROFILEf;
            break;
        default:
            return BCM_E_PARAM;
    }

    for (array_idx = 0; array_idx < max_pri_count; array_idx ++) {
        pri_val = priority_list[array_idx];
        if (pri_val < 0 || pri_val >= 8) {
            return BCM_E_PARAM;
        }
        field_val |= (1 << pri_val);
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

    BCM_IF_ERROR_RETURN(
        soc_reg64_get(unit, obm_priority_profile_regs[pm_num], pipe, subp, &rval));
    soc_reg64_field32_set(unit, obm_priority_profile_regs[pm_num], &rval, field,
            field_val);
    BCM_IF_ERROR_RETURN(
        soc_reg64_set(unit, obm_priority_profile_regs[pm_num], pipe, subp, rval));

    return BCM_E_NONE;
}

int
bcm_tomahawk3_obm_traffic_class_pfc_priority_mapping_get (int unit, bcm_port_t port,
        bcm_obm_traffic_class_t obm_tc, int max_pri_count,
        int* priority_list, int* pri_count) {

    int pipe, pm_num, phy_port, subp;
    uint64 rval;
    soc_field_t field;
    soc_info_t *si = &SOC_INFO(unit);
    int field_val, pri_val, array_idx = 0;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (max_pri_count <= 0 || max_pri_count > 8) {
        return BCM_E_PARAM;
    }
    switch (obm_tc) {
        case bcmObmClassLossless0:
            field = LOSSLESS0_PRIORITY_PROFILEf;
            break;
        case bcmObmClassLossless1:
            field = LOSSLESS1_PRIORITY_PROFILEf;
            break;
        default:
            return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

    BCM_IF_ERROR_RETURN(
                soc_reg_get(unit, obm_priority_profile_regs[pm_num], pipe,
                    subp, &rval));
    field_val = soc_reg64_field32_get(unit, obm_priority_profile_regs[pm_num], rval,
            field);
    for (pri_val = 0; pri_val < 8 ; pri_val++) {
        if ((field_val == 0) || (array_idx == max_pri_count)) {
            break;
        }
        if ((field_val >> pri_val) & 1) {
            priority_list[array_idx++] = pri_val;
        }
    }
    *pri_count = array_idx;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_cosq_bucket_set
 * Purpose:
 *     Get COS queue bandwidth control bucket setting
 * Parameters:
 *     unit          - (IN) unit number
 *     gport         - (IN) port number or GPORT identifier
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
_bcm_th3_cosq_bucket_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 kbits_burst_min, uint32 kbits_burst_max,
                        uint32 flags)
{
    _bcm_th3_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    uint32 rval, meter_flags;
    uint32 bucketsize_max, bucketsize_min;
    uint32 granularity_max, granularity_min;
    uint32 refresh_rate_max, refresh_rate_min;
    int refresh_bitsize = 0, bucket_bitsize = 0;
    int pipe, local_mmu_port, index;
    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    int cpu_mc_base = 0;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    BCM_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    local_mmu_port = SOC_TH3_MMU_LOCAL_PORT(unit, local_port);

    if (BCM_GPORT_IS_SET(gport) &&
        ((BCM_GPORT_IS_SCHEDULER(gport)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, gport, NULL,
                                   NULL, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (node->level == SOC_TH3_NODE_LVL_L0) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_Am)[pipe];
                cosq = node->hw_index % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
                index = ((local_mmu_port & (SOC_TH3_MMU_PORT_STRIDE - 1)) *
                         _BCM_TH3_NUM_SCHEDULER_PER_PORT) + cosq;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            /* L1 level metering is supported only for CPU.
               CPU has only multicast queues.
               So cannot be Unicast type */
                return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            /* L1 level metering is supported only for CPU */
            if (!IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PORT;
            }
            config_mem = MMU_MTRO_CPU_L1_Am;
            cosq = node->hw_index % si->port_num_cosq[local_port];

            /*Adjust mc cosq for cpu port*/
            cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
            cosq = (node->hw_index - cpu_mc_base) %
                   _BCM_TH3_NUM_CPU_MCAST_QUEUE;
            index = cosq;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if ((cosq < 0) || (cosq >= _TH3_PORT_NUM_COS(unit, local_port))) {
            return BCM_E_PARAM;
        }
        if (!IS_CPU_PORT(unit, local_port)) {
            index = ((local_mmu_port & (SOC_TH3_MMU_PORT_STRIDE - 1)) *
                     _BCM_TH3_NUM_SCHEDULER_PER_PORT) + cosq;
            config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_Am)[pipe];
        } else {
            /* For CPU port, refer L1 memory directly */
            config_mem = MMU_MTRO_CPU_L1_Am;
            
            index = cosq;
        }
    }

    /*
     * COVERITY
     *
     * This checker will not be True.
     * It is kept intentionally as a defensive case for future development.
     */
    /* coverity[dead_error_line] */
    if (config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_TH3_METER_FLAG_PACKET_MODE : 0;

    BCM_IF_ERROR_RETURN(READ_MMU_MTRO_CONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MMU_MTRO_CONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TH3_METER_FLAG_NON_LINEAR;
    }


    refresh_bitsize = soc_mem_field_length(unit, config_mem, MAX_REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, config_mem, MAX_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_rate_to_bucket_encoding(unit, max_quantum, kbits_burst_max,
                                           meter_flags, refresh_bitsize,
                                           bucket_bitsize, &refresh_rate_max,
                                           &bucketsize_max, &granularity_max));

    refresh_bitsize = soc_mem_field_length(unit, config_mem, MIN_REFRESHf);
    bucket_bitsize = soc_mem_field_length(unit, config_mem, MIN_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_rate_to_bucket_encoding(unit, min_quantum, kbits_burst_min,
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
    soc_mem_field32_set(unit, config_mem, &entry, PACKET_SHAPINGf, (flags & BCM_COSQ_BW_PACKET_MODE) ? 1 : 0);
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_cosq_bucket_get
 * Purpose:
 *     Get COS queue bandwidth control bucket setting
 * Parameters:
 *     unit          - (IN) unit number
 *     gport         - (IN) port number or GPORT identifier
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
_bcm_th3_cosq_bucket_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                        uint32 *flags)
{
    _bcm_th3_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    uint32 rval;
    int local_mmu_port, index;
    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    int cpu_mc_base = 0;
    int pipe;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;

    meter_flags = 0;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "gport: %d cosq: %d\n"),
              gport, cosq));
    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
    pipe = si->port_pipe[local_port];

    local_mmu_port = SOC_TH3_MMU_LOCAL_PORT(unit, local_port);

    if (BCM_GPORT_IS_SET(gport) &&
        ((BCM_GPORT_IS_SCHEDULER(gport)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport))) {

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_node_get(unit, gport, NULL,
                                   NULL, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (node->level == SOC_TH3_NODE_LVL_L0) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_Am)[pipe];
                cosq = node->hw_index % _BCM_TH3_NUM_SCHEDULER_PER_PORT;
                index = ((local_mmu_port & (SOC_TH3_MMU_PORT_STRIDE - 1)) *
                         _BCM_TH3_NUM_SCHEDULER_PER_PORT) + cosq;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            /* L1 level metering is supported only for CPU
               so cannot be Unicast type */
                return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            /* L1 level metering is supported only for CPU */
            if (!IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PORT;
            }
            config_mem = MMU_MTRO_CPU_L1_Am;
            cosq = node->hw_index % si->port_num_cosq[local_port];

            /*Adjust mc cosq for cpu port*/
            cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
            cosq = (node->hw_index - cpu_mc_base) %
                   _BCM_TH3_NUM_CPU_MCAST_QUEUE;
            index = cosq;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if ((cosq < 0) || (cosq >= _TH3_PORT_NUM_COS(unit, local_port))) {
            return BCM_E_PARAM;
        }
        if (!IS_CPU_PORT(unit, local_port)) {
            index = ((local_mmu_port & (SOC_TH3_MMU_PORT_STRIDE - 1)) *
                     _BCM_TH3_NUM_SCHEDULER_PER_PORT) + cosq;
            config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_Am)[pipe];
        } else {
            /* For CPU port, refer L1 memory directly */
            config_mem = MMU_MTRO_CPU_L1_Am;
            index = cosq;
        }
    }

    /*
     * COVERITY
     *
     * This checker will not be True.
     * It is kept intentionally as a defensive case for future development.
     */
    /* coverity[dead_error_line] */
    if (config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    /* index = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, config_mem, index); */

    if (min_quantum == NULL || max_quantum == NULL ||
        kbits_burst_max == NULL || kbits_burst_min == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    *flags = 0;
    BCM_IF_ERROR_RETURN(READ_MMU_MTRO_CONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MMU_MTRO_CONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TH3_METER_FLAG_NON_LINEAR;
    }

    if (soc_mem_field32_get(unit, config_mem, &entry, PACKET_SHAPINGf)) {
        meter_flags |= _BCM_TH3_METER_FLAG_PACKET_MODE;
        *flags |= BCM_COSQ_BW_PACKET_MODE;
    }

    granularity = soc_mem_field32_get(unit, config_mem, &entry, MAX_METER_GRANf);
    refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MAX_REFRESHf);
    bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MAX_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           max_quantum, kbits_burst_max));

    granularity = soc_mem_field32_get(unit, config_mem, &entry, MIN_METER_GRANf);
    refresh_rate = soc_mem_field32_get(unit, config_mem, &entry, MIN_REFRESHf);
    bucketsize = soc_mem_field32_get(unit, config_mem, &entry, MIN_THD_SELf);
    BCM_IF_ERROR_RETURN
        (_bcm_th3_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           min_quantum, kbits_burst_min));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_port_bandwidth_set
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
bcm_th3_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags)
{
        return bcm_th3_cosq_gport_bandwidth_set(unit, port, cosq,
                                               min_quantum, max_quantum,
                                               flags);
}


/*
 * Function:
 *     bcm_th3_cosq_port_bandwidth_get
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
bcm_th3_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{
    return bcm_th3_cosq_gport_bandwidth_get(unit, port, cosq,
                                               min_quantum, max_quantum,
                                               flags);
}

/*
 * Function:
 *     bcm_th3_cosq_gport_bandwidth_burst_get
 * Purpose:
 *     Get COS queue bandwidth burst setting
 * Parameters:
 *     unit        - (IN) unit number
 *     gport       - (IN) GPORT identifier
 *     cosq        - (IN) COS queue to get, -1 for Queue 0
 *     kbits_burst - (OUT) maximum burst, kbits
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 *kbits_burst_min,
                                      uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max, flags;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 &kbits_sec_min, &kbits_sec_max, kbits_burst_min,
                                 kbits_burst_max, &flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_bandwidth_burst_set
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
bcm_th3_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int i, start_cos=0, end_cos=0;
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst, flags;
    bcm_port_t local_port;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    start_cos = end_cos = cosq;

    if (cosq == BCM_COS_INVALID) {
        start_cos = 0;
        end_cos = (IS_CPU_PORT(unit, local_port)) ?
                   NUM_CPU_COSQ(unit) - 1 : _TH3_PORT_NUM_COS(unit, local_port)
                   - 1;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
             BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
             BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            start_cos = end_cos = 0;
        }
    }

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_bucket_get(unit, gport, i, &kbits_sec_min,
                                     &kbits_sec_max, &kbits_sec_burst,
                                     &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                     kbits_sec_max, kbits_burst_min,
                                     kbits_burst_max, flags));
    }
    return BCM_E_NONE;
}

int
bcm_th3_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst, burst_min, flags;
    bcm_port_t  local_port =  port;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port) &&
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                    &burst, &flags));
    min = pps;
    burst_min = (min > 0) ?
          _bcm_th3_default_burst_size(unit, local_port, min) : 0;
    burst = burst_min;

    return _bcm_th3_cosq_bucket_set(unit, port, cosq, min, pps, burst_min,
                                   burst, flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_th3_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port) &&
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                 &flags));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_th3_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst)
{
    uint32 min, max, cur_burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port) &&
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst,
                                 &cur_burst, &flags));

    /* Replace the current BURST setting, keep PPS the same */

    return _bcm_th3_cosq_bucket_set(unit, port, cosq, min, max, burst, burst,
                                   flags | BCM_COSQ_BW_PACKET_MODE);
    return BCM_E_NONE;
}

int
bcm_th3_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst, cur_burst_min, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port) &&
        ((BCM_GPORT_IS_SCHEDULER(port)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
          BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, port, cosq, &min, &max,
                                 &cur_burst_min, &cur_burst, &flags));
    *burst = cur_burst;
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_time_domain_set(int unit, int time_value, int *time_id)
{
    int id;
    uint32 time_domain_val;
    soc_reg_t time_domain_reg = MMU_WRED_TIME_DOMAINr;

    if (time_value < 0 || time_value > 63) {
        return SOC_E_PARAM;
    }

    for (id = 0; id < _BCM_TH3_NUM_TIME_DOMAIN; id++) {
        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, time_domain_reg, REG_PORT_ANY, id, &time_domain_val));
        if (!th3_time_domain_info[unit][id].ref_count) {
            soc_reg_field_set(unit, time_domain_reg, &time_domain_val,
                              TIME_DOMAIN_FIELDf, time_value);
            th3_time_domain_info[unit][id].ref_count++;
            break;
        }
    }

    if (id == _BCM_TH3_NUM_TIME_DOMAIN) {/* No field in TIME_DOMAINr is free,return ERR*/
        return BCM_E_RESOURCE;
    }

    if (time_id) {
        *time_id = id;
    }
    BCM_IF_ERROR_RETURN(
        soc_reg32_set(unit, time_domain_reg, REG_PORT_ANY, id, time_domain_val));
    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_time_domain_get(int unit, int time_id, int *time_value)
{
    uint32 time_domain_val;
    soc_reg_t time_domain_reg = MMU_WRED_TIME_DOMAINr;

    if (time_id < 0 || time_id > _BCM_TH3_NUM_TIME_DOMAIN - 1) {
        return SOC_E_PARAM;
    }

    if (time_value == NULL) {
        return SOC_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        soc_reg32_get(unit, time_domain_reg, REG_PORT_ANY, time_id, &time_domain_val));
    *time_value = soc_reg_field_get(unit, time_domain_reg,
                                   time_domain_val, TIME_DOMAIN_FIELDf);
    return SOC_E_NONE;
}


/*
 * Function:
 *     _bcm_th3_cosq_wred_set
 * Purpose:
 *     Configure unicast queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (IN) Min Threshold
 *     max_thresh          - (IN) Max Threshold
 *     drop_probablity     - (IN) Probabilistic drop rate
 *     gain                - (IN)
 *     ignore_enable_flags - (IN)
 *     lflags              - (IN) Config Flags
 *     refresh_time        - (IN) actual refresh time
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_th3_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      bcm_cosq_buffer_id_t buffer,
                      uint32 flags, uint32 min_thresh, uint32 max_thresh,
                      int drop_probability, int gain, int ignore_enable_flags,
                      uint32 lflags, int refresh_time, uint32 resolution_id)
{
    /* Local Variables */
    soc_mem_t wred_config_mem = INVALIDm;
    soc_reg_t wred_config_reg = INVALIDr;
    bcm_port_t local_port = -1;
    bcm_port_t port_num;
    int index, i, start_index, end_index, count = 0;
    int end_offset, need_profile = 0, pipe, itm, itm_map, accessed = 0;
    uint32 profile_index, old_profile_index;
    uint32 wred_flags = 0;
    void *entries[_BCM_TH3_NUM_WRED_PROFILES];
    soc_mem_t mems[_BCM_TH3_NUM_WRED_PROFILES];
    static soc_mem_t wred_mems[_BCM_TH3_NUM_WRED_PROFILES] = {
        MMU_WRED_DROP_CURVE_PROFILE_0_0m, MMU_WRED_DROP_CURVE_PROFILE_1_0m,
        MMU_WRED_DROP_CURVE_PROFILE_2_0m, MMU_WRED_DROP_CURVE_PROFILE_3_0m,
        MMU_WRED_DROP_CURVE_PROFILE_4_0m, MMU_WRED_DROP_CURVE_PROFILE_5_0m,
        MMU_WRED_DROP_CURVE_PROFILE_6_0m, MMU_WRED_DROP_CURVE_PROFILE_7_0m,
        MMU_WRED_DROP_CURVE_PROFILE_8_0m
    };
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    mmu_wred_drop_curve_profile_6_entry_t entry_ecn_green;
    mmu_wred_drop_curve_profile_7_entry_t entry_ecn_yellow;
    mmu_wred_drop_curve_profile_8_entry_t entry_ecn_red;
    /* mmu_wred_config_entry_t entry; */
    uint32 entry = 0;
    int rate = 0;
    int from_pipe, to_pipe;
    int time_id,time,old_time,current_time_sel;
    int rv = SOC_E_NONE, exists = 0;
    int device_setting, portsp_setting, queue_setting;

    device_setting = 0;
    portsp_setting = 0;
    queue_setting = 0;

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: port: %d cosq: %d min_thres: %d max_thresh: %d flags: %x\n"),
            port, cosq, min_thresh, max_thresh, flags));
    if ((buffer != BCM_COSQ_BUFFER_ID_INVALID) &&
        ((buffer < 0) || (buffer >= _TH3_ITMS_PER_DEV))) {
        return BCM_E_PARAM;
    }

    wred_flags = flags | lflags;
    end_offset = 0;

    /* Check Flags for WRED res level - Device-SP/Port-SP/Queues */
    if ((wred_flags & BCM_COSQ_DISCARD_DEVICE) && (port == BCM_GPORT_INVALID)) {
        /* (Port == -1) For all Global SPs */
        wred_config_reg = MMU_WRED_POOL_CONFIGr;
        wred_config_mem = MMU_WRED_QUEUE_CONFIGm;
        index = 0;
        count = _TH3_MMU_NUM_POOL;
        /*For all pipe*/
        from_pipe = 0;
        to_pipe = NUM_PIPE(unit) - 1;
        device_setting = 1;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        from_pipe = to_pipe = pipe;

        if (wred_flags & BCM_COSQ_DISCARD_DEVICE) {
            wred_config_reg = MMU_WRED_POOL_CONFIGr;
            device_setting = 1;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_DEVICE,
                                            NULL, &index, &count));
            /* (cosq == -1) For all Global SPs */
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Device settings port: %d cosq: %d index: %d count: %d\n"),
                    port, cosq, index, count));
        } else if (wred_flags & BCM_COSQ_DISCARD_PORT) {
            /* Port SP - Given or all SPs of the port */
            wred_config_mem = MMU_WRED_PORTSP_CONFIGm;
            portsp_setting = 1;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_PORT,
                                            NULL, &index, &count));
            /* (cosq == -1) For all SPs of the given port */
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Port settings port: %d cosq: %d index: %d count: %d\n"),
                    port, cosq, index, count));
        } else {
            /* Per Queue WRED settings */
            wred_config_mem = MMU_WRED_QUEUE_CONFIGm;
            queue_setting = 1;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_QUEUE,
                                            NULL, &index, &count));
            /* (cosq == -1) For all Queues of the given port */
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d index: %d count: %d\n"),
                    port, cosq, index, count));
        }
    }


    end_offset = (count > 0) ? (count - 1) : 0;

    if (index < 0) {
        return BCM_E_PARAM;
    }
    if (refresh_time < 1) {
        return BCM_E_PARAM;
    }
    start_index = index;
    end_index = start_index + end_offset;

    rate = _bcm_th3_percent_to_drop_prob(drop_probability);

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d rate: %d start_index: %d end_index:%d\n"),
            port, cosq, rate, start_index, end_index));

    for (pipe = from_pipe; pipe <= to_pipe; pipe++ ) {
        /* Config MMU_WRED_CONFIGm for the respective resource */

        for (i=0; i < _BCM_TH3_NUM_WRED_PROFILES; i++) {
            mems[i] = INVALIDm;
        }

        if (wred_flags & (BCM_COSQ_DISCARD_COLOR_ALL |
                          BCM_COSQ_DISCARD_TCP |
                          BCM_COSQ_DISCARD_NONTCP |
                          BCM_COSQ_DISCARD_ECT_MARKED |
                          BCM_COSQ_DISCARD_RESPONSIVE_DROP |
                          BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
            need_profile = 1;
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d need_profile: %d\n"),
                    port, cosq, need_profile));

            if (!(flags & (BCM_COSQ_DISCARD_COLOR_GREEN |
                           BCM_COSQ_DISCARD_COLOR_YELLOW |
                           BCM_COSQ_DISCARD_COLOR_RED))) {
                flags |= BCM_COSQ_DISCARD_COLOR_ALL;
            }

            if (!(wred_flags & (BCM_COSQ_DISCARD_NONTCP |
                                BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                                BCM_COSQ_DISCARD_ECT_MARKED)) ||
                 ((wred_flags & BCM_COSQ_DISCARD_TCP) ||
                 (wred_flags & BCM_COSQ_DISCARD_RESPONSIVE_DROP))) {
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[0] = wred_mems[0];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[1] = wred_mems[1];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[2] = wred_mems[2];
                }
            }
            if ((wred_flags & BCM_COSQ_DISCARD_NONTCP) ||
                (wred_flags & BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)){
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[3] = wred_mems[3];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[4] = wred_mems[4];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[5] = wred_mems[5];
                }
            }
            if (wred_flags & BCM_COSQ_DISCARD_ECT_MARKED) {
                if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                    mems[6] = wred_mems[6];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                    mems[7] = wred_mems[7];
                }
                if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                    mems[8] = wred_mems[8];
                }
            }
        }

        entries[0] = &entry_tcp_green;
        entries[1] = &entry_tcp_yellow;
        entries[2] = &entry_tcp_red;
        entries[3] = &entry_nontcp_green;
        entries[4] = &entry_nontcp_yellow;
        entries[5] = &entry_nontcp_red;
        entries[6] = &entry_ecn_green;
        entries[7] = &entry_ecn_yellow;
        entries[8] = &entry_ecn_red;

        if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
            itm_map = 3;
        } else {
            itm_map = (1 << buffer);
        }

        for (itm = 0; itm < NUM_ITM(unit); itm ++) {
            if (!(itm_map & (1 << itm))) {
                continue;
            }

            accessed = 1;
            for (index = start_index; index <= end_index; index++) {
                old_profile_index = TH3_WRED_PROFILE_INDEX_INVALID;
                if (portsp_setting || queue_setting) {
                    BCM_IF_ERROR_RETURN
                     (soc_tomahawk3_itm_mem_read(unit, wred_config_mem, itm, pipe,
                                                MEM_BLOCK_ALL, index, &entry));
                }  else { /* device_setting */
                    BCM_IF_ERROR_RETURN
                     (soc_tomahawk3_itm_reg32_get(unit, wred_config_reg, -1, -1,
                                                index, &entry));
                }
                if (need_profile) {
                    if (portsp_setting || queue_setting) {
                        old_profile_index = soc_mem_field32_get(unit, wred_config_mem,
                                                            &entry, PROFILE_INDEXf);
                    } else { /* device_setting */
                        old_profile_index = soc_reg_field_get(unit, wred_config_reg,
                                                            entry, PROFILE_INDEXf);
                    }
                    LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d old_profile_index: %d\n"),
                            port, cosq, old_profile_index));

                    BCM_IF_ERROR_RETURN
                        (soc_profile_mem_get(unit, _bcm_th3_wred_profile[unit],
                                             old_profile_index, 1, entries));
                    LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d 1\n"),
                            port, cosq));

                    for (i = 0; i < _BCM_TH3_NUM_WRED_PROFILES; i++) {
                        if (mems[i] == INVALIDm) {
                            continue;
                        }
                        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                                            MIN_THDf, min_thresh);
                        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                                            MAX_THDf, max_thresh);
                        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                                            MAX_DROP_RATEf, rate);
                    }
                    BCM_IF_ERROR_RETURN
                        (soc_profile_mem_add(unit, _bcm_th3_wred_profile[unit],
                                             entries, 1, &profile_index));

                    LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d profile_index: %d\n"),
                            port, cosq, profile_index));
                    if (portsp_setting || queue_setting) {
                        soc_mem_field32_set(unit, wred_config_mem, &entry, PROFILE_INDEXf, profile_index);
                    } else { /* device_setting */
                        soc_reg_field_set(unit, wred_config_reg, &entry, PROFILE_INDEXf, profile_index);
                    }
                }

                if (queue_setting) {
                    /* Determine SPID */
                    int spid;
                    if (cosq == -1) {
                        BCM_IF_ERROR_RETURN
                            (_bcm_th3_cosq_index_resolve(unit, port, index - start_index,
                                                _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                                NULL, &spid, &count));
                    } else {
                        BCM_IF_ERROR_RETURN
                            (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                                _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                                NULL, &spid, &count));
                    }
                    LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: Queue settings port: %d cosq: %d spid: %d\n"),
                            port, cosq, spid));
                    soc_mem_field32_set(unit, wred_config_mem, &entry, SPIDf, spid);
                }


                if (portsp_setting || queue_setting) {
                    soc_mem_field32_set(unit, wred_config_mem, &entry, WEIGHTf, gain);
                } else { /* device_setting */
                    soc_reg_field_set(unit, wred_config_reg, &entry, WEIGHTf, gain);
                }

                /* Some APIs only modify profiles */
                if (!ignore_enable_flags) {
                    if (portsp_setting || queue_setting) {
                        soc_mem_field32_set(unit, wred_config_mem, &entry, CAP_AVERAGEf,
                                        flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
                        soc_mem_field32_set(unit, wred_config_mem, &entry, WRED_ENf,
                                    flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
                    } else { /* device_setting */
                        soc_reg_field_set(unit, wred_config_reg, &entry, CAP_AVERAGEf,
                                        flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
                        soc_reg_field_set(unit, wred_config_reg, &entry, WRED_ENf,
                                    flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
                    }
                    if (queue_setting) {
                        soc_mem_field32_set(unit, wred_config_mem, &entry, ECN_MARKING_ENf,
                                        flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?  1 : 0);
                    }
                }

                if (portsp_setting || queue_setting) {
                    current_time_sel = soc_mem_field32_get(unit, wred_config_mem,
                                                       &entry, TIME_DOMAIN_SELf);
                } else { /* device_setting */
                    current_time_sel = soc_reg_field_get(unit, wred_config_reg,
                                                       entry, TIME_DOMAIN_SELf);
                }
                time = refresh_time - 1;
                exists = 0;
                /* If the time value exist, update reference count only */
                for (time_id = 0; time_id < _BCM_TH3_NUM_TIME_DOMAIN; time_id++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th3_cosq_time_domain_get(unit, time_id, &old_time));
                    if (time == old_time) {
                        /* Only set exist flag if this entry reference count already update,
                                      otherwise update reference count */
                        if(time_id != current_time_sel){
                            if (portsp_setting || queue_setting) {
                                soc_mem_field32_set(unit, wred_config_mem, &entry,
                                                TIME_DOMAIN_SELf, time_id);
                            } else { /* device_setting */
                                soc_reg_field_set(unit, wred_config_reg, &entry,
                                                TIME_DOMAIN_SELf, time_id);
                            }
                            th3_time_domain_info[unit][time_id].ref_count++;
                            th3_time_domain_info[unit][current_time_sel].ref_count--;
                        }
                        exists = 1;
                        break;
                    }
                }

                if(!exists){
                    rv = _bcm_th3_cosq_time_domain_set(unit, time, &time_id);
                    if(rv == SOC_E_NONE){
                        if (portsp_setting || queue_setting) {
                            soc_mem_field32_set(unit, wred_config_mem, &entry,
                                            TIME_DOMAIN_SELf, time_id);
                        } else { /* device_setting */
                            soc_reg_field_set(unit, wred_config_reg, &entry,
                                            TIME_DOMAIN_SELf, time_id);
                        }
                        th3_time_domain_info[unit][current_time_sel].ref_count--;
                    } else {
                        return rv;
                    }
                }

                if (flags & BCM_COSQ_DISCARD_MARK_CONGESTION) {
                    if (queue_setting) {
                        soc_mem_field32_set(unit, wred_config_mem, &entry,
                            WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf,
                            resolution_id);
                    }
                } else {
                    if (queue_setting) {
                        soc_mem_field32_set(unit, wred_config_mem, &entry,
                            WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf,
                            resolution_id);
                    }
                }

                if (portsp_setting || queue_setting) {
                    BCM_IF_ERROR_RETURN
                        (soc_tomahawk3_itm_mem_write(unit, wred_config_mem, itm, pipe,
                                                MEM_BLOCK_ALL, index, &entry));
                } else { /* device_setting */
                    BCM_IF_ERROR_RETURN
                        (soc_tomahawk3_itm_reg32_set(unit, wred_config_reg, itm, -1,
                                                index, entry));
                }

                if (old_profile_index != TH3_WRED_PROFILE_INDEX_INVALID) {
                    SOC_IF_ERROR_RETURN
                        (soc_profile_mem_delete(unit, _bcm_th3_wred_profile[unit],
                                                old_profile_index));
                }

            }
        }
    }

    /* pipe (port) and itm (buffer) are not related */
    if (!accessed) {
        return BCM_E_PARAM;
    }

    if (device_setting) {
        flags &= ~(BCM_COSQ_DISCARD_DEVICE);
        PBMP_PORT_ITER(unit, port_num) {
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: device_setting calling wred_queue with port: %d cosq: -1\n"),
                            port_num));
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_wred_set(unit, port_num, -1, buffer,
                                   flags, min_thresh, max_thresh,
                                   drop_probability, gain,
                                   FALSE, flags, refresh_time,
                                   resolution_id));
        }
    }
    if (portsp_setting) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "_bcm_th3_cosq_wred_set: portsp_setting calling wred_queue with port: %d cosq: -1\n"),
                        port));
        flags &= ~(BCM_COSQ_DISCARD_PORT);
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_wred_set(unit, port, -1, buffer,
                               flags, min_thresh, max_thresh,
                               drop_probability, gain,
                               FALSE, flags, refresh_time,
                               resolution_id));
    }

    return rv;
}

/*
 * Function:
 *     _bcm_th3_cosq_wred_get
 * Purpose:
 *     Configure unicast queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN/OUT) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (OUT) Min Threshold
 *     max_thresh          - (OUT) Max Threshold
 *     drop_probablity     - (OUT) Probabilistic drop rate
 *     gain                - (OUT)
 *     lflags              - (IN) Config flags
 *     refresh_time        - (OUT) Time Domain
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_th3_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      bcm_cosq_buffer_id_t buffer,
                      uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                      int *drop_probability, int *gain, uint32 lflags,
                      int *refresh_time, uint32 *resolution_id)
{
    soc_mem_t wred_config_mem = INVALIDm, mem = INVALIDm;
    soc_reg_t wred_config_reg = INVALIDr;
    bcm_port_t local_port = -1;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    mmu_wred_drop_curve_profile_6_entry_t entry_ecn_green;
    mmu_wred_drop_curve_profile_7_entry_t entry_ecn_yellow;
    mmu_wred_drop_curve_profile_8_entry_t entry_ecn_red;
    void *entries[_BCM_TH3_NUM_WRED_PROFILES];
    void *entry_p;
    int index, profile_index;
    uint32 entry = 0;
    uint32 wred_flags = 0;
    int rate = 0, pipe = -1, itm;
    int time_id, time;
    int portsp_setting = 0;
    int queue_setting = 0;

    if ((buffer != BCM_COSQ_BUFFER_ID_INVALID) &&
        ((buffer < 0) || (buffer >= NUM_ITM(unit)))) {
        return BCM_E_PARAM;
    }

    if (flags) {
        wred_flags = *flags | lflags;
    } else {
        wred_flags = lflags;
    }

    if ((wred_flags & BCM_COSQ_DISCARD_DEVICE) && (port == BCM_GPORT_INVALID)) {
        /* (Port == -1) For all Global SPs */
        index = 0;
        pipe = 0;
        wred_config_reg = MMU_WRED_POOL_CONFIGr;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, port, &local_port));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        /* Check Flags for WRED res level - Device-SP/Port-SP/Queues */
        if(wred_flags & BCM_COSQ_DISCARD_DEVICE) {
            wred_config_reg = MMU_WRED_POOL_CONFIGr;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_DEVICE,
                                            NULL, &index, NULL));
        } else if (wred_flags & BCM_COSQ_DISCARD_PORT) {
            /* Port SP - Given or all SPs of the port */
            wred_config_mem = MMU_WRED_PORTSP_CONFIGm;
            portsp_setting = 1;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_PORT,
                                            NULL, &index, NULL));
        } else {
            /* Per Queue WRED settings */
            wred_config_mem = MMU_WRED_QUEUE_CONFIGm;
            queue_setting = 1;
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_WRED_QUEUE,
                                            NULL, &index, NULL));
        }
    }

    if ((index < 0) || (pipe == -1)) {
        return BCM_E_PARAM;
    }

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        itm = -1;
    } else if (port == BCM_GPORT_INVALID) {
        itm = buffer;
        pipe = -1;
    } else {
        itm = buffer;
    }

    if (portsp_setting || queue_setting) {
        BCM_IF_ERROR_RETURN
            (soc_tomahawk3_itm_mem_read(unit, wred_config_mem, itm, pipe,
                                   MEM_BLOCK_ALL, index, &entry));
        profile_index = soc_mem_field32_get(unit, wred_config_mem,
                                        &entry, PROFILE_INDEXf);
    } else { /* device_setting */
        BCM_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, wred_config_reg, itm, -1,
                                                index, &entry));
        profile_index = soc_reg_field_get(unit, wred_config_reg,
                                        entry, PROFILE_INDEXf);
    }


    if (!(wred_flags & (BCM_COSQ_DISCARD_NONTCP |
                        BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                        BCM_COSQ_DISCARD_ECT_MARKED))) {
        /* By default we will assume
           BCM_COSQ_DISCARD_TCP */
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_1_0m;
            entry_p = &entry_tcp_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_2_0m;
            entry_p = &entry_tcp_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_0_0m;
            entry_p = &entry_tcp_green;
        }
    } else if ((wred_flags & BCM_COSQ_DISCARD_NONTCP) ||
               (wred_flags & BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_4_0m;
            entry_p = &entry_nontcp_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_5_0m;
            entry_p = &entry_nontcp_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_3_0m;
            entry_p = &entry_nontcp_green;
        }
    } else {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_7_0m;
            entry_p = &entry_ecn_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_8_0m;
            entry_p = &entry_ecn_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_6_0m;
            entry_p = &entry_ecn_green;
        }
    }

    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    entries[6] = &entry_ecn_green;
    entries[7] = &entry_ecn_yellow;
    entries[8] = &entry_ecn_red;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_th3_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_th3_drop_prob_to_percent(rate);
    }
    if (gain) {
        if (portsp_setting || queue_setting) {
            *gain = soc_mem_field32_get(unit, wred_config_mem, &entry, WEIGHTf);
        } else { /* device_setting */
            *gain = soc_reg_field_get(unit, wred_config_reg, entry, WEIGHTf);
        }
    }

    /* Get the Time Domain value */
    if (refresh_time) {
        if (portsp_setting || queue_setting) {
            time_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                                          TIME_DOMAIN_SELf);
        } else { /* device_setting */
            time_id = soc_reg_field_get(unit, wred_config_reg, entry,
                                          TIME_DOMAIN_SELf);
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_time_domain_get(unit,time_id,&time));
        *refresh_time = time + 1;
    }

    /* Get flags */
    if (flags) {
        *flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE | BCM_COSQ_DISCARD_ENABLE |
                    BCM_COSQ_DISCARD_MARK_CONGESTION);
        if (portsp_setting || queue_setting) {
            if (soc_mem_field32_get(unit, wred_config_mem, &entry, CAP_AVERAGEf)) {
                *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
            }
            if (soc_mem_field32_get(unit, wred_config_mem, &entry, WRED_ENf)) {
                *flags |= BCM_COSQ_DISCARD_ENABLE;
            }
        } else { /* device_setting */
            if (soc_reg_field_get(unit, wred_config_reg, entry, CAP_AVERAGEf)) {
                *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
            }
            if (soc_reg_field_get(unit, wred_config_reg, entry, WRED_ENf)) {
                *flags |= BCM_COSQ_DISCARD_ENABLE;
            }
        }
        if (queue_setting) {
            if (soc_mem_field32_get(unit, wred_config_mem, &entry, ECN_MARKING_ENf)) {
                *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
            }
        }
    }

    /* Get resolution table index */
    if (resolution_id) {
        if (queue_setting) {
            if (soc_mem_field32_get(unit, wred_config_mem, &entry, ECN_MARKING_ENf)) {
                *resolution_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                    WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf);
            } else {
                *resolution_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                    WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf);
            }
        }
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *     bcm_th3_cosq_discard_set
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit          - (IN) unit number
 *     flags         - (IN) DISCARD flags
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Set the WRED discard flags
 */

int
bcm_th3_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;

    if (_bcm_th3_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (flags & ~(BCM_COSQ_DISCARD_DEVICE |
                  BCM_COSQ_DISCARD_NONTCP |
                  BCM_COSQ_DISCARD_COLOR_ALL |
                  BCM_COSQ_DISCARD_PORT |
                  BCM_COSQ_DISCARD_TCP |
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
        /* cosq(0), buffer(INVALID), flags, min_thresh(0)
           max_thresh(0), drop_prob(0), gain(0), ignore_enable_flags(0),
           lflags(BCM_COSQ_DISCARD_PORT), refresh_time(1), resolution_id(0) */
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_wred_set(unit, port, 0, BCM_COSQ_BUFFER_ID_INVALID,
                                   flags, 0, 0, 0, 0,
                                   FALSE, BCM_COSQ_DISCARD_PORT, 1, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_discard_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit          - (IN) unit number
 *     flags         - (OUT) DISCARD flags
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Get the WRED discard flags
 */

int
bcm_th3_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_th3_cosq_wred_get(unit, port, 0, BCM_COSQ_BUFFER_ID_INVALID,
                                     flags, NULL, NULL, NULL,
                                     NULL, BCM_COSQ_DISCARD_PORT, NULL, NULL);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_th3_cosq_discard_port_set
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
 *     Same as TD2 except for Reg changes.
 */
int
bcm_th3_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain;
    uint32 min_thresh, max_thresh, shared_limit, itm_map;
    uint32 bits, flags = 0;

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    /*
     * average queue size is reculated every 1us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     * gain = log2(average_time)
     */
    bits = average_time & 0xffff;
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
    itm_map = SOC_INFO(unit).itm_map;
    _TH3_ARRAY_MIN(_BCM_TH3_MMU_INFO(unit)->shared_limit, NUM_ITM(unit),
                  itm_map, shared_limit);
    min_thresh = ((drop_start * shared_limit) + (100 - 1)) / 100;

    /* Calculate the max threshold. For a given slope (angle in
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_th3_angle_to_cells(drop_slope);
    if (max_thresh > TH3_WRED_CELL_FIELD_MAX) {
        max_thresh = TH3_WRED_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_wred_set(unit, port, cosq,
                                   BCM_COSQ_BUFFER_ID_INVALID, color,
                                   min_thresh, max_thresh, 100, gain,
                                   TRUE, flags, 1, 0));
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }

        BCM_PBMP_ITER(pbmp, local_port) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_wred_set(unit, local_port, cosq,
                                       BCM_COSQ_BUFFER_ID_INVALID, color,
                                       min_thresh, max_thresh, 100, gain,
                                       TRUE, 0, 1, 0));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_discard_port_get
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
 *     Same as TD2 except for Reg changes.
 */
int
bcm_th3_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
    uint32 min_thresh, max_thresh, shared_limit, itm_map;
    int refresh_time = 0;

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
        (_bcm_th3_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                               BCM_COSQ_BUFFER_ID_INVALID,
                               &color, &min_thresh, &max_thresh, &drop_prob,
                               &gain, 0, &refresh_time, NULL));

    /*
     * average queue size is reculated every 8us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain);

    itm_map = SOC_INFO(unit).itm_map;
    _TH3_ARRAY_MIN(_BCM_TH3_MMU_INFO(unit)->shared_limit, NUM_ITM(unit),
                  itm_map, shared_limit);

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
    *drop_slope = _bcm_th3_cells_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_discard_set
 * Purpose:
 *     Configure port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to configure, -1 for all COS queues
 *     buffer  - (IN) ITM ID
 *     discard - (IN) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_buffer_id_t buffer,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh, flags = 0;
    int cell_size, cell_field_max;
    bcm_port_t local_port = -1;

    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: gport: %d cosq: %d buffer: %d \n"),
            gport, cosq, buffer));
    /* Time Domain */
    /* the reasonable range of refresh_time  is from 1 to 64 */
    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100 ||
        discard->profile_id > 3 ||
        discard->refresh_time <= 0 || discard->refresh_time > 64) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 1 gport: %d cosq: %d\n"),
                gport, cosq));
        return BCM_E_PARAM;
    }

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100 ||
        (discard->min_thresh > discard->max_thresh) ||
        (discard->min_thresh < 0)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 2 gport: %d cosq: %d\n"),
                gport, cosq));
        return BCM_E_PARAM;
    }

    cell_size = _TH3_MMU_BYTES_PER_CELL;
    cell_field_max = 0x7ffff;

    min_thresh = discard->min_thresh;
    max_thresh = discard->max_thresh;
    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert bytes to cells */
        min_thresh += (cell_size - 1);
        min_thresh /= cell_size;

        max_thresh += (cell_size - 1);
        max_thresh /= cell_size;
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: gport: %d cosq: %d min_thres: %d max_thresh: %d\n"),
                gport, cosq, min_thresh, max_thresh));

        if ((min_thresh > cell_field_max) ||
            (max_thresh > cell_field_max)) {
            LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 3 gport: %d cosq: %d\n"),
                gport, cosq));
            return BCM_E_PARAM;
        }
    } else if (discard->flags & BCM_COSQ_DISCARD_PACKETS) {
        /* Packet mode not supported */
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 4 gport: %d cosq: %d\n"),
            gport, cosq));
        return BCM_E_PARAM;
    } else {
        /* Lower layer function will update thresh when these flags are
         * configured, this interface only allow such opration when
         * BCM_COSQ_DISCARD_BYTES is also configured.
         */
        if (discard->flags & (BCM_COSQ_DISCARD_COLOR_ALL |
            BCM_COSQ_DISCARD_TCP |
            BCM_COSQ_DISCARD_NONTCP |
            BCM_COSQ_DISCARD_ECT_MARKED |
            BCM_COSQ_DISCARD_RESPONSIVE_DROP |
            BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 5 gport: %d cosq: %d\n"),
            gport, cosq));
            return BCM_E_PARAM;
        }
    }

    if (gport != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 6 gport: %d cosq: %d\n"),
            gport, cosq));
            return BCM_E_PORT;
        }
        if ((cosq < -1) || (cosq >= _bcm_th3_get_num_ucq(unit))) {
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "bcm_th3_cosq_gport_discard_set: Incorrect Parameter 7 gport: %d cosq: %d\n"),
            gport, cosq));
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_wred_set(unit, gport, cosq, buffer,
                               discard->flags, min_thresh, max_thresh,
                               discard->drop_probability, discard->gain,
                               FALSE, flags, discard->refresh_time,
                               discard->profile_id));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_discard_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to get, -1 for any queue
 *     buffer  - (IN) ITM ID
 *     discard - (OUT) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_buffer_id_t buffer,
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
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
        if ((cosq < -1) || (cosq >= _BCM_TH3_COS_MAX)) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq, buffer,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain,
                               0, &discard->refresh_time,
                               &discard->profile_id));

    /* Convert number of cells to number of bytes */
    discard->min_thresh = min_thresh * _TH3_MMU_BYTES_PER_CELL;
    discard->max_thresh = max_thresh * _TH3_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_wred_resolution_id_create
 * Purpose:
 *     Assign a unused entry. Default drop rules are WRED drop.
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (OUT) entry index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_wred_resolution_entry_create(int unit, int *index)
{
    uint32 i, rval = 0;
    soc_reg_t reg = MMU_WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    for (i = 0; i < _BCM_TH3_NUM_WRED_RESOLUTION_TABLE; i++) {
        if (_bcm_th3_wred_resolution_tbl[i] == 0) {
            /* hw reset value */
            soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, 0xAAA);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, i, rval));
            _bcm_th3_wred_resolution_tbl[i] ++;
            *index = i;
            return SOC_E_NONE;
        }
    }

    return SOC_E_RESOURCE;
}

/*
 * Function:
 *     bcm_th3_cosq_wred_resolution_id_destroy
 * Purpose:
 *     Callback an entry and set its drop rules to WRED drop.
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (IN) entry index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_cosq_wred_resolution_entry_destroy(int unit, int index)
{
    uint32 rval = 0;
    soc_reg_t reg = MMU_WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    if ((index < 0) || (index >= _BCM_TH3_NUM_WRED_RESOLUTION_TABLE)) {
        return SOC_E_PARAM;
    }

    if (_bcm_th3_wred_resolution_tbl[index] == 0) {
        return SOC_E_NOT_FOUND;
    }

    /* hw reset value */
    soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, 0xAAA);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));
    _bcm_th3_wred_resolution_tbl[index] = 0;

    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_wred_resolution_set
 * Purpose:
 *     Set one rule of a resolution table entry
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (IN) entry index
 *     rule                - (IN) drop rule
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     One entry has 12 bits, equals to 6 rules
 */
int
bcm_th3_cosq_wred_resolution_set(int unit, int index, bcm_cosq_discard_rule_t *rule)
{
    uint32 offset, sp, qmin;
    uint32 rval, fval, drop;
    soc_reg_t reg = MMU_WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    if ((index < 0) || (index >= _BCM_TH3_NUM_WRED_RESOLUTION_TABLE)) {
        return SOC_E_PARAM;
    }

    if (_bcm_th3_wred_resolution_tbl[index] == 0) {
        return BCM_E_NOT_FOUND;
    }

    switch (rule->egress_pool_congestion_state) {
        case BCM_COSQ_DISCARD_CONGESTION_LOW:
            sp = 0;
            break;
        case BCM_COSQ_DISCARD_CONGESTION_MEDIUM:
            sp = 1;
            break;
        case BCM_COSQ_DISCARD_CONGESTION_HIGH:
            sp = 2;
            break;
        default:
            return SOC_E_PARAM;
    }
    switch (rule->queue_min_congestion_state) {
        case BCM_COSQ_DISCARD_CONGESTION_LOW:
            qmin = 0;
            break;
        case BCM_COSQ_DISCARD_CONGESTION_HIGH:
            qmin = 1;
            break;
        default:
            return SOC_E_PARAM;
    }

    switch (rule->action) {
        case BCM_COSQ_DISCARD_ACTION_WRED_DROP:
            drop = 0x2;
            break;
        case BCM_COSQ_DISCARD_ACTION_FORCE_DROP:
            drop = 0x3;
            break;
        case BCM_COSQ_DISCARD_ACTION_FORCE_NO_DROP:
            drop = 0x0;
            break;
        default:
            return SOC_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
    fval = soc_reg_field_get(unit, reg, rval, MMU_CONGESTION_EXPERIENCEf);

    offset = (sp * 2 + qmin) * 2;       /* 2 bits per rule */
    fval = (fval & (~(0x3 << offset))) | (drop << offset);

    soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, fval);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_wred_resolution_get
 * Purpose:
 *     Get all rules of a resolution table entry
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (IN) entry index
 *     max                 - (IN) rule array size
 *     rule                - (OUT) rule array
 *     count               - (OUT) actual rules count
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     One entry has 12 bits, equals to 6 rules
 */
int
bcm_th3_cosq_wred_resolution_get(int unit, int index, int max,
                                 bcm_cosq_discard_rule_t *rule, int *count)
{
    uint32 offset, sp, qmin, i = 0;
    uint32 rval, fval, action;
    soc_reg_t reg = MMU_WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;
    uint32 sp_map[3] = {
        BCM_COSQ_DISCARD_CONGESTION_LOW,
        BCM_COSQ_DISCARD_CONGESTION_MEDIUM,
        BCM_COSQ_DISCARD_CONGESTION_HIGH,
    };
    uint32 qmin_map[2] = {
        BCM_COSQ_DISCARD_CONGESTION_LOW,
        BCM_COSQ_DISCARD_CONGESTION_HIGH,
    };
    uint32 action_map[4] = {
        BCM_COSQ_DISCARD_ACTION_FORCE_NO_DROP,
        -1,
        BCM_COSQ_DISCARD_ACTION_WRED_DROP,
        BCM_COSQ_DISCARD_ACTION_FORCE_DROP,
    };
    if ((!count) ||
        (index < 0) || (index >= _BCM_TH3_NUM_WRED_RESOLUTION_TABLE)) {
        return SOC_E_PARAM;
    }

    *count = 3 * 2;

    if (!rule) {
        return SOC_E_NONE;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
    fval = soc_reg_field_get(unit, reg, rval, MMU_CONGESTION_EXPERIENCEf);

    for (sp = 0; sp < 3; sp ++) {
        for (qmin = 0; qmin < 2; qmin ++) {
            offset = (sp * 2 + qmin) * 2;       /* 2 bits per rule */
            action = (fval >> offset) & 0x3;
            if (i < max) {
                rule[i].egress_pool_congestion_state = sp_map[sp];
                rule[i].queue_min_congestion_state = qmin_map[qmin];
                rule[i].action = action_map[action];
                i++;
            }
        }
    }

    return SOC_E_NONE;
}

/* Shaper APIs */
/*
 * Function:
 *     bcm_th3_cosq_gport_bandwidth_get
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
bcm_th3_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 kbits_sec_burst;

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_bucket_get(unit, gport, cosq,
                                 kbits_sec_min, kbits_sec_max,
                                 &kbits_sec_burst, &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_cosq_gport_bandwidth_set
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
bcm_th3_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int i, start_cos=0, end_cos=0, local_port;
    _bcm_th3_cosq_node_t *node = NULL;
    uint32 burst_min, burst_max;

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport,
                                                       &local_port));
    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_node_get(unit, gport, NULL,
                                        NULL, NULL, &node));
            if (node != NULL) {
                start_cos = end_cos = node->hw_index;
            } else {
                cosq = 0;
            }
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            start_cos = end_cos = cosq;
        }
    } else {
        if (cosq < 0) {
            return BCM_E_PARAM;
        }
        start_cos = end_cos = cosq;
    }

    burst_min = (kbits_sec_min > 0) ?
          _bcm_th3_default_burst_size(unit, local_port, kbits_sec_min) : 0;

    burst_max = (kbits_sec_max > 0) ?
          _bcm_th3_default_burst_size(unit, local_port, kbits_sec_max) : 0;

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                         kbits_sec_max, burst_min, burst_max, flags));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_cpu_port_info_dump(int unit)
{
    _bcm_th3_cosq_cpu_port_info_t *port_info;
    bcm_port_t port = 0; /* CPU Port */
    _bcm_th3_cosq_node_t *node;
    int index;
    soc_th3_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    port_info = _bcm_th3_cosq_cpu_port_info[unit];

    LOG_CLI((BSL_META_U(unit,
                        "=== CPU PORT ===\n")));

    LOG_CLI((BSL_META_U(unit,
                        "L0 Nodes\n")));
    for (index = 0; index < _BCM_TH3_NUM_SCHEDULER_PER_PORT; index++) {
        /* Node */
        node = &port_info->sched[index];

        BCM_IF_ERROR_RETURN(
                soc_th3_cosq_sched_mode_get(unit, port, node->level, index,
                                           &sched_mode, &weight, 0));
        LOG_CLI((BSL_META_U(unit,
                            "    LO.%d: GPORT=0x%x MODE=%s WT=%d\n"),
                             index, node->gport,
                             sched_modes[sched_mode], weight));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nMC Queues\n")));
    for (index = 0; index < _BCM_TH3_NUM_CPU_MCAST_QUEUE; index++) {
        int parent_id = 0;
        /* MC Queue */
        node = &port_info->mcast[index];
        BCM_IF_ERROR_RETURN(
                soc_th3_cosq_sched_mode_get(unit, port, node->level, index,
                                           &sched_mode, &weight, 1));
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_node_get(unit, node->parent_gport, NULL, NULL,
                                  &parent_id, NULL));
        LOG_CLI((BSL_META_U(unit,
                            "    MC.%d: GPORT=0x%x MODE=%s WT=%d"
                            " PARENT=L0.%d\n"),
                             index, node->gport,
                             sched_modes[sched_mode], weight,
                             parent_id));
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_port_info_dump(int unit, bcm_port_t port)
{
    _bcm_th3_cosq_port_info_t *port_info;
    _bcm_th3_cosq_node_t *node;
    soc_th3_cosq_node_t *l1_node;
    int index, empty;
    soc_th3_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};
    int l1_parent = -1;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    port_info = &_bcm_th3_cosq_port_info[unit][port];

    empty = TRUE;
    for (index = 0; index < _BCM_TH3_NUM_SCHEDULER_PER_PORT; index++) {
        if (port_info->sched[index].numq > 0) {
            empty = FALSE;
            break;
        }
    }
    if (empty) {
        for (index = 0; index < _bcm_th3_get_num_ucq(unit); index++) {
            if (port_info->ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < _bcm_th3_get_num_mcq(unit); index++) {
            if (port_info->mcast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }

    if (empty == TRUE) {
        return BCM_E_NOT_FOUND;
    }

    LOG_CLI((BSL_META_U(unit,
                        "=== PORT %d\n"), port));

    for (index = 0; index < _BCM_TH3_NUM_SCHEDULER_PER_PORT; index++) {
        /* Node */
        node = &port_info->sched[index];
        if (node->in_use == 0 || node->numq == 0) {
            continue;
        }

        BCM_IF_ERROR_RETURN(
                soc_th3_cosq_sched_mode_get(unit, port, node->level, index,
                                           &sched_mode, &weight, 0));
        LOG_CLI((BSL_META_U(unit,
                            "LO.%d: GPORT=0x%x MODE=%s WT=%d, COS=%d\n"),
                             index, node->gport,
                             sched_modes[sched_mode], weight, node->cos));
    }

    for (index = 0; index < _bcm_th3_get_num_ucq(unit); index++) {
        /* UC Queue */
        int parent_id;
        node = &port_info->ucast[index];
        /* coverity[overrun-local] */
        l1_node =
            th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[index].parent;
        BCM_IF_ERROR_RETURN(soc_th3_cosq_parent_get(unit, port, index,
                    node->level, &l1_parent));
        BCM_IF_ERROR_RETURN(
            soc_th3_cosq_sched_mode_get(unit, port, l1_node->level,
                l1_parent, &sched_mode, &weight, 0));
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_node_get(unit, node->parent_gport, NULL, NULL,
                                  &parent_id, NULL));
        LOG_CLI((BSL_META_U(unit,
                        "UC.%d: GPORT=0x%x MODE=%s WT=%d PARENT=%d\n"),
                         index, node->gport,
                         sched_modes[sched_mode], weight, parent_id));
    }

    for (index = 0; index < _bcm_th3_get_num_mcq(unit); index++) {
        /* MC Queue */
        int parent_id;
        node = &port_info->mcast[index];
        /* coverity[overrun-local] */
        l1_node =
            th3_cosq_mmu_info[unit]->th3_port_info[port].mmuq[(index +
            _bcm_th3_get_num_ucq(unit))].parent;
        BCM_IF_ERROR_RETURN(soc_th3_cosq_parent_get(unit, port,
                    (index + _bcm_th3_get_num_ucq(unit)),
                    node->level, &l1_parent));
        BCM_IF_ERROR_RETURN(
            soc_th3_cosq_sched_mode_get(unit, port, l1_node->level,
                l1_parent, &sched_mode, &weight, 1));
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_node_get(unit, node->parent_gport, NULL, NULL,
                                  &parent_id, NULL));
        LOG_CLI((BSL_META_U(unit,
                        "MC.%d: GPORT=0x%x MODE=%s WT=%d PARENT=%d\n"),
                         index, node->gport,
                         sched_modes[sched_mode], weight, parent_id));
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_th3_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_th3_cosq_sw_dump(int unit)
{
    bcm_port_t port;
    int ii;
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            (void)_bcm_th3_cosq_cpu_port_info_dump(unit);
        }
        (void)_bcm_th3_cosq_port_info_dump(unit, port);
    }
    for (ii = 0; ii <  _BCM_TH3_NUM_TIME_DOMAIN; ii++) {
        LOG_CLI((BSL_META_U(unit,
                            "\nTIME_DOMAIN[%d] - refcnt %d\n"), ii,
                            th3_time_domain_info[unit][ii].ref_count));
    }

    return;
}


/*
 * Function:
 *      _bcm_th3_cosq_port_guarantee_reset
 * Purpose:
 *      Reset the guarantee of priority group, uc queue, mc queue to the default
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_cosq_port_guarantee_reset(int unit, bcm_port_t port)
{
    _soc_mmu_device_info_t devcfg;
    int lossless, override, default_mtu_cells, idx, numq;
    int pg_guarantee, uc_guarantee, mc_guarantee;
    int qgroup_uc_guarantee, qgroup_mc_guarantee;

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    override = soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1);

    _soc_th3_mmu_init_dev_config(unit, &devcfg, lossless);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg.default_mtu_size +
                                                    devcfg.mmu_hdr_byte,
                                                    devcfg.mmu_cell_size);

    /* priority group */
    for (idx = 0; idx < _TH3_MMU_NUM_PG; idx++) {
        pg_guarantee = 0;
        if ((idx == 7) && lossless) {
            pg_guarantee = default_mtu_cells;
        }
        if (!override) {
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_PRIGROUP, idx, spn_GUARANTEE, FALSE,
                 &pg_guarantee, devcfg.mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }
        pg_guarantee *= devcfg.mmu_cell_size;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_res_set(unit, port, idx,
                                      bcmCosqControlIngressPortPGMinLimitBytes,
                                      pg_guarantee));
    }

    /* UC */
    numq = _bcm_th3_get_num_ucq(unit);
    for (idx = 0; idx < numq; idx++) {
        uc_guarantee = 0;
        if (!override) {
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_QUEUE, idx, spn_GUARANTEE, FALSE,
                 &uc_guarantee, devcfg.mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }
        uc_guarantee *= devcfg.mmu_cell_size;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_egr_queue_set(unit, port, idx,
                                       bcmCosqControlEgressUCQueueMinLimitBytes,
                                       uc_guarantee));
    }

    /* MC */
    numq = _bcm_th3_get_num_ucq(unit) + _bcm_th3_get_num_mcq(unit);
    for (idx = _bcm_th3_get_num_ucq(unit); idx < numq; idx++) {
        mc_guarantee = 0;
        if (!override) {
            _soc_mmu_cfg_property_get_cells
                (unit, port, spn_MQUEUE, idx, spn_GUARANTEE, FALSE,
                 &mc_guarantee, devcfg.mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }
        mc_guarantee *= devcfg.mmu_cell_size;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_egr_queue_set(unit, port, idx,
                                       bcmCosqControlEgressMCQueueMinLimitBytes,
                                       mc_guarantee));
    }

    /* QGROUP UC */
    numq = _bcm_th3_get_num_ucq(unit);
    for (idx = 0; idx < numq; idx++) {
        qgroup_uc_guarantee = 2 * default_mtu_cells;
        if (!override) {
            _soc_mmu_cfg_property_get_cells
                (unit, -1, spn_QGROUP, idx, spn_GUARANTEE, FALSE,
                 &qgroup_uc_guarantee, devcfg.mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }
        qgroup_uc_guarantee *= devcfg.mmu_cell_size;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_qgroup_limit_set(unit, port, idx,
                                       bcmCosqControlEgressUCQueueGroupMinLimitBytes,
                                       qgroup_uc_guarantee));
    }

    /* QGROUP MC */
    numq = _bcm_th3_get_num_ucq(unit) + _bcm_th3_get_num_mcq(unit);
    for (idx = _bcm_th3_get_num_ucq(unit); idx < numq; idx++) {
        qgroup_mc_guarantee = 2 * default_mtu_cells;
        if (!override) {
            _soc_mmu_cfg_property_get_cells
                (unit, -1, spn_QGROUP, idx, spn_GUARANTEE_MC, FALSE,
                 &qgroup_mc_guarantee, devcfg.mmu_cell_size,
                 _MMU_DEFAULT_THRES_TYPE);
        }
        qgroup_mc_guarantee *= devcfg.mmu_cell_size;
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_qgroup_limit_set(unit, port, idx,
                                       bcmCosqControlEgressMCQueueGroupMinLimitBytes,
                                       qgroup_mc_guarantee));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_port_detach
 * Purpose:
 *      Clear cosq configuration of port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcm_th3_cosq_port_detach(int unit, bcm_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    uint32 profile_index;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_th3_cosq_port_detach port %d\n"),
              port));

    /* When update guarantee of PG/UC/MC via API, the shared limit is adjusted
     * accordingly. So MUST reset the gurarntee to the default value before
     * deleting the port to recover the shared limit. */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_port_guarantee_reset(unit, port));

    /* Destroy cosq gport tree on port */
    port_info = &_bcm_th3_cosq_port_info[unit][port];
    sal_memset(port_info, 0, sizeof(_bcm_th3_cosq_port_info_t));

    /* Reset soc gport tree on port */
    BCM_IF_ERROR_RETURN(soc_tomahawk3_cosq_port_info_init(unit, port));

    /* Decrease the reference of COS_MAP_PROFILE */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
    profile_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, _bcm_th3_cos_map_profile[unit],
                                profile_index * 16));

    /* Clear cosq */
    si->port_num_cosq[port] = 0;
    si->port_cosq_base[port] = 0;
    si->port_num_uc_cosq[port] = 0;
    si->port_uc_cosq_base[port] = 0;
    si->port_num_ext_cosq[port] = 0;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th3_cosq_port_attach
 * Purpose:
 *      Initialize Cosq for newly added port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcm_th3_cosq_port_attach(int unit, bcm_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    int default_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN, default_weight = 1;
    int i, itm_map, shd_size, pipe;
    int L0[_BCM_TH3_COS_MAX];
    int schedq[_BCM_TH3_COS_MAX];
    int mmuq[_BCM_TH3_COS_MAX];
    int cos_list[_BCM_TH3_COS_MAX];
    soc_reg_t reg;
    uint64 rval_64;
    soc_field_t mmuq_field[12] = {
        MMU_Q_NUM_0f, MMU_Q_NUM_1f,
        MMU_Q_NUM_2f, MMU_Q_NUM_3f,
        MMU_Q_NUM_4f, MMU_Q_NUM_5f,
        MMU_Q_NUM_6f, MMU_Q_NUM_7f,
        MMU_Q_NUM_8f, MMU_Q_NUM_9f,
        MMU_Q_NUM_10f, MMU_Q_NUM_11f,
    };

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }
    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_th3_cosq_port_attach port %d\n"),
              port));

    /* Increase the reference of default COS_MAP_PROFILE */
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf, 0));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_reference(unit, _bcm_th3_cos_map_profile[unit], 0, 0));

    /* set scheduler profile to port 0 */
    reg = MMU_PPSCH_SCHQ_MMUQ_PROFILEr;
    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval_64));

    for (i = 0; i < _BCM_TH3_NUM_SCHEDULER_PER_PORT; i++) {
        L0[i] = L1_TO_L0_MAPPING[unit][0][i];
        schedq[i] = i;
        mmuq[i] = soc_reg64_field32_get(unit, reg, rval_64, mmuq_field[i]);
        cos_list[i] = L0_TO_COSQ_MAPPING[unit][0][i];
    }
    soc_tomahawk3_sched_update_flex_per_port(unit, port, 0, L0, schedq, mmuq, cos_list);

    /* Create gport tree */
    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = FALSE;
    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_gport_tree_create(unit, port));
    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = TRUE;

    /* Set Default Scheduler Configuration on the ports */
    port_info = &_bcm_th3_cosq_port_info[unit][port];
    for (i = 0; i < _BCM_TH3_NUM_SCHEDULER_PER_PORT; i++) {
        if (port_info->sched[i].in_use == 0 || port_info->sched[i].numq == 0) {
            continue;
        }

        BCM_IF_ERROR_RETURN(
                bcm_th3_cosq_gport_sched_set(unit, port_info->sched[i].gport,
                -1, default_mode, default_weight));
    }
    for (i = 0; i < _bcm_th3_get_num_ucq(unit); i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_sched_set(unit, port_info->ucast[i].gport,
                -1, default_mode, default_weight));
    }
    for (i = 0; i < _bcm_th3_get_num_mcq(unit); i++) {
        BCM_IF_ERROR_RETURN
            (bcm_th3_cosq_gport_sched_set(unit, port_info->mcast[i].gport,
                -1, default_mode, default_weight));
    }

    /* Set MMU threshold and scheduler profiles to profile 0
     * Customer can change the profile after flexport sequence is done */
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_cosq_port_profile_set(unit, port,
                       bcmCosqProfilePGProperties, 0));
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_cosq_port_profile_set(unit, port,
                       bcmCosqProfileIntPriToPGMap, 0));
    BCM_IF_ERROR_RETURN(bcm_tomahawk3_cosq_port_profile_set(unit, port,
                       bcmCosqProfilePFCAndQueueHierarchy, 0));

    /* Update shared limit of new ports in case the shared limit has been
     * adjusted by bcm_cosq_control_set() API */
    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, port, &pipe));
    shd_size = 0;
    for (i = 0; i < NUM_ITM(unit); i++) {
        itm_map = si->itm_ipipe_map[i];
        if (itm_map & (1 << pipe)) {
            shd_size = _BCM_TH3_MMU_INFO(unit)->shared_limit[i];
        }
    }

    BCM_IF_ERROR_RETURN(
        soc_th3_mmu_config_flex_thresholds(unit, port, shd_size));

    return BCM_E_NONE;
}

int
bcm_th3_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port;

    if ((int_pri < 0) || (int_pri >= _TH3_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, ing_port,
                                                       &local_port));

    if ((cosq < 0) || (cosq >= _TH3_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }

    return _bcm_th3_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
                                    cosq, cosq);
}

int
bcm_th3_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    if ((int_pri < 0) || (int_pri >= _TH3_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    return _bcm_th3_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

/*Function to return the number of child nodes and child node gport array for
 * scheduler gport */
int
_bcm_th3_cosq_sched_node_child_get(int unit, bcm_gport_t sched_gport, int
        *num_uc_child, int *uc_queue, int *num_mc_child, int *mc_queue)
{
    _bcm_th3_cosq_port_info_t *port_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;

    int local_port, id;
    _bcm_th3_cosq_index_style_t style = _BCM_TH3_COSQ_INDEX_STYLE_COS;
    int num_mc, num_uc;
    int cpu_mc_base;
    soc_info_t *si = &SOC_INFO(unit);

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        /* Function valid only scheduler gport*/
        return BCM_E_INTERNAL;
    }
    if ((num_uc_child == NULL) || (uc_queue == NULL) || (num_mc_child == NULL)
            || (mc_queue == NULL)) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve
            (unit, sched_gport, -1, style, &local_port, NULL, NULL));

    if (!IS_CPU_PORT(unit, local_port)) {
        port_info = &_bcm_th3_cosq_port_info[unit][local_port];
        num_uc = 0;
        num_mc = 0;
        for (id = 0; id < si->port_num_uc_cosq[local_port]; id++) {
            if (port_info->ucast[id].numq == 0) {
                continue;
            }
            if (port_info->ucast[id].parent_gport == sched_gport) {
                uc_queue[num_uc] = port_info->ucast[id].hw_index %
                    si->port_num_uc_cosq[local_port];
                num_uc++;
            }
        }
        for (id = 0; id < si->port_num_cosq[local_port]; id++) {
            if (port_info->mcast[id].numq == 0) {
                continue;
            }
            if (port_info->mcast[id].parent_gport == sched_gport) {
                    mc_queue[num_mc] = port_info->mcast[id].hw_index %
                        si->port_num_cosq[local_port];
                num_mc++;
            }
        }
    } else {
        /* CPU port */
        num_uc = -1;
        num_mc = 0;
        cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
        for (id = 0; id < _BCM_TH3_NUM_CPU_MCAST_QUEUE; id++) {
            if (cpu_port_info->mcast[id].numq == 0) {
                continue;
            }
            if (cpu_port_info->mcast[id].parent_gport == sched_gport) {
                cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                mc_queue[num_mc] = (cpu_port_info->mcast[id].hw_index -
                        cpu_mc_base) % NUM_CPU_COSQ(unit);
                num_mc++;
            }
        }
    }
    *num_mc_child = num_mc;
    *num_uc_child = num_uc;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th3_cosq_field_classifier_get
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
bcm_th3_cosq_field_classifier_get(
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
 *      bcm_th3_cosq_field_classifier_id_create
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
bcm_th3_cosq_field_classifier_id_create(
    int unit,
    bcm_cosq_classifier_t *classifier,
    int *classifier_id)
{
    int rv;
    int ref_count = 0;
    int ent_per_set = _TH3_MMU_NUM_INT_PRI;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < (SOC_MEM_SIZE(unit, IFP_COS_MAPm) - ent_per_set) ; i += ent_per_set) {
        rv = soc_profile_mem_ref_count_get(
                unit, _bcm_th3_ifp_cos_map_profile[unit], i, &ref_count);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
        if (0 == ref_count) {
            break;
        }
    }

    if (i >= (SOC_MEM_SIZE(unit, IFP_COS_MAPm) - ent_per_set) && ref_count != 0) {
        *classifier_id = 0;
        return BCM_E_RESOURCE;
    }

    _BCM_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, (i / ent_per_set));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_field_classifier_map_set
 * Purpose:
 *      Set internal priority to ingress field processor CoS queue
 *      override mapping.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      classifier_id  - (IN) Classifier ID
 *      count          - (IN) Number of elements in priority_array and
 *                            cosq_array.
 *      priority_array - (IN) Array of internal priorities
 *      cosq_array     - (IN) Array of COS queues
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      Supported count value is [0-16] and 48
 *      When count = [0-16]
 *          Array Index [15-0] : Maps Prio[15-0] to UC, MC
 *                               and RQE cos[15-0]
 *      When count = 48
 *      Prio Array Index Map
 *          Array Index [15-0] : Maps Prio[15-0] to UC cos[15-0]
 *          Array Index [31-16] : Maps Prio[15-0] to MC cos[15-0]
 *          Array Index [47-32] : Maps Prio[15-0] to RQE cos[15-0]
 */
int
bcm_th3_cosq_field_classifier_map_set(
    int unit,
    int classifier_id,
    int count,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array)
{
    int rv;
    int i;
    int max_entries = _TH3_MMU_NUM_INT_PRI;
    uint32 index, new_index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_TH3_MMU_NUM_INT_PRI];
    soc_field_t field = -1;
    int min_max_qnum;
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Input parameter check. */
    if (!_BCM_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        return BCM_E_PARAM;
    }

    if (NULL == priority_array || NULL == cosq_array) {
        return BCM_E_PARAM;
    }

    if ((count > max_entries) && (count != (max_entries * 3))) {
        return BCM_E_PARAM;
    }

    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * max_entries);
    entries[0] = ent_buf;

    if (rev_id == BCM56980_A0_REV_ID) {
        min_max_qnum = MIN(MIN(_bcm_th3_get_num_mcq(unit), _bcm_th3_get_num_ucq(unit)),
                           _TH3_A0_MMU_NUM_RQE_QUEUES);
    } else {
        min_max_qnum = MIN(MIN(_bcm_th3_get_num_mcq(unit), _bcm_th3_get_num_ucq(unit)),
                           _TH3_MMU_NUM_RQE_QUEUES);
    }

    if (count != (max_entries * 3)) {
        for (i = 0; i < count; i++) {
            if (priority_array[i] < max_entries) {
                /* When count != max_entries * 3
                   We program the same mapping for all three
                   fields UC/MC/RQE queues.
                   Hence cos_array[i] cannot have value more than
                   equal to MIN(MAX UC_COS, MAX MC_COS, MAX RQE_NUM) */
                if ((cosq_array[i] < 0) ||
                    (cosq_array[i] >= min_max_qnum)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_MC_COSf, cosq_array[i]);
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_UC_COSf, cosq_array[i]);
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_RQE_Q_NUMf, cosq_array[i]);
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (priority_array[i] < max_entries) {
                if (i < max_entries) {
                    /* Number of UCAST queue per port can be 6,8,10,12 based on
                       MCQ mode */
                    if ((cosq_array[i] < 0) ||
                        (cosq_array[i] >= _bcm_th3_get_num_ucq(unit))) {
                        return BCM_E_PARAM;
                    }
                    field = IFP_UC_COSf;
                } else if (i < (max_entries * 2)) {
                    /* Number of MCAST queue per port can be 0,2,4,6 based on
                       MCQ MODE */
                    if ((cosq_array[i] < 0) ||
                        (cosq_array[i] >= _bcm_th3_get_num_mcq(unit))) {
                        return BCM_E_PARAM;
                    }
                    field = IFP_MC_COSf;
                } else {
                    /* Number of RQE queue per unit = 3 */
                    if (rev_id == BCM56980_A0_REV_ID) {
                        if ((cosq_array[i] < 0) ||
                            (cosq_array[i] >= _TH3_A0_MMU_NUM_RQE_QUEUES)) {
                            return BCM_E_PARAM;
                        }
                    } else {
                        if ((cosq_array[i] < 0) ||
                            (cosq_array[i] >= _TH3_MMU_NUM_RQE_QUEUES)) {
                            return BCM_E_PARAM;
                        }
                    }
                    field = IFP_RQE_Q_NUMf;
                }
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    field, cosq_array[i]);
            }
        }
    }

    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    rv = soc_profile_mem_delete(unit, _bcm_th3_ifp_cos_map_profile[unit],
                                index * max_entries);

    /* If rv = SOC_E_NONE means that there exists a
     * mapping for given classifier_id which is now
     * successfully deleted and we can overwrite.
     * rv = SOC_E_NOT_FOUND means there was no mapping
     * for given classifier_id and we can create a new mapping
     * Otherwise there is something else wrong and we return
     * the error value returned by soc_profile_mem_delete()
     */
    if ((rv != SOC_E_NONE) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    SOC_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_th3_ifp_cos_map_profile[unit],
                             entries, max_entries, &new_index));

    /* If we get (index == new_index / max_entries) means we have
     * a successful mapping operation.
     * But if they are not equal which means there already exists
     * a similar mapping associated with other classifier_id hence
     * we return BCM_E_EXISTS.
     */

    if (index != (new_index / max_entries)) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_th3_ifp_cos_map_profile[unit],
                                    new_index));
        return BCM_E_EXISTS;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_field_classifier_map_get
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
 * Notes:
 *      Supported array_max = [0-16] and 48
 *      When array_max = [0-16]
 *          Array Index [15-0] : Maps Prio[15-0] to UC cos[15-0]
 *      When array_max = 48
 *      Prio Array Index Map
 *          Array Index [15-0] : Maps Prio[15-0] to UC cos[15-0]
 *          Array Index [31-16] : Maps Prio[15-0] to MC cos[15-0]
 *          Array Index [47-32] : Maps Prio[15-0] to RQE cos[15-0]
 */
int
bcm_th3_cosq_field_classifier_map_get(
    int unit,
    int classifier_id,
    int array_max,
    bcm_cos_t *priority_array,
    bcm_cos_queue_t *cosq_array,
    int *array_count)
{
    int rv;
    int i;
    int ent_per_set = _TH3_MMU_NUM_INT_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_TH3_MMU_NUM_INT_PRI];
    soc_field_t field = -1;

    /* Input parameter check. */
    if (NULL == priority_array || NULL == cosq_array || NULL == array_count) {
        return BCM_E_PARAM;
    }

    sal_memset(ent_buf, 0, sizeof(ifp_cos_map_entry_t) * ent_per_set);
    entries[0] = ent_buf;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Read out entries from profile table into allocated buffer. */
    rv = soc_profile_mem_get(unit, _bcm_th3_ifp_cos_map_profile[unit],
                             index * ent_per_set, ent_per_set, entries);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (array_max <= ent_per_set) {
        *array_count = array_max;
    } else if ((array_max > ent_per_set) && (array_max < (ent_per_set * 3))) {
        return BCM_E_PARAM;
    } else {
        *array_count = ent_per_set * 3;
    }
        /* Copy values into API OUT parameters. */
    for (i = 0; i < *array_count; i++) {
        if (priority_array[i % 16] >= _TH3_MMU_NUM_INT_PRI) {
            return BCM_E_PARAM;
        }
        if (i < ent_per_set) {
            field = IFP_UC_COSf;
        } else if (i < (ent_per_set * 2)) {
            field = IFP_MC_COSf;
        } else {
            field = IFP_RQE_Q_NUMf;
        }
        cosq_array[i] = soc_mem_field32_get(unit, IFP_COS_MAPm,
                                            &ent_buf[priority_array[i]],
                                            field);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_field_classifier_map_clear
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
bcm_th3_cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    int ent_per_set = _TH3_MMU_NUM_INT_PRI;
    uint32 index;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Delete the profile entries set. */
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_th3_ifp_cos_map_profile[unit],
                                index * ent_per_set));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_field_classifier_id_destroy
 * Purpose:
 *      Free resource associated with this field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th3_cosq_field_classifier_id_destroy(int unit, int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_cosq_field_reserved_classifier_get
 * Purpose:
 *      Get reserved field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (OUT) Classifier ID.
 *      port          - (OUT) Port reserved.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_cosq_field_reserved_classifier_get(int unit, int *classifier_id,
                                            bcm_port_t *port)
{
    _BCM_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, _TH3_IFP_COS_MAP_RSVD_PROF);
    *port = _TH3_MMU_SPARE_DST_PORT;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_cpu_cosq_enable_set
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
bcm_th3_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, enable_status;
    _bcm_th3_cosq_cpu_cosq_config_t *cpu_cosq_config;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
        return BCM_E_PARAM;
    }

    cpu_cosq_config = _bcm_th3_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq_config) {
        return BCM_E_INTERNAL;
    }
    if (enable) {
        enable = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_th3_cosq_cpu_cosq_enable_get(unit, cosq,
                                                         &enable_status));
    if (enable == enable_status) {
        return BCM_E_NONE;
    }

    index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;

    /* To set DB and QE entries */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_THDO_QUEUE_CONFIGm, MEM_BLOCK_ALL, index, &entry));

    /* When enabled, restore the Min and Shared from the cached value */
    if (!enable) {
        cpu_cosq_config->q_min_limit = soc_mem_field32_get(unit,
                                                       MMU_THDO_QUEUE_CONFIGm,
                                                       &entry,
                                                       MIN_LIMITf);
        cpu_cosq_config->q_shared_limit = soc_mem_field32_get(unit,
                                                          MMU_THDO_QUEUE_CONFIGm,
                                                          &entry,
                                                          SHARED_LIMITf);
        cpu_cosq_config->q_limit_dynamic = soc_mem_field32_get(unit,
                                                           MMU_THDO_QUEUE_CONFIGm,
                                                           &entry,
                                                           LIMIT_DYNAMICf);
        cpu_cosq_config->q_limit_enable = soc_mem_field32_get(unit,
                                                          MMU_THDO_QUEUE_CONFIGm,
                                                          &entry,
                                                          LIMIT_ENABLEf);
        cpu_cosq_config->q_color_limit_enable = soc_mem_field32_get(unit,
                                                          MMU_THDO_QUEUE_CONFIGm,
                                                          &entry,
                                                          COLOR_ENABLEf);
    }
    soc_mem_field32_set(unit, MMU_THDO_QUEUE_CONFIGm, &entry, LIMIT_DYNAMICf,
                        enable ? cpu_cosq_config->q_limit_dynamic : 0);
    soc_mem_field32_set(unit, MMU_THDO_QUEUE_CONFIGm, &entry, LIMIT_ENABLEf,
                        enable ? cpu_cosq_config->q_limit_enable : 1);
    soc_mem_field32_set(unit, MMU_THDO_QUEUE_CONFIGm, &entry, COLOR_ENABLEf,
                        enable ? cpu_cosq_config->q_color_limit_enable : 1);
    soc_mem_field32_set(unit, MMU_THDO_QUEUE_CONFIGm, &entry, MIN_LIMITf,
                        enable ? cpu_cosq_config->q_min_limit : 0);
    soc_mem_field32_set(unit, MMU_THDO_QUEUE_CONFIGm, &entry, SHARED_LIMITf,
                        enable ? cpu_cosq_config->q_shared_limit : 0);
    cpu_cosq_config->enable = enable;
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, MMU_THDO_QUEUE_CONFIGm, MEM_BLOCK_ALL, index, &entry));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th3_cosq_cpu_cosq_enable_get
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
bcm_th3_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq, int *enable)
{
    _bcm_th3_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    int index, hw_enable;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t min_limit, shared_limit, limit_enable, dynamic_enable;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_th3_cosq_cpu_cosq_config[unit][cosq];
    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }

    index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;
    hw_enable = 1;

    /* Sync up software record with hardware status. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_THDO_QUEUE_CONFIGm, MEM_BLOCK_ALL,
                                     index, &entry));
    min_limit = soc_mem_field32_get(unit, MMU_THDO_QUEUE_CONFIGm, &entry,
                                    MIN_LIMITf);
    shared_limit = soc_mem_field32_get(unit, MMU_THDO_QUEUE_CONFIGm, &entry,
                                    SHARED_LIMITf);
    limit_enable = soc_mem_field32_get(unit, MMU_THDO_QUEUE_CONFIGm, &entry,
                                    LIMIT_ENABLEf);
    dynamic_enable = soc_mem_field32_get(unit, MMU_THDO_QUEUE_CONFIGm, &entry,
                                    LIMIT_DYNAMICf);
    if ((limit_enable != 0) && (dynamic_enable == 0) &&
        (min_limit == 0) && (shared_limit == 0)) {
        hw_enable = 0;
    }
    cpu_cosq->enable = hw_enable;

    *enable = cpu_cosq->enable;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_cosq_buffer_id_multi_get
 * Purpose:
 *      Get mutliple buffer ids associated with a specified port.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      gport - (IN) Port
 *      cosq - (IN) Reserved field
 *      direction - (IN) Specify ingress or egress direction
 *      array_max - (IN) number of entries to be retrieved
 *      buf_id_array - (OUT) Buffer id array
 *      array_count - (OUT) Actural buffer id count in buf_id_array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq, bcm_cosq_dir_t direction,
                              int array_max, bcm_cosq_buffer_id_t *buf_id_array,
                              int *array_count)
{
    bcm_port_t local_port;
    int pipe, i, idx;
    int itm_map;
    int itm;
    int ibuf_arr[_TH3_ITMS_PER_DEV] = {0};
    int ebuf_arr[_TH3_ITMS_PER_DEV] = {0};

    if ((direction < bcmCosqIngress) ||
        (direction >= bcmCosqDirectionMaxCount)) {
        return BCM_E_PARAM;
    }

    if (buf_id_array == NULL) {
        return BCM_E_PARAM;
    }

    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_SET(gport)) {
        local_port = gport;
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        local_port = BCM_GPORT_LOCAL_GET(gport);
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &local_port));
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_index_resolve(unit, gport, cosq,
            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE, &local_port, &idx, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_cosq_index_resolve(unit, gport, cosq,
            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE, &local_port, &idx, NULL));
    } else {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    if (array_max <= 0) {
        return BCM_E_PARAM;
    } else {
        *array_count = (direction == bcmCosqIngress) ? 1 :
                             MIN(array_max, NUM_ITM(unit));
    }

    itm_map = SOC_INFO(unit).ipipe_itm_map[pipe];
    idx = 0;
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        ebuf_arr[itm] = itm;
        if (itm_map & (1 << itm)) {
            ibuf_arr[idx] = itm;
            idx++;
        }
    }

    for (i = 0; i < *array_count; i++) {
        if (bcmCosqIngress == direction) {
            buf_id_array[i] = ibuf_arr[i];
        } else {
            buf_id_array[i] = ebuf_arr[i];
        }
    }
    for (i = *array_count; i < array_max; i++) {
        buf_id_array[i] = BCM_COSQ_INVALID_BUF_ID;
    }

    return BCM_E_NONE;
}

int bcm_th3_cosq_burst_monitor_dma_config_reset(int unit)
{
    uint32   qcm_app_status[2], qcm_host_status, qcm_uc_status, time_count;
    uint32  *read_addr_ptr, read_addr;
    int      rv = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        uint32 host_mem_size = 0;
        uint32 *host_mem_addr = NULL;

        /* Reset the host memory size and address of micro burst data provided by the QCM App */
        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_get(unit, &host_mem_size, &host_mem_addr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM get host memory message communication failed \n")));
            return rv;
        }

        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_set(unit, 0, 0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM reset host memory message communication failed \n")));
            return rv;
        }

        if (host_mem_addr != NULL) {
            soc_cm_sfree(unit, host_mem_addr);
            host_mem_addr = NULL;
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with CM app srec file
         * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
         */
        if ((qcm_app_status[0] != SHR_QCM_APP_STATUS_1) ||
            (qcm_app_status[1] != SHR_QCM_APP_STATUS_2)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "load CM APP srec file to core 0\n")));
            return BCM_E_UNAVAIL;
        }

        qcm_host_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)));
        if (!(qcm_host_status & SHR_QCM_INIT_STATUS_BIT)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "shared memory not allocated yet\n")));
            return BCM_E_INIT;
        }
        read_addr = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_start_addr)));
        read_addr_ptr = (uint32 *)soc_cm_p2l(unit, read_addr);

        qcm_host_status = (qcm_host_status & (~SHR_QCM_INIT_STATUS_BIT));
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)), (uint32)qcm_host_status);

        time_count = 0;
        while (1) {
            qcm_uc_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(uc_status)));
            qcm_uc_status = qcm_uc_status & SHR_QCM_INIT_STATUS_BIT;
            if (!qcm_uc_status) {
                break;
            }
            ++time_count;
            if ((time_count * QCM_UC_STATUS_TIMEOUT) > QCM_UC_MSG_TIMEOUT) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "CM APP R5 core 0 does not responded\n")));
                return BCM_E_TIMEOUT;
            }
            sal_usleep(QCM_UC_STATUS_TIMEOUT);
        }
        soc_cm_sfree(unit, read_addr_ptr);
    }

    return rv;
}

int bcm_th3_cosq_burst_monitor_dma_config_set(int unit,
                                             uint32 host_mem_size,
                                             uint32 **host_mem_addr)
{
    uint32 qcm_app_status[2], qcm_host_status, qcm_uc_status, shared_mem_size, share_mem_addr, endianess;
    int    time_count, rv = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        /* Sets the host memory size and address of micro burst data provided by the QCM App */

        rv = bcm_th3_cosq_burst_monitor_dma_config_reset(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "QCM reset host memory message communication failed \n")));
            return rv;
        }

        /* Ensure QCM app host memory is not NULL */
        if (host_mem_size) {
            *host_mem_addr = (uint32 *) soc_cm_salloc(unit, host_mem_size, "Host buffer");
            if (*host_mem_addr == NULL) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "host memory address NULL\n")));
                return BCM_E_PARAM;
            }
        }
        sal_memset(*host_mem_addr, 0, host_mem_size);
        share_mem_addr = (uint32)soc_cm_l2p(unit, *host_mem_addr);

        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_set(unit, host_mem_size, share_mem_addr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "QCM set host memory message communication failed \n")));
            return rv;
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with QCM app srec file
         * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
         */
        if ((qcm_app_status[0] != SHR_QCM_APP_STATUS_1) ||
            (qcm_app_status[1] != SHR_QCM_APP_STATUS_2)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "load QCM app srec file to core 0\n")));
            return BCM_E_UNAVAIL;
        }

        qcm_host_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)));
        if (qcm_host_status & SHR_QCM_INIT_STATUS_BIT) {
            if (host_mem_size == 0) {
                rv = bcm_th3_cosq_burst_monitor_dma_config_reset(unit);
                *host_mem_addr = NULL;
                soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_start_addr)), (uint32)0);
                soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_size)), (uint32)0);
                return rv;
            } else {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "already configured\n")));
                return BCM_E_PARAM;
            }
        }

        qcm_host_status = qcm_host_status | SHR_QCM_INIT_STATUS_BIT;
        shared_mem_size = (uint32)sizeof(struct bcm_cosq_burst_output_s);
        shared_mem_size = host_mem_size - (host_mem_size % shared_mem_size);

        if (shared_mem_size < SHR_QCM_MEM_MIN_SIZE) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "except's minimum shared memory size : %x\n"),
                      SHR_QCM_MEM_MIN_SIZE));
            return BCM_E_PARAM;
        }

        /* Ensure QCM app host memory is not NULL */
        *host_mem_addr = (uint32 *) soc_cm_salloc(unit, host_mem_size, "Host buffer");
        if (*host_mem_addr == NULL) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "host memory address NULL\n")));
            return BCM_E_PARAM;
        }

        sal_memset(*host_mem_addr, 0, host_mem_size);
        share_mem_addr = (uint32)soc_cm_l2p(unit, *host_mem_addr);

        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_start_addr)), (uint32)share_mem_addr);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_size)), (uint32)shared_mem_size);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(drop_table_index)),
                         (uint32)soc_mem_base(unit, MMU_THDO_QUEUE_DROP_COUNT_ITM0m));
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(drop_table_index) + 4),
                         (uint32)soc_mem_base(unit, MMU_THDO_QUEUE_DROP_COUNT_ITM1m));

        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(egr_table_index)),
                         (uint32)soc_mem_base(unit, EGR_PERQ_XMT_COUNTERSm));

        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(queues_per_pipe)),
                         (uint32)SOC_TH3_NUM_COS_QUEUES_PER_PIPE);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(queues_per_port)),
                         (uint32)SOC_TH3_COS_MAX);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(ports_per_pipe)),
                         (uint32)SOC_TH3_MAX_DEV_PORTS_PER_PIPE);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(num_pipes)),
                         (uint32)_TH3_PIPES_PER_DEV);
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(num_counters_per_pipe)),
                         (uint32)(NUM_ITM(unit)));
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)), (uint32)qcm_host_status);

        time_count = 0;
        while (1) {
            qcm_uc_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(uc_status)));
            qcm_uc_status = qcm_uc_status & SHR_QCM_INIT_STATUS_BIT;
            if (qcm_host_status == qcm_uc_status) {
                break;
            }
            ++time_count;
            if ((time_count * QCM_UC_STATUS_TIMEOUT) > QCM_UC_MSG_TIMEOUT) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "QCM app R5 core 0 does not responded\n")));
                return BCM_E_TIMEOUT;
            }
            sal_usleep(QCM_UC_STATUS_TIMEOUT);
        }
        if (*host_mem_addr[0] == 0x1000000) {
            endianess = 1;
        } else {
            endianess = 0;
        }
        soc_uc_mem_write(unit, (SRAM_BASEADDR(unit) + sram_offset(endianess)), (uint32)endianess);
    }

    return rv;
}

int bcm_th3_cosq_burst_monitor_dma_config_get(int unit,
                                             uint32 *host_mem_size,
                                             uint32 **host_mem_addr)
{
    uint32  qcm_app_status[2], qcm_host_status;
    uint32  host_addr;
    int rv = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        /* Gets the host memory size and address of micro burst data provided by the QCM App */
        rv = _bcm_xgs5_ft_qcm_eapp_host_buf_get(unit, host_mem_size, host_mem_addr);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "QCM get host memory message communication failed \n")));
            return rv;
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with CM app srec file
         *          * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
 */
        if ((qcm_app_status[0] != SHR_QCM_APP_STATUS_1) || (qcm_app_status[1] != SHR_QCM_APP_STATUS_2)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "load CM APP srec file to core 0\n")));
            return BCM_E_UNAVAIL;
        }

        qcm_host_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)));
        if (!(qcm_host_status & SHR_QCM_INIT_STATUS_BIT)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "shared memory not allocated yet\n")));
            return BCM_E_INIT;
        }
        host_addr = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_start_addr)));
        *host_mem_size = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_size)));
        *host_mem_addr = (uint32 *)soc_cm_p2l(unit, host_addr);
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_bcm_th3_cosq_rx_queue_channel_reinit(int unit)
{
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    int idx;
    /*
    * L0.0 is associated with CMC0 CH1(Rx)
    * L0.1 is associated with CMC0 CH2(Rx)
    * L0.2 is associated with CMC0 CH3(Rx)
    * L0.3 is associated with CMC0 CH4(Rx)
    * L0.4 is associated with CMC0 CH5(Rx)
    * L0.5 is associated with CMC0 CH6(Rx)
    * L0.6 is associated with CMC0 CH7(Rx)
    */
    bcm_gport_t sched_gport[7];
    bcm_cos_queue_t parent_cos;

    cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }

    for (idx = 0; idx < 7; idx++) {
        sched_gport[idx] = cpu_port_info->sched[idx].gport;
    }

    for (idx = 0; idx < _BCM_TH3_NUM_CPU_MCAST_QUEUE; idx++) {
        parent_cos = -1;
        if (cpu_port_info->mcast[idx].parent_gport == sched_gport[0]) {
            parent_cos = 0;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[1]) {
            parent_cos = 1;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[2]) {
            parent_cos = 2;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[3]) {
            parent_cos = 3;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[4]) {
            parent_cos = 4;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[5]) {
            parent_cos = 5;
        } else if (cpu_port_info->mcast[idx].parent_gport == sched_gport[6]) {
            parent_cos = 6;
        }
        if ((parent_cos >= 0) && (parent_cos < 7)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_rx_queue_channel_set(unit, parent_cos, idx, 1));
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT
int
bcm_th3_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    int *int_scache_ptr = NULL;
    uint16 *u16_scache_ptr = NULL;
    uint32 *u32_scache_ptr = NULL;
    _bcm_th3_mmu_info_t *mmu_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    int alloc_size = 0, i, j, pipe = -1, itm;
    soc_profile_mem_t *profile_mem;
    soc_mem_t wred_config_mem = INVALIDm;
    int index, profile_index, resolution_index;
    int stable_size, rv, num, set_index;
    mmu_wred_config_entry_t *qentry;
    uint8 * mmu_wred_config_buf[8][2];
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_port_t port;
    _bcm_bst_device_handlers_t bst_callbks;
    
    _bcm_th3_cosq_cpu_cosq_config_t *cpu_cosq_config_p;
    uint16 recovered_ver = 0;
    int index_min, index_max;
    int additional_scache_sz = 0;
    int profile_idx, qidx;
    uint32  qcm_app_status[2], host_mem_size;
    uint32  *host_mem_addr;
    uint32 pipe_map;
    int ii;
    int itm_map;
    uint16 dev_id;
    uint8  rev_id;
    uint8  qcm_enable = 0;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    itm_map = SOC_INFO(unit).itm_map;

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

    mmu_info = _bcm_th3_mmu_info[unit];

    if (mmu_info == NULL) {
        return BCM_E_INIT;
    }

    cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }

    /* Update mmu_info shared limit parameters */
    int_scache_ptr = (int *) scache_ptr;
    /* shared limits are now recovered from hw */
    int_scache_ptr += 1;
    BCM_IF_ERROR_RETURN(
        soc_th3_mmu_get_shared_size(unit,
             mmu_info->shared_limit));

    alloc_size = sizeof(_bcm_th3_cosq_cpu_port_info_t);
    scache_ptr = (uint8*) int_scache_ptr;
    sal_memcpy(cpu_port_info, scache_ptr, alloc_size);

    /* Restore the Queue to Channel Mapping */
    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_rx_queue_channel_reinit(unit));

    scache_ptr += alloc_size;

    /* Update IFP_COS_MAP memory profile reference counter */
    profile_mem = _bcm_th3_ifp_cos_map_profile[unit];
    num = SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _TH3_MMU_NUM_INT_PRI - 1;
    u16_scache_ptr = (uint16 *)scache_ptr;
    for (i = 0; i < num; i++) {
        for (j = 0; j < *u16_scache_ptr; j++) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, profile_mem,
                                          i * _TH3_MMU_NUM_INT_PRI,
                                          _TH3_MMU_NUM_INT_PRI));
        }
        u16_scache_ptr++;
    }

    /* Recover CPU queue config information from scache */
    cpu_cosq_config_p = _bcm_th3_cosq_cpu_cosq_config[unit][0];
    if (cpu_cosq_config_p == NULL) {
        return BCM_E_INIT;
    }
    alloc_size = (SOC_TH3_NUM_CPU_QUEUES *
                   sizeof(_bcm_th3_cosq_cpu_cosq_config_t));
    scache_ptr = (uint8*) u16_scache_ptr;
    sal_memcpy(cpu_cosq_config_p, scache_ptr, alloc_size);
    scache_ptr += alloc_size;

    /* Retriving num_cos value */
    int_scache_ptr = (int *) scache_ptr;
    _TH3_NUM_COS(unit) = *int_scache_ptr++;
    scache_ptr = (uint8*) int_scache_ptr;

    /*
     * Retriving th3_sched_profile_info struture for all scheduler profiles
     * size = sizeof(soc_th3_sched_profile_info_t) * SOC_TH3_MAX_NUM_SCHED_PROFILE * SOC_TH3_NUM_GP_QUEUES;
     */
    int_scache_ptr = (int*) scache_ptr;
    for (profile_idx = 0; profile_idx < SOC_TH3_MAX_NUM_SCHED_PROFILE; profile_idx++) {
        for (qidx = 0; qidx < SOC_TH3_NUM_GP_QUEUES; qidx++) {
            th3_sched_profile_info[unit][profile_idx][qidx].cos = *int_scache_ptr++;
            th3_sched_profile_info[unit][profile_idx][qidx].mmuq[0] = *int_scache_ptr++;
            th3_sched_profile_info[unit][profile_idx][qidx].mmuq[1] = *int_scache_ptr++;
            th3_sched_profile_info[unit][profile_idx][qidx].strict_priority = *int_scache_ptr++;
            th3_sched_profile_info[unit][profile_idx][qidx].fc_is_uc_only = *int_scache_ptr++;
        }
    }
    scache_ptr = (uint8 *)int_scache_ptr;

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_scheduler_reinit(unit));

    /* PFC Deadlock recovery should be initialized
       irrespective of scache version to handle
       upgrades. */
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_pfc_deadlock_init(unit));
        BCM_IF_ERROR_RETURN(_bcm_th3_pfc_deadlock_reinit(unit, &scache_ptr,
                                                         recovered_ver));
        BCM_IF_ERROR_RETURN(_bcm_th3_pfc_deadlock_recovery_reset(unit));
    }

    /* Update PRIO2COS_PROFILE register profile reference counter */
    

    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_th3_cos_map_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * _TH3_MMU_NUM_INT_PRI,
                       _TH3_MMU_NUM_INT_PRI));
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL,
                                         SOC_INFO(unit).cpu_hg_index, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * _TH3_MMU_NUM_INT_PRI,
                              _TH3_MMU_NUM_INT_PRI));
    }

    index_min = soc_mem_index_min(unit, MMU_WRED_QUEUE_CONFIGm);
    index_max = soc_mem_index_max(unit, MMU_WRED_QUEUE_CONFIGm);
    /* index_max = soc_mem_index_max(unit, SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit,
                                                MMU_WRED_QUEUE_CONFIGm, 0, 0));*/

    wred_config_mem = MMU_WRED_QUEUE_CONFIGm;

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            if (!(itm_map & 1 << itm)) {
                continue;
            }
                mmu_wred_config_buf[pipe][itm] = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit,
                                                    wred_config_mem),
                                                    "mmu_wred");
                BCM_IF_ERROR_RETURN
                        (soc_tomahawk3_itm_mem_read_range(unit, wred_config_mem,
                            itm, pipe, MEM_BLOCK_ALL, index_min, index_max, mmu_wred_config_buf[pipe][itm]));
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            if (!(itm_map & 1 << itm)) {
                continue;
            }
            if (pipe_map & (1 << pipe)) {
                for (index = index_min; index < index_max; index++) {

                    qentry = soc_mem_table_idx_to_pointer(unit, wred_config_mem, mmu_wred_config_entry_t *,
                            mmu_wred_config_buf[pipe][itm], index);

                    profile_index = soc_mem_field32_get(unit, wred_config_mem,
                                                    qentry, PROFILE_INDEXf);
                    rv = (soc_profile_mem_reference
                        (unit, _bcm_th3_wred_profile[unit], profile_index, 1));

                    if (SOC_FAILURE(rv)) {
                        goto cleanup;
                    }

                    if (soc_mem_field32_get(unit, wred_config_mem, qentry,
                        ECN_MARKING_ENf)) {
                        resolution_index = soc_mem_field32_get(
                            unit, wred_config_mem, qentry,
                            WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf);
                    } else {
                        resolution_index = soc_mem_field32_get(
                            unit, wred_config_mem, qentry,
                            WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf);
                    }
                    _bcm_th3_wred_resolution_tbl[resolution_index] = 1;
                }
            }
        }
    }
cleanup:
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
            if (!(itm_map & 1 << itm)) {
                continue;
            }
            if (mmu_wred_config_buf[pipe][itm]) {
                soc_cm_sfree(unit, mmu_wred_config_buf[pipe][itm]);
                mmu_wred_config_buf[pipe][itm] = NULL;
            }
        }
    }
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* Create gport tree */
    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = FALSE;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_gport_tree_create(unit, port));
    }

    _BCM_TH3_MMU_INFO(unit)->gport_tree_created = TRUE;

    /* BST reinit */
    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &bcm_bst_th3_index_resolve;
    bst_callbks.reverse_resolve_index =
        &_bcm_th3_cosq_bst_map_resource_to_gport_cos;

    BCM_IF_ERROR_RETURN(_bcm_bst_attach(unit, &bst_callbks));

#if defined(INCLUDE_PSTATS)
    if (soc_feature(unit, soc_feature_pstats)) {
        rv = _bcm_pstats_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_th3_cosq_cleanup(unit);
            return rv;
        }
    }
#endif /* INCLUDE_PSTATS */

    /* Recovery TIME_DOMAINr count reference */
    for (ii = 0; ii <  _BCM_TH3_NUM_TIME_DOMAIN; ii++) {
        alloc_size =  sizeof(uint32);
        sal_memcpy(&th3_time_domain_info[unit][ii].ref_count, scache_ptr, alloc_size);
        scache_ptr += alloc_size;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_ecn_wred)) {
        BCM_IF_ERROR_RETURN(bcmi_xgs5_ecn_reinit(unit, &scache_ptr));
    }
#endif


    /* Retriving _bcm_th3_wred_resolution_tbl value */
    u32_scache_ptr = (uint32 *) scache_ptr;
    for (i = 0; i < _BCM_TH3_NUM_WRED_RESOLUTION_TABLE; i++) {
        _bcm_th3_wred_resolution_tbl[i] = *u32_scache_ptr++;
    }

    /* realloc scache_handle resource*/
    if (additional_scache_sz != 0) {
        rv = soc_scache_realloc(unit, scache_handle, additional_scache_sz);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

#if defined(INCLUDE_FLOWTRACKER)
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
#endif /* INCLUDE_FLOWTRACKER */
    if (!qcm_enable) {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with QCM app srec file
         * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
         */
        if ((qcm_app_status[0] == SHR_QCM_APP_STATUS_1) &&
            (qcm_app_status[1] == SHR_QCM_APP_STATUS_2)) {
            host_mem_size = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_buf_size)));
            if (host_mem_size >= SHR_QCM_MEM_MIN_SIZE) {
                bcm_th3_cosq_burst_monitor_dma_config_set(unit, host_mem_size, &host_mem_addr);
            }
        }
    }

    return BCM_E_NONE;
}

int
bcm_th3_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    uint32 *u32_scache_ptr;
    uint16 *u16_scache_ptr;
    int *int_scache_ptr;
    _bcm_th3_mmu_info_t *mmu_info = NULL;
    _bcm_th3_cosq_cpu_port_info_t *cpu_port_info = NULL;
    int size = 0, ii;
    int shared_limit_min;
    int itm_map;
    int rv = BCM_E_NONE, ref_count, val = 0, pri, port;
    _bcm_th3_cosq_cpu_cosq_config_t **cpu_cosq_config_p;
    int profile_idx, qidx;
    uint32  qcm_app_status[2], qcm_host_status;
    bcm_cosq_pfc_dd_recovery_mode_t recovery_mode;

    mmu_info = _bcm_th3_mmu_info[unit];
    if (mmu_info == NULL) {
        return BCM_E_INIT;
    }

    cpu_port_info = _bcm_th3_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    /* Cache minimal ing_shared_limit/egr_db_shared_limit/egr_qe_shared_limit */
    itm_map = SOC_INFO(unit).itm_map;
    _TH3_ARRAY_MIN(_BCM_TH3_MMU_INFO(unit)->shared_limit, NUM_ITM(unit),
                  itm_map, shared_limit_min);
    u32_scache_ptr = (uint32*) scache_ptr;
    *u32_scache_ptr++ = shared_limit_min;

    /* Cache _bcm_th3_cosq_cpu_port_info_t structure */
    size = sizeof(_bcm_th3_cosq_cpu_port_info_t);
    scache_ptr = (uint8*)u32_scache_ptr;
    sal_memcpy(scache_ptr, cpu_port_info, size);
    scache_ptr += size/sizeof(uint8);

    /* Cache _bcm_th3_ifp_cos_map_profile reference count */
    u16_scache_ptr = (uint16 *)scache_ptr;
    for (ii = 0;
         ii < ((SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _TH3_MMU_NUM_INT_PRI) - 1);
         ii++) {
        rv = soc_profile_mem_ref_count_get(unit,
                                           _bcm_th3_ifp_cos_map_profile[unit],
                                           ii * _TH3_MMU_NUM_INT_PRI,
                                           &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }

    /* Sync CPU Queue info */
    cpu_cosq_config_p = _bcm_th3_cosq_cpu_cosq_config[unit];
    size = (SOC_TH3_NUM_CPU_QUEUES * sizeof(_bcm_th3_cosq_cpu_cosq_config_t));
    scache_ptr = (uint8*) u16_scache_ptr;
    sal_memcpy(scache_ptr, *cpu_cosq_config_p, size);
    scache_ptr += (size / sizeof(uint8));

    /* Sync num_cos value */
    int_scache_ptr = (int*) scache_ptr;
    *int_scache_ptr++ = _TH3_NUM_COS(unit);
    scache_ptr = (uint8 *)int_scache_ptr;

    /*
     * Sync th3_sched_profile_info struture for all scheduler profiles
     * size = sizeof(soc_th3_sched_profile_info_t) * SOC_TH3_MAX_NUM_SCHED_PROFILE * SOC_TH3_NUM_GP_QUEUES;
     */
    int_scache_ptr = (int*) scache_ptr;
    for (profile_idx = 0; profile_idx < SOC_TH3_MAX_NUM_SCHED_PROFILE; profile_idx++) {
        for (qidx = 0; qidx < SOC_TH3_NUM_GP_QUEUES; qidx++) {
            *int_scache_ptr++ = th3_sched_profile_info[unit][profile_idx][qidx].cos;
            *int_scache_ptr++ = th3_sched_profile_info[unit][profile_idx][qidx].mmuq[0];
            *int_scache_ptr++ = th3_sched_profile_info[unit][profile_idx][qidx].mmuq[1];
            *int_scache_ptr++ = th3_sched_profile_info[unit][profile_idx][qidx].strict_priority;
            *int_scache_ptr++ = th3_sched_profile_info[unit][profile_idx][qidx].fc_is_uc_only;
        }
    }
    scache_ptr = (uint8 *)int_scache_ptr;

    /* Sync PFC deadlock recovery timer period. */
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        u16_scache_ptr = (uint16 *)scache_ptr;
        for (port = 0; port <  SOC_MAX_NUM_PORTS; port++) {
            for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
                if (bcm_tomahawk3_cosq_pfc_deadlock_control_get(unit,
                                                      port, pri,
                                                      bcmCosqPFCDeadlockRecoveryTimer,
                                                      &val) == BCM_E_NONE) {
                    *u16_scache_ptr++ = (uint16) val;
                } else {
                    *u16_scache_ptr++ = 0;
                }
            }
        }
    }
    scache_ptr = (uint8*)u16_scache_ptr;
    u32_scache_ptr = (uint32*) scache_ptr;

    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        u32_scache_ptr = (uint32 *)scache_ptr;
        for (port = 0; port <  SOC_MAX_NUM_PORTS; port++) {
            for (pri = 0; pri < TH3_PFC_PRIORITY_NUM; pri++) {
                 if (bcm_tomahawk3_cosq_pfc_deadlock_control_get(unit,
                                            port, pri,
                                            bcmCosqPFCDeadlockRecoveryOccurrences,
                                            &val) == BCM_E_NONE) {
                    *u32_scache_ptr++ = val;
                } else {
                    *u32_scache_ptr++ = 0;
                }
            }
        }
    }
    scache_ptr = (uint8*)u32_scache_ptr;

	/* Sync PFC deadlock recovery mode */
    if ((soc_feature(unit, soc_feature_pfc_deadlock)) &&
        (BCM_WB_DEFAULT_VERSION >= BCM_WB_VERSION_1_1)) {
        u16_scache_ptr = (uint16 *)scache_ptr;
        for (port = 0; port <  SOC_MAX_NUM_PORTS; port++) {
                if (bcm_tomahawk3_cosq_pfc_dd_recovery_mode_get
                        (unit, port, &recovery_mode) == BCM_E_NONE) {
                    if (recovery_mode == bcmCosqPfcDdRecoveryModePFCState) {
                        val = 1;
                    } else {
                        val = 0;
                    }
                    *u16_scache_ptr++ = (uint16) val;
                } else {
                    *u16_scache_ptr++ = 0;
                }
        }
        scache_ptr = (uint8*)u16_scache_ptr;
    }

    /* Sync TIME_DOMAINr field and count reference */
    u32_scache_ptr = (uint32*) scache_ptr;
    size = sizeof(uint32);
    for (ii = 0; ii <  _BCM_TH3_NUM_TIME_DOMAIN; ii++) {
        sal_memcpy(u32_scache_ptr, &th3_time_domain_info[unit][ii].ref_count, size);
        u32_scache_ptr++;
    }

    scache_ptr = (uint8*)u32_scache_ptr;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_ecn_wred)) {
        BCM_IF_ERROR_RETURN(bcmi_xgs5_ecn_sync(unit, &scache_ptr));
    }
#endif

    u32_scache_ptr = (uint32*) scache_ptr;
    for (ii = 0; ii < _BCM_TH3_NUM_WRED_RESOLUTION_TABLE; ii++) {
        *u32_scache_ptr++ = _bcm_th3_wred_resolution_tbl[ii];
    }
    scache_ptr = (uint8 *)u32_scache_ptr;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        bcm_th3_cosq_burst_monitor_dma_config_reset(unit);
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with QCM app srec file
         * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
         */
        if ((qcm_app_status[0] == SHR_QCM_APP_STATUS_1) &&
            (qcm_app_status[1] == SHR_QCM_APP_STATUS_2)) {

            qcm_host_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)));
            if (qcm_host_status & SHR_QCM_INIT_STATUS_BIT) {
                bcm_th3_cosq_burst_monitor_dma_config_reset(unit);
            }
        }
    }

    return BCM_E_NONE;
}

int
_bcm_th3_cosq_scheduler_reinit(int unit)
{
    _soc_mmu_cfg_scheduler_profile_t *sched_profile;
    int idx, cosq_idx,profile_idx;
    int L0[_BCM_TH3_COS_MAX];
    int schedq[_BCM_TH3_COS_MAX];
    int mmuq[_BCM_TH3_COS_MAX];
    int cos_list[_BCM_TH3_COS_MAX];
    int strict_priority[_BCM_TH3_COS_MAX];
    int fc_is_uc_only[_BCM_TH3_COS_MAX];
    int rv = BCM_E_NONE;

    sched_profile = soc_mmu_cfg_scheduler_profile_alloc(unit);

    for (profile_idx = 0; profile_idx < _BCM_TH3_NUM_QSCHED_PROFILES; profile_idx++) {
        cosq_idx = 0;
        for (idx = 0; idx < _BCM_TH3_COS_MAX; idx++) {
            sched_profile[profile_idx].num_unicast_queue[idx] = 0;
            sched_profile[profile_idx].num_multicast_queue[idx] = 0;
            sched_profile[profile_idx].strict_priority[idx] = 0;
            sched_profile[profile_idx].flow_control_only_unicast[idx] = 0;
        }
        for (idx = 0; idx < _BCM_TH3_COS_MAX; idx++) {
            if (th3_sched_profile_info[unit][profile_idx][idx].cos == -1) {
                continue;
            }
            if (th3_sched_profile_info[unit][profile_idx][idx].mmuq[0] != -1) {
                if (th3_sched_profile_info[unit][profile_idx][idx].mmuq[0] <
                    _bcm_th3_get_num_ucq(unit)) {
                    sched_profile[profile_idx].num_unicast_queue[cosq_idx]++;
                }
                else {
                    sched_profile[profile_idx].num_multicast_queue[cosq_idx]++;
                }

            }
            if (th3_sched_profile_info[unit][profile_idx][idx].mmuq[1] != -1) {
                if (th3_sched_profile_info[unit][profile_idx][idx].mmuq[1] <
                    _bcm_th3_get_num_ucq(unit)) {
                    sched_profile[profile_idx].num_unicast_queue[cosq_idx]++;
                }
                else {
                    sched_profile[profile_idx].num_multicast_queue[cosq_idx]++;
                }

            }
            sched_profile[profile_idx].strict_priority[cosq_idx] =
                th3_sched_profile_info[unit][profile_idx][idx].strict_priority;
            sched_profile[profile_idx].flow_control_only_unicast[cosq_idx] =
                th3_sched_profile_info[unit][profile_idx][idx].fc_is_uc_only;
            cosq_idx++;
        }
        rv = _soc_mmu_tomahawk3_scheduler_profile_config_check(unit,
                        profile_idx, sched_profile);

        if (rv != BCM_E_NONE) {
            goto exit;
        }

        rv = _soc_scheduler_profile_mapping_setup(unit,
                        sched_profile, profile_idx, L0, schedq, mmuq, cos_list,
                        strict_priority, fc_is_uc_only);

        if (rv != BCM_E_NONE) {
            goto exit;
        }

        /*initilization of L0.i->schedq.i->mmuq.i*/
        LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit, "Profile %d cos %d L0 %d schedq %d mmuq %d cos_list %d\n"),
             profile_idx, cosq_idx, L0[cosq_idx], schedq[cosq_idx], mmuq[cosq_idx], cos_list[cosq_idx]));
        rv = soc_tomahawk3_sched_update_reinit(unit, profile_idx, L0, schedq, mmuq, cos_list, strict_priority, fc_is_uc_only);
        if (rv != BCM_E_NONE) {
            goto exit;
        }

    }

exit:
    soc_mmu_cfg_scheduler_profile_free(unit, sched_profile);
    return rv;;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

int bcm_tomahawk3_cosq_burst_monitor_init(int unit)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        result = bcm_esw_flowtracker_init(unit);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM FT init failed \n")));
            return result;
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            return result;
        } else
#endif /* BCM_WARM_BOOT_SUPPORT */
        {
            _bcm_esw_ft_qcm_init_cfg_t cfg_init;
            sal_memset(&cfg_init, 0, sizeof(_bcm_esw_ft_qcm_init_cfg_t));

            /* 276 queues pipe-0, start index 0 */
            cfg_init.egr_index[0]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE0m,
                                             0, 0, 0, &cfg_init.acc_type[0]);
            cfg_init.util_index[0]     = soc_mem_addr_get(unit,
                                             MMU_THDO_COUNTER_QUEUE_ITM0m,
                                             0, 0, 0, &cfg_init.acc_type[0]);

            cfg_init.drop_index[0]     = soc_mem_addr_get(unit,
                                             MMU_THDO_QUEUE_DROP_COUNT_ITM0m,
                                             0, 0, 0, &cfg_init.acc_type[0]);

            /* 240 queues pipe-1, start index 276 */
            cfg_init.egr_index[1]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE1m,
                                             0, 0, 0, &cfg_init.acc_type[1]);
            cfg_init.util_index[1]     = cfg_init.util_index[0] + 276;
            cfg_init.drop_index[1]     = cfg_init.drop_index[0] + 276;
            cfg_init.acc_type[1]       = cfg_init.acc_type[0];

            /* 228 queues pipe-2, start index 516 */
            cfg_init.egr_index[2]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE2m,
                                             0, 0, 0, &cfg_init.acc_type[2]);
            cfg_init.util_index[2]     = cfg_init.util_index[1] + 240;
            cfg_init.drop_index[2]     = cfg_init.drop_index[1] + 240;
            cfg_init.acc_type[2]       = cfg_init.acc_type[0];

            /* 228 queues pipe-3, start index 744 */
            cfg_init.egr_index[3]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE3m,
                                             0, 0, 0, &cfg_init.acc_type[3]);
            cfg_init.util_index[3]     = cfg_init.util_index[2] + 228;
            cfg_init.drop_index[3]     = cfg_init.drop_index[2] + 228;
            cfg_init.acc_type[3]       = cfg_init.acc_type[0];

            /* 228 queues pipe-4, start index 972 */
            cfg_init.egr_index[4]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE4m,
                                             0, 0, 0, &cfg_init.acc_type[4]);
            cfg_init.util_index[4]     = cfg_init.util_index[3] + 228;
            cfg_init.drop_index[4]     = cfg_init.drop_index[3] + 228;
            cfg_init.acc_type[4]       = cfg_init.acc_type[0];

            /* 240 queues pipe-5, start index 1200 */
            cfg_init.egr_index[5]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE5m,
                                             0, 0, 0, &cfg_init.acc_type[5]);
            cfg_init.util_index[5]     = cfg_init.util_index[4] + 228;
            cfg_init.drop_index[5]     = cfg_init.drop_index[4] + 228;
            cfg_init.acc_type[5]       = cfg_init.acc_type[0];

            /* 240 queues pipe-6, start index 1440 */
            cfg_init.egr_index[6]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE6m,
                                             0, 0, 0, &cfg_init.acc_type[6]);
            cfg_init.util_index[6]     = cfg_init.util_index[5] + 240;
            cfg_init.drop_index[6]     = cfg_init.drop_index[5] + 240;
            cfg_init.acc_type[6]       = cfg_init.acc_type[0];

            /* 240 queues pipe-7, start index 1680 */
            cfg_init.egr_index[7]      = soc_mem_addr_get(unit,
                                             EGR_PERQ_XMT_COUNTERS_PIPE7m,
                                             0, 0, 0, &cfg_init.acc_type[7]);
            cfg_init.util_index[7]     = cfg_init.util_index[6] + 240;
            cfg_init.drop_index[7]     = cfg_init.drop_index[6] + 240;
            cfg_init.acc_type[7]       = cfg_init.acc_type[0];

            cfg_init.util_index[8]     = soc_mem_addr_get(unit,
                                             MMU_THDO_COUNTER_QUEUE_ITM1m,
                                             0, 0, 0, &cfg_init.acc_type[8]);

            cfg_init.drop_index[8]     = soc_mem_addr_get(unit,
                                             MMU_THDO_QUEUE_DROP_COUNT_ITM1m,
                                             0, 0, 0, &cfg_init.acc_type[8]);

            /* 240 queues pipe-1, start index 276 */
            cfg_init.util_index[9]     = cfg_init.util_index[8] + 276;
            cfg_init.drop_index[9]     = cfg_init.drop_index[8] + 276;
            cfg_init.acc_type[9]       = cfg_init.acc_type[8];

            /* 228 queues pipe-2, start index 516 */
            cfg_init.util_index[10]    = cfg_init.util_index[9] + 240;
            cfg_init.drop_index[10]    = cfg_init.drop_index[9] + 240;
            cfg_init.acc_type[10]      = cfg_init.acc_type[8];

            /* 228 queues pipe-3, start index 744 */
            cfg_init.util_index[11]    = cfg_init.util_index[10] + 228;
            cfg_init.drop_index[11]    = cfg_init.drop_index[10] + 228;
            cfg_init.acc_type[11]      = cfg_init.acc_type[8];

            /* 228 queues pipe-4, start index 972 */
            cfg_init.util_index[12]    = cfg_init.util_index[11] + 228;
            cfg_init.drop_index[12]    = cfg_init.drop_index[11] + 228;
            cfg_init.acc_type[12]      = cfg_init.acc_type[8];

            /* 240 queues pipe-5, start index 1200 */
            cfg_init.util_index[13]    = cfg_init.util_index[12] + 228;
            cfg_init.drop_index[13]    = cfg_init.drop_index[12] + 228;
            cfg_init.acc_type[13]      = cfg_init.acc_type[8];

            /* 240 queues pipe-6, start index 1440 */
            cfg_init.util_index[14]    = cfg_init.util_index[13] + 240;
            cfg_init.drop_index[14]    = cfg_init.drop_index[13] + 240;
            cfg_init.acc_type[14]      = cfg_init.acc_type[8];

            /* 240 queues pipe-7, start index 1680 */
            cfg_init.util_index[15]    = cfg_init.util_index[14] + 240;
            cfg_init.drop_index[15]    = cfg_init.drop_index[14] + 240;
            cfg_init.acc_type[15]      = cfg_init.acc_type[8];

            cfg_init.max_port_num       = SOC_TH3_MAX_NUM_PORTS;
            cfg_init.queues_per_pipe    = SOC_TH3_NUM_COS_QUEUES_PER_PIPE;
            cfg_init.queues_per_port    = SOC_TH3_COS_MAX;
            cfg_init.ports_per_pipe     = SOC_TH3_MAX_DEV_PORTS_PER_PIPE;
            cfg_init.num_pipes          = _TH3_PIPES_PER_DEV;
            cfg_init.num_counters_per_pipe   = NUM_ITM(unit);

            result = _bcm_xgs5_ft_qcm_eapp_cfg_init_set(unit, cfg_init);
            if (BCM_FAILURE(result)) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                                "QCM init cfg set failed \n")));
                return result;
            }
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        return result;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_detach(int unit)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        result = bcm_esw_flowtracker_detach(unit);
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        return result;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_flow_view_config_set(int unit,
                                                    uint32 options,
                                                    bcm_cosq_burst_monitor_flow_view_info_t *view_cfg)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the set/reset configuration information */
        result = _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_set(unit, options, view_cfg);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM config set uc message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_flow_view_config_get(int unit,
                                                    bcm_cosq_burst_monitor_flow_view_info_t *view_cfg)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the get configuration information */
        result = _bcm_xgs5_ft_qcm_eapp_flow_view_cfg_get(unit, view_cfg);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM config get uc message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_flow_view_data_clear(int unit)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the clear flow view data */
        result = _bcm_xgs5_ft_qcm_eapp_flow_view_data_clear(unit);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM flow data clear message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_flow_view_data_get(int unit,
                                                  uint32 usr_mem_size,
                                                  uint8 *usr_mem_addr)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the get flow view data */
        result = _bcm_xgs5_ft_qcm_eapp_flow_view_data_get(unit, usr_mem_size, usr_mem_addr);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM flow data clear message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_get_current_time(int unit,
                                                uint64 *time_usecs)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the get current FW time */
        result = _bcm_xgs5_ft_qcm_eapp_get_current_time(unit, time_usecs);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM FW current time message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_view_summary_get(int unit, uint32 mem_size,
                                                uint8 *mem_addr, int max_num_views,
                                                int *num_views)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the get current FW time */
        result = _bcm_xgs5_ft_qcm_eapp_view_summary_get(unit, mem_size,
                mem_addr, max_num_views,
                num_views);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM view summary message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_flow_view_stats_get(int unit, uint32 mem_size,
                                                   uint8 *mem_addr, int max_num_flows,
                                                   int view_id, uint32 flags, int *num_flows)
{
    int result = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    uint8 qcm_enable = 0;
    qcm_enable = soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0);
    if (qcm_enable) {
        /* Inform QCM EAPP of the get current FW time */
        result = _bcm_xgs5_ft_qcm_eapp_flow_view_stats_get(unit, mem_size,
                mem_addr, max_num_flows,
                view_id, flags, num_flows);
        if (BCM_FAILURE(result)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM flow view stats message communication failed \n")));
        }
    } else if (!qcm_enable) {
        result = BCM_E_CONFIG;
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        result = BCM_E_UNAVAIL;
    }
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_dma_config_set(int unit,
                                              uint32 host_mem_size,
                                              uint32 **host_mem_addr)
{
    int result = BCM_E_UNAVAIL;
    result = bcm_th3_cosq_burst_monitor_dma_config_set(unit, host_mem_size,
                                                       host_mem_addr);
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_dma_config_get(int unit,
                                              uint32 *host_mem_size,
                                              uint32 **host_mem_addr)
{
    int result = BCM_E_UNAVAIL;
    result = bcm_th3_cosq_burst_monitor_dma_config_get(unit, host_mem_size,
                                                       host_mem_addr);
    return result;
}

int bcm_tomahawk3_cosq_burst_monitor_set(int unit, int num_gports,
                                   bcm_gport_t *gport_list)
{
    int  rv = BCM_E_NONE;

    if (gport_list == NULL) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                  "gport parameter NULL\n")));
        return BCM_E_PARAM;
    }

    if (num_gports > SHR_QCM_MAX_GPORT_MONITOR) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                  "Maximum number of entries :%d\n"),
                  SHR_QCM_MAX_GPORT_MONITOR));
        return BCM_E_PARAM;
    }

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        /* Customer provided array gports that are being monitored by the QCM App. */
        _bcm_esw_ft_qcm_port_cfg_t cfg_info[SHR_QCM_MAX_GPORT_MONITOR];
        int                        phy_port, pipe, mmu_port, local_mmu_port, i, num_ucq;
        _bcm_th3_cosq_node_t      *node;
        bcm_port_t                 local_port;
        soc_info_t                *si;
        si = &SOC_INFO(unit);

        sal_memset(cfg_info, 0, (sizeof(_bcm_esw_ft_qcm_port_cfg_t) *
                   SHR_QCM_MAX_GPORT_MONITOR));
        num_ucq = _soc_th3_get_num_ucq(unit);
        for (i = 0; i < num_gports; i++) {
            if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport_list[i]))) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "%x gport is non unicast queue type"
                          "Only uincast type gport are alone monitored\n"),
                          gport_list[i]));
                return BCM_E_PARAM;
            }

            rv = _bcm_th3_cosq_node_get(unit, gport_list[i], NULL, &local_port,
                                       NULL, &node);

            if ((BCM_E_NONE != rv) || (local_port < 0)) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "gport %x with wrong local port value\n"),
                          gport_list[i]));
                return rv;
            }

            rv = soc_port_pipe_get(unit, local_port, &pipe);
            if (BCM_E_NONE != rv) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "gport %x with wrong pipe\n"),
                          gport_list[i]));
                return rv;
            }

            if ((node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT) >=
                 num_ucq) {
                LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                          "gport %x with wrong queue number\n"),
                          gport_list[i]));
                return BCM_E_PORT;
            }
            phy_port = si->port_l2p_mapping[local_port];
            mmu_port = si->port_p2m_mapping[phy_port];
            local_mmu_port = mmu_port & (_BCM_TH3_NUM_SCHEDULER_PER_PORT - 1);
            /* local_mmu_port = si->port_l2i_mapping[local_port]; */

            cfg_info[i].gport = gport_list[i];
            cfg_info[i].pport = (uint16)local_port;
            cfg_info[i].global_num = (pipe * SOC_TH3_NUM_COS_QUEUES_PER_PIPE) +
                             (local_mmu_port * _BCM_TH3_MAX_NUM_QUEUE_PER_PORT) +
                             (node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT);
            cfg_info[i].port_pipe_queue_num = (((local_mmu_port & 0xFFFF) << 16) |
                                      ((pipe & 0xF) << 12) |
                                      ((node->hw_index % _BCM_TH3_MAX_NUM_QUEUE_PER_PORT) & 0xFFF));
        }

        rv = _bcm_xgs5_ft_qcm_eapp_config_set(unit, num_gports, cfg_info);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                            "QCM config set message communication failed \n")));
            return rv;
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        rv = BCM_E_UNAVAIL;
    }
    return rv;
}

int bcm_tomahawk3_cosq_burst_monitor_get(int unit, int  max_gports,
                                   bcm_gport_t *gport_list, int *num_gports)
{
    uint32  qcm_app_status[2], qcm_host_status, i, entries_transfer;
    int rv = BCM_E_NONE;

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_property_get(unit, spn_QCM_FLOW_ENABLE, 0)) {
        /* Customer provided array gports that are being monitored by the QCM App. */
        rv = _bcm_xgs5_ft_qcm_eapp_config_get(unit, max_gports, gport_list, num_gports);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "QCM config get message communication failed \n")));
            return rv;
        }
    } else
#endif /* INCLUDE_FLOWTRACKER */
    {
        qcm_app_status[0] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status)));
        qcm_app_status[1] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(app_status) + 4));

        /* Validation check whether core 0 R5 is loaded with QCM app srec file
         *          * ASCII of word QCM_(SHR_QCM_APP_STATUS_1) and _APP(SHR_QCM_APP_STATUS_2)
 */
        if ((qcm_app_status[0] != SHR_QCM_APP_STATUS_1) ||
                (qcm_app_status[1] != SHR_QCM_APP_STATUS_2)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "load QCM app srec file to core 0\n")));
            return BCM_E_UNAVAIL;
        }

        qcm_host_status = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + sram_offset(host_status)));
        if (!(qcm_host_status & SHR_QCM_INIT_STATUS_BIT)) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "set shared memory\n")));
            return BCM_E_INIT;
        }

        if (gport_list == NULL) {
            LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                      "gport parameter NULL\n")));
            return BCM_E_PARAM;
        }

        *num_gports = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + (sram_offset(total_num_gports))));

        if (!max_gports) {
            return BCM_E_NONE;
        } else if (*num_gports > max_gports) {
            entries_transfer = max_gports;
        } else {
            entries_transfer = *num_gports;
        }

        sal_memset(gport_list, 0, (sizeof(bcm_gport_t) * max_gports));

        for (i = 0; i < entries_transfer; i++) {
            gport_list[i] = soc_uc_mem_read(unit, (SRAM_BASEADDR(unit) + (sram_offset(port_pipe_queue)) + (i * 2 * 4)));
        }
    }
    return rv;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
