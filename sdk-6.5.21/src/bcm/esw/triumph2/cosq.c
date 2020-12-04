/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/stack.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TR2_DRR_WEIGHT_MAX       0x7f
#define TR2_WRR_WEIGHT_MAX       0x7f

#define TR2_PACKET_FIELD_MAX     0x3fff
#define TR2_CELL_FIELD_MAX       0x7fff

#define TR2_PKT_REFRESH_MAX      0xfffff
#define TR2_PKT_THD_MAX          0x7ff

/* COSQ where output of hierarchical COSQs scheduler is attached */
#define TR2_SCHEDULER_COSQ       8

/* MMU cell size in bytes */
#define _BCM_TR2_COSQ_CELLSIZE   128

/* Number of ports */
#define TR2_PORT_COUNT           52

/* COS Map profile entries per set */
#define TR2_COS_MAP_ENTRIES_PER_SET    16

/* Cache of COS_MAP Profile Table */
STATIC soc_profile_mem_t *_tr2_cos_map_profile[BCM_MAX_NUM_UNITS] = {NULL};
/* Cache of COS_EGR_MAP Profile Table */
STATIC soc_profile_mem_t *_tr2_cos_egr_map_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* Number of COSQs configured for this device */
STATIC int _tr2_num_cosq[SOC_MAX_NUM_DEVICES];

/* Port Bitmap which includes ports capable of hierarchical queueing */
STATIC soc_pbmp_t _tr2_cosq_24q_ports[SOC_MAX_NUM_DEVICES];

/* Array to keep track of the number of hierarchical COSQs added per port */
STATIC uint8 *_tr2_num_port_cosq[SOC_MAX_NUM_DEVICES] = {NULL};

/* Forward declarations */
STATIC int _bcm_tr2_cosq_gport_delete(int unit, bcm_port_t port);
STATIC int _bcm_tr2_cosq_port_sched_set(int unit, soc_reg_t config_reg, 
                                       soc_reg_t weight_reg, soc_reg_t minsp_reg,
                                       bcm_port_t port, bcm_cos_queue_t start_cosq,
                                       int num_weights, const int weights[], 
                                       int mode);
STATIC int _bcm_tr2_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg,
                                    soc_reg_t thresh_reg_green,
                                    soc_reg_t thresh_reg_yellow,
                                    soc_reg_t thresh_reg_red,
                                    soc_reg_t thresh_reg_non_tcp,
                                    uint32 min_thresh, uint32 max_thresh,
                                    int drop_probability, int gain);


/*
 * Function:
 *      _bcm_tr2_egr_cpu_cos_map
 * Purpose:
 *      Set the initial one to one mapping for he EGR_CPU_COS_MAP table
 * Parameters:
 *      unit - Unit number.
 *      numq - Number of cosq provided
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_tr2_egr_cpu_cos_map(int unit, int numq)
{
    int i, imin, imax, bytes, nent, rv;
    egr_cpu_cos_map_entry_t *entblk, *entryp;
    int cos, ratio, remain;
    
    imin = soc_mem_index_min(unit, EGR_CPU_COS_MAPm);
    imax = soc_mem_index_max(unit, EGR_CPU_COS_MAPm);
    nent = soc_mem_index_count(unit, EGR_CPU_COS_MAPm);
    bytes = soc_mem_entry_words(unit, EGR_CPU_COS_MAPm);
    bytes = WORDS2BYTES(bytes);
    entblk = soc_cm_salloc(unit, nent * sizeof(egr_cpu_cos_map_entry_t), 
                          "EGR_CPU_COS_MAP");
    if (entblk == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, EGR_CPU_COS_MAPm);
    rv = soc_mem_read_range(unit, EGR_CPU_COS_MAPm, MEM_BLOCK_ANY,
                            imin, imax, entblk);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, EGR_CPU_COS_MAPm);
        soc_cm_sfree(unit, entblk);
        return rv;
    }
    
    ratio = nent / numq;
    remain = nent % numq;
    
    cos = 0;
    for(i = 0; i < nent; i++) {
        entryp = soc_mem_table_idx_to_pointer(unit, EGR_CPU_COS_MAPm,
                        egr_cpu_cos_map_entry_t *, entblk, i);

        soc_mem_field32_set(unit, EGR_CPU_COS_MAPm, (uint32 *) entryp,
                            REDIRECT_COSf, cos);

        if ((i + 1) == (((cos + 1) * ratio) + 
                        ((remain < (numq - cos)) ? 0 :
                         (remain - (numq - cos) + 1)))) {
            cos++;
        }

    }

    rv = soc_mem_write_range(unit, EGR_CPU_COS_MAPm, MEM_BLOCK_ALL, 
                             imin, imax, entblk);
    soc_mem_unlock(unit, EGR_CPU_COS_MAPm);
    soc_cm_sfree(unit, entblk);

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      bcm_tr2_cosq_sync
 * Purpose:
 *      Record COSQ module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &cosq_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL));
    
    /* Number COSQ */
    num_cosq = _tr2_num_cosq[unit];
    sal_memcpy(cosq_scache_ptr, &num_cosq, sizeof(num_cosq));
    cosq_scache_ptr += sizeof(num_cosq);
        
    /* Number of hierarchical COSQs added per port */
    if (_tr2_num_port_cosq[unit] != NULL) {
        sal_memcpy(cosq_scache_ptr, _tr2_num_port_cosq[unit],
                   sizeof(uint8) * TR2_PORT_COUNT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_cosq_reinit(int unit)
{
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint8               *cosq_scache_ptr;
    uint32              num_cosq;
 	 
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
        _tr2_num_cosq[unit] = num_cosq;
        cosq_scache_ptr += sizeof(num_cosq);

        /* Number of hierarchical COSQs added per port */
        if (_tr2_num_port_cosq[unit] != NULL) {
            sal_memcpy(_tr2_num_port_cosq[unit], cosq_scache_ptr,
                       sizeof(uint8) * TR2_PORT_COUNT);
        }

    } else {
        _tr2_num_cosq[unit] = _bcm_esw_cosq_config_property_get(unit);
    }
    
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_cosq_init
 * Purpose:
 *      Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_cosq_init(int unit)
{
    STATIC int _tr2_max_cosq = -1;
    int num_cos;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint32              cosq_scache_size;
    uint8               *cosq_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (_tr2_max_cosq < 0) {
        _tr2_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN (bcm_tr2_cosq_detach(unit, 0));
    }

    num_cos = _bcm_esw_cosq_config_property_get(unit);

    SOC_PBMP_CLEAR(_tr2_cosq_24q_ports[unit]);
    /* Keep track of ports that support 24 (16 + 8) queues */
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 26);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 27);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 28);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 29);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 30);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 31);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 34);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 38);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 39);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 42);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 43);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 46);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 50);
    SOC_PBMP_PORT_ADD(_tr2_cosq_24q_ports[unit], 51);
    if (_tr2_num_port_cosq[unit] == NULL) {
        _tr2_num_port_cosq[unit] = sal_alloc(sizeof(uint8) * TR2_PORT_COUNT,
                                     "_tr2_num_port_cosq");
        if (_tr2_num_port_cosq[unit] == NULL) {
                return BCM_E_MEMORY;
        }
    }
    sal_memset(_tr2_num_port_cosq[unit], 0, sizeof(uint8) * TR2_PORT_COUNT);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Warm boot level 2 cache size */
    cosq_scache_size = sizeof(uint32); /* Number COSQ */
    if (_tr2_num_port_cosq[unit] != NULL) {  /* Number of COSQs per port */
        cosq_scache_size += (sizeof(uint8) * TR2_PORT_COUNT);
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                 (0 == SOC_WARM_BOOT(unit)),
                                 cosq_scache_size, &cosq_scache_ptr, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_tr2_cosq_reinit(unit));
        num_cos = _tr2_num_cosq[unit];
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return bcm_tr2_cosq_config_set(unit, num_cos);
}

