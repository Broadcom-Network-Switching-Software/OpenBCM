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
#if defined(BCM_TRX_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#ifdef BCM_SCORPION_SUPPORT
#include <bcm_int/esw/scorpion.h>
#endif 
#include <bcm_int/esw/triumph.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TR_DRR_WEIGHT_MAX       0x7f
#define TR_WRR_WEIGHT_MAX       0x7f

#define TR_PACKET_FIELD_MAX(_u_)   (SOC_IS_ENDURO(_u_) ? (0x1fff) : (0x3fff))
#define TR_CELL_FIELD_MAX(_u_)   (SOC_IS_ENDURO(_u_) ? (0x3fff) : (0x7fff))

#define TR_PKT_REFRESH_MAX(_u_)   (SOC_IS_ENDURO(_u_) ? (0x3ffff) : (0xfffff))
#define TR_PKT_THD_MAX(_u_)   (SOC_IS_ENDURO(_u_) ? (0xfff) : (0x7ff))

/* COSQ where output of hierarchical COSQs scheduler is attached */
#define TR_SCHEDULER_COSQ       8

/* MMU cell size in bytes */
#define _BCM_TR_COSQ_CELLSIZE   128

#ifdef BCM_SCORPION_SUPPORT
#define SC_CELL_FIELD_MAX       0x3fff
#define _BCM_SC_COSQ_CELLSIZE   128
STATIC int _bcm_sc_cosq_discard_all_disable (int unit);
#endif


/* Cache of COS_MAP Profile Table */
#define TR_COS_MAP_ENTRIES_PER_SET    16 /* COS Map profile entries per set */
STATIC soc_profile_mem_t *_tr_cos_map_profile[BCM_MAX_NUM_UNITS] = {NULL};

/* Number of COSQs configured for this device */
STATIC int _tr_num_cosq[SOC_MAX_NUM_DEVICES];

/* Port Bitmap which includes ports capable of hierarchical queueing */
STATIC soc_pbmp_t _tr_cosq_24q_ports[SOC_MAX_NUM_DEVICES];

/* Array to keep track of the number of hierarchical COSQs added per port */
STATIC uint8 *_tr_num_port_cosq[SOC_MAX_NUM_DEVICES] = {NULL};

/* Forward declarations */
STATIC int _bcm_tr_cosq_gport_delete(int unit, bcm_port_t port);
STATIC int _bcm_tr_cosq_port_sched_set(int unit, soc_reg_t config_reg,
                                       soc_reg_t weight_reg, soc_reg_t minsp_reg,
                                       bcm_port_t port, bcm_cos_queue_t start_cosq,
                                       int num_weights, const int weights[], 
                                       int mode);
STATIC int _bcm_tr_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
                                    bcm_cos_queue_t cosq, soc_reg_t config_reg,
                                    soc_reg_t thresh_reg_green,
                                    soc_reg_t thresh_reg_yellow,
                                    soc_reg_t thresh_reg_red,
                                    soc_reg_t thresh_reg_non_tcp,
                                    uint32 min_thresh, uint32 max_thresh,
                                    int drop_probability, int gain);

/*
 * Function:
 *      _bcm_tr_cosw_port_count_get
 * Purpose:
 *      Returns number of ports in device.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_cosq_port_count_get(int unit)
{
    int port_count = 44;

#ifdef BCM_ENDURO_SUPPORT
    if(SOC_IS_ENDURO (unit)) {
        port_count = 30;
    }
#endif /* BCM_ENDURO_SUPPORT */

    return port_count;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      bcm_tr_cosq_sync
 * Purpose:
 *      Record COSQ module persistent info for Level 2 Warm Boot.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_cosq_sync(int unit)
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
    num_cosq = _tr_num_cosq[unit];
    sal_memcpy(cosq_scache_ptr, &num_cosq, sizeof(num_cosq));
    cosq_scache_ptr += sizeof(num_cosq);
        
    /* Number of hierarchical COSQs added per port */
    if (_tr_num_port_cosq[unit] != NULL) {
        sal_memcpy(cosq_scache_ptr, _tr_num_port_cosq[unit],
                   sizeof(uint8) * _bcm_tr_cosq_port_count_get(unit));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_cosq_reinit
 * Purpose:
 *      Recover COSQ software state.
 * Parameters:
 *      unit - Unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_cosq_reinit(int unit)
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
        _tr_num_cosq[unit] = num_cosq;
        cosq_scache_ptr += sizeof(num_cosq);

        /* Number of hierarchical COSQs added per port */
        if (_tr_num_port_cosq[unit] != NULL) {
            sal_memcpy(_tr_num_port_cosq[unit], cosq_scache_ptr,
                       sizeof(uint8) * _bcm_tr_cosq_port_count_get(unit));
        }

    } else {
        _tr_num_cosq[unit] = _bcm_esw_cosq_config_property_get(unit);
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
bcm_tr_cosq_init(int unit)
{
    STATIC int _tr_max_cosq = -1;
    int num_cos;
    int port_count;
#ifdef BCM_WARM_BOOT_SUPPORT
    int                 rv;
    soc_scache_handle_t scache_handle;
    uint32              cosq_scache_size;
    uint8               *cosq_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
    STATIC int _sc_max_cosq = -1;
    if (SOC_IS_SC_CQ(unit)) {
        if (_sc_max_cosq < 0) {
            _sc_max_cosq = NUM_COS(unit);
            NUM_COS(unit) = 8;
        }
    } else 
#endif
    if (_tr_max_cosq < 0) {
        _tr_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN (bcm_tr_cosq_detach(unit, 0));
    }

    num_cos = _bcm_esw_cosq_config_property_get(unit);

    port_count = _bcm_tr_cosq_port_count_get(unit);
    if (SOC_IS_TR_VL(unit)) {
        SOC_PBMP_CLEAR(_tr_cosq_24q_ports[unit]);
        /* Keep track of ports that support 24 (16 + 8) queues */
#ifdef BCM_ENDURO_SUPPORT
        if(SOC_IS_ENDURO (unit)) {
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 26);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 27);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 28);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 29);            
        } else
#endif /* BCM_ENDURO_SUPPORT */
        {
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 2);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 3);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 14);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 15);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 26);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 27);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 28);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 29);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 30);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 31);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 32);
            SOC_PBMP_PORT_ADD(_tr_cosq_24q_ports[unit], 43);
        }

        if (_tr_num_port_cosq[unit] == NULL) {
            _tr_num_port_cosq[unit] = sal_alloc(sizeof(uint8) * port_count,
                                         "_tr_num_port_cosq");
        }
        sal_memset(_tr_num_port_cosq[unit], 0, sizeof(uint8) * port_count);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Warm boot level 2 cache size */
    cosq_scache_size = sizeof(uint32); /* Number COSQ */
    if (_tr_num_port_cosq[unit] != NULL) {  /* Number of COSQs per port */
        cosq_scache_size += (sizeof(uint8) * port_count);
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
        BCM_IF_ERROR_RETURN(_bcm_tr_cosq_reinit(unit));
        num_cos = _tr_num_cosq[unit];
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return bcm_tr_cosq_config_set(unit, num_cos);
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
bcm_tr_cosq_detach(int unit, int software_state_only)
{
    bcm_port_t port;
    int weights[8];
    int cosq;

    if (SOC_IS_TR_VL(unit))
    {
        /* Delete any gports that were added */

        if (_tr_num_port_cosq[unit])
        {
            if (!software_state_only)
            {
                BCM_PBMP_ITER(_tr_cosq_24q_ports[unit], port)
                {
                    if (_tr_num_port_cosq[unit][port])
                    {
                        BCM_IF_ERROR_RETURN
                            (_bcm_tr_cosq_gport_delete(unit, port));
                    }
                }
            }

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_destroy(unit, _tr_cos_map_profile[unit]));
            sal_free(_tr_cos_map_profile[unit]);
            _tr_cos_map_profile[unit] = NULL;

            sal_free(_tr_num_port_cosq[unit]);
            _tr_num_port_cosq[unit] = NULL;
        }
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

    #ifdef BCM_SCORPION_SUPPORT
            if (SOC_IS_SC_CQ(unit)) {
                /* Clear bandwidth settings on port */
                for (cosq = 0; cosq < 8; cosq++) {
                     BCM_IF_ERROR_RETURN
                        (bcm_sc_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
                }
            } else 
    #endif
            {
                /* Clear bandwidth settings on port */
                for (cosq = 0; cosq < 8; cosq++) {
                     BCM_IF_ERROR_RETURN
                        (bcm_tr_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
                }
            }
            /* Clear scheduling settings on port */
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_port_sched_set(unit, 
                                             ESCONFIGr, COSWEIGHTSr, MINSPCONFIGr,
                                             port, 0, 8, weights, 
                                             BCM_COSQ_WEIGHTED_ROUND_ROBIN));
            /* Clear discard settings on port */
     
            if (SOC_IS_TR_VL(unit)) {
                for (cosq = 0; cosq < 8; cosq++) {
                     BCM_IF_ERROR_RETURN
                        (_bcm_tr_cosq_discard_set(unit, port, 
                                              BCM_COSQ_DISCARD_COLOR_ALL | 
                                              BCM_COSQ_DISCARD_NONTCP, cosq,
                                              WREDCONFIG_PACKETr,
                                              WREDPARAM_PACKETr,
                                              WREDPARAM_YELLOW_PACKETr,
                                              WREDPARAM_RED_PACKETr,
                                              WREDPARAM_NONTCP_PACKETr,
                                              TR_PACKET_FIELD_MAX(unit), 
                                              TR_PACKET_FIELD_MAX(unit), 100, 0));
                }
            }
        }
    }

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        BCM_IF_ERROR_RETURN (_bcm_sc_cosq_discard_all_disable(unit));
    } else
