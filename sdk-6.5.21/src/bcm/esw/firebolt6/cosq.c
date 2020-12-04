/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 *  COS Queue Management
 *  Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/helix5.h>
#include <soc/firebolt6.h>

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/firebolt6.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/bst.h>
#include <bcm_int/esw/ecn.h>
#include <bcm_int/esw/pfc_deadlock.h>
#if defined(INCLUDE_PSTATS)
#include <bcm_int/esw/pstats.h>
#endif /* INCLUDE_PSTATS */


#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

/* COS default value for FB6 is assumed to be 8 */
#define _BCM_FB6_COS_DEFAULT       8
#define _BCM_FB6_NUM_COS(unit)     NUM_COS(unit)

/* FB6 supports 8 PFC priorities */
#define _BCM_FB6_NUM_PFC_CLASS  8

#define _BCM_FB6_NUM_SAFC_CLASS  16
/* Number of logical Ports */
#define _BCM_FB6_NUM_PORTS 72
#define _BCM_FB6_NUM_SUB_PORTS 460

#define _BCM_FB6_MC_QUEUE_BASE    3680 

#define _FB6_CPU_LO_NODE_BASE     461
#define _FB6_CPU_L1_NODE_BASE     3690
#define _FB6_CPU_L2_NODE_BASE     3690

#define FB6_WRED_CELL_FIELD_MAX       0x3ffff
#define FB6_WRED_PROFILE_NUM          9
#define _BCM_FB6_NUM_WRED_RESOLUTION_TABLE   4

#define FB6_SUBPORT_NUM_PRI               2
#define FB6_NUM_SUBPORTS                  384 

#define FB6_CPU_SCHED_LEVEL_GET(unit, cosq) \
        ((cosq < 1) ? SOC_FB6_NODE_LVL_L0 : SOC_FB6_NODE_LVL_L1)

/* Number of COSQs configured for this unit */
#define _BCM_FB6_NUM_COS(unit) NUM_COS(unit)


#define _FB6_PORT_NUM_COS(unit, port) \
    ((IS_CPU_PORT(unit, port)) ? NUM_CPU_COSQ(unit) : \
     (IS_LB_PORT(unit, port)) ? 10 : _BCM_FB6_NUM_COS(unit))

#define _FB6_CPU_NUML0_NODES      1
#define _FB6_CPU_NUML1_NODES      10

#define _FB6_HG_NUML0_NODES       4
#define _FB6_HG_NUML1_NODES       32

#define _FB6_ST_NUML0_NODES       3
#define _FB6_ST_NUML1_NODES       18

#define _FB6_DL_NUML0_NODES       1
#define _FB6_DL_NUML1_NODES       8

#define FB6_SUBPORT_BASE         72
#define FB6_SUBPORT_INDEX_GET(unit, subport) \
        (subport - FB6_SUBPORT_BASE)

#define FB6_SUBPORT_PFC_CLASS      2
#define FB6_SUBPORT_COS_COUNT      8
typedef struct _bcm_fb6_cosq_cpu_cosq_config_s {
    /* All MC queues have DB[idx 0] and MCQE[idx 1] space */
    int q_min_limit[2];
    int q_shared_limit[2];
    uint8 q_limit_dynamic[2];
    uint8 q_limit_enable[2];
    uint8 q_color_limit_enable[2];
    uint8 enable;
} _bcm_fb6_cosq_cpu_cosq_config_t;


#define _BCM_FB6_NUM_TIME_DOMAIN    4

typedef struct _bcm_fb6_cosq_time_info_s {
    uint32 ref_count;
} _bcm_fb6_cosq_time_info_t;

extern int _bcm_td3_ecn_init(int unit);

STATIC _bcm_fb6_cosq_time_info_t fb6_time_domain_info[BCM_MAX_NUM_UNITS][_BCM_FB6_NUM_TIME_DOMAIN];


STATIC uint32 _bcm_fb6_wred_resolution_tbl[_BCM_FB6_NUM_WRED_RESOLUTION_TABLE] = {
    1,
    0,
    0,
    0,
};


typedef struct _bcm_fb6_cosq_node_s {
    bcm_gport_t gport;
    int numq;
    int level;
    int hw_index;
    int in_use;
    bcm_gport_t parent_gport;
} _bcm_fb6_cosq_node_t;


typedef struct _bcm_fb6_cosq_cpu_port_info_s {
    _bcm_fb6_cosq_node_t sched[SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT];
    _bcm_fb6_cosq_node_t mcast[SOC_FB6_NUM_CPU_QUEUES];
} _bcm_fb6_cosq_cpu_port_info_t;

typedef struct _bcm_fb6_cosq_port_info_s {
    _bcm_fb6_cosq_node_t ucast[SOC_FB6_NUM_UCAST_QUEUE_PER_PORT];
    _bcm_fb6_cosq_node_t mcast[SOC_FB6_NUM_MCAST_QUEUE_PER_PORT];
    _bcm_fb6_cosq_node_t sched[SOC_FB6_NUM_SCHEDULER_PER_PORT];
} _bcm_fb6_cosq_port_info_t;

typedef struct _bcm_fb6_cosq_st_port_info_s {
    _bcm_fb6_cosq_node_t ucast[SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT];
    _bcm_fb6_cosq_node_t mcast[SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT];
    _bcm_fb6_cosq_node_t sched[SOC_FB6_NUM_SCHEDULER_PER_ST_PORT];
} _bcm_fb6_cosq_st_port_info_t;

typedef struct _bcm_fb6_cosq_hg_port_info_s {
    _bcm_fb6_cosq_node_t ucast[SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT];
    _bcm_fb6_cosq_node_t mcast[SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT];
    _bcm_fb6_cosq_node_t sched[SOC_FB6_NUM_SCHEDULER_PER_HG_PORT];
} _bcm_fb6_cosq_hg_port_info_t;

typedef struct _bcm_fb6_cosq_coe_port_info_s {
    _bcm_fb6_cosq_node_t ucast[SOC_FB6_NUM_UCAST_QUEUE_PER_PORT];
    _bcm_fb6_cosq_node_t mcast[SOC_FB6_NUM_MCAST_QUEUE_PER_PORT];
    _bcm_fb6_cosq_node_t sched[SOC_FB6_NUM_SCHEDULER_PER_PORT];
} _bcm_fb6_cosq_coe_port_info_t;

typedef struct _bcm_fb6_mmu_info_s {
    _bcm_fb6_cosq_port_info_t *_bcm_fb6_port_info;
    _bcm_fb6_cosq_cpu_port_info_t *_bcm_fb6_cpu_port_info;
    _bcm_fb6_cosq_st_port_info_t *_bcm_fb6_st_port_info;
    _bcm_fb6_cosq_hg_port_info_t *_bcm_fb6_hg_port_info;
    _bcm_fb6_cosq_coe_port_info_t *_bcm_fb6_coe_port_info;
    int gport_tree_created; /* FALSE: No GPORT tree exists */
} _bcm_fb6_mmu_info_t;


STATIC _bcm_fb6_mmu_info_t *_bcm_fb6_mmu_info[BCM_MAX_NUM_UNITS]; /* MMU info */
STATIC _bcm_fb6_cosq_port_info_t *_bcm_fb6_cosq_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_fb6_cosq_cpu_port_info_t *_bcm_fb6_cosq_cpu_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_fb6_cosq_st_port_info_t *_bcm_fb6_cosq_st_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_fb6_cosq_hg_port_info_t *_bcm_fb6_cosq_hg_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_fb6_cosq_coe_port_info_t *_bcm_fb6_cosq_coe_port_info[BCM_MAX_NUM_UNITS];
STATIC _bcm_fb6_cosq_cpu_cosq_config_t *_bcm_fb6_cosq_cpu_cosq_config[BCM_MAX_NUM_UNITS][SOC_FB6_NUM_CPU_QUEUES];



STATIC int
_bcm_fb6_cosq_bucket_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                         uint32 min_quantum, uint32 max_quantum,
                         uint32 kbits_burst_min, uint32 kbits_burst_max,
                         uint32 flags);
STATIC int
_bcm_fb6_cosq_bucket_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                        uint32 *flags);


#define _BCM_FB6_MMU_INFO(unit) _bcm_fb6_mmu_info[(unit)]
#define _FB6_NUM_COS(unit) NUM_COS(unit)
#define _BCM_FB6_CPU_COS_MAPS_RSVD   9
#define _BCM_FB6_INT_PRI_8_15_MASK   0x8
#define _BCM_FB6_INT_PRI_0_7_MASK    0xf

STATIC soc_profile_mem_t *_bcm_fb6_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_fb6_wred_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_fb6_ifp_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_fb6_prio2cos_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_fb6_chprio2cos_profile[BCM_MAX_NUM_UNITS];



/*
 *  index: degree, value: contangent(degree) * 100
 *  max value is 0xffff (16-bit) at 0 degree
 */
STATIC int
_bcm_fb6_cotangent_lookup_table[] =
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

extern int bcm_fb6_port_param_check(int unit, bcm_port_t port)
{
    if(!SOC_PORT_VALID(unit, port)) {
        if (port >= _BCM_FB6_NUM_PORTS && port <= _BCM_FB6_NUM_SUB_PORTS) {
            return BCM_E_NONE;
        } else {
            return BCM_E_PORT;
        }
    }
    return BCM_E_NONE;
}

/*
 * Given a slope (angle in degrees) from 0 to 90, return the number of cells
 * in the range from 0% drop probability to 100% drop probability.
 */
STATIC int
_bcm_fb6_angle_to_cells(int angle)
{
    return (_bcm_fb6_cotangent_lookup_table[angle]);
}

/*
 *  Given a number of packets in the range from 0% drop probability
 *  to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_fb6_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_fb6_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}


/*
 **  Convert HW drop probability to percent value
 */
STATIC int
_bcm_fb6_hw_drop_prob_to_percent[] = {
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
_bcm_fb6_percent_to_drop_prob(int percent)
{
    int i;

    for (i = 14; i > 0 ; i--) {
        if (percent >= _bcm_fb6_hw_drop_prob_to_percent[i]) {
            break;
        }
    }
    return i;
}

STATIC int
_bcm_fb6_drop_prob_to_percent(int drop_prob) {
    return (_bcm_fb6_hw_drop_prob_to_percent[drop_prob]);
}

int bcm_cosq_coe_port_get(int unit, int coe_port, int *fport)
{
    int port ;
    int base_l0, num_l0;
    PBMP_PORT_ITER(unit, port) {

        base_l0 = 0;
        num_l0 = 0;
        BCM_IF_ERROR_RETURN(
                _soc_fb6_port_base_l0_mapping_get(0, port, &base_l0, &num_l0));
        if (coe_port >= base_l0 && coe_port <= (base_l0 + num_l0)) {
            *fport = port;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_pri2cos_profile_setget(int unit, int port, int index, int *getindex, int setget)
{
   int l0_num, num_l0, i;
   int phy_port, mmu_port;
   soc_mem_t mem = NONCH_L0_CONFIG_TBL_SC0m;
   uint32 entry[SOC_MAX_MEM_WORDS];
   soc_info_t *si = &SOC_INFO(unit);

   phy_port = si->port_l2p_mapping[port];
   mmu_port = si->port_p2m_mapping[phy_port];
   BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, port, &l0_num, &num_l0));

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_num, &entry));
   if (setget == 1) {
       for (i = 0; i < num_l0; i++) {
           sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
           soc_mem_field32_set(unit, mem, &entry, MMU_PORTNUMf, mmu_port);
           soc_mem_field32_set(unit, mem, &entry, PROFILE_INDEXf, index);
           soc_mem_field32_set(unit, mem, &entry, L0_OFFSETf, i);
           BCM_IF_ERROR_RETURN
               (soc_mem_write(unit, mem, MEM_BLOCK_ALL, l0_num + i, &entry));

       }
   } else {
       if (getindex != NULL) {
           *getindex = soc_mem_field32_get(unit, mem, &entry, PROFILE_INDEXf);
       }
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cpu_cosq_mapping_default_set
 * Purpose:
 *     Map 16 internal priorities to CPU COS queues using CPU_COS_MAPm
 *     Default cpu_cos_maps should have lower priorities than user created
 *     maps
 * Parameters:
 *     unit - (IN) Unit number.
 *     numq - (IN) number of COS queues (0-7).
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_fb6_cpu_cosq_mapping_default_set(int unit, int numq)
{
    int cos = 0, prio = 0, index = 0, i = 0;
    cpu_cos_map_entry_t cpu_cos_map_entry;

    /* Nothing to do on devices with no CPU_COS_MAP memory */
    if (!SOC_MEM_IS_VALID(unit, CPU_COS_MAPm)) {
        return BCM_E_NONE;
    }

    /* Map internal priorities 0 ~ 7 to appropriate CPU CoS queue */
    sal_memset(&cpu_cos_map_entry, 0, sizeof(cpu_cos_map_entry_t));
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, VALIDf, 1);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
            INT_PRI_MASKf, _BCM_FB6_INT_PRI_0_7_MASK);

    index = (soc_mem_index_count(unit, CPU_COS_MAPm) - _BCM_FB6_CPU_COS_MAPS_RSVD);
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
                    INT_PRI_KEYf, prio);
            soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, COSf, cos);
            SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                        &cpu_cos_map_entry));
            prio++;
            index++;
        }
    }

    /* Map internal priorities 8 ~ 15 to (numq - 1) CPU CoS queue */
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
            INT_PRI_KEYf, 0x8);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry,
            INT_PRI_MASKf, _BCM_FB6_INT_PRI_8_15_MASK);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &cpu_cos_map_entry, COSf, numq - 1);
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                &cpu_cos_map_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_fb6_cosq_config_set
 * Purpose:
 *     Set the number of classes of service supported
 * Parameters:
 *     unit - (IN) unit number
 *     numq - (IN) number of classes of service
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];
    uint32 index;
    int cos, prio = 0, ref_count = -1;
    uint32 i;
    soc_mem_t mem = PORT_COS_MAPm;

    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }

    index = 0;
    while (index < soc_mem_index_count(unit, mem)) {
        BCM_IF_ERROR_RETURN(soc_profile_mem_ref_count_get(unit,
                    _bcm_fb6_cos_map_profile[unit],
                    index, &ref_count));
        if (ref_count > 0) {
            while (ref_count) {
                if (!soc_profile_mem_delete(unit,
                            _bcm_fb6_cos_map_profile[unit], index)) {
                    ref_count -= 1;
                }
            }
        }
        index += 16;
    }

    /*
     *Distribute first 8 internal priority levels into the specified number
     *of cosq, map remaining internal priority levels to highest priority
     *cosq
 */
    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;

    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                    UC_COS1f, cos);
            soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                    MC_COS1f, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS1f,
                numq - 1);
        soc_mem_field32_set(unit, mem, &cos_map_entries[prio], MC_COS1f,
                numq - 1);
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_fb6_cpu_cosq_mapping_default_set(unit, numq));

    SOC_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_fb6_cos_map_profile[unit], entries, 16,
                             &index));
    for (i = 0; i < _BCM_FB6_NUM_SUB_PORTS; i++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm, i, SELECTf,
                                    index / 16));
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    index / 16));
    }
    
    for (i = 0; i < _BCM_FB6_NUM_SUB_PORTS; i++) {
        soc_profile_mem_reference(unit, _bcm_fb6_cos_map_profile[unit], index, 0);
    }

    _BCM_FB6_NUM_COS(unit) = numq;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_fb6_cosq_config_get
 * Purpose:
 *   Get the number of Classes of Service
 * Parameters:
 *    unit - (IN) unit number
 *    numq - (OUT) Number of Classes of Service
 *  Returns:
 *      BCM_E_XXX
 */
int
bcm_fb6_cosq_config_get(int unit, int *numq)
{
    if (numq != NULL) {
        *numq = _BCM_FB6_NUM_COS(unit);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cosq_cleanup
 * Purpose:
 *     Clean resource in case of error during Cosq Init
 * Parameters:
 *     unit - (IN) unit number
 * Returns:
 *     No return value
 */
STATIC void
_bcm_fb6_cosq_cleanup(int unit)
{
    if (_bcm_fb6_cosq_port_info[unit] != NULL) {
        sal_free(_bcm_fb6_cosq_port_info[unit]);
        _bcm_fb6_cosq_port_info[unit] = NULL;
    }

    if (_bcm_fb6_cosq_st_port_info[unit] != NULL) {
        sal_free(_bcm_fb6_cosq_st_port_info[unit]);
        _bcm_fb6_cosq_st_port_info[unit] = NULL;
    }

    if (_bcm_fb6_cosq_hg_port_info[unit] != NULL) {
        sal_free(_bcm_fb6_cosq_hg_port_info[unit]);
        _bcm_fb6_cosq_hg_port_info[unit] = NULL;
    }

    if (_bcm_fb6_cosq_cpu_port_info[unit] != NULL) {
        sal_free(_bcm_fb6_cosq_cpu_port_info[unit]);
        _bcm_fb6_cosq_cpu_port_info[unit] = NULL;
    }

    if (_bcm_fb6_mmu_info[unit] != NULL) {
        sal_free(_bcm_fb6_mmu_info[unit]);
        _bcm_fb6_mmu_info[unit] = NULL;
    }

    if (_bcm_fb6_cos_map_profile[unit] != NULL) {
        (void)soc_profile_mem_destroy(unit, _bcm_fb6_cos_map_profile[unit]);
        sal_free(_bcm_fb6_cos_map_profile[unit]);
        _bcm_fb6_cos_map_profile[unit] = NULL;
    }

    if (_bcm_fb6_wred_profile[unit] != NULL) {
        (void)soc_profile_mem_destroy(unit, _bcm_fb6_wred_profile[unit]);
        sal_free(_bcm_fb6_wred_profile[unit]);
        _bcm_fb6_wred_profile[unit] = NULL;
    }

    if (_bcm_fb6_ifp_cos_map_profile[unit] != NULL) {
        (void)soc_profile_mem_destroy(unit, _bcm_fb6_ifp_cos_map_profile[unit]);
        sal_free(_bcm_fb6_ifp_cos_map_profile[unit]);
        _bcm_fb6_ifp_cos_map_profile[unit] = NULL;
    }

    if (_bcm_fb6_prio2cos_profile[unit] != NULL) {
        (void)soc_profile_reg_destroy(unit, _bcm_fb6_prio2cos_profile[unit]);
        sal_free(_bcm_fb6_prio2cos_profile[unit]);
        _bcm_fb6_prio2cos_profile[unit] = NULL;
    }

    if (_bcm_fb6_chprio2cos_profile[unit] != NULL) {
        (void)soc_profile_reg_destroy(unit, _bcm_fb6_chprio2cos_profile[unit]);
        sal_free(_bcm_fb6_chprio2cos_profile[unit]);
        _bcm_fb6_chprio2cos_profile[unit] = NULL;
    }
#if defined(INCLUDE_PSTATS)
    if (_BCM_PSTATS_UNIT_DRIVER(unit) != NULL) {
        _bcm_pstats_deinit(unit);
    }
#endif /* INCLUDE_PSTATS */

}

/*
 * Function:
 *     _bcm_fb6_cosq_node_get
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
int
_bcm_fb6_cosq_node_get(int unit, bcm_gport_t gport, bcm_module_t *modid,
                       bcm_port_t *port, int *id, _bcm_fb6_cosq_node_t **node)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_coe_port_info_t *coeport_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_node_t *node_base = NULL;
    bcm_module_t modid_out;
    bcm_port_t port_out;
    int index;
    int num_sched_port = 0;

    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_coe_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        port_out = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        port_out = BCM_GPORT_SCHEDULER_GET(gport) & 0xfff;
    } else {
        return BCM_E_PORT;
    }

    if (IS_LB_PORT(unit, port_out) ) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, port_out)) {
        cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
        num_sched_port = SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT;
    } else if(IS_STK_PORT(unit, port_out)){
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][port_out];
        num_sched_port = SOC_FB6_NUM_SCHEDULER_PER_ST_PORT;
    } else if(IS_UPLINK_PORT(unit, port_out)){
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][port_out];
        num_sched_port = SOC_FB6_NUM_SCHEDULER_PER_HG_PORT;
    } else if(port_out >= _BCM_FB6_NUM_PORTS) {
        coeport_info = &_bcm_fb6_cosq_coe_port_info[unit][port_out];
        num_sched_port = SOC_FB6_NUM_SCHEDULER_PER_PORT;
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][port_out];
        num_sched_port = SOC_FB6_NUM_SCHEDULER_PER_PORT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        index = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        if(IS_STK_PORT(unit, port_out)) {
            node_base = stport_info->ucast;
        } else if(IS_UPLINK_PORT(unit, port_out)) {
            node_base = hgport_info->ucast;
        } else if(port_out >= _BCM_FB6_NUM_PORTS) {
            node_base = coeport_info->ucast;
        } else {
            node_base = port_info->ucast;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        index = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        if (IS_CPU_PORT(unit, port_out)) {
            node_base = cpu_port_info->mcast;
        } else if(IS_UPLINK_PORT(unit, port_out)) {
            node_base = hgport_info->mcast;
        } else if(IS_STK_PORT(unit, port_out)) {
            node_base = stport_info->mcast;
        } else if(port_out >= _BCM_FB6_NUM_PORTS) {
            node_base = coeport_info->mcast;
        } else {
            node_base = port_info->mcast;
        }
    } else {
        index = (BCM_GPORT_SCHEDULER_GET(gport) >> 16) & 0x3ff;
        if (index >= num_sched_port) {
            return BCM_E_PORT;
        }
        if (IS_CPU_PORT(unit, port_out)) {
            node_base = cpu_port_info->sched;
        } else if(IS_UPLINK_PORT(unit, port_out)) {
            node_base = hgport_info->sched;
        } else if(IS_STK_PORT(unit, port_out)) {
            node_base = stport_info->sched;
        } else if(port_out >= _BCM_FB6_NUM_PORTS) {
            node_base = coeport_info->sched;
        } else {
            node_base =  port_info->sched;
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
        *id = index;
    }
    if (node != NULL) {
        *node = &node_base[index];
    }

    return BCM_E_NONE;

}
/*
 * Function:
 *     _bcm_fb6_cosq_port_cos_resolve
 * Purpose:
 *     Find UC/MC queue gport for a given logical port
 *     and cos value.
 * Parameters:
 *     unit      - (IN) unit number
 *     port      - (IN) logical port
 *     cos       - (IN) cos value
 *     style     - (IN) index style
 *                 (_BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE or
 *                  _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE)
 *     gport     - (OUT) Queue gport
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fb6_cosq_port_cos_resolve(int unit, bcm_port_t port, bcm_cos_t cos,
                              _bcm_fb6_cosq_index_style_t style,
                              bcm_gport_t *gport)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (style != _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER) {
        return BCM_E_PARAM;
    }

    if (cos < 0) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (_BCM_FB6_MMU_INFO(unit)->gport_tree_created != TRUE) {
        return BCM_E_INTERNAL;
    }

    if (IS_CPU_PORT(unit, port)) {
        cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
        if (style == _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER) {
            if (cos >= SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT) {
                return BCM_E_PARAM;
            }
            *gport = cpu_port_info->sched[cos].gport;
        }
    } else if (IS_STK_PORT(unit, port)) {
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][port];
        if (style == _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER) {
            if (cos >= SOC_FB6_NUM_SCHEDULER_PER_ST_PORT) {
                return BCM_E_PARAM;
            }
            *gport = stport_info->sched[cos].gport;
        }
    } else if (IS_UPLINK_PORT(unit, port)) {
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][port];
        if (style == _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER) {
            if (cos >= SOC_FB6_NUM_SCHEDULER_PER_HG_PORT) {
                return BCM_E_PARAM;
            }
            *gport = hgport_info->sched[cos].gport;
        } 
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][port];
        if (style == _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER) {
            if (cos >= SOC_FB6_NUM_SCHEDULER_PER_PORT) {
                return BCM_E_PARAM;
            }
            *gport = port_info->sched[cos].gport;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *    _bcm_fb6_cosq_port_resolve
 * Purpose:
 *     Resolve following COS queue related GPORT
 *     BCM_GPORT_TYPE_UCAST_QUEUE_GROUP
 *     BCM_GPORT_TYPE_MCAST_QUEUE_GROUP
 *     BCM_GPORT_TYPE SCHEDULER
 *Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     modid      - (OUT) module identifier
 *     port       - (OUT) port number
 *     trunk_id   - (OUT) trunk identifier
 *     id         - (OUT) queue group or scheduler identifier
 *     qnum       - (OUT) hw_index for given queue gport
 *Returns:
 *    BCM_E_XXX
 */

int
_bcm_fb6_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
        bcm_port_t *port, bcm_trunk_t *trunk_id, int *id,
        int *qnum)
{
    _bcm_fb6_cosq_node_t *node;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, gport, modid, port, id, &node));
    if (trunk_id) {
        *trunk_id = -1;
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
*  Function:
*      _bcm_fb6_cosq_subport_resolve
*  Purpose:
*      Resolve subport if it is a subport gport 
*      
*  Parameters:
*      unit       - (IN) unit number
*      gport      - (IN) subport GPORT identifier
*      sub_port   - (OUT) sub  port number
*  Returns:
*      BCM_E_XXX
*/

STATIC int
_bcm_fb6_cosq_subport_resolve(int unit, bcm_gport_t gport,
        bcm_port_t *sub_port)
{
    int subport_num = 0;
    int port;

    if (!BCM_GPORT_IS_SET(gport) &&
            ! _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
        return BCM_E_PORT;
    }

    if (BCM_GPORT_IS_SET(gport) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {

        BCM_IF_ERROR_RETURN(
                _bcm_coe_subtag_subport_port_subport_num_get(unit,
                    gport, &port, &subport_num));

        *sub_port = subport_num + 4;
    }
    return BCM_E_NONE;
}

/*
*  Function:
*      _bcm_fb6_cosq_localport_resolve
*  Purpose:
*      Resolve GPORT if it is a local port
*  Parameters:
*      unit       - (IN) unit number
*      gport      - (IN) GPORT identifier
*      local_port - (OUT) local port number
*  Returns:
*      BCM_E_XXX
*/

STATIC int
_bcm_fb6_cosq_localport_resolve(int unit, bcm_gport_t gport,
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
    *local_port = port;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PARAM;
    }


    return BCM_E_NONE;
}


/*
*  Function:
*      _bcm_fb6_cosq_cpu_parent_set
*  Purpose:
*      Set Parent(L1) for a given L2 queue of the CPU port.
*      Only child @ L2 level is supported. Because only L2 level queues can be
*      attached to different parent (L1.0-L1.9) of the CPU port.
*  Parameters:
*      unit         - (IN) unit number
*      child_index  - (IN) Child node index
*      child_level  - (IN) Child node level
*      parent_index - (IN) Parent node index
*  Returns:
*      BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_cpu_parent_set(int unit, int child_index, int child_level,
        int parent_index)
{
    soc_reg_t reg =  Q_SCHED_CPU_L2_MC_QUEUE_L1_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_FB6_NODE_LVL_L2) {
        return BCM_E_PARAM;
    }

    /* Set the L2 queue's parent to the given L1 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    soc_reg_field_set(unit, reg, &rval, SELECTf, parent_index);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, 0, child_index, rval));

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_fb6_cosq_gport_get
 * Purpose:
 *     Get a cosq gport structure
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     port       - (OUT) port number
 *     numq       - (OUT) number of COS queues
 *     flags      - (OUT) flags (BCM_COSQ_GPORT_XXX)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                       int *numq, uint32 *flags)
{
    _bcm_fb6_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_module_t modid;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
              "bcm_fb6_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                    &node));
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
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        *flags = 0;
        if (IS_CPU_PORT(unit, local_port)) {
            *numq = SOC_FB6_NUM_CPU_QUEUES;
        } else if(IS_STK_PORT(unit, local_port)){
            *numq = SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT;
        } else if(IS_UPLINK_PORT(unit, local_port)){
            *numq = SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT;
        } else {
            *numq = SOC_FB6_NUM_MCAST_QUEUE_PER_PORT;
        }
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
              "                       port=0x%x numq=%d flags=0x%x\n"),
              *port, *numq, *flags));

    return BCM_E_NONE;
}

/*
* CMIC CMC Channel to Queue mapping.
*/

STATIC int
_bcm_fb6_cosq_rx_queue_channel_set(int unit, bcm_cos_queue_t parent_cos,
        bcm_cos_queue_t queue_id, int enable)
{
#if defined(BCM_CMICX_SUPPORT)
    if soc_feature(unit, soc_feature_cmicx) {
        int pci_cmc = SOC_PCI_CMC(unit);
        int chan_id = -1; 
        int start_chan_id = 0;
        uint32 chan_off;
        uint32 reg_addr, reg_val;
        uint32 ix;
        uint32 max_rx_channels = 0;

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
            soc_dma_cos_ctrl_reg_addr_get(unit, pci_cmc, chan_off, queue_id,
                    &reg_addr);
            reg_val = soc_pci_read(unit, reg_addr);

            if (enable == 1) {
                if (ix == (uint32)chan_id) {
                    reg_val |= ((uint32)1 << (queue_id % 32)); 
                } else {
                    /* Clear all other channels */
                    reg_val &= ~((uint32)1 << (queue_id % 32)); 
                }
            } else {
                if (ix == (uint32)chan_id) {
                    reg_val &= ~((uint32)1 << (queue_id % 32)); 
                }
            }
            /* Incoporate the reserved queues (if any on this device)
             *              * into the CMIC programming,  */
            reg_val |=
                CPU_ARM_RSVD_QUEUE_BITMAP(unit, pci_cmc)[queue_id / 32];
            soc_pci_write(unit, reg_addr, reg_val);
        }
    }
#endif /* BCM_CMICX_SUPPORT */

    return BCM_E_NONE;
}

/*
 *Function:
 *     _bcm_fb6_cosq_cpu_parent_get
 * Purpose:
 *     Get Parent(L1) for a given L2 queue of the CPU port.
 *     Only child @ L2 level is supported. Because only L2 level queues can be
 *     attached to different parent (L1.0-L1.9) of the CPU port.
 * Parameters:
 *     unit         - (IN)  unit number
 *     child_index  - (IN)  Child node index
 *     child_level  - (IN)  Child node level
 *     parent_index - (OUT) Parent node index
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_cpu_parent_get(int unit, int child_index, int child_level,
        int *parent_index)
{
    soc_reg_t reg = Q_SCHED_CPU_L2_MC_QUEUE_L1_MAPPINGr;
    uint32 rval = 0;

    if (child_level != SOC_FB6_NODE_LVL_L2) {
        return BCM_E_PARAM;
    }

    /* Set the L2 queue's parent to the given L1 node */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, 0, child_index, &rval));
    *parent_index = soc_reg_field_get(unit, reg, rval, SELECTf);

    return BCM_E_NONE;
}




/*
 * Function:
 *     _bcm_fb6_cosq_cpu_gport_detach
 * Purpose:
 *     Detach any of the L2 MC Queue from its parent L1 nodes.
 *     Post detach, L2 MC queue is attached to Parent L1.9 node
 * Parameters:
 *     unit        - (IN) unit number
 *     input_port  - (IN) scheduler GPORT identifier
 *     parent_port - (IN) GPORT to detach from
 *     cosq        - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_cpu_gport_detach(int unit, bcm_gport_t input_gport,
        bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    bcm_port_t parent_port, input_port;
    _bcm_fb6_cosq_node_t *input_node = NULL, *parent_node = NULL;
    int input_cos = 0, parent_cos = 0, config_cos = 0;
    int index = 0;

    /* Post Default tree created, expected Input
     * Gport is of MC Gport
     * and expected Parent Gport is new L0
     * Parent
     */
    if ((!BCM_GPORT_IS_SCHEDULER(parent_gport)) ||
            (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                &input_node));
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, parent_gport, NULL, &parent_port, NULL,
                                &parent_node));

    if (!IS_CPU_PORT(unit, parent_port)) {
        return BCM_E_PARAM;
    }

    /* Return BCM_E_PARAM when queue to be detached is
       not attached to the given gport */
    if (input_node->parent_gport != parent_gport) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        switch(parent_node->level) {
            case SOC_FB6_NODE_LVL_L0:
                parent_cos =  parent_node->hw_index -  _FB6_CPU_LO_NODE_BASE;
                break;
            case SOC_FB6_NODE_LVL_L1:
                parent_cos =  parent_node->hw_index -  _FB6_CPU_L1_NODE_BASE;
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    input_cos = cosq % SOC_FB6_NUM_CPU_QUEUES;

    if(parent_node->level == SOC_FB6_NODE_LVL_L1) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_cpu_parent_get(unit, input_cos, SOC_FB6_NODE_LVL_L2,
                                          &config_cos));
        if (config_cos != parent_cos) {
            /* Error when detaching from wrong Parent (L0 level) */
            return BCM_E_PARAM;
        }

        /*
         * L1.0 is associated with CMC0 CH1(Rx)
         * L1.1 is associated with CMC0 CH2(Rx)
         * L1.2 is associated with CMC0 CH3(Rx)
         * L1.3 is associated with CMC0 CH4(Rx)
         * L1.4 is associated with CMC0 CH5(Rx)
         * L1.5 is associated with CMC0 CH6(Rx)
         * L1.6 is associated with CMC0 CH7(Rx)
         */
        if ((parent_cos >= 0) && (parent_cos < 7)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_rx_queue_channel_set(unit, parent_cos, input_cos,
                                                    0));
        }

        /* Reset parent to L1.9 */
        index = _FB6_CPU_NUML1_NODES;
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_cpu_parent_set(unit, input_cos, SOC_FB6_NODE_LVL_L2, index - 1));
        input_node->parent_gport = _BCM_FB6_MMU_INFO(unit)->_bcm_fb6_cpu_port_info->
            sched[index].gport;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cosq_cpu_gport_attach
 * Purpose:
 *     Attach any of the L2 MC Queue to one of the L1 nodes
 * Parameters:
 *     unit         - (IN) unit number
 *     input_gport  - (IN) GPORT id, that will attach to parent gport
 *     parent_gport - (IN) Parent Gport to which current gport will be attached
 *     cosq         - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_cpu_gport_attach(int unit, bcm_gport_t input_gport,
        bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{

    _bcm_fb6_cosq_node_t *input_node = NULL;
    _bcm_fb6_cosq_node_t *parent_node = NULL;
    bcm_port_t input_port, parent_port;
    int parent_cos = 0;

    LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,
                        "bcm_fb6_cosq_gport_attach: unit=%d parent_gport=0x%x "
                        "input_gport=0x%x cosq=%d\n"), unit, parent_gport,
             input_gport, cosq));

    if (_BCM_FB6_MMU_INFO(unit)->gport_tree_created == FALSE) {
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
        (_bcm_fb6_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                &input_node));

    if (!input_node) {
        return BCM_E_PARAM;
    }
    if (input_port < 0) {
        return BCM_E_PORT;
    }

    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, parent_gport, NULL, &parent_port, NULL,
                                    &parent_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, parent_gport, &parent_port));
        parent_node = NULL;
    }

    if (!IS_CPU_PORT(unit, parent_port)) {
        return BCM_E_PARAM;
    }

    /* Return BCM_E_EXISTS when queue to be attached is
     * already attached to the given gport
     */
    if ((_BCM_FB6_MMU_INFO(unit)->gport_tree_created == TRUE) &&
            (input_node->parent_gport == parent_gport)) {
        return BCM_E_EXISTS;
    }


    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport)) {
        input_node->hw_index = SOC_FB6_CPU_MCQ_BASE + cosq;
    } else {
        input_node->level = FB6_CPU_SCHED_LEVEL_GET(unit, cosq);
        switch(input_node->level) {
            case SOC_FB6_NODE_LVL_L0:
                input_node->hw_index = _FB6_CPU_LO_NODE_BASE;
                break;
            case SOC_FB6_NODE_LVL_L1:
                input_node->hw_index = _FB6_CPU_L1_NODE_BASE + (cosq - _FB6_CPU_NUML0_NODES);
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(parent_gport)) {
        if(parent_node->level == SOC_FB6_NODE_LVL_L1) {
            parent_cos = parent_node->hw_index - _FB6_CPU_L1_NODE_BASE;
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_cpu_parent_set(unit, cosq, SOC_FB6_NODE_LVL_L2,
                                              parent_cos));
            /*
             * L2.0 is associated with CMC0 CH1(Rx)
             * L2.1 is associated with CMC0 CH2(Rx)
             * L2.2 is associated with CMC0 CH3(Rx)
             * L2.3 is associated with CMC0 CH4(Rx)
             * L2.4 is associated with CMC0 CH5(Rx)
             * L2.5 is associated with CMC0 CH6(Rx)
             * L2.6 is associated with CMC0 CH7(Rx)
             */
            if ((parent_cos >= 0) && (parent_cos < 7)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_rx_queue_channel_set(unit, parent_cos, cosq, 1));
            }
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
        if(input_node->level == SOC_FB6_NODE_LVL_L0) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_gport_get(unit, input_port,
                                        &(input_node->parent_gport)));
        } else {
            input_node->parent_gport = parent_gport;
        }
    }else if(BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport)){
        input_node->parent_gport = parent_gport;
    }


    return BCM_E_NONE;

}

static int
bcm_fb6_cosq_gport_level_get(int unit, bcm_port_t port, int cosq, int *lvl)
{
    int level = 0;

    if (NULL == lvl){
        return BCM_E_PARAM;
    }

    if (IS_UPLINK_PORT(unit, port)){
        level = (cosq < _FB6_HG_NUML0_NODES) ? SOC_FB6_NODE_LVL_L0 : SOC_FB6_NODE_LVL_L1;
    }else if (IS_STK_PORT(unit, port)){
        level = (cosq < _FB6_ST_NUML0_NODES) ? SOC_FB6_NODE_LVL_L0 : SOC_FB6_NODE_LVL_L1;
    }else {
        level = (cosq < _FB6_DL_NUML0_NODES) ? SOC_FB6_NODE_LVL_L0 : SOC_FB6_NODE_LVL_L1;
    }
    *lvl = level;


    return BCM_E_NONE;
}

static int
bcm_fb6_cosq_port_numl0_get(int unit, bcm_port_t port, int *num)
{
    int numl0 = 0;

    if (NULL == num){
        return BCM_E_PARAM;
    }

    if (IS_UPLINK_PORT(unit, port)){
        numl0 = _FB6_HG_NUML0_NODES;
    }else if (IS_STK_PORT(unit, port)){
        numl0 = _FB6_ST_NUML0_NODES;
    }else {
        numl0 = _FB6_DL_NUML0_NODES;
    }
    *num = numl0;
 
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_attach
 * Purpose:
 *     Attach sched_port to the specified index (cosq) of input_port
 * Parameters:
 *     unit          - (IN) unit number
 *     input_port    - (IN) GPORT id, that will attach to parent gport
 *     parent_gport  - (IN) Parent Gport to which current gport will be attached
 *     cosq          - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                          bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    _bcm_fb6_cosq_node_t *input_node = NULL;
    bcm_port_t input_port;
    int numl0, lvl, base_l0_num;
    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "bcm_fb6_cosq_gport_attach: unit=%d parent_gport=0x%x "
                    "input_gport=0x%x cosq=%d\n"), unit, parent_gport,
                    input_gport, cosq));

    if (!(BCM_GPORT_IS_SCHEDULER(input_gport) ||
        BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                               &input_node));

    if (!input_node) {
        return BCM_E_PARAM;
    }
    if (input_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, input_port)) {
        return _bcm_fb6_cosq_cpu_gport_attach(unit, input_gport, parent_gport,
                cosq);
    } else if ((_BCM_FB6_MMU_INFO(unit)->gport_tree_created == TRUE) &&
                    !(input_port >= _BCM_FB6_NUM_PORTS)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _soc_fb6_port_base_l0_mapping_get(unit, input_port, &base_l0_num, NULL));
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport)) {
        input_node->hw_index = (base_l0_num * 8) + cosq;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport)) {
        input_node->hw_index = (base_l0_num * 8) +  cosq;
    } else {
         if(input_port > _BCM_FB6_NUM_PORTS) {
            lvl = input_node->level;
         } else {
         BCM_IF_ERROR_RETURN(
             bcm_fb6_cosq_gport_level_get(unit, input_port, cosq,  &lvl));
         }
         BCM_IF_ERROR_RETURN(
             bcm_fb6_cosq_port_numl0_get(unit, input_port, &numl0));
        switch(lvl) {
            case SOC_FB6_NODE_LVL_L0:
                input_node->hw_index = base_l0_num + cosq;
                input_node->level = lvl;
                break;
            case SOC_FB6_NODE_LVL_L1:
                if(input_port > _BCM_FB6_NUM_PORTS) {
                    input_node->hw_index = (base_l0_num * 8) + cosq;
                }  else {
                    input_node->hw_index = (base_l0_num * 8) + (cosq - numl0);
                }
                input_node->level = lvl;
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
        if(input_node->level == SOC_FB6_NODE_LVL_L0) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_gport_get(unit, input_port,
                                        &(input_node->parent_gport)));
        } else {
            input_node->parent_gport = parent_gport;
        }
    } else if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport)) ||
                (BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        input_node->parent_gport = parent_gport;
    }

    if(input_port > _BCM_FB6_NUM_PORTS) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_sched_set(unit, input_node->gport, -1,
                                          BCM_COSQ_WEIGHTED_ROUND_ROBIN, 1));
    }
    return BCM_E_NONE;
}

int _bcm_fb6_cosq_reset_subport_sched(int unit, int subport)
{
    int index = 0, weight = 0, mindex = 0, mc_index = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem0 = Q_SCHED_L0_WEIGHT_MEM_PIPE0m;
    soc_mem_t mem1 = Q_SCHED_L1_WEIGHT_MEM_PIPE0m;
    soc_mem_t mem2 = Q_SCHED_L2_WEIGHT_MEM_PIPE0m;
    /* set the L0/L1/L2 node weights to 0 so that that node is treated as strict priority */
    index = subport;
    sal_memset(&entry, 0, sizeof(entry));
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem0, MEM_BLOCK_ALL, index, &entry));
    soc_mem_field32_set(unit, mem0, &entry, WEIGHTf, weight);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem0, MEM_BLOCK_ALL, index, &entry));

    index = subport * 8;
    mindex = index + 8;
    for(; index < mindex; index++){

        sal_memset(&entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem1, MEM_BLOCK_ALL, index, &entry));
        soc_mem_field32_set(unit, mem1, &entry, WEIGHTf, weight);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, index, &entry));

        sal_memset(&entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, index, &entry));
        soc_mem_field32_set(unit, mem2, &entry, WEIGHTf, weight);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, index, &entry));

        sal_memset(&entry, 0, sizeof(entry));
        mc_index = index + SOC_FB6_NUM_UC_QUEUES_PER_PIPE;
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, mc_index, &entry));
        soc_mem_field32_set(unit, mem2, &entry, WEIGHTf, weight);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, mc_index, &entry));
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *   bcm_fb6_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *       unit  - (IN) unit number
 *       gport - (IN) GPORT identifier
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_fb6_cosq_coe_port_info_t *coeport_info = NULL;
    bcm_port_t local_port;
    int sub_port;
     

    if(!_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)){
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
          _bcmi_coe_subport_physical_port_get(unit, gport, &local_port));

    if(!IS_COE_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (_bcm_fb6_cosq_coe_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_subport_resolve(unit, gport, &sub_port));

    coeport_info = &_bcm_fb6_cosq_coe_port_info[unit][sub_port];
    sal_memset(coeport_info, 0, sizeof(_bcm_fb6_cosq_coe_port_info_t));
    BCM_IF_ERROR_RETURN(
         _bcm_fb6_cosq_reset_subport_sched(unit, sub_port));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_add
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
bcm_fb6_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                       bcm_gport_t *gport)
{
    _bcm_fb6_cosq_coe_port_info_t *coeport_info = NULL;
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    bcm_port_t sub_port;
    uint32 sched_encap;
    int id;
    int num_uc, num_mc, num_sc;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_cosq_coe_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)){
        BCM_IF_ERROR_RETURN(
                _bcmi_coe_subport_physical_port_get(unit, port, &local_port));
    }  else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    }
    if (local_port < 0) {
       return BCM_E_PORT;
    }

    if(!IS_COE_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_subport_resolve(unit, port, &sub_port));

    coeport_info = &_bcm_fb6_cosq_coe_port_info[unit][sub_port];
    num_uc = num_mc = SOC_FB6_MMU_NUM_QUEUES_PER_PORT;
    num_sc = num_uc + 1;



    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            for (id = 0; id < num_uc; id++) {
                if ((coeport_info->ucast[id].numq == 0) ||
                        (coeport_info->ucast[id].in_use == 0)) {
                    break;
                }
            }
            if (id == num_uc) {
                return BCM_E_RESOURCE;
            }
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, sub_port, id);
            node = &coeport_info->ucast[id];
            node->level = SOC_FB6_NODE_LVL_L2;
            break;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            for (id = 0; id < num_mc; id++) {
                if ((coeport_info->mcast[id].numq == 0) ||
                        (coeport_info->mcast[id].in_use == 0)) {
                    break;
                }
            }
            if (id == num_mc) {
                return BCM_E_RESOURCE;
            }
            BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, sub_port, id);
            node = &coeport_info->mcast[id];
            node->level = SOC_FB6_NODE_LVL_L2;
            break;
        case BCM_COSQ_GPORT_SCHEDULER:
        case 0:
            if (numq <= 0) {
                return BCM_E_PARAM;
            }
            for (id = 0; id < num_sc; id++) {
                if (coeport_info->sched[id].numq == 0) {
                    break;
                }
            }
            if (id == num_sc) {
                return BCM_E_RESOURCE;
            }
            sched_encap = (id << 16) | sub_port;
            BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
            node = &coeport_info->sched[id];
            if(id) {
                node->level = SOC_FB6_NODE_LVL_L1;
            } else {
                node->level = SOC_FB6_NODE_LVL_L0;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    node->gport = *gport;
    node->numq = numq;
    node->in_use = 1;

    LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,"gport=0x%x\n"), *gport));

    return BCM_E_NONE;
}

    
/*
 * Function:
 *     _bcm_fb6_cosq_gport_add
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
STATIC int
_bcm_fb6_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                       bcm_gport_t *gport)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    uint32 sched_encap;
    int id;
    int num_uc, num_mc, num_sc;


    LOG_INFO(BSL_LS_BCM_COSQ,
        (BSL_META_U(unit,
                    "_bcm_fb6_cosq_gport_add: unit=%d port=0x%x "
                    "numq=%d flags=0x%x\n"), unit, port, numq, flags));

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0 || IS_LB_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
        num_uc = 0;
        num_mc = SOC_FB6_NUM_CPU_QUEUES;
        num_sc = SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT;
    } else if(IS_STK_PORT(unit, local_port)) {
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][local_port];
        num_uc = SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT;
        num_mc = SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT;
        num_sc = SOC_FB6_NUM_SCHEDULER_PER_ST_PORT;
    } else if(IS_UPLINK_PORT(unit, local_port)) {
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][local_port];
        num_uc = SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT;
        num_mc = SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT;
        num_sc = SOC_FB6_NUM_SCHEDULER_PER_HG_PORT;
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][local_port];
        num_uc = SOC_FB6_NUM_UCAST_QUEUE_PER_PORT;
        num_mc = SOC_FB6_NUM_MCAST_QUEUE_PER_PORT;
        num_sc = SOC_FB6_NUM_SCHEDULER_PER_PORT;
    }

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            if (IS_CPU_PORT(unit, local_port)) {
                return BCM_E_PARAM;
            }
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            for (id = 0; id < num_uc; id++) {
                if(IS_STK_PORT(unit,local_port)) {
                    if ((stport_info->ucast[id].numq == 0) ||
                        (stport_info->ucast[id].in_use == 0)) {
                         break;
                    }
                } else if(IS_UPLINK_PORT(unit,local_port)) {
                    if ((hgport_info->ucast[id].numq == 0) ||
                        (hgport_info->ucast[id].in_use == 0)) {
                         break;
                    }
                } else {
                    if ((port_info->ucast[id].numq == 0) ||
                        (port_info->ucast[id].in_use == 0)) {
                         break;
                    }
                }
            }
            if (id == num_uc) {
                return BCM_E_RESOURCE;
            }
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
            if (IS_STK_PORT(unit, local_port)){
               node = &stport_info->ucast[id];
            } else if (IS_UPLINK_PORT(unit, local_port)){
               node = &hgport_info->ucast[id];
            } else {
               node = &port_info->ucast[id];
            }
            node->level = SOC_FB6_NODE_LVL_L2;
            break;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            if (numq != 1) {
                return BCM_E_PARAM;
            }
            if (IS_CPU_PORT(unit, local_port)) {
                for (id = 0; id < SOC_FB6_NUM_CPU_QUEUES; id++) {
                    if ((cpu_port_info->mcast[id].numq == 0) ||
                        (cpu_port_info->mcast[id].in_use == 0)) {
                        break;
                    }
                }
                if (id == SOC_FB6_NUM_CPU_QUEUES) {
                    return BCM_E_RESOURCE;
                }
            } else {
                for (id = 0; id < num_mc; id++) {
                    if(IS_STK_PORT(unit, local_port)) {
                        if ((stport_info->mcast[id].numq == 0) ||
                            (stport_info->mcast[id].in_use == 0)) {
                             break;
                        }
                    } else if(IS_UPLINK_PORT(unit, local_port)) {
                        if ((hgport_info->mcast[id].numq == 0) ||
                            (hgport_info->mcast[id].in_use == 0)) {
                             break;
                        }
                     } else {
                        if ((port_info->mcast[id].numq == 0) ||
                            (port_info->mcast[id].in_use == 0)) {
                            break;
                        }
                    }
                }
                if (id == num_mc) {
                    return BCM_E_RESOURCE;
                }
            }
            BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
            if (IS_CPU_PORT(unit, local_port)) {
                node = &cpu_port_info->mcast[id];
            } else if ( IS_STK_PORT(unit, local_port)){
                node = &stport_info->mcast[id];
            } else if ( IS_UPLINK_PORT(unit, local_port)){
                node = &hgport_info->mcast[id];
            } else {
                node = &port_info->mcast[id];
            }
            node->level = SOC_FB6_NODE_LVL_L2;
            break;
        case BCM_COSQ_GPORT_SCHEDULER:
        case 0:
            if (numq <= 0) {
                return BCM_E_PARAM;
            }
            if (IS_CPU_PORT(unit, local_port)) {
                for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; id++) {
                    if (cpu_port_info->sched[id].numq == 0) {
                        break;
                    }
                    if (id == num_sc) {
                        return BCM_E_RESOURCE;
                    }
                }
                if (id == SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT) {
                    return BCM_E_RESOURCE;
                }

                sched_encap = (id << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
                node = &cpu_port_info->sched[id];
                node->level = SOC_FB6_NODE_LVL_L0;
            }

            if (IS_STK_PORT(unit, local_port) && !IS_CPU_PORT(unit, local_port)
                 && !IS_UPLINK_PORT(unit, local_port)){
                for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; id++) {
                    if (stport_info->sched[id].numq == 0) {
                        break;
                    }
                }
                if (id == SOC_FB6_NUM_SCHEDULER_PER_ST_PORT) {
                    return BCM_E_RESOURCE;
                }
                sched_encap = (id << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
                node = &stport_info->sched[id];
                node->level = SOC_FB6_NODE_LVL_L0;
            }

            if (!IS_STK_PORT(unit, local_port) && !IS_CPU_PORT(unit, local_port)
                 && IS_UPLINK_PORT(unit, local_port)){
                for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; id++) {
                    if (hgport_info->sched[id].numq == 0) {
                        break;
                    }
                }
                if (id == SOC_FB6_NUM_SCHEDULER_PER_HG_PORT) {
                    return BCM_E_RESOURCE;
               }
                sched_encap = (id << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
                node = &hgport_info->sched[id];
                node->level = SOC_FB6_NODE_LVL_L0;
             }

            if (!IS_STK_PORT(unit, local_port) && !IS_CPU_PORT(unit, local_port)
                 && !IS_UPLINK_PORT(unit, local_port)){
                for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_PORT; id++) {
                    if (port_info->sched[id].numq == 0) {
                        break;
                    }
                }
                if (id == SOC_FB6_NUM_SCHEDULER_PER_PORT) {
                    return BCM_E_RESOURCE;
               }
                sched_encap = (id << 16) | local_port;
                BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
                node = &port_info->sched[id];
                node->level = SOC_FB6_NODE_LVL_L0;
             }
            break;
        default:
            return BCM_E_PARAM;
    }

    if (NULL != node) {
        node->gport = *gport;
        node->numq = numq;
        node->in_use = 1;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,"gport=0x%x\n"), *gport));

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_cpu_tree_create(int unit, bcm_port_t port)
{
    int id = 0, sc_id = 0;
    bcm_gport_t sched_gport[SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT] = {0};
    bcm_gport_t cpu_mc_gport[SOC_FB6_NUM_CPU_QUEUES] = {0};
    int numq = 1 ;

    /*Creating all L0,L1 scheduler nodes for cpu port  */
    for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; id++) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                     BCM_COSQ_GPORT_SCHEDULER,
                                     &sched_gport[id]));
    }

    /* Attaching L0 schedular node to front panel port */
    BCM_IF_ERROR_RETURN
        (bcm_fb6_cosq_gport_attach(unit, sched_gport[0], port, 0));

    /* Attaching L1 schedular node to L0 schedular node  */
    for (sc_id = 1; sc_id < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; sc_id++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],  sched_gport[0], sc_id));
    }

    for (id = 0; id < SOC_FB6_NUM_CPU_QUEUES; id++) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                     BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                     &cpu_mc_gport[id]));
        /* Attach all the 48 CPU MC queues to L1.0 node */
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, cpu_mc_gport[id],
                                       sched_gport[1], id));
    }

    return BCM_E_NONE;

}

/*
 *  Function:
 *      bcm_fb6_cosq_gport_child_get
 *  Purpose:
 *      Get the child node GPORT atatched to N-th index (cosq)
 *      of the scheduler GPORT.
 *  Parameters:
 *      unit       - (IN)  Unit number.
 *      in_gport   - (IN)  Scheduler GPORT ID.
 *      cosq       - (IN)  COS queue attached to.
 *      out_gport  - (OUT) Child GPORT ID.
 *  Returns:
 *       BCM_E_XXX
 */

int
bcm_fb6_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
        bcm_cos_queue_t cosq, bcm_gport_t *out_gport)
{
    _bcm_fb6_cosq_node_t *tmp_node = NULL;
    bcm_port_t local_port = -1;
    _bcm_fb6_cosq_cpu_port_info_t *port_info = NULL;
    int index = -1, cpu_mc_base = 0;
    soc_info_t *si = &SOC_INFO(unit);

    if (out_gport == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, in_gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    /* This API is only supported for CPU ports because
     *        for all other ports we have fixed hierarchy */
    if (!IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];

    if (BCM_GPORT_IS_SCHEDULER(in_gport)) {
        for (index = 0; index < SOC_FB6_NUM_CPU_QUEUES; index++) {
            if (port_info->mcast[index].parent_gport == in_gport) {
                if (cosq == ((port_info->mcast[index].hw_index - cpu_mc_base) %
                            SOC_FB6_NUM_CPU_QUEUES)) {
                    tmp_node = &port_info->mcast[index];
                    break;
                }
            }
        }
    } else {
        return BCM_E_PARAM;
    }

    if (tmp_node == NULL) {
        return BCM_E_NOT_FOUND;
    }

    *out_gport = tmp_node->gport;

    return BCM_E_NONE;
}




/*
 * Function:
 *      _bcm_fb6_cosq_hg_gport_tree_create
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
STATIC int
_bcm_fb6_cosq_hg_gport_tree_create(int unit, bcm_port_t port)
{
    int id = 0, sc_id = 0;
    bcm_gport_t sched_gport[SOC_FB6_NUM_SCHEDULER_PER_HG_PORT] = {0};
    bcm_gport_t uc_gport[SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT] = {0};
    bcm_gport_t mc_gport[SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT] = {0};
    int numq = 1;

    /*Creating all L0,L1  scheduler nodes */
    for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; id++) {
        numq = (id < 4) ? 8 : 2;
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq, BCM_COSQ_GPORT_SCHEDULER,
                                     &sched_gport[id]));
    }

    /* Attaching L0 schedular node to front panel port */
    for (sc_id = 0; sc_id < 4; sc_id++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id], port, sc_id));
    }

    /* Aattaching L1 schedular nodes to  l0 Schedular nodes */
   for (sc_id = 4; sc_id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; sc_id++) {
        if (sc_id < 12) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[0], sc_id));
        } else if((sc_id >= 12) && (sc_id < 20)) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[1], sc_id));
        } else if((sc_id >= 20 ) && (sc_id < 28)) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[2], sc_id));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[3], sc_id));
        }
    }
    numq = 1;
   /* Creating and attaching l2 uc and l2 mc queues to  l1 Schedular nodes */
   for (id = 0, sc_id = 4; id < SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT &&
           sc_id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; id++, sc_id++) {
       BCM_IF_ERROR_RETURN
           (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                    &uc_gport[id]));
       BCM_IF_ERROR_RETURN
           (bcm_fb6_cosq_gport_attach(unit, uc_gport[id],
                                      sched_gport[sc_id], id));

       BCM_IF_ERROR_RETURN
           (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                    &mc_gport[id]));
       BCM_IF_ERROR_RETURN
           (bcm_fb6_cosq_gport_attach(unit, mc_gport[id],
                                      sched_gport[sc_id], id));
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb6_cosq_st_gport_tree_create
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
STATIC int
_bcm_fb6_cosq_st_gport_tree_create(int unit, bcm_port_t port)
{
    int id = 0, sc_id = 0;
    bcm_gport_t sched_gport[SOC_FB6_NUM_SCHEDULER_PER_ST_PORT] = {0};
    bcm_gport_t uc_gport[SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT] = {0};
    bcm_gport_t mc_gport[SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT] = {0};
    int numq = 1;

    /*Creating all L0,L1  scheduler nodes */
    for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; id++) {
        numq = (id < 3) ? 8 : 2;
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq, BCM_COSQ_GPORT_SCHEDULER,
                                     &sched_gport[id]));
    }

    /* Attaching L0 schedular node to front panel port */
    for (sc_id = 0; sc_id < 3; sc_id++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id], port, sc_id));
    }

    /* Aattaching L1 schedular nodes to  l0 Schedular nodes */
   for (sc_id = 3; sc_id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; sc_id++) {
        if (sc_id < 12) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[0], sc_id));
        } else if((sc_id >= 12) && (sc_id < 20)) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[1], sc_id));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id],
                                           sched_gport[2], sc_id));
        }
    }
    numq = 1;
   /* Creating and attaching l2 uc and l2 mc queues to  l1 Schedular nodes */
   for (id = 0, sc_id = 3; id < SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT &&
           sc_id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; id++, sc_id++) {
       BCM_IF_ERROR_RETURN
           (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                    &uc_gport[id]));
       BCM_IF_ERROR_RETURN
           (bcm_fb6_cosq_gport_attach(unit, uc_gport[id],
                                      sched_gport[sc_id], id));

       BCM_IF_ERROR_RETURN
           (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                    &mc_gport[id]));
       BCM_IF_ERROR_RETURN
           (bcm_fb6_cosq_gport_attach(unit, mc_gport[id],
                                      sched_gport[sc_id], id));
   }
   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb6_cosq_gport_tree_create
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
STATIC int
_bcm_fb6_cosq_gport_tree_create(int unit, bcm_port_t port)
{
    int id = 0, sc_id = 0;
    bcm_gport_t sched_gport[SOC_FB6_NUM_SCHEDULER_PER_PORT] = {0};
    bcm_gport_t uc_gport[SOC_FB6_NUM_UCAST_QUEUE_PER_PORT] = {0};
    bcm_gport_t mc_gport[SOC_FB6_NUM_MCAST_QUEUE_PER_PORT] = {0};
    int numq = 1;

    /*Creating all L0,L1 and L2 scheduler nodes */
    for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_PORT; id++) {
        numq = (id == 0) ? 8 : 2;

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                     BCM_COSQ_GPORT_SCHEDULER,
                                     &sched_gport[id]));
    }

    sc_id = 0;
    BCM_IF_ERROR_RETURN
        (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id], port, sc_id));

    /* Attaching L1 schedular nodes to L0 schedular node */
    for(sc_id = 1; sc_id < SOC_FB6_NUM_SCHEDULER_PER_PORT; sc_id++){
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, sched_gport[sc_id], sched_gport[0], sc_id));
    }
    numq = 1;
    /* Creating and attaching l2 uc and l2 mc queues to  l1 Schedular nodes */
    for (id = 0, sc_id = 1; id < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT &&
            sc_id < SOC_FB6_NUM_SCHEDULER_PER_PORT; id++, sc_id++) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                     BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                                     &uc_gport[id]));
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, uc_gport[id],
                                       sched_gport[sc_id], id));
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_gport_add(unit, port, numq,
                                     BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                     &mc_gport[id]));
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_attach(unit, mc_gport[id],
                                       sched_gport[sc_id], id));

    }
    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_fb6_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of input_port
 * Parameters:
 *     unit        - (IN) unit number
 *     input_port  - (IN) scheduler GPORT identifier
 *     parent_port - (IN) GPORT to detach from
 *     cosq        - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_detach(int unit, bcm_gport_t input_gport,
        bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    _bcm_fb6_cosq_node_t *input_node = NULL;
    bcm_port_t input_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,
                        "bcm_td3_cosq_gport_detach: unit=%d input_gport=0x%x "
                        "parent_gport=0x%x cosq=%d\n"),
             unit, input_gport, parent_gport, cosq));

    if (!(BCM_GPORT_IS_SCHEDULER(input_gport) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(input_gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(input_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                &input_node));

    if (!input_node) {
        return BCM_E_PARAM;
    }

    if (_BCM_FB6_MMU_INFO(unit)->gport_tree_created == TRUE) {
        if (IS_CPU_PORT(unit, input_port)) {
            return _bcm_fb6_cosq_cpu_gport_detach(unit, input_gport, parent_gport,
                    cosq);
        } else {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_attach_get
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
        bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port = -1;
    int base_l0_num = 0;
    soc_info_t *si;
    si = &SOC_INFO(unit);

    if (!(BCM_GPORT_IS_SET(sched_gport) ||
                BCM_GPORT_IS_SCHEDULER(sched_gport) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport))) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, sched_gport, NULL, &local_port, NULL,
                                &node));

    if (node == NULL) {
        return BCM_E_PARAM;
    }

    if (local_port < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _soc_fb6_port_base_l0_mapping_get(unit, local_port, &base_l0_num, NULL));

    if (BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        switch(node->level) {
            case SOC_FB6_NODE_LVL_L0:
                if (IS_CPU_PORT(unit, local_port)) {
                    *cosq = node->hw_index - _FB6_CPU_LO_NODE_BASE;
                } else  {
                    *cosq = node->hw_index - base_l0_num;
                }
                break;
            case SOC_FB6_NODE_LVL_L1:
                if (IS_CPU_PORT(unit, local_port)) {
                    *cosq = node->hw_index - _FB6_CPU_L1_NODE_BASE;
                } else {
                    *cosq = node->hw_index - (base_l0_num * 8);
                }
                break;
            default:
                return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(sched_gport)) {
        *cosq = node->hw_index - si->port_uc_cosq_base[local_port];;
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(sched_gport)) {
        if(IS_CPU_PORT(unit, local_port)) {
            *cosq = node->hw_index - _FB6_CPU_L2_NODE_BASE;
        } else {
            *cosq = node->hw_index - si->port_cosq_base[local_port];;
        }
    } else {
        return BCM_E_PARAM;
    }

    *input_gport = node->parent_gport;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_traverse
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
bcm_fb6_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
        void *user_data)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port, port_out, local_port;
    bcm_gport_t gport;
    int id, rv = SOC_E_NONE;


    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, my_modid,  port,
                                     &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(gport, modid_out, port_out);

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

        if (IS_CPU_PORT(unit, local_port)) {
            cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
        } else if(IS_UPLINK_PORT(unit, local_port)) {
            hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][local_port];
        } else if(IS_STK_PORT(unit, local_port)) {
            stport_info = &_bcm_fb6_cosq_st_port_info[unit][local_port];
        } else {
            port_info = &_bcm_fb6_cosq_port_info[unit][local_port];
        }

        if (!IS_CPU_PORT(unit, local_port) && !IS_UPLINK_PORT(unit, local_port) &&
            !IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; id++) {
                if (port_info->ucast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, port_info->ucast[id].numq,
                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                        port_info->ucast[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }

        if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT; id++) {
                if (stport_info != NULL) {
                    if (stport_info->ucast[id].numq == 0) {
                        continue;
                    }
                    rv = cb(unit, gport, stport_info->ucast[id].numq,
                            BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                            stport_info->ucast[id].gport, user_data);
                }
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }

        if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT; id++) {
                if (hgport_info->ucast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->ucast[id].numq,
                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                        hgport_info->ucast[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }
        if (IS_CPU_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_CPU_QUEUES; id++) {
                if (cpu_port_info->mcast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, cpu_port_info->mcast[id].numq,
                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                        cpu_port_info->mcast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    return rv;
                }
#else
                rv = BCM_E_NONE;
#endif
            }
        }

        if (!IS_CPU_PORT(unit, local_port) && !IS_UPLINK_PORT(unit, local_port) &&
            !IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; id++) {
                if (port_info->mcast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, port_info->mcast[id].numq,
                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                        port_info->mcast[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }


        if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT; id++) {
                if (stport_info != NULL) {
                    if (stport_info->mcast[id].numq == 0) {
                        continue;
                    }
                    rv = cb(unit, gport, stport_info->mcast[id].numq,
                            BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                            stport_info->mcast[id].gport, user_data);
                }
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }

        if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT; id++) {
                if (hgport_info->mcast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->mcast[id].numq,
                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                        hgport_info->mcast[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }

        if (IS_CPU_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; id++) {
                if (cpu_port_info->sched[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, cpu_port_info->sched[id].numq,
                        BCM_COSQ_GPORT_SCHEDULER,
                        cpu_port_info->sched[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    return rv;
                }
#else
                rv = BCM_E_NONE;
#endif
            }
        }

        if (!IS_CPU_PORT(unit, local_port) && !IS_UPLINK_PORT(unit, local_port) &&
             !IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_PORT; id++) {
                if (port_info->sched[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, port_info->sched[id].numq,
                        BCM_COSQ_GPORT_SCHEDULER,
                        port_info->sched[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }


        if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; id++) {
                if (stport_info != NULL) {
                    if (stport_info->sched[id].numq == 0) {
                        continue;
                    }
                    rv = cb(unit, gport, stport_info->sched[id].numq,
                            BCM_COSQ_GPORT_SCHEDULER,
                            stport_info->sched[id].gport, user_data);
                }
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }

        if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
            for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; id++) {
                if (hgport_info->sched[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->sched[id].numq,
                        BCM_COSQ_GPORT_SCHEDULER,
                        hgport_info->sched[id].gport, user_data);
            }
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif

        }
   }
   return rv;
}

/*
 * Function:
 *    bcm_cosq_gport_traverse_by_port
 * Purpose:
 *     Walks through the valid COSQ GPORTs belong to a port and calls
 *     the user supplied callback function for each entry.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number or GPORT identifier
 *     trav_fn    - (IN) Callback function.
 *     user_data  - (IN) User data to be passed to callback function
 * Returns:
 *     BCM_E_NONE - Success.
 *     BCM_E_XXX
 */

int
bcm_fb6_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                            bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port_out, local_port;
    bcm_gport_t gport;
    int id, rv = SOC_E_NONE;

    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SET(port) &&
            (!BCM_GPORT_IS_LOCAL(port) &&
             !BCM_GPORT_IS_DEVPORT(port) &&
             !BCM_GPORT_IS_MODPORT(port))) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                        "Invalid gport type\n")));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));


    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, my_modid,  port,
                                 &modid_out, &port_out));
    BCM_GPORT_MODPORT_SET(gport, modid_out, port_out);

    if (IS_CPU_PORT(unit, local_port)) {
        cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    } else if(IS_STK_PORT(unit, local_port)) {
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][local_port];
    } else if(IS_UPLINK_PORT(unit, local_port)) {
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][local_port];
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][local_port];
    }

    if (!IS_CPU_PORT(unit, local_port) && !IS_STK_PORT(unit, local_port) &&
         !IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; id++) {
            if (port_info->ucast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport, port_info->ucast[id].numq,
                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                    port_info->ucast[id].gport, user_data);
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }

    if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT; id++) {
            if (stport_info != NULL) {
                if (stport_info->ucast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, stport_info->ucast[id].numq,
                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                        stport_info->ucast[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }

    if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT; id++) {
            if (hgport_info != NULL) {
                if (hgport_info->ucast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->ucast[id].numq,
                        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                        hgport_info->ucast[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }
    if (IS_CPU_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_CPU_QUEUES; id++) {
            if (cpu_port_info->mcast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport, cpu_port_info->mcast[id].numq,
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                    cpu_port_info->mcast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }
    }

    if (!IS_CPU_PORT(unit, local_port) && !IS_STK_PORT(unit, local_port) &&
        !IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; id++) {
            if (port_info->mcast[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport, port_info->mcast[id].numq,
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                    port_info->mcast[id].gport, user_data);
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }


    if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT; id++) {
            if (stport_info != NULL) {
                if (stport_info->mcast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, stport_info->mcast[id].numq,
                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                        stport_info->mcast[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }

    if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT; id++) {
            if (hgport_info != NULL) {
                if (hgport_info->mcast[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->mcast[id].numq,
                        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                        hgport_info->mcast[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
   }

    if (IS_CPU_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; id++) {
            if (cpu_port_info->sched[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport, cpu_port_info->sched[id].numq,
                    BCM_COSQ_GPORT_SCHEDULER,
                    cpu_port_info->sched[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#else
            rv = BCM_E_NONE;
#endif
        }
    }

    if (!IS_CPU_PORT(unit, local_port) && !IS_STK_PORT(unit, local_port) &&
        !IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_PORT; id++) {
            if (port_info->sched[id].numq == 0) {
                continue;
            }
            rv = cb(unit, gport, port_info->sched[id].numq,
                    BCM_COSQ_GPORT_SCHEDULER,
                    port_info->sched[id].gport, user_data);
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }


    if (!IS_CPU_PORT(unit, local_port) && IS_STK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; id++) {
            if (stport_info != NULL) {
                if (stport_info->sched[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, stport_info->sched[id].numq,
                        BCM_COSQ_GPORT_SCHEDULER,
                        stport_info->sched[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }

    if (!IS_CPU_PORT(unit, local_port) && IS_UPLINK_PORT(unit, local_port)) {
        for (id = 0; id < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; id++) {
            if (hgport_info != NULL) {
                if (hgport_info->sched[id].numq == 0) {
                    continue;
                }
                rv = cb(unit, gport, hgport_info->sched[id].numq,
                        BCM_COSQ_GPORT_SCHEDULER,
                        hgport_info->sched[id].gport, user_data);
            }
        }
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            return rv;
        }
#else
        rv = BCM_E_NONE;
#endif
    }
    return rv;
}

/*
 * Function:
 *     _bcm_fb6_sched_weight_setget
 * Purpose:
 *     Get/Set weights for given COS policy
 * Parameters:
 *     unit     - (IN)  unit number
 *     port     - (IN)  port number
 *     level    - (IN)  node level
 *     index    - (IN)  COS queue number
 *     weights  - (OUT) Weights for COS queue.
 *     mc       - (IN)  Multicast queue group
 *     setget   - (IN)  Set/get flag
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_sched_weight_setget(int unit, int port, int level, int index,
        int *weight, int mc, int setget)
{
    soc_mem_t mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe = 0, base_l0_num;

    BCM_IF_ERROR_RETURN(
        _soc_fb6_port_base_l0_mapping_get(unit, port, &base_l0_num, NULL));
    switch(level) {
        case SOC_FB6_NODE_LVL_L0:
            mem = Q_SCHED_L0_WEIGHT_MEMm;
            if(IS_CPU_PORT(unit, port)) {
                index = _FB6_CPU_LO_NODE_BASE;
            } else {
                index +=  base_l0_num;
            }
            break;

        case SOC_FB6_NODE_LVL_L1:
            mem = Q_SCHED_L1_WEIGHT_MEMm;
            if(IS_CPU_PORT(unit, port)) {
                index += _FB6_CPU_L1_NODE_BASE;
            } else {
                index +=  (base_l0_num * 8);
            }
            break;

        case SOC_FB6_NODE_LVL_L2:
            mem = Q_SCHED_L2_WEIGHT_MEMm;
            if(IS_CPU_PORT(unit, port)) {
                index += _FB6_CPU_L2_NODE_BASE;
            } else {
                index +=  (base_l0_num * 8);
            }
            if(mc == 1) {
                index += SOC_FB6_NUM_UC_QUEUES_PER_PIPE;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

    if (setget == 1) {
        soc_mem_field32_set(unit, mem, &entry, WEIGHTf, *weight);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
    } else {
        *weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_fb6_cosq_sched_mode_setget
 * Purpose:
 *     Get/Set scheduling mode
 * Parameters:
 *     unit     - (IN)  Unit number
 *     port     - (IN)  Port number
 *     level    - (IN)  Node level
 *     index    - (IN)  COS queue number
 *     mode     - (IN/OUT) Scheduling mode (BCM_COSQ_xxx)
 *     weights  - (IN/OUT) Weights for COS queue.
 *     mc       - (IN)  Multicast queue group
 *     setget   - (IN)  Set/get flag
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_sched_mode_setget(int unit, bcm_port_t port, int level, int index,
        soc_fb6_sched_mode_e *mode, int *weight, int mc,
        int setget)
{
    uint32 fval = 0, rval, wrr_mask = 0;
    int base_l0_num;
    soc_reg_t reg = INVALIDr;
    l1_sched_wrr_config_ram_entry_t  config_mem;
    soc_mem_t mem = L1_SCHED_WRR_CONFIG_RAM_PIPE0m;
    int fport = 0;
    int parent_index = 0;

    if (setget == 1) {
        LOG_INFO(BSL_LS_SOC_COSQ,
                (BSL_META_U(unit,
                            "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
                 port, (level == 0) ? "r" : "", level - 1, index, *mode,
                 *weight));
    }


    BCM_IF_ERROR_RETURN(
            _soc_fb6_port_base_l0_mapping_get(unit, port, &base_l0_num, NULL));

    /* Selection between SP and WxRR is based on weight property. */
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_sched_weight_setget(unit, port, level, index, weight, mc,
                                      setget));

    if ((level == SOC_FB6_NODE_LVL_L1) && (IS_CPU_PORT(unit, port))) {
        return BCM_E_NONE;
    }
    sal_memset(&config_mem, 0, sizeof(config_mem));
    if (!IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                          base_l0_num, &config_mem));
    }

    switch(level) {
        case SOC_FB6_NODE_LVL_L0:
            reg = IS_CPU_PORT(unit, port) ? Q_SCHED_CPU_PORT_CONFIGr : Q_SCHED_PORT_CONFIGr;
            index = 0;
            break;

        case SOC_FB6_NODE_LVL_L1:
            index = 0;
            wrr_mask = soc_mem_field32_get(unit, mem, &config_mem, ENABLE_WRR_L0f);
            break;

        case SOC_FB6_NODE_LVL_L2:
            index = index % 8;
            wrr_mask = soc_mem_field32_get(unit, mem, &config_mem, ENABLE_WRR_NODEf);
            if (IS_CPU_PORT(unit, port)) {
                reg = Q_SCHED_CPU_L1_NODE_CONFIGr;
                SOC_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_cpu_parent_get(unit, index, SOC_FB6_NODE_LVL_L2,
                                                  &parent_index));
                index = parent_index;
            }
            break;

        default:
            return BCM_E_PARAM;

    }

    if ((level == SOC_FB6_NODE_LVL_L0) ||
            ((level == SOC_FB6_NODE_LVL_L2) && (IS_CPU_PORT(unit, port)))) {
        if(port > _BCM_FB6_NUM_PORTS) {
            BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, port, &fport));
            port = fport;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
    }

    if (setget == 1) {
        if (*mode == SOC_FB6_SCHED_MODE_WRR) {
            fval = 1;
        } else if (*mode == SOC_FB6_SCHED_MODE_WERR) {
            fval = 0;
        } else {
            return BCM_E_NONE;
        }

        if (level == SOC_FB6_NODE_LVL_L0) {
            wrr_mask &= ~(1 << index);
            wrr_mask |= (fval << index);
            soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }else if (level == SOC_FB6_NODE_LVL_L1) {
            if (!IS_CPU_PORT(unit , port)){
                wrr_mask &= ~(1 << index);
                wrr_mask |= (fval << index);
                soc_mem_field32_set(unit, mem, &config_mem, ENABLE_WRR_L0f, wrr_mask);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                   base_l0_num, &config_mem));
            }

        }else if (level == SOC_FB6_NODE_LVL_L2) {
            if (IS_CPU_PORT(unit, port)) {
                wrr_mask &= ~(1 << index);
                wrr_mask |= (fval << index);
                soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
            } else {
                wrr_mask &= ~(1 << index);
                wrr_mask |= (fval << index);
                soc_mem_field32_set(unit, mem, &config_mem, ENABLE_WRR_NODEf, wrr_mask);
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                   base_l0_num, &config_mem));
            }
        }
    } else {
        if (*weight == 0) {
            *mode = SOC_FB6_SCHED_MODE_STRICT;
        } else if (wrr_mask & (1 << index)) {
            *mode = SOC_FB6_SCHED_MODE_WRR;
        } else {
            *mode = SOC_FB6_SCHED_MODE_WERR;
        }
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *     _bcm_fb6_cosq_sched_set
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) GPORT identifier
 *     cosq     - (IN) COS queue number
 *     mode     - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weights  - (IN) Weight for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int mode, int weight)
{
    soc_fb6_sched_mode_e sched_mode = 0, get_mode = 0;
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    int lwts = 1, child_index=0, lvl = SOC_FB6_NODE_LVL_L1, mc = 0;
    int get_wt = 0;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else {
            return BCM_E_PARAM;
        }
    }

    if ((weight < 0) || (weight > 127)) {
        return BCM_E_PARAM;
    }

    switch(mode) {
        case BCM_COSQ_STRICT:
            sched_mode = SOC_FB6_SCHED_MODE_STRICT;
            lwts = 0;
            break;
        case BCM_COSQ_ROUND_ROBIN:
            sched_mode = SOC_FB6_SCHED_MODE_WRR;
            lwts = 1;
            break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            sched_mode = SOC_FB6_SCHED_MODE_WRR;
            lwts = weight;
            break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            sched_mode = SOC_FB6_SCHED_MODE_WERR;
            lwts = weight;
            break;
        default:
            return BCM_E_PARAM;
    }

    /* Weight = 0 corresponds to Strict Mode */
    if (lwts == 0) {
        sched_mode = SOC_FB6_SCHED_MODE_STRICT;
    }

    if(BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                    &node));
        lvl = node->level;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_localport_resolve(unit, gport,
                    &local_port));
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        mc = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ? 1 : 0;
    } else if (IS_CPU_PORT(unit, local_port)) {
        if(BCM_GPORT_IS_SET(gport)) {
            if(lvl == SOC_FB6_NODE_LVL_L2) {
                mc = 1;
            }
        } else {
            lvl = SOC_FB6_NODE_LVL_L2;
            mc = 1;
        }
    }

    child_index = cosq;

    if((local_port > _BCM_FB6_NUM_PORTS) && (lvl == SOC_FB6_NODE_LVL_L0)) {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_sched_mode_setget(unit, local_port, lvl,
                    child_index, &get_mode, &get_wt, mc, 0));
        if ((get_mode != SOC_FB6_SCHED_MODE_WRR) && (sched_mode == SOC_FB6_SCHED_MODE_WRR)) {
            BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_sched_mode_setget(unit, local_port, lvl,
                        child_index, &sched_mode, &lwts, mc, 1));
        } else {
            return BCM_E_PARAM;
        }

    } else {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_sched_mode_setget(unit, local_port, lvl,
                    child_index, &sched_mode, &lwts, mc, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_fb6_cosq_node_hw_index_get
 * Purpose:
 *    Get cosq node hardware index
 * Parameters:
 *      unit        - (IN) unit number
 *      gport       - (IN) GPORT identifier
 *      cosq        - (OUT) COS queue number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_node_hw_index_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t *cosq)
{
    _bcm_fb6_cosq_node_t *node = NULL;
    int local_port = -1, cpu_mc_base = 0;
    int base_l0_num = 0;
    soc_info_t *si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                &node));

    BCM_IF_ERROR_RETURN(
            _soc_fb6_port_base_l0_mapping_get(unit, local_port, &base_l0_num, NULL));

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        *cosq = node->hw_index - (base_l0_num * 8);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        /* For CPU ports, num MC queues are 48 */
        if (IS_CPU_PORT(unit, local_port)) {
            cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
            *cosq = (node->hw_index - cpu_mc_base) %
                SOC_FB6_NUM_CPU_QUEUES;
        } else {
            *cosq = node->hw_index - (base_l0_num * 8);
        }
    } else {
        switch(node->level) {
            case SOC_FB6_NODE_LVL_L0:
                if(IS_CPU_PORT(unit, local_port)){
                     *cosq = node->hw_index - _FB6_CPU_LO_NODE_BASE;
                } else {
                    *cosq = node->hw_index - base_l0_num;
                }
                break;
            case SOC_FB6_NODE_LVL_L1:
                if(IS_CPU_PORT(unit, local_port)){
                    *cosq = node->hw_index - _FB6_CPU_L1_NODE_BASE;
                } else {
                    *cosq = node->hw_index - (base_l0_num * 8);
                }
                break;
            default :
                return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_sched_set
 * Purpose:
 *     Set up class-of-service policy and corresponding weights
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) GPORT identifier
 *     cosq     - (IN) COS queue to get
 *     mode     - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight   - (IN) Weight for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_sched_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int mode, int weight)
{
    soc_info_t *si = &SOC_INFO(unit);
    int local_port = -1;

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_SCHEDULER(gport) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_hw_index_get(unit, gport, &cosq));

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_sched_set(unit, gport, cosq, mode, weight));
            return BCM_E_NONE;

        } else {
            BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_localport_resolve(unit, gport,
                        &local_port));

            if ((cosq < 0) || (cosq >= si->port_num_cosq[local_port])) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_sched_set(unit, local_port, cosq, mode, weight));
            return BCM_E_NONE;
        }
    } else {
        return BCM_E_PORT;
    }
    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_fb6_cosq_sched_config_port
 * Purpose:
 *      Set scheduler Configuration for the specified port
 * Parameters:
 *      unit        - (IN) unit number.
 *      port        - (IN) port number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_sched_config_port(int unit, int port, int mode, int weight)
{
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    int i;

    if (IS_STK_PORT(unit, port)) {
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][port];

        for (i = 0; i < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, stport_info->sched[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, stport_info->ucast[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, stport_info->mcast[i].gport,
                                              -1, mode, weight));
        }
    } else if(IS_UPLINK_PORT(unit, port))  {
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][port];

        for (i = 0; i < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, hgport_info->sched[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, hgport_info->ucast[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, hgport_info->mcast[i].gport,
                                              -1, mode, weight));
        }
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][port];

        for (i = 0; i < SOC_FB6_NUM_SCHEDULER_PER_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, port_info->sched[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, port_info->ucast[i].gport,
                                              -1, mode, weight));
        }
        for (i = 0; i < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; i++) {
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_sched_set(unit, port_info->mcast[i].gport,
                                              -1, mode, weight));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb6_cosq_sched_default_config
 * Purpose:
 *      Set Default Scheduler Configuration for all the ports
 * Parameters:
 *      unit        - (IN) unit number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_sched_default_config(int unit)
{
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    bcm_port_t port;
    int default_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN, default_weight = 1;
    int i;

    port = 0;
    cpu_port_info = &_bcm_fb6_cosq_cpu_port_info[unit][port];
    for (i = 0; i < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_sched_set(unit, cpu_port_info->sched[i].gport,
                                          -1, default_mode, default_weight));
    }
    for (i = 0; i < SOC_FB6_NUM_CPU_QUEUES; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_fb6_cosq_gport_sched_set(unit, cpu_port_info->mcast[i].gport,
                                          -1, default_mode, default_weight));
    }

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_FAE_PORT(unit, port)
                ||  (IS_COE_PORT(unit, port))) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_sched_config_port(unit, port, default_mode,
                                             default_weight));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_gport_default_config(int unit)
{
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_sched_default_config(unit));

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_ingress_pg_get(int unit, bcm_gport_t gport, bcm_cos_queue_t pri,
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
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (pri >= _FB6_MMU_NUM_INT_PRI) {
        return BCM_E_PARAM;
    }
    /* get PG for port using Port+Cos */
    if (pri < (_FB6_MMU_NUM_INT_PRI / 2)) {
        reg = prigroup_reg[0];
    } else {
        reg = prigroup_reg[1];
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, local_port, 0, &rval));
    pool = soc_reg_field_get(unit, reg, rval, prigroup_field[pri]);

    if (p_pg_start) {
        *p_pg_start = pool;
    }
    if (p_pg_end) {
        *p_pg_end = pool;
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get headroom pool
 * for given Ingress [Port, Priority]
 */
STATIC int
_bcm_fb6_cosq_ingress_hdrm_pool_get(int unit, bcm_gport_t gport,
        bcm_cos_t pri,
        int *p_hdrm_pool_start,
        int *p_hdrm_pool_end)
{
    int local_port;
    uint32 rval, hdrm_pool;
    int pri_grp;
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    if (pri == BCM_COS_INVALID) {
        if (p_hdrm_pool_start && p_hdrm_pool_end) {
            *p_hdrm_pool_start = 0;
            *p_hdrm_pool_end = 3;
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
    }

    if ((pri < 0) || (pri >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, pri, &pri_grp, &pri_grp));

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (pri_grp >= _FB6_MMU_NUM_PG) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_HDRM_PORT_PG_HPIDr(unit, local_port, &rval));
    hdrm_pool = soc_reg_field_get(unit, THDI_HDRM_PORT_PG_HPIDr, rval,
            prigroup_hpid_field[pri_grp]);

    if (p_hdrm_pool_start) {
        *p_hdrm_pool_start = hdrm_pool;
    }
    if (p_hdrm_pool_end) {
        *p_hdrm_pool_end = hdrm_pool;
    }

    return BCM_E_NONE;
}




/*
 * This function is used to get shared pool
 * for given Ingress [Port, Priority]
 */
STATIC int
_bcm_fb6_cosq_ingress_sp_get(int unit, bcm_gport_t gport, bcm_cos_t pri,
        int *p_pool_start, int *p_pool_end)
{
    int local_port;
    uint32 rval, pool;
    int pri_grp;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (pri == BCM_COS_INVALID) {
        if (p_pool_start && p_pool_end) {
            *p_pool_start = 0;
            *p_pool_end = 3;
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
    }

    if ((pri < 0) || (pri >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, pri, &pri_grp, &pri_grp));

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (pri_grp >= _FB6_MMU_NUM_PG) {
        return BCM_E_PARAM;
    }


    SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, local_port, &rval));
    pool = soc_reg_field_get(unit, THDI_PORT_PG_SPIDr, rval,
            prigroup_spid_field[pri_grp]);

    if (p_pool_start) {
        *p_pool_start = pool;
    }
    if (p_pool_end) {
        *p_pool_end = pool;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_egress_sp_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
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
        (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
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
 *  This function is used to set headroom pool
 *  or shared pool for given Ingress [Port, Priority]
 */
STATIC int
_bcm_fb6_cosq_ing_pool_set(int unit, bcm_gport_t gport, bcm_cos_t pri,
        bcm_cosq_control_t type, int arg)
{
    int local_port;
    uint32 rval;
    int pri_grp;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    static const soc_field_t prigroup_hpid_field[] = {
        PG0_HPIDf, PG1_HPIDf, PG2_HPIDf, PG3_HPIDf,
        PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf
    };

    if((arg < 0) || (arg >= _FB6_MMU_NUM_POOL)) {
        return BCM_E_PARAM;
    }

    if ((pri < 0) || (pri >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, pri, &pri_grp, &pri_grp));

    /* coverity fix : pri_grp can take value from 0 to 7 */
    if ((pri_grp < 0) || (pri_grp >= _FB6_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPool) {
        SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, local_port, &rval));
        soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                prigroup_spid_field[pri_grp],arg);
        BCM_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, local_port, rval));
    } else if (type == bcmCosqControlIngressHeadroomPool) {
        SOC_IF_ERROR_RETURN(READ_THDI_HDRM_PORT_PG_HPIDr(unit, local_port, &rval));
        soc_reg_field_set(unit, THDI_HDRM_PORT_PG_HPIDr, &rval,
                prigroup_hpid_field[pri_grp], arg);
        BCM_IF_ERROR_RETURN(WRITE_THDI_HDRM_PORT_PG_HPIDr(unit, local_port, rval));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_ing_pool_get(int unit, bcm_gport_t gport,
        bcm_cos_t pri,
        bcm_cosq_control_t type, int *arg)
{
    int pool_start_idx = 0, pool_end_idx = 0;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if ((pri < 0) || (pri >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPool) {
        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_ingress_sp_get(unit, gport, pri,
                    &pool_start_idx,
                    &pool_end_idx));
    } else if (type == bcmCosqControlIngressHeadroomPool) {
        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_ingress_hdrm_pool_get(unit, gport, pri,
                    &pool_start_idx,
                    &pool_end_idx));
    } else {
        return BCM_E_PARAM;
    }
    *arg = pool_start_idx;
    return BCM_E_NONE;
}

/*
 *Function:
 *   bcm_fb6_cosq_gport_bandwidth_burst_set
 *Purpose:
 *   Configure COS queue bandwidth burst setting
 *Parameters:
 *   unit        - (IN) unit number
 *   gport       - (IN) GPORT identifier
 *   cosq        - (IN) COS queue to configure, -1 for all COS queues
 *   kbits_burst - (IN) maximum burst, kbits
 *Returns:
 *   BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_bandwidth_burst_set(int unit,
        bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        uint32 kbits_burst_min,
        uint32 kbits_burst_max)
{
    int i, start_cos=0, end_cos=0;
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst, flags;
    bcm_port_t local_port;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    start_cos = end_cos = cosq;

    if (cosq == BCM_COS_INVALID) {
        start_cos = 0;
        if (IS_CPU_PORT(unit, local_port)) {
            end_cos =  NUM_CPU_COSQ(unit);
        } else if(IS_STK_PORT(unit, local_port)) {
            end_cos = SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT;
        } else if (IS_UPLINK_PORT(unit, local_port)){
            end_cos = SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT;
        } else {
            end_cos = SOC_FB6_NUM_UCAST_QUEUE_PER_PORT;
        }
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
            (_bcm_fb6_cosq_bucket_get(unit, gport, i, &kbits_sec_min,
                                      &kbits_sec_max, &kbits_sec_burst,
                                      &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                      kbits_sec_max, kbits_burst_min,
                                      kbits_burst_max, flags));
    }
    return BCM_E_NONE;
}

/*
 *Function:
 *   bcm_fb6_cosq_gport_bandwidth_burst_get
 *Purpose:
 *   Get COS queue bandwidth burst setting
 *Parameters:
 *   unit        - (IN) unit number
 *   gport       - (IN) GPORT identifier
 *   cosq        - (IN) COS queue to get, -1 for Queue 0
 *   kbits_burst - (OUT) maximum burst, kbits
 *Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_bandwidth_burst_get(int unit,
        bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        uint32 *kbits_burst_min,
        uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max, flags;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                  &kbits_sec_min, &kbits_sec_max, kbits_burst_min,
                                  kbits_burst_max, &flags));

    return BCM_E_NONE;
}

uint32 _bcm_fb6_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

/*
 * Function used for index calculation of certain memories,
 * where index is derived from more than one key.
 * Eg. (Port, SP) or (Port, PG)
 * Note: If no special indexig required, return the same index.
 */
uint32 _bcm_fb6_mmu_piped_mem_index(int unit, soc_port_t port,
        soc_mem_t mem, int arr_off)
{
    int     mmu_port, index;

    mmu_port = _bcm_fb6_mmu_port(unit, port);

    switch (mem) {
        case MMU_THDM_DB_PORTSP_CONFIGm:
        case MMU_THDM_MCQE_PORTSP_CONFIGm:
            index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * _FB6_MMU_NUM_POOL)
                + arr_off;
            break;
        case THDI_PORT_SP_CONFIGm:
        case MMU_THDU_CONFIG_PORTm:
        case MMU_THDU_RESUME_PORTm:
        case MMU_THDU_COUNTER_PORTm:
            index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * _FB6_MMU_NUM_POOL)
                + arr_off;
            break;
        case THDI_PORT_PG_CONFIGm:
            index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * _FB6_MMU_NUM_PG)
                + arr_off;
            break;
        default:
            return arr_off;
    }

    return index;
}
STATIC int
_bcm_fb6_cosq_egr_pool_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, pool, midx;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t base_mem = INVALIDm, base_mem2 = INVALIDm;
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1, pipe = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));


    base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    midx, entry));

        soc_mem_field32_set(unit, mem, entry, SHARED_LIMIT_ENABLEf, !!arg);

        return soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry);
    }
    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
            type == bcmCosqControlMCEgressPool) {
        if((arg < 0) || (arg >= _FB6_MMU_NUM_POOL)) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            base_mem2 = MMU_THDM_DB_QUEUE_CONFIGm;
            mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];
        } else  { /* local port */
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
                mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
                mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
                base_mem2 = MMU_THDM_DB_QUEUE_CONFIGm;
                mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];
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

    arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;

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
    if (type == bcmCosqControlEgressPoolHighCongestionLimitBytes ||
            type == bcmCosqControlEgressPoolLowCongestionLimitBytes) {
        uint64 rval64;
        soc_reg_t reg64;
        soc_reg_t wred_cng_lmt[4] = {
            WRED_POOL_INST_CONG_LIMIT_0r,
            WRED_POOL_INST_CONG_LIMIT_1r,
            WRED_POOL_INST_CONG_LIMIT_2r,
            WRED_POOL_INST_CONG_LIMIT_3r,
        };

        switch (type) {
            case bcmCosqControlEgressPoolHighCongestionLimitBytes:
                reg64 = wred_cng_lmt[pool];
                fld_limit = POOL_HI_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolLowCongestionLimitBytes:
                reg64 = wred_cng_lmt[pool];
                fld_limit = POOL_LOW_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
                /* coverity[dead_error_begin] */
            default:
                return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg64, REG_PORT_ANY, pool, &rval64));

        /* Check for min/max values */
        max_val = (1 << soc_reg_field_length(unit, reg64, fld_limit)) - 1;
        if ((arg < 0) || ((arg/granularity) > max_val)) {
            return BCM_E_PARAM;
        } else {
            soc_reg64_field32_set(unit, reg64, &rval64, fld_limit, (arg/granularity));
        }

        BCM_IF_ERROR_RETURN(soc_reg64_set(unit, reg64, REG_PORT_ANY, pool, rval64));

        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        base_mem2 = MMU_THDU_Q_TO_QGRP_MAPm;
        mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        startq = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
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
_bcm_fb6_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, pool, midx, pipe = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem, base_mem;
    int granularity = 1;

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));


    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
            type == bcmCosqControlMCEgressPool) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            /* regular unicast queue */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
            base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (type != bcmCosqControlEgressPool) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
            base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        } else  { /* local port */
            if (type == bcmCosqControlUCEgressPool) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
                mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 &local_port, &startq, NULL));
                base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
                mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_SPIDf);
        return BCM_E_NONE;
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
        *arg = (*arg) * granularity * _FB6_MMU_BYTES_PER_CELL;

        return BCM_E_NONE;
    }

    if (type == bcmCosqControlEgressPoolHighCongestionLimitBytes ||
            type == bcmCosqControlEgressPoolLowCongestionLimitBytes) {
        uint64 rval64;
        soc_reg_t reg64;
        soc_reg_t wred_cng_lmt[4] = {
            WRED_POOL_INST_CONG_LIMIT_0r,
            WRED_POOL_INST_CONG_LIMIT_1r,
            WRED_POOL_INST_CONG_LIMIT_2r,
            WRED_POOL_INST_CONG_LIMIT_3r,
        };
        soc_field_t fld_limit = INVALIDf;

        switch (type) {
            case bcmCosqControlEgressPoolHighCongestionLimitBytes:
                reg64 = wred_cng_lmt[pool];
                fld_limit = POOL_HI_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
            case bcmCosqControlEgressPoolLowCongestionLimitBytes:
                reg64 = wred_cng_lmt[pool];
                fld_limit = POOL_LOW_CONG_LIMITf;
                pool = -1;
                granularity = 1;
                break;
                /* coverity[dead_error_begin] */
            default:
                return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, reg64, REG_PORT_ANY, pool, &rval64));

        *arg = soc_reg64_field32_get(unit, reg64, rval64, fld_limit);
        *arg = (*arg) * granularity * _TD3_MMU_BYTES_PER_CELL;

        return BCM_E_NONE;
    }
    base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

    if (type == bcmCosqControlEgressPoolLimitEnable) {
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    midx, entry));

        *arg = soc_mem_field32_get(unit, mem, entry, SHARED_LIMIT_ENABLEf);
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));
        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));

        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    } else {
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0) == 0) {
            /* Need to have the UC_MC_Combine config enabled */
            return BCM_E_PARAM;
        }
        base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        startq = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
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
    *arg = (*arg) * granularity * _FB6_MMU_BYTES_PER_CELL;
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int pipe = 0, startq;
    uint32 max_val, rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int from_cos, to_cos, ci;
    int egr_db_shd_size, egr_qe_shd_size, cur_val, post_update;
    int limit;
    int disable_queuing = 0;
    int uc_qlimit_enable = 0, mc_qlimit_enable = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
            (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve
                     (unit, local_port, ci, _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDU_CONFIG_QUEUEm)[pipe];
        mem2 = SOC_MEM_UNIQUE_ACC(unit, MMU_THDU_Q_TO_QGRP_MAPm)[pipe];
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
            (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve
                     (unit, local_port, ci, _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
            if(IS_CPU_PORT(0, local_port)) {
               startq += _FB6_CPU_L1_NODE_BASE;
            }
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDM_DB_QUEUE_CONFIGm)[pipe];
        mem2 = SOC_MEM_UNIQUE_ACC(unit, MMU_THDM_MCQE_QUEUE_CONFIGm)[pipe];
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));

    switch (type) {
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
            fld_limit = Q_SHARED_LIMIT_CELLf;
            uc_qlimit_enable = 1;

            /* Set MMU_THDU_XPIPE_Q_TO_QGRP_MAP.DISABLE_QUEUING to 1 when both MIN
             * and SHARED LIMIT are 0. Set it to 0 when either MIN or SHARED LIMIT
             * is non-zero.
 */
            limit = soc_mem_field32_get(unit, mem, entry, Q_MIN_LIMIT_CELLf);
            if (arg == 0 && limit == 0) {
                disable_queuing = 1;
            } else if (arg != 0 || limit != 0) {
                disable_queuing = 0;
            }
            granularity = 1;
            break;
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
            fld_limit = Q_SHARED_LIMITf;
            mc_qlimit_enable = 1;
            granularity = 1;
            break;
        case bcmCosqControlEgressUCQueueMinLimitBytes:
            fld_limit = Q_MIN_LIMIT_CELLf;

            /* Set MMU_THDU_Q_TO_QGRP_MAP.DISABLE_QUEUING to 1 when both MIN
             *              * and SHARED LIMIT are 0. Set it to 0 when either MIN or
             *                           * SHARED LIMIT is non-zero
 */
            limit = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMIT_CELLf);
            if (arg == 0 && limit == 0) {
                disable_queuing = 1;
            } else if (arg != 0 || limit != 0) {
                disable_queuing = 0;
            }

            granularity = 1;
            break;
        case bcmCosqControlEgressMCQueueMinLimitBytes:
            fld_limit = Q_MIN_LIMITf;
            granularity = 1;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    /* Recalculate Shared Values if Min Changed */
    /* THDO - DB */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
    egr_db_shd_size = soc_reg_field_get(unit, MMU_THDM_DB_POOL_SHARED_LIMITr,
            rval, SHARED_LIMITf);

    /* THDO - QE */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (READ_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit, 0, &rval));
    egr_qe_shd_size = soc_reg_field_get(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
            rval, SHARED_LIMITf);
    cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);

    if ((arg / granularity) > cur_val) {
        /* New Min Val > Cur Min Val
         *          * So reduce the Shared values first and then
         *                   * Increase the Min Value for given resource
 */
        post_update = 0;
    } else if ((arg / granularity) < cur_val) {
        /* New Min Val < Cur Min Val
         *          * So reduce the Min values first and then
         *                   * Increase Shared values everywhere
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
        if (delta > egr_db_shd_size) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(
                soc_fb6_mmu_config_shared_buf_recalc(unit, 2 /* egr */, -1,
                    egr_db_shd_size - delta,
                    egr_qe_shd_size, 1));
    }


    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
    if (mem2 != INVALIDm) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
    }
    /* Check for min/max values */
    max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
    if ((arg < 0) || ((arg/granularity) > max_val)) {
        return BCM_E_PARAM;
    } else {
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
    }

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
            (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        soc_mem_field32_set(unit, mem2, entry2, DISABLE_QUEUINGf,
                disable_queuing);
    }

    if (mc_qlimit_enable) {
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, 1);
        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);
    }

    if (uc_qlimit_enable) {
        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_ENABLEf, 1);
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq,
                entry2));

    if (post_update &&
            ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
             (type == bcmCosqControlEgressMCQueueMinLimitBytes))) {
        int delta = 0;
        delta = (cur_val - (arg / granularity)) * granularity;

        SOC_IF_ERROR_RETURN(
                soc_fb6_mmu_config_shared_buf_recalc(unit, 2 /* egr */, -1,
                    egr_db_shd_size + delta,
                    egr_qe_shd_size, 0));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int pipe = 0, startq;
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
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve
                     (unit, local_port, ci, _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      NULL, &startq, NULL));
            }
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDU_CONFIG_QUEUEm)[pipe];
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
            (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve
                     (unit, local_port, ci, _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                      NULL, &startq, NULL));
            }
            if(IS_CPU_PORT(0, local_port)) {
               startq += _FB6_CPU_L1_NODE_BASE;
            }
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDM_DB_QUEUE_CONFIGm)[pipe];
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
    *arg = (*arg) * granularity * _FB6_MMU_BYTES_PER_CELL;
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_t type,
        int arg)
{
    bcm_port_t local_port;
    int startq, pipe = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }


        base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, local_port, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             NULL, &startq, NULL));
        }


        base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }

        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_t type,
        int *arg)
{
    bcm_port_t local_port;
    int startq, pipe = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }


        base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, local_port, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             NULL, &startq, NULL));
        }

        base_mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLEf);
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_egr_port_pool_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int pool, midx, pipe = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
        base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
    } else {
        base_mem = MMU_THDU_CONFIG_PORTm;
    }
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
                fld_limit = YELLOW_SHARED_LIMITf;
            } else {
                fld_limit = YELLOW_LIMITf;
            }
            granularity = 8;
            break;
        case bcmCosqControlEgressPortPoolRedLimitBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
                fld_limit = RED_SHARED_LIMITf;
            } else {
                fld_limit = RED_LIMITf;
            }
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;

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
_bcm_fb6_cosq_egr_port_pool_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    int granularity = 1;
    int pool, midx, pipe = 0;

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                     &local_port, &pool, NULL));

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
        base_mem = MMU_THDM_DB_PORTSP_CONFIGm;
    } else {
        base_mem = MMU_THDU_CONFIG_PORTm;
    }
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

    switch (type) {
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
                *arg = soc_mem_field32_get(unit, mem, entry,
                        YELLOW_SHARED_LIMITf);
            } else {
                *arg = soc_mem_field32_get(unit, mem, entry, YELLOW_LIMITf);
            }
            granularity = 8;
            break;
        case bcmCosqControlEgressPortPoolRedLimitBytes:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || IS_CPU_PORT(unit, local_port)) {
                *arg = soc_mem_field32_get(unit, mem, entry, RED_SHARED_LIMITf);
            } else {
                *arg = soc_mem_field32_get(unit, mem, entry, RED_LIMITf);
            }
            granularity = 8;
            break;
        default:
            return BCM_E_UNAVAIL;
    }

    *arg = (*arg) * granularity * _FB6_MMU_BYTES_PER_CELL;
    return BCM_E_NONE;
}

/*
 * This function is used to get the
 * Min and Max value of ingress PG or SP
 */
STATIC int
_bcm_fb6_cosq_ing_res_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int pipe = 0, port_pg;
    int pool, granularity = 1;

    if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, cosq, &port_pg, NULL));
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_sp_get(unit, gport, cosq, &pool, NULL));

    if ((type == bcmCosqControlIngressPortPGSharedLimitBytes) ||
            (type == bcmCosqControlIngressPortPGMinLimitBytes) ||
            (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) ||
            (type == bcmCosqControlIngressPortPGResetOffsetBytes) ||
            (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
            (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        base_mem = THDI_PORT_SP_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-SP */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
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

    *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
    *arg = (*arg) * granularity * _FB6_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_ing_res_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int pipe = 0, port_pg;
    int pool, granularity = 1;
    int  ing_shd_size, cur_val;

    if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
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

    arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, cosq, &port_pg, NULL));
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_sp_get(unit, gport, cosq, &pool, NULL));

    if ((type == bcmCosqControlIngressPortPGSharedLimitBytes) ||
            (type == bcmCosqControlIngressPortPGMinLimitBytes) ||
            (type == bcmCosqControlIngressPortPGHeadroomLimitBytes) ||
            (type == bcmCosqControlIngressPortPGResetOffsetBytes) ||
            (type == bcmCosqControlIngressPortPGResetFloorBytes)) {
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
    } else if ((type == bcmCosqControlIngressPortPoolMaxLimitBytes) ||
            (type == bcmCosqControlIngressPortPoolMinLimitBytes)) {
        base_mem = THDI_PORT_SP_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-SP */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, pool);
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

    /* Recalculate Shared Values if Min Changed */
    cur_val = soc_mem_field32_get(unit, mem, entry, fld_limit);

    if((arg / granularity) == cur_val) {
        return BCM_E_NONE;
    }

    if (type == bcmCosqControlIngressPortPGMinLimitBytes) {
        int delta = 0; /* In Cells */
        uint32 rval = 0;
        /* Read THDI shared size from Service pool 0 */
        SOC_IF_ERROR_RETURN
            (READ_THDI_BUFFER_CELL_LIMIT_SPr(unit, 0, &rval));
        ing_shd_size = soc_reg_field_get(unit, THDI_BUFFER_CELL_LIMIT_SPr,
                rval, LIMITf);
        if ((arg / granularity) > cur_val) {
            /* New Min Val > Cur Min Val
             * So reduce the Shared values first and then
             * Increase the Min Value for given resource */
            delta = ((arg / granularity) - cur_val) * granularity;
            if (delta > ing_shd_size) {
                return BCM_E_PARAM;
            }

            SOC_IF_ERROR_RETURN(
                    soc_fb6_mmu_config_shared_buf_recalc(unit, 1 /* ing */,
                        ing_shd_size - delta,
                        -1, -1, 1));

            /* the value may be changed by soc_fb6_mmu_config_res_limits_update
             * so read the register again in order to keep consistent  */
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        }

        /* Check for min/max values */
        max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
        if ((arg < 0) || ((arg/granularity) > max_val)) {
            return BCM_E_PARAM;
        }

        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));

        if ((arg / granularity) < cur_val) {
            /* New Min Val < Cur Min Val
             *             * So reduce the Min values first and then
             *                         * Increase Shared values everywhere */
            delta = (cur_val - (arg / granularity)) * granularity;
            if (delta > ing_shd_size) {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(
                    soc_fb6_mmu_config_shared_buf_recalc(unit, 1 /* ing */,
                        ing_shd_size + delta,
                        -1, -1,  0));
        }
    } else {
        /* Check for min/max values */
        max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
        if ((arg < 0) || ((arg/granularity) > max_val)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}

/*
 *This function is used to get ingress pool
 *limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_fb6_cosq_ing_res_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    uint32 rval = 0x0;
    int pool;
    int local_port;
    soc_reg_t reg = INVALIDr;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (type == bcmCosqControlIngressPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_ing_pool_get(unit, gport, cosq,
                                        bcmCosqControlIngressPool,
                                        &pool));
        reg = THDI_BUFFER_CELL_LIMIT_SPr;
    } else if (type == bcmCosqControlIngressHeadroomPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_ing_pool_get(unit, gport, cosq,
                                        bcmCosqControlIngressHeadroomPool,
                                        &pool));
        reg = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    } else {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_get(unit,reg,
                -1, 0, pool, &rval));
    *arg = soc_reg_field_get(unit, reg, rval, LIMITf);

    *arg *= _FB6_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_ing_res_enable_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type,
        int *arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    soc_field_t fld_en = INVALIDf;
    int port_pg;

    if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
        /* Error. Input pri > Max Pri_Grp */
        return BCM_E_PARAM;
    }

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, cosq, &port_pg, NULL));


    if (type == bcmCosqControlIngressPortPGGlobalHeadroomEnable) {
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
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
_bcm_fb6_cosq_ing_res_enable_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, bcm_cosq_control_t type,
        int arg)
{
    bcm_port_t local_port;
    int midx;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    soc_field_t fld_en = INVALIDf;
    int port_pg;

    if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
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
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_ingress_pg_get(unit, gport, cosq, &port_pg, NULL)); 

    if (type == bcmCosqControlIngressPortPGGlobalHeadroomEnable) {
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
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
/*
 *This function is used to set ingress pool
 *limit given Ingress [Port, Priority]
 */
STATIC int
_bcm_fb6_cosq_ing_res_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
        bcm_cosq_control_t type, int arg)
{
    uint32 rval = 0x0;
    int pool;
    int local_port;
    soc_reg_t reg = INVALIDr;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;

    if (type == bcmCosqControlIngressPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_ing_pool_get(unit, gport, cosq,
                                        bcmCosqControlIngressPool,
                                        &pool));
        reg = THDI_BUFFER_CELL_LIMIT_SPr;
    } else if (type == bcmCosqControlIngressHeadroomPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_ing_pool_get(unit, gport, cosq,
                                        bcmCosqControlIngressHeadroomPool,
                                        &pool));
        reg = THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    } else {
        return BCM_E_PARAM;
    }

    soc_reg_field_set(unit, reg, &rval, LIMITf, arg);
    SOC_IF_ERROR_RETURN(
            soc_trident3_xpe_reg32_set(unit, reg,
                -1, 0, pool, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_dynamic_thresh_enable_set(int unit,
        bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq, pipe = 0;
    int from_cos, to_cos, ci;
    int midx;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t base_mem = INVALIDm, base_mem2 = INVALIDm;
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
                BCM_GPORT_IS_SCHEDULER(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
            /* Error. Input pri > Max Pri_Grp */
            return BCM_E_PARAM;
        }

        /* get PG for port using Port+Cos */
        if (cosq < _FB6_MMU_NUM_PG) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry,
                PG_SHARED_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry, PG_SHARED_LIMITf,
                    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, local_port, ci,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 NULL, &startq, NULL));

            }
        }
        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry,
                Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry, Q_SHARED_ALPHA_CELLf,
                    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, local_port, ci,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 NULL, &startq, NULL));

            }
            if(IS_CPU_PORT(0, local_port)) {
                startq += _FB6_CPU_L1_NODE_BASE;
            }

        }

        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        base_mem2 = MMU_THDM_MCQE_QUEUE_CONFIGm;
        mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem, entry, Q_SHARED_ALPHAf,
                    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, startq, entry2));
        soc_mem_field32_set(unit, mem2, entry2, Q_LIMIT_DYNAMICf, arg ? 1 : 0);
        /* Set default alpha value*/
        if (arg) {
            soc_mem_field32_set(unit, mem2, entry2, Q_SHARED_ALPHAf,
                    SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, startq, entry2));

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_dynamic_thresh_enable_get(int unit,
        bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int startq, pipe = 0, from_cos, to_cos, ci, midx, port_pg;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
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

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_SCHEDULER(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
            /* Error. Input pri > Max Pri_Grp */
            return BCM_E_PARAM;
        }
        /* get PG for port using Port+Cos */
        if (cosq < _FB6_MMU_NUM_PG) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    midx, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, PG_SHARED_DYNAMICf);

    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
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
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, local_port, ci,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 NULL, &startq, NULL));
            }
        }
        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                             &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, local_port, ci,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                                 NULL, &startq, NULL));
            }
            if(IS_CPU_PORT(0, local_port)) {
                startq += _FB6_CPU_L1_NODE_BASE;
            }
        }
        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMICf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_alpha_set(int unit,
        bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int startq, midx, pipe = 0;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t base_mem = INVALIDm, base_mem2 = INVALIDm;
    soc_mem_t mem = INVALIDm, mem2 = INVALIDm;
    int alpha, dynamic_thresh_mode, port_pg;
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

    switch(arg) {
        case bcmCosqControlDropLimitAlpha_1_128:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128;
            break;
        case bcmCosqControlDropLimitAlpha_1_64:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_64;
            break;
        case bcmCosqControlDropLimitAlpha_1_32:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_32;
            break;
        case bcmCosqControlDropLimitAlpha_1_16:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_16;
            break;
        case bcmCosqControlDropLimitAlpha_1_8:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_8;
            break;
        case bcmCosqControlDropLimitAlpha_1_4:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_4;
            break;
        case bcmCosqControlDropLimitAlpha_1_2:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_2;
            break;
        case bcmCosqControlDropLimitAlpha_1:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1;
            break;
        case bcmCosqControlDropLimitAlpha_2:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_2;
            break;
        case bcmCosqControlDropLimitAlpha_4:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_4;
            break;
        case bcmCosqControlDropLimitAlpha_8:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_8;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlEgressUCSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));

        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry,
                Q_SHARED_ALPHA_CELLf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlEgressMCSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));

        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        base_mem2 = MMU_THDM_MCQE_QUEUE_CONFIGm;
        mem2 = SOC_MEM_UNIQUE_ACC(unit, base_mem2)[pipe];

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
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlIngressPortPGSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
            /* Error. Input pri > Max Pri_Grp */
            return BCM_E_PARAM;
        }
        /* get PG for port using Port+Cos */
        if (cosq < _FB6_MMU_NUM_PG) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, PG_SHARED_LIMITf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_alpha_get(int unit,
        bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    bcm_port_t local_port;
    int startq, midx, pipe = 0;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm, base_mem = INVALIDm;
    int alpha, dynamic_thresh_mode, port_pg;
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

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlEgressUCSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));

        base_mem = MMU_THDU_CONFIG_QUEUEm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);

    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlEgressMCSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));

        base_mem = MMU_THDM_DB_QUEUE_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHAf);

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                     bcmCosqControlIngressPortPGSharedDynamicEnable,
                                                     &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }


        if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_INT_PRI)) {
            /* Error. Input pri > Max Pri_Grp */
            return BCM_E_PARAM;
        }
        /* get PG for port using Port+Cos */
        if (cosq < _FB6_MMU_NUM_PG) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);

        base_mem = THDI_PORT_PG_CONFIGm;
        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
        /* get index for Port-PG */
        midx = _bcm_fb6_mmu_piped_mem_index(unit, local_port, base_mem, port_pg);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    midx, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, PG_SHARED_LIMITf);

    }

    switch(alpha) {
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128:
            *arg = bcmCosqControlDropLimitAlpha_1_128;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_64:
            *arg = bcmCosqControlDropLimitAlpha_1_64;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_32:
            *arg = bcmCosqControlDropLimitAlpha_1_32;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_16:
            *arg = bcmCosqControlDropLimitAlpha_1_16;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_8:
            *arg = bcmCosqControlDropLimitAlpha_1_8;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_4:
            *arg = bcmCosqControlDropLimitAlpha_1_4;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_2:
            *arg = bcmCosqControlDropLimitAlpha_1_2;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1:
            *arg = bcmCosqControlDropLimitAlpha_1;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_2:
            *arg = bcmCosqControlDropLimitAlpha_2;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_4:
            *arg = bcmCosqControlDropLimitAlpha_4;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_8:
            *arg = bcmCosqControlDropLimitAlpha_8;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_qgroup_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
        bcm_cosq_control_t type, int arg)
{
    int pipe = 0;
    int phy_port;
    int mmu_port;
    bcm_port_t local_port;
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    soc_field_t field = INVALIDf;
    int granularity = 1;
    int egr_db_shd_size;
    int egr_qe_shd_size;
    int cur_val;
    int max_val;
    int delta;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes ||
            type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        arg = (arg + _FB6_MMU_BYTES_PER_CELL - 1) / _FB6_MMU_BYTES_PER_CELL;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    base_mem = MMU_THDU_CONFIG_QGROUPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, mem, MEM_BLOCK_ALL, mmu_port, entry));

    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
        field = Q_MIN_LIMIT_CELLf;
        granularity = 1;

        rval = 0;
        SOC_IF_ERROR_RETURN(
                READ_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, 0, &rval));
        egr_db_shd_size = soc_reg_field_get(
                unit, MMU_THDM_DB_POOL_SHARED_LIMITr, rval, SHARED_LIMITf);

        rval = 0;
        SOC_IF_ERROR_RETURN(
                READ_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit, 0, &rval));
        egr_qe_shd_size = soc_reg_field_get(
                unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr, rval, SHARED_LIMITf);

        cur_val = soc_mem_field32_get(unit, mem, entry, field);
        delta = ((arg / granularity) - cur_val) * granularity;
        if (delta > egr_db_shd_size) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(soc_fb6_mmu_config_shared_buf_recalc(
                    unit, 2, -1, egr_db_shd_size - delta, egr_qe_shd_size, 1));
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        field = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable) {
        field = Q_LIMIT_DYNAMIC_CELLf;
    } else {
        return BCM_E_PARAM;
    }

    max_val = (1 << soc_mem_field_length(unit, mem, field)) - 1;
    if ((arg < 0) || ((arg / granularity) > max_val)) {
        return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, entry, field, (arg / granularity));
    BCM_IF_ERROR_RETURN(soc_mem_write(
                unit, mem, MEM_BLOCK_ALL, mmu_port, entry));
    return BCM_E_NONE;
}



STATIC int
_bcm_fb6_cosq_qgroup_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    int pipe = 0;
    int phy_port;
    int mmu_port;
    bcm_port_t local_port;
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    soc_field_t field = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    base_mem = MMU_THDU_CONFIG_QGROUPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ALL, mmu_port, entry));

    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes) {
        field = Q_MIN_LIMIT_CELLf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        field = Q_SHARED_LIMIT_CELLf;
    } else if (type == bcmCosqControlEgressUCQueueGroupSharedDynamicEnable) {
        field = Q_LIMIT_DYNAMIC_CELLf;
    } else {
        return BCM_E_PARAM;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, field);
    if (type == bcmCosqControlEgressUCQueueGroupMinLimitBytes ||
            type == bcmCosqControlEgressUCQueueGroupSharedLimitBytes) {
        *arg = (*arg) * _FB6_MMU_BYTES_PER_CELL;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_qgroup_alpha_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    int alpha;
    int pipe = 0;
    int phy_port;
    int mmu_port;
    bcm_port_t local_port;
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    switch(arg) {
        case bcmCosqControlDropLimitAlpha_1_128:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128;
            break;
        case bcmCosqControlDropLimitAlpha_1_64:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_64;
            break;
        case bcmCosqControlDropLimitAlpha_1_32:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_32;
            break;
        case bcmCosqControlDropLimitAlpha_1_16:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_16;
            break;
        case bcmCosqControlDropLimitAlpha_1_8:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_8;
            break;
        case bcmCosqControlDropLimitAlpha_1_4:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_4;
            break;
        case bcmCosqControlDropLimitAlpha_1_2:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_2;
            break;
        case bcmCosqControlDropLimitAlpha_1:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1;
            break;
        case bcmCosqControlDropLimitAlpha_2:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_2;
            break;
        case bcmCosqControlDropLimitAlpha_4:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_4;
            break;
        case bcmCosqControlDropLimitAlpha_8:
            alpha = SOC_FB6_COSQ_DROP_LIMIT_ALPHA_8;
            break;
        default:
            return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    base_mem = MMU_THDU_CONFIG_QGROUPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, mem, MEM_BLOCK_ALL, mmu_port, entry));

    soc_mem_field32_set(unit, mem, entry, Q_SHARED_ALPHA_CELLf, alpha);
    BCM_IF_ERROR_RETURN(soc_mem_write(
                unit, mem, MEM_BLOCK_ALL, mmu_port, entry));
    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_qgroup_alpha_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    int alpha;
    int pipe = 0;
    int phy_port;
    int mmu_port;
    bcm_port_t local_port;
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    base_mem = MMU_THDU_CONFIG_QGROUPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, mem, MEM_BLOCK_ALL, mmu_port, entry));
    alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);

    switch(alpha) {
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_128:
            *arg = bcmCosqControlDropLimitAlpha_1_128;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_64:
            *arg = bcmCosqControlDropLimitAlpha_1_64;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_32:
            *arg = bcmCosqControlDropLimitAlpha_1_32;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_16:
            *arg = bcmCosqControlDropLimitAlpha_1_16;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_8:
            *arg = bcmCosqControlDropLimitAlpha_1_8;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_4:
            *arg = bcmCosqControlDropLimitAlpha_1_4;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1_2:
            *arg = bcmCosqControlDropLimitAlpha_1_2;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_1:
            *arg = bcmCosqControlDropLimitAlpha_1;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_2:
            *arg = bcmCosqControlDropLimitAlpha_2;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_4:
            *arg = bcmCosqControlDropLimitAlpha_4;
            break;
        case SOC_FB6_COSQ_DROP_LIMIT_ALPHA_8:
            *arg = bcmCosqControlDropLimitAlpha_8;
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_qgroup_limit_enables_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        bcm_cosq_control_t type,
        int *arg)
{
    int startq, pipe = 0;
    bcm_port_t local_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    soc_field_t field = INVALIDf;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                    _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE, &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < 0) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_index_resolve(unit, local_port, cosq,
                    _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL, &startq, NULL));
    }

    base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

    if (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) {
        field = QGROUP_LIMIT_ENABLEf;
    } else if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
        field = USE_QGROUP_MINf;
    } else {
        return BCM_E_PARAM;
    }

    *arg = soc_mem_field32_get(unit, mem, entry, field);
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_qgroup_limit_enables_set(int unit, bcm_gport_t gport,
        bcm_cos_t cosq,
        bcm_cosq_control_t type,
        int arg)
{
    bcm_port_t local_port = 0;
    int startq = 0, pipe = 0, count = 1, lc = 0, to_cos = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry2[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_mem_t base_mem = INVALIDm;
    int enable = 0;
    int valid = 0;
    soc_field_t field = INVALIDf;
    soc_field_t field2 = INVALIDf;

    if (arg < 0) {
        return BCM_E_PARAM;
    }
    arg = arg ? 1 : 0;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                    _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE, &local_port, &startq, NULL));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    } else {
        if (cosq < -1) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_index_resolve(unit, local_port, cosq,
                    _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL, &startq, &count));
    }

    base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[pipe];
    if (type == bcmCosqControlEgressUCQueueGroupSharedLimitEnable) {
        field = QGROUP_LIMIT_ENABLEf;
        field2 = QGROUP_VALIDf;
    } else if (type == bcmCosqControlEgressUCQueueGroupMinEnable) {
        field = USE_QGROUP_MINf;
        field2 = QGROUP_VALIDf;
    } else {
        return BCM_E_PARAM;
    }

    to_cos = startq + count;
    for (lc = startq; lc < to_cos; lc++) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, lc, entry));
        enable = soc_mem_field32_get(unit, mem, entry, field);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, field, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, lc, entry));
        }
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, lc, entry2));
        valid = soc_mem_field32_get(unit, mem, entry2, field2);
        if (valid != 1) {
            soc_mem_field32_set(unit, mem, entry2, field2, 1);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, lc, entry2));
        }
    }

    return BCM_E_NONE;
}



/*
 *Function:
 *   bcm_fb6_cosq_control_set
 *Purpose:
 *   Set specified feature configuration
 *
 *Parameters:
 *   unit - (IN) Unit number.
 *   port - (IN) GPORT ID.
 *   cosq - (IN) COS queue.
 *   type - (IN) feature
 *   arg  - (IN) feature value
 *Returns:
 *   BCM_E_XXX
 *   Notes:
 */
int
bcm_fb6_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int arg)
{
    uint32 kbits_burst_min, kbits_burst_max;
    uint32 kbits_burst;

    kbits_burst_min = 0;
    kbits_burst_max = 0;

    COMPILER_REFERENCE(kbits_burst_min);
    COMPILER_REFERENCE(kbits_burst_max);

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            kbits_burst = arg & 0x7fffffff; /* Convert to uint32  */
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                        &kbits_burst_min,
                                                        &kbits_burst_max));
            return bcm_fb6_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                    kbits_burst_min,
                    kbits_burst);
        case bcmCosqControlBandwidthBurstMin:
            kbits_burst = arg & 0x7fffffff; /* Convert to uint32  */
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                        &kbits_burst_min,
                                                        &kbits_burst_max));
            return bcm_fb6_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                    kbits_burst,
                    kbits_burst_max);
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
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            return _bcm_fb6_cosq_egr_pool_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
            return _bcm_fb6_cosq_egr_queue_set(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGSharedLimitBytes:
        case bcmCosqControlIngressPortPGMinLimitBytes:
        case bcmCosqControlIngressPortPoolMaxLimitBytes:
        case bcmCosqControlIngressPortPoolMinLimitBytes:
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        case bcmCosqControlIngressPortPGResetFloorBytes:
        case bcmCosqControlIngressPortPGResetOffsetBytes:
            return _bcm_fb6_cosq_ing_res_set(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPool:
        case bcmCosqControlIngressHeadroomPool:
            return _bcm_fb6_cosq_ing_pool_set(unit, gport, cosq, type, arg);
        case bcmCosqControlDropLimitAlpha:
            return _bcm_fb6_cosq_alpha_set(unit, gport, cosq,
                    (bcm_cosq_control_drop_limit_alpha_value_t)arg);
        case bcmCosqControlIngressPortPGSharedDynamicEnable:
        case bcmCosqControlEgressUCSharedDynamicEnable:
        case bcmCosqControlEgressMCSharedDynamicEnable:
            return _bcm_fb6_cosq_dynamic_thresh_enable_set(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
        case bcmCosqControlEgressPortPoolRedLimitBytes:
            return _bcm_fb6_cosq_egr_port_pool_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueLimitEnable:
        case bcmCosqControlEgressMCQueueLimitEnable:
            return _bcm_fb6_cosq_egr_queue_limit_enable_set(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlIngressPoolLimitBytes:
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            return _bcm_fb6_cosq_ing_res_limit_set(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            return _bcm_fb6_cosq_ing_res_enable_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinEnable:
        case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
            return _bcm_fb6_cosq_qgroup_limit_enables_set(
                    unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
        case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
            return _bcm_fb6_cosq_qgroup_limit_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
            return _bcm_fb6_cosq_qgroup_alpha_set(unit, gport, cosq,
                    (bcm_cosq_control_drop_limit_alpha_value_t)arg);
        default:
            break;
    }
    return BCM_E_UNAVAIL;
}

/*
 *Function:
 *   bcm_fb6_cosq_control_get
 *Purpose:
 *   Get specified feature configuration
 *
 *Parameters:
 *   unit - (IN) Unit number.
 *   port - (IN) GPORT ID.
 *   cosq - (IN) COS queue.
 *   type - (IN) feature
 *   arg  - (OUT) feature value
 *Returns:
 *   BCM_E_XXX
 *  Notes:
 */
int
bcm_fb6_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        bcm_cosq_control_t type, int *arg)
{
    uint32 kburst_tmp, kbits_burst;

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            BCM_IF_ERROR_RETURN
                (bcm_fb6_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                        &kburst_tmp,
                                                        &kbits_burst));
            *arg = kbits_burst & 0x7fffffff; /* Convert to int  */
            return BCM_E_NONE;

        case bcmCosqControlBandwidthBurstMin:
            return bcm_fb6_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                    (uint32*)arg,
                    &kburst_tmp);
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
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            return _bcm_fb6_cosq_egr_pool_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
            return _bcm_fb6_cosq_egr_queue_get(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGSharedLimitBytes:
        case bcmCosqControlIngressPortPGMinLimitBytes:
        case bcmCosqControlIngressPortPoolMaxLimitBytes:
        case bcmCosqControlIngressPortPoolMinLimitBytes:
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        case bcmCosqControlIngressPortPGResetFloorBytes:
        case bcmCosqControlIngressPortPGResetOffsetBytes:
            return _bcm_fb6_cosq_ing_res_get(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPool:
        case bcmCosqControlIngressHeadroomPool:
            return _bcm_fb6_cosq_ing_pool_get(unit, gport, cosq, type, arg);
        case bcmCosqControlDropLimitAlpha:
            return _bcm_fb6_cosq_alpha_get(unit, gport, cosq,
                    (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
        case bcmCosqControlIngressPortPGSharedDynamicEnable:
        case bcmCosqControlEgressUCSharedDynamicEnable:
        case bcmCosqControlEgressMCSharedDynamicEnable:
            return _bcm_fb6_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
        case bcmCosqControlEgressPortPoolRedLimitBytes:
            return _bcm_fb6_cosq_egr_port_pool_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueLimitEnable:
        case bcmCosqControlEgressMCQueueLimitEnable:
            return _bcm_fb6_cosq_egr_queue_limit_enable_get(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlIngressPoolLimitBytes:
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            return _bcm_fb6_cosq_ing_res_limit_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinEnable:
        case bcmCosqControlEgressUCQueueGroupSharedLimitEnable:
            return _bcm_fb6_cosq_qgroup_limit_enables_get(
                    unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGGlobalHeadroomEnable:
            return _bcm_fb6_cosq_ing_res_enable_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressUCQueueGroupSharedLimitBytes:
        case bcmCosqControlEgressUCQueueGroupSharedDynamicEnable:
            return _bcm_fb6_cosq_qgroup_limit_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueGroupDropLimitAlpha:
            return _bcm_fb6_cosq_qgroup_alpha_get(unit, gport, cosq,
                    (bcm_cosq_control_drop_limit_alpha_value_t*)arg);

        default:
            break;
    }
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_fb6_cosq_bst_map_resource_to_gport_cos(int unit,
        bcm_bst_stat_id_t bid, int port, int index, bcm_gport_t *gport,
        bcm_cos_t *cosq)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_bst_resource_info_t *pres = NULL;
    int phy_port = 0;
    int mmu_port = 0;
    int stats_per_inst = 1;
    int stats_per_pipe = 1;

    if (bid >= bcmBstStatIdMaxCount) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                (BSL_META_U(unit,
                            "_bcm_th_cosq_bst_map_resource_to_gport_cos: unit=%d bid=0x%x\n"),
                 unit, bid));
        return BCM_E_NONE;
    }

    pres = _BCM_BST_RESOURCE(unit, bid);
    stats_per_inst = (pres->num_stat_pp) / (pres->num_instance);
    index %= stats_per_inst;
    if (pres->flags & _BCM_BST_CMN_RES_F_PIPED) {
        stats_per_pipe = stats_per_inst / NUM_PIPE(unit);
        index %= stats_per_pipe;
    }

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
            *cosq = index % _FB6_MMU_NUM_POOL;
            break;

        case bcmBstStatIdRQEQueue:
            *gport = -1;
            *cosq = index % _FB6_MMU_NUM_RQE_QUEUES;
            break;

        case bcmBstStatIdUCQueueGroup:
            mmu_port = index;
            phy_port = si->port_m2p_mapping[mmu_port];
            *gport = si->port_p2l_mapping[phy_port];
            *cosq =  -1;
            break;

        case bcmBstStatIdPortPool:
            mmu_port= index/_FB6_MMU_NUM_POOL;
            phy_port = si->port_m2p_mapping[mmu_port];
            *gport = si->port_p2l_mapping[phy_port];
            *cosq = index % _FB6_MMU_NUM_POOL;
            break;

        case bcmBstStatIdPriGroupHeadroom:
        case bcmBstStatIdPriGroupShared:
            mmu_port = index/_FB6_MMU_NUM_PG;
            phy_port = si->port_m2p_mapping[mmu_port];
            *gport = si->port_p2l_mapping[phy_port];
            *cosq = index % _FB6_MMU_NUM_PG;
            break;

        case bcmBstStatIdUcast:
        case bcmBstStatIdMcast:
            if ((bid == bcmBstStatIdMcast) &&
                    (index >= si->port_cosq_base[CMIC_PORT(unit)])) {
                *gport = CMIC_PORT(unit);
                *cosq = index - si->port_cosq_base[CMIC_PORT(unit)];
            } else {
                phy_port = si->port_m2p_mapping[mmu_port];
                *gport = si->port_p2l_mapping[phy_port];
                *cosq = index - si->port_uc_cosq_base[*gport];
            }
            break;
        case bcmBstStatIdEgrPortPoolSharedUcast:
        case bcmBstStatIdEgrPortPoolSharedMcast:
            mmu_port = index/_FB6_MMU_NUM_POOL;
            phy_port = si->port_m2p_mapping[mmu_port];
            *gport = si->port_p2l_mapping[phy_port];
            *cosq = index % _FB6_MMU_NUM_POOL;
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}



STATIC _bcm_bst_cb_ret_t
_bcm_fb6_bst_index_resolve(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_bst_stat_id_t bid, int *pipe, int *start_hw_index,
                          int *end_hw_index, int *rcb_data, void **cb_data, int *bcm_rv)
{
    int phy_port, mmu_port, local_port;
    int mmu_port_base = 0;
    int pool_idx = 0;
    uint32 rval = 0;
    _bcm_fb6_cosq_node_t *node = NULL;
    soc_info_t *si;

    *bcm_rv = _bcm_fb6_cosq_localport_resolve(unit, gport, &local_port);

    if (BCM_E_NONE != (*bcm_rv)) {
        goto error;
    }

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    *pipe = mmu_port / SOC_FB6_MMU_PORT_STRIDE;

    if (bid == bcmBstStatIdDevice) {
        *start_hw_index = *end_hw_index = 0;
        *bcm_rv = BCM_E_NONE;
        return _BCM_BST_RV_OK;
    }

    if ((bid == bcmBstStatIdEgrPool) || (bid == bcmBstStatIdEgrMCastPool)) {
        if(_bcm_fb6_cosq_egress_sp_get(unit, gport, cosq, start_hw_index,
                    end_hw_index)) {
            goto error;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdHeadroomPool) {
        cosq = cosq % 8;
        /* ingress headroom pool */
        if (_bcm_fb6_cosq_ingress_hdrm_pool_get(unit, gport, cosq,
                    start_hw_index, end_hw_index)) {
            goto error;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdIngPool) {
        cosq = cosq % 8;
        /* ingress pool */
        if (_bcm_fb6_cosq_ingress_sp_get(unit, gport, cosq,
                    start_hw_index, end_hw_index)) {
            goto error;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdRQEPool) {
        /* RQE pool */
        if (cosq == BCM_COS_INVALID) {
            *start_hw_index = 0;
            *end_hw_index = _FB6_MMU_NUM_POOL - 1;
        } else if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_POOL)) {
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
            *end_hw_index = _FB6_MMU_NUM_RQE_QUEUES - 1;
        } else if ((cosq < 0) || (cosq >= _FB6_MMU_NUM_RQE_QUEUES)) {
            return BCM_E_PARAM;
        } else {
            *start_hw_index = *end_hw_index = cosq;
        }
        return _BCM_BST_RV_OK;
    } else if (bid == bcmBstStatIdUCQueueGroup) {
        /* unicast queue group */
        mmu_port_base = SOC_FB6_MMU_PORT_STRIDE * (*pipe);
        *start_hw_index = *end_hw_index = mmu_port - mmu_port_base;
        return _BCM_BST_RV_OK;
    }


    if (bid == bcmBstStatIdPortPool) {
        cosq = cosq % 8;
        /* ingress pool */
        if (_bcm_fb6_cosq_ingress_sp_get(unit, gport, cosq,
                                        start_hw_index, end_hw_index)) {
            goto error;
        }
        *start_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 4) +
                          *start_hw_index;
        *end_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 4) +
                        *end_hw_index;
    } else if ((bid == bcmBstStatIdPriGroupShared) ||
               (bid == bcmBstStatIdPriGroupHeadroom)) {
        cosq = cosq % 8;
        /* ingress PG */
        if (_bcm_fb6_cosq_ingress_pg_get(unit, gport, cosq,
                                        start_hw_index, end_hw_index)) {
            goto error;
        }
        *start_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 8) +
                          *start_hw_index;
        *end_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 8) +
                        *end_hw_index;
    } else if (bid == bcmBstStatIdEgrPortPoolSharedUcast ||
               bid == bcmBstStatIdEgrPortPoolSharedMcast) {
        /* egress pool */
        if (_bcm_fb6_cosq_egress_sp_get(unit, gport, cosq,
                                       start_hw_index, end_hw_index)) {
            goto error;
        }
        *start_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 4) +
                          *start_hw_index;
        *end_hw_index = ((mmu_port & SOC_FB6_MMU_PORT_STRIDE) * 4) +
                        *end_hw_index;
    } else {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (bid != bcmBstStatIdUcast) {
                goto error;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_get(unit, gport, NULL,
                                       &local_port, NULL, &node));
            if (!node) {
                goto error;
            }
            *start_hw_index =  node->hw_index;
            *end_hw_index = *start_hw_index;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (bid != bcmBstStatIdMcast) {
                goto error;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_get(unit, gport, NULL,
                                       &local_port, NULL, &node));
            if (!node) {
                goto error;
            }
            if (IS_CPU_PORT(unit, local_port)) {
                *start_hw_index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;
            } else {
                *start_hw_index =  node->hw_index;
            }
            *end_hw_index = *start_hw_index;
        } else {
            if (cosq < 0) {
                if (bid == bcmBstStatIdUcast) {
                    if (IS_LB_PORT(unit, local_port)) {
                        goto error;
                    }
                    *start_hw_index = si->port_uc_cosq_base[local_port];
                    *end_hw_index = *start_hw_index +
                                    si->port_num_uc_cosq[local_port] - 1;
                } else {
                    if (IS_CPU_PORT(unit, local_port)) {
                        *start_hw_index = si->port_cosq_base[CMIC_PORT(unit)];
                        *end_hw_index = *start_hw_index +
                                         SOC_FB6_NUM_CPU_QUEUES - 1;
                    } else {
                        *start_hw_index =  si->port_cosq_base[local_port]; 
                        *end_hw_index = *start_hw_index +
                            si->port_num_cosq[local_port] - 1;
                    }
                }
            } else {
                if (bid == bcmBstStatIdUcast) {
                    if (IS_LB_PORT(unit, local_port)) {
                        goto error;
                    }
                    *start_hw_index = si->port_num_uc_cosq[local_port] + cosq;
                    *end_hw_index = *start_hw_index;
                } else {
                    if (IS_CPU_PORT(unit, local_port)) {
                        *start_hw_index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;
                    } else {
                        *start_hw_index = si->port_num_cosq[local_port] + cosq;
                    }
                    *end_hw_index = *start_hw_index;
                }
            }
        }
    }

    *bcm_rv = BCM_E_NONE;
    return _BCM_BST_RV_OK;

error:
    *bcm_rv = BCM_E_PARAM;
    return _BCM_BST_RV_ERROR;
}


/*
 * Function:
 *     bcm_fb6_cosq_field_classifier_get
 * Purpose:
 *     Get classifier type information.
 * Parameters:
 *     unit          - (IN) unit number
 *     classifier_id - (IN) Classifier ID
 *     classifier    - (OUT) Classifier info
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_field_classifier_get(int unit, int classifier_id,
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
 *     bcm_fb6_cosq_field_classifier_id_create
 * Purpose:
 *     Create a cosq classifier.
 * Parameters:
 *     unit          - (IN) unit number
 *     classifier    - (IN) Classifier attributes
 *     classifier_id - (OUT) Classifier ID
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_field_classifier_id_create(int unit, bcm_cosq_classifier_t
                                        *classifier, int *classifier_id)
{
    int rv;
    int ref_count = 0;
    int ent_per_set = _FB6_MMU_NUM_INT_PRI;
    int i;

    if (NULL == classifier || NULL == classifier_id) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < SOC_MEM_SIZE(unit, IFP_COS_MAPm); i += ent_per_set) {
        rv = soc_profile_mem_ref_count_get(
                unit, _bcm_fb6_ifp_cos_map_profile[unit], i, &ref_count);
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
 *      bcm_fb6_cosq_field_classifier_id_destroy
 * Purpose:
 *      Free resource associated with this field classifier id.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      classifier_id - (IN) Classifier ID
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_fb6_cosq_field_classifier_id_destroy(int unit, int classifier_id)
{
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_field_classifier_map_set
 * Purpose:
 *     Set internal priority to ingress field processor CoS queue override
 *     mapping.
 * Parameters:
 *     unit           - (IN) unit number
 *     classifier_id  - (OUT) Classifier ID
 *     count          - (IN) Number of elements in priority_array and cosq_array
 *     priority_array - (IN) Array of internal priorities
 *     cosq_array     - (IN) Array of COS queues
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *      Supported count value is [0-16] and 48
 *      When count = [0-16]
 *          Array Index [15-0] : Maps Prio[15-0] to UC, MC and RQE cos[15-0]
 *      When count = 48
 *      Prio Array Index Map
 *          Array Index [15-0] : Maps Prio[15-0] to UC cos[15-0]
 *          Array Index [31-16] : Maps Prio[15-0] to MC cos[15-0]
 *          Array Index [47-32] : Maps Prio[15-0] to RQE cos[15-0]
 */
int
bcm_fb6_cosq_field_classifier_map_set(int unit, int classifier_id, int count,
                        bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array)
{
    int rv;
    int i;
    int max_entries = _FB6_MMU_NUM_INT_PRI;
    uint32 index, new_index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_FB6_MMU_NUM_INT_PRI];
    soc_field_t field = -1;

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

    if (count != (max_entries * 3)) {
        for (i = 0; i < count; i++) {
            if (priority_array[i] < max_entries) {
                /* When count != max_entries * 3
                   We program the same mapping for all three
                   fields UC/MC/RQE queues.
                   Hence cos_array[i] cannot have value more than
                   equal to 32 */
                if ((cosq_array[i] < 0) ||
                    (cosq_array[i] >= SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT)) {
                    return BCM_E_PARAM;
                }
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_MC_COSf, cosq_array[i]);
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_UC_COSf, cosq_array[i]);
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_HG_COSf, cosq_array[i]);
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    IFP_RQE_Q_NUMf,
                                    (cosq_array[i] > _FB6_MMU_NUM_RQE_QUEUES) ? _FB6_MMU_NUM_RQE_QUEUES : cosq_array[i]);
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            if (priority_array[i] < max_entries) {
                if (i < max_entries) {
                    /* Number of UCAST queue per port = 32 */
                    if ((cosq_array[i] < 0) ||
                        (cosq_array[i] >= SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT)) {
                        return BCM_E_PARAM;
                    }
                    field = IFP_UC_COSf;

                } else if (i < (max_entries * 2)) {
                    /* Number of MCAST queue per port = 32 */
                    if ((cosq_array[i] < 0) ||
                        (cosq_array[i] >= SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT)) {
                        return BCM_E_PARAM;
                    }
                    field = IFP_MC_COSf;
                } else {
                    if ((cosq_array[i] < 0) ||
                            (cosq_array[i] >= SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT)) {
                        return BCM_E_PARAM;
                    }
                    /* Number of RQE queue per unit = 11 */
                    if (cosq_array[i] > _FB6_MMU_NUM_RQE_QUEUES) {
                        cosq_array[i] = _FB6_MMU_NUM_RQE_QUEUES;
                    }
                    field = IFP_RQE_Q_NUMf;
                }
                soc_mem_field32_set(unit, IFP_COS_MAPm, &ent_buf[priority_array[i]],
                                    field, cosq_array[i]);
            }
        }
    }

    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    rv = soc_profile_mem_delete(unit, _bcm_fb6_ifp_cos_map_profile[unit],
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
        (soc_profile_mem_add(unit, _bcm_fb6_ifp_cos_map_profile[unit],
                             entries, max_entries, &new_index));

    /* If we get (index == new_index / max_entries) means we have
     * a successful mapping operation.
     * But if they are not equal which means there already exists
     * a similar mapping associated with other classifier_id hence
     * we return BCM_E_EXISTS.
     */

    if (index != (new_index / max_entries)) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_fb6_ifp_cos_map_profile[unit],
                                    new_index));
        return BCM_E_EXISTS;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb6_cosq_field_classifier_map_get
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
bcm_fb6_cosq_field_classifier_map_get(int unit, int classifier_id, int array_max,
    bcm_cos_t *priority_array, bcm_cos_queue_t *cosq_array, int *array_count)
{
    int rv;
    int i;
    int ent_per_set = _FB6_MMU_NUM_INT_PRI;
    uint32 index;
    void *entries[1];
    ifp_cos_map_entry_t ent_buf[_FB6_MMU_NUM_INT_PRI];
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
    rv = soc_profile_mem_get(unit, _bcm_fb6_ifp_cos_map_profile[unit],
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
        if (priority_array[i % 16] >= _FB6_MMU_NUM_INT_PRI) {
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
 *      bcm_fb6_cosq_field_classifier_map_clear
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
bcm_fb6_cosq_field_classifier_map_clear(int unit, int classifier_id)
{
    int ent_per_set = _FB6_MMU_NUM_INT_PRI;
    uint32 index;

    /* Get profile table entry set base index. */
    index = _BCM_COSQ_CLASSIFIER_FIELD_GET(classifier_id);

    /* Delete the profile entries set. */
    SOC_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit,
                                _bcm_fb6_ifp_cos_map_profile[unit],
                                index * ent_per_set));
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0
STATIC int
_bcm_fb6_cosq_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr = NULL;
    int rv, alloc_size = 0, max_size = (4096 * 75);
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_coe_port_info_t *coe_port_info = NULL;

    /*
     * Sync _bcm_fb6_cosq_coe_port_info structure
     */
    coe_port_info = _bcm_fb6_cosq_coe_port_info[unit];
    if (coe_port_info == NULL) {
        return BCM_E_INIT;
    }

    alloc_size += sizeof(_bcm_fb6_cosq_coe_port_info_t) * (_BCM_FB6_NUM_SUB_PORTS);

    /*
     * Sync _bcm_fb6_cosq_cpu_port_info structure
     */
    cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }
    alloc_size += sizeof(_bcm_fb6_cosq_cpu_port_info_t);

    /*
     * Sync the ref_count of IFP_COS_MAPm/COS_MAPm
     */
    alloc_size += (SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _FB6_MMU_NUM_INT_PRI) *
        sizeof(uint16);

    /*
     * Sync _bcm_fb6_cosq_cpu_cosq_config_t structure for all CPU queues
     */
    alloc_size += (SOC_FB6_NUM_CPU_QUEUES *
            sizeof(_bcm_fb6_cosq_cpu_cosq_config_t));

    /*
     *
     *
     * Sync num_cos value
     */
    alloc_size += sizeof(int);

    /*
     *Sync _soc_td3_mmu_traffic_ctrl_t
     */
    BCM_IF_ERROR_RETURN(soc_td3_mmu_traffic_ctrl_size(unit, &alloc_size));

    /*
     * Sync _bcm_pfc_deadlock_control_t structure
 */
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        alloc_size += sizeof(_bcm_fb6_pfc_deadlock_control_t);
    }

    /*
     * Sync _bcm_fb6_cosq_cpu_cosq_config_t structure for all CPU queues
     */
    alloc_size += (_BCM_FB6_NUM_WRED_RESOLUTION_TABLE *
            sizeof(uint32));

    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        uint32 scache_size = 0;
        BCM_IF_ERROR_RETURN(
                bcmi_xgs5_ecn_scache_size_get(unit, &scache_size));
        alloc_size += scache_size;
    }

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

int
bcm_fb6_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    soc_profile_mem_t *profile_mem;
    soc_mem_t wred_config_mem = INVALIDm;
    int index, profile_index, resolution_index;
    int stable_size, rv, set_index, num;
    int i, j;
    mmu_wred_config_entry_t qentry;
    uint32 entry[SOC_MAX_MEM_WORDS];
    bcm_port_t port;
    _bcm_bst_device_handlers_t bst_callbks;
    _bcm_fb6_cosq_cpu_cosq_config_t *cpu_cosq_config_p;
    int index_min, index_max;
    int alloc_size = 0, pipe = -1;
    uint16 recovered_ver = 0;
    uint8 *scache_ptr = NULL;
    int *int_scache_ptr = NULL;
    uint16 *u16_scache_ptr = NULL;
    uint32 *u32_scache_ptr = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_coe_port_info_t *coe_port_info = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    soc_mem_t fc_mem = SRC_SUBPORT_FC_CONFIG_SC0m;



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

    /* Restore _bcm_fb6_cosq_coe_port_info structure */
    coe_port_info = _bcm_fb6_cosq_coe_port_info[unit];
    if (coe_port_info == NULL) {
        return BCM_E_INIT;
    }
    alloc_size = sizeof(_bcm_fb6_cosq_coe_port_info_t) * (_BCM_FB6_NUM_SUB_PORTS);
    sal_memcpy( _bcm_fb6_cosq_coe_port_info[unit], scache_ptr, alloc_size);
    scache_ptr += alloc_size;

    /* Restore _bcm_fb6_cosq_cpu_port_info structure */
    cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }
    alloc_size = sizeof(_bcm_fb6_cosq_cpu_port_info_t);
    sal_memcpy(cpu_port_info, scache_ptr, alloc_size);
    scache_ptr += alloc_size;

    /* Update IFP_COS_MAP memory profile reference counter */
    profile_mem = _bcm_fb6_ifp_cos_map_profile[unit];
    num = SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _FB6_MMU_NUM_INT_PRI;
    u16_scache_ptr = (uint16 *)scache_ptr;
    for (i = 0; i < num; i++) {
        for (j = 0; j < *u16_scache_ptr; j++) {
            SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, profile_mem,
                        i * _FB6_MMU_NUM_INT_PRI,
                        _FB6_MMU_NUM_INT_PRI));
        }
        u16_scache_ptr++;
    }
    scache_ptr = (uint8*)u16_scache_ptr;

    /* Recover CPU queue config information from scache */
    cpu_cosq_config_p = _bcm_fb6_cosq_cpu_cosq_config[unit][0];
    if (cpu_cosq_config_p == NULL) {
        return BCM_E_INIT;
    }
    alloc_size = (SOC_FB6_NUM_CPU_QUEUES *
            sizeof(_bcm_fb6_cosq_cpu_cosq_config_t));
    sal_memcpy(cpu_cosq_config_p, scache_ptr, alloc_size);
    scache_ptr += alloc_size;

    /* Retriving num_cos value */
    int_scache_ptr = (int *) scache_ptr;
    _BCM_FB6_NUM_COS(unit) = *int_scache_ptr++;
    scache_ptr = (uint8*) int_scache_ptr;

    /* Update PRIO2COS_PROFILE register profile reference counter */
    PBMP_PORT_ITER(unit, port) {
        if (!IS_COE_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
                    (unit, port, -1, &index, 0));
            profile_index = 16 * index;
            BCM_IF_ERROR_RETURN(soc_profile_reg_reference(unit, _bcm_fb6_prio2cos_profile[unit],
                        profile_index, 16));

        }
    }

    /* Update CH_PRIO2COS_PROFILE register profile reference counter */
    for(i = 0; i < FB6_NUM_SUBPORTS; i++) {
        sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, fc_mem, MEM_BLOCK_ALL, i, &entry));
        set_index = soc_mem_field32_get(unit, fc_mem, &entry, PROFILE_INDEXf);
        profile_index = 2 * set_index;
        BCM_IF_ERROR_RETURN(soc_profile_reg_reference(unit, _bcm_fb6_chprio2cos_profile[unit],
                    profile_index, 2));

    }


    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_fb6_cos_map_profile[unit];
    for (i = 1; i < _BCM_FB6_NUM_SUB_PORTS; i++) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, i, &entry));
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
    /* Update MMU_WRED_DROP_CURVE_PROFILE memory profile reference counter */
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        wred_config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_WRED_CONFIGm)[pipe];
        if (wred_config_mem == INVALIDm) {
            return BCM_E_PARAM;
        }
        index_min = soc_mem_index_min(unit, wred_config_mem);
        index_max = soc_mem_index_max(unit, wred_config_mem);
        for (index = index_min; index < index_max; index++) {
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, wred_config_mem,
                              MEM_BLOCK_ALL, index, &qentry));
            profile_index = soc_mem_field32_get(unit, wred_config_mem,
                    &qentry, PROFILE_INDEXf);
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, _bcm_fb6_wred_profile[unit],
                                           profile_index, 1));

            if (soc_mem_field32_get(unit, wred_config_mem, &qentry,
                        ECN_MARKING_ENf)) {
                resolution_index = soc_mem_field32_get(
                        unit, wred_config_mem, &qentry,
                        WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf);
            } else {
                resolution_index = soc_mem_field32_get(
                        unit, wred_config_mem, &qentry,
                        WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf);
            }
            _bcm_fb6_wred_resolution_tbl[resolution_index] = 1;
        }
    }

    /* Create gport tree */
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = FALSE;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)
                || IS_FAE_PORT(unit, port)) {
            continue;
        }
        if (IS_STK_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_st_gport_tree_create(unit, port));
        } else if(IS_UPLINK_PORT(unit, port)){
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_hg_gport_tree_create(unit, port));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_gport_tree_create(unit, port));
        }
    }
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = TRUE;

    /* BST reinit */
    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &_bcm_fb6_bst_index_resolve;
    bst_callbks.reverse_resolve_index = NULL;
    BCM_IF_ERROR_RETURN(_bcm_bst_attach(unit, &bst_callbks));

    /* Recover _soc_td3_mmu_traffic_ctrl_t */
    BCM_IF_ERROR_RETURN(soc_td3_mmu_traffic_ctrl_wb_restore(unit, &scache_ptr));

    /* Recover PFC Deadlock recovery information from scache */
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        BCM_IF_ERROR_RETURN(_bcm_pfc_deadlock_init(unit));
        pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
        if (pfc_deadlock_control == NULL) {
            return BCM_E_INIT;
        }
        alloc_size = sizeof(_bcm_fb6_pfc_deadlock_control_t);
        sal_memcpy(pfc_deadlock_control, scache_ptr, alloc_size);
        scache_ptr += alloc_size;
        /* Re-set the Hardware Registers/Memory info */
        BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_hw_res_init(unit));
        BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_reinit(unit));
        BCM_IF_ERROR_RETURN(_bcm_fb6_pfc_deadlock_recovery_reset(unit));
        pfc_deadlock_control->cosq_inv_mapping_get = _bcm_fb6_cosq_inv_mapping_get;
    }


    scache_ptr += sizeof(uint16);
    /* Retriving _bcm_fb6_wred_resolution_tbl value */
    u32_scache_ptr = (uint32 *) scache_ptr;
    for (i = 0; i < _BCM_FB6_NUM_WRED_RESOLUTION_TABLE; i++) {
        _bcm_fb6_wred_resolution_tbl[i] = *u32_scache_ptr++;
    }
    return BCM_E_NONE;
}

int
bcm_fb6_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int *int_scache_ptr;
    _bcm_fb6_cosq_cpu_cosq_config_t **cpu_cosq_config_p;
    int ii;
    int ref_count;
    uint16 *u16_scache_ptr;
    uint32 *u32_scache_ptr;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_coe_port_info_t *coe_port_info = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    int size = 0;
    int rv = BCM_E_NONE;

    cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    if (cpu_port_info == NULL) {
        return BCM_E_INIT;
    }

    coe_port_info = _bcm_fb6_cosq_coe_port_info[unit];
    if (coe_port_info == NULL) {
        return BCM_E_INIT;
    }


    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    /* Cache _bcm_fb6_cosq_coe_port_info_t structure */
    size = sizeof(_bcm_fb6_cosq_coe_port_info_t) * (_BCM_FB6_NUM_SUB_PORTS);
    sal_memcpy(scache_ptr, _bcm_fb6_cosq_coe_port_info[unit], size);
    scache_ptr += size/sizeof(uint8);

    /* Cache _bcm_fb6_cosq_cpu_port_info_t structure */
    size = sizeof(_bcm_fb6_cosq_cpu_port_info_t);
    sal_memcpy(scache_ptr, cpu_port_info, size);
    scache_ptr += size/sizeof(uint8);

    /* Cache _bcm_fb6_ifp_cos_map_profile reference count */
    u16_scache_ptr = (uint16 *)scache_ptr;
    for (ii = 0;
            ii < (SOC_MEM_SIZE(unit, IFP_COS_MAPm) / _FB6_MMU_NUM_INT_PRI);
            ii++) {
        rv = soc_profile_mem_ref_count_get(unit,
                _bcm_fb6_ifp_cos_map_profile[unit],
                ii * _FB6_MMU_NUM_INT_PRI,
                &ref_count);
        if (!(rv == SOC_E_NOT_FOUND || rv == SOC_E_NONE)) {
            return rv;
        }
        *u16_scache_ptr++ = (uint16)(ref_count & 0xffff);
    }
    scache_ptr = (uint8*) u16_scache_ptr;

    /* Sync CPU Queue info */
    cpu_cosq_config_p = _bcm_fb6_cosq_cpu_cosq_config[unit];
    size = (SOC_FB6_NUM_CPU_QUEUES * sizeof(_bcm_fb6_cosq_cpu_cosq_config_t));
    sal_memcpy(scache_ptr, *cpu_cosq_config_p, size);
    scache_ptr += (size / sizeof(uint8));

    /* Sync num_cos value */
    int_scache_ptr = (int*) scache_ptr;
    *int_scache_ptr++ = _BCM_FB6_NUM_COS(unit);
    scache_ptr = (uint8 *)int_scache_ptr;

    /* Sync _soc_td3_mmu_traffic_ctrl_t */
    BCM_IF_ERROR_RETURN(soc_td3_mmu_traffic_ctrl_wb_sync(unit, &scache_ptr));

    /* Sync PFC Deadlock Control info */
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
        if (pfc_deadlock_control == NULL) {
            return BCM_E_INIT;
        }
        size = sizeof(_bcm_fb6_pfc_deadlock_control_t);
        sal_memcpy(scache_ptr, pfc_deadlock_control, size);
        scache_ptr += (size / sizeof(uint8));
    }

    u32_scache_ptr = (uint32*) u16_scache_ptr;

    for (ii = 0; ii < _BCM_FB6_NUM_WRED_RESOLUTION_TABLE; ii++) {
        *u32_scache_ptr++ = _bcm_fb6_wred_resolution_tbl[ii];
    }

    scache_ptr = (uint8 *)u32_scache_ptr;

    return BCM_E_NONE;
}
#endif

/*
*  Function:
*      bcm_fb6_cosq_init
*  Purpose:
*      Initialize (clear) all COS schedule/mapping state.
*  Parameters:
*      unit - (IN) unit number
*  Returns:
*      BCM_E_XXX
 */
int
bcm_fb6_cosq_init(int unit)
{
    int i, alloc_size = 0, numq = 0;
    int rv = BCM_E_NONE;
    int pipe;
    bcm_port_t port;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    int entry_words[9];
    soc_mem_t wred_config_mem = INVALIDm;
    uint64 rval64s[16], *prval64s[1];
    uint64 chvals[2], *pchvals[1];
    _bcm_bst_device_handlers_t bst_callbks;
    _bcm_fb6_cosq_cpu_cosq_config_t *cpu_cosq_config_p = NULL;
    int subport_count = 0;

    static soc_mem_t wred_mems[9] = {
        MMU_WRED_DROP_CURVE_PROFILE_0m, MMU_WRED_DROP_CURVE_PROFILE_1m,
        MMU_WRED_DROP_CURVE_PROFILE_2m, MMU_WRED_DROP_CURVE_PROFILE_3m,
        MMU_WRED_DROP_CURVE_PROFILE_4m, MMU_WRED_DROP_CURVE_PROFILE_5m,
        MMU_WRED_DROP_CURVE_PROFILE_6m, MMU_WRED_DROP_CURVE_PROFILE_7m,
        MMU_WRED_DROP_CURVE_PROFILE_8m
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
    void *entries[9];
    uint32 profile_index;
    int ii, wred_prof_count;
    soc_info_t *si =  &SOC_INFO(unit);

    /* Getting Number of Class of Service input from config.bcm */
    numq = soc_property_get(unit, spn_BCM_NUM_COS, _BCM_FB6_COS_DEFAULT);
    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    /* Create Port Information structure */
    alloc_size = sizeof(_bcm_fb6_cosq_port_info_t) * (_BCM_FB6_NUM_PORTS);
    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
        _bcm_fb6_cosq_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_cosq_port_info");
        if (_bcm_fb6_cosq_port_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_cosq_port_info[unit], 0, alloc_size);

    /* Create ST Port Information structure */
    alloc_size = sizeof(_bcm_fb6_cosq_st_port_info_t) * (_BCM_FB6_NUM_PORTS);
    if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
        _bcm_fb6_cosq_st_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_cosq_st_port_info");
        if (_bcm_fb6_cosq_st_port_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_cosq_st_port_info[unit], 0, alloc_size);

    /* Create HG Port Information structure */
    alloc_size = sizeof(_bcm_fb6_cosq_hg_port_info_t) * (_BCM_FB6_NUM_PORTS);
    if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
        _bcm_fb6_cosq_hg_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_cosq_hg_port_info");
        if (_bcm_fb6_cosq_hg_port_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_cosq_hg_port_info[unit], 0, alloc_size);

    /* Create coe Port Information structure */
    alloc_size = sizeof(_bcm_fb6_cosq_coe_port_info_t) * (_BCM_FB6_NUM_SUB_PORTS);
    if (_bcm_fb6_cosq_coe_port_info[unit] == NULL) {
        _bcm_fb6_cosq_coe_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_cosq_coe_port_info");
        if (_bcm_fb6_cosq_coe_port_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_cosq_coe_port_info[unit], 0, alloc_size);

    /* Create CPU Port Information structure */
    alloc_size = sizeof(_bcm_fb6_cosq_cpu_port_info_t);
    if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
        _bcm_fb6_cosq_cpu_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_cosq_cpu_port_info");
        if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_cosq_cpu_port_info[unit], 0, alloc_size);

    /* Create MMU Port Information structure */
    alloc_size = sizeof(_bcm_fb6_mmu_info_t);
    if (_bcm_fb6_mmu_info[unit] == NULL) {
        _bcm_fb6_mmu_info[unit] =
            sal_alloc(alloc_size, "_bcm_fb6_mmu_info");
        if (_bcm_fb6_mmu_info[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_fb6_mmu_info[unit], 0, alloc_size);
    _BCM_FB6_MMU_INFO(unit)->_bcm_fb6_port_info =
        _bcm_fb6_cosq_port_info[unit];
    _BCM_FB6_MMU_INFO(unit)->_bcm_fb6_cpu_port_info =
        _bcm_fb6_cosq_cpu_port_info[unit];

    if (_bcm_fb6_cosq_cpu_cosq_config[unit][0] == NULL) {
        cpu_cosq_config_p = sal_alloc(SOC_FB6_NUM_CPU_QUEUES *
                sizeof(_bcm_fb6_cosq_cpu_cosq_config_t), "CPU Cosq Config");
        if (cpu_cosq_config_p == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        if (NUM_CPU_COSQ(unit) > SOC_FB6_NUM_CPU_QUEUES) {
            /* Invalid CPU Queues */
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_INIT;
        }
        for (i = 0; i < NUM_CPU_COSQ(unit); i++) {
            _bcm_fb6_cosq_cpu_cosq_config[unit][i] = cpu_cosq_config_p + i;
        }
    }


    /* Create profile for PORT_COS_MAP table */
    if (_bcm_fb6_cos_map_profile[unit] == NULL) {
        _bcm_fb6_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                "COS_MAP Profile Mem");
        if (_bcm_fb6_cos_map_profile[unit] == NULL) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_fb6_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);

    rv = soc_profile_mem_create(unit, &mem, entry_words, 1,
            _bcm_fb6_cos_map_profile[unit]);
    if (rv != SOC_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for IFP_COS_MAP table */
    if (_bcm_fb6_ifp_cos_map_profile[unit] == NULL) {
        _bcm_fb6_ifp_cos_map_profile[unit]
            = sal_alloc(sizeof(soc_profile_mem_t), "IFP_COS_MAP Profile Mem");
        if (_bcm_fb6_ifp_cos_map_profile[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_fb6_ifp_cos_map_profile[unit]);
    }
    mem = IFP_COS_MAPm;
    entry_words[0] = sizeof(ifp_cos_map_entry_t) / sizeof(uint32);

    rv = soc_profile_mem_create(unit, &mem, entry_words, 1,
            _bcm_fb6_ifp_cos_map_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_fb6_wred_profile[unit] == NULL) {
        _bcm_fb6_wred_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "WRED Profile Mem");
        if (_bcm_fb6_wred_profile[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_fb6_wred_profile[unit]);
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
    entry_words[6] =
        sizeof(mmu_wred_drop_curve_profile_6_entry_t) / sizeof(uint32);
    entry_words[7] =
        sizeof(mmu_wred_drop_curve_profile_7_entry_t) / sizeof(uint32);
    entry_words[8] =
        sizeof(mmu_wred_drop_curve_profile_8_entry_t) / sizeof(uint32);

    rv = soc_profile_mem_create(unit, wred_mems, entry_words, 9,
            _bcm_fb6_wred_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for PRIO2COS_PROFILE register */
    if (_bcm_fb6_prio2cos_profile[unit] == NULL) {
        _bcm_fb6_prio2cos_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "PRIO2COS_PROFILE Profile Reg");
        if (_bcm_fb6_prio2cos_profile[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_fb6_prio2cos_profile[unit]);
    }
    reg = PRIO2COS_PROFILE_64r;
    rv = soc_profile_reg_create(unit, &reg, 1, _bcm_fb6_prio2cos_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }

    /* Create profile for CH_PRIO2COS_PROFILE register */
    if (_bcm_fb6_chprio2cos_profile[unit] == NULL) {
        _bcm_fb6_chprio2cos_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "CH_PRIO2COS_PROFILE Profile Reg");
        if (_bcm_fb6_chprio2cos_profile[unit] == NULL) {
            _bcm_fb6_cosq_cleanup(unit);
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_fb6_chprio2cos_profile[unit]);
    }
    reg = CH_PRIO2COS_PROFILE_64r;
    rv = soc_profile_reg_create(unit, &reg, 1, _bcm_fb6_chprio2cos_profile[unit]);
    if (rv != SOC_E_NONE) {
        _bcm_fb6_cosq_cleanup(unit);
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
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }
#endif /* INCLUDE_PSTATS */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = bcm_fb6_cosq_reinit(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
        }
        /*
         * COVERITY
         *
         * This checker will not be True.
         * cpu_cosq_config_p pointer is assigned to
         * _bcm_fb6_cosq_cpu_cosq_config global data
         * structure.
         */
        /* coverity[resource_leak] */
        return rv;
    } else {
        rv = _bcm_fb6_cosq_wb_alloc(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        /* Clear the WRED memories pointing to WRED profiles */
        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_WRED_CONFIGm)[pipe];
        rv = soc_mem_clear(unit, mem, COPYNO_ALL, 1);
        if (rv != SOC_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
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

    for (ii = 0; ii < FB6_WRED_PROFILE_NUM; ii++) {
        soc_mem_field32_set(unit, wred_mems[ii], entries[ii],
                MIN_THDf, 0xffff);
        soc_mem_field32_set(unit, wred_mems[ii], entries[ii],
                MAX_THDf, 0xffff);
    }
    profile_index = 0xffffffff;
    wred_config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_WRED_CONFIGm)[0];
    wred_prof_count = soc_mem_index_count(unit, wred_config_mem);

    /* In FB6 Profile table duplicate across all pipes.
     *         * Ref count needs to be updated too */
    wred_prof_count *= NUM_PIPE(unit);
    while(wred_prof_count) {
        if (profile_index == 0xffffffff) {
            rv = soc_profile_mem_add(unit, _bcm_fb6_wred_profile[unit],
                    entries, 1, &profile_index);
        } else {
            rv = soc_profile_mem_reference(unit, _bcm_fb6_wred_profile[unit],
                    profile_index, 0);
        }
        if (rv != SOC_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
        wred_prof_count -= 1;
    }

    /* Add default entries for PRIO2COS_PROFILE register profile */
    sal_memset(rval64s, 0, sizeof(rval64s));
    prval64s[0] = rval64s;
    profile_index = 0xffffffff;
    PBMP_PORT_ITER(unit, port) {
        if(IS_COE_PORT(unit, port)) {
           subport_count += si->port_num_subport[port];
            continue;
        }
        if (profile_index == 0xffffffff) {
            rv = soc_profile_reg_add(unit, _bcm_fb6_prio2cos_profile[unit],
                    prval64s, 16, &profile_index);
        } else {
            rv = soc_profile_reg_reference(unit, _bcm_fb6_prio2cos_profile[unit],
                    profile_index, 0);
        }
        if (rv != SOC_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
        rv =  _bcm_fb6_cosq_pri2cos_profile_setget(unit, port, profile_index/16, NULL, 1);
        if (rv != SOC_E_NONE) {
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }

    /* Add default entries for CH_PRIO2COS_PROFILE register profile */
    COMPILER_64_SET(chvals[0], 0x0, 0xFC);
    COMPILER_64_SET(chvals[1], 0x0, 0x03);
    pchvals[0] = chvals;
    profile_index = 0xffffffff;
    for ( i = 0; i < 384; i++) {
        if (profile_index == 0xffffffff) {
            rv = soc_profile_reg_add(unit, _bcm_fb6_chprio2cos_profile[unit],
                    pchvals, 2, &profile_index);
        } else {
            rv = soc_profile_reg_reference(unit, _bcm_fb6_chprio2cos_profile[unit],
                    profile_index, 0);
        }
        if (rv != SOC_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }
    /* Create default gport tree */
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = FALSE;
    PBMP_ALL_ITER(unit, port) {
        if (IS_LB_PORT(unit, port) ||
                IS_FAE_PORT(unit, port) ||
                IS_COE_PORT(unit, port)) {
            continue;
        }

        if(IS_CPU_PORT(unit, port)){
            rv = _bcm_fb6_cosq_cpu_tree_create(unit, port);
            if (rv != BCM_E_NONE) {
                _bcm_fb6_cosq_cleanup(unit);
                return rv;
            }
        } else if (IS_STK_PORT(unit, port)){
            rv = _bcm_fb6_cosq_st_gport_tree_create(unit, port);
            if (rv != BCM_E_NONE) {
                _bcm_fb6_cosq_cleanup(unit);
                return rv;
            }
        } else if (IS_UPLINK_PORT(unit, port)){
            rv = _bcm_fb6_cosq_hg_gport_tree_create(unit, port);
            if (rv != BCM_E_NONE) {
                _bcm_fb6_cosq_cleanup(unit);
                return rv;
            }
        } else {
            rv = _bcm_fb6_cosq_gport_tree_create(unit, port);
            if (rv != BCM_E_NONE) {
                _bcm_fb6_cosq_cleanup(unit);
                return rv;
            }
        }
    }
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = TRUE;

    /* Add default entries for PORT_COS_MAP memory profile */
    rv = bcm_fb6_cosq_config_set(unit, numq);
    if (rv != BCM_E_NONE) {
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }

    rv = _bcm_fb6_cosq_gport_default_config(unit);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }
    if (soc_feature(unit, soc_feature_helix5_ecn)) {
        rv = _bcm_td3_ecn_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL;
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }


    /* BST INIT */
    sal_memset(&bst_callbks, 0, sizeof(_bcm_bst_device_handlers_t));
    bst_callbks.resolve_index = &_bcm_fb6_bst_index_resolve;
    bst_callbks.reverse_resolve_index = &_bcm_fb6_cosq_bst_map_resource_to_gport_cos;
    rv = _bcm_bst_attach(unit, &bst_callbks);
    if (rv != BCM_E_NONE) {
        if (cpu_cosq_config_p != NULL) {
            sal_free(cpu_cosq_config_p);
            cpu_cosq_config_p = NULL;
        }
        _bcm_fb6_cosq_cleanup(unit);
        return rv;
    }
    if (soc_feature(unit, soc_feature_pfc_deadlock)) {
        rv = _bcm_pfc_deadlock_init(unit);
        if (rv != BCM_E_NONE) {
            if (cpu_cosq_config_p != NULL) {
                sal_free(cpu_cosq_config_p);
                cpu_cosq_config_p = NULL; 
            }
            _bcm_fb6_cosq_cleanup(unit);
            return rv;
        }
    }

    sal_memset(fb6_time_domain_info, 0x0, sizeof(fb6_time_domain_info));
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        mmu_wred_config_entry_t entry;
        uint32 time_id = 0;
        uint32 wred_mem_count = 0;

        wred_config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_WRED_CONFIGm)[pipe];
        wred_mem_count = soc_mem_index_count(unit, wred_config_mem);
        if (INVALIDm != wred_config_mem) {
            for (i = 0; i< wred_mem_count; i++){
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, wred_config_mem, MEM_BLOCK_ALL, i,
                                  &entry));
                time_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                        TIME_DOMAIN_SELf);
                if(time_id < _BCM_FB6_NUM_TIME_DOMAIN) {
                    fb6_time_domain_info[unit][time_id].ref_count ++;
                }
            }
        }
    }

    for (i = 0; i < _BCM_FB6_NUM_WRED_RESOLUTION_TABLE; i++) {
        if (i == 0) {
            _bcm_fb6_wred_resolution_tbl[i] = 1;
        } else {
            _bcm_fb6_wred_resolution_tbl[i] = 0;
        }
    }

    return BCM_E_NONE;
}

int
bcm_fb6_rx_queue_channel_set_test(int unit, bcm_cos_queue_t queue_id,
                                          bcm_rx_chan_t chan_id)
{
#if defined(BCM_CMICX_SUPPORT)
    if (soc_feature(unit, soc_feature_cmicx)) {
        int pci_cmc = SOC_PCI_CMC(unit);
        uint32 reg_val;
        uint32 bit;
        uint32 max_rx_channels = 0;
        int index;
        _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
        bcm_gport_t cpu_l2_gport = -1;
        bcm_gport_t parent_gport = -1;

        soc_dma_max_rx_channels_get(unit, &max_rx_channels);

        if ((chan_id <= 0) || (chan_id >= max_rx_channels)) {
            return BCM_E_PARAM;
        }

        if (_bcm_fb6_cosq_cpu_port_info[unit] == NULL) {
            return BCM_E_INIT;
        }

        cpu_port_info = _bcm_fb6_cosq_cpu_port_info[unit];

        if ((chan_id > 0) && (chan_id < 8 ))  {
            /*index = chan_id + 2;*/
            index = chan_id;
            cpu_l2_gport = cpu_port_info->sched[index].gport;
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
                    if (parent_gport != cpu_l2_gport) {
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
                if (parent_gport != cpu_l2_gport) {
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
 *     _bcm_fb6_cosq_bucket_get
 * Purpose:
 *     Get COS queue bandwidth control bucket setting
 * Parameters:
 *     unit          - (IN) unit number
 *     gport         - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (OUT) kbps or packet/second
 *     max_quantum   - (OUT) kbps or packet/second
 *     burst_quantum - (OUT) kbps or packet/second
 *     flags         - (IN) BCM_COSQ_BW_XXX
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_fb6_cosq_bucket_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                        uint32 *min_quantum, uint32 *max_quantum,
                        uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                        uint32 *flags)
{
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    uint32 rval;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;
    int pipe = 0, index = 0;

    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    if (BCM_GPORT_IS_SET(gport) &&
        ((BCM_GPORT_IS_SCHEDULER(gport)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))) { 

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (node->level == SOC_FB6_NODE_LVL_L0) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_MEMm)[pipe];
                index = node->hw_index;
            } else if(node->level == SOC_FB6_NODE_LVL_L1) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L1_MEMm)[pipe];
                index = node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                   BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if ((cosq < 0) || (cosq >= si->port_num_cosq[local_port])) {
            return BCM_E_PARAM;
        }
        index =   si->port_cosq_base[local_port] + cosq;
        config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L1_MEMm)[pipe];
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

    if (min_quantum == NULL || max_quantum == NULL ||
        kbits_burst_max == NULL || kbits_burst_min == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    meter_flags = 0;
    *flags = 0;
    BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }
    if (soc_mem_field32_get(unit, config_mem, &entry, SHAPER_CONTROLf)) {
        meter_flags |= _BCM_TD_METER_FLAG_PACKET_MODE;
        *flags |= BCM_COSQ_BW_PACKET_MODE;
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
/*
 * Function:
 *     _bcm_fb6_cosq_bucket_set
 * Purpose:
 *     Configure COS queue bandwidth control bucket
 * Parameters:
 *     unit          - (IN) unit number
 *     gport         - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (IN) kbps or packet/second
 *     max_quantum   - (IN) kbps or packet/second
 *     burst_quantum - (IN) kbps or packet/second
 *     flags         - (IN) BCM_COSQ_BW_XXX
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_fb6_cosq_bucket_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        uint32 min_quantum, uint32 max_quantum,
                        uint32 kbits_burst_min, uint32 kbits_burst_max,
                        uint32 flags)
{
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port;
    uint32 rval, meter_flags;
    uint32 bucketsize_max, bucketsize_min;
    uint32 granularity_max, granularity_min;
    uint32 refresh_rate_max, refresh_rate_min;
    int refresh_bitsize = 0, bucket_bitsize = 0;
    int pipe = 0, index = 0;
    soc_mem_t config_mem = INVALIDm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si;

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
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    if (BCM_GPORT_IS_SET(gport) &&
        ((BCM_GPORT_IS_SCHEDULER(gport)) ||
          BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
          BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))) { 

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
    }

    if (node) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (node->level == SOC_FB6_NODE_LVL_L0) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L0_MEMm)[pipe];
                index = node->hw_index;
            } else if(node->level == SOC_FB6_NODE_LVL_L1) {
                config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L1_MEMm)[pipe];
                index = node->hw_index;
            } else {
                return BCM_E_PARAM;
            }
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            return BCM_E_PARAM;
        }
    } else {
        if ((cosq < 0) || (cosq >= si->port_num_cosq[local_port])) {
            return BCM_E_PARAM;
        }
        config_mem = SOC_MEM_UNIQUE_ACC(unit, MMU_MTRO_L1_MEMm)[pipe];
        index = si->port_cosq_base[local_port] + cosq;
    }

    /*
     * COVERITY
     *
     * This checker will not be True.
     * It is kept intentionally as a defensive check for future development.
     */
    /* coverity[dead_error_line] */
    if (config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MMU_SEDCFG_MISCCONFIGr(unit, &rval));

    if (soc_reg_field_get(unit, MMU_SEDCFG_MISCCONFIGr, rval, ITU_MODE_SELf)) {
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

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, config_mem, MEM_BLOCK_ALL, index, &entry));

    return BCM_E_NONE;
}
/*
 * Function:
 *     bcm_fb6_cosq_port_burst_set
 * Purpose:
 *     Set the per-Cos burst setting
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) queue group GPORT identifier
 *     cosq     - (IN) CoS to configure
 *     burst    - (IN) Burst rate for the system in packets
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
        (_bcm_fb6_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst,
                                  &cur_burst, &flags));

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_fb6_cosq_bucket_set(unit, port, cosq, min, max, burst, burst,
            flags | BCM_COSQ_BW_PACKET_MODE);
}

/*
 * Function:
 *     bcm_fb6_cosq_port_burst_set
 * Purpose:
 *     Get the per-Cos burst setting
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) queue group GPORT identifier
 *     cosq     - (IN) CoS queue number
 *     burst    - (OUT) Burst rate for the system in packets
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_get(unit, port, cosq, &min, &max,
                                  &cur_burst_min, &cur_burst, &flags));

    *burst = cur_burst;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_port_pps_set
 * Purpose:
 *     Set the RX per-Cos rate limiting value
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) queue group GPORT identifier
 *     cosq     - (IN) CoS to configure
 *     pps      - (IN) Rate in packets/second
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
             BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))) { 
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_get(unit, port, cosq, &min, &max, &burst_min,
                                  &burst, &flags));

    min = pps;
    burst_min = (min > 0) ?
        _bcm_td_default_burst_size(unit, local_port, min) : 0;
    burst = burst_min;

    return _bcm_fb6_cosq_bucket_set(unit, port, cosq, min, pps, burst_min,
            burst, flags | BCM_COSQ_BW_PACKET_MODE);
}

/*
 * Function:
 *     bcm_fb6_cosq_port_pps_get
 * Purpose:
 *     Get the RX per-Cos rate limiting value
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) queue group GPORT identifier
 *     cosq     - (IN) COS queue number
 *     pps      - (OUT) Rate in packets/second
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
             BCM_GPORT_IS_MCAST_QUEUE_GROUP(port))) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                  &flags));

    *pps = max;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_sched_weight_max_get
 * Purpose:
 *     Retrieve maximum weights for given COS policy
 * Parameters:
 *     unit       - (IN) unit number
 *     mode       - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight_max - (OUT) Maximum weight for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
        case BCM_COSQ_STRICT:
            *weight_max = BCM_COSQ_WEIGHT_STRICT;
            break;
        case BCM_COSQ_ROUND_ROBIN:
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            /* In fb6, weights can be [1, 127] */
            *weight_max =
                (1 << soc_mem_field_length(unit, Q_SCHED_L0_WEIGHT_MEMm,
                                           WEIGHTf)) - 1;
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cosq_sched_get
 * Purpose:
 *     Get scheduling mode setting
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) GPORT identifier
 *     cosq     - (IN) COS queue number
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_xxx)
 *     weights  - (OUT) Weights for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int *mode, int *weight)
{
    soc_fb6_sched_mode_e sched_mode;
    bcm_port_t local_port;
    _bcm_fb6_cosq_node_t *node = NULL;
    int lvl = SOC_FB6_NODE_LVL_L1, mc = 0;


    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, NULL, &local_port, NULL,
                                    &node));
        lvl = node->level;
    }else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        mc = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ? 1 : 0;
    } else if (IS_CPU_PORT(unit, local_port)) {
        mc = 1;
        lvl = SOC_FB6_NODE_LVL_L2;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_sched_mode_setget(unit, local_port, lvl, cosq,
                                         &sched_mode, weight, mc, 0));

    switch(sched_mode) {
        case SOC_FB6_SCHED_MODE_STRICT:
            *mode = BCM_COSQ_STRICT;
            break;

        case SOC_FB6_SCHED_MODE_WRR:
            *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
            break;

        case SOC_FB6_SCHED_MODE_WERR:
            *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
            break;

        default:
            return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_sched_get
 * Purpose:
 *     Get COS queue schedular setting
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) GPORT identifier
 *     cosq     - (IN) COS queue to get, -1 for any queue
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_xxx)
 *     weights  - (OUT) Weights for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
        int *mode, int *weight)
{
    int local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);

    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_hw_index_get(unit, gport, &cosq));

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_sched_get(unit, gport, cosq, mode, weight));
            return BCM_E_NONE;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

            if ((cosq < 0) || (cosq >= si->port_num_cosq[local_port])) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_sched_get(unit, local_port, cosq, mode, weight));
            return BCM_E_NONE;
        }
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *     bcm_fb6_cosq_port_sched_get
 * Purpose:
 *     Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit     - (IN) unit number
 *     pbm      - (IN) port bitmap
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_xxx)
 *     weights  - (OUT) Weights for each COS queue.
 *     delay    - This parameter is not used
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_sched_get(int unit, bcm_pbmp_t pbm, int *mode, int *weights,
        int *delay)
{
    bcm_port_t port;
    int num_weights, i;

    BCM_PBMP_ITER(pbm, port) {
        if (IS_CPU_PORT(unit, port) && SOC_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
            continue;
        }
        if (IS_UPLINK_PORT(unit, port)) {
           num_weights = SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT;
        } else if (IS_STK_PORT(unit, port)) {
           num_weights = SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT;
        } else {
            num_weights = _BCM_FB6_NUM_COS(unit);
        }
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_get(unit, port, i, mode, &weights[i]));
        }
    }

    return BCM_E_NONE;
}



/*     bcm_fb6_cosq_port_sched_set
* Purpose:
*       Set up class-of-service policy and corresponding weights and delay
* Parameters:
*      unit     - (IN) unit number
*      pbm      - (IN) port bitmap
*      mode     - (IN) Scheduling mode (BCM_COSQ_xxx)
*      weight   - (IN) Weight for each COS queue.
*      delay    - This parameter is not used
* Returns:
*      BCM_E_XXX
 */
int
bcm_fb6_cosq_port_sched_set(int unit, bcm_pbmp_t pbm, int mode, const int
        *weights, int delay)
{
    bcm_port_t port; 
    int num_weights, i = 0;

    BCM_PBMP_ITER(pbm, port) {
        if (IS_UPLINK_PORT(unit, port)) {
           num_weights = SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT;
        } else if (IS_STK_PORT(unit, port)) {
           num_weights = SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT;
        } else {
            num_weights = _BCM_FB6_NUM_COS(unit);
        }
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_sched_set(unit, port, i, mode, weights[i]));

        }
    }
    return BCM_E_NONE;
}

/*
* bcm_fb6_cosq_gport_bandwidth_get
* Purpose:
*        Get COS queue bandwidth control configuration
* Parameters:
*        unit   - (IN) unit number
*        gport  - (IN) GPORT identifier
*        cosq   - (IN) COS queue number
*
*        kbits_sec_min - (OUT) minimum bandwidth, kbits/sec
*        kbits_sec_max - (OUT) maximum bandwidth, kbits/sec
*        flags  - (OUT) BCM_COSQ_BW_XXX
*Returns:
*       BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
        uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 kbits_sec_burst;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_get(unit, gport, cosq, kbits_sec_min,
                                  kbits_sec_max, &kbits_sec_burst,
                                  &kbits_sec_burst, flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_fb6_cosq_gport_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *       unit   - (IN) unit number
 *       gport  - (IN) GPORT identifier
 *       cosq   - (IN) COS queue number
 *       kbits_sec_min - (IN) minimum bandwidth, kbits/sec
 *       kbits_sec_max - (IN) maximum bandwidth, kbits/sec
 *       flags  - (IN) BCM_COSQ_BW_XXX
 * Returns:
 * *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq, uint32 kbits_sec_min,
        uint32 kbits_sec_max, uint32 flags)
{
    int i, start_cos=0, end_cos=0, local_port;
    int fport;
    _bcm_fb6_cosq_node_t *node = NULL;
    uint32 burst_min, burst_max;

    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_localport_resolve(unit, gport,
                &local_port));

    if (BCM_GPORT_IS_SET(gport)) {
        if ((BCM_GPORT_IS_SCHEDULER(gport)) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

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

    if(local_port > _BCM_FB6_NUM_PORTS) {
        BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, local_port, &fport));
        local_port = fport;
    }
    burst_min = (kbits_sec_min > 0) ?
        _bcm_td_default_burst_size(unit, local_port, kbits_sec_min) : 0;

    burst_max = (kbits_sec_max > 0) ?
        _bcm_td_default_burst_size(unit, local_port, kbits_sec_max) : 0;

    for (i = start_cos; i <= end_cos; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_bucket_set(unit, gport, i, kbits_sec_min,
                                      kbits_sec_max, burst_min, burst_max,
                                      flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_fb6_cosq_port_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *       unit   - (IN) unit number
 *       port   - (IN) port number
 *       cosq   - (IN) COS queue number
 *       min_quantum   - (IN) minimum bandwidth, kbits/sec
 *       max_quantum   - (IN) maximum bandwidth, kbits/sec
 *       burst_quantum - (IN) burst size, kbits/sec
 *       flags  - (IN) BCM_COSQ_BW_XXX
 *       Returns:
 *BCM_E_XXX
 */
int
bcm_fb6_cosq_port_bandwidth_set(int unit, bcm_port_t port,
        bcm_cos_queue_t cosq, uint32 min_quantum,
        uint32 max_quantum, uint32 burst_quantum,
        uint32 flags)
{
    uint32 burst_min, burst_max;

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if ((BCM_GPORT_IS_SCHEDULER(port)) ||
                BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) {
            return BCM_E_UNAVAIL;
        }
    }

    burst_min = (min_quantum > 0) ?
        _bcm_td_default_burst_size(unit, port, min_quantum) : 0;

    burst_max = (max_quantum > 0) ?
        _bcm_td_default_burst_size(unit, port, max_quantum) : 0;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_bucket_set(unit, port, cosq, min_quantum,
                                  max_quantum, burst_min, burst_max,
                                  flags));

    return BCM_E_NONE;
}

/*
 *Function:
 *     bcm_fb6_cosq_port_bandwidth_get
 *Purpose:
 *     Get COS queue bandwidth control configuration
 *Parameters:
 *     unit   - (IN) unit number
 *     port   - (IN) port number
 *     cosq   - (IN) COS queue number
 *     min_quantum   - (OUT) minimum bandwidth, kbits/sec
 *     max_quantum   - (OUT) maximum bandwidth, kbits/sec
 *     burst_quantum - (OUT) burst size, kbits/sec
 *     flags  - (OUT) BCM_COSQ_BW_XXX
 * * Returns:
 * *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_bandwidth_get(int unit, bcm_port_t port,
        bcm_cos_queue_t cosq, uint32 *min_quantum,
        uint32 *max_quantum, uint32 *burst_quantum,
        uint32 *flags)
{
    uint32 kbits_burst_min;

    if (cosq < -1) {
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
        (_bcm_fb6_cosq_bucket_get(unit, port, cosq, min_quantum,
                                  max_quantum, &kbits_burst_min,
                                  burst_quantum, flags));

    return BCM_E_NONE;
}

int
_bcm_fb6_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *class)
{

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
            /*
             *COVERITY
             *
             *This default is unreachable but needed for some compiler.
             */
            /* coverity[dead_error_begin] */
        default:
            return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_port_pfc_op
 * Purpose:
 *     Set PFC Prio 2 Cos Map and set the profile index for the Port
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) GPORT identifier
 *     sctype   - (IN) Input Class
 *     op       - (IN) Operation to be performed (ADD|SET|CLEAR)
 *     gport    - (IN) Gport/Cos array
 *     gport_ct - (IN) Count of values in the array
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_pfc_op(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                         bcm_gport_t *gport, int gport_count)
{
    bcm_port_t local_port;
    int class = -1, index, hw_cosq;
    uint32 profile_index = 0, old_profile_index = 0;
    uint32 fval, cos_bmp;
    int l0_num;
    uint64 rval64[16], *rval64s[1];
    _bcm_fb6_cosq_node_t *node;
    soc_field_t field = COS_BMPf;
    int rv = SOC_E_NONE;

    if (gport_count < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, local_port, &l0_num, NULL));

    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pfc_class_resolve(sctype, &class));

    cos_bmp = 0;
    for (index = 0; index < gport_count; index++) {
        if (BCM_GPORT_IS_SET(gport[index]) &&
                (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport[index]) ||
                 BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport[index]))) {
            return BCM_E_PARAM;
        }
        hw_cosq = -1;
        if (BCM_GPORT_IS_SET(gport[index]) &&
                (BCM_GPORT_IS_SCHEDULER(gport[index]))) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_node_get(unit, gport[index], NULL,
                                        NULL, NULL, &node));
            hw_cosq = node->hw_index - (l0_num * 8);
        } else {
            /* Find hardware index for given local_port, cosq */
            hw_cosq = gport[index] % 32;
        }
        cos_bmp |= (1 << hw_cosq);
    }

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
        (unit, local_port, -1, &index, 0));

    if (op == _BCM_COSQ_OP_CLEAR || cos_bmp != 0) {
        old_profile_index = 16 * index;

        BCM_IF_ERROR_RETURN
            (soc_profile_reg_get(unit, _bcm_fb6_prio2cos_profile[unit],
                                 old_profile_index, 16, rval64s));
        if (op == _BCM_COSQ_OP_SET) {
            soc_reg64_field32_set(unit, PRIO2COS_PROFILE_64r, &rval64[class],
                                  field, cos_bmp);
        } else if (op == _BCM_COSQ_OP_CLEAR || cos_bmp != 0) {
            fval = soc_reg64_field32_get(unit, PRIO2COS_PROFILE_64r, rval64[class],
                                         field);
            if (op == _BCM_COSQ_OP_ADD) {
                fval |= cos_bmp;
            } else { /* _BCM_COSQ_OP_CLEAR */
                fval = 0;
            }
            soc_reg64_field32_set(unit, PRIO2COS_PROFILE_64r, &rval64[class],
                                  field, fval);
        }
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, _bcm_fb6_prio2cos_profile[unit],
                                    old_profile_index));
        rv = soc_profile_reg_add(unit, _bcm_fb6_prio2cos_profile[unit], rval64s,
                                 16, &profile_index);
        if (rv != BCM_E_NONE) {
            SOC_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, _bcm_fb6_prio2cos_profile[unit],
                                           old_profile_index, 16));
            return rv;
        }
    }
    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
        (unit, local_port, profile_index/16, NULL, 1));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_port_pfc_get
 * Purpose:
 *     Get PFC Prio2Cos Map for a given Traffic Class
 * Parameters:
 *     unit     - (IN) unit number
 *     port     - (IN) GPORT identifier
 *     sctype   - (IN) Input Traffic Class
 *     gport    - (OUT) Gport/Cos array
 *     gport_ct - (IN) Input Count of gport required
 *     actual_gport_ct - (OUT) Actual Count of gport/cos in the array
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_port_pfc_get(int unit, bcm_port_t port,
                          bcm_switch_control_t sctype,
                          bcm_gport_t *gport, int gport_count,
                          int *actual_gport_count)
{
    bcm_port_t local_port;
    int class = -1, hw_cosq, count = 0;
    uint32 profile_index;
    int index;
    uint32 cos_bmp;
    int num_cos, start_cos;
    uint64 rval64[16], *rval64s[1];
    soc_field_t field = COS_BMPf;
    _bcm_fb6_cosq_node_t *port_node = NULL;

    if (gport == NULL || gport_count <= 0 || actual_gport_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pfc_class_resolve(sctype, &class));

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
        (unit, local_port, -1, &index, 0));

    profile_index = 16 * index;

    if (IS_STK_PORT(unit,  local_port)) {
        port_node = _bcm_fb6_cosq_st_port_info[unit][local_port].sched;
        start_cos = _FB6_ST_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_ST_PORT;
    } else if (IS_UPLINK_PORT(unit, local_port)) {
        port_node = _bcm_fb6_cosq_hg_port_info[unit][local_port].sched;
        start_cos = _FB6_HG_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_HG_PORT;
    } else {
        port_node = _bcm_fb6_cosq_port_info[unit][local_port].sched;
        start_cos = _FB6_DL_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_PORT;
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_prio2cos_profile[unit],
                             profile_index, 16, rval64s));

    cos_bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILE_64r, rval64[class],
                                    field);
    for (hw_cosq = 0; hw_cosq < (num_cos - start_cos); hw_cosq++) {
        if (!(cos_bmp & (1 << hw_cosq ))) {
            continue;
        }
        gport[count++] = port_node[hw_cosq + start_cos].gport;

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
 *     _bcm_fb6_cosq_port_fc_get
 * Purpose:
 *     Get the schedular gport values associated with
 *     the given PFC/SAFC class.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     fc_class         - (IN) PFC/SAFC internal priority class
 *     gport_list       - (OUT) schedular gport list
 *     gport_count      - (OUT) gport count
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_port_fc_get(int unit, bcm_gport_t local_port,
                         int fc_class, bcm_gport_t *gport_list,
                         int *gport_count)
{
    int hw_cosq, count = 0;
    uint32 profile_index;
    int index;
    int num_cos, start_cos;
    uint32 cos_bmp;
    uint64 rval64[16], *rval64s[1];

    if ((gport_list == NULL) || (gport_count == NULL)) {
        return BCM_E_PARAM;
    }

    rval64s[0] = rval64;

    if (IS_STK_PORT(unit,  local_port)) {
        start_cos = _FB6_ST_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_ST_PORT;
    } else if (IS_UPLINK_PORT(unit, local_port)) {
        start_cos = _FB6_HG_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_HG_PORT;
    } else {
        start_cos = _FB6_DL_NUML0_NODES;
        num_cos = SOC_FB6_NUM_SCHEDULER_PER_PORT;
    }

    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
        (unit, local_port, -1, &index, 0));

    profile_index = 16 * index;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_prio2cos_profile[unit],
                             profile_index, 16, rval64s));

    cos_bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILE_64r, rval64[fc_class],
                                       COS_BMPf);

    for (hw_cosq = start_cos; hw_cosq < num_cos; hw_cosq++) {
        if (!(cos_bmp & (1 << (hw_cosq - start_cos)))) {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_port_cos_resolve(unit, local_port, hw_cosq,
                                          _BCM_FB6_COSQ_INDEX_STYLE_SCHEDULER, 
                                           &gport_list[count++]));
    }

    if (count == 0) {
        return BCM_E_NOT_FOUND;
    }
    *gport_count = count;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_fb6_cosq_port_fc_profile_set
 * Purpose:
 *     Set PFC/SAFC Prio 2 Cos Map and set the profile index for the Port
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) GPORT identifier
 *     fc_class   - (IN) Input Class
 *     cos_bmp    - (IN)  Cos bitmap array
 *     num_class  - (IN) Number of input class
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_port_fc_profile_set(int unit, bcm_port_t local_port,
                int *fc_class, uint32 *cos_bmp, int num_class)
{
    uint32 profile_index, old_profile_index;
    uint64 rval64[16], *rval64s[1];
    int entry, cur_class;
    int index;
    uint32 cur_bmp;

    if ((fc_class == NULL) ||
        (cos_bmp == NULL)) {
        return BCM_E_PARAM;
    }

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
                        (unit, local_port, -1, &index, 0));

    old_profile_index = 16 * index;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_prio2cos_profile[unit],
                             old_profile_index, 16, rval64s));

    for (entry = 0; entry < num_class; entry++) {
        cur_class = fc_class[entry];
        cur_bmp = cos_bmp[entry];
        soc_reg64_field32_set(unit, PRIO2COS_PROFILE_64r, &rval64[cur_class],
                              COS_BMPf, cur_bmp);

    }

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_add(unit, _bcm_fb6_prio2cos_profile[unit], rval64s,
                             16, &profile_index));


    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_fb6_prio2cos_profile[unit],
                                old_profile_index));

    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
        (unit, local_port, profile_index/16, NULL, 1));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb6_fc_status_map_gport
 * Purpose:
 *      Set mapping in PFC/SAFC for gport, and update
 *      uc_cos_bmp/mc_cos_bmp with gport cosq.
 * Parameters:
 *      unit        - (IN) unit number
 *      port        - (IN) port number
 *      gport       - (IN) gport to be mapped
 *      cos_map     - (OUT) schedular hw_cosq of gport
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb6_fc_status_map_gport(int unit, bcm_port_t port,
                bcm_gport_t gport, uint32 *cos_bmp)
{
    _bcm_fb6_cosq_node_t *node = NULL;
    bcm_port_t local_port = -1;
    bcm_cos_queue_t cosq = -1;
    int l0_num;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, gport, NULL, &local_port, NULL, &node));

    if (node == NULL) {
        return BCM_E_PARAM;
    }
    if (local_port < 0) {
        return BCM_E_PARAM;
    }
    if (local_port != port) {
        return BCM_E_PARAM;
    }

   BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, port, &l0_num, NULL));

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        cosq = node->hw_index - (l0_num * 8);
        *cos_bmp |= 1 << cosq;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_pfc_class_mapping_check
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
_bcm_fb6_pfc_class_mapping_check(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int count, index, cur_class;
    bcm_port_t local_port = -1, resolved_port;
    bcm_gport_t cur_gport = 0;

    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
    if (local_port < 0) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_FB6_NUM_PFC_CLASS)) {
            return BCM_E_PARAM;
        }

        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }
            if (BCM_GPORT_IS_SET(cur_gport)) {
                if (!BCM_GPORT_IS_SCHEDULER(cur_gport)) { 
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_node_get(unit, cur_gport,
                                        NULL, &resolved_port, NULL, NULL));
                if (resolved_port != local_port) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_pfc_class_mapping_set
 * Purpose:
 *     Set PFC mapping for port.
 *     PFC class is mapped to cosq or scheduler node gports.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_pfc_class_mapping_set(int unit, bcm_gport_t port,
                                   int array_count,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0;
    int pfc_class[_BCM_FB6_NUM_PFC_CLASS];
    bcm_gport_t cur_gport = 0;
    uint32 cos_bmp[_BCM_FB6_NUM_PFC_CLASS] = {0};
    bcm_port_t local_port = -1;

    if ((array_count < 0) || (array_count > _BCM_FB6_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_pfc_class_mapping_check(unit, port,
                                          array_count, mapping_array));

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        cos_bmp[cur_class] = 0;

        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_fc_status_map_gport(unit, local_port, cur_gport,
                                              &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_FB6_NUM_PFC_CLASS; index++) {
        pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_port_fc_profile_set(unit, local_port, pfc_class,
                                           cos_bmp, _BCM_FB6_NUM_PFC_CLASS));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_pfc_class_mapping_get
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
bcm_fb6_cosq_pfc_class_mapping_get(int unit, bcm_gport_t port,
                                   int array_max,
                                   bcm_cosq_pfc_class_mapping_t *mapping_array,
                                   int *array_count)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    bcm_port_t local_port = -1;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }
    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }
    if (((mapping_array == NULL) && (array_max > 0)) ||
        ((mapping_array != NULL) && (array_max <= 0)) ||
        (array_count == NULL)) {
        return BCM_E_PARAM;
    }
    if (array_max <= 0) {
        *array_count = _BCM_FB6_NUM_PFC_CLASS;
        return BCM_E_NONE;
    }

    for (cur_class = 0; cur_class < _BCM_FB6_NUM_PFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        rv = _bcm_fb6_cosq_port_fc_get(unit, local_port, cur_class,
                                       mapping_array[class_count].gport_list,
                                       &actual_gport_count);

        if (rv == BCM_E_NONE) {
            mapping_array[class_count].class_id = cur_class;
            if (actual_gport_count < BCM_COSQ_PFC_GPORT_COUNT) {
                mapping_array[class_count].gport_list[actual_gport_count] =
                                                        BCM_GPORT_INVALID;
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
 *     bcm_fb6_cosq_safc_class_mapping_set
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
bcm_fb6_cosq_safc_class_mapping_set(int unit, bcm_gport_t port, int array_count,
                                bcm_cosq_safc_class_mapping_t *mapping_array)
{
    int index, count;
    int cur_class = 0;
    int safc_class[_BCM_FB6_NUM_SAFC_CLASS];
    bcm_gport_t cur_gport = 0;
    uint32 cos_bmp[_BCM_FB6_NUM_SAFC_CLASS] = {0};
    bcm_port_t local_port = -1;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    if ((array_count < 0) || (array_count > _BCM_FB6_NUM_SAFC_CLASS)) {
        return BCM_E_PARAM;
    }

    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_FB6_NUM_SAFC_CLASS)) {
            return BCM_E_PARAM;
        } else {
            cos_bmp[cur_class] = 0;
        }

        for (index = 0; index < BCM_COSQ_SAFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }

            if (BCM_GPORT_IS_SCHEDULER(cur_gport)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_fc_status_map_gport(unit, local_port, cur_gport,
                                                  &cos_bmp[cur_class]));
            } else {
                return BCM_E_PARAM;
            }
        }
    }

    for (index = 0; index < _BCM_FB6_NUM_SAFC_CLASS; index++) {
        safc_class[index] = index;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_port_fc_profile_set(unit, local_port, safc_class,
                                          cos_bmp,  _BCM_FB6_NUM_SAFC_CLASS));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_safc_class_mapping_get
 * Purpose:
 *     Get SAFC mapping for port.
 *     Retrieves cosq or scheduler node gports associated to SAFC classes.
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
bcm_fb6_cosq_safc_class_mapping_get(int unit, bcm_gport_t port, int array_max,
                                bcm_cosq_safc_class_mapping_t *mapping_array,
                                int *array_count)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    bcm_port_t local_port = -1;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, local_port)) {
        return BCM_E_PARAM;
    }

    if (((mapping_array == NULL) && (array_max > 0)) ||
        ((mapping_array != NULL) && (array_max <= 0)) ||
        (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    if (array_max <= 0) {
        *array_count = _BCM_FB6_NUM_SAFC_CLASS;
        return BCM_E_NONE;
    }

    for (cur_class = 0; cur_class < _BCM_FB6_NUM_SAFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        rv = _bcm_fb6_cosq_port_fc_get(unit, local_port, cur_class,
                                       mapping_array[class_count].gport_list,
                                       &actual_gport_count);

        if (rv == BCM_E_NONE) {
            mapping_array[class_count].class_id = cur_class;
            if (actual_gport_count < BCM_COSQ_SAFC_GPORT_COUNT) {
                mapping_array[class_count].gport_list[actual_gport_count] =
                                                BCM_GPORT_INVALID;
            }
            class_count++;
        } else if (rv != BCM_E_NOT_FOUND) {
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
/*
 * Function:
 *     _bcm_fb6_cosq_index_resolve
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
_bcm_fb6_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_fb6_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count)
{
    bcm_port_t resolved_port;
    soc_info_t *si;
    int resolved_index = -1;
    int id, startq, numq = 0;
    int pipe = 0;
    int cpu_mc_base; /* Q base number */
    int phy_port, mmu_port;
    int uc_base = 0;
    int mc_base = 0;

    _bcm_fb6_cosq_node_t *node;
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
            (_bcm_fb6_cosq_node_get(unit, port, NULL, &resolved_port, &id,
                                   &node));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = (IS_CPU_PORT(unit, resolved_port)) ? NUM_CPU_COSQ(unit) : NUM_COS(unit);
    }

    phy_port = si->port_l2p_mapping[resolved_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    uc_base = si->port_uc_cosq_base[resolved_port];

    switch (style) {
        case _BCM_FB6_COSQ_INDEX_STYLE_WRED_DEVICE:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if ((port == -1) || (cosq == -1)) {
                numq = _FB6_MMU_NUM_POOL; /* 4 Global Service Pools */
                resolved_index = _FB6_WRED_PER_PIPE_GLOBAL_SP_BASE;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_egress_sp_get(unit, port, cosq,
                                                 &resolved_index,
                                                 NULL));
                numq = 1;
                resolved_index += _FB6_WRED_PER_PIPE_GLOBAL_SP_BASE;
            }
            break;

        case _BCM_FB6_COSQ_INDEX_STYLE_WRED_PORT:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            resolved_index = 0;
            if (cosq != -1) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_egress_sp_get(unit, port, cosq,
                                                 &resolved_index,
                                                 NULL));
            }
            resolved_index += _FB6_WRED_PER_PIPE_PORT_SP_BASE +
                (mmu_port * 4);
            numq = (cosq == -1) ? 4: 1;
            break;

        case _BCM_FB6_COSQ_INDEX_STYLE_WRED_QUEUE:
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            resolved_index = uc_base;
            if (cosq != -1) {
                numq = si->port_num_cosq[resolved_port];
                if (cosq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index += cosq;
            }
            numq = (cosq == -1) ? NUM_COS(unit): 1;
            break;

        case _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL:
            if ((node != NULL) && (!BCM_GPORT_IS_MODPORT(port))) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    /* regular unicast queue */
                    resolved_index = node->hw_index ;
                    mem = MMU_THDU_Q_TO_QGRP_MAPm;
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                    resolved_index = node->hw_index ;
                    mem = MMU_THDM_MCQE_QUEUE_CONFIGm;
                } else { /* scheduler */
                    return BCM_E_PARAM;
                }
            } else { /* node == NULL */
                numq = si->port_num_cosq[resolved_port];
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = uc_base + startq;
                mem = MMU_THDU_Q_TO_QGRP_MAPm;
            }
            mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, resolved_index,
                        entry0));
            resolved_index = soc_mem_field32_get(unit, mem, entry0, Q_SPIDf);
            break;

        case _BCM_FB6_COSQ_INDEX_STYLE_COS:
            if (node) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    resolved_index = node->hw_index;
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                    if (IS_CPU_PORT(unit, resolved_port)) {
                        cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];
                        resolved_index = (node->hw_index - cpu_mc_base) %
                            NUM_CPU_COSQ(unit);
                    } else {
                        resolved_index = node->hw_index;
                    }
                } else {
                    resolved_index = node->hw_index;
                }
            } else {
                resolved_index = startq;
            }
            break;
        case _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE:
            /* Return hw_index for given unicast queue gport */
            if (node) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    resolved_index = node->hw_index;
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                if ((startq >= numq) || (IS_LB_PORT(unit, resolved_port))) {
                    return BCM_E_PARAM;
                }
                resolved_index = uc_base + startq;
            }
            break;
        case _BCM_FB6_COSQ_INDEX_STYLE_MCAST_QUEUE:
            /* Return hw_index for given multicast queue gport */
            if (node) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                    resolved_index = node->hw_index;
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = mc_base + startq;
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

STATIC int
_bcm_fb6_cosq_time_domain_get(int unit, int time_id, int *time_value)
{
    uint32 time_domain_val;
    soc_reg_t time_domain_reg = TIME_DOMAINr;

    if (time_id < 0 || time_id > _BCM_FB6_NUM_TIME_DOMAIN - 1) {
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

STATIC int
_bcm_fb6_cosq_time_domain_set(int unit, int time_value, int *time_id)
{
    int id;
    uint32 time_domain_val;
    soc_reg_t time_domain_reg = TIME_DOMAINr;

    if (time_value < 0 || time_value > 63) {
        return SOC_E_PARAM;
    }

    for (id = 0; id < _BCM_FB6_NUM_TIME_DOMAIN; id++) {
        BCM_IF_ERROR_RETURN(
            soc_reg32_get(unit, time_domain_reg, REG_PORT_ANY, id, &time_domain_val));
        if (!fb6_time_domain_info[unit][id].ref_count) {
            soc_reg_field_set(unit, time_domain_reg, &time_domain_val,
                              TIME_DOMAIN_FIELDf, time_value);
            fb6_time_domain_info[unit][id].ref_count++;
            break;
        }
    }

    if (id == _BCM_FB6_NUM_TIME_DOMAIN) {
        return BCM_E_RESOURCE;
    }

    if (time_id) {
        *time_id = id;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, time_domain_reg, REG_PORT_ANY, id, time_domain_val));
    return SOC_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_wred_resolution_entry_create
 * Purpose:
 *     Assign a unused entry. Default drop rules are WRED drop.
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (OUT) entry index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_wred_resolution_entry_create(int unit, int *index)
{
    uint32 i, rval = 0;
    soc_reg_t reg = WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    for (i = 0; i < _BCM_FB6_NUM_WRED_RESOLUTION_TABLE; i++) {
        if (_bcm_fb6_wred_resolution_tbl[i] == 0) {
            /* hw reset value */
            soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, 0xAAA);
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, i, rval));
            _bcm_fb6_wred_resolution_tbl[i]++;
            *index = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *     bcm_fb6_cosq_wred_resolution_entry_destroy
 * Purpose:
 *     Callback an entry and set its drop rules to WRED drop.
 * Parameters:
 *     unit                - (IN) unit number
 *     index               - (IN) entry index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_wred_resolution_entry_destroy(int unit, int index)
{
    uint32 rval = 0;
    soc_reg_t reg = WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    if ((index < 0) || (index >= _BCM_FB6_NUM_WRED_RESOLUTION_TABLE)) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_wred_resolution_tbl[index] == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* hw reset value */
    soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, 0xAAA);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));
    _bcm_fb6_wred_resolution_tbl[index] = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_wred_resolution_set
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
bcm_fb6_cosq_wred_resolution_set(int unit, int index, bcm_cosq_discard_rule_t *rule)
{
    uint32 offset, sp, qmin;
    uint32 rval, fval, drop;
    soc_reg_t reg = WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;

    if ((index < 0) || (index >= _BCM_FB6_NUM_WRED_RESOLUTION_TABLE)) {
        return BCM_E_PARAM;
    }

    if (_bcm_fb6_wred_resolution_tbl[index] == 0) {
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
            return BCM_E_PARAM;
    }

    switch (rule->queue_min_congestion_state) {
        case BCM_COSQ_DISCARD_CONGESTION_LOW:
            qmin = 0;
            break;
        case BCM_COSQ_DISCARD_CONGESTION_HIGH:
            qmin = 1;
            break;
        default:
            return BCM_E_PARAM;
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
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
    fval = soc_reg_field_get(unit, reg, rval, MMU_CONGESTION_EXPERIENCEf);

    offset = (sp * 2 + qmin) * 2;       /* 2 bits per rule */
    fval = (fval & (~(0x3 << offset))) | (drop << offset);

    soc_reg_field_set(unit, reg, &rval, MMU_CONGESTION_EXPERIENCEf, fval);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_wred_resolution_get
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
bcm_fb6_cosq_wred_resolution_get(int unit, int index, int max,
                                 bcm_cosq_discard_rule_t *rule, int *count)
{
    uint32 offset, sp, qmin, i = 0;
    uint32 rval, fval, action;
    soc_reg_t reg = WRED_CONG_NOTIFICATION_RESOLUTION_TABLEr;
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
        (index < 0) || (index >= _BCM_FB6_NUM_WRED_RESOLUTION_TABLE)) {
        return BCM_E_PARAM;
    }

    *count = 3 * 2;

    if (!rule) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
    fval = soc_reg_field_get(unit, reg, rval, MMU_CONGESTION_EXPERIENCEf);

    for (sp = 0; sp < 3; sp ++) {
        for (qmin = 0; qmin < 2; qmin++) {
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

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 flags, uint32 min_thresh, uint32 max_thresh,
                      int drop_probability, int gain, int ignore_enable_flags,
                      uint32 lflags,  int refresh_time, uint32 resolution_id,
                      uint32 use_queue_group)
{
    /* Local Variables */
    soc_mem_t wred_config_mem = INVALIDm;
    bcm_port_t local_port = -1;
    int index, i, start_index, end_index, count = 0;
    int end_offset, need_profile = 0, pipe = 0;
    uint32 profile_index, old_profile_index;
    uint32 wred_flags = 0;
    void *entries[9];
    soc_mem_t mems[9];
    static soc_mem_t wred_mems[9] = {
        MMU_WRED_DROP_CURVE_PROFILE_0m, MMU_WRED_DROP_CURVE_PROFILE_1m,
        MMU_WRED_DROP_CURVE_PROFILE_2m, MMU_WRED_DROP_CURVE_PROFILE_3m,
        MMU_WRED_DROP_CURVE_PROFILE_4m, MMU_WRED_DROP_CURVE_PROFILE_5m,
        MMU_WRED_DROP_CURVE_PROFILE_6m, MMU_WRED_DROP_CURVE_PROFILE_7m,
        MMU_WRED_DROP_CURVE_PROFILE_8m
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
    mmu_wred_config_entry_t entry;
    int rate = 0;
    int from_pipe, to_pipe;
    int time_id,time,old_time,current_time_sel;
    int rv = SOC_E_NONE, exists = 0;

    wred_flags = flags | lflags;
    end_offset = 0;

    /* Check Flags for WRED res level - Device-SP/Port-SP/Queues */
    if ((wred_flags & BCM_COSQ_DISCARD_DEVICE) && (port == BCM_GPORT_INVALID)) {
        /* (Port == -1) For all Global SPs */
        index = _FB6_WRED_PER_PIPE_GLOBAL_SP_BASE;
        count = _FB6_MMU_NUM_POOL;
        /*For all pipe*/
        from_pipe = 0;
        to_pipe = NUM_PIPE(unit) - 1;

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
        from_pipe = to_pipe = pipe;

        if (wred_flags & BCM_COSQ_DISCARD_DEVICE) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                            _BCM_FB6_COSQ_INDEX_STYLE_WRED_DEVICE,
                                            NULL, &index, &count));
            /* (cosq == -1) For all Global SPs */
        } else if (wred_flags & BCM_COSQ_DISCARD_PORT) {
            /* Port SP - Given or all SPs of the port */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                            _BCM_FB6_COSQ_INDEX_STYLE_WRED_PORT,
                                            NULL, &index, &count));
            /* (cosq == -1) For all SPs of the given port */
        } else {
            /* Per Queue WRED settings */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                            _BCM_FB6_COSQ_INDEX_STYLE_WRED_QUEUE,
                                            NULL, &index, &count));
            /* (cosq == -1) For all Queues of the given port */
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

    rate = _bcm_fb6_percent_to_drop_prob(drop_probability);

    for (pipe = from_pipe; pipe <= to_pipe; pipe++ ) {
        /* Config MMU_WRED_CONFIGm for the respective resource */
        wred_config_mem = MMU_WRED_CONFIGm;
        wred_config_mem = SOC_MEM_UNIQUE_ACC(unit, wred_config_mem)[pipe];
        if (wred_config_mem == INVALIDm) {
            return BCM_E_PARAM;
        }

        for (i=0; i < 9; i++) {
            mems[i] = INVALIDm;
        }

        if (wred_flags & (BCM_COSQ_DISCARD_COLOR_ALL |
                          BCM_COSQ_DISCARD_TCP |
                          BCM_COSQ_DISCARD_NONTCP |
                          BCM_COSQ_DISCARD_ECT_MARKED |
                          BCM_COSQ_DISCARD_RESPONSIVE_DROP |
                          BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
            need_profile = 1;

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

        for (index = start_index; index <= end_index; index++) {
            old_profile_index = 0xffffffff;
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, wred_config_mem, MEM_BLOCK_ALL, index, &entry));

            if (need_profile) {
                old_profile_index = soc_mem_field32_get(unit, wred_config_mem,
                                                        &entry, PROFILE_INDEXf);

                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_get(unit, _bcm_fb6_wred_profile[unit],
                                         old_profile_index, 1, entries));

                for (i = 0; i < FB6_WRED_PROFILE_NUM; i++) {
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
                    (soc_profile_mem_add(unit, _bcm_fb6_wred_profile[unit],
                                         entries, 1, &profile_index));
                soc_mem_field32_set(unit, wred_config_mem, &entry, PROFILE_INDEXf, profile_index);
            }
                soc_mem_field32_set(unit, wred_config_mem, &entry, WEIGHTf, gain);

            /* Some APIs only modify profiles */
            if (!ignore_enable_flags) {
                soc_mem_field32_set(unit, wred_config_mem, &entry, CAP_AVERAGEf,
                                    flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
                soc_mem_field32_set(unit, wred_config_mem, &entry, WRED_ENf,
                                    flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
                soc_mem_field32_set(unit, wred_config_mem, &entry, ECN_MARKING_ENf,
                                    flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?  1 : 0);
            }

            current_time_sel = soc_mem_field32_get(unit, wred_config_mem,
                                                   &entry, TIME_DOMAIN_SELf);
            time = refresh_time - 1;
            exists = 0;
            /* If the time value exist, update reference count only */
            for (time_id = 0; time_id < _BCM_FB6_NUM_TIME_DOMAIN; time_id++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_time_domain_get(unit, time_id, &old_time));
                if (time == old_time) {
                    /* Only set exist flag if this entry reference count already update,
                                  otherwise update reference count */
                    if(time_id != current_time_sel){
                        soc_mem_field32_set(unit, wred_config_mem, &entry,
                                            TIME_DOMAIN_SELf, time_id);
                        fb6_time_domain_info[unit][time_id].ref_count++;
                        fb6_time_domain_info[unit][current_time_sel].ref_count--;
                    }
                    exists = 1;
                    break;
                }
            }

            if(!exists){
                rv = _bcm_fb6_cosq_time_domain_set(unit, time, &time_id);
                if(rv == SOC_E_NONE){
                    soc_mem_field32_set(unit, wred_config_mem, &entry,
                                        TIME_DOMAIN_SELf, time_id);
                    fb6_time_domain_info[unit][current_time_sel].ref_count--;
                } else {
                    return rv;
                }
            }

            soc_mem_field32_set(unit, wred_config_mem, &entry, QG_ENf,
                                use_queue_group ? 1 : 0);
            if (flags & BCM_COSQ_DISCARD_MARK_CONGESTION) {
                soc_mem_field32_set(unit, wred_config_mem, &entry,
                    WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf,
                    resolution_id);
            } else {
                soc_mem_field32_set(unit, wred_config_mem, &entry,
                    WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf,
                    resolution_id);
            }


            BCM_IF_ERROR_RETURN(
                    soc_mem_write(unit, wred_config_mem, MEM_BLOCK_ALL, index, &entry));

            if (old_profile_index != 0xffffffff) {
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_delete(unit, _bcm_fb6_wred_profile[unit],
                                            old_profile_index));
            }

        }

    }

    /* Check Flags for WRED profile based on Color */
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cosq_wred_get
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
 *     resolution_id       - (OUT) Resolution index
 *     use_queue_group     - (OUT) Queue group status is used
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_fb6_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                      int *drop_probability, int *gain, uint32 lflags,
                      int *refresh_time, uint32 *resolution_id,
                      uint32 *use_queue_group)
{
    soc_mem_t wred_config_mem = INVALIDm, mem = INVALIDm;
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
    void *entries[9];
    void *entry_p;
    int index, profile_index;
    mmu_wred_config_entry_t entry;
    uint32 wred_flags = 0;
    int rate = 0, pipe = 0;
    int time_id, time;

    if (flags) {
        wred_flags = *flags | lflags;
    } else {
        wred_flags = lflags;
    }

    if ((wred_flags & BCM_COSQ_DISCARD_DEVICE) && (port == BCM_GPORT_INVALID)) {
        /* (Port == -1) For all Global SPs */
        index = _FB6_WRED_PER_PIPE_GLOBAL_SP_BASE;
        pipe = 0;

    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

        /* Check Flags for WRED res level - Device-SP/Port-SP/Queues */
        if(wred_flags & BCM_COSQ_DISCARD_DEVICE) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                        _BCM_FB6_COSQ_INDEX_STYLE_WRED_DEVICE,
                                        NULL, &index, NULL));
        } else if (wred_flags & BCM_COSQ_DISCARD_PORT) {
            /* Port SP - Given or all SPs of the port */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                            _BCM_FB6_COSQ_INDEX_STYLE_WRED_PORT,
                                            NULL, &index, NULL));
        } else {
            /* Per Queue WRED settings */
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                            _BCM_FB6_COSQ_INDEX_STYLE_WRED_QUEUE,
                                            NULL, &index, NULL));
        }
    }

    if ((index < 0) || (pipe == -1)) {
        return BCM_E_PARAM;
    }

    wred_config_mem = MMU_WRED_CONFIGm;
    wred_config_mem = SOC_MEM_UNIQUE_ACC(unit, wred_config_mem)[pipe];
    if (wred_config_mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, wred_config_mem, MEM_BLOCK_ALL, index, &entry));
    profile_index = soc_mem_field32_get(unit, wred_config_mem,
                                        &entry, PROFILE_INDEXf);

    if (!(wred_flags & (BCM_COSQ_DISCARD_NONTCP |
                        BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP |
                        BCM_COSQ_DISCARD_ECT_MARKED))) {
        /* By default we will assume
           BCM_COSQ_DISCARD_TCP */
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_1m;
            entry_p = &entry_tcp_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_2m;
            entry_p = &entry_tcp_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
            entry_p = &entry_tcp_green;
        }
    } else if ((wred_flags & BCM_COSQ_DISCARD_NONTCP) ||
               (wred_flags & BCM_COSQ_DISCARD_NON_RESPONSIVE_DROP)) {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_4m;
            entry_p = &entry_nontcp_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_5m;
            entry_p = &entry_nontcp_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
            entry_p = &entry_nontcp_green;
        }
    } else {
        if (wred_flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_7m;
            entry_p = &entry_ecn_yellow;
        } else if (wred_flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_8m;
            entry_p = &entry_ecn_red;
        } else {
            /* By default we will assume
               BCM_COSQ_DISCARD_COLOR_GREEN */
            mem = MMU_WRED_DROP_CURVE_PROFILE_6m;
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
        (soc_profile_mem_get(unit, _bcm_fb6_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_fb6_drop_prob_to_percent(rate);
    }
    if (gain) {
        *gain = soc_mem_field32_get(unit, wred_config_mem, &entry, WEIGHTf);
    }

    if (refresh_time) {
            time_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                                          TIME_DOMAIN_SELf);
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_time_domain_get(unit,time_id,&time));
            *refresh_time = time + 1;
    }

    /* Get flags */
    if (flags) {
        if (soc_mem_field32_get(unit, wred_config_mem, &entry, CAP_AVERAGEf)) {
            *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
        }
        if (soc_mem_field32_get(unit, wred_config_mem, &entry, WRED_ENf)) {
            *flags |= BCM_COSQ_DISCARD_ENABLE;
        }
        if (soc_mem_field32_get(unit, wred_config_mem, &entry, ECN_MARKING_ENf)) {
            *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
    }

    /* Get resolution table index */
    if (resolution_id) {
        if (soc_mem_field32_get(unit, wred_config_mem, &entry, ECN_MARKING_ENf)) {
            *resolution_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_MARKING_INDEXf);
        } else {
            *resolution_id = soc_mem_field32_get(unit, wred_config_mem, &entry,
                WRED_CONG_NOTIFICATION_RESOLUTION_TABLE_DROPPING_INDEXf);
        }
    }

    /* Get use_queue_group */
    if (use_queue_group) {
        *use_queue_group =
            soc_mem_field32_get(unit, wred_config_mem, &entry, QG_ENf);
    }

    return BCM_E_NONE;
}


int
bcm_fb6_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;

    if (_bcm_fb6_cosq_port_info[unit] == NULL) {
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
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_wred_set(unit, port, 0, flags, 0, 0, 0, 0,
                                   FALSE, BCM_COSQ_DISCARD_PORT, 0x1, 0, 0));
    }

    return BCM_E_NONE;
}

int
bcm_fb6_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_fb6_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL, BCM_COSQ_DISCARD_PORT, NULL, NULL,
                                     NULL);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_fb6_cosq_discard_port_set
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
bcm_fb6_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
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
    max_thresh = min_thresh + _bcm_fb6_angle_to_cells(drop_slope);
    if (max_thresh > FB6_WRED_CELL_FIELD_MAX) {
        max_thresh = FB6_WRED_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        numq = 1;
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_wred_set(unit, port, cosq + i, color,
                                        min_thresh, max_thresh, 100, gain,
                                        TRUE, flags, 0x1, 0, 0));
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
                    (_bcm_fb6_cosq_wred_set(unit, local_port, cosq + i,
                                            color, min_thresh, max_thresh, 100,
                                            gain, TRUE, 0, 0x1, 0, 0));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_discard_port_get
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
bcm_fb6_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
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
        (_bcm_fb6_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                                &color, &min_thresh, &max_thresh, &drop_prob,
                                &gain, 0, NULL, NULL, NULL));

    /*
     * average queue size is reculated every 8us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain) * 8;

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
    *drop_slope = _bcm_fb6_cells_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_discard_set
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
bcm_fb6_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    int numq, i;
    uint32 min_thresh, max_thresh, flags = 0;
    int cell_size, cell_field_max;
    bcm_port_t local_port = -1;

    /* the reasonable range of refresh_time  is from 1 to 64 */
    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100 ||
        discard->refresh_time <= 0 || discard->refresh_time > 64 || 
        discard->profile_id > 3) {
        return BCM_E_PARAM;
    }

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100) {
        return BCM_E_PARAM;
    }

    cell_size = _FB6_MMU_BYTES_PER_CELL;
    cell_field_max = FB6_WRED_CELL_FIELD_MAX;

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
    } else if (discard->flags & BCM_COSQ_DISCARD_PACKETS) {
        /* Packet mode not supported */
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
            return BCM_E_PARAM;
        }
    }

    if (gport != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
        if ((cosq < -1) || (cosq >= SOC_PORT_NUM_COSQ(unit, local_port))) {
            return BCM_E_PARAM;
        }
    }

    numq = 1;

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_wred_set(unit, gport, cosq + i, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE, flags, discard->refresh_time,
                                   discard->profile_id, discard->use_queue_group));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fb6_cosq_gport_discard_get
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
bcm_fb6_cosq_gport_discard_get(int unit, bcm_gport_t gport,
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
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        if (IS_CPU_PORT(unit,local_port) || IS_LB_PORT(unit,local_port)) {
            return BCM_E_PORT;
        }
        if ((cosq < -1) || (cosq >= SOC_PORT_NUM_COSQ(unit, local_port))) {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                                &discard->flags, &min_thresh, &max_thresh,
                                &discard->drop_probability, &discard->gain,
                                0, &discard->refresh_time,
                                &discard->profile_id, &discard->use_queue_group));

    /* Convert number of cells to number of bytes */
    discard->min_thresh = min_thresh * _FB6_MMU_BYTES_PER_CELL;
    discard->max_thresh = max_thresh * _FB6_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/* For a given logical port, return the OBM id and the lane number */
int
bcm_firebolt6_port_obm_info_get(int unit, soc_port_t port, 
        int *obm_id, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int clport, port_block_base;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
            (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port))) {
        return SOC_E_PARAM;
    }
    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    if (lane != NULL) {
        *lane = (phy_port - port_block_base) % _FB6_PORTS_PER_PBLK;
    }

    clport = si->port_serdes[port];
    if ((obm_id != NULL) && (NUM_PIPE(unit) != 1)) { 
        *obm_id = clport & 0xF;
    } else {
        *obm_id = clport;
    }
    return SOC_E_NONE;
}


STATIC int
_bcm_fb6_cosq_obmhighwatermark_set(int unit, bcm_port_t port, uint64 value)
{
    int pipe, obm_id, lane;
    soc_reg_t reg;
    uint64 rval64;
    soc_info_t *si;
    soc_reg_t obm_high_watermark_reg[] = {
        IDB_OBM0_MAX_USAGEr, IDB_OBM1_MAX_USAGEr,
        IDB_OBM2_MAX_USAGEr, IDB_OBM3_MAX_USAGEr,
        IDB_OBM4_MAX_USAGEr, IDB_OBM5_MAX_USAGEr,
        IDB_OBM6_MAX_USAGEr, IDB_OBM7_MAX_USAGEr,
        IDB_OBM8_MAX_USAGEr, IDB_OBM9_MAX_USAGEr,
        IDB_OBM10_MAX_USAGEr, IDB_OBM11_MAX_USAGEr,
        IDB_OBM12_MAX_USAGEr, IDB_OBM13_MAX_USAGEr,
        IDB_OBM14_MAX_USAGEr, IDB_OBM15_MAX_USAGEr,
        IDB_OBM16_MAX_USAGEr,
    };

    soc_field_t obm_high_watermark_fields[] = {
        PORT0_MAX_USAGEf, PORT1_MAX_USAGEf,
        PORT2_MAX_USAGEf, PORT3_MAX_USAGEf
    };

    SOC_IF_ERROR_RETURN
        (bcm_firebolt6_port_obm_info_get(unit, port, &obm_id, &lane));

    if (obm_id >= _FB6_PBLKS_PER_PIPE ||
            (lane < 0 || lane >= _FB6_PORTS_PER_PBLK)) {
        return BCM_E_PARAM;
    }
    if(!COMPILER_64_IS_ZERO(value)) {
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_high_watermark_reg[obm_id])[pipe];
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));

    soc_reg64_field32_set(unit, reg, &rval64, obm_high_watermark_fields[lane], 0);
    BCM_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_obmhighwatermark_get(int unit, bcm_port_t port, uint64 *value)
{
    int pipe, obm_id, lane;
    soc_reg_t reg;
    uint64 rval64;
    soc_info_t *si;
    soc_reg_t obm_high_watermark_reg[] = {
        IDB_OBM0_MAX_USAGEr, IDB_OBM1_MAX_USAGEr,
        IDB_OBM2_MAX_USAGEr, IDB_OBM3_MAX_USAGEr,
        IDB_OBM4_MAX_USAGEr, IDB_OBM5_MAX_USAGEr,
        IDB_OBM6_MAX_USAGEr, IDB_OBM7_MAX_USAGEr,
        IDB_OBM8_MAX_USAGEr, IDB_OBM9_MAX_USAGEr,
        IDB_OBM10_MAX_USAGEr, IDB_OBM11_MAX_USAGEr,
        IDB_OBM12_MAX_USAGEr, IDB_OBM13_MAX_USAGEr,
        IDB_OBM14_MAX_USAGEr, IDB_OBM15_MAX_USAGEr,
        IDB_OBM16_MAX_USAGEr
    };

    soc_field_t obm_high_watermark_fields[] = {
        PORT0_MAX_USAGEf, PORT1_MAX_USAGEf,
        PORT2_MAX_USAGEf, PORT3_MAX_USAGEf
    };

    SOC_IF_ERROR_RETURN
        (bcm_firebolt6_port_obm_info_get(unit, port, &obm_id, &lane));
    if (obm_id >= _FB6_PBLKS_PER_PIPE ||
            (lane < 0 || lane >= _FB6_PORTS_PER_PBLK)) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_high_watermark_reg[obm_id])[pipe];
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &rval64));

    *value = soc_reg64_field_get(unit, reg, rval64, obm_high_watermark_fields[lane]);
    COMPILER_64_UMUL_32(*value, 16);    /* 16B cell granularity. */

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_obm_buf_use_cnt_set(int unit, bcm_port_t port, uint64 value)
{
    int pipe, obm_id, lane;
    soc_reg_t reg;
    soc_field_t fld;
    uint64 rval64;
    soc_info_t *si;
    soc_reg_t obm_usage_reg[] = {
        IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
        IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
        IDB_OBM4_USAGEr, IDB_OBM5_USAGEr,
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr,
        IDB_OBM8_USAGEr, IDB_OBM9_USAGEr,
        IDB_OBM10_USAGEr, IDB_OBM11_USAGEr,
        IDB_OBM12_USAGEr, IDB_OBM13_USAGEr,
        IDB_OBM14_USAGEr, IDB_OBM15_USAGEr,
        IDB_OBM16_USAGEr,
    };

    BCM_IF_ERROR_RETURN
            (bcm_firebolt6_port_obm_info_get(unit, port, &obm_id, &lane));
    if (obm_id >= _FB6_PBLKS_PER_PIPE ||
            (lane < 0 || lane >= _FB6_PORTS_PER_PBLK)) {
        return BCM_E_PARAM;
    }
    if(!COMPILER_64_IS_ZERO(value)) {
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg[obm_id])[pipe];
    fld = si->mmu_lossless ? LOSSLESS0_COUNTf : LOSSY_COUNTf;
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, lane, &rval64));

    soc_reg64_field_set(unit, reg, &rval64, fld, value);
    BCM_IF_ERROR_RETURN
            (soc_reg64_set(unit, reg, REG_PORT_ANY, lane, rval64));
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_obm_buf_use_cnt_get(int unit, bcm_port_t port, uint64 *value)
{
    int pipe, obm_id, lane;
    soc_reg_t reg;
    soc_field_t fld;
    uint64 rval64;
    soc_info_t *si;
    soc_reg_t obm_usage_reg[] = {
        IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
        IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
        IDB_OBM4_USAGEr, IDB_OBM5_USAGEr,
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr,
        IDB_OBM8_USAGEr, IDB_OBM9_USAGEr,
        IDB_OBM10_USAGEr, IDB_OBM11_USAGEr,
        IDB_OBM12_USAGEr, IDB_OBM13_USAGEr,
        IDB_OBM14_USAGEr, IDB_OBM15_USAGEr,
        IDB_OBM16_USAGEr,
    };

    SOC_IF_ERROR_RETURN
            (bcm_firebolt6_port_obm_info_get(unit, port, &obm_id, &lane));
    if (obm_id >= _FB6_PBLKS_PER_PIPE ||
            (lane < 0 || lane >= _FB6_PORTS_PER_PBLK)) {
        return BCM_E_PARAM;
    }
    si = &SOC_INFO(unit);
    pipe = si->port_pipe[port];

    reg = SOC_REG_UNIQUE_ACC(unit, obm_usage_reg[obm_id])[pipe];
    fld = si->mmu_lossless ? LOSSLESS0_COUNTf : LOSSY_COUNTf;
    COMPILER_64_ZERO(rval64);
    BCM_IF_ERROR_RETURN
            (soc_reg64_get(unit, reg, REG_PORT_ANY, lane, &rval64));

    *value = soc_reg64_field_get(unit, reg, rval64, fld);
    COMPILER_64_UMUL_32(*value, 16);    /* 16B cell granularity. */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb6_cosq_stat_set
 * Purpose:
 *      Set MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) GPORT ID
 *      cosq  - (IN) COS queue.
 *      stat  - (IN) Statistic to be set.
 *      value - (IN) Statistic value to be set.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_fb6_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg, ctr_reg_uc, ctr_reg_mc;
    _bcm_fb6_cosq_index_style_t style = _BCM_FB6_COSQ_INDEX_STYLE_COUNT;
    int startq = 0, ci, from_cos, to_cos;
    int fport;
    bcm_port_t local_port = -1;
    soc_info_t *si = &SOC_INFO(unit);

    ctrl_info.instance = -1;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
    if ((local_port < _BCM_FB6_NUM_PORTS) &&
            ((cosq < -1) || (cosq >=  si->port_num_cosq[local_port]))) {
        return BCM_E_PARAM;
    }

    switch (stat) {
        case bcmCosqStatDroppedPackets:
        case bcmCosqStatDroppedBytes:
        case bcmCosqStatOutPackets:
        case bcmCosqStatOutBytes:
            style = _BCM_FB6_COSQ_INDEX_STYLE_COS;
            if (stat == bcmCosqStatDroppedBytes) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
            } else if (stat == bcmCosqStatDroppedPackets) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
            } else if (stat == bcmCosqStatOutBytes) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
                ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            } else { /* (stat == bcmCosqStatOutPackets) */
                ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
                ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            }
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (ctr_reg_uc != INVALIDr) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));
                    if((local_port > _BCM_FB6_NUM_PORTS) &&
                            ((stat == bcmCosqStatOutPackets) ||
                            (stat == bcmCosqStatOutBytes))) {
                        BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, local_port, &fport));
                        ctrl_info.instance = fport;
                        startq = startq - si->port_uc_cosq_base[fport];
                    } else {
                        ctrl_info.instance = local_port;
                        startq = startq - si->port_uc_cosq_base[local_port];
                    }
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_set(unit, ctr_reg_uc, ctrl_info, 0,
                                                     startq, value));
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                if (ctr_reg_mc != INVALIDr) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));
                    if((local_port > _BCM_FB6_NUM_PORTS) &&
                            ((stat == bcmCosqStatOutPackets) ||
                            (stat == bcmCosqStatOutBytes))) {
                        BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, local_port, &fport));
                        ctrl_info.instance = fport;
                        startq = startq - si->port_uc_cosq_base[fport];
                    } else {
                        ctrl_info.instance = local_port;
                        startq = startq - si->port_cosq_base[local_port];
                    }
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_set(unit, ctr_reg_mc, ctrl_info, 0,
                                                startq, value));
                }
            } else {
                if (BCM_GPORT_IS_SCHEDULER(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));
                    from_cos = to_cos = startq;
                } else {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_localport_resolve(unit, port,
                                                        &local_port));

                    if (cosq == BCM_COS_INVALID) {
                        /* if (cosq == -1) set value for the first cosq */
                        cosq = 0;
                    }
                    from_cos = to_cos = cosq;
                }
                ctrl_info.instance = local_port;
                for (ci = from_cos; ci <= to_cos; ci++) {
                    if (!IS_CPU_PORT(unit, local_port)) {
                        if (ctr_reg_uc != INVALIDr) {
                            /* Set UC values if NOT CPU port */
                            BCM_IF_ERROR_RETURN
                                (soc_counter_generic_set(unit, ctr_reg_uc,
                                                         ctrl_info, 0,
                                                         ci, value));
                        }
                    }

                    if (ctr_reg_mc != INVALIDr) {
                        BCM_IF_ERROR_RETURN
                            (soc_counter_generic_set(unit, ctr_reg_mc,
                                                     ctrl_info, 0, ci, value));
                    }
                }
            }
            break;
        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatRedCongestionDroppedPackets:
        case bcmCosqStatGreenDiscardDroppedPackets:
        case bcmCosqStatYellowDiscardDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_UNAVAIL;
            }
            if (stat == bcmCosqStatYellowCongestionDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
            } else if (stat == bcmCosqStatRedCongestionDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
            } else if (stat == bcmCosqStatYellowDiscardDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
            } else if (stat == bcmCosqStatRedDiscardDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
            } else {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

            ctrl_info.instance = local_port;
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                         startq, value));
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
        case bcmCosqStatOBMFlowControlEvents:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
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
            } else if (stat == bcmCosqStatOBMLossless1DroppedBytes) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
            } else { /* (stat == bcmCosqStatOBMFlowControlEvents) */
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
            }
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            ctrl_info.instance = local_port;
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                         0, value));
            break;
        case bcmCosqStatOBMHighWatermark:
        case bcmCosqStatOBMBufferBytes:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port) ||
                    BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if (stat == bcmCosqStatOBMHighWatermark) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_obmhighwatermark_set(unit, local_port, value));
            } else { /* stat == bcmCosqStatOBMBufferBytes */
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_obm_buf_use_cnt_set(unit, local_port, value));
            }
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get shared pool
 * for given Ingress [Port, Priority_group]
 */
STATIC int
_bcm_fb6_cosq_ingress_sp_get_by_pg(int unit, bcm_gport_t gport, bcm_cos_queue_t pri_grp,
        int *p_pool_start, int *p_pool_end)
{
    int local_port;
    uint32 rval, pool; 
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };    

    if (pri_grp == BCM_COS_INVALID) {
        if (p_pool_start && p_pool_end) {
            *p_pool_start = 0;
            *p_pool_end = 3;
            return BCM_E_NONE;
        } else {
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if (pri_grp >= _FB6_MMU_NUM_PG) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, local_port, &rval));
    pool = soc_reg_field_get(unit, THDI_PORT_PG_SPIDr, rval, 
            prigroup_spid_field[pri_grp]);

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
 *      bcm_fb6_cosq_stat_get
 * Purpose:
 *      Get MMU statistics on a per port per cosq basis.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) GPORT ID.
 *      cosq      - (IN) COS queue.
 *      stat      - (IN) Statistic to be retrieved.
 *      sync_mode - (IN) whether to sync before read.
 *      value     - (OUT) Returned statistic value.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_fb6_cosq_stat_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    soc_ctr_control_info_t ctrl_info;
    soc_reg_t ctr_reg, ctr_reg_uc, ctr_reg_mc;
    soc_mem_t base_mem = INVALIDm, mem = INVALIDm;
    soc_field_t field = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 rval, value32 = 0;
    _bcm_fb6_cosq_index_style_t style = _BCM_FB6_COSQ_INDEX_STYLE_COUNT;
    uint64 sum, value64;
    int startq = 0, ci, from_cos, to_cos, i, start_hw_index, end_hw_index;
    bcm_port_t local_port = -1;
    int sync = sync_mode ? SOC_COUNTER_SYNC_ENABLE : 0;
    int fport;
    int cpu_mc_base = 0, pool, mem_idx;
    soc_info_t *si = &SOC_INFO(unit);
    cpu_mc_base = si->port_cosq_base[CMIC_PORT(unit)];

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
    ctrl_info.instance = -1;
    ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;

    switch (stat) {
        case bcmCosqStatDroppedPackets:
        case bcmCosqStatDroppedBytes:
        case bcmCosqStatOutPackets:
        case bcmCosqStatOutBytes:
        case bcmCosqStatEgressMCQueueBytesPeak:
        case bcmCosqStatEgressMCQueueBytesCurrent:
        case bcmCosqStatEgressMCQueueMinBytesCurrent:
        case bcmCosqStatEgressUCQueueBytesPeak:
        case bcmCosqStatEgressUCQueueBytesCurrent:
        case bcmCosqStatEgressUCQueueMinBytesCurrent:
            if ((local_port < _BCM_FB6_NUM_PORTS) && ((cosq < -1) || (cosq >=  si->port_num_cosq[local_port]))) {
                return BCM_E_PARAM;
            }
            style = _BCM_FB6_COSQ_INDEX_STYLE_COS;
            if (stat == bcmCosqStatEgressMCQueueBytesPeak) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                ctr_reg_uc = INVALIDr;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_QUEUE_PEAK;
            } else if (stat == bcmCosqStatEgressMCQueueBytesCurrent) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                ctr_reg_uc = INVALIDr;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_QUEUE_CURRENT;
            } else if (stat == bcmCosqStatEgressMCQueueMinBytesCurrent) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                        BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                if (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve(unit, port, cosq, style,
                                                     &local_port, &startq, NULL));
                    if (IS_CPU_PORT(unit, local_port)) {
                        startq = (cpu_mc_base + startq) - si->port_cosq_base[local_port];
                    } else {
                        startq = startq - si->port_cosq_base[local_port];
                    }
                }
                ctr_reg_uc = INVALIDr;
                ctr_reg_mc = INVALIDr;
                base_mem = MMU_THDM_DB_QUEUE_COUNTm;
                field = MIN_COUNTf;
            } else if (stat == bcmCosqStatEgressUCQueueBytesPeak) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_localport_resolve(unit, port,
                                                         &local_port));
                    if (IS_CPU_PORT(unit, local_port))
                    {
                        return BCM_E_PARAM;
                    }
                }
                ctr_reg_uc = SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK;
                ctr_reg_mc = INVALIDr;
            } else if (stat == bcmCosqStatEgressUCQueueBytesCurrent) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_localport_resolve(unit, port,
                                                         &local_port));
                    if (IS_CPU_PORT(unit, local_port))
                    {
                        return BCM_E_PARAM;
                    }
                }
                ctr_reg_uc = SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT;
                ctr_reg_mc = INVALIDr;
            } else if (stat == bcmCosqStatEgressUCQueueMinBytesCurrent) {
                if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                        BCM_GPORT_IS_SCHEDULER(port)) {
                    return BCM_E_PARAM;
                }
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_localport_resolve(unit, port,
                                                         &local_port));
                    if (IS_CPU_PORT(unit, local_port))
                    {
                        return BCM_E_PARAM;
                    }
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve(unit, port, cosq, style,
                                                     &local_port, &startq, NULL));

                    startq = startq - si->port_uc_cosq_base[local_port];
                }
                ctr_reg_uc = INVALIDr;
                ctr_reg_mc = INVALIDr;
                base_mem = MMU_THDU_COUNTER_QUEUEm;
                field = MIN_COUNTf;
            } else if (stat == bcmCosqStatDroppedBytes) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
            } else if (stat == bcmCosqStatDroppedPackets) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
            } else if (stat == bcmCosqStatOutBytes) {
                ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
                ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            } else { /* (stat == bcmCosqStatOutPackets) */
                ctr_reg_uc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
                ctr_reg_mc = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
                ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            }
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if ((ctr_reg_uc != INVALIDr) || (base_mem != INVALIDm)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));

                    if((local_port > _BCM_FB6_NUM_PORTS) &&
                            ((stat == bcmCosqStatOutPackets) ||
                             (stat == bcmCosqStatOutBytes))) {
                        BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, local_port, &fport));
                        ctrl_info.instance = fport;
                        startq = startq - si->port_uc_cosq_base[fport];
                    } else {
                        ctrl_info.instance = local_port;
                        startq = startq - si->port_uc_cosq_base[local_port];
                    }
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_get(unit, ctr_reg_uc, ctrl_info,
                                                 sync, startq, value));
                } else {
                    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                startq, entry));
                    value32 = soc_mem_field32_get(unit, mem, entry, field);
                    COMPILER_64_ADD_32(*value, value32);
                    COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);

                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                if ((ctr_reg_uc != INVALIDr) || (base_mem != INVALIDm)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));
                    if((local_port > _BCM_FB6_NUM_PORTS) &&
                            ((stat == bcmCosqStatOutPackets) ||
                             (stat == bcmCosqStatOutBytes))) {
                        BCM_IF_ERROR_RETURN(bcm_cosq_coe_port_get(unit, local_port, &fport));
                        ctrl_info.instance = fport;
                        startq = startq - si->port_cosq_base[fport];
                    } else {
                        ctrl_info.instance = local_port;
                        startq = startq - si->port_cosq_base[local_port];
                    }
                    BCM_IF_ERROR_RETURN
                        (soc_counter_generic_get(unit, ctr_reg_mc, ctrl_info,
                                                 sync, startq, value));
                } else {
                    mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                startq, entry));
                    value32 = soc_mem_field32_get(unit, mem, entry, field);
                    COMPILER_64_ADD_32(*value, value32);
                    COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);

                }
            } else {
                if (BCM_GPORT_IS_SCHEDULER(port)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_index_resolve
                         (unit, port, cosq, style, &local_port, &startq, NULL));
                    from_cos = to_cos = startq;
                } else {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb6_cosq_localport_resolve(unit, port,
                                                        &local_port));

                    if (cosq == BCM_COS_INVALID) {
                        from_cos = 0;
                        to_cos = (IS_CPU_PORT(unit, local_port)) ? \
                            NUM_CPU_COSQ(unit) - 1 : _BCM_FB6_NUM_COS(unit) - 1;
                    } else {
                        from_cos = to_cos = cosq;
                    }
                }
                COMPILER_64_ZERO(sum);
                ctrl_info.instance = local_port;
                for (ci = from_cos; ci <= to_cos; ci++) {

                    if (!IS_CPU_PORT(unit, local_port)) {
                        /* Get UC values if NOT CPU port */
                        if (ctr_reg_uc != INVALIDr) {
                            BCM_IF_ERROR_RETURN
                                (soc_counter_generic_get(unit, ctr_reg_uc,
                                                         ctrl_info,
                                                         sync, ci, &value64));
                            COMPILER_64_ADD_64(sum, value64);
                        }
                    }

                    if (ctr_reg_mc != INVALIDr) {
                        BCM_IF_ERROR_RETURN
                            (soc_counter_generic_get(unit, ctr_reg_mc,
                                                     ctrl_info,
                                                     sync, ci, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                    if (base_mem != INVALIDm) {
                        mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
                        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                    startq + ci, entry));
                        value32 = soc_mem_field32_get(unit, mem, entry, field) *
                            _FB6_MMU_BYTES_PER_CELL;
                        COMPILER_64_ADD_32(sum, value32);
                    }
                }
                *value = sum;
            }
            break;
        case bcmCosqStatYellowCongestionDroppedPackets:
        case bcmCosqStatRedCongestionDroppedPackets:
        case bcmCosqStatGreenDiscardDroppedPackets:
        case bcmCosqStatYellowDiscardDroppedPackets:
        case bcmCosqStatRedDiscardDroppedPackets:
            if ((cosq < -1) || (cosq >=  si->port_num_cosq[local_port])) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_UNAVAIL;
            }
            if (stat == bcmCosqStatYellowCongestionDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
            } else if (stat == bcmCosqStatRedCongestionDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
            } else if (stat == bcmCosqStatYellowDiscardDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
            } else if (stat == bcmCosqStatRedDiscardDroppedPackets) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
            } else { /* (stat == bcmCosqStatGreenDiscardDroppedPackets) */
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

            ctrl_info.instance = local_port;
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                         sync, startq, value));
            break;
        case bcmCosqStatIngressPortPGMinBytesPeak:
        case bcmCosqStatIngressPortPGMinBytesCurrent:
        case bcmCosqStatIngressPortPGSharedBytesPeak:
        case bcmCosqStatIngressPortPGSharedBytesCurrent:
        case bcmCosqStatIngressPortPGHeadroomBytesPeak:
        case bcmCosqStatIngressPortPGHeadroomBytesCurrent:
            if (cosq < -1) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if (stat == bcmCosqStatIngressPortPGMinBytesPeak) {
                ctr_reg = SOC_COUNTER_NON_DMA_PG_MIN_PEAK;
            } else if (stat == bcmCosqStatIngressPortPGMinBytesCurrent) {
                ctr_reg = SOC_COUNTER_NON_DMA_PG_MIN_CURRENT;
            } else if (stat == bcmCosqStatIngressPortPGSharedBytesPeak) {
                ctr_reg = SOC_COUNTER_NON_DMA_PG_SHARED_PEAK;
            } else if (stat == bcmCosqStatIngressPortPGSharedBytesCurrent) {
                ctr_reg = SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT;
            } else if (stat == bcmCosqStatIngressPortPGHeadroomBytesPeak) {
                ctr_reg = SOC_COUNTER_NON_DMA_PG_HDRM_PEAK;
            } else {
                /* (stat == bcmCosqStatIngressPortPGHeadroomBytesCurrent) */
                ctr_reg = SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_ingress_pg_get(unit, port, cosq,
                                             &start_hw_index, &end_hw_index));

            COMPILER_64_ZERO(sum);
            for (i = start_hw_index; i <= end_hw_index; i++) {
                ctrl_info.instance = local_port;
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                             sync, i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }

            COMPILER_64_UMUL_32(sum, _FB6_MMU_BYTES_PER_CELL);
            *value = sum;
            break;
        case bcmCosqStatIngressPortPoolSharedBytesPeak:
            if (cosq < -1) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_ingress_sp_get_by_pg(unit, port, cosq,
                                             &start_hw_index, &end_hw_index));

            BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
            ctrl_info.instance = local_port;
            ctr_reg = SOC_COUNTER_NON_DMA_POOL_PEAK;

            COMPILER_64_ZERO(sum);
            for (i = start_hw_index; i <= end_hw_index; i++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                             sync, i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }

            COMPILER_64_UMUL_32(sum, _FB6_MMU_BYTES_PER_CELL);
            *value = sum;
            break;

        case bcmCosqStatOBMHighWatermark:
        case bcmCosqStatOBMBufferBytes:
            if ((cosq < -1) || (cosq >=  si->port_num_cosq[local_port])) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                    BCM_GPORT_IS_SCHEDULER(port) ||
                    BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            if (stat == bcmCosqStatOBMHighWatermark) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_obmhighwatermark_get(unit, local_port, value));
            } else { /* stat == bcmCosqStatOBMBufferBytes */
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_obm_buf_use_cnt_get(unit, local_port, value));
            }
            break;

        case bcmCosqStatIngressPortPoolSharedBytesCurrent:
            if (cosq < -1) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_ingress_sp_get_by_pg(unit, port, cosq,
                                             &start_hw_index, &end_hw_index));
            BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
            ctrl_info.instance = local_port;
            ctr_reg = SOC_COUNTER_NON_DMA_POOL_CURRENT;

            COMPILER_64_ZERO(sum);
            for (i = start_hw_index; i <= end_hw_index; i++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                             sync, i, &value64));
               COMPILER_64_ADD_64(sum, value64);
            }

            COMPILER_64_UMUL_32(sum, _FB6_MMU_BYTES_PER_CELL);
            *value = sum;
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
        case bcmCosqStatOBMFlowControlEvents:
            if ((cosq < -1) || (cosq >=  si->port_num_cosq[local_port])) {
                return BCM_E_PARAM;
            }

            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            }
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
            } else if (stat == bcmCosqStatOBMLossless1DroppedBytes) {
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
            } else { /* (stat == bcmCosqStatOBMFlowControlEvents) */
                ctr_reg = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
            }
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            ctrl_info.instance = local_port;
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_get(unit, ctr_reg, ctrl_info, 0,
                                         0, value));
            break;
        case bcmCosqStatEgressUCQueuePortBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                             &local_port, &pool, NULL));
            base_mem = MMU_THDU_COUNTER_PORTm;
            mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, base_mem, 0, 0);
            mem_idx = SOC_TD3_MMU_PIPED_MEM_INDEX(unit, local_port, base_mem, pool);
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, mem_idx, entry));
            value32 = soc_mem_field32_get(unit, mem, entry, SHARED_COUNTf);

            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);
            break;

        case bcmCosqStatEgressPortPoolTotalBytesCurrent:
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_UNAVAIL;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_THDM_DB_PORT_TOTAL_COUNTr,
                               local_port, 0, &rval));
            value32 = soc_reg_field_get(unit, MMU_THDM_DB_PORT_TOTAL_COUNTr,
                    rval, COUNTf);
            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);
            break;

        case bcmCosqStatEgressPortPoolSharedBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                             &local_port, &pool, NULL));
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_THDM_DB_PORTSP_SHARED_COUNTr,
                               local_port, pool, &rval));
            value32 = soc_reg_field_get(unit, MMU_THDM_DB_PORTSP_SHARED_COUNTr,
                    rval, COUNTf);
            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);
            break;

        case bcmCosqStatEgressPoolSharedBytesCurrent:
            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                             _BCM_FB6_COSQ_INDEX_STYLE_EGR_POOL,
                                             &local_port, &pool, NULL));
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, MMU_THDM_DB_POOL_SHARED_COUNTr,
                               REG_PORT_ANY, pool, &rval));
            value32 = soc_reg_field_get(unit, MMU_THDM_DB_PORTSP_SHARED_COUNTr,
                    rval, COUNTf);
            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);
            break;

        case bcmCosqStatEgressQgroupMinBytesCurrent:
        case bcmCosqStatEgressQgroupBytesCurrent:
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, port, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 &local_port, &startq, NULL));
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                return BCM_E_PARAM;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));
                if (local_port < 0) {
                    return BCM_E_PORT;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, local_port, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                                 NULL, &startq, NULL));
            }
            base_mem = MMU_THDU_Q_TO_QGRP_MAPm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
            if (!soc_mem_field32_get(unit, mem, entry, QGROUP_VALIDf)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                        (BSL_META_U(unit,
                                    "UCQ doesn't associate with a queue group!\n")));
                return BCM_E_UNAVAIL;
            }

            base_mem = MMU_THDU_COUNTER_QGROUPm;
            mem = SOC_MEM_UNIQUE_ACC(unit, base_mem)[0];
            if (stat == bcmCosqStatEgressQgroupMinBytesCurrent) {
                field = MIN_COUNTf;
            } else if (stat == bcmCosqStatEgressQgroupBytesCurrent) {
                field = SHARED_COUNTf;
            }
            mem_idx = _soc_td3_mmu_port(unit, local_port) &  SOC_FB6_MMU_PORT_STRIDE;
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, mem_idx, entry));
            value32 = soc_mem_field32_get(unit, mem, entry, field);

            COMPILER_64_ADD_32(*value, value32);
            COMPILER_64_UMUL_32(*value, _FB6_MMU_BYTES_PER_CELL);
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

            BCM_IF_ERROR_RETURN
                (_bcm_fb6_cosq_localport_resolve(unit, port, &local_port));

            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
            ctrl_info.instance = local_port;
            ctr_reg = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
            BCM_IF_ERROR_RETURN
                (soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                         sync, startq, value));
            break;
        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_fb6_cosq_mapping_set
 * Purpose:
 *    Set COS queue for the specified priority of an ingress port
 * Parameters:
 *    unit     - (IN) unit number
 *    ing_port - (IN) ingress port
 *    priority - (IN) priority value to map
 *    flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *    gport    - (IN) queue group GPORT identifier
 *    cosq     - (IN) COS queue number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb6_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
        uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port, out_port;
    bcm_cos_queue_t hw_cosq;
    int rv, idx;
    int port = 0;
    soc_field_t field[2] = {INVALIDf, INVALIDf}; /* [0]:UC, [1]:MC cos */
    void *entries[1];
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[_FB6_MMU_NUM_INT_PRI];
    uint32 old_index, new_index;

    soc_info_t *si;
    si = &SOC_INFO(unit);

    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, ing_port)){
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_subport_resolve(unit, ing_port, &local_port));
            local_port -= 4;
    } else {

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, ing_port, &local_port));
    }
    if (gport != -1) {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_localport_resolve(unit, gport,
                    &out_port));
    }

    switch(flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            if (gport == -1) {
                hw_cosq = cosq;
            } else {
                if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_COS, &port, &hw_cosq, NULL));
            }
            if (!IS_CPU_PORT(unit, port)) {
                hw_cosq = hw_cosq - si->port_uc_cosq_base[port];
            }
            field[0] = UC_COS1f;
            break;
        case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
            if (gport == -1) {
                hw_cosq = cosq;
            } else {
                if (!BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN
                    (_bcm_fb6_cosq_index_resolve(unit, gport, cosq,
                                                 _BCM_FB6_COSQ_INDEX_STYLE_COS, &port, &hw_cosq, NULL));
            }
            if (!IS_CPU_PORT(unit, port)) {
                hw_cosq = hw_cosq - si->port_cosq_base[port];
            }
            field[0] = MC_COS1f;
            break;
            case (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP):
                if (gport == -1) {
                    hw_cosq = cosq;
                } else {
                    return BCM_E_PARAM;
                }
            hw_cosq %= SOC_FB6_NUM_SCHEDULER_PER_PORT;
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
    old_index *= _FB6_MMU_NUM_INT_PRI;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_fb6_cos_map_profile[unit], old_index,
                             _FB6_MMU_NUM_INT_PRI, entries));

    for (idx = 0; idx < 2; idx++) {
        if (field[idx] != INVALIDf) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                    field[idx], hw_cosq);
        }
    }

    soc_mem_lock(unit, PORT_COS_MAPm);
    rv = soc_profile_mem_delete(unit, _bcm_fb6_cos_map_profile[unit],
            old_index);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_fb6_cos_map_profile[unit], entries,
            _FB6_MMU_NUM_INT_PRI, &new_index);

    soc_mem_unlock(unit, PORT_COS_MAPm);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_mem_field32_set(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf,
            new_index / _FB6_MMU_NUM_INT_PRI);

    BCM_IF_ERROR_RETURN
        (WRITE_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                            &cos_map_sel_entry));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (IS_CPU_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    new_index / _FB6_MMU_NUM_INT_PRI));
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_fb6_cosq_mapping_get
 * Purpose:
 *   Determine which COS queue a given priority currently maps to.
 * Parameters:
 *   unit     - (IN) unit number
 *   ing_port - (IN) ingress port
 *   priority - (IN) priority value to map
 *   flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *   gport    - (IN) queue group GPORT identifier
 *   cosq     - (OUT) COS queue number
 * Returns:
 *   BCM_E_XXX
 */
 STATIC int
_bcm_fb6_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
        uint32 flags, bcm_gport_t *gport,
        bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    cos_map_sel_entry_t cos_map_sel_entry;
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_cpu_port_info_t *cpu_port_info = NULL;
    _bcm_fb6_cosq_coe_port_info_t *coeport_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    _bcm_fb6_cosq_node_t *node = NULL;
    int ii, index;
    void *entry_p;
    bcm_cos_queue_t hw_cosq = BCM_COS_INVALID, node_cosq = BCM_COS_INVALID;
    bcm_cos_queue_t new_cosq = BCM_COS_INVALID;
    int numq = -1;


    soc_info_t *si = &SOC_INFO(unit);

    if ((flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) &&
            (flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)) {
        return BCM_E_PARAM;
    }

    if(_BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, ing_port)){
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_subport_resolve(unit, ing_port, &local_port));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, ing_port, &local_port));
    }
    if (gport) {
        *gport = BCM_GPORT_INVALID;
    }

    *cosq = BCM_COS_INVALID;

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
            SELECTf);
    index *= _FB6_MMU_NUM_INT_PRI;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_fb6_cos_map_profile[unit],
            port_cos_map_entry_t *, index + priority);

    if (IS_CPU_PORT(unit, local_port)) {
        cpu_port_info = &_bcm_fb6_cosq_cpu_port_info[unit][local_port];
        numq = SOC_FB6_NUM_CPU_QUEUES;
    } else if(local_port > _BCM_FB6_NUM_PORTS) {
        coeport_info = &_bcm_fb6_cosq_coe_port_info[unit][local_port];
        numq = SOC_FB6_NUM_UCAST_QUEUE_PER_PORT;
    } else if(IS_STK_PORT(unit, local_port)){
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][local_port];
        numq = SOC_FB6_NUM_MCAST_QUEUE_PER_ST_PORT;
    } else if(IS_UPLINK_PORT(unit, local_port)){
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][local_port];
        numq = SOC_FB6_NUM_MCAST_QUEUE_PER_HG_PORT;
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][local_port];
        numq = SOC_FB6_NUM_UCAST_QUEUE_PER_PORT;
    }

    switch (flags) {
        case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
            hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p,
                    UC_COS1f);
            if ((IS_CPU_PORT(unit, local_port) == FALSE) && (gport)) {
                for (ii = 0; ii < numq; ii++) {
                    if (local_port > _BCM_FB6_NUM_PORTS) {
                        node = &coeport_info->ucast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - (local_port * 8);
                        }
                    } else if(IS_STK_PORT(unit, local_port)){
                        node = &stport_info->ucast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_uc_cosq_base[local_port];
                        }
                    } else if(IS_UPLINK_PORT(unit, local_port)){
                        node = &hgport_info->ucast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_uc_cosq_base[local_port];
                        }
                    } else {
                        node = &port_info->ucast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_uc_cosq_base[local_port];
                        }
                    }
                    if (node == NULL) {
                        return BCM_E_NOT_FOUND;
                    }
                    if( !IS_STK_PORT(unit, local_port) && !IS_UPLINK_PORT(unit, local_port)) {
                        new_cosq = (hw_cosq > 7)  ? hw_cosq % 8 : hw_cosq;
                    } else {
                        new_cosq = hw_cosq;
                    }
                    if (node && (node_cosq == new_cosq)) {
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
                for (ii = 0; ii < numq; ii++) {
                    if (IS_CPU_PORT(unit, local_port)) {
                        node = &cpu_port_info->mcast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - _FB6_CPU_L2_NODE_BASE;
                        }
                    } else if (local_port > _BCM_FB6_NUM_PORTS) {
                        node = &coeport_info->mcast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - (local_port * 8);
                        }
                    } else if (IS_STK_PORT(unit, local_port)) {
                        node = &stport_info->mcast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_cosq_base[local_port];
                        }
                    } else if (IS_UPLINK_PORT(unit, local_port)) {
                        node = &hgport_info->mcast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_cosq_base[local_port];
                        }
                    } else {
                        node = &port_info->mcast[ii];
                        if(node != NULL) {
                            node_cosq = node->hw_index - si->port_cosq_base[local_port];
                        }
                    }
                    if (node == NULL) {
                        return BCM_E_NOT_FOUND;
                    }
                    if( !IS_STK_PORT(unit, local_port) && !IS_UPLINK_PORT(unit, local_port)) {
                        new_cosq = (hw_cosq > 7)  ? hw_cosq % 8 : hw_cosq;
                    } else {
                        new_cosq = hw_cosq;
                    }
                    if (node && (node_cosq == new_cosq)) {
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
             *This default is unreachable. It is kept intentionally
             *as a defensive default for future development.
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
 *    bcm_fb6_cosq_mapping_set
 * Purpose:
 *    Set which cosq a given priority should fall into
 * Parameters:
 *    unit     - (IN) unit number
 *    gport    - (IN) queue group GPORT identifier
 *    priority - (IN) priority value to map
 *    cosq     - (INT) COS queue number
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_fb6_cosq_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
        bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;

    BCM_PBMP_CLEAR(pbmp);

    if ((priority < 0) || (priority >= _FB6_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    if (port == -1) {
        /* port value of -1 will cause the cosq mapping set for
         * all types of local ports except Loopback ports.
         * So we do the sanity check of cos value with Default Number
         * of COS
 */
        if (cosq >= _FB6_NUM_COS(unit)) {
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
        if (cosq >= _FB6_PORT_NUM_COS(unit, local_port)) {
            return BCM_E_PARAM;
        }
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         *          * mcast */
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_mapping_set(unit, local_port, priority,
                                       BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                       BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                       -1, cosq));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcm_fb6_cosq_gport_mapping_set
 * Purpose:
 *    Set COS queue for the specified priority of an ingress port
 * Parameters:
 *    unit     - (IN) unit number
 *    ing_port - (IN) ingress port
 *    int_pri  - (IN) priority value to map
 *    flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *    gport    - (IN) queue group GPORT identifier
 *    cosq     - (IN) COS queue number
 *    Returns:
 * BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t int_pri,
        uint32 flags, bcm_gport_t gport,
        bcm_cos_queue_t cosq)
{

    if ((int_pri < 0) || (int_pri >= _FB6_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    if ((cosq < 0) || (cosq >= SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT)) {
        return BCM_E_PARAM;
    }

    return _bcm_fb6_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
            cosq);
}

/*
 * Function:
 *      bcm_fb6_cosq_mapping_get
 * Purpose:
 *      Determine which COS queue a given priority currently maps to.
 * Parameters:
 *      unit     - (IN) unit number
 *      gport    - (IN) queue group GPORT identifier
 *      priority - (IN) priority value to map
 *      cosq     - (OUT) COS queue number
 *Returns:
 *BCM_E_XXX
 */
int
bcm_fb6_cosq_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
        bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);

    if ((priority < 0) || (priority >= _FB6_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

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
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_mapping_get(unit, local_port, priority,
                                       BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL,
                                       cosq));
        break;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     _bcm_fb6_cosq_gport_mapping_get
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     ing_port - (IN) ingress port
 *     int_pri  - (IN) priority value to map
 *     flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *     gport    - (IN) queue group GPORT identifier
 *     cosq     - (OUT) COS queue number
 *Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_cosq_gport_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t int_pri,
        uint32 flags, bcm_gport_t *gport,
        bcm_cos_queue_t *cosq)
{
    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    if ((int_pri < 0) || (int_pri >= _FB6_MMU_NUM_INT_PRI)) {
        return BCM_E_PARAM;
    }

    return _bcm_fb6_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
            cosq);
}


/*
* bcm_fb6_cosq_cpu_cosq_enable_get
* Purpose:
*    To get enable/disable status on Rx of packets on the specified CPU cosq.
* Parameters:
*    unit    - (IN) unit number
*    cosq    - (IN) CPU Cosq ID
*    enable  - (IN) Enable/Disable
* Returns:
*      BCM_E_XXX
 */
int
bcm_fb6_cosq_cpu_cosq_enable_get(int unit, bcm_cos_queue_t cosq, int *enable)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, hw_enable;
    _bcm_fb6_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    soc_mem_t thdm_mem = MMU_THDM_DB_QUEUE_CONFIG_PIPE0m;
    soc_field_t min_limit, shared_limit, limit_enable, dynamic_enable;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_fb6_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }

    /* CPU Port (0) - MC cosq's range from 3690 - 3738. */
    index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;
    hw_enable = 1;

    /* Sync up software record with hardware status. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, thdm_mem, MEM_BLOCK_ALL, index,
                &entry));
    min_limit = soc_mem_field32_get(unit, thdm_mem, &entry, Q_MIN_LIMITf);
    shared_limit = soc_mem_field32_get(unit, thdm_mem, &entry, Q_SHARED_LIMITf);
    limit_enable = soc_mem_field32_get(unit, thdm_mem, &entry, Q_LIMIT_ENABLEf);
    dynamic_enable = soc_mem_field32_get(unit, thdm_mem, &entry, Q_LIMIT_DYNAMICf);

    if ((limit_enable != 0) && (dynamic_enable == 0) &&
            (min_limit == 0) && (shared_limit == 0)) {
        hw_enable = 0;
    }
    cpu_cosq->enable = hw_enable;

    *enable = cpu_cosq->enable;
    return BCM_E_NONE;

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcm_fb6_cosq_cpu_cosq_enable_set
 * Purpose:
 *   To enable/disable Rx of packets on the specified CPU cosq.
 * Parameters:
 *   unit    - (IN) unit number
 *   cosq    - (IN) CPU Cosq ID
 *   enable  - (IN) Enable/Disable
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_fb6_cosq_cpu_cosq_enable_set(int unit, bcm_cos_queue_t cosq, int enable)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int i, index, enable_status;
    _bcm_fb6_cosq_cpu_cosq_config_t *cpu_cosq_config;
    soc_info_t *si;
    soc_mem_t thdm_mem[] = {
        MMU_THDM_DB_QUEUE_CONFIG_PIPE0m,
        MMU_THDM_MCQE_QUEUE_CONFIG_PIPE0m
    };

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= NUM_CPU_COSQ(unit))) {
        return BCM_E_PARAM;
    }

    cpu_cosq_config = _bcm_fb6_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq_config) {
        return BCM_E_INTERNAL;
    }

    BCM_IF_ERROR_RETURN(bcm_fb6_cosq_cpu_cosq_enable_get(unit, cosq,
                &enable_status));
    if (enable == enable_status) {
        return BCM_E_NONE;
    }

    /* CPU Port (0) - MC cosq's range from 3690 - 3738. */
    index = si->port_cosq_base[CMIC_PORT(unit)] + cosq;

    /* To set DB and QE entries */
    for (i = 0; i < 2; i++) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, thdm_mem[i], MEM_BLOCK_ALL, index, &entry));

        /* When enabled, restore the Min and Shared from the cached value */
        if (!enable) {
            cpu_cosq_config->q_min_limit[i] = soc_mem_field32_get(unit,
                    thdm_mem[i], &entry,
                    Q_MIN_LIMITf);
            cpu_cosq_config->q_shared_limit[i] = soc_mem_field32_get(unit,
                    thdm_mem[i], &entry,
                    Q_SHARED_LIMITf);
            cpu_cosq_config->q_limit_dynamic[i] = soc_mem_field32_get(unit,
                    thdm_mem[i], &entry,
                    Q_LIMIT_DYNAMICf);
            cpu_cosq_config->q_limit_enable[i] = soc_mem_field32_get(unit,
                    thdm_mem[i], &entry,
                    Q_LIMIT_ENABLEf);
            cpu_cosq_config->q_color_limit_enable[i] = soc_mem_field32_get(unit,
                    thdm_mem[i], &entry,
                    Q_COLOR_LIMIT_ENABLEf);
        }
        soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_LIMIT_DYNAMICf,
                enable ? cpu_cosq_config->q_limit_dynamic[i] : 0);
        soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_LIMIT_ENABLEf,
                enable ? cpu_cosq_config->q_limit_enable[i] : 1);
        soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_COLOR_LIMIT_ENABLEf,
                enable ? cpu_cosq_config->q_color_limit_enable[i] :1);
        soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_MIN_LIMITf,
                enable ? cpu_cosq_config->q_min_limit[i] : 0);
        soc_mem_field32_set(unit, thdm_mem[i], &entry, Q_SHARED_LIMITf,
                enable ? cpu_cosq_config->q_shared_limit[i] : 0);
        cpu_cosq_config->enable = enable;
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, thdm_mem[i], MEM_BLOCK_ALL, index, &entry));
    }

    return BCM_E_NONE;
}

/*
 *bcm_fb6_cosq_port_attach
 *     Purpose:
 *Initialize Cosq for newly added port
 *     Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) Logical port.
 *Returns:
 *    BCM_E_XXX
 */

int
bcm_fb6_cosq_port_attach(int unit, bcm_port_t port)
{
    int default_mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN, default_weight = 1;
    uint64 rval64s[16], *prval64s[1];
    uint32 profile_index;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
            IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Increase the reference of default PRIO2COS_PROFILE */
    if (!IS_COE_PORT(unit, port)) {
        sal_memset(rval64s, 0, sizeof(rval64s));
        prval64s[0] = rval64s;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_add(unit, _bcm_fb6_prio2cos_profile[unit],
                                 prval64s, 16, &profile_index));
    }

    /* Create gport tree */
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = FALSE;
    if (IS_STK_PORT(unit, port)){
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_st_gport_tree_create(unit, port));
    } else if (IS_UPLINK_PORT(unit, port)){
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_hg_gport_tree_create(unit, port));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_gport_tree_create(unit, port));
    }
    _BCM_FB6_MMU_INFO(unit)->gport_tree_created = TRUE;

    /* Set Default Scheduler Configuration on the ports */
    if (!IS_COE_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_sched_config_port(unit, port, default_mode,
                                             default_weight));
    }
    return BCM_E_NONE;
}

/*
 *Function:
 *   bcm_fb6_cosq_port_detach
 *Purpose:
 *   Clear cosq configuration of port
 *Parameters:
 *   unit    - (IN) unit number
 *   port    - (IN) Logical port.
 *Returns:
 *  BCM_E_XXX
 */

int
bcm_fb6_cosq_port_detach(int unit, bcm_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    _bcm_fb6_cosq_port_info_t *port_info = NULL;
    _bcm_fb6_cosq_st_port_info_t *stport_info = NULL;
    _bcm_fb6_cosq_hg_port_info_t *hgport_info = NULL;
    uint32 profile_index;
    int index;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
            IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_PARAM;
    }

    /* Destroy cosq gport tree on port */
    if (IS_STK_PORT(unit, port)) {
        stport_info = &_bcm_fb6_cosq_st_port_info[unit][port];
        sal_memset(stport_info, 0, sizeof(_bcm_fb6_cosq_st_port_info_t));
    } else if (IS_UPLINK_PORT(unit, port)) {
        hgport_info = &_bcm_fb6_cosq_hg_port_info[unit][port];
        sal_memset(hgport_info, 0, sizeof(_bcm_fb6_cosq_hg_port_info_t));
    } else {
        port_info = &_bcm_fb6_cosq_port_info[unit][port];
        sal_memset(port_info, 0, sizeof(_bcm_fb6_cosq_port_info_t));
    }

    if (!IS_COE_PORT(unit, port)) {
    /* Decrease the reference of defalt PRIO2COS_PROFILE */
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_pri2cos_profile_setget(unit, port, -1, &index, 0));
        profile_index = index * 16;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, _bcm_fb6_prio2cos_profile[unit],
                                    profile_index));

    }
    /* Clear cosq */
    si->port_num_cosq[port] = 0;
    si->port_cosq_base[port] = 0;
    si->port_num_uc_cosq[port] = 0;
    si->port_uc_cosq_base[port] = 0;
    si->port_num_ext_cosq[port] = 0;

    return BCM_E_NONE;
}

/* Function:
 *      bcmi_fb6_port_cosmap_update
 *  Purpose:
 *     called during flexport during port attach/detach
 *     to update the cos_map profile for the port
 *  Parameters:
 *      unit   - unit number
 *      port   - port number
 *      enable - add/delete
 *  Returns:
 *      BCM_E_XXX
 */

int
bcmi_fb6_port_cosmap_update(int unit, pbmp_t pbm, int enable)
{
    port_cos_map_entry_t cos_map_entries[16];
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entries[1];
    uint32 index, old_index;
    bcm_port_t port;
    int cos, prio, numq, rv;
    soc_mem_t mem = PORT_COS_MAPm;
    uint32 i;

    numq = _BCM_FB6_NUM_COS(unit);

    SOC_PBMP_ITER(pbm, port) {
        prio = 0;
        sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
        entries[0] = &cos_map_entries;

        if (enable) {
            /*
             * Distribute first 8 internal priority levels into the specified number
             * of cosq, map remaining internal priority levels to highest priority
             * cosq
 */
            sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
            entries[0] = &cos_map_entries;

            for (cos = 0; cos < numq; cos++) {
                for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
                    soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            UC_COS1f, cos);
                    soc_mem_field32_set(unit, mem, &cos_map_entries[prio],
                            MC_COS1f, cos);
                    prio++;
                }
            }
            for (prio = 8; prio < 16; prio++) {
                soc_mem_field32_set(unit, mem, &cos_map_entries[prio], UC_COS1f,
                        numq - 1);
                soc_mem_field32_set(unit, mem, &cos_map_entries[prio], MC_COS1f,
                        numq - 1);
            }

            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_fb6_cos_map_profile[unit], entries, 16,
                                     &index));
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        index / 16));

        } else {
            BCM_IF_ERROR_RETURN
                (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, port, &cos_map_sel_entry));
            old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                    SELECTf);
            old_index *= 16;

            rv = soc_profile_mem_delete(unit, _bcm_fb6_cos_map_profile[unit],
                    old_index);

            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
    return BCM_E_NONE;
}

int
bcm_fb6_cosq_gport_info_get(int unit, bcm_gport_t gport,
        bcm_cosq_gport_level_info_t *info)
{

    int local_port;
    _bcm_fb6_cosq_node_t *node = NULL; 
    int type;

    if( NULL == info ) {
        return BCM_E_PARAM;
    }

    if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) &&
            !BCM_GPORT_IS_SCHEDULER(gport) &&
            !BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_fb6_cosq_node_get(unit, gport, NULL, 
                                &local_port, NULL, &node));


    if (IS_STK_PORT(unit, local_port)) {
        type = BCM_COSQ_STACKING_GPORT;
    } else if (IS_UPLINK_PORT(unit, local_port)) {
        type =  BCM_COSQ_UPLINK_GPORT;
    } else {
        type =  BCM_COSQ_DOWNLINK_GPORT;
    }
    info->parent_port_type = type;
    info->level = node->level;

    return BCM_E_NONE;
}

int bcm_fb6_subport_speed_reverse_map_get(int unit,  int *class, int *speed)
{

     switch(*class) {
         case 0:
              *speed = 10;
               break;
         case 1:
              *speed = 100;
               break;
         case 2:
              *speed = 1000;
               break;
         case 3:
              *speed = 2500;
               break;
         case 4:
              *speed = 5000;
               break;
         case 5:
              *speed = 10000;
               break;
         default :
              *speed = -1;
              return BCM_E_FAIL;
     }
     return BCM_E_NONE;

}
int bcm_fb6_subport_speed_map_get(int unit, int *speed, int *class)
{
     switch(*speed) {
         case 10:
              *class =0;
               break;
         case 100:
              *class =1;
               break;
         case 1000:
              *class =2;
               break;
         case 2500:
              *class =3;
               break;
         case 5000:
              *class =4;
               break;
         case 10000:
              *class =5;
               break;
         default :
              *class = -1;
              return BCM_E_FAIL;
     }
     return BCM_E_NONE;
}

/*
 * Function :
 *        bcm_fb6_cosq_subport_congestion_config_set
 * Purpose:
 *        To enable the reception of PAUSE/PFC/E2ECC on the subport level.
 * Parameters :
 *        unit   -   (IN) Unit number.
 *        gport  -   (IN) Subport gport.
 *        type   -   (IN) Congestion/Flow control type.
 *        config -   (IN) bcm_cosq_subport_congestion_config_t.
 * Returns :
 *        BCM_E_XXX
 */

int
bcm_fb6_cosq_subport_congestion_config_set(int unit, bcm_gport_t gport,
                                            bcm_cosq_subport_congestion_type_t congestion_type,
                                            bcm_cosq_subport_congestion_config_t *congestion_config)
{
    int speed_class, index;
    int subport_num = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry1[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = SRC_SUBPORT_FC_CONFIG_SC0m;
    soc_mem_t mem1 = SUBPORT_TIMER_TBL_SC0m;

    if(!BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        return BCM_E_PARAM;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    sal_memset(entry1, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);

    BCM_IF_ERROR_RETURN(_bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

    index = FB6_SUBPORT_INDEX_GET(unit, subport_num);

    switch (congestion_type) {
        case bcmCosqSubportCongestionTypePause:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

            if(congestion_config->status) {
                if (congestion_config->subport_speed <= 0 || congestion_config->subport_speed > 10000) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(bcm_fb6_subport_speed_map_get(unit, &congestion_config->subport_speed, &speed_class));
                soc_mem_field32_set(unit, mem, &entry, FC_ENABLEf, 1);
                soc_mem_field32_set(unit, mem, &entry, FC_TYPEf, 0);
                soc_mem_field32_set(unit, mem, &entry, SPEEDf, speed_class);
            } else {
                soc_mem_field32_set(unit, mem, &entry, FC_ENABLEf, 0);
                soc_mem_field32_set(unit, mem, &entry, SPEEDf, 0);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL, index, &entry1));
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
            break;

        case bcmCosqSubportCongestionTypePfc:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

            if(congestion_config->status) {
                if (congestion_config->subport_speed <= 0 || congestion_config->subport_speed > 10000) {
                    return BCM_E_PARAM;
                }
                BCM_IF_ERROR_RETURN(bcm_fb6_subport_speed_map_get(unit, &congestion_config->subport_speed, &speed_class));
                soc_mem_field32_set(unit, mem, &entry, FC_ENABLEf, 1);
                soc_mem_field32_set(unit, mem, &entry, FC_TYPEf, 1);
                soc_mem_field32_set(unit, mem, &entry, SPEEDf, speed_class);
                soc_mem_field32_set(unit, mem, &entry, PROFILE_INDEXf, congestion_config->cos_to_pfc_map_profile_id);
            } else {
                soc_mem_field32_set(unit, mem, &entry, FC_ENABLEf, 0);
                soc_mem_field32_set(unit, mem, &entry, SPEEDf, 0);
                soc_mem_field32_set(unit, mem, &entry, FC_TYPEf, 0);
                soc_mem_field32_set(unit, mem, &entry, PROFILE_INDEXf, 0);
                BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem1, MEM_BLOCK_ALL, index, &entry1));
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
            break;

        case bcmCosqSubportCongestionTypeE2ecc:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

            if(congestion_config->status) {
                soc_mem_field32_set(unit, mem, &entry, E2ECC_ENABLEf, 1);
            } else {
                soc_mem_field32_set(unit, mem, &entry, E2ECC_ENABLEf, 0);
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
            break;
        default :
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function :
 *        bcm_fb6_cosq_subport_congestion_config_get
 * Purpose:
 *        To get the congestion congfuration on the subport level.
 * Parameters :
 *        unit   -   (IN) Unit number.
 *        gport  -   (IN) Subport gport.
 *        type   -   (IN) Congestion/Flow control type.
 *        config -   (OUT) bcm_cosq_subport_congestion_config_t *.
 * Returns :
 *        BCM_E_XXX
 */

int
bcm_fb6_cosq_subport_congestion_config_get(int unit, bcm_gport_t gport,
                                            bcm_cosq_subport_congestion_type_t congestion_type,
                                            bcm_cosq_subport_congestion_config_t *congestion_config)
{
    int speed, index;
    int subport_num = 0;
    int fc_enable = 0;
    int fc_type = 0;
    int e2ecc = 0;
    int profile_index = -1;
    int class = -1;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = SRC_SUBPORT_FC_CONFIG_SC0m;

   if (congestion_config == NULL) {
        return BCM_E_PARAM;
    }

    if(!BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        return BCM_E_PARAM;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);


    BCM_IF_ERROR_RETURN(_bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

    index = FB6_SUBPORT_INDEX_GET(unit, subport_num);

    switch (congestion_type) {
        case bcmCosqSubportCongestionTypePause:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

            fc_enable = soc_mem_field32_get(unit, mem, &entry, FC_ENABLEf);
            fc_type = soc_mem_field32_get(unit, mem, &entry, FC_TYPEf);
            class = soc_mem_field32_get(unit, mem, &entry, SPEEDf);
            if (fc_enable == 1 && fc_type == 0) {
                BCM_IF_ERROR_RETURN(bcm_fb6_subport_speed_reverse_map_get(unit, &class, &speed));
                congestion_config->status = 1;
                congestion_config->subport_speed = speed;
            }
            break;

        case bcmCosqSubportCongestionTypePfc:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            fc_enable = soc_mem_field32_get(unit, mem, &entry, FC_ENABLEf);
            fc_type = soc_mem_field32_get(unit, mem, &entry, FC_TYPEf);
            class = soc_mem_field32_get(unit, mem, &entry, SPEEDf);
            profile_index = soc_mem_field32_get(unit, mem, &entry, PROFILE_INDEXf);
            if (fc_enable == 1 && fc_type == 1) {
                BCM_IF_ERROR_RETURN(bcm_fb6_subport_speed_reverse_map_get(unit, &class, &speed));
                congestion_config->status = 1;
                congestion_config->subport_speed = speed;
                congestion_config->cos_to_pfc_map_profile_id = profile_index;
            }
            break;

        case bcmCosqSubportCongestionTypeE2ecc:

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            e2ecc = soc_mem_field32_get(unit, mem, &entry, E2ECC_ENABLEf);
            if (e2ecc == 1) {
                congestion_config->status = 1;
            }
            break;
        default :
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
* Function :
*        bcm_fb6_cosq_subport_qgroup_update
* Purpose:
*         Update the queue group num b/w lw and high priority queues.
* Parameters :
*        unit   -   (IN) Unit number.
*        subport_num  -   (IN) Subport number.
*        cos_to_pfc_map -   (IN) bcm_subport_cos_to_pfc_pri_map_t .
* Returns :
*        BCM_E_XXX
*/

int bcm_fb6_cosq_subport_qgroup_update(int unit, int subport_num, bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map)
{
    int low=-1, high=-1, idx=0;
    int i, j;
    soc_mem_t mem = MMU_THDU_Q_TO_QGRP_MAP_PIPE0m;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index = (subport_num + 4) * FB6_SUBPORT_COS_COUNT;


    /*Get the current qgroup num for low and  high priority queues */
    for(idx = index; idx < (index + FB6_SUBPORT_COS_COUNT); idx++) {
        sal_memset(entry, 0, sizeof(mmu_thdu_config_queue_entry_t));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &entry));
        i = soc_mem_field32_get(unit, mem, &entry, QGROUPf);
        if( idx == index) {
            low = i;
        }
        if (i != low) {
            high = i;
        }
    }
    if (high == -1) {
        high = low + 1;
    }
    /* Update the group num for low and high priority queeus */
    for ( i = 0; i < FB6_SUBPORT_PFC_CLASS; i++) {
        for(idx = index, j = 0; idx < (index + FB6_SUBPORT_COS_COUNT) && j < FB6_SUBPORT_COS_COUNT; idx++, j++) {
            sal_memset(entry, 0, sizeof(mmu_thdu_config_queue_entry_t));
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &entry));
            if ((i == 0) && (cos_to_pfc_map->cos_to_pfc_pri_low[j] == 1)) {
                soc_mem_field32_set(unit, mem, entry, QGROUPf, low);
            } else if((i == 1) && (cos_to_pfc_map->cos_to_pfc_pri_high[j] == 1)){
                soc_mem_field32_set(unit, mem, entry, QGROUPf, high);
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, entry));
        }

    }
    return BCM_E_NONE;
}
/*
* Function :
*        bcm_fb6_cosq_subport_pfc_class_mapping_set
* Purpose:
*         Set mapping of COSQs L1s to PFC class (LO and HIGH).
* Parameters :
*        unit   -   (IN) Unit number.
*        gport  -   (IN) Subport gport.
*        cos_to_pfc_map -   (IN) bcm_subport_cos_to_pfc_pri_map_t .
*        cos_to_pfc_map_profile_id -   (OUT) int *.
* Returns :
*        BCM_E_XXX
 */

int bcm_fb6_cosq_subport_pfc_class_mapping_set(int unit, bcm_gport_t gport,
                                               bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map,
                                               int *cos_to_pfc_map_profile_id)
{

    int subport_num;
    uint32 profile_index, old_profile_index, index;
    uint32 l0_bmp;
    uint64 rval64[2], *rval64s[1];
    soc_field_t field = INVALIDf;
    int rv = BCM_E_NONE;
    int i, j;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = SRC_SUBPORT_FC_CONFIG_SC0m;
    field = L0_COS_BMPf;

    if (cos_to_pfc_map == NULL || cos_to_pfc_map_profile_id == NULL) {
        return BCM_E_PARAM;
    }

    if(!BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        return BCM_E_PARAM;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);


    BCM_IF_ERROR_RETURN(_bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

    index = FB6_SUBPORT_INDEX_GET(unit, subport_num);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

    old_profile_index = 2 * soc_mem_field32_get(unit, mem, &entry, PROFILE_INDEXf);

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_chprio2cos_profile[unit],
                            old_profile_index, 2, rval64s));

    for(i = 0; i < FB6_SUBPORT_PFC_CLASS; i++) {
        l0_bmp = 0x0;
        for(j = 0; j < FB6_SUBPORT_COS_COUNT; j++) {
            if(i) {
                if(cos_to_pfc_map->cos_to_pfc_pri_low[j] == 1) {
                    l0_bmp |= (1<<j);
                }
            } else {
                if(cos_to_pfc_map->cos_to_pfc_pri_high[j] == 1) {
                    l0_bmp |= (1<<j);
                }
            }
        }
        soc_reg64_field32_set(unit, CH_PRIO2COS_PROFILE_64r, &rval64[i], field, l0_bmp);
    }


    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_fb6_chprio2cos_profile[unit],
                                old_profile_index));
    rv = soc_profile_reg_add(unit, _bcm_fb6_chprio2cos_profile[unit], rval64s,
                              2, &profile_index);
    if (rv != BCM_E_NONE) {
        SOC_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, _bcm_fb6_chprio2cos_profile[unit],
                                       old_profile_index, 2));
        return rv;
    }
    BCM_IF_ERROR_RETURN(bcm_fb6_cosq_subport_qgroup_update(unit, subport_num, cos_to_pfc_map));
    soc_mem_field32_set(unit, mem, &entry, PROFILE_INDEXf, profile_index/2);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
    *cos_to_pfc_map_profile_id = profile_index / 2;
    return BCM_E_NONE;
}

/*
* Function :
*        bcm_fb6_cosq_subport_pfc_class_mapping_get
* Purpose:
*        Get mapping of COSQs L1s to PFC class (LO and HIGH).
* Parameters :
*        unit   -   (IN) Unit number.
*        gport  -   (IN) Subport gport.
*        cos_to_pfc_map -   (OUT) bcm_subport_cos_to_pfc_pri_map_t .
* Returns :
*        BCM_E_XXX
 */
int bcm_fb6_cosq_subport_pfc_class_mapping_get(int unit, bcm_gport_t gport,
                                               bcm_subport_cos_to_pfc_pri_map_t *cos_to_pfc_map)
{
    int subport_num;
    uint32 profile_index, index;
    uint32 l0_bmp;
    uint64 rval64[2], *rval64s[1];
    int i, j;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = SRC_SUBPORT_FC_CONFIG_SC0m;

    if (cos_to_pfc_map == NULL) {
        return BCM_E_PARAM;
    }

    if(!BCM_GPORT_IS_SUBPORT_PORT(gport)) {
        return BCM_E_PARAM;
    }

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);


    BCM_IF_ERROR_RETURN(_bcm_coe_subtag_subport_port_subport_num_get(unit,
                gport, NULL, &subport_num));

    index = FB6_SUBPORT_INDEX_GET(unit, subport_num);
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));

    profile_index = 2 * soc_mem_field32_get(unit, mem, &entry, PROFILE_INDEXf);

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_chprio2cos_profile[unit],
                             profile_index, 2, rval64s));
    for(i = 0; i < FB6_SUBPORT_PFC_CLASS; i++) {
        l0_bmp = COMPILER_64_LO(rval64[i]);
        for(j = 0; j < FB6_SUBPORT_COS_COUNT; j++) {
            if ((i == 0) && (l0_bmp & (1 << j))) {
               cos_to_pfc_map->cos_to_pfc_pri_high[j] = 1;
            } else if((i == 1) && (l0_bmp & (1 << j))){
               cos_to_pfc_map->cos_to_pfc_pri_low[j] = 1;
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_cpu_port_info_dump(int unit)
{
    _bcm_fb6_cosq_cpu_port_info_t *port_info;
     bcm_port_t port = 0; /* CPU Port */
    _bcm_fb6_cosq_node_t *node;
    int index, new_index;
    soc_fb6_sched_mode_e sched_mode = 0;
    int weight = 0;

    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    port_info = _bcm_fb6_cosq_cpu_port_info[unit];
    if (port_info == NULL) {
        return BCM_E_INIT;
    }

    LOG_CLI((BSL_META_U(unit,
                        "=== CPU PORT ===\n")));

    LOG_CLI((BSL_META_U(unit,
                        "Nodes\n")));
    for (index = 0; index < SOC_FB6_NUM_SCHEDULER_PER_CPU_PORT; index++) {
        /* Node */
        node = &port_info->sched[index];
        new_index = (index >= 1) ? index - 1 : index;
        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, new_index,
                    &sched_mode, &weight, 0, 0));
        LOG_CLI((BSL_META_U(unit,
                        "L%d.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                    node->level - 1, new_index, node->gport, node->hw_index,
                    sched_modes[sched_mode], weight));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nMC Queues\n")));
    for (index = 0; index < SOC_FB6_NUM_CPU_QUEUES; index++) {
        int parent_id = 0;
        /* MC Queue */
        node = &port_info->mcast[index];
        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, index,
                                                &sched_mode, &weight, 1, 0));
        BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_node_get(unit, node->parent_gport, NULL, NULL,
                                   &parent_id, NULL));
        LOG_CLI((BSL_META_U(unit,
                            "    MC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d"
                            " PARENT=L1.%d\n"),
                             index, node->gport, node->hw_index,
                             sched_modes[sched_mode], weight,
                             (parent_id - 1)));
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}



STATIC int
_bcm_fb6_cosq_uplink_port_info_dump(int unit, bcm_port_t port)
{
    _bcm_fb6_cosq_hg_port_info_t *port_info;
    _bcm_fb6_cosq_node_t *node;
    int index, empty;
    int l1index = 0, numL0s = 0;
    int l1_max = 0;
    soc_fb6_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};



    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    port_info =  &_bcm_fb6_cosq_hg_port_info[unit][port];
    numL0s = 4;

    empty = TRUE;
    for (index = 0; index < SOC_FB6_NUM_SCHEDULER_PER_HG_PORT; index++) {
        if (port_info->sched[index].numq > 0) {
            empty = FALSE;
            break;
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT; index++) {
            if (port_info->ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_HG_PORT; index++) {
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

    index = 0;

    for (index = 0; index < numL0s; index++) {
        /* Node */

        node = &port_info->sched[index];

        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, index,
                    &sched_mode, &weight, 0, 0));
        LOG_CLI((BSL_META_U(unit,
                        "L0.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                    index, node->gport, node->hw_index,
                    sched_modes[sched_mode], weight));

        l1index = (index * 8) + numL0s;
        l1_max = l1index + 8;

        for(; l1index < l1_max; l1index++) {
            node = &port_info->sched[l1index];
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                        &sched_mode, &weight, 0, 0));
            LOG_CLI((BSL_META_U(unit,
                            "    L1.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                        l1index - numL0s, node->gport, node->hw_index,
                        sched_modes[sched_mode], weight));

                /* UC Queue */
                node = &port_info->ucast[l1index - numL0s];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                            &sched_mode, &weight, 0, 0));
                LOG_CLI((BSL_META_U(unit,
                                "           UC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - numL0s, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));

                /* MC Queue */
                node = &port_info->mcast[l1index - numL0s];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                            &sched_mode, &weight, 1, 0));
                LOG_CLI((BSL_META_U(unit,
                                "           MC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - numL0s, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));
            }
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_st_port_info_dump(int unit, bcm_port_t port)
{
    _bcm_fb6_cosq_st_port_info_t *port_info;
    _bcm_fb6_cosq_node_t *node;
    int index, empty;
    int l1index = 0, numL0s = 0;
    int l1_max = 0;
    soc_fb6_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};



    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    port_info =  &_bcm_fb6_cosq_st_port_info[unit][port];
    numL0s = 3;

    empty = TRUE;
    for (index = 0; index < SOC_FB6_NUM_SCHEDULER_PER_ST_PORT; index++) {
        if (port_info->sched[index].numq > 0) {
            empty = FALSE;
            break;
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT; index++) {
            if (port_info->ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_ST_PORT; index++) {
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

    index = 0;

    for (index = 0; index < numL0s; index++) {
        /* Node */

        node = &port_info->sched[index];

        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, index,
                    &sched_mode, &weight, 0, 0));
        LOG_CLI((BSL_META_U(unit,
                        "L0.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                    index, node->gport, node->hw_index,
                    sched_modes[sched_mode], weight));

        l1index = (index * 8) + numL0s;
        l1_max = l1index + 8;
        if (index == 2) {
            l1_max = l1index + 2;
        }
        for(; l1index < l1_max; l1index++) {
            node = &port_info->sched[l1index];
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                        &sched_mode, &weight, 0, 0));
            LOG_CLI((BSL_META_U(unit,
                            "    L1.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                        l1index - numL0s, node->gport, node->hw_index,
                        sched_modes[sched_mode], weight));

                /* UC Queue */
                node = &port_info->ucast[l1index - numL0s];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                            &sched_mode, &weight, 0, 0));
                LOG_CLI((BSL_META_U(unit,
                                "           UC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - numL0s, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));

                /* MC Queue */
                node = &port_info->mcast[l1index - numL0s];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - numL0s,
                            &sched_mode, &weight, 1, 0));
                LOG_CLI((BSL_META_U(unit,
                                "           MC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - numL0s, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));
            }
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}

STATIC int
_bcm_fb6_cosq_port_info_dump(int unit, bcm_port_t port)
{
    _bcm_fb6_cosq_port_info_t *port_info;
    _bcm_fb6_cosq_node_t *node;
    int index = 0, empty;
    int l1index;
    soc_fb6_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};



    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    port_info = &_bcm_fb6_cosq_port_info[unit][port];

    empty = TRUE;
    for (index = 0; index < SOC_FB6_NUM_SCHEDULER_PER_PORT; index++) {
        if (port_info->sched[index].numq > 0) {
            empty = FALSE;
            break;
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; index++) {
            if (port_info->ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; index++) {
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

    node = &port_info->sched[index];

    BCM_IF_ERROR_RETURN(
            _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, index,
                &sched_mode, &weight, 0, 0));
    LOG_CLI((BSL_META_U(unit,
                    "L0.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                index, node->gport, node->hw_index,
                sched_modes[sched_mode], weight));

    for (l1index = 1; l1index < 9; l1index++) {
        /* Node */
        node = &port_info->sched[l1index];

        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                    &sched_mode, &weight, 0, 0));
        LOG_CLI((BSL_META_U(unit,
                        "    L1.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                    l1index - 1, node->gport, node->hw_index,
                    sched_modes[sched_mode], weight));

                /* UC Queue */
                node = &port_info->ucast[l1index - 1];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                            &sched_mode, &weight, 0, 0));
                LOG_CLI((BSL_META_U(unit,
                                "          UC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - 1, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));

                /* MC Queue */
                node = &port_info->mcast[l1index - 1];
                BCM_IF_ERROR_RETURN(
                        _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                            &sched_mode, &weight, 1, 0));
                LOG_CLI((BSL_META_U(unit,
                                "          MC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                            l1index - 1, node->gport, node->hw_index,
                            sched_modes[sched_mode], weight));
    }
    LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));

    return BCM_E_NONE;
}

int
bcm_fb6_dump_port_hsp(int unit, int port)
{
    LOG_CLI((BSL_META_U(unit,
                    "\nSW Information COSQ - Unit %d\n"), unit));
    if (!IS_FAE_PORT(unit, port) && !IS_LB_PORT(unit, port)) {
        if (IS_CPU_PORT(unit, port)) {
            (void)_bcm_fb6_cosq_cpu_port_info_dump(unit);
        } else if(IS_STK_PORT(unit, port)) {
            (void)_bcm_fb6_cosq_st_port_info_dump(unit, port);
        }  else if(IS_UPLINK_PORT(unit, port)) {
            (void)_bcm_fb6_cosq_uplink_port_info_dump(unit, port);
        }  else {
            (void)_bcm_fb6_cosq_port_info_dump(unit, port);
        }
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_fb6_cosq_coe_port_info_dump(int unit, bcm_port_t port, int idx)
{

    int l0_num, num_l0;
    _bcm_fb6_cosq_coe_port_info_t *port_info;
    _bcm_fb6_cosq_node_t *node;
    int index = 0, empty;
    int l1index, i;
    int start, end;
    soc_fb6_sched_mode_e sched_mode = 0;
    int weight = 0;
    char *sched_modes[] = {"X", "SP", "WRR", "WERR"};

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, port, &l0_num, &num_l0));

    if(idx != -1) {
       if ( idx > num_l0) {
          return BCM_E_PARAM;
       }
        start = l0_num + idx;
        end = start + 1;
    } else {
       start = l0_num;
       end = l0_num + num_l0;
    }
    for (i = start; i < end; i++) {
        l1index = 0;
        port_info = &_bcm_fb6_cosq_coe_port_info[unit][i];

        empty = TRUE;
        for (index = 0; index < SOC_FB6_NUM_SCHEDULER_PER_PORT; index++) {
            if (port_info->sched[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
        if (empty) {
            for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; index++) {
                if (port_info->ucast[index].numq > 0) {
                    empty = FALSE;
                    break;
                }
            }
        }
        if (empty) {
            for (index = 0; index < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; index++) {
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
                        "=== PORT %d.%d\n"), port, (i - l0_num)));

        node = &port_info->sched[index];

        BCM_IF_ERROR_RETURN(
                _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, index,
                    &sched_mode, &weight, 0, 0));
        LOG_CLI((BSL_META_U(unit,
                        "L0.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                    index, node->gport, node->hw_index,
                    sched_modes[sched_mode], weight));

        for (l1index = 1; l1index < 9; l1index++) {
            /* Node */
            node = &port_info->sched[l1index];

            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                        &sched_mode, &weight, 0, 0));
            LOG_CLI((BSL_META_U(unit,
                            "    L1.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                        l1index - 1, node->gport, node->hw_index,
                        sched_modes[sched_mode], weight));

            /* UC Queue */
            node = &port_info->ucast[l1index - 1];
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                        &sched_mode, &weight, 0, 0));
            LOG_CLI((BSL_META_U(unit,
                            "          UC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                        l1index - 1, node->gport, node->hw_index,
                        sched_modes[sched_mode], weight));

            /* MC Queue */
            node = &port_info->mcast[l1index - 1];
            BCM_IF_ERROR_RETURN(
                    _bcm_fb6_cosq_sched_mode_setget(unit, port, node->level, l1index - 1,
                        &sched_mode, &weight, 1, 0));
            LOG_CLI((BSL_META_U(unit,
                            "          MC.%d: GPORT=0x%x HW_INDEX=%d MODE=%s WT=%d\n"),
                        l1index - 1, node->gport, node->hw_index,
                        sched_modes[sched_mode], weight));
        }
        LOG_CLI((BSL_META_U(unit,
                        "=========== \n")));
    }
   return BCM_E_NONE;
}

int
bcm_fb6_dump_subport_hsp(int unit, bcm_port_t port, int subport)
{
    if(!IS_COE_PORT(unit, port)) {
       return BCM_E_PARAM;
    }
    (void)_bcm_fb6_cosq_coe_port_info_dump(unit, port, subport-1);
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcm_fb6_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_fb6_cosq_sw_dump(int unit)
{
    bcm_port_t port;
    LOG_CLI((BSL_META_U(unit,
                    "\nSW Information COSQ - Unit %d\n"), unit));

    PBMP_ALL_ITER(unit, port) {
        if (!IS_FAE_PORT(unit, port) && !IS_LB_PORT(unit, port)) {
            if (IS_CPU_PORT(unit, port)) {
                (void)_bcm_fb6_cosq_cpu_port_info_dump(unit);
            } else if(IS_STK_PORT(unit, port)) {
                (void)_bcm_fb6_cosq_st_port_info_dump(unit, port);
            }  else if(IS_UPLINK_PORT(unit, port)) {
                (void)_bcm_fb6_cosq_uplink_port_info_dump(unit, port);
            }  else {
                if(IS_COE_PORT(unit, port)) {
                   (void)_bcm_fb6_cosq_coe_port_info_dump(unit, port, -1);
                }
                (void)_bcm_fb6_cosq_port_info_dump(unit, port);
            }
        }
    }
    return;
}


/* Routine to perform Inverse mapping for given Queue port back to Port/Priority
 * which was defined by default or through cosq_mapping_set functions
 */
int
_bcm_fb6_cosq_inv_mapping_get(int unit, bcm_gport_t gport,
        bcm_cos_queue_t cosq,
        uint32 flags, bcm_port_t *ing_port,
        bcm_cos_t *priority)
{
    bcm_port_t local_port;
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entries[1];
    int index, ii;
    bcm_cos_queue_t in_cosq = BCM_COS_INVALID, pri_cosq = BCM_COS_INVALID;
    bcm_module_t    modid;
    soc_field_t cos_field = INVALIDf;
    port_cos_map_entry_t cos_map_entries[_FB6_MMU_NUM_INT_PRI];

    if ((flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) &&
            (flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP)) {
        return BCM_E_PARAM;
    }

    if ((ing_port == NULL) || (priority == NULL)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) &&
                !(BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, &modid, &local_port,
                                    &in_cosq, NULL));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_localport_resolve(unit, gport, &local_port));
        in_cosq = cosq;
    }

    if ((cosq < -1) || (cosq >= _FB6_PORT_NUM_COS(unit, local_port))) {
        return BCM_E_PARAM;
    }

    if ((IS_CPU_PORT(unit, local_port)) || (IS_LB_PORT(unit, local_port))) {
        return BCM_E_PARAM;
    }

    *ing_port = local_port;

    if (flags == BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        cos_field = MC_COS1f;
    } else {
        cos_field = UC_COS1f;
    }
    if (in_cosq == BCM_COS_INVALID) {
        return BCM_E_PARAM;
    }

    entries[0] = &cos_map_entries;
    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
            SELECTf);
    index *= _FB6_MMU_NUM_INT_PRI;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_fb6_cos_map_profile[unit],
                             index, _FB6_MMU_NUM_INT_PRI, entries));

    for (ii = 0; ii < _FB6_MMU_NUM_INT_PRI; ii++) {
        pri_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm,
                &cos_map_entries[ii], cos_field);
        if (pri_cosq == in_cosq) {
            /* Required Cosq found. Break as the index is the priority */
            *priority = ii;
            break;
        }
    }

    if (ii == _FB6_MMU_NUM_INT_PRI) {
        /* Mapping cannot be found */
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

int
bcm_fb6_pfc_deadlock_ignore_pfc_xoff_gen(int unit, int priority,
        bcm_port_t port, uint32 *rval32)
{

    uint32 profile_index, fc_class;
    int index;
    uint32 cos_bmp;
    uint64 rval64[16], *rval64s[1];


    rval64s[0] = rval64;


    BCM_IF_ERROR_RETURN(_bcm_fb6_cosq_pri2cos_profile_setget
            (unit, port, -1, &index, 0));

    profile_index = 16 * index;

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_fb6_prio2cos_profile[unit],
                             profile_index, 16, rval64s));

    for (fc_class = 0; fc_class < _BCM_FB6_NUM_PFC_CLASS; ++fc_class ) {
        cos_bmp = soc_reg64_field32_get(unit, PRIO2COS_PROFILE_64r, rval64[fc_class],
                COS_BMPf);

        if (cos_bmp & (1 << priority)) {
            *rval32 = cos_bmp;
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

/*
 *  Function:
 *      bcm_fb6_pfc_deadlock_recovery_start
 *  Purpose:
 *     Begin recovery when a Port is deaclared to be in Deadlock by Hardware
 *  Parameters:
 *     unit - unit number
 *     port - port number
 *     priority - priority
 *  Returns:
 *     BCM_E_XXX
 */
int
bcm_fb6_pfc_deadlock_recovery_start(int unit, bcm_port_t port, int priority)
{
    uint32 rval = 0, rval1 = 0;
    int cos = 0;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    soc_mem_t mem = INVALIDm;
    soc_field_t field = INVALIDf;
    soc_field_t field1 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, local_port, split;
    int type, offset, rv;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    LOG_INFO(BSL_LS_BCM_COSQ,
            (BSL_META_U(unit,
                        "PFC Deadlock Detected: Cos %d port=%d\n"), cos, port));


   BCM_IF_ERROR_RETURN(
           bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));

   BCM_IF_ERROR_RETURN(
          _bcm_fb6_pfc_deadlock_hw_cos_index_get(unit, priority, &cos));

   sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
   mem = hw_res->timer_en[split];
   rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
   if (BCM_FAILURE(rv)) {
       return rv;
   }

   field = (offset == 1)  ? STATUS_MASK1f : STATUS_MASK2f;
   field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
   rval = soc_mem_field32_get(unit, mem, entry, field);
   rval1 = soc_mem_field32_get(unit, mem, entry, field1);
   rval |= (1 << cos);  /* set the status mask to 1*/
   rval1 &= ~(1 << cos);  /* disable the timer by setting 0 */
   soc_mem_field32_set(unit, mem, entry, field, rval);
   soc_mem_field32_set(unit, mem, entry, field, rval1);

   rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
   if (BCM_FAILURE(rv)) {
       return rv;
   }


    return BCM_E_NONE;
}

/*
 *  Function:
 *     bcm_fb6_pfc_deadlock_recovery_exit
 *  Purpose:
 *     Reset recovery state of a port and config port back in original state
 *  Parameters:
 *     unit - unit number
 *     port - port number
 *  priority - priority
 *     Returns:
 *  BCM_E_XXX
 */

int
bcm_fb6_pfc_deadlock_recovery_exit(int unit, bcm_port_t port, int priority)
{
    uint32 rval, rval1;
    _bcm_fb6_pfc_hw_resorces_t *hw_res = NULL;
    _bcm_fb6_pfc_deadlock_control_t *pfc_deadlock_control = NULL;
    soc_mem_t mem = INVALIDm;
    soc_field_t field1 = INVALIDf;
    soc_field_t field2 = INVALIDf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int index, local_port, split;
    int type, offset, rv;

    pfc_deadlock_control = _BCM_FB6_UNIT_PFC_DEADLOCK_CONTROL(unit);
    hw_res = &pfc_deadlock_control->hw_regs_fields;

    BCM_IF_ERROR_RETURN(
            bcm_fb6_pfc_deadlock_queue_info_get(unit, port, &local_port, &type, &split, &index, &offset));

    /* For that port, set ignore_pfc_xoff = 0  */
    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_mask[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    field1 = (offset == 1)  ? IGNORE_PFC_XOFF1f: IGNORE_PFC_XOFF2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval &= ~(1 << priority);

    soc_mem_field32_set(unit, mem, entry, field1, rval);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Mask the Intr STATUS_MASK by setting 0 and
     *      Turn timer off TIMER_ENABLE  by setting 1 */

    sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
    mem = hw_res->timer_en[split];
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    field1 = (offset == 1)  ? ENABLE_1f : ENABLE_2f;
    field2 = (offset == 1)  ? STATUS_MASK1f : STATUS_MASK2f;

    rval = soc_mem_field32_get(unit, mem, entry, field1);
    rval1 = soc_mem_field32_get(unit, mem, entry, field2);

    rval |= (1 << priority);    /*enable  the timer by setting 1 */
    rval1 &= ~(1 << priority);  /* set the status mask to 0*/

    soc_mem_field32_set(unit, mem, entry, field1, rval);
    soc_mem_field32_set(unit, mem, entry, field2, rval1);
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}


int
bcm_fb6_pfc_deadlock_queue_info_get(int unit, bcm_gport_t gport,
                                    bcm_port_t *local_port, int *type,
                                    int *split, int *index, int *offset)
{
    int lport = 0, in_cosq = 0;
    int l0_num = 0, num_l0 = 0, i = 0;
    int modid;
    if( (local_port == NULL) ||  (split == NULL) ||
          (index == NULL) || (offset == NULL) ) {
        return BCM_E_PARAM;
    }


    if (BCM_GPORT_IS_SET(gport)) {
        if (!(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) &&
                !(BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_fb6_cosq_node_get(unit, gport, &modid, &lport,
                                    &in_cosq, NULL));
    } else {
         lport = gport;
    }

    if (BCM_GPORT_IS_SET(gport)) {
        if (in_cosq < 8) {
            i = 0;
        } else if (in_cosq >= 8 && in_cosq < 16) {
            i = 1;
        } else if (in_cosq >= 16 && in_cosq < 24) {
            i = 2;
        } else {
            i = 3;
        }
        *type = i;
    }
    BCM_IF_ERROR_RETURN(_soc_fb6_port_base_l0_mapping_get(unit, lport, &l0_num, &num_l0));
    *split = (l0_num & 0x6) >> 1;
    *index = (l0_num & 0x1f8) >> 3;
    *offset = (l0_num & 0x01) + 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fb6_cosq_gport_hw_qnum_get
 * Purpose:
 *     Get hw cos queue number from cos gport
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 *     qnum       - (OUT) queue number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fb6_cosq_gport_hw_qnum_get(int unit,
            bcm_gport_t gport, int *qnum)
{
    int rv = BCM_E_NONE;

    rv = _bcm_fb6_cosq_port_resolve(unit,
            gport, NULL, NULL, NULL, NULL, qnum);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        (*qnum) += SOC_FB6_NUM_UC_QUEUES_PER_PIPE;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_fb6_cosq_hw_qnum_gport_get
 * Purpose:
 *     Get GPORT for given hw cos queue number.
 * Parameters:
 *     unit       - (IN) unit number
 *     qnum       - (IN) queue number
 *     gport      - (OUT) queue group/scheduler GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fb6_cosq_hw_qnum_gport_get(int unit, int qnum, bcm_gport_t *gport)
{
    int is_mcast = 0;
    int port, index;
    _bcm_fb6_cosq_node_t *node_base = NULL;
    _bcm_fb6_cosq_port_info_t *port_info = NULL;

    if (gport == NULL) {
        return BCM_E_PARAM;
    }
    if (qnum >= SOC_FB6_NUM_UC_QUEUES_PER_PIPE) {
        qnum -= SOC_FB6_NUM_UC_QUEUES_PER_PIPE;
        is_mcast = 1;
    }

    *gport = BCM_GPORT_INVALID;
    PBMP_ALL_ITER(unit, port) {
        if (IS_FAE_PORT(unit, port) || IS_LB_PORT(unit, port)) {
            continue;
        }
        if (IS_CPU_PORT(unit, port) || IS_STK_PORT(unit, port) ||
                IS_UPLINK_PORT(unit, port) || IS_COE_PORT(unit, port)) {
            continue;
        }

        port_info = &_bcm_fb6_cosq_port_info[unit][port];

        if (is_mcast == 0) {

            /* Unicast node */
            node_base = port_info->ucast;
            for (index = 0; index < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT; index++) {
                if (node_base[index].hw_index == qnum) {
                    break;
                }
            }
            if (index < SOC_FB6_NUM_UCAST_QUEUE_PER_PORT) {
                BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, port, index);
                return BCM_E_NONE;
            }
        } else {

            /* Multicast node */
            node_base = port_info->mcast;
            for (index = 0; index < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT; index++) {
                if (node_base[index].hw_index == qnum) {
                    break;
                }
            }
            if (index < SOC_FB6_NUM_MCAST_QUEUE_PER_PORT) {
                BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, port, index);
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
}

#endif /* BCM_FIREBOLT6_SUPPORT */