/*
 * Function:
 *      bcm_cosq_detach
 * Purpose:
 *      Discard all COS schedule/mapping state.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_cosq_detach(int unit, int software_state_only)
{
    bcm_port_t port;
    int weights[8];
    int cosq;

    /* Delete any gports that were added */
    if (_tr2_num_port_cosq[unit]) {
        BCM_PBMP_ITER(_tr2_cosq_24q_ports[unit], port) {
            if (_tr2_num_port_cosq[unit][port])
            {
                if (!software_state_only)
                {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr2_cosq_gport_delete(unit, port));
                }
            }
        } 

        /* Destroy profile table cache */
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_destroy(unit, _tr2_cos_map_profile[unit]));
        sal_free(_tr2_cos_map_profile[unit]);
        _tr2_cos_map_profile[unit] = NULL;

        sal_free(_tr2_num_port_cosq[unit]);
        _tr2_num_port_cosq[unit] = NULL;
    }

    for (cosq = 0; cosq < 8; cosq++) {
        weights[cosq] = 0;
    }

    if (!software_state_only)
    {
        PBMP_ALL_ITER(unit, port) {
            if (IS_CPU_PORT(unit, port)) {
                continue;
            }

            /* Clear bandwidth settings on port */
            for (cosq = 0; cosq < 8; cosq++) {
                 BCM_IF_ERROR_RETURN
                    (bcm_tr2_cosq_port_bandwidth_set(unit, port, cosq,
                                                     0, 0, 0, 0));
            }

            /* Clear scheduling settings on port */
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_port_sched_set(unit,
                                             ESCONFIGr, COSWEIGHTSr, MINSPCONFIGr,
                                             port, 0, 8, weights, 
                                             BCM_COSQ_WEIGHTED_ROUND_ROBIN));
            /* Clear discard settings on port */
     
            for (cosq = 0; cosq < 8; cosq++) {
                 BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_set(unit, port, 
                                          BCM_COSQ_DISCARD_COLOR_ALL | 
                                          BCM_COSQ_DISCARD_NONTCP, cosq,
                                          WREDCONFIG_PACKETr,
                                          WREDPARAM_PACKETr,
                                          WREDPARAM_YELLOW_PACKETr,
                                          WREDPARAM_RED_PACKETr,
                                          WREDPARAM_NONTCP_PACKETr,
                                          TR2_PACKET_FIELD_MAX, 
                                          TR2_PACKET_FIELD_MAX, 100, 0));
            }
        }

        /* Disable WRED on all cosq level */
        BCM_IF_ERROR_RETURN (bcm_tr2_cosq_discard_set(unit, 0));
    }


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_config_set
 * Purpose:
 *      Set the number of COS queues
 * Parameters:
 *      unit - Unit number.
 *      numq - number of COS queues (1-8).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_cosq_config_set(int unit, int numq)
{
    int cos, prio, ratio, remain;
    uint32 index, egr_index, minsp;
    soc_mem_t mem = PORT_COS_MAPm; 
    soc_mem_t egr_mem = EGR_COS_MAPm;
    int entry_words;
    bcm_pbmp_t ports;
    bcm_port_t port;
    port_cos_map_entry_t cos_map_array[TR2_COS_MAP_ENTRIES_PER_SET];
    egr_cos_map_entry_t  egr_cos_map_array[TR2_COS_MAP_ENTRIES_PER_SET];
    void *entries[1], *egr_entries[1];

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    /* Map the eight 802.1 priority levels to the active cosqs */
    if (numq > 8) {
        numq = 8;
    }

    sal_memset(cos_map_array, 0, 
               COUNTOF(cos_map_array) * sizeof(port_cos_map_entry_t));
    entries[0] = &cos_map_array;
    if (_tr2_cos_map_profile[unit] == NULL) {
        _tr2_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "COS_MAP Profile Mem");
        if (_tr2_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_tr2_cos_map_profile[unit]);
    }
    /* Create profile table cache (or re-init if it already exists) */
    entry_words = sizeof(port_cos_map_entry_t)/ sizeof(uint32);
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               _tr2_cos_map_profile[unit]));

    sal_memset(egr_cos_map_array, 0, 
               COUNTOF(egr_cos_map_array) * sizeof(egr_cos_map_entry_t));
    egr_entries[0] = &egr_cos_map_array;
    if (_tr2_cos_egr_map_profile[unit] == NULL) {
        _tr2_cos_egr_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "EGR COS_MAP Profile Mem");
        if (_tr2_cos_egr_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_tr2_cos_egr_map_profile[unit]);
    }
    /* Create profile table cache (or re-init if it already exists) */
    entry_words = sizeof(egr_cos_map_entry_t)/ sizeof(uint32);
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &egr_mem, &entry_words, 1,
                                               _tr2_cos_egr_map_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {    /* Warm Boot */
        int     i;
        uint32  val;
        uint64  val64;

        /* Gather reference count for cosq map profile tables */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
            index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
            index *= TR2_COS_MAP_ENTRIES_PER_SET;
            for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                SOC_PROFILE_MEM_REFERENCE
                    (unit, _tr2_cos_map_profile[unit],
                     index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET
                    (unit, _tr2_cos_map_profile[unit],
                     index + i, TR2_COS_MAP_ENTRIES_PER_SET);
            }

            if (soc_mem_field_valid(unit, EGR_COS_MAPm, REDIRECT_COSf)) {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_PORT_64r(unit, port, &val64));
                index = soc_reg64_field32_get(unit, EGR_PORT_64r, val64,
                                              EGR_COS_MAP_SELf);
                index *= TR2_COS_MAP_ENTRIES_PER_SET;
                for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                    SOC_PROFILE_MEM_REFERENCE
                        (unit, _tr2_cos_egr_map_profile[unit],
                         index + i, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET
                        (unit, _tr2_cos_egr_map_profile[unit],
                         index + i, TR2_COS_MAP_ENTRIES_PER_SET);
                }
            }

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
            if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
                SOC_IF_ERROR_RETURN(READ_ICOS_MAP_SELr(unit, port, &val));
                index = soc_reg_field_get(unit, ICOS_MAP_SELr, val, SELECTf);
                index *= TR2_COS_MAP_ENTRIES_PER_SET;
                for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                    SOC_PROFILE_MEM_REFERENCE
                        (unit, _tr2_cos_map_profile[unit],
                         index + i, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET
                        (unit, _tr2_cos_map_profile[unit],
                         index + i, TR2_COS_MAP_ENTRIES_PER_SET);
                }

                if (soc_mem_field_valid(unit, EGR_COS_MAPm, REDIRECT_COSf)) {
                    SOC_IF_ERROR_RETURN
                        (READ_IEGR_PORT_64r(unit, port, &val64));
                    index = soc_reg64_field32_get(unit, IEGR_PORT_64r, val64,
                                                  EGR_COS_MAP_SELf);
                    index *= TR2_COS_MAP_ENTRIES_PER_SET;
                    for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                        SOC_PROFILE_MEM_REFERENCE
                            (unit, _tr2_cos_egr_map_profile[unit],
                             index + i, 1);
                        SOC_PROFILE_MEM_ENTRIES_PER_SET
                            (unit, _tr2_cos_egr_map_profile[unit],
                             index + i, TR2_COS_MAP_ENTRIES_PER_SET);
                    }
                }
            }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
        }

        _tr2_num_cosq[unit] = numq;

        return BCM_E_NONE;
    }

    ratio = 8 / numq;
    remain = 8 % numq;
    cos = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                                &cos_map_array[prio], HG_COSf, 0);
        }
        if ((prio + 1) == (((cos + 1) * ratio) +
                           ((remain < (numq - cos)) ? 0 :
                            (remain - (numq - cos) + 1)))) {
            cos++;
        }
    }

    /* Map remaining internal priority levels to highest priority cosq */
    cos = numq - 1;
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, cos);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

    /* Map egress priority */
    if (soc_mem_field_valid(unit, EGR_COS_MAPm, REDIRECT_COSf)) {
        ratio = 16 / numq;
        remain = 16 % numq;
        cos = 0;
        for (prio = 0; prio < 16; prio++) {
            soc_mem_field32_set(unit, EGR_COS_MAPm,
                                &egr_cos_map_array[prio], REDIRECT_COSf, cos);
            if ((prio + 1) == (((cos + 1) * ratio) +
                               ((remain < (numq - cos)) ? 0 :
                                (remain - (numq - cos) + 1)))) {
                cos++;
            }
        }
        /* Map egress priority */
        if (soc_mem_field_valid(unit, EGR_CPU_COS_MAPm, REDIRECT_COSf)) {
            BCM_IF_ERROR_RETURN(
                                _bcm_tr2_egr_cpu_cos_map(unit, numq));
        }
    }

    /* Add a profile mem entry for each port */
    BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _tr2_cos_map_profile[unit],
                                 (void *) &entries,
                                 TR2_COS_MAP_ENTRIES_PER_SET, &index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, port, SELECTf,
                                    index / TR2_COS_MAP_ENTRIES_PER_SET));
        if (soc_mem_field_valid(unit, EGR_COS_MAPm, REDIRECT_COSf)) {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_add(unit, _tr2_cos_egr_map_profile[unit],
                                    (void *) &egr_entries,
                                    TR2_COS_MAP_ENTRIES_PER_SET, &egr_index));
                SOC_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, EGR_PORT_64r, port,  
                                              EGR_COS_MAP_SELf,
                                              egr_index / TR2_COS_MAP_ENTRIES_PER_SET));
        }
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if ((IS_HG_PORT(unit, port) || port == CMIC_PORT(unit))) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _tr2_cos_map_profile[unit],
                                     (void *) &entries,
                                     TR2_COS_MAP_ENTRIES_PER_SET, &index));
            if (soc_mem_field_valid(unit, EGR_COS_MAPm, REDIRECT_COSf)) {
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_add(unit, _tr2_cos_egr_map_profile[unit],
                                        (void *) &egr_entries,
                                        TR2_COS_MAP_ENTRIES_PER_SET,
                                        &egr_index));
                 SOC_IF_ERROR_RETURN
                      (soc_reg_field32_modify(unit, IEGR_PORT_64r, port,  
                                              EGR_COS_MAP_SELf,
                                              egr_index / TR2_COS_MAP_ENTRIES_PER_SET));
            }

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, SELECTf,
                                        index / TR2_COS_MAP_ENTRIES_PER_SET));
        }