#endif
    {
        /* Disable WRED on all cosq level */
        BCM_IF_ERROR_RETURN (bcm_tr_cosq_discard_set(unit, 0));
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
bcm_tr_cosq_config_set(int unit, int numq)
{
    int cos, prio, ratio, remain;
    uint32 index, minsp;
    soc_mem_t mem = PORT_COS_MAPm; 
    int entry_words = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    bcm_pbmp_t ports;
    bcm_port_t port;
    port_cos_map_entry_t cos_map_array[TR_COS_MAP_ENTRIES_PER_SET];
    void *entries[1];

    /* Validate cosq num */
    if (numq < 1) {
        return BCM_E_PARAM;
    }

    /* Map the eight 802.1 priority levels to the active cosqs */
    if (numq > 8) {
        numq = 8;
    }

    if (SOC_IS_SHADOW(unit)) {
        return BCM_E_NONE; 
    }

    sal_memset(cos_map_array, 0,
               TR_COS_MAP_ENTRIES_PER_SET * sizeof(port_cos_map_entry_t));

    if (_tr_cos_map_profile[unit] == NULL) {
        _tr_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                          "COS_MAP Profile Mem");
        if (_tr_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_tr_cos_map_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               _tr_cos_map_profile[unit]));

    if (SOC_WARM_BOOT(unit)) {    /* Warm Boot */
        int     i;
        uint32  val;

        /* Gather reference count for cosq map profile table */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, port, &val));
            index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
            index *= TR_COS_MAP_ENTRIES_PER_SET;
            for (i = 0; i < TR_COS_MAP_ENTRIES_PER_SET; i++) {
                SOC_PROFILE_MEM_REFERENCE(unit, _tr_cos_map_profile[unit],
                                          index + i, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit,
                                                _tr_cos_map_profile[unit],
                                                index + i,
                                                TR_COS_MAP_ENTRIES_PER_SET);
            }
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
            if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
                SOC_IF_ERROR_RETURN(READ_ICOS_MAP_SELr(unit, port, &val));
                index = soc_reg_field_get(unit, ICOS_MAP_SELr, val, SELECTf);
                index *= TR_COS_MAP_ENTRIES_PER_SET;
                for (i = 0; i < TR_COS_MAP_ENTRIES_PER_SET; i++) {
                    SOC_PROFILE_MEM_REFERENCE(unit, _tr_cos_map_profile[unit],
                                              index + i, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit,
                                                    _tr_cos_map_profile[unit],
                                                    index + i,
                                                    TR_COS_MAP_ENTRIES_PER_SET);
                }
            }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
        }
    } else {    /* Cold Boot */

        entries[0] = &cos_map_array;

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

        /* Add a profile mem entry for each port */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
        PBMP_ITER(ports, port) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _tr_cos_map_profile[unit],
                                     (void *) &entries,
                                     TR_COS_MAP_ENTRIES_PER_SET,
                                     &index));
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, COS_MAP_SELr, port, 
                                                       SELECTf,
                                                       (index / TR_COS_MAP_ENTRIES_PER_SET)));
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
            if (IS_HG_PORT(unit, port) || port == CMIC_PORT(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, _tr_cos_map_profile[unit],
                                         (void *) &entries,
                                         TR_COS_MAP_ENTRIES_PER_SET,
                                         &index));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                                           SELECTf,
                                                           (index / TR_COS_MAP_ENTRIES_PER_SET)));
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
                    (soc_profile_mem_add(unit, _tr_cos_map_profile[unit],
                                         (void *) &entries,
                                         TR_COS_MAP_ENTRIES_PER_SET, &index));
                soc_reg_field32_modify(unit, ICOS_MAP_SELr, port, 
                                       SELECTf,
                                       (index / TR_COS_MAP_ENTRIES_PER_SET));
            }
        }
#endif

        /* Always strict priority for SC and QM COS queues */
        PBMP_ITER(ports, port) {
#ifdef BCM_SCORPION_SUPPORT
            if (SOC_IS_SC_CQ(unit)) {
                /* COS8 and COS9 not configurable on Scorpion */
                continue;
            }
#endif
            SOC_IF_ERROR_RETURN
                (READ_MINSPCONFIGr(unit, port, &minsp));
            soc_reg_field_set(unit, MINSPCONFIGr, &minsp, COS8_IS_SPf, 1);
            soc_reg_field_set(unit, MINSPCONFIGr, &minsp, COS9_IS_SPf, 1);
            SOC_IF_ERROR_RETURN
                (WRITE_MINSPCONFIGr(unit, port, minsp));
        }
    }

    _tr_num_cosq[unit] = numq;

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
bcm_tr_cosq_config_get(int unit, int *numq)
{
    if (_tr_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _tr_num_cosq[unit];
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
bcm_tr_cosq_mapping_set(int unit, bcm_port_t port,
                        bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    uint32 val, old_index, new_index;
    int i;
    port_cos_map_entry_t cos_map_array[TR_COS_MAP_ENTRIES_PER_SET], *entry_p;
    void *entries[1];
    bcm_pbmp_t ports;
    bcm_port_t local_port;

    if (priority < 0 || priority >= 16) {
        return (BCM_E_PARAM);
    }

    if (cosq < 0 || cosq >= 8) {
        return (BCM_E_PARAM);
    }

    if (port == -1) {	/* all ports */
        BCM_PBMP_ASSIGN(ports, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_local_get(unit, port, &local_port)); 
        } else {
            local_port = port;
        }

        if (!SOC_PORT_VALID(unit, local_port) || !IS_ALL_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
        BCM_PBMP_CLEAR(ports);
        BCM_PBMP_PORT_ADD(ports, local_port);
    }

    entries[0] = &cos_map_array;

    PBMP_ITER(ports, local_port) {
        SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, local_port, &val));
        old_index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
        old_index *= TR_COS_MAP_ENTRIES_PER_SET;

        /* get current mapping profile values */
        for (i = 0; i < TR_COS_MAP_ENTRIES_PER_SET; i++) {
            entry_p = SOC_PROFILE_MEM_ENTRY(unit, _tr_cos_map_profile[unit],
                                            port_cos_map_entry_t *, (old_index + i));
            memcpy(&cos_map_array[i], entry_p, sizeof(*entry_p));
        }
        soc_mem_field32_set(unit, PORT_COS_MAPm,
                            &cos_map_array[priority], COSf, cosq);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _tr_cos_map_profile[unit],
                                 (void *) &entries,
                                 TR_COS_MAP_ENTRIES_PER_SET,
                                 &new_index));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, COS_MAP_SELr, local_port, 
                                    SELECTf,
                                    new_index / TR_COS_MAP_ENTRIES_PER_SET));
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _tr_cos_map_profile[unit], old_index));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_HG_PORT(unit, local_port) || local_port == CMIC_PORT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _tr_cos_map_profile[unit],
                                     (void *) &entries,
                                     TR_COS_MAP_ENTRIES_PER_SET,
                                     &new_index));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ICOS_MAP_SELr, local_port, 
                                        SELECTf,
                                        new_index / TR_COS_MAP_ENTRIES_PER_SET));
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _tr_cos_map_profile[unit], old_index));
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
bcm_tr_cosq_mapping_get(int unit, bcm_port_t port,
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

        if (!SOC_PORT_VALID(unit, local_port) || !IS_ALL_PORT(unit, local_port)) {
            return BCM_E_PORT;
        }
    }

    SOC_IF_ERROR_RETURN(READ_COS_MAP_SELr(unit, local_port, &val));
    index = soc_reg_field_get(unit, COS_MAP_SELr, val, SELECTf);
    index *= TR_COS_MAP_ENTRIES_PER_SET;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _tr_cos_map_profile[unit],
                                    port_cos_map_entry_t *, 
                                    (index + priority));
    *cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, COSf);
    return BCM_E_NONE;
}

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
STATIC int
_bcm_tr_cpu_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags)
{
    uint32 regval;
    uint32 bucket_val = 0;
    uint64 regval_64;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
    int    refresh_bitsize, bucket_bitsize;

    if(SOC_IS_RCPU_ONLY(unit)) {
        return 0; 
    }   

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET,MINBUCKET
     * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */
    if (SOC_IS_ENDURO(unit)) {
        meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_XGS_METER_FLAG_PACKET_MODE : 0;
    }
     
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    /* Disable egress metering for this port */
    if (SOC_REG_IS_VALID(unit, CPUMAXBUCKETCONFIG_64r)) {
        BCM_IF_ERROR_RETURN(READ_CPUMAXBUCKETCONFIG_64r(unit, cosq, &regval_64));
        soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64, MAX_REFRESHf, 0);
        soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64, MAX_THDf, 0);
        BCM_IF_ERROR_RETURN(WRITE_CPUMAXBUCKETCONFIG_64r(unit, cosq, regval_64));
    }

    /*reset the MAXBUCKET register*/
    if (SOC_REG_IS_VALID(unit, CPUMAXBUCKETr)) {
        soc_reg_field_set(unit, CPUMAXBUCKETr, &bucket_val, PKT_MAX_BUCKETf, 0);
        soc_reg_field_set(unit, CPUMAXBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
        BCM_IF_ERROR_RETURN(WRITE_CPUMAXBUCKETr(unit, cosq, bucket_val));
    }

    refresh_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_REFRESHf);
    bucket_bitsize =
        soc_reg_field_length(unit, MINBUCKETCONFIG_64r, MIN_THDf);

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min, kbits_sec_min,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    if (SOC_REG_IS_VALID(unit, CPUMAXBUCKETCONFIG_64r)) {
        refresh_bitsize =
            soc_reg_field_length(unit, CPUMAXBUCKETCONFIG_64r, MAX_REFRESHf);
        bucket_bitsize =
            soc_reg_field_length(unit, CPUMAXBUCKETCONFIG_64r, MAX_THDf);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max, kbits_sec_burst,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

    COMPILER_64_ZERO(regval_64);
    if (SOC_REG_IS_VALID(unit, CPUMAXBUCKETCONFIG_64r)) {
        soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64,
                              METER_GRANf, granularity);
        soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64,
                              MAX_REFRESHf, refresh_rate);
        soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64, MAX_THDf,
                              bucketsize);
        if (SOC_IS_ENDURO(unit)) {                          
            soc_reg64_field32_set(unit, CPUMAXBUCKETCONFIG_64r, &regval_64, MODEf,
                              (flags & BCM_COSQ_BW_PACKET_MODE) ? 1 : 0);
        }
        BCM_IF_ERROR_RETURN(WRITE_CPUMAXBUCKETCONFIG_64r(unit, cosq, regval_64));
    }
    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}