#endif
    }

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* identity mapping for higig ports */

    /* map prio0->cos0, prio1->cos1, ... , prio7->cos7 */
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, prio);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
    }
    /* Map remaining internal priority levels to highest priority cosq */
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], COSf, 7);
        if (soc_mem_field_valid(unit,PORT_COS_MAPm,HG_COSf)) {
            soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[prio], HG_COSf, 0);
        }
    }
    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _tr2_cos_map_profile[unit],
                                     (void *) &entries,
                                     TR2_COS_MAP_ENTRIES_PER_SET, &index));
            soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                   SELECTf,
                                   (index / TR2_COS_MAP_ENTRIES_PER_SET));
        }
    }
#endif

    /* Always strict priority for SC and QM COS queues */
    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN
            (READ_MINSPCONFIGr(unit, port, &minsp));
        soc_reg_field_set(unit, MINSPCONFIGr, &minsp, COS8_IS_SPf, 1);
        soc_reg_field_set(unit, MINSPCONFIGr, &minsp, COS9_IS_SPf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_MINSPCONFIGr(unit, port, minsp));
    }

    _tr2_num_cosq[unit] = numq;

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_config_get
 * Purpose:
 *      Get the number of cos queues
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      numq - (Output) number of cosq
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_cosq_config_get(int unit, int *numq)
{
    if (_tr2_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _tr2_num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_mapping_set
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
bcm_tr2_cosq_mapping_set(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    uint32 val, old_index, new_index;
    int i;
    port_cos_map_entry_t cos_map_array[TR2_COS_MAP_ENTRIES_PER_SET], *entry_p;
    void *entries[1];
    bcm_pbmp_t ports;

    if (priority < 0 || priority >= 16) {
        return (BCM_E_PARAM);
    }

    if (cosq < 0 || cosq >= 8) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {	/* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else if (SOC_PORT_VALID(unit, port) && IS_ALL_PORT(unit, port)) {
        BCM_PBMP_CLEAR(ports);
        BCM_PBMP_PORT_ADD(ports, port);
    } else {
        return BCM_E_PORT;
    }

    entries[0] = &cos_map_array;

    PBMP_ITER(ports, port) {
        SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
        old_index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
        old_index *= TR2_COS_MAP_ENTRIES_PER_SET;

        /* get current mapping profile values */
        for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
            entry_p = SOC_PROFILE_MEM_ENTRY(unit, _tr2_cos_map_profile[unit],
                                            port_cos_map_entry_t *, (old_index + i));
            memcpy(&cos_map_array[i], entry_p, sizeof(*entry_p));
        }
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[priority], COSf, cosq);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _tr2_cos_map_profile[unit],
                                 (void *) &entries,
                                 TR2_COS_MAP_ENTRIES_PER_SET, &new_index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, port, 
                                    SELECTf,
                                    new_index / TR2_COS_MAP_ENTRIES_PER_SET));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _tr2_cos_map_profile[unit], old_index));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _tr2_cos_map_profile[unit],
                                     (void *) &entries,
                                     TR2_COS_MAP_ENTRIES_PER_SET, &new_index));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                        SELECTf,
                                        new_index / TR2_COS_MAP_ENTRIES_PER_SET));
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _tr2_cos_map_profile[unit], old_index));
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_mapping_get
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
bcm_tr2_cosq_mapping_get(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    uint32 val;
    int index;
    port_cos_map_entry_t *entry_p;

    if (priority < 0 || priority >= 16) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {
        port = REG_PORT_ANY;
    } else if (!SOC_PORT_VALID(unit, port) || !IS_ALL_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
    index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
    index *= TR2_COS_MAP_ENTRIES_PER_SET;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _tr2_cos_map_profile[unit],
                                    port_cos_map_entry_t *, 
                                    (index + priority));
    *cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, COSf);
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_bucket_set(int unit, bcm_port_t port, 
                                bcm_cos_queue_t cosq,
                                uint32 min_quantum, /* kbps or packet/second */
                                uint32 max_quantum, /* kbps or packet/second */
                                uint32 kbits_burst_min, 
                                uint32 kbits_burst_max, 
                                uint32 flags)
{
    uint32 regval;
    uint32 bucket_val = 0;
    uint64 regval_64, tmp;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = flags;
    int    refresh_bitsize, bucket_bitsize;

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET,MINBUCKET
     * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    /* Disable egress metering for this port */
    BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_REFRESHf, 0);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_THDf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64, MIN_REFRESHf, 0);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64, MIN_THDf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MINBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    /*reset the MAXBUCKET register*/
    soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, MAX_BUCKETf, 0);
    soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETr(unit, port, cosq, bucket_val));

    /*reset the MINBUCKET register value*/
    soc_reg_field_set(unit, MINBUCKETr, &bucket_val, MIN_BUCKETf, 0);
    soc_reg_field_set(unit, MINBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MINBUCKETr(unit, port, cosq, bucket_val));

    refresh_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_REFRESHf);
    bucket_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(min_quantum, kbits_burst_min,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    COMPILER_64_ZERO(regval_64);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64,
                          METER_GRANf, granularity);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r, &regval_64,
                          MIN_REFRESHf, refresh_rate);
    soc_reg64_field32_set(unit, MINBUCKETCONFIG_64r,
                          &regval_64, MIN_THDf, bucketsize);
    BCM_IF_ERROR_RETURN
        (WRITE_MINBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    refresh_bitsize =
        soc_reg_field_length(unit, MAXBUCKETCONFIG_64r, MAX_REFRESHf);
    bucket_bitsize =
        soc_reg_field_length(unit, MAXBUCKETCONFIG_64r, MAX_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(max_quantum, kbits_burst_max,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    COMPILER_64_ZERO(regval_64);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64,
                          METER_GRANf, granularity);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64,
                          MAX_REFRESHf, refresh_rate);
    soc_reg64_field32_set(unit, MAXBUCKETCONFIG_64r, &regval_64, MAX_THDf,
                          bucketsize);
    BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG_64r(unit, port, cosq, regval_64));

    if (flags & _BCM_XGS_METER_FLAG_PACKET_MODE) {
        BCM_IF_ERROR_RETURN(READ_SHAPING_MODEr(unit, port, &regval_64));
        COMPILER_64_SET(tmp, 0, 1);
        COMPILER_64_SHL(tmp, cosq);
        COMPILER_64_OR(regval_64, tmp);
        BCM_IF_ERROR_RETURN(WRITE_SHAPING_MODEr(unit, port, regval_64));
    } else {
        BCM_IF_ERROR_RETURN(READ_SHAPING_MODEr(unit, port, &regval_64));
        COMPILER_64_SET(tmp, 0, 1);
        COMPILER_64_SHL(tmp, cosq);
        COMPILER_64_NOT(tmp);
        COMPILER_64_AND(regval_64, tmp);
        BCM_IF_ERROR_RETURN(WRITE_SHAPING_MODEr(unit, port, regval_64));
    }

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_bucket_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                uint32 *min_quantum, /* kbps or packet/second */
                                uint32 *max_quantum, /* kbps or packet/second */
                                uint32 *kbits_burst_min, uint32 *kbits_burst_max,
                                uint32 *flags)
{
    uint32 regval;
    uint64 regval_64;
    uint32 refresh_rate = 0, bucketsize = 0, granularity = 3, meter_flags;

    if (!min_quantum || !max_quantum || !flags) {
        return (BCM_E_PARAM);
    }

    meter_flags = *flags;

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    granularity = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                            regval_64, METER_GRANf);
    refresh_rate = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           max_quantum, kbits_burst_max));

    BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG_64r(unit, port, cosq, &regval_64));

    granularity = soc_reg64_field32_get(unit, MINBUCKETCONFIG_64r,
                                            regval_64, METER_GRANf);
    refresh_rate = soc_reg64_field32_get(unit, MINBUCKETCONFIG_64r,
                                             regval_64, MIN_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, MINBUCKETCONFIG_64r,
                                             regval_64, MIN_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           min_quantum, kbits_burst_min));

    return BCM_E_NONE;
}