#endif


STATIC int
_bcm_tr_cosq_bucket_set(int unit, bcm_port_t port, 
                                bcm_cos_queue_t cosq,
                                uint32 kbits_sec_min, /* kbps or packet/second */
                                uint32 kbits_sec_max, /* kbps or packet/second */
                                uint32 kbits_burst_min, 
                                uint32 kbits_burst_max, 
                                uint32 flags)
{
    uint32 regval;
    uint32 bucket_val = 0;
    uint64 regval_64;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
    int    refresh_bitsize, bucket_bitsize;

    if(SOC_IS_RCPU_ONLY(unit)) {
        return 0; 
    }   

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT) 
    if (IS_CPU_PORT(unit, port)) {
        return _bcm_tr_cpu_cosq_port_bandwidth_set(unit, port, cosq, 
                kbits_sec_min, kbits_sec_max, kbits_burst_max, flags);
    }    
#endif /* BCM_ENDURO_SUPPORT */ 

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
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min, kbits_burst_min,
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
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max, kbits_burst_max,
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

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

STATIC int
_bcm_tr_cosq_bucket_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_min_burst,
                               uint32 *kbits_max_burst,
                               uint32 *flags)
{
    uint32 regval;
    uint64 regval_64;
    uint32 refresh_rate = 0, bucketsize = 0,
        granularity = 3, meter_flags = 0;

    if (!kbits_sec_min || !kbits_sec_max || !kbits_min_burst
        || !kbits_max_burst || !flags) {
        return (BCM_E_PARAM);
    }

    *flags = 0;
    BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG_64r(unit, port, cosq, &regval_64));
    granularity = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                            regval_64, METER_GRANf);
    refresh_rate = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, MAXBUCKETCONFIG_64r,
                                             regval_64, MAX_THDf);

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }

    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;



    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           kbits_sec_max, kbits_max_burst));

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
                                           kbits_sec_min, kbits_min_burst));

    return BCM_E_NONE;
}


int
bcm_tr_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags)
{
    
    /*keep place with original*/
    return _bcm_tr_cosq_bucket_set(unit, port, cosq, kbits_sec_min, kbits_sec_max,
                                    kbits_sec_min, kbits_sec_burst, flags); 
}

int
bcm_tr_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *kbits_sec_min,
                               uint32 *kbits_sec_max,
                               uint32 *kbits_sec_burst,
                               uint32 *flags)
{

    uint32 kbits_min_burst; /* Placeholder, since burst is unused for min */

    BCM_IF_ERROR_RETURN(_bcm_tr_cosq_bucket_get(unit, port, cosq,
                        kbits_sec_min, kbits_sec_max, &kbits_min_burst,
                        kbits_sec_burst, flags));
    return BCM_E_NONE;

}

STATIC int
_bcm_tr_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int pps, int burst)

{
    uint32 regval, max_config_addr, max_addr;
    soc_reg_t maxbucket_config_reg, maxbucket_reg;
    soc_field_t refresh_f, thd_sel_f, bucket_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPKTPORTMAXBUCKETCONFIGr;
        maxbucket_reg = CPUPKTPORTMAXBUCKETr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        bucket_f = PKT_PORT_MAX_BUCKETf;
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUPKTMAXBUCKETCONFIGr;
        maxbucket_reg = CPUPKTMAXBUCKETr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
        bucket_f = PKT_MAX_BUCKETf;
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG and MAXBUCKET
     * b. update MAXBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, 0);
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, thd_sel_f, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_config_addr, regval));

    /* reset the MAXBUCKET register*/
    max_addr = soc_reg_addr(unit, maxbucket_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_addr, &regval));
    soc_reg_field_set(unit, maxbucket_reg, &regval, bucket_f, 0);
    soc_reg_field_set(unit, maxbucket_reg, &regval, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_addr, regval));

    /* Check packets-per second upper limit */
    if (pps > TR_PKT_REFRESH_MAX(unit)) {
        pps = TR_PKT_REFRESH_MAX(unit);
    }

    /* Check burst upper limit */
    if (burst > TR_PKT_THD_MAX(unit)) {
        burst = TR_PKT_THD_MAX(unit);
    }

    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    soc_reg_field_set(unit, maxbucket_config_reg, &regval, refresh_f, pps);
    soc_reg_field_set(unit, maxbucket_config_reg, 
                                &regval, thd_sel_f, burst);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_config_addr, regval));

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

#ifdef BCM_ENDURO_SUPPORT

STATIC int
_bcm_en_cosq_port_packet_bandwidth_set(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int pps, int burst, uint32 flags)
{
    uint64 regval_64;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
    int    refresh_bitsize, bucket_bitsize;
    uint32 regval, max_config_addr, max_addr;
    soc_reg_t maxbucket_config_reg, maxbucket_reg;
    soc_field_t refresh_f, thd_sel_f, bucket_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPORTMAXBUCKETCONFIG_64r;
        maxbucket_reg = CPUPORTMAXBUCKETr;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THD_SELf;
        bucket_f = MAX_BUCKETf;            
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUMAXBUCKETCONFIG_64r;
        maxbucket_reg = CPUMAXBUCKETr;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THDf;
        bucket_f = PKT_MAX_BUCKETf;
    }

    /*
     * To set the new Bandwidth settings, the procedure adopted is
     * a. reset MAXBUCKETCONFIG and MAXBUCKET
     * b. update MAXBUCKETCONFIG with new values passed
     * c. if MISCCONFIG.METERING_CLK_EN not set before, enable it.
     */

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg64_read(unit, max_config_addr, &regval_64));
    soc_reg64_field32_set(unit, maxbucket_config_reg, 
                                &regval_64, refresh_f, 0);
    soc_reg64_field32_set(unit, maxbucket_config_reg, 
                                &regval_64, thd_sel_f, 0);
    BCM_IF_ERROR_RETURN(soc_reg64_write(unit, max_config_addr, regval_64));       

    /* reset the MAXBUCKET register*/
    max_addr = soc_reg_addr(unit, maxbucket_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_addr, &regval));
    soc_reg_field_set(unit, maxbucket_reg, &regval, bucket_f, 0);
    soc_reg_field_set(unit, maxbucket_reg, &regval, OUT_PROFILE_FLAGf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_write(unit, max_addr, regval));

    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
                            _BCM_XGS_METER_FLAG_PACKET_MODE : 0;
        
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }
    meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

    refresh_bitsize =
            soc_reg_field_length(unit, maxbucket_config_reg, refresh_f);
    bucket_bitsize =
            soc_reg_field_length(unit, maxbucket_config_reg, thd_sel_f);

    BCM_IF_ERROR_RETURN
            (_bcm_xgs_kbits_to_bucket_encoding(pps, burst,
                              meter_flags, refresh_bitsize, bucket_bitsize,
                              &refresh_rate, &bucketsize, &granularity));

    BCM_IF_ERROR_RETURN(soc_reg64_read(unit, max_config_addr, &regval_64));
    soc_reg64_field32_set(unit, maxbucket_config_reg, 
                                    &regval_64, METER_GRANf, granularity);
    soc_reg64_field32_set(unit, maxbucket_config_reg, 
                                    &regval_64, refresh_f, refresh_rate);
    soc_reg64_field32_set(unit, maxbucket_config_reg, 
                                    &regval_64, thd_sel_f, bucketsize);
    soc_reg64_field32_set(unit, maxbucket_config_reg, &regval_64, MODEf, 
                                    (flags & BCM_COSQ_BW_PACKET_MODE) ? 1 : 0);
    BCM_IF_ERROR_RETURN(soc_reg64_write(unit, max_config_addr, regval_64));      

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    return BCM_E_NONE;
}

#endif /* BCM_ENDURO_SUPPORT */


STATIC int
_bcm_tr_cosq_port_packet_bandwidth_get(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int *pps, int *burst)