int
bcm_tr2_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                uint32 min_quantum, /* kbps or packet/second */
                                uint32 max_quantum, /* kbps or packet/second */
                                uint32 burst_quantum, /* kbps or packet/second */
                                uint32 flags)
{

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    /*keep place with original*/
    return _bcm_tr2_cosq_bucket_set(unit, port, cosq, min_quantum, max_quantum,
                                    min_quantum, burst_quantum, flags);    
}

int
bcm_tr2_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                                bcm_cos_queue_t cosq,
                                uint32 *min_quantum, /* kbps or packet/second */
                                uint32 *max_quantum, /* kbps or packet/second */
                                uint32 *burst_quantum, /* kbps or packet/second */
                                uint32 *flags)
{
    uint32 kbit_burst_min;

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_tr2_cosq_bucket_get(unit, port, cosq,
                        min_quantum, max_quantum, &kbit_burst_min,
                        burst_quantum, flags));
    return BCM_E_NONE;
}

int
bcm_tr2_cosq_port_pps_set(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq, int pps)
{
    uint32 min, max, burst, flags = _BCM_XGS_METER_FLAG_PACKET_MODE;
    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (bcm_tr2_cosq_port_bandwidth_get(unit, port, cosq,
                                         &min, &max, &burst, &flags));
    /* Set the current PPS setting */
    return bcm_tr2_cosq_port_bandwidth_set(unit, port, cosq,
                                           min, pps, burst, flags);
}

int
bcm_tr2_cosq_port_pps_get(int unit, bcm_port_t port,
                          bcm_cos_queue_t cosq, int *pps)
{
    uint32 min, max, burst, flags = _BCM_XGS_METER_FLAG_PACKET_MODE;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_tr2_cosq_port_bandwidth_get(unit, port, cosq,
                                               &min, &max, &burst, &flags));
    *pps = max;
    return BCM_E_NONE;
}

int
bcm_tr2_cosq_port_burst_set(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, int burst)
{
    uint32 min, max, cur_burst, flags = _BCM_XGS_METER_FLAG_PACKET_MODE;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN(bcm_tr2_cosq_port_bandwidth_get(unit, port, cosq,
                                          &min, &max, &cur_burst, &flags));
    /* Replace the current BURST setting, keep PPS the same */
    return bcm_tr2_cosq_port_bandwidth_set(unit, port, cosq, min, max,
                                           burst, flags);
}

int
bcm_tr2_cosq_port_burst_get(int unit, bcm_port_t port,
                            bcm_cos_queue_t cosq, int *burst)
{
    uint32 min, max, cur_burst, flags = _BCM_XGS_METER_FLAG_PACKET_MODE;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(bcm_tr2_cosq_port_bandwidth_get(unit, port, cosq,
                                          &min, &max, &cur_burst, &flags));
    *burst = cur_burst;
    return BCM_E_NONE;
}

int
_bcm_tr2_cosq_port_resolve(int unit, bcm_gport_t gport,
                           bcm_module_t *modid, bcm_port_t *port,
                           bcm_trunk_t *trunk_id, int *id)
{
    bcm_module_t mod_in;
    bcm_port_t port_in;

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        mod_in = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0xff;
        port_in = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
        *id = BCM_GPORT_SCHEDULER_GET(gport);
        *trunk_id = -1;

        /* Since the modid/port in the scheduler GPORT is the
         * applications modid/port space, convert to local space.
         */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in, modid, port));

        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (!SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], *port)) {
            return BCM_E_BADID;
        } else if (_tr2_num_port_cosq[unit][*port] == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_BADID;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_resolve_mod_port(int unit, bcm_gport_t gport,
                               bcm_module_t *modid, bcm_port_t *port,
                               bcm_trunk_t *trunk_id)
{
    int gport_id, result;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, modid,
                                port, trunk_id, &gport_id));
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, *modid, &result)); 

    if (TRUE != result) {
        return BCM_E_PORT;
    }
    return BCM_E_NONE;
}

STATIC int     
_bcm_tr2_cosq_discard_cap_enable_set(int unit, bcm_port_t port, 
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg, 
                                    uint32 flags)
{
    uint32 val, addr;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    if (flags & BCM_COSQ_DISCARD_CAP_AVERAGE) {
        soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 1);
    } else {
        soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 0);
    }
    if (flags & BCM_COSQ_DISCARD_ENABLE) {
        soc_reg_field_set(unit, config_reg, &val, ENABLEf, 1);
    } else {
        soc_reg_field_set(unit, config_reg, &val, ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_discard_cap_enable_get(int unit, bcm_port_t port, 
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg, 
                                    uint32 *flags)
{
    uint32 val, addr;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    if (soc_reg_field_get(unit, config_reg, val, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if (soc_reg_field_get(unit, config_reg, val, ENABLEf)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    return BCM_E_NONE;
}

int
bcm_tr2_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    soc_reg_t config_reg = WREDCONFIG_PACKETr;

    if (flags & ~(BCM_COSQ_DISCARD_CAP_AVERAGE |
                  BCM_COSQ_DISCARD_ENABLE)) {
        return BCM_E_PARAM;
    }

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        for (cosq = 0; cosq < 8; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_cap_enable_set(unit, port, cosq,
                                                     config_reg, flags));
        }
        if (SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], port)) {
            for (cosq = 8; cosq < 24; cosq++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_set(unit, port, cosq,
                                                         config_reg, flags));
            }
        }
    }
    return BCM_E_NONE;
}

int
bcm_tr2_cosq_discard_get(int unit, uint32 *flags)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    soc_reg_t config_reg = WREDCONFIG_PACKETr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        *flags = 0;
        rv = _bcm_tr2_cosq_discard_cap_enable_get(unit, port, 0,
                                                 config_reg, flags);
        break;
    }
    return rv;
}

/*      
 *  Convert HW drop probability to percent value
 */     
STATIC int       
_bcm_tr2_hw_drop_prob_to_percent[] = {
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
_bcm_tr2_percent_to_drop_prob(int percent) {
   int i;

   for (i=14; i > 0 ; i--) {
      if (percent >= _bcm_tr2_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_tr2_drop_prob_to_percent(int drop_prob) {
   return (_bcm_tr2_hw_drop_prob_to_percent[drop_prob]);
}   

STATIC int 
_bcm_tr2_angle_to_packets_table[] =
{
    /*  0.. 5 */  16383, 5727, 2862, 1908, 1430, 1142,
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
 * Given a slope (angle in degrees) from 0 to 90, return
 * the number of packets in the range from 0% drop
 * probability to 100% drop probability.
 */
STATIC int
_bcm_tr2_angle_to_packets(int angle) {
    return (_bcm_tr2_angle_to_packets_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_tr2_packets_to_angle(int packets) {
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_tr2_angle_to_packets_table[angle]) {
            break;
        }
    }
    return angle;
}

STATIC int     
_bcm_tr2_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
                         bcm_cos_queue_t cosq, soc_reg_t config_reg,
                         soc_reg_t thresh_reg_green,
                         soc_reg_t thresh_reg_yellow,
                         soc_reg_t thresh_reg_red,
                         soc_reg_t thresh_reg_non_tcp,
                         uint32 min_thresh, uint32 max_thresh,
                         int drop_probability, int gain)
{
    uint32 val, addr;

    if (flags & ~(BCM_COSQ_DISCARD_COLOR_GREEN  | 
                  BCM_COSQ_DISCARD_COLOR_YELLOW |
                  BCM_COSQ_DISCARD_COLOR_RED    |
                  BCM_COSQ_DISCARD_COLOR_ALL    |
                  BCM_COSQ_DISCARD_CAP_AVERAGE  |
                  BCM_COSQ_DISCARD_PACKETS      |
                  BCM_COSQ_DISCARD_BYTES        |
                  BCM_COSQ_DISCARD_ENABLE       |
                  BCM_COSQ_DISCARD_NONTCP)) {
        return BCM_E_PARAM;
    }

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    /* Program the weight */
    soc_reg_field_set(unit, config_reg, &val, WEIGHTf, gain);

    /* Program the drop probabilty */
    if ((flags & BCM_COSQ_DISCARD_COLOR_GREEN) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, MAXDROPRATEf,
            _bcm_tr2_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_YELLOW) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, YELLOW_MAXDROPRATEf,
            _bcm_tr2_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_RED) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, RED_MAXDROPRATEf,
            _bcm_tr2_percent_to_drop_prob(drop_probability));
    }
    if (flags & BCM_COSQ_DISCARD_NONTCP) {
        soc_reg_field_set(unit, config_reg, &val, NONTCP_MAXDROPRATEf,
            _bcm_tr2_percent_to_drop_prob(drop_probability));
    }
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));

    /* Program the min and max thresholds */
    if ((flags & BCM_COSQ_DISCARD_COLOR_GREEN) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_green, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_green, &val, DROPSTARTPOINTf,
                          min_thresh);
        soc_reg_field_set(unit, thresh_reg_green, &val, DROPENDPOINTf,
                          max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_YELLOW) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_yellow, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_yellow, &val,
                          YELLOW_DROPSTARTPOINTf, min_thresh);
        soc_reg_field_set(unit, thresh_reg_yellow, &val,
                          YELLOW_DROPENDPOINTf, max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_RED) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        addr = soc_reg_addr(unit, thresh_reg_red, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_red, &val,
                          RED_DROPSTARTPOINTf, min_thresh);
        soc_reg_field_set(unit, thresh_reg_red, &val,
                          RED_DROPENDPOINTf, max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }
    if (flags & BCM_COSQ_DISCARD_NONTCP) {
        addr = soc_reg_addr(unit, thresh_reg_non_tcp, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, thresh_reg_non_tcp, &val,
                          NONTCP_DROPSTARTPOINTf, min_thresh);
        soc_reg_field_set(unit, thresh_reg_non_tcp, &val,
                          NONTCP_DROPENDPOINTf, max_thresh);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }

    return BCM_E_NONE;
}