{
    uint32 regval, max_config_addr;
    soc_reg_t maxbucket_config_reg;
    soc_field_t refresh_f, thd_sel_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPKTPORTMAXBUCKETCONFIGr;
        refresh_f = PKT_PORT_MAX_REFRESHf;
        thd_sel_f = PKT_PORT_MAX_THD_SELf;
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUPKTMAXBUCKETCONFIGr;
        refresh_f = PKT_MAX_REFRESHf;
        thd_sel_f = PKT_MAX_THD_SELf;
    }

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(soc_reg32_read(unit, max_config_addr, &regval));
    *pps = soc_reg_field_get(unit, maxbucket_config_reg, regval, refresh_f);
    *burst = soc_reg_field_get(unit, maxbucket_config_reg, 
                                         regval, thd_sel_f);

    return BCM_E_NONE;
}

#ifdef BCM_ENDURO_SUPPORT

STATIC int
_bcm_en_cosq_port_packet_bandwidth_get(int unit, bcm_port_t port,
                                       bcm_cos_queue_t cosq,
                                       int *pps, int *burst,
                                       uint32 *flags)
{
    uint64 regval_64;
    uint32 refresh_rate = 0, bucketsize = 0,
        granularity = 3, meter_flags = 0;
    uint32 regval;
    uint32 max_config_addr;
    
    soc_reg_t maxbucket_config_reg;
    soc_field_t refresh_f, thd_sel_f;

    if (cosq < 0) {
        maxbucket_config_reg = CPUPORTMAXBUCKETCONFIG_64r;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THD_SELf;
        cosq = 0;
    } else {
        maxbucket_config_reg = CPUMAXBUCKETCONFIG_64r;
        refresh_f = MAX_REFRESHf;
        thd_sel_f = MAX_THDf;
    }

    /* Disable egress metering */
    max_config_addr = soc_reg_addr(unit, maxbucket_config_reg, port, cosq);
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
        if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
            meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
        }

        *flags = 0;

        BCM_IF_ERROR_RETURN(soc_reg64_read(unit, max_config_addr, &regval_64));       
        if (soc_reg64_field32_get(unit, maxbucket_config_reg, 
                                                  regval_64, MODEf)) {
            meter_flags |= _BCM_XGS_METER_FLAG_PACKET_MODE;
            *flags |= BCM_COSQ_BW_PACKET_MODE;
        }
        meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;

        granularity = soc_reg64_field32_get(unit, maxbucket_config_reg, 
                                                  regval_64, METER_GRANf);
        refresh_rate = soc_reg64_field32_get(unit, maxbucket_config_reg, 
                                                   regval_64, refresh_f);
        bucketsize = soc_reg64_field32_get(unit, maxbucket_config_reg, 
                                                 regval_64, thd_sel_f);

        BCM_IF_ERROR_RETURN
            (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                               granularity, meter_flags,
                                               (uint32 *)pps, (uint32 *)burst));

    return BCM_E_NONE;
}

#endif /* BCM_ENDURO_SUPPORT */

int
bcm_tr_cosq_port_pps_set(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int pps)
{
    int temp_pps, burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    } else if (pps < 0) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
#ifdef BCM_ENDURO_SUPPORT    
    if (SOC_IS_ENDURO(unit)) {
        uint32 flags;

        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &temp_pps, &burst,
                                                &flags));

        /* Replace the current PPS setting, keep BURST the same */
        return _bcm_en_cosq_port_packet_bandwidth_set(unit, port, cosq,
                             pps, burst, flags | BCM_COSQ_BW_PACKET_MODE);
                                                
    } else 
#endif /* BCM_ENDURO_SUPPORT */    
    {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &temp_pps, &burst));

        /* Replace the current PPS setting, keep BURST the same */
        return _bcm_tr_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                  pps, burst);

    }
}

int
bcm_tr_cosq_port_pps_get(int unit, bcm_port_t port,
                         bcm_cos_queue_t cosq, int *pps)
{
    int burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }
    
#ifdef BCM_ENDURO_SUPPORT    
    if (SOC_IS_ENDURO(unit)) {
        uint32 flags;
    
        return _bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  pps, &burst, &flags);
    } else 
#endif /* BCM_ENDURO_SUPPORT */        
    {
        return _bcm_tr_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  pps, &burst);
    }
}

int
bcm_tr_cosq_port_burst_set(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int burst)
{
    int pps, temp_burst;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    } else if (burst < 0) {
        return BCM_E_PARAM;
    }
#ifdef BCM_ENDURO_SUPPORT    
    if (SOC_IS_ENDURO(unit)) {
        uint32 flags;

        /* Get the current PPS and BURST settings */
        BCM_IF_ERROR_RETURN
            (_bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &pps, &temp_burst,
                                                &flags));

        /* Replace the current BURST setting, keep PPS the same */
        return _bcm_en_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                pps, burst, flags | BCM_COSQ_BW_PACKET_MODE);
    } else 
#endif /* BCM_ENDURO_SUPPORT */            
    {
        /* Get the current PPS and BURST settings */
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                &pps, &temp_burst));

        /* Replace the current BURST setting, keep PPS the same */
        return _bcm_tr_cosq_port_packet_bandwidth_set(unit, port, cosq,
                                                  pps, burst);
    }    
}

int
bcm_tr_cosq_port_burst_get(int unit, bcm_port_t port,
                           bcm_cos_queue_t cosq, int *burst)
{
    int pps;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_ENDURO_SUPPORT        
    if (SOC_IS_ENDURO(unit)) {
        uint32 flags;

        return _bcm_en_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  &pps, burst, &flags);
    } else 
#endif /* BCM_ENDURO_SUPPORT */                
    {
        return _bcm_tr_cosq_port_packet_bandwidth_get(unit, port, cosq,
                                                  &pps, burst);
    }
}

int
_bcm_tr_cosq_port_resolve(int unit, bcm_gport_t gport,
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

        if (!_tr_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (!SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], *port)) {
            return BCM_E_BADID;
        } else if (_tr_num_port_cosq[unit][*port] == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_BADID;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_tr_cosq_resolve_mod_port(int unit, bcm_gport_t gport,
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
_bcm_tr_cosq_discard_cap_enable_set(int unit, bcm_port_t port, 
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
_bcm_tr_cosq_discard_cap_enable_get(int unit, bcm_port_t port, 
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
bcm_tr_cosq_discard_set(int unit, uint32 flags)
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
                (_bcm_tr_cosq_discard_cap_enable_set(unit, port, cosq,
                                                     config_reg, flags));
        }
        if (SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], port)) {
            for (cosq = 8; cosq < 24; cosq++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_cap_enable_set(unit, port, cosq,
                                                         config_reg, flags));
            }
        }
    }
    return BCM_E_NONE;
}

int
bcm_tr_cosq_discard_get(int unit, uint32 *flags)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    soc_reg_t config_reg = WREDCONFIG_PACKETr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        *flags = 0;
        rv = _bcm_tr_cosq_discard_cap_enable_get(unit, port, 0,
                                                 config_reg, flags);
        break;
    }
    return rv;
}

/*      
 *  Convert HW drop probability to percent value
 */     
STATIC int       
_bcm_tr_hw_drop_prob_to_percent[] = {
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
_bcm_tr_percent_to_drop_prob(int percent) {
   int i;

   for (i=14; i > 0 ; i--) {
      if (percent >= _bcm_tr_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_tr_drop_prob_to_percent(int drop_prob) {
   return (_bcm_tr_hw_drop_prob_to_percent[drop_prob]);
}   

STATIC int 
_bcm_tr_angle_to_packets_table[] =
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
_bcm_tr_angle_to_packets(int angle) {
    return (_bcm_tr_angle_to_packets_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_tr_packets_to_angle(int packets) {
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_tr_angle_to_packets_table[angle]) {
            break;
        }
    }
    return angle;
}

STATIC int     
_bcm_tr_cosq_discard_set(int unit, bcm_port_t port, uint32 flags,
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
            _bcm_tr_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_YELLOW) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, YELLOW_MAXDROPRATEf,
            _bcm_tr_percent_to_drop_prob(drop_probability));
    }
    if ((flags & BCM_COSQ_DISCARD_COLOR_RED) ||
        ((flags & BCM_COSQ_DISCARD_COLOR_ALL) == BCM_COSQ_DISCARD_COLOR_ALL)) {
        soc_reg_field_set(unit, config_reg, &val, RED_MAXDROPRATEf,
            _bcm_tr_percent_to_drop_prob(drop_probability));
    }
    if (flags & BCM_COSQ_DISCARD_NONTCP)  {
        soc_reg_field_set(unit, config_reg, &val, NONTCP_MAXDROPRATEf,
            _bcm_tr_percent_to_drop_prob(drop_probability));
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
_bcm_tr_cosq_discard_get(int unit, bcm_port_t port, uint32 flags,
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
    *drop_probability = _bcm_tr_drop_prob_to_percent(drop_rate);

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
bcm_tr_cosq_discard_port_set(int unit, bcm_port_t port,
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

    if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
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
            BCM_PBMP_PORT_SET(pbmp, port);
            local_port = port;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        if (_tr_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < -1 || cosq >= _tr_num_port_cosq[unit][local_port]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq_start = 8;
            num_cosq = _tr_num_port_cosq[unit][local_port];
        } else {
            cosq_start = cosq + 8;
            num_cosq = 1;
        }
    } else {
        if (cosq < -1 || cosq >= _tr_num_cosq[unit]) {
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
    max_thresh = min_thresh + _bcm_tr_angle_to_packets(drop_slope);
    if (max_thresh > TR_PACKET_FIELD_MAX(unit)) {
        max_thresh = TR_PACKET_FIELD_MAX(unit);
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        for (i = cosq_start; i < (cosq_start + num_cosq); i++) { 
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_set(unit, local_port, color, i,
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
bcm_tr_cosq_discard_port_get(int unit, bcm_port_t port,
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

    if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                               &local_port, &trunk_id));
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, port, &local_port));
        }
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
#ifdef BCM_ENDURO_SUPPORT
            if (SOC_IS_ENDURO(unit)) {
                BCM_PBMP_ASSIGN(pbmp, PBMP_E_ALL(unit));
            } else 		
#endif /* BCM_ENDURO_SUPPORT */
            {
                BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
            }
        } else if (IS_CPU_PORT(unit, port) || !SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
            local_port = port;
        }
    }

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        if (_tr_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < -1 || cosq >= _tr_num_port_cosq[unit][local_port]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq = 8;
        } else {
            cosq = cosq + 8;
        }
    } else {
        if (cosq < -1 || cosq >= _tr_num_cosq[unit]) {
            return BCM_E_PARAM;
        } else if (cosq == -1) {
            cosq = 0;
        }
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_discard_get(unit, local_port, color, cosq,
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
        *drop_slope = _bcm_tr_packets_to_angle(max_thresh - min_thresh);
        break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_tr_cosq_port_sched_set(int unit, soc_reg_t config_reg,
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
            if (weights[i] > TR_WRR_WEIGHT_MAX) {
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
            if (weight_max <= TR_DRR_WEIGHT_MAX * (1 << (sel + 1))) {
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
_bcm_tr_cosq_port_sched_get(int unit, soc_reg_t config_reg,
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
bcm_tr_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int weights[], int delay)
{
    bcm_port_t port;
    uint32 rval, mbits;
    int i,max_weight;

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
        /* coverity[overrun-local : FALSE] */
        if (IS_CPU_PORT(unit, port) && !SOC_IS_ENDURO(unit)) {
            switch (mode) {
            case BCM_COSQ_STRICT:
                mbits = 0;
                break;
            case BCM_COSQ_ROUND_ROBIN:
                mbits = 1;
                break;
            case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            case BCM_COSQ_DEFICIT_ROUND_ROBIN:
                if (BCM_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
                    continue;
                }
                return BCM_E_PARAM;
            default:
                return BCM_E_PARAM;
            }

            if (SOC_IS_TR_VL(unit)) {
                SOC_IF_ERROR_RETURN(READ_ES_CPU_SCHEDULERr(unit, &rval));
                soc_reg_field_set(unit, ES_CPU_SCHEDULERr, &rval, MODEf,
                                  mbits);
                SOC_IF_ERROR_RETURN(WRITE_ES_CPU_SCHEDULERr(unit, rval));
                continue;
            } /* Scorpion uses the call below for allowed modes on CPU */
        }
    
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_sched_set(unit,
                                         ESCONFIGr, COSWEIGHTSr,
                                         MINSPCONFIGr,
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
bcm_tr_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int weights[], int *delay)
{
    bcm_port_t port;
    uint32 rval;

    PBMP_ITER(pbm, port) {
        /* coverity[overrun-local : FALSE] */
        if (IS_CPU_PORT(unit, port)) {
            if (BCM_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
                continue;
            }
            if (SOC_IS_TR_VL(unit) && !SOC_IS_ENDURO(unit)) {
                SOC_IF_ERROR_RETURN(READ_ES_CPU_SCHEDULERr(unit, &rval));
                if (soc_reg_field_get(unit, ES_CPU_SCHEDULERr,
                                      rval, MODEf)) {
                    *mode = BCM_COSQ_ROUND_ROBIN;
                } else {
                    *mode = BCM_COSQ_STRICT;
                }
                return BCM_E_NONE;
            } /* Scorpion uses the call below for CPU */
        }
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_sched_get(unit, ESCONFIGr, COSWEIGHTSr,
                                         port, 0, 8, weights, mode));
        break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr_cosq_sched_weight_max_get
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
bcm_tr_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        *weight_max = TR_WRR_WEIGHT_MAX;
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        *weight_max = TR_DRR_WEIGHT_MAX * 16; /* Maximum MTU quanta */
        break;
    default:
        *weight_max = BCM_COSQ_WEIGHT_UNLIMITED;
        return BCM_E_PARAM;
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
bcm_tr_cosq_gport_add(int unit, bcm_gport_t port, int numq,
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
        (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                       &local_port, &trunk_id));

    /* Verify that the specified port is for my modid and that 
     * it is capable of hierarchical queueing.
     */
    if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if (!SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], local_port)) {
        return BCM_E_PORT;
    } else if (_tr_num_port_cosq[unit][local_port]) {
        return BCM_E_EXISTS;
    }
    /* Call delete routine to init HW settings */
    BCM_IF_ERROR_RETURN (_bcm_tr_cosq_gport_delete(unit, local_port));

    _tr_num_port_cosq[unit][local_port] = numq;

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
_bcm_tr_cosq_gport_delete(int unit, bcm_port_t port)
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
            (bcm_tr_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));
    }

    /* Clear bandwidth of S1 output queue */
    cosq = 24;
    BCM_IF_ERROR_RETURN
        (bcm_tr_cosq_port_bandwidth_set(unit, port, cosq, 0, 0, 0, 0));

    /* Clear weights of all Stage1 COSQs */
    for (cosq = 0; cosq < 16; cosq++) {
        weights[cosq] = 0;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_port_sched_set(unit, S1V_CONFIGr, S1V_COSWEIGHTSr,
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
            (_bcm_tr_cosq_discard_set(unit, port, 
                                      BCM_COSQ_DISCARD_COLOR_ALL | 
                                      BCM_COSQ_DISCARD_NONTCP, cosq,
                                      WREDCONFIG_PACKETr,
                                      WREDPARAM_PACKETr,
                                      WREDPARAM_YELLOW_PACKETr,
                                      WREDPARAM_RED_PACKETr,
                                      WREDPARAM_NONTCP_PACKETr,
                                      TR_PACKET_FIELD_MAX(unit), 
                                      TR_PACKET_FIELD_MAX(unit), 100, 0));
    }

    _tr_num_port_cosq[unit][port] = 0;

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
bcm_tr_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));
    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        if (!_tr_num_port_cosq[unit]) {
            return BCM_E_INIT;
        } else if (_tr_num_port_cosq[unit][local_port] == 0) {
            return BCM_E_NOT_FOUND;
        }
    } else if(BCM_GPORT_IS_LOCAL(gport) ||
            BCM_GPORT_IS_MODPORT(gport)) {
        if (!_tr_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
    } else {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (_bcm_tr_cosq_gport_delete(unit, local_port));
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
bcm_tr_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    bcm_module_t my_modid, mod_out;
    bcm_port_t port, port_out;
    bcm_gport_t gport, sched_gport;
    int gport_id;
    uint32 flags = BCM_COSQ_GPORT_SCHEDULER;
    int rv = SOC_E_NONE;

    if (_tr_num_port_cosq[unit]) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        BCM_PBMP_ITER(_tr_cosq_24q_ports[unit], port) {
            if (_tr_num_port_cosq[unit][port]) {

                /* Construct physical port GPORT ID */
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                            my_modid, port, &mod_out, &port_out));
                BCM_GPORT_MODPORT_SET(gport, mod_out, port_out);

                /* Construct scheduler GPORT ID */
                gport_id = (mod_out << 8) | port_out;
                BCM_GPORT_SCHEDULER_SET(sched_gport, gport_id);

                /* Call application call-back */
                rv = cb(unit, gport, _tr_num_port_cosq[unit][port],
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
_bcm_tr_cosq_gport_bandwidth_port_resolve(int unit, bcm_gport_t gport,
                                          bcm_cos_queue_t cosq,
                                          bcm_port_t *local_port,
                                          bcm_cos_queue_t *cosq_start,
                                          bcm_cos_queue_t *cosq_end)
{
    bcm_module_t modid;
    bcm_trunk_t trunk_id;

    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                           local_port, &trunk_id));
    } else {
        *local_port = gport;
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
    }

    if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (cosq < -1) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        if (_tr_num_port_cosq[unit][*local_port] == 0) {
            return BCM_E_NOT_FOUND;
        } else if (cosq < 0) {
            *cosq_start = 8;
            *cosq_end = 23;
        } else if (cosq >= _tr_num_port_cosq[unit][*local_port]) {
            return BCM_E_PARAM;
        } else {
            *cosq_start = *cosq_end = (cosq + 8);
        }
    } else if (cosq == TR_SCHEDULER_COSQ) {
        if (!SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], *local_port)) {
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
            if (cosq >= _tr_num_cosq[unit]) {
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
bcm_tr_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    bcm_port_t local_port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                   &local_port,
                                                   &cosq_start, &cosq_end));

    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (bcm_tr_cosq_port_bandwidth_set(unit, local_port, i,
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
bcm_tr_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    bcm_port_t local_port;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_burst;    /* Dummy variable */

    *kbits_sec_min = *kbits_sec_max = *flags = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                   &local_port,
                                                   &cosq_start, &cosq_end));

    BCM_IF_ERROR_RETURN
        (bcm_tr_cosq_port_bandwidth_get(unit, local_port, cosq_start,
                                        kbits_sec_min, kbits_sec_max,
                                        &kbits_sec_burst, flags));
    return BCM_E_NONE;
}

/*  
 * Function:
 *      bcm_tr_cosq_gport_bandwidth_burst_set
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
bcm_tr_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    bcm_port_t local_port, port;
    int i;
    bcm_cos_queue_t cosq_start = 0, cosq_end = 0;
    uint32 kbits_sec_min, kbits_sec_max, flags;
    uint32 kbits_sec_burst; /* Dummy variable */

    if (gport < 0) {
        PBMP_ALL_ITER(unit, port) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, port, cosq,
                                                           &local_port,
                                                           &cosq_start, &cosq_end));
            for (i = cosq_start; i <= cosq_end; i++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_bucket_get(unit, local_port, i,
                                                    &kbits_sec_min, &kbits_sec_max,
                                                    &kbits_sec_burst,&kbits_sec_burst,&flags));
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_bucket_set(unit, local_port, i,
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
        (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                   &local_port,
                                                   &cosq_start, &cosq_end));

    for (i = cosq_start; i <= cosq_end; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_bucket_get(unit, local_port, cosq_start,
                                            &kbits_sec_min, &kbits_sec_max,
                                            &kbits_sec_burst, &kbits_sec_burst,&flags));
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_bucket_set(unit, local_port, i,
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
 *      bcm_tr_cosq_gport_bandwidth_burst_get
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
bcm_tr_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
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
            (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, port, cosq,
                                                       &local_port,
                                                       &cosq_start, &cosq_end));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_gport_bandwidth_port_resolve(unit, gport, cosq,
                                                       &local_port,
                                                       &cosq_start, &cosq_end));
    }

    kbits_sec_min = kbits_sec_max = flags = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_bucket_get(unit, local_port, cosq_start,
                                        &kbits_sec_min, &kbits_sec_max,
                                        kbits_burst_min,kbits_burst_max, &flags));
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
bcm_tr_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i;
	int numq;
    bcm_cos_queue_t cosq_start = 0;
    int num_weights = 1, weights[64];
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;
    soc_reg_t minsp_reg = MINSPCONFIGr;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));

        if (!_tr_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_tr_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _tr_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 16;
                for (i = 0; i < num_weights; i++) {
                    if (i < _tr_num_port_cosq[unit][local_port]) {
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
        } else if (IS_CPU_PORT(unit, local_port)) {
            numq = SOC_INFO(unit).num_cpu_cosq;
            if (cosq >= numq) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = NUM_CPU_COSQ(unit);
                for (i = 0; i < num_weights; i++) {
                    weights[i] = weight;
                } 
            } else {	
                cosq_start = cosq;
                num_weights = 1;
                weights[0] = weight;
            }
		} else if (cosq == TR_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq_start = 8;
            num_weights = 1;
            weights[0] = weight;
        } else {
            if (cosq >= _tr_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq_start = 0;
                num_weights = 8;
                for (i = 0; i < num_weights; i++) {
                    if (i < _tr_num_cosq[unit]) {
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

        /* coverity[uninit_use_in_call : FALSE] */
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_sched_set(unit, config_reg, weight_reg, minsp_reg,
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
bcm_tr_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight)
{   
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    soc_reg_t config_reg = ESCONFIGr;
    soc_reg_t weight_reg = COSWEIGHTSr;

    *mode = *weight = 0;

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));
        if (!_tr_num_port_cosq[unit]) {
            return BCM_E_INIT;
        }
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            if (_tr_num_port_cosq[unit][local_port] == 0) {
                return BCM_E_NOT_FOUND;
            } else if (cosq >= _tr_num_port_cosq[unit][local_port]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
            config_reg = S1V_CONFIGr;
            weight_reg = S1V_COSWEIGHTSr;
        } else if (cosq == TR_SCHEDULER_COSQ) {
            if (!SOC_PBMP_MEMBER(_tr_cosq_24q_ports[unit], local_port)) {
                return BCM_E_PORT;
            }
            /* Weight for output of S1 scheduler is configured using offset 8 
             * of COSWEIGHTS register.
             */
            cosq = 8;
        } else {
            if (cosq >= _tr_num_cosq[unit]) {
                return BCM_E_PARAM;
            } else if (cosq < 0) {
                cosq = 0;
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_port_sched_get(unit, config_reg, weight_reg, 
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
bcm_tr_cosq_gport_discard_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
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

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        if (!(discard->flags & BCM_COSQ_DISCARD_BYTES)) {
            /* Scorpion only supports byte accounting for WRED */ 
            return BCM_E_UNAVAIL;
        }
        cell_size      = _BCM_SC_COSQ_CELLSIZE;
        cell_field_max = SC_CELL_FIELD_MAX;
    } else
#endif /* BCM_SCORPION_SUPPORT */
    {
        cell_size      = _BCM_TR_COSQ_CELLSIZE;
        cell_field_max = TR_CELL_FIELD_MAX(unit);
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
        if ((min_thresh > TR_PACKET_FIELD_MAX(unit)) || 
            (max_thresh > TR_PACKET_FIELD_MAX(unit))) {
            return BCM_E_PARAM;
        }
    }

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_set(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         discard->flags));

            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_set(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_set(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                if (_tr_num_port_cosq[unit][local_port] == 0) {
                    return BCM_E_NOT_FOUND;
                } else if (cosq >= _tr_num_port_cosq[unit][local_port]) {
                    return BCM_E_PARAM;
                } else {
                    cosq = cosq + 8;
                }
            } else if (cosq >= _tr_num_cosq[unit]) {
                return BCM_E_PARAM;
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_set(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_set(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_set(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_set(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          min_thresh, max_thresh, 
                                          discard->drop_probability,
                                          discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_cap_enable_set(unit, REG_PORT_ANY, 0,
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
bcm_tr_cosq_gport_discard_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    uint32 min_thresh, max_thresh;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        if (!(discard->flags & BCM_COSQ_DISCARD_BYTES)) {
            return BCM_E_UNAVAIL;
        }
    }
#endif

    if (port != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        if (cosq < 0) {
            /* per-port discard settings */
            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_get(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_get(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_get(unit, local_port, 0,
                                                         PORT_WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        } else {
            /* per-port/per-cos discard settings */
            if (SOC_IS_TR_VL(unit)) {
                if (BCM_GPORT_IS_SCHEDULER(port)) {
                    if (_tr_num_port_cosq[unit][local_port] == 0) {
                        return BCM_E_NOT_FOUND;
                    } else if (cosq >= _tr_num_port_cosq[unit][local_port]) {
                        return BCM_E_PARAM;
                    } else {
                        cosq = cosq + 8;
                    }
                } else if (cosq >= _tr_num_cosq[unit]) {
                    return BCM_E_PARAM;
                }
            } else {
                if (cosq < BCM_COS_MIN || cosq > BCM_COS_MAX) {
                    return BCM_E_PARAM;
                }
            }

            if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_get(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_CELLr, 
                                                         &discard->flags));
            } else { /* BCM_COSQ_DISCARD_PACKETS */
                BCM_IF_ERROR_RETURN
                    (_bcm_tr_cosq_discard_get(unit, local_port,
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
                    (_bcm_tr_cosq_discard_cap_enable_get(unit, local_port, cosq,
                                                         WREDCONFIG_PACKETr, 
                                                         &discard->flags));
            }
        }
    } else {
        /* chip-wide discard settings */
        if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_CELLr,
                                          GLOBAL_WREDPARAM_CELLr,
                                          GLOBAL_WREDPARAM_YELLOW_CELLr,
                                          GLOBAL_WREDPARAM_RED_CELLr,
                                          GLOBAL_WREDPARAM_NONTCP_CELLr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_CELLr, 
                                                     &discard->flags));
        } else { /* BCM_COSQ_DISCARD_PACKETS */
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_get(unit, REG_PORT_ANY, discard->flags,
                                          0, GLOBAL_WREDCONFIG_PACKETr,
                                          GLOBAL_WREDPARAM_PACKETr,
                                          GLOBAL_WREDPARAM_YELLOW_PACKETr,
                                          GLOBAL_WREDPARAM_RED_PACKETr,
                                          GLOBAL_WREDPARAM_NONTCP_PACKETr,
                                          &min_thresh, &max_thresh, 
                                          &discard->drop_probability,
                                          &discard->gain));
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_cap_enable_get(unit, REG_PORT_ANY, 0,
                                                     GLOBAL_WREDCONFIG_PACKETr, 
                                                     &discard->flags));
        }
    }

    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert cells to bytes */
        min_thresh *= _BCM_TR_COSQ_CELLSIZE;
        max_thresh *= _BCM_TR_COSQ_CELLSIZE;
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
bcm_tr_cosq_gport_attach(int unit, bcm_gport_t sched_gport, 
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PARAM;
    } else if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if (cosq < 0) {
        cosq = TR_SCHEDULER_COSQ;
    } else if (cosq != TR_SCHEDULER_COSQ) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));

        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                           &input_port, &trunk_id));

        if (_tr_num_port_cosq[unit][sched_port] == 0) {
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
bcm_tr_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_PORT;
    } else if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    } else if ((cosq > 0) && (cosq != TR_SCHEDULER_COSQ)) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));
       BCM_IF_ERROR_RETURN
           (_bcm_tr_cosq_resolve_mod_port(unit, input_gport, &input_modid,
                                          &input_port, &trunk_id));
        if (_tr_num_port_cosq[unit][sched_port] == 0) {
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
bcm_tr_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    bcm_module_t sched_modid, input_modid;
    bcm_port_t sched_port, input_port;
    bcm_trunk_t trunk_id;
    uint32 val;

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport) ||
        !cosq || !input_gport) {
        return BCM_E_PARAM;
    } else if (!_tr_num_port_cosq[unit]) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, sched_gport, &sched_modid,
                                       &sched_port, &trunk_id));
        if (_tr_num_port_cosq[unit][sched_port] == 0) {
            /* GPORT has not been added. */
            return BCM_E_NOT_FOUND;
        }

        BCM_IF_ERROR_RETURN(READ_COS_MODEr(unit, sched_port, &val));
        if (soc_reg_field_get(unit, COS_MODEr, val, SELECTf) == 3) {
            /* GPORT has been attached */
            *cosq = TR_SCHEDULER_COSQ;

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

#ifdef BCM_SCORPION_SUPPORT
STATIC int 
_bcm_sc_cosq_discard_all_disable (int unit)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    soc_reg_t config_reg;
    uint32 val, addr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        /* Cosq level disabling */
        config_reg = WREDCONFIG_CELLr;
        for (cosq = 0; cosq < 8; cosq++) {
             addr = soc_reg_addr(unit, config_reg, port, cosq);
             SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
             soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 0);
             soc_reg_field_set(unit, config_reg, &val, ENABLEf, 0);
             SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
        }

        /* Port level disabling */
        config_reg = PORT_WREDCONFIG_CELLr;
        addr = soc_reg_addr(unit, config_reg, port, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
        soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 0);
        soc_reg_field_set(unit, config_reg, &val, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
    }

    /* Chip level disabling */
    config_reg = GLOBAL_WREDCONFIG_CELLr;
    addr = soc_reg_addr(unit, config_reg, REG_PORT_ANY, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &val));
    soc_reg_field_set(unit, config_reg, &val, CAP_AVERAGEf, 0);
    soc_reg_field_set(unit, config_reg, &val, ENABLEf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr, val));
   
    return BCM_E_NONE;
}