STATIC int     
_bcm_tr2_cosq_discard_get(int unit, bcm_port_t port, uint32 flags,
                         bcm_cos_queue_t cosq, soc_reg_t config_reg,
                         soc_reg_t thresh_reg_green,
                         soc_reg_t thresh_reg_yellow,
                         soc_reg_t thresh_reg_red,
                         soc_reg_t thresh_reg_non_tcp,
                         uint32 *min_thresh, uint32 *max_thresh,
                         int *drop_probability, int *gain)
{
    uint32 val, addr, drop_rate;

    addr = soc_reg_addr(unit, config_reg, port, cosq);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));

    /* Get the weight */
    *gain = soc_reg_field_get(unit, config_reg, val, WEIGHTf);

    /* Get the drop probabilty */
    if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      YELLOW_MAXDROPRATEf);
    } else if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      RED_MAXDROPRATEf);
    } else {
        drop_rate = soc_reg_field_get(unit, config_reg, val,
                                      MAXDROPRATEf);
    }
    *drop_probability = _bcm_tr2_drop_prob_to_percent(drop_rate);

    /* Get the min and max thresholds */
    if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
        addr = soc_reg_addr(unit, thresh_reg_yellow, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_yellow, val,
                                        YELLOW_DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_yellow, val,
                                        YELLOW_DROPENDPOINTf);
    } else if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
        addr = soc_reg_addr(unit, thresh_reg_red, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_red, val,
                                        RED_DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_red, val,
                                        RED_DROPENDPOINTf);
    } else {
        addr = soc_reg_addr(unit, thresh_reg_green, port, cosq);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        *min_thresh = soc_reg_field_get(unit, thresh_reg_green, val, 
                                        DROPSTARTPOINTf);
        *max_thresh = soc_reg_field_get(unit, thresh_reg_green, val, 
                                        DROPENDPOINTf);
    }
    return BCM_E_NONE;
}

#define ABS(n) (((n) < 0) ? -(n) : (n))

int
bcm_tr2_cosq_discard_port_set(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int drop_start,
                             int drop_slope,
                             int average_time)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    bcm_pbmp_t pbmp;
    int i, gain, cosq_start, num_cosq;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval, bits;

    if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_resolve_mod_port(unit, port, &modid,
                                                &local_port, &trunk_id));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, port, &local_port));
        }
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (IS_CPU_PORT(unit, port) || !SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            local_port = port;
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        if (_tr2_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < -1 || cosq >= _tr2_num_port_cosq[unit][local_port]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq_start = 8;
            num_cosq = _tr2_num_port_cosq[unit][local_port];
        } else {
            cosq_start = cosq + 8;
            num_cosq = 1;
        }
    } else {
        if (cosq < -1 || cosq >= _tr2_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq_start = 0;
            num_cosq = 8;
        } else {
            cosq_start = cosq;
            num_cosq = 1;
        }
    }

    /*
     * average queue size is reculated every 4us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     * gain = log2(average_time / 4)
     */
    bits = (average_time / 4) & 0xffff;
    bits |= bits >> 1;
    bits |= bits >> 2;
    bits |= bits >> 4;
    bits |= bits >> 8;
    gain = _shr_popcount(bits);
    /* Don't allow negative value of gain */
    if (gain) {
        gain -= 1;
    }

    /*
     * per-port packet limit may be disabled.
     * per-port per-cos packet limit may be set to use dynamic method.
     * therefore drop_start percentage is based on per-device total shared
     * packets.
     */
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, &rval));
    shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_PACKETr,
                                     rval, OP_BUFFER_SHARED_LIMIT_PACKETf);
    min_thresh = drop_start * shared_limit / 100;

    /* Calculate the max threshold. For a given slope (angle in 
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_tr2_angle_to_packets(drop_slope);
    if (max_thresh > TR2_PACKET_FIELD_MAX) {
        max_thresh = TR2_PACKET_FIELD_MAX;
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        for (i = cosq_start; i < (cosq_start + num_cosq); i++) { 
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_set(unit, local_port, color, i,
                                           WREDCONFIG_PACKETr,
                                           WREDPARAM_PACKETr,
                                           WREDPARAM_YELLOW_PACKETr,
                                           WREDPARAM_RED_PACKETr,
                                           WREDPARAM_NONTCP_PACKETr,
                                           min_thresh, max_thresh, 100, gain));
        }
    }
    return BCM_E_NONE;
}

int
bcm_tr2_cosq_discard_port_get(int unit, bcm_port_t port,
                             bcm_cos_queue_t cosq,
                             uint32 color,
                             int *drop_start,
                             int *drop_slope,
                             int *average_time)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval;

    if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_resolve_mod_port(unit, port, &modid,
                                                &local_port, &trunk_id));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, port, &local_port));
        }
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (IS_CPU_PORT(unit, port) || !SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            local_port = port;
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        if (_tr2_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < -1 || cosq >= _tr2_num_port_cosq[unit][local_port]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq = 8;
        } else {
            cosq = cosq + 8;
        }
    } else {
        if (cosq < -1 || cosq >= _tr2_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq = 0;
        }
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_discard_get(unit, local_port, color, cosq,
                                       WREDCONFIG_PACKETr,
                                       WREDPARAM_PACKETr,
                                       WREDPARAM_YELLOW_PACKETr,
                                       WREDPARAM_RED_PACKETr,
                                       WREDPARAM_NONTCP_PACKETr,
                                       &min_thresh, &max_thresh, &drop_prob,
                                       &gain));

        /*
         * average queue size is reculated every 4us, the formula is
         * avg_qsize(t + 1) =
         *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
         */
        *average_time = (1 << gain) * 4;

        /*
         * per-port packet limit may be disabled.
         * per-port per-cos packet limit may be set to use dynamic method.
         * therefore drop_start percentage is based on per-device total shared
         * packets.
         */
        SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_PACKETr(unit, &rval));
        shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_PACKETr,
                                         rval, OP_BUFFER_SHARED_LIMIT_PACKETf);
        if (min_thresh >= shared_limit) {
            *drop_start = 100;
        } else {
            *drop_start = min_thresh * 100 / shared_limit;
        }

        /* Calculate the slope using the min and max threshold.
         * The angle is calculated knowing drop probability at min
         * threshold is 0% and drop probability at max threshold is 100%.
         */
        *drop_slope = _bcm_tr2_packets_to_angle(max_thresh - min_thresh);
        break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_port_sched_set(int unit, soc_reg_t config_reg,
                            soc_reg_t weight_reg, soc_reg_t minsp_reg,
                            bcm_port_t port, bcm_cos_queue_t start_cosq,
                            int num_weights, const int weights[],
                            int mode)
{
    int t, i;
    uint32 cfg_addr, cosw_addr, minsp_addr, cfg, cosw, minsp;
    int mbits = 0;
    int weight_max, sel, quanta;

    cfg_addr = soc_reg_addr(unit, config_reg, port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, cfg_addr, &cfg));

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
        for(i = 0; i < num_weights; i++) {
            if (weights[i] > TR2_WRR_WEIGHT_MAX) {
                return BCM_E_PARAM;
            }
        }
        for (i = 0; i < num_weights; i++) {
            cosw_addr = soc_reg_addr(unit, weight_reg, port, start_cosq + i);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, cosw_addr, &cosw));
            soc_reg_field_set(unit, weight_reg, &cosw, COSWEIGHTSf,
                              weights[i]);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, cosw_addr, cosw));
        }
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mbits = 3;

        weight_max = 0;
        for (i = 0; i < num_weights; i++) {
            if (weight_max < weights[i]) {
                weight_max = weights[i];
            }
        }

        for (sel = 0; sel <= 3; sel++) {
            if (weight_max <= TR2_DRR_WEIGHT_MAX * (1 << (sel + 1))) {
                break;
            }
        }
        if (sel > 3) {
            return BCM_E_PARAM;
        }
        quanta = 1 << (sel + 1);
        soc_reg_field_set(unit, config_reg, &cfg, MTU_QUANTA_SELECTf, sel);
        for (i = 0; i < num_weights; i++) {
            cosw_addr = soc_reg_addr(unit, weight_reg, port, start_cosq + i);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, cosw_addr, &cosw));
            /* the unit of weight for WDRR is specified mtu quanta */
            soc_reg_field_set(unit, weight_reg, &cosw, COSWEIGHTSf,
                              (weights[i] + quanta - 1) / quanta);
            SOC_IF_ERROR_RETURN(soc_reg32_write(unit, cosw_addr, cosw));
        }
        break;
    case BCM_COSQ_BOUNDED_DELAY:        /* not supported in xgs */
    default:
        return BCM_E_PARAM;
    }

    if ((mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN) ||
        (mode == BCM_COSQ_DEFICIT_ROUND_ROBIN)) {
        /* Set strict priority bit if weight is zero */
        minsp_addr = soc_reg_addr(unit, minsp_reg, port, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_read(unit, minsp_addr, &minsp));
        for(i = 0; i < num_weights; i++) {
            t = start_cosq + i;
            if (weights[i] == 0) {
                minsp |= (1 << t);
            } else {
                minsp &= ~(1 << t);
            }
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_write(unit, minsp_addr, minsp));
    }

    /* Program the scheduling mode */
    soc_reg_field_set(unit, config_reg, &cfg, SCHEDULING_SELECTf, mbits);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, cfg_addr, cfg));

    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_port_sched_get(int unit, soc_reg_t config_reg,
                            soc_reg_t weight_reg, bcm_port_t port,
                            bcm_cos_queue_t start_cosq,
                            int num_weights, int weights[], int *mode)
{
    uint32 addr, cfg, cosw;
    int mbits, i;
    uint32 quanta;

    addr = soc_reg_addr(unit, config_reg, port, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &cfg));
    mbits = soc_reg_field_get(unit, config_reg, cfg, SCHEDULING_SELECTf);

    switch (mbits) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        for (i = 0; i < num_weights; i++) {
            addr = soc_reg_addr(unit, weight_reg, port, start_cosq + i);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &cosw));
            weights[i] = soc_reg_field_get(unit, weight_reg, cosw,
                                           COSWEIGHTSf);
        }
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        switch (soc_reg_field_get(unit, config_reg, cfg, MTU_QUANTA_SELECTf)) {
        case 0: /* quanta is 2k bytes */
            quanta = 2;
            break;
        case 1: /* quanta is 4k bytes */
            quanta = 4;
            break;
        case 2: /* quanta is 8k bytes */
            quanta = 8;
            break;
        case 3: /* quanta is 16k bytes */
            quanta = 16;
            break;
        default:
            return SOC_E_INTERNAL;
        }
        for (i = 0; i < num_weights; i++) {
            addr = soc_reg_addr(unit, weight_reg, port, start_cosq + i);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &cosw));
            /* the unit of weight for WDRR is specified mtu quanta */
            weights[i] = soc_reg_field_get(unit, weight_reg, cosw,
                                           COSWEIGHTSf) * quanta;
        }
        break;
    default:
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_port_sched_set
 * Purpose:
 *      Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      pbm - port bitmap
 *      mode - Scheduling mode, one of BCM_COSQ_xxx
 *      weights - Weights for each COS queue
 *                Only for BCM_COSQ_WEIGHTED_FAIR_ROUND_ROBIN mode.
 *                For DRR Weight is specified in Kbytes
 *      delay - This parameter is not used 
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int weights[], int delay)
{
    bcm_port_t port;
    int        i, max_weight;

    mbcm_driver[unit]->mbcm_cosq_sched_weight_max_get(unit, mode, &max_weight);
    if((mode != BCM_COSQ_STRICT) && (mode != BCM_COSQ_ROUND_ROBIN) &&
             (max_weight != BCM_COSQ_WEIGHT_UNLIMITED)) {
        for(i = 0; i < NUM_COS(unit); i++) {
            if((weights[i] < 0) || (weights[i] > max_weight)) {
                return BCM_E_PARAM;
            }
        }
    }

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_port_sched_set(unit,
                                         ESCONFIGr, COSWEIGHTSr, MINSPCONFIGr,
                                         port, 0, 8, weights, mode));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_port_sched_get
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
bcm_tr2_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int weights[], int *delay)
{
    bcm_port_t port;

    PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_port_sched_get(unit, ESCONFIGr, COSWEIGHTSr,
                                         port, 0, 8, weights, mode));
        break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_add
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Physical port.
 *      numq - (IN) Number of COS queues.
 *      flags - (IN) Flags.
 *      gport - (IN/OUT) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */     