int
bcm_sc_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 kbits_sec_min,
                               uint32 kbits_sec_max,
                               uint32 kbits_sec_burst,
                               uint32 flags)
{
    uint32 regval;
    uint32 bucket_val = 0;
    uint32 refresh_rate, bucketsize, granularity = 3, meter_flags = 0;
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

    if (!IS_CPU_PORT(unit,port)) {
        /* Disable egress metering for this port */
        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG1r(unit, port, cosq, &regval));
        soc_reg_field_set(unit, MAXBUCKETCONFIG1r, &regval, REFRESHf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG1r(unit, port, cosq, regval));

        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, THRESHOLDf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));
    } else {
        BCM_IF_ERROR_RETURN(READ_CPUPKTMAXBUCKETCONFIGr(unit, cosq, &regval));
        soc_reg_field_set(unit, CPUPKTMAXBUCKETCONFIGr,&regval, PKT_MAX_THD_SELf,0);
        soc_reg_field_set(unit, CPUPKTMAXBUCKETCONFIGr,&regval, PKT_MAX_REFRESHf,0);
        BCM_IF_ERROR_RETURN(WRITE_CPUPKTMAXBUCKETCONFIGr(unit, cosq, regval));
    }
    if (!IS_CPU_PORT(unit,port)) {
        BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG1r(unit, port, cosq, &regval));
        soc_reg_field_set(unit, MINBUCKETCONFIG1r, &regval, REFRESHf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MINBUCKETCONFIG1r(unit, port, cosq, regval));

        BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIGr(unit, port, cosq, &regval));
        soc_reg_field_set(unit, MINBUCKETCONFIGr, &regval, THRESHOLDf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MINBUCKETCONFIGr(unit, port, cosq, regval));
        /* Reset the MAXBUCKET register */
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, MAX_BUCKETf, 0);
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETr(unit, port, cosq, bucket_val));

        /*reset the MINBUCKET register value*/
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, MIN_BUCKETf, 0);
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, OUT_PROFILE_FLAGf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MINBUCKETr(unit, port, cosq, bucket_val));

        refresh_bitsize =
            soc_reg_field_length(unit, MINBUCKETCONFIG1r, REFRESHf);
        bucket_bitsize =
            soc_reg_field_length(unit, MINBUCKETCONFIGr, THRESHOLDf);
        BCM_IF_ERROR_RETURN
            (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_min, kbits_sec_min,
                              meter_flags, refresh_bitsize, bucket_bitsize,
                              &refresh_rate, &bucketsize, &granularity));

        regval = 0;
        soc_reg_field_set(unit, MINBUCKETCONFIGr, &regval,
                          METER_GRANf, granularity);
        soc_reg_field_set(unit, MINBUCKETCONFIGr,
                              &regval, THRESHOLDf, bucketsize);
        BCM_IF_ERROR_RETURN
            (WRITE_MINBUCKETCONFIGr(unit, port, cosq, regval));

        regval = 0;
        soc_reg_field_set(unit, MINBUCKETCONFIG1r, &regval,
                              REFRESHf, refresh_rate);
        BCM_IF_ERROR_RETURN
            (WRITE_MINBUCKETCONFIG1r(unit, port, cosq, regval));
    }


    if (!IS_CPU_PORT(unit,port)) {
        refresh_bitsize =
            soc_reg_field_length(unit, MAXBUCKETCONFIG1r, REFRESHf);
        bucket_bitsize =
            soc_reg_field_length(unit, MAXBUCKETCONFIGr, THRESHOLDf);
    } else {
        bucket_bitsize = soc_reg_field_length(unit, CPUPKTMAXBUCKETCONFIGr,
                                              PKT_MAX_THD_SELf);
        refresh_bitsize = soc_reg_field_length(unit, CPUPKTMAXBUCKETCONFIGr,
                                              PKT_MAX_REFRESHf);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max, kbits_sec_burst,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));


    if (!IS_CPU_PORT(unit,port)) {
        regval = 0;
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval,
                          METER_GRANf, granularity);
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, THRESHOLDf,
                          bucketsize);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));

        regval = 0;                                                         
        soc_reg_field_set(unit, MAXBUCKETCONFIG1r, &regval,
                              REFRESHf, refresh_rate);
        BCM_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIG1r(unit, port, cosq, regval));
    } else {
        regval = 0;
        soc_reg_field_set(unit, CPUPKTMAXBUCKETCONFIGr, &regval, PKT_MAX_THD_SELf, 
                                bucketsize);
        soc_reg_field_set(unit, CPUPKTMAXBUCKETCONFIGr, &regval, PKT_MAX_REFRESHf, 
                                refresh_rate);
        BCM_IF_ERROR_RETURN(WRITE_CPUPKTMAXBUCKETCONFIGr(unit, cosq, regval));
    }

    /* 10MISCCONFIG.METERING_CLK_EN is set by chip init */
    return BCM_E_NONE;
}

int
bcm_sc_cosq_port_bandwidth_get(int unit, bcm_port_t port,
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

    if (!kbits_sec_min || !kbits_sec_max || !kbits_sec_burst || !flags) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &regval));
    if (soc_reg_field_get(unit, MISCCONFIGr, regval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_XGS_METER_FLAG_NON_LINEAR;
    }

    if (!IS_CPU_PORT(unit,port)) {
        meter_flags |= _BCM_XGS_METER_FLAG_GRANULARITY;
        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));
        granularity = soc_reg_field_get(unit, MAXBUCKETCONFIGr,
                                        regval, METER_GRANf);
        bucketsize = soc_reg_field_get(unit, MAXBUCKETCONFIGr,
                                       regval, THRESHOLDf);
        BCM_IF_ERROR_RETURN(READ_MAXBUCKETCONFIG1r(unit, port, cosq, &regval));
        refresh_rate = soc_reg_field_get(unit, MAXBUCKETCONFIG1r,
                                         regval, REFRESHf);
    } else {
        BCM_IF_ERROR_RETURN(READ_CPUPKTMAXBUCKETCONFIGr(unit, cosq, &regval));
        bucketsize = soc_reg_field_get(unit, CPUPKTMAXBUCKETCONFIGr,
                                       regval, PKT_MAX_THD_SELf);
        refresh_rate = soc_reg_field_get(unit, CPUPKTMAXBUCKETCONFIGr,
                                         regval, PKT_MAX_REFRESHf);
    }

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, bucketsize,
                                           granularity, meter_flags,
                                           kbits_sec_max, kbits_sec_burst));

    if (!IS_CPU_PORT(unit,port)) {
        BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIGr(unit, port, cosq, &regval));
        granularity = soc_reg_field_get(unit, MINBUCKETCONFIGr,
                                        regval, METER_GRANf);
        BCM_IF_ERROR_RETURN(READ_MINBUCKETCONFIG1r(unit, port, cosq, &regval));
        refresh_rate = soc_reg_field_get(unit, MINBUCKETCONFIG1r,
                                         regval, REFRESHf);
    } 

    BCM_IF_ERROR_RETURN
        (_bcm_xgs_bucket_encoding_to_kbits(refresh_rate, 0,
                                           granularity, meter_flags,
                                           kbits_sec_min, &kbits_min_burst));

    *flags = 0;

    return BCM_E_NONE;
}