int 
bcm_tr2_cosq_gport_add(int unit, bcm_gport_t port, int numq,
                      uint32 flags, bcm_gport_t *gport)
{
    bcm_module_t modid, mod_out;
    bcm_port_t local_port, port_out;
    bcm_trunk_t trunk_id;
    int gport_id;

    if ((numq <= 0) || (numq > 16) ||
        (!BCM_GPORT_IS_LOCAL(port) && !BCM_GPORT_IS_MODPORT(port))) {
        return BCM_E_PARAM;
    }
    if (flags & (BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_OVERLAY | 
        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_MCAST_QUEUE |
        BCM_COSQ_GPORT_MCAST_QUEUE_GROUP | BCM_COSQ_GPORT_SUBSCRIBER |
        BCM_COSQ_GPORT_EGRESS_GROUP | BCM_COSQ_GPORT_DISABLE |
        BCM_COSQ_GPORT_CALENDAR)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, port, &modid,
                                       &local_port, &trunk_id));

    /* Verify that the specified port is for my modid and that 
     * it is capable of hierarchical queueing.
     */
        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (!SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], local_port)) {
            return BCM_E_PORT;
        } else if (_tr2_num_port_cosq[unit][local_port]) {
            return BCM_E_EXISTS;
        }
        /* Call delete routine to init HW settings */
        BCM_IF_ERROR_RETURN (_bcm_tr2_cosq_gport_delete(unit, local_port));

        _tr2_num_port_cosq[unit][local_port] = numq;

        /* The GPORT ID for SCHEDULER is the modid/port from application
         * space. This allows the GPORT ID to be used on remote module
         * API calls. Convert from local space to application space.
         */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    modid, local_port, &mod_out, &port_out));
        gport_id = (mod_out << 8) | port_out;
        BCM_GPORT_SCHEDULER_SET(*gport, gport_id);
    return BCM_E_NONE;
}
 
STATIC int
_bcm_tr2_cosq_gport_delete(int unit, bcm_port_t port)
{
    int cosq, weights[16];
    uint32 val;

    /* Detach from physical port */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, COS_MODEr, port, SELECTf, 0x0));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_COS_MODEr, port, SELECTf, 0x0));

    /* Clear bandwidth of all Stage1 COSQs */
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN
            (bcm_tr2_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
    }

    /* Clear bandwidth of S1 output queue */
    cosq = 24;
    BCM_IF_ERROR_RETURN
        (bcm_tr2_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));

    /* Clear weights of all Stage1 COSQs */
    for (cosq = 0; cosq < 16; cosq++) {
        weights[cosq] = 0;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_port_sched_set(unit, S1V_CONFIGr, S1V_COSWEIGHTSr,
                                     S1V_MINSPCONFIGr, port, 0, 16,
                                     weights, BCM_COSQ_WEIGHTED_ROUND_ROBIN));

    /* Clear weight for S1 output queue */
    cosq = 8;
    BCM_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, port, cosq, &val));
    soc_reg_field_set(unit, COSWEIGHTSr, &val, COSWEIGHTSf, 0);
    BCM_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, port, cosq, val));

    /* Clear discard settings for all Stage1 COSQs */
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_discard_set(unit, port, 
                                      BCM_COSQ_DISCARD_COLOR_ALL | 
                                      BCM_COSQ_DISCARD_NONTCP, cosq,
                                      WREDCONFIG_PACKETr,
                                      WREDPARAM_PACKETr,
                                      WREDPARAM_YELLOW_PACKETr,
                                      WREDPARAM_RED_PACKETr,
                                      WREDPARAM_NONTCP_PACKETr,
                                      TR2_PACKET_FIELD_MAX, 
                                      TR2_PACKET_FIELD_MAX, 100, 0));
    }

    _tr2_num_port_cosq[unit][port] = 0;

    return BCM_E_NONE;
}