int
bcm_sc_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    soc_reg_t config_reg = WREDCONFIG_CELLr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        for (cosq = 0; cosq < 8; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_cosq_discard_cap_enable_set(unit, port, cosq,
                                                     config_reg, flags));
        }
    }
    return BCM_E_NONE;
}

int
bcm_sc_cosq_discard_get(int unit, uint32 *flags)
{
    int rv = BCM_E_NONE;
    bcm_port_t port;
    soc_reg_t config_reg = WREDCONFIG_CELLr;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        *flags = 0;
        rv = _bcm_tr_cosq_discard_cap_enable_get(unit, port, 0,
                                                 config_reg, flags);
        break;
    }
    return rv;
}

int
bcm_sc_cosq_discard_port_set(int unit, bcm_port_t port,
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
    _bcm_gport_dest_t dest;
    bcm_gport_t gport;
    int i, cosq_start, num_cosq;
    uint32 shared_limit;
    uint32 rval, bits;
    bcm_cosq_gport_discard_t discard;

    if (!_tr_num_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }

    if (cosq < -1 || cosq >= _tr_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        cosq_start = 0;
        num_cosq = _tr_num_cosq[unit];
    } else {
        cosq_start = cosq;
        num_cosq = 1;
    }

    /* Init structure */
    bcm_cosq_gport_discard_t_init(&discard);

    /* BCM_COSQ_DISCARD_ENABLE & BCM_COSQ_DISCARD_CAP_AVERAGE
     * from current state */
    bcm_sc_cosq_discard_get(unit, &(discard.flags));

    discard.flags |= color;

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
    discard.gain = _shr_popcount(bits);
    /* Don't alow negative value of gain */
    if (discard.gain) {
        discard.gain -= 1;
    }

    /*
     * per-port cell limit may be disabled.
     * per-port per-cos cell limit may be set to use dynamic method.
     * therefore drop_start percentage is based on per-device total shared
     * cells.
     */
    SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMITr(unit, &rval));
    shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMITr,
                                     rval, OP_BUFFER_SHARED_LIMITf);
    discard.min_thresh = drop_start * shared_limit / 100;

    /* Calculate the max threshold. For a given slope (angle in 
     * degrees), determine how many cells are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_threshold for the the max_threshold.
     */

    /* Because SC_CELL_FIELD_MAX == TR_PACKET_FIELD_MAX(unit),
     * we can use the TR packet converter as our cell converter */
    discard.max_thresh = discard.min_thresh +
        _bcm_tr_angle_to_packets(drop_slope);
    if (discard.max_thresh > SC_CELL_FIELD_MAX) {
        discard.max_thresh = SC_CELL_FIELD_MAX;
    }

    /* Put thresholds in units of bytes */
    discard.min_thresh *= _BCM_TR_COSQ_CELLSIZE;
    discard.max_thresh *= _BCM_TR_COSQ_CELLSIZE;
    discard.flags |= BCM_COSQ_DISCARD_BYTES;

    /* We use drop probabily of 1 here.  Use bcm_cosq_gport_discard_set
     * for more flexibility. */
    discard.drop_probability = 100;

    BCM_PBMP_ITER(pbmp, local_port) {
        _bcm_gport_dest_t_init(&dest);
        dest.port = local_port;
        dest.gport_type = _SHR_GPORT_TYPE_LOCAL;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &gport));
        for (i = cosq_start; i < (cosq_start + num_cosq); i++) { 
            BCM_IF_ERROR_RETURN
                (bcm_tr_cosq_gport_discard_set(unit, gport, i, &discard));
        }
    }
    return BCM_E_NONE;
}

int
bcm_sc_cosq_discard_port_get(int unit, bcm_port_t port,
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
    _bcm_gport_dest_t dest;
    bcm_gport_t gport;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval;
    bcm_cosq_gport_discard_t discard;

    if (!_tr_num_cosq[unit]) {
        return BCM_E_INIT;
    }

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr_cosq_resolve_mod_port(unit, port, &modid,
                                           &local_port, &trunk_id));
        BCM_PBMP_PORT_SET(pbmp, local_port);
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
    }

    if (cosq < -1 || cosq >= _tr_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        cosq = 0;
    }

    BCM_PBMP_ITER(pbmp, local_port) {
        _bcm_gport_dest_t_init(&dest);
        dest.port = local_port;
        dest.gport_type = _SHR_GPORT_TYPE_LOCAL;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &gport));

        /* Init structure */
        bcm_cosq_gport_discard_t_init(&discard);

        /* Prepare input flags */
        /* Convert color to flags */
        discard.flags |= color;
        discard.flags |= BCM_COSQ_DISCARD_BYTES;

        BCM_IF_ERROR_RETURN
            (bcm_tr_cosq_gport_discard_get(unit, gport, cosq, &discard));

        /*
         * average queue size is reculated every 4us, the formula is
         * avg_qsize(t + 1) =
         *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
         */
        *average_time = (1 << discard.gain) * 4;

        /* Change bytes to cells for the following calculations */
        min_thresh = discard.min_thresh / _BCM_SC_COSQ_CELLSIZE;
        max_thresh = discard.max_thresh / _BCM_SC_COSQ_CELLSIZE;

        /*
         * per-port cell limit may be disabled.
         * per-port per-cos cell limit may be set to use dynamic method.
         * therefore drop_start percentage is based on per-device total shared
         * cells.
         */
        SOC_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMITr(unit, &rval));
        shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMITr,
                                         rval, OP_BUFFER_SHARED_LIMITf);
        *drop_start = min_thresh * 100 / shared_limit;

        /* Calculate the slope using the min and max threshold.
         * The angle is calculated knowing drop probability at min
         * threshold is 0% and drop probability at max threshold is 100%.
         */
        *drop_slope = _bcm_tr_packets_to_angle(max_thresh - min_thresh);
        break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_sc_cosq_gport_sched_set
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
bcm_sc_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;
    int i;
    bcm_cos_queue_t cosq_start = 0;
    int num_weights = 1, weights[16];

    /* Note, we use the TR functions here, but they work for Scorpion */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));

    if (cosq >= _tr_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq < 0) {
        cosq_start = 0;
        num_weights = 8;
        for (i = 0; i < num_weights; i++) {
            if (i < _tr_num_cosq[unit]) {
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

    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_port_sched_set(unit, ESCONFIGr, COSWEIGHTSr,
                                     MINSPCONFIGr, local_port,
                                     cosq_start, num_weights,
                                     weights, mode));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_sc_cosq_gport_sched_get
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
bcm_sc_cosq_gport_sched_get(int unit, bcm_gport_t gport, 
                            bcm_cos_queue_t cosq, int *mode, int *weight)
{   
    bcm_module_t modid;
    bcm_port_t local_port;
    bcm_trunk_t trunk_id;

    *mode = *weight = 0;

    /* Note, we use the TR functions here, but they work for Scorpion */
    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_resolve_mod_port(unit, gport, &modid,
                                       &local_port, &trunk_id));
    if (cosq >= _tr_num_cosq[unit]) {
        return BCM_E_PARAM;
    } else if (cosq < 0) {
        cosq = 0;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_tr_cosq_port_sched_get(unit, ESCONFIGr, COSWEIGHTSr, 
                                     local_port, cosq, 1,
                                     weight, mode));
    return BCM_E_NONE;
}

#endif /* BCM_SCORPION_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_tr_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr_cosq_sw_dump(int unit)
{
    int  i;
    int  port_count;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));

    /* Number COSQ */
    LOG_CLI((BSL_META_U(unit,
                        "    Number: %d\n"), _tr_num_cosq[unit]));

    /* Number of COSQs per port */
    port_count = _bcm_tr_cosq_port_count_get(unit);
    LOG_CLI((BSL_META_U(unit,
                        "    Number COSQ per port:\n")));
    if (_tr_num_port_cosq[unit] != NULL) {
        for (i = 0; i < port_count; i++) {
            LOG_CLI((BSL_META_U(unit,
                                "        Port %2d: %d\n"), i,
                     _tr_num_port_cosq[unit][i]));
        }
    }

    /* COSQ Map profile */
    /* Display those entries with reference count > 0 */
    LOG_CLI((BSL_META_U(unit,
                        "    COSQ Map Profile:\n")));
    if (_tr_cos_map_profile[unit] != NULL) {
        int     num_entries;
        int     ref_count;
        int     entries_per_set;
        uint32  index;
        port_cos_map_entry_t *entry_p;
        uint32  cosq, hg_cosq;

        num_entries = soc_mem_index_count
            (unit, _tr_cos_map_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            "        Number of entries: %d\n"), num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "        Index RefCount EntriesPerSet - "
                 "COS HG_COS\n")));

        for (index = 0; index < num_entries;
             index += TR_COS_MAP_ENTRIES_PER_SET) {
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               _tr_cos_map_profile[unit],
                                               index, &ref_count))) {
                break;
            }

            if (ref_count <= 0) {
                continue;
            }

            for (i = 0; i < TR_COS_MAP_ENTRIES_PER_SET; i++) {
                entries_per_set =
                    _tr_cos_map_profile[unit]->tables[0].entries[index+i].entries_per_set;
                LOG_CLI((BSL_META_U(unit,
                                    "       %5d %8d %13d    "),
                         index + i, ref_count, entries_per_set));

                entry_p = SOC_PROFILE_MEM_ENTRY(unit,
                                                _tr_cos_map_profile[unit],
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
#endif /*  BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_TRX_SUPPORT */
typedef int _tr_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRX_SUPPORT */