/*      
 * Function:
 *      bcm_cosq_gport_delete
 * Purpose:
 *      
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));
    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (_tr2_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else if(BCM_GPORT_IS_LOCAL(gport) ||
                BCM_GPORT_IS_MODPORT(gport)) {
        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }    
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN (_bcm_tr2_cosq_gport_delete(unit, local_port));
    return BCM_E_NONE;
}
    
/*
 * Function:
 *      bcm_cosq_gport_traverse
 * Purpose:
 *      Walks through the valid COSQ GPORTs and calls
 *      the user supplied callback function for each entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function.
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int     
bcm_tr2_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                            void *user_data)
{
    bcm_module_t my_modid, mod_out;
    bcm_port_t port, port_out;
    bcm_gport_t gport, sched_gport;
    int gport_id;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = SOC_E_NONE;

    if (_tr2_num_port_cosq[unit]) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        BCM_PBMP_ITER(_tr2_cosq_24q_ports[unit], port) {
            if (_tr2_num_port_cosq[unit][port]) {

                /* Construct physical port GPORT ID */
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            my_modid, port, &mod_out, &port_out));
                BCM_GPORT_MODPORT_SET(gport, mod_out, port_out);

                /* Construct scheduler GPORT ID */
                gport_id = (mod_out << 8) | port_out;
                BCM_GPORT_SCHEDULER_SET(sched_gport, gport_id);

                /* Call application call-back */
                rv = cb(unit, gport, _tr2_num_port_cosq[unit][port],
                        flags, sched_gport, user_data);
                if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
                    if (SOC_CB_ABORT_ON_ERR(unit)) {
                        return rv;
                    }
#endif
                }
            }
        } 
    } else {
        return BCM_E_INIT;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr2_cosq_gport_bandwidth_port_resolve(int unit, bcm_gport_t gport,
                                           bcm_cos_queue_t cosq,
                                           bcm_port_t *local_port,
                                           bcm_cos_queue_t *cosq_start,
                                           bcm_cos_queue_t *cosq_end)
{
    bcm_module_t modid;
    bcm_trunk_t trunk_id;

    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_resolve_mod_port(unit, gport, &modid,
                                            local_port, &trunk_id));
    } else {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
    }

    if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        if (_tr2_num_port_cosq[unit][*local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < 0) {
            *cosq_start = 8;
            *cosq_end = 23;
        } else if (cosq >= _tr2_num_port_cosq[unit][*local_port]) {
            return BCM_E_PARAM;
        } else {
            *cosq_start = *cosq_end = (cosq + 8);
        }
    } else if (cosq == TR2_SCHEDULER_COSQ) {
        if (!SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], *local_port)) {
            return BCM_E_PORT;
        }
        /* Output of S1 scheduler is configured using register offset 24 */
        *cosq_start = *cosq_end = 24;
    } else {
        if (IS_CPU_PORT(unit, *local_port)) {
            if (cosq >=  NUM_CPU_COSQ(unit)) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                *cosq_start = 0;
                *cosq_end =  NUM_CPU_COSQ(unit) - 1 ; 
            } else {
                *cosq_start = *cosq_end = cosq;
            }
        } else {
            if (cosq >= _tr2_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                *cosq_start = 0;
                *cosq_end = 7;
            } else {
                *cosq_start = *cosq_end = cosq;
            }
        }
    }
    return BCM_E_NONE;
}

/*  
 * Function:
 *      bcm_cosq_gport_bandwidth_set
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
bcm_tr2_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    bcm_port_t local_port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                    &cosq_start, &cosq_end));

    flags &= ~_BCM_XGS_METER_FLAG_PACKET_MODE;
    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_tr2_cosq_port_bandwidth_set(unit, local_port, i,
                                             kbits_sec_min,
                                             kbits_sec_max,
                                             kbits_sec_max,
                                             flags));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_bandwidth_get
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
bcm_tr2_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    bcm_port_t local_port;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_burst;    /* Dummy variable */

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                    &cosq_start, &cosq_end));
    *kbits_sec_min = *kbits_sec_max = *flags = 0;

    BCM_IF_ERROR_RETURN
        (bcm_tr2_cosq_port_bandwidth_get(unit, local_port, cosq_start,
                                         kbits_sec_min, kbits_sec_max,
                                         &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*  
 * Function:
 *      bcm_tr2_cosq_gport_bandwidth_burst_set
 * Purpose:
 *       
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_burst_min - (IN) minimum burst, kbits.
 *      kbits_burst_max - (IN) maximum burst, kbits.
 * Returns:
 *      BCM_E_XXX 
 */ 
int
bcm_tr2_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 kbits_burst_min,
                                       uint32 kbits_burst_max)
{
    bcm_port_t local_port, port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_min, kbits_sec_max, flags = 0;
    uint32 kbits_sec_burst; /* Dummy variable */

    if (gport < 0) {
        PBMP_ALL_ITER(unit, port) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, port, cosq,
                                                           &local_port,
                                                           &cosq_start, &cosq_end));
            for (i = cosq_start; i <= cosq_end; i++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_bucket_get(unit, local_port, i,
                                              &kbits_sec_min, &kbits_sec_max,
                                              &kbits_sec_burst, &kbits_sec_burst,
                                              &flags));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_bucket_set(unit, local_port, i,
                                                    kbits_sec_min,
                                                    kbits_sec_max,
                                                    kbits_burst_min,
                                                    kbits_burst_max,
                                                    flags));
            }
        }
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                    &local_port,
                                                   &cosq_start, &cosq_end));

    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_bucket_get(unit, local_port, i,
                                      &kbits_sec_min, &kbits_sec_max,
                                      &kbits_sec_burst, &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_bucket_set(unit, local_port, i,
                                      kbits_sec_min,
                                      kbits_sec_max,
                                      kbits_burst_min,
                                      kbits_burst_max,
                                      flags));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_cosq_gport_bandwidth_burst_get
 * Purpose:
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) GPORT ID.
 *      cosq - (IN) COS queue to configure, -1 for all COS queues.
 *      kbits_burst_min - (OUT) minimum burst, kbits.
 *      kbits_burst_max - (OUT) maximum burst, kbits.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    bcm_port_t local_port, port;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_min, kbits_sec_max, flags;    /* Dummy variables */

    if (gport < 0) {
        port = SOC_PORT_MIN(unit,all);

        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, port, cosq,
                                                       &local_port,
                                                       &cosq_start, &cosq_end));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                       &local_port,
                                                       &cosq_start, &cosq_end));
    }

    kbits_sec_min = kbits_sec_max = flags = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_bucket_get(unit, local_port, cosq_start,
                                  &kbits_sec_min, &kbits_sec_max,
                                  kbits_burst_min, kbits_burst_max, &flags));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_sched_set
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
bcm_tr2_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i;
    bcm_cos_queue_t cosq_start = 0;
    int num_weights = 1, weights[16];
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;
    soc_reg_t minsp_reg = MINSPCONFIGr;

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));

        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_tr2_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _tr2_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 16;
                for (i = 0; i < num_weights; i++) {
                    if (i < _tr2_num_port_cosq[unit][local_port]) {
                        weights[i] = weight;
                    } else {
                        weights[i] = 0;
                    }
                }
            } else {
                cosq_start = cosq;
                num_weights = 1;
                weights[0] = weight;
            }
            config_reg = S1V_CONFIGr;
            weight_reg = S1V_COSWEIGHTSr;
            minsp_reg = S1V_MINSPCONFIGr;
        } else if (cosq == TR2_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq_start = 8;
            num_weights = 1;
            weights[0] = weight;
        } else {
            if (cosq >= _tr2_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 8;
                for (i = 0; i < num_weights; i++) {
                    if (i < _tr2_num_cosq[unit]) {
                        weights[i] = weight;
                    } else {
                        weights[i] = 0;
                    }
                }
            } else {
                cosq_start = cosq;
                num_weights = 1;
                weights[0] = weight;
            }
        }

        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_port_sched_set(unit, config_reg, weight_reg, minsp_reg,
                                         local_port, cosq_start, num_weights,
                                         weights, mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_sched_get
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
bcm_tr2_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight)
{   
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;

    *mode = *weight = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));
        if (!_tr2_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_tr2_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _tr2_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
            config_reg = S1V_CONFIGr;
            weight_reg = S1V_COSWEIGHTSr;
        } else if (cosq == TR2_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_tr2_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq = 8;
        } else {
            if (cosq >= _tr2_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_port_sched_get(unit, config_reg, weight_reg, 
                                         local_port, cosq, 1,
                                         weight, mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_discard_set
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to configure
 *      discard - (IN) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_gport_discard_set(int unit, bcm_gport_t port,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;

    if ((discard == NULL) || 
        (discard->gain < 0) || (discard->gain > 15) ||
        (discard->drop_probability < 0) || (discard->drop_probability > 100)) {
        return BCM_E_PARAM;
    }

    cell_size      = _BCM_TR2_COSQ_CELLSIZE;
    cell_field_max = TR2_CELL_FIELD_MAX;

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
        if ((min_thresh > TR2_PACKET_FIELD_MAX) || 
            (max_thresh > TR2_PACKET_FIELD_MAX)) {
            return BCM_E_PARAM;
        }
    }

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_set(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_CELLr,
                                              PORT_WREDPARAM_CELLr,
                                              PORT_WREDPARAM_YELLOW_CELLr,
                                              PORT_WREDPARAM_RED_CELLr,
                                              PORT_WREDPARAM_NONTCP_CELLr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         discard->flags));

            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_set(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_PACKETr,
                                              PORT_WREDPARAM_PACKETr,
                                              PORT_WREDPARAM_YELLOW_PACKETr,
                                              PORT_WREDPARAM_RED_PACKETr,
                                              PORT_WREDPARAM_NONTCP_PACKETr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_tr2_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _tr2_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else {
                    cosq = cosq + 8;
                }
            } else if (cosq >= _tr2_num_cosq[unit]) {
                return BCM_E_PARAM;
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_set(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_CELLr,
                                              WREDPARAM_CELLr,
                                              WREDPARAM_YELLOW_CELLr,
                                              WREDPARAM_RED_CELLr,
                                              WREDPARAM_NONTCP_CELLr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_set(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              min_thresh, max_thresh, 
                                              discard->drop_probability,
                                              discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_PACKETr, 
                                                     discard->flags));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_discard_get
 * Purpose:
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to get
 *      discard - (IN/OUT) Discard settings
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    uint32 min_thresh, max_thresh;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_get(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_CELLr,
                                              PORT_WREDPARAM_CELLr,
                                              PORT_WREDPARAM_YELLOW_CELLr,
                                              PORT_WREDPARAM_RED_CELLr,
                                              PORT_WREDPARAM_NONTCP_CELLr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_get(unit, local_port,
                                              discard->flags, 0,
                                              PORT_WREDCONFIG_PACKETr,
                                              PORT_WREDPARAM_PACKETr,
                                              PORT_WREDPARAM_YELLOW_PACKETr,
                                              PORT_WREDPARAM_RED_PACKETr,
                                              PORT_WREDPARAM_NONTCP_PACKETr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_tr2_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _tr2_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else {
                    cosq = cosq + 8;
                }
            } else if (cosq >= _tr2_num_cosq[unit]) {
                return BCM_E_PARAM;
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_get(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_CELLr,
                                              WREDPARAM_CELLr,
                                              WREDPARAM_YELLOW_CELLr,
                                              WREDPARAM_RED_CELLr,
                                              WREDPARAM_NONTCP_CELLr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_get(unit, local_port,
                                              discard->flags, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              &min_thresh, &max_thresh, 
                                              &discard->drop_probability,
                                              &discard->gain));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr2_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     &discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_PACKETr, 
                                                     &discard->flags));
        }
    }

    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert cells to bytes */
        min_thresh *= _BCM_TR2_COSQ_CELLSIZE;
        max_thresh *= _BCM_TR2_COSQ_CELLSIZE;
    }
    discard->min_thresh = min_thresh;
    discard->max_thresh = max_thresh;

    return BCM_E_NONE;
}

/* 
 * Function:
 *      bcm_cosq_gport_attach
 * Purpose:
 *      
 * Parameters: 
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to attach to.
 *      cosq       - (IN) COS queue to attach to.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PARAM;
    } else if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if (cosq < 0) {
        cosq = TR2_SCHEDULER_COSQ;
    } else if (cosq != TR2_SCHEDULER_COSQ) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));

        BCM_IF_ERROR_RETURN
            (_bcm_tr2_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                           &input_port, &trunk_id));

        if (_tr2_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        } else if ((sched_modid != input_modid) || (sched_port != input_port)) {
            /* Be sure the GPORT matches to the physical port */
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port, 
                                    SELECTf, 0x3)); /* VLAN_COS */
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MODEr, input_port, 
                                    SELECTf, 0x3)); /* VLAN_COS */
    return BCM_E_NONE;
}   

/*
 * Function:
 *      bcm_cosq_gport_detach
 * Purpose:
 * 
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (IN) GPORT to detach from.
 *      cosq       - (IN) COS queue to detach from.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int     
bcm_tr2_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PORT;
    } else if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if ((cosq > 0) && (cosq != TR2_SCHEDULER_COSQ)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));
       BCM_IF_ERROR_RETURN
           (_bcm_tr2_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                          &input_port, &trunk_id));
        if (_tr2_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        } else if ((sched_modid != input_modid) || (sched_port != input_port)) {
            /* Be sure the GPORT matches to the physical port */
            return BCM_E_PORT;
        }
    
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MODEr, input_port, SELECTf, 0x0));
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port, SELECTf, 0x0));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_cosq_gport_attach_get
 * Purpose:
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      sched_port - (IN) Scheduler GPORT ID.
 *      input_port - (OUT) GPORT attached to.
 *      cosq       - (OUT) COS queue attached to.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_tr2_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;
    uint32 val;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport) ||
        !cosq || !input_gport) {
        return BCM_E_PARAM;
    } else if (!_tr2_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr2_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));
        if (_tr2_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_COS_MODEr(unit, sched_port, &val));
        if (soc_reg_field_get(unit, COS_MODEr, val, SELECTf) == 3) {
            /* GPORT has been attached */
            *cosq = TR2_SCHEDULER_COSQ;

            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                        sched_modid, sched_port,
                                        &input_modid, &input_port));
            BCM_GPORT_MODPORT_SET(*input_gport, input_modid, input_port);
        } else {
            /* GPORT has not been attached */
            return BCM_E_NOT_FOUND;
        }
    return BCM_E_NONE;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_tr2_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr2_cosq_sw_dump(int unit)
{
    int  i;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));

    /* Number COSQ */
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), _tr2_num_cosq[unit]));

    /* Number of COSQs per port */
    LOG_CLI((BSL_META_U(unit,
                        "    Number COSQ per port:\n")));
    if (_tr2_num_port_cosq[unit]) {
        for (i = 0; i < TR2_PORT_COUNT; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "        Port %2d: %d\n"), i,
                     _tr2_num_port_cosq[unit][i]));
        }
    }
    
    /* COSQ Map profile */
    /* Display those entries with reference count > 0 */
    LOG_CLI((BSL_META_U(unit,
                        "    COSQ Map Profile:\n")));
    if (_tr2_cos_map_profile[unit] != NULL) {
        int     num_entries;
        int     ref_count;
        int     entries_per_set;
        uint32  index;
        uint32  cosq, hg_cosq;
        port_cos_map_entry_t *entry_p;

        num_entries = soc_mem_index_count
            (unit, _tr2_cos_map_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            "        Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "        Index RefCount EntriesPerSet - "
                 "COS HG_COS\n")));

        for (index = 0; index < num_entries;
             index += TR2_COS_MAP_ENTRIES_PER_SET) {
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               _tr2_cos_map_profile[unit],
                                               index, &ref_count))) {
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                entries_per_set =
                    _tr2_cos_map_profile[unit]->tables[0].entries[index+i].entries_per_set;
                LOG_CLI((BSL_META_U(unit,
                                    "       %5d %8d %13d    "),
                         index + i, ref_count, entries_per_set));

                entry_p = SOC_PROFILE_MEM_ENTRY(unit,
                                                _tr2_cos_map_profile[unit],
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
                                        "    %2d"), hg_cosq));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
        }
    }

    /* Egress COSQ Map profile */
    /* Display those entries with reference count > 0 */
    LOG_CLI((BSL_META_U(unit,
                        "    Egress COSQ Map Profile:\n")));
    if (_tr2_cos_egr_map_profile[unit] != NULL) {
        int     num_entries;
        int     ref_count;
        int     entries_per_set;
        uint32  index;
        uint32  cosq;
        egr_cos_map_entry_t *entry_p;

        num_entries = soc_mem_index_count
            (unit, _tr2_cos_egr_map_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            "        Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "        Index RefCount EntriesPerSet - "
                 "Entry: REDIRECT_COS\n")));

        for (index = 0; index < num_entries;
             index += TR2_COS_MAP_ENTRIES_PER_SET) {
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               _tr2_cos_egr_map_profile[unit],
                                               index, &ref_count))) {
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (i = 0; i < TR2_COS_MAP_ENTRIES_PER_SET; i++) {
                entries_per_set =
                    _tr2_cos_egr_map_profile[unit]->tables[0].entries[index+i].entries_per_set;
                LOG_CLI((BSL_META_U(unit,
                                    "       %5d %8d %13d     "),
                         index + i, ref_count, entries_per_set));

                entry_p = SOC_PROFILE_MEM_ENTRY(unit,
                                                _tr2_cos_egr_map_profile[unit],
                                                egr_cos_map_entry_t *,
                                                index + i);
                cosq = soc_mem_field32_get(unit,
                                           EGR_COS_MAPm,
                                           entry_p,
                                           REDIRECT_COSf);
                LOG_CLI((BSL_META_U(unit,
                                    "              %3d "), cosq));

                LOG_CLI((BSL_META_U(unit,
                                    "\n")));
            }
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_TRIUMPH2_SUPPORT */
typedef int _tr2_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIUMPH2_SUPPORT */

